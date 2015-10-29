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
		#print "number of input samples is %i" % num_input_samples
		chunksize = num_input_samples / (numberOfJobs-1)
		print "chunksize is: %i" % chunksize
		fid = 1
		lineswritten = 0
		f = open(samplesListsDir+"/splitLists/"+'split_%i_%s'%(fid, inputFile), 'w')
		for i,line in enumerate(inputSamplesList):
			#print "%i : \n      %s" % (i, line)
			f.write(line)
			lineswritten+=1
			if lineswritten == chunksize and fid<numberOfJobs:
				f.close()
				fid += 1
				lineswritten = 0
				f = open(samplesListsDir+"/splitLists/"+'split_%i_%s'%(fid, inputFile), 'w')
		f.close()
		return fid

def bSubSplitJobs(pyScriptName, configFile, inputFile, numberOfJobs):
	samplesListsDir="samplesLists_data"

	if not os.path.exists("output"):
		os.makedirs("output")

	clearSplitLists()
	clearBsubShellScripts()
	nJobs = splitJobsForBsub(inputFile, numberOfJobs)
	print "Prepared %i jobs ready to be submitted to bsub." % nJobs
	for i in range (1, nJobs+1):
		splitListFile="split_%i_%s" % (i , inputFile)
		pyCommand = "runEndpoint -i " + samplesListsDir + "/splitLists/" + splitListFile + " " + " -o output/" + pyScriptName + "-output_%i" %i + " -c " + configFile
		makeBsubShellScript(pyCommand, samplesListsDir+"/splitLists/"+splitListFile, pyScriptName, i)

def makeBsubShellScript(pyCommand, splitListName, pyScriptName, index):
	f = open("bsubs/bsub-%s-%s.sh" % (pyScriptName, index), "w")
	f.write("#!/bin/bash\n")
	f.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
	f.write("export X509_USER_PROXY=/afs/cern.ch/user/s/sturdy/x509up_u17329")
	f.write("cd " + os.getcwd()+"\n")
	f.write("cmsenv\n")
	f.write("export PYTHONPATH=$PYTHONPATH:$CMSSW_BASE/src/WSUDiLeptons/CosmicEndpoint/test\n")
	f.write("export PATH=$PATH:$CMSSW_BASE/test/$SCRAM_ARCH\n")
	f.write(pyCommand)
	f.close()
	os.chmod("bsubs/bsub-%s-%s.sh" % (pyScriptName, index), 0777)

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
