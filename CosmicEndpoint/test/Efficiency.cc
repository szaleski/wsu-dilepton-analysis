#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/Vector3D.h"
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
#include "TTreeReaderArray.h"
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


void Efficiency(std::string const& filelist, std::string const& outFile,
		int trackVal_, double minPt_, double maxBias_, int nBiasBins_,
		double factor_=1.0, bool symmetric_=false, bool debug_=false)
{
  bool debug = debug_;
  
  if (debug) {
    std::cout<<"arg 1 is:  " << filelist   << std::endl;
    std::cout<<"arg 2 is:  " << outFile    << std::endl;
    std::cout<<"arg 3 is:  " << trackVal_  << std::endl;
    std::cout<<"arg 4 is:  " << minPt_     << std::endl;
    std::cout<<"arg 5 is:  " << maxBias_   << std::endl;
    std::cout<<"arg 6 is:  " << nBiasBins_ << std::endl;
    std::cout<<"arg 7 is:  " << factor_    << std::endl;
    std::cout<<"arg 8 is:  " << symmetric_ << std::endl;
  }
  
  TFile *g;
  TChain *myChain;

  std::string trackAlgo;
  std::ofstream lumiFileOut100_loose;
  std::ofstream lumiFileOut200_loose;
  std::ofstream lumiFileOut400_loose;
  std::ofstream lumiFileOut100_tight;
  std::ofstream lumiFileOut200_tight;
  std::ofstream lumiFileOut400_tight;
  
  std::string outname;

  bool istrackerp = false;
  bool istunep    = false;

  if (trackVal_== 1) {
    myChain   = new TChain(TString("analysisTrackerMuons/MuonTree"));
    outname   = "TrackerOnly";
    trackAlgo = "trackerOnly";
    // if using TrackerOnly, should *not* apply muon system cuts
    istrackerp = true;
  }  
  else if (trackVal_== 2) {
    myChain   = new TChain(TString("analysisTPFMSMuons/MuonTree"));
    outname   = "TPFMS";
    trackAlgo = "tpfms";
  }  
  else if (trackVal_== 3) {
    myChain   = new TChain(TString("analysisDYTMuons/MuonTree"));
    outname   = "DYTT";
    trackAlgo = "dyt";
  } 
  else if (trackVal_== 4) {
    myChain   = new TChain(TString("analysisPickyMuons/MuonTree"));
    outname   = "Picky";
    trackAlgo = "picky";
  } 
  else if (trackVal_== 5) {
    myChain   = new TChain(TString("analysisTunePMuons/MuonTree"));
    outname   = "TuneP";
    trackAlgo = "tuneP";
    // if using TuneP and pT < 200, should *not* apply muon system cuts
    istrackerp = true;
    istunep    = true;
  } 
  else {
    std::cout << "INVALID TRACK SPECIFIED! Choose a value between [1, 5]" << std::endl;
    return;
  }  

  std::cout << "chose the track object"  << std::endl;

  std::stringstream outrootfile, outlumifile;
  std::cout << "checking for OUTPUTDIR " << std::endl;
  const char* envvar = std::getenv("OUTPUTDIR");
  if (envvar) {
    std::string outdir = std::string(envvar);
    outrootfile << outdir << "/" << outFile << outname;
    outlumifile << outdir << "/" << outFile << trackAlgo;
    std::cout << "checked for found OUTPUTDIR "
	      << outdir << std::endl;
  }
  else {
    outrootfile << outFile << outname;
    outlumifile << outFile << trackAlgo;
  }

  std::cout << "Output files "
	    << outrootfile.str() << std::endl
	    << outlumifile.str() << std::endl;

  g = new TFile(TString(outrootfile.str()+".root"),"RECREATE"); 

  std::cout << "Processing tracks from " << trackAlgo << " algorithm" << std::endl;
  
  std::string name;
  std::stringstream inputfiles;

  std::cout << "checking for AFSJOBDIR " << std::endl;
  const char* afsvar = std::getenv("AFSJOBDIR");
  if (afsvar) {
    std::string jobdir = std::string(afsvar);
    inputfiles << jobdir << "/" << filelist;
  }
  else { 
    // what if AFSJOBDIR is not set, then just assume current working directory
    inputfiles << "./" << filelist;
  }
  
  std::ifstream file(inputfiles.str());
  std:: cout << "opening input file list "
	     << inputfiles.str() << std::hex << "  " << file << std::endl;

  while (std::getline(file,name)) {
    std::stringstream newString;
    newString << "root://xrootd.unl.edu//" << name;

    //Use the following line with line above commented out for running on local files.
    //newString << name;
    std::cout << newString.str() << std::endl;
    myChain->Add(TString(newString.str()));
  }
  std::cout << "Successfully opened inputfiles list!" << std::endl;
  //  myChain->Add(TString());
  //   newString << "root://xrootd.unl.edu//" << name;
 
  TTree *myTree = myChain;
  TTreeReader trackReader(myTree);

  TH1F *h_Pt       = new TH1F("Pt",    "Pt",    300,  0., 3000.);
  TH1F *h_Eta      = new TH1F("Eta",   "Eta",    40, -2.,    2.);
  TH1F *h_Phi      = new TH1F("Phi",   "Phi",    40, -4.,    4.);

  TH1F *h_TrackPt  = new TH1F("TrackPt", "TrackPt",  300, 0., 3000.);
  TH1F *h_TrackEta = new TH1F("TrackEta","TrackEta", 40, -2., 2.);
  TH1F *h_TrackPhi = new TH1F("TrackPhi","TrackPhi", 40, -4., 4.);

  TH1F *h_passRECOPt       = new TH1F("passRECOPt",    "passRECOPt",    300,  0., 3000.);
  TH1F *h_passRECOEta      = new TH1F("passRECOEta",   "passRECOEta",    40, -2.,    2.);
  TH1F *h_passRECOPhi      = new TH1F("passRECOPhi",   "passRECOPhi",    40, -4.,    4.);

  TH1F *h_passRECOTrackPt  = new TH1F("passRECOTrackPt", "passRECOTrackPt",  300, 0., 3000.);
  TH1F *h_passRECOTrackEta = new TH1F("passRECOTrackEta","passRECOTrackEta", 40, -2., 2.);
  TH1F *h_passRECOTrackPhi = new TH1F("passRECOTrackPhi","passRECOTrackPhi", 40, -4., 4.);

  TH1F *h_passIDPt       = new TH1F("passIDPt",    "passIDPt",    300,  0., 3000.);
  TH1F *h_passIDEta      = new TH1F("passIDEta",   "passIDEta",    40, -2.,    2.);
  TH1F *h_passIDPhi      = new TH1F("passIDPhi",   "passIDPhi",    40, -4.,    4.);

  TH1F *h_passIDTrackPt  = new TH1F("passIDTrackPt", "passIDTrackPt",  300, 0., 3000.);
  TH1F *h_passIDTrackEta = new TH1F("passIDTrackEta","passIDTrackEta", 40, -2., 2.);
  TH1F *h_passIDTrackPhi = new TH1F("passIDTrackPhi","passIDTrackPhi", 40, -4., 4.);

  TH1F *h_passRECOIDPt       = new TH1F("passRECOIDPt",    "passRECOIDPt",    300,  0., 3000.);
  TH1F *h_passRECOIDEta      = new TH1F("passRECOIDEta",   "passRECOIDEta",    40, -2.,    2.);
  TH1F *h_passRECOIDPhi      = new TH1F("passRECOIDPhi",   "passRECOIDPhi",    40, -4.,    4.);

  TH1F *h_passRECOIDTrackPt  = new TH1F("passRECOIDTrackPt", "passRECOIDTrackPt",  300, 0., 3000.);
  TH1F *h_passRECOIDTrackEta = new TH1F("passRECOIDTrackEta","passRECOIDTrackEta", 40, -2., 2.);
  TH1F *h_passRECOIDTrackPhi = new TH1F("passRECOIDTrackPhi","passRECOIDTrackPhi", 40, -4., 4.);

  std::cout << "Creating upper muMinus TTreeReaderValues" << std::endl;
  TTreeReaderValue<int>    run(  trackReader, "run"  );
  TTreeReaderValue<int>    lumi( trackReader, "lumi"  );
  if (lumi.GetSetupStatus() < 0) {
    std::cerr << "Error " << lumi.GetSetupStatus()
	      << "setting up reader for " << lumi.GetBranchName() << '\n';
  }
  TTreeReaderValue<int>    event(trackReader, "event");
  if (event.GetSetupStatus() < 0) {
    std::cerr << "Error " << event.GetSetupStatus()
	      << "setting up reader for " << event.GetBranchName() << '\n';
  }
  
  TTreeReaderValue<int> nMuons(    trackReader, "nMuons"    );
  TTreeReaderValue<int> nUpperLegs(trackReader, "nUpperLegs");
  TTreeReaderValue<int> nLowerLegs(trackReader, "nLowerLegs");

  TTreeReaderArray<math::XYZTLorentzVector> MuonP4(trackReader,"muonP4"  );
  TTreeReaderArray<math::XYZVector>         Track( trackReader,"trackVec");
  if (Track.GetSetupStatus() < 0) {
    std::cerr << "Error " << Track.GetSetupStatus()
	      << "setting up reader for " << Track.GetBranchName() << '\n';
  }
  
  TTreeReaderArray<int>    isGlobal(    trackReader, "isGlobal"    );
  if (isGlobal.GetSetupStatus() < 0) {
    std::cerr << "Error " << isGlobal.GetSetupStatus()
	      << "setting up reader for " << isGlobal.GetBranchName() << '\n';
  }
  TTreeReaderArray<int>    isTracker(   trackReader, "isTracker"   );
  TTreeReaderArray<int>    isStandAlone(trackReader, "isStandAlone");

  TTreeReaderArray<double> globalPt(      trackReader, "globalpT" );
  TTreeReaderArray<double> globalEta(     trackReader, "globalEta");
  TTreeReaderArray<double> globalPhi(     trackReader, "globalPhi");

  TTreeReaderArray<double> trackPt( trackReader, "trackpT" );
  TTreeReaderArray<double> trackEta(trackReader, "trackEta");
  TTreeReaderArray<double> trackPhi(trackReader, "trackPhi");
  TTreeReaderArray<int>    Charge(  trackReader, "charge"  );
  TTreeReaderArray<double> Chi2(    trackReader, "chi2"    );
  TTreeReaderArray<int>    Ndof(    trackReader, "ndof"    );
  TTreeReaderArray<double> Dxy(     trackReader, "dxy"     );
  TTreeReaderArray<double> Dz(      trackReader, "dz"      );
  TTreeReaderArray<double> DxyError(trackReader, "dxyError");
  TTreeReaderArray<double> DzError( trackReader, "dzError" );
  TTreeReaderArray<double> PtError( trackReader, "ptError" );

  TTreeReaderArray<int> Phits(                trackReader, "pixelHits"       );
  TTreeReaderArray<int> Thits(                trackReader, "trackerHits"     );
  TTreeReaderArray<int> Mhits(                trackReader, "muonStationHits" );
  TTreeReaderArray<int> ValidHits(            trackReader, "nValidHits"      );
  TTreeReaderArray<int> ValidMuonHits(        trackReader, "nValidMuonHits"  );
  TTreeReaderArray<int> MatchedMuonStations(  trackReader, "nMatchedStations");
  TTreeReaderArray<int> LayersWithMeasurement(trackReader, "tkLayersWMeas"   );
  
  if (debug)
    std::cout << "Made it to Histogramming!" << std::endl;
  int j = 0;
  double maxDR = 0.15; // what is reasonable here? Aachen did dPhi < 0.1, dTheta (eta?) < 0.05
  while (trackReader.Next()) {
    if (debug)
      std::cout << "Made it into the first loop" << std::endl;
    g->cd();
    
    // only consider events with at most one upper/lower leg for now
    if (*nUpperLegs > 1 && *nLowerLegs > 1)
      continue;
    
    for (int muIdx = 0; muIdx < *nMuons; ++ muIdx) {
      // make sure we're not reading from the skipped events
      if (debug)
	std::cout << "looping over muons on index " << muIdx 
		  << ", chi2=" << Chi2[muIdx] << std::endl;
      //if (Chi2[muIdx] > -1) {
	if (!(trackPt[muIdx] > 0))
	  continue;
	
	double RelPtErr = PtError[muIdx]/(trackPt[muIdx]);
      
	bool MuStationHits = MatchedMuonStations[muIdx] > 1;
	bool ValidMuHits   = (!istrackerp || (istunep && trackPt[muIdx] > 200)) ? ValidMuonHits[muIdx] > 0 : 1;

	bool passRECO = ((RelPtErr   < 0.3) && (Phits[muIdx] > 0  ) && (LayersWithMeasurement[muIdx] > 5)) ? 1 : 0;
	bool passID   = (ValidMuHits && MuStationHits) ? 1 : 0;

	h_Pt->Fill( globalPt[muIdx] );
	h_Eta->Fill(globalEta[muIdx]);
	h_Phi->Fill(globalPhi[muIdx]);
	
	h_TrackPt->Fill(  trackPt[muIdx] );
	h_TrackEta->Fill( trackEta[muIdx]);
	h_TrackPhi->Fill( trackPhi[muIdx]);

	if (passRECO) {
	  h_passRECOPt->Fill( globalPt[muIdx] );
	  h_passRECOEta->Fill(globalEta[muIdx]);
	  h_passRECOPhi->Fill(globalPhi[muIdx]);
	  
	  h_passRECOTrackPt->Fill(  trackPt[muIdx] );
	  h_passRECOTrackEta->Fill( trackEta[muIdx]);
	  h_passRECOTrackPhi->Fill( trackPhi[muIdx]);
	}

	if (passID) {
	  h_passIDPt->Fill( globalPt[muIdx] );
	  h_passIDEta->Fill(globalEta[muIdx]);
	  h_passIDPhi->Fill(globalPhi[muIdx]);
	  
	  h_passIDTrackPt->Fill( trackPt[muIdx] );
	  h_passIDTrackEta->Fill(trackEta[muIdx]);
	  h_passIDTrackPhi->Fill(trackPhi[muIdx]);
	}
	
	if (passRECO && passID) {
	  h_passRECOIDPt->Fill( globalPt[muIdx] );
	  h_passRECOIDEta->Fill(globalEta[muIdx]);
	  h_passRECOIDPhi->Fill(globalPhi[muIdx]);
	  
	  h_passRECOIDTrackPt->Fill( trackPt[muIdx] );
	  h_passRECOIDTrackEta->Fill(trackEta[muIdx]);
	  h_passRECOIDTrackPhi->Fill(trackPhi[muIdx]);
	}
	
	std::cout << "pt="      << trackPt[muIdx]
		  << ",eta="    << trackEta[muIdx]
		  << ",phi="    << trackPhi[muIdx]
		  << ",charge=" << (int)Charge[muIdx]
		  << std::endl
		  << std::endl;
	
	//} // closing if (Chi2[muIdx] > -1)
    } // end for loop
  } // end while loop
  
  g->Write();
  g->Close();
  
  return;
}
