#ifndef WSUDILEPTONS_MUONTREE_H
#define WSUDILEPTONS_MUONTREE_H


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

class MuonTree : public edm::EDAnalyzer {

 public:
  explicit MuonTree(const edm::ParameterSet&);
  ~MuonTree();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  reco::TrackRef GetTrackType(int algoType, reco::Muon const* muon);

  bool isGen_;  
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
  edm::EDGetTokenT<reco::MuonCollection > muonToken_, upperLegToken_, lowerLegToken_;
  edm::EDGetTokenT<reco::TrackCollection > globalTrackToken_, cosmicTrackToken_, trackerTrackToken_;
  edm::EDGetTokenT<edm::SimTrackContainer> simTrackToken_;
  edm::EDGetTokenT<std::vector<l1extra::L1MuonParticle> > l1MuonToken_;
  edm::EDGetTokenT<edm::TriggerResults>   trigResultsToken_;
  edm::EDGetTokenT<bool> fakeL1SingleMuToken_;

  edm::InputTag muonSrc_, upperLegSrc_, lowerLegSrc_;
  edm::InputTag globalTrackSrc_, cosmicTrackSrc_, trackerTrackSrc_, simTrackSrc_;
  edm::InputTag l1MuonSrc_, trigResultsSrc_, fakeL1SingleMuSrc_;

  std::string hltTrigCut_;
  edm::Service<TFileService> fs;

  TTree *muonTree;

  int event, run, lumi,
    nMuons, nUpperLegs, nLowerLegs,
    nGlobalTracks, nCosmicTracks, nTrackerTracks;
  
  //reco::Muon::ArbitrationType type;

  reco::Candidate::LorentzVector muonP4[10];
  math::XYZVector muon_trackVec[10];
 
  
  int    nSimTracks, simtrack_type[25];
  double simtrack_pt[25], simtrack_eta[25], simtrack_phi[25], simtrack_charge[25];

  int    nL1Muons, l1SingleMu, fakeL1SingleMu, l1muon_isFwd[25], l1muon_isRPC[25],
    l1muon_quality[25], l1muon_detector[25], l1muon_bx[25];
  double l1muon_pt[25], l1muon_eta[25], l1muon_phi[25], l1muon_charge[25];

  double muon_innerY[25], muon_outerY[25], muon_tpin[25], muon_tpout[25];
  double muon_global_chi2[25],muon_inner_chi2[25],muon_outer_chi2[25];
  int    muon_global_ndof[25],muon_inner_ndof[25],muon_outer_ndof[25];
  double muon_chi2[25], muon_dxy[25], muon_dz[25], muon_pT[25];
  double muon_ptError[25], muon_dxyError[25], muon_dzError[25];
  double muon_Pt[25], muon_Eta[25], muon_Phi[25];
  double muon_trackPt[25], muon_trackEta[25], muon_trackPhi[25];
  int muon_hasGlobal[25], muon_hasInner[25], muon_hasOuter[25];
  int muon_isUpper[25], muon_isLower[25], muon_isUpperOld[25], muon_isLowerOld[25],
    muon_isGlobal[25], muon_isTracker[25], muon_isStandAlone[25];
  int muon_ndof[25], muon_charge[25];
  int muon_firstPixel[25],muon_pixHits[25],muon_tkHits[25],muon_muonStaHits[25],
    muon_nVHits[25],muon_nVMuHits[25],muon_nMatSta[25],muon_tkLayWMeas[25];

  double track_innerY[7][25], track_outerY[7][25], track_tpin[7][25], track_tpout[7][25];
  double track_chi2[7][25], track_dxy[7][25], track_dz[7][25], track_pT[7][25];
  double track_ptError[7][25], track_dxyError[7][25], track_dzError[7][25];
  double track_Pt[7][25], track_Eta[7][25], track_Phi[7][25];
  double track_trackPt[7][25], track_trackEta[7][25], track_trackPhi[7][25];
  int track_isUpper[7][25], track_isLower[7][25], track_isUpperOld[7][25], track_isLowerOld[7][25];
  int track_ndof[7][25], track_charge[7][25], track_matchedMuIdx[7][25];
  int track_firstPixel[7][25],track_pixHits[7][25],track_tkHits[7][25],track_muonStaHits[7][25],
    track_nVHits[7][25],track_nVMuHits[7][25],track_nMatSta[7][25],track_tkLayWMeas[7][25];
};


#endif
