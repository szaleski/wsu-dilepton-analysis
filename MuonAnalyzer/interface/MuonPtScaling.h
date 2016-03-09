#ifndef WSUDILEPTONS_MUONPTSCALING_H
#define WSUDILEPTONS_MUONPTSCALING_H

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

// TFile Service
#include <FWCore/ServiceRegistry/interface/Service.h>
#include <CommonTools/UtilAlgos/interface/TFileService.h>
#include <TH1.h>
#include "TROOT.h"
#include "TFile.h"


//
// class declaration
//

class MuonPtScaling : public edm::EDAnalyzer {

 public:
  explicit MuonPtScaling(const edm::ParameterSet&);
  ~MuonPtScaling();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  // ----------member data ---------------------------
  edm::EDGetTokenT<reco::MuonCollection > muonToken_;
  edm::InputTag muonSrc_;

  bool debug_;

  edm::Service<TFileService> fs;
  TH1D  *allMuonPt, *leadingMuonPt;
  TH1D  *allMuonPtEta09, *leadingMuonPtEta09;
};


#endif
