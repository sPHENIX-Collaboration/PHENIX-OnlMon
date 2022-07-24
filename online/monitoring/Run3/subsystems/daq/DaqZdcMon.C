#include "Event.h"
#include "EventTypes.h"
#include "packet.h"
#include "DaqZdcMon.h"
#include "GranuleMonDefs.h"
#include "DaqMon.h"

using namespace std;

DaqZdcMon::DaqZdcMon()
{
  ThisName = "ZDC";
  GranuleNumber = GRAN_NO_ZDC;
  return ;
}

int DaqZdcMon::Init()
{
  packetnumbers.insert(13001);
  GranuleMon::Init();
  return 0;
}

int DaqZdcMon::FEMClockCounterCheck()
{
  packetiter->FEMClockCounterError = -1;
  return 0;
}

