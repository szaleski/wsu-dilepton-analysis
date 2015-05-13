import FWCore.ParameterSet.Config as cms

process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        #'file:///afs/cern.ch/user/s/sturdy/work/WSUAnalysis/DiLeptons/CMSSW_7_4_0/src/CosmicSP_2012Av1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/WSUAnalysis/DiLeptons/CMSSW_7_4_0/src/CosmicSP_2012Bv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/WSUAnalysis/DiLeptons/CMSSW_7_4_0/src/CosmicSP_2012Cv1RECO.root'
        'file:///afs/cern.ch/user/s/sturdy/work/WSUAnalysis/DiLeptons/CMSSW_7_4_0/src/CosmicSP_2012Dv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/WSUAnalysis/DiLeptons/CMSSW_7_4_0/src/CosmicSP_2015v1RECO.root'
        #'/store/data/Commissioning2015/Cosmics/RECO/PromptReco-v1/000/232/928/00000/94B73492-63A8-E411-A07B-02163E01054C.root'
        #'/store/data/Commissioning2015/Cosmics/AOD/PromptReco-v1/000/232/928/00000/E64DC997-63A8-E411-BB15-02163E0124F8.root'
    )
)
from WSUDiLeptons.MuonAnalyzer.wsuMuonAnalyzer_cfi import muonAnalysis

process.analysis1Leg = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons1Leg")
    )
process.analysisLHC = muonAnalysis.clone(
    muonSrc = cms.InputTag("lhcSTAMuons")
)
process.analysisSplit = muonAnalysis.clone(
    muonSrc = cms.InputTag("splitMuons")
)
process.analysisMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons")
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
    fileName = cms.string('CosmicMuonAnalysis_2012D.root')
)
process.p = cms.Path(
    process.analysis1Leg
    +process.analysisLHC
    +process.analysisSplit
    +process.analysisMuons
    +process.analysisT0Corr
    +process.analysisBHECOnly
    +process.analysisNoRPC
    )
