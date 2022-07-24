//////////////////////////////////////////////////////////////////
// Author  :  Jaehyeon Do   Jan/08/2014                         //
// Revision                                                     //
// MPCEX Online PRDF Unpacker                                   // 
//////////////////////////////////////////////////////////////////

#ifndef __MPCEXUNPACKPRDFBASE_H__
#define __MPCEXUNPACKPRDFBASE_H__

#include <MpcExConst.h>
#include <fileEventiterator.h>    // contains Event and Packet 
#include <EventTypes.h>
class TMpcExHitMap;

class MpcExUnpackPRDFBase
{
public:
  MpcExUnpackPRDFBase();
  virtual ~MpcExUnpackPRDFBase();
  int getArm(int ipacket, int iblock);
  int getLayer(int ipacket, int iblock);
  int getType(int ipacket, int iblock);
  int getSensorX(int ipacket, int iblock);
  int getSensorY(int ipacket, int iblock);
  int getMiniPadX(int ipacket, int iblock);
  int getMiniPadY(int ipacket, int iblock);
  int getChipID(int iArm, int iLayer, int iType, int iSx, int iSy);
  int fetchEvent(Event *evt);
  int getEventNumber(Event *evt);
  int getRunNumber(Event *evt);
  TMpcExHitMap* getHitMap();

  static int MapSx[24];
  static int MapSy[24];
 
protected:
  int fRunNumber;
  int fEventNumber;
  TMpcExHitMap* _hitmap;
};

#endif
