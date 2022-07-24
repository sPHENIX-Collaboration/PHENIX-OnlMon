#include "DaqPadMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <Event.h>
#include <packet.h>
#include <packetConstants.h>
#include <msg_profile.h>

#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

DaqPadMon::DaqPadMon(const char *arm)
{
  if (!strcmp(arm, "WEST"))
    {
      ThisName = "PADWEST";
      GranuleNumber = GRAN_NO_PAD_WEST;
    }
  else if (!strcmp(arm, "EAST"))
    {
      ThisName = "PADEAST";
      GranuleNumber = GRAN_NO_PAD_EAST;
    }
  else
    {
      cout << "DaqPadMon: bad arm: " << arm
	   << " valid is EAST WEST, initializing EAST" << endl;
      ThisName = "PADEAST";
      GranuleNumber = GRAN_NO_PAD_EAST;
    }
  ResetEvent();
  return ;
}

int
DaqPadMon::Init()
{
  unsigned int ipktmin[3];
  unsigned int ipktmax[3];
  unsigned short int jmax;
  if (!strcmp(Name(), "PADWEST"))
    {
      ipktmin[0] = 4001;
      ipktmax[0] = 4016;
      ipktmin[1] = 4033;
      ipktmax[1] = 4048;
      ipktmin[2] = 4065;
      ipktmax[2] = 4080;
      jmax = 3;
    }
  else if (!strcmp(Name(), "PADEAST"))
    {
      ipktmin[0] = 4017;
      ipktmax[0] = 4032;
      ipktmin[1] = 4081;
      ipktmax[1] = 4096;
      jmax = 2;
    }
  else
    {
      cout << "DaqPadMon: Bad PAD: " << Name() << endl;
      return -1;
    }
  for (unsigned short j = 0; j < jmax; j++)
    {
      for (unsigned int ipkt = ipktmin[j]; ipkt <= ipktmax[j];ipkt++)
        {
          packetnumbers.insert(ipkt);
        }
    }
  GranuleMon::Init();
  return 0;
}

int
DaqPadMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

int
DaqPadMon::FEMParityErrorCheck()
{
  if (IDPC_FPGA == p->getHitFormat())
    {
      packetiter->FEMParityError = -1;
      return 0;
    }
  int iret = p->iValue(0, "CHECKPARITY");
  switch (iret)
    {
    case 0:
      packetiter->FEMParityError = -1;
      return 0;

    case 1:
      return 0;

    case -1:
      packetiter->FEMParityError++;
      break;

    default:
      cout << "GranuleMon::FEMParityErrorCheck(): Packet " << packetiter->PacketId
	   << " has invalid CHECKPARITY status" << endl;
      packetiter->FEMParityError++;
      break;
    }
  return -1;
}

int
DaqPadMon::FEMGL1ClockCounterCheck()
{
  // pad chambers have a rollover problem in the beamcounter
  if (BeamClock == 0x0)
    {
      packetiter->LastClock = -1;
      return 0;
    }
  int iret = GranuleMon::FEMGL1ClockCounterCheck();
  return iret;
}

int
DaqPadMon::SubsystemCheck(Event *evt, const int iwhat)
{
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return 0;
    }
  static int BadVal[8] = {0xFFFFF, 0xFFFFF, 0xFFFFF, 0xFFFFF, 0xFFFFF, 0xFFFFF, 0xFFFFF, 0xFFFFF};
  // only declare user words as bad if ALL userwords are 0xFFFFF
  for (int i = 0; i < 8; i++)
    {
      if (p->iValue(i, "USERWORD") != BadVal[i])
        {
          return 0;
        }
    }
  ostringstream msg;
  msg << "Packet " << packetiter->PacketId
      << ": Bad Pad User Words all are 0xFFFFF"
      << dec;
  DaqMon::instance()->send_message(MSG_SEV_FATAL, msg.str(), 20);
  NsubsysErrors++;
  packetiter->SubsystemError++;
  return -1;
}

int
DaqPadMon::GranuleSubsystemCheck()
{
  // if all packets PC.E south have issues NsubsysErrors = 16
  // leave a bit of room if some packets are removed
  // they are removed in pairs, so this gets triggered even
  // if 2 pairs were removed. But we don't want to send this
  // stop the daq if only a few FEMs are shot
  if (NsubsysErrors > 11)
    {
      totalsubsystemerror++;
    }
  return 0;
}

int 
DaqPadMon::ResetEvent()
{
  NsubsysErrors = 0;
  return 0;
}
