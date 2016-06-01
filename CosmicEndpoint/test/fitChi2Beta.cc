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
#include "TF1.h"
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

void fitIt(std::string const& outFile, std::string const& kinBin, TFile *oFile, TFile *file);

void fitChi2Beta(std::string const& rootFile, std::string const& outFile){//, double minFit, double maxFit){

  TFile *file = new TFile(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+rootFile+".root"), "READ");
  TFile *g = new TFile(TString(outFile+".root"),"RECREATE");
  std::cout << "The file reading is" << file << std::endl;
  
  fitIt(outFile, "EMPM", g, file);
  fitIt(outFile, "EMPZ", g, file);
  fitIt(outFile, "EMPP", g, file);
  fitIt(outFile, "EPPM", g, file);
  fitIt(outFile, "EPPZ", g, file);
  fitIt(outFile, "EPPP", g, file);
  
  
  g->Write();
  g->Close();
  

 
 return;
}

void fitIt(std::string const& outFile, std::string const&kinBin, TFile *oFile, TFile *file){

  TH1F *graph = (TH1F*)file->Get(TString("TunePChi2vCurve"+kinBin+";1")); //file->Get("tunep_Chi2");
  double minChi2 = graph->GetBinContent(graph->GetMinimumBin());
  oFile->cd();

  int subtractBins = 0;
  int addBins = 0;
  for(int numBinsMinus = 0; numBinsMinus < 100; numBinsMinus++){

    if(graph->GetBinContent((graph->GetMinimumBin()) - numBinsMinus) > (minChi2 + 5)) break;
    subtractBins++;

  }
  
  for(int numBinsPlus = 0; numBinsPlus < 100; numBinsPlus++){

    if(graph->GetBinContent((graph->GetMinimumBin()) + numBinsPlus) > (minChi2 + 5)) break;
    addBins++;
  }

  double minFit = 0.0;
  double maxFit = 0.0;
  minFit = 0.0025*(graph->GetMinimumBin() - subtractBins) - 0.5;

  maxFit = 0.0025*(graph->GetMinimumBin() + addBins) - 0.5;

  TH1F* dataHist = (TH1F*)file->Get("TunePCraftBoth"); //Use for DatavMC
  //TH1F* dataHist = (TH1F*)file->Get("dataHist"); //Use for MCvMC and DatavData
  std::cout << "\nGot the first histo\n";
  
  
  double binWidth = dataHist->GetBinWidth(3);
  double stepSize = 0.04;
  
  
  std::cout << "\nThe graph to be read is " << graph << std::endl;
  TF1 *fitPoly2 = new TF1("fitPoly2","pol2", -1,1);
  TF1 *fitPoly6 = new TF1("fitPoly6", "pol6", -1, 1);
  TF1 *fitPoly8 = new TF1("fitPoly8", "pol8", -1, 1);
  TF1 *fitPoly2Left = new TF1("fitPoly2Left","pol2", -1,1);
  TF1 *fitPoly6Left = new TF1("fitPoly6Left", "pol6", -1, 1);
  TF1 *fitPoly8Left = new TF1("fitPoly8Left", "pol8", -1, 1);
  TF1 *fitPoly2Right = new TF1("fitPoly2Right","pol2", -1,1);
  TF1 *fitPoly6Right = new TF1("fitPoly6Right", "pol6", -1, 1);
  TF1 *fitPoly8Right = new TF1("fitPoly8Right", "pol8", -1, 1);

  
  std::cout << "\nThe fit to be used is is " << fitPoly2 << std::endl;
  fitPoly2->SetParameters(0,0,0);
  fitPoly6->SetParameters(0,0,0,0,0,0,0);
  fitPoly8->SetParameters(0,0,0,0,0,0,0,0,0);
  fitPoly2Left->SetParameters(0,0,0);
  fitPoly6Left->SetParameters(0,0,0,0,0,0,0);
  fitPoly8Left->SetParameters(0,0,0,0,0,0,0,0,0);
  fitPoly2Right->SetParameters(0,0,0);
  fitPoly6Right->SetParameters(0,0,0,0,0,0,0);
  fitPoly8Right->SetParameters(0,0,0,0,0,0,0,0,0);

  std::cout << "\nSet Parameters" << std::endl;
  
  
  graph->Fit("fitPoly2", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly6", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly8", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly2Left", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly6Left", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly8Left", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly2Right", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly6Right", "VEMFRN", "", minFit, maxFit);
  graph->Fit("fitPoly8Right", "VEMFRN", "", minFit, maxFit);

  std::cout << "\nFit the polynomial" << std::endl;
  
  //graph->Draw("alp");
  std::cout << "\nDrew graph" << std::endl;
  //  fitPoly2->Draw("sames");
  //fitPoly6->Draw("sames");
  //fitPoly8->Draw("sames");
  std::cout << "\nDrew the Fit" << std::endl;
  
  graph->Write();
  fitPoly2->Write();
  fitPoly6->Write();
  fitPoly8->Write();
  
  double minX2 = fitPoly2->GetMinimumX(minFit, maxFit, 1.E-10,100, 0);
  double minY2 = fitPoly2->GetMinimum(minFit,maxFit,1.E-10,100,0);
  std::cout << "\nMinX2 is: " << minX2;
  std::cout << "\nMinY2 is: " << minY2 << std::endl << std::endl;
  double minX6 = fitPoly6->GetMinimumX(minFit, maxFit, 1.E-10,100, 0);
  double minY6 = fitPoly6->GetMinimum(minFit,maxFit,1.E-10,100,0);
  std::cout << "\nMinX6 is: " << minX6;
  std::cout << "\nMinY6 is: " << minY6 << std::endl << std::endl;
  double minX8 = fitPoly8->GetMinimumX(minFit, maxFit, 1.E-10,100, 0);
  double minY8 = fitPoly8->GetMinimum(minFit,maxFit,1.E-10,100,0);
  std::cout << "\nMinX2 is: " << minX8;
  std::cout << "\nMinY2 is: " << minY8 << std::endl << std::endl;
  
  double leftEdge = minFit - 0.02;
  double rightEdge = maxFit +0.02;

  Double_t xLeft2 = 0.0;
  Double_t xRight2 = 0.0;
  
  Double_t xLeft6 = 0.0;
  Double_t xRight6 = 0.0;
  
  Double_t xLeft8 = 0.0;
  Double_t xRight8 = 0.0; 
  xLeft2 = fitPoly2->GetX(minY2+1, 0.00, minX2, 1.E-10, 100, 0);
  xRight2 = fitPoly2->GetX(minY2+1, minX2, 0.15, 1.E-10, 100, 0);
  xLeft6 = fitPoly6->GetX(minY6+1, 0.00, minX6, 1.E-10, 100, 0);
  xRight6 = fitPoly6->GetX(minY6+1, minX6, 0.15, 1.E-10, 100, 0); 
  xLeft8 = fitPoly8->GetX(minY8+1, 0.00, minX8, 1.E-10, 100, 0);
  xRight8 = fitPoly8->GetX(minY8+1, minX8, 0.15, 1.E-10, 100, 0);
  
  
  Double_t minSet2 = minY2 * 0.95;
  Double_t maxSet2 = (minY2 + 1);
  
  Double_t minSet6 = minY6 * 0.95;
  Double_t maxSet6 = (minY6 + 1);
  
  Double_t minSet8 = minY8 * 0.95;
  Double_t maxSet8 = (minY8 + 1);
 
  Double_t lineYmax2 = maxSet2;
  Double_t lineYmin2 = minSet2;
  
  Double_t lineYmax6 = maxSet6;
  Double_t lineYmin6 = minSet6;
  
  Double_t lineYmax8 = maxSet8;
  Double_t lineYmin8 = minSet8; 
  std::cout << "\n\n\nThe left--right value of minY2+1 are: " << xLeft2 << "--" << xRight2 << std::endl;
  std::cout << "\n\n\nThe left--right value of minY6+1 are: " << xLeft6 << "--" << xRight6 << std::endl;
  std::cout << "\n\n\nThe left--right value of minY8+1 are: " << xLeft8 << "--" << xRight8 << std::endl;
  
  
  
  TCanvas *can = new TCanvas("Canvas", "Canvas", 1600, 900);
  TPad *pad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);
  
  can->cd();
  pad->Draw();
  pad->cd();
  graph->Draw("p0");
  graph->SetMarkerColor(kBlack);
  graph->SetLineColor(kBlack);
  graph->SetMarkerStyle(33);
  graph->SetMarkerSize(2);
  graph->SetMinimum(minChi2 - 5);
  graph->SetMaximum(minChi2 + 25);
  graph->SetTitle("#chi^{2} vs #Delta#kappa_{bias}");
  graph->GetXaxis()->SetTitle("#Delta#kappa [c/TeV]");
  //  graph->GetXaxis()->SetRangeUser(minFit, maxFit);
  graph->GetYaxis()->SetTitle("#chi^{2}");
  graph->SetStats(0);
  pad->Update();
  can->Update();
  
  
  fitPoly2->Draw("sames");
  fitPoly2->SetLineColor(kGreen+2);
  fitPoly2->SetRange(minFit, maxFit);
  fitPoly2->SetLineStyle(2);
  fitPoly2->SetLineWidth(3);
  pad->Update();
  can->Update();


  /*  fitPoly2Left->Draw("sames");
  fitPoly2Left->SetLineColor(kGreen+2);
  fitPoly2Left->SetRange(-0.5, minFit);
  pad->Update();
  can->Update();


  fitPoly2Right->Draw("sames");
  fitPoly2Right->SetLineColor(kGreen+2);
  fitPoly2Right->SetRange(maxFit, 0.5);
  std::cout << "\nDrew quadratic Fit\n";
  pad->Update();
  can->Update();
  
  */
  
  fitPoly6->Draw("sames");
  fitPoly6->SetLineColor(kRed+2);
  fitPoly6->SetRange(minFit, maxFit);
  fitPoly6->SetLineStyle(2);
  fitPoly6->SetLineWidth(3);
  pad->Update();
  can->Update();

  /*  fitPoly6Left->Draw("sames");
  fitPoly6Left->SetLineColor(kRed+2);
  fitPoly6Left->SetRange(-0.5, minFit);
  pad->Update();
  can->Update();

  fitPoly6Right->Draw("sames");
  fitPoly6Right->SetLineColor(kRed+2);
  fitPoly6Right->SetRange(maxFit, 0.5);

  std::cout << "\nDrew 6th order fit!\n";
  pad->Update();
  can->Update();
  */
  fitPoly8->Draw("sames");
  fitPoly8->SetLineColor(kAzure);
  fitPoly8->SetRange(minFit, maxFit);
  fitPoly8->SetLineStyle(2);
  fitPoly8->SetLineWidth(3);
  pad->Update();
  can->Update();


  /*fitPoly8Left->Draw("sames");
  fitPoly8Left->SetLineColor(kAzure);
  fitPoly8Left->SetRange(-0.5, minFit);

  pad->Update();
  can->Update();


  fitPoly8Right->Draw("sames");
  fitPoly8Right->SetLineColor(kAzure);
  fitPoly8Right->SetRange(maxFit, 0.5);

  pad->Update();
  can->Update();
  */
  //TFitResultsPtr p2 = graph->Fit("pol2", "S");
  //  Int_t fitStatus = graphFit("pol2", "S");
  Double_t p2Ndof = fitPoly2->GetNDF();
  Double_t p2Chi2 = fitPoly2->GetChisquare();
  //  Double_t p2Par2 = fitPoly2->GetParameter(2);

  Double_t p6Ndof = fitPoly6->GetNDF();
  Double_t p6Chi2 = fitPoly6->GetChisquare();
  //  Double_t p6Par2 = fitPoly6->GetParameter(2);
  
  Double_t p8Ndof = fitPoly8->GetNDF();
  Double_t p8Chi2 = fitPoly8->GetChisquare();
  //  Double_t p8Par2 = fitPoly8->GetParameter(2);
  
  std::stringstream leftSigma;
  std::stringstream rightSigma;
  std::stringstream centralVal;
  std::stringstream legendText1;
  std::stringstream legendText2;
  
  
  leftSigma.str( std::string() );
  leftSigma.clear();
  rightSigma.str( std::string() );
  rightSigma.clear();
  centralVal.str( std::string() );
  centralVal.clear();
  
  legendText1.str( std::string() );
  legendText1.clear();
  legendText1 << stepSize;
  legendText2.str( std::string() );
  legendText2.clear();
  legendText2 << binWidth;
 
  double minusUn2 = minX2 - xLeft2;
  double plusUn2 = xRight2 - minX2;
  double minusUn6 = minX6 - xLeft6;
  double plusUn6 = xRight6 - minX6;
  double minusUn8 = minX8 - xLeft8;
  double plusUn8 = xRight8 - minX8;
  
  

  TLegend* leg = new TLegend(0.2, 0.6, 0.60, 0.9);
  TLegend* legP2 = new TLegend(0.7, 0.8, 0.9, 0.9);
  TLegend* legP6 = new TLegend(0.7, 0.7, 0.9, 0.8);
  TLegend* legP8 = new TLegend(0.7, 0.6, 0.9, 0.7);
  TLegend* fitInfo = new TLegend(0.7, 0.5, 0.9, 0.6);

  std::stringstream fitMin;
  std::stringstream fitMax;

  char fitMaxArray[15];
  sprintf(fitMaxArray, "%2.2f", maxFit);
  fitMax << "Fit Range Max: " << fitMaxArray << " [c/TeV]";

  char fitMinArray[15];
  sprintf(fitMinArray, "%2.2f", minFit);
  fitMin << "Fit Range Min: " << fitMinArray << " [c/TeV]";

  std::stringstream p2P0;
  std::stringstream p2P1;
  //  std::stringstream p2P2;

  std::stringstream p6P0;
  std::stringstream p6P1;
  //std::stringstream p6P2;

  std::stringstream p8P0;
  std::stringstream p8P1;
  //std::stringstream p8P2;
  
  char poly2P0[15];
  sprintf(poly2P0, "%2.2f", p2Ndof);
  p2P0 << "Ndof: " << poly2P0;

  char poly2P1[15];
  sprintf(poly2P1, "%2.2f", p2Chi2);
  p2P1 << "#chi^{2}: " << poly2P1;

  
  //  char poly2P2[15];
  //sprintf(poly2P2, "%2.2f", p2Par2);
  //p2P2 << "p2: " << poly2P2;

  char poly6P0[15];
  sprintf(poly6P0, "%2.2f", p6Ndof);
  p6P0 << "Ndof: " << poly6P0;

  char poly6P1[15];
  sprintf(poly6P1, "%2.2f", p6Chi2);
  p6P1 << "#chi^{2}: " << poly6P1;

  
  //  char poly6P2[15];
  //  sprintf(poly6P2, "%2.2f", p6Par2);
  //  p6P2 << "p2: " << poly6P2;

  char poly8P0[15];
  sprintf(poly8P0, "%2.2f", p8Ndof);
  p8P0 << "Ndof: " << poly8P0;

  char poly8P1[15];
  sprintf(poly8P1, "%2.2f", p8Chi2);
  p8P1 << "#chi^{2}: " << poly8P1;

  
  //  char poly8P2[15];
  //  sprintf(poly8P2, "%2.2f", p8Par2);
  //  p8P2 << "p2: " << poly8P2;

  legP2->AddEntry((TObject *) 0, "Quadratic Fit", "");
  legP2->AddEntry((TObject *) 0, TString(p2P0.str()), "");
  legP2->AddEntry((TObject *) 0, TString(p2P1.str()), "");
  //legP2->AddEntry((TObject *) 0, TString(p2P2.str()), "");
  legP2->SetTextSize(0.020);
  legP2->SetTextColor(kGreen+2);
  legP2->SetFillStyle(3000);
  legP2->Draw();

  legP6->AddEntry((TObject *) 0, "6^{th} degree Fit", "");
  legP6->AddEntry((TObject *) 0, TString(p6P0.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P1.str()), "");
  
  legP6->SetTextSize(0.020);
  legP6->SetTextColor(kRed+2);
  legP6->SetFillStyle(3000);
  legP6->Draw();

  legP8->AddEntry((TObject *) 0, "8^{th} degree Fit", "");
  legP8->AddEntry((TObject *) 0, TString(p8P0.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P1.str()), "");
  
  legP8->SetTextSize(0.020);
  legP8->SetTextColor(kAzure);
  legP8->SetFillStyle(3000);
  legP8->Draw();

  fitInfo->AddEntry((TObject *) 0, TString(fitMin.str()), "");
  fitInfo->AddEntry((TObject *) 0, TString(fitMax.str()), "");
  fitInfo->SetTextSize(0.020);
  fitInfo->SetTextColor(kBlack);
  fitInfo->Draw();

  char minusBias2[10];
  sprintf(minusBias2, "-%2.5f", minusUn2);
  leftSigma << minusBias2;
  
  char plusBias2[10];
  sprintf(plusBias2, "+%2.5f", plusUn2);
  rightSigma << plusBias2;
  
  
  leg->AddEntry((TObject*) 0, TString("Injected bias step size: "+legendText1.str()+" [c/TeV]"), "");
  leg->AddEntry((TObject*) 0, TString("Curvature bin width is: "+legendText2.str())+" [c/TeV]", "");
  
  
  char centralBias2[10];
  sprintf(centralBias2, "%2.5f", minX2);
  centralVal << centralBias2;
  leg->AddEntry(fitPoly2, TString("P2 #Delta#kappa_{min} = "+ centralVal.str() + " _{" + leftSigma.str() + "} ^{" + rightSigma.str() + "} [c/TeV]"), "l");
  
  char minusBias6[10];
  sprintf(minusBias6, "-%2.5f", minusUn6);
  leftSigma.str( std::string() );
  leftSigma.clear();
  leftSigma << minusBias6;
  
  char plusBias6[10];
  sprintf(plusBias6, "+%2.5f", plusUn6);
  rightSigma.str( std::string() );
  rightSigma.clear();
  rightSigma << plusBias6;
  
  char centralBias6[10];
  sprintf(centralBias6, "%2.5f", minX6);
  centralVal.str( std::string() );
  centralVal.clear();
  centralVal << centralBias6;
  leg->AddEntry(fitPoly6, TString("P6 #Delta#kappa_{min} = "+ centralVal.str() + " _{" + leftSigma.str() + "} ^{" + rightSigma.str() + "} [c/TeV]"), "l");

  
  char minusBias8[10];
  sprintf(minusBias8, "-%2.5f", minusUn8);
  leftSigma.str( std::string() );
  leftSigma.clear();
  leftSigma << minusBias8;
  
  char plusBias8[10];
  sprintf(plusBias8, "+%2.5f", plusUn8);
  rightSigma.str( std::string() );
  rightSigma.clear();
  rightSigma << plusBias8;
  
  char centralBias8[10];
  sprintf(centralBias8, "%2.5f", minX8);
  centralVal.str( std::string() );
  centralVal.clear();
  centralVal << centralBias8;
  leg->AddEntry(fitPoly8, TString("P8 #Delta#kappa_{min} = "+ centralVal.str() + " _{" + leftSigma.str() + "} ^{" + rightSigma.str() + "} [c/TeV]"), "l");
  
  
  leg->SetTextSize(0.030);
  leg->SetFillStyle(3000);
  leg->Draw();
  
  
  pad->Update();
  can->Update();
  can->Write();
  can->SaveAs(TString("~/public/html/cosmics/April16/"+outFile+kinBin+".png"));
  can->SaveAs("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/Chi2Plots/data_MCchi2FitP2.C");


  TCanvas *fitCan = new TCanvas("FitCanvas", "FitCanvas", 1600, 900);
  TPad *fitPad = new TPad("FitPad", "FitPad", 0.0, 0.0, 1.0, 1.0);
  
  fitCan->cd();
  fitPad->Draw();
  fitPad->cd();
  graph->Draw("p0");
  graph->SetMarkerColor(kBlack);
  graph->SetLineColor(kBlack);
  graph->SetMarkerStyle(7);
  graph->SetMarkerSize(100);
  graph->SetMinimum(minChi2 - 5);
  graph->SetMaximum(minChi2 + 25);
  graph->SetTitle("#chi^{2} vs #Delta#kappa_{bias}");
  graph->GetXaxis()->SetTitle("#Delta#kappa [c/TeV]");
  graph->GetXaxis()->SetRangeUser(minFit, maxFit);
  graph->GetYaxis()->SetTitle("#chi^{2}");
  graph->SetStats(0);
  fitPad->Update();
  fitCan->Update();
  
  
  fitPoly2->Draw("sames");
  fitPoly2->SetLineColor(kGreen+2);
  fitPoly2->SetRange(minFit, maxFit);
  fitPoly2->SetLineStyle(2);
  fitPoly2->SetLineWidth(1);
  fitPad->Update();
  fitCan->Update();


  std::cout << "\nDrew quadratic Fit\n";
  
  
  fitPoly6->Draw("sames");
  fitPoly6->SetLineColor(kRed+2);
  fitPoly6->SetRange(minFit, maxFit);
  fitPoly6->SetLineStyle(2);
  fitPoly6->SetLineWidth(1);
  fitPad->Update();
  fitCan->Update();

  std::cout << "\nDrew 6th order fit!\n";
  
  fitPoly8->Draw("sames");
  fitPoly8->SetLineColor(kAzure);
  fitPoly8->SetRange(minFit, maxFit);
  fitPoly8->SetLineStyle(2);
  fitPoly8->SetLineWidth(1);
  fitPad->Update();
  fitCan->Update();

  fitCan->Write();
  fitCan->SaveAs(TString("~/public/html/cosmics/April16/"+outFile+kinBin+"_fitRangeOnly.png"));
  fitCan->SaveAs("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/Chi2Plots/data_MCchi2FitP2.C");
  leg->Draw();

}
