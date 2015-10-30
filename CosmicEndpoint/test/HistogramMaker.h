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
	
	int runLoop(int debug=0);
	
      private:

	void parseConfiguration(std::string const& confFileName);
	void parseFileList(     std::string const& inputFiles);

	std::string m_inFileList, m_outFileName, m_confFileName;
	bool m_debug;

	int Plot(TTree* inputTree);

	double maxBias, minPt;
	int nBiasBins, massBinSize;
	
	std::shared_ptr<TTree>       m_tree;
	std::shared_ptr<TChain>      m_treeChain;
	std::shared_ptr<TTreeReader> m_treeReader;
	
	//std::shared_ptr<TFile>          m_outFile;
	std::shared_ptr<TDirectory>     m_ptBinDir;
	std::shared_ptr<TDirectoryFile> m_outFile;
	
	// histograms
	// [3] for upper, lower, and combined
	// [2] for plus/minus
	// [12+1] for inclusive, and then pT bins {50., 100., 150., 200., 250., 300., 400., 500., 750., 1000., 1500., 2000.}
	std::shared_ptr<TH1D> h_Chi2[3][2][13],   h_Ndof[3][2][13], h_Chi2Ndof[3][2][13];
	std::shared_ptr<TH1D> h_Charge[3][2][13], h_Curve[3][2][13];
	std::shared_ptr<TH1D> h_Dxy[3][2][13],    h_Dz[3][2][13],      h_DxyError[3][2][13], h_DzError [3][2][13];
	std::shared_ptr<TH1D> h_Pt[3][2][13],     h_TrackPt[3][2][13], h_PtError[3][2][13],  h_TrackEta[3][2][13], h_TrackPhi[3][2][13];

	std::shared_ptr<TH1D> h_TkHits[3][2][13], h_PixelHits[3][2][13], h_ValidHits[3][2][13];
	std::shared_ptr<TH1D> h_MuonStationHits[3][2][13], h_MatchedMuonStations[3][2][13], h_TkLayersWithMeasurement[3][2][13];
	
	std::shared_ptr<TH1D> h_CurvePlusBias[3][2][13][500], h_CurveMinusBias[3][2][13][500]; // what to do when 500 is too many/few?
	
	//libconfig::config_t         *cfg;
	//libconfig::config_setting_t *setting;
	
	ConfigurationParameters confParams;
      }; // end class HistogramMaker
    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu

#endif
