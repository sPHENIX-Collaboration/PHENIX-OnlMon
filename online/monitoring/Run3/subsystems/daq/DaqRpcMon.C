#include <DaqRpcMon.h>
#include <DaqMon.h>
#include <GranuleMonDefs.h>

#include <Event.h>
#include <packet.h>

#include <cstdlib>

using namespace std;

DaqRpcMon::DaqRpcMon(const string &arm)
{
  if (arm == "RPC1")
    {
      ThisName = "RPC1";
    }
  else if (arm == "RPC3")
    {
      ThisName = "RPC3";
    }
  else
    {
      cout << "invalid arm: " << arm << endl;
      exit(-2);
    }
  return ;
}

int 
DaqRpcMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (ThisName == "RPC3")
    {
      GranuleNumber = GRAN_NO_RPC3;
      ipktmin = 19001;
      ipktmax = 19008;
    }
  else if (ThisName == "RPC1")
    {
      GranuleNumber = GRAN_NO_RPC1;
      ipktmin = 19009;
      ipktmax = 19012;
    }
  else
    {
      cout << "invalid subsystem name: " << Name() << endl;
      exit(1);
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax; ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int DaqRpcMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

unsigned int
DaqRpcMon::LocalEventCounter()
{
  int iev = p->iValue(0, "EVTNR");
  iev++;
  return (iev&0xFFF); // 12 bit event number
}

int
DaqRpcMon::GlinkCheck()
{
  packetiter->GlinkError = -1;
  return 0;
}

int
DaqRpcMon::FEMEventSequenceCheck()
{
  DaqMon *daqmon = DaqMon::instance();
  int local_event_counter = LocalEventCounter();
  if (!daqmon->GL1exists())
    {
      if (local_event_counter != ((daqmon->EventNumber() + LocalEventNumberOffset()) & 0xFFF))
        {
          if (verbosity > 0)
            {
              cout << ThisName << "  <E> FEMEventSequenceCheck: "
		   << "Packet: " << packetiter->PacketId
		   << ", local_event_counter: 0x" << hex << local_event_counter
		   << ", daqmon->EventNumber() + LocalEventNumberOffset(): 0x"
		   << ((daqmon->EventNumber() + LocalEventNumberOffset())  & 0xFFF)
		   << dec << endl;
            }
          packetiter->FEMEvtSeqError++;
          return -1;
        }
    }
  else
    {
      if ((local_event_counter&0xFFFF) != ((int) (GlobalEventCounter()&0xFFF)))
        {
          if (verbosity > 0)
            {
              cout << ThisName << " <E> FEMEventSequenceCheck: "
		   << "Packet: " << packetiter->PacketId
		   << ", local_event_counter: 0x" << hex << local_event_counter
		   << ", GlobalEventCounter(): 0x"
		   << (GlobalEventCounter()&0xFFF) << dec << endl;
            }
          packetiter->FEMEvtSeqError++;
          return -1;
        }
    }
  return 0;
}

void
DaqRpcMon::SetBeamClock()
{
  BeamClock = p->iValue(0, "CLOCK");
  return;
}

