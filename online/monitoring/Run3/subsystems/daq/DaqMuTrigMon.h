#ifndef __DAQMUTRIGMON_H__
#define __DAQMUTRIGMON_H__

#include <GranuleMon.h>

class DaqMuTrigMon: public GranuleMon
{
 public:
  DaqMuTrigMon(const std::string &system);
  virtual ~DaqMuTrigMon() {}

  int Init();

  int DcmFEMParityErrorCheck(); // Dcm does not recalculte fem parity
  unsigned int LocalEventCounter();
  int FEMEventSequenceCheck();
  int FEMGL1ClockCounterCheck();
  int FEMClockCounterCheck();
  int GlinkCheck();

 protected:
  DaqMuTrigMon() {}
};

#endif /* __DAQMUTRIGMON_H__ */
