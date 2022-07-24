#ifndef __BBCLVL1MONDRAW_H__
#define __BBCLVL1MONDRAW_H__

#include <OnlMonDraw.h>

class TCanvas;
class TH1;
class TLine;
class TPad;
class TStyle;

class BbcLvl1MonDraw: public OnlMonDraw
{

 public:
  BbcLvl1MonDraw(const char *name = "BBCLVL1MON");
  virtual ~BbcLvl1MonDraw();

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvases(const char *name = "ALL");
  void FormatHist(TH1 *Hist);
  int DrawDeadServer(TPad *pad);
  TStyle *bbcll1Style;
  TStyle* oldStyle;
  TCanvas *TC;
  TPad *Lvl1Pad[4], *Lvl1PadErr[8];
  TPad *transparent;
  TLine *line1, *line2;
  TH1 *BbcLvl1_NorthBadHist;
  TH1 *BbcLvl1_SouthBadHist;
};

#endif /*__BBCLVL1MONDRAW_H__ */
