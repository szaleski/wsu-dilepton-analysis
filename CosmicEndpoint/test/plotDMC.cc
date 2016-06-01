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

void getHistos(TFile *file1, TFile *file2, TFile *file3, TFile* outFile, int rebins, std::string const& histBase, std::string const& kinVar, double scale);
void plotIt(TH1F *dataHist1, TH1F *mcHist1, std::string const& kinVar, std::string const & plotName);
void plotPtQSep(TFile *file1, TFile *file2, TFile file3, TFile* outFile, int rebins, std::string const& histBase, std::string const& kinVar, double scale);

double plotPt(TFile *file1, TFile *file2, TFile* file3,  TFile* outFile, int rebins, std::string const& histBase, std::string const& kinVar);

void plotDMC(std::string const& file1, std::string const& file2, std::string const& file3, int rebins, std::string const& histBase){

  TFile *g;
  g = new TFile("kinPlots.root","RECREATE");
  
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

  TFile *z = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+file3));

  if(z == 0){
    std::cout << "Error: cannot open file3! \n";
    return;
  }
  std::cout << "\nsuccessfully opened file3!\n";



  double scale =  plotPt(f, h, g, rebins, histBase, "TrackPt");

  getHistos(f, h, g, z, rebins, histBase, "TrackPt", scale);

  getHistos(f, h, g, z, rebins, histBase, "TrackEta", scale);

  getHistos(f, h, g, z, rebins, histBase, "TrackPhi", scale);
  
  getHistos(f, h, g, z, rebins, histBase, "PtRelErr", scale);

  getHistos(f, h, g, z, rebins, histBase, "PixelHits", scale);
  
  getHistos(f, h, g, z, rebins, histBase, "MuonStationHits", scale);

  int *res = 0;
  std::cout << res << std::endl;
  std::cout << *res << std::endl;

  std::cout << "\n\n\n\n\n\n\n\n";


  return;

}

double plotPt(TFile *file1, TFile *file2, TFile *file3, TFile* outFile, int rebins, std::string const& histBase, std::string const& kinVar){


  Double_t dataInt = 0.0;
  Double_t mcInt = 0.0;
  Double_t inScale = 0.0;
  Int_t dataNum = 0;
  Int_t mcNum = 0;
  Double_t numScale = 0.0;
  
  TH1F *dataHist1 = (TH1F*)file2->Get(TString(histBase+"Minus"+kinVar));
  TH1F *dataHist2 = (TH1F*)file2->Get(TString(histBase+"Plus"+kinVar));
  TH1F *mcHist1 = (TH1F*)file1->Get(TString(histBase+"Minus"+kinVar));
  TH1F *mcHist2 = (TH1F*)file1->Get(TString(histBase+"Plus"+kinVar));

  outFile->cd();
  dataHist1->SetName(TString("data"+kinVar+"Minus"));
  dataHist1->Write();
  dataHist2->SetName(TString("data"+kinVar+"Plus"));
  dataHist2->Write();
  mcHist1->SetName(TString("mc"+kinVar+"Minus"));
  mcHist1->Write();
  mcHist2->SetName(TString("mc"+kinVar+"Plus"));
  mcHist2->Write();

  mcHist1->Sumw2();
  mcHist2->Sumw2();
  dataHist1->Sumw2();
  dataHist2->Sumw2();
  mcHist1->Add(mcHist2);
  dataHist1->Add(dataHist2);
  mcHist1->Sumw2();
  dataHist1->Sumw2();
  dataHist1->SetName(TString("data"+kinVar+"Added"));
  dataHist1->Write();
  mcHist1->SetName(TString("mc"+kinVar+"Added"));
  mcHist1->Sumw2();
  dataHist1->Sumw2();


  dataInt = dataHist1->Integral();
  mcInt = mcHist1->Integral();
  double intScale = dataInt/mcInt;
  
  dataNum = dataHist1->GetEntries();
  mcNum = mcHist1->GetEntries();
  numScale = dataNum/mcNum;

  mcHist1->Scale(intScale);
  mcHist1->SetName(TString("mc"+kinVar+"Scaled"));
  mcHist1->Write();

  
  double ptWidth = dataHist1->GetBinWidth(3);
  double totWidth = 5*rebins*ptWidth;
  std::cout << "\npT bin width is: " << totWidth << std::endl;
  std::stringstream ss;
  ss.str( std::string() );
  ss.clear();
  ss << totWidth;

  int cutBin = 100/ptWidth;
  for(int j = 1; j < (cutBin+1); j++){
    dataHist1->SetBinContent(j+1, 0);
    dataHist1->SetBinError(j+1, 0);
    mcHist1->SetBinContent(j+1, 0);
    mcHist1->SetBinError(j+1, 0);

  }

  dataHist1->SetName(TString("data"+kinVar+"Cut"));
  dataHist1->Write();
  mcHist1->SetName(TString("mc"+kinVar+"Cut"));
  mcHist1->Write();


  dataHist1->Sumw2();
  mcHist1->Sumw2();
  dataHist1->Rebin(5*rebins);
  mcHist1->Rebin(5*rebins);
  dataHist1->SetName(TString("data"+kinVar+"Rebinned"));
  dataHist1->Write();
  mcHist1->SetName(TString("mc"+kinVar+"Rebinned"));
  mcHist1->Write();
  /*
  double mcMax = mcHist1->GetMaximum();
  double dataMax = dataHist1->GetMaximum();
  double max = 0.0;

  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;


  TCanvas *tmpCanvas = new TCanvas(TString("Data:MC_"+kinVar+"_Comparison"), TString("Data:MC_"+kinVar+"_Comparison"), 1600, 900);
  TPad *tmpPad1 = new TPad("pad", "pad", 0.0, 0.31, 1.0, 1.0);
  TPad *tmpPad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.30);
  
  
  tmpCanvas->cd();
  tmpPad1->Draw();
  tmpPad1->cd();
  dataHist1->Draw("ep0");
  //dataHist1->SetStats(0);
  dataHist1->SetMarkerColor(4);
  dataHist1->SetLineColor(4);
  dataHist1->SetMaximum(max*1.2);
  dataHist1->SetMinimum(0.1);
  tmpPad1->SetLogy(1);
  dataHist1->GetXaxis()->SetTitle("pT (GeV)");
  dataHist1->GetYaxis()->SetTitle(TString("N_#mu/"+ss.str()+"[GeV]"));
  dataHist1->SetTitle(TString(kinVar+"_Distribution"));
  tmpPad1->Update();

  tmpCanvas->Update();

  mcHist1->SetMarkerColor(2);
  mcHist1->SetLineColor(2);
  mcHist1->Draw("ep0sames");
  //  mcHist1->SetStats(0);
  tmpPad1->Update();
  tmpCanvas->Update();

  TPaveStats *dataStats = (TPaveStats*)dataHist1->FindObject("stats");
  dataStats->SetName("data_TrackPt");
  dataStats->SetY1NDC(0.7);
  dataStats->SetY2NDC(0.9);
  dataStats->SetTextColor(4);
  dataStats->SetOptStat(11111111);
  tmpPad1->Update();
  tmpCanvas->Update();
  
  TPaveStats *mcStats = (TPaveStats*)mcHist1->FindObject("stats");
  mcStats->SetName("mc_TrackPt");
  mcStats->SetY1NDC(0.5);
  mcStats->SetY2NDC(0.7);
  mcStats->SetTextColor(2);
  mcStats->SetOptStat(11111111);
  tmpPad1->Update();
  tmpCanvas->Update();

  dataHist1->Sumw2();
  mcHist1->Sumw2();
  TH1F *h_ratio = (TH1F*)dataHist1->Clone("numerator");
  h_ratio->Divide(dataHist1, mcHist1, 1.0, 1.0, "");
  
  tmpCanvas->cd();
  //tmpPad2->cd();
  tmpPad2->Draw();
  tmpPad2->cd();
  h_ratio->Draw("ep0");
  h_ratio->GetXaxis()->SetTitle("pT (GeV)");
  h_ratio->SetTitle(TString(kinVar+"_Distribution_Ratio"));
  //h_ratio->SetStats(0);
  tmpPad2->SetGridy(1);
  tmpPad2->Update();
  tmpCanvas->Update();

  tmpCanvas->SaveAs(TString("~/public/html/cosmics/"+kinVar+"_KinPlotNew.png"));
  tmpCanvas->SaveAs(TString("kinPlots/"+kinVar+"_KinPlotNew.png"));
  */
  return intScale;

}


void getHistos(TFile *file1, TFile *file2, TFile *file3, TFile* outFile, int rebins, std::string const& histBase, std::string const& kinVar, double scale){



  Double_t dataInt = 0.0;
  Double_t mc1Int = 0.0;
  Double_t mc2Int = 0.0;
  Double_t inScale = 0.0;
  Int_t dataNum = 0;
  Int_t mcNum = 0;
  Double_t numScale = 0.0;
  
  TH1F *dataHist1 = (TH1F*)file1->Get(TString(histBase+"Minus"+kinVar));
  TH1F *dataHist2 = (TH1F*)file1->Get(TString(histBase+"Plus"+kinVar));
  TH1F *mcHist1 = (TH1F*)file2->Get(TString(histBase+"Minus"+kinVar));
  TH1F *mcHist2 = (TH1F*)file2->Get(TString(histBase+"Plus"+kinVar));
  TH1F *mcHist3 = (TH1F*)file3->Get(TString(histBase+"Minus"+kinVar));
  TH1F *mcHist4 = (TH1F*)file3->Get(TString(histBase+"Plus"+kinVar));

  TH1F *dataHistMinus = (TH1F*)file3->Get(TString(histBase+"Minus"+kinVar));
  TH1F *dataHistPlus = (TH1F*)file3->Get(TString(histBase+"Plus"+kinVar));
  TH1F *mcHist1Minus = (TH1F*)file1->Get(TString(histBase+"Minus"+kinVar));
  TH1F *mcHist1Plus = (TH1F*)file1->Get(TString(histBase+"Plus"+kinVar));
  TH1F *mcHist2Minus = (TH1F*)file2->Get(TString(histBase+"Minus"+kinVar));
  TH1F *mcHist2Plus = (TH1F*)file2->Get(TString(histBase+"Plus"+kinVar));


  int  numBins = dataHistMinus->GetNbinsX();
  double binEdges[] = {0, 50, 100, 150, 200, 250, 300, 400, 500, 750, 1000, 3000};

  outFile->cd();

  dataHistMinus->SetName(TString("data"+kinVar+"Minus"));
  dataHistPlus->SetName(TString("data"+kinVar+"Plus"));
  mcHist1Minus->SetName(TString("mc1"+kinVar+"Minus"));
  mcHist1Plus->SetName(TString("mc1"+kinVar+"Plus"));
  mcHist2Minus->SetName(TString("mc2"+kinVar+"Minus"));
  mcHist2Plus->SetName(TString("mc2"+kinVar+"Plus"));
  dataHistMinus->Sumw2();
  dataHistPlus->Sumw2();
  mcHist1Minus->Sumw2();
  mcHist1Plus->Sumw2();
  mcHist2Minus->Sumw2();
  mcHist2Plus->Sumw2();

  double dataMinusInt =  dataHistMinus->Integral();
  double dataPlusInt =  dataHistPlus->Integral();
  double mcMinus1Int =  mcHist1Minus->Integral();
  double mcPlus1Int =  mcHist1Plus->Integral();  
  double mcMinus2Int =  mcHist2Minus->Integral();
  double mcPlus2Int =  mcHist2Plus->Integral();  

  double scale1Minus = dataMinusInt/mcMinus1Int;
  double scale1Plus = dataPlusInt/mcPlus1Int;
  double scale2Minus = dataMinusInt/mcMinus2Int;
  double scale2Plus = dataPlusInt/mcPlus2Int;

  
  mcHist1Minus->Scale(scale1Minus);
  mcHist1Minus->SetName(TString("mc1Minus"+kinVar+"Scaled"));
  mcHist1Minus->Write();
  mcHist1Plus->Scale(scale1Plus);
  mcHist1Plus->SetName(TString("mc1Plus"+kinVar+"Scaled"));
  mcHist1Plus->Write();
  mcHist2Minus->Scale(scale2Minus);
  mcHist2Minus->SetName(TString("mc2Minus"+kinVar+"Scaled"));
  mcHist2Minus->Write();
  mcHist2Plus->Scale(scale2Plus);
  mcHist2Plus->SetName(TString("mc2Plus"+kinVar+"Scaled"));
  mcHist2Plus->Write();

  dataHistMinus->Sumw2();
  dataHistPlus->Sumw2();
  mcHist1Minus->Sumw2();
  mcHist1Plus->Sumw2();

  mcHist2Minus->Sumw2();
  mcHist2Plus->Sumw2();


  if(kinVar == "TrackPt"){
    dataHistMinus->Rebin(11, TString("dataMinus"+kinVar+"Rebinned"),binEdges);
    dataHistMinus->Write();
    dataHistPlus->Rebin(11, TString("dataPlus"+kinVar+"Rebinned"), binEdges);
    dataHistPlus->Write();
    mcHist1Minus->Rebin(11,TString("mc1Minus"+kinVar+"Rebinned"), binEdges);
    mcHist1Minus->Write();
    mcHist1Plus->Rebin(11, TString("mc1Plus"+kinVar+"Rebinned"), binEdges);
    mcHist1Plus->Write();
    mcHist2Minus->Rebin(11,TString("mc2Minus"+kinVar+"Rebinned"), binEdges);
    mcHist2Minus->Write();
    mcHist2Plus->Rebin(11, TString("mc2Plus"+kinVar+"Rebinned"), binEdges);
    mcHist2Plus->Write();


    plotIt(dataHistPlus, dataHistMinus, kinVar, "Data_Data");
    plotIt(mcHist1Plus, mcHist1Minus, kinVar, "Start_Start");
    plotIt(mcHist2Plus, mcHist2Minus, kinVar, "Start_Start");

  }
  else{

    dataHistMinus->Rebin(rebins);
    dataHistMinus->SetName(TString("dataMinus"+kinVar+"Rebinned"));
    dataHistMinus->Write();
    dataHistPlus->Rebin(rebins);
    dataHistPlus->SetName(TString("dataPlus"+kinVar+"Rebinned"));
    dataHistPlus->Write();
    mcHist1Minus->Rebin(rebins);
    mcHist1Minus->SetName(TString("mc1Minus"+kinVar+"Rebinned"));
    mcHist1Minus->Write();
    mcHist1Plus->Rebin(rebins);
    mcHist1Plus->SetName(TString("mc1Plus"+kinVar+"Rebinned"));
    mcHist1Plus->Write();
    mcHist2Minus->Rebin(rebins);
    mcHist2Minus->SetName(TString("mc2Minus"+kinVar+"Rebinned"));
    mcHist2Minus->Write();
    mcHist2Plus->Rebin(rebins);
    mcHist2Plus->SetName(TString("mc2Plus"+kinVar+"Rebinned"));
    mcHist2Plus->Write();


  }


  dataHist1->SetName(TString("data"+kinVar+"Minus"));
  dataHist1->Write();
  dataHist2->SetName(TString("data"+kinVar+"Plus"));
  dataHist2->Write();
  mcHist1->SetName(TString("mc1"+kinVar+"Minus"));
  mcHist1->Write();
  mcHist2->SetName(TString("mc1"+kinVar+"Plus"));
  mcHist2->Write();
  mcHist3->SetName(TString("mc2"+kinVar+"Minus"));
  mcHist3->Write();
  mcHist4->SetName(TString("mc2"+kinVar+"Plus"));
  mcHist4->Write();


  mcHist1->Sumw2();
  mcHist2->Sumw2();
  dataHist1->Sumw2();
  dataHist2->Sumw2();
  mcHist1->Add(mcHist2);
  dataHist1->Add(dataHist2);
  mcHist1->Sumw2();
  dataHist1->Sumw2();
  dataHist1->SetName(TString("data"+kinVar+"Added"));
  dataHist1->Write();
  mcHist1->SetName(TString("mc"+kinVar+"Added"));
  dataInt = dataHist1->Integral();
  mcInt = mcHist1->Integral();
  double intScale = dataInt/mcInt;


  dataNum = dataHist1->GetEntries();
  mcNum = mcHist1->GetEntries();
  numScale = dataNum/mcNum;

  mcHist1->Scale(intScale);
  mcHist1->SetName(TString("mc"+kinVar+"Scaled"));
  mcHist1->Write();

  dataHist1->Sumw2();
  mcHist1->Sumw2();
  dataHist1->Rebin(rebins);
  mcHist1->Rebin(rebins);
  dataHist1->SetName(TString("data"+kinVar+"Rebinned"));
  dataHist1->Write();
  mcHist1->SetName(TString("mc"+kinVar+"Rebinned"));
  mcHist1->Write();

  plotIt(dataHistMinus, mcHistMinus, kinVar, "Data_Start_Minus");
  plotIt(dataHistPlus, mcHistPlus, kinVar, "Data_Start_Plus");
  std::cout << "\n\nMade it to combined function call! " << std::endl;
  //  std::cout << "\ndataHist1" << std::hex << dataHist1 << std::dec << std::endl;
  //std::cout << "\nmcHist1" << std::hex << mcHist1 << std::dec << std::endl;
  plotIt(dataHist1, mcHist1, kinVar, "Data_Start_Scaled");

  return;

}

void plotIt(TH1F *dataHist1, TH1F *mcHist1, std::string const& kinVar, std::string const & plotName){

  double mcMax = mcHist1->GetMaximum();
  double dataMax = dataHist1->GetMaximum();
  double max = 0.0;

  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;

  std::cout << "\n\nSet Maximum!" << std::endl;

  TCanvas *tmpCanvas = new TCanvas(TString("Data:MC_"+kinVar+"_Comparison"), TString("Data:MC_"+kinVar+"_Comparison"), 1600, 900);
  TPad *tmpPad1 = new TPad("pad1", "pad1", 0.0, 0.31, 1.0, 1.0);
  TPad *tmpPad2 = new TPad("pad2", "pad2", 0.0, 0.0, 1.0, 0.30);
  
  
  tmpCanvas->cd();
  
  tmpPad1->Draw();
  tmpPad1->cd();
  dataHist1->Draw("ep0");
  dataHist1->SetMarkerColor(4);
  dataHist1->SetLineColor(4);
  dataHist1->SetMaximum(max*1.2);
  dataHist1->SetMinimum(0.1);
  dataHist1->SetStats(0);
  dataHist1->GetYaxis()->SetTitle("N_#mu");
  if(kinVar == "TrackEta"){
    dataHist1->GetXaxis()->SetTitle("#eta");
    dataHist1->GetXaxis()->SetRangeUser(-1.5, 1.5);
  }
  else if(kinVar == "TrackPhi"){
    dataHist1->GetXaxis()->SetTitle("#phi");
    dataHist1->GetXaxis()->SetRangeUser(-4, 1);
  }
  else if((kinVar == "PixelHits") || (kinVar == "MuonStationHits")){
    dataHist1->GetXaxis()->SetTitle("Hits");
    dataHist1->GetXaxis()->SetRangeUser(-0.5, 7);
  }
  /*  else if(kinVar == "TrackPt"){
    dataHist1->GetXaxis()->SetTitle("TrackPt");
    dataHist1->GetXaxis()->SetRangeUser(0, 1);
  }
  */
  else if(kinVar == "PtRelErr")dataHist1->GetXaxis()->SetRangeUser(0, 0.3);
  //  dataHist1->GetYaxis()->SetTitle(TString("N_#mu/"+ss.str()+"[GeV]"));
  dataHist1->SetTitle(TString(kinVar+plotName+"_Distribution"));
  
  tmpPad1->Update();
  
  tmpCanvas->Update();
  std::cout << "\n\nDrew data Histogram! " << std::endl;

  mcHist1->SetMarkerColor(2);
  mcHist1->SetLineColor(2);
  mcHist1->Draw("ep0sames");
  mcHist1->SetStats(0);
  tmpPad1->Update();
  tmpCanvas->Update();

  std::cout << "\n\nDrew MC histogram!" << std::endl;

  /*TPaveStats *dataStats = (TPaveStats*)dataHist1->FindObject("stats");
  dataStats->SetName(TString("data_"+kinVar));
  dataStats->SetY1NDC(0.7);
  dataStats->SetY2NDC(0.9);
  dataStats->SetTextColor(4);
  dataStats->SetOptStat(11111111);
  tmpPad1->Update();
  tmpCanvas->Update();

  std::cout << "\n\nCreated 1st statsBox!" << std::endl;
  
  TPaveStats *mcStats = (TPaveStats*)mcHist1->FindObject("stats");
  mcStats->SetName(TString("mc_"+kinVar));
  mcStats->SetY1NDC(0.5);
  mcStats->SetY2NDC(0.7);
  mcStats->SetTextColor(2);
  mcStats->SetOptStat(11111111);
  tmpPad1->Update();
  tmpCanvas->Update();
  std::cout << "\n\nCreated 2nd statsBox!" << std::endl;
  
  */

  dataHist1->Sumw2();
  mcHist1->Sumw2();
  TH1F *h_ratio = (TH1F*)dataHist1->Clone("numerator");
  h_ratio->Divide(dataHist1, mcHist1, 1.0, 1.0, "");
  

  tmpCanvas->cd();
  tmpPad2->Draw();
  tmpPad2->cd();
  h_ratio->Draw("ep0");
  h_ratio->GetXaxis()->SetTitle("#eta");
  h_ratio->SetTitle(TString(kinVar+plotName+"_Distribution_Ratio"));
  tmpPad2->SetGridy(1);
  tmpPad2->Update();
  tmpCanvas->Update();
  
  tmpCanvas->SaveAs(TString("~/public/html/cosmics/"+kinVar+plotName+"_KinPlotNew.png"));
  tmpCanvas->SaveAs(TString("kinPlots/"+kinVar+plotName+"_KinPlotNew.png"));

  return;

}
