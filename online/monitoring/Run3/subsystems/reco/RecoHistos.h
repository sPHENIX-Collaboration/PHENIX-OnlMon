#ifndef __RECOHISTOS_H__
#define __RECOHISTOS_H__

#include <SubsysReco.h>

class TH1;
class TH2;
class TProfile;

class RecoHistos: public SubsysReco
{
 public:
  RecoHistos(const std::string &name = "RECOHISTOS");
  virtual ~RecoHistos() {}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode) {return 0;}
  int process_event(PHCompositeNode *topNode);
  int Reset(PHCompositeNode *topNode) {return 0;}

 protected:
  TH1 *reco_bbc_qtot;
  TH1 *reco_bbc_ntot;
  TH1 *reco_bbc_zvtx;
  TH1 *reco_bbc_tzero;
  TH2 *reco_bbc_qns;
  TH2 *reco_bbc_nns;
  TH2 *reco_bbc_pc1;
  TH2 *reco_bbc_pc3;
  TH1 *reco_pc1_nclus;
  TH1 *reco_pc2_nclus;
  TH1 *reco_pc3_nclus;
  TH2 *reco_dch_pc1;
  TH1 *reco_dch_ntracks;
  TH1 *reco_dch_mom;
  TH1 *reco_emc_nclus;
  TH1 *reco_emc_energy;
  TH1 *reco_rxn_qtot;
  TH2 *reco_bbc_rxn;
  TH1 *reco_bcross;
  TProfile *reco_bcross_npc1;
  TProfile *reco_bcross_npc2;
  TProfile *reco_bcross_npc3;
  TProfile *reco_bcross_ndch;
  TProfile *reco_bcross_emce;
  unsigned int bcross_array[120];
};


#endif
