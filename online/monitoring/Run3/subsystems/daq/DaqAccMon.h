#ifndef __DAQACCMON_H__
#define __DAQACCMON_H__

#include <GranuleMon.h>

class DaqAccMon: public GranuleMon
{
 public:
  DaqAccMon();
  virtual ~DaqAccMon() {}

  int Init();

  int DcmFEMParityErrorCheck(); // dcm does not check fem parity
  int FEMParityErrorCheck();

 protected:

};

#endif /* __DAQACCMON_H__ */
