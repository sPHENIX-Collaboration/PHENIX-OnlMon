#include "Event.h"
#include "packet.h"
#include "DaqDchMon.h"
#include "GranuleMonDefs.h"

#include <cstring>
#include <iostream>

using namespace std;

DaqDchMon::DaqDchMon(const char *arm)
{
  if (!strcmp(arm, "WEST"))
    {
      ThisName = "DCHWEST";
      GranuleNumber = GRAN_NO_DCH_WEST;
    }
  else if (!strcmp(arm, "EAST"))
    {
      ThisName = "DCHEAST";
      GranuleNumber = GRAN_NO_DCH_EAST;
    }
  else
    {
      cout << "DaqDchMon: bad arm: " << arm
	   << " valid is EAST WEST, initializing EAST" << endl;
      ThisName = "DCHEAST";
      GranuleNumber = GRAN_NO_DCH_EAST;
    }
  return ;
}

int 
DaqDchMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (!strcmp(Name(), "DCHWEST"))
    {
      ipktmin = 3081;
      ipktmax = 3160;
    }
  else if (!strcmp(Name(), "DCHEAST"))
    {
      ipktmin = 3001;
      ipktmax = 3080;
    }
  else
    {
      cout << "DaqDchMon: Bad DCH: " << Name() << endl;
      return -1;
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int DaqDchMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}
