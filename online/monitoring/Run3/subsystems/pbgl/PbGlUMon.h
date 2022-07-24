#ifndef __PBGLUMON_H__
#define __PBGLUMON_H__

#include <OnlMon.h>
#include <string>

//#define max_fix_time 4000
#define max_fix_time 5000
#define min_rep_fix_time 3000
#define max_time 1000
// the #define thrashold collided with root TH2
const unsigned int threshold = 3;
#define HIVOC_IP "10.20.32.102"
#define HV_FILE "/home/phnxpbgl/pbgl_HV/HIVOC/actual/hv_adjustedRun5_IY.dat"
#define number_of_signals 3


class Event;
class PbGlUDBodbc;
class TH1;
class TH2;

class PbGlUMon: public OnlMon
{
 public:
  PbGlUMon(const char *name = "PBGLUMON");
  virtual ~PbGlUMon();

  int process_event(Event *evt);
  int Init();
  void setDebugMode(const int mode=1){DEBUGPBGLU=mode;return;};
  void setWriteDBMode(const int mode=1){WRITEDB=mode;return;}
  void setAction(const int mode=1){if(mode)NO_ACTION=0;else NO_ACTION=1;return;};


 protected:

  

  int nomercy(const int ch);
  int check_signal(const int ch, const int time, const int signal);	
  void hv_init();
  int PbGlGeom(int index1,int& sector,int &zrow,int &yrow,int &femId);
  // ### desription of signals of U-signal

  int sig_weight[number_of_signals];

  // ### description of treatment condition

  int stor[9216][threshold*2][number_of_signals];
  int cure_time[9216];
  int skip[9216];
  int bbads[9216];

  int fhvinit;
  int hvs[9216];
  int units[9216];
  int nunits[9216];

  int ped_count;
  // histos for internal use
  TH2* he;
  float fne;

  //histos for registering
  TH2* hese;
  TH2* hcese;
  TH2* hbese;

  int WRITEDB;
  int NO_ACTION;
  int DEBUGPBGLU;
  int hv_cycle_enabled;
  PbGlUDBodbc *db;

};

#endif /* __PBGLUMON_H__ */

