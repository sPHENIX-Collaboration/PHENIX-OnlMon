#include "RecoHistos.h"

#include <OnlMonServer.h>

#include <BbcOut.h>
#include <PadCluster.h>
#include <DchTrack.h>
#include <emcClusterContainer.h>
#include <emcClusterContent.h>
#include <RpSumXYObject.h>
#include <PHCentralTrack.h>

#include <Event.h>
#include <packet.h>

#include <getClass.h>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>

using namespace std;

RecoHistos::RecoHistos(const string &name): SubsysReco(name)
{
  memset(bcross_array,0,sizeof(bcross_array));
  return;
}

int
RecoHistos::Init(PHCompositeNode *topNode)
{
OnlMonServer *se = OnlMonServer::instance();

  reco_bbc_qtot = new TH1F("reco_bbc_qtot","BBC Q N+S",400,0.0,400.0);
  reco_bbc_ntot = new TH1F("reco_bbc_ntot","BBC N N+S",131,-0.5,130.5);
  reco_bbc_zvtx = new TH1F("reco_bbc_zvtx","BBC z-vertex",100,-150.0,150.0);
  reco_bbc_tzero = new TH1F("reco_bbc_tzero","BBC time zero",100,0.0,16.0);
  reco_bbc_qns = new TH2F("reco_bbc_qns","BBC q North vs. South",300,0.0,300.0,300,0.0,300.0);
  reco_bbc_nns = new TH2F("reco_bbc_nns","BBC n North vs. South",65,-0.5,64.5,65,-0.5,64.5);

  reco_bbc_pc1 = new TH2F("reco_bbc_pc1","BBC q vs PC1 n clusters",300,0.0,300.0,450,0.0,450.0);
  reco_bbc_pc3 = new TH2F("reco_bbc_pc3","BBC q vs PC3 n clusters",300,0.0,300.0,450,0.0,450.0);
  reco_dch_ntracks = new TH1F("reco_dch_ntracks","DCH n tracks",300,0.0,300.0);
  reco_emc_nclus = new TH1F("reco_emc_nclus","EMC n clusters",600,0.0,600.0);

  reco_pc1_nclus = new TH1F("reco_pc1_nclus","PC1 n clusters",400,0.0,400.0);
  reco_pc2_nclus = new TH1F("reco_pc2_nclus","PC2 n clusters",400,0.0,400.0);
  reco_pc3_nclus = new TH1F("reco_pc3_nclus","PC3 n clusters",300,0.0,300.0);
  reco_dch_mom = new TH1F("reco_dch_mom","DCH momentum",100,0.0,3.0);
  reco_emc_energy = new TH1F("reco_emc_energy","EMC e total",100,0.0,250.0);
  reco_dch_pc1 = new TH2F("reco_dch_pc1","DCH n vs PC1 n ",300,0.0,300.0,300,0.0,300.0);
  //reco_rxn_qtot = new TH1F("reco_rxn_qtot","RXNP q total",100,0.0,160000.0);
  //reco_bbc_rxn = new TH2F("reco_bbc_rxn","BBC q vs RXN q ",100,0.0,160000.0,100,0.0,100.0);


  se->registerHisto("RECOMON",reco_bbc_qtot->GetName(),reco_bbc_qtot);
  se->registerHisto("RECOMON",reco_bbc_ntot->GetName(),reco_bbc_ntot);
  se->registerHisto("RECOMON",reco_bbc_zvtx->GetName(),reco_bbc_zvtx);
  se->registerHisto("RECOMON",reco_bbc_tzero->GetName(),reco_bbc_tzero);
  se->registerHisto("RECOMON",reco_bbc_qns->GetName(),reco_bbc_qns);
  se->registerHisto("RECOMON",reco_bbc_nns->GetName(),reco_bbc_nns);
  se->registerHisto("RECOMON",reco_bbc_pc1->GetName(),reco_bbc_pc1);
  se->registerHisto("RECOMON",reco_bbc_pc3->GetName(),reco_bbc_pc3);
  se->registerHisto("RECOMON",reco_dch_ntracks->GetName(),reco_dch_ntracks);
  se->registerHisto("RECOMON",reco_emc_nclus->GetName(),reco_emc_nclus);
  se->registerHisto("RECOMON",reco_pc1_nclus->GetName(),reco_pc1_nclus);
  se->registerHisto("RECOMON",reco_pc2_nclus->GetName(),reco_pc2_nclus);
  se->registerHisto("RECOMON",reco_pc3_nclus->GetName(),reco_pc3_nclus);
  se->registerHisto("RECOMON",reco_dch_mom->GetName(),reco_dch_mom);
  se->registerHisto("RECOMON",reco_emc_energy->GetName(),reco_emc_energy);
  se->registerHisto("RECOMON",reco_dch_pc1->GetName(),reco_dch_pc1);
  //se->registerHisto("RECOMON",reco_rxn_qtot->GetName(),reco_rxn_qtot);
  //se->registerHisto("RECOMON",reco_bbc_rxn->GetName(),reco_bbc_rxn);
  reco_bcross_npc1 = new TProfile("reco_bcross_npc1","bcross vx npc1",120,-0.5,119.5);
  reco_bcross_npc2 = new TProfile("reco_bcross_npc2","bcross vx npc2",120,-0.5,119.5);
  reco_bcross_npc3 = new TProfile("reco_bcross_npc3","bcross vx npc3",120,-0.5,119.5);
  reco_bcross_ndch = new TProfile("reco_bcross_ndch","bcross vx ndch",120,-0.5,119.5);
  reco_bcross_emce = new TProfile("reco_bcross_emce","bcross vx emce",120,-0.5,119.5);
  reco_bcross = new TH1F("reco_bcross","bunch crossing",120,-0.5,119.5);
  se->registerHisto("RECOMON",reco_bcross_npc1->GetName(),reco_bcross_npc1);
  se->registerHisto("RECOMON",reco_bcross_npc2->GetName(),reco_bcross_npc2);
  se->registerHisto("RECOMON",reco_bcross_npc3->GetName(),reco_bcross_npc3);
  se->registerHisto("RECOMON",reco_bcross_ndch->GetName(),reco_bcross_ndch);
  se->registerHisto("RECOMON",reco_bcross_emce->GetName(),reco_bcross_emce);
  se->registerHisto("RECOMON",reco_bcross->GetName(),reco_bcross);
  return 0;

}

int
RecoHistos::process_event(PHCompositeNode *topNode)
{

  BbcOut *bbc = findNode::getClass<BbcOut>(topNode,"BbcOut");
  PadCluster *pc1 = findNode::getClass<PadCluster>(topNode,"Pc1Cluster");
  PadCluster *pc2 = findNode::getClass<PadCluster>(topNode,"Pc2Cluster");
  PadCluster *pc3 = findNode::getClass<PadCluster>(topNode,"Pc3Cluster");
  emcClusterContainer *emc = findNode::getClass<emcClusterContainer>(topNode,"emcClusterContainer");
  DchTrack *dch = findNode::getClass<DchTrack>(topNode,"DchTrack");
  //RpSumXYObject *rxn = findNode::getClass<RpSumXYObject>(topNode,"RpSumXYObject");
  PHCentralTrack *cgl = findNode::getClass<PHCentralTrack>(topNode,"PHCentralTrack");
  Event *evt = findNode::getClass<Event>(topNode,"PRDF");
  int crossingcounter = -9999;
  Packet *p = evt->getPacket(14001);
  if (p)
    {
      crossingcounter = p->iValue(0, "CROSSCTR");
      bcross_array[crossingcounter]++;
      reco_bcross->SetBinContent((crossingcounter + 1), bcross_array[crossingcounter]);
      delete p;
    }
  if (bbc)
    {

      float qNorth = bbc->get_ChargeSum(0);
      float qSouth = bbc->get_ChargeSum(1);
      float qTotal = qNorth + qSouth;
      float bbcZ = bbc->get_VertexPoint();
      float bbcT0 = bbc->get_TimeZero();
      int nNorth = bbc->get_nPmt(0);
      int nSouth = bbc->get_nPmt(1);
      int nTotal = nSouth + nNorth;
      reco_bbc_qtot->Fill(qTotal);
      reco_bbc_qns->Fill(qNorth,qSouth);
      reco_bbc_zvtx->Fill(bbcZ);
      reco_bbc_tzero->Fill(bbcT0);
      reco_bbc_ntot->Fill((float) nTotal);
      reco_bbc_nns->Fill((float) nNorth,(float) nSouth);

    }

  //if (rxn)
  //  {
  //    float rxnpSumAll = rxn->getRXNsumW8();
  //    //float rxnpSumS = rxn->getRXNsumW2();
  //    //float rxnpSumN = rxn->getRXNsumW5();
  //    reco_rxn_qtot->Fill(rxnpSumAll);
  //  }

  //if (rxn && bbc)
  //  {
  //    float rxnpQ = rxn->getRXNsumW8();
  //    float qN = bbc->get_ChargeSum(0);
  //    float qS = bbc->get_ChargeSum(1);
  //    float qTot = qN + qS;
  //    reco_bbc_rxn->Fill(rxnpQ,qTot);
  //  }

  if (pc1)
    {
      int nPc1Clus = pc1->get_PadNCluster();
      reco_pc1_nclus->Fill((float) nPc1Clus);
      if (crossingcounter >= 0 )
	{
	  reco_bcross_npc1->Fill(crossingcounter,nPc1Clus);
	}
    }

  if (pc2)
    {
      int nPc2Clus = pc2->get_PadNCluster();
      reco_pc2_nclus->Fill((float) nPc2Clus);
      if (crossingcounter >= 0)
	{
	  reco_bcross_npc2->Fill(crossingcounter,nPc2Clus);
	}
    }

  if (pc3)
    {
      int nPc3Clus = pc3->get_PadNCluster();
      reco_pc3_nclus->Fill((float) nPc3Clus);
      if (crossingcounter >= 0)
	{
	  reco_bcross_npc3->Fill(crossingcounter,nPc3Clus);
	}
    }

  if (bbc && pc1)
    {
      float charge = 0;
      for (int i=0; i<2;i++)
	{
	  charge += bbc->get_ChargeSum(i);
	}
      //      pc1->identify();
      float pc1cluster = pc1->get_PadNCluster();
      //      cout << "bbc charge: " << charge << ", pc1: " << pc1cluster << endl;
      reco_bbc_pc1->Fill(pc1cluster,charge);
    }
  if (bbc && pc3)
    {
      float charge = 0;
      for (int i=0; i<2;i++)
	{
	  charge += bbc->get_ChargeSum(i);
	}
      //      pc3->identify();
      float pc3cluster = pc3->get_PadNCluster();
      //      cout << "bbc charge: " << charge << ", pc3: " << pc3cluster << endl;
      reco_bbc_pc3->Fill(pc3cluster,charge);
    }

  if (emc)
    {
      int emcN = emc->size();
      reco_emc_nclus->Fill(float(emcN));

      float emcEtot = 0.0;
      for (int i=0; i<emcN; i++) {
	emcClusterContent *emcCluster = emc->getCluster(i);
	float emcE = emcCluster->e();
	float emcTheta = emcCluster->theta();
	float emcEt = emcE*sin(emcTheta);
	emcEtot += emcEt;
      }
      reco_emc_energy->Fill(emcEtot);
      if (crossingcounter >= 0)
	{
	  reco_bcross_emce->Fill(crossingcounter,emcEtot);
	}

    }

  if (dch)
    {
      int dchN = dch->get_DchNTrack();
      reco_dch_ntracks->Fill((float) dchN);
      if (crossingcounter >= 0)
	{
	  reco_bcross_ndch->Fill(crossingcounter,dchN);
	}

      for (int i=0; i<dchN; i++)
	{
	  float dchMom = dch->get_momentum(i);
	  reco_dch_mom->Fill(dchMom);
	}

    }
  
  if (dch && pc1)
    {
      int nDchTracks = dch->get_DchNTrack();
      int nPc1Clus = pc1->get_PadNCluster();
      reco_dch_pc1->Fill((float) nDchTracks,(float) nPc1Clus);
    }

  if (cgl)
    {
      //int dchN = cgl->get_npart();
    }

  return 0;

}
