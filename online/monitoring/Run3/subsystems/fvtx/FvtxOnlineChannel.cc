#include <iomanip>
#include <cstdio>

#include "FvtxOnlineChannel.hh"

using namespace std;

FvtxOnlineChannel::FvtxOnlineChannel() 
//  : PdbFvtxCalibStrip()
{
  PacketId = -1; FemId = -1; ChipId = -1; ChannelId = -1; 
  ArmId = -1; CageId = -1; StationId = -1; WedgeId = -1; ColumnId = -1; StripId = -1;
}

FvtxOnlineChannel::FvtxOnlineChannel(int ipacket, int ifem, int ichip, int ichan)
//  : PdbFvtxCalibStrip()
{
  PacketId = ipacket; FemId = ifem; ChipId = ichip; ChannelId = ichan;  
}

FvtxOnlineChannel::FvtxOnlineChannel(int iarm, int icage, int istation,
		 int iwedge, int icolumn, int istrip)
//  : PdbFvtxCalibStrip(iarm, istation, ioctant,
//		     ihalfoctant, igap, iplane,
//		     istrip)
{
  PacketId = -1; FemId = -1; ChipId = -1; ChannelId = -1;             
  ArmId = -1; CageId = -1; StationId = -1; WedgeId = -1; ColumnId = -1; StripId = -1;       
}

FvtxOnlineChannel&
FvtxOnlineChannel::operator=(const FvtxOnlineChannel& rhs)
{ 
/*  // copy inherited members of PdbFvtxCalibStrip by hand
  // error prone.. but easy to fix if variables in PdbFvtxCalibStrip change
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
*/
  // and this class' own variables..
  PacketId = rhs.PacketId;
  FemId = rhs.FemId;
  ChipId = rhs.ChipId;
  ChannelId = rhs.ChannelId;

  ArmId = rhs.ArmId;
  CageId = rhs.CageId;
  StationId = rhs.StationId;
  WedgeId = rhs.WedgeId;
  ColumnId = rhs.ColumnId;
  StripId = rhs.StripId;

  return *this;
}

void 
FvtxOnlineChannel::readChannelMap(istream& is) 
{
/*  // arm and station should be knwon when we read this
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
*/
}

void 
FvtxOnlineChannel::writeChannelMap() const
{
//  printf("%d %03d %03d %01d %d %d %d %d %d %03d\n",Packetid,Channel, BackPlaneChannel,ArmNum,StationNum,OctantNum,HalfOctantNum,GapNum,PlaneNum,StripNum);
  return;
}

void 
FvtxOnlineChannel::readFullChannelMap(istream& is)
{
/*  is >> Packetid  
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
*/
  return;
}

int
FvtxOnlineChannel::getGlobalChannelId() const
{
  int id = ChannelId;
  // packet - 24001: 0..23; bit 15-19
  // fem : 1..4; bit 13-14
  // chip : 1..52; bit 7-12
  // channel: 0..127; bits 0-6
  id += ChipId << 7;
  id += FemId << 13;
  int redPacketId = PacketId - PACKETSTART[0];   
  id += redPacketId << 15;
  return id;
} 


/*
int
FvtxOnlineChannel::getRealPacket (int reducedPacket) const
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
FvtxOnlineChannel::getReducedPacket (int realPacket) const
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
*/
/*
void
FvtxOnlineChannel::calcBackPlaneChannel()
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
*/

/*
void
FvtxOnlineChannel::copyCalibInfo (const PdbFvtxCalibStrip * rhs) 
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
*/
