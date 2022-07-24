#ifndef __DAQMUIDMON_H__
#define __DAQMUIDMON_H__

#include <GranuleMon.h>

class Event;

class DaqMuidMon: public GranuleMon
{
 public:
  DaqMuidMon(const char *arm);
  virtual ~DaqMuidMon() {}

  int Init();

  int DcmFEMParityErrorCheck();
  int SubsystemCheck(Event *evt, const int iwhat);  // check the user words
  int MissingPacketCheck(Event *evt) {return MissingPacketCheckImpl(evt);}
  int IncrementPacketCounterNoMiss() {return IncrementPacketCounterNoMissImpl();}
  int IncrementPacketCounter() {return 0;}
  int GranuleSubsystemCheck();
  int ResetEvent();

 protected:

  int NsubsysErrors;
  

};

#endif /* __DAQMUIDMON_H__ */
