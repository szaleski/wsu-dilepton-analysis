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
trig_eff_hist = {
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

def dPhi(phi1,phi2,debug=False):
    import math
    dphi = phi1-phi2
    if dphi < -math.pi:
        dphi = dphi + 2*math.pi
    if dphi > math.pi:
        dphi = dphi - 2*math.pi
    return dphi

def passHighPtMuon(event, muIdx, first=False, debug=False):
    if not ev.isTracker[muIdx]:
        return False
    if not ev.isGlobal[muIdx]:
        return False
    if not ev.nValidMuonHits[muIdx] > 0:
        return False
    if not ev.nMatchedStations[muIdx] > 1:
        return False
    if not (ev.ptError[muIdx]/ev.trackpT[muIdx]) < 0.3:
        return False
    if first and not ev.firstPixel[muIdx] > 0:
        return False
    if not ev.pixelHits[muIdx] > 0:
        return False
    if not ev.tkLayersWMeas[muIdx] > 5:
        return False
    
    return True

def matchSimTrack(event, muIdx, minDEta=0.05, minDPhi=0.15, debug=False):
    # sim tracks have phi positive and negative, while the reco::Muons and reco::Tracks seem always negative
    # only consider lower legs for now?
    bestDEta = 10
    bestDPhi = 10
    matchIdx = -1
    for sim in range(ev.nSimTracks):
        if ev.simTrackPhi[sim] > 0:
            continue
        tmpDEta = abs(ev.simTrackEta[sim] - ev.trackEta[muIdx])
        tmpDPhi = abs(dPhi(ev.simTrackPhi[sim], ev.trackPhi[muIdx]))
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi
            if bestDEta < minDEta and bestDPhi < minDPhi:
                matchIdx = sim
                pass
            pass
        pass
    if debug:
        print "muon %d matched to sim track %d with dEta=%2.2f and dPhi=%2.2f"%(muIdx,matchIdx,bestDEta,bestDPhi)
    return matchIdx

def matchL1SingleMu(event, muIdx, minDEta=0.05, minDPhi=0.15, debug=False):
    bestDEta = 10
    bestDPhi = 10
    matchIdx = -1
    for l1m in range(event.nL1Muons):
        if not (ev.l1MuonIsFwd[l1m]==0):
            continue
        if ev.l1MuonPhi[l1m] < -2.88:
            continue
        if ev.l1MuonPhi[l1m] > -0.26:
            continue
        
        tmpDEta = abs(ev.l1MuonEta[l1m] - ev.trackEta[muIdx])
        tmpDPhi = abs(dPhi(ev.l1MuonPhi[l1m], ev.trackPhi[muIdx]))
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi
            if bestDEta < minDEta and bestDPhi < minDPhi:
                matchIdx = l1m
                pass
            pass
        pass
    if debug:
        print "muon %d matched to l1Muon %d with dEta=%2.2f and dPhi=%2.2f"%(muIdx,matchIdx,bestDEta,bestDPhi)
    return matchIdx

def passL1SingleMu(event, debug=False):
    isL1SingleMu = False
    for l1m in range(event.nL1Muons):
        if not (ev.l1MuonIsFwd[l1m]==0):
            continue
        if ev.l1MuonPhi[l1m] > -2.88 and ev.l1MuonPhi[l1m] < -0.26:
            isL1SingleMu = True
            pass
        pass
    return isL1SingleMu

trigCan  = r.TCanvas("trigCan","trigCan",800,800)
trigCan.Divide(2,2)
trigCounter = 0
r.gStyle.SetOptStat(11111111)
for fileName in fileNames.keys():
    print "%s"%(fileName)
    trigCounter = trigCounter + 1
    funky = open("%s_funky_runs.txt"%(fileName),"w")
    mytree = fileNames[fileName].Get("analysisSPMuons/MuonTree")
    triginfo_hist[fileName] = r.TH2D("triginfo_%s"%(fileName),"%s Trigger counters"%(fileName),
                                     6, -0.5, 5.5, 5, -0.5, 4.5)
    triginfo_hist[fileName].SetStats(0)
    triginfo_hist[fileName].GetXaxis().SetBinLabel(1,"all")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(2,"-2.88 < L1 #phi < -0.26")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(3,"HLT_L1SingleMuOpen")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(4,"both")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(5,"L1 #phi > -0.26")
    triginfo_hist[fileName].GetXaxis().SetBinLabel(6,"both2")
    triginfo_hist[fileName].GetXaxis().SetTitle("")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(1,"all")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(2,"noFwd")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(3,"isFwd")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(4,"noRPC")
    triginfo_hist[fileName].GetYaxis().SetBinLabel(5,"isRPC")
    triginfo_hist[fileName].GetYaxis().SetTitle("")

    trig_eff_hist[fileName] = r.TH2D("trig_eff_%s"%(fileName),"%s Lower leg high-p_{T} ID to trigger efficiency"%(fileName),
                                     2, -0.5, 1.5, 2, -0.5, 1.5)
    trig_eff_hist[fileName].SetStats(r.kFALSE)
    trig_eff_hist[fileName].GetYaxis().SetBinLabel(1,"all")
    trig_eff_hist[fileName].GetYaxis().SetBinLabel(2,"-2.88 < L1 #phi < -0.26")
    trig_eff_hist[fileName].GetYaxis().SetTitle("")
    trig_eff_hist[fileName].GetXaxis().SetBinLabel(1,"High-p_{T} ID")
    trig_eff_hist[fileName].GetXaxis().SetBinLabel(2,"High-p_{T} ID+firstPixel")
    trig_eff_hist[fileName].GetXaxis().SetTitle("")
    
    for ev in mytree:
        selections = []
        for l1mu in range(ev.nL1Muons):
            tmp1 = []
            for xbin in range(6):
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
                pass
            #if not ev.l1MuonQuality[l1m] > 4:
            #    continue
            if not ev.l1MuonpT[l1m] > 10:
                continue
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
            elif ev.l1MuonPhi[l1m] > -0.26:
                selections[l1m][4][0] = True
                if ev.l1MuonIsFwd[l1m]==0:
                    selections[l1m][4][1] = True
                    pass
                if ev.l1MuonIsFwd[l1m]==1:
                    selections[l1m][4][2] = True
                    pass
                if ev.l1MuonIsRPC[l1m]==0:
                    selections[l1m][4][3] = True
                    pass
                if ev.l1MuonIsRPC[l1m]==1:
                    selections[l1m][4][4] = True
                    pass

                if ev.l1SingleMu:
                    selections[l1m][5][0] = True
                    if ev.l1MuonIsFwd[l1m]==0:
                        selections[l1m][5][1] = True
                        pass
                    if ev.l1MuonIsFwd[l1m]==1:
                        selections[l1m][5][2] = True
                        pass
                    if ev.l1MuonIsRPC[l1m]==0:
                        selections[l1m][5][3] = True
                        pass
                    if ev.l1MuonIsRPC[l1m]==1:
                        selections[l1m][5][4] = True
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

        for xbin in range(6):
            #print "xbin=%d"%(xbin)
            for ybin in range(5):
                #print "ybin=%d,nL1Muons=%d"%(ybin,ev.nL1Muons)
                result = 0;
                for l1mu in range(ev.nL1Muons):
                    #print "l1mu=%d, result=%d"%(l1mu,result)
                    result = result + selections[l1mu][xbin][ybin]
                    #print "result(%d): and selection[%d][%d][%d](%d)"%(result,l1mu,xbin,ybin,selections[l1mu][xbin][ybin])
                    pass
                # if there is any l1muon in the event passing the given selection, we fill the histo
                if result > 0:
                    triginfo_hist[fileName].Fill(xbin,ybin)
                    pass
                pass
            pass
        matchCount = 0
        for mu in range(ev.nMuons):
            if ev.isUpper[mu]:
                continue
            if abs(ev.trackEta[mu]) > 0.9:
                continue
            #if abs(ev.trackpT[mu]) < 53.:
            #    continue
            if not passHighPtMuon(ev,mu):
                continue
            if fileName in ["startup","asymptotic"] and not matchSimTrack(ev,mu,0.9,0.9,True) > -1:
                continue
            trig_eff_hist[fileName].Fill(0,0)
            matchCount = matchCount + 1
            if matchL1SingleMu(ev,mu,0.9,0.9,True) > -1:
                trig_eff_hist[fileName].Fill(0,1)
            # second selection with firstPixelLayer
            if passHighPtMuon(ev,mu,True):
                trig_eff_hist[fileName].Fill(1,0)
                if matchL1SingleMu(ev,mu,0.9,0.9,True) > -1:
                    trig_eff_hist[fileName].Fill(1,1)
                    pass
                pass
            pass
        if matchCount > 1:
            print "%s: found %d high-pT muons"%(fileName,matchCount)
        pass
    
    trigCan.cd(trigCounter)
    #triginfo_hist[fileName].Draw("colztext")
    trig_eff_hist[fileName].Draw("colztext")
    trigCan.Update()
    funky.close()
    pass
trigCan.SaveAs("~/public/html/Cosmics/L1Info/trig_eff_match_deta90_dphi90.png")
trigCan.SaveAs("~/public/html/Cosmics/L1Info/trig_eff_match_deta90_dphi90.pdf")
trigCan.SaveAs("~/public/html/Cosmics/L1Info/trig_eff_match_deta90_dphi90.C")
    

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
