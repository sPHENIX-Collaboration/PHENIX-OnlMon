#include "DaqMpcMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <msg_profile.h>
#include <Event.h>
#include <packet.h>

#include <sstream>

using namespace std;

DaqMpcMon::DaqMpcMon(): GranuleMon("MPC")
{
  GranuleNumber = GRAN_NO_FCAL;
  return ;
}

int 
DaqMpcMon::Init()
{
  unsigned int ipktmin = 21101;
  unsigned int ipktmax = 21106;
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int DaqMpcMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

unsigned int
DaqMpcMon::LocalEventCounter()
{
  //  p->identify();
  return p->iValue(0, "TRIGGER");
}

int
DaqMpcMon::GlinkCheck()
{
  packetiter->GlinkError = -1;
  return 0;
}

int 
DaqMpcMon::FEMEventSequenceCheck()
{
  if (! p->iValue(0, "NRMODULES")) // check for empty packet
    {
      return 0;
    }
  DaqMon *daqmon = DaqMon::instance();
  int local_event_counter = LocalEventCounter();
  if (!daqmon->GL1exists())
    {
      if (local_event_counter != (daqmon->EventNumber() + LocalEventNumberOffset()))
        {
          if (verbosity > 0)
            {
              cout << ThisName << "  <E> FEMEventSequenceCheck: "
		   << "Packet: " << packetiter->PacketId
		   << ", local_event_counter: 0x" << hex << local_event_counter
		   << ", daqmon->EventNumber() + LocalEventNumberOffset(): 0x"
		   << daqmon->EventNumber() + LocalEventNumberOffset()
		   << dec << endl;
            }
          packetiter->FEMEvtSeqError++;
          return -1;
        }
    }
  else
    {
      if (local_event_counter != (int) (GlobalEventCounter() & 0xFFF))
        {
          if (verbosity > 0)
            {
              cout << ThisName << " <E> FEMEventSequenceCheck: "
		   << "Event " << GlobalEventCounter()
		   << "Packet: " << packetiter->PacketId
		   << ", local_event_counter: 0x" << hex << local_event_counter
		   << ", GlobalEventCounter(): 0x"
		   << (GlobalEventCounter() &0xFFF) << dec 
		   << endl;
            }
          packetiter->FEMEvtSeqError++;
          return -1;
        }
    }
  return 0;
}

int 
DaqMpcMon::FEMClockCounterCheck()
{
  if (! p->iValue(0, "NRMODULES")) // check for empty packet
    {
      return 0;
    }
  int iret = GranuleMon::FEMClockCounterCheck();
  return iret;

} 
int
DaqMpcMon::FEMGL1ClockCounterCheck()
{
  if (! p->iValue(0, "NRMODULES")) // check for empty packet
    {
      packetiter->LastClock = -1;
      return 0;
    }
  int iret = GranuleMon::FEMGL1ClockCounterCheck();
  return iret;
}

int
DaqMpcMon::SubsystemCheck(Event *evt, const int iwhat)
{
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return 0;
    }
  if (! p->iValue(0, "NRMODULES")) // check for empty packet
    {
      packetiter->SubsystemError++;
      return -1;
    }
  return 0;
}
