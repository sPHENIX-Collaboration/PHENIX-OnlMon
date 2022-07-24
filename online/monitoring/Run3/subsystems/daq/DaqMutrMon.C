#include "DaqMutrMon.h"
#include "GranuleMonDefs.h"
#include "DaqMon.h"

#include <Event.h>
#include <packet.h>

#include <cstring>
#include <iostream>

using namespace std;

DaqMutrMon::DaqMutrMon(const char *arm)
{
  if (!strcmp(arm, "SOUTH"))
    {
      ThisName = "MUTRSOUTH";
      GranuleNumber = GRAN_NO_MUTR_SOUTH;
    }
  else if (!strcmp(arm, "NORTH"))
    {
      ThisName = "MUTRNORTH";
      GranuleNumber = GRAN_NO_MUTR_NORTH;
    }
  else
    {
      cout << "DaqMutrMon: bad arm: " << arm
	   << " valid is SOUTH, initializing SOUTH" << endl;
      ThisName = "MUTRSOUTH";
      GranuleNumber = GRAN_NO_MUTR_SOUTH;
    }
  ResetEvent();
  return ;
}

int 
DaqMutrMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (!strcmp(Name(), "MUTRSOUTH"))
    {
      ipktmin = 11001;
      ipktmax = 11168;
    }
  else if (!strcmp(Name(), "MUTRNORTH"))
    {
      ipktmin = 11171;
      ipktmax = 11362;
    }
  else
    {
      cout << "DaqMutrMon: Bad MUTR: " << Name() << endl;
      return -1;
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  ResetEvent();
  return 0;
}

int
DaqMutrMon::SubsystemCheck(Event *evt, const int iwhat)
{
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return 0;
    }
  int iarray = p->iValue(0, "MODULE");
  if (iarray < 0 || iarray > 1)
    {
      //      cout << "Invalid Module address from packet " << packetiter->PacketId << endl;
      packetiter->SubsystemError++;
      return -1;
    }
  return 0;
}


int 
DaqMutrMon::FEMClockCounterCheck()
{
  int iarray = p->iValue(0, "MODULE");
  if (iarray < 0 || iarray > 1)
    {
      return 0; // this is checked and flagged in the subsystem check
    }
  if ( standard_clock_array[iarray] == INIT_STANDARD_CLOCK)
    {
      if (! NoUseForFemClkReference.empty())
        {
          if (NoUseForFemClkReference.find(packetiter->PacketId) != NoUseForFemClkReference.end())
            {
              return 0;
            }
        }
      if (!Gl1SyncBad.empty())
        {
          if (Gl1SyncBad.find(packetiter->PacketId) != Gl1SyncBad.end())
            {
              return 0;
            }
        }
      standard_clock_array[iarray] = BeamClock;
      return 0;
    }
  else
    {
      if (standard_clock_array[iarray] !=  BeamClock)
        {
          packetiter->FEMClockCounterError++;
          if (verbosity > 0)
            {
              cout << ThisName << " <E> FEMClockCounterCheck: "
		   << "PacketId: " << packetiter->PacketId
		   << ", BeamClock: " << hex << BeamClock
		   << ", standard_clock_array[" << iarray << "]: "
		   << standard_clock_array[iarray] << dec << endl;
            }
          return -1;
        }
    }
  return 0;
}

int 
DaqMutrMon::ResetEvent()
{
  for (int i=0;i<2;i++)
    {
      standard_clock_array[i] = INIT_STANDARD_CLOCK;
    }
  return 0;
}
