//
// Original Author:  Eric Appelt
//         Created:  Tue Nov 26, 2013
//
//

#include <memory>
#include <algorithm>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include <TTree.h>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <DataFormats/VertexReco/interface/Vertex.h>
#include <DataFormats/VertexReco/interface/VertexFwd.h>
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/TrackReco/interface/TrackFwd.h>
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

// Particle Flow
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
// RecoJets
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

#include "TrackingCode/HIRun2015Ana/interface/HITrackCorrectionTreeHelper.h"

class HITrackCorrectionAnalyzer : public edm::EDAnalyzer {
   public:
      explicit HITrackCorrectionAnalyzer(const edm::ParameterSet&);
      ~HITrackCorrectionAnalyzer();

      static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );

   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      void initHistos(const edm::Service<TFileService> & fs);
      bool passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex);
      bool passesTrackCutsPixel(const reco::Track & track, const reco::Vertex & vertex);
      bool caloMatched(const reco::Track & track, const edm::Event& iEvent, unsigned it );

      // ----------member data ---------------------------


      std::map<std::string,TTree*> trkTree_;
      std::map<std::string,TH2F*> trkCorr2D_;
      std::map<std::string,TH3F*> trkCorr3D_;
      TH3F * momRes_;
      TH1F * vtxZ_;
      TH1F * pthat_;
      TF1 * vtxWeightFunc_;
      TH3F * signal;
      TH1F * hIndex;



      int centeffbins = 5; int centeffmax[5] = {5, 10, 30, 50, 100}; int centeffmin[5] = {0, 5,  10, 30, 50 }; 
///////

      HITrackCorrectionTreeHelper treeHelper_;

      edm::EDGetTokenT<reco::VertexCollection> vertexSrc_;
      edm::EDGetTokenT<edm::View<reco::Track> > trackSrc_;
      edm::EDGetTokenT<edm::View<reco::Track> > trackSrc1_;
      edm::EDGetTokenT<TrackingParticleCollection> tpFakSrc_;
      edm::EDGetTokenT<TrackingParticleCollection> tpEffSrc_;
      edm::EDGetTokenT<reco::RecoToSimCollection> associatorMapRTS_;
      edm::EDGetTokenT<reco::SimToRecoCollection> associatorMapSTR_;

      edm::InputTag pfCandSrc_;
      edm::InputTag jetSrc_;

      std::vector<double> ptBins_;
      std::vector<double> etaBins_;
      std::vector<double> occBins_;

      bool doCaloMatched_;
      double reso_;
      double crossSection_;
      
      std::vector<double> vtxWeightParameters_;
      std::vector<int> algoParameters_;
      bool doVtxReweighting_;

      bool applyVertexZCut_;
      double vertexZMax_;

      bool applyTrackCuts_;
      std::string qualityString_;
      double dxyErrMax_;
      double dzErrMax_;
      double ptErrMax_;
      int    nhitsMin_;
      double chi2nMax_;      
      bool pCharge_;
      bool nCharge_;
      double dxyErrMaxPixel_;
      double dzErrMaxPixel_;
      double chi2nMaxPixel_;
      double ptErrMaxPixel_;
      double ptMerge_;

      bool doMergeCount_;
      bool doMomRes_;

      bool fillNTuples_;

      bool useCentrality_;
      edm::EDGetTokenT<int> centralitySrc_;

      int npthigh;
};

HITrackCorrectionAnalyzer::HITrackCorrectionAnalyzer(const edm::ParameterSet& iConfig):
treeHelper_(),
vertexSrc_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
trackSrc_(consumes<edm::View<reco::Track> >(iConfig.getParameter<edm::InputTag>("trackSrc"))),
trackSrc1_(consumes<edm::View<reco::Track> >(iConfig.getParameter<edm::InputTag>("trackSrc1"))),
tpFakSrc_(consumes<TrackingParticleCollection>(iConfig.getParameter<edm::InputTag>("tpFakSrc"))),
tpEffSrc_(consumes<TrackingParticleCollection>(iConfig.getParameter<edm::InputTag>("tpEffSrc"))),
associatorMapRTS_(consumes<reco::RecoToSimCollection>(iConfig.getParameter<edm::InputTag>("associatorMap"))),
associatorMapSTR_(consumes<reco::SimToRecoCollection>(iConfig.getParameter<edm::InputTag>("associatorMap"))),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
occBins_(iConfig.getParameter<std::vector<double> >("occBins")),
doCaloMatched_(iConfig.getParameter<bool>("doCaloMatched")),
reso_(iConfig.getParameter<double>("reso")),
crossSection_(iConfig.getParameter<double>("crossSection")),
vtxWeightParameters_(iConfig.getParameter<std::vector<double> >("vtxWeightParameters")),
algoParameters_(iConfig.getParameter<std::vector<int> >("algoParameters")),
doVtxReweighting_(iConfig.getParameter<bool>("doVtxReweighting")),
applyVertexZCut_(iConfig.getParameter<bool>("applyVertexZCut")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
applyTrackCuts_(iConfig.getParameter<bool>("applyTrackCuts")),
qualityString_(iConfig.getParameter<std::string>("qualityString")),
dxyErrMax_(iConfig.getParameter<double>("dxyErrMax")),
dzErrMax_(iConfig.getParameter<double>("dzErrMax")),
ptErrMax_(iConfig.getParameter<double>("ptErrMax")),
nhitsMin_(iConfig.getParameter<int>("nhitsMin")),
chi2nMax_(iConfig.getParameter<double>("chi2nMax")),
pCharge_(iConfig.getParameter<bool>("pCharge")),
nCharge_(iConfig.getParameter<bool>("nCharge")),
dxyErrMaxPixel_(iConfig.getParameter<double>("dxyErrMaxPixel")),
dzErrMaxPixel_(iConfig.getParameter<double>("dzErrMaxPixel")),
chi2nMaxPixel_(iConfig.getParameter<double>("chi2nMaxPixel")),
ptErrMaxPixel_(iConfig.getParameter<double>("ptErrMaxPixel")),
ptMerge_(iConfig.getParameter<double>("ptMerge")),
doMergeCount_(iConfig.getParameter<bool>("doMergeCount")),
doMomRes_(iConfig.getParameter<bool>("doMomRes")),
fillNTuples_(iConfig.getParameter<bool>("fillNTuples")),
useCentrality_(iConfig.getParameter<bool>("useCentrality")),
centralitySrc_(consumes<int>(iConfig.getParameter<edm::InputTag>("centralitySrc")))
{

   pfCandSrc_ = iConfig.getUntrackedParameter<edm::InputTag>("pfCandSrc");
   jetSrc_ = iConfig.getParameter<edm::InputTag>("jetSrc");

   edm::Service<TFileService> fs;
   initHistos(fs);

   vtxWeightFunc_ = new TF1("vtxWeight","gaus(0)/gaus(3)",-50.,50.);
   // vtxWeightParameters should have size 6,
   // one really should throw an error if not
   if( (int)vtxWeightParameters_.size() == 6 )
   {
     for( unsigned int i=0;i<vtxWeightParameters_.size(); i++)
       vtxWeightFunc_->SetParameter(i,vtxWeightParameters_[i]);
   }

   if( fillNTuples_ )
   {
     trkTree_["rec"] = fs->make<TTree>("recTree","recTree");
     trkTree_["rec"]->Branch("recValues",&treeHelper_.b,treeHelper_.hiTrackLeafString.Data());
     trkTree_["sim"] = fs->make<TTree>("simTree","simTree");
     trkTree_["sim"]->Branch("simValues",&treeHelper_.b,treeHelper_.hiTrackLeafString.Data());
   }
  
   npthigh=0;
}

HITrackCorrectionAnalyzer::~HITrackCorrectionAnalyzer()
{
   delete vtxWeightFunc_;
}

void
HITrackCorrectionAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  edm::Handle<reco::PFCandidateCollection> pfCandidates;
  iEvent.getByLabel(pfCandSrc_, pfCandidates);
  if( !pfCandidates.isValid() ) std::cout<< "false callo matching"<<std::endl ;   
//  else std::cout<<"true callo matching"<<std::endl;

   using namespace edm;

   // obtain collections of simulated particles 
   edm::Handle<TrackingParticleCollection>  TPCollectionHeff, TPCollectionHfake;
   iEvent.getByToken(tpEffSrc_,TPCollectionHeff);
   iEvent.getByToken(tpFakSrc_,TPCollectionHfake);

   // obtain association map between tracks and simulated particles
   reco::RecoToSimCollection recSimColl;
   reco::SimToRecoCollection simRecColl;
   edm::Handle<reco::SimToRecoCollection > simtorecoCollectionH;
   edm::Handle<reco::RecoToSimCollection > recotosimCollectionH;
   iEvent.getByToken(associatorMapSTR_,simtorecoCollectionH);
   simRecColl= *(simtorecoCollectionH.product());
   iEvent.getByToken(associatorMapRTS_,recotosimCollectionH);
   recSimColl= *(recotosimCollectionH.product());

  // beam spot
  reco::BeamSpot beamSpot;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByLabel("offlineBeamSpot", beamSpotHandle);

   //calo jets
/*
   Handle<reco::CaloJetCollection> JetCollection;
   iEvent.getByLabel(jetSrc_, JetCollection);
   if( !JetCollection.isValid() ) return; 
   double leadingJet = 0.;
   for(unsigned irecojet = 0; irecojet < JetCollection->size(); irecojet++ ){
      const reco::CaloJet & JetCand = (*JetCollection)[irecojet];
      if( JetCand.pt() > leadingJet ) leadingJet = JetCand.pt();//finding leading pT jets
   }

   pthat_->Fill( leadingJet, crossSection_ );
*/
   // obtain reconstructed tracks
   Handle<edm::View<reco::Track> > tcol, tcol1;
   iEvent.getByToken(trackSrc_, tcol);

     //Handle<edm::View<reco::Track> > tcol1;
   iEvent.getByToken(trackSrc1_, tcol1);


   // obtain primary vertices
   Handle<reco::VertexCollection> vertex;
   iEvent.getByToken(vertexSrc_, vertex);
  
   // sort the vertcies by number of tracks in descending order
   reco::VertexCollection vsorted = *vertex;
   std::sort( vsorted.begin(), vsorted.end(), HITrackCorrectionAnalyzer::vtxSort );

   std::cout<<"ovde radi"<<std::endl;

   // skip events with no PV, this should not happen
   if( vsorted.size() == 0) return;

   // skip events failing vertex cut
   if( applyVertexZCut_)
   {
     if( fabs(vsorted[0].z()) > vertexZMax_ ) return;
   }

   // determine vertex reweighting factor
   double w = 1.0;
   w = w * crossSection_;
   if ( doVtxReweighting_ )
     w *= vtxWeightFunc_->Eval(vsorted[0].z());

   vtxZ_->Fill(vsorted[0].z(),w);

   // determine event multipliticy
   int multiplicity =0;
   for(edm::View<reco::Track>::size_type i=0; i<tcol->size(); ++i){
     edm::RefToBase<reco::Track> track(tcol, i);
     reco::Track* tr=const_cast<reco::Track*>(track.get());
     if( passesTrackCuts(*tr, vsorted[0]) ) 
       multiplicity++;
   }

   // determine centrality if set
   // note if there is no centrality information multiplicity 
   // will be used in place of the centrality
   int cbin = multiplicity;
   int occ = 1;
   if( useCentrality_ )
   {
     edm::Handle<int> centralityBin;
     iEvent.getByToken(centralitySrc_, centralityBin);
     cbin = *centralityBin;
     occ = cbin;
   } 

   int ntrack = 0, ntrackgt = 0, ntrackpt = 0;


  ptMerge_ = 0.6;
  if (occ < 20) ptMerge_ = 1.0;
  double cutWidth_ = 0.2;
  double trkRes_ = 0.02;

   // ---------------------
   // loop through reco tracks to fill fake, reco, and secondary histograms
   // ---------------------

   for(edm::View<reco::Track>::size_type i=0; i<tcol->size(); ++i){ 
     edm::RefToBase<reco::Track> track(tcol, i);
     reco::Track* tr=const_cast<reco::Track*>(track.get());

     if ( tr->charge() == 0  ) continue;

     if (fabs(tr->eta()) > 2.4 ) continue;
 


     // skip tracks that fail cuts, using vertex with most tracks as PV       
     //if( ! passesTrackCuts(*tr, vsorted[0]) ) continue;
     if( ! passesTrackCutsPixel(*tr, vsorted[0]) ) continue;
     if( ! caloMatched(*tr, iEvent, i) ) continue;
     if (fabs(tr->eta()) > 2.4 ) continue;


     ntrack++;
   
     ntrackpt++; 

     if(doMergeCount_ && (tr->pt() > (ptMerge_ - cutWidth_) )) {
	int nrecG=0;
	for(edm::View<reco::Track>::size_type ii=0; ii<tcol1->size(); ++ii){
             edm::RefToBase<reco::Track> trackgt(tcol1, ii);
             reco::Track* trgt=const_cast<reco::Track*>(trackgt.get());
	     if ( ( passesTrackCuts(*trgt, vsorted[0]) ) &&   fabs(tr->eta()- trgt->eta()) < trkRes_ && fabs(tr->phi()- trgt->phi()) < trkRes_) 	
		nrecG++;
	}
     if (nrecG>0) continue;
     }
     if (tr->numberOfValidHits() > 6 ) ntrackgt++;

     trkCorr2D_["hrec"]->Fill(tr->eta(), tr->pt(), w);
     trkCorr3D_["hrec3D"]->Fill(tr->eta(), tr->pt(), occ, w);

     // look for match to simulated particle, use first match if it exists
     std::vector<std::pair<TrackingParticleRef, double> > tp;
     const TrackingParticle *mtp=0;
     if(recSimColl.find(track) != recSimColl.end())      {
     
       tp = recSimColl[track];
       mtp = tp.begin()->first.get();  
       if( fillNTuples_) treeHelper_.Set(*mtp, *tr, vsorted[0], tp.size(), cbin); 
/*
       size_t nrecM=0;
// merged collection duplicate
       if (doMergeCount_) {
         std::vector<std::pair<edm::RefToBase<reco::Track>, double> > rtM;
         if(simRecColl.find(tp.begin()->first) != simRecColl.end())
         {
            rtM = (std::vector<std::pair<edm::RefToBase<reco::Track>, double> >) simRecColl[tp.begin()->first];
            std::vector<std::pair<edm::RefToBase<reco::Track>, double> >::const_iterator rtit;
            for (rtit = rtM.begin(); rtit != rtM.end(); ++rtit)
            {
               const reco::Track* tmtr = rtit->first.get();
      
		
               if( ! passesTrackCuts(*tmtr, vsorted[0]) ) continue;
//               if( ! passesTrackCutsPixel(*tmtr, vsorted[0]) ) continue;
        
               nrecM++;
               trkCorr2D_["hmul"]->Fill(tr->eta(),tr->pt(), w);
               trkCorr3D_["hmul3D"]->Fill(tr->eta(),tr->pt(), occ, w);
               trkCorr2D_["hmul"]->Fill(tmtr->eta(),tmtr->pt(), w);
               trkCorr3D_["hmul3D"]->Fill(tmtr->eta(),tmtr->pt(), occ, w);
           }
       }

     }
// merged collection duplicate ends
*/
       if( mtp->status() < 0 ) 
       {
         trkCorr2D_["hsec"]->Fill(tr->eta(), tr->pt(), w);     
         trkCorr3D_["hsec3D"]->Fill(tr->eta(), tr->pt(), occ, w);     
       }
     }
     else
     {
       if( fillNTuples_) treeHelper_.Set(*tr, vsorted[0], cbin); 
       trkCorr2D_["hfak"]->Fill(tr->eta(), tr->pt(), w);
       trkCorr3D_["hfak3D"]->Fill(tr->eta(), tr->pt(), occ, w);

     }
     if( fillNTuples_) trkTree_["rec"]->Fill(); 
   }




   // ---------------------
   // loop through sim particles to fill matched, multiple,  and sim histograms 
   // ---------------------
   for(TrackingParticleCollection::size_type i=0; i<TPCollectionHeff->size(); i++) 
   {      
     TrackingParticleRef tpr(TPCollectionHeff, i);
     TrackingParticle* tp=const_cast<TrackingParticle*>(tpr.get());
      
     if(tp->status() < 0 || tp->charge()==0) continue; //only charged primaries

     if ((pCharge_) && (tp->charge() < 0) ) continue;
     if ((nCharge_) && (tp->charge() > 0) ) continue;


     trkCorr2D_["hsim"]->Fill(tp->eta(),tp->pt(), w);
     trkCorr3D_["hsim3D"]->Fill(tp->eta(),tp->pt(), occ, w);

     //if ( tp->pt()<0.4 && tp->pt() > 0.3 ) {
           //i_sim++;
     //}

     // find number of matched reco tracks that pass cuts
     std::vector<std::pair<edm::RefToBase<reco::Track>, double> > rt;
     size_t nrec=0;
     if(simRecColl.find(tpr) != simRecColl.end())
     {
       rt = (std::vector<std::pair<edm::RefToBase<reco::Track>, double> >) simRecColl[tpr];
       std::vector<std::pair<edm::RefToBase<reco::Track>, double> >::const_iterator rtit;
       for (rtit = rt.begin(); rtit != rt.end(); ++rtit)
       {
         const reco::Track* tmtr = rtit->first.get();
   

         //if( ! passesTrackCuts(*tmtr, vsorted[0]) ) continue;
         if( ! passesTrackCutsPixel(*tmtr, vsorted[0]) ) continue;

         unsigned index = -1;
         if( tp->pt()>20 && doCaloMatched_ ){ 
          for(edm::View<reco::Track>::size_type i=0; i<tcol1->size(); ++i){ 
             edm::RefToBase<reco::Track> track(tcol1, i);
             reco::Track* tr=const_cast<reco::Track*>(track.get());
             index++;
             if( tmtr->pt() == tr->pt() && tmtr->eta() == tr->eta() && tmtr->phi() == tr->phi() && tmtr->numberOfValidHits() == tr->numberOfValidHits() ) break;//simple match to find the corresponding index number (i-th track) in the track collection
          }
         if( ! caloMatched(*tmtr, iEvent, index) ) continue;
         }  
         nrec++;
	 if (tp->pt() > 20 ) npthigh++;

         if( doMomRes_ ) momRes_->Fill( tp->eta(), tp->pt(), tmtr->pt(), w);
       }
     }
     if( nrec>0 && fillNTuples_ ) treeHelper_.Set(*tp, *(rt.begin()->first.get()), vsorted[0], rt.size(), cbin);
     if( nrec==0 && fillNTuples_ ) treeHelper_.Set(*tp, cbin);
     if(nrec>0) trkCorr2D_["heff"]->Fill(tp->eta(),tp->pt(), w);
     if(nrec>0) trkCorr3D_["heff3D"]->Fill(tp->eta(),tp->pt(), occ, w);
     if(nrec>1) trkCorr2D_["hmul"]->Fill(tp->eta(),tp->pt(), w);
     if(nrec>1 && (!doMergeCount_) ) trkCorr3D_["hmul3D"]->Fill(tp->eta(),tp->pt(), occ, w);
     if( fillNTuples_) trkTree_["sim"]->Fill(); 

   }

}

bool
HITrackCorrectionAnalyzer::passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex)
{
   if ( ! applyTrackCuts_ ) return true;

   math::XYZPoint vtxPoint(0.0,0.0,0.0);
   double vzErr =0.0, vxErr=0.0, vyErr=0.0;
   vtxPoint=vertex.position();
   vzErr=vertex.zError();
   vxErr=vertex.xError();
   vyErr=vertex.yError();

   double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
   dxy = track.dxy(vtxPoint);
   dz = track.dz(vtxPoint);
   dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
   dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);
   
   double chi2n = track.normalizedChi2();
   double nlayers = track.hitPattern().trackerLayersWithMeasurement();
   chi2n = chi2n/nlayers;
   int nhits = track.numberOfValidHits();
   int algo  = track.originalAlgo();  
   int charge = track.charge();


   if ((pCharge_) && (charge < 0) ) return false;
   if ((nCharge_) && (charge > 0) ) return false;
   if( track.pt() <= ptMerge_ ) return false;
   if(track.quality(reco::TrackBase::qualityByName(qualityString_)) != 1)
       return false;
   if(fabs(track.ptError()) / track.pt() > ptErrMax_) return false;

   if(fabs(dxy/dxysigma) > dxyErrMax_) return false;
   if(fabs(dz/dzsigma) > dzErrMax_) return false;

   if(nhits < nhitsMin_ ) return false;
   int count = 0;
   for(unsigned i = 0; i < algoParameters_.size(); i++){
      if( algo == algoParameters_[i] ) count++;
   }
//   if( count == 0 ) return false;
   if(chi2n > chi2nMax_ ) return false;  
   if(track.pt() <= ptMerge_ ) return false;

   return true;
}

bool
HITrackCorrectionAnalyzer::passesTrackCutsPixel(const reco::Track & track, const reco::Vertex & vertex)
{
   if ( ! applyTrackCuts_ ) return true;
   
   math::XYZPoint vtxPoint(0.0,0.0,0.0);
   double vzErr =0.0, vxErr=0.0, vyErr=0.0;
   vtxPoint=vertex.position();
   vzErr=vertex.zError();
   vxErr=vertex.xError();
   vyErr=vertex.yError();

   double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
   dxy = track.dxy(vtxPoint);
   dz = track.dz(vtxPoint);
   dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
   dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);

   double chi2n = track.chi2();
   double nlayers = track.hitPattern().trackerLayersWithMeasurement();
   chi2n = chi2n/nlayers;
   int nhits = track.numberOfValidHits();
   int algo  = track.originalAlgo();
   int charge = track.charge();
   

   if ((pCharge_) && (charge < 0) ) return false;
   if ((nCharge_) && (charge > 0) ) return false;
   if(fabs(dxy/dxysigma) > dxyErrMaxPixel_) return false;
   if( fabs(dz/dzsigma) > dzErrMaxPixel_) return false;
   if(chi2n > chi2nMaxPixel_ ) return false;
   if(fabs(track.ptError()) / track.pt() > ptErrMaxPixel_) return false;
   if(track.pt() > ptMerge_ ) return false;
   return true;

}

bool 
HITrackCorrectionAnalyzer::caloMatched( const reco::Track & track, const edm::Event& iEvent, unsigned it )
{
  if( ! doCaloMatched_ ) return true;

  // obtain pf candidates
  edm::Handle<reco::PFCandidateCollection> pfCandidates;
  iEvent.getByLabel(pfCandSrc_, pfCandidates);
  if( !pfCandidates.isValid() ) return false;

  double ecalEnergy = 0.;
  double hcalEnergy = 0.;

  for( unsigned ic = 0; ic < pfCandidates->size(); ic++ ) {//calo matching loops

      const reco::PFCandidate& cand = (*pfCandidates)[ic];

      int type = cand.particleId();

      // only charged hadrons and leptons can be asscociated with a track
      if(!(type == reco::PFCandidate::h ||     //type1
      type == reco::PFCandidate::e ||     //type2
      type == reco::PFCandidate::mu      //type3
      )) continue;

      reco::TrackRef trackRef = cand.trackRef();
      if( it == trackRef.key() ) {
        // cand_index = ic;
        ecalEnergy = cand.ecalEnergy();
        hcalEnergy = cand.hcalEnergy();              
        break;
      } 
  }

  //if((track.pt()-reso_*track.ptError())*TMath::CosH( track.eta() )>15 && (track.pt()-reso_*track.ptError())*TMath::CosH( track.eta() ) > hcalEnergy+ecalEnergy ) return false;
  if( track.pt() < 20 || ( (hcalEnergy+ecalEnergy)/( track.pt()*TMath::CosH(track.eta() ) ) > reso_ && (hcalEnergy+ecalEnergy)/(TMath::CosH(track.eta())) > (track.pt() - 80.0) )  ) return true;
  else {
    return false;
  }
}


void
HITrackCorrectionAnalyzer::initHistos(const edm::Service<TFileService> & fs)
{

  signal = fs->make<TH3F>(Form("signal") , ";p_T;#Delta#phi;#Delta#eta" , 100, -50, 50,  31,  -0.5*TMath::Pi()+TMath::Pi()/32, 1.5*TMath::Pi()-TMath::Pi()/32, 33, -4.8 - 4.8/32.0, 4.8 + 4.8/32.0 );  
  hIndex = fs->make<TH1F>("Index","index", 20,-1000,1000 );

  std::vector<std::string> hNames2D = { "hsim", "hrec", "hmul", "hfak",
                                        "heff", "hsec" };

  for( auto name : hNames2D )
  {
     trkCorr2D_[name] = fs->make<TH2F>(name.c_str(),";#eta;p_{T}",
                           etaBins_.size()-1, &etaBins_[0],
			   ptBins_.size()-1, &ptBins_[0]); 
  }

  std::vector<std::string> hNames3D = { "hsim3D", "hrec3D", "hmul3D", "hfak3D",
                                        "heff3D", "hsec3D" };

  for( auto name : hNames3D )
  {
     trkCorr3D_[name] = fs->make<TH3F>(name.c_str(),";#eta;p_{T};occ",
                           etaBins_.size()-1, &etaBins_[0],
                           ptBins_.size()-1, &ptBins_[0],
                           occBins_.size()-1, &occBins_[0]);
  }


  vtxZ_ = fs->make<TH1F>("vtxZ","Vertex z position",100,-30,30);
  pthat_ = fs->make<TH1F>("pthat", "p_{T}(GeV)", 8000,0,800);



//////////
  std::vector<double> ptBinsFine;
  for( unsigned int bin = 0; bin<ptBins_.size()-1; bin++)
  {
    double bStart = ptBins_[bin];
    double bWid = ptBins_[bin+1] - ptBins_[bin];
    for( int i=0;i<5;i++)
      ptBinsFine.push_back( bStart + (double)i * bWid / 5. );
  }
  ptBinsFine.push_back(ptBins_[ptBins_.size()-1]);

  momRes_ = fs->make<TH3F>("momRes","momentum resolution sim vs reco",
                           etaBins_.size()-1, &etaBins_[0],
                           ptBinsFine.size()-1, &ptBinsFine[0],
                           ptBinsFine.size()-1, &ptBinsFine[0]);

}

bool
HITrackCorrectionAnalyzer::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
  if( a.tracksSize() != b.tracksSize() )
    return  a.tracksSize() > b.tracksSize() ? true : false ;
  else
    return  a.chi2() < b.chi2() ? true : false ;  
}

void
HITrackCorrectionAnalyzer::beginJob()
{
}

void
HITrackCorrectionAnalyzer::endJob()
{
std::cout<<"number of particles with pt over 20 GeV is "<<npthigh<<std::endl;
}

DEFINE_FWK_MODULE(HITrackCorrectionAnalyzer);
