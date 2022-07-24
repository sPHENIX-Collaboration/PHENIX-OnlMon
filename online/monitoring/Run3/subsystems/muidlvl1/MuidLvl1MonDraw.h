#ifndef __MUIDLVL1MONDRAW_H__
#define __MUIDLVL1MONDRAW_H__

#include "OnlMonDraw.h"
#include "MuidLvl1MonDefs.h"

class TCanvas;
class TPad;
class TText;
class TH1;
class TH2;
class TStyle;
class TPaveText;

#define NUM_CANVASES  2
#define MAX_PADS_PER_CANVAS  16

class MuidLvl1MonDraw: public OnlMonDraw
{

 public:
  MuidLvl1MonDraw();
  virtual ~MuidLvl1MonDraw();

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  TPaveText *tptext_status[NUM_CANVASES];
  int MakeCanvases(const char *name = "ALL");
  void FormatHist(TH1 *Hist);
  int DrawDeadServer(TPad *pad);
  int DrawCommonCanvas(const int index);
  TCanvas *MuidLvl1Canv[NUM_CANVASES];
  TPad *MuidLvl1Pad[NUM_CANVASES][MAX_PADS_PER_CANVAS];
  TPad *transparent[NUM_CANVASES];
  TStyle *localStyle;
  TH1 *MuidLvl1_DataErr[ARMS];
  TH1 *MuidLvl1_SyncErr[ARMS];
  TH1 *MuidLvl1_DataErr2[ARMS];
  TH1 *MuidLvl1_SyncErr2[ARMS];
  TH1 *MuidLvl1_HorRoadNum[ARMS];
  TH1 *MuidLvl1_VerRoadNum[ARMS];
  TH1 *MuidLvl1_HorRoadNumShal[ARMS];
  TH1 *MuidLvl1_VerRoadNumShal[ARMS];
  TH1 *MuidLvl1_HorRoadNumSim[ARMS];
  TH1 *MuidLvl1_VerRoadNumSim[ARMS];
  TH1 *MuidLvl1_HorRoadNumShalSim[ARMS];
  TH1 *MuidLvl1_VerRoadNumShalSim[ARMS];
  TH1 *MuidLvl1_HorRoadCount[ARMS];
  TH1 *MuidLvl1_VerRoadCount[ARMS];
  TH1 *MuidLvl1_HorRoadCountShal[ARMS];
  TH1 *MuidLvl1_VerRoadCountShal[ARMS];
  TH1 *MuidLvl1_HorRoadCountSim[ARMS];
  TH1 *MuidLvl1_VerRoadCountSim[ARMS];
  TH1 *MuidLvl1_HorRoadCountShalSim[ARMS];
  TH1 *MuidLvl1_VerRoadCountShalSim[ARMS];
  TH1 *MuidLvl1_HorRoadCountDif[ARMS];
  TH1 *MuidLvl1_VerRoadCountDif[ARMS];
  TH2 *MuidLvl1_CompareVH[ARMS];
  TH2 *MuidLvl1_CompareSumsV[ARMS];
  TH1 *MuidLvl1_ESNMismatch[ARMS];
  TH1 *MuidLvl1_ESNCtlMis_V[ARMS];
  TH2 *MuidLvl1_CompareSumsH[ARMS];
  TH1 *MuidLvl1_ESNCtlMis_H[ARMS];
  TH1 *MuidLvl1_HorSymEff[ARMS];
  TH1 *MuidLvl1_VerSymEff[ARMS];
  TH1 *MuidLvl1_HorSymEffCut[ARMS];
  TH1 *MuidLvl1_VerSymEffCut[ARMS];
  TH1 *MuidLvl1_HorSymEffCutShal[ARMS];
  TH1 *MuidLvl1_VerSymEffCutShal[ARMS];
  TH1 *MuidLvl1_GL1Eff[ARMS];
	TH1 *MuidLvl1_InfoHist;
};

#endif /*__MUIDLVL1MONDRAW_H__ */
