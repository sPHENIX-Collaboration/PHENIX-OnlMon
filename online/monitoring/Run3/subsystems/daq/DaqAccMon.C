#include "DaqAccMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <Event.h>
#include <packet.h>
#include <packetConstants.h>

using namespace std;

DaqAccMon::DaqAccMon()
{
  ThisName = "ACC";
  GranuleNumber = GRAN_NO_ACC;
  return ;
}

int 
DaqAccMon::Init()
{
  unsigned int ipktmin = 17001;
  unsigned int ipktmax = 17020;
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax; ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int 
DaqAccMon::DcmFEMParityErrorCheck()
{
  packetiter->DcmFEMParityError = -1;
  return 0;
}

int
DaqAccMon::FEMParityErrorCheck()
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
