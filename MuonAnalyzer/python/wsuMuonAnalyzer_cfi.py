import FWCore.ParameterSet.Config as cms

muonAnalysis = cms.EDAnalyzer('MuonAnalyzer',
    muonSrc = cms.InputTag("muons1Leg"),
                              algoType = cms.int32(1), #Different values for track type can be found in the config file.
                              debug = cms.int32(0) #debugging uses integer for verbosity of mode 0 = debug mode "off"
#vector<reco::Muon>                    "lhcSTAMuons"               ""                "RECO"    
#vector<reco::Muon>                    "muons"                     ""                "RECO"    
#vector<reco::Muon>                    "muons1Leg"                 ""                "RECO"    
#vector<reco::Muon>                    "muonsBeamHaloEndCapsOnly"   ""                "RECO"    
#vector<reco::Muon>                    "muonsNoRPC"                ""                "RECO"    
#vector<reco::Muon>                    "muonsWitht0Correction"     ""                "RECO"    
#vector<reco::Muon>                    "splitMuons"                ""                "RECO"    

)
