#!/bin/env python
import sys,os,re
import ROOT as r

from optparse import OptionParser
#from histograms import outputHistograms
from wsuPythonUtils import checkRequiredArguments
from wsuPyROOTUtils import makeNicePlot

if __name__ == "__main__":
    parser = OptionParser(usage="Usage: %prog -i inputfile.root -o outputfile.root [-d]")
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      metavar="infile",
                      help="[REQUIRED] Location of the input ROOT files")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      metavar="outfile",
                      help="[REQUIRED] Name of the output ROOT file")
    parser.add_option("-b", "--rebins", type="int", dest="rebins",
                      metavar="rebins", default=1,
                      help="[OPTIONAL] Number of bins to combine in the q/pT plot (default is 1)")
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

    looseCanvas    = r.TCanvas("loose",   "loose",   1600,900)
    looseEtaPlusPhiMinusCanvas    = r.TCanvas("looseEtaPlusPhiMinus",   "looseEtaPlusPhiMinus",   1600,900)
    looseEtaPlusPhiZeroCanvas    = r.TCanvas("looseEtaPlusPhiZero",   "looseEtaPlusPhiZero",   1600,900)
    looseEtaPlusPhiPlusCanvas    = r.TCanvas("looseEtaPlusPhiPlus",   "looseEtaPlusPhiPlus",   1600,900)
    looseEtaMinusPhiMinusCanvas    = r.TCanvas("looseEtaMinusPhiMinus",   "looseEtaMinusPhiMinus",   1600,900)
    looseEtaMinusPhiZeroCanvas    = r.TCanvas("looseEtaMinusPhiZero",   "looseEtaMinusPhiZero",   1600,900)
    looseEtaMinusPhiPlusCanvas    = r.TCanvas("looseEtaMinusPhiPlus",   "looseEtaMinusPhiPlus",   1600,900)
    #tightCanvas    = r.TCanvas("tight",   "tight",   1600,900)
    combinedCanvas = r.TCanvas("combined","combined",1600,900)
    counterCanvas  = r.TCanvas("counter", "counter", 1600,900)
    
    histograms = [
        "Chi2",
        "Ndof",
        "Charge",
        "TrackPt",
        "TrackEta",
        "TrackPhi",
        "PtRelErr",
        "Dz",
        "Dxy",
        "Curve",
        "ValidHits",
        "PixelHits",
        "TrackerHits",
        "ValidMuonHits",
        "MuonStationHits",
        "MatchedMuonStations",
        "TrackerLayersWithMeasurement",
        ]
    extensions = [
        "EtaPlusPhiMinus",
        "EtaPlusPhiZero",
        "EtaPlusPhiPlus",
        "EtaMinusPhiMinus",
        "EtaMinusPhiZero",
        "EtaMinusPhiPlus"
        ]

    tightmuons = [
        "TightMuon",
        "AntiTightMuon",
        ]
    cutmuons = [
        "looseMuPlus",
        "looseMuMinus",
        "tightMuPlus",
        "tightMuMinus",
        ]
    
    looseCanvas.Divide(5,4)
    looseEtaPlusPhiMinusCanvas.Divide(5,4)
    looseEtaPlusPhiZeroCanvas.Divide(5,4)
    looseEtaPlusPhiPlusCanvas.Divide(5,4)
    looseEtaMinusPhiMinusCanvas.Divide(5,4)
    looseEtaMinusPhiZeroCanvas.Divide(5,4)
    looseEtaMinusPhiPlusCanvas.Divide(5,4)
    #tightCanvas.Divide(5,4)
    combinedCanvas.Divide(5,4)
    inputfile = r.TFile(options.infile,"READ")
    if not inputfile.IsOpen() or inputfile.IsZombie():
        print "Unable to open %s"%(options.infile)
        exit(1)
    #plus/upper is red
    paramsP = {"color":r.kRed,  "marker":r.kFullCross  , "stats":111111, "coords": {"x": [-1,-1], "y": [0.5,0.7]}}
    #minus/lower is blue
    paramsM = {"color":r.kBlue, "marker":r.kFullDiamond, "stats":111111, "coords": {"x": [-1,-1], "y": [0.7,0.9]}}

    if options.debug:
        print inputfile

    for i,hist in enumerate(histograms):
        combinedMuUpper = inputfile.Get("%s%s"%("upper",hist))
        combinedMuLower = inputfile.Get("%s%s"%("lower",hist))

        looseMuPlus  = inputfile.Get("%s%s"%(cutmuons[0],hist))
        looseMuMinus = inputfile.Get("%s%s"%(cutmuons[1],hist))

        looseMuPlusEtaPlusPhiMinus = inputfile.Get("%s%s%s"%(cutmuons[0],hist,extensions[0]))
        looseMuMinusEtaPlusPhiMinus = inputfile.Get("%s%s%s"%(cutmuons[1],hist,extensions[0]))

        looseMuPlusEtaPlusPhiZero = inputfile.Get("%s%s%s"%(cutmuons[0],hist,extensions[1]))
        looseMuMinusEtaPlusPhiZero = inputfile.Get("%s%s%s"%(cutmuons[1],hist,extensions[1]))

        looseMuPlusEtaPlusPhiPlus = inputfile.Get("%s%s%s"%(cutmuons[0],hist,extensions[2]))
        looseMuMinusEtaPlusPhiPlus = inputfile.Get("%s%s%s"%(cutmuons[1],hist,extensions[2]))

        looseMuPlusEtaMinusPhiMinus = inputfile.Get("%s%s%s"%(cutmuons[0],hist,extensions[3]))
        looseMuMinusEtaMinusPhiMinus = inputfile.Get("%s%s%s"%(cutmuons[1],hist,extensions[3]))

        looseMuPlusEtaMinusPhiZero = inputfile.Get("%s%s%s"%(cutmuons[0],hist,extensions[4]))
        looseMuMinusEtaMinusPhiZero = inputfile.Get("%s%s%s"%(cutmuons[1],hist,extensions[4]))

        looseMuPlusEtaMinusPhiPlus = inputfile.Get("%s%s%s"%(cutmuons[0],hist,extensions[5]))
        looseMuMinusEtaMinusPhiPlus = inputfile.Get("%s%s%s"%(cutmuons[1],hist,extensions[5]))


        #tightMuPlus  = inputfile.Get("%s%s"%(cutmuons[2],hist))
        #tightMuMinus = inputfile.Get("%s%s"%(cutmuons[3],hist))

        if hist == "Curve":
            combinedMuUpper.Rebin(options.rebins)
            combinedMuLower.Rebin(options.rebins)
            
            looseMuPlus.Rebin(options.rebins)
            looseMuMinus.Rebin(options.rebins)

            looseMuPlusEtaPlusPhiMinus.Rebin(options.rebins)
            looseMuMinusEtaPlusPhiMinus.Rebin(options.rebins)

            looseMuPlusEtaPlusPhiZero.Rebin(options.rebins)
            looseMuMinusEtaPlusPhiZero.Rebin(options.rebins)

            looseMuPlusEtaPlusPhiPlus.Rebin(options.rebins)
            looseMuMinusEtaPlusPhiPlus.Rebin(options.rebins)

            looseMuPlusEtaMinusPhiMinus.Rebin(options.rebins)
            looseMuMinusEtaMinusPhiMinus.Rebin(options.rebins)

            looseMuPlusEtaMinusPhiZero.Rebin(options.rebins)
            looseMuMinusEtaMinusPhiZero.Rebin(options.rebins)

            looseMuPlusEtaMinusPhiPlus.Rebin(options.rebins)
            looseMuMinusEtaMinusPhiPlus.Rebin(options.rebins)

            #tightMuPlus.Rebin(options.rebins)
            #tightMuMinus.Rebin(options.rebins)
            
        looseCanvas.cd(i+1)
        looseMax = max(looseMuMinus.GetMaximum(),looseMuPlus.GetMaximum())
        looseMuMinus.SetMaximum(looseMax*1.2)
        looseMuMinus.Draw("ep0")
        looseMuPlus.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlus = makeNicePlot(looseMuPlus,paramsP,debug)
        looseMuMinus = makeNicePlot(looseMuMinus,paramsM,debug)
        r.gPad.Update()

        looseEtaPlusPhiMinusCanvas.cd(i+1)
        looseEtaPlusPhiMinusMax = max(looseMuMinusEtaPlusPhiMinus.GetMaximum(),looseMuPlusEtaPlusPhiMinus.GetMaximum())
        looseMuMinusEtaPlusPhiMinus.SetMaximum(looseEtaPlusPhiMinusMax*1.2)
        looseMuMinusEtaPlusPhiMinus.Draw("ep0")
        looseMuPlusEtaPlusPhiMinus.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlusEtaPlusPhiMinus = makeNicePlot(looseMuPlusEtaPlusPhiMinus,paramsP,debug)
        looseMuMinusEtaPlusPhiMinus = makeNicePlot(looseMuMinusEtaPlusPhiMinus,paramsM,debug)
        r.gPad.Update()

        looseEtaPlusPhiZeroCanvas.cd(i+1)
        looseEtaPlusPhiZeroMax = max(looseMuMinusEtaPlusPhiZero.GetMaximum(),looseMuPlusEtaPlusPhiZero.GetMaximum())
        looseMuMinusEtaPlusPhiZero.SetMaximum(looseEtaPlusPhiZeroMax*1.2)
        looseMuMinusEtaPlusPhiZero.Draw("ep0")
        looseMuPlusEtaPlusPhiZero.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlusEtaPlusPhiZero = makeNicePlot(looseMuPlusEtaPlusPhiZero,paramsP,debug)
        looseMuMinusEtaPlusPhiZero = makeNicePlot(looseMuMinusEtaPlusPhiZero,paramsM,debug)
        r.gPad.Update()

        looseEtaPlusPhiPlusCanvas.cd(i+1)
        looseEtaPlusPhiPlusMax = max(looseMuMinusEtaPlusPhiPlus.GetMaximum(),looseMuPlusEtaPlusPhiPlus.GetMaximum())
        looseMuMinusEtaPlusPhiPlus.SetMaximum(looseEtaPlusPhiPlusMax*1.2)
        looseMuMinusEtaPlusPhiPlus.Draw("ep0")
        looseMuPlusEtaPlusPhiPlus.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlusEtaPlusPhiPlus = makeNicePlot(looseMuPlusEtaPlusPhiPlus,paramsP,debug)
        looseMuMinusEtaPlusPhiPlus = makeNicePlot(looseMuMinusEtaPlusPhiPlus,paramsM,debug)
        r.gPad.Update()

        looseEtaMinusPhiMinusCanvas.cd(i+1)
        looseEtaMinusPhiMinusMax = max(looseMuMinusEtaMinusPhiMinus.GetMaximum(),looseMuPlusEtaMinusPhiMinus.GetMaximum())
        looseMuMinusEtaMinusPhiMinus.SetMaximum(looseEtaMinusPhiMinusMax*1.2)
        looseMuMinusEtaMinusPhiMinus.Draw("ep0")
        looseMuPlusEtaMinusPhiMinus.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlusEtaMinusPhiMinus = makeNicePlot(looseMuPlusEtaMinusPhiMinus,paramsP,debug)
        looseMuMinusEtaMinusPhiMinus = makeNicePlot(looseMuMinusEtaMinusPhiMinus,paramsM,debug)
        r.gPad.Update()

        looseEtaMinusPhiZeroCanvas.cd(i+1)
        looseEtaMinusPhiZeroMax = max(looseMuMinusEtaMinusPhiZero.GetMaximum(),looseMuPlusEtaMinusPhiZero.GetMaximum())
        looseMuMinusEtaMinusPhiZero.SetMaximum(looseEtaMinusPhiZeroMax*1.2)
        looseMuMinusEtaMinusPhiZero.Draw("ep0")
        looseMuPlusEtaMinusPhiZero.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlusEtaMinusPhiZero = makeNicePlot(looseMuPlusEtaMinusPhiZero,paramsP,debug)
        looseMuMinusEtaMinusPhiZero = makeNicePlot(looseMuMinusEtaMinusPhiZero,paramsM,debug)
        r.gPad.Update()

        looseEtaMinusPhiPlusCanvas.cd(i+1)
        looseEtaMinusPhiPlusMax = max(looseMuMinusEtaMinusPhiPlus.GetMaximum(),looseMuPlusEtaMinusPhiPlus.GetMaximum())
        looseMuMinusEtaMinusPhiPlus.SetMaximum(looseEtaMinusPhiPlusMax*1.2)
        looseMuMinusEtaMinusPhiPlus.Draw("ep0")
        looseMuPlusEtaMinusPhiPlus.Draw("ep0sames")
        r.gPad.Update()
        looseMuPlusEtaMinusPhiPlus = makeNicePlot(looseMuPlusEtaMinusPhiPlus,paramsP,debug)
        looseMuMinusEtaMinusPhiPlus = makeNicePlot(looseMuMinusEtaMinusPhiPlus,paramsM,debug)
        r.gPad.Update()



        #tightCanvas.cd(i+1)
        #tightMax = max(tightMuMinus.GetMaximum(),tightMuPlus.GetMaximum())
        #tightMuMinus.SetMaximum(tightMax*1.2)
        #tightMuMinus.Draw("ep0")
        #tightMuPlus.Draw("ep0sames")
        #r.gPad.Update()
        #tightMuPlus = makeNicePlot(tightMuPlus,paramsP,debug)
        #tightMuMinus = makeNicePlot(tightMuMinus,paramsM,debug)
        #r.gPad.Update()

        combinedCanvas.cd(i+1)
        combinedMax = max(combinedMuLower.GetMaximum(),combinedMuUpper.GetMaximum())
        combinedMuLower.SetMaximum(combinedMax*1.2)
        combinedMuLower.Draw("ep0")
        combinedMuUpper.Draw("ep0sames")
        r.gPad.Update()
        combinedMuUpper = makeNicePlot(combinedMuUpper,paramsP,debug)
        combinedMuLower = makeNicePlot(combinedMuLower,paramsM,debug)
        r.gPad.Update()

    # event counters after applying various cuts
    counterCanvas.cd()
    counterMuUpper = inputfile.Get("upperCounters")
    counterMuLower = inputfile.Get("lowerCounters")
    counterMuLower.SetTitle("Cut event counters")
    counterMuLower.Draw("ep0")
    counterMuUpper.Draw("ep0sames")
    r.gPad.Update()
    paramsP["stats"] = 11
    paramsP["coords"]["x"] = [0.8,0.9]
    paramsP["coords"]["y"] = [0.7,0.8]
    counterMuUpper = makeNicePlot(counterMuUpper,paramsP)
    paramsM["stats"] = 11
    paramsM["coords"]["x"] = [0.8,0.9]
    paramsM["coords"]["y"] = [0.8,0.9]
    counterMuLower = makeNicePlot(counterMuLower,paramsM)
    r.gPad.Update()
    
    looseCanvas.Update()
    #tightCanvas.Update()
    combinedCanvas.Update()
    counterCanvas.Update()
    if options.debug:
        raw_input("enter to end")
    output = r.TFile(options.outfile,"RECREATE")
    looseCanvas.Write()
    looseEtaPlusPhiMinusCanvas.Write()
    looseEtaPlusPhiZeroCanvas.Write()
    looseEtaPlusPhiPlusCanvas.Write()
    looseEtaMinusPhiMinusCanvas.Write()
    looseEtaMinusPhiZeroCanvas.Write()
    looseEtaMinusPhiPlusCanvas.Write()

    #tightCanvas.Write()
    combinedCanvas.Write()
    counterCanvas.Write()
    output.Write()
    output.Close()
