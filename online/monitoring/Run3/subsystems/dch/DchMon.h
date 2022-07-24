#ifndef DCHMON_H__
#define DCHMON_H__

#include "DchMonDefs.h"
#include <OnlMon.h>

#include <string>

class Event;
class TH1;
class TH2;

class DchMon: public OnlMon
{
 public:
  DchMon(const char *name = "DCHMON");
  virtual ~DchMon();
  int BeginRun(const int runno);
  int process_event(Event *evt);
  int Init();
  void HistoInit();
  int init_wire_positions();
  int Reset();

  int read_dead();
  int write_dead();
  int read_noise();
  int write_noise();
  int read_templates();
  int write_templates();
  void print_templates();

  void set_averages_on_the_fly(const int value) { calculateAveragesOnTheFly = value; }
 
  void set_noise_threshold(const float value) { noise_threshold = value; }
  void set_dead_threshold (const float value) { dead_threshold  = value; }
  void set_stat_threshold (const unsigned int value)   { stat_threshold  = value; }
  void set_update_rate(const unsigned int value) { update_rate = value; }
  void set_HIT_PARAMS(const float v1, const float v2, const float v3, const float v4, const float v5, const float v6, const float v7) { 	 
    SIGMA_HITRATE = v1; 	 
    LOW_GOOD = v2; 	 
    HIGH_GOOD = v3; 	 
    LOW_HOT = v4; 	 
    HIGH_HOT = v5; 	 
    LOW_COLD = v6; 	 
    HIGH_COLD = v7; 	 
  } //DLAN 	 
  void set_ALERT_PARAMS(const float v1, const float v2, const float v3, const float v4) { 	 
    LLOAD_ALERT_RED = v1; 	 
    HLOAD_ALERT_RED = v2; 	 
    LLOAD_ALERT_YEL = v3; 	 
    HLOAD_ALERT_YEL = v4; 	 
  } //DLAN  
  float get_noise_threshold() const { return noise_threshold; }
  float get_dead_threshold () const { return dead_threshold;  }
  unsigned int get_stat_threshold () const { return stat_threshold;  }

  void set_running_average(const int value) { running_average = value; }
  void reset_arrays_histograms();

  void write_all_files();

 protected:
  int CALCULATRIX();  
  
  void calculate_density_averages();
  int normalize_densities();
  int calculate_densities();

  int calculate_efficiencies();
  
  int update_new_noisy_dead();
  int check_dead_noisy_alerts();
  void count_noise(int a,int s, int &num1, int &num2, int &num3, int &num4);

  int update_t0();

  int update_drift_velocities();

  void encodeInfoToHistogram();

  int calculateAveragesOnTheFly;

  std::string directory;
  std::string species;
  int   running_average;
  int   dead_report;
  int   runNum;
  int   infogroup;
  int   old_noisy_n;
  int   new_noisy_n;
  int   old_dead_n;
  int   new_dead_n;
  float noise_threshold;
  float dead_threshold;
  unsigned int   stat_threshold;
  int   tzero_east;
  int   tzero_west;
  float vdrift_east;
  float vdrift_west;
  float eff_east;
  float eff_west;
  int   bad_events   ;
  int   new_noisy_east_n;
  int   new_noisy_west_n;
  int   new_dead_east_n;
  int   new_dead_west_n;

  
  unsigned int update_rate;   	// DLAN
  float SIGMA_HITRATE;  	// All the new vars to store in DchTemplates.txt 
  float LOW_GOOD;		//
  float HIGH_GOOD;
  float LOW_HOT;
  float HIGH_HOT;
  float LOW_COLD;
  float HIGH_COLD;
  
  
 
  float template_LLOAD_ALERT_RED; //DLAN
  float template_LLOAD_ALERT_YEL; // These are read from the template and 
  float template_HLOAD_ALERT_RED; // encoded in dc_info histo to be picked up
  float template_HLOAD_ALERT_YEL; // in DchMonDraw.C


  int template_noisy_east_n;
  int template_noisy_west_n;
  int template_dead_east_n;
  int template_dead_west_n;
  int template_tzero_east;
  int template_tzero_west;
  float template_vdrift_east;
  float template_vdrift_west;
  float template_eff_east;
  float template_eff_west;

  
  int template_running_average; 	//DLAN 
  int template_update_rate;     	//
  unsigned int template_stat_threshold; // The new vars to be read out from DchTemplates.txt
  float template_dead_threshold;	//
  float template_noise_threshold;
  float template_SIGMA_HITRATE;
  float template_LOW_GOOD;
  float template_HIGH_GOOD;
  float template_LOW_HOT;
  float template_HIGH_HOT;
  float template_LOW_COLD;
  float template_HIGH_COLD;
 
  float LLOAD_ALERT_RED;  //DLAN
  float LLOAD_ALERT_YEL;  // These are written to template
  float HLOAD_ALERT_RED;
  float HLOAD_ALERT_YEL;

 
  bool  report1;
  bool  report2;
  
  /////////////////
  //data storage
  /////////////////
  
  //the bulk of data
  int hits_array[NUMARM][NUMSIDE][NUMPLANE][NUMBOARD];
  int firing[NUMARM][NUMSIDE][NUMBOARD][NUMPLANE];
  int time1[NUMARM][NUMSIDE][NUMBOARD][NUMPLANE];

  //noise arrays: old means the one read from template files, new is the one caclulated actively
  int old_noise_array[NOISEARRAYSIZE];
  int new_noise_array[NOISEARRAYSIZE];

  //dead arrays
  int old_dead_array[DEADARRAYSIZE];
  int new_dead_array[DEADARRAYSIZE];

  //hit rate averages
  float hitrate_x1x2average[NUMARM][NUMSIDE];
  float hitrate_uv1average[NUMARM][NUMSIDE];
  float hitrate_u2average[NUMARM][NUMSIDE];
  float hitrate_v2average[NUMARM][NUMSIDE];
  TH2 *s00_average;
  TH2 *s01_average;
  TH2 *s10_average;
  TH2 *s11_average;

  //unnormalized efficiencies
  TH1 *ek1, *dchek1, *dch_ek1_1, *tmp_ek1_1;
  TH1 *ek2, *dchek2, *dch_ek2_2, *tmp_ek2_2;
  TH1 *ek3, *dchek3, *dch_ek3_3, *tmp_ek3_3;
  TH1 *ek4, *dchek4, *dch_ek4_4, *tmp_ek4_4;

  //unormalized timing distributions
  TH1 *dc_h_dc1;
  TH1 *dc_h_dc2;

  //drift velocity
  float WireWidth[40];
  TH1 *driftvelocity_east;
  TH1 *driftvelocity_west;

  //densities
  TH2 *dc_dens_00;
  TH2 *dc_dens_01;
  TH2 *dc_dens_10;
  TH2 *dc_dens_11;
  TH1 *h_dc5;
  
  //send info to drawing client
  TH1 *dc_info;
  TH1 *dc_good_distro;
  TH1 *dc_dead_distro;
  TH1 *dc_cold_distro;
  TH1 *dc_noisy_distro;

  TH1 *dc_hot_distro;

  //timing
  TH1 *h_dc1norm;
  TH1 *h_dc2norm;

  //Info encoded to the histogram dc_info
  int old_noise_num_00;
  int old_noise_num_01;
  int old_noise_num_10;
  int old_noise_num_11;
  int new_noise_num_00;
  int new_noise_num_01;
  int new_noise_num_10;
  int new_noise_num_11;
  int missing_noise_num_00;
  int missing_noise_num_01;
  int missing_noise_num_10;
  int missing_noise_num_11;
  int hit_num_00;
  int hit_num_01;
  int hit_num_10;
  int hit_num_11;
  float avg_load_00;
  float avg_load_10;
  float avg_load_01;
  float avg_load_11;



  unsigned int nevt;
  unsigned int NCOUNTS;
  unsigned int ncalls;
  unsigned int UPDATE_RATE;

};

#endif /* __DCHMON_H__ */
