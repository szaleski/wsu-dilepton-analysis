import FWCore.ParameterSet.Config as cms

basic_cut  = "pt > 45"
# restrict collections to muons near the pixel
dxy_cut = "(abs(muonBestTrack.dxy) < 50.)"
dz_cut  = " && (abs(muonBestTrack.dz)  < 75.)"

betterMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("muons"),
    cut = cms.string(basic_cut),
)

globalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string("isGlobalMuon"),
)

betterSPMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string(dxy_cut+dz_cut),
)

globalSPMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterSPMuons"),
    cut = cms.string("isGlobalMuon"),
)

# how do we ensure upper/lower for cosmic reconstruction
#  - Y position: sometimes for whatever reason this isn't reliable
#  - time information: also reliability issues
upperMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterSPMuons"),
    cut = cms.string("muonBestTrack.outerPosition.Y > 0"),
    #cut = cms.string("((muonBestTrack.outerPosition.Y > 0) && (time.timeAtIpOutIn < 0)) || ((muonBestTrack.outerPosition.Y < 0) && (time.timeAtIpOutIn < 0))"),
)

upperGlobalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("globalSPMuons"),
    cut = cms.string("muonBestTrack.outerPosition.Y > 0"),
    #cut = cms.string("((muonBestTrack.outerPosition.Y > 0) && (time.timeAtIpOutIn < 0)) || ((muonBestTrack.outerPosition.Y < 0) && (time.timeAtIpOutIn < 0))"),
)

lowerMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterSPMuons"),
    cut = cms.string("muonBestTrack.outerPosition.Y < 0"),
    #cut = cms.string("((muonBestTrack.outerPosition.Y < 0) && (time.timeAtIpOutIn > 0)) || ((muonBestTrack.outerPosition.Y > 0) && (time.timeAtIpOutIn > 0))"),
)

lowerGlobalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("globalSPMuons"),
    cut = cms.string("muonBestTrack.outerPosition.Y < 0"),
    #cut = cms.string("((muonBestTrack.outerPosition.Y < 0) && (time.timeAtIpOutIn > 0)) || ((muonBestTrack.outerPosition.Y > 0) && (time.timeAtIpOutIn > 0))"),
)


#upperGlobalMuons = cms.EDFilter("MuonSelector",
#    src = cms.InputTag("upperMuons"),
#    cut = cms.string("isGlobalMuon"),
#)
#
#lowerGlobalMuons = cms.EDFilter("MuonSelector",
#    src = cms.InputTag("lowerMuons"),
#    cut = cms.string("isGlobalMuon"),
#)


