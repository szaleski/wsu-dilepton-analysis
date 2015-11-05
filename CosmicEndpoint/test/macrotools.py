import subprocess
import math
import os,socket
import shutil

# copied from John Hakala's bhmacros and modified for quick use here
def callMacro(macroName, inputFile, outputFile):
	macroCommand = "%s(\"%s\", \"%s\")" % (macroName, inputFile, outputFile)
	subprocess.call(["root", "-l", "-q", macroCommand])

def splitJobsForBsub(inputFile, numberOfJobs, maxBias, minPt, nBiasBins, symasym):
	samplesListsDir="samplesLists_data"
	inputFilePath = samplesListsDir+"/"+inputFile
	num_input_samples = sum(1 for line in open(inputFilePath))
	with open(inputFilePath) as inputSamplesList:
		print "number of input samples is %i" % num_input_samples
		chunksize = math.ceil(1.0*num_input_samples/numberOfJobs)
		print "chunksize is: %i" % chunksize
		fid = 1
		lineswritten = 0
		f = open("%s/splitLists_b%.2f_pt%2.0f_n%d/split_%i_%s_%s"%(samplesListsDir,
									 1000*maxBias,
									 minPt,nBiasBins,
									 fid, symasym,
									 inputFile), 'w')
		for i,line in enumerate(inputSamplesList):
			#print "%i : \n      %s" % (i, line)
			f.write(line)
			lineswritten+=1
			if lineswritten == chunksize:# and fid < numberOfJobs:
				f.close()
				fid += 1
				lineswritten = 0
				f = open("%s/splitLists_b%.2f_pt%2.0f_n%d/split_%i_%s_%s"%(samplesListsDir,
											 1000*maxBias,
											 minPt,nBiasBins,
											 fid, symasym,
											 inputFile), 'w')
		f.close()
		return fid

def bSubSplitJobs(pyScriptName, outputFile, inputFile, proxyPath, numberOfJobs,
		  maxBias, minPt, nBiasBins, symmetric, debug):
	symasym = "asym"
	if symmetric:
		symasym = "sym"

	samplesListsDir="samplesLists_data"

	if not os.path.exists("/tmp/%s/output_b%.2f_pt%2.0f_n%d"%(os.getlogin(),1000*maxBias,minPt,nBiasBins)):
		os.makedirs("/tmp/%s/output_b%.2f_pt%2.0f_n%d"%(  os.getlogin(),1000*maxBias,minPt,nBiasBins))

	rootScriptDir = "bsubs_b%.2f_pt%2.0f_n%d/roots"%(1000*maxBias,minPt,nBiasBins)
	if not os.path.exists(rootScriptDir):
		os.makedirs(rootScriptDir)

	clearSplitLists(maxBias, minPt, nBiasBins, symasym)
	clearBsubShellScripts(maxBias, minPt, nBiasBins, symasym)
	nJobs = splitJobsForBsub(inputFile, numberOfJobs, maxBias, minPt, nBiasBins, symasym)
	print "Prepared %i jobs ready to be submitted to bsub." % nJobs
	for i in range (1, nJobs+1):
		splitListFile="split_%i_%s_%s" % (i, symasym, inputFile)
		rootScriptName = "root-%s-_b%.2f_pt%2.0f_n%d_%s_%d.C"%(pyScriptName, 1000*maxBias,minPt,nBiasBins, symasym, i)
		f = open("%s/%s"%(rootScriptDir,rootScriptName), "w")
		f.write("{\n")
		#f.write("  gROOT->ProcessLine(\" .L %s/Plot.so\");\n"%(os.getcwd()))
		inputFileList = samplesListsDir + "/splitLists_b%.2f_pt%2.0f_n%d/"%(1000*maxBias,minPt,nBiasBins) + splitListFile
		f.write("  gROOT->ProcessLine(\" .L Plot.so\");\n")
		##the first execution seems to clear the proxy error
		
		f.write("  Plot(\"%s\",\"%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
										  symasym,outputFile,i,
										  1,
										  minPt,maxBias,nBiasBins,
										  1000., symmetric))
		for tk in range(5):
			f.write("  Plot(\"%s\",\"%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
											  symasym,outputFile,i,
											  tk+1,
											  minPt,maxBias,nBiasBins,
											  1000., symmetric))
		f.write("}\n")
		# root -x -b -q  put this in the shell script
		pyCommand = "%s"%(rootScriptName)
		makeBsubShellScript(pyCommand, rootScriptDir, "%s/splitLists_b%.2f_pt%2.0f_n%d/%s"%(samplesListsDir,1000*maxBias,
												  minPt,nBiasBins,splitListFile),
				    pyScriptName, i, proxyPath, maxBias, minPt, nBiasBins, symasym, debug)

def makeBsubShellScript(pyCommand, rootScriptDir, splitListName, pyScriptName, index, proxyPath,
			maxBias, minPt, nBiasBins, symasym, debug):
	subfile = "%s/bsubs_b%.2f_pt%2.0f_n%d/bsub-%s-%s-%s.sh"%( os.getcwd(),1000*maxBias,minPt,nBiasBins,pyScriptName, symasym, index)
	logfile = "%s/bsubs_b%.2f_pt%2.0f_n%d/bsub-%s-%s-%s.log"%(os.getcwd(),1000*maxBias,minPt,nBiasBins,pyScriptName, symasym, index)
	f = open(subfile, "w")
	f.write("""#!/bin/bash
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
export X509_USER_PROXY=%s

echo "hostname is $HOSTNAME" #|& tee %s
export JOBDIR=${PWD}
echo "batch job directory is ${JOBDIR}" #| tee -a %s
export OUTPUTDIR=${JOBDIR}/output_b%.2f_pt%2.0f_n%d
echo "output directory is ${OUTPUTDIR}" #| tee -a %s
mkdir ${OUTPUTDIR}
ls -tar #|tee -a %s

cd %s
export AFSJOBDIR=${PWD}
eval `scramv1 runtime -sh`
cp Plot* ${JOBDIR}/
cp %s/%s ${JOBDIR}/
cd ${JOBDIR}
ls -tar #|tee -a %s
root -b -q -x %s
tee #|tee -a %s
rsync -aAXch --progress ${OUTPUTDIR} %s:/tmp/${USER}/
"""%(proxyPath,
     logfile,logfile,
     1000*maxBias,minPt,nBiasBins,
     logfile,logfile,
     os.getcwd(),
     rootScriptDir,pyCommand,logfile,
     pyCommand,logfile,
     socket.gethostname()))
	f.close()
	os.chmod(subfile, 0777)
	# cmd = "bsub -q test -W 0:20 %s/%s"%(subfile) #submit to the test queue if debug (only a single job though
	cmd = "bsub -q 8nm -W 0:10 %s"%(subfile)
	print cmd
	if not debug:
		os.system(cmd)

def clearSplitLists(maxBias, minPt, nBiasBins, symasym):
	samplesListsDir="samplesLists_data"
	splitListsDir=samplesListsDir+"/splitLists_b%.2f_pt%2.0f_n%d/"%(1000*maxBias,minPt,nBiasBins)

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

def clearBsubShellScripts(maxBias, minPt, nBiasBins, symasym):
	bSubScriptsDir="bsubs_b%.2f_pt%2.0f_n%d/"%(1000*maxBias,minPt,nBiasBins)
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
