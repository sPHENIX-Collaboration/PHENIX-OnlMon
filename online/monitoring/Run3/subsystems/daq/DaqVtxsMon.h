#ifndef DAQVTXSMON_H__
#define DAQVTXSMON_H__

#include "GranuleMon.h"

#include <map>

class TH2;

class vtxspacketerrors
{
 public:
  vtxspacketerrors(const unsigned int ipkt = 0);
  virtual ~vtxspacketerrors() {}
  void Reset();
  unsigned int PacketId;
  int ldtbtimeout;
  int noactivercc;
  int rcc[6];
  int bclk[6];
  int stuck_bclk[6];
  int disabled_bclk[6];
  int hybrid[6][4];
  int cellid[6][12];
  int invalid_cellid[6][12];
  int disabled_cellid[6][12];
  int stuck_cellid[6][12];
};


class DaqVtxsMon: public GranuleMon
{
 public:
  DaqVtxsMon(const std::string &system);
  virtual ~DaqVtxsMon() {}

  int Init();
  int BeginRun(const int runno);

  int DcmFEMParityErrorCheck();
  void SetBeamClock(); 
  int GlinkCheck();
  unsigned int LocalEventCounter();
  int SubsystemCheck(Event *evt, const int iwhat);
  int FillHisto();
  void identify(std::ostream& out = std::cout) const;
  int ReadFeedConfig();

 protected:
  DaqVtxsMon() {}
  std::map<unsigned int, vtxspacketerrors> mypkt;
  TH2* h2status;
  int last_bclk[6];
  int last_cellid[6][12];
  int rccmasked[40][6];
  int hybridmask[40][6][4];
  unsigned int ipktmin;
  unsigned int ipktmax;
  int nsubsystemerrors;
  std::map<unsigned int, int> numrccs;
  std::map<std::string, unsigned int> ladderpktmap;
  
};

#endif /* DAQVTXSMON_H__ */
