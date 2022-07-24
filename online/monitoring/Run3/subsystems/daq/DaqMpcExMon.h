#ifndef DAQMPCEXMON_H__
#define DAQMPCEXMON_H__

#include <GranuleMon.h>

class DaqMpcExMon: public GranuleMon
{
 public:
  DaqMpcExMon(const std::string &system);
  virtual ~DaqMpcExMon() {}

  int Init();

  void SetBeamClock();
  unsigned int LocalEventCounter(); 
  int DcmFEMParityErrorCheck(); // Dcm does not recalculte fem parity
  /* int FEMEventSequenceCheck(); */
  /* int FEMGL1ClockCounterCheck(); */
  /* int FEMClockCounterCheck(); */
  /* int GlinkCheck(); */

 protected:
  DaqMpcExMon() {}
};

#endif /* __DAQMPCEXMON_H__ */
