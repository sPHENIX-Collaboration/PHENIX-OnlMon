#ifndef __DAQDCHMON_H__
#define __DAQDCHMON_H__

#include "GranuleMon.h"

class Event;

class DaqDchMon: public GranuleMon
{
 public:
  DaqDchMon(const char *arm);
  virtual ~DaqDchMon() {}

  int Init();

  int DcmFEMParityErrorCheck();

 protected:

};

#endif /* __DAQDCHMON_H__ */
