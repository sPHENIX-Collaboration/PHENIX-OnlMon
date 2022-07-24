#ifndef __ACCMON_H__
#define __ACCMON_H__

#include "ACCMonDefs.h"
#include <OnlMon.h>

#include <set>
#include <string>

class CalibrationsRead;
class Event;
class TH1;
class TH2;

class ACCMon: public OnlMon
{
 public:
  ACCMon(const char *name = "ACCMON");
  virtual ~ACCMon();

  int process_event(Event *evt);
  int Reset();

 protected:

  void ring_finder(float hitmat[],TH1** ringhist);

  unsigned int  evtcnt; 
  CalibrationsRead *calread;


  int chanperpacket;  // The number of PMT's per packet
  int packetsperevent; // The number of packets per event
  int status;
  float pedestal[ACC_ALLCHANNEL],sigped[ACC_ALLCHANNEL],pep[ACC_ALLCHANNEL],sigpep[ACC_ALLCHANNEL];
  float tacpedestal[ACC_ALLCHANNEL],tacsigped[ACC_ALLCHANNEL],tacpep[ACC_ALLCHANNEL],tacsigpep[ACC_ALLCHANNEL];
  float hitmat[ACC_ALLCHANNEL],hittime[ACC_ALLCHANNEL];
  int dead[ACC_ALLCHANNEL];
  int tacdead[ACC_ALLCHANNEL];
  //std::string adcgainfile;
  //std::string adcpedfile;
  //std::string tacpedfile;
  int  calread_status;
  TH1 *acc_livehist;
  TH1 *acc_taclivehist;
  TH1 *acc_Nadcsum;
  TH1 *acc_Sadcsum;
  TH1 *acc_Ntacsum;
  TH1 *acc_Stacsum;
  TH1 *adc_average;
  TH1 *tac_average;
  TH1 *acc_par;
  TH1 *acc_Ntacsum_board[5];
  TH1 *acc_Stacsum_board[5];
  TH2 *acc_adc_tac_N;
  TH2 *acc_adc_tac_S;
  
};

#endif /* __ACCMON_H__ */
