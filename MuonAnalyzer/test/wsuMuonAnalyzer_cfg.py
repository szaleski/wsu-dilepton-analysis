import FWCore.ParameterSet.Config as cms

process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Av1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Bv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Cv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Dv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2015v1RECO.root'
        #'/store/data/Commissioning2015/Cosmics/RECO/PromptReco-v1/000/232/928/00000/94B73492-63A8-E411-A07B-02163E01054C.root'
        #'/store/data/Commissioning2015/Cosmics/AOD/PromptReco-v1/000/232/928/00000/E64DC997-63A8-E411-BB15-02163E0124F8.root',
                                    'file:///afs/cern.ch/work/s/szaleski/private/CMSSW_7_4_12/src/WSUCosmicAnalysis/MuonAnalyzer/test/crab_projects/crab_MuonAnalysis_Oct22_New_2015/results/CosmicMuonAnalysis_2015_4.root '
    )
)
from WSUCosmicAnalysis.MuonAnalyzer.wsuMuonAnalyzer_cfi import muonAnalysis

process.analysis1Leg = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons1Leg")
    )
process.analysisLHC = muonAnalysis.clone(
    muonSrc = cms.InputTag("lhcSTAMuons")
)
process.analysisSplit = muonAnalysis.clone(
    muonSrc = cms.InputTag("splitMuons")
)
process.analysisTrackerMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(1),
    debug = cms.int32(0)
)
process.analysisTPFMSMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(2),
    debug = cms.int32(0)
)
process.analysisDYTMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(3),
    debug = cms.int32(0)
)
process.analysisPickyMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(4),
    debug = cms.int32(0)
)
process.analysisTunePMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(5),
    debug = cms.int32(0)
)
process.analysisT0Corr = muonAnalysis.clone(
    muonSrc = cms.InputTag("muonsWitht0Correction")
)
process.analysisBHECOnly = muonAnalysis.clone(
    muonSrc = cms.InputTag("muonsBeamHaloEndCapsOnly")
)
process.analysisNoRPC = muonAnalysis.clone(
    muonSrc = cms.InputTag("muonsNoRPC")
)


process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonAnalysis_2015.root')
)
process.p = cms.Path(
    #process.analysis1Leg
    #+process.analysisLHC
    #+process.analysisSplit
    process.analysisTrackerMuons
    +process.analysisTPFMSMuons
    +process.analysisDYTMuons
    +process.analysisPickyMuons
    +process.analysisTunePMuons
    #+process.analysisT0Corr
    #+process.analysisBHECOnly
    #+process.analysisNoRPC
    )

################################################################
###algoType determines the track type that you are interested in
###1 = Tracker Only
###2 = Tracker Plus First Muon Station
###3 = DYT
###4 = Picky
###5 = TuneP
################################################################

###############################################################
###debug determines the verbosity for debugging mode
###0 = "off"
###1 = minor problems, check for new problems for fresh code
###2 = moderate problems, check new plus affected old code
###3 = nothing works, check everything
###############################################################
