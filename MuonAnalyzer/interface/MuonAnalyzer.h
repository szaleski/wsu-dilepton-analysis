#ifndef WSUDILEPTONS_MUONANALYZER_H
#define WSUDILEPTONS_MUONANALYZER_H


// system include files
#include <memory>

// user include files
#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EDAnalyzer.h>

#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>

#include <FWCore/ParameterSet/interface/ParameterSet.h>
#include <DataFormats/PatCandidates/interface/Muon.h>
#include <DataFormats/Candidate/interface/Candidate.h>
#include <DataFormats/MuonReco/interface/MuonCocktails.h>
#include <DataFormats/MuonReco/interface/MuonFwd.h>
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"

//sim track information
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

// Trigger information
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"

// TFile Service
#include <FWCore/ServiceRegistry/interface/Service.h>
#include <CommonTools/UtilAlgos/interface/TFileService.h>
#include <TTree.h>
#include <TVector2.h>
#include <TH2.h>
#include <TH1.h>
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TMath.h"


//
// class declaration
//

class MuonAnalyzer : public edm::EDAnalyzer {

 public:
  explicit MuonAnalyzer(const edm::ParameterSet&);
  ~MuonAnalyzer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void TrackFill(reco::TrackRef ref, reco::Muon const* muon, reco::Muon::ArbitrationType const& arbType);
  reco::TrackRef GetTrackType(int algoType, reco::Muon const* muon);
  std::shared_ptr<reco::Muon> findBestMatch(reco::MuonCollection::const_iterator& mu1,
					    reco::MuonCollection const& muons,
					    double deta, double dphi, double dr);
  
  int algoType_;
  int debug_;

  double maxDR_;
  double maxDPhi_;
  double maxDEta_;
  double minPt_;

  
  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //  //The following says we do not know what parameters are allowed so do no validation
  //  // Please change this to state exactly what you do use, even if it is no parameters
  //  edm::ParameterSetDescription desc;
  //  desc.setUnknown();
  //  descriptions.addDefault(desc);
  //}
  
  // ----------member data ---------------------------
  edm::EDGetTokenT<reco::MuonCollection > muonToken_, tagLegToken_, probeLegToken_;
  edm::EDGetTokenT<edm::SimTrackContainer> simTrackToken_;
  edm::EDGetTokenT<edm::TriggerResults>   trigResultsToken_;
  edm::EDGetTokenT<bool> fakeL1SingleMuToken_;

  edm::InputTag muonSrc_, tagLegSrc_, probeLegSrc_;
  edm::InputTag simTrackSrc_, trigResultsSrc_, fakeL1SingleMuSrc_;
  edm::Service<TFileService> fs;

  std::string hltTrigCut_;
  bool isGen_;
  
  TTree *cosmicTree;

  int event, run, lumi, nMuons, nTags, nProbes, foundMatch;
  double matchDR, matchDEta, matchDPhi;
  
  reco::Muon::ArbitrationType type;
 
  int    nSimTracks, simtrack_type[5];
  double simtrack_pt[5], simtrack_eta[5], simtrack_phi[5], simtrack_charge[5];

  int    l1SingleMu, fakeL1SingleMu;

  reco::Candidate::LorentzVector lowerMuon_P4;
  math::XYZVector lowerMuon_trackVec;
  double lowerMuon_chi2, lowerMuon_dxy, lowerMuon_dz, lowerMuon_pT;
  double lowerMuon_ptError, lowerMuon_dxyError, lowerMuon_dzError;
  double lowerMuon_trackPt;
  int lowerMuon_ndof, lowerMuon_charge,
    lowerMuon_isGlobal, lowerMuon_isTracker, lowerMuon_isStandAlone;
  int lowerMuon_firstPixel,
    lowerMuon_pixelHits,
    lowerMuon_trackerHits,
    lowerMuon_muonStationHits,
    lowerMuon_numberOfValidHits,
    lowerMuon_numberOfValidMuonHits,
    lowerMuon_numberOfMatchedStations,
    lowerMuon_trackerLayersWithMeasurement;

  reco::Candidate::LorentzVector upperMuon_P4;
  math::XYZVector upperMuon_trackVec;
  double upperMuon_chi2, upperMuon_dz, upperMuon_dxy, upperMuon_pT;
  double upperMuon_ptError, upperMuon_dxyError, upperMuon_dzError;
  double upperMuon_trackPt;
  int upperMuon_ndof, upperMuon_charge,
    upperMuon_isGlobal, upperMuon_isTracker, upperMuon_isStandAlone;
  int  upperMuon_firstPixel,
    upperMuon_pixelHits,
    upperMuon_trackerHits,
    upperMuon_muonStationHits,
    upperMuon_numberOfValidHits,
    upperMuon_numberOfValidMuonHits,
    upperMuon_numberOfMatchedStations,
    upperMuon_trackerLayersWithMeasurement;

};


#endif
