#include "DaqBbcMon.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"
#include <OnlMonServer.h>

#include <Event.h>
#include <EventTypes.h>
#include <packet.h>

using namespace std;

DaqBbcMon::DaqBbcMon():
  GranuleMon("BBC"),
  twobbcpkts(-1)
{
  GranuleNumber = GRAN_NO_BBC;
  return ;
}

int 
DaqBbcMon::Init()
{

  unsigned int ipktmin = 1001;
  unsigned int ipktmax = 1003;
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax;ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int
DaqBbcMon::BeginRun(const int runno)
{
  OnlMonServer *se = OnlMonServer::instance();
  GranuleMon::BeginRun(runno);
  if (se->IsPacketActive(1001))
    {
      twobbcpkts = 0;
    }
  else
    {
      twobbcpkts = 1;
    }
  return 0;
}

int 
DaqBbcMon::FEMClockCounterCheck()
{
  if (packetiter->PacketId == 1001)
    {
      packetiter->FEMClockCounterError = -1;
    }
  else
    {
      return (GranuleMon::FEMClockCounterCheck());
    }
  return 0;
}

int
DaqBbcMon::FEMParityErrorCheck()
{
  if (packetiter->PacketId == 1001)
    {
      return (GranuleMon::FEMParityErrorCheck());
    }
  else
    {
      packetiter->FEMParityError = -1;
    }
  return 0;
}

