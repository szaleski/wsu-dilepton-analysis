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
  debug_       = pset.getParameter<int>("debug");
  algoType_    = pset.getParameter<int>("algoType");

  maxDEta_ = pset.getParameter<double>("maxDEta");
  maxDPhi_ = pset.getParameter<double>("maxDPhi");
  maxDR_   = pset.getParameter<double>("maxDR");
  minPt_   = pset.getParameter<double>("minPt");

  muonToken_     = consumes<reco::MuonCollection>(muonSrc_);
  upperLegToken_ = consumes<reco::MuonCollection>(upperLegSrc_);
  lowerLegToken_ = consumes<reco::MuonCollection>(lowerLegSrc_);
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
  //edm::Handle<edm::View<reco::Muon> > muonColl;
  edm::Handle<reco::MuonCollection > muonColl;
  edm::Handle<reco::MuonCollection > upperLegColl;
  edm::Handle<reco::MuonCollection > lowerLegColl;
  ev.getByToken(muonToken_,     muonColl);
  ev.getByToken(upperLegToken_, upperLegColl);
  ev.getByToken(lowerLegToken_, lowerLegColl);
  
  // skip processing empty collection
  if ( muonColl->size() < 1)
    return;

  event = (ev.id()).event();  
  run   = (ev.id()).run();
  lumi  = ev.luminosityBlock();
  
  //type = reco::Muon::SegmentAndTrackArbitration;
  
  nMuons     = muonColl->size();
  nUpperLegs = upperLegColl->size();
  nLowerLegs = lowerLegColl->size();

  matchDR = 100.;  matchDEta = 100.;  matchDPhi = 100.;
  foundMatch = -1;

  for (int idx = 0; idx < 10; ++idx) {
    muon_isGlobal    [idx] = -1;
    muon_isTracker   [idx] = -1;
    muon_isStandAlone[idx] = -1;
    
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
  }
  
  if (debug_ > 0)
    std::cout << std::endl
	      << "found "     << nMuons     << " muons "      << std::endl
	      << "found "     << nUpperLegs << " upper legs " << std::endl
	      << "found "     << nLowerLegs << " lower legs " << std::endl
	      << "trackAlgo " << algoType_ << std::endl;
  if (debug_ > -1) {
    std::cout << "run/lumi/event " << run  << "/" << lumi << "/" << event << std::endl;
    //for (auto muon = muonColl->begin(); muon != muonColl->end(); ++muon)
    std::cout << " muons: " << std::endl;
    for (reco::MuonCollection::const_iterator muon = muonColl->begin();
	 muon != muonColl->end(); ++muon)
      std::cout << std::setw(5) << *muon << " ("
		<< muon->isTrackerMuon()            << "t"
		<< "/"  << muon->isGlobalMuon()     << "g"
		<< "/"  << muon->isStandAloneMuon() << "sa"
		<< ") " << std::setw(10) << "y:"
		<< muon->muonBestTrack()->innerPosition().Y()
		<< "/"
		<< muon->muonBestTrack()->outerPosition().Y()
		<< " "  << std::setw(10) << "chi2:"  << muon->muonBestTrack()->chi2()
		<< " "  << std::setw(10) << "dxy:"   << muon->muonBestTrack()->dxy()
		<< " "  << std::setw(10) << "dz:"    << muon->muonBestTrack()->dz()
		<< " "  << std::setw(10) << "tpin:"  << muon->time().timeAtIpInOut
		<< " "  << std::setw(10) << "tpout:" << muon->time().timeAtIpOutIn
		<< std::endl;
    std::cout << " upper legs: " << std::endl;
    for (reco::MuonCollection::const_iterator muon = upperLegColl->begin();
	 muon != upperLegColl->end(); ++muon)
      std::cout << std::setw(5) << *muon << " ("
		<< muon->isTrackerMuon()            << "t"
		<< "/"  << muon->isGlobalMuon()     << "g"
		<< "/"  << muon->isStandAloneMuon() << "sa"
		<< ") " << muon->muonBestTrack()->innerPosition().Y()
		<< std::endl;
    std::cout << " lower legs: " << std::endl;
    for (reco::MuonCollection::const_iterator muon = lowerLegColl->begin();
	 muon != lowerLegColl->end(); ++muon)
      std::cout << std::setw(5) << *muon << " ("
		<< muon->isTrackerMuon()            << "t"
		<< "/"  << muon->isGlobalMuon()     << "g"
		<< "/"  << muon->isStandAloneMuon() << "sa"
		<< ") " << muon->muonBestTrack()->innerPosition().Y()
		<< std::endl;
  }
  std::cout.flush();  

  if ( muonColl->size() < 1)
    return;
  
  int muIdx = 0;
  for (auto mu = muonColl->begin(); mu != muonColl->end(); ++ mu) {
    //reco::TrackRef ref = wsu::dileptons::MuonHelpers::GetTrackType(algoType_, &(*mu));
    reco::TrackRef ref = GetTrackType(algoType_, &(*mu));
    
    muon_innerY[muIdx] = mu->muonBestTrack()->innerPosition().Y();
    muon_outerY[muIdx] = mu->muonBestTrack()->outerPosition().Y();
    muon_tpin[muIdx]   = mu->time().timeAtIpInOut;
    muon_tpout[muIdx]  = mu->time().timeAtIpOutIn;

    muon_isTracker   [muIdx] = mu->isTrackerMuon();
    muon_isGlobal    [muIdx] = mu->isGlobalMuon();
    muon_isStandAlone[muIdx] = mu->isStandAloneMuon();

    muonP4       [muIdx] = mu->p4();
    muon_pT      [muIdx] = mu->pt();
    muon_Eta     [muIdx] = mu->eta();
    muon_Phi     [muIdx] = mu->phi();
    if (ref.isNonnull()) { // can't dereference if the desired track ref is null
      // selections are done on "best track" what should we do for study?
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
      // take hit pattern from associated TrackRef? why or why not?
      muon_pixHits    [muIdx] = ref->hitPattern().numberOfValidPixelHits();
      muon_tkHits     [muIdx] = ref->hitPattern().numberOfValidTrackerHits();
      muon_tkLayWMeas [muIdx] = ref->hitPattern().trackerLayersWithMeasurement();
      muon_muonStaHits[muIdx] = ref->hitPattern().muonStationsWithValidHits();
      muon_nVHits     [muIdx] = ref->hitPattern().numberOfValidHits();
      muon_nVMuHits   [muIdx] = ref->hitPattern().numberOfValidMuonHits();
    }

    if (debug_ > 3) {
      std::cout << "nPixHits: " << ((ref.isNonnull())?ref->hitPattern().numberOfValidPixelHits():-1) << "ref"
		<< "/" << ((mu->muonBestTrack().isNonnull())?mu->muonBestTrack()->hitPattern().numberOfValidPixelHits():-1) << "best"
		<< "/" << ((mu->track().isNonnull())?mu->track()->hitPattern().numberOfValidPixelHits():-1) << "track"
		<< "/" << ((mu->tpfmsTrack().isNonnull())?mu->tpfmsTrack()->hitPattern().numberOfValidPixelHits():-1) << "tpfms"
		<< "/" << ((mu->dytTrack().isNonnull())?mu->dytTrack()->hitPattern().numberOfValidPixelHits():-1) << "dyt"
		<< "/" << ((mu->pickyTrack().isNonnull())?mu->pickyTrack()->hitPattern().numberOfValidPixelHits():-1) << "picky"
		<< "/" << ((mu->tunePMuonBestTrack().isNonnull())?mu->tunePMuonBestTrack()->hitPattern().numberOfValidPixelHits():-1) << "tunep"
	
		<< "/" << ((mu->innerTrack().isNonnull())?mu->innerTrack()->hitPattern().numberOfValidPixelHits():-1) << "i"
		<< "/" << ((mu->globalTrack().isNonnull())?mu->globalTrack()->hitPattern().numberOfValidPixelHits():-1) << "g"
		<< "/" << ((mu->outerTrack().isNonnull())?mu->outerTrack()->hitPattern().numberOfValidPixelHits():-1) << "o"
		<< "/" << ((mu->standAloneMuon().isNonnull())?mu->standAloneMuon()->hitPattern().numberOfValidPixelHits():-1) << "sa"
		<< std::endl
	
		<< "nTkHits: " << ((ref.isNonnull())?ref->hitPattern().numberOfValidTrackerHits():-1) << "ref"
		<< "/" << ((mu->muonBestTrack().isNonnull())?mu->muonBestTrack()->hitPattern().numberOfValidTrackerHits():-1) << "best"
		<< "/" << ((mu->track().isNonnull())?mu->track()->hitPattern().numberOfValidTrackerHits():-1) << "track"
		<< "/" << ((mu->tpfmsTrack().isNonnull())?mu->tpfmsTrack()->hitPattern().numberOfValidTrackerHits():-1) << "tpfms"
		<< "/" << ((mu->dytTrack().isNonnull())?mu->dytTrack()->hitPattern().numberOfValidTrackerHits():-1) << "dyt"
		<< "/" << ((mu->pickyTrack().isNonnull())?mu->pickyTrack()->hitPattern().numberOfValidTrackerHits():-1) << "picky"
		<< "/" << ((mu->tunePMuonBestTrack().isNonnull())?mu->tunePMuonBestTrack()->hitPattern().numberOfValidTrackerHits():-1) << "tunep"
	
		<< "/" << ((mu->innerTrack().isNonnull())?mu->innerTrack()->hitPattern().numberOfValidTrackerHits():-1) << "i"
		<< "/" << ((mu->globalTrack().isNonnull())?mu->globalTrack()->hitPattern().numberOfValidTrackerHits():-1) << "g"
		<< "/" << ((mu->outerTrack().isNonnull())?mu->outerTrack()->hitPattern().numberOfValidTrackerHits():-1) << "o"
		<< "/" << ((mu->standAloneMuon().isNonnull())?mu->standAloneMuon()->hitPattern().numberOfValidTrackerHits():-1) << "sa"
		<< std::endl

		<< "nVHits: " << ((ref.isNonnull())?ref->hitPattern().numberOfValidHits():-1) << "ref"
		<< "/" << ((mu->muonBestTrack().isNonnull())?mu->muonBestTrack()->hitPattern().numberOfValidHits():-1) << "best"
		<< "/" << ((mu->track().isNonnull())?mu->track()->hitPattern().numberOfValidHits():-1) << "track"
		<< "/" << ((mu->tpfmsTrack().isNonnull())?mu->tpfmsTrack()->hitPattern().numberOfValidHits():-1) << "tpfms"
		<< "/" << ((mu->dytTrack().isNonnull())?mu->dytTrack()->hitPattern().numberOfValidHits():-1) << "dyt"
		<< "/" << ((mu->pickyTrack().isNonnull())?mu->pickyTrack()->hitPattern().numberOfValidHits():-1) << "picky"
		<< "/" << ((mu->tunePMuonBestTrack().isNonnull())?mu->tunePMuonBestTrack()->hitPattern().numberOfValidHits():-1) << "tunep"
	
		<< "/" << ((mu->innerTrack().isNonnull())?mu->innerTrack()->hitPattern().numberOfValidHits():-1) << "i"
		<< "/" << ((mu->globalTrack().isNonnull())?mu->globalTrack()->hitPattern().numberOfValidHits():-1) << "g"
		<< "/" << ((mu->outerTrack().isNonnull())?mu->outerTrack()->hitPattern().numberOfValidHits():-1) << "o"
		<< "/" << ((mu->standAloneMuon().isNonnull())?mu->standAloneMuon()->hitPattern().numberOfValidHits():-1) << "sa"
		<< std::endl

		<< "nVMuHits: " << ((ref.isNonnull())?ref->hitPattern().numberOfValidMuonHits():-1) << "ref"
		<< "/" << ((mu->muonBestTrack().isNonnull())?mu->muonBestTrack()->hitPattern().numberOfValidMuonHits():-1) << "best"
		<< "/" << ((mu->track().isNonnull())?mu->track()->hitPattern().numberOfValidMuonHits():-1) << "track"
		<< "/" << ((mu->tpfmsTrack().isNonnull())?mu->tpfmsTrack()->hitPattern().numberOfValidMuonHits():-1) << "tpfms"
		<< "/" << ((mu->dytTrack().isNonnull())?mu->dytTrack()->hitPattern().numberOfValidMuonHits():-1) << "dyt"
		<< "/" << ((mu->pickyTrack().isNonnull())?mu->pickyTrack()->hitPattern().numberOfValidMuonHits():-1) << "picky"
		<< "/" << ((mu->tunePMuonBestTrack().isNonnull())?mu->tunePMuonBestTrack()->hitPattern().numberOfValidMuonHits():-1) << "tunep"
	
		<< "/" << ((mu->innerTrack().isNonnull())?mu->innerTrack()->hitPattern().numberOfValidMuonHits():-1) << "i"
		<< "/" << ((mu->globalTrack().isNonnull())?mu->globalTrack()->hitPattern().numberOfValidMuonHits():-1) << "g"
		<< "/" << ((mu->outerTrack().isNonnull())?mu->outerTrack()->hitPattern().numberOfValidMuonHits():-1) << "o"
		<< "/" << ((mu->standAloneMuon().isNonnull())?mu->standAloneMuon()->hitPattern().numberOfValidMuonHits():-1) << "sa"
		<< std::endl

		<< "muonStaHits: " << ((ref.isNonnull())?ref->hitPattern().muonStationsWithValidHits():-1) << "ref"
		<< "/" << ((mu->muonBestTrack().isNonnull())?mu->muonBestTrack()->hitPattern().muonStationsWithValidHits():-1) << "best"
		<< "/" << ((mu->track().isNonnull())?mu->track()->hitPattern().muonStationsWithValidHits():-1) << "track"
		<< "/" << ((mu->tpfmsTrack().isNonnull())?mu->tpfmsTrack()->hitPattern().muonStationsWithValidHits():-1) << "tpfms"
		<< "/" << ((mu->dytTrack().isNonnull())?mu->dytTrack()->hitPattern().muonStationsWithValidHits():-1) << "dyt"
		<< "/" << ((mu->pickyTrack().isNonnull())?mu->pickyTrack()->hitPattern().muonStationsWithValidHits():-1) << "picky"
		<< "/" << ((mu->tunePMuonBestTrack().isNonnull())?mu->tunePMuonBestTrack()->hitPattern().muonStationsWithValidHits():-1) << "tunep"
	
		<< "/" << ((mu->innerTrack().isNonnull())?mu->innerTrack()->hitPattern().muonStationsWithValidHits():-1) << "i"
		<< "/" << ((mu->globalTrack().isNonnull())?mu->globalTrack()->hitPattern().muonStationsWithValidHits():-1) << "g"
		<< "/" << ((mu->outerTrack().isNonnull())?mu->outerTrack()->hitPattern().muonStationsWithValidHits():-1) << "o"
		<< "/" << ((mu->standAloneMuon().isNonnull())?mu->standAloneMuon()->hitPattern().muonStationsWithValidHits():-1) << "sa"
		<< std::endl

		<< "tkLayWMeas: " << ((ref.isNonnull())?ref->hitPattern().trackerLayersWithMeasurement():-1) << "ref"
		<< "/" << ((mu->muonBestTrack().isNonnull())?mu->muonBestTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "best"
		<< "/" << ((mu->track().isNonnull())?mu->track()->hitPattern().trackerLayersWithMeasurement():-1) << "track"
		<< "/" << ((mu->tpfmsTrack().isNonnull())?mu->tpfmsTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "tpfms"
		<< "/" << ((mu->dytTrack().isNonnull())?mu->dytTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "dyt"
		<< "/" << ((mu->pickyTrack().isNonnull())?mu->pickyTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "picky"
		<< "/" << ((mu->tunePMuonBestTrack().isNonnull())?mu->tunePMuonBestTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "tunep"
		
		<< "/" << ((mu->innerTrack().isNonnull())?mu->innerTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "i"
		<< "/" << ((mu->globalTrack().isNonnull())?mu->globalTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "g"
		<< "/" << ((mu->outerTrack().isNonnull())?mu->outerTrack()->hitPattern().trackerLayersWithMeasurement():-1) << "o"
		<< "/" << ((mu->standAloneMuon().isNonnull())?mu->standAloneMuon()->hitPattern().trackerLayersWithMeasurement():-1) << "sa"
		<< std::endl;

    }
    /*
    if (muon_isGlobal) {
      muon_pixHits    [muIdx] = mu->globalTrack()->hitPattern().numberOfValidPixelHits();
      muon_tkHits     [muIdx] = mu->globalTrack()->hitPattern().numberOfValidTrackerHits();
      muon_tkLayWMeas [muIdx] = mu->globalTrack()->hitPattern().trackerLayersWithMeasurement();
      muon_muonStaHits[muIdx] = mu->globalTrack()->hitPattern().muonStationsWithValidHits();
      muon_nVHits     [muIdx] = mu->globalTrack()->hitPattern().numberOfValidHits();
      muon_nVMuHits   [muIdx] = mu->globalTrack()->hitPattern().numberOfValidMuonHits();
    } else { // use the variables from the inner/outer track if available
      if (muon_isTracker) {
	muon_pixHits    [muIdx] = mu->innerTrack()->hitPattern().numberOfValidPixelHits();
	muon_tkHits     [muIdx] = mu->innerTrack()->hitPattern().numberOfValidTrackerHits();
	muon_tkLayWMeas [muIdx] = mu->innerTrack()->hitPattern().trackerLayersWithMeasurement();
	//muon_muonStaHits[muIdx] = mu->innerTrack()->hitPattern().muonStationsWithValidHits();
	//muon_nVHits     [muIdx] = mu->innerTrack()->hitPattern().numberOfValidHits();
	//muon_nVMuHits   [muIdx] = mu->innerTrack()->hitPattern().numberOfValidMuonHits();
      }
      if (muon_isStandAlone) {
	//muon_pixHits    [muIdx] = mu->outerTrack()->hitPattern().numberOfValidPixelHits();
	//muon_tkHits     [muIdx] = mu->outerTrack()->hitPattern().numberOfValidTrackerHits();
	//muon_tkLayWMeas [muIdx] = mu->outerTrack()->hitPattern().trackerLayersWithMeasurement();
	muon_muonStaHits[muIdx] = mu->outerTrack()->hitPattern().muonStationsWithValidHits();
	muon_nVHits     [muIdx] = mu->outerTrack()->hitPattern().numberOfValidHits();
	muon_nVMuHits   [muIdx] = mu->outerTrack()->hitPattern().numberOfValidMuonHits();
      }
    }
    */
    muon_nMatSta[muIdx] = mu->numberOfMatchedStations(reco::Muon::SegmentAndTrackArbitration);
    ++muIdx;
  } // end loop over muons
  
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
  muonTree->Branch("event",      &event,      "event/I"     );
  muonTree->Branch("run",        &run,        "run/I"       );
  muonTree->Branch("lumi",       &lumi,       "lumi/I"      );

  muonTree->Branch("matchDR",    &matchDR,    "matchDR/D"   );
  muonTree->Branch("matchDPhi",  &matchDPhi,  "matchDPhi/D" );
  muonTree->Branch("matchDEta",  &matchDEta,  "matchDEta/D" );
  muonTree->Branch("foundMatch", &foundMatch, "foundMatch/I");

  // variables per muon ([nMuons] indexed)
  muonTree->Branch("globalpT",  muon_pT,  "globalpT[nMuons]/D" );
  muonTree->Branch("globalEta", muon_Eta, "globalEta[nMuons]/D");
  muonTree->Branch("globalPhi", muon_Phi, "globalPhi[nMuons]/D");
  muonTree->Branch("muonP4",    muonP4,   10000, 99            );

  muonTree->Branch("isGlobal",     muon_isGlobal,     "isGlobal[nMuons]/I"    );
  muonTree->Branch("isTracker",    muon_isTracker,    "isTracker[nMuons]/I"   );
  muonTree->Branch("isStandAlone", muon_isStandAlone, "isStandAlone[nMuons]/I");

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
