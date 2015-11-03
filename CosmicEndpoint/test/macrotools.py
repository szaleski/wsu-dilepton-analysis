import subprocess
import math
import os
import shutil

# copied from John Hakala's bhmacros and modified for quick use here
def callMacro(macroName, inputFile, outputFile):
	macroCommand = "%s(\"%s\", \"%s\")" % (macroName, inputFile, outputFile)
	subprocess.call(["root", "-l", "-q", macroCommand])

def splitJobsForBsub(inputFile, numberOfJobs):
	samplesListsDir="samplesLists_data"
	inputFilePath = samplesListsDir+"/"+inputFile
	num_input_samples = sum(1 for line in open(inputFilePath))
	with open(inputFilePath) as inputSamplesList:
		print "number of input samples is %i" % num_input_samples
		chunksize = math.ceil(1.0*num_input_samples/numberOfJobs)
		print "chunksize is: %i" % chunksize
		fid = 1
		lineswritten = 0
		f = open(samplesListsDir+"/splitLists/"+'split_%i_%s'%(fid, inputFile), 'w')
		for i,line in enumerate(inputSamplesList):
			#print "%i : \n      %s" % (i, line)
			f.write(line)
			lineswritten+=1
			if lineswritten == chunksize:# and fid < numberOfJobs:
				f.close()
				fid += 1
				lineswritten = 0
				f = open(samplesListsDir+"/splitLists/"+'split_%i_%s'%(fid, inputFile), 'w')
		f.close()
		return fid

def bSubSplitJobs(pyScriptName, outputFile, inputFile, proxyPath, numberOfJobs):
	samplesListsDir="samplesLists_data"

	if not os.path.exists("output"):
		os.makedirs("output")

	clearSplitLists()
	clearBsubShellScripts()
	nJobs = splitJobsForBsub(inputFile, numberOfJobs)
	print "Prepared %i jobs ready to be submitted to bsub." % nJobs
	for i in range (1, nJobs+1):
		splitListFile="split_%i_%s" % (i , inputFile)
		rootScriptName = "root-%s-%s.C" % (pyScriptName, i)
		f = open(rootScriptName, "w")
		f.write("{\n")
		#f.write("  gROOT->ProcessLine(\" .L %s/Plot.so\");\n"%(os.getcwd()))
		inputFileList = samplesListsDir + "/splitLists/" + splitListFile
		f.write("  gROOT->ProcessLine(\" .L Plot.so\");\n")
		##the first execution seems to clear the proxy error
		f.write("  Plot(\"%s\",\"output/%s_%d_\",%d, %f, %f, %d, %f);\n"%(inputFileList,
										  outputFile, i, 1,
										  50.,0.005,500,1000.))
		for tk in range(1,5):
			f.write("  Plot(\"%s\",\"output/%s_%d_\",%d, %f, %f, %d, %f);\n"%(inputFileList,
											  outputFile, i, tk+1,
											  50.,0.005,500,1000.))
		f.write("}\n")
		pyCommand = "root -x -b -q %s"%(rootScriptName)
		makeBsubShellScript(pyCommand, samplesListsDir+"/splitLists/"+splitListFile, pyScriptName, i, proxyPath)

def makeBsubShellScript(pyCommand, splitListName, pyScriptName, index, proxyPath):
	subfile = "bsubs/bsub-%s-%s.sh" % (pyScriptName, index)
	f = open(subfile, "w")
	f.write("""#!/bin/bash
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
#source /cvmfs/cms.cern.ch/cmsset_default.sh
export X509_USER_PROXY=%s
cd %s
alias cmsenv='eval `scramv1 runtime -sh`'
cmsenv
eval `scramv1 runtime -sh`
%s
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
"""%(proxyPath, os.getcwd(), pyCommand))
	f.close()
	os.chmod(subfile, 0777)
	cmd = "bsub -q 1nh %s/%s"%(os.getcwd(),subfile)
	print cmd
	#os.system(cmd)

def clearSplitLists():
	samplesListsDir="samplesLists_data"
	splitListsDir=samplesListsDir+"/splitLists/"

	if not os.path.exists(splitListsDir):
		os.makedirs(splitListsDir)

	for the_file in os.listdir(splitListsDir):
		file_path = os.path.join(splitListsDir, the_file)
		try:
			if os.path.isfile(file_path):
				os.unlink(file_path)
			#elif os.path.isdir(file_path): shutil.rmtree(file_path)
		except Exception, e:
			print e

def clearBsubShellScripts():
	bSubScriptsDir="bsubs/"
        #d = os.path.dirname(bSubScriptsDir)
        if not os.path.exists(bSubScriptsDir):
                os.makedirs(bSubScriptsDir)

	for the_file in os.listdir(bSubScriptsDir):
		file_path = os.path.join(bSubScriptsDir, the_file)
		try:
			if os.path.isfile(file_path):
				os.unlink(file_path)
			#elif os.path.isdir(file_path): shutil.rmtree(file_path)
		except Exception, e:
			print e
