#include "WSUDiLeptons/MuonAnalyzer/interface/MuonAnalyzer.h"
//#include "WSUDiLeptons/MuonAnalyzer/interface/MuonHelpers.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"

// -*- C++ -*-
//
// Package:    WSUDiLeptons/MuonAnalyzer
// Class:      MuonAnalyzer
// 
/**\class MuonAnalyzer MuonAnalyzer.cc WSUDiLeptons/MuonAnalyzer/plugins/MuonAnalyzer.cc

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
  muonSrc_     = pset.getParameter<edm::InputTag>("muonSrc");
  tagLegSrc_   = pset.getParameter<edm::InputTag>("tagLegSrc");
  probeLegSrc_ = pset.getParameter<edm::InputTag>("probeLegSrc");

  isGen_       = pset.getParameter<bool>("isGen");
  if (isGen_) {
    simTrackSrc_ = pset.getParameter<edm::InputTag>("simTrackSrc");
  }

  trigResultsSrc_    = pset.getParameter<edm::InputTag>("trigResultsSrc");
  fakeL1SingleMuSrc_ = pset.getParameter<edm::InputTag>("fakeL1SingleMuSrc");
  hltTrigCut_        = pset.getParameter<std::string>("hltTrigCut");

  debug_       = pset.getParameter<int>("debug");
  algoType_    = pset.getParameter<int>("algoType");

  maxDEta_ = pset.getParameter<double>("maxDEta");
  maxDPhi_ = pset.getParameter<double>("maxDPhi");
  maxDR_   = pset.getParameter<double>("maxDR");
  minPt_   = pset.getParameter<double>("minPt");

  //now do what ever initialization is needed
  muonToken_     = consumes<reco::MuonCollection>(muonSrc_);
  tagLegToken_   = consumes<reco::MuonCollection>(tagLegSrc_);
  probeLegToken_ = consumes<reco::MuonCollection>(probeLegSrc_);

  if (isGen_) {
    simTrackToken_ = consumes<edm::SimTrackContainer>(simTrackSrc_);
  } 

  trigResultsToken_    = consumes<edm::TriggerResults>(trigResultsSrc_);
  fakeL1SingleMuToken_ = consumes<bool>(fakeL1SingleMuSrc_);
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
  edm::Handle<reco::MuonCollection > tagLegColl;
  edm::Handle<reco::MuonCollection > probeLegColl;
  ev.getByToken(muonToken_, muonColl);
  ev.getByToken(tagLegToken_, tagLegColl);
  ev.getByToken(probeLegToken_, probeLegColl);
  
  edm::Handle<edm::SimTrackContainer > simTrackColl;
  if (isGen_)
    ev.getByToken(simTrackToken_, simTrackColl);

  edm::Handle<edm::TriggerResults> triggerResults;
  ev.getByToken(trigResultsToken_, triggerResults);

  edm::Handle<bool>                   fakeL1SingleMuH;
  ev.getByToken(fakeL1SingleMuToken_, fakeL1SingleMuH);      

  // skip processing empty collection
  if ( muonColl->size() < 1)
    return;

  event = (ev.id()).event();  
  run   = (ev.id()).run();
  lumi  = ev.luminosityBlock();
  
  type = reco::Muon::SegmentAndTrackArbitration;
  
  nMuons  = muonColl->size();
  nTags   = tagLegColl->size();
  nProbes = probeLegColl->size();

  nSimTracks = -1;
  for (int idx = 0; idx < 5; ++idx) {
    // initialize muon sim track variables
    simtrack_type[idx]   = 0;
    simtrack_pt[idx]     = -1.;
    simtrack_eta[idx]    = -10.;
    simtrack_phi[idx]    = -10.;
    simtrack_charge[idx] = -10;
  }

  matchDR = 100.;  matchDEta = 100.;  matchDPhi = 100.;
  foundMatch = -1;

  upperMuon_isGlobal     = -1; lowerMuon_isGlobal     = -1;
  upperMuon_isTracker    = -1; lowerMuon_isTracker    = -1;
  upperMuon_isStandAlone = -1; lowerMuon_isStandAlone = -1;

  upperMuon_P4.SetXYZT(0,0,0,-1); lowerMuon_P4.SetXYZT(0,0,0,-1);
  upperMuon_pT = -1;              lowerMuon_pT = -1;
  
  upperMuon_trackVec.SetXYZ(0,0,0); lowerMuon_trackVec.SetXYZ(0,0,0);
  upperMuon_trackPt = -1;           lowerMuon_trackPt = -1;
  upperMuon_ptError = -1;           lowerMuon_ptError = -1;
  
  upperMuon_charge = -10; lowerMuon_charge = -10;
  upperMuon_chi2   = -1;  lowerMuon_chi2   = -1;
  upperMuon_ndof   = -1;  lowerMuon_ndof   = -1;
  
  upperMuon_dxy      = -1000; lowerMuon_dxy      = -1000;
  upperMuon_dz       = -1000; lowerMuon_dz       = -1000;
  upperMuon_dxyError = -1;    lowerMuon_dxyError = -1;
  upperMuon_dzError  = -1;    lowerMuon_dzError  = -1;
  
  upperMuon_firstPixel                   = -1; lowerMuon_firstPixel                   = -1;
  upperMuon_pixelHits                    = -1; lowerMuon_pixelHits                    = -1;
  upperMuon_trackerHits                  = -1; lowerMuon_trackerHits                  = -1;
  upperMuon_muonStationHits              = -1; lowerMuon_muonStationHits              = -1;
  upperMuon_numberOfValidHits            = -1; lowerMuon_numberOfValidHits            = -1;
  upperMuon_numberOfValidMuonHits        = -1; lowerMuon_numberOfValidMuonHits        = -1;
  upperMuon_numberOfMatchedStations      = -1; lowerMuon_numberOfMatchedStations      = -1;
  upperMuon_trackerLayersWithMeasurement = -1; lowerMuon_trackerLayersWithMeasurement = -1;

  if (debug_ > 0)
    std::cout << std::endl
	      << "found "     << nMuons    << " muons "  << std::endl
	      << "found "     << nTags     << " tags "   << std::endl
	      << "found "     << nProbes   << " probes " << std::endl
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
		<< muon->tunePMuonBestTrack()->innerPosition().Y()
		<< "/"
		<< muon->tunePMuonBestTrack()->outerPosition().Y()
		<< " "  << std::setw(10) << "chi2:"  << muon->tunePMuonBestTrack()->chi2()
		<< " "  << std::setw(10) << "dxy:"   << muon->tunePMuonBestTrack()->dxy()
		<< " "  << std::setw(10) << "dz:"    << muon->tunePMuonBestTrack()->dz()
		<< " "  << std::setw(10) << "tpin:"  << muon->time().timeAtIpInOut
		<< " "  << std::setw(10) << "tpout:" << muon->time().timeAtIpOutIn
		<< std::endl;
    std::cout << " tag legs: " << std::endl;
    for (reco::MuonCollection::const_iterator muon = tagLegColl->begin();
	 muon != tagLegColl->end(); ++muon)
      std::cout << std::setw(5) << *muon << " ("
		<< muon->isTrackerMuon()            << "t"
		<< "/"  << muon->isGlobalMuon()     << "g"
		<< "/"  << muon->isStandAloneMuon() << "sa"
		<< ") " << muon->tunePMuonBestTrack()->innerPosition().Y()
		<< std::endl;
    std::cout << " probe legs: " << std::endl;
    for (reco::MuonCollection::const_iterator muon = probeLegColl->begin();
	 muon != probeLegColl->end(); ++muon)
      std::cout << std::setw(5) << *muon << " ("
		<< muon->isTrackerMuon()            << "t"
		<< "/"  << muon->isGlobalMuon()     << "g"
		<< "/"  << muon->isStandAloneMuon() << "sa"
		<< ") " << muon->tunePMuonBestTrack()->innerPosition().Y()
		<< std::endl;
  }
  std::cout.flush();  

  // reject events without tags or probes
  if ( tagLegColl->size() < 1 && probeLegColl->size() < 1)
    return;
  
  // what sort of matching should be done?
  // require opposite halves
  // require the deta/dphi cuts
  // look for match with highest pT muon/first in collection?
  // 
  //int goodPairs[2] = [-1,-1];
  // this will treat each time through the collection as a new matching
  double deta = maxDEta_;
  double dphi = maxDPhi_;
  double dr   = maxDR_;
  // this will ensure that the next match will be closer than the previous best,
  // even if using a new muon in the collection as the reference, is this desired?
  //double deta = matchDEta;
  //double dphi = matchDPhi;
  //double dr   = matchDR;

  // if we have tags but no probes then what?
  // if we have probes but no tags then what?
  reco::MuonCollection::const_iterator muon  = tagLegColl->size() > 0 ? tagLegColl->begin() : probeLegColl->begin();
  reco::MuonCollection::const_iterator muend = tagLegColl->size() > 0 ? tagLegColl->end()   : probeLegColl->end();
  
  std::shared_ptr<reco::Muon> bestMatch = findBestMatch(muon, tagLegColl->size() > 0 ? *probeLegColl:*tagLegColl, deta, dphi, dr);
  // keep the upper/lower legs as an std::pair<tag leg, probe leg>
  std::pair<std::shared_ptr<reco::Muon>, std::shared_ptr<reco::Muon> > bestPair;
  std::vector<std::shared_ptr<reco::Muon> > muonPair;

  if ((tagLegColl->size() > 0 && nTags > 1) || (probeLegColl->size() > 0 && nProbes > 1)) {
    if (debug_ > 2)
      std::cout << "before call: muon " << std::hex << *muon << std::dec << std::endl;
    // stop looping when we have a match, or reach the end of the collection
    // if mu(0) doesn't have a "best match" go to mu(1)
    // what if mu(2) and mu(4) were the best actual match, but mu(0) and mu(3) matched?
    while (!bestMatch && ++muon != muend) {
      if (muon->pt() < minPt_)
	continue;
      bestMatch = findBestMatch(muon, tagLegColl->size() > 0 ? *probeLegColl :  *tagLegColl, deta, dphi, dr);
      if (debug_ > 2)
	std::cout << "after call: muon " << std::hex << *muon << std::dec << std::endl;
    }
  }
  std::cout.flush();
  if (debug_ > 0)
    if (muon == muend)

      std::cout << "muon iterator is at the end of the collection" << std::endl;
  
  if (muon->pt() < minPt_) {
    if (debug_ > 2)
      std::cout << "selected muon pT too low, returning" << muon->pt() << std::endl;
    return;
  }
  
  // for the generic study, what to do when more than one muon and no match?
  // have to ensure that we get a pair
  if (!bestMatch) {
    foundMatch = 0;
    if (debug_ > 0)
      std::cout << "unable to match two legs using deta(" << matchDEta
		<< "), dphi(" << matchDPhi
		<< "), dr("   << matchDR
		<< ")"        << std::endl;

    // here need to select which muon to use?
    muon = tagLegColl->size() > 0 ? tagLegColl->begin() : probeLegColl->begin();
    //return;
  } else {
    foundMatch = 1;
    if (debug_ > 0)
      std::cout << "matched two legs using deta(" << matchDEta
		<< "), dphi(" << matchDPhi
		<< "), dr("   << matchDR
		<< ")"        << std::endl;
    //return;
  }

  if (debug_ > 2)
    std::cout << "have passed the matching portion, now filling variables " << std::endl;

  if (debug_ > -1)
    std::cout << "comparing tag " << std::hex << *muon << std::dec
	      << " (" << muon->isTrackerMuon()     << "t"
	      << "/"  << muon->isGlobalMuon()      << "g"
	      << "/"  << muon->isStandAloneMuon()  << "sa"
	      << ") " << muon->tunePMuonBestTrack()->innerPosition().Y() << std::endl;
  if (debug_ > -1 && bestMatch)
    std::cout << "with probe    " << std::hex << *bestMatch << std::dec
	      << " (" << bestMatch->isTrackerMuon()     << "t"
	      << "/"  << bestMatch->isGlobalMuon()      << "g"
	      << "/"  << bestMatch->isStandAloneMuon()  << "sa"
	      << ") " << bestMatch->tunePMuonBestTrack()->innerPosition().Y() << std::endl;
  std::cout.flush();

  // this still segfaults on occasion...
  bestPair = std::make_pair(std::make_shared<reco::Muon>(*muon),bestMatch);

  muonPair.push_back(bestPair.first);
  muonPair.push_back(bestPair.second);
  if (debug_ > 3)
    std::cout << "looping over legs" << std::endl;
  int muIdx = 0;
  for (auto leg = muonPair.begin(); leg != muonPair.end(); ++leg) {
    if (debug_ > 3)
      std::cout << "leg = " << std::hex << *leg << "/" << leg->get() << std::dec << std::endl;
    if (!(*leg)) {
    //if (!leg->get()) {
      if (debug_ > 3)
	std::cout << "null pointer found for leg " << muIdx << ", continuing" << std::endl;
      continue;
    }
    if (debug_ > 3) {
      std::cout << "globalMuon pt [GeV]: " << leg->get()->pt() << 
	"  eta : " << leg->get()->eta() << 
	"  phi : " << leg->get()->phi() << 
	std::endl;
      
      if (leg->get()->combinedMuon().isNonnull()) {
	reco::TrackRef ref = leg->get()->combinedMuon();
	std::cout << "combinedMuon number of hits: " << ref->numberOfValidHits() << std::endl;
      }
      
      if (leg->get()->standAloneMuon().isNonnull()) {
	reco::TrackRef ref = leg->get()->standAloneMuon();
	if (ref->innerOk())
	  std::cout << "standAloneMuon inner x [cm]: " << ref->innerPosition().X()
		    << "  outer x [cm]: "              << ref->outerPosition().X()
		    << "  inner y [cm]: "              << ref->innerPosition().Y()
		    << "  outer y [cm]: "              << ref->outerPosition().Y()
		    << std::endl;
      }
    }
    std::cout.flush();
    if (debug_ > 2)
      std::cout << "creating track ref variable" << std::endl;
    
    reco::TrackRef ref;
    
    if (debug_ > 2)
      std::cout << "Calling GetTrackType(" << algoType_ << "," << leg->get() << ")" << std::endl;
    
    //ref = wsu::dileptons::MuonHelpers::GetTrackType(algoType_, leg->get());
    ref = GetTrackType(algoType_, leg->get());
    
    if (debug_ > 2)
      std::cout << "Found track type for leg " << muIdx << ". If NonNull fill." << std::endl;
    
    if (ref.isNonnull())
      TrackFill(ref, leg->get(), type);
    
    if (debug_ > 2)
      std::cout << "Filled histograms!" << std::endl ;
    
    if (debug_ > 3) {
      if (leg->get()->track().isNonnull()) {
	reco::TrackRef ref = leg->get()->track();
	if (ref->outerOk())
	  std::cout << "track outer pt [GeV]: " << ref->outerPt();
	if (ref->innerOk())
	  std::cout << "  inner pt [GeV]: "     << ref->innerMomentum().Rho();
	std::cout << std::endl;
      }
    }
    std::cout.flush();
    ++muIdx;
  } // end for (auto leg = muonPair.begin(); leg != muonPair.end(); ++leg)

  ////////////////// edm::SimTrack information /////////////////////////
  if (isGen_) {
    if (simTrackColl->size() > 0) {
      nSimTracks = 0;
      int simIdx = 0;
      for (auto simtrack = simTrackColl->begin(); simtrack != simTrackColl->end(); ++simtrack) {
	if (fabs(simtrack->type()) == 13) { // only consider simtracks from muons
	  if (debug_ > 2)
	    std::cout << std::setw(5) << *simtrack << std::endl; 
	  
	  simtrack_charge[simIdx] = simtrack->charge();
	  simtrack_type[simIdx]   = simtrack->type();
	  simtrack_pt[simIdx]  = simtrack->trackerSurfaceMomentum().pt();
	  simtrack_eta[simIdx] = simtrack->trackerSurfaceMomentum().eta();
	  simtrack_phi[simIdx] = simtrack->trackerSurfaceMomentum().phi();
	  ++simIdx;
	  ++nSimTracks;
	}
      }
    }
  }
  
  // trigger information
  l1SingleMu = 0;
  const edm::TriggerNames& trigNames = ev.triggerNames(*triggerResults);
  for (unsigned int trig = 0; trig < trigNames.size(); ++trig) {
    //for (auto trig = triggerNames.begin(); trig != triggerNames.end(); ++trig) {
    if (triggerResults->accept(trig)) {
      std::string pathName = trigNames.triggerName(trig);
      if (debug_ > 2)
	std::cout << "Trigger path " << pathName << " fired" << std::endl;
      
      if (pathName.find(hltTrigCut_) != std::string::npos) {
	if (debug_ > 0)
	  std::cout << "Trigger path " << pathName << " fired" << std::endl;
	l1SingleMu = 1;
      }
    }
  }
  
  fakeL1SingleMu = *fakeL1SingleMuH;
  cosmicTree->Fill();
}
  

std::shared_ptr<reco::Muon> MuonAnalyzer::findBestMatch(reco::MuonCollection::const_iterator& mu1,
							reco::MuonCollection const& muons,
							double deta, double dphi, double dr)
{
  std::shared_ptr<reco::Muon> theBestMu;
  if (debug_ > 2)
    std::cout << "starting theBestMu = " << std::hex << theBestMu.get() << std::dec << std::endl;
  double bestDEta = deta;
  double bestDPhi = dphi;
  double bestDR   = dr;
  if (mu1->pt() < minPt_) {
    if (debug_ > 2)
      std::cout << "mu1 pT < minPt_, moving on" << std::endl;
    return theBestMu;
  }
  for (auto mu = muons.begin(); mu != muons.end(); ++mu) {
    if (mu == mu1) {
      if (debug_ > 2)
	std::cout << "iterators matched, moving on" << std::endl;
      continue;
    }
    /*
    if (((mu->tunePMuonBestTrack()->innerPosition().Y() > 0) &&
	 (mu1->tunePMuonBestTrack()->innerPosition().Y() > 0)) ||
	((mu->tunePMuonBestTrack()->innerPosition().Y() < 0) &&
	 (mu1->tunePMuonBestTrack()->innerPosition().Y() < 0))) {
      if (debug_ > 2)
	std::cout << "both muons in same half, moving on" << std::endl;
      continue;
    }
    */
    if (debug_ > 5) {
      std::cout << "comparing " << std::hex << *mu1 << std::dec << std::endl;
      std::cout << "with      " << std::hex << *mu  << std::dec << std::endl;
    }
    double tmpDEta = fabs(mu->eta()-mu1->eta());
    double tmpDPhi = fabs(reco::deltaPhi(mu->phi(),mu1->phi()));
    double tmpDR   = reco::deltaR(*mu,*mu1);

    if (tmpDEta < deta && tmpDEta < bestDEta) {
    //if (tmpDEta < bestDEta) {
      if (debug_ > 5)
	std::cout << "tmpDEta < deta && tmpDEta < bestDEta" << std::endl;
      bestDEta = tmpDEta;
      if (tmpDPhi < dphi && tmpDPhi < bestDPhi) {
      //if (tmpDPhi < bestDPhi) {
	if (debug_ > 5)
	  std::cout << "tmpDPhi < dphi && tmpDPhi < bestDPhi" << std::endl;
	bestDPhi = tmpDPhi;
	theBestMu = std::make_shared<reco::Muon>(*mu);
	if (debug_ > 5)
	  std::cout << "setting theBestMu = " << std::hex << theBestMu.get() << std::dec << std::endl;
	if (tmpDR < dr && tmpDR < bestDR) {
	//if (tmpDR < bestDR) {
	  bestDR = tmpDR;
	  if (debug_ > 5)
	    std::cout << "passes deltaR cut" << std::endl;
	}
      }
    }
    if (debug_ > 2)
      std::cout << "tmpDEta  = "   << tmpDEta
		<< ", tmpDPhi  = " << tmpDPhi
		<< ", tmpDR  = "   << tmpDR << std::endl
		<< "bestDEta = "   << bestDEta
		<< ", bestDPhi = " << bestDPhi
		<< ", bestDR = "   << bestDR << std::endl;
  }
  // update variables if best match is found
  if (theBestMu) {
    if (bestDEta < matchDEta)
      matchDEta = bestDEta;
    if (bestDPhi < matchDPhi)
      matchDPhi = bestDPhi;
    if (bestDR < matchDR)
      matchDR = bestDR;
  }
  if (debug_ > 2)
    std::cout << "returning theBestMu = " << std::hex << theBestMu.get() << std::dec << std::endl;
  return theBestMu;
}


void MuonAnalyzer::TrackFill(reco::TrackRef ref, reco::Muon const* muon, reco::Muon::ArbitrationType const& arbType)
{
  if (debug_ > 2)
    std::cout << "Starting to Fill Histograms!" << std::endl;

  //if (muon->tunePMuonBestTrack()->innerPosition().Y() > 0) {
  //if (fabs(muon->tunePMuonBestTrack()->innerPosition().Y()) > fabs(muon->tunePMuonBestTrack()->outerPosition().Y())) {
  if ((muon->outerTrack().isNonnull() && muon->outerTrack()->outerPosition().Y() > 0 ) || 
      (!(muon->outerTrack().isNonnull()) && (fabs(muon->innerTrack()->innerPosition().Y()) > fabs(muon->innerTrack()->outerPosition().Y())))) {
    if (debug_ > 2)
      std::cout << "upper muon, check on tracker" << std::endl;
    upperMuon_isTracker    = muon->isTrackerMuon();
    if (debug_ > 2)
      std::cout << "upper muon, check on global" << std::endl;
    upperMuon_isGlobal     = muon->isGlobalMuon();
    if (debug_ > 2)
      std::cout << "upper muon, check on standalone" << std::endl;
    upperMuon_isStandAlone = muon->isStandAloneMuon();

    upperMuon_P4       = muon->p4();
    upperMuon_pT       = muon->pt();
    if (debug_ > 2)
      std::cout << "upper muon, ref variables" << std::endl;
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
    
    if (debug_ > 3)
      std::cout << "upper muon, tracker/global/standalone " 
		<< upperMuon_isGlobal << "/"
		<< upperMuon_isTracker << "/"
		<< upperMuon_isStandAlone
		<< std::endl;

    if (upperMuon_isGlobal) {
      if (debug_ > 2)
	std::cout << "upper muon, global track hit pattern variables" << std::endl;
      upperMuon_firstPixel                   = (muon->globalTrack()->hitPattern().hasValidHitInFirstPixelBarrel() ||
						muon->globalTrack()->hitPattern().hasValidHitInFirstPixelEndcap());
      upperMuon_pixelHits                    = muon->globalTrack()->hitPattern().numberOfValidPixelHits();
      upperMuon_trackerHits                  = muon->globalTrack()->hitPattern().numberOfValidTrackerHits();
      upperMuon_trackerLayersWithMeasurement = muon->globalTrack()->hitPattern().trackerLayersWithMeasurement();
      upperMuon_muonStationHits              = muon->globalTrack()->hitPattern().muonStationsWithValidHits();
      upperMuon_numberOfValidHits            = muon->globalTrack()->hitPattern().numberOfValidHits();
      upperMuon_numberOfValidMuonHits        = muon->globalTrack()->hitPattern().numberOfValidMuonHits();
    } else { // use the variables from the inner/outer track if available
      if (upperMuon_isTracker) {
	if (debug_ > 2)
	  std::cout << "upper muon, inner track hit pattern variables" << std::endl;
	upperMuon_firstPixel                   = (muon->innerTrack()->hitPattern().hasValidHitInFirstPixelBarrel() ||
						  muon->innerTrack()->hitPattern().hasValidHitInFirstPixelEndcap());
	upperMuon_pixelHits                    = muon->innerTrack()->hitPattern().numberOfValidPixelHits();
	upperMuon_trackerHits                  = muon->innerTrack()->hitPattern().numberOfValidTrackerHits();
	upperMuon_trackerLayersWithMeasurement = muon->innerTrack()->hitPattern().trackerLayersWithMeasurement();
	//upperMuon_muonStationHits              = muon->innerTrack()->hitPattern().muonStationsWithValidHits();
	//upperMuon_numberOfValidHits            = muon->innerTrack()->hitPattern().numberOfValidHits();
	//upperMuon_numberOfValidMuonHits        = muon->innerTrack()->hitPattern().numberOfValidMuonHits();
      }
      if (upperMuon_isStandAlone) {
	if (debug_ > 2)
	  std::cout << "upper muon, outer track hit pattern variables" << std::endl;
	//upperMuon_pixelHits                    = muon->outerTrack()->hitPattern().numberOfValidPixelHits();
	//upperMuon_trackerHits                  = muon->outerTrack()->hitPattern().numberOfValidTrackerHits();
	//upperMuon_trackerLayersWithMeasurement = muon->outerTrack()->hitPattern().trackerLayersWithMeasurement();
	upperMuon_muonStationHits              = muon->outerTrack()->hitPattern().muonStationsWithValidHits();
	upperMuon_numberOfValidHits            = muon->outerTrack()->hitPattern().numberOfValidHits();
	upperMuon_numberOfValidMuonHits        = muon->outerTrack()->hitPattern().numberOfValidMuonHits();
      }
    }
    if (debug_ > 2)
      std::cout << "upper muon, matched stations" << std::endl;
    upperMuon_numberOfMatchedStations      = muon->numberOfMatchedStations(arbType);

    if ( debug_ > 3) {
      double relError = upperMuon_ptError/upperMuon_trackPt;
      std::cout << "Upper muon pT error/pT is: " << relError       << std::endl
		<< "Upper muon pT error is: "    << ref->ptError() << std::endl
		<< "Upper muon pT is: "          << ref->pt()      << std::endl;
      std::cout << "Upper number of pixel hits is: "           << ref->hitPattern().numberOfValidPixelHits();
      if (upperMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidPixelHits() << "i";
      if (upperMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidPixelHits() << "g";
      if (upperMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidPixelHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidPixelHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Upper number of valid tracker hits is: "  << ref->hitPattern().numberOfValidTrackerHits();
      if (upperMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidTrackerHits() << "i";
      if (upperMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidTrackerHits() << "g";
      if (upperMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidTrackerHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidTrackerHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Upper number of valid muon station hits is: "      << ref->hitPattern().muonStationsWithValidHits();
      if (upperMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().muonStationsWithValidHits() << "i";
      if (upperMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().muonStationsWithValidHits() << "g";
      if (upperMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().muonStationsWithValidHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().muonStationsWithValidHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Upper number of valid hits is: "      << ref->hitPattern().numberOfValidHits();
      if (upperMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidHits() << "i";
      if (upperMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidHits() << "g";
      if (upperMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Upper number of valid muon hits is: "      << ref->hitPattern().numberOfValidMuonHits();
      if (upperMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidMuonHits() << "i";
      if (upperMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidMuonHits() << "g";
      if (upperMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidMuonHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidMuonHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Upper number of matched muon stations is: " << muon->numberOfMatchedStations(arbType);
      std::cout << std::endl;
    }
  }
  
  //else if (muon->tunePMuonBestTrack()->innerPosition().Y() < 0) {
  //else if (fabs(muon->tunePMuonBestTrack()->innerPosition().Y()) < fabs(muon->tunePMuonBestTrack()->outerPosition().Y())) {
  else if ((muon->outerTrack().isNonnull() && muon->outerTrack()->outerPosition().Y() < 0 ) || 
	   (!(muon->outerTrack().isNonnull()) && (fabs(muon->innerTrack()->innerPosition().Y()) < fabs(muon->innerTrack()->outerPosition().Y())))) {
    if (debug_ > 2)
      std::cout << "lower muon, check on tracker" << std::endl;
    lowerMuon_isTracker    = muon->isTrackerMuon();
    if (debug_ > 2)
      std::cout << "lower muon, check on global" << std::endl;
    lowerMuon_isGlobal     = muon->isGlobalMuon();
    if (debug_ > 2)
      std::cout << "lower muon, check on standalone" << std::endl;
    lowerMuon_isStandAlone = muon->isStandAloneMuon();

    lowerMuon_P4       = muon->p4();
    lowerMuon_pT       = muon->pt();
    if (debug_ > 2)
      std::cout << "lower muon, ref variables" << std::endl;
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
    
    if (debug_ > 3)
      std::cout << "lower muon, tracker/global/standalone " 
		<< lowerMuon_isGlobal  << "/"
		<< lowerMuon_isTracker << "/"
		<< lowerMuon_isStandAlone
		<< std::endl;

    if (lowerMuon_isGlobal) {
      if (debug_ > 2)
	std::cout << "lower muon, inner track hit pattern variables" << std::endl;
      lowerMuon_firstPixel                   = (muon->globalTrack()->hitPattern().hasValidHitInFirstPixelBarrel() ||
						muon->globalTrack()->hitPattern().hasValidHitInFirstPixelEndcap());
      lowerMuon_pixelHits                    = muon->globalTrack()->hitPattern().numberOfValidPixelHits();
      lowerMuon_trackerHits                  = muon->globalTrack()->hitPattern().numberOfValidTrackerHits();
      lowerMuon_trackerLayersWithMeasurement = muon->globalTrack()->hitPattern().trackerLayersWithMeasurement();
      lowerMuon_muonStationHits              = muon->globalTrack()->hitPattern().muonStationsWithValidHits();
      lowerMuon_numberOfValidHits            = muon->globalTrack()->hitPattern().numberOfValidHits();
      lowerMuon_numberOfValidMuonHits        = muon->globalTrack()->hitPattern().numberOfValidMuonHits();
    } else { // use the variables from the inner/outer track if available
      if (lowerMuon_isTracker) {
	if (debug_ > 2)
	  std::cout << "lower muon, inner track hit pattern variables" << std::endl;
	lowerMuon_firstPixel                   = (muon->innerTrack()->hitPattern().hasValidHitInFirstPixelBarrel() ||
						  muon->innerTrack()->hitPattern().hasValidHitInFirstPixelEndcap());
	lowerMuon_pixelHits                    = muon->innerTrack()->hitPattern().numberOfValidPixelHits();
	lowerMuon_trackerHits                  = muon->innerTrack()->hitPattern().numberOfValidTrackerHits();
	lowerMuon_trackerLayersWithMeasurement = muon->innerTrack()->hitPattern().trackerLayersWithMeasurement();
	//lowerMuon_muonStationHits              = muon->innerTrack()->hitPattern().muonStationsWithValidHits();
	//lowerMuon_numberOfValidHits            = muon->innerTrack()->hitPattern().numberOfValidHits();
	//lowerMuon_numberOfValidMuonHits        = muon->innerTrack()->hitPattern().numberOfValidMuonHits();
      }
      if (lowerMuon_isStandAlone) {
	if (debug_ > 2)
	  std::cout << "lower muon, outer track hit pattern variables" << std::endl;
	//lowerMuon_pixelHits                    = muon->outerTrack()->hitPattern().numberOfValidPixelHits();
	//lowerMuon_trackerHits                  = muon->outerTrack()->hitPattern().numberOfValidTrackerHits();
	//lowerMuon_trackerLayersWithMeasurement = muon->outerTrack()->hitPattern().trackerLayersWithMeasurement();
	lowerMuon_muonStationHits              = muon->outerTrack()->hitPattern().muonStationsWithValidHits();
	lowerMuon_numberOfValidHits            = muon->outerTrack()->hitPattern().numberOfValidHits();
	lowerMuon_numberOfValidMuonHits        = muon->outerTrack()->hitPattern().numberOfValidMuonHits();
      }
    }
    if (debug_ > 2)
      std::cout << "lower muon, number of matched stations" << std::endl;
    lowerMuon_numberOfMatchedStations      = muon->numberOfMatchedStations(arbType);

    /* adapting for sensibility
    lowerMuon_pixelHits                    = ref->hitPattern().numberOfValidPixelHits();
    lowerMuon_trackerHits                  = ref->hitPattern().numberOfValidTrackerHits();
    lowerMuon_muonStationHits              = ref->hitPattern().muonStationsWithValidHits();
    lowerMuon_numberOfValidHits            = ref->hitPattern().numberOfValidHits();
    lowerMuon_numberOfValidMuonHits        = ref->hitPattern().numberOfValidMuonHits();
    lowerMuon_numberOfMatchedStations      = muon->numberOfMatchedStations(arbType);
    lowerMuon_trackerLayersWithMeasurement = ref->hitPattern().trackerLayersWithMeasurement();
    */
    if ( debug_ > 3) {
      double relError = lowerMuon_ptError/lowerMuon_trackPt;
      std::cout << "Lower muon pT error/pT is: " << relError       << std::endl
		<< "Lower muon pT error is: "    << ref->ptError() << std::endl
		<< "Lower muon pT is: "          << ref->pt()      << std::endl;
      std::cout << "Lower number of pixel hits is: "           << ref->hitPattern().numberOfValidPixelHits();
      if (lowerMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidPixelHits() << "i";
      if (lowerMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidPixelHits() << "g";
      if (lowerMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidPixelHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidPixelHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Lower number of valid tracker hits is: "  << ref->hitPattern().numberOfValidTrackerHits();
      if (lowerMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidTrackerHits() << "i";
      if (lowerMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidTrackerHits() << "g";
      if (lowerMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidTrackerHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidTrackerHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Lower number of valid muon station hits is: "      << ref->hitPattern().muonStationsWithValidHits();
      if (lowerMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().muonStationsWithValidHits() << "i";
      if (lowerMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().muonStationsWithValidHits() << "g";
      if (lowerMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().muonStationsWithValidHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().muonStationsWithValidHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Lower number of valid hits is: "      << ref->hitPattern().numberOfValidHits();
      if (lowerMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidHits() << "i";
      if (lowerMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidHits() << "g";
      if (lowerMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Lower number of valid muon hits is: "      << ref->hitPattern().numberOfValidMuonHits();
      if (lowerMuon_isTracker)
	std::cout << "/" << muon->innerTrack()->hitPattern().numberOfValidMuonHits() << "i";
      if (lowerMuon_isGlobal)
	std::cout << "/" << muon->globalTrack()->hitPattern().numberOfValidMuonHits() << "g";
      if (lowerMuon_isStandAlone) {
	std::cout << "/" << muon->outerTrack()->hitPattern().numberOfValidMuonHits() << "o";
	std::cout << "/" << muon->standAloneMuon()->hitPattern().numberOfValidMuonHits() << "sa";
      }
      std::cout << std::endl;
      std::cout << "Lower number of matched muon stations is: " << muon->numberOfMatchedStations(arbType);
      std::cout << std::endl;
    } 
  }
  if (debug_ > 2)
    std::cout << "Histograms Filled!" << std::endl;
}

reco::TrackRef MuonAnalyzer::GetTrackType(int algoType, reco::Muon const* muon)
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
void MuonAnalyzer::beginJob()
{
  edm::Service< TFileService > fs;
  
  cosmicTree = fs->make<TTree>( "MuonTree", "TTree variables" );

  cosmicTree->Branch("nMuons",  &nMuons,  10000, 1);
  cosmicTree->Branch("nTags",   &nTags,   10000, 1);
  cosmicTree->Branch("nProbes", &nProbes, 10000, 1);
  cosmicTree->Branch("muonEventNumber", &event, 10000, 1);
  cosmicTree->Branch("muonRunNumber",   &run,   10000, 1);
  cosmicTree->Branch("muonLumiBlock",   &lumi,  10000, 1);

  cosmicTree->Branch("matchDR",    &matchDR,    10000, 1);
  cosmicTree->Branch("matchDPhi",  &matchDPhi,  10000, 1);
  cosmicTree->Branch("matchDEta",  &matchDEta,  10000, 1);
  cosmicTree->Branch("foundMatch", &foundMatch, 10000, 1);

  cosmicTree->Branch("l1SingleMu",     &l1SingleMu,      "l1SingleMu/I");
  cosmicTree->Branch("fakeL1SingleMu", &fakeL1SingleMu, "fakeL1SingleMu/I");

  cosmicTree->Branch("nSimTracks", &nSimTracks, "nSimTracks/I");
  // variables per simTrack ([nSimTracks] indexed)
  cosmicTree->Branch("simTrackpT",  simtrack_pt,  "simTrackpT[nSimTracks]/D" );
  cosmicTree->Branch("simTrackEta", simtrack_eta, "simTrackEta[nSimTracks]/D");
  cosmicTree->Branch("simTrackPhi", simtrack_phi, "simTrackPhi[nSimTracks]/D");
  cosmicTree->Branch("simTrackCharge", simtrack_charge, "simTrackCharge[nSimTracks]/I");
  cosmicTree->Branch("simTrackType",   simtrack_type,   "simTrackType[nSimTracks]/I");

  /////////Muon in upper half of CMS
  cosmicTree->Branch("upperMuon_pT", &upperMuon_pT, 10000, 1);
  cosmicTree->Branch("upperMuon_P4", &upperMuon_P4, 10000, 1);

  cosmicTree->Branch("upperMuon_isGlobal",     &upperMuon_isGlobal,     10000, 1);
  cosmicTree->Branch("upperMuon_isTracker",    &upperMuon_isTracker,    10000, 1);
  cosmicTree->Branch("upperMuon_isStandAlone", &upperMuon_isStandAlone, 10000, 1);

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

  cosmicTree->Branch("upperMuon_firstPixel",                   &upperMuon_firstPixel,                   10000, 1);
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

  cosmicTree->Branch("lowerMuon_isGlobal",     &lowerMuon_isGlobal,     10000, 1);
  cosmicTree->Branch("lowerMuon_isTracker",    &lowerMuon_isTracker,    10000, 1);
  cosmicTree->Branch("lowerMuon_isStandAlone", &lowerMuon_isStandAlone, 10000, 1);

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

  cosmicTree->Branch("lowerMuon_firstPixel",                   &lowerMuon_firstPixel,                   10000, 1);
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
void MuonAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(MuonAnalyzer);
