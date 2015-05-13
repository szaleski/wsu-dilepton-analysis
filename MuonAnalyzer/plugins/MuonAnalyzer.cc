#include "WSUDiLeptons/MuonAnalyzer/interface/MuonAnalyzer.h"
// -*- C++ -*-
//
// Package:    CosmicStudies/MuonAnalyzer
// Class:      MuonAnalyzer
// 
/**\class MuonAnalyzer MuonAnalyzer.cc CosmicStudies/MuonAnalyzer/plugins/MuonAnalyzer.cc

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
  muonSrc_ = pset.getParameter<edm::InputTag>("muonSrc");
  
  //now do what ever initialization is needed
  h_pt  = fs->make<TH1F>( "pt"  , "p_{t}", 500,  0., 2500. );
  h_eta = fs->make<TH1F>( "eta" , "#eta" , 100, -5., 5.    );
  h_phi = fs->make<TH1F>( "phi" , "#phi" , 100,  0., 6.3   );

  h2_pt1_vs_pt2   = fs->make<TH2F>( "pt1_vs_pt2"   , "p_{t,1} vs. p_{t,2}"   , 500,  0., 2500., 500,  0., 2500.);
  h2_eta1_vs_eta2 = fs->make<TH2F>( "eta1_vs_eta2" , "#eta_{1} vs. #eta_{2}" , 100, -5., 5.   , 100, -5., 5.   );
  h2_phi1_vs_phi2 = fs->make<TH2F>( "phi1_vs_phi2" , "#phi_{1} vs. #phi_{2}" , 100,  0., 6.3  , 100,  0., 6.3  );

}


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
  //edm::Handle<edm::View<reco::Muon> > muonColl;
  edm::Handle<reco::MuonCollection > muonColl;
  ev.getByLabel(muonSrc_, muonColl);
  
  
  for (reco::MuonCollection::const_iterator muon=muonColl->begin(), muonCollEnd=muonColl->end();
       muon!=muonCollEnd; ++muon)
    {
      std::cout << "globalMuon pt [GeV]: " << muon->pt() << 
	"  eta : " << muon->eta() << 
	"  phi : " << muon->phi() << 
	std::endl;
      
      h_pt->Fill(muon->pt());
      h_eta->Fill(muon->eta());
      h_phi->Fill(muon->phi());

      if (muon->combinedMuon().isNonnull()) {
	reco::TrackRef ref = muon->combinedMuon();
	std::cout << "combinedMuon number of hits: " << ref->numberOfValidHits() << std::endl;
      }
      
      if (muon->standAloneMuon().isNonnull()) {
	reco::TrackRef ref = muon->standAloneMuon();
	if (ref->innerOk())
	  std::cout << "standAloneMuon inner x [cm]: " << ref->innerPosition().X()
		    << "  outer x [cm]: "              << ref->outerPosition().X()
		    << "  inner y [cm]: "              << ref->innerPosition().Y()
		    << "  outer y [cm]: "              << ref->outerPosition().Y()
		    << std::endl;
      }
      
      if (muon->track().isNonnull()) {
	reco::TrackRef ref = muon->track();
	if (ref->outerOk())
	  std::cout << "track outer pt [GeV]: " << ref->outerPt();
	if (ref->innerOk())
	  std::cout << "  inner pt [GeV]: "     << ref->innerMomentum().Rho();
	std::cout << std::endl;
      }
    }
}


// ------------ method called once each job just before starting event loop  ------------
void MuonAnalyzer::beginJob()
{
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
void MuonAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonAnalyzer);
