#!/bin/env python

import sys,os
import ROOT as r

from optparse import OptionParser

from cosmicEndpointShort import cosmicEndpointShort

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-n", "--nbiasbins", type="int", dest="nbiasbins",
                      metavar="nbiasbins", default=2500,
                      help="[OPTIONAL] Number of steps used to bias the curvature (default is 2500)")
    parser.add_option("-t", "--totalbins", type="int", dest="totalbins",
                      metavar="totalbins", default=1500,
                      help="[OPTIONAL] Total number of bins in the original curvature distribution (default is 1500)")
    parser.add_option("-r", "--rebins", type="int", dest="rebins",
                      metavar="rebins", default=1,
                      help="[OPTIONAL] Number of bins to merge into one in the input histograms (default is 1, no merging)")
    parser.add_option("-f", "--factor", type="int", dest="factor",
                      metavar="factor", default=1000,
                      help="[OPTIONAL] Multiplicative factor on the curvature (default is 1000)")
    parser.add_option("-b", "--maxbias", type="float", dest="maxbias",
                      metavar="maxbias", default=0.0005,
                      help="[] Maximum bias that was injected into the curvature (default is 0.0005/GeV")
    parser.add_option("-i", "--infiledir", type="string", dest="infiledir",
                      metavar="infiledir",
                      help="[REQUIRED] Location of the input ROOT files")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      metavar="outfile",
                      help="[REQUIRED] Name of the output ROOT file")
    parser.add_option("-s", "--symmetric", action="store_true", dest="symmetric",
                      metavar="symmetric",
                      help="[OPTIONAL] Are the curevature plots symmetric about 0, if not specified, asymmetric is assumed")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      metavar="debug",
                      help="[OPTIONAL] Run in debug mode")
    parser.add_option("--histbase", type="string", dest="histbase",
                      metavar="histbase", default="looseMuUpper",
                      help="[OPTIONAL] Base name of the histogram object (default is \"looseMuUpper\")")
    parser.add_option("--obs", type="string", dest="obs",
                      metavar="obs", default="Minus",
                      help="[OPTIONAL] Name of the observed histogram object (default is \"Minus\", using the negatively charged muon as the observed value)")
    parser.add_option("--ref", type="string", dest="ref",
                      metavar="ref", default="Plus",
                      help="[OPTIONAL] Name of the reference histogram object (default is \"Plus\", using the positively charged muon as the reference value)")
    
    (options, args) = parser.parse_args()

    r.gROOT.SetBatch(True)
    myStudy = cosmicEndpointShort(options.infiledir,options.outfile,
                                  options.maxbias,options.nbiasbins,
                                  options.totalbins,
                                  options.factor,
                                  options.rebins)
    if options.debug:
        print options
        print myStudy
    
    needsFlip = False
    if options.symmetric:
        needsFlip = True
    # The arguments are baseHistogramName, Observed, Reference
    # The example below would use histograms of the form looseMuUpperPlusCurve and looseMuUpperMinusCurve
    #   (and their corresponding suffixes PlusBiasXXX and MinusBiasXXX for the injected positive or negative bias)
    # myStudy.runMinimization("looseMuUpper","Plus", "Minus", needsFlip)
    myStudy.runMinimization(options.histbase,options.obs,options.ref, needsFlip)
