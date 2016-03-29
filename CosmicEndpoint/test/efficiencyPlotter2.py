#!/bin/env python

import ROOT as r
import numpy as np

samples = [
    #"asym_deco_p10_v2",
    #"startup_peak_p10_v2",
    #"asym_deco_p100_v2",
    #"startup_peak_p100_v2",
    #"asym_deco_p10_v3",
    #"startup_peak_p10_v3",
    #"asym_deco_p100_v3",
    #"startup_peak_p100_v3",
    #"asym_deco_p10_v4",
    #"startup_peak_p10_v4",
    #"asym_deco_p100_v4",
    #"lower_asym_deco_p10_v5",
    #"lower_tight_asym_deco_p10_v5",
    #"upper_asym_deco_p10_v5",
    #"upper_tight_asym_deco_p10_v5",
    #"lower_asym_deco_p100_v5",
    #"lower_tight_asym_deco_p100_v5",
    #"upper_asym_deco_p100_v5",
    #"upper_tight_asym_deco_p100_v5",
    #"lower_asym_deco_p10_v6",
    #"lower_tight_asym_deco_p10_v6",
    #"upper_asym_deco_p10_v6",
    #"upper_tight_asym_deco_p10_v6",
    #"lower_asym_deco_p100_v6",
    #"lower_tight_asym_deco_p100_v6",
    #"upper_asym_deco_p100_v6",
    #"upper_tight_asym_deco_p100_v6",
    #"lower_craft15_v6",
    #"lower_tight_craft15_v6",
    #"upper_craft15_v6",
    #"upper_tight_craft15_v6",
    #"lower_interfill2015d_v6",
    #"lower_tight_interfill2015d_v6",
    #"upper_interfill2015d_v6",
    #"upper_tight_interfill2015d_v6",
    #"lower_asym_deco_p10_v10",
    #"lower_tight_asym_deco_p10_v10",
    #"upper_asym_deco_p10_v10",
    #"upper_tight_asym_deco_p10_v10",
    "lower_asym_deco_p100_tag",
    "lower_tight_asym_deco_p100_tag",
    "upper_asym_deco_p100_tag",
    "upper_tight_asym_deco_p100_tag",
    #"lower_startup_peak_p100_tag",
    #"lower_tight_startup_peak_p100_tag",
    #"upper_startup_peak_p100_tag",
    #"upper_tight_startup_peak_p100_tag",
    "lower_craft15_tag",
    "lower_tight_craft15_tag",
    "upper_craft15_tag",
    "upper_tight_craft15_tag",
    #"lower_interfill2015d_tag",
    #"lower_tight_interfill2015d_tag",
    #"upper_interfill2015d_tag",
    #"upper_tight_interfill2015d_tag",
    #"startup_peak_p100_v4",
    #"craft15",
    #"interfill2015d"
           ]
#samples = [
#    "test-eff-lower-p10",
#    "test-eff-upper-p10"
#    ]
isTrk = ["", "IsTrk"]
isGlb = ["", "IsGlb"]
ptBins = np.array([0., 50., 100., 150., 200., 300., 500., 1000.])
#ptBins = np.array([50.,60.,75.,100.,125.,150.,175.,200.,250.,300.,500.,3000.])
r.gROOT.SetBatch(True)

## v12
# denominator: isGlobal && isTracker && pT > 53 && |eta| < 0.9
# IsTrk: passFirstPixel && nPixHits > 0 && nTkLayers > 5
# numerator: relPtErr < 0.3 && nValidMuHits > 0 && nMatchedMuonStations > 1
# IsGlb: 

## v13
# denominator: isTracker && pT > 53 && |eta| < 0.9
# IsTrk: passFirstPixel && nPixHits > 0 && nTkLayers > 5
# numerator: relPtErr < 0.3 && nValidMuHits > 0 && nMatchedMuonStations > 1
# IsGlb: isGlobal

## v14
# denominator: passFirstPixel && isTracker && pT > 53 && |eta| < 0.9
# IsTrk: passFirstPixel && nPixHits > 0 && nTkLayers > 5
# numerator: relPtErr < 0.3 && nValidMuHits > 0 && nMatchedMuonStations > 1
# IsGlb: isGlobal

## v15
# require tag reco::Muon (opposite leg) passing all high-pT selections
# denominator: isTracker && pT > 53 && |eta| < 0.9
# IsTrk: passFirstPixel && nPixHits > 0 && nTkLayers > 5
# numerator: relPtErr < 0.3 && nValidMuHits > 0 && nMatchedMuonStations > 1
# IsGlb: isGlobal

## trk
# require tag reco::Track (same leg) passing basic track/kinematic cuts
# denominator: require match to reco::Muon
# ID numerator  : relPtErr < 0.3 && nValidMuHits > 0 && nMatchedMuonStations > 1
# reco numerator: nPixHits > 0 && nTkLayers > 5
# IsTrk: passFirstPixel (added to reco num.)
# IsGlb: isGlobal (added to ID num.)

## tag
# require tag reco::Muon (opposite leg) passing all high-pT selections
# denominator: require match to reco::Muon
# IsTrk: passFirstPixel (added to den.)
# ID numerator  : relPtErr < 0.3 && nValidMuHits > 0 && nMatchedMuonStations > 1
# reco numerator: nPixHits > 0 && nTkLayers > 5
# IsGlb: isGlobal (added to ID num.), passFirstPixel (added to reco num.)

for sample in samples:
    tightCut = ""
    if sample.rfind("tight") > 0:
        tightCut = " && |dxy| < 2.5 && |dz| < 10"
    inputfile = r.TFile("eff_%s.root"%(sample),"r")
    print inputfile
    for hist in isTrk:
        for ghist in isGlb:
            denPt = inputfile.Get("muon%s%sDenominatorPt"%(ghist,hist))
            print denPt,"muon%s%sDenominatorPt"%(ghist,hist)
            denPt = denPt.Rebin(len(ptBins)-1,"muon%s%sDenominatorPt_rebinned"%(ghist,hist),ptBins)
            denPt.SetLineColor(r.kBlack)
            denPt.SetLineWidth(2)
            
            passIDPt = inputfile.Get("muon%s%sPassIDPt"%(ghist,hist))
            passIDPt = passIDPt.Rebin(len(ptBins)-1,"muon%s%sPassIDPt_rebinned"%(ghist,hist),ptBins)
            passIDPt.SetLineColor(r.kBlue)
            passIDPt.SetLineWidth(2)
            
            passIDTrkPt = inputfile.Get("muon%s%sPassIDTrkPt"%(ghist,hist))
            passIDTrkPt = passIDTrkPt.Rebin(len(ptBins)-1,"muon%s%sPassIDTrkPt_rebinned"%(ghist,hist),ptBins)
            passIDTrkPt.SetLineColor(r.kRed)
            passIDTrkPt.SetLineWidth(2)
            
            outputCan   = r.TCanvas("outcan","Efficiency vs. p_{T}",800,800)
            outputCan.cd()
            plotPad  = r.TPad("plotPad", "plotPad",0.0,0.3,1.0,1.0)
            plotPad.SetFillStyle(4000)
            plotPad.SetFrameFillStyle(4000)
            plotPad.SetTopMargin(0.025)
            plotPad.SetBottomMargin(0.06)
            plotPad.SetLeftMargin(0.075)
            plotPad.SetRightMargin(0.05)
            
            plotPad.Draw()
            
            outputCan.cd()
            ratioPad = r.TPad("ratioPad","ratioPad",0.0,0.0,1.0,0.3)
            ratioPad.SetFillStyle(4000)
            ratioPad.SetFrameFillStyle(4000)
            ratioPad.SetTopMargin(0.03)
            ratioPad.SetBottomMargin(0.1)
            ratioPad.SetLeftMargin(0.075)
            ratioPad.SetRightMargin(0.05)
            ratioPad.Draw()
            
            # effToID    = r.TGraphAsymmErrors(len(ptBins)-1)
            # effToIDTrk = r.TGraphAsymmErrors(len(ptBins)-1)
            # effToID.Divide(passIDPt,      denPt, "cl=0.683 b(1,1) mode")
            # effToIDTrk.Divide(passIDTrkPt,denPt, "cl=0.683 b(1,1) mode")
            
            effToID    = r.TGraphAsymmErrors(passIDPt,   denPt, "cl=0.683 b(1,1) mode")
            effToIDTrk = r.TGraphAsymmErrors(passIDTrkPt,denPt, "cl=0.683 b(1,1) mode")

            outputCan.cd()
            plotPad.cd()
            #denPt.SetXTitle("p_{T} [GeV]")
            denPt.SetTitleOffset(1.)
            denPt.GetXaxis().SetLabelSize(0.04)
            denPt.GetXaxis().SetLabelOffset(0.005)
            denPt.GetYaxis().SetLabelSize(0.035)
            denPt.GetXaxis().SetNdivisions(510)
            minimum = 0.8*passIDTrkPt.GetMinimum(10)
            if minimum == 0 or minimum < 0:
                minimum = 0.01
            denPt.GetYaxis().SetRangeUser(0.5*minimum,1.2*denPt.GetMaximum())
            denPt.SetStats(0)
            denPt.SetTitle("")
            denPt.Draw("ep0")
            passIDPt.Draw("ep0same")
            passIDTrkPt.Draw("ep0same")
            plotPad.SetLogy(1)
            plotPad.SetGridy(1)
            plotPad.SetGridx(1)
            
            thelegend = r.TLegend(0.15,0.7,0.9,0.9)
            thelegend.SetBorderSize(0)
            thelegend.SetTextSize(0.025)
            thelegend.SetLineWidth(0)
            thelegend.SetFillColor(0)
            thelegend.SetFillStyle(3000)

            denLegend     = "tag:upper leg reco::Muon matched to probe:lower leg reco::Muon"
            denSubLegend  = "full high-p_{T} && p_{T} > 45. && |#eta| < 0.9 && |D_{xy}|< 4. && |D_{z}| < 10."
            numLegendI = "probe:#frac{#Delta p_{T}}{p_{T}} < 0.3 && N_{#mu hits} > 0 && N_{#mu stations} > 1"
            numLegendR = "probe:N_{pix} > 0 && N_{trk layers} > 5"
            if ghist == "IsGlb":
                numLegendI = numLegendI + " && isGlobal"
                numLegendR = numLegendR + " && firstPixel"
            else:
                pass
            if hist == "IsTrk":
                denSubLegend = denSubLegend + " && firstPixel"
            else:
                pass
            thelegend.AddEntry(denPt,      denLegend,    "lpe")
            thelegend.AddEntry(None,       denSubLegend, "")
            thelegend.AddEntry(passIDPt,   numLegendI, "lpe")
            thelegend.AddEntry(passIDTrkPt,numLegendR, "lpe")
            thelegend.Draw("nb")
            
            ratioPad.cd()
            effToID.SetTitle("")
            effToID.SetMarkerStyle(r.kFullDiamond)
            effToID.SetMarkerSize(1)
            effToID.SetMarkerColor(r.kBlue)
            effToID.SetLineColor(r.kBlue)
            effToID.SetLineWidth(2)
            effToID.Draw("ACP+")
            effToID.GetXaxis().SetTitle("p_{T} [GeV]")
            effToID.GetXaxis().SetNdivisions(510)
            effToID.GetXaxis().SetTitleOffset(-0.75)
            effToID.GetXaxis().SetTitleSize(0.075)
            effToID.GetXaxis().SetLabelSize(0)
            effToID.GetXaxis().SetRangeUser(ptBins[0],ptBins[-1])
            effToID.GetYaxis().SetNdivisions(410)
            effToID.GetYaxis().SetLabelSize(0.07)
            effToID.GetYaxis().SetTickLength(0.02)
            effToID.GetYaxis().SetRangeUser(0.9,1.)
            
            effToIDTrk.SetMarkerStyle(r.kFullDiamond)
            effToIDTrk.SetMarkerSize(1)
            effToIDTrk.SetMarkerColor(r.kRed)
            effToIDTrk.SetLineColor(r.kRed)
            effToIDTrk.SetLineWidth(2)
            effToIDTrk.GetXaxis().SetTitle("p_{T} [GeV]")
            effToIDTrk.GetXaxis().SetNdivisions(510)
            effToIDTrk.GetXaxis().SetTitleOffset(-0.75)
            effToIDTrk.GetXaxis().SetTitleSize(0.075)
            effToIDTrk.GetXaxis().SetLabelSize(0)
            effToIDTrk.GetXaxis().SetRangeUser(ptBins[0],ptBins[-1])
            effToIDTrk.GetYaxis().SetNdivisions(410)
            effToIDTrk.GetYaxis().SetLabelSize(0.07)
            effToIDTrk.GetYaxis().SetTickLength(0.02)
            effToIDTrk.GetYaxis().SetRangeUser(0.9,1.)
            effToIDTrk.Draw("CP+")
            
            ratioPad.SetGridy(1)
            ratioPad.SetGridx(1)
            outputCan.cd()
            outputCan.Update()
            outputCan.SaveAs("~/public/html/Cosmics/eff_%s_%s_%s.png"%(sample,hist,ghist))
            outputCan.SaveAs("~/public/html/Cosmics/eff_%s_%s_%s.pdf"%(sample,hist,ghist))
            outputCan.SaveAs("~/public/html/Cosmics/eff_%s_%s_%s.C"%(  sample,hist,ghist))

            # look at the efficiency of each cut separately
            if hist == "" and ghist == "IsGlb":
                passFirstPixel = inputfile.Get("muonPassFirstPix")
                passFirstPixel = passFirstPixel.Rebin(len(ptBins)-1,"muonPassFirstPix_rebinned",ptBins)
                passFirstPixel.SetLineColor(r.kOrange)
                passFirstPixel.SetLineWidth(2)

                passNPixHits = inputfile.Get("muonPassNPixHit")
                passNPixHits = passNPixHits.Rebin(len(ptBins)-1,"muonPassNPixHit_rebinned",ptBins)
                passNPixHits.SetLineColor(r.kBlue)
                passNPixHits.SetLineWidth(2)

                passNTkLayers = inputfile.Get("muonPassNTkLayers")
                passNTkLayers = passNTkLayers.Rebin(len(ptBins)-1,"muonPassTkMeas_rebinned",ptBins)
                passNTkLayers.SetLineColor(r.kRed)
                passNTkLayers.SetLineWidth(2)

                passRelPtErr = inputfile.Get("muonPassRelPtErr")
                passRelPtErr = passRelPtErr.Rebin(len(ptBins)-1,"muonPassRelPtErr_rebinned",ptBins)
                passRelPtErr.SetLineColor(r.kGreen)
                passRelPtErr.SetLineWidth(2)

                passNValidMuHits = inputfile.Get("muonPassNValidMuHits")
                passNValidMuHits = passNValidMuHits.Rebin(len(ptBins)-1,"muonPassNValidMuHits_rebinned",ptBins)
                passNValidMuHits.SetLineColor(r.kYellow)
                passNValidMuHits.SetLineWidth(2)

                passNMatchedStations = inputfile.Get("muonPassNMuStations")
                passNMatchedStations = passNMatchedStations.Rebin(len(ptBins)-1,
                                                                  "muonPassNMuStations_rebinned",ptBins)
                passNMatchedStations.SetLineColor(r.kViolet)
                passNMatchedStations.SetLineWidth(2)

                passIsGlobal = inputfile.Get("muonPassIsGlobal")
                passIsGlobal = passIsGlobal.Rebin(len(ptBins)-1,"muonPassIsGlobal_rebinned",ptBins)
                passIsGlobal.SetLineColor(r.kMagenta)
                passIsGlobal.SetLineWidth(2)

                outputCan.cd()
                plotPad.cd()
                denPt.Draw("ep0")
                passFirstPixel.Draw("ep0same")
                passNPixHits.Draw("ep0same")
                passNTkLayers.Draw("ep0same")
                passRelPtErr.Draw("ep0same")
                passNValidMuHits.Draw("ep0same")
                passNMatchedStations.Draw("ep0same")
                passIsGlobal.Draw("ep0same")

                plotPad.SetLogy(1)
                plotPad.SetGridy(1)
                plotPad.SetGridx(1)

                thelegend2 = r.TLegend(0.35,0.75,0.9,0.975)
                thelegend2.SetNColumns(2)
                thelegend2.SetBorderSize(0)
                thelegend2.SetTextSize(0.025)
                thelegend2.SetLineWidth(0)
                thelegend2.SetFillColor(0)
                thelegend2.SetFillStyle(3000)

                denLegend     = "tag:upper leg reco::Muon matched to probe:lower leg reco::Muon"
                thelegend2.AddEntry(denPt,                "tag:upper leg reco::Muon matched to probe:lower leg reco::Muon", "lpe")
                thelegend2.AddEntry(None,                 "", "")
                thelegend2.AddEntry(None,                 "full high-p_{T} && p_{T} > 45. && |#eta| < 0.9", "")
                thelegend2.AddEntry(None,                 "&& |D_{xy}|< 4. && |D_{z}| < 10.", "")
                thelegend2.AddEntry(passFirstPixel,       "probe:first pixel layer",                      "lpe")
                thelegend2.AddEntry(passNPixHits,         "probe:N_{pix} > 0",                            "lpe")
                thelegend2.AddEntry(passNTkLayers,        "probe:N_{trk layers} > 5",                     "lpe")
                thelegend2.AddEntry(passRelPtErr,         "probe:#frac{#Delta p_{T}}{p_{T}} < 0.3",       "lpe")
                thelegend2.AddEntry(passNValidMuHits,     "probe:N_{#mu hits} > 0",                       "lpe")
                thelegend2.AddEntry(passNMatchedStations, "probe:N_{#mu stations} > 1",                   "lpe")
                thelegend2.AddEntry(passIsGlobal,         "probe:isGlobal",                               "lpe")
                thelegend2.Draw("nb")

                effFirstPixel       = r.TGraphAsymmErrors(passFirstPixel,denPt,       "cl=0.683 b(1,1) mode")
                effNPixHits         = r.TGraphAsymmErrors(passNPixHits,denPt,         "cl=0.683 b(1,1) mode")
                effNTkLayers        = r.TGraphAsymmErrors(passNTkLayers,denPt,        "cl=0.683 b(1,1) mode")
                effRelPtErr         = r.TGraphAsymmErrors(passRelPtErr,denPt,         "cl=0.683 b(1,1) mode")
                effNValidMuHits     = r.TGraphAsymmErrors(passNValidMuHits,denPt,     "cl=0.683 b(1,1) mode")
                effNMatchedStations = r.TGraphAsymmErrors(passNMatchedStations,denPt, "cl=0.683 b(1,1) mode")
                effIsGlobal         = r.TGraphAsymmErrors(passIsGlobal,denPt,         "cl=0.683 b(1,1) mode")
                
                ratioPad.cd()
                effNPixHits.SetTitle("")
                effNPixHits.SetMarkerStyle(r.kFullDiamond)
                effNPixHits.SetMarkerSize(1)
                effNPixHits.SetMarkerColor(r.kBlue)
                effNPixHits.SetLineColor(r.kBlue)
                effNPixHits.SetLineWidth(2)
                effNPixHits.Draw("ACP+")
                effNPixHits.GetXaxis().SetTitle("p_{T} [GeV]")
                effNPixHits.GetXaxis().SetNdivisions(510)
                effNPixHits.GetXaxis().SetTitleOffset(-0.75)
                effNPixHits.GetXaxis().SetTitleSize(0.075)
                effNPixHits.GetXaxis().SetLabelSize(0)
                effNPixHits.GetXaxis().SetRangeUser(ptBins[0],ptBins[-1])
                effNPixHits.GetYaxis().SetNdivisions(410)
                effNPixHits.GetYaxis().SetLabelSize(0.07)
                effNPixHits.GetYaxis().SetTickLength(0.02)
                effNPixHits.GetYaxis().SetRangeUser(0.75,1.)

                effFirstPixel.SetMarkerStyle(r.kFullDiamond)
                effFirstPixel.SetMarkerSize(1)
                effFirstPixel.SetMarkerColor(r.kOrange)
                effFirstPixel.SetLineColor(r.kOrange)
                effFirstPixel.SetLineWidth(2)
                effFirstPixel.Draw("CP+")

                effNTkLayers.SetMarkerStyle(r.kFullDiamond)
                effNTkLayers.SetMarkerSize(1)
                effNTkLayers.SetMarkerColor(r.kRed)
                effNTkLayers.SetLineColor(r.kRed)
                effNTkLayers.SetLineWidth(2)
                effNTkLayers.Draw("CP+")

                effRelPtErr.SetMarkerStyle(r.kFullDiamond)
                effRelPtErr.SetMarkerSize(1)
                effRelPtErr.SetMarkerColor(r.kGreen)
                effRelPtErr.SetLineColor(r.kGreen)
                effRelPtErr.SetLineWidth(2)
                effRelPtErr.Draw("CP+")

                effNValidMuHits.SetMarkerStyle(r.kFullDiamond)
                effNValidMuHits.SetMarkerSize(1)
                effNValidMuHits.SetMarkerColor(r.kYellow)
                effNValidMuHits.SetLineColor(r.kYellow)
                effNValidMuHits.SetLineWidth(2)
                effNValidMuHits.Draw("CP+")

                effNMatchedStations.SetMarkerStyle(r.kFullDiamond)
                effNMatchedStations.SetMarkerSize(1)
                effNMatchedStations.SetMarkerColor(r.kViolet)
                effNMatchedStations.SetLineColor(r.kViolet)
                effNMatchedStations.SetLineWidth(2)
                effNMatchedStations.Draw("CP+")

                effIsGlobal.SetMarkerStyle(r.kFullDiamond)
                effIsGlobal.SetMarkerSize(1)
                effIsGlobal.SetMarkerColor(r.kMagenta)
                effIsGlobal.SetLineColor(r.kMagenta)
                effIsGlobal.SetLineWidth(2)
                effIsGlobal.Draw("CP+")

                ratioPad.SetGridy(1)
                ratioPad.SetGridx(1)
                outputCan.cd()
                outputCan.Update()
                outputCan.SaveAs("~/public/html/Cosmics/eff_%s_comparison.png"%(sample))
                outputCan.SaveAs("~/public/html/Cosmics/eff_%s_comparison.pdf"%(sample))
                outputCan.SaveAs("~/public/html/Cosmics/eff_%s_comparison.C"%(  sample))

#raw_input("press enter to exit")

