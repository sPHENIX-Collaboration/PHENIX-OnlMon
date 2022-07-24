#ifndef __DAQFVTXMON_H__
#define __DAQFVTXMON_H__

#include "GranuleMon.h"

class DaqFvtxMon: public GranuleMon
{
 public:
  DaqFvtxMon(const std::string &system);
  virtual ~DaqFvtxMon() {}

  int Init();

  int DcmFEMParityErrorCheck(); // Compare the dcm2-calculated packet parity to the parity the FEM thinks it sent out
  void SetBeamClock(); 
  int GlinkCheck();
  unsigned int LocalEventCounter();
  int FEMClockCounterCheck();
  int ResetEvent();

 protected:
  DaqFvtxMon() {}
  int fvtx_standard_clock[4];
};

#endif /* __DAQFVTXMON_H__ */
