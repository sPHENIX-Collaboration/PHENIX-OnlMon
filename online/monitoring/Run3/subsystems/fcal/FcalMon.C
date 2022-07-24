//
//
//  Written by:  Jane M. Burward-Hoy and Gerd J. Kunde
//
//
//
//SCJ>  Remove the initChannelConfig routine and replace it with the indexer
//SCJ>  29 Jan 2003
//
//RJN>  Filter PPG events from Analog Reset monitor
//RJN>  27 Mar 2004

#include <FcalMon.h>
#include <OnlMonServer.h>
#include <OnlMonTrigger.h>

#include <Event.h>
#include <FclIndexer.h>
#include <FclCalib.h>

#include <BbcEvent.hh>
#include <BbcCalib.hh>
#include <Bbc.hh>
#include <ZdcEvent.hh>
#include <ZdcCalib.hh>
#include <packet_gl1.h>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// for monitor channel
const int MONROW = 6;
const int MONCOL = 0;

// for channel histograms
const int NCHBINS = 144;
const float CHMIN = -0.5;
const float CHMAX = 143.5;

// for 2D histograms of channel configuration
const int NXBINS = 9;
const int NYBINS = 10;
const float YMAX = 9.5;
const float YMIN = -0.5;
const float XMAX = 8.5;
const float XMIN = -0.5;


FcalMon::FcalMon(): OnlMon("FCALMON")
{
  ppg_trigger_mask = 0;
  fclcalib[FCALNORTH] = 0;
  fclcalib[FCALSOUTH] = 0;
  indexer = 0;
  return ;

}

FcalMon::~FcalMon()
{
  delete fclcalib[FCALSOUTH];
  delete fclcalib[FCALNORTH];
  for (int det = 0; det < 2; det++)
    {
      delete hpFCAL_LOADC_MEAN[det];
    }
  delete indexer;
  return ;
}

int FcalMon::Init()
{

  indexer = FclIndexer::Instance();
  initAndRegisterHistos();
  initCalib();
  Reset();
  return 0;
}

int FcalMon::formatChanHist(TH1 *thisHisto)
{

  int HISTLINECOLOR = 6;
  int HISTMARKER = 20;

  thisHisto->SetMarkerColor(HISTLINECOLOR);
  thisHisto->SetLineColor(HISTLINECOLOR);
  thisHisto->SetMarkerStyle(HISTMARKER);

  return 0;

}


void FcalMon::initCalib()
{
  //Just grabs the latest constants from the Db I guess
  //need to give it a time or run number
  //but runnumber doesn't seem to work, so...:

  //Just after approximate entry of run 5 calibrations in database:
  PHTimeStamp timestp(2007, 12, 20, 12, 0, 0, 0);

  fclcalib[FCALSOUTH] = new FclCalib;
  fclcalib[FCALSOUTH]->setSide(FCALSOUTH);
  fclcalib[FCALSOUTH]->getDatabaseInfo(timestp);

  fclcalib[FCALNORTH] = new FclCalib;
  fclcalib[FCALNORTH]->setSide(FCALNORTH);
  fclcalib[FCALNORTH]->getDatabaseInfo(timestp);


}

int FcalMon::initAndRegisterHistos()
{

  ///////////////////////////////////////////////////////////////////////

  ostringstream idstring;
  ostringstream title;
  const char* compass[2] = {"north", "south"};

  //0 = north, 1 = south, see $OFFLINE_MAIN/include/FclConsts.h
   for(int det = 0; det < 2; det++) {
    ///////////////////////////////////////////////////////////////////////

    idstring << "hpFCAL_" << compass[det] << "_LOADC_MEAN" << ends;
    title << "Mean lo gain ADC values for FCAL " << compass[det] << ends;
    hpFCAL_LOADC_MEAN[det] = new TProfile(idstring.str().c_str(),title.str().c_str(), NCHBINS, CHMIN, CHMAX);
    hpFCAL_LOADC_MEAN[det]->GetXaxis()->SetTitle("ASIC Channel");
    hpFCAL_LOADC_MEAN[det]->GetYaxis()->SetTitle("LO ADC MEAN");
    idstring.str(""); title.str("");

    // LO GAIN ADC value (post - pre) in each row and column

    idstring << "fcal_" << compass[det] << "_loadc" << ends;
    title << compass[det] << " Stack Geometry: lo gain ADC" << ends;
    h2FCAL_LOADC[det] = new TH2F(idstring.str().c_str(),title.str().c_str(), NXBINS, XMIN, XMAX, NYBINS, YMIN, YMAX);
    h2FCAL_LOADC[det]->GetXaxis()->SetTitle("Module Column");
    h2FCAL_LOADC[det]->GetYaxis()->SetTitle("Module Row");
    h2FCAL_LOADC[det]->GetXaxis()->SetNdivisions(NXBINS);
    h2FCAL_LOADC[det]->GetYaxis()->SetNdivisions(NYBINS);
    idstring.str(""); title.str("");

    // JLK Corrected ADC value in each row and column
    idstring << "fcal_" << compass[det] << "_loadc_corrected" << ends;
    title << "Fcal " << compass[det] << ": Gain-corrected ADC Values" << ends;
    h2FCAL_LOADC_CORRECTED[det] = new TH2F(idstring.str().c_str(),title.str().c_str(), NXBINS, XMIN, XMAX, NYBINS, YMIN, YMAX);
    h2FCAL_LOADC_CORRECTED[det]->GetXaxis()->SetTitle("Module Column");
    h2FCAL_LOADC_CORRECTED[det]->GetYaxis()->SetTitle("Module Row");
    h2FCAL_LOADC_CORRECTED[det]->GetXaxis()->SetNdivisions(NXBINS);
    h2FCAL_LOADC_CORRECTED[det]->GetYaxis()->SetNdivisions(NYBINS);
    idstring.str(""); title.str("");
  }  //det loop

  ///////////////////////////////////////////////////////////////////////


  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  for (int det = 0; det < 2; det++)
    {

      Onlmonserver->registerHisto(this,h2FCAL_LOADC[det]);
      Onlmonserver->registerHisto(this,h2FCAL_LOADC_CORRECTED[det]);

    }

  return 0;

}

int FcalMon::Reset()
{

  evtcnt = 0;

  return 0;

}

int FcalMon::getPostMinusPre(int post, int pre)
{

  int gain;

  if (post < POSTTHRESHOLD)
    {
      gain = ADCZERO;
    }
  else
    {
      gain = post - pre;
    }

  return gain;

}


int FcalMon::process_event(Event *evt)
{
  ostringstream msg;

  //Get the indexer:

  evtcnt++;

  if (evtcnt % 500 == 0)
    {
      printf("FcalMon:Processing Event %d\n",evtcnt);
    }

  // Determine if this is a PPG(Pedestal), PPG(Test Pulse), or PPG(Laser)
  // event.  These events are triggered during the abort gap when the FCAL
  // analog reset is being performed.  The histograms that check for
  // negative signals should not be filled for these events.

  // need to init eventwise variables here

  Packet *p = 0;
  for (int det = 0; det < 2; det++)
    {

      //need to get calibs here with side info...

      if (det == FCALNORTH)
	{
	  p = evt->getPacket(FEM_ID_NORTH);
	}
      if (det == FCALSOUTH)
	{
	  p = evt->getPacket(FEM_ID_SOUTH);
	}

      if (!p)
	{
        if (evtcnt > 1)
          {
            //          cout << "FcalMon::process_event():  Detector: " << det << " does not exist for evtcnt " << evtcnt << "  Skipping..." << endl;
          }
      }
    else
      {

      // loop over our channels in datastream and get the values we want

      for (int k = 0; k < CHANTOT; k++)
        {

          int loadcpost = p->iValue(k, 2);
          int loadcpre = p->iValue(k, 4);
          int logain = getPostMinusPre(loadcpost, loadcpre);
          if (logain != ADCZERO)
            {
              hpFCAL_LOADC_MEAN[det]->Fill(k, (float)(logain));
              int lobin = hpFCAL_LOADC_MEAN[det]->FindBin(k);
              float lomean = hpFCAL_LOADC_MEAN[det]->GetBinContent(lobin);
              int lobinx = h2FCAL_LOADC[det]->GetXaxis()->FindBin(indexer->getColumn(det,k));
              int lobiny = h2FCAL_LOADC[det]->GetYaxis()->FindBin(indexer->getRow(det,k));
              h2FCAL_LOADC[det]->SetBinContent(lobinx, lobiny, lomean);

	      //Gain corrected data, zeroing out bad channels
	      if(fclcalib[det]->getCalib(k) <=0) {
		h2FCAL_LOADC_CORRECTED[det]->SetBinContent(lobinx,lobiny,0.0);
	      }else{
		h2FCAL_LOADC_CORRECTED[det]->SetBinContent(lobinx,lobiny,lomean/fclcalib[det]->getCalib(k));
	      }
	    }
	}
    
      // tidy-up
      if (p)
	{
	  delete p;
	}
      }

    } //det 

  // histogram eventwise variables here

  return 0;

}


// the South stack module numbers (temporarily hard-wired - to be put into database.  indexing needs to be reversed (horizontal starting from inside dimension).

/*
  s[0][9]=710; s[1][9]=711; s[2][9]= 477; s[3][9]= 360; s[4][9]= 348; s[5][9]=	323; s[6][9]= 359;s[7][9]=203; s[8][9]= 226;
  s[0][8]=136; s[1][8]=761; s[2][8]= 739; s[3][8]= 741; s[4][8]= 731; s[5][8]=	392; s[6][8]= 365;s[7][8]=310; s[8][8]= 176;
  s[0][7]=702; s[1][7]=694; s[2][7]= 575; s[3][7]= 674; s[4][7]= 689; s[5][7]=	696; s[6][7]= 691;s[7][7]=704; s[8][7]= 134;
  s[0][6]=057; s[1][6]=063; s[2][6]= 728; s[3][6]= 709; s[4][6]= 706; s[5][6]=	705; s[6][6]= 708;s[7][6]=712; s[8][6]= 699;
  s[0][5]=040; s[1][5]=047; s[2][5]= 073; s[3][5]= 065; s[4][5]= 062; s[5][5]=	060; s[6][5]= 061;s[7][5]=066; s[8][5]= 058;
  s[0][4]=364; s[1][4]=440; s[2][4]= 329; s[3][4]= 339; s[4][4]= 335; s[5][4]=	344; s[6][4]= 331;s[7][4]=341; s[8][4]= 467;
  s[0][3]=260; s[1][3]=275; s[2][3]= 265; s[3][3]= 305; s[4][3]= 307; s[5][3]=	286; s[6][3]= 461;s[7][3]=456; s[8][3]= 443;
  s[0][2]=566; s[1][2]=011; s[2][2]= 100; s[3][2]= 422; s[4][2]= 435; s[5][2]=	436; s[6][2]= 315;s[7][2]=235; s[8][2]= 261;
  s[0][1]=571; s[1][1]=597; s[2][1]= 543; s[3][1]= 542; s[4][1]= 551; s[5][1]=	549; s[6][1]= 563;s[7][1]=561; s[8][1]= 562;
  s[0][0]=129; s[1][0]=153; s[2][0]= 159; s[3][0]= 152; s[4][0]= 033; s[5][0]=	146; s[6][0]= 075;s[7][0]=019; s[8][0]= 565;
*/


int FcalMon::BeginRun(const int runnumber)
{
  ostringstream msg;
  OnlMonServer *se = OnlMonServer::instance();
  if (runnumber > 0)
    {
      for (int i = 0; i < 32; i++)
        {
          int hexvalue = 1;
          hexvalue = hexvalue << i;

          string trigname = se->OnlTrig()->get_lvl1_trig_name(i);
          if (trigname.find("PPG") != string::npos
              || trigname == "Clock"
              || trigname == "ZDCLL1narrow")
            {
              msg << "        " << se->OnlTrig()->get_lvl1_trig_name(i) << ": "
              << dec << i << " 0x" << hex << hexvalue << dec << " "
              << se->OnlTrig()->get_lvl1_trig_scale_down(i) << endl;
              ppg_trigger_mask = hexvalue | ppg_trigger_mask;
            }
        }
      printf("PPG/Clock Trigger mask 0x%lx\n",ppg_trigger_mask);
    }

  return 0;
}

