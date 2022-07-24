//-----------------------------------------------------------------------------
//
// One online muTr channel, one object
//
//  Author: silvermy (silvermy@lanl.gov)
//-----------------------------------------------------------------------------
#ifndef __MUTONLINECHANNEL_HH__
#define __MUTONLINECHANNEL_HH__

#include <iostream>
#include "PdbMutCalibStrip.hh"

// constants are capitalized
const int PACKETSTART[6] = { 11001, 11041, 11105, 11171, 11211, 11283};  
const int OFFSET_CHN[16] = { 64, 0, 65, 1, 96, 32, 97, 33, 
			     66, 2, 67, 3, 98, 34, 99, 35 };
const int NPARTS[3] = { 4, 8, 8}; // quadrants or octants, per station
const int NPACKETS[6] = { 10, 8, 8, // South: sta1 has 10 per quadrant
			  10, 9, 10}; // North
// station one's packetid<->octant mapping doesn't quite follow logic
const int S1OCT[16] = { 7, 0, 1, 2, 3, 4, 5, 6,
			0, 1, 2, 3, 4, 5, 6, 7};
const int S1Quad[16] = { 0, 1, 1, 2, 2, 3, 3, 0,
			 0, 0, 1, 1, 2, 2, 3, 3}; //map from octant to quadrant

const int NSAMPLES = 4;

class MutOnlineChannel : public PdbMutCalibStrip { 

public:

  MutOnlineChannel();
  MutOnlineChannel(int ipacket, int ichan, int ibackchan);
  MutOnlineChannel(int iarm, int istation, int ioctant,
		  int ihalfoctant, int igap, int iplane,
		  int istrip);
  virtual ~MutOnlineChannel() {}
  
  MutOnlineChannel& operator=(const MutOnlineChannel& rhs);
  virtual void readChannelMap(std::istream& is);
  virtual void writeChannelMap() const;
  virtual void readFullChannelMap(std::istream& is);

  int getChannelId() const;

  int getPacketid()         const { return Packetid; } 
  int getChannel()          const { return Channel; }
  int getBackPlaneChannel() const { return BackPlaneChannel; }

  void setPacketid         (int temp) { Packetid = temp; }
  void setChannel          (int temp) { Channel = temp; }
  void setBackPlaneChannel (int temp) { BackPlaneChannel = temp; }

  int getRealPacket(int reducedPacket) const;
  int getReducedPacket (int realPacket) const;

  void calcBackPlaneChannel();

  // ADC info
  int getNsamples()  const { return NSAMPLES; }

  // copy calib info from PdbMutCalibStrip object
  virtual void copyCalibInfo(const PdbMutCalibStrip * rhs);

private:

  int Packetid; 
  int Channel;
  int BackPlaneChannel;

};

#endif /* __MUTONLINECHANNEL_HH__ */
