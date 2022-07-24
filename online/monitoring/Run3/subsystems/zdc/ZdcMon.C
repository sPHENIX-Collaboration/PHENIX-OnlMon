#include <ZdcMon.h>
#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <OnlMonTrigger.h>

#include <ZdcEvent.hh>
#include <ZdcCalib.hh>
#include <BbcEvent.hh>
#include <BbcCalib.hh>

#include <msg_profile.h>

#include <Event.h>
#include <EventTypes.h>
#include <packet.h>

#include <phool.h>

#include <TH1.h>
#include <TH2.h>

#include <gsl/gsl_const.h>
#include <gsl/gsl_math.h>

#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;


// uncommenting DBTEST will call a DB test routine which will add a new
// entry per call
//#define DBTEST

static const int Nlas = 5;
static const int Nsmdpos = 1000;
static const int Nzvert = 1000;
static const int Nzadc = 1000;

ZDCMon::ZDCMon(): OnlMon("ZdcMON")
{
  CreateHistos();
  RegisterHistos();
  GetCalConst();

  zdccalib = new ZdcCalib();
  ostringstream zdccalibdir;
  if (getenv("ZDCCALIBDIR"))
    {
      zdccalibdir << getenv("ZDCCALIBDIR");
    }
  zdccalibdir << "/ZdcCalib" ;
  printf("zdccalibdir: %s\n", zdccalibdir.str().c_str()) ;
  zdccalib->restore(zdccalibdir.str().c_str());
  zdcevent = new ZdcEvent();
  PHTimeStamp now;
  now.setToSystemTime();
  zdcevent->setCalibDataAll(zdccalib,now);

  bbccalib = new BbcCalib();
  bbcevent = new BbcEvent();
  // BBC Calib
  if (strcmp(getenv("BBCCALIBRATION"), "PDBCAL") == 0)
    {
      // store from PdbCal
      // time is from run 66302(Mon Jan 20 11:36:17 2003).
      PHTimeStamp time_bbccalib(2003, 1, 22, 12, 0, 0, 0);
      int calib_version = 1004;
      bbccalib->restore(time_bbccalib, calib_version);

    }
  else if (strcmp(getenv("BBCCALIBRATION"), "FILE") == 0)
    {
      // store from File

      if (!getenv("BBCCALIBDIR"))
        {
          ostringstream msg;
          msg << "Environment variable BBCCALIBDIR not set" ;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_WARNING, msg.str(), 1);
          bbccalib->restore("./BbcCalib");
        }
      else
        {
          ostringstream otmp;
          otmp << getenv("BBCCALIBDIR") << "/BbcCalib";
          bbccalib->restore(otmp.str().c_str());
        }
    }
  else
    {
      ostringstream msg;
      msg << "BBCCALIBRATION not set, Calibration constants of the BBC not loaded correctly" ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 1);
      exit(1);
    }
  bbcevent->setCalibDataAll(bbccalib);

  dbvars = new OnlMonDB(ThisName);
  lastcommit = 0;
  DBVarInit();
  Reset();
}


ZDCMon::~ZDCMon()
{
  delete bbccalib;
  delete bbcevent;
  delete dbvars;
  delete zdccalib;
  delete zdcevent;
}

void ZDCMon::CreateHistos()
{

  // Max range for 200 GeV Au
  const float MAX_ENERGY1 = 4000.;
  const float MAX_ENERGY2 = 4000.;
  const int BIN_NUMBER=1000;

  // Max range for 200 GeV pp
  // const float MAX_ENERGY1 = 400.;
  // const float MAX_ENERGY2 = 400.;
  // const int BIN_NUMBER=100;

  // Max range for 500 GeV pp
  // const float MAX_ENERGY1 = 700.;
  // const float MAX_ENERGY2 = 700.;
  // const int BIN_NUMBER=350;

  // Max range for 200 GeV pAu p:north, Au: south
  //const float MAX_ENERGY1 = 400.;
  //const float MAX_ENERGY2 = 4000.;
  //const int BIN_NUMBER1=100;
  //const int BIN_NUMBER2=1000;

  // 200GeVAu = 4000, d = 500, 200GeVCu = 900, 62.4GeVCu = 250, 62.4GeVpp: 90, 200GeVpp: 300
   zdc_adc_north = new TH1F("zdc_adc_north", "ZDC ADC north", BIN_NUMBER, 0, 100 );
   zdc_adc_south = new TH1F("zdc_adc_south", "ZDC ADC south", BIN_NUMBER, 0, MAX_ENERGY2 );
  //zdc_adc_north = new TH1F("zdc_adc_north", "ZDC ADC north", BIN_NUMBER1, 0, MAX_ENERGY1 );
  //zdc_adc_south = new TH1F("zdc_adc_south", "ZDC ADC south", BIN_NUMBER2, 0, MAX_ENERGY2 );

 /*Ciprian Gal 110211*/
  for(int i=0;i<3;i++)
    {
      char hname[256],htitle[256];
      sprintf(htitle,"ZDC ADC north PMT %d",i);
      sprintf(hname,"zdc_adc_n_ind_%d",i);
      zdc_adc_n_ind[i]=new TH1F(hname, htitle, BIN_NUMBER, 0, MAX_ENERGY1 );
      // zdc_adc_n_ind[i]=new TH1F(hname, htitle, BIN_NUMBER1, 0, MAX_ENERGY1 );

      sprintf(htitle,"ZDC ADC south PMT %d",i);
      sprintf(hname,"zdc_adc_s_ind_%d",i);
      zdc_adc_s_ind[i]=new TH1F(hname, htitle, BIN_NUMBER, 0, MAX_ENERGY1 );
      // zdc_adc_s_ind[i]=new TH1F(hname, htitle, BIN_NUMBER2, 0, MAX_ENERGY2 );

    }

 /*Ciprian Gal 110211*/

  zdc_vertex = new TH1F("zdc_vertex", "ZDC vertex position", 128, -256, 256 );
  zdc_vertex_b = new TH1F("zdc_vertex_b",
                          "ZDC vertex position (BBCLL1 trigger)", 128, -256, 256);
  zdc_vs_bbc = new TH2F("zdc_vs_bbc", "ZDC versus BBC vertex", 128, -63, 64, 128, -63, 64);
  smd_hor_south = new TH1F("smd_hor_south",
                           "Beam centroid distribution, SMD South y", 296, -5.92, 5.92);
  smd_hor_north = new TH1F ("smd_hor_north",
                            "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_hor_north_small = new TH1F ("smd_hor_north_small",
                                  "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_hor_north_good = new TH1F ("smd_hor_north_good",
                                 "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_sum_hor_south = new TH1F ("smd_sum_hor_south", "SMD South y", 640, 0, 2560);
  smd_sum_hor_north = new TH1F ("smd_sum_hor_north", "SMD North y", 512, 0, 2048);
  smd_ver_south = new TH1F("smd_ver_south",
                           "Beam centroid distribution, SMD South x", 220, -5.5, 5.5);
  smd_ver_north = new TH1F("smd_ver_north",
                           "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  smd_ver_north_small = new TH1F ("smd_ver_north_small",
                                  "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  smd_ver_north_good = new TH1F ("smd_ver_north_good",
                                 "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  smd_sum_ver_south = new TH1F ("smd_sum_ver_south", "SMD South x", 640, 0, 2560);
  smd_sum_ver_north = new TH1F ("smd_sum_ver_north", "SMD North x", 512, 0, 2048);

  zdc_hor_north = new TH2F("zdc_hor_north",
                           "North Horizontal SMD vs ZDC", 448, 0, 448, 220, -5.5, 5.5);
  zdc_ver_north = new TH2F("zdc_ver_north",
                           "North Vertical SMD vs ZDC", 448, 0, 448, 296, -5.92, 5.92);

  smd_xy_north = new TH2F("smd_xy_north",
                          "SMD hit position north", 110, -5.5, 5.5, 119, -5.92, 5.92);
  smd_xy_south = new TH2F("smd_xy_south",
                          "SMD hit position south", 110, -5.5, 5.5, 119, -5.92, 5.92);
//   smd_xy_north = new TH2F("smd_xy_north",
//                           "SMD hit position north", 275, -5.5, 5.5, 296, -5.92, 5.92);
//   smd_xy_south = new TH2F("smd_xy_south",
//                           "SMD hit position south", 275, -5.5, 5.5, 296, -5.92, 5.92);
  zdc_value = new TH2F("zdc_value",
                       "ZDC channel# vs value", 4096, 0, 4096, 40, 0, 40);

  smd_value_good = new TH2F("smd_value_good",
                            "SMD channel# vs value", 1024, 0, 4096, 16, 0, 16);
  smd_value_small = new TH2F("smd_value_small",
                             "SMD channel# vs value", 1024, 0, 4096, 16, 0, 16);
  smd_value = new TH2F("smd_value",
                       "SMD channel# vs value", 1024, 0, 4096, 32, 0, 32);

  smd_yt_south = new TH2F("smd_yt_south",
                          "Time dep. of y in SMD south", 100, 0, 100000, 296, -5.92, 5.92);
  smd_xt_south = new TH2F("smd_xt_south",
                          "Time dep. of x in SMD south", 100, 0, 100000, 220, -5.5, 5.5);
  smd_yt_north = new TH2F("smd_yt_north",
                          "Time dep. of y in SMD north", 100, 0, 100000, 296, -5.92, 5.92);
  smd_xt_north = new TH2F("smd_xt_north",
                          "Time dep. of x in SMD north", 100, 0, 100000, 220, -5.5, 5.5);

  zdc_laser_south1 = new TH2F("zdc_laser_south1",
                              "Time dep. LED en., 1. ZDC south", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_south2 = new TH2F("zdc_laser_south2",
                              "Time dep. LED en., 2. ZDC south", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_south3 = new TH2F("zdc_laser_south3",
                              "Time dep. LED en., 3. ZDC south", 200, 0, 100000, 100, 0, 4096);

  zdc_laser_north1 = new TH2F("zdc_laser_north1",
                              "Time dep. LED en., 1. ZDC north", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_north2 = new TH2F("zdc_laser_north2",
                              "Time dep. LED en., 2. ZDC north", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_north3 = new TH2F("zdc_laser_north3",
                              "Time dep. LED en., 3. ZDC north", 200, 0, 100000, 100, 0, 4096);


  zdc_laser_south1b = new TH2F("zdc_laser_south1b",
                               "Time dep. LED tim., 1. ZDC south", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_south2b = new TH2F("zdc_laser_south2b",
                               "Time dep. LED tim., 2. ZDC south", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_south3b = new TH2F("zdc_laser_south3b",
                               "Time dep. LED tim., 3. ZDC south", 200, 0, 100000, 100, 0, 4096);

  zdc_laser_north1b = new TH2F("zdc_laser_north1b",
                               "Time dep. LED tim., 1. ZDC north", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_north2b = new TH2F("zdc_laser_north2b",
                               "Time dep. LED tim., 2. ZDC north", 200, 0, 100000, 100, 0, 4096);
  zdc_laser_north3b = new TH2F("zdc_laser_north3b",
                               "Time dep. LED tim., 3. ZDC north", 200, 0, 100000, 100, 0, 4096);

  zdc_ratio_north = new TH2F("zdc_ratio_north",
                             "N1 to N2+N3 ratio", 200, 0, 4096, 200, 0, 4096);
  zdc_ratio_south = new TH2F("zdc_ratio_south",
                             "S1 to S2+S3 ratio", 200, 0, 4096, 200, 0, 4096);



  ostringstream pedfile, calibdir;
  float temp_zdcped[40];
  calibdir.str("");
  if (getenv("ZDCCALIBDIR"))
    {
      calibdir << getenv("ZDCCALIBDIR");
    }
  else
    {
      calibdir << getenv("ONLMON_MAIN") << "/share";
    }
  //getting pedestals
  pedfile.str("");
  pedfile << calibdir.str().c_str() << "/ZdcCalib.pedestal";
  ifstream ped_infile(pedfile.str().c_str(), ios::in);
  if (!ped_infile)
    {
      ostringstream msg;
      msg << pedfile << " could not be opened." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
      exit(1);
    }
  float col1, col2, col3;
  for (int i = 0; i < 40; i++)
    {
      ped_infile >> col1 >> col2 >> col3;
      temp_zdcped[i] = col1;
    }

  for(int i=0; i<40; i++)
    {
      char name[64];
      char title[64];
      sprintf(name,"zdc_adc_vs_tdc_ch%d",i);
      if( i<4 )
	{
	  sprintf(title,"ADC vs TDC for ZDCLL1wide trig");
	  zdc_adc_vs_tdc[i] = new TH2F(name,title, 50, temp_zdcped[i]-50, temp_zdcped[i]+3850, 128, 0, 4096);
	}
      else if( i<8 )
	{
	  sprintf(title,"ADC vs TDC for ZDCLL1wide trig");
	  zdc_adc_vs_tdc[i] = new TH2F(name,title, 50,  temp_zdcped[i]-50, temp_zdcped[i]+3850,  128, 0, 4096); //for Au
	  //zdc_adc_vs_tdc[i] = new TH2F(name,title, 50,  temp_zdcped[i]-50, temp_zdcped[i]+550,  128, 0, 4096); for p
	}
      else if( i<24 )
	{
	  sprintf(title,"ADC vs TDC for ZDCLL1wide trig");
	  zdc_adc_vs_tdc[i] = new TH2F(name,title, 60, temp_zdcped[i]-50, temp_zdcped[i]+2950, 128, 0, 4096);
	}
      else
	{
	  sprintf(title,"ADC vs TDC for ZDCLL1wide trig");
      	  zdc_adc_vs_tdc[i] = new TH2F(name,title, 60, temp_zdcped[i]-50, temp_zdcped[i]+2950, 128, 0, 4096);
	}
    }

  for(int i=0; i<40; i++)
    {
      char name[64];
      char title[64];
      sprintf(name,"fveto_adc_vs_tdc_ch%d",i);
      if( i<4 )
	{
	  sprintf(title,"Front Veto ADC vs ZDC TDC for ZDCLL1wide trig");
	  fc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000, 64, 0, 4096);
	}
      else if( i<8 )
	{
	  sprintf(title,"Front Veto ADC vs ZDC TDC for ZDCLL1wide trig");
	  fc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000,  64, 0, 4096);
	}
      else if( i<24 )
	{
	  sprintf(title,"Front Veto ADC vs SMD TDC for ZDCLL1wide trig");
	  fc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000,  64, 0, 4096);
	}
      else
	{
	  sprintf(title,"Front Veto TDC vs SMD TDC for ZDCLL1wide trig");
      	  fc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000, 64, 0, 4096);
	}
    }

  for(int i=0; i<40; i++)
    {
      char name[64];
      char title[64];
      sprintf(name,"rveto_adc_vs_tdc_ch%d",i);
      if( i<4 )
	{
	  sprintf(title,"Rear Veto ADC vs ZDC TDC for ZDCLL1wide trig");
	  rc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000,  64, 0, 4096);
	}
      else if( i<8 )
	{
	  sprintf(title,"Rear Veto ADC vs ZDC TDC for ZDCLL1wide trig");
	  rc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000,  64, 0, 4096);
	}
      else if( i<24 )
	{
	  sprintf(title,"Rear Veto ADC vs SMD TDC for ZDCLL1wide trig");
	  rc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000,  64, 0, 4096);
	}
      else
	{
	  sprintf(title,"Rear Veto TDC vs SMD TDC for ZDCLL1wide trig");
      	  rc_adc_vs_zdc_tdc[i] = new TH2F(name,title, 50, 500, 4000,  64, 0, 4096);
	}
    }
  

  for(int i=0; i<8; i++)
    {
      char name[64];
      char title[8][64];
      sprintf(title[0],"ADC ch42 (Sf) vs SMD.S Y");
      sprintf(title[1],"ADC ch42 (Sf) vs SMD.S X");
      sprintf(title[2],"ADC ch43 (Sr) vs SMD.S Y");
      sprintf(title[3],"ADC ch43 (Sr) vs SMD.S X");
      sprintf(title[4],"ADC ch40 (Nf) vs SMD.N Y");
      sprintf(title[5],"ADC ch40 (Nf) vs SMD.N X");
      sprintf(title[6],"ADC ch41 (Nr) vs SMD.N Y");
      sprintf(title[7],"ADC ch41 (Nr) vs SMD.N X");
      
      sprintf(name,"veto_adc_vs_pos_%d",i);
      veto_adc_vs_pos[i] = new TH2F(name,title[i], 140, 596, 4096, 20, -5, 5 );
    } 

 
}

void ZDCMon::RegisterHistos()
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  Onlmonserver->registerHisto(this, zdc_adc_north );
  Onlmonserver->registerHisto(this, zdc_adc_south );
  /*Ciprian Gal 110211*/
  for(int i=0;i<3;i++)
    {
      Onlmonserver->registerHisto(this, zdc_adc_n_ind[i]);
      Onlmonserver->registerHisto(this, zdc_adc_s_ind[i]);
    }

  Onlmonserver->registerHisto(this, smd_hor_south);
  Onlmonserver->registerHisto(this, smd_hor_north);
  Onlmonserver->registerHisto(this, smd_hor_north_small);
  Onlmonserver->registerHisto(this, smd_hor_north_good);
  Onlmonserver->registerHisto(this, smd_sum_hor_south);
  Onlmonserver->registerHisto(this, smd_sum_ver_south);
  Onlmonserver->registerHisto(this, smd_ver_south);
  Onlmonserver->registerHisto(this, smd_ver_north);
  Onlmonserver->registerHisto(this, smd_ver_north_small);
  Onlmonserver->registerHisto(this, smd_ver_north_good);
  Onlmonserver->registerHisto(this, smd_sum_hor_north);
  Onlmonserver->registerHisto(this, smd_sum_ver_north);
  Onlmonserver->registerHisto(this, zdc_vertex);
  Onlmonserver->registerHisto(this, zdc_vertex_b);
  Onlmonserver->registerHisto(this, zdc_vs_bbc);

  Onlmonserver->registerHisto(this, zdc_hor_north);
  Onlmonserver->registerHisto(this, zdc_ver_north);
  Onlmonserver->registerHisto(this, smd_xy_north);
  Onlmonserver->registerHisto(this, smd_xy_south);
  Onlmonserver->registerHisto(this, zdc_value);

  Onlmonserver->registerHisto(this, smd_value);
  Onlmonserver->registerHisto(this, smd_value_good);
  Onlmonserver->registerHisto(this, smd_value_small);

  Onlmonserver->registerHisto(this, smd_yt_south);
  Onlmonserver->registerHisto(this, smd_xt_south);
  Onlmonserver->registerHisto(this, smd_yt_north);
  Onlmonserver->registerHisto(this, smd_xt_north);
  Onlmonserver->registerHisto(this, zdc_laser_south1);
  Onlmonserver->registerHisto(this, zdc_laser_south2);
  Onlmonserver->registerHisto(this, zdc_laser_south3);
  Onlmonserver->registerHisto(this, zdc_laser_north1);
  Onlmonserver->registerHisto(this, zdc_laser_north2);
  Onlmonserver->registerHisto(this, zdc_laser_north3);
  Onlmonserver->registerHisto(this, zdc_laser_south1b);
  Onlmonserver->registerHisto(this, zdc_laser_south2b);
  Onlmonserver->registerHisto(this, zdc_laser_south3b);
  Onlmonserver->registerHisto(this, zdc_laser_north1b);
  Onlmonserver->registerHisto(this, zdc_laser_north2b);
  Onlmonserver->registerHisto(this, zdc_laser_north3b);

  Onlmonserver->registerHisto(this, zdc_ratio_north);
  Onlmonserver->registerHisto(this, zdc_ratio_south);

  for(int i=0;i<40;i++)
    Onlmonserver->registerHisto(this,  zdc_adc_vs_tdc[i]);

 for(int i=0; i<40; i++)
   {
     Onlmonserver->registerHisto(this,  fc_adc_vs_zdc_tdc[i] );
     Onlmonserver->registerHisto(this,  rc_adc_vs_zdc_tdc[i] );
   }


  for(int i=0; i<8; i++)
    Onlmonserver->registerHisto(this,  veto_adc_vs_pos[i]);
  
}

void ZDCMon::GetCalConst()
{
  ostringstream pedfile, gainfile, ovf0file, ovf1file, calibdir;
  //getting directory where the calibration files are
  calibdir.str("");
  if (getenv("ZDCCALIBDIR"))
    {
      calibdir << getenv("ZDCCALIBDIR");
    }
  else
    {
      calibdir << getenv("ONLMON_MAIN") << "/share";
    }
  //getting pedestals
  pedfile.str("");
  pedfile << calibdir.str().c_str() << "/ZdcCalib.pedestal";
  ifstream ped_infile(pedfile.str().c_str(), ios::in);
  if (!ped_infile)
    {
      ostringstream msg;
      msg << pedfile << " could not be opened." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
      exit(1);
    }
  float col1, col2, col3;
  for (int i = 0; i < 40; i++)
    {
      ped_infile >> col1 >> col2 >> col3;
      pedestal[i] = col1;
    }
  //writing pedestals in two different arrays
  for (int i = 0; i < 8; i++)
    {
      zdc_ped[i] = pedestal[i];
    }
  for (int i = 8; i < 40; i++)
    {
      smd_ped[i - 8] = pedestal[i];
    }
  //getting gains
  gainfile.str("");
  gainfile << calibdir.str().c_str() << "/ZdcCalib.pmtgain";
  ifstream gain_infile(gainfile.str().c_str(), ios::in);
  if (!gain_infile)
    {
      ostringstream msg;
      msg << gainfile << " could not be opened." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
      exit(1);
    }

  for (int i = 0; i < 32; i++)
    {
      gain_infile >> col1 >> col2 >> col3;
      gain[i] = col1;
    }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
    {
      smd_south_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
    }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
    {
      smd_north_rgain[i] = gain[i + 16];  // 0-7: y channels, 8-14: x channels, 15: analog sum
    }

  for (int i = 0; i < 8; i++)
    {
      // relative gain of the y strips to the x strips at SMD north
      smd_north_rgain[i] = smd_north_rgain[i] * 1.610;
      // relative gain of the y strips to the x strips at SMD south
      smd_south_rgain[i] = smd_south_rgain[i] * 1.715;
    }
  // relative gain of the strips at SMD south  to the strips at SMD north
  for (int i = 0; i < 16; i++)
    {
      //      smd_south_rgain[i] = smd_south_rgain[i] * 1.012;
      smd_south_rgain[i] = smd_south_rgain[i] * 1.5; // From RUN9
    }
  //getting overflows
  ovf0file.str("");
  ovf0file << calibdir.str().c_str() << "/ZdcCalib.overflow0";
  ifstream ovf0_infile(ovf0file.str().c_str(), ios::in);
  if (!ovf0_infile)
    {
      ostringstream msg;
      msg << ovf0file << " could not be opened." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
      exit(1);
    }
  float col5, col6, col7;
  for (int i = 0; i < 40; i++)
    {
      ovf0_infile >> col5 >> col6 >> col7;
      overflow0[i] = 3800; //for now using 3800 instead of the values in the file
    }
  ovf1file.str("");
  ovf1file << calibdir.str().c_str() << "/ZdcCalib.overflow1";
  ifstream ovf1_infile(ovf1file.str().c_str(), ios::in);
  if (!ovf1_infile)
    {
      ostringstream msg;
      msg << ovf1file << " could not be opened." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
      exit(1);
    }
  float col8, col9, col10;
  for (int i = 0; i < 40; i++)
    {
      ovf1_infile >> col8 >> col9 >> col10;
      overflow1[i] = 3800; //for now using 3800 instead of the values in the file
    }
}

int ZDCMon::CompTrigFire(Event *evt) //compute booleans for trigger
{
  OnlMonServer *se = OnlMonServer::instance();
  
  did_BbcLvl1_fire = se->OnlTrig()->didLevel1TriggerFire("BBCLL1(>1 tubes)");
  did_BbcLvl1_fire |= se->OnlTrig()->didLevel1TriggerFire("BBCLL1(>0 tubes)");
  //true, if Level1TriggerFired...
  did_laser_fire = se->OnlTrig()->didLevel1TriggerFire("PPG(Laser)");
  //  did_zdcns_fire = se->OnlTrig()->didLevel1TriggerFire("ZDCNS");
  did_zdcns_fire = se->OnlTrig()->didLevel1TriggerFire("ZDCLL1wide");
  //  did_zdclw_fire = se->OnlTrig()->didLevel1TriggerFire("ZDCLL1wide");
  //  did_zdcln_fire = se->OnlTrig()->didLevel1TriggerFire("ZDCLL1narrow");

  did_zdc_north_fire =  se->OnlTrig()->didLevel1TriggerFire("ZDCLL1wide");
  did_zdc_south_fire =  se->OnlTrig()->didLevel1TriggerFire("ZDCLL1wide");

//  did_zdc_north_fire =  se->OnlTrig()->didLevel1TriggerFire("ZDCN_BL");
//  did_zdc_south_fire =  se->OnlTrig()->didLevel1TriggerFire("ZDCS");

  return 0;
}
void ZDCMon::GetRawAdcTdc() //Get Raw ADC/TDC values
{
  for (int i = 0; i < 44; i++)
    {
      zdcraw_adc[i]  = p->iValue(i);
      zdcraw_tdc0[i] = p->iValue(i, "T1");
      zdcraw_tdc1[i] = p->iValue(i, "T2");
    }
}
void ZDCMon::CompZdcAdc() //substracting pedestals
{
  for (int i = 0; i < 8; i++)
    {
      float temp = p->iValue(i) - zdc_ped[i];
      //if(temp < 0.0 )
      //  temp = 0.0;
      zdc_adc[i] = temp;
    }
}

void ZDCMon::CompSmdAdc() //substacting pedestals and multiplying with gains
{
  for (int i = 8; i < 40; i++)
    {
      float temp = p->iValue(i) - smd_ped[i - 8];
      //if (temp < 0.0 )
      //  temp = 0.0;
      smd_adc[i - 8] = temp;
    }

  for (int i = 0; i < 15; i++)
    {
      // multiply SMD channels with their gain factors
      // to get the absolute ADC values in the same units
      smd_adc[i] = smd_adc[i] * smd_south_rgain[i];
      smd_adc[i + 16] = smd_adc[i + 16] * smd_north_rgain[i];
    }
}

void ZDCMon::CompSmdPos() //computing position with weighted averages
{
  float w_ave[4]; // 1. south hor. 2. south vert. 3. north hor. 4. north vert.
  float weights[4] = {0};
  memset(weights, 0, sizeof(weights)); // memset float works only for 0
  float w_sum[4];
  memset(w_sum, 0, sizeof(w_sum));

  // these constants convert the SMD channel number into real dimensions (cm's)
  const float hor_scale = 2.0 * 11.0 / 10.5 * sin(M_PI_4); // from gsl_math.h
  const float ver_scale = 1.5 * 11.0 / 10.5;
  float hor_offset = (hor_scale * 8 / 2.0) * (7.0 / 8.0);
  float ver_offset = (ver_scale * 7 / 2.0) * (6.0 / 7.0);

  for (int i = 0; i < 8; i++)
    {
      weights[0] += smd_adc[i]; //summing weights
      weights[2] += smd_adc[i + 16];
      w_sum[0] += (float)i * smd_adc[i]; //summing for the average
      w_sum[2] += ((float)i + 16.) * smd_adc[i + 16];
    }
  for (int i = 0; i < 7; i++)
    {
      weights[1] += smd_adc[i + 8];
      weights[3] += smd_adc[i + 24];
      w_sum[1] += ((float)i + 8.) * smd_adc[i + 8];
      w_sum[3] += ((float)i + 24.) * smd_adc[i + 24];
    }

  if ( weights[0] > 0.0 )
    {
      w_ave[0] = w_sum[0] / weights[0]; //average = sum / sumn of weights...
      smd_pos[0] = hor_scale * w_ave[0] - hor_offset;
    }
  else
    {
      smd_pos[0] = 0;
    }
  if ( weights[1] > 0.0 )
    {
      w_ave[1] = w_sum[1] / weights[1];
      smd_pos[1] = ver_scale * (w_ave[1] - 8.0) - ver_offset;
    }
  else
    {
      smd_pos[1] = 0;
    }

  if ( weights[2] > 0.0 )
    {
      w_ave[2] = w_sum[2] / weights[2];
      smd_pos[2] = hor_scale * (w_ave[2] - 16.0) - hor_offset;
    }
  else
    {
      smd_pos[2] = 0;
    }

  if ( weights[3] > 0.0 )
    {
      w_ave[3] = w_sum[3] / weights[3];
      smd_pos[3] = ver_scale * (w_ave[3] - 24.0) - ver_offset;
    }
  else
    {
      smd_pos[3] = 0;
    }
}

void ZDCMon::CompSumSmd() //compute 'digital' sum
{
  memset(smd_sum, 0, sizeof(smd_sum));

  for (int i = 0; i < 8; i++)
    {
      smd_sum[0] += smd_adc[i];
      smd_sum[2] += smd_adc[i + 16];
    }
  for (int i = 0; i < 7; i++)
    {
      smd_sum[1] += smd_adc[i + 8];
      smd_sum[3] += smd_adc[i + 24];
    }
}

void ZDCMon::CompOvfBool() //compute booleans for overflow
{
  for (int i = 0; i < 40; i++)
    {
      ovfbool[i] = ((p->iValue(i, "T0") < overflow0[i]) && (p->iValue(i, "T1") < overflow1[i]));
    }
}

int ZDCMon::process_event(Event *evt) //this runs for every event
{
  if (evt ->getEvtType() != DATAEVENT) //using only dataevents
    {
      return 0;
    }

  nevt++;

  p = evt->getPacket(ZDC_PACKET_ID); //this gets the package for ZDC/SMD

  if ( !p )
    {
      ostringstream msg;
      msg << "ZDC packet not found." ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_WARNING, msg.str(), 3);
      return 0;
    }

  else
    {
      //calculate ZDC
      zdcevent->Clear();
      zdcevent->setRawData(evt);
      zdcevent->calculate();
      //calculate BBC
      bbcevent->Clear();
      bbcevent->setRawData(evt);
      bbcevent->calculate();

      did_BbcLvl1_fire = true;
      did_laser_fire = false;

      //compute zdc and smd output values
      GetRawAdcTdc();
      CompZdcAdc();
      CompSmdAdc();
      CompSmdPos();
      CompSumSmd();
      int iret = CompTrigFire(evt);
      if (iret)
        {
          delete p;
          return iret;
        } // if trigger selection does not work, return.

      
      if( did_zdc_north_fire )
	{
	  for(int i=4; i<8; i++)
	    if( zdcraw_tdc1[i] < 3800 )
	      {		
		zdc_adc_vs_tdc[i]->Fill(zdcraw_adc[i], zdcraw_tdc1[i]);
		fc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[40], zdcraw_tdc1[i]);
		rc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[41], zdcraw_tdc1[i]);
	      }
	   

	  for(int i=24; i<40; i++)
	    if( zdcraw_tdc1[i] < 3800 )
	      {
		zdc_adc_vs_tdc[i]->Fill(zdcraw_adc[i], zdcraw_tdc1[i]);
		fc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[40], zdcraw_tdc1[i]);
		rc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[41], zdcraw_tdc1[i]);
	      }
	}

      if( did_zdc_south_fire )
	{
	  for(int i=0; i<4; i++)
	    if( zdcraw_tdc1[i] < 3800 )
	      {
		zdc_adc_vs_tdc[i]->Fill(zdcraw_adc[i], zdcraw_tdc1[i]);
		fc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[42], zdcraw_tdc1[i]);
		rc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[43], zdcraw_tdc1[i]);
	      }

	  for(int i=8; i<24; i++)
	    if( zdcraw_tdc1[i] < 3800 )
	      {
		zdc_adc_vs_tdc[i]->Fill(zdcraw_adc[i], zdcraw_tdc1[i]);
		fc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[42], zdcraw_tdc1[i]);
		rc_adc_vs_zdc_tdc[i]->Fill(zdcraw_adc[43], zdcraw_tdc1[i]);
	      }
	}


      //compute booleans for pedestals
      //bool ped_zdc_south = true; //Run10 AuAu 62GeV
      //bool ped_zdc_north = true; //Run10 AuAu 62GeV
      bool ped_zdc_south = (zdc_adc[0] > 70.); //60 in 200GeV Cu or Au runs
      bool ped_zdc_north = (zdc_adc[4] > 70.); //70 in 200GeV Cu or Au runs

      //compute, if smd is overloaded
      bool smd_ovld_north = false;
      bool smd_ovld_south = false;
      //smd_ovld_south = (zdc_adc[0] > 1000);
      //smd_ovld_north = (zdc_adc[4] > 1000);

      //factors converting ADC to GeV
      //const float ADC_to_GeV_south = 2.2*100/31.2; //for 62.4 GeV Cu
      //const float ADC_to_GeV_north = 2.2*100/31.2; //for 62.4 GeV Cu
      //const float ADC_to_GeV_south = 2.2; //for 200 GeV Cu (0.5pC)
      //const float ADC_to_GeV_north = 2.2; //for 200 GeV Cu (0.5pC)
      //      const float ADC_to_GeV_south = 3.91;    // for 200 GeV p
      //      const float ADC_to_GeV_north = 3.91;    // for 200 GeV p
      //const float ADC_to_GeV_south = 3.91*100/31.2;    // for 62.4 GeV p
      //const float ADC_to_GeV_north = 3.91*100/31.2;    // for 62.4 GeV p
      //const float ADC_to_GeV_north = 2;    // for 200 GeV d (1.5 pC, Run 8)
      //const float ADC_to_GeV_south = 1;    // for 200 GeV Au (5.0 pC, Run 8)
      // const float ADC_to_GeV_south = 100./31.2; // for 62.4 GeV Au
      // const float ADC_to_GeV_north = 100./31.2; // for 62.4 GeV Au
      
      //const float ADC_to_GeV_south = 1./0.6800;    // for 500 GeV p run 13
      //const float ADC_to_GeV_north = 1./0.4654;    // for 500 GeV p run 13
      //const float ADC_to_GeV_s[3]={1.0/0.32,1.0/0.36, 1.0/0.38}; //for 500 GeV pp run 13
      //const float ADC_to_GeV_n[3]={1.0/0.3205,1.0/0.1923 , 1.0/0.2129}; //for 500 GeV pp run 13
      const float ADC_to_GeV_south = 1.;    // for 200 GeV AuAu run 11, run12 UU, run14 200 GeV AuAu, run15pp200, run15 pAu200, run16 AuAu200
      const float ADC_to_GeV_north = 1.;    // for 200 GeV AuAu run 11, run12 UU, run14 200 GeV AuAu, run15pp200, run15 pAu200, run16 AuAu200
      const float ADC_to_GeV_s[3]={1.0,1.0,1.0}; //for 200 GeV AuAu run11, run12 UU, run14 200 GeV AuAu
      const float ADC_to_GeV_n[3]={1.0,1.0,1.0}; //for 200 GeV AuAu run11, run12 UU, run14 200 GeV AuAu

      //const float ADC_to_GeV_south = 1.;    // for Run10 200 GeV Au
      //const float ADC_to_GeV_north = 1.;    // for Run10 200 GeV Au
      //const float ADC_to_GeV_south = 100./31.2; // for Run10 62.4 GeV Au
      //const float ADC_to_GeV_north = 100./31.2; // for Run10 62.4 GeV Au


      //if (ped_zdc_south && !did_laser_fire && did_zdcns_fire && zdc_adc_south)
      if (ped_zdc_south && !did_laser_fire && did_BbcLvl1_fire && zdc_adc_south) //for delay test. From Run15
	{
	  zdc_adc_south->Fill( zdc_adc[0] / ADC_to_GeV_south);
	  /*Ciprian Gal 110211*/
	  for(int i=0;i<3;i++)
	    zdc_adc_s_ind[i]->Fill(zdc_adc[i+1]/ADC_to_GeV_s[i]);
	}
      
     
      //if (ped_zdc_north && !did_laser_fire && did_zdcns_fire && zdc_adc_north)
      if (ped_zdc_north && !did_laser_fire && did_BbcLvl1_fire && zdc_adc_north) //for delay test. From Run15
	{
	  zdc_adc_north->Fill( zdc_adc[4] / ADC_to_GeV_north);
	  /*Ciprian Gal 110211*/
	  for(int i=0;i<3;i++)
	    zdc_adc_n_ind[i]->Fill(zdc_adc[i+5]/ADC_to_GeV_n[i]);
	}
      CompOvfBool();
      
      if ((did_laser_fire == false))
        {
          if (ovfbool[0] && ovfbool[4])
            {
              smd_sum_hor_south->Fill(smd_sum[0] / 8.);
            }
          if (ovfbool[0] && ovfbool[4])
            {
              smd_sum_ver_south->Fill(smd_sum[1] / 7.);
            }
          if (ovfbool[0] && ovfbool[4])
            {
              smd_sum_hor_north->Fill(smd_sum[2] / 8.);
            }
          if (ovfbool[0] && ovfbool[4])
            {
              smd_sum_ver_north->Fill(smd_sum[3] / 7.);
            }
        }

      bool fill_hor_south = false;
      bool fill_ver_south = false;

      bool fill_hor_north = false;
      bool fill_ver_north = false;


      int s_ver_north = 0;
      int s_hor_north = 0;

      for ( int i = 0; i < 8; i++)
        if ( smd_adc[i] > 8 )
          s_hor_north++;

      for ( int i = 0; i < 7; i++)
        if ( smd_adc[i + 16] > 5 )
          s_ver_north++;

      bool fired_smd_hor_s = (s_hor_north > 1);
      bool fired_smd_ver_s = (s_ver_north > 1);

      if ( ped_zdc_south && !did_laser_fire && fired_smd_hor_s && 
	   fired_smd_ver_s && ovfbool[0] && ovfbool[4] && !smd_ovld_south)
        {
          fill_hor_south = true;
          fill_ver_south = true;
          smd_hor_south->Fill( smd_pos[0] );
          smd_ver_south->Fill( smd_pos[1] );
          for (int i = 0 ; i < 8; i++)
            {
              smd_value->Fill(smd_adc[i], float(i) );
              smd_value->Fill(smd_adc[i + 8], float(i) + 8. );
            }
          if ((zdc_adc[0] > 200.))
            {
              for (int i = 0; i < 8; i++)
                {
                  smd_value_good->Fill(smd_adc[i], float(i));
                  smd_value_good->Fill(smd_adc[i + 8], float(i) + 8);
                }
            }
          if ((zdc_adc[0] <= 200.))
            {
              for (int i = 0; i < 8; i++)
                {
                  smd_value_small->Fill(smd_adc[i], float(i));
                  smd_value_small->Fill(smd_adc[i + 8], float(i) + 8);
                }
            }
        }

      //      if ( fill_hor_south && fill_ver_south && zdc_adc[0]/ADC_to_GeV_south>40 )
      //	if( (smd_pos[1]>0.1 || smd_pos[1]<0) || (smd_pos[0]>0.05 || smd_pos[0]<-0.05) )
      //             smd_xy_south->Fill(smd_pos[1], smd_pos[0]);
      if ( fill_hor_south && fill_ver_south && zdc_adc[0]>40 )
	{
	  smd_xy_south->Fill(smd_pos[1], smd_pos[0]);
	  veto_adc_vs_pos[0]->Fill(zdcraw_adc[42], smd_pos[0]);
	  veto_adc_vs_pos[1]->Fill(zdcraw_adc[42], smd_pos[1]);
	  veto_adc_vs_pos[2]->Fill(zdcraw_adc[43], smd_pos[0]);
	  veto_adc_vs_pos[3]->Fill(zdcraw_adc[43], smd_pos[1]);
	}

      bool ped_smd_hnorth = true; //in 200GeV, it was: (smd_sum[2] > 800.);
      bool ped_smd_vnorth = true; //in 200GeV, it was: (smd_sum[3] > 700.);

      int n_ver_north = 0;
      int n_hor_north = 0;

      for ( int i = 0; i < 8; i++)
        if ( smd_adc[i + 16] > 8 )
          n_hor_north++;

      for ( int i = 0; i < 7; i++)
        if ( smd_adc[i + 24] > 5 )
          n_ver_north++;

      bool fired_smd_hor = (n_hor_north > 1);
      bool fired_smd_ver = (n_ver_north > 1);

      if (ped_zdc_north && ped_smd_hnorth && ovfbool[0] && ovfbool[4]
          && !smd_ovld_north && fired_smd_hor && !did_laser_fire)
        {
          fill_hor_north = true;
          smd_hor_north->Fill( smd_pos[2] );
          zdc_hor_north->Fill( zdc_adc[4] / ADC_to_GeV_north, smd_pos[2] );
          for (int i = 0; i < 8; i++)
            {
              smd_value->Fill(smd_adc[i + 16], float(i) + 16);
            }
          if ((zdc_adc[4] > 200.))
            {
              smd_hor_north_good->Fill( smd_pos[2] );
              for (int i = 0; i < 8; i++)
                {
                  smd_value_good->Fill(smd_adc[i + 16], float(i) + 16.);
                }
            }
          if ((zdc_adc[4] <= 200.))
            {
              smd_hor_north_small->Fill( smd_pos[2] );
              for (int i = 0; i < 8; i++)
                {
                  smd_value_small->Fill(smd_adc[i + 16], float(i) + 16.);
                }
            }
        }
      if (ped_zdc_north && ped_smd_vnorth && ovfbool[0] && ovfbool[4]
          && !smd_ovld_north && fired_smd_ver && !did_laser_fire)
        {
          fill_ver_north = true;
          smd_ver_north->Fill( smd_pos[3] );
          zdc_ver_north->Fill( zdc_adc[4] / ADC_to_GeV_north, smd_pos[3] );
          for (int i = 0; i < 8; i++)
            {
              smd_value->Fill(smd_adc[i + 24], float(i) + 24);
            }
          if ((zdc_adc[4] > 200.))
            {
              smd_ver_north_good->Fill( smd_pos[3] );
              for (int i = 0; i < 8; i++)
                {
                  smd_value_good->Fill(smd_adc[i + 24], float(i) + 24.);
                }
            }
          if ((zdc_adc[4] <= 200.))
            {
              smd_ver_north_small->Fill( smd_pos[3] );
              for (int i = 0; i < 8; i++)
                {
                  smd_value_small->Fill(smd_adc[i + 24], float(i) + 24.);
                }
            }
        }
//      if ( fill_hor_north && fill_ver_north && zdc_adc[4]/ADC_to_GeV_north>40 )
//        {
//	  if( (smd_pos[3]>0.1 || smd_pos[3]<0) || (smd_pos[2]>0.05 || smd_pos[2]<-0.05) )
//	    smd_xy_north->Fill( smd_pos[3], smd_pos[2]);
//        }
      if ( fill_hor_north && fill_ver_north && zdc_adc[4]>40 )
	{
	  smd_xy_north->Fill( smd_pos[3], smd_pos[2]);
	  veto_adc_vs_pos[4]->Fill(zdcraw_adc[40], smd_pos[2]);
	  veto_adc_vs_pos[5]->Fill(zdcraw_adc[40], smd_pos[3]);
	  veto_adc_vs_pos[6]->Fill(zdcraw_adc[41], smd_pos[2]);
	  veto_adc_vs_pos[7]->Fill(zdcraw_adc[41], smd_pos[3]);
	}
 

      //const float TDC_to_ns = 142.86; // 1 ns = 142.86 TDC ( from 1 TDC = 7 ps )
      //      static const float C = GSL_CONST_CGS_SPEED_OF_LIGHT/1.e9; // cm/ns from gsl

      if (ped_zdc_south && ped_zdc_north && !did_laser_fire
          && ovfbool[0] && ovfbool[4])
        {
          zdc_vertex->Fill( zdcevent->getZvertex() );
        }
      if (ped_zdc_south && ped_zdc_north && !did_laser_fire
          && ovfbool[0] && ovfbool[4] && did_BbcLvl1_fire)
        //&& ovfbool[0] && ovfbool[4] && did_zdcns_fire)
        {
          zdc_vertex_b->Fill( zdcevent->getZvertex() );
          zdc_vs_bbc->Fill( zdcevent->getZvertex(), bbcevent->getZVertex() );
        }

      for ( int i = 0; i < 40; i++)
        {
          zdc_value->Fill(p->iValue(i), float(i));
        }
      if (fill_hor_south)
        {
          smd_yt_south->Fill(nevt, smd_pos[0]);
        }
      if (fill_ver_south)
        {
          smd_xt_south->Fill(nevt, smd_pos[1]);
        }
      if (fill_hor_north)
        {
          smd_yt_north->Fill(nevt, smd_pos[2]);
        }
      if (fill_ver_north)
        {
          smd_xt_north->Fill(nevt, smd_pos[3]);
        }

      if (did_laser_fire)
        {
          zdc_laser_south1->Fill(nevt, p->iValue(1));
          zdc_laser_south2->Fill(nevt, p->iValue(2));
          zdc_laser_south3->Fill(nevt, p->iValue(3));
          zdc_laser_north1->Fill(nevt, p->iValue(5));
          zdc_laser_north2->Fill(nevt, p->iValue(6));
          zdc_laser_north3->Fill(nevt, p->iValue(7));

          zdc_laser_south1b->Fill(nevt, p->iValue(1, "T1"));
          zdc_laser_south2b->Fill(nevt, p->iValue(2, "T1"));
          zdc_laser_south3b->Fill(nevt, p->iValue(3, "T1"));
          zdc_laser_north1b->Fill(nevt, p->iValue(5, "T1"));
          zdc_laser_north2b->Fill(nevt, p->iValue(6, "T1"));
          zdc_laser_north3b->Fill(nevt, p->iValue(7, "T1"));
        }

      if (did_laser_fire == false && did_zdcns_fire)
        {
          if (ped_zdc_south)
            {
              zdc_ratio_south->Fill(zdc_adc[5], zdc_adc[6] + zdc_adc[7]);
            }
          if (ped_zdc_north)
            {
              zdc_ratio_north->Fill(zdc_adc[1], zdc_adc[2] + zdc_adc[3]);
            }
        }


      if (did_laser_fire)
        {
          n_laser++;
          for (int i = 0;i < 40;i++)
            {
              zdcave[i] += p->iValue(i) / Nlas;
            }

          if (n_laser % Nlas == 0)
            {
              ostringstream ppgvar;
              for (int i = 0;i < 40;i++)
                {
                  ppgvar.str("");
                  ppgvar << "zdcmonppg" << i;
                  dbvars->SetVar(ppgvar.str().c_str(), zdcave[i], 0., (float) nevt);
                }
              commitflag |= 0x1;
              printf("commitflag ppg: 0x%x\n", commitflag);
              memset(zdcave, 0, sizeof(zdcave));
            }
        }

      if ( fill_hor_north && fill_ver_north && fill_hor_south && fill_ver_south )
        {
          n_smdpos++;
          for (int i = 0;i < 4;i++)
            {
              smdposave[i] += smd_pos[i] / Nsmdpos;
            }
          if (n_smdpos % Nsmdpos == 0)
            {
              ostringstream smdvar;
              for (int i = 0;i < 4;i++)
                {
                  smdvar.str("");
                  smdvar << "smd_pos_" << i;
                  dbvars->SetVar(smdvar.str().c_str(), smdposave[i], 0., (float)nevt);
                }
              commitflag |= 0x2;
              printf("commitflag smd: 0x%x\n", commitflag);
              memset(smdposave, 0, sizeof(smdposave));
            }
        }
      if (ped_zdc_south && ped_zdc_north && !did_laser_fire
          && ovfbool[0] && ovfbool[4 ] && did_BbcLvl1_fire)
        {
          n_zvert++;
          zvertave += zdcevent->getZvertex() / Nzvert;
          if (n_zvert % Nzvert == 0)
            {
              dbvars->SetVar("zdc_vertex", zvertave, 0., (float)nevt);
              commitflag |= 0x4;
              printf("commitflag vtx: 0x%x\n", commitflag);
              zvertave = 0;
            }
        }



      ped_zdc_south = ped_zdc_south && (zdc_adc[0] / ADC_to_GeV_south > 1500) &&
	(zdc_adc[0] / ADC_to_GeV_south < 2500);
      ped_zdc_north = ped_zdc_north && (zdc_adc[4] / ADC_to_GeV_north > 1500) &&
	(zdc_adc[4] / ADC_to_GeV_north < 2500);
      if (ped_zdc_south && !did_laser_fire && did_zdcns_fire && ped_zdc_north)
        {
          n_zadc++;
          zadcsave += (zdc_adc[0] / ADC_to_GeV_south / Nzadc);
          zadcnave += (zdc_adc[4] / ADC_to_GeV_north / Nzadc);
          if (n_zadc % Nzadc == 0)
            {
              dbvars->SetVar("zdc_energy_south", zadcsave, 0., (float)nevt);
              dbvars->SetVar("zdc_energy_north", zadcnave, 0., (float)nevt);
              printf("committing energy stuff\n") ;
              commitflag |= 0x8;
              printf("commitflag energy: 0x%x\n", commitflag);
              zadcsave = 0;
              zadcnave = 0;
            }
        }
      delete p;
    }
  if ((commitflag & 0x7) == 0x7 )
    {
      CommitAll();
    }
  return 0;
}

int ZDCMon::Reset()
{
  memset(zdcave, 0, sizeof(zdcave)); // for floats only 0 with memset
  memset(smdposave, 0, sizeof(smdposave)); // for floats only 0 with memset
  zvertave = 0;
  zadcsave = 0;
  zadcnave = 0;
  nevt = 0;
  n_laser = 0;
  n_smdpos = 0;
  n_zvert = 0;
  n_zadc = 0;
  lastcommit = 0;
  commitflag = 0x0;
  return 0;
}

int
ZDCMon::EndRun(const int runno)
{
  // lets do a last commit at the end of the run independant
  // of the previous commit
  dbvars->DBcommit();
  return 0;
}

int ZDCMon::DBVarInit()
{
  // variable names are not case sensitive
  ostringstream var;
  for (int i = 0;i < 40;i++)
    {
      var.str("");
      var << "zdcmonppg" << i;
      dbvars->registerVar(var.str());
    }
  for (int i = 0;i < 4;i++)
    {
      var.str("");
      var << "smd_pos_" << i;
      dbvars->registerVar(var.str());
    }
  var.str("");
  var << "zdc_vertex";
  dbvars->registerVar(var.str());
  var.str("");
  var << "zdc_energy_south";
  dbvars->registerVar(var.str());
  var.str("");
  var << "zdc_energy_north";
  dbvars->registerVar(var.str());

  if (verbosity > 0)
    {
      dbvars->Print();
    }
  dbvars->DBInit();
  return 0;
}

int ZDCMon::CommitAll()
{
  OnlMonServer *se = OnlMonServer::instance();
  printf("Trying to commit, seconds till last comit: %ld\n", se->CurrentTicks() - lastcommit) ;
  commitflag = 0x0;
  if (se->CurrentTicks() > lastcommit + 360) // add 6 minutes
    {
      if (!dbvars->DBcommit())
        {
          printf("Committed\n") ;
          lastcommit = se->CurrentTicks();
          return 0;
        }
    }
  return 1;
}
