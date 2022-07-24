#ifndef __BBCLVL1MON_H__
#define __BBCLVL1MON_H__

#include <OnlMon.h>
#include <ctime>
#include <string>

class BbcLl1;
class Event;
class TH1;

class BbcLvl1Mon: public OnlMon
{

 public:
  BbcLvl1Mon(const char *name = "BBCLVL1MON");
  virtual ~BbcLvl1Mon();

  int process_event(Event *evt);
  int Reset();
  int BeginRun(const int runno);
  //  setmaptime is just for forcing a map load for testing
  //  void setmaptime(time_t newtime) {bbcll1_mapdate = newtime;}

 protected:
  int check_bbcll1_map_date();
  BbcLl1 *BBCLvl1;

  unsigned int trigignoremask;
  float ConvertToCM;
  float ConvertToNS;

  int evtcnt, goodevt, badevt, FailedRB, EvtCount;
  int SouthHits, SouthHitsOut;
  int NorthHits, NorthHitsOut;
  int Vertex, VertexOut;
  int Average, AverageOut;
  int ArmTimeSouth, ArmTimeSouthOut;
  int ArmTimeNorth, ArmTimeNorthOut;
  unsigned int VertexOK, VertexOKOut;
  unsigned int Vertex2OK; 
  unsigned int HitsInput, HitsInputOut;
  unsigned int BB_LL1_RB, BB_LL1_RB_Out;
  time_t bbcll1_mapdate;
  std::string bbcll1_mapdir;
  int bbc_in_partition;

  TH1 *BbcLvl1_VertHist;
  TH1 *BbcLvl1_Vert2Hist;
  TH1 *BbcLvl1_AverHist;
  TH1 *BbcLvl1_SHitsHist;
  TH1 *BbcLvl1_NHitsHist;
  TH1 *BbcLvl1_VertHistErr;
  TH1 *BbcLvl1_AverHistErr;
  TH1 *BbcLvl1_SHitsHistErr;
  TH1 *BbcLvl1_NHitsHistErr;
  TH1 *BbcLvl1_STimeHistErr;
  TH1 *BbcLvl1_NTimeHistErr;
  TH1 *BbcLvl1_NChAllHist;
  TH1 *BbcLvl1_SChAllHist;
  TH1 *BbcLvl1_NChBadHist;
  TH1 *BbcLvl1_SChBadHist;
  TH1 *BbcLvl1_DataError;


};

#endif /* __BBCLVL1MON_H__ */
