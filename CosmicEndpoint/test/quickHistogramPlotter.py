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
    combinedCanvas = r.TCanvas("combined","combined",1600,900)
    counterCanvas = r.TCanvas("counter","counter",1600,900)
    
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
        "TrackerLayersWithMeasurement",
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
    combinedCanvas.Divide(4,4)
    inputfile = r.TFile(options.infile,"READ")
    print inputfile
    for i,hist in enumerate(histograms):
        combinedMuUpper = inputfile.Get("%s%s"%("upper",hist))
        combinedMuLower = inputfile.Get("%s%s"%("lower",hist))

        looseMuPlus  = inputfile.Get("%s%s"%(loosemuons[0],hist))
        looseMuMinus = inputfile.Get("%s%s"%(loosemuons[1],hist))
        print tightmuons[0],hist
        tightMuPlus  = inputfile.Get("%s%s"%(tightmuons[0],hist))
        tightMuMinus = inputfile.Get("%s%s"%(tightmuons[1],hist))
        #if hist == "MuonStationHits":
        #    histname = "%sTight%s"%(tightmuons[0],hist)
        #    print histname
        #    tightMuPlus  = inputfile.Get(histname)
        #    tightMuMinus = inputfile.Get(histname)
        #if hist == "MatchedMuonStations":
        #    #histname = "%s%sTight%s"%(tightmuons[0],hist[:7],hist[7:])
        #    #print histname
        #    #tightMuPlus  = inputfile.Get(histname)
        #    #tightMuMinus = inputfile.Get(histname)
        #    combinedMuUpper = inputfile.Get("%s%s"%("upper",hist[:-1]))
        #    combinedMuLower = inputfile.Get("%s%s"%("lower",hist[:-1]))
        #if hist == "TrackLayersWithMeasurement":
        #    combinedMuUpper = inputfile.Get("%s%s"%("upper","TrackerLayersWithMeasurement"))
        #    combinedMuLower = inputfile.Get("%s%s"%("lower","TrackerLayersWithMeasurement"))
        
        
        #plus/upper is red
        tightMuPlus = makeNicePlot(tightMuPlus,r.kRed,r.kFullCross)
        looseMuPlus = makeNicePlot(looseMuPlus,r.kRed,r.kFullCross)
        combinedMuUpper = makeNicePlot(combinedMuUpper,r.kRed,r.kFullCross)
        #minus/lower is blue
        tightMuMinus = makeNicePlot(tightMuMinus,r.kBlue,r.kFullDiamond)
        looseMuMinus = makeNicePlot(looseMuMinus,r.kBlue,r.kFullDiamond)
        combinedMuLower = makeNicePlot(combinedMuLower,r.kBlue,r.kFullDiamond)
        
        looseCanvas.cd(i+1)
        looseMax = max(looseMuMinus.GetMaximum(),looseMuPlus.GetMaximum())
        looseMuMinus.SetMaximum(looseMax*1.2)
        looseMuMinus.Draw("ep0")
        looseMuPlus.Draw("ep0sames")

        tightCanvas.cd(i+1)
        tightMax = max(tightMuMinus.GetMaximum(),tightMuPlus.GetMaximum())
        tightMuMinus.SetMaximum(tightMax*1.2)
        tightMuMinus.Draw("ep0")
        tightMuPlus.Draw("ep0sames")

        combinedCanvas.cd(i+1)
        combinedMax = max(combinedMuLower.GetMaximum(),combinedMuUpper.GetMaximum())
        combinedMuLower.SetMaximum(combinedMax*1.2)
        combinedMuLower.Draw("ep0")
        combinedMuUpper.Draw("ep0sames")

    counterCanvas.cd()
    combinedMuUpper = inputfile.Get("upperCounters")
    combinedMuUpper = makeNicePlot(combinedMuUpper,r.kRed,r.kFullCross)
    combinedMuLower = inputfile.Get("lowerCounters")
    combinedMuLower = makeNicePlot(combinedMuLower,r.kBlue,r.kFullDiamond)
    combinedMuLower.Draw("ep0")
    combinedMuUpper.Draw("ep0sames")
    
    raw_input("enter to end")
    output = r.TFile(options.outfile,"RECREATE")
    looseCanvas.Write()
    tightCanvas.Write()
    combinedCanvas.Write()
    output.Write()
    output.Close()
