#ifndef __DAQTOFMON_H__
#define __DAQTOFMON_H__

#include "GranuleMon.h"

class Event;

class DaqTofMon: public GranuleMon
{
 public:
  DaqTofMon(const char *arm = "EAST");
  virtual ~DaqTofMon() {}

  int Init();
  int FEMClockCounterCheck(); // two different branches with different fem clk's
  int ResetEvent();

 protected:
  int standard_clock_1;
  int standard_clock_2;
};

#endif /* __DAQTOFMON_H__ */
