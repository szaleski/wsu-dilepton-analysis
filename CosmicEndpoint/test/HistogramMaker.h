#ifndef __HISTOGRAMMAKER_H__
#define __HISTOGRAMMAKER_H__

//#include "DataFormats/Math/interface/Vector.h"
//#include "DataFormats/Math/interface/LorentzVector.h"
//#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include <string>
//#include <libconfig.h++>

// forward declare ROOT things
class TH1D;
class TFile;
class TTree;
class TChain;
class TTreeReader;

namespace wsu {
  namespace dileptons {
    namespace cosmics {
      
      typedef struct HighPtMuonCuts {
	int numValidHits;
	int numMatchedStations;
	int numTkLayersWMeas;
	double ptRelErr;
	double dBMax;
	double dZMax;
	double rMax; /** only for super pointing selection */
	double zMax; /** only for super pointing selection */

	HighPtMuonCuts() {
	  numValidHits       = 0;
	  numMatchedStations = 1;
	  numTkLayersWMeas   = 5;
	  ptRelErr = 0.3;
	  dBMax    = 0.2;
	  dZMax    = 0.5;
	  rMax     = 10.; /** only for super pointing selection */
	  zMax     = 50.; /** only for super pointing selection */
	};
      } HighPtMuonCuts;
      
      typedef struct ConfigurationParameters {
	std::string PathPrefix;
	int NBiasBins;
	double MaxKBias;
	double MinPtCut;
	std::string Arbitration; // plus(positive)/minus(negative) as reference
	std::string TrackAlgo;   // Tracker, TPFMS, DYT, Picky, TuneP
	std::string MuonLeg;     // upper, lower, combined
	HighPtMuonCuts MuCuts;
	
	ConfigurationParameters() {
	  PathPrefix  = "root://cms-xrd-global.cern.ch//"; // different xrootd redirectors, or local file
	  NBiasBins   = 100;
	  MaxKBias    = 0.05;
	  MinPtCut    = 100.;
	  Arbitration = "positive"; // plus(positive)/minus(negative) as reference
	  TrackAlgo   = "tunep"; // Tracker, TPFMS, DYT, Picky, TuneP
	  MuonLeg     = "lower"; // upper, lower, combined
	};
      } ConfigurationParameters;
      
      class HistogramMaker {
	
      public: 
	HistogramMaker(std::string const& fileList,
		       std::string const& outFileName,
		       std::string const& confParmsFile,
		       int debug=0);
	~HistogramMaker();
	
	void parseConfiguration(std::string const& confFileName);
	void parseFileList(     std::string const& inputFiles);
	
	void Plot(TTree* inputTree);

      private:
	double maxBias, minPt;
	int nBiasBins, massBinSize;
	
	TTree*       tree;
	TChain*      treeChain;
	TTreeReader* treeReader;

	TFile *outFile;
	
	// histograms
	// [3] for upper, lower, and combined
	// [2] for plus/minus
	TH1D *h_Chi2[3][2],   *h_Ndof[3][2], *h_Chi2Ndof[3][2];
	TH1D *h_Charge[3][2], *h_Curve[3][2];
	TH1D *h_Dxy[3][2],    *h_Dz[3][2],      *h_DxyError[3][2], *h_DzError [3][2];
	TH1D *h_Pt[3][2],     *h_TrackPt[3][2], *h_PtError[3][2],  *h_TrackEta[3][2], *h_TrackPhi[3][2];

	TH1D *h_TkHits[3][2], *h_PixelHits[3][2], *h_ValidHits[3][2];
	TH1D *h_MuonStationHits[3][2], *h_MatchedMuonStations[3][2], *h_TkLayersWithMeasurement[3][2];
	
	TH1D *h_CurvePlusBias[3][2][500];
	TH1D *h_CurveMinusBias[3][2][500];
	
	//libconfig::config_t         *cfg;
	//libconfig::config_setting_t *setting;
	
	ConfigurationParameters confParams;
      }; // end class HistogramMaker
    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu

#endif
