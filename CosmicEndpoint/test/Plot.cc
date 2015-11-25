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

void Plot(std::string const& filelist, std::string const& outFile,
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

  lumiFileOut100_loose.open(outlumifile.str()+"_pt100_loose.txt");
  lumiFileOut200_loose.open(outlumifile.str()+"_pt200_loose.txt");
  lumiFileOut400_loose.open(outlumifile.str()+"_pt400_loose.txt");
  
  lumiFileOut100_tight.open(outlumifile.str()+"_pt100_tight.txt");
  lumiFileOut200_tight.open(outlumifile.str()+"_pt200_tight.txt");
  lumiFileOut400_tight.open(outlumifile.str()+"_pt400_tight.txt");
  
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

  float maxBias = maxBias_;
  int nBiasBins = nBiasBins_;
  
  std::string counterBinLabels[55] = {
    "00 - Uncut",                                // 0
    "01 - #frac{#Deltap_{T}}{p_{T}} N-1 (loose)",// 1
    "02 - N_{Trk Hits} N-1 (loose)",             // 2
    "03 - N_{Pix Hits} N-1 (loose)",             // 3
    "04 - N_{Valid #mu Hits} N-1 (loose)",       // 4
    "05 - N_{Mt. Sta. Hits} N-1 (loose)",        // 5
    "06 - D_{xy} N-1 (loose)",                   // 6
    "07 - D_{z} N-1 (loose)",                    // 7
    "08 - p_{T} N-1 (loose)",                    // 8
    "09",
    "10",

    "11 - #frac{#Deltap_{T}}{p_{T}} N-1 (w/ D_{xy})",// 11
    "12 - N_{Trk Hits} N-1 (w/ D_{xy})",             // 12
    "13 - N_{Pix Hits} N-1 (w/ D_{xy})",             // 13
    "14 - N_{Valid #mu Hits} N-1 (w/ D_{xy})",       // 14
    "15 - N_{Mt. Sta. Hits} N-1 (w/ D_{xy})",        // 15
    "16 - D_{xy} N-1 (w/ D_{xy})",                   // 16
    "17 - D_{z} N-1 (w/ D_{xy})",                    // 17
    "18 - p_{T} N-1 (w/ D_{xy})",                    // 18
    "19",
    "20",

    "21 - #frac{#Deltap_{T}}{p_{T}} N-1 (w/ D_{z})",// 21
    "22 - N_{Trk Hits} N-1 (w/ D_{z})",             // 22
    "23 - N_{Pix Hits} N-1 (w/ D_{z})",             // 23
    "24 - N_{Valid #mu Hits} N-1 (w/ D_{z})",       // 24
    "25 - N_{Mt. Sta. Hits} N-1 (w/ D_{z})",        // 25
    "26 - D_{xy} N-1 (w/ D_{z})",                   // 26
    "27 - D_{z} N-1 (w/ D_{z})",                    // 27
    "28 - p_{T} N-1 (w/ D_{z})",                    // 28
    "29",
    "30",

    "31 - #frac{#Deltap_{T}}{p_{T}} N-1 (tight)",// 31
    "32 - N_{Trk Hits} N-1 (tight)",             // 32
    "33 - N_{Pix Hits} N-1 (tight)",             // 33
    "34 - N_{Valid #mu Hits} N-1 (tight)",       // 34
    "35 - N_{Mt. Sta. Hits} N-1 (tight)",        // 35
    "36 - D_{xy} N-1 (tight)",                   // 36
    "37 - D_{z} N-1 (tight)",                    // 37
    "38 - p_{T} N-1 (tight)",                    // 38
    "39",

    "40 - p_{T} > 50",   // 40
    "41 - p_{T} > 100",  // 41
    "42 - p_{T} > 150",  // 42
    "43 - p_{T} > 200",  // 43
    "44 - p_{T} > 300",  // 44
    "45 - p_{T} > 400",  // 45
    "46 - p_{T} > 500",  // 46
    "47 - p_{T} > 1000", // 47
    "48 - p_{T} > 1500", // 48
    "49 - p_{T} > 2000", // 49
    "50 - p_{T} > 3000", // 50
    "51",
    "52",
    "53",
    "54"
  };
  
  TH1I *h_countersUpper = new TH1I("upperCounters","upperCounters",55, -0.5, 54.5);
  TH1I *h_countersLower = new TH1I("lowerCounters","lowerCounters",55, -0.5, 54.5);
  
  for (int b = 0; b < 55; ++b) {
    h_countersUpper->GetXaxis()->SetBinLabel(b+1,TString(counterBinLabels[b]));
    h_countersLower->GetXaxis()->SetBinLabel(b+1,TString(counterBinLabels[b]));
  }

  // histograms for upper leg muons, inclusive
  TH1F *h_upperPt       = new TH1F("upperPt",    "upperPt",    300,  0., 3000.);
  TH1F *h_upperEta      = new TH1F("upperEta",   "upperEta",    40, -2.,    2.);
  TH1F *h_upperPhi      = new TH1F("upperPhi",   "upperPhi",    40, -4.,    4.);
  TH1F *h_upperChi2     = new TH1F("upperChi2",  "upperChi2",   50,  0.,  150.);
  TH1F *h_upperNdof     = new TH1F("upperNdof",  "upperNdof",   100, -0.5, 99.5);
  TH1F *h_upperCharge   = new TH1F("upperCharge","upperCharge", 3, -1.5, 1.5);
  TH1F *h_upperCurve    = new TH1F("upperCurve", "upperCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_upperDxy      = new TH1F("upperDxy",   "upperDxy",    100, -100., 100.);
  TH1F *h_upperDz       = new TH1F("upperDz",    "upperDz",     100, -250., 250.);
  TH1F *h_upperDxyError = new TH1F("upperDxyError","upperDxyError", 50, 0., 150.);
  TH1F *h_upperDzError  = new TH1F("upperDzError", "upperDzError",  50, 0., 150.);
  TH1F *h_upperTrackPt  = new TH1F("upperTrackPt", "upperTrackPt",  300, 0., 3000.);
  TH1F *h_upperTrackEta = new TH1F("upperTrackEta","upperTrackEta", 40, -2., 2.);
  TH1F *h_upperTrackPhi = new TH1F("upperTrackPhi","upperTrackPhi", 40, -4., 4.);
  TH1F *h_upperPtError  = new TH1F("upperPtError", "upperPtError",  100, 0., 600.);
  TH1F *h_upperPtRelErr = new TH1F("upperPtRelErr","upperPtRelErr", 100, 0., 1.);

  TH1F *h_upperPixelHits                    = new TH1F("upperPixelHits",                   "upperPixelHits",           10, -0.5, 9.5 );
  TH1F *h_upperTrackerHits                  = new TH1F("upperTrackerHits",                 "upperTrackerHits",         35, -0.5, 34.5);
  TH1F *h_upperMuonStationHits              = new TH1F("upperMuonStationHits",             "upperMuonStationHits",     10, -0.5, 9.5 );
  TH1F *h_upperValidHits                    = new TH1F("upperValidHits",                   "upperValidHits",           100,-0.5, 99.5);
  TH1F *h_upperValidMuonHits                = new TH1F("upperValidMuonHits",               "upperValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_upperMatchedMuonStations          = new TH1F("upperMatchedMuonStations",         "upperMatchedMuonStations", 10, -0.5, 9.5 );
  TH1F *h_upperTrackerLayersWithMeasurement = new TH1F("upperTrackerLayersWithMeasurement","upperTrackerLayersWithMeasurement", 20, -0.5, 19.5);

  TH1F *h_upperCurvePlusBias[nBiasBins];
  TH1F *h_upperCurveMinusBias[nBiasBins];
  for (int i = 0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_upperCurvePlusBias[i]  = new TH1F(TString("upperCurvePlusBias" + name.str()), TString("upperCurvePlusBias" + name.str()),  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_upperCurveMinusBias[i] = new TH1F(TString("upperCurveMinusBias" + name.str()),TString("upperCurveMinusBias" + name.str()), 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
  }

  // histograms for lower leg muons, inclusive
  TH1F *h_lowerPt       = new TH1F("lowerPt", "lowerPt",  300, 0., 3000.);
  TH1F *h_lowerEta      = new TH1F("lowerEta","lowerEta", 40, -2., 2.   );
  TH1F *h_lowerPhi      = new TH1F("lowerPhi","lowerPhi", 40, -4., 4.   );
  TH1F *h_lowerChi2     = new TH1F("lowerChi2",  "lowerChi2",   50,   0.,  150.);
  TH1F *h_lowerNdof     = new TH1F("lowerNdof",  "lowerNdof",   100, -0.5, 99.5);
  TH1F *h_lowerCharge   = new TH1F("lowerCharge","lowerCharge", 3,   -1.5, 1.5 );
  TH1F *h_lowerCurve    = new TH1F("lowerCurve", "lowerCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_lowerDxy      = new TH1F("lowerDxy",   "lowerDxy",    100, -100., 100.);
  TH1F *h_lowerDz       = new TH1F("lowerDz",    "lowerDz",     100, -250., 250.);
  TH1F *h_lowerDxyError = new TH1F("lowerDxyError","lowerDxyError", 50, 0., 150.);
  TH1F *h_lowerDzError  = new TH1F("lowerDzError", "lowerDzError",  50, 0., 150.);
  TH1F *h_lowerTrackPt  = new TH1F("lowerTrackPt", "lowerTrackPt",  300, 0., 3000.);
  TH1F *h_lowerTrackEta = new TH1F("lowerTrackEta","lowerTrackEta", 40, -2., 2.);
  TH1F *h_lowerTrackPhi = new TH1F("lowerTrackPhi","lowerTrackPhi", 40, -4., 4.);
  TH1F *h_lowerPtError  = new TH1F("lowerPtError", "lowerPtError",  100, 0., 600.);
  TH1F *h_lowerPtRelErr = new TH1F("lowerPtRelErr","lowerPtRelErr", 100, 0., 1.);

  TH1F *h_lowerPixelHits           = new TH1F("lowerPixelHits",          "lowerPixelHits",           10, -0.5, 9.5 );
  TH1F *h_lowerTrackerHits         = new TH1F("lowerTrackerHits",        "lowerTrackerHits",         35, -0.5, 34.5);
  TH1F *h_lowerMuonStationHits     = new TH1F("lowerMuonStationHits",    "lowerMuonStationHits",     10, -0.5, 9.5 );
  TH1F *h_lowerValidHits           = new TH1F("lowerValidHits",          "lowerValidHits",           100,-0.5, 99.5);
  TH1F *h_lowerValidMuonHits       = new TH1F("lowerValidMuonHits",      "lowerValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_lowerMatchedMuonStations = new TH1F("lowerMatchedMuonStations","lowerMatchedMuonStations", 10, -0.5, 9.5 );
  TH1F *h_lowerTrackerLayersWithMeasurement = new TH1F("lowerTrackerLayersWithMeasurement","lowerTrackerLayersWithMeasurement", 20, -0.5, 19.5);

  TH1F *h_lowerCurvePlusBias[nBiasBins];
  TH1F *h_lowerCurveMinusBias[nBiasBins];
  for (int i = 0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_lowerCurvePlusBias[i]  = new TH1F(TString("lowerCurvePlusBias" + name.str()), TString("lowerCurvePlusBias" + name.str()),  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_lowerCurveMinusBias[i] = new TH1F(TString("lowerCurveMinusBias" + name.str()),TString("lowerCurveMinusBias" + name.str()), 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
  }
  
  // histograms for no cuts
  TH1F *h_muMinusPt      = new TH1F("muMinusPt",     "muMinusPt",      300, 0., 3000.);
  TH1F *h_muUpperMinusPt = new TH1F("muUpperMinusPt","muUpperMinusPt", 300, 0., 3000.);
  TH1F *h_muPlusPt       = new TH1F("muPlusPt",      "muPlusPt",       300, 0., 3000.);
  TH1F *h_muUpperPlusPt  = new TH1F("muUpperPlusPt", "muUpperPlusPt",  300, 0., 3000.);
  TH1F *h_muLowerMinusPt = new TH1F("muLowerMinusPt","muLowerMinusPt", 300, 0., 3000.);
  TH1F *h_muLowerPlusPt  = new TH1F("muLowerPlusPt", "muLowerPlusPt",  300, 0., 3000.);
  TH1F *h_muMinusEta      = new TH1F("muMinusEta",     "muMinusEta",      40, -2., 2.);
  TH1F *h_muUpperMinusEta = new TH1F("muUpperMinusEta","muUpperMinusEta", 40, -2., 2.);
  TH1F *h_muPlusEta       = new TH1F("muPlusEta",      "muPlusEta",       40, -2., 2.);
  TH1F *h_muUpperPlusEta  = new TH1F("muUpperPlusEta", "muUpperPlusEta",  40, -2., 2.);
  TH1F *h_muLowerMinusEta = new TH1F("muLowerMinusEta","muLowerMinusEta", 40, -2., 2.);
  TH1F *h_muLowerPlusEta  = new TH1F("muLowerPlusEta", "muLowerPlusEta",  40, -2., 2.);
  TH1F *h_muMinusPhi      = new TH1F("muMinusPhi",     "muMinusPhi",      40, -4., 4.);
  TH1F *h_muUpperMinusPhi = new TH1F("muUpperMinusPhi","muUpperMinusPhi", 40, -4., 4.);
  TH1F *h_muPlusPhi       = new TH1F("muPlusPhi",      "muPlusPhi",       40, -4., 4.);
  TH1F *h_muUpperPlusPhi  = new TH1F("muUpperPlusPhi", "muUpperPlusPhi",  40, -4., 4.);
  TH1F *h_muLowerMinusPhi = new TH1F("muLowerMinusPhi","muLowerMinusPhi", 40, -4., 4.);
  TH1F *h_muLowerPlusPhi  = new TH1F("muLowerPlusPhi", "muLowerPlusPhi",  40, -4., 4.);

  TH1F *h_muMinusChi2      = new TH1F("muMinusChi2",     "muMinusChi2",      50, 0., 150.);
  TH1F *h_muUpperMinusChi2 = new TH1F("muUpperMinusChi2","muUpperMinusChi2", 50, 0., 150.);
  TH1F *h_muPlusChi2       = new TH1F("muPlusChi2",      "muPlusChi2",       50, 0., 150.);
  TH1F *h_muUpperPlusChi2  = new TH1F("muUpperPlusChi2", "muUpperPlusChi2",  50, 0., 150.);
  TH1F *h_muLowerMinusChi2 = new TH1F("muLowerMinusChi2","muLowerMinusChi2", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2  = new TH1F("muLowerPlusChi2", "muLowerPlusChi2",  50, 0., 150.);
  TH1F *h_muMinusNdof      = new TH1F("muMinusNdof",     "muMinusNdof",      100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdof = new TH1F("muUpperMinusNdof","muUpperMinusNdof", 100, -0.5, 99.5);
  TH1F *h_muPlusNdof       = new TH1F("muPlusNdof",      "muPlusNdof",       100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdof  = new TH1F("muUpperPlusNdof", "muUpperPlusNdof",  100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdof = new TH1F("muLowerMinusNdof","muLowerMinusNdof", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdof  = new TH1F("muLowerPlusNdof", "muLowerPlusNdof",  100, -0.5, 99.5);
  TH1F *h_muMinusCharge      = new TH1F("muMinusCharge",     "muMinusCharge",      3, -1.5, 1.5);
  TH1F *h_muUpperMinusCharge = new TH1F("muUpperMinusCharge","muUpperMinusCharge", 3, -1.5, 1.5);
  TH1F *h_muPlusCharge       = new TH1F("muPlusCharge",      "muPlusCharge",       3, -1.5, 1.5);
  TH1F *h_muUpperPlusCharge  = new TH1F("muUpperPlusCharge", "muUpperPlusCharge",  3, -1.5, 1.5);
  TH1F *h_muLowerMinusCharge = new TH1F("muLowerMinusCharge","muLowerMinusCharge", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusCharge  = new TH1F("muLowerPlusCharge", "muLowerPlusCharge",  3, -1.5, 1.5);
  TH1F *h_muMinusCurve      = new TH1F("muMinusCurve",     "muMinusCurve",      7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_muUpperMinusCurve = new TH1F("muUpperMinusCurve","muUpperMinusCurve", 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_muPlusCurve       = new TH1F("muPlusCurve",      "muPlusCurve",       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_muUpperPlusCurve  = new TH1F("muUpperPlusCurve", "muUpperPlusCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_muLowerMinusCurve = new TH1F("muLowerMinusCurve","muLowerMinusCurve", 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_muLowerPlusCurve  = new TH1F("muLowerPlusCurve", "muLowerPlusCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_muMinusDxy      = new TH1F("muMinusDxy",     "muMinusDxy",      100, -100., 100.);
  TH1F *h_muUpperMinusDxy = new TH1F("muUpperMinusDxy","muUpperMinusDxy", 100, -100., 100.);
  TH1F *h_muPlusDxy       = new TH1F("muPlusDxy",      "muPlusDxy",       100, -100., 100.);
  TH1F *h_muUpperPlusDxy  = new TH1F("muUpperPlusDxy", "muUpperPlusDxy",  100, -100., 100.);
  TH1F *h_muLowerMinusDxy = new TH1F("muLowerMinusDxy","muLowerMinusDxy", 100, -100., 100.);
  TH1F *h_muLowerPlusDxy  = new TH1F("muLowerPlusDxy", "muLowerPlusDxy",  100, -100., 100.);
  TH1F *h_muMinusDz       = new TH1F("muMinusDz",     "muMinusDz",        100, -250., 250.);
  TH1F *h_muUpperMinusDz  = new TH1F("muUpperMinusDz","muUpperMinusDz",   100, -250., 250.);
  TH1F *h_muPlusDz        = new TH1F("muPlusDz",      "muPlusDz",         100, -250., 250.);
  TH1F *h_muUpperPlusDz   = new TH1F("muUpperPlusDz", "muUpperPlusDz",    100, -250., 250.);
  TH1F *h_muLowerMinusDz  = new TH1F("muLowerMinusDz","muLowerMinusDz",   100, -250., 250.);
  TH1F *h_muLowerPlusDz   = new TH1F("muLowerPlusDz", "muLowerPlusDz",    100, -250., 250.);
  TH1F *h_muMinusDxyError      = new TH1F("muMinusDxyError",     "muMinusDxyError",      50, 0., 150.);
  TH1F *h_muUpperMinusDxyError = new TH1F("muUpperMinusDxyError","muUpperMinusDxyError", 50, 0., 150.);
  TH1F *h_muPlusDxyError       = new TH1F("muPlusDxyError",      "muPlusDxyError",       50, 0., 150.);
  TH1F *h_muUpperPlusDxyError  = new TH1F("muUpperPlusDxyError", "muUpperPlusDxyError",  50, 0., 150.);
  TH1F *h_muLowerMinusDxyError = new TH1F("muLowerMinusDxyError","muLowerMinusDxyError", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyError  = new TH1F("muLowerPlusDxyError", "muLowerPlusDxyError",  50, 0., 150.);
  TH1F *h_muMinusDzError      = new TH1F("muMinusDzError",     "muMinusDzError",      50, 0., 150.);
  TH1F *h_muUpperMinusDzError = new TH1F("muUpperMinusDzError","muUpperMinusDzError", 50, 0., 150.);
  TH1F *h_muPlusDzError       = new TH1F("muPlusDzError",      "muPlusDzError",       50, 0., 150.);
  TH1F *h_muUpperPlusDzError  = new TH1F("muUpperPlusDzError", "muUpperPlusDzError",  50, 0., 150.);
  TH1F *h_muLowerMinusDzError = new TH1F("muLowerMinusDzError","muLowerMinusDzError", 50, 0., 150.);
  TH1F *h_muLowerPlusDzError  = new TH1F("muLowerPlusDzError", "muLowerPlusDzError",  50, 0., 150.);

  TH1F *h_muMinusTrackPt      = new TH1F("muMinusTrackPt",     "muMinusTrackPt",      300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPt = new TH1F("muUpperMinusTrackPt","muUpperMinusTrackPt", 300, 0., 3000.);
  TH1F *h_muPlusTrackPt       = new TH1F("muPlusTrackPt",      "muPlusTrackPt",       300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPt  = new TH1F("muUpperPlusTrackPt", "muUpperPlusTrackPt",  300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPt = new TH1F("muLowerMinusTrackPt","muLowerMinusTrackPt", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPt  = new TH1F("muLowerPlusTrackPt", "muLowerPlusTrackPt",  300, 0., 3000.);
  TH1F *h_muMinusTrackEta      = new TH1F("muMinusTrackEta",   "muMinusTrackEta",        40, -2., 2.);
  TH1F *h_muUpperMinusTrackEta = new TH1F("muUpperMinusTrackEta","muUpperMinusTrackEta", 40, -2., 2.);
  TH1F *h_muPlusTrackEta       = new TH1F("muPlusTrackEta",      "muPlusTrackEta",       40, -2., 2.);
  TH1F *h_muUpperPlusTrackEta  = new TH1F("muUpperPlusTrackEta", "muUpperPlusTrackEta",  40, -2., 2.);
  TH1F *h_muLowerMinusTrackEta = new TH1F("muLowerMinusTrackEta","muLowerMinusTrackEta", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEta  = new TH1F("muLowerPlusTrackEta", "muLowerPlusTrackEta",  40, -2., 2.);
  TH1F *h_muMinusTrackPhi      = new TH1F("muMinusTrackPhi",     "muMinusTrackPhi",      40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhi = new TH1F("muUpperMinusTrackPhi","muUpperMinusTrackPhi", 40, -4., 4.);
  TH1F *h_muPlusTrackPhi       = new TH1F("muPlusTrackPhi",      "muPlusTrackPhi",       40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhi  = new TH1F("muUpperPlusTrackPhi", "muUpperPlusTrackPhi",  40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhi = new TH1F("muLowerMinusTrackPhi","muLowerMinusTrackPhi", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhi  = new TH1F("muLowerPlusTrackPhi", "muLowerPlusTrackPhi",  40, -4., 4.);
  TH1F *h_muMinusPtError      = new TH1F("muMinusPtError",     "muMinusPtError",      100, 0., 600.);
  TH1F *h_muUpperMinusPtError = new TH1F("muUpperMinusPtError","muUpperMinusPtError", 100, 0., 600.);
  TH1F *h_muPlusPtError       = new TH1F("muPlusPtError",      "muPlusPtError",       100, 0., 600.);
  TH1F *h_muUpperPlusPtError  = new TH1F("muUpperPlusPtError", "muUpperPlusPtError",  100, 0., 600.);
  TH1F *h_muLowerMinusPtError = new TH1F("muLowerMinusPtError","muLowerMinusPtError", 100, 0., 600.);
  TH1F *h_muLowerPlusPtError  = new TH1F("muLowerPlusPtError", "muLowerPlusPtError",  100, 0., 600.);
  TH1F *h_muMinusPtRelErr      = new TH1F("muMinusPtRelErr",     "muMinusPtRelErr",      100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErr = new TH1F("muUpperMinusPtRelErr","muUpperMinusPtRelErr", 100, 0., 1.);
  TH1F *h_muPlusPtRelErr       = new TH1F("muPlusPtRelErr",      "muPlusPtRelErr",       100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErr  = new TH1F("muUpperPlusPtRelErr", "muUpperPlusPtRelErr",  100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErr = new TH1F("muLowerMinusPtRelErr","muLowerMinusPtRelErr", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErr  = new TH1F("muLowerPlusPtRelErr","muLowerPlusPtRelErr",   100, 0., 1.);

  TH1F *h_muMinusPixelHits      = new TH1F("muMinusPixelHits",     "muMinusPixelHits",      10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHits = new TH1F("muUpperMinusPixelHits","muUpperMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHits       = new TH1F("muPlusPixelHits",      "muPlusPixelHits",       10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHits  = new TH1F("muUpperPlusPixelHits", "muUpperPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHits = new TH1F("muLowerMinusPixelHits","muLowerMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHits  = new TH1F("muLowerPlusPixelHits", "muLowerPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_muMinusTrackerHits      = new TH1F("muMinusTrackerHits",     "muMinusTrackerHits",      35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHits = new TH1F("muUpperMinusTrackerHits","muUpperMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHits       = new TH1F("muPlusTrackerHits",      "muPlusTrackerHits",       35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHits  = new TH1F("muUpperPlusTrackerHits", "muUpperPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHits = new TH1F("muLowerMinusTrackerHits","muLowerMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHits  = new TH1F("muLowerPlusTrackerHits", "muLowerPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_muMinusMuonStationHits      = new TH1F("muMinusMuonStationHits",     "muMinusMuonStationHits",      10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHits = new TH1F("muUpperMinusMuonStationHits","muUpperMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHits       = new TH1F("muPlusMuonStationHits",      "muPlusMuonStationHits",       10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHits  = new TH1F("muUpperPlusMuonStationHits", "muUpperPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHits = new TH1F("muLowerMinusMuonStationHits","muLowerMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHits  = new TH1F("muLowerPlusMuonStationHits", "muLowerPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_muMinusValidHits      = new TH1F("muMinusValidHits",     "muMinusValidHits",      100,-0.5, 99.5);
  TH1F *h_muUpperMinusValidHits = new TH1F("muUpperMinusValidHits","muUpperMinusValidHits", 100,-0.5, 99.5);
  TH1F *h_muPlusValidHits       = new TH1F("muPlusValidHits",      "muPlusValidHits",       100,-0.5, 99.5);
  TH1F *h_muUpperPlusValidHits  = new TH1F("muUpperPlusValidHits", "muUpperPlusValidHits",  100,-0.5, 99.5);
  TH1F *h_muLowerMinusValidHits = new TH1F("muLowerMinusValidHits","muLowerMinusValidHits", 100,-0.5, 99.5);
  TH1F *h_muLowerPlusValidHits  = new TH1F("muLowerPlusValidHits", "muLowerPlusValidHits",  100,-0.5, 99.5);
  TH1F *h_muMinusValidMuonHits      = new TH1F("muMinusValidMuonHits",     "muMinusValidMuonHits",      75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHits = new TH1F("muUpperMinusValidMuonHits","muUpperMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHits       = new TH1F("muPlusValidMuonHits",      "muPlusValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHits  = new TH1F("muUpperPlusValidMuonHits", "muUpperPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHits = new TH1F("muLowerMinusValidMuonHits","muLowerMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHits  = new TH1F("muLowerPlusValidMuonHits", "muLowerPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_muMinusMatchedMuonStations      = new TH1F("muMinusMatchedMuonStations",     "muMinusMatchedMuonStations",      10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStations = new TH1F("muUpperMinusMatchedMuonStations","muUpperMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStations       = new TH1F("muPlusMatchedMuonStations",      "muPlusMatchedMuonStations",       10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStations  = new TH1F("muUpperPlusMatchedMuonStations", "muUpperPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStations = new TH1F("muLowerMinusMatchedMuonStations","muLowerMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStations  = new TH1F("muLowerPlusMatchedMuonStations", "muLowerPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_muMinusTrackerLayersWithMeasurement      = new TH1F("muMinusTrackerLayersWithMeasurement",     "muMinusTrackerLayersWithMeasurement",      20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurement = new TH1F("muUpperMinusTrackerLayersWithMeasurement","muUpperMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurement       = new TH1F("muPlusTrackerLayersWithMeasurement",      "muPlusTrackerLayersWithMeasurement",       20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurement  = new TH1F("muUpperPlusTrackerLayersWithMeasurement", "muUpperPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurement = new TH1F("muLowerMinusTrackerLayersWithMeasurement","muLowerMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurement  = new TH1F("muLowerPlusTrackerLayersWithMeasurement", "muLowerPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);

  TH1F *h_muMinusCurvePlusBias[      nBiasBins];
  TH1F *h_muMinusCurveMinusBias[     nBiasBins];
  TH1F *h_muUpperMinusCurvePlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurvePlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveMinusBias[nBiasBins];
  TH1F *h_muPlusCurvePlusBias[       nBiasBins];
  TH1F *h_muPlusCurveMinusBias[      nBiasBins];
  TH1F *h_muUpperPlusCurvePlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurvePlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveMinusBias[ nBiasBins];

  for (int i =0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_muMinusCurvePlusBias[i]       = new TH1F(TString("muMinusCurvePlusBias"       + name.str()),
					       TString("muMinusCurvePlusBias"       + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_muMinusCurveMinusBias[i]      = new TH1F(TString("muMinusCurveMinusBias"      + name.str()),
					       TString("muMinusCurveMinusBias"      + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_muUpperMinusCurvePlusBias[i]  = new TH1F(TString("muUpperMinusCurvePlusBias"  + name.str()),
					       TString("muUpperMinusCurvePlusBias"  + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_muUpperMinusCurveMinusBias[i] = new TH1F(TString("muUpperMinusCurveMinusBias" + name.str()),
					       TString("muUpperMinusCurveMinusBias" + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_muLowerMinusCurvePlusBias[i]  = new TH1F(TString("muLowerMinusCurvePlusBias"  + name.str()),
					       TString("muLowerMinusCurvePlusBias"  + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_muLowerMinusCurveMinusBias[i] = new TH1F(TString("muLowerMinusCurveMinusBias" + name.str()),
					       TString("muLowerMinusCurveMinusBias" + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_muPlusCurvePlusBias[i]        = new TH1F(TString("muPlusCurvePlusBias"        + name.str()),
					       TString("muPlusCurvePlusBias"        + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_muPlusCurveMinusBias[i]       = new TH1F(TString("muPlusCurveMinusBias"       + name.str()),
					       TString("muPlusCurveMinusBias"       + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_muUpperPlusCurvePlusBias[i]   = new TH1F(TString("muUpperPlusCurvePlusBias"   + name.str()),
					       TString("muUpperPlusCurvePlusBias"   + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_muUpperPlusCurveMinusBias[i]  = new TH1F(TString("muUpperPlusCurveMinusBias"  + name.str()),
					       TString("muUpperPlusCurveMinusBias"  + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_muLowerPlusCurvePlusBias[i]   = new TH1F(TString("muLowerPlusCurvePlusBias"   + name.str()),
					       TString("muLowerPlusCurvePlusBias"   + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_muLowerPlusCurveMinusBias[i]  = new TH1F(TString("muLowerPlusCurveMinusBias"  + name.str()),
					       TString("muLowerPlusCurveMinusBias"  + name.str()),
					       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
  }

  // histograms for loose cuts (not applying the Dxy/Dz cuts)
  TH1F *h_looseMuMinusPt      = new TH1F("looseMuMinusPt",     "looseMuMinusPt",      300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPt = new TH1F("looseMuUpperMinusPt","looseMuUpperMinusPt", 300, 0., 3000.);
  TH1F *h_looseMuPlusPt       = new TH1F("looseMuPlusPt",      "looseMuPlusPt",       300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPt  = new TH1F("looseMuUpperPlusPt", "looseMuUpperPlusPt",  300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPt = new TH1F("looseMuLowerMinusPt","looseMuLowerMinusPt", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPt  = new TH1F("looseMuLowerPlusPt", "looseMuLowerPlusPt",  300, 0., 3000.);
  TH1F *h_looseMuMinusEta      = new TH1F("looseMuMinusEta",     "looseMuMinusEta",      40, -2., 2.);
  TH1F *h_looseMuUpperMinusEta = new TH1F("looseMuUpperMinusEta","looseMuUpperMinusEta", 40, -2., 2.);
  TH1F *h_looseMuPlusEta       = new TH1F("looseMuPlusEta",      "looseMuPlusEta",       40, -2., 2.);
  TH1F *h_looseMuUpperPlusEta  = new TH1F("looseMuUpperPlusEta", "looseMuUpperPlusEta",  40, -2., 2.);
  TH1F *h_looseMuLowerMinusEta = new TH1F("looseMuLowerMinusEta","looseMuLowerMinusEta", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEta  = new TH1F("looseMuLowerPlusEta", "looseMuLowerPlusEta",  40, -2., 2.);
  TH1F *h_looseMuMinusPhi      = new TH1F("looseMuMinusPhi",     "looseMuMinusPhi",      40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhi = new TH1F("looseMuUpperMinusPhi","looseMuUpperMinusPhi", 40, -4., 4.);
  TH1F *h_looseMuPlusPhi       = new TH1F("looseMuPlusPhi",      "looseMuPlusPhi",       40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhi  = new TH1F("looseMuUpperPlusPhi", "looseMuUpperPlusPhi",  40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhi = new TH1F("looseMuLowerMinusPhi","looseMuLowerMinusPhi", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhi  = new TH1F("looseMuLowerPlusPhi", "looseMuLowerPlusPhi",  40, -4., 4.);

  TH1F *h_looseMuMinusChi2      = new TH1F("looseMuMinusChi2",     "looseMuMinusChi2",      50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2 = new TH1F("looseMuUpperMinusChi2","looseMuUpperMinusChi2", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2       = new TH1F("looseMuPlusChi2",      "looseMuPlusChi2",       50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2  = new TH1F("looseMuUpperPlusChi2", "looseMuUpperPlusChi2",  50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2 = new TH1F("looseMuLowerMinusChi2","looseMuLowerMinusChi2", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2  = new TH1F("looseMuLowerPlusChi2", "looseMuLowerPlusChi2",  50, 0., 150.);
  TH1F *h_looseMuMinusNdof      = new TH1F("looseMuMinusNdof",     "looseMuMinusNdof",      100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdof = new TH1F("looseMuUpperMinusNdof","looseMuUpperMinusNdof", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdof       = new TH1F("looseMuPlusNdof",      "looseMuPlusNdof",       100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdof  = new TH1F("looseMuUpperPlusNdof", "looseMuUpperPlusNdof",  100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdof = new TH1F("looseMuLowerMinusNdof","looseMuLowerMinusNdof", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdof  = new TH1F("looseMuLowerPlusNdof", "looseMuLowerPlusNdof",  100, -0.5, 99.5);
  TH1F *h_looseMuMinusCharge      = new TH1F("looseMuMinusCharge",     "looseMuMinusCharge",      3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusCharge = new TH1F("looseMuUpperMinusCharge","looseMuUpperMinusCharge", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusCharge       = new TH1F("looseMuPlusCharge",      "looseMuPlusCharge",       3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusCharge  = new TH1F("looseMuUpperPlusCharge", "looseMuUpperPlusCharge",  3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusCharge = new TH1F("looseMuLowerMinusCharge","looseMuLowerMinusCharge", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusCharge  = new TH1F("looseMuLowerPlusCharge", "looseMuLowerPlusCharge",  3, -1.5, 1.5);
  TH1F *h_looseMuMinusCurve      = new TH1F("looseMuMinusCurve",     "looseMuMinusCurve",      7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_looseMuUpperMinusCurve = new TH1F("looseMuUpperMinusCurve","looseMuUpperMinusCurve", 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_looseMuPlusCurve       = new TH1F("looseMuPlusCurve",      "looseMuPlusCurve",       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_looseMuUpperPlusCurve  = new TH1F("looseMuUpperPlusCurve", "looseMuUpperPlusCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_looseMuLowerMinusCurve = new TH1F("looseMuLowerMinusCurve","looseMuLowerMinusCurve", 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_looseMuLowerPlusCurve  = new TH1F("looseMuLowerPlusCurve", "looseMuLowerPlusCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
  TH1F *h_looseMuMinusDxy      = new TH1F("looseMuMinusDxy",     "looseMuMinusDxy",      100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxy = new TH1F("looseMuUpperMinusDxy","looseMuUpperMinusDxy", 100, -100., 100.);
  TH1F *h_looseMuPlusDxy       = new TH1F("looseMuPlusDxy",      "looseMuPlusDxy",       100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxy  = new TH1F("looseMuUpperPlusDxy", "looseMuUpperPlusDxy",  100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxy = new TH1F("looseMuLowerMinusDxy","looseMuLowerMinusDxy", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxy  = new TH1F("looseMuLowerPlusDxy", "looseMuLowerPlusDxy",  100, -100., 100.);
  TH1F *h_looseMuMinusDz       = new TH1F("looseMuMinusDz",     "looseMuMinusDz",        100, -250., 250.);
  TH1F *h_looseMuUpperMinusDz  = new TH1F("looseMuUpperMinusDz","looseMuUpperMinusDz",   100, -250., 250.);
  TH1F *h_looseMuPlusDz        = new TH1F("looseMuPlusDz",      "looseMuPlusDz",         100, -250., 250.);
  TH1F *h_looseMuUpperPlusDz   = new TH1F("looseMuUpperPlusDz", "looseMuUpperPlusDz",    100, -250., 250.);
  TH1F *h_looseMuLowerMinusDz  = new TH1F("looseMuLowerMinusDz","looseMuLowerMinusDz",   100, -250., 250.);
  TH1F *h_looseMuLowerPlusDz   = new TH1F("looseMuLowerPlusDz", "looseMuLowerPlusDz",    100, -250., 250.);
  TH1F *h_looseMuMinusDxyError      = new TH1F("looseMuMinusDxyError",     "looseMuMinusDxyError",      50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyError = new TH1F("looseMuUpperMinusDxyError","looseMuUpperMinusDxyError", 50, 0., 150.);
  TH1F *h_looseMuPlusDxyError       = new TH1F("looseMuPlusDxyError",      "looseMuPlusDxyError",       50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyError  = new TH1F("looseMuUpperPlusDxyError", "looseMuUpperPlusDxyError",  50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyError = new TH1F("looseMuLowerMinusDxyError","looseMuLowerMinusDxyError", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyError  = new TH1F("looseMuLowerPlusDxyError", "looseMuLowerPlusDxyError",  50, 0., 150.);
  TH1F *h_looseMuMinusDzError      = new TH1F("looseMuMinusDzError",     "looseMuMinusDzError",      50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzError = new TH1F("looseMuUpperMinusDzError","looseMuUpperMinusDzError", 50, 0., 150.);
  TH1F *h_looseMuPlusDzError       = new TH1F("looseMuPlusDzError",      "looseMuPlusDzError",       50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzError  = new TH1F("looseMuUpperPlusDzError", "looseMuUpperPlusDzError",  50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzError = new TH1F("looseMuLowerMinusDzError","looseMuLowerMinusDzError", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzError  = new TH1F("looseMuLowerPlusDzError", "looseMuLowerPlusDzError",  50, 0., 150.);

  TH1F *h_looseMuMinusTrackPt      = new TH1F("looseMuMinusTrackPt",     "looseMuMinusTrackPt",      300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPt = new TH1F("looseMuUpperMinusTrackPt","looseMuUpperMinusTrackPt", 300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPt       = new TH1F("looseMuPlusTrackPt",      "looseMuPlusTrackPt",       300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPt  = new TH1F("looseMuUpperPlusTrackPt", "looseMuUpperPlusTrackPt",  300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPt = new TH1F("looseMuLowerMinusTrackPt","looseMuLowerMinusTrackPt", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPt  = new TH1F("looseMuLowerPlusTrackPt", "looseMuLowerPlusTrackPt",  300, 0., 3000.);
  TH1F *h_looseMuMinusTrackEta      = new TH1F("looseMuMinusTrackEta",   "looseMuMinusTrackEta",        40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEta = new TH1F("looseMuUpperMinusTrackEta","looseMuUpperMinusTrackEta", 40, -2., 2.);
  TH1F *h_looseMuPlusTrackEta       = new TH1F("looseMuPlusTrackEta",      "looseMuPlusTrackEta",       40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEta  = new TH1F("looseMuUpperPlusTrackEta", "looseMuUpperPlusTrackEta",  40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEta = new TH1F("looseMuLowerMinusTrackEta","looseMuLowerMinusTrackEta", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEta  = new TH1F("looseMuLowerPlusTrackEta", "looseMuLowerPlusTrackEta",  40, -2., 2.);
  TH1F *h_looseMuMinusTrackPhi      = new TH1F("looseMuMinusTrackPhi",     "looseMuMinusTrackPhi",      40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhi = new TH1F("looseMuUpperMinusTrackPhi","looseMuUpperMinusTrackPhi", 40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhi       = new TH1F("looseMuPlusTrackPhi",      "looseMuPlusTrackPhi",       40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhi  = new TH1F("looseMuUpperPlusTrackPhi", "looseMuUpperPlusTrackPhi",  40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhi = new TH1F("looseMuLowerMinusTrackPhi","looseMuLowerMinusTrackPhi", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusTrackPhi  = new TH1F("looseMuLowerPlusTrackPhi", "looseMuLowerPlusTrackPhi",  40, -4., 4.);
  TH1F *h_looseMuMinusPtError      = new TH1F("looseMuMinusPtError",     "looseMuMinusPtError",      100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtError = new TH1F("looseMuUpperMinusPtError","looseMuUpperMinusPtError", 100, 0., 600.);
  TH1F *h_looseMuPlusPtError       = new TH1F("looseMuPlusPtError",      "looseMuPlusPtError",       100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtError  = new TH1F("looseMuUpperPlusPtError", "looseMuUpperPlusPtError",  100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtError = new TH1F("looseMuLowerMinusPtError","looseMuLowerMinusPtError", 100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtError  = new TH1F("looseMuLowerPlusPtError", "looseMuLowerPlusPtError",  100, 0., 600.);
  TH1F *h_looseMuMinusPtRelErr      = new TH1F("looseMuMinusPtRelErr",     "looseMuMinusPtRelErr",      100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErr = new TH1F("looseMuUpperMinusPtRelErr","looseMuUpperMinusPtRelErr", 100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErr       = new TH1F("looseMuPlusPtRelErr",      "looseMuPlusPtRelErr",       100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErr  = new TH1F("looseMuUpperPlusPtRelErr", "looseMuUpperPlusPtRelErr",  100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErr = new TH1F("looseMuLowerMinusPtRelErr","looseMuLowerMinusPtRelErr", 100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErr  = new TH1F("looseMuLowerPlusPtRelErr","looseMuLowerPlusPtRelErr",   100, 0., 1.);

  TH1F *h_looseMuMinusPixelHits      = new TH1F("looseMuMinusPixelHits",     "looseMuMinusPixelHits",      10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHits = new TH1F("looseMuUpperMinusPixelHits","looseMuUpperMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHits       = new TH1F("looseMuPlusPixelHits",      "looseMuPlusPixelHits",       10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHits  = new TH1F("looseMuUpperPlusPixelHits", "looseMuUpperPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHits = new TH1F("looseMuLowerMinusPixelHits","looseMuLowerMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHits  = new TH1F("looseMuLowerPlusPixelHits", "looseMuLowerPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_looseMuMinusTrackerHits      = new TH1F("looseMuMinusTrackerHits",     "looseMuMinusTrackerHits",      35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHits = new TH1F("looseMuUpperMinusTrackerHits","looseMuUpperMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHits       = new TH1F("looseMuPlusTrackerHits",      "looseMuPlusTrackerHits",       35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHits  = new TH1F("looseMuUpperPlusTrackerHits", "looseMuUpperPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHits = new TH1F("looseMuLowerMinusTrackerHits","looseMuLowerMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_looseMuLowerPlusTrackerHits  = new TH1F("looseMuLowerPlusTrackerHits", "looseMuLowerPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_looseMuMinusMuonStationHits      = new TH1F("looseMuMinusMuonStationHits",     "looseMuMinusMuonStationHits",      10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHits = new TH1F("looseMuUpperMinusMuonStationHits","looseMuUpperMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHits       = new TH1F("looseMuPlusMuonStationHits",      "looseMuPlusMuonStationHits",       10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHits  = new TH1F("looseMuUpperPlusMuonStationHits", "looseMuUpperPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHits = new TH1F("looseMuLowerMinusMuonStationHits","looseMuLowerMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHits  = new TH1F("looseMuLowerPlusMuonStationHits", "looseMuLowerPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_looseMuMinusValidHits      = new TH1F("looseMuMinusValidHits",     "looseMuMinusValidHits",      100,-0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHits = new TH1F("looseMuUpperMinusValidHits","looseMuUpperMinusValidHits", 100,-0.5, 99.5);
  TH1F *h_looseMuPlusValidHits       = new TH1F("looseMuPlusValidHits",      "looseMuPlusValidHits",       100,-0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHits  = new TH1F("looseMuUpperPlusValidHits", "looseMuUpperPlusValidHits",  100,-0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHits = new TH1F("looseMuLowerMinusValidHits","looseMuLowerMinusValidHits", 100,-0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHits  = new TH1F("looseMuLowerPlusValidHits", "looseMuLowerPlusValidHits",  100,-0.5, 99.5);
  TH1F *h_looseMuMinusValidMuonHits      = new TH1F("looseMuMinusValidMuonHits",     "looseMuMinusValidMuonHits",      75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHits = new TH1F("looseMuUpperMinusValidMuonHits","looseMuUpperMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHits       = new TH1F("looseMuPlusValidMuonHits",      "looseMuPlusValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHits  = new TH1F("looseMuUpperPlusValidMuonHits", "looseMuUpperPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHits = new TH1F("looseMuLowerMinusValidMuonHits","looseMuLowerMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHits  = new TH1F("looseMuLowerPlusValidMuonHits", "looseMuLowerPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_looseMuMinusMatchedMuonStations      = new TH1F("looseMuMinusMatchedMuonStations",     "looseMuMinusMatchedMuonStations",      10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStations = new TH1F("looseMuUpperMinusMatchedMuonStations","looseMuUpperMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStations       = new TH1F("looseMuPlusMatchedMuonStations",      "looseMuPlusMatchedMuonStations",       10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStations  = new TH1F("looseMuUpperPlusMatchedMuonStations", "looseMuUpperPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStations = new TH1F("looseMuLowerMinusMatchedMuonStations","looseMuLowerMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStations  = new TH1F("looseMuLowerPlusMatchedMuonStations", "looseMuLowerPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurement      = new TH1F("looseMuMinusTrackerLayersWithMeasurement",     "looseMuMinusTrackerLayersWithMeasurement",      20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurement = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurement","looseMuUpperMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurement       = new TH1F("looseMuPlusTrackerLayersWithMeasurement",      "looseMuPlusTrackerLayersWithMeasurement",       20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurement  = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurement", "looseMuUpperPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurement = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurement","looseMuLowerMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurement  = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurement", "looseMuLowerPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);

  TH1F *h_looseMuMinusCurvePlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveMinusBias[     nBiasBins];
  TH1F *h_looseMuUpperMinusCurvePlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurvePlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveMinusBias[nBiasBins];
  TH1F *h_looseMuPlusCurvePlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveMinusBias[      nBiasBins];
  TH1F *h_looseMuUpperPlusCurvePlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurvePlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveMinusBias[ nBiasBins];

  for (int i =0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    h_looseMuMinusCurvePlusBias[i]       = new TH1F(TString("looseMuMinusCurvePlusBias"       + name.str()),
						    TString("looseMuMinusCurvePlusBias"       + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_looseMuMinusCurveMinusBias[i]      = new TH1F(TString("looseMuMinusCurveMinusBias"      + name.str()),
						    TString("looseMuMinusCurveMinusBias"      + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_looseMuUpperMinusCurvePlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurvePlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurvePlusBias"  + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_looseMuUpperMinusCurveMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveMinusBias" + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_looseMuLowerMinusCurvePlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurvePlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurvePlusBias"  + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_looseMuLowerMinusCurveMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveMinusBias" + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_looseMuPlusCurvePlusBias[i]        = new TH1F(TString("looseMuPlusCurvePlusBias"        + name.str()),
						    TString("looseMuPlusCurvePlusBias"        + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_looseMuPlusCurveMinusBias[i]       = new TH1F(TString("looseMuPlusCurveMinusBias"       + name.str()),
						    TString("looseMuPlusCurveMinusBias"       + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_looseMuUpperPlusCurvePlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurvePlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurvePlusBias"   + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_looseMuUpperPlusCurveMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveMinusBias"  + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
    h_looseMuLowerPlusCurvePlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurvePlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurvePlusBias"   + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
    h_looseMuLowerPlusCurveMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveMinusBias"  + name.str()),
						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
  }

// remove tight info //  // histograms for tight cuts
// remove tight info //  TH1F *h_tightMuMinusPt      = new TH1F("tightMuMinusPt",     "tightMuMinusPt",      300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuUpperMinusPt = new TH1F("tightMuUpperMinusPt","tightMuUpperMinusPt", 300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuPlusPt       = new TH1F("tightMuPlusPt",      "tightMuPlusPt",       300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuUpperPlusPt  = new TH1F("tightMuUpperPlusPt", "tightMuUpperPlusPt",  300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuLowerMinusPt = new TH1F("tightMuLowerMinusPt","tightMuLowerMinusPt", 300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuLowerPlusPt  = new TH1F("tightMuLowerPlusPt", "tightMuLowerPlusPt",  300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuMinusEta      = new TH1F("tightMuMinusEta",     "tightMuMinusEta",      40, -2., 2.);
// remove tight info //  TH1F *h_tightMuUpperMinusEta = new TH1F("tightMuUpperMinusEta","tightMuUpperMinusEta", 40, -2., 2.);
// remove tight info //  TH1F *h_tightMuPlusEta       = new TH1F("tightMuPlusEta",      "tightMuPlusEta",       40, -2., 2.);
// remove tight info //  TH1F *h_tightMuUpperPlusEta  = new TH1F("tightMuUpperPlusEta", "tightMuUpperPlusEta",  40, -2., 2.);
// remove tight info //  TH1F *h_tightMuLowerMinusEta = new TH1F("tightMuLowerMinusEta","tightMuLowerMinusEta", 40, -2., 2.);
// remove tight info //  TH1F *h_tightMuLowerPlusEta  = new TH1F("tightMuLowerPlusEta", "tightMuLowerPlusEta",  40, -2., 2.);
// remove tight info //  TH1F *h_tightMuMinusPhi      = new TH1F("tightMuMinusPhi",     "tightMuMinusPhi",      40, -4., 4.);
// remove tight info //  TH1F *h_tightMuUpperMinusPhi = new TH1F("tightMuUpperMinusPhi","tightMuUpperMinusPhi", 40, -4., 4.);
// remove tight info //  TH1F *h_tightMuPlusPhi       = new TH1F("tightMuPlusPhi",      "tightMuPlusPhi",       40, -4., 4.);
// remove tight info //  TH1F *h_tightMuUpperPlusPhi  = new TH1F("tightMuUpperPlusPhi", "tightMuUpperPlusPhi",  40, -4., 4.);
// remove tight info //  TH1F *h_tightMuLowerMinusPhi = new TH1F("tightMuLowerMinusPhi","tightMuLowerMinusPhi", 40, -4., 4.);
// remove tight info //  TH1F *h_tightMuLowerPlusPhi  = new TH1F("tightMuLowerPlusPhi", "tightMuLowerPlusPhi",  40, -4., 4.);
// remove tight info //
// remove tight info //  TH1F *h_tightMuMinusChi2      = new TH1F("tightMuMinusChi2",     "tightMuMinusChi2",      50, 0., 150.);
// remove tight info //  TH1F *h_tightMuUpperMinusChi2 = new TH1F("tightMuUpperMinusChi2","tightMuUpperMinusChi2", 50, 0., 150.);
// remove tight info //  TH1F *h_tightMuPlusChi2       = new TH1F("tightMuPlusChi2",      "tightMuPlusChi2",       50, 0., 150.);
// remove tight info //  TH1F *h_tightMuUpperPlusChi2  = new TH1F("tightMuUpperPlusChi2", "tightMuUpperPlusChi2",  50, 0., 150.);
// remove tight info //  TH1F *h_tightMuLowerMinusChi2 = new TH1F("tightMuLowerMinusChi2","tightMuLowerMinusChi2", 50, 0., 150.);
// remove tight info //  TH1F *h_tightMuLowerPlusChi2  = new TH1F("tightMuLowerPlusChi2", "tightMuLowerPlusChi2",  50, 0., 150.);
// remove tight info //  TH1F *h_tightMuMinusNdof      = new TH1F("tightMuMinusNdof",     "tightMuMinusNdof",      100, -0.5, 99.5);
// remove tight info //  TH1F *h_tightMuUpperMinusNdof = new TH1F("tightMuUpperMinusNdof","tightMuUpperMinusNdof", 100, -0.5, 99.5);
// remove tight info //  TH1F *h_tightMuPlusNdof       = new TH1F("tightMuPlusNdof",      "tightMuPlusNdof",       100, -0.5, 99.5);
// remove tight info //  TH1F *h_tightMuUpperPlusNdof  = new TH1F("tightMuUpperPlusNdof", "tightMuUpperPlusNdof",  100, -0.5, 99.5);
// remove tight info //  TH1F *h_tightMuLowerMinusNdof = new TH1F("tightMuLowerMinusNdof","tightMuLowerMinusNdof", 100, -0.5, 99.5);
// remove tight info //  TH1F *h_tightMuLowerPlusNdof  = new TH1F("tightMuLowerPlusNdof", "tightMuLowerPlusNdof",  100, -0.5, 99.5);
// remove tight info //  TH1F *h_tightMuMinusCharge      = new TH1F("tightMuMinusCharge",     "tightMuMinusCharge",      3, -1.5, 1.5);
// remove tight info //  TH1F *h_tightMuUpperMinusCharge = new TH1F("tightMuUpperMinusCharge","tightMuUpperMinusCharge", 3, -1.5, 1.5);
// remove tight info //  TH1F *h_tightMuPlusCharge       = new TH1F("tightMuPlusCharge",      "tightMuPlusCharge",       3, -1.5, 1.5);
// remove tight info //  TH1F *h_tightMuUpperPlusCharge  = new TH1F("tightMuUpperPlusCharge", "tightMuUpperPlusCharge",  3, -1.5, 1.5);
// remove tight info //  TH1F *h_tightMuLowerMinusCharge = new TH1F("tightMuLowerMinusCharge","tightMuLowerMinusCharge", 3, -1.5, 1.5);
// remove tight info //  TH1F *h_tightMuLowerPlusCharge  = new TH1F("tightMuLowerPlusCharge", "tightMuLowerPlusCharge",  3, -1.5, 1.5);
// remove tight info //  TH1F *h_tightMuMinusCurve      = new TH1F("tightMuMinusCurve",     "tightMuMinusCurve",      7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //  TH1F *h_tightMuUpperMinusCurve = new TH1F("tightMuUpperMinusCurve","tightMuUpperMinusCurve", 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //  TH1F *h_tightMuPlusCurve       = new TH1F("tightMuPlusCurve",      "tightMuPlusCurve",       7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //  TH1F *h_tightMuUpperPlusCurve  = new TH1F("tightMuUpperPlusCurve", "tightMuUpperPlusCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //  TH1F *h_tightMuLowerMinusCurve = new TH1F("tightMuLowerMinusCurve","tightMuLowerMinusCurve", 7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //  TH1F *h_tightMuLowerPlusCurve  = new TH1F("tightMuLowerPlusCurve", "tightMuLowerPlusCurve",  7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //  TH1F *h_tightMuMinusDxy      = new TH1F("tightMuMinusDxy",     "tightMuMinusDxy",      100, -100., 100.);
// remove tight info //  TH1F *h_tightMuUpperMinusDxy = new TH1F("tightMuUpperMinusDxy","tightMuUpperMinusDxy", 100, -100., 100.);
// remove tight info //  TH1F *h_tightMuPlusDxy       = new TH1F("tightMuPlusDxy",      "tightMuPlusDxy",       100, -100., 100.);
// remove tight info //  TH1F *h_tightMuUpperPlusDxy  = new TH1F("tightMuUpperPlusDxy", "tightMuUpperPlusDxy",  100, -100., 100.);
// remove tight info //  TH1F *h_tightMuLowerMinusDxy = new TH1F("tightMuLowerMinusDxy","tightMuLowerMinusDxy", 100, -100., 100.);
// remove tight info //  TH1F *h_tightMuLowerPlusDxy  = new TH1F("tightMuLowerPlusDxy", "tightMuLowerPlusDxy",  100, -100., 100.);
// remove tight info //  TH1F *h_tightMuMinusDz       = new TH1F("tightMuMinusDz",     "tightMuMinusDz",        100, -250., 250.);
// remove tight info //  TH1F *h_tightMuUpperMinusDz  = new TH1F("tightMuUpperMinusDz","tightMuUpperMinusDz",   100, -250., 250.);
// remove tight info //  TH1F *h_tightMuPlusDz        = new TH1F("tightMuPlusDz",      "tightMuPlusDz",         100, -250., 250.);
// remove tight info //  TH1F *h_tightMuUpperPlusDz   = new TH1F("tightMuUpperPlusDz", "tightMuUpperPlusDz",    100, -250., 250.);
// remove tight info //  TH1F *h_tightMuLowerMinusDz  = new TH1F("tightMuLowerMinusDz","tightMuLowerMinusDz",   100, -250., 250.);
// remove tight info //  TH1F *h_tightMuLowerPlusDz   = new TH1F("tightMuLowerPlusDz", "tightMuLowerPlusDz",    100, -250., 250.);
// remove tight info //  TH1F *h_tightMuMinusDxyError      = new TH1F("tightMuMinusDxyError",     "tightMuMinusDxyError",      50, 0., 150.);
// remove tight info //  TH1F *h_tightMuUpperMinusDxyError = new TH1F("tightMuUpperMinusDxyError","tightMuUpperMinusDxyError", 50, 0., 150.);
// remove tight info //  TH1F *h_tightMuPlusDxyError       = new TH1F("tightMuPlusDxyError",      "tightMuPlusDxyError",       50, 0., 150.);
// remove tight info //  TH1F *h_tightMuUpperPlusDxyError  = new TH1F("tightMuUpperPlusDxyError", "tightMuUpperPlusDxyError",  50, 0., 150.);
// remove tight info //  TH1F *h_tightMuLowerMinusDxyError = new TH1F("tightMuLowerMinusDxyError","tightMuLowerMinusDxyError", 50, 0., 150.);
// remove tight info //  TH1F *h_tightMuLowerPlusDxyError  = new TH1F("tightMuLowerPlusDxyError", "tightMuLowerPlusDxyError",  50, 0., 150.);
// remove tight info //  TH1F *h_tightMuMinusDzError      = new TH1F("tightMuMinusDzError",     "tightMuMinusDzError",      50, 0., 150.);
// remove tight info //  TH1F *h_tightMuUpperMinusDzError = new TH1F("tightMuUpperMinusDzError","tightMuUpperMinusDzError", 50, 0., 150.);
// remove tight info //  TH1F *h_tightMuPlusDzError       = new TH1F("tightMuPlusDzError",      "tightMuPlusDzError",       50, 0., 150.);
// remove tight info //  TH1F *h_tightMuUpperPlusDzError  = new TH1F("tightMuUpperPlusDzError", "tightMuUpperPlusDzError",  50, 0., 150.);
// remove tight info //  TH1F *h_tightMuLowerMinusDzError = new TH1F("tightMuLowerMinusDzError","tightMuLowerMinusDzError", 50, 0., 150.);
// remove tight info //  TH1F *h_tightMuLowerPlusDzError  = new TH1F("tightMuLowerPlusDzError", "tightMuLowerPlusDzError",  50, 0., 150.);
// remove tight info //
// remove tight info //  TH1F *h_tightMuMinusTrackPt      = new TH1F("tightMuMinusTrackPt",     "tightMuMinusTrackPt",      300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuUpperMinusTrackPt = new TH1F("tightMuUpperMinusTrackPt","tightMuUpperMinusTrackPt", 300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuPlusTrackPt       = new TH1F("tightMuPlusTrackPt",      "tightMuPlusTrackPt",       300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuUpperPlusTrackPt  = new TH1F("tightMuUpperPlusTrackPt", "tightMuUpperPlusTrackPt",  300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuLowerMinusTrackPt = new TH1F("tightMuLowerMinusTrackPt","tightMuLowerMinusTrackPt", 300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuLowerPlusTrackPt  = new TH1F("tightMuLowerPlusTrackPt", "tightMuLowerPlusTrackPt",  300, 0., 3000.);
// remove tight info //  TH1F *h_tightMuMinusTrackEta      = new TH1F("tightMuMinusTrackEta",   "tightMuMinusTrackEta",        40, -2., 2.);
// remove tight info //  TH1F *h_tightMuUpperMinusTrackEta = new TH1F("tightMuUpperMinusTrackEta","tightMuUpperMinusTrackEta", 40, -2., 2.);
// remove tight info //  TH1F *h_tightMuPlusTrackEta       = new TH1F("tightMuPlusTrackEta",      "tightMuPlusTrackEta",       40, -2., 2.);
// remove tight info //  TH1F *h_tightMuUpperPlusTrackEta  = new TH1F("tightMuUpperPlusTrackEta", "tightMuUpperPlusTrackEta",  40, -2., 2.);
// remove tight info //  TH1F *h_tightMuLowerMinusTrackEta = new TH1F("tightMuLowerMinusTrackEta","tightMuLowerMinusTrackEta", 40, -2., 2.);
// remove tight info //  TH1F *h_tightMuLowerPlusTrackEta  = new TH1F("tightMuLowerPlusTrackEta", "tightMuLowerPlusTrackEta",  40, -2., 2.);
// remove tight info //  TH1F *h_tightMuMinusTrackPhi      = new TH1F("tightMuMinusTrackPhi",     "tightMuMinusTrackPhi",      40, -4., 4.);
// remove tight info //  TH1F *h_tightMuUpperMinusTrackPhi = new TH1F("tightMuUpperMinusTrackPhi","tightMuUpperMinusTrackPhi", 40, -4., 4.);
// remove tight info //  TH1F *h_tightMuPlusTrackPhi       = new TH1F("tightMuPlusTrackPhi",      "tightMuPlusTrackPhi",       40, -4., 4.);
// remove tight info //  TH1F *h_tightMuUpperPlusTrackPhi  = new TH1F("tightMuUpperPlusTrackPhi", "tightMuUpperPlusTrackPhi",  40, -4., 4.);
// remove tight info //  TH1F *h_tightMuLowerMinusTrackPhi = new TH1F("tightMuLowerMinusTrackPhi","tightMuLowerMinusTrackPhi", 40, -4., 4.);
// remove tight info //  TH1F *h_tightMuLowerPlusTrackPhi  = new TH1F("tightMuLowerPlusTrackPhi", "tightMuLowerPlusTrackPhi",  40, -4., 4.);
// remove tight info //  TH1F *h_tightMuMinusPtError      = new TH1F("tightMuMinusPtError",     "tightMuMinusPtError",      100, 0., 600.);
// remove tight info //  TH1F *h_tightMuUpperMinusPtError = new TH1F("tightMuUpperMinusPtError","tightMuUpperMinusPtError", 100, 0., 600.);
// remove tight info //  TH1F *h_tightMuPlusPtError       = new TH1F("tightMuPlusPtError",      "tightMuPlusPtError",       100, 0., 600.);
// remove tight info //  TH1F *h_tightMuUpperPlusPtError  = new TH1F("tightMuUpperPlusPtError", "tightMuUpperPlusPtError",  100, 0., 600.);
// remove tight info //  TH1F *h_tightMuLowerMinusPtError = new TH1F("tightMuLowerMinusPtError","tightMuLowerMinusPtError", 100, 0., 600.);
// remove tight info //  TH1F *h_tightMuLowerPlusPtError  = new TH1F("tightMuLowerPlusPtError", "tightMuLowerPlusPtError",  100, 0., 600.);
// remove tight info //  TH1F *h_tightMuMinusPtRelErr      = new TH1F("tightMuMinusPtRelErr",     "tightMuMinusPtRelErr",      100, 0., 1.);
// remove tight info //  TH1F *h_tightMuUpperMinusPtRelErr = new TH1F("tightMuUpperMinusPtRelErr","tightMuUpperMinusPtRelErr", 100, 0., 1.);
// remove tight info //  TH1F *h_tightMuPlusPtRelErr       = new TH1F("tightMuPlusPtRelErr",      "tightMuPlusPtRelErr",       100, 0., 1.);
// remove tight info //  TH1F *h_tightMuUpperPlusPtRelErr  = new TH1F("tightMuUpperPlusPtRelErr", "tightMuUpperPlusPtRelErr",  100, 0., 1.);
// remove tight info //  TH1F *h_tightMuLowerMinusPtRelErr = new TH1F("tightMuLowerMinusPtRelErr","tightMuLowerMinusPtRelErr", 100, 0., 1.);
// remove tight info //  TH1F *h_tightMuLowerPlusPtRelErr  = new TH1F("tightMuLowerPlusPtRelErr","tightMuLowerPlusPtRelErr",   100, 0., 1.);
// remove tight info //
// remove tight info //  TH1F *h_tightMuMinusPixelHits      = new TH1F("tightMuMinusPixelHits",     "tightMuMinusPixelHits",      10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuUpperMinusPixelHits = new TH1F("tightMuUpperMinusPixelHits","tightMuUpperMinusPixelHits", 10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuPlusPixelHits       = new TH1F("tightMuPlusPixelHits",      "tightMuPlusPixelHits",       10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuUpperPlusPixelHits  = new TH1F("tightMuUpperPlusPixelHits", "tightMuUpperPlusPixelHits",  10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuLowerMinusPixelHits = new TH1F("tightMuLowerMinusPixelHits","tightMuLowerMinusPixelHits", 10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuLowerPlusPixelHits  = new TH1F("tightMuLowerPlusPixelHits", "tightMuLowerPlusPixelHits",  10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuMinusTrackerHits      = new TH1F("tightMuMinusTrackerHits",     "tightMuMinusTrackerHits",      35, -0.5, 34.5);
// remove tight info //  TH1F *h_tightMuUpperMinusTrackerHits = new TH1F("tightMuUpperMinusTrackerHits","tightMuUpperMinusTrackerHits", 35, -0.5, 34.5);
// remove tight info //  TH1F *h_tightMuPlusTrackerHits       = new TH1F("tightMuPlusTrackerHits",      "tightMuPlusTrackerHits",       35, -0.5, 34.5);
// remove tight info //  TH1F *h_tightMuUpperPlusTrackerHits  = new TH1F("tightMuUpperPlusTrackerHits", "tightMuUpperPlusTrackerHits",  35, -0.5, 34.5);
// remove tight info //  TH1F *h_tightMuLowerMinusTrackerHits = new TH1F("tightMuLowerMinusTrackerHits","tightMuLowerMinusTrackerHits", 35, -0.5, 34.5);
// remove tight info //  TH1F *h_tightMuLowerPlusTrackerHits  = new TH1F("tightMuLowerPlusTrackerHits", "tightMuLowerPlusTrackerHits",  35, -0.5, 34.5);
// remove tight info //  TH1F *h_tightMuMinusMuonStationHits      = new TH1F("tightMuMinusMuonStationHits",     "tightMuMinusMuonStationHits",      10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuUpperMinusMuonStationHits = new TH1F("tightMuUpperMinusMuonStationHits","tightMuUpperMinusMuonStationHits", 10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuPlusMuonStationHits       = new TH1F("tightMuPlusMuonStationHits",      "tightMuPlusMuonStationHits",       10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuUpperPlusMuonStationHits  = new TH1F("tightMuUpperPlusMuonStationHits", "tightMuUpperPlusMuonStationHits",  10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuLowerMinusMuonStationHits = new TH1F("tightMuLowerMinusMuonStationHits","tightMuLowerMinusMuonStationHits", 10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuLowerPlusMuonStationHits  = new TH1F("tightMuLowerPlusMuonStationHits", "tightMuLowerPlusMuonStationHits",  10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuMinusValidHits      = new TH1F("tightMuMinusValidHits",     "tightMuMinusValidHits",      100,-0.5, 99.5);
// remove tight info //  TH1F *h_tightMuUpperMinusValidHits = new TH1F("tightMuUpperMinusValidHits","tightMuUpperMinusValidHits", 100,-0.5, 99.5);
// remove tight info //  TH1F *h_tightMuPlusValidHits       = new TH1F("tightMuPlusValidHits",      "tightMuPlusValidHits",       100,-0.5, 99.5);
// remove tight info //  TH1F *h_tightMuUpperPlusValidHits  = new TH1F("tightMuUpperPlusValidHits", "tightMuUpperPlusValidHits",  100,-0.5, 99.5);
// remove tight info //  TH1F *h_tightMuLowerMinusValidHits = new TH1F("tightMuLowerMinusValidHits","tightMuLowerMinusValidHits", 100,-0.5, 99.5);
// remove tight info //  TH1F *h_tightMuLowerPlusValidHits  = new TH1F("tightMuLowerPlusValidHits", "tightMuLowerPlusValidHits",  100,-0.5, 99.5);
// remove tight info //  TH1F *h_tightMuMinusValidMuonHits      = new TH1F("tightMuMinusValidMuonHits",     "tightMuMinusValidMuonHits",      75, -0.5, 74.5);
// remove tight info //  TH1F *h_tightMuUpperMinusValidMuonHits = new TH1F("tightMuUpperMinusValidMuonHits","tightMuUpperMinusValidMuonHits", 75, -0.5, 74.5);
// remove tight info //  TH1F *h_tightMuPlusValidMuonHits       = new TH1F("tightMuPlusValidMuonHits",      "tightMuPlusValidMuonHits",       75, -0.5, 74.5);
// remove tight info //  TH1F *h_tightMuUpperPlusValidMuonHits  = new TH1F("tightMuUpperPlusValidMuonHits", "tightMuUpperPlusValidMuonHits",  75, -0.5, 74.5);
// remove tight info //  TH1F *h_tightMuLowerMinusValidMuonHits = new TH1F("tightMuLowerMinusValidMuonHits","tightMuLowerMinusValidMuonHits", 75, -0.5, 74.5);
// remove tight info //  TH1F *h_tightMuLowerPlusValidMuonHits  = new TH1F("tightMuLowerPlusValidMuonHits", "tightMuLowerPlusValidMuonHits",  75, -0.5, 74.5);
// remove tight info //  TH1F *h_tightMuMinusMatchedMuonStations      = new TH1F("tightMuMinusMatchedMuonStations",     "tightMuMinusMatchedMuonStations",      10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuUpperMinusMatchedMuonStations = new TH1F("tightMuUpperMinusMatchedMuonStations","tightMuUpperMinusMatchedMuonStations", 10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuPlusMatchedMuonStations       = new TH1F("tightMuPlusMatchedMuonStations",      "tightMuPlusMatchedMuonStations",       10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuUpperPlusMatchedMuonStations  = new TH1F("tightMuUpperPlusMatchedMuonStations", "tightMuUpperPlusMatchedMuonStations",  10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuLowerMinusMatchedMuonStations = new TH1F("tightMuLowerMinusMatchedMuonStations","tightMuLowerMinusMatchedMuonStations", 10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuLowerPlusMatchedMuonStations  = new TH1F("tightMuLowerPlusMatchedMuonStations", "tightMuLowerPlusMatchedMuonStations",  10, -0.5, 9.5);
// remove tight info //  TH1F *h_tightMuMinusTrackerLayersWithMeasurement      = new TH1F("tightMuMinusTrackerLayersWithMeasurement",     "tightMuMinusTrackerLayersWithMeasurement",      20, -0.5, 19.5);
// remove tight info //  TH1F *h_tightMuUpperMinusTrackerLayersWithMeasurement = new TH1F("tightMuUpperMinusTrackerLayersWithMeasurement","tightMuUpperMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
// remove tight info //  TH1F *h_tightMuPlusTrackerLayersWithMeasurement       = new TH1F("tightMuPlusTrackerLayersWithMeasurement",      "tightMuPlusTrackerLayersWithMeasurement",       20, -0.5, 19.5);
// remove tight info //  TH1F *h_tightMuUpperPlusTrackerLayersWithMeasurement  = new TH1F("tightMuUpperPlusTrackerLayersWithMeasurement", "tightMuUpperPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);
// remove tight info //  TH1F *h_tightMuLowerMinusTrackerLayersWithMeasurement = new TH1F("tightMuLowerMinusTrackerLayersWithMeasurement","tightMuLowerMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
// remove tight info //  TH1F *h_tightMuLowerPlusTrackerLayersWithMeasurement  = new TH1F("tightMuLowerPlusTrackerLayersWithMeasurement", "tightMuLowerPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);
// remove tight info //
// remove tight info //  TH1F *h_tightMuMinusCurvePlusBias[      nBiasBins];
// remove tight info //  TH1F *h_tightMuMinusCurveMinusBias[     nBiasBins];
// remove tight info //  TH1F *h_tightMuUpperMinusCurvePlusBias[ nBiasBins];
// remove tight info //  TH1F *h_tightMuUpperMinusCurveMinusBias[nBiasBins];
// remove tight info //  TH1F *h_tightMuLowerMinusCurvePlusBias[ nBiasBins];
// remove tight info //  TH1F *h_tightMuLowerMinusCurveMinusBias[nBiasBins];
// remove tight info //  TH1F *h_tightMuPlusCurvePlusBias[       nBiasBins];
// remove tight info //  TH1F *h_tightMuPlusCurveMinusBias[      nBiasBins];
// remove tight info //  TH1F *h_tightMuUpperPlusCurvePlusBias[  nBiasBins];
// remove tight info //  TH1F *h_tightMuUpperPlusCurveMinusBias[ nBiasBins];
// remove tight info //  TH1F *h_tightMuLowerPlusCurvePlusBias[  nBiasBins];
// remove tight info //  TH1F *h_tightMuLowerPlusCurveMinusBias[ nBiasBins];
// remove tight info //
// remove tight info //  for (int i =0; i < nBiasBins; ++i) {
// remove tight info //    std::stringstream name;
// remove tight info //    name << std::setw(3) << std::setfill('0') << i + 1;
// remove tight info //    h_tightMuMinusCurvePlusBias[i]       = new TH1F(TString("tightMuMinusCurvePlusBias"       + name.str()),
// remove tight info //						    TString("tightMuMinusCurvePlusBias"       + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //    h_tightMuMinusCurveMinusBias[i]      = new TH1F(TString("tightMuMinusCurveMinusBias"      + name.str()),
// remove tight info //						    TString("tightMuMinusCurveMinusBias"      + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
// remove tight info //    h_tightMuUpperMinusCurvePlusBias[i]  = new TH1F(TString("tightMuUpperMinusCurvePlusBias"  + name.str()),
// remove tight info //						    TString("tightMuUpperMinusCurvePlusBias"  + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //    h_tightMuUpperMinusCurveMinusBias[i] = new TH1F(TString("tightMuUpperMinusCurveMinusBias" + name.str()),
// remove tight info //						    TString("tightMuUpperMinusCurveMinusBias" + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
// remove tight info //    h_tightMuLowerMinusCurvePlusBias[i]  = new TH1F(TString("tightMuLowerMinusCurvePlusBias"  + name.str()),
// remove tight info //						    TString("tightMuLowerMinusCurvePlusBias"  + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //    h_tightMuLowerMinusCurveMinusBias[i] = new TH1F(TString("tightMuLowerMinusCurveMinusBias" + name.str()),
// remove tight info //						    TString("tightMuLowerMinusCurveMinusBias" + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
// remove tight info //    h_tightMuPlusCurvePlusBias[i]        = new TH1F(TString("tightMuPlusCurvePlusBias"        + name.str()),
// remove tight info //						    TString("tightMuPlusCurvePlusBias"        + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //    h_tightMuPlusCurveMinusBias[i]       = new TH1F(TString("tightMuPlusCurveMinusBias"       + name.str()),
// remove tight info //						    TString("tightMuPlusCurveMinusBias"       + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
// remove tight info //    h_tightMuUpperPlusCurvePlusBias[i]   = new TH1F(TString("tightMuUpperPlusCurvePlusBias"   + name.str()),
// remove tight info //						    TString("tightMuUpperPlusCurvePlusBias"   + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //    h_tightMuUpperPlusCurveMinusBias[i]  = new TH1F(TString("tightMuUpperPlusCurveMinusBias"  + name.str()),
// remove tight info //						    TString("tightMuUpperPlusCurveMinusBias"  + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
// remove tight info //    h_tightMuLowerPlusCurvePlusBias[i]   = new TH1F(TString("tightMuLowerPlusCurvePlusBias"   + name.str()),
// remove tight info //						    TString("tightMuLowerPlusCurvePlusBias"   + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);
// remove tight info //    h_tightMuLowerPlusCurveMinusBias[i]  = new TH1F(TString("tightMuLowerPlusCurveMinusBias"  + name.str()),
// remove tight info //						    TString("tightMuLowerPlusCurveMinusBias"  + name.str()),
// remove tight info //						    7500, symmetric_ ? -0.0075*factor_ : 0., 0.0075*factor_);  
// remove tight info //  }

  std::cout << "Creating upper muMinus TTreeReaderValues" << std::endl;
  TTreeReaderValue<Int_t>    run(  trackReader, "muonRunNumber"  );
  TTreeReaderValue<Int_t>    lumi( trackReader, "muonLumiBlock"  );
  TTreeReaderValue<Int_t>    event(trackReader, "muonEventNumber");
  
  TTreeReaderValue<math::XYZTLorentzVector> upTrackerMuonP4(trackReader,"upperMuon_P4"      );
  TTreeReaderValue<math::XYZVector>         upTrackerTrack( trackReader,"upperMuon_trackVec");
  TTreeReaderValue<Double_t> upTrackerPt(      trackReader, "upperMuon_trackPt" );
  TTreeReaderValue<Int_t>    upTrackerCharge(  trackReader, "upperMuon_charge"  );
  TTreeReaderValue<Double_t> upTrackerChi2(    trackReader, "upperMuon_chi2"    );
  TTreeReaderValue<Int_t>    upTrackerNdof(    trackReader, "upperMuon_ndof"    );
  TTreeReaderValue<Double_t> upTrackerDxy(     trackReader, "upperMuon_dxy"     );
  TTreeReaderValue<Double_t> upTrackerDz(      trackReader, "upperMuon_dz"      );
  TTreeReaderValue<Double_t> upTrackerDxyError(trackReader, "upperMuon_dxyError");
  TTreeReaderValue<Double_t> upTrackerDzError( trackReader, "upperMuon_dzError" );
  TTreeReaderValue<Double_t> upTrackerPtError( trackReader, "upperMuon_ptError" );

  TTreeReaderValue<Int_t> upTrackerPhits(                trackReader, "upperMuon_pixelHits"                   );
  TTreeReaderValue<Int_t> upTrackerThits(                trackReader, "upperMuon_trackerHits"                 );
  TTreeReaderValue<Int_t> upTrackerMhits(                trackReader, "upperMuon_muonStationHits"             );
  TTreeReaderValue<Int_t> upTrackerValidHits(            trackReader, "upperMuon_numberOfValidHits"           );
  TTreeReaderValue<Int_t> upTrackerValidMuonHits(        trackReader, "upperMuon_numberOfValidMuonHits"       );
  TTreeReaderValue<Int_t> upTrackerMatchedMuonStations(  trackReader, "upperMuon_numberOfMatchedStations"     );
  TTreeReaderValue<Int_t> upTrackerLayersWithMeasurement(trackReader, "upperMuon_trackerLayersWithMeasurement");


  std::cout << "Creating lower muMinus TTreeReaderValues" << std::endl;
  TTreeReaderValue<math::XYZTLorentzVector> lowTrackerMuonP4(trackReader,"lowerMuon_P4"      );
  TTreeReaderValue<math::XYZVector>         lowTrackerTrack( trackReader,"lowerMuon_trackVec");
  TTreeReaderValue<Double_t> lowTrackerPt(      trackReader, "lowerMuon_trackPt" );
  TTreeReaderValue<Int_t>    lowTrackerCharge(  trackReader, "lowerMuon_charge"  );
  TTreeReaderValue<Double_t> lowTrackerChi2(    trackReader, "lowerMuon_chi2"    );
  TTreeReaderValue<Int_t>    lowTrackerNdof(    trackReader, "lowerMuon_ndof"    );
  TTreeReaderValue<Double_t> lowTrackerDxy(     trackReader, "lowerMuon_dxy"     );
  TTreeReaderValue<Double_t> lowTrackerDz(      trackReader, "lowerMuon_dz"      );
  TTreeReaderValue<Double_t> lowTrackerDxyError(trackReader, "lowerMuon_dxyError");
  TTreeReaderValue<Double_t> lowTrackerDzError( trackReader, "lowerMuon_dzError" );
  TTreeReaderValue<Double_t> lowTrackerPtError( trackReader, "lowerMuon_ptError" );

  TTreeReaderValue<Int_t>    lowTrackerPhits(                trackReader, "lowerMuon_pixelHits"                   );
  TTreeReaderValue<Int_t>    lowTrackerThits(                trackReader, "lowerMuon_trackerHits"                 );
  TTreeReaderValue<Int_t>    lowTrackerMhits(                trackReader, "lowerMuon_muonStationHits"             );
  TTreeReaderValue<Int_t>    lowTrackerValidHits(            trackReader, "lowerMuon_numberOfValidHits"           );
  TTreeReaderValue<Int_t>    lowTrackerValidMuonHits(        trackReader, "lowerMuon_numberOfValidMuonHits"       );
  TTreeReaderValue<Int_t>    lowTrackerMatchedMuonStations(  trackReader, "lowerMuon_numberOfMatchedStations"     );
  TTreeReaderValue<Int_t>    lowTrackerLayersWithMeasurement(trackReader, "lowerMuon_trackerLayersWithMeasurement");
  
  std::cout << "Made it to Histogramming!" << std::endl;
  int j = 0;
  double maxDR = 0.15; // what is reasonable here? Aachen did dPhi < 0.1, dTheta (eta?) < 0.05
  while (trackReader.Next()) {
    if (debug)
      std::cout << "Made it into the first loop" << std::endl;
    g->cd();

    bool hasPt100Loose(false), hasPt200Loose(false), hasPt400Loose(false);
    bool hasPt100Tight(false), hasPt200Tight(false), hasPt400Tight(false);
    
    std::stringstream upperstring;
    std::stringstream lowerstring;
    
    // make sure we're not reading from the skipped events
    if (*upTrackerChi2 > -1) {
      // what about cases where the upper/lower muon have pT passing, but not the other leg
      // also, interest in (q/pT_up - q/pT_low)/(sqrt(2)*(q/pT_low)), relative residual
      // and possibly (q/pT_low - q/pT_up)/(sqrt(2)*(q/pT_up)), relative residual?
      // binned vs. pT (50,60,75,100,150,200,300,400,500,750,1000,1500,2000,3000,inf?
      // can't apply a tight min pT cut for these
      // should we apply a dR cut to ensure they are well matched, e.g., dR < 0.1, 0.3?
      
      //if (sqrt(upTrackerTrack->perp2()) > minPt_) {
      if (sqrt(upTrackerTrack->perp2()) > minPt_ || sqrt(lowTrackerTrack->perp2()) > minPt_) {
	/** ensure that the two muon tracks are indeed the same muon */
	// double deltaR = upTrackerMuonP4->DR(*lowTrackerMuonP4); // why doesn't this work?
	/*
	  double dEta = upTrackerMuonP4->eta()-lowTrackerMuonP4->eta();
	  double dPhi = upTrackerMuonP4->phi()-lowTrackerMuonP4->phi();
	  if (dPhi >= M_PI)
	  dPhi-=2*M_PI;
	  else if (dPhi < -M_PI)
	  dPhi+=2*M_PI;
	  double deltaR = sqrt((dEta*dEta) + (dPhi*dPhi));
	  if (deltaR > maxDR)
	  continue;
	*/
	double upperCpT = factor_*(*upTrackerCharge)/(sqrt(upTrackerTrack->perp2())); //(*upTrackerMuonP4).Pt();
	/*
	// make the curvature absolute value (asymmetric)
	if (!symmetric_)
	  upperCpT = factor_/(sqrt(upTrackerTrack->perp2()));//upTrackerMuonP4->pt();
	*/
	double upperRelPtErr = *upTrackerPtError/(sqrt(upTrackerTrack->perp2()));//upTrackerMuonP4->pt();
      
	// make bool's for each cut level?
	// uint32_t upperCuts; // 1 bit per cut?
      
	bool up_tightdxy = (*upTrackerDxy < 0.2) ? 1 : 0;
	bool up_tightdz  = (*upTrackerDz  < 0.5) ? 1 : 0;
	bool up_superpointing = ((std::fabs(*upTrackerDxy) < 10) && (std::fabs(*upTrackerDz)  < 50))
	  ? 1 : 0;

	// if using TuneP or TrackerOnly and pT < 200, should *not* apply muon system cuts
	// bool upperMuStationHits = (!istrackerp || (istunep && sqrt(upTrackerTrack->perp2()) > 200)) ? *upTrackerMatchedMuonStations > 1 : 1;
	bool upperMuStationHits = *upTrackerMatchedMuonStations > 1;
	bool upperValidMuHits   = (!istrackerp || (istunep && sqrt(upTrackerTrack->perp2()) > 200)) ? *upTrackerValidMuonHits       > 0 : 1;

	bool up_n1dxymax      = (upperValidMuHits        &&
				 upperMuStationHits      &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerPhits > 0  ) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool up_n1dzmax       = (upperValidMuHits        &&
				 upperMuStationHits      &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerPhits > 0  ) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool up_n1ptrelerr    = (upperValidMuHits      &&
				 upperMuStationHits    &&
				 (*upTrackerPhits > 0) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool up_n1pt          = (upperValidMuHits        &&
				 upperMuStationHits      &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerPhits > 0  ) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool up_n1trkhits     = (upperValidMuHits        &&
				 upperMuStationHits      &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerPhits > 0 ))
	  ? 1 : 0;
	bool up_n1pixhits     = (upperValidMuHits        &&
				 upperMuStationHits      &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool up_n1vmuhits     = (upperMuStationHits      &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerPhits > 0  ) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool up_n1mmustahits  = (upperValidMuHits        &&
				 (upperRelPtErr   < 0.3) &&
				 (*upTrackerPhits > 0  ) &&
				 (*upTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;

	if ((j % 100) == 0)
	  std::cout << "upper leg" << std::endl
		    << "mu pt  = " << std::setw(8) << std::setprecision(2) << std::fixed << upTrackerMuonP4->pt()
		    << " - eta = " << std::setw(6) << std::setprecision(2) << std::fixed << upTrackerMuonP4->eta()
		    << " - phi = " << std::setw(6) << std::setprecision(2) << std::fixed << upTrackerMuonP4->phi()
		    << std::endl

		    << "trk pt = " << std::setw(8) << std::setprecision(2) << std::fixed << sqrt(upTrackerTrack->perp2())
		    << " - eta = " << std::setw(6) << std::setprecision(2) << std::fixed << upTrackerTrack->eta()
		    << " - phi = " << std::setw(6) << std::setprecision(2) << std::fixed << upTrackerTrack->phi()
		    << std::endl;
	
	h_upperChi2->Fill(*upTrackerChi2);
	h_upperNdof->Fill(*upTrackerNdof);
	h_upperCharge->Fill(*upTrackerCharge);
	h_upperCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));

	h_upperDxy->Fill(*upTrackerDxy);
	h_upperDz->Fill(*upTrackerDz);
	h_upperDxyError->Fill(*upTrackerDxyError);
	h_upperDzError->Fill(*upTrackerDzError);

	h_upperPt->Fill(upTrackerMuonP4->pt());
	h_upperEta->Fill(upTrackerMuonP4->eta());
	h_upperPhi->Fill(upTrackerMuonP4->phi());
	
	h_upperPtError->Fill(*upTrackerPtError);
	h_upperPtRelErr->Fill(upperRelPtErr);
	h_upperTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	h_upperTrackEta->Fill(upTrackerTrack->eta());
	h_upperTrackPhi->Fill(upTrackerTrack->phi());

	upperstring << "pt="      << sqrt(upTrackerTrack->perp2())
		    << ",eta="    << upTrackerTrack->eta()
		    << ",phi="    << upTrackerTrack->phi()
		    << ",charge=" << *upTrackerCharge;

	h_upperPixelHits->Fill(*upTrackerPhits);
	h_upperTrackerHits->Fill(*upTrackerThits);
	h_upperValidHits->Fill(*upTrackerValidHits);
	h_upperValidMuonHits->Fill(*upTrackerValidMuonHits);
	h_upperMuonStationHits->Fill(*upTrackerMhits);
	h_upperMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	h_upperTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

	for (int i = 0; i < nBiasBins; ++i) {
	  double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
	  double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
	  h_upperCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	  h_upperCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	  if (debug)
	    std::cout << "Made it through the upper bias loop " << i << std::endl; 
	}

	// fill the counters histogram for the upper leg muons passing the super-pointing selection
	if (up_superpointing || true) {
	  h_countersUpper->Fill(0);
	  
	  if (up_n1ptrelerr)
	    h_countersUpper->Fill(1);
	  if (up_n1trkhits)
	    h_countersUpper->Fill(2);
	  if (up_n1pixhits)
	    h_countersUpper->Fill(3);
	  if (up_n1vmuhits)
	    h_countersUpper->Fill(4);
	  if (up_n1mmustahits)
	    h_countersUpper->Fill(5);
	  if (up_n1dxymax)
	    h_countersUpper->Fill(6);
	  if (up_n1dzmax)
	    h_countersUpper->Fill(7);
	  if (up_n1pt)
	    h_countersUpper->Fill(8);

	  if (up_tightdxy) {
	    if (up_n1ptrelerr)
	      h_countersUpper->Fill(11);
	    if (up_n1trkhits)
	      h_countersUpper->Fill(12);
	    if (up_n1pixhits)
	      h_countersUpper->Fill(13);
	    if (up_n1vmuhits)
	      h_countersUpper->Fill(14);
	    if (up_n1mmustahits)
	      h_countersUpper->Fill(15);
	    if (up_n1dxymax)
	      h_countersUpper->Fill(16);
	    if (up_n1dzmax)
	      h_countersUpper->Fill(17);
	    if (up_n1pt)
	      h_countersUpper->Fill(18);
	  }
	  
	  if (up_tightdz) {
	    if (up_n1ptrelerr)
	      h_countersUpper->Fill(21);
	    if (up_n1trkhits)
	      h_countersUpper->Fill(22);
	    if (up_n1pixhits)
	      h_countersUpper->Fill(23);
	    if (up_n1vmuhits)
	      h_countersUpper->Fill(24);
	    if (up_n1mmustahits)
	      h_countersUpper->Fill(25);
	    if (up_n1dxymax)
	      h_countersUpper->Fill(26);
	    if (up_n1dzmax)
	      h_countersUpper->Fill(27);
	    if (up_n1pt)
	      h_countersUpper->Fill(28);
	  }
	  
	  if (up_tightdxy && up_tightdz) {
	    if (up_n1ptrelerr)
	      h_countersUpper->Fill(31);
	    if (up_n1trkhits)
	      h_countersUpper->Fill(32);
	    if (up_n1pixhits)
	      h_countersUpper->Fill(33);
	    if (up_n1vmuhits)
	      h_countersUpper->Fill(34);
	    if (up_n1mmustahits)
	      h_countersUpper->Fill(35);
	    if (up_n1dxymax)
	      h_countersUpper->Fill(36);
	    if (up_n1dzmax)
	      h_countersUpper->Fill(37);
	    if (up_n1pt)
	      h_countersUpper->Fill(38);
	  }
	  
	  if (up_n1pt) {
	    // counters of passing the loose selection binned by pT
	    if (sqrt(upTrackerTrack->perp2()) > 50)
	      h_countersUpper->Fill(40);
	    if (sqrt(upTrackerTrack->perp2()) > 100)
	      h_countersUpper->Fill(41);
	    if (sqrt(upTrackerTrack->perp2()) > 150)
	      h_countersUpper->Fill(42);
	    if (sqrt(upTrackerTrack->perp2()) > 200)
	      h_countersUpper->Fill(43);
	    if (sqrt(upTrackerTrack->perp2()) > 300)
	      h_countersUpper->Fill(44);
	    if (sqrt(upTrackerTrack->perp2()) > 400)
	      h_countersUpper->Fill(45);
	    if (sqrt(upTrackerTrack->perp2()) > 500)
	      h_countersUpper->Fill(46);
	    if (sqrt(upTrackerTrack->perp2()) > 1000)
	      h_countersUpper->Fill(47);
	    if (sqrt(upTrackerTrack->perp2()) > 1500)
	      h_countersUpper->Fill(48);
	    if (sqrt(upTrackerTrack->perp2()) > 2000)
	      h_countersUpper->Fill(49);
	    if (sqrt(upTrackerTrack->perp2()) > 3000)
	      h_countersUpper->Fill(50);
	  }

	  if (*upTrackerCharge < 0) {
	    h_muMinusChi2->Fill(*upTrackerChi2);
	    h_muUpperMinusChi2->Fill(*upTrackerChi2);
	    h_muMinusNdof->Fill(*upTrackerNdof);
	    h_muUpperMinusNdof->Fill(*upTrackerNdof);
	    h_muMinusCharge->Fill(*upTrackerCharge);
	    h_muUpperMinusCharge->Fill(*upTrackerCharge);
	    h_muMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	    h_muUpperMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));

	    h_muMinusDxy->Fill(*upTrackerDxy);
	    h_muUpperMinusDxy->Fill(*upTrackerDxy);
	    h_muMinusDz->Fill(*upTrackerDz);
	    h_muUpperMinusDz->Fill(*upTrackerDz);
	    h_muMinusDxyError->Fill(*upTrackerDxyError);
	    h_muUpperMinusDxyError->Fill(*upTrackerDxyError);
	    h_muMinusDzError->Fill(*upTrackerDzError);
	    h_muUpperMinusDzError->Fill(*upTrackerDzError);

	    h_muMinusPt->Fill(upTrackerMuonP4->pt());
	    h_muUpperMinusPt->Fill(upTrackerMuonP4->pt());
	    h_muMinusEta->Fill(upTrackerMuonP4->eta());
	    h_muUpperMinusEta->Fill(upTrackerMuonP4->eta());
	    h_muMinusPhi->Fill(upTrackerMuonP4->phi());
	    h_muUpperMinusPhi->Fill(upTrackerMuonP4->phi());

	    h_muMinusPtError->Fill(*upTrackerPtError);
	    h_muUpperMinusPtError->Fill(*upTrackerPtError);
	    h_muMinusPtRelErr->Fill(upperRelPtErr);
	    h_muUpperMinusPtRelErr->Fill(upperRelPtErr);
	    h_muMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	    h_muUpperMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	    h_muMinusTrackEta->Fill(upTrackerTrack->eta());
	    h_muUpperMinusTrackEta->Fill(upTrackerTrack->eta());
	    h_muMinusTrackPhi->Fill(upTrackerTrack->phi());
	    h_muUpperMinusTrackPhi->Fill(upTrackerTrack->phi());

	    h_muMinusPixelHits->Fill(*upTrackerPhits);
	    h_muUpperMinusPixelHits->Fill(*upTrackerPhits);
	    h_muMinusTrackerHits->Fill(*upTrackerThits);
	    h_muUpperMinusTrackerHits->Fill(*upTrackerThits);
	    h_muMinusValidHits->Fill(*upTrackerValidHits);
	    h_muUpperMinusValidHits->Fill(*upTrackerValidHits);
	    h_muMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	    h_muUpperMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	    h_muMinusMuonStationHits->Fill(*upTrackerMhits);
	    h_muUpperMinusMuonStationHits->Fill(*upTrackerMhits);
	    h_muMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	    h_muUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	    h_muMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	    h_muUpperMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
	      h_muUpperMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muUpperMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
	  
	      if (debug)
		std::cout << "Made it through the upper bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (up_n1pt) {
	      h_looseMuMinusChi2->Fill(*upTrackerChi2);
	      h_looseMuUpperMinusChi2->Fill(*upTrackerChi2);
	      h_looseMuMinusNdof->Fill(*upTrackerNdof);
	      h_looseMuUpperMinusNdof->Fill(*upTrackerNdof);
	      h_looseMuMinusCharge->Fill(*upTrackerCharge);
	      h_looseMuUpperMinusCharge->Fill(*upTrackerCharge);
	      h_looseMuMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      h_looseMuUpperMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuMinusDxy->Fill(*upTrackerDxy);
	      h_looseMuUpperMinusDxy->Fill(*upTrackerDxy);
	      h_looseMuMinusDz->Fill(*upTrackerDz);
	      h_looseMuUpperMinusDz->Fill(*upTrackerDz);
	      h_looseMuMinusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuUpperMinusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuMinusDzError->Fill(*upTrackerDzError);
	      h_looseMuUpperMinusDzError->Fill(*upTrackerDzError);

	      h_looseMuMinusPt->Fill(upTrackerMuonP4->pt());
	      h_looseMuUpperMinusPt->Fill(upTrackerMuonP4->pt());
	      h_looseMuMinusEta->Fill(upTrackerMuonP4->eta());
	      h_looseMuUpperMinusEta->Fill(upTrackerMuonP4->eta());
	      h_looseMuMinusPhi->Fill(upTrackerMuonP4->phi());
	      h_looseMuUpperMinusPhi->Fill(upTrackerMuonP4->phi());

	      h_looseMuMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      h_looseMuUpperMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      h_looseMuMinusTrackEta->Fill(upTrackerTrack->eta());
	      h_looseMuUpperMinusTrackEta->Fill(upTrackerTrack->eta());
	      h_looseMuMinusTrackPhi->Fill(upTrackerTrack->phi());
	      h_looseMuUpperMinusTrackPhi->Fill(upTrackerTrack->phi());

	      h_looseMuMinusTrackerHits->Fill(*upTrackerThits);
	      h_looseMuUpperMinusTrackerHits->Fill(*upTrackerThits);
	      h_looseMuMinusValidHits->Fill(*upTrackerValidHits);
	      h_looseMuUpperMinusValidHits->Fill(*upTrackerValidHits);
	      h_looseMuMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      h_looseMuUpperMinusValidMuonHits->Fill(*upTrackerValidMuonHits);

	      // remove tight info //if (up_tightdxy) {
	      // remove tight info //	h_tightMuMinusDz->Fill(*upTrackerDz);
	      // remove tight info //	h_tightMuUpperMinusDz->Fill(*upTrackerDz);
	      // remove tight info //	h_tightMuMinusDzError->Fill(*upTrackerDzError);
	      // remove tight info //	h_tightMuUpperMinusDzError->Fill(*upTrackerDzError);
	      // remove tight info //}
	      // remove tight info //if (up_tightdz) {
	      // remove tight info //	h_tightMuMinusDxy->Fill(*upTrackerDxy);
	      // remove tight info //	h_tightMuUpperMinusDxy->Fill(*upTrackerDxy);
	      // remove tight info //	h_tightMuMinusDxyError->Fill(*upTrackerDxyError);
	      // remove tight info //	h_tightMuUpperMinusDxyError->Fill(*upTrackerDxyError);
	      // remove tight info //}
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuMinusChi2->Fill(*upTrackerChi2);
	      // remove tight info //	h_tightMuUpperMinusChi2->Fill(*upTrackerChi2);
	      // remove tight info //	h_tightMuMinusNdof->Fill(*upTrackerNdof);
	      // remove tight info //	h_tightMuUpperMinusNdof->Fill(*upTrackerNdof);
	      // remove tight info //	h_tightMuMinusCharge->Fill(*upTrackerCharge);
	      // remove tight info //	h_tightMuUpperMinusCharge->Fill(*upTrackerCharge);
	      // remove tight info //	h_tightMuMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      // remove tight info //	h_tightMuUpperMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      // remove tight info //
	      // remove tight info //	h_tightMuMinusPt->Fill(upTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuUpperMinusPt->Fill(upTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuMinusEta->Fill(upTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuUpperMinusEta->Fill(upTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuMinusPhi->Fill(upTrackerMuonP4->phi());
	      // remove tight info //	h_tightMuUpperMinusPhi->Fill(upTrackerMuonP4->phi());
	      // remove tight info //
	      // remove tight info //	h_tightMuMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuUpperMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuMinusTrackEta->Fill(upTrackerTrack->eta());
	      // remove tight info //	h_tightMuUpperMinusTrackEta->Fill(upTrackerTrack->eta());
	      // remove tight info //	h_tightMuMinusTrackPhi->Fill(upTrackerTrack->phi());
	      // remove tight info //	h_tightMuUpperMinusTrackPhi->Fill(upTrackerTrack->phi());
	      // remove tight info //
	      // remove tight info //	h_tightMuMinusTrackerHits->Fill(*upTrackerThits);
	      // remove tight info //	h_tightMuUpperMinusTrackerHits->Fill(*upTrackerThits);
	      // remove tight info //}
	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		h_looseMuUpperMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuUpperMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //if (up_tightdxy && up_tightdz) {
		// remove tight info //  h_tightMuMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //  h_tightMuUpperMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuUpperMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //}
		if (debug)
		  std::cout << "Made it through the upper bias loop " << i << std::endl; 
	      }
	    } // end if (up_n1pt)

	    if (up_n1pixhits) {
	      h_looseMuMinusPixelHits->Fill(*upTrackerPhits);
	      h_looseMuUpperMinusPixelHits->Fill(*upTrackerPhits);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuMinusPixelHits->Fill(*upTrackerPhits);
	      // remove tight info //	h_tightMuUpperMinusPixelHits->Fill(*upTrackerPhits);
	      // remove tight info //}
	    }
	    if (up_n1vmuhits) {
	      h_looseMuMinusValidHits->Fill(*upTrackerValidHits);
	      h_looseMuUpperMinusValidHits->Fill(*upTrackerValidHits);
	      h_looseMuMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      h_looseMuUpperMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      h_looseMuMinusMuonStationHits->Fill(*upTrackerMhits);
	      h_looseMuUpperMinusMuonStationHits->Fill(*upTrackerMhits);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuMinusValidHits->Fill(*upTrackerValidHits);
	      // remove tight info //	h_tightMuUpperMinusValidHits->Fill(*upTrackerValidHits);
	      // remove tight info //	h_tightMuMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      // remove tight info //	h_tightMuUpperMinusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      // remove tight info //	h_tightMuMinusMuonStationHits->Fill(*upTrackerMhits);
	      // remove tight info //	h_tightMuUpperMinusMuonStationHits->Fill(*upTrackerMhits);
	      // remove tight info //}
	    }
	    if (up_n1ptrelerr) {
	      h_looseMuMinusPtError->Fill(*upTrackerPtError);
	      h_looseMuUpperMinusPtError->Fill(*upTrackerPtError);
	      h_looseMuMinusPtRelErr->Fill(upperRelPtErr);
	      h_looseMuUpperMinusPtRelErr->Fill(upperRelPtErr);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuMinusPtError->Fill(*upTrackerPtError);
	      // remove tight info //	h_tightMuUpperMinusPtError->Fill(*upTrackerPtError);
	      // remove tight info //	h_tightMuMinusPtRelErr->Fill(upperRelPtErr);
	      // remove tight info //	h_tightMuUpperMinusPtRelErr->Fill(upperRelPtErr);
	      // remove tight info //}
	    }
	    if (up_n1trkhits) {
	      h_looseMuMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      h_looseMuUpperMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      // remove tight info //	h_tightMuUpperMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      // remove tight info //}
	    }
	    if (up_n1mmustahits) {
	      h_looseMuMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      h_looseMuUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      // remove tight info //	h_tightMuUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      // remove tight info //}
	    }
	  } // end if (charge < 0)

	  else { // charge > 0
	    h_muPlusChi2->Fill(*upTrackerChi2);
	    h_muUpperPlusChi2->Fill(*upTrackerChi2);
	    h_muPlusNdof->Fill(*upTrackerNdof);
	    h_muUpperPlusNdof->Fill(*upTrackerNdof);
	    h_muPlusCharge->Fill(*upTrackerCharge);
	    h_muUpperPlusCharge->Fill(*upTrackerCharge);
	    h_muPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	    h_muUpperPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));

	    h_muPlusDxy->Fill(*upTrackerDxy);
	    h_muUpperPlusDxy->Fill(*upTrackerDxy);
	    h_muPlusDz->Fill(*upTrackerDz);
	    h_muUpperPlusDz->Fill(*upTrackerDz);
	    h_muPlusDxyError->Fill(*upTrackerDxyError);
	    h_muUpperPlusDxyError->Fill(*upTrackerDxyError);
	    h_muPlusDzError->Fill(*upTrackerDzError);
	    h_muUpperPlusDzError->Fill(*upTrackerDzError);

	    h_muPlusPt->Fill(upTrackerMuonP4->pt());
	    h_muUpperPlusPt->Fill(upTrackerMuonP4->pt());
	    h_muPlusEta->Fill(upTrackerMuonP4->eta());
	    h_muUpperPlusEta->Fill(upTrackerMuonP4->eta());
	    h_muPlusPhi->Fill(upTrackerMuonP4->phi());
	    h_muUpperPlusPhi->Fill(upTrackerMuonP4->phi());

	    h_muPlusPtError->Fill(*upTrackerPtError);
	    h_muUpperPlusPtError->Fill(*upTrackerPtError);
	    h_muPlusPtRelErr->Fill(upperRelPtErr);
	    h_muUpperPlusPtRelErr->Fill(upperRelPtErr);
	    h_muPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	    h_muUpperPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	    h_muPlusTrackEta->Fill(upTrackerTrack->eta());
	    h_muUpperPlusTrackEta->Fill(upTrackerTrack->eta());
	    h_muPlusTrackPhi->Fill(upTrackerTrack->phi());
	    h_muUpperPlusTrackPhi->Fill(upTrackerTrack->phi());

	    h_muPlusPixelHits->Fill(*upTrackerPhits);
	    h_muUpperPlusPixelHits->Fill(*upTrackerPhits);
	    h_muPlusTrackerHits->Fill(*upTrackerThits);
	    h_muUpperPlusTrackerHits->Fill(*upTrackerThits);
	    h_muPlusValidHits->Fill(*upTrackerValidHits);
	    h_muUpperPlusValidHits->Fill(*upTrackerValidHits);
	    h_muPlusValidMuonHits->Fill(*upTrackerValidMuonHits);
	    h_muUpperPlusValidMuonHits->Fill(*upTrackerValidMuonHits);
	    h_muPlusMuonStationHits->Fill(*upTrackerMhits);
	    h_muUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	    h_muPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	    h_muUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	    h_muPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	    h_muUpperPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
	      h_muUpperPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muUpperPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	  
	      if (debug)
		std::cout << "Made it through the upper bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (up_n1pt) {
	      h_looseMuPlusChi2->Fill(*upTrackerChi2);
	      h_looseMuUpperPlusChi2->Fill(*upTrackerChi2);
	      h_looseMuPlusNdof->Fill(*upTrackerNdof);
	      h_looseMuUpperPlusNdof->Fill(*upTrackerNdof);
	      h_looseMuPlusCharge->Fill(*upTrackerCharge);
	      h_looseMuUpperPlusCharge->Fill(*upTrackerCharge);
	      h_looseMuPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      h_looseMuUpperPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuPlusDxy->Fill(*upTrackerDxy);
	      h_looseMuUpperPlusDxy->Fill(*upTrackerDxy);
	      h_looseMuPlusDz->Fill(*upTrackerDz);
	      h_looseMuUpperPlusDz->Fill(*upTrackerDz);
	      h_looseMuPlusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuUpperPlusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuPlusDzError->Fill(*upTrackerDzError);
	      h_looseMuUpperPlusDzError->Fill(*upTrackerDzError);

	      h_looseMuPlusPt->Fill(upTrackerMuonP4->pt());
	      h_looseMuUpperPlusPt->Fill(upTrackerMuonP4->pt());
	      h_looseMuPlusEta->Fill(upTrackerMuonP4->eta());
	      h_looseMuUpperPlusEta->Fill(upTrackerMuonP4->eta());
	      h_looseMuPlusPhi->Fill(upTrackerMuonP4->phi());
	      h_looseMuUpperPlusPhi->Fill(upTrackerMuonP4->phi());

	      h_looseMuPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      h_looseMuUpperPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      h_looseMuPlusTrackEta->Fill(upTrackerTrack->eta());
	      h_looseMuUpperPlusTrackEta->Fill(upTrackerTrack->eta());
	      h_looseMuPlusTrackPhi->Fill(upTrackerTrack->phi());
	      h_looseMuUpperPlusTrackPhi->Fill(upTrackerTrack->phi());

	      // remove tight info //if (up_tightdxy) {
	      // remove tight info //	h_tightMuPlusDz->Fill(*upTrackerDz);
	      // remove tight info //	h_tightMuUpperPlusDz->Fill(*upTrackerDz);
	      // remove tight info //	h_tightMuPlusDzError->Fill(*upTrackerDzError);
	      // remove tight info //	h_tightMuUpperPlusDzError->Fill(*upTrackerDzError);
	      // remove tight info //}
	      // remove tight info //if (up_tightdz) {
	      // remove tight info //	h_tightMuPlusDxy->Fill(*upTrackerDxy);
	      // remove tight info //	h_tightMuUpperPlusDxy->Fill(*upTrackerDxy);
	      // remove tight info //	h_tightMuPlusDxyError->Fill(*upTrackerDxyError);
	      // remove tight info //	h_tightMuUpperPlusDxyError->Fill(*upTrackerDxyError);
	      // remove tight info //}
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuPlusChi2->Fill(*upTrackerChi2);
	      // remove tight info //	h_tightMuUpperPlusChi2->Fill(*upTrackerChi2);
	      // remove tight info //	h_tightMuPlusNdof->Fill(*upTrackerNdof);
	      // remove tight info //	h_tightMuUpperPlusNdof->Fill(*upTrackerNdof);
	      // remove tight info //	h_tightMuPlusCharge->Fill(*upTrackerCharge);
	      // remove tight info //	h_tightMuUpperPlusCharge->Fill(*upTrackerCharge);
	      // remove tight info //	h_tightMuPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      // remove tight info //	h_tightMuUpperPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      // remove tight info //
	      // remove tight info //	h_tightMuPlusPt->Fill(upTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuUpperPlusPt->Fill(upTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuPlusEta->Fill(upTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuUpperPlusEta->Fill(upTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuPlusPhi->Fill(upTrackerMuonP4->phi());
	      // remove tight info //	h_tightMuUpperPlusPhi->Fill(upTrackerMuonP4->phi());
	      // remove tight info //
	      // remove tight info //	h_tightMuPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuUpperPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuPlusTrackEta->Fill(upTrackerTrack->eta());
	      // remove tight info //	h_tightMuUpperPlusTrackEta->Fill(upTrackerTrack->eta());
	      // remove tight info //	h_tightMuPlusTrackPhi->Fill(upTrackerTrack->phi());
	      // remove tight info //	h_tightMuUpperPlusTrackPhi->Fill(upTrackerTrack->phi());
	      // remove tight info //}

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
		h_looseMuUpperPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuUpperPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	    
		// remove tight info //if (up_tightdxy && up_tightdz) {
		// remove tight info //  h_tightMuPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
		// remove tight info //  h_tightMuUpperPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuUpperPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
		// remove tight info //}
		if (debug)
		  std::cout << "Made it through the upper bias loop " << i << std::endl; 
	      }
	      // end if (up_n1pt)
	    }
	
	    if (up_n1pixhits) {
	      h_looseMuPlusPixelHits->Fill(*upTrackerPhits);
	      h_looseMuUpperPlusPixelHits->Fill(*upTrackerPhits);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuPlusPixelHits->Fill(*upTrackerPhits);
	      // remove tight info //	h_tightMuUpperPlusPixelHits->Fill(*upTrackerPhits);
	      // remove tight info //}
	    }
	    if (up_n1vmuhits) {
	      h_looseMuPlusValidHits->Fill(*upTrackerValidHits);
	      h_looseMuUpperPlusValidHits->Fill(*upTrackerValidHits);
	      h_looseMuPlusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      h_looseMuUpperPlusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      h_looseMuPlusMuonStationHits->Fill(*upTrackerMhits);
	      h_looseMuUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuPlusValidHits->Fill(*upTrackerValidHits);
	      // remove tight info //	h_tightMuUpperPlusValidHits->Fill(*upTrackerValidHits);
	      // remove tight info //	h_tightMuPlusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      // remove tight info //	h_tightMuUpperPlusValidMuonHits->Fill(*upTrackerValidMuonHits);
	      // remove tight info //	h_tightMuPlusMuonStationHits->Fill(*upTrackerMhits);
	      // remove tight info //	h_tightMuUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	      // remove tight info //}
	    }
	    if (up_n1ptrelerr) {
	      h_looseMuPlusPtError->Fill(*upTrackerPtError);
	      h_looseMuUpperPlusPtError->Fill(*upTrackerPtError);
	      h_looseMuPlusPtRelErr->Fill(upperRelPtErr);
	      h_looseMuUpperPlusPtRelErr->Fill(upperRelPtErr);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuPlusPtError->Fill(*upTrackerPtError);
	      // remove tight info //	h_tightMuUpperPlusPtError->Fill(*upTrackerPtError);
	      // remove tight info //	h_tightMuPlusPtRelErr->Fill(upperRelPtErr);
	      // remove tight info //	h_tightMuUpperPlusPtRelErr->Fill(upperRelPtErr);
	      // remove tight info //}
	    }
	    if (up_n1trkhits) {
	      h_looseMuPlusTrackerHits->Fill(*upTrackerThits);
	      h_looseMuUpperPlusTrackerHits->Fill(*upTrackerThits);
	      h_looseMuPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      h_looseMuUpperPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuPlusTrackerHits->Fill(*upTrackerThits);
	      // remove tight info //	h_tightMuUpperPlusTrackerHits->Fill(*upTrackerThits);
	      // remove tight info //	h_tightMuPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      // remove tight info //	h_tightMuUpperPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	      // remove tight info //}
	    }
	    if (up_n1mmustahits) {
	      h_looseMuPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      h_looseMuUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      // remove tight info //if (up_tightdxy && up_tightdz) {
	      // remove tight info //	h_tightMuPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      // remove tight info //	h_tightMuUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	      // remove tight info //}
	    }
	  } // end else { // charge > 0

	  if (up_n1pt) {
	    if (sqrt(upTrackerTrack->perp2()) > 100) {
	      hasPt100Loose = true;
	      if (up_tightdxy && up_tightdz)
		hasPt100Tight = true;
	      if (sqrt(upTrackerTrack->perp2()) > 200) {
		hasPt200Loose = true;
		if (up_tightdxy && up_tightdz)
		  hasPt200Tight = true;
		if (sqrt(upTrackerTrack->perp2()) > 400) {
		  hasPt400Loose = true;
		  if (up_tightdxy && up_tightdz)
		    hasPt400Tight = true;
		}
	      }
	    }
	  } // end setting up bools for lumi print info
	} // end check on up_superpointing
	
	/* // commented out to include or of tracks
	   } // end if (sqrt(upTrackerTrack->perp2()) > minPt_)

	   //////// Lower muon leg ///////
	   if (sqrt(lowTrackerTrack->perp2()) > minPt_) {
	*/
	double lowerCpT = factor_*(*lowTrackerCharge)/(sqrt(lowTrackerTrack->perp2()));
	/*
	if (!symmetric_)
	  lowerCpT = factor_/(sqrt(lowTrackerTrack->perp2()));
	*/
	double lowerRelPtErr = *lowTrackerPtError/(sqrt(lowTrackerTrack->perp2()));

	// make bool's for each cut level?
	uint32_t lowerCuts; // 1 bit per cut
      
	bool low_tightdxy = (*lowTrackerDxy < 0.2) ? 1 : 0;
	bool low_tightdz  = (*lowTrackerDz  < 0.5) ? 1 : 0;
	bool low_superpointing = ((std::fabs(*lowTrackerDxy) < 10) && (std::fabs(*lowTrackerDz)  < 50))
	  ? 1 : 0;
	
	// if using TrackerOnly or TuneP and pT < 200, should *not* apply muon system cuts
	// bool lowerMuStationHits = (!istrackerp || (istunep && sqrt(lowTrackerTrack->perp2()) > 200)) ? *lowTrackerMatchedMuonStations > 1 : 1;
	bool lowerMuStationHits = *lowTrackerMatchedMuonStations > 1;

	bool lowerValidMuHits   = (!istrackerp || (istunep && sqrt(lowTrackerTrack->perp2()) > 200)) ? *lowTrackerValidMuonHits       > 0 : 1;

	bool low_n1dxymax      = (lowerValidMuHits         &&
				  lowerMuStationHits       &&
				  (lowerRelPtErr   < 0.3)  &&
				  (*lowTrackerPhits > 0  ) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool low_n1dzmax       = (lowerValidMuHits         &&
				  lowerMuStationHits       &&
				  (lowerRelPtErr   < 0.3)  &&
				  (*lowTrackerPhits > 0  ) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool low_n1ptrelerr    = (lowerValidMuHits       &&
				  lowerMuStationHits     &&
				  (*lowTrackerPhits > 0) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool low_n1pt          = (lowerValidMuHits         &&
				  lowerMuStationHits       &&
				  (lowerRelPtErr   < 0.3)  &&
				  (*lowTrackerPhits > 0  ) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool low_n1trkhits     = (lowerValidMuHits        &&
				  lowerMuStationHits      &&
				  (lowerRelPtErr   < 0.3) &&
				  (*lowTrackerPhits > 0 ))
	  ? 1 : 0;
	bool low_n1pixhits     = (lowerValidMuHits        &&
				 lowerMuStationHits       &&
				  (lowerRelPtErr   < 0.3) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool low_n1vmuhits     = (lowerMuStationHits       &&
				  (lowerRelPtErr   < 0.3)  &&
				  (*lowTrackerPhits > 0  ) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	bool low_n1mmustahits  = (lowerValidMuHits         &&
				  (lowerRelPtErr   < 0.3)  &&
				  (*lowTrackerPhits > 0  ) &&
				  (*lowTrackerLayersWithMeasurement > 5))
	  ? 1 : 0;
	
	if ((j % 100) == 0)
	  std::cout << "lower leg" << std::endl
		    << "mu pt  = " << std::setw(8) << std::setprecision(2) << std::fixed << lowTrackerMuonP4->pt()
		    << " - eta = " << std::setw(6) << std::setprecision(2) << std::fixed << lowTrackerMuonP4->eta()
		    << " - phi = " << std::setw(6) << std::setprecision(2) << std::fixed << lowTrackerMuonP4->phi()
		    << std::endl

		    << "trk pt = " << std::setw(8) << std::setprecision(2) << std::fixed << sqrt(lowTrackerTrack->perp2())
		    << " - eta = " << std::setw(6) << std::setprecision(2) << std::fixed << lowTrackerTrack->eta()
		    << " - phi = " << std::setw(6) << std::setprecision(2) << std::fixed << lowTrackerTrack->phi()
		    << std::endl;

	h_lowerChi2->Fill(*lowTrackerChi2);
	h_lowerNdof->Fill(*lowTrackerNdof);
	h_lowerCharge->Fill(*lowTrackerCharge);
	h_lowerCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));

	h_lowerDxy->Fill(*lowTrackerDxy);
	h_lowerDz->Fill(*lowTrackerDz);
	h_lowerDxyError->Fill(*lowTrackerDxyError);
	h_lowerDzError->Fill(*lowTrackerDzError);

	h_lowerPt->Fill(lowTrackerMuonP4->pt());
	h_lowerEta->Fill(lowTrackerMuonP4->eta());
	h_lowerPhi->Fill(lowTrackerMuonP4->phi());

	h_lowerPtError->Fill(*lowTrackerPtError);
	h_lowerPtRelErr->Fill(lowerRelPtErr);
	h_lowerTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	h_lowerTrackEta->Fill(lowTrackerTrack->eta());
	h_lowerTrackPhi->Fill(lowTrackerTrack->phi());

	lowerstring << "pt="      << sqrt(lowTrackerTrack->perp2())
		    << ",eta="    << lowTrackerTrack->eta()
		    << ",phi="    << lowTrackerTrack->phi()
		    << ",charge=" << *lowTrackerCharge;

	h_lowerPixelHits->Fill(*lowTrackerPhits);
	h_lowerTrackerHits->Fill(*lowTrackerThits);
	h_lowerValidHits->Fill(*lowTrackerValidHits);
	h_lowerValidMuonHits->Fill(*lowTrackerValidMuonHits);
	h_lowerMuonStationHits->Fill(*lowTrackerMhits);
	h_lowerMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	h_lowerTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

	for (int i = 0; i < nBiasBins; ++i) {
	  double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
	  double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
	  h_lowerCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	  h_lowerCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	  if (debug)
	    std::cout << "Made it through the lower bias loop " << i << std::endl; 
	}

	// fill the counters histogram for the lower leg muons passing the super-pointing selection
	if (low_superpointing || true) {
	  h_countersLower->Fill(0);

	  if (low_n1ptrelerr)
	    h_countersLower->Fill(1);
	  if (low_n1trkhits)
	    h_countersLower->Fill(2);
	  if (low_n1pixhits)
	    h_countersLower->Fill(3);
	  if (low_n1vmuhits)
	    h_countersLower->Fill(4);
	  if (low_n1mmustahits)
	    h_countersLower->Fill(5);
	  if (low_n1dxymax)
	    h_countersLower->Fill(6);
	  if (low_n1dzmax)
	    h_countersLower->Fill(7);
	  if (low_n1pt)
	    h_countersLower->Fill(8);

	  if (low_tightdxy) {
	    if (low_n1ptrelerr)
	      h_countersLower->Fill(11);
	    if (low_n1trkhits)
	      h_countersLower->Fill(12);
	    if (low_n1pixhits)
	      h_countersLower->Fill(13);
	    if (low_n1vmuhits)
	      h_countersLower->Fill(14);
	    if (low_n1mmustahits)
	      h_countersLower->Fill(15);
	    if (low_n1dxymax)
	      h_countersLower->Fill(16);
	    if (low_n1dzmax)
	      h_countersLower->Fill(17);
	    if (low_n1pt)
	      h_countersLower->Fill(18);
	  }

	  if (low_tightdz) {
	    if (low_n1ptrelerr)
	      h_countersLower->Fill(21);
	    if (low_n1trkhits)
	      h_countersLower->Fill(22);
	    if (low_n1pixhits)
	      h_countersLower->Fill(23);
	    if (low_n1vmuhits)
	      h_countersLower->Fill(24);
	    if (low_n1mmustahits)
	      h_countersLower->Fill(25);
	    if (low_n1dxymax)
	      h_countersLower->Fill(26);
	    if (low_n1dzmax)
	      h_countersLower->Fill(27);
	    if (low_n1pt)
	      h_countersLower->Fill(28);
	  }

	  if (low_tightdxy && low_tightdz) {
	    if (low_n1ptrelerr)
	      h_countersLower->Fill(31);
	    if (low_n1trkhits)
	      h_countersLower->Fill(32);
	    if (low_n1pixhits)
	      h_countersLower->Fill(33);
	    if (low_n1vmuhits)
	      h_countersLower->Fill(34);
	    if (low_n1mmustahits)
	      h_countersLower->Fill(35);
	    if (low_n1dxymax)
	      h_countersLower->Fill(36);
	    if (low_n1dzmax)
	      h_countersLower->Fill(37);
	    if (low_n1pt)
	      h_countersLower->Fill(38);
	  }

	  if (low_n1pt) {
	    // counters of passing the loose selection binned by pT
	    if (sqrt(lowTrackerTrack->perp2()) > 50)
	      h_countersLower->Fill(40);
	    if (sqrt(lowTrackerTrack->perp2()) > 100)
	      h_countersLower->Fill(41);
	    if (sqrt(lowTrackerTrack->perp2()) > 150)
	      h_countersLower->Fill(42);
	    if (sqrt(lowTrackerTrack->perp2()) > 200)
	      h_countersLower->Fill(43);
	    if (sqrt(lowTrackerTrack->perp2()) > 300)
	      h_countersLower->Fill(44);
	    if (sqrt(lowTrackerTrack->perp2()) > 400)
	      h_countersLower->Fill(45);
	    if (sqrt(lowTrackerTrack->perp2()) > 500)
	      h_countersLower->Fill(46);
	    if (sqrt(lowTrackerTrack->perp2()) > 1000)
	      h_countersLower->Fill(47);
	    if (sqrt(lowTrackerTrack->perp2()) > 1500)
	      h_countersLower->Fill(48);
	    if (sqrt(lowTrackerTrack->perp2()) > 2000)
	      h_countersLower->Fill(49);
	    if (sqrt(lowTrackerTrack->perp2()) > 3000)
	      h_countersLower->Fill(50);
	  }
	
	  if (*lowTrackerCharge < 0) {
	    h_muMinusChi2->Fill(*lowTrackerChi2);
	    h_muLowerMinusChi2->Fill(*lowTrackerChi2);
	    h_muMinusNdof->Fill(*lowTrackerNdof);
	    h_muLowerMinusNdof->Fill(*lowTrackerNdof);
	    h_muMinusCharge->Fill(*lowTrackerCharge);
	    h_muLowerMinusCharge->Fill(*lowTrackerCharge);
	    h_muMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	    h_muLowerMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));

	    h_muMinusDxy->Fill(*lowTrackerDxy);
	    h_muLowerMinusDxy->Fill(*lowTrackerDxy);
	    h_muMinusDz->Fill(*lowTrackerDz);
	    h_muLowerMinusDz->Fill(*lowTrackerDz);
	    h_muMinusDxyError->Fill(*lowTrackerDxyError);
	    h_muLowerMinusDxyError->Fill(*lowTrackerDxyError);
	    h_muMinusDzError->Fill(*lowTrackerDzError);
	    h_muLowerMinusDzError->Fill(*lowTrackerDzError);

	    h_muMinusPt->Fill(lowTrackerMuonP4->pt());
	    h_muLowerMinusPt->Fill(lowTrackerMuonP4->pt());
	    h_muMinusEta->Fill(lowTrackerMuonP4->eta());
	    h_muLowerMinusEta->Fill(lowTrackerMuonP4->eta());
	    h_muMinusPhi->Fill(lowTrackerMuonP4->phi());
	    h_muLowerMinusPhi->Fill(lowTrackerMuonP4->phi());

	    h_muMinusPtError->Fill(*lowTrackerPtError);
	    h_muLowerMinusPtError->Fill(*lowTrackerPtError);
	    h_muMinusPtRelErr->Fill(lowerRelPtErr);
	    h_muLowerMinusPtRelErr->Fill(lowerRelPtErr);
	    h_muMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	    h_muLowerMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	    h_muMinusTrackEta->Fill(lowTrackerTrack->eta());
	    h_muLowerMinusTrackEta->Fill(lowTrackerTrack->eta());
	    h_muMinusTrackPhi->Fill(lowTrackerTrack->phi());
	    h_muLowerMinusTrackPhi->Fill(lowTrackerTrack->phi());

	    h_muMinusPixelHits->Fill(*lowTrackerPhits);
	    h_muLowerMinusPixelHits->Fill(*lowTrackerPhits);
	    h_muMinusTrackerHits->Fill(*lowTrackerThits);
	    h_muLowerMinusTrackerHits->Fill(*lowTrackerThits);
	    h_muMinusValidHits->Fill(*lowTrackerValidHits);
	    h_muLowerMinusValidHits->Fill(*lowTrackerValidHits);
	    h_muMinusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	    h_muLowerMinusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	    h_muMinusMuonStationHits->Fill(*lowTrackerMhits);
	    h_muLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
	    h_muMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	    h_muLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	    h_muMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	    h_muLowerMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	      h_muLowerPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muLowerPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));

	      if (debug)
		std::cout << "Made it through the lower bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (low_n1pt) {
	      h_looseMuMinusChi2->Fill(*lowTrackerChi2);
	      h_looseMuLowerMinusChi2->Fill(*lowTrackerChi2);
	      h_looseMuMinusNdof->Fill(*lowTrackerNdof);
	      h_looseMuLowerMinusNdof->Fill(*lowTrackerNdof);
	      h_looseMuMinusCharge->Fill(*lowTrackerCharge);
	      h_looseMuLowerMinusCharge->Fill(*lowTrackerCharge);
	      h_looseMuMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      h_looseMuLowerMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuMinusDxy->Fill(*lowTrackerDxy);
	      h_looseMuLowerMinusDxy->Fill(*lowTrackerDxy);
	      h_looseMuMinusDz->Fill(*lowTrackerDz);
	      h_looseMuLowerMinusDz->Fill(*lowTrackerDz);
	      h_looseMuMinusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuLowerMinusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuMinusDzError->Fill(*lowTrackerDzError);
	      h_looseMuLowerMinusDzError->Fill(*lowTrackerDzError);

	      h_looseMuMinusPt->Fill(lowTrackerMuonP4->pt());
	      h_looseMuLowerMinusPt->Fill(lowTrackerMuonP4->pt());
	      h_looseMuMinusEta->Fill(lowTrackerMuonP4->eta());
	      h_looseMuLowerMinusEta->Fill(lowTrackerMuonP4->eta());
	      h_looseMuMinusPhi->Fill(lowTrackerMuonP4->phi());
	      h_looseMuLowerMinusPhi->Fill(lowTrackerMuonP4->phi());

	      h_looseMuMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      h_looseMuLowerMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      h_looseMuMinusTrackEta->Fill(lowTrackerTrack->eta());
	      h_looseMuLowerMinusTrackEta->Fill(lowTrackerTrack->eta());
	      h_looseMuMinusTrackPhi->Fill(lowTrackerTrack->phi());
	      h_looseMuLowerMinusTrackPhi->Fill(lowTrackerTrack->phi());

	      // remove tight info //if (low_tightdxy) {
	      // remove tight info //	h_tightMuMinusDz->Fill(*lowTrackerDz);
	      // remove tight info //	h_tightMuLowerMinusDz->Fill(*lowTrackerDz);
	      // remove tight info //	h_tightMuMinusDzError->Fill(*lowTrackerDzError);
	      // remove tight info //	h_tightMuLowerMinusDzError->Fill(*lowTrackerDzError);
	      // remove tight info //}
	      // remove tight info //if (low_tightdz) {
	      // remove tight info //	h_tightMuMinusDxy->Fill(*lowTrackerDxy);
	      // remove tight info //	h_tightMuLowerMinusDxy->Fill(*lowTrackerDxy);
	      // remove tight info //	h_tightMuMinusDxyError->Fill(*lowTrackerDxyError);
	      // remove tight info //	h_tightMuLowerMinusDxyError->Fill(*lowTrackerDxyError);
	      // remove tight info //}
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuMinusChi2->Fill(*lowTrackerChi2);
	      // remove tight info //	h_tightMuLowerMinusChi2->Fill(*lowTrackerChi2);
	      // remove tight info //	h_tightMuMinusNdof->Fill(*lowTrackerNdof);
	      // remove tight info //	h_tightMuLowerMinusNdof->Fill(*lowTrackerNdof);
	      // remove tight info //	h_tightMuMinusCharge->Fill(*lowTrackerCharge);
	      // remove tight info //	h_tightMuLowerMinusCharge->Fill(*lowTrackerCharge);
	      // remove tight info //	h_tightMuMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      // remove tight info //	h_tightMuLowerMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      // remove tight info //
	      // remove tight info //	h_tightMuMinusPt->Fill(lowTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuLowerMinusPt->Fill(lowTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuMinusEta->Fill(lowTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuLowerMinusEta->Fill(lowTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuMinusPhi->Fill(lowTrackerMuonP4->phi());
	      // remove tight info //	h_tightMuLowerMinusPhi->Fill(lowTrackerMuonP4->phi());
	      // remove tight info //
	      // remove tight info //	h_tightMuMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuLowerMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuMinusTrackEta->Fill(lowTrackerTrack->eta());
	      // remove tight info //	h_tightMuLowerMinusTrackEta->Fill(lowTrackerTrack->eta());
	      // remove tight info //	h_tightMuMinusTrackPhi->Fill(lowTrackerTrack->phi());
	      // remove tight info //	h_tightMuLowerMinusTrackPhi->Fill(lowTrackerTrack->phi());
	      // remove tight info //}

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		h_looseMuLowerMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuLowerMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	

		// remove tight info //if (low_tightdxy && low_tightdz) {
		// remove tight info //  h_tightMuMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //  h_tightMuLowerMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuLowerMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //}
		if (debug)
		  std::cout << "Made it through the lower bias loop " << i << std::endl; 
	      }
	    } // end if (low_n1pt)

	    if (low_n1pixhits) {
	      h_looseMuMinusPixelHits->Fill(*lowTrackerPhits);
	      h_looseMuLowerMinusPixelHits->Fill(*lowTrackerPhits);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuMinusPixelHits->Fill(*lowTrackerPhits);
	      // remove tight info //	h_tightMuLowerMinusPixelHits->Fill(*lowTrackerPhits);
	      // remove tight info //}
	    }
	    if (low_n1vmuhits) {
	      h_looseMuMinusValidHits->Fill(*lowTrackerValidHits);
	      h_looseMuLowerMinusValidHits->Fill(*lowTrackerValidHits);
	      h_looseMuMinusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      h_looseMuLowerMinusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      h_looseMuMinusMuonStationHits->Fill(*lowTrackerMhits);
	      h_looseMuLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuMinusValidHits->Fill(*lowTrackerValidHits);
	      // remove tight info //	h_tightMuLowerMinusValidHits->Fill(*lowTrackerValidHits);
	      // remove tight info //	h_tightMuMinusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //	h_tightMuLowerMinusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //	h_tightMuMinusMuonStationHits->Fill(*lowTrackerMhits);
	      // remove tight info //	h_tightMuLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
	      // remove tight info //}
	    }
	    if (low_n1ptrelerr) {
	      h_looseMuMinusPtError->Fill(*lowTrackerPtError);
	      h_looseMuLowerMinusPtError->Fill(*lowTrackerPtError);
	      h_looseMuMinusPtRelErr->Fill(lowerRelPtErr);
	      h_looseMuLowerMinusPtRelErr->Fill(lowerRelPtErr);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuMinusPtError->Fill(*lowTrackerPtError);
	      // remove tight info //	h_tightMuLowerMinusPtError->Fill(*lowTrackerPtError);
	      // remove tight info //	h_tightMuMinusPtRelErr->Fill(lowerRelPtErr);
	      // remove tight info //	h_tightMuLowerMinusPtRelErr->Fill(lowerRelPtErr);
	      // remove tight info //}
	    }
	    if (low_n1trkhits) {
	      h_looseMuMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      h_looseMuLowerMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      h_looseMuMinusTrackerHits->Fill(*lowTrackerThits);
	      h_looseMuLowerMinusTrackerHits->Fill(*lowTrackerThits);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      // remove tight info //	h_tightMuLowerMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      // remove tight info //	h_tightMuMinusTrackerHits->Fill(*lowTrackerThits);
	      // remove tight info //	h_tightMuLowerMinusTrackerHits->Fill(*lowTrackerThits);
	      // remove tight info //}
	    }
	    if (low_n1mmustahits) {
	      h_looseMuMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      h_looseMuLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      // remove tight info //	h_tightMuLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      // remove tight info //}
	    }
	  } // end if charge < 0

	  else { // charge > 0
	    h_muPlusChi2->Fill(*lowTrackerChi2);
	    h_muLowerPlusChi2->Fill(*lowTrackerChi2);
	    h_muPlusNdof->Fill(*lowTrackerNdof);
	    h_muLowerPlusNdof->Fill(*lowTrackerNdof);
	    h_muPlusCharge->Fill(*lowTrackerCharge);
	    h_muLowerPlusCharge->Fill(*lowTrackerCharge);
	    h_muPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	    h_muLowerPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));

	    h_muPlusDxy->Fill(*lowTrackerDxy);
	    h_muLowerPlusDxy->Fill(*lowTrackerDxy);
	    h_muPlusDz->Fill(*lowTrackerDz);
	    h_muLowerPlusDz->Fill(*lowTrackerDz);
	    h_muPlusDxyError->Fill(*lowTrackerDxyError);
	    h_muLowerPlusDxyError->Fill(*lowTrackerDxyError);
	    h_muPlusDzError->Fill(*lowTrackerDzError);
	    h_muLowerPlusDzError->Fill(*lowTrackerDzError);

	    h_muPlusPt->Fill(lowTrackerMuonP4->pt());
	    h_muLowerPlusPt->Fill(lowTrackerMuonP4->pt());
	    h_muPlusEta->Fill(lowTrackerMuonP4->eta());
	    h_muLowerPlusEta->Fill(lowTrackerMuonP4->eta());
	    h_muPlusPhi->Fill(lowTrackerMuonP4->phi());
	    h_muLowerPlusPhi->Fill(lowTrackerMuonP4->phi());

	    h_muPlusPtError->Fill(*lowTrackerPtError);
	    h_muLowerPlusPtError->Fill(*lowTrackerPtError);
	    h_muPlusPtRelErr->Fill(lowerRelPtErr);
	    h_muLowerPlusPtRelErr->Fill(lowerRelPtErr);
	    h_muPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	    h_muLowerPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	    h_muPlusTrackEta->Fill(lowTrackerTrack->eta());
	    h_muLowerPlusTrackEta->Fill(lowTrackerTrack->eta());
	    h_muPlusTrackPhi->Fill(lowTrackerTrack->phi());
	    h_muLowerPlusTrackPhi->Fill(lowTrackerTrack->phi());

	    h_muPlusPixelHits->Fill(*lowTrackerPhits);
	    h_muLowerPlusPixelHits->Fill(*lowTrackerPhits);
	    h_muPlusTrackerHits->Fill(*lowTrackerThits);
	    h_muLowerPlusTrackerHits->Fill(*lowTrackerThits);
	    h_muPlusValidHits->Fill(*lowTrackerValidHits);
	    h_muLowerPlusValidHits->Fill(*lowTrackerValidHits);
	    h_muPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	    h_muLowerPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	    h_muPlusMuonStationHits->Fill(*lowTrackerMhits);
	    h_muLowerPlusMuonStationHits->Fill(*lowTrackerMhits);
	    h_muPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	    h_muLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	    h_muPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	    h_muLowerPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
	      h_muLowerPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      h_muLowerPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	

	      if (debug)
		std::cout << "Made it through the lower bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (low_n1pt) {
	      h_looseMuPlusChi2->Fill(*lowTrackerChi2);
	      h_looseMuLowerPlusChi2->Fill(*lowTrackerChi2);
	      h_looseMuPlusNdof->Fill(*lowTrackerNdof);
	      h_looseMuLowerPlusNdof->Fill(*lowTrackerNdof);
	      h_looseMuPlusCharge->Fill(*lowTrackerCharge);
	      h_looseMuLowerPlusCharge->Fill(*lowTrackerCharge);
	      h_looseMuPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      h_looseMuLowerPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuPlusDxy->Fill(*lowTrackerDxy);
	      h_looseMuLowerPlusDxy->Fill(*lowTrackerDxy);
	      h_looseMuPlusDz->Fill(*lowTrackerDz);
	      h_looseMuLowerPlusDz->Fill(*lowTrackerDz);
	      h_looseMuPlusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuLowerPlusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuPlusDzError->Fill(*lowTrackerDzError);
	      h_looseMuLowerPlusDzError->Fill(*lowTrackerDzError);

	      h_looseMuPlusPt->Fill(lowTrackerMuonP4->pt());
	      h_looseMuLowerPlusPt->Fill(lowTrackerMuonP4->pt());
	      h_looseMuPlusEta->Fill(lowTrackerMuonP4->eta());
	      h_looseMuLowerPlusEta->Fill(lowTrackerMuonP4->eta());
	      h_looseMuPlusPhi->Fill(lowTrackerMuonP4->phi());
	      h_looseMuLowerPlusPhi->Fill(lowTrackerMuonP4->phi());

	      h_looseMuPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      h_looseMuLowerPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      h_looseMuPlusTrackEta->Fill(lowTrackerTrack->eta());
	      h_looseMuLowerPlusTrackEta->Fill(lowTrackerTrack->eta());
	      h_looseMuPlusTrackPhi->Fill(lowTrackerTrack->phi());
	      h_looseMuLowerPlusTrackPhi->Fill(lowTrackerTrack->phi());

	      h_looseMuPlusTrackerHits->Fill(*lowTrackerThits);
	      h_looseMuLowerPlusTrackerHits->Fill(*lowTrackerThits);
	      h_looseMuPlusValidHits->Fill(*lowTrackerValidHits);
	      h_looseMuLowerPlusValidHits->Fill(*lowTrackerValidHits);
	      h_looseMuPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      h_looseMuLowerPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      h_looseMuPlusMuonStationHits->Fill(*lowTrackerMhits);
	      h_looseMuLowerPlusMuonStationHits->Fill(*lowTrackerMhits);

	      // remove tight info //if (low_tightdxy) {
	      // remove tight info //	h_tightMuPlusDz->Fill(*lowTrackerDz);
	      // remove tight info //	h_tightMuLowerPlusDz->Fill(*lowTrackerDz);
	      // remove tight info //	h_tightMuPlusDzError->Fill(*lowTrackerDzError);
	      // remove tight info //	h_tightMuLowerPlusDzError->Fill(*lowTrackerDzError);
	      // remove tight info //}
	      // remove tight info //if (low_tightdz) {
	      // remove tight info //	h_tightMuPlusDxy->Fill(*lowTrackerDxy);
	      // remove tight info //	h_tightMuLowerPlusDxy->Fill(*lowTrackerDxy);
	      // remove tight info //	h_tightMuPlusDxyError->Fill(*lowTrackerDxyError);
	      // remove tight info //	h_tightMuLowerPlusDxyError->Fill(*lowTrackerDxyError);
	      // remove tight info //}
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuPlusChi2->Fill(*lowTrackerChi2);
	      // remove tight info //	h_tightMuLowerPlusChi2->Fill(*lowTrackerChi2);
	      // remove tight info //	h_tightMuPlusNdof->Fill(*lowTrackerNdof);
	      // remove tight info //	h_tightMuLowerPlusNdof->Fill(*lowTrackerNdof);
	      // remove tight info //	h_tightMuPlusCharge->Fill(*lowTrackerCharge);
	      // remove tight info //	h_tightMuLowerPlusCharge->Fill(*lowTrackerCharge);
	      // remove tight info //	h_tightMuPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      // remove tight info //	h_tightMuLowerPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      // remove tight info //
	      // remove tight info //	h_tightMuPlusPt->Fill(lowTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuLowerPlusPt->Fill(lowTrackerMuonP4->pt());
	      // remove tight info //	h_tightMuPlusEta->Fill(lowTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuLowerPlusEta->Fill(lowTrackerMuonP4->eta());
	      // remove tight info //	h_tightMuPlusPhi->Fill(lowTrackerMuonP4->phi());
	      // remove tight info //	h_tightMuLowerPlusPhi->Fill(lowTrackerMuonP4->phi());
	      // remove tight info //
	      // remove tight info //	h_tightMuPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuLowerPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      // remove tight info //	h_tightMuPlusTrackEta->Fill(lowTrackerTrack->eta());
	      // remove tight info //	h_tightMuLowerPlusTrackEta->Fill(lowTrackerTrack->eta());
	      // remove tight info //	h_tightMuPlusTrackPhi->Fill(lowTrackerTrack->phi());
	      // remove tight info //	h_tightMuLowerPlusTrackPhi->Fill(lowTrackerTrack->phi());
	      // remove tight info //
	      // remove tight info //	h_tightMuPlusTrackerHits->Fill(*lowTrackerThits);
	      // remove tight info //	h_tightMuLowerPlusTrackerHits->Fill(*lowTrackerThits);
	      // remove tight info //	h_tightMuPlusValidHits->Fill(*lowTrackerValidHits);
	      // remove tight info //	h_tightMuLowerPlusValidHits->Fill(*lowTrackerValidHits);
	      // remove tight info //	h_tightMuPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //	h_tightMuLowerPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //	h_tightMuPlusMuonStationHits->Fill(*lowTrackerMhits);
	      // remove tight info //	h_tightMuLowerPlusMuonStationHits->Fill(*lowTrackerMhits);
	      // remove tight info //}

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		h_looseMuLowerPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		h_looseMuLowerPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	

		// remove tight info //if (low_tightdxy && low_tightdz) {
		// remove tight info //  h_tightMuPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //  h_tightMuLowerPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		// remove tight info //  h_tightMuLowerPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		// remove tight info //}
		if (debug)
		  std::cout << "Made it through the lower bias loop " << i << std::endl; 
	      }
	    }

	    if (low_n1pixhits) {
	      h_looseMuPlusPixelHits->Fill(*lowTrackerPhits);
	      h_looseMuLowerPlusPixelHits->Fill(*lowTrackerPhits);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuPlusPixelHits->Fill(*lowTrackerPhits);
	      // remove tight info //	h_tightMuLowerPlusPixelHits->Fill(*lowTrackerPhits);
	      // remove tight info //}
	    }
	    if (low_n1vmuhits) {
	      h_looseMuPlusValidHits->Fill(*lowTrackerValidHits);
	      h_looseMuLowerPlusValidHits->Fill(*lowTrackerValidHits);
	      h_looseMuPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      h_looseMuLowerPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuPlusValidHits->Fill(*lowTrackerValidHits);
	      // remove tight info //	h_tightMuLowerPlusValidHits->Fill(*lowTrackerValidHits);
	      // remove tight info //	h_tightMuPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //	h_tightMuLowerPlusValidMuonHits->Fill(*lowTrackerValidMuonHits);
	      // remove tight info //}
	    }
	    if (low_n1ptrelerr) {
	      h_looseMuPlusPtError->Fill(*lowTrackerPtError);
	      h_looseMuLowerPlusPtError->Fill(*lowTrackerPtError);
	      h_looseMuPlusPtRelErr->Fill(lowerRelPtErr);
	      h_looseMuLowerPlusPtRelErr->Fill(lowerRelPtErr);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuPlusPtError->Fill(*lowTrackerPtError);
	      // remove tight info //	h_tightMuLowerPlusPtError->Fill(*lowTrackerPtError);
	      // remove tight info //	h_tightMuPlusPtRelErr->Fill(lowerRelPtErr);
	      // remove tight info //	h_tightMuLowerPlusPtRelErr->Fill(lowerRelPtErr);
	      // remove tight info //}
	    }
	    if (low_n1trkhits) {
	      h_looseMuPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      h_looseMuLowerPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      // remove tight info //	h_tightMuLowerPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	      // remove tight info //}
	    }
	    if (low_n1mmustahits) {
	      h_looseMuPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      h_looseMuLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      // remove tight info //if (low_tightdxy && low_tightdz) {
	      // remove tight info //	h_tightMuPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      // remove tight info //	h_tightMuLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	      // remove tight info //}
	    }
	  } // end else { // charge > 0
	  if (low_n1pt) {
	    if (sqrt(lowTrackerTrack->perp2()) > 100) {
	      hasPt100Loose = true;
	      if (low_tightdxy && low_tightdz)
		hasPt100Tight = true;
	      if (sqrt(lowTrackerTrack->perp2()) > 200) {
		hasPt200Loose = true;
		if (low_tightdxy && low_tightdz)
		  hasPt200Tight = true;
		if (sqrt(lowTrackerTrack->perp2()) > 400) {
		  hasPt400Loose = true;
		  if (low_tightdxy && low_tightdz)
		    hasPt400Tight = true;
		}
	      }
	    }
	  } // end setting up bools for lumi print info
	} // end check on low_superpointing
	//} // end if (sqrt(lowTrackerTrack->perp2()) > minPt_)
      } // end if (sqrt(upTrackerTrack->perp2()) > minPt_ || sqrt(lowTrackerTrack->perp2()) > minPt_)
      // end of the loop
      j++;
      if (debug)
	std::cout << "Made it through " << j << " sets of fills" << std::endl;	

      if (hasPt100Loose)
	lumiFileOut100_loose
	  << "\"" << *run << "\":"
	  << " [[" << *lumi << "," << *lumi << "]]"
	  << " : " << *event
	  << " (upper leg) " << upperstring.str()
	  << " (lower leg) " << lowerstring.str()
	  << std::endl;
      
      if (hasPt100Tight)
	lumiFileOut100_tight
	  << "\""  << *run  << "\":"
	  << " [[" << *lumi << "," << *lumi << "]]"
	  << " : " << *event
	  << " (upper leg) " << upperstring.str()
	  << " (lower leg) " << lowerstring.str()
	  << std::endl;
      
      if (hasPt200Loose)
	lumiFileOut200_loose
	  << "\"" << *run << "\":"
	  << " [[" << *lumi << "," << *lumi << "]]"
	  << " : " << *event
	  << " (upper leg) " << upperstring.str()
	  << " (lower leg) " << lowerstring.str()
	  << std::endl;
      
      if (hasPt200Tight)
	lumiFileOut200_tight
	  << "\""  << *run  << "\":"
	  << " [[" << *lumi << "," << *lumi << "]]"
	  << " : " << *event
	  << " (upper leg) " << upperstring.str()
	  << " (lower leg) " << lowerstring.str()
	  << std::endl;
      
      if (hasPt400Loose)
	lumiFileOut400_loose
	  << "\"" << *run << "\":"
	  << " [[" << *lumi << "," << *lumi << "]]"
	  << " : " << *event
	  << " (upper leg) " << upperstring.str()
	  << " (lower leg) " << lowerstring.str()
	  << std::endl;
      
      if (hasPt400Tight)
	lumiFileOut400_tight
	  << "\""  << *run  << "\":"
	  << " [[" << *lumi << "," << *lumi << "]]"
	  << " : " << *event
	  << " (upper leg) " << upperstring.str()
	  << " (lower leg) " << lowerstring.str()
	  << std::endl;
    } // closing if (*upTrackerChi2 > -1)
  } // end while loop
  
  lumiFileOut100_loose.close();
  lumiFileOut200_loose.close();
  lumiFileOut400_loose.close();
  
  lumiFileOut100_tight.close();
  lumiFileOut200_tight.close();
  lumiFileOut400_tight.close();

  std::cout << std::hex << g << std::dec << std::endl;
  
  g->Write();
  g->Close();
  
  return;
}
