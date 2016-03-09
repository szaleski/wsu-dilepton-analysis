#include "WSUDiLeptons/MuonAnalyzer/interface/MuonTree.h"
//#include "WSUDiLeptons/MuonAnalyzer/interface/MuonHelpers.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"

// -*- C++ -*-
//
// Package:    WSUDiLeptons/MuonAnalyzer
// Class:      MuonTree
// 
/**\class MuonTree MuonTree.cc WSUDiLeptons/MuonAnalyzer/plugins/MuonTree.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Jared Sturdy
//         Created:  Wed, 25 Feb 2015 12:55:49 GMT
//
//




MuonTree::MuonTree(const edm::ParameterSet& pset)
{
  muonSrc_     = pset.getParameter<edm::InputTag>("muonSrc");
  upperLegSrc_ = pset.getParameter<edm::InputTag>("upperLegSrc");
  lowerLegSrc_ = pset.getParameter<edm::InputTag>("lowerLegSrc");

  globalTrackSrc_  = pset.getParameter<edm::InputTag>("globalTrackSrc");
  cosmicTrackSrc_  = pset.getParameter<edm::InputTag>("cosmicTrackSrc");
  trackerTrackSrc_ = pset.getParameter<edm::InputTag>("trackerTrackSrc");

  debug_       = pset.getParameter<int>("debug");
  algoType_    = pset.getParameter<int>("algoType");

  maxDEta_ = pset.getParameter<double>("maxDEta");
  maxDPhi_ = pset.getParameter<double>("maxDPhi");
  maxDR_   = pset.getParameter<double>("maxDR");
  minPt_   = pset.getParameter<double>("minPt");

  muonToken_     = consumes<reco::MuonCollection>(muonSrc_);
  upperLegToken_ = consumes<reco::MuonCollection>(upperLegSrc_);
  lowerLegToken_ = consumes<reco::MuonCollection>(lowerLegSrc_);

  globalTrackToken_  = consumes<reco::TrackCollection>(globalTrackSrc_);
  cosmicTrackToken_  = consumes<reco::TrackCollection>(cosmicTrackSrc_);
  trackerTrackToken_ = consumes<reco::TrackCollection>(trackerTrackSrc_);
  //now do what ever initialization is needed
}


MuonTree::~MuonTree()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void MuonTree::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  using namespace ROOT::Math;
  edm::Handle<reco::MuonCollection > muonColl;
  edm::Handle<reco::MuonCollection > upperLegColl;
  edm::Handle<reco::MuonCollection > lowerLegColl;
  ev.getByToken(muonToken_,     muonColl);
  ev.getByToken(upperLegToken_, upperLegColl);
  ev.getByToken(lowerLegToken_, lowerLegColl);
  
  edm::Handle<reco::TrackCollection> globalTrackColl;
  edm::Handle<reco::TrackCollection> cosmicTrackColl;
  edm::Handle<reco::TrackCollection> trackerTrackColl;
  ev.getByToken(globalTrackToken_,  globalTrackColl);
  ev.getByToken(cosmicTrackToken_,  cosmicTrackColl);
  ev.getByToken(trackerTrackToken_, trackerTrackColl);
  
  edm::Handle<reco::TrackCollection> tracks[3] = {globalTrackColl, cosmicTrackColl, trackerTrackColl};
  
  event = (ev.id()).event();  
  run   = (ev.id()).run();
  lumi  = ev.luminosityBlock();
  
    
  //type = reco::Muon::SegmentAndTrackArbitration;
    
  nMuons     = muonColl->size();
  nUpperLegs = upperLegColl->size();
  nLowerLegs = lowerLegColl->size();

  nGlobalTracks  = globalTrackColl->size();
  nCosmicTracks  = cosmicTrackColl->size();
  nTrackerTracks = trackerTrackColl->size();
    
  for (int idx = 0; idx < 10; ++idx) {
    muon_isGlobal    [idx] = -1;
    muon_isTracker   [idx] = -1;
    muon_isStandAlone[idx] = -1;
    muon_isLower[idx]      = -1;
    muon_isUpper[idx]      = -1;
      
    muonP4[idx].SetXYZT(0,0,0,-1);
    muon_pT[idx]  = -1;
    muon_Eta[idx] = -10;
    muon_Phi[idx] = -10;
      
    muon_innerY[idx] = 0; // what's a good nonsense value for this?
    muon_outerY[idx] = 0; // what's a good nonsense value for this?
    muon_tpin[idx]   = 0; // what's a good nonsense value for this?
    muon_tpout[idx]  = 0; // what's a good nonsense value for this?
      
    muon_trackVec[idx].SetXYZ(0,0,0);
    muon_trackPt[idx]  = -1;
    muon_trackEta[idx] = -10;
    muon_trackPhi[idx] = -10;
    muon_ptError[idx]  = -1;
      
    muon_charge[idx] = -10;
    muon_chi2  [idx] = -1;
    muon_ndof  [idx] = -1;
      
    muon_dxy     [idx] = -1000;
    muon_dz      [idx] = -1000;
    muon_dxyError[idx] = -1;
    muon_dzError [idx] = -1;
      
    muon_pixHits    [idx] = -1;
    muon_tkHits     [idx] = -1;
    muon_muonStaHits[idx] = -1;
    muon_nVHits     [idx] = -1;
    muon_nVMuHits   [idx] = -1;
    muon_nMatSta    [idx] = -1;
    muon_tkLayWMeas [idx] = -1;

    for (int tk = 0; tk < 3; ++tk) {
      track_innerY[tk][idx] = 0; // what's a good nonsense value for this?
      track_outerY[tk][idx] = 0; // what's a good nonsense value for this?
      track_tpin[tk][idx]   = 0; // what's a good nonsense value for this?
      track_tpout[tk][idx]  = 0; // what's a good nonsense value for this?
      
      track_trackPt[tk][idx]  = -1;
      track_trackEta[tk][idx] = -10;
      track_trackPhi[tk][idx] = -10;
      track_ptError[tk][idx]  = -1;
      
      track_charge[tk][idx] = -10;
      track_chi2  [tk][idx] = -1;
      track_ndof  [tk][idx] = -1;

      track_isLower  [tk][idx] = -1;
      track_isUpper  [tk][idx] = -1;
      
      track_dxy     [tk][idx] = -1000;
      track_dz      [tk][idx] = -1000;
      track_dxyError[tk][idx] = -1;
      track_dzError [tk][idx] = -1;

      track_matchedMuIdx[tk][idx] = -1;
      
      track_pixHits    [tk][idx] = -1;
      track_tkHits     [tk][idx] = -1;
      track_muonStaHits[tk][idx] = -1;
      track_nVHits     [tk][idx] = -1;
      track_nVMuHits   [tk][idx] = -1;
      track_nMatSta    [tk][idx] = -1;
      track_tkLayWMeas [tk][idx] = -1;
    }
  }
  
  // skip processing empty collection
  if ( muonColl->size() > 0) {
    //return;
    if (debug_ > -1) {
      std::cout << "run/lumi/event " << run  << "/" << lumi << "/" << event << std::endl;
      std::cout << std::endl
		<< "found "     << nMuons     << " muons "      << std::endl
		<< "found "     << nUpperLegs << " upper legs " << std::endl
		<< "found "     << nLowerLegs << " lower legs " << std::endl
		<< "trackAlgo " << algoType_ << std::endl;
      std::cout << " muons: " << std::endl;
      for (auto muon = muonColl->begin(); muon != muonColl->end(); ++muon)
	std::cout << std::setw(5) << *muon 
		  << " (" << muon->isTrackerMuon()    << "t"
		  << "/"  << muon->isGlobalMuon()     << "g"
		  << "/"  << muon->isStandAloneMuon() << "sa"
		  << ") " << "y:"
		  << std::setw(8) << muon->tunePMuonBestTrack()->innerPosition().Y()
		  << "/"
		  << std::setw(8) << muon->tunePMuonBestTrack()->outerPosition().Y()
		  << " chi2:"  << std::setw(8) << muon->tunePMuonBestTrack()->chi2()
		  << " dxy:"   << std::setw(8) << muon->tunePMuonBestTrack()->dxy()
		  << " dz:"    << std::setw(8) << muon->tunePMuonBestTrack()->dz()
		  << " tpin:"  << std::setw(8) << muon->time().timeAtIpInOut
		  << " tpout:" << std::setw(8) << muon->time().timeAtIpOutIn
		  << std::endl;
      std::cout << " upper legs: " << std::endl;
      for (auto muon = upperLegColl->begin(); muon != upperLegColl->end(); ++muon)
	std::cout << std::setw(5) << *muon 
		  << " (" << muon->isTrackerMuon()    << "t"
		  << "/"  << muon->isGlobalMuon()     << "g"
		  << "/"  << muon->isStandAloneMuon() << "sa"
		  << ") " << muon->tunePMuonBestTrack()->innerPosition().Y()
		  << std::endl;
      std::cout << " lower legs: " << std::endl;
      for (auto muon = lowerLegColl->begin(); muon != lowerLegColl->end(); ++muon)
	std::cout << std::setw(5) << *muon
		  << " (" << muon->isTrackerMuon()    << "t"
		  << "/"  << muon->isGlobalMuon()     << "g"
		  << "/"  << muon->isStandAloneMuon() << "sa"
		  << ") " << muon->tunePMuonBestTrack()->innerPosition().Y()
		  << std::endl;
    }
    std::cout.flush();  
    
    int muIdx = 0;
    for (auto mu = muonColl->begin(); mu != muonColl->end(); ++ mu) {
      //reco::TrackRef ref = wsu::dileptons::MuonHelpers::GetTrackType(algoType_, &(*mu));
      reco::TrackRef ref = GetTrackType(algoType_, &(*mu));
      
      muon_tpin[muIdx]   = mu->time().timeAtIpInOut;
      muon_tpout[muIdx]  = mu->time().timeAtIpOutIn;
      
      muon_isTracker   [muIdx] = mu->isTrackerMuon();
      muon_isGlobal    [muIdx] = mu->isGlobalMuon();
      muon_isStandAlone[muIdx] = mu->isStandAloneMuon();

      muon_isLower[muIdx] = abs(mu->tunePMuonBestTrack()->innerPosition().Y()) < fabs(mu->tunePMuonBestTrack()->outerPosition().Y());
      muon_isUpper[muIdx] = abs(mu->tunePMuonBestTrack()->innerPosition().Y()) > fabs(mu->tunePMuonBestTrack()->outerPosition().Y());
      
      muonP4       [muIdx] = mu->p4();
      muon_pT      [muIdx] = mu->pt();
      muon_Eta     [muIdx] = mu->eta();
      muon_Phi     [muIdx] = mu->phi();
      if (ref.isNonnull()) { // can't dereference if the desired track ref is null
	// selections are done on "best track" in high pT ID, so take the track under study
	muon_chi2    [muIdx] = ref->chi2();
	muon_ndof    [muIdx] = ref->ndof();
	muon_charge  [muIdx] = ref->charge();
	muon_dxy     [muIdx] = ref->dxy();
	muon_dz      [muIdx] = ref->dz();
	muon_ptError [muIdx] = ref->ptError();
	muon_dxyError[muIdx] = ref->dxyError();
	muon_dzError [muIdx] = ref->dzError();
	muon_trackPt [muIdx] = ref->pt();
	muon_trackEta[muIdx] = ref->eta();
	muon_trackPhi[muIdx] = ref->phi();
	muon_trackVec[muIdx] = ref->momentum();
	muon_innerY  [muIdx] = ref->innerPosition().Y();
	muon_outerY  [muIdx] = ref->outerPosition().Y();
      }

      // take hit pattern from global track
      if (mu->globalTrack().isNonnull()) {
	muon_pixHits    [muIdx] = mu->globalTrack()->hitPattern().numberOfValidPixelHits();
	muon_tkHits     [muIdx] = mu->globalTrack()->hitPattern().numberOfValidTrackerHits();
	muon_tkLayWMeas [muIdx] = mu->globalTrack()->hitPattern().trackerLayersWithMeasurement();
	muon_muonStaHits[muIdx] = mu->globalTrack()->hitPattern().muonStationsWithValidHits();
	muon_nVHits     [muIdx] = mu->globalTrack()->hitPattern().numberOfValidHits();
	muon_nVMuHits   [muIdx] = mu->globalTrack()->hitPattern().numberOfValidMuonHits();
      } else {// otherwise take hit pattern from inner/outer track separately
	if (mu->innerTrack().isNonnull()) {
	  muon_pixHits    [muIdx] = mu->innerTrack()->hitPattern().numberOfValidPixelHits();
	  muon_tkHits     [muIdx] = mu->innerTrack()->hitPattern().numberOfValidTrackerHits();
	  muon_tkLayWMeas [muIdx] = mu->innerTrack()->hitPattern().trackerLayersWithMeasurement();
	}
	if (mu->outerTrack().isNonnull()) {
	  muon_muonStaHits[muIdx] = mu->outerTrack()->hitPattern().muonStationsWithValidHits();
	  muon_nVHits     [muIdx] = mu->outerTrack()->hitPattern().numberOfValidHits();
	  muon_nVMuHits   [muIdx] = mu->outerTrack()->hitPattern().numberOfValidMuonHits();
	}
      }
      muon_nMatSta[muIdx] = mu->numberOfMatchedStations(reco::Muon::SegmentAndTrackArbitration);
      
      ++muIdx;
    } // end loop over muons
  } // end check on muon collection size
  
  if (debug_ > -1) {
    if (globalTrackColl->size() > 0) {
      std::cout	<< "found "     << nGlobalTracks  << " global "  << std::endl;
      std::cout << " global tracks: " << std::endl;
      for (auto track = globalTrackColl->begin(); track != globalTrackColl->end(); ++track)
	std::cout << "y:"
		  << std::setw(8) << track->innerPosition().Y()
		  << "/"
		  << std::setw(8) << track->outerPosition().Y()
		  << " pt:"           << std::setw(8) << track->pt()
		  << " eta:"          << std::setw(8) << track->eta()
		  << " phi:"          << std::setw(8) << track->phi()
		  << " chi2:"         << std::setw(8) << track->chi2()
		  << " dxy:"          << std::setw(8) << track->dxy()
		  << " dz:"           << std::setw(8) << track->dz()
		  << " nPixHits:"     << std::setw(2) << track->hitPattern().numberOfValidPixelHits()
		  << " nVTkHits:"     << std::setw(2) << track->hitPattern().numberOfValidTrackerHits()
		  << " nTkLayWMeas:"  << std::setw(2) << track->hitPattern().trackerLayersWithMeasurement()
		  << " nMuStaWVHits:" << std::setw(2) << track->hitPattern().muonStationsWithValidHits()
		  << " nVHits:"       << std::setw(2) << track->hitPattern().numberOfValidHits()
		  << " nVMuHits:"     << std::setw(2) << track->hitPattern().numberOfValidMuonHits()
		  << std::endl;
    }

    if (cosmicTrackColl->size() > 0) {
      std::cout << "found "     << nCosmicTracks  << " cosmic "  << std::endl;
      std::cout << " cosmic tracks: " << std::endl;
      for (auto track = cosmicTrackColl->begin(); track != cosmicTrackColl->end(); ++track)
	std::cout << "y:"
		  << std::setw(8) << track->innerPosition().Y()
		  << "/"
		  << std::setw(8) << track->outerPosition().Y()
		  << " pt:"           << std::setw(8) << track->pt()
		  << " eta:"          << std::setw(8) << track->eta()
		  << " phi:"          << std::setw(8) << track->phi()
		  << " chi2:"         << std::setw(8) << track->chi2()
		  << " dxy:"          << std::setw(8) << track->dxy()
		  << " dz:"           << std::setw(8) << track->dz()
		  << " nPixHits:"     << std::setw(2) << track->hitPattern().numberOfValidPixelHits()
		  << " nVTkHits:"     << std::setw(2) << track->hitPattern().numberOfValidTrackerHits()
		  << " nTkLayWMeas:"  << std::setw(2) << track->hitPattern().trackerLayersWithMeasurement()
		  << " nMuStaWVHits:" << std::setw(2) << track->hitPattern().muonStationsWithValidHits()
		  << " nVHits:"       << std::setw(2) << track->hitPattern().numberOfValidHits()
		  << " nVMuHits:"     << std::setw(2) << track->hitPattern().numberOfValidMuonHits()
		  << std::endl;
    }
    
    if (trackerTrackColl->size() > 0) {
      std::cout	<< "found "     << nTrackerTracks  << " tracker "  << std::endl;
      std::cout << " tracker tracks: " << std::endl;
      for (auto track = trackerTrackColl->begin(); track != trackerTrackColl->end(); ++track)
	std::cout << "y:"
		  << std::setw(8) << track->innerPosition().Y()
		  << "/"
		  << std::setw(8) << track->outerPosition().Y()
		  << " pt:"           << std::setw(8) << track->pt()
		  << " eta:"          << std::setw(8) << track->eta()
		  << " phi:"          << std::setw(8) << track->phi()
		  << " chi2:"         << std::setw(8) << track->chi2()
		  << " dxy:"          << std::setw(8) << track->dxy()
		  << " dz:"           << std::setw(8) << track->dz()
		  << " nPixHits:"     << std::setw(2) << track->hitPattern().numberOfValidPixelHits()
		  << " nVTkHits:"     << std::setw(2) << track->hitPattern().numberOfValidTrackerHits()
		  << " nTkLayWMeas:"  << std::setw(2) << track->hitPattern().trackerLayersWithMeasurement()
		  << " nMuStaWVHits:" << std::setw(2) << track->hitPattern().muonStationsWithValidHits()
		  << " nVHits:"       << std::setw(2) << track->hitPattern().numberOfValidHits()
		  << " nVMuHits:"     << std::setw(2) << track->hitPattern().numberOfValidMuonHits()
		  << std::endl;
    }
  }
  
  for (int tk = 0; tk < 3; ++tk) {
    int tkIdx = 0;
    if (debug_ > 2)
      std::cout << "looping over track collection " << tk << std::endl;
    for (auto trk = tracks[tk]->begin(); trk != tracks[tk]->end(); ++trk,++tkIdx) {
      
      track_isLower [tk][tkIdx] = abs(trk->innerPosition().Y()) < fabs(trk->outerPosition().Y());
      track_isUpper [tk][tkIdx] = abs(trk->innerPosition().Y()) > fabs(trk->outerPosition().Y());
      track_chi2    [tk][tkIdx] = trk->chi2();
      track_ndof    [tk][tkIdx] = trk->ndof();
      track_charge  [tk][tkIdx] = trk->charge();
      track_dxy     [tk][tkIdx] = trk->dxy();
      track_dz      [tk][tkIdx] = trk->dz();
      track_ptError [tk][tkIdx] = trk->ptError();
      track_dxyError[tk][tkIdx] = trk->dxyError();
      track_dzError [tk][tkIdx] = trk->dzError();
      track_trackPt [tk][tkIdx] = trk->pt();
      track_trackEta[tk][tkIdx] = trk->eta();
      track_trackPhi[tk][tkIdx] = trk->phi();
      track_innerY  [tk][tkIdx] = trk->innerPosition().Y();
      track_outerY  [tk][tkIdx] = trk->outerPosition().Y();

      track_pixHits    [tk][tkIdx] = trk->hitPattern().numberOfValidPixelHits();
      track_tkHits     [tk][tkIdx] = trk->hitPattern().numberOfValidTrackerHits();
      track_tkLayWMeas [tk][tkIdx] = trk->hitPattern().trackerLayersWithMeasurement();
      track_muonStaHits[tk][tkIdx] = trk->hitPattern().muonStationsWithValidHits();
      track_nVHits     [tk][tkIdx] = trk->hitPattern().numberOfValidHits();
      track_nVMuHits   [tk][tkIdx] = trk->hitPattern().numberOfValidMuonHits();

      // try to match to a muon, if found, take nMatchedMuonStations from here
      // insufficient to simply match deta/dphi, what about upper/lower splitting?
      // be able to know index so we can easily get the tuneP pT/dpT
      
      double bestDEta = maxDEta_;
      double bestDPhi = maxDPhi_;
      double bestDR   = maxDR_;
      int muIdx = 0;
      if (debug_ > 2)
	std::cout << "looking for matching muons to track " << tkIdx << std::endl;
      for (auto muon = muonColl->begin(); muon != muonColl->end(); ++muon) {
	double tmpDEta = fabs(muon->eta()-trk->eta());
	double tmpDPhi = fabs(reco::deltaPhi(muon->phi(),trk->phi()));
	double tmpDR   = reco::deltaR(*muon,*trk);
	
	//double tmpTrkInY  = trk->innerPosition().Y();
	//double tmpMuInY  = muon->tunePMuonBestTrack()->innerPosition().Y();

	double tmpTrkOutY = trk->outerPosition().Y();
	double tmpMuOutY = muon->tunePMuonBestTrack()->outerPosition().Y();

	bool yCompatible = false;
	if (tmpTrkOutY > 0 && tmpMuOutY > 0)
	  yCompatible = true;
	else if (tmpTrkOutY < 0 && tmpMuOutY < 0)
	  yCompatible = true;
	
	if (tmpDEta < bestDEta && tmpDPhi < bestDPhi) {
	  if (debug_ > 2)
	    std::cout << " deta = " << std::setw(6) << tmpDEta
		      << " dphi = " << std::setw(6) << tmpDPhi
		      << " y-outer (mu) = "  << std::setw(8) << muon->tunePMuonBestTrack()->outerPosition().Y()
		      << " y-outer (trk) = " << std::setw(8) << trk->outerPosition().Y()
		      << " (" << std::setw(2) << (yCompatible ? "" : "in") << "compatible)"
		      << std::endl;
	  if (yCompatible) {
	    bestDEta = tmpDEta;
	    bestDPhi = tmpDPhi;
	    if (tmpDR < bestDR)
	      bestDR   = tmpDR;
	    track_matchedMuIdx[tk][tkIdx] = muIdx;
	    //track_nMatSta[tk][tkIdx] = muon->numberOfMatchedStations(reco::Muon::SegmentAndTrackArbitration);
	    track_nMatSta[tk][tkIdx] = muon->numberOfMatchedStations();
	  } // end check on y compatibility
	} // end check on deta/dphi
	++muIdx;
      }
    }
    if (debug_ > 2)
      std::cout << std::endl;
  }// end loop over the differen track collections
  muonTree->Fill();
}


reco::TrackRef MuonTree::GetTrackType(int algoType, reco::Muon const* muon)
{
  
  if (debug_ > 2)
    std::cout << "Started finding track type!" << std::endl;

  reco::TrackRef ref;
  
  if      (algoType == 1) ref = muon->track();
  else if (algoType == 2) ref = muon->tpfmsTrack();
  else if (algoType == 3) ref = muon->dytTrack();
  else if (algoType == 4) ref = muon->pickyTrack();
  else if (algoType == 5) ref = muon->tunePMuonBestTrack();
  else                    ref = muon->track();
  
  if (debug_ > 2)
    std::cout << "Returning track ref " << ref.isNonnull() << std::endl;

  return ref;
}

// ------------ method called once each job just before starting event loop  ------------
void MuonTree::beginJob()
{
  edm::Service< TFileService > fs;
  
  muonTree = fs->make<TTree>( "MuonTree", "Muon variables" );

  muonTree->Branch("nMuons",     &nMuons,     "nMuons/I"    );
  muonTree->Branch("nUpperLegs", &nUpperLegs, "nUpperLegs/I");
  muonTree->Branch("nLowerLegs", &nLowerLegs, "nLowerLegs/I");

  muonTree->Branch("nGlobalTracks",  &nGlobalTracks,  "nGlobalTracks/I" );
  muonTree->Branch("nCosmicTracks",  &nCosmicTracks,  "nCosmicTracks/I" );
  muonTree->Branch("nTrackerTracks", &nTrackerTracks, "nTrackerTracks/I");

  muonTree->Branch("event",      &event,      "event/I"     );
  muonTree->Branch("run",        &run,        "run/I"       );
  muonTree->Branch("lumi",       &lumi,       "lumi/I"      );

  // variables per muon ([nMuons] indexed)
  muonTree->Branch("globalpT",  muon_pT,  "globalpT[nMuons]/D" );
  muonTree->Branch("globalEta", muon_Eta, "globalEta[nMuons]/D");
  muonTree->Branch("globalPhi", muon_Phi, "globalPhi[nMuons]/D");
  muonTree->Branch("muonP4",    muonP4,   10000, 99            );

  muonTree->Branch("isGlobal",     muon_isGlobal,     "isGlobal[nMuons]/I"    );
  muonTree->Branch("isTracker",    muon_isTracker,    "isTracker[nMuons]/I"   );
  muonTree->Branch("isStandAlone", muon_isStandAlone, "isStandAlone[nMuons]/I");
  muonTree->Branch("isLower",      muon_isLower,      "isLower[nMuons]/I"     );
  muonTree->Branch("isUpper",      muon_isUpper,      "isUpper[nMuons]/I"     );

  muonTree->Branch("innerY",    muon_innerY, "innerY[nMuons]/D");
  muonTree->Branch("outerY",    muon_outerY, "outerY[nMuons]/D");
  muonTree->Branch("tpin",      muon_tpin,   "tpin[nMuons]/D"  );
  muonTree->Branch("tpout",     muon_tpout,  "tpout[nMuons]/D" );

  muonTree->Branch("trackpT",  muon_trackPt,  "trackpT[nMuons]/D" );
  muonTree->Branch("trackEta", muon_trackEta, "trackEta[nMuons]/D");
  muonTree->Branch("trackPhi", muon_trackPhi, "trackPhi[nMuons]/D");
  muonTree->Branch("trackVec", muon_trackVec, 10000, 99           );
  muonTree->Branch("chi2",     muon_chi2,     "chi2[nMuons]/D"    );
  muonTree->Branch("ndof",     muon_ndof,     "ndof[nMuons]/I"    );
  muonTree->Branch("charge",   muon_charge,   "charge[nMuons]/I"  );
  muonTree->Branch("dxy",      muon_dxy,      "dxy[nMuons]/D"     );
  muonTree->Branch("dz",       muon_dz,       "dz[nMuons]/D"      );
  muonTree->Branch("ptError",  muon_ptError,  "ptError[nMuons]/D" );
  muonTree->Branch("dxyError", muon_dxyError, "dxyError[nMuons]/D");
  muonTree->Branch("dzError",  muon_dzError,  "dzError[nMuons]/D" );

  muonTree->Branch("pixelHits",        muon_pixHits,     "pixelHits[nMuons]/I"       );
  muonTree->Branch("trackerHits",      muon_tkHits,      "trackerHits[nMuons]/I"     );
  muonTree->Branch("muonStationHits",  muon_muonStaHits, "muonStationHits[nMuons]/I" );
  muonTree->Branch("nValidHits",       muon_nVHits,      "nValidHits[nMuons]/I"      );
  muonTree->Branch("nValidMuonHits",   muon_nVMuHits,    "nValidMuonHits[nMuons]/I"  );
  muonTree->Branch("nMatchedStations", muon_nMatSta,     "nMatchedStations[nMuons]/I");
  muonTree->Branch("tkLayersWMeas",    muon_tkLayWMeas,  "tkLayersWMeas[nMuons]/I"   );

  // variables per muon ([nTracks] indexed)
  muonTree->Branch("trk_innerY",    track_innerY, "trk_innerY[3][10]/D");
  muonTree->Branch("trk_outerY",    track_outerY, "trk_outerY[3][10]/D");
  muonTree->Branch("trk_tpin",      track_tpin,   "trk_tpin[3][10]/D"  );
  muonTree->Branch("trk_tpout",     track_tpout,  "trk_tpout[3][10]/D" );

  muonTree->Branch("trk_trackpT",  track_trackPt,  "trk_trackpT[3][10]/D" );
  muonTree->Branch("trk_trackEta", track_trackEta, "trk_trackEta[3][10]/D");
  muonTree->Branch("trk_trackPhi", track_trackPhi, "trk_trackPhi[3][10]/D");
  muonTree->Branch("trk_chi2",     track_chi2,     "trk_chi2[3][10]/D"    );
  muonTree->Branch("trk_isLower",  track_isLower,  "trk_isLower[3][10]/I" );
  muonTree->Branch("trk_isUpper",  track_isUpper,  "trk_isUpper[3][10]/I" );
  muonTree->Branch("trk_ndof",     track_ndof,     "trk_ndof[3][10]/I"    );
  muonTree->Branch("trk_charge",   track_charge,   "trk_charge[3][10]/I"  );
  muonTree->Branch("trk_dxy",      track_dxy,      "trk_dxy[3][10]/D"     );
  muonTree->Branch("trk_dz",       track_dz,       "trk_dz[3][10]/D"      );
  muonTree->Branch("trk_ptError",  track_ptError,  "trk_ptError[3][10]/D" );
  muonTree->Branch("trk_dxyError", track_dxyError, "trk_dxyError[3][10]/D");
  muonTree->Branch("trk_dzError",  track_dzError,  "trk_dzError[3][10]/D" );

  muonTree->Branch("trk_matchedMuIdx",   track_matchedMuIdx,   "trk_matchedMuIdx[3][10]/I"  );

  muonTree->Branch("trk_pixelHits",        track_pixHits,     "trk_pixelHits[3][10]/I"       );
  muonTree->Branch("trk_trackerHits",      track_tkHits,      "trk_trackerHits[3][10]/I"     );
  muonTree->Branch("trk_muonStationHits",  track_muonStaHits, "trk_muonStationHits[3][10]/I" );
  muonTree->Branch("trk_nValidHits",       track_nVHits,      "trk_nValidHits[3][10]/I"      );
  muonTree->Branch("trk_nValidMuonHits",   track_nVMuHits,    "trk_nValidMuonHits[3][10]/I"  );
  muonTree->Branch("trk_nMatchedStations", track_nMatSta,     "trk_nMatchedStations[3][10]/I");
  muonTree->Branch("trk_tkLayersWMeas",    track_tkLayWMeas,  "trk_tkLayersWMeas[3][10]/I"   );
}



// ------------ method called once each job just after ending the event loop  ------------
void MuonTree::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
  void MuonTree::beginRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a run  ------------
/*
  void MuonTree::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
  void MuonTree::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
  void MuonTree::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void MuonTree::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(MuonTree);
