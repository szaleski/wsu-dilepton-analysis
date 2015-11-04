#include "WSUDiLeptons/MuonAnalyzer/interface/MuonAnalyzer.h"
// -*- C++ -*-
//
// Package:    CosmicStudies/MuonAnalyzer
// Class:      MuonAnalyzer
// 
/**\class MuonAnalyzer MuonAnalyzer.cc CosmicStudies/MuonAnalyzer/plugins/MuonAnalyzer.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Jared Sturdy
//         Created:  Wed, 25 Feb 2015 12:55:49 GMT
//
//




MuonAnalyzer::MuonAnalyzer(const edm::ParameterSet& pset)
{
  muonSrc_ = pset.getParameter<edm::InputTag>("muonSrc");
  debug_ = pset.getParameter<int>("debug");
  algoType_ = pset.getParameter<int>("algoType");
  
  //now do what ever initialization is needed
 
}




/*inline double LorentzVector::DeltaTheta(const LorentzVector & muon1, const LorentzVector & muon2 ) const{
  return TVector2::Theta_mpi_pi(muon1.theta()- muon2.theta())
  }*/




MuonAnalyzer::~MuonAnalyzer()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void MuonAnalyzer::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  using namespace ROOT::Math;
  //edm::Handle<edm::View<reco::Muon> > muonColl;
  edm::Handle<reco::MuonCollection > muonColl;
  ev.getByLabel(muonSrc_, muonColl);

  event = (ev.id()).event();  
  run   = (ev.id()).run();
  lumi  = ev.luminosityBlock();
  
  type = reco::Muon::SegmentAndTrackArbitration;
  
  if ( muonColl->size() != 2) return;

   upperMuon_P4.SetXYZT(0,0,0,-1);
  lowerMuon_P4.SetXYZT( 0,0,0,-1);

  upperMuon_chi2 = -1;
  lowerMuon_chi2 = -1;

  upperMuon_ndof = -1;
  lowerMuon_ndof = -1;

  upperMuon_charge = -1000;
  lowerMuon_charge = -1000;

  upperMuon_dxy = -1000;
  lowerMuon_dxy = -1000;

  upperMuon_dz = -1000;
  lowerMuon_dz = -1000;

  upperMuon_pixelHits = -1;
  lowerMuon_pixelHits = -1;

  upperMuon_trackerHits = -1;
  lowerMuon_trackerHits = -1;

  upperMuon_muonStationHits = -1;
  lowerMuon_muonStationHits = -1;

  upperMuon_numberOfValidHits = -1;
  lowerMuon_numberOfValidHits = -1;

  upperMuon_numberOfValidMuonHits = -1;
  lowerMuon_numberOfValidMuonHits = -1;

  upperMuon_numberOfMatchedStations = -1;
  lowerMuon_numberOfMatchedStations = -1;

  upperMuon_trackerLayersWithMeasurement = -1;
  lowerMuon_trackerLayersWithMeasurement = -1;

  upperMuon_ptError = -1;
  lowerMuon_ptError = -1;

  upperMuon_dxyError = -1000;
  lowerMuon_dxyError = -1000;

  upperMuon_dzError = -1000;
  lowerMuon_dzError = -1000;

  upperMuon_pT = -1;
  lowerMuon_pT = -1;
 
  upperMuon_trackPt = -1;
  lowerMuon_trackPt = -1;

  upperMuon_trackVec.SetXYZ(0,0,0);
  lowerMuon_trackVec.SetXYZ(0,0,0);

  upperMuon_trackerLayersWithMeasurement = -1000;
  lowerMuon_trackerLayersWithMeasurement = -1000;
  
  upperMuon_numberOfValidMuonHits = -1000;
  lowerMuon_numberOfValidMuonHits = -1000;
  
  
  //  reco::TrackHitPattern *upperMuon_numberOfValidHits, *lowerMuon_numberOfValidHits;
  int muIdx = 0;
  for (reco::MuonCollection::const_iterator muon=muonColl->begin(), muonCollEnd=muonColl->end();muon!=muonCollEnd; ++muon) {
    if(debug_ > 3){
      std::cout << "globalMuon pt [GeV]: " << muon->pt() << 
	"  eta : " << muon->eta() << 
	"  phi : " << muon->phi() << 
	std::endl;
    }
    
    
    if (debug_ > 3) {
      if (muon->combinedMuon().isNonnull()) {
	reco::TrackRef ref = muon->combinedMuon();
	std::cout << "combinedMuon number of hits: " << ref->numberOfValidHits() << std::endl;
      }
    }
    
    if ( debug_ > 3) {
      if (muon->standAloneMuon().isNonnull()) {
	reco::TrackRef ref = muon->standAloneMuon();
	if (ref->innerOk())
	  std::cout << "standAloneMuon inner x [cm]: " << ref->innerPosition().X()
		    << "  outer x [cm]: "              << ref->outerPosition().X()
		    << "  inner y [cm]: "              << ref->innerPosition().Y()
		    << "  outer y [cm]: "              << ref->outerPosition().Y()
		    << std::endl;
      }
    }
      
    if(debug_ > 1) std::cout << "\n\ncreating Track ref variable\n";
    
    reco::TrackRef ref;
    
    if(debug_ > 1) std::cout << "\n variable createing. Calling GetTrackType\n";
    ref = GetTrackType(algoType_, muon);
    
    if(debug_ > 1) std::cout << "\nFound Track Type. If NonNull Fill.\n";
    
    if(ref.isNonnull()) TrackFill(ref, muon, type);
    
    if(debug_ > 1) std::cout << "\nFilled Histograms!\n" ;
    
    if(debug_ > 3){
      if (muon->track().isNonnull()) {
	reco::TrackRef ref = muon->track();
	if (ref->outerOk())
	  std::cout << "track outer pt [GeV]: " << ref->outerPt();
	if (ref->innerOk())
	  std::cout << "  inner pt [GeV]: "     << ref->innerMomentum().Rho();
	std::cout << std::endl;
      }
    }
    ++muIdx;
  }
  cosmicTree->Fill();
}


void MuonAnalyzer::TrackFill(reco::TrackRef ref, reco::MuonCollection::const_iterator muon, reco::Muon::ArbitrationType const& arbType){

  if(debug_ > 1)std::cout << "\nStarting to Fill Histograms!\n";

  if (ref->outerPosition().Y() > 0){
    //	upperMuon =&(*muon);

    upperMuon_P4       = muon->p4();
    upperMuon_pT       = muon->pt();
    upperMuon_chi2     = ref->chi2();
    upperMuon_ndof     = ref->ndof();
    upperMuon_charge   = ref->charge();
    upperMuon_dxy      = ref->dxy();
    upperMuon_dz       = ref->dz();
    upperMuon_ptError  = ref->ptError();
    upperMuon_dxyError = ref->dxyError();
    upperMuon_dzError  = ref->dzError();
    upperMuon_trackPt  = ref->pt();
    upperMuon_trackVec = ref->momentum();

    upperMuon_pixelHits                    = ref->hitPattern().numberOfValidPixelHits();
    upperMuon_trackerHits                  = ref->hitPattern().numberOfValidTrackerHits();
    upperMuon_muonStationHits              = ref->hitPattern().muonStationsWithValidHits();
    upperMuon_numberOfValidHits            = ref->hitPattern().numberOfValidHits();
    upperMuon_numberOfValidMuonHits        = ref->hitPattern().numberOfValidMuonHits();
    upperMuon_numberOfMatchedStations      = muon->numberOfMatchedStations(arbType);
    upperMuon_trackerLayersWithMeasurement = ref->hitPattern().trackerLayersWithMeasurement();

    if( debug_ > 3){
      double relError = upperMuon_ptError/upperMuon_trackPt;
      std::cout << "Muon pT Error/pT is: " << relError       << std::endl
		<< "Muon pT Error is: "    << ref->ptError() << std::endl
		<< "Muon pT is: "          << ref->pt()      << std::endl;
      std::cout << "Number of Pixel hits are: "           << ref->hitPattern().numberOfValidPixelHits()    << std::endl
		<< "Number of Valid Tracker Hits are : "  << ref->hitPattern().numberOfValidTrackerHits()  << std::endl
		<< "Number of Valid Muon Hits are: "      << ref->hitPattern().muonStationsWithValidHits() << std::endl
		<< "Number of matched muon stations is: " << muon->numberOfMatchedStations(arbType)        << std::endl;
    } 
  }
  
  else if (ref->outerPosition().Y() < 0){
    lowerMuon_P4       = muon->p4();
    lowerMuon_pT       = muon->pt();
    lowerMuon_chi2     = ref->chi2();
    lowerMuon_ndof     = ref->ndof();
    lowerMuon_charge   = ref->charge();
    lowerMuon_dxy      = ref->dxy();
    lowerMuon_dz       = ref->dz();
    lowerMuon_ptError  = ref->ptError();
    lowerMuon_dxyError = ref->dxyError();
    lowerMuon_dzError  = ref->dzError();
    lowerMuon_trackPt  = ref->pt();
    lowerMuon_trackVec = ref->momentum();
    
    lowerMuon_pixelHits                    = ref->hitPattern().numberOfValidPixelHits();
    lowerMuon_trackerHits                  = ref->hitPattern().numberOfValidTrackerHits();
    lowerMuon_muonStationHits              = ref->hitPattern().muonStationsWithValidHits();
    lowerMuon_numberOfValidHits            = ref->hitPattern().numberOfValidHits();
    lowerMuon_numberOfValidMuonHits        = ref->hitPattern().numberOfValidMuonHits();
    lowerMuon_numberOfMatchedStations      = muon->numberOfMatchedStations(arbType);
    lowerMuon_trackerLayersWithMeasurement = ref->hitPattern().trackerLayersWithMeasurement();
    
    if( debug_ > 3){
      double relError = lowerMuon_ptError/lowerMuon_trackPt;
      std::cout << "Muon pT Error/pT is: " << relError       << std::endl
		<< "Muon pT Error is: "    << ref->ptError() << std::endl
		<< "Muon pT is: "          << ref->pt()      << std::endl;
      std::cout << "Number of Pixel hits are: "           << ref->hitPattern().numberOfValidPixelHits()    << std::endl
		<< "Number of Valid Tracker Hits are : "  << ref->hitPattern().numberOfValidTrackerHits()  << std::endl
		<< "Number of Valid Muon Hits are: "      << ref->hitPattern().muonStationsWithValidHits() << std::endl
		<< "Number of matched muon stations is: " << muon->numberOfMatchedStations(arbType)        << std::endl;
    } 
  }
  if(debug_ > 1)
    std::cout << "\nHistograms Filled!" << std::endl;
}


reco::TrackRef MuonAnalyzer::GetTrackType(int algoType,reco::MuonCollection::const_iterator muon){
  
  if(debug_ > 1)
    std::cout << "Started Finding Track Type!" << std::endl;

  reco::TrackRef ref;
  
  if (algoType == 1) ref = muon->track();
  else if (algoType == 2) ref = muon->tpfmsTrack();
  else if (algoType == 3) ref = muon->dytTrack();
  else if (algoType == 4) ref = muon->pickyTrack();
  else if (algoType == 5) ref = muon->tunePMuonBestTrack();
  else ref = muon->track();
  
  if(debug_ > 1)
    std::cout << "Returning track type" << std::endl;

  return ref;
}



// ------------ method called once each job just before starting event loop  ------------
void MuonAnalyzer::beginJob()
{
  edm::Service< TFileService > fs;
  
  cosmicTree = fs->make<TTree>( "MuonTree", "TTree variables" );

  cosmicTree->Branch("muonEventNumber", &event, 10000, 1);
  cosmicTree->Branch("muonRunNumber",   &run,   10000, 1);
  cosmicTree->Branch("muonLumiBlock",   &lumi,  10000, 1);

  /////////Muon in upper half of CMS
  cosmicTree->Branch("upperMuon_pT", &upperMuon_pT, 10000, 1);
  cosmicTree->Branch("upperMuon_P4", &upperMuon_P4, 10000, 1);

  cosmicTree->Branch("upperMuon_trackPt",  &upperMuon_trackPt,  10000, 1);
  cosmicTree->Branch("upperMuon_trackVec", &upperMuon_trackVec, 10000, 1);
  cosmicTree->Branch("upperMuon_chi2",     &upperMuon_chi2,     10000, 1);
  cosmicTree->Branch("upperMuon_ndof",     &upperMuon_ndof,     10000, 1);
  cosmicTree->Branch("upperMuon_charge",   &upperMuon_charge,   10000, 1);
  cosmicTree->Branch("upperMuon_dxy",      &upperMuon_dxy,      10000, 1);
  cosmicTree->Branch("upperMuon_dz",       &upperMuon_dz,       10000, 1);
  cosmicTree->Branch("upperMuon_ptError",  &upperMuon_ptError,  10000, 1);
  cosmicTree->Branch("upperMuon_dxyError", &upperMuon_dxyError, 10000, 1);
  cosmicTree->Branch("upperMuon_dzError",  &upperMuon_dzError,  10000, 1);

  cosmicTree->Branch("upperMuon_pixelHits",                    &upperMuon_pixelHits,                    10000, 1);
  cosmicTree->Branch("upperMuon_trackerHits",                  &upperMuon_trackerHits,                  10000, 1);
  cosmicTree->Branch("upperMuon_muonStationHits",              &upperMuon_muonStationHits,              10000, 1);
  cosmicTree->Branch("upperMuon_numberOfValidHits",            &upperMuon_numberOfValidHits,            10000, 1);
  cosmicTree->Branch("upperMuon_numberOfValidMuonHits",        &upperMuon_numberOfValidMuonHits,        10000, 1);
  cosmicTree->Branch("upperMuon_numberOfMatchedStations",      &upperMuon_numberOfMatchedStations,      10000, 1);
  cosmicTree->Branch("upperMuon_trackerLayersWithMeasurement", &upperMuon_trackerLayersWithMeasurement, 10000, 1);
  
  /////////Muon in lower half of CMS
  cosmicTree->Branch("lowerMuon_pT", &lowerMuon_pT, 10000, 1);
  cosmicTree->Branch("lowerMuon_P4", &lowerMuon_P4, 10000, 1);

  cosmicTree->Branch("lowerMuon_trackPt",  &lowerMuon_trackPt,  10000, 1);
  cosmicTree->Branch("lowerMuon_trackVec", &lowerMuon_trackVec, 10000, 1);
  cosmicTree->Branch("lowerMuon_chi2",     &lowerMuon_chi2,     10000, 1);
  cosmicTree->Branch("lowerMuon_ndof",     &lowerMuon_ndof,     10000, 1);
  cosmicTree->Branch("lowerMuon_charge",   &lowerMuon_charge,   10000, 1);
  cosmicTree->Branch("lowerMuon_dxy",      &lowerMuon_dxy,      10000, 1);
  cosmicTree->Branch("lowerMuon_dz",       &lowerMuon_dz,       10000, 1);
  cosmicTree->Branch("lowerMuon_ptError",  &lowerMuon_ptError,  10000, 1);
  cosmicTree->Branch("lowerMuon_dxyError", &lowerMuon_dxyError, 10000, 1);
  cosmicTree->Branch("lowerMuon_dzError",  &lowerMuon_dzError,  10000, 1);

  cosmicTree->Branch("lowerMuon_pixelHits",                    &lowerMuon_pixelHits,                    10000, 1);
  cosmicTree->Branch("lowerMuon_trackerHits",                  &lowerMuon_trackerHits,                  10000, 1);
  cosmicTree->Branch("lowerMuon_muonStationHits",              &lowerMuon_muonStationHits,              10000, 1);
  cosmicTree->Branch("lowerMuon_numberOfValidHits",            &lowerMuon_numberOfValidHits,            10000, 1);
  cosmicTree->Branch("lowerMuon_numberOfValidMuonHits",        &lowerMuon_numberOfValidMuonHits,        10000, 1);
  cosmicTree->Branch("lowerMuon_numberOfMatchedStations",      &lowerMuon_numberOfMatchedStations,      10000, 1);
  cosmicTree->Branch("lowerMuon_trackerLayersWithMeasurement", &lowerMuon_trackerLayersWithMeasurement, 10000, 1);
}



// ------------ method called once each job just after ending the event loop  ------------
void MuonAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
  void MuonAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a run  ------------
/*
  void MuonAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
  void MuonAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
  void MuonAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void MuonAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(MuonAnalyzer);
