#!/bin/env python

import sys,os
import ROOT as r

r.gROOT.SetBatch(True)

startupFile = r.TFile("CosmicMuonTree_MC_76X_startup_HLT.root","r")
asymFile    = r.TFile("CosmicMuonTree_MC_76X_asymptotic_HLT.root","r")
craftFile     = r.TFile("CosmicMuonTree_76X_CRAFT15.root","r")
interfillFile = r.TFile("CosmicMuonTree_76X_Interfill.root","r")
fileNames = {
    "startup"   : startupFile,
    "asymptotic": asymFile,
    "craft15"   : craftFile,
    "interfill" : interfillFile
    }

triginfo_hist = {
    "startup"   : None,
    "asymptotic": None,
    "craft15"   : None,
    "interfill" : None
    }
eta_vs_phi_hist = {
    "startup"   : None,
    "asymptotic": None,
    "craft15"   : None,
    "interfill" : None
    }
eta_vs_qual_hist = {
    "startup"   : None,
    "asymptotic": None,
    "craft15"   : None,
    "interfill" : None
    }
eta_vs_singlemu_hist = {
    "startup"   : None,
    "asymptotic": None,
    }
phi_vs_singlemu_hist = {
    "startup"   : None,
    "asymptotic": None,
    }

cuts = {
    "_all":"1",
    "_noFwd":"l1MuonIsFwd==0",
    "_noRPC":"l1MuonIsRPC==0",
    "_isFwd":"l1MuonIsFwd==1",
    "_isRPC":"l1MuonIsRPC==1",
    }

trigCan  = r.TCanvas("trigCan","trigCan",800,800)
trigCan.Divide(2,2)
trigCounter = 0
for fileName in fileNames.keys():
    trigCounter = trigCounter + 1
    funky = open("%s_funky_runs.txt"%(fileName),"w")
    mytree = fileNames[fileName].Get("analysisSPMuons/MuonTree")
    triginfo_hist[fileName] = r.TH2D("triginfo_%s"%(fileName),"%s Trigger counters"%(fileName),
                                     5, -0.5, 4.5, 5, -0.5, 4.5)
    triginfo_hist[fileName].SetStats(0)
    triginfo_hist[fileName].GetXaxis().SetBinLabel(1,"all")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(2,"-2.88 < L1 #phi < -0.26")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(3,"HLT_L1SingleMuOpen")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(4,"both")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(5,"")
    triginfo_hist[fileName].GetXaxis().SetTitle("")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(1,"all")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(2,"noFwd")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(3,"isFwd")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(4,"noRPC")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(5,"isRPC")
    triginfo_hist[fileName].GetYaxis().SetTitle("")
    
    for ev in mytree:
        selections = []
        for l1mu in range(ev.nL1Muons):
            tmp1 = []
            for xbin in range(5):
                tmp2 = []
                for ybin in range(5):
                    tmp2.append(False)
                    pass
                tmp1.append(tmp2)
                pass
            #print "tmp[%d]"%(xbin),tmp
            selections.append(tmp1)
            pass
        #print "selections:",selections

        for l1m in range(ev.nL1Muons):
            if ev.l1MuonPhi[l1m]>2. and ev.l1MuonIsFwd[l1m]==0:
                funky.write("%d/%d/%d - %d\n"%(ev.run,ev.lumi,ev.event,ev.nL1Muons))

            # find out if there is a muon passing each selection, but this doesn't give quite the info i want...
            selections[l1m][0][0] = True
            if ev.l1MuonIsFwd[l1m]==0:
                selections[l1m][0][1] = True
                pass
            if ev.l1MuonIsFwd[l1m]==1:
                selections[l1m][0][2] = True
                pass
            if ev.l1MuonIsRPC[l1m]==0:
                selections[l1m][0][3] = True
                pass
            if ev.l1MuonIsRPC[l1m]==1:
                selections[l1m][0][4] = True
                pass

            if ev.l1MuonPhi[l1m] > -2.88 and ev.l1MuonPhi[l1m] < -0.26:
                selections[l1m][1][0] = True
                if ev.l1MuonIsFwd[l1m]==0:
                    selections[l1m][1][1] = True
                    pass
                if ev.l1MuonIsFwd[l1m]==1:
                    selections[l1m][1][2] = True
                    pass
                if ev.l1MuonIsRPC[l1m]==0:
                    selections[l1m][1][3] = True
                    pass
                if ev.l1MuonIsRPC[l1m]==1:
                    selections[l1m][1][4] = True
                    pass

                if ev.l1SingleMu:
                    selections[l1m][3][0] = True
                    if ev.l1MuonIsFwd[l1m]==0:
                        selections[l1m][3][1] = True
                        pass
                    if ev.l1MuonIsFwd[l1m]==1:
                        selections[l1m][3][2] = True
                        pass
                    if ev.l1MuonIsRPC[l1m]==0:
                        selections[l1m][3][3] = True
                        pass
                    if ev.l1MuonIsRPC[l1m]==1:
                        selections[l1m][3][4] = True
                        pass
                    pass
                pass

            if ev.l1SingleMu:
                selections[l1m][2][0] = True
                if ev.l1MuonIsFwd[l1m]==0:
                    selections[l1m][2][1] = True
                    pass
                if ev.l1MuonIsFwd[l1m]==1:
                    selections[l1m][2][2] = True
                    pass
                if ev.l1MuonIsRPC[l1m]==0:
                    selections[l1m][2][3] = True
                    pass
                if ev.l1MuonIsRPC[l1m]==1:
                    selections[l1m][2][4] = True
                    pass
                pass
            pass

        for xbin in range(5):
            for ybin in range(5):
                result = 0;
                for l1mu in range(ev.nL1Muons):
                    result = result + selections[l1mu][xbin][ybin]
                    pass
                if result > 0:
                    triginfo_hist[fileName].Fill(xbin,ybin)
                    pass
                pass
            pass
        pass
    
    trigCan.cd(trigCounter)
    triginfo_hist[fileName].Draw("colztext")
    trigCan.Update()
    funky.close()
    pass
trigCan.SaveAs("~/public/html/Cosmics/L1Info/triginfo.png")
trigCan.SaveAs("~/public/html/Cosmics/L1Info/triginfo.pdf")
trigCan.SaveAs("~/public/html/Cosmics/L1Info/triginfo.C")
    

for cut in cuts.keys():
    compCan  = r.TCanvas("compCan","compCan",800,800)
    compCan.Divide(2,2)

    qualCan  = r.TCanvas("qualCan","qualCan",800,800)
    qualCan.Divide(2,2)

    startAsymCan  = r.TCanvas("startAsymCan","startAsymCan",800,800)
    startAsymCan.Divide(2,2)
    
    canCounter = 0
    startAsymCounter = 0
    
    
    for fileName in fileNames.keys():
        canCounter = canCounter + 1
        mycan = r.TCanvas("myCan","myCan",800,800)
        mytree = fileNames[fileName].Get("analysisSPMuons/MuonTree")
        eta_vs_phi = mytree.Draw("l1MuonPhi:l1MuonEta","%s"%(cuts[cut]),"colz")
        mycan.Update()
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_eta_vs_phi%s.png"%(fileName,cut))
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_eta_vs_phi%s.pdf"%(fileName,cut))
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_eta_vs_phi%s.C"%(fileName,cut))
    
        eta_vs_phi_hist[fileName] = r.TH2D("eta_vs_phi_%s"%(fileName),"%s #eta vs. #phi"%(fileName),
                                           50, -2.5, 2.5, 40, -4., 4.)
        eta_vs_phi_hist[fileName].SetStats(0)
        eta_vs_phi_hist[fileName].GetXaxis().SetTitle("L1Muon #eta")
        eta_vs_phi_hist[fileName].GetYaxis().SetTitle("L1Muon #phi")
        compCan.cd(canCounter)
        eta_vs_phi = mytree.Draw("l1MuonPhi:l1MuonEta>>eta_vs_phi_%s"%(fileName),"%s"%(cuts[cut]),"colz")
        compCan.Update()
    
        eta_vs_qual_hist[fileName] = r.TH2D("eta_vs_qual_%s"%(fileName),"%s #eta vs. Quality"%(fileName),
                                           50, -2.5, 2.5, 10, -0.5, 9.5)
        eta_vs_qual_hist[fileName].SetStats(0)
        eta_vs_qual_hist[fileName].GetXaxis().SetTitle("L1Muon #eta")
        eta_vs_qual_hist[fileName].GetYaxis().SetTitle("L1Muon Quality")
        qualCan.cd(canCounter)
        eta_vs_qual = mytree.Draw("l1MuonQuality:l1MuonEta>>eta_vs_qual_%s"%(fileName),"l1MuonPhi>2&&%s"%(cuts[cut]),"colz")
        qualCan.Update()
    
        mycan.cd()
        mycan.Clear()
        eta_vs_l1open = mytree.Draw("l1MuonEta:l1SingleMu","%s"%(cuts[cut]),"colz")
        mycan.Update()
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_eta_vs_L1SingleMu%s.png"%(fileName,cut))
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_eta_vs_L1SingleMu%s.pdf"%(fileName,cut))
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_eta_vs_L1SingleMu%s.C"%(fileName,cut))
    
        if fileName in ["startup","asymptotic"]:
            startAsymCounter = startAsymCounter + 1
            eta_vs_singlemu_hist[fileName] = r.TH2D("eta_vs_singlemu_%s"%(fileName),"%s #eta vs. L1SingleMuOpen"%(fileName),
                                                    50, -2.5, 2.5, 2, -0.5, 1.5)
            eta_vs_singlemu_hist[fileName].SetStats(0)
            eta_vs_singlemu_hist[fileName].GetXaxis().SetTitle("L1Muon #eta")
            eta_vs_singlemu_hist[fileName].GetYaxis().SetTitle("L1MuonOpen")
            startAsymCan.cd(startAsymCounter)
            mytree.Draw("l1SingleMu:l1MuonEta>>eta_vs_singlemu_%s"%(fileName),"%s"%(cuts[cut]),"colz")
            startAsymCan.Update()
            pass
    
        mycan.Clear()
        phi_vs_l1open = mytree.Draw("l1MuonPhi:l1SingleMu","%s"%(cuts[cut]),"colz")
        mycan.Update()
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_phi_vs_L1SingleMu%s.png"%(fileName,cut))
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_phi_vs_L1SingleMu%s.pdf"%(fileName,cut))
        mycan.SaveAs("~/public/html/Cosmics/L1Info/%s_l1_phi_vs_L1SingleMu%s.C"%(fileName,cut))
    
        if fileName in ["startup","asymptotic"]:
            startAsymCounter = startAsymCounter + 1
            phi_vs_singlemu_hist[fileName] = r.TH2D("phi_vs_singlemu_%s"%(fileName),"%s #phi vs. L1SingleMuOpen"%(fileName),
                                                    40, -4., 4., 2, -0.5, 1.5)
            phi_vs_singlemu_hist[fileName].SetStats(0)
            phi_vs_singlemu_hist[fileName].GetXaxis().SetTitle("L1Muon #phi")
            phi_vs_singlemu_hist[fileName].GetYaxis().SetTitle("L1MuonOpen")
            startAsymCan.cd(startAsymCounter)
            mytree.Draw("l1SingleMu:l1MuonPhi>>phi_vs_singlemu_%s"%(fileName),"%s"%(cuts[cut]),"colz")
            startAsymCan.Update()
            pass
        pass
    compCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_eta_vs_phi%s.png"%(cut))
    compCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_eta_vs_phi%s.pdf"%(cut))
    compCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_eta_vs_phi%s.C"%(cut))
    
    qualCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_eta_vs_quality%s.png"%(cut))
    qualCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_eta_vs_quality%s.pdf"%(cut))
    qualCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_eta_vs_quality%s.C"%(cut))
    
    startAsymCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_singlemu%s.png"%(cut))
    startAsymCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_singlemu%s.pdf"%(cut))
    startAsymCan.SaveAs("~/public/html/Cosmics/L1Info/comparison_l1_singlemu%s.C"%(cut))
