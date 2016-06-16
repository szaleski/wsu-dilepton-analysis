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


void plotIt(TH1F *mcHist, TH1F *dataHist, std::string const& plotName, std::string const& oDir, Double_t binWidth, double stepSize, int numBin, double chi2, int ndof, double intBefore, double intAfter, double intData);

void makeThePlotsSS(std::string const& inFile1, std::string const& inFile2, std::string const& oDir, int numBin){

  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inFile1));
  if(f == 0){
    std::cout << "Error: cannot open file1!\n";
    return;
  }

  std::cout << "\nSuccessfully openedd file 1!\n";
  

  TFile *g = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inFile2));
  if(f == 0){
    std::cout << "Error: cannot open file2!\n";
    return;
  }

  std::cout << "\nSuccessfully opened file2!\n";

  Double_t binWidth = 0.0;
  double stepSize = 0.0005;
  Double_t chi2 = 0.0;
  Int_t ndof =0;
  std::stringstream ss;
  ss.str( std::string() );
  ss.clear();

  if(numBin > 0){
    if(numBin == 1000) ss << std::setw(4) << numBin;
    else ss << std::setw(3) << std::setfill('0') << numBin;
    
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
    
    chi2 = chi2Hist->GetBinContent(1000+numBin);
    ndof = ndofHist->GetBinContent(1000+numBin);
    
    double intData = dataHistScaled->Integral(251, 1250);
    double intBefore = mcHistAdded->Integral(251, 1250);
    double intAfter = mcHistScaled->Integral(251, 1250);
    
    plotIt(mcHistScaled, dataHistScaled, "Scaled", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistCut, dataHistCut, "PtCut", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    binWidth = dataHistRebin->GetBinWidth(3);
    plotIt(mcHistRebin, dataHistRebin, "Rebinned", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);


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
    
    chi2 = chi2Hist->GetBinContent(1000 + numBin);
    ndof = ndofHist->GetBinContent(1000 + numBin);

    double intData = dataHistScaled->Integral(251, 1250);
    double intBefore = mcHistAdded->Integral(251, 1250);
    double intAfter = mcHistScaled->Integral(251, 1250);
    
    plotIt(mcHistScaled, dataHistScaled, "Scaled", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistCut, dataHistCut, "PtCut", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    binWidth = dataHistRebin->GetBinWidth(3);
    plotIt(mcHistRebin, dataHistRebin, "Rebinned", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);


  }
  
  
  else if(numBin < 0){
    int    numBinMinus =(abs(1000 + numBin));
    if(numBinMinus == 1000) ss << std::setw(4) << numBinMinus;
    else ss << std::setw(3) << std::setfill('0') << numBinMinus;
    
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
    
    chi2 = chi2Hist->GetBinContent(1000 - numBin);
    ndof = ndofHist->GetBinContent(1000 - numBin);

    double intData = dataHistScaled->Integral(251, 1250);
    double intBefore = mcHistAdded->Integral(251, 1250);
    double intAfter = mcHistScaled->Integral(251, 1250);
    
    plotIt(mcHistScaled, dataHistScaled, "Scaled", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    plotIt(mcHistCut, dataHistCut, "PtCut", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
    binWidth = dataHistRebin->GetBinWidth(3);
    plotIt(mcHistRebin, dataHistRebin, "Rebinned", "ScalePlots", binWidth, stepSize, numBin, chi2, ndof, intBefore, intAfter, intData);
  

    
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

void plotIt(TH1F *mcHist, TH1F *dataHist, std::string const& plotName, std::string const& oDir, Double_t binWidth, double stepSize, int numBin, double chi2, int ndof, double intBefore, double intAfter, double intData){
  std::cout << "\nMade it into the function call!\n";
 
  Double_t mcMax;
  Double_t dataMax;
  Double_t max;
  double deltaKappa = 0.0;
  deltaKappa = stepSize*numBin;
  std::stringstream legendText1;
  std::stringstream legendText2;
  std::stringstream legendText3;
  std::stringstream legendText4;
  std::stringstream legendText5;
  std::stringstream bwidth;
  std::stringstream legendText6;
  std::stringstream legendText7;
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
  dataHist->GetYaxis()->SetTitle(TString("N_#mu/"+bwidth.str()+"[c/TeV]"));
  dataHist->SetTitle(TString(plotName+" #kappa Distribution"));
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
  
  TLegend* leg = new TLegend(0.2, 0.7, 0.7, 0.9);
  legendText1.str( std::string() );
  legendText1.clear();
  legendText1 << "Bias step: " << stepSize << " [c/TeV]";
  legendText2.str( std::string() );
  legendText2.clear();
  legendText2 << "Curvature Hist bin width: " << binWidth << " [c/TeV]";
  legendText3.str( std::string() );
  legendText3.clear();
  legendText3 << "#Delta#kappa = " << deltaKappa << " [c/TeV]";
  legendText4.str( std::string() );
  legendText4.clear();
  legendText4 << "#chi^{2} = " << chi2;
  legendText5.str( std::string() );
  legendText5.clear();
  legendText5 << "Ndof = " << ndof;
  legendText6.str( std::string() );
  legendText6.clear();
  legendText6 << "MC integral before scaling: " << intBefore;
  legendText7.str( std::string() );
  legendText7.clear();
  legendText7 << "MC integral after scaling: " << intAfter;
  legendText8.str( std::string() );
  legendText8.clear();
  legendText8 << "Data integral: " << intData;

  leg->AddEntry((TObject*) 0, TString(legendText1.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText2.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText3.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText4.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText5.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText6.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText7.str()), "");
  leg->AddEntry((TObject*) 0, TString(legendText8.str()), "");

  leg->SetTextSize(0.020);
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
  legendText3.str( std::string() );
  legendText3.clear();
  legendText3 << deltaKappa;


  tmpCanvas->SaveAs(TString(oDir+"/curvature_"+plotName+legendText3.str()+"NewSS.png"));

  tmpCanvas->SaveAs(TString("~/public/html/cosmics/curvature_"+plotName+legendText3.str()+"NewSS.png"));

  return;
}
