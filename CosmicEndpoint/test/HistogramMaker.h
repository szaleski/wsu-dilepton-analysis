#ifndef __HISTOGRAMMAKER_H__
#define __HISTOGRAMMAKER_H__

//#include "DataFormats/Math/interface/Vector.h"
//#include "DataFormats/Math/interface/LorentzVector.h"
//#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include <string>
#include <memory> /**for smart pointers*/
//#include <libconfig.h++> /**for some config parsing, not working on lxplus*/

// forward declare ROOT things
class TH1D;
class TFile;
class TDirectory;
class TDirectoryFile;

class TTree;
class TChain;
class TTreeReader;

namespace wsu {
  namespace dileptons {
    namespace cosmics {
      
      enum DebugLevel {
	OFF = -1,
	MIN = 0,
	LOW = 1,
	MAX = 19,
	POINTERS       = 1,
	TREEINFO       = 3,
	HISTOGRAMS     = 2,
	BIASHISTOGRAMS = 5,
	CONFIGLINES    = 6,
	CONFIGPARSER   = 5,
	CONFIGPARAMS   = 2,
	EVENTLOOP      = 6
      };
      
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
	  dBMax    = 0.2; /** don't apply for cosmics */
	  dZMax    = 0.5; /** don't apply for cosmics */
	  rMax     = 10.; /** only for super pointing selection */
	  zMax     = 50.; /** only for super pointing selection */
	};
      } HighPtMuonCuts;
      
      typedef struct ConfigurationParameters {
	std::string PathPrefix;
	int NBiasBins;
	bool IsSymmetric;
	double MaxKBias;
	double MinPtCut;
	double Factor;
	std::string Arbitration; // plus(positive)/minus(negative) as reference
	std::string TrackAlgo;   // Tracker, TPFMS, DYT, Picky, TuneP
	std::string MuonLeg;     // upper, lower, combined
	HighPtMuonCuts MuCuts;
	
	ConfigurationParameters() {
	  PathPrefix  = "root://cms-xrd-global.cern.ch//"; // different xrootd redirectors, or local file
	  NBiasBins   = 1000;
	  IsSymmetric = true;
	  MaxKBias    = 0.0005;
	  MinPtCut    = 50.;
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
	
	int runLoop(int debug=0);
	
      private:

	void parseConfiguration(std::string const& confFileName);
	void parseFileList(     std::string const& inputFiles);

	ConfigurationParameters m_confParams;
	std::string m_inFileList, m_outFileName, m_confFileName;
	int m_debug;

	int Plot(TTree* inputTree);

	double m_maxBias, m_minPt;
	int m_nBiasBins;
	
	std::shared_ptr<TTree>       m_tree;
	std::shared_ptr<TChain>      m_treeChain;
	std::shared_ptr<TTreeReader> m_treeReader;
	
	std::shared_ptr<TFile>          m_outFile;
	std::shared_ptr<TDirectory>     m_ptBinDir[13];
	std::shared_ptr<TDirectoryFile> m_outFileD;
	
	// histograms
	// [3] for upper, lower, and combined
	// [2] for plus/minus
	// [12+1] for inclusive, and then pT bins
	//  - {50., 100., 150., 200., 250., 300., 400., 500., 750., 1000., 1500., 2000.}
	std::shared_ptr<TH1D> h_Chi2[3][2][13],   h_Ndof[3][2][13], h_Chi2Ndof[3][2][13];
	std::shared_ptr<TH1D> h_Charge[3][2][13], h_Curve[3][2][13];
	std::shared_ptr<TH1D> h_Dxy[3][2][13],    h_Dz[3][2][13],      h_DxyError[3][2][13], h_DzError [3][2][13];
	std::shared_ptr<TH1D> h_Pt[3][2][13],     h_TrackPt[3][2][13], h_PtError[3][2][13],  h_PtRelErr[3][2][13];
	std::shared_ptr<TH1D> h_TrackEta[3][2][13], h_TrackPhi[3][2][13];

	std::shared_ptr<TH1D> h_TkHits[3][2][13], h_PixelHits[3][2][13], h_ValidHits[3][2][13];
	std::shared_ptr<TH1D> h_MuonStationHits[3][2][13], h_MatchedMuonStations[3][2][13],
	  h_TkLayersWithMeasurement[3][2][13];

	std::shared_ptr<TH1D> h_CurveUpperResidual[3][2][13], h_CurveLowerResidual[3][2][13];
	std::shared_ptr<TH1D> h_CurveUpperPulls[3][2][13],    h_CurveLowerPulls[3][2][13];
	
	// doesn't make sense to bin these vs. pT
	std::shared_ptr<TH1D> h_CurvePlusBias[3][2][1000], h_CurveMinusBias[3][2][1000];
	
      }; // end class HistogramMaker
    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu

#endif
