#include "DaqMuTrigMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <Event.h>
#include <packet.h>

#include <cstdlib>

using namespace std;

DaqMuTrigMon::DaqMuTrigMon(const string &arm)
{
  if (arm == "NORTH")
    {
      ThisName = "MUTRIGNORTH";
    }
  else if (arm == "SOUTH")
    {
      ThisName = "MUTRIGSOUTH";
    }
  else
    {
      cout << "invalid arm: " << arm << endl;
      exit(-2);
    }
  return ;
}

int 
DaqMuTrigMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (ThisName == "MUTRIGNORTH")
    {
      GranuleNumber = GRAN_NO_MUTR_NORTH;
      ipktmin = 11500;
      ipktmax = 11503;
    }
  else if (ThisName == "MUTRIGSOUTH")
    {
      GranuleNumber = GRAN_NO_MUTR_SOUTH;
      ipktmin = 11510;
      ipktmax = 11513;
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

int DaqMuTrigMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

unsigned int
DaqMuTrigMon::LocalEventCounter()
{
  //  return p->iValue(0, "TRIGGER");
  return 0;
}

int
DaqMuTrigMon::GlinkCheck()
{
  packetiter->GlinkError = -1;
  return 0;
}

int 
DaqMuTrigMon::FEMEventSequenceCheck()
{
  packetiter->FEMEvtSeqError = -1;
  return 0;
}

int 
DaqMuTrigMon::FEMGL1ClockCounterCheck()
{
  packetiter->FEMGL1ClockError = -1;
  return 0;
}

int 
DaqMuTrigMon::FEMClockCounterCheck()
{
  packetiter->FEMClockCounterError = -1;
  return 0;
}
