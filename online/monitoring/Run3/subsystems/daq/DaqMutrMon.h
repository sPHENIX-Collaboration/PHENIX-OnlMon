#ifndef __DAQMUTRMON_H__
#define __DAQMUTRMON_H__

#include <GranuleMon.h>

class DaqMutrMon: public GranuleMon
{
 public:
  DaqMutrMon(const char *arm);
  virtual ~DaqMutrMon() {}

  int Init();

  int FEMClockCounterCheck(); // treat Master and Slave boards different
  int ResetEvent();
  int SubsystemCheck(Event *evt, const int iwhat);

 protected:
  unsigned int standard_clock_array[2];

};

#endif /* __DAQMUTRMON_H__ */
