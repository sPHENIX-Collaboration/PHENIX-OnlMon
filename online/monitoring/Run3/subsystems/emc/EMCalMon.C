// Online Monitoring includes
#include <EMCalMon.h>
#include <EmcAnalys.h>
#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <RunDBodbc.h>

// Online/Offline includes
#include <Event.h>
#include <msg_profile.h>
#include <EmcIndexer.h>

// Root includes
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

// system includes
#include <cmath>
#include <cstdlib> // getenv
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

EMCalMon::EMCalMon(const char *calibData): OnlMon("EMCalMON")
{
  fVerbose = 0;
  rd = 0;
  mAnalys = new Analys(this);

  EMCalMonInit();
  // Info for Emcal

  EMCal_Info = new TH1F("EMCal_Info", "Encoded EMCal Information", 200, -0.5, 195.5);

  // PBGL booking

  PbGl_sm = new TH1F("PBGL_sm", " PMTs ", 200, 0, 2);
  PbGl_ref = new TH1F("PBGL_ref", " PINs ", 200, 0, 2);
  PbGl_T_sm = new TH1F("PBGL_T_sm", " PMTs (Time) ", 500, -3000, 3000);
  PbGl_T_ref = new TH1F("PBGL_T_ref", " PINs (Time) ", 500, -3000, 3000);
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  avPbGl_SM = new TH1F("avPBGL_SM", " PMTs average", 9216, 0, 9216);
  avPbGl_T_SM = new TH1F("avPBGL_T_SM", " PMTs (Time) average", 9216, 0, 9216);
  dispPbGl_SM = new TH1F("dispPBGL_SM", " PMTs DISP", 9216, 0, 9216);
  dispPbGl_T_SM = new TH1F("dispPBGL_T_SM", " PMTs (Time) DISP", 9216, 0, 9216);

  avPbGl_REF = new TH1F("avPBGL_REF", " PINs ", 384, 0, 384);
  avPbGl_T_REF = new TH1F("avPBGL_T_REF", " PINs (Time) ", 384, 0, 384);
  dispPbGl_REF = new TH1F("dispPBGL_REF", "Reference DISP", 384, 0, 384);
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  // PBSc booking
  PbSc_sm = new TH1F("PBSC_sm", " PMTs ", 200, 0, 2);
  PbSc_ref = new TH1F("PBSC_ref", "Reference", 200, 0, 2);
  PbSc_T_sm = new TH1F("PBSC_T_sm", " PMTs (Time) ", 500, -3000, 3000);
  PbSc_T_ref = new TH1F("PBSC_T_ref", "Reference (Time) ", 500, -3000, 3000);

  PbSc_TP = new TH1F("PBSC_TP", "Test pulse (ADC)", 108, 0, 108);
  PbSc_T_TP = new TH1F("PBSC_T_TP", "Test pulse (Time)", 108, 0, 108);
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  avPbSc_SM = new TH1F("avPBSC_SM", " PMTs average", 15552, 0, 15552);
  avPbSc_T_SM = new TH1F("avPBSC_T_SM", " PMTs (Time) average", 15552, 0, 15552);
  dispPbSc_SM = new TH1F("dispPBSC_SM", " PMTs DISP", 15552, 0, 15552);
  dispPbSc_T_SM = new TH1F("dispPBSC_T_SM", " PMTs (Time) DISP", 15552, 0, 15552);

  avPbSc_REF = new TH1F("avPBSC_REF", "Reference ", 108, 0, 108);
  avPbSc_T_REF = new TH1F("avPBSC_T_REF", "Reference (Time) ", 108, 0, 108);
  dispPbSc_REF = new TH1F("dispPBSC_REF", "Reference DISP", 108, 0, 108);
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  PbSc_sm->SetXTitle("adc/adc(time0)");
  PbSc_ref->SetXTitle("adc/adc(time0)");
  PbSc_T_sm->SetXTitle("tdc-tdc(time0)");
  PbSc_T_ref->SetXTitle("tdc-tdc(time0)");

  PbGl_sm->SetXTitle("adc/adc(time0)");
  PbGl_ref->SetXTitle("adc/adc(time0)");
  PbGl_T_sm->SetXTitle("tdc-tdc(time0)");
  PbGl_T_ref->SetXTitle("tdc-tdc(time0)");

  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  Onlmonserver->registerHisto(this,EMCal_Info);

  Onlmonserver->registerHisto(this,PbGl_sm);
  Onlmonserver->registerHisto(this,PbGl_ref);
  Onlmonserver->registerHisto(this,PbGl_T_sm);
  Onlmonserver->registerHisto(this,PbGl_T_ref);
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Onlmonserver->registerHisto(this,avPbGl_SM);
  Onlmonserver->registerHisto(this,avPbGl_T_SM);
  Onlmonserver->registerHisto(this,dispPbGl_SM);
  Onlmonserver->registerHisto(this,dispPbGl_T_SM);

  Onlmonserver->registerHisto(this,avPbGl_REF);
  Onlmonserver->registerHisto(this,avPbGl_T_REF);
  Onlmonserver->registerHisto(this,dispPbGl_REF);
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Onlmonserver->registerHisto(this,PbSc_sm);
  Onlmonserver->registerHisto(this,PbSc_ref);
  Onlmonserver->registerHisto(this,PbSc_T_sm);
  Onlmonserver->registerHisto(this,PbSc_T_ref);

  Onlmonserver->registerHisto(this,PbSc_TP);
  Onlmonserver->registerHisto(this,PbSc_T_TP);
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Onlmonserver->registerHisto(this,avPbSc_SM);
  Onlmonserver->registerHisto(this,avPbSc_T_SM);
  Onlmonserver->registerHisto(this,dispPbSc_SM);
  Onlmonserver->registerHisto(this,dispPbSc_T_SM);

  Onlmonserver->registerHisto(this,avPbSc_REF);
  Onlmonserver->registerHisto(this,avPbSc_T_REF);
  Onlmonserver->registerHisto(this,dispPbSc_REF);
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  TFile *myfile = new TFile(calibData, "READ");

  if (!myfile)
    {
      ostringstream errmsg;
      errmsg << "Can't open .root"  << calibData ; 
       OnlMonServer *se = OnlMonServer::instance();
     se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_FATAL, errmsg.str(),1);
      return ;
    }
  // reference histo reading
  gROOT->cd();

  //PbGl
  Refer_PBGL_orig = (TH1*) myfile->Get("avPBGL_SM;1")->Clone();
  if (!Refer_PBGL_orig)
    {
       ostringstream errmsg;
       errmsg << "Can't get Refer_PBGL_orig" ; 
       OnlMonServer *se = OnlMonServer::instance();
       se->send_message(this,MSG_SOURCE_PBGL, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      Refer_PBGL_orig->SetName("Refer_PBGL");
    }

  ReferPin_PBGL_orig = (TH1*) myfile->Get("avPBGL_REF;1")->Clone();
  if (!ReferPin_PBGL_orig)
    {
      ostringstream errmsg;
      errmsg << "Can't get ReferPin_PBGL_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBGL, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      ReferPin_PBGL_orig->SetName("ReferPin_PBGL");
    }

  Refer_T_PBGL_orig = (TH1*) myfile->Get("avPBGL_T_SM;1")->Clone();
  if (!Refer_T_PBGL_orig)
    {
      ostringstream errmsg;
      errmsg << "Can't get Refer_T_PBGL_orig " ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBGL, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      Refer_T_PBGL_orig->SetName("Refer_T_PBGL");
    }

  ReferPin_T_PBGL_orig = (TH1*) myfile->Get("avPBGL_T_REF;1")->Clone();
  if (!ReferPin_T_PBGL_orig)
    {
      ostringstream errmsg;
      errmsg << "Can't get ReferPin_T_PBGL_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBGL, MSG_SEV_ERROR, errmsg.str(),1);  
    }
  else
    {
      ReferPin_T_PBGL_orig->SetName("ReferPin_T_PBGL");
    }

  //PbSc
  gROOT->cd();

  Refer_PBSC_orig = (TH1*) myfile->Get("avPBSC_SM;1")->Clone();
  if (!Refer_PBSC_orig)
    {
      ostringstream errmsg;  
      errmsg << "Can't get Refer_PBSC_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      Refer_PBSC_orig->SetName("Refer_PBSC");
    }

  ReferPin_PBSC_orig = (TH1*) myfile->Get("avPBSC_REF;1")->Clone();
  if (!ReferPin_PBSC_orig)
    {
      ostringstream errmsg;
      errmsg << "Can't get ReferPin_PBSC_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      ReferPin_PBSC_orig->SetName("ReferPin_PBSC");
    }

  Refer_T_PBSC_orig = (TH1*) myfile->Get("avPBSC_T_SM;1")->Clone();
  if (!Refer_T_PBSC_orig)
    {
      ostringstream errmsg; 
      errmsg << "Can't get Refer_T_PBSC_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      Refer_T_PBSC_orig->SetName("Refer_T_PBSC");
    }

  ReferPin_T_PBSC_orig = (TH1*) myfile->Get("avPBSC_T_REF;1")->Clone();
  if (!ReferPin_T_PBSC_orig)
    {
      ostringstream errmsg;
      errmsg << "Can't get ReferPin_T_PBSC_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      ReferPin_T_PBSC_orig->SetName("ReferPin_T_PBSC");
    }

  Refer_TP_PBSC_orig = (TH1*) myfile->Get("PBSC_TP;1")->Clone();
  if (!Refer_TP_PBSC_orig)
    {
      ostringstream errmsg;
      errmsg << "Can't get Refer_TP_PBSC_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      Refer_TP_PBSC_orig->SetName("Refer_TP_PBSC");
    }

  Refer_T_TP_PBSC_orig = (TH1*) myfile->Get("PBSC_T_TP;1")->Clone();
  if (!Refer_T_TP_PBSC_orig)
    {
      ostringstream errmsg; 
      errmsg << "Can't get Refer_T_TP_PBSC_orig" ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(),1);
    }
  else
    {
      Refer_T_TP_PBSC_orig->SetName("Refer_T_TP_PBSC");
    }

  myfile->Close();
  delete myfile;
  Refer_PBGL = (TH1 *) Refer_PBGL_orig->Clone();
  ReferPin_PBGL = (TH1 *) ReferPin_PBGL_orig->Clone();
  Refer_T_PBGL = (TH1 *) Refer_T_PBGL_orig->Clone();
  ReferPin_T_PBGL = (TH1 *) ReferPin_T_PBGL_orig->Clone();

  Onlmonserver->registerHisto(this,Refer_PBGL);
  Onlmonserver->registerHisto(this,ReferPin_PBGL);
  Onlmonserver->registerHisto(this,Refer_T_PBGL);
  Onlmonserver->registerHisto(this,ReferPin_T_PBGL);

  Refer_PBSC = (TH1 *) Refer_PBSC_orig->Clone();
  ReferPin_PBSC = (TH1 *) ReferPin_PBSC_orig->Clone();
  Refer_T_PBSC = (TH1 *) Refer_T_PBSC_orig->Clone();
  ReferPin_T_PBSC = (TH1 *) ReferPin_T_PBSC_orig->Clone();
  Refer_TP_PBSC = (TH1 *) Refer_TP_PBSC_orig->Clone();
  Refer_T_TP_PBSC = (TH1 *) Refer_T_TP_PBSC_orig->Clone();

  Onlmonserver->registerHisto(this,Refer_PBSC);
  Onlmonserver->registerHisto(this,ReferPin_PBSC);
  Onlmonserver->registerHisto(this,Refer_T_PBSC);
  Onlmonserver->registerHisto(this,ReferPin_T_PBSC);
  Onlmonserver->registerHisto(this,Refer_TP_PBSC);
  Onlmonserver->registerHisto(this,Refer_T_TP_PBSC);

  emc_etot = new TH1F("emc_etot", "Etotal", 500, 0.0, 500.0);
  Onlmonserver->registerHisto(this,emc_etot);


  const char *mchtitle[2][10] = {
    {"BBCLL1_PBSC_2_6GeV_c", "BBCLL1_PBSC_W0_2_6GeV_c", "BBCLL1_PBSC_W1_2_6GeV_c", "BBCLL1_PBSC_W2_2_6GeV_c",
     "BBCLL1_PBSC_W3_2_6GeV_c", "BBCLL1_PBSC_E2_2_6GeV_c", "BBCLL1_PBSC_E3_2_6GeV_c",
     "BBCLL1_PBGL_2_6GeV_c", "BBCLL1_PBGL_E0_2_6GeV_c", "BBCLL1_PBGL_E1_2_6GeV_c" },
    {"Gamma3_PBSC_2_6GeV_c", "Gamma3_PBSC_W0_2_6GeV_c", "Gamma3_PBSC_W1_2_6GeV_c", "Gamma3_PBSC_W2_2_6GeV_c",
     "Gamma3_PBSC_W3_2_6GeV_c", "Gamma3_PBSC_E2_2_6GeV_c", "Gamma3_PBSC_E3_2_6GeV_c",
     "Gamma3_PBGL_2_6GeV_c", "Gamma3_PBGL_E0_2_6GeV_c", "Gamma3_PBGL_E1_2_6GeV_c" }
  };

  for (int itr = 0; itr < 2; itr++) //itr: 0= BBCLL1, 1= Gamma3 & BBCLL1
    for (int ih = 0; ih < 10; ih++)  //ih: 2 < pt < 6 GeV/c PBSC ALL, PBSC Sector, PBGL ALL, PBSC Sector
      {
        pihist[itr][ih] = new TH1F(mchtitle[itr][ih], mchtitle[itr][ih], 100, 0, 1.0);
        pihist[itr][ih]->Sumw2();
        Onlmonserver->registerHisto(this,pihist[itr][ih]);
      }
  dbvars = new OnlMonDB(ThisName);
  DBVarInit();

}

int
EMCalMon::Init()
{
  rd = new RunDBodbc();
  return 0;
}

void
EMCalMon::avAmplReset(int emcal=0 )
{
  if(emcal==0 || emcal==1)
    {
    memset(entrPbSc, 0, sizeof(entrPbSc));
    memset(avADCPbSc, 0, sizeof(avADCPbSc));
    memset(avTACPbSc, 0 , sizeof(avTACPbSc));
    memset(av2ADCPbSc, 0, sizeof(av2ADCPbSc));
    memset(av2TACPbSc, 0, sizeof(av2TACPbSc));
    memset(dispADCPbSc, 0, sizeof(dispADCPbSc));
    memset(dispTACPbSc, 0, sizeof(dispTACPbSc));
   
    memset(entrPbScRef, 0, sizeof(entrPbScRef));
    memset(avADCPbScRef, 0, sizeof(avADCPbScRef));
    memset(avTACPbScRef, 0, sizeof(avTACPbScRef));
    memset(av2ADCPbScRef, 0, sizeof(av2ADCPbScRef));
    memset(dispADCPbScRef, 0, sizeof(dispADCPbScRef));

    memset(fStatResetPbSc, 0, sizeof(fStatResetPbSc));
    }
  if(emcal==0 || emcal==2)
    {
    memset(entrPbGl, 0, sizeof(entrPbGl));
    memset(avADCPbGl, 0, sizeof(avADCPbGl));
    memset(avTACPbGl, 0, sizeof(avTACPbGl));
    memset(av2ADCPbGl, 0, sizeof(av2ADCPbGl));
    memset(av2TACPbGl, 0, sizeof(av2TACPbGl));
    memset(dispADCPbGl, 0, sizeof(dispADCPbGl));
    memset(dispTACPbGl, 0, sizeof(dispTACPbGl));

    memset(entrPbGlRef, 0, sizeof(entrPbGlRef));
    memset(avADCPbGlRef, 0, sizeof(avADCPbGlRef));
    memset(avTACPbGlRef, 0, sizeof(avTACPbGlRef));
    memset(av2ADCPbGlRef, 0, sizeof(av2ADCPbGlRef));
    memset(dispADCPbGlRef, 0, sizeof(dispADCPbGlRef));

    memset(fStatResetPbGl, 0, sizeof(fStatResetPbGl));    
    }
  if(emcal==0)
    {
       pedallEntry =0; 
       memset(pedPbScEntryLG, 0, sizeof(pedPbScEntryLG));
       memset(pedPbScEntryHG, 0, sizeof(pedPbScEntryHG));
       memset(avpedPbScLG, 0, sizeof(avpedPbScLG));
       memset(avpedPbScHG, 0, sizeof(avpedPbScHG));
       memset(av2pedPbScLG, 0, sizeof(av2pedPbScLG));
       memset(av2pedPbScHG, 0, sizeof(av2pedPbScHG));
       memset(disppedPbScLG, 0, sizeof(disppedPbScLG));
       memset(disppedPbScHG, 0, sizeof(disppedPbScHG));
    }  
}


int EMCalMon::EMCalMonInit()
{
  const char* pbgldatfile;
  const char* pbscdatfile;
  ievt = 0;
  evt_tmp = 0;
  RefEvt[0] = 0;
  RefEvt[1] = 0;
  RefEvt[2] = 0;

  for (int i=0;i<100;i++)
    {
      Check_evt[i] = -999;
      Check_old[i] = -999;
      Check_new[i] = -999;
    }

  Check_old[61] = 10;  // N_Pbsc_ref>10
  Check_old[62] = 200; // N_Pbsc_ph>200
  Check_old[63] = 20;  // N_Pbgl_ref>40
  Check_old[64] = 200; // N_Pbgl_ph>200

  Check_old[65] = 10;  // T_Pbsc_ref>10
  Check_old[66] = 50;  // T_Pbsc_ph>50
  Check_old[67] = 100; // T_Pbgl_ref>100
  Check_old[68] = 200; // T_Pbgl_ph>200


  Check_old[80] = 100; // NpacketPBSC=100
  Check_old[81] = 64;  // NpacketPBGL=64
  Check_old[82] = 3;   // NpacketRPBSC=3
  Check_old[83] = 4;   // NpacketRPBGL=4

  Check_old[84] = 4;   // PBSC pre -post = 4
  Check_old[85] = 4;   // PBGL pre -post = 4
  Check_old[86] = 4;   // ref PBSC pre -post = 4
  Check_old[87] = 7;   // ref PBGL pre -post = 7

  Check_old[88] = 3;   // PBSC pre -time = 3
  Check_old[89] = 3;   // PBGL pre -time = 3
  Check_old[90] = 3;   // ref PBSC pre -time = 3
  Check_old[91] = 3;   // ref PBGL pre -time = 3

  //  Check_old[99]=200;// waiting too long fo laser event


  memset(PbglRefId, 0, sizeof(PbglRefId));
  memset(PbscRefId, 0, sizeof(PbscRefId));
  memset(Check_Packet, 0, sizeof(Check_Packet));

  //reading PBGL reference module list
  int id, indexSM;


  //  ifstream fromGl("/home/phnxemc/Monitor/SF_Monitor/emc/PBGL.dat");
  pbgldatfile = getenv("PBGLDATA");
  ifstream fromGl(pbgldatfile);
  if (!fromGl)
    {
      ostringstream errmsg;        
      errmsg << "Can't open .dat" << pbgldatfile ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(),1);  
    }
  else
    {
      while (fromGl >> id >> indexSM)
        {
          if (id < 576) //144*4
            {
              PbglRefId[0][id] = 1;
              PbglRefId[1][id] = indexSM;
            }
          else
            { 
              ostringstream errmsg; 
              errmsg << "Bad PBGL ref id=" << id ;
       OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this,MSG_SOURCE_PBGL, MSG_SEV_WARNING, errmsg.str(),1); 
            }
        }
      fromGl.close();
    }

  //reading PBSC reference module list
  pbscdatfile = getenv("PBSCDATA");
  ifstream fromSc(pbscdatfile);
  if (!fromSc)
    {
      ostringstream errmsg;   
      errmsg << "Can't open .dat" << pbscdatfile ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_SEVEREERROR, errmsg.str(),1);
    }
  else
    {
      while (fromSc >> id >> indexSM)
        {
          if (id < 432) //144*3
            {
              if (indexSM < 108)
                {
                  PbscRefId[0][id] = 1;
                  PbscRefId[1][id] = indexSM;
                }
            }
          else
            {
              ostringstream errmsg;   
              errmsg << "Bad PBSC ref id=" << id ;
       OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_WARNING, errmsg.str(),1);
            }

        }
      fromSc.close();
    }

  avAmplReset(0);

  return 0;
}





EMCalMon::~EMCalMon()
{
  if (mAnalys)
    {
      delete mAnalys;
    }
  if (dbvars)
    {
      delete dbvars;
    }
  delete rd;
  delete Refer_PBGL_orig;
  delete ReferPin_PBGL_orig;
  delete Refer_T_PBGL_orig;
  delete ReferPin_T_PBGL_orig;

  delete Refer_PBSC_orig;
  delete ReferPin_PBSC_orig;
  delete Refer_T_PBSC_orig;
  delete ReferPin_T_PBSC_orig;
  delete Refer_TP_PBSC_orig;
  delete Refer_T_TP_PBSC_orig;

  return;
}


int EMCalMon::EmcDetector(int ip, int ch, const char* what)
{

  ip = ip - 8001;

  if (ip < 0 || ip > 180)
    {
      ostringstream errmsg;        
      errmsg << "It isn't EMC ip=" <<   ip + 8001  ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_PBSC, MSG_SEV_WARNING, errmsg.str(),2);
      return false;
    }
  
  if (strcmp(what, "PBSC") == 0 && ip < 108)
    {
      return true;
    }
  if (strcmp(what, "PBGL") == 0 && ip >= 108 && ip < 172)
    {
      return true;
    }
  if (strcmp(what, "RPBSC") == 0 && ip >= 172 && ip < 175)
    {
      if ( PbscRefId[0][(ip - 172)*144 + ch] == 1)
        {
          return true;
        }
      return false; 
    }

  if (strcmp(what, "RPBGL") == 0 && ip >= 176 && ip < 180)
    {
      if ( PbglRefId[0][(ip - 176)*144 + ch] == 1)
        {
          return true;
        }
      return false;
    }

  return false;
}

int EMCalMon::OneCheck(int id, int test, const char* mess, int msg_source, int msg_sev)
{
  if (msg_sev < MSG_SEV_ERROR)
    {
      return true; // not print warning and information error
    }

  if (Check_evt[id] != ievt)
    {
      Check_evt[id] = ievt;
      if (Check_old[id] == -999)
        {
          Check_old[id] = test;
        }
      Check_new[id] = test;
      return true;
    }
  if (Check_new[id] != test)
    {        
      ostringstream errmsg;
      errmsg << "ip=" << IPP << " " << mess ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,msg_source, msg_sev, errmsg.str(),3);         
      return false;
    }

  return true;
}


int EMCalMon::AbsCheck(int id, int test, const char* mess, int msg_source, int msg_sev)
{
  if (msg_sev < MSG_SEV_ERROR)
    {
      return true; // not print warning and information error
    }

  if (Check_old[id] == -999)
    {
      Check_old[id] = test;
    }
  if (Check_old[id] != test)
    {     
      ostringstream errmsg;
      errmsg << mess << " " << test ;
       OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,msg_source, msg_sev, errmsg.str(),4); 
      return false;
    }

  return true;
}

unsigned EMCalMon::igray(unsigned n, int is)
  //For zero or positive values of is, return the Gray code of n; if is is negative, return the inverse Gray code of n.
  // From http://lib-www.lanl.gov/numerical/bookcpdf/c20-2.pdf
{
  int ish;
  unsigned ans,idiv;
  if ( is >= 0 ) return n ^ (n >> 1);
  ish=1;
  ans=n;
  for (;;) {
    ans ^= (idiv=ans >> ish);
    if (idiv <= 1 || ish == 16) return ans;
    ish <<= 1;
  }
}


int EMCalMon::EmcCheck(Event *evt)
{

  int Npacket = 0;
  int NpacketPbgl = 0, NpacketPbsc = 0;
  int NpacketRPbgl = 0, NpacketRPbsc = 0;

  int range = 180;
  int first_packet = 8001;

  for (int ip = 0; ip < range; ip++)
    {
      int IP = first_packet + ip;
      Packet *p = evt->getPacket(IP);
      if (p)
        {
          Npacket++;
          int BCLK = p->iValue(0, "BCLK"); // Beam clock value from FEM
          int AMU_time = (int) igray(p->iValue(0, "AMU"), -1); // AMU cell from timing conversion
          int AMU_pre  = (int) igray(p->iValue(1, "AMU"), -1); // AMU cell from "pre" conversion
          int AMU_post = (int) igray(p->iValue(2, "AMU"), -1); // AMU cell from "post" conversion
          int evtnr = p->iValue(0, "EVTNR");  //The FEM event number

          delete p;

          int pre_post = -(AMU_pre - AMU_post);
          if (pre_post < 0)
            {
              pre_post = pre_post + 64;
            }
          int pre_time = -(AMU_pre - AMU_time);
          if (pre_time < 0)
            {
              pre_time = pre_time + 64;
            }

          //  	  packet->iValue(0,"MODULE");//   The Module ID
          //  	  packet->iValue(i,"USERWORD");// The USERWORD i, where i is from 0 through 7
          //  	  packet->iValue(i,"PARITY") ;//The longitudinal parity


          IPP = IP;

          if (EmcDetector(IP, 0, "PBSC"))
            {
              NpacketPbsc++;
              if (fVerbose)
                {
                  OneCheck(0, BCLK, "bad PBSC BCLK", MSG_SOURCE_PBSC, MSG_SEV_WARNING); // ? MSG_SEV_WARNING
                  OneCheck(1, AMU_time, "bad PBSC AMU time", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(2, AMU_pre, "bad PBSC AMU pre ", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(3, AMU_post, "bad PBSC AMU post", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(4, evtnr, "bad PBSC evtnr", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  AbsCheck(84, pre_post, "bad PBSC pre - post ", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  AbsCheck(88, pre_time, "bad PBSC pre - time ", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                }
            }

          if (EmcDetector(IP, 0, "PBGL"))
            {
              NpacketPbgl++;
              if (fVerbose)
                {
                  OneCheck(10, BCLK, "bad PBGL BCLK", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(11, AMU_time, "bad PBGL AMU time", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(12, AMU_pre, "bad PBGL AMU pre ", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(13, AMU_post, "bad PBGL AMU post", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(4, evtnr, "bad PBGL evtnr", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  AbsCheck(85, pre_post, "bad PBGL pre - post", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  AbsCheck(89, pre_time, "bad PBGL pre - time", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                }
            }
          if (EmcDetector(IP, 0, "RPBSC"))
            {
              NpacketRPbsc++;
              if (fVerbose)
                {
                  OneCheck(0, BCLK, "bad REF PBSC BCLK", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(21, AMU_time, "bad REF PBSC AMU time", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(22, AMU_pre, "bad REF PBSC AMU pre ", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(23, AMU_post, "bad REF PBSC AMU post", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  OneCheck(4, evtnr, "bad REF PBSC evtnt", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  AbsCheck(86, pre_post, "bad REF PBSC pre - post", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                  AbsCheck(90, pre_time, "bad REF PBSC pre - time", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
                }
            }
          if (EmcDetector(IP, 0, "RPBGL"))
            {
              NpacketRPbgl++;
              if (fVerbose)
                {
                  OneCheck(10, BCLK, "bad REF PBGL BCLK", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(31, AMU_time, "bad REF PBGL AMU time", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(32, AMU_pre , "bad REF PBGL AMU pre", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(33, AMU_post, "bad REF PBGL AMU post", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  OneCheck(4, evtnr, "bad REF PBGL evtnr", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  AbsCheck(87, pre_post, "bad REF PBGL AMU pre - post", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                  AbsCheck(91, pre_time, "bad REF PBGL pre - time", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
                }
            }
        

        } //end if packet
    }
  if (fVerbose)
    {
      AbsCheck(80, NpacketPbsc , "wrong  numbers of PBSC packets", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
      AbsCheck(82, NpacketRPbsc, "wrong  numbers of ref PBSC packets", MSG_SOURCE_PBSC, MSG_SEV_WARNING);
      AbsCheck(81, NpacketPbgl , "wrong  numbers of PBGL packets", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
      AbsCheck(83, NpacketRPbgl, "wrong  numbers of ref PBGL packets", MSG_SOURCE_PBGL, MSG_SEV_WARNING);
    }

  return true;
}


int EMCalMon::EmcTrig(Event *evt)
{
  Packet *pGL1 = evt->getPacket(14001);
  if (pGL1)
    {
      unsigned rbib = pGL1->iValue(0, "GDISABLE");
      int ppg = (rbib >> 28) & 3;
      liveTrig = pGL1->iValue(0, "LIVETRIG");
      scaledTrig = pGL1->iValue(0, "SCALEDTRIG");

      delete pGL1;
      switch (ppg)
        {

        case 1:
          { //Pedestal
            return 1;
          }
        case 2:
          { //Test pulse
            return 2;
          }
        case 3:
          { //Laser
            return 4;
          }
        }
      if (scaledTrig & bit_used) 
        {
          if (evt->getEvtType() != 1) //using only data events
            return 0;

          return 5;
        }

    }

  return 0;  
}


int EMCalMon::process_event(Event *evt)
{
  ievt++;
  evt_tmp++;

  int first_packet = 8001;
  int range;
  int channels = 144;
  int lg = 0, time = 0;

  struct emcChannelLongList ecl[144];
  int nw;
  int NumberOfChannels;
  

  if (evt_tmp % 500 == 0 && fVerbose)
    {
      AbsCheck(91, evt_tmp, "please, please, laser event, please ", MSG_SOURCE_PBSC, MSG_SEV_ERROR);
    }

  OnlMonServer *Onlmonserver = NULL;

  int wichTrig = EmcTrig(evt);

  Onlmonserver = OnlMonServer::instance();

  if(wichTrig == 1)
    {
     range=108;
     for(int iFEM=0; iFEM < range; iFEM++)
       {
          Packet *p = evt->getPacket(first_packet + iFEM); 
          if(p)
	    {
              NumberOfChannels = p->fillIntArray ( (int *) ecl, sizeof(emcChannelLongList)*144, &nw, "SPARSE");
              for (int i = 0; i < NumberOfChannels; i++)
		{
                   int ich = ecl[i].channel;
                   int lgpre =  4095 - ecl[i].lowpre;
                   int lgpost = 4095 - ecl[i].lowpost;
                   int hgpre =  4095 - ecl[i].highpre;
                   int hgpost = 4095 - ecl[i].highpost;
                   
                   lg = lgpre - lgpost;
                   int hg = hgpre - hgpost; 
                   int towerId = EmcIndexer::PXSM144iCH_iPX(iFEM, ich); 
                   if(lg)
		     {  
		       pedPbScEntryLG[towerId]++;
                       avpedPbScLG[towerId]  += (lg - avpedPbScLG[towerId]) / pedPbScEntryLG[towerId];
                       av2pedPbScLG[towerId] += (lg * lg - av2pedPbScLG[towerId]) / pedPbScEntryLG[towerId];
                       disppedPbScLG[towerId] = av2pedPbScLG[towerId] - avpedPbScLG[towerId] * avpedPbScLG[towerId];
                       disppedPbScLG[towerId] = disppedPbScLG[towerId] > 0 ? disppedPbScLG[towerId] : 0;       
                     }  
                   if(hg)
                     {
                       pedPbScEntryHG[towerId]++;
                       avpedPbScHG[towerId]  += (hg - avpedPbScHG[towerId]) / pedPbScEntryHG[towerId];
                       av2pedPbScHG[towerId] += (hg * hg - av2pedPbScHG[towerId]) / pedPbScEntryHG[towerId];
                       disppedPbScHG[towerId] = av2pedPbScHG[towerId] - avpedPbScHG[towerId] * avpedPbScHG[towerId];
                       disppedPbScHG[towerId] = disppedPbScHG[towerId] > 0 ? disppedPbScHG[towerId] : 0;
                     } 
		} 
	      delete p;
	    }      
   
       }
     pedallEntry++;
   
    }  
  else if (wichTrig == 2) // test pulse event
    {
      RefEvt[1]++;
      if (fVerbose)
        {
          printf("EMCalMon:Processing Event %d [test-pulse trigger]\n",ievt) ;
        }

      float norm1;
      norm1 = RefEvt[1] - 1;
      if (norm1 < 1)
        {
          norm1 = 1.0;
        }
      PbSc_TP->Scale(norm1);
      PbSc_T_TP->Scale(norm1);


      range = 175;
      for (int ip = 172; ip < range; ip++)
        {
          Packet *p = evt->getPacket(first_packet + ip);
          if (p)
            {

               NumberOfChannels = p->fillIntArray ( (int *) ecl, sizeof(emcChannelLongList)*144, &nw, "SPARSE");
               if (NumberOfChannels > 144)
		 {
                   printf("NumbCh: %d\n",NumberOfChannels);
                   NumberOfChannels = -1;
		 } 

	       for (int i = 0; i < NumberOfChannels; i++)
                {                 
                   int ich = ecl[i].channel;
                   if (ich < 0 || ich >= 144)
                   {
                      printf("out range: %d\n",ich);
                      break ;
                   }

                   int lgpre = 4095 - ecl[i].lowpre;
                   int lgpost = 4095 - ecl[i].lowpost;
		  
                   lg = lgpre - lgpost;
                   time =  ecl[i].time;
                   if(lg < 0)
		     {
                       continue;  
		     }
                  if (EmcDetector(first_packet + ip, ich, "RPBSC"))
                    {
                      int IP = PbscRefId[1][(ip - 172) * channels + ich];
                      PbSc_TP->Fill(IP, lg);
                      PbSc_T_TP->Fill(IP, time);
                      // if(IP == 5)
                      //  cout << "lg=" << lg << "tac=" << time << endl;
                    }
                }
              delete p;
            }
          else
            {
              if (fVerbose)
                {
                  printf("[test-pulse trigger]Not packet ## %d\n",(first_packet + ip)) ;
                }
            }

        }

      //normalize histogram
      norm1 = 1. / RefEvt[1];
      PbSc_TP->Scale(norm1);
      PbSc_T_TP->Scale(norm1);
    }
  else if (wichTrig == 4 )  //laser event
    {
      evt_tmp = 0;
      RefEvt[2]++;
      EmcCheck(evt);

      int fStatPbGl=0, fStatPbSc=0;
      for (int IP = 0; IP < 9216; IP++)
	{
	  if(fStatResetPbGl[IP]>1)
            {
	      fStatPbGl++;
            }
        }
      for (int IP = 0; IP < 15552; IP++)
	{
	  if(fStatResetPbSc[IP]>1)
            {
	      fStatPbSc++;
            }
        }
       
      if(fStatPbGl > 144)
	{   
          avAmplReset(2);
          printf("laser event=%d status=%d",RefEvt[2],fStatPbGl);
	  printf("Average amplitude of the PBGL has been dumped\n") ;
	}
      if(fStatPbSc > 144)   
  	{
          avAmplReset(1);
          printf("laser event=%d status=%d\n",RefEvt[2],fStatPbSc);
          printf("Average amplitude of the PBSC has been dumped\n") ;
        }
      N_Pbsc_ref = 0;
      N_Pbsc_ph = 0;
      N_Pbgl_ref = 0;
      N_Pbgl_ph = 0;

      T_Pbsc_ref = 0;
      T_Pbsc_ph = 0;
      T_Pbgl_ref = 0;
      T_Pbgl_ph = 0;

      if(fVerbose)
        {
          printf("EMCalMon:Processing Event %d [laser trigger]\n",ievt) ;
        }


      // Loop through EMC Packets
      range = 180;

      for (int ip = 0; ip < range; ip++)
        {
          Packet *p = evt->getPacket(first_packet + ip);
          if (p)
            {
              NumberOfChannels = p->fillIntArray ( (int *) ecl, sizeof(emcChannelLongList)*144, &nw, "SPARSE");
              if (NumberOfChannels > 144)
		{
                  cout << "NumbCh: " << NumberOfChannels << endl;
                  NumberOfChannels = -1;
		}
               
              for (int i = 0; i < NumberOfChannels; i++)
                {
		  
                   int ich = ecl[i].channel;
                   if (ich < 0 || ich >= 144)
                   {
                      printf("out range: %d\n",ich);
                      break ;
                   }

                   int lgpre = 4095 - ecl[i].lowpre;
                   int lgpost = 4095 - ecl[i].lowpost;
		  
                   lg = lgpre - lgpost;
                   time =  4095 - ecl[i].time;
                   if(lg < 0)
		     {
                       continue;
		     }    
                       
                  //**********fill pbgl histogramm ************
                  if (EmcDetector(first_packet + ip, ich, "PBGL"))
                    {
                      int IP = (ip - 108) * channels + ich;
                      entrPbGl[IP] += 1;
                      if (2*lg < avADCPbGl[IP] && !(mAnalys->GetDataError(15552+IP) & 0xC0))
			{
			  fStatResetPbGl[IP] += 1; 
                        } 
                      avADCPbGl[IP] += (lg - avADCPbGl[IP]) / entrPbGl[IP];
                      avTACPbGl[IP] += (time - avTACPbGl[IP]) / entrPbGl[IP];
                      av2ADCPbGl[IP] += (lg * lg - av2ADCPbGl[IP]) / entrPbGl[IP];
                      av2TACPbGl[IP] += (time * time - av2TACPbGl[IP]) / entrPbGl[IP];
                      dispADCPbGl[IP] = av2ADCPbGl[IP] - avADCPbGl[IP] * avADCPbGl[IP];
                      dispTACPbGl[IP] = av2TACPbGl[IP] - avTACPbGl[IP] * avTACPbGl[IP];
		      dispADCPbGl[IP] = dispADCPbGl[IP] > 0 ? dispADCPbGl[IP] : 0;
                      dispTACPbGl[IP] = dispTACPbGl[IP] > 0 ? dispTACPbGl[IP] : 0;   

                    }
                  //**********fill pbsc histogramm ************
                  else if (EmcDetector(first_packet + ip, ich, "PBSC"))
                    {
                      int IP = ip * channels + ich;
                      entrPbSc[IP] += 1;
                      if (2*lg < avADCPbSc[IP] &&!(mAnalys->GetDataError(IP) & 0xC0))
			{   
			  fStatResetPbSc[IP] += 1; 
                        }
                      avADCPbSc[IP] += (lg - avADCPbSc[IP]) / entrPbSc[IP];
                      avTACPbSc[IP] += (time - avTACPbSc[IP]) / entrPbSc[IP];
                      av2ADCPbSc[IP] += (lg * lg - av2ADCPbSc[IP]) / entrPbSc[IP];
                      av2TACPbSc[IP] += (time * time - av2TACPbSc[IP]) / entrPbSc[IP];
                      dispADCPbSc[IP] = av2ADCPbSc[IP] - avADCPbSc[IP] * avADCPbSc[IP];
                      dispTACPbSc[IP] = av2TACPbSc[IP] - avTACPbSc[IP] * avTACPbSc[IP];
                      dispADCPbSc[IP] = dispADCPbSc[IP] > 0 ? dispADCPbSc[IP] : 0;
                      dispTACPbSc[IP] = dispTACPbSc[IP] > 0 ? dispTACPbSc[IP] : 0;                         

                    }
                  //********fill pbgl reference histogramm*******
                  else if (EmcDetector(first_packet + ip, ich, "RPBGL"))
                    {
                      int IP = PbglRefId[1][(ip - 176) * channels + ich];
                      entrPbGlRef[IP] += 1;
                      avADCPbGlRef[IP] += (lg - avADCPbGlRef[IP]) / entrPbGlRef[IP];
                      avTACPbGlRef[IP] += (time - avTACPbGlRef[IP]) / entrPbGlRef[IP];
                      avPbGl_REF->SetBinContent(IP + 1, avADCPbGlRef[IP]);
                      avPbGl_T_REF->SetBinContent(IP + 1, avTACPbGlRef[IP]);
                      
                      av2ADCPbGlRef[IP] += (lg * lg - av2ADCPbGlRef[IP]) / entrPbGlRef[IP];
                      dispADCPbGlRef[IP] = av2ADCPbGlRef[IP] - avADCPbGlRef[IP] * avADCPbGlRef[IP];
                      dispADCPbGlRef[IP] = dispADCPbGlRef[IP] > 0 ? dispADCPbGlRef[IP] : 0;
                      dispPbGl_REF->SetBinContent(IP + 1, dispADCPbGlRef[IP]);
          
                    }
                  //********fill pbsc reference histogramm*******
                  else if (EmcDetector(first_packet + ip, ich, "RPBSC"))
                    {
		      int IP = PbscRefId[1][(ip - 172) * channels + ich];
                      entrPbScRef[IP] += 1;
                      avADCPbScRef[IP] += (lg - avADCPbScRef[IP]) / entrPbScRef[IP];
                      avTACPbScRef[IP] += (time - avTACPbScRef[IP]) / entrPbScRef[IP];
                      avPbSc_REF->SetBinContent(IP + 1, avADCPbScRef[IP]);
                      avPbSc_T_REF->SetBinContent(IP + 1, avTACPbScRef[IP]);                    
                      av2ADCPbScRef[IP] += (lg * lg - av2ADCPbScRef[IP]) / entrPbScRef[IP];
                      dispADCPbScRef[IP] = av2ADCPbScRef[IP] - avADCPbScRef[IP] * avADCPbScRef[IP];
                      dispADCPbScRef[IP] = dispADCPbScRef[IP] > 0 ? dispADCPbScRef[IP] : 0;
                      dispPbSc_REF->SetBinContent(IP + 1, dispADCPbScRef[IP]);
      
                    }

                } //end for to channels

              delete p;
            } // end if to packet
          else
            {
               if (ip != 175)
                {
		  if(fVerbose)
		  {
                    printf("[laser trigger]Not packet ## %d\n",(first_packet + ip)) ;
		  }
		  Check_Packet[ip]++;
                }                  
                
            }

        }

      int nch;
      float Ampl, Time0;

      PbGl_sm->Reset();
      PbGl_T_sm->Reset();
      for (int IP = 0; IP < 9216; IP++)
        {
          avPbGl_SM->SetBinContent(IP + 1, avADCPbGl[IP]);
          dispPbGl_SM->SetBinContent(IP + 1, dispADCPbGl[IP]);
          avPbGl_T_SM->SetBinContent(IP + 1, avTACPbGl[IP]);
          dispPbGl_T_SM->SetBinContent(IP + 1, dispTACPbGl[IP]);

          nch = Refer_PBGL->FindBin(IP);
          Ampl = Refer_PBGL->GetBinContent(nch);
          if (Ampl > 200)
            {
              PbGl_sm->Fill(avADCPbGl[IP] / Ampl);
              Time0 = Refer_T_PBGL->GetBinContent(nch);
              PbGl_T_sm->Fill(avTACPbGl[IP] - Time0);
              if (fabs(lg / Ampl - 1) > 0.1)
                {
                  N_Pbgl_ph++;
                }
              if (fabs(time - Time0) > 200)
                {
                  T_Pbgl_ph++;
                }
            }
        }

      PbSc_sm->Reset();
      PbSc_T_sm->Reset();
      for (int IP = 0; IP < 15552; IP++)
        {
          avPbSc_SM->SetBinContent(IP + 1, avADCPbSc[IP]);
          dispPbSc_SM->SetBinContent(IP + 1, dispADCPbSc[IP]);
          avPbSc_T_SM->SetBinContent(IP + 1, avTACPbSc[IP]);
          dispPbSc_T_SM->SetBinContent(IP + 1, dispTACPbSc[IP]);

          nch = Refer_PBSC->FindBin(IP);
          Ampl = Refer_PBSC->GetBinContent(nch);
          if (Ampl > 50)
            {
              PbSc_sm->Fill(avADCPbSc[IP] / Ampl);
              Time0 = Refer_T_PBSC->GetBinContent(nch);
              PbSc_T_sm->Fill(avTACPbSc[IP] - Time0);
              if (fabs(lg / Ampl - 1) > 0.30)
                {
                  N_Pbsc_ph++;
                }
              if (fabs(time - Time0) > 200)
                {
                  T_Pbsc_ph++;
                }
            }
        }

      PbGl_ref->Reset();
      PbGl_T_ref->Reset();
      for (int IP = 0; IP < 384; IP++)
        {
          nch = ReferPin_PBGL->FindBin(IP);
          Ampl = ReferPin_PBGL->GetBinContent(nch);
          if (Ampl > 200)
            {
              PbGl_ref->Fill(avADCPbGlRef[IP] / Ampl);
              Time0 = ReferPin_T_PBGL->GetBinContent(nch);
              PbGl_T_ref->Fill(avTACPbGlRef[IP] - Time0);
              if (fabs(lg / Ampl - 1) > 0.15)
                {
                  N_Pbgl_ref++;
                }
              if (fabs(time - Time0) > 300)
                {
                  T_Pbgl_ref++;
                }
            }
        }

      PbSc_ref->Reset();
      PbSc_T_ref->Reset();
      for (int IP = 0; IP < 108; IP++)
        {
          nch = ReferPin_PBSC->FindBin(IP);
          Ampl = ReferPin_PBSC->GetBinContent(nch);
          if (Ampl > 50)
            {
              PbSc_ref->Fill(avADCPbScRef[IP] / Ampl);
              Time0 = ReferPin_T_PBSC->GetBinContent(nch);
              PbSc_T_ref->Fill(avTACPbScRef[IP] - Time0);
              if (fabs(lg / Ampl - 1) > 0.30)
                {
                  N_Pbsc_ref++;
                }
              if (fabs(time - Time0) > 200)
                {
                  T_Pbsc_ref++;
                }
            }
        }

      for (int IP = 0; IP < 180; IP++)
        {
          EMCal_Info->SetBinContent(IP + 11, Check_Packet[IP]);
        }
      if (fVerbose)
        {
          if (N_Pbsc_ref > Check_old[61])
            {
              AbsCheck(61, N_Pbsc_ref, " PBSC reference problem ", MSG_SOURCE_PBSC, MSG_SEV_ERROR);
            }
          if (N_Pbsc_ph > Check_old[62])
            {
              AbsCheck(62, N_Pbsc_ph, "PBSC PMT's problem  ", MSG_SOURCE_PBSC, MSG_SEV_ERROR);
            }
          if (N_Pbgl_ref > Check_old[63])
            {
              AbsCheck(63, N_Pbgl_ref, " PBGL reference problem ", MSG_SOURCE_PBGL, MSG_SEV_ERROR);
            }
          if (N_Pbgl_ph > Check_old[64])
            {
              AbsCheck(64, N_Pbgl_ph, "PBGL PMT's problem  ", MSG_SOURCE_PBGL, MSG_SEV_ERROR);
            }
          if (T_Pbsc_ref > Check_old[65])
            {
              AbsCheck(65, T_Pbsc_ref, " PBSC reference time problem ", MSG_SOURCE_PBSC, MSG_SEV_ERROR);
            }
          if (T_Pbsc_ph > Check_old[66])
            {
              AbsCheck(66, T_Pbsc_ph, "PBSC PMT's time problem  ", MSG_SOURCE_PBSC, MSG_SEV_ERROR);
            }
          if (T_Pbgl_ref > Check_old[67])
            {
              AbsCheck(67, T_Pbgl_ref, " PBGL reference time problem ", MSG_SOURCE_PBGL, MSG_SEV_ERROR);
            }
          if (T_Pbgl_ph > Check_old[68])
            {
              AbsCheck(68, T_Pbgl_ph, "PBGL PMT's time problem  ", MSG_SOURCE_PBGL, MSG_SEV_ERROR);
            }
        }

    }
  else if (wichTrig == 5 && mAnalys->ProcessBBC_ZDC(evt))
    {
      if (fVerbose)
	{
          printf("EMCalMon:Processing Event %d [BBCLL1 trigger]",ievt) ;
	}
      mAnalys->ProcessPhysEMC(evt);
      if (mAnalys->ProcessPi0())
        {
          bool trigMask[4];
	  for (int i=0;i<4;i++)
	    {
              trigMask[i] =false;
	    }
          if ( Onlmonserver->Trigger("BBCLL1(>0 tubes)") || Onlmonserver->Trigger("BBCLL1(>0 tubes) novertex") ) // (Onlmonserver->Trigger("ONLMONBBCLL1")) 
            {
              trigMask[0] = true;
            }
	  //modfied by karatsu (contact: karatsu@scphys.kyoto-u.ac.jp)
          //if (Onlmonserver->Trigger("ERTLL1_4x4c&BBCLL1") ) 
          if ( 
	      Onlmonserver->Trigger("ERT_4x4c&BBCLL1") ||      
	      Onlmonserver->Trigger("ERT_4x4a&BBCLL1") ||
	      // Onlmonserver->Trigger("ERT_4x4c&BBCLL1(noVtx)") ||
	      // Onlmonserver->Trigger("ERT_4x4a&BBCLL1(noVtx)") || 
	      // Onlmonserver->Trigger("ERT_4x4c&BBCLL1(narrow)") || 
	      // Onlmonserver->Trigger("ERT_4x4b")                   
	      Onlmonserver->Trigger("ERT_4x4b&BBCLL1") 
             ) 
            {
              trigMask[1] = true;
            }

          //cout << "etot=" << mAnalys->emc_etot << " npair=" << mAnalys->emc_npair << endl;
          //cout<< "sec=" <<mAnalys->emc_sec1[i] <<" pt=" <<mAnalys->emc_pt[i] << " mc=" <<mAnalys->emc_mc[i]<< endl;
          emc_etot->Fill(mAnalys->Emc_Etot());

          float ipt, ptMin, ptMax;
          ptMin = 2.0;
          ptMax = 6.0;
          for (int i = 0; i < mAnalys->Emc_Npair(); i++)
            {
              ipt = mAnalys->EmcClPair[i].emc_pt;
              if (ipt < 0.)
                {
                  ipt = 0.;
                }
              if (ipt < ptMin || ipt > ptMax)
                {
                  continue;   //ptMin  < pt < ptMax
                }

              //energy asymmetry cut added by karatsu (contact: karatsu@scphys.kyoto-u.ac.jp)
              float tmpe1 = mAnalys->EmcClPair[i].emc_eg1;
              float tmpe2 = mAnalys->EmcClPair[i].emc_eg2;
              float ene_asym = fabs( (tmpe1 - tmpe2)/(tmpe1 + tmpe2) );
              //energy asymmetry cut for eta
              if( ene_asym>0.7 ) continue;
              //cout << ene_asym << endl;

              if (mAnalys->EmcClPair[i].emc_pr1 > 0.03 && mAnalys->EmcClPair[i].emc_pr2 > 0.03 && mAnalys->Emc_Etot() < 70.)
                {
                  if (mAnalys->EmcClPair[i].emc_sec1 < 6 && mAnalys->EmcClPair[i].emc_sec2 < 6)
                    {
                      for (int itr = 0; itr < 2; itr++)
                        {
                          if (trigMask[itr]) //BBCLL1,  Gamma3&BBCLL1
                            {
                              pihist[itr][0]->Fill(mAnalys->EmcClPair[i].emc_mc); //PBSC ALL
                              for (int is = 0; is < 6; is++) //Sector
                                {
                                  if (mAnalys->EmcClPair[i].emc_sec1 == is && mAnalys->EmcClPair[i].emc_sec2 == is)
                                    {
                                      pihist[itr][is + 1]->Fill(mAnalys->EmcClPair[i].emc_mc);
                                      break;
                                    }
                                }
                            }

                        }
                    }
                  else if ( (mAnalys->EmcClPair[i].emc_sec1 == 6 || mAnalys->EmcClPair[i].emc_sec1 == 7) && (mAnalys->EmcClPair[i].emc_sec2 == 6 || mAnalys->EmcClPair[i].emc_sec2 == 7) )
                    {
                      for (int itr = 0; itr < 2; itr++)
                        {
                          if (trigMask[itr]) //BBCLL1, Gamma3&BBCLL1
                            {
                              pihist[itr][7]->Fill(mAnalys->EmcClPair[i].emc_mc); //PBGL ALL
                              for (int is = 6; is < 8; is++) //Sector
                                {
                                  if (mAnalys->EmcClPair[i].emc_sec1 == is && mAnalys->EmcClPair[i].emc_sec2 == is)
                                    {
                                      pihist[itr][is + 2]->Fill(mAnalys->EmcClPair[i].emc_mc);
                                      break;
                                    }
                                }
                            }
                        }

                    }
                }

            } // end for

        } //if pi0
    }

  EMCal_Info->SetBinContent(0, ievt);       // Total event
  EMCal_Info->SetBinContent(1, RefEvt[2]);  // Laser events
  EMCal_Info->SetBinContent(2, RefEvt[1]);  // Test pulse events

  return 0;
}

int EMCalMon::BeginRun(const int runno)
{
  //   if(!mAnalys->GetStatusCalib())
  //     {
  mAnalys->Setup(runno);
  //    }
  memset(Check_Packet, 0, sizeof(Check_Packet));
  // since all histograms are resetted at the end of a run
  // we loose the reference histograms and they have to be refilled from
  // a local copy
  // for the first run they are not resetted and contain already
  // data, so the integral is > 1 and we don't add the local copy

  // PbGl:
  if (Refer_PBGL->Integral() < 1) // double-> do not check == 0
    {
      Refer_PBGL->Add(Refer_PBGL_orig, 1.);
    }

  if (ReferPin_PBGL->Integral() < 1)
    {
      ReferPin_PBGL->Add(ReferPin_PBGL_orig, 1.);
    }

  if (Refer_T_PBGL->Integral() < 1)
    {
      Refer_T_PBGL->Add(Refer_T_PBGL_orig, 1.);
    }

  if (ReferPin_T_PBGL->Integral() < 1)
    {
      ReferPin_T_PBGL->Add(ReferPin_T_PBGL_orig, 1.);
    }

  // PbSC:
  if (Refer_PBSC->Integral() < 1)
    {
      Refer_PBSC->Add(Refer_PBSC_orig, 1.);
    }

  if (ReferPin_PBSC->Integral() < 1)
    {
      ReferPin_PBSC->Add(ReferPin_PBSC_orig, 1.);
    }

  if (Refer_T_PBSC->Integral() < 1)
    {
      Refer_T_PBSC->Add(Refer_T_PBSC_orig, 1.);
    }

  if (ReferPin_T_PBSC->Integral() < 1)
    {
      ReferPin_T_PBSC->Add(ReferPin_T_PBSC_orig, 1.);
    }

  if (Refer_TP_PBSC->Integral() < 1)
    {
      Refer_TP_PBSC->Add(Refer_TP_PBSC_orig, 1.);
    }

  if (Refer_T_TP_PBSC->Integral() < 1)
    {
      Refer_T_TP_PBSC->Add(Refer_T_TP_PBSC_orig, 1.);
    }

  // reset bit selection
  bit_used = 0;
  if (! TriggerList.empty())
    {

      OnlMonServer *se = OnlMonServer::instance();
      std::set<std::string>::const_iterator iter;
      for (iter = TriggerList.begin(); iter != TriggerList.end(); iter++)
	{
	  string trigname = *iter;
	  // exclude ppg events
	  if ((*iter).find("PPG") == string::npos)
	    {
	      bit_used |= se->getLevel1Bit(*iter);
	    }
      }
  }
  return 0;
}

int
EMCalMon::EndRun(const int runno)
{
  savepedEntry(runno, "pbscpedEntry.dat");
  mAnalys->SaveTowerHits("emc_twrhits.dat");
  if (rd->RunType(runno) == "PHYSICS")
    {
      DBSetVar("pbscPi0");
      DBSetVar("pbglPi0");
      dbvars->DBcommit();
    }
  return 0;
}

int EMCalMon::Reset()
{
  ievt = 0;
  evt_tmp = 0;
  memset(RefEvt, 0, sizeof(RefEvt));
  avAmplReset(0);

  return 0;
}


int EMCalMon::DBVarInit()
{
  string varname;
  varname = "pbscPi0mean";
  dbvars->registerVar(varname);
  varname = "pbscPi0sigma";
  dbvars->registerVar(varname);
  varname = "pbglPi0mean";
  dbvars->registerVar(varname);
  varname = "pbglPi0sigma";
  dbvars->registerVar(varname);
  if (fVerbose)
    {
      dbvars->Print();
    }
  dbvars->DBInit();

  return 0;
}


int EMCalMon::DBSetVar(const char *varname)
{
  int choiceVar = 0;

  const char *mchtitle[2][2] = {
    {"BBCLL1_PBSC_2_6GeV_c", "BBCLL1_PBGL_2_6GeV_c" },
    {"Gamma3_PBSC_2_6GeV_c", "Gamma3_PBGL_2_6GeV_c" }
  };

  const char *strvarname [] = {"pbscPi0", "pbglPi0"};


  const char *strname[2][2] = { {"pbscPi0mean", "pbscPi0sigma"},
                          {"pbglPi0mean", "pbglPi0sigma"} };

  TH1 *mchist[2][2];
  int ntrig = 2; // BBCLL1 &  Gamma3
  TH1 *mcCopy[2] = {0,0};

  OnlMonServer *se = OnlMonServer::instance();

  for (int itr = 0; itr < 2; itr++)
    {
      for (int ih = 0; ih < 2; ih++)
	{
	  mchist[itr][ih] = se->getHisto(mchtitle[itr][ih]);
	}
    }
  for (int ih = 0; ih < 2; ih++)
    {
      if (!strcmp(varname, strvarname[ih]))
        {
          mcCopy[0] = (TH1*) mchist[0][ih]->Clone();  // BBCLL1
          mcCopy[1] = (TH1*) mchist[1][ih]->Clone();  // Gamma3
          choiceVar = ih;
          break;
        }
    }
  if (mcCopy[0]->GetEntries() < 100)
    {
      for (int i=0;i<2;i++)
	{
	  delete mcCopy[i];
	}
      return 0;
    }
  if (mcCopy[1]->GetEntries() < 100)
    {
      ntrig = 1;
    }

  int peak;
  double xp, yp = 0, polV = 0;

  TF1 *fitp0 = new TF1("fitp0", "pol2(0)+gaus(3)");
  TF1 *fpol2 = new TF1("fpol2", "pol2(0)", 0.07, 0.25);

  TSpectr *spec = new TSpectr(10);
  for (int itr = 0; itr < ntrig; itr++)
    {
      // int nfind = spec->Search(mcCopy[itr], 7);
      int nfind = spec->Search(mcCopy[itr], 9); 
      float *xpeaks = spec->GetPositionX();
      peak = 0;
      for (int ip = 0; ip < nfind; ip++)
        {
          if (xpeaks[ip] > 0.1 && xpeaks[ip] < 0.2)
            {
              xp = xpeaks[ip];
              yp = mcCopy[itr]->GetBinContent(mcCopy[itr]->GetXaxis()->FindBin(xp));
              double ypl = mcCopy[itr]->GetBinContent(mcCopy[itr]->GetXaxis()->FindBin(xp - 0.01));
              double ypr = mcCopy[itr]->GetBinContent(mcCopy[itr]->GetXaxis()->FindBin(xp + 0.01));
              mcCopy[itr]->Fit("fpol2", "0Q", "RM", 0.07, 0.25);
              polV = fpol2->Eval(xp);
              if ((yp - sqrt(yp) > polV) && (ypl - sqrt(ypl) > fpol2->Eval(xp - 0.01)) &&
                  (ypr - sqrt(ypr) > fpol2->Eval(xp + 0.01)))
                {
                  peak = 1;
                }
              break;
            }
        }
      if (peak)
        {
          fitp0->SetParameters(100, 0.1, 0.1, yp - polV, 0.148, 0.01);
          fitp0->SetParLimits(3, 10.0, 10000000.0);
          fitp0->SetParLimits(4, 0.07, 0.25);
          fitp0->SetParLimits(5, 0.005, 0.05);
          mcCopy[itr]->Fit("fitp0", "0Q", "RM", 0.07, 0.25);
          float mean = fitp0->GetParameter(4);
          float errmean = fitp0->GetParError(4);
          float sigma = fitp0->GetParameter(5);
          float errsigma = fitp0->GetParError(5);
          if (mean > 0.128 && mean < 0.150)
	    {
            dbvars->SetVar(strname[choiceVar][0], mean, errmean, 1.0);
	    }
          else
	    {
            dbvars->SetVar(strname[choiceVar][0], mean, errmean, 0.0);
	    }
          if (sigma > 0.007 && sigma < 0.04)
	    {
            dbvars->SetVar(strname[choiceVar][1], sigma, errsigma, 1.0);
	    }
          else
	    {
            dbvars->SetVar(strname[choiceVar][1], sigma, errsigma, 0.0);
	    }
          // dbvars->DBcommit();
        }

    }

  delete fitp0;
  delete fpol2;
  delete spec;
      for (int i=0;i<2;i++)
	{
	  delete mcCopy[i];
	}

  return 1;
}

void EMCalMon::savepedEntry(const int runno, const char *fileName)
{
  ofstream fout(fileName, ios_base::out);
  if (!fout)
    {
      return;
    }
  fout << runno << " " << pedallEntry << endl;

  for (int towerId = 0; towerId < 15552; towerId++)
    {
      fout << towerId <<" "<< pedPbScEntryHG[towerId] <<" "<< avpedPbScHG[towerId] <<" "<< sqrt(disppedPbScHG[towerId]);
      fout <<" "<< pedPbScEntryLG[towerId] <<" "<< avpedPbScLG[towerId] <<" "<< sqrt(disppedPbScLG[towerId]) << endl;  
    }

  fout.close();
}

