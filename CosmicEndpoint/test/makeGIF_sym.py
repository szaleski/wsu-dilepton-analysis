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
    parser.add_option("--infile2", type="string", dest="infile2",
                      metavar="infile2",
                      help="[OPTIONAL] Location of the comparison input ROOT file (necessary for \"dmc\" study)")
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
    parser.add_option("--study", type="string", dest="study",
                      metavar="study", default="pm",
                      help="[OPTIONAL]  Study to perform (default is \"pm\", options are: \"pm\",\"dmc[a-h]\")")

    # want to do several studies
    # 1) compare plus to minus (in data or MC, separately, scaling minus to plus)
    # 2) compare data to MC (varying the bias in MC)
    #   requires two input files
    #   scaling will be done as follows
    #   a) scaling MC to data (integral before cutting on pT) at each bias step
    #   b) scaling MC to data (integral before cutting on pT) for the unbiased histograms
    #   c) scaling MC to data (integral after cutting on pT) at each bias step
    #   d) scaling MC to data (integral after cutting on pT) for the unbiased histograms
    #   e-h) perform the scaling on the plus and minus separately, but follow the steps from above
    # may want to manually move the entries that migrate from plus to minus?

    (options, args) = parser.parse_args()
    print options
    print args

    if options.study not in ["pm","dmca","dmcb","dmcc","dmcd","dmce","dmcf","dmcg","dmch"]:
        print "Invalid study specified: %s. Valid options are \"pm\" or \"dmc[a-h]\""%(options.study)
        exit(1)
        pass

    if options.study in ["dmca","dmcb","dmcc","dmcd","dmce","dmcf","dmcg","dmch"] and options.infile2 == None:
        print "Must specify second input file for \"dmc\"-type study"
        exit(1)
        pass

    if not options.debug:
        print "setting batch mode True"
        r.gROOT.SetBatch(True)
    else:
        print "setting batch mode False"
        r.gROOT.SetBatch(False)
        pass
    
    checkRequiredArguments(options, parser)
    myInFileName = options.infile
    myInFile = r.TFile(myInFileName,"READ")

    myInFileName2 = options.infile2
    myInFile2 = None
    
    if options.study.find("dmc") > -1:
        myInFile2 = r.TFile(myInFileName2,"READ")
        pass

    gifDir = "sampleGif"
    histName = options.histbase

    testHist = myInFile.Get("%s%s"%(histName,"PlusCurve")).Clone("test")
    testHist = setMinPT(testHist,options.totalbins,200./1000.,True)
    testHist.Rebin(options.rebins)
    refmax = testHist.GetMaximum()

    #need two arrays, length = (2*nBiasBins)+1
    xVals = np.zeros(2*options.biasbins/options.stepsize+1,np.dtype('float64'))
    
    legLabel = {}
    legLabel["pm"] = {}
    
    yVals = {}
    for test in ["KS","Chi2"]:
        yVals[test] = np.zeros(2*options.biasbins/options.stepsize+1,np.dtype('float64'))
        pass
    
    gifcanvas = r.TCanvas("gifcanvas","gifcanvas",750,750)
    r.gStyle.SetOptStat(0)
    print histName
    # doing the "pm" study (comparing plus to minus in data or MC separately)
    #if options.study == "pm":
    for step in range(0,options.biasbins/options.stepsize):
        gifcanvas.cd()
        compHist = myInFile.Get("%s%sMinusBias%03d"%(histName,"MinusCurve",options.biasbins-step*options.stepsize))
        refHist  = myInFile.Get("%s%sMinusBias%03d"%(histName,"PlusCurve", options.biasbins-step*options.stepsize))
        compHist.Rebin(options.rebins)
        compHist = setMinPT(compHist,options.totalbins,200./1000.,True)
        refHist.Rebin(options.rebins)
        refHist = setMinPT(refHist,options.totalbins,200./1000.,True)
        compinto = compHist.Integral()
        refinto = refHist.Integral()
        if (compHist.Integral() > 0):
            compHist.Scale(refHist.Integral()/compHist.Integral())
        else:
            print "unable to scale comp histogram, integral is 0"
            pass
        compinta = compHist.Integral()
        refinta = refHist.Integral()
        compHist.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%(-(options.maxbias/options.biasbins)*(options.biasbins-step*options.stepsize)))
        compHist.SetLineColor(r.kRed)
        compHist.SetLineWidth(2)
        compHist.Draw()
        compHist.GetXaxis().SetTitle("#kappa [c/TeV]")
        compHist.SetMaximum(1.2*refmax)
        refHist.SetLineColor(r.kBlue)
        refHist.SetLineWidth(2)
        refHist.Draw("sames")
        fcompHist = flipHist(compHist)
        compHist.SetLineStyle(8)
        fcompHist.SetLineStyle(1)
        fcompHist.Draw("sames")
        r.gPad.Update()
        #compHist.FindObject("stats").SetOptStat(0)
        #refHist.FindObject("stats").SetOptStat(0)
        r.gPad.Update()
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UU,NORM" # unweighted/weighted, normalized
        resids = np.zeros(refHist.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
        prob   = refHist.Chi2TestX(flipHist(compHist),chi2Val,chi2ndf,igood,histopts,resids)
        ksprob = refHist.KolmogorovTest(flipHist(compHist),"D")
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

        thelegend.AddEntry(refHist,  "#mu+ (%d,%d)"%(refinto,refinta))
        thelegend.AddEntry(fcompHist,"#mu- (%d,%d)"%(compinto,compinta))
        #thelegend.AddEntry(fcompHist,"#mu- (%d,%d) (flipped)"%(compinto,compinta))
        gifcanvas.cd()
        thetext.Draw("nb")
        thelegend.Draw()
        r.gPad.Update()
        gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,step*options.stepsize))
        pass

    if options.debug:
        raw_input("press enter to exit")
        pass
    gifcanvas.cd()
    compHist = myInFile.Get("%s%s"%(histName,"MinusCurve"))
    refCurve = myInFile.Get("%s%s"%(histName,"PlusCurve"))
    print "refNBins %d, compNBins %d"%(refCurve.GetNbinsX(),compHist.GetNbinsX())
    compHist.Rebin(options.rebins)
    compHist = setMinPT(compHist,options.totalbins,200./1000.,True,True)
    refCurve.Rebin(options.rebins)
    refCurve = setMinPT(refCurve,options.totalbins,200./1000.,True,True)
    compinto = compHist.Integral()
    refinto = refCurve.Integral()
    if (compHist.Integral() > 0):
        compHist.Scale(refCurve.Integral()/compHist.Integral())
    else:
        print "unable to scale comp histogram, integral is 0"
        pass
    compinta = compHist.Integral()
    refinta = refCurve.Integral()
    compHist.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%(0))
    compHist.SetLineColor(r.kRed)
    compHist.SetLineWidth(2)
    compHist.Draw()
    compHist.GetXaxis().SetTitle("#kappa [c/TeV]")
    compHist.SetMaximum(1.2*refmax)
    refCurve.SetLineColor(r.kBlue)
    refCurve.SetLineWidth(2)
    refCurve.Draw("sames")
    fcompHist = flipHist(compHist)
    compHist.SetLineStyle(8)
    fcompHist.SetLineStyle(1)
    fcompHist.Draw("sames")
    r.gPad.Update()
    #compHist.FindObject("stats").SetOptStat(0)
    #refCurve.FindObject("stats").SetOptStat(0)
    gifcanvas.SaveAs("%s/biasBin%04d_sym.png"%(gifDir,1000))
    r.gPad.Update()
    chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
    chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
    igood    = r.Long(0)    # necessary for pass-by-reference in python
    histopts = "UU,NORM" # unweighted/weighted, normalized
    resids = np.zeros(refCurve.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
    prob = refCurve.Chi2TestX(flipHist(compHist),chi2Val,chi2ndf,igood,histopts,resids)
    ksprob = refCurve.KolmogorovTest(flipHist(compHist),"D")
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
    thelegend.AddEntry(refCurve,"#mu+ (%d,%d)"%(refinto,refinta))
    thelegend.AddEntry(fcompHist,"#mu- (%d,%d)"%(compinto,compinta))
    #thelegend.AddEntry(fcompHist,"#mu- (%d,%d)(flipped)"%(compinto,compinta))
    gifcanvas.cd()
    thetext.Draw("nb")
    thelegend.Draw()
    r.gPad.Update()
    if options.debug:
        raw_input("press enter to exit")
        pass

    for step in range(0,options.biasbins/options.stepsize):
        gifcanvas.cd()
        compHist = myInFile.Get("%s%sPlusBias%03d"%(histName,"MinusCurve",1+step*options.stepsize))
        refHist = myInFile.Get("%s%sPlusBias%03d"%(histName,"PlusCurve", 1+step*options.stepsize))
        compHist.Rebin(options.rebins)
        compHist = setMinPT(compHist,options.totalbins,200./1000.,True)
        refHist.Rebin(options.rebins)
        refHist = setMinPT(refHist,options.totalbins,200./1000.,True)
        compinto = compHist.Integral()
        refinto = refHist.Integral()
        if (compHist.Integral() > 0):
            compHist.Scale(refHist.Integral()/compHist.Integral())
        else:
            print "unable to scale comp histogram, integral is 0"
        pass
        compinta = compHist.Integral()
        refinta = refHist.Integral()
        compHist.SetTitle("#Delta#kappa = %2.4f [c/TeV]"%((options.maxbias/options.biasbins)*(step*options.stepsize+1)))
        compHist.SetLineColor(r.kRed)
        compHist.SetLineWidth(2)
        compHist.Draw()
        compHist.GetXaxis().SetTitle("#kappa [c/TeV]")
        compHist.SetMaximum(1.2*refmax)
        refHist.SetLineColor(r.kBlue)
        refHist.SetLineWidth(2)
        refHist.Draw("sames")
        fcompHist = flipHist(compHist)
        compHist.SetLineStyle(8)
        fcompHist.SetLineStyle(1)
        fcompHist.Draw("sames")
        r.gPad.Update()
        #compHist.FindObject("stats").SetOptStat(0)
        #refHist.FindObject("stats").SetOptStat(0)
        r.gPad.Update()
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UU,NORM" # unweighted/weighted, normalized
        resids = np.zeros(refHist.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
        prob   = refHist.Chi2TestX(flipHist(compHist),chi2Val,chi2ndf,igood,histopts,resids)
        ksprob = refHist.KolmogorovTest(flipHist(compHist),"D")
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
        thelegend.AddEntry(refHist,"#mu+ (%d,%d)"%(refinto,refinta))
        thelegend.AddEntry(fcompHist,"#mu- (%d,%d)"%(compinto,compinta))
        #thelegend.AddEntry(fcompHist,"#mu- (%d,%d)(flipped)"%(compinto,compinta))
        gifcanvas.cd()
        thetext.Draw("nb")
        thelegend.Draw()
        r.gPad.Update()
        gifcanvas.SaveAs("%s/%s_biasBin%04d_sym.png"%(gifDir,options.study,options.biasbins+1+step*options.stepsize))
        pass
    
    if options.debug:
        raw_input("press enter to exit")
        pass

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
    chi2graph.SaveAs("chi2_%s_%s.C"%(  options.histbase,options.study))
    chi2graph.SaveAs("chi2_%s_%s.pdf"%(options.histbase,options.study))
    chi2graph.SaveAs("chi2_%s_%s.png"%(options.histbase,options.study))
    if options.debug:
        ksgraph.Draw("LPSAMES")
    else:
        ksgraph.Draw("ALP")
        pass
    ksgraph.SaveAs("ks_%s_%s.C"%(  options.histbase,options.study))
    ksgraph.SaveAs("ks_%s_%s.pdf"%(options.histbase,options.study))
    ksgraph.SaveAs("ks_%s_%s.png"%(options.histbase,options.study))
    if options.debug:
        raw_input("press enter to exit")
        pass
    
