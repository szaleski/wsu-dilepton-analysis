

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

/*void chargeBasedCompare(TH1F *hist1 ,TH1F *hist2, double setMin, std::string const& xAxis, std::string const& yAxis, std::string const& plotName ){

  TCanvas *c = new TCanvas("c", "Canvas", 1000, 1000);
  TPad *pad = new TPad("comp", "comp", 0.0, 0.0, 1.0, 1.0);
  c->cd();

  pad->Draw();

  pad->cd();
  pad->SetLogy(1);
  hist1->SetMarkerColor(7);
  hist1->SetLineColor(7);
  hist1->SetMarkerStyle(4);
  hist1->Draw("ep0");
  hist1->GetXaxis()->SetTitle(xAxis.c_str());
  hist1->GetYaxis()->SetTitle(yAxis.c_str());
  hist1->SetMinimum(setMin);
  hist2->SetMarkerColor(9);
  hist2->SetLineColor(9);
  hist2->SetMarkerStyle(5);
  hist2->Draw("ep0sames");

  pad->Update();
  c->Update();
  TPaveStats *stats1 = (TPaveStats*)hist1->FindObject("stats");
  stats1->SetName("noCut");  
  stats1->SetY1NDC(.7);
  stats1->SetY2NDC(.9);
  stats1->SetTextColor(7);
  stats1->SetOptStat(11111111);
  pad->Update();
  c->Update();
  TPaveStats *stats2 = (TPaveStats*)hist2->FindObject("stats");
  stats2->SetName("(200)pTCut");
  stats2->SetY1NDC(.4);
  stats2->SetY2NDC(.6);
  stats2->SetTextColor(9);
  stats2->SetOptStat(11111111);
  pad->Update();
  c->Update();
  pad->Update();
  c->Update();

  c->SaveAs(TString(plotName+".jpg"));
  c->SaveAs(TString(plotName+".png"));
  c->SaveAs(TString(plotName+".pdf"));
  c->SaveAs(TString(plotName+".eps"));




  return;

  }

  void FillHist(TH1F *hist1, ){

  //  hist1->Fill
 

  return;
  }

  void plot2D(TH2F *hist1, std::string const& xAxis, std::string const& yAxis, std::string const& plotName){
  TCanvas *e = new TCanvas("e", "Canvas", 1000, 1000);
  e->cd();
  TPad *pad3 = new TPad("comp", "comp", 0.0, 0.0, 1.0, 1.0);

  pad3->Draw();

  pad3->cd();
  hist1->Draw("colz");
  hist1->GetXaxis()->SetTitle(xAxis.c_str());
  hist1->GetYaxis()->SetTitle(yAxis.c_str());
  pad3->Update();
  e->Update();
  e->SaveAs(TString(plotName+".jpg"));
  e->SaveAs(TString(plotName+".png"));
  e->SaveAs(TString(plotName+".pdf"));
  e->SaveAs(TString(plotName+".eps"));


  return;

  }
*/

void Plot(std::string const& file1)
{
int trackVal_ = 5;
  TFile *g;
  TChain *myChain;

  if (trackVal_== 1) {
    myChain = new TChain(TString("analysisTrackerMuons/MuonTree"));
    g = new TFile("CosmicHistogramsTrackerOnly.root","RECREATE"); 
  }  
  else if (trackVal_== 2) {
    myChain = new TChain(TString("analysisTPFMSMuons/MuonTree"));
    g = new TFile("CosmicHistogramsTPFMS.root","RECREATE"); 
  }  
  else if (trackVal_== 3) {
        myChain = new TChain(TString("analysisDYTMuons/MuonTree"));
    g = new TFile("CosmicHistogramsDYTT.root","RECREATE"); 
  } 
  else if (trackVal_== 4) {
    myChain = new TChain(TString("analysisPickyMuons/MuonTree"));
    g = new TFile("CosmicHistogramsPicky.root","RECREATE"); 
  } 
  else if (trackVal_== 5) {
    myChain = new TChain(TString("analysisTunePMuons/MuonTree"));
    g = new TFile("CosmicHistogramsTuneP.root","RECREATE"); 
  } 
  else{
    std::cout << "\n\nINVALID TRACK SPECIFIED! Choose a value between [1, 5]";
    return;
    //TTreeReader trackReader("analysisTrackerMuons/MuonTree", f);
    //TTreeReader tpfmsReader("analysisTPFMSMuons/MuonTree", f);
    //TTreeReader dytReader("analysisDYTMuons/MuonTree", f);
    //TTreeReader pickyReader("analysisPickyMuons/MuonTree", f);
    //TTreeReader tunePReader("analysisTunePMuons/MuonTree", f);
  }  
  
  std::string name;
  std::ifstream file(file1);
  while(std::getline(file,name)){
    std::stringstream newString;
    newString << "root://xrootd.unl.edu//" << name;

    //Use the following line with line above commented out for running on local files.
    //newString << name;
    std::cout << newString.str() << std::endl << std::endl;
    myChain->Add(TString(newString.str()));
  }
  //  myChain->Add(TString());
  //   newString << "root://xrootd.unl.edu//" << name;
 
  TTree *myTree = myChain;
  TTreeReader trackReader(myTree);
  std::cout << "Successfully opened file 1! \n\n";

 

  std::cout<<"arg 1 is:  " << file1 << std::endl;
 
  TCanvas *c1 = new TCanvas("c1", "Boson", 1000, 1000);

  c1->cd();
  float maxBias = 0.05;
  int nBiasBins = 100;


 
  TPad *comp = new TPad("comp", "comp", 0.0, 0.0, 1.0, 1.0);


  TH1F *h_looseMuMinusChi2 = new TH1F("looseMuMinusChi2","looseMuMinusChi2", 100, 0, 50);
  TH1F *h_looseMuUpperMinusChi2 = new TH1F("looseMuUpperMinusChi2","looseMuUpperMinusChi2", 100, 0, 50);
  TH1F *h_looseMuPlusChi2 = new TH1F("looseMuPlusChi2","looseMuPlusChi2", 100, 0, 50);
  TH1F *h_looseMuUpperPlusChi2 = new TH1F("looseMuUpperPlusChi2","looseMuUpperPlusChi2", 100, 0, 50);
  TH1F *h_looseMuLowerMinusChi2 = new TH1F("looseMuLowerMinusChi2","looseMuLowerMinusChi2", 100, 0, 50);
  TH1F *h_looseMuLowerPlusChi2 = new TH1F("looseMuLowerPlusChi2","looseMuLowerPlusChi2", 100, 0, 50);
  TH1F *h_looseMuMinusNdof = new TH1F("looseMuMinusNdof","looseMuMinusNdof", 100, 0, 50);
  TH1F *h_looseMuUpperMinusNdof = new TH1F("looseMuUpperMinusNdof","looseMuUpperMinusNdof", 100, 0, 50);
  TH1F *h_looseMuPlusNdof = new TH1F("looseMuPlusNdof","looseMuPlusNdof", 100, 0, 50);
  TH1F *h_looseMuUpperPlusNdof = new TH1F("looseMuUpperPlusNdof","looseMuUpperPlusNdof", 100, 0, 50);
  TH1F *h_looseMuLowerMinusNdof = new TH1F("looseMuLowerMinusNdof","looseMuLowerMinusNdof", 100, 0, 50);
  TH1F *h_looseMuLowerPlusNdof = new TH1F("looseMuLowerPlusNdof","looseMuLowerPlusNdof", 100, 0, 50);
  TH1F *h_looseMuMinusCharge = new TH1F("looseMuMinusCharge","looseMuMinusCharge", 10, -2, 2);
  TH1F *h_looseMuUpperMinusCharge = new TH1F("looseMuUpperMinusCharge","looseMuUpperMinusCharge", 10, -2, 2);
  TH1F *h_looseMuPlusCharge = new TH1F("looseMuPlusCharge","looseMuPlusCharge", 10, -2, 2);
  TH1F *h_looseMuUpperPlusCharge = new TH1F("looseMuUpperPlusCharge","looseMuUpperPlusCharge", 10, -2, 2);
  TH1F *h_looseMuLowerMinusCharge = new TH1F("looseMuLowerMinusCharge","looseMuLowerMinusCharge", 10, -2, 2);
  TH1F *h_looseMuLowerPlusCharge = new TH1F("looseMuLowerPlusCharge","looseMuLowerPlusCharge", 10, -2, 2);
  TH1F *h_looseMuMinusCurve = new TH1F("looseMuMinusCurve","looseMuMinusCurve", 100, -1, 1);
  TH1F *h_looseMuUpperMinusCurve = new TH1F("looseMuUpperMinusCurve","looseMuUpperMinusCurve", 100, -1, 1);
  TH1F *h_looseMuPlusCurve = new TH1F("looseMuPlusCurve","looseMuPlusCurve", 100, -1, 1);
  TH1F *h_looseMuUpperPlusCurve = new TH1F("looseMuUpperPlusCurve","looseMuUpperPlusCurve", 100, -1, 1);
  TH1F *h_looseMuLowerMinusCurve = new TH1F("looseMuLowerMinusCurve","looseMuLowerMinusCurve", 100, -1, 1);
  TH1F *h_looseMuLowerPlusCurve = new TH1F("looseMuLowerPlusCurve","looseMuLowerPlusCurve", 100, -1, 1);
  TH1F *h_looseMuMinusDxy = new TH1F("looseMuMinusDxy","looseMuMinusDxy", 100, -150, 150);
  TH1F *h_looseMuUpperMinusDxy = new TH1F("looseMuUpperMinusDxy","looseMuUpperMinusDxy", 100, -150, 1500);
  TH1F *h_looseMuPlusDxy = new TH1F("looseMuPlusDxy","looseMuPlusDxy", 100, -300, 300);
  TH1F *h_looseMuUpperPlusDxy = new TH1F("looseMuUpperPlusDxy","looseMuUpperPlusDxy", 100, -300, 300);
  TH1F *h_looseMuLowerMinusDxy = new TH1F("looseMuLowerMinusDxy","looseMuLowerMinusDxy", 100, -300, 300);
  TH1F *h_looseMuLowerPlusDxy = new TH1F("looseMuLowerPlusDxy","looseMuLowerPlusDxy", 100, -300, 300);
  TH1F *h_looseMuMinusDz = new TH1F("looseMuMinusDz","looseMuMinusDz", 100, -500, 500);
  TH1F *h_looseMuUpperMinusDz = new TH1F("looseMuUpperMinusDz","looseMuUpperMinusDz", 100, -500, 500);
  TH1F *h_looseMuPlusDz = new TH1F("looseMuPlusDz","looseMuPlusDz", 100, -500, 500);
  TH1F *h_looseMuUpperPlusDz = new TH1F("looseMuUpperPlusDz","looseMuUpperPlusDz", 100, -500, 500);
  TH1F *h_looseMuLowerMinusDz = new TH1F("looseMuLowerMinusDz","looseMuLowerMinusDz", 100, -500, 500);
  TH1F *h_looseMuLowerPlusDz = new TH1F("looseMuLowerPlusDz","looseMuLowerPlusDz", 100, -500, 500);
  TH1F *h_looseMuMinusPixelHits = new TH1F("looseMuMinusPixelHits","looseMuMinusPixelHits", 20, 0, 20);
  TH1F *h_looseMuUpperMinusPixelHits = new TH1F("looseMuUpperMinusPixelHits","looseMuUpperMinusPixelHits", 20, 0, 20);
  TH1F *h_looseMuPlusPixelHits = new TH1F("looseMuPlusPixelHits","looseMuPlusPixelHits", 20, 0, 20);
  TH1F *h_looseMuUpperPlusPixelHits = new TH1F("looseMuUpperPlusPixelHits","looseMuUpperPlusPixelHits", 20, 0, 20);
  TH1F *h_looseMuLowerMinusPixelHits = new TH1F("looseMuLowerMinusPixelHits","looseMuLowerMinusPixelHits", 20, 0, 20);
  TH1F *h_looseMuLowerPlusPixelHits = new TH1F("looseMuLowerPlusPixelHits","looseMuLowerPlusPixelHits", 20, 0, 20);
  TH1F *h_looseMuMinusTrackerHits = new TH1F("looseMuMinusTrackerHits","looseMuMinusTrackerHits", 30, 0, 30);
  TH1F *h_looseMuUpperMinusTrackerHits = new TH1F("looseMuUpperMinusTrackerHits","looseMuUpperMinusTrackerHits", 30, 0, 30);
  TH1F *h_looseMuPlusTrackerHits = new TH1F("looseMuPlusTrackerHits","looseMuPlusTrackerHits", 30, 0, 30);
  TH1F *h_looseMuUpperPlusTrackerHits = new TH1F("looseMuUpperPlusTrackerHits","looseMuUpperPlusTrackerHits", 30, 0, 30);
  TH1F *h_looseMuLowerMinusTrackerHits = new TH1F("looseMuLowerMinusTrackerHits","looseMuLowerMinusTrackerHits", 30, 0, 30);
  TH1F *h_looseMuLowerPlusTrackerHits = new TH1F("looseMuLowerPlusTrackerHits","looseMuLowerPlusTrackerHits", 30, 0, 30);
  TH1F *h_looseMuMinusMuonStationHits = new TH1F("looseMuMinusMuonStationHits","looseMuMinusMuonStationHits", 10, 0, 10);
  TH1F *h_looseMuUpperMinusMuonStationHits = new TH1F("looseMuUpperMinusMuonStationHits","looseMuUpperMinusMuonStationHits", 10, 0, 10);
  TH1F *h_looseMuPlusMuonStationHits = new TH1F("looseMuPlusMuonStationHits","looseMuPlusMuonStationHits", 10, 0, 10);
  TH1F *h_looseMuUpperPlusMuonStationHits = new TH1F("looseMuUpperPlusMuonStationHits","looseMuUpperPlusMuonStationHits", 10, 0, 10);
  TH1F *h_looseMuLowerMinusMuonStationHits = new TH1F("looseMuLowerMinusMuonStationHits","looseMuLowerMinusMuonStationHits", 10, 0, 10);
  TH1F *h_looseMuLowerPlusMuonStationHits = new TH1F("looseMuLowerPlusMuonStationHits","looseMuLowerPlusMuonStationHits", 10, 0, 10);
  TH1F *h_looseMuMinusValidHits = new TH1F("looseMuMinusValidHits","looseMuMinusValidHits", 40, 0, 40);
  TH1F *h_looseMuUpperMinusValidHits = new TH1F("looseMuUpperMinusValidHits","looseMuUpperMinusValidHits", 40, 0, 40);
  TH1F *h_looseMuPlusValidHits = new TH1F("looseMuPlusValidHits","looseMuPlusValidHits", 40, 0, 40);
  TH1F *h_looseMuUpperPlusValidHits = new TH1F("looseMuUpperPlusValidHits","looseMuUpperPlusValidHits", 40, 0, 40);
  TH1F *h_looseMuLowerMinusValidHits = new TH1F("looseMuLowerMinusValidHits","looseMuLowerMinusValidHits", 40, 0, 40);
  TH1F *h_looseMuLowerPlusValidHits = new TH1F("looseMuLowerPlusValidHits","looseMuLowerPlusValidHits", 40, 0, 40);
  TH1F *h_looseMuMinusMatchedMuonStations = new TH1F("looseMuMinusMatchedMuonStations","looseMuMinusMatchedMuonStations", 10, 0, 10);
  TH1F *h_looseMuUpperMinusMatchedMuonStations = new TH1F("looseMuUpperMinusMatchedMuonStations","looseMuUpperMinusMatchedMuonStations", 10, 0, 10);
  TH1F *h_looseMuPlusMatchedMuonStations = new TH1F("looseMuPlusMatchedMuonStations","looseMuPlusMatchedMuonStations", 10, 0, 10);
  TH1F *h_looseMuUpperPlusMatchedMuonStations = new TH1F("looseMuUpperPlusMatchedMuonStations","looseMuUpperPlusMatchedMuonStations", 10, 0, 10);
  TH1F *h_looseMuLowerMinusMatchedMuonStations = new TH1F("looseMuLowerMinusMatchedMuonStations","looseMuLowerMinusMatchedMuonStations", 10, 0, 10);
  TH1F *h_looseMuLowerPlusMatchedMuonStations = new TH1F("looseMuLowerPlusMatchedMuonStations","looseMuLowerPlusMatchedMuonStations", 10, 0, 10);
  TH1F *h_looseMuMinusPtError = new TH1F("looseMuMinusPtError","looseMuMinusPtError", 100, 0, 600);
  TH1F *h_looseMuUpperMinusPtError = new TH1F("looseMuUpperMinusPtError","looseMuUpperMinusPtError", 100, 0, 600);
  TH1F *h_looseMuPlusPtError = new TH1F("looseMuPlusPtError","looseMuPlusPtError", 100, 0, 600);
  TH1F *h_looseMuUpperPlusPtError = new TH1F("looseMuUpperPlusPtError","looseMuUpperPlusPtError", 100, 0, 600);
  TH1F *h_looseMuLowerMinusPtError = new TH1F("looseMuLowerMinusPtError","looseMuLowerMinusPtError", 100, 0, 600);
  TH1F *h_looseMuLowerPlusPtError = new TH1F("looseMuLowerPlusPtError","looseMuLowerPlusPtError", 100, 0, 600);
  TH1F *h_looseMuMinusDxyError = new TH1F("looseMuMinusDxyError","looseMuMinusDxyError", 100, 0, 200);
  TH1F *h_looseMuUpperMinusDxyError = new TH1F("looseMuUpperMinusDxyError","looseMuUpperMinusDxyError", 100, 0, 200);
  TH1F *h_looseMuPlusDxyError = new TH1F("looseMuPlusDxyError","looseMuPlusDxyError", 100, 0, 200);
  TH1F *h_looseMuUpperPlusDxyError = new TH1F("looseMuUpperPlusDxyError","looseMuUpperPlusDxyError", 100, 0, 200);
  TH1F *h_looseMuLowerMinusDxyError = new TH1F("looseMuLowerMinusDxyError","looseMuLowerMinusDxyError", 100, 0, 200);
  TH1F *h_looseMuLowerPlusDxyError = new TH1F("looseMuLowerPlusDxyError","looseMuLowerPlusDxyError", 100, 0, 200);
  TH1F *h_looseMuMinusDzError = new TH1F("looseMuMinusDzError","looseMuMinusDzError", 100, 0, 200);
  TH1F *h_looseMuUpperMinusDzError = new TH1F("looseMuUpperMinusDzError","looseMuUpperMinusDzError", 100, 0, 200);
  TH1F *h_looseMuPlusDzError = new TH1F("looseMuPlusDzError","looseMuPlusDzError", 100, 0, 200);
  TH1F *h_looseMuUpperPlusDzError = new TH1F("looseMuUpperPlusDzError","looseMuUpperPlusDzError", 100, 0, 200);
  TH1F *h_looseMuLowerMinusDzError = new TH1F("looseMuLowerMinusDzError","looseMuLowerMinusDzError", 100, 0, 200);
  TH1F *h_looseMuLowerPlusDzError = new TH1F("looseMuLowerPlusDzError","looseMuLowerPlusDzError", 100, 0, 200);
  TH1F *h_looseMuMinusPt = new TH1F("looseMuMinusPt","looseMuMinusPt", 400, 0, 2000);
  TH1F *h_looseMuUpperMinusPt = new TH1F("looseMuUpperMinusPt","looseMuUpperMinusPt", 400, 0, 2000);
  TH1F *h_looseMuPlusPt = new TH1F("looseMuPlusPt","looseMuPlusPt", 400, 0, 2000);
  TH1F *h_looseMuUpperPlusPt = new TH1F("looseMuUpperPlusPt","looseMuUpperPlusPt", 400, 0, 2000);
  TH1F *h_looseMuLowerMinusPt = new TH1F("looseMuLowerMinusPt","looseMuLowerMinusPt", 400, 0, 2000);
  TH1F *h_looseMuLowerPlusPt = new TH1F("looseMuLowerPlusPt","looseMuLowerPlusPt", 400, 0, 2000);
  TH1F *h_looseMuMinusEta = new TH1F("looseMuMinusEta","looseMuMinusEta", 50, -5, 5);
  TH1F *h_looseMuUpperMinusEta = new TH1F("looseMuUpperMinusEta","looseMuUpperMinusEta", 50, -5, 5);
  TH1F *h_looseMuPlusEta = new TH1F("looseMuPlusEta","looseMuPlusEta", 50, -5, 5);
  TH1F *h_looseMuUpperPlusEta = new TH1F("looseMuUpperPlusEta","looseMuUpperPlusEta", 50, -5, 5);
  TH1F *h_looseMuLowerMinusEta = new TH1F("looseMuLowerMinusEta","looseMuLowerMinusEta", 50, -5, 5);
  TH1F *h_looseMuLowerPlusEta = new TH1F("looseMuLowerPlusEta","looseMuLowerPlusEta", 50, -5, 5);
  TH1F *h_looseMuMinusPhi = new TH1F("looseMuMinusPhi","looseMuMinusPhi", 90, -4, 4);
  TH1F *h_looseMuUpperMinusPhi = new TH1F("looseMuUpperMinusPhi","looseMuUpperMinusPhi", 90, -4, 4);
  TH1F *h_looseMuPlusPhi = new TH1F("looseMuPlusPhi","looseMuPlusPhi", 90, -4, 4);
  TH1F *h_looseMuUpperPlusPhi = new TH1F("looseMuUpperPlusPhi","looseMuUpperPlusPhi", 90, -4, 4);
  TH1F *h_looseMuLowerMinusPhi = new TH1F("looseMuLowerMinusPhi","looseMuLowerMinusPhi", 90, -4, 4);
  TH1F *h_looseMuLowerPlusPhi = new TH1F("looseMuLowerPlusPhi","looseMuLowerPlusPhi", 90, -4, 4);
  TH1F *h_looseMuMinusTrackPt = new TH1F("looseMuMinusTrackPt","looseMuMinusTrackPt", 400, 0, 2000);
  TH1F *h_looseMuUpperMinusTrackPt = new TH1F("looseMuUpperMinusTrackPt","looseMuUpperMinusTrackPt", 400, 0, 2000);
  TH1F *h_looseMuPlusTrackPt = new TH1F("looseMuPlusTrackPt","looseMuPlusTrackPt", 400, 0, 2000);
  TH1F *h_looseMuUpperPlusTrackPt = new TH1F("looseMuUpperPlusTrackPt","looseMuUpperPlusTrackPt", 400, 0, 2000);
  TH1F *h_looseMuLowerMinusTrackPt = new TH1F("looseMuLowerMinusTrackPt","looseMuLowerMinusTrackPt", 400, 0, 2000);
  TH1F *h_looseMuLowerPlusTrackPt = new TH1F("looseMuLowerPlusTrackPt","looseMuLowerPlusTrackPt", 400, 0, 2000);
  TH1F *h_looseMuMinusTrackEta = new TH1F("looseMuMinusTrackEta","looseMuMinusTrackEta", 50, -5, 5);
  TH1F *h_looseMuUpperMinusTrackEta = new TH1F("looseMuUpperMinusTrackEta","looseMuUpperMinusTrackEta", 50, -5, 5);
  TH1F *h_looseMuPlusTrackEta = new TH1F("looseMuPlusTrackEta","looseMuPlusTrackEta", 50, -5, 5);
  TH1F *h_looseMuUpperPlusTrackEta = new TH1F("looseMuUpperPlusTrackEta","looseMuUpperPlusTrackEta", 50, -5, 5);
  TH1F *h_looseMuLowerMinusTrackEta = new TH1F("looseMuLowerMinusTrackEta","looseMuLowerMinusTrackEta", 50, -5, 5);
  TH1F *h_looseMuLowerPlusTrackEta = new TH1F("looseMuLowerPlusTrackEta","looseMuLowerPlusTrackEta", 50, -5, 5);
  TH1F *h_looseMuMinusTrackPhi = new TH1F("looseMuMinusTrackPhi","looseMuMinusTrackPhi", 90, -4, 4);
  TH1F *h_looseMuUpperMinusTrackPhi = new TH1F("looseMuUpperMinusTrackPhi","looseMuUpperMinusTrackPhi", 90, -4, 4);
  TH1F *h_looseMuPlusTrackPhi = new TH1F("looseMuPlusTrackPhi","looseMuPlusTrackPhi", 90, -4, 4);
  TH1F *h_looseMuUpperPlusTrackPhi = new TH1F("looseMuUpperPlusTrackPhi","looseMuUpperPlusTrackPhi", 90, -4, 4);
  TH1F *h_looseMuLowerMinusTrackPhi = new TH1F("looseMuLowerMinusTrackPhi","looseMuLowerMinusTrackPhi", 90, -4, 4);
  TH1F *h_looseMuLowerPlusTrackPhi = new TH1F("looseMuLowerPlusTrackPhi","looseMuLowerPlusTrackPhi", 90, -4, 4);
  TH1F *h_looseMuMinusTrackLayersWithMeasurement = new TH1F("looseMuMinusTrackLayersWithMeasurement","looseMuMinusTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_looseMuUpperMinusTrackLayersWithMeasurement = new TH1F("looseMuUpperMinusTrackLayersWithMeasurement","looseMuUpperMinusTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_looseMuPlusTrackLayersWithMeasurement = new TH1F("looseMuPlusTrackLayersWithMeasurement","looseMuPlusTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_looseMuUpperPlusTrackLayersWithMeasurement = new TH1F("looseMuUpperPlusTrackLayersWithMeasurement","looseMuUpperPlusTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_looseMuLowerMinusTrackLayersWithMeasurement = new TH1F("looseMuLowerMinusTrackLayersWithMeasurement","looseMuLowerMinusTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_looseMuLowerPlusTrackLayersWithMeasurement = new TH1F("looseMuLowerPlusTrackLayersWithMeasurement","looseMuLowerPlusTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_looseMuMinusCurvePlusBias[nBiasBins];
  TH1F *h_looseMuMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuMinusCurvePlusBias[i] = new TH1F(TString("looseMuMinusCurvePlusBias" + name.str()), TString("looseMuMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_looseMuMinusCurveMinusBias[i] = new TH1F(TString("looseMuMinusCurveMinusBias" + name.str()),TString("looseMuMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_looseMuUpperMinusCurvePlusBias[nBiasBins];
  TH1F *h_looseMuUpperMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuUpperMinusCurvePlusBias[i] = new TH1F(TString("looseMuUpperMinusCurvePlusBias" + name.str()), TString("looseMuUpperMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_looseMuUpperMinusCurveMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveMinusBias" + name.str()),TString("looseMuUpperMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_looseMuLowerMinusCurvePlusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuLowerMinusCurvePlusBias[i] = new TH1F(TString("looseMuLowerMinusCurvePlusBias" + name.str()), TString("looseMuLowerMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_looseMuLowerMinusCurveMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveMinusBias" + name.str()),TString("looseMuLowerMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_looseMuPlusCurvePlusBias[nBiasBins];
  TH1F *h_looseMuPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuPlusCurvePlusBias[i] = new TH1F(TString("looseMuPlusCurvePlusBias" + name.str()), TString("looseMuPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_looseMuPlusCurveMinusBias[i] = new TH1F(TString("looseMuPlusCurveMinusBias" + name.str()),TString("looseMuPlusCurveMinusBias" + name.str()), 100, -01, 0.1);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_looseMuUpperPlusCurvePlusBias[nBiasBins];
  TH1F *h_looseMuUpperPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuUpperPlusCurvePlusBias[i] = new TH1F(TString("looseMuUpperPlusCurvePlusBias" + name.str()), TString("looseMuUpperPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_looseMuUpperPlusCurveMinusBias[i] = new TH1F(TString("looseMuUpperPlusCurveMinusBias" + name.str()),TString("looseMuUpperPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_looseMuLowerPlusCurvePlusBias[nBiasBins];
  TH1F *h_looseMuLowerPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuLowerPlusCurvePlusBias[i] = new TH1F(TString("looseMuLowerPlusCurvePlusBias" + name.str()), TString("looseMuLowerPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_looseMuLowerPlusCurveMinusBias[i] = new TH1F(TString("looseMuLowerPlusCurveMinusBias" + name.str()),TString("looseMuLowerPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }


  TH1F *h_tightMuMinusChi2 = new TH1F("TightMuonChi2","TightMuonChi2", 100, 0, 50);
  TH1F *h_tightMuUpperMinusChi2 = new TH1F("UpperTightMuonChi2","UpperTightMuonChi2", 100, 0, 50);
  TH1F *h_tightMuPlusChi2 = new TH1F("AntiTightMuonChi2","AntiTightMuonChi2", 100, 0, 50);
  TH1F *h_tightMuUpperPlusChi2 = new TH1F("UpperAntiTightMuonChi2","UpperAntiTightMuonChi2", 100, 0, 50);
  TH1F *h_tightMuLowerMinusChi2 = new TH1F("LowerTightMuonChi2","LowerTightMuonChi2", 100, 0, 50);
  TH1F *h_tightMuLowerPlusChi2 = new TH1F("LowerAntiTightMuonChi2","LowerAntiTightMuonChi2", 100, 0, 50);
  TH1F *h_tightMuMinusNdof = new TH1F("TightMuonNdof","TightMuonNdof", 100, 0, 50);
  TH1F *h_tightMuUpperMinusNdof = new TH1F("UpperTightMuonNdof","UpperTightMuonNdof", 100, 0, 50);
  TH1F *h_tightMuPlusNdof = new TH1F("AntiTightMuonNdof","AntiTightMuonNdof", 100, 0, 50);
  TH1F *h_tightMuUpperPlusNdof = new TH1F("UpperAntiTightMuonNdof","UpperAntiTightMuonNdof", 100, 0, 50);
  TH1F *h_tightMuLowerMinusNdof = new TH1F("LowerTightMuonNdof","LowerTightMuonNdof", 100, 0, 50);
  TH1F *h_tightMuLowerPlusNdof = new TH1F("LowerAntiTightMuonNdof","LowerAntiTightMuonNdof", 100, 0, 50);
  TH1F *h_tightMuMinusCharge = new TH1F("TightMuonCharge","TightMuonCharge", 10, -2, 2);
  TH1F *h_tightMuUpperMinusCharge = new TH1F("UpperTightMuonCharge","UpperTightMuonCharge", 10, -2, 2);
  TH1F *h_tightMuPlusCharge = new TH1F("AntiTightMuonCharge","AntiTightMuonCharge", 10, -2, 2);
  TH1F *h_tightMuUpperPlusCharge = new TH1F("UpperAntiTightMuonCharge","UpperAntiTightMuonCharge", 10, -2, 2);
  TH1F *h_tightMuLowerMinusCharge = new TH1F("LowerTightMuonCharge","LowerTightMuonCharge", 10, -2, 2);
  TH1F *h_tightMuLowerPlusCharge = new TH1F("LowerAntiTightMuonCharge","LowerAntiTightMuonCharge", 10, -2, 2);
  TH1F *h_tightMuMinusCurve = new TH1F("TightMuonCurve","TightMuonCurve", 100, -1, 1);
  TH1F *h_tightMuUpperMinusCurve = new TH1F("UpperTightMuonCurve","UpperTightMuonCurve", 100, -1, 1);
  TH1F *h_tightMuPlusCurve = new TH1F("AntiTightMuonCurve","AntiTightMuonCurve", 100, -1, 1);
  TH1F *h_tightMuUpperPlusCurve = new TH1F("UpperAntiTightMuonCurve","UpperAntiTightMuonCurve", 100, -1, 1);
  TH1F *h_tightMuLowerMinusCurve = new TH1F("LowerTightMuonCurve","LowerTightMuonCurve", 100, -1, 1);
  TH1F *h_tightMuLowerPlusCurve = new TH1F("LowerAntiTightMuonCurve","LowerAntiTightMuonCurve", 100, -1, 1);
  TH1F *h_tightMuMinusDxy = new TH1F("TightMuonDxy","TightMuonDxy", 100, -300, 300);
  TH1F *h_tightMuUpperMinusDxy = new TH1F("UpperTightMuonDxy","UpperTightMuonDxy", 100, -300, 300);
  TH1F *h_tightMuPlusDxy = new TH1F("AntiTightMuonDxy","AntiTightMuonDxy", 100, -300, 300);
  TH1F *h_tightMuUpperPlusDxy = new TH1F("UpperAntiTightMuonDxy","UpperAntiTightMuonDxy", 100, -300, 300);
  TH1F *h_tightMuLowerMinusDxy = new TH1F("LowerTightMuonDxy","LowerTightMuonDxy", 100, -300, 300);
  TH1F *h_tightMuLowerPlusDxy = new TH1F("LowerAntiTightMuonDxy","LowerAntiTightMuonDxy", 100, -300, 300);
  TH1F *h_tightMuMinusDz = new TH1F("TightMuonDz","TightMuonDz", 100, -500, 500);
  TH1F *h_tightMuUpperMinusDz = new TH1F("UpperTightMuonDz","UpperTightMuonDz", 100, -500, 500);
  TH1F *h_tightMuPlusDz = new TH1F("AntiTightMuonDz","AntiTightMuonDz", 100, -500, 500);
  TH1F *h_tightMuUpperPlusDz = new TH1F("UpperAntiTightMuonDz","UpperAntiTightMuonDz", 100, -500, 500);
  TH1F *h_tightMuLowerMinusDz = new TH1F("LowerTightMuonDz","LowerTightMuonDz", 100, -500, 500);
  TH1F *h_tightMuLowerPlusDz = new TH1F("LowerAntiTightMuonDz","LowerAntiTightMuonDz", 100, -500, 500);
  TH1F *h_tightMuMinusPixelHits = new TH1F("TightMuonPixelHits","TightMuonPixelHits", 20, 0, 20);
  TH1F *h_tightMuUpperMinusPixelHits = new TH1F("UpperTightMuonPixelHits","UpperTightMuonPixelHits", 20, 0, 20);
  TH1F *h_tightMuPlusPixelHits = new TH1F("AntiTightMuonPixelHits","AntiTightMuonPixelHits", 20, 0, 20);
  TH1F *h_tightMuUpperPlusPixelHits = new TH1F("UpperAntiTightMuonPixelHits","UpperAntiTightMuonPixelHits", 20, 0, 20);
  TH1F *h_tightMuLowerMinusPixelHits = new TH1F("LowerTightMuonPixelHits","LowerTightMuonPixelHits", 20, 0, 20);
  TH1F *h_tightMuLowerPlusPixelHits = new TH1F("LowerAntiTightMuonPixelHits","LowerAntiTightMuonPixelHits", 20, 0, 20);
  TH1F *h_tightMuMinusTrackerHits = new TH1F("TightMuonTrackerHits","TightMuonTrackerHits", 30, 0, 30);
  TH1F *h_tightMuUpperMinusTrackerHits = new TH1F("UpperTightMuonTrackerHits","UpperTightMuonTrackerHits", 30, 0, 30);
  TH1F *h_tightMuPlusTrackerHits = new TH1F("AntiTightMuonTrackerHits","AntiTightMuonTrackerHits", 30, 0, 30);
  TH1F *h_tightMuUpperPlusTrackerHits = new TH1F("UpperAntiTightMuonTrackerHits","UpperAntiTightMuonTrackerHits", 30, 0, 30);
  TH1F *h_tightMuLowerMinusTrackerHits = new TH1F("LowerTightMuonTrackerHits","LowerTightMuonTrackerHits", 30, 0, 30);
  TH1F *h_tightMuLowerPlusTrackerHits = new TH1F("LowerAntiTightMuonTrackerHits","LowerAntiTightMuonTrackerHits", 30, 0, 30);
  TH1F *h_tightMuMinusMuonStationHits = new TH1F("TightMuonTightMuonStationHits","TightMuonTightMuonStationHits", 10, 0, 10);
  TH1F *h_tightMuUpperMinusMuonStationHits = new TH1F("UpperTightMuonTightMuonStationHits","UpperTightMuonTightMuonStationHits", 10, 0, 10);
  TH1F *h_tightMuPlusMuonStationHits = new TH1F("AntiTightMuonTightMuonStationHits","AntiTightMuonTightMuonStationHits", 10, 0, 10);
  TH1F *h_tightMuUpperPlusMuonStationHits = new TH1F("UpperAntiTightMuonTightMuonStationHits","UpperAntiTightMuonTightMuonStationHits", 10, 0, 10);
  TH1F *h_tightMuLowerMinusMuonStationHits = new TH1F("LowerTightMuonTightMuonStationHits","LowerTightMuonTightMuonStationHits", 10, 0, 10);
  TH1F *h_tightMuLowerPlusMuonStationHits = new TH1F("LowerAntiTightMuonTightMuonStationHits","LowerAntiTightMuonTightMuonStationHits", 10, 0, 10);
  TH1F *h_tightMuMinusValidHits = new TH1F("TightMuonValidHits","TightMuonValidHits", 40, 0, 40);
  TH1F *h_tightMuUpperMinusValidHits = new TH1F("UpperTightMuonValidHits","UpperTightMuonValidHits", 40, 0, 40);
  TH1F *h_tightMuPlusValidHits = new TH1F("AntiTightMuonValidHits","AntiTightMuonValidHits", 40, 0, 40);
  TH1F *h_tightMuUpperPlusValidHits = new TH1F("UpperAntiTightMuonValidHits","UpperAntiTightMuonValidHits", 40, 0, 40);
  TH1F *h_tightMuLowerMinusValidHits = new TH1F("LowerTightMuonValidHits","LowerTightMuonValidHits", 40, 0, 40);
  TH1F *h_tightMuLowerPlusValidHits = new TH1F("LowerAntiTightMuonValidHits","LowerAntiTightMuonValidHits", 40, 0, 40);
  TH1F *h_tightMuMinusMatchedMuonStations = new TH1F("TightMuonMatchedTightMuonStations","TightMuonMatchedTightMuonStations", 10, 0, 10);
  TH1F *h_tightMuUpperMinusMatchedMuonStations = new TH1F("UpperTightMuonMatchedTightMuonStations","UpperTightMuonMatchedTightMuonStations", 10, 0, 10);
  TH1F *h_tightMuPlusMatchedMuonStations = new TH1F("AntiTightMuonMatchedTightMuonStations","AntiTightMuonMatchedTightMuonStations", 10, 0, 10);
  TH1F *h_tightMuUpperPlusMatchedMuonStations = new TH1F("UpperAntiTightMuonMatchedTightMuonStations","UpperAntiTightMuonMatchedTightMuonStations", 10, 0, 10);
  TH1F *h_tightMuLowerMinusMatchedMuonStations = new TH1F("LowerTightMuonMatchedTightMuonStations","LowerTightMuonMatchedTightMuonStations", 10, 0, 10);
  TH1F *h_tightMuLowerPlusMatchedMuonStations = new TH1F("LowerAntiTightMuonMatchedTightMuonStations","LowerAntiTightMuonMatchedTightMuonStations", 10, 0, 10);
  TH1F *h_tightMuMinusPtError = new TH1F("TightMuonPtError","TightMuonPtError", 100, 0, 600);
  TH1F *h_tightMuUpperMinusPtError = new TH1F("UpperTightMuonPtError","UpperTightMuonPtError", 100, 0, 600);
  TH1F *h_tightMuPlusPtError = new TH1F("AntiTightMuonPtError","AntiTightMuonPtError", 100, 0, 600);
  TH1F *h_tightMuUpperPlusPtError = new TH1F("UpperAntiTightMuonPtError","UpperAntiTightMuonPtError", 100, 0, 600);
  TH1F *h_tightMuLowerMinusPtError = new TH1F("LowerTightMuonPtError","LowerTightMuonPtError", 100, 0, 600);
  TH1F *h_tightMuLowerPlusPtError = new TH1F("LowerAntiTightMuonPtError","LowerAntiTightMuonPtError", 100, 0, 600);
  TH1F *h_tightMuMinusDxyError = new TH1F("TightMuonDxyError","TightMuonDxyError", 100, 0, 200);
  TH1F *h_tightMuUpperMinusDxyError = new TH1F("UpperTightMuonDxyError","UpperTightMuonDxyError", 100, 0, 200);
  TH1F *h_tightMuPlusDxyError = new TH1F("AntiTightMuonDxyError","AntiTightMuonDxyError", 100, 0, 200);
  TH1F *h_tightMuUpperPlusDxyError = new TH1F("UpperAntiTightMuonDxyError","UpperAntiTightMuonDxyError", 100, 0, 200);
  TH1F *h_tightMuLowerMinusDxyError = new TH1F("LowerTightMuonDxyError","LowerTightMuonDxyError", 100, 0, 200);
  TH1F *h_tightMuLowerPlusDxyError = new TH1F("LowerAntiTightMuonDxyError","LowerAntiTightMuonDxyError", 100, 0, 200);
  TH1F *h_tightMuMinusDzError = new TH1F("TightMuonDzError","TightMuonDzError", 100, 0, 200);
  TH1F *h_tightMuUpperMinusDzError = new TH1F("UpperTightMuonDzError","UpperTightMuonDzError", 100, 0, 200);
  TH1F *h_tightMuPlusDzError = new TH1F("AntiTightMuonDzError","AntiTightMuonDzError", 100, 0, 200);
  TH1F *h_tightMuUpperPlusDzError = new TH1F("UpperAntiTightMuonDzError","UpperAntiTightMuonDzError", 100, 0, 200);
  TH1F *h_tightMuLowerMinusDzError = new TH1F("LowerTightMuonDzError","LowerTightMuonDzError", 100, 0, 200);
  TH1F *h_tightMuLowerPlusDzError = new TH1F("LowerAntiTightMuonDzError","LowerAntiTightMuonDzError", 100, 0, 200);
  TH1F *h_tightMuMinusPt = new TH1F("TightMuonPt","TightMuonPt", 400, 0, 2000);
  TH1F *h_tightMuUpperMinusPt = new TH1F("UpperTightMuonPt","UpperTightMuonPt", 400, 0, 2000);
  TH1F *h_tightMuPlusPt = new TH1F("AntiTightMuonPt","AntiTightMuonPt", 400, 0, 2000);
  TH1F *h_tightMuUpperPlusPt = new TH1F("UpperAntiTightMuonPt","UpperAntiTightMuonPt", 400, 0, 2000);
  TH1F *h_tightMuLowerMinusPt = new TH1F("LowerTightMuonPt","LowerTightMuonPt", 400, 0, 2000);
  TH1F *h_tightMuLowerPlusPt = new TH1F("LowerAntiTightMuonPt","LowerAntiTightMuonPt", 400, 0, 2000);
  TH1F *h_tightMuMinusEta = new TH1F("TightMuonEta","TightMuonEta", 50, -5, 5);
  TH1F *h_tightMuUpperMinusEta = new TH1F("UpperTightMuonEta","UpperTightMuonEta", 50, -5, 5);
  TH1F *h_tightMuPlusEta = new TH1F("AntiTightMuonEta","AntiTightMuonEta", 50, -5, 5);
  TH1F *h_tightMuUpperPlusEta = new TH1F("UpperAntiTightMuonEta","UpperAntiTightMuonEta", 50, -5, 5);
  TH1F *h_tightMuLowerMinusEta = new TH1F("LowerTightMuonEta","LowerTightMuonEta", 50, -5, 5);
  TH1F *h_tightMuLowerPlusEta = new TH1F("LowerAntiTightMuonEta","LowerAntiTightMuonEta", 50, -5, 5);
  TH1F *h_tightMuMinusPhi = new TH1F("TightMuonPhi","TightMuonPhi", 90, -4, 4);
  TH1F *h_tightMuUpperMinusPhi = new TH1F("UpperTightMuonPhi","UpperTightMuonPhi", 90, -4, 4);
  TH1F *h_tightMuPlusPhi = new TH1F("AntiTightMuonPhi","AntiTightMuonPhi", 90, -4, 4);
  TH1F *h_tightMuUpperPlusPhi = new TH1F("UpperAntiTightMuonPhi","UpperAntiTightMuonPhi", 90, -4, 4);
  TH1F *h_tightMuLowerMinusPhi = new TH1F("LowerTightMuonPhi","LowerTightMuonPhi", 90, -4, 4);
  TH1F *h_tightMuLowerPlusPhi = new TH1F("LowerAntiTightMuonPhi","LowerAntiTightMuonPhi", 90, -4, 4);
  TH1F *h_tightMuMinusTrackPt = new TH1F("TightMuonTrackPt","TightMuonTrackPt", 1000, 0, 2000);
  TH1F *h_tightMuUpperMinusTrackPt = new TH1F("UpperTightMuonTrackPt","UpperTightMuonTrackPt", 1000, 0, 2000);
  TH1F *h_tightMuPlusTrackPt = new TH1F("AntiTightMuonTrackPt","AntiTightMuonTrackPt", 1000, 0, 2000);
  TH1F *h_tightMuUpperPlusTrackPt = new TH1F("UpperAntiTightMuonTrackPt","UpperAntiTightMuonTrackPt", 1000, 0, 2000);
  TH1F *h_tightMuLowerMinusTrackPt = new TH1F("LowerTightMuonTrackPt","LowerTightMuonTrackPt", 1000, 0, 2000);
  TH1F *h_tightMuLowerPlusTrackPt = new TH1F("LowerAntiTightMuonTrackPt","LowerAntiTightMuonTrackPt", 1000, 0, 2000);
  TH1F *h_tightMuMinusTrackEta = new TH1F("TightMuonTrackEta","TightMuonTrackEta", 42, -10, 10);
  TH1F *h_tightMuUpperMinusTrackEta = new TH1F("UpperTightMuonTrackEta","UpperTightMuonTrackEta", 42, -10, 10);
  TH1F *h_tightMuPlusTrackEta = new TH1F("AntiTightMuonTrackEta","AntiTightMuonTrackEta", 42, -10, 10);
  TH1F *h_tightMuUpperPlusTrackEta = new TH1F("UpperAntiTightMuonTrackEta","UpperAntiTightMuonTrackEta", 42, -10, 10);
  TH1F *h_tightMuLowerMinusTrackEta = new TH1F("LowerTightMuonTrackEta","LowerTightMuonTrackEta", 42, -10, 10);
  TH1F *h_tightMuLowerPlusTrackEta = new TH1F("LowerAntiTightMuonTrackEta","LowerAntiTightMuonTrackEta", 42, -10, 10);
  TH1F *h_tightMuMinusTrackPhi = new TH1F("TightMuonTrackPhi","TightMuonTrackPhi", 90, -4, 4);
  TH1F *h_tightMuUpperMinusTrackPhi = new TH1F("UpperTightMuonTrackPhi","UpperTightMuonTrackPhi", 90, -4, 4);
  TH1F *h_tightMuPlusTrackPhi = new TH1F("AntiTightMuonTrackPhi","AntiTightMuonTrackPhi", 90, -4, 4);
  TH1F *h_tightMuUpperPlusTrackPhi = new TH1F("UpperAntiTightMuonTrackPhi","UpperAntiTightMuonTrackPhi", 90, -4, 4);
  TH1F *h_tightMuLowerMinusTrackPhi = new TH1F("LowerTightMuonTrackPhi","LowerTightMuonTrackPhi", 90, -4, 4);
  TH1F *h_tightMuLowerPlusTrackPhi = new TH1F("LowerAntiTightMuonTrackPhi","LowerAntiTightMuonTrackPhi", 90, -4, 4);
  TH1F *h_tightMuMinusTrackLayersWithMeasurement = new TH1F("TightMuonTrackLayersWithMeasurement","TightMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_tightMuUpperMinusTrackLayersWithMeasurement = new TH1F("UpperTightMuonTrackLayersWithMeasurement","UpperTightMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_tightMuPlusTrackLayersWithMeasurement = new TH1F("AntiTightMuonTrackLayersWithMeasurement","AntiTightMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_tightMuUpperPlusTrackLayersWithMeasurement = new TH1F("UpperAntiTightMuonTrackLayersWithMeasurement","UpperAntiTightMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_tightMuLowerMinusTrackLayersWithMeasurement = new TH1F("LowerTightMuonTrackLayersWithMeasurement","LowerTightMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_tightMuLowerPlusTrackLayersWithMeasurement = new TH1F("LowerAntiTightMuonTrackLayersWithMeasurement","LowerAntiTightMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_tightMuMinusCurvePlusBias[nBiasBins];
  TH1F *h_tightMuMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_tightMuMinusCurvePlusBias[i] = new TH1F(TString("tightMuMinusCurvePlusBias" + name.str()), TString("tightMuMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_tightMuMinusCurveMinusBias[i] = new TH1F(TString("tightMuMinusCurveMinusBias" + name.str()),TString("tightMuMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_tightMuUpperMinusCurvePlusBias[nBiasBins];
  TH1F *h_tightMuUpperMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_tightMuUpperMinusCurvePlusBias[i] = new TH1F(TString("tightMuUpperMinusCurvePlusBias" + name.str()), TString("tightMuUpperMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_tightMuUpperMinusCurveMinusBias[i] = new TH1F(TString("tightMuUpperMinusCurveMinusBias" + name.str()),TString("tightMuUpperMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_tightMuLowerMinusCurvePlusBias[nBiasBins];
  TH1F *h_tightMuLowerMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_tightMuLowerMinusCurvePlusBias[i] = new TH1F(TString("tightMuLowerMinusCurvePlusBias" + name.str()), TString("tightMuLowerMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_tightMuLowerMinusCurveMinusBias[i] = new TH1F(TString("tightMuLowerMinusCurveMinusBias" + name.str()),TString("tightMuLowerMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_tightMuPlusCurvePlusBias[nBiasBins];
  TH1F *h_tightMuPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_tightMuPlusCurvePlusBias[i] = new TH1F(TString("tightMuPlusCurvePlusBias" + name.str()), TString("tightMuPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_tightMuPlusCurveMinusBias[i] = new TH1F(TString("tightMuPlusCurveMinusBias" + name.str()),TString("tightMuPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_tightMuUpperPlusCurvePlusBias[nBiasBins];
  TH1F *h_tightMuUpperPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_tightMuUpperPlusCurvePlusBias[i] = new TH1F(TString("tightMuUpperPlusCurvePlusBias" + name.str()), TString("tightMuUpperPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_tightMuUpperPlusCurveMinusBias[i] = new TH1F(TString("tightMuUpperPlusCurveMinusBias" + name.str()),TString("tightMuUpperPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_tightMuLowerPlusCurvePlusBias[nBiasBins];
  TH1F *h_tightMuLowerPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_tightMuLowerPlusCurvePlusBias[i] = new TH1F(TString("tightMuLowerPlusCurvePlusBias" + name.str()), TString("tightMuLowerPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_tightMuLowerPlusCurveMinusBias[i] = new TH1F(TString("tightMuLowerPlusCurveMinusBias" + name.str()),TString("tightMuLowerPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }

  TH1F *h_muMinusChi2 = new TH1F("MuonChi2","MuonChi2", 100, 0, 50);
  TH1F *h_muUpperMinusChi2 = new TH1F("UpperMuonChi2","UpperMuonChi2", 100, 0, 50);
  TH1F *h_muPlusChi2 = new TH1F("AntiMuonChi2","AntiMuonChi2", 100, 0, 50);
  TH1F *h_muUpperPlusChi2 = new TH1F("UpperAntiMuonChi2","UpperAntiMuonChi2", 100, 0, 50);
  TH1F *h_muLowerMinusChi2 = new TH1F("LowerMuonChi2","LowerMuonChi2", 100, 0, 50);
  TH1F *h_muLowerPlusChi2 = new TH1F("LowerAntiMuonChi2","LowerAntiMuonChi2", 100, 0, 50);
  TH1F *h_muMinusNdof = new TH1F("MuonNdof","MuonNdof", 100, 0, 50);
  TH1F *h_muUpperMinusNdof = new TH1F("UpperMuonNdof","UpperMuonNdof", 100, 0, 50);
  TH1F *h_muPlusNdof = new TH1F("AntiMuonNdof","AntiMuonNdof", 100, 0, 50);
  TH1F *h_muUpperPlusNdof = new TH1F("UpperAntiMuonNdof","UpperAntiMuonNdof", 100, 0, 50);
  TH1F *h_muLowerMinusNdof = new TH1F("LowerMuonNdof","LowerMuonNdof", 100, 0, 50);
  TH1F *h_muLowerPlusNdof = new TH1F("LowerAntiMuonNdof","LowerAntiMuonNdof", 100, 0, 50);
  TH1F *h_muMinusCharge = new TH1F("MuonCharge","MuonCharge", 10, -2, 2);
  TH1F *h_muUpperMinusCharge = new TH1F("UpperMuonCharge","UpperMuonCharge", 10, -2, 2);
  TH1F *h_muPlusCharge = new TH1F("AntiMuonCharge","AntiMuonCharge", 10, -2, 2);
  TH1F *h_muUpperPlusCharge = new TH1F("UpperAntiMuonCharge","UpperAntiMuonCharge", 10, -2, 2);
  TH1F *h_muLowerMinusCharge = new TH1F("LowerMuonCharge","LowerMuonCharge", 10, -2, 2);
  TH1F *h_muLowerPlusCharge = new TH1F("LowerAntiMuonCharge","LowerAntiMuonCharge", 10, -2, 2);
  TH1F *h_muMinusCurve = new TH1F("MuonCurve","MuonCurve", 100, -1, 1);
  TH1F *h_muUpperMinusCurve = new TH1F("UpperMuonCurve","UpperMuonCurve", 100, -1, 1);
  TH1F *h_muPlusCurve = new TH1F("AntiMuonCurve","AntiMuonCurve", 100, -1, 1);
  TH1F *h_muUpperPlusCurve = new TH1F("UpperAntiMuonCurve","UpperAntiMuonCurve", 100, -1, 1);
  TH1F *h_muLowerMinusCurve = new TH1F("LowerMuonCurve","LowerMuonCurve", 100, -1, 1);
  TH1F *h_muLowerPlusCurve = new TH1F("LowerAntiMuonCurve","LowerAntiMuonCurve", 100, -1, 1);
  TH1F *h_muMinusDxy = new TH1F("MuonDxy","MuonDxy", 100, -300, 300);
  TH1F *h_muUpperMinusDxy = new TH1F("UpperMuonDxy","UpperMuonDxy", 100, -300, 300);
  TH1F *h_muPlusDxy = new TH1F("AntiMuonDxy","AntiMuonDxy", 100, -300, 300);
  TH1F *h_muUpperPlusDxy = new TH1F("UpperAntiMuonDxy","UpperAntiMuonDxy", 100, -300, 300);
  TH1F *h_muLowerMinusDxy = new TH1F("LowerMuonDxy","LowerMuonDxy", 100, -300, 300);
  TH1F *h_muLowerPlusDxy = new TH1F("LowerAntiMuonDxy","LowerAntiMuonDxy", 100, -300, 300);
  TH1F *h_muMinusDz = new TH1F("MuonDz","MuonDz", 100, -500, 500);
  TH1F *h_muUpperMinusDz = new TH1F("UpperMuonDz","UpperMuonDz", 100, -500, 500);
  TH1F *h_muPlusDz = new TH1F("AntiMuonDz","AntiMuonDz", 100, -500, 500);
  TH1F *h_muUpperPlusDz = new TH1F("UpperAntiMuonDz","UpperAntiMuonDz", 100, -500, 500);
  TH1F *h_muLowerMinusDz = new TH1F("LowerMuonDz","LowerMuonDz", 100, -500, 500);
  TH1F *h_muLowerPlusDz = new TH1F("LowerAntiMuonDz","LowerAntiMuonDz", 100, -500, 500);
  TH1F *h_muMinusPixelHits = new TH1F("MuonPixelHits","MuonPixelHits", 20, 0, 20);
  TH1F *h_muUpperMinusPixelHits = new TH1F("UpperMuonPixelHits","UpperMuonPixelHits", 20, 0, 20);
  TH1F *h_muPlusPixelHits = new TH1F("AntiMuonPixelHits","AntiMuonPixelHits", 20, 0, 20);
  TH1F *h_muUpperPlusPixelHits = new TH1F("UpperAntiMuonPixelHits","UpperAntiMuonPixelHits", 20, 0, 20);
  TH1F *h_muLowerMinusPixelHits = new TH1F("LowerMuonPixelHits","LowerMuonPixelHits", 20, 0, 20);
  TH1F *h_muLowerPlusPixelHits = new TH1F("LowerAntiMuonPixelHits","LowerAntiMuonPixelHits", 20, 0, 20);
  TH1F *h_muMinusTrackerHits = new TH1F("MuonTrackerHits","MuonTrackerHits", 30, 0, 30);
  TH1F *h_muUpperMinusTrackerHits = new TH1F("UpperMuonTrackerHits","UpperMuonTrackerHits", 30, 0, 30);
  TH1F *h_muPlusTrackerHits = new TH1F("AntiMuonTrackerHits","AntiMuonTrackerHits", 30, 0, 30);
  TH1F *h_muUpperPlusTrackerHits = new TH1F("UpperAntiMuonTrackerHits","UpperAntiMuonTrackerHits", 30, 0, 30);
  TH1F *h_muLowerMinusTrackerHits = new TH1F("LowerMuonTrackerHits","LowerMuonTrackerHits", 30, 0, 30);
  TH1F *h_muLowerPlusTrackerHits = new TH1F("LowerAntiMuonTrackerHits","LowerAntiMuonTrackerHits", 30, 0, 30);
  TH1F *h_muMinusMuonStationHits = new TH1F("MuonMuonStationHits","MuonMuonStationHits", 10, 0, 10);
  TH1F *h_muUpperMinusMuonStationHits = new TH1F("UpperMuonMuonStationHits","UpperMuonMuonStationHits", 10, 0, 10);
  TH1F *h_muPlusMuonStationHits = new TH1F("AntiMuonMuonStationHits","AntiMuonMuonStationHits", 10, 0, 10);
  TH1F *h_muUpperPlusMuonStationHits = new TH1F("UpperAntiMuonMuonStationHits","UpperAntiMuonMuonStationHits", 10, 0, 10);
  TH1F *h_muLowerMinusMuonStationHits = new TH1F("LowerMuonMuonStationHits","LowerMuonMuonStationHits", 10, 0, 10);
  TH1F *h_muLowerPlusMuonStationHits = new TH1F("LowerAntiMuonMuonStationHits","LowerAntiMuonMuonStationHits", 10, 0, 10);
  TH1F *h_muMinusValidHits = new TH1F("MuonValidHits","MuonValidHits", 40, 0, 40);
  TH1F *h_muUpperMinusValidHits = new TH1F("UpperMuonValidHits","UpperMuonValidHits", 40, 0, 40);
  TH1F *h_muPlusValidHits = new TH1F("AntiMuonValidHits","AntiMuonValidHits", 40, 0, 40);
  TH1F *h_muUpperPlusValidHits = new TH1F("UpperAntiMuonValidHits","UpperAntiMuonValidHits", 40, 0, 40);
  TH1F *h_muLowerMinusValidHits = new TH1F("LowerMuonValidHits","LowerMuonValidHits", 40, 0, 40);
  TH1F *h_muLowerPlusValidHits = new TH1F("LowerAntiMuonValidHits","LowerAntiMuonValidHits", 40, 0, 40);
  TH1F *h_muMinusMatchedMuonStations = new TH1F("MuonMatchedMuonStations","MuonMatchedMuonStations", 10, 0, 10);
  TH1F *h_muUpperMinusMatchedMuonStations = new TH1F("UpperMuonMatchedMuonStations","UpperMuonMatchedMuonStations", 10, 0, 10);
  TH1F *h_muPlusMatchedMuonStations = new TH1F("AntiMuonMatchedMuonStations","AntiMuonMatchedMuonStations", 10, 0, 10);
  TH1F *h_muUpperPlusMatchedMuonStations = new TH1F("UpperAntiMuonMatchedMuonStations","UpperAntiMuonMatchedMuonStations", 10, 0, 10);
  TH1F *h_muLowerMinusMatchedMuonStations = new TH1F("LowerMuonMatchedMuonStations","LowerMuonMatchedMuonStations", 10, 0, 10);
  TH1F *h_muLowerPlusMatchedMuonStations = new TH1F("LowerAntiMuonMatchedMuonStations","LowerAntiMuonMatchedMuonStations", 10, 0, 10);
  TH1F *h_muMinusPtError = new TH1F("MuonPtError","MuonPtError", 100, 0, 600);
  TH1F *h_muUpperMinusPtError = new TH1F("UpperMuonPtError","UpperMuonPtError", 100, 0, 600);
  TH1F *h_muPlusPtError = new TH1F("AntiMuonPtError","AntiMuonPtError", 100, 0, 600);
  TH1F *h_muUpperPlusPtError = new TH1F("UpperAntiMuonPtError","UpperAntiMuonPtError", 100, 0, 600);
  TH1F *h_muLowerMinusPtError = new TH1F("LowerMuonPtError","LowerMuonPtError", 100, 0, 600);
  TH1F *h_muLowerPlusPtError = new TH1F("LowerAntiMuonPtError","LowerAntiMuonPtError", 100, 0, 600);
  TH1F *h_muMinusDxyError = new TH1F("MuonDxyError","MuonDxyError", 100, 0, 200);
  TH1F *h_muUpperMinusDxyError = new TH1F("UpperMuonDxyError","UpperMuonDxyError", 100, 0, 200);
  TH1F *h_muPlusDxyError = new TH1F("AntiMuonDxyError","AntiMuonDxyError", 100, 0, 200);
  TH1F *h_muUpperPlusDxyError = new TH1F("UpperAntiMuonDxyError","UpperAntiMuonDxyError", 100, 0, 200);
  TH1F *h_muLowerMinusDxyError = new TH1F("LowerMuonDxyError","LowerMuonDxyError", 100, 0, 200);
  TH1F *h_muLowerPlusDxyError = new TH1F("LowerAntiMuonDxyError","LowerAntiMuonDxyError", 100, 0, 200);
  TH1F *h_muMinusDzError = new TH1F("MuonDzError","MuonDzError", 100, 0, 200);
  TH1F *h_muUpperMinusDzError = new TH1F("UpperMuonDzError","UpperMuonDzError", 100, 0, 200);
  TH1F *h_muPlusDzError = new TH1F("AntiMuonDzError","AntiMuonDzError", 100, 0, 200);
  TH1F *h_muUpperPlusDzError = new TH1F("UpperAntiMuonDzError","UpperAntiMuonDzError", 100, 0, 200);
  TH1F *h_muLowerMinusDzError = new TH1F("LowerMuonDzError","LowerMuonDzError", 100, 0, 200);
  TH1F *h_muLowerPlusDzError = new TH1F("LowerAntiMuonDzError","LowerAntiMuonDzError", 100, 0, 200);
  TH1F *h_muMinusPt = new TH1F("MuonPt","MuonPt", 400, 0, 2000);
  TH1F *h_muUpperMinusPt = new TH1F("UpperMuonPt","UpperMuonPt", 400, 0, 2000);
  TH1F *h_muPlusPt = new TH1F("AntiMuonPt","AntiMuonPt", 400, 0, 2000);
  TH1F *h_muUpperPlusPt = new TH1F("UpperAntiMuonPt","UpperAntiMuonPt", 400, 0, 2000);
  TH1F *h_muLowerMinusPt = new TH1F("LowerMuonPt","LowerMuonPt", 400, 0, 2000);
  TH1F *h_muLowerPlusPt = new TH1F("LowerAntiMuonPt","LowerAntiMuonPt", 400, 0, 2000);
  TH1F *h_muMinusEta = new TH1F("MuonEta","MuonEta", 50, -5, 5);
  TH1F *h_muUpperMinusEta = new TH1F("UpperMuonEta","UpperMuonEta", 50, -5, 5);
  TH1F *h_muPlusEta = new TH1F("AntiMuonEta","AntiMuonEta", 50, -5, 5);
  TH1F *h_muUpperPlusEta = new TH1F("UpperAntiMuonEta","UpperAntiMuonEta", 50, -5, 5);
  TH1F *h_muLowerMinusEta = new TH1F("LowerMuonEta","LowerMuonEta", 50, -5, 5);
  TH1F *h_muLowerPlusEta = new TH1F("LowerAntiMuonEta","LowerAntiMuonEta", 50, -5, 5);
  TH1F *h_muMinusPhi = new TH1F("MuonPhi","MuonPhi", 90, -4, 4);
  TH1F *h_muUpperMinusPhi = new TH1F("UpperMuonPhi","UpperMuonPhi", 90, -4, 4);
  TH1F *h_muPlusPhi = new TH1F("AntiMuonPhi","AntiMuonPhi", 90, -4, 4);
  TH1F *h_muUpperPlusPhi = new TH1F("UpperAntiMuonPhi","UpperAntiMuonPhi", 90, -4, 4);
  TH1F *h_muLowerMinusPhi = new TH1F("LowerMuonPhi","LowerMuonPhi", 90, -4, 4);
  TH1F *h_muLowerPlusPhi = new TH1F("LowerAntiMuonPhi","LowerAntiMuonPhi", 90, -4, 4);
  TH1F *h_muMinusTrackPt = new TH1F("MuonTrackPt","MuonTrackPt", 1000, 0, 2000);
  TH1F *h_muUpperMinusTrackPt = new TH1F("UpperMuonTrackPt","UpperMuonTrackPt", 400, 0, 2000);
  TH1F *h_muPlusTrackPt = new TH1F("AntiMuonTrackPt","AntiMuonTrackPt", 400, 0, 2000);
  TH1F *h_muUpperPlusTrackPt = new TH1F("UpperAntiMuonTrackPt","UpperAntiMuonTrackPt", 400, 0, 2000);
  TH1F *h_muLowerMinusTrackPt = new TH1F("LowerMuonTrackPt","LowerMuonTrackPt", 400, 0, 2000);
  TH1F *h_muLowerPlusTrackPt = new TH1F("LowerAntiMuonTrackPt","LowerAntiMuonTrackPt", 400, 0, 2000);
  TH1F *h_muMinusTrackEta = new TH1F("MuonTrackEta","MuonTrackEta", 50, -5, 5);
  TH1F *h_muUpperMinusTrackEta = new TH1F("UpperMuonTrackEta","UpperMuonTrackEta", 50, -5, 5);
  TH1F *h_muPlusTrackEta = new TH1F("AntiMuonTrackEta","AntiMuonTrackEta", 50, -5, 5);
  TH1F *h_muUpperPlusTrackEta = new TH1F("UpperAntiMuonTrackEta","UpperAntiMuonTrackEta", 50, -5, 5);
  TH1F *h_muLowerMinusTrackEta = new TH1F("LowerMuonTrackEta","LowerMuonTrackEta", 50, -5, 5);
  TH1F *h_muLowerPlusTrackEta = new TH1F("LowerAntiMuonTrackEta","LowerAntiMuonTrackEta", 50, -5, 5);
  TH1F *h_muMinusTrackPhi = new TH1F("MuonTrackPhi","MuonTrackPhi", 90, -4, 4);
  TH1F *h_muUpperMinusTrackPhi = new TH1F("UpperMuonTrackPhi","UpperMuonTrackPhi", 90, -4, 4);
  TH1F *h_muPlusTrackPhi = new TH1F("AntiMuonTrackPhi","AntiMuonTrackPhi", 90, -4, 4);
  TH1F *h_muUpperPlusTrackPhi = new TH1F("UpperAntiMuonTrackPhi","UpperAntiMuonTrackPhi", 90, -4, 4);
  TH1F *h_muLowerMinusTrackPhi = new TH1F("LowerMuonTrackPhi","LowerMuonTrackPhi", 90, -4, 4);
  TH1F *h_muLowerPlusTrackPhi = new TH1F("LowerAntiMuonTrackPhi","LowerAntiMuonTrackPhi", 90, -4, 4);
  TH1F *h_muMinusTrackLayersWithMeasurement = new TH1F("MuonTrackLayersWithMeasurement","MuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_muUpperMinusTrackLayersWithMeasurement = new TH1F("UpperMuonTrackLayersWithMeasurement","UpperMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_muPlusTrackLayersWithMeasurement = new TH1F("AntiMuonTrackLayersWithMeasurement","AntiMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_muUpperPlusTrackLayersWithMeasurement = new TH1F("UpperAntiMuonTrackLayersWithMeasurement","UpperAntiMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_muLowerMinusTrackLayersWithMeasurement = new TH1F("LowerMuonTrackLayersWithMeasurement","LowerMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_muLowerPlusTrackLayersWithMeasurement = new TH1F("LowerAntiMuonTrackLayersWithMeasurement","LowerAntiMuonTrackLayersWithMeasurement", 100, 0, 50);
  TH1F *h_muMinusCurvePlusBias[nBiasBins];
  TH1F *h_muMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muMinusCurvePlusBias[i] = new TH1F(TString("muMinusCurvePlusBias" + name.str()), TString("muMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_muMinusCurveMinusBias[i] = new TH1F(TString("muMinusCurveMinusBias" + name.str()),TString("muMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_muUpperMinusCurvePlusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muUpperMinusCurvePlusBias[i] = new TH1F(TString("muUpperMinusCurvePlusBias" + name.str()), TString("muUpperMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_muUpperMinusCurveMinusBias[i] = new TH1F(TString("muUpperMinusCurveMinusBias" + name.str()),TString("muUpperMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_muLowerMinusCurvePlusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muLowerMinusCurvePlusBias[i] = new TH1F(TString("muLowerMinusCurvePlusBias" + name.str()), TString("muLowerMinusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_muLowerMinusCurveMinusBias[i] = new TH1F(TString("muLowerMinusCurveMinusBias" + name.str()),TString("muLowerMinusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_muPlusCurvePlusBias[nBiasBins];
  TH1F *h_muPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muPlusCurvePlusBias[i] = new TH1F(TString("muPlusCurvePlusBias" + name.str()), TString("muPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_muPlusCurveMinusBias[i] = new TH1F(TString("muPlusCurveMinusBias" + name.str()),TString("muPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_muUpperPlusCurvePlusBias[nBiasBins];
  TH1F *h_muUpperPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muUpperPlusCurvePlusBias[i] = new TH1F(TString("muUpperPlusCurvePlusBias" + name.str()), TString("muUpperPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_muUpperPlusCurveMinusBias[i] = new TH1F(TString("muUpperPlusCurveMinusBias" + name.str()),TString("muUpperPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }
  TH1F *h_muLowerPlusCurvePlusBias[nBiasBins];
  TH1F *h_muLowerPlusCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muLowerPlusCurvePlusBias[i] = new TH1F(TString("muLowerPlusCurvePlusBias" + name.str()), TString("muLowerPlusCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_muLowerPlusCurveMinusBias[i] = new TH1F(TString("muLowerPlusCurveMinusBias" + name.str()),TString("muLowerPlusCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }




 

 
  TH1F *h_upperChi2 = new TH1F("upperChi2","upperChi2", 100, 0, 50);
  TH1F *h_upperNdof = new TH1F("upperNdof","upperNdof", 100, 0, 50);
  TH1F *h_upperCharge = new TH1F("upperCharge","upperCharge", 10, -2, 2);
  TH1F *h_upperCurve = new TH1F("upperCurve","upperCurve", 100, -1, 1);
  TH1F *h_upperDxy = new TH1F("upperDxy","upperDxy", 100, -300, 300);
  TH1F *h_upperDz = new TH1F("upperDz","upperDz", 100, -500, 500);
  TH1F *h_upperPixelHits = new TH1F("upperPixelHits","upperPixelHits", 20, 0, 20);
  TH1F *h_upperTrackerHits = new TH1F("upperTrackerHits","upperTrackerHits", 30, 0, 30);
  TH1F *h_upperMuonStationHits = new TH1F("upperMuonStationHits","upperMuonStationHits", 10, 0, 10);
  TH1F *h_upperValidHits = new TH1F("upperValidHits","upperValidHits", 40, 0, 40);
  TH1F *h_upperMatchedMuonStations = new TH1F("upperMatchedMuonStation","upperMatchedMuonStations", 10, 0, 10);
  TH1F *h_upperPtError = new TH1F("upperPtError","upperPtError", 100, 0, 600);
  TH1F *h_upperDxyError = new TH1F("upperDxyError","upperDxyError", 100, 0, 200);
  TH1F *h_upperDzError = new TH1F("upperDzError","upperDzError", 100, 0, 200);
  TH1F *h_upperPt = new TH1F("upperPt","upperPt", 400, 0, 2000);
  TH1F *h_upperEta = new TH1F("upperEta","upperEta", 50, -5, 5);
  TH1F *h_upperPhi = new TH1F("upperPhi","upperPhi", 90, -4, 4);
  TH1F *h_upperTrackPt = new TH1F("upperTrackPt","upperTrackPt", 400, 0, 2000);
  TH1F *h_upperTrackEta = new TH1F("upperTrackEta","upperTrackEta", 50, -5, 5);
  TH1F *h_upperTrackPhi = new TH1F("upperTrackPhi","upperTrackPhi", 90, -4, 4);
  TH1F *h_upperTrackerLayersWithMeasurement = new TH1F("upperTrackerLayersWithMeasurement","upperTrackerLayersWithMeasurement", 100, 0, 50);
  TH1F *h_upperCurvePlusBias[nBiasBins];
  TH1F *h_upperCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_upperCurvePlusBias[i] = new TH1F(TString("upperCurvePlusBias" + name.str()), TString("upperCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_upperCurveMinusBias[i] = new TH1F(TString("upperCurveMinusBias" + name.str()),TString("upperCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  } 


  TH1F *h_lowerChi2 = new TH1F("lowerChi2","lowerChi2", 100, 0, 50);
  TH1F *h_lowerNdof = new TH1F("lowerNdof","lowerNdof", 100, 0, 50);
  TH1F *h_lowerCharge = new TH1F("lowerCharge","lowerCharge", 10, -2, 2);
  TH1F *h_lowerCurve = new TH1F("lowerCurve","lowerCurve",100 , -1, 1);
  TH1F *h_lowerDxy = new TH1F("lowerDxy","lowerDxy", 100, -300, 300);
  TH1F *h_lowerDz = new TH1F("lowerDz","lowerDz", 100, -500, 500);
  TH1F *h_lowerPixelHits = new TH1F("lowerPixelHits","lowerPixelHits", 20, 0, 20);
  TH1F *h_lowerTrackerHits = new TH1F("lowerTrackerHits","lowerTrackerHits", 30, 0, 30);
  TH1F *h_lowerMuonStationHits = new TH1F("lowerMuonStationHits","lowerMuonStationHits", 10, 0, 10);
  TH1F *h_lowerValidHits = new TH1F("lowerValidHits","lowerValidHits", 40, 0, 40);
  TH1F *h_lowerMatchedMuonStations = new TH1F("lowerMatchedMuonStation","lowerMatchedMuonStations", 10, 0, 10);
  TH1F *h_lowerPtError = new TH1F("lowerPtError","lowerPtError", 100, 0, 600);
  TH1F *h_lowerDxyError = new TH1F("lowerDxyError","lowerDxyError", 100, 0, 200);
  TH1F *h_lowerDzError = new TH1F("lowerDzError","lowerDzError", 100, 0, 200);
  TH1F *h_lowerPt = new TH1F("lowerPt","lowerPt", 400, 0, 2000);
  TH1F *h_lowerEta = new TH1F("lowerEta","lowerEta", 50, -5, 5);
  TH1F *h_lowerPhi = new TH1F("lowerPhi","lowerPhi", 90, -4, 4);
  TH1F *h_lowerTrackPt = new TH1F("lowerTrackPt","lowerTrackPt", 400, 0, 2000);
  TH1F *h_lowerTrackEta = new TH1F("lowerTrackEta","lowerTrackEta", 50, -5, 5);
  TH1F *h_lowerTrackPhi = new TH1F("lowerTrackPhi","lowerTrackPhi", 90, -4, 4);
  TH1F *h_lowerTrackerLayersWithMeasurement = new TH1F("lowerTrackerLayersWithMeasurement","lowerTrackerLayersWithMeasurement", 100, 0, 50);
  TH1F *h_lowerCurvePlusBias[nBiasBins];
  TH1F *h_lowerCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_lowerCurvePlusBias[i] = new TH1F(TString("lowerCurvePlusBias" + name.str()), TString("lowerCurvePlusBias" + name.str()), 200, -0.05, 0.05);
    h_lowerCurveMinusBias[i] = new TH1F(TString("lowerCurveMinusBias" + name.str()),TString("lowerCurveMinusBias" + name.str()), 200, -0.05, 0.05);  
    //    std::cout << "\nCreated bias empty histograms number: " << i << std::endl;
  }   
  


 
  /*TFile *f = TFile::Open(TString(file1));
    if (f == 0){
    std::cout << "Error: cannot open file 1 \n";
    return;
    }
  */
  
 
  
  std::cout << "\nCreating upper Muon TTreeReaderValues\n";
  TTreeReaderValue<Double_t> upTrackerPt(trackReader,     "upperMuon_trackPt"        );
  TTreeReaderValue<Double_t> upTrackerDxy(trackReader,    "upperMuon_dxy"            );
  TTreeReaderValue<Double_t> upTrackerDz(trackReader,     "upperMuon_dz"             );
  TTreeReaderValue<Int_t>    upTrackerPhits(trackReader,  "upperMuon_pixelHits"      );
  TTreeReaderValue<Int_t>    upTrackerCharge(trackReader, "upperMuon_charge"         );
  TTreeReaderValue<Int_t>    upTrackerThits(trackReader,  "upperMuon_trackerHits"    );
  TTreeReaderValue<Int_t>    upTrackerMhits(trackReader,  "upperMuon_muonStationHits");
  TTreeReaderValue<Double_t> upTrackerChi2(trackReader,   "upperMuon_chi2"           );
  TTreeReaderValue<Int_t>    upTrackerNdof(trackReader,   "upperMuon_ndof"           );
  TTreeReaderValue<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > upTrackerMuonP4(trackReader,"upperMuon_P4");
  TTreeReaderValue<Int_t>    upTrackerMatchedMuonStations(trackReader,"upperMuon_numberOfMatchedStations");
  TTreeReaderValue<Int_t>    upTrackerValidHits(trackReader, "upperMuon_numberOfValidHits");
  TTreeReaderValue<Double_t> upTrackerDxyError(trackReader,  "upperMuon_dxyError");
  TTreeReaderValue<Double_t> upTrackerDzError(trackReader,   "upperMuon_dzError" );
  TTreeReaderValue<Double_t> upTrackerPtError(trackReader,   "upperMuon_ptError" );
  TTreeReaderValue<Int_t>    upTrackerLayersWithMeasurement(trackReader,"upperMuon_trackerLayersWithMeasurement");
  //TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > upTrackerTrack(trackReader,     "upperMuon_trackVec");
  //TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > upTrackerTrack(trackReader,     "upperMuon_trackVec");


  std::cout << "\nCreating lower Muon TTreeReaderValues\n";
  TTreeReaderValue<Double_t> lowTrackerPt(trackReader,     "lowerMuon_trackPt"        );
  TTreeReaderValue<Double_t> lowTrackerDxy(trackReader,    "lowerMuon_dxy"            );
  TTreeReaderValue<Double_t> lowTrackerDz(trackReader,     "lowerMuon_dz"             );
  TTreeReaderValue<Int_t>    lowTrackerPhits(trackReader,  "lowerMuon_pixelHits"      );
  TTreeReaderValue<Int_t>    lowTrackerCharge(trackReader, "lowerMuon_charge"         );
  TTreeReaderValue<Int_t>    lowTrackerThits(trackReader,  "lowerMuon_trackerHits"    );
  TTreeReaderValue<Int_t>    lowTrackerMhits(trackReader,  "lowerMuon_muonStationHits");
  TTreeReaderValue<Double_t> lowTrackerChi2(trackReader,   "lowerMuon_chi2"           );
  TTreeReaderValue<Int_t>    lowTrackerNdof(trackReader,   "lowerMuon_ndof"           );
  TTreeReaderValue<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > lowTrackerMuonP4(trackReader,"lowerMuon_P4");
  TTreeReaderValue<Int_t>    lowTrackerMatchedMuonStations(trackReader,  "lowerMuon_numberOfMatchedStations"     );
  TTreeReaderValue<Int_t>    lowTrackerValidHits(trackReader,            "lowerMuon_numberOfValidHits"           );
  TTreeReaderValue<Double_t> lowTrackerDxyError(trackReader,             "lowerMuon_dxyError"                    );
  TTreeReaderValue<Double_t> lowTrackerDzError(trackReader,              "lowerMuon_dzError"                     );
  TTreeReaderValue<Double_t> lowTrackerPtError(trackReader,              "lowerMuon_ptError"                     );
  TTreeReaderValue<Int_t>    lowTrackerLayersWithMeasurement(trackReader,"lowerMuon_trackerLayersWithMeasurement");
  //TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > lowTrackerTrack(trackReader,"lowerMuon_trackVec");

  
  //note, the old way would only loop to the maximum number of events in the smallest file
  // this checks that at least one of the files still has events to process,
  // but only fills histograms when there are still events
  //  bool fchk = trackReader.Next();
  // bool gchk = newReader.Next();
  //  bool echk = nextReader.Next();
  std::cout << "\nMade it to Histogramming!\n";
  int j = 0;
  bool debug = false;
  while (trackReader.Next()){
    if (debug)
      std::cout << "\nMade it into the first loop\n" << std::endl;
    g->cd();
    
    if(*upTrackerChi2 > -1000){
      double upperCpT = *upTrackerCharge / (*upTrackerMuonP4).Pt();
      h_upperChi2->Fill(*upTrackerChi2);
      h_upperNdof->Fill(*upTrackerNdof);
      h_upperPt->Fill((*upTrackerMuonP4).Pt());
      h_upperEta->Fill((*upTrackerMuonP4).Eta());
      h_upperPhi->Fill((*upTrackerMuonP4).Phi());
      if((j % 100) == 0)  std::cout << "pt = " << upTrackerMuonP4->pt()
		<< " - eta = "  << upTrackerMuonP4->eta()
		<< " - phi = "  << upTrackerMuonP4->phi()
		<< std::endl;

      //      std::cout	<< "pt = " << sqrt(upTrackerTrack->Perp2())
      //	<< " - eta = "  << upTrackerTrack->Eta()
      //	<< " - phi = "  << upTrackerTrack->Phi()
      //	<< std::endl;
      h_upperCharge->Fill(*upTrackerCharge);
      h_upperCurve->Fill(upperCpT);
      h_upperDxy->Fill(*upTrackerDxy);
      h_upperDz->Fill(*upTrackerDz);
      h_upperPixelHits->Fill(*upTrackerPhits);
      h_upperTrackerHits->Fill(*upTrackerThits);
      h_upperMuonStationHits-> Fill(*upTrackerMhits);
      h_upperValidHits->Fill(*upTrackerValidHits);
      h_upperMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
      h_upperPtError->Fill(*upTrackerPtError);
      h_upperDxyError->Fill(*upTrackerDxyError);
      h_upperDzError->Fill(*upTrackerDzError);
      h_upperTrackPt->Fill(*upTrackerPt);
      //h_upperTrackEta->Fill((*upTrackerTrack).Eta());
      //h_upperTrackPhi->Fill((*upTrackerTrack).Phi());
      h_upperTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
      if(*upTrackerCharge < 0){
	h_muMinusChi2->Fill(*upTrackerChi2);
	h_muUpperMinusChi2->Fill(*upTrackerChi2);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusChi2->Fill(*upTrackerChi2);
	h_muUpperPlusChi2->Fill(*upTrackerChi2);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusNdof->Fill(*upTrackerNdof);
	h_muUpperMinusNdof->Fill(*upTrackerNdof);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusNdof->Fill(*upTrackerNdof);
	h_muUpperPlusNdof->Fill(*upTrackerNdof);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusCharge->Fill(*upTrackerCharge);
	h_muUpperMinusCharge->Fill(*upTrackerCharge);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusCharge->Fill(*upTrackerCharge);
	h_muUpperPlusCharge->Fill(*upTrackerCharge);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusCurve->Fill(upperCpT);
	h_muUpperMinusCurve->Fill(upperCpT);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusCurve->Fill(upperCpT);
	h_muUpperPlusCurve->Fill(upperCpT);
	
      }

      if(*upTrackerCharge < 0){
	h_muMinusDxy->Fill(*upTrackerDxy);
	h_muUpperMinusDxy->Fill(*upTrackerDxy);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusDxy->Fill(*upTrackerDxy);
	h_muUpperPlusDxy->Fill(*upTrackerDxy);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusDz->Fill(*upTrackerDz);
	h_muUpperMinusDz->Fill(*upTrackerDz);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusDz->Fill(*upTrackerDz);
	h_muUpperPlusDz->Fill(*upTrackerDz);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusPt->Fill((*upTrackerMuonP4).Pt());
	h_muUpperMinusPt->Fill((*upTrackerMuonP4).Pt());
      }
      else if(*upTrackerCharge > 0){

	h_muPlusPt->Fill((*upTrackerMuonP4).Pt());
	h_muUpperPlusPt->Fill((*upTrackerMuonP4).Pt());

      }

      if(*upTrackerCharge < 0){
	h_muMinusEta->Fill((*upTrackerMuonP4).Eta());
	h_muUpperMinusEta->Fill((*upTrackerMuonP4).Eta());
      }
      else if(*upTrackerCharge > 0){

	h_muPlusEta->Fill((*upTrackerMuonP4).Eta());
	h_muUpperPlusEta->Fill((*upTrackerMuonP4).Eta());

      }

      if(*upTrackerCharge < 0){
	h_muMinusPhi->Fill((*upTrackerMuonP4).Phi());
	h_muUpperMinusPhi->Fill((*upTrackerMuonP4).Phi());
      }
      else if(*upTrackerCharge > 0){

	h_muPlusPhi->Fill((*upTrackerMuonP4).Phi());
	h_muUpperPlusPhi->Fill((*upTrackerMuonP4).Phi());

      }
      if(*upTrackerCharge < 0){
	h_muMinusPixelHits->Fill(*upTrackerPhits);
	h_muUpperMinusPixelHits->Fill(*upTrackerPhits);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusPixelHits->Fill(*upTrackerPhits);
	h_muUpperPlusPixelHits->Fill(*upTrackerPhits);
	
      }
      
      if(*upTrackerCharge < 0){
	h_muMinusTrackerHits->Fill(*upTrackerThits);
	h_muUpperMinusTrackerHits->Fill(*upTrackerThits);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusTrackerHits->Fill(*upTrackerThits);
	h_muUpperPlusTrackerHits->Fill(*upTrackerThits);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusMuonStationHits->Fill(*upTrackerMhits);
	h_muUpperMinusMuonStationHits->Fill(*upTrackerMhits);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusMuonStationHits->Fill(*upTrackerMhits);
	h_muUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusValidHits->Fill(*upTrackerValidHits);
	h_muUpperMinusValidHits->Fill(*upTrackerValidHits);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusValidHits->Fill(*upTrackerValidHits);
	h_muUpperPlusValidHits->Fill(*upTrackerValidHits);
	
      }

      if(*upTrackerCharge < 0){
	h_muMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	h_muUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	h_muUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusPtError->Fill(*upTrackerPtError);
	h_muUpperMinusPtError->Fill(*upTrackerPtError);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusPtError->Fill(*upTrackerPtError);
	h_muUpperPlusPtError->Fill(*upTrackerPtError);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusDxyError->Fill(*upTrackerDxyError);
	h_muUpperMinusDxyError->Fill(*upTrackerDxyError);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusDxyError->Fill(*upTrackerDxyError);
	h_muUpperPlusDxyError->Fill(*upTrackerDxyError);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusDzError->Fill(*upTrackerDzError);
	h_muUpperMinusDzError->Fill(*upTrackerDzError);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusDzError->Fill(*upTrackerDzError);
	h_muUpperPlusDzError->Fill(*upTrackerDzError);
	
      }
      if(*upTrackerCharge < 0){
	h_muMinusTrackPt->Fill(*upTrackerPt);
	h_muUpperMinusTrackPt->Fill(*upTrackerPt);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusTrackPt->Fill(*upTrackerPt);
	h_muUpperPlusTrackPt->Fill(*upTrackerPt);
	
      }
      /* if(*upTrackerCharge < 0){
	 h_muMinusTrackEta->Fill((*upTrackerTrack).Eta());
	 h_muUpperMinusTrackEta->Fill((*upTrackerTrack).Eta());
	 }
	 else if(*upTrackerCharge > 0){

	 h_muPlusTrackEta->Fill((*upTrackerTrack).Eta());
	 h_muUpperPlusTrackEta->Fill((*upTrackerTrack).Eta());

	 }


	 if(*upTrackerCharge < 0){
	 h_muMinusTrackPhi->Fill((*upTrackerTrack).Phi());
	 h_muUpperMinusTrackPhi->Fill((*upTrackerTrack).Phi());
	 }
	 else if(*upTrackerCharge > 0){

	 h_muPlusTrackPhi->Fill((*upTrackerTrack).Phi());
	 h_muUpperPlusTrackPhi->Fill((*upTrackerTrack).Phi());

	 }
      */ 
      if(*upTrackerCharge < 0){
	h_muMinusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	h_muUpperMinusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
      }
      else if(*upTrackerCharge > 0){
	
	h_muPlusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	h_muUpperPlusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	
      }
      for(int i = 0; i < nBiasBins; ++i){

	h_upperCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	h_upperCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	if (debug)
	  std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
      }

      if(*upTrackerCharge <  0){
	for(int i = 0; i < nBiasBins; ++i){

	  h_muMinusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muMinusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	  h_muUpperMinusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muUpperMinusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	

	  if (debug)
	    std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	}

      }

      else if(*upTrackerCharge > 0){
	for(int i = 0; i < nBiasBins; ++i){

	  h_muPlusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muPlusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	  h_muUpperPlusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muUpperPlusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	

	  if (debug)
	    std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	}

      }

      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) ){
	

	if(*upTrackerCharge < 0){
	  h_looseMuMinusChi2->Fill(*upTrackerChi2);
	  h_looseMuUpperMinusChi2->Fill(*upTrackerChi2);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusChi2->Fill(*upTrackerChi2);
	  h_looseMuUpperPlusChi2->Fill(*upTrackerChi2);
	
	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusNdof->Fill(*upTrackerNdof);
	  h_looseMuUpperMinusNdof->Fill(*upTrackerNdof);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusNdof->Fill(*upTrackerNdof);
	  h_looseMuUpperPlusNdof->Fill(*upTrackerNdof);
	
	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusCharge->Fill(*upTrackerCharge);
	  h_looseMuUpperMinusCharge->Fill(*upTrackerCharge);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusCharge->Fill(*upTrackerCharge);
	  h_looseMuUpperPlusCharge->Fill(*upTrackerCharge);
	
	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusCurve->Fill(upperCpT);
	  h_looseMuUpperMinusCurve->Fill(upperCpT);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusCurve->Fill(upperCpT);
	  h_looseMuUpperPlusCurve->Fill(upperCpT);
	
	}

	if(*upTrackerCharge < 0){
	  h_looseMuMinusDxy->Fill(*upTrackerDxy);
	  h_looseMuUpperMinusDxy->Fill(*upTrackerDxy);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusDxy->Fill(*upTrackerDxy);
	  h_looseMuUpperPlusDxy->Fill(*upTrackerDxy);
	
	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusDz->Fill(*upTrackerDz);
	  h_looseMuUpperMinusDz->Fill(*upTrackerDz);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusDz->Fill(*upTrackerDz);
	  h_looseMuUpperPlusDz->Fill(*upTrackerDz);
	
	}
	/*  

	    if(*upTrackerCharge < 0){
	    h_looseMuMinusEta->Fill((*upTrackerMuonP4).Eta());
	    h_looseMuUpperMinusEta->Fill((*upTrackerMuonP4).Eta());
	    }
	    else if(*upTrackerCharge > 0){

	    h_looseMuPlusEta->Fill((*upTrackerMuonP4).Eta());
	    h_looseMuUpperPlusEta->Fill((*upTrackerMuonP4).Eta());

	    }

	    if(*upTrackerCharge < 0){
	    h_looseMuMinusPhi->Fill((*upTrackerMuonP4).Phi());
	    h_looseMuUpperMinusPhi->Fill((*upTrackerMuonP4).Phi());
	    }
	    else if(*upTrackerCharge > 0){

	    h_looseMuPlusPhi->Fill((*upTrackerMuonP4).Phi());
	    h_looseMuUpperPlusPhi->Fill((*upTrackerMuonP4).Phi());

	    }
	*/ 
	if(*upTrackerCharge < 0){
	  h_looseMuMinusPixelHits->Fill(*upTrackerPhits);
	  h_looseMuUpperMinusPixelHits->Fill(*upTrackerPhits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusPixelHits->Fill(*upTrackerPhits);
	  h_looseMuUpperPlusPixelHits->Fill(*upTrackerPhits);
	
	}
      
	if(*upTrackerCharge < 0){
	  h_looseMuMinusTrackerHits->Fill(*upTrackerThits);
	  h_looseMuUpperMinusTrackerHits->Fill(*upTrackerThits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusTrackerHits->Fill(*upTrackerThits);
	  h_looseMuUpperPlusTrackerHits->Fill(*upTrackerThits);
	
	}
 
	if(*upTrackerCharge < 0){
	  h_looseMuMinusValidHits->Fill(*upTrackerValidHits);
	  h_looseMuUpperMinusValidHits->Fill(*upTrackerValidHits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusValidHits->Fill(*upTrackerValidHits);
	  h_looseMuUpperPlusValidHits->Fill(*upTrackerValidHits);
	
	}

 
 
	if(*upTrackerCharge < 0){
	  h_looseMuMinusDxyError->Fill(*upTrackerDxyError);
	  h_looseMuUpperMinusDxyError->Fill(*upTrackerDxyError);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusDxyError->Fill(*upTrackerDxyError);
	  h_looseMuUpperPlusDxyError->Fill(*upTrackerDxyError);
	
	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusDzError->Fill(*upTrackerDzError);
	  h_looseMuUpperMinusDzError->Fill(*upTrackerDzError);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusDzError->Fill(*upTrackerDzError);
	  h_looseMuUpperPlusDzError->Fill(*upTrackerDzError);
	
	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusTrackPt->Fill(*upTrackerPt);
	  h_looseMuUpperMinusTrackPt->Fill(*upTrackerPt);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusTrackPt->Fill(*upTrackerPt);
	  h_looseMuUpperPlusTrackPt->Fill(*upTrackerPt);
	
	}
	/* if(*upTrackerCharge < 0){
	   h_looseMuMinusTrackEta->Fill((*upTrackerTrack).Eta());
	   h_looseMuUpperMinusTrackEta->Fill((*upTrackerTrack).Eta());
	   }
	   else if(*upTrackerCharge > 0){

	   h_looseMuPlusTrackEta->Fill((*upTrackerTrack).Eta());
	   h_looseMuUpperPlusTrackEta->Fill((*upTrackerTrack).Eta());

	   }


	   if(*upTrackerCharge < 0){
	   h_looseMuMinusTrackPhi->Fill((*upTrackerTrack).Phi());
	   h_looseMuUpperMinusTrackPhi->Fill((*upTrackerTrack).Phi());
	   }
	   else if(*upTrackerCharge > 0){

	   h_looseMuPlusTrackPhi->Fill((*upTrackerTrack).Phi());
	   h_looseMuUpperPlusTrackPhi->Fill((*upTrackerTrack).Phi());

	   }
	*/
	if(*upTrackerCharge <  0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_looseMuMinusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuMinusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_looseMuUpperMinusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuUpperMinusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}

	else if(*upTrackerCharge > 0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_looseMuPlusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuPlusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_looseMuUpperPlusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuUpperPlusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}
      }

      if((*upTrackerMatchedMuonStations > 1) /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) ){

	if(*upTrackerCharge < 0){
	  h_looseMuMinusMuonStationHits->Fill(*upTrackerMhits);
	  h_looseMuUpperMinusMuonStationHits->Fill(*upTrackerMhits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusMuonStationHits->Fill(*upTrackerMhits);
	  h_looseMuUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	
	}
      }

      if((*upTrackerMhits > 0) /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) ){
	if(*upTrackerCharge < 0){
	  h_looseMuMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	  h_looseMuUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	  h_looseMuUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	
	}

      }


      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) && (*upTrackerLayersWithMeasurement > 5) ){
	if(*upTrackerCharge < 0){
	  h_looseMuMinusPt->Fill((*upTrackerMuonP4).Pt());
	  h_looseMuUpperMinusPt->Fill((*upTrackerMuonP4).Pt());
	}
	else if(*upTrackerCharge > 0){

	  h_looseMuPlusPt->Fill((*upTrackerMuonP4).Pt());
	  h_looseMuUpperPlusPt->Fill((*upTrackerMuonP4).Pt());

	}
	if(*upTrackerCharge < 0){
	  h_looseMuMinusPtError->Fill(*upTrackerPtError);
	  h_looseMuUpperMinusPtError->Fill(*upTrackerPtError);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusPtError->Fill(*upTrackerPtError);
	  h_looseMuUpperPlusPtError->Fill(*upTrackerPtError);
	
	}



      }


      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/){
	if(*upTrackerCharge < 0){
	  h_looseMuMinusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	  h_looseMuUpperMinusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	}
	else if(*upTrackerCharge > 0){
	
	  h_looseMuPlusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	  h_looseMuUpperPlusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	
	}
      }








      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) && (*upTrackerPt > 150)  /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) && (*upTrackerDxy < 0.2) && (*upTrackerDz < 0.5)){
	

	if(*upTrackerCharge < 0){
	  h_tightMuMinusChi2->Fill(*upTrackerChi2);
	  h_tightMuUpperMinusChi2->Fill(*upTrackerChi2);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusChi2->Fill(*upTrackerChi2);
	  h_tightMuUpperPlusChi2->Fill(*upTrackerChi2);
	
	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusNdof->Fill(*upTrackerNdof);
	  h_tightMuUpperMinusNdof->Fill(*upTrackerNdof);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusNdof->Fill(*upTrackerNdof);
	  h_tightMuUpperPlusNdof->Fill(*upTrackerNdof);
	
	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusCharge->Fill(*upTrackerCharge);
	  h_tightMuUpperMinusCharge->Fill(*upTrackerCharge);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusCharge->Fill(*upTrackerCharge);
	  h_tightMuUpperPlusCharge->Fill(*upTrackerCharge);
	
	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusCurve->Fill(upperCpT);
	  h_tightMuUpperMinusCurve->Fill(upperCpT);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusCurve->Fill(upperCpT);
	  h_tightMuUpperPlusCurve->Fill(upperCpT);
	
	}


	if(*upTrackerCharge < 0){
	  h_tightMuMinusEta->Fill((*upTrackerMuonP4).Eta());
	  h_tightMuUpperMinusEta->Fill((*upTrackerMuonP4).Eta());
	}
	else if(*upTrackerCharge > 0){

	  h_tightMuPlusEta->Fill((*upTrackerMuonP4).Eta());
	  h_tightMuUpperPlusEta->Fill((*upTrackerMuonP4).Eta());

	}

	if(*upTrackerCharge < 0){
	  h_tightMuMinusPhi->Fill((*upTrackerMuonP4).Phi());
	  h_tightMuUpperMinusPhi->Fill((*upTrackerMuonP4).Phi());
	}
	else if(*upTrackerCharge > 0){

	  h_tightMuPlusPhi->Fill((*upTrackerMuonP4).Phi());
	  h_tightMuUpperPlusPhi->Fill((*upTrackerMuonP4).Phi());

	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusPixelHits->Fill(*upTrackerPhits);
	  h_tightMuUpperMinusPixelHits->Fill(*upTrackerPhits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusPixelHits->Fill(*upTrackerPhits);
	  h_tightMuUpperPlusPixelHits->Fill(*upTrackerPhits);
	
	}
      
	if(*upTrackerCharge < 0){
	  h_tightMuMinusTrackerHits->Fill(*upTrackerThits);
	  h_tightMuUpperMinusTrackerHits->Fill(*upTrackerThits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusTrackerHits->Fill(*upTrackerThits);
	  h_tightMuUpperPlusTrackerHits->Fill(*upTrackerThits);
	
	}
 
	if(*upTrackerCharge < 0){
	  h_tightMuMinusValidHits->Fill(*upTrackerValidHits);
	  h_tightMuUpperMinusValidHits->Fill(*upTrackerValidHits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusValidHits->Fill(*upTrackerValidHits);
	  h_tightMuUpperPlusValidHits->Fill(*upTrackerValidHits);
	
	}

 
 
	if(*upTrackerCharge < 0){
	  h_tightMuMinusDxyError->Fill(*upTrackerDxyError);
	  h_tightMuUpperMinusDxyError->Fill(*upTrackerDxyError);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusDxyError->Fill(*upTrackerDxyError);
	  h_tightMuUpperPlusDxyError->Fill(*upTrackerDxyError);
	
	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusDzError->Fill(*upTrackerDzError);
	  h_tightMuUpperMinusDzError->Fill(*upTrackerDzError);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusDzError->Fill(*upTrackerDzError);
	  h_tightMuUpperPlusDzError->Fill(*upTrackerDzError);
	
	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusTrackPt->Fill(*upTrackerPt);
	  h_tightMuUpperMinusTrackPt->Fill(*upTrackerPt);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusTrackPt->Fill(*upTrackerPt);
	  h_tightMuUpperPlusTrackPt->Fill(*upTrackerPt);
	
	}
	/*
	  if(*upTrackerCharge < 0){
	  h_tightMuMinusTrackEta->Fill((*upTrackerTrack).Eta());
	  h_tightMuUpperMinusTrackEta->Fill((*upTrackerTrack).Eta());
	  }
	  else if(*upTrackerCharge > 0){

	  h_tightMuPlusTrackEta->Fill((*upTrackerTrack).Eta());
	  h_tightMuUpperPlusTrackEta->Fill((*upTrackerTrack).Eta());

	  }


	  if(*upTrackerCharge < 0){
	  h_tightMuMinusTrackPhi->Fill((*upTrackerTrack).Phi());
	  h_tightMuUpperMinusTrackPhi->Fill((*upTrackerTrack).Phi());
	  }
	  else if(*upTrackerCharge > 0){

	  h_tightMuPlusTrackPhi->Fill((*upTrackerTrack).Phi());
	  h_tightMuUpperPlusTrackPhi->Fill((*upTrackerTrack).Phi());

	  }
 
	*/
	if(*upTrackerCharge <  0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_tightMuMinusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuMinusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_tightMuUpperMinusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuUpperMinusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}

	else if(*upTrackerCharge > 0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_tightMuPlusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuPlusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_tightMuUpperPlusCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuUpperPlusCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}

      }

      if((*upTrackerMatchedMuonStations > 1) && (*upTrackerPt > 150)  /*&& ((*upTrackerPtError / *upTrackerPt) < 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) && (*upTrackerDxy < 0.2) && (*upTrackerDz < 0.5) ){

	if(*upTrackerCharge < 0){
	  h_tightMuMinusMuonStationHits->Fill(*upTrackerMhits);
	  h_tightMuUpperMinusMuonStationHits->Fill(*upTrackerMhits);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusMuonStationHits->Fill(*upTrackerMhits);
	  h_tightMuUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	
	}
      }

      if((*upTrackerMhits > 0) && (*upTrackerPt > 150) /*&& ((*upTrackerPtError / *upTrackerPt) < 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) && (*upTrackerDxy < 0.2) && (*upTrackerDz < 0.5) ){
	if(*upTrackerCharge < 0){
	  h_tightMuMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	  h_tightMuUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	  h_tightMuUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	
	}

      }


      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) && (*upTrackerLayersWithMeasurement > 5) && (*upTrackerDxy < 0.2) && (*upTrackerDz < 0.5)){
	if(*upTrackerCharge < 0){
	  h_tightMuMinusPt->Fill((*upTrackerMuonP4).Pt());
	  h_tightMuUpperMinusPt->Fill((*upTrackerMuonP4).Pt());
	}
	else if(*upTrackerCharge > 0){

	  h_tightMuPlusPt->Fill((*upTrackerMuonP4).Pt());
	  h_tightMuUpperPlusPt->Fill((*upTrackerMuonP4).Pt());

	}
	if(*upTrackerCharge < 0){
	  h_tightMuMinusPtError->Fill(*upTrackerPtError);
	  h_tightMuUpperMinusPtError->Fill(*upTrackerPtError);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusPtError->Fill(*upTrackerPtError);
	  h_tightMuUpperPlusPtError->Fill(*upTrackerPtError);
	
	}



      }


      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) && (*upTrackerPt > 150)  /*&& ((*upTrackerPtError / *upTrackerPt) < 0.3)*/ && (*upTrackerDxy < 0.2) && (*upTrackerDz < 0.5)){
	if(*upTrackerCharge < 0){
	  h_tightMuMinusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	  h_tightMuUpperMinusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	  h_tightMuUpperPlusTrackLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	
	}
      }


      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) && (*upTrackerPt > 150) /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) && (*upTrackerDz < 0.5)){
	if(*upTrackerCharge < 0){
	  h_tightMuMinusDxy->Fill(*upTrackerDxy);
	  h_tightMuUpperMinusDxy->Fill(*upTrackerDxy);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusDxy->Fill(*upTrackerDxy);
	  h_tightMuUpperPlusDxy->Fill(*upTrackerDxy);
	
	}
	
      }


      if((*upTrackerMhits > 0) && (*upTrackerMatchedMuonStations > 1) && (*upTrackerPt > 150) /*&& ((*upTrackerPtError / *upTrackerPt) > 0.3)*/ && (*upTrackerLayersWithMeasurement > 5) && (*upTrackerDxy < 0.2)){
	if(*upTrackerCharge < 0){
	  h_tightMuMinusDz->Fill(*upTrackerDz);
	  h_tightMuUpperMinusDz->Fill(*upTrackerDz);
	}
	else if(*upTrackerCharge > 0){
	
	  h_tightMuPlusDz->Fill(*upTrackerDz);
	  h_tightMuUpperPlusDz->Fill(*upTrackerDz);
	
	}
     
	
      }

      double lowerCpT = *lowTrackerCharge / (*lowTrackerMuonP4).Pt();
      h_lowerChi2->Fill(*lowTrackerChi2);
      h_lowerNdof->Fill(*lowTrackerNdof);
      h_lowerPt->Fill((*lowTrackerMuonP4).Pt());
      //h_lowerEta->Fill((*lowTrackerMuonP4).Eta());
      //h_lowerPhi->Fill((*lowTrackerMuonP4).Phi());
      h_lowerCharge->Fill(*lowTrackerCharge);
      h_lowerCurve->Fill(lowerCpT);
      h_lowerDxy->Fill(*lowTrackerDxy);
      h_lowerDz->Fill(*lowTrackerDz);
      h_lowerPixelHits->Fill(*lowTrackerPhits);
      h_lowerTrackerHits->Fill(*lowTrackerThits);
      h_lowerMuonStationHits-> Fill(*lowTrackerMhits);
      h_lowerValidHits->Fill(*lowTrackerValidHits);
      h_lowerMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
      h_lowerPtError->Fill(*lowTrackerPtError);
      h_lowerDxyError->Fill(*lowTrackerDxyError);
      h_lowerDzError->Fill(*lowTrackerDzError);
      h_lowerTrackPt->Fill(*lowTrackerPt);
      // h_lowerTrackEta->Fill((*lowTrackerTrack).Eta());
      //h_lowerTrackPhi->Fill((*lowTrackerTrack).Phi());
      h_lowerTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

      if(*lowTrackerCharge < 0){
	h_muMinusChi2->Fill(*lowTrackerChi2);
	h_muLowerMinusChi2->Fill(*lowTrackerChi2);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusChi2->Fill(*lowTrackerChi2);
	h_muLowerPlusChi2->Fill(*lowTrackerChi2);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusNdof->Fill(*lowTrackerNdof);
	h_muLowerMinusNdof->Fill(*lowTrackerNdof);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusNdof->Fill(*lowTrackerNdof);
	h_muLowerPlusNdof->Fill(*lowTrackerNdof);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusCharge->Fill(*lowTrackerCharge);
	h_muLowerMinusCharge->Fill(*lowTrackerCharge);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusCharge->Fill(*lowTrackerCharge);
	h_muLowerPlusCharge->Fill(*lowTrackerCharge);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusCurve->Fill(lowerCpT);
	h_muLowerMinusCurve->Fill(lowerCpT);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusCurve->Fill(lowerCpT);
	h_muLowerPlusCurve->Fill(lowerCpT);
	
      }

      if(*lowTrackerCharge < 0){
	h_muMinusDxy->Fill(*lowTrackerDxy);
	h_muLowerMinusDxy->Fill(*lowTrackerDxy);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusDxy->Fill(*lowTrackerDxy);
	h_muLowerPlusDxy->Fill(*lowTrackerDxy);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusDz->Fill(*lowTrackerDz);
	h_muLowerMinusDz->Fill(*lowTrackerDz);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusDz->Fill(*lowTrackerDz);
	h_muLowerPlusDz->Fill(*lowTrackerDz);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusPt->Fill((*lowTrackerMuonP4).Pt());
	h_muLowerMinusPt->Fill((*lowTrackerMuonP4).Pt());
      }
      else if(*lowTrackerCharge > 0){

	h_muPlusPt->Fill((*lowTrackerMuonP4).Pt());
	h_muLowerPlusPt->Fill((*lowTrackerMuonP4).Pt());

      }

      if(*lowTrackerCharge < 0){
	h_muMinusEta->Fill((*lowTrackerMuonP4).Eta());
	h_muLowerMinusEta->Fill((*lowTrackerMuonP4).Eta());
      }
      else if(*lowTrackerCharge > 0){

	h_muPlusEta->Fill((*lowTrackerMuonP4).Eta());
	h_muLowerPlusEta->Fill((*lowTrackerMuonP4).Eta());

      }

      if(*lowTrackerCharge < 0){
	h_muMinusPhi->Fill((*lowTrackerMuonP4).Phi());
	h_muLowerMinusPhi->Fill((*lowTrackerMuonP4).Phi());
      }
      else if(*lowTrackerCharge > 0){

	h_muPlusPhi->Fill((*lowTrackerMuonP4).Phi());
	h_muLowerPlusPhi->Fill((*lowTrackerMuonP4).Phi());

      }
      if(*lowTrackerCharge < 0){
	h_muMinusPixelHits->Fill(*lowTrackerPhits);
	h_muLowerMinusPixelHits->Fill(*lowTrackerPhits);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusPixelHits->Fill(*lowTrackerPhits);
	h_muLowerPlusPixelHits->Fill(*lowTrackerPhits);
	
      }
      
      if(*lowTrackerCharge < 0){
	h_muMinusTrackerHits->Fill(*lowTrackerThits);
	h_muLowerMinusTrackerHits->Fill(*lowTrackerThits);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusTrackerHits->Fill(*lowTrackerThits);
	h_muLowerPlusTrackerHits->Fill(*lowTrackerThits);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusMuonStationHits->Fill(*lowTrackerMhits);
	h_muLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusMuonStationHits->Fill(*lowTrackerMhits);
	h_muLowerPlusMuonStationHits->Fill(*lowTrackerMhits);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusValidHits->Fill(*lowTrackerValidHits);
	h_muLowerMinusValidHits->Fill(*lowTrackerValidHits);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusValidHits->Fill(*lowTrackerValidHits);
	h_muLowerPlusValidHits->Fill(*lowTrackerValidHits);
	
      }

      if(*lowTrackerCharge < 0){
	h_muMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	h_muLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	h_muLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusPtError->Fill(*lowTrackerPtError);
	h_muLowerMinusPtError->Fill(*lowTrackerPtError);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusPtError->Fill(*lowTrackerPtError);
	h_muLowerPlusPtError->Fill(*lowTrackerPtError);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusDxyError->Fill(*lowTrackerDxyError);
	h_muLowerMinusDxyError->Fill(*lowTrackerDxyError);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusDxyError->Fill(*lowTrackerDxyError);
	h_muLowerPlusDxyError->Fill(*lowTrackerDxyError);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusDzError->Fill(*lowTrackerDzError);
	h_muLowerMinusDzError->Fill(*lowTrackerDzError);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusDzError->Fill(*lowTrackerDzError);
	h_muLowerPlusDzError->Fill(*lowTrackerDzError);
	
      }
      if(*lowTrackerCharge < 0){
	h_muMinusTrackPt->Fill(*lowTrackerPt);
	h_muLowerMinusTrackPt->Fill(*lowTrackerPt);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusTrackPt->Fill(*lowTrackerPt);
	h_muLowerPlusTrackPt->Fill(*lowTrackerPt);
	
      }
      /*   if(*lowTrackerCharge < 0){
	   h_muMinusTrackEta->Fill((*lowTrackerTrack).Eta());
	   h_muLowerMinusTrackEta->Fill((*lowTrackerTrack).Eta());
	   }
	   else if(*lowTrackerCharge > 0){

	   h_muPlusTrackEta->Fill((*lowTrackerTrack).Eta());
	   h_muLowerPlusTrackEta->Fill((*lowTrackerTrack).Eta());

	   }


	   if(*lowTrackerCharge < 0){
	   h_muMinusTrackPhi->Fill((*lowTrackerTrack).Phi());
	   h_muLowerMinusTrackPhi->Fill((*lowTrackerTrack).Phi());
	   }
	   else if(*lowTrackerCharge > 0){

	   h_muPlusTrackPhi->Fill((*lowTrackerTrack).Phi());
	   h_muLowerPlusTrackPhi->Fill((*lowTrackerTrack).Phi());

	   }
      */ 
      if(*lowTrackerCharge < 0){
	h_muMinusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	h_muLowerMinusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
      }
      else if(*lowTrackerCharge > 0){
	
	h_muPlusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	h_muLowerPlusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	
      }
      for(int i = 0; i < nBiasBins; ++i){

	h_lowerCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	h_lowerCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	if (debug)
	  std::cout << "\nMade it through the lower bias loop " << i << std::endl; 
      }


      if(*lowTrackerCharge <  0){
	for(int i = 0; i < nBiasBins; ++i){

	  h_muMinusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muMinusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	  h_muLowerMinusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muLowerMinusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	

	  if (debug)
	    std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	}

      }

      else if(*lowTrackerCharge > 0){
	for(int i = 0; i < nBiasBins; ++i){

	  h_muPlusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muPlusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	  h_muLowerPlusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	  h_muLowerPlusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	

	  if (debug)
	    std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	}

      }

	
      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/ && (*lowTrackerLayersWithMeasurement > 5) ){

	if(*lowTrackerCharge < 0){
	  h_looseMuMinusChi2->Fill(*lowTrackerChi2);
	  h_looseMuLowerMinusChi2->Fill(*lowTrackerChi2);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusChi2->Fill(*lowTrackerChi2);
	  h_looseMuLowerPlusChi2->Fill(*lowTrackerChi2);
	
	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusNdof->Fill(*lowTrackerNdof);
	  h_looseMuLowerMinusNdof->Fill(*lowTrackerNdof);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusNdof->Fill(*lowTrackerNdof);
	  h_looseMuLowerPlusNdof->Fill(*lowTrackerNdof);
	
	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusCharge->Fill(*lowTrackerCharge);
	  h_looseMuLowerMinusCharge->Fill(*lowTrackerCharge);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusCharge->Fill(*lowTrackerCharge);
	  h_looseMuLowerPlusCharge->Fill(*lowTrackerCharge);
	
	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusCurve->Fill(lowerCpT);
	  h_looseMuLowerMinusCurve->Fill(lowerCpT);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusCurve->Fill(lowerCpT);
	  h_looseMuLowerPlusCurve->Fill(lowerCpT);
	
	}

	if(*lowTrackerCharge < 0){
	  h_looseMuMinusDxy->Fill(*lowTrackerDxy);
	  h_looseMuLowerMinusDxy->Fill(*lowTrackerDxy);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusDxy->Fill(*lowTrackerDxy);
	  h_looseMuLowerPlusDxy->Fill(*lowTrackerDxy);
	
	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusDz->Fill(*lowTrackerDz);
	  h_looseMuLowerMinusDz->Fill(*lowTrackerDz);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusDz->Fill(*lowTrackerDz);
	  h_looseMuLowerPlusDz->Fill(*lowTrackerDz);
	
	}


	if(*lowTrackerCharge < 0){
	  h_looseMuMinusEta->Fill((*lowTrackerMuonP4).Eta());
	  h_looseMuLowerMinusEta->Fill((*lowTrackerMuonP4).Eta());
	}
	else if(*lowTrackerCharge > 0){

	  h_looseMuPlusEta->Fill((*lowTrackerMuonP4).Eta());
	  h_looseMuLowerPlusEta->Fill((*lowTrackerMuonP4).Eta());

	}

	if(*lowTrackerCharge < 0){
	  h_looseMuMinusPhi->Fill((*lowTrackerMuonP4).Phi());
	  h_looseMuLowerMinusPhi->Fill((*lowTrackerMuonP4).Phi());
	}
	else if(*lowTrackerCharge > 0){

	  h_looseMuPlusPhi->Fill((*lowTrackerMuonP4).Phi());
	  h_looseMuLowerPlusPhi->Fill((*lowTrackerMuonP4).Phi());

	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusPixelHits->Fill(*lowTrackerPhits);
	  h_looseMuLowerMinusPixelHits->Fill(*lowTrackerPhits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusPixelHits->Fill(*lowTrackerPhits);
	  h_looseMuLowerPlusPixelHits->Fill(*lowTrackerPhits);
	
	}
      
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusTrackerHits->Fill(*lowTrackerThits);
	  h_looseMuLowerMinusTrackerHits->Fill(*lowTrackerThits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusTrackerHits->Fill(*lowTrackerThits);
	  h_looseMuLowerPlusTrackerHits->Fill(*lowTrackerThits);
	
	}

	if(*lowTrackerCharge < 0){
	  h_looseMuMinusValidHits->Fill(*lowTrackerValidHits);
	  h_looseMuLowerMinusValidHits->Fill(*lowTrackerValidHits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusValidHits->Fill(*lowTrackerValidHits);
	  h_looseMuLowerPlusValidHits->Fill(*lowTrackerValidHits);
	
	}



	if(*lowTrackerCharge < 0){
	  h_looseMuMinusDxyError->Fill(*lowTrackerDxyError);
	  h_looseMuLowerMinusDxyError->Fill(*lowTrackerDxyError);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusDxyError->Fill(*lowTrackerDxyError);
	  h_looseMuLowerPlusDxyError->Fill(*lowTrackerDxyError);
	
	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusDzError->Fill(*lowTrackerDzError);
	  h_looseMuLowerMinusDzError->Fill(*lowTrackerDzError);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusDzError->Fill(*lowTrackerDzError);
	  h_looseMuLowerPlusDzError->Fill(*lowTrackerDzError);
	
	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusTrackPt->Fill(*lowTrackerPt);
	  h_looseMuLowerMinusTrackPt->Fill(*lowTrackerPt);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusTrackPt->Fill(*lowTrackerPt);
	  h_looseMuLowerPlusTrackPt->Fill(*lowTrackerPt);
	
	}
	/*   if(*lowTrackerCharge < 0){
	     h_looseMuMinusTrackEta->Fill((*lowTrackerTrack).Eta());
	     h_looseMuLowerMinusTrackEta->Fill((*lowTrackerTrack).Eta());
	     }
	     else if(*lowTrackerCharge > 0){

	     h_looseMuPlusTrackEta->Fill((*lowTrackerTrack).Eta());
	     h_looseMuLowerPlusTrackEta->Fill((*lowTrackerTrack).Eta());

	     }


	     if(*lowTrackerCharge < 0){
	     h_looseMuMinusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     h_looseMuLowerMinusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     }
	     else if(*lowTrackerCharge > 0){

	     h_looseMuPlusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     h_looseMuLowerPlusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     }
	*/
 
	if(*lowTrackerCharge <  0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_looseMuMinusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuMinusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_looseMuLowerMinusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuLowerMinusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}

	else if(*lowTrackerCharge > 0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_looseMuPlusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuPlusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_looseMuLowerPlusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_looseMuLowerPlusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}


      }

	

      if((*lowTrackerMatchedMuonStations > 1) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/ && (*lowTrackerLayersWithMeasurement > 5) ){

	if(*lowTrackerCharge < 0){
	  h_looseMuMinusMuonStationHits->Fill(*lowTrackerMhits);
	  h_looseMuLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusMuonStationHits->Fill(*lowTrackerMhits);
	  h_looseMuLowerPlusMuonStationHits->Fill(*lowTrackerMhits);
	
	}
      }

      if((*lowTrackerMhits > 0) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/ && (*lowTrackerLayersWithMeasurement > 5) ){
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	  h_looseMuLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	  h_looseMuLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	
	}

      }


      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) && (*lowTrackerLayersWithMeasurement > 5) ){
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusPt->Fill((*lowTrackerMuonP4).Pt());
	  h_looseMuLowerMinusPt->Fill((*lowTrackerMuonP4).Pt());
	}
	else if(*lowTrackerCharge > 0){

	  h_looseMuPlusPt->Fill((*lowTrackerMuonP4).Pt());
	  h_looseMuLowerPlusPt->Fill((*lowTrackerMuonP4).Pt());

	}
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusPtError->Fill(*lowTrackerPtError);
	  h_looseMuLowerMinusPtError->Fill(*lowTrackerPtError);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusPtError->Fill(*lowTrackerPtError);
	  h_looseMuLowerPlusPtError->Fill(*lowTrackerPtError);
	
	}



      }


      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/){
	if(*lowTrackerCharge < 0){
	  h_looseMuMinusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	  h_looseMuLowerMinusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_looseMuPlusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	  h_looseMuLowerPlusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	
	}
      }


    







      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/&& (*lowTrackerPt > 150) && (*lowTrackerLayersWithMeasurement > 5) && (*lowTrackerDxy < 0.2) && (*lowTrackerDz < 0.5)){
	

	if(*lowTrackerCharge < 0){
	  h_tightMuMinusChi2->Fill(*lowTrackerChi2);
	  h_tightMuLowerMinusChi2->Fill(*lowTrackerChi2);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusChi2->Fill(*lowTrackerChi2);
	  h_tightMuLowerPlusChi2->Fill(*lowTrackerChi2);
	
	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusNdof->Fill(*lowTrackerNdof);
	  h_tightMuLowerMinusNdof->Fill(*lowTrackerNdof);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusNdof->Fill(*lowTrackerNdof);
	  h_tightMuLowerPlusNdof->Fill(*lowTrackerNdof);
	
	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusCharge->Fill(*lowTrackerCharge);
	  h_tightMuLowerMinusCharge->Fill(*lowTrackerCharge);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusCharge->Fill(*lowTrackerCharge);
	  h_tightMuLowerPlusCharge->Fill(*lowTrackerCharge);
	
	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusCurve->Fill(lowerCpT);
	  h_tightMuLowerMinusCurve->Fill(lowerCpT);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusCurve->Fill(lowerCpT);
	  h_tightMuLowerPlusCurve->Fill(lowerCpT);
	
	}


	if(*lowTrackerCharge < 0){
	  h_tightMuMinusEta->Fill((*lowTrackerMuonP4).Eta());
	  h_tightMuLowerMinusEta->Fill((*lowTrackerMuonP4).Eta());
	}
	else if(*lowTrackerCharge > 0){

	  h_tightMuPlusEta->Fill((*lowTrackerMuonP4).Eta());
	  h_tightMuLowerPlusEta->Fill((*lowTrackerMuonP4).Eta());

	}

	if(*lowTrackerCharge < 0){
	  h_tightMuMinusPhi->Fill((*lowTrackerMuonP4).Phi());
	  h_tightMuLowerMinusPhi->Fill((*lowTrackerMuonP4).Phi());
	}
	else if(*lowTrackerCharge > 0){

	  h_tightMuPlusPhi->Fill((*lowTrackerMuonP4).Phi());
	  h_tightMuLowerPlusPhi->Fill((*lowTrackerMuonP4).Phi());

	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusPixelHits->Fill(*lowTrackerPhits);
	  h_tightMuLowerMinusPixelHits->Fill(*lowTrackerPhits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusPixelHits->Fill(*lowTrackerPhits);
	  h_tightMuLowerPlusPixelHits->Fill(*lowTrackerPhits);
	
	}
      
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusTrackerHits->Fill(*lowTrackerThits);
	  h_tightMuLowerMinusTrackerHits->Fill(*lowTrackerThits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusTrackerHits->Fill(*lowTrackerThits);
	  h_tightMuLowerPlusTrackerHits->Fill(*lowTrackerThits);
	
	}
 
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusValidHits->Fill(*lowTrackerValidHits);
	  h_tightMuLowerMinusValidHits->Fill(*lowTrackerValidHits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusValidHits->Fill(*lowTrackerValidHits);
	  h_tightMuLowerPlusValidHits->Fill(*lowTrackerValidHits);
	
	}

 
 
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusDxyError->Fill(*lowTrackerDxyError);
	  h_tightMuLowerMinusDxyError->Fill(*lowTrackerDxyError);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusDxyError->Fill(*lowTrackerDxyError);
	  h_tightMuLowerPlusDxyError->Fill(*lowTrackerDxyError);
	
	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusDzError->Fill(*lowTrackerDzError);
	  h_tightMuLowerMinusDzError->Fill(*lowTrackerDzError);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusDzError->Fill(*lowTrackerDzError);
	  h_tightMuLowerPlusDzError->Fill(*lowTrackerDzError);
	
	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusTrackPt->Fill(*lowTrackerPt);
	  h_tightMuLowerMinusTrackPt->Fill(*lowTrackerPt);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusTrackPt->Fill(*lowTrackerPt);
	  h_tightMuLowerPlusTrackPt->Fill(*lowTrackerPt);
	
	}
	/*   if(*lowTrackerCharge < 0){
	     h_tightMuMinusTrackEta->Fill((*lowTrackerTrack).Eta());
	     h_tightMuLowerMinusTrackEta->Fill((*lowTrackerTrack).Eta());
	     }
	     else if(*lowTrackerCharge > 0){

	     h_tightMuPlusTrackEta->Fill((*lowTrackerTrack).Eta());
	     h_tightMuLowerPlusTrackEta->Fill((*lowTrackerTrack).Eta());

	     }


	     if(*lowTrackerCharge < 0){
	     h_tightMuMinusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     h_tightMuLowerMinusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     }
	     else if(*lowTrackerCharge > 0){

	     h_tightMuPlusTrackPhi->Fill((*lowTrackerTrack).Phi());
	     h_tightMuLowerPlusTrackPhi->Fill((*lowTrackerTrack).Phi());

	     }
	*/

	if(*lowTrackerCharge <  0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_tightMuMinusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuMinusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_tightMuLowerMinusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuLowerMinusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}

	else if(*lowTrackerCharge > 0){
	  for(int i = 0; i < nBiasBins; ++i){

	    h_tightMuPlusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuPlusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	    h_tightMuLowerPlusCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	    h_tightMuLowerPlusCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	

	    if (debug)
	      std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
	  }

	}



      }

      if((*lowTrackerMatchedMuonStations > 1) && /*((*lowTrackerPtError / *lowTrackerPt) < 0.3) &&*/ (*lowTrackerPt > 150) &&(*lowTrackerLayersWithMeasurement > 5) && (*lowTrackerDxy < 0.2) && (*lowTrackerDz < 0.5) ){

	if(*lowTrackerCharge < 0){
	  h_tightMuMinusMuonStationHits->Fill(*lowTrackerMhits);
	  h_tightMuLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusMuonStationHits->Fill(*lowTrackerMhits);
	  h_tightMuLowerPlusMuonStationHits->Fill(*lowTrackerMhits);
	
	}
      }

      if((*lowTrackerMhits > 0) && /*((*lowTrackerPtError / *lowTrackerPt) < 0.3) &&*/ (*lowTrackerPt > 150) && (*lowTrackerLayersWithMeasurement > 5) && (*lowTrackerDxy < 0.2) && (*lowTrackerDz < 0.5) ){
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	  h_tightMuLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	  h_tightMuLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	
	}

      }


      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) && (*lowTrackerLayersWithMeasurement > 5) && (*lowTrackerDxy < 0.2) && (*lowTrackerDz < 0.5)){
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusPt->Fill((*lowTrackerMuonP4).Pt());
	  h_tightMuLowerMinusPt->Fill((*lowTrackerMuonP4).Pt());
	}
	else if(*lowTrackerCharge > 0){

	  h_tightMuPlusPt->Fill((*lowTrackerMuonP4).Pt());
	  h_tightMuLowerPlusPt->Fill((*lowTrackerMuonP4).Pt());

	}
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusPtError->Fill(*lowTrackerPtError);
	  h_tightMuLowerMinusPtError->Fill(*lowTrackerPtError);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusPtError->Fill(*lowTrackerPtError);
	  h_tightMuLowerPlusPtError->Fill(*lowTrackerPtError);
	
	}



      }


      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) && (*lowTrackerPt > 150) &&  /*((*lowTrackerPtError / *lowTrackerPt) < 0.3) &&*/ (*lowTrackerDxy < 0.2) && (*lowTrackerDz < 0.5)){
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	  h_tightMuLowerMinusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	  h_tightMuLowerPlusTrackLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	
	}
      }


      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) && (*lowTrackerPt > 150) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/ && (*lowTrackerLayersWithMeasurement > 5) && (*lowTrackerDz < 0.5)){
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusDxy->Fill(*lowTrackerDxy);
	  h_tightMuLowerMinusDxy->Fill(*lowTrackerDxy);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusDxy->Fill(*lowTrackerDxy);
	  h_tightMuLowerPlusDxy->Fill(*lowTrackerDxy);
	
	}
	
      }


      if((*lowTrackerMhits > 0) && (*lowTrackerMatchedMuonStations > 1) && (*lowTrackerPt > 150) /*&& ((*lowTrackerPtError / *lowTrackerPt) > 0.3)*/ && (*lowTrackerLayersWithMeasurement > 5) && (*lowTrackerDxy < 0.2)){
	if(*lowTrackerCharge < 0){
	  h_tightMuMinusDz->Fill(*lowTrackerDz);
	  h_tightMuLowerMinusDz->Fill(*lowTrackerDz);
	}
	else if(*lowTrackerCharge > 0){
	
	  h_tightMuPlusDz->Fill(*lowTrackerDz);
	  h_tightMuLowerPlusDz->Fill(*lowTrackerDz);
	
	}
     
	
      }

      j++;
      if (debug)
	std::cout << "\n\nMade it through " << j << " sets of fills\n";	
    }//Closing if fill
    //  } // end if tracker.Next
  } // end while loop
  

  std::cout << std::hex << g << std::dec << std::endl;
  
  c1->cd();

  comp->Draw();

  comp->cd();
  comp->SetLogy(1);
  h_upperChi2->SetMarkerColor(7);
  h_upperChi2->SetLineColor(7);
  h_upperChi2->SetMarkerStyle(4);
  h_upperChi2->Draw("ep0");
  h_upperChi2->GetXaxis()->SetTitle("Chi2");
  h_upperChi2->GetYaxis()->SetTitle("Number of Muons");


  comp->Update();
  c1->Update();

  c1->SaveAs(TString("UpperMuonChi2.jpg"));
  c1->SaveAs(TString("UpperMuonChi2.png"));
  c1->SaveAs(TString("UpperMuonChi2.pdf"));
  c1->SaveAs(TString("UpperMuonChi2.eps"));

  g->Write();
  g->Close();


  // compare2Plot(h_noCut , h_pTCut, 0.0001, "Mass (GeV)", "Number of events/5GeV", "MassCompare");
  //compare3Plot(h_mu1noCut,h_mu1NewCut, h_mu1pTCut,0.001, "Momentum (GeV)", "Number of events/5GeV", "Muon1Compare");
  //compare3Plot(h_mu2noCut, h_mu2NewCut, h_mu2pTCut, 0.001, "Momentum (GeV)", "Number of events/5GeV", "Muon2Compare");
  // compare3Plot(h_ZnoCut, h_ZpTnewCut, h_ZpTCut, 0.01, "Momentum(GeV)", "Number of events/5GeV", "ZpTCompare");
  // compare2Plot(h_invarMassNoCut, h_invarMasspTCut, 0.0001, "Mass (GeV)", "Number of events/5GeV", "InvariantMassCompare");
  // plot2D(h_pTCompare,"Muon1 pT (GeV)", "Muon2 pT (GeV)", "MuonCompare");
  //plot2D(h_pTCompareNew, "Muon1 pT (GeV)", "Muon2 pT (GeV)", "NewMuonCompare");

  return;
}

