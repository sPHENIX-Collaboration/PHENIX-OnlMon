#ifndef __DAQGL1MON_H__
#define __DAQGL1MON_H__

#include <GranuleMon.h>

class DaqGl1Mon: public GranuleMon
{
 public:
  DaqGl1Mon();
  virtual ~DaqGl1Mon() {}

  int Init();

  int process_event(Event *e);
  int IncrementPacketCounter() {return 0;}
  int IncrementPacketCounterNoMiss();
  int MissingPacketCheck(Event *evt) {return MissingPacketCheckImpl(evt);}
  int SubsystemCheck(Event *evt, const int iwhat);
  unsigned int LocalEventCounter();
  int FEMEventSequenceCheck();

 protected:

};

#endif /* __DAQGL1MON_H__ */
