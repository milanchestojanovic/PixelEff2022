import FWCore.ParameterSet.Config as cms


HITrackCorrections = cms.EDAnalyzer('HITrackCorrectionAnalyzer',
    trackSrc = cms.InputTag("hiGeneralTracks"),
    vertexSrc = cms.InputTag("offlinePrimaryVertices"),
#    vertexSrc = cms.InputTag("hiSelectedVertex"),
    pfCandSrc = cms.untracked.InputTag("particleFlowTmp"),
    jetSrc = cms.InputTag("akPu4CaloJets"),
    tpEffSrc = cms.InputTag('mix','MergedTrackTruth'),
    tpFakSrc = cms.InputTag('mix','MergedTrackTruth'),
    associatorMap = cms.InputTag('tpRecoAssocGeneralTracks'),
    ptBins = cms.vdouble(
         0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
        0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 
        1.0, 1.05,  1.1, 1.15, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8,
	1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.8, 3.2, 3.6,4.0, 4.4,
	4.8, 5.2, 5.6, 6.0, 6.4, 6.8, 7.2, 8.4, 9.6, 10.8, 12.0, 13.2, 14.4,
	16.8 ,19.2 ,21.6 ,24 ,26.4 ,28.8 ,32 ,35.2 ,38.4 ,41.6 ,44.8 ,48 ,54.4 ,60.8 ,67.2 ,73.6 ,80 ,100 ,200, 300.0, 500.0

    ),
#    ptBins = cms.vdouble(
#         0.3, 0.4, 0.5, 0.6, 0.8, 1.0, 1.25, 1.5, 2.0, 
#        2.5, 3.0, 3.6,4.0, 4.4,
#        4.8, 5.2, 5.6, 6.0, 6.4, 6.8, 7.2, 8.4, 9.6, 10.8, 12.0, 13.2, 14.4,
#        16.8 ,19.2 ,21.6 ,24 ,26.4 ,28.8 ,32 ,35.2 ,38.4 ,41.6 ,44.8 ,48 ,54.4 ,60.8 ,67.2 ,73.6 ,80 ,100 ,200, 300.0, 500.0
#
#    ),
    etaBins = cms.vdouble( 
        -3.0,-2.8,-2.6,-2.4,-2.2,-2.0,-1.8,-1.6,-1.4,-1.2,-1.0,
	-0.8,-0.6,-0.4,-0.2,0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,
	1.8,2.0,2.2,2.4,2.6,2.8,3.0
    ),
    occBins = cms.vdouble(0.0, 10.0, 20, 60, 100, 200),
    vtxWeightParameters = cms.vdouble( 4.49636e-02, 1.36629e-01, 5.30010e+00,
                                       2.50170e-02, 4.59123e-01, 9.64888e+00 ),
    algoParameters = cms.vint32(1,2,3,4,5,6,7,8,9,10,11),
    doCaloMatched = cms.bool(True),
    reso = cms.double(2.0),
    crossSection = cms.double(1.0),
    doVtxReweighting = cms.bool(False),
    applyVertexZCut = cms.bool(False),
    vertexZMax = cms.double(15.),
    applyTrackCuts = cms.bool(True),
    qualityString = cms.string("highPurity"),
    dxyErrMax = cms.double(3.0),
    dzErrMax = cms.double(3.0),
    ptErrMax = cms.double(0.3),
    nhitsMin = cms.int32(0),
    chi2nMax = cms.double(9999.0),
    dxyErrMaxPixel = cms.double(7.0),
    dzErrMaxPixel = cms.double(20.0),
    chi2nMaxPixel = cms.double(9999.0),
    ptErrMaxPixel = cms.double(9999),
    pCharge = cms.bool(False),
    nCharge = cms.bool(False),
    ptMerge = cms.double(1.0),
    doMergeCount = cms.bool(False),
    doMomRes = cms.bool(False),
    fillNTuples = cms.bool(False),
    useCentrality = cms.bool(False),
    centralitySrc = cms.InputTag("centralityBin","HFTowers")
)
