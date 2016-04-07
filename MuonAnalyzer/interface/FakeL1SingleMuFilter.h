#ifndef WSUDILEPTONS_FAKEL1SINGLEMUFILTER_H
#define WSUDILEPTONS_FAKEL1SINGLEMUFILTER_H
// -*- C++ -*-
//
// Package:    WSUDiLeptons/MuonAnalyzer
// Class:      FakeL1SingleMuFilter
// 
/**\class FakeL1SingleMuFilter FakeL1SingleMuFilter.cc WSUDiLeptons/MuonAnalyzer/plugins/FakeL1SingleMuFilter.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Jared Sturdy
//         Created:  Thu, 07 Apr 2016 08:53:14 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

//
// class declaration
//

class FakeL1SingleMuFilter : public edm::stream::EDFilter<> {
 public:
  explicit FakeL1SingleMuFilter(const edm::ParameterSet&);
  ~FakeL1SingleMuFilter();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

 private:
  virtual void beginStream(edm::StreamID) override;
  virtual bool filter(edm::Event&, const edm::EventSetup&) override;
  virtual void endStream() override;

  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------
  edm::EDGetTokenT<std::vector<l1extra::L1MuonParticle> > l1MuonToken_;
      
  edm::InputTag l1MuonSrc_;
      
  std::string l1SingleMuCuts_;

  int debug_;
  bool filter_;
  
  //StringCutObjectSelector<l1extra::L1MuonParticle> muonSelector_;
};

#endif
