#ifndef __MCRMON_H__
#define __MCRMON_H__

#include <OnlMon.h>

#include <ctime>
#include <string>
#include <set>

class BbcCalib;
class BbcEvent;
class Event;
class McrSend;
class OnlMonDB;
class TH1;
class TH2;
class ZdcCalib;
class ZdcEvent;


class McrMon: public OnlMon
{


 public:
  McrMon(const char *name = "MCRMON");
  virtual ~McrMon();

  int Init();
  int process_event(Event *evt);
  int Reset();
  int BeginRun(const int runno);
  void SetGoodEvents(const unsigned int i, const unsigned int j);
  void SetnBins(const int i,const int j) {nBins[j] = i;}
  void SetVtxRange(const int i, const int j) {vtxrange[j] = i;}
  void VertexCalc(const std::string &detector) {vtxdetstr = detector;}
  void AddTriggerName(const std::string &trgname, const int itrig) {triggername[itrig].insert(trgname);}

 protected:
  enum {BBCLL1, ZDCLL1, LAST};
  void setup_calibration(const int runno);
  int Update2d(const int iwhat);
  int Create2dHistos();
  int handleVertex(const float vtx, const int iwhat, const int SendToMcr);
  int DBVarInit();
  std::set<std::string> triggername[LAST];
  std::string vtxdetstr;
  int vtxdet;
  int runnumber;
  int vtxrange[LAST];
  unsigned int nevts[LAST];
  unsigned int ifill[LAST];
  unsigned int trigbit[LAST];
  int currentslice[LAST];
  float mean[LAST];
  float RMS[LAST];
  time_t LastUpdateMcr[LAST];
  time_t LastUpdateDB[LAST];
  int enoughEventsDB[LAST];
  unsigned int nGoodEvents[LAST];
  unsigned int TotalEvents[LAST];
  int nBins[LAST];
  ZdcCalib *zdccalib;
  ZdcEvent *zevt;
  BbcCalib *bbccalib;
  BbcEvent *bevt;
  TH1 *mcrerror;
  TH1 *mcrstat;
  TH1 *zvertex[LAST];
  TH1 *zvertexDB[LAST];
  TH2 *zvertexhist[LAST];
  OnlMonDB *dbvars;
  McrSend *mcrsend;
};

#endif /* __BBCMON_H__ */
