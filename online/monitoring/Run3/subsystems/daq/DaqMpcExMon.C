#include "DaqMpcExMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <Event.h>
#include <packet.h>

#include <cstdlib>

using namespace std;

DaqMpcExMon::DaqMpcExMon(const string &arm)
{
  SetFEMEventSequenceMask(0xFFF); // only 12 bit counter
  if (arm == "NORTH")
    {
      ThisName = "MPCEXNORTH";
    }
  else if (arm == "SOUTH")
    {
      ThisName = "MPCEXSOUTH";
    }
  else
    {
      cout << "invalid arm: " << arm << endl;
      exit(-2);
    }
  return ;
}

int 
DaqMpcExMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (ThisName == "MPCEXNORTH")
    {
      GranuleNumber = GRAN_NO_MPCEX_NORTH;
      ipktmin = 21301;
      ipktmax = 21308;
    }
  else if (ThisName == "MPCEXSOUTH")
    {
      GranuleNumber = GRAN_NO_MPCEX_SOUTH;
      ipktmin = 21351;
      ipktmax = 21358;
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

unsigned int
DaqMpcExMon::LocalEventCounter()
{
  int iev = p->iValue(0, "EVTNR");
  iev++;
  return (iev&0xFFF); // 12 bit event number
}

void
DaqMpcExMon::SetBeamClock()
{
  BeamClock = p->iValue(0, "BCLCK");
  return;
}


int DaqMpcExMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

/*

int 
DaqMpcExMon::FEMEventSequenceCheck()
{
  packetiter->FEMEvtSeqError = -1;
  return 0;
}

int 
DaqMpcExMon::FEMGL1ClockCounterCheck()
{
  packetiter->FEMGL1ClockError = -1;
  return 0;
}

int 
DaqMpcExMon::FEMClockCounterCheck()
{
  packetiter->FEMClockCounterError = -1;
  return 0;
}
*/
