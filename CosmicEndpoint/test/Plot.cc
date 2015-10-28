//#include <FWCore/Framework/interface/Frameworkfwd.h>
//#include <FWCore/Framework/interface/EDAnalyzer.h>
//
//#include <FWCore/Framework/interface/Event.h>
//#include <FWCore/Framework/interface/MakerMacros.h>
//
//#include <FWCore/ParameterSet/interface/ParameterSet.h>
//#include <DataFormats/PatCandidates/interface/Muon.h>
//#include <DataFormats/Candidate/interface/Candidate.h>
//#include <DataFormats/MuonReco/interface/MuonCocktails.h>
//#include <DataFormats/MuonReco/interface/MuonFwd.h>
#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
//#include "DataFormats/Math/interface/Vector3D.h"
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
#include "TPad.h"
#include "TPaveStats.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>

void compare2Plot(TH1F *hist1 ,TH1F *hist2, double setMin, std::string const& xAxis, std::string const& yAxis, std::string const& plotName ){

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

void compare3Plot(TH1F *hist1, TH1F *hist2, TH1F *hist3,double setMin, std::string const& xAxis, std::string const& yAxis, std::string const& plotName ){


  TCanvas *d = new TCanvas("c", "Canvas", 1000, 1000);
  TPad *pad2 = new TPad("comp", "comp", 0.0, 0.0, 1.0, 1.0);
  d->cd();

  pad2->Draw();

  pad2->cd();
  pad2->SetLogy(1);
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
  hist3->SetMarkerColor(3);
  hist3->SetLineColor(3);
  hist3->SetMarkerStyle(32);
  hist3->Draw("ep0sames");


  pad2->Update();
  d->Update();
  TPaveStats *stats1 = (TPaveStats*)hist1->FindObject("stats");
  stats1->SetName("noCut");  
  stats1->SetY1NDC(.7);
  stats1->SetY2NDC(.9);
  stats1->SetTextColor(7);
  stats1->SetOptStat(11111111);
  pad2->Update();
  d->Update();
  TPaveStats *stats2 = (TPaveStats*)hist2->FindObject("stats");
  stats2->SetName("(200)pTCut");
  stats2->SetY1NDC(.4);
  stats2->SetY2NDC(.6);
  stats2->SetTextColor(9);
  stats2->SetOptStat(11111111);
  pad2->Update();
  d->Update();
  TPaveStats *stats3 = (TPaveStats*)hist3->FindObject("stats");
  stats3->SetName("pTCut");
  stats3->SetY1NDC(.1);
  stats3->SetY2NDC(.3);
  stats3->SetTextColor(3);
  stats3->SetOptStat(11111111);
  pad2->Update();

  pad2->Update();
  d->Update();

  d->SaveAs(TString(plotName+".jpg"));
  d->SaveAs(TString(plotName+".png"));
  d->SaveAs(TString(plotName+".pdf"));
  d->SaveAs(TString(plotName+".eps"));




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


void Plot(std::string const& file1)
{

  TFile *g = new TFile("CosmicHistograms.root","RECREATE");
  /*  if (argc <= 2){
      std::cout<<"Must specify two files to compare!\n";
      return;
      }
      if (argc > 3){
      std::cout<<"Too many files specified! Only use two files\n";
      return;
      }
  */
  std::cout<<"arg 1 is:  " << file1 << std::endl;
 
  float maxBias = 0.05;
  int nBiasBins = 100;
  int massBinSize = 200;
  //int pBinSize = 200;
  float mMin = 0.;
  float mMax = 100.;

  TH1F *h_upChi2            = new TH1F("upChi2",    "upChi2",     100,   -0.5,   99.5 );
  TH1F *h_upNdof            = new TH1F("upNdof",    "upNdof",     100,   -0.5,   99.5 );
  TH1F *h_upCharge          = new TH1F("upCharge",  "upCharge",   3,     -1.5,   1.5  );
  TH1F *h_upCurve           = new TH1F("upCurve",   "upCurve",    2*500, -0.1,   0.1  );
  TH1F *h_upDxy             = new TH1F("upDxy",     "upDxy",      2*500, -1000., 1000.);
  TH1F *h_upDz              = new TH1F("upDz",      "upDz",       2*500, -1000., 1000.);
  TH1F *h_upDxyError        = new TH1F("upDxyError","upDxyError", 100,   -100.,  100. );
  TH1F *h_upDzError         = new TH1F("upDzError", "upDzError",  100,   -100.,  100. );
  TH1F *h_upPt              = new TH1F("upPt",      "upPt",       300,    0.,    3000.);
  TH1F *h_upTrackPt         = new TH1F("upTrackPt", "upTrackPt",  300,    0.,    3000.);
  TH1F *h_upPtError         = new TH1F("upPtError", "upPtError",  500,    0.,    500. );
  TH1F *h_upTrackEta        = new TH1F("upTrackEta","upTrackEta", 2*100, -5.,    5.   );
  TH1F *h_upTrackPhi        = new TH1F("upTrackPhi","upTrackPhi", 2*50,  -4.,    4.   );

  TH1F *h_upPixelHits               = new TH1F("upPixelHits",              "upPixelHits",               100, -0.5, 99.5 );
  TH1F *h_upTkHits                  = new TH1F("upTkHits",                 "upTkHits",                  100, -0.5, 99.5 );
  TH1F *h_upMuonStationHits         = new TH1F("upMuonStationHits",        "upMuonStationHits",         20,  -0.5, 19.5 );
  TH1F *h_upValidHits               = new TH1F("upValidHits",              "upValidHits",               200, -0.5, 199.5);
  TH1F *h_upMatchedMuonStations     = new TH1F("upMatchedMuonStation",     "upMatchedMuonStations",     20,  -0.5, 19.5 );
  TH1F *h_upTkLayersWithMeasurement = new TH1F("upTkLayersWithMeasurement","upTkLayersWithMeasurement", 200, -0.5, 199.5);
  
  TH1F *h_upperCurvePlusBias[nBiasBins];
  TH1F *h_upperCurveMinusBias[nBiasBins];
  for (int i = 0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    TString histname = TString("upperCurvePlusBias" + name.str());
    h_upperCurvePlusBias[i]  = new TH1F(histname, histname, 2*500, -0.1, 0.1);
    histname = TString("upperCurveMinusBias" + name.str());
    h_upperCurveMinusBias[i] = new TH1F(histname, histname, 2*500, -0.1, 0.1);
  }


  TH1F *h_lowerChi2 = new TH1F("lowerChi2","lowerChi2", massBinSize, 0, 20);
  TH1F *h_lowerNdof = new TH1F("lowerNdof","lowerNdof", massBinSize, 0, 20);
  TH1F *h_lowerCharge = new TH1F("lowerCharge","lowerCharge", massBinSize, -2, 2);
  TH1F *h_lowerCurve = new TH1F("lowerCurve","lowerCurve", massBinSize, -1, 1);
  TH1F *h_lowerDxy = new TH1F("lowerDxy","lowerDxy", massBinSize, -200, 200);
  TH1F *h_lowerDz = new TH1F("lowerDz","lowerDz", massBinSize, -200, 200);
  TH1F *h_lowerPixelHits = new TH1F("lowerPixelHits","lowerPixelHits", massBinSize, 0, 100);
  TH1F *h_lowerTrackerHits = new TH1F("lowerTrackerHits","lowerTrackerHits", massBinSize, 0, 100);
  TH1F *h_lowerMuonStationHits = new TH1F("lowerMuonStationHits","lowerMuonStationHits", massBinSize, 0, 10);
  TH1F *h_lowerValidHits = new TH1F("lowerValidHits","lowerValidHits", massBinSize, 0, 200);
  TH1F *h_lowerMatchedMuonStations = new TH1F("lowerMatchedMuonStation","lowerMatchedMuonStations", massBinSize, 0, 10);
  TH1F *h_lowerPtError = new TH1F("lowerPtError","lowerPtError", massBinSize, 0, 100);
  TH1F *h_lowerDxyError = new TH1F("lowerDxyError","lowerDxyError", massBinSize, -800, 800);
  TH1F *h_lowerDzError = new TH1F("lowerDzError","lowerDzError", massBinSize, -800, 800);
  TH1F *h_lowerPt = new TH1F("lowerPt","lowerPt", massBinSize, 0, 2000);
  TH1F *h_lowerTrackPt = new TH1F("lowerTrackPt","lowerTrackPt", massBinSize, 0, 2000);
  TH1F *h_lowerTrackEta = new TH1F("lowerTrackEta","lowerTrackEta", massBinSize, -10, 10);
  TH1F *h_lowerTrackPhi = new TH1F("lowerTrackPhi","lowerTrackPhi", massBinSize, -4, 4);
  TH1F *h_lowerTrackerLayersWithMeasurement = new TH1F("lowerTrackerLayersWithMeasurement","lowerTrackerLayersWithMeasurement", massBinSize, 0, 200);
  TH1F *h_lowerCurvePlusBias[nBiasBins];
  TH1F *h_lowerCurveMinusBias[nBiasBins];
  for(int i =0; i < nBiasBins; ++i){
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_lowerCurvePlusBias[i] = new TH1F(TString("lowerCurvePlusBias" + name.str()), TString("lowerCurvePlusBias" + name.str()), massBinSize, 0, 0.1);
    h_lowerCurveMinusBias[i] = new TH1F(TString("lowerCurveMinusBias" + name.str()),TString("lowerCurveMinusBias" + name.str()), massBinSize, 0, 0.1);  
    //std::cout << "\nCreated Lower bias histogram: " << i << std::endl;
  }
  


 
  TFile *f = TFile::Open(TString(file1));
  if (f == 0){
    std::cout << "Error: cannot open file 1 \n";
    return;
  }

  std::cout << "Successfully opened file 1! \n\n";

  TTreeReader trackReader("analysisTrackerMuons/MuonTree", f);
  //TTreeReader tpfmsReader("analysisTPFMSMuons/MuonTree", f);
  //TTreeReader dytReader("analysisDYTMuons/MuonTree", f);
  //TTreeReader pickyReader("analysisPickyMuons/MuonTree", f);
  //TTreeReader tunePReader("analysisTunePMuons/MuonTree", f);
  
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
  TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > upTrackerTrack(trackReader,     "upperMuon_trackVec");
  // TTreeReaderValue<Double_t> upTrackerTrackPhi(trackReader,"upperMuon_trackVec.phi()");


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
  //TTreeReaderValue<Double_t> lowTrackerTrackEta(trackReader,"lowerMuon_trackVec.eta()");
  //TTreeReaderValue<Double_t> lowTrackerTrackPhi(trackReader,"lowerMuon_trackVec.phi()");
  
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
      double upperCpT = *upTrackerCharge / * upTrackerPt;
      h_upperChi2->Fill(*upTrackerChi2);
      h_upperNdof->Fill(*upTrackerNdof);
      h_upperPt->Fill((*upTrackerMuonP4).Pt());
      std::cout << "pt = " << upTrackerMuonP4->pt()
		<< " - eta = "  << upTrackerMuonP4->eta()
		<< " - phi = "  << upTrackerMuonP4->phi()
		<< std::endl;

      std::cout	<< "pt = " << sqrt(upTrackerTrack->Perp2())
		<< " - eta = "  << upTrackerTrack->Eta()
		<< " - phi = "  << upTrackerTrack->Phi()
		<< std::endl;
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
      //h_upperTrackEta->Fill(*upTrackerTrackEta);
      //h_upperTrackPhi->Fill(*upTrackerTrackPhi);
      h_upperTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

      for(int i = 0; i < nBiasBins; ++i){

	h_upperCurvePlusBias[i]->Fill(upperCpT + (i + 1)*(maxBias/nBiasBins));
	h_upperCurveMinusBias[i]->Fill(upperCpT - (i + 1)*(maxBias/nBiasBins));	
	if (debug)
	  std::cout << "\nMade it through the upper bias loop " << i << std::endl; 
      }
	
	
      double lowerCpT = *lowTrackerCharge / *lowTrackerPt;
      h_lowerChi2->Fill(*lowTrackerChi2);
      h_lowerNdof->Fill(*lowTrackerNdof);
      //	h_lowerPt->Fill(*lowTrackerMuonPt);
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
      //h_lowerTrackEta->Fill(*lowTrackerTrackEta);
      //	h_lowerTrackPhi->Fill(*lowTrackerTrackPhi);
      h_lowerTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	
      for(int i = 0; i < nBiasBins; ++i){

	h_lowerCurvePlusBias[i]->Fill(lowerCpT + (i + 1)*(maxBias/nBiasBins));
	h_lowerCurveMinusBias[i]->Fill(lowerCpT - (i + 1)*(maxBias/nBiasBins));	
	if (debug)
	  std::cout << "\nMade it through the lower bias loop " << i << std::endl;
      }
      j++;
      if (debug)
	std::cout << "\n\nMade it through " << j << " sets of fills\n";	
    }//Closing if fill
    // } // end if tracker.Next
  } // end while loop
  

  std::cout << std::hex << g << std::dec << std::endl;
  
  g->Write();
  g->Close();

  return;
}

