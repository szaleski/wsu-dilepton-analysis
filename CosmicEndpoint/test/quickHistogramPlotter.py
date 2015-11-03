#!/bin/env python
import sys,os,re
import ROOT as r

from optparse import OptionParser
#from histograms import outputHistograms

def makeNicePlot(hist,color,marker,debug=False):
    if debug:
        print hist
    hist.SetLineWidth(2)
    hist.SetLineColor(color)
    hist.SetMarkerStyle(marker)
    hist.SetMarkerColor(color)
    hist.SetMarkerStyle(marker)
    return hist

def checkRequiredArguments(opts, parser):
    """From: http://stackoverflow.com/questions/4407539/python-how-to-make-an-option-to-be-required-in-optparse"""
    missing_options = []
    for option in parser.option_list:
        if re.match(r'^\[REQUIRED\]', option.help) and eval('opts.' + option.dest) == None:
            missing_options.extend(option._long_opts)
        if len(missing_options) > 0:
            parser.error('Missing REQUIRED parameters: ' + str(missing_options))
    return                                                        
                                                        
if __name__ == "__main__":
    parser = OptionParser(usage="Usage: %prog -i inputfile.root -o outputfile.root [-d]")
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      help="[REQUIRED] Location of the input ROOT files", metavar="infile")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      help="[REQUIRED] Name of the output ROOT file", metavar="outfile")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      help="[OPTIONAL] Debug mode", metavar="debug")
    
    (options, args) = parser.parse_args()
    checkRequiredArguments(options, parser)
    
    if not options.debug:
        print "setting batch mode True"
        r.gROOT.SetBatch(True)
    else:
        print "setting batch mode False"
        r.gROOT.SetBatch(False)

    looseCanvas    = r.TCanvas("loose","loose",1600,900)
    tightCanvas    = r.TCanvas("tight","tight",1600,900)
    combinedCanvas = r.TCanvas("combined","combined",1600,900)
    counterCanvas  = r.TCanvas("counter","counter",1600,900)
    
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
        "MuonStationHits",
        "MatchedMuonStations",
        "TrackerLayersWithMeasurement",
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
    
    looseCanvas.Divide(4,4)
    tightCanvas.Divide(4,4)
    combinedCanvas.Divide(4,4)
    inputfile = r.TFile(options.infile,"READ")
    if options.debug:
        print inputfile
    for i,hist in enumerate(histograms):
        combinedMuUpper = inputfile.Get("%s%s"%("upper",hist))
        combinedMuLower = inputfile.Get("%s%s"%("lower",hist))

        looseMuPlus  = inputfile.Get("%s%s"%(cutmuons[0],hist))
        looseMuMinus = inputfile.Get("%s%s"%(cutmuons[1],hist))
        if options.debug:
            print tightmuons[0],hist
        tightMuPlus  = inputfile.Get("%s%s"%(cutmuons[2],hist))
        tightMuMinus = inputfile.Get("%s%s"%(cutmuons[3],hist))
        
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
        r.gPad.Update()
        lminus = looseMuMinus.FindObject("stats")
        lminus.SetTextColor(r.kRed)
        lminus.SetOptStat(111111)
        lminus.SetY1NDC(0.5)
        lminus.SetY2NDC(0.7)
        looseMuPlus.Draw("ep0sames")
        r.gPad.Update()
        lplus = looseMuPlus.FindObject("stats")
        lplus.SetTextColor(r.kBlue)
        lplus.SetOptStat(111111)
        lplus.SetY1NDC(0.7)
        lplus.SetY2NDC(0.9)
        r.gPad.Update()

        tightCanvas.cd(i+1)
        tightMax = max(tightMuMinus.GetMaximum(),tightMuPlus.GetMaximum())
        tightMuMinus.SetMaximum(tightMax*1.2)
        tightMuMinus.Draw("ep0")
        r.gPad.Update()
        tminus = tightMuMinus.FindObject("stats")
        tminus.SetTextColor(r.kRed)
        tminus.SetOptStat(111111)
        tminus.SetY1NDC(0.5)
        tminus.SetY2NDC(0.7)
        tightMuPlus.Draw("ep0sames")
        r.gPad.Update()
        tplus = tightMuPlus.FindObject("stats")
        tplus.SetTextColor(r.kBlue)
        tplus.SetOptStat(111111)
        tplus.SetY1NDC(0.7)
        tplus.SetY2NDC(0.9)
        r.gPad.Update()

        combinedCanvas.cd(i+1)
        combinedMax = max(combinedMuLower.GetMaximum(),combinedMuUpper.GetMaximum())
        combinedMuLower.SetMaximum(combinedMax*1.2)
        combinedMuLower.Draw("ep0")
        r.gPad.Update()
        lower = combinedMuLower.FindObject("stats")
        lower.SetTextColor(r.kRed)
        lower.SetOptStat(111111)
        lower.SetY1NDC(0.5)
        lower.SetY2NDC(0.7)
        combinedMuUpper.Draw("ep0sames")
        r.gPad.Update()
        upper = combinedMuUpper.FindObject("stats")
        upper.SetTextColor(r.kBlue)
        upper.SetOptStat(111111)
        upper.SetY1NDC(0.7)
        upper.SetY2NDC(0.9)
        r.gPad.Update()

    counterCanvas.cd()
    combinedMuUpper = inputfile.Get("upperCounters")
    combinedMuUpper = makeNicePlot(combinedMuUpper,r.kRed,r.kFullCross)
    combinedMuLower = inputfile.Get("lowerCounters")
    combinedMuLower = makeNicePlot(combinedMuLower,r.kBlue,r.kFullDiamond)
    combinedMuLower.SetTitle("Cut event counters")
    combinedMuLower.Draw("ep0")
    counterCanvas.cd()
    r.gPad.Update()
    lower = combinedMuLower.FindObject("stats")
    lower.SetTextColor(r.kRed)
    lower.SetOptStat(11)
    lower.SetX1NDC(0.8)
    lower.SetX2NDC(0.9)
    lower.SetY1NDC(0.7)
    lower.SetY2NDC(0.8)
    combinedMuUpper.Draw("ep0sames")
    counterCanvas.cd()
    r.gPad.Update()
    upper = combinedMuUpper.FindObject("stats")
    upper.SetTextColor(r.kBlue)
    upper.SetOptStat(11)
    upper.SetX1NDC(0.8)
    upper.SetX2NDC(0.9)
    upper.SetY1NDC(0.8)
    upper.SetY2NDC(0.9)
    counterCanvas.cd()
    r.gPad.Update()
    
    if options.debug:
        raw_input("enter to end")
    output = r.TFile(options.outfile,"RECREATE")
    looseCanvas.Write()
    tightCanvas.Write()
    combinedCanvas.Write()
    counterCanvas.Write()
    output.Write()
    output.Close()
