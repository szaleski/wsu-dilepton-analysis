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
#include "TLegend.h"
#include "TDirectory.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>

void checkScale(std::string const& inFile1, int bias){

  TFile* oFile;
  oFile = new TFile("checkScale.root", "RECREATE");

  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inFile1));
  if(f == 0){
    std::cout << "Error: cannot open file1!\n";
    return;
  }

  std::cout << "\nSuccessfully openedd file 1!\n";
  
  std::stringstream ss;
  ss.str( std::string() );
  ss.clear();
  if(bias == 1000) ss << std::setw(4) << bias;
  else ss << std::setw(3) << std::setfill('0') << bias;

  TH1F* dataHistScaled = (TH1F*)f->Get("TunePCraftMinus;2");
  TH1F* dataHistCut = (TH1F*)f->Get("TunePCraftMinus_AfterCut");
  TH1F* mcHistScaled = (TH1F*)f->Get(TString("TunePMCMinus_Curvature_Scaled_MinusBias"+ss.str()));
  TH1F* mcHistCut = (TH1F*)f->Get(TString("TunePMCMinus_Curve_AfterCut_MinusBias"+ss.str()));
  TH1F* dataHistRebin = (TH1F*)f->Get("TunePCraftMinus_Rebinned");
  dataHistRebin->Rebin(2);
  TH1F* mcHistRebin = (TH1F*)f->Get(TString("TunePMCMinus_Curve_Rebinned_MinusBias"+ss.str()));
  mcHistRebin->Rebin(2);
  TH1F* chi2Hist = (TH1F*)f->Get(TString("TunePChi2vCurveMinus"));
  std::cout << "\nGot the Chi2 histo\n";
  std::cout << "\nchi2 histo:" << std::hex << chi2Hist << std::dec << std::endl;
  
  double chi2 = chi2Hist->GetBinContent(1000 - bias);


  oFile->cd();
  Double_t mcMax;
  Double_t dataMax;
  Double_t max;

  mcMax = mcHistScaled->GetMaximum();
  dataMax = dataHistScaled->GetMaximum();
  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;

  TCanvas *tmpCanvas1 = new TCanvas("canvas1","canvas1",1600,900);
  TPad *tmpPad1 = new TPad("pad1", "pad1", 0.0, 0.0, 1.0, 1.0);
  tmpCanvas1->cd();
  tmpPad1->Draw();
  tmpPad1->cd();
  dataHistScaled->Draw("ep0");
  dataHistScaled->SetMarkerColor(4);
  dataHistScaled->SetLineColor(4);
  dataHistScaled->SetMaximum(max*1.2);
  dataHistScaled->SetMinimum(0.1);
  dataHistScaled->GetXaxis()->SetTitle("#kappa[c/TeV]");
  dataHistScaled->GetYaxis()->SetTitle(TString("N_#mu"));
  dataHistScaled->SetTitle("#kappa Distribution");
  tmpPad1->Update();
  tmpCanvas1->Update();

  mcHistScaled->SetMarkerColor(2);
  mcHistScaled->SetLineColor(2);
  mcHistScaled->SetMaximum(max*1.2);
  mcHistScaled->Draw("ep0sames");
  tmpPad1->Update();  
  tmpCanvas1->Update();


  
  tmpPad1->cd();
  TPaveStats *stats1 = (TPaveStats*)dataHistScaled->FindObject("stats");
  std::cout << "\nCalled stats" << stats1  << std::endl;
  stats1->SetName("data");
  std::cout << "\nSet Name" << std::endl;
  stats1->SetY1NDC(.7);
  std::cout << "\nSet lower y pos" << std::endl;
  stats1->SetY2NDC(.9);
  std::cout << "\nSet upper y pos" << std::endl;
  stats1->SetTextColor(4);
  std::cout << "\nSet text color" << std::endl;
  stats1->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad1->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpPad1->Update();
  tmpCanvas1->Update();

  std::stringstream legendText1;

  TLegend* leg1 = new TLegend(0.2, 0.7, 0.7, 0.9);
  legendText1.str( std::string() );
  legendText1.clear();
  legendText1 << "#chi^{2} = " << chi2;
  leg1->AddEntry((TObject*) 0, TString(legendText1.str()), "");

  leg1->SetTextSize(0.020);
  leg1->Draw();
  tmpPad1->Update();
  tmpCanvas1->Update();

  TPaveStats *stats2 = (TPaveStats*)mcHistScaled->FindObject("stats");
  std::cout << "\nCalled stats" << stats2  << std::endl;
  stats2->SetName("data");
  std::cout << "\nSet Name" << std::endl;
  stats2->SetY1NDC(.5);
  std::cout << "\nSet lower y pos" << std::endl;
  stats2->SetY2NDC(.7);
  std::cout << "\nSet upper y pos" << std::endl;
  stats2->SetTextColor(2);
  std::cout << "\nSet text color" << std::endl;
  stats2->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad1->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpPad1->Update();
  tmpCanvas1->Update();


  mcMax = mcHistCut->GetMaximum();
  dataMax = dataHistCut->GetMaximum();
  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;

  TCanvas *tmpCanvas2 = new TCanvas("canvas2","canavas2",1600,900);
  TPad *tmpPad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 1.0);
  tmpCanvas2->cd();
  tmpPad2->Draw();
  tmpPad2->cd();
  dataHistCut->Draw("ep0");
  dataHistCut->SetMarkerColor(4);
  dataHistCut->SetLineColor(4);
  dataHistCut->SetMaximum(max*1.2);
  dataHistCut->SetMinimum(0.1);
  dataHistCut->GetXaxis()->SetTitle("#kappa[c/TeV]");
  dataHistCut->GetYaxis()->SetTitle(TString("N_#mu"));
  dataHistCut->SetTitle(" #kappa Distribution");
  tmpPad2->Update();
  tmpCanvas2->Update();

  mcHistCut->SetMarkerColor(2);
  mcHistCut->SetLineColor(2);
  mcHistCut->SetMaximum(max*1.2);
  mcHistCut->Draw("ep0sames");
  tmpPad2->Update();  
  tmpCanvas2->Update();
  
  TLegend* leg2 = new TLegend(0.2, 0.7, 0.7, 0.9);
  legendText1.str( std::string() );
  legendText1.clear();
  legendText1 << "#chi^{2} = " << chi2;
  leg2->AddEntry((TObject*) 0, TString(legendText1.str()), "");

  leg2->SetTextSize(0.020);
  leg2->Draw();
  tmpPad2->Update();
  tmpCanvas2->Update();


  tmpPad2->cd();
  TPaveStats *stats3 = (TPaveStats*)dataHistCut->FindObject("stats");
  std::cout << "\nCalled stats" << stats3  << std::endl;
  stats3->SetName("data");
  std::cout << "\nSet Name" << std::endl;
  stats3->SetY1NDC(.7);
  std::cout << "\nSet lower y pos" << std::endl;
  stats3->SetY2NDC(.9);
  std::cout << "\nSet upper y pos" << std::endl;
  stats3->SetTextColor(4);
  std::cout << "\nSet text color" << std::endl;
  stats3->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad2->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpPad2->Update();
  tmpCanvas2->Update();

  TPaveStats *stats4 = (TPaveStats*)mcHistCut->FindObject("stats");
  std::cout << "\nCalled stats" << stats4  << std::endl;
  stats4->SetName("data");
  std::cout << "\nSet Name" << std::endl;
  stats4->SetY1NDC(.5);
  std::cout << "\nSet lower y pos" << std::endl;
  stats4->SetY2NDC(.7);
  std::cout << "\nSet upper y pos" << std::endl;
  stats4->SetTextColor(2);
  std::cout << "\nSet text color" << std::endl;
  stats4->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad2->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpPad2->Update();
  tmpCanvas2->Update();

  mcMax = mcHistRebin->GetMaximum();
  dataMax = dataHistRebin->GetMaximum();
  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;



  TCanvas *tmpCanvas3 = new TCanvas("canvas3","canvas3",1600,900);
  TPad *tmpPad3 = new TPad("pad3", "pad3", 0.0, 0.0, 1.0, 1.0);
  tmpCanvas3->cd();
  tmpPad3->Draw();
  tmpPad3->cd();
  dataHistRebin->Draw("ep0");
  dataHistRebin->SetMarkerColor(4);
  dataHistRebin->SetLineColor(4);
  dataHistRebin->SetMaximum(max*1.2);
  dataHistRebin->SetMinimum(0.1);
  dataHistRebin->GetXaxis()->SetTitle("#kappa[c/TeV]");
  dataHistRebin->GetYaxis()->SetTitle(TString("N_#mu"));
  dataHistRebin->SetTitle(" #kappa Distribution");
  tmpPad3->Update();
  tmpCanvas3->Update();

  mcHistRebin->SetMarkerColor(2);
  mcHistRebin->SetLineColor(2);
  mcHistRebin->SetMaximum(max*1.2);
  mcHistRebin->Draw("ep0sames");
  tmpPad3->Update();  
  tmpCanvas3->Update();


  TLegend* leg3 = new TLegend(0.2, 0.7, 0.7, 0.9);
  legendText1.str( std::string() );
  legendText1.clear();
  legendText1 << "#chi^{2} = " << chi2;
  leg3->AddEntry((TObject*) 0, TString(legendText1.str()), "");

  leg3->SetTextSize(0.020);
  leg3->Draw();
  tmpPad3->Update();
  tmpCanvas3->Update();


  tmpPad3->cd();
  TPaveStats *stats5 = (TPaveStats*)dataHistRebin->FindObject("stats");
  std::cout << "\nCalled stats" << stats5  << std::endl;
  stats5->SetName("data");
  std::cout << "\nSet Name" << std::endl;
  stats5->SetY1NDC(.7);
  std::cout << "\nSet lower y pos" << std::endl;
  stats5->SetY2NDC(.9);
  std::cout << "\nSet upper y pos" << std::endl;
  stats5->SetTextColor(4);
  std::cout << "\nSet text color" << std::endl;
  stats5->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad3->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpPad3->Update();
  tmpCanvas3->Update();

  TPaveStats *stats6 = (TPaveStats*)mcHistRebin->FindObject("stats");
  std::cout << "\nCalled stats" << stats6  << std::endl;
  stats6->SetName("data");
  std::cout << "\nSet Name" << std::endl;
  stats6->SetY1NDC(.5);
  std::cout << "\nSet lower y pos" << std::endl;
  stats6->SetY2NDC(.7);
  std::cout << "\nSet upper y pos" << std::endl;
  stats6->SetTextColor(2);
  std::cout << "\nSet text color" << std::endl;
  stats6->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad3->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpPad3->Update();
  tmpCanvas3->Update();


  tmpCanvas1->Write();
  tmpCanvas2->Write();
  tmpCanvas3->Write();
  oFile->Write();
  oFile->Close();

  return;

}
