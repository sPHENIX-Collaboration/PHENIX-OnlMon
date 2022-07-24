#ifndef __EMCANALYS_H__
#define __EMCANALYS_H__

#include <string>
#include <vector>

class OnlMon;

#define NCHANNEL  24768
#define NMAXPAIR  10000
#define NMAXCL    10000

#define NSEC 8 
#define NFEM 172

#define NENERGYLIMITS    3       // 0.2, 1.0, 3.0 GeV
#define BBCZ             30 
#define ZDCZ             150

#define HG_PRE_OUT       0x4     // high gain pre-sample value is OUT-OF-RANGE
#define HG_POST_OUT      0x8     // high gain post-sample value is OUT-OF-RANGE

#define LG_PRE_OUT       0x40    // low gain pre-sample value is OUT-OF-RANGE
#define LG_POST_OUT      0x80    // low gain post-sample value is OUT-OF-RANGE

#define TAC_OUT          0x400   // TAC value is OUT-OF-RANGE
#define CHANNEL_DISABLED 0x2000  // Dead channels for example 

#define ECALIB_DISABLED  0x4000    



#define HG_MIN           1024
#define HG_MAX           4095
#define LG_MIN           0
#define LG_MAX           4095
#define TAC_MIN          50
#define TAC_MAX          4000

class Event;
class BbcEvent;
class BbcCalib;
class ZdcEvent;
class ZdcCalib;
class EmcModule;
class mEmcGeometryModule;
class EmcClusterReco;
class OnCalDBodbc;
class TH1;

struct EMCCluster_DataBlock;

//NMAXPAIR
// EMCal cluster pairs
struct EMCClusterPair {

  float emc_pt;
  float emc_eg1;
  float emc_eg2;
  int   emc_sec1;
  int   emc_sec2;
  float emc_pr1;
  float emc_pr2;
  float emc_tof1;
  float emc_tof2;
  float emc_dgg;
  float emc_m;
  float emc_mc;

};

// EMCal clusters 
struct EMCClusters {
  int   emc_sec;      
  int   emc_sm;       
  int   emc_id;       
  int   emc_nh;     
  int   emc_warn;  
  float emc_e;       
  float emc_ecorr;  
  float emc_ecore;   
  float emc_ecorecorr; 
  float emc_x;         
  float emc_y;   
  float emc_z;   
  float emc_tof;  
  float emc_prob;  

};

const int    kMaxNumberPeaks= 100;
class TSpectr 
{ 
 public:
  TSpectr();
  TSpectr(int maxpositions);     
  virtual ~TSpectr();
  int GetNPeaks() const {return fNPeaks;}
  float *GetPositionX() const {return fPositionX;}
  float *GetFWHM() const {return fFWHM;}
  int Search(TH1 *hist, int nsmoothCoeff); 

 private:
  int fMaxPeaks;     
  int fNPeaks;
  float *fPositionX;
  float fArea[kMaxNumberPeaks];
  float fAreaErr[kMaxNumberPeaks];
  float *fFWHM;
};



class Analys 
{
  public: 
 
  Analys(const OnlMon *parent);   
  virtual ~Analys();
 
  void SetDataWarn(int towerID, int errCode); //mark 3x3 area
  
  void SetDataError(int towerID, int errCode) { fDataError[towerID] |= errCode; } 
  int  GetDataError(int towerID) const { return fDataError[towerID]; }
  void SaveTowerHits(const char *fileName); 
  void SetUseOnlyLowGain(bool flag = false) { fUseOnlyLowGain = flag; }  
  bool GetStatusCalib() const { return fCalib; } 
  void Setup(const int runno);

  int  ProcessBBC_ZDC(Event *ev);
  void ProcessPhysEMC(Event *ev);
  int  ProcessPi0();
  float Emc_Etot() const {return emc_etot;}  
  int  Emc_Npair() const {return emc_npair;}
  EMCClusterPair *EmcClPair;
  
  protected:
  const OnlMon *mymon;
  OnCalDBodbc *oncalDB;
  float InvMass(float e1, float* xyz1, float e2, float* xyz2);

  float bbc_z;
  float bbc_t0;
  float zdc_z;

  float emc_etot;
  int  emc_npair;     
  int  emc_ncl; 
 
  // EMCal clusters  
  EMCClusters *EmcCl;  
  
  BbcEvent *mBbcEvent; 
  BbcCalib *mBbcCalib;
  ZdcEvent *mZdcEvent;
  ZdcCalib *mZdcCalib;

  mEmcGeometryModule *mEmcGeometry;
  EmcClusterReco *mEmcCluster;
  EMCCluster_DataBlock *mEMCClusterData; 
  
  std::vector<EmcModule> HitList[NSEC];
 
  float ThresholdADC;
  float TowerThresholdPbSc;
  float TowerThresholdPbGl;
  
  int fDataError[NCHANNEL];

  bool  fCalib;
  float fEmcCal  [NCHANNEL];
  int  fEmcWarn [NCHANNEL];
  int  emc_twrHit   [NENERGYLIMITS][NCHANNEL]; 
  int  emc_twrEntry [NENERGYLIMITS];    
  bool  fUseOnlyLowGain;
  
};

#endif /*__EMCANALYS_H__*/


