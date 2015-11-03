#!/bin/env python

import sys,os
import ROOT as r

from optparse import OptionParser

from cosmicEndpointShort import cosmicEndpointShort

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-n", "--nbiasbins", type="int", dest="nbiasbins",
                      metavar="nbiasbins", default=100,
                      help="Number of steps used to bias the curvature")
    parser.add_option("-r", "--rebins", type="int", dest="rebins",
                      metavar="rebins", default=1,
                      help="Number of bins to merge into one in the input histograms")
    parser.add_option("-b", "--maxbias", type="float", dest="maxbias",
                      metavar="maxbias", default=0.005,
                      help="Maximum bias that was injected into the curvature")
    parser.add_option("-i", "--infiledir", type="string", dest="infiledir",
                      metavar="infiledir",
                      help="Location of the input ROOT files")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      metavar="outfile",
                      help="Name of the output ROOT file")
    parser.add_option("-s", "--symmetric", action="store_true", dest="symmetric",
                      metavar="symmetric",
                      help="Are the curevature plots symmetric about 0")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      metavar="debug",
                      help="Run in debug mode, i.e., don't submit jobs, just create them")
    
    (options, args) = parser.parse_args()

    r.gROOT.SetBatch(True)
    myStudy = cosmicEndpointShort(options.infiledir,options.outfile,options.maxbias,options.nbiasbins,options.rebins)
    print myStudy
    needsFlip = False
    if options.symmetric:
        needsFlip = True

    myStudy.runMinimization("looseMuUpper","Minus", "Plus", needsFlip) # run with the minus as the observed, plus as the reference
    # myStudy.runMinimization("looseMuUpper","Plus", "Minus", needsFlip)
