////////////////////////////////////////////////
//                                            //
// BBC Lvl1 Monitoring method                 // 
// Authors: S. Belikov (belikov@bnl.gov)      //
//          P. Constantin (pcon@iastate.edu)  //
//                                            // 
////////////////////////////////////////////////

#ifndef __BbcLl1_h__
#define __BbcLl1_h__

#include <Event.h>
#include <Eventiterator.h>
#include <fileEventiterator.h>
#include <packet.h>
#include <string>
#include "BbcLUT.h"

const int NumberOfPmt=128;
class OnlMon;

class BbcLl1{

  protected:
  const OnlMon *mymon; // pointer to my monitor
  int maskN[64];
  int maskS[64];
  int rangeN[2][8];
  int rangeS[2][8];
  int MultLimits[2][2];
  int BbcLut[32];
  int VertexLow, VertexHigh;
  int VertexLow2, VertexHigh2;

  int success;
  char filename[50];

  int SouthMultOK, NorthMultOK, VertexOK, Vertex2OK; 

  // output values:
  int BeamCntr0;
  int BeamCntr1;
  int nHitPmt[2];
  int ZVertex, TimeAverage;
  int RB11, BBRB2;
  int VertexOut, AverageOut;
  int SHitsOut, NHitsOut;
  int HitsInOut, BBLL1RBOut;
  int VertexOKOut;
  int ArmHitTime[2];


 public:

  BbcLUT* bbcFEMLUT;
  int T2[128];
  int* T2S;
  int* T2N;
  unsigned char LL1T2[128];
  unsigned char* LL1T2S;
  unsigned char* LL1T2N;
  int chann_stat[128];
  
  BbcLl1(const char*filename,const OnlMon *mymonitor);
  virtual ~BbcLl1();

  int getDataFromBbcPacket(Event*);
  int getDataFromLL1Packet(Event*);
  int getDataFromGL1Packet(Event*);

  int getNorthMask(char*);
  int getSouthMask(char*);
  int getNorthRange(char*);
  int getSouthRange(char*);
  int getMultiplicityLimits(char*);
  int getVertexLimits(char*);
  int getBbcLut(char*);

  int calculate();

  int Success() {return success;};
  int getMultiplicitySouth() {return nHitPmt[0];};
  int getMultSouthOut() {return SHitsOut;};
  int getMultiplicityNorth() {return nHitPmt[1];};
  int getMultNorthOut() {return NHitsOut;};
  int getZVertex() {return ZVertex;};
  int getVertexOut() {return VertexOut;};
  int getTimeAverage() {return TimeAverage;};
  int getTimeAveOut() {return AverageOut;};
  int getRB11() {return RB11;};
  int getRB11Out() {return HitsInOut;};
  int getBBRB2() {return BBRB2;};
  int getBBRB2Out() {return BBLL1RBOut;};
  int getVertexOK() {return VertexOK;};
  int getVertex2OK() {return Vertex2OK;};
  int getVtxOKOut() {return VertexOKOut;};
  int getBeamCntr0() {return BeamCntr0;};
  int getBeamCntr1() {return BeamCntr1;};
  int getArmTimeSouth() {return ArmHitTime[0];};
  int getArmTimeNorth() {return ArmHitTime[1];};

};

#endif



