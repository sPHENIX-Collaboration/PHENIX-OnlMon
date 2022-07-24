#ifndef __DAQZDCMON_H__
#define __DAQZDCMON_H__

#include "GranuleMon.h"

class DaqZdcMon: public GranuleMon
{
 public:
  DaqZdcMon();
  virtual ~DaqZdcMon() {}

  int Init();

  int FEMClockCounterCheck(); // for single FEM pointless
  int MissingPacketCheck(Event *evt) {return MissingPacketCheckImpl(evt);}
  int IncrementPacketCounterNoMiss() {return IncrementPacketCounterNoMissImpl();}
  int IncrementPacketCounter() {return 0;}

 protected:

};

#endif /* __DAQZDCMON_H__ */
