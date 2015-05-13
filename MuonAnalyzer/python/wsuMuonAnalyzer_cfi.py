import FWCore.ParameterSet.Config as cms

muonAnalysis = cms.EDAnalyzer('MuonAnalyzer',
    muonSrc = cms.InputTag("muons1Leg")
#vector<reco::Muon>                    "lhcSTAMuons"               ""                "RECO"    
#vector<reco::Muon>                    "muons"                     ""                "RECO"    
#vector<reco::Muon>                    "muons1Leg"                 ""                "RECO"    
#vector<reco::Muon>                    "muonsBeamHaloEndCapsOnly"   ""                "RECO"    
#vector<reco::Muon>                    "muonsNoRPC"                ""                "RECO"    
#vector<reco::Muon>                    "muonsWitht0Correction"     ""                "RECO"    
#vector<reco::Muon>                    "splitMuons"                ""                "RECO"    

)
