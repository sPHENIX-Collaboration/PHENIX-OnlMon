#ifndef __DAQEMCMON_H__
#define __DAQEMCMON_H__

#include "GranuleMon.h"

class Event;

class DaqEmcMon: public GranuleMon
{
 public:
  DaqEmcMon(const char *arm = "EAST", const char *sector = "TOP");
  virtual ~DaqEmcMon() {}

  int Init();
  int DcmFEMParityErrorCheck();

 protected:

};

#endif /* __DAQEMCMON_H__ */
