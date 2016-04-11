#!/bin/env python

class getMCScaleFactors():
    def __init__(self, infile, outfile, minpT, maxpT, tchain=False,debug=False) :
        import ROOT as r
        print "getMCScaleFactors"
        self.infile  = infile
        #self.outfile = r.TFile("%s.root"%(outfile),"update")
        self.outfile = outfile
        self.outdir  = self.outfile.mkdir("genMCScaleFactors")
        self.minpT   = minpT
        self.maxpT   = maxpT
        self.tchain  = tchain
        self.debug   = debug

        print "getMCScaleFactors:creating histograms"
        self.outfile.cd()
        self.outdir.cd()
        self.ptHisto  = r.TH1D("ptHisto" ,"sim track p_{T}", 300, 0. ,3000.)
        self.etaHisto = r.TH1D("etaHisto","sim track #eta" , 60 ,-3. ,3.   )
        self.phiHisto = r.TH1D("phiHisto","sim track #phi" , 40 ,-3.2,3.2  )
        self.ptHistoCut  = r.TH1D("ptHistoCut" ,"sim track p_{T}", 300, 0. ,3000.)
        self.etaHistoCut = r.TH1D("etaHistoCut","sim track #eta" , 60 ,-3. ,3.   )
        self.phiHistoCut = r.TH1D("phiHistoCut","sim track #phi" , 40 ,-3.2,3.2  )
        
        self.ptHisto .Sumw2()
        self.etaHisto.Sumw2()
        self.phiHisto.Sumw2()
        self.ptHistoCut .Sumw2()
        self.etaHistoCut.Sumw2()
        self.phiHistoCut.Sumw2()
        #self.outfile.Write()
        print "getMCScaleFactors:done with __init__"
        return

    def processTree(self,tree,minpT,maxpT):
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
            for sim in range(ev.nSimTracks):
                self.ptHisto.Fill(ev.simTrackpT[sim])
                self.etaHisto.Fill(ev.simTrackEta[sim])
                self.phiHisto.Fill(ev.simTrackPhi[sim])
                if ev.simTrackpT[sim] < minpT:
                    continue
                if ev.simTrackpT[sim] > maxpT:
                    continue
                self.ptHistoCut.Fill(ev.simTrackpT[sim])
                self.etaHistoCut.Fill(ev.simTrackEta[sim])
                self.phiHistoCut.Fill(ev.simTrackPhi[sim])
                pass
            sys.stdout.flush()
            eid = eid + 1
            pass
        return
        
    def writeOut(self):
        self.outfile.cd()
        self.outdir.cd()
        print """integral of distribution: %d
  0 < pT <  10: %d
 10 < pT < 100: %d
100 < pT < 500: %d
500 < pT:       %d
"""%(self.ptHisto.Integral(),
     self.ptHisto.Integral(0                          ,self.ptHisto.FindBin(10)),
     self.ptHisto.Integral(self.ptHisto.FindBin(10)+1 ,self.ptHisto.FindBin(100)),
     self.ptHisto.Integral(self.ptHisto.FindBin(100)+1,self.ptHisto.FindBin(500)),
     self.ptHisto.Integral(self.ptHisto.FindBin(500)+1,-1)
     )
        self.ptHisto .Write()
        self.etaHisto.Write()
        self.phiHisto.Write()
        self.ptHistoCut .Write()
        self.etaHistoCut.Write()
        self.phiHistoCut.Write()

        self.outdir.Write()
        self.outfile.Write()
        #self.outfile.Close()
        return
