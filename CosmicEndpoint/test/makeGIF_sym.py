#!/bin/env python

import ROOT as r
import sys,os
import numpy as np

from optparse import OptionParser
from wsuPythonUtils import checkRequiredArguments,setMinPT

if __name__ == "__main__":
    parser = OptionParser(usage="Usage: %prog -i inputfile.root -o outputfile.root [-d]")
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      metavar="infile",
                      help="[REQUIRED] Location of the input ROOT files")
    parser.add_option("-b", "--rebins", type="int", dest="rebins",
                      metavar="rebins", default=1,
                      help="[OPTIONAL] Number of bins to combine in the q/pT plot (default is 1)")
    parser.add_option("-n", "--biasbins", type="int", dest="biasbins",
                      metavar="biasbins", default=1000,
                      help="[OPTIONAL] Total number of injected bias points (default is 1000)")
    parser.add_option("-t", "--totalbins", type="int", dest="totalbins",
                      metavar="totalbins", default=5000,
                      help="[OPTIONAL] Total number of bins in the original curvature distribution (default is 5000)")
    parser.add_option("-m", "--maxbias", type="float", dest="maxbias",
                      metavar="maxbias", default=0.1,
                      help="[OPTIONAL] Maximum injected bias (default is 0.1 c/TeV)")
    parser.add_option("-s", "--stepsize", type="int", dest="stepsize",
                      metavar="stepsize", default=1,
                      help="[OPTIONAL] Step size in the GIF (default is 1)")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      metavar="debug",
                      help="[OPTIONAL] Debug mode")
    parser.add_option("--histbase", type="string", dest="histbase",
                      metavar="histbase", default="looseMuUpper",
                      help="[OPTIONAL] Base name of the histogram object (default is \"looseMuUpper\")")
    
    (options, args) = parser.parse_args()
    print options
    print args
    checkRequiredArguments(options, parser)
    myInFileName = options.infile
    myInFile = r.TFile(myInFileName,"READ")
    
    gifDir = "sampleGif"
    histName = options.histbase
    gifcanvas = r.TCanvas("gifcanvas","gifcanvas",750,750)
    r.gStyle.SetOptStat(0)
    print histName
    for step in range(0,options.biasbins/options.stepsize):
        gifcanvas.cd()
        negBias = myInFile.Get("%s%sMinusBias%03d"%(histName,"MinusCurve",options.biasbins-step*options.stepsize))
        negBias = setMinPT(negBias,options.totalbins,200./1000.,True)
        negBias.Rebin(options.rebins)
        posBias = myInFile.Get("%s%sMinusBias%03d"%(histName,"PlusCurve", options.biasbins-step*options.stepsize))
        posBias = setMinPT(posBias,options.totalbins,200./1000.,True)
        posBias.Rebin(options.rebins)
        neginto = negBias.Integral()
        posinto = posBias.Integral()
        if (negBias.Integral()>0):
            negBias.Scale(posBias.Integral()/negBias.Integral())
        neginta = negBias.Integral()
        posinta = posBias.Integral()
        negBias.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%(-(options.maxbias/options.biasbins)*(options.biasbins-step*options.stepsize)))
        negBias.SetLineColor(r.kRed)
        negBias.SetLineWidth(2)
        negBias.Draw()
        negBias.GetXaxis().SetTitle("#kappa [c/TeV]")
        negBias.SetMaximum(125)
        posBias.SetLineColor(r.kBlue)
        posBias.SetLineWidth(2)
        posBias.Draw("same")
        r.gPad.Update()
        #negBias.FindObject("stats").SetOptStat(0)
        #posBias.FindObject("stats").SetOptStat(0)
        r.gPad.Update()
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UUNORM" # unweighted/weighted, normalized
        resids = np.zeros(posBias.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
        #print "getting the probability"
        prob = posBias.Chi2TestX(negBias,chi2Val,chi2ndf,igood,histopts,resids)
        thelegend = r.TLegend(0.6,0.75,0.8,0.9)
        thelegend.SetHeader("#chi^{2}/ndf = %2.2f(%2.2f/%d)"%(chi2Val/chi2ndf,chi2Val,chi2ndf))
        thelegend.AddEntry(posBias,"#mu+ (%d,%d)"%(posinto,posinta))
        thelegend.AddEntry(negBias,"#mu- (%d,%d)"%(neginto,neginta))
        gifcanvas.cd()
        thelegend.Draw()
        r.gPad.Update()
        gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,step*options.stepsize))

    gifcanvas.cd()
    negBias = myInFile.Get("%s%s"%(histName,"MinusCurve"))
    negBias = setMinPT(negBias,options.totalbins,200./1000.,True)
    negBias.Rebin(options.rebins)
    posBias = myInFile.Get("%s%s"%(histName,"PlusCurve"))
    posBias = setMinPT(posBias,options.totalbins,200./1000.,True)
    posBias.Rebin(options.rebins)
    neginto = negBias.Integral()
    posinto = posBias.Integral()
    if (negBias.Integral()>0):
        negBias.Scale(posBias.Integral()/negBias.Integral())
    neginta = negBias.Integral()
    posinta = posBias.Integral()
    negBias.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%(0))
    negBias.SetLineColor(r.kRed)
    negBias.SetLineWidth(2)
    negBias.Draw()
    negBias.GetXaxis().SetTitle("#kappa [c/TeV]")
    negBias.SetMaximum(125)
    posBias.SetLineColor(r.kBlue)
    posBias.SetLineWidth(2)
    posBias.Draw("sames")
    r.gPad.Update()
    #negBias.FindObject("stats").SetOptStat(0)
    #posBias.FindObject("stats").SetOptStat(0)
    gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,1000))
    r.gPad.Update()
    chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
    chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
    igood    = r.Long(0)    # necessary for pass-by-reference in python
    histopts = "UUNORM" # unweighted/weighted, normalized
    resids = np.zeros(posBias.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
    #print "getting the probability"
    prob = posBias.Chi2TestX(negBias,chi2Val,chi2ndf,igood,histopts,resids)
    thelegend = r.TLegend(0.6,0.75,0.8,0.9)
    thelegend.SetHeader("#chi^{2}/ndf = %2.2f(%2.2f/%d)"%(chi2Val/chi2ndf,chi2Val,chi2ndf))
    thelegend.AddEntry(posBias,"#mu+ (%d,%d)"%(posinto,posinta))
    thelegend.AddEntry(negBias,"#mu- (%d,%d)"%(neginto,neginta))
    gifcanvas.cd()
    thelegend.Draw()
    r.gPad.Update()

    for step in range(0,options.biasbins/options.stepsize):
        gifcanvas.cd()
        negBias = myInFile.Get("%s%sPlusBias%03d"%(histName,"MinusCurve",1+step*options.stepsize))
        negBias = setMinPT(negBias,options.totalbins,200./1000.,True)
        negBias.Rebin(options.rebins)
        posBias = myInFile.Get("%s%sPlusBias%03d"%(histName,"PlusCurve", 1+step*options.stepsize))
        posBias = setMinPT(posBias,options.totalbins,200./1000.,True)
        posBias.Rebin(options.rebins)
        neginto = negBias.Integral()
        posinto = posBias.Integral()
        if (negBias.Integral()>0):
            negBias.Scale(posBias.Integral()/negBias.Integral())
        neginta = negBias.Integral()
        posinta = posBias.Integral()
        negBias.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%((options.maxbias/options.biasbins)*(step*options.stepsize+1)))
        negBias.SetLineColor(r.kRed)
        negBias.SetLineWidth(2)
        negBias.Draw()
        negBias.GetXaxis().SetTitle("#kappa [c/TeV]")
        negBias.SetMaximum(125)
        posBias.SetLineColor(r.kBlue)
        posBias.SetLineWidth(2)
        posBias.Draw("sames")
        r.gPad.Update()
        #negBias.FindObject("stats").SetOptStat(0)
        #posBias.FindObject("stats").SetOptStat(0)
        r.gPad.Update()
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UUNORM" # unweighted/weighted, normalized
        resids = np.zeros(posBias.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
        #print "getting the probability"
        prob = posBias.Chi2TestX(negBias,chi2Val,chi2ndf,igood,histopts,resids)
        thelegend = r.TLegend(0.6,0.75,0.8,0.9)
        thelegend.SetHeader("#chi^{2}/ndf = %2.2f(%2.2f/%d)"%(chi2Val/chi2ndf,chi2Val,chi2ndf))
        thelegend.AddEntry(posBias,"#mu+ (%d,%d)"%(posinto,posinta))
        thelegend.AddEntry(negBias,"#mu- (%d,%d)"%(neginto,neginta))
        gifcanvas.cd()
        thelegend.Draw()
        r.gPad.Update()
        gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,options.biasbins+1+step*options.stepsize))
    raw_input("press enter to exit")
