#include "DaqTofMon.h"
#include "GranuleMonDefs.h"
#include "DaqMon.h"


#include <msg_profile.h>
#include <Event.h>
#include <packet.h>

#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

DaqTofMon::DaqTofMon(const char *arm):
  standard_clock_1(-1),
  standard_clock_2(-1)
{
  if (!strcmp(arm, "EAST"))
    {
      ThisName = "TOFEAST";
      GranuleNumber = GRAN_NO_TOF_EAST;
    }
  else if (!strcmp(arm, "WEST"))
    {
      ThisName = "TOFWEST";
      GranuleNumber = GRAN_NO_TOF_WEST;
    }
  else
    {
      cout << "DaqTofMon: bad arm: " << arm
           << " valid is EAST, RPC or WEST initializing EAST" << endl;
      ThisName = "TOFEAST";
      GranuleNumber = GRAN_NO_TOF_EAST;
    }

  return ;
}

int
DaqTofMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (!strcmp(Name(), "TOFEAST"))
    {
      ipktmin = 7001;
      ipktmax = 7008;
    }
  else if (!strcmp(Name(), "TOFWEST"))
    {
      ipktmin = 7101;
      ipktmax = 7104;
    }
  else
    {
      ostringstream errmsg;
      errmsg
	<< "DaqTofMon::Init(): The Impossible happened, strange name made it to here: "
	<< Name() << ", setting packetrange to TOFWEST";
      DaqMon::instance()->send_message(MSG_SEV_FATAL, errmsg.str(), 30);
      ipktmin = 7001;
      ipktmax = 7008;
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax; ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  ResetEvent(); // init internal variables
  return 0;
}

int
DaqTofMon::FEMClockCounterCheck()
{
  if ((packetiter->PacketId >= 7001 && packetiter->PacketId <= 7004)
      || (packetiter->PacketId >= 7101 && packetiter->PacketId <= 7102))
    {
      if (standard_clock_1 == -1)
        {
          standard_clock_1 = BeamClock;
        }
      else
        {
          if (BeamClock != (unsigned int )standard_clock_1)
            {
              if (verbosity > 0)
                {
                  cout << Name() << ": FEMClockCounterError, BeamClock: 0x"
                       << hex << BeamClock << ", std clock1: 0x"
                       << standard_clock_1
                       << dec << endl;
                }
              packetiter->FEMClockCounterError++;
              return -1;
            }
        }
    }
  else
    {
      if (standard_clock_2 == -1)
        {
          standard_clock_2 = BeamClock;
        }
      else
        {
          if (BeamClock != (unsigned int) standard_clock_2)
            {
              if (verbosity > 0)
                {
                  cout << Name() << ": FEMClockCounterError, BeamClock: 0x"
                       << hex << BeamClock << ", std clock2: 0x"
                       << standard_clock_2
                       << dec << endl;
                }
              packetiter->FEMClockCounterError++;
              return -1;
            }
        }
    }
  return 0;
}

int
DaqTofMon::ResetEvent()
{
  standard_clock_1 = -1;
  standard_clock_2 = -1;
  return 0;
}

