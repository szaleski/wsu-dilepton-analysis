#!/bin/env python

from macrotools import *
import sys,os

from optparse import OptionParser

def checkRequiredArguments(opts, parser):
    """From: http://stackoverflow.com/questions/4407539/python-how-to-make-an-option-to-be-required-in-optparse"""
    missing_options = []
    for option in parser.option_list:
        if re.match(r'^\[REQUIRED\]', option.help) and eval('opts.' + option.dest) == None:
            missing_options.extend(option._long_opts)
        if len(missing_options) > 0:
            parser.error('Missing REQUIRED parameters: ' + str(missing_options))
    return                                                        

parser = OptionParser()
parser.add_option("-n", "--njobs", type="int", dest="njobs",
                  default=10,
                  help="[OPTIONAL] Number of jobs to submit (default is 10)", metavar="njobs")
parser.add_option("-g", "--gridproxy", action="store_true", dest="gridproxy",
                  metavar="gridproxy",
                  help="[OPTIONAL] Generate a GRID proxy")
parser.add_option("-i", "--infiles", type="string", dest="infiles",
                  metavar="infiles",
                  help="[REQUIRED] Text file with list of input files to process ")
parser.add_option("-d", "--debug", action="store_true", dest="debug",
                  metavar="debug",
                  help="[OPTIONAL] Run in debug mode, i.e., don't submit jobs, just create them")

(options, args) = parser.parse_args()
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
bSubSplitJobs("craft15", "histograms", options.infiles, proxyPath, options.njobs,debug)
