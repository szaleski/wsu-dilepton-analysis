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


muonSPFilter = cms.EDFilter("MuonCountFilter",
    src = cms.InputTag("betterSPMuons"),
    minNumber = cms.uint32(1)
)

globalMuonSPFilter = cms.EDFilter("MuonCountFilter",
    src = cms.InputTag("globalSPMuons"),
    minNumber = cms.uint32(2)
)

# EDM Output definition
COSMICoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RECO'),
        filterName = cms.untracked.string('muonanalysis')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('Cosmics_deco_p100_CosmicSP_ReReco_outer_new_changed.root'),
    outputCommands = cms.untracked.vstring('drop *',
                                           'keep *_muons_*_*',
                                           'keep *_betterMuons_*_*',
                                           'keep *_globalMuons_*_*',
                                           'keep *_betterSPMuons_*_*',
                                           'keep *_globalSPMuons_*_*',
                                           'keep *_lowerMuons_*_*',
                                           'keep *_lowerGlobalMuons_*_*',
                                           'keep *_upperMuons_*_*',
                                           'keep *_upperGlobalMuons_*_*',
                                           "keep *_tevMuons_*_*",
                                           "keep *_beamhaloTracks_*_*",
                                           "keep *_ckfInOutTracksFromConversions_*_*",
                                           "keep *_ckfOutInTracksFromConversions_*_*",
                                           "keep *_cosmicMuons_*_*",
                                           "keep *_cosmicMuons1Leg_*_*",
                                           "keep *_cosmicMuonsEndCapsOnly_*_*",
                                           "keep *_cosmicMuonsNoRPC_*_*",
                                           "keep *_cosmicMuonsWitht0Correction_*_*",
                                           "keep *_cosmictrackfinderP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5LHCNavigation_*_*",
                                           "keep *_globalBeamHaloMuonEndCapslOnly_*_*",
                                           "keep *_globalCosmicMuons_*_*",
                                           "keep *_globalCosmicMuons1Leg_*_*",
                                           "keep *_globalCosmicMuonsNoRPC_*_*",
                                           "keep *_globalCosmicMuonsWitht0Correction_*_*",
                                           "keep *_globalCosmicSplitMuons_*_*",
                                           "keep *_regionalCosmicTracks_*_*",
                                           "keep *_splittedTracksP5_*_*",
                                           "keep *_standAloneMuons_*_*",
                                           "keep *_beamhaloTracks_*_*",
                                           "keep *_ckfInOutTracksFromConversions_*_*",
                                           "keep *_ckfOutInTracksFromConversions_*_*",
                                           "keep *_cosmicMuons_*_*",
                                           "keep *_cosmicMuons1Leg_*_*",
                                           "keep *_cosmicMuonsEndCapsOnly_*_*",
                                           "keep *_cosmicMuonsNoRPC_*_*",
                                           "keep *_cosmicMuonsWitht0Correction_*_*",
                                           "keep *_cosmictrackfinderP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5_*_*",
    ),
    splitLevel = cms.untracked.int32(0)
)

