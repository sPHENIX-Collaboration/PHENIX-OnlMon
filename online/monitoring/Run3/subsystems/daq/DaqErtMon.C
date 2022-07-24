#include "DaqErtMon.h"
#include "GranuleMonDefs.h"
#include "Event.h"
#include "packet.h"

#include <cstring>
#include <iostream>

using namespace std;

DaqErtMon::DaqErtMon(const char *arm)
{
  if (!strcmp(arm, "WEST"))
    {
      ThisName = "ERTWEST";
      GranuleNumber = GRAN_NO_ERT_WEST;
    }
  else if (!strcmp(arm, "EAST"))
    {
      ThisName = "ERTEAST";
      GranuleNumber = GRAN_NO_ERT_EAST;
    }
  else
    {
      cout << "DaqErtMon: bad arm: " << arm
	   << " valid is WEST, initializing WEST" << endl;
      ThisName = "ERTWEST";
      GranuleNumber = GRAN_NO_ERT_WEST;
    }
  return ;
}

int 
DaqErtMon::Init()
{
  unsigned int ipkt;
  if (!strcmp(Name(), "ERTWEST"))
    {
      ipkt = 14200;
    }
  else if (!strcmp(Name(), "ERTEAST"))
    {
      ipkt = 14201;
    }
  else
    {
      cout << "DaqErtMon: Bad ERT: " << Name() << endl;
      return -1;
    }

  packetnumbers.insert(ipkt);
  GranuleMon::Init();
  return 0;
}

int DaqErtMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}
