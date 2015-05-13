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

// TFile Service
#include <FWCore/ServiceRegistry/interface/Service.h>
#include <CommonTools/UtilAlgos/interface/TFileService.h>
#include <TTree.h>
#include <TH2.h>
#include <TH1.h>

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
  TH1F * h_pt, * h_eta, * h_phi;
  TH2F * h2_pt1_vs_pt2, * h2_eta1_vs_eta2, * h2_phi1_vs_phi2;

  int muon1_charge, muon2_charge;
  double muon1_pt, muon1_eta, muon1_phi;
  double muon2_pt, muon2_eta, muon2_phi;
};

#endif
