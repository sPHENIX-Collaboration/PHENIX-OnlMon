#include "DaqFvtxMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <OnlMonServer.h>

#include <Event.h>
#include <packet.h>

#include <cstdlib>
#include <sstream>

using namespace std;

DaqFvtxMon::DaqFvtxMon(const string &arm)
{
  if (arm == "FVTX")
    {
      ThisName = "FVTX";
    }
  else
    {
      cout << "invalid arm: " << arm << endl;
      exit(-2);
    }
  ResetEvent();
  return ;
}

int
DaqFvtxMon::Init()
{
  unsigned int ipktmin[2];
  unsigned int ipktmax[2];
  if (ThisName == "FVTX")
    {
      GranuleNumber = GRAN_NO_FVTX;
      ipktmin[0] = 25001;
      ipktmax[0] = 25024;
      ipktmin[1] = 25101;
      ipktmax[1] = 25124;
    }
  else
    {
      cout << "invalid subsystem name: " << Name() << endl;
      exit(1);
    }
  for (int j = 0; j < 2; j++)
    {
      for (unsigned int ipkt = ipktmin[j]; ipkt <= ipktmax[j]; ipkt++)
        {
          packetnumbers.insert(ipkt);
        }
    }
  GranuleMon::Init();
  ResetEvent(); // init internal variables
  return 0;
}

int
DaqFvtxMon::DcmFEMParityErrorCheck()
{
  if (p->iValue(0, "CHECKPARITY"))
    {
      return 0;
    }
  else
    {
      packetiter->DcmFEMParityError++;
    }
  return -1;
}

int
DaqFvtxMon::GlinkCheck()
{
  packetiter->GlinkError = -1;
  return 0;
}

void
DaqFvtxMon::SetBeamClock()
{
  BeamClock = p->iValue(0, "BCLCK");
  return;
}

unsigned int
DaqFvtxMon::LocalEventCounter()
{
  int iret = p->iValue(0, "EVTNR");
  iret++;
  return iret;
}

int
DaqFvtxMon::FEMClockCounterCheck()
{
  int iclk;
  if (packetiter->PacketId <= 25012)
    {
      iclk = 0;
    }
  else if (packetiter->PacketId <= 25025)
    {
      iclk = 1;
    }
  else if (packetiter->PacketId <= 25112)
    {
      iclk = 2;
    }
  else
    {
      iclk = 3;
    }
  if (fvtx_standard_clock[iclk] == -1)
    {
      fvtx_standard_clock[iclk] = BeamClock;
    }
  else
    {
      if (BeamClock != (unsigned int )fvtx_standard_clock[iclk])
        {
          if (verbosity > 0)
            {
              cout << Name() << ": FEMClockCounterError, BeamClock: 0x"
                   << hex << BeamClock << ", std clock1: 0x"
                   << fvtx_standard_clock[iclk]
                   << dec << endl;
            }
          packetiter->FEMClockCounterError++;
          return -1;
        }
    }
  return 0;
}

int
DaqFvtxMon::ResetEvent()
{
  for (int i=0; i<4;i++)
    {
      fvtx_standard_clock[i] = -1;
    }
  return 0;
}
