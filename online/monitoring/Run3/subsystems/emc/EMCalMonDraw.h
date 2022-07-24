#ifndef __EMCalMONDRAW_H__
#define __EMCalMONDRAW_H__

#include "OnlMonDraw.h"
#include "TLine.h"

class OnlMonDB;
class TArrow;
class TCanvas;
class TF1;
class TH1;
class TH2;
class TPad;
class TPaveText;
class TStyle;

class EMCalMonDraw: public OnlMonDraw
{

 public: 
  EMCalMonDraw();
  virtual ~EMCalMonDraw(); 

  int Init();
 
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  
  // protected:
 private: 
  int MakeCanvas (const char *name);
  int DrawEmcMon1(const char *what);
  int DrawEmcMon2(const char *what);
  int DrawEmcMon3(const char *what);
  int DrawEmcMon4(const char *what);
 
  int DrawDeadServer(TPad *pad);
    

  TStyle *EmcStyle;
    
  TCanvas *c_Emc[4];
  TPad *transparent[2]; 
  TPad *pad_PbGl[4];
  TPad *pad_PbSc[6];
  TArrow *arrowPbgl[4]; 
  TArrow *arrowPbsc[6];

  TPaveText *EmcLabel[5];
  TPaveText *EmcStatLabel[11];
 
  TH1* PbSc_tp;
  TH1* normPbSc_sm;
   
  TPad *pad1_PbGl_Stat;
  TPad *pad1_PbSc_Stat;
  TH2* PbGl_Stat_Blind; 
  TH2* PbGl_Stat_NoiseADC; 
  TH2* PbGl_Stat_NoiseTAC; 
  // TH2* PbGl_Stat_GOOD;
  TH2* PbGl_Stat_PROBL;  
 
  TH2* PbSc_Stat_Blind; 
  TH2* PbSc_Stat_NoiseADC; 
  TH2* PbSc_Stat_NoiseTAC; 
  // TH2* PbSc_Stat_GOOD;
  TH2* PbSc_Stat_PROBL;

  TPad *pad2_EmcPi0[10];
  TPaveText *EmcLabelPi0[10];

  OnlMonDB *dbvars;    
  
  int statDeadServer[2];
  int drawBoxGrid(TH2 *h, TLine &l); 
  TLine lGrid;
  int iCHiSMT(int  iCH);
  int SMiSMTiST(int iSM ,int iSMTower, int emc); 
  TF1 *fpol2;
  TF1 *fitp0;
  int drawLimitHist(float min, float max, TH1 *h, TLine &l); 
   
  int fTr; // 0= BBCLL1, 1= Gamma3 & BBCLL1    
  float m_inv[10], sigm_inv[10];
  float errm_inv[10], errsigm_inv[10];
    
 
};



#endif /*__EMCalMONDRAW_H__ */
















