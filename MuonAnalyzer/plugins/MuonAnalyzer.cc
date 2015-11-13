#include "WSUDiLeptons/MuonAnalyzer/interface/MuonAnalyzer.h"
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
  muonSrc_  = pset.getParameter<edm::InputTag>("muonSrc");
  debug_    = pset.getParameter<int>("debug");
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
  
  if ( muonColl->size() < 2)
    return;

  /*
    not doing this any more as we need to catch all the good cosmics
    if ( muonColl->size() != 2)
    return;
  */
  
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
  
  upperMuon_pixelHits                    = -1; lowerMuon_pixelHits                    = -1;
  upperMuon_trackerHits                  = -1; lowerMuon_trackerHits                  = -1;
  upperMuon_muonStationHits              = -1; lowerMuon_muonStationHits              = -1;
  upperMuon_numberOfValidHits            = -1; lowerMuon_numberOfValidHits            = -1;
  upperMuon_numberOfValidMuonHits        = -1; lowerMuon_numberOfValidMuonHits        = -1;
  upperMuon_numberOfMatchedStations      = -1; lowerMuon_numberOfMatchedStations      = -1;
  upperMuon_trackerLayersWithMeasurement = -1; lowerMuon_trackerLayersWithMeasurement = -1;

  //int goodPairs[2] = [-1,-1];
  double deta = 0.1;
  double dphi = 0.05;
  double dr   = 0.15;
  reco::MuonCollection::const_iterator muon = muonColl->begin();
  std::shared_ptr<reco::Muon> bestMatch = findBestMatch(muon, *muonColl, deta, dphi, dr);
  // keep the upper/lower legs as an std::pair<upper leg, lower leg>
  std::pair<std::shared_ptr<reco::Muon>, std::shared_ptr<reco::Muon> > bestPair;
  std::vector<std::shared_ptr<reco::Muon> > muonPair;

  // while (bestMatch.get() == NULL && ++muon != muonColl->end()) {
  if (debug_ > 2)
    std::cout << "before call: muon " << std::hex << *muon << std::endl;
  while (!bestMatch && ++muon != muonColl->end()) {
    bestMatch = findBestMatch(muon, *muonColl, deta, dphi, dr);
    if (debug_ > 2)
      std::cout << "after call: muon "  << std::hex << *muon << std::endl;
  }
  
  if (muon == muonColl->end())
    std::cout << "muon iterator is at the end of the collection" << std::endl;
  
  // have to ensure that we get a pair
  // if (bestMatch.get() == NULL) {
  if (!bestMatch) {
    std::cout << "unable to match two legs using deta(" << deta
	      << "), dphi(" << dphi
	      << "), dr("   << dr
	      << ")"        << std::endl;
    return;
  }
  
  if (debug_ > 2)
    std::cout << "comparison " << std::hex << *muon
	      << "comparison " << std::hex << *bestMatch << std::endl;

  // only keep going if we find global muons
  if (!muon->isGlobalMuon())
    return;
  if (!bestMatch->isGlobalMuon())
    return;
  
  if (muon->standAloneMuon()->innerPosition().Y() > 0)
    bestPair = std::make_pair<std::shared_ptr<reco::Muon>, std::shared_ptr<reco::Muon> >(std::make_shared<reco::Muon>(*muon),
											 std::make_shared<reco::Muon>(*bestMatch));
  else
    bestPair = std::make_pair<std::shared_ptr<reco::Muon>, std::shared_ptr<reco::Muon> >(std::make_shared<reco::Muon>(*bestMatch),
											 std::make_shared<reco::Muon>(*muon));
  
  muonPair.push_back(bestPair.first);
  muonPair.push_back(bestPair.second);
  int muIdx = 0;
  for (auto leg = muonPair.begin(); leg != muonPair.end(); ++leg) {
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
    
    if (debug_ > 1)
      std::cout << "creating Track ref variable" << std::endl;
    
    reco::TrackRef ref;
    
    if (debug_ > 1)
      std::cout << " variable createing. Calling GetTrackType" << std::endl;
    
    ref = GetTrackType(algoType_, leg->get());
    
    if (debug_ > 1)
      std::cout << "Found Track Type. If NonNull Fill." << std::endl;
    
    if (ref.isNonnull())
      TrackFill(ref, leg->get(), type);
    
    if (debug_ > 1)
      std::cout << "Filled Histograms!" << std::endl ;
    
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
    ++muIdx;
  } // end for (auto leg = muonPair.begin(); leg != muonPair.end(); ++leg)
  cosmicTree->Fill();
}
  

std::shared_ptr<reco::Muon> MuonAnalyzer::findBestMatch(reco::MuonCollection::const_iterator& mu1,
							reco::MuonCollection const& muons,
							double deta, double dphi, double dr)
{
  std::shared_ptr<reco::Muon> theBestMu;
  if (debug_ > 2)
    std::cout << "starting theBestMu = " << std::hex << theBestMu.get() << std::endl;
  double bestDEta = 100.;
  double bestDPhi = 100.;
  double bestDR   = 100.;
  for (auto mu = muons.begin(); mu != muons.end(); ++mu) {
    if (mu == mu1) {
      if (debug_ > 2)
	std::cout << "iterators matched, moving on" << std::endl;
      continue;
    }
    if (debug_ > 2) {
      std::cout << "mu "  << std::hex << *mu  << std::endl;
      std::cout << "mu1 " << std::hex << *mu1 << std::endl;
    }
    double tmpDEta = fabs(mu->eta()-mu1->eta());
    double tmpDPhi = fabs(reco::deltaPhi(mu->phi(),mu1->phi()));
    double tmpDR   = reco::deltaR(*mu,*mu1);

    if (tmpDEta < deta && tmpDEta < bestDEta) {
      if (debug_ > 2)
	std::cout << "tmpDEta < deta && tmpDEta < bestDEta" << std::endl;
      bestDEta = tmpDEta;
      if (tmpDPhi < dphi && tmpDPhi < bestDPhi) {
	if (debug_ > 2)
	  std::cout << "tmpDPhi < dphi && tmpDPhi < bestDPhi" << std::endl;
	bestDPhi = tmpDPhi;
	theBestMu = std::make_shared<reco::Muon>(*mu);
	if (debug_ > 2)
	  std::cout << "setting theBestMu = " << std::hex << theBestMu.get() << std::endl;
	if (tmpDR < dr && tmpDR < bestDR) {
	  bestDR = tmpDR;
	  if (debug_ > 2)
	    std::cout << "passes deltaR cut" << std::endl;
	}
      }
    }
      std::cout << "tmpDEta = "   << tmpDEta
		<< ", tmpDPhi = " << tmpDPhi
		<< ", tmpDR = "   << tmpDR << std::endl
		<< "bestDEta = "   << bestDEta
		<< ", bestDPhi = " << bestDPhi
		<< ", bestDR = "   << bestDR << std::endl;
  }
  if (debug_ > 2)
    std::cout << "returning theBestMu = " << std::hex << theBestMu.get() << std::endl;
  return theBestMu;
}


void MuonAnalyzer::TrackFill(reco::TrackRef ref, reco::Muon const* muon, reco::Muon::ArbitrationType const& arbType)
{
  if (debug_ > 1)
    std::cout << "Starting to Fill Histograms!" << std::endl;

  if (ref->outerPosition().Y() > 0) {
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

    if ( debug_ > 3) {
      double relError = upperMuon_ptError/upperMuon_trackPt;
      std::cout << "Upper Muon pT Error/pT is: " << relError       << std::endl
		<< "Upper Muon pT Error is: "    << ref->ptError() << std::endl
		<< "Upper Muon pT is: "          << ref->pt()      << std::endl;
      std::cout << "Upper Number of Pixel hits are: "           << ref->hitPattern().numberOfValidPixelHits()    << std::endl
		<< "Upper Number of Valid Tracker Hits are : "  << ref->hitPattern().numberOfValidTrackerHits()  << std::endl
		<< "Upper Number of Valid Muon Hits are: "      << ref->hitPattern().muonStationsWithValidHits() << std::endl
		<< "Upper Number of matched muon stations is: " << muon->numberOfMatchedStations(arbType)        << std::endl;
    } 
  }
  
  else if (ref->outerPosition().Y() < 0) {
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
    
    if ( debug_ > 3) {
      double relError = lowerMuon_ptError/lowerMuon_trackPt;
      std::cout << "Lower Muon pT Error/pT is: " << relError       << std::endl
		<< "Lower Muon pT Error is: "    << ref->ptError() << std::endl
		<< "Lower Muon pT is: "          << ref->pt()      << std::endl;
      std::cout << "Lower Number of Pixel hits are: "           << ref->hitPattern().numberOfValidPixelHits()    << std::endl
		<< "Lower Number of Valid Tracker Hits are : "  << ref->hitPattern().numberOfValidTrackerHits()  << std::endl
		<< "Lower Number of Valid Muon Hits are: "      << ref->hitPattern().muonStationsWithValidHits() << std::endl
		<< "Lower Number of matched muon stations is: " << muon->numberOfMatchedStations(arbType)        << std::endl;
    } 
  }
  if (debug_ > 1)
    std::cout << "Histograms Filled!" << std::endl;
}


reco::TrackRef MuonAnalyzer::GetTrackType(int algoType, reco::Muon const* muon)
{
  
  if (debug_ > 1)
    std::cout << "Started Finding Track Type!" << std::endl;

  reco::TrackRef ref;
  
  if      (algoType == 1) ref = muon->track();
  else if (algoType == 2) ref = muon->tpfmsTrack();
  else if (algoType == 3) ref = muon->dytTrack();
  else if (algoType == 4) ref = muon->pickyTrack();
  else if (algoType == 5) ref = muon->tunePMuonBestTrack();
  else                    ref = muon->track();
  
  if (debug_ > 1)
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
