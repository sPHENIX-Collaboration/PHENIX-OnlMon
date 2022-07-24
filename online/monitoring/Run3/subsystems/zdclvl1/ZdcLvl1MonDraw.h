#ifndef __ZDCLVL1MONDRAW_H__
#define __ZDCLVL1MONDRAW_H__

#include "OnlMonDraw.h"

class TCanvas;
class TPad;
class TText;
class TH1;

#define NUM_CANVASES  1
#define MAX_PADS_PER_CANVAS  4

class ZdcLvl1MonDraw: public OnlMonDraw
{

 public:
  ZdcLvl1MonDraw();
  virtual ~ZdcLvl1MonDraw(){}

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvases(const char *name = "ALL");
  void FormatHist(TH1 *Hist);
  int DrawDeadServer(TPad *pad);
  TCanvas *ZdcLvl1Canv[NUM_CANVASES];
  TPad *ZdcLvl1Pad[NUM_CANVASES][MAX_PADS_PER_CANVAS];
  TPad *transparent[NUM_CANVASES];
};

#endif /*__ZDCLVL1MONDRAW_H__ */
