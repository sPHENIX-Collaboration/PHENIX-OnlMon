#include <BbcMon.h>
#include <OnlMonServer.h>
#include <OnlMonTrigger.h>
#include <OnlMonDB.h>

#include <BbcEvent.hh>
#include <BbcCalib.hh>
#include <ZdcEvent.hh>
#include <ZdcCalib.hh>

#include <Event.h>
#include <EventTypes.h>

#include <msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TPaveText.h>
#include <TProfile.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// #define PRINT_HIST_LIST

#ifdef PRINT_HIST_LIST
#define registHist(x,h)  {cout<<"cl->registerHisto(\""<<h->GetName()<<"\",\"BBCMON\");"<<endl;x->registerHisto(this,h);}
#else
#define registHist(x,h) x->registerHisto(this,h)
#endif

using namespace std;

BBCMon::BBCMon(const char *thisname): OnlMon(thisname)
{
  evtcnt = 0;

  // -------------------------------------------------------------------------
  // Create Histograms

  ostringstream name, title;
  for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
  {
    // TDC Distribution ----------------------------------------------------
    name << "bbc_tdc" << tdc ;
    title << "BBC Raw TDC" << tdc << " Distribution" ;
    bbc_tdc[tdc] = new TH2F(name.str().c_str(), title.str().c_str(),
	nPMT_BBC, -.5, nPMT_BBC - .5,
	nBIN_TDC, 0, tdc_max_overflow * TDC_CONVERSION_FACTOR );
    name.str("");
    title.str("");

    // TDC Overflow Deviation ----------------------------------------------
    name << "bbc_tdc" << tdc << "_overflow" ;
    title << "BBC TDC" << tdc << " Overflow Deviation" ;
    bbc_tdc_overflow[tdc] = new TH2F(name.str().c_str(), title.str().c_str(),
	nPMT_BBC, -.5, nPMT_BBC - .5,
	int(VIEW_OVERFLOW_MAX - VIEW_OVERFLOW_MIN + 1),
	VIEW_OVERFLOW_MIN - .5, VIEW_OVERFLOW_MAX + .5 );
    name.str("");
    title.str("");


    // TDC Overflow Distribution for each PMT ------------------------------
    for ( int i = 0 ; i < nPMT_BBC ; i++ )
    {
      name << "bbc_tdc" << tdc << "_overflow_" << setw(3) << setfill('0') << i ;
      title << "BBC TDC" << tdc << " Overflow Deviation of #" << setw(3) << setfill('0') << i ;
      bbc_tdc_overflow_each[tdc][i] = new TH1F(name.str().c_str(), title.str().c_str(),
	  int(VIEW_OVERFLOW_MAX - VIEW_OVERFLOW_MIN + 1),
	  VIEW_OVERFLOW_MIN, VIEW_OVERFLOW_MAX );
      name.str("");
      title.str("");
    }
  }

  // ADC Distribution --------------------------------------------------------

  bbc_adc = new TH2F("bbc_adc", "BBC ADC(Charge) Distribution",
      nPMT_BBC, -.5, nPMT_BBC - .5,
      nBIN_ADC, 0, MAX_ADC_MIP );

  for ( int trig = 0 ; trig < nTRIGGER ; trig++ )
  {
    // nHit ----------------------------------------------------------------

    name << "bbc_nhit_" << TRIGGER_str[trig] ;
    title << "BBC nHIT by " << TRIGGER_str[trig] ;
    bbc_nhit[trig] = new TH1D(name.str().c_str(), title.str().c_str(),
	nPMT_BBC, -.5, nPMT_BBC - .5 );
    name.str("");
    title.str("");
        
  }

  bbc_tdc_armhittime = new TH2F("bbc_tdc_armhittime",
      "Arm-Hit-Time Correlation of North and South BBC",
      64, min_armhittime, max_armhittime,
      64, min_armhittime, max_armhittime );
  bbc_tdc_armhittime->GetXaxis()->SetTitle("South[ns]");
  bbc_tdc_armhittime->GetYaxis()->SetTitle("North[ns]");

  bbc_zvertex = new TH1F("bbc_zvertex",
      "BBC ZVertex",
      128, min_zvertex, max_zvertex );
  bbc_zvertex->GetXaxis()->SetTitle("BBC Raw ZVertex [cm]");
  bbc_zvertex->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_bbll1 = new TH1F("bbc_zvertex_bbll1",
      "BBC ZVertex triggered by BBLL1",
      zvtnbin, min_zvertex, max_zvertex );
  bbc_zvertex_bbll1->Sumw2();
  bbc_zvertex_bbll1->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_bbll1->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_bbll1->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_bbll1->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_zdc = new TH1F("bbc_zvertex_zdc",
      "BBC ZVertex triggered by ZDC",
      zvtnbin/2, min_zvertex, max_zvertex );
  bbc_zvertex_zdc->Sumw2();
  bbc_zvertex_zdc->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_zdc->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_zdc->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex_zdc->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex_zdc->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_zdc->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_zdc_scale3 = new TH1F("bbc_zvertex_zdc_scale3",
      "BBC ZVertex triggered by ZDC (scale 3 times up)",
      zvtnbin, min_zvertex, max_zvertex );
  bbc_zvertex_zdc_scale3->Sumw2();
  bbc_zvertex_zdc_scale3->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_zdc_scale3->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_zdc_scale3->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex_zdc_scale3->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex_zdc_scale3->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_zdc_scale3->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_bbll1_novtx = new TH1F("bbc_zvertex_bbll1_novtx",
      "BBC ZVertex triggered by BBLL1(noVtxCut)",
      zvtnbin/2, min_zvertex, max_zvertex );
  bbc_zvertex_bbll1_novtx->Sumw2();
  bbc_zvertex_bbll1_novtx->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_bbll1_novtx->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_bbll1_novtx->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1_novtx->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1_novtx->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_bbll1_novtx->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_bbll1_narrowvtx = new TH1F("bbc_zvertex_bbll1_narrowvtx",//RUN11 AuAu
      "BBC ZVertex triggered by BBLL1(narrowVtxCut)",
      zvtnbin, min_zvertex, max_zvertex );
  bbc_zvertex_bbll1_narrowvtx->Sumw2();
  bbc_zvertex_bbll1_narrowvtx->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_bbll1_narrowvtx->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_bbll1_narrowvtx->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1_narrowvtx->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1_narrowvtx->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_bbll1_narrowvtx->GetYaxis()->SetTitleOffset(1.75);

  bbc_zvertex_bbll1_zdc = new TH1F("bbc_zvertex_bbll1_zdc",
      "BBC ZVertex triggered by BBLL1&ZDCNS",
  //bbc_zvertex_bbll1_zdc = new TH1F("bbc_zvertex_bbll1_zdc",
  //    "BBC ZVertex triggered by BBLL1&ZDCLL1wide",
      zvtnbin, min_zvertex, max_zvertex );
  bbc_zvertex_bbll1_zdc->Sumw2();
  bbc_zvertex_bbll1_zdc->GetXaxis()->SetTitle("ZVertex [cm]");
  bbc_zvertex_bbll1_zdc->GetYaxis()->SetTitle("Number of Event");
  bbc_zvertex_bbll1_zdc->GetXaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1_zdc->GetYaxis()->SetTitleSize( 0.05);
  bbc_zvertex_bbll1_zdc->GetXaxis()->SetTitleOffset(0.70);
  bbc_zvertex_bbll1_zdc->GetYaxis()->SetTitleOffset(1.75);

  bbc_nevent_counter = new TH1F("bbc_nevent_counter",
      "The nEvent Counter bin1:Total Event bin2:Collision Event bin3:Laser Event",
      16, 0, 16 );

  //bbc_tzero_zvtx = new TH2F("bbc_tzero_zvtx",
  //    "TimeZero vs ZVertex", 100, -200, 200, 110, -11, 11 );
  bbc_tzero_zvtx = new TH2F("bbc_tzero_zvtx",
      "TimeZero vs ZVertex", 100, -200, 200, 110, -6, 16 );
  bbc_tzero_zvtx->SetXTitle("ZVertex[cm]");
  bbc_tzero_zvtx->SetYTitle("TimeZero[ns]");


  bbc_avr_hittime = new TH1F("bbc_avr_hittime",
      "BBC Average Hittime",
      128, 0, 24 );
  bbc_south_hittime = new TH1F("bbc_south_hittime",
			       "BBC South Hittime",
			       128, 0, 24 );
  bbc_north_hittime = new TH1F("bbc_north_hittime",
			       "BBC North Hittime",
			       128, 0, 24 );
  bbc_south_chargesum = new TH1F("bbc_south_chargesum",
      "BBC South ChargeSum [MIP]",
      128, 0, MAX_CHARGE_SUM );
  bbc_north_chargesum = new TH1F("bbc_north_chargesum",
      "BBC North ChargeSum [MIP]",
      128, 0, MAX_CHARGE_SUM );
  bbc_avr_hittime->Sumw2();
  bbc_avr_hittime->GetXaxis()->SetTitle("Avr HitTime [ns]");
  bbc_avr_hittime->GetYaxis()->SetTitle("Number of Event");
  bbc_avr_hittime->GetXaxis()->SetTitleSize( 0.05);
  bbc_avr_hittime->GetYaxis()->SetTitleSize( 0.05);
  bbc_avr_hittime->GetXaxis()->SetTitleOffset(0.70);
  bbc_avr_hittime->GetYaxis()->SetTitleOffset(1.75);

  bbc_south_hittime->GetXaxis()->SetTitle("South HitTime [ns]");
  bbc_south_hittime->GetYaxis()->SetTitle("Number of Event");
  bbc_south_hittime->GetXaxis()->SetTitleSize( 0.05);
  bbc_south_hittime->GetYaxis()->SetTitleSize( 0.05);
  bbc_south_hittime->GetXaxis()->SetTitleOffset(0.70);
  bbc_south_hittime->GetYaxis()->SetTitleOffset(1.75);

  bbc_north_hittime->GetXaxis()->SetTitle("North HitTime [ns]");
  bbc_north_hittime->GetYaxis()->SetTitle("Number of Event");
  bbc_north_hittime->GetXaxis()->SetTitleSize( 0.05);
  bbc_north_hittime->GetYaxis()->SetTitleSize( 0.05);
  bbc_north_hittime->GetXaxis()->SetTitleOffset(0.70);
  bbc_north_hittime->GetYaxis()->SetTitleOffset(1.75);

  bbc_north_chargesum->SetTitle("BBC ChargeSum [MIP]");
  bbc_north_chargesum->GetXaxis()->SetTitle("ChargeSum [MIP]");
  //bbc_north_chargesum->GetXaxis()->SetTitle("North ChargeSum [MIP]");
  bbc_north_chargesum->GetYaxis()->SetTitle("Number of Event");
  bbc_north_chargesum->GetXaxis()->SetTitleSize( 0.05);
  bbc_north_chargesum->GetYaxis()->SetTitleSize( 0.05);
  bbc_north_chargesum->GetXaxis()->SetTitleOffset(0.70);
  bbc_north_chargesum->GetYaxis()->SetTitleOffset(1.75);

  //bbc_south_chargesum->GetXaxis()->SetTitle("South ChargeSum [MIP]");
  bbc_south_chargesum->GetYaxis()->SetTitle("Number of Event");
  bbc_south_chargesum->GetXaxis()->SetTitleSize( 0.05);
  bbc_south_chargesum->GetYaxis()->SetTitleSize( 0.05);
  bbc_south_chargesum->GetXaxis()->SetTitleOffset(0.70);
  bbc_south_chargesum->GetYaxis()->SetTitleOffset(1.75);

  // scale down factor for each trigger
  bbc_prescale_hist = new TH1F("bbc_prescale_hist", "", 100, 0, 100);
  // ----------------------------------------------------------------------------------------------------
  // Regist Histograms

  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
  {
    registHist( Onlmonserver, bbc_tdc[tdc] );
    registHist( Onlmonserver, bbc_tdc_overflow[tdc] );
    for ( int i = 0 ; i < nPMT_BBC ; i++ )
    {
      registHist( Onlmonserver, bbc_tdc_overflow_each[tdc][i] );
    }
  }
  registHist( Onlmonserver, bbc_adc );

  for ( int trig = 0 ; trig < nTRIGGER ; trig++ )
  {
    registHist( Onlmonserver, bbc_nhit[trig] );
  }
  
  registHist( Onlmonserver, bbc_tdc_armhittime );
  registHist( Onlmonserver, bbc_zvertex );
  registHist( Onlmonserver, bbc_zvertex_bbll1 );
  registHist( Onlmonserver, bbc_zvertex_zdc );
  registHist( Onlmonserver, bbc_zvertex_zdc_scale3 );
  registHist( Onlmonserver, bbc_zvertex_bbll1_novtx );
  registHist( Onlmonserver, bbc_zvertex_bbll1_narrowvtx );
  registHist( Onlmonserver, bbc_zvertex_bbll1_zdc );
  registHist( Onlmonserver, bbc_nevent_counter );
  registHist( Onlmonserver, bbc_tzero_zvtx );
  registHist( Onlmonserver, bbc_prescale_hist );
  registHist( Onlmonserver, bbc_avr_hittime );
  registHist( Onlmonserver, bbc_north_hittime );
  registHist( Onlmonserver, bbc_south_hittime );
  registHist( Onlmonserver, bbc_north_chargesum );
  registHist( Onlmonserver, bbc_south_chargesum );


  // BBC Calib
  bbccalib = new BbcCalib();
  bevt = new BbcEvent();

  // initBbcHV();

  zdccalib = new ZdcCalib();
  zevt = new ZdcEvent();

  // Initialization of trigger bit definition.
  numbbll1 = 0;
  memset(bbll1_trig, 0, sizeof(bbll1_trig));
  memset(bbll1_scale_factor_buf, 0 , sizeof(bbll1_scale_factor_buf)); // floats can only be set to zero this way, do not try other values

  zdc_trig = 0;
  bbll1_novtx_trig = 0;
  bbll1_narrowvtx_trig = 0;//RUN11 pp
  laser_trig = 0;

  bbll1_scale_factor = 0.0;
  zdc_scale_factor = 0.0;
  bbll1_novtx_scale_factor = 0.0;
  bbll1_narrowvtx_scale_factor = 0.0;//RUN11 pp

  bbll1_ok = false;
  zdc_ok = false;
  bbll1_novtx_ok = false;
  bbll1_narrowvtx_ok = false;//RUN11 pp
  laser_ok = false;

  // time value
  bbc_start_time = 0;
  dbvars = new OnlMonDB(ThisName);
  DBVarInit();
  
}


BBCMon::~BBCMon()
{
  // deleting NULL pointers is allowed according to C++ standard, 
  // it will result in a nop (no operation), so checking for not NULL
  // is not needed
  delete bbccalib;
  delete bevt;
  delete zdccalib;
  delete zevt;
  delete dbvars;
  return ;
}

int
BBCMon::process_event(Event *evt)
{
  if (evt->getEvtType() != DATAEVENT)
    {
      return 0;
    }
  evtcnt++;
  ostringstream msg;
  Packet *p = evt->getPacket(BBC_PACKET_ID);
  Packet *np = evt->getPacket(BBCN_PACKET_ID);
  Packet *sp = evt->getPacket(BBCS_PACKET_ID);
  if ( !p && !np && !sp)
    {
      OnlMonServer *se = OnlMonServer::instance();
      msg << "Bbc packet not found" ;
      se->send_message(this,MSG_SOURCE_BBC,MSG_SEV_WARNING, msg.str(),1);
      msg.str("");
      return 0;
    }

  // calculate BBC
  bevt->Clear();
  bevt->setRawData(evt);
  bevt->calculate();

  // calculate ZDC
  zevt->Clear();
  zevt->setRawData(evt);
  zevt->calculate();


  if (evtcnt % 1000 == 0)
  //if (evtcnt % 10 == 0)//for test
    {
      printf("BBCMon:Processing Event %u\n",evtcnt);
    }

  // get scaled trigger
    OnlMonServer *se = OnlMonServer::instance();
	  // You should use scaledtrig for Physics Run
    unsigned long scaledtrig = se->Trigger(2); // get scaled trigger from server
		// 
    //unsigned long scaledtrig = se->Trigger(1); // get live trigger from server

  //===========================================================
  // Readout mode selection
  // All BBC data were taken by single DCM channel before Run4.
  // double DCM channel readout are implemented from Run5.
  //===========================================================

  if ((p) != 0 || (np) != 0 || (sp) != 0)
    {
      // fill trigger information
      read_trigger_decision(scaledtrig);
      if ( ! laser_ok )
        {
          for (int i = 0; i < num_pmt; i++)
            {

              if ((p) != 0)
                {
                  // Attention!! only p pointer contain north at the first
                  // iValue(i,xx)  i:0-63 North / 64-127 South
                  int t0 = p->iValue(i ^ 64, "T1"); // T1 = TDC0
                  int t1 = p->iValue(i ^ 64, "T2"); // T2 = TDC1
                  int a = bevt->getAdc(i); // ADC
                  float t0s = bbccalib->getHitTime0( i, (int)t0, (int)a ); // *TDC_CONVERSION_FACTOR
                  float t1s = bbccalib->getHitTime1( i, (int)t1, (int)a );
                  
		  //if( bbll1_ok){                 // 2014.02.22 AuAu 15GeV
		    bbc_tdc [0]->Fill( i, t0s ); //
		  //}                              //
		  
                  bbc_tdc_overflow[0]->Fill( i, t0 - bbccalib->getOverflow0()
                                             ->getCalibPar(i)->getPeakChannel() );
		  
		  //if( bbll1_ok){                 // 2014.02.22 AuAu 15GeV
		    bbc_tdc [1]->Fill( i, t1s ); // 
		  //}                              //
		  
                  bbc_tdc_overflow[1]->Fill( i, t1 - bbccalib->getOverflow1()
                                             ->getCalibPar(i)->getPeakChannel() );
                  bbc_tdc_overflow_each[0][i]->Fill( t0 - bbccalib->getOverflow0()->getCalibPar(i)->getPeakChannel() );
                  bbc_tdc_overflow_each[1][i]->Fill( t1 - bbccalib->getOverflow1()->getCalibPar(i)->getPeakChannel() );

                  if ( bevt->isHit(i) == 1 )
                    {
                      if ( bbll1_ok )
                        {
                          bbc_nhit[0]->Fill( i );
                        }
                      bbc_adc->Fill( i, bevt->getCharge(i) );
                    }
                }
              else if ((np) != 0 && (sp) != 0)
                {
                  int t0, t1;
                  if (i > 63)
                    {
                      t0 = np->iValue(i - 64, "T1");
                      t1 = np->iValue(i - 64, "T2");
                    }
                  else
                    {
                      t0 = sp->iValue(i, "T1");
                      t1 = sp->iValue(i, "T2");
                    }
                  int a = bevt->getAdc(i); // ADC
                  float t0s = bbccalib->getHitTime0( i, (int)t0, (int)a ); // *TDC_CONVERSION_FACTOR
                  float t1s = bbccalib->getHitTime1( i, (int)t1, (int)a );

		  //if( bbll1_ok){                 // 2014.02.22 AuAu 15GeV
		    bbc_tdc [0]->Fill( i, t0s ); //
		  //}                              //
		  
                  bbc_tdc_overflow[0]->Fill( i, t0 - bbccalib->getOverflow0()
                                             ->getCalibPar(i)->getPeakChannel() );
                  
		  //if( bbll1_ok){       // 2014.02.22 AuAu 15GeV
		    bbc_tdc [1]->Fill( i, t1s ); //
		  //}                              //
		    
                  bbc_tdc_overflow[1]->Fill( i, t1 - bbccalib->getOverflow1()
                                             ->getCalibPar(i)->getPeakChannel() );
                  bbc_tdc_overflow_each[0][i]->Fill( t0 - bbccalib->getOverflow0()->getCalibPar(i)->getPeakChannel() );
                  bbc_tdc_overflow_each[1][i]->Fill( t1 - bbccalib->getOverflow1()->getCalibPar(i)->getPeakChannel() );

                  if ( bevt->isHit(i) == 1 )
                    {
                      if ( bbll1_ok )
                        {
                          bbc_nhit[0]->Fill( i );
                        }
                      bbc_adc->Fill( i, bevt->getCharge(i) );
                    }
                }
              else
                {
                  OnlMonServer *se = OnlMonServer::instance();
                  msg << "wrong BBC paket" ;
                  se->send_message(this,MSG_SOURCE_BBC,MSG_SEV_ERROR, msg.str(),2);
                  msg.str("");
                  return 0;
                }
            }
        }
      if (p)
        {
          delete p;
        }
      if (sp)
        {
          delete sp;
        }
      if (np)
        {
          delete np;
        }
      // treatment of the laser event
      if ( laser_ok )
        {
          for (int i = 0; i < num_pmt; i++)
            {
              if ( bevt->isHit(i) == 1 )
                {
									bbc_nhit[1]->Fill( i );
								}
            }
        }

      // Event Count
      bbc_nevent_counter->Fill( 0 );
      if ( bbll1_ok )
        {
          bbc_nevent_counter->Fill( 1 );
        }
      else if ( laser_ok )
        {
          bbc_nevent_counter->Fill( 2 );
        }

      if ( laser_ok )
        {
          // cout << "Laser Event" << endl;
        }
      // BBC arm hit time correlation
      if ( ! laser_ok )
        {
          bbc_tdc_armhittime->Fill( bevt->getArmHitTime(Bbc::South),
                                    bevt->getArmHitTime(Bbc::North) );

          bbc_zvertex->Fill( bevt->getZVertex() );
          bbc_south_chargesum->Fill( bevt->getChargeSum(Bbc::South) );
          bbc_north_chargesum->Fill( bevt->getChargeSum(Bbc::North) );

          if ( fabs(bevt->getZVertex()) < 130 )
            {
              // Average Arm Hit Time in order to estimate how much we should shift V124
              bbc_avr_hittime->Fill( (bevt->getArmHitTime(Bbc::South) + bevt->getArmHitTime(Bbc::North)) / 2.0 );
              bbc_south_hittime->Fill( bevt->getArmHitTime(Bbc::South) );
              bbc_north_hittime->Fill( bevt->getArmHitTime(Bbc::North) );
            }

          if (bbll1_ok)
            {
              bbc_zvertex_bbll1->Fill( bevt->getZVertex(), bbll1_scale_factor );
	      //              bbc_zvertex_bbll1->Fill( bevt->getZVertex());
            }
          if (zdc_ok)
            {
               bbc_zvertex_zdc->Fill( zevt->getZvertex(), zdc_scale_factor );
               //bbc_zvertex_zdc->Fill( zevt->getZvertex() );
               //bbc_zvertex_zdc->Fill( zevt->getZvertex(), zdc_scale_adjust);
            }
            
              //--------------------------------------------------------------------------------------------
              //--(1)The old discription--------------------------------------------------------------------
              // You may need to adjust zvertex distribution triggerd by ZDCNS or ZDCLL1wide
              // by changing ZDC_BBC_TRIGSCALE value.
              // in order to  make it easy to compare zvertex by ZDCNS or ZDCLL1wide with one by BBLL1.
              //--------------------------------------------------------------------------------------------
              //line 655 :: bbc_zdc_scale_adjust = zdc_scale_factor*ZDC_BBC_TRIGSCALE; // old discription
              //bbc_zvertex_zdc->Fill( zevt->getZvertex(), bbc_zdc_scale_adjust);
              // for Au+Au : ZDC_BBC_TRIGSCALE ==  1 in BbcMonDefs.h
              // for d+Au  : ZDC_BBC_TRIGSCALE ==  3 in BbcMonDefs.h
              // for Cu+Cu : ZDC_BBC_TRIGSCALE == 10 in BbcMonDefs.h
              // for p+p   : ZDC_BBC_TRIGSCALE == 50 in BbcMonDefs.h
              //
              //--(2)The old discription--------------------------------------------------------------------
              // for AuAu
              //bbc_zvertex_zdc->Fill( zevt->getZvertex(), zdc_scale_factor);
              // for AuAu
              //bbc_zvertex_zdc_scale3->Fill( zevt->getZvertex(), zdc_scale_factor);
              // for d+Au
              //bbc_zvertex_zdc_scale3->Fill( zevt->getZvertex(), zdc_scale_factor*3.0);
              // for p+p
              // bbc_zvertex_zdc_scale3->Fill( zevt->getZvertex(), zdc_scale_factor*50.0);
              // for test
              //bbc_zvertex_zdc->Fill( bevt->getZVertex(), zdc_scale_factor);
              //bbc_zvertex_zdc_scale3->Fill( bevt->getZVertex(), zdc_scale_factor*3.0);
              //-------------------------------------------------------------------------------------------

	  // if the narrow vtx trigger is enabled, the following can be set up as all 
	  // other triggers, these constructs only enable the plotting of the
	  // narrow vtx if it was scaled away
	  if (bbll1_narrowvtx_ok || ((se->Trigger(1) & bbll1_narrowvtx_trig) && bbll1_narrowvtx_scale_factor > 1e5) )
	  {
	      if (bbll1_narrowvtx_scale_factor > 1e5)
		    {
		       bbc_zvertex_bbll1_narrowvtx->Fill( bevt->getZVertex(), bbll1_scale_factor );
		    }
	      else
		    {
		       bbc_zvertex_bbll1_narrowvtx->Fill( bevt->getZVertex(), bbll1_narrowvtx_scale_factor );
		    }
	      //bbc_zvertex_bbll1_narrowvtx->Fill( bevt->getZVertex());
	  }
	  if (bbll1_novtx_ok)
            {
              bbc_zvertex_bbll1_novtx->Fill( bevt->getZVertex(), bbll1_novtx_scale_factor/2 );
              //bbc_zvertex_bbll1_novtx->Fill( bevt->getZVertex() );
            }
          if (bbll1_ok && zdc_ok)
            {
              bbc_zvertex_bbll1_zdc->Fill( bevt->getZVertex(), bbll1_scale_factor*zdc_scale_factor/2 );
            }
          // bbc_zvertex_zdc->Fill( bevt->getZVertex(),
          // zevt->getZvertex() );

          bbc_tzero_zvtx->Fill(bevt->getZVertex(), bevt->getTimeZero());
        }

      //    delete p;
    }

  return 0;
}

void
BBCMon::setup_runinfo(int runnumber)
{
  OnlMonServer *se = OnlMonServer::instance();
  if (runnumber > 0)
    {
      // BBLL1
      numbbll1 = 0;

      printf("\n === Trigger Definition  === \n");
      printf("      BBLL1 :\n");
      for (int i = 0; i < 32; i++)
        {
          int hexvalue = 1;
          int hexvalue_for_bit = 1;
          hexvalue = hexvalue << i;
          string trigname = se->OnlTrig()->get_lvl1_trig_name(i);
          if (trigname == "BBCLL1(>0 tubes)" || trigname == "BBCLL1(>0_tubes)" )
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue, se->OnlTrig()->get_lvl1_trig_scale_down(i), se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //bbll1_trig[numbbll1] = hexvalue;
              bbll1_trig[numbbll1] = hexvalue_for_bit;
              bbll1_scale_factor_buf[numbbll1] = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
              numbbll1++;
            }
          else if (trigname == "BBCLL1(>1 tubes)")
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue, se->OnlTrig()->get_lvl1_trig_scale_down(i), se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //bbll1_trig[numbbll1] = hexvalue;
              bbll1_trig[numbbll1] = hexvalue_for_bit;
              bbll1_scale_factor_buf[numbbll1] = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
              numbbll1++;
            }
          else if (trigname == "BBCLL1>=1")
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue,
                     se->OnlTrig()->get_lvl1_trig_scale_down(i),
                     se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //bbll1_trig[numbbll1] = hexvalue;
              bbll1_trig[numbbll1] = hexvalue_for_bit;
              bbll1_scale_factor_buf[numbbll1] = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
              numbbll1++;
            }
          //else if (trigname == "ZDCNS")
          else if (trigname == "ZDCLL1wide")
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue,
                     se->OnlTrig()->get_lvl1_trig_scale_down(i),
                     se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //zdc_trig = hexvalue;
              zdc_trig = hexvalue_for_bit;
              zdc_scale_factor = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
              //bbc_zdc_scale_adjust = zdc_scale_factor*ZDC_BBC_TRIGSCALE; // old discription
            }
          // BBLL1(noVertexCut)
          else if (trigname == "BBCLL1(>1 tubes) novertex")
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue,
                     se->OnlTrig()->get_lvl1_trig_scale_down(i),
                     se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //bbll1_novtx_trig = hexvalue;
              bbll1_novtx_trig = hexvalue_for_bit;
              bbll1_novtx_scale_factor = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
            }
          else if (trigname == "BBCLL1(>0 tubes) novertex" || trigname == "BBCLL1(>0_tubes)_novertex"  )
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue,
                     se->OnlTrig()->get_lvl1_trig_scale_down(i),
                     se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //bbll1_novtx_trig = hexvalue;
              bbll1_novtx_trig = hexvalue_for_bit;
              bbll1_novtx_scale_factor = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
            }
          //BBLL1(narrowVertexCut)//RUN11 pp
          else if (trigname == "BBCLL1(>0 tubes) narrowvtx" || trigname == "BBCLL1(>1 tubes) narrowvtx" || trigname == "BBCLL1(>0_tubes)_narrowvtx")
            {
             hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue,
                     se->OnlTrig()->get_lvl1_trig_scale_down(i),
                     se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              bbll1_narrowvtx_trig = hexvalue_for_bit;
              bbll1_narrowvtx_scale_factor = se->OnlTrig()->get_lvl1_trig_scale_down(i) + 1.0;
           }
          // Laser
          else if (trigname == "PPG(Laser)")
            {
              hexvalue_for_bit = hexvalue_for_bit << se->OnlTrig()->get_lvl1_trig_bit(i);
              printf("        %s: Index = %d 0x%08x %d , Bit No. = %d 0x%08x\n",
                     se->OnlTrig()->get_lvl1_trig_name(i).c_str(), i, hexvalue,
                     se->OnlTrig()->get_lvl1_trig_scale_down(i),
                     se->OnlTrig()->get_lvl1_trig_bit(i), hexvalue_for_bit);
              //laser_trig = hexvalue;
              laser_trig = hexvalue_for_bit;
              //laser_scale_factor = se->OnlTrig()->get_lvl1_trig_scale_down(i)+1.0;
            }
        }

      // Overwrite temporary
      // We can not get correct trigger bit by trigger name
      // Therefore it should be hardwired as 0x80000000
      // bbll1_novtx_trig         = 0x80000000;
      // bbll1_novtx_scale_factor = se->OnlTrig()->get_lvl1_trig_scale_down(31)+1.0;
      // cout <<"bbll1 novtx scale factor "<< bbll1_novtx_scale_factor << endl;

      // save prescale factor
      bbc_prescale_hist->SetBinContent(1, bbll1_scale_factor_buf[0]);
      bbc_prescale_hist->SetBinContent(2, zdc_scale_factor);
      bbc_prescale_hist->SetBinContent(3, bbll1_novtx_scale_factor);
      bbc_prescale_hist->SetBinContent(4, bbll1_narrowvtx_scale_factor);//RUN11 pp
      //     bbc_prescale_hist->Fill(0., bbll1_scale_factor_buf[0]);
      //     bbc_prescale_hist->Fill(1., zdc_scale_factor);
//     bbc_prescale_hist->Fill(2., bbll1_novtx_scale_factor);
    }
  return ;
}

void BBCMon::setup_calibration(const int runnumber)
{
  if (runnumber > 0)
  {
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
        OnlMonServer *se = OnlMonServer::instance();
        ostringstream msg;
	msg << "Environment variable BBCCALIBDIR not set";
        se->send_message(this,MSG_SOURCE_BBC,MSG_SEV_ERROR, msg.str(),2);
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
      OnlMonServer *se = OnlMonServer::instance();
  ostringstream msg;
      msg << "BBCCALIBRATION not set, Calibration of BBC is not loaded correctly";
      se->send_message(this,MSG_SOURCE_BBC,MSG_SEV_ERROR, msg.str(),2);
    }
    bevt->setCalibDataAll(bbccalib);

    // This is for new (temp) ZDC constants as same as ZdcMon.C, 2004.04.23 by Yuji Tsuchimoto
    ostringstream zdccalibdir;
    if (getenv("ZDCCALIBDIR"))
    {
      zdccalibdir << getenv("ZDCCALIBDIR");
    }
    zdccalibdir << "/ZdcCalib" ;
    zdccalib->restore(zdccalibdir.str().c_str());
    PHTimeStamp now;
    now.setToSystemTime();
    zevt->setCalibDataAll(zdccalib,now);
  }
}

void BBCMon::read_trigger_decision(const unsigned long scaledtrig)
{
  // BBLL1
  bbll1_ok = false;
  for (int i = 0; i < numbbll1; i++)
  {
    if ( (bbll1_trig[i]&scaledtrig) == bbll1_trig[i] && bbll1_trig[i] != 0)
    {
      bbll1_ok = true;
      bbll1_scale_factor = bbll1_scale_factor_buf[i];
      //cout << "BLLL1  " << i << " " << bbll1_trig[i] <<" " << bbll1_scale_factor << endl;
    }
  }


  // ZDC
  zdc_ok = false;
  if ((zdc_trig&scaledtrig) == zdc_trig && zdc_trig != 0)
  {
    zdc_ok = true;
  }
  // BBLL1(noVertexCut)
  bbll1_novtx_ok = false;
  if ((bbll1_novtx_trig&scaledtrig) == bbll1_novtx_trig && bbll1_novtx_trig != 0 )
  {
    bbll1_novtx_ok = true;
  }

  //BBLL1(narrowVertexCut)//RUN11 pp
  bbll1_narrowvtx_ok = false;
  if ((bbll1_narrowvtx_trig&scaledtrig) == bbll1_narrowvtx_trig && bbll1_narrowvtx_trig != 0 )
  {
    bbll1_narrowvtx_ok = true;
       //cout << "BLLL1(narrow)  " << " " << bbll1_narrowvtx_trig <<" " << endl;
  }

  // Laser
  laser_ok = false;
  if ((laser_trig&scaledtrig) == laser_trig && laser_trig != 0)
  {
    laser_ok = true;
  }
  //cout<<"BBLL1:ZDC:BBLL1(NoVtx) : "<<(int)bbll1_ok<<":"<<(int)zdc_ok<<":"<<(int)bbll1_novtx_ok<<"  0x"<<hex<<scaledtrig<<dec<<endl;
  return ;
}

int BBCMon::BeginRun(const int runno)
{
  // setup run information
  setup_runinfo(runno);
  setup_calibration(runno);
  // trigger selection now done in the macro
  return 0;
}


int
BBCMon::DBVarInit()
{
  dbvars->registerVar("aver_hittime");
  dbvars->DBInit();
  return 0;
}
  
int
BBCMon::EndRun(const int runno)
{
  if (bbc_avr_hittime->GetEntries() > 1000)
    {
      dbvars->SetVar("aver_hittime",
                     bbc_avr_hittime->GetMean(),
                     bbc_avr_hittime->GetRMS(),
                     bbc_avr_hittime->GetEntries());
      dbvars->DBcommit();
    }
  return 0;
}

int
BBCMon::Reset()
{
  evtcnt = 0;
  return 0;
}
