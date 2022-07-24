#include "ZDCLl1.h"
#include "ZDCLut.h"
#include "OnlMonServer.h"

#include "Event.h"
#include "packet.h"
#include "msg_profile.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

ZDCLl1::ZDCLl1(const OnlMon *parent)
{
  mymon = parent;
  zdcFEMLUT = new ZDCLut(mymon);
  success = zdcFEMLUT->Success();
  if (!success)
    {
      ostringstream msg;
      msg << "ZDCLl1: Can't open ZDCLut LUT file " << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 3);
      return ;
    }

  // Default TDC range and vertex limits

  SouthRangeLow = 0x1;
  SouthRangeHigh = 0x7F;
  NorthRangeLow = 0x1;
  NorthRangeHigh = 0x7F;

  VertexLimA = 0x2D;  // 148.5 cm
  VertexLimB = 0x9;   // 29.7 cm

}

ZDCLl1::~ZDCLl1()
{
  delete zdcFEMLUT;
}

int ZDCLl1::getDataFromZdcPacket(Event *event)
{

  static const int packetid = 13001;

  Packet* p;

  if ((p = event->getPacket(packetid)) != 0)
    {
      for (int ipmt = 0; ipmt < 8; ipmt++)
        {
          T2[ipmt] = p->iValue(ipmt, "T2");
          //cout << "ZDCLl1: T2["<<ipmt<<"] = "<< T2[ipmt] << endl;
        }
    }
  else
    {
      return 0;
    }
  delete p;

  return 1;
}

int ZDCLl1::setTDCRanges(unsigned int S_low,
                         unsigned int S_high,
                         unsigned int N_low,
                         unsigned int N_high)
{

  SouthRangeLow = S_low;
  SouthRangeHigh = S_high;
  NorthRangeLow = N_low;
  NorthRangeHigh = N_high;

  return 1;
}

int ZDCLl1::setVertexLimits(unsigned int VlimA, unsigned int VlimB)
{

  VertexLimA = VlimA;
  VertexLimB = VlimB;

  return 1;
}

int ZDCLl1::calculate()
{

  // Convert the TDC's to LL1 TDC values
  zdcFEMLUT->fillLL1array(LL1T2, T2);

  int SouthAnalogSumTDC = LL1T2[0];
  int NorthAnalogSumTDC = LL1T2[4];

  //cout << SouthAnalogSumTDC << " " << NorthAnalogSumTDC << endl; 

  SouthTDC_OK = (SouthAnalogSumTDC >= SouthRangeLow) && (SouthAnalogSumTDC <= SouthRangeHigh);
  NorthTDC_OK = (NorthAnalogSumTDC >= NorthRangeLow) && (NorthAnalogSumTDC <= NorthRangeHigh);

  Vertex = SouthAnalogSumTDC - NorthAnalogSumTDC;

  VtxAOK = SouthTDC_OK && NorthTDC_OK && (Vertex < VertexLimA) && (Vertex > ( -1 * VertexLimA));
  VtxBOK = SouthTDC_OK && NorthTDC_OK && (Vertex < VertexLimB) && (Vertex > ( -1 * VertexLimB));

  return 1;
}

