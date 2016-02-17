#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"

#include "TLorentzVector.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TTreeReaderValue.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TString.h"
#include "TChain.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>

void printQpT(std::string const& file1){

  TFile *g;
  g = new TFile("CosmicCurvature.root","RECREATE");
  
  //TH1F *h_lowerCurve = new TH1F("Lower_Muon_Curvature", "Lower_Muon_Curvature", 20, -1.0, 1.0);
  //TH1F *h_upperCurve = new TH1F("Upper_Muon_Curvature", "Upper_Muon_Curvature", 20, -1.0, 1.0);
  TH1F *h_muonCurve = new TH1F("Muon_Curvature", "Muon_Curvature", 40, -0.03, 0.03);
  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/MuonAnalyzer/test/"+file1));
  if(f == 0){
    std::cout << "Error: cannot open file! \n";
    return;
  }
  //  std::cout << std::hex << *f << std::dec << std::endl;
  std::cout << "successfully opened file!";

  TTreeReader muonReader("analysisUpperTagTunePMuons/MuonTree",f);


  TTreeReaderValue<Int_t> lowerCharge(muonReader, "lowerMuon_charge");
  TTreeReaderValue<Int_t> upperCharge(muonReader, "upperMuon_charge");
  TTreeReaderValue<Double_t> lowerPt(muonReader, "lowerMuon_pT");
  TTreeReaderValue<Double_t> upperPt(muonReader, "upperMuon_pT");

  bool muonChk = muonReader.Next();
  double lowerCurve = 0.0;
  double upperCurve = 0.0;
  double curve = 0.0;
  
  std::cout << std::endl << muonChk << std::endl;

  std::cout << "\n\tQ\tpT\tkappa\t\n";
  
  while(muonChk){
    g->cd();
    //    std::cout << "\t" << *lowerCharge << "\t" << *lowerPt << std::endl;
    //lowerCurve = *lowerCharge / (*lowerPt);
    //std::cout << "\nLower Curve is: " << lowerCurve << std::endl;
    //h_lowerCurve->Fill(lowerCurve);

    upperCurve = *upperCharge / (*upperPt);
    std::cout << "\t" << *upperCharge << "\t" << *upperPt << "\t\t" << upperCurve << std::endl;
    //std::cout << "\nUpper Curve is: " << upperCurve << std::endl;
    // h_upperCurve->Fill(upperCurve);
    h_muonCurve->Fill(upperCurve);
    
    muonChk = muonReader.Next();

  }

  g->Write();
  g->Close();


  return;
}
