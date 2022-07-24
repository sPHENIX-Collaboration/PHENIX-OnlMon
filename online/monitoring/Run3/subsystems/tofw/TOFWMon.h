#ifndef __TOFWMON_H__
#define __TOFWMON_H__

#include <OnlMon.h>
#include <string>

const int TOFW_NMRPC_ALL = 128;
const int TOFW_NMRPC     = TOFW_NMRPC_ALL;

const int TOFW_NSTRIP_ALL  = 512;
const int TOFW_NSTRIP     = TOFW_NSTRIP_ALL;

const int TOFW_NCRATE    = 4;            // Number of FEM crates
const int TOFW_NBOARD    = 16;           // Number of FEM boards per crate
const int TOFW_NCHANNEL  = 16;           // Number of channels per FEM board

const float TOFW_STRIP_LENGTH = 45;   // Strip Length [cm]
const float TOFW_STRIP_WIDTH = 2.5;   // Strip Width [cm]

const float TOFW_MRPC_WIDTH  = 12.0;   // MRPC Width [cm]
const float TOFW_MRPC_LENGTH = 45.0;   // MRPC Length[cm]

class Event;
class TH1;
//class TofwAddressObject;
//class TofwCalibObject;
class TProfile;

class TOFWMon: public OnlMon
{
 public:
  TOFWMon();
  virtual ~TOFWMon(){}

  int process_event(Event *evt);
  int Reset();

 private:
  int tofwQc1[2][TOFW_NSTRIP];
  int tofwQc2[2][TOFW_NSTRIP];
  int tofwTc3[2][TOFW_NSTRIP];
  int tofwTc4[2][TOFW_NSTRIP];

  int tofwQcnew1[2][4][128];
  int tofwQcnew2[2][4][128];
  int tofwTcnew3[2][4][128];
  int tofwTcnew4[2][4][128];

  int cutTofwQVC;
  int cutTofwTc_0;
  int cutTofwTc;
  int debug;
  int evtcnt;
  int evtTrigIn;


  float cutTofwChg;
  float cutTofwChg_low;
  float cutTofwChg_high;

  TH1 *htofwinfo;
  TH1 *htofw210;
  TH1 *htofw220;
  TH1 *htofw230;
  TH1 *htofw420;
  TProfile *htofw530;

  //TofAddressObject *tofwmonaddress; 
  //TofCalibObject   *tofwmoncalib;


};

#endif /* __TOFWMON_H__ */

