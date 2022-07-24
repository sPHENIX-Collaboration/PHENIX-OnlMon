#ifndef __DAQERTMON_H__
#define __DAQERTMON_H__

#include "GranuleMon.h"

class Event;

class DaqErtMon: public GranuleMon
{
 public:
  DaqErtMon(const char *arm);
  virtual ~DaqErtMon() {}

  int Init();

  int DcmFEMParityErrorCheck();

 protected:

};

#endif /* __DAQERTMON_H__ */
