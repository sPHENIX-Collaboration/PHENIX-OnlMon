#include <iomanip>
#include <cstdio>

#include "MutOnlineChannel.hh"

using namespace std;

MutOnlineChannel::MutOnlineChannel() 
  : PdbMutCalibStrip()
{
  Packetid = -1; Channel = -1; BackPlaneChannel = -1; 
}

MutOnlineChannel::MutOnlineChannel(int ipacket, int ichan, int ibackchan)
  : PdbMutCalibStrip()
{
  Packetid = ipacket; Channel = ichan; BackPlaneChannel = ibackchan;
}

MutOnlineChannel::MutOnlineChannel(int iarm, int istation, int ioctant,
		 int ihalfoctant, int igap, int iplane,
		 int istrip)
  : PdbMutCalibStrip(iarm, istation, ioctant,
		     ihalfoctant, igap, iplane,
		     istrip)
{
  Packetid = -1; Channel = -1; BackPlaneChannel = -1; 
}

MutOnlineChannel&
MutOnlineChannel::operator=(const MutOnlineChannel& rhs)
{ 
  // copy inherited members of PdbMutCalibStrip by hand
  // error prone.. but easy to fix if variables in PdbMutCalibStrip change
  ArmNum = rhs.ArmNum;
  StationNum = rhs.StationNum;
  OctantNum = rhs.OctantNum;
  HalfOctantNum = rhs.HalfOctantNum;
  GapNum = rhs.GapNum;
  PlaneNum = rhs.PlaneNum;
  StripNum = rhs.StripNum;
  pedestal = rhs.pedestal;
  gain = rhs.gain; 
  rms = rhs.rms;

  for (int i=0; i<NMUTCALIBPAR; i++)
    {
      calibpar[i] = rhs.calibpar[i];
    }

  Status = rhs.Status; 
  Step = rhs.Step;
  Saturation = rhs.Saturation;

  // and this class' own variables..
  Packetid = rhs.Packetid;
  Channel = rhs.Channel;
  BackPlaneChannel = rhs.BackPlaneChannel;

  return *this;
}

void 
MutOnlineChannel::readChannelMap(istream& is) 
{
  // arm and station should be knwon when we read this
  int reducedPacket;
  is >> reducedPacket;  // a shortened version of the real packetid
  is >> Channel;
  if (StationNum == 0)  
    {
      is >> OctantNum;
    }  
  is >> HalfOctantNum
     >> GapNum  
     >> PlaneNum
     >> StripNum; 
  // ok, now we need to figure out what the other indices are
  Packetid = getRealPacket(reducedPacket); // the real packetid
  // we don't know the BackPlaneChannel 
  calcBackPlaneChannel();
  return;
}

void 
MutOnlineChannel::writeChannelMap() const
{
  printf("%d %03d %03d %01d %d %d %d %d %d %03d\n",Packetid,Channel, BackPlaneChannel,ArmNum,StationNum,OctantNum,HalfOctantNum,GapNum,PlaneNum,StripNum);
  return;
}

void 
MutOnlineChannel::readFullChannelMap(istream& is)
{
  is >> Packetid  
     >> Channel
    //     >> BackPlaneChannel, not available from offline
     >> ArmNum
     >> StationNum
     >> OctantNum;
  is >> HalfOctantNum
     >> GapNum  
     >> PlaneNum
     >> StripNum;
  // the plane index from offline was 2, where we expected 1
  if (PlaneNum == 2) PlaneNum = 1;
  // we don't know the BackPlaneChannel 
  calcBackPlaneChannel();
  return;
}

int
MutOnlineChannel::getChannelId () const
{
  int id = Channel;
  // packet - 11001: 0..361; bit 7-16
  // channel: 0..128 (max on North); bits 0-6
  int redPacketId = Packetid - PACKETSTART[0]; 
  id += redPacketId << 7;

  return id;
} 

int
MutOnlineChannel::getRealPacket (int reducedPacket) const
{
  int id = reducedPacket + PACKETSTART[ArmNum*3 + StationNum];
  if (StationNum == 0)
    {
      id += S1Quad[ArmNum*8 + OctantNum]*NPACKETS[ArmNum*3 + StationNum];
    }
  else 
    {
      id += OctantNum*NPACKETS[ArmNum*3 + StationNum];
    }
  return id;
} 

int
MutOnlineChannel::getReducedPacket (int realPacket) const
{
  int id = realPacket - PACKETSTART[ArmNum*3 + StationNum]; 
  if (StationNum == 0)
    {
      id -= S1Quad[ArmNum*8 + OctantNum]*NPACKETS[ArmNum*3 + StationNum];
    }
  else 
    {
      id -= OctantNum*NPACKETS[ArmNum*3 + StationNum];
    }

  return id;
} 

void
MutOnlineChannel::calcBackPlaneChannel()
{
  int chnbunch, chan;
  int first_chn;

  // if we know the elec. channel, but not backplane channel
  // then we need to figure out what the backplane channel is 
  for (chnbunch = 0; chnbunch<16; chnbunch++)
    {
      for (chan=0; chan<8; chan++)
	{
	  first_chn = OFFSET_CHN[chnbunch] + 4*chan;
	  if (first_chn == Channel) BackPlaneChannel = chan + chnbunch*8;
	}
    }
  return;
}

void
MutOnlineChannel::copyCalibInfo (const PdbMutCalibStrip * rhs) 
{
  pedestal = rhs->get_pedestal();
  gain = rhs->get_gain(); 
  rms = rhs->get_rms();

  for (int i = 0; i<NMUTCALIBPAR; i++)
    {
      calibpar[i] = rhs->get_calibpar(i);
    }
  Status = rhs->getStatus(); 
  Step = rhs->getStep();
  Saturation = rhs->getSaturation();

  return;
} 


