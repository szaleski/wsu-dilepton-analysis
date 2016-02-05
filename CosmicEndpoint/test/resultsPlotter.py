#!/bin/env python
import sys,os,re
import ROOT as r
import numpy as np

from optparse import OptionParser
#from histograms import outputHistograms
from wsuPythonUtils import checkRequiredArguments
from wsuPyROOTUtils import makeNicePlot

def makeBiasPlot(inputfile,canvasName,plusHist,minusHist,outName,biasBin,posneg,maxBias,nSteps=1000):
    positiveo = r.TH1F(inputfile.Get("obs_Minus/%s_original"%(canvasName)).GetPrimitive("%s"%(plusHist)))
    negativeo = r.TH1F(inputfile.Get("obs_Minus/%s_original"%(canvasName)).GetPrimitive("%s"%(minusHist)))
    positivea = r.TH1F(inputfile.Get("obs_Minus/%s_analyzed"%(canvasName)).GetPrimitive("%s"%(plusHist)))
    negativea = r.TH1F(inputfile.Get("obs_Minus/%s_analyzed"%(canvasName)).GetPrimitive("%s"%(minusHist)))
    print "raw: %d, analyzed: %d bins: %d"%(positiveo.Integral(),positivea.Integral(),positivea.GetNbinsX())
    print "raw: %d, analyzed: %d bins: %d"%(negativeo.Integral(),negativea.Integral(),negativea.GetNbinsX())

    resultsCanvas = r.TCanvas("results%s"%(outName),"results%s"%(outName),800,800)
    resultsCanvas.cd()
    chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
    chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
    igood    = r.Long(0)    # necessary for pass-by-reference in python
    histopts = "UUNORM" # unweighted/weighted, normalized
    resids = np.zeros(positivea.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
    print "getting the probability"
    prob = positivea.Chi2TestX(negativea,chi2Val,chi2ndf,igood,histopts,resids)
    print positivea
    print negativea
    if posneg == "p":
        positivea.SetTitle("Curvature (#Delta#kappa == +%.2f c/TeV)"%(biasBin*maxBias/nSteps))
    elif posneg == "n" or posneg == "m":
        positivea.SetTitle("Curvature (#Delta#kappa == -%.2f c/TeV)"%(biasBin*maxBias/nSteps))
    else:
        positivea.SetTitle("Curvature (#Delta#kappa == %.f c/TeV)"%(biasBin*maxBias/nSteps))
    positivea.GetXaxis().SetRangeUser(0,5)
    positivea.GetYaxis().SetRangeUser(0,100)
    positivea.GetXaxis().SetTitle("#kappa [c/TeV]")
    positivea.GetYaxis().SetTitle("entries / %2.1f c/TeV]"%(positivea.GetXaxis().GetBinWidth(3)))
    positivea.SetStats(r.kFALSE)
    posinto = positiveo.Integral()
    neginto = negativeo.Integral()
    posinta = positivea.Integral()
    neginta = negativea.Integral()
    resultsCanvas.cd()
    positivea.Draw("ep0")
    negativea.Draw("ep0same")
    thelegend = r.TLegend(0.6,0.75,0.8,0.9)
    thelegend.SetHeader("#chi^{2}/ndf = %2.2f/%d"%(chi2Val,chi2ndf))
    thelegend.AddEntry(positivea,"#mu+ (%d,%d)"%(posinto,posinta))
    thelegend.AddEntry(negativea,"#mu- (%d,%d)"%(neginto,neginta))
    resultsCanvas.cd()
    thelegend.Draw()
    print "saving cosmics_kappa_bias_%s%d_%s"%(posneg,biasBin,outName)
    resultsCanvas.SaveAs("cosmics_kappa_bias_%s%d_%s_maxbias%.1f.pdf"%(posneg,biasBin,outName,maxBias))
    resultsCanvas.SaveAs("cosmics_kappa_bias_%s%d_%s_maxbias%.1f.eps"%(posneg,biasBin,outName,maxBias))
    resultsCanvas.SaveAs("cosmics_kappa_bias_%s%d_%s_maxbias%.1f.png"%(posneg,biasBin,outName,maxBias))
    resultsCanvas.SaveAs("cosmics_kappa_bias_%s%d_%s_maxbias%.1f.C"%(  posneg,biasBin,outName,maxBias))
    raw_input("enter to continue")

if __name__ == "__main__":
    parser = OptionParser(usage="Usage: %prog -i inputfile.root -o outputfile.root [-d]")
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      metavar="infile",
                      help="[REQUIRED] Location of the input ROOT files")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      metavar="outfile",
                      help="[OPTIONAL] Name of the output ROOT file")
    parser.add_option("-b", "--maxbias", type="float", dest="maxbias",
                      metavar="maxbias", default=0.1,
                      help="[] Maximum bias that was injected into the curvature (units of c/TeV)")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      metavar="debug",
                      help="[OPTIONAL] Debug mode")
    
    (options, args) = parser.parse_args()
    print options
    print args
    checkRequiredArguments(options, parser)

    debug = False
    if not options.debug:
        print "setting batch mode True"
        r.gROOT.SetBatch(True)
    else:
        print "setting batch mode False"
        r.gROOT.SetBatch(False)
        debug = True

    resultsCanvas = r.TCanvas("results","results",800,800)
    inputfile = r.TFile(options.infile,"READ")
    if not inputfile.IsOpen() or inputfile.IsZombie():
        print "Unable to open %s"%(options.infile)
        exit(1)
    #Chi2 is red
    paramsP = {"color":r.kRed,  "marker":r.kFullDiamond  , "stats":111111, "coords": {"x": [-1,-1], "y": [0.5,0.7]}}
    #KS is blue
    paramsM = {"color":r.kBlue, "marker":r.kFullDiamond, "stats":111111, "coords": {"x": [-1,-1], "y": [0.7,0.9]}}

    if options.debug:
        print inputfile

    chi2result  = inputfile.Get("obs_Minus/graph_tunep_Chi2")
    ksresult    = inputfile.Get("obs_Minus/graph_tunep_KS")
    
    chi2result.SetMarkerStyle(r.kFullDiamond)
    chi2result.SetMarkerColor(r.kRed)
    chi2result.SetMarkerSize(0.75)
    chi2result.SetLineColor(r.kRed)
    chi2result.SetLineWidth(2)
    chi2result.SetTitle("")
    chi2result.GetYaxis().SetTitle("result / %.1f [c/GeV]"%(options.maxbias))

    chi2result.SetMaximum(4)
    chi2result.SetMinimum(0)
    chi2result.Draw("alp")

    ksresult.SetMarkerStyle(r.kFullDiamond)
    ksresult.SetMarkerColor(r.kBlue)
    ksresult.SetMarkerSize(0.75)
    ksresult.SetLineColor(r.kBlue)
    ksresult.SetLineWidth(2)
    ksresult.Draw("lp")

    thelegend = r.TLegend(0.6,0.75,0.8,0.9)
    thelegend.SetHeader("TuneP")
    thelegend.AddEntry(chi2result,"#chi^{2}/ndf test")
    thelegend.AddEntry(ksresult,"KS test")
    thelegend.Draw()

    resultsCanvas.SaveAs("cosmics_scale_minimization_%s_maxbias%.1f.pdf"%(options.outfile,options.maxbias))
    resultsCanvas.SaveAs("cosmics_scale_minimization_%s_maxbias%.1f.eps"%(options.outfile,options.maxbias))
    resultsCanvas.SaveAs("cosmics_scale_minimization_%s_maxbias%.1f.png"%(options.outfile,options.maxbias))
    resultsCanvas.SaveAs("cosmics_scale_minimization_%s_maxbias%.1f.C"%(  options.outfile,options.maxbias))

    print "tunep"
    makeBiasPlot(inputfile,"looseMu%s_tunep"%(options.outfile),
                 "looseMu%sPlusCurve" %(options.outfile),
                 "looseMu%sMinusCurve"%(options.outfile),
                 options.outfile,
                 0,"",options.maxbias,1000)
    
    print "tunep pos_bias300"
    makeBiasPlot(inputfile,"looseMu%s_tunep_pos_bias300"%(options.outfile),
                 "looseMu%sPlusCurvePlusBias301" %(options.outfile),
                 "looseMu%sMinusCurvePlusBias301"%(options.outfile),
                 options.outfile,
                 300,"p",options.maxbias,1000)

    print "tunep pos_bias500"
    makeBiasPlot(inputfile,"looseMu%s_tunep_pos_bias500"%(options.outfile),
                 "looseMu%sPlusCurvePlusBias501" %(options.outfile),
                 "looseMu%sMinusCurvePlusBias501"%(options.outfile),
                 options.outfile,
                 500,"p",options.maxbias,1000)
    
    print "tunep neg_bias300"
    makeBiasPlot(inputfile,"looseMu%s_tunep_neg_bias300"%(options.outfile),
                 "looseMu%sPlusCurveMinusBias301" %(options.outfile),
                 "looseMu%sMinusCurveMinusBias301"%(options.outfile),
                 options.outfile,
                 300,"n",options.maxbias,1000)

    print "tunep neg_bias500"
    makeBiasPlot(inputfile,"looseMu%s_tunep_neg_bias500"%(options.outfile),
                 "looseMu%sPlusCurveMinusBias501" %(options.outfile),
                 "looseMu%sMinusCurveMinusBias501"%(options.outfile),
                 options.outfile,
                 500,"n",options.maxbias,1000)

    if options.debug:
        raw_input("enter to end")
