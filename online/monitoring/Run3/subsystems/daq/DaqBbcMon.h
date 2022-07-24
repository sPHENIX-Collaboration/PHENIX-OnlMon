#ifndef __DAQBBCMON_H__
#define __DAQBBCMON_H__

#include "GranuleMon.h"

class DaqBbcMon: public GranuleMon
{
 public:
  DaqBbcMon();
  virtual ~DaqBbcMon() {}

  int Init();

  int FEMClockCounterCheck();  // BBC can be 1 or 2 packets
  int MissingPacketCheck(Event *evt) {return MissingPacketCheckImpl(evt);}
  int BeginRun(const int runno); // reset no of bbc packet check
  int FEMParityErrorCheck(); // for time being 1002 and 1003 do not recalculate the parity
  int IncrementPacketCounterNoMiss() {return IncrementPacketCounterNoMissImpl();}
  int IncrementPacketCounter() {return 0;}

 protected:

  int twobbcpkts;

};

#endif /* __DAQBBCMON_H__ */
