#!/bin/env python

from macrotools import *
import sys,os

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-n", "--njobs", type="int", dest="njobs",
                  help="Number of jobs to submit", metavar="njobs", default=10)
parser.add_option("-g", "--gridproxy", action="store_true", dest="gridproxy",
                  help="Generate a GRID proxy", metavar="gridproxy")
parser.add_option("-i", "--infiles", type="string", dest="infiles",
                  help="Text file with list of input files to process ", metavar="infiles")

(options, args) = parser.parse_args()
if options.gridproxy:
    os.system("voms-proxy-init --voms cms --valid 168:00")
    
username = os.getenv("USER")
userkey  = os.getenv("KRB5CCNAME")[17:17+5] #should be the form of FILE:/tmp/krb5cc_17329_lNizN17453
print username, userkey
proxyPath = "/afs/cern.ch/user/%s/%s/x509up_u%s"%(username[0],username,userkey)
cmd = "cp /tmp/x509up_u%s  %s"%(userkey,proxyPath)
print cmd
os.system(cmd)
bSubSplitJobs("craft15", "histograms", options.infiles, proxyPath, options.njobs)
