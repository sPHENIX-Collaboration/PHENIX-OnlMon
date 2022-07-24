#ifndef __TOFMON_H__
#define __TOFMON_H__

#include <Tof.hh>
#include <OnlMon.h>

class Event;
class TH1;
class TofAddressObject;
class TofCalibObject;
class TProfile;

class TOFMon: public OnlMon
{
 public:
  TOFMon();
  virtual ~TOFMon();
  int process_event(Event *evt);
  int Reset();

 private:

  int tofQc1_[2][TOF_NSLAT];
  int tofQc2_[2][TOF_NSLAT];
  int tofTc3_[2][TOF_NSLAT];
  int tofTc4_[2][TOF_NSLAT];

  int cutTofQVC;
  int cutTofTc_0;
  int cutTofTc;
  int debug;
  int evtcnt;
  int evtTrigIn;


  float cutTofChg;
  float cutTofChg_low;
  float cutTofChg_high;

  TH1 *htofinfo;
  TH1 *htof210;
  TH1 *htof220;
  TH1 *htof230;
  TH1 *htof420;
  TProfile *htof530;

  TofAddressObject *tofmonaddress;
  TofCalibObject *tofmoncalib;


};

#endif /* __TOFMON_H__ */

