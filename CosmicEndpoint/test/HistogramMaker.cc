#include "HistogramMaker.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/Vector3D.h"

#include "TLorentzVector.h"
#include "TROOT.h"
#include "TFile.h"
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
      
      
      HistogramMaker::HistogramMaker(std::string const& fileList,
				     std::string const& outFileName,
				     std::string const& confParmsFile,
				     int debug)
      {
	std::cout << "HistogramMaker constructed with:"  << std::endl
		  << "fileList = "      << fileList      << std::endl
		  << "outFileName = "   << outFileName   << std::endl
		  << "confParmsFile = " << confParmsFile << std::endl
		  << "debug = "         << debug         << std::endl;
	
	parseConfiguration(confParmsFile);
	parseFileList(fileList);
	
	nBiasBins = confParams.NBiasBins;
	maxBias   = confParams.MaxKBias;
	minPt     = confParams.MinPtCut;

	std::string legs[3]   = {"up", "low", "comb"};
	std::string charge[2] = {"muon", "antiMuon"};
	
	m_outFile = std::shared_ptr<TFile>(new TFile(TString(outFileName+".root"),"RECREATE"));

	for (int leg = 0; leg < 3; ++leg) {
	  for (int ch = 0; ch < 2; ++ch) {
	    // TDirectory* myDirectory = new TDirecto
	    h_Chi2[leg][ch]     = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Chi2"),    "#Chi^{2}",         100,   -0.5,   99.5 ));
	    h_Ndof[leg][ch]     = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Ndof"),    "N d.o.f.",         100,   -0.5,   99.5 ));
	    h_Chi2Ndof[leg][ch] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Chi2Ndof"),"#Chi^{2}/N d.o.f.",100,   -0.5,   99.5 ));
	    h_Charge[leg][ch]   = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Charge"),  "q",                3,     -1.5,   1.5  ));
	    h_Curve[leg][ch]    = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Curve"),   "#frac{q}{p_{T}}",  2*500, -0.1,   0.1  ));
	    h_Dxy[leg][ch]      = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Dxy"),     "D_{xy}",           2*500, -1000., 1000.));
	    h_Dz[leg][ch]       = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Dz"),      "D_{z}",            2*500, -1000., 1000.));
	    h_DxyError[leg][ch] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_DxyError"),"#DeltaD_{xy}",     100,   -100.,  100. ));
	    h_DzError[leg][ch]  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_DzError"), "#DeltaD_{z}",      100,   -100.,  100. ));
	    h_Pt[leg][ch]       = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_Pt"),      "p_{T}",            300,    0.,    3000.));
	    h_TrackPt[leg][ch]  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_TrackPt"), "p_{T}",            300,    0.,    3000.));
	    h_PtError[leg][ch]  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_PtError"), "#Deltap_{T}",      500,    0.,    500. ));
	    h_TrackEta[leg][ch] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_TrackEta"),"#eta",             2*100, -5.,    5.   ));
	    h_TrackPhi[leg][ch] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_TrackPhi"),"#phi",             2*50,  -4.,    4.   ));
	    
	    h_PixelHits[leg][ch]               = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_PixelHits"),         "N_{pix. hits}",
										100, -0.5, 99.5 ));
	    h_TkHits[leg][ch]                  = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_TkHits"),            "N_{trk. hits}",
										100, -0.5, 99.5 ));
	    h_MuonStationHits[leg][ch]         = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_MuonStationHits"),   "N_{station hits}",
										20,  -0.5, 19.5 ));
	    h_ValidHits[leg][ch]               = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_ValidHits"),         "N_{hits}",
										200, -0.5, 199.5));
	    h_MatchedMuonStations[leg][ch]     = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_MatchedMuonStation"),"N_{matched stations}",
										20,  -0.5, 19.5 ));
	    h_TkLayersWithMeasurement[leg][ch] = std::shared_ptr<TH1D>(new TH1D(TString(charge[ch]+"_"+legs[leg]+"_TkLayersWithMeas"),  "N_{trk. layer w/ meas.}",
										200, -0.5, 199.5));
	    
	    h_Chi2[leg][ch]    ->Sumw2();
	    h_Ndof[leg][ch]    ->Sumw2();
	    h_Chi2Ndof[leg][ch]->Sumw2();
	    h_Charge[leg][ch]  ->Sumw2();
	    h_Curve[leg][ch]   ->Sumw2();
	    h_Dxy[leg][ch]     ->Sumw2();
	    h_Dz[leg][ch]      ->Sumw2();
	    h_DxyError[leg][ch]->Sumw2();
	    h_DzError[leg][ch] ->Sumw2();
	    h_Pt[leg][ch]      ->Sumw2();
	    h_TrackPt[leg][ch] ->Sumw2();
	    h_PtError[leg][ch] ->Sumw2();
	    h_TrackEta[leg][ch]->Sumw2();
	    h_TrackPhi[leg][ch]->Sumw2();
	    
	    h_PixelHits[leg][ch]              ->Sumw2();
	    h_TkHits[leg][ch]                 ->Sumw2();
	    h_MuonStationHits[leg][ch]        ->Sumw2();
	    h_ValidHits[leg][ch]              ->Sumw2();
	    h_MatchedMuonStations[leg][ch]    ->Sumw2();
	    h_TkLayersWithMeasurement[leg][ch]->Sumw2();
	    
	    for (int i = 0; i < nBiasBins; ++i) {
	      std::stringstream name;
	      double biasValue = (maxBias/nBiasBins)*(i+1);
	      name << std::setw(3) << std::setfill('0') << i + 1;

	      // inject positive bias
	      TString histname  = TString(charge[ch]+"_"+legs[leg]+"_CurvePlusBias_"+name.str());
	      name.str("");
	      name.clear();
	      name << biasValue;
	      TString histtitle("#frac{q}{p_{T}}+#Delta#kappa("+name.str()+")");
	      if (debug > 3)
		std::cout << "creating histogram " << histname << " " << histtitle << std::endl;
	      h_CurvePlusBias[leg][ch][i] = std::shared_ptr<TH1D>(new TH1D(histname, histtitle, 2*500, -0.1, 0.1));
	      h_CurvePlusBias[leg][ch][i]->Sumw2();

	      // inject negative bias
	      name.str("");
	      name.clear();
	      name << std::setw(3) << std::setfill('0') << i + 1;
	      TString histname2  = TString(charge[ch]+"_"+legs[leg]+"_CurveMinusBias_"+name.str());
	      name.str("");
	      name.clear();
	      name << biasValue;
	      TString histtitle2 = TString("#frac{q}{p_{T}}-#Delta#kappa("+name.str()+")");
	      if (debug > 3)
		std::cout << "trying to create problematic histogram " << histname2 << " " << histtitle2 << std::endl;
	      h_CurveMinusBias[leg][ch][i] = std::shared_ptr<TH1D>(new TH1D(histname2, histtitle2, 2*500, -0.1, 0.1));
	      h_CurveMinusBias[leg][ch][i]->Sumw2();
	    }
	  }// end loop on different charge histograms
	}// end loop on different muon leg histograms
	std::cout << "Ending the HistogramMaker constructor" << std::endl;
      } // end constructorr


      HistogramMaker::~HistogramMaker()
      {
	std::cout << "HistogramMaker destructor called" << std::endl;
	m_outFile->Write();
	m_outFile->Close();
	std::cout << "Wrote and closed output file 0x" << std::hex << m_outFile.get() << std::dec << std::endl;
	/* use shared_ptr to avoid all this
	for (int leg = 0; leg < 3; ++ leg) {
	  for (int ch = 0; ch < 3; ++ ch) {
	    delete h_Chi2[leg][ch];
	    delete h_Ndof[leg][ch];
	    delete h_Chi2Ndof[leg][ch];
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
	    h_Chi2Ndof[leg][ch]  = NULL;
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

	delete m_outFile;
	m_outFile = NULL;

	delete m_treeReader;
	m_treeReader = NULL;

	delete m_tree;
	m_tree = NULL;

	delete m_treeChain;
	m_treeChain = NULL;
	*/
	std::cout << "HistogramMaker destructor finished" << std::endl;
      } // end destructor


      int HistogramMaker::runLoop(int debug)
      {
	return Plot(m_tree.get());
      }
      
      
      void HistogramMaker::parseConfiguration(std::string const& confFileName)
      {
	std::ifstream infile(confFileName.c_str());
	if (!infile.is_open()) {
	  std::cout << "Problem opening file " << confFileName << std::endl;
	  return; // maybe should exit(1) here?
	}
 
	std::string line;
	while (std::getline(infile,line)) {
	  if (line.find("#") == 0 || line.find("!") == 0)
	    continue; // found comment, skip processing
	  
	  // looking for NBiasBins, MaxKBias, MinPtCut, Arbitration, TrackAlgo, MuonLeg, MuCuts
	  std::string key;
	  std::getline(infile,key,'=');
	  
	  std::string value;
	  std::getline(infile,value);
	  std::istringstream valstream(value);

	  if (key.find("NBiasBins") != std::string::npos) {
	    valstream >> confParams.NBiasBins;
	  } else if (key.find("MaxKBias") != std::string::npos) {
	    valstream >> confParams.MaxKBias;
	  } else if (key.find("MinPtCut") != std::string::npos) {
	    valstream >> confParams.MinPtCut;
	  } else if (key.find("Arbitration") != std::string::npos) {
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    confParams.Arbitration = value;
	  } else if (key.find("TrackAlgo") != std::string::npos) {
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    confParams.TrackAlgo = value;
	  } else if (key.find("MuonLeg") != std::string::npos) {
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    confParams.MuonLeg = value;
	  } else if (key.find("PathPrefix") != std::string::npos) {
	    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	    confParams.PathPrefix = value;
	  }
	  
	}
	return;
	std::stringstream treeName;
	treeName << "analysis" << confParams.TrackAlgo << "Muons/MuonTree";
	m_treeChain  = std::shared_ptr<TChain>(new TChain(TString(treeName.str())));
      }
      
      void HistogramMaker::parseFileList(std::string const& fileList)
      {
	std::ifstream infile(fileList.c_str());
	if (!infile.is_open()) {
	  std::cout << "Problem opening file " << fileList << std::endl;
	  return; // maybe should exit(1) here?
	}
 
	std::string line;
	while (std::getline(infile,line)) {
	  if (line.find("#") == 0 || line.find("!") == 0)
	    continue; // found comment, skip processing, only at very beginning of line
	  if (line.find(".root") == std::string::npos)
	    continue; // found line without .root file name, skip
	  std::stringstream filepath;

	  filepath << confParams.PathPrefix << line; // need to strip off the newline?
	  m_treeChain->Add(TString(filepath.str()));
	}// end while loop over lines in file
	m_tree = m_treeChain;
	return;
      }
      

      int HistogramMaker::Plot(TTree* intree)
      {
	maxBias     = confParams.MaxKBias;
	nBiasBins   = confParams.NBiasBins;

	m_treeReader = std::shared_ptr<TTreeReader>(new TTreeReader(intree));

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
	TTreeReaderValue<Int_t>    upTrackMatchedMuonStations(  *m_treeReader,"upperMuon_numberOfMatchedStations");
	TTreeReaderValue<Int_t>    upTrackTkLayersWithMeasurement(*m_treeReader,"upperMuon_trackerLayersWithMeasurement");

	//TTreeReaderValue<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > upTrackMuonP4(*m_treeReader,"upperMuon_P4");
	//TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > upTrackTrack(*m_treeReader,"upperMuon_trackVec");
	TTreeReaderValue<math::XYZTLorentzVector> upMuonP4(  *m_treeReader,"upperMuon_P4");
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
	
	//TTreeReaderValue<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > lowTrackMuonP4(*m_treeReader,"lowerMuon_P4");
	//TTreeReaderValue<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> > lowTrackTrackVec(*m_treeReader,"lowerMuon_trackVec");
	TTreeReaderValue<math::XYZTLorentzVector> lowMuonP4(  *m_treeReader,"lowerMuon_P4");
	TTreeReaderValue<math::XYZVector>         lowTrackVec(*m_treeReader,"lowerMuon_trackVec");
  
	int j = 0;
	while (m_treeReader->Next()){
	  if(*upTrackChi2 > -1000) { //ensure values are from an actual event
	    int combLow[2] = {1,2};
	    int combUp[2] = {0,2};
	    for (int fill = 0; fill < 2; ++fill) {
	      int charge = *upTrackCharge;
	      double upperTrackPt = sqrt(upTrackVec->perp2());
	      double upperTrackEta = upTrackVec->eta();
	      double upperTrackPhi = upTrackVec->phi();
	      double upperCpT = (*upTrackCharge)/(upperTrackPt);
	      h_Chi2[combUp[fill]][     (charge<0)?0:1]->Fill(*upTrackChi2);
	      h_Ndof[combUp[fill]][     (charge<0)?0:1]->Fill(*upTrackNdof);
	      h_Chi2Ndof[combUp[fill]][ (charge<0)?0:1]->Fill((*upTrackChi2)/(*upTrackNdof));
	      h_Pt[combUp[fill]][       (charge<0)?0:1]->Fill(*upTrackPt);
	      h_Charge[combUp[fill]][   (charge<0)?0:1]->Fill(*upTrackCharge);
	      h_Curve[combUp[fill]][    (charge<0)?0:1]->Fill(upperCpT);
	      h_Dxy[combUp[fill]][      (charge<0)?0:1]->Fill(*upTrackDxy);
	      h_Dz[combUp[fill]][       (charge<0)?0:1]->Fill(*upTrackDz);
	      h_PtError[combUp[fill]][  (charge<0)?0:1]->Fill(*upTrackPtError);
	      h_DxyError[combUp[fill]][ (charge<0)?0:1]->Fill(*upTrackDxyError);
	      h_DzError[combUp[fill]][  (charge<0)?0:1]->Fill(*upTrackDzError);
	      h_TrackPt[combUp[fill]][  (charge<0)?0:1]->Fill(upperTrackPt);
	      h_TrackEta[combUp[fill]][ (charge<0)?0:1]->Fill(upperTrackEta);
	      h_TrackPhi[combUp[fill]][ (charge<0)?0:1]->Fill(upperTrackPhi);
	      
	      h_PixelHits[combUp[fill]][              (charge<0)?0:1]->Fill(*upTrackPhits);
	      h_TkHits[combUp[fill]][                 (charge<0)?0:1]->Fill(*upTrackThits);
	      h_MuonStationHits[combUp[fill]][        (charge<0)?0:1]->Fill(*upTrackMhits);
	      h_ValidHits[combUp[fill]][              (charge<0)?0:1]->Fill(*upTrackValidHits);
	      h_MatchedMuonStations[combUp[fill]][    (charge<0)?0:1]->Fill(*upTrackMatchedMuonStations);
	      h_TkLayersWithMeasurement[combUp[fill]][(charge<0)?0:1]->Fill(*upTrackTkLayersWithMeasurement);
	      
	      for (int i = 0; i < nBiasBins; ++i) {
		h_CurvePlusBias[combUp[fill]][ (charge<0)?0:1][i]->Fill(upperCpT+(i+1)*(maxBias/nBiasBins));
		h_CurveMinusBias[combUp[fill]][(charge<0)?0:1][i]->Fill(upperCpT-(i+1)*(maxBias/nBiasBins));
	      }
	      
	      charge = *lowTrackCharge;
	      double lowerTrackPt = sqrt(lowTrackVec->perp2());
	      double lowerTrackEta = lowTrackVec->eta();
	      double lowerTrackPhi = lowTrackVec->phi();
	      double lowerCpT = (*lowTrackCharge)/(lowerTrackPt);
	      h_Chi2[combLow[fill]][    (charge<0)?0:1]->Fill(*lowTrackChi2);
	      h_Ndof[combLow[fill]][    (charge<0)?0:1]->Fill(*lowTrackNdof);
	      h_Chi2Ndof[combLow[fill]][(charge<0)?0:1]->Fill((*lowTrackChi2)/(*lowTrackNdof));
	      h_Pt[combLow[fill]][      (charge<0)?0:1]->Fill(*lowTrackPt);
	      h_Charge[combLow[fill]][  (charge<0)?0:1]->Fill(*lowTrackCharge);
	      h_Curve[combLow[fill]][   (charge<0)?0:1]->Fill(lowerCpT);
	      h_Dxy[combLow[fill]][     (charge<0)?0:1]->Fill(*lowTrackDxy);
	      h_Dz[combLow[fill]][      (charge<0)?0:1]->Fill(*lowTrackDz);
	      h_PtError[combLow[fill]][ (charge<0)?0:1]->Fill(*lowTrackPtError);
	      h_DxyError[combLow[fill]][(charge<0)?0:1]->Fill(*lowTrackDxyError);
	      h_DzError[combLow[fill]][ (charge<0)?0:1]->Fill(*lowTrackDzError);
	      h_TrackPt[combLow[fill]][ (charge<0)?0:1]->Fill(lowerTrackPt);
	      h_TrackEta[combLow[fill]][(charge<0)?0:1]->Fill(lowerTrackEta);
	      h_TrackPhi[combLow[fill]][(charge<0)?0:1]->Fill(lowerTrackPhi);
	      
	      h_PixelHits[combLow[fill]][          (charge<0)?0:1]->Fill(*lowTrackPhits);
	      h_TkHits[combLow[fill]][             (charge<0)?0:1]->Fill(*lowTrackThits);
	      h_MuonStationHits[combLow[fill]][    (charge<0)?0:1]->Fill(*lowTrackMhits);
	      h_ValidHits[combLow[fill]][          (charge<0)?0:1]->Fill(*lowTrackValidHits);
	      h_MatchedMuonStations[combLow[fill]][(charge<0)?0:1]->Fill(*lowTrackMatchedMuonStations);
	      h_TkLayersWithMeasurement[combLow[fill]][(charge<0)?0:1]->Fill(*lowTrackTkLayersWithMeasurement);
	      
	      for (int i = 0; i < nBiasBins; ++i) {
		h_CurvePlusBias[combLow[fill]][ (charge<0)?0:1][i]->Fill(lowerCpT+(i+1)*(maxBias/nBiasBins));
		h_CurveMinusBias[combLow[fill]][(charge<0)?0:1][i]->Fill(lowerCpT-(i+1)*(maxBias/nBiasBins));
	      }
	      ++j;
	    }// closing if fill
	  }// closing for loop over combining plots
	} // end while loop

	return j;
      }

    } // end namespace wsu::dileptons::cosmics
  } // end namespace wsu::dileptons
} // end namespace wsu