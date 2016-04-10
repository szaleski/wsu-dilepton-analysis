#!/bin/env python

import sys,os
import ROOT as r

import numpy as np
from wsuPythonUtils import checkRequiredArguments
from wsuMuonTreeUtils import *

from getMCScaleFactors import getMCScaleFactors
from l1TrigInfo import l1TrigInfo

from optparse import OptionParser

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      metavar="infile", default="CosmicMuonTree_MC_76X",
                      help="[REQUIRED] Name of the input ROOT file, or list of files (for -t mode)")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      metavar="outfile", default="simtrack_dists",
                      help="[REQUIRED] Name of the output ROOT file")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      metavar="debug",
                      help="[OPTIONAL] Run in debug mode")
    parser.add_option("-m", "--mc", action="store_true", dest="mc",
                      metavar="mc",
                      help="[OPTIONAL] Running ananalysis on MC")
    parser.add_option("-t", "--tchain", action="store_true", dest="tchain",
                      metavar="tchain",
                      help="[OPTIONAL] Use a TChain rather than the raw file, must specify a list of input files as a text file to -i")

    (options, args) = parser.parse_args()
    checkRequiredArguments(options, parser)
    r.gROOT.SetBatch(True)

    myfile = None    
    mytree = None    

    print "creating output file"
    outfile = r.TFile("%s.root"%(options.outfile),"recreate")
    #outfile = options.outfile
    sys.stdout.flush()
            
    minpT = 0
    maxpT = 0
    if options.outfile.find("p10_") > -1:
        minpT = 0
        maxpT = 100
    elif options.outfile.find("p100_") > -1:
        minpT = 100
        maxpT = 500
    elif options.outfile.find("p500_") > -1:
        minpT = 500
        maxpT = 10000
        pass

    if options.mc:
        print "setting up scaling"
        scaling = getMCScaleFactors(options.infile,outfile,minpT,maxpT,options.tchain,options.debug)
        sys.stdout.flush()
    print "setting up l1trigs"
    l1trigs = l1TrigInfo(options.infile,outfile,options.mc,options.tchain,options.debug)
    sys.stdout.flush()

    print "setting up tree access"
    sys.stdout.flush()
    if options.tchain:
        mychain = r.TChain("analysisSPMuons/MuonTree")
        for line in open("%s"%(options.infile),"r"):
            if options.debug:
                print "root://xrootd.unl.edu//%s"%(line[:-1])
                tmp = r.TNetXNGFile("root://xrootd.unl.edu//%s"%(line[:-1]),"r")
                print tmp, "%d - %d"%(tmp.IsZombie(),tmp.IsOpen())
                sys.stdout.flush()
                pass
            print "adding root://xrootd.unl.edu//%s"%(line[:-1])
            sys.stdout.flush()
            mychain.Add("root://xrootd.unl.edu//%s"%(line[:-1]))
            print "to my chain (%d)"%(mychain.GetEntries())
            sys.stdout.flush()
            pass
        
        mytree = mychain
        #l1trigs.processTree(mytree)
        #scaling.processTree(mytree,minpT,maxpT)
        pass
    else:
        if (options.infile).find("root://") > -1:
            print "using TNetXNGFile for EOS access"
            print "%s"%(options.infile)
            sys.stdout.flush()
            myfile = r.TNetXNGFile("%s"%(options.infile),"r")
            pass 
        else:
            print "%s"%(options.infile)
            sys.stdout.flush()
            myfile = r.TFile("%s"%(options.infile),"r")
            pass
        
        mytree = myfile.Get("analysisSPMuons/MuonTree")
        if options.debug:
            print mytree
            sys.stdout.flush()
            pass
        pass
    print "running l1trigs"
    sys.stdout.flush()
    l1trigs.processTree(mytree)
    print "writing out l1trigs"
    sys.stdout.flush()
    l1trigs.writeOut()

    if options.mc:
        print "running scaling"
        sys.stdout.flush()
        scaling.processTree(mytree,minpT,maxpT)
        print "writing out scaling results"
        sys.stdout.flush()
        scaling.writeOut()
    
    outfile.Write()
    outfile.Close()
