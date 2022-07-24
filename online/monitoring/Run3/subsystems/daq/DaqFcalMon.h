#ifndef __DAQFCALMON_H__
#define __DAQFCALMON_H__

#include "GranuleMon.h"

class Event;

class DaqFcalMon: public GranuleMon
{
 public:
  DaqFcalMon(const char *what);
  virtual ~DaqFcalMon() {}

  int Init();

  int DcmFEMParityErrorCheck();

 protected:

};

#endif /* __DAQFCALMON_H__ */
