#ifndef __GL1MON_H__
#define __GL1MON_H__


#include <OnlMon.h>

#include <string>
#include <ctime>


class Event;
class TH1;
class TrigRunLvl1;
class OnlMonDB; 


class Gl1Mon: public OnlMon
{
 public:
  Gl1Mon();
  virtual ~Gl1Mon();

  int process_event(Event *evt);
  int Reset();
  int BeginRun(const int runno);
  int EndRun(const int runno);

 protected:
  int evtcnt;
  time_t LastCommit;
  OnlMonDB *dbvars;
  int DBVarInit();

  float bias1_expected_rejection[32], bias1_expected_rejection_err[32];
  float bias2_expected_rejection[32], bias2_expected_rejection_err[32];
  std::string BBCLL1_NAME, BBCLL1_NOVERTEXCUT_NAME, ZDC_NAME;
  
  double total_size;
  float trigsize[32];
  float minbias_triggers;

  long int initial_countRaw[32];
  long int initial_countLive[32];
  long int initial_countScaled[32];
  long int present_countRaw[32];
  long int present_countLive[32];
  long int present_countScaled[32];
  long int runbegin_time, initial_seconds, time_now, time, last_time;
  int scaler_init;
  float zdcRaw, bbczdcRaw, bbcRaw, zdcLive, bbczdcLive, bbcLive, zdcScaled, bbczdcScaled, bbcScaled;

  int BBCLL1_bit_mask;
  int ZDC_bit_mask;
  int BBCLL1_NOVERTEXCUT_bit_mask;

  int NumBadScaledEvents;
  int NumRawNELiveEvents;
  int NumBadScalLiveEvents;
  int NumBadScalRawEvents;

  TH1 *Gl1_RawHist;
  TH1 *Gl1_LiveHist;
  TH1 *Gl1_ScaledHist;
  TH1 *Gl1_ScaledHist_BBCLL1;
  TH1 *Gl1_ScaledHist_ZDC;
  TH1 *Gl1_ScalerRawHist;
  TH1 *Gl1_ScalerLiveHist;
  TH1 *Gl1_ScalerScaledHist;
  TH1 *Gl1_RejectHist;
  TH1 *Gl1_ExpectRejectHist;
  TH1 *Gl1_bandwidthplot;
  TH1 *Gl1_CrossCounter_bbcll1;
  TH1 *Gl1_CrossCounter_bbcll1_nvc;
  TH1 *Gl1_CrossCounter_zdc;
  TH1 *Gl1_present_count;
  TH1 *Gl1_present_rate;
  TH1 *Gl1_RejectRatio_1;
  TH1 *Gl1_RejectRatio_2;
  TH1 *Gl1_StorageHist;
  TH1 *GL1_AllCross[32];
};

#endif /* __GL1MON_H__ */

