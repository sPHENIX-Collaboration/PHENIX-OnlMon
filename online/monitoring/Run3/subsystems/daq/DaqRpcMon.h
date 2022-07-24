#ifndef __DAQRPCMON_H__
#define __DAQRPCMON_H__

#include <GranuleMon.h>

class DaqRpcMon: public GranuleMon
{
 public:
  DaqRpcMon(const std::string &system);
  virtual ~DaqRpcMon() {}

  int Init();

  int DcmFEMParityErrorCheck(); // Dcm does not recalculate fem parity
  unsigned int LocalEventCounter();
  int FEMEventSequenceCheck();
  void SetBeamClock();
  int GlinkCheck();

 protected:
  DaqRpcMon() {}
};

#endif /* __DAQRPCMON_H__ */
