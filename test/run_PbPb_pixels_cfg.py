import FWCore.ParameterSet.Config as cms

process = cms.Process('TRACKANA')
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
#process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('TrackingCode.HIRun2015Ana.HITrackCorrectionAnalyzer_cfi')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('trk.root')
)

process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")

process.tpRecoAssocGeneralTracks = process.trackingParticleRecoTrackAsssociation.clone()
process.tpRecoAssocGeneralTracks.label_tr = cms.InputTag("hiConformalPixelTracks")

process.tpRecoAssocGeneralTracks.label_trSR = cms.InputTag("hiConformalPixelTracks")
process.tpRecoAssocGeneralTracks.label_trRS = cms.InputTag("hiConformalPixelTracks")
#process.tpRecoAssocGeneralTracks.label_trSR = cms.InputTag("generalTracks")
#process.tpRecoAssocGeneralTracks.label_trRS = cms.InputTag("generalTracks")

process.load("SimTracker.TrackAssociatorProducers.quickTrackAssociatorByHits_cfi")
process.quickTrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')

#process.load("SimTracker.TrackerHitAssociation.clusterTpAssociationProducer_cfi")

# Input source
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames =  cms.untracked.vstring(
   #'file:HydjetMB_10_3_1_patch2_UpdatedSmearing_crab_20181217_012503_181217_002602_0000_step3_42.root'
   '/store/group/phys_heavyions/abaty/UCC_Efficiency/step2_3/MinBias_Hydjet_Drum5F_2018_5p02TeV/crab_Step23_UCCFiltered_May9MB/220509_193040/0000/step3_1.root'
),

#    eventsToProcess = cms.untracked.VEventRange('1:6652:352538')

)
### centrality ###
#process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi") 
#process.centralityBin.Centrality = cms.InputTag("hiCentrality")
#process.centralityBin.centralityVariable = cms.string("HFtowers")
#process.centralityBin.nonDefaultGlauberModel = cms.string("HydjetDrum5")

### Track cuts ###
# input collections
process.HITrackCorrections.centralitySrc = cms.InputTag("centralityBin","HFtowers")
process.HITrackCorrections.trackSrc1 = cms.InputTag("generalTracks")
process.HITrackCorrections.trackSrc = cms.InputTag("hiConformalPixelTracks")
process.HITrackCorrections.qualityString = cms.string("highPurity")
process.HITrackCorrections.pfCandSrc = cms.untracked.InputTag("particleFlowTmp")
process.HITrackCorrections.jetSrc = cms.InputTag("akPu4CaloJets")
# options
process.HITrackCorrections.useCentrality = True
process.HITrackCorrections.applyTrackCuts = True
process.HITrackCorrections.fillNTuples = False
process.HITrackCorrections.applyVertexZCut = True
process.HITrackCorrections.doVtxReweighting = False
process.HITrackCorrections.doCaloMatched = False
process.HITrackCorrections.doMergeCount = False
#process.HITrackCorrections.pCharge = True
#process.HITrackCorrections.nCharge = True
# cut values
process.HITrackCorrections.dxyErrMax = 2.0
process.HITrackCorrections.dzErrMax = 2.0
process.HITrackCorrections.ptErrMax = 0.05
process.HITrackCorrections.nhitsMin = 11 
process.HITrackCorrections.chi2nMax =0.15 
process.HITrackCorrections.dxyErrMaxPixel = 7.0 
process.HITrackCorrections.dzErrMaxPixel = 20.0
process.HITrackCorrections.chi2nMaxPixel = 999.0
process.HITrackCorrections.ptMerge = 1. 
process.HITrackCorrections.reso = 0.2
#process.HITrackCorrections.crossSection = 1.0 #1.0 is no reweigh
#algo 
process.HITrackCorrections.algoParameters = cms.vint32(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15)
# vertex reweight parameters
process.HITrackCorrections.vtxWeightParameters = cms.vdouble(0.0306789, 0.427748, 5.16555, 0.0228019, -0.02049, 7.01258 )
###
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2018_realistic', '')
#process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
   cms.PSet(record = cms.string("HeavyIonRcd"),
      tag = cms.string("CentralityTable_HFtowers200_HydjetDrum5Ev8_v1030pre5x02_mc"),
      connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
      label = cms.untracked.string("HFtowers")
   ),
])

###
process.p = cms.Path(
                      process.tpClusterProducer *
                      process.quickTrackAssociatorByHits *
                      process.tpRecoAssocGeneralTracks *
                      process.centralityBin *
                      process.HITrackCorrections
)
