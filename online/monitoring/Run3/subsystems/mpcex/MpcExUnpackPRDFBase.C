#include <MpcExUnpackPRDFBase.h>
#include <TMpcExHitMap.h>
#include <Event.h>
#include <PHKey.hh>
#include <MPCEXGEOM.h>
#include <MpcExConst.h>
#include <TMpcExKeyGen.h>
#include <iostream>
#include <string.h>
#include <TRandom.h>

using namespace std;
int MpcExUnpackPRDFBase::MapSx[24] = {1, 2, 3, 0, 1, 2, 3, 4, 0, 1, 4, 5, 0, 1, 4, 5, 0, 1, 2, 3, 4, 1, 2, 3};  // simple ordering up to down, left to right
int MpcExUnpackPRDFBase::MapSy[24] = {5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0};

MpcExUnpackPRDFBase::MpcExUnpackPRDFBase()
{
  _hitmap = new TMpcExHitMap();
}

MpcExUnpackPRDFBase::~MpcExUnpackPRDFBase()
{
  if (_hitmap) delete _hitmap;
}

int MpcExUnpackPRDFBase::getArm(int ipacket, int iblock)
{
  return 0;
}

int MpcExUnpackPRDFBase::getLayer(int ipacket, int iblock)
{
  return ipacket;
}

int MpcExUnpackPRDFBase::getType(int ipacket, int iblock)
{
  return 0;
}

int MpcExUnpackPRDFBase::getSensorX(int ipacket, int iblock)
{
  int id = iblock / 128;
  int iret = MpcExUnpackPRDFBase::MapSx[id];
  return iret;
}

int MpcExUnpackPRDFBase::getSensorY(int ipacket, int iblock)
{
  int id = iblock / 128;
  int iret = MpcExUnpackPRDFBase::MapSy[id];
  return iret;
}

int MpcExUnpackPRDFBase::getMiniPadX(int ipacket, int iblock)
{
  int iret = -1;
  int residue = iblock % 128;
  if (getType(ipacket, iblock) == 0) iret = residue % 32;
  if (getType(ipacket, iblock) == 1) iret = residue % 4;
  return iret;
}

int MpcExUnpackPRDFBase::getMiniPadY(int ipacket, int iblock)
{
  int iret = -1;
  int residue = iblock % 128;
  if (getType(ipacket, iblock) == 0) iret = residue / 32;
  if (getType(ipacket, iblock) == 1) iret = residue / 4;
  return iret;
}

int MpcExUnpackPRDFBase::getChipID(int iArm, int iLayer, int iType, int iSx, int iSy)
{
  int iret = -1;
  for (int id = 0; id < 24; id++) {
    if (MpcExUnpackPRDFBase::MapSx[id] == iSx && MpcExUnpackPRDFBase::MapSy[id] == iSy) {
      iret = 24*iLayer + id;
      break;
    }
  }
  return iret;
}

int MpcExUnpackPRDFBase::fetchEvent(Event *evt)
{
  if (!evt) return 0;
//  unsigned long key;
  _hitmap->clear();
//  TMpcExHit_v1 hit;
  for (int ipacket = 0; ipacket < nPacket; ipacket++) { 
    Packet *p = evt->getPacket(PacketList[ipacket]);
    if (!p) {cout << "Packet " << PacketList[ipacket] << " is missing" << endl;  continue;}
    for(int iblock = 0; iblock < nBlock; iblock++) {
      int iarm = getArm(ipacket, iblock);
      int ilayer = getLayer(ipacket, iblock);
      int itype = getType(ipacket, iblock);
      int isx = getSensorX(ipacket, iblock);
      int isy = getSensorY(ipacket, iblock);
      int imx = getMiniPadX(ipacket, iblock);
      int imy = getMiniPadY(ipacket, iblock);
//      if (!MPCEXGEOM::CheckMiniPad(iarm, ilayer, itype, isx, isy, imx, imy)) continue;
//      key = TMpcExKeyGen::get_key(iarm, ilayer, itype, isx, isy, imx, imy);
      TMpcExHitMap::iterator hit_iter = _hitmap->get(iarm, ilayer, itype, isx, isy, imx, imy);
      if (hit_iter.at_end()) hit_iter = _hitmap->insert_new(iarm, ilayer, itype, isx, isy, imx, imy);

//      hit = hit_iter->get();
      if (iblock < nMiniPad) hit_iter->get()->set_low_q(p->iValue(iblock));  // Need actual structure of data
      if (iblock >= nMiniPad) {hit_iter->get()->set_high_q(p->iValue(iblock)); /*cout << hit_iter->get()->get_high_q() << endl;*/}
    }
  }
  return 1;
}
/*
      hit_iter = _hit_map->get(arm,layer,xy,sen_x,sen_y,minipad_x,minipad_y);

      if(!hit_iter.at_end()) {//Add energy to this minipad
	double energy = hit_iter->get()->get_low_q();
	hit_iter->get()->set_low_q(hitEnergy+energy);
	energy = hit_iter->get()->get_high_q();
	hit_iter->get()->set_high_q(hitEnergy+energy); }
      else {//insert a new hit into the map
	hit_iter = _hit_map->insert_new(arm,layer,xy,
					sen_x,sen_y,minipad_x,minipad_y);
	hit_iter->get()->set_status(TMpcExHit::RAW_SIMULATION);
	hit_iter->get()->set_low_q(hitEnergy);
	hit_iter->get()->set_high_q(hitEnergy); }
*/
int MpcExUnpackPRDFBase::getEventNumber(Event *evt)
{ 
  fEventNumber = evt->getEvtSequence();
  return fEventNumber;
}

int MpcExUnpackPRDFBase::getRunNumber(Event *evt)
{ 
  fRunNumber = evt->getRunNumber();
  return fRunNumber;
}

TMpcExHitMap* MpcExUnpackPRDFBase::getHitMap()
{ 
  return _hitmap;
}


