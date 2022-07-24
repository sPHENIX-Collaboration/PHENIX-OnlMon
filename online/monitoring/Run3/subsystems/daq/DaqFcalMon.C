#include "DaqFcalMon.h"
#include "GranuleMonDefs.h"

#include "Event.h"
#include "packet.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

DaqFcalMon::DaqFcalMon(const char *what)
{
  if (!strcmp(what, "FCAL"))
    {
      ThisName = "FCAL";
      GranuleNumber = GRAN_NO_FCAL;
    }
  else if (!strcmp(what, "MPC"))
    {
      ThisName = "MPCNORTH";
      GranuleNumber = GRAN_NO_FCAL;
    }
  else if (!strcmp(what, "RXNP"))
    {
      ThisName = "RXNP";
      GranuleNumber = GRAN_NO_FCAL;
    }
  else
    {
      cout << "Invalid Subsystem Name " << what
	   << ", exiting" << endl;
      exit(1);
    }
  return ;
}

int
DaqFcalMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (!strcmp(Name(), "FCAL"))
    {
      ipktmin = 16001;
      ipktmax = 16002;
    }
  else if (!strcmp(Name(), "MPCNORTH"))
    {
      ipktmin = 21011;
      ipktmax = 21042;
    }
  else if (!strcmp(Name(), "RXNP"))
    {
      ipktmin = 23001;
      ipktmax = 23001;
    }
  else
    {
      cout << "Invalid Subsystem Name " << ThisName
	   << ", exiting" << endl;
      exit(1);
    }

  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax; ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int 
DaqFcalMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}
