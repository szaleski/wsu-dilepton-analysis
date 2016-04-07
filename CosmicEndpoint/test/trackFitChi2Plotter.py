#!/bin/env python

import ROOT as r
import numpy as np

# keep a pointer to the original TCanvas constructor
caninit = r.TCanvas.__init__

# define a new TCanvas class (inheriting from the original one),
# setting the memory ownership in the constructor
class GarbageCollectionResistentCanvas(r.TCanvas):
  def __init__(self, *args):
    caninit(self,*args)
    r.SetOwnership(self,False)

# replace the old TCanvas class by the new one
r.TCanvas = GarbageCollectionResistentCanvas

# keep a pointer to the original TPad constructor
padinit = r.TPad.__init__

# define a new TPad class (inheriting from the original one),
# setting the memory ownership in the constructor
class GarbageCollectionResistentCanvas(r.TPad):
  def __init__(self, *args):
    padinit(self,*args)
    r.SetOwnership(self,False)

# replace the old TPad class by the new one
r.TPad = GarbageCollectionResistentCanvas

# keep a pointer to the original TH1D constructor
th1dinit = r.TH1D.__init__

# define a new TH1D class (inheriting from the original one),
# setting the memory ownership in the constructor
class GarbageCollectionResistentH1D(r.TH1D):
  def __init__(self, *args):
    th1dinit(self,*args)
    r.SetOwnership(self,False)

# replace the old TH1D class by the new one
r.TH1D = GarbageCollectionResistentH1D

# keep a pointer to the original TLegend constructor
leginit = r.TLegend.__init__

# define a new TLegend class (inheriting from the original one),
# setting the memory ownership in the constructor
class GarbageCollectionResistentLegend(r.TLegend):
  def __init__(self, *args):
    leginit(self,*args)
    r.SetOwnership(self,False)

# replace the old TLegend class by the new one
r.TLegend = GarbageCollectionResistentLegend

# keep a pointer to the original TFile constructor
fileinit = r.TFile.__init__

# define a new TFile class (inheriting from the original one),
# setting the memory ownership in the constructor
class GarbageCollectionResistentFile(r.TFile):
  def __init__(self, *args):
    fileinit(self,*args)
    r.SetOwnership(self,False)

# replace the old TFile class by the new one
r.TFile = GarbageCollectionResistentFile

#r.gROOT.SetBatch(True)

outputCan = r.TCanvas("outcan","#chi^{2}/ndf",800,800)
outputCan.cd()
plotPad  = r.TPad("plotPad", "plotPad",0.0,0.0,1.0,1.0)
plotPad.SetFillStyle(4000)
plotPad.SetFrameFillStyle(4000)
plotPad.SetTopMargin(0.025)
plotPad.SetBottomMargin(0.06)
plotPad.SetLeftMargin(0.075)
plotPad.SetRightMargin(0.05)

plotPad.Draw()
plotPad.cd()

thelegend = r.TLegend(0.1,0.7,1.0,0.95)
thelegend.SetBorderSize(0)
thelegend.SetTextSize(0.015)
thelegend.SetLineWidth(0)
thelegend.SetFillColor(0)
thelegend.SetFillStyle(3000)

denLegend = "p_{T} > 53 && |#eta| < 0.9 && isTracker"
numLegend = "+#frac{#Delta p_{T}}{p_{T}} < 0.3 && N_{#mu hits} > 0 && N_{#mu stations} > 1"
samples = [
    "asym_deco_p100_v12",
    "startup_peak_p100_v12",
    "craft15_v12",
]
chi2d = []
chi2  = []
inputfile = []

histColours = {
    "chi2_":          r.kRed,
    "chi2_IsTrk":     r.kAzure,
    "chi2_IsGlb":     r.kPink+10,
    "chi2_IsGlbIsTrk":r.kOrange,
    "chi2__tight":          r.kGreen,
    "chi2_IsTrk_tight":     r.kYellow,
    "chi2_IsGlb_tight":     r.kBlack,
    "chi2_IsGlbIsTrk_tight":r.kMagenta,
    }
same = ""
for cut in ["","_tight"]:
    inputfile.append(r.TFile("eff_lower%s_%s.root"%(cut,samples[0]),"r"))
    print inputfile[-1]
    isTrk = ["", "IsTrk"]
    isGlb = ["", "IsGlb"]
    
    tightCut = ""
    if cut is "_tight":
        tightCut = " && |dxy| < 2.5 && |dz| < 10"
        print "setting tightCut to %s"%(tightCut)

    for hist in isTrk:
        for ghist in isGlb:
            histname = "muon%s%sDenominatorGlbNormChi2"%(ghist,hist)
            #print histname
            chi2d.append(inputfile[-1].Get(histname))
            #print chi2d
            chi2.append(chi2d[-1].ProjectionY("%s%s_py"%(chi2d[-1].GetName(),cut)))
            
            chi2Legend = ""
            if ghist == "IsGlb":
                chi2Legend = "%s"%(denLegend)
                pass
            else:
                chi2Legend = "%s%s"%(denLegend," && isGlobal")
                pass
            if hist == "IsTrk":
                chi2Legend = "%s%s%s"%(chi2Legend,tightCut," && N_{pix} > 0 && N_{trk layers} > 5 && firstPixel")
                pass
            else:
                chi2Legend = "%s%s"%(chi2Legend,tightCut)
                pass
            
            thelegend.AddEntry(chi2[-1], chi2Legend, "p")
            plotPad.cd()
            chi2[-1].SetLineWidth(2)
            chi2[-1].SetLineColor(  histColours["chi2_%s%s%s"%(ghist,hist,cut)])
            chi2[-1].SetMarkerColor(histColours["chi2_%s%s%s"%(ghist,hist,cut)])
            chi2[-1].SetMarkerStyle(r.kFullDiamond)
            chi2[-1].SetMarkerSize(1)
            chi2[-1].SetStats(0)
            chi2[-1].Draw("ep0%s"%same)
            plotPad.SetLogy(1)
            outputCan.cd()
            outputCan.Update()
            same = "sames"
            print "done with ghist %s"%(ghist)
            #raw_input("press enter to continue")
            pass
        print "done with hist %s"%(hist)
        pass
    print "done with cut %s"%(cut)
    pass
print "drawing legend",thelegend
print chi2
print inputfile
plotPad.cd()
plotPad.SetLogy(1)
thelegend.Draw("np")
outputCan.cd()
outputCan.Update()
#raw_input("press enter to save canvas")
print "saving canvas",outputCan
outputCan.SaveAs("~/public/html/Cosmics/chi2_comparison_%s.png"%(samples[0]))
outputCan.SaveAs("~/public/html/Cosmics/chi2_comparison_%s.pdf"%(samples[0]))
outputCan.SaveAs("~/public/html/Cosmics/chi2_comparison_%s.C"%(  samples[0]))
print "done!"
raw_input("press enter to exit")

chi2d = []
chi2  = []
inputfile = []

thelegend = r.TLegend(0.1,0.7,1.0,0.95)
thelegend.SetBorderSize(0)
thelegend.SetTextSize(0.015)
thelegend.SetLineWidth(0)
thelegend.SetFillColor(0)
thelegend.SetFillStyle(3000)

denCuts = ["FirstPix", "NPixHit", "NTkLayers", "RelPtErr", "NValidMuHits", "NMuStations", "IsGlobal"]
histColours = {
    "chi2_FirstPix":    r.kRed,
    "chi2_NPixHit":     r.kAzure,
    "chi2_NTkLayers":   r.kPink+10,
    "chi2_RelPtErr":    r.kOrange,
    "chi2_NValidMuHits":r.kGreen,
    "chi2_NMuStations": r.kMagenta,
    "chi2_IsGlobal":    r.kTeal,
    "chi2_FirstPix_tight":    r.kRed+3,
    "chi2_NPixHit_tight":     r.kAzure+3,
    "chi2_NTkLayers_tight":   r.kPink+7,
    "chi2_RelPtErr_tight":    r.kOrange+3,
    "chi2_NValidMuHits_tight":r.kGreen+3,
    "chi2_NMuStations_tight": r.kMagenta+3,
    "chi2_IsGlobal_tight":    r.kTeal+3,
    }
same = ""
for cut in ["","_tight"]:
    inputfile.append(r.TFile("eff_lower%s_%s.root"%(cut,samples[0]),"r"))
    print inputfile[-1]
    
    tightCut = ""
    if cut is "_tight":
        tightCut = " && |dxy| < 2.5 && |dz| < 10"
        print "setting tightCut to %s"%(tightCut)
        pass
    for dcut in denCuts:
        histname = "muonPass%s"%(dcut)
        # print histname
        chi2d.append(inputfile[-1].Get(histname))
        # print chi2d
        chi2.append(chi2d[-1].Clone("%s%s"%(chi2d[-1].GetName(),cut)))
    
        chi2Legend = "%s"%(dcut)
        thelegend.AddEntry(chi2[-1], chi2Legend, "p")
        plotPad.cd()
        chi2[-1].SetLineWidth(2)
        chi2[-1].SetLineColor(  histColours["chi2_%s%s"%(dcut,cut)])
        chi2[-1].SetMarkerColor(histColours["chi2_%s%s"%(dcut,cut)])
        chi2[-1].SetMarkerStyle(r.kFullDiamond)
        chi2[-1].SetMarkerSize(1)
        chi2[-1].SetStats(0)
        chi2[-1].Draw("ep0%s"%same)
        plotPad.SetLogy(1)
        outputCan.cd()
        outputCan.Update()
        same = "sames"
        print "done with dcut %s"%(dcut)
        raw_input("press enter to continue")
        pass
    pass
print "drawing legend",thelegend
print chi2
print inputfile
plotPad.cd()
plotPad.SetLogy(1)
thelegend.Draw("np")
outputCan.cd()
outputCan.Update()
raw_input("press enter to save canvas")
print "saving canvas",outputCan
outputCan.SaveAs("~/public/html/Cosmics/chi2_comparison2_%s.png"%(samples[0]))
outputCan.SaveAs("~/public/html/Cosmics/chi2_comparison2_%s.pdf"%(samples[0]))
outputCan.SaveAs("~/public/html/Cosmics/chi2_comparison2_%s.C"%(  samples[0]))
print "done!"
raw_input("press enter to exit")
