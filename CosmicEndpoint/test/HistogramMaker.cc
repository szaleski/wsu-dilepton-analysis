#include "HistogramMaker.h"

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

namespace wsu {
  namespace dileptons {
    namespace cosmics {
      
      
      HistogramMaker::HistogramMaker(std::string const& fileList,
				     std::string const& outFileName,
				     std::string const& confParmsFile)
      {
	parseConfiguration(confParamsFile);
	
	nBiasBins = confParams.NBiasBins;
	maxBias   = confParams.MaxKBias;
	minPt     = confParams.MinPtCut;
	std::string legs[3]   = {"up", "low", "comb"};
	std::string charge[2] = {"muon", "antiMuon"};
	
	outFile = new TFile(TString(outFileName+".root"),"RECREATE");
	for (int leg = 0; leg < 3; ++ leg) {
	  for (int ch = 0; ch < 3; ++ ch) {
	    // TDirectory* myDirectory = new TDirecto
	    h_Chi2[leg][ch]     = new TH1F(charge[ch]+"_"+legs[leg]+"_Chi2",    "#Chi^{2}",         100,   -0.5,   99.5 );
	    h_Ndof[leg][ch]     = new TH1F(charge[ch]+"_"+legs[leg]+"_Ndof",    "N d.o.f.",         100,   -0.5,   99.5 );
	    h_Chi2Ndof[leg][ch] = new TH1F(charge[ch]+"_"+legs[leg]+"_Chi2Ndof","#Chi^{2}/N d.o.f.",100,   -0.5,   99.5 );
	    h_Charge[leg][ch]   = new TH1F(charge[ch]+"_"+legs[leg]+"_Charge",  "q",                3,     -1.5,   1.5  );
	    h_Curve[leg][ch]    = new TH1F(charge[ch]+"_"+legs[leg]+"_Curve",   "#frac{q}{p_{T}}",  2*500, -0.1,   0.1  );
	    h_Dxy[leg][ch]      = new TH1F(charge[ch]+"_"+legs[leg]+"_Dxy",     "D_{xy}",           2*500, -1000., 1000.);
	    h_Dz[leg][ch]       = new TH1F(charge[ch]+"_"+legs[leg]+"_Dz",      "D_{z}",            2*500, -1000., 1000.);
	    h_DxyError[leg][ch] = new TH1F(charge[ch]+"_"+legs[leg]+"_DxyError","#DeltaD_{xy}",     100,   -100.,  100. );
	    h_DzError[leg][ch]  = new TH1F(charge[ch]+"_"+legs[leg]+"_DzError", "#DeltaD_{z}",      100,   -100.,  100. );
	    h_Pt[leg][ch]       = new TH1F(charge[ch]+"_"+legs[leg]+"_Pt",      "p_{T}",            300,    0.,    3000.);
	    h_TrackPt[leg][ch]  = new TH1F(charge[ch]+"_"+legs[leg]+"_TrackPt", "p_{T}",            300,    0.,    3000.);
	    h_PtError[leg][ch]  = new TH1F(charge[ch]+"_"+legs[leg]+"_PtError", "#Deltap_{T}",      500,    0.,    500. );
	    h_TrackEta[leg][ch] = new TH1F(charge[ch]+"_"+legs[leg]+"_TrackEta","#eta",             2*100, -5.,    5.   );
	    h_TrackPhi[leg][ch] = new TH1F(charge[ch]+"_"+legs[leg]+"_TrackPhi","#phi",             2*50,  -4.,    4.   );
	    
	    h_PixelHits[leg][ch]               = new TH1F(charge[ch]+"_"+legs[leg]+"_PixelHits",         "N_{pix. hits}",
							  100, -0.5, 99.5 );
	    h_TkHits[leg][ch]                  = new TH1F(charge[ch]+"_"+legs[leg]+"_TkHits",            "N_{trk. hits}",
							  100, -0.5, 99.5 );
	    h_MuonStationHits[leg][ch]         = new TH1F(charge[ch]+"_"+legs[leg]+"_MuonStationHits",   "N_{station hits}",
							  20,  -0.5, 19.5 );
	    h_ValidHits[leg][ch]               = new TH1F(charge[ch]+"_"+legs[leg]+"_ValidHits",         "N_{hits}",
							  200, -0.5, 199.5);
	    h_MatchedMuonStations[leg][ch]     = new TH1F(charge[ch]+"_"+legs[leg]+"_MatchedMuonStation","N_{matched stations}",
							  20,  -0.5, 19.5 );
	    h_TkLayersWithMeasurement[leg][ch] = new TH1F(charge[ch]+"_"+legs[leg]+"_TkLayersWithMeas",  "N_{trk. layer w/ meas.}",
							  200, -0.5, 199.5);
	    
	    h_Chi2    ->Sumw2();
	    h_Ndof    ->Sumw2();
	    h_Charge  ->Sumw2();
	    h_Curve   ->Sumw2();
	    h_Dxy     ->Sumw2();
	    h_Dz      ->Sumw2();
	    h_DxyError->Sumw2();
	    h_DzError ->Sumw2();
	    h_Pt      ->Sumw2();
	    h_TrackPt ->Sumw2();
	    h_PtError ->Sumw2();
	    h_TrackEta->Sumw2();
	    h_TrackPhi->Sumw2();
	    
	    h_PixelHits              ->Sumw2();
	    h_TkHits                 ->Sumw2();
	    h_MuonStationHits        ->Sumw2();
	    h_ValidHits              ->Sumw2();
	    h_MatchedMuonStations    ->Sumw2();
	    h_TkLayersWithMeasurement->Sumw2();
	    
	    for (int i = 0; i < nBiasBins; ++i) {
	      std::stringstream name;
	      double biasValue = (maxBias/nBiasBins)*(i+1);
	      name << std::setw(3) << std::setfill('0') << i + 1;
	      TString histname  = TString(charge[ch]+"_"+legs[leg]+"_CurvePlusBias_"+name.str());
	      name.str("");
	      name.clear();
	      name << biasValue;
	      TString histtitle = TString("#frac{q}{p_{T}}+#Delta#kappa("++")");
	      h_CurvePlusBias[leg][ch][i]  = new TH1F(histname, histtitle, 2*500, -0.1, 0.1);
	      
	      histname  = TString(charge[ch]+"_"+legs[leg]+"_CurveMinusBias_"+name.str());
	      name.str("");
	      name.clear();
	      name << biasValue;
	      histtitle = TString("#frac{q}{p_{T}}-#Delta#kappa("++")");
	      h_CurveMinusBias[leg][ch][i] = new TH1F(histname, histtitle, 2*500, -0.1, 0.1);
	      
	      h_CurvePlusBias[leg][ch][i] ->Sumw2();
	      h_CurveMinusBias[leg][ch][i]->Sumw2();
	    }
	  }// end loop on different charge histograms
	}// end loop on different muon leg histograms	
      } // end constructorr
      
      HistogramMaker::~HistogramMaker()
      {
	outFile->Write();
	outFile->Close();
	
	for (int leg = 0; leg < 3; ++ leg) {
	  for (int ch = 0; ch < 3; ++ ch) {
	    delete h_Chi2[leg][ch];
	    delete h_Ndof[leg][ch];
	    delete h_Charge[leg][ch];
	    delete h_Curve[leg][ch];
	    delete h_Dxy[leg][ch];
	    delete h_Dz[leg][ch];
	    delete h_DxyError[leg][ch];
	    delete h_DzError[leg][ch];
	    delete h_Pt[leg][ch];
	    delete h_TrackPt[leg][ch];
	    delete h_PtError[leg][ch];
	    delete h_TrackEta[leg][ch];
	    delete h_TrackPhi[leg][ch];
	    
	    delete h_PixelHits[leg][ch];
	    delete h_TkHits[leg][ch];
	    delete h_MuonStationHits[leg][ch];
	    delete h_ValidHits[leg][ch];
	    delete h_MatchedMuonStations[leg][ch];
	    delete h_TkLayersWithMeasurement[leg][ch];
	    
	    h_Chi2[leg][ch]      = NULL;
	    h_Ndof[leg][ch]      = NULL;
	    h_Charge[leg][ch]    = NULL;
	    h_Curve[leg][ch]     = NULL;
	    h_Dxy[leg][ch]       = NULL;
	    h_Dz[leg][ch]        = NULL;
	    h_DxyError[leg][ch]  = NULL;
	    h_DzError[leg][ch]   = NULL;
	    h_Pt[leg][ch]        = NULL;
	    h_TrackPt[leg][ch]   = NULL;
	    h_PtError[leg][ch]   = NULL;
	    h_TrackEta[leg][ch]  = NULL;
	    h_TrackPhi[leg][ch]  = NULL;
	    
	    h_PixelHits[leg][ch]               = NULL;
	    h_TkHits[leg][ch]                  = NULL;
	    h_MuonStationHits[leg][ch]         = NULL;
	    h_ValidHits[leg][ch]               = NULL;
	    h_MatchedMuonStations[leg][ch]     = NULL;
	    h_TkLayersWithMeasurement[leg][ch] = NULL;
	    
	    for (int i = 0; i < nBiasBins; ++i) {
	      delete h_CurvePlusBias[leg][ch][i];
	      delete h_CurveMinusBias[leg][ch][i];
	      
	      h_CurvePlusBias[leg][ch][i]  = NULL;
	      h_CurveMinusBias[leg][ch][i] = NULL;	
	    }
	  }// end loop on different charge histograms
	}// end loop on different muon leg histograms

	delete outFile;
	outFile = NULL;
      } // end destructor

      
      void HistogramMaker::parseConfiguration(std::string const& confFileName)
      {
	return;
	std::stringstream treeName;
	treeName << "analysis" << confParams.TrackAlgo << "Muons/MuonTree";
	treeReader = new TTreeReader(TString(treeName.str()));
      }
      
      void HistogramMaker::parseFileList(std::string const& fileList)
      {
	return;
      }
      

      void HistogramMaker::Plot(TTree* tree)
      {
	maxBias     = 0.05;
	nBiasBins   = 100;
	massBinSize = 200;
	mMin = 0.;
	mMax = 100.;

	std::cout << "\nCreating upper Muon TTreeReaderValues\n";
	TTreeReaderValue<Double_t> upTrackerPt(       *treeReader, "upperMuon_trackPt"          );
	TTreeReaderValue<Double_t> upTrackerDxy(      *treeReader, "upperMuon_dxy"              );
	TTreeReaderValue<Double_t> upTrackerDz(       *treeReader, "upperMuon_dz"               );
	TTreeReaderValue<Int_t>    upTrackerPhits(    *treeReader, "upperMuon_pixelHits"        );
	TTreeReaderValue<Int_t>    upTrackerCharge(   *treeReader, "upperMuon_charge"           );
	TTreeReaderValue<Int_t>    upTrackerThits(    *treeReader, "upperMuon_trackerHits"      );
	TTreeReaderValue<Int_t>    upTrackerMhits(    *treeReader, "upperMuon_muonStationHits"  );
	TTreeReaderValue<Double_t> upTrackerChi2(     *treeReader, "upperMuon_chi2"             );
	TTreeReaderValue<Int_t>    upTrackerNdof(     *treeReader, "upperMuon_ndof"             );
	TTreeReaderValue<Int_t>    upTrackerValidHits(*treeReader, "upperMuon_numberOfValidHits");
	TTreeReaderValue<Double_t> upTrackerDxyError( *treeReader, "upperMuon_dxyError"         );
	TTreeReaderValue<Double_t> upTrackerDzError(  *treeReader, "upperMuon_dzError"          );
	TTreeReaderValue<Double_t> upTrackerPtError(  *treeReader, "upperMuon_ptError"          );
	TTreeReaderValue<Int_t>    upTrackerMatchedMuonStations(  *treeReader,"upperMuon_numberOfMatchedStations");
	TTreeReaderValue<Int_t>    upTrackerLayersWithMeasurement(*treeReader,"upperMuon_trackerLayersWithMeasurement");

	TTreeReaderValue<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > upTrackerMuonP4(*treeReader,"upperMuon_P4");
	TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > upTrackerTrack(*treeReader,"upperMuon_trackVec");


	std::cout << "\nCreating lower Muon TTreeReaderValues\n";
	TTreeReaderValue<Double_t> lowTrackerPt(       *treeReader, "lowerMuon_trackPt"          );
	TTreeReaderValue<Double_t> lowTrackerDxy(      *treeReader, "lowerMuon_dxy"              );
	TTreeReaderValue<Double_t> lowTrackerDz(       *treeReader, "lowerMuon_dz"               );
	TTreeReaderValue<Int_t>    lowTrackerPhits(    *treeReader, "lowerMuon_pixelHits"        );
	TTreeReaderValue<Int_t>    lowTrackerCharge(   *treeReader, "lowerMuon_charge"           );
	TTreeReaderValue<Int_t>    lowTrackerThits(    *treeReader, "lowerMuon_trackerHits"      );
	TTreeReaderValue<Int_t>    lowTrackerMhits(    *treeReader, "lowerMuon_muonStationHits"  );
	TTreeReaderValue<Double_t> lowTrackerChi2(     *treeReader, "lowerMuon_chi2"             );
	TTreeReaderValue<Int_t>    lowTrackerNdof(     *treeReader, "lowerMuon_ndof"             );
	TTreeReaderValue<Int_t>    lowTrackerValidHits(*treeReader, "lowerMuon_numberOfValidHits");
	TTreeReaderValue<Double_t> lowTrackerDxyError( *treeReader, "lowerMuon_dxyError"         );
	TTreeReaderValue<Double_t> lowTrackerDzError(  *treeReader, "lowerMuon_dzError"          );
	TTreeReaderValue<Double_t> lowTrackerPtError(  *treeReader, "lowerMuon_ptError"          );
	TTreeReaderValue<Int_t>    lowTrackerMatchedMuonStations(  *treeReader,"lowerMuon_numberOfMatchedStations"     );
	TTreeReaderValue<Int_t>    lowTrackerLayersWithMeasurement(*treeReader,"lowerMuon_trackerLayersWithMeasurement");
	
	TTreeReaderValue<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > lowTrackerMuonP4(*treeReader,"lowerMuon_P4");
	TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > lowTrackerTrack(*treeReader,"lowerMuon_trackVec");
  
	//note, the old way would only loop to the maximum number of events in the smallest file
	// this checks that at least one of the files still has events to process,
	// but only fills histograms when there are still events
	//  bool fchk = treeReader.Next();
	// bool gchk = newReader.Next();
	//  bool echk = nextReader.Next();
	std::cout << "\nMade it to Histogramming!\n";
	int j = 0;
	bool debug = false;
	while (treeReader->Next()){
	  if (debug)
	    std::cout << "\nMade it into the first loop\n" << std::endl;
	  g->cd();
    
	  if(*upTrackerChi2 > -1000) { //ensure values are from an actual event
	    double upCpT = (*upTrackerCharge)/(upTrackerTrack->Pt());
	    h_Chi2[0][     (charge < 0) ? 0 : 1]->Fill(*upTrackerChi2);
	    h_Ndof[0][     (charge < 0) ? 0 : 1]->Fill(*upTrackerNdof);
	    h_Chi2Ndof[0][ (charge < 0) ? 0 : 1]->Fill((*upTrackerChi2)/(*upTrackerNdof));
	    h_Pt[0][       (charge < 0) ? 0 : 1]->Fill(*upTrackerMuonPt);
	    h_Charge[0][   (charge < 0) ? 0 : 1]->Fill(*upTrackerCharge);
	    h_Curve[0][    (charge < 0) ? 0 : 1]->Fill(upCpT);
	    h_Dxy[0][      (charge < 0) ? 0 : 1]->Fill(*upTrackerDxy);
	    h_Dz[0][       (charge < 0) ? 0 : 1]->Fill(*upTrackerDz);
	    h_PtError[0][  (charge < 0) ? 0 : 1]->Fill(*upTrackerPtError);
	    h_DxyError[0][ (charge < 0) ? 0 : 1]->Fill(*upTrackerDxyError);
	    h_DzError[0][  (charge < 0) ? 0 : 1]->Fill(*upTrackerDzError);
	    h_TrackPt[0][  (charge < 0) ? 0 : 1]->Fill(upTrackerTrack->Pt());
	    h_TrackEta[0][ (charge < 0) ? 0 : 1]->Fill(upTrackerTrack->Eta());
	    h_TrackPhi[0][ (charge < 0) ? 0 : 1]->Fill(upTrackerTrack->Phi());

	    h_PixelHits[0][                   (charge < 0) ? 0 : 1]->Fill(*upTrackerPhits);
	    h_TrackerHits[0][                 (charge < 0) ? 0 : 1]->Fill(*upTrackerThits);
	    h_MuonStationHits[0][             (charge < 0) ? 0 : 1]->Fill(*upTrackerMhits);
	    h_ValidHits[0][                   (charge < 0) ? 0 : 1]->Fill(*upTrackerValidHits);
	    h_MatchedMuonStations[0][         (charge < 0) ? 0 : 1]->Fill(*upTrackerMatchedMuonStations);
	    h_TrackerLayersWithMeasurement[0][(charge < 0) ? 0 : 1]->Fill(*upTrackerLayersWithMeasurement);
	
	    for (int i = 0; i < nBiasBins; ++i) {
	      h_CurvePlusBias[0][ (charge < 0) ? 0 : 1][i]->Fill(upCpT + (i+1)*(maxBias/nBiasBins));
	      h_CurveMinusBias[0][(charge < 0) ? 0 : 1][i]->Fill(upCpT - (i+1)*(maxBias/nBiasBins));	
	      if (debug)
		std::cout << "\nMade it through the  bias loop " << i << std::endl;
	    }
	
	    double lowerCpT = (*lowTrackerCharge)/(*lowTrackerPt);
	    h_Chi2[1][     (charge < 0) ? 0 : 1]->Fill(*lowTrackerChi2);
	    h_Ndof[1][     (charge < 0) ? 0 : 1]->Fill(*lowTrackerNdof);
	    h_Pt[1][       (charge < 0) ? 0 : 1]->Fill(*lowTrackerMuonPt);
	    h_Charge[1][   (charge < 0) ? 0 : 1]->Fill(*lowTrackerCharge);
	    h_Curve[1][    (charge < 0) ? 0 : 1]->Fill(lowCpT);
	    h_Dxy[1][      (charge < 0) ? 0 : 1]->Fill(*lowTrackerDxy);
	    h_Dz[1][       (charge < 0) ? 0 : 1]->Fill(*lowTrackerDz);
	    h_PtError[1][  (charge < 0) ? 0 : 1]->Fill(*lowTrackerPtError);
	    h_DxyError[1][ (charge < 0) ? 0 : 1]->Fill(*lowTrackerDxyError);
	    h_DzError[1][  (charge < 0) ? 0 : 1]->Fill(*lowTrackerDzError);
	    h_TrackPt[1][  (charge < 0) ? 0 : 1]->Fill(lowTrackerTrack->Pt());
	    h_TrackEta[1][ (charge < 0) ? 0 : 1]->Fill(lowTrackerTrack->Eta());
	    h_TrackPhi[1][ (charge < 0) ? 0 : 1]->Fill(lowTrackerTrack->Phi());

	    h_PixelHits[1][                   (charge < 0) ? 0 : 1]->Fill(*lowTrackerPhits);
	    h_TrackerHits[1][                 (charge < 0) ? 0 : 1]->Fill(*lowTrackerThits);
	    h_MuonStationHits[1][             (charge < 0) ? 0 : 1]->Fill(*lowTrackerMhits);
	    h_ValidHits[1][                   (charge < 0) ? 0 : 1]->Fill(*lowTrackerValidHits);
	    h_MatchedMuonStations[1][         (charge < 0) ? 0 : 1]->Fill(*lowTrackerMatchedMuonStations);
	    h_TrackerLayersWithMeasurement[1][(charge < 0) ? 0 : 1]->Fill(*lowTrackerLayersWithMeasurement);
	
	    for (int i = 0; i < nBiasBins; ++i) {
	      h_CurvePlusBias[1][ (charge < 0) ? 0 : 1][i]->Fill(lowCpT + (i+1)*(maxBias/nBiasBins));
	      h_CurveMinusBias[1][(charge < 0) ? 0 : 1][i]->Fill(lowCpT - (i+1)*(maxBias/nBiasBins));	
	      if (debug)
		std::cout << "\nMade it through the  bias loop " << i << std::endl;
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

    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu
