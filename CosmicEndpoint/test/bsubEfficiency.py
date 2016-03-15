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

for line in open(options.infiles):
    scriptname = "efficiencyScripts/bsub_eff_%s_job_%d.sh"%(options.title,count)
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
export OUTPUTDIR=${JOBDIR}/output_%s
echo "output directory is ${OUTPUTDIR}"
mkdir ${OUTPUTDIR}
ls -tar

cd %s
export AFSJOBDIR=${PWD}
eval `scramv1 runtime -sh`
cp efficiencyStudy.py ${JOBDIR}/
cp ../python/wsuPythonUtils.py ${JOBDIR}/
cd ${JOBDIR}
export PATH=${PWD}:${PATH}
export PYTHONPATH=${PWD}:${PYTHONPATH}
ls -tar
./efficiencyStudy.py -i root://xrootd.unl.edu//%s -o ${OUTPUTDIR}/eff_lower_%s_job_%d
./efficiencyStudy.py -i root://xrootd.unl.edu//%s --tight -o ${OUTPUTDIR}/eff_lower_tight_%s_job_%d
./efficiencyStudy.py -i root://xrootd.unl.edu//%s -u -o ${OUTPUTDIR}/eff_upper_%s_job_%d
./efficiencyStudy.py -i root://xrootd.unl.edu//%s -u --tight -o ${OUTPUTDIR}/eff_upper_tight_%s_job_%d
tree
echo "rsync \\"ssh -T -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null\\" -aAXch --progress ${OUTPUTDIR} %s:/tmp/${USER}/"
rsync -e "ssh -T -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" -aAXch --progress ${OUTPUTDIR} %s:/tmp/${USER}/
"""%(proxyPath,
     options.title,
     os.getcwd(),
     line[:-6],options.title,count,
     line[:-6],options.title,count,
     line[:-6],options.title,count,
     line[:-6],options.title,count,
     socket.gethostname(),socket.gethostname()))

    script.close()
    os.chmod(scriptname,0777)

    if not debug:
        cmd = "bsub -q 8nh -W 240 %s/%s"%(os.getcwd(),scriptname)
        print cmd
        os.system(cmd)
    elif count == 0:
        cmd = "bsub -q test -W 240 %s/%s"%(os.getcwd(),scriptname)
        print cmd
        os.system(cmd)

    count = count + 1
