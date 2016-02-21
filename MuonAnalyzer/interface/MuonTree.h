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
#include "DataFormats/MuonReco/src/Muon.cc"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"

// TFile Service
#include <FWCore/ServiceRegistry/interface/Service.h>
#include <CommonTools/UtilAlgos/interface/TFileService.h>
#include <TTree.h>
#include <TVector2.h>
#include <TH2.h>
#include <TH1.h>
#include "TROOT.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TBrowser.h"
#include "TMath.h"
#include "TRandom.h"
#include "TCanvas.h"


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
  edm::InputTag muonSrc_, upperLegSrc_, lowerLegSrc_;
  edm::Service<TFileService> fs;
  TTree *muonTree;

  int event, run, lumi, nMuons, nUpperLegs, nLowerLegs, foundMatch;
  double matchDR, matchDEta, matchDPhi;
  
  //reco::Muon::ArbitrationType type;
 
  reco::Candidate::LorentzVector muonP4[10];
  math::XYZVector muon_trackVec[10];
  double muon_innerY[10], muon_outerY[10], muon_tpin[10], muon_tpout[10];
  double muon_chi2[10], muon_dxy[10], muon_dz[10], muon_pT[10];
  double muon_ptError[10], muon_dxyError[10], muon_dzError[10];
  double muon_trackPt[10];
  int muon_ndof[10], muon_charge[10], muon_isGlobal[10], muon_isTracker[10], muon_isStandAlone[10];
  int muon_pixHits[10],muon_tkHits[10],muon_muonStaHits[10],
    muon_nVHits[10],muon_nVMuHits[10],muon_nMatSta[10],muon_tkLayWMeas[10];
};


#endif
