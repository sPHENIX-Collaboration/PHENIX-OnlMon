#ifndef __ERTLVL1MONDRAW_H__
#define __ERTLVL1MONDRAW_H__

#include "OnlMonDraw.h"
#include "ErtLvl1MonDefs.h"
class TCanvas;
class TPad;
class TText;
class TH1;
class TStyle;

#define NUM_CANVASES  4

#define MAX_PADS_PER_CANVAS  9

class ErtLvl1MonDraw: public OnlMonDraw
{

 public:
  ErtLvl1MonDraw();
  virtual ~ErtLvl1MonDraw();

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvases(const char *name = "ALL");
  void FormatHist(TH1 *Hist);
  int DrawDeadServer(TPad *pad);
  int DrawCommonCanvas(const int index);
  int DrawRocEff(const int iarm);


  TCanvas *ErtLvl1Canv[NUM_CANVASES];
  TPad *ErtLvl1Pad[NUM_CANVASES][MAX_PADS_PER_CANVAS];
  TPad *transparent[NUM_CANVASES];
  TStyle *localStyle;
  TText *labeltext[5];

  TH1 *ErtLvl1_FFA[ARMS];
  TH1 *ErtLvl1_FFB[ARMS];
  TH1 *ErtLvl1_FFC[ARMS];
  TH1 *ErtLvl1_TBT[ARMS];
  TH1 *ErtLvl1_ERON[ARMS];
  TH1 *ErtLvl1_TERON[ARMS];
  TH1 *ErtLvl1_FFASim[ARMS];
  TH1 *ErtLvl1_FFBSim[ARMS];
  TH1 *ErtLvl1_FFCSim[ARMS];
  TH1 *ErtLvl1_TBTSim[ARMS];
  TH1 *ErtLvl1_ERONSim[ARMS];
  TH1 *ErtLvl1_TERONSim[ARMS];

};

#endif /*__ERTLVL1MONDRAW_H__ */
