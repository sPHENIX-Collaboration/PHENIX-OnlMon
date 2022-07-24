#include <MpcMon.h>
#include <OnlMonServer.h>
#include <OnlMonTrigger.h>
#include <Event.h>
#include <msg_profile.h>

#include <MpcMap.h>
#include <MpcCalib.h>

#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include <BbcEvent.hh>
#include <BbcCalib.hh>

#include <cmath>
#include <iostream>
#include <sstream>

#include <packet_hbd_fpgashort.h>

using namespace std;
//using namespace mpcmon;

static const int mpc_packetid[] =
{
  21101, 21102, 21103, 21104, 21105, 21106
    //21101, 21102, 21103, 21031, 21032, 21041, 21042
    //21011, 21012, 21021, 21022, 21031, 21032, 21041, 21042, 21001, 21002, 21003, 21004
}; //MPC packet Ids... Order is important

static const int NFEM = 6;
static const int MPC_NCHANNELS = 576;	/*200 + 220*/

MpcMon::MpcMon(const char *name): OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new MpcMon()
  nevents=0;
  return ;
}

MpcMon::~MpcMon()
{
  if (bbccalib)
  {
    delete bbccalib;
  }
  if (bevt)
  {
    delete bevt;
  }

  // clean up the memory
  if ( mpcmap )
    {
      delete mpcmap;
    }
  if ( mpccalib )
    {
      delete mpccalib;
    }
}

int MpcMon::Init()
{
  mpcmap = MpcMap::instance();
  //mpccalib = MpcCalib::instance();
  mpccalib = new MpcCalib();

  for(int ifeech = 0; ifeech < 576; ifeech++)
    {
      gridx[ifeech] = mpcmap->getGridX(ifeech);
      gridy[ifeech] = mpcmap->getGridY(ifeech);
      if(ifeech < 288)
        online_ch[ifeech] = mpcmap->getFeeCh(gridx[ifeech], gridy[ifeech], 0);
      else
        online_ch[ifeech] = mpcmap->getFeeCh(gridx[ifeech], gridy[ifeech], 1);
      tdc2ns[ifeech] = mpccalib->get_tdc_leastcount(ifeech);
      t0[ifeech] = mpccalib->get_t0(ifeech);
      slewcor0[ifeech] = mpccalib->get_slewcor0(ifeech);
      slewcor1[ifeech] = mpccalib->get_slewcor1(ifeech);
      //cout << ifeech << " " << slewcor0[ifeech] << " " << slewcor1[ifeech] << " " << t0[ifeech] << " " << tdc2ns[ifeech] << endl;
    }

  // Energy Sum Spectra for Different Trigger
  //float max_escale = 10000.;	// 19.6 GeV Au+Au
  //float max_escale = 180000.;		// Au+Au 200
  //float max_escale = 60000.;		// Au+Au 200
  //float max_escale = 10000.;		// Au+Au 27, run11
  //float max_escale = 10000.;		// p+p 500
  //float max_escale = 5000.;		// p+p 500, run12
  //float max_escale = 40000.;		// UU 193, run12
  //float max_escale = 5000.;		// Cu+Au 200, run12
  //float max_escale = 4000.;		// p+p 510, run13
  //float max_escale = 15000.;		// Au+Au 200, run14
  //float max_escale = 3000.;		// p+p 200, run15
  float max_escale = 12000.;		// Au+Au 200, run16

  int nbins=100;

  mpc_scaledowns = new TH1F("mpc_scaledowns","mpc_scaledowns",10,-0.5,9.5);

  mpc_nevents = new TH1F("mpc_nevents","mpc_nevents",1,0,1);
  mpc_bbctrig_ehist = new TH1F("mpc_bbctrig_ehist", "BBC Triggered Spectrum", nbins, 0., max_escale);

  TString tname;
  for (int itrig=0; itrig<NMPCTRIGS; itrig++)
    {
      tname = "mpc_ehist"; tname += itrig;
      mpc_ehist[itrig] = new TH1F(tname, "MPC Triggered Spectrum", nbins, 0., max_escale);
      mpc_ehist[itrig]->SetXTitle("ADC Post - Pre");
      mpc_ehist[itrig]->SetLineColor(itrig+2);

      tname = "mpc_ehist"; tname += itrig; tname += "_n";
      mpc_ehist_n[itrig] = new TH1F(tname, "MPC Trig'ed Spectrum North", nbins, 0., max_escale);
      mpc_ehist_n[itrig]->SetXTitle("ADC Post - Pre");
      mpc_ehist_n[itrig]->SetLineColor(itrig+2);

      tname = "mpc_ehist"; tname += itrig; tname += "_s";
      mpc_ehist_s[itrig] = new TH1F(tname, "MPC Trig'ed Spectrum South", nbins, 0., max_escale);
      mpc_ehist_s[itrig]->SetXTitle("ADC Post - Pre");
      mpc_ehist_s[itrig]->SetLineColor(itrig+2);
    }

  mpc_bbctrig_ehist_n = new TH1F("mpc_bbctrig_ehist_n", "BBC Triggered Spectrum North", nbins, 0., max_escale);
  mpc_bbctrig_ehist_n->SetXTitle("ADC Post - Pre");

  mpc_bbctrig_ehist_s = new TH1F("mpc_bbctrig_ehist_s", "BBC Triggered Spectrum South", nbins, 0., max_escale);
  mpc_bbctrig_ehist_s->SetXTitle("ADC Post - Pre");

  // Trigger Checks
  mpc_rbits = new TH1F("mpc_rbits1","MPC Reduced Bit Check",14,-0.5,13.5);
  mpc_rbits->SetXTitle("reduced bit frequency");

  mpc_rbits_consistency = new TH1F("mpc_rbits2","MPC Reduced Bit Consistency Check",24,-0.5,23.5);
  mpc_rbits_consistency->SetXTitle("trigger");

  for (int itrig=0; itrig<NMPCTRIGS; itrig++)
    {
      tname = "mpc0_trig"; tname += itrig; tname += "_crossing";
      mpc_trig_crossing[itrig][0] = new TH1F(tname,"MPC.S Crossing",120,-0.5,119.5);
      mpc_trig_crossing[itrig][0]->SetXTitle("crossing");
      mpc_trig_crossing[itrig][0]->SetLineColor(1);	//[trig][arm]
      tname = "mpc1_trig"; tname += itrig; tname += "_crossing";
      mpc_trig_crossing[itrig][1] = new TH1F(tname,"MPC.N Crossing",120,-0.5,119.5);
      mpc_trig_crossing[itrig][1]->SetXTitle("crossing");
      mpc_trig_crossing[itrig][1]->SetLineColor(4);	//North
      mpc_trig_crossing[itrig][1]->SetFillColor(4);
    }

  // Time Distribution
  mpc_ch_timedist = new TH2F("mpc_ch_timedist","MPC Time",2000,0,200,MPC_NCHANNELS,0,MPC_NCHANNELS);
  mpc_ch_timedist->SetXTitle("Raw TDC");
  mpc_ch_timedist->SetYTitle("MPC Online Channel No.");

  smpc_timedist = new TH1F("mpcs_timedist","MPC Waveform Peak (North=Blue : South=Gray)",12,0,12);
  smpc_timedist->SetXTitle("Sample Number");
  smpc_timedist->SetLineColor(1);

  nmpc_timedist = new TH1F("mpcn_timedist","MPC Waveform Peak (North=Blue : South=Gray)",12,0,12);
  nmpc_timedist->SetXTitle("Sample Number");
  nmpc_timedist->SetLineColor(4);

  // Slew Curve
  mpc_slewdist = new TH2F("mpc_slewdist","MPC TDC vs ADC",500,0,4000,500,0,80);
  mpc_slewdist->SetXTitle("Raw ADC");
  mpc_slewdist->SetYTitle("Raw TDC");

  // spectra for each channel, bbc triggers
  mpcmon_hist2d = new TH2F("mpc_bbctrig_2d", "Channel vs Energy, BBC Trig", 1300, -100., 2500., MPC_NCHANNELS, 0, MPC_NCHANNELS);
  mpcmon_hist2d->SetXTitle("ADC Post - Pre");
  mpcmon_hist2d->SetYTitle("Channel");

  // spectra for each channel, bbc triggers
  mpcmon_mpctrig_2d = new TH2F("mpc_mpctrig_2d", "Channel vs Energy, MPC Trig", 1300, -100., 2500., MPC_NCHANNELS, 0, MPC_NCHANNELS);
  mpcmon_mpctrig_2d->SetXTitle("ADC Post - Pre");
  mpcmon_mpctrig_2d->SetYTitle("Channel");

  // integrated energy in each crystal, as a grid
  TString hname;
  TString htitle;
  for (int itrig=0; itrig<NMPCTRIGS; itrig++)
    {
      hname = "mpcs_crystal"; hname += itrig;
      htitle = "Energy Per Crystal, SMPC Trigger "; htitle += itrig;
      smpc_crystal[itrig] = new TH2F(hname,htitle,18,-0.5,17.5,18,-0.5,17.5);
      smpc_crystal[itrig]->GetXaxis()->SetNdivisions(118);
      smpc_crystal[itrig]->GetYaxis()->SetNdivisions(118);
    
      hname = "mpcn_crystal"; hname += itrig;
      htitle = "Energy Per Crystal, NMPC Trigger "; htitle += itrig;
      nmpc_crystal[itrig] = new TH2F(hname,htitle,18,-0.5,17.5,18,-0.5,17.5);
      nmpc_crystal[itrig]->GetXaxis()->SetNdivisions(118);
      nmpc_crystal[itrig]->GetYaxis()->SetNdivisions(118);
    }

  smpc_bbctrig_crystal_energy = new TH2F("mpcs_bbctrig_crystal_energy","Energy Per South Crystal, BBC Triggers",18,-0.5,17.5,18,-0.5,17.5);
  smpc_bbctrig_crystal_energy->GetXaxis()->SetNdivisions(118);
  smpc_bbctrig_crystal_energy->GetYaxis()->SetNdivisions(118);

  nmpc_bbctrig_crystal_energy = new TH2F("mpcn_bbctrig_crystal_energy","Energy Per North Crystal, BBC Triggers",18,-0.5,17.5,18,-0.5,17.5);
  nmpc_bbctrig_crystal_energy->GetXaxis()->SetNdivisions(118);
  nmpc_bbctrig_crystal_energy->GetYaxis()->SetNdivisions(118);

  smpc_bbctrig_crystal_count = new TH2F("mpcs_bbctrig_crystal_count","Count Per South Crystal, BBC Triggers",18,-0.5,17.5,18,-0.5,17.5);
  smpc_bbctrig_crystal_energy->GetXaxis()->SetNdivisions(118);
  smpc_bbctrig_crystal_energy->GetYaxis()->SetNdivisions(118);

  nmpc_bbctrig_crystal_count = new TH2F("mpcn_bbctrig_crystal_count","Count Per North Crystal, BBC Triggers",18,-0.5,17.5,18,-0.5,17.5);
  nmpc_bbctrig_crystal_energy->GetXaxis()->SetNdivisions(118);
  nmpc_bbctrig_crystal_energy->GetYaxis()->SetNdivisions(118);

  smpc_led_crystal_energy = new TH2F("mpcs_led_crystal_energy","Energy Per Crystal, South LED",18,-0.5,17.5,18,-0.5,17.5);
  smpc_led_crystal_energy->GetXaxis()->SetNdivisions(118);
  smpc_led_crystal_energy->GetYaxis()->SetNdivisions(118);

  nmpc_led_crystal_energy = new TH2F("mpcn_led_crystal_energy","Energy Per Crystal, North LED",18,-0.5,17.5,18,-0.5,17.5);
  nmpc_led_crystal_energy->GetXaxis()->SetNdivisions(118);
  nmpc_led_crystal_energy->GetYaxis()->SetNdivisions(118);

  // led spectra for each channel
  mpcledhist2d = new TH2F("mpc_led_hist2d", "Channel vs LED ADC", 1100, -100., 4000., MPC_NCHANNELS, 0., MPC_NCHANNELS);
  mpcledhist2d->SetXTitle("ADC Post - Pre");

  smpc_bbctrig_occupancy = new TH1F("mpcs_bbctrig_occupancy", "Occupancy (South=Red, North=Blue)", 219, 1, 220);
  smpc_bbctrig_occupancy->SetLineColor(2);
  smpc_bbctrig_occupancy->SetXTitle("No. Hit Crystals");
  nmpc_bbctrig_occupancy = new TH1F("mpcn_bbctrig_occupancy", "Occupancy (South=Red, North=Blue)", 219, 1, 220);
  nmpc_bbctrig_occupancy->SetLineColor(4);
  nmpc_bbctrig_occupancy->SetXTitle("No. Hit Crystals");

  // Trigger Numbers
  mpc_triginfo = new TH1F("mpc_triginfo","Trigger Counts",20,0.5,20.5);
  mpc_triginfo->SetXTitle("trig_number");

  mpcvsbbcst0 = new TH2F("mpcvsbbcst0", "South Start Time", 100, 0, 20, 100, -20, 40);
  mpcvsbbcst0->GetXaxis()->SetTitle("Bbc South Start Time");
  mpcvsbbcst0->GetYaxis()->SetTitle("Mpc South Start Time");

  mpcvsbbcnt0 = new TH2F("mpcvsbbcnt0", "North Start Time", 100, 0, 20, 100, -20, 40);
  mpcvsbbcnt0->GetXaxis()->SetTitle("Bbc North Start Time");
  mpcvsbbcnt0->GetYaxis()->SetTitle("Mpc North Start Time");

  mpcvsbbcvtx = new TH2F("mpcvsbbcvtx", "Vertex", 50, -50, 50, 50, -150, 150);
  mpcvsbbcvtx->GetXaxis()->SetTitle("Bbc ZVertex");
  mpcvsbbcvtx->GetYaxis()->SetTitle("Mpc ZVertex");

  mpcvsbbct0 = new TH2F("mpcvsbbct0", "Event T0", 100, -10, 20, 100, -10, 40);
  mpcvsbbct0->GetXaxis()->SetTitle("Bbc T0");
  mpcvsbbct0->GetYaxis()->SetTitle("Mpc T0");


  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  // register histograms with server otherwise client won't get them
  Onlmonserver->registerHisto(this,mpc_bbctrig_ehist);
  Onlmonserver->registerHisto(this,mpc_bbctrig_ehist_n);
  Onlmonserver->registerHisto(this,mpc_bbctrig_ehist_s);

  Onlmonserver->registerHisto(this,mpc_nevents);
  Onlmonserver->registerHisto(this,mpc_scaledowns);
  Onlmonserver->registerHisto(this,mpc_rbits);
  Onlmonserver->registerHisto(this,mpc_rbits_consistency);

  Onlmonserver->registerHisto(this,smpc_bbctrig_crystal_energy);
  Onlmonserver->registerHisto(this,smpc_bbctrig_crystal_count);
  Onlmonserver->registerHisto(this,nmpc_bbctrig_crystal_energy);
  Onlmonserver->registerHisto(this,nmpc_bbctrig_crystal_count);

  for (int itrig=0; itrig<NMPCTRIGS; itrig++)
    {
      Onlmonserver->registerHisto(this,mpc_ehist[itrig]);
      Onlmonserver->registerHisto(this,mpc_ehist_n[itrig]);
      Onlmonserver->registerHisto(this,mpc_ehist_s[itrig]);
      Onlmonserver->registerHisto(this,smpc_crystal[itrig]);
      Onlmonserver->registerHisto(this,nmpc_crystal[itrig]);
      Onlmonserver->registerHisto(this,mpc_trig_crossing[itrig][0]);
      Onlmonserver->registerHisto(this,mpc_trig_crossing[itrig][1]);
    }

  Onlmonserver->registerHisto(this,smpc_led_crystal_energy);
  Onlmonserver->registerHisto(this,nmpc_led_crystal_energy);
  Onlmonserver->registerHisto(this,smpc_timedist);
  Onlmonserver->registerHisto(this,nmpc_timedist);
  Onlmonserver->registerHisto(this,mpc_slewdist);
  Onlmonserver->registerHisto(this,mpc_ch_timedist);
  Onlmonserver->registerHisto(this,mpcmon_hist2d);
  Onlmonserver->registerHisto(this,mpcmon_mpctrig_2d);
  Onlmonserver->registerHisto(this,mpcledhist2d);
  Onlmonserver->registerHisto(this,mpc_triginfo);
  Onlmonserver->registerHisto(this,smpc_bbctrig_occupancy);
  Onlmonserver->registerHisto(this,nmpc_bbctrig_occupancy);
  Onlmonserver->registerHisto(this,mpcvsbbcst0);
  Onlmonserver->registerHisto(this,mpcvsbbcnt0);
  Onlmonserver->registerHisto(this,mpcvsbbcvtx);
  Onlmonserver->registerHisto(this,mpcvsbbct0);

  // Initialize BBC Module
  bbccalib = new BbcCalib();
  bevt = new BbcEvent();

  // Initialize Variables
  n_trig_led = 0;
  n_trig_bbc[0] = 0;
  n_trig_bbc[1] = 0;
  n_trig_bbc[2] = 0;
  n_trig_bbc[3] = 0;
  for (int itrig=0; itrig<NMPCTRIGS; itrig++)
    {
      n_trig_mpc[itrig] = 0;
    }
  n_trig_mpcany = 0;

  return 0;
}//end of Init()

int MpcMon::BeginRun(const int runno)
{
  setup_calibration(runno);
  return 0;
}

int MpcMon::setup_calibration(const int runno)
{
  if ( runno<=0 ) return 0;
  OnlMonServer *se = OnlMonServer::instance();

  BBCTRIG0_name = "BBCLL1(>0 tubes) narrowvtx";
  BBCTRIG1_name = "BBCLL1(>0 tubes)";
  BBCTRIG2_name = "BBCLL1(>0 tubes) novertex";
  /*
  BBCTRIG0_name = "BBCLL1(>1 tubes)";
  BBCTRIG1_name = "BBCLL1(>1 tubes) novertex";
  BBCTRIG2_name = "EMPTY";
  */

  MPCTRIG0_name = "MPC_N_A";
  MPCTRIG1_name = "MPC_N_B";
  MPCTRIG2_name = "MPC_S_A";
  MPCTRIG3_name = "MPC_S_B";

  /*
  // Run15 p+Au
  MPCTRIG0_name = "MPC_N_A";
  MPCTRIG1s_name = "MPC_S_A";
  MPCTRIG1n_name = "MPC_S_A";
  MPCTRIG2s_name = "MPC_N_B";
  MPCTRIG2n_name = "MPC_N_B";
  MPCTRIG3s_name = "MPC_S_C&ERTLL1_2x2";
  MPCTRIG3n_name = "MPC_N_C&ERTLL1_2x2";
  */

  LEDTRIG_name = "PPG(Laser)";
    
  for(int i=0; i<32; i++)
    {
      int hexvalue = 1;
      //int hexvalue_for_bit = 1;
      hexvalue = hexvalue << i;
      string trigname = se->OnlTrig()->get_lvl1_trig_name(i);
      
      if( trigname == BBCTRIG0_name )
        {
         //hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
         /*
            printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
            se->OnlTrig()->get_lvl1_trig_name(i).c_str(),
            i, 
            hexvalue, 
            se->OnlTrig()->get_lvl1_trig_scale_down(i), 
            se->OnlTrig()->get_lvl1_trig_bit(i),
            hexvalue_for_bit);
            */
         BBCTRIG0_scaledown = se->OnlTrig()->get_lvl1_trig_scale_down(i);
         mpc_scaledowns->SetBinContent(BBCTRIG0,BBCTRIG0_scaledown);
         //bbll1_trig[numbbll1] = hexvalue;
         //bbll1_trig[numbbll1] = hexvalue_for_bit;
         //bbll1_scale_factor_buf[numbbll1] = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
         //numbbll1++;
       }
      if( trigname == BBCTRIG1_name )
      {
        BBCTRIG1_scaledown = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(BBCTRIG1,BBCTRIG1_scaledown);
      }
      if( trigname == BBCTRIG2_name )
      {
        BBCTRIG2_scaledown = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(BBCTRIG2,BBCTRIG2_scaledown);
      }
      if( trigname == MPCTRIG0_name )
      {
        MPCTRIG_scaledown[0] = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(MPCTRIG0,MPCTRIG_scaledown[0]);
      }
      if( trigname == MPCTRIG1_name )
      {
        MPCTRIG_scaledown[1] = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(MPCTRIG1,MPCTRIG_scaledown[1]);
      }
      if( trigname == MPCTRIG2_name )
      {
        MPCTRIG_scaledown[2] = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(MPCTRIG2,MPCTRIG_scaledown[2]);
      }
      if( trigname == MPCTRIG3_name )
      {
        MPCTRIG_scaledown[3] = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(MPCTRIG3,MPCTRIG_scaledown[3]);
      }
      if( trigname == MPCTRIG4_name )
      {
        MPCTRIG_scaledown[4] = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(MPCTRIG4,MPCTRIG_scaledown[4]);
      }
      if( trigname == LEDTRIG_name )
      {
        LEDTRIG_scaledown = se->OnlTrig()->get_lvl1_trig_scale_down(i);
        mpc_scaledowns->SetBinContent(LEDTRIG,LEDTRIG_scaledown);
      }
    }

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
  	  msg << "Environment variable BBCCALIBDIR not set";
  	  //OnlMonServer *se = OnlMonServer::instance();
  	  se->send_message(this, MSG_SOURCE_MPC, MSG_SEV_ERROR, msg.str(), 1);
          bbccalib->restore("./BbcCalib");
        }
      else
        {
          ostringstream otmp;
          otmp << getenv("BBCCALIBDIR") << "/BbcCalib";
          // need a cast from (const char *) to (char *)
          bbccalib->restore(otmp.str().c_str());
        }
    }
  else
    {
      // need to set BBCCALIBRATION in order to specify the source of the constants
      ostringstream msg;
      msg << "BBCCALIBRATION not set, the calibration constants for the BBC are not loaded correctly.";
      //OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_MPC, MSG_SEV_ERROR, msg.str(), 1);
    }

  bevt->setCalibDataAll(bbccalib);

  return 1;
}

int MpcMon::process_event(Event *evt)
{
  nevents++;
  mpc_nevents->SetBinContent(1,nevents);
  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  //cout << BBCTRIG <<" " << LEDTRIG << endl;
  // set up trigger information
  bool trig_bbc_vtx[3] = {false};
  bool trig_mpc[NMPCTRIGS] = {false};
  bool trig_mpcany = false;
  bool trig_led = false;

  int noccupancy = 0;
  int soccupancy = 0;

  TH1F *st0 = new TH1F("st0","st0", 200, -100, 100);
  TH1F *nt0 = new TH1F("nt0","nt0", 200, -100, 100);    

  //cout << "for this event: " << endl;
  //cout << "BBCLL1(>0 tubes)" << "\t" << Onlmonserver->Trigger("BBCLL1(>0 tubes)") << endl;
  //cout << "BBCLL1(noVertexCut)" << "\t" << Onlmonserver->Trigger("BBCLL1(noVertexCut)") << endl;
  //cout << "MPC_4x4A" << "\t" << Onlmonserver->Trigger("MPC_4x4A") << endl; 

  //unsigned long scaledtrig = Onlmonserver->Trigger(2); // get scaled trigger from server
  //OnlMonTrigger *Onlmontrigger = OnlMonTrigger::instance();
  //unsigned int bbcgt0 = Onlmontrigger->get_lvl1_trig_scale_down(scaledtrig);

  // calculate BBC
  bevt->Clear();
  bevt->setRawData(evt);
  bevt->calculate();

  float bbc_stime =bevt->getArmHitTime(Bbc::South);
  float bbc_ntime = bevt->getArmHitTime(Bbc::North);
  float bbc_zvtx  = bevt->getZVertex();
  float bbc_t0 = bevt->getTimeZero();

  //float adcgain = 1.0; //Converts ADC to energy
  float esum = 0.;
  float esum_n = 0.;
  float esum_s = 0.;

  Packet *gl1_packet = evt->getPacket(14001);
  
  if( gl1_packet )
    {
      long rb[5];
      rb[0] = gl1_packet->iValue(0, "RBITS0");
      rb[1] = gl1_packet->iValue(0, "RBITS1");
      rb[2] = gl1_packet->iValue(0, "GDISABLE");
      rb[3] = gl1_packet->iValue(0, "FACCEPT");
      rb[4] = gl1_packet->iValue(0, "MODEBITS");

      int rbit=0;
      bool isrbit[16]; for(int ibit=0; ibit<14; ibit++) { isrbit[ibit]=false; }

      for (int iword=0; iword<5; iword++)
        {
          unsigned int rbits_word = rb[iword]; //triglvl1->get_lvl1_rbits(iword);
          unsigned int rbit_mask = 0;
          for (int ibit=0; ibit<32; ibit++)
            {
              rbit_mask = static_cast<unsigned int>(0x1<<ibit);
              if ( (rbits_word & rbit_mask)==rbit_mask )
                {
                  /*
                  //this is the run13 configuration
                  //same as run12
                  if(rbit==78) isrbit[0]=true; //MPC_A,NorS
                  if(rbit==77) isrbit[1]=true; //MPC_B,NorS
                  */

                  // Run15 configuration
                  if(rbit==72) isrbit[0]=true; //MPC_B,S
                  if(rbit==78) isrbit[1]=true; //MPC_B,N

                  if(rbit==67) isrbit[2]=true; // MPCS_C_0
                  if(rbit==68) isrbit[3]=true;
                  if(rbit==69) isrbit[4]=true;
                  if(rbit==70) isrbit[5]=true;
                  if(rbit==71) isrbit[6]=true;
                  //if(rbit==72) isrbit[7]=true; // MPCS_C_5

                  if(rbit==73) isrbit[8]=true; //MPCN_C_0
                  if(rbit==75) isrbit[9]=true;
                  if(rbit==90) isrbit[10]=true;
                  if(rbit==91) isrbit[11]=true;
                  if(rbit==94) isrbit[12]=true;
                  if(rbit==95) isrbit[13]=true; //MPCN_C_5

                  // pre run16 d+Au
                  /*
                  if(rbit==79) isrbit[14]=true; //MPC_A,NorS
                  if(rbit==77) isrbit[15]=true; //MPC_A,NorS
                  */
                  // run16 d+Au
                  if(rbit==79) isrbit[14]=true; //MPC_A,S
                  if(rbit==77) isrbit[15]=true; //MPC_A,N
                }
              ++rbit;
            }
        }

      for(int ibit=0; ibit<14; ibit++)
        {
          if( isrbit[ibit] ) mpc_rbits->Fill(ibit);
        }

      int ncrbit = 0;	// north C
      int scrbit = 0;	// south C
      int nccrbit = 0;	// north C&C
      int sccrbit = 0;	// south C&C
      for (int ibit=0; ibit<6; ibit++)
        {
          // Check if C fired
          if ( isrbit[2+ibit]==true ) scrbit = 1;
          if ( isrbit[8+ibit]==true ) ncrbit = 1;
        }

      // Check if C&C fired
      if ( isrbit[2]&&(isrbit[4]||isrbit[5]||isrbit[6]) ) sccrbit = 1;
      if ( isrbit[3]&&(isrbit[5]||isrbit[6]||isrbit[7]) ) sccrbit = 1;
      if ( isrbit[4]&&(isrbit[6]||isrbit[7]||isrbit[2]) ) sccrbit = 1;
      if ( isrbit[5]&&(isrbit[7]||isrbit[2]||isrbit[3]) ) sccrbit = 1;
      if ( isrbit[6]&&(isrbit[2]||isrbit[3]||isrbit[4]) ) sccrbit = 1;
      if ( isrbit[7]&&(isrbit[3]||isrbit[4]||isrbit[5]) ) sccrbit = 1;

      if ( isrbit[8]&&(isrbit[10]||isrbit[11]||isrbit[12]) ) nccrbit = 1;
      if ( isrbit[9]&&(isrbit[11]||isrbit[12]||isrbit[13]) ) nccrbit = 1;
      if ( isrbit[10]&&(isrbit[12]||isrbit[13]||isrbit[8]) ) nccrbit = 1;
      if ( isrbit[11]&&(isrbit[13]||isrbit[8]||isrbit[9]) ) nccrbit = 1;
      if ( isrbit[12]&&(isrbit[8]||isrbit[9]||isrbit[10]) ) nccrbit = 1;
      if ( isrbit[13]&&(isrbit[9]||isrbit[10]||isrbit[11]) ) nccrbit = 1;

      /*  this isn't critical for the moment, lets do this later
      bool N4x4a = isrbit[0];
      bool S4x4a = isrbit[1];
      bool T4x4a = trig_mpc[0];

      //these are when trigger logic is consistent
      if( T4x4a && N4x4a  && !S4x4a ) mpc_rbits_consistency->Fill(0);
      if( T4x4a && !N4x4a && S4x4a ) mpc_rbits_consistency->Fill(1);
      if( !T4x4a && !N4x4a && !S4x4a ) mpc_rbits_consistency->Fill(2);
      //should have very low rate, but not inconsistent
      if( T4x4a && N4x4a && S4x4a ) mpc_rbits_consistency->Fill(3);

      //trigger is outright inconsistent
      if( T4x4a && !N4x4a && !S4x4a ) mpc_rbits_consistency->Fill(4);
      if( !T4x4a && N4x4a && S4x4a )  mpc_rbits_consistency->Fill(5);
      if( !T4x4a && N4x4a && !S4x4a ) mpc_rbits_consistency->Fill(6);
      if( !T4x4a && !N4x4a && S4x4a ) mpc_rbits_consistency->Fill(7);

      //4x4c is AND'ed with ERTLL1_2x2
      bool N4x4c = isrbit[4];
      bool S4x4c = isrbit[5];
      bool T4x4c = trig_mpc[1];

      //these are when trigger logic is consistent
      if( T4x4c && N4x4c  && !S4x4c ) mpc_rbits_consistency->Fill(8+0);
      if( T4x4c && !N4x4c &&  S4x4c ) mpc_rbits_consistency->Fill(8+1);
      if( !T4x4c && !N4x4c && !S4x4c ) mpc_rbits_consistency->Fill(8+2);

      //has very low rate but not inconsistent
      if( T4x4c && N4x4c && S4x4c ) mpc_rbits_consistency->Fill(8+3);

      //trigger is outright inconsistent
      if( T4x4c && !N4x4c && !S4x4c ) mpc_rbits_consistency->Fill(8+4);

      //because 4x4c is anded with ert these bits could fire without getting a trigger
      if( !T4x4c && N4x4c && S4x4c )  mpc_rbits_consistency->Fill(8+5);
      if( !T4x4c && N4x4c && !S4x4c ) mpc_rbits_consistency->Fill(8+6);
      if( !T4x4c && !N4x4c && S4x4c ) mpc_rbits_consistency->Fill(8+7);

      bool N4x4b = isrbit[2];
      bool S4x4b = isrbit[3];
      bool T4x4b = trig_mpc[2];
      if(rbit==77) isrbit[3]=true;  //4x4B South

      //these are when trigger logic is consistent
      if( T4x4b && N4x4b  && !S4x4b ) mpc_rbits_consistency->Fill(16+0);
      if( T4x4b && !N4x4b && S4x4b ) mpc_rbits_consistency->Fill(16+1);
      if( !T4x4b && !N4x4b && !S4x4b ) mpc_rbits_consistency->Fill(16+2);

      //should have very low rate, but not inconsistent
      if( T4x4b && N4x4a && S4x4a ) mpc_rbits_consistency->Fill(16+3);

      //trigger is outright inconsistent
      if( T4x4b && !N4x4b && !S4x4b ) mpc_rbits_consistency->Fill(16+4);
      if( !T4x4b && N4x4b && S4x4b )  mpc_rbits_consistency->Fill(16+5);
      if( !T4x4b && N4x4b && !S4x4b ) mpc_rbits_consistency->Fill(16+6);
      if( !T4x4b && !N4x4b && S4x4b ) mpc_rbits_consistency->Fill(16+7);

      */

      //cout << isrbit[0] << "\t" << isrbit[1] << "\t" << isrbit[2] << "\t" << isrbit[3] << "\t" << isrbit[4] << endl;
      //printf("0x%08lx\t0x%08lx\t0x%08lx\t0x%08lx\t0x%08lx\n",rb[0],rb[1],rb[2],rb[3],rb[4]);
      //printf("GL1 Reduced Bits1 0x%08lx\n", rb1);
      //printf("GL1 Reduced Bits2 0x%08lx\n", rb2);
      //printf("GL1 Reduced Bits3 0x%08lx\n", rb3);
      //printf("GL1 Reduced Bits4 0x%08lx\n", rb4);

      //assume there are two words?
      //int rbits[2] = {rb0, rb1};
      //for(int ibit=0; ibti


      /////////////////////////////////////////////////////////


      if ( Onlmonserver->Trigger( BBCTRIG0_name ) )
        {
          trig_bbc_vtx[0] = true;
          mpc_triginfo->Fill(BBCTRIG0); //AddBinContent( BBCTRIG0, 1 ); //BBCTRIG0_scaledown+1 );
          ++n_trig_bbc[0];
        }
      
     
      if ( Onlmonserver->Trigger( BBCTRIG1_name ) )
        {
          trig_bbc_vtx[1] = true;
          //cout << mpc_triginfo->GetEntries() << "\t" << mpc_triginfo->GetBinContent(BBCTRIG1) << endl;
          mpc_triginfo->Fill(BBCTRIG1); //AddBinContent( BBCTRIG1, 1 ); //BBCTRIG1_scaledown+1 );
          ++n_trig_bbc[1];
        }

      if ( Onlmonserver->Trigger( BBCTRIG2_name ) )
      {
        trig_bbc_vtx[2] = true;
        mpc_triginfo->Fill(BBCTRIG2); //AddBinContent( BBCTRIG2, 1 ); //BBCTRIG2_scaledown+1 );
        ++n_trig_bbc[2];
      }


      if ( Onlmonserver->Trigger( LEDTRIG_name ) )
      {
        //cout << "ppg laser event" << endl;
        trig_led = true;
        mpc_triginfo->Fill(LEDTRIG); //AddBinContent( LEDTRIG, 1 ); //LEDTRIG_scaledown+1 );
        ++n_trig_led;
      }

      // please no cout's they go into the message system, for those
      // type of messages, please use printf which doesn't get intercepted
      //   if(n_trig_bbc%1000==0) cout << "processed " << n_trig_led << " led events and " << n_trig_bbc << " bbc events\n";

      //if ( Onlmonserver->Trigger( MPCTRIG0_name ) || isrbit[0] )
      if ( Onlmonserver->Trigger( MPCTRIG0_name ) )
      {
        trig_mpc[0] = true;
        mpc_triginfo->Fill(MPCTRIG0);
        ++n_trig_mpc[0];
        //cout << n_trig_mpc[0] << "\t 4x4a trig = " << Onlmonserver->Trigger("MPC4x4a") << endl;
      }
      if ( Onlmonserver->Trigger( MPCTRIG1_name ) )
      {
        trig_mpc[1] = true;
        mpc_triginfo->Fill(MPCTRIG1);
        ++n_trig_mpc[1];
      }
      //if ( Onlmonserver->Trigger( MPCTRIG2s_name ) || Onlmonserver->Trigger( MPCTRIG2n_name ) )
      if ( Onlmonserver->Trigger( MPCTRIG2_name ) )
        //|| isrbit[2] || isrbit[3] || isrbit[4] || isrbit[5] || isrbit[6] || isrbit[7]  )
      {
        //cout << "Found trig2 " << MPCTRIG2_name << endl;
        trig_mpc[2] = true;
        mpc_triginfo->Fill(MPCTRIG2);
        ++n_trig_mpc[2];
      }
      if ( Onlmonserver->Trigger( MPCTRIG3_name ) )
        //|| isrbit[8] || isrbit[9] || isrbit[10] || isrbit[11] || isrbit[12] || isrbit[13]  )
      {
        //cout << "Found trig3 " << MPCTRIG3_name << endl;
        trig_mpc[3] = true;
        mpc_triginfo->Fill(MPCTRIG3);
        ++n_trig_mpc[3];
      }
      if ( Onlmonserver->Trigger( MPCTRIG4_name ) )
      {
        trig_mpc[4] = true;
        mpc_triginfo->Fill(MPCTRIG4);
        ++n_trig_mpc[4];
      }


      if ( trig_mpc[0] || trig_mpc[1] || trig_mpc[2] || trig_mpc[3] || trig_mpc[4] )
      {
        trig_mpcany = true;
        //mpc_triginfo->AddBinContent( MPCTRIGANY );
        ++n_trig_mpcany;
      }

      /*
      // Keep track of a bad trigger condition
      static int ntrigboth = 0;
      static int ntrigbad = 0;
      if ( trig_mpc[1]==true && trig_mpc[0]!=true )
        {
          ntrigbad++;
          cout << "ERROR, B trig but not A " << ntrigbad << "\t" << ntrigboth
               << "\t" << gl1_packet->iValue(0, "CROSSCTR") << endl;
        }
      else if ( trig_mpc[1]==true && trig_mpc[0]==true )
        {
          ntrigboth++;
        }
        */

      //////////////////////////////////////////////////////////

      // Fill the trigger crossing
      long crossctr = gl1_packet->iValue(0, "CROSSCTR");
      //printf("GL1 cross counter = 0x%08lx\n", crossctr);

      if ( trig_mpc[0]==true && isrbit[15]==true ) //mpc_A north fired
        {
          mpc_trig_crossing[0][0]->Fill( crossctr );
        }
      if ( trig_mpc[1]==true && isrbit[1]==true ) //mpc_B north fired
        {
          mpc_trig_crossing[0][1]->Fill( crossctr );
        }
      if ( trig_mpc[2]==true && isrbit[14]==true ) //mpc_A south fired
        {
          mpc_trig_crossing[1][0]->Fill( crossctr );
        }
      if ( trig_mpc[3]==true && isrbit[0]==true ) //mpc_B south fired
        {
          mpc_trig_crossing[1][1]->Fill( crossctr );
        }
      /*
      if ( trig_mpc[2]==true && sccrbit==1 )      //mpc_C&C south fired
        {
          mpc_trig_crossing[2][0]->Fill( crossctr ); 
        }
      if ( trig_mpc[2]==true && nccrbit == 1 )    //mpc_C&C north fired
        {
          mpc_trig_crossing[2][1]->Fill( crossctr );
        }
      if ( trig_mpc[3]==true && scrbit == 0 )     //C&ERT fired in mpc.s
        {
          mpc_trig_crossing[3][0]->Fill( crossctr );
        }
      if ( trig_mpc[3]==true && ncrbit == 1 )     //C&ERT fired in mpc.n
        {
          mpc_trig_crossing[3][1]->Fill( crossctr );
        }
        */
      if ( trig_mpc[4]==true )
        {
          mpc_trig_crossing[4][0]->Fill( crossctr );
          mpc_trig_crossing[4][1]->Fill( crossctr );
        }


      delete gl1_packet;
    }

  // Loop over packets, and extract data
  for (int ifem = 0; ifem < NFEM; ifem++)
    {

      Packet *p = evt->getPacket( mpc_packetid[ifem] );
      if(!p) continue;

      Packet_hbd_fpgashort *hbd_p = dynamic_cast<Packet_hbd_fpgashort*>( p );
      hbd_p->setNumSamples( NSAMPLES );
      
      const int nmod_per_fem = p->iValue(0,"NRMODULES");
      
      for(int imod=0; imod<nmod_per_fem; imod++)
        {
          for(int j=0; j<NCHAN_PER_MOD; j++)
          {

            int fee576ch = ifem*nmod_per_fem*NCHAN_PER_MOD + imod*NCHAN_PER_MOD + j;
            int ch = imod*NCHAN_PER_MOD + j;

            short isamp_min=0, samp_min=9999, isamp_max=0, samp_max=0;

            for(int isamp=0; isamp<NSAMPLES; isamp++)
            {
              short val = ( p->iValue(ch,isamp) ) & 0xfff;
              if( val<samp_min )
              {
                isamp_min=isamp;		// sample number of min
                samp_min=val;
              }
              if( val>samp_max )
              {
                isamp_max=isamp;		// sample number of max
                samp_max=val;
              }
            }

            //impose some minimum on the adc???
            //short adc = samp_max-samp_min;
            //adcsum += adc;
            float energy = samp_max-samp_min;              

            // correct for gains in new towers
            if ( fee576ch==484 || fee576ch==494 || fee576ch==500 || fee576ch==509 )
              {
                energy /= 10.0;
                //energy = 0.0;
              }

              //we really can get a time very directly, albeit certainly by 
              //a better method than the trivial one here, whereas with the old electronics
              //we need a timing scan, I guess the only issue would be a global timing offset
              //this needs to be revisted later if we care about its value
              //the values here are going to be in ns (so ~0-200) and in the south it is adc (0-4095)
              float tdc = 17.762*isamp_max;

              //float energy = (lopost - lopre - loped) * adcgain;

              if ( energy < 10 ) continue;    // skip empty channels
          
              esum += energy;
              if( fee576ch<288 ) esum_s += energy;
              else               esum_n += energy;
          
              float corrtime = 0;          
              // fill histograms for bbc trigger
              //only using BBCLL1(>0 tubes), because we usually only use events in the vtx range anyways for mpc analyses...
              if ( trig_bbc_vtx[0] || trig_bbc_vtx[1] || trig_bbc_vtx[2] )
                {
                  mpcmon_hist2d->Fill(energy, fee576ch);
                  if ( fee576ch<288 ) mpc_ch_timedist->Fill(tdc-bbc_stime, fee576ch);
                  else                mpc_ch_timedist->Fill(tdc-bbc_ntime, fee576ch);
                  if ( energy>30 && mpcmap->isCrystal( fee576ch ) )
                    {
                      float relative_time = 0;
                      if(fee576ch < 288)
                        {
                          corrtime = tdc-125.;
                          //corrtime = tdc * tdc2ns[sw_ch] - t0[sw_ch];
                          relative_time = corrtime - bbc_stime;
                          if(corrtime && bbc_stime)
                            {
                              smpc_timedist->Fill( isamp_max );
                              st0->Fill(corrtime);
                            }
                        }
                      else
                        {
                          corrtime = tdc-125.;
                          //corrtime = tdc * tdc2ns[sw_ch] - t0[sw_ch];
                          relative_time = corrtime - bbc_ntime;
                          if(corrtime && bbc_ntime)
                            {
                              nmpc_timedist->Fill( isamp_max );
                              nt0->Fill(corrtime);
                            }
                        }
                      //mpc_slewdist->Fill(energy, relative_time);
                    }
                }
          
              // Fill MPC triggered histograms
              if ( trig_mpcany )
                {
                  mpcmon_mpctrig_2d->Fill(energy, fee576ch);
                }
          
              for (int itrig=0; itrig<NMPCTRIGS; itrig++)
                {
                  if ( trig_mpc[itrig] )
                    {
                      if(fee576ch < 288)
                        smpc_crystal[itrig]->Fill(gridx[fee576ch],gridy[fee576ch],energy);
                      else
                        nmpc_crystal[itrig]->Fill(gridx[fee576ch],gridy[fee576ch],energy);
                    }
                }
          
              // Fill BBC triggered histograms
              if ( trig_bbc_vtx[0] || trig_bbc_vtx[1] || trig_bbc_vtx[2] )
                {
                  if(fee576ch < 288)
                    {
                      smpc_bbctrig_crystal_energy->Fill(gridx[fee576ch],gridy[fee576ch],energy);
                      smpc_bbctrig_crystal_count->Fill(gridx[fee576ch],gridy[fee576ch]);
                      if (gridx[fee576ch]>=0) soccupancy++;
                    }
                  else
                    {
                      nmpc_bbctrig_crystal_energy->Fill(gridx[fee576ch],gridy[fee576ch],energy);
                      nmpc_bbctrig_crystal_count->Fill(gridx[fee576ch],gridy[fee576ch]);
                      if (gridx[fee576ch]>=0) noccupancy++;
                    }
              
                }
          
              // Fill LED triggered histogram
              if ( trig_led )
                {
                  mpcledhist2d->Fill(energy, fee576ch);
                  if (fee576ch < 288)
                    {
                      smpc_led_crystal_energy->Fill(gridx[fee576ch],gridy[fee576ch],energy);
                    }
                  else
                    {
                      nmpc_led_crystal_energy->Fill(gridx[fee576ch],gridy[fee576ch],energy);
                    }
                }

            } //end ifemch loop
        }  //end module loops 
      
      delete p;
    }

  // Fill esum spectra
  if ( trig_bbc_vtx[0] || trig_bbc_vtx[1] || trig_bbc_vtx[2] )
    {
      unsigned int scaledown = 0;
      if( trig_bbc_vtx[0] ) scaledown = BBCTRIG0_scaledown;
      if( trig_bbc_vtx[1] ) scaledown = BBCTRIG1_scaledown;
      if( trig_bbc_vtx[2] ) scaledown = BBCTRIG2_scaledown;

      mpc_bbctrig_ehist->Fill( esum ); //, scaledown+1 );
      mpc_bbctrig_ehist_s->Fill( esum_s ); //, scaledown+1 );
      mpc_bbctrig_ehist_n->Fill( esum_n ); //, scaledown+1 );

      /*
      TF1 *sg = new TF1("sg", "gaus");
      TF1 *ng = new TF1("ng", "gaus");
      st0->Fit(sg, "Q", "Q");
      nt0->Fit(ng, "Q", "Q");
  
      if(sg->GetParameter(1) != 0 && ng->GetParameter(1) != 0)
      {     
      mpcvsbbcst0->Fill(bbc_stime, sg->GetParameter(1));
      mpcvsbbcnt0->Fill(bbc_ntime, ng->GetParameter(1));
      mpc_zvtx = (sg->GetParameter(1) - ng->GetParameter(1)) * 15;
      mpcvsbbcvtx->Fill(bbc_zvtx, mpc_zvtx);
      }

*/

      if(st0->GetEntries() > 0 && nt0->GetEntries() > 0 && st0->GetMean()!=0. && nt0->GetMean()!=0. )
	      {
          mpcvsbbcst0->Fill(bbc_stime, st0->GetMean());
          mpcvsbbcnt0->Fill(bbc_ntime, nt0->GetMean());
          mpc_zvtx = (st0->GetMean() - nt0->GetMean()) * 15;
          mpcvsbbcvtx->Fill(bbc_zvtx, mpc_zvtx);
          float mpc_t0 = (st0->GetMean()+nt0->GetMean())/2 - (222.08+221.3)/(2*30);
          //cout << mpc_zvtx < " " << bbc_t0 << " " << mpc_t0 << endl;
          mpcvsbbct0->Fill(bbc_t0, mpc_t0);
        }      
    }

  delete st0;
  delete nt0;  

  for (int itrig=0; itrig<NMPCTRIGS; itrig++)
    {
      if ( trig_mpc[itrig] )
        {
          mpc_ehist[itrig]->Fill( esum );
          mpc_ehist_n[itrig]->Fill( esum_n );
          mpc_ehist_s[itrig]->Fill( esum_s );
        }
    }

  smpc_bbctrig_occupancy->Fill((float)soccupancy);
  nmpc_bbctrig_occupancy->Fill((float)noccupancy);		  

  return 0;
}//end process event

