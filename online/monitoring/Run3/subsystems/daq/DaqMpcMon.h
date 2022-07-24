#ifndef __DAQMPCMON_H__
#define __DAQMPCMON_H__

#include "GranuleMon.h"

class DaqMpcMon: public GranuleMon
{
 public:
  DaqMpcMon();
  virtual ~DaqMpcMon() {}

  int Init();

  int DcmFEMParityErrorCheck(); // Dcm does not recalculte fem parity
  unsigned int LocalEventCounter();
  int FEMEventSequenceCheck();
  int GlinkCheck();
  int FEMClockCounterCheck();
  int FEMGL1ClockCounterCheck();
  int SubsystemCheck(Event *evt, const int iwhat);  // check for empty packets

 protected:

};

#endif /* __DAQMPCMON_H__ */
