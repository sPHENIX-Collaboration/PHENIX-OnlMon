#ifndef __ZDCLVL1MON_H__
#define __ZDCLVL1MON_H__

#include <OnlMon.h>
#include <ctime>
#include <string>

const int p_ZDCLL1 = 14007;
const int p_BIGLL1 = 14777;
const int NUMDATAWORDS = 12;


class Event;
class TH1;
class ZDCLl1;

class ZdcLvl1Mon: public OnlMon{ 

 public:
  ZdcLvl1Mon(unsigned int SL=0x1, unsigned int SH=0x7F, 
	     unsigned int NL=0x1, unsigned int NH=0x7F, 
	     unsigned int A=0x2D, unsigned int B=0x9);
  virtual ~ZdcLvl1Mon();

  int process_event(Event *evt);
  int Reset();
  int BeginRun(const int runno);

  int* getDataFromZdcLL1Packet(Event *event);
  bool IsTriggeredEvent(Event *event);

  int setVertexLimits(unsigned int A, unsigned int B);
  int setTDCRanges(unsigned int SL, unsigned int SH,
		   unsigned int NL, unsigned int NH);


  //  setmaptime is just for forcing a map load for testing
  //  void setmaptime(time_t newtime) {zdcll1_mapdate = newtime;}

 protected:
  int check_zdcll1_map_date();
  unsigned int evtcnt;
  time_t zdcll1_mapdate;
  unsigned int sl;
  unsigned int sh;
  unsigned int nl;
  unsigned int nh;
  unsigned int a;
  unsigned int b;

  bool ZDC_A_EVENT;
  bool ZDC_B_EVENT;

  ZDCLl1 *SimZDCLL1;

  int PacketData[NUMDATAWORDS];

  TH1 *ZdcLvl1_ZDCVertex;
  TH1 *ZdcLvl1_ZDCSimVertex;
  TH1 *ZdcLvl1_ZDCVertexDiff;
  TH1 *ZdcLvl1_ZDCVertexCutA;
  TH1 *ZdcLvl1_ZDCVertexCutB;
  TH1 *ZdcLvl1_ZDCVertexNoCut;

};

#endif /* __ZDCLVL1MON_H__ */
