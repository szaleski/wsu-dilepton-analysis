#!/bin/env python
import ROOT as r
import numpy as np

dataFile = r.TFile("Data2MC.root","r")
mcFile   = r.TFile("Data2MC.root","r")

dhist  = dataFile.Get("dataHist;15")
#dhist.Add(dataFile.Get(plushistname))
mcHist = mcFile.Get("TunePMCCurvaturePlusBias785")
#mchist.Add(mcFile.Get(plushistname))
print mcHist
mcHist.Scale(dhist.Integral()/mcHist.Integral())
print mcHist
dhist = dhist.Rebin(4)
mcHist = mcHist.Rebin(4)
resids = np.zeros(mcHist.GetNbinsX(),np.dtype('float64')) # pointer argument, one per bin, not quite working
chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
igood    = r.Long(0)    # necessary for pass-by-reference in python
histopts = "UU,NORM" # unweighted/unweighted, normalized

prob = dhist.Chi2TestX(mcHist,chi2Val,chi2ndf,igood,histopts,resids)
print "prob: %2.4g  chi2:%2.4f  ndf:%d  igood:%d"%(prob,chi2Val,chi2ndf,igood)
resHist = r.TH1D("ResHist", "ResHist", len(resids), -7.5,7.5)
resHist.Sumw2();
for i,res in enumerate(resids):
    print "residual %d = %2.4f"%(i,res)
    resHist.SetBinContent(i+1, res)

can = r.TCanvas("can","can",1000,1000)
pad = r.TPad("pad","pad",0.0,0.3,1.0,1.0)
#can.Divide(2,1)
pad.Draw()
can.cd()
resPad = r.TPad("Respad","Respad",0.0,0.0,1.0,0.3)
resPad.Draw()
pad.cd()
dhist.Draw()
dhist.SetLineColor(r.kRed)
mcHist.Draw("ep0sames")
mcHist.SetLineColor(r.kBlue);
resPad.cd()
resHist.Draw()
can.SaveAs("~/public/html/cosmics/resids785.png")
#raw_input("Enter to quit")
