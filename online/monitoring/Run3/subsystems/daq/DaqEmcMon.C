#include "Event.h"
#include "packet.h"
#include "DaqEmcMon.h"
#include "GranuleMonDefs.h"

#include <cstring>
#include <iostream>

using namespace std;

DaqEmcMon::DaqEmcMon(const char *arm, const char *sector)
{
  string strarm = arm;
  string tb = sector;
  ThisName = "EMC";
  ThisName = ThisName + strarm + tb;
  if (!strcmp(arm, "WEST"))
    {
      if (!strcmp(sector, "TOP"))
        {
          GranuleNumber = GRAN_NO_EMC_WEST_TOP;
        }
      else if (!strcmp(sector, "BOTTOM"))
        {
          GranuleNumber = GRAN_NO_EMC_WEST_BOTTOM;
        }
      else
        {
          goto badparam;
        }
    }
  else if (!strcmp(arm, "EAST"))
    {
      if (!strcmp(sector, "TOP"))
        {
          GranuleNumber = GRAN_NO_EMC_EAST_TOP;
        }
      else if (!strcmp(sector, "BOTTOM"))
        {
          GranuleNumber = GRAN_NO_EMC_EAST_BOTTOM;
        }
      else
        {
          goto badparam;
        }
    }
  else
    {
      goto badparam;
    }
  return ;
 badparam:
  cout << "DaqEmcMon: bad arm: " << arm << " or sector: " << sector
       << " valid is arm = EAST WEST, sector = TOP BOTTOM, initializing EASTTOP" << endl;
  ThisName = "EMCEASTTOP";
  GranuleNumber = GRAN_NO_EMC_EAST_TOP;
  return ;
}

int 
DaqEmcMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  unsigned int refchannelmin;
  unsigned int refchannelmax;
  if (!strcmp(Name(), "EMCWESTTOP"))
    {
      ipktmin = 8037;
      ipktmax = 8072;
      refchannelmin = 8174; 
      refchannelmax = 8174; 
    }
  else if (!strcmp(Name(), "EMCEASTTOP"))
    {
      ipktmin = 8073;
      ipktmax = 8108;
      refchannelmin = 8175; 
      refchannelmax = 8175; 
    }
  else if (!strcmp(Name(), "EMCWESTBOTTOM"))
    {
      ipktmin = 8001;
      ipktmax = 8036;
      refchannelmin = 8173; 
      refchannelmax = 8173; 
    }
  else if (!strcmp(Name(), "EMCEASTBOTTOM"))
    {
      ipktmin = 8109;
      ipktmax = 8172;
      refchannelmin = 8177; 
      refchannelmax = 8180; 
    }
  else
    {
      cout << "DaqEmcMon: Bad EMC: " << Name() << endl;
      return -1;
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  for (unsigned int ipkt = refchannelmin; ipkt <= refchannelmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int 
DaqEmcMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}
