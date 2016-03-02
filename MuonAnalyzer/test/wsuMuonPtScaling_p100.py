import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonPtScaling")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        mcfilespt100asym
        #dyfiles
    )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")

from WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi import COSMICTrackoutput
process.COSMICoutput.outputCommands.append(COSMICTrackoutput)

from WSUDiLeptons.MuonAnalyzer.wsuMuonPtScaling_cfi import *

## for comparing with standard collision data/MC
# can't get y position from AOD/AODSIM, lives in TrackExtra not stored in AOD
#process.cosmicMuonTracks.src  = cms.InputTag("standAloneMuons")
#process.globalMuonTracks.src  = cms.InputTag("globalMuons")
#process.trackerMuonTracks.src = cms.InputTag("generalTracks")

process.analysisPtScaling = muonPtScaling.clone(
    muonSrc     = cms.InputTag("muons"),
)
process.analysisPtScaling1Leg = muonPtScaling.clone(
    muonSrc     = cms.InputTag("muons1Leg"),
)
process.analysisPtScalingSplit = muonPtScaling.clone(
    muonSrc     = cms.InputTag("splitMuons"),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonPtScaling_MC_p100asym.root')
)

process.muonanalysis = cms.Path(
    process.analysisPtScaling
    +process.analysisPtScaling1Leg
    +process.analysisPtScalingSplit
)

# Schedule definition
process.schedule = cms.Schedule(
    process.muonanalysis
)
