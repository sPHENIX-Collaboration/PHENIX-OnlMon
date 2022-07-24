#ifndef DAQVTXPMON_H__
#define DAQVTXPMON_H__

#include "GranuleMon.h"

#include <map>

class TH2;

class vtxppacketerrors
{
 public:
  vtxppacketerrors(const unsigned int ipkt = 0);
  virtual ~vtxppacketerrors() {};
  void Reset();
  unsigned int PacketId;
  int spirobadcount;
  int spiroAlosslock;
  int spiroBlosslock;
  int spiroAsizeerror;
  int spiroBsizeerror;
  int spiroparity;
  int spiroeventnum;
  int spiroclk03;
  int spiroclk47;
};

class DaqVtxpMon: public GranuleMon
{
 public:
  DaqVtxpMon(const std::string &system);
  virtual ~DaqVtxpMon() {}

  int Init();
  int BeginRun(const int runno);
  int DcmFEMParityErrorCheck(); // Compare the dcm2-calculated packet parity to the parity the FEM thinks it sent out
  void SetBeamClock(); 
  int GlinkCheck();
  int LocalEventNumberOffset() {return 0;}
  int SubsystemCheck(Event *evt, const int iwhat);
  int FillHisto();

 protected:
  DaqVtxpMon() {}
  std::map<unsigned int, vtxppacketerrors> mypkt;
  TH2* h2status;
};

#endif /* DAQVTXPMON_H__ */
