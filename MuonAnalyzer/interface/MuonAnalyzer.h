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

class MuonAnalyzer : public edm::EDAnalyzer {

public:
  explicit MuonAnalyzer(const edm::ParameterSet&);
  ~MuonAnalyzer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void TrackFill(reco::TrackRef ref, reco::MuonCollection::const_iterator muon, reco::Muon::ArbitrationType const& arbType);
  reco::TrackRef GetTrackType(int algoType,reco::MuonCollection::const_iterator muon);
  int algoType_;
  int debug_;

  
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
  edm::InputTag muonSrc_;
   edm::Service<TFileService> fs;
  TTree *cosmicTree;
  TH1F * h_pt, * h_eta, * h_phi, * h_charge;
  TH1F * h_curve; 
  TH1F * h_dtheta;
  TH1F * h_dphi;
  TH1F * h_thetaMu1;
  TH1F * h_thetaMu2;
  TH2F * h2_pt1_vs_pt2, * h2_eta1_vs_eta2, * h2_phi1_vs_phi2;

  int event, run, lumi;

 reco::Muon::ArbitrationType type;
 
  int muon1_charge, muon2_charge;
  double muon1_pt, muon1_eta, muon1_phi, muon1_theta;
  double muon2_pt, muon2_eta, muon2_phi, muon2_theta;

  //   TLorentzVector lowerMuon_P4;
   reco::Candidate::LorentzVector lowerMuon_P4;
   math::XYZVector lowerMuon_trackVec;
   double lowerMuon_chi2, lowerMuon_dxy, lowerMuon_dz, lowerMuon_pT;
  double lowerMuon_ptError, lowerMuon_dxyError, lowerMuon_dzError;
  double lowerMuon_trackPt;
  int lowerMuon_ndof, lowerMuon_charge, lowerMuon_pixelHits;
  int lowerMuon_trackerHits, lowerMuon_muonStationHits;
  int lowerMuon_numberOfValidMuonHits, lowerMuon_numberOfValidHits, lowerMuon_numberOfMatchedStations;
  int lowerMuon_trackerLayersWithMeasurement;


  //TLorentzVector upperMuon_P4; 
  reco::Candidate::LorentzVector upperMuon_P4;
  math::XYZVector upperMuon_trackVec;
  double upperMuon_chi2, upperMuon_dz, upperMuon_dxy, upperMuon_pT;
  double upperMuon_ptError, upperMuon_dxyError, upperMuon_dzError;
  double upperMuon_trackPt;
  int upperMuon_ndof, upperMuon_charge,upperMuon_pixelHits;
  int  upperMuon_trackerHits, upperMuon_muonStationHits;
  int  upperMuon_numberOfValidMuonHits, upperMuon_numberOfValidHits, upperMuon_numberOfMatchedStations;
  int upperMuon_trackerLayersWithMeasurement;

};


#endif
