#include "WSUDiLeptons/MuonAnalyzer/interface/MuonPtScaling.h"

// -*- C++ -*-
//
// Package:    WSUDiLeptons/MuonAnalyzer
// Class:      MuonPtScaling
// 
/**\class MuonPtScaling MuonPtScaling.cc WSUDiLeptons/MuonAnalyzer/plugins/MuonPtScaling.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Jared Sturdy
//         Created:  Wed, 25 Feb 2015 12:55:49 GMT
//
//




MuonPtScaling::MuonPtScaling(const edm::ParameterSet& pset)
{
  muonSrc_   = pset.getParameter<edm::InputTag>("muonSrc");
  muonToken_ = consumes<reco::MuonCollection>(muonSrc_);
}


MuonPtScaling::~MuonPtScaling()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void MuonPtScaling::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  using namespace ROOT::Math;
  edm::Handle<reco::MuonCollection > muonColl;
  ev.getByToken(muonToken_,     muonColl);

  if ( muonColl->size() > 0) {
    leadingMuonPt->Fill((*muonColl)[0].pt());
    
    int muIdx = 0;
    for (auto mu = muonColl->begin(); mu != muonColl->end(); ++ mu) {
      allMuonPt->Fill(mu->pt());
      ++muIdx;
    } // end loop over muons
  } // end check on muon collection size
  
}


// ------------ method called once each job just before starting event loop  ------------
void MuonPtScaling::beginJob()
{
  edm::Service< TFileService > fs;
  allMuonPt     = fs->make<TH1D>( "allMuonPt",     "#mu p_{T}",         300, 0., 3000. );
  leadingMuonPt = fs->make<TH1D>( "leadingMuonPt", "leading #mu p_{T}", 300, 0., 3000. );
}



// ------------ method called once each job just after ending the event loop  ------------
void MuonPtScaling::endJob() 
{
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void MuonPtScaling::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(MuonPtScaling);
