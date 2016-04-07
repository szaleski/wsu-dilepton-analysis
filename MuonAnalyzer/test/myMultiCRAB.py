#!/bin/env python

import sys,os
import subprocess
import math
import sys,os,socket
import shutil

from optparse import OptionParser
from wsuPythonUtils import checkRequiredArguments

parser = OptionParser()
parser.add_option("-t", "--tasks", type="string", dest="tasks",
                  metavar="tasks",
                  help="[REQUIRED] comma separated list of task names")
parser.add_option("-j", "--jobtype", type="string", dest="jobtype",
                  metavar="jobtype",
                  help="[REQUIRED] job type to submit (MuonTree, MuonEfficiency, MuonAnalyzer)")
parser.add_option("-d", "--debug", action="store_true", dest="debug",
                  metavar="debug",
                  help="[OPTIONAL] Run in debug mode, i.e., don't submit jobs, just create them")

(options, args) = parser.parse_args()
checkRequiredArguments(options, parser)

jobs = [x.strip() for x in options.tasks.split(',')]

jobnames = ["MuonTree", "MuonEfficiency", "MuonAnalyzer"]
crab = "/cvmfs/cms.cern.ch/crab3/crab-env-bootstrap.sh"

if options.jobtype not in jobnames:
    print "Invalid job type specified, options are:"
    print jobnames
    exit(0)
    
if options.debug:
    print jobs

for job in jobs:
    # check if config file exists:
    cfg = "crabConfig_%s_%s.py"%(options.jobtype,job)
    if os.path.exists(cfg):
        cmd = "%s submit -c %s"%(crab,cfg)
        if options.debug:
            print "found config file %s"%(cfg)
            print "%s"%(cmd)
            continue
        print "submitting job"
        print "%s"%(cmd)
        os.system(cmd)
    else:
        print "unable to find config file %s"%(cfg)
    # check if crab already created the project
    # parse config file, search
    # don't do this for now

    
