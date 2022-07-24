#ifndef __RICHMON_H__
#define __RICHMON_H__

#include <OnlMon.h>
#include <RICHMonDefs.h>
#include <string>

class CalibrationsRead;
class Event;
class TacT0Read;
class TH1;

class RICHMon: public OnlMon
{
 public:
  RICHMon();
  virtual ~RICHMon();

  int process_event(Event *evt);
  int Reset();

 protected:
  void ring_finder(float hitmat[],TH1** ringhist);

  unsigned int  evtcnt; 
  CalibrationsRead *calread;
  TacT0Read *tact0read;


  int chanperpacket;  // The number of PMT's per packet
  int packetsperevent; // The number of packets per event
  int pmtidnum;
  float pedestal[5120],sigped[5120],pep[5120],sigpep[5120];
  float tacpedestal[5120],tacsigped[5120],tacpep[5120],tacsigpep[5120];
  float tact0[5120];
  int dead[5120];
  int tacdead[5120];
  float hitmat[5120];
  bool did_BbcLvl1_fire;
  std::string calfile;
  std::string taccalfile;
  std::string tact0file;
  int  calread_status;
  int  tact0read_status;

  TH1 *rich_eventcounter;
  TH1 *rich_adclivehist;
  TH1 *rich_tdclivehist;
  TH1 *rich_adchistsumWS;
  TH1 *rich_adchistsumWN;
  TH1 *rich_adchistsumES;
  TH1 *rich_adchistsumEN;
  TH1 *rich_adc_first;
  TH1 *rich_adc_second;
  TH1 *rich_adc_hit;
  TH1 *rich_adc_pmt;
  TH1 *rich_nhit_eventWS;
  TH1 *rich_nhit_eventWN;
  TH1 *rich_nhit_eventES;
  TH1 *rich_nhit_eventEN;
  TH1 *rich_deadchinlivehist;
  TH1 *rich_par;
  TH1 *rich_ringhist[4];
  TH1 *rich_tdchistsumWS;
  TH1 *rich_tdchistsumWN;
  TH1 *rich_tdchistsumES;
  TH1 *rich_tdchistsumEN;
  TH1 *rich_tdc_first;
  TH1 *rich_tdc_second;
  TH1 *rich_tdc_hit;
  TH1 *rich_tdc_pmt;

  TH1 *rich_nhit_eventWS1;
  TH1 *rich_nhit_eventWS2;
  TH1 *rich_nhit_eventWS3;
  TH1 *rich_nhit_eventWS4;
  TH1 *rich_nhit_eventWN1;
  TH1 *rich_nhit_eventWN2;
  TH1 *rich_nhit_eventWN3;
  TH1 *rich_nhit_eventWN4;
  TH1 *rich_nhit_eventES1;
  TH1 *rich_nhit_eventES2;
  TH1 *rich_nhit_eventES3;
  TH1 *rich_nhit_eventES4;
  TH1 *rich_nhit_eventEN1;
  TH1 *rich_nhit_eventEN2;
  TH1 *rich_nhit_eventEN3;
  TH1 *rich_nhit_eventEN4;

};

#endif /* __RICHMON_H__ */

