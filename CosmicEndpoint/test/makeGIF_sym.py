#!/bin/env python

import ROOT as r
import sys,os
import numpy as np

from optparse import OptionParser
from wsuPythonUtils import *

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
                      metavar="histbase", default="looseMuLower",
                      help="[OPTIONAL] Base name of the histogram object (default is \"looseMuLower\")")
    
    (options, args) = parser.parse_args()
    print options
    print args

    if not options.debug:
        print "setting batch mode True"
        r.gROOT.SetBatch(True)
    else:
        print "setting batch mode False"
        r.gROOT.SetBatch(False)

        checkRequiredArguments(options, parser)
    myInFileName = options.infile
    myInFile = r.TFile(myInFileName,"READ")

    gifDir = "sampleGif"
    histName = options.histbase

    testHist = myInFile.Get("%s%s"%(histName,"PlusCurve")).Clone("test")
    testHist = setMinPT(testHist,options.totalbins,200./1000.,True)
    testHist.Rebin(options.rebins)
    posmax = testHist.GetMaximum()

    #need two arrays, length = (2*nBiasBins)+1
    xVals = np.zeros(2*options.biasbins/options.stepsize+1,np.dtype('float64'))
    
    yVals = {}
    for test in ["KS","Chi2"]:
        yVals[test] = np.zeros(2*options.biasbins/options.stepsize+1,np.dtype('float64'))
                                                                
    gifcanvas = r.TCanvas("gifcanvas","gifcanvas",750,750)
    r.gStyle.SetOptStat(0)
    print histName
    for step in range(0,options.biasbins/options.stepsize):
        gifcanvas.cd()
        negBias = myInFile.Get("%s%sMinusBias%03d"%(histName,"MinusCurve",options.biasbins-step*options.stepsize))
        posBias = myInFile.Get("%s%sMinusBias%03d"%(histName,"PlusCurve", options.biasbins-step*options.stepsize))
        negBias.Rebin(options.rebins)
        negBias = setMinPT(negBias,options.totalbins,200./1000.,True)
        posBias.Rebin(options.rebins)
        posBias = setMinPT(posBias,options.totalbins,200./1000.,True)
        neginto = negBias.Integral()
        posinto = posBias.Integral()
        if (negBias.Integral()>0):
            negBias.Scale(posBias.Integral()/negBias.Integral())
        else:
            print "unable to scale neg histogram, integral is 0"
        neginta = negBias.Integral()
        posinta = posBias.Integral()
        negBias.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%(-(options.maxbias/options.biasbins)*(options.biasbins-step*options.stepsize)))
        negBias.SetLineColor(r.kRed)
        negBias.SetLineWidth(2)
        negBias.Draw()
        negBias.GetXaxis().SetTitle("#kappa [c/TeV]")
        negBias.SetMaximum(1.2*posmax)
        posBias.SetLineColor(r.kBlue)
        posBias.SetLineWidth(2)
        posBias.Draw("sames")
        fnegBias = flipHist(negBias)
        negBias.SetLineStyle(8)
        fnegBias.SetLineStyle(1)
        fnegBias.Draw("sames")
        r.gPad.Update()
        #negBias.FindObject("stats").SetOptStat(0)
        #posBias.FindObject("stats").SetOptStat(0)
        r.gPad.Update()
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UU,NORM" # unweighted/weighted, normalized
        resids = np.zeros(posBias.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
        #print "getting the probability"
        prob   = posBias.Chi2TestX(flipHist(negBias),chi2Val,chi2ndf,igood,histopts,resids)
        ksprob = posBias.KolmogorovTest(flipHist(negBias),"D")
        xVals[step] = -(options.maxbias/options.biasbins)*(options.biasbins-step*options.stepsize)
        yVals["Chi2"][step] = chi2Val/chi2ndf
        yVals["KS"][step]   = ksprob

        thetext = r.TPaveText(0.7,0.2,0.9,0.3,"ndc")
        thetext.SetFillColor(0)
        thetext.SetFillStyle(3000)
        thetext.AddText("#chi^{2}/ndf = %2.2f(%2.2f/%d)"%(chi2Val/chi2ndf,chi2Val,chi2ndf))
        thetext.AddText("KS prob = %2.4e"%(ksprob))
        thelegend = r.TLegend(0.7,0.1,0.9,0.2)
        thelegend.SetFillColor(0)
        thelegend.SetFillStyle(3000)
        thelegend.AddEntry(posBias,"#mu+ (%d,%d)"%(posinto,posinta))
        thelegend.AddEntry(fnegBias,"#mu- (%d,%d)"%(neginto,neginta))
        #thelegend.AddEntry(fnegBias,"#mu- (%d,%d) (flipped)"%(neginto,neginta))
        gifcanvas.cd()
        thetext.Draw("nb")
        thelegend.Draw()
        r.gPad.Update()
        gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,step*options.stepsize))

    if options.debug:
        raw_input("press enter to exit")
    gifcanvas.cd()
    negCurve = myInFile.Get("%s%s"%(histName,"MinusCurve"))
    posCurve = myInFile.Get("%s%s"%(histName,"PlusCurve"))
    print "posNBins %d, negNBins %d"%(posCurve.GetNbinsX(),negCurve.GetNbinsX())
    negCurve.Rebin(options.rebins)
    negCurve = setMinPT(negCurve,options.totalbins,200./1000.,True,True)
    posCurve.Rebin(options.rebins)
    posCurve = setMinPT(posCurve,options.totalbins,200./1000.,True,True)
    neginto = negCurve.Integral()
    posinto = posCurve.Integral()
    if (negCurve.Integral()>0):
        negCurve.Scale(posCurve.Integral()/negCurve.Integral())
    else:
        print "unable to scale neg histogram, integral is 0"
    neginta = negCurve.Integral()
    posinta = posCurve.Integral()
    negCurve.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%(0))
    negCurve.SetLineColor(r.kRed)
    negCurve.SetLineWidth(2)
    negCurve.Draw()
    negCurve.GetXaxis().SetTitle("#kappa [c/TeV]")
    negCurve.SetMaximum(1.2*posmax)
    posCurve.SetLineColor(r.kBlue)
    posCurve.SetLineWidth(2)
    posCurve.Draw("sames")
    fnegCurve = flipHist(negCurve)
    negCurve.SetLineStyle(8)
    fnegCurve.SetLineStyle(1)
    fnegCurve.Draw("sames")
    r.gPad.Update()
    #negCurve.FindObject("stats").SetOptStat(0)
    #posCurve.FindObject("stats").SetOptStat(0)
    gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,1000))
    r.gPad.Update()
    chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
    chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
    igood    = r.Long(0)    # necessary for pass-by-reference in python
    histopts = "UU,NORM" # unweighted/weighted, normalized
    resids = np.zeros(posCurve.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
    #print "getting the probability"
    prob = posCurve.Chi2TestX(flipHist(negCurve),chi2Val,chi2ndf,igood,histopts,resids)
    ksprob = posCurve.KolmogorovTest(flipHist(negCurve),"D")
    xVals[options.biasbins/options.stepsize] = 0.0
    yVals["Chi2"][options.biasbins/options.stepsize] = chi2Val/chi2ndf
    yVals["KS"][options.biasbins/options.stepsize]   = ksprob
    
    thetext = r.TPaveText(0.7,0.2,0.9,0.3,"ndc")
    thetext.SetFillColor(0)
    thetext.SetFillStyle(3000)
    thetext.AddText("#chi^{2}/ndf = %2.2f(%2.2f/%d)"%(chi2Val/chi2ndf,chi2Val,chi2ndf))
    thetext.AddText("KS prob = %2.4e"%(ksprob))
    thelegend = r.TLegend(0.7,0.1,0.9,0.2)
    thelegend.SetFillColor(0)
    thelegend.SetFillStyle(3000)
    thelegend.AddEntry(posCurve,"#mu+ (%d,%d)"%(posinto,posinta))
    thelegend.AddEntry(fnegCurve,"#mu- (%d,%d)"%(neginto,neginta))
    #thelegend.AddEntry(fnegCurve,"#mu- (%d,%d)(flipped)"%(neginto,neginta))
    gifcanvas.cd()
    thetext.Draw("nb")
    thelegend.Draw()
    r.gPad.Update()
    if options.debug:
        raw_input("press enter to exit")

    for step in range(0,options.biasbins/options.stepsize):
        gifcanvas.cd()
        negBias = myInFile.Get("%s%sPlusBias%03d"%(histName,"MinusCurve",1+step*options.stepsize))
        posBias = myInFile.Get("%s%sPlusBias%03d"%(histName,"PlusCurve", 1+step*options.stepsize))
        negBias.Rebin(options.rebins)
        negBias = setMinPT(negBias,options.totalbins,200./1000.,True)
        posBias.Rebin(options.rebins)
        posBias = setMinPT(posBias,options.totalbins,200./1000.,True)
        neginto = negBias.Integral()
        posinto = posBias.Integral()
        if (negBias.Integral()>0):
            negBias.Scale(posBias.Integral()/negBias.Integral())
        else:
            print "unable to scale neg histogram, integral is 0"
        neginta = negBias.Integral()
        posinta = posBias.Integral()
        negBias.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%((options.maxbias/options.biasbins)*(step*options.stepsize+1)))
        negBias.SetLineColor(r.kRed)
        negBias.SetLineWidth(2)
        negBias.Draw()
        negBias.GetXaxis().SetTitle("#kappa [c/TeV]")
        negBias.SetMaximum(1.2*posmax)
        posBias.SetLineColor(r.kBlue)
        posBias.SetLineWidth(2)
        posBias.Draw("sames")
        fnegBias = flipHist(negBias)
        negBias.SetLineStyle(8)
        fnegBias.SetLineStyle(1)
        fnegBias.Draw("sames")
        r.gPad.Update()
        #negBias.FindObject("stats").SetOptStat(0)
        #posBias.FindObject("stats").SetOptStat(0)
        r.gPad.Update()
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UU,NORM" # unweighted/weighted, normalized
        resids = np.zeros(posBias.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
        #print "getting the probability"
        prob   = posBias.Chi2TestX(flipHist(negBias),chi2Val,chi2ndf,igood,histopts,resids)
        ksprob = posBias.KolmogorovTest(flipHist(negBias),"D")
        xVals[options.biasbins/options.stepsize+1+step] = (options.maxbias/options.biasbins)*(step*options.stepsize+1)
        yVals["Chi2"][options.biasbins/options.stepsize+1+step] = chi2Val/chi2ndf
        yVals["KS"][options.biasbins/options.stepsize+1+step]   = ksprob

        thetext = r.TPaveText(0.7,0.2,0.9,0.3,"ndc")
        thetext.SetFillColor(0)
        thetext.SetFillStyle(3000)
        thetext.AddText("#chi^{2}/ndf = %2.2f(%2.2f/%d)"%(chi2Val/chi2ndf,chi2Val,chi2ndf))
        thetext.AddText("KS prob = %2.4e"%(ksprob))
        thelegend = r.TLegend(0.7,0.1,0.9,0.2)
        thelegend.SetFillColor(0)
        thelegend.SetFillStyle(3000)
        thelegend.AddEntry(posBias,"#mu+ (%d,%d)"%(posinto,posinta))
        thelegend.AddEntry(fnegBias,"#mu- (%d,%d)"%(neginto,neginta))
        #thelegend.AddEntry(fnegBias,"#mu- (%d,%d)(flipped)"%(neginto,neginta))
        gifcanvas.cd()
        thetext.Draw("nb")
        thelegend.Draw()
        r.gPad.Update()
        gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,options.biasbins+1+step*options.stepsize))
    if options.debug:
        raw_input("press enter to exit")

    graphInfo = {}
    graphInfo["KS"]   = {"color":r.kRed,"marker":r.kFullCircle,
                         "title":"Kolmogorov test statistic",
                         "yaxis":""}
    graphInfo["Chi2"] = {"color":r.kBlue, "marker":r.kFullCircle,
                         "title":"ROOT #chi^{2}/ndf",
                         "yaxis":""}
    
    chi2graph = prettifyGraph(r.TGraph(xVals.size,xVals,yVals["Chi2"]),graphInfo["Chi2"])
    ksgraph   = prettifyGraph(r.TGraph(xVals.size,xVals,yVals["KS"])  ,graphInfo["KS"]  )
    chi2graph.Draw("ALP")
    gifcanvas.SetGridx(1)
    gifcanvas.SetGridy(1)
    chi2graph.GetYaxis().SetRangeUser(0,5)
    chi2graph.SaveAs("chi2_%s.C"%(options.histbase))
    if options.debug:
        ksgraph.Draw("LPSAMES")
    else:
        ksgraph.Draw("ALP")
    ksgraph.SaveAs("ks_%s.C"%(options.histbase))
    if options.debug:
        raw_input("press enter to exit")
