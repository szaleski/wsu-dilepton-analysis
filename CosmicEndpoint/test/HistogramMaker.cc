#include "HistogramMaker.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/Vector3D.h"

#include "TLorentzVector.h"
#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TVector2.h"
#include "TString.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <math.h>


namespace wsu {
  namespace dileptons {
    namespace cosmics {
      
      //typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > Lorentz4V;
      //typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> Vector3;
      typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> Vector3;

      HistogramMaker::HistogramMaker(std::string const& fileList,
				     std::string const& outFileName,
				     std::string const& confParmsFile,
				     int debug) :
	m_confParams(),
	m_inFileList(fileList),
	m_outFileName(outFileName),
	m_confFileName(confParmsFile),
	m_debug(debug)
      {
	if (m_debug > 2)
	  std::cout << "HistogramMaker constructed with:"    << std::endl
		    << "m_inFileList = "   << m_inFileList   << std::endl
		    << "m_outFileName = "  << m_outFileName  << std::endl
		    << "m_confFileName = " << m_confFileName << std::endl
		    << "m_debug = "        << m_debug        << std::endl << std::flush;
	
	std::cout << "Parsing config file " << confParmsFile << std::endl;
	parseConfiguration(confParmsFile);
	
	std::cout << "Parsing input file list " << fileList << std::endl;
	parseFileList(fileList);
	
	std::cout << "Setting local varaiables " << std::endl;

	std::string legs[3]   = {"up", "low", "comb"};
	std::string charge[2] = {"muon", "antiMuon"};
	
	// draw all plots inclusive, and then with the following binning
	double ptBinMin[12] = {50., 100., 150., 200., 250., 300., 400., 500., 750., 1000., 1500., 2000.};

	//m_outFile = std::shared_ptr<TFile>(new TFile(TString(outFileName+".root"),"RECREATE"));
	m_outFile = std::shared_ptr<TDirectoryFile>(new TDirectoryFile(TString(outFileName+".root"),"RECREATE"));

	for (int leg = 0; leg < 3; ++leg) {
	  for (int ch = 0; ch < 2; ++ch) {
	    for (int ptb = 0; ptb < 13; ++ptb) {
	      std::stringstream ptbinlabel;
	      if (ptb == 12)
		ptbinlabel << "inclusive";
	      else if (ptb < 12)
		ptbinlabel << ptBinMin[ptb] << "to" << ptBinMin[ptb+1];
	      else
		ptbinlabel << ptBinMin[ptb] << "toInf";

	      m_outFile->mkdir(TString(ptbinlabel.str()));
	      //TDirectory* ptbinDir = new TDirectory(TString(ptbinlabel.str()),TString(ptbinlabel.str()));
	      //TDirectory* ptbinDir = m_outFile->mkdir(TString(ptbinlabel.str()));
	      m_ptBinDir = std::shared_ptr<TDirectory>(m_outFile->mkdir(TString(ptbinlabel.str())));
	      m_ptBinDir->cd();
	      
	      h_Chi2[leg][ch][ptb]     = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Chi2"),    "#Chi^{2}",         100,   -0.5,   99.5 ));
	      h_Ndof[leg][ch][ptb]     = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Ndof"),    "N d.o.f.",         100,   -0.5,   99.5 ));
	      h_Chi2Ndof[leg][ch][ptb] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Chi2Ndof"),"#Chi^{2}/N d.o.f.",100,   -0.5,   99.5 ));
	      h_Charge[leg][ch][ptb]   = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Charge"),  "q",                3,     -1.5,   1.5  ));
	      h_Curve[leg][ch][ptb]    = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Curve"),   "#frac{q}{p_{T}}",  2*500, -0.05,  0.05 ));
	      h_Dxy[leg][ch][ptb]      = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Dxy"),     "D_{xy}",           2*500, -1000., 1000.));
	      h_Dz[leg][ch][ptb]       = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Dz"),      "D_{z}",            2*500, -1000., 1000.));
	      h_DxyError[leg][ch][ptb] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_DxyError"),"#DeltaD_{xy}",     100,   -100.,  100. ));
	      h_DzError[leg][ch][ptb]  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_DzError"), "#DeltaD_{z}",      100,   -100.,  100. ));
	      h_Pt[leg][ch][ptb]       = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_Pt"),      "p_{T}",            300,    0.,    3000.));
	      h_TrackPt[leg][ch][ptb]  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_TrackPt"), "p_{T}",            300,    0.,    3000.));
	      h_PtError[leg][ch][ptb]  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_PtError"), "#Deltap_{T}",      500,    0.,    500. ));
	      h_TrackEta[leg][ch][ptb] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_TrackEta"),"#eta",             2*100, -5.,    5.   ));
	      h_TrackPhi[leg][ch][ptb] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_TrackPhi"),"#phi",             2*50,  -4.,    4.   ));
	    
	      h_PixelHits[leg][ch][ptb]               = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_PixelHits"),         "N_{pix. hits}",
										       100, -0.5, 99.5 ));
	      h_TkHits[leg][ch][ptb]                  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_TkHits"),            "N_{trk. hits}",
										       100, -0.5, 99.5 ));
	      h_MuonStationHits[leg][ch][ptb]         = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_MuonStationHits"),   "N_{station hits}",
										       20,  -0.5, 19.5 ));
	      h_ValidHits[leg][ch][ptb]               = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_ValidHits"),         "N_{hits}",
										       200, -0.5, 199.5));
	      h_MatchedMuonStations[leg][ch][ptb]     = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_MatchedMuonStation"),"N_{matched stations}",
										       20,  -0.5, 19.5 ));
	      h_TkLayersWithMeasurement[leg][ch][ptb] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_TkLayersWithMeas"),  "N_{trk. layer w/ meas.}",
										       200, -0.5, 199.5));
	    
	      h_Chi2[leg][ch][ptb]    ->Sumw2();
	      h_Ndof[leg][ch][ptb]    ->Sumw2();
	      h_Chi2Ndof[leg][ch][ptb]->Sumw2();
	      h_Charge[leg][ch][ptb]  ->Sumw2();
	      h_Curve[leg][ch][ptb]   ->Sumw2();
	      h_Dxy[leg][ch][ptb]     ->Sumw2();
	      h_Dz[leg][ch][ptb]      ->Sumw2();
	      h_DxyError[leg][ch][ptb]->Sumw2();
	      h_DzError[leg][ch][ptb] ->Sumw2();
	      h_Pt[leg][ch][ptb]      ->Sumw2();
	      h_TrackPt[leg][ch][ptb] ->Sumw2();
	      h_PtError[leg][ch][ptb] ->Sumw2();
	      h_TrackEta[leg][ch][ptb]->Sumw2();
	      h_TrackPhi[leg][ch][ptb]->Sumw2();
	    
	      h_PixelHits[leg][ch][ptb]              ->Sumw2();
	      h_TkHits[leg][ch][ptb]                 ->Sumw2();
	      h_MuonStationHits[leg][ch][ptb]        ->Sumw2();
	      h_ValidHits[leg][ch][ptb]              ->Sumw2();
	      h_MatchedMuonStations[leg][ch][ptb]    ->Sumw2();
	      h_TkLayersWithMeasurement[leg][ch][ptb]->Sumw2();
	    
	      for (int i = 0; i < m_nBiasBins; ++i) {
		std::stringstream name;
		double biasValue = (m_maxBias/m_nBiasBins)*(i+1);
		name << std::setw(3) << std::setfill('0') << i + 1;

		// inject positive bias
		TString histname  = TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_CurvePlusBias_"+name.str());
		name.str("");
		name.clear();
		name << biasValue;
		TString histtitle("#frac{q}{p_{T}}+#Delta#kappa("+name.str()+")");
		if (m_debug > 3)
		  std::cout << "creating histogram " << histname << " " << histtitle << std::endl << std::flush;
		h_CurvePlusBias[leg][ch][ptb][i] = std::shared_ptr<TH1D>(new TH1D(histname, histtitle, 2*500, -0.05, 0.05));
		h_CurvePlusBias[leg][ch][ptb][i]->Sumw2();

		// inject negative bias
		name.str("");
		name.clear();
		name << std::setw(3) << std::setfill('0') << i + 1;
		TString histname2  = TString(charge[ch]+"_"+legs[leg]+"_"+ptbinlabel.str()+"_CurveMinusBias_"+name.str());
		name.str("");
		name.clear();
		name << biasValue;
		TString histtitle2 = TString("#frac{q}{p_{T}}-#Delta#kappa("+name.str()+")");
		if (m_debug > 3)
		  std::cout << "trying to create problematic histogram " << histname2 << " " << histtitle2 << std::endl << std::flush;
		h_CurveMinusBias[leg][ch][ptb][i] = std::shared_ptr<TH1D>(new TH1D(histname2, histtitle2, 2*500, -0.05, 0.05));
		h_CurveMinusBias[leg][ch][ptb][i]->Sumw2();
	      }
	    }// end loop on pt binnings
	  }// end loop on different charge histograms
	}// end loop on different muon leg histograms
	std::cout << "Ending the HistogramMaker constructor" << std::endl << std::flush;
      } // end constructor


      HistogramMaker::~HistogramMaker()
      {
	std::cout << "HistogramMaker destructor called" << std::endl << std::flush;
	m_outFile->Save();
	m_outFile->Write();
	m_outFile->Close();
	std::cout << "Wrote and closed output file 0x" << std::hex << m_outFile.get()
		  << std::dec << std::endl << std::flush;

	std::cout << "HistogramMaker destructor finished" << std::endl << std::flush;
      } // end destructor


      int HistogramMaker::runLoop(int debug)
      {
	return Plot(m_tree.get());
      } // end runLoop(int)
      
      
      void HistogramMaker::parseConfiguration(std::string const& confFileName)
      {
	std::ifstream infile(confFileName.c_str());
	if (!infile.is_open()) {
	  std::cout << "Problem opening file " << confFileName << std::endl << std::flush;
	  return; // maybe should exit(1) here?
	}
 
	std::cout << "Configuration file " << confFileName 
		  << " opened successfully, parsing"
		  << std::endl << std::flush;

	std::string line;
	while (std::getline(infile,line)) {
	  if (m_debug > 7)
	    std::cout << "line " << line << std::endl << std::flush;

	  if (line.find("#") == 0 || line.find("!") == 0)
	    continue; // found comment character at the front, skip processing
	  
	  // looking for NBiasBins, MaxKBias, MinPtCut, Arbitration, TrackAlgo, MuonLeg, MuCuts
	  std::istringstream nocomments(line);
	  std::string none;
	  std::getline(nocomments,none,'#');
	  nocomments.str("");
	  nocomments.clear();
	  nocomments.str(none);
	  std::string goodline;
	  std::getline(nocomments,goodline,'!');
	  
	  nocomments.str("");
	  nocomments.clear();
	  nocomments.str(goodline);
	  std::string key, value;
	  std::getline(nocomments,key,'=');
	  std::getline(nocomments,value);
	  std::size_t valstart = value.find_first_not_of(" ");
	  if (valstart != std::string::npos)
	    value = value.substr(valstart);

	  std::istringstream valstream(value);
	  
	  if (m_debug > 5)
	    std::cout << "key "   << key   << std::endl
		      << "value " << value << std::endl
		      << std::flush;
	  

	  if (key.find("NBiasBins") != std::string::npos) {
	    if (m_debug > 2)
	      std::cout << "using NBiasBins " << valstream.str() << std::endl << std::flush;;
	    valstream >> m_confParams.NBiasBins;
	  } else if (key.find("MaxKBias") != std::string::npos) {
	    if (m_debug > 2)
	      std::cout << "using MaxKBias " << valstream.str() << std::endl << std::flush;;
	    valstream >> m_confParams.MaxKBias;
	  } else if (key.find("MinPtCut") != std::string::npos) {
	    if (m_debug > 2)
	      std::cout << "using MinPtCut " << valstream.str() << std::endl << std::flush;;
	    valstream >> m_confParams.MinPtCut;
	  } else if (key.find("Arbitration") != std::string::npos) {
	    if (m_debug > 2)
	      std::cout << "using Arbitration " << value << std::endl << std::flush;
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    m_confParams.Arbitration = value;
	  } else if (key.find("TrackAlgo") != std::string::npos) {
	    //std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    if (m_debug > 2)
	      std::cout << "using TrackAlgo " << value << std::endl << std::flush;
	    m_confParams.TrackAlgo = value;
	  } else if (key.find("MuonLeg") != std::string::npos) {
	    if (m_debug > 2)
	      std::cout << "using MuonLeg " << value << std::endl << std::flush;
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    m_confParams.MuonLeg = value;
	  } else if (key.find("PathPrefix") != std::string::npos) {
	    if (m_debug > 2)
	      std::cout << "using PathPrefix " << value << std::endl << std::flush;
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    m_confParams.PathPrefix = value;
	  }
	}
	
	//set the member variables as taken from the config
	m_nBiasBins = m_confParams.NBiasBins;
	m_maxBias   = m_confParams.MaxKBias;
	m_minPt     = m_confParams.MinPtCut;

	std::stringstream treeName;
	treeName << "analysis" << m_confParams.TrackAlgo << "Muons/MuonTree";
	std::cout << "looking for TTree " << treeName.str() << std::endl << std::flush;
	m_treeChain  = std::shared_ptr<TChain>(new TChain(TString(treeName.str())));

	std::cout << "done with parseConfiguration " << std::endl << std::flush;
	return;
      } // end parseConfiguration(std::string)
      
      void HistogramMaker::parseFileList(std::string const& fileList)
      {
	std::ifstream infile(fileList.c_str());
	if (!infile.is_open()) {
	  std::cout << "Problem opening file " << fileList << std::endl << std::flush;
	  return; // maybe should exit(1) here?
	}
 
	std::string line;
	while (std::getline(infile,line)) {
	  if (line.find("#") == 0 || line.find("!") == 0)
	    continue; // found comment, skip processing, only at very beginning of line
	  if (line.find(".root") == std::string::npos)
	    continue; // found line without .root file name, skip
	  
	  std::stringstream filepath;
	  filepath << m_confParams.PathPrefix << line; // need to strip off the newline?
	  std::cout << filepath.str() << std::endl << std::flush;
	  m_treeChain->Add(TString(filepath.str()));
	}// end while loop over lines in file
	
	m_tree = m_treeChain;

	if (m_debug > 5) {
	  std::cout << "m_treeChain has the following files:" << std::endl;
	  auto chainIter = m_tree->GetListOfLeaves()->MakeIterator();
	  do {
	    TChain* chainItem = (TChain*)chainIter->Next();
	    std::cout << chainItem->GetName() << " "
		      << chainItem->GetTitle() << " "
		      << chainItem->ClassName() << " "
		      << std::endl;
	  } while (chainIter->Next());
	  
	  std::cout << "m_tree has the following leaves:"      << std::endl;
	  auto treeIter = m_tree->GetListOfLeaves()->MakeIterator();
	  do {
	    TTree* treeItem = (TTree*)treeIter->Next();
	    std::cout << treeItem->GetName() << " "
		      << treeItem->GetTitle() << " "
		      << treeItem->ClassName() << " "
		      << std::endl;
	  } while (treeIter->Next());
	}
	return;
      } // end parseFileList(std::string)
      

      int HistogramMaker::Plot(TTree* intree)
      {
	m_treeReader = std::shared_ptr<TTreeReader>(new TTreeReader(intree));

	if (m_debug > 5) {
	  std::cout << "intree has the following leaves:" << std::endl;
	  auto treeIter = intree->GetListOfLeaves()->MakeIterator();
	  do {
	    TTree* treeItem = (TTree*)treeIter->Next();
	    std::cout << treeItem->GetName() << " "
		      << treeItem->GetTitle() << " "
		      << treeItem->ClassName() << " "
		      << std::endl;
	  } while (treeIter->Next());
	  
	  std::cout << "m_treeReader has the following leaves:" << std::endl;
	  treeIter = m_treeReader->GetTree()->GetListOfLeaves()->MakeIterator();
	  do {
	    TTree* treeItem = (TTree*)treeIter->Next();
	    std::cout << treeItem->GetName() << " "
		      << treeItem->GetTitle() << " "
		      << treeItem->ClassName() << " "
		      << std::endl;
	  } while (treeIter->Next());
	}

	TTreeReaderValue<Double_t> upTrackPt(       *m_treeReader, "upperMuon_trackPt"          );
	TTreeReaderValue<Double_t> upTrackDxy(      *m_treeReader, "upperMuon_dxy"              );
	TTreeReaderValue<Double_t> upTrackDz(       *m_treeReader, "upperMuon_dz"               );
	TTreeReaderValue<Int_t>    upTrackPhits(    *m_treeReader, "upperMuon_pixelHits"        );
	TTreeReaderValue<Int_t>    upTrackCharge(   *m_treeReader, "upperMuon_charge"           );
	TTreeReaderValue<Int_t>    upTrackThits(    *m_treeReader, "upperMuon_trackerHits"      );
	TTreeReaderValue<Int_t>    upTrackMhits(    *m_treeReader, "upperMuon_muonStationHits"  );
	TTreeReaderValue<Double_t> upTrackChi2(     *m_treeReader, "upperMuon_chi2"             );
	TTreeReaderValue<Int_t>    upTrackNdof(     *m_treeReader, "upperMuon_ndof"             );
	TTreeReaderValue<Int_t>    upTrackValidHits(*m_treeReader, "upperMuon_numberOfValidHits");
	TTreeReaderValue<Double_t> upTrackDxyError( *m_treeReader, "upperMuon_dxyError"         );
	TTreeReaderValue<Double_t> upTrackDzError(  *m_treeReader, "upperMuon_dzError"          );
	TTreeReaderValue<Double_t> upTrackPtError(  *m_treeReader, "upperMuon_ptError"          );
	TTreeReaderValue<Int_t>    upTrackMatchedMuonStations(    *m_treeReader,"upperMuon_numberOfMatchedStations"     );
	TTreeReaderValue<Int_t>    upTrackTkLayersWithMeasurement(*m_treeReader,"upperMuon_trackerLayersWithMeasurement");

	TTreeReaderValue<math::XYZTLorentzVector> upMuonP4(  *m_treeReader,"upperMuon_P4");
	// still not working, thanks to ROOT...
	TTreeReaderValue<math::XYZVector>         upTrackVec(*m_treeReader,"upperMuon_trackVec");

	TTreeReaderValue<Double_t> lowTrackPt(       *m_treeReader, "lowerMuon_trackPt"          );
	TTreeReaderValue<Double_t> lowTrackDxy(      *m_treeReader, "lowerMuon_dxy"              );
	TTreeReaderValue<Double_t> lowTrackDz(       *m_treeReader, "lowerMuon_dz"               );
	TTreeReaderValue<Int_t>    lowTrackPhits(    *m_treeReader, "lowerMuon_pixelHits"        );
	TTreeReaderValue<Int_t>    lowTrackCharge(   *m_treeReader, "lowerMuon_charge"           );
	TTreeReaderValue<Int_t>    lowTrackThits(    *m_treeReader, "lowerMuon_trackerHits"      );
	TTreeReaderValue<Int_t>    lowTrackMhits(    *m_treeReader, "lowerMuon_muonStationHits"  );
	TTreeReaderValue<Double_t> lowTrackChi2(     *m_treeReader, "lowerMuon_chi2"             );
	TTreeReaderValue<Int_t>    lowTrackNdof(     *m_treeReader, "lowerMuon_ndof"             );
	TTreeReaderValue<Int_t>    lowTrackValidHits(*m_treeReader, "lowerMuon_numberOfValidHits");
	TTreeReaderValue<Double_t> lowTrackDxyError( *m_treeReader, "lowerMuon_dxyError"         );
	TTreeReaderValue<Double_t> lowTrackDzError(  *m_treeReader, "lowerMuon_dzError"          );
	TTreeReaderValue<Double_t> lowTrackPtError(  *m_treeReader, "lowerMuon_ptError"          );
	TTreeReaderValue<Int_t>    lowTrackMatchedMuonStations(    *m_treeReader,"lowerMuon_numberOfMatchedStations"     );
	TTreeReaderValue<Int_t>    lowTrackTkLayersWithMeasurement(*m_treeReader,"lowerMuon_trackerLayersWithMeasurement");
	
	TTreeReaderValue<math::XYZTLorentzVector> lowMuonP4(  *m_treeReader,"lowerMuon_P4");
	// still not working, thanks to ROOT...
	TTreeReaderValue<math::XYZVector>         lowTrackVec(*m_treeReader,"lowerMuon_trackVec");
	
	int j = 0;
	std::cout << "looping over entries in the TTree" << std::endl << std::flush;
	while (m_treeReader->Next()){
	  if (*upTrackChi2 > -1000) { //ensure values are from an actual event
	    int combLow[2] = {1,2};
	    int combUp[2] = {0,2};
	    for (int fill = 0; fill < 2; ++fill) {
	      int charge = *upTrackCharge;
	      double upperTrackPt = sqrt(upTrackVec->perp2());
	      double upperTrackEta = upTrackVec->eta();
	      double upperTrackPhi = upTrackVec->phi();
	      double upperCpT = (*upTrackCharge)/(upperTrackPt);
	      h_Chi2[combUp[fill]][     (charge<0)?0:1][13]->Fill(*upTrackChi2);
	      h_Ndof[combUp[fill]][     (charge<0)?0:1][13]->Fill(*upTrackNdof);
	      h_Chi2Ndof[combUp[fill]][ (charge<0)?0:1][13]->Fill((*upTrackChi2)/(*upTrackNdof));
	      h_Pt[combUp[fill]][       (charge<0)?0:1][13]->Fill(*upTrackPt);
	      h_Charge[combUp[fill]][   (charge<0)?0:1][13]->Fill(*upTrackCharge);
	      h_Curve[combUp[fill]][    (charge<0)?0:1][13]->Fill(upperCpT);
	      h_Dxy[combUp[fill]][      (charge<0)?0:1][13]->Fill(*upTrackDxy);
	      h_Dz[combUp[fill]][       (charge<0)?0:1][13]->Fill(*upTrackDz);
	      h_PtError[combUp[fill]][  (charge<0)?0:1][13]->Fill(*upTrackPtError);
	      h_DxyError[combUp[fill]][ (charge<0)?0:1][13]->Fill(*upTrackDxyError);
	      h_DzError[combUp[fill]][  (charge<0)?0:1][13]->Fill(*upTrackDzError);
	      h_TrackPt[combUp[fill]][  (charge<0)?0:1][13]->Fill(upperTrackPt);
	      h_TrackEta[combUp[fill]][ (charge<0)?0:1][13]->Fill(upperTrackEta);
	      h_TrackPhi[combUp[fill]][ (charge<0)?0:1][13]->Fill(upperTrackPhi);
	      
	      h_PixelHits[combUp[fill]][              (charge<0)?0:1][13]->Fill(*upTrackPhits);
	      h_TkHits[combUp[fill]][                 (charge<0)?0:1][13]->Fill(*upTrackThits);
	      h_MuonStationHits[combUp[fill]][        (charge<0)?0:1][13]->Fill(*upTrackMhits);
	      h_ValidHits[combUp[fill]][              (charge<0)?0:1][13]->Fill(*upTrackValidHits);
	      h_MatchedMuonStations[combUp[fill]][    (charge<0)?0:1][13]->Fill(*upTrackMatchedMuonStations);
	      h_TkLayersWithMeasurement[combUp[fill]][(charge<0)?0:1][13]->Fill(*upTrackTkLayersWithMeasurement);
	      
	      for (int i = 0; i < m_nBiasBins; ++i) {
		h_CurvePlusBias[combUp[fill]][ (charge<0)?0:1][13][i]->Fill(upperCpT+(i+1)*(m_maxBias/m_nBiasBins));
		h_CurveMinusBias[combUp[fill]][(charge<0)?0:1][13][i]->Fill(upperCpT-(i+1)*(m_maxBias/m_nBiasBins));
	      }
	      
	      charge = *lowTrackCharge;
	      double lowerTrackPt = sqrt(lowTrackVec->perp2());
	      double lowerTrackEta = lowTrackVec->eta();
	      double lowerTrackPhi = lowTrackVec->phi();
	      double lowerCpT = (*lowTrackCharge)/(lowerTrackPt);
	      h_Chi2[combLow[fill]][    (charge<0)?0:1][13]->Fill(*lowTrackChi2);
	      h_Ndof[combLow[fill]][    (charge<0)?0:1][13]->Fill(*lowTrackNdof);
	      h_Chi2Ndof[combLow[fill]][(charge<0)?0:1][13]->Fill((*lowTrackChi2)/(*lowTrackNdof));
	      h_Pt[combLow[fill]][      (charge<0)?0:1][13]->Fill(*lowTrackPt);
	      h_Charge[combLow[fill]][  (charge<0)?0:1][13]->Fill(*lowTrackCharge);
	      h_Curve[combLow[fill]][   (charge<0)?0:1][13]->Fill(lowerCpT);
	      h_Dxy[combLow[fill]][     (charge<0)?0:1][13]->Fill(*lowTrackDxy);
	      h_Dz[combLow[fill]][      (charge<0)?0:1][13]->Fill(*lowTrackDz);
	      h_PtError[combLow[fill]][ (charge<0)?0:1][13]->Fill(*lowTrackPtError);
	      h_DxyError[combLow[fill]][(charge<0)?0:1][13]->Fill(*lowTrackDxyError);
	      h_DzError[combLow[fill]][ (charge<0)?0:1][13]->Fill(*lowTrackDzError);
	      h_TrackPt[combLow[fill]][ (charge<0)?0:1][13]->Fill(lowerTrackPt);
	      h_TrackEta[combLow[fill]][(charge<0)?0:1][13]->Fill(lowerTrackEta);
	      h_TrackPhi[combLow[fill]][(charge<0)?0:1][13]->Fill(lowerTrackPhi);
	      
	      h_PixelHits[combLow[fill]][          (charge<0)?0:1][13]->Fill(*lowTrackPhits);
	      h_TkHits[combLow[fill]][             (charge<0)?0:1][13]->Fill(*lowTrackThits);
	      h_MuonStationHits[combLow[fill]][    (charge<0)?0:1][13]->Fill(*lowTrackMhits);
	      h_ValidHits[combLow[fill]][          (charge<0)?0:1][13]->Fill(*lowTrackValidHits);
	      h_MatchedMuonStations[combLow[fill]][(charge<0)?0:1][13]->Fill(*lowTrackMatchedMuonStations);
	      h_TkLayersWithMeasurement[combLow[fill]][(charge<0)?0:1][13]->Fill(*lowTrackTkLayersWithMeasurement);
	      
	      for (int i = 0; i < m_nBiasBins; ++i) {
		h_CurvePlusBias[combLow[fill]][ (charge<0)?0:1][13][i]->Fill(lowerCpT+(i+1)*(m_maxBias/m_nBiasBins));
		h_CurveMinusBias[combLow[fill]][(charge<0)?0:1][13][i]->Fill(lowerCpT-(i+1)*(m_maxBias/m_nBiasBins));
	      }
	      ++j;
	    }// closing if fill
	  }// closing for loop over combining plots
	} // end while loop
	std::cout << "done looping over " << j << " entries in the TTree" << std::endl << std::flush;

	return j;
      } // end Plot(TTree*)

    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu
