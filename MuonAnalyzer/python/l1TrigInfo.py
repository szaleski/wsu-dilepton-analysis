#!/bin/env python

class l1TrigInfo():
    def __init__(self, infile, outfile, isMC=False, tchain=False,debug=False) :
        import ROOT as r
        print "l1TrigInfo"
        self.infile  = infile
        #self.outfile = r.TFile("%s.root"%(outfile),"update")
        self.outfile = outfile
        self.outdir  = self.outfile.mkdir("l1TrigInfo")
        self.isMC    = isMC
        self.tchain  = tchain
        self.debug   = debug

        print "l1TrigInfo:creating histograms"
        self.outfile.cd()
        self.outdir.cd()
        self.trig_info_hist = r.TH2D("trig_info","Trigger counters",
                                     6, -0.5, 5.5, 5, -0.5, 4.5)
        self.trig_info_hist.SetStats(r.kFALSE)
        self.trig_info_hist.GetXaxis().SetBinLabel(1,"all")
        self.trig_info_hist.GetXaxis().SetBinLabel(2,"-2.88 < L1 #phi < -0.26")
        self.trig_info_hist.GetXaxis().SetBinLabel(3,"HLT_L1SingleMuOpen")
        self.trig_info_hist.GetXaxis().SetBinLabel(4,"both")
        self.trig_info_hist.GetXaxis().SetBinLabel(5,"L1 #phi > -0.26")
        self.trig_info_hist.GetXaxis().SetBinLabel(6,"both2")
        self.trig_info_hist.GetXaxis().SetTitle("")
        self.trig_info_hist.GetYaxis().SetBinLabel(1,"all")
        self.trig_info_hist.GetYaxis().SetBinLabel(2,"noFwd")
        self.trig_info_hist.GetYaxis().SetBinLabel(3,"isFwd")
        self.trig_info_hist.GetYaxis().SetBinLabel(4,"noRPC")
        self.trig_info_hist.GetYaxis().SetBinLabel(5,"isRPC")
        self.trig_info_hist.GetYaxis().SetTitle("")
        self.trig_info_hist.Sumw2()

        self.trig_eff_hist = r.TH2D("trig_eff","Lower leg high-p_{T} ID to trigger efficiency",
                                    4, -0.5, 3.5, 2, -0.5, 1.5)
        self.trig_eff_hist.SetStats(r.kFALSE)
        self.trig_eff_hist.GetYaxis().SetBinLabel(1,"all")
        self.trig_eff_hist.GetYaxis().SetBinLabel(2,"-2.88 < L1 #phi < -0.26")
        self.trig_eff_hist.GetYaxis().SetTitle("")
        self.trig_eff_hist.GetXaxis().SetBinLabel(1,"High-p_{T} ID")
        self.trig_eff_hist.GetXaxis().SetBinLabel(2,"High-p_{T} ID&&firstPixel")
        self.trig_eff_hist.GetXaxis().SetBinLabel(3,"High-p_{T} ID&&D_{xy}<4&&D_{z}<10")
        self.trig_eff_hist.GetXaxis().SetBinLabel(4,"High-p_{T} ID&&D_{xy}<4&&D_{z}<10&&firstPixel")
        self.trig_eff_hist.GetXaxis().SetTitle("")
        self.trig_eff_hist.Sumw2()

        self.eta_vs_phi_hist = r.TH2D("eta_vs_phi","#eta vs. #phi",
                                      50, -2.5, 2.5, 40, -4., 4.)
        self.eta_vs_phi_hist.SetStats(r.kFALSE)
        self.eta_vs_phi_hist.GetXaxis().SetTitle("L1Muon #eta")
        self.eta_vs_phi_hist.GetYaxis().SetTitle("L1Muon #phi")
        self.eta_vs_phi_hist.Sumw2()

        self.eta_vs_qual_hist = r.TH2D("eta_vs_qual","#eta vs. Quality",
                                       50, -2.5, 2.5, 10, -0.5, 9.5)
        self.eta_vs_qual_hist.SetStats(r.kFALSE)
        self.eta_vs_qual_hist.GetXaxis().SetTitle("L1Muon #eta")
        self.eta_vs_qual_hist.GetYaxis().SetTitle("L1Muon Quality")
        self.eta_vs_qual_hist.Sumw2()

        self.eta_vs_singlemu_hist = r.TH2D("eta_vs_singlemu","#eta vs. L1SingleMuOpen",
                                           50, -2.5, 2.5, 2, -0.5, 1.5)
        self.eta_vs_singlemu_hist.SetStats(r.kFALSE)
        self.eta_vs_singlemu_hist.GetXaxis().SetTitle("L1Muon #eta")
        self.eta_vs_singlemu_hist.GetYaxis().SetTitle("L1MuonOpen")
        self.eta_vs_singlemu_hist.Sumw2()

        self.phi_vs_singlemu_hist = r.TH2D("phi_vs_singlemu","#phi vs. L1SingleMuOpen",
                                           40, -4., 4., 2, -0.5, 1.5)
        self.phi_vs_singlemu_hist.SetStats(r.kFALSE)
        self.phi_vs_singlemu_hist.GetXaxis().SetTitle("L1Muon #phi")
        self.phi_vs_singlemu_hist.GetYaxis().SetTitle("L1MuonOpen")
        self.phi_vs_singlemu_hist.Sumw2()
        
        self.pt_hist   = {"den":[],"num":[]}
        self.eta_hist  = {"den":[],"num":[]}
        self.phi_hist  = {"den":[],"num":[]}
        self.time_hist = {"den":[],"num":[]}
        
        cuts = [
            "high p_{T} ID",
            "high p_{T} ID+firstPixel",
            "high p_{T} ID+D_{xy}<4+D_{z}<10",
            "high p_{T} ID+both",
            ]
        
        for cut in range(4):
            self.pt_hist["den"].append(r.TH1D("pt_den%d_hist"%(cut), cuts[cut], 300, 0., 3000.))
            self.pt_hist["num"].append(r.TH1D("pt_num%d_hist"%(cut), cuts[cut], 300, 0., 3000.))
            self.pt_hist["den"][cut].Sumw2()
            self.pt_hist["num"][cut].Sumw2()
            
            self.eta_hist["den"].append(r.TH1D("eta_den%d_hist"%(cut), cuts[cut], 100, -1., 1.))
            self.eta_hist["num"].append(r.TH1D("eta_num%d_hist"%(cut), cuts[cut], 100, -1., 1.))
            self.eta_hist["den"][cut].Sumw2()
            self.eta_hist["num"][cut].Sumw2()
            
            self.phi_hist["den"].append(r.TH1D("phi_den%d_hist"%(cut), cuts[cut], 50, -3.2, 0.))
            self.phi_hist["num"].append(r.TH1D("phi_num%d_hist"%(cut), cuts[cut], 50, -3.2, 0.))
            self.phi_hist["den"][cut].Sumw2()
            self.phi_hist["num"][cut].Sumw2()
            
            self.time_hist["den"].append(r.TH1D("time_den%d_hist"%(cut), cuts[cut], 4000, -100., 100.))
            self.time_hist["num"].append(r.TH1D("time_num%d_hist"%(cut), cuts[cut], 4000, -100., 100.))
            self.time_hist["den"][cut].Sumw2()
            self.time_hist["num"][cut].Sumw2()
            pass

        #self.outfile.Write()
        print "l1TrigInfo:done with __init__"
        return

    def processTree(self,tree):
        from wsuMuonTreeUtils import passHighPtMuon, matchSimTrack, matchL1SingleMu, passDxyDz

        import sys,os
        nEvents = tree.GetEntries()
        eid = 0
        for ev in tree:
            if self.debug and eid > 1000:
                print "debugging, not processing events more than 1000"
                break
                
            if self.debug and eid%10 == 0:
                print "event=%d/%d: simTracks=%d"%(eid,nEvents,ev.nSimTracks)
                pass
            elif eid%1000 == 0:
                print "event=%d/%d: simTracks=%d"%(eid,nEvents,ev.nSimTracks)
                pass

            sys.stdout.flush()
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
                selections.append(tmp1)
                pass
            
            for l1mu in range(ev.nL1Muons):
                if ev.l1MuonPhi[l1mu]>2. and ev.l1MuonIsFwd[l1mu]==0:
                    # funky.write("%d/%d/%d - %d\n"%(ev.run,ev.lumi,ev.event,ev.nL1Muons))
                    pass
                
                self.eta_vs_phi_hist.Fill( ev.l1MuonEta[l1mu],ev.l1MuonPhi[l1mu])
                self.eta_vs_qual_hist.Fill(ev.l1MuonEta[l1mu],ev.l1MuonQuality[l1mu])
                self.eta_vs_singlemu_hist.Fill(ev.l1MuonEta[l1mu],ev.l1SingleMu)
                self.phi_vs_singlemu_hist.Fill(ev.l1MuonPhi[l1mu],ev.l1SingleMu)
                
                selections[l1mu][0][0] = True
                if ev.l1MuonIsFwd[l1mu]==0:
                    selections[l1mu][0][1] = True
                    pass
                if ev.l1MuonIsFwd[l1mu]==1:
                    selections[l1mu][0][2] = True
                    pass
                if ev.l1MuonIsRPC[l1mu]==0:
                    selections[l1mu][0][3] = True
                    pass
                if ev.l1MuonIsRPC[l1mu]==1:
                    selections[l1mu][0][4] = True
                    pass
                
                if ev.l1MuonPhi[l1mu] > -2.88 and ev.l1MuonPhi[l1mu] < -0.26:
                    selections[l1mu][1][0] = True
                    if ev.l1MuonIsFwd[l1mu]==0:
                        selections[l1mu][1][1] = True
                        pass
                    if ev.l1MuonIsFwd[l1mu]==1:
                        selections[l1mu][1][2] = True
                        pass
                    if ev.l1MuonIsRPC[l1mu]==0:
                        selections[l1mu][1][3] = True
                        pass
                    if ev.l1MuonIsRPC[l1mu]==1:
                        selections[l1mu][1][4] = True
                        pass
                    
                    if ev.l1SingleMu:
                        selections[l1mu][3][0] = True
                        if ev.l1MuonIsFwd[l1mu]==0:
                            selections[l1mu][3][1] = True
                            pass
                        if ev.l1MuonIsFwd[l1mu]==1:
                            selections[l1mu][3][2] = True
                            pass
                        if ev.l1MuonIsRPC[l1mu]==0:
                            selections[l1mu][3][3] = True
                            pass
                        if ev.l1MuonIsRPC[l1mu]==1:
                            selections[l1mu][3][4] = True
                            pass
                        pass
                    pass
                elif ev.l1MuonPhi[l1mu] > -0.26:
                    selections[l1mu][4][0] = True
                    if ev.l1MuonIsFwd[l1mu]==0:
                        selections[l1mu][4][1] = True
                        pass
                    if ev.l1MuonIsFwd[l1mu]==1:
                        selections[l1mu][4][2] = True
                        pass
                    if ev.l1MuonIsRPC[l1mu]==0:
                        selections[l1mu][4][3] = True
                        pass
                    if ev.l1MuonIsRPC[l1mu]==1:
                        selections[l1mu][4][4] = True
                        pass
                    
                    if ev.l1SingleMu:
                        selections[l1mu][5][0] = True
                        if ev.l1MuonIsFwd[l1mu]==0:
                            selections[l1mu][5][1] = True
                            pass
                        if ev.l1MuonIsFwd[l1mu]==1:
                            selections[l1mu][5][2] = True
                            pass
                        if ev.l1MuonIsRPC[l1mu]==0:
                            selections[l1mu][5][3] = True
                            pass
                        if ev.l1MuonIsRPC[l1mu]==1:
                            selections[l1mu][5][4] = True
                            pass
                        pass
                    pass
                
                if ev.l1SingleMu:
                    selections[l1mu][2][0] = True
                    if ev.l1MuonIsFwd[l1mu]==0:
                        selections[l1mu][2][1] = True
                        pass
                    if ev.l1MuonIsFwd[l1mu]==1:
                        selections[l1mu][2][2] = True
                        pass
                    if ev.l1MuonIsRPC[l1mu]==0:
                        selections[l1mu][2][3] = True
                        pass
                    if ev.l1MuonIsRPC[l1mu]==1:
                        selections[l1mu][2][4] = True
                        pass
                    pass
                pass
            
            for xbin in range(6):
                for ybin in range(5):
                    result = 0;
                    for l1mu in range(ev.nL1Muons):
                        result = result + selections[l1mu][xbin][ybin]
                        pass
                    # if there is any l1muon in the event passing the given selection, we fill the histo
                    if result > 0:
                        self.trig_info_hist.Fill(xbin,ybin)
                        pass
                    pass
                pass
            
            matchCount = 0
            for mu in range(ev.nMuons):
                if ev.isUpper[mu]:
                    continue
                if abs(ev.trackEta[mu]) > 0.9:
                    continue
                if not passHighPtMuon(ev,mu):
                    continue
                if self.isMC and not matchSimTrack(ev,mu,0.9,0.9,self.debug) > -1:
                    continue
                
                self.trig_eff_hist.Fill(0,0)
                self.pt_hist["den"][0].Fill(ev.trackpT[mu])
                self.eta_hist["den"][0].Fill(ev.trackEta[mu])
                self.phi_hist["den"][0].Fill(ev.trackPhi[mu])
                self.time_hist["den"][0].Fill(ev.tpin[mu])
                
                matchCount = matchCount + 1
                
                if matchL1SingleMu(ev,mu,0.9,0.9,self.debug) > -1:
                    self.trig_eff_hist.Fill(0,1)
                    self.pt_hist["num"][0].Fill(ev.trackpT[mu])
                    self.eta_hist["num"][0].Fill(ev.trackEta[mu])
                    self.phi_hist["num"][0].Fill(ev.trackPhi[mu])
                    self.time_hist["num"][0].Fill(ev.tpin[mu])
                    pass
                
                # second selection with firstPixelLayer
                if passHighPtMuon(ev,mu,True):
                    self.trig_eff_hist.Fill(1,0)
                    self.pt_hist["den"][1].Fill(ev.trackpT[mu])
                    self.eta_hist["den"][1].Fill(ev.trackEta[mu])
                    self.phi_hist["den"][1].Fill(ev.trackPhi[mu])
                    self.time_hist["den"][1].Fill(ev.tpin[mu])
                    
                    if matchL1SingleMu(ev,mu,0.9,0.9,self.debug) > -1:
                        self.trig_eff_hist.Fill(1,1)
                        self.pt_hist["num"][1].Fill(ev.trackpT[mu])
                        self.eta_hist["num"][1].Fill(ev.trackEta[mu])
                        self.phi_hist["num"][1].Fill(ev.trackPhi[mu])
                        self.time_hist["num"][1].Fill(ev.tpin[mu])
                        pass
                    pass
                
                # third selection with dxy < 4, dz < 10
                if passHighPtMuon(ev,mu) and passDxyDz(ev,mu,4.,10.):
                    self.trig_eff_hist.Fill(2,0)
                    self.pt_hist["den"][2].Fill(ev.trackpT[mu])
                    self.eta_hist["den"][2].Fill(ev.trackEta[mu])
                    self.phi_hist["den"][2].Fill(ev.trackPhi[mu])
                    self.time_hist["den"][2].Fill(ev.tpin[mu])
                    
                    if matchL1SingleMu(ev,mu,0.9,0.9,self.debug) > -1:
                        self.trig_eff_hist.Fill(2,1)
                        self.pt_hist["num"][2].Fill(ev.trackpT[mu])
                        self.eta_hist["num"][2].Fill(ev.trackEta[mu])
                        self.phi_hist["num"][2].Fill(ev.trackPhi[mu])
                        self.time_hist["num"][2].Fill(ev.tpin[mu])
                        pass
                    pass
                
                # fourth selection with dxy < 4, dz < 10 and firstPixelLayer
                if passHighPtMuon(ev,mu,True) and passDxyDz(ev,mu,4.,10.):
                    self.trig_eff_hist.Fill(3,0)
                    self.pt_hist["den"][3].Fill(ev.trackpT[mu])
                    self.eta_hist["den"][3].Fill(ev.trackEta[mu])
                    self.phi_hist["den"][3].Fill(ev.trackPhi[mu])
                    self.time_hist["den"][3].Fill(ev.tpin[mu])
                    
                    if matchL1SingleMu(ev,mu,0.9,0.9,self.debug) > -1:
                        self.trig_eff_hist.Fill(3,1)
                        self.pt_hist["num"][3].Fill(ev.trackpT[mu])
                        self.eta_hist["num"][3].Fill(ev.trackEta[mu])
                        self.phi_hist["num"][3].Fill(ev.trackPhi[mu])
                        self.time_hist["num"][3].Fill(ev.tpin[mu])
                        pass
                    pass
                pass
            if matchCount > 1:
                print "found %d high-pT muons"%(matchCount)
                pass
            
            eid = eid + 1
            pass
        return
    
    def writeOut(self):
        self.outfile.cd()
        self.outdir.cd()

        self.trig_info_hist.Write()
        self.trig_eff_hist.Write()
        
        self.eta_vs_phi_hist.Write()
        self.eta_vs_qual_hist.Write()
        self.eta_vs_singlemu_hist.Write()
        self.phi_vs_singlemu_hist.Write()
        
        for cut in range(4):
            self.pt_hist["den"][cut].Write()
            self.pt_hist["num"][cut].Write()
            
            self.eta_hist["den"][cut].Write()
            self.eta_hist["num"][cut].Write()
            
            self.phi_hist["den"][cut].Write()
            self.phi_hist["num"][cut].Write()
            
            self.time_hist["den"][cut].Write()
            self.time_hist["num"][cut].Write()
            pass
        
        self.outdir.Write()
        self.outfile.Write()
        #self.outfile.Close()
        return
