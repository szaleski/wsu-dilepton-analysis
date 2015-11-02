#!/bin/env python
import sys,os
import ROOT as r

from optparse import OptionParser
#from histograms import outputHistograms

def makeNicePlot(hist,color,marker):
    print hist
    hist.SetLineWidth(2)
    hist.SetLineColor(color)
    hist.SetMarkerStyle(marker)
    hist.SetMarkerColor(color)
    hist.SetMarkerStyle(marker)
    return hist

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      help="Location of the input ROOT files", metavar="infile")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      help="Name of the output ROOT file", metavar="outfile")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      help="Debug mode", metavar="debug")
    
    (options, args) = parser.parse_args()

    r.gROOT.SetBatch(False)

    looseCanvas = r.TCanvas("loose","loose",1600,900)
    
    tightCanvas = r.TCanvas("tight","tight",1600,900)
    
    histograms = [
        "Chi2",
        "TrackPt",
        "TrackEta",
        "TrackPhi",
        "PtRelErr",
        "Dz",
        "Dxy",
        "ValidHits",
        "PixelHits",
        "TrackerHits",
        "MuonStationHits",
        "MatchedMuonStations",
        "TrackLayersWithMeasurement",
        ]
    tightmuons = [
        "TightMuon",
        "AntiTightMuon",
        ]
    loosemuons = [
        "looseMuPlus",
        "looseMuMinus",
        ]
    
    looseCanvas.Divide(4,4)
    tightCanvas.Divide(4,4)
    inputfile = r.TFile(options.infile,"READ")
    print inputfile
    for i,hist in enumerate(histograms):
        looseMuPlus  = inputfile.Get("%s%s"%(loosemuons[0],hist))
        looseMuMinus = inputfile.Get("%s%s"%(loosemuons[1],hist))
        print tightmuons[0],hist
        tightMuPlus  = inputfile.Get("%s%s"%(tightmuons[0],hist))
        tightMuMinus = inputfile.Get("%s%s"%(tightmuons[1],hist))
        if hist == "MuonStationHits":
            histname = "%sTight%s"%(tightmuons[0],hist)
            print histname
            tightMuPlus  = inputfile.Get(histname)
            tightMuMinus = inputfile.Get(histname)
        if hist == "MatchedMuonStations":
            histname = "%s%sTight%s"%(tightmuons[0],hist[:7],hist[7:])
            print histname
            tightMuPlus  = inputfile.Get(histname)
            tightMuMinus = inputfile.Get(histname)
        
        
        #plus is red
        tightMuPlus = makeNicePlot(tightMuPlus,r.kRed,r.kFullCross)
        looseMuPlus = makeNicePlot(looseMuPlus,r.kRed,r.kFullCross)
        #minus is blue
        tightMuMinus = makeNicePlot(tightMuMinus,r.kBlue,r.kFullDiamond)
        looseMuMinus = makeNicePlot(looseMuMinus,r.kBlue,r.kFullDiamond)
        
        looseCanvas.cd(i+1)
        looseMuMinus.Draw("ep0")
        looseMuPlus.Draw("ep0sames")

        tightCanvas.cd(i+1)
        tightMuMinus.Draw("ep0")
        tightMuPlus.Draw("ep0sames")

    raw_input("enter to end")
    output = r.TFile(options.outfile,"RECREATE")
    looseCanvas.Write()
    tightCanvas.Write()
    output.Write()
    output.Close()
