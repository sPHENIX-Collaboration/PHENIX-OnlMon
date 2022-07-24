#ifndef GRANULEMON_H__
#define GRANULEMON_H__

#include <packet.h>

#include <iostream>
#include <set>
#include <string>
#include <vector>

class TH1;
class Event;

struct packetstruct
{
  unsigned int PacketId;
  int BadLength;
  int DcmCheckSumError;
  int FEMParityError;
  int FEMClockCounterError;
  int LastClock;
  int FEMGL1ClockError;
  int FEMEvtSeqError;
  unsigned int PacketCounter;
  unsigned int GoodPackets;
  unsigned int SumBytes;
  int DcmFEMParityError;
  int GlinkError;
  int SubsystemError;
  int MissingPackets;
};

#define NBINS 100


class GranuleMon
{
 public:
  GranuleMon(const std::string &name="UNKNOWN");
  virtual ~GranuleMon();

  virtual int Init();
  virtual int process_event(Event *e);
  virtual int Reset() {return 0;}
  virtual int ResetEvent() {return 0;}
  virtual int PacketErrorReset();
  virtual int PacketResetAll();
  virtual int BeginRun(const int runno);
  virtual int EndRun(const int runno) {return 0;}
  const char *Name() const
    {
      return ThisName.c_str();
    }

  virtual int DcmCheckSumCheck(); // check the dcm checksum (transmission errors)
  virtual int DcmFEMParityErrorCheck(); // check dcm bit for good fem parity
  virtual int FEMParityErrorCheck(); // Check Fem parity
  virtual void SetBeamClock();  // set the BeamClock
  virtual int FEMClockCounterCheck(); // Check if FEM clock counters are consistent in subsystem
  virtual int FEMGL1ClockCounterCheck(); // check if FEM and GL1 agree
  virtual int LocalEventNumberOffset(); // offset between fem event cnter and event number
  virtual unsigned int LocalEventCounter();
  virtual int FEMEventSequenceCheck(); // check if event numbers agree
  virtual unsigned int GlobalEventCounter(); // return GL1 event counter for this granule
  virtual int GranuleGL1SynchCheck(); // test if granule is in sync with GL1
  virtual int GranuleSubsystemCheck() {return 0;} // granule wide subsys check
  virtual int GlinkCheck();
  virtual int SubsystemCheck(Event *evt, const int iwhat);// subsystem specific checks
  virtual void identify(std::ostream& out = std::cout) const;
  virtual int CreateHisto();
  virtual int FillHisto();
  virtual int Verbosity() const {return verbosity;}
  virtual void Verbosity(const int i) {verbosity = i;}
  virtual int NoFemClkReference(const unsigned int PacketId);
  virtual int DoEveryEvent(Event *evt) {return 0;}
  virtual int IncrementPacketCounter();
  virtual int IncrementPacketCounterNoMiss() {return 0;}
  virtual int IncrementPacketCounterNoMissImpl();
  virtual int MissingPacketCheck(Event *evt);
  virtual int MissingPacketCheckImpl(Event *evt);
  virtual int TotalGl1SyncErrors() const {return totalgl1syncerror;}
  virtual int GranuleNum() {return GranuleNumber;}
  virtual int IncrementEvbError() {totalevberror++; return totalevberror;}
  virtual void SetFEMEventSequenceMask(const int i) {femsequence_check_mask = i;}

 protected:
  virtual void ResetErrorCounters();
  int InitPacketIdHisto();
  int CreateRunwiseHisto();
  std::string ThisName;
  int verbosity;
  int firstupdate;
  unsigned int GranuleNumber;
  unsigned int BeamClock;
  int packets_found;
  int gl1syncerror;
  int gl1syncok;
  unsigned int nfill; 
  int totalgl1syncerror;
  int totalsubsystemerror;
  int totaldropcontent;
  int totalscaledtrigerror;
  int totalevberror;
  int packetLength;
  unsigned int standard_clock;
  unsigned int evtsizesum;
  int femsequence_check_mask;
  Packet *p;
  TH1 *ErrorCounterHisto; // store the error counter
  TH1 *packetHisto; // store the packet id
  TH1 *variableHisto; // store whatever vars we need
  std::vector<packetstruct> pkt;
  std::vector<packetstruct>::iterator packetiter;
  std::set<unsigned int> NoUseForFemClkReference;
  std::set<unsigned int> Gl1SyncBad;
  std::set<unsigned int> packetnumbers;
};

#endif /* __GRANULEMON_H__ */

