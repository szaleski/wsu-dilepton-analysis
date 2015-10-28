#ifndef __HISTOGRAMMAKER_H__
#define __HISTOGRAMMAKER_H__

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
#include "TPad.h"
#include "TPaveStats.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>

// forward declare ROOT things
class TH1D;
class TFile;
class TTree;

namespace wsu {
  namespace dileptons {
    namespace cosmics {
      
      struct HighPtMuonCuts {
	double ptRelErr;
      } HighPtMuonCuts;
      
      struct ConfigurationParameters {
	int NBiasBins;
	double MaxKBias;
	double MinPtCut;
	std::string Arbitration; // 
	std::string TrackAlgo;   // Tracker, TPFMS, DYT, Picky, TuneP
	std::string MuonLeg;     // upper, lower, combined
	HighPtMuonCuts muCuts;
      } ConfigurationParameters;
      
      class HistogramMaker {
	
      public: 
	HistogramMaker(std::string const& fileList, std::string const& outFileName, std::string const& confParmsFile);
	~HistogramMaker();
	
	void parseConfiguration(std::string const& confFileName);
	
	void Plot(TTree* inputTree,
		  std::string const& inputFiles,
		  std::string const& file1);

      private:
	float maxBias, mMin, mMax;
	int nBiasBins, massBinSize;
	
	TTreeReader* treeReader;

	TFile *outFile;
	
	// histograms
	// [3] for upper, lower, and combined
	// [2] for plus/minus
	TH1F *h_Chi2[3][2],   *h_Ndof[3][2];
	TH1F *h_Charge[3][2], *h_Curve[3][2];
	TH1F *h_Dxy[3][2], *h_Dz[3][2],      *h_DxyError[3][2], *h_DzError [3][2];
	TH1F *h_Pt[3][2],  *h_TrackPt[3][2], *h_PtError[3][2],  *h_TrackEta[3][2], *h_TrackPhi[3][2];

	TH1F *h_TkHits[3][2], *h_PixelHits[3][2], *h_ValidHits[3][2];
	TH1F *h_MuonStationHits[3][2], *h_MatchedMuonStations[3][2], *h_TkLayersWithMeasurement[3][2];
	
	TH1F *h_CurvePlusBias[3][2][nBiasBins];
	TH1F *h_CurveMinusBias[3][2][nBiasBins];
	
	ConfigurationParameters confParams;
      }; // end class HistogramMaker
    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu

#endif
