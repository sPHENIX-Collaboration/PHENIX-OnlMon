#include "DaqMuidMon.h"
#include "GranuleMonDefs.h"
#include "DaqMon.h"

#include <Event.h>
#include <EventTypes.h>
#include <packet.h>
#include <msg_profile.h>

#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

DaqMuidMon::DaqMuidMon(const char *arm):
  NsubsysErrors(0)
{
  if (!strcmp(arm, "SOUTH"))
    {
      ThisName = "MUIDSOUTH";
      GranuleNumber = GRAN_NO_MUID_SOUTH;
    }
  else if (!strcmp(arm, "NORTH"))
    {
      ThisName = "MUIDNORTH";
      GranuleNumber = GRAN_NO_MUID_NORTH;
    }
  else
    {
      cout << "DaqMuidMon: bad arm: " << arm
	   << " valid is SOUTH, initializing SOUTH" << endl;
      ThisName = "MUIDSOUTH";
      GranuleNumber = GRAN_NO_MUID_SOUTH;
    }
  return ;
}

int
DaqMuidMon::Init()
{
  unsigned int ipktmin = 0;
  unsigned int ipktmax = 0;
  if (!strcmp(Name(), "MUIDSOUTH"))
    {
      ipktmin = 12001;
      ipktmax = 12002;
    }
  else if (!strcmp(Name(), "MUIDNORTH"))
    {
      ipktmin = 12003;
      ipktmax = 12004;
    }
  else
    {
      cout << "DaqMuidMon: Bad MUID: " << Name() << endl;
      return -1;
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int 
DaqMuidMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

int 
DaqMuidMon::SubsystemCheck(Event *evt, const int iwhat)
{
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return 0;
    }
  static int GoodVal[3] = {0xFFFF, 0xFFFF, 0xFF};
  for (int i = 0;i < 3;i++)
    {
      if (p->iValue(i, "USERWORD") != GoodVal[i])
        {
          ostringstream msg;
          msg << "Packet " << packetiter->PacketId
	      << ": Bad MUID User Word " << i << ": "
	      << hex << p->iValue(i, "USERWORD") << dec;
          DaqMon::instance()->send_message(MSG_SEV_FATAL, msg.str(),20);
          NsubsysErrors++;
          packetiter->SubsystemError++;
	  return -1;
        }
    }
  return 0;
}

int
DaqMuidMon::GranuleSubsystemCheck()
{
  // in Run13 (starting May 19th) we have intermittent errors of this kind. 
  // If we just
  // increment this counter for every error it will reach the threshold of
  // >5 at some point sending a stop the daq message to the shift crew.
  // as a remedy the totalsubsystemerror is reset if the event was fine
  // If the beam clock is really shot, the error occurs in every event
  // and the counter will never be reset making sure the stop the daq
  // will make it to the shift crew
  if (NsubsysErrors)
    {
      totalsubsystemerror++;
    }
  else
    {
      totalsubsystemerror = 0;
    }
  return 0;
}

int 
DaqMuidMon::ResetEvent()
{
  NsubsysErrors = 0;
  return 0;
}
