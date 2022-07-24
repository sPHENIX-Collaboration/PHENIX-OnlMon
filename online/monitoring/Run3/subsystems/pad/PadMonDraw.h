#ifndef __PADMONDRAW_H__
#define __PADMONDRAW_H__

#include "OnlMonDraw.h"
#include "PadMonPar.h"

class TCanvas;
class TH1;
class TLine;
class TPad;
class TBox;
class TText;

class PadMonDraw: public OnlMonDraw
{

 public: 
  PadMonDraw();
  virtual ~PadMonDraw();

  int CreateCanvas(); 
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:

  //  TStyle *padstyle;
  TCanvas *cPad;
  TPad *cPadTop;
  TPad *cPadMiddle;
  TPad *cPadBottom;
  TText *text, *warning;
  TLine *bar,*dashedbar;
  TBox *pt1;
  TBox *frame;
  TH1 *padClustRatio;
  TH1 *padClustRatioRed;
  TH1 *padClustRatioBlack;
  TH1 *padClustRatioWatch;
  TH1 *padClustActivityRed;
  TH1 *padClustActivityBlack;
  TH1 *padClustActivityWatch;
  TH1 *padEventErrorRed;
  TH1 *padEventErrorBlack;

  int getHVHardwareNotation(short ipc, short iarm, short ihvsectid, char *hvhwname="HV_PCX_Y_Z_W_V");
  int getFEMHardwareNotation(short ipc, short iarm, short ifemid, char *femhwname="FEM.PCX.Y.Z.W");
  int padSortVector(float val[], int NBins, int NPos);

};

#endif /*__PADMONDRAW_H__ */
