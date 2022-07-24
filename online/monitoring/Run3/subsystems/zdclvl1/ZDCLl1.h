
#ifndef __ZDCLl1_h__
#define __ZDCLl1_h__

#include <string>

class Event;
class ZDCLut;
class OnlMon;

class ZDCLl1
{
  protected:
  const OnlMon *mymon;
  int SouthRangeLow, SouthRangeHigh; 
  int NorthRangeLow, NorthRangeHigh; 
  int VertexLimA, VertexLimB;

  int success; 

  // output values:
  int Vertex;
  bool SouthTDC_OK, NorthTDC_OK;
  bool VtxAOK, VtxBOK; 

  public:

  ZDCLut* zdcFEMLUT;
  int T2[8];
  unsigned char LL1T2[8];
  
  ZDCLl1(const OnlMon *parent);
  virtual ~ZDCLl1();

  int getDataFromZdcPacket(Event*);

  int setTDCRanges(unsigned int S_low,
		   unsigned int S_high,
		   unsigned int N_low,
		   unsigned int N_high);

  int setVertexLimits(unsigned int VlimA, unsigned int VlimB);

  int calculate();
  int getVertex(){return Vertex;};
  bool getVtxAOK(){return VtxAOK;};
  bool getVtxBOK(){return VtxBOK;};
  bool getSouthTDCOK(){return SouthTDC_OK;};
  bool getNorthTDCOK(){return NorthTDC_OK;};

  int Success() {return success;};

};

#endif



