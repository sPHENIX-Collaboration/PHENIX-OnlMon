#ifndef __DAQMON_H__
#define __DAQMON_H__


#include <OnlMon.h>
#include <framePublic.h>
#include <errorBlock.h>

#include <ctime>
#include <iostream>
#include <map>
#include <set>
#include <vector>

class Event;
class GranuleMon;
class OnlMonDB;
class Packet;
class TH1;

class DaqMon: public OnlMon
{
 public:
  static DaqMon *instance(const char *name = "DAQ");
  virtual ~DaqMon();

  int Init();
  int process_event(Event *e);
  int Reset();
  int BeginRun(const int runno);
  int EndRun(const int runno);

  int registerMonitor(GranuleMon *granule);
  void identify(std::ostream& out = std::cout) const;
  int EventNumber() const {return eventnumber;}
  int GL1exists() const {return gl1exists;}
  int LastGL1Clock() const {return last_gl1_clock;}
  int GL1ClockDiff() const {return gl1clockdiff;}
  int GL1ClockCounter() const {return gl1_clock_counter;}
  int GL1GranuleCounter(const short i) {return gl1_granule_counter[i];}
  int isGranuleActive(const short i) {return granule_active[i];}
  unsigned int DaqMonEvts() {return daqmonevts;}
  int DBVarInit();
  unsigned int ClockDiff_PreviousEvent() const {return clkdiff_to_previous_event;}
  int send_message(const int severity, const std::string &err_message, const int msgtype);
  int Run_Has_Gl1() const {return run_has_gl1;}
  void EventIsCorrupt() {eventiscorrupt++;}
  void EventMissesPkts() {eventmisspkts++;}
  void EventHadBadFrameStatus() {eventbadframestatus++;}
  GranuleMon *GetMonitor(std::string &name);
  int fill_sourceid_granulemap();
  int fill_granule_map();

 protected:
  int DcmCheckSumCheck(Event *evt);
  int FrameStatusCheck(Event *evt);
  DaqMon(const char *name);
  static DaqMon *__instance;
  Packet *plist[10000];
  std::vector<GranuleMon*> Granule;
  GranuleMon* GranuleArray[32];
  OnlMonDB *dbvars;
  TH1 *DaqMonStatus;
  TH1 *TimeToLastEvent[4];
  TH1 *FrameStatus;
  TH1 *daq_bcross;
  unsigned int framestatusbits[17];

  int eventnumber;
  int gl1exists;
  int last_gl1_clock;
  int gl1clockdiff;
  int gl1_clock_counter;
  unsigned int daqmonevts;
  unsigned int abortgapchkevts;
  unsigned int totalevents;
  unsigned int corruptevts;
  unsigned int missingpktevts;
  unsigned int badframestatusevts;
  unsigned int previous_corruptevts;
  unsigned int gl1_granule_counter[32];
  int granule_active[32];
  unsigned int clkdiff_to_previous_event;
  unsigned int bbcll1_trigmask;
  unsigned int bbcll1_bcross_array[120];
  int run_has_gl1;
  int eventiscorrupt;
  int eventmisspkts;
  int eventbadframestatus;
  int mark_as_bad;
  unsigned int multibufferedTicks;
  int ismultieventbuffered;
  std::map<FrameTypes,std::string> frametypes;
  std::map<daqErrorCodes,std::string> errorcodes;
  int sourceid_granulemap_initialized;
  std::set<std::string> pcffiles;
  std::map<unsigned int, int> sourceid_granulemap; 
  std::map<std::string, int> granulemap;
};

#endif /* __DAQMON_H__ */
