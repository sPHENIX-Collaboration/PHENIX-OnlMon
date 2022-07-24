#include "DchMon.h"

#include <OnlMonServer.h>

#include <Event.h>
#include <EventTypes.h>
#include <msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

//#define DEBUG

DchMon::DchMon(const char *name): 
  OnlMon(name),
  //Default thresholds for p-p run
  calculateAveragesOnTheFly(0),
  running_average(1),
  noise_threshold(0.5),
  dead_threshold(0.001),
  stat_threshold(300),
  update_rate(1000),
  old_noise_num_00(0),
  old_noise_num_01(0),
  old_noise_num_10(0),
  old_noise_num_11(0),
  new_noise_num_00(0),
  new_noise_num_01(0),
  new_noise_num_10(0),
  new_noise_num_11(0),
  missing_noise_num_00(0),
  missing_noise_num_01(0),
  missing_noise_num_10(0),
  missing_noise_num_11(0),
  hit_num_00(0),
  hit_num_01(0),
  hit_num_10(0),
  hit_num_11(0),
  avg_load_00(0),
  avg_load_10(0),
  avg_load_01(0),
  avg_load_11(0),

  nevt(0),
  NCOUNTS(0),
  ncalls(1)
{
  if (getenv("DCHFILESDIR"))
    {
      directory = getenv("DCHFILESDIR");
      directory += "/";
    }
  else
    {
      directory = "./dch_files/";
    }


  printf("DchMon::Input directory for template files: %s\n", directory.c_str());


  //variables initialization
  dead_report = 0;
  infogroup = 0;
  runNum = 0;
  old_noisy_n = 0;
  new_noisy_n = 0;
  old_dead_n = 0;
  new_dead_n = 0;
  report1 = 0;
  report2 = 0;
  tzero_east = 0;
  tzero_west = 0;
  vdrift_east = 0;
  vdrift_west = 0;
  eff_east = 0;
  eff_west = 0;
  new_noisy_east_n = 0;
  new_noisy_west_n = 0;
  new_dead_east_n = 0;
  new_dead_west_n = 0;
  bad_events = 0;

  running_average = 1;  //DLAN
  update_rate = 0;	//Have to initialize all these
  stat_threshold = 0;   //things that are added to template
  dead_threshold = 0;
  noise_threshold = 0;
  SIGMA_HITRATE = 0;
  LOW_GOOD = 0;
  HIGH_GOOD = 0;
  LOW_HOT = 0;
  HIGH_HOT = 0;
  LOW_COLD = 0;
  HIGH_COLD = 0;
  LLOAD_ALERT_RED = 0;
  LLOAD_ALERT_YEL = 0;
  HLOAD_ALERT_RED = 0;
  HLOAD_ALERT_YEL = 0;


  template_noisy_east_n = 0;
  template_noisy_west_n = 0;
  template_dead_east_n = 0;
  template_dead_west_n = 0;
  template_tzero_east = 0;
  template_tzero_west = 0;
  template_vdrift_east = 0;
  template_vdrift_west = 0;
  template_eff_east = 0;
  template_eff_west = 0;

  template_running_average = 0; //DLAN
  template_update_rate = 0;	//Have to initialize all these
  template_stat_threshold = 0;  // things that are added to template
  template_dead_threshold = 0;
  template_noise_threshold = 0;
  template_SIGMA_HITRATE = 0;
  template_LOW_GOOD = 0;
  template_HIGH_GOOD = 0;
  template_LOW_HOT = 0;
  template_HIGH_HOT = 0;
  template_LOW_COLD = 0;
  template_HIGH_COLD = 0;
  template_LLOAD_ALERT_RED = 0;
  template_LLOAD_ALERT_YEL = 0;
  template_HLOAD_ALERT_RED = 0;
  template_HLOAD_ALERT_YEL = 0;



  //additional helping histograms for hit rates
  s00_average = new TH2F("s00", "0.0", 100, -0.5, 99.5, 46, -0.5, 45.5);
  s01_average = new TH2F("s01", "0.1", 100, -0.5, 99.5, 46, -0.5, 45.5);
  s10_average = new TH2F("s10", "1.0", 100, -0.5, 99.5, 46, -0.5, 45.5);
  s11_average = new TH2F("s11", "1.1", 100, -0.5, 99.5, 46, -0.5, 45.5);

  init_wire_positions();
  HistoInit();

}

DchMon::~DchMon()
{
  delete s00_average;
  delete s01_average;
  delete s10_average;
  delete s11_average;
  return;
}

int
DchMon::init_wire_positions()
{

  // from DchGeometry.info2001
  WireWidth[0] = ALLEY_WIDTH_RADIANS * 204.55;
  WireWidth[1] = ALLEY_WIDTH_RADIANS * 205.15;
  WireWidth[2] = ALLEY_WIDTH_RADIANS * 205.75;
  WireWidth[3] = ALLEY_WIDTH_RADIANS * 206.35;
  WireWidth[4] = ALLEY_WIDTH_RADIANS * 206.95;
  WireWidth[5] = ALLEY_WIDTH_RADIANS * 207.55;
  WireWidth[6] = ALLEY_WIDTH_RADIANS * 208.15;
  WireWidth[7] = ALLEY_WIDTH_RADIANS * 208.75;
  WireWidth[8] = ALLEY_WIDTH_RADIANS * 209.35;
  WireWidth[9] = ALLEY_WIDTH_RADIANS * 209.95;
  WireWidth[10] = ALLEY_WIDTH_RADIANS * 210.55;
  WireWidth[11] = ALLEY_WIDTH_RADIANS * 211.15;
  WireWidth[12] = ALLEY_WIDTH_RADIANS * 214.85 * 0.99942;
  WireWidth[13] = ALLEY_WIDTH_RADIANS * 215.45 * 0.99942;
  WireWidth[14] = ALLEY_WIDTH_RADIANS * 216.05 * 0.99942;
  WireWidth[15] = ALLEY_WIDTH_RADIANS * 216.65 * 0.99942;
  WireWidth[16] = ALLEY_WIDTH_RADIANS * 220.35 * 0.99942;
  WireWidth[17] = ALLEY_WIDTH_RADIANS * 220.95 * 0.99942;
  WireWidth[18] = ALLEY_WIDTH_RADIANS * 221.55 * 0.99942;
  WireWidth[19] = ALLEY_WIDTH_RADIANS * 222.15 * 0.99942;
  WireWidth[20] = ALLEY_WIDTH_RADIANS * 225.85;
  WireWidth[21] = ALLEY_WIDTH_RADIANS * 226.45;
  WireWidth[22] = ALLEY_WIDTH_RADIANS * 227.05;
  WireWidth[23] = ALLEY_WIDTH_RADIANS * 227.65;
  WireWidth[24] = ALLEY_WIDTH_RADIANS * 228.25;
  WireWidth[25] = ALLEY_WIDTH_RADIANS * 228.85;
  WireWidth[26] = ALLEY_WIDTH_RADIANS * 229.45;
  WireWidth[27] = ALLEY_WIDTH_RADIANS * 230.05;
  WireWidth[28] = ALLEY_WIDTH_RADIANS * 230.65;
  WireWidth[29] = ALLEY_WIDTH_RADIANS * 231.25;
  WireWidth[30] = ALLEY_WIDTH_RADIANS * 231.85;
  WireWidth[31] = ALLEY_WIDTH_RADIANS * 232.45;
  WireWidth[32] = ALLEY_WIDTH_RADIANS * 236.15 * 0.99942;
  WireWidth[33] = ALLEY_WIDTH_RADIANS * 236.75 * 0.99942;
  WireWidth[34] = ALLEY_WIDTH_RADIANS * 237.35 * 0.99942;
  WireWidth[35] = ALLEY_WIDTH_RADIANS * 237.95 * 0.99942;
  WireWidth[36] = ALLEY_WIDTH_RADIANS * 241.65 * 0.99942;
  WireWidth[37] = ALLEY_WIDTH_RADIANS * 242.25 * 0.99942;
  WireWidth[38] = ALLEY_WIDTH_RADIANS * 242.85 * 0.99942;
  WireWidth[39] = ALLEY_WIDTH_RADIANS * 243.45 * 0.99942;

  return 0;
}




/////////////////////////////////////
// Initialization
////////////////////////////////////

int DchMon::Init()
{
  //Zero hits_array
  memset(hits_array, 0, sizeof(hits_array));

  //Zero old_noise_array
  memset(old_noise_array, 0, sizeof(old_noise_array));

  read_noise();

  //Zero old_dead_array
  memset(old_dead_array, 0, sizeof(old_dead_array));

  read_dead();

  //zero average hit densities arrays
  //intitialize the arrays
  memset(hitrate_x1x2average, 0, sizeof(hitrate_x1x2average));
  memset(hitrate_uv1average, 0, sizeof(hitrate_uv1average));
  memset(hitrate_u2average, 0, sizeof(hitrate_u2average));
  memset(hitrate_v2average, 0, sizeof(hitrate_v2average));
  read_templates();

  return 0;
}


// Read template at the beginning of each run.
int DchMon::BeginRun(const int runno)
{
  read_noise();
  read_dead();
  read_templates();
  return 0;
}



void DchMon::HistoInit()
{

  ////////////////////////////////////////////////////////////////////////////
  //Histograms that we register to the server  (will be drawn by DchMonDraw)
  ////////////////////////////////////////////////////////////////////////////

  cout << "initializing histos" <<endl;

  //channel by channel density, normalized hit rate, thats what the user sees
  dc_dens_00 = new TH2F("dc_dens_00", "0.0", 100, -0.5, 99.5, 46, -0.5, 45.5);
  dc_dens_01 = new TH2F("dc_dens_01", "0.1", 100, -0.5, 99.5, 46, -0.5, 45.5);
  dc_dens_10 = new TH2F("dc_dens_10", "1.0", 100, -0.5, 99.5, 46, -0.5, 45.5);
  dc_dens_11 = new TH2F("dc_dens_11", "1.1", 100, -0.5, 99.5, 46, -0.5, 45.5);

  //global density histogram
  h_dc5 = new TH1F("h_dc5", "Hit Density", 50, -0.5, 4.5);
  h_dc5->SetFillColor(9);

  //normalized time distributions
  h_dc1norm = new TH1F("h_dc1norm", "Normalized Time Distribution DC.E", 800, 0, 800);
  h_dc1norm->SetFillColor(3);
  h_dc1norm->SetStats(kFALSE);

  h_dc2norm = new TH1F("h_dc2norm", "Normalized Time Distribution DC.W", 800, 0, 800);
  h_dc2norm->SetFillColor(3);
  h_dc2norm->SetStats(kFALSE);

  //efficiency histograms
  tmp_ek1_1 = new TH1F("tmp_ek1_1", "East X1 Efficiency", 1280, -0.5, 1279.5);
  dch_ek1_1 = new TH1F("dch_ek1_1", "East X1 Efficiency", 1280, -0.5, 1279.5);
  dch_ek1_1->SetMaximum(1.0);
  dch_ek1_1->SetMinimum(0.3);
  dch_ek1_1->SetLineWidth(3);
  dch_ek1_1->SetFillColor(3);

  tmp_ek2_2 = new TH1F("tmp_ek2_2", "East X2 Efficiency", 1280, -0.5, 1279.5);
  dch_ek2_2 = new TH1F("dch_ek2_2", "East X2 Efficiency", 1280, -0.5, 1279.5);
  dch_ek2_2->SetMaximum(1.0);
  dch_ek2_2->SetMinimum(0.3);
  dch_ek2_2->SetLineWidth(3);
  dch_ek2_2->SetFillColor(3);

  tmp_ek3_3 = new TH1F("tmp_ek3_3", "West X1 Efficiency", 1280, -0.5, 1279.5);
  dch_ek3_3 = new TH1F("dch_ek3_3", "West X1 Efficiency", 1280, -0.5, 1279.5);
  dch_ek3_3->SetMaximum(1.0);
  dch_ek3_3->SetMinimum(0.3);
  dch_ek3_3->SetLineWidth(3);
  dch_ek3_3->SetFillColor(3);

  tmp_ek4_4 = new TH1F("tmp_ek4_4", "West X2 Efficiency", 1280, -0.5, 1279.5);
  dch_ek4_4 = new TH1F("dch_ek4_4", "West X2 Efficiency", 1280, -0.5, 1279.5);
  dch_ek4_4->SetMaximum(1.0);
  dch_ek4_4->SetMinimum(0.3);
  dch_ek4_4->SetLineWidth(3);
  dch_ek4_4->SetFillColor(3);

  //efficiency histograms

  //hit rate averages in color
  dc_good_distro = new TH1F("dc_good_distro", "good", 400, -2., 2.);
  dc_good_distro->SetLineColor( GOODCOL );
  dc_good_distro->SetFillColor( GOODCOL );
  //dc_good_distro->SetLineColor( kGreen );
  //dc_good_distro->SetFillColor( kGreen );
  dc_good_distro->SetStats(kFALSE);
  dc_good_distro->SetTitle("");

  dc_noisy_distro = new TH1F("dc_noisy_distro", "noisy", 400, -2., 2.);
  dc_noisy_distro->SetLineColor( NOISYCOL );
  dc_noisy_distro->SetFillColor( NOISYCOL );
  //dc_noisy_distro->SetLineColor( kRed );
  //dc_noisy_distro->SetFillColor( kRed );
  dc_noisy_distro->SetStats(kFALSE);
  dc_noisy_distro->SetTitle("");

  dc_dead_distro = new TH1F("dc_dead_distro", "dead", 400, -2., 2.);
  dc_dead_distro->SetLineColor( DEADCOL );
  dc_dead_distro->SetFillColor( DEADCOL );
  //dc_dead_distro->SetLineColor( kBlack );
  //dc_dead_distro->SetFillColor( kBlack );
  dc_dead_distro->SetStats(kFALSE);
  dc_dead_distro->SetTitle("");


  //cout <<"initialized some histos"<<endl;

  dc_hot_distro = new TH1F("dc_hot_distro", "hot", 400, -2., 2.);
  dc_hot_distro->SetLineColor( HOTCOL );
  dc_hot_distro->SetFillColor( HOTCOL );
  //dc_hot_distro->SetLineColor( kViolet );
  //dc_hot_distro->SetFillColor( kViolet );
  dc_hot_distro->SetStats(kFALSE);
  dc_hot_distro->SetTitle("");

  dc_cold_distro = new TH1F("dc_cold_distro", "cold", 400, -2., 2.);
  dc_cold_distro->SetLineColor( COLDCOL );
  dc_cold_distro->SetFillColor( COLDCOL );
  // dc_cold_distro->SetLineColor( kBlue );
  //dc_cold_distro->SetFillColor( kBlue );
  dc_cold_distro->SetStats(kFALSE);
  dc_cold_distro->SetTitle("");


  //information sent to the drawing client DchMonDraw
  dc_info = new TH1F("dc_info", "Encoded Drift Chamber Information", NDC_INFO_BIN - 1, -0.5, 44.5);


  //Registration of the histograms to the SERVER
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(this,h_dc1norm);
  Onlmonserver->registerHisto(this,h_dc2norm);
  Onlmonserver->registerHisto(this,h_dc5);

  Onlmonserver->registerHisto(this,dch_ek1_1);
  Onlmonserver->registerHisto(this,dch_ek2_2);
  Onlmonserver->registerHisto(this,dch_ek3_3);
  Onlmonserver->registerHisto(this,dch_ek4_4);

  Onlmonserver->registerHisto(this,dc_info);
  Onlmonserver->registerHisto(this,dc_dens_00);
  Onlmonserver->registerHisto(this,dc_dens_01);
  Onlmonserver->registerHisto(this,dc_dens_10);
  Onlmonserver->registerHisto(this,dc_dens_11);
  Onlmonserver->registerHisto(this,dc_good_distro);
  Onlmonserver->registerHisto(this,dc_dead_distro);
  Onlmonserver->registerHisto(this,dc_noisy_distro);
  Onlmonserver->registerHisto(this,dc_hot_distro);
  Onlmonserver->registerHisto(this,dc_cold_distro);



  //////////////////////////////////////////////////////////////////////
  //GLOBAL HISTOGRAMS (HELPERS) THAT WE DONT REGISTER TO THE SERVER
  //////////////////////////////////////////////////////////////////////
  //additional helping histograms for timing distributions
  dc_h_dc1 = new TH1F("dc_h_dc1", "Time Distribution DC.E", 800, 0, 800);
  dc_h_dc2 = new TH1F("dc_h_dc2", "Time Distribution DC.W", 800, 0, 800);
  //additional helping histograms for efficiencies
  dchek1 = new TH1F("dchek1", "dchek1", 1280, -0.5, 1279.5);
  dchek2 = new TH1F("dchek2", "dchek2", 1280, -0.5, 1279.5);
  dchek3 = new TH1F("dchek3", "dchek3", 1280, -0.5, 1279.5);
  dchek4 = new TH1F("dchek4", "dchek4", 1280, -0.5, 1279.5);
  ek1 = new TH1F("ek1", "ek1", 20, -0.5, 19.5);
  ek2 = new TH1F("ek2", "ek2", 20, -0.5, 19.5);
  ek3 = new TH1F("ek3", "ek3", 20, -0.5, 19.5);
  ek4 = new TH1F("ek4", "ek4", 20, -0.5, 19.5);
  //additional helping histograms for drift velocity
  driftvelocity_east = new TH1F("driftvelocity_east", "DV Distribution DC.E", 200, 0, 300);
  driftvelocity_west = new TH1F("driftvelocity_west", "DV Distribution DC.W", 200, 0, 300);

}


///////////////////////////////////////////
//  read and write noise
///////////////////////////////////////////


int DchMon::read_noise()
{
  int index, arm, side, plane, board;
  float noise;

  FILE *infile = 0;


  string filename = directory + "DchNoise.txt";
  infile = fopen(filename.c_str(), "r");

  if (!infile)
    {
      ostringstream msg;
      msg << "Noisy channel file could not be opened " ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_ERROR, msg.str(), 1);
      return 1;
    }

  int nnoisy = 0;
  while (!feof(infile))
    {
      fscanf(infile, "%d %d %d %d %d %f", &index, &arm, &side, &plane, &board, &noise);
      if (!feof(infile))
        {
          if (nnoisy < NOISEARRAYSIZE)
            {
              old_noise_array[nnoisy] = index;
              nnoisy++;
            }
          else
            {
              ostringstream msg;
              msg << "DCH: Noise Arrays size too small need: " << nnoisy ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_ERROR, msg.str(), 1);
            }
        }
    }

  fclose(infile);
  old_noisy_n = nnoisy;
  printf("Starting monitoring with %d noisy channels in both chambers, as read from file.\n", old_noisy_n);
  return 0;
}




int DchMon::write_noise()
{
  if (nevt < 2*stat_threshold + 14)
    {
      printf("Not enough statistics gathered. I refuse to write.... Run a few more events, at least %d\n", 2*stat_threshold + 14);
      return 1;
    }

  string filename;
  filename = directory + "DchNoise.txt";
  FILE *outfile = fopen(filename.c_str(), "w");

  if (!outfile)
    {
      printf("Temporary noise file could not be opened\n");
      return 1;
    }


  std::ostringstream buffer;
  buffer << runNum;
  string filenamerun;
  filenamerun = directory + "DchNoise_" + buffer.str() + ".txt";
  FILE *outfile2 = fopen(filenamerun.c_str(), "w");

  if (!outfile2)
    {
      printf("Permanent noise file could not be opened\n");
      fclose(outfile);
      return 1;
    }


  for (int i = 0; i < NUMARM; i++)
    {
      for (int j = 0; j < NUMSIDE; j++)
        {
          for (int k = 0; k < NUMPLANE; k++)
            {
              for (int l = 0; l < NUMBOARD; l++)
                {
                  float dens = (float)hits_array[i][j][k][l] / (float)NCOUNTS; //nevt;
                  if (dens > noise_threshold)
                    {
                      int index = l + NUMBOARD * k + NUMBOARD * NUMPLANE * j + NUMBOARD * NUMPLANE * 2 * i;
                      fprintf(outfile, "%d %d %d %d %d %f\n", index, i, j, k, l, dens);
                      fprintf(outfile2, "%d %d %d %d %d %f\n", index, i, j, k, l, dens);
                    }
                }
            }
        }
    }


  fclose(outfile);
  fclose(outfile2);

  printf("Wrote Noise file %s", filename.c_str()) ;
  return 0;
}



///////////////////////////////////////////
//  read and write dead
///////////////////////////////////////////

int DchMon::read_dead()
{
  int index, arm, side, plane, board;
  float signal;
  int i = 0;

  FILE *infile = 0;


  string filename;
  filename = directory + "DchDead.txt";
  infile = fopen(filename.c_str(), "r");

  if (!infile)
    {
      ostringstream msg;
      msg << "\nDead channel file could not be opened " ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_INFORMATIONAL, msg.str(), 1);
      return 1;
    }

  while (!feof(infile))
    {

      fscanf(infile, "%d %d %d %d %d %f", &index, &arm, &side, &plane, &board, &signal);
      if (!feof(infile))
        {
          if (i < DEADARRAYSIZE)
            {
              old_dead_array[i] = index;
              i++;  // dont increment i if we overshoot, who knows what these guys do with it later on
            }
          else
            {
              ostringstream msg;
              msg << "DCH: Dead Array size too small, current need more than: " << i ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_ERROR, msg.str(), 1);
              break;
            }
        }
    }

  fclose(infile);
  old_dead_n = i;

  printf("Starting monitoring with %d dead channels in both chambers as read from file.\n", old_dead_n) ;
  return 0;
}







int
DchMon::write_dead()
{
  if (nevt < 2*stat_threshold + 14)
    {
      printf("Not enough statistics gathered. I refuse to write.... Run a few more events, at least %d\n", 2*stat_threshold + 14);
      return 1;
    }


  string filename;
  filename = directory + "DchDead.txt";
  FILE *outfile = fopen(filename.c_str(), "w");

  if (!outfile)
    {
      printf("Temporary dead file could not be opened\n") ;
      return 1;
    }



  std::ostringstream buffer;
  buffer << runNum;
  string filenamerun;
  filenamerun = directory + "DchDead_" + buffer.str() + ".txt";
  FILE *outfile2 = fopen(filenamerun.c_str(), "w");

  if (!outfile2)
    {
      printf("Permanent dead file could not be opened ") ;
      fclose(outfile);
      return 1;
    }

  for (int i = 0; i < NUMARM; i++)
    {
      for (int j = 0; j < NUMSIDE; j++)
        {
          for (int k = 0; k < NUMPLANE; k++)
            {
              for (int l = 0; l < NUMBOARD; l++)
                {
                  float dens = (float)hits_array[i][j][k][l] / (float)NCOUNTS; //nevt;
                  if (dens < dead_threshold)
                    {
                      int index = l + NUMBOARD * k + NUMBOARD * NUMPLANE * j + NUMBOARD * NUMPLANE * 2 * i;
                      fprintf(outfile, "%d %d %d %d %d %f\n", index, i, j, k, l, dens);
                      fprintf(outfile2, "%d %d %d %d %d %f\n", index, i, j, k, l, dens);
                    }
                }
            }
        }
    }

  fclose(outfile);
  fclose(outfile2);

  printf("Wrote dead channel file %s\n", filename.c_str()) ;
  return 0;
}




int
DchMon::write_templates()
{
  if (nevt < 2*stat_threshold + 14)
    {
      printf("Not enough statistics gathered. I refuse to write.... Run a few more events, at least %d\n", 2*stat_threshold + 14);
      return 1;
    }


  calculate_densities();

  calculate_density_averages();

  for (int irep = 0; irep < 2; irep++)
    {

      string filename;

      if (irep == 0)
        {
          filename = directory + "DchTemplates.txt";
        }
      else
        {
          std::ostringstream buffer;
          buffer << runNum;
          filename = directory + "DchTemplates" + "_" + buffer.str() + ".txt";
        }

      ofstream out;
      out.open(filename.c_str());
      if (! out.good() )
        {
          printf("DchMon:write_templates(): could not write file. Call an EXPERT!\n") ;
          return 1;
        }
      else
        {
          out << running_average << endl; //DLAN
          out << update_rate << endl;     //  Add some thresholds, etc. to template
          out << stat_threshold << endl;  //
          out << dead_threshold << endl;  //
          out << noise_threshold << endl; //

          out << SIGMA_HITRATE << endl;   //DLAN
          out << LOW_GOOD << endl;	  //
          out << HIGH_GOOD << endl;	  //  These are the constants used to determine if
          out << LOW_HOT << endl;	  //  it is GOOD, HOT, COLD
          out << HIGH_HOT << endl;	  //  They are species dependent, so they are put
          out << LOW_COLD << endl;	  //  in the template now that we have eliminated
          out << HIGH_COLD << endl;	  //  the DchTemplates_species.txt in favor of DchTemplates.txt

          out << LLOAD_ALERT_RED << endl; // These are the constants used to determine color warning
          out << LLOAD_ALERT_YEL << endl; // on the canvas based on LOAd
          out << HLOAD_ALERT_RED << endl; // Again, species dependent so they're in DchTemplates.txt
          out << HLOAD_ALERT_YEL << endl; //

          out << new_noisy_east_n << endl;
          out << new_noisy_west_n << endl;
          out << new_dead_east_n << endl;
          out << new_dead_west_n << endl;
          out << tzero_east << endl ;
          out << tzero_west << endl;
          out << vdrift_east << endl;
          out << vdrift_west << endl ;
          out << eff_east << endl;
          out << eff_west << endl ;
          for (int arm = 0; arm < NUMARM; arm++)
            {
              for (int side = 0; side < NUMSIDE; side++)
                {
                  out << hitrate_x1x2average[arm][side] << endl;
                  out << hitrate_uv1average[arm][side] << endl;
                  out << hitrate_u2average[arm][side] << endl;
                  out << hitrate_v2average[arm][side] << endl;
                }
            }

          out.close();
        }

      printf("Wrote Templates file %s\n", filename.c_str()) ;
    }

  return 0;
}



int
DchMon::read_templates()
{
  string filename;

  filename = directory + "DchTemplates.txt";

  ifstream in;
  printf(" will read : %s\n", filename.c_str()) ;
  in.open(filename.c_str());
  if (! in.good() )
    {
      ostringstream msg;
      msg << "DchMon:read_templates(): could not open file for output. Call an EXPERT!" ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_FATAL, msg.str(), 1);
      exit(1);
    }
  else
    {
      in >> template_running_average;  //DLAN
      in >> template_update_rate;      //
      in >> template_stat_threshold;   // Read all of these from DchTemplates.txt
      in >> template_dead_threshold;   //
      in >> template_noise_threshold;  //

      in >> template_SIGMA_HITRATE;	//DLAN
      in >> template_LOW_GOOD;		//
      in >> template_HIGH_GOOD;		//Must be read now; no species templates
      in >> template_LOW_HOT;		//
      in >> template_HIGH_HOT;		//
      in >> template_LOW_COLD;		//
      in >> template_HIGH_COLD;		//

      in >> template_LLOAD_ALERT_RED;	//DLAN
      in >> template_LLOAD_ALERT_YEL;	//
      in >> template_HLOAD_ALERT_RED;	//Must be read now; no species templates
      in >> template_HLOAD_ALERT_YEL;	//

      in >> template_noisy_east_n;
      in >> template_noisy_west_n;
      in >> template_dead_east_n;
      in >> template_dead_west_n;
      in >> template_tzero_east;
      in >> template_tzero_west;
      in >> template_vdrift_east;
      in >> template_vdrift_west;
      in >> template_eff_east;
      in >> template_eff_west;


      running_average = template_running_average;
      update_rate = template_update_rate;
      stat_threshold = template_stat_threshold;
      dead_threshold = template_dead_threshold;
      noise_threshold = template_noise_threshold;

      for (int arm = 0; arm < NUMARM; arm++)
        {
          for (int side = 0; side < NUMSIDE; side++)
            {
              in >> hitrate_x1x2average[arm][side];
              in >> hitrate_uv1average[arm][side];
              in >> hitrate_u2average[arm][side];
              in >> hitrate_v2average[arm][side];

              if ( hitrate_x1x2average[arm][side] < 0. || hitrate_uv1average[arm][side] < 0.
                   || hitrate_u2average[arm][side] < 0. || hitrate_v2average[arm][side] < 0.)
                {
                  ostringstream msg;
                  msg << "INVALID AVERAGE READ FROM TEMPLATES FILE!!! Aborting. Call an expert!" ;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this, MSG_SOURCE_DC, MSG_SEV_FATAL, msg.str(), 1);
                  in.close();
                  //exit(1);
                }
            }
        }

      in.close();
    }

  printf("Read templates from file: %s\n", filename.c_str()) ;

  print_templates();

  //////////////////////////////////////////////////////////////////////////////////////////
  //Fill the shuffle boards (histos with averages that will divide the densisty histograms)
  //////////////////////////////////////////////////////////////////////////////////////////
  s00_average->Reset();
  s01_average->Reset();
  s10_average->Reset();
  s11_average->Reset();
  TH1 *s = NULL;



  int c = 0; //sanity counter

  for (int arm = 0; arm < NUMARM; arm++)
    {
      for (int side = 0; side < NUMSIDE; side++)
        {
          for (int p = 0; p < 40; p++)

            {
              for (int b = 0; b < 80; b++)
                {


                  if (arm == 0)
                    {
                      if (side == 0)
                        {
                          s = s00_average;
                        }
                      else if (side == 1)
                        {
                          s = s01_average;
                        }
                    }
                  else if (arm == 1)
                    {
                      if (side == 0)
                        {
                          s = s10_average;
                        }
                      else if (side == 1)
                        {
                          s = s11_average;
                        }
                    }

                  if (s == NULL)
                    {
                      exit(1);
                    }

                  if ( p < 12 || (p > 19 && p < 32) )  //X1X2
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_x1x2average[arm][side]);
                      c++;
                    }
                  else if ( p > 11 && p < 20 )    //UV1
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_uv1average[arm][side]);
                      c++;
                    }
                  else if ( p > 31 && p < 36 )  //U2
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_u2average[arm][side]);
                      c++;
                    }
                  else if ( p > 35 )  //V2
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_v2average[arm][side]);
                      c++;
                    }
                  else
                    {
                      //  exit(1);
                    }
                }
            }
        }
    }

  printf("done reading templates\n") ;

#ifdef DEBUG

  printf("sanity counter %d\n", c);
#endif //DEBUG

  return 0;

}

void DchMon::write_all_files()
{
  directory = "./";
  write_dead();
  write_noise();
  write_templates();
  exit(1);
}

void DchMon::print_templates()
{
  printf("Template parameters for Drift Chamber Online Monitoring:\n") ;
  printf("--------------------------------------------------------\n") ;
  printf("template_running_average %d\n", template_running_average);
  printf("template_update_rate %d\n", template_update_rate);
  printf("template_stat_threshold %u\n", template_stat_threshold) ;    //Print out the new thresholds that
  printf("template_dead_threshold %f\n", template_dead_threshold) ;    //now are in the DchTemplate.txt file
  printf("template_noise_threshold %f\n", template_noise_threshold) ;  //
  printf("template_SIGMA_HITRATE %f\n", template_SIGMA_HITRATE) ;	    //
  printf("template_LOW_GOOD %f\n", template_LOW_GOOD) ;	 	    //
  printf("template_HIGH_GOOD %f\n", template_HIGH_GOOD) ;		    //
  printf("template_LOW_HOT %f\n", template_LOW_HOT) ; 		    //
  printf("template_HIGH_HOT %f\n", template_HIGH_HOT) ;		    //
  printf("template_LOW_COLD %f\n", template_LOW_COLD) ;		    //
  printf("template_HIGH_COLD %f\n", template_HIGH_COLD) ;		    //
  printf("template_noisy_east_n %d\n", template_noisy_east_n) ;
  printf("template_noisy_west_n %d\n", template_noisy_west_n) ;
  printf("template_dead_east_n %d\n", template_dead_east_n) ;
  printf("template_dead_west_n %d\n", template_dead_west_n) ;
  printf("template_tzero_east %d\n", template_tzero_east) ;
  printf("template_tzero_west %d\n", template_tzero_west) ;
  printf("template_vdrift_east %f\n", template_vdrift_east) ;
  printf("template_vdrift_west %f\n", template_vdrift_west) ;
  printf("template_eff_east %f\n", template_eff_east) ;
  printf("template_eff_west %f\n", template_eff_west) ;
  for (int arm = 0; arm < NUMARM; arm++)
    {
      for (int side = 0; side < NUMSIDE; side++)
        {
          printf("hitrate_x1x2average[%d][%d] %f\n", arm, side, hitrate_x1x2average[arm][side]) ;
          printf("hitrate_uv1average [%d][%d] %f\n", arm, side, hitrate_uv1average[arm][side]) ;
          printf("hitrate_u2average  [%d][%d] %f\n", arm, side, hitrate_u2average[arm][side]) ;
          printf("hitrate_v2average  [%d][%d] %f\n", arm, side, hitrate_v2average[arm][side]) ;
        }
    }


}

/////////////////////////////////
//count noise
//a typical call to the function : count_noise(0 ,0 ,old_noise_num_00,hit_num_00,new_noise_num_00,missing_noise_num_00);
// a = arm, s = side
//
//
// uses the old_noise/dead_array that was read from file and compares them with the new_noise/dead_arrays
//
// This should be done periodically, every update_rate #of events
//
// It should be called after the update of new_noise/dead_arrays has been done
//
////////////////////////////////
void DchMon::count_noise(int a, int s, int &num1, int &num2, int &num3, int &num4)
{
  bool noisy = 0;
  bool new_noisy = 0;
  int n, k, l, index;
  num1 = num2 = num3 = num4 = 0;


  for (k = 0; k < NUMPLANE; k++)
    {
      for (l = 0; l < NUMBOARD; l++)
        {
          index = l + NUMBOARD * k + NUMBOARD * NUMPLANE * s + NUMBOARD * NUMPLANE * 2 * a;
          noisy = 0;
          new_noisy = 0;
          for (n = 0; n < old_noisy_n; n++)
            {
              if (old_noise_array[n] == index)
                {
                  num1++;
                  noisy = 1;
                }
            }
          if (!noisy)
            {
              num2 += hits_array[a][s][k][l];
            }
          if (nevt > stat_threshold)
            {
              for (n = 0; n < new_noisy_n; n++)
                {
                  if (new_noise_array[n] == index)
                    {
                      new_noisy = 1;
                      if (!noisy)
                        {
                          num3++;
                        }
                    }
                }
              if (noisy)
                {                //if old noisy AND NOT new noisy then recovered!
                  if (!new_noisy)
                    {
                      num4++;
                    }
                }
            }
        }
    }
}

int 
DchMon::process_event(Event *evt)
{
  OnlMonServer *se = OnlMonServer::instance();
  int n_words;
  int dlength, DCMword, dummy, value;
  int TimeSlice, Nibble;
  short arm, pair, side, keystone, channel, ch_old, plane, board, edge, mask;
  int width, time, ltime, x;
  int hits_n;
  static int time2[2][2][80][40][5];
  static int rawdat[MAXPACKETLENGTH];
  hits_n = 0;


  //  read_dead();
  //  read_templates();

  if (evt->getEvtType() != DATAEVENT)
    {
      return 0;
    }
  if ((nevt % 1000) == 0)
    {
      if (verbosity > 0)
        {
          printf("DchMonitor::process_event(%u)\n", nevt) ;
        }
    }

  //addition by Richard Petti
  //this is taken from DchCalibrate.C and was developed by torsten dahms and implemented by Megan Conners.
  //This removes noisy events from the online monitoring caused by the PC on certain clock ticks
  
  // int clockticks[3]= {0,0,0};
//   int multieventbuffered = 0;
  
//   Packet *pkt14009 = evt->getPacket(14009);
//   if (pkt14009)
//     {
//       int granaccpvec = pkt14009->iValue(0, "PARVECT");
//       int n = 0;
//       unsigned int clockcounter = static_cast <unsigned int> (pkt14009->iValue(0, "EVCLOCK")); // ivalue returns int
      
//       for (int i = 1; i < 4; i++) // there are 4 events kept including the current one
// 	{
// 	  if (pkt14009->iValue(i, "PARVECT") == granaccpvec)
// 	    {
// 	      unsigned int thisclock = static_cast <unsigned int>
// 		(pkt14009->iValue(i, "EVCLOCK"));
// 	      if (thisclock < clockcounter)
// 		{
// 		  clockticks[n] = clockcounter - thisclock;
// 		  // prevevt->set_clockticks(clockcounter - thisclock, n);
// 		}
// 	      else
// 		{
// 		  cout << "rollover: clockcounter: " << clockcounter
// 		       << ", thisclock: " << thisclock
// 		       << ",  0xFFFFFFFF - thisclock: " << 0xFFFFFFFF - thisclock
// 		       << ", result: " << clockcounter + (0xFFFFFFFF - thisclock) + 1
// 		       << endl;
// 		  cout << hex << "rollover: clockcounter: " <<   clockcounter
// 		       << ", thisclock: " << thisclock
//                            << ",  0xFFFFFFFF - thisclock: " <<
// 		    0xFFFFFFFF - thisclock
// 		       << ", result: " << clockcounter + (0xFFFFFFFF -
// 							  thisclock) + 1
// 		       << dec << endl;
// 		  clockticks[n] = clockcounter + (0xFFFFFFFF - thisclock) + 1; // +1 to include the 0x0 clock counter
// 		  // prevevt->set_clockticks(clockcounter +(0xFFFFFFFF - thisclock) + 1, n); // +1 to include the 0x0 clock counter
// 		}
// 	      n++;
// 	    }
// 	}

//       delete pkt14009;

//       if (clockticks[0] <= 1824 && clockticks[0] > 0)
// 	{
// 	  multieventbuffered = 1;
// 	  if (clockticks[0] < 16)
// 	    {
// 	      cout << "Clock Tick Diff between 2 events too small: " <<	clockticks[0] << endl;
// 	    }

// 	  if (clockticks[0]>112 && clockticks[0]<124)
// 	    {
// 	      return 1;
// 	    }
// 	  if ((clockticks[1]>112 && clockticks[1]<124) ||
// 	      (clockticks[1]>269 && clockticks[1]<280))
// 	    {
// 	      return 1;
// 	    }
// 	  if ((clockticks[2]>112 && clockticks[2]<124) ||
// 	      (clockticks[2]>269 && clockticks[2]<280) ||
// 	      (clockticks[2]>427 && clockticks[2]<437))
// 	    {
// 	      return 1;
// 	    }
// 	 //  if(clockticks[0]<500 || clockticks[1]<500 || clockticks[2]<500)   //exculdes info from first bunch of clockticks...resulting in a lower hit rate
// // 	  {
// // 	   return 1;
// // 	  }
	 
// 	}
//     }
  /////end addition
  nevt++;
  NCOUNTS++;


  runNum = se->RunNumber();
  edge = -1;
  ltime = -10;

  //At the beggining of each event we reset the firing and leading time arrays
  memset(firing, 0, sizeof(firing));
  memset(time1, 0, sizeof(time1));
  //notice that the hits array is not being reset at each event but is a cumulative array


  //The Legendary U_N_P_A_C_K_E_R WOW!!!!!!!!
  for (short n_p = 0 ; n_p < NDCHP ; n_p++ )
    {
      Packet *p = evt->getPacket(3001 + n_p);
      if (p)
        {
          dlength = p->getDataLength();
          if (dlength > MAXPACKETLENGTH)
            {
              ostringstream msg;
              msg << "Dch packet length is too high:" << dlength
		  << ", packet = " << 3001 + n_p
		  << ", event  = " << evt->getEvtSequence();
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_ERROR, msg.str(), 2);
              break;
            }
          else
            {
              p->fillIntArray(rawdat, dlength, &n_words, "DATA");
              arm = rawdat[0] & 0x1;
              dummy = rawdat[2] & 0x000FFFFF;

              pair = dummy & 0x0F;                     // Decode pair
              dummy >>= 4;
              keystone = dummy & 0xFF;                 // Decode keystone
              dummy >>= 8;
              side = dummy & 0xF;                      // Decode side
              ch_old = -1;

              for (x = 5;x < dlength - 6;x++)
                {
                  DCMword = rawdat[x];
                  if (DCMword&0x000FFFFF)
                    {
                      channel = (DCMword >> 24) & 0x7f;      // Decode  FEM channel

                      if (ch_old != channel)             // If we are in  new channel reset edge counters
                        {
                          time = -1;
                          ltime = -1;
                          edge = -1;
                        }

                      ch_old = channel;
                      plane = 39 - channel / 2;              // Decode plane

                      if (((arm == 0) && (side == 1)) || ((arm == 1) && (side == 0)))    //  Decode board
                        board = keystone * 4 + pair * 2 + channel % 2;
                      else
                        board = keystone * 4 + (3 - pair * 2 - channel % 2);

                      TimeSlice = (DCMword >> 20) & 0xf;
                      for (mask = 0;mask < 4;mask++)
                        {
                          Nibble = TimeSlice * 4 + mask;
                          value = (DCMword >> (mask * 5)) & 0x1f;                  //  Unpacking time pieces

                          if (value&0x10)                                  //  Leading edge found
                            {
                              edge = 0;
                              ltime = (Nibble * 16) + (value & 0xf);
                            }

                          if ((value&0x18) == 0x08)                           //  Trailing edge found
                            {
                              if (edge == 0)                                 //  If previouse edge was leading - do the magic
                                {
                                  time = (Nibble * 16) + (value & 0x7) * 2;
                                  width = time - ltime;

                                  if (width > WidthCut)
                                    {
                                      if (arm > NUMARM || side > NUMSIDE || plane > NUMPLANE || board > NUMBOARD)
                                        {
                                          ostringstream msg;
                                          msg << "Dch index out of range: arm = " << arm
					      << ", side = " << side
					      << ", plane = " << plane
					      << ", board = " << board
					      << ", pair = " << pair
					      << ", keystone = " << keystone;
                                          OnlMonServer *se = OnlMonServer::instance();
                                          se->send_message(this, MSG_SOURCE_DC, MSG_SEV_INFORMATIONAL, msg.str(), 3);
                                        }
                                      else
                                        {
                                          //hits_array[arm][side][plane][board]++;
                                          if (firing[arm][side][board][plane] < 5)
                                            {
                                              time2[arm][side][board][plane][firing[arm][side][board][plane]] = ltime;
                                            }
                                          firing[arm][side][board][plane]++;
                                          time1[arm][side][board][plane] = ltime;
                                          hits_n++;
                                        }
                                    }
                                }
                              edge = 1;
                            } //trailing edge found
                        } //trailing edge
                    } //DCMword&000FFFFF
                } //for(package length) loop
            } //dlength > MAXPACKETLENGTH
          delete p;
        } //if(p)
    } //number of packets


  //addition by Richard Petti
  //this is taken from DchCalibrate.C and was developed by torsten dahms and implemented by Megan Conners.
  //This removes noisy events from the online monitoring caused by the PC on certain clock ticks
  
  int clockticks2[3]= {0,0,0};
  int multieventbuffered2 = 0;
  
  Packet *pkt14009a = evt->getPacket(14009);
  if (pkt14009a)
    {
      int granaccpvec2 = pkt14009a->iValue(0, "PARVECT");
      int n = 0;
      unsigned int clockcounter2 = static_cast <unsigned int> (pkt14009a->iValue(0, "EVCLOCK")); // ivalue returns int
      
      for (int i = 1; i < 4; i++) // there are 4 events kept including the current one
	{
	  if (pkt14009a->iValue(i, "PARVECT") == granaccpvec2)
	    {
	      unsigned int thisclock2 = static_cast <unsigned int>
		(pkt14009a->iValue(i, "EVCLOCK"));
	      if (thisclock2 < clockcounter2)
		{
		  clockticks2[n] = clockcounter2 - thisclock2;
		}
	      else // handle rollover 
		{
		  clockticks2[n] = clockcounter2 + (0xFFFFFFFF - thisclock2) + 1; // +1 to include the 0x0 clock counter
		}
	      n++;
	    }
	}

      delete pkt14009a;

      if (clockticks2[0] <= 1824 && clockticks2[0] > 0)
	{
	  multieventbuffered2 = 1;
	  if (clockticks2[0] < 16)
	    {
	      cout << "Clock Tick Diff between 2 events too small: " <<	clockticks2[0] << endl;
	    }

	  if (clockticks2[0]>112 && clockticks2[0]<124)
	    {
	      return 1;
	    }
	  if ((clockticks2[1]>112 && clockticks2[1]<124) ||
	      (clockticks2[1]>269 && clockticks2[1]<280))
	    {
	      return 1;
	    }
	  if ((clockticks2[2]>112 && clockticks2[2]<124) ||
	      (clockticks2[2]>269 && clockticks2[2]<280) ||
	      (clockticks2[2]>427 && clockticks2[2]<437))
	    {
	      return 1;
	    }
// 	  if(clockticks2[0]<500 || clockticks2[1]<500 || clockticks2[2]<500)   //exculdes info from first bunch of clockticks...resulting in a lower hit rate
// 	  {
// 	   return 1;
// 	  }
	 
	}
    }
  /////end addition



  // Filling histograms
  if (hits_n < 10000) // hardcoded number!!! change at next occasion
    {
      for (arm = 0; arm < 2; arm++)
        {
          for (side = 0; side < 2; side++)
            {
              for (board = 0; board < 80; board++)
                {
                  for (plane = 0; plane < 40; plane++)
                    {
                      // Efficiency calculation (8 wire from X1 + 8 wire from X2)
                      bool noisy = 0;
                      int index = board + NUMBOARD * plane + NUMBOARD * NUMPLANE * side + NUMBOARD * NUMPLANE * 2 * arm;
                      for (int i = 0; i < old_noisy_n; i++)
                        {
                          if (old_noise_array[i] == index)
                            {
                              noisy = 1;
                            }
                        }
                      if (!noisy)
                        {
                          if ((plane > 1 && plane < 10) || (plane > 21 && plane < 30))
                            {
                              if (firing[arm][side][board][plane - 2] == 1 &&
				  firing[arm][side][board][plane + 2] == 1 &&
				  firing[arm][side][board][plane] < 2 &&
				  firing[arm][side][board][plane - 1] == 0 &&
				  firing[arm][side][board][plane + 1] == 0)
                                {
                                  if (abs(time1[arm][side][board][plane - 2] - 300) < 100 &&
				      abs(time1[arm][side][board][plane + 2] - 300) < 100)
                                    {
                                      float t_resid = (time1[arm][side][board][plane - 2] + time1[arm][side][board][plane + 2]) * 0.5 - time1[arm][side][board][plane];
                                      if (plane < 10) // X1
                                        {
                                          if (arm == 0)
                                            {
                                              dchek1->Fill((80*(plane - 2) + board + 640*side )); //east X1
                                              if (abs((int) t_resid) < 50)
                                                {
                                                  tmp_ek1_1->Fill((80*(plane - 2) + board + 640*side));
                                                }
                                            }
                                          else
                                            {
                                              dchek3->Fill((80*(plane - 2) + board + 640*side )); //West X1
                                              if (abs((int) t_resid) < 50)
                                                {
                                                  tmp_ek3_3->Fill(80*(plane - 2) + board + 640*side);
                                                }
                                            }
                                        }
                                      else // X2
                                        {
                                          if (arm == 0)
                                            {
                                              dchek2->Fill((80*(plane - 22) + board + 640*side)); //east X2
                                              if (abs((int) t_resid) < 50)
                                                {
                                                  tmp_ek2_2->Fill((80*(plane - 22) + board + 640*side));
                                                }
                                            }
                                          else
                                            {
                                              dchek4->Fill((80*(plane - 22) + board + 640*side)); //West X2
                                              if (abs((int) t_resid) < 50)
                                                {
                                                  tmp_ek4_4->Fill((80*(plane - 22) + board + 640*side));
                                                }
                                            }
                                        } // X1 or X2
                                    }
                                }
                            }
                          for (int fir = 0; fir < firing[arm][side][board][plane]; fir++)
                            {
                              if (firing[arm][side][board][plane] > 4)
                                {
                                  continue;
                                }
                              if (arm == 0)
                                {
                                  hits_array[arm][side][plane][board]++;

				  // inserted to cut out the spiked channels 03/26/09
				  if( (board != 26) && (plane > 30 || plane < 28) && (side != 0))
				    dc_h_dc1->Fill(time2[arm][side][board][plane][fir]);
				  
                                  if ( ((plane < 12) || (plane < 32 && plane > 19)) && (tzero_east > 0))
                                    {
				      driftvelocity_east->Fill((time2[arm][side][board][plane][fir] - tzero_east) / WireWidth[plane]);
                                      //dc_h_dc1->Fill(time2[arm][side][board][plane][fir]);
                                    }
                                } // East
                              else
                                {
                                  hits_array[arm][side][plane][board]++;
                                  dc_h_dc2->Fill(time2[arm][side][board][plane][fir]);
                                  if (((plane < 12) || ((plane < 32) && plane > 19)) && (tzero_west > 0))
                                    {
                                      driftvelocity_west->Fill((time2[arm][side][board][plane][fir] - tzero_west) / WireWidth[plane]);
                                      //dc_h_dc2->Fill(time2[arm][side][board][plane][fir]);
                                    }
                                } //West
                            } // Noise
                        } //firing
                    } //plane
                } //board
            } //side
        } //arm
    } //hits number check
  else
    {
      bad_events++;
    }
  
  
  // Some comments by felix matathias (EFFICIENCY README from Felix)
  // assume k=2, then if firing is:            1    0   <2    0    1                       (remember the odd-even anode drifting)
  //                                       ----x----x----x----x----x----x----x----x-- ....
  //                                           0    1    2    3    4    5    6    7   ....   plane number
  //
  // which basically means that the adjacent+1 wires fired (modulo the adjacent anodes with different drifting direction)
  //
  // then take the average leading time of these adjacent wires: (timel(plane_0)+timel(plane_4))*0.5
  // and subtract the leading time of the wire under examination: - timel(plane_2)
  // call this residual time and check if the residual time was within +-50nsec, which means if the wire under examination
  // fired in synchronicity with the 2 adjacent wires that also fired.
  // IF IT DID THEN eff=1 !!!!! dah!!!

  if ( (ncalls++ > update_rate) && nevt > 2*stat_threshold )
    {
      ncalls = 0;
      CALCULATRIX();

#ifdef DEBUG

      time_t currentTick = evt->getTime();
      printf("UPDATE THRESHOLD REACHED: Event number: %u HUMANOID time: %s", nevt, ctime(&currentTick)) ;
      printf("new_dead_east_n %d\n", new_dead_east_n) ;
      printf("new_dead_west_n %d\n", new_dead_west_n) ;
      printf("new_noisy_east_n %d\n", new_noisy_east_n) ;
      printf("new_noisy_west_n %d\n", new_noisy_west_n) ;
      printf("vdrift_east      %f\n", vdrift_east) ;
      printf("vdrift_west      %f\n", vdrift_west) ;
      printf("tzero_east       %d\n", tzero_east) ;
      printf("tzero_west       %d\n", tzero_west) ;
      printf("eff_east         %f\n", eff_east) ;
      printf("eff_west         %f\n", eff_west) ;
      printf("bad_events       %d\n", bad_events);
      printf("nevents          %u\n", nevt) ;
#endif

      infogroup++;

      if (!running_average)
        {
          reset_arrays_histograms();
        }
    }

  if (nevt == stat_threshold)
    {
      update_t0(); //needed by drift_velocity algorithm
    }

  dc_info->SetBinContent( NEVT_BIN , nevt );
  dc_info->SetBinContent( BADEV_BIN , bad_events );

  return 0;
}

int DchMon::CALCULATRIX()
{
  update_new_noisy_dead();
  update_t0();
  update_drift_velocities();
  calculate_efficiencies();
  calculate_densities();
  if (calculateAveragesOnTheFly)
    {
      calculate_density_averages();
    }
  normalize_densities();

  ////////////////////////////////////////////////////////////////////////////////
  //Compute  new noise, old noise, recovered noise, number of hits *PER QUADRANT*
  ///////////////////////////////////////////////////////////////////////////////
  count_noise(0, 0, old_noise_num_00, hit_num_00, new_noise_num_00, missing_noise_num_00);
  count_noise(0, 1, old_noise_num_01, hit_num_01, new_noise_num_01, missing_noise_num_01);
  count_noise(1, 0, old_noise_num_10, hit_num_10, new_noise_num_10, missing_noise_num_10);
  count_noise(1, 1, old_noise_num_11, hit_num_11, new_noise_num_11, missing_noise_num_11);

  //////////////////////////////////////////////////////////////////
  //Compute Average Load in percent -- 15Dec2009 benjil changed to absolute 
  //3200 is the number of wires per quadrant = 80boards x 40planes
  /////////////////////////////////////////////////////////////////
  avg_load_00 = ((float)(hit_num_00) / (float)(NCOUNTS * 3200)); //was nevt
  avg_load_01 = ((float)(hit_num_01) / (float)(NCOUNTS * 3200));
  avg_load_10 = ((float)(hit_num_10) / (float)(NCOUNTS * 3200));
  avg_load_11 = ((float)(hit_num_11) / (float)(NCOUNTS * 3200));

  ///////////////////////////////////////////////
  //Send all info calculated to the dc_info histogram
  //////////////////////////////////////////////
  encodeInfoToHistogram();
  return 0;
}

int DchMon::calculate_efficiencies()
{
  dch_ek1_1->Reset("ICE");
  dch_ek2_2->Reset("ICE");
  dch_ek3_3->Reset("ICE");
  dch_ek4_4->Reset("ICE");
  dch_ek1_1->Add(tmp_ek1_1);
  dch_ek2_2->Add(tmp_ek2_2);
  dch_ek3_3->Add(tmp_ek3_3);
  dch_ek4_4->Add(tmp_ek4_4);

  //cout<<"**********************************************"<<endl;
  //for (int i=0; i<25; i++) {cout<<setw(5)<<dch_ek1_1->GetBinContent(i + 1);}
  //cout<<endl;
  //for (int i=0; i<25; i++) {cout<<setw(5)<<dchek1->GetBinContent(i + 1);}
  //cout<<endl;
  //cout<<"**********************************************"<<endl;

  dch_ek1_1->Divide(dchek1);
  dch_ek2_2->Divide(dchek2);
  dch_ek3_3->Divide(dchek3);
  dch_ek4_4->Divide(dchek4);
  //   dch_ek1_1->SetMaximum(1.0);
  //   dch_ek1_1->SetMinimum(0.3);
  //   dch_ek2_2->SetMaximum(1.0);
  //   dch_ek2_2->SetMinimum(0.3);
  //   dch_ek3_3->SetMaximum(1.0);
  //   dch_ek3_3->SetMinimum(0.3);
  //   dch_ek4_4->SetMaximum(1.0);
  //   dch_ek4_4->SetMinimum(0.3);

  eff_east = 0;
  eff_west = 0;

  if (nevt < stat_threshold)
    {
      return 0;
    }

  //calculate average efficiencies

  double eff_east_sum = 0;
  double eff_west_sum = 0;
  double eff_east_counts = 0;
  double eff_west_counts = 0;
  double thr[4] = {0, 0, 0, 0};


  for (int i = 0; i < 1280 ; i++)
    {
      thr[0] = thr[0] + dchek1->GetBinContent(i + 1);
      thr[1] = thr[1] + dchek2->GetBinContent(i + 1);
      thr[2] = thr[2] + dchek3->GetBinContent(i + 1);
      thr[3] = thr[3] + dchek4->GetBinContent(i + 1);
    }


  for (int i = 0; i < 1280 ; i++)
    {
      if (dch_ek1_1->GetBinContent(i + 1) > 0.6  && dchek1->GetBinContent(i + 1) > thr[0] / 1280. / 0.8)
        {
          eff_east_counts++;
          eff_east_sum += dch_ek1_1->GetBinContent(i + 1);
        }
      if (dch_ek2_2->GetBinContent(i + 1) > 0.6  && dchek2->GetBinContent(i + 1) > thr[1] / 1280. / 0.8)
        {
          eff_east_counts++;
          eff_east_sum += dch_ek2_2->GetBinContent(i + 1);
        }
      if (dch_ek3_3->GetBinContent(i + 1) > 0.6  && dchek3->GetBinContent(i + 1) > thr[2] / 1280. / 0.8)
        {
          eff_west_counts++;
          eff_west_sum += dch_ek3_3->GetBinContent(i + 1);
        }
      if (dch_ek4_4->GetBinContent(i + 1) > 0.6  && dchek4->GetBinContent(i + 1) > thr[3] / 1280. / 0.8)
        {
          eff_west_counts++;
          eff_west_sum += dch_ek4_4->GetBinContent(i + 1);
        }
    }

  //Put these into Dc_info histo as extra bin, so can put in status bar of Hit Rate Canvas later
  if (eff_east_counts)
    eff_east = eff_east_sum / eff_east_counts ;

  if (eff_west_counts)
    eff_west = eff_west_sum / eff_west_counts ;
  return 0;
}



int DchMon::calculate_densities()
{
  //Reset the Quadrant densities histograms
  dc_dens_00->Reset();
  dc_dens_01->Reset();
  dc_dens_10->Reset();
  dc_dens_11->Reset();
  h_dc5->Reset();
  dc_hot_distro->Reset();
  dc_cold_distro->Reset();
  dc_good_distro->Reset();
  dc_dead_distro->Reset();
  dc_noisy_distro->Reset();

  for (int i = 0; i < NUMARM; i++)
    {
      for (int j = 0; j < NUMSIDE; j++)
        {
          for (int k = 0; k < NUMPLANE; k++)
            {
              for (int l = 0; l < NUMBOARD; l++)
                {
                  float dens = (float)hits_array[i][j][k][l] / (float)NCOUNTS; //nevt;
                  //if( i==1 && j==1 && l==) DONT FILL CRAZY BOARD
                  h_dc5->Fill(dens);
                  if ( i == 0 && j == 0 )
                    dc_dens_00->SetBinContent(l + 1, k + 1, dens);
                  if ( i == 0 && j == 1 )
                    dc_dens_01->SetBinContent(l + 1, k + 1, dens);
                  if ( i == 1 && j == 0 )
                    dc_dens_10->SetBinContent(l + 1, k + 1, dens);
                  if ( i == 1 && j == 1 )
                    dc_dens_11->SetBinContent(l + 1, k + 1, dens);

                }
            }
        }
    }

  return 0;
}



int DchMon::normalize_densities()
{
  //Divide the hit rates by the corresponding averages
  dc_dens_00->Divide(s00_average);
  dc_dens_01->Divide(s01_average);
  dc_dens_10->Divide(s10_average);
  dc_dens_11->Divide(s11_average);

  int prev_dead = 0; // 1 if listed dead in the templates (DchDead.txt)
  int ipre_dead = 0;
  int dead = 0;
  int noisy = 0;
  int good = 0;
  int hot = 0;
  int cold = 0;

  float content = 0;

  TH1 *s = NULL;

  float SIGMA_HITRATE = 0;
  float LOW_GOOD, HIGH_GOOD;
  float LOW_HOT, HIGH_HOT;
  float LOW_COLD, HIGH_COLD;

  SIGMA_HITRATE = template_SIGMA_HITRATE;
  LOW_GOOD = template_LOW_GOOD;
  HIGH_GOOD = template_HIGH_GOOD;
  LOW_HOT = template_LOW_HOT;
  HIGH_HOT = template_HIGH_HOT;
  LOW_COLD = template_LOW_COLD;
  HIGH_COLD = template_HIGH_COLD;



  //Mark dead, noisy, good, mediocre channels
  for (int i = 0; i < NUMARM; i++)
    {
      for (int j = 0; j < NUMSIDE; j++)
        {
          for (int k = 0; k < NUMPLANE; k++)
            {
              for (int l = 0; l < NUMBOARD; l++)
                {

                  if (i == 0 && j == 0)
                    s = dc_dens_00;
                  if (i == 0 && j == 1)
                    s = dc_dens_01;
                  if (i == 1 && j == 0)
                    s = dc_dens_10;
                  if (i == 1 && j == 1)
                    s = dc_dens_11;

                  if (s == NULL)
                    {
                      exit(1);
                    }

		  // Search List of dead channels to determine if this ch was considered dead
		  prev_dead = 0;
		  int index = l + NUMBOARD * k + NUMBOARD * NUMPLANE * j + NUMBOARD * NUMPLANE * 2 * i;
		  ipre_dead = 0;
		  while (ipre_dead < old_dead_n && prev_dead == 0 && ipre_dead < DEADARRAYSIZE)
		    {
		      if (old_dead_array[ipre_dead] == index)
			{
			  prev_dead = 1; // this channel is in the dead list
			}
		      ipre_dead++;
		    }


                  content = s->GetBinContent(l + 1, k + 1) - 1.0;  //gaussian distro centered at zero

                  if ( (content >= LOW_GOOD*SIGMA_HITRATE) && (content <= HIGH_GOOD*SIGMA_HITRATE) )
                    {
                      good = 1;
                    }
                  else if ( (content > LOW_HOT*SIGMA_HITRATE) && (content <= HIGH_HOT*SIGMA_HITRATE) )
                    {
                      hot = 1;
                    }
                  else if ( (content >= LOW_COLD*SIGMA_HITRATE) && (content < HIGH_COLD*SIGMA_HITRATE) )
                    {
                      cold = 1;
                    }
                  else if (content > HIGH_HOT*SIGMA_HITRATE)
                    {
                      noisy = 1;
                    }
                  else if (content < LOW_COLD*SIGMA_HITRATE)
                    {
                      dead = 1;
                    }

		  // If the channel was previously dead (prev_dead=1), then the OnlMon should ignore
		  // these channels.  Mark them grey or white (???)
		  // If the channel was previosly dead but now it is alive, then the color should
		  // be shifted 1/2 unit (up???)

		  int resurrected = 0;
		  if (prev_dead == 1 && dead == 1)
		    {
		      
		    }
		  else if (prev_dead == 1 && dead == 0)
		    {
		      resurrected = 1;		      
		    }

                  if (hot)
                    {
                      s->SetBinContent(l + 1, k + 1, HOT);  //HOT 6
                      dc_hot_distro->Fill(content);
                    }
                  if (cold)
                    {
                      s->SetBinContent(l + 1, k + 1, COLD);  //COLD 4
                      dc_cold_distro->Fill(content);
                    }
                  if (dead)
                    {
		      if (prev_dead == 1)
			{
			  s->SetBinContent(l + 1, k + 1, MASK);  //MASK 17
			  dc_dead_distro->Fill(content);
			}
		      else 
			{
			  s->SetBinContent(l + 1, k + 1, DEAD);  //DEAD 1
			  dc_dead_distro->Fill(content);
			}
                    }
                  if (noisy)
                    {
                      s->SetBinContent(l + 1, k + 1, NOISY);  //NOISY 2
                      dc_noisy_distro->Fill(content);
                    }
                  if (good)
                    {
                      s->SetBinContent(l + 1, k + 1, GOOD);   //GOOD 3
                      dc_good_distro->Fill(content);
                    }

                  good = noisy = dead = hot = cold = 0;
                  content = 0;

                }
            }
        }
    }

  //force root to use the 5 color palette
  dc_dens_00->SetBinContent(1, 1, MIN_HITRATE);
  dc_dens_00->SetBinContent(1, 2, MAX_HITRATE);
  dc_dens_01->SetBinContent(1, 1, MIN_HITRATE);
  dc_dens_01->SetBinContent(1, 2, MAX_HITRATE);
  dc_dens_10->SetBinContent(1, 1, MIN_HITRATE);
  dc_dens_10->SetBinContent(1, 2, MAX_HITRATE);
  dc_dens_11->SetBinContent(1, 1, MIN_HITRATE);
  dc_dens_11->SetBinContent(1, 2, MAX_HITRATE);

  return 0;
}



int DchMon::update_new_noisy_dead()
{

  if (nevt < stat_threshold)
    return 0;

  //reset the number of new nosy and new dead
  new_noisy_n = 0;
  new_dead_n = 0;
  new_noisy_east_n = 0;
  new_noisy_west_n = 0;
  new_dead_east_n = 0;
  new_dead_west_n = 0;

  //Reset the new_noise array and new_dead_array
  for (int i = 0; i < NOISEARRAYSIZE; i++)
    {
      new_noise_array[i] = 0;
    }
  for (int i = 0;i < DEADARRAYSIZE;i++)
    {
      new_dead_array[i] = 0;
    }

  //Fill the new_noise and new_dead arrays with fresh data
  for (int i = 0; i < NUMARM; i++)
    {
      for (int j = 0; j < NUMSIDE; j++)
        {
          for (int k = 0; k < NUMPLANE; k++)
            {
              for (int l = 0; l < NUMBOARD; l++)
                {
                  float dens = (float)hits_array[i][j][k][l] / (float)NCOUNTS; //nevt;

                  if (dens > noise_threshold)
                    {
                      int index = l + NUMBOARD * k + NUMBOARD * NUMPLANE * j + NUMBOARD * NUMPLANE * 2 * i;
                      new_noise_array[new_noisy_n] = index;
                      if (new_noisy_n < (NOISEARRAYSIZE-1))
                        {
                          new_noisy_n++;
                          if (i == 0)
                            new_noisy_east_n++;
                          else
                            new_noisy_west_n++;
                        }
                    }
                  if (dens < dead_threshold)
                    {
                      int index = l + NUMBOARD * k + NUMBOARD * NUMPLANE * j + NUMBOARD * NUMPLANE * 2 * i;
                      new_dead_array[new_dead_n] = index;
                      if (new_dead_n < (DEADARRAYSIZE-1))
                        {
                          new_dead_n++;
                          if (i == 0)
                            new_dead_east_n++;
                          else
                            new_dead_west_n++;
                        }
                    }
                }
            }
        }
    }
  dead_report = new_dead_n - old_dead_n;  //Change in dead channels

  check_dead_noisy_alerts();

  return 0;
}

int DchMon::check_dead_noisy_alerts()
{
  // checks for variations in noisy and dead channels
  // It compares the pre-recorded (at the initialization) number_of_dead/noisy_channels
  // with the currently calculated ones

  int check_noisy_n = 0;
  int check_dead_n = 0;
  int alert_noise = 0;
  int alert_dead = 0;

  if (nevt < stat_threshold)
    {
      if (!report1)
        {
          printf("Not enough statistics to calculate new status yet.\n") ;
          report1 = 1;
        }
    }
  if (nevt > stat_threshold)
    {
      if (!report2)
        {
          printf("Sufficent statistics have been recorded.\n") ;
          printf("DC Monitor is active.\n") ;
          report2 = 1;
        }


      check_noisy_n = new_noisy_n;
      check_dead_n = new_dead_n;


      //Noise message ALERT
      if ((check_noisy_n != old_noisy_n) && 1==0) //disable noise message alert -benjil 23March2011
        {
          alert_noise = check_noisy_n - old_noisy_n;
          if (abs(alert_noise) >= 1)
            {
              ostringstream msg;
              msg << "Net change of " << alert_noise << " noisy channel in drift chamber." ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_WARNING, msg.str(), 4);
            }
        }

      //Dead message ALERT
      if ((check_dead_n != old_dead_n) && 1==0 ) // disable dead message alert -benjil 23March2011
        {
          alert_dead = check_dead_n - old_dead_n;

          if (alert_dead > 0)
            {

              ostringstream msg;
              msg << "Number of dead channels in Drift Chamber has increased by " << abs(alert_dead) ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_WARNING, msg.str(), 4);
              msg.str("");
            }
          else
            {
              ostringstream msg;

              msg << "Number of dead channels in Drift Chamber has decreased by " << abs(alert_dead) ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_WARNING, msg.str(), 4);
            }

        }
    }

  return 0;
}




int DchMon::update_t0()
{
  if (nevt < stat_threshold)
    {
      return 0;
    }

  //Caclulates tzero -> check_n1 and check_n2
  //alerts in case of tzero drifts

  h_dc1norm->Reset();
  h_dc2norm->Reset();

  float check_max1 = 0;
  float check_bin_content1 = 0;
  int check_n1 = 20;   //dont start from bin zero, pp data have a spike there;
  float check_max2 = 0;
  float check_bin_content2 = 0;
  int check_n2 = 20;  //dont start from bin zero, pp data have a spike there;

  //calulate tzeros
  h_dc1norm->Add(dc_h_dc1);
  h_dc1norm->Scale(1. / h_dc1norm->Integral());

  h_dc2norm->Add(dc_h_dc2);
  h_dc2norm->Scale(1. / h_dc2norm->Integral());

  //remove the spike from the pp and possibly dAu data
  //the Drawer draws the normalized histograms so we will be able to see the spike there
  for (int nbin = 1; nbin < 20; nbin++)
    {
      dc_h_dc1->SetBinContent(nbin, 0.);
      dc_h_dc2->SetBinContent(nbin, 0.);
    }

  //11April2011 benjil: to reduce the chances of having "early" peaks that are slightly above the
  //                    1/2 max value.  I changed the search method to look for the 1/2 max value by
  //                    searching 'backward' (to the left) from the peak value.
  check_n1 = dc_h_dc1->GetMaximumBin();
  check_bin_content1 = dc_h_dc1->GetBinContent(check_n1);
  check_max1 = dc_h_dc1->GetMaximum();
  while ((check_bin_content1 > (check_max1 / 2)) && check_n1 > 0)
    {
      check_bin_content1 = dc_h_dc1->GetBinContent(check_n1);
      check_n1--;
    }
  check_n1 = check_n1 + 2;//+1 for the while loop, +1 to get the upper side of t0

  check_n2 = dc_h_dc2->GetMaximumBin();
  check_bin_content2 = dc_h_dc2->GetBinContent(check_n2);
  check_max2 = dc_h_dc2->GetMaximum();
  while ((check_bin_content2 > (check_max2 / 2)) && check_n2 > 0)
    {
      check_bin_content2 = dc_h_dc2->GetBinContent(check_n2);
      check_n2--;
    }
  check_n2 = check_n2 + 2;//+1 for the while loop, +1 to get the upper side of t0

  //T-Zero message alerts
  if ((abs(template_tzero_east - check_n1) > 5) && 1==0) //disable tzero message alert -benjil 23March2011
    {
      ostringstream msg;

      msg << "East T-Zero has shifted by " << template_tzero_east - check_n1 << " time bins." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_WARNING, msg.str(), 5);
      msg.str("");
    }
  if ((abs(template_tzero_west - check_n2) > 5) && 1==0) //disable tzero message alert -benjil 23March2011
    {
      ostringstream msg;

      msg << "West T-Zero has shifted by " << template_tzero_west - check_n2 << " time bins." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_WARNING, msg.str(), 5);
    }


  tzero_east = check_n1;
  tzero_west = check_n2;

  return 0;
}



int DchMon::update_drift_velocities()
{
  if (nevt < 2*stat_threshold)
    {
      return 0;
    }
  if (NCOUNTS < update_rate)
    {
      return 0;
    }

  //calulate dv


  //////////// Drift Velocity East Calculation
  TF1 *vdfit = new TF1("vdfit", "[0]/(exp(-(x-[2])/[1])+1)");
  int Midbin = int (driftvelocity_east->GetNbinsX() / 2.);
  int check_n1 = int (driftvelocity_east->GetNbinsX() / 2.);
  float check_east = driftvelocity_east->GetBinContent(Midbin);
  // assume the minimum point in the DV distribution is the last bin.
  int Maxbin = int (driftvelocity_east->GetNbinsX());
  float min_value = (driftvelocity_east->GetBinContent(Maxbin));
  // find the bin where the value of the DV distribution is half the plateau (check_east).  
  // (check_east+min_value) was substituted due to occasional pedestal.
  // To reduce the chance that a fluctuation (due to low statistics created the condition of 
  //   1/2 the plateau, search for 3 consecutive bins that satisfy the condition (n1, n1+1, n1+2). 
  while ((driftvelocity_east->GetNbinsX() > check_n1) && ( (driftvelocity_east->GetBinContent(check_n1++) > ((check_east+min_value) / 2)) || (driftvelocity_east->GetBinContent(check_n1+1) > ((check_east+min_value) / 2)) || (driftvelocity_east->GetBinContent(check_n1+2) > ((check_east+min_value) / 2)) ))
    { }
  check_n1--;
  Int_t factor = int (driftvelocity_east->GetEntries());
  float left = driftvelocity_east->GetBinCenter(check_n1 - 60);
  float right = driftvelocity_east->GetBinCenter(check_n1 + 5);
  vdfit->SetParameter(2, driftvelocity_east->GetBinCenter(check_n1));
  vdfit->SetParLimits(2, left, right);
  vdfit->SetParameter(0, factor / 200.0);
  vdfit->SetParameter(1, -9);
  vdfit->SetRange(left, right);
  driftvelocity_east->Fit("vdfit", "NQ", "", left, right);
  float newdv_east = 1.0 / vdfit->GetParameter(2);

   //write out the parameters of the fit to the screen
  //cout << "fit parameters for east arm: " << vdfit->GetParameter(0) << " " << vdfit->GetParameter(1) << " " << vdfit->GetParameter(2) << endl;


  //////////// Drift Velocity West Calculation
  Midbin = int (driftvelocity_west->GetNbinsX() / 2.);
  int check_n2 = int (driftvelocity_west->GetNbinsX() / 2.);
  float check_west = driftvelocity_west->GetBinContent(Midbin);
  // assume the minimum point in the DV distribution is the last bin.
  Maxbin = int (driftvelocity_west->GetNbinsX());
  min_value = (driftvelocity_west->GetBinContent(Maxbin));
  // find the bin where the value of the DV distribution is half the plateau (check_west).  
  // (check_west+min_value) was substituted due to occasional pedestal.
  // To reduce the chance that a fluctuation (due to low statistics created the condition of 
  //   1/2 the plateau, search for 3 consecutive bins that satisfy the condition (n1, n1+1, n1+2). 
  while ((driftvelocity_west->GetNbinsX() > check_n2) && ( (driftvelocity_west->GetBinContent(check_n2++) > ((check_west+min_value) / 2)) || (driftvelocity_west->GetBinContent(check_n2+1) > ((check_west+min_value) / 2)) || (driftvelocity_west->GetBinContent(check_n2+2) > ((check_west+min_value) / 2)) ))
    { }
  check_n2--;
  factor = int (driftvelocity_west->GetEntries());
  left = driftvelocity_west->GetBinCenter(check_n2 - 60);
  right = driftvelocity_west->GetBinCenter(check_n2 + 5);
  vdfit->SetParameter(2, driftvelocity_west->GetBinCenter(check_n2));
  vdfit->SetParLimits(2, left, right);
  vdfit->SetParameter(0, factor / 200.0);
  vdfit->SetParameter(1, -9);
  vdfit->SetRange(left, right);
  driftvelocity_west->Fit("vdfit", "NQ", "", left, right);
  float newdv_west = 1.0 / vdfit->GetParameter(2);


  // The following are output statements only used during debugging and optimization
  /*
  write out the parameters of the fit to the screen
  cout << "fit parameters for west arm: " << vdfit->GetParameter(0) << " " << vdfit->GetParameter(1) << " " << vdfit->GetParameter(2) << endl;
  ostringstream msg;
  cout << endl;
  cout << "check_west " << check_west << " min_value = " << min_value << endl;
  cout << "west: left,val(check_n2),right = " << left << " , " << driftvelocity_west->GetBinCenter(check_n2) << " , " << right << endl;

  msg << "DV East (um/ns) = " << newdv_east*10000;
  cout << msg.str().c_str() << endl;
  msg.str("");
  msg << "DV West (um/ns) = " << newdv_west*10000;
  cout << msg.str().c_str() << endl;
  */

  //Drift Velocity message alerts
  if ((fabs(template_vdrift_east - newdv_east) / 0.42 > 10) && 1==0) //disable dv message alert -benjil 23March2011
    {
      ostringstream msg;
      msg << "East Drift Velocity has shifted by " << template_vdrift_east - newdv_east << " cm/ns." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_INFORMATIONAL, msg.str(), 6);
    }
  if ((fabs(template_vdrift_west - newdv_west) / 0.42 > 10) && 1==0) //disable dv message alert -benjil 23March2011
    {
      ostringstream msg;
      msg << "West Drift Velocity has shifted by " << template_vdrift_west - newdv_west << " cm/ns." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DC, MSG_SEV_INFORMATIONAL, msg.str(), 6);
    }


  vdrift_east = newdv_east;
  vdrift_west = newdv_west;

  delete vdfit;

  return 0;
}




void DchMon::calculate_density_averages()
{

  //calculate the meadian of the hit rates distributions

  float x1x2sum[NUMARM][NUMSIDE];
  float uv1sum[NUMARM][NUMSIDE];
  float u2sum[NUMARM][NUMSIDE];
  float v2sum[NUMARM][NUMSIDE];

  memset(x1x2sum, 0, sizeof(x1x2sum));
  memset(uv1sum, 0, sizeof(uv1sum));
  memset(u2sum, 0, sizeof(u2sum));
  memset(v2sum, 0, sizeof(v2sum));

  float countsx1x2[NUMARM][NUMSIDE];
  float countsuv1[NUMARM][NUMSIDE];
  float countsu2[NUMARM][NUMSIDE];
  float countsv2[NUMARM][NUMSIDE];
  memset(countsx1x2, 0, sizeof(countsx1x2));
  memset(countsuv1, 0, sizeof(countsuv1));
  memset(countsu2, 0, sizeof(countsu2));
  memset(countsv2, 0, sizeof(countsv2));

  memset(hitrate_x1x2average, 0, sizeof(hitrate_x1x2average));
  memset(hitrate_uv1average, 0, sizeof(hitrate_uv1average));
  memset(hitrate_u2average, 0, sizeof(hitrate_u2average));
  memset(hitrate_v2average, 0, sizeof(hitrate_v2average));

  for (int arm = 0; arm < NUMARM; arm++)
    {
      for (int side = 0; side < NUMSIDE; side++)
        {
          for (int p = 0; p < 40; p++)
            {
              for (int b = 0; b < 80; b++)
                {

                  float dens;
                  if (arm == 0)
                    {
                      if (side == 0)
                        {
                          dens = dc_dens_00->GetBinContent(b + 1, p + 1);
                        }
                      else
                        {
                          dens = dc_dens_01->GetBinContent(b + 1, p + 1);
                        }
                    }
                  else
                    {
                      if (side == 0)
                        {
                          dens = dc_dens_10->GetBinContent(b + 1, p + 1);
                        }
                      else
                        {
                          dens = dc_dens_11->GetBinContent(b + 1, p + 1);
                        }
                    }

                  if (dens < dead_threshold || dens > noise_threshold)
                    {
                      continue;
                    }

                  if ( p < 12 || (p > 19 && p < 32) )  //X1X2
                    {
                      x1x2sum[arm][side] += dens;
                      countsx1x2[arm][side]++;
                    }
                  else if ( p > 11 && p < 20 )    //UV1
                    {
                      uv1sum[arm][side] += dens;
                      countsuv1[arm][side]++;
                    }
                  else if ( p > 31 && p < 36 )  //U2
                    {
                      u2sum[arm][side] += dens;
                      countsu2[arm][side]++;
                    }
                  else if ( p > 35 )  //V2
                    {
                      v2sum[arm][side] += dens;
                      countsv2[arm][side]++;
                    }
                  else
                    {
                      exit(1);
                    }
                }
            }
        }
    }




  for (int arm = 0; arm < NUMARM; arm++)
    {
      for (int side = 0; side < NUMSIDE; side++)
        {
          if (countsx1x2[arm][side])
            {
              hitrate_x1x2average[arm][side] = x1x2sum[arm][side] / countsx1x2[arm][side];
            }
          if (countsuv1[arm][side])
            {
              hitrate_uv1average[arm][side] = uv1sum[arm][side] / countsuv1[arm][side];
            }
          if (countsu2[arm][side])
            {
              hitrate_u2average[arm][side] = u2sum[arm][side] / countsu2[arm][side];
            }
          if (countsv2[arm][side])
            {
              hitrate_v2average[arm][side] = v2sum[arm][side] / countsv2[arm][side];
            }

          printf("CALC: arm: %d side: %d X1X2 average %f\n", arm, side, hitrate_x1x2average[arm][side]);
          printf("CALC: arm: %d side: %d UV1  average %f\n" , arm, side, hitrate_uv1average[arm][side]) ;
          printf("CALC: arm: %d side: %d U2   average %f\n", arm, side, hitrate_u2average[arm][side]) ;
          printf("CALC: arm: %d side: %d V2   average %f\n", arm, side, hitrate_v2average[arm][side]) ;

          //printf("%f  %f   %f   %f \n", x1x2sum[arm][side],uv1sum[arm][side],u2sum[arm][side],v2sum[arm][side]);
          //printf("%f  %f   %f   %f \n", countsx1x2[arm][side],countsuv1[arm][side],countsu2[arm][side],countsv2[arm][side]);
          //printf("%f  %f   %f   %f \n",hitrate_x1x2average[arm][side],hitrate_uv1average[arm][side],hitrate_u2average[arm][side],hitrate_v2average[arm][side]);

          if ( hitrate_x1x2average[arm][side] <= 0. || hitrate_uv1average[arm][side] <= 0.
               || hitrate_u2average[arm][side] <= 0. || hitrate_v2average[arm][side] <= 0.)
            {
              ostringstream msg;
              msg << "INVALID AVERAGE!!! Aborting. Call an expert!" ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_DC, MSG_SEV_FATAL, msg.str(), 7);
              //exit(1);
            }

        }
    }



  //////////////////////////////////////////////////////////////////////////////////////////
  //Fill the shuffle boards (histos with averages that will divide the densisty histograms)
  //////////////////////////////////////////////////////////////////////////////////////////
  s00_average->Reset();
  s01_average->Reset();
  s10_average->Reset();
  s11_average->Reset();
  TH1 *s = NULL;
  int c = 0;

  for (int arm = 0; arm < NUMARM; arm++)
    {
      for (int side = 0; side < NUMSIDE; side++)
        {
          for (int p = 0; p < 40; p++)
            {
              for (int b = 0; b < 80; b++)
                {

                  if (!arm)
                    {
                      if (!side)
                        {
                          s = s00_average;
                        }
                      else
                        {
                          s = s01_average;
                        }
                    }
                  else
                    {
                      if (!side)
                        {
                          s = s10_average;
                        }
                      else
                        {
                          s = s11_average;
                        }

                    }
                  if (s == NULL)
                    {
                      exit(1);
                    }

                  if ( p < 12 || (p > 19 && p < 32) )  //X1X2
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_x1x2average[arm][side]);
                      c++;
                    }
                  else if ( p > 11 && p < 20 )    //UV1
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_uv1average[arm][side]);
                      c++;
                    }
                  else if ( p > 31 && p < 36 )  //U2
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_u2average[arm][side]);
                      c++;
                    }
                  else if ( p > 35 )  //V2
                    {
                      s->SetBinContent(b + 1, p + 1, hitrate_v2average[arm][side]);
                      c++;
                    }
                }
            }
        }
    }

}




void DchMon::encodeInfoToHistogram()
{

  dc_info->SetBinContent( RUNNUM_BIN , runNum );
  dc_info->SetBinContent( NEVT_BIN , nevt );

  dc_info->SetBinContent( OLD_NOISE_NUM_00_BIN , old_noise_num_00 );
  dc_info->SetBinContent( OLD_NOISE_NUM_01_BIN , old_noise_num_01 );
  dc_info->SetBinContent( OLD_NOISE_NUM_10_BIN , old_noise_num_10 );
  dc_info->SetBinContent( OLD_NOISE_NUM_11_BIN , old_noise_num_11 );

  dc_info->SetBinContent( NEW_NOISE_NUM_00_BIN , new_noise_num_00 );
  dc_info->SetBinContent( NEW_NOISE_NUM_01_BIN , new_noise_num_01 );
  dc_info->SetBinContent( NEW_NOISE_NUM_10_BIN , new_noise_num_10 );
  dc_info->SetBinContent( NEW_NOISE_NUM_11_BIN , new_noise_num_11 );

  dc_info->SetBinContent( MISSING_NOISE_NUM_00_BIN , missing_noise_num_00 );
  dc_info->SetBinContent( MISSING_NOISE_NUM_01_BIN , missing_noise_num_01 );
  dc_info->SetBinContent( MISSING_NOISE_NUM_10_BIN , missing_noise_num_10 );
  dc_info->SetBinContent( MISSING_NOISE_NUM_11_BIN , missing_noise_num_11 );

  dc_info->SetBinContent( AVG_LOAD_00_BIN , avg_load_00 );
  dc_info->SetBinContent( AVG_LOAD_01_BIN , avg_load_01 );
  dc_info->SetBinContent( AVG_LOAD_10_BIN , avg_load_10 );
  dc_info->SetBinContent( AVG_LOAD_11_BIN , avg_load_11 );

  dc_info->SetBinContent( DEAD_REPORT_BIN , dead_report );
  dc_info->SetBinContent( INFOGROUP_BIN , infogroup );

  //caution: the new_noisy.. and new_dead_... mean something different than the above new_noise_num_??
  //the above variables measure the noisy channels that are new for this run while the values below
  //measure the number of noisy channels in the active (new) run.
  //I will changes these names in the future
  dc_info->SetBinContent( TEMPLATE_NOISY_EAST_BIN , template_noisy_east_n );
  dc_info->SetBinContent( TEMPLATE_NOISY_WEST_BIN , template_noisy_west_n );
  dc_info->SetBinContent( TEMPLATE_DEAD_EAST_BIN , template_dead_east_n );
  dc_info->SetBinContent( TEMPLATE_DEAD_WEST_BIN , template_dead_west_n );
  dc_info->SetBinContent( TEMPLATE_TZERO_EAST_BIN , template_tzero_east );
  dc_info->SetBinContent( TEMPLATE_TZERO_WEST_BIN , template_tzero_west );
  dc_info->SetBinContent( TEMPLATE_VDRIFT_EAST_BIN , template_vdrift_east );
  dc_info->SetBinContent( TEMPLATE_VDRIFT_WEST_BIN , template_vdrift_west );
  dc_info->SetBinContent( TEMPLATE_EFF_EAST_BIN , template_eff_east );
  dc_info->SetBinContent( TEMPLATE_EFF_WEST_BIN , template_eff_west );

  dc_info->SetBinContent( TEMPLATE_LLOAD_ALERT_RED_BIN , template_LLOAD_ALERT_RED ); //DLAN
  dc_info->SetBinContent( TEMPLATE_LLOAD_ALERT_YEL_BIN , template_LLOAD_ALERT_YEL ); // Store these here so they can
  dc_info->SetBinContent( TEMPLATE_HLOAD_ALERT_RED_BIN , template_HLOAD_ALERT_RED ); // be read in DchMonDraw.C and used
  dc_info->SetBinContent( TEMPLATE_HLOAD_ALERT_YEL_BIN , template_HLOAD_ALERT_YEL ); // to set color alerts for LOAD

  dc_info->SetBinContent( NOISY_EAST_BIN , new_noisy_east_n );
  dc_info->SetBinContent( NOISY_WEST_BIN , new_noisy_west_n );
  dc_info->SetBinContent( DEAD_EAST_BIN , new_dead_east_n );
  dc_info->SetBinContent( DEAD_WEST_BIN , new_dead_west_n );
  dc_info->SetBinContent( TZERO_EAST_BIN , tzero_east );
  dc_info->SetBinContent( TZERO_WEST_BIN , tzero_west );
  dc_info->SetBinContent( VDRIFT_EAST_BIN , vdrift_east );
  dc_info->SetBinContent( VDRIFT_WEST_BIN , vdrift_west );
  dc_info->SetBinContent( EFF_EAST_BIN , eff_east );
  dc_info->SetBinContent( EFF_WEST_BIN , eff_west );
  dc_info->SetBinContent( BADEV_BIN , bad_events );

} //encode



void DchMon::reset_arrays_histograms()
{
  NCOUNTS = 0;

  dc_h_dc1->Reset();
  dc_h_dc2->Reset();
  driftvelocity_east->Reset();
  driftvelocity_west->Reset();

  dchek1->Reset();
  dchek2->Reset();
  dchek3->Reset();
  dchek4->Reset();

  tmp_ek1_1->Reset();
  tmp_ek2_2->Reset();
  tmp_ek3_3->Reset();
  tmp_ek4_4->Reset();

  //Zero hits_array
  memset( hits_array, 0, sizeof( hits_array));
  printf("Resetting...Done\n") ;
}



int DchMon::Reset()
{
  dchek1->Reset();
  dchek2->Reset();
  dchek3->Reset();
  dchek4->Reset();

  tmp_ek1_1->Reset();
  tmp_ek2_2->Reset();
  tmp_ek3_3->Reset();
  tmp_ek4_4->Reset();

  dc_h_dc1->Reset();
  dc_h_dc2->Reset();
  driftvelocity_east->Reset();
  driftvelocity_west->Reset();


  //Zero hits_array
  memset(hits_array, 0, sizeof(hits_array));

  //   //Zero old_noise_array
  //   for (int i = 0; i < NOISEARRAYSIZE; i++)
  //     {
  //       old_noise_array[i] = 0;
  //     }


  //   //Zero old_dead_array
  //   for (int i = 0; i < DEADARRAYSIZE; i++)
  //     {
  //       old_dead_array[i] = 0;
  //     }


  NCOUNTS = 0;
  nevt = 0;

  ncalls = 1;
  infogroup = 0;
  dead_report = 0;
  runNum = 0;
  new_noisy_n = 0;
  new_dead_n = 0;

  new_noisy_east_n = 0;
  new_noisy_west_n = 0;
  new_dead_east_n = 0;
  new_dead_west_n = 0;

  report1 = 0;
  report2 = 0;
  tzero_east = 0;
  tzero_west = 0;
  vdrift_east = 0;
  vdrift_west = 0;
  eff_east = 0;
  eff_west = 0;

  old_noise_num_00 = 0;
  old_noise_num_01 = 0;
  old_noise_num_10 = 0;
  old_noise_num_11 = 0;
  new_noise_num_00 = 0;
  new_noise_num_01 = 0;
  new_noise_num_10 = 0;
  new_noise_num_11 = 0;
  missing_noise_num_00 = 0;
  missing_noise_num_01 = 0;
  missing_noise_num_10 = 0;
  missing_noise_num_11 = 0;
  hit_num_00 = 0;
  hit_num_01 = 0;
  hit_num_10 = 0;
  hit_num_11 = 0;
  avg_load_00 = 0;
  avg_load_10 = 0;
  avg_load_01 = 0;
  avg_load_11 = 0;
  bad_events = 0;

  printf("Resetting...Done\n") ;

  return 0;
}
