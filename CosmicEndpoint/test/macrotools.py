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
		
		f.write("  Plot(\"%s\",\"/tmp/sturdy/output%.2f/%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
													 1000*maxBias,
													 symasym,outputFile,i,
													 1,
													 50.,maxBias,1000,
													 1000., symmetric))
		for tk in range(5):
			f.write("  Plot(\"%s\",\"/tmp/sturdy/output%.2f/%s_%s_%d_\",%d, %f, %f, %d, %f, %d);\n"%(inputFileList,
														 1000*maxBias,
														 symasym,outputFile,i,
														 tk+1,
														 50.,maxBias,1000,
														 1000., symmetric))
		f.write("}\n")
		pyCommand = "root -x -b -q %s"%(rootScriptName)
		makeBsubShellScript(pyCommand, samplesListsDir+"/splitLists%.2f/"%(1000*maxBias)+splitListFile,
				    pyScriptName, i, proxyPath, maxBias, symasym, debug)

def makeBsubShellScript(pyCommand, splitListName, pyScriptName, index, proxyPath, maxBias, symasym, debug):
	subfile = "bsubs%.2f/bsub-%s-%s-%s.sh"%(1000*maxBias,pyScriptName, symasym, index)
	f = open(subfile, "w")
	f.write("""#!/bin/bash
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
export X509_USER_PROXY=%s
cd %s
mkdir /tmp/sturdy/output%.2f
eval `scramv1 runtime -sh`
%s
rsync -aAXch --progress /tmp/sturdy/output%.2f lxplus0104:/tmp/sturdy/
#### here do some cleanup, e.g., remove input scripts, remove split lists, remove bsub script
### but only if the bsub job exited cleanly
##jobfailed = $(fgrep Killed "LSFJOB_$jobid/STDOUT")
##if [jobfailed];
##then
##    #do nothing
##else
##    rm root.C script
##    rm bsubs/bsub-script
##    rm -rf LSFJOB_$jobid
##fi
"""%(proxyPath, os.getcwd(), 1000*maxBias, pyCommand, 1000*maxBias))
	f.close()
	os.chmod(subfile, 0777)
	cmd = "bsub -q 1nh %s/%s"%(os.getcwd(),subfile)
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
