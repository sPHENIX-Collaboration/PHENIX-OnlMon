#ifndef __SVXPIXELMON_H__
#define __SVXPIXELMON_H__

#include <OnlMon.h>
#include <string>

#include "SvxPixelMonDefs.h"
class Event;
class OnlMonDB;
class TH1;
class TH2;
class TProfile;
class TProfile2D;

#define ifdelete(x) if(x!=NULL){ delete x;x=NULL;}

class SvxPixelMon: public OnlMon
{
 public:
  SvxPixelMon(const char *name = "MYMON");
  virtual ~SvxPixelMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();


 protected:

  int read_config_file(unsigned int data[60][8][32], const std::string fname);
  int DBVarInit();
  int evtcnt;
  int idummy;
  OnlMonDB *dbvars;

  std::string desc_arm[2];
  std::string desc_side[2];

  time_t bor_time;                  // beginning of run unix time stamp
  TProfile*   pmulti_time;          // detector wide multiplicity
  TProfile*   pmulti[vtxp_npacket]; // one for each half-ladder
  TProfile2D* pmulti2d;             // one for each half-ladder

  TH2* h2chipmap[vtxp_npacket][NCHIP];
  TH1* h1chiphit[NLADDER];
  TH1* h1detmult;
  TH2* h2map[NBARREL][2];
  TH1* h1chipcount[NBARREL][2][ntype];     // [barrel][west/east][counts,hot but ok,hot but not ok]
  TH1* vtxp_pars;

  unsigned int ig_hotc [60][8][32];//[packet][chip][column]

  const unsigned int WEST;
  const unsigned int EAST;
  const unsigned int SOUTH;
  const unsigned int NORTH;
};

#endif /* __SVXPIXELMON_H__ */
