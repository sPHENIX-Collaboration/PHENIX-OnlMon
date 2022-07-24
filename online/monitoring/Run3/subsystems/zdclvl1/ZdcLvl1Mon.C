#include <ZdcLvl1Mon.h>
#include <ZdcLvl1MonDefs.h>
#include <ZDCLl1.h>
#include <OnlMonServer.h>

#include <Event.h>
#include <msg_profile.h>

#include <TH1.h>
#include <TH2.h>

#include <sys/stat.h>

#include <iostream>
#include <sstream>

using namespace std;

ZdcLvl1Mon::ZdcLvl1Mon(unsigned int SL, unsigned int SH,
                       unsigned int NL, unsigned int NH,
                       unsigned int A, unsigned int B): OnlMon("ZDCLVL1MON")
{
  sl=SL;
  sh=SH;
  nl=NL;
  nh=NH;
  a=A;
  b=B;
  // Initialize Globals
  SimZDCLL1 = 0;
  zdcll1_mapdate = 0;
  evtcnt = 0;

  // Create Histograms
  ZdcLvl1_ZDCVertex = new TH1F("ZdcLvl1_ZDCVertex", " ZDC Vertex (TDC counts) ",
                                    60, -60., 60.);
  ZdcLvl1_ZDCSimVertex = new TH1F("ZdcLvl1_ZDCSimVertex", " SIMULATED ZDC Vertex (TDC counts) ",
                                       60, -60., 60.);
  ZdcLvl1_ZDCVertexDiff = new TH1F("ZdcLvl1_ZDCVertexDiff", " Simulated minus LL1 ZDC Vertex (TDC counts) ",
                                        21, -10., 10.);

  ZdcLvl1_ZDCVertexCutA = new TH1F("ZdcLvl1_ZDCVertexCutA", " ZDC LL1 Cut Vertex (cm)",
                                        40, -240., 240.);

  ZdcLvl1_ZDCVertexCutB = new TH1F("ZdcLvl1_ZDCVertexCutB", " ZDC LL1 Cut Vertex (cm)",
                                        40, -240., 240.);

  ZdcLvl1_ZDCVertexNoCut = new TH1F("ZdcLvl1_ZDCVertexNoCut", " ZDC LL1 Cut Vertex (cm)",
                                         40, -240., 240.);


  // Register Histograms

  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(this, ZdcLvl1_ZDCVertex);
  Onlmonserver->registerHisto(this, ZdcLvl1_ZDCSimVertex);
  Onlmonserver->registerHisto(this, ZdcLvl1_ZDCVertexDiff);
  Onlmonserver->registerHisto(this, ZdcLvl1_ZDCVertexCutA);
  Onlmonserver->registerHisto(this, ZdcLvl1_ZDCVertexCutB);
  Onlmonserver->registerHisto(this, ZdcLvl1_ZDCVertexNoCut);

}

ZdcLvl1Mon::~ZdcLvl1Mon()
{
  delete SimZDCLL1;
}

int
ZdcLvl1Mon::BeginRun(const int runno)
{
  if (check_zdcll1_map_date())
    {
      if (SimZDCLL1)
        {
          delete SimZDCLL1;
        }
      SimZDCLL1 = new ZDCLl1(this);
      SimZDCLL1->setVertexLimits(a, b);
      SimZDCLL1->setTDCRanges(sl, sh, nl, nh);
    }

  return 0;
}

int ZdcLvl1Mon::process_event(Event *event)
{

  int *LocPacketData;

  // Only data events
  if (event->getEvtType() != 1)
    {
      return 0;
    }
  evtcnt++;

  // Trigger selection
  if (!IsTriggeredEvent(event))
    {
      return 0;
    }
  // Get packet data

  LocPacketData = getDataFromZdcLL1Packet(event);
  if (!LocPacketData)
    {
      ostringstream msg;
      msg << "Event " << event->getEvtSequence() << " has empty ZDC LL1 packet" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 1);
      return 0;
    }

  //cout << " -------- " << endl;
  //cout << " ZDC LL1 Vertex = " << LocPacketData[1] << endl;
  //cout << " ZDCA_OK = " << LocPacketData[2] << endl;
  //cout << " ZDCB_OK = " << LocPacketData[3] << endl;
  //cout << " -------- " << endl;

  // Fill Histogram data

  // Fill the ZDC vertex if there were TDC hits on both sides
  if ((LocPacketData[10] > 0) && (LocPacketData[11] > 0))
    {
      ZdcLvl1_ZDCVertex->Fill(LocPacketData[1]);
      ZdcLvl1_ZDCVertexNoCut->Fill( (((float)LocPacketData[1]) / 2.0)*0.220*29.98 );
    }

  // Fill the ZDC vertex if there were TDC hits on both sides
  if ((LocPacketData[10] > 0) && (LocPacketData[11] > 0) && (LocPacketData[2]))
    ZdcLvl1_ZDCVertexCutA->Fill( (((float)LocPacketData[1]) / 2.0)*0.220*29.98 );
  if ((LocPacketData[10] > 0) && (LocPacketData[11] > 0) && (LocPacketData[3]))
    ZdcLvl1_ZDCVertexCutB->Fill( (((float)LocPacketData[1]) / 2.0)*0.220*29.98 );

  // Run Simulated ZDC LL1 Trigger

  int ZDC_data_status = SimZDCLL1->getDataFromZdcPacket(event);
  if (!ZDC_data_status)
    {
      ostringstream msg;
      msg << "Event " << event->getEvtSequence() << " has ZDC LL1 simulator failure." << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 2);
    }
  else
    {
      SimZDCLL1->calculate();
      if ( SimZDCLL1->getSouthTDCOK() && SimZDCLL1->getNorthTDCOK() )
	{
        ZdcLvl1_ZDCSimVertex->Fill( SimZDCLL1->getVertex() );
	}

      ZdcLvl1_ZDCVertexDiff->Fill( SimZDCLL1->getVertex() - LocPacketData[1] );

      //cout << " ZDC LL1 Simulated Vertex = " << SimZDCLL1->getVertex() << endl;
      //cout << " ZDC LL1 Simulated VTXA_OK = " << SimZDCLL1->getVtxAOK() << endl;
      //cout << " ZDC LL1 Simulated VTXB_OK = " << SimZDCLL1->getVtxBOK() << endl;
      //cout << endl;
    }

  // All Done!

  return 0;
}

int* ZdcLvl1Mon::getDataFromZdcLL1Packet(Event *event)
{

  Packet* p;

  // Check for the NTCZDC packet first, then for the BIG LL1 packet...

  if (!(p = event->getPacket(p_ZDCLL1))){
    p = event->getPacket(p_BIGLL1);
  }

  if(p)
    {

      PacketData[0] = 0;
      PacketData[1] = p->iValue(0, "ZDCVTX");
      PacketData[2] = p->iValue(0, "ZDCAOK");
      PacketData[3] = p->iValue(0, "ZDCBOK");
      PacketData[4] = 0;
      PacketData[5] = 0;
      PacketData[6] = 0;
      PacketData[7] = 0;
      PacketData[8] = 0;
      PacketData[9] = 0;
      PacketData[10] = p->iValue(0, "ZDC_SOUTH_TDC_OK");
      PacketData[11] = p->iValue(0, "ZDC_NORTH_TDC_OK");

      delete p;
    }
  else
    {
      return 0;
    }

  return PacketData;
}

bool ZdcLvl1Mon::IsTriggeredEvent(Event *event)
{

  return true;

  /*
    Packet* p;

    if ((p = event->getPacket(14001)) != 0) {

    unsigned int RawTriggers = p->iValue(0, "RAWTRIG");
    delete p;

    // Test on triggers here

    if(RawTriggers){
    return true;
    }
    else
    return false;

    }
    else
    return false;

  */

}

int ZdcLvl1Mon::Reset()
{
  // Reset internal counters
  evtcnt = 0;
  return 0;
}

int ZdcLvl1Mon::setVertexLimits(unsigned int A, unsigned int B)
{
  return SimZDCLL1->setVertexLimits(A, B);
}


int ZdcLvl1Mon::setTDCRanges(unsigned int SL, unsigned int SH,
                             unsigned int NL, unsigned int NH)
{
  return SimZDCLL1->setTDCRanges(SL, SH, NL, NH);
}

int
ZdcLvl1Mon::check_zdcll1_map_date()
{
  
  struct stat attrib;
  if (int iret = stat(zdcll1map.c_str(), &attrib))
    {
      ostringstream msg;
      msg << "stat of " <<  zdcll1map << " returned " << iret << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 1);
      return -1;
    }
  time_t mtime = attrib.st_mtime;
  if (zdcll1_mapdate < mtime)

    {
      zdcll1_mapdate = mtime;
      return 1;
    }
  return 0;
}
