#ifndef __DAQMONDRAW_H__
#define __DAQMONDRAW_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <OnlMonDraw.h>

class GranMonDraw;
class OnlMonDB;
class RunDBodbc;
class TCanvas;
class TGraphErrors;
class TH1;
class TPad;

static const int NRTYPES = 8; // number of different runtypes
static const int PLOTCOLUMNS = 6;
static const int PLOTROWS = 6;

class DaqMonDraw: public OnlMonDraw
{

 public: 
  static DaqMonDraw *instance(const char *name = "DAQ");
  virtual ~DaqMonDraw();

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  void registerHisto(const char *hname, TH1 *h1d, const int replace = 0);
  void registerHisto(TH1 *h1d, const int replace = 0);
  TH1 *getHisto(const char *) const;
  void Print(const char *what = "ALL") const;
  void registerGranule(GranMonDraw *granule);
  void identify(std::ostream& out = std::cout) const;
  int CanvasSize(const int i = -1,const int xsize = 0, const int ysize = 0);
  void DrawStop(const int i = 1) {drawstop = i;}
  unsigned int GetQuantityPerEvent(std::map<std::string,double> &quantmap, const int histobin);
  int DrawDeadServer(TPad *transparent);
  void AbortGapMessage(const int i=1) {abortgapmessage = i;}

 protected:
  DaqMonDraw(const char *name);
  static DaqMonDraw *__instance;
  int MakeCanvas(const char *name);
  int DrawAllGranules(const char *what);
  int DrawGranule(const char *what);
  int DrawGlobal(const char *what);
  int DrawLowRun(const char *what);
  int DrawNoEvents(TPad *transparent);
  int DrawVtxpGranule(const int packetid);
  std::string GetRunType(const int runno);
  int htmloutflag;
  int canvassize[1][2];
  int drawstop;
  std::vector<GranMonDraw *> Granule;
  std::map<const std::string, TH1 *> HistoMap;
  std::map<int,std::string> runtypecache;
  RunDBodbc *rundb;
  OnlMonDB *dbvars;
  TCanvas *TC1;
  TPad *Pad1[PLOTCOLUMNS*PLOTROWS];
  TCanvas *TC2;
  TPad *Pad2[12];
  TPad *transparent1;
  TPad *transparent2;
  TPad *transparent3;
  TPad *transparent4;
  TPad *transparent5;
  TCanvas *TC3;
  TPad *Pad3[3];
  TCanvas *TC4;
  TPad *Pad4[7];
  TCanvas *TC5;
  TPad *Pad5;
  TGraphErrors *gr[3*NRTYPES];
  int abortgapmessage;
  int multibufferedTicks;
  std::map<std::string, GranMonDraw *> granmap;
};

#endif /*__DAQMONDRAW_H__ */
