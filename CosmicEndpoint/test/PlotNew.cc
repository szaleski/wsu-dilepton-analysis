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

void PlotNew(std::string const& filelist, std::string const& outFile,
	  int trackVal_, double minPt_, double maxBias_, int nBiasBins_,
	  double factor_=1.0, double lowpT_=-1.0, double highpT_=-1.0,
	  bool symmetric_=false, bool applyTrigger_=false, bool mcFlag_=false,
	  bool debug_=false)

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
  
  // turn on Sumw2 by default
  TH1::SetDefaultSumw2();
  
  TH1I *h_countersUpper = new TH1I("upperCounters","upperCounters",55, -0.5, 54.5);
  TH1I *h_countersLower = new TH1I("lowerCounters","lowerCounters",55, -0.5, 54.5);
  
  for (int b = 0; b < 55; ++b) {
    h_countersUpper->GetXaxis()->SetBinLabel(b+1,TString(counterBinLabels[b]));
    h_countersLower->GetXaxis()->SetBinLabel(b+1,TString(counterBinLabels[b]));
  }

  // histograms for upper leg muons, inclusive
  TH1F *h_upperPt       = new TH1F("upperPt",    "upperPt",    300,  0., 3000.);
  TH1F *h_upperPtEtaPlus      = new TH1F("upperPtEtaPlus", "upperPtEtaPlus", 300, 0., 3000.);
  TH1F *h_upperPtEtaMinus      = new TH1F("upperPtEtaMinus", "upperPtEtaMinus", 300, 0., 3000.);
  TH1F *h_upperPtEtaPlusPhiMinus      = new TH1F("upperPtEtaPlusPhiMinus", "upperPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_upperPtEtaPlusPhiZero      = new TH1F("upperPtEtaPlusPhiZero", "upperPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_upperPtEtaPlusPhiPlus      = new TH1F("upperPtEtaPlusPhiPlus", "upperPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_upperPtEtaMinusPhiMinus      = new TH1F("upperPtEtaMinusPhiMinus", "upperPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_upperPtEtaMinusPhiZero      = new TH1F("upperPtEtaMinusPhiZero", "upperPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_upperPtEtaMinusPhiPlus      = new TH1F("upperPtEtaMinusPhiPlus", "upperPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_upperEta      = new TH1F("upperEta",   "upperEta",    40, -2.,    2.);
  TH1F *h_upperEtaEtaPlus      = new TH1F("upperEtaEtaPlus", "upperEtaEtaPlus", 40, -2., 2.);
  TH1F *h_upperEtaEtaMinus      = new TH1F("upperEtaEtaMinus", "upperEtaEtaMinus", 40, -2., 2.);
  TH1F *h_upperEtaEtaPlusPhiMinus      = new TH1F("upperEtaEtaPlusPhiMinus", "upperEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_upperEtaEtaPlusPhiZero      = new TH1F("upperEtaEtaPlusPhiZero", "upperEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_upperEtaEtaPlusPhiPlus      = new TH1F("upperEtaEtaPlusPhiPlus", "upperEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_upperEtaEtaMinusPhiMinus      = new TH1F("upperEtaEtaMinusPhiMinus", "upperEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_upperEtaEtaMinusPhiZero      = new TH1F("upperEtaEtaMinusPhiZero", "upperEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_upperEtaEtaMinusPhiPlus      = new TH1F("upperEtaEtaMinusPhiPlus", "upperEtaEtaMinusPhiPlus", 40, -2., 2.);


  TH1F *h_upperPhi      = new TH1F("upperPhi",   "upperPhi",    40, -4.,    4.);
  TH1F *h_upperPhiEtaPlus      = new TH1F("upperPhiEtaPlus", "upperPhiEtaPlus", 40, -4., 4.);
  TH1F *h_upperPhiEtaMinus      = new TH1F("upperPhiEtaMinus", "upperPhiEtaMinus", 40, -4., 4.);
  TH1F *h_upperPhiEtaPlusPhiMinus      = new TH1F("upperPhiEtaPlusPhiMinus", "upperPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_upperPhiEtaPlusPhiZero      = new TH1F("upperPhiEtaPlusPhiZero", "upperPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_upperPhiEtaPlusPhiPlus      = new TH1F("upperPhiEtaPlusPhiPlus", "upperPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_upperPhiEtaMinusPhiMinus      = new TH1F("upperPhiEtaMinusPhiMinus", "upperPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_upperPhiEtaMinusPhiZero      = new TH1F("upperPhiEtaMinusPhiZero", "upperPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_upperPhiEtaMinusPhiPlus      = new TH1F("upperPhiEtaMinusPhiPlus", "upperPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_upperChi2     = new TH1F("upperChi2",  "upperChi2",   50,  0.,  150.);
  TH1F *h_upperChi2EtaPlus      = new TH1F("upperChi2EtaPlus", "upperChi2EtaPlus", 50, 0., 150.);
  TH1F *h_upperChi2EtaMinus      = new TH1F("upperChi2EtaMinus", "upperChi2EtaMinus", 50, 0., 150.);
  TH1F *h_upperChi2EtaPlusPhiMinus      = new TH1F("upperChi2EtaPlusPhiMinus", "upperChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_upperChi2EtaPlusPhiZero      = new TH1F("upperChi2EtaPlusPhiZero", "upperChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_upperChi2EtaPlusPhiPlus      = new TH1F("upperChi2EtaPlusPhiPlus", "upperChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_upperChi2EtaMinusPhiMinus      = new TH1F("upperChi2EtaMinusPhiMinus", "upperChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_upperChi2EtaMinusPhiZero      = new TH1F("upperChi2EtaMinusPhiZero", "upperChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_upperChi2EtaMinusPhiPlus      = new TH1F("upperChi2EtaMinusPhiPlus", "upperChi2EtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_upperNdof     = new TH1F("upperNdof",  "upperNdof",   100, -0.5, 99.5);
  TH1F *h_upperNdofEtaPlus      = new TH1F("upperNdofEtaPlus", "upperNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaMinus      = new TH1F("upperNdofEtaMinus", "upperNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaPlusPhiMinus      = new TH1F("upperNdofEtaPlusPhiMinus", "upperNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaPlusPhiZero      = new TH1F("upperNdofEtaPlusPhiZero", "upperNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaPlusPhiPlus      = new TH1F("upperNdofEtaPlusPhiPlus", "upperNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaMinusPhiMinus      = new TH1F("upperNdofEtaMinusPhiMinus", "upperNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaMinusPhiZero      = new TH1F("upperNdofEtaMinusPhiZero", "upperNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_upperNdofEtaMinusPhiPlus      = new TH1F("upperNdofEtaMinusPhiPlus", "upperNdofEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_upperCharge   = new TH1F("upperCharge","upperCharge", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaPlus      = new TH1F("upperChargeEtaPlus", "upperChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaMinus      = new TH1F("upperChargeEtaMinus", "upperChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaPlusPhiMinus      = new TH1F("upperChargeEtaPlusPhiMinus", "upperChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaPlusPhiZero      = new TH1F("upperChargeEtaPlusPhiZero", "upperChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaPlusPhiPlus      = new TH1F("upperChargeEtaPlusPhiPlus", "upperChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaMinusPhiMinus      = new TH1F("upperChargeEtaMinusPhiMinus", "upperChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaMinusPhiZero      = new TH1F("upperChargeEtaMinusPhiZero", "upperChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_upperChargeEtaMinusPhiPlus      = new TH1F("upperChargeEtaMinusPhiPlus", "upperChargeEtaMinusPhiPlus", 3, -1.5, 1.5);


  // default asymmetric binning -8.0/TeV to 8.0/TeV = 16/TeV, 16/1600 = 0.01/TeV
  // suggested for KS is 0.01/TeV
  // suggested for chi2 is 0.25/TeV to be around expected resolution, means rebinning 25 0.01 bins into one
  // symmetric binning 0/TeV to 8.0/TeV = 8.0/TeV, 8.0/800 = 0.01/TeV
  // suggested for KS is 0.01/TeV
  // suggested for chi2 is 0.25/TeV to be around expected resolution, means rebinning 25 0.01 bins into one
  // should *never* have a bin that straddles 0, 0 should *always* be a bin boundary
  TH1F *h_upperCurve    = new TH1F("upperCurve", "upperCurve",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);

  TH1F *h_upperCurveEtaPlus    = new TH1F("upperCurveEtaPlus", "upperCurveEtaPlus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaMinus    = new TH1F("upperCurveEtaMinus", "upperCurveEtaMinus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaPlusPhiMinus    = new TH1F("upperCurveEtaPlusPhiMinus", "upperCurveEtaPlusPhiMinus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaPlusPhiZero    = new TH1F("upperCurveEtaPlusPhiZero", "upperCurveEtaPlusPhiZero",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaPlusPhiPlus    = new TH1F("upperCurveEtaPlusPhiPlus", "upperCurveEtaPlusPhiPlus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaMinusPhiMinus    = new TH1F("upperCurveEtaMinusPhiMinus", "upperCurveEtaMinusPhiMinus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaMinusPhiZero    = new TH1F("upperCurveEtaMinusPhiZero", "upperCurveEtaMinusPhiZero",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaMinusPhiPlus    = new TH1F("upperCurveEtaMinusPhiPlus", "upperCurveEtaMinusPhiPlus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaAllPhiMinus    = new TH1F("upperCurveEtaAllPhiMinus", "upperCurveEtaAllPhiMinus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaAllPhiZero    = new TH1F("upperCurveEtaAllPhiZero", "upperCurveEtaAllPhiZero",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_upperCurveEtaAllPhiPlus    = new TH1F("upperCurveEtaAllPhiPlus", "upperCurveEtaAllPhiPlus",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);


  TH1F *h_upperDxy      = new TH1F("upperDxy",   "upperDxy",    100, -100., 100.);
  TH1F *h_upperDxyEtaPlus      = new TH1F("upperDxyEtaPlus", "upperDxyEtaPlus", 100, -100., 100.);
  TH1F *h_upperDxyEtaMinus      = new TH1F("upperDxyEtaMinus", "upperDxyEtaMinus", 100, -100., 100.);
  TH1F *h_upperDxyEtaPlusPhiMinus      = new TH1F("upperDxyEtaPlusPhiMinus", "upperDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_upperDxyEtaPlusPhiZero      = new TH1F("upperDxyEtaPlusPhiZero", "upperDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_upperDxyEtaPlusPhiPlus      = new TH1F("upperDxyEtaPlusPhiPlus", "upperDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_upperDxyEtaMinusPhiMinus      = new TH1F("upperDxyEtaMinusPhiMinus", "upperDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_upperDxyEtaMinusPhiZero      = new TH1F("upperDxyEtaMinusPhiZero", "upperDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_upperDxyEtaMinusPhiPlus      = new TH1F("upperDxyEtaMinusPhiPlus", "upperDxyEtaMinusPhiPlus", 100, -100., 100.);


  TH1F *h_upperDz       = new TH1F("upperDz",    "upperDz",     100, -250., 250.);
  TH1F *h_upperDzEtaPlus      = new TH1F("upperDzEtaPlus", "upperDzEtaPlus", 100, -250., 250.);
  TH1F *h_upperDzEtaMinus      = new TH1F("upperDzEtaMinus", "upperDzEtaMinus", 100, -250., 250.);
  TH1F *h_upperDzEtaPlusPhiMinus      = new TH1F("upperDzEtaPlusPhiMinus", "upperDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_upperDzEtaPlusPhiZero      = new TH1F("upperDzEtaPlusPhiZero", "upperDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_upperDzEtaPlusPhiPlus      = new TH1F("upperDzEtaPlusPhiPlus", "upperDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_upperDzEtaMinusPhiMinus      = new TH1F("upperDzEtaMinusPhiMinus", "upperDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_upperDzEtaMinusPhiZero      = new TH1F("upperDzEtaMinusPhiZero", "upperDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_upperDzEtaMinusPhiPlus      = new TH1F("upperDzEtaMinusPhiPlus", "upperDzEtaMinusPhiPlus", 100, -250., 250.);


  TH1F *h_upperDxyError = new TH1F("upperDxyError","upperDxyError", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaPlus      = new TH1F("upperDxyErrorEtaPlus", "upperDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaMinus      = new TH1F("upperDxyErrorEtaMinus", "upperDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaPlusPhiMinus      = new TH1F("upperDxyErrorEtaPlusPhiMinus", "upperDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaPlusPhiZero      = new TH1F("upperDxyErrorEtaPlusPhiZero", "upperDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaPlusPhiPlus      = new TH1F("upperDxyErrorEtaPlusPhiPlus", "upperDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaMinusPhiMinus      = new TH1F("upperDxyErrorEtaMinusPhiMinus", "upperDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaMinusPhiZero      = new TH1F("upperDxyErrorEtaMinusPhiZero", "upperDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_upperDxyErrorEtaMinusPhiPlus      = new TH1F("upperDxyErrorEtaMinusPhiPlus", "upperDxyErrorEtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_upperDzError  = new TH1F("upperDzError", "upperDzError",  50, 0., 150.);
  TH1F *h_upperDzErrorEtaPlus      = new TH1F("upperDzErrorEtaPlus", "upperDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaMinus      = new TH1F("upperDzErrorEtaMinus", "upperDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaPlusPhiMinus      = new TH1F("upperDzErrorEtaPlusPhiMinus", "upperDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaPlusPhiZero      = new TH1F("upperDzErrorEtaPlusPhiZero", "upperDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaPlusPhiPlus      = new TH1F("upperDzErrorEtaPlusPhiPlus", "upperDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaMinusPhiMinus      = new TH1F("upperDzErrorEtaMinusPhiMinus", "upperDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaMinusPhiZero      = new TH1F("upperDzErrorEtaMinusPhiZero", "upperDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_upperDzErrorEtaMinusPhiPlus      = new TH1F("upperDzErrorEtaMinusPhiPlus", "upperDzErrorEtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_upperTrackPt  = new TH1F("upperTrackPt", "upperTrackPt",  300, 0., 3000.);
  TH1F *h_upperTrackPtEtaPlus      = new TH1F("upperTrackPtEtaPlus", "upperTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaMinus      = new TH1F("upperTrackPtEtaMinus", "upperTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaPlusPhiMinus      = new TH1F("upperTrackPtEtaPlusPhiMinus", "upperTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaPlusPhiZero      = new TH1F("upperTrackPtEtaPlusPhiZero", "upperTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaPlusPhiPlus      = new TH1F("upperTrackPtEtaPlusPhiPlus", "upperTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaMinusPhiMinus      = new TH1F("upperTrackPtEtaMinusPhiMinus", "upperTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaMinusPhiZero      = new TH1F("upperTrackPtEtaMinusPhiZero", "upperTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_upperTrackPtEtaMinusPhiPlus      = new TH1F("upperTrackPtEtaMinusPhiPlus", "upperTrackPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_upperTrackEta = new TH1F("upperTrackEta","upperTrackEta", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaPlus      = new TH1F("upperTrackEtaEtaPlus", "upperTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaMinus      = new TH1F("upperTrackEtaEtaMinus", "upperTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaPlusPhiMinus      = new TH1F("upperTrackEtaEtaPlusPhiMinus", "upperTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaPlusPhiZero      = new TH1F("upperTrackEtaEtaPlusPhiZero", "upperTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaPlusPhiPlus      = new TH1F("upperTrackEtaEtaPlusPhiPlus", "upperTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaMinusPhiMinus      = new TH1F("upperTrackEtaEtaMinusPhiMinus", "upperTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaMinusPhiZero      = new TH1F("upperTrackEtaEtaMinusPhiZero", "upperTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_upperTrackEtaEtaMinusPhiPlus      = new TH1F("upperTrackEtaEtaMinusPhiPlus", "upperTrackEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_upperTrackPhi = new TH1F("upperTrackPhi","upperTrackPhi", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaPlus      = new TH1F("upperTrackPhiEtaPlus", "upperTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaMinus      = new TH1F("upperTrackPhiEtaMinus", "upperTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaPlusPhiMinus      = new TH1F("upperTrackPhiEtaPlusPhiMinus", "upperTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaPlusPhiZero      = new TH1F("upperTrackPhiEtaPlusPhiZero", "upperTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaPlusPhiPlus      = new TH1F("upperTrackPhiEtaPlusPhiPlus", "upperTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaMinusPhiMinus      = new TH1F("upperTrackPhiEtaMinusPhiMinus", "upperTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaMinusPhiZero      = new TH1F("upperTrackPhiEtaMinusPhiZero", "upperTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_upperTrackPhiEtaMinusPhiPlus      = new TH1F("upperTrackPhiEtaMinusPhiPlus", "upperTrackPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_upperPtError  = new TH1F("upperPtError", "upperPtError",  100, 0., 600.);
  TH1F *h_upperPtErrorEtaPlus      = new TH1F("upperPtErrorEtaPlus", "upperPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaMinus      = new TH1F("upperPtErrorEtaMinus", "upperPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaPlusPhiMinus      = new TH1F("upperPtErrorEtaPlusPhiMinus", "upperPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaPlusPhiZero      = new TH1F("upperPtErrorEtaPlusPhiZero", "upperPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaPlusPhiPlus      = new TH1F("upperPtErrorEtaPlusPhiPlus", "upperPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaMinusPhiMinus      = new TH1F("upperPtErrorEtaMinusPhiMinus", "upperPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaMinusPhiZero      = new TH1F("upperPtErrorEtaMinusPhiZero", "upperPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_upperPtErrorEtaMinusPhiPlus      = new TH1F("upperPtErrorEtaMinusPhiPlus", "upperPtErrorEtaMinusPhiPlus", 100, 0., 600.);


  TH1F *h_upperPtRelErr = new TH1F("upperPtRelErr","upperPtRelErr", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaPlus      = new TH1F("upperPtRelErrEtaPlus", "upperPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaMinus      = new TH1F("upperPtRelErrEtaMinus", "upperPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaPlusPhiMinus      = new TH1F("upperPtRelErrEtaPlusPhiMinus", "upperPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaPlusPhiZero      = new TH1F("upperPtRelErrEtaPlusPhiZero", "upperPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaPlusPhiPlus      = new TH1F("upperPtRelErrEtaPlusPhiPlus", "upperPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaMinusPhiMinus      = new TH1F("upperPtRelErrEtaMinusPhiMinus", "upperPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaMinusPhiZero      = new TH1F("upperPtRelErrEtaMinusPhiZero", "upperPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_upperPtRelErrEtaMinusPhiPlus      = new TH1F("upperPtRelErrEtaMinusPhiPlus", "upperPtRelErrEtaMinusPhiPlus", 100, 0., 1.);


  TH1F *h_upperPixelHits                    = new TH1F("upperPixelHits",                   "upperPixelHits",           10, -0.5, 9.5 );
  TH1F *h_upperPixelHitsEtaPlus      = new TH1F("upperPixelHitsEtaPlus", "upperPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaMinus      = new TH1F("upperPixelHitsEtaMinus", "upperPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaPlusPhiMinus      = new TH1F("upperPixelHitsEtaPlusPhiMinus", "upperPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaPlusPhiZero      = new TH1F("upperPixelHitsEtaPlusPhiZero", "upperPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaPlusPhiPlus      = new TH1F("upperPixelHitsEtaPlusPhiPlus", "upperPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaMinusPhiMinus      = new TH1F("upperPixelHitsEtaMinusPhiMinus", "upperPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaMinusPhiZero      = new TH1F("upperPixelHitsEtaMinusPhiZero", "upperPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_upperPixelHitsEtaMinusPhiPlus      = new TH1F("upperPixelHitsEtaMinusPhiPlus", "upperPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_upperTrackerHits                  = new TH1F("upperTrackerHits",                 "upperTrackerHits",         35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaPlus      = new TH1F("upperTrackerHitsEtaPlus", "upperTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaMinus      = new TH1F("upperTrackerHitsEtaMinus", "upperTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaPlusPhiMinus      = new TH1F("upperTrackerHitsEtaPlusPhiMinus", "upperTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaPlusPhiZero      = new TH1F("upperTrackerHitsEtaPlusPhiZero", "upperTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaPlusPhiPlus      = new TH1F("upperTrackerHitsEtaPlusPhiPlus", "upperTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaMinusPhiMinus      = new TH1F("upperTrackerHitsEtaMinusPhiMinus", "upperTrackerHitsEtaMinusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaMinusPhiZero      = new TH1F("upperTrackerHitsEtaMinusPhiZero", "upperTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_upperTrackerHitsEtaMinusPhiPlus      = new TH1F("upperTrackerHitsEtaMinusPhiPlus", "upperTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);


  TH1F *h_upperMuonStationHits              = new TH1F("upperMuonStationHits",             "upperMuonStationHits",     10, -0.5, 9.5 );
  TH1F *h_upperMuonStationHitsEtaPlus      = new TH1F("upperMuonStationHitsEtaPlus", "upperMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaMinus      = new TH1F("upperMuonStationHitsEtaMinus", "upperMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaPlusPhiMinus      = new TH1F("upperMuonStationHitsEtaPlusPhiMinus", "upperMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaPlusPhiZero      = new TH1F("upperMuonStationHitsEtaPlusPhiZero", "upperMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaPlusPhiPlus      = new TH1F("upperMuonStationHitsEtaPlusPhiPlus", "upperMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaMinusPhiMinus      = new TH1F("upperMuonStationHitsEtaMinusPhiMinus", "upperMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaMinusPhiZero      = new TH1F("upperMuonStationHitsEtaMinusPhiZero", "upperMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_upperMuonStationHitsEtaMinusPhiPlus      = new TH1F("upperMuonStationHitsEtaMinusPhiPlus", "upperMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_upperValidHits                    = new TH1F("upperValidHits",                   "upperValidHits",           100,-0.5, 99.5);
  TH1F *h_upperValidHitsEtaPlus      = new TH1F("upperValidHitsEtaPlus", "upperValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaMinus      = new TH1F("upperValidHitsEtaMinus", "upperValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaPlusPhiMinus      = new TH1F("upperValidHitsEtaPlusPhiMinus", "upperValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaPlusPhiZero      = new TH1F("upperValidHitsEtaPlusPhiZero", "upperValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaPlusPhiPlus      = new TH1F("upperValidHitsEtaPlusPhiPlus", "upperValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaMinusPhiMinus      = new TH1F("upperValidHitsEtaMinusPhiMinus", "upperValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaMinusPhiZero      = new TH1F("upperValidHitsEtaMinusPhiZero", "upperValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_upperValidHitsEtaMinusPhiPlus      = new TH1F("upperValidHitsEtaMinusPhiPlus", "upperValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_upperValidMuonHits                = new TH1F("upperValidMuonHits",               "upperValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaPlus      = new TH1F("upperValidMuonHitsEtaPlus", "upperValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaMinus      = new TH1F("upperValidMuonHitsEtaMinus", "upperValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaPlusPhiMinus      = new TH1F("upperValidMuonHitsEtaPlusPhiMinus", "upperValidMuonHitsEtaPlusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaPlusPhiZero      = new TH1F("upperValidMuonHitsEtaPlusPhiZero", "upperValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaPlusPhiPlus      = new TH1F("upperValidMuonHitsEtaPlusPhiPlus", "upperValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaMinusPhiMinus      = new TH1F("upperValidMuonHitsEtaMinusPhiMinus", "upperValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaMinusPhiZero      = new TH1F("upperValidMuonHitsEtaMinusPhiZero", "upperValidMuonHitsEtaMinusPhiZero", 75, -0.5, 74.5);
  TH1F *h_upperValidMuonHitsEtaMinusPhiPlus      = new TH1F("upperValidMuonHitsEtaMinusPhiPlus", "upperValidMuonHitsEtaMinusPhiPlus", 75, -0.5, 74.5);


  TH1F *h_upperMatchedMuonStations          = new TH1F("upperMatchedMuonStations",         "upperMatchedMuonStations", 10, -0.5, 9.5 );
  TH1F *h_upperMatchedMuonStationsEtaPlus      = new TH1F("upperMatchedMuonStationsEtaPlus",
								   "upperMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaMinus      = new TH1F("upperMatchedMuonStationsEtaMinus",
								    "upperMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("upperMatchedMuonStationsEtaPlusPhiMinus",
									   "upperMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaPlusPhiZero      = new TH1F("upperMatchedMuonStationsEtaPlusPhiZero",
									  "upperMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("upperMatchedMuonStationsEtaPlusPhiPlus",
									  "upperMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("upperMatchedMuonStationsEtaMinusPhiMinus",
									    "upperMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaMinusPhiZero      = new TH1F("upperMatchedMuonStationsEtaMinusPhiZero",
									   "upperMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_upperMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("upperMatchedMuonStationsEtaMinusPhiPlus",
									   "upperMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_upperTrackerLayersWithMeasurement = new TH1F("upperTrackerLayersWithMeasurement","upperTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaPlus      = new TH1F("upperTrackerLayersWithMeasurementEtaPlus",
								   "upperTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaMinus      = new TH1F("upperTrackerLayersWithMeasurementEtaMinus",
								    "upperTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("upperTrackerLayersWithMeasurementEtaPlusPhiMinus",
									   "upperTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("upperTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "upperTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("upperTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "upperTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("upperTrackerLayersWithMeasurementEtaMinusPhiMinus",
									    "upperTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("upperTrackerLayersWithMeasurementEtaMinusPhiZero",
									   "upperTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_upperTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("upperTrackerLayersWithMeasurementEtaMinusPhiPlus",
									   "upperTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_upperCurvePlusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPhiMinusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPhiZeroPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPhiPlusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPhiMinusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPhiZeroPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPhiPlusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaAllPhiMinusPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaAllPhiZeroPlusBias[nBiasBins];
  TH1F *h_upperCurveEtaAllPhiPlusPlusBias[nBiasBins];

  TH1F *h_upperCurveMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPhiMinusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPhiZeroMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaPlusPhiPlusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPhiMinusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPhiZeroMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaMinusPhiPlusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaAllPhiMinusMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaAllPhiZeroMinusBias[nBiasBins];
  TH1F *h_upperCurveEtaAllPhiPlusMinusBias[nBiasBins];

  for (int i = 0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;

    std::stringstream title;
    title << "#Delta#kappa = +" << (i+1)*(factor_*maxBias/nBiasBins);
    h_upperCurvePlusBias[i]  = new TH1F(TString("upperCurvePlusBias" + name.str()), TString("upperCurvePlusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaPlusPlusBias[i]  = new TH1F(TString("upperCurveEtaPlusPlusBias" + name.str()), TString("upperCurveEtaPlusPlusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaMinusPlusBias[i]  = new TH1F(TString("upperCurveEtaMinusPlusBias" + name.str()), TString("upperCurveEtaMinusPlusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaPlusPhiMinusPlusBias[i]  = new TH1F(TString("upperCurveEtaPlusPhiMinusPlusBias" + name.str()),
						       TString("upperCurveEtaPlusPhiMinusPlusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaPlusPhiZeroPlusBias[i]  = new TH1F(TString("upperCurveEtaPlusPhiZeroPlusBias" + name.str()),
						      TString("upperCurveEtaPlusPhiZeroPlusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaPlusPhiPlusPlusBias[i]  = new TH1F(TString("upperCurveEtaPlusPhiPlusPlusBias" + name.str()),
						      TString("upperCurveEtaPlusPhiPlusPlusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaMinusPhiMinusPlusBias[i]  = new TH1F(TString("upperCurveEtaMinususPhiMinusPlusBias" + name.str()),
							TString("upperCurveEtaMinusPhiMinusPlusBias" + title.str()),
							symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaMinusPhiZeroPlusBias[i]  = new TH1F(TString("upperCurveEtaMinusPhiZeroPlusBias" + name.str()),
						       TString("upperCurveEtaMinusPhiZeroPlusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaMinusPhiPlusPlusBias[i]  = new TH1F(TString("upperCurveEtaMinusPhiPlusPlusBias" + name.str()),
						       TString("upperCurveEtaMinusPhiPlusPlusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaAllPhiMinusPlusBias[i]  = new TH1F(TString("upperCurveEtaAllPhiMinusPlusBias" + name.str()),
						      TString("upperCurveEtaAllPhiMinusPlusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaAllPhiZeroPlusBias[i]  = new TH1F(TString("upperCurveEtaAllPhiZeroPlusBias" + name.str()),
						     TString("upperCurveEtaAllPhiZeroPlusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_upperCurveEtaAllPhiPlusPlusBias[i]  = new TH1F(TString("upperCurveEtaAllPhiPlusPlusBias" + name.str()),
						     TString("upperCurveEtaAllPhiPlusPlusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    title.str("");
    title.clear();
    title << "#Delta#kappa = -" << (i+1)*(factor_*maxBias/nBiasBins);
    h_upperCurveMinusBias[i] = new TH1F(TString("upperCurveMinusBias" + name.str()),TString("upperCurveMinusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaPlusMinusBias[i] = new TH1F(TString("upperCurveEtaPlusMinusBias" + name.str()),TString("upperCurveEtaPlusMinusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaMinusMinusBias[i] = new TH1F(TString("upperCurveEtaMinusMinusBias" + name.str()),TString("upperCurveEtaMinusMinusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaPlusPhiMinusMinusBias[i] = new TH1F(TString("upperCurveEtaPlusPhiMinusMinusBias" + name.str()),
						       TString("upperCurveEtaPlusPhiMinusMinusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaPlusPhiZeroMinusBias[i] = new TH1F(TString("upperCurveEtaPlusPhiZeroMinusBias" + name.str()),
						      TString("upperCurveEtaPlusPhiZeroMinusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaPlusPhiPlusMinusBias[i] = new TH1F(TString("upperCurveEtaPlusPhiPlusMinusBias" + name.str()),
						      TString("upperCurveEtaPlusPhiPlusMinusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaMinusPhiMinusMinusBias[i] = new TH1F(TString("upperCurveEtaMinusPhiMinusMinusBias" + name.str()),
							TString("upperCurveEtaMinusPhiMinusMinusBias" + title.str()),
							symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaMinusPhiZeroMinusBias[i] = new TH1F(TString("upperCurveEtaMinusPhiZeroMinusBias" + name.str()),
						       TString("upperCurveEtaMinusPhiZeroMinusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaMinusPhiPlusMinusBias[i] = new TH1F(TString("upperCurveEtaMinusPhiPlusMinusBias" + name.str()),
						       TString("upperCurveEtaMinusPhiPlusMinusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaAllPhiMinusMinusBias[i] = new TH1F(TString("upperCurveEtaAllPhiMinusMinusBias" + name.str()),
						      TString("upperCurveEtaAllPhiMinusMinusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaAllPhiZeroMinusBias[i] = new TH1F(TString("upperCurveEtaAllPhiZeroMinusBias" + name.str()),
						     TString("upperCurveEtaAllPhiZeroMinusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_upperCurveEtaAllPhiPlusMinusBias[i] = new TH1F(TString("upperCurveEtaAllPhiPlusMinusBias" + name.str()),
						     TString("upperCurveEtaAllPhiPlusMinusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
  }

  // histograms for lower leg muons, inclusive
  TH1F *h_lowerPt       = new TH1F("lowerPt", "lowerPt",  300, 0., 3000.);
  TH1F *h_lowerPtEtaPlus      = new TH1F("lowerPtEtaPlus", "lowerPtEtaPlus", 300, 0., 3000.);
  TH1F *h_lowerPtEtaMinus      = new TH1F("lowerPtEtaMinus", "lowerPtEtaMinus", 300, 0., 3000.);
  TH1F *h_lowerPtEtaPlusPhiMinus      = new TH1F("lowerPtEtaPlusPhiMinus", "lowerPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_lowerPtEtaPlusPhiZero      = new TH1F("lowerPtEtaPlusPhiZero", "lowerPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_lowerPtEtaPlusPhiPlus      = new TH1F("lowerPtEtaPlusPhiPlus", "lowerPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_lowerPtEtaMinusPhiMinus      = new TH1F("lowerPtEtaMinusPhiMinus", "lowerPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_lowerPtEtaMinusPhiZero      = new TH1F("lowerPtEtaMinusPhiZero", "lowerPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_lowerPtEtaMinusPhiPlus      = new TH1F("lowerPtEtaMinusPhiPlus", "lowerPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_lowerEta      = new TH1F("lowerEta","lowerEta", 40, -2., 2.   );
  TH1F *h_lowerEtaEtaPlus      = new TH1F("lowerEtaEtaPlus", "lowerEtaEtaPlus", 40, -2., 2.);
  TH1F *h_lowerEtaEtaMinus      = new TH1F("lowerEtaEtaMinus", "lowerEtaEtaMinus", 40, -2., 2.);
  TH1F *h_lowerEtaEtaPlusPhiMinus      = new TH1F("lowerEtaEtaPlusPhiMinus", "lowerEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_lowerEtaEtaPlusPhiZero      = new TH1F("lowerEtaEtaPlusPhiZero", "lowerEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_lowerEtaEtaPlusPhiPlus      = new TH1F("lowerEtaEtaPlusPhiPlus", "lowerEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_lowerEtaEtaMinusPhiMinus      = new TH1F("lowerEtaEtaMinusPhiMinus", "lowerEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_lowerEtaEtaMinusPhiZero      = new TH1F("lowerEtaEtaMinusPhiZero", "lowerEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_lowerEtaEtaMinusPhiPlus      = new TH1F("lowerEtaEtaMinusPhiPlus", "lowerEtaEtaMinusPhiPlus", 40, -2., 2.);


  TH1F *h_lowerPhi      = new TH1F("lowerPhi","lowerPhi", 40, -4., 4.   );
  TH1F *h_lowerPhiEtaPlus      = new TH1F("lowerPhiEtaPlus", "lowerPhiEtaPlus", 40, -4., 4.);
  TH1F *h_lowerPhiEtaMinus      = new TH1F("lowerPhiEtaMinus", "lowerPhiEtaMinus", 40, -4., 4.);
  TH1F *h_lowerPhiEtaPlusPhiMinus      = new TH1F("lowerPhiEtaPlusPhiMinus", "lowerPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_lowerPhiEtaPlusPhiZero      = new TH1F("lowerPhiEtaPlusPhiZero", "lowerPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_lowerPhiEtaPlusPhiPlus      = new TH1F("lowerPhiEtaPlusPhiPlus", "lowerPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_lowerPhiEtaMinusPhiMinus      = new TH1F("lowerPhiEtaMinusPhiMinus", "lowerPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_lowerPhiEtaMinusPhiZero      = new TH1F("lowerPhiEtaMinusPhiZero", "lowerPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_lowerPhiEtaMinusPhiPlus      = new TH1F("lowerPhiEtaMinusPhiPlus", "lowerPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_lowerChi2     = new TH1F("lowerChi2",  "lowerChi2",   50,   0.,  150.);
  TH1F *h_lowerChi2EtaPlus      = new TH1F("lowerChi2EtaPlus", "lowerChi2EtaPlus", 50, 0., 150.);
  TH1F *h_lowerChi2EtaMinus      = new TH1F("lowerChi2EtaMinus", "lowerChi2EtaMinus", 50, 0., 150.);
  TH1F *h_lowerChi2EtaPlusPhiMinus      = new TH1F("lowerChi2EtaPlusPhiMinus", "lowerChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_lowerChi2EtaPlusPhiZero      = new TH1F("lowerChi2EtaPlusPhiZero", "lowerChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_lowerChi2EtaPlusPhiPlus      = new TH1F("lowerChi2EtaPlusPhiPlus", "lowerChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_lowerChi2EtaMinusPhiMinus      = new TH1F("lowerChi2EtaMinusPhiMinus", "lowerChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_lowerChi2EtaMinusPhiZero      = new TH1F("lowerChi2EtaMinusPhiZero", "lowerChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_lowerChi2EtaMinusPhiPlus      = new TH1F("lowerChi2EtaMinusPhiPlus", "lowerChi2EtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_lowerNdof     = new TH1F("lowerNdof",  "lowerNdof",   100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaPlus      = new TH1F("lowerNdofEtaPlus", "lowerNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaMinus      = new TH1F("lowerNdofEtaMinus", "lowerNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaPlusPhiMinus      = new TH1F("lowerNdofEtaPlusPhiMinus", "lowerNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaPlusPhiZero      = new TH1F("lowerNdofEtaPlusPhiZero", "lowerNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaPlusPhiPlus      = new TH1F("lowerNdofEtaPlusPhiPlus", "lowerNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaMinusPhiMinus      = new TH1F("lowerNdofEtaMinusPhiMinus", "lowerNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaMinusPhiZero      = new TH1F("lowerNdofEtaMinusPhiZero", "lowerNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_lowerNdofEtaMinusPhiPlus      = new TH1F("lowerNdofEtaMinusPhiPlus", "lowerNdofEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_lowerCharge   = new TH1F("lowerCharge","lowerCharge", 3,   -1.5, 1.5 );
  TH1F *h_lowerChargeEtaPlus      = new TH1F("lowerChargeEtaPlus", "lowerChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaMinus      = new TH1F("lowerChargeEtaMinus", "lowerChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaPlusPhiMinus      = new TH1F("lowerChargeEtaPlusPhiMinus", "lowerChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaPlusPhiZero      = new TH1F("lowerChargeEtaPlusPhiZero", "lowerChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaPlusPhiPlus      = new TH1F("lowerChargeEtaPlusPhiPlus", "lowerChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaMinusPhiMinus      = new TH1F("lowerChargeEtaMinusPhiMinus", "lowerChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaMinusPhiZero      = new TH1F("lowerChargeEtaMinusPhiZero", "lowerChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_lowerChargeEtaMinusPhiPlus      = new TH1F("lowerChargeEtaMinusPhiPlus", "lowerChargeEtaMinusPhiPlus", 3, -1.5, 1.5);


  TH1F *h_lowerCurve    = new TH1F("lowerCurve", "lowerCurve",
				   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaPlus    = new TH1F("lowerCurveEtaPlus", "lowerCurveEtaPlus",
					  symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaMinus    = new TH1F("lowerCurveEtaMinus", "lowerCurveEtaMinus",
					   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaPlusPhiMinus    = new TH1F("lowerCurveEtaPlusPhiMinus", "lowerCurveEtaPlusPhiMinus",
						  symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaPlusPhiZero    = new TH1F("lowerCurveEtaPlusPhiZero", "lowerCurveEtaPlusPhiZero",
						 symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaPlusPhiPlus    = new TH1F("lowerCurveEtaPlusPhiPlus", "lowerCurveEtaPlusPhiPlus",
						 symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaMinusPhiMinus    = new TH1F("lowerCurveEtaMinusPhiMinus", "lowerCurveEtaMinusPhiMinus",
						   symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaMinusPhiZero    = new TH1F("lowerCurveEtaMinusPhiZero", "lowerCurveEtaMinusPhiZero",
						  symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaMinusPhiPlus    = new TH1F("lowerCurveEtaMinusPhiPlus", "lowerCurveEtaMinusPhiPlus",
						  symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaAllPhiMinus    = new TH1F("lowerCurveEtaAllPhiMinus", "lowerCurveEtaAllPhiMinus",
						 symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaAllPhiZero    = new TH1F("lowerCurveEtaAllPhiZero", "lowerCurveEtaAllPhiZero",
						symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerCurveEtaAllPhiPlus    = new TH1F("lowerCurveEtaAllPhiPlus", "lowerCurveEtaAllPhiPlus",
						symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
  TH1F *h_lowerDxy      = new TH1F("lowerDxy",   "lowerDxy",    100, -100., 100.);
  TH1F *h_lowerDxyEtaPlus      = new TH1F("lowerDxyEtaPlus", "lowerDxyEtaPlus", 100, -100., 100.);
  TH1F *h_lowerDxyEtaMinus      = new TH1F("lowerDxyEtaMinus", "lowerDxyEtaMinus", 100, -100., 100.);
  TH1F *h_lowerDxyEtaPlusPhiMinus      = new TH1F("lowerDxyEtaPlusPhiMinus", "lowerDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_lowerDxyEtaPlusPhiZero      = new TH1F("lowerDxyEtaPlusPhiZero", "lowerDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_lowerDxyEtaPlusPhiPlus      = new TH1F("lowerDxyEtaPlusPhiPlus", "lowerDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_lowerDxyEtaMinusPhiMinus      = new TH1F("lowerDxyEtaMinusPhiMinus", "lowerDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_lowerDxyEtaMinusPhiZero      = new TH1F("lowerDxyEtaMinusPhiZero", "lowerDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_lowerDxyEtaMinusPhiPlus      = new TH1F("lowerDxyEtaMinusPhiPlus", "lowerDxyEtaMinusPhiPlus", 100, -100., 100.);


  TH1F *h_lowerDz       = new TH1F("lowerDz",    "lowerDz",     100, -250., 250.);
  TH1F *h_lowerDzEtaPlus      = new TH1F("lowerDzEtaPlus", "lowerDzEtaPlus", 100, -250., 250.);
  TH1F *h_lowerDzEtaMinus      = new TH1F("lowerDzEtaMinus", "lowerDzEtaMinus", 100, -250., 250.);
  TH1F *h_lowerDzEtaPlusPhiMinus      = new TH1F("lowerDzEtaPlusPhiMinus", "lowerDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_lowerDzEtaPlusPhiZero      = new TH1F("lowerDzEtaPlusPhiZero", "lowerDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_lowerDzEtaPlusPhiPlus      = new TH1F("lowerDzEtaPlusPhiPlus", "lowerDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_lowerDzEtaMinusPhiMinus      = new TH1F("lowerDzEtaMinusPhiMinus", "lowerDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_lowerDzEtaMinusPhiZero      = new TH1F("lowerDzEtaMinusPhiZero", "lowerDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_lowerDzEtaMinusPhiPlus      = new TH1F("lowerDzEtaMinusPhiPlus", "lowerDzEtaMinusPhiPlus", 100, -250., 250.);


  TH1F *h_lowerDxyError = new TH1F("lowerDxyError","lowerDxyError", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaPlus      = new TH1F("lowerDxyErrorEtaPlus", "lowerDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaMinus      = new TH1F("lowerDxyErrorEtaMinus", "lowerDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaPlusPhiMinus      = new TH1F("lowerDxyErrorEtaPlusPhiMinus", "lowerDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaPlusPhiZero      = new TH1F("lowerDxyErrorEtaPlusPhiZero", "lowerDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaPlusPhiPlus      = new TH1F("lowerDxyErrorEtaPlusPhiPlus", "lowerDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaMinusPhiMinus      = new TH1F("lowerDxyErrorEtaMinusPhiMinus", "lowerDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaMinusPhiZero      = new TH1F("lowerDxyErrorEtaMinusPhiZero", "lowerDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_lowerDxyErrorEtaMinusPhiPlus      = new TH1F("lowerDxyErrorEtaMinusPhiPlus", "lowerDxyErrorEtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_lowerDzError  = new TH1F("lowerDzError", "lowerDzError",  50, 0., 150.);
  TH1F *h_lowerDzErrorEtaPlus      = new TH1F("lowerDzErrorEtaPlus", "lowerDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaMinus      = new TH1F("lowerDzErrorEtaMinus", "lowerDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaPlusPhiMinus      = new TH1F("lowerDzErrorEtaPlusPhiMinus", "lowerDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaPlusPhiZero      = new TH1F("lowerDzErrorEtaPlusPhiZero", "lowerDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaPlusPhiPlus      = new TH1F("lowerDzErrorEtaPlusPhiPlus", "lowerDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaMinusPhiMinus      = new TH1F("lowerDzErrorEtaMinusPhiMinus", "lowerDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaMinusPhiZero      = new TH1F("lowerDzErrorEtaMinusPhiZero", "lowerDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_lowerDzErrorEtaMinusPhiPlus      = new TH1F("lowerDzErrorEtaMinusPhiPlus", "lowerDzErrorEtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_lowerTrackPt  = new TH1F("lowerTrackPt", "lowerTrackPt",  300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaPlus      = new TH1F("lowerTrackPtEtaPlus", "lowerTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaMinus      = new TH1F("lowerTrackPtEtaMinus", "lowerTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaPlusPhiMinus      = new TH1F("lowerTrackPtEtaPlusPhiMinus", "lowerTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaPlusPhiZero      = new TH1F("lowerTrackPtEtaPlusPhiZero", "lowerTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaPlusPhiPlus      = new TH1F("lowerTrackPtEtaPlusPhiPlus", "lowerTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaMinusPhiMinus      = new TH1F("lowerTrackPtEtaMinusPhiMinus", "lowerTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaMinusPhiZero      = new TH1F("lowerTrackPtEtaMinusPhiZero", "lowerTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_lowerTrackPtEtaMinusPhiPlus      = new TH1F("lowerTrackPtEtaMinusPhiPlus", "lowerTrackPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_lowerTrackEta = new TH1F("lowerTrackEta","lowerTrackEta", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaPlus      = new TH1F("lowerTrackEtaEtaPlus", "lowerTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaMinus      = new TH1F("lowerTrackEtaEtaMinus", "lowerTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaPlusPhiMinus      = new TH1F("lowerTrackEtaEtaPlusPhiMinus", "lowerTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaPlusPhiZero      = new TH1F("lowerTrackEtaEtaPlusPhiZero", "lowerTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaPlusPhiPlus      = new TH1F("lowerTrackEtaEtaPlusPhiPlus", "lowerTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaMinusPhiMinus      = new TH1F("lowerTrackEtaEtaMinusPhiMinus", "lowerTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaMinusPhiZero      = new TH1F("lowerTrackEtaEtaMinusPhiZero", "lowerTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_lowerTrackEtaEtaMinusPhiPlus      = new TH1F("lowerTrackEtaEtaMinusPhiPlus", "lowerTrackEtaEtaMinusPhiPlus", 40, -2., 2.);


  TH1F *h_lowerTrackPhi = new TH1F("lowerTrackPhi","lowerTrackPhi", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaPlus      = new TH1F("lowerTrackPhiEtaPlus", "lowerTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaMinus      = new TH1F("lowerTrackPhiEtaMinus", "lowerTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaPlusPhiMinus      = new TH1F("lowerTrackPhiEtaPlusPhiMinus", "lowerTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaPlusPhiZero      = new TH1F("lowerTrackPhiEtaPlusPhiZero", "lowerTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaPlusPhiPlus      = new TH1F("lowerTrackPhiEtaPlusPhiPlus", "lowerTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaMinusPhiMinus      = new TH1F("lowerTrackPhiEtaMinusPhiMinus", "lowerTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaMinusPhiZero      = new TH1F("lowerTrackPhiEtaMinusPhiZero", "lowerTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_lowerTrackPhiEtaMinusPhiPlus      = new TH1F("lowerTrackPhiEtaMinusPhiPlus", "lowerTrackPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_lowerPtError  = new TH1F("lowerPtError", "lowerPtError",  100, 0., 600.);
  TH1F *h_lowerPtErrorEtaPlus      = new TH1F("lowerPtErrorEtaPlus", "lowerPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaMinus      = new TH1F("lowerPtErrorEtaMinus", "lowerPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaPlusPhiMinus      = new TH1F("lowerPtErrorEtaPlusPhiMinus", "lowerPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaPlusPhiZero      = new TH1F("lowerPtErrorEtaPlusPhiZero", "lowerPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaPlusPhiPlus      = new TH1F("lowerPtErrorEtaPlusPhiPlus", "lowerPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaMinusPhiMinus      = new TH1F("lowerPtErrorEtaMinusPhiMinus", "lowerPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaMinusPhiZero      = new TH1F("lowerPtErrorEtaMinusPhiZero", "lowerPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_lowerPtErrorEtaMinusPhiPlus      = new TH1F("lowerPtErrorEtaMinusPhiPlus", "lowerPtErrorEtaMinusPhiPlus", 100, 0., 600.);


  TH1F *h_lowerPtRelErr = new TH1F("lowerPtRelErr","lowerPtRelErr", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaPlus      = new TH1F("lowerPtRelErrEtaPlus", "lowerPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaMinus      = new TH1F("lowerPtRelErrEtaMinus", "lowerPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaPlusPhiMinus      = new TH1F("lowerPtRelErrEtaPlusPhiMinus", "lowerPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaPlusPhiZero      = new TH1F("lowerPtRelErrEtaPlusPhiZero", "lowerPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaPlusPhiPlus      = new TH1F("lowerPtRelErrEtaPlusPhiPlus", "lowerPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaMinusPhiMinus      = new TH1F("lowerPtRelErrEtaMinusPhiMinus", "lowerPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaMinusPhiZero      = new TH1F("lowerPtRelErrEtaMinusPhiZero", "lowerPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_lowerPtRelErrEtaMinusPhiPlus      = new TH1F("lowerPtRelErrEtaMinusPhiPlus", "lowerPtRelErrEtaMinusPhiPlus", 100, 0., 1.);


  TH1F *h_lowerPixelHits           = new TH1F("lowerPixelHits",          "lowerPixelHits",           10, -0.5, 9.5 );
  TH1F *h_lowerPixelHitsEtaPlus      = new TH1F("lowerPixelHitsEtaPlus", "lowerPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaMinus      = new TH1F("lowerPixelHitsEtaMinus", "lowerPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaPlusPhiMinus      = new TH1F("lowerPixelHitsEtaPlusPhiMinus", "lowerPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaPlusPhiZero      = new TH1F("lowerPixelHitsEtaPlusPhiZero", "lowerPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaPlusPhiPlus      = new TH1F("lowerPixelHitsEtaPlusPhiPlus", "lowerPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaMinusPhiMinus      = new TH1F("lowerPixelHitsEtaMinusPhiMinus", "lowerPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaMinusPhiZero      = new TH1F("lowerPixelHitsEtaMinusPhiZero", "lowerPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_lowerPixelHitsEtaMinusPhiPlus      = new TH1F("lowerPixelHitsEtaMinusPhiPlus", "lowerPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_lowerTrackerHits         = new TH1F("lowerTrackerHits",        "lowerTrackerHits",         35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaPlus      = new TH1F("lowerTrackerHitsEtaPlus", "lowerTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaMinus      = new TH1F("lowerTrackerHitsEtaMinus", "lowerTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaPlusPhiMinus      = new TH1F("lowerTrackerHitsEtaPlusPhiMinus", "lowerTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaPlusPhiZero      = new TH1F("lowerTrackerHitsEtaPlusPhiZero", "lowerTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaPlusPhiPlus      = new TH1F("lowerTrackerHitsEtaPlusPhiPlus", "lowerTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaMinusPhiMinus      = new TH1F("lowerTrackerHitsEtaMinusPhiMinus", "lowerTrackerHitsEtaMinusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaMinusPhiZero      = new TH1F("lowerTrackerHitsEtaMinusPhiZero", "lowerTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_lowerTrackerHitsEtaMinusPhiPlus      = new TH1F("lowerTrackerHitsEtaMinusPhiPlus", "lowerTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);


  TH1F *h_lowerMuonStationHits     = new TH1F("lowerMuonStationHits",    "lowerMuonStationHits",     10, -0.5, 9.5 );
  TH1F *h_lowerMuonStationHitsEtaPlus      = new TH1F("lowerMuonStationHitsEtaPlus", "lowerMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaMinus      = new TH1F("lowerMuonStationHitsEtaMinus", "lowerMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaPlusPhiMinus      = new TH1F("lowerMuonStationHitsEtaPlusPhiMinus", "lowerMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaPlusPhiZero      = new TH1F("lowerMuonStationHitsEtaPlusPhiZero", "lowerMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaPlusPhiPlus      = new TH1F("lowerMuonStationHitsEtaPlusPhiPlus", "lowerMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaMinusPhiMinus      = new TH1F("lowerMuonStationHitsEtaMinusPhiMinus", "lowerMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaMinusPhiZero      = new TH1F("lowerMuonStationHitsEtaMinusPhiZero", "lowerMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_lowerMuonStationHitsEtaMinusPhiPlus      = new TH1F("lowerMuonStationHitsEtaMinusPhiPlus", "lowerMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_lowerValidHits           = new TH1F("lowerValidHits",          "lowerValidHits",           100,-0.5, 99.5);
  TH1F *h_lowerValidHitsEtaPlus      = new TH1F("lowerValidHitsEtaPlus", "lowerValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaMinus      = new TH1F("lowerValidHitsEtaMinus", "lowerValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaPlusPhiMinus      = new TH1F("lowerValidHitsEtaPlusPhiMinus", "lowerValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaPlusPhiZero      = new TH1F("lowerValidHitsEtaPlusPhiZero", "lowerValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaPlusPhiPlus      = new TH1F("lowerValidHitsEtaPlusPhiPlus", "lowerValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaMinusPhiMinus      = new TH1F("lowerValidHitsEtaMinusPhiMinus", "lowerValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaMinusPhiZero      = new TH1F("lowerValidHitsEtaMinusPhiZero", "lowerValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_lowerValidHitsEtaMinusPhiPlus      = new TH1F("lowerValidHitsEtaMinusPhiPlus", "lowerValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_lowerValidMuonHits       = new TH1F("lowerValidMuonHits",      "lowerValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaPlus      = new TH1F("lowerValidMuonHitsEtaPlus", "lowerValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaMinus      = new TH1F("lowerValidMuonHitsEtaMinus", "lowerValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaPlusPhiMinus      = new TH1F("lowerValidMuonHitsEtaPlusPhiMinus", "lowerValidMuonHitsEtaPlusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaPlusPhiZero      = new TH1F("lowerValidMuonHitsEtaPlusPhiZero", "lowerValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaPlusPhiPlus      = new TH1F("lowerValidMuonHitsEtaPlusPhiPlus", "lowerValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaMinusPhiMinus      = new TH1F("lowerValidMuonHitsEtaMinusPhiMinus", "lowerValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaMinusPhiZero      = new TH1F("lowerValidMuonHitsEtaMinusPhiZero", "lowerValidMuonHitsEtaMinusPhiZero", 75, -0.5, 74.5);
  TH1F *h_lowerValidMuonHitsEtaMinusPhiPlus      = new TH1F("lowerValidMuonHitsEtaMinusPhiPlus", "lowerValidMuonHitsEtaMinusPhiPlus", 75, -0.5, 74.5);

  TH1F *h_lowerMatchedMuonStations = new TH1F("lowerMatchedMuonStations","lowerMatchedMuonStations", 10, -0.5, 9.5 );
  TH1F *h_lowerMatchedMuonStationsEtaPlus      = new TH1F("lowerMatchedMuonStationsEtaPlus",
								   "lowerMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaMinus      = new TH1F("lowerMatchedMuonStationsEtaMinus",
								    "lowerMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("lowerMatchedMuonStationsEtaPlusPhiMinus",
									   "lowerMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaPlusPhiZero      = new TH1F("lowerMatchedMuonStationsEtaPlusPhiZero",
									  "lowerMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("lowerMatchedMuonStationsEtaPlusPhiPlus",
									  "lowerMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("lowerMatchedMuonStationsEtaMinusPhiMinus",
									    "lowerMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaMinusPhiZero      = new TH1F("lowerMatchedMuonStationsEtaMinusPhiZero",
									   "lowerMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_lowerMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("lowerMatchedMuonStationsEtaMinusPhiPlus",
									   "lowerMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_lowerTrackerLayersWithMeasurement = new TH1F("lowerTrackerLayersWithMeasurement","lowerTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaPlus      = new TH1F("lowerTrackerLayersWithMeasurementEtaPlus",
								   "lowerTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaMinus      = new TH1F("lowerTrackerLayersWithMeasurementEtaMinus",
								    "lowerTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("lowerTrackerLayersWithMeasurementEtaPlusPhiMinus",
									   "lowerTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("lowerTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "lowerTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("lowerTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "lowerTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("lowerTrackerLayersWithMeasurementEtaMinusPhiMinus",
									    "lowerTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("lowerTrackerLayersWithMeasurementEtaMinusPhiZero",
									   "lowerTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_lowerTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("lowerTrackerLayersWithMeasurementEtaMinusPhiPlus",
									   "lowerTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_lowerCurvePlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPhiMinusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPhiZeroPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPhiPlusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPhiMinusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPhiZeroPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPhiPlusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaAllPhiMinusPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaAllPhiZeroPlusBias[nBiasBins];
  TH1F *h_lowerCurveEtaAllPhiPlusPlusBias[nBiasBins];

  TH1F *h_lowerCurveMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPhiMinusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPhiZeroMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaPlusPhiPlusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPhiMinusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPhiZeroMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaMinusPhiPlusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaAllPhiMinusMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaAllPhiZeroMinusBias[nBiasBins];
  TH1F *h_lowerCurveEtaAllPhiPlusMinusBias[nBiasBins];

  for (int i = 0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;

    std::stringstream title;
    title << "#Delta#kappa = +" << (i+1)*(factor_*maxBias/nBiasBins);
    h_lowerCurvePlusBias[i]  = new TH1F(TString("lowerCurvePlusBias" + name.str()), TString("lowerCurvePlusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaPlusPlusBias[i]  = new TH1F(TString("lowerCurveEtaPlusPlusBias" + name.str()), TString("lowerCurveEtaPlusPlusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaMinusPlusBias[i]  = new TH1F(TString("lowerCurveEtaMinusPlusBias" + name.str()), TString("lowerCurveEtaMinusPlusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaPlusPhiMinusPlusBias[i]  = new TH1F(TString("lowerCurveEtaPlusPhiMinusPlusBias" + name.str()),
						       TString("lowerCurveEtaPlusPhiMinusPlusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaPlusPhiZeroPlusBias[i]  = new TH1F(TString("lowerCurveEtaPlusPhiZeroPlusBias" + name.str()),
						      TString("lowerCurveEtaPlusPhiZeroPlusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaPlusPhiPlusPlusBias[i]  = new TH1F(TString("lowerCurveEtaPlusPhiPlusPlusBias" + name.str()),
						      TString("lowerCurveEtaPlusPhiPlusPlusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaMinusPhiMinusPlusBias[i]  = new TH1F(TString("lowerCurveEtaMinusPhiMinusPlusBias" + name.str()),
							TString("lowerCurveEtaMinusPhiMinusPlusBias" + title.str()),
							symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaMinusPhiZeroPlusBias[i]  = new TH1F(TString("lowerCurveEtaMinusPhiZeroPlusBias" + name.str()),
						       TString("lowerCurveEtaMinusPhiZeroPlusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaMinusPhiPlusPlusBias[i]  = new TH1F(TString("lowerCurveEtaMinusPhiPlusPlusBias" + name.str()),
						       TString("lowerCurveEtaMinusPhiPlusPlusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaAllPhiMinusPlusBias[i]  = new TH1F(TString("lowerCurveEtaAllPhiMinusPlusBias" + name.str()),
						      TString("lowerCurveEtaAllPhiMinusPlusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaAllPhiZeroPlusBias[i]  = new TH1F(TString("lowerCurveEtaAllPhiZeroPlusBias" + name.str()),
						     TString("lowerCurveEtaAllPhiZeroPlusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);
    h_lowerCurveEtaAllPhiPlusPlusBias[i]  = new TH1F(TString("lowerCurveEtaAllPhiPlusPlusBias" + name.str()),
						     TString("lowerCurveEtaAllPhiPlusPlusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);

    title.str("");
    title.clear();
    title << "#Delta#kappa = -" << (i+1)*(factor_*maxBias/nBiasBins);
    h_lowerCurveMinusBias[i] = new TH1F(TString("lowerCurveMinusBias" + name.str()),TString("lowerCurveMinusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaPlusMinusBias[i] = new TH1F(TString("lowerCurveEtaPlusMinusBias" + name.str()),TString("lowerCurveEtaPlusMinusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaMinusMinusBias[i] = new TH1F(TString("lowerCurveEtaMinusMinusBias" + name.str()),TString("lowerCurveEtaMinusMinusBias" + title.str()),
					symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaPlusPhiMinusMinusBias[i] = new TH1F(TString("lowerCurveEtaPlusPhiMinusMinusBias" + name.str()),
						       TString("lowerCurveEtaPlusPhiMinusMinusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaPlusPhiZeroMinusBias[i] = new TH1F(TString("lowerCurveEtaPlusPhiZeroMinusBias" + name.str()),
						      TString("lowerCurveEtaPlusPhiZeroMinusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaPlusPhiPlusMinusBias[i] = new TH1F(TString("lowerCurveEtaPlusPhiPlusMinusBias" + name.str()),
						      TString("lowerCurveEtaPlusPhiPlusMinusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaMinusPhiMinusMinusBias[i] = new TH1F(TString("lowerCurveEtaMinusPhiMinusMinusBias" + name.str()),
							TString("lowerCurveEtaMinusPhiMinusMinusBias" + title.str()),
							symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaMinusPhiZeroMinusBias[i] = new TH1F(TString("lowerCurveEtaMinusPhiZeroMinusBias" + name.str()),
						       TString("lowerCurveEtaMinusPhiZeroMinusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaMinusPhiPlusMinusBias[i] = new TH1F(TString("lowerCurveEtaMinusPhiPlusMinusBias" + name.str()),
						       TString("lowerCurveEtaMinusPhiPlusMinusBias" + title.str()),
						       symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaAllPhiMinusMinusBias[i] = new TH1F(TString("lowerCurveEtaAllPhiMinusMinusBias" + name.str()),
						      TString("lowerCurveEtaAllPhiMinusMinusBias" + title.str()),
						      symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  

    h_lowerCurveEtaAllPhiZeroMinusBias[i] = new TH1F(TString("lowerCurveEtaAllPhiZeroMinusBias" + name.str()),
						     TString("lowerCurveEtaAllPhiZeroMinusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  
    h_lowerCurveEtaAllPhiPlusMinusBias[i] = new TH1F(TString("lowerCurveEtaAllPhiPlusMinusBias" + name.str()),
						     TString("lowerCurveEtaAllPhiPlusMinusBias" + title.str()),
						     symmetric_ ? 1600 : 800, symmetric_ ? -0.0080*factor_ : 0., 0.0080*factor_);  


  }
  
  // histograms for no cuts
  TH1F *h_muMinusPt      = new TH1F("muMinusPt",     "muMinusPt",      300, 0., 3000.);
  TH1F *h_muMinusPtEtaPlus      = new TH1F("muMinusPtEtaPlus", "muMinusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaMinus      = new TH1F("muMinusPtEtaMinus", "muMinusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaPlusPhiMinus      = new TH1F("muMinusPtEtaPlusPhiMinus", "muMinusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaPlusPhiZero      = new TH1F("muMinusPtEtaPlusPhiZero", "muMinusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaPlusPhiPlus      = new TH1F("muMinusPtEtaPlusPhiPlus", "muMinusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaMinusPhiMinus      = new TH1F("muMinusPtEtaMinusPhiMinus", "muMinusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaMinusPhiZero      = new TH1F("muMinusPtEtaMinusPhiZero", "muMinusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muMinusPtEtaMinusPhiPlus      = new TH1F("muMinusPtEtaMinusPhiPlus", "muMinusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muPlusPt       = new TH1F("muPlusPt",      "muPlusPt",       300, 0., 3000.);
  TH1F *h_muPlusPtEtaPlus      = new TH1F("muPlusPtEtaPlus", "muPlusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaMinus      = new TH1F("muPlusPtEtaMinus", "muPlusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaPlusPhiMinus      = new TH1F("muPlusPtEtaPlusPhiMinus", "muPlusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaPlusPhiZero      = new TH1F("muPlusPtEtaPlusPhiZero", "muPlusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaPlusPhiPlus      = new TH1F("muPlusPtEtaPlusPhiPlus", "muPlusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaMinusPhiMinus      = new TH1F("muPlusPtEtaMinusPhiMinus", "muPlusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaMinusPhiZero      = new TH1F("muPlusPtEtaMinusPhiZero", "muPlusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muPlusPtEtaMinusPhiPlus      = new TH1F("muPlusPtEtaMinusPhiPlus", "muPlusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muUpperMinusPt = new TH1F("muUpperMinusPt","muUpperMinusPt", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaPlus      = new TH1F("muUpperMinusPtEtaPlus", "muUpperMinusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaMinus      = new TH1F("muUpperMinusPtEtaMinus", "muUpperMinusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaPlusPhiMinus      = new TH1F("muUpperMinusPtEtaPlusPhiMinus", "muUpperMinusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaPlusPhiZero      = new TH1F("muUpperMinusPtEtaPlusPhiZero", "muUpperMinusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaPlusPhiPlus      = new TH1F("muUpperMinusPtEtaPlusPhiPlus", "muUpperMinusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaMinusPhiMinus      = new TH1F("muUpperMinusPtEtaMinusPhiMinus", "muUpperMinusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaMinusPhiZero      = new TH1F("muUpperMinusPtEtaMinusPhiZero", "muUpperMinusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperMinusPtEtaMinusPhiPlus      = new TH1F("muUpperMinusPtEtaMinusPhiPlus", "muUpperMinusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muUpperPlusPt  = new TH1F("muUpperPlusPt", "muUpperPlusPt",  300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaPlus      = new TH1F("muUpperPlusPtEtaPlus", "muUpperPlusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaMinus      = new TH1F("muUpperPlusPtEtaMinus", "muUpperPlusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaPlusPhiMinus      = new TH1F("muUpperPlusPtEtaPlusPhiMinus", "muUpperPlusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaPlusPhiZero      = new TH1F("muUpperPlusPtEtaPlusPhiZero", "muUpperPlusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaPlusPhiPlus      = new TH1F("muUpperPlusPtEtaPlusPhiPlus", "muUpperPlusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaMinusPhiMinus      = new TH1F("muUpperPlusPtEtaMinusPhiMinus", "muUpperPlusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaMinusPhiZero      = new TH1F("muUpperPlusPtEtaMinusPhiZero", "muUpperPlusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperPlusPtEtaMinusPhiPlus      = new TH1F("muUpperPlusPtEtaMinusPhiPlus", "muUpperPlusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muLowerMinusPt = new TH1F("muLowerMinusPt","muLowerMinusPt", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaPlus      = new TH1F("muLowerMinusPtEtaPlus", "muLowerMinusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaMinus      = new TH1F("muLowerMinusPtEtaMinus", "muLowerMinusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaPlusPhiMinus      = new TH1F("muLowerMinusPtEtaPlusPhiMinus", "muLowerMinusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaPlusPhiZero      = new TH1F("muLowerMinusPtEtaPlusPhiZero", "muLowerMinusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaPlusPhiPlus      = new TH1F("muLowerMinusPtEtaPlusPhiPlus", "muLowerMinusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaMinusPhiMinus      = new TH1F("muLowerMinusPtEtaMinusPhiMinus", "muLowerMinusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaMinusPhiZero      = new TH1F("muLowerMinusPtEtaMinusPhiZero", "muLowerMinusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerMinusPtEtaMinusPhiPlus      = new TH1F("muLowerMinusPtEtaMinusPhiPlus", "muLowerMinusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muLowerPlusPt  = new TH1F("muLowerPlusPt", "muLowerPlusPt",  300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaPlus      = new TH1F("muLowerPlusPtEtaPlus", "muLowerPlusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaMinus      = new TH1F("muLowerPlusPtEtaMinus", "muLowerPlusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaPlusPhiMinus      = new TH1F("muLowerPlusPtEtaPlusPhiMinus", "muLowerPlusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaPlusPhiZero      = new TH1F("muLowerPlusPtEtaPlusPhiZero", "muLowerPlusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaPlusPhiPlus      = new TH1F("muLowerPlusPtEtaPlusPhiPlus", "muLowerPlusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaMinusPhiMinus      = new TH1F("muLowerPlusPtEtaMinusPhiMinus", "muLowerPlusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaMinusPhiZero      = new TH1F("muLowerPlusPtEtaMinusPhiZero", "muLowerPlusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerPlusPtEtaMinusPhiPlus      = new TH1F("muLowerPlusPtEtaMinusPhiPlus", "muLowerPlusPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_muMinusEta      = new TH1F("muMinusEta",     "muMinusEta",      40, -2., 2.);
  TH1F *h_muMinusEtaEtaPlus      = new TH1F("muMinusEtaEtaPlus", "muMinusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaMinus      = new TH1F("muMinusEtaEtaMinus", "muMinusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaPlusPhiMinus      = new TH1F("muMinusEtaEtaPlusPhiMinus", "muMinusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaPlusPhiZero      = new TH1F("muMinusEtaEtaPlusPhiZero", "muMinusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaPlusPhiPlus      = new TH1F("muMinusEtaEtaPlusPhiPlus", "muMinusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaMinusPhiMinus      = new TH1F("muMinusEtaEtaMinusPhiMinus", "muMinusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaMinusPhiZero      = new TH1F("muMinusEtaEtaMinusPhiZero", "muMinusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muMinusEtaEtaMinusPhiPlus      = new TH1F("muMinusEtaEtaMinusPhiPlus", "muMinusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muPlusEta       = new TH1F("muPlusEta",      "muPlusEta",       40, -2., 2.);
  TH1F *h_muPlusEtaEtaPlus      = new TH1F("muPlusEtaEtaPlus", "muPlusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaMinus      = new TH1F("muPlusEtaEtaMinus", "muPlusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaPlusPhiMinus      = new TH1F("muPlusEtaEtaPlusPhiMinus", "muPlusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaPlusPhiZero      = new TH1F("muPlusEtaEtaPlusPhiZero", "muPlusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaPlusPhiPlus      = new TH1F("muPlusEtaEtaPlusPhiPlus", "muPlusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaMinusPhiMinus      = new TH1F("muPlusEtaEtaMinusPhiMinus", "muPlusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaMinusPhiZero      = new TH1F("muPlusEtaEtaMinusPhiZero", "muPlusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muPlusEtaEtaMinusPhiPlus      = new TH1F("muPlusEtaEtaMinusPhiPlus", "muPlusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muUpperMinusEta = new TH1F("muUpperMinusEta","muUpperMinusEta", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaPlus      = new TH1F("muUpperMinusEtaEtaPlus", "muUpperMinusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaMinus      = new TH1F("muUpperMinusEtaEtaMinus", "muUpperMinusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaPlusPhiMinus      = new TH1F("muUpperMinusEtaEtaPlusPhiMinus", "muUpperMinusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaPlusPhiZero      = new TH1F("muUpperMinusEtaEtaPlusPhiZero", "muUpperMinusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaPlusPhiPlus      = new TH1F("muUpperMinusEtaEtaPlusPhiPlus", "muUpperMinusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaMinusPhiMinus      = new TH1F("muUpperMinusEtaEtaMinusPhiMinus", "muUpperMinusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaMinusPhiZero      = new TH1F("muUpperMinusEtaEtaMinusPhiZero", "muUpperMinusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperMinusEtaEtaMinusPhiPlus      = new TH1F("muUpperMinusEtaEtaMinusPhiPlus", "muUpperMinusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muUpperPlusEta  = new TH1F("muUpperPlusEta", "muUpperPlusEta",  40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaPlus      = new TH1F("muUpperPlusEtaEtaPlus", "muUpperPlusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaMinus      = new TH1F("muUpperPlusEtaEtaMinus", "muUpperPlusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaPlusPhiMinus      = new TH1F("muUpperPlusEtaEtaPlusPhiMinus", "muUpperPlusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaPlusPhiZero      = new TH1F("muUpperPlusEtaEtaPlusPhiZero", "muUpperPlusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaPlusPhiPlus      = new TH1F("muUpperPlusEtaEtaPlusPhiPlus", "muUpperPlusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaMinusPhiMinus      = new TH1F("muUpperPlusEtaEtaMinusPhiMinus", "muUpperPlusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaMinusPhiZero      = new TH1F("muUpperPlusEtaEtaMinusPhiZero", "muUpperPlusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperPlusEtaEtaMinusPhiPlus      = new TH1F("muUpperPlusEtaEtaMinusPhiPlus", "muUpperPlusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muLowerMinusEta = new TH1F("muLowerMinusEta","muLowerMinusEta", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaPlus      = new TH1F("muLowerMinusEtaEtaPlus", "muLowerMinusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaMinus      = new TH1F("muLowerMinusEtaEtaMinus", "muLowerMinusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaPlusPhiMinus      = new TH1F("muLowerMinusEtaEtaPlusPhiMinus", "muLowerMinusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaPlusPhiZero      = new TH1F("muLowerMinusEtaEtaPlusPhiZero", "muLowerMinusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaPlusPhiPlus      = new TH1F("muLowerMinusEtaEtaPlusPhiPlus", "muLowerMinusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaMinusPhiMinus      = new TH1F("muLowerMinusEtaEtaMinusPhiMinus", "muLowerMinusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaMinusPhiZero      = new TH1F("muLowerMinusEtaEtaMinusPhiZero", "muLowerMinusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerMinusEtaEtaMinusPhiPlus      = new TH1F("muLowerMinusEtaEtaMinusPhiPlus", "muLowerMinusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muLowerPlusEta  = new TH1F("muLowerPlusEta", "muLowerPlusEta",  40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaPlus      = new TH1F("muLowerPlusEtaEtaPlus", "muLowerPlusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaMinus      = new TH1F("muLowerPlusEtaEtaMinus", "muLowerPlusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaPlusPhiMinus      = new TH1F("muLowerPlusEtaEtaPlusPhiMinus", "muLowerPlusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaPlusPhiZero      = new TH1F("muLowerPlusEtaEtaPlusPhiZero", "muLowerPlusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaPlusPhiPlus      = new TH1F("muLowerPlusEtaEtaPlusPhiPlus", "muLowerPlusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaMinusPhiMinus      = new TH1F("muLowerPlusEtaEtaMinusPhiMinus", "muLowerPlusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaMinusPhiZero      = new TH1F("muLowerPlusEtaEtaMinusPhiZero", "muLowerPlusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerPlusEtaEtaMinusPhiPlus      = new TH1F("muLowerPlusEtaEtaMinusPhiPlus", "muLowerPlusEtaEtaMinusPhiPlus", 40, -2., 2.);


  TH1F *h_muMinusPhi      = new TH1F("muMinusPhi",     "muMinusPhi",      40, -4., 4.);
  TH1F *h_muMinusPhiEtaPlus      = new TH1F("muMinusPhiEtaPlus", "muMinusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaMinus      = new TH1F("muMinusPhiEtaMinus", "muMinusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaPlusPhiMinus      = new TH1F("muMinusPhiEtaPlusPhiMinus", "muMinusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaPlusPhiZero      = new TH1F("muMinusPhiEtaPlusPhiZero", "muMinusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaPlusPhiPlus      = new TH1F("muMinusPhiEtaPlusPhiPlus", "muMinusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaMinusPhiMinus      = new TH1F("muMinusPhiEtaMinusPhiMinus", "muMinusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaMinusPhiZero      = new TH1F("muMinusPhiEtaMinusPhiZero", "muMinusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muMinusPhiEtaMinusPhiPlus      = new TH1F("muMinusPhiEtaMinusPhiPlus", "muMinusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muPlusPhi       = new TH1F("muPlusPhi",      "muPlusPhi",       40, -4., 4.);
  TH1F *h_muPlusPhiEtaPlus      = new TH1F("muPlusPhiEtaPlus", "muPlusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaMinus      = new TH1F("muPlusPhiEtaMinus", "muPlusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaPlusPhiMinus      = new TH1F("muPlusPhiEtaPlusPhiMinus", "muPlusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaPlusPhiZero      = new TH1F("muPlusPhiEtaPlusPhiZero", "muPlusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaPlusPhiPlus      = new TH1F("muPlusPhiEtaPlusPhiPlus", "muPlusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaMinusPhiMinus      = new TH1F("muPlusPhiEtaMinusPhiMinus", "muPlusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaMinusPhiZero      = new TH1F("muPlusPhiEtaMinusPhiZero", "muPlusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muPlusPhiEtaMinusPhiPlus      = new TH1F("muPlusPhiEtaMinusPhiPlus", "muPlusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muUpperMinusPhi = new TH1F("muUpperMinusPhi","muUpperMinusPhi", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaPlus      = new TH1F("muUpperMinusPhiEtaPlus", "muUpperMinusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaMinus      = new TH1F("muUpperMinusPhiEtaMinus", "muUpperMinusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaPlusPhiMinus      = new TH1F("muUpperMinusPhiEtaPlusPhiMinus", "muUpperMinusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaPlusPhiZero      = new TH1F("muUpperMinusPhiEtaPlusPhiZero", "muUpperMinusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaPlusPhiPlus      = new TH1F("muUpperMinusPhiEtaPlusPhiPlus", "muUpperMinusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaMinusPhiMinus      = new TH1F("muUpperMinusPhiEtaMinusPhiMinus", "muUpperMinusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaMinusPhiZero      = new TH1F("muUpperMinusPhiEtaMinusPhiZero", "muUpperMinusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperMinusPhiEtaMinusPhiPlus      = new TH1F("muUpperMinusPhiEtaMinusPhiPlus", "muUpperMinusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muUpperPlusPhi  = new TH1F("muUpperPlusPhi", "muUpperPlusPhi",  40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaPlus      = new TH1F("muUpperPlusPhiEtaPlus", "muUpperPlusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaMinus      = new TH1F("muUpperPlusPhiEtaMinus", "muUpperPlusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaPlusPhiMinus      = new TH1F("muUpperPlusPhiEtaPlusPhiMinus", "muUpperPlusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaPlusPhiZero      = new TH1F("muUpperPlusPhiEtaPlusPhiZero", "muUpperPlusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaPlusPhiPlus      = new TH1F("muUpperPlusPhiEtaPlusPhiPlus", "muUpperPlusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaMinusPhiMinus      = new TH1F("muUpperPlusPhiEtaMinusPhiMinus", "muUpperPlusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaMinusPhiZero      = new TH1F("muUpperPlusPhiEtaMinusPhiZero", "muUpperPlusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperPlusPhiEtaMinusPhiPlus      = new TH1F("muUpperPlusPhiEtaMinusPhiPlus", "muUpperPlusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muLowerMinusPhi = new TH1F("muLowerMinusPhi","muLowerMinusPhi", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaPlus      = new TH1F("muLowerMinusPhiEtaPlus", "muLowerMinusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaMinus      = new TH1F("muLowerMinusPhiEtaMinus", "muLowerMinusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaPlusPhiMinus      = new TH1F("muLowerMinusPhiEtaPlusPhiMinus", "muLowerMinusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaPlusPhiZero      = new TH1F("muLowerMinusPhiEtaPlusPhiZero", "muLowerMinusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaPlusPhiPlus      = new TH1F("muLowerMinusPhiEtaPlusPhiPlus", "muLowerMinusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaMinusPhiMinus      = new TH1F("muLowerMinusPhiEtaMinusPhiMinus", "muLowerMinusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaMinusPhiZero      = new TH1F("muLowerMinusPhiEtaMinusPhiZero", "muLowerMinusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerMinusPhiEtaMinusPhiPlus      = new TH1F("muLowerMinusPhiEtaMinusPhiPlus", "muLowerMinusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muLowerPlusPhi  = new TH1F("muLowerPlusPhi", "muLowerPlusPhi",  40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaPlus      = new TH1F("muLowerPlusPhiEtaPlus", "muLowerPlusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaMinus      = new TH1F("muLowerPlusPhiEtaMinus", "muLowerPlusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaPlusPhiMinus      = new TH1F("muLowerPlusPhiEtaPlusPhiMinus", "muLowerPlusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaPlusPhiZero      = new TH1F("muLowerPlusPhiEtaPlusPhiZero", "muLowerPlusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaPlusPhiPlus      = new TH1F("muLowerPlusPhiEtaPlusPhiPlus", "muLowerPlusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaMinusPhiMinus      = new TH1F("muLowerPlusPhiEtaMinusPhiMinus", "muLowerPlusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaMinusPhiZero      = new TH1F("muLowerPlusPhiEtaMinusPhiZero", "muLowerPlusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerPlusPhiEtaMinusPhiPlus      = new TH1F("muLowerPlusPhiEtaMinusPhiPlus", "muLowerPlusPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_muMinusChi2      = new TH1F("muMinusChi2",     "muMinusChi2",      50, 0., 150.);
  TH1F *h_muMinusChi2EtaPlus      = new TH1F("muMinusChi2EtaPlus", "muMinusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaMinus      = new TH1F("muMinusChi2EtaMinus", "muMinusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaPlusPhiMinus      = new TH1F("muMinusChi2EtaPlusPhiMinus", "muMinusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaPlusPhiZero      = new TH1F("muMinusChi2EtaPlusPhiZero", "muMinusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaPlusPhiPlus      = new TH1F("muMinusChi2EtaPlusPhiPlus", "muMinusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaMinusPhiMinus      = new TH1F("muMinusChi2EtaMinusPhiMinus", "muMinusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaMinusPhiZero      = new TH1F("muMinusChi2EtaMinusPhiZero", "muMinusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muMinusChi2EtaMinusPhiPlus      = new TH1F("muMinusChi2EtaMinusPhiPlus", "muMinusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muPlusChi2       = new TH1F("muPlusChi2",      "muPlusChi2",       50, 0., 150.);
  TH1F *h_muPlusChi2EtaPlus      = new TH1F("muPlusChi2EtaPlus", "muPlusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaMinus      = new TH1F("muPlusChi2EtaMinus", "muPlusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaPlusPhiMinus      = new TH1F("muPlusChi2EtaPlusPhiMinus", "muPlusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaPlusPhiZero      = new TH1F("muPlusChi2EtaPlusPhiZero", "muPlusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaPlusPhiPlus      = new TH1F("muPlusChi2EtaPlusPhiPlus", "muPlusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaMinusPhiMinus      = new TH1F("muPlusChi2EtaMinusPhiMinus", "muPlusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaMinusPhiZero      = new TH1F("muPlusChi2EtaMinusPhiZero", "muPlusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muPlusChi2EtaMinusPhiPlus      = new TH1F("muPlusChi2EtaMinusPhiPlus", "muPlusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muUpperMinusChi2 = new TH1F("muUpperMinusChi2","muUpperMinusChi2", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaPlus      = new TH1F("muUpperMinusChi2EtaPlus", "muUpperMinusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaMinus      = new TH1F("muUpperMinusChi2EtaMinus", "muUpperMinusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaPlusPhiMinus      = new TH1F("muUpperMinusChi2EtaPlusPhiMinus", "muUpperMinusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaPlusPhiZero      = new TH1F("muUpperMinusChi2EtaPlusPhiZero", "muUpperMinusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaPlusPhiPlus      = new TH1F("muUpperMinusChi2EtaPlusPhiPlus", "muUpperMinusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaMinusPhiMinus      = new TH1F("muUpperMinusChi2EtaMinusPhiMinus", "muUpperMinusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaMinusPhiZero      = new TH1F("muUpperMinusChi2EtaMinusPhiZero", "muUpperMinusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperMinusChi2EtaMinusPhiPlus      = new TH1F("muUpperMinusChi2EtaMinusPhiPlus", "muUpperMinusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muUpperPlusChi2  = new TH1F("muUpperPlusChi2", "muUpperPlusChi2",  50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaPlus      = new TH1F("muUpperPlusChi2EtaPlus", "muUpperPlusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaMinus      = new TH1F("muUpperPlusChi2EtaMinus", "muUpperPlusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaPlusPhiMinus      = new TH1F("muUpperPlusChi2EtaPlusPhiMinus", "muUpperPlusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaPlusPhiZero      = new TH1F("muUpperPlusChi2EtaPlusPhiZero", "muUpperPlusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaPlusPhiPlus      = new TH1F("muUpperPlusChi2EtaPlusPhiPlus", "muUpperPlusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaMinusPhiMinus      = new TH1F("muUpperPlusChi2EtaMinusPhiMinus", "muUpperPlusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaMinusPhiZero      = new TH1F("muUpperPlusChi2EtaMinusPhiZero", "muUpperPlusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperPlusChi2EtaMinusPhiPlus      = new TH1F("muUpperPlusChi2EtaMinusPhiPlus", "muUpperPlusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muLowerMinusChi2 = new TH1F("muLowerMinusChi2","muLowerMinusChi2", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaPlus      = new TH1F("muLowerMinusChi2EtaPlus", "muLowerMinusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaMinus      = new TH1F("muLowerMinusChi2EtaMinus", "muLowerMinusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaPlusPhiMinus      = new TH1F("muLowerMinusChi2EtaPlusPhiMinus", "muLowerMinusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaPlusPhiZero      = new TH1F("muLowerMinusChi2EtaPlusPhiZero", "muLowerMinusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaPlusPhiPlus      = new TH1F("muLowerMinusChi2EtaPlusPhiPlus", "muLowerMinusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaMinusPhiMinus      = new TH1F("muLowerMinusChi2EtaMinusPhiMinus", "muLowerMinusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaMinusPhiZero      = new TH1F("muLowerMinusChi2EtaMinusPhiZero", "muLowerMinusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerMinusChi2EtaMinusPhiPlus      = new TH1F("muLowerMinusChi2EtaMinusPhiPlus", "muLowerMinusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muLowerPlusChi2  = new TH1F("muLowerPlusChi2", "muLowerPlusChi2",  50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaPlus      = new TH1F("muLowerPlusChi2EtaPlus", "muLowerPlusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaMinus      = new TH1F("muLowerPlusChi2EtaMinus", "muLowerPlusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaPlusPhiMinus      = new TH1F("muLowerPlusChi2EtaPlusPhiMinus", "muLowerPlusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaPlusPhiZero      = new TH1F("muLowerPlusChi2EtaPlusPhiZero", "muLowerPlusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaPlusPhiPlus      = new TH1F("muLowerPlusChi2EtaPlusPhiPlus", "muLowerPlusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaMinusPhiMinus      = new TH1F("muLowerPlusChi2EtaMinusPhiMinus", "muLowerPlusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaMinusPhiZero      = new TH1F("muLowerPlusChi2EtaMinusPhiZero", "muLowerPlusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerPlusChi2EtaMinusPhiPlus      = new TH1F("muLowerPlusChi2EtaMinusPhiPlus", "muLowerPlusChi2EtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_muMinusNdof      = new TH1F("muMinusNdof",     "muMinusNdof",      100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaPlus      = new TH1F("muMinusNdofEtaPlus", "muMinusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaMinus      = new TH1F("muMinusNdofEtaMinus", "muMinusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaPlusPhiMinus      = new TH1F("muMinusNdofEtaPlusPhiMinus", "muMinusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaPlusPhiZero      = new TH1F("muMinusNdofEtaPlusPhiZero", "muMinusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaPlusPhiPlus      = new TH1F("muMinusNdofEtaPlusPhiPlus", "muMinusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaMinusPhiMinus      = new TH1F("muMinusNdofEtaMinusPhiMinus", "muMinusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaMinusPhiZero      = new TH1F("muMinusNdofEtaMinusPhiZero", "muMinusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muMinusNdofEtaMinusPhiPlus      = new TH1F("muMinusNdofEtaMinusPhiPlus", "muMinusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muPlusNdof       = new TH1F("muPlusNdof",      "muPlusNdof",       100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaPlus      = new TH1F("muPlusNdofEtaPlus", "muPlusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaMinus      = new TH1F("muPlusNdofEtaMinus", "muPlusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaPlusPhiMinus      = new TH1F("muPlusNdofEtaPlusPhiMinus", "muPlusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaPlusPhiZero      = new TH1F("muPlusNdofEtaPlusPhiZero", "muPlusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaPlusPhiPlus      = new TH1F("muPlusNdofEtaPlusPhiPlus", "muPlusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaMinusPhiMinus      = new TH1F("muPlusNdofEtaMinusPhiMinus", "muPlusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaMinusPhiZero      = new TH1F("muPlusNdofEtaMinusPhiZero", "muPlusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muPlusNdofEtaMinusPhiPlus      = new TH1F("muPlusNdofEtaMinusPhiPlus", "muPlusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muUpperMinusNdof = new TH1F("muUpperMinusNdof","muUpperMinusNdof", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaPlus      = new TH1F("muUpperMinusNdofEtaPlus", "muUpperMinusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaMinus      = new TH1F("muUpperMinusNdofEtaMinus", "muUpperMinusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaPlusPhiMinus      = new TH1F("muUpperMinusNdofEtaPlusPhiMinus", "muUpperMinusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaPlusPhiZero      = new TH1F("muUpperMinusNdofEtaPlusPhiZero", "muUpperMinusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaPlusPhiPlus      = new TH1F("muUpperMinusNdofEtaPlusPhiPlus", "muUpperMinusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaMinusPhiMinus      = new TH1F("muUpperMinusNdofEtaMinusPhiMinus", "muUpperMinusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaMinusPhiZero      = new TH1F("muUpperMinusNdofEtaMinusPhiZero", "muUpperMinusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusNdofEtaMinusPhiPlus      = new TH1F("muUpperMinusNdofEtaMinusPhiPlus", "muUpperMinusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muUpperPlusNdof  = new TH1F("muUpperPlusNdof", "muUpperPlusNdof",  100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaPlus      = new TH1F("muUpperPlusNdofEtaPlus", "muUpperPlusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaMinus      = new TH1F("muUpperPlusNdofEtaMinus", "muUpperPlusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaPlusPhiMinus      = new TH1F("muUpperPlusNdofEtaPlusPhiMinus", "muUpperPlusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaPlusPhiZero      = new TH1F("muUpperPlusNdofEtaPlusPhiZero", "muUpperPlusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaPlusPhiPlus      = new TH1F("muUpperPlusNdofEtaPlusPhiPlus", "muUpperPlusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaMinusPhiMinus      = new TH1F("muUpperPlusNdofEtaMinusPhiMinus", "muUpperPlusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaMinusPhiZero      = new TH1F("muUpperPlusNdofEtaMinusPhiZero", "muUpperPlusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusNdofEtaMinusPhiPlus      = new TH1F("muUpperPlusNdofEtaMinusPhiPlus", "muUpperPlusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muLowerMinusNdof = new TH1F("muLowerMinusNdof","muLowerMinusNdof", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaPlus      = new TH1F("muLowerMinusNdofEtaPlus", "muLowerMinusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaMinus      = new TH1F("muLowerMinusNdofEtaMinus", "muLowerMinusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaPlusPhiMinus      = new TH1F("muLowerMinusNdofEtaPlusPhiMinus", "muLowerMinusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaPlusPhiZero      = new TH1F("muLowerMinusNdofEtaPlusPhiZero", "muLowerMinusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaPlusPhiPlus      = new TH1F("muLowerMinusNdofEtaPlusPhiPlus", "muLowerMinusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaMinusPhiMinus      = new TH1F("muLowerMinusNdofEtaMinusPhiMinus", "muLowerMinusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaMinusPhiZero      = new TH1F("muLowerMinusNdofEtaMinusPhiZero", "muLowerMinusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusNdofEtaMinusPhiPlus      = new TH1F("muLowerMinusNdofEtaMinusPhiPlus", "muLowerMinusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muLowerPlusNdof  = new TH1F("muLowerPlusNdof", "muLowerPlusNdof",  100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaPlus      = new TH1F("muLowerPlusNdofEtaPlus", "muLowerPlusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaMinus      = new TH1F("muLowerPlusNdofEtaMinus", "muLowerPlusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaPlusPhiMinus      = new TH1F("muLowerPlusNdofEtaPlusPhiMinus", "muLowerPlusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaPlusPhiZero      = new TH1F("muLowerPlusNdofEtaPlusPhiZero", "muLowerPlusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaPlusPhiPlus      = new TH1F("muLowerPlusNdofEtaPlusPhiPlus", "muLowerPlusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaMinusPhiMinus      = new TH1F("muLowerPlusNdofEtaMinusPhiMinus", "muLowerPlusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaMinusPhiZero      = new TH1F("muLowerPlusNdofEtaMinusPhiZero", "muLowerPlusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusNdofEtaMinusPhiPlus      = new TH1F("muLowerPlusNdofEtaMinusPhiPlus", "muLowerPlusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_muMinusCharge      = new TH1F("muMinusCharge",     "muMinusCharge",      3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaPlus      = new TH1F("muMinusChargeEtaPlus", "muMinusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaMinus      = new TH1F("muMinusChargeEtaMinus", "muMinusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaPlusPhiMinus      = new TH1F("muMinusChargeEtaPlusPhiMinus", "muMinusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaPlusPhiZero      = new TH1F("muMinusChargeEtaPlusPhiZero", "muMinusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaPlusPhiPlus      = new TH1F("muMinusChargeEtaPlusPhiPlus", "muMinusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaMinusPhiMinus      = new TH1F("muMinusChargeEtaMinusPhiMinus", "muMinusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaMinusPhiZero      = new TH1F("muMinusChargeEtaMinusPhiZero", "muMinusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muMinusChargeEtaMinusPhiPlus      = new TH1F("muMinusChargeEtaMinusPhiPlus", "muMinusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_muPlusCharge       = new TH1F("muPlusCharge",      "muPlusCharge",       3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaPlus      = new TH1F("muPlusChargeEtaPlus", "muPlusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaMinus      = new TH1F("muPlusChargeEtaMinus", "muPlusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaPlusPhiMinus      = new TH1F("muPlusChargeEtaPlusPhiMinus", "muPlusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaPlusPhiZero      = new TH1F("muPlusChargeEtaPlusPhiZero", "muPlusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaPlusPhiPlus      = new TH1F("muPlusChargeEtaPlusPhiPlus", "muPlusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaMinusPhiMinus      = new TH1F("muPlusChargeEtaMinusPhiMinus", "muPlusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaMinusPhiZero      = new TH1F("muPlusChargeEtaMinusPhiZero", "muPlusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muPlusChargeEtaMinusPhiPlus      = new TH1F("muPlusChargeEtaMinusPhiPlus", "muPlusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_muUpperMinusCharge = new TH1F("muUpperMinusCharge","muUpperMinusCharge", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaPlus      = new TH1F("muUpperMinusChargeEtaPlus", "muUpperMinusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaMinus      = new TH1F("muUpperMinusChargeEtaMinus", "muUpperMinusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaPlusPhiMinus      = new TH1F("muUpperMinusChargeEtaPlusPhiMinus", "muUpperMinusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaPlusPhiZero      = new TH1F("muUpperMinusChargeEtaPlusPhiZero", "muUpperMinusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaPlusPhiPlus      = new TH1F("muUpperMinusChargeEtaPlusPhiPlus", "muUpperMinusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaMinusPhiMinus      = new TH1F("muUpperMinusChargeEtaMinusPhiMinus", "muUpperMinusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaMinusPhiZero      = new TH1F("muUpperMinusChargeEtaMinusPhiZero", "muUpperMinusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muUpperMinusChargeEtaMinusPhiPlus      = new TH1F("muUpperMinusChargeEtaMinusPhiPlus", "muUpperMinusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_muUpperPlusCharge  = new TH1F("muUpperPlusCharge", "muUpperPlusCharge",  3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaPlus      = new TH1F("muUpperPlusChargeEtaPlus", "muUpperPlusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaMinus      = new TH1F("muUpperPlusChargeEtaMinus", "muUpperPlusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaPlusPhiMinus      = new TH1F("muUpperPlusChargeEtaPlusPhiMinus", "muUpperPlusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaPlusPhiZero      = new TH1F("muUpperPlusChargeEtaPlusPhiZero", "muUpperPlusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaPlusPhiPlus      = new TH1F("muUpperPlusChargeEtaPlusPhiPlus", "muUpperPlusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaMinusPhiMinus      = new TH1F("muUpperPlusChargeEtaMinusPhiMinus", "muUpperPlusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaMinusPhiZero      = new TH1F("muUpperPlusChargeEtaMinusPhiZero", "muUpperPlusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muUpperPlusChargeEtaMinusPhiPlus      = new TH1F("muUpperPlusChargeEtaMinusPhiPlus", "muUpperPlusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_muLowerMinusCharge = new TH1F("muLowerMinusCharge","muLowerMinusCharge", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaPlus      = new TH1F("muLowerMinusChargeEtaPlus", "muLowerMinusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaMinus      = new TH1F("muLowerMinusChargeEtaMinus", "muLowerMinusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaPlusPhiMinus      = new TH1F("muLowerMinusChargeEtaPlusPhiMinus", "muLowerMinusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaPlusPhiZero      = new TH1F("muLowerMinusChargeEtaPlusPhiZero", "muLowerMinusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaPlusPhiPlus      = new TH1F("muLowerMinusChargeEtaPlusPhiPlus", "muLowerMinusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaMinusPhiMinus      = new TH1F("muLowerMinusChargeEtaMinusPhiMinus", "muLowerMinusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaMinusPhiZero      = new TH1F("muLowerMinusChargeEtaMinusPhiZero", "muLowerMinusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muLowerMinusChargeEtaMinusPhiPlus      = new TH1F("muLowerMinusChargeEtaMinusPhiPlus", "muLowerMinusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_muLowerPlusCharge  = new TH1F("muLowerPlusCharge", "muLowerPlusCharge",  3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaPlus      = new TH1F("muLowerPlusChargeEtaPlus", "muLowerPlusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaMinus      = new TH1F("muLowerPlusChargeEtaMinus", "muLowerPlusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaPlusPhiMinus      = new TH1F("muLowerPlusChargeEtaPlusPhiMinus", "muLowerPlusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaPlusPhiZero      = new TH1F("muLowerPlusChargeEtaPlusPhiZero", "muLowerPlusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaPlusPhiPlus      = new TH1F("muLowerPlusChargeEtaPlusPhiPlus", "muLowerPlusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaMinusPhiMinus      = new TH1F("muLowerPlusChargeEtaMinusPhiMinus", "muLowerPlusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaMinusPhiZero      = new TH1F("muLowerPlusChargeEtaMinusPhiZero", "muLowerPlusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_muLowerPlusChargeEtaMinusPhiPlus      = new TH1F("muLowerPlusChargeEtaMinusPhiPlus", "muLowerPlusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);


  TH1F *h_muMinusCurve      = new TH1F("muMinusCurve",     "muMinusCurve",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaPlus      = new TH1F("muMinusCurveEtaPlus",     "muMinusCurveEtaPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaMinus      = new TH1F("muMinusCurveEtaMinus",     "muMinusCurveEtaMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaPlusPhiMinus      = new TH1F("muMinusCurveEtaPlusPhiMinus",     "muMinusCurveEtaPlusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaPlusPhiZero      = new TH1F("muMinusCurveEtaPlusPhiZero",     "muMinusCurveEtaPlusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaPlusPhiPlus      = new TH1F("muMinusCurveEtaPlusPhiPlus",     "muMinusCurveEtaPlusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaMinusPhiMinus      = new TH1F("muMinusCurveEtaMinusPhiMinus",     "muMinusCurveEtaMinusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaMinusPhiZero      = new TH1F("muMinusCurveEtaMinusPhiZero",     "muMinusCurveEtaMinusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaMinusPhiPlus      = new TH1F("muMinusCurveEtaMinusPhiPlus",     "muMinusCurveEtaMinusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaAllPhiMinus      = new TH1F("muMinusCurveEtaAllPhiMinus",     "muMinusCurveEtaAllPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaAllPhiZero      = new TH1F("muMinusCurveEtaAllPhiZero",     "muMinusCurveEtaAllPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muMinusCurveEtaAllPhiPlus      = new TH1F("muMinusCurveEtaAllPhiPlus",     "muMinusCurveEtaAllPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_muPlusCurve       = new TH1F("muPlusCurve",      "muPlusCurve",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaPlus       = new TH1F("muPlusCurveEtaPlus",      "muPlusCurveEtaPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaMinus       = new TH1F("muPlusCurveEtaMinus",      "muPlusCurveEtaMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaPlusPhiMinus       = new TH1F("muPlusCurveEtaPlusPhiMinus",      "muPlusCurveEtaPlusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaPlusPhiZero       = new TH1F("muPlusCurveEtaPlusPhiZero",      "muPlusCurveEtaPlusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaPlusPhiPlus       = new TH1F("muPlusCurveEtaPlusPhiPlus",      "muPlusCurveEtaPlusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaMinusPhiMinus       = new TH1F("muPlusCurveEtaMinusPhiMinus",      "muPlusCurveEtaMinusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaMinusPhiZero       = new TH1F("muPlusCurveEtaMinusPhiZero",      "muPlusCurveEtaMinusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaMinusPhiPlus       = new TH1F("muPlusCurveEtaMinusPhiPlus",      "muPlusCurveEtaMinusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaAllPhiMinus       = new TH1F("muPlusCurveEtaAllPhiMinus",      "muPlusCurveEtaAllPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaAllPhiZero       = new TH1F("muPlusCurveEtaAllPhiZero",      "muPlusCurveEtaAllPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muPlusCurveEtaAllPhiPlus       = new TH1F("muPlusCurveEtaAllPhiPlus",      "muPlusCurveEtaAllPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_muUpperMinusCurve = new TH1F("muUpperMinusCurve","muUpperMinusCurve",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaPlus = new TH1F("muUpperMinusCurveEtaPlus","muUpperMinusCurveEtaPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaMinus = new TH1F("muUpperMinusCurveEtaMinus","muUpperMinusCurveEtaMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaPlusPhiMinus = new TH1F("muUpperMinusCurveEtaPlusPhiMinus","muUpperMinusCurveEtaPlusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaPlusPhiZero = new TH1F("muUpperMinusCurveEtaPlusPhiZero","muUpperMinusCurveEtaPlusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaPlusPhiPlus = new TH1F("muUpperMinusCurveEtaPlusPhiPlus","muUpperMinusCurveEtaPlusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaMinusPhiMinus = new TH1F("muUpperMinusCurveEtaMinusPhiMinus","muUpperMinusCurveEtaMinusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaMinusPhiZero = new TH1F("muUpperMinusCurveEtaMinusPhiZero","muUpperMinusCurveEtaMinusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaMinusPhiPlus = new TH1F("muUpperMinusCurveEtaMinusPhiPlus","muUpperMinusCurveEtaMinusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaAllPhiMinus = new TH1F("muUpperMinusCurveEtaAllPhiMinus","muUpperMinusCurveEtaAllPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaAllPhiZero = new TH1F("muUpperMinusCurveEtaAllPhiZero","muUpperMinusCurveEtaAllPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperMinusCurveEtaAllPhiPlus = new TH1F("muUpperMinusCurveEtaAllPhiPlus","muUpperMinusCurveEtaAllPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_muUpperPlusCurve = new TH1F("muUpperPlusCurve", "muUpperPlusCurve",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaPlus = new TH1F("muUpperPlusCurveEtaPlus", "muUpperPlusCurveEtaPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaMinus = new TH1F("muUpperPlusCurveEtaMinus", "muUpperPlusCurveEtaMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaPlusPhiMinus = new TH1F("muUpperPlusCurveEtaPlusPhiMinus", "muUpperPlusCurveEtaPlusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaPlusPhiZero = new TH1F("muUpperPlusCurveEtaPlusPhiZero", "muUpperPlusCurveEtaPlusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaPlusPhiPlus = new TH1F("muUpperPlusCurveEtaPlusPhiPlus", "muUpperPlusCurveEtaPlusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaMinusPhiMinus = new TH1F("muUpperPlusCurveEtaMinusPhiMinus", "muUpperPlusCurveEtaMinusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaMinusPhiZero = new TH1F("muUpperPlusCurveEtaMinusPhiZero", "muUpperPlusCurveEtaMinusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaMinusPhiPlus = new TH1F("muUpperPlusCurveEtaMinusPhiPlus", "muUpperPlusCurveEtaMinusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaAllPhiMinus = new TH1F("muUpperPlusCurveEtaAllPhiMinus", "muUpperPlusCurveEtaAllPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaAllPhiZero = new TH1F("muUpperPlusCurveEtaAllPhiZero", "muUpperPlusCurveEtaAllPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muUpperPlusCurveEtaAllPhiPlus = new TH1F("muUpperPlusCurveEtaAllPhiPlus", "muUpperPlusCurveEtaAllPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_muLowerMinusCurve = new TH1F("muLowerMinusCurve","muLowerMinusCurve",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaPlus = new TH1F("muLowerMinusCurveEtaPlus","muLowerMinusCurveEtaPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaMinus = new TH1F("muLowerMinusCurveEtaMinus","muLowerMinusCurveEtaMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaPlusPhiMinus = new TH1F("muLowerMinusCurveEtaPlusPhiMinus","muLowerMinusCurveEtaPlusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaPlusPhiZero = new TH1F("muLowerMinusCurveEtaPlusPhiZero","muLowerMinusCurveEtaPlusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaPlusPhiPlus = new TH1F("muLowerMinusCurveEtaPlusPhiPlus","muLowerMinusCurveEtaPlusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaMinusPhiMinus = new TH1F("muLowerMinusCurveEtaMinusPhiMinus","muLowerMinusCurveEtaMinusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaMinusPhiZero = new TH1F("muLowerMinusCurveEtaMinusPhiZero","muLowerMinusCurveEtaMinusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaMinusPhiPlus = new TH1F("muLowerMinusCurveEtaMinusPhiPlus","muLowerMinusCurveEtaMinusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaAllPhiMinus = new TH1F("muLowerMinusCurveEtaAllPhiMinus","muLowerMinusCurveEtaAllPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaAllPhiZero = new TH1F("muLowerMinusCurveEtaAllPhiZero","muLowerMinusCurveEtaAllPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerMinusCurveEtaAllPhiPlus = new TH1F("muLowerMinusCurveEtaAllPhiPlus","muLowerMinusCurveEtaAllPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);

  TH1F *h_muLowerPlusCurve  = new TH1F("muLowerPlusCurve", "muLowerPlusCurve",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaPlus  = new TH1F("muLowerPlusCurveEtaPlus", "muLowerPlusCurveEtaPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaMinus  = new TH1F("muLowerPlusCurveEtaMinus", "muLowerPlusCurveEtaMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaPlusPhiMinus  = new TH1F("muLowerPlusCurveEtaPlusPhiMinus", "muLowerPlusCurveEtaPlusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaPlusPhiZero  = new TH1F("muLowerPlusCurveEtaPlusPhiZero", "muLowerPlusCurveEtaPlusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaPlusPhiPlus  = new TH1F("muLowerPlusCurveEtaPlusPhiPlus", "muLowerPlusCurveEtaPlusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaMinusPhiMinus  = new TH1F("muLowerPlusCurveEtaMinusPhiMinus", "muLowerPlusCurveEtaMinusPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaMinusPhiZero  = new TH1F("muLowerPlusCurveEtaMinusPhiZero", "muLowerPlusCurveEtaMinusPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaMinusPhiPlus  = new TH1F("muLowerPlusCurveEtaMinusPhiPlus", "muLowerPlusCurveEtaMinusPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaAllPhiMinus  = new TH1F("muLowerPlusCurveEtaAllPhiMinus", "muLowerPlusCurveEtaAllPhiMinus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaAllPhiZero  = new TH1F("muLowerPlusCurveEtaAllPhiZero", "muLowerPlusCurveEtaAllPhiZero",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_muLowerPlusCurveEtaAllPhiPlus  = new TH1F("muLowerPlusCurveEtaAllPhiPlus", "muLowerPlusCurveEtaAllPhiPlus",
				       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_muMinusDxy      = new TH1F("muMinusDxy",     "muMinusDxy",      100, -100., 100.);
  TH1F *h_muMinusDxyEtaPlus      = new TH1F("muMinusDxyEtaPlus", "muMinusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaMinus      = new TH1F("muMinusDxyEtaMinus", "muMinusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaPlusPhiMinus      = new TH1F("muMinusDxyEtaPlusPhiMinus", "muMinusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaPlusPhiZero      = new TH1F("muMinusDxyEtaPlusPhiZero", "muMinusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaPlusPhiPlus      = new TH1F("muMinusDxyEtaPlusPhiPlus", "muMinusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaMinusPhiMinus      = new TH1F("muMinusDxyEtaMinusPhiMinus", "muMinusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaMinusPhiZero      = new TH1F("muMinusDxyEtaMinusPhiZero", "muMinusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_muMinusDxyEtaMinusPhiPlus      = new TH1F("muMinusDxyEtaMinusPhiPlus", "muMinusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_muPlusDxy       = new TH1F("muPlusDxy",      "muPlusDxy",       100, -100., 100.);
  TH1F *h_muPlusDxyEtaPlus      = new TH1F("muPlusDxyEtaPlus", "muPlusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaMinus      = new TH1F("muPlusDxyEtaMinus", "muPlusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaPlusPhiMinus      = new TH1F("muPlusDxyEtaPlusPhiMinus", "muPlusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaPlusPhiZero      = new TH1F("muPlusDxyEtaPlusPhiZero", "muPlusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaPlusPhiPlus      = new TH1F("muPlusDxyEtaPlusPhiPlus", "muPlusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaMinusPhiMinus      = new TH1F("muPlusDxyEtaMinusPhiMinus", "muPlusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaMinusPhiZero      = new TH1F("muPlusDxyEtaMinusPhiZero", "muPlusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_muPlusDxyEtaMinusPhiPlus      = new TH1F("muPlusDxyEtaMinusPhiPlus", "muPlusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_muUpperMinusDxy = new TH1F("muUpperMinusDxy","muUpperMinusDxy", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaPlus      = new TH1F("muUpperMinusDxyEtaPlus", "muUpperMinusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaMinus      = new TH1F("muUpperMinusDxyEtaMinus", "muUpperMinusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaPlusPhiMinus      = new TH1F("muUpperMinusDxyEtaPlusPhiMinus", "muUpperMinusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaPlusPhiZero      = new TH1F("muUpperMinusDxyEtaPlusPhiZero", "muUpperMinusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaPlusPhiPlus      = new TH1F("muUpperMinusDxyEtaPlusPhiPlus", "muUpperMinusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaMinusPhiMinus      = new TH1F("muUpperMinusDxyEtaMinusPhiMinus", "muUpperMinusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaMinusPhiZero      = new TH1F("muUpperMinusDxyEtaMinusPhiZero", "muUpperMinusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_muUpperMinusDxyEtaMinusPhiPlus      = new TH1F("muUpperMinusDxyEtaMinusPhiPlus", "muUpperMinusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_muUpperPlusDxy  = new TH1F("muUpperPlusDxy", "muUpperPlusDxy",  100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaPlus      = new TH1F("muUpperPlusDxyEtaPlus", "muUpperPlusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaMinus      = new TH1F("muUpperPlusDxyEtaMinus", "muUpperPlusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaPlusPhiMinus      = new TH1F("muUpperPlusDxyEtaPlusPhiMinus", "muUpperPlusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaPlusPhiZero      = new TH1F("muUpperPlusDxyEtaPlusPhiZero", "muUpperPlusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaPlusPhiPlus      = new TH1F("muUpperPlusDxyEtaPlusPhiPlus", "muUpperPlusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaMinusPhiMinus      = new TH1F("muUpperPlusDxyEtaMinusPhiMinus", "muUpperPlusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaMinusPhiZero      = new TH1F("muUpperPlusDxyEtaMinusPhiZero", "muUpperPlusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_muUpperPlusDxyEtaMinusPhiPlus      = new TH1F("muUpperPlusDxyEtaMinusPhiPlus", "muUpperPlusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_muLowerMinusDxy = new TH1F("muLowerMinusDxy","muLowerMinusDxy", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaPlus      = new TH1F("muLowerMinusDxyEtaPlus", "muLowerMinusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaMinus      = new TH1F("muLowerMinusDxyEtaMinus", "muLowerMinusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaPlusPhiMinus      = new TH1F("muLowerMinusDxyEtaPlusPhiMinus", "muLowerMinusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaPlusPhiZero      = new TH1F("muLowerMinusDxyEtaPlusPhiZero", "muLowerMinusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaPlusPhiPlus      = new TH1F("muLowerMinusDxyEtaPlusPhiPlus", "muLowerMinusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaMinusPhiMinus      = new TH1F("muLowerMinusDxyEtaMinusPhiMinus", "muLowerMinusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaMinusPhiZero      = new TH1F("muLowerMinusDxyEtaMinusPhiZero", "muLowerMinusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_muLowerMinusDxyEtaMinusPhiPlus      = new TH1F("muLowerMinusDxyEtaMinusPhiPlus", "muLowerMinusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_muLowerPlusDxy  = new TH1F("muLowerPlusDxy", "muLowerPlusDxy",  100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaPlus      = new TH1F("muLowerPlusDxyEtaPlus", "muLowerPlusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaMinus      = new TH1F("muLowerPlusDxyEtaMinus", "muLowerPlusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaPlusPhiMinus      = new TH1F("muLowerPlusDxyEtaPlusPhiMinus", "muLowerPlusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaPlusPhiZero      = new TH1F("muLowerPlusDxyEtaPlusPhiZero", "muLowerPlusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaPlusPhiPlus      = new TH1F("muLowerPlusDxyEtaPlusPhiPlus", "muLowerPlusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaMinusPhiMinus      = new TH1F("muLowerPlusDxyEtaMinusPhiMinus", "muLowerPlusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaMinusPhiZero      = new TH1F("muLowerPlusDxyEtaMinusPhiZero", "muLowerPlusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_muLowerPlusDxyEtaMinusPhiPlus      = new TH1F("muLowerPlusDxyEtaMinusPhiPlus", "muLowerPlusDxyEtaMinusPhiPlus", 100, -100., 100.);


  TH1F *h_muMinusDz       = new TH1F("muMinusDz",     "muMinusDz",        100, -250., 250.);
  TH1F *h_muMinusDzEtaPlus      = new TH1F("muMinusDzEtaPlus", "muMinusDzEtaPlus", 100, -250., 250.);
  TH1F *h_muMinusDzEtaMinus      = new TH1F("muMinusDzEtaMinus", "muMinusDzEtaMinus", 100, -250., 250.);
  TH1F *h_muMinusDzEtaPlusPhiMinus      = new TH1F("muMinusDzEtaPlusPhiMinus", "muMinusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_muMinusDzEtaPlusPhiZero      = new TH1F("muMinusDzEtaPlusPhiZero", "muMinusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_muMinusDzEtaPlusPhiPlus      = new TH1F("muMinusDzEtaPlusPhiPlus", "muMinusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_muMinusDzEtaMinusPhiMinus      = new TH1F("muMinusDzEtaMinusPhiMinus", "muMinusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_muMinusDzEtaMinusPhiZero      = new TH1F("muMinusDzEtaMinusPhiZero", "muMinusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_muMinusDzEtaMinusPhiPlus      = new TH1F("muMinusDzEtaMinusPhiPlus", "muMinusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_muPlusDz        = new TH1F("muPlusDz",      "muPlusDz",         100, -250., 250.);
  TH1F *h_muPlusDzEtaPlus      = new TH1F("muPlusDzEtaPlus", "muPlusDzEtaPlus", 100, -250., 250.);
  TH1F *h_muPlusDzEtaMinus      = new TH1F("muPlusDzEtaMinus", "muPlusDzEtaMinus", 100, -250., 250.);
  TH1F *h_muPlusDzEtaPlusPhiMinus      = new TH1F("muPlusDzEtaPlusPhiMinus", "muPlusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_muPlusDzEtaPlusPhiZero      = new TH1F("muPlusDzEtaPlusPhiZero", "muPlusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_muPlusDzEtaPlusPhiPlus      = new TH1F("muPlusDzEtaPlusPhiPlus", "muPlusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_muPlusDzEtaMinusPhiMinus      = new TH1F("muPlusDzEtaMinusPhiMinus", "muPlusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_muPlusDzEtaMinusPhiZero      = new TH1F("muPlusDzEtaMinusPhiZero", "muPlusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_muPlusDzEtaMinusPhiPlus      = new TH1F("muPlusDzEtaMinusPhiPlus", "muPlusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_muUpperMinusDz  = new TH1F("muUpperMinusDz","muUpperMinusDz",   100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaPlus      = new TH1F("muUpperMinusDzEtaPlus", "muUpperMinusDzEtaPlus", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaMinus      = new TH1F("muUpperMinusDzEtaMinus", "muUpperMinusDzEtaMinus", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaPlusPhiMinus      = new TH1F("muUpperMinusDzEtaPlusPhiMinus", "muUpperMinusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaPlusPhiZero      = new TH1F("muUpperMinusDzEtaPlusPhiZero", "muUpperMinusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaPlusPhiPlus      = new TH1F("muUpperMinusDzEtaPlusPhiPlus", "muUpperMinusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaMinusPhiMinus      = new TH1F("muUpperMinusDzEtaMinusPhiMinus", "muUpperMinusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaMinusPhiZero      = new TH1F("muUpperMinusDzEtaMinusPhiZero", "muUpperMinusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_muUpperMinusDzEtaMinusPhiPlus      = new TH1F("muUpperMinusDzEtaMinusPhiPlus", "muUpperMinusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_muUpperPlusDz   = new TH1F("muUpperPlusDz", "muUpperPlusDz",    100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaPlus      = new TH1F("muUpperPlusDzEtaPlus", "muUpperPlusDzEtaPlus", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaMinus      = new TH1F("muUpperPlusDzEtaMinus", "muUpperPlusDzEtaMinus", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaPlusPhiMinus      = new TH1F("muUpperPlusDzEtaPlusPhiMinus", "muUpperPlusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaPlusPhiZero      = new TH1F("muUpperPlusDzEtaPlusPhiZero", "muUpperPlusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaPlusPhiPlus      = new TH1F("muUpperPlusDzEtaPlusPhiPlus", "muUpperPlusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaMinusPhiMinus      = new TH1F("muUpperPlusDzEtaMinusPhiMinus", "muUpperPlusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaMinusPhiZero      = new TH1F("muUpperPlusDzEtaMinusPhiZero", "muUpperPlusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_muUpperPlusDzEtaMinusPhiPlus      = new TH1F("muUpperPlusDzEtaMinusPhiPlus", "muUpperPlusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_muLowerMinusDz  = new TH1F("muLowerMinusDz","muLowerMinusDz",   100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaPlus      = new TH1F("muLowerMinusDzEtaPlus", "muLowerMinusDzEtaPlus", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaMinus      = new TH1F("muLowerMinusDzEtaMinus", "muLowerMinusDzEtaMinus", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaPlusPhiMinus      = new TH1F("muLowerMinusDzEtaPlusPhiMinus", "muLowerMinusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaPlusPhiZero      = new TH1F("muLowerMinusDzEtaPlusPhiZero", "muLowerMinusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaPlusPhiPlus      = new TH1F("muLowerMinusDzEtaPlusPhiPlus", "muLowerMinusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaMinusPhiMinus      = new TH1F("muLowerMinusDzEtaMinusPhiMinus", "muLowerMinusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaMinusPhiZero      = new TH1F("muLowerMinusDzEtaMinusPhiZero", "muLowerMinusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_muLowerMinusDzEtaMinusPhiPlus      = new TH1F("muLowerMinusDzEtaMinusPhiPlus", "muLowerMinusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_muLowerPlusDz   = new TH1F("muLowerPlusDz", "muLowerPlusDz",    100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaPlus      = new TH1F("muLowerPlusDzEtaPlus", "muLowerPlusDzEtaPlus", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaMinus      = new TH1F("muLowerPlusDzEtaMinus", "muLowerPlusDzEtaMinus", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaPlusPhiMinus      = new TH1F("muLowerPlusDzEtaPlusPhiMinus", "muLowerPlusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaPlusPhiZero      = new TH1F("muLowerPlusDzEtaPlusPhiZero", "muLowerPlusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaPlusPhiPlus      = new TH1F("muLowerPlusDzEtaPlusPhiPlus", "muLowerPlusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaMinusPhiMinus      = new TH1F("muLowerPlusDzEtaMinusPhiMinus", "muLowerPlusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaMinusPhiZero      = new TH1F("muLowerPlusDzEtaMinusPhiZero", "muLowerPlusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_muLowerPlusDzEtaMinusPhiPlus      = new TH1F("muLowerPlusDzEtaMinusPhiPlus", "muLowerPlusDzEtaMinusPhiPlus", 100, -250., 250.);


  TH1F *h_muMinusDxyError      = new TH1F("muMinusDxyError",     "muMinusDxyError",      50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaPlus      = new TH1F("muMinusDxyErrorEtaPlus", "muMinusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaMinus      = new TH1F("muMinusDxyErrorEtaMinus", "muMinusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaPlusPhiMinus      = new TH1F("muMinusDxyErrorEtaPlusPhiMinus", "muMinusDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaPlusPhiZero      = new TH1F("muMinusDxyErrorEtaPlusPhiZero", "muMinusDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaPlusPhiPlus      = new TH1F("muMinusDxyErrorEtaPlusPhiPlus", "muMinusDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaMinusPhiMinus      = new TH1F("muMinusDxyErrorEtaMinusPhiMinus", "muMinusDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaMinusPhiZero      = new TH1F("muMinusDxyErrorEtaMinusPhiZero", "muMinusDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muMinusDxyErrorEtaMinusPhiPlus      = new TH1F("muMinusDxyErrorEtaMinusPhiPlus", "muMinusDxyErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muPlusDxyError       = new TH1F("muPlusDxyError",      "muPlusDxyError",       50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaPlus      = new TH1F("muPlusDxyErrorEtaPlus", "muPlusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaMinus      = new TH1F("muPlusDxyErrorEtaMinus", "muPlusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaPlusPhiMinus      = new TH1F("muPlusDxyErrorEtaPlusPhiMinus", "muPlusDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaPlusPhiZero      = new TH1F("muPlusDxyErrorEtaPlusPhiZero", "muPlusDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaPlusPhiPlus      = new TH1F("muPlusDxyErrorEtaPlusPhiPlus", "muPlusDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaMinusPhiMinus      = new TH1F("muPlusDxyErrorEtaMinusPhiMinus", "muPlusDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaMinusPhiZero      = new TH1F("muPlusDxyErrorEtaMinusPhiZero", "muPlusDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muPlusDxyErrorEtaMinusPhiPlus      = new TH1F("muPlusDxyErrorEtaMinusPhiPlus", "muPlusDxyErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muUpperMinusDxyError = new TH1F("muUpperMinusDxyError","muUpperMinusDxyError", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaPlus      = new TH1F("muUpperMinusDxyErrorEtaPlus", "muUpperMinusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaMinus      = new TH1F("muUpperMinusDxyErrorEtaMinus", "muUpperMinusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaPlusPhiMinus      = new TH1F("muUpperMinusDxyErrorEtaPlusPhiMinus", "muUpperMinusDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaPlusPhiZero      = new TH1F("muUpperMinusDxyErrorEtaPlusPhiZero", "muUpperMinusDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaPlusPhiPlus      = new TH1F("muUpperMinusDxyErrorEtaPlusPhiPlus", "muUpperMinusDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaMinusPhiMinus      = new TH1F("muUpperMinusDxyErrorEtaMinusPhiMinus", "muUpperMinusDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaMinusPhiZero      = new TH1F("muUpperMinusDxyErrorEtaMinusPhiZero", "muUpperMinusDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperMinusDxyErrorEtaMinusPhiPlus      = new TH1F("muUpperMinusDxyErrorEtaMinusPhiPlus", "muUpperMinusDxyErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muUpperPlusDxyError  = new TH1F("muUpperPlusDxyError", "muUpperPlusDxyError",  50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaPlus      = new TH1F("muUpperPlusDxyErrorEtaPlus", "muUpperPlusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaMinus      = new TH1F("muUpperPlusDxyErrorEtaMinus", "muUpperPlusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaPlusPhiMinus      = new TH1F("muUpperPlusDxyErrorEtaPlusPhiMinus", "muUpperPlusDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaPlusPhiZero      = new TH1F("muUpperPlusDxyErrorEtaPlusPhiZero", "muUpperPlusDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaPlusPhiPlus      = new TH1F("muUpperPlusDxyErrorEtaPlusPhiPlus", "muUpperPlusDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaMinusPhiMinus      = new TH1F("muUpperPlusDxyErrorEtaMinusPhiMinus", "muUpperPlusDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaMinusPhiZero      = new TH1F("muUpperPlusDxyErrorEtaMinusPhiZero", "muUpperPlusDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperPlusDxyErrorEtaMinusPhiPlus      = new TH1F("muUpperPlusDxyErrorEtaMinusPhiPlus", "muUpperPlusDxyErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muLowerMinusDxyError = new TH1F("muLowerMinusDxyError","muLowerMinusDxyError", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaPlus      = new TH1F("muLowerMinusDxyErrorEtaPlus", "muLowerMinusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaMinus      = new TH1F("muLowerMinusDxyErrorEtaMinus", "muLowerMinusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaPlusPhiMinus      = new TH1F("muLowerMinusDxyErrorEtaPlusPhiMinus", "muLowerMinusDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaPlusPhiZero      = new TH1F("muLowerMinusDxyErrorEtaPlusPhiZero", "muLowerMinusDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaPlusPhiPlus      = new TH1F("muLowerMinusDxyErrorEtaPlusPhiPlus", "muLowerMinusDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaMinusPhiMinus      = new TH1F("muLowerMinusDxyErrorEtaMinusPhiMinus", "muLowerMinusDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaMinusPhiZero      = new TH1F("muLowerMinusDxyErrorEtaMinusPhiZero", "muLowerMinusDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerMinusDxyErrorEtaMinusPhiPlus      = new TH1F("muLowerMinusDxyErrorEtaMinusPhiPlus", "muLowerMinusDxyErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muLowerPlusDxyError  = new TH1F("muLowerPlusDxyError", "muLowerPlusDxyError",  50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaPlus      = new TH1F("muLowerPlusDxyErrorEtaPlus", "muLowerPlusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaMinus      = new TH1F("muLowerPlusDxyErrorEtaMinus", "muLowerPlusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaPlusPhiMinus      = new TH1F("muLowerPlusDxyErrorEtaPlusPhiMinus", "muLowerPlusDxyErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaPlusPhiZero      = new TH1F("muLowerPlusDxyErrorEtaPlusPhiZero", "muLowerPlusDxyErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaPlusPhiPlus      = new TH1F("muLowerPlusDxyErrorEtaPlusPhiPlus", "muLowerPlusDxyErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaMinusPhiMinus      = new TH1F("muLowerPlusDxyErrorEtaMinusPhiMinus", "muLowerPlusDxyErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaMinusPhiZero      = new TH1F("muLowerPlusDxyErrorEtaMinusPhiZero", "muLowerPlusDxyErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerPlusDxyErrorEtaMinusPhiPlus      = new TH1F("muLowerPlusDxyErrorEtaMinusPhiPlus", "muLowerPlusDxyErrorEtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_muMinusDzError      = new TH1F("muMinusDzError",     "muMinusDzError",      50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaPlus      = new TH1F("muMinusDzErrorEtaPlus", "muMinusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaMinus      = new TH1F("muMinusDzErrorEtaMinus", "muMinusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaPlusPhiMinus      = new TH1F("muMinusDzErrorEtaPlusPhiMinus", "muMinusDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaPlusPhiZero      = new TH1F("muMinusDzErrorEtaPlusPhiZero", "muMinusDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaPlusPhiPlus      = new TH1F("muMinusDzErrorEtaPlusPhiPlus", "muMinusDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaMinusPhiMinus      = new TH1F("muMinusDzErrorEtaMinusPhiMinus", "muMinusDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaMinusPhiZero      = new TH1F("muMinusDzErrorEtaMinusPhiZero", "muMinusDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muMinusDzErrorEtaMinusPhiPlus      = new TH1F("muMinusDzErrorEtaMinusPhiPlus", "muMinusDzErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muPlusDzError       = new TH1F("muPlusDzError",      "muPlusDzError",       50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaPlus      = new TH1F("muPlusDzErrorEtaPlus", "muPlusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaMinus      = new TH1F("muPlusDzErrorEtaMinus", "muPlusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaPlusPhiMinus      = new TH1F("muPlusDzErrorEtaPlusPhiMinus", "muPlusDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaPlusPhiZero      = new TH1F("muPlusDzErrorEtaPlusPhiZero", "muPlusDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaPlusPhiPlus      = new TH1F("muPlusDzErrorEtaPlusPhiPlus", "muPlusDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaMinusPhiMinus      = new TH1F("muPlusDzErrorEtaMinusPhiMinus", "muPlusDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaMinusPhiZero      = new TH1F("muPlusDzErrorEtaMinusPhiZero", "muPlusDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muPlusDzErrorEtaMinusPhiPlus      = new TH1F("muPlusDzErrorEtaMinusPhiPlus", "muPlusDzErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muUpperMinusDzError = new TH1F("muUpperMinusDzError","muUpperMinusDzError", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaPlus      = new TH1F("muUpperMinusDzErrorEtaPlus", "muUpperMinusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaMinus      = new TH1F("muUpperMinusDzErrorEtaMinus", "muUpperMinusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaPlusPhiMinus      = new TH1F("muUpperMinusDzErrorEtaPlusPhiMinus", "muUpperMinusDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaPlusPhiZero      = new TH1F("muUpperMinusDzErrorEtaPlusPhiZero", "muUpperMinusDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaPlusPhiPlus      = new TH1F("muUpperMinusDzErrorEtaPlusPhiPlus", "muUpperMinusDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaMinusPhiMinus      = new TH1F("muUpperMinusDzErrorEtaMinusPhiMinus", "muUpperMinusDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaMinusPhiZero      = new TH1F("muUpperMinusDzErrorEtaMinusPhiZero", "muUpperMinusDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperMinusDzErrorEtaMinusPhiPlus      = new TH1F("muUpperMinusDzErrorEtaMinusPhiPlus", "muUpperMinusDzErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muUpperPlusDzError  = new TH1F("muUpperPlusDzError", "muUpperPlusDzError",  50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaPlus      = new TH1F("muUpperPlusDzErrorEtaPlus", "muUpperPlusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaMinus      = new TH1F("muUpperPlusDzErrorEtaMinus", "muUpperPlusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaPlusPhiMinus      = new TH1F("muUpperPlusDzErrorEtaPlusPhiMinus", "muUpperPlusDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaPlusPhiZero      = new TH1F("muUpperPlusDzErrorEtaPlusPhiZero", "muUpperPlusDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaPlusPhiPlus      = new TH1F("muUpperPlusDzErrorEtaPlusPhiPlus", "muUpperPlusDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaMinusPhiMinus      = new TH1F("muUpperPlusDzErrorEtaMinusPhiMinus", "muUpperPlusDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaMinusPhiZero      = new TH1F("muUpperPlusDzErrorEtaMinusPhiZero", "muUpperPlusDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muUpperPlusDzErrorEtaMinusPhiPlus      = new TH1F("muUpperPlusDzErrorEtaMinusPhiPlus", "muUpperPlusDzErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muLowerMinusDzError = new TH1F("muLowerMinusDzError","muLowerMinusDzError", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaPlus      = new TH1F("muLowerMinusDzErrorEtaPlus", "muLowerMinusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaMinus      = new TH1F("muLowerMinusDzErrorEtaMinus", "muLowerMinusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaPlusPhiMinus      = new TH1F("muLowerMinusDzErrorEtaPlusPhiMinus", "muLowerMinusDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaPlusPhiZero      = new TH1F("muLowerMinusDzErrorEtaPlusPhiZero", "muLowerMinusDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaPlusPhiPlus      = new TH1F("muLowerMinusDzErrorEtaPlusPhiPlus", "muLowerMinusDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaMinusPhiMinus      = new TH1F("muLowerMinusDzErrorEtaMinusPhiMinus", "muLowerMinusDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaMinusPhiZero      = new TH1F("muLowerMinusDzErrorEtaMinusPhiZero", "muLowerMinusDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerMinusDzErrorEtaMinusPhiPlus      = new TH1F("muLowerMinusDzErrorEtaMinusPhiPlus", "muLowerMinusDzErrorEtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_muLowerPlusDzError  = new TH1F("muLowerPlusDzError", "muLowerPlusDzError",  50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaPlus      = new TH1F("muLowerPlusDzErrorEtaPlus", "muLowerPlusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaMinus      = new TH1F("muLowerPlusDzErrorEtaMinus", "muLowerPlusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaPlusPhiMinus      = new TH1F("muLowerPlusDzErrorEtaPlusPhiMinus", "muLowerPlusDzErrorEtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaPlusPhiZero      = new TH1F("muLowerPlusDzErrorEtaPlusPhiZero", "muLowerPlusDzErrorEtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaPlusPhiPlus      = new TH1F("muLowerPlusDzErrorEtaPlusPhiPlus", "muLowerPlusDzErrorEtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaMinusPhiMinus      = new TH1F("muLowerPlusDzErrorEtaMinusPhiMinus", "muLowerPlusDzErrorEtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaMinusPhiZero      = new TH1F("muLowerPlusDzErrorEtaMinusPhiZero", "muLowerPlusDzErrorEtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_muLowerPlusDzErrorEtaMinusPhiPlus      = new TH1F("muLowerPlusDzErrorEtaMinusPhiPlus", "muLowerPlusDzErrorEtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_muMinusTrackPt      = new TH1F("muMinusTrackPt",     "muMinusTrackPt",      300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaPlus      = new TH1F("muMinusTrackPtEtaPlus", "muMinusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaMinus      = new TH1F("muMinusTrackPtEtaMinus", "muMinusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaPlusPhiMinus      = new TH1F("muMinusTrackPtEtaPlusPhiMinus", "muMinusTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaPlusPhiZero      = new TH1F("muMinusTrackPtEtaPlusPhiZero", "muMinusTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaPlusPhiPlus      = new TH1F("muMinusTrackPtEtaPlusPhiPlus", "muMinusTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaMinusPhiMinus      = new TH1F("muMinusTrackPtEtaMinusPhiMinus", "muMinusTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaMinusPhiZero      = new TH1F("muMinusTrackPtEtaMinusPhiZero", "muMinusTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muMinusTrackPtEtaMinusPhiPlus      = new TH1F("muMinusTrackPtEtaMinusPhiPlus", "muMinusTrackPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muPlusTrackPt       = new TH1F("muPlusTrackPt",      "muPlusTrackPt",       300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaPlus      = new TH1F("muPlusTrackPtEtaPlus", "muPlusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaMinus      = new TH1F("muPlusTrackPtEtaMinus", "muPlusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaPlusPhiMinus      = new TH1F("muPlusTrackPtEtaPlusPhiMinus", "muPlusTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaPlusPhiZero      = new TH1F("muPlusTrackPtEtaPlusPhiZero", "muPlusTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaPlusPhiPlus      = new TH1F("muPlusTrackPtEtaPlusPhiPlus", "muPlusTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaMinusPhiMinus      = new TH1F("muPlusTrackPtEtaMinusPhiMinus", "muPlusTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaMinusPhiZero      = new TH1F("muPlusTrackPtEtaMinusPhiZero", "muPlusTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muPlusTrackPtEtaMinusPhiPlus      = new TH1F("muPlusTrackPtEtaMinusPhiPlus", "muPlusTrackPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muUpperMinusTrackPt = new TH1F("muUpperMinusTrackPt","muUpperMinusTrackPt", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaPlus      = new TH1F("muUpperMinusTrackPtEtaPlus", "muUpperMinusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaMinus      = new TH1F("muUpperMinusTrackPtEtaMinus", "muUpperMinusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaPlusPhiMinus      = new TH1F("muUpperMinusTrackPtEtaPlusPhiMinus", "muUpperMinusTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaPlusPhiZero      = new TH1F("muUpperMinusTrackPtEtaPlusPhiZero", "muUpperMinusTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaPlusPhiPlus      = new TH1F("muUpperMinusTrackPtEtaPlusPhiPlus", "muUpperMinusTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaMinusPhiMinus      = new TH1F("muUpperMinusTrackPtEtaMinusPhiMinus", "muUpperMinusTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaMinusPhiZero      = new TH1F("muUpperMinusTrackPtEtaMinusPhiZero", "muUpperMinusTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperMinusTrackPtEtaMinusPhiPlus      = new TH1F("muUpperMinusTrackPtEtaMinusPhiPlus", "muUpperMinusTrackPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muUpperPlusTrackPt  = new TH1F("muUpperPlusTrackPt", "muUpperPlusTrackPt",  300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaPlus      = new TH1F("muUpperPlusTrackPtEtaPlus", "muUpperPlusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaMinus      = new TH1F("muUpperPlusTrackPtEtaMinus", "muUpperPlusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaPlusPhiMinus      = new TH1F("muUpperPlusTrackPtEtaPlusPhiMinus", "muUpperPlusTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaPlusPhiZero      = new TH1F("muUpperPlusTrackPtEtaPlusPhiZero", "muUpperPlusTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaPlusPhiPlus      = new TH1F("muUpperPlusTrackPtEtaPlusPhiPlus", "muUpperPlusTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaMinusPhiMinus      = new TH1F("muUpperPlusTrackPtEtaMinusPhiMinus", "muUpperPlusTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaMinusPhiZero      = new TH1F("muUpperPlusTrackPtEtaMinusPhiZero", "muUpperPlusTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muUpperPlusTrackPtEtaMinusPhiPlus      = new TH1F("muUpperPlusTrackPtEtaMinusPhiPlus", "muUpperPlusTrackPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muLowerMinusTrackPt = new TH1F("muLowerMinusTrackPt","muLowerMinusTrackPt", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaPlus      = new TH1F("muLowerMinusTrackPtEtaPlus", "muLowerMinusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaMinus      = new TH1F("muLowerMinusTrackPtEtaMinus", "muLowerMinusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaPlusPhiMinus      = new TH1F("muLowerMinusTrackPtEtaPlusPhiMinus", "muLowerMinusTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaPlusPhiZero      = new TH1F("muLowerMinusTrackPtEtaPlusPhiZero", "muLowerMinusTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaPlusPhiPlus      = new TH1F("muLowerMinusTrackPtEtaPlusPhiPlus", "muLowerMinusTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaMinusPhiMinus      = new TH1F("muLowerMinusTrackPtEtaMinusPhiMinus", "muLowerMinusTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaMinusPhiZero      = new TH1F("muLowerMinusTrackPtEtaMinusPhiZero", "muLowerMinusTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerMinusTrackPtEtaMinusPhiPlus      = new TH1F("muLowerMinusTrackPtEtaMinusPhiPlus", "muLowerMinusTrackPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_muLowerPlusTrackPt  = new TH1F("muLowerPlusTrackPt", "muLowerPlusTrackPt",  300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaPlus      = new TH1F("muLowerPlusTrackPtEtaPlus", "muLowerPlusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaMinus      = new TH1F("muLowerPlusTrackPtEtaMinus", "muLowerPlusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaPlusPhiMinus      = new TH1F("muLowerPlusTrackPtEtaPlusPhiMinus", "muLowerPlusTrackPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaPlusPhiZero      = new TH1F("muLowerPlusTrackPtEtaPlusPhiZero", "muLowerPlusTrackPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaPlusPhiPlus      = new TH1F("muLowerPlusTrackPtEtaPlusPhiPlus", "muLowerPlusTrackPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaMinusPhiMinus      = new TH1F("muLowerPlusTrackPtEtaMinusPhiMinus", "muLowerPlusTrackPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaMinusPhiZero      = new TH1F("muLowerPlusTrackPtEtaMinusPhiZero", "muLowerPlusTrackPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_muLowerPlusTrackPtEtaMinusPhiPlus      = new TH1F("muLowerPlusTrackPtEtaMinusPhiPlus", "muLowerPlusTrackPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_muMinusTrackEta      = new TH1F("muMinusTrackEta",   "muMinusTrackEta",        40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaPlus      = new TH1F("muMinusTrackEtaEtaPlus", "muMinusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaMinus      = new TH1F("muMinusTrackEtaEtaMinus", "muMinusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaPlusPhiMinus      = new TH1F("muMinusTrackEtaEtaPlusPhiMinus", "muMinusTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaPlusPhiZero      = new TH1F("muMinusTrackEtaEtaPlusPhiZero", "muMinusTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaPlusPhiPlus      = new TH1F("muMinusTrackEtaEtaPlusPhiPlus", "muMinusTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaMinusPhiMinus      = new TH1F("muMinusTrackEtaEtaMinusPhiMinus", "muMinusTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaMinusPhiZero      = new TH1F("muMinusTrackEtaEtaMinusPhiZero", "muMinusTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muMinusTrackEtaEtaMinusPhiPlus      = new TH1F("muMinusTrackEtaEtaMinusPhiPlus", "muMinusTrackEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muPlusTrackEta       = new TH1F("muPlusTrackEta",      "muPlusTrackEta",       40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaPlus      = new TH1F("muPlusTrackEtaEtaPlus", "muPlusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaMinus      = new TH1F("muPlusTrackEtaEtaMinus", "muPlusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaPlusPhiMinus      = new TH1F("muPlusTrackEtaEtaPlusPhiMinus", "muPlusTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaPlusPhiZero      = new TH1F("muPlusTrackEtaEtaPlusPhiZero", "muPlusTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaPlusPhiPlus      = new TH1F("muPlusTrackEtaEtaPlusPhiPlus", "muPlusTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaMinusPhiMinus      = new TH1F("muPlusTrackEtaEtaMinusPhiMinus", "muPlusTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaMinusPhiZero      = new TH1F("muPlusTrackEtaEtaMinusPhiZero", "muPlusTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muPlusTrackEtaEtaMinusPhiPlus      = new TH1F("muPlusTrackEtaEtaMinusPhiPlus", "muPlusTrackEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muUpperMinusTrackEta = new TH1F("muUpperMinusTrackEta","muUpperMinusTrackEta", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaPlus      = new TH1F("muUpperMinusTrackEtaEtaPlus", "muUpperMinusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaMinus      = new TH1F("muUpperMinusTrackEtaEtaMinus", "muUpperMinusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaPlusPhiMinus      = new TH1F("muUpperMinusTrackEtaEtaPlusPhiMinus", "muUpperMinusTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaPlusPhiZero      = new TH1F("muUpperMinusTrackEtaEtaPlusPhiZero", "muUpperMinusTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaPlusPhiPlus      = new TH1F("muUpperMinusTrackEtaEtaPlusPhiPlus", "muUpperMinusTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaMinusPhiMinus      = new TH1F("muUpperMinusTrackEtaEtaMinusPhiMinus", "muUpperMinusTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaMinusPhiZero      = new TH1F("muUpperMinusTrackEtaEtaMinusPhiZero", "muUpperMinusTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperMinusTrackEtaEtaMinusPhiPlus      = new TH1F("muUpperMinusTrackEtaEtaMinusPhiPlus", "muUpperMinusTrackEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muUpperPlusTrackEta  = new TH1F("muUpperPlusTrackEta", "muUpperPlusTrackEta",  40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaPlus      = new TH1F("muUpperPlusTrackEtaEtaPlus", "muUpperPlusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaMinus      = new TH1F("muUpperPlusTrackEtaEtaMinus", "muUpperPlusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaPlusPhiMinus      = new TH1F("muUpperPlusTrackEtaEtaPlusPhiMinus", "muUpperPlusTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaPlusPhiZero      = new TH1F("muUpperPlusTrackEtaEtaPlusPhiZero", "muUpperPlusTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaPlusPhiPlus      = new TH1F("muUpperPlusTrackEtaEtaPlusPhiPlus", "muUpperPlusTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaMinusPhiMinus      = new TH1F("muUpperPlusTrackEtaEtaMinusPhiMinus", "muUpperPlusTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaMinusPhiZero      = new TH1F("muUpperPlusTrackEtaEtaMinusPhiZero", "muUpperPlusTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muUpperPlusTrackEtaEtaMinusPhiPlus      = new TH1F("muUpperPlusTrackEtaEtaMinusPhiPlus", "muUpperPlusTrackEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muLowerMinusTrackEta = new TH1F("muLowerMinusTrackEta","muLowerMinusTrackEta", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaPlus      = new TH1F("muLowerMinusTrackEtaEtaPlus", "muLowerMinusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaMinus      = new TH1F("muLowerMinusTrackEtaEtaMinus", "muLowerMinusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaPlusPhiMinus      = new TH1F("muLowerMinusTrackEtaEtaPlusPhiMinus", "muLowerMinusTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaPlusPhiZero      = new TH1F("muLowerMinusTrackEtaEtaPlusPhiZero", "muLowerMinusTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaPlusPhiPlus      = new TH1F("muLowerMinusTrackEtaEtaPlusPhiPlus", "muLowerMinusTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaMinusPhiMinus      = new TH1F("muLowerMinusTrackEtaEtaMinusPhiMinus", "muLowerMinusTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaMinusPhiZero      = new TH1F("muLowerMinusTrackEtaEtaMinusPhiZero", "muLowerMinusTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerMinusTrackEtaEtaMinusPhiPlus      = new TH1F("muLowerMinusTrackEtaEtaMinusPhiPlus", "muLowerMinusTrackEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_muLowerPlusTrackEta  = new TH1F("muLowerPlusTrackEta", "muLowerPlusTrackEta",  40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaPlus      = new TH1F("muLowerPlusTrackEtaEtaPlus", "muLowerPlusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaMinus      = new TH1F("muLowerPlusTrackEtaEtaMinus", "muLowerPlusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaPlusPhiMinus      = new TH1F("muLowerPlusTrackEtaEtaPlusPhiMinus", "muLowerPlusTrackEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaPlusPhiZero      = new TH1F("muLowerPlusTrackEtaEtaPlusPhiZero", "muLowerPlusTrackEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaPlusPhiPlus      = new TH1F("muLowerPlusTrackEtaEtaPlusPhiPlus", "muLowerPlusTrackEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaMinusPhiMinus      = new TH1F("muLowerPlusTrackEtaEtaMinusPhiMinus", "muLowerPlusTrackEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaMinusPhiZero      = new TH1F("muLowerPlusTrackEtaEtaMinusPhiZero", "muLowerPlusTrackEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_muLowerPlusTrackEtaEtaMinusPhiPlus      = new TH1F("muLowerPlusTrackEtaEtaMinusPhiPlus", "muLowerPlusTrackEtaEtaMinusPhiPlus", 40, -2., 2.);


  TH1F *h_muMinusTrackPhi      = new TH1F("muMinusTrackPhi",     "muMinusTrackPhi",      40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaPlus      = new TH1F("muMinusTrackPhiEtaPlus", "muMinusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaMinus      = new TH1F("muMinusTrackPhiEtaMinus", "muMinusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaPlusPhiMinus      = new TH1F("muMinusTrackPhiEtaPlusPhiMinus", "muMinusTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaPlusPhiZero      = new TH1F("muMinusTrackPhiEtaPlusPhiZero", "muMinusTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaPlusPhiPlus      = new TH1F("muMinusTrackPhiEtaPlusPhiPlus", "muMinusTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaMinusPhiMinus      = new TH1F("muMinusTrackPhiEtaMinusPhiMinus", "muMinusTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaMinusPhiZero      = new TH1F("muMinusTrackPhiEtaMinusPhiZero", "muMinusTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muMinusTrackPhiEtaMinusPhiPlus      = new TH1F("muMinusTrackPhiEtaMinusPhiPlus", "muMinusTrackPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muPlusTrackPhi       = new TH1F("muPlusTrackPhi",      "muPlusTrackPhi",       40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaPlus      = new TH1F("muPlusTrackPhiEtaPlus", "muPlusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaMinus      = new TH1F("muPlusTrackPhiEtaMinus", "muPlusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaPlusPhiMinus      = new TH1F("muPlusTrackPhiEtaPlusPhiMinus", "muPlusTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaPlusPhiZero      = new TH1F("muPlusTrackPhiEtaPlusPhiZero", "muPlusTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaPlusPhiPlus      = new TH1F("muPlusTrackPhiEtaPlusPhiPlus", "muPlusTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaMinusPhiMinus      = new TH1F("muPlusTrackPhiEtaMinusPhiMinus", "muPlusTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaMinusPhiZero      = new TH1F("muPlusTrackPhiEtaMinusPhiZero", "muPlusTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muPlusTrackPhiEtaMinusPhiPlus      = new TH1F("muPlusTrackPhiEtaMinusPhiPlus", "muPlusTrackPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muUpperMinusTrackPhi = new TH1F("muUpperMinusTrackPhi","muUpperMinusTrackPhi", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaPlus      = new TH1F("muUpperMinusTrackPhiEtaPlus", "muUpperMinusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaMinus      = new TH1F("muUpperMinusTrackPhiEtaMinus", "muUpperMinusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaPlusPhiMinus      = new TH1F("muUpperMinusTrackPhiEtaPlusPhiMinus", "muUpperMinusTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaPlusPhiZero      = new TH1F("muUpperMinusTrackPhiEtaPlusPhiZero", "muUpperMinusTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaPlusPhiPlus      = new TH1F("muUpperMinusTrackPhiEtaPlusPhiPlus", "muUpperMinusTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaMinusPhiMinus      = new TH1F("muUpperMinusTrackPhiEtaMinusPhiMinus", "muUpperMinusTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaMinusPhiZero      = new TH1F("muUpperMinusTrackPhiEtaMinusPhiZero", "muUpperMinusTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperMinusTrackPhiEtaMinusPhiPlus      = new TH1F("muUpperMinusTrackPhiEtaMinusPhiPlus", "muUpperMinusTrackPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muUpperPlusTrackPhi  = new TH1F("muUpperPlusTrackPhi", "muUpperPlusTrackPhi",  40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaPlus      = new TH1F("muUpperPlusTrackPhiEtaPlus", "muUpperPlusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaMinus      = new TH1F("muUpperPlusTrackPhiEtaMinus", "muUpperPlusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaPlusPhiMinus      = new TH1F("muUpperPlusTrackPhiEtaPlusPhiMinus", "muUpperPlusTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaPlusPhiZero      = new TH1F("muUpperPlusTrackPhiEtaPlusPhiZero", "muUpperPlusTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaPlusPhiPlus      = new TH1F("muUpperPlusTrackPhiEtaPlusPhiPlus", "muUpperPlusTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaMinusPhiMinus      = new TH1F("muUpperPlusTrackPhiEtaMinusPhiMinus", "muUpperPlusTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaMinusPhiZero      = new TH1F("muUpperPlusTrackPhiEtaMinusPhiZero", "muUpperPlusTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muUpperPlusTrackPhiEtaMinusPhiPlus      = new TH1F("muUpperPlusTrackPhiEtaMinusPhiPlus", "muUpperPlusTrackPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muLowerMinusTrackPhi = new TH1F("muLowerMinusTrackPhi","muLowerMinusTrackPhi", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaPlus      = new TH1F("muLowerMinusTrackPhiEtaPlus", "muLowerMinusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaMinus      = new TH1F("muLowerMinusTrackPhiEtaMinus", "muLowerMinusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaPlusPhiMinus      = new TH1F("muLowerMinusTrackPhiEtaPlusPhiMinus", "muLowerMinusTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaPlusPhiZero      = new TH1F("muLowerMinusTrackPhiEtaPlusPhiZero", "muLowerMinusTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaPlusPhiPlus      = new TH1F("muLowerMinusTrackPhiEtaPlusPhiPlus", "muLowerMinusTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaMinusPhiMinus      = new TH1F("muLowerMinusTrackPhiEtaMinusPhiMinus", "muLowerMinusTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaMinusPhiZero      = new TH1F("muLowerMinusTrackPhiEtaMinusPhiZero", "muLowerMinusTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerMinusTrackPhiEtaMinusPhiPlus      = new TH1F("muLowerMinusTrackPhiEtaMinusPhiPlus", "muLowerMinusTrackPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_muLowerPlusTrackPhi  = new TH1F("muLowerPlusTrackPhi", "muLowerPlusTrackPhi",  40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaPlus      = new TH1F("muLowerPlusTrackPhiEtaPlus", "muLowerPlusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaMinus      = new TH1F("muLowerPlusTrackPhiEtaMinus", "muLowerPlusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaPlusPhiMinus      = new TH1F("muLowerPlusTrackPhiEtaPlusPhiMinus", "muLowerPlusTrackPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaPlusPhiZero      = new TH1F("muLowerPlusTrackPhiEtaPlusPhiZero", "muLowerPlusTrackPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaPlusPhiPlus      = new TH1F("muLowerPlusTrackPhiEtaPlusPhiPlus", "muLowerPlusTrackPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaMinusPhiMinus      = new TH1F("muLowerPlusTrackPhiEtaMinusPhiMinus", "muLowerPlusTrackPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaMinusPhiZero      = new TH1F("muLowerPlusTrackPhiEtaMinusPhiZero", "muLowerPlusTrackPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_muLowerPlusTrackPhiEtaMinusPhiPlus      = new TH1F("muLowerPlusTrackPhiEtaMinusPhiPlus", "muLowerPlusTrackPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_muMinusPtError      = new TH1F("muMinusPtError",     "muMinusPtError",      100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaPlus      = new TH1F("muMinusPtErrorEtaPlus", "muMinusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaMinus      = new TH1F("muMinusPtErrorEtaMinus", "muMinusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaPlusPhiMinus      = new TH1F("muMinusPtErrorEtaPlusPhiMinus", "muMinusPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaPlusPhiZero      = new TH1F("muMinusPtErrorEtaPlusPhiZero", "muMinusPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaPlusPhiPlus      = new TH1F("muMinusPtErrorEtaPlusPhiPlus", "muMinusPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaMinusPhiMinus      = new TH1F("muMinusPtErrorEtaMinusPhiMinus", "muMinusPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaMinusPhiZero      = new TH1F("muMinusPtErrorEtaMinusPhiZero", "muMinusPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_muMinusPtErrorEtaMinusPhiPlus      = new TH1F("muMinusPtErrorEtaMinusPhiPlus", "muMinusPtErrorEtaMinusPhiPlus", 100, 0., 600.);

  TH1F *h_muPlusPtError       = new TH1F("muPlusPtError",      "muPlusPtError",       100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaPlus      = new TH1F("muPlusPtErrorEtaPlus", "muPlusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaMinus      = new TH1F("muPlusPtErrorEtaMinus", "muPlusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaPlusPhiMinus      = new TH1F("muPlusPtErrorEtaPlusPhiMinus", "muPlusPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaPlusPhiZero      = new TH1F("muPlusPtErrorEtaPlusPhiZero", "muPlusPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaPlusPhiPlus      = new TH1F("muPlusPtErrorEtaPlusPhiPlus", "muPlusPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaMinusPhiMinus      = new TH1F("muPlusPtErrorEtaMinusPhiMinus", "muPlusPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaMinusPhiZero      = new TH1F("muPlusPtErrorEtaMinusPhiZero", "muPlusPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_muPlusPtErrorEtaMinusPhiPlus      = new TH1F("muPlusPtErrorEtaMinusPhiPlus", "muPlusPtErrorEtaMinusPhiPlus", 100, 0., 600.);

  TH1F *h_muUpperMinusPtError = new TH1F("muUpperMinusPtError","muUpperMinusPtError", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaPlus      = new TH1F("muUpperMinusPtErrorEtaPlus", "muUpperMinusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaMinus      = new TH1F("muUpperMinusPtErrorEtaMinus", "muUpperMinusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaPlusPhiMinus      = new TH1F("muUpperMinusPtErrorEtaPlusPhiMinus", "muUpperMinusPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaPlusPhiZero      = new TH1F("muUpperMinusPtErrorEtaPlusPhiZero", "muUpperMinusPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaPlusPhiPlus      = new TH1F("muUpperMinusPtErrorEtaPlusPhiPlus", "muUpperMinusPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaMinusPhiMinus      = new TH1F("muUpperMinusPtErrorEtaMinusPhiMinus", "muUpperMinusPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaMinusPhiZero      = new TH1F("muUpperMinusPtErrorEtaMinusPhiZero", "muUpperMinusPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_muUpperMinusPtErrorEtaMinusPhiPlus      = new TH1F("muUpperMinusPtErrorEtaMinusPhiPlus", "muUpperMinusPtErrorEtaMinusPhiPlus", 100, 0., 600.);

  TH1F *h_muUpperPlusPtError  = new TH1F("muUpperPlusPtError", "muUpperPlusPtError",  100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaPlus      = new TH1F("muUpperPlusPtErrorEtaPlus", "muUpperPlusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaMinus      = new TH1F("muUpperPlusPtErrorEtaMinus", "muUpperPlusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaPlusPhiMinus      = new TH1F("muUpperPlusPtErrorEtaPlusPhiMinus", "muUpperPlusPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaPlusPhiZero      = new TH1F("muUpperPlusPtErrorEtaPlusPhiZero", "muUpperPlusPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaPlusPhiPlus      = new TH1F("muUpperPlusPtErrorEtaPlusPhiPlus", "muUpperPlusPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaMinusPhiMinus      = new TH1F("muUpperPlusPtErrorEtaMinusPhiMinus", "muUpperPlusPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaMinusPhiZero      = new TH1F("muUpperPlusPtErrorEtaMinusPhiZero", "muUpperPlusPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_muUpperPlusPtErrorEtaMinusPhiPlus      = new TH1F("muUpperPlusPtErrorEtaMinusPhiPlus", "muUpperPlusPtErrorEtaMinusPhiPlus", 100, 0., 600.);

  TH1F *h_muLowerMinusPtError = new TH1F("muLowerMinusPtError","muLowerMinusPtError", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaPlus      = new TH1F("muLowerMinusPtErrorEtaPlus", "muLowerMinusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaMinus      = new TH1F("muLowerMinusPtErrorEtaMinus", "muLowerMinusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaPlusPhiMinus      = new TH1F("muLowerMinusPtErrorEtaPlusPhiMinus", "muLowerMinusPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaPlusPhiZero      = new TH1F("muLowerMinusPtErrorEtaPlusPhiZero", "muLowerMinusPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaPlusPhiPlus      = new TH1F("muLowerMinusPtErrorEtaPlusPhiPlus", "muLowerMinusPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaMinusPhiMinus      = new TH1F("muLowerMinusPtErrorEtaMinusPhiMinus", "muLowerMinusPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaMinusPhiZero      = new TH1F("muLowerMinusPtErrorEtaMinusPhiZero", "muLowerMinusPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_muLowerMinusPtErrorEtaMinusPhiPlus      = new TH1F("muLowerMinusPtErrorEtaMinusPhiPlus", "muLowerMinusPtErrorEtaMinusPhiPlus", 100, 0., 600.);

  TH1F *h_muLowerPlusPtError  = new TH1F("muLowerPlusPtError", "muLowerPlusPtError",  100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaPlus      = new TH1F("muLowerPlusPtErrorEtaPlus", "muLowerPlusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaMinus      = new TH1F("muLowerPlusPtErrorEtaMinus", "muLowerPlusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaPlusPhiMinus      = new TH1F("muLowerPlusPtErrorEtaPlusPhiMinus", "muLowerPlusPtErrorEtaPlusPhiMinus", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaPlusPhiZero      = new TH1F("muLowerPlusPtErrorEtaPlusPhiZero", "muLowerPlusPtErrorEtaPlusPhiZero", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaPlusPhiPlus      = new TH1F("muLowerPlusPtErrorEtaPlusPhiPlus", "muLowerPlusPtErrorEtaPlusPhiPlus", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaMinusPhiMinus      = new TH1F("muLowerPlusPtErrorEtaMinusPhiMinus", "muLowerPlusPtErrorEtaMinusPhiMinus", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaMinusPhiZero      = new TH1F("muLowerPlusPtErrorEtaMinusPhiZero", "muLowerPlusPtErrorEtaMinusPhiZero", 100, 0., 600.);
  TH1F *h_muLowerPlusPtErrorEtaMinusPhiPlus      = new TH1F("muLowerPlusPtErrorEtaMinusPhiPlus", "muLowerPlusPtErrorEtaMinusPhiPlus", 100, 0., 600.);


  TH1F *h_muMinusPtRelErr      = new TH1F("muMinusPtRelErr",     "muMinusPtRelErr",      100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaPlus      = new TH1F("muMinusPtRelErrEtaPlus", "muMinusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaMinus      = new TH1F("muMinusPtRelErrEtaMinus", "muMinusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaPlusPhiMinus      = new TH1F("muMinusPtRelErrEtaPlusPhiMinus", "muMinusPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaPlusPhiZero      = new TH1F("muMinusPtRelErrEtaPlusPhiZero", "muMinusPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaPlusPhiPlus      = new TH1F("muMinusPtRelErrEtaPlusPhiPlus", "muMinusPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaMinusPhiMinus      = new TH1F("muMinusPtRelErrEtaMinusPhiMinus", "muMinusPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaMinusPhiZero      = new TH1F("muMinusPtRelErrEtaMinusPhiZero", "muMinusPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_muMinusPtRelErrEtaMinusPhiPlus      = new TH1F("muMinusPtRelErrEtaMinusPhiPlus", "muMinusPtRelErrEtaMinusPhiPlus", 100, 0., 1.);

  TH1F *h_muPlusPtRelErr       = new TH1F("muPlusPtRelErr",      "muPlusPtRelErr",       100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaPlus      = new TH1F("muPlusPtRelErrEtaPlus", "muPlusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaMinus      = new TH1F("muPlusPtRelErrEtaMinus", "muPlusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaPlusPhiMinus      = new TH1F("muPlusPtRelErrEtaPlusPhiMinus", "muPlusPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaPlusPhiZero      = new TH1F("muPlusPtRelErrEtaPlusPhiZero", "muPlusPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaPlusPhiPlus      = new TH1F("muPlusPtRelErrEtaPlusPhiPlus", "muPlusPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaMinusPhiMinus      = new TH1F("muPlusPtRelErrEtaMinusPhiMinus", "muPlusPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaMinusPhiZero      = new TH1F("muPlusPtRelErrEtaMinusPhiZero", "muPlusPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_muPlusPtRelErrEtaMinusPhiPlus      = new TH1F("muPlusPtRelErrEtaMinusPhiPlus", "muPlusPtRelErrEtaMinusPhiPlus", 100, 0., 1.);

  TH1F *h_muUpperMinusPtRelErr = new TH1F("muUpperMinusPtRelErr","muUpperMinusPtRelErr", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaPlus      = new TH1F("muUpperMinusPtRelErrEtaPlus", "muUpperMinusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaMinus      = new TH1F("muUpperMinusPtRelErrEtaMinus", "muUpperMinusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaPlusPhiMinus      = new TH1F("muUpperMinusPtRelErrEtaPlusPhiMinus", "muUpperMinusPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaPlusPhiZero      = new TH1F("muUpperMinusPtRelErrEtaPlusPhiZero", "muUpperMinusPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaPlusPhiPlus      = new TH1F("muUpperMinusPtRelErrEtaPlusPhiPlus", "muUpperMinusPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaMinusPhiMinus      = new TH1F("muUpperMinusPtRelErrEtaMinusPhiMinus", "muUpperMinusPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaMinusPhiZero      = new TH1F("muUpperMinusPtRelErrEtaMinusPhiZero", "muUpperMinusPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_muUpperMinusPtRelErrEtaMinusPhiPlus      = new TH1F("muUpperMinusPtRelErrEtaMinusPhiPlus", "muUpperMinusPtRelErrEtaMinusPhiPlus", 100, 0., 1.);

  TH1F *h_muUpperPlusPtRelErr  = new TH1F("muUpperPlusPtRelErr", "muUpperPlusPtRelErr",  100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaPlus      = new TH1F("muUpperPlusPtRelErrEtaPlus", "muUpperPlusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaMinus      = new TH1F("muUpperPlusPtRelErrEtaMinus", "muUpperPlusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaPlusPhiMinus      = new TH1F("muUpperPlusPtRelErrEtaPlusPhiMinus", "muUpperPlusPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaPlusPhiZero      = new TH1F("muUpperPlusPtRelErrEtaPlusPhiZero", "muUpperPlusPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaPlusPhiPlus      = new TH1F("muUpperPlusPtRelErrEtaPlusPhiPlus", "muUpperPlusPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaMinusPhiMinus      = new TH1F("muUpperPlusPtRelErrEtaMinusPhiMinus", "muUpperPlusPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaMinusPhiZero      = new TH1F("muUpperPlusPtRelErrEtaMinusPhiZero", "muUpperPlusPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_muUpperPlusPtRelErrEtaMinusPhiPlus      = new TH1F("muUpperPlusPtRelErrEtaMinusPhiPlus", "muUpperPlusPtRelErrEtaMinusPhiPlus", 100, 0., 1.);

  TH1F *h_muLowerMinusPtRelErr = new TH1F("muLowerMinusPtRelErr","muLowerMinusPtRelErr", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaPlus      = new TH1F("muLowerMinusPtRelErrEtaPlus", "muLowerMinusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaMinus      = new TH1F("muLowerMinusPtRelErrEtaMinus", "muLowerMinusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaPlusPhiMinus      = new TH1F("muLowerMinusPtRelErrEtaPlusPhiMinus", "muLowerMinusPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaPlusPhiZero      = new TH1F("muLowerMinusPtRelErrEtaPlusPhiZero", "muLowerMinusPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaPlusPhiPlus      = new TH1F("muLowerMinusPtRelErrEtaPlusPhiPlus", "muLowerMinusPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaMinusPhiMinus      = new TH1F("muLowerMinusPtRelErrEtaMinusPhiMinus", "muLowerMinusPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaMinusPhiZero      = new TH1F("muLowerMinusPtRelErrEtaMinusPhiZero", "muLowerMinusPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_muLowerMinusPtRelErrEtaMinusPhiPlus      = new TH1F("muLowerMinusPtRelErrEtaMinusPhiPlus", "muLowerMinusPtRelErrEtaMinusPhiPlus", 100, 0., 1.);

  TH1F *h_muLowerPlusPtRelErr  = new TH1F("muLowerPlusPtRelErr","muLowerPlusPtRelErr",   100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaPlus      = new TH1F("muLowerPlusPtRelErrEtaPlus", "muLowerPlusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaMinus      = new TH1F("muLowerPlusPtRelErrEtaMinus", "muLowerPlusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaPlusPhiMinus      = new TH1F("muLowerPlusPtRelErrEtaPlusPhiMinus", "muLowerPlusPtRelErrEtaPlusPhiMinus", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaPlusPhiZero      = new TH1F("muLowerPlusPtRelErrEtaPlusPhiZero", "muLowerPlusPtRelErrEtaPlusPhiZero", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaPlusPhiPlus      = new TH1F("muLowerPlusPtRelErrEtaPlusPhiPlus", "muLowerPlusPtRelErrEtaPlusPhiPlus", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaMinusPhiMinus      = new TH1F("muLowerPlusPtRelErrEtaMinusPhiMinus", "muLowerPlusPtRelErrEtaMinusPhiMinus", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaMinusPhiZero      = new TH1F("muLowerPlusPtRelErrEtaMinusPhiZero", "muLowerPlusPtRelErrEtaMinusPhiZero", 100, 0., 1.);
  TH1F *h_muLowerPlusPtRelErrEtaMinusPhiPlus      = new TH1F("muLowerPlusPtRelErrEtaMinusPhiPlus", "muLowerPlusPtRelErrEtaMinusPhiPlus", 100, 0., 1.);


  TH1F *h_muMinusPixelHits      = new TH1F("muMinusPixelHits",     "muMinusPixelHits",      10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaPlus      = new TH1F("muMinusPixelHitsEtaPlus", "muMinusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaMinus      = new TH1F("muMinusPixelHitsEtaMinus", "muMinusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaPlusPhiMinus      = new TH1F("muMinusPixelHitsEtaPlusPhiMinus", "muMinusPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaPlusPhiZero      = new TH1F("muMinusPixelHitsEtaPlusPhiZero", "muMinusPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaPlusPhiPlus      = new TH1F("muMinusPixelHitsEtaPlusPhiPlus", "muMinusPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaMinusPhiMinus      = new TH1F("muMinusPixelHitsEtaMinusPhiMinus", "muMinusPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaMinusPhiZero      = new TH1F("muMinusPixelHitsEtaMinusPhiZero", "muMinusPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muMinusPixelHitsEtaMinusPhiPlus      = new TH1F("muMinusPixelHitsEtaMinusPhiPlus", "muMinusPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muPlusPixelHits       = new TH1F("muPlusPixelHits",      "muPlusPixelHits",       10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaPlus      = new TH1F("muPlusPixelHitsEtaPlus", "muPlusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaMinus      = new TH1F("muPlusPixelHitsEtaMinus", "muPlusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaPlusPhiMinus      = new TH1F("muPlusPixelHitsEtaPlusPhiMinus", "muPlusPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaPlusPhiZero      = new TH1F("muPlusPixelHitsEtaPlusPhiZero", "muPlusPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaPlusPhiPlus      = new TH1F("muPlusPixelHitsEtaPlusPhiPlus", "muPlusPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaMinusPhiMinus      = new TH1F("muPlusPixelHitsEtaMinusPhiMinus", "muPlusPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaMinusPhiZero      = new TH1F("muPlusPixelHitsEtaMinusPhiZero", "muPlusPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muPlusPixelHitsEtaMinusPhiPlus      = new TH1F("muPlusPixelHitsEtaMinusPhiPlus", "muPlusPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muUpperMinusPixelHits = new TH1F("muUpperMinusPixelHits","muUpperMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaPlus      = new TH1F("muUpperMinusPixelHitsEtaPlus", "muUpperMinusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaMinus      = new TH1F("muUpperMinusPixelHitsEtaMinus", "muUpperMinusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaPlusPhiMinus      = new TH1F("muUpperMinusPixelHitsEtaPlusPhiMinus", "muUpperMinusPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaPlusPhiZero      = new TH1F("muUpperMinusPixelHitsEtaPlusPhiZero", "muUpperMinusPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaPlusPhiPlus      = new TH1F("muUpperMinusPixelHitsEtaPlusPhiPlus", "muUpperMinusPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaMinusPhiMinus      = new TH1F("muUpperMinusPixelHitsEtaMinusPhiMinus", "muUpperMinusPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaMinusPhiZero      = new TH1F("muUpperMinusPixelHitsEtaMinusPhiZero", "muUpperMinusPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusPixelHitsEtaMinusPhiPlus      = new TH1F("muUpperMinusPixelHitsEtaMinusPhiPlus", "muUpperMinusPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muUpperPlusPixelHits  = new TH1F("muUpperPlusPixelHits", "muUpperPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaPlus      = new TH1F("muUpperPlusPixelHitsEtaPlus", "muUpperPlusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaMinus      = new TH1F("muUpperPlusPixelHitsEtaMinus", "muUpperPlusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaPlusPhiMinus      = new TH1F("muUpperPlusPixelHitsEtaPlusPhiMinus", "muUpperPlusPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaPlusPhiZero      = new TH1F("muUpperPlusPixelHitsEtaPlusPhiZero", "muUpperPlusPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaPlusPhiPlus      = new TH1F("muUpperPlusPixelHitsEtaPlusPhiPlus", "muUpperPlusPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaMinusPhiMinus      = new TH1F("muUpperPlusPixelHitsEtaMinusPhiMinus", "muUpperPlusPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaMinusPhiZero      = new TH1F("muUpperPlusPixelHitsEtaMinusPhiZero", "muUpperPlusPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusPixelHitsEtaMinusPhiPlus      = new TH1F("muUpperPlusPixelHitsEtaMinusPhiPlus", "muUpperPlusPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muLowerMinusPixelHits = new TH1F("muLowerMinusPixelHits","muLowerMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaPlus      = new TH1F("muLowerMinusPixelHitsEtaPlus", "muLowerMinusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaMinus      = new TH1F("muLowerMinusPixelHitsEtaMinus", "muLowerMinusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaPlusPhiMinus      = new TH1F("muLowerMinusPixelHitsEtaPlusPhiMinus", "muLowerMinusPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaPlusPhiZero      = new TH1F("muLowerMinusPixelHitsEtaPlusPhiZero", "muLowerMinusPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaPlusPhiPlus      = new TH1F("muLowerMinusPixelHitsEtaPlusPhiPlus", "muLowerMinusPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaMinusPhiMinus      = new TH1F("muLowerMinusPixelHitsEtaMinusPhiMinus", "muLowerMinusPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaMinusPhiZero      = new TH1F("muLowerMinusPixelHitsEtaMinusPhiZero", "muLowerMinusPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusPixelHitsEtaMinusPhiPlus      = new TH1F("muLowerMinusPixelHitsEtaMinusPhiPlus", "muLowerMinusPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muLowerPlusPixelHits  = new TH1F("muLowerPlusPixelHits", "muLowerPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaPlus      = new TH1F("muLowerPlusPixelHitsEtaPlus", "muLowerPlusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaMinus      = new TH1F("muLowerPlusPixelHitsEtaMinus", "muLowerPlusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaPlusPhiMinus      = new TH1F("muLowerPlusPixelHitsEtaPlusPhiMinus", "muLowerPlusPixelHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaPlusPhiZero      = new TH1F("muLowerPlusPixelHitsEtaPlusPhiZero", "muLowerPlusPixelHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaPlusPhiPlus      = new TH1F("muLowerPlusPixelHitsEtaPlusPhiPlus", "muLowerPlusPixelHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaMinusPhiMinus      = new TH1F("muLowerPlusPixelHitsEtaMinusPhiMinus", "muLowerPlusPixelHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaMinusPhiZero      = new TH1F("muLowerPlusPixelHitsEtaMinusPhiZero", "muLowerPlusPixelHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusPixelHitsEtaMinusPhiPlus      = new TH1F("muLowerPlusPixelHitsEtaMinusPhiPlus", "muLowerPlusPixelHitsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_muMinusTrackerHits      = new TH1F("muMinusTrackerHits",     "muMinusTrackerHits",      35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaPlus      = new TH1F("muMinusTrackerHitsEtaPlus", "muMinusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaMinus      = new TH1F("muMinusTrackerHitsEtaMinus", "muMinusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaPlusPhiMinus      = new TH1F("muMinusTrackerHitsEtaPlusPhiMinus", "muMinusTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaPlusPhiZero      = new TH1F("muMinusTrackerHitsEtaPlusPhiZero", "muMinusTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaPlusPhiPlus      = new TH1F("muMinusTrackerHitsEtaPlusPhiPlus", "muMinusTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaMinusPhiMinus      = new TH1F("muMinusTrackerHitsEtaMinusPhiMinus", "muMinusTrackerHitsEtaMinusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaMinusPhiZero      = new TH1F("muMinusTrackerHitsEtaMinusPhiZero", "muMinusTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muMinusTrackerHitsEtaMinusPhiPlus      = new TH1F("muMinusTrackerHitsEtaMinusPhiPlus", "muMinusTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);

  TH1F *h_muPlusTrackerHits       = new TH1F("muPlusTrackerHits",      "muPlusTrackerHits",       35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaPlus      = new TH1F("muPlusTrackerHitsEtaPlus", "muPlusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaMinus      = new TH1F("muPlusTrackerHitsEtaMinus", "muPlusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaPlusPhiMinus      = new TH1F("muPlusTrackerHitsEtaPlusPhiMinus", "muPlusTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaPlusPhiZero      = new TH1F("muPlusTrackerHitsEtaPlusPhiZero", "muPlusTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaPlusPhiPlus      = new TH1F("muPlusTrackerHitsEtaPlusPhiPlus", "muPlusTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaMinusPhiMinus      = new TH1F("muPlusTrackerHitsEtaMinusPhiMinus", "muPlusTrackerHitsEtaMinusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaMinusPhiZero      = new TH1F("muPlusTrackerHitsEtaMinusPhiZero", "muPlusTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muPlusTrackerHitsEtaMinusPhiPlus      = new TH1F("muPlusTrackerHitsEtaMinusPhiPlus", "muPlusTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);

  TH1F *h_muUpperMinusTrackerHits = new TH1F("muUpperMinusTrackerHits","muUpperMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaPlus      = new TH1F("muUpperMinusTrackerHitsEtaPlus", "muUpperMinusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaMinus      = new TH1F("muUpperMinusTrackerHitsEtaMinus", "muUpperMinusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaPlusPhiMinus      = new TH1F("muUpperMinusTrackerHitsEtaPlusPhiMinus", "muUpperMinusTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaPlusPhiZero      = new TH1F("muUpperMinusTrackerHitsEtaPlusPhiZero", "muUpperMinusTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaPlusPhiPlus      = new TH1F("muUpperMinusTrackerHitsEtaPlusPhiPlus", "muUpperMinusTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaMinusPhiMinus      = new TH1F("muUpperMinusTrackerHitsEtaMinusPhiMinus", "muUpperMinusTrackerHitsEtaMinusPhiMinus",
								  35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaMinusPhiZero      = new TH1F("muUpperMinusTrackerHitsEtaMinusPhiZero", "muUpperMinusTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muUpperMinusTrackerHitsEtaMinusPhiPlus      = new TH1F("muUpperMinusTrackerHitsEtaMinusPhiPlus", "muUpperMinusTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);

  TH1F *h_muUpperPlusTrackerHits  = new TH1F("muUpperPlusTrackerHits", "muUpperPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaPlus      = new TH1F("muUpperPlusTrackerHitsEtaPlus", "muUpperPlusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaMinus      = new TH1F("muUpperPlusTrackerHitsEtaMinus", "muUpperPlusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaPlusPhiMinus      = new TH1F("muUpperPlusTrackerHitsEtaPlusPhiMinus", "muUpperPlusTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaPlusPhiZero      = new TH1F("muUpperPlusTrackerHitsEtaPlusPhiZero", "muUpperPlusTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaPlusPhiPlus      = new TH1F("muUpperPlusTrackerHitsEtaPlusPhiPlus", "muUpperPlusTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaMinusPhiMinus      = new TH1F("muUpperPlusTrackerHitsEtaMinusPhiMinus", "muUpperPlusTrackerHitsEtaMinusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaMinusPhiZero      = new TH1F("muUpperPlusTrackerHitsEtaMinusPhiZero", "muUpperPlusTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muUpperPlusTrackerHitsEtaMinusPhiPlus      = new TH1F("muUpperPlusTrackerHitsEtaMinusPhiPlus", "muUpperPlusTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);

  TH1F *h_muLowerMinusTrackerHits = new TH1F("muLowerMinusTrackerHits","muLowerMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaPlus      = new TH1F("muLowerMinusTrackerHitsEtaPlus", "muLowerMinusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaMinus      = new TH1F("muLowerMinusTrackerHitsEtaMinus", "muLowerMinusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaPlusPhiMinus      = new TH1F("muLowerMinusTrackerHitsEtaPlusPhiMinus", "muLowerMinusTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaPlusPhiZero      = new TH1F("muLowerMinusTrackerHitsEtaPlusPhiZero", "muLowerMinusTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaPlusPhiPlus      = new TH1F("muLowerMinusTrackerHitsEtaPlusPhiPlus", "muLowerMinusTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaMinusPhiMinus      = new TH1F("muLowerMinusTrackerHitsEtaMinusPhiMinus", "muLowerMinusTrackerHitsEtaMinusPhiMinus",
								  35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaMinusPhiZero      = new TH1F("muLowerMinusTrackerHitsEtaMinusPhiZero", "muLowerMinusTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muLowerMinusTrackerHitsEtaMinusPhiPlus      = new TH1F("muLowerMinusTrackerHitsEtaMinusPhiPlus", "muLowerMinusTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);

  TH1F *h_muLowerPlusTrackerHits  = new TH1F("muLowerPlusTrackerHits", "muLowerPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaPlus      = new TH1F("muLowerPlusTrackerHitsEtaPlus", "muLowerPlusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaMinus      = new TH1F("muLowerPlusTrackerHitsEtaMinus", "muLowerPlusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaPlusPhiMinus      = new TH1F("muLowerPlusTrackerHitsEtaPlusPhiMinus", "muLowerPlusTrackerHitsEtaPlusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaPlusPhiZero      = new TH1F("muLowerPlusTrackerHitsEtaPlusPhiZero", "muLowerPlusTrackerHitsEtaPlusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaPlusPhiPlus      = new TH1F("muLowerPlusTrackerHitsEtaPlusPhiPlus", "muLowerPlusTrackerHitsEtaPlusPhiPlus", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaMinusPhiMinus      = new TH1F("muLowerPlusTrackerHitsEtaMinusPhiMinus", "muLowerPlusTrackerHitsEtaMinusPhiMinus", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaMinusPhiZero      = new TH1F("muLowerPlusTrackerHitsEtaMinusPhiZero", "muLowerPlusTrackerHitsEtaMinusPhiZero", 35, -0.5, 34.5);
  TH1F *h_muLowerPlusTrackerHitsEtaMinusPhiPlus      = new TH1F("muLowerPlusTrackerHitsEtaMinusPhiPlus", "muLowerPlusTrackerHitsEtaMinusPhiPlus", 35, -0.5, 34.5);


  TH1F *h_muMinusMuonStationHits      = new TH1F("muMinusMuonStationHits",     "muMinusMuonStationHits",      10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaPlus      = new TH1F("muMinusMuonStationHitsEtaPlus", "muMinusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaMinus      = new TH1F("muMinusMuonStationHitsEtaMinus", "muMinusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaPlusPhiMinus      = new TH1F("muMinusMuonStationHitsEtaPlusPhiMinus", "muMinusMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaPlusPhiZero      = new TH1F("muMinusMuonStationHitsEtaPlusPhiZero", "muMinusMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaPlusPhiPlus      = new TH1F("muMinusMuonStationHitsEtaPlusPhiPlus", "muMinusMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaMinusPhiMinus      = new TH1F("muMinusMuonStationHitsEtaMinusPhiMinus", "muMinusMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaMinusPhiZero      = new TH1F("muMinusMuonStationHitsEtaMinusPhiZero", "muMinusMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muMinusMuonStationHitsEtaMinusPhiPlus      = new TH1F("muMinusMuonStationHitsEtaMinusPhiPlus", "muMinusMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muPlusMuonStationHits       = new TH1F("muPlusMuonStationHits",      "muPlusMuonStationHits",       10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaPlus      = new TH1F("muPlusMuonStationHitsEtaPlus", "muPlusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaMinus      = new TH1F("muPlusMuonStationHitsEtaMinus", "muPlusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaPlusPhiMinus      = new TH1F("muPlusMuonStationHitsEtaPlusPhiMinus", "muPlusMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaPlusPhiZero      = new TH1F("muPlusMuonStationHitsEtaPlusPhiZero", "muPlusMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaPlusPhiPlus      = new TH1F("muPlusMuonStationHitsEtaPlusPhiPlus", "muPlusMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaMinusPhiMinus      = new TH1F("muPlusMuonStationHitsEtaMinusPhiMinus", "muPlusMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaMinusPhiZero      = new TH1F("muPlusMuonStationHitsEtaMinusPhiZero", "muPlusMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muPlusMuonStationHitsEtaMinusPhiPlus      = new TH1F("muPlusMuonStationHitsEtaMinusPhiPlus", "muPlusMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muUpperMinusMuonStationHits = new TH1F("muUpperMinusMuonStationHits","muUpperMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaPlus      = new TH1F("muUpperMinusMuonStationHitsEtaPlus", "muUpperMinusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaMinus      = new TH1F("muUpperMinusMuonStationHitsEtaMinus", "muUpperMinusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaPlusPhiMinus      = new TH1F("muUpperMinusMuonStationHitsEtaPlusPhiMinus", "muUpperMinusMuonStationHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaPlusPhiZero      = new TH1F("muUpperMinusMuonStationHitsEtaPlusPhiZero", "muUpperMinusMuonStationHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaPlusPhiPlus      = new TH1F("muUpperMinusMuonStationHitsEtaPlusPhiPlus", "muUpperMinusMuonStationHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaMinusPhiMinus      = new TH1F("muUpperMinusMuonStationHitsEtaMinusPhiMinus", "muUpperMinusMuonStationHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaMinusPhiZero      = new TH1F("muUpperMinusMuonStationHitsEtaMinusPhiZero", "muUpperMinusMuonStationHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_muUpperMinusMuonStationHitsEtaMinusPhiPlus      = new TH1F("muUpperMinusMuonStationHitsEtaMinusPhiPlus", "muUpperMinusMuonStationHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_muUpperPlusMuonStationHits  = new TH1F("muUpperPlusMuonStationHits", "muUpperPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaPlus      = new TH1F("muUpperPlusMuonStationHitsEtaPlus", "muUpperPlusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaMinus      = new TH1F("muUpperPlusMuonStationHitsEtaMinus", "muUpperPlusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaPlusPhiMinus      = new TH1F("muUpperPlusMuonStationHitsEtaPlusPhiMinus", "muUpperPlusMuonStationHitsEtaPlusPhiMinus",
								    10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaPlusPhiZero      = new TH1F("muUpperPlusMuonStationHitsEtaPlusPhiZero", "muUpperPlusMuonStationHitsEtaPlusPhiZero",
								   10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaPlusPhiPlus      = new TH1F("muUpperPlusMuonStationHitsEtaPlusPhiPlus", "muUpperPlusMuonStationHitsEtaPlusPhiPlus",
								   10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaMinusPhiMinus      = new TH1F("muUpperPlusMuonStationHitsEtaMinusPhiMinus", "muUpperPlusMuonStationHitsEtaMinusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaMinusPhiZero      = new TH1F("muUpperPlusMuonStationHitsEtaMinusPhiZero", "muUpperPlusMuonStationHitsEtaMinusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_muUpperPlusMuonStationHitsEtaMinusPhiPlus      = new TH1F("muUpperPlusMuonStationHitsEtaMinusPhiPlus", "muUpperPlusMuonStationHitsEtaMinusPhiPlus",
								    10, -0.5, 9.5);

  TH1F *h_muLowerMinusMuonStationHits = new TH1F("muLowerMinusMuonStationHits","muLowerMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaPlus      = new TH1F("muLowerMinusMuonStationHitsEtaPlus", "muLowerMinusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaMinus      = new TH1F("muLowerMinusMuonStationHitsEtaMinus", "muLowerMinusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaPlusPhiMinus      = new TH1F("muLowerMinusMuonStationHitsEtaPlusPhiMinus", "muLowerMinusMuonStationHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaPlusPhiZero      = new TH1F("muLowerMinusMuonStationHitsEtaPlusPhiZero", "muLowerMinusMuonStationHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaPlusPhiPlus      = new TH1F("muLowerMinusMuonStationHitsEtaPlusPhiPlus", "muLowerMinusMuonStationHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaMinusPhiMinus      = new TH1F("muLowerMinusMuonStationHitsEtaMinusPhiMinus", "muLowerMinusMuonStationHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaMinusPhiZero      = new TH1F("muLowerMinusMuonStationHitsEtaMinusPhiZero", "muLowerMinusMuonStationHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_muLowerMinusMuonStationHitsEtaMinusPhiPlus      = new TH1F("muLowerMinusMuonStationHitsEtaMinusPhiPlus", "muLowerMinusMuonStationHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_muLowerPlusMuonStationHits  = new TH1F("muLowerPlusMuonStationHits", "muLowerPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaPlus      = new TH1F("muLowerPlusMuonStationHitsEtaPlus", "muLowerPlusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaMinus      = new TH1F("muLowerPlusMuonStationHitsEtaMinus", "muLowerPlusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaPlusPhiMinus      = new TH1F("muLowerPlusMuonStationHitsEtaPlusPhiMinus", "muLowerPlusMuonStationHitsEtaPlusPhiMinus",
								    10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaPlusPhiZero      = new TH1F("muLowerPlusMuonStationHitsEtaPlusPhiZero", "muLowerPlusMuonStationHitsEtaPlusPhiZero",
								   10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaPlusPhiPlus      = new TH1F("muLowerPlusMuonStationHitsEtaPlusPhiPlus", "muLowerPlusMuonStationHitsEtaPlusPhiPlus",
								   10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaMinusPhiMinus      = new TH1F("muLowerPlusMuonStationHitsEtaMinusPhiMinus", "muLowerPlusMuonStationHitsEtaMinusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaMinusPhiZero      = new TH1F("muLowerPlusMuonStationHitsEtaMinusPhiZero", "muLowerPlusMuonStationHitsEtaMinusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_muLowerPlusMuonStationHitsEtaMinusPhiPlus      = new TH1F("muLowerPlusMuonStationHitsEtaMinusPhiPlus", "muLowerPlusMuonStationHitsEtaMinusPhiPlus",
								    10, -0.5, 9.5);


  TH1F *h_muMinusValidHits      = new TH1F("muMinusValidHits",     "muMinusValidHits",      100,-0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaPlus      = new TH1F("muMinusValidHitsEtaPlus", "muMinusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaMinus      = new TH1F("muMinusValidHitsEtaMinus", "muMinusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaPlusPhiMinus      = new TH1F("muMinusValidHitsEtaPlusPhiMinus", "muMinusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaPlusPhiZero      = new TH1F("muMinusValidHitsEtaPlusPhiZero", "muMinusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaPlusPhiPlus      = new TH1F("muMinusValidHitsEtaPlusPhiPlus", "muMinusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaMinusPhiMinus      = new TH1F("muMinusValidHitsEtaMinusPhiMinus", "muMinusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaMinusPhiZero      = new TH1F("muMinusValidHitsEtaMinusPhiZero", "muMinusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muMinusValidHitsEtaMinusPhiPlus      = new TH1F("muMinusValidHitsEtaMinusPhiPlus", "muMinusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muPlusValidHits       = new TH1F("muPlusValidHits",      "muPlusValidHits",       100,-0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaPlus      = new TH1F("muPlusValidHitsEtaPlus", "muPlusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaMinus      = new TH1F("muPlusValidHitsEtaMinus", "muPlusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaPlusPhiMinus      = new TH1F("muPlusValidHitsEtaPlusPhiMinus", "muPlusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaPlusPhiZero      = new TH1F("muPlusValidHitsEtaPlusPhiZero", "muPlusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaPlusPhiPlus      = new TH1F("muPlusValidHitsEtaPlusPhiPlus", "muPlusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaMinusPhiMinus      = new TH1F("muPlusValidHitsEtaMinusPhiMinus", "muPlusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaMinusPhiZero      = new TH1F("muPlusValidHitsEtaMinusPhiZero", "muPlusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muPlusValidHitsEtaMinusPhiPlus      = new TH1F("muPlusValidHitsEtaMinusPhiPlus", "muPlusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muUpperMinusValidHits = new TH1F("muUpperMinusValidHits","muUpperMinusValidHits", 100,-0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaPlus      = new TH1F("muUpperMinusValidHitsEtaPlus", "muUpperMinusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaMinus      = new TH1F("muUpperMinusValidHitsEtaMinus", "muUpperMinusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaPlusPhiMinus      = new TH1F("muUpperMinusValidHitsEtaPlusPhiMinus", "muUpperMinusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaPlusPhiZero      = new TH1F("muUpperMinusValidHitsEtaPlusPhiZero", "muUpperMinusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaPlusPhiPlus      = new TH1F("muUpperMinusValidHitsEtaPlusPhiPlus", "muUpperMinusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaMinusPhiMinus      = new TH1F("muUpperMinusValidHitsEtaMinusPhiMinus", "muUpperMinusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaMinusPhiZero      = new TH1F("muUpperMinusValidHitsEtaMinusPhiZero", "muUpperMinusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperMinusValidHitsEtaMinusPhiPlus      = new TH1F("muUpperMinusValidHitsEtaMinusPhiPlus", "muUpperMinusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muUpperPlusValidHits  = new TH1F("muUpperPlusValidHits", "muUpperPlusValidHits",  100,-0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaPlus      = new TH1F("muUpperPlusValidHitsEtaPlus", "muUpperPlusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaMinus      = new TH1F("muUpperPlusValidHitsEtaMinus", "muUpperPlusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaPlusPhiMinus      = new TH1F("muUpperPlusValidHitsEtaPlusPhiMinus", "muUpperPlusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaPlusPhiZero      = new TH1F("muUpperPlusValidHitsEtaPlusPhiZero", "muUpperPlusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaPlusPhiPlus      = new TH1F("muUpperPlusValidHitsEtaPlusPhiPlus", "muUpperPlusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaMinusPhiMinus      = new TH1F("muUpperPlusValidHitsEtaMinusPhiMinus", "muUpperPlusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaMinusPhiZero      = new TH1F("muUpperPlusValidHitsEtaMinusPhiZero", "muUpperPlusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muUpperPlusValidHitsEtaMinusPhiPlus      = new TH1F("muUpperPlusValidHitsEtaMinusPhiPlus", "muUpperPlusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muLowerMinusValidHits = new TH1F("muLowerMinusValidHits","muLowerMinusValidHits", 100,-0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaPlus      = new TH1F("muLowerMinusValidHitsEtaPlus", "muLowerMinusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaMinus      = new TH1F("muLowerMinusValidHitsEtaMinus", "muLowerMinusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaPlusPhiMinus      = new TH1F("muLowerMinusValidHitsEtaPlusPhiMinus", "muLowerMinusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaPlusPhiZero      = new TH1F("muLowerMinusValidHitsEtaPlusPhiZero", "muLowerMinusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaPlusPhiPlus      = new TH1F("muLowerMinusValidHitsEtaPlusPhiPlus", "muLowerMinusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaMinusPhiMinus      = new TH1F("muLowerMinusValidHitsEtaMinusPhiMinus", "muLowerMinusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaMinusPhiZero      = new TH1F("muLowerMinusValidHitsEtaMinusPhiZero", "muLowerMinusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerMinusValidHitsEtaMinusPhiPlus      = new TH1F("muLowerMinusValidHitsEtaMinusPhiPlus", "muLowerMinusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_muLowerPlusValidHits  = new TH1F("muLowerPlusValidHits", "muLowerPlusValidHits",  100,-0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaPlus      = new TH1F("muLowerPlusValidHitsEtaPlus", "muLowerPlusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaMinus      = new TH1F("muLowerPlusValidHitsEtaMinus", "muLowerPlusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaPlusPhiMinus      = new TH1F("muLowerPlusValidHitsEtaPlusPhiMinus", "muLowerPlusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaPlusPhiZero      = new TH1F("muLowerPlusValidHitsEtaPlusPhiZero", "muLowerPlusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaPlusPhiPlus      = new TH1F("muLowerPlusValidHitsEtaPlusPhiPlus", "muLowerPlusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaMinusPhiMinus      = new TH1F("muLowerPlusValidHitsEtaMinusPhiMinus", "muLowerPlusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaMinusPhiZero      = new TH1F("muLowerPlusValidHitsEtaMinusPhiZero", "muLowerPlusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_muLowerPlusValidHitsEtaMinusPhiPlus      = new TH1F("muLowerPlusValidHitsEtaMinusPhiPlus", "muLowerPlusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_muMinusValidMuonHits      = new TH1F("muMinusValidMuonHits",     "muMinusValidMuonHits",      75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaPlus      = new TH1F("muMinusValidMuonHitsEtaPlus", "muMinusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaMinus      = new TH1F("muMinusValidMuonHitsEtaMinus", "muMinusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaPlusPhiMinus      = new TH1F("muMinusValidMuonHitsEtaPlusPhiMinus", "muMinusValidMuonHitsEtaPlusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaPlusPhiZero      = new TH1F("muMinusValidMuonHitsEtaPlusPhiZero", "muMinusValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaPlusPhiPlus      = new TH1F("muMinusValidMuonHitsEtaPlusPhiPlus", "muMinusValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaMinusPhiMinus      = new TH1F("muMinusValidMuonHitsEtaMinusPhiMinus", "muMinusValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaMinusPhiZero      = new TH1F("muMinusValidMuonHitsEtaMinusPhiZero", "muMinusValidMuonHitsEtaMinusPhiZero", 75, -0.5, 74.5);
  TH1F *h_muMinusValidMuonHitsEtaMinusPhiPlus      = new TH1F("muMinusValidMuonHitsEtaMinusPhiPlus", "muMinusValidMuonHitsEtaMinusPhiPlus", 75, -0.5, 74.5);

  TH1F *h_muPlusValidMuonHits       = new TH1F("muPlusValidMuonHits",      "muPlusValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaPlus      = new TH1F("muPlusValidMuonHitsEtaPlus", "muPlusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaMinus      = new TH1F("muPlusValidMuonHitsEtaMinus", "muPlusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaPlusPhiMinus      = new TH1F("muPlusValidMuonHitsEtaPlusPhiMinus", "muPlusValidMuonHitsEtaPlusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaPlusPhiZero      = new TH1F("muPlusValidMuonHitsEtaPlusPhiZero", "muPlusValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaPlusPhiPlus      = new TH1F("muPlusValidMuonHitsEtaPlusPhiPlus", "muPlusValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaMinusPhiMinus      = new TH1F("muPlusValidMuonHitsEtaMinusPhiMinus", "muPlusValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaMinusPhiZero      = new TH1F("muPlusValidMuonHitsEtaMinusPhiZero", "muPlusValidMuonHitsEtaMinusPhiZero", 75, -0.5, 74.5);
  TH1F *h_muPlusValidMuonHitsEtaMinusPhiPlus      = new TH1F("muPlusValidMuonHitsEtaMinusPhiPlus", "muPlusValidMuonHitsEtaMinusPhiPlus", 75, -0.5, 74.5);

  TH1F *h_muUpperMinusValidMuonHits = new TH1F("muUpperMinusValidMuonHits","muUpperMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaPlus      = new TH1F("muUpperMinusValidMuonHitsEtaPlus", "muUpperMinusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaMinus      = new TH1F("muUpperMinusValidMuonHitsEtaMinus", "muUpperMinusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaPlusPhiMinus      = new TH1F("muUpperMinusValidMuonHitsEtaPlusPhiMinus", "muUpperMinusValidMuonHitsEtaPlusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaPlusPhiZero      = new TH1F("muUpperMinusValidMuonHitsEtaPlusPhiZero", "muUpperMinusValidMuonHitsEtaPlusPhiZero",
								  75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaPlusPhiPlus      = new TH1F("muUpperMinusValidMuonHitsEtaPlusPhiPlus", "muUpperMinusValidMuonHitsEtaPlusPhiPlus",
								  75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaMinusPhiMinus      = new TH1F("muUpperMinusValidMuonHitsEtaMinusPhiMinus", "muUpperMinusValidMuonHitsEtaMinusPhiMinus",
								    75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaMinusPhiZero      = new TH1F("muUpperMinusValidMuonHitsEtaMinusPhiZero", "muUpperMinusValidMuonHitsEtaMinusPhiZero",
								   75, -0.5, 74.5);
  TH1F *h_muUpperMinusValidMuonHitsEtaMinusPhiPlus      = new TH1F("muUpperMinusValidMuonHitsEtaMinusPhiPlus", "muUpperMinusValidMuonHitsEtaMinusPhiPlus", 
								   75, -0.5, 74.5);

  TH1F *h_muUpperPlusValidMuonHits  = new TH1F("muUpperPlusValidMuonHits", "muUpperPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaPlus      = new TH1F("muUpperPlusValidMuonHitsEtaPlus", "muUpperPlusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaMinus      = new TH1F("muUpperPlusValidMuonHitsEtaMinus", "muUpperPlusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaPlusPhiMinus      = new TH1F("muUpperPlusValidMuonHitsEtaPlusPhiMinus", "muUpperPlusValidMuonHitsEtaPlusPhiMinus",
								  75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaPlusPhiZero      = new TH1F("muUpperPlusValidMuonHitsEtaPlusPhiZero", "muUpperPlusValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaPlusPhiPlus      = new TH1F("muUpperPlusValidMuonHitsEtaPlusPhiPlus", "muUpperPlusValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaMinusPhiMinus      = new TH1F("muUpperPlusValidMuonHitsEtaMinusPhiMinus", "muUpperPlusValidMuonHitsEtaMinusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaMinusPhiZero      = new TH1F("muUpperPlusValidMuonHitsEtaMinusPhiZero", "muUpperPlusValidMuonHitsEtaMinusPhiZero",
								  75, -0.5, 74.5);
  TH1F *h_muUpperPlusValidMuonHitsEtaMinusPhiPlus      = new TH1F("muUpperPlusValidMuonHitsEtaMinusPhiPlus", "muUpperPlusValidMuonHitsEtaMinusPhiPlus",
								  75, -0.5, 74.5);
  
  TH1F *h_muLowerMinusValidMuonHits = new TH1F("muLowerMinusValidMuonHits","muLowerMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaPlus      = new TH1F("muLowerMinusValidMuonHitsEtaPlus", "muLowerMinusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaMinus      = new TH1F("muLowerMinusValidMuonHitsEtaMinus", "muLowerMinusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaPlusPhiMinus      = new TH1F("muLowerMinusValidMuonHitsEtaPlusPhiMinus", "muLowerMinusValidMuonHitsEtaPlusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaPlusPhiZero      = new TH1F("muLowerMinusValidMuonHitsEtaPlusPhiZero", "muLowerMinusValidMuonHitsEtaPlusPhiZero",
								  75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaPlusPhiPlus      = new TH1F("muLowerMinusValidMuonHitsEtaPlusPhiPlus", "muLowerMinusValidMuonHitsEtaPlusPhiPlus",
								  75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaMinusPhiMinus      = new TH1F("muLowerMinusValidMuonHitsEtaMinusPhiMinus", "muLowerMinusValidMuonHitsEtaMinusPhiMinus",
								    75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaMinusPhiZero      = new TH1F("muLowerMinusValidMuonHitsEtaMinusPhiZero", "muLowerMinusValidMuonHitsEtaMinusPhiZero",
								   75, -0.5, 74.5);
  TH1F *h_muLowerMinusValidMuonHitsEtaMinusPhiPlus      = new TH1F("muLowerMinusValidMuonHitsEtaMinusPhiPlus", "muLowerMinusValidMuonHitsEtaMinusPhiPlus",
								   75, -0.5, 74.5);

  TH1F *h_muLowerPlusValidMuonHits  = new TH1F("muLowerPlusValidMuonHits", "muLowerPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaPlus      = new TH1F("muLowerPlusValidMuonHitsEtaPlus", "muLowerPlusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaMinus      = new TH1F("muLowerPlusValidMuonHitsEtaMinus", "muLowerPlusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaPlusPhiMinus      = new TH1F("muLowerPlusValidMuonHitsEtaPlusPhiMinus", "muLowerPlusValidMuonHitsEtaPlusPhiMinus",
								  75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaPlusPhiZero      = new TH1F("muLowerPlusValidMuonHitsEtaPlusPhiZero", "muLowerPlusValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaPlusPhiPlus      = new TH1F("muLowerPlusValidMuonHitsEtaPlusPhiPlus", "muLowerPlusValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaMinusPhiMinus      = new TH1F("muLowerPlusValidMuonHitsEtaMinusPhiMinus", "muLowerPlusValidMuonHitsEtaMinusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaMinusPhiZero      = new TH1F("muLowerPlusValidMuonHitsEtaMinusPhiZero", "muLowerPlusValidMuonHitsEtaMinusPhiZero",
								  75, -0.5, 74.5);
  TH1F *h_muLowerPlusValidMuonHitsEtaMinusPhiPlus      = new TH1F("muLowerPlusValidMuonHitsEtaMinusPhiPlus", "muLowerPlusValidMuonHitsEtaMinusPhiPlus",
								  75, -0.5, 74.5);


  TH1F *h_muMinusMatchedMuonStations      = new TH1F("muMinusMatchedMuonStations",     "muMinusMatchedMuonStations",      10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaPlus      = new TH1F("muMinusMatchedMuonStationsEtaPlus",
								     "muMinusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaMinus      = new TH1F("muMinusMatchedMuonStationsEtaMinus",
								      "muMinusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("muMinusMatchedMuonStationsEtaPlusPhiMinus",
									     "muMinusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("muMinusMatchedMuonStationsEtaPlusPhiZero",
									    "muMinusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("muMinusMatchedMuonStationsEtaPlusPhiPlus",
									    "muMinusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("muMinusMatchedMuonStationsEtaMinusPhiMinus",
									      "muMinusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("muMinusMatchedMuonStationsEtaMinusPhiZero",
									     "muMinusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muMinusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("muMinusMatchedMuonStationsEtaMinusPhiPlus",
								    "muMinusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_muPlusMatchedMuonStations       = new TH1F("muPlusMatchedMuonStations",      "muPlusMatchedMuonStations",       10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaPlus      = new TH1F("muPlusMatchedMuonStationsEtaPlus",
								     "muPlusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaMinus      = new TH1F("muPlusMatchedMuonStationsEtaMinus",
								      "muPlusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("muPlusMatchedMuonStationsEtaPlusPhiMinus",
									     "muPlusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("muPlusMatchedMuonStationsEtaPlusPhiZero",
									    "muPlusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("muPlusMatchedMuonStationsEtaPlusPhiPlus",
									    "muPlusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("muPlusMatchedMuonStationsEtaMinusPhiMinus",
									      "muPlusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("muPlusMatchedMuonStationsEtaMinusPhiZero",
									     "muPlusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muPlusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("muPlusMatchedMuonStationsEtaMinusPhiPlus",
								    "muPlusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muUpperMinusMatchedMuonStations = new TH1F("muUpperMinusMatchedMuonStations","muUpperMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaPlus      = new TH1F("muUpperMinusMatchedMuonStationsEtaPlus",
								     "muUpperMinusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaMinus      = new TH1F("muUpperMinusMatchedMuonStationsEtaMinus",
								      "muUpperMinusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("muUpperMinusMatchedMuonStationsEtaPlusPhiMinus",
									     "muUpperMinusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("muUpperMinusMatchedMuonStationsEtaPlusPhiZero",
									    "muUpperMinusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("muUpperMinusMatchedMuonStationsEtaPlusPhiPlus",
									    "muUpperMinusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("muUpperMinusMatchedMuonStationsEtaMinusPhiMinus",
									      "muUpperMinusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("muUpperMinusMatchedMuonStationsEtaMinusPhiZero",
									     "muUpperMinusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperMinusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("muUpperMinusMatchedMuonStationsEtaMinusPhiPlus",
								    "muUpperMinusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muUpperPlusMatchedMuonStations  = new TH1F("muUpperPlusMatchedMuonStations", "muUpperPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaPlus      = new TH1F("muUpperPlusMatchedMuonStationsEtaPlus",
								     "muUpperPlusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaMinus      = new TH1F("muUpperPlusMatchedMuonStationsEtaMinus",
								      "muUpperPlusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("muUpperPlusMatchedMuonStationsEtaPlusPhiMinus",
									     "muUpperPlusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("muUpperPlusMatchedMuonStationsEtaPlusPhiZero",
									    "muUpperPlusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("muUpperPlusMatchedMuonStationsEtaPlusPhiPlus",
									    "muUpperPlusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("muUpperPlusMatchedMuonStationsEtaMinusPhiMinus",
									      "muUpperPlusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("muUpperPlusMatchedMuonStationsEtaMinusPhiZero",
									     "muUpperPlusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muUpperPlusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("muUpperPlusMatchedMuonStationsEtaMinusPhiPlus",
								    "muUpperPlusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muLowerMinusMatchedMuonStations = new TH1F("muLowerMinusMatchedMuonStations","muLowerMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaPlus      = new TH1F("muLowerMinusMatchedMuonStationsEtaPlus",
								     "muLowerMinusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaMinus      = new TH1F("muLowerMinusMatchedMuonStationsEtaMinus",
								      "muLowerMinusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("muLowerMinusMatchedMuonStationsEtaPlusPhiMinus",
									     "muLowerMinusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("muLowerMinusMatchedMuonStationsEtaPlusPhiZero",
									    "muLowerMinusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("muLowerMinusMatchedMuonStationsEtaPlusPhiPlus",
									    "muLowerMinusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("muLowerMinusMatchedMuonStationsEtaMinusPhiMinus",
									      "muLowerMinusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("muLowerMinusMatchedMuonStationsEtaMinusPhiZero",
									     "muLowerMinusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerMinusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("muLowerMinusMatchedMuonStationsEtaMinusPhiPlus",
								    "muLowerMinusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_muLowerPlusMatchedMuonStations  = new TH1F("muLowerPlusMatchedMuonStations", "muLowerPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaPlus      = new TH1F("muLowerPlusMatchedMuonStationsEtaPlus",
								     "muLowerPlusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaMinus      = new TH1F("muLowerPlusMatchedMuonStationsEtaMinus",
								      "muLowerPlusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("muLowerPlusMatchedMuonStationsEtaPlusPhiMinus",
									     "muLowerPlusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("muLowerPlusMatchedMuonStationsEtaPlusPhiZero",
									    "muLowerPlusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("muLowerPlusMatchedMuonStationsEtaPlusPhiPlus",
									    "muLowerPlusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("muLowerPlusMatchedMuonStationsEtaMinusPhiMinus",
									      "muLowerPlusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("muLowerPlusMatchedMuonStationsEtaMinusPhiZero",
									     "muLowerPlusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_muLowerPlusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("muLowerPlusMatchedMuonStationsEtaMinusPhiPlus",
								    "muLowerPlusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_muMinusTrackerLayersWithMeasurement      = new TH1F("muMinusTrackerLayersWithMeasurement",     "muMinusTrackerLayersWithMeasurement",      20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaPlus      = new TH1F("muMinusTrackerLayersWithMeasurementEtaPlus",
								     "muMinusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaMinus      = new TH1F("muMinusTrackerLayersWithMeasurementEtaMinus",
								      "muMinusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("muMinusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									     "muMinusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("muMinusTrackerLayersWithMeasurementEtaPlusPhiZero",
									    "muMinusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("muMinusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									    "muMinusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("muMinusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									      "muMinusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("muMinusTrackerLayersWithMeasurementEtaMinusPhiZero",
									     "muMinusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muMinusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("muMinusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									     "muMinusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);

  TH1F *h_muPlusTrackerLayersWithMeasurement       = new TH1F("muPlusTrackerLayersWithMeasurement",      "muPlusTrackerLayersWithMeasurement",       20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaPlus      = new TH1F("muPlusTrackerLayersWithMeasurementEtaPlus",
								     "muPlusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaMinus      = new TH1F("muPlusTrackerLayersWithMeasurementEtaMinus",
								      "muPlusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("muPlusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									     "muPlusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("muPlusTrackerLayersWithMeasurementEtaPlusPhiZero",
									    "muPlusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("muPlusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									    "muPlusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("muPlusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									      "muPlusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("muPlusTrackerLayersWithMeasurementEtaMinusPhiZero",
									     "muPlusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muPlusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("muPlusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									     "muPlusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_muUpperMinusTrackerLayersWithMeasurement = new TH1F("muUpperMinusTrackerLayersWithMeasurement","muUpperMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaPlus      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaPlus",
								     "muUpperMinusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaMinus      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaMinus",
								      "muUpperMinusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									     "muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero",
									    "muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									    "muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									      "muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero",
									     "muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									     "muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_muUpperPlusTrackerLayersWithMeasurement  = new TH1F("muUpperPlusTrackerLayersWithMeasurement", "muUpperPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaPlus      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaPlus",
								     "muUpperPlusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaMinus      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaMinus",
								      "muUpperPlusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									     "muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero",
									    "muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									    "muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									      "muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero",
									     "muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									     "muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_muLowerMinusTrackerLayersWithMeasurement = new TH1F("muLowerMinusTrackerLayersWithMeasurement","muLowerMinusTrackerLayersWithMeasurement", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaPlus      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaPlus",
								     "muLowerMinusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaMinus      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaMinus",
								      "muLowerMinusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									     "muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero",
									    "muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									    "muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									      "muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero",
									     "muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									     "muLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_muLowerPlusTrackerLayersWithMeasurement  = new TH1F("muLowerPlusTrackerLayersWithMeasurement", "muLowerPlusTrackerLayersWithMeasurement",  20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaPlus      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaPlus",
								     "muLowerPlusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaMinus      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaMinus",
								      "muLowerPlusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									     "muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero",
									    "muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									    "muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									      "muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero",
									     "muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									     "muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);



  TH1F *h_muMinusCurvePlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPhiMinusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPhiZeroPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPhiPlusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPhiMinusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPhiZeroPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPhiPlusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaAllPhiMinusPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaAllPhiZeroPlusBias[      nBiasBins];
  TH1F *h_muMinusCurveEtaAllPhiPlusPlusBias[      nBiasBins];

  TH1F *h_muMinusCurveMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaPlusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaMinusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPhiMinusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPhiZeroMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaPlusPhiPlusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPhiMinusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPhiZeroMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaMinusPhiPlusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaAllPhiMinusMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaAllPhiZeroMinusBias[     nBiasBins];
  TH1F *h_muMinusCurveEtaAllPhiPlusMinusBias[     nBiasBins];

  TH1F *h_muPlusCurvePlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPhiMinusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPhiZeroPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPhiPlusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPhiMinusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPhiZeroPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPhiPlusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaAllPhiMinusPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaAllPhiZeroPlusBias[       nBiasBins];
  TH1F *h_muPlusCurveEtaAllPhiPlusPlusBias[       nBiasBins];
  
  TH1F *h_muPlusCurveMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaPlusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaMinusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPhiMinusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPhiZeroMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaPlusPhiPlusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPhiMinusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPhiZeroMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaMinusPhiPlusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaAllPhiMinusMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaAllPhiZeroMinusBias[      nBiasBins];
  TH1F *h_muPlusCurveEtaAllPhiPlusMinusBias[      nBiasBins];
  
  TH1F *h_muUpperMinusCurvePlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaAllPhiMinusPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaAllPhiZeroPlusBias[ nBiasBins];
  TH1F *h_muUpperMinusCurveEtaAllPhiPlusPlusBias[ nBiasBins];



  TH1F *h_muUpperMinusCurveMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPhiMinusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPhiZeroMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaPlusPhiPlusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPhiMinusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPhiZeroMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaMinusPhiPlusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaAllPhiMinusMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaAllPhiZeroMinusBias[nBiasBins];
  TH1F *h_muUpperMinusCurveEtaAllPhiPlusMinusBias[nBiasBins];


  TH1F *h_muUpperPlusCurvePlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaAllPhiMinusPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaAllPhiZeroPlusBias[  nBiasBins];
  TH1F *h_muUpperPlusCurveEtaAllPhiPlusPlusBias[  nBiasBins];

  TH1F *h_muUpperPlusCurveMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaPlusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaMinusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaAllPhiMinusMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaAllPhiZeroMinusBias[ nBiasBins];
  TH1F *h_muUpperPlusCurveEtaAllPhiPlusMinusBias[ nBiasBins];

  TH1F *h_muLowerMinusCurvePlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaAllPhiMinusPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaAllPhiZeroPlusBias[ nBiasBins];
  TH1F *h_muLowerMinusCurveEtaAllPhiPlusPlusBias[ nBiasBins];

  TH1F *h_muLowerMinusCurveMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPhiMinusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPhiZeroMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaPlusPhiPlusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPhiMinusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPhiZeroMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaMinusPhiPlusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaAllPhiMinusMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaAllPhiZeroMinusBias[nBiasBins];
  TH1F *h_muLowerMinusCurveEtaAllPhiPlusMinusBias[nBiasBins];


  TH1F *h_muLowerPlusCurvePlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaAllPhiMinusPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaAllPhiZeroPlusBias[  nBiasBins];
  TH1F *h_muLowerPlusCurveEtaAllPhiPlusPlusBias[  nBiasBins];

  TH1F *h_muLowerPlusCurveMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaPlusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaMinusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaAllPhiMinusMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaAllPhiZeroMinusBias[ nBiasBins];
  TH1F *h_muLowerPlusCurveEtaAllPhiPlusMinusBias[ nBiasBins];



  for (int i =0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    std::stringstream ptitle;
    ptitle << "#Delta#kappa = +" << (i+1)*(factor_*maxBias/nBiasBins);
    std::stringstream mtitle;
    mtitle << "#Delta#kappa = -" << (i+1)*(factor_*maxBias/nBiasBins);
    h_muMinusCurvePlusBias[i]       = new TH1F(TString("muMinusCurvePlusBias"       + name.str()),
					       TString("muMinusCurvePlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaPlusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaPlusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaPlusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaMinusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaMinusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaMinusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaPlusPhiMinusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaPlusPhiMinusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaPlusPhiMinusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaPlusPhiZeroPlusBias[i]       = new TH1F(TString("muMinusCurveEtaPlusPhiZeroPlusBias"       + name.str()),
					       TString("muMinusCurveEtaPlusPhiZeroPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaPlusPhiPlusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaPlusPhiPlusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaPlusPhiPlusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaMinusPhiMinusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaMinusPhiMinusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaMinusPhiMinusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaMinusPhiZeroPlusBias[i]       = new TH1F(TString("muMinusCurveEtaMinusPhiZeroPlusBias"       + name.str()),
					       TString("muMinusCurveEtaMinusPhiZeroPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaMinusPhiPlusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaMinusPhiPlusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaMinusPhiPlusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaAllPhiMinusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaAllPhiMinusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaAllPhiMinusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaAllPhiZeroPlusBias[i]       = new TH1F(TString("muMinusCurveEtaAllPhiZeroPlusBias"       + name.str()),
					       TString("muMinusCurveEtaAllPhiZeroPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muMinusCurveEtaAllPhiPlusPlusBias[i]       = new TH1F(TString("muMinusCurveEtaAllPhiPlusPlusBias"       + name.str()),
					       TString("muMinusCurveEtaAllPhiPlusPlusBias"       + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


    h_muMinusCurveMinusBias[i]      = new TH1F(TString("muMinusCurveMinusBias"      + name.str()),
					       TString("muMinusCurveMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaPlusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaPlusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaPlusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaMinusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaMinusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaMinusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaPlusPhiMinusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaPlusPhiMinusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaPlusPhiMinusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaPlusPhiZeroMinusBias[i]      = new TH1F(TString("muMinusCurveEtaPlusPhiZeroMinusBias"      + name.str()),
					       TString("muMinusCurveEtaPlusPhiZeroMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaPlusPhiPlusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaPlusPhiPlusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaPlusPhiPlusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaMinusPhiMinusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaMinusPhiMinusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaMinusPhiMinusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaMinusPhiZeroMinusBias[i]      = new TH1F(TString("muMinusCurveEtaMinusPhiZeroMinusBias"      + name.str()),
					       TString("muMinusCurveEtaMinusPhiZeroMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaMinusPhiPlusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaMinusPhiPlusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaMinusPhiPlusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muMinusCurveEtaAllPhiMinusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaAllPhiMinusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaAllPhiMinusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_); 
    h_muMinusCurveEtaAllPhiZeroMinusBias[i]      = new TH1F(TString("muMinusCurveEtaAllPhiZeroMinusBias"      + name.str()),
					       TString("muMinusCurveEtaAllPhiZeroMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_); 
    h_muMinusCurveEtaAllPhiPlusMinusBias[i]      = new TH1F(TString("muMinusCurveEtaAllPhiPlusMinusBias"      + name.str()),
					       TString("muMinusCurveEtaAllPhiPlusMinusBias"      + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_); 
 

    h_muPlusCurvePlusBias[i]        = new TH1F(TString("muPlusCurvePlusBias"        + name.str()),
					       TString("muPlusCurvePlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaPlusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaPlusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaPlusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaMinusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaMinusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaMinusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaPlusPhiMinusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaPlusPhiMinusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaPlusPhiMinusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaPlusPhiZeroPlusBias[i]        = new TH1F(TString("muPlusCurveEtaPlusPhiZeroPlusBias"        + name.str()),
					       TString("muPlusCurveEtaPlusPhiZeroPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaPlusPhiPlusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaPlusPhiPlusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaPlusPhiPlusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaMinusPhiMinusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaMinusPhiMinusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaMinusPhiMinusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaMinusPhiZeroPlusBias[i]        = new TH1F(TString("muPlusCurveEtaMinusPhiZeroPlusBias"        + name.str()),
					       TString("muPlusCurveEtaMinusPhiZeroPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaMinusPhiPlusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaMinusPhiPlusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaMinusPhiPlusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaAllPhiMinusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaAllPhiMinusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaAllPhiMinusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaAllPhiZeroPlusBias[i]        = new TH1F(TString("muPlusCurveEtaAllPhiZeroPlusBias"        + name.str()),
					       TString("muPlusCurveEtaAllPhiZeroPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muPlusCurveEtaAllPhiPlusPlusBias[i]        = new TH1F(TString("muPlusCurveEtaAllPhiPlusPlusBias"        + name.str()),
					       TString("muPlusCurveEtaAllPhiPlusPlusBias"        + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



    h_muPlusCurveMinusBias[i]       = new TH1F(TString("muPlusCurveMinusBias"       + name.str()),
					       TString("muPlusCurveMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaPlusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaPlusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaPlusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaMinusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaMinusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaMinusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaPlusPhiMinusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaPlusPhiMinusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaPlusPhiMinusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaPlusPhiZeroMinusBias[i]       = new TH1F(TString("muPlusCurveEtaPlusPhiZeroMinusBias"       + name.str()),
					       TString("muPlusCurveEtaPlusPhiZeroMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaPlusPhiPlusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaPlusPhiPlusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaPlusPhiPlusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaMinusPhiMinusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaMinusPhiMinusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaMinusPhiMinusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaMinusPhiZeroMinusBias[i]       = new TH1F(TString("muPlusCurveEtaMinusPhiZeroMinusBias"       + name.str()),
					       TString("muPlusCurveEtaMinusPhiZeroMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaMinusPhiPlusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaMinusPhiPlusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaMinusPhiPlusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaAllPhiMinusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaAllPhiMinusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaAllPhiMinusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaAllPhiZeroMinusBias[i]       = new TH1F(TString("muPlusCurveEtaAllPhiZeroMinusBias"       + name.str()),
					       TString("muPlusCurveEtaAllPhiZeroMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muPlusCurveEtaAllPhiPlusMinusBias[i]       = new TH1F(TString("muPlusCurveEtaAllPhiPlusMinusBias"       + name.str()),
					       TString("muPlusCurveEtaAllPhiPlusMinusBias"       + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  



    h_muUpperMinusCurvePlusBias[i]  = new TH1F(TString("muUpperMinusCurvePlusBias"  + name.str()),
					       TString("muUpperMinusCurvePlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaPlusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaPlusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaMinusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaMinusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaPlusPhiMinusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaPlusPhiMinusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaPlusPhiMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaPlusPhiZeroPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaPlusPhiZeroPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaPlusPhiZeroPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaPlusPhiPlusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaPlusPhiPlusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaPlusPhiPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaMinusPhiMinusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaMinusPhiMinusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaMinusPhiMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaMinusPhiZeroPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaMinusPhiZeroPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaMinusPhiZeroPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaMinusPhiPlusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaMinusPhiPlusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaMinusPhiPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaAllPhiMinusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaAllPhiMinusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaAllPhiMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaAllPhiZeroPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaAllPhiZeroPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaAllPhiZeroPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperMinusCurveEtaAllPhiPlusPlusBias[i]  = new TH1F(TString("muUpperMinusCurveEtaAllPhiPlusPlusBias"  + name.str()),
					       TString("muUpperMinusCurveEtaAllPhiPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);




    h_muUpperMinusCurveMinusBias[i] = new TH1F(TString("muUpperMinusCurveMinusBias" + name.str()),
					       TString("muUpperMinusCurveMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaPlusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaPlusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaMinusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaMinusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaPlusPhiMinusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaPlusPhiMinusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaPlusPhiMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaPlusPhiZeroMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaPlusPhiZeroMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaPlusPhiZeroMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaPlusPhiPlusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaPlusPhiPlusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaPlusPhiMinusPlusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaMinusPhiMinusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaMinusPhiMinusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaMinusPhiMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaMinusPhiZeroMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaMinusPhiZeroMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaMinusPhiZeroMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaMinusPhiPlusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaMinusPhiPlusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaMinusPhiPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaAllPhiMinusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaAllPhiMinusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaAllPhiMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaAllPhiZeroMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaAllPhiZeroMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaAllPhiZeroMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperMinusCurveEtaAllPhiPlusMinusBias[i] = new TH1F(TString("muUpperMinusCurveEtaAllPhiPlusMinusBias" + name.str()),
					       TString("muUpperMinusCurveEtaAllPhiPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  

    h_muUpperPlusCurvePlusBias[i]   = new TH1F(TString("muUpperPlusCurvePlusBias"   + name.str()),
					       TString("muUpperPlusCurvePlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaPlusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaPlusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaMinusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaMinusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaPlusPhiMinusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaPlusPhiMinusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaPlusPhiMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaPlusPhiZeroPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaPlusPhiZeroPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaPlusPhiZeroPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaPlusPhiPlusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaPlusPhiPlusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaPlusPhiPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaMinusPhiMinusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaMinusPhiMinusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaMinusPhiMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaMinusPhiZeroPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaMinusPhiZeroPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaMinusPhiZeroPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaMinusPhiPlusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaMinusPhiPlusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaMinusPhiPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaAllPhiMinusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaAllPhiMinusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaAllPhiMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaAllPhiZeroPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaAllPhiZeroPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaAllPhiZeroPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muUpperPlusCurveEtaAllPhiPlusPlusBias[i]   = new TH1F(TString("muUpperPlusCurveEtaAllPhiPlusPlusBias"   + name.str()),
					       TString("muUpperPlusCurveEtaAllPhiPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


    h_muUpperPlusCurveMinusBias[i]  = new TH1F(TString("muUpperPlusCurveMinusBias"  + name.str()),
					       TString("muUpperPlusCurveMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaPlusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaPlusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaMinusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaMinusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaPlusPhiMinusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaPlusPhiMinusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaPlusPhiMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaPlusPhiZeroMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaPlusPhiZeroMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaPlusPhiZeroMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaPlusPhiPlusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaPlusPhiPlusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaPlusPhiPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaMinusPhiMinusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaMinusPhiMinusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaMinusPhiMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaMinusPhiZeroMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaMinusPhiZeroMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaMinusPhiZeroMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaMinusPhiPlusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaMinusPhiPlusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaMinusPhiPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaAllPhiMinusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaAllPhiMinusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaAllPhiMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaAllPhiZeroMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaAllPhiZeroMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaAllPhiZeroMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muUpperPlusCurveEtaAllPhiPlusMinusBias[i]  = new TH1F(TString("muUpperPlusCurveEtaAllPhiPlusMinusBias"  + name.str()),
					       TString("muUpperPlusCurveEtaAllPhiPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  


    h_muLowerMinusCurvePlusBias[i]  = new TH1F(TString("muLowerMinusCurvePlusBias"  + name.str()),
					       TString("muLowerMinusCurvePlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaPlusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaPlusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaMinusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaMinusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaPlusPhiMinusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaPlusPhiMinusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaPlusPhiMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaPlusPhiZeroPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaPlusPhiZeroPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaPlusPhiZeroPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaPlusPhiPlusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaPlusPhiPlusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaPlusPhiPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaMinusPhiMinusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaMinusPhiMinusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaMinusPhiMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaMinusPhiZeroPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaMinusPhiZeroPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaMinusPhiZeroPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaMinusPhiPlusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaMinusPhiPlusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaMinusPhiPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaAllPhiMinusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaAllPhiMinusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaAllPhiMinusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaAllPhiZeroPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaAllPhiZeroPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaAllPhiZeroPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerMinusCurveEtaAllPhiPlusPlusBias[i]  = new TH1F(TString("muLowerMinusCurveEtaAllPhiPlusPlusBias"  + name.str()),
					       TString("muLowerMinusCurveEtaAllPhiPlusPlusBias"  + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



    h_muLowerMinusCurveMinusBias[i] = new TH1F(TString("muLowerMinusCurveMinusBias" + name.str()),
					       TString("muLowerMinusCurveMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaPlusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaPlusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaMinusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaMinusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaPlusPhiMinusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaPlusPhiMinusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaPlusPhiMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaPlusPhiZeroMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaPlusPhiZeroMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaPlusPhiZeroMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaPlusPhiPlusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaPlusPhiPlusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaPlusPhiPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaMinusPhiMinusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaMinusPhiMinusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaMinusPhiMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaMinusPhiZeroMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaMinusPhiZeroMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaMinusPhiZeroMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaMinusPhiPlusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaMinusPhiPlusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaMinusPhiPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaAllPhiMinusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaAllPhiMinusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaAllPhiMinusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaAllPhiZeroMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaAllPhiZeroMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaAllPhiZeroMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerMinusCurveEtaAllPhiPlusMinusBias[i] = new TH1F(TString("muLowerMinusCurveEtaAllPhiPlusMinusBias" + name.str()),
					       TString("muLowerMinusCurveEtaAllPhiPlusMinusBias" + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  



    h_muLowerPlusCurvePlusBias[i]   = new TH1F(TString("muLowerPlusCurvePlusBias"   + name.str()),
					       TString("muLowerPlusCurvePlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaPlusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaPlusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaMinusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaMinusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaPlusPhiMinusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaPlusPhiMinusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaPlusPhiMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaPlusPhiZeroPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaPlusPhiZeroPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaPlusPhiZeroPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaPlusPhiPlusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaPlusPhiPlusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaPlusPhiPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaMinusPhiMinusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaMinusPhiMinusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaMinusPhiMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaMinusPhiZeroPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaMinusPhiZeroPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaMinusPhiZeroPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaMinusPhiPlusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaMinusPhiPlusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaMinusPhiPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaAllPhiMinusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaAllPhiMinusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaAllPhiMinusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaAllPhiZeroPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaAllPhiZeroPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaAllPhiZeroPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_muLowerPlusCurveEtaAllPhiPlusPlusBias[i]   = new TH1F(TString("muLowerPlusCurveEtaAllPhiPlusPlusBias"   + name.str()),
					       TString("muLowerPlusCurveEtaAllPhiPlusPlusBias"   + ptitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


    h_muLowerPlusCurveMinusBias[i]  = new TH1F(TString("muLowerPlusCurveMinusBias"  + name.str()),
					       TString("muLowerPlusCurveMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaPlusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaPlusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaMinusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaMinusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaPlusPhiMinusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaPlusPhiMinusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaPlusPhiMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaPlusPhiZeroMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaPlusPhiZeroMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaPlusPhiZeroMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaPlusPhiPlusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaPlusPhiPlusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaPlusPhiPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaMinusPhiMinusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaMinusPhiMinusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaMinusPhiMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaMinusPhiZeroMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaMinusPhiZeroMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaMinusPhiZeroMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaMinusPhiPlusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaMinusPhiPlusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaMinusPhiPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaAllPhiMinusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaAllPhiMinusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaAllPhiMinusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaAllPhiZeroMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaAllPhiZeroMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaAllPhiZeroMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_muLowerPlusCurveEtaAllPhiPlusMinusBias[i]  = new TH1F(TString("muLowerPlusCurveEtaAllPhiPlusMinusBias"  + name.str()),
					       TString("muLowerPlusCurveEtaAllPhiPlusMinusBias"  + mtitle.str()),
					       symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  


  }

  // histograms for loose cuts (not applying the Dxy/Dz cuts)
  TH1F *h_looseMuMinusPt      = new TH1F("looseMuMinusPt",     "looseMuMinusPt",      300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaPlus      = new TH1F("looseMuMinusPtEtaPlus","looseMuMinusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaMinus      = new TH1F("looseMuMinusPtEtaMinus","looseMuMinusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaPlusPhiMinus      = new TH1F("looseMuMinusPtEtaPlusPhiMinus","looseMuMinusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaPlusPhiZero      = new TH1F("looseMuMinusPtEtaPlusPhiZero","looseMuMinusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaPlusPhiPlus      = new TH1F("looseMuMinusPtEtaPlusPhiPlus","looseMuMinusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaMinusPhiMinus      = new TH1F("looseMuMinusPtEtaMinusPhiMinus","looseMuMinusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaMinusPhiZero      = new TH1F("looseMuMinusPtEtaMinusPhiZero", "looseMuMinusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuMinusPtEtaMinusPhiPlus      = new TH1F("looseMuMinusPtEtaMinusPhiPlus", "looseMuMinusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_looseMuPlusPt       = new TH1F("looseMuPlusPt",      "looseMuPlusPt",       300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaPlus      = new TH1F("looseMuPlusPtEtaPlus","looseMuPlusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaMinus      = new TH1F("looseMuPlusPtEtaMinus","looseMuPlusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaPlusPhiMinus      = new TH1F("looseMuPlusPtEtaPlusPhiMinus","looseMuPlusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaPlusPhiZero      = new TH1F("looseMuPlusPtEtaPlusPhiZero","looseMuPlusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaPlusPhiPlus      = new TH1F("looseMuPlusPtEtaPlusPhiPlus","looseMuPlusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaMinusPhiMinus      = new TH1F("looseMuPlusPtEtaMinusPhiMinus","looseMuPlusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaMinusPhiZero      = new TH1F("looseMuPlusPtEtaMinusPhiZero", "looseMuPlusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuPlusPtEtaMinusPhiPlus      = new TH1F("looseMuPlusPtEtaMinusPhiPlus", "looseMuPlusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_looseMuUpperMinusPt = new TH1F("looseMuUpperMinusPt","looseMuUpperMinusPt", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaPlus      = new TH1F("looseMuUpperMinusPtEtaPlus","looseMuUpperMinusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaMinus      = new TH1F("looseMuUpperMinusPtEtaMinus","looseMuUpperMinusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusPtEtaPlusPhiMinus","looseMuUpperMinusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaPlusPhiZero      = new TH1F("looseMuUpperMinusPtEtaPlusPhiZero","looseMuUpperMinusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusPtEtaPlusPhiPlus","looseMuUpperMinusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusPtEtaMinusPhiMinus","looseMuUpperMinusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaMinusPhiZero      = new TH1F("looseMuUpperMinusPtEtaMinusPhiZero", "looseMuUpperMinusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusPtEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusPtEtaMinusPhiPlus", "looseMuUpperMinusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_looseMuUpperPlusPt  = new TH1F("looseMuUpperPlusPt", "looseMuUpperPlusPt",  300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaPlus      = new TH1F("looseMuUpperPlusPtEtaPlus","looseMuUpperPlusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaMinus      = new TH1F("looseMuUpperPlusPtEtaMinus","looseMuUpperPlusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusPtEtaPlusPhiMinus","looseMuUpperPlusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaPlusPhiZero      = new TH1F("looseMuUpperPlusPtEtaPlusPhiZero","looseMuUpperPlusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusPtEtaPlusPhiPlus","looseMuUpperPlusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusPtEtaMinusPhiMinus","looseMuUpperPlusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaMinusPhiZero      = new TH1F("looseMuUpperPlusPtEtaMinusPhiZero", "looseMuUpperPlusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusPtEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusPtEtaMinusPhiPlus", "looseMuUpperPlusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_looseMuLowerMinusPt = new TH1F("looseMuLowerMinusPt","looseMuLowerMinusPt", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaPlus      = new TH1F("looseMuLowerMinusPtEtaPlus","looseMuLowerMinusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaMinus      = new TH1F("looseMuLowerMinusPtEtaMinus","looseMuLowerMinusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusPtEtaPlusPhiMinus","looseMuLowerMinusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaPlusPhiZero      = new TH1F("looseMuLowerMinusPtEtaPlusPhiZero","looseMuLowerMinusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusPtEtaPlusPhiPlus","looseMuLowerMinusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusPtEtaMinusPhiMinus","looseMuLowerMinusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaMinusPhiZero      = new TH1F("looseMuLowerMinusPtEtaMinusPhiZero", "looseMuLowerMinusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusPtEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusPtEtaMinusPhiPlus", "looseMuLowerMinusPtEtaMinusPhiPlus", 300, 0., 3000.);

  TH1F *h_looseMuLowerPlusPt  = new TH1F("looseMuLowerPlusPt", "looseMuLowerPlusPt",  300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaPlus      = new TH1F("looseMuLowerPlusPtEtaPlus","looseMuLowerPlusPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaMinus      = new TH1F("looseMuLowerPlusPtEtaMinus","looseMuLowerPlusPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusPtEtaPlusPhiMinus","looseMuLowerPlusPtEtaPlusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaPlusPhiZero      = new TH1F("looseMuLowerPlusPtEtaPlusPhiZero","looseMuLowerPlusPtEtaPlusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusPtEtaPlusPhiPlus","looseMuLowerPlusPtEtaPlusPhiPlus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusPtEtaMinusPhiMinus","looseMuLowerPlusPtEtaMinusPhiMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaMinusPhiZero      = new TH1F("looseMuLowerPlusPtEtaMinusPhiZero", "looseMuLowerPlusPtEtaMinusPhiZero", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusPtEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusPtEtaMinusPhiPlus", "looseMuLowerPlusPtEtaMinusPhiPlus", 300, 0., 3000.);


  TH1F *h_looseMuMinusEta      = new TH1F("looseMuMinusEta",     "looseMuMinusEta",      40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaPlus      = new TH1F("looseMuMinusEtaEtaPlus","looseMuMinusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaMinus      = new TH1F("looseMuMinusEtaEtaMinus","looseMuMinusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaPlusPhiMinus      = new TH1F("looseMuMinusEtaEtaPlusPhiMinus","looseMuMinusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaPlusPhiZero      = new TH1F("looseMuMinusEtaEtaPlusPhiZero","looseMuMinusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaPlusPhiPlus      = new TH1F("looseMuMinusEtaEtaPlusPhiPlus","looseMuMinusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaMinusPhiMinus      = new TH1F("looseMuMinusEtaEtaMinusPhiMinus","looseMuMinusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaMinusPhiZero      = new TH1F("looseMuMinusEtaEtaMinusPhiZero", "looseMuMinusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuMinusEtaEtaMinusPhiPlus      = new TH1F("looseMuMinusEtaEtaMinusPhiPlus", "looseMuMinusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_looseMuPlusEta       = new TH1F("looseMuPlusEta",      "looseMuPlusEta",       40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaPlus      = new TH1F("looseMuPlusEtaEtaPlus","looseMuPlusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaMinus      = new TH1F("looseMuPlusEtaEtaMinus","looseMuPlusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaPlusPhiMinus      = new TH1F("looseMuPlusEtaEtaPlusPhiMinus","looseMuPlusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaPlusPhiZero      = new TH1F("looseMuPlusEtaEtaPlusPhiZero","looseMuPlusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaPlusPhiPlus      = new TH1F("looseMuPlusEtaEtaPlusPhiPlus","looseMuPlusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaMinusPhiMinus      = new TH1F("looseMuPlusEtaEtaMinusPhiMinus","looseMuPlusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaMinusPhiZero      = new TH1F("looseMuPlusEtaEtaMinusPhiZero", "looseMuPlusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuPlusEtaEtaMinusPhiPlus      = new TH1F("looseMuPlusEtaEtaMinusPhiPlus", "looseMuPlusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_looseMuUpperMinusEta = new TH1F("looseMuUpperMinusEta","looseMuUpperMinusEta", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaPlus      = new TH1F("looseMuUpperMinusEtaEtaPlus","looseMuUpperMinusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaMinus      = new TH1F("looseMuUpperMinusEtaEtaMinus","looseMuUpperMinusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusEtaEtaPlusPhiMinus","looseMuUpperMinusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaPlusPhiZero      = new TH1F("looseMuUpperMinusEtaEtaPlusPhiZero","looseMuUpperMinusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusEtaEtaPlusPhiPlus","looseMuUpperMinusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusEtaEtaMinusPhiMinus","looseMuUpperMinusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaMinusPhiZero      = new TH1F("looseMuUpperMinusEtaEtaMinusPhiZero", "looseMuUpperMinusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusEtaEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusEtaEtaMinusPhiPlus", "looseMuUpperMinusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_looseMuUpperPlusEta  = new TH1F("looseMuUpperPlusEta", "looseMuUpperPlusEta",  40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaPlus      = new TH1F("looseMuUpperPlusEtaEtaPlus","looseMuUpperPlusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaMinus      = new TH1F("looseMuUpperPlusEtaEtaMinus","looseMuUpperPlusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusEtaEtaPlusPhiMinus","looseMuUpperPlusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaPlusPhiZero      = new TH1F("looseMuUpperPlusEtaEtaPlusPhiZero","looseMuUpperPlusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusEtaEtaPlusPhiPlus","looseMuUpperPlusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusEtaEtaMinusPhiMinus","looseMuUpperPlusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaMinusPhiZero      = new TH1F("looseMuUpperPlusEtaEtaMinusPhiZero", "looseMuUpperPlusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusEtaEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusEtaEtaMinusPhiPlus", "looseMuUpperPlusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_looseMuLowerMinusEta = new TH1F("looseMuLowerMinusEta","looseMuLowerMinusEta", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaPlus      = new TH1F("looseMuLowerMinusEtaEtaPlus","looseMuLowerMinusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaMinus      = new TH1F("looseMuLowerMinusEtaEtaMinus","looseMuLowerMinusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusEtaEtaPlusPhiMinus","looseMuLowerMinusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaPlusPhiZero      = new TH1F("looseMuLowerMinusEtaEtaPlusPhiZero","looseMuLowerMinusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusEtaEtaPlusPhiPlus","looseMuLowerMinusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusEtaEtaMinusPhiMinus","looseMuLowerMinusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaMinusPhiZero      = new TH1F("looseMuLowerMinusEtaEtaMinusPhiZero", "looseMuLowerMinusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusEtaEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusEtaEtaMinusPhiPlus", "looseMuLowerMinusEtaEtaMinusPhiPlus", 40, -2., 2.);

  TH1F *h_looseMuLowerPlusEta  = new TH1F("looseMuLowerPlusEta", "looseMuLowerPlusEta",  40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaPlus      = new TH1F("looseMuLowerPlusEtaEtaPlus","looseMuLowerPlusEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaMinus      = new TH1F("looseMuLowerPlusEtaEtaMinus","looseMuLowerPlusEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusEtaEtaPlusPhiMinus","looseMuLowerPlusEtaEtaPlusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaPlusPhiZero      = new TH1F("looseMuLowerPlusEtaEtaPlusPhiZero","looseMuLowerPlusEtaEtaPlusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusEtaEtaPlusPhiPlus","looseMuLowerPlusEtaEtaPlusPhiPlus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusEtaEtaMinusPhiMinus","looseMuLowerPlusEtaEtaMinusPhiMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaMinusPhiZero      = new TH1F("looseMuLowerPlusEtaEtaMinusPhiZero", "looseMuLowerPlusEtaEtaMinusPhiZero", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusEtaEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusEtaEtaMinusPhiPlus", "looseMuLowerPlusEtaEtaMinusPhiPlus", 40, -2., 2.);


  TH1F *h_looseMuMinusPhi      = new TH1F("looseMuMinusPhi",     "looseMuMinusPhi",      40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaPlus      = new TH1F("looseMuMinusPhiEtaPlus","looseMuMinusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaMinus      = new TH1F("looseMuMinusPhiEtaMinus","looseMuMinusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaPlusPhiMinus      = new TH1F("looseMuMinusPhiEtaPlusPhiMinus","looseMuMinusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaPlusPhiZero      = new TH1F("looseMuMinusPhiEtaPlusPhiZero","looseMuMinusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaPlusPhiPlus      = new TH1F("looseMuMinusPhiEtaPlusPhiPlus","looseMuMinusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaMinusPhiMinus      = new TH1F("looseMuMinusPhiEtaMinusPhiMinus","looseMuMinusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaMinusPhiZero      = new TH1F("looseMuMinusPhiEtaMinusPhiZero", "looseMuMinusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuMinusPhiEtaMinusPhiPlus      = new TH1F("looseMuMinusPhiEtaMinusPhiPlus", "looseMuMinusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_looseMuPlusPhi       = new TH1F("looseMuPlusPhi",      "looseMuPlusPhi",       40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaPlus      = new TH1F("looseMuPlusPhiEtaPlus","looseMuPlusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaMinus      = new TH1F("looseMuPlusPhiEtaMinus","looseMuPlusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaPlusPhiMinus      = new TH1F("looseMuPlusPhiEtaPlusPhiMinus","looseMuPlusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaPlusPhiZero      = new TH1F("looseMuPlusPhiEtaPlusPhiZero","looseMuPlusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaPlusPhiPlus      = new TH1F("looseMuPlusPhiEtaPlusPhiPlus","looseMuPlusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaMinusPhiMinus      = new TH1F("looseMuPlusPhiEtaMinusPhiMinus","looseMuPlusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaMinusPhiZero      = new TH1F("looseMuPlusPhiEtaMinusPhiZero", "looseMuPlusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuPlusPhiEtaMinusPhiPlus      = new TH1F("looseMuPlusPhiEtaMinusPhiPlus", "looseMuPlusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_looseMuUpperMinusPhi = new TH1F("looseMuUpperMinusPhi","looseMuUpperMinusPhi", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaPlus      = new TH1F("looseMuUpperMinusPhiEtaPlus","looseMuUpperMinusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaMinus      = new TH1F("looseMuUpperMinusPhiEtaMinus","looseMuUpperMinusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusPhiEtaPlusPhiMinus","looseMuUpperMinusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaPlusPhiZero      = new TH1F("looseMuUpperMinusPhiEtaPlusPhiZero","looseMuUpperMinusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusPhiEtaPlusPhiPlus","looseMuUpperMinusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusPhiEtaMinusPhiMinus","looseMuUpperMinusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaMinusPhiZero      = new TH1F("looseMuUpperMinusPhiEtaMinusPhiZero", "looseMuUpperMinusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusPhiEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusPhiEtaMinusPhiPlus", "looseMuUpperMinusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_looseMuUpperPlusPhi  = new TH1F("looseMuUpperPlusPhi", "looseMuUpperPlusPhi",  40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaPlus      = new TH1F("looseMuUpperPlusPhiEtaPlus","looseMuUpperPlusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaMinus      = new TH1F("looseMuUpperPlusPhiEtaMinus","looseMuUpperPlusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusPhiEtaPlusPhiMinus","looseMuUpperPlusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaPlusPhiZero      = new TH1F("looseMuUpperPlusPhiEtaPlusPhiZero","looseMuUpperPlusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusPhiEtaPlusPhiPlus","looseMuUpperPlusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusPhiEtaMinusPhiMinus","looseMuUpperPlusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaMinusPhiZero      = new TH1F("looseMuUpperPlusPhiEtaMinusPhiZero", "looseMuUpperPlusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusPhiEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusPhiEtaMinusPhiPlus", "looseMuUpperPlusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_looseMuLowerMinusPhi = new TH1F("looseMuLowerMinusPhi","looseMuLowerMinusPhi", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaPlus      = new TH1F("looseMuLowerMinusPhiEtaPlus","looseMuLowerMinusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaMinus      = new TH1F("looseMuLowerMinusPhiEtaMinus","looseMuLowerMinusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusPhiEtaPlusPhiMinus","looseMuLowerMinusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaPlusPhiZero      = new TH1F("looseMuLowerMinusPhiEtaPlusPhiZero","looseMuLowerMinusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusPhiEtaPlusPhiPlus","looseMuLowerMinusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusPhiEtaMinusPhiMinus","looseMuLowerMinusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaMinusPhiZero      = new TH1F("looseMuLowerMinusPhiEtaMinusPhiZero", "looseMuLowerMinusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusPhiEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusPhiEtaMinusPhiPlus", "looseMuLowerMinusPhiEtaMinusPhiPlus", 40, -4., 4.);

  TH1F *h_looseMuLowerPlusPhi  = new TH1F("looseMuLowerPlusPhi", "looseMuLowerPlusPhi",  40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaPlus      = new TH1F("looseMuLowerPlusPhiEtaPlus","looseMuLowerPlusPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaMinus      = new TH1F("looseMuLowerPlusPhiEtaMinus","looseMuLowerPlusPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusPhiEtaPlusPhiMinus","looseMuLowerPlusPhiEtaPlusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaPlusPhiZero      = new TH1F("looseMuLowerPlusPhiEtaPlusPhiZero","looseMuLowerPlusPhiEtaPlusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusPhiEtaPlusPhiPlus","looseMuLowerPlusPhiEtaPlusPhiPlus", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusPhiEtaMinusPhiMinus","looseMuLowerPlusPhiEtaMinusPhiMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaMinusPhiZero      = new TH1F("looseMuLowerPlusPhiEtaMinusPhiZero", "looseMuLowerPlusPhiEtaMinusPhiZero", 40, -4., 4.);
  TH1F *h_looseMuLowerPlusPhiEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusPhiEtaMinusPhiPlus", "looseMuLowerPlusPhiEtaMinusPhiPlus", 40, -4., 4.);


  TH1F *h_looseMuMinusChi2      = new TH1F("looseMuMinusChi2",     "looseMuMinusChi2",      50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaPlus      = new TH1F("looseMuMinusChi2EtaPlus","looseMuMinusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaMinus      = new TH1F("looseMuMinusChi2EtaMinus","looseMuMinusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaPlusPhiMinus      = new TH1F("looseMuMinusChi2EtaPlusPhiMinus","looseMuMinusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaPlusPhiZero      = new TH1F("looseMuMinusChi2EtaPlusPhiZero","looseMuMinusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaPlusPhiPlus      = new TH1F("looseMuMinusChi2EtaPlusPhiPlus","looseMuMinusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaMinusPhiMinus      = new TH1F("looseMuMinusChi2EtaMinusPhiMinus","looseMuMinusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaMinusPhiZero      = new TH1F("looseMuMinusChi2EtaMinusPhiZero", "looseMuMinusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuMinusChi2EtaMinusPhiPlus      = new TH1F("looseMuMinusChi2EtaMinusPhiPlus", "looseMuMinusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_looseMuPlusChi2       = new TH1F("looseMuPlusChi2",      "looseMuPlusChi2",       50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaPlus      = new TH1F("looseMuPlusChi2EtaPlus","looseMuPlusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaMinus      = new TH1F("looseMuPlusChi2EtaMinus","looseMuPlusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaPlusPhiMinus      = new TH1F("looseMuPlusChi2EtaPlusPhiMinus","looseMuPlusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaPlusPhiZero      = new TH1F("looseMuPlusChi2EtaPlusPhiZero","looseMuPlusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaPlusPhiPlus      = new TH1F("looseMuPlusChi2EtaPlusPhiPlus","looseMuPlusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaMinusPhiMinus      = new TH1F("looseMuPlusChi2EtaMinusPhiMinus","looseMuPlusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaMinusPhiZero      = new TH1F("looseMuPlusChi2EtaMinusPhiZero", "looseMuPlusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuPlusChi2EtaMinusPhiPlus      = new TH1F("looseMuPlusChi2EtaMinusPhiPlus", "looseMuPlusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_looseMuUpperMinusChi2 = new TH1F("looseMuUpperMinusChi2","looseMuUpperMinusChi2", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaPlus      = new TH1F("looseMuUpperMinusChi2EtaPlus","looseMuUpperMinusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaMinus      = new TH1F("looseMuUpperMinusChi2EtaMinus","looseMuUpperMinusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaPlusPhiMinus      = new TH1F("looseMuUpperMinusChi2EtaPlusPhiMinus","looseMuUpperMinusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaPlusPhiZero      = new TH1F("looseMuUpperMinusChi2EtaPlusPhiZero","looseMuUpperMinusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaPlusPhiPlus      = new TH1F("looseMuUpperMinusChi2EtaPlusPhiPlus","looseMuUpperMinusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaMinusPhiMinus      = new TH1F("looseMuUpperMinusChi2EtaMinusPhiMinus","looseMuUpperMinusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaMinusPhiZero      = new TH1F("looseMuUpperMinusChi2EtaMinusPhiZero", "looseMuUpperMinusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusChi2EtaMinusPhiPlus      = new TH1F("looseMuUpperMinusChi2EtaMinusPhiPlus", "looseMuUpperMinusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_looseMuUpperPlusChi2  = new TH1F("looseMuUpperPlusChi2", "looseMuUpperPlusChi2",  50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaPlus      = new TH1F("looseMuUpperPlusChi2EtaPlus","looseMuUpperPlusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaMinus      = new TH1F("looseMuUpperPlusChi2EtaMinus","looseMuUpperPlusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaPlusPhiMinus      = new TH1F("looseMuUpperPlusChi2EtaPlusPhiMinus","looseMuUpperPlusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaPlusPhiZero      = new TH1F("looseMuUpperPlusChi2EtaPlusPhiZero","looseMuUpperPlusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaPlusPhiPlus      = new TH1F("looseMuUpperPlusChi2EtaPlusPhiPlus","looseMuUpperPlusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaMinusPhiMinus      = new TH1F("looseMuUpperPlusChi2EtaMinusPhiMinus","looseMuUpperPlusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaMinusPhiZero      = new TH1F("looseMuUpperPlusChi2EtaMinusPhiZero", "looseMuUpperPlusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusChi2EtaMinusPhiPlus      = new TH1F("looseMuUpperPlusChi2EtaMinusPhiPlus", "looseMuUpperPlusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_looseMuLowerMinusChi2 = new TH1F("looseMuLowerMinusChi2","looseMuLowerMinusChi2", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaPlus      = new TH1F("looseMuLowerMinusChi2EtaPlus","looseMuLowerMinusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaMinus      = new TH1F("looseMuLowerMinusChi2EtaMinus","looseMuLowerMinusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaPlusPhiMinus      = new TH1F("looseMuLowerMinusChi2EtaPlusPhiMinus","looseMuLowerMinusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaPlusPhiZero      = new TH1F("looseMuLowerMinusChi2EtaPlusPhiZero","looseMuLowerMinusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaPlusPhiPlus      = new TH1F("looseMuLowerMinusChi2EtaPlusPhiPlus","looseMuLowerMinusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaMinusPhiMinus      = new TH1F("looseMuLowerMinusChi2EtaMinusPhiMinus","looseMuLowerMinusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaMinusPhiZero      = new TH1F("looseMuLowerMinusChi2EtaMinusPhiZero", "looseMuLowerMinusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusChi2EtaMinusPhiPlus      = new TH1F("looseMuLowerMinusChi2EtaMinusPhiPlus", "looseMuLowerMinusChi2EtaMinusPhiPlus", 50, 0., 150.);

  TH1F *h_looseMuLowerPlusChi2  = new TH1F("looseMuLowerPlusChi2", "looseMuLowerPlusChi2",  50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaPlus      = new TH1F("looseMuLowerPlusChi2EtaPlus","looseMuLowerPlusChi2EtaPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaMinus      = new TH1F("looseMuLowerPlusChi2EtaMinus","looseMuLowerPlusChi2EtaMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaPlusPhiMinus      = new TH1F("looseMuLowerPlusChi2EtaPlusPhiMinus","looseMuLowerPlusChi2EtaPlusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaPlusPhiZero      = new TH1F("looseMuLowerPlusChi2EtaPlusPhiZero","looseMuLowerPlusChi2EtaPlusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaPlusPhiPlus      = new TH1F("looseMuLowerPlusChi2EtaPlusPhiPlus","looseMuLowerPlusChi2EtaPlusPhiPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaMinusPhiMinus      = new TH1F("looseMuLowerPlusChi2EtaMinusPhiMinus","looseMuLowerPlusChi2EtaMinusPhiMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaMinusPhiZero      = new TH1F("looseMuLowerPlusChi2EtaMinusPhiZero", "looseMuLowerPlusChi2EtaMinusPhiZero", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusChi2EtaMinusPhiPlus      = new TH1F("looseMuLowerPlusChi2EtaMinusPhiPlus", "looseMuLowerPlusChi2EtaMinusPhiPlus", 50, 0., 150.);


  TH1F *h_looseMuMinusNdof      = new TH1F("looseMuMinusNdof",     "looseMuMinusNdof",      100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaPlus      = new TH1F("looseMuMinusNdofEtaPlus","looseMuMinusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaMinus      = new TH1F("looseMuMinusNdofEtaMinus","looseMuMinusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaPlusPhiMinus      = new TH1F("looseMuMinusNdofEtaPlusPhiMinus","looseMuMinusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaPlusPhiZero      = new TH1F("looseMuMinusNdofEtaPlusPhiZero","looseMuMinusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaPlusPhiPlus      = new TH1F("looseMuMinusNdofEtaPlusPhiPlus","looseMuMinusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaMinusPhiMinus      = new TH1F("looseMuMinusNdofEtaMinusPhiMinus","looseMuMinusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaMinusPhiZero      = new TH1F("looseMuMinusNdofEtaMinusPhiZero", "looseMuMinusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusNdofEtaMinusPhiPlus      = new TH1F("looseMuMinusNdofEtaMinusPhiPlus", "looseMuMinusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuPlusNdof       = new TH1F("looseMuPlusNdof",      "looseMuPlusNdof",       100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaPlus      = new TH1F("looseMuPlusNdofEtaPlus","looseMuPlusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaMinus      = new TH1F("looseMuPlusNdofEtaMinus","looseMuPlusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaPlusPhiMinus      = new TH1F("looseMuPlusNdofEtaPlusPhiMinus","looseMuPlusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaPlusPhiZero      = new TH1F("looseMuPlusNdofEtaPlusPhiZero","looseMuPlusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaPlusPhiPlus      = new TH1F("looseMuPlusNdofEtaPlusPhiPlus","looseMuPlusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaMinusPhiMinus      = new TH1F("looseMuPlusNdofEtaMinusPhiMinus","looseMuPlusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaMinusPhiZero      = new TH1F("looseMuPlusNdofEtaMinusPhiZero", "looseMuPlusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusNdofEtaMinusPhiPlus      = new TH1F("looseMuPlusNdofEtaMinusPhiPlus", "looseMuPlusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuUpperMinusNdof = new TH1F("looseMuUpperMinusNdof","looseMuUpperMinusNdof", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaPlus      = new TH1F("looseMuUpperMinusNdofEtaPlus","looseMuUpperMinusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaMinus      = new TH1F("looseMuUpperMinusNdofEtaMinus","looseMuUpperMinusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusNdofEtaPlusPhiMinus","looseMuUpperMinusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaPlusPhiZero      = new TH1F("looseMuUpperMinusNdofEtaPlusPhiZero","looseMuUpperMinusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusNdofEtaPlusPhiPlus","looseMuUpperMinusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusNdofEtaMinusPhiMinus","looseMuUpperMinusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaMinusPhiZero      = new TH1F("looseMuUpperMinusNdofEtaMinusPhiZero", "looseMuUpperMinusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusNdofEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusNdofEtaMinusPhiPlus", "looseMuUpperMinusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuUpperPlusNdof  = new TH1F("looseMuUpperPlusNdof", "looseMuUpperPlusNdof",  100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaPlus      = new TH1F("looseMuUpperPlusNdofEtaPlus","looseMuUpperPlusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaMinus      = new TH1F("looseMuUpperPlusNdofEtaMinus","looseMuUpperPlusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusNdofEtaPlusPhiMinus","looseMuUpperPlusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaPlusPhiZero      = new TH1F("looseMuUpperPlusNdofEtaPlusPhiZero","looseMuUpperPlusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusNdofEtaPlusPhiPlus","looseMuUpperPlusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusNdofEtaMinusPhiMinus","looseMuUpperPlusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaMinusPhiZero      = new TH1F("looseMuUpperPlusNdofEtaMinusPhiZero", "looseMuUpperPlusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusNdofEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusNdofEtaMinusPhiPlus", "looseMuUpperPlusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuLowerMinusNdof = new TH1F("looseMuLowerMinusNdof","looseMuLowerMinusNdof", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaPlus      = new TH1F("looseMuLowerMinusNdofEtaPlus","looseMuLowerMinusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaMinus      = new TH1F("looseMuLowerMinusNdofEtaMinus","looseMuLowerMinusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusNdofEtaPlusPhiMinus","looseMuLowerMinusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaPlusPhiZero      = new TH1F("looseMuLowerMinusNdofEtaPlusPhiZero","looseMuLowerMinusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusNdofEtaPlusPhiPlus","looseMuLowerMinusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusNdofEtaMinusPhiMinus","looseMuLowerMinusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaMinusPhiZero      = new TH1F("looseMuLowerMinusNdofEtaMinusPhiZero", "looseMuLowerMinusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusNdofEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusNdofEtaMinusPhiPlus", "looseMuLowerMinusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuLowerPlusNdof  = new TH1F("looseMuLowerPlusNdof", "looseMuLowerPlusNdof",  100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaPlus      = new TH1F("looseMuLowerPlusNdofEtaPlus","looseMuLowerPlusNdofEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaMinus      = new TH1F("looseMuLowerPlusNdofEtaMinus","looseMuLowerPlusNdofEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusNdofEtaPlusPhiMinus","looseMuLowerPlusNdofEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaPlusPhiZero      = new TH1F("looseMuLowerPlusNdofEtaPlusPhiZero","looseMuLowerPlusNdofEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusNdofEtaPlusPhiPlus","looseMuLowerPlusNdofEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusNdofEtaMinusPhiMinus","looseMuLowerPlusNdofEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaMinusPhiZero      = new TH1F("looseMuLowerPlusNdofEtaMinusPhiZero", "looseMuLowerPlusNdofEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusNdofEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusNdofEtaMinusPhiPlus", "looseMuLowerPlusNdofEtaMinusPhiPlus", 100, -0.5, 99.5);


  TH1F *h_looseMuMinusCharge      = new TH1F("looseMuMinusCharge",     "looseMuMinusCharge",      3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaPlus      = new TH1F("looseMuMinusChargeEtaPlus","looseMuMinusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaMinus      = new TH1F("looseMuMinusChargeEtaMinus","looseMuMinusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaPlusPhiMinus      = new TH1F("looseMuMinusChargeEtaPlusPhiMinus","looseMuMinusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaPlusPhiZero      = new TH1F("looseMuMinusChargeEtaPlusPhiZero","looseMuMinusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaPlusPhiPlus      = new TH1F("looseMuMinusChargeEtaPlusPhiPlus","looseMuMinusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaMinusPhiMinus      = new TH1F("looseMuMinusChargeEtaMinusPhiMinus","looseMuMinusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaMinusPhiZero      = new TH1F("looseMuMinusChargeEtaMinusPhiZero", "looseMuMinusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuMinusChargeEtaMinusPhiPlus      = new TH1F("looseMuMinusChargeEtaMinusPhiPlus", "looseMuMinusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_looseMuPlusCharge       = new TH1F("looseMuPlusCharge",      "looseMuPlusCharge",       3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaPlus      = new TH1F("looseMuPlusChargeEtaPlus","looseMuPlusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaMinus      = new TH1F("looseMuPlusChargeEtaMinus","looseMuPlusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaPlusPhiMinus      = new TH1F("looseMuPlusChargeEtaPlusPhiMinus","looseMuPlusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaPlusPhiZero      = new TH1F("looseMuPlusChargeEtaPlusPhiZero","looseMuPlusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaPlusPhiPlus      = new TH1F("looseMuPlusChargeEtaPlusPhiPlus","looseMuPlusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaMinusPhiMinus      = new TH1F("looseMuPlusChargeEtaMinusPhiMinus","looseMuPlusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaMinusPhiZero      = new TH1F("looseMuPlusChargeEtaMinusPhiZero", "looseMuPlusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuPlusChargeEtaMinusPhiPlus      = new TH1F("looseMuPlusChargeEtaMinusPhiPlus", "looseMuPlusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_looseMuUpperMinusCharge = new TH1F("looseMuUpperMinusCharge","looseMuUpperMinusCharge", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaPlus      = new TH1F("looseMuUpperMinusChargeEtaPlus","looseMuUpperMinusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaMinus      = new TH1F("looseMuUpperMinusChargeEtaMinus","looseMuUpperMinusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusChargeEtaPlusPhiMinus","looseMuUpperMinusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaPlusPhiZero      = new TH1F("looseMuUpperMinusChargeEtaPlusPhiZero","looseMuUpperMinusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusChargeEtaPlusPhiPlus","looseMuUpperMinusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusChargeEtaMinusPhiMinus","looseMuUpperMinusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaMinusPhiZero      = new TH1F("looseMuUpperMinusChargeEtaMinusPhiZero", "looseMuUpperMinusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperMinusChargeEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusChargeEtaMinusPhiPlus", "looseMuUpperMinusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_looseMuUpperPlusCharge  = new TH1F("looseMuUpperPlusCharge", "looseMuUpperPlusCharge",  3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaPlus      = new TH1F("looseMuUpperPlusChargeEtaPlus","looseMuUpperPlusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaMinus      = new TH1F("looseMuUpperPlusChargeEtaMinus","looseMuUpperPlusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusChargeEtaPlusPhiMinus","looseMuUpperPlusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaPlusPhiZero      = new TH1F("looseMuUpperPlusChargeEtaPlusPhiZero","looseMuUpperPlusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusChargeEtaPlusPhiPlus","looseMuUpperPlusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusChargeEtaMinusPhiMinus","looseMuUpperPlusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaMinusPhiZero      = new TH1F("looseMuUpperPlusChargeEtaMinusPhiZero", "looseMuUpperPlusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuUpperPlusChargeEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusChargeEtaMinusPhiPlus", "looseMuUpperPlusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_looseMuLowerMinusCharge = new TH1F("looseMuLowerMinusCharge","looseMuLowerMinusCharge", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaPlus      = new TH1F("looseMuLowerMinusChargeEtaPlus","looseMuLowerMinusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaMinus      = new TH1F("looseMuLowerMinusChargeEtaMinus","looseMuLowerMinusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusChargeEtaPlusPhiMinus","looseMuLowerMinusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaPlusPhiZero      = new TH1F("looseMuLowerMinusChargeEtaPlusPhiZero","looseMuLowerMinusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusChargeEtaPlusPhiPlus","looseMuLowerMinusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusChargeEtaMinusPhiMinus","looseMuLowerMinusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaMinusPhiZero      = new TH1F("looseMuLowerMinusChargeEtaMinusPhiZero", "looseMuLowerMinusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerMinusChargeEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusChargeEtaMinusPhiPlus", "looseMuLowerMinusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);

  TH1F *h_looseMuLowerPlusCharge  = new TH1F("looseMuLowerPlusCharge", "looseMuLowerPlusCharge",  3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaPlus      = new TH1F("looseMuLowerPlusChargeEtaPlus","looseMuLowerPlusChargeEtaPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaMinus      = new TH1F("looseMuLowerPlusChargeEtaMinus","looseMuLowerPlusChargeEtaMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusChargeEtaPlusPhiMinus","looseMuLowerPlusChargeEtaPlusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaPlusPhiZero      = new TH1F("looseMuLowerPlusChargeEtaPlusPhiZero","looseMuLowerPlusChargeEtaPlusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusChargeEtaPlusPhiPlus","looseMuLowerPlusChargeEtaPlusPhiPlus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusChargeEtaMinusPhiMinus","looseMuLowerPlusChargeEtaMinusPhiMinus", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaMinusPhiZero      = new TH1F("looseMuLowerPlusChargeEtaMinusPhiZero", "looseMuLowerPlusChargeEtaMinusPhiZero", 3, -1.5, 1.5);
  TH1F *h_looseMuLowerPlusChargeEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusChargeEtaMinusPhiPlus", "looseMuLowerPlusChargeEtaMinusPhiPlus", 3, -1.5, 1.5);


  TH1F *h_looseMuMinusCurve      = new TH1F("looseMuMinusCurve",     "looseMuMinusCurve",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaPlus      = new TH1F("looseMuMinusCurveEtaPlus",     "looseMuMinusCurveEtaPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaMinus      = new TH1F("looseMuMinusCurveEtaMinus",     "looseMuMinusCurveEtaMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaPlusPhiMinus      = new TH1F("looseMuMinusCurveEtaPlusPhiMinus",     "looseMuMinusCurveEtaPlusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaPlusPhiZero      = new TH1F("looseMuMinusCurveEtaPlusPhiZero",     "looseMuMinusCurveEtaPlusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaPlusPhiPlus      = new TH1F("looseMuMinusCurveEtaPlusPhiPlus",     "looseMuMinusCurveEtaPlusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaMinusPhiMinus      = new TH1F("looseMuMinusCurveEtaMinusPhiMinus",     "looseMuMinusCurveEtaMinusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaMinusPhiZero      = new TH1F("looseMuMinusCurveEtaMinusPhiZero",     "looseMuMinusCurveEtaMinusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaMinusPhiPlus      = new TH1F("looseMuMinusCurveEtaMinusPhiPlus",     "looseMuMinusCurveEtaMinusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaAllPhiMinus      = new TH1F("looseMuMinusCurveEtaAllPhiMinus",     "looseMuMinusCurveEtaAllPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaAllPhiZero      = new TH1F("looseMuMinusCurveEtaAllPhiZero",     "looseMuMinusCurveEtaAllPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuMinusCurveEtaAllPhiPlus      = new TH1F("looseMuMinusCurveEtaAllPhiPlus",     "looseMuMinusCurveEtaAllPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_looseMuPlusCurve       = new TH1F("looseMuPlusCurve",      "looseMuPlusCurve",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaPlus       = new TH1F("looseMuPlusCurveEtaPlus",      "looseMuPlusCurveEtaPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaMinus       = new TH1F("looseMuPlusCurveEtaMinus",      "looseMuPlusCurveEtaMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaPlusPhiMinus       = new TH1F("looseMuPlusCurveEtaPlusPhiMinus",      "looseMuPlusCurveEtaPlusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaPlusPhiZero       = new TH1F("looseMuPlusCurveEtaPlusPhiZero",      "looseMuPlusCurveEtaPlusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaPlusPhiPlus       = new TH1F("looseMuPlusCurveEtaPlusPhiPlus",      "looseMuPlusCurveEtaPlusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaMinusPhiMinus       = new TH1F("looseMuPlusCurveEtaMinusPhiMinus",      "looseMuPlusCurveEtaMinusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaMinusPhiZero       = new TH1F("looseMuPlusCurveEtaMinusPhiZero",      "looseMuPlusCurveEtaMinusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaMinusPhiPlus       = new TH1F("looseMuPlusCurveEtaMinusPhiPlus",      "looseMuPlusCurveEtaMinusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaAllPhiMinus       = new TH1F("looseMuPlusCurveEtaAllPhiMinus",      "looseMuPlusCurveEtaAllPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaAllPhiZero       = new TH1F("looseMuPlusCurveEtaAllPhiZero",      "looseMuPlusCurveEtaAllPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuPlusCurveEtaAllPhiPlus       = new TH1F("looseMuPlusCurveEtaAllPhiPlus",      "looseMuPlusCurveEtaAllPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);

  TH1F *h_looseMuUpperMinusCurve = new TH1F("looseMuUpperMinusCurve","looseMuUpperMinusCurve",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaPlus = new TH1F("looseMuUpperMinusCurveEtaPlus","looseMuUpperMinusCurveEtaPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaMinus = new TH1F("looseMuUpperMinusCurveEtaMinus","looseMuUpperMinusCurveEtaMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiMinus = new TH1F("looseMuUpperMinusCurveEtaPlusPhiMinus","looseMuUpperMinusCurveEtaPlusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiZero = new TH1F("looseMuUpperMinusCurveEtaPlusPhiZero","looseMuUpperMinusCurveEtaPlusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiPlus = new TH1F("looseMuUpperMinusCurveEtaPlusPhiPlus","looseMuUpperMinusCurveEtaPlusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiMinus = new TH1F("looseMuUpperMinusCurveEtaMinusPhiMinus","looseMuUpperMinusCurveEtaMinusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiZero = new TH1F("looseMuUpperMinusCurveEtaMinusPhiZero","looseMuUpperMinusCurveEtaMinusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiPlus = new TH1F("looseMuUpperMinusCurveEtaMinusPhiPlus","looseMuUpperMinusCurveEtaMinusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiMinus = new TH1F("looseMuUpperMinusCurveEtaAllPhiMinus","looseMuUpperMinusCurveEtaAllPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiZero = new TH1F("looseMuUpperMinusCurveEtaAllPhiZero","looseMuUpperMinusCurveEtaAllPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiPlus = new TH1F("looseMuUpperMinusCurveEtaAllPhiPlus","looseMuUpperMinusCurveEtaAllPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



  TH1F *h_looseMuUpperPlusCurve  = new TH1F("looseMuUpperPlusCurve", "looseMuUpperPlusCurve",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaPlus  = new TH1F("looseMuUpperPlusCurveEtaPlus", "looseMuUpperPlusCurveEtaPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaMinus  = new TH1F("looseMuUpperPlusCurveEtaMinus", "looseMuUpperPlusCurveEtaMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiMinus  = new TH1F("looseMuUpperPlusCurveEtaPlusPhiMinus", "looseMuUpperPlusCurveEtaPlusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiZero  = new TH1F("looseMuUpperPlusCurveEtaPlusPhiZero", "looseMuUpperPlusCurveEtaPlusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiPlus  = new TH1F("looseMuUpperPlusCurveEtaPlusPhiPlus", "looseMuUpperPlusCurveEtaPlusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiMinus  = new TH1F("looseMuUpperPlusCurveEtaMinusPhiMinus", "looseMuUpperPlusCurveEtaMinusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiZero  = new TH1F("looseMuUpperPlusCurveEtaMinusPhiZero", "looseMuUpperPlusCurveEtaMinusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiPlus  = new TH1F("looseMuUpperPlusCurveEtaMinusPhiPlus", "looseMuUpperPlusCurveEtaMinusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiMinus  = new TH1F("looseMuUpperPlusCurveEtaAllPhiMinus", "looseMuUpperPlusCurveEtaAllPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiZero  = new TH1F("looseMuUpperPlusCurveEtaAllPhiZero", "looseMuUpperPlusCurveEtaAllPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiPlus  = new TH1F("looseMuUpperPlusCurveEtaAllPhiPlus", "looseMuUpperPlusCurveEtaAllPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



  TH1F *h_looseMuLowerMinusCurve = new TH1F("looseMuLowerMinusCurve","looseMuLowerMinusCurve",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaPlus = new TH1F("looseMuLowerMinusCurveEtaPlus","looseMuLowerMinusCurveEtaPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaMinus = new TH1F("looseMuLowerMinusCurveEtaMinus","looseMuLowerMinusCurveEtaMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiMinus = new TH1F("looseMuLowerMinusCurveEtaPlusPhiMinus","looseMuLowerMinusCurveEtaPlusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiZero = new TH1F("looseMuLowerMinusCurveEtaPlusPhiZero","looseMuLowerMinusCurveEtaPlusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiPlus = new TH1F("looseMuLowerMinusCurveEtaPlusPhiPlus","looseMuLowerMinusCurveEtaPlusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiMinus = new TH1F("looseMuLowerMinusCurveEtaMinusPhiMinus","looseMuLowerMinusCurveEtaMinusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiZero = new TH1F("looseMuLowerMinusCurveEtaMinusPhiZero","looseMuLowerMinusCurveEtaMinusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiPlus = new TH1F("looseMuLowerMinusCurveEtaMinusPhiPlus","looseMuLowerMinusCurveEtaMinusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiMinus = new TH1F("looseMuLowerMinusCurveEtaAllPhiMinus","looseMuLowerMinusCurveEtaAllPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiZero = new TH1F("looseMuLowerMinusCurveEtaAllPhiZero","looseMuLowerMinusCurveEtaAllPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiPlus = new TH1F("looseMuLowerMinusCurveEtaAllPhiPlus","looseMuLowerMinusCurveEtaAllPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


  TH1F *h_looseMuLowerPlusCurve  = new TH1F("looseMuLowerPlusCurve", "looseMuLowerPlusCurve",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaPlus  = new TH1F("looseMuLowerPlusCurveEtaPlus", "looseMuLowerPlusCurveEtaPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaMinus  = new TH1F("looseMuLowerPlusCurveEtaMinus", "looseMuLowerPlusCurveEtaMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiMinus  = new TH1F("looseMuLowerPlusCurveEtaPlusPhiMinus", "looseMuLowerPlusCurveEtaPlusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiZero  = new TH1F("looseMuLowerPlusCurveEtaPlusPhiZero", "looseMuLowerPlusCurveEtaPlusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiPlus  = new TH1F("looseMuLowerPlusCurveEtaPlusPhiPlus", "looseMuLowerPlusCurveEtaPlusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiMinus  = new TH1F("looseMuLowerPlusCurveEtaMinusPhiMinus", "looseMuLowerPlusCurveEtaMinusPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiZero  = new TH1F("looseMuLowerPlusCurveEtaMinusPhiZero", "looseMuLowerPlusCurveEtaMinusPhiZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiPlus  = new TH1F("looseMuLowerPlusCurveEtaMinusPhiPlus", "looseMuLowerPlusCurveEtaMinusPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiMinus  = new TH1F("looseMuLowerPlusCurveEtaAllPhiMinus", "looseMuLowerPlusCurveEtaAllPhiMinus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiZero  = new TH1F("looseMuLowerPlusCurveEtaAllPhiZero", "looseMuLowerPlusCurveEtaAllZero",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiPlus  = new TH1F("looseMuLowerPlusCurveEtaAllPhiPlus", "looseMuLowerPlusCurveEtaAllPhiPlus",
					    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);




  TH1F *h_looseMuMinusDxy      = new TH1F("looseMuMinusDxy",     "looseMuMinusDxy",      100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaPlus      = new TH1F("looseMuMinusDxyEtaPlus","looseMuMinusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaMinus      = new TH1F("looseMuMinusDxyEtaMinus","looseMuMinusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaPlusPhiMinus      = new TH1F("looseMuMinusDxyEtaPlusPhiMinus","looseMuMinusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaPlusPhiZero      = new TH1F("looseMuMinusDxyEtaPlusPhiZero","looseMuMinusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaPlusPhiPlus      = new TH1F("looseMuMinusDxyEtaPlusPhiPlus","looseMuMinusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaMinusPhiMinus      = new TH1F("looseMuMinusDxyEtaMinusPhiMinus","looseMuMinusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaMinusPhiZero      = new TH1F("looseMuMinusDxyEtaMinusPhiZero", "looseMuMinusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuMinusDxyEtaMinusPhiPlus      = new TH1F("looseMuMinusDxyEtaMinusPhiPlus", "looseMuMinusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_looseMuPlusDxy       = new TH1F("looseMuPlusDxy",      "looseMuPlusDxy",       100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaPlus      = new TH1F("looseMuPlusDxyEtaPlus","looseMuPlusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaMinus      = new TH1F("looseMuPlusDxyEtaMinus","looseMuPlusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaPlusPhiMinus      = new TH1F("looseMuPlusDxyEtaPlusPhiMinus","looseMuPlusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaPlusPhiZero      = new TH1F("looseMuPlusDxyEtaPlusPhiZero","looseMuPlusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaPlusPhiPlus      = new TH1F("looseMuPlusDxyEtaPlusPhiPlus","looseMuPlusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaMinusPhiMinus      = new TH1F("looseMuPlusDxyEtaMinusPhiMinus","looseMuPlusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaMinusPhiZero      = new TH1F("looseMuPlusDxyEtaMinusPhiZero", "looseMuPlusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuPlusDxyEtaMinusPhiPlus      = new TH1F("looseMuPlusDxyEtaMinusPhiPlus", "looseMuPlusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_looseMuUpperMinusDxy = new TH1F("looseMuUpperMinusDxy","looseMuUpperMinusDxy", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaPlus      = new TH1F("looseMuUpperMinusDxyEtaPlus","looseMuUpperMinusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaMinus      = new TH1F("looseMuUpperMinusDxyEtaMinus","looseMuUpperMinusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusDxyEtaPlusPhiMinus","looseMuUpperMinusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaPlusPhiZero      = new TH1F("looseMuUpperMinusDxyEtaPlusPhiZero","looseMuUpperMinusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusDxyEtaPlusPhiPlus","looseMuUpperMinusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusDxyEtaMinusPhiMinus","looseMuUpperMinusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaMinusPhiZero      = new TH1F("looseMuUpperMinusDxyEtaMinusPhiZero", "looseMuUpperMinusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuUpperMinusDxyEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusDxyEtaMinusPhiPlus", "looseMuUpperMinusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_looseMuUpperPlusDxy  = new TH1F("looseMuUpperPlusDxy", "looseMuUpperPlusDxy",  100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaPlus      = new TH1F("looseMuUpperPlusDxyEtaPlus","looseMuUpperPlusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaMinus      = new TH1F("looseMuUpperPlusDxyEtaMinus","looseMuUpperPlusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusDxyEtaPlusPhiMinus","looseMuUpperPlusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaPlusPhiZero      = new TH1F("looseMuUpperPlusDxyEtaPlusPhiZero","looseMuUpperPlusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusDxyEtaPlusPhiPlus","looseMuUpperPlusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusDxyEtaMinusPhiMinus","looseMuUpperPlusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaMinusPhiZero      = new TH1F("looseMuUpperPlusDxyEtaMinusPhiZero", "looseMuUpperPlusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuUpperPlusDxyEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusDxyEtaMinusPhiPlus", "looseMuUpperPlusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_looseMuLowerMinusDxy = new TH1F("looseMuLowerMinusDxy","looseMuLowerMinusDxy", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaPlus      = new TH1F("looseMuLowerMinusDxyEtaPlus","looseMuLowerMinusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaMinus      = new TH1F("looseMuLowerMinusDxyEtaMinus","looseMuLowerMinusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusDxyEtaPlusPhiMinus","looseMuLowerMinusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaPlusPhiZero      = new TH1F("looseMuLowerMinusDxyEtaPlusPhiZero","looseMuLowerMinusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusDxyEtaPlusPhiPlus","looseMuLowerMinusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusDxyEtaMinusPhiMinus","looseMuLowerMinusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaMinusPhiZero      = new TH1F("looseMuLowerMinusDxyEtaMinusPhiZero", "looseMuLowerMinusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuLowerMinusDxyEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusDxyEtaMinusPhiPlus", "looseMuLowerMinusDxyEtaMinusPhiPlus", 100, -100., 100.);

  TH1F *h_looseMuLowerPlusDxy  = new TH1F("looseMuLowerPlusDxy", "looseMuLowerPlusDxy",  100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaPlus      = new TH1F("looseMuLowerPlusDxyEtaPlus","looseMuLowerPlusDxyEtaPlus", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaMinus      = new TH1F("looseMuLowerPlusDxyEtaMinus","looseMuLowerPlusDxyEtaMinus", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusDxyEtaPlusPhiMinus","looseMuLowerPlusDxyEtaPlusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaPlusPhiZero      = new TH1F("looseMuLowerPlusDxyEtaPlusPhiZero","looseMuLowerPlusDxyEtaPlusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusDxyEtaPlusPhiPlus","looseMuLowerPlusDxyEtaPlusPhiPlus", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusDxyEtaMinusPhiMinus","looseMuLowerPlusDxyEtaMinusPhiMinus", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaMinusPhiZero      = new TH1F("looseMuLowerPlusDxyEtaMinusPhiZero", "looseMuLowerPlusDxyEtaMinusPhiZero", 100, -100., 100.);
  TH1F *h_looseMuLowerPlusDxyEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusDxyEtaMinusPhiPlus", "looseMuLowerPlusDxyEtaMinusPhiPlus", 100, -100., 100.);


  TH1F *h_looseMuMinusDz       = new TH1F("looseMuMinusDz",     "looseMuMinusDz",        100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaPlus      = new TH1F("looseMuMinusDzEtaPlus","looseMuMinusDzEtaPlus", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaMinus      = new TH1F("looseMuMinusDzEtaMinus","looseMuMinusDzEtaMinus", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaPlusPhiMinus      = new TH1F("looseMuMinusDzEtaPlusPhiMinus","looseMuMinusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaPlusPhiZero      = new TH1F("looseMuMinusDzEtaPlusPhiZero","looseMuMinusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaPlusPhiPlus      = new TH1F("looseMuMinusDzEtaPlusPhiPlus","looseMuMinusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaMinusPhiMinus      = new TH1F("looseMuMinusDzEtaMinusPhiMinus","looseMuMinusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaMinusPhiZero      = new TH1F("looseMuMinusDzEtaMinusPhiZero", "looseMuMinusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuMinusDzEtaMinusPhiPlus      = new TH1F("looseMuMinusDzEtaMinusPhiPlus", "looseMuMinusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_looseMuPlusDz        = new TH1F("looseMuPlusDz",      "looseMuPlusDz",         100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaPlus      = new TH1F("looseMuPlusDzEtaPlus","looseMuPlusDzEtaPlus", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaMinus      = new TH1F("looseMuPlusDzEtaMinus","looseMuPlusDzEtaMinus", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaPlusPhiMinus      = new TH1F("looseMuPlusDzEtaPlusPhiMinus","looseMuPlusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaPlusPhiZero      = new TH1F("looseMuPlusDzEtaPlusPhiZero","looseMuPlusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaPlusPhiPlus      = new TH1F("looseMuPlusDzEtaPlusPhiPlus","looseMuPlusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaMinusPhiMinus      = new TH1F("looseMuPlusDzEtaMinusPhiMinus","looseMuPlusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaMinusPhiZero      = new TH1F("looseMuPlusDzEtaMinusPhiZero", "looseMuPlusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuPlusDzEtaMinusPhiPlus      = new TH1F("looseMuPlusDzEtaMinusPhiPlus", "looseMuPlusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_looseMuUpperMinusDz  = new TH1F("looseMuUpperMinusDz","looseMuUpperMinusDz",   100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaPlus      = new TH1F("looseMuUpperMinusDzEtaPlus","looseMuUpperMinusDzEtaPlus", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaMinus      = new TH1F("looseMuUpperMinusDzEtaMinus","looseMuUpperMinusDzEtaMinus", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusDzEtaPlusPhiMinus","looseMuUpperMinusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaPlusPhiZero      = new TH1F("looseMuUpperMinusDzEtaPlusPhiZero","looseMuUpperMinusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusDzEtaPlusPhiPlus","looseMuUpperMinusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusDzEtaMinusPhiMinus","looseMuUpperMinusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaMinusPhiZero      = new TH1F("looseMuUpperMinusDzEtaMinusPhiZero", "looseMuUpperMinusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuUpperMinusDzEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusDzEtaMinusPhiPlus", "looseMuUpperMinusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_looseMuUpperPlusDz   = new TH1F("looseMuUpperPlusDz", "looseMuUpperPlusDz",    100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaPlus      = new TH1F("looseMuUpperPlusDzEtaPlus","looseMuUpperPlusDzEtaPlus", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaMinus      = new TH1F("looseMuUpperPlusDzEtaMinus","looseMuUpperPlusDzEtaMinus", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusDzEtaPlusPhiMinus","looseMuUpperPlusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaPlusPhiZero      = new TH1F("looseMuUpperPlusDzEtaPlusPhiZero","looseMuUpperPlusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusDzEtaPlusPhiPlus","looseMuUpperPlusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusDzEtaMinusPhiMinus","looseMuUpperPlusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaMinusPhiZero      = new TH1F("looseMuUpperPlusDzEtaMinusPhiZero", "looseMuUpperPlusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuUpperPlusDzEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusDzEtaMinusPhiPlus", "looseMuUpperPlusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_looseMuLowerMinusDz  = new TH1F("looseMuLowerMinusDz","looseMuLowerMinusDz",   100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaPlus      = new TH1F("looseMuLowerMinusDzEtaPlus","looseMuLowerMinusDzEtaPlus", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaMinus      = new TH1F("looseMuLowerMinusDzEtaMinus","looseMuLowerMinusDzEtaMinus", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusDzEtaPlusPhiMinus","looseMuLowerMinusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaPlusPhiZero      = new TH1F("looseMuLowerMinusDzEtaPlusPhiZero","looseMuLowerMinusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusDzEtaPlusPhiPlus","looseMuLowerMinusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusDzEtaMinusPhiMinus","looseMuLowerMinusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaMinusPhiZero      = new TH1F("looseMuLowerMinusDzEtaMinusPhiZero", "looseMuLowerMinusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuLowerMinusDzEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusDzEtaMinusPhiPlus", "looseMuLowerMinusDzEtaMinusPhiPlus", 100, -250., 250.);

  TH1F *h_looseMuLowerPlusDz   = new TH1F("looseMuLowerPlusDz", "looseMuLowerPlusDz",    100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaPlus      = new TH1F("looseMuLowerPlusDzEtaPlus","looseMuLowerPlusDzEtaPlus", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaMinus      = new TH1F("looseMuLowerPlusDzEtaMinus","looseMuLowerPlusDzEtaMinus", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusDzEtaPlusPhiMinus","looseMuLowerPlusDzEtaPlusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaPlusPhiZero      = new TH1F("looseMuLowerPlusDzEtaPlusPhiZero","looseMuLowerPlusDzEtaPlusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusDzEtaPlusPhiPlus","looseMuLowerPlusDzEtaPlusPhiPlus", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusDzEtaMinusPhiMinus","looseMuLowerPlusDzEtaMinusPhiMinus", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaMinusPhiZero      = new TH1F("looseMuLowerPlusDzEtaMinusPhiZero", "looseMuLowerPlusDzEtaMinusPhiZero", 100, -250., 250.);
  TH1F *h_looseMuLowerPlusDzEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusDzEtaMinusPhiPlus", "looseMuLowerPlusDzEtaMinusPhiPlus", 100, -250., 250.);


  TH1F *h_looseMuMinusDxyError      = new TH1F("looseMuMinusDxyError",     "looseMuMinusDxyError",      50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaPlus      = new TH1F("looseMuMinusDxyErrorEtaPlus","looseMuMinusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaMinus      = new TH1F("looseMuMinusDxyErrorEtaMinus","looseMuMinusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaPlusPhiMinus      = new TH1F("looseMuMinusDxyErrorEtaPlusPhiMinus","looseMuMinusDxyErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaPlusPhiZero      = new TH1F("looseMuMinusDxyErrorEtaPlusPhiZero","looseMuMinusDxyErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaPlusPhiPlus      = new TH1F("looseMuMinusDxyErrorEtaPlusPhiPlus","looseMuMinusDxyErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaMinusPhiMinus      = new TH1F("looseMuMinusDxyErrorEtaMinusPhiMinus","looseMuMinusDxyErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaMinusPhiZero      = new TH1F("looseMuMinusDxyErrorEtaMinusPhiZero", "looseMuMinusDxyErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuMinusDxyErrorEtaMinusPhiPlus      = new TH1F("looseMuMinusDxyErrorEtaMinusPhiPlus", "looseMuMinusDxyErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuPlusDxyError       = new TH1F("looseMuPlusDxyError",      "looseMuPlusDxyError",       50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaPlus      = new TH1F("looseMuPlusDxyErrorEtaPlus","looseMuPlusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaMinus      = new TH1F("looseMuPlusDxyErrorEtaMinus","looseMuPlusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaPlusPhiMinus      = new TH1F("looseMuPlusDxyErrorEtaPlusPhiMinus","looseMuPlusDxyErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaPlusPhiZero      = new TH1F("looseMuPlusDxyErrorEtaPlusPhiZero","looseMuPlusDxyErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaPlusPhiPlus      = new TH1F("looseMuPlusDxyErrorEtaPlusPhiPlus","looseMuPlusDxyErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaMinusPhiMinus      = new TH1F("looseMuPlusDxyErrorEtaMinusPhiMinus","looseMuPlusDxyErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaMinusPhiZero      = new TH1F("looseMuPlusDxyErrorEtaMinusPhiZero", "looseMuPlusDxyErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuPlusDxyErrorEtaMinusPhiPlus      = new TH1F("looseMuPlusDxyErrorEtaMinusPhiPlus", "looseMuPlusDxyErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuUpperMinusDxyError = new TH1F("looseMuUpperMinusDxyError","looseMuUpperMinusDxyError", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaPlus      = new TH1F("looseMuUpperMinusDxyErrorEtaPlus","looseMuUpperMinusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaMinus      = new TH1F("looseMuUpperMinusDxyErrorEtaMinus","looseMuUpperMinusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusDxyErrorEtaPlusPhiMinus","looseMuUpperMinusDxyErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaPlusPhiZero      = new TH1F("looseMuUpperMinusDxyErrorEtaPlusPhiZero","looseMuUpperMinusDxyErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusDxyErrorEtaPlusPhiPlus","looseMuUpperMinusDxyErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusDxyErrorEtaMinusPhiMinus","looseMuUpperMinusDxyErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaMinusPhiZero      = new TH1F("looseMuUpperMinusDxyErrorEtaMinusPhiZero", "looseMuUpperMinusDxyErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuUpperMinusDxyErrorEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusDxyErrorEtaMinusPhiPlus", "looseMuUpperMinusDxyErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuUpperPlusDxyError  = new TH1F("looseMuUpperPlusDxyError", "looseMuUpperPlusDxyError",  50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaPlus      = new TH1F("looseMuUpperPlusDxyErrorEtaPlus","looseMuUpperPlusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaMinus      = new TH1F("looseMuUpperPlusDxyErrorEtaMinus","looseMuUpperPlusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusDxyErrorEtaPlusPhiMinus","looseMuUpperPlusDxyErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaPlusPhiZero      = new TH1F("looseMuUpperPlusDxyErrorEtaPlusPhiZero","looseMuUpperPlusDxyErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusDxyErrorEtaPlusPhiPlus","looseMuUpperPlusDxyErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusDxyErrorEtaMinusPhiMinus","looseMuUpperPlusDxyErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaMinusPhiZero      = new TH1F("looseMuUpperPlusDxyErrorEtaMinusPhiZero", "looseMuUpperPlusDxyErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuUpperPlusDxyErrorEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusDxyErrorEtaMinusPhiPlus", "looseMuUpperPlusDxyErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuLowerMinusDxyError = new TH1F("looseMuLowerMinusDxyError","looseMuLowerMinusDxyError", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaPlus      = new TH1F("looseMuLowerMinusDxyErrorEtaPlus","looseMuLowerMinusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaMinus      = new TH1F("looseMuLowerMinusDxyErrorEtaMinus","looseMuLowerMinusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusDxyErrorEtaPlusPhiMinus","looseMuLowerMinusDxyErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaPlusPhiZero      = new TH1F("looseMuLowerMinusDxyErrorEtaPlusPhiZero","looseMuLowerMinusDxyErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusDxyErrorEtaPlusPhiPlus","looseMuLowerMinusDxyErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusDxyErrorEtaMinusPhiMinus","looseMuLowerMinusDxyErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaMinusPhiZero      = new TH1F("looseMuLowerMinusDxyErrorEtaMinusPhiZero", "looseMuLowerMinusDxyErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuLowerMinusDxyErrorEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusDxyErrorEtaMinusPhiPlus", "looseMuLowerMinusDxyErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuLowerPlusDxyError  = new TH1F("looseMuLowerPlusDxyError", "looseMuLowerPlusDxyError",  50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaPlus      = new TH1F("looseMuLowerPlusDxyErrorEtaPlus","looseMuLowerPlusDxyErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaMinus      = new TH1F("looseMuLowerPlusDxyErrorEtaMinus","looseMuLowerPlusDxyErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusDxyErrorEtaPlusPhiMinus","looseMuLowerPlusDxyErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaPlusPhiZero      = new TH1F("looseMuLowerPlusDxyErrorEtaPlusPhiZero","looseMuLowerPlusDxyErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusDxyErrorEtaPlusPhiPlus","looseMuLowerPlusDxyErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusDxyErrorEtaMinusPhiMinus","looseMuLowerPlusDxyErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaMinusPhiZero      = new TH1F("looseMuLowerPlusDxyErrorEtaMinusPhiZero", "looseMuLowerPlusDxyErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuLowerPlusDxyErrorEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusDxyErrorEtaMinusPhiPlus", "looseMuLowerPlusDxyErrorEtaMinusPhiPlus",
								     50, 0., 150.);


  TH1F *h_looseMuMinusDzError      = new TH1F("looseMuMinusDzError",     "looseMuMinusDzError",      50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaPlus      = new TH1F("looseMuMinusDzErrorEtaPlus","looseMuMinusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaMinus      = new TH1F("looseMuMinusDzErrorEtaMinus","looseMuMinusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaPlusPhiMinus      = new TH1F("looseMuMinusDzErrorEtaPlusPhiMinus","looseMuMinusDzErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaPlusPhiZero      = new TH1F("looseMuMinusDzErrorEtaPlusPhiZero","looseMuMinusDzErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaPlusPhiPlus      = new TH1F("looseMuMinusDzErrorEtaPlusPhiPlus","looseMuMinusDzErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaMinusPhiMinus      = new TH1F("looseMuMinusDzErrorEtaMinusPhiMinus","looseMuMinusDzErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaMinusPhiZero      = new TH1F("looseMuMinusDzErrorEtaMinusPhiZero", "looseMuMinusDzErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuMinusDzErrorEtaMinusPhiPlus      = new TH1F("looseMuMinusDzErrorEtaMinusPhiPlus", "looseMuMinusDzErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuPlusDzError       = new TH1F("looseMuPlusDzError",      "looseMuPlusDzError",       50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaPlus      = new TH1F("looseMuPlusDzErrorEtaPlus","looseMuPlusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaMinus      = new TH1F("looseMuPlusDzErrorEtaMinus","looseMuPlusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaPlusPhiMinus      = new TH1F("looseMuPlusDzErrorEtaPlusPhiMinus","looseMuPlusDzErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaPlusPhiZero      = new TH1F("looseMuPlusDzErrorEtaPlusPhiZero","looseMuPlusDzErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaPlusPhiPlus      = new TH1F("looseMuPlusDzErrorEtaPlusPhiPlus","looseMuPlusDzErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaMinusPhiMinus      = new TH1F("looseMuPlusDzErrorEtaMinusPhiMinus","looseMuPlusDzErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaMinusPhiZero      = new TH1F("looseMuPlusDzErrorEtaMinusPhiZero", "looseMuPlusDzErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuPlusDzErrorEtaMinusPhiPlus      = new TH1F("looseMuPlusDzErrorEtaMinusPhiPlus", "looseMuPlusDzErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuUpperMinusDzError = new TH1F("looseMuUpperMinusDzError","looseMuUpperMinusDzError", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaPlus      = new TH1F("looseMuUpperMinusDzErrorEtaPlus","looseMuUpperMinusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaMinus      = new TH1F("looseMuUpperMinusDzErrorEtaMinus","looseMuUpperMinusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusDzErrorEtaPlusPhiMinus","looseMuUpperMinusDzErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaPlusPhiZero      = new TH1F("looseMuUpperMinusDzErrorEtaPlusPhiZero","looseMuUpperMinusDzErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusDzErrorEtaPlusPhiPlus","looseMuUpperMinusDzErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusDzErrorEtaMinusPhiMinus","looseMuUpperMinusDzErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaMinusPhiZero      = new TH1F("looseMuUpperMinusDzErrorEtaMinusPhiZero", "looseMuUpperMinusDzErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuUpperMinusDzErrorEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusDzErrorEtaMinusPhiPlus", "looseMuUpperMinusDzErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuUpperPlusDzError  = new TH1F("looseMuUpperPlusDzError", "looseMuUpperPlusDzError",  50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaPlus      = new TH1F("looseMuUpperPlusDzErrorEtaPlus","looseMuUpperPlusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaMinus      = new TH1F("looseMuUpperPlusDzErrorEtaMinus","looseMuUpperPlusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusDzErrorEtaPlusPhiMinus","looseMuUpperPlusDzErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaPlusPhiZero      = new TH1F("looseMuUpperPlusDzErrorEtaPlusPhiZero","looseMuUpperPlusDzErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusDzErrorEtaPlusPhiPlus","looseMuUpperPlusDzErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusDzErrorEtaMinusPhiMinus","looseMuUpperPlusDzErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaMinusPhiZero      = new TH1F("looseMuUpperPlusDzErrorEtaMinusPhiZero", "looseMuUpperPlusDzErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuUpperPlusDzErrorEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusDzErrorEtaMinusPhiPlus", "looseMuUpperPlusDzErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuLowerMinusDzError = new TH1F("looseMuLowerMinusDzError","looseMuLowerMinusDzError", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaPlus      = new TH1F("looseMuLowerMinusDzErrorEtaPlus","looseMuLowerMinusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaMinus      = new TH1F("looseMuLowerMinusDzErrorEtaMinus","looseMuLowerMinusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusDzErrorEtaPlusPhiMinus","looseMuLowerMinusDzErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaPlusPhiZero      = new TH1F("looseMuLowerMinusDzErrorEtaPlusPhiZero","looseMuLowerMinusDzErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusDzErrorEtaPlusPhiPlus","looseMuLowerMinusDzErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusDzErrorEtaMinusPhiMinus","looseMuLowerMinusDzErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaMinusPhiZero      = new TH1F("looseMuLowerMinusDzErrorEtaMinusPhiZero", "looseMuLowerMinusDzErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuLowerMinusDzErrorEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusDzErrorEtaMinusPhiPlus", "looseMuLowerMinusDzErrorEtaMinusPhiPlus",
								     50, 0., 150.);

  TH1F *h_looseMuLowerPlusDzError  = new TH1F("looseMuLowerPlusDzError", "looseMuLowerPlusDzError",  50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaPlus      = new TH1F("looseMuLowerPlusDzErrorEtaPlus","looseMuLowerPlusDzErrorEtaPlus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaMinus      = new TH1F("looseMuLowerPlusDzErrorEtaMinus","looseMuLowerPlusDzErrorEtaMinus", 50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusDzErrorEtaPlusPhiMinus","looseMuLowerPlusDzErrorEtaPlusPhiMinus",
								     50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaPlusPhiZero      = new TH1F("looseMuLowerPlusDzErrorEtaPlusPhiZero","looseMuLowerPlusDzErrorEtaPlusPhiZero",
								    50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusDzErrorEtaPlusPhiPlus","looseMuLowerPlusDzErrorEtaPlusPhiPlus",
								    50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusDzErrorEtaMinusPhiMinus","looseMuLowerPlusDzErrorEtaMinusPhiMinus",
								      50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaMinusPhiZero      = new TH1F("looseMuLowerPlusDzErrorEtaMinusPhiZero", "looseMuLowerPlusDzErrorEtaMinusPhiZero",
								     50, 0., 150.);
  TH1F *h_looseMuLowerPlusDzErrorEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusDzErrorEtaMinusPhiPlus", "looseMuLowerPlusDzErrorEtaMinusPhiPlus",
							     50, 0., 150.);

  TH1F *h_looseMuMinusTrackPt      = new TH1F("looseMuMinusTrackPt",     "looseMuMinusTrackPt",      300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaPlus      = new TH1F("looseMuMinusTrackPtEtaPlus","looseMuMinusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaMinus      = new TH1F("looseMuMinusTrackPtEtaMinus","looseMuMinusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaPlusPhiMinus      = new TH1F("looseMuMinusTrackPtEtaPlusPhiMinus","looseMuMinusTrackPtEtaPlusPhiMinus",
								     300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaPlusPhiZero      = new TH1F("looseMuMinusTrackPtEtaPlusPhiZero","looseMuMinusTrackPtEtaPlusPhiZero",
								    300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaPlusPhiPlus      = new TH1F("looseMuMinusTrackPtEtaPlusPhiPlus","looseMuMinusTrackPtEtaPlusPhiPlus",
								    300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaMinusPhiMinus      = new TH1F("looseMuMinusTrackPtEtaMinusPhiMinus","looseMuMinusTrackPtEtaMinusPhiMinus",
								      300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaMinusPhiZero      = new TH1F("looseMuMinusTrackPtEtaMinusPhiZero", "looseMuMinusTrackPtEtaMinusPhiZero",
								     300, 0., 3000.);
  TH1F *h_looseMuMinusTrackPtEtaMinusPhiPlus      = new TH1F("looseMuMinusTrackPtEtaMinusPhiPlus", "looseMuMinusTrackPtEtaMinusPhiPlus",
								     300, 0., 3000.);

  TH1F *h_looseMuPlusTrackPt       = new TH1F("looseMuPlusTrackPt",      "looseMuPlusTrackPt",       300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaPlus      = new TH1F("looseMuPlusTrackPtEtaPlus","looseMuPlusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaMinus      = new TH1F("looseMuPlusTrackPtEtaMinus","looseMuPlusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaPlusPhiMinus      = new TH1F("looseMuPlusTrackPtEtaPlusPhiMinus","looseMuPlusTrackPtEtaPlusPhiMinus",
								     300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaPlusPhiZero      = new TH1F("looseMuPlusTrackPtEtaPlusPhiZero","looseMuPlusTrackPtEtaPlusPhiZero",
								    300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaPlusPhiPlus      = new TH1F("looseMuPlusTrackPtEtaPlusPhiPlus","looseMuPlusTrackPtEtaPlusPhiPlus",
								    300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaMinusPhiMinus      = new TH1F("looseMuPlusTrackPtEtaMinusPhiMinus","looseMuPlusTrackPtEtaMinusPhiMinus",
								      300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaMinusPhiZero      = new TH1F("looseMuPlusTrackPtEtaMinusPhiZero", "looseMuPlusTrackPtEtaMinusPhiZero",
								     300, 0., 3000.);
  TH1F *h_looseMuPlusTrackPtEtaMinusPhiPlus      = new TH1F("looseMuPlusTrackPtEtaMinusPhiPlus", "looseMuPlusTrackPtEtaMinusPhiPlus",
								     300, 0., 3000.);


  TH1F *h_looseMuUpperMinusTrackPt = new TH1F("looseMuUpperMinusTrackPt","looseMuUpperMinusTrackPt", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaPlus      = new TH1F("looseMuUpperMinusTrackPtEtaPlus","looseMuUpperMinusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaMinus      = new TH1F("looseMuUpperMinusTrackPtEtaMinus","looseMuUpperMinusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusTrackPtEtaPlusPhiMinus","looseMuUpperMinusTrackPtEtaPlusPhiMinus",
								     300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaPlusPhiZero      = new TH1F("looseMuUpperMinusTrackPtEtaPlusPhiZero","looseMuUpperMinusTrackPtEtaPlusPhiZero",
								    300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusTrackPtEtaPlusPhiPlus","looseMuUpperMinusTrackPtEtaPlusPhiPlus",
								    300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusTrackPtEtaMinusPhiMinus","looseMuUpperMinusTrackPtEtaMinusPhiMinus",
								      300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaMinusPhiZero      = new TH1F("looseMuUpperMinusTrackPtEtaMinusPhiZero", "looseMuUpperMinusTrackPtEtaMinusPhiZero",
								     300, 0., 3000.);
  TH1F *h_looseMuUpperMinusTrackPtEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusTrackPtEtaMinusPhiPlus", "looseMuUpperMinusTrackPtEtaMinusPhiPlus",
								     300, 0., 3000.);

  TH1F *h_looseMuUpperPlusTrackPt  = new TH1F("looseMuUpperPlusTrackPt", "looseMuUpperPlusTrackPt",  300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaPlus      = new TH1F("looseMuUpperPlusTrackPtEtaPlus","looseMuUpperPlusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaMinus      = new TH1F("looseMuUpperPlusTrackPtEtaMinus","looseMuUpperPlusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusTrackPtEtaPlusPhiMinus","looseMuUpperPlusTrackPtEtaPlusPhiMinus",
								     300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaPlusPhiZero      = new TH1F("looseMuUpperPlusTrackPtEtaPlusPhiZero","looseMuUpperPlusTrackPtEtaPlusPhiZero",
								    300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusTrackPtEtaPlusPhiPlus","looseMuUpperPlusTrackPtEtaPlusPhiPlus",
								    300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusTrackPtEtaMinusPhiMinus","looseMuUpperPlusTrackPtEtaMinusPhiMinus",
								      300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaMinusPhiZero      = new TH1F("looseMuUpperPlusTrackPtEtaMinusPhiZero", "looseMuUpperPlusTrackPtEtaMinusPhiZero",
								     300, 0., 3000.);
  TH1F *h_looseMuUpperPlusTrackPtEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusTrackPtEtaMinusPhiPlus", "looseMuUpperPlusTrackPtEtaMinusPhiPlus",
								     300, 0., 3000.);

  TH1F *h_looseMuLowerMinusTrackPt = new TH1F("looseMuLowerMinusTrackPt","looseMuLowerMinusTrackPt", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaPlus      = new TH1F("looseMuLowerMinusTrackPtEtaPlus","looseMuLowerMinusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaMinus      = new TH1F("looseMuLowerMinusTrackPtEtaMinus","looseMuLowerMinusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusTrackPtEtaPlusPhiMinus","looseMuLowerMinusTrackPtEtaPlusPhiMinus",
								     300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaPlusPhiZero      = new TH1F("looseMuLowerMinusTrackPtEtaPlusPhiZero","looseMuLowerMinusTrackPtEtaPlusPhiZero",
								    300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusTrackPtEtaPlusPhiPlus","looseMuLowerMinusTrackPtEtaPlusPhiPlus",
								    300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusTrackPtEtaMinusPhiMinus","looseMuLowerMinusTrackPtEtaMinusPhiMinus",
								      300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaMinusPhiZero      = new TH1F("looseMuLowerMinusTrackPtEtaMinusPhiZero", "looseMuLowerMinusTrackPtEtaMinusPhiZero",
								     300, 0., 3000.);
  TH1F *h_looseMuLowerMinusTrackPtEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusTrackPtEtaMinusPhiPlus", "looseMuLowerMinusTrackPtEtaMinusPhiPlus",
								     300, 0., 3000.);

  TH1F *h_looseMuLowerPlusTrackPt  = new TH1F("looseMuLowerPlusTrackPt", "looseMuLowerPlusTrackPt",  300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaPlus      = new TH1F("looseMuLowerPlusTrackPtEtaPlus","looseMuLowerPlusTrackPtEtaPlus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaMinus      = new TH1F("looseMuLowerPlusTrackPtEtaMinus","looseMuLowerPlusTrackPtEtaMinus", 300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusTrackPtEtaPlusPhiMinus","looseMuLowerPlusTrackPtEtaPlusPhiMinus",
								     300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaPlusPhiZero      = new TH1F("looseMuLowerPlusTrackPtEtaPlusPhiZero","looseMuLowerPlusTrackPtEtaPlusPhiZero",
								    300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusTrackPtEtaPlusPhiPlus","looseMuLowerPlusTrackPtEtaPlusPhiPlus",
								    300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusTrackPtEtaMinusPhiMinus","looseMuLowerPlusTrackPtEtaMinusPhiMinus",
								      300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaMinusPhiZero      = new TH1F("looseMuLowerPlusTrackPtEtaMinusPhiZero", "looseMuLowerPlusTrackPtEtaMinusPhiZero",
								     300, 0., 3000.);
  TH1F *h_looseMuLowerPlusTrackPtEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusTrackPtEtaMinusPhiPlus", "looseMuLowerPlusTrackPtEtaMinusPhiPlus",
								     300, 0., 3000.);


  TH1F *h_looseMuMinusTrackEta      = new TH1F("looseMuMinusTrackEta",   "looseMuMinusTrackEta",        40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaPlus      = new TH1F("looseMuMinusTrackEtaEtaPlus","looseMuMinusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaMinus      = new TH1F("looseMuMinusTrackEtaEtaMinus","looseMuMinusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaPlusPhiMinus      = new TH1F("looseMuMinusTrackEtaEtaPlusPhiMinus","looseMuMinusTrackEtaEtaPlusPhiMinus",
								     40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaPlusPhiZero      = new TH1F("looseMuMinusTrackEtaEtaPlusPhiZero","looseMuMinusTrackEtaEtaPlusPhiZero",
								    40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaPlusPhiPlus      = new TH1F("looseMuMinusTrackEtaEtaPlusPhiPlus","looseMuMinusTrackEtaEtaPlusPhiPlus",
								    40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaMinusPhiMinus      = new TH1F("looseMuMinusTrackEtaEtaMinusPhiMinus","looseMuMinusTrackEtaEtaMinusPhiMinus",
								      40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaMinusPhiZero      = new TH1F("looseMuMinusTrackEtaEtaMinusPhiZero", "looseMuMinusTrackEtaEtaMinusPhiZero",
								     40, -2., 2.);
  TH1F *h_looseMuMinusTrackEtaEtaMinusPhiPlus      = new TH1F("looseMuMinusTrackEtaEtaMinusPhiPlus", "looseMuMinusTrackEtaEtaMinusPhiPlus",
								     40, -2., 2.);


  TH1F *h_looseMuPlusTrackEta       = new TH1F("looseMuPlusTrackEta",      "looseMuPlusTrackEta",       40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaPlus      = new TH1F("looseMuPlusTrackEtaEtaPlus","looseMuPlusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaMinus      = new TH1F("looseMuPlusTrackEtaEtaMinus","looseMuPlusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaPlusPhiMinus      = new TH1F("looseMuPlusTrackEtaEtaPlusPhiMinus","looseMuPlusTrackEtaEtaPlusPhiMinus",
								     40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaPlusPhiZero      = new TH1F("looseMuPlusTrackEtaEtaPlusPhiZero","looseMuPlusTrackEtaEtaPlusPhiZero",
								    40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaPlusPhiPlus      = new TH1F("looseMuPlusTrackEtaEtaPlusPhiPlus","looseMuPlusTrackEtaEtaPlusPhiPlus",
								    40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaMinusPhiMinus      = new TH1F("looseMuPlusTrackEtaEtaMinusPhiMinus","looseMuPlusTrackEtaEtaMinusPhiMinus",
								      40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaMinusPhiZero      = new TH1F("looseMuPlusTrackEtaEtaMinusPhiZero", "looseMuPlusTrackEtaEtaMinusPhiZero",
								     40, -2., 2.);
  TH1F *h_looseMuPlusTrackEtaEtaMinusPhiPlus      = new TH1F("looseMuPlusTrackEtaEtaMinusPhiPlus", "looseMuPlusTrackEtaEtaMinusPhiPlus",
								     40, -2., 2.);


  TH1F *h_looseMuUpperMinusTrackEta = new TH1F("looseMuUpperMinusTrackEta","looseMuUpperMinusTrackEta", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaPlus      = new TH1F("looseMuUpperMinusTrackEtaEtaPlus","looseMuUpperMinusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaMinus      = new TH1F("looseMuUpperMinusTrackEtaEtaMinus","looseMuUpperMinusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusTrackEtaEtaPlusPhiMinus","looseMuUpperMinusTrackEtaEtaPlusPhiMinus",
								     40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaPlusPhiZero      = new TH1F("looseMuUpperMinusTrackEtaEtaPlusPhiZero","looseMuUpperMinusTrackEtaEtaPlusPhiZero",
								    40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusTrackEtaEtaPlusPhiPlus","looseMuUpperMinusTrackEtaEtaPlusPhiPlus",
								    40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusTrackEtaEtaMinusPhiMinus","looseMuUpperMinusTrackEtaEtaMinusPhiMinus",
								      40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaMinusPhiZero      = new TH1F("looseMuUpperMinusTrackEtaEtaMinusPhiZero", "looseMuUpperMinusTrackEtaEtaMinusPhiZero",
								     40, -2., 2.);
  TH1F *h_looseMuUpperMinusTrackEtaEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusTrackEtaEtaMinusPhiPlus", "looseMuUpperMinusTrackEtaEtaMinusPhiPlus",
								     40, -2., 2.);

  TH1F *h_looseMuUpperPlusTrackEta  = new TH1F("looseMuUpperPlusTrackEta", "looseMuUpperPlusTrackEta",  40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaPlus      = new TH1F("looseMuUpperPlusTrackEtaEtaPlus","looseMuUpperPlusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaMinus      = new TH1F("looseMuUpperPlusTrackEtaEtaMinus","looseMuUpperPlusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusTrackEtaEtaPlusPhiMinus","looseMuUpperPlusTrackEtaEtaPlusPhiMinus",
								     40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaPlusPhiZero      = new TH1F("looseMuUpperPlusTrackEtaEtaPlusPhiZero","looseMuUpperPlusTrackEtaEtaPlusPhiZero",
								    40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusTrackEtaEtaPlusPhiPlus","looseMuUpperPlusTrackEtaEtaPlusPhiPlus",
								    40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusTrackEtaEtaMinusPhiMinus","looseMuUpperPlusTrackEtaEtaMinusPhiMinus",
								      40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaMinusPhiZero      = new TH1F("looseMuUpperPlusTrackEtaEtaMinusPhiZero", "looseMuUpperPlusTrackEtaEtaMinusPhiZero",
								     40, -2., 2.);
  TH1F *h_looseMuUpperPlusTrackEtaEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusTrackEtaEtaMinusPhiPlus", "looseMuUpperPlusTrackEtaEtaMinusPhiPlus",
								     40, -2., 2.);

  TH1F *h_looseMuLowerMinusTrackEta = new TH1F("looseMuLowerMinusTrackEta","looseMuLowerMinusTrackEta", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaPlus      = new TH1F("looseMuLowerMinusTrackEtaEtaPlus","looseMuLowerMinusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaMinus      = new TH1F("looseMuLowerMinusTrackEtaEtaMinus","looseMuLowerMinusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusTrackEtaEtaPlusPhiMinus","looseMuLowerMinusTrackEtaEtaPlusPhiMinus",
								     40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaPlusPhiZero      = new TH1F("looseMuLowerMinusTrackEtaEtaPlusPhiZero","looseMuLowerMinusTrackEtaEtaPlusPhiZero",
								    40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusTrackEtaEtaPlusPhiPlus","looseMuLowerMinusTrackEtaEtaPlusPhiPlus",
								    40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusTrackEtaEtaMinusPhiMinus","looseMuLowerMinusTrackEtaEtaMinusPhiMinus",
								      40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaMinusPhiZero      = new TH1F("looseMuLowerMinusTrackEtaEtaMinusPhiZero", "looseMuLowerMinusTrackEtaEtaMinusPhiZero",
								     40, -2., 2.);
  TH1F *h_looseMuLowerMinusTrackEtaEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusTrackEtaEtaMinusPhiPlus", "looseMuLowerMinusTrackEtaEtaMinusPhiPlus",
								     40, -2., 2.);

  TH1F *h_looseMuLowerPlusTrackEta  = new TH1F("looseMuLowerPlusTrackEta", "looseMuLowerPlusTrackEta",  40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaPlus      = new TH1F("looseMuLowerPlusTrackEtaEtaPlus","looseMuLowerPlusTrackEtaEtaPlus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaMinus      = new TH1F("looseMuLowerPlusTrackEtaEtaMinus","looseMuLowerPlusTrackEtaEtaMinus", 40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusTrackEtaEtaPlusPhiMinus","looseMuLowerPlusTrackEtaEtaPlusPhiMinus",
								     40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaPlusPhiZero      = new TH1F("looseMuLowerPlusTrackEtaEtaPlusPhiZero","looseMuLowerPlusTrackEtaEtaPlusPhiZero",
								    40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusTrackEtaEtaPlusPhiPlus","looseMuLowerPlusTrackEtaEtaPlusPhiPlus",
								    40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusTrackEtaEtaMinusPhiMinus","looseMuLowerPlusTrackEtaEtaMinusPhiMinus",
								      40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaMinusPhiZero      = new TH1F("looseMuLowerPlusTrackEtaEtaMinusPhiZero", "looseMuLowerPlusTrackEtaEtaMinusPhiZero",
								     40, -2., 2.);
  TH1F *h_looseMuLowerPlusTrackEtaEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusTrackEtaEtaMinusPhiPlus", "looseMuLowerPlusTrackEtaEtaMinusPhiPlus",
								     40, -2., 2.);


  TH1F *h_looseMuMinusTrackPhi      = new TH1F("looseMuMinusTrackPhi",     "looseMuMinusTrackPhi",      40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaPlus      = new TH1F("looseMuMinusTrackPhiEtaPlus","looseMuMinusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaMinus      = new TH1F("looseMuMinusTrackPhiEtaMinus","looseMuMinusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaPlusPhiMinus      = new TH1F("looseMuMinusTrackPhiEtaPlusPhiMinus","looseMuMinusTrackPhiEtaPlusPhiMinus",
								     40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaPlusPhiZero      = new TH1F("looseMuMinusTrackPhiEtaPlusPhiZero","looseMuMinusTrackPhiEtaPlusPhiZero",
								    40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaPlusPhiPlus      = new TH1F("looseMuMinusTrackPhiEtaPlusPhiPlus","looseMuMinusTrackPhiEtaPlusPhiPlus",
								    40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaMinusPhiMinus      = new TH1F("looseMuMinusTrackPhiEtaMinusPhiMinus","looseMuMinusTrackPhiEtaMinusPhiMinus",
								      40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaMinusPhiZero      = new TH1F("looseMuMinusTrackPhiEtaMinusPhiZero", "looseMuMinusTrackPhiEtaMinusPhiZero",
								     40, -4., 4.);
  TH1F *h_looseMuMinusTrackPhiEtaMinusPhiPlus      = new TH1F("looseMuMinusTrackPhiEtaMinusPhiPlus", "looseMuMinusTrackPhiEtaMinusPhiPlus",
								     40, -4., 4.);

  TH1F *h_looseMuPlusTrackPhi       = new TH1F("looseMuPlusTrackPhi",      "looseMuPlusTrackPhi",       40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaPlus      = new TH1F("looseMuPlusTrackPhiEtaPlus","looseMuPlusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaMinus      = new TH1F("looseMuPlusTrackPhiEtaMinus","looseMuPlusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaPlusPhiMinus      = new TH1F("looseMuPlusTrackPhiEtaPlusPhiMinus","looseMuPlusTrackPhiEtaPlusPhiMinus",
								     40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaPlusPhiZero      = new TH1F("looseMuPlusTrackPhiEtaPlusPhiZero","looseMuPlusTrackPhiEtaPlusPhiZero",
								    40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaPlusPhiPlus      = new TH1F("looseMuPlusTrackPhiEtaPlusPhiPlus","looseMuPlusTrackPhiEtaPlusPhiPlus",
								    40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaMinusPhiMinus      = new TH1F("looseMuPlusTrackPhiEtaMinusPhiMinus","looseMuPlusTrackPhiEtaMinusPhiMinus",
								      40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaMinusPhiZero      = new TH1F("looseMuPlusTrackPhiEtaMinusPhiZero", "looseMuPlusTrackPhiEtaMinusPhiZero",
								     40, -4., 4.);
  TH1F *h_looseMuPlusTrackPhiEtaMinusPhiPlus      = new TH1F("looseMuPlusTrackPhiEtaMinusPhiPlus", "looseMuPlusTrackPhiEtaMinusPhiPlus",
								     40, -4., 4.);


  TH1F *h_looseMuUpperMinusTrackPhi = new TH1F("looseMuUpperMinusTrackPhi","looseMuUpperMinusTrackPhi", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaPlus      = new TH1F("looseMuUpperMinusTrackPhiEtaPlus","looseMuUpperMinusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaMinus      = new TH1F("looseMuUpperMinusTrackPhiEtaMinus","looseMuUpperMinusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusTrackPhiEtaPlusPhiMinus","looseMuUpperMinusTrackPhiEtaPlusPhiMinus",
								     40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaPlusPhiZero      = new TH1F("looseMuUpperMinusTrackPhiEtaPlusPhiZero","looseMuUpperMinusTrackPhiEtaPlusPhiZero",
								    40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusTrackPhiEtaPlusPhiPlus","looseMuUpperMinusTrackPhiEtaPlusPhiPlus",
								    40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusTrackPhiEtaMinusPhiMinus","looseMuUpperMinusTrackPhiEtaMinusPhiMinus",
								      40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaMinusPhiZero      = new TH1F("looseMuUpperMinusTrackPhiEtaMinusPhiZero", "looseMuUpperMinusTrackPhiEtaMinusPhiZero",
								     40, -4., 4.);
  TH1F *h_looseMuUpperMinusTrackPhiEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusTrackPhiEtaMinusPhiPlus", "looseMuUpperMinusTrackPhiEtaMinusPhiPlus",
								     40, -4., 4.);


  TH1F *h_looseMuUpperPlusTrackPhi  = new TH1F("looseMuUpperPlusTrackPhi", "looseMuUpperPlusTrackPhi",  40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaPlus      = new TH1F("looseMuUpperPlusTrackPhiEtaPlus","looseMuUpperPlusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaMinus      = new TH1F("looseMuUpperPlusTrackPhiEtaMinus","looseMuUpperPlusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusTrackPhiEtaPlusPhiMinus","looseMuUpperPlusTrackPhiEtaPlusPhiMinus",
								     40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaPlusPhiZero      = new TH1F("looseMuUpperPlusTrackPhiEtaPlusPhiZero","looseMuUpperPlusTrackPhiEtaPlusPhiZero",
								    40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusTrackPhiEtaPlusPhiPlus","looseMuUpperPlusTrackPhiEtaPlusPhiPlus",
								    40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusTrackPhiEtaMinusPhiMinus","looseMuUpperPlusTrackPhiEtaMinusPhiMinus",
								      40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaMinusPhiZero      = new TH1F("looseMuUpperPlusTrackPhiEtaMinusPhiZero", "looseMuUpperPlusTrackPhiEtaMinusPhiZero",
								     40, -4., 4.);
  TH1F *h_looseMuUpperPlusTrackPhiEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusTrackPhiEtaMinusPhiPlus", "looseMuUpperPlusTrackPhiEtaMinusPhiPlus",
								     40, -4., 4.);


  TH1F *h_looseMuLowerMinusTrackPhi = new TH1F("looseMuLowerMinusTrackPhi","looseMuLowerMinusTrackPhi", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaPlus      = new TH1F("looseMuLowerMinusTrackPhiEtaPlus","looseMuLowerMinusTrackPhiEtaPlus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaMinus      = new TH1F("looseMuLowerMinusTrackPhiEtaMinus","looseMuLowerMinusTrackPhiEtaMinus", 40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusTrackPhiEtaPlusPhiMinus","looseMuLowerMinusTrackPhiEtaPlusPhiMinus",
								     40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaPlusPhiZero      = new TH1F("looseMuLowerMinusTrackPhiEtaPlusPhiZero","looseMuLowerMinusTrackPhiEtaPlusPhiZero",
								    40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusTrackPhiEtaPlusPhiPlus","looseMuLowerMinusTrackPhiEtaPlusPhiPlus",
								    40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusTrackPhiEtaMinusPhiMinus","looseMuLowerMinusTrackPhiEtaMinusPhiMinus",
								      40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaMinusPhiZero      = new TH1F("looseMuLowerMinusTrackPhiEtaMinusPhiZero", "looseMuLowerMinusTrackPhiEtaMinusPhiZero",
								     40, -4., 4.);
  TH1F *h_looseMuLowerMinusTrackPhiEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusTrackPhiEtaMinusPhiPlus", "looseMuLowerMinusTrackPhiEtaMinusPhiPlus",
								     40, -4., 4.);


  TH1F *h_looseMuLowerPlusTrackPhi  = new TH1F("looseMuLowerPlusTrackPhi", "looseMuLowerPlusTrackPhi",  40, -4., 4.);
  TH1F *h_looseMuLowerPlusTrackPhiEtaPlus      = new TH1F("looseMuLowerPlusTrackPhiEtaPlus","looseMuLowerPlusTrackPhiEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaMinus      = new TH1F("looseMuLowerPlusTrackPhiEtaMinus","looseMuLowerPlusTrackPhiEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusTrackPhiEtaPlusPhiMinus","looseMuLowerPlusTrackPhiEtaPlusPhiMinus",
								     20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaPlusPhiZero      = new TH1F("looseMuLowerPlusTrackPhiEtaPlusPhiZero","looseMuLowerPlusTrackPhiEtaPlusPhiZero",
								    20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusTrackPhiEtaPlusPhiPlus","looseMuLowerPlusTrackPhiEtaPlusPhiPlus",
								    20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusTrackPhiEtaMinusPhiMinus","looseMuLowerPlusTrackPhiEtaMinusPhiMinus",
								      20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaMinusPhiZero      = new TH1F("looseMuLowerPlusTrackPhiEtaMinusPhiZero", "looseMuLowerPlusTrackPhiEtaMinusPhiZero",
								     20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackPhiEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusTrackPhiEtaMinusPhiPlus", "looseMuLowerPlusTrackPhiEtaMinusPhiPlus",
								     20, -0.5, 19.5);



  TH1F *h_looseMuMinusPtError      = new TH1F("looseMuMinusPtError",     "looseMuMinusPtError",      100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaPlus      = new TH1F("looseMuMinusPtErrorEtaPlus","looseMuMinusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaMinus      = new TH1F("looseMuMinusPtErrorEtaMinus","looseMuMinusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaPlusPhiMinus      = new TH1F("looseMuMinusPtErrorEtaPlusPhiMinus","looseMuMinusPtErrorEtaPlusPhiMinus",
								     100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaPlusPhiZero      = new TH1F("looseMuMinusPtErrorEtaPlusPhiZero","looseMuMinusPtErrorEtaPlusPhiZero",
								    100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaPlusPhiPlus      = new TH1F("looseMuMinusPtErrorEtaPlusPhiPlus","looseMuMinusPtErrorEtaPlusPhiPlus",
								    100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaMinusPhiMinus      = new TH1F("looseMuMinusPtErrorEtaMinusPhiMinus","looseMuMinusPtErrorEtaMinusPhiMinus",
								      100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaMinusPhiZero      = new TH1F("looseMuMinusPtErrorEtaMinusPhiZero", "looseMuMinusPtErrorEtaMinusPhiZero",
								     100, 0., 600.);
  TH1F *h_looseMuMinusPtErrorEtaMinusPhiPlus      = new TH1F("looseMuMinusPtErrorEtaMinusPhiPlus", "looseMuMinusPtErrorEtaMinusPhiPlus",
								     100, 0., 600.);

  TH1F *h_looseMuPlusPtError       = new TH1F("looseMuPlusPtError",      "looseMuPlusPtError",       100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaPlus      = new TH1F("looseMuPlusPtErrorEtaPlus","looseMuPlusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaMinus      = new TH1F("looseMuPlusPtErrorEtaMinus","looseMuPlusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaPlusPhiMinus      = new TH1F("looseMuPlusPtErrorEtaPlusPhiMinus","looseMuPlusPtErrorEtaPlusPhiMinus",
								     100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaPlusPhiZero      = new TH1F("looseMuPlusPtErrorEtaPlusPhiZero","looseMuPlusPtErrorEtaPlusPhiZero",
								    100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaPlusPhiPlus      = new TH1F("looseMuPlusPtErrorEtaPlusPhiPlus","looseMuPlusPtErrorEtaPlusPhiPlus",
								    100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaMinusPhiMinus      = new TH1F("looseMuPlusPtErrorEtaMinusPhiMinus","looseMuPlusPtErrorEtaMinusPhiMinus",
								      100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaMinusPhiZero      = new TH1F("looseMuPlusPtErrorEtaMinusPhiZero", "looseMuPlusPtErrorEtaMinusPhiZero",
								     100, 0., 600.);
  TH1F *h_looseMuPlusPtErrorEtaMinusPhiPlus      = new TH1F("looseMuPlusPtErrorEtaMinusPhiPlus", "looseMuPlusPtErrorEtaMinusPhiPlus",
								     100, 0., 600.);

  TH1F *h_looseMuUpperMinusPtError = new TH1F("looseMuUpperMinusPtError","looseMuUpperMinusPtError", 100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaPlus      = new TH1F("looseMuUpperMinusPtErrorEtaPlus","looseMuUpperMinusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaMinus      = new TH1F("looseMuUpperMinusPtErrorEtaMinus","looseMuUpperMinusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusPtErrorEtaPlusPhiMinus","looseMuUpperMinusPtErrorEtaPlusPhiMinus",
								     100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaPlusPhiZero      = new TH1F("looseMuUpperMinusPtErrorEtaPlusPhiZero","looseMuUpperMinusPtErrorEtaPlusPhiZero",
								    100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusPtErrorEtaPlusPhiPlus","looseMuUpperMinusPtErrorEtaPlusPhiPlus",
								    100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusPtErrorEtaMinusPhiMinus","looseMuUpperMinusPtErrorEtaMinusPhiMinus",
								      100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaMinusPhiZero      = new TH1F("looseMuUpperMinusPtErrorEtaMinusPhiZero", "looseMuUpperMinusPtErrorEtaMinusPhiZero",
								     100, 0., 600.);
  TH1F *h_looseMuUpperMinusPtErrorEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusPtErrorEtaMinusPhiPlus", "looseMuUpperMinusPtErrorEtaMinusPhiPlus",
								     100, 0., 600.);

  TH1F *h_looseMuUpperPlusPtError  = new TH1F("looseMuUpperPlusPtError", "looseMuUpperPlusPtError",  100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaPlus      = new TH1F("looseMuUpperPlusPtErrorEtaPlus","looseMuUpperPlusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaMinus      = new TH1F("looseMuUpperPlusPtErrorEtaMinus","looseMuUpperPlusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusPtErrorEtaPlusPhiMinus","looseMuUpperPlusPtErrorEtaPlusPhiMinus",
								     100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaPlusPhiZero      = new TH1F("looseMuUpperPlusPtErrorEtaPlusPhiZero","looseMuUpperPlusPtErrorEtaPlusPhiZero",
								    100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusPtErrorEtaPlusPhiPlus","looseMuUpperPlusPtErrorEtaPlusPhiPlus",
								    100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusPtErrorEtaMinusPhiMinus","looseMuUpperPlusPtErrorEtaMinusPhiMinus",
								      100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaMinusPhiZero      = new TH1F("looseMuUpperPlusPtErrorEtaMinusPhiZero", "looseMuUpperPlusPtErrorEtaMinusPhiZero",
								     100, 0., 600.);
  TH1F *h_looseMuUpperPlusPtErrorEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusPtErrorEtaMinusPhiPlus", "looseMuUpperPlusPtErrorEtaMinusPhiPlus",
								     100, 0., 600.);

  TH1F *h_looseMuLowerMinusPtError = new TH1F("looseMuLowerMinusPtError","looseMuLowerMinusPtError", 100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaPlus      = new TH1F("looseMuLowerMinusPtErrorEtaPlus","looseMuLowerMinusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaMinus      = new TH1F("looseMuLowerMinusPtErrorEtaMinus","looseMuLowerMinusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusPtErrorEtaPlusPhiMinus","looseMuLowerMinusPtErrorEtaPlusPhiMinus",
								     100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaPlusPhiZero      = new TH1F("looseMuLowerMinusPtErrorEtaPlusPhiZero","looseMuLowerMinusPtErrorEtaPlusPhiZero",
								    100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusPtErrorEtaPlusPhiPlus","looseMuLowerMinusPtErrorEtaPlusPhiPlus",
								    100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusPtErrorEtaMinusPhiMinus","looseMuLowerMinusPtErrorEtaMinusPhiMinus",
								      100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaMinusPhiZero      = new TH1F("looseMuLowerMinusPtErrorEtaMinusPhiZero", "looseMuLowerMinusPtErrorEtaMinusPhiZero",
								     100, 0., 600.);
  TH1F *h_looseMuLowerMinusPtErrorEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusPtErrorEtaMinusPhiPlus", "looseMuLowerMinusPtErrorEtaMinusPhiPlus",
								     100, 0., 600.);

  TH1F *h_looseMuLowerPlusPtError  = new TH1F("looseMuLowerPlusPtError", "looseMuLowerPlusPtError",  100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaPlus      = new TH1F("looseMuLowerPlusPtErrorEtaPlus","looseMuLowerPlusPtErrorEtaPlus", 100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaMinus      = new TH1F("looseMuLowerPlusPtErrorEtaMinus","looseMuLowerPlusPtErrorEtaMinus", 100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusPtErrorEtaPlusPhiMinus","looseMuLowerPlusPtErrorEtaPlusPhiMinus",
								     100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaPlusPhiZero      = new TH1F("looseMuLowerPlusPtErrorEtaPlusPhiZero","looseMuLowerPlusPtErrorEtaPlusPhiZero",
								    100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusPtErrorEtaPlusPhiPlus","looseMuLowerPlusPtErrorEtaPlusPhiPlus",
								    100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusPtErrorEtaMinusPhiMinus","looseMuLowerPlusPtErrorEtaMinusPhiMinus",
								      100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaMinusPhiZero      = new TH1F("looseMuLowerPlusPtErrorEtaMinusPhiZero", "looseMuLowerPlusPtErrorEtaMinusPhiZero",
								     100, 0., 600.);
  TH1F *h_looseMuLowerPlusPtErrorEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusPtErrorEtaMinusPhiPlus", "looseMuLowerPlusPtErrorEtaMinusPhiPlus",
								     100, 0., 600.);


  TH1F *h_looseMuMinusPtRelErr      = new TH1F("looseMuMinusPtRelErr",     "looseMuMinusPtRelErr",      100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaPlus      = new TH1F("looseMuMinusPtRelErrEtaPlus","looseMuMinusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaMinus      = new TH1F("looseMuMinusPtRelErrEtaMinus","looseMuMinusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaPlusPhiMinus      = new TH1F("looseMuMinusPtRelErrEtaPlusPhiMinus","looseMuMinusPtRelErrEtaPlusPhiMinus",
								     100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaPlusPhiZero      = new TH1F("looseMuMinusPtRelErrEtaPlusPhiZero","looseMuMinusPtRelErrEtaPlusPhiZero",
								    100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaPlusPhiPlus      = new TH1F("looseMuMinusPtRelErrEtaPlusPhiPlus","looseMuMinusPtRelErrEtaPlusPhiPlus",
								    100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaMinusPhiMinus      = new TH1F("looseMuMinusPtRelErrEtaMinusPhiMinus","looseMuMinusPtRelErrEtaMinusPhiMinus",
								      100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaMinusPhiZero      = new TH1F("looseMuMinusPtRelErrEtaMinusPhiZero", "looseMuMinusPtRelErrEtaMinusPhiZero",
								     100, 0., 1.);
  TH1F *h_looseMuMinusPtRelErrEtaMinusPhiPlus      = new TH1F("looseMuMinusPtRelErrEtaMinusPhiPlus", "looseMuMinusPtRelErrEtaMinusPhiPlus",
								     100, 0., 1.);

  TH1F *h_looseMuPlusPtRelErr       = new TH1F("looseMuPlusPtRelErr",      "looseMuPlusPtRelErr",       100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaPlus      = new TH1F("looseMuPlusPtRelErrEtaPlus","looseMuPlusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaMinus      = new TH1F("looseMuPlusPtRelErrEtaMinus","looseMuPlusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaPlusPhiMinus      = new TH1F("looseMuPlusPtRelErrEtaPlusPhiMinus","looseMuPlusPtRelErrEtaPlusPhiMinus",
								     100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaPlusPhiZero      = new TH1F("looseMuPlusPtRelErrEtaPlusPhiZero","looseMuPlusPtRelErrEtaPlusPhiZero",
								    100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaPlusPhiPlus      = new TH1F("looseMuPlusPtRelErrEtaPlusPhiPlus","looseMuPlusPtRelErrEtaPlusPhiPlus",
								    100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaMinusPhiMinus      = new TH1F("looseMuPlusPtRelErrEtaMinusPhiMinus","looseMuPlusPtRelErrEtaMinusPhiMinus",
								      100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaMinusPhiZero      = new TH1F("looseMuPlusPtRelErrEtaMinusPhiZero", "looseMuPlusPtRelErrEtaMinusPhiZero",
								     100, 0., 1.);
  TH1F *h_looseMuPlusPtRelErrEtaMinusPhiPlus      = new TH1F("looseMuPlusPtRelErrEtaMinusPhiPlus", "looseMuPlusPtRelErrEtaMinusPhiPlus",
								     100, 0., 1.);

  TH1F *h_looseMuUpperMinusPtRelErr = new TH1F("looseMuUpperMinusPtRelErr","looseMuUpperMinusPtRelErr", 100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaPlus      = new TH1F("looseMuUpperMinusPtRelErrEtaPlus","looseMuUpperMinusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaMinus      = new TH1F("looseMuUpperMinusPtRelErrEtaMinus","looseMuUpperMinusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusPtRelErrEtaPlusPhiMinus","looseMuUpperMinusPtRelErrEtaPlusPhiMinus",
								     100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaPlusPhiZero      = new TH1F("looseMuUpperMinusPtRelErrEtaPlusPhiZero","looseMuUpperMinusPtRelErrEtaPlusPhiZero",
								    100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusPtRelErrEtaPlusPhiPlus","looseMuUpperMinusPtRelErrEtaPlusPhiPlus",
								    100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusPtRelErrEtaMinusPhiMinus","looseMuUpperMinusPtRelErrEtaMinusPhiMinus",
								      100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaMinusPhiZero      = new TH1F("looseMuUpperMinusPtRelErrEtaMinusPhiZero", "looseMuUpperMinusPtRelErrEtaMinusPhiZero",
								     100, 0., 1.);
  TH1F *h_looseMuUpperMinusPtRelErrEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusPtRelErrEtaMinusPhiPlus", "looseMuUpperMinusPtRelErrEtaMinusPhiPlus",
								     100, 0., 1.);

  TH1F *h_looseMuUpperPlusPtRelErr  = new TH1F("looseMuUpperPlusPtRelErr", "looseMuUpperPlusPtRelErr",  100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaPlus      = new TH1F("looseMuUpperPlusPtRelErrEtaPlus","looseMuUpperPlusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaMinus      = new TH1F("looseMuUpperPlusPtRelErrEtaMinus","looseMuUpperPlusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusPtRelErrEtaPlusPhiMinus","looseMuUpperPlusPtRelErrEtaPlusPhiMinus",
								     100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaPlusPhiZero      = new TH1F("looseMuUpperPlusPtRelErrEtaPlusPhiZero","looseMuUpperPlusPtRelErrEtaPlusPhiZero",
								    100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusPtRelErrEtaPlusPhiPlus","looseMuUpperPlusPtRelErrEtaPlusPhiPlus",
								    100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusPtRelErrEtaMinusPhiMinus","looseMuUpperPlusPtRelErrEtaMinusPhiMinus",
								      100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaMinusPhiZero      = new TH1F("looseMuUpperPlusPtRelErrEtaMinusPhiZero", "looseMuUpperPlusPtRelErrEtaMinusPhiZero",
								     100, 0., 1.);
  TH1F *h_looseMuUpperPlusPtRelErrEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusPtRelErrEtaMinusPhiPlus", "looseMuUpperPlusPtRelErrEtaMinusPhiPlus",
								     100, 0., 1.);

  TH1F *h_looseMuLowerMinusPtRelErr = new TH1F("looseMuLowerMinusPtRelErr","looseMuLowerMinusPtRelErr", 100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaPlus      = new TH1F("looseMuLowerMinusPtRelErrEtaPlus","looseMuLowerMinusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaMinus      = new TH1F("looseMuLowerMinusPtRelErrEtaMinus","looseMuLowerMinusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusPtRelErrEtaPlusPhiMinus","looseMuLowerMinusPtRelErrEtaPlusPhiMinus",
								     100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaPlusPhiZero      = new TH1F("looseMuLowerMinusPtRelErrEtaPlusPhiZero","looseMuLowerMinusPtRelErrEtaPlusPhiZero",
								    100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusPtRelErrEtaPlusPhiPlus","looseMuLowerMinusPtRelErrEtaPlusPhiPlus",
								    100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusPtRelErrEtaMinusPhiMinus","looseMuLowerMinusPtRelErrEtaMinusPhiMinus",
								      100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaMinusPhiZero      = new TH1F("looseMuLowerMinusPtRelErrEtaMinusPhiZero", "looseMuLowerMinusPtRelErrEtaMinusPhiZero",
								     100, 0., 1.);
  TH1F *h_looseMuLowerMinusPtRelErrEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusPtRelErrEtaMinusPhiPlus", "looseMuLowerMinusPtRelErrEtaMinusPhiPlus",
								     100, 0., 1.);

  TH1F *h_looseMuLowerPlusPtRelErr  = new TH1F("looseMuLowerPlusPtRelErr","looseMuLowerPlusPtRelErr",   100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaPlus      = new TH1F("looseMuLowerPlusPtRelErrEtaPlus","looseMuLowerPlusPtRelErrEtaPlus", 100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaMinus      = new TH1F("looseMuLowerPlusPtRelErrEtaMinus","looseMuLowerPlusPtRelErrEtaMinus", 100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusPtRelErrEtaPlusPhiMinus","looseMuLowerPlusPtRelErrEtaPlusPhiMinus",
								     100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaPlusPhiZero      = new TH1F("looseMuLowerPlusPtRelErrEtaPlusPhiZero","looseMuLowerPlusPtRelErrEtaPlusPhiZero",
								    100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusPtRelErrEtaPlusPhiPlus","looseMuLowerPlusPtRelErrEtaPlusPhiPlus",
								    100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusPtRelErrEtaMinusPhiMinus","looseMuLowerPlusPtRelErrEtaMinusPhiMinus",
								      100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaMinusPhiZero      = new TH1F("looseMuLowerPlusPtRelErrEtaMinusPhiZero", "looseMuLowerPlusPtRelErrEtaMinusPhiZero",
								     100, 0., 1.);
  TH1F *h_looseMuLowerPlusPtRelErrEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusPtRelErrEtaMinusPhiPlus", "looseMuLowerPlusPtRelErrEtaMinusPhiPlus",
								     100, 0., 1.);

  TH1F *h_looseMuMinusPixelHits      = new TH1F("looseMuMinusPixelHits",     "looseMuMinusPixelHits",      10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaPlus      = new TH1F("looseMuMinusPixelHitsEtaPlus","looseMuMinusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaMinus      = new TH1F("looseMuMinusPixelHitsEtaMinus","looseMuMinusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaPlusPhiMinus      = new TH1F("looseMuMinusPixelHitsEtaPlusPhiMinus","looseMuMinusPixelHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaPlusPhiZero      = new TH1F("looseMuMinusPixelHitsEtaPlusPhiZero","looseMuMinusPixelHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaPlusPhiPlus      = new TH1F("looseMuMinusPixelHitsEtaPlusPhiPlus","looseMuMinusPixelHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaMinusPhiMinus      = new TH1F("looseMuMinusPixelHitsEtaMinusPhiMinus","looseMuMinusPixelHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaMinusPhiZero      = new TH1F("looseMuMinusPixelHitsEtaMinusPhiZero", "looseMuMinusPixelHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuMinusPixelHitsEtaMinusPhiPlus      = new TH1F("looseMuMinusPixelHitsEtaMinusPhiPlus", "looseMuMinusPixelHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_looseMuPlusPixelHits       = new TH1F("looseMuPlusPixelHits",      "looseMuPlusPixelHits",       10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaPlus      = new TH1F("looseMuPlusPixelHitsEtaPlus","looseMuPlusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaMinus      = new TH1F("looseMuPlusPixelHitsEtaMinus","looseMuPlusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaPlusPhiMinus      = new TH1F("looseMuPlusPixelHitsEtaPlusPhiMinus","looseMuPlusPixelHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaPlusPhiZero      = new TH1F("looseMuPlusPixelHitsEtaPlusPhiZero","looseMuPlusPixelHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaPlusPhiPlus      = new TH1F("looseMuPlusPixelHitsEtaPlusPhiPlus","looseMuPlusPixelHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaMinusPhiMinus      = new TH1F("looseMuPlusPixelHitsEtaMinusPhiMinus","looseMuPlusPixelHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaMinusPhiZero      = new TH1F("looseMuPlusPixelHitsEtaMinusPhiZero", "looseMuPlusPixelHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuPlusPixelHitsEtaMinusPhiPlus      = new TH1F("looseMuPlusPixelHitsEtaMinusPhiPlus", "looseMuPlusPixel",
								     10, -0.5, 9.5);

  TH1F *h_looseMuUpperMinusPixelHits = new TH1F("looseMuUpperMinusPixelHits","looseMuUpperMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaPlus      = new TH1F("looseMuUpperMinusPixelHitsEtaPlus","looseMuUpperMinusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaMinus      = new TH1F("looseMuUpperMinusPixelHitsEtaMinus","looseMuUpperMinusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusPixelHitsEtaPlusPhiMinus","looseMuUpperMinusPixelHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaPlusPhiZero      = new TH1F("looseMuUpperMinusPixelHitsEtaPlusPhiZero","looseMuUpperMinusPixelHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusPixelHitsEtaPlusPhiPlus","looseMuUpperMinusPixelHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusPixelHitsEtaMinusPhiMinus","looseMuUpperMinusPixelHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaMinusPhiZero      = new TH1F("looseMuUpperMinusPixelHitsEtaMinusPhiZero", "looseMuUpperMinusPixelHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusPixelHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusPixelHitsEtaMinusPhiPlus", "looseMuUpperMinusPixelHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_looseMuUpperPlusPixelHits  = new TH1F("looseMuUpperPlusPixelHits", "looseMuUpperPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaPlus      = new TH1F("looseMuUpperPlusPixelHitsEtaPlus","looseMuUpperPlusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaMinus      = new TH1F("looseMuUpperPlusPixelHitsEtaMinus","looseMuUpperPlusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusPixelHitsEtaPlusPhiMinus","looseMuUpperPlusPixelHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaPlusPhiZero      = new TH1F("looseMuUpperPlusPixelHitsEtaPlusPhiZero","looseMuUpperPlusPixelHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusPixelHitsEtaPlusPhiPlus","looseMuUpperPlusPixelHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusPixelHitsEtaMinusPhiMinus","looseMuUpperPlusPixelHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaMinusPhiZero      = new TH1F("looseMuUpperPlusPixelHitsEtaMinusPhiZero", "looseMuUpperPlusPixelHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusPixelHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusPixelHitsEtaMinusPhiPlus", "looseMuUpperPlusPixelHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_looseMuLowerMinusPixelHits = new TH1F("looseMuLowerMinusPixelHits","looseMuLowerMinusPixelHits", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaPlus      = new TH1F("looseMuLowerMinusPixelHitsEtaPlus","looseMuLowerMinusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaMinus      = new TH1F("looseMuLowerMinusPixelHitsEtaMinus","looseMuLowerMinusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusPixelHitsEtaPlusPhiMinus","looseMuLowerMinusPixelHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaPlusPhiZero      = new TH1F("looseMuLowerMinusPixelHitsEtaPlusPhiZero","looseMuLowerMinusPixelHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusPixelHitsEtaPlusPhiPlus","looseMuLowerMinusPixelHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusPixelHitsEtaMinusPhiMinus","looseMuLowerMinusPixelHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaMinusPhiZero      = new TH1F("looseMuLowerMinusPixelHitsEtaMinusPhiZero", "looseMuLowerMinusPixelHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusPixelHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusPixelHitsEtaMinusPhiPlus", "looseMuLowerMinusPixelHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_looseMuLowerPlusPixelHits  = new TH1F("looseMuLowerPlusPixelHits", "looseMuLowerPlusPixelHits",  10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaPlus      = new TH1F("looseMuLowerPlusPixelHitsEtaPlus","looseMuLowerPlusPixelHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaMinus      = new TH1F("looseMuLowerPlusPixelHitsEtaMinus","looseMuLowerPlusPixelHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusPixelHitsEtaPlusPhiMinus","looseMuLowerPlusPixelHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaPlusPhiZero      = new TH1F("looseMuLowerPlusPixelHitsEtaPlusPhiZero","looseMuLowerPlusPixelHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusPixelHitsEtaPlusPhiPlus","looseMuLowerPlusPixelHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusPixelHitsEtaMinusPhiMinus","looseMuLowerPlusPixelHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaMinusPhiZero      = new TH1F("looseMuLowerPlusPixelHitsEtaMinusPhiZero", "looseMuLowerPlusPixelHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusPixelHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusPixelHitsEtaMinusPhiPlus", "looseMuLowerPlusPixelHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);


  TH1F *h_looseMuMinusTrackerHits      = new TH1F("looseMuMinusTrackerHits",     "looseMuMinusTrackerHits",      35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaPlus      = new TH1F("looseMuMinusTrackerHitsEtaPlus","looseMuMinusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaMinus      = new TH1F("looseMuMinusTrackerHitsEtaMinus","looseMuMinusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaPlusPhiMinus      = new TH1F("looseMuMinusTrackerHitsEtaPlusPhiMinus","looseMuMinusTrackerHitsEtaPlusPhiMinus",
								     35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaPlusPhiZero      = new TH1F("looseMuMinusTrackerHitsEtaPlusPhiZero","looseMuMinusTrackerHitsEtaPlusPhiZero",
								    35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaPlusPhiPlus      = new TH1F("looseMuMinusTrackerHitsEtaPlusPhiPlus","looseMuMinusTrackerHitsEtaPlusPhiPlus",
								    35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaMinusPhiMinus      = new TH1F("looseMuMinusTrackerHitsEtaMinusPhiMinus","looseMuMinusTrackerHitsEtaMinusPhiMinus",
								      35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaMinusPhiZero      = new TH1F("looseMuMinusTrackerHitsEtaMinusPhiZero", "looseMuMinusTrackerHitsEtaMinusPhiZero",
								     35, -0.5, 34.5);
  TH1F *h_looseMuMinusTrackerHitsEtaMinusPhiPlus      = new TH1F("looseMuMinusTrackerHitsEtaMinusPhiPlus", "looseMuMinusTrackerHitsEtaMinusPhiPlus",
								     35, -0.5, 34.5);

  TH1F *h_looseMuPlusTrackerHits       = new TH1F("looseMuPlusTrackerHits",      "looseMuPlusTrackerHits",       35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaPlus      = new TH1F("looseMuPlusTrackerHitsEtaPlus","looseMuPlusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaMinus      = new TH1F("looseMuPlusTrackerHitsEtaMinus","looseMuPlusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaPlusPhiMinus      = new TH1F("looseMuPlusTrackerHitsEtaPlusPhiMinus","looseMuPlusTrackerHitsEtaPlusPhiMinus",
								     35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaPlusPhiZero      = new TH1F("looseMuPlusTrackerHitsEtaPlusPhiZero","looseMuPlusTrackerHitsEtaPlusPhiZero",
								    35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaPlusPhiPlus      = new TH1F("looseMuPlusTrackerHitsEtaPlusPhiPlus","looseMuPlusTrackerHitsEtaPlusPhiPlus",
								    35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaMinusPhiMinus      = new TH1F("looseMuPlusTrackerHitsEtaMinusPhiMinus","looseMuPlusTrackerHitsEtaMinusPhiMinus",
								      35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaMinusPhiZero      = new TH1F("looseMuPlusTrackerHitsEtaMinusPhiZero", "looseMuPlusTrackerHitsEtaMinusPhiZero",
								     35, -0.5, 34.5);
  TH1F *h_looseMuPlusTrackerHitsEtaMinusPhiPlus      = new TH1F("looseMuPlusTrackerHitsEtaMinusPhiPlus", "looseMuPlusTrackerHitsEtaMinusPhiPlus",
								     35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHits = new TH1F("looseMuUpperMinusTrackerHits","looseMuUpperMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaPlus      = new TH1F("looseMuUpperMinusTrackerHitsEtaPlus","looseMuUpperMinusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaMinus      = new TH1F("looseMuUpperMinusTrackerHitsEtaMinus","looseMuUpperMinusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusTrackerHitsEtaPlusPhiMinus","looseMuUpperMinusTrackerHitsEtaPlusPhiMinus",
								     35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaPlusPhiZero      = new TH1F("looseMuUpperMinusTrackerHitsEtaPlusPhiZero","looseMuUpperMinusTrackerHitsEtaPlusPhiZero",
								    35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusTrackerHitsEtaPlusPhiPlus","looseMuUpperMinusTrackerHitsEtaPlusPhiPlus",
								    35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusTrackerHitsEtaMinusPhiMinus","looseMuUpperMinusTrackerHitsEtaMinusPhiMinus",
								      35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaMinusPhiZero      = new TH1F("looseMuUpperMinusTrackerHitsEtaMinusPhiZero", "looseMuUpperMinusTrackerHitsEtaMinusPhiZero",
								     35, -0.5, 34.5);
  TH1F *h_looseMuUpperMinusTrackerHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusTrackerHitsEtaMinusPhiPlus", "looseMuUpperMinusTrackerHitsEtaMinusPhiPlus",
								     35, -0.5, 34.5);

  TH1F *h_looseMuUpperPlusTrackerHits  = new TH1F("looseMuUpperPlusTrackerHits", "looseMuUpperPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaPlus      = new TH1F("looseMuUpperPlusTrackerHitsEtaPlus","looseMuUpperPlusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaMinus      = new TH1F("looseMuUpperPlusTrackerHitsEtaMinus","looseMuUpperPlusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusTrackerHitsEtaPlusPhiMinus","looseMuUpperPlusTrackerHitsEtaPlusPhiMinus",
								     35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaPlusPhiZero      = new TH1F("looseMuUpperPlusTrackerHitsEtaPlusPhiZero","looseMuUpperPlusTrackerHitsEtaPlusPhiZero",
								    35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusTrackerHitsEtaPlusPhiPlus","looseMuUpperPlusTrackerHitsEtaPlusPhiPlus",
								    35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusTrackerHitsEtaMinusPhiMinus","looseMuUpperPlusTrackerHitsEtaMinusPhiMinus",
								      35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaMinusPhiZero      = new TH1F("looseMuUpperPlusTrackerHitsEtaMinusPhiZero", "looseMuUpperPlusTrackerHitsEtaMinusPhiZero",
								     35, -0.5, 34.5);
  TH1F *h_looseMuUpperPlusTrackerHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusTrackerHitsEtaMinusPhiPlus", "looseMuUpperPlusTrackerHitsEtaMinusPhiPlus",
								     35, -0.5, 34.5);

  TH1F *h_looseMuLowerMinusTrackerHits = new TH1F("looseMuLowerMinusTrackerHits","looseMuLowerMinusTrackerHits", 35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaPlus      = new TH1F("looseMuLowerMinusTrackerHitsEtaPlus","looseMuLowerMinusTrackerHitsEtaPlus", 35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaMinus      = new TH1F("looseMuLowerMinusTrackerHitsEtaMinus","looseMuLowerMinusTrackerHitsEtaMinus", 35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusTrackerHitsEtaPlusPhiMinus","looseMuLowerMinusTrackerHitsEtaPlusPhiMinus",
								     35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaPlusPhiZero      = new TH1F("looseMuLowerMinusTrackerHitsEtaPlusPhiZero","looseMuLowerMinusTrackerHitsEtaPlusPhiZero",
								    35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusTrackerHitsEtaPlusPhiPlus","looseMuLowerMinusTrackerHitsEtaPlusPhiPlus",
								    35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusTrackerHitsEtaMinusPhiMinus","looseMuLowerMinusTrackerHitsEtaMinusPhiMinus",
								      35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaMinusPhiZero      = new TH1F("looseMuLowerMinusTrackerHitsEtaMinusPhiZero", "looseMuLowerMinusTrackerHitsEtaMinusPhiZero",
								     35, -0.5, 34.5);
  TH1F *h_looseMuLowerMinusTrackerHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusTrackerHitsEtaMinusPhiPlus", "looseMuLowerMinusTrackerHitsEtaMinusPhiPlus",
								     35, -0.5, 34.5);

  TH1F *h_looseMuLowerPlusTrackerHits  = new TH1F("looseMuLowerPlusTrackerHits", "looseMuLowerPlusTrackerHits",  35, -0.5, 34.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaPlus      = new TH1F("looseMuLowerPlusTrackerHitsEtaPlus","looseMuLowerPlusTrackerHitsEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaMinus      = new TH1F("looseMuLowerPlusTrackerHitsEtaMinus","looseMuLowerPlusTrackerHitsEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusTrackerHitsEtaPlusPhiMinus","looseMuLowerPlusTrackerHitsEtaPlusPhiMinus",
								     20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaPlusPhiZero      = new TH1F("looseMuLowerPlusTrackerHitsEtaPlusPhiZero","looseMuLowerPlusTrackerHitsEtaPlusPhiZero",
								    20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusTrackerHitsEtaPlusPhiPlus","looseMuLowerPlusTrackerHitsEtaPlusPhiPlus",
								    20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusTrackerHitsEtaMinusPhiMinus","looseMuLowerPlusTrackerHitsEtaMinusPhiMinus",
								      20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaMinusPhiZero      = new TH1F("looseMuLowerPlusTrackerHitsEtaMinusPhiZero", "looseMuLowerPlusTrackerHitsEtaMinusPhiZero",
								     20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusTrackerHitsEtaMinusPhiPlus", "looseMuLowerPlusTrackerHitsEtaMinusPhiPlus",
								     20, -0.5, 19.5);

  TH1F *h_looseMuMinusMuonStationHits      = new TH1F("looseMuMinusMuonStationHits",     "looseMuMinusMuonStationHits",      10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaPlus      = new TH1F("looseMuMinusMuonStationHitsEtaPlus","looseMuMinusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaMinus      = new TH1F("looseMuMinusMuonStationHitsEtaMinus","looseMuMinusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaPlusPhiMinus      = new TH1F("looseMuMinusMuonStationHitsEtaPlusPhiMinus","looseMuMinusMuonStationHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaPlusPhiZero      = new TH1F("looseMuMinusMuonStationHitsEtaPlusPhiZero","looseMuMinusMuonStationHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaPlusPhiPlus      = new TH1F("looseMuMinusMuonStationHitsEtaPlusPhiPlus","looseMuMinusMuonStationHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaMinusPhiMinus      = new TH1F("looseMuMinusMuonStationHitsEtaMinusPhiMinus","looseMuMinusMuonStationHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaMinusPhiZero      = new TH1F("looseMuMinusMuonStationHitsEtaMinusPhiZero", "looseMuMinusMuonStationHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuMinusMuonStationHitsEtaMinusPhiPlus      = new TH1F("looseMuMinusMuonStationHitsEtaMinusPhiPlus", "looseMuMinusMuonStationHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_looseMuPlusMuonStationHits       = new TH1F("looseMuPlusMuonStationHits",      "looseMuPlusMuonStationHits",       10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaPlus      = new TH1F("looseMuPlusMuonStationHitsEtaPlus","looseMuPlusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaMinus      = new TH1F("looseMuPlusMuonStationHitsEtaMinus","looseMuPlusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaPlusPhiMinus      = new TH1F("looseMuPlusMuonStationHitsEtaPlusPhiMinus","looseMuPlusMuonStationHitsEtaPlusPhiMinus",
								     10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaPlusPhiZero      = new TH1F("looseMuPlusMuonStationHitsEtaPlusPhiZero","looseMuPlusMuonStationHitsEtaPlusPhiZero",
								    10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaPlusPhiPlus      = new TH1F("looseMuPlusMuonStationHitsEtaPlusPhiPlus","looseMuPlusMuonStationHitsEtaPlusPhiPlus",
								    10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaMinusPhiMinus      = new TH1F("looseMuPlusMuonStationHitsEtaMinusPhiMinus","looseMuPlusMuonStationHitsEtaMinusPhiMinus",
								      10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaMinusPhiZero      = new TH1F("looseMuPlusMuonStationHitsEtaMinusPhiZero", "looseMuPlusMuonStationHitsEtaMinusPhiZero",
								     10, -0.5, 9.5);
  TH1F *h_looseMuPlusMuonStationHitsEtaMinusPhiPlus      = new TH1F("looseMuPlusMuonStationHitsEtaMinusPhiPlus", "looseMuPlusMuonStationHitsEtaMinusPhiPlus",
								     10, -0.5, 9.5);

  TH1F *h_looseMuUpperMinusMuonStationHits = new TH1F("looseMuUpperMinusMuonStationHits","looseMuUpperMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaPlus      = new TH1F("looseMuUpperMinusMuonStationHitsEtaPlus","looseMuUpperMinusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaMinus      = new TH1F("looseMuUpperMinusMuonStationHitsEtaMinus","looseMuUpperMinusMuonStationHitsEtaMinus",
								   10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusMuonStationHitsEtaPlusPhiMinus",
									  "looseMuUpperMinusMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaPlusPhiZero      = new TH1F("looseMuUpperMinusMuonStationHitsEtaPlusPhiZero",
									 "looseMuUpperMinusMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusMuonStationHitsEtaPlusPhiPlus",
									 "looseMuUpperMinusMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusMuonStationHitsEtaMinusPhiMinus",
									   "looseMuUpperMinusMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaMinusPhiZero      = new TH1F("looseMuUpperMinusMuonStationHitsEtaMinusPhiZero", 
									  "looseMuUpperMinusMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMuonStationHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusMuonStationHitsEtaMinusPhiPlus",
									  "looseMuUpperMinusMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuUpperPlusMuonStationHits  = new TH1F("looseMuUpperPlusMuonStationHits", "looseMuUpperPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaPlus      = new TH1F("looseMuUpperPlusMuonStationHitsEtaPlus","looseMuUpperPlusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaMinus      = new TH1F("looseMuUpperPlusMuonStationHitsEtaMinus","looseMuUpperPlusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusMuonStationHitsEtaPlusPhiMinus",
									 "looseMuUpperPlusMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaPlusPhiZero      = new TH1F("looseMuUpperPlusMuonStationHitsEtaPlusPhiZero",
									"looseMuUpperPlusMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusMuonStationHitsEtaPlusPhiPlus",
									"looseMuUpperPlusMuonStationHitsEtaPlusPhiPlus",10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusMuonStationHitsEtaMinusPhiMinus",
									  "looseMuUpperPlusMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaMinusPhiZero      = new TH1F("looseMuUpperPlusMuonStationHitsEtaMinusPhiZero",
									 "looseMuUpperPlusMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMuonStationHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusMuonStationHitsEtaMinusPhiPlus", 
									 "looseMuUpperPlusMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuLowerMinusMuonStationHits = new TH1F("looseMuLowerMinusMuonStationHits","looseMuLowerMinusMuonStationHits", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaPlus      = new TH1F("looseMuLowerMinusMuonStationHitsEtaPlus","looseMuLowerMinusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaMinus      = new TH1F("looseMuLowerMinusMuonStationHitsEtaMinus","looseMuLowerMinusMuonStationHitsEtaMinus",
								   10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusMuonStationHitsEtaPlusPhiMinus",
									  "looseMuLowerMinusMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaPlusPhiZero      = new TH1F("looseMuLowerMinusMuonStationHitsEtaPlusPhiZero",
									 "looseMuLowerMinusMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusMuonStationHitsEtaPlusPhiPlus",
									 "looseMuLowerMinusMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusMuonStationHitsEtaMinusPhiMinus",
									   "looseMuLowerMinusMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaMinusPhiZero      = new TH1F("looseMuLowerMinusMuonStationHitsEtaMinusPhiZero", 
									  "looseMuLowerMinusMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMuonStationHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusMuonStationHitsEtaMinusPhiPlus", 
									  "looseMuLowerMinusMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuLowerPlusMuonStationHits  = new TH1F("looseMuLowerPlusMuonStationHits", "looseMuLowerPlusMuonStationHits",  10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaPlus      = new TH1F("looseMuLowerPlusMuonStationHitsEtaPlus","looseMuLowerPlusMuonStationHitsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaMinus      = new TH1F("looseMuLowerPlusMuonStationHitsEtaMinus","looseMuLowerPlusMuonStationHitsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusMuonStationHitsEtaPlusPhiMinus",
									 "looseMuLowerPlusMuonStationHitsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaPlusPhiZero      = new TH1F("looseMuLowerPlusMuonStationHitsEtaPlusPhiZero",
									"looseMuLowerPlusMuonStationHitsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusMuonStationHitsEtaPlusPhiPlus",
									"looseMuLowerPlusMuonStationHitsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusMuonStationHitsEtaMinusPhiMinus",
									  "looseMuLowerPlusMuonStationHitsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaMinusPhiZero      = new TH1F("looseMuLowerPlusMuonStationHitsEtaMinusPhiZero", 
									 "looseMuLowerPlusMuonStationHitsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMuonStationHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusMuonStationHitsEtaMinusPhiPlus",
									 "looseMuLowerPlusMuonStationHitsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_looseMuMinusValidHits      = new TH1F("looseMuMinusValidHits",     "looseMuMinusValidHits",      100,-0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaPlus      = new TH1F("looseMuMinusValidHitsEtaPlus","looseMuMinusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaMinus      = new TH1F("looseMuMinusValidHitsEtaMinus","looseMuMinusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaPlusPhiMinus      = new TH1F("looseMuMinusValidHitsEtaPlusPhiMinus","looseMuMinusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaPlusPhiZero      = new TH1F("looseMuMinusValidHitsEtaPlusPhiZero","looseMuMinusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaPlusPhiPlus      = new TH1F("looseMuMinusValidHitsEtaPlusPhiPlus","looseMuMinusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaMinusPhiMinus      = new TH1F("looseMuMinusValidHitsEtaMinusPhiMinus","looseMuMinusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaMinusPhiZero      = new TH1F("looseMuMinusValidHitsEtaMinusPhiZero", "looseMuMinusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuMinusValidHitsEtaMinusPhiPlus      = new TH1F("looseMuMinusValidHitsEtaMinusPhiPlus", "looseMuMinusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuPlusValidHits       = new TH1F("looseMuPlusValidHits",      "looseMuPlusValidHits",       100,-0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaPlus      = new TH1F("looseMuPlusValidHitsEtaPlus","looseMuPlusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaMinus      = new TH1F("looseMuPlusValidHitsEtaMinus","looseMuPlusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaPlusPhiMinus      = new TH1F("looseMuPlusValidHitsEtaPlusPhiMinus","looseMuPlusValidHitsEtaPlusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaPlusPhiZero      = new TH1F("looseMuPlusValidHitsEtaPlusPhiZero","looseMuPlusValidHitsEtaPlusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaPlusPhiPlus      = new TH1F("looseMuPlusValidHitsEtaPlusPhiPlus","looseMuPlusValidHitsEtaPlusPhiPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaMinusPhiMinus      = new TH1F("looseMuPlusValidHitsEtaMinusPhiMinus","looseMuPlusValidHitsEtaMinusPhiMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaMinusPhiZero      = new TH1F("looseMuPlusValidHitsEtaMinusPhiZero", "looseMuPlusValidHitsEtaMinusPhiZero", 100, -0.5, 99.5);
  TH1F *h_looseMuPlusValidHitsEtaMinusPhiPlus      = new TH1F("looseMuPlusValidHitsEtaMinusPhiPlus", "looseMuPlusValidHitsEtaMinusPhiPlus", 100, -0.5, 99.5);

  TH1F *h_looseMuUpperMinusValidHits       = new TH1F("looseMuUpperMinusValidHits",      "looseMuUpperMinusValidHits",       100,-0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaPlus      = new TH1F("looseMuUpperMinusValidHitsEtaPlus","looseMuUpperMinusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaMinus      = new TH1F("looseMuUpperMinusValidHitsEtaMinus","looseMuUpperMinusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusValidHitsEtaPlusPhiMinus","looseMuUpperMinusValidHitsEtaPlusPhiMinus",
								    100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaPlusPhiZero      = new TH1F("looseMuUpperMinusValidHitsEtaPlusPhiZero","looseMuUpperMinusValidHitsEtaPlusPhiZero",
								   100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusValidHitsEtaPlusPhiPlus","looseMuUpperMinusValidHitsEtaPlusPhiPlus",
								   100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusValidHitsEtaMinusPhiMinus","looseMuUpperMinusValidHitsEtaMinusPhiMinus",
								     100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaMinusPhiZero      = new TH1F("looseMuUpperMinusValidHitsEtaMinusPhiZero", "looseMuUpperMinusValidHitsEtaMinusPhiZero",
								    100, -0.5, 99.5);
  TH1F *h_looseMuUpperMinusValidHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusValidHitsEtaMinusPhiPlus", "looseMuUpperMinusValidHitsEtaMinusPhiPlus",
								    100, -0.5, 99.5);

  TH1F *h_looseMuUpperPlusValidHits       = new TH1F("looseMuUpperPlusValidHits",      "looseMuUpperPlusValidHits",       100,-0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaPlus      = new TH1F("looseMuUpperPlusValidHitsEtaPlus","looseMuUpperPlusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaMinus      = new TH1F("looseMuUpperPlusValidHitsEtaMinus","looseMuUpperPlusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusValidHitsEtaPlusPhiMinus","looseMuUpperPlusValidHitsEtaPlusPhiMinus",
								    100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaPlusPhiZero      = new TH1F("looseMuUpperPlusValidHitsEtaPlusPhiZero","looseMuUpperPlusValidHitsEtaPlusPhiZero",
								   100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusValidHitsEtaPlusPhiPlus","looseMuUpperPlusValidHitsEtaPlusPhiPlus",
								   100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusValidHitsEtaMinusPhiMinus","looseMuUpperPlusValidHitsEtaMinusPhiMinus",
								     100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaMinusPhiZero      = new TH1F("looseMuUpperPlusValidHitsEtaMinusPhiZero", "looseMuUpperPlusValidHitsEtaMinusPhiZero",
								    100, -0.5, 99.5);
  TH1F *h_looseMuUpperPlusValidHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusValidHitsEtaMinusPhiPlus", "looseMuUpperPlusValidHitsEtaMinusPhiPlus",
								    100, -0.5, 99.5);

  TH1F *h_looseMuLowerMinusValidHits       = new TH1F("looseMuLowerMinusValidHits",      "looseMuLowerMinusValidHits",       100,-0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaPlus      = new TH1F("looseMuLowerMinusValidHitsEtaPlus","looseMuLowerMinusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaMinus      = new TH1F("looseMuLowerMinusValidHitsEtaMinus","looseMuLowerMinusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusValidHitsEtaPlusPhiMinus","looseMuLowerMinusValidHitsEtaPlusPhiMinus",
								    100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaPlusPhiZero      = new TH1F("looseMuLowerMinusValidHitsEtaPlusPhiZero","looseMuLowerMinusValidHitsEtaPlusPhiZero",
								   100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusValidHitsEtaPlusPhiPlus","looseMuLowerMinusValidHitsEtaPlusPhiPlus",
								   100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusValidHitsEtaMinusPhiMinus","looseMuLowerMinusValidHitsEtaMinusPhiMinus",
								     100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaMinusPhiZero      = new TH1F("looseMuLowerMinusValidHitsEtaMinusPhiZero", "looseMuLowerMinusValidHitsEtaMinusPhiZero",
								    100, -0.5, 99.5);
  TH1F *h_looseMuLowerMinusValidHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusValidHitsEtaMinusPhiPlus", "looseMuLowerMinusValidHitsEtaMinusPhiPlus",
								    100, -0.5, 99.5);

  TH1F *h_looseMuLowerPlusValidHits       = new TH1F("looseMuLowerPlusValidHits",      "looseMuLowerPlusValidHits",       100,-0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaPlus      = new TH1F("looseMuLowerPlusValidHitsEtaPlus","looseMuLowerPlusValidHitsEtaPlus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaMinus      = new TH1F("looseMuLowerPlusValidHitsEtaMinus","looseMuLowerPlusValidHitsEtaMinus", 100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusValidHitsEtaPlusPhiMinus","looseMuLowerPlusValidHitsEtaPlusPhiMinus",
								    100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaPlusPhiZero      = new TH1F("looseMuLowerPlusValidHitsEtaPlusPhiZero","looseMuLowerPlusValidHitsEtaPlusPhiZero",
								   100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusValidHitsEtaPlusPhiPlus","looseMuLowerPlusValidHitsEtaPlusPhiPlus",
								   100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusValidHitsEtaMinusPhiMinus","looseMuLowerPlusValidHitsEtaMinusPhiMinus",
								     100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaMinusPhiZero      = new TH1F("looseMuLowerPlusValidHitsEtaMinusPhiZero", "looseMuLowerPlusValidHitsEtaMinusPhiZero",
								    100, -0.5, 99.5);
  TH1F *h_looseMuLowerPlusValidHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusValidHitsEtaMinusPhiPlus", "looseMuLowerPlusValidHitsEtaMinusPhiPlus",
								    100, -0.5, 99.5);



  TH1F *h_looseMuMinusValidMuonHits      = new TH1F("looseMuMinusValidMuonHits",     "looseMuMinusValidMuonHits",      75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaPlus      = new TH1F("looseMuMinusValidMuonHitsEtaPlus","looseMuMinusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaMinus      = new TH1F("looseMuMinusValidMuonHitsEtaMinus","looseMuMinusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaPlusPhiMinus      = new TH1F("looseMuMinusValidMuonHitsEtaPlusPhiMinus","looseMuMinusValidMuonHitsEtaPlusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaPlusPhiZero      = new TH1F("looseMuMinusValidMuonHitsEtaPlusPhiZero","looseMuMinusValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaPlusPhiPlus      = new TH1F("looseMuMinusValidMuonHitsEtaPlusPhiPlus","looseMuMinusValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaMinusPhiMinus      = new TH1F("looseMuMinusValidMuonHitsEtaMinusPhiMinus","looseMuMinusValidMuonHitsEtaMinusPhiMinus",
								    75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaMinusPhiZero      = new TH1F("looseMuMinusValidMuonHitsEtaMinusPhiZero", "looseMuMinusValidMuonHitsEtaMinusPhiZero",
								   75, -0.5, 74.5);
  TH1F *h_looseMuMinusValidMuonHitsEtaMinusPhiPlus      = new TH1F("looseMuMinusValidMuonHitsEtaMinusPhiPlus", "looseMuMinusValidMuonHitsEtaMinusPhiPlus",
								   75, -0.5, 74.5);

  TH1F *h_looseMuPlusValidMuonHits       = new TH1F("looseMuPlusValidMuonHits",      "looseMuPlusValidMuonHits",       75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaPlus      = new TH1F("looseMuPlusValidMuonHitsEtaPlus","looseMuPlusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaMinus      = new TH1F("looseMuPlusValidMuonHitsEtaMinus","looseMuPlusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaPlusPhiMinus      = new TH1F("looseMuPlusValidMuonHitsEtaPlusPhiMinus","looseMuPlusValidMuonHitsEtaPlusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaPlusPhiZero      = new TH1F("looseMuPlusValidMuonHitsEtaPlusPhiZero","looseMuPlusValidMuonHitsEtaPlusPhiZero", 75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaPlusPhiPlus      = new TH1F("looseMuPlusValidMuonHitsEtaPlusPhiPlus","looseMuPlusValidMuonHitsEtaPlusPhiPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaMinusPhiMinus      = new TH1F("looseMuPlusValidMuonHitsEtaMinusPhiMinus","looseMuPlusValidMuonHitsEtaMinusPhiMinus",
								    75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaMinusPhiZero      = new TH1F("looseMuPlusValidMuonHitsEtaMinusPhiZero", "looseMuPlusValidMuonHitsEtaMinusPhiZero",
								   75, -0.5, 74.5);
  TH1F *h_looseMuPlusValidMuonHitsEtaMinusPhiPlus      = new TH1F("looseMuPlusValidMuonHitsEtaMinusPhiPlus", "looseMuPlusValidMuonHitsEtaMinusPhiPlus",
								   75, -0.5, 74.5);

  TH1F *h_looseMuUpperMinusValidMuonHits = new TH1F("looseMuUpperMinusValidMuonHits","looseMuUpperMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaPlus      = new TH1F("looseMuUpperMinusValidMuonHitsEtaPlus","looseMuUpperMinusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaMinus      = new TH1F("looseMuUpperMinusValidMuonHitsEtaMinus","looseMuUpperMinusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusValidMuonHitsEtaPlusPhiMinus",
									"looseMuUpperMinusValidMuonHitsEtaPlusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaPlusPhiZero      = new TH1F("looseMuUpperMinusValidMuonHitsEtaPlusPhiZero","looseMuUpperMinusValidMuonHitsEtaPlusPhiZero",
								       75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusValidMuonHitsEtaPlusPhiPlus","looseMuUpperMinusValidMuonHitsEtaPlusPhiPlus",
								       75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusValidMuonHitsEtaMinusPhiMinus",
									 "looseMuUpperMinusValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaMinusPhiZero      = new TH1F("looseMuUpperMinusValidMuonHitsEtaMinusPhiZero",
									"looseMuUpperMinusValidMuonHitsEtaMinusPhiZero", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperMinusValidMuonHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusValidMuonHitsEtaMinusPhiPlus",
									"looseMuUpperMinusValidMuonHitsEtaMinusPhiPlus", 75, -0.5, 74.5);


  TH1F *h_looseMuUpperPlusValidMuonHits  = new TH1F("looseMuUpperPlusValidMuonHits", "looseMuUpperPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaPlus      = new TH1F("looseMuUpperPlusValidMuonHitsEtaPlus","looseMuUpperPlusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaMinus      = new TH1F("looseMuUpperPlusValidMuonHitsEtaMinus","looseMuUpperPlusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusValidMuonHitsEtaPlusPhiMinus","looseMuUpperPlusValidMuonHitsEtaPlusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaPlusPhiZero      = new TH1F("looseMuUpperPlusValidMuonHitsEtaPlusPhiZero","looseMuUpperPlusValidMuonHitsEtaPlusPhiZero",
								      75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusValidMuonHitsEtaPlusPhiPlus","looseMuUpperPlusValidMuonHitsEtaPlusPhiPlus",
								      75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusValidMuonHitsEtaMinusPhiMinus",
									"looseMuUpperPlusValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaMinusPhiZero      = new TH1F("looseMuUpperPlusValidMuonHitsEtaMinusPhiZero", "looseMuUpperPlusValidMuonHitsEtaMinusPhiZero",
								   75, -0.5, 74.5);
  TH1F *h_looseMuUpperPlusValidMuonHitsEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusValidMuonHitsEtaMinusPhiPlus", "looseMuUpperPlusValidMuonHitsEtaMinusPhiPlus",
								   75, -0.5, 74.5);

  TH1F *h_looseMuLowerMinusValidMuonHits = new TH1F("looseMuLowerMinusValidMuonHits","looseMuLowerMinusValidMuonHits", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaPlus      = new TH1F("looseMuLowerMinusValidMuonHitsEtaPlus","looseMuLowerMinusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaMinus      = new TH1F("looseMuLowerMinusValidMuonHitsEtaMinus","looseMuLowerMinusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusValidMuonHitsEtaPlusPhiMinus",
									"looseMuLowerMinusValidMuonHitsEtaPlusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaPlusPhiZero      = new TH1F("looseMuLowerMinusValidMuonHitsEtaPlusPhiZero","looseMuLowerMinusValidMuonHitsEtaPlusPhiZero",
								       75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusValidMuonHitsEtaPlusPhiPlus","looseMuLowerMinusValidMuonHitsEtaPlusPhiPlus",
								       75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusValidMuonHitsEtaMinusPhiMinus",
									 "looseMuLowerMinusValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaMinusPhiZero      = new TH1F("looseMuLowerMinusValidMuonHitsEtaMinusPhiZero",
									"looseMuLowerMinusValidMuonHitsEtaMinusPhiZero",75, -0.5, 74.5);
  TH1F *h_looseMuLowerMinusValidMuonHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusValidMuonHitsEtaMinusPhiPlus",
									"looseMuLowerMinusValidMuonHitsEtaMinusPhiPlus", 75, -0.5, 74.5);

  TH1F *h_looseMuLowerPlusValidMuonHits  = new TH1F("looseMuLowerPlusValidMuonHits", "looseMuLowerPlusValidMuonHits",  75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaPlus      = new TH1F("looseMuLowerPlusValidMuonHitsEtaPlus","looseMuLowerPlusValidMuonHitsEtaPlus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaMinus      = new TH1F("looseMuLowerPlusValidMuonHitsEtaMinus","looseMuLowerPlusValidMuonHitsEtaMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusValidMuonHitsEtaPlusPhiMinus","looseMuLowerPlusValidMuonHitsEtaPlusPhiMinus",
								   75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaPlusPhiZero      = new TH1F("looseMuLowerPlusValidMuonHitsEtaPlusPhiZero","looseMuLowerPlusValidMuonHitsEtaPlusPhiZero",
								      75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusValidMuonHitsEtaPlusPhiPlus","looseMuLowerPlusValidMuonHitsEtaPlusPhiPlus",
								      75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusValidMuonHitsEtaMinusPhiMinus",
									"looseMuLowerPlusValidMuonHitsEtaMinusPhiMinus", 75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaMinusPhiZero      = new TH1F("looseMuLowerPlusValidMuonHitsEtaMinusPhiZero", "looseMuLowerPlusValidMuonHitsEtaMinusPhiZero",
								   75, -0.5, 74.5);
  TH1F *h_looseMuLowerPlusValidMuonHitsEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusValidMuonHitsEtaMinusPhiPlus", "looseMuLowerPlusValidMuonHitsEtaMinusPhiPlus",
								   75, -0.5, 74.5);

  TH1F *h_looseMuMinusMatchedMuonStations      = new TH1F("looseMuMinusMatchedMuonStations",     "looseMuMinusMatchedMuonStations",      10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaPlus      = new TH1F("looseMuMinusMatchedMuonStationsEtaPlus",
									  "looseMuMinusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaMinus      = new TH1F("looseMuMinusMatchedMuonStationsEtaMinus",
									  "looseMuMinusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("looseMuMinusMatchedMuonStationsEtaPlusPhiMinus",
									  "looseMuMinusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("looseMuMinusMatchedMuonStationsEtaPlusPhiZero",
									  "looseMuMinusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("looseMuMinusMatchedMuonStationsEtaPlusPhiPlus",
									  "looseMuMinusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("looseMuMinusMatchedMuonStationsEtaMinusPhiMinus",
									  "looseMuMinusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("looseMuMinusMatchedMuonStationsEtaMinusPhiZero",
									  "looseMuMinusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuMinusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("looseMuMinusMatchedMuonStationsEtaMinusPhiPlus",
									  "looseMuMinusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuPlusMatchedMuonStations       = new TH1F("looseMuPlusMatchedMuonStations",      "looseMuPlusMatchedMuonStations",       10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaPlus      = new TH1F("looseMuPlusMatchedMuonStationsEtaPlus",
									  "looseMuPlusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaMinus      = new TH1F("looseMuPlusMatchedMuonStationsEtaMinus",
									  "looseMuPlusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("looseMuPlusMatchedMuonStationsEtaPlusPhiMinus",
									  "looseMuPlusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("looseMuPlusMatchedMuonStationsEtaPlusPhiZero",
									  "looseMuPlusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("looseMuPlusMatchedMuonStationsEtaPlusPhiPlus",
									  "looseMuPlusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("looseMuPlusMatchedMuonStationsEtaMinusPhiMinus",
									  "looseMuPlusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("looseMuPlusMatchedMuonStationsEtaMinusPhiZero",
									  "looseMuPlusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuPlusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("looseMuPlusMatchedMuonStationsEtaMinusPhiPlus",
									  "looseMuPlusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuUpperMinusMatchedMuonStations = new TH1F("looseMuUpperMinusMatchedMuonStations","looseMuUpperMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaPlus      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaPlus",
									  "looseMuUpperMinusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaMinus      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaMinus",
									  "looseMuUpperMinusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaPlusPhiMinus",
									  "looseMuUpperMinusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaPlusPhiZero",
									  "looseMuUpperMinusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaPlusPhiPlus",
									  "looseMuUpperMinusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaMinusPhiMinus",
									  "looseMuUpperMinusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaMinusPhiZero",
									  "looseMuUpperMinusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperMinusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusMatchedMuonStationsEtaMinusPhiPlus",
									  "looseMuUpperMinusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuUpperPlusMatchedMuonStations  = new TH1F("looseMuUpperPlusMatchedMuonStations", "looseMuUpperPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaPlus      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaPlus",
									  "looseMuUpperPlusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaMinus      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaMinus",
									  "looseMuUpperPlusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaPlusPhiMinus",
									  "looseMuUpperPlusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaPlusPhiZero",
									  "looseMuUpperPlusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaPlusPhiPlus",
									  "looseMuUpperPlusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaMinusPhiMinus",
									  "looseMuUpperPlusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaMinusPhiZero",
									  "looseMuUpperPlusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuUpperPlusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusMatchedMuonStationsEtaMinusPhiPlus",
									  "looseMuUpperPlusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);

  TH1F *h_looseMuLowerMinusMatchedMuonStations = new TH1F("looseMuLowerMinusMatchedMuonStations","looseMuLowerMinusMatchedMuonStations", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaPlus      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaPlus",
									  "looseMuLowerMinusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaMinus      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaMinus",
									  "looseMuLowerMinusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaPlusPhiMinus",
									  "looseMuLowerMinusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaPlusPhiZero",
									  "looseMuLowerMinusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaPlusPhiPlus",
									  "looseMuLowerMinusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaMinusPhiMinus",
									  "looseMuLowerMinusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaMinusPhiZero",
									  "looseMuLowerMinusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerMinusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusMatchedMuonStationsEtaMinusPhiPlus",
									  "looseMuLowerMinusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);


  TH1F *h_looseMuLowerPlusMatchedMuonStations  = new TH1F("looseMuLowerPlusMatchedMuonStations", "looseMuLowerPlusMatchedMuonStations",  10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaPlus      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaPlus",
									  "looseMuLowerPlusMatchedMuonStationsEtaPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaMinus      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaMinus",
									  "looseMuLowerPlusMatchedMuonStationsEtaMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaPlusPhiMinus",
									  "looseMuLowerPlusMatchedMuonStationsEtaPlusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaPlusPhiZero      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaPlusPhiZero",
									  "looseMuLowerPlusMatchedMuonStationsEtaPlusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaPlusPhiPlus",
									  "looseMuLowerPlusMatchedMuonStationsEtaPlusPhiPlus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaMinusPhiMinus",
									  "looseMuLowerPlusMatchedMuonStationsEtaMinusPhiMinus", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaMinusPhiZero      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaMinusPhiZero",
									  "looseMuLowerPlusMatchedMuonStationsEtaMinusPhiZero", 10, -0.5, 9.5);
  TH1F *h_looseMuLowerPlusMatchedMuonStationsEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusMatchedMuonStationsEtaMinusPhiPlus",
									  "looseMuLowerPlusMatchedMuonStationsEtaMinusPhiPlus", 10, -0.5, 9.5);





  TH1F *h_looseMuMinusTrackerLayersWithMeasurement      = new TH1F("looseMuMinusTrackerLayersWithMeasurement",     "looseMuMinusTrackerLayersWithMeasurement",
								   20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaPlus      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaPlus",
									  "looseMuMinusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaMinus      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaMinus",
									  "looseMuMinusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									  "looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									  "looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiZero",
									  "looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									  "looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);



  TH1F *h_looseMuPlusTrackerLayersWithMeasurement       = new TH1F("looseMuPlusTrackerLayersWithMeasurement",      "looseMuPlusTrackerLayersWithMeasurement",
								   20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaPlus      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaPlus",
									  "looseMuPlusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaMinus      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaMinus",
									  "looseMuPlusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									  "looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									  "looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiZero",
									  "looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									  "looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurement = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurement","looseMuUpperMinusTrackerLayersWithMeasurement",
								   20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlus      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaPlus",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinus      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaMinus",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									  "looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurement  = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurement", "looseMuUpperPlusTrackerLayersWithMeasurement", 
								   20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlus      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaPlus",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinus      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaMinus",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									  "looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurement = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurement","looseMuLowerMinusTrackerLayersWithMeasurement",
								   20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlus      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaPlus",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinus      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaMinus",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									  "looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);


  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurement  = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurement", "looseMuLowerPlusTrackerLayersWithMeasurement",
								   20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlus      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaPlus",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinus      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaMinus",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero", 20, -0.5, 19.5);
  TH1F *h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus      = new TH1F("looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus",
									  "looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus", 20, -0.5, 19.5);



  TH1F *h_looseMuMinusCurvePlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPhiMinusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPhiZeroPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPhiPlusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPhiMinusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPhiZeroPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPhiPlusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaAllPhiMinusPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaAllPhiZeroPlusBias[      nBiasBins];
  TH1F *h_looseMuMinusCurveEtaAllPhiPlusPlusBias[      nBiasBins];
  
  TH1F *h_looseMuMinusCurveMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPhiMinusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPhiZeroMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaPlusPhiPlusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPhiMinusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPhiZeroMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaMinusPhiPlusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaAllPhiMinusMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaAllPhiZeroMinusBias[     nBiasBins];
  TH1F *h_looseMuMinusCurveEtaAllPhiPlusMinusBias[     nBiasBins];


  TH1F *h_looseMuPlusCurvePlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPhiMinusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPhiZeroPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPhiPlusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPhiMinusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPhiZeroPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPhiPlusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaAllPhiMinusPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaAllPhiZeroPlusBias[       nBiasBins];
  TH1F *h_looseMuPlusCurveEtaAllPhiPlusPlusBias[       nBiasBins];



  TH1F *h_looseMuPlusCurveMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPhiMinusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPhiZeroMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaPlusPhiPlusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPhiMinusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPhiZeroMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaMinusPhiPlusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaAllPhiMinusMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaAllPhiZeroMinusBias[      nBiasBins];
  TH1F *h_looseMuPlusCurveEtaAllPhiPlusMinusBias[      nBiasBins];



  TH1F *h_looseMuUpperMinusCurvePlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaPlusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiZeroPlusBias[ nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiPlusPlusBias[ nBiasBins];


  TH1F *h_looseMuUpperMinusCurveMinusBias[nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaPlusMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaMinusMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiMinusMinusBias[nBiasBins];
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiZeroMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaPlusPhiPlusMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiMinusMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiZeroMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaMinusPhiPlusMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiMinusMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiZeroMinusBias[nBiasBins];  
  TH1F *h_looseMuUpperMinusCurveEtaAllPhiPlusMinusBias[nBiasBins];  


  TH1F *h_looseMuUpperPlusCurvePlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiZeroPlusBias[  nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiPlusPlusBias[  nBiasBins];

  TH1F *h_looseMuUpperPlusCurveMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaPlusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaMinusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiZeroMinusBias[ nBiasBins];
  TH1F *h_looseMuUpperPlusCurveEtaAllPhiPlusMinusBias[ nBiasBins];


  TH1F *h_looseMuLowerMinusCurvePlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiZeroPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiPlusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiMinusPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiZeroPlusBias[ nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiPlusPlusBias[ nBiasBins];


  TH1F *h_looseMuLowerMinusCurveMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiMinusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiZeroMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaPlusPhiPlusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiMinusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiZeroMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaMinusPhiPlusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiMinusMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiZeroMinusBias[nBiasBins];
  TH1F *h_looseMuLowerMinusCurveEtaAllPhiPlusMinusBias[nBiasBins];


  TH1F *h_looseMuLowerPlusCurvePlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiZeroPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiPlusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiMinusPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiZeroPlusBias[  nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiPlusPlusBias[  nBiasBins];


  TH1F *h_looseMuLowerPlusCurveMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaPlusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiZeroMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaMinusPhiPlusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiMinusMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiZeroMinusBias[ nBiasBins];
  TH1F *h_looseMuLowerPlusCurveEtaAllPhiPlusMinusBias[ nBiasBins];


  for (int i =0; i < nBiasBins; ++i) {
    std::stringstream name;
    name << std::setw(3) << std::setfill('0') << i + 1;
    std::stringstream ptitle;
    ptitle << "#Delta#kappa = +" << (i+1)*(factor_*maxBias/nBiasBins);
    std::stringstream mtitle;
    mtitle << "#Delta#kappa = -" << (i+1)*(factor_*maxBias/nBiasBins);
    h_looseMuMinusCurvePlusBias[i]       = new TH1F(TString("looseMuMinusCurvePlusBias"       + name.str()),
						    TString("looseMuMinusCurvePlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaPlusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaPlusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaPlusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaMinusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaMinusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaMinusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaPlusPhiMinusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaPlusPhiMinusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaPlusPhiMinusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaPlusPhiZeroPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaPlusPhiZeroPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaPlusPhiZeroPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaPlusPhiPlusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaPlusPhiPlusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaPlusPhiPlusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaMinusPhiMinusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaMinusPhiMinusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaMinusPhiMinusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaMinusPhiZeroPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaMinusPhiZeroPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaMinusPhiZeroPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaMinusPhiPlusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaMinusPhiPlusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaMinusPhiPlusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaAllPhiMinusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaAllPhiMinusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaAllPhiMinusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaAllPhiZeroPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaAllPhiZeroPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaAllPhiZeroPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuMinusCurveEtaAllPhiPlusPlusBias[i]       = new TH1F(TString("looseMuMinusCurveEtaAllPhiPlusPlusBias"       + name.str()),
						    TString("looseMuMinusCurveEtaAllPhiPlusPlusBias"       + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



    h_looseMuMinusCurveMinusBias[i]      = new TH1F(TString("looseMuMinusCurveMinusBias"      + name.str()),
						    TString("looseMuMinusCurveMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaPlusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaPlusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaPlusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaMinusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaMinusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaMinusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaPlusPhiMinusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaPlusPhiMinusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaPlusPhiMinusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaPlusPhiZeroMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaPlusPhiZeroMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaPlusPhiZeroMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaPlusPhiPlusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaPlusPhiPlusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaPlusPhiPlusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaMinusPhiMinusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaMinusPhiMinusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaMinusPhiMinusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaMinusPhiZeroMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaMinusPhiZeroMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaMinusPhiZeroMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaMinusPhiPlusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaMinusPhiPlusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaMinusPhiMinusPlusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaAllPhiMinusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaAllPhiMinusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaAllPhiMinusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaAllPhiZeroMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaAllPhiZeroMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaAllPhiZeroMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuMinusCurveEtaAllPhiPlusMinusBias[i]      = new TH1F(TString("looseMuMinusCurveEtaAllPhiPlusMinusBias"      + name.str()),
						    TString("looseMuMinusCurveEtaAllPhiPlusMinusBias"      + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  



    h_looseMuPlusCurvePlusBias[i]        = new TH1F(TString("looseMuPlusCurvePlusBias"        + name.str()),
						    TString("looseMuPlusCurvePlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaPlusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaPlusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaPlusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaMinusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaMinusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaMinusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaPlusPhiMinusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaPlusPhiMinusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaPlusPhiMinusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaPlusPhiZeroPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaPlusPhiZeroPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaPlusPhiZeroPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaPlusPhiPlusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaPlusPhiPlusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaPlusPhiPlusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaMinusPhiMinusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaMinusPhiMinusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaMinusPhiMinusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaMinusPhiZeroPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaMinusPhiZeroPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaMinusPhiZeroPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaMinusPhiPlusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaMinusPhiPlusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaMinusPhiPlusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaAllPhiMinusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaAllPhiMinusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaAllPhiMinusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaAllPhiZeroPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaAllPhiZeroPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaAllPhiZeroPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuPlusCurveEtaAllPhiPlusPlusBias[i]        = new TH1F(TString("looseMuPlusCurveEtaAllPhiPlusPlusBias"        + name.str()),
						    TString("looseMuPlusCurveEtaAllPhiPlusPlusBias"        + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



    h_looseMuPlusCurveMinusBias[i]       = new TH1F(TString("looseMuPlusCurveMinusBias"       + name.str()),
						    TString("looseMuPlusCurveMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaPlusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaPlusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaPlusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaMinusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaMinusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaMinusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaPlusPhiMinusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaPlusPhiMinusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaPlusPhiMinusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaPlusPhiZeroMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaPlusPhiZeroMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaPlusPhiZeroMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaPlusPhiPlusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaPlusPhiPlusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaPlusPhiPlusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaMinusPhiMinusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaMinusPhiMinusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaMinusPhiMinusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaMinusPhiZeroMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaMinusPhiZeroMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaMinusPhiZeroMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaMinusPhiPlusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaMinusPhiPlusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaMinusPhiPlusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaAllPhiMinusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaAllPhiMinusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaAllPhiMinusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaAllPhiZeroMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaAllPhiZeroMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaAllPhiZeroMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuPlusCurveEtaAllPhiMinusMinusBias[i]       = new TH1F(TString("looseMuPlusCurveEtaAllPhiPlusMinusBias"       + name.str()),
						    TString("looseMuPlusCurveEtaAllPhiPlusMinusBias"       + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  


    h_looseMuUpperMinusCurvePlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurvePlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurvePlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaPlusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaMinusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaPlusPhiMinusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPhiMinusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPhiMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaPlusPhiZeroPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPhiZeroPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPhiZeroPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaPlusPhiPlusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPhiPlusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPhiPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaMinusPhiMinusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPhiMinusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusPhiMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaMinusPhiZeroPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPhiZeroPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusPhiZeroPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaMinusPhiPlusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPhiPlusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusPhiPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaAllPhiMinusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaAllPhiMinusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaAllPhiMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaAllPhiZeroPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaAllPhiZeroPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaAllPhiZeroPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaAllPhiPlusPlusBias[i]  = new TH1F(TString("looseMuUpperMinusCurveEtaAllPhiPlusPlusBias"  + name.str()),
						    TString("looseMuUpperMinusCurveEtaAllPhiPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



    h_looseMuUpperMinusCurveMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaPlusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaPlusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaMinusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaMinusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaPlusPhiMinusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPhiMinusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPhiMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaPlusPhiZeroMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPhiZeroMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPhiZeroMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaPlusPhiPlusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaPlusPhiPlusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaPlusPhiPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaMinusPhiMinusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPhiMinusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusPhiMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperMinusCurveEtaMinusPhiZeroMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPhiZerpMinusBias" + name.str()),
								   TString("looseMuUpperMinusCurveEtaMinusPhiZeroMinusBias" + mtitle.str()),
								   symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaMinusPhiPlusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaMinusPhiPlusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaMinusPhiPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaAllPhiMinusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaAllPhiMinusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaAllPhiMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaAllPhiZeroMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaAllPhiZeroMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaAllPhiZeroMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperMinusCurveEtaAllPhiPlusMinusBias[i] = new TH1F(TString("looseMuUpperMinusCurveEtaAllPhiPlusMinusBias" + name.str()),
						    TString("looseMuUpperMinusCurveEtaAllPhiPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  




    h_looseMuUpperPlusCurvePlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurvePlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurvePlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaPlusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaMinusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaPlusPhiMinusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPhiMinusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPhiMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaPlusPhiZeroPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPhiZeroPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPhiZeroPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaPlusPhiPlusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPhiPlusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPhiPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaMinusPhiMinusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPhiMinusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPhiMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaMinusPhiZeroPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPhiZeroPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPhiZeroPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaMinusPhiPlusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPhiPlusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPhiPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaAllPhiMinusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaAllPhiMinusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaAllPhiMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaAllPhiZeroPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaAllPhiZeroPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaAllPhiZeroPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuUpperPlusCurveEtaAllPhiPlusPlusBias[i]   = new TH1F(TString("looseMuUpperPlusCurveEtaAllPhiPlusPlusBias"   + name.str()),
						    TString("looseMuUpperPlusCurveEtaAllPhiPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


    h_looseMuUpperPlusCurveMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaPlusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaPlusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaMinusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaMinusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaPlusPhiMinusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPhiMinusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPhiMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaPlusPhiZeroMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPhiZeroMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPhiZeroMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaPlusPhiPlusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaPlusPhiPlusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaPlusPhiPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaMinusPhiMinusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPhiMinusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPhiMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaMinusPhiZeroMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPhiZeroMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPhiZeroMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaMinusPhiPlusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaMinusPhiPlusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaMinusPhiPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaAllPhiMinusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaAllPhiMinusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaAllPhiMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaAllPhiZeroMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaAllPhiZeroMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaAllPhiZeroMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuUpperPlusCurveEtaAllPhiPlusMinusBias[i]  = new TH1F(TString("looseMuUpperPlusCurveEtaAllPhiPlusMinusBias"  + name.str()),
						    TString("looseMuUpperPlusCurveEtaAllPhiPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  



    h_looseMuLowerMinusCurvePlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurvePlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurvePlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaPlusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaMinusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaPlusPhiMinusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPhiMinusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPhiMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaPlusPhiZeroPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPhiZeroPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPhiZeroPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaPlusPhiPlusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPhiPlusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPhiPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaMinusPhiMinusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPhiMinusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPhiMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaMinusPhiZeroPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPhiZeroPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPhiZeroPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaMinusPhiPlusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPhiPlusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPhiPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaAllPhiMinusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaAllPhiMinusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaAllPhiMinusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaAllPhiZeroPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaAllPhiZeroPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaAllPhiZeroPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerMinusCurveEtaAllPhiPlusPlusBias[i]  = new TH1F(TString("looseMuLowerMinusCurveEtaAllPhiPlusPlusBias"  + name.str()),
						    TString("looseMuLowerMinusCurveEtaAllPhiPlusPlusBias"  + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);


    h_looseMuLowerMinusCurveMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaPlusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaPlusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaMinusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaMinusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaPlusPhiMinusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPhiMinusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPhiMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaPlusPhiZeroMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPhiZeroMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPhiZeroMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaPlusPhiPlusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaPlusPhiPlusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaPlusPhiPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaMinusPhiMinusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPhiMinusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPhiMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaMinusPhiZeroMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPhiZeroMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPhiZeroMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaMinusPhiPlusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaMinusPhiPlusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaMinusPhiPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaAllPhiMinusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaAllPhiMinusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaAllPhiMinusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaAllPhiZeroMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaAllPhiZeroMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaAllPhiZeroMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerMinusCurveEtaAllPhiPlusMinusBias[i] = new TH1F(TString("looseMuLowerMinusCurveEtaAllPhiPlusMinusBias" + name.str()),
						    TString("looseMuLowerMinusCurveEtaAllPhiPlusMinusBias" + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  


    h_looseMuLowerPlusCurvePlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurvePlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurvePlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaPlusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaMinusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPlusBias"   + name.str()),
							   TString("looseMuLowerPlusCurveEtaMinusPlusBias"   + ptitle.str()),
							   symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaPlusPhiMinusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPhiMinusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPhiMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaPlusPhiZeroPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPhiZeroPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPhiZeroPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaPlusPhiPlusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPhiPlusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPhiPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaMinusPhiMinusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPhiMinusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusPhiMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaMinusPhiZeroPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPhiZeroPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusPhiZeroPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaMinusPhiPlusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPhiPlusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusPhiPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaAllPhiMinusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaAllPhiMinusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaAllPhiMinusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaAllPhiZeroPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaAllPhiZeroPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaAllPhiZeroPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);
    h_looseMuLowerPlusCurveEtaAllPhiPlusPlusBias[i]   = new TH1F(TString("looseMuLowerPlusCurveEtaAllPhiPlusPlusBias"   + name.str()),
						    TString("looseMuLowerPlusCurveEtaAllPhiPlusPlusBias"   + ptitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);



    h_looseMuLowerPlusCurveMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaPlusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaPlusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaMinusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaMinusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaPlusPhiMinusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPhiMinusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPhiMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaPlusPhiZeroMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPhiZeroMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPhiZeroMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaPlusPhiPlusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaPlusPhiPlusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaPlusPhiPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaMinusPhiMinusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPhiMinusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusPhiMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaMinusPhiZeroMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPhiZeroMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusPhiZeroMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaMinusPhiPlusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaMinusPhiPlusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaMinusPhiPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaAllPhiMinusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaAllPhiMinusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaAllPhiMinusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaAllPhiZeroMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaAllPhiZeroMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaAllPhiZeroMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  
    h_looseMuLowerPlusCurveEtaAllPhiPlusMinusBias[i]  = new TH1F(TString("looseMuLowerPlusCurveEtaAllPhiPlusMinusBias"  + name.str()),
						    TString("looseMuLowerPlusCurveEtaAllPhiPlusMinusBias"  + mtitle.str()),
						    symmetric_ ? 1600 : 800, symmetric_ ? -0.008*factor_ : 0., 0.008*factor_);  


  }


  std::cout << "Creating upper muMinus TTreeReaderValues" << std::endl;
  TTreeReaderValue<Int_t>    run(  trackReader, "muonRunNumber"  );
  TTreeReaderValue<Int_t>    lumi( trackReader, "muonLumiBlock"  );
  TTreeReaderValue<Int_t>    event(trackReader, "muonEventNumber");

  TTreeReaderValue<Int_t> trueL1SingleMu(trackReader, "l1SingleMu");
  TTreeReaderValue<Int_t> fakeL1SingleMu(trackReader, "fakeL1SingleMu");
  TTreeReaderValue<Int_t> nSimTracks( trackReader,    "nSimTracks");
  TTreeReaderArray<double> simTrackpT(trackReader,    "simTrackpT" );

  
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
  TTreeReaderValue<Int_t> upTrackerValidMuonHits(        trackReader, "upperMuon_numberOfValidHits"       );//Temporarily changed from ValidMuonHits to Valid Hits
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
  TTreeReaderValue<Int_t>    lowTrackerValidMuonHits(        trackReader, "lowerMuon_numberOfValidHits"       );//Temporarily changed from ValidMuonHits to Valid Hits
  TTreeReaderValue<Int_t>    lowTrackerMatchedMuonStations(  trackReader, "lowerMuon_numberOfMatchedStations"     );
  TTreeReaderValue<Int_t>    lowTrackerLayersWithMeasurement(trackReader, "lowerMuon_trackerLayersWithMeasurement");
  
  std::cout << "Made it to Histogramming!" << std::endl; 
  int j = 0;

  //double maxDR = 0.15; // what is reasonable here? Aachen did dPhi < 0.1, dTheta (eta?) < 0.05

  while (trackReader.Next()) {

    if (debug)
      std::cout << "Made it into the first loop" << std::endl;
    g->cd();
    
    // apply the trigger, i.e., don't process if the trigger didn't fire
    if (applyTrigger_ && !(*fakeL1SingleMu))
      continue;

    // make combination of samples easy
    if (mcFlag_) {
      if (*nSimTracks > 0) {
	if ((simTrackpT[0] >= highpT_) || (simTrackpT[0] < lowpT_))
	  continue;
      }
    }
    
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
      
	bool up_etabar   = (fabs(upTrackerMuonP4->eta()) < 0.9) ? 1 : 0;
	bool up_tightdxy = (*upTrackerDxy < 0.2) ? 1 : 0;
	bool up_tightdz  = (*upTrackerDz  < 0.5) ? 1 : 0;
	bool up_etaBar   = (fabs(upTrackerMuonP4->eta()) < 0.9) ? 1 : 0;
	bool up_superpointing = ((std::fabs(*upTrackerDxy) < 10) && (std::fabs(*upTrackerDz)  < 50))
	  ? 1 : 0;

	// if using TuneP or TrackerOnly and pT < 200, should *not* apply muon system cuts
	// bool upperMuStationHits = (!istrackerp || (istunep && sqrt(upTrackerTrack->perp2()) > 200)) ? *upTrackerMatchedMuonStations > 1 : 1;
	bool upperMuStationHits = *upTrackerMatchedMuonStations > 1;
	bool upperValidMuHits   = (!istrackerp || (istunep && sqrt(upTrackerTrack->perp2()) > 200)) ? *upTrackerValidMuonHits > 0 : 1;

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
	
	h_upperChi2->Fill(  *upTrackerChi2);
	h_upperNdof->Fill(  *upTrackerNdof);
	h_upperCharge->Fill(*upTrackerCharge);
	h_upperCurve->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	if (upTrackerTrack->eta() > 0){
	  h_upperChi2EtaPlus->Fill(  *upTrackerChi2);
	  h_upperNdofEtaPlus->Fill(  *upTrackerNdof);
	  h_upperChargeEtaPlus->Fill(*upTrackerCharge);
	  h_upperCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	  h_upperDxyEtaPlus->Fill(     *upTrackerDxy);
	  h_upperDzEtaPlus->Fill(      *upTrackerDz);
	  h_upperDxyErrorEtaPlus->Fill(*upTrackerDxyError);
	  h_upperDzErrorEtaPlus->Fill( *upTrackerDzError);
	  
	  h_upperPtEtaPlus->Fill( upTrackerMuonP4->pt());
	  h_upperEtaEtaPlus->Fill(upTrackerMuonP4->eta());
	  h_upperPhiEtaPlus->Fill(upTrackerMuonP4->phi());
	  
	  h_upperPtErrorEtaPlus->Fill( *upTrackerPtError);
	  h_upperPtRelErrEtaPlus->Fill( upperRelPtErr);
	  h_upperTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	  h_upperTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
	  h_upperTrackPhiEtaPlus->Fill( upTrackerTrack->phi());

	  h_upperPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
	  h_upperTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
	  h_upperValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
	  h_upperValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
	  h_upperMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
	  h_upperMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
	  h_upperTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);
	  
	  if(upTrackerTrack->phi() < -1.0472){
	    h_upperChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
	    h_upperNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
	    h_upperChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
	    h_upperCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    h_upperDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
	    h_upperDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
	    h_upperDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
	    h_upperDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
	    
	    h_upperPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
	    h_upperEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
	    h_upperPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
	    
	    h_upperPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
	    h_upperPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
	    h_upperTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	    h_upperTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
	    h_upperTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
	    
	    h_upperPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
	    h_upperTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
	    h_upperValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
	    h_upperValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
	    h_upperMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
	    h_upperMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
	    h_upperTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	    

	  }
	  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	    h_upperChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
	    h_upperNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
	    h_upperChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
	    h_upperCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    h_upperDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
	    h_upperDzEtaPlusPhiZero->Fill(      *upTrackerDz);
	    h_upperDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
	    h_upperDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
	    
	    h_upperPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
	    h_upperEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
	    h_upperPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
	    
	    h_upperPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
	    h_upperPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
	    h_upperTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
	    h_upperTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
	    h_upperTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
	    
	    h_upperPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
	    h_upperTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
	    h_upperValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
	    h_upperValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
	    h_upperMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
	    h_upperMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
	    h_upperTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);
	 	   

	  }
	  else if(upTrackerTrack->phi() > 1.0472){
	    h_upperChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
	    h_upperNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
	    h_upperChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
	    h_upperCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    h_upperDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
	    h_upperDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
	    h_upperDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
	    h_upperDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
	    
	    h_upperPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
	    h_upperEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
	    h_upperPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
	    
	    h_upperPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
	    h_upperPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
	    h_upperTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	    h_upperTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
	    h_upperTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
	    
	    h_upperPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
	    h_upperTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
	    h_upperValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
	    h_upperValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
	    h_upperMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
	    h_upperMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
	    h_upperTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	  }
	  
	  
	}
	

	else if (upTrackerTrack->eta() < 0){
	  h_upperChi2EtaMinus->Fill(  *upTrackerChi2);
	  h_upperNdofEtaMinus->Fill(  *upTrackerNdof);
	  h_upperChargeEtaMinus->Fill(*upTrackerCharge);
	  h_upperCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	  h_upperDxyEtaMinus->Fill(     *upTrackerDxy);
	  h_upperDzEtaMinus->Fill(      *upTrackerDz);
	  h_upperDxyErrorEtaMinus->Fill(*upTrackerDxyError);
	  h_upperDzErrorEtaMinus->Fill( *upTrackerDzError);
	  
	  h_upperPtEtaMinus->Fill( upTrackerMuonP4->pt());
	  h_upperEtaEtaMinus->Fill(upTrackerMuonP4->eta());
	  h_upperPhiEtaMinus->Fill(upTrackerMuonP4->phi());
	  
	  h_upperPtErrorEtaMinus->Fill( *upTrackerPtError);
	  h_upperPtRelErrEtaMinus->Fill( upperRelPtErr);
	  h_upperTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	  h_upperTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
	  h_upperTrackPhiEtaMinus->Fill( upTrackerTrack->phi());

	  h_upperPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
	  h_upperTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
	  h_upperValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
	  h_upperValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
	  h_upperMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
	  h_upperMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
	  h_upperTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);


	  if(upTrackerTrack->phi() < -1.0472){
	    
	    h_upperChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
	    h_upperNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
	    h_upperChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
	    h_upperCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    h_upperDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
	    h_upperDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
	    h_upperDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
	    h_upperDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
	    
	    h_upperPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
	    h_upperEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
	    h_upperPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
	    
	    h_upperPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
	    h_upperPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
	    h_upperTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	    h_upperTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
	    h_upperTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
	    
	    h_upperPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
	    h_upperTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
	    h_upperValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
	    h_upperValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
	    h_upperMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
	    h_upperMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
	    h_upperTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	  }
	  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	    h_upperChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
	    h_upperNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
	    h_upperChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
	    h_upperCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    h_upperDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
	    h_upperDzEtaMinusPhiZero->Fill(      *upTrackerDz);
	    h_upperDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
	    h_upperDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
	    
	    h_upperPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
	    h_upperEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
	    h_upperPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
	    
	    h_upperPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
	    h_upperPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
	    h_upperTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
	    h_upperTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
	    h_upperTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
	    
	    h_upperPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
	    h_upperTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
	    h_upperValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
	    h_upperValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
	    h_upperMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
	    h_upperMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
	    h_upperTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
	  }
	  else if(upTrackerTrack->phi() > 1.0472){
	    h_upperChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
	    h_upperNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
	    h_upperChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
	    h_upperCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    h_upperDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
	    h_upperDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
	    h_upperDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
	    h_upperDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
	    
	    h_upperPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
	    h_upperEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
	    h_upperPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
	    
	    h_upperPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
	    h_upperPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
	    h_upperTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	    h_upperTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
	    h_upperTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
	    
	    h_upperPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
	    h_upperTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
	    h_upperValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
	    h_upperValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
	    h_upperMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
	    h_upperMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
	    h_upperTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	    
	    
	  }
	}
	if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)) h_upperCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));


	h_upperDxy->Fill(     *upTrackerDxy);
	h_upperDz->Fill(      *upTrackerDz);
	h_upperDxyError->Fill(*upTrackerDxyError);
	h_upperDzError->Fill( *upTrackerDzError);

	h_upperPt->Fill( upTrackerMuonP4->pt());
	h_upperEta->Fill(upTrackerMuonP4->eta());
	h_upperPhi->Fill(upTrackerMuonP4->phi());
	
	h_upperPtError->Fill( *upTrackerPtError);
	h_upperPtRelErr->Fill( upperRelPtErr);
	h_upperTrackPt->Fill(  sqrt(upTrackerTrack->perp2()));
	h_upperTrackEta->Fill( upTrackerTrack->eta());
	h_upperTrackPhi->Fill( upTrackerTrack->phi());

	upperstring << "pt="      << sqrt(upTrackerTrack->perp2())
		    << ",eta="    << upTrackerTrack->eta()
		    << ",phi="    << upTrackerTrack->phi()
		    << ",charge=" << *upTrackerCharge;

	h_upperPixelHits->Fill(                   *upTrackerPhits);
	h_upperTrackerHits->Fill(                 *upTrackerThits);
	h_upperValidHits->Fill(                   *upTrackerValidHits);
	h_upperValidMuonHits->Fill(               *upTrackerValidMuonHits);
	h_upperMuonStationHits->Fill(             *upTrackerMhits);
	h_upperMatchedMuonStations->Fill(         *upTrackerMatchedMuonStations);
	h_upperTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

	for (int i = 0; i < nBiasBins; ++i) {
	  double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
	  double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
	  h_upperCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	  if (upTrackerTrack->eta() > 0){
	    h_upperCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_upperCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  }
	  
	  
	  else if (upTrackerTrack->eta() < 0){
	    h_upperCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_upperCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  }
	  if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	    h_upperCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  }
	  else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
       

	  h_upperCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	  if (upTrackerTrack->eta() > 0){
	    h_upperCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_upperCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  }
	  
	  
	  else if (upTrackerTrack->eta() < 0){
	    h_upperCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_upperCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  }
	  if(upTrackerTrack->phi() < -1.0472) h_upperCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	    h_upperCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  }
	  else if(upTrackerTrack->phi() > 1.0472) h_upperCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));





	  if (debug)
	    std::cout << "Made it through the upper bias loop " << i << std::endl; 
	}

	// fill the counters histogram for the upper leg muons passing the super-pointing selection, currently just fill

	if (up_superpointing && up_etabar) {
	//if (up_superpointing || true) {

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
	    h_muMinusChi2->Fill(       *upTrackerChi2);
	    h_muUpperMinusChi2->Fill(  *upTrackerChi2);
	    h_muMinusNdof->Fill(       *upTrackerNdof);
	    h_muUpperMinusNdof->Fill(  *upTrackerNdof);
	    h_muMinusCharge->Fill(     *upTrackerCharge);
	    h_muUpperMinusCharge->Fill(*upTrackerCharge);
	    h_muMinusCurve->Fill(     symmetric_?upperCpT:fabs(upperCpT));
	    if (upTrackerTrack->eta() > 0){
	      h_muMinusChi2EtaPlus->Fill(  *upTrackerChi2);
	      h_muMinusNdofEtaPlus->Fill(  *upTrackerNdof);
	      h_muMinusChargeEtaPlus->Fill(*upTrackerCharge);
	      h_muMinusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muMinusDxyEtaPlus->Fill(     *upTrackerDxy);
	      h_muMinusDzEtaPlus->Fill(      *upTrackerDz);
	      h_muMinusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
	      h_muMinusDzErrorEtaPlus->Fill( *upTrackerDzError);
	      
	      h_muMinusPtEtaPlus->Fill( upTrackerMuonP4->pt());
	      h_muMinusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
	      h_muMinusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
	      
	      h_muMinusPtErrorEtaPlus->Fill( *upTrackerPtError);
	      h_muMinusPtRelErrEtaPlus->Fill( upperRelPtErr);
	      h_muMinusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muMinusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
	      h_muMinusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
	      
	      h_muMinusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
	      h_muMinusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
	      h_muMinusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
	      h_muMinusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
	      h_muMinusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
	      h_muMinusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
	      h_muMinusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);
	      
	      
	      
	      
	      if(upTrackerTrack->phi() < -1.0472){
		h_muMinusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		h_muMinusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		h_muMinusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		h_muMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muMinusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		h_muMinusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		h_muMinusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		h_muMinusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
	      
		h_muMinusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muMinusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muMinusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		h_muMinusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		h_muMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		h_muMinusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		h_muMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		h_muMinusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		h_muMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	      }
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muMinusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		h_muMinusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		h_muMinusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		h_muMinusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muMinusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		h_muMinusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		h_muMinusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		h_muMinusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		
		h_muMinusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muMinusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muMinusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		h_muMinusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		h_muMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		h_muMinusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		h_muMinusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		h_muMinusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		h_muMinusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		h_muMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		
	
	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muMinusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		h_muMinusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		h_muMinusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		h_muMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muMinusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		h_muMinusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		h_muMinusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		h_muMinusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
	      
		h_muMinusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muMinusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muMinusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		h_muMinusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		h_muMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		h_muMinusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		h_muMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		h_muMinusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		h_muMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
	      }
	    }
	    
	    
	    else if (upTrackerTrack->eta() < 0){
	      h_muMinusChi2EtaMinus->Fill(  *upTrackerChi2);
	      h_muMinusNdofEtaMinus->Fill(  *upTrackerNdof);
	      h_muMinusChargeEtaMinus->Fill(*upTrackerCharge);
	      h_muMinusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muMinusDxyEtaMinus->Fill(     *upTrackerDxy);
	      h_muMinusDzEtaMinus->Fill(      *upTrackerDz);
	      h_muMinusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
	      h_muMinusDzErrorEtaMinus->Fill( *upTrackerDzError);
	      
	      h_muMinusPtEtaMinus->Fill( upTrackerMuonP4->pt());
	      h_muMinusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
	      h_muMinusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
	      
	      h_muMinusPtErrorEtaMinus->Fill( *upTrackerPtError);
	      h_muMinusPtRelErrEtaMinus->Fill( upperRelPtErr);
	      h_muMinusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muMinusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
	      h_muMinusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
	      
	      h_muMinusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
	      h_muMinusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
	      h_muMinusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
	      h_muMinusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
	      h_muMinusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
	      h_muMinusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
	      h_muMinusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);
	      


	      h_muMinusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      if(upTrackerTrack->phi() < -1.0472){
		h_muMinusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		h_muMinusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		h_muMinusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		h_muMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muMinusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		h_muMinusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		h_muMinusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		h_muMinusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		
		h_muMinusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muMinusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muMinusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		h_muMinusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		h_muMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		h_muMinusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		h_muMinusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		h_muMinusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		h_muMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		
	      }

	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muMinusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		h_muMinusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		h_muMinusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		h_muMinusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muMinusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		h_muMinusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		h_muMinusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		h_muMinusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		
		h_muMinusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muMinusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muMinusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		h_muMinusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		h_muMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		h_muMinusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		h_muMinusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		h_muMinusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		h_muMinusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		h_muMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
				

	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muMinusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		h_muMinusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		h_muMinusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		h_muMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muMinusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		h_muMinusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		h_muMinusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		h_muMinusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		
		h_muMinusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muMinusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muMinusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		h_muMinusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		h_muMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		h_muMinusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		h_muMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		h_muMinusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		h_muMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	      }
	    }
	    if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_muMinusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    
	    
	    
	    
	    h_muUpperMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	    if (upTrackerTrack->eta() > 0){
	      h_muUpperMinusChi2EtaPlus->Fill(  *upTrackerChi2);
	      h_muUpperMinusNdofEtaPlus->Fill(  *upTrackerNdof);
	      h_muUpperMinusChargeEtaPlus->Fill(*upTrackerCharge);
	      h_muUpperMinusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muUpperMinusDxyEtaPlus->Fill(     *upTrackerDxy);
	      h_muUpperMinusDzEtaPlus->Fill(      *upTrackerDz);
	      h_muUpperMinusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
	      h_muUpperMinusDzErrorEtaPlus->Fill( *upTrackerDzError);
	      
	      h_muUpperMinusPtEtaPlus->Fill( upTrackerMuonP4->pt());
	      h_muUpperMinusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
	      h_muUpperMinusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
	      
	      h_muUpperMinusPtErrorEtaPlus->Fill( *upTrackerPtError);
	      h_muUpperMinusPtRelErrEtaPlus->Fill( upperRelPtErr);
	      h_muUpperMinusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muUpperMinusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
	      h_muUpperMinusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
	      
	      h_muUpperMinusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
	      h_muUpperMinusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
	      h_muUpperMinusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
	      h_muUpperMinusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
	      h_muUpperMinusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
	      h_muUpperMinusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
	      h_muUpperMinusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);
	      

	    
	      if(upTrackerTrack->phi() < -1.0472){
		h_muUpperMinusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		h_muUpperMinusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		h_muUpperMinusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		h_muUpperMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperMinusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		h_muUpperMinusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		h_muUpperMinusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		h_muUpperMinusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		
		h_muUpperMinusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muUpperMinusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muUpperMinusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperMinusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		h_muUpperMinusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		h_muUpperMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperMinusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		h_muUpperMinusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muUpperMinusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		h_muUpperMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		h_muUpperMinusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muUpperMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muUpperMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		h_muUpperMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	      }		
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperMinusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		h_muUpperMinusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		h_muUpperMinusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		h_muUpperMinusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperMinusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		h_muUpperMinusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		h_muUpperMinusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		h_muUpperMinusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		
		h_muUpperMinusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muUpperMinusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muUpperMinusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muUpperMinusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		h_muUpperMinusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		h_muUpperMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperMinusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		h_muUpperMinusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muUpperMinusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		h_muUpperMinusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		h_muUpperMinusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		h_muUpperMinusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muUpperMinusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		h_muUpperMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);

	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muUpperMinusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		h_muUpperMinusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		h_muUpperMinusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		h_muUpperMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperMinusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		h_muUpperMinusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		h_muUpperMinusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		h_muUpperMinusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		
		h_muUpperMinusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muUpperMinusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muUpperMinusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperMinusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		h_muUpperMinusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		h_muUpperMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperMinusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		h_muUpperMinusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muUpperMinusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		h_muUpperMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		h_muUpperMinusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muUpperMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muUpperMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		h_muUpperMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	      }
	    }
	    
	    
	    else if (upTrackerTrack->eta() < 0){
	      h_muUpperMinusChi2EtaMinus->Fill(  *upTrackerChi2);
	      h_muUpperMinusNdofEtaMinus->Fill(  *upTrackerNdof);
	      h_muUpperMinusChargeEtaMinus->Fill(*upTrackerCharge);
	      h_muUpperMinusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muUpperMinusDxyEtaMinus->Fill(     *upTrackerDxy);
	      h_muUpperMinusDzEtaMinus->Fill(      *upTrackerDz);
	      h_muUpperMinusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
	      h_muUpperMinusDzErrorEtaMinus->Fill( *upTrackerDzError);
	      
	      h_muUpperMinusPtEtaMinus->Fill( upTrackerMuonP4->pt());
	      h_muUpperMinusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
	      h_muUpperMinusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
	      
	      h_muUpperMinusPtErrorEtaMinus->Fill( *upTrackerPtError);
	      h_muUpperMinusPtRelErrEtaMinus->Fill( upperRelPtErr);
	      h_muUpperMinusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muUpperMinusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
	      h_muUpperMinusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
	      
	      h_muUpperMinusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
	      h_muUpperMinusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
	      h_muUpperMinusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
	      h_muUpperMinusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
	      h_muUpperMinusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
	      h_muUpperMinusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
	      h_muUpperMinusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);



	      if(upTrackerTrack->phi() < -1.0472){
		h_muUpperMinusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		h_muUpperMinusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		h_muUpperMinusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		h_muUpperMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperMinusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		h_muUpperMinusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		h_muUpperMinusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		h_muUpperMinusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		
		h_muUpperMinusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muUpperMinusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muUpperMinusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperMinusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		h_muUpperMinusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		h_muUpperMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperMinusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		h_muUpperMinusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muUpperMinusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		h_muUpperMinusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		h_muUpperMinusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muUpperMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muUpperMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		h_muUpperMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	      }
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperMinusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		h_muUpperMinusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		h_muUpperMinusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		h_muUpperMinusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperMinusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		h_muUpperMinusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		h_muUpperMinusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		h_muUpperMinusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		
		h_muUpperMinusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muUpperMinusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muUpperMinusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muUpperMinusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		h_muUpperMinusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		h_muUpperMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperMinusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		h_muUpperMinusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muUpperMinusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		h_muUpperMinusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		h_muUpperMinusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		h_muUpperMinusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muUpperMinusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		h_muUpperMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
	
	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muUpperMinusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		h_muUpperMinusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		h_muUpperMinusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		h_muUpperMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperMinusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		h_muUpperMinusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		h_muUpperMinusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		h_muUpperMinusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		
		h_muUpperMinusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muUpperMinusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muUpperMinusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperMinusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		h_muUpperMinusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		h_muUpperMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperMinusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		h_muUpperMinusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muUpperMinusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		h_muUpperMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		h_muUpperMinusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muUpperMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muUpperMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		h_muUpperMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	      }
	    }
	    if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_muUpperMinusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    
	    
	    h_muMinusDxy->Fill(     *upTrackerDxy);
	    h_muUpperMinusDxy->Fill(*upTrackerDxy);
	    h_muMinusDz->Fill(      *upTrackerDz);
	    h_muUpperMinusDz->Fill( *upTrackerDz);
	    h_muMinusDxyError->Fill(*upTrackerDxyError);
	    h_muUpperMinusDxyError->Fill(*upTrackerDxyError);
	    h_muMinusDzError->Fill(      *upTrackerDzError);
	    h_muUpperMinusDzError->Fill( *upTrackerDzError);
	    
	    h_muMinusPt->Fill(      upTrackerMuonP4->pt());
	    h_muUpperMinusPt->Fill( upTrackerMuonP4->pt());
	    h_muMinusEta->Fill(     upTrackerMuonP4->eta());
	    h_muUpperMinusEta->Fill(upTrackerMuonP4->eta());
	    h_muMinusPhi->Fill(     upTrackerMuonP4->phi());
	    h_muUpperMinusPhi->Fill(upTrackerMuonP4->phi());

	    h_muMinusPtError->Fill(     *upTrackerPtError);
	    h_muUpperMinusPtError->Fill(*upTrackerPtError);
	    h_muMinusPtRelErr->Fill(     upperRelPtErr);
	    h_muUpperMinusPtRelErr->Fill(upperRelPtErr);
	    h_muMinusTrackPt->Fill(     sqrt(upTrackerTrack->perp2()));
	    h_muUpperMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	    h_muMinusTrackEta->Fill(     upTrackerTrack->eta());
	    h_muUpperMinusTrackEta->Fill(upTrackerTrack->eta());
	    h_muMinusTrackPhi->Fill(     upTrackerTrack->phi());
	    h_muUpperMinusTrackPhi->Fill(upTrackerTrack->phi());

	    h_muMinusPixelHits->Fill(                        *upTrackerPhits);
	    h_muUpperMinusPixelHits->Fill(                   *upTrackerPhits);
	    h_muMinusTrackerHits->Fill(                      *upTrackerThits);
	    h_muUpperMinusTrackerHits->Fill(                 *upTrackerThits);
	    h_muMinusValidHits->Fill(                        *upTrackerValidHits);
	    h_muUpperMinusValidHits->Fill(                   *upTrackerValidHits);
	    h_muMinusValidMuonHits->Fill(                    *upTrackerValidMuonHits);
	    h_muUpperMinusValidMuonHits->Fill(               *upTrackerValidMuonHits);
	    h_muMinusMuonStationHits->Fill(                  *upTrackerMhits);
	    h_muUpperMinusMuonStationHits->Fill(             *upTrackerMhits);
	    h_muMinusMatchedMuonStations->Fill(              *upTrackerMatchedMuonStations);
	    h_muUpperMinusMatchedMuonStations->Fill(         *upTrackerMatchedMuonStations);
	    h_muMinusTrackerLayersWithMeasurement->Fill(     *upTrackerLayersWithMeasurement);
	    h_muUpperMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muMinusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
	      if (upTrackerTrack->eta() > 0){
		h_muMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      
	      
	      h_muMinusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));	
	      if (upTrackerTrack->eta() > 0){
		h_muMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      
	      
	      h_muUpperMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      if (upTrackerTrack->eta() > 0){
		h_muUpperMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muUpperMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      
	      
	      h_muUpperMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
	      if (upTrackerTrack->eta() > 0){
		h_muUpperMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muUpperMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muUpperMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muUpperMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      

	      
	      if (debug)
		std::cout << "Made it through the upper bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (up_n1pt) {
	      h_looseMuMinusChi2->Fill(       *upTrackerChi2);
	      h_looseMuUpperMinusChi2->Fill(  *upTrackerChi2);
	      h_looseMuMinusNdof->Fill(       *upTrackerNdof);
	      h_looseMuUpperMinusNdof->Fill(  *upTrackerNdof);
	      h_looseMuMinusCharge->Fill(     *upTrackerCharge);
	      h_looseMuUpperMinusCharge->Fill(*upTrackerCharge);
	      h_looseMuMinusCurve->Fill(     symmetric_?upperCpT:fabs(upperCpT));
	      if (upTrackerTrack->eta() > 0){
		h_looseMuMinusChi2EtaPlus->Fill(  *upTrackerChi2);
		h_looseMuMinusNdofEtaPlus->Fill(  *upTrackerNdof);
		h_looseMuMinusChargeEtaPlus->Fill(*upTrackerCharge);
		h_looseMuMinusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuMinusDxyEtaPlus->Fill(     *upTrackerDxy);
		h_looseMuMinusDzEtaPlus->Fill(      *upTrackerDz);
		h_looseMuMinusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
		h_looseMuMinusDzErrorEtaPlus->Fill( *upTrackerDzError);
		
		h_looseMuMinusPtEtaPlus->Fill( upTrackerMuonP4->pt());
		h_looseMuMinusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
		h_looseMuMinusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuMinusPtErrorEtaPlus->Fill( *upTrackerPtError);
		h_looseMuMinusPtRelErrEtaPlus->Fill( upperRelPtErr);
		h_looseMuMinusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuMinusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
		h_looseMuMinusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
		
		h_looseMuMinusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
		h_looseMuMinusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
		h_looseMuMinusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
		h_looseMuMinusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
		h_looseMuMinusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
		h_looseMuMinusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuMinusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);
		
		
		
		
		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuMinusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuMinusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuMinusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuMinusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuMinusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuMinusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuMinusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuMinusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuMinusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuMinusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		}
		
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuMinusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuMinusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuMinusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuMinusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuMinusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuMinusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuMinusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuMinusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuMinusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		  h_looseMuMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuMinusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		

		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuMinusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuMinusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuMinusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuMinusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuMinusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuMinusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuMinusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuMinusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuMinusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuMinusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		}
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_looseMuMinusChi2EtaPlus->Fill(  *upTrackerChi2);
		h_looseMuMinusNdofEtaMinus->Fill(  *upTrackerNdof);
		h_looseMuMinusChargeEtaMinus->Fill(*upTrackerCharge);
		h_looseMuMinusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuMinusDxyEtaMinus->Fill(     *upTrackerDxy);
		h_looseMuMinusDzEtaMinus->Fill(      *upTrackerDz);
		h_looseMuMinusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
		h_looseMuMinusDzErrorEtaMinus->Fill( *upTrackerDzError);
		
		h_looseMuMinusPtEtaMinus->Fill( upTrackerMuonP4->pt());
		h_looseMuMinusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
		h_looseMuMinusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuMinusPtErrorEtaMinus->Fill( *upTrackerPtError);
		h_looseMuMinusPtRelErrEtaMinus->Fill( upperRelPtErr);
		h_looseMuMinusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuMinusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
		h_looseMuMinusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
		
		h_looseMuMinusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
		h_looseMuMinusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
		h_looseMuMinusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
		h_looseMuMinusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
		h_looseMuMinusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
		h_looseMuMinusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuMinusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);
		

		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuMinusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuMinusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuMinusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuMinusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuMinusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuMinusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuMinusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuMinusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuMinusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		}
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuMinusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuMinusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuMinusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuMinusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuMinusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuMinusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuMinusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuMinusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		  h_looseMuMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuMinusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
	
		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuMinusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuMinusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuMinusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuMinusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuMinusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuMinusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuMinusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuMinusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuMinusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		}
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_looseMuMinusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      
	      
	      h_looseMuUpperMinusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      if (upTrackerTrack->eta() > 0){
		h_looseMuUpperMinusChi2EtaPlus->Fill(  *upTrackerChi2);
		h_looseMuUpperMinusNdofEtaPlus->Fill(  *upTrackerNdof);
		h_looseMuUpperMinusChargeEtaPlus->Fill(*upTrackerCharge);
		h_looseMuUpperMinusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuUpperMinusDxyEtaPlus->Fill(     *upTrackerDxy);
		h_looseMuUpperMinusDzEtaPlus->Fill(      *upTrackerDz);
		h_looseMuUpperMinusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
		h_looseMuUpperMinusDzErrorEtaPlus->Fill( *upTrackerDzError);
		
		h_looseMuUpperMinusPtEtaPlus->Fill( upTrackerMuonP4->pt());
		h_looseMuUpperMinusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
		h_looseMuUpperMinusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuUpperMinusPtErrorEtaPlus->Fill( *upTrackerPtError);
		h_looseMuUpperMinusPtRelErrEtaPlus->Fill( upperRelPtErr);
		h_looseMuUpperMinusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuUpperMinusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
		h_looseMuUpperMinusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
		
		h_looseMuUpperMinusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
		h_looseMuUpperMinusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
		h_looseMuUpperMinusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
		h_looseMuUpperMinusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
		h_looseMuUpperMinusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
		h_looseMuUpperMinusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);


		h_looseMuUpperMinusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuUpperMinusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuUpperMinusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuUpperMinusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuUpperMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperMinusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuUpperMinusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuUpperMinusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuUpperMinusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperMinusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperMinusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperMinusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperMinusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuUpperMinusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuUpperMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperMinusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperMinusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperMinusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuUpperMinusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuUpperMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		  

		}

		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperMinusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuUpperMinusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuUpperMinusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuUpperMinusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperMinusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuUpperMinusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuUpperMinusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuUpperMinusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperMinusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperMinusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperMinusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperMinusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuUpperMinusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		  h_looseMuUpperMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperMinusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuUpperMinusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperMinusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuUpperMinusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuUpperMinusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperMinusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperMinusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuUpperMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		 
		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuUpperMinusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuUpperMinusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuUpperMinusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuUpperMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperMinusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuUpperMinusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuUpperMinusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuUpperMinusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperMinusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperMinusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperMinusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperMinusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuUpperMinusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuUpperMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperMinusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperMinusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperMinusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuUpperMinusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuUpperMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		}
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_looseMuUpperMinusChi2EtaMinus->Fill(  *upTrackerChi2);
		h_looseMuUpperMinusNdofEtaMinus->Fill(  *upTrackerNdof);
		h_looseMuUpperMinusChargeEtaMinus->Fill(*upTrackerCharge);
		h_looseMuUpperMinusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuUpperMinusDxyEtaMinus->Fill(     *upTrackerDxy);
		h_looseMuUpperMinusDzEtaMinus->Fill(      *upTrackerDz);
		h_looseMuUpperMinusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
		h_looseMuUpperMinusDzErrorEtaMinus->Fill( *upTrackerDzError);
		
		h_looseMuUpperMinusPtEtaMinus->Fill( upTrackerMuonP4->pt());
		h_looseMuUpperMinusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
		h_looseMuUpperMinusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuUpperMinusPtErrorEtaMinus->Fill( *upTrackerPtError);
		h_looseMuUpperMinusPtRelErrEtaMinus->Fill( upperRelPtErr);
		h_looseMuUpperMinusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuUpperMinusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
		h_looseMuUpperMinusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
		
		h_looseMuUpperMinusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
		h_looseMuUpperMinusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
		h_looseMuUpperMinusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
		h_looseMuUpperMinusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
		h_looseMuUpperMinusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
		h_looseMuUpperMinusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);
		
		

		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuUpperMinusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuUpperMinusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuUpperMinusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuUpperMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperMinusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuUpperMinusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuUpperMinusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuUpperMinusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperMinusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperMinusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperMinusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperMinusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuUpperMinusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuUpperMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperMinusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperMinusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperMinusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperMinusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuUpperMinusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuUpperMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		  
		}
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperMinusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuUpperMinusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuUpperMinusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuUpperMinusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperMinusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuUpperMinusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuUpperMinusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuUpperMinusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperMinusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperMinusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperMinusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperMinusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuUpperMinusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		  h_looseMuUpperMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperMinusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuUpperMinusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperMinusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuUpperMinusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuUpperMinusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperMinusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperMinusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuUpperMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		 		 

		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuUpperMinusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuUpperMinusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuUpperMinusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuUpperMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperMinusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuUpperMinusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuUpperMinusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuUpperMinusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperMinusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperMinusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperMinusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperMinusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuUpperMinusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuUpperMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperMinusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperMinusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperMinusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuUpperMinusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuUpperMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		}
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_looseMuUpperMinusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      
	      
	      

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuMinusDxy->Fill(     *upTrackerDxy);
	      h_looseMuUpperMinusDxy->Fill(*upTrackerDxy);
	      h_looseMuMinusDz->Fill(      *upTrackerDz);
	      h_looseMuUpperMinusDz->Fill( *upTrackerDz);
	      h_looseMuMinusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuUpperMinusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuMinusDzError->Fill(      *upTrackerDzError);
	      h_looseMuUpperMinusDzError->Fill( *upTrackerDzError);

	      h_looseMuMinusPt->Fill(      upTrackerMuonP4->pt());
	      h_looseMuUpperMinusPt->Fill( upTrackerMuonP4->pt());
	      h_looseMuMinusEta->Fill(     upTrackerMuonP4->eta());
	      h_looseMuUpperMinusEta->Fill(upTrackerMuonP4->eta());
	      h_looseMuMinusPhi->Fill(     upTrackerMuonP4->phi());
	      h_looseMuUpperMinusPhi->Fill(upTrackerMuonP4->phi());

	      h_looseMuMinusTrackPt->Fill(     sqrt(upTrackerTrack->perp2()));
	      h_looseMuUpperMinusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      h_looseMuMinusTrackEta->Fill(         upTrackerTrack->eta());
	      h_looseMuUpperMinusTrackEta->Fill(    upTrackerTrack->eta());
	      h_looseMuMinusTrackPhi->Fill(         upTrackerTrack->phi());
	      h_looseMuUpperMinusTrackPhi->Fill(    upTrackerTrack->phi());

	      h_looseMuMinusValidHits->Fill(         *upTrackerValidHits);
	      h_looseMuUpperMinusValidHits->Fill(    *upTrackerValidHits);

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuMinusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
		if (upTrackerTrack->eta() > 0){
		  h_looseMuMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuMinusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));	
		if (upTrackerTrack->eta() > 0){
		  h_looseMuMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		
		h_looseMuUpperMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		if (upTrackerTrack->eta() > 0){
		  h_looseMuUpperMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuUpperMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuUpperMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		if (upTrackerTrack->eta() > 0){
		  h_looseMuUpperMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
	      
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuUpperMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		if (debug)
		  std::cout << "Made it through the upper bias loop " << i << std::endl; 
	      }
	    } // end if (up_n1pt)

	    if (up_n1pixhits) {
	      h_looseMuMinusPixelHits->Fill(     *upTrackerPhits);
	      h_looseMuUpperMinusPixelHits->Fill(*upTrackerPhits);
	    }
	    if (up_n1vmuhits) {
	      h_looseMuMinusValidMuonHits->Fill(       *upTrackerValidMuonHits);
	      h_looseMuUpperMinusValidMuonHits->Fill(  *upTrackerValidMuonHits);
	      h_looseMuMinusMuonStationHits->Fill(     *upTrackerMhits);
	      h_looseMuUpperMinusMuonStationHits->Fill(*upTrackerMhits);
	    }
	    if (up_n1ptrelerr) {
	      h_looseMuMinusPtError->Fill(     *upTrackerPtError);
	      h_looseMuUpperMinusPtError->Fill(*upTrackerPtError);
	      h_looseMuMinusPtRelErr->Fill(     upperRelPtErr);
	      h_looseMuUpperMinusPtRelErr->Fill(upperRelPtErr);
	    }
	    if (up_n1trkhits) {
	      h_looseMuMinusTrackerHits->Fill(       *upTrackerThits);
	      h_looseMuUpperMinusTrackerHits->Fill(  *upTrackerThits);
	      h_looseMuMinusTrackerLayersWithMeasurement->Fill(     *upTrackerLayersWithMeasurement);
	      h_looseMuUpperMinusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	    }
	    if (up_n1mmustahits) {
	      h_looseMuMinusMatchedMuonStations->Fill(     *upTrackerMatchedMuonStations);
	      h_looseMuUpperMinusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
	    }
	  } // end if (charge < 0)

	  else { // charge > 0
	    h_muPlusChi2->Fill(       *upTrackerChi2);
	    h_muUpperPlusChi2->Fill(  *upTrackerChi2);
	    h_muPlusNdof->Fill(       *upTrackerNdof);
	    h_muUpperPlusNdof->Fill(  *upTrackerNdof);
	    h_muPlusCharge->Fill(     *upTrackerCharge);
	    h_muUpperPlusCharge->Fill(*upTrackerCharge);
	    h_muPlusCurve->Fill(     symmetric_?upperCpT:fabs(upperCpT));
	    if (upTrackerTrack->eta() > 0){

	      h_muPlusChi2EtaPlus->Fill(  *upTrackerChi2);
	      h_muPlusNdofEtaPlus->Fill(  *upTrackerNdof);
	      h_muPlusChargeEtaPlus->Fill(*upTrackerCharge);
	      h_muPlusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muPlusDxyEtaPlus->Fill(     *upTrackerDxy);
	      h_muPlusDzEtaPlus->Fill(      *upTrackerDz);
	      h_muPlusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
	      h_muPlusDzErrorEtaPlus->Fill( *upTrackerDzError);
	      
	      h_muPlusPtEtaPlus->Fill( upTrackerMuonP4->pt());
	      h_muPlusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
	      h_muPlusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
	      
	      h_muPlusPtErrorEtaPlus->Fill( *upTrackerPtError);
	      h_muPlusPtRelErrEtaPlus->Fill( upperRelPtErr);
	      h_muPlusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muPlusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
	      h_muPlusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
	      
	      h_muPlusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
	      h_muPlusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
	      h_muPlusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
	      h_muPlusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
	      h_muPlusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
	      h_muPlusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
	      h_muPlusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);


	      if(upTrackerTrack->phi() < -1.0472){
		h_muPlusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		h_muPlusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		h_muPlusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		h_muPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		h_muPlusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		h_muPlusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		h_muPlusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		
		h_muPlusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muPlusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muPlusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		h_muPlusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		h_muPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		h_muPlusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		h_muPlusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		h_muPlusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		h_muPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);

	      }
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muPlusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		h_muPlusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		h_muPlusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		h_muPlusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		h_muPlusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		h_muPlusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		h_muPlusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		
		h_muPlusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muPlusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muPlusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		h_muPlusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		h_muPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		h_muPlusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		h_muPlusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		h_muPlusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		h_muPlusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		h_muPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		

	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muPlusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		h_muPlusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		h_muPlusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		h_muPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		h_muPlusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		h_muPlusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		h_muPlusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		
		h_muPlusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muPlusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muPlusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		h_muPlusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		h_muPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		h_muPlusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		h_muPlusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		h_muPlusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		h_muPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	      }

	    }
	    
	    
	    else if (upTrackerTrack->eta() < 0){
	      h_muPlusChi2EtaMinus->Fill(  *upTrackerChi2);
	      h_muPlusNdofEtaMinus->Fill(  *upTrackerNdof);
	      h_muPlusChargeEtaMinus->Fill(*upTrackerCharge);
	      h_muPlusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muPlusDxyEtaMinus->Fill(     *upTrackerDxy);
	      h_muPlusDzEtaMinus->Fill(      *upTrackerDz);
	      h_muPlusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
	      h_muPlusDzErrorEtaMinus->Fill( *upTrackerDzError);
	      
	      h_muPlusPtEtaMinus->Fill( upTrackerMuonP4->pt());
	      h_muPlusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
	      h_muPlusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
	      
	      h_muPlusPtErrorEtaMinus->Fill( *upTrackerPtError);
	      h_muPlusPtRelErrEtaMinus->Fill( upperRelPtErr);
	      h_muPlusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muPlusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
	      h_muPlusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
	      
	      h_muPlusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
	      h_muPlusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
	      h_muPlusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
	      h_muPlusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
	      h_muPlusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
	      h_muPlusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
	      h_muPlusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);



	      if(upTrackerTrack->phi() < -1.0472){
		h_muPlusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		h_muPlusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		h_muPlusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		h_muPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		h_muPlusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		h_muPlusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		h_muPlusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		
		h_muPlusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muPlusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muPlusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		h_muPlusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		h_muPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		h_muPlusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		h_muPlusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		h_muPlusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		h_muPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	      }
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muPlusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		h_muPlusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		h_muPlusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		h_muPlusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		h_muPlusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		h_muPlusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		h_muPlusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		
		h_muPlusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muPlusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muPlusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		h_muPlusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		h_muPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		h_muPlusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		h_muPlusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		h_muPlusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		h_muPlusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		h_muPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		
	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muPlusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		h_muPlusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		h_muPlusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		h_muPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muPlusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		h_muPlusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		h_muPlusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		h_muPlusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		
		h_muPlusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muPlusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muPlusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		h_muPlusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		h_muPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		h_muPlusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		h_muPlusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		h_muPlusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		h_muPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
	      }
	    }
	    if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_muPlusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    
	    
	    
	    h_muUpperPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	    if (upTrackerTrack->eta() > 0){
	      h_muUpperPlusChi2EtaPlus->Fill(  *upTrackerChi2);
	      h_muUpperPlusNdofEtaPlus->Fill(  *upTrackerNdof);
	      h_muUpperPlusChargeEtaPlus->Fill(*upTrackerCharge);
	      h_muUpperPlusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muUpperPlusDxyEtaPlus->Fill(     *upTrackerDxy);
	      h_muUpperPlusDzEtaPlus->Fill(      *upTrackerDz);
	      h_muUpperPlusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
	      h_muUpperPlusDzErrorEtaPlus->Fill( *upTrackerDzError);
	      
	      h_muUpperPlusPtEtaPlus->Fill( upTrackerMuonP4->pt());
	      h_muUpperPlusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
	      h_muUpperPlusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
	      
	      h_muUpperPlusPtErrorEtaPlus->Fill( *upTrackerPtError);
	      h_muUpperPlusPtRelErrEtaPlus->Fill( upperRelPtErr);
	      h_muUpperPlusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muUpperPlusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
	      h_muUpperPlusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
	      
	      h_muUpperPlusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
	      h_muUpperPlusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
	      h_muUpperPlusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
	      h_muUpperPlusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
	      h_muUpperPlusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
	      h_muUpperPlusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
	      h_muUpperPlusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);


	      if(upTrackerTrack->phi() < -1.0472){
		h_muUpperPlusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		h_muUpperPlusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		h_muUpperPlusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		h_muUpperPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperPlusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		h_muUpperPlusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		h_muUpperPlusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		h_muUpperPlusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		
		h_muUpperPlusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muUpperPlusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muUpperPlusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperPlusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		h_muUpperPlusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		h_muUpperPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperPlusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		h_muUpperPlusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muUpperPlusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		h_muUpperPlusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		h_muUpperPlusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muUpperPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muUpperPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		h_muUpperPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	      }
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperPlusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		h_muUpperPlusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		h_muUpperPlusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		h_muUpperPlusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperPlusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		h_muUpperPlusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		h_muUpperPlusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		h_muUpperPlusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		
		h_muUpperPlusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muUpperPlusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muUpperPlusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muUpperPlusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		h_muUpperPlusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		h_muUpperPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperPlusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		h_muUpperPlusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muUpperPlusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		h_muUpperPlusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		h_muUpperPlusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		h_muUpperPlusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muUpperPlusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		h_muUpperPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);


	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muUpperPlusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		h_muUpperPlusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		h_muUpperPlusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		h_muUpperPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperPlusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		h_muUpperPlusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		h_muUpperPlusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		h_muUpperPlusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		
		h_muUpperPlusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muUpperPlusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muUpperPlusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperPlusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		h_muUpperPlusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		h_muUpperPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperPlusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		h_muUpperPlusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		
		h_muUpperPlusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		h_muUpperPlusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		h_muUpperPlusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muUpperPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muUpperPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		h_muUpperPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);

	      }
	    }
	    
	    
	    else if (upTrackerTrack->eta() < 0){
	      h_muUpperPlusChi2EtaMinus->Fill(  *upTrackerChi2);
	      h_muUpperPlusNdofEtaMinus->Fill(  *upTrackerNdof);
	      h_muUpperPlusChargeEtaMinus->Fill(*upTrackerCharge);
	      h_muUpperPlusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      h_muUpperPlusDxyEtaMinus->Fill(     *upTrackerDxy);
	      h_muUpperPlusDzEtaMinus->Fill(      *upTrackerDz);
	      h_muUpperPlusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
	      h_muUpperPlusDzErrorEtaMinus->Fill( *upTrackerDzError);
	      
	      h_muUpperPlusPtEtaMinus->Fill( upTrackerMuonP4->pt());
	      h_muUpperPlusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
	      h_muUpperPlusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
	      
	      h_muUpperPlusPtErrorEtaMinus->Fill( *upTrackerPtError);
	      h_muUpperPlusPtRelErrEtaMinus->Fill( upperRelPtErr);
	      h_muUpperPlusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
	      h_muUpperPlusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
	      h_muUpperPlusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
	      
	      h_muUpperPlusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
	      h_muUpperPlusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
	      h_muUpperPlusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
	      h_muUpperPlusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
	      h_muUpperPlusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
	      h_muUpperPlusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
	      h_muUpperPlusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);



	      if(upTrackerTrack->phi() < -1.0472){
		h_muUpperPlusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		h_muUpperPlusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		h_muUpperPlusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		h_muUpperPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperPlusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		h_muUpperPlusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		h_muUpperPlusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		h_muUpperPlusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		
		h_muUpperPlusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		h_muUpperPlusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		h_muUpperPlusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperPlusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		h_muUpperPlusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		h_muUpperPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperPlusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		h_muUpperPlusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		
		h_muUpperPlusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		h_muUpperPlusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		h_muUpperPlusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		h_muUpperPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		h_muUpperPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		h_muUpperPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
	      }		

	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperPlusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		h_muUpperPlusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		h_muUpperPlusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		h_muUpperPlusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperPlusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		h_muUpperPlusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		h_muUpperPlusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		h_muUpperPlusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		
		h_muUpperPlusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		h_muUpperPlusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		h_muUpperPlusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		
		h_muUpperPlusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		h_muUpperPlusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		h_muUpperPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperPlusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		h_muUpperPlusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		
		h_muUpperPlusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		h_muUpperPlusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		h_muUpperPlusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		h_muUpperPlusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		h_muUpperPlusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		h_muUpperPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		

	      }
	      else if(upTrackerTrack->phi() > 1.0472){
		h_muUpperPlusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		h_muUpperPlusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		h_muUpperPlusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		h_muUpperPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_muUpperPlusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		h_muUpperPlusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		h_muUpperPlusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		h_muUpperPlusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		
		h_muUpperPlusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		h_muUpperPlusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		h_muUpperPlusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		
		h_muUpperPlusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		h_muUpperPlusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		h_muUpperPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_muUpperPlusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		h_muUpperPlusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
	      
		h_muUpperPlusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		h_muUpperPlusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		h_muUpperPlusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		h_muUpperPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		h_muUpperPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		h_muUpperPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		h_muUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);

	      }
		
	      }
	    if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_muUpperPlusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    }
	    else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	    
	    

	    h_muPlusDxy->Fill(     *upTrackerDxy);
	    h_muUpperPlusDxy->Fill(*upTrackerDxy);
	    h_muPlusDz->Fill(      *upTrackerDz);
	    h_muUpperPlusDz->Fill( *upTrackerDz);
	    h_muPlusDxyError->Fill(*upTrackerDxyError);
	    h_muUpperPlusDxyError->Fill(*upTrackerDxyError);
	    h_muPlusDzError->Fill(      *upTrackerDzError);
	    h_muUpperPlusDzError->Fill( *upTrackerDzError);

	    h_muPlusPt->Fill(      upTrackerMuonP4->pt());
	    h_muUpperPlusPt->Fill( upTrackerMuonP4->pt());
	    h_muPlusEta->Fill(     upTrackerMuonP4->eta());
	    h_muUpperPlusEta->Fill(upTrackerMuonP4->eta());
	    h_muPlusPhi->Fill(     upTrackerMuonP4->phi());
	    h_muUpperPlusPhi->Fill(upTrackerMuonP4->phi());

	    h_muPlusPtError->Fill(     *upTrackerPtError);
	    h_muUpperPlusPtError->Fill(*upTrackerPtError);
	    h_muPlusPtRelErr->Fill(     upperRelPtErr);
	    h_muUpperPlusPtRelErr->Fill(upperRelPtErr);
	    h_muPlusTrackPt->Fill(     sqrt(upTrackerTrack->perp2()));
	    h_muUpperPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	    h_muPlusTrackEta->Fill(     upTrackerTrack->eta());
	    h_muUpperPlusTrackEta->Fill(upTrackerTrack->eta());
	    h_muPlusTrackPhi->Fill(     upTrackerTrack->phi());
	    h_muUpperPlusTrackPhi->Fill(upTrackerTrack->phi());

	    h_muPlusPixelHits->Fill(                        *upTrackerPhits);
	    h_muUpperPlusPixelHits->Fill(                   *upTrackerPhits);
	    h_muPlusTrackerHits->Fill(                      *upTrackerThits);
	    h_muUpperPlusTrackerHits->Fill(                 *upTrackerThits);
	    h_muPlusValidHits->Fill(                        *upTrackerValidHits);
	    h_muUpperPlusValidHits->Fill(                   *upTrackerValidHits);
	    h_muPlusValidMuonHits->Fill(                    *upTrackerValidMuonHits);
	    h_muUpperPlusValidMuonHits->Fill(               *upTrackerValidMuonHits);
	    h_muPlusMuonStationHits->Fill(                  *upTrackerMhits);
	    h_muUpperPlusMuonStationHits->Fill(             *upTrackerMhits);
	    h_muPlusMatchedMuonStations->Fill(              *upTrackerMatchedMuonStations);
	    h_muUpperPlusMatchedMuonStations->Fill(         *upTrackerMatchedMuonStations);
	    h_muPlusTrackerLayersWithMeasurement->Fill(     *upTrackerLayersWithMeasurement);
	    h_muUpperPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muPlusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
	      if (upTrackerTrack->eta() > 0){
		h_muPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		  else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      
	      
	      
	      h_muPlusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));	
	      if (upTrackerTrack->eta() > 0){
		h_muPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		  else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      
	      
	      
	      h_muUpperPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      if (upTrackerTrack->eta() > 0){
		h_muUpperPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muUpperPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      
	      
	      h_muUpperPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	      if (upTrackerTrack->eta() > 0){
		h_muUpperPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_muUpperPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_muUpperPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_muUpperPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_muUpperPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_muUpperPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      
	      
	      
	      if (debug)
		std::cout << "Made it through the upper bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (up_n1pt) {
	      h_looseMuPlusChi2->Fill(       *upTrackerChi2);
	      h_looseMuUpperPlusChi2->Fill(  *upTrackerChi2);
	      h_looseMuPlusNdof->Fill(       *upTrackerNdof);
	      h_looseMuUpperPlusNdof->Fill(  *upTrackerNdof);
	      h_looseMuPlusCharge->Fill(     *upTrackerCharge);
	      h_looseMuUpperPlusCharge->Fill(*upTrackerCharge);
	      h_looseMuPlusCurve->Fill(     symmetric_?upperCpT:fabs(upperCpT));
	      if (upTrackerTrack->eta() > 0){
		h_looseMuPlusChi2EtaPlus->Fill(  *upTrackerChi2);
		h_looseMuPlusNdofEtaPlus->Fill(  *upTrackerNdof);
		h_looseMuPlusChargeEtaPlus->Fill(*upTrackerCharge);
		h_looseMuPlusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuPlusDxyEtaPlus->Fill(     *upTrackerDxy);
		h_looseMuPlusDzEtaPlus->Fill(      *upTrackerDz);
		h_looseMuPlusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
		h_looseMuPlusDzErrorEtaPlus->Fill( *upTrackerDzError);
		
		h_looseMuPlusPtEtaPlus->Fill( upTrackerMuonP4->pt());
		h_looseMuPlusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
		h_looseMuPlusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuPlusPtErrorEtaPlus->Fill( *upTrackerPtError);
		h_looseMuPlusPtRelErrEtaPlus->Fill( upperRelPtErr);
		h_looseMuPlusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuPlusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
		h_looseMuPlusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
		
		h_looseMuPlusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
		h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
		h_looseMuPlusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
		h_looseMuPlusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
		h_looseMuPlusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
		h_looseMuPlusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuPlusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);

		



		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuPlusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuPlusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuPlusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuPlusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuPlusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuPlusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuPlusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuPlusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuPlusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuPlusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		}
		
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuPlusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuPlusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuPlusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuPlusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuPlusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuPlusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuPlusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuPlusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuPlusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		  h_looseMuPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuPlusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		  
		  
		  
		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuPlusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuPlusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuPlusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuPlusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuPlusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuPlusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuPlusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuPlusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuPlusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuPlusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		}
		
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_looseMuPlusChi2EtaMinus->Fill(  *upTrackerChi2);
		h_looseMuPlusNdofEtaMinus->Fill(  *upTrackerNdof);
		h_looseMuPlusChargeEtaMinus->Fill(*upTrackerCharge);
		h_looseMuPlusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuPlusDxyEtaMinus->Fill(     *upTrackerDxy);
		h_looseMuPlusDzEtaMinus->Fill(      *upTrackerDz);
		h_looseMuPlusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
		h_looseMuPlusDzErrorEtaMinus->Fill( *upTrackerDzError);
		
		h_looseMuPlusPtEtaMinus->Fill( upTrackerMuonP4->pt());
		h_looseMuPlusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
		h_looseMuPlusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuPlusPtErrorEtaMinus->Fill( *upTrackerPtError);
		h_looseMuPlusPtRelErrEtaMinus->Fill( upperRelPtErr);
		h_looseMuPlusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuPlusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
		h_looseMuPlusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
		
		h_looseMuPlusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
		h_looseMuPlusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
		h_looseMuPlusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
		h_looseMuPlusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
		h_looseMuPlusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
		h_looseMuPlusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuPlusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);

	


		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuPlusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuPlusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuPlusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuPlusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuPlusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuPlusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuPlusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuPlusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuPlusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuPlusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		}

		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuPlusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuPlusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuPlusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuPlusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuPlusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuPlusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuPlusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuPlusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		
		  h_looseMuPlusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuPlusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		  h_looseMuPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuPlusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		  


		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuPlusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuPlusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuPlusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuPlusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuPlusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuPlusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuPlusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuPlusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuPlusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuPlusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		  
		}
		
		
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
	      h_looseMuPlusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      
	      
	      
	      h_looseMuUpperPlusCurve->Fill(symmetric_?upperCpT:fabs(upperCpT));
	      if (upTrackerTrack->eta() > 0){
		h_looseMuUpperPlusChi2EtaPlus->Fill(  *upTrackerChi2);
		h_looseMuUpperPlusNdofEtaPlus->Fill(  *upTrackerNdof);
		h_looseMuUpperPlusChargeEtaPlus->Fill(*upTrackerCharge);
		h_looseMuUpperPlusCurveEtaPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuUpperPlusDxyEtaPlus->Fill(     *upTrackerDxy);
		h_looseMuUpperPlusDzEtaPlus->Fill(      *upTrackerDz);
		h_looseMuUpperPlusDxyErrorEtaPlus->Fill(*upTrackerDxyError);
		h_looseMuUpperPlusDzErrorEtaPlus->Fill( *upTrackerDzError);
		
		h_looseMuUpperPlusPtEtaPlus->Fill( upTrackerMuonP4->pt());
		h_looseMuUpperPlusEtaEtaPlus->Fill(upTrackerMuonP4->eta());
		h_looseMuUpperPlusPhiEtaPlus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuUpperPlusPtErrorEtaPlus->Fill( *upTrackerPtError);
		h_looseMuUpperPlusPtRelErrEtaPlus->Fill( upperRelPtErr);
		h_looseMuUpperPlusTrackPtEtaPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuUpperPlusTrackEtaEtaPlus->Fill( upTrackerTrack->eta());
		h_looseMuUpperPlusTrackPhiEtaPlus->Fill( upTrackerTrack->phi());
		
		h_looseMuUpperPlusPixelHitsEtaPlus->Fill(                   *upTrackerPhits);
		h_looseMuUpperPlusTrackerHitsEtaPlus->Fill(                 *upTrackerThits);
		h_looseMuUpperPlusValidHitsEtaPlus->Fill(                   *upTrackerValidHits);
		h_looseMuUpperPlusValidMuonHitsEtaPlus->Fill(               *upTrackerValidMuonHits);
		h_looseMuUpperPlusMuonStationHitsEtaPlus->Fill(             *upTrackerMhits);
		h_looseMuUpperPlusMatchedMuonStationsEtaPlus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlus->Fill(*upTrackerLayersWithMeasurement);



		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuUpperPlusChi2EtaPlusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuUpperPlusNdofEtaPlusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuUpperPlusChargeEtaPlusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuUpperPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperPlusDxyEtaPlusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuUpperPlusDzEtaPlusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuUpperPlusDxyErrorEtaPlusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuUpperPlusDzErrorEtaPlusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperPlusPtEtaPlusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperPlusEtaEtaPlusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperPlusPhiEtaPlusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperPlusPtErrorEtaPlusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuUpperPlusPtRelErrEtaPlusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuUpperPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperPlusTrackEtaEtaPlusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperPlusTrackPhiEtaPlusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperPlusPixelHitsEtaPlusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperPlusTrackerHitsEtaPlusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuUpperPlusValidHitsEtaPlusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuUpperPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		  
		}
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperPlusChi2EtaPlusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuUpperPlusNdofEtaPlusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuUpperPlusChargeEtaPlusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuUpperPlusCurveEtaPlusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperPlusDxyEtaPlusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuUpperPlusDzEtaPlusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuUpperPlusDxyErrorEtaPlusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuUpperPlusDzErrorEtaPlusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperPlusPtEtaPlusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperPlusEtaEtaPlusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperPlusPhiEtaPlusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperPlusPtErrorEtaPlusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuUpperPlusPtRelErrEtaPlusPhiZero->Fill( upperRelPtErr);
		  h_looseMuUpperPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperPlusTrackEtaEtaPlusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuUpperPlusTrackPhiEtaPlusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperPlusPixelHitsEtaPlusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuUpperPlusTrackerHitsEtaPlusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuUpperPlusValidHitsEtaPlusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperPlusValidMuonHitsEtaPlusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperPlusMuonStationHitsEtaPlusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuUpperPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		  
		  
		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuUpperPlusChi2EtaPlusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuUpperPlusNdofEtaPlusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuUpperPlusChargeEtaPlusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuUpperPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperPlusDxyEtaPlusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuUpperPlusDzEtaPlusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuUpperPlusDxyErrorEtaPlusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuUpperPlusDzErrorEtaPlusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperPlusPtEtaPlusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperPlusEtaEtaPlusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperPlusPhiEtaPlusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperPlusPtErrorEtaPlusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuUpperPlusPtRelErrEtaPlusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuUpperPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperPlusTrackEtaEtaPlusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperPlusTrackPhiEtaPlusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperPlusPixelHitsEtaPlusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperPlusTrackerHitsEtaPlusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuUpperPlusValidHitsEtaPlusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuUpperPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		}
		
	      }
	      
	      
	      else if (upTrackerTrack->eta() < 0){
		h_looseMuUpperPlusChi2EtaMinus->Fill(  *upTrackerChi2);
		h_looseMuUpperPlusNdofEtaMinus->Fill(  *upTrackerNdof);
		h_looseMuUpperPlusChargeEtaMinus->Fill(*upTrackerCharge);
		h_looseMuUpperPlusCurveEtaMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		h_looseMuUpperPlusDxyEtaMinus->Fill(     *upTrackerDxy);
		h_looseMuUpperPlusDzEtaMinus->Fill(      *upTrackerDz);
		h_looseMuUpperPlusDxyErrorEtaMinus->Fill(*upTrackerDxyError);
		h_looseMuUpperPlusDzErrorEtaMinus->Fill( *upTrackerDzError);
		
		h_looseMuUpperPlusPtEtaMinus->Fill( upTrackerMuonP4->pt());
		h_looseMuUpperPlusEtaEtaMinus->Fill(upTrackerMuonP4->eta());
		h_looseMuUpperPlusPhiEtaMinus->Fill(upTrackerMuonP4->phi());
		
		h_looseMuUpperPlusPtErrorEtaMinus->Fill( *upTrackerPtError);
		h_looseMuUpperPlusPtRelErrEtaMinus->Fill( upperRelPtErr);
		h_looseMuUpperPlusTrackPtEtaMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		h_looseMuUpperPlusTrackEtaEtaMinus->Fill( upTrackerTrack->eta());
		h_looseMuUpperPlusTrackPhiEtaMinus->Fill( upTrackerTrack->phi());
		
		h_looseMuUpperPlusPixelHitsEtaMinus->Fill(                   *upTrackerPhits);
		h_looseMuUpperPlusTrackerHitsEtaMinus->Fill(                 *upTrackerThits);
		h_looseMuUpperPlusValidHitsEtaMinus->Fill(                   *upTrackerValidHits);
		h_looseMuUpperPlusValidMuonHitsEtaMinus->Fill(               *upTrackerValidMuonHits);
		h_looseMuUpperPlusMuonStationHitsEtaMinus->Fill(             *upTrackerMhits);
		h_looseMuUpperPlusMatchedMuonStationsEtaMinus->Fill(         *upTrackerMatchedMuonStations);
		h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinus->Fill(*upTrackerLayersWithMeasurement);
	


		if(upTrackerTrack->phi() < -1.0472){
		  h_looseMuUpperPlusChi2EtaMinusPhiMinus->Fill(  *upTrackerChi2);
		  h_looseMuUpperPlusNdofEtaMinusPhiMinus->Fill(  *upTrackerNdof);
		  h_looseMuUpperPlusChargeEtaMinusPhiMinus->Fill(*upTrackerCharge);
		  h_looseMuUpperPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperPlusDxyEtaMinusPhiMinus->Fill(     *upTrackerDxy);
		  h_looseMuUpperPlusDzEtaMinusPhiMinus->Fill(      *upTrackerDz);
		  h_looseMuUpperPlusDxyErrorEtaMinusPhiMinus->Fill(*upTrackerDxyError);
		  h_looseMuUpperPlusDzErrorEtaMinusPhiMinus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperPlusPtEtaMinusPhiMinus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperPlusEtaEtaMinusPhiMinus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperPlusPhiEtaMinusPhiMinus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperPlusPtErrorEtaMinusPhiMinus->Fill( *upTrackerPtError);
		  h_looseMuUpperPlusPtRelErrEtaMinusPhiMinus->Fill( upperRelPtErr);
		  h_looseMuUpperPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperPlusTrackEtaEtaMinusPhiMinus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperPlusTrackPhiEtaMinusPhiMinus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperPlusPixelHitsEtaMinusPhiMinus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperPlusTrackerHitsEtaMinusPhiMinus->Fill(                 *upTrackerThits);
		  h_looseMuUpperPlusValidHitsEtaMinusPhiMinus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *upTrackerMhits);
		  h_looseMuUpperPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*upTrackerLayersWithMeasurement);
		  
		}
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperPlusChi2EtaMinusPhiZero->Fill(  *upTrackerChi2);
		  h_looseMuUpperPlusNdofEtaMinusPhiZero->Fill(  *upTrackerNdof);
		  h_looseMuUpperPlusChargeEtaMinusPhiZero->Fill(*upTrackerCharge);
		  h_looseMuUpperPlusCurveEtaMinusPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperPlusDxyEtaMinusPhiZero->Fill(     *upTrackerDxy);
		  h_looseMuUpperPlusDzEtaMinusPhiZero->Fill(      *upTrackerDz);
		  h_looseMuUpperPlusDxyErrorEtaMinusPhiZero->Fill(*upTrackerDxyError);
		  h_looseMuUpperPlusDzErrorEtaMinusPhiZero->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperPlusPtEtaMinusPhiZero->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperPlusEtaEtaMinusPhiZero->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperPlusPhiEtaMinusPhiZero->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperPlusPtErrorEtaMinusPhiZero->Fill( *upTrackerPtError);
		  h_looseMuUpperPlusPtRelErrEtaMinusPhiZero->Fill( upperRelPtErr);
		  h_looseMuUpperPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperPlusTrackEtaEtaMinusPhiZero->Fill( upTrackerTrack->eta());
		  h_looseMuUpperPlusTrackPhiEtaMinusPhiZero->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperPlusPixelHitsEtaMinusPhiZero->Fill(                   *upTrackerPhits);
		  h_looseMuUpperPlusTrackerHitsEtaMinusPhiZero->Fill(                 *upTrackerThits);
		  h_looseMuUpperPlusValidHitsEtaMinusPhiZero->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperPlusValidMuonHitsEtaMinusPhiZero->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperPlusMuonStationHitsEtaMinusPhiZero->Fill(             *upTrackerMhits);
		  h_looseMuUpperPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*upTrackerLayersWithMeasurement);
		  
		  
		  
		}
		else if(upTrackerTrack->phi() > 1.0472){
		  h_looseMuUpperPlusChi2EtaMinusPhiPlus->Fill(  *upTrackerChi2);
		  h_looseMuUpperPlusNdofEtaMinusPhiPlus->Fill(  *upTrackerNdof);
		  h_looseMuUpperPlusChargeEtaMinusPhiPlus->Fill(*upTrackerCharge);
		  h_looseMuUpperPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
		  h_looseMuUpperPlusDxyEtaMinusPhiPlus->Fill(     *upTrackerDxy);
		  h_looseMuUpperPlusDzEtaMinusPhiPlus->Fill(      *upTrackerDz);
		  h_looseMuUpperPlusDxyErrorEtaMinusPhiPlus->Fill(*upTrackerDxyError);
		  h_looseMuUpperPlusDzErrorEtaMinusPhiPlus->Fill( *upTrackerDzError);
		  
		  h_looseMuUpperPlusPtEtaMinusPhiPlus->Fill( upTrackerMuonP4->pt());
		  h_looseMuUpperPlusEtaEtaMinusPhiPlus->Fill(upTrackerMuonP4->eta());
		  h_looseMuUpperPlusPhiEtaMinusPhiPlus->Fill(upTrackerMuonP4->phi());
		  
		  h_looseMuUpperPlusPtErrorEtaMinusPhiPlus->Fill( *upTrackerPtError);
		  h_looseMuUpperPlusPtRelErrEtaMinusPhiPlus->Fill( upperRelPtErr);
		  h_looseMuUpperPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(upTrackerTrack->perp2()));
		  h_looseMuUpperPlusTrackEtaEtaMinusPhiPlus->Fill( upTrackerTrack->eta());
		  h_looseMuUpperPlusTrackPhiEtaMinusPhiPlus->Fill( upTrackerTrack->phi());
		  
		  h_looseMuUpperPlusPixelHitsEtaMinusPhiPlus->Fill(                   *upTrackerPhits);
		  h_looseMuUpperPlusTrackerHitsEtaMinusPhiPlus->Fill(                 *upTrackerThits);
		  h_looseMuUpperPlusValidHitsEtaMinusPhiPlus->Fill(                   *upTrackerValidHits);
		  h_looseMuUpperPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *upTrackerValidMuonHits);
		  h_looseMuUpperPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *upTrackerMhits);
		  h_looseMuUpperPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *upTrackerMatchedMuonStations);
		  h_looseMuUpperPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*upTrackerLayersWithMeasurement);
		  
		}
	      }
	      if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaAllPhiMinus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		h_looseMuUpperPlusCurveEtaAllPhiZero->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      }
	      else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaAllPhiPlus->Fill(  symmetric_?upperCpT:fabs(upperCpT));
	      
	      
	      

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuPlusDxy->Fill(     *upTrackerDxy);
	      h_looseMuUpperPlusDxy->Fill(*upTrackerDxy);
	      h_looseMuPlusDz->Fill(      *upTrackerDz);
	      h_looseMuUpperPlusDz->Fill( *upTrackerDz);
	      h_looseMuPlusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuUpperPlusDxyError->Fill(*upTrackerDxyError);
	      h_looseMuPlusDzError->Fill(      *upTrackerDzError);
	      h_looseMuUpperPlusDzError->Fill( *upTrackerDzError);

	      h_looseMuPlusPt->Fill(      upTrackerMuonP4->pt());
	      h_looseMuUpperPlusPt->Fill( upTrackerMuonP4->pt());
	      h_looseMuPlusEta->Fill(     upTrackerMuonP4->eta());
	      h_looseMuUpperPlusEta->Fill(upTrackerMuonP4->eta());
	      h_looseMuPlusPhi->Fill(     upTrackerMuonP4->phi());
	      h_looseMuUpperPlusPhi->Fill(upTrackerMuonP4->phi());

	      h_looseMuPlusTrackPt->Fill(     sqrt(upTrackerTrack->perp2()));
	      h_looseMuUpperPlusTrackPt->Fill(sqrt(upTrackerTrack->perp2()));
	      h_looseMuPlusTrackEta->Fill(         upTrackerTrack->eta());
	      h_looseMuUpperPlusTrackEta->Fill(    upTrackerTrack->eta());
	      h_looseMuPlusTrackPhi->Fill(         upTrackerTrack->phi());
	      h_looseMuUpperPlusTrackPhi->Fill(    upTrackerTrack->phi());

	      h_looseMuPlusValidHits->Fill(     *upTrackerValidHits);
	      h_looseMuUpperPlusValidHits->Fill(*upTrackerValidHits);

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = upperCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = upperCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuPlusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
		if (upTrackerTrack->eta() > 0){
		  h_looseMuPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		
		h_looseMuPlusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));
		if (upTrackerTrack->eta() > 0){
		  h_looseMuPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		h_looseMuUpperPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		if (upTrackerTrack->eta() > 0){
		  h_looseMuUpperPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuUpperPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuUpperPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
		if (upTrackerTrack->eta() > 0){
		  h_looseMuUpperPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
		else if (upTrackerTrack->eta() < 0){
		  h_looseMuUpperPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		    h_looseMuUpperPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		if(upTrackerTrack->phi() < -1.0472) h_looseMuUpperPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= upTrackerTrack->phi()) && (upTrackerTrack->phi() <= 1.0472)){
		  h_looseMuUpperPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(upTrackerTrack->phi() > 1.0472) h_looseMuUpperPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
	    
		if (debug)
		  std::cout << "Made it through the upper bias loop " << i << std::endl; 
	      }
	    } // end if (up_n1pt)
	
	    if (up_n1pixhits) {
	      h_looseMuPlusPixelHits->Fill(     *upTrackerPhits);
	      h_looseMuUpperPlusPixelHits->Fill(*upTrackerPhits);
	    }
	    if (up_n1vmuhits) {
	      h_looseMuPlusValidMuonHits->Fill(       *upTrackerValidMuonHits);
	      h_looseMuUpperPlusValidMuonHits->Fill(  *upTrackerValidMuonHits);
	      h_looseMuPlusMuonStationHits->Fill(     *upTrackerMhits);
	      h_looseMuUpperPlusMuonStationHits->Fill(*upTrackerMhits);
	    }
	    if (up_n1ptrelerr) {
	      h_looseMuPlusPtError->Fill(     *upTrackerPtError);
	      h_looseMuUpperPlusPtError->Fill(*upTrackerPtError);
	      h_looseMuPlusPtRelErr->Fill(     upperRelPtErr);
	      h_looseMuUpperPlusPtRelErr->Fill(upperRelPtErr);
	    }
	    if (up_n1trkhits) {
	      h_looseMuPlusTrackerHits->Fill(     *upTrackerThits);
	      h_looseMuUpperPlusTrackerHits->Fill(*upTrackerThits);
	      h_looseMuPlusTrackerLayersWithMeasurement->Fill(     *upTrackerLayersWithMeasurement);
	      h_looseMuUpperPlusTrackerLayersWithMeasurement->Fill(*upTrackerLayersWithMeasurement);
	    }
	    if (up_n1mmustahits) {
	      h_looseMuPlusMatchedMuonStations->Fill(     *upTrackerMatchedMuonStations);
	      h_looseMuUpperPlusMatchedMuonStations->Fill(*upTrackerMatchedMuonStations);
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
	//uint32_t lowerCuts; // 1 bit per cut
      
	bool low_etabar   = (fabs(lowTrackerMuonP4->eta()) < 0.9) ? 1 : 0;
	bool low_tightdxy = (*lowTrackerDxy < 0.2) ? 1 : 0;
	bool low_tightdz  = (*lowTrackerDz  < 0.5) ? 1 : 0;
	bool low_etaBar   = (fabs(lowTrackerMuonP4->eta()) < 0.9) ? 1 : 0;
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

	h_lowerChi2->Fill(  *lowTrackerChi2);
	h_lowerNdof->Fill(  *lowTrackerNdof);
	h_lowerCharge->Fill(*lowTrackerCharge);
	h_lowerCurve->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	if (lowTrackerTrack->eta() > 0){
	  h_lowerChi2EtaPlus->Fill(  *lowTrackerChi2);
	  h_lowerNdofEtaPlus->Fill(  *lowTrackerNdof);
	  h_lowerChargeEtaPlus->Fill(*lowTrackerCharge);
	  h_lowerCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	  h_lowerDxyEtaPlus->Fill(     *lowTrackerDxy);
	  h_lowerDzEtaPlus->Fill(      *lowTrackerDz);
	  h_lowerDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
	  h_lowerDzErrorEtaPlus->Fill( *lowTrackerDzError);
	  
	  h_lowerPtEtaPlus->Fill( lowTrackerMuonP4->pt());
	  h_lowerEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
	  h_lowerPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
	  
	  h_lowerPtErrorEtaPlus->Fill( *lowTrackerPtError);
	  h_lowerPtRelErrEtaPlus->Fill( lowerRelPtErr);
	  h_lowerTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
	  h_lowerTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
	  h_lowerTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());

	  h_lowerPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
	  h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	  h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	  h_lowerValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
	  h_lowerMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
	  h_lowerMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
	  h_lowerTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);



	  if(lowTrackerTrack->phi() < -1.0472){
	    h_lowerChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
	    h_lowerNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
	    h_lowerChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
	    h_lowerCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    h_lowerDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
	    h_lowerDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
	    h_lowerDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
	    h_lowerDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
	    
	    h_lowerPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
	    h_lowerEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
	    h_lowerPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
	    
	    h_lowerPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
	    h_lowerPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
	    h_lowerTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	    h_lowerTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
	    h_lowerTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
	    
	    h_lowerPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
	    h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	    h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	    h_lowerValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
	    h_lowerMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
	    h_lowerMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
	    h_lowerTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	    
	  }
	  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	    h_lowerChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
	    h_lowerNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
	    h_lowerChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
	    h_lowerCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    h_lowerDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
	    h_lowerDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
	    h_lowerDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
	    h_lowerDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
	    
	    h_lowerPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
	    h_lowerEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
	    h_lowerPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
	  
	    h_lowerPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
	    h_lowerPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
	    h_lowerTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
	    h_lowerTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
	    h_lowerTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
	    
	    h_lowerPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
	    h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	    h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	    h_lowerValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
	    h_lowerMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
	    h_lowerMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
	    h_lowerTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
	    
	  }
	  else if(lowTrackerTrack->phi() > 1.0472){
	    h_lowerChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
	    h_lowerNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
	    h_lowerChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
	    h_lowerCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    h_lowerDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
	    h_lowerDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
	    h_lowerDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
	    h_lowerDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
	    
	    h_lowerPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
	    h_lowerEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
	    h_lowerPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
	    
	    h_lowerPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
	    h_lowerPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
	    h_lowerTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
	    h_lowerTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
	    h_lowerTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
	    
	    h_lowerPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
	    h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	    h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	    h_lowerValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
	    h_lowerMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
	    h_lowerMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
	    h_lowerTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
	  }
	  
	}
	
	
	else if (lowTrackerTrack->eta() < 0){
	  h_lowerChi2EtaMinus->Fill(  *lowTrackerChi2);
	  h_lowerNdofEtaMinus->Fill(  *lowTrackerNdof);
	  h_lowerChargeEtaMinus->Fill(*lowTrackerCharge);
	  h_lowerCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	  h_lowerDxyEtaMinus->Fill(     *lowTrackerDxy);
	  h_lowerDzEtaMinus->Fill(      *lowTrackerDz);
	  h_lowerDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
	  h_lowerDzErrorEtaMinus->Fill( *lowTrackerDzError);
	  
	  h_lowerPtEtaMinus->Fill( lowTrackerMuonP4->pt());
	  h_lowerEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
	  h_lowerPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
	  
	  h_lowerPtErrorEtaMinus->Fill( *lowTrackerPtError);
	  h_lowerPtRelErrEtaMinus->Fill( lowerRelPtErr);
	  h_lowerTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	  h_lowerTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
	  h_lowerTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());

	  h_lowerPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
	  h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	  h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	  h_lowerValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
	  h_lowerMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
	  h_lowerMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
	  h_lowerTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);

	 


	  if(lowTrackerTrack->phi() < -1.0472){
	    h_lowerChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
	    h_lowerNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
	    h_lowerChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
	    h_lowerCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    h_lowerDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
	    h_lowerDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
	    h_lowerDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
	    h_lowerDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
	    
	    h_lowerPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
	    h_lowerEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
	    h_lowerPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
	    
	    h_lowerPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
	    h_lowerPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
	    h_lowerTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	    h_lowerTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
	    h_lowerTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
	    
	    h_lowerPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
	    h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	    h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	    h_lowerValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
	    h_lowerMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
	    h_lowerMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
	    h_lowerTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	    
	  }
	  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	    h_lowerChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
	    h_lowerNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
	    h_lowerChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
	    h_lowerCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    h_lowerDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
	    h_lowerDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
	    h_lowerDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
	    h_lowerDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
	    
	    h_lowerPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
	    h_lowerEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
	    h_lowerPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
	    
	    h_lowerPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
	    h_lowerPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
	    h_lowerTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
	    h_lowerTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
	    h_lowerTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
	    
	    h_lowerPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
	    h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	    h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	    h_lowerValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
	    h_lowerMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
	    h_lowerMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
	    h_lowerTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
	  }
	  else if(lowTrackerTrack->phi() > 1.0472){
	    h_lowerChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
	    h_lowerNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
	    h_lowerChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
	    h_lowerCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    h_lowerDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
	    h_lowerDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
	    h_lowerDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
	    h_lowerDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
	    
	    h_lowerPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
	    h_lowerEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
	    h_lowerPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
	    
	    h_lowerPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
	    h_lowerPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
	    h_lowerTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
	    h_lowerTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
	    h_lowerTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
	    
	    h_lowerPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
	    h_lowerTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	    h_lowerValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	    h_lowerValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
	    h_lowerMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
	    h_lowerMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
	    h_lowerTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
	  }
	  
	}
	if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)) h_lowerCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	
	
	
	h_lowerDxy->Fill(     *lowTrackerDxy);
	h_lowerDz->Fill(      *lowTrackerDz);
	h_lowerDxyError->Fill(*lowTrackerDxyError);
	h_lowerDzError->Fill( *lowTrackerDzError);

	h_lowerPt->Fill( lowTrackerMuonP4->pt());
	h_lowerEta->Fill(lowTrackerMuonP4->eta());
	h_lowerPhi->Fill(lowTrackerMuonP4->phi());

	h_lowerPtError->Fill( *lowTrackerPtError);
	h_lowerPtRelErr->Fill( lowerRelPtErr);
	h_lowerTrackPt->Fill(  sqrt(lowTrackerTrack->perp2()));
	h_lowerTrackEta->Fill( lowTrackerTrack->eta());
	h_lowerTrackPhi->Fill( lowTrackerTrack->phi());

	lowerstring << "pt="      << sqrt(lowTrackerTrack->perp2())
		    << ",eta="    << lowTrackerTrack->eta()
		    << ",phi="    << lowTrackerTrack->phi()
		    << ",charge=" << *lowTrackerCharge;

	h_lowerPixelHits->Fill(                   *lowTrackerPhits);
	h_lowerTrackerHits->Fill(                 *lowTrackerThits);
	h_lowerValidHits->Fill(                   *lowTrackerValidHits);
	h_lowerValidMuonHits->Fill(               *lowTrackerValidMuonHits);
	h_lowerMuonStationHits->Fill(             *lowTrackerMhits);
	h_lowerMatchedMuonStations->Fill(         *lowTrackerMatchedMuonStations);
	h_lowerTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

	for (int i = 0; i < nBiasBins; ++i) {
	  double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
	  double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
	  h_lowerCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	  if (lowTrackerTrack->eta() > 0){
	    h_lowerCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	      h_lowerCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    }
	    else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  }
	  
	  
	  else if (lowTrackerTrack->eta() < 0){
	    h_lowerCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	      h_lowerCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	    }
	    else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  }
	  if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	    h_lowerCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  }
	  else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	  
	  
	  h_lowerCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	  if (lowTrackerTrack->eta() > 0){
	    h_lowerCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	      h_lowerCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    }
	    else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  }
	  
	  
	  else if (lowTrackerTrack->eta() < 0){
	    h_lowerCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	      h_lowerCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	    }
	    else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  }
	  if(lowTrackerTrack->phi() < -1.0472) h_lowerCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	    h_lowerCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  }
	  else if(lowTrackerTrack->phi() > 1.0472) h_lowerCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  

	  if (debug)
	    std::cout << "Made it through the lower bias loop " << i << std::endl; 
	}

	// fill the counters histogram for the lower leg muons passing the super-pointing selection
	if (low_superpointing && low_etabar) {
	//if (low_superpointing || true) {

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
	    h_muMinusChi2->Fill(       *lowTrackerChi2);
	    h_muLowerMinusChi2->Fill(  *lowTrackerChi2);
	    h_muMinusNdof->Fill(       *lowTrackerNdof);
	    h_muLowerMinusNdof->Fill(  *lowTrackerNdof);
	    h_muMinusCharge->Fill(     *lowTrackerCharge);
	    h_muLowerMinusCharge->Fill(*lowTrackerCharge);
	    h_muMinusCurve->Fill(     symmetric_?lowerCpT:fabs(lowerCpT));
	    if (lowTrackerTrack->eta() > 0){
	      h_muMinusChi2EtaPlus->Fill(  *lowTrackerChi2);
	      h_muMinusNdofEtaPlus->Fill(  *lowTrackerNdof);
	      h_muMinusChargeEtaPlus->Fill(*lowTrackerCharge);
	      h_muMinusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muMinusDxyEtaPlus->Fill(     *lowTrackerDxy);
	      h_muMinusDzEtaPlus->Fill(      *lowTrackerDz);
	      h_muMinusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
	      h_muMinusDzErrorEtaPlus->Fill( *lowTrackerDzError);
	      
	      h_muMinusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
	      h_muMinusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
	      h_muMinusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muMinusPtErrorEtaPlus->Fill( *lowTrackerPtError);
	      h_muMinusPtRelErrEtaPlus->Fill( lowerRelPtErr);
	      h_muMinusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muMinusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
	      h_muMinusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
	      
	      h_muMinusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
	      h_muMinusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	      h_muMinusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muMinusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
	      h_muMinusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
	      h_muMinusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muMinusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
	      
	      
	      
	      if(lowTrackerTrack->phi() < -1.0472){
		h_muMinusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		h_muMinusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		h_muMinusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		h_muMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muMinusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		h_muMinusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		h_muMinusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		h_muMinusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muMinusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muMinusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muMinusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		h_muMinusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		h_muMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muMinusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
	    
		h_muMinusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *lowTrackerThits);
		h_muMinusValidHitsEtaPlusPhiMinus->Fill(                   *lowTrackerValidHits);
		h_muMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		h_muMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	      
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muMinusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		h_muMinusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		h_muMinusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		h_muMinusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muMinusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		h_muMinusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		h_muMinusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		h_muMinusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
	    
		h_muMinusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muMinusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muMinusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
	    
		h_muMinusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		h_muMinusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		h_muMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		h_muMinusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		h_muMinusTrackerHitsEtaPlusPhiZero->Fill(                 *lowTrackerThits);
		h_muMinusValidHitsEtaPlusPhiZero->Fill(                   *lowTrackerValidHits);
		h_muMinusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		h_muMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		
		
	  }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muMinusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		h_muMinusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		h_muMinusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		h_muMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muMinusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		h_muMinusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		h_muMinusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		h_muMinusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muMinusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muMinusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muMinusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		h_muMinusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		h_muMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muMinusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *lowTrackerThits);
		h_muMinusValidHitsEtaPlusPhiPlus->Fill(                   *lowTrackerValidHits);
		h_muMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		h_muMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		
	      }
	    }
	    
	    else if (lowTrackerTrack->eta() < 0){
	      h_muMinusChi2EtaMinus->Fill(  *lowTrackerChi2);
	      h_muMinusNdofEtaMinus->Fill(  *lowTrackerNdof);
	      h_muMinusChargeEtaMinus->Fill(*lowTrackerCharge);
	      h_muMinusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muMinusDxyEtaMinus->Fill(     *lowTrackerDxy);
	      h_muMinusDzEtaMinus->Fill(      *lowTrackerDz);
	      h_muMinusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
	      h_muMinusDzErrorEtaMinus->Fill( *lowTrackerDzError);
	      
	      h_muMinusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
	      h_muMinusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
	      h_muMinusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muMinusPtErrorEtaMinus->Fill( *lowTrackerPtError);
	      h_muMinusPtRelErrEtaMinus->Fill( lowerRelPtErr);
	      h_muMinusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muMinusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
	      h_muMinusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
	      
	      h_muMinusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
	      h_muMinusTrackerHitsEtaMinus->Fill(                 *lowTrackerThits);
	      h_muMinusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muMinusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
	      h_muMinusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
	      h_muMinusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muMinusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);

	     

	      
	      if(lowTrackerTrack->phi() < -1.0472){
		h_muMinusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		h_muMinusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		h_muMinusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		h_muMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muMinusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		h_muMinusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		h_muMinusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		h_muMinusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muMinusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muMinusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muMinusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		h_muMinusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		h_muMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muMinusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muMinusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muMinusValidHitsEtaMinusPhiMinus->Fill(                   *lowTrackerValidHits);
		h_muMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		h_muMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		
	      }	     	     
	      
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		
		h_muMinusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		h_muMinusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		h_muMinusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		h_muMinusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muMinusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		h_muMinusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		h_muMinusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		h_muMinusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		
		h_muMinusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muMinusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muMinusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		h_muMinusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		h_muMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		h_muMinusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		h_muMinusTrackerHitsEtaMinusPhiZero->Fill(                 *lowTrackerThits);
		h_muMinusValidHitsEtaMinusPhiZero->Fill(                   *lowTrackerValidHits);
		h_muMinusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		h_muMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		
	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muMinusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		h_muMinusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		h_muMinusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		h_muMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muMinusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		h_muMinusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		h_muMinusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		h_muMinusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muMinusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muMinusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muMinusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muMinusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		h_muMinusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		h_muMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muMinusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muMinusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muMinusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *lowTrackerThits);
		h_muMinusValidHitsEtaMinusPhiPlus->Fill(                   *lowTrackerValidHits);
		h_muMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		h_muMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	      
	    }
	    if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)) h_muMinusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    
	    

	    
	    h_muLowerMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	    

	    if (lowTrackerTrack->eta() > 0){
	      h_muLowerMinusChi2EtaPlus->Fill(  *lowTrackerChi2);
	      h_muLowerMinusNdofEtaPlus->Fill(  *lowTrackerNdof);
	      h_muLowerMinusChargeEtaPlus->Fill(*lowTrackerCharge);
	      h_muLowerMinusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muLowerMinusDxyEtaPlus->Fill(     *lowTrackerDxy);
	      h_muLowerMinusDzEtaPlus->Fill(      *lowTrackerDz);
	      h_muLowerMinusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
	      h_muLowerMinusDzErrorEtaPlus->Fill( *lowTrackerDzError);
	      
	      h_muLowerMinusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
	      h_muLowerMinusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
	      h_muLowerMinusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muLowerMinusPtErrorEtaPlus->Fill( *lowTrackerPtError);
	      h_muLowerMinusPtRelErrEtaPlus->Fill( lowerRelPtErr);
	      h_muLowerMinusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muLowerMinusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
	      h_muLowerMinusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
	      
	      h_muLowerMinusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
	      h_muLowerMinusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	      h_muLowerMinusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muLowerMinusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
	      h_muLowerMinusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
	      h_muLowerMinusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muLowerMinusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
	      
	      
	      
	      if(lowTrackerTrack->phi() < -1.0472){
		h_muLowerMinusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		h_muLowerMinusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		h_muLowerMinusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		h_muLowerMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerMinusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		h_muLowerMinusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		h_muLowerMinusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		h_muLowerMinusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muLowerMinusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muLowerMinusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muLowerMinusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerMinusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		h_muLowerMinusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		h_muLowerMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerMinusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muLowerMinusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		
		h_muLowerMinusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muLowerMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *lowTrackerThits);
		h_muLowerMinusValidHitsEtaPlusPhiMinus->Fill(                   *lowTrackerValidHits);
		h_muLowerMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		h_muLowerMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerMinusChi2EtaPlus->Fill(  *lowTrackerChi2);
		h_muLowerMinusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		h_muLowerMinusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		h_muLowerMinusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerMinusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		h_muLowerMinusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		h_muLowerMinusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		h_muLowerMinusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
		
		h_muLowerMinusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muLowerMinusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muLowerMinusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerMinusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		h_muLowerMinusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		h_muLowerMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerMinusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		h_muLowerMinusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muLowerMinusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		h_muLowerMinusTrackerHitsEtaPlusPhiZero->Fill(                 *lowTrackerThits);
		h_muLowerMinusValidHitsEtaPlusPhiZero->Fill(                   *lowTrackerValidHits);
		h_muLowerMinusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muLowerMinusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		h_muLowerMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		

	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muLowerMinusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		h_muLowerMinusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		h_muLowerMinusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		h_muLowerMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerMinusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		h_muLowerMinusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		h_muLowerMinusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		h_muLowerMinusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muLowerMinusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muLowerMinusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muLowerMinusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerMinusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		h_muLowerMinusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		h_muLowerMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerMinusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muLowerMinusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muLowerMinusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muLowerMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *lowTrackerThits);
		h_muLowerMinusValidHitsEtaPlusPhiPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		h_muLowerMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		
	      }
	    }
	    
	    
	    else if (lowTrackerTrack->eta() < 0){
	      h_muLowerMinusChi2EtaMinus->Fill(  *lowTrackerChi2);
	      h_muLowerMinusNdofEtaMinus->Fill(  *lowTrackerNdof);
	      h_muLowerMinusChargeEtaMinus->Fill(*lowTrackerCharge);
	      h_muLowerMinusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muLowerMinusDxyEtaMinus->Fill(     *lowTrackerDxy);
	      h_muLowerMinusDzEtaMinus->Fill(      *lowTrackerDz);
	      h_muLowerMinusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
	      h_muLowerMinusDzErrorEtaMinus->Fill( *lowTrackerDzError);
	      
	      h_muLowerMinusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
	      h_muLowerMinusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
	      h_muLowerMinusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muLowerMinusPtErrorEtaMinus->Fill( *lowTrackerPtError);
	      h_muLowerMinusPtRelErrEtaMinus->Fill( lowerRelPtErr);
	      h_muLowerMinusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muLowerMinusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
	      h_muLowerMinusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
	      
	      h_muLowerMinusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
	      h_muLowerMinusTrackerHitsEtaMinus->Fill(                 *lowTrackerThits);
	      h_muLowerMinusValidHitsEtaMinus->Fill(                   *lowTrackerValidHits);
	      h_muLowerMinusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
	      h_muLowerMinusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
	      h_muLowerMinusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muLowerMinusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);
	      
	      
	      if(lowTrackerTrack->phi() < -1.0472){
		h_muLowerMinusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		h_muLowerMinusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		h_muLowerMinusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		h_muLowerMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerMinusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		h_muLowerMinusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		h_muLowerMinusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		h_muLowerMinusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muLowerMinusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muLowerMinusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muLowerMinusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerMinusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		h_muLowerMinusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		h_muLowerMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerMinusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muLowerMinusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		
		h_muLowerMinusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muLowerMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *lowTrackerThits);
		h_muLowerMinusValidHitsEtaPlusPhiMinus->Fill(                   *lowTrackerValidHits);
		h_muLowerMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		h_muLowerMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }	    
	      
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerMinusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		h_muLowerMinusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		h_muLowerMinusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		h_muLowerMinusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerMinusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		h_muLowerMinusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		h_muLowerMinusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		h_muLowerMinusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
	    
		h_muLowerMinusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muLowerMinusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muLowerMinusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerMinusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		h_muLowerMinusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		h_muLowerMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerMinusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		h_muLowerMinusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muLowerMinusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		h_muLowerMinusTrackerHitsEtaPlusPhiZero->Fill(                 *lowTrackerThits);
		h_muLowerMinusValidHitsEtaPlusPhiZero->Fill(                   *lowTrackerValidHits);
		h_muLowerMinusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muLowerMinusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		h_muLowerMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		
		
	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muLowerMinusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		h_muLowerMinusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		h_muLowerMinusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		h_muLowerMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerMinusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		h_muLowerMinusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		h_muLowerMinusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		h_muLowerMinusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muLowerMinusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muLowerMinusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muLowerMinusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerMinusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		h_muLowerMinusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		h_muLowerMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerMinusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muLowerMinusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muLowerMinusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muLowerMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *lowTrackerThits);
		h_muLowerMinusValidHitsEtaPlusPhiPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		h_muLowerMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	    }
	    if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	      h_muLowerMinusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    }
	    else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    

	    h_muMinusDxy->Fill(     *lowTrackerDxy);
	    h_muLowerMinusDxy->Fill(*lowTrackerDxy);
	    h_muMinusDz->Fill(      *lowTrackerDz);
	    h_muLowerMinusDz->Fill( *lowTrackerDz);
	    h_muMinusDxyError->Fill(*lowTrackerDxyError);
	    h_muLowerMinusDxyError->Fill(*lowTrackerDxyError);
	    h_muMinusDzError->Fill(      *lowTrackerDzError);
	    h_muLowerMinusDzError->Fill( *lowTrackerDzError);

	    h_muMinusPt->Fill(      lowTrackerMuonP4->pt());
	    h_muLowerMinusPt->Fill( lowTrackerMuonP4->pt());
	    h_muMinusEta->Fill(     lowTrackerMuonP4->eta());
	    h_muLowerMinusEta->Fill(lowTrackerMuonP4->eta());
	    h_muMinusPhi->Fill(     lowTrackerMuonP4->phi());
	    h_muLowerMinusPhi->Fill(lowTrackerMuonP4->phi());

	    h_muMinusPtError->Fill(     *lowTrackerPtError);
	    h_muLowerMinusPtError->Fill(*lowTrackerPtError);
	    h_muMinusPtRelErr->Fill(     lowerRelPtErr);
	    h_muLowerMinusPtRelErr->Fill(lowerRelPtErr);
	    h_muMinusTrackPt->Fill(     sqrt(lowTrackerTrack->perp2()));
	    h_muLowerMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	    h_muMinusTrackEta->Fill(     lowTrackerTrack->eta());
	    h_muLowerMinusTrackEta->Fill(lowTrackerTrack->eta());
	    h_muMinusTrackPhi->Fill(     lowTrackerTrack->phi());
	    h_muLowerMinusTrackPhi->Fill(lowTrackerTrack->phi());

	    h_muMinusPixelHits->Fill(                        *lowTrackerPhits);
	    h_muLowerMinusPixelHits->Fill(                   *lowTrackerPhits);
	    h_muMinusTrackerHits->Fill(                      *lowTrackerThits);
	    h_muLowerMinusTrackerHits->Fill(                 *lowTrackerThits);
	    h_muMinusValidHits->Fill(                        *lowTrackerValidHits);
	    h_muLowerMinusValidHits->Fill(                   *lowTrackerValidHits);
	    h_muMinusValidMuonHits->Fill(                    *lowTrackerValidMuonHits);
	    h_muLowerMinusValidMuonHits->Fill(               *lowTrackerValidMuonHits);
	    h_muMinusMuonStationHits->Fill(                  *lowTrackerMhits);
	    h_muLowerMinusMuonStationHits->Fill(             *lowTrackerMhits);
	    h_muMinusMatchedMuonStations->Fill(              *lowTrackerMatchedMuonStations);
	    h_muLowerMinusMatchedMuonStations->Fill(         *lowTrackerMatchedMuonStations);
	    h_muMinusTrackerLayersWithMeasurement->Fill(     *lowTrackerLayersWithMeasurement);
	    h_muLowerMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muMinusCurvePlusBias[i]->Fill(     symmetric_?posBias:fabs(posBias));
	      if (lowTrackerTrack->eta() > 0){
		h_muMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      
	      
	      h_muMinusCurveMinusBias[i]->Fill(    symmetric_?negBias:fabs(negBias));
	      if (lowTrackerTrack->eta() > 0){
		h_muMinusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muMinusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muMinusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muMinusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muMinusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muMinusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	  
	      
	      
	      // these were changed from LowerPlusCurve to LowerMinusCurve on April 11
	      h_muLowerMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      if (lowTrackerTrack->eta() > 0){
		h_muLowerMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muLowerMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      


	      h_muLowerMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));
	      if (lowTrackerTrack->eta() > 0){
		h_muLowerMinusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerMinusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muLowerMinusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerMinusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muLowerMinusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerMinusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muLowerMinusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      
	      

	      if (debug)
		std::cout << "Made it through the lower bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (low_n1pt) {
	      h_looseMuMinusChi2->Fill(       *lowTrackerChi2);
	      h_looseMuLowerMinusChi2->Fill(  *lowTrackerChi2);
	      h_looseMuMinusNdof->Fill(       *lowTrackerNdof);
	      h_looseMuLowerMinusNdof->Fill(  *lowTrackerNdof);
	      h_looseMuMinusCharge->Fill(     *lowTrackerCharge);
	      h_looseMuLowerMinusCharge->Fill(*lowTrackerCharge);
	      h_looseMuMinusCurve->Fill(     symmetric_?lowerCpT:fabs(lowerCpT));
	      if (lowTrackerTrack->eta() > 0){
		h_looseMuMinusChi2EtaPlus->Fill(  *lowTrackerChi2);
		h_looseMuMinusNdofEtaPlus->Fill(  *lowTrackerNdof);
		h_looseMuMinusChargeEtaPlus->Fill(*lowTrackerCharge);
		h_looseMuMinusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuMinusDxyEtaPlus->Fill(     *lowTrackerDxy);
		h_looseMuMinusDzEtaPlus->Fill(      *lowTrackerDz);
		h_looseMuMinusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
		h_looseMuMinusDzErrorEtaPlus->Fill( *lowTrackerDzError);
		
		h_looseMuMinusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
		h_looseMuMinusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
		h_looseMuMinusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuMinusPtErrorEtaPlus->Fill( *lowTrackerPtError);
		h_looseMuMinusPtRelErrEtaPlus->Fill( lowerRelPtErr);
		h_looseMuMinusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuMinusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
		h_looseMuMinusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
		
		h_looseMuMinusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
		h_looseMuMinusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_looseMuMinusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_looseMuMinusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuMinusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
		h_looseMuMinusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuMinusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
		
		
		
		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuMinusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuMinusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuMinusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuMinusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuMinusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuMinusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuMinusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuMinusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuMinusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *lowTrackerThits);
		  h_looseMuMinusValidHitsEtaPlusPhiMinus->Fill(                   *lowTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		  
		}
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuMinusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuMinusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuMinusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuMinusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuMinusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuMinusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuMinusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
	      
		  h_looseMuMinusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuMinusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaPlusPhiZero->Fill(                 *lowTrackerThits);
		  h_looseMuMinusValidHitsEtaPlusPhiZero->Fill(                   *lowTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		  
		  

		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuMinusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuMinusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuMinusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuMinusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuMinusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuMinusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuMinusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuMinusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuMinusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuMinusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaPlusPhiZero->Fill(                 *lowTrackerThits);
		  h_looseMuMinusValidHitsEtaPlusPhiZero->Fill(                   *lowTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		}
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_looseMuMinusChi2EtaMinus->Fill(  *lowTrackerChi2);
		h_looseMuMinusNdofEtaMinus->Fill(  *lowTrackerNdof);
		h_looseMuMinusChargeEtaMinus->Fill(*lowTrackerCharge);
		h_looseMuMinusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuMinusDxyEtaMinus->Fill(     *lowTrackerDxy);
		h_looseMuMinusDzEtaMinus->Fill(      *lowTrackerDz);
		h_looseMuMinusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
		h_looseMuMinusDzErrorEtaMinus->Fill( *lowTrackerDzError);
		
		h_looseMuMinusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
		h_looseMuMinusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
		h_looseMuMinusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuMinusPtErrorEtaMinus->Fill( *lowTrackerPtError);
		h_looseMuMinusPtRelErrEtaMinus->Fill( lowerRelPtErr);
		h_looseMuMinusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuMinusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
		h_looseMuMinusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
		
		h_looseMuMinusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
		h_looseMuMinusTrackerHitsEtaMinus->Fill(                 *lowTrackerThits);
		h_looseMuMinusValidHitsEtaMinus->Fill(                   *lowTrackerValidHits);
		h_looseMuMinusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuMinusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
		h_looseMuMinusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuMinusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);

	      
		
		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuMinusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuMinusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuMinusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuMinusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuMinusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuMinusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuMinusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuMinusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuMinusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaMinusPhiMinus->Fill(                 *lowTrackerThits);
		  h_looseMuMinusValidHitsEtaMinusPhiMinus->Fill(                   *lowTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}
		
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuMinusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuMinusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuMinusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuMinusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuMinusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuMinusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuMinusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuMinusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuMinusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaMinusPhiZero->Fill(                 *lowTrackerThits);
		  h_looseMuMinusValidHitsEtaMinusPhiZero->Fill(                   *lowTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		  
		  
		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuMinusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuMinusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuMinusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuMinusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuMinusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuMinusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuMinusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuMinusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuMinusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuMinusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuMinusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuMinusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuMinusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuMinusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuMinusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *lowTrackerThits);
		  h_looseMuMinusValidHitsEtaMinusPhiPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		  h_looseMuMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		}
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_looseMuMinusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      

	      h_looseMuLowerMinusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      if (lowTrackerTrack->eta() > 0){
		h_looseMuLowerMinusChi2EtaPlus->Fill(  *lowTrackerChi2);
		h_looseMuLowerMinusNdofEtaPlus->Fill(  *lowTrackerNdof);
		h_looseMuLowerMinusChargeEtaPlus->Fill(*lowTrackerCharge);
		h_looseMuLowerMinusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuLowerMinusDxyEtaPlus->Fill(     *lowTrackerDxy);
		h_looseMuLowerMinusDzEtaPlus->Fill(      *lowTrackerDz);
		h_looseMuLowerMinusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
		h_looseMuLowerMinusDzErrorEtaPlus->Fill( *lowTrackerDzError);
		
		h_looseMuLowerMinusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
		h_looseMuLowerMinusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
		h_looseMuLowerMinusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuLowerMinusPtErrorEtaPlus->Fill( *lowTrackerPtError);
		h_looseMuLowerMinusPtRelErrEtaPlus->Fill( lowerRelPtErr);
		h_looseMuLowerMinusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuLowerMinusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
		h_looseMuLowerMinusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
		
		h_looseMuLowerMinusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
		h_looseMuLowerMinusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_looseMuLowerMinusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_looseMuLowerMinusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuLowerMinusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
		h_looseMuLowerMinusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
		
		
		
		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuLowerMinusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerMinusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerMinusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuLowerMinusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerMinusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerMinusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuLowerMinusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerMinusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerMinusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerMinusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerMinusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerMinusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuLowerMinusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuLowerMinusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerMinusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerMinusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerMinusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerMinusTrackerHitsEtaPlusPhiMinus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerMinusValidHitsEtaPlusPhiMinus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerMinusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerMinusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerMinusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerMinusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuLowerMinusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuLowerMinusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuLowerMinusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerMinusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuLowerMinusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuLowerMinusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuLowerMinusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerMinusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerMinusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerMinusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerMinusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuLowerMinusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuLowerMinusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerMinusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerMinusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		
		  h_looseMuLowerMinusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerMinusTrackerHitsEtaPlusPhiZero->Fill(                 *lowTrackerThits);
		  h_looseMuLowerMinusValidHitsEtaPlusPhiZero->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerMinusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerMinusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuLowerMinusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);

		  
		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuLowerMinusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerMinusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerMinusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuLowerMinusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerMinusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerMinusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuLowerMinusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerMinusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerMinusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerMinusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerMinusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerMinusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuLowerMinusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuLowerMinusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerMinusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerMinusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerMinusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerMinusTrackerHitsEtaPlusPhiPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerMinusValidHitsEtaPlusPhiPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerMinusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerMinusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerMinusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuLowerMinusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		}
		
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_looseMuLowerMinusChi2EtaMinus->Fill(  *lowTrackerChi2);
		h_looseMuLowerMinusNdofEtaMinus->Fill(  *lowTrackerNdof);
		h_looseMuLowerMinusChargeEtaMinus->Fill(*lowTrackerCharge);
		h_looseMuLowerMinusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuLowerMinusDxyEtaMinus->Fill(     *lowTrackerDxy);
		h_looseMuLowerMinusDzEtaMinus->Fill(      *lowTrackerDz);
		h_looseMuLowerMinusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
		h_looseMuLowerMinusDzErrorEtaMinus->Fill( *lowTrackerDzError);
		
		h_looseMuLowerMinusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
		h_looseMuLowerMinusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
		h_looseMuLowerMinusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuLowerMinusPtErrorEtaMinus->Fill( *lowTrackerPtError);
		h_looseMuLowerMinusPtRelErrEtaMinus->Fill( lowerRelPtErr);
		h_looseMuLowerMinusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuLowerMinusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
		h_looseMuLowerMinusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
		
		h_looseMuLowerMinusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
		h_looseMuLowerMinusTrackerHitsEtaMinus->Fill(                 *lowTrackerThits);
		h_looseMuLowerMinusValidHitsEtaMinus->Fill(                   *lowTrackerValidHits);
		h_looseMuLowerMinusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuLowerMinusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
		h_looseMuLowerMinusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);
		
		
		
		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuLowerMinusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerMinusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerMinusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuLowerMinusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerMinusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerMinusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuLowerMinusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerMinusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerMinusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerMinusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerMinusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		  h_looseMuLowerMinusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuLowerMinusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuLowerMinusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerMinusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerMinusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerMinusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerMinusTrackerHitsEtaMinusPhiMinus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerMinusValidHitsEtaMinusPhiMinus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerMinusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerMinusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerMinusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}		
		
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerMinusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuLowerMinusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuLowerMinusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuLowerMinusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerMinusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuLowerMinusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuLowerMinusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuLowerMinusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		
		  h_looseMuLowerMinusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerMinusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerMinusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerMinusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuLowerMinusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuLowerMinusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerMinusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerMinusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerMinusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerMinusTrackerHitsEtaMinusPhiZero->Fill(                 *lowTrackerThits);
		  h_looseMuLowerMinusValidHitsEtaMinusPhiZero->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerMinusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerMinusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuLowerMinusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		  
		  
		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuLowerMinusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerMinusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerMinusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuLowerMinusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerMinusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerMinusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuLowerMinusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerMinusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerMinusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerMinusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerMinusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerMinusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuLowerMinusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuLowerMinusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerMinusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerMinusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		
		  h_looseMuLowerMinusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerMinusTrackerHitsEtaMinusPhiPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerMinusValidHitsEtaMinusPhiPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerMinusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuLowerMinusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		h_looseMuLowerMinusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuLowerMinusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		}
		
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_looseMuLowerMinusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      
	      
	      
	      // loose doesn't include the dxy/dz cuts
	      h_looseMuMinusDxy->Fill(     *lowTrackerDxy);
	      h_looseMuLowerMinusDxy->Fill(*lowTrackerDxy);
	      h_looseMuMinusDz->Fill(      *lowTrackerDz);
	      h_looseMuLowerMinusDz->Fill( *lowTrackerDz);
	      h_looseMuMinusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuLowerMinusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuMinusDzError->Fill(      *lowTrackerDzError);
	      h_looseMuLowerMinusDzError->Fill( *lowTrackerDzError);

	      h_looseMuMinusPt->Fill(      lowTrackerMuonP4->pt());
	      h_looseMuLowerMinusPt->Fill( lowTrackerMuonP4->pt());
	      h_looseMuMinusEta->Fill(     lowTrackerMuonP4->eta());
	      h_looseMuLowerMinusEta->Fill(lowTrackerMuonP4->eta());
	      h_looseMuMinusPhi->Fill(     lowTrackerMuonP4->phi());
	      h_looseMuLowerMinusPhi->Fill(lowTrackerMuonP4->phi());

	      h_looseMuMinusTrackPt->Fill(     sqrt(lowTrackerTrack->perp2()));
	      h_looseMuLowerMinusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      h_looseMuMinusTrackEta->Fill(         lowTrackerTrack->eta());
	      h_looseMuLowerMinusTrackEta->Fill(    lowTrackerTrack->eta());
	      h_looseMuMinusTrackPhi->Fill(         lowTrackerTrack->phi());
	      h_looseMuLowerMinusTrackPhi->Fill(    lowTrackerTrack->phi());

	      h_looseMuMinusValidHits->Fill(     *lowTrackerValidHits);
	      h_looseMuLowerMinusValidHits->Fill(*lowTrackerValidHits);

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuMinusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (lowTrackerTrack->eta() < 0){
		  h_looseMuMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuMinusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));	
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuMinusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
		else if (lowTrackerTrack->eta() < 0){
		  h_looseMuMinusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuMinusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuMinusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuMinusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuMinusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		h_looseMuLowerMinusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuLowerMinusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerMinusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (lowTrackerTrack->eta() < 0){
		  h_looseMuLowerMinusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerMinusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerMinusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuLowerMinusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuLowerMinusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerMinusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
		else if (lowTrackerTrack->eta() < 0){
		  h_looseMuLowerMinusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerMinusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerMinusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerMinusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerMinusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		
		if (debug)
		  std::cout << "Made it through the lower bias loop " << i << std::endl; 
	      }
	    } // end if (low_n1pt)

	    if (low_n1pixhits) {
	      h_looseMuMinusPixelHits->Fill(     *lowTrackerPhits);
	      h_looseMuLowerMinusPixelHits->Fill(*lowTrackerPhits);
	    }
	    if (low_n1vmuhits) {
	      h_looseMuMinusValidMuonHits->Fill(       *lowTrackerValidMuonHits);
	      h_looseMuLowerMinusValidMuonHits->Fill(  *lowTrackerValidMuonHits);
	      h_looseMuMinusMuonStationHits->Fill(     *lowTrackerMhits);
	      h_looseMuLowerMinusMuonStationHits->Fill(*lowTrackerMhits);
	    }
	    if (low_n1ptrelerr) {
	      h_looseMuMinusPtError->Fill(     *lowTrackerPtError);
	      h_looseMuLowerMinusPtError->Fill(*lowTrackerPtError);
	      h_looseMuMinusPtRelErr->Fill(     lowerRelPtErr);
	      h_looseMuLowerMinusPtRelErr->Fill(lowerRelPtErr);
	    }
	    if (low_n1trkhits) {
	      h_looseMuMinusTrackerHits->Fill(     *lowTrackerThits);
	      h_looseMuLowerMinusTrackerHits->Fill(*lowTrackerThits);
	      h_looseMuMinusTrackerLayersWithMeasurement->Fill(     *lowTrackerLayersWithMeasurement);
	      h_looseMuLowerMinusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	    }
	    if (low_n1mmustahits) {
	      h_looseMuMinusMatchedMuonStations->Fill(     *lowTrackerMatchedMuonStations);
	      h_looseMuLowerMinusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
	    }
	  } // end if charge < 0

	  else { // charge > 0
	    h_muPlusChi2->Fill(       *lowTrackerChi2);
	    h_muLowerPlusChi2->Fill(  *lowTrackerChi2);
	    h_muPlusNdof->Fill(       *lowTrackerNdof);
	    h_muLowerPlusNdof->Fill(  *lowTrackerNdof);
	    h_muPlusCharge->Fill(     *lowTrackerCharge);
	    h_muLowerPlusCharge->Fill(*lowTrackerCharge);
	    h_muPlusCurve->Fill(     symmetric_?lowerCpT:fabs(lowerCpT));
	    if (lowTrackerTrack->eta() > 0){

	      h_muPlusChi2EtaPlus->Fill(  *lowTrackerChi2);
	      h_muPlusNdofEtaPlus->Fill(  *lowTrackerNdof);
	      h_muPlusChargeEtaPlus->Fill(*lowTrackerCharge);
	      h_muPlusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muPlusDxyEtaPlus->Fill(     *lowTrackerDxy);
	      h_muPlusDzEtaPlus->Fill(      *lowTrackerDz);
	      h_muPlusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
	      h_muPlusDzErrorEtaPlus->Fill( *lowTrackerDzError);
	      
	      h_muPlusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
	      h_muPlusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
	      h_muPlusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muPlusPtErrorEtaPlus->Fill( *lowTrackerPtError);
	      h_muPlusPtRelErrEtaPlus->Fill( lowerRelPtErr);
	      h_muPlusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muPlusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
	      h_muPlusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
	      
	      h_muPlusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
	      h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	      h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muPlusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
	      h_muPlusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
	      h_muPlusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muPlusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
	      

	      if(lowTrackerTrack->phi() < -1.0472){ h_muPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		h_muPlusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		h_muPlusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		h_muPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		h_muPlusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		h_muPlusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		h_muPlusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muPlusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muPlusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muPlusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		h_muPlusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		h_muPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muPlusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
	      
		h_muPlusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		h_muPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }   	      
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muPlusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		h_muPlusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		h_muPlusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		h_muPlusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		h_muPlusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		h_muPlusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		h_muPlusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
	      
		h_muPlusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muPlusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muPlusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		h_muPlusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		h_muPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		h_muPlusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muPlusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		h_muPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		
	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muPlusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		h_muPlusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		h_muPlusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		h_muPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		h_muPlusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		h_muPlusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		h_muPlusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muPlusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muPlusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muPlusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		h_muPlusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		h_muPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muPlusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		h_muPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	      
	    }
	    
	    
	    else if (lowTrackerTrack->eta() < 0){
	      h_muPlusChi2EtaMinus->Fill(  *lowTrackerChi2);
	      h_muPlusNdofEtaMinus->Fill(  *lowTrackerNdof);
	      h_muPlusChargeEtaMinus->Fill(*lowTrackerCharge);
	      h_muPlusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muPlusDxyEtaMinus->Fill(     *lowTrackerDxy);
	      h_muPlusDzEtaMinus->Fill(      *lowTrackerDz);
	      h_muPlusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
	      h_muPlusDzErrorEtaMinus->Fill( *lowTrackerDzError);
	      
	      h_muPlusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
	      h_muPlusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
	      h_muPlusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muPlusPtErrorEtaMinus->Fill( *lowTrackerPtError);
	      h_muPlusPtRelErrEtaMinus->Fill( lowerRelPtErr);
	      h_muPlusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muPlusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
	      h_muPlusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
	      
	      h_muPlusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
	      h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	      h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muPlusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
	      h_muPlusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
	      h_muPlusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muPlusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);
	      
	    
	      
	      if(lowTrackerTrack->phi() < -1.0472){
		h_muPlusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		h_muPlusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		h_muPlusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		h_muPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		h_muPlusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		h_muPlusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		h_muPlusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muPlusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muPlusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muPlusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		h_muPlusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		h_muPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muPlusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		h_muPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }	      
	      
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muPlusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		h_muPlusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		h_muPlusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		h_muPlusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		h_muPlusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		h_muPlusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		h_muPlusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		
		h_muPlusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muPlusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muPlusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		h_muPlusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		h_muPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		h_muPlusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
	      
		h_muPlusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muPlusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		h_muPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
	      	
		
		
	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muPlusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		h_muPlusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		h_muPlusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		h_muPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muPlusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		h_muPlusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		h_muPlusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		h_muPlusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
	      
		h_muPlusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muPlusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muPlusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muPlusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		h_muPlusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		h_muPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muPlusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muPlusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muPlusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		h_muPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	    }
	    if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)) h_muPlusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    


	    h_muLowerPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	    if (lowTrackerTrack->eta() > 0){
	      h_muLowerPlusChi2EtaPlus->Fill(  *lowTrackerChi2);
	      h_muLowerPlusNdofEtaPlus->Fill(  *lowTrackerNdof);
	      h_muLowerPlusChargeEtaPlus->Fill(*lowTrackerCharge);
	      h_muLowerPlusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muLowerPlusDxyEtaPlus->Fill(     *lowTrackerDxy);
	      h_muLowerPlusDzEtaPlus->Fill(      *lowTrackerDz);
	      h_muLowerPlusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
	      h_muLowerPlusDzErrorEtaPlus->Fill( *lowTrackerDzError);
	      
	      h_muLowerPlusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
	      h_muLowerPlusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
	      h_muLowerPlusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muLowerPlusPtErrorEtaPlus->Fill( *lowTrackerPtError);
	      h_muLowerPlusPtRelErrEtaPlus->Fill( lowerRelPtErr);
	      h_muLowerPlusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muLowerPlusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
	      h_muLowerPlusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
	      
	      h_muLowerPlusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
	      h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	      h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muLowerPlusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
	      h_muLowerPlusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
	      h_muLowerPlusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muLowerPlusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
	      


	      if(lowTrackerTrack->phi() < -1.0472){
		h_muLowerPlusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		h_muLowerPlusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		h_muLowerPlusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		h_muLowerPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerPlusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		h_muLowerPlusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		h_muLowerPlusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		h_muLowerPlusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muLowerPlusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muLowerPlusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muLowerPlusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerPlusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		h_muLowerPlusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		h_muLowerPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerPlusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muLowerPlusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		
		h_muLowerPlusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		h_muLowerPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }	      	     
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerPlusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		h_muLowerPlusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		h_muLowerPlusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		h_muLowerPlusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerPlusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		h_muLowerPlusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		h_muLowerPlusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		h_muLowerPlusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
		
		h_muLowerPlusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muLowerPlusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muLowerPlusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerPlusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		h_muLowerPlusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		h_muLowerPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerPlusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		h_muLowerPlusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muLowerPlusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerPlusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muLowerPlusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		h_muLowerPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		
	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muLowerPlusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		h_muLowerPlusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		h_muLowerPlusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		h_muLowerPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerPlusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		h_muLowerPlusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		h_muLowerPlusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		h_muLowerPlusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muLowerPlusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muLowerPlusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muLowerPlusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerPlusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		h_muLowerPlusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		h_muLowerPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerPlusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muLowerPlusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
	      
		h_muLowerPlusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		h_muLowerPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
	      }
	      
	    }
	    
	    
	    else if (lowTrackerTrack->eta() < 0){
	      h_muLowerPlusChi2EtaMinus->Fill(  *lowTrackerChi2);
	      h_muLowerPlusNdofEtaMinus->Fill(  *lowTrackerNdof);
	      h_muLowerPlusChargeEtaMinus->Fill(*lowTrackerCharge);
	      h_muLowerPlusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      h_muLowerPlusDxyEtaMinus->Fill(     *lowTrackerDxy);
	      h_muLowerPlusDzEtaMinus->Fill(      *lowTrackerDz);
	      h_muLowerPlusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
	      h_muLowerPlusDzErrorEtaMinus->Fill( *lowTrackerDzError);
	      
	      h_muLowerPlusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
	      h_muLowerPlusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
	      h_muLowerPlusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
	      
	      h_muLowerPlusPtErrorEtaMinus->Fill( *lowTrackerPtError);
	      h_muLowerPlusPtRelErrEtaMinus->Fill( lowerRelPtErr);
	      h_muLowerPlusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
	      h_muLowerPlusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
	      h_muLowerPlusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
	      
	      h_muLowerPlusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
	      h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
	      h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
	      h_muLowerPlusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
	      h_muLowerPlusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
	      h_muLowerPlusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
	      h_muLowerPlusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);
	      

	      
	      if(lowTrackerTrack->phi() < -1.0472){
		h_muLowerPlusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		h_muLowerPlusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		h_muLowerPlusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		h_muLowerPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerPlusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		h_muLowerPlusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		h_muLowerPlusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		h_muLowerPlusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		
		h_muLowerPlusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		h_muLowerPlusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		h_muLowerPlusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerPlusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		h_muLowerPlusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		h_muLowerPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerPlusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		h_muLowerPlusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
	      
		h_muLowerPlusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		h_muLowerPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
	      }	      
	      
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerPlusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		h_muLowerPlusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		h_muLowerPlusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		h_muLowerPlusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerPlusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		h_muLowerPlusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		h_muLowerPlusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		h_muLowerPlusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		
		h_muLowerPlusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		h_muLowerPlusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		h_muLowerPlusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerPlusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		h_muLowerPlusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		h_muLowerPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerPlusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		h_muLowerPlusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
		
		h_muLowerPlusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerPlusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		h_muLowerPlusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		h_muLowerPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		
		

	      }
	      else if(lowTrackerTrack->phi() > 1.0472){
		h_muLowerPlusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		h_muLowerPlusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		h_muLowerPlusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		h_muLowerPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_muLowerPlusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		h_muLowerPlusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		h_muLowerPlusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		h_muLowerPlusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
		
		h_muLowerPlusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		h_muLowerPlusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		h_muLowerPlusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		
		h_muLowerPlusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		h_muLowerPlusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		h_muLowerPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_muLowerPlusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		h_muLowerPlusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		
		h_muLowerPlusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		h_muLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_muLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_muLowerPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		h_muLowerPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		h_muLowerPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_muLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		
	      }
	    }
	    if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
	      h_muLowerPlusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    }
	    else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	    


	    h_muPlusDxy->Fill(     *lowTrackerDxy);
	    h_muLowerPlusDxy->Fill(*lowTrackerDxy);
	    h_muPlusDz->Fill(      *lowTrackerDz);
	    h_muLowerPlusDz->Fill( *lowTrackerDz);
	    h_muPlusDxyError->Fill(*lowTrackerDxyError);
	    h_muLowerPlusDxyError->Fill(*lowTrackerDxyError);
	    h_muPlusDzError->Fill(      *lowTrackerDzError);
	    h_muLowerPlusDzError->Fill(*lowTrackerDzError);

	    h_muPlusPt->Fill(      lowTrackerMuonP4->pt());
	    h_muLowerPlusPt->Fill( lowTrackerMuonP4->pt());
	    h_muPlusEta->Fill(     lowTrackerMuonP4->eta());
	    h_muLowerPlusEta->Fill(lowTrackerMuonP4->eta());
	    h_muPlusPhi->Fill(     lowTrackerMuonP4->phi());
	    h_muLowerPlusPhi->Fill(lowTrackerMuonP4->phi());

	    h_muPlusPtError->Fill(     *lowTrackerPtError);
	    h_muLowerPlusPtError->Fill(*lowTrackerPtError);
	    h_muPlusPtRelErr->Fill(     lowerRelPtErr);
	    h_muLowerPlusPtRelErr->Fill(lowerRelPtErr);
	    h_muPlusTrackPt->Fill(     sqrt(lowTrackerTrack->perp2()));
	    h_muLowerPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	    h_muPlusTrackEta->Fill(     lowTrackerTrack->eta());
	    h_muLowerPlusTrackEta->Fill(lowTrackerTrack->eta());
	    h_muPlusTrackPhi->Fill(     lowTrackerTrack->phi());
	    h_muLowerPlusTrackPhi->Fill(lowTrackerTrack->phi());

	    h_muPlusPixelHits->Fill(                        *lowTrackerPhits);
	    h_muLowerPlusPixelHits->Fill(                   *lowTrackerPhits);
	    h_muPlusTrackerHits->Fill(                      *lowTrackerThits);
	    h_muLowerPlusTrackerHits->Fill(                 *lowTrackerThits);
	    h_muPlusValidHits->Fill(                        *lowTrackerValidHits);
	    h_muLowerPlusValidHits->Fill(                   *lowTrackerValidHits);
	    h_muPlusValidMuonHits->Fill(                    *lowTrackerValidMuonHits);
	    h_muLowerPlusValidMuonHits->Fill(               *lowTrackerValidMuonHits);
	    h_muPlusMuonStationHits->Fill(                  *lowTrackerMhits);
	    h_muLowerPlusMuonStationHits->Fill(             *lowTrackerMhits);
	    h_muPlusMatchedMuonStations->Fill(              *lowTrackerMatchedMuonStations);
	    h_muLowerPlusMatchedMuonStations->Fill(         *lowTrackerMatchedMuonStations);
	    h_muPlusTrackerLayersWithMeasurement->Fill(     *lowTrackerLayersWithMeasurement);
	    h_muLowerPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);

	    for (int i = 0; i < nBiasBins; ++i) {
	      double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
	      double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
	      h_muPlusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
	      if (lowTrackerTrack->eta() > 0){
		h_muPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      
	      
	      h_muPlusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));	
	      if (lowTrackerTrack->eta() > 0){
		h_muPlusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muPlusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muPlusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muPlusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muPlusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muPlusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      
	      
	      h_muLowerPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
	      if (lowTrackerTrack->eta() > 0){
		h_muLowerPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muLowerPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
	      

	      h_muLowerPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
	      if (lowTrackerTrack->eta() > 0){
		h_muLowerPlusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerPlusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_muLowerPlusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_muLowerPlusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_muLowerPlusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_muLowerPlusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_muLowerPlusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      


	      if (debug)
		std::cout << "Made it through the lower bias loop " << i << std::endl; 
	    }
	
	    // if a variable doesn't appear in the High-pT muon selection, then apply all the cuts
	    if (low_n1pt) {
	      h_looseMuPlusChi2->Fill(       *lowTrackerChi2);
	      h_looseMuLowerPlusChi2->Fill(  *lowTrackerChi2);
	      h_looseMuPlusNdof->Fill(       *lowTrackerNdof);
	      h_looseMuLowerPlusNdof->Fill(  *lowTrackerNdof);
	      h_looseMuPlusCharge->Fill(     *lowTrackerCharge);
	      h_looseMuLowerPlusCharge->Fill(*lowTrackerCharge);
	      h_looseMuPlusCurve->Fill(     symmetric_?lowerCpT:fabs(lowerCpT));
	      if (lowTrackerTrack->eta() > 0){
		h_looseMuPlusChi2EtaPlus->Fill(  *lowTrackerChi2);
		h_looseMuPlusNdofEtaPlus->Fill(  *lowTrackerNdof);
		h_looseMuPlusChargeEtaPlus->Fill(*lowTrackerCharge);
		h_looseMuPlusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuPlusDxyEtaPlus->Fill(     *lowTrackerDxy);
		h_looseMuPlusDzEtaPlus->Fill(      *lowTrackerDz);
		h_looseMuPlusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
		h_looseMuPlusDzErrorEtaPlus->Fill( *lowTrackerDzError);
		
		h_looseMuPlusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
		h_looseMuPlusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
		h_looseMuPlusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuPlusPtErrorEtaPlus->Fill( *lowTrackerPtError);
		h_looseMuPlusPtRelErrEtaPlus->Fill( lowerRelPtErr);
		h_looseMuPlusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuPlusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
		h_looseMuPlusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
		
		h_looseMuPlusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
		h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_looseMuPlusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuPlusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
		h_looseMuPlusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuPlusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);
		

		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuPlusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuPlusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuPlusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuPlusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuPlusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuPlusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuPlusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuPlusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuPlusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}


		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuPlusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuPlusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuPlusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuPlusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuPlusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuPlusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuPlusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuPlusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		
		  h_looseMuPlusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuPlusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		  
		  
		  h_looseMuPlusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuPlusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuPlusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuPlusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuPlusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuPlusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuPlusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuPlusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuPlusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuPlusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		
		}

	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_looseMuPlusChi2EtaMinus->Fill(  *lowTrackerChi2);
		h_looseMuPlusNdofEtaMinus->Fill(  *lowTrackerNdof);
		h_looseMuPlusChargeEtaMinus->Fill(*lowTrackerCharge);
		h_looseMuPlusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuPlusDxyEtaMinus->Fill(     *lowTrackerDxy);
		h_looseMuPlusDzEtaMinus->Fill(      *lowTrackerDz);
		h_looseMuPlusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
		h_looseMuPlusDzErrorEtaMinus->Fill( *lowTrackerDzError);
		
		h_looseMuPlusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
		h_looseMuPlusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
		h_looseMuPlusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuPlusPtErrorEtaMinus->Fill( *lowTrackerPtError);
		h_looseMuPlusPtRelErrEtaMinus->Fill( lowerRelPtErr);
		h_looseMuPlusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuPlusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
		h_looseMuPlusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
		
		h_looseMuPlusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
		h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_looseMuPlusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuPlusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
		h_looseMuPlusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuPlusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);
		


		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuPlusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuPlusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuPlusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuPlusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuPlusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuPlusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuPlusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuPlusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuPlusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}

		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuPlusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuPlusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuPlusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuPlusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuPlusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuPlusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuPlusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuPlusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuPlusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		  
		  

		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuPlusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuPlusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuPlusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuPlusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuPlusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuPlusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuPlusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuPlusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuPlusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuPlusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuPlusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuPlusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuPlusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuPlusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuPlusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		  h_looseMuPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		}
		
	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_looseMuPlusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      
	      
	      h_looseMuLowerPlusCurve->Fill(symmetric_?lowerCpT:fabs(lowerCpT));
	      if (lowTrackerTrack->eta() > 0){
		h_looseMuLowerPlusChi2EtaPlus->Fill(  *lowTrackerChi2);
		h_looseMuLowerPlusNdofEtaPlus->Fill(  *lowTrackerNdof);
		h_looseMuLowerPlusChargeEtaPlus->Fill(*lowTrackerCharge);
		h_looseMuLowerPlusCurveEtaPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuLowerPlusDxyEtaPlus->Fill(     *lowTrackerDxy);
		h_looseMuLowerPlusDzEtaPlus->Fill(      *lowTrackerDz);
		h_looseMuLowerPlusDxyErrorEtaPlus->Fill(*lowTrackerDxyError);
		h_looseMuLowerPlusDzErrorEtaPlus->Fill( *lowTrackerDzError);
		
		h_looseMuLowerPlusPtEtaPlus->Fill( lowTrackerMuonP4->pt());
		h_looseMuLowerPlusEtaEtaPlus->Fill(lowTrackerMuonP4->eta());
		h_looseMuLowerPlusPhiEtaPlus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuLowerPlusPtErrorEtaPlus->Fill( *lowTrackerPtError);
		h_looseMuLowerPlusPtRelErrEtaPlus->Fill( lowerRelPtErr);
		h_looseMuLowerPlusTrackPtEtaPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuLowerPlusTrackEtaEtaPlus->Fill( lowTrackerTrack->eta());
		h_looseMuLowerPlusTrackPhiEtaPlus->Fill( lowTrackerTrack->phi());
		
		h_looseMuLowerPlusPixelHitsEtaPlus->Fill(                   *lowTrackerPhits);
		h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_looseMuLowerPlusValidMuonHitsEtaPlus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuLowerPlusMuonStationHitsEtaPlus->Fill(             *lowTrackerMhits);
		h_looseMuLowerPlusMatchedMuonStationsEtaPlus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlus->Fill(*lowTrackerLayersWithMeasurement);



		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuLowerPlusChi2EtaPlusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerPlusNdofEtaPlusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerPlusChargeEtaPlusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuLowerPlusCurveEtaPlusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerPlusDxyEtaPlusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerPlusDzEtaPlusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuLowerPlusDxyErrorEtaPlusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerPlusDzErrorEtaPlusPhiMinus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerPlusPtEtaPlusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerPlusEtaEtaPlusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerPlusPhiEtaPlusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerPlusPtErrorEtaPlusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuLowerPlusPtRelErrEtaPlusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuLowerPlusTrackPtEtaPlusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerPlusTrackEtaEtaPlusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerPlusTrackPhiEtaPlusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerPlusPixelHitsEtaPlusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerPlusValidMuonHitsEtaPlusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerPlusMuonStationHitsEtaPlusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerPlusMatchedMuonStationsEtaPlusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerPlusChi2EtaPlusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuLowerPlusNdofEtaPlusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuLowerPlusChargeEtaPlusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuLowerPlusCurveEtaPlusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerPlusDxyEtaPlusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuLowerPlusDzEtaPlusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuLowerPlusDxyErrorEtaPlusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuLowerPlusDzErrorEtaPlusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerPlusPtEtaPlusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerPlusEtaEtaPlusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerPlusPhiEtaPlusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerPlusPtErrorEtaPlusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuLowerPlusPtRelErrEtaPlusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuLowerPlusTrackPtEtaPlusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerPlusTrackEtaEtaPlusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerPlusTrackPhiEtaPlusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerPlusPixelHitsEtaPlusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerPlusValidMuonHitsEtaPlusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerPlusMuonStationHitsEtaPlusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuLowerPlusMatchedMuonStationsEtaPlusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		  
		  
		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuLowerPlusChi2EtaPlusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerPlusNdofEtaPlusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerPlusChargeEtaPlusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuLowerPlusCurveEtaPlusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerPlusDxyEtaPlusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerPlusDzEtaPlusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuLowerPlusDxyErrorEtaPlusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerPlusDzErrorEtaPlusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerPlusPtEtaPlusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerPlusEtaEtaPlusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerPlusPhiEtaPlusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerPlusPtErrorEtaPlusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuLowerPlusPtRelErrEtaPlusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuLowerPlusTrackPtEtaPlusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerPlusTrackEtaEtaPlusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerPlusTrackPhiEtaPlusPhiPlus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerPlusPixelHitsEtaPlusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerPlusValidMuonHitsEtaPlusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerPlusMuonStationHitsEtaPlusPhiPlus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerPlusMatchedMuonStationsEtaPlusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerPlusTrackerLayersWithMeasurementEtaPlusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);
		  
		}
	      }
	      
	      
	      else if (lowTrackerTrack->eta() < 0){
		h_looseMuLowerPlusChi2EtaMinus->Fill(  *lowTrackerChi2);
		h_looseMuLowerPlusNdofEtaMinus->Fill(  *lowTrackerNdof);
		h_looseMuLowerPlusChargeEtaMinus->Fill(*lowTrackerCharge);
		h_looseMuLowerPlusCurveEtaMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		h_looseMuLowerPlusDxyEtaMinus->Fill(     *lowTrackerDxy);
		h_looseMuLowerPlusDzEtaMinus->Fill(      *lowTrackerDz);
		h_looseMuLowerPlusDxyErrorEtaMinus->Fill(*lowTrackerDxyError);
		h_looseMuLowerPlusDzErrorEtaMinus->Fill( *lowTrackerDzError);
		
		h_looseMuLowerPlusPtEtaMinus->Fill( lowTrackerMuonP4->pt());
		h_looseMuLowerPlusEtaEtaMinus->Fill(lowTrackerMuonP4->eta());
		h_looseMuLowerPlusPhiEtaMinus->Fill(lowTrackerMuonP4->phi());
		
		h_looseMuLowerPlusPtErrorEtaMinus->Fill( *lowTrackerPtError);
		h_looseMuLowerPlusPtRelErrEtaMinus->Fill( lowerRelPtErr);
		h_looseMuLowerPlusTrackPtEtaMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		h_looseMuLowerPlusTrackEtaEtaMinus->Fill( lowTrackerTrack->eta());
		h_looseMuLowerPlusTrackPhiEtaMinus->Fill( lowTrackerTrack->phi());
		
		h_looseMuLowerPlusPixelHitsEtaMinus->Fill(                   *lowTrackerPhits);
		h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		h_looseMuLowerPlusValidMuonHitsEtaMinus->Fill(               *lowTrackerValidMuonHits);
		h_looseMuLowerPlusMuonStationHitsEtaMinus->Fill(             *lowTrackerMhits);
		h_looseMuLowerPlusMatchedMuonStationsEtaMinus->Fill(         *lowTrackerMatchedMuonStations);
		h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinus->Fill(*lowTrackerLayersWithMeasurement);
		
		if(lowTrackerTrack->phi() < -1.0472){
		  h_looseMuLowerPlusChi2EtaMinusPhiMinus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerPlusNdofEtaMinusPhiMinus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerPlusChargeEtaMinusPhiMinus->Fill(*lowTrackerCharge);
		  h_looseMuLowerPlusCurveEtaMinusPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerPlusDxyEtaMinusPhiMinus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerPlusDzEtaMinusPhiMinus->Fill(      *lowTrackerDz);
		  h_looseMuLowerPlusDxyErrorEtaMinusPhiMinus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerPlusDzErrorEtaMinusPhiMinus->Fill( *lowTrackerDzError);
		
		  h_looseMuLowerPlusPtEtaMinusPhiMinus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerPlusEtaEtaMinusPhiMinus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerPlusPhiEtaMinusPhiMinus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerPlusPtErrorEtaMinusPhiMinus->Fill( *lowTrackerPtError);
		  h_looseMuLowerPlusPtRelErrEtaMinusPhiMinus->Fill( lowerRelPtErr);
		  h_looseMuLowerPlusTrackPtEtaMinusPhiMinus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerPlusTrackEtaEtaMinusPhiMinus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerPlusTrackPhiEtaMinusPhiMinus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerPlusPixelHitsEtaMinusPhiMinus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerPlusValidMuonHitsEtaMinusPhiMinus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerPlusMuonStationHitsEtaMinusPhiMinus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerPlusMatchedMuonStationsEtaMinusPhiMinus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiMinus->Fill(*lowTrackerLayersWithMeasurement);
		}

		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerPlusChi2EtaMinusPhiZero->Fill(  *lowTrackerChi2);
		  h_looseMuLowerPlusNdofEtaMinusPhiZero->Fill(  *lowTrackerNdof);
		  h_looseMuLowerPlusChargeEtaMinusPhiZero->Fill(*lowTrackerCharge);
		  h_looseMuLowerPlusCurveEtaMinusPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerPlusDxyEtaMinusPhiZero->Fill(     *lowTrackerDxy);
		  h_looseMuLowerPlusDzEtaMinusPhiZero->Fill(      *lowTrackerDz);
		  h_looseMuLowerPlusDxyErrorEtaMinusPhiZero->Fill(*lowTrackerDxyError);
		  h_looseMuLowerPlusDzErrorEtaMinusPhiZero->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerPlusPtEtaMinusPhiZero->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerPlusEtaEtaMinusPhiZero->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerPlusPhiEtaMinusPhiZero->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerPlusPtErrorEtaMinusPhiZero->Fill( *lowTrackerPtError);
		  h_looseMuLowerPlusPtRelErrEtaMinusPhiZero->Fill( lowerRelPtErr);
		  h_looseMuLowerPlusTrackPtEtaMinusPhiZero->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerPlusTrackEtaEtaMinusPhiZero->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerPlusTrackPhiEtaMinusPhiZero->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerPlusPixelHitsEtaMinusPhiZero->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerPlusValidMuonHitsEtaMinusPhiZero->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerPlusMuonStationHitsEtaMinusPhiZero->Fill(             *lowTrackerMhits);
		  h_looseMuLowerPlusMatchedMuonStationsEtaMinusPhiZero->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiZero->Fill(*lowTrackerLayersWithMeasurement);
		  

		}
		else if(lowTrackerTrack->phi() > 1.0472){
		  h_looseMuLowerPlusChi2EtaMinusPhiPlus->Fill(  *lowTrackerChi2);
		  h_looseMuLowerPlusNdofEtaMinusPhiPlus->Fill(  *lowTrackerNdof);
		  h_looseMuLowerPlusChargeEtaMinusPhiPlus->Fill(*lowTrackerCharge);
		  h_looseMuLowerPlusCurveEtaMinusPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
		  h_looseMuLowerPlusDxyEtaMinusPhiPlus->Fill(     *lowTrackerDxy);
		  h_looseMuLowerPlusDzEtaMinusPhiPlus->Fill(      *lowTrackerDz);
		  h_looseMuLowerPlusDxyErrorEtaMinusPhiPlus->Fill(*lowTrackerDxyError);
		  h_looseMuLowerPlusDzErrorEtaMinusPhiPlus->Fill( *lowTrackerDzError);
		  
		  h_looseMuLowerPlusPtEtaMinusPhiPlus->Fill( lowTrackerMuonP4->pt());
		  h_looseMuLowerPlusEtaEtaMinusPhiPlus->Fill(lowTrackerMuonP4->eta());
		  h_looseMuLowerPlusPhiEtaMinusPhiPlus->Fill(lowTrackerMuonP4->phi());
		  
		  h_looseMuLowerPlusPtErrorEtaMinusPhiPlus->Fill( *lowTrackerPtError);
		  h_looseMuLowerPlusPtRelErrEtaMinusPhiPlus->Fill( lowerRelPtErr);
		  h_looseMuLowerPlusTrackPtEtaMinusPhiPlus->Fill(  sqrt(lowTrackerTrack->perp2()));
		  h_looseMuLowerPlusTrackEtaEtaMinusPhiPlus->Fill( lowTrackerTrack->eta());
		  h_looseMuLowerPlusTrackPhiEtaMinusPhiPlus->Fill( lowTrackerTrack->phi());
		  
		  h_looseMuLowerPlusPixelHitsEtaMinusPhiPlus->Fill(                   *lowTrackerPhits);
		  h_looseMuLowerPlusTrackerHitsEtaPlus->Fill(                 *lowTrackerThits);
		  h_looseMuLowerPlusValidHitsEtaPlus->Fill(                   *lowTrackerValidHits);
		  h_looseMuLowerPlusValidMuonHitsEtaMinusPhiPlus->Fill(               *lowTrackerValidMuonHits);
		  h_looseMuLowerPlusMuonStationHitsEtaMinusPhiPlus->Fill(             *lowTrackerMhits);
		  h_looseMuLowerPlusMatchedMuonStationsEtaMinusPhiPlus->Fill(         *lowTrackerMatchedMuonStations);
		  h_looseMuLowerPlusTrackerLayersWithMeasurementEtaMinusPhiPlus->Fill(*lowTrackerLayersWithMeasurement);

		}

	      }
	      if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaAllPhiMinus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		h_looseMuLowerPlusCurveEtaAllPhiZero->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      }
	      else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaAllPhiPlus->Fill(  symmetric_?lowerCpT:fabs(lowerCpT));
	      
	      

	      // loose doesn't include the dxy/dz cuts
	      h_looseMuPlusDxy->Fill(     *lowTrackerDxy);
	      h_looseMuLowerPlusDxy->Fill(*lowTrackerDxy);
	      h_looseMuPlusDz->Fill(      *lowTrackerDz);
	      h_looseMuLowerPlusDz->Fill( *lowTrackerDz);
	      h_looseMuPlusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuLowerPlusDxyError->Fill(*lowTrackerDxyError);
	      h_looseMuPlusDzError->Fill(      *lowTrackerDzError);
	      h_looseMuLowerPlusDzError->Fill( *lowTrackerDzError);

	      h_looseMuPlusPt->Fill(      lowTrackerMuonP4->pt());
	      h_looseMuLowerPlusPt->Fill( lowTrackerMuonP4->pt());
	      h_looseMuPlusEta->Fill(     lowTrackerMuonP4->eta());
	      h_looseMuLowerPlusEta->Fill(lowTrackerMuonP4->eta());
	      h_looseMuPlusPhi->Fill(     lowTrackerMuonP4->phi());
	      h_looseMuLowerPlusPhi->Fill(lowTrackerMuonP4->phi());

	      h_looseMuPlusTrackPt->Fill(     sqrt(lowTrackerTrack->perp2()));
	      h_looseMuLowerPlusTrackPt->Fill(sqrt(lowTrackerTrack->perp2()));
	      h_looseMuPlusTrackEta->Fill(         lowTrackerTrack->eta());
	      h_looseMuLowerPlusTrackEta->Fill(    lowTrackerTrack->eta());
	      h_looseMuPlusTrackPhi->Fill(         lowTrackerTrack->phi());
	      h_looseMuLowerPlusTrackPhi->Fill(    lowTrackerTrack->phi());

	      h_looseMuPlusValidHits->Fill(     *lowTrackerValidHits);
	      h_looseMuLowerPlusValidHits->Fill(*lowTrackerValidHits);

	      for (int i = 0; i < nBiasBins; ++i) {
		double posBias = lowerCpT+(i+1)*(factor_*maxBias/nBiasBins);
		double negBias = lowerCpT-(i+1)*(factor_*maxBias/nBiasBins);
		h_looseMuPlusCurvePlusBias[i]->Fill(      symmetric_?posBias:fabs(posBias));
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (lowTrackerTrack->eta() < 0){
		  h_looseMuPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuPlusCurveMinusBias[i]->Fill(     symmetric_?negBias:fabs(negBias));	
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuPlusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuPlusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
	      else if (lowTrackerTrack->eta() < 0){
		h_looseMuPlusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuPlusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuPlusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuPlusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		h_looseMuLowerPlusCurvePlusBias[i]->Fill( symmetric_?posBias:fabs(posBias));
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuLowerPlusCurveEtaPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaPlusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerPlusCurveEtaPlusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaPlusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		
		
		else if (lowTrackerTrack->eta() < 0){
		  h_looseMuLowerPlusCurveEtaMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaMinusPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerPlusCurveEtaMinusPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaMinusPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaAllPhiMinusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerPlusCurveEtaAllPhiZeroPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		}
	      else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaAllPhiPlusPlusBias[i]->Fill(  symmetric_?posBias:fabs(posBias));
		
		
		h_looseMuLowerPlusCurveMinusBias[i]->Fill(symmetric_?negBias:fabs(negBias));	
		if (lowTrackerTrack->eta() > 0){
		  h_looseMuLowerPlusCurveEtaPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaPlusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		    h_looseMuLowerPlusCurveEtaPlusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		  }
		  else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaPlusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		
		
	      else if (lowTrackerTrack->eta() < 0){
		h_looseMuLowerPlusCurveEtaMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaMinusPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerPlusCurveEtaMinusPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		}
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaMinusPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
		if(lowTrackerTrack->phi() < -1.0472) h_looseMuLowerPlusCurveEtaAllPhiMinusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		else if((-1.0472 <= lowTrackerTrack->phi()) && (lowTrackerTrack->phi() <= 1.0472)){
		  h_looseMuLowerPlusCurveEtaAllPhiZeroMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
	      }
		else if(lowTrackerTrack->phi() > 1.0472) h_looseMuLowerPlusCurveEtaAllPhiPlusMinusBias[i]->Fill(  symmetric_?negBias:fabs(negBias));
		
		
		
		if (debug)
		  std::cout << "Made it through the lower bias loop " << i << std::endl; 
	      }
	    } // end if (low_n1pt)
	    
	    if (low_n1pixhits) {
	      h_looseMuPlusPixelHits->Fill(     *lowTrackerPhits);
	      h_looseMuLowerPlusPixelHits->Fill(*lowTrackerPhits);
	    }
	    if (low_n1vmuhits) {
	      h_looseMuPlusValidMuonHits->Fill(       *lowTrackerValidMuonHits);
	      h_looseMuLowerPlusValidMuonHits->Fill(  *lowTrackerValidMuonHits);
	      h_looseMuPlusMuonStationHits->Fill(     *lowTrackerMhits);
	      h_looseMuLowerPlusMuonStationHits->Fill(*lowTrackerMhits);
	    }
	    if (low_n1ptrelerr) {
	      h_looseMuPlusPtError->Fill(     *lowTrackerPtError);
	      h_looseMuLowerPlusPtError->Fill(*lowTrackerPtError);
	      h_looseMuPlusPtRelErr->Fill(     lowerRelPtErr);
	      h_looseMuLowerPlusPtRelErr->Fill(lowerRelPtErr);
	    }
	    if (low_n1trkhits) {
	      h_looseMuPlusTrackerHits->Fill(     *lowTrackerThits);
	      h_looseMuLowerPlusTrackerHits->Fill(*lowTrackerThits);
	      h_looseMuPlusTrackerLayersWithMeasurement->Fill(     *lowTrackerLayersWithMeasurement);
	      h_looseMuLowerPlusTrackerLayersWithMeasurement->Fill(*lowTrackerLayersWithMeasurement);
	    }
	    if (low_n1mmustahits) {
	      h_looseMuPlusMatchedMuonStations->Fill(     *lowTrackerMatchedMuonStations);
	      h_looseMuLowerPlusMatchedMuonStations->Fill(*lowTrackerMatchedMuonStations);
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
