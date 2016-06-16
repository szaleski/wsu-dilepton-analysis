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

void plotIt(TH1F *hist1, double setMin, std::string const& xAxis, std::string const& yAxis, std::string const& plotName);
void plotIt2(TH1F *hist1, TH1F *hist2, std::string const& xAxis, std::string const& yAxis, std::string const& plotName);


void printQpT(std::string const& file1){

  TFile *g;
  g = new TFile("CosmicCurvature.root","RECREATE");
  
  //  TH1::SetDefaultSumw2();
  TH1F *h_muMinusCurve = new TH1F("MuMinus_Curvature", "MuMinus_Curvature", 20, -0.03, 0.0);
  TH1F *h_muMinusScaledCurve = new TH1F("Scaled_MuMinus_Curvature", "Scaled_MuMinus_Curvature", 20, -0.03, 0.0);
  TH1F *h_muMinusSumw2ScaledCurve = new TH1F("SumScaled_MuMinus_Curvature", "SumScaled_MuMinus_Curvature", 20, -0.03, 0.0);
  h_muMinusSumw2ScaledCurve->Sumw2();
  TH1F *h_muPlusCurve = new TH1F("MuPlus_Curvature", "MuPlus_Curvature", 20, 0.0, 0.03);
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
  int numMuMinus = 0;
  int numMuPlus = 0;
  double scale = 1.5;


  std::cout << std::endl << muonChk << std::endl;

  std::cout << "\n\tQ\tpT\tkappa\t\n";
  


  while(muonChk){
    g->cd();
    //    std::cout << "\t" << *lowerCharge << "\t" << *lowerPt << std::endl;
    //lowerCurve = *lowerCharge / (*lowerPt);
    //std::cout << "\nLower Curve is: " << lowerCurve << std::endl;
    //h_lowerCurve->Fill(lowerCurve);

    upperCurve = *upperCharge / (*upperPt);
    if(*upperCharge > 0){
      h_muonCurve->Fill(upperCurve);
      h_muPlusCurve->Fill(upperCurve);
      numMuPlus++;
    
    }
    else if(*upperCharge < 0){
      h_muonCurve->Fill(upperCurve, scale);
      h_muMinusCurve->Fill(upperCurve);
      h_muMinusScaledCurve->Fill(upperCurve, scale);
      h_muMinusSumw2ScaledCurve->Fill(upperCurve,scale);
      numMuMinus++;

    }

    std::cout << "\t" << *upperCharge << "\t" << *upperPt << "\t\t" << upperCurve << std::endl;
    
    //std::cout << "\nUpper Curve is: " << upperCurve << std::endl;
    // h_upperCurve->Fill(upperCurve);
    //    h_muonCurve->Fill(upperCurve);
    
    muonChk = muonReader.Next();

  }

  std::cout << "\n\nThe total number of muPlus are: " << numMuPlus << "  The total number of muMinus are: " << numMuMinus << std::endl << std::endl; 

  /*  plotIt2(h_muMinusScaledCurve, h_muPlusCurve, "Curvature(c/GeV)", "Number of Muons/ 15[c/GeV]", "CosmicCombinedCurve");
  plotIt2(h_muMinusSumw2ScaledCurve, h_muPlusCurve, "Curvature(c/GeV)", "Number of Muons/ 15[c/GeV]", "CosmicCombinedError");
  plotIt(h_muonCurve, 0.0, "Curvature(c/GeV)", "Number of Muons/ 15[c/GeV]", "CosmicCurve");
  plotIt(h_muMinusCurve, 0.0, "Curvature(c/GeV)", "Number of Muons/ 15[c/GeV]", "CosmicMinusCurve");
  plotIt(h_muMinusScaledCurve, 0.0, "Curvature(c/GeV)", "Number of Muons/ 15[c/GeV]", "CosmicMinusScaledCurve");
  plotIt(h_muPlusCurve, 0.0, "Curvature(c/GeV)", "Number of Muons/ 15[c/GeV]", "CosmicPlusCurve");
  */
  g->cd();
  h_muMinusScaledCurve->Write();
  h_muPlusCurve->Write();
  h_muMinusSumw2ScaledCurve->Write();
  g->Write();
  g->Close();


  return;
}

void plotIt(TH1F *hist1, double setMin, std::string const& xAxis, std::string const& yAxis, std::string const& plotName){

  TCanvas *c = new TCanvas("c", "Canvas", 1000, 1000);
  TPad *pad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);
  c->cd();

  pad->Draw();
  pad->cd();
  //pad->setLogy(1);
  hist1->SetMarkerColor(4);
  hist1->SetLineColor(4);
  hist1->SetMarkerStyle(4);
  hist1->Draw("ep0");
  hist1->GetXaxis()->SetTitle(xAxis.c_str());
  hist1->GetYaxis()->SetTitle(yAxis.c_str());
  //hist1->SetMinimum(setMin);
  
  pad->Update();
  c->Update();
   TPaveStats *stats1 = (TPaveStats*)hist1->FindObject("stats");
  stats1->SetName("Curvature");
  stats1->SetY1NDC(.7);
  stats1->SetY2NDC(.9);
  stats1->SetTextColor(4);
  stats1->SetOptStat(11111111);
  pad->Update();
  c->Update();

  c->SaveAs(TString(plotName+".jpg"));
  c->SaveAs(TString(plotName+".png"));
  c->SaveAs(TString(plotName+".pdf"));
  c->SaveAs(TString(plotName+".eps"));

  return;
}

void plotIt2(TH1F *hist1, TH1F *hist2, std::string const& xAxis, std::string const& yAxis, std::string const& plotName){

  TCanvas *c = new TCanvas("c", "Canvas", 1000, 1000);
  TPad *pad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);
  c->cd();

  pad->Draw();
  pad->cd();
  //pad->setLogy(1);
  hist1->SetMarkerColor(4);
  hist1->SetLineColor(4);
  hist1->SetMarkerStyle(4);
  hist1->Draw("ep0");
  hist1->GetXaxis()->SetTitle(xAxis.c_str());
  hist1->GetYaxis()->SetTitle(yAxis.c_str());
  hist2->SetMarkerColor(46);
  hist2->SetLineColor(46);
  hist2->SetMarkerStyle(5);
  hist2->Draw("ep0sames");
  //hist1->SetMinimum(setMin);
  
  pad->Update();
  c->Update();
   TPaveStats *stats1 = (TPaveStats*)hist1->FindObject("stats");
  stats1->SetName("Curvature");
  stats1->SetY1NDC(.7);
  stats1->SetY2NDC(.9);
  stats1->SetTextColor(4);
  stats1->SetOptStat(11111111);
  pad->Update();
  c->Update();

  TPaveStats *stats2 = (TPaveStats*)hist2->FindObject("stats");
  stats2->SetName("Curvature");
  stats2->SetY1NDC(.4);
  stats2->SetY2NDC(.6);
  stats2->SetTextColor(46);
  stats2->SetOptStat(11111111);
  pad->Update();
  c->Update();
  
  c->SaveAs(TString(plotName+".jpg"));
  c->SaveAs(TString(plotName+".png"));
  c->SaveAs(TString(plotName+".pdf"));
  c->SaveAs(TString(plotName+".eps"));

  return;
}
