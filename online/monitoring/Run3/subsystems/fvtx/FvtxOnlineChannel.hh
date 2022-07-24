//-----------------------------------------------------------------------------
//
// One online fvtx channel, one object
//
//  Author: Zhengyun You (youzy@lanl.gov)
//-----------------------------------------------------------------------------
#ifndef __FVTXONLINECHANNEL_HH__
#define __FVTXONLINECHANNEL_HH__

#include <iostream>
//#include "PdbFvtxCalibStrip.hh"

// constants are capitalized
const int PACKETSTART[2] = { 24001, 24025 };  
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

const int NSAMPLES = 3;

class FvtxOnlineChannel // : public PdbFvtxCalibStrip
{ 

public:

  FvtxOnlineChannel();
  FvtxOnlineChannel(int ipacket, int ifem, int ichip, int ichan);
  FvtxOnlineChannel(int iarm, int icage, int istation,
                    int iwedge, int icolumn, int istrip);
  virtual ~FvtxOnlineChannel() {}
  
  FvtxOnlineChannel& operator=(const FvtxOnlineChannel& rhs);
  virtual void readChannelMap(std::istream& is);
  virtual void writeChannelMap() const;
  virtual void readFullChannelMap(std::istream& is);

  int getGlobalChannelId() const;

  int getPacketId()         const { return PacketId; } 
  int getFemId()            const { return FemId; }
  int getChipId()           const { return ChipId; }
  int getChannelId()        const { return ChannelId; }

  void setPacketId         (int temp) { PacketId = temp; }
  void setFemId            (int temp) { FemId = temp; }
  void setChipId           (int temp) { ChipId = temp; }
  void setChannelId        (int temp) { ChannelId = temp; }

  int getArmId()              const { return ArmId; }
  int getCageId()             const { return CageId; }
  int getStationId()          const { return StationId; }
  int getWedgeId()            const { return WedgeId; }
  int getColumnId()           const { return ColumnId; }
  int getStripId()            const { return StripId; }

  void setArmId            (int temp) { ArmId = temp; }
  void setCageId           (int temp) { CageId = temp; }
  void setStationId        (int temp) { StationId = temp; }
  void setWedgeId          (int temp) { WedgeId = temp; }
  void setColumnId         (int temp) { ColumnId = temp; }
  void setStripId          (int temp) { StripId = temp; }

  //int getRealPacket(int reducedPacket) const;
  //int getReducedPacket (int realPacket) const;

  // ADC info
  int getNsamples()  const { return NSAMPLES; }

  // copy calib info from PdbFvtxCalibStrip object
  //virtual void copyCalibInfo(const PdbFvtxCalibStrip * rhs);

private:

  int PacketId;
  int FemId;
  int ChipId; 
  int ChannelId;

  int ArmId;
  int CageId;
  int StationId;
  int WedgeId;
  int ColumnId;
  int StripId;
};

#endif /* __FVTXONLINECHANNEL_HH__ */
