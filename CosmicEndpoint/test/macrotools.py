import subprocess
import math
import os
import shutil

# copied from John Hakala's bhmacros and modified for quick use here
def callMacro(macroName, inputFile, outputFile):
	macroCommand = "%s(\"%s\", \"%s\")" % (macroName, inputFile, outputFile)
	subprocess.call(["root", "-l", "-q", macroCommand])

def splitJobsForBsub(inputFile, numberOfJobs, maxBias, symasym):
	samplesListsDir="samplesLists_data"
	inputFilePath = samplesListsDir+"/"+inputFile
	num_input_samples = sum(1 for line in open(inputFilePath))
	with open(inputFilePath) as inputSamplesList:
		print "number of input samples is %i" % num_input_samples
		chunksize = math.ceil(1.0*num_input_samples/numberOfJobs)
		print "chunksize is: %i" % chunksize
		fid = 1
		lineswritten = 0
		f = open(samplesListsDir+"/splitLists%.2f/"%(1000*maxBias)+'split_%i_%s_%s'%(fid, symasym,
											     inputFile), 'w')
		for i,line in enumerate(inputSamplesList):
			#print "%i : \n      %s" % (i, line)
			f.write(line)
			lineswritten+=1
			if lineswritten == chunksize:# and fid < numberOfJobs:
				f.close()
				fid += 1
				lineswritten = 0
				f = open(samplesListsDir+"/splitLists%.2f/"%(1000*maxBias)+'split_%i_%s_%s'%(fid, symasym,
													     inputFile), 'w')
		f.close()
		return fid

def bSubSplitJobs(pyScriptName, outputFile, inputFile, proxyPath, numberOfJobs, maxBias, symmetric, debug):
	symasym = "asym"
	if symmetric:
		symasym = "sym"

	samplesListsDir="samplesLists_data"

	if not os.path.exists("/tmp/sturdy/output%.2f"%(1000*maxBias)):
		os.makedirs("/tmp/sturdy/output%.2f"%(1000*maxBias))

	clearSplitLists(maxBias, symasym)
	clearBsubShellScripts(maxBias, symasym)
	nJobs = splitJobsForBsub(inputFile, numberOfJobs, maxBias, symasym)
	print "Prepared %i jobs ready to be submitted to bsub." % nJobs
	for i in range (1, nJobs+1):
		splitListFile="split_%i_%s_%s" % (i, symasym, inputFile)
		rootScriptName = "root-%s-%.2f_%s_%d.C" % (pyScriptName, 1000*maxBias, symasym, i)
		f = open(rootScriptName, "w")
		f.write("{\n")
		#f.write("  gROOT->ProcessLine(\" .L %s/Plot.so\");\n"%(os.getcwd()))
		inputFileList = samplesListsDir + "/splitLists%.2f/"%(1000*maxBias) + splitListFile
		f.write("  gROOT->ProcessLine(\" .L Plot.so\");\n")
		##the first execution seems to clear the proxy error
		
		# f.write("  Plot(\"%s\",\"/tmp/sturdy/output%.2f/%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
		f.write("  Plot(\"%s\",\"${OUTPUTDIR}/%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
											       symasym,outputFile,i,
											       1,
											       50.,maxBias,1000,
											       1000., symmetric))
		for tk in range(5):
			# f.write("  Plot(\"%s\",\"/tmp/sturdy/output%.2f/%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
			f.write("  Plot(\"%s\",\"${OUTPUTDIR}/%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
												       symasym,outputFile,i,
												       tk+1,
												       50.,maxBias,1000,
												       1000., symmetric))
		f.write("}\n")
		# root -x -b -q  put this in the shell script
		pyCommand = "%s"%(rootScriptName)
		makeBsubShellScript(pyCommand, samplesListsDir+"/splitLists%.2f/"%(1000*maxBias)+splitListFile,
				    pyScriptName, i, proxyPath, maxBias, symasym, debug)

def makeBsubShellScript(pyCommand, splitListName, pyScriptName, index, proxyPath, maxBias, symasym, debug):
	subfile = "%s/bsubs%.2f/bsub-%s-%s-%s.sh"%( os.getcwd(),1000*maxBias,pyScriptName, symasym, index)
	logfile = "%s/bsubs%.2f/bsub-%s-%s-%s.log"%(os.getcwd(),1000*maxBias,pyScriptName, symasym, index)
	f = open(subfile, "w")
	f.write("""#!/bin/bash
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
export X509_USER_PROXY=%s

echo "hostname is $HOSTNAME" #|& tee %s
export JOBDIR=${PWD}
echo "batch job directory is ${JOBDIR}" #| tee -a %s
export OUTPUTDIR=${JOBDIR}/output%.2f
echo "output directory is ${OUTPUTDIR}" #| tee -a %s
mkdir ${OUTPUTDIR}
ls -tar #|tee -a %s

cd %s
export AFSJOBDIR=${PWD}
eval `scramv1 runtime -sh`
cp Plot* ${JOBDIR}/
cp %s ${JOBDIR}/
cd ${JOBDIR}
ls -tar #|tee -a %s
root -b -q -x %s
tee #|tee -a %s
rsync -aAXch --progress ${OUTPUTDIR} ${HOSTNAME}:/tmp/${USER}/
"""%(proxyPath,
     logfile,logfile,
     1000*maxBias,
     logfile,logfile,
     os.getcwd(),
     pyCommand,logfile,
     pyCommand,logfile))
	f.close()
	os.chmod(subfile, 0777)
	# cmd = "bsub -q test -W 0:20 %s/%s"%(subfile) #submit to the test queue if debug (only a single job though
	cmd = "bsub -q 8nm -W 0:10 %s"%(subfile)
	print cmd
	if not debug:
		os.system(cmd)

def clearSplitLists(maxBias, symasym):
	samplesListsDir="samplesLists_data"
	splitListsDir=samplesListsDir+"/splitLists%.2f/"%(1000*maxBias)

	if not os.path.exists(splitListsDir):
		os.makedirs(splitListsDir)

	for the_file in os.listdir(splitListsDir):
		file_path = os.path.join(splitListsDir, the_file)
		if (file_path.find("_"+symasym+"_") > 0):
			try:
				if os.path.isfile(file_path):
					os.unlink(file_path)
				# elif os.path.isdir(file_path): shutil.rmtree(file_path)
			except Exception, e:
				print e

def clearBsubShellScripts(maxBias, symasym):
	bSubScriptsDir="bsubs%.2f/"%(1000*maxBias)
        #d = os.path.dirname(bSubScriptsDir)
        if not os.path.exists(bSubScriptsDir):
                os.makedirs(bSubScriptsDir)

	for the_file in os.listdir(bSubScriptsDir):
		file_path = os.path.join(bSubScriptsDir, the_file)
		if (file_path.find("-"+symasym+"-") > 0):
			try:
				if os.path.isfile(file_path):
					os.unlink(file_path)
				# elif os.path.isdir(file_path): shutil.rmtree(file_path)
			except Exception, e:
				print e
