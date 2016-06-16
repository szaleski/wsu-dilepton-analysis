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

void scaleIt(TFile *file, TH1F *mcHist1, TH1F *mcHist2, TH1F *dataHist1, TH1F *dataHist2, Double_t scale, TCanvas *looseCanvas, std::string const& plotName, int isPt);
void scaleCurve(TFile *file, TH1F *mcHist1, TH1F *mcHist2, TH1F *dataHist1, TH1F *dataHist2, Double_t scale, TCanvas *looseCanvas, int rebins, std::string const& plotName, int isPt);
void saveIt(TH1F *mcHist, TH1F *dataHist, TCanvas *tmpCanvas, std::string const& plotName);
void plotIt(TFile *file, TH1F *mcHist, TH1F *dataHist, TCanvas *looseCanvas, std::string const& plotName, int isPt);
void ptScaleIt(TFile *file, TH1F *mcHist1, TH1F *mcHist2, TH1F *dataHist1, TH1F *dataHist2, Double_t scale, TCanvas *looseCanvas);


void plotMCvData(std::string const& file1, std::string const& file2, int rebins){

  TFile *g;
  g = new TFile("CosmicCurvature.root","RECREATE");
  
  //  TH1::SetDefaultSumw2();


  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+file1));
  if(f == 0){
    std::cout << "Error: cannot open file1! \n";
    return;
  }
  //  std::cout << std::hex << *f << std::dec << std::endl;
  std::cout << "\nsuccessfully opened file1!\n";

  TFile *h = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+file2));

  if(h == 0){
    std::cout << "Error: cannot open file2! \n";
    return;
  }
  std::cout << "\nsuccessfully opened file2!\n";

  TCanvas *looseCanvas = new TCanvas("loose","loose",1600,900);
  TCanvas *ptCanvas = new TCanvas("pt","pt", 1600, 900);
  Double_t scale = 0.28;

  looseCanvas->Divide(4,5);

  TH1F *dataMuMinusCurve = (TH1F*)h->Get("looseMuLowerMinusCurve");
  TH1F *dataMuPlusCurve = (TH1F*)h->Get("looseMuLowerPlusCurve");
  TH1F *mcMuMinusCurve = (TH1F*)f->Get("looseMuLowerMinusCurve");
  TH1F *mcMuPlusCurve = (TH1F*)f->Get("looseMuLowerPlusCurve");
  TH1F *dataMuMinusChi2 = (TH1F*)h->Get("looseMuLowerMinusChi2");
  TH1F *dataMuPlusChi2 = (TH1F*)h->Get("looseMuLowerPlusChi2");
  TH1F *mcMuMinusChi2 = (TH1F*)f->Get("looseMuLowerMinusChi2");
  TH1F *mcMuPlusChi2 = (TH1F*)f->Get("looseMuLowerPlusChi2");
  TH1F *dataMuMinusNdof = (TH1F*)h->Get("looseMuLowerMinusNdof");
  TH1F *dataMuPlusNdof = (TH1F*)h->Get("looseMuLowerPlusNdof");
  TH1F *mcMuMinusNdof = (TH1F*)f->Get("looseMuLowerMinusNdof");
  TH1F *mcMuPlusNdof = (TH1F*)f->Get("looseMuLowerPlusNdof");
  TH1F *dataMuMinusCharge = (TH1F*)h->Get("looseMuLowerMinusCharge");
  TH1F *dataMuPlusCharge = (TH1F*)h->Get("looseMuLowerPlusCharge");
  TH1F *mcMuMinusCharge = (TH1F*)f->Get("looseMuLowerMinusCharge");
  TH1F *mcMuPlusCharge = (TH1F*)f->Get("looseMuLowerPlusCharge");
  TH1F *dataMuMinusTrackPt = (TH1F*)h->Get("looseMuLowerMinusTrackPt");
  TH1F *dataMuPlusTrackPt = (TH1F*)h->Get("looseMuLowerPlusTrackPt");
  TH1F *mcMuMinusTrackPt = (TH1F*)f->Get("looseMuLowerMinusTrackPt");
  TH1F *mcMuPlusTrackPt = (TH1F*)f->Get("looseMuLowerPlusTrackPt");
  TH1F *dataMuMinusTrackEta = (TH1F*)h->Get("looseMuLowerMinusTrackEta");
  TH1F *dataMuPlusTrackEta = (TH1F*)h->Get("looseMuLowerPlusTrackEta");
  TH1F *mcMuMinusTrackEta = (TH1F*)f->Get("looseMuLowerMinusTrackEta");
  TH1F *mcMuPlusTrackEta = (TH1F*)f->Get("looseMuLowerPlusTrackEta");
  TH1F *dataMuMinusTrackPhi = (TH1F*)h->Get("looseMuLowerMinusTrackPhi");
  TH1F *dataMuPlusTrackPhi = (TH1F*)h->Get("looseMuLowerPlusTrackPhi");
  TH1F *mcMuMinusTrackPhi = (TH1F*)f->Get("looseMuLowerMinusTrackPhi");
  TH1F *mcMuPlusTrackPhi = (TH1F*)f->Get("looseMuLowerPlusTrackPhi");
  TH1F *dataMuMinusPtRelErr = (TH1F*)h->Get("looseMuLowerMinusPtRelErr");
  TH1F *dataMuPlusPtRelErr = (TH1F*)h->Get("looseMuLowerPlusPtRelErr");
  TH1F *mcMuMinusPtRelErr = (TH1F*)f->Get("looseMuLowerMinusPtRelErr");
  TH1F *mcMuPlusPtRelErr = (TH1F*)f->Get("looseMuLowerPlusPtRelErr");
  TH1F *dataMuMinusDz = (TH1F*)h->Get("looseMuLowerMinusDz");
  TH1F *dataMuPlusDz = (TH1F*)h->Get("looseMuLowerPlusDz");
  TH1F *mcMuMinusDz = (TH1F*)f->Get("looseMuLowerMinusDz");
  TH1F *mcMuPlusDz = (TH1F*)f->Get("looseMuLowerPlusDz");
  TH1F *dataMuMinusDxy = (TH1F*)h->Get("looseMuLowerMinusDxy");
  TH1F *dataMuPlusDxy = (TH1F*)h->Get("looseMuLowerPlusDxy");
  TH1F *mcMuMinusDxy = (TH1F*)f->Get("looseMuLowerMinusDxy");
  TH1F *mcMuPlusDxy = (TH1F*)f->Get("looseMuLowerPlusDxy");
  TH1F *dataMuMinusValidHits = (TH1F*)h->Get("looseMuLowerMinusValidHits");
  TH1F *dataMuPlusValidHits = (TH1F*)h->Get("looseMuLowerPlusValidHits");
  TH1F *mcMuMinusValidHits = (TH1F*)f->Get("looseMuLowerMinusValidHits");
  TH1F *mcMuPlusValidHits = (TH1F*)f->Get("looseMuLowerPlusValidHits");
  TH1F *dataMuMinusPixelHits = (TH1F*)h->Get("looseMuLowerMinusPixelHits");
  TH1F *dataMuPlusPixelHits = (TH1F*)h->Get("looseMuLowerPlusPixelHits");
  TH1F *mcMuMinusPixelHits = (TH1F*)f->Get("looseMuLowerMinusPixelHits");
  TH1F *mcMuPlusPixelHits = (TH1F*)f->Get("looseMuLowerPlusPixelHits");
  TH1F *dataMuMinusTrackerHits = (TH1F*)h->Get("looseMuLowerMinusTrackerHits");
  TH1F *dataMuPlusTrackerHits = (TH1F*)h->Get("looseMuLowerPlusTrackerHits");
  TH1F *mcMuMinusTrackerHits = (TH1F*)f->Get("looseMuLowerMinusTrackerHits");
  TH1F *mcMuPlusTrackerHits = (TH1F*)f->Get("looseMuLowerPlusTrackerHits");
  TH1F *dataMuMinusValidMuonHits = (TH1F*)h->Get("looseMuLowerMinusValidMuonHits");
  TH1F *dataMuPlusValidMuonHits = (TH1F*)h->Get("looseMuLowerPlusValidMuonHits");
  TH1F *mcMuMinusValidMuonHits = (TH1F*)f->Get("looseMuLowerMinusValidMuonHits");
  TH1F *mcMuPlusValidMuonHits = (TH1F*)f->Get("looseMuLowerPlusValidMuonHits");
  TH1F *dataMuMinusMuonStationHits = (TH1F*)h->Get("looseMuLowerMinusMuonStationHits");
  TH1F *dataMuPlusMuonStationHits = (TH1F*)h->Get("looseMuLowerPlusMuonStationHits");
  TH1F *mcMuMinusMuonStationHits = (TH1F*)f->Get("looseMuLowerMinusMuonStationHits");
  TH1F *mcMuPlusMuonStationHits = (TH1F*)f->Get("looseMuLowerPlusMuonStationHits");
  TH1F *dataMuMinusMatchedMuonStations = (TH1F*)h->Get("looseMuLowerMinusMatchedMuonStations");
  TH1F *dataMuPlusMatchedMuonStations = (TH1F*)h->Get("looseMuLowerPlusMatchedMuonStations");
  TH1F *mcMuMinusMatchedMuonStations = (TH1F*)f->Get("looseMuLowerMinusMatchedMuonStations");
  TH1F *mcMuPlusMatchedMuonStations = (TH1F*)f->Get("looseMuLowerPlusMatchedMuonStations");
  TH1F *dataMuMinusTrackerLayersWithMeasurement = (TH1F*)h->Get("looseMuLowerMinusTrackerLayersWithMeasurement");
  TH1F *dataMuPlusTrackerLayersWithMeasurement = (TH1F*)h->Get("looseMuLowerPlusTrackerLayersWithMeasurement");
  TH1F *mcMuMinusTrackerLayersWithMeasurement = (TH1F*)f->Get("looseMuLowerMinusTrackerLayersWithMeasurement");
  TH1F *mcMuPlusTrackerLayersWithMeasurement = (TH1F*)f->Get("looseMuLowerPlusTrackerLayersWithMeasurement");

  Int_t data1Width = dataMuMinusTrackPt->GetBinWidth(50);
  Int_t data2Width = dataMuPlusTrackPt->GetBinWidth(50);
  Int_t mc1Width = mcMuMinusTrackPt->GetBinWidth(50);
  Int_t mc2Width = mcMuPlusTrackPt->GetBinWidth(50);

  std::cout << "\n\nThe bin widths are: " << data1Width << "\t" << data2Width << "\t" << mc1Width << "\t" << mc2Width << std::endl;

  Int_t width = -1;
  if( (data1Width == data2Width) && (data2Width == mc1Width) && (mc1Width == mc2Width)) width = data1Width;
  else{
    std::cout << "\n\nThe bin widths are not the same between histograms! Must fix!";
      return;
  }

  std::cout << "\n\nThe bin width is: " << width << std::endl;

  Int_t startBin = 100/width;
  std::cout << "\n\nStarting bin is: " << startBin << std::endl;

  Int_t data1Bins = dataMuMinusTrackPt->GetNbinsX();
  Int_t data2Bins = dataMuPlusTrackPt->GetNbinsX();
  Int_t mc1Bins = mcMuMinusTrackPt->GetNbinsX();
  Int_t mc2Bins = mcMuPlusTrackPt->GetNbinsX();

  std::cout << "\n\nThe total number of bins are: " << data1Bins << "\t" << data2Bins << "\t" << mc1Bins << "\t" << mc2Bins << std::endl;

  Int_t numBins;
  if((data1Bins == data2Bins) && (data2Bins == mc1Bins) && (mc1Bins == mc2Bins)) numBins = data1Bins;
  else{
    std::cout << "\n\nThe number of bins in histograms differs!! Must fix!";
    return;
  }
  
  std::cout << "\n\nNumber of bins in each histogram are: " << numBins << std::endl;
  Int_t endBin = numBins;
  std::cout << "\n\nEnd bin is: " << endBin << std::endl;

  Double_t data1Entries = dataMuMinusTrackPt->Integral(startBin, endBin);
  Double_t data2Entries = dataMuPlusTrackPt->Integral(startBin, endBin);
  Double_t mc1Entries = mcMuMinusTrackPt->Integral(startBin, endBin);
  Double_t mc2Entries = mcMuPlusTrackPt->Integral(startBin, endBin);

  std::cout << "\n\nThe total number of pT entries are: " << data1Entries << "\t" << data2Entries << "\t" << mc1Entries << "\t" << mc2Entries << std::endl;

  Double_t dataTot = data1Entries + data2Entries;
  Double_t mcTot = mc1Entries + mc2Entries;
  scale = dataTot/mcTot;
  std::cout << "\n\nTotal data-mc-scale: " << dataTot << "\t" << mcTot << "\t" << scale << std::endl;

  looseCanvas->cd(1);
  scaleCurve(g, mcMuMinusCurve, mcMuPlusCurve, dataMuMinusCurve, dataMuPlusCurve, scale, looseCanvas, rebins, "Curvature", 0);
  looseCanvas->cd(2);
  scaleIt(g, mcMuMinusChi2, mcMuPlusChi2, dataMuMinusChi2, dataMuPlusChi2, scale, looseCanvas, "Chi2", 0);
  looseCanvas->cd(3);
  scaleIt(g, mcMuMinusNdof, mcMuPlusNdof, dataMuMinusNdof, dataMuPlusNdof, scale, looseCanvas, "Ndof", 0);
  looseCanvas->cd(4);
  scaleIt(g, mcMuMinusCharge, mcMuPlusCharge, dataMuMinusCharge, dataMuPlusCharge, scale, looseCanvas, "Charge", 0);
  looseCanvas->cd(5);
  //  scaleIt(g, mcMuMinusTrackPt, mcMuPlusTrackPt, dataMuMinusTrackPt, dataMuPlusTrackPt, scale, looseCanvas, "pT", 0);
  scaleIt(g, mcMuMinusTrackPt, mcMuPlusTrackPt, dataMuMinusTrackPt, dataMuPlusTrackPt, scale, looseCanvas, "pTlog", 1);
  looseCanvas->cd(6);
  scaleIt(g, mcMuMinusTrackEta, mcMuPlusTrackEta, dataMuMinusTrackEta, dataMuPlusTrackEta, scale, looseCanvas, "eta", 0);
  looseCanvas->cd(7);
  scaleIt(g, mcMuMinusTrackPhi, mcMuPlusTrackPhi, dataMuMinusTrackPhi, dataMuPlusTrackPhi, scale, looseCanvas, "phi", 0);
  looseCanvas->cd(8);
  scaleIt(g, mcMuMinusPtRelErr, mcMuPlusPtRelErr, dataMuMinusPtRelErr, dataMuPlusPtRelErr, scale, looseCanvas, "pTRelError", 0);
  looseCanvas->cd(9);
  scaleIt(g, mcMuMinusDz, mcMuPlusDz, dataMuMinusDz, dataMuPlusDz, scale, looseCanvas, "dz", 0);
  looseCanvas->cd(10);
  scaleIt(g, mcMuMinusDxy, mcMuPlusDxy, dataMuMinusDxy, dataMuPlusDxy, scale, looseCanvas, "dxy", 0);
  looseCanvas->cd(11);
  scaleIt(g, mcMuMinusValidHits, mcMuPlusValidHits, dataMuMinusValidHits, dataMuPlusValidHits, scale, looseCanvas, "Valid_Hits", 0);
  looseCanvas->cd(12);
  scaleIt(g, mcMuMinusPixelHits, mcMuPlusPixelHits, dataMuMinusPixelHits, dataMuPlusPixelHits, scale, looseCanvas, "Pixel_Hits", 0);
  looseCanvas->cd(13);
  scaleIt(g, mcMuMinusTrackerHits, mcMuPlusTrackerHits, dataMuMinusTrackerHits, dataMuPlusTrackerHits, scale, looseCanvas, "Tracker_Hits", 0);
  looseCanvas->cd(14);
  scaleIt(g, mcMuMinusValidMuonHits, mcMuPlusValidMuonHits, dataMuMinusValidMuonHits, dataMuPlusValidMuonHits, scale, looseCanvas, "Valid_MuonHits", 0);
  looseCanvas->cd(15);
  scaleIt(g, mcMuMinusMuonStationHits, mcMuPlusMuonStationHits, dataMuMinusMuonStationHits, dataMuPlusMuonStationHits, scale, looseCanvas, "Muon_Station_Hits", 0);
  looseCanvas->cd(16);
  scaleIt(g, mcMuMinusMatchedMuonStations, mcMuPlusMatchedMuonStations, dataMuMinusMatchedMuonStations, dataMuPlusMatchedMuonStations, scale, looseCanvas, "Matched_Muon_Stations", 0);
  looseCanvas->cd(17);
  scaleIt(g, mcMuMinusTrackerLayersWithMeasurement, mcMuPlusTrackerLayersWithMeasurement, dataMuMinusTrackerLayersWithMeasurement, dataMuPlusTrackerLayersWithMeasurement, scale, looseCanvas, "Tracker_Layers_With_Measurement", 0);
  

  g->cd();
  looseCanvas->Write();
  
  g->Write();
  g->Close();


  return;
}

void scaleIt(TFile *file, TH1F *mcHist1, TH1F *mcHist2, TH1F *dataHist1, TH1F *dataHist2, Double_t scale, TCanvas *looseCanvas, std::string const& plotName, int isPt){

  mcHist1->Sumw2();
  mcHist2->Sumw2();
  std::cout << "\n\nThe scale used here is: " << scale << std::endl;
  mcHist1->Scale(scale);
  mcHist2->Scale(scale);
  mcHist1->Add(mcHist2);
  dataHist1->Add(dataHist2);

  //  plotIt(file, mcHist1, mcHist2, dataHist1, dataHist2, looseCanvas);
  plotIt(file, mcHist1, dataHist1, looseCanvas, plotName, isPt);
  //TCanvas *tmpCanvas = new TCanvas(TString(plotName),TString(plotName),1600,900);
  //saveIt(mcHist1, dataHist1, tmpCanvas, plotName);
  return;
}

void scaleCurve(TFile *file, TH1F *mcHist1, TH1F *mcHist2, TH1F *dataHist1, TH1F *dataHist2, Double_t scale, TCanvas *looseCanvas, int rebins, std::string const& plotName, int isPt){

  mcHist1->Sumw2();
  mcHist2->Sumw2();
  std::cout << "\n\nThe scale used here is: " << scale << std::endl;
  mcHist1->Scale(scale);
  mcHist2->Scale(scale);
  mcHist1->Rebin(rebins);
  mcHist2->Rebin(rebins);
  dataHist1->Rebin(rebins);
  dataHist2->Rebin(rebins);
  mcHist1->Add(mcHist2);
  dataHist1->Add(dataHist2);


  plotIt(file, mcHist1, dataHist1, looseCanvas, plotName, isPt);
  return;
}

void plotIt(TFile *file, TH1F *mcHist, TH1F *dataHist, TCanvas *looseCanvas, std::string const& plotName, int isPt){
 
  Double_t mcMax;
  Double_t dataMax;
  Double_t max;

  mcMax = mcHist->GetMaximum();
  dataMax = dataHist->GetMaximum();
  

  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;


  std::cout << "\n\nTotal maxima are: " << mcMax << "\t" << dataMax << "\t" << max << std::endl;

  dataHist->SetMarkerColor(4);
  dataHist->SetLineColor(4);
  dataHist->SetMaximum(max*1.2);
  //  dataHist1->SetMarkerStyle(4);
  dataHist->Draw("ep0");
  looseCanvas->Update();

  mcHist->SetMarkerColor(2);
  mcHist->SetLineColor(2);
  mcHist->SetMaximum(max*1.2);
  //mcHist1->SetMarkerStyle(5);
  mcHist->Draw("ep0sames");
  looseCanvas->Update();

  TCanvas *tmpCanvas = new TCanvas(TString(plotName),TString(plotName),1600,900);
  TPad *tmpPad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);
  tmpCanvas->cd();
  tmpPad->Draw();
  tmpPad->cd();
  dataHist->SetMarkerColor(4);
  dataHist->SetLineColor(4);
  dataHist->SetMaximum(max*1.2);
  dataHist->SetMinimum(0.1);
  //  dataHist1->SetMarkerStyle(4);
  dataHist->Draw("ep0");
  tmpPad->Update();
  tmpCanvas->Update();

  std::cout << "\n\nLog scale set" << std::endl;
  
  mcHist->SetMarkerColor(2);
  mcHist->SetLineColor(2);
  mcHist->SetMaximum(max*1.2);
  //  mcHist->SetMinimum(0.1);
  //mcHist1->SetMarkerStyle(5);
  mcHist->Draw("ep0sames");
  tmpPad->Update();  
  tmpCanvas->Update();
  
  std::cout << "\n\nCalling TPaveStats" << std::endl;
  //  std::cout << "\nHistogram"  << dataHist->GetPrimitives()->Print() << std::endl;
  //std::cout << "\nPad" << tmpPad->GetPrimitives()->Print() << std::endl;
  //std::cout << "\nCanvas" << tmpCanvas->GetPrimitives()->Print() << std::endl;

  tmpPad->cd();
  TPaveStats *stats1 = (TPaveStats*)dataHist->FindObject("stats");
  std::cout << "\nCalled stats" << stats1  << std::endl;
  stats1->SetName(TString("data"+plotName));
  std::cout << "\nSet Name" << std::endl;
  stats1->SetY1NDC(.7);
  std::cout << "\nSet lower y pos" << std::endl;
  stats1->SetY2NDC(.9);
  std::cout << "\nSet upper y pos" << std::endl;
  stats1->SetTextColor(4);
  std::cout << "\nSet text color" << std::endl;
  stats1->SetOptStat(111111111);
  std::cout << "\nSetOptStat" << std::endl;
  tmpPad->Update();
  std::cout << "\nUpdate pad" << std::endl;
  tmpCanvas->Update();

  std::cout << "\n\nCalling TPaveStats again" << std::endl;

  TPaveStats *stats2 = (TPaveStats*)mcHist->FindObject("stats");
  stats2->SetName(TString("MC"+plotName));
  stats2->SetY1NDC(.4);
  stats2->SetY2NDC(.6);
  stats2->SetTextColor(2);
  stats2->SetOptStat(111111111);
  tmpPad->Update();
  tmpCanvas->Update();

  std::cout << "\n\nSetting y-axis to log scale" << std::endl;
  if(isPt == 1) tmpPad->SetLogy(1);
  else tmpPad->SetLogy(0);
  tmpPad->Update();
  tmpCanvas->Update();


  tmpCanvas->SaveAs(TString("kinPlots/"+plotName+".jpg"));
  tmpCanvas->SaveAs(TString("kinPlots/"+plotName+".pdf"));
  tmpCanvas->SaveAs(TString("kinPlots/"+plotName+".png"));
  tmpCanvas->SaveAs(TString("kinPlots/"+plotName+".eps"));


  file->cd();
  dataHist->Write();
  mcHist->Write();
  tmpCanvas->Write();

  return;
}
