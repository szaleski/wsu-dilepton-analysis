from macrotools import *
import sys,os

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-n", "--njobs", type="int", dest="njobs",
                  help="Number of jobs to submit", metavar="njobs", default=10)
parser.add_option("-g", "--gridproxy", action="store_true", dest="gridproxy",
                  help="Generate a GRID proxy", metavar="gridproxy")

(options, args) = parser.parse_args()
if options.gridproxy:
    os.system("voms-proxy-init --voms cms --valid 168:00")
    
username = os.getenv("USER")
cmd = "cp /tmp/x509up_u17329  /afs/cern.ch/user/%s/%s/"%(username[0],username)
print cmd
os.system(cmd)
bSubSplitJobs("craft15", "jobConfig.cfg", "allInputFiles.txt", options.njobs)
