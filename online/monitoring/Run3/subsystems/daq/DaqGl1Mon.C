#include "DaqGl1Mon.h"
#include "GranuleMonDefs.h"
#include "DaqMon.h"
#include <OnlMonServer.h>

#include <msg_profile.h>
#include <Event.h>
#include <packet.h>

#include <TH1.h>

#include <iostream>
#include <sstream>

using namespace std;

DaqGl1Mon::DaqGl1Mon() : GranuleMon("GL1")
{
  GranuleNumber = GRAN_NO_GL1;
  return ;
}

int
DaqGl1Mon::Init()
{
  packetnumbers.insert(14001);
  packetnumbers.insert(14002); // BBCLL1
  //  packetnumbers.insert(14007);
  packetnumbers.insert(14008); // GL1P
  packetnumbers.insert(14009); // Previous Event Clk counter
  packetnumbers.insert(14011); // GL1PSum
  packetnumbers.insert(14020); // ERTLL1 East
  packetnumbers.insert(14021); // ERTLL1 West
  GranuleMon::Init();
  return 0;
}

int
DaqGl1Mon::process_event(Event *e)
{
  DaqMon *daq = DaqMon::instance();
  OnlMonServer *se = OnlMonServer::instance();
  unsigned int scaledtrigger = se->Trigger(2);
  for (packetiter = pkt.begin(); packetiter != pkt.end(); ++packetiter)
    {
      packetiter->FEMClockCounterError = -1;
      //      packetiter->FEMEvtSeqError = -1;
      packetiter->DcmFEMParityError = -1;
      packetiter->GlinkError = -1;

      // increment packet counter, since gl1 has to be in every event it
      // is incremented for every event so missing packets show up
      // under bad packets
      if (daq->GL1exists()) // only increment packet counter if we ran this with gl1
	{
          IncrementPacketCounterNoMiss();
	}
      p = e->getPacket(packetiter->PacketId);
      if (p)
        {
          packetLength = p->getLength();
          if (packetLength & 0xFFFFF800)
            {
              packetiter->BadLength++;
              delete p;
              continue;
            }
          packetiter->SumBytes += packetLength << 2; // bitshift by 2 is equal to div by 4 but faster
	  int isGood = 0;
          isGood += FEMParityErrorCheck();
          if (p->getDebugLength())
            {
              if (DcmCheckSumCheck()) // corrupt data can cause crashes
                {
                  delete p;
                  continue;
                }
            }
          isGood += FEMEventSequenceCheck();
          if (scaledtrigger == 0 && daq->GL1exists()) // scaled trigger = 0 -> gl1 out of sync
            {
              isGood += SubsystemCheck(e, SUBSYS_PACKETEXIST);
            }
          if (isGood == 0)
            {
              packetiter->GoodPackets++;
            }
          delete p;
        }
      else
        {
          MissingPacketCheck(e);
        }
    }
  // do not reset totalscaledtrigerror because this is a fatal condition
  // and there might be intermittent events with a scaled trigger word != 0
  if (scaledtrigger == 0 && daq->GL1exists())
    {
      totalscaledtrigerror++;
      ostringstream errmsg;
      errmsg << "DaqGl1Mon: Event with Scaled trigger word = 0, counter: " << totalscaledtrigerror;
      DaqMon::instance()->send_message(MSG_SEV_FATAL, errmsg.str(), 11);
    }
  variableHisto->SetBinContent(SCALEDFATALBIN, totalscaledtrigerror);
  if (! firstupdate && daq->DaqMonEvts() > FIRSTUPDATE)
    {
      firstupdate = 1;
      FillHisto();
    }
  if ( daq->DaqMonEvts() % NUPDATE == 0)
    {
      FillHisto();
    }
  return 0;
}

int
DaqGl1Mon::IncrementPacketCounterNoMiss()
{
  packetiter->PacketCounter++;
  return 0;
}

int
DaqGl1Mon::SubsystemCheck(Event *evt, const int iwhat)
{
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return 0;
    }
  if (packetiter->PacketId == 14001)
    {
      packetiter->SubsystemError++;
      return 1;
    }
  return 0;
}

unsigned int
DaqGl1Mon::LocalEventCounter()
{
  switch (packetiter->PacketId)
    {
    case 14001:
      return p->iValue(0, "ACPTCTR");
    case 14008:
      return p->iValue(0, "EVNUMBER");
    default:
      return -1;
    }
  return 0xFFFFFFFF;
}

int
DaqGl1Mon::FEMEventSequenceCheck()
{
  int local_event_counter = (int) LocalEventCounter();
  if (local_event_counter < 0)
    {
      return 0;
    }
  DaqMon *daqmon = DaqMon::instance();
  if ((local_event_counter&0xFF) != (daqmon->EventNumber()&0xFF))
    {
      if (verbosity > 0)
        {
          cout << ThisName << " <E> FEMEventSequenceCheck: "
               << "Packet: " << packetiter->PacketId
               << ", local_event_counter: 0x" << hex << local_event_counter
               << ", GlobalEventCounter(): 0x"
               << GlobalEventCounter() << dec << endl;
        }
      packetiter->FEMEvtSeqError++;
      return -1;
    }
  return 0;
}
