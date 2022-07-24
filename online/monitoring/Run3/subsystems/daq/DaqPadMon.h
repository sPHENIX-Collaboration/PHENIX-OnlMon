#ifndef __DAQPADMON_H__
#define __DAQPADMON_H__

#include "GranuleMon.h"

class DaqPadMon: public GranuleMon
{
 public:
  DaqPadMon(const char *arm);
  virtual ~DaqPadMon() {}

  int Init();

  int DcmFEMParityErrorCheck();
  int FEMParityErrorCheck();
  int FEMGL1ClockCounterCheck();
  int SubsystemCheck(Event *evt, const int iwhat);  // check the user words
  int GranuleSubsystemCheck();
  int ResetEvent();

 protected:
  int NsubsysErrors;

};

#endif /* __DAQPADMON_H__ */
