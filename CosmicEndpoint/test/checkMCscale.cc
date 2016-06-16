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
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>

void checkMCscale(std::string const& inDir1, std::string const& inDir2, std::string const& histBase, int isStart){
 std:stringstream type;
 type.str( std::string() );
 type.clear();

 double startupScale = 58898.0/882609;
 double asymScale = 58898.0/1028051;
 double mc500Scale = 0.0;
 if(isStart == 1){
   type << "Startup";
   mc500Scale = startupScale;
 }
 else{
   type << "Asymptotic";
   mc500Scale = asymScale;
 }

 TFile *outFile = new TFile("mcScaledPlots","RECREATE");
 
 TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inDir1+"/CosmicHistOut_TuneP.root"));
 
 if(f == 0){
   
   std::cout << "Error: cannot open file1 \n";
   return;
   
  }
 
 std::cout << std::hex << f << std::dec << std::endl;
 std::cout << "\nSuccessfully opened file1!\n";
 
 TFile *g = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inDir2+"/CosmicHistOut_TuneP.root"));
 
 if(g == 0){
   
   std::cout << "Error: cannot open file2!";
   return;
   
 }
 
 std::cout << std::hex << g << std::dec << std::endl;
 std::cout << "\nSuccessfully opened file2!";
  
 TH1F *mc100HistMinus = (TH1F*)f->Get(TString(histBase+"MinusTrackPt"));
 TH1F *mc100HistPlus = (TH1F*)f->Get(TString(histBase+"PlusTrackPt"));
 TH1F *mc500HistMinus = (TH1F*)g->Get(TString(histBase+"MinusTrackPt"));
 TH1F *mc500HistPlus = (TH1F*)g->Get(TString(histBase+"PlusTrackPt"));
 
 
 outFile->cd();
 mc100HistMinus->SetName("mc100TrackPtMinus");
 mc100HistMinus->Write();
 mc100HistPlus->SetName("mc100TrackPtPlus");
 mc100HistPlus->Write();
 mc500HistMinus->SetName("mc500TrackPtMinus");
 mc500HistMinus->Write();
 mc500HistPlus->SetName("mc500TrackPtPlus");
 mc500HistPlus->Write();
 
 mc100HistMinus->Sumw2();
 mc100HistPlus->Sumw2();
 mc500HistMinus->Sumw2();
 mc500HistPlus->Sumw2();
  
 mc100HistMinus->Add(mc100HistPlus);
 mc100HistMinus->SetName("mc100TrackPtAdded");
 mc100HistMinus->Write();
 mc500HistMinus->Add(mc500HistPlus);
 mc500HistMinus->SetName("mc500HistPlus");
 mc500HistMinus->Write();
 
 TCanvas *unscaledCanvas = new TCanvas("unscaledCanvas", "unscaledCanvas", 1600, 900);
 TPad *unscaledPad = new TPad("unscaledPad", "UnscaledPad", 0.0, 0.0, 1.0, 1.0);
 TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
 //TLegend* leg = new TLegend(0.2, 0.6, 0.60, 0.9);
 double ptWidth = mc100HistMinus->GetBinWidth(3);
 std::stringstream ss;
 ss.str( std::string() );
 ss.clear();
 ss << ptWidth;
 
 unscaledCanvas->cd();
 unscaledPad->Draw();
 unscaledPad->cd();
 mc100HistMinus->Draw("ep0");
 mc100HistMinus->SetMarkerColor(kAzure);
 mc100HistMinus->SetLineColor(kAzure);
 mc100HistMinus->SetMaximum(1.2*mc100HistMinus->GetMaximum());
 mc100HistMinus->SetMinimum(0.1);
 mc100HistMinus->GetXaxis()->SetTitle("pT [GeV]");
 mc100HistMinus->GetYaxis()->SetTitle(TString("N_#mu/"+ss.str()+"[GeV]"));
 mc100HistMinus->SetTitle(TString("Unscaled_MC_"+type.str()+"_TrackPt_Distribution"));
 mc100HistMinus->SetStats(0);
 unscaledPad->SetLogy(1);
 unscaledPad->Update();
 unscaledCanvas->Update();
 
 mc500HistMinus->SetMarkerColor(kRed+2);
 mc500HistMinus->SetLineColor(kRed+2);
 mc500HistMinus->Draw("ep0sames");
 mc500HistMinus->SetStats(0);
 unscaledPad->Update();
 unscaledCanvas->Update();
 leg->AddEntry(mc100HistMinus, TString("MC_"+type.str()+"_Peak_P>100"), "l");
 leg->AddEntry(mc500HistMinus, TString("MC_"+type.str()+"_P>500"), "l");
 leg->SetTextSize(0.020);
 leg->SetFillColor(0);
 leg->SetFillStyle(3000);
 leg->Draw();
 unscaledPad->Update();
 unscaledCanvas->Update();
  
 unscaledCanvas->SaveAs(TString("~/public/html/cosmics/April16/unscaledMC_"+type.str()+"TrackPt.png"));
 
 unscaledCanvas->Write();


 mc100HistMinus->Sumw2();
 mc500HistMinus->Sumw2();
 mc500HistMinus->Scale(mc500Scale);
 
 mc500HistMinus->SetName("MC500Hist_Scaled");
 mc500HistMinus->Write();

 TCanvas *scaledCanvas = new TCanvas("scaledCanvas", "scaledCanvas", 1600, 900);
 TPad *scaledPad = new TPad("scaledPad", "scaledPad", 0.0, 0.0, 1.0, 1.0);
 TLegend* leg2 = new TLegend(0.7, 0.7, 0.9, 0.9);
 //TLegend* leg = new TLegend(0.2, 0.6, 0.60, 0.9);

 
 scaledCanvas->cd();
 scaledPad->Draw();
 scaledPad->cd();
 mc100HistMinus->Draw("ep0");
 mc100HistMinus->SetMarkerColor(kAzure);
 mc100HistMinus->SetLineColor(kAzure);
 mc100HistMinus->SetMaximum(1.2*mc100HistMinus->GetMaximum());
 mc100HistMinus->SetMinimum(0.1);
 mc100HistMinus->GetXaxis()->SetTitle("pT [GeV]");
 mc100HistMinus->GetYaxis()->SetTitle(TString("N_#mu/"+ss.str()+"[GeV]"));
 mc100HistMinus->SetTitle(TString("Scaled_MC_"+type.str()+"_TrackPt_Distribution"));
 mc100HistMinus->SetStats(0);
 scaledPad->SetLogy(1);
 scaledPad->Update();
 scaledCanvas->Update();
 
 mc500HistMinus->SetMarkerColor(kRed+2);
 mc500HistMinus->SetLineColor(kRed+2);
 mc500HistMinus->Draw("ep0sames");
 mc500HistMinus->SetStats(0);
 scaledPad->Update();
 scaledCanvas->Update();
 leg2->AddEntry(mc100HistMinus, TString("MC_"+type.str()+"_Peak_P>100"), "l");
 leg2->AddEntry(mc500HistMinus, TString("MC_"+type.str()+"_P>500"), "l");
 leg2->SetTextSize(0.020);
 leg2->SetFillColor(0);
 leg2->SetFillStyle(3000);
 leg2->Draw();
 scaledPad->Update();
 scaledCanvas->Update();
  
 scaledCanvas->SaveAs(TString("~/public/html/cosmics/April16/scaledMC_"+type.str()+"TrackPt.png"));
 
 scaledCanvas->Write();


 TCanvas *addedCanvas = new TCanvas("addedCanvas", "addedCanvas", 1600, 900);
 TPad *addedPad = new TPad("addedPad", "addedPad", 0.0, 0.0, 1.0, 1.0);
 
 mc100HistMinus->Sumw2();
 mc500HistMinus->Sumw2();
 mc100HistMinus->Add(mc500HistMinus);
 mc100HistMinus->SetName("mcAdded");

 mc100HistMinus->Write();

 addedCanvas->cd();
 addedPad->Draw();
 addedPad->cd();
 mc100HistMinus->Draw("ep0");
 mc100HistMinus->SetMarkerColor(kAzure);
 mc100HistMinus->SetLineColor(kAzure);
 mc100HistMinus->SetMaximum(1.2*mc100HistMinus->GetMaximum());
 mc100HistMinus->SetMinimum(0.1);
 mc100HistMinus->GetXaxis()->SetTitle("pT [GeV]");
 mc100HistMinus->GetYaxis()->SetTitle(TString("N_#mu/"+ss.str()+"[GeV]"));
 mc100HistMinus->SetTitle(TString("Added_MC_"+type.str()+"_TrackPt_Distribution"));
 mc100HistMinus->SetStats(0);
 addedPad->SetLogy(1);
 addedPad->Update();
 addedCanvas->Update();

 /* leg2->AddEntry(mc100HistMinus, TString("MC_"+type.str()+"_Peak_P>100"), "l");
 leg2->AddEntry(mc500HistMinus, TString("MC_"+type.str()+"_P>500"), "l");
 leg2->SetTextSize(0.020);
 leg2->SetFillColor(0);
 leg2->SetFillStyle(3000);
 leg2->Draw();
 addedPad->Update();
 addedCanvas->Update();
 */

 addedCanvas->SaveAs(TString("~/public/html/cosmics/April16/addedMC_"+type.str()+"TrackPt.png"));
 
 addedCanvas->Write();
 
 
 outFile->Write();
 outFile->Close();

 return;
}
