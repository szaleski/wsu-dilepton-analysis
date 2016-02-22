#!/bin/env python

from macrotools import *
import sys,os

from optparse import OptionParser
from wsuPythonUtils import checkRequiredArguments

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
parser.add_option("-t", "--title", type="string", dest="title",
                  metavar="title",
                  help="[REQUIRED] Task title ")
parser.add_option("-d", "--debug", action="store_true", dest="debug",
                  metavar="debug",
                  help="[OPTIONAL] Run in debug mode, i.e., don't submit jobs, just create them")
parser.add_option("-m", "--maxbias", type="float", dest="maxbias",
                  metavar="maxbias", default=0.0001,
                  help="[OPTIONAL] Maximum bias in the curvature to inject in units of c/GeV (default = 0.0001)")
parser.add_option("-a", "--asymmetric", action="store_true", dest="asymmetric",
                  metavar="asymmetric", default=False,
                  help="[OPTIONAL] Specify whether to create asymmetric (absolute value) curvature (default = False)")
parser.add_option("-p", "--minpt", type="float", dest="minpt",
                  metavar="minpt", default=50.,
                  help="[OPTIONAL] Minimum pT cut to apply to the muons (default = 50.)")
parser.add_option("-b", "--nbiasbins", type="int", dest="nbiasbins",
                  metavar="nbiasbins", default=2500,
                  help="[OPTIONAL] Number of steps to vary the injected bias by (default = 2500)")

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
if options.asymmetric:
    bSubSplitJobs("%s-%s"%(options.title,options.infiles[:-4]), "histograms", options.infiles, proxyPath, options.njobs,
                  options.maxbias, options.minpt, options.nbiasbins, False, debug)
else:
    bSubSplitJobs("%s-%s"%(options.title,options.infiles[:-4]), "histograms", options.infiles, proxyPath, options.njobs,
                  options.maxbias, options.minpt, options.nbiasbins, True, debug)
