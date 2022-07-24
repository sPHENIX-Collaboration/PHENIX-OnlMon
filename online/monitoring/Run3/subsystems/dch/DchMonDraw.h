#ifndef __DCHMONDRAW_H__
#define __DCHMONDRAW_H__

#include "OnlMonDraw.h"
#include "DchMonDefs.h"

class TCanvas;
class TGraph;
class TH2;
class TPad;
class TPaveText;
class TStyle;

class DchMonDraw: public OnlMonDraw
{

 public: 
  DchMonDraw();
  virtual ~DchMonDraw();

  int Init();
  int Draw(const char *what = "ALL");

  int MakeHitRateCanvas();

  int DrawHitRateCanvas(); 
  
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

  int decodeInfoFromHistogram();

 private:
  double get_maximum( double a, double b, double c, double d );
  double get_minimum( double a, double b, double c, double d );
  void set_species_load_alerts();
  
 protected:

  TStyle *dchStyle;
   
  //Dch
  TCanvas * DcHitRateCanvas;     //2
 
  //canvas2
  TPad * paddc2_1;
  TPad * paddc2_2;
  TPad * paddc2_3;
  TPad * paddc2_4;


  //canvas3
  TPad * paddc3_00;
  TPad * paddc3_01;
  TPad * paddc3_10;
  TPad * paddc3_11;
  TPad * paddc3_02;
  TPad * paddc3_12;


  TPaveText * hitrate_title_pave_text;
  TPaveText * hitrate_status_pave_text;
  TPaveText * hitrate_statistics_pave_text;
  TPaveText * hitrate_legend_mask_pave_text;
  TPaveText * hitrate_legend_dead_pave_text;
  TPaveText * hitrate_legend_cold_pave_text;
  TPaveText * hitrate_legend_good_pave_text;
  TPaveText * hitrate_legend_hot_pave_text;
  TPaveText * hitrate_legend_noisy_pave_text;

  TPaveText *pt00x1;  //xuv labels
  TPaveText *pt00x2;
  TPaveText *pt00u1;
  TPaveText *pt00v1;
  TPaveText *pt00u2;
  TPaveText *pt00v2;
  TPaveText *pt01x1;
  TPaveText *pt01x2;
  TPaveText *pt01u1;
  TPaveText *pt01v1;
  TPaveText *pt01u2;
  TPaveText *pt01v2;
  TPaveText *pt10x1;
  TPaveText *pt10x2;
  TPaveText *pt10u1;
  TPaveText *pt10v1;
  TPaveText *pt10u2;
  TPaveText *pt10v2;
  TPaveText *pt11x1;
  TPaveText *pt11x2;
  TPaveText *pt11u1;
  TPaveText *pt11v1;
  TPaveText *pt11u2;
  TPaveText *pt11v2;

  TH2 *dis00;
  TH2 *dis01;
  TH2 *dis10;
  TH2 *dis11;

  
  TPaveText * padnoisehit;
  TPaveText * paddeadhit;
  TPaveText * padloadhit;
  TPaveText * padt0hit;
  TPaveText * paddvhit;
  TPaveText * padswhit;
  TPaveText * padbadev;

  TPaveText * noise_Sumtitle;
  TPaveText * dead_Sumtitle;
  TPaveText * load_Sumtitle;
  TPaveText * t0_Sumtitle;
  TPaveText * vd_Sumtitle;
  TPaveText * sw_Sumtitle; 
  TPaveText * Bad_title;   

  TH2 *scale_dead;
  TH2 *scale_noisy;
  TH2 *scale_tzero;
  TH2 *scale_vdrift;
  TH2 *scale_eff;

  //colors
  int colors[nCOLORS];

  float LLOAD_ALERT_RED;
  float LLOAD_ALERT_YEL;
  float HLOAD_ALERT_RED;
  float HLOAD_ALERT_YEL;


  int stat_threshold;

  //variables encoded in dc_info_histogram

  int nevt;

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

  int dead_report;

  int template_noisy_east_n ;
  int template_noisy_west_n ;
  int template_dead_east_n  ;
  int template_dead_west_n  ;
  int template_tzero_east   ;
  int template_tzero_west   ;
  float template_vdrift_east  ;
  float template_vdrift_west  ;
  float template_eff_east     ;
  float template_eff_west     ;
  
  

  int new_noisy_east_n ;
  int new_noisy_west_n ;
  int new_dead_east_n  ;
  int new_dead_west_n  ;
  int tzero_east   ;
  int tzero_west   ;
  float vdrift_east  ;
  float vdrift_west  ;
  float eff_east     ;
  float eff_west     ;
  int  bad_events   ;

  //int speciescode;




};

#endif /*__DCHMONDRAW_H__ */
