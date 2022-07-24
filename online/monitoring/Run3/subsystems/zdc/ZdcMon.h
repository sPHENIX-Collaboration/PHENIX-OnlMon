#ifndef __ZDCMON_H__
#define __ZDCMON_H__

#include <OnlMon.h>
#include <ZdcMonDefs.h>
#include <ctime>

class ZdcCalib;
class ZdcEvent;
class BbcCalib;
class BbcEvent;
class Event;
class Packet;
class TH1;
class TH2;
class OnlMonDB;

class ZDCMon: public OnlMon
{
 public:
  ZDCMon();
  virtual ~ZDCMon();

  int process_event(Event *evt);

  int EndRun(const int runno);
  int Reset();

 protected:

  void CreateHistos();
  void RegisterHistos();
  void GetCalConst();
  void GetRawAdcTdc();
  void CompZdcAdc();
  void CompSmdAdc();
  void CompSmdPos();
  void CompSumSmd();
  void CompOvfBool();
  int CompTrigFire(Event *evt);
  int CommitAll();
  int DBVarInit();
  time_t lastcommit;
  int commitflag;
  Packet *p;

  ZdcEvent * zdcevent;
  ZdcCalib * zdccalib;
  BbcEvent * bbcevent;
  BbcCalib * bbccalib;

  unsigned long nevt;
  long int n_laser;
  long int n_smdpos;
  long int n_zvert;
  long int n_zadc;

  float zdcave[40];
  float smdposave[4];
  float zvertave, zadcsave, zadcnave;

  bool did_BbcLvl1_fire;
  bool did_laser_fire;
  bool did_zdcns_fire;
  bool did_zdclw_fire;
  bool did_zdcln_fire;
  bool did_zdc_north_fire; //for Run15 pA runs (2015.06.05 Minjung)
  bool did_zdc_south_fire; //for Run15 pA runs (2015.06.05 Minjung)

  // ZDC channels:
  // ADC values: packet 13001, channels 0 thru 7
  // 0     sum of the ADC values of all slabs on the South side
  // 1..3  slabs on the North side
  // 4     sum of the ADC values of all slabs on the North side
  // 5..7  slabs on the North side

  // SMD channels:
  // ADC values: packet 13001, channels 8 thru 40
  //  8..15 horizontal slabs on the South side (giving *y* coordinate)
  // 16..22 vertical slabs on the South side (giving *x* coordinate)
  // 23     sum of the ADC values of all slabs on the South side
  // 24..31 horizontal slabs on the North side (giving *y* coordinate)
  // 32..38 vertical slabs on the North side (giving *x* coordinate)
  // 39     sum of the ADC values of all slabs on the North side
  // vertical slabs are 1.5 cm wide, "vertical" slabs are in reality
  // 45 degrees from vertical
  // horizontal slabs are 2.0 cm wide


  //To monitor raw values (2015.06.05 Minjung)
  float zdcraw_adc[44]; //ch40-43: charge veto counter (POL)
  float zdcraw_tdc0[44];
  float zdcraw_tdc1[44];

  float smd_adc[32];
  float zdc_adc[8];
  float smd_sum[4]; 
  float smd_pos[4];

  
  float pedestal[40];
  float zdc_ped[8];
  float smd_ped[32];
  float gain[32];
  float smd_south_rgain[16];
  float smd_north_rgain[16];
  float overflow0[40];
  float overflow1[40];

  bool ovfbool[40];

  TH1 * zdc_adc_north;
  TH1 * zdc_adc_south;

  /*Ciprian Gal 110211*/
  /*Added adc distribution histograms for the individual channels*/
  TH1 * zdc_adc_n_ind[3];
  TH1 * zdc_adc_s_ind[3];
  /*Ciprian Gal 110211*/

  TH1 * zdc_vertex;
  TH1 * zdc_vertex_b;
  TH2 * zdc_vs_bbc;

  TH1 * smd_hor_south;
  TH1 * smd_hor_north;
  TH1 * smd_hor_north_small;
  TH1 * smd_hor_north_good;
  TH1 * smd_sum_hor_south;
  TH1 * smd_sum_hor_north;

  TH1 * smd_ver_south;
  TH1 * smd_ver_north;
  TH1 * smd_ver_north_small;
  TH1 * smd_ver_north_good;
  TH1 * smd_sum_ver_south;
  TH1 * smd_sum_ver_north;

  TH2 * zdc_hor_north;
  TH2 * zdc_ver_north;
  TH2 * smd_xy_north;
  TH2 * smd_xy_south;

  TH2 * zdc_value;
  TH2 * smd_value;
  TH2 * smd_value_good;
  TH2 * smd_value_small;

  TH2 * smd_yt_south;
  TH2 * smd_xt_south;
  TH2 * smd_yt_north;
  TH2 * smd_xt_north;

  TH2 * zdc_laser_south1;
  TH2 * zdc_laser_south2;
  TH2 * zdc_laser_south3;
  TH2 * zdc_laser_north1;
  TH2 * zdc_laser_north2;  
  TH2 * zdc_laser_north3; 

  TH2 * zdc_laser_south1b;
  TH2 * zdc_laser_south2b;
  TH2 * zdc_laser_south3b;
  TH2 * zdc_laser_north1b;
  TH2 * zdc_laser_north2b;  
  TH2 * zdc_laser_north3b; 

  TH2 * zdc_ratio_south;  
  TH2 * zdc_ratio_north; 

  //Monitor FEM healthiness, & background (2015.06.05 Minjung)
  TH2 * zdc_adc_vs_tdc[40];
  TH2 * fc_adc_vs_zdc_tdc[40];
  TH2 * rc_adc_vs_zdc_tdc[40];
  // TH1 * charge_bkg_ratio_vs_time[4];
  TH2 * veto_adc_vs_pos[8];


  int evtcnt;
  int idummy;
  OnlMonDB *dbvars;
};

#endif /* __ZDCMON_H__ */
