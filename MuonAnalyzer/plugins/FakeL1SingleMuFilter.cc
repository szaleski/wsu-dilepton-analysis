// -*- C++ -*-
//
// Package:    WSUDiLeptons/FakeL1SingleMuFilter
// Class:      FakeL1SingleMuFilter
// 
/**\class FakeL1SingleMuFilter FakeL1SingleMuFilter.cc WSUDiLeptons/FakeL1SingleMuFilter/plugins/FakeL1SingleMuFilter.cc

 Description: Using l1extra::L1MuonParticles, emulate the decision of the L1SingleMuOpen trigger

 Implementation:
     The configuration parameters are defined in  the module configuration file wsuFakeL1SingleMuFilter_cfi.py 
     l1MuonSrc_      : l1MuonSrc       = cms.InputTag("l1extraParticles","","RECO") # l1extra::L1MuonParticle collection name
     l1SingleMuCuts_ : l1SingleMuCuts  = cms.string('-2.88 < phi < -0.26 && gmtMuonCand.isFwd==0') #cut to apply on the particles
     debug_          : debug           = cms.int32(0)   # debug level
     filter_         : filterEvent     = cms.bool(True) # whether or not to filter the event, or just to store the result
*/
//
// Original Author:  Jared Sturdy
//         Created:  Thu, 07 Apr 2016 08:53:14 GMT
//
//


#include "WSUDiLeptons/MuonAnalyzer/interface/FakeL1SingleMuFilter.h"

FakeL1SingleMuFilter::FakeL1SingleMuFilter(const edm::ParameterSet& pset)
{
  l1MuonSrc_      = pset.getParameter<edm::InputTag>("l1MuonSrc");
  l1MuonToken_    = consumes<std::vector<l1extra::L1MuonParticle> >(l1MuonSrc_);
  l1SingleMuCuts_ = pset.getParameter<std::string>("l1SingleMuCuts");

  debug_  = pset.getParameter<int>("debug");
  filter_ = pset.getParameter<bool>("filterEvent");

  //now do what ever initialization is needed
  produces<bool>();
}


FakeL1SingleMuFilter::~FakeL1SingleMuFilter()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
FakeL1SingleMuFilter::filter(edm::Event& ev, const edm::EventSetup& es)
{
  edm::Handle<std::vector<l1extra::L1MuonParticle> > l1MuonColl;
  ev.getByToken(l1MuonToken_,      l1MuonColl);      
  
  bool result = false;
  StringCutObjectSelector<l1extra::L1MuonParticle> select(l1SingleMuCuts_);
  for (auto l1mu = l1MuonColl->begin(); l1mu != l1MuonColl->end(); ++ l1mu)
    if (select(*l1mu)) {
      result = true;
      std::auto_ptr<bool> pOut(new bool(result));
      ev.put( pOut);
      
      if (filter_)
	return result;
      else
	return true;
    }
  
  std::auto_ptr<bool> pOut(new bool(result));
  ev.put( pOut);
  
  if (filter_)
    return result;
  else
    return true;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
FakeL1SingleMuFilter::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
FakeL1SingleMuFilter::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
FakeL1SingleMuFilter::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
FakeL1SingleMuFilter::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
FakeL1SingleMuFilter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
FakeL1SingleMuFilter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
FakeL1SingleMuFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(FakeL1SingleMuFilter);
