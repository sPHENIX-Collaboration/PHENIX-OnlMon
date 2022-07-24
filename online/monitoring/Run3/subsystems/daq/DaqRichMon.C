#include <DaqRichMon.h>
#include <GranuleMonDefs.h>

#include <Event.h>
#include <packet.h>
#include <packetConstants.h>

#include <cstring>
#include <iostream>

using namespace std;

DaqRichMon::DaqRichMon(const char *arm)
{
  if (!strcmp(arm, "WEST"))
    {
      ThisName = "RICHWEST";
      GranuleNumber = GRAN_NO_RICH_WEST;
    }
  else if (!strcmp(arm, "EAST"))
    {
      ThisName = "RICHEAST";
      GranuleNumber = GRAN_NO_RICH_EAST;
    }
  else
    {
      cout << "DaqRichMon: bad arm: " << arm
           << " valid is EAST WEST, initializing EAST" << endl;
      ThisName = "RICHEAST";
      GranuleNumber = GRAN_NO_RICH_EAST;
    }
  return ;
}

int
DaqRichMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (!strcmp(Name(), "RICHWEST"))
    {
      ipktmin = 6001;
      ipktmax = 6018;
    }
  else if (!strcmp(Name(), "RICHEAST"))
    {
      ipktmin = 6021;
      ipktmax = 6038;
    }
  else
    {
      cout << "DaqRichMon: Bad RICH: " << Name() << endl;
      return -1;
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax; ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int
DaqRichMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

int
DaqRichMon::FEMParityErrorCheck()
{
  // FEM parity word is wrong for this hitformat
  if (IDRICH_FPGA == p->getHitFormat())
    {
      packetiter->FEMParityError = -1;
      return 0;
    }
  int iret = GranuleMon::FEMParityErrorCheck();
  return iret;
}
