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

void chi2BinCheck(std::string const& file, int binBefore, int binLow, int binHigh, int binAfter, int binMin){

  TFile *g;
  g = new TFile("Chi2Check.root", "RECREATE");

  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+file));

  if(f == 0){
    std::cout << "Error: cannot open file! \n";
    return;

  }


  std::cout << "Opened File succesfully!";

  TH1F *refHist = (TH1F*)f->Get("dataHist;2");
  std::cout << "\nGot reference histogram!\n";
  TH1F *ndof = (TH1F*)f->Get("TunePNdof_Plot;2");
  std::cout << "\nGot ndof histogram!";
  TH1F *chi2 = (TH1F*)f->Get("Chi2vCurve_TuneP");
  std::cout << "\nGot Chi2 histogram!";

  std::stringstream ss;

  ss << std::setw(3) << binBefore;
  TH1F *beforeWeird = (TH1F*)f->Get(TString("TunePMCCurvaturePlusBias"+ss.str()));
  int ndofBefore = ndof->GetBinContent(1000+binBefore);
  double chi2Before = chi2->GetBinContent(1000+binBefore);
  std::cout << "\nGot first histogram!\n";

  ss.str( std::string() );
  ss.clear();
  ss << std::setw(3) << binLow;
  TH1F *duringWeirdLow = (TH1F*)f->Get(TString("TunePMCCurvaturePlusBias"+ss.str()));
  int ndofLow = ndof->GetBinContent(1000+binLow);
  double chi2Low = chi2->GetBinContent(1000+binLow);
  std::cout << "\nGot 2nd histogram!";

  ss.str( std::string() );
  ss.clear();
  ss << std::setw(3) << binHigh;
  TH1F *duringWeirdHigh = (TH1F*)f->Get(TString("TunePMCCurvaturePlusBias"+ss.str()));
  int ndofHigh = ndof->GetBinContent(1000+binHigh);
  double chi2High = chi2->GetBinContent(1000+binHigh);  
  std::cout << "\nGot 3rd histogram!";
  ss.str( std::string() );
  ss.clear();
  ss << std::setw(3) << binAfter;
  TH1F *afterWeird = (TH1F*)f->Get(TString("TunePMCCurvaturePlusBias"+ss.str()));
  int ndofAfter = ndof->GetBinContent(1000+binAfter);
  double chi2After = chi2->GetBinContent(1000+binAfter);
  std::cout << "\nGot last histogram!";

  ss.str( std::string() );
  ss.clear();
  ss << std::setw(3) << binMin;
  TH1F *min = (TH1F*)f->Get(TString("TunePMCCurvaturePlusBias"+ss.str()));
  int ndofMin = ndof->GetBinContent(1000+binMin);
  double chi2Min = chi2->GetBinContent(1000+binMin);
  std::cout << "\nGot last histogram!";  
  std::cout << "\n\nThe number of degrees of freedom before-low-high-after are:"
	    << ndofBefore << "-" << ndofLow << "-" << ndofHigh << "-" << ndofAfter << std::endl;

  std::cout << "\n\nThe Chi2 value for before-low-high-after are:" << chi2Before << "-" << chi2Low
	    << "-" << chi2High << "-" << chi2After << std::endl;

  TCanvas *canvas = new TCanvas("Curvatures", "Curvatures", 1600, 900);
  TPad *pad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);

  canvas->cd();
  pad->Draw();
  pad->cd();
  
  refHist->SetMarkerColor(1);
  refHist->SetLineColor(1);
  refHist->Draw("ep0");
  pad->Update();
  canvas->Update();
  
  beforeWeird->SetMarkerColor(9);
  beforeWeird->SetLineColor(9);
  beforeWeird->Draw("ep0sames");
  pad->Update();
  canvas->Update();
  
  duringWeirdLow->SetMarkerColor(2);
  duringWeirdLow->SetLineColor(2);
  duringWeirdLow->Draw("ep0sames");
  pad->Update();
  canvas->Update();
  
  duringWeirdHigh->SetMarkerColor(3);
  duringWeirdHigh->SetLineColor(3);
  duringWeirdHigh->Draw("ep0sames");
  pad->Update();
  canvas->Update();

  afterWeird->SetMarkerColor(5);
  afterWeird->SetLineColor(5);
  afterWeird->Draw("ep0sames");
  pad->Update();
  canvas->Update();
  
  min->SetMarkerColor(46);
  min->SetLineColor(46);
  min->Draw("ep0sames");
  pad->Update();
  canvas->Update();
  
  TLegend*  leg = new TLegend(0.1, 0.7, 0.48, 0.9);
  ss.str( std::string());
  ss.clear();
  ss << "Chi2:" << chi2Before << " - ndof:" << ndofBefore << " - bias: 712";
  leg->AddEntry(beforeWeird, TString(ss.str()), "l");

  ss.str( std::string());
  ss.clear();
  ss << "Chi2:" << chi2Low << " - ndof:" << ndofLow << " - bias: 775";
  leg->AddEntry(duringWeirdLow, TString(ss.str()), "l");

  ss.str( std::string());
  ss.clear();
  ss << "Chi2:" << chi2High << " - ndof:" << ndofHigh << " - bias: 785";
  leg->AddEntry(duringWeirdHigh, TString(ss.str()), "l");

  ss.str( std::string());
  ss.clear();
  ss << "Chi2:" << chi2After << " - ndof:" << ndofAfter << " - bias: 792";
  leg->AddEntry(afterWeird, TString(ss.str()), "l");

  ss.str( std::string());
  ss.clear();
  ss << "Chi2:" << chi2Min << " - ndof:" << ndofMin << " - bias: 214";
  leg->AddEntry(min, TString(ss.str()), "l");

  leg->Draw();
  g->cd();
  refHist->Write();
  beforeWeird->Write();
  duringWeirdLow->Write();
  duringWeirdHigh->Write();
  afterWeird->Write();
  canvas->Write();

  return;
}
