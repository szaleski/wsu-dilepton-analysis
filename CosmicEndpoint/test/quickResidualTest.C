{
  TString dfilename  = "~szaleski/public/DataCosmicHistOut_TuneP.root";
  TString mcfilename = "~szaleski/public/MCCosmicHistOut_TuneP.root";
    
  TString minushistname = "looseMuLowerMinusCurve";
  TString plushistname  = "looseMuLowerPlusCurve";

  TFile* dataFile = new TFile(dfilename, "r");
  TFile* mcFile   = new TFile(mcfilename,"r");
    
  std::cout << "dataFile = " << std::hex << dataFile << std::dec << std::endl;
  std::cout << "mcFile = " << std::hex << mcFile << std::dec << std::endl;
  TH1D* dhist = (TH1D*)dataFile->Get(minushistname);
  std::cout << "dhist = " << std::hex << dhist << std::dec << std::endl;
  dhist->Add((TH1D*)dataFile->Get(plushistname));
  TH1D* mchist = (TH1D*)mcFile->Get(minushistname);
  std::cout << "mchist = " << std::hex << mchist << std::dec << std::endl;
  mchist->Add((TH1D*)mcFile->Get(plushistname));

  mchist->Scale(dhist->Integral()/mchist->Integral());
    
  Double_t *res = new Double_t[dhist->GetNbinsX()];
  Double_t chi2Val = 0.;
  Int_t    chi2ndf = 0;
  Int_t    igood   = 0;
  TString histopts = "UU,NORM";
    
  Double_t prob = dhist->Chi2TestX(mchist,chi2Val,chi2ndf,igood,histopts,res);
    
  std::cout << "res = " << std::hex << res << std::dec << std::endl;

  for (int i = 0; i < dhist->GetNbinsX(); ++i) {
    std::cout << "pres[" << i << "] = " << *(res+i) << std::endl;
    std::cout << "ares[" << i << "] = " << res[i]   << std::endl;
  }

  res = 0;
  delete res;
}
