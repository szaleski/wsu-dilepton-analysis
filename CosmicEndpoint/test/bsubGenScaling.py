#!/bin/env python

import subprocess
import math
import sys,os,socket
import shutil

from optparse import OptionParser
from wsuPythonUtils import checkRequiredArguments

parser = OptionParser()
parser.add_option("-g", "--gridproxy", action="store_true", dest="gridproxy",
                  metavar="gridproxy",
                  help="[OPTIONAL] Generate a GRID proxy")
parser.add_option("-i", "--infiles", type="string", dest="infiles",
                  metavar="infiles",
                  help="[REQUIRED] Text file with list of input files to process ")
parser.add_option("-t", "--title", type="string", dest="title",
                  metavar="title",
                  help="[REQUIRED] Task title ")
parser.add_option("-x", "--tool", type="string", dest="tool",
                  metavar="tool", default="runGenL1Studies",
                  help="[REQUIRED] Tool name (default is runGenL1Studies)")
parser.add_option("-m", "--mc", action="store_true", dest="mc",
                  metavar="mc",
                  help="[OPTIONAL] Running ananalysis on MC")
parser.add_option("-d", "--debug", action="store_true", dest="debug",
                  metavar="debug",
                  help="[OPTIONAL] Run in debug mode, i.e., don't submit jobs, just create them")

(options, args) = parser.parse_args()
checkRequiredArguments(options, parser)

if options.gridproxy:
    os.system("voms-proxy-init --voms cms --valid 168:00")

debug = False    
if options.debug:
    debug = True
username = os.getenv("USER")
userkey  = os.getenv("KRB5CCNAME")[17:17+5] #should be the form of FILE:/tmp/krb5cc_17329_lNizN17453
print username, userkey
proxyPath = "/afs/cern.ch/user/%s/%s/x509up_u%s"%(username[0],username,userkey)
cmd = "cp /tmp/x509up_u%s  %s"%(userkey,proxyPath)
print cmd
os.system(cmd)
count = 0

isMC = ""
if options.mc:
    isMC = "-m"

for line in open(options.infiles):
    scriptname = "genScaling/bsub_gen_%s_%s_job_%d.sh"%(options.title,options.tool,count)
    script = open(scriptname,"w")
    script.write("""#!/bin/bash
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
export X509_USER_PROXY=%s

kinit -R
aklog
klist

echo "hostname is $HOSTNAME"
export JOBDIR=${PWD}
echo "batch job directory is ${JOBDIR}"
export OUTPUTDIR=${JOBDIR}/output_%s_%s
echo "output directory is ${OUTPUTDIR}"
mkdir ${OUTPUTDIR}
ls -tar

cd %s
export AFSJOBDIR=${PWD}
eval `scramv1 runtime -sh`
cp %s.py ${JOBDIR}/
cp ../python/wsuPythonUtils.py ${JOBDIR}/
cp ../python/wsuPyROOTUtils.py ${JOBDIR}/
cp ../python/wsuMuonTreeUtils.py ${JOBDIR}/
cp ../../MuonAnalyzer/python/l1TrigInfo.py ${JOBDIR}/
cp ../../MuonAnalyzer/python/getMCScaleFactors.py ${JOBDIR}/
cp runGenL1Studies.py ${JOBDIR}/
cd ${JOBDIR}
export PATH=${PWD}:${PATH}
export PYTHONPATH=${PWD}:${PYTHONPATH}
ls -tar
xrdcp -d 1 -f root://cms-xrd-global.cern.ch//%s ${PWD}/local_copy_job_%d.root
./%s.py -i ${PWD}/local_copy_job_%d.root %s -o ${OUTPUTDIR}/gen_scaling_%s_job_%d
#./%s.py -i root://cms-xrd-global.cern.ch//%s %s -o ${OUTPUTDIR}/gen_scaling_%s_job_%d
tree
echo "rsync \\"ssh -T -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null\\" -aAXch --progress ${OUTPUTDIR} %s:/tmp/${USER}/"
rsync -e "ssh -T -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" -aAXch --progress ${OUTPUTDIR} %s:/tmp/${USER}/
"""%(proxyPath,
     options.title,options.tool,
     os.getcwd(),
     options.tool,
     line[:-1],count,
     options.tool,count,isMC,options.title,count,
     options.tool,line[:-1],isMC,options.title,count,
     socket.gethostname(),socket.gethostname()))

    script.close()
    os.chmod(scriptname,0777)

    if not debug:
        cmd = "bsub -q 8nh -W 240 %s/%s"%(os.getcwd(),scriptname)
        print cmd
        os.system(cmd)
    elif count == 3:
        cmd = "bsub -q test -W 240 %s/%s"%(os.getcwd(),scriptname)
        print cmd
        os.system(cmd)

    count = count + 1
