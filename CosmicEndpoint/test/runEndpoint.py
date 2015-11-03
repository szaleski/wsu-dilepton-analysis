#!/bin/env python

import sys,os
import ROOT as r

from optparse import OptionParser

from cosmicEndpointShort import cosmicEndpointShort

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-n", "--nbiasbins", type="int", dest="nbiasbins",
                      help="Number of steps used to bias the curvature", metavar="nbiasbins", default=100)
    parser.add_option("-r", "--rebins", type="int", dest="rebins",
                      help="Number of bins to merge into one in the input histograms", metavar="rebins", default=1)
    parser.add_option("-b", "--maxbias", type="float", dest="maxbias",
                      help="Maximum bias that was injected into the curvature", metavar="maxbias", default=0.005)
    parser.add_option("-i", "--infiledir", type="string", dest="infiledir",
                      help="Location of the input ROOT files", metavar="infiledir")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      help="Name of the output ROOT file", metavar="outfile")
    parser.add_option("-s", "--symmetric", action="store_true", dest="symmetric",
                      help="Are the curevature plots symmetric about 0", metavar="symmetric")
    
    (options, args) = parser.parse_args()

    r.gROOT.SetBatch(True)
    myStudy = cosmicEndpointShort(options.infiledir,options.outfile,options.maxbias,options.nbiasbins,options.rebins)
    print myStudy
    needsFlip = False
    if options.symmetric:
        needsFlip = True

    myStudy.runMinimization("looseMuUpper","Minus", "Plus", needsFlip) # run with the minus as the observed, plus as the reference
    # myStudy.runMinimization("looseMuUpper","Plus", "Minus", needsFlip)
