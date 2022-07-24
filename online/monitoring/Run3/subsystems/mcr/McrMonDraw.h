#ifndef __MCRMONDRAW_H__
#define __MCRMONDRAW_H__

#include "OnlMonDraw.h"

class OnlMonDB;
class TCanvas;
class TGraph;
class TH2;
class TPad;

class McrMonDraw: public OnlMonDraw
{

 public: 
  McrMonDraw(const char *name = "MCRMON");
  virtual ~McrMonDraw();

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  void setrmsrange(const int i, const float val1, const float val2 );
  void setvtxrange(const int i, const float val1, const float val2 );

 protected:
  enum {BBC, ZDC, LAST};
  int MakeCanvas(const char *name);
  int DrawFirst(const char *what = "ALL");
  int DrawHistory(const char *what = "ALL");
  int DrawDeadServer(TPad *transparent);
  int DrawDeadRhicServer(TPad *transparent, const int status);
  int TimeOffsetTicks;
  float rmsrange[LAST][2];
  float vtxrange[LAST][2];
  TCanvas *TC[2];
  TPad *transparent[2];
  TPad *Pad[6];
  TH2 *zvertexhist[LAST];
  TGraph *tgr[4];
  OnlMonDB *dbvars;
};

#endif /*__MCRMONDRAW_H__ */
