#ifndef __ONLMON_H__
#define __ONLMON_H__

#include "OnlMonBase.h"
#include <iostream>
#include <set>
#include <string>


class Event;
class OnlMonServer;

class OnlMon: public OnlMonBase
{
 public:
  OnlMon(const std::string &name = "NONE");
  virtual ~OnlMon() {}

  enum {ACTIVE = -1, OK = 0, WARNING = 1, ERROR = 2};
  virtual int process_event_common(Event *evt);
  virtual int process_event(Event *evt);
  virtual int InitCommon(OnlMonServer *se);
  virtual int Init() {return 0;}
  virtual int Reset();
  virtual void identify(std::ostream& out = std::cout) const;
  virtual int BeginRunCommon(const int runno, OnlMonServer *se);
  virtual int BeginRun(const int runno) {return 0;}
  virtual int EndRun(const int runno) {return 0;}
  virtual void AddTrigger(const std::string &name);
  virtual void AddLiveTrigger(const std::string &name);
  virtual void SetStatus(const int newstatus);
  virtual int ResetEvent() {return 0;}
  
 protected:
  std::set<std::string> TriggerList; // trigger selection send to the et pool
  std::set<std::string> LiveTriggerList; // triggers filtered in process_event according to live bit
  unsigned int livetrigmask;
  int status;
};

#endif /* __ONLMON_H__ */

