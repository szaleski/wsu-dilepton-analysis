import FWCore.ParameterSet.Config as cms



basic_cut  = "pt > 40. && isTrackerMuon"
# restrict collections to muons near the pixel
dxy_cut = "(abs(tunePMuonBestTrack.dxy) < 50.)"
dz_cut  = " && (abs(tunePMuonBestTrack.dz)  < 75.)"

# upper: outer position > 0 if standalone track outerTrack.isNonnull ? outerPosition.Y > 0
# upper: inner position > 0
# upper: abs(inner position) > abs(outer position)
upper_cut = "? outerTrack.isNonnull ? (outerTrack.outerPosition.Y > 0) : (abs(innerTrack.innerPosition.Y) > abs(innerTrack.outerPosition.Y))"
lower_cut = "? outerTrack.isNonnull ? (outerTrack.outerPosition.Y < 0) : (abs(innerTrack.innerPosition.Y) < abs(innerTrack.outerPosition.Y))"
upper_cut_ = "? (outerTrack.isNonnull) ? (outerTrack.outerPosition.Y > 0) : ((abs(innerTrack.innerPosition.Y) > abs(innerTrack.outerPosition.Y)))"
lower_cut_ = "? (outerTrack.isNonnull) ? (outerTrack.outerPosition.Y < 0) : ((abs(innerTrack.innerPosition.Y) < abs(innerTrack.outerPosition.Y)))"
upper_cut_2 = "(outerTrack.isNonnull && (outerTrack.outerPosition.Y > 0)) || (!(outerTrack.isNonnull) &&(abs(innerTrack.innerPosition.Y) > abs(innerTrack.outerPosition.Y)))"
lower_cut_2 = "(outerTrack.isNonnull && (outerTrack.outerPosition.Y < 0)) || (!(outerTrack.isNonnull) &&(abs(innerTrack.innerPosition.Y) < abs(innerTrack.outerPosition.Y)))"

betterMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("muons"),
    cut = cms.string(basic_cut),
)

zprimeMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("muons"),
    cut = cms.string("pt > 45. && abs(eta) < 0.9 && abs(tunePMuonBestTrack.dxy) < 10. && abs(tunePMuonBestTrack.dz) < 50."),
    # no need to have isTrackerMuon here as we can save it and put it in the denominator selection anyway
)
zprimeLowerMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("zprimeMuons"),
    #cut = cms.string("tunePMuonBestTrack.outerPosition.Y < 0"),
    cut = cms.string(lower_cut_2)
    #cut = cms.string("abs(tunePMuonBestTrack.innerPosition.Y) < abs(tunePMuonBestTrack.outerPosition.Y)"),
    #cut = cms.string("((tunePMuonBestTrack.outerPosition.Y < 0) && (time.timeAtIpOutIn > 0)) || ((tunePMuonBestTrack.outerPosition.Y > 0) && (time.timeAtIpOutIn > 0))"),
)

zprimeUpperMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("zprimeMuons"),
    #cut = cms.string("tunePMuonBestTrack.outerPosition.Y > 0"),
    cut = cms.string(upper_cut_2)
    #cut = cms.string("abs(tunePMuonBestTrack.innerPosition.Y) > abs(tunePMuonBestTrack.outerPosition.Y)"),
    #cut = cms.string("((tunePMuonBestTrack.outerPosition.Y > 0) && (time.timeAtIpOutIn < 0)) || ((tunePMuonBestTrack.outerPosition.Y < 0) && (time.timeAtIpOutIn < 0))"),
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
    cut = cms.string("isGlobalMuon && (innerTrack.hitPattern.numberOfValidPixelHits > 0)"),
)

# how do we ensure upper/lower for cosmic reconstruction
#  - Y position: sometimes for whatever reason this isn't reliable
#  - time information: also reliability issues
upperMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterSPMuons"),
    #cut = cms.string("tunePMuonBestTrack.outerPosition.Y > 0"),
    cut = cms.string(upper_cut_2)
    #cut = cms.string("abs(tunePMuonBestTrack.innerPosition.Y) > abs(tunePMuonBestTrack.outerPosition.Y)"),
)

upperGlobalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("globalSPMuons"),
    #cut = cms.string("tunePMuonBestTrack.outerPosition.Y > 0"),
    cut = cms.string(upper_cut_2)
    #cut = cms.string("abs(tunePMuonBestTrack.innerPosition.Y) > abs(tunePMuonBestTrack.outerPosition.Y)"),
)

lowerMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterSPMuons"),
    #cut = cms.string("tunePMuonBestTrack.outerPosition.Y < 0"),
    cut = cms.string(lower_cut_2)
    #cut = cms.string("abs(tunePMuonBestTrack.innerPosition.Y) < abs(tunePMuonBestTrack.outerPosition.Y)"),
)

lowerGlobalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("globalSPMuons"),
    #cut = cms.string("tunePMuonBestTrack.outerPosition.Y < 0"),
    cut = cms.string(lower_cut_2)
    #cut = cms.string("abs(tunePMuonBestTrack.innerPosition.Y) < abs(tunePMuonBestTrack.outerPosition.Y)"),
)


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
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('muonanalysis')),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('Cosmics_deco_p100_CosmicSP_ReReco_outer_new_changed.root'),
    outputCommands = cms.untracked.vstring('drop *',
                                           'keep *_muons_*_*',
                                           'keep *_zprimeMuons_*_*',
                                           'keep *_zprimeUpperMuons_*_*',
                                           'keep *_zprimeLowerMuons_*_*',
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
                                           "keep *_cosmicMuons*_*_*",
                                           "keep *_cosmictrackfinderP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5*_*_*",
                                           "keep *_globalBeamHaloMuonEndCapslOnly_*_*",
                                           "keep *_globalCosmic*_*_*",
                                           "keep *_regionalCosmicTracks_*_*",
                                           "keep *_splittedTracksP5_*_*",
                                           "keep *_standAloneMuons_*_*",
                                           "keep *_beamhaloTracks_*_*",
    ),
    splitLevel = cms.untracked.int32(0)
)

