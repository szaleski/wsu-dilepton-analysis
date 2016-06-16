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


void plotIt(TH1F *mcHist, TH1F *dataHist, std::string const& plotName, std::string const& oDir, Double_t binWidth, Double_t reBinWidth, double stepSize, int numBin, double chi2, int ndof, double intBefore, double intAfter, double intData);

void makeThePlots(std::string const& inFile1, std::string const& oDir, int numBin){

  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inFile1));
  if(f == 0){
    std::cout << "Error: cannot open file1!\n";
    return;
  }

  std::cout << "\nSuccessfully openedd file 1!\n";
  

  /*  TFile *g = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inFile2));
  if(f == 0){
    std::cout << "Error: cannot open file2!\n";
    return;
  }

  std::cout << "\nSuccessfully opened file2!\n";
  */
  Double_t binWidth = 0.0;
  Double_t reBinWidth = 0.0;
  double stepSize = 0.0025;
  Double_t chi2 = 0.0;
  Int_t ndof =0;
  std::stringstream ss;
  ss.str( std::string() );
  ss.clear();

  if(numBin > 0){
    ss << std::setw(3) << std::setfill('0') << numBin;
    
    TH1F* dataHistScaled = (TH1F*)f->Get("TunePCraftBoth");
    std::cout << "\nGot the first histo\n";
    std::cout << "\nfirst histo:" << std::hex << dataHistScaled << std::dec << std::endl;  
    TH1F* dataHistCut = (TH1F*)f->Get("TunePdataHist_AfterCut");
    std::cout << "\nGot the second histo\n";
    std::cout << "\nSecond histo:" << std::hex << dataHistCut << std::dec << std::endl;  
    TH1F* dataHistRebin = (TH1F*)f->Get("TunePdataHist_Rebinned");
    std::cout << "\nGot the third histo\n";
    std::cout << "\nThird histo:" << std::hex << dataHistRebin << std::dec << std::endl;  

    TH1F* mcHistAdded = (TH1F*)f->Get(TString("TunePMC_Curve_Added_PlusBias"+ss.str()));
    std::cout << "\nGot the added MC histo!";
    std::cout << "\nMC histo:" << std::hex << mcHistAdded << std::dec << std::endl;


    TH1F* mcHistScaled = (TH1F*)f->Get(TString("TunePMCCurvature_Scaled_PlusBias"+ss.str()));
    std::cout << "\nGot the fourth histo\n";
    std::cout << "\nfourth histo:" << std::hex << mcHistScaled << std::dec << std::endl;  
    TH1F* mcHistCut = (TH1F*)f->Get(TString("TunePMC_Curve_AfterCut_PlusBias"+ss.str()));
    std::cout << "\nGot the fifth histo\n";
    std::cout << "\nfifth histo:" << std::hex << mcHistCut << std::dec << std::endl;  
    TH1F* mcHistRebin = (TH1F*)f->Get(TString("TunePMC_Curve_Rebinned_PlusBias"+ss.str()));
    std::cout << "\nGot the last histo\n";
    std::cout << "\nlast histo:" << std::hex << mcHistRebin << std::dec << std::endl;  
  
    TH1F* chi2Hist = (TH1F*)f->Get(TString("TunePChi2vCurve"));
    std::cout << "\nGot the Chi2 histo\n";
    std::cout << "\nchi2 histo:" << std::hex << chi2Hist << std::dec << std::endl;
    
    TH1F* ndofHist = (TH1F*)f->Get(TString("TunePNdof_Plot;2"));
    std::cout << "\nGot the Ndof histo\n";
    std::cout << "\nNdof histo:" << std::hex << ndofHist << std::dec << std::endl;

    
    binWidth = dataHistScaled->GetBinWidth(3);
    std::cout << "\n\nThe binWidth is: " <<  binWidth << std::endl;
    reBinWidth = dataHistRebin->GetBinWidth(3);
    std::cout << "\n\nThe reBinWidth is: " << reBinWidth << std::endl;

    chi2 = chi2Hist->GetBinContent(200+numBin);
    ndof = ndofHist->GetBinContent(200+numBin);
    
    double intData = dataHistScaled->Integral(301, 1300);
    double intBefore = mcHistAdded->Integral(301, 1300);
    double intAfter = mcHistScaled->Integral(301, 1300);
    
    plotIt(mcHistScaled, dataHistScaled, "Scaled", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistCut, dataHistCut, "PtCut", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistRebin, dataHistRebin, "Rebinned", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);


  }


  
  else if(numBin == 0){

    
    TH1F* dataHistScaled = (TH1F*)f->Get("TunePCraftBoth");
    std::cout << "\nGot the first histo\n";
    std::cout << "\nfirst histo:" << std::hex << dataHistScaled << std::dec << std::endl;  
    TH1F* dataHistCut = (TH1F*)f->Get("TunePdataHist_AfterCut");
    std::cout << "\nGot the second histo\n";
    std::cout << "\nSecond histo:" << std::hex << dataHistCut << std::dec << std::endl;  
    TH1F* dataHistRebin = (TH1F*)f->Get("TunePdataHist_Rebinned");
    std::cout << "\nGot the third histo\n";
    std::cout << "\nThird histo:" << std::hex << dataHistRebin << std::dec << std::endl;  

    TH1F* mcHistAdded = (TH1F*)f->Get(TString("TunePMC_Curve_Added_NoBias"));
    std::cout << "\nGot the added MC histo!";
    std::cout << "\nMC histo:" << std::hex << mcHistAdded << std::dec << std::endl;

    TH1F* mcHistScaled = (TH1F*)f->Get(TString("TunePMCCurvature_Scaled_NoBias"));
    std::cout << "\nGot the fourth histo\n";
    std::cout << "\nfourth histo:" << std::hex << mcHistScaled << std::dec << std::endl;  
    TH1F* mcHistCut = (TH1F*)f->Get(TString("TunePMC_Curve_AfterCut_NoBias"));
    std::cout << "\nGot the fifth histo\n";
    std::cout << "\nfifth histo:" << std::hex << mcHistCut << std::dec << std::endl;  
    TH1F* mcHistRebin = (TH1F*)f->Get(TString("TunePMC_Curve_Rebinned_NoBias"));
    std::cout << "\nGot the last histo\n";
    std::cout << "\nlast histo:" << std::hex << mcHistRebin << std::dec << std::endl;  
  
    TH1F* chi2Hist = (TH1F*)f->Get(TString("TunePChi2vCurve"));
    std::cout << "\nGot the Chi2 histo\n";
    std::cout << "\nchi2 histo:" << std::hex << chi2Hist << std::dec << std::endl;
    
    TH1F* ndofHist = (TH1F*)f->Get(TString("TunePNdof_Plot;2"));
    std::cout << "\nGot the Ndof histo\n";
    std::cout << "\nNdof histo:" << std::hex << ndofHist << std::dec << std::endl;

    
    binWidth = dataHistScaled->GetBinWidth(3);
    std::cout << "\n\nThe binWidth is: " <<  binWidth << std::endl;
    reBinWidth = dataHistRebin->GetBinWidth(3);
    std::cout << "\n\nThe reBinWidth is: " << reBinWidth << std::endl;

    
    chi2 = chi2Hist->GetBinContent(200 + numBin);
    ndof = ndofHist->GetBinContent(200 + numBin);

    double intData = dataHistScaled->Integral(301, 1300);
    double intBefore = mcHistAdded->Integral(301, 1300);
    double intAfter = mcHistScaled->Integral(301, 1300);
    
    plotIt(mcHistScaled, dataHistScaled, "Scaled", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistCut, dataHistCut, "PtCut", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistRebin, dataHistRebin, "Rebinned", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);


  }
  
  
  else if(numBin < 0){
    int    numBinMinus = abs(numBin);
    ss << std::setw(3) << std::setfill('0') << numBinMinus;
    
    TH1F* dataHistScaled = (TH1F*)f->Get("TunePCraftBoth");
    std::cout << "\nGot the first histo\n";
    std::cout << "\nfirst histo:" << std::hex << dataHistScaled << std::dec << std::endl;  
    TH1F* dataHistCut = (TH1F*)f->Get("TunePdataHist_AfterCut");
    std::cout << "\nGot the second histo\n";
    std::cout << "\nSecond histo:" << std::hex << dataHistCut << std::dec << std::endl;  
    TH1F* dataHistRebin = (TH1F*)f->Get("TunePdataHist_Rebinned");
    std::cout << "\nGot the third histo\n";
    std::cout << "\nThird histo:" << std::hex << dataHistRebin << std::dec << std::endl;  
    
    TH1F* mcHistAdded = (TH1F*)f->Get(TString("TunePMC_Curve_Added_MinusBias"+ss.str()));
    std::cout << "\nGot the added MC histo!";
    std::cout << "\nMC histo:" << std::hex << mcHistAdded << std::dec << std::endl;

    
    TH1F* mcHistScaled = (TH1F*)f->Get(TString("TunePMC_Curvature_Scaled_MinusBias"+ss.str()));
    std::cout << "\nGot the fourth histo\n";
    std::cout << "\nfourth histo:" << std::hex << mcHistScaled << std::dec << std::endl;  
    TH1F* mcHistCut = (TH1F*)f->Get(TString("TunePMC_Curve_AfterCut_MinusBias"+ss.str()));
    std::cout << "\nGot the fifth histo\n";
    std::cout << "\nfifth histo:" << std::hex << mcHistCut << std::dec << std::endl;  
    TH1F* mcHistRebin = (TH1F*)f->Get(TString("TunePMC_Curve_Rebinned_MinusBias"+ss.str()));
    std::cout << "\nGot the last histo\n";
    std::cout << "\nlast histo:" << std::hex << mcHistRebin << std::dec << std::endl;  
  
    TH1F* chi2Hist = (TH1F*)f->Get(TString("TunePChi2vCurve"));
    std::cout << "\nGot the Chi2 histo\n";
    std::cout << "\nchi2 histo:" << std::hex << chi2Hist << std::dec << std::endl;
    
    TH1F* ndofHist = (TH1F*)f->Get(TString("TunePNdof_Plot;2"));
    std::cout << "\nGot the Ndof histo\n";
    std::cout << "\nNdof histo:" << std::hex << ndofHist << std::dec << std::endl;

    
    binWidth = dataHistScaled->GetBinWidth(3);
    std::cout << "\n\nThe binWidth is: " <<  binWidth << std::endl;
    reBinWidth = dataHistRebin->GetBinWidth(3);
    std::cout << "\n\nThe reBinWidth is: " << reBinWidth << std::endl;

    
    chi2 = chi2Hist->GetBinContent(200 - numBin);
    ndof = ndofHist->GetBinContent(200 - numBin);

    double intData = dataHistScaled->Integral(301, 1300);
    double intBefore = mcHistAdded->Integral(301, 1300);
    double intAfter = mcHistScaled->Integral(301, 1300);
    
    plotIt(mcHistScaled, dataHistScaled, "Scaled", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistCut, dataHistCut, "PtCut", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistRebin, dataHistRebin, "Rebinned", "ScalePlots", binWidth, reBinWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
  

    
  }

  /*
  TH1F* pt = (TH1F*)g->Get("looseMuLowerMinusTrackPt");
  TH1F* eta = (TH1F*)g->Get("looseMuLowerMinusTrackEta");
  TH1F* phi = (TH1F*)g->Get("looseMuLowerMinusTrackPhi");
  TH1F* relPt = (TH1F*)g->Get("looseMuLowerMinus");
  TH1F* pixel = (TH1F*(g->Get("looseMuLowerMinus");
  */



  return;
}

void plotIt(TH1F *mcHist, TH1F *dataHist, std::string const& plotName, std::string const& oDir, Double_t binWidth, Double_t reBinWidth, double stepSize, int numBin, double chi2, int ndof, double intBefore, double intAfter, double intData){
  std::cout << "\nMade it into the function call!\n";
 
  Double_t mcMax;
  Double_t dataMax;
  Double_t max;
  double deltaKappa = 0.0;
  deltaKappa = stepSize*numBin;
  std::cout << "\n\nIn function binWidth-reBinWidth are: " << binWidth << "-" << reBinWidth << std::endl;

  std::stringstream legendText2;
  std::stringstream legendText4;
  std::stringstream bwidth;
  std::stringstream rbWidth;
  std::stringstream legendText6;
  std::stringstream legendText8;


  mcMax = mcHist->GetMaximum();
  dataMax = dataHist->GetMaximum();
  

  if(mcMax > dataMax) max = mcMax;
  else if(mcMax < dataMax)  max = dataMax;
  else if(mcMax == dataMax) max = mcMax;
  else max = -1000;


  std::cout << "\n\nTotal maxima are: " << mcMax << "\t" << dataMax << "\t" << max << std::endl;
  bwidth.str( std::string() );
  bwidth.clear();
  bwidth << binWidth;
  
  rbWidth.str( std::string() );
  rbWidth.clear();
  rbWidth << reBinWidth;

  std::stringstream addedBias;
  addedBias.str( std::string() );
  addedBias.clear();
  addedBias << deltaKappa;

  TCanvas *tmpCanvas = new TCanvas(TString(plotName),TString(plotName),1600,900);
  TPad *tmpPad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);
  tmpCanvas->cd();
  tmpPad->Draw();
  tmpPad->cd();
  dataHist->Draw("ep0");
  dataHist->SetMarkerColor(4);
  dataHist->SetLineColor(4);
  dataHist->SetMaximum(max*1.2);
  dataHist->SetMinimum(0.1);
  dataHist->GetXaxis()->SetTitle("#kappa[c/TeV]");
  dataHist->GetYaxis()->SetTitle(TString("N_#mu/"+rbWidth.str()+"[c/TeV]"));
  dataHist->SetTitle(TString("#Delta#kappa = "+addedBias.str() + " [c/TeV]") );
  dataHist->GetXaxis()->SetRangeUser(-5.0, 5.0);
  dataHist->SetStats(0);
  tmpPad->Update();
  tmpCanvas->Update();

  mcHist->SetMarkerColor(2);
  mcHist->SetLineColor(2);
  mcHist->SetMaximum(max*1.2);
  mcHist->SetStats(0);
  mcHist->Draw("ep0sames");
  tmpPad->Update();  
  tmpCanvas->Update();

  double rchi2 = chi2/ndof;
  
  TLegend* leg = new TLegend(0.2, 0.7, 0.7, 0.9);
  legendText2.str( std::string() );
  legendText2.clear();
  legendText2 << "Raw Curvature Hist bin width: " << binWidth << " [c/TeV]";
  legendText4.str( std::string() );
  legendText4.clear();
  legendText4 << "#chi^{2}/ndof = " << chi2 << "/" << ndof << " = " << rchi2;
  legendText6.str( std::string() );
  legendText6.clear();
  legendText6 << "MC integral before:after scaling: " << intBefore << ":" << intAfter;
  legendText8.str( std::string() );
  legendText8.clear();
  legendText8 << "Data integral: " << intData;


  leg->AddEntry((TObject*) 0, TString(legendText2.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText4.str()), "");
  leg->AddEntry(mcHist, TString(legendText6.str()), "lpe");
  leg->AddEntry(dataHist, TString(legendText8.str()), "lpe");

  leg->SetTextSize(0.030);
  leg->Draw();
  tmpPad->Update();
  tmpCanvas->Update();


  std::cout << "\n\nCalling TPaveStats" << std::endl;

  tmpPad->cd();
  /*  TPaveStats *stats1 = (TPaveStats*)dataHist->FindObject("stats");
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
  */

  tmpCanvas->SaveAs(TString(oDir+"/curvature_"+plotName+addedBias.str()+"New.png"));

  tmpCanvas->SaveAs(TString("~/public/html/cosmics/curvature_"+plotName+addedBias.str()+"New.png"));

  return;
}
