#include <RICHMon.h>

#include <GetRichPmtData.h>
#include <CalibrationsRead.h>
#include <TacT0Read.h>
#include <RichGeom.h>

#include <OnlMonServer.h>
#include <OnlMonStatus.h>
#include <OnlMonTrigger.h>

#include <Event.h>

#include <msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>

#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


using namespace std;

#undef DECODE_TEXTFILE
#undef DECODE_ROOTFILE

// Use printf for screen output, cout is caught by the message system
// Use the message system for "important" messages, 
// not for the  "Processing event XXX" 

RICHMon::RICHMon(): OnlMon("RICHMON")
{
  
  ostringstream msg;

  evtcnt = 0;
  chanperpacket = 160;  // The number of PMT's per packet
  packetsperevent = 32; // The number of packets per event

  rich_eventcounter = new TH1F("rich_eventcounter","rich_eventcounter",1,0.5,1.5);

  rich_adclivehist = new TH1F("rich_adclivehist",
                                    "N of hit of each RICH PMT on ADC",
                                    RICH_NCHANNEL, 0, RICH_NCHANNEL);
  rich_tdclivehist = new TH1F("rich_tdclivehist",
                                    "N of hit of each RICH PMT on TDC",
                                    RICH_NCHANNEL, 0, RICH_NCHANNEL);
  rich_adchistsumWS = new TH1F("rich_adchistsumWS",
                                     "W.S.",
                                     220, -1, 10);
  rich_adchistsumWN = new TH1F("rich_adchistsumWN",
                                     "W.N.",
                                     220, -1, 10);
  //				     1050,-150,900);
  rich_adchistsumES = new TH1F("rich_adchistsumES",
                                     "E.S.",
                                     220, -1, 10);
  //				     1050,-150,900);
  rich_adchistsumEN = new TH1F("rich_adchistsumEN",
                                     "E.N.",
                                     220, -1, 10);
  //				     1050,-150,900);


  rich_adc_first = new TH1F("rich_adc_first", "RICH PMT ID vs ADC first",
                                  5120, 0, 5120);
  rich_adc_second = new TH1F("rich_adc_second", "RICH PMT ID vs ADC 2",
                                   5120, 0, 5120);
  rich_adc_hit = new TH1F("rich_adc_hit", "RICH PMT ID vs ADC hit",
                                5120, 0, 5120);
  rich_adc_pmt = new TH1F("rich_adc_pmt", "RICH ADC vs PMT ID ",
                                5120, 0, 5120);

  // these are used as ring buffers
  rich_nhit_eventWS = new TH1F("rich_nhit_eventWS", "# of hits in 10,000 events, WS",
			       1000, 0, 1000);
  rich_nhit_eventWN = new TH1F("rich_nhit_eventWN", "# of hits in 10,000 events, WN",
			       1000, 0, 1000);
  rich_nhit_eventES = new TH1F("rich_nhit_eventES", "# of hits in 10,000 events, ES",
			       1000, 0, 1000);
  rich_nhit_eventEN = new TH1F("rich_nhit_eventEN", "# of hits in 10,000 events, EN",
			       1000, 0, 1000);

  // These are for detecting pre-amp power problem
  rich_nhit_eventWS1 = new TH1F("rich_nhit_eventWS1", "# of hits in 10,000 events, WS part1",
			        1000, 0, 1000);
  rich_nhit_eventWS2 = new TH1F("rich_nhit_eventWS2", "# of hits in 10,000 events, WS part2",
			        1000, 0, 1000);
  rich_nhit_eventWS3 = new TH1F("rich_nhit_eventWS3", "# of hits in 10,000 events, WS part3",
			        1000, 0, 1000);
  rich_nhit_eventWS4 = new TH1F("rich_nhit_eventWS4", "# of hits in 10,000 events, WS part4",
			        1000, 0, 1000);

  rich_nhit_eventWN1 = new TH1F("rich_nhit_eventWN1", "# of hits in 10,000 events, WN part1",
			        1000, 0, 1000);
  rich_nhit_eventWN2 = new TH1F("rich_nhit_eventWN2", "# of hits in 10,000 events, WN part2",
			        1000, 0, 1000);
  rich_nhit_eventWN3 = new TH1F("rich_nhit_eventWN3", "# of hits in 10,000 events, WN part3",
			        1000, 0, 1000);
  rich_nhit_eventWN4 = new TH1F("rich_nhit_eventWN4", "# of hits in 10,000 events, WN part4",
			        1000, 0, 1000);

  rich_nhit_eventES1 = new TH1F("rich_nhit_eventES1", "# of hits in 10,000 events, ES part1",
			        1000, 0, 1000);
  rich_nhit_eventES2 = new TH1F("rich_nhit_eventES2", "# of hits in 10,000 events, ES part2",
			        1000, 0, 1000);
  rich_nhit_eventES3 = new TH1F("rich_nhit_eventES3", "# of hits in 10,000 events, ES part3",
			        1000, 0, 1000);
  rich_nhit_eventES4 = new TH1F("rich_nhit_eventES4", "# of hits in 10,000 events, ES part4",
			        1000, 0, 1000);

  rich_nhit_eventEN1 = new TH1F("rich_nhit_eventEN1", "# of hits in 10,000 events, EN part1",
 			        1000, 0, 1000);
  rich_nhit_eventEN2 = new TH1F("rich_nhit_eventEN2", "# of hits in 10,000 events, EN part2",
 			        1000, 0, 1000);
  rich_nhit_eventEN3 = new TH1F("rich_nhit_eventEN3", "# of hits in 10,000 events, EN part3",
 			        1000, 0, 1000);
  rich_nhit_eventEN4 = new TH1F("rich_nhit_eventEN4", "# of hits in 10,000 events, EN part4",
 			        1000, 0, 1000);

  rich_deadchinlivehist = new TH1F("rich_deadchinlivehist",
                                         "Dead ch to be drawn in livehistos",
                                         RICH_NCHANNEL, 0, RICH_NCHANNEL);
  rich_par = new TH1F("rich_par", "parameter lists", 100, 0, 100);

  /*
    0: N of evt processed
    1: N of packet format error
    2: N of several "ffff" pattern appered
    3: N of so much "ffff" pattern appered
    4: N of too much "ffff" pattern appered
    5: packet format
    6: Run #
  */

  rich_ringhist[0] = new TH1F("rich_ringhistWS", "npe per ring for WS", 150, 0, 30);
  rich_ringhist[1] = new TH1F("rich_ringhistWN", "npe per ring for WN", 150, 0, 30);
  rich_ringhist[2] = new TH1F("rich_ringhistES", "npe per ring for ES", 150, 0, 30);
  rich_ringhist[3] = new TH1F("rich_ringhistEN", "npe per ring for EN", 150, 0, 30);

  rich_deadchinlivehist->SetFillColor(3);
  rich_deadchinlivehist->SetLineColor(3);


  rich_tdchistsumWS = new TH1F("rich_tdchistsumWS",
                                     "W.S.",
                                     512, -600, 424);
  rich_tdchistsumWN = new TH1F("rich_tdchistsumWN",
                                     "W.N.",
                                     512, -600, 424);
  rich_tdchistsumES = new TH1F("rich_tdchistsumES",
                                     "E.S.",
                                     512, -600, 424);
  rich_tdchistsumEN = new TH1F("rich_tdchistsumEN",
                                     "E.N.",
                                     512, -600, 424);

  rich_tdc_first = new TH1F("rich_tdc_first", "RICH PMT ID vs TDC first",
                                  5120, 0, 5120);
  rich_tdc_second = new TH1F("rich_tdc_second", "RICH PMT ID vs TDC 2",
                                   5120, 0, 5120);
  rich_tdc_hit = new TH1F("rich_tdc_hit", "RICH PMT ID vs hit",
                                5120, 0, 5120);
  rich_tdc_pmt = new TH1F("rich_tdc_pmt", "RICH TDC vs PMT ID ",
                                5120, 0, 5120);



  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(this, rich_eventcounter);

  Onlmonserver->registerHisto(this, rich_adclivehist);
  Onlmonserver->registerHisto(this, rich_tdclivehist);

  Onlmonserver->registerHisto(this, rich_adchistsumWS);
  Onlmonserver->registerHisto(this, rich_adchistsumWN);
  Onlmonserver->registerHisto(this, rich_adchistsumES);
  Onlmonserver->registerHisto(this, rich_adchistsumEN);

  Onlmonserver->registerHisto(this, rich_adc_first);
  Onlmonserver->registerHisto(this, rich_adc_second);
  Onlmonserver->registerHisto(this, rich_adc_hit);
  Onlmonserver->registerHisto(this, rich_adc_pmt);
  Onlmonserver->registerHisto(this, rich_nhit_eventWS);
  Onlmonserver->registerHisto(this, rich_nhit_eventWN);
  Onlmonserver->registerHisto(this, rich_nhit_eventES);
  Onlmonserver->registerHisto(this, rich_nhit_eventEN);

  Onlmonserver->registerHisto(this, rich_nhit_eventWS1);
  Onlmonserver->registerHisto(this, rich_nhit_eventWS2);
  Onlmonserver->registerHisto(this, rich_nhit_eventWS3);
  Onlmonserver->registerHisto(this, rich_nhit_eventWS4);
  Onlmonserver->registerHisto(this, rich_nhit_eventWN1);
  Onlmonserver->registerHisto(this, rich_nhit_eventWN2);
  Onlmonserver->registerHisto(this, rich_nhit_eventWN3);
  Onlmonserver->registerHisto(this, rich_nhit_eventWN4);
  Onlmonserver->registerHisto(this, rich_nhit_eventES1);
  Onlmonserver->registerHisto(this, rich_nhit_eventES2);
  Onlmonserver->registerHisto(this, rich_nhit_eventES3);
  Onlmonserver->registerHisto(this, rich_nhit_eventES4);
  Onlmonserver->registerHisto(this, rich_nhit_eventEN1);
  Onlmonserver->registerHisto(this, rich_nhit_eventEN2);
  Onlmonserver->registerHisto(this, rich_nhit_eventEN3);
  Onlmonserver->registerHisto(this, rich_nhit_eventEN4);

  Onlmonserver->registerHisto(this, rich_deadchinlivehist);
  Onlmonserver->registerHisto(this, rich_par);

  Onlmonserver->registerHisto(this, rich_ringhist[0]);
  Onlmonserver->registerHisto(this, rich_ringhist[1]);
  Onlmonserver->registerHisto(this, rich_ringhist[2]);
  Onlmonserver->registerHisto(this, rich_ringhist[3]);

  Onlmonserver->registerHisto(this, rich_tdchistsumWS);
  Onlmonserver->registerHisto(this, rich_tdchistsumWN);
  Onlmonserver->registerHisto(this, rich_tdchistsumES);
  Onlmonserver->registerHisto(this, rich_tdchistsumEN);


  Onlmonserver->registerHisto(this, rich_tdc_first);
  Onlmonserver->registerHisto(this, rich_tdc_second);
  Onlmonserver->registerHisto(this, rich_tdc_hit);
  Onlmonserver->registerHisto(this, rich_tdc_pmt);

  calfile = RICH_ADC_CONST_FILE;
  taccalfile = RICH_TAC_CONST_FILE;

  tact0file =  RICH_TAC_T0_CONST_FILE;

  calread = new CalibrationsRead();
  calread_status =
    calread->getcalibs(calfile, pedestal, sigped, pep, sigpep,
                       taccalfile, tacpedestal, tacsigped, tacpep, tacsigpep,
                       dead, tacdead);

  tact0read = new TacT0Read();
  tact0read_status = tact0read->gett0(tact0file, tact0);
  return;
}

RICHMon::~RICHMon()
{
  delete calread;
  delete tact0read;
  return;
}

int RICHMon::process_event(Event *evt)
{
  
  evtcnt++;

  rich_eventcounter->Fill(1.0);

  rich_par->Fill(0);

  Packet *p = 0;
  int Format = 1006;
  int fio = 0;
  for (int it = 0;it < 4;it++)
    {
      if (fio == 0)
        {
          for (int io = 0;io < 8;io++)
            {
              p = evt->getPacket(6001 + io + it * 10);
              if (p)
                {
                  Format = p->getHitFormat();
                  fio = 1;
                  delete p;
                  break;
                }
            }
        }
    }

  rich_par->SetBinContent(6, Format);
  rich_par->SetBinContent(7, evt->getRunNumber());


  memset(hitmat,0,sizeof(hitmat));

  int status;
  int print_hits = 0;
  GetRichPmtData *get_data =
    new GetRichPmtData(evt, &status, pedestal, sigped, pep, sigpep, print_hits);


  int ipmt = 0;
  // this one is used only to make sure the vertical line goes up to
  // the histogram border
  double invalid_max = rich_adclivehist->GetMaximum() * 2;
  // # of hits at the events for each sector
  int nhit_sector[4] = {0,0,0,0};
  int nhit_secpar[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
  while (ipmt < packetsperevent*chanperpacket)
    {
      // The call to get_data->get_pmt_data() returns the PMT number, ADC
      // value, TAC value, hit status (yes=1, no=0), sector, Z value and PHI
      // for the next PMT channel in the event. If all PMT channels have
      // been read out, the function returns status=1 and we move on to the
      // next event.

      int pmtnum = 0, sector = 0, secpar = 0, adcval = 0, adcpost = 0, tac = 0;
      float z = 0, phi = 0;
      float nhit = 0;
      int status = get_data->get_pmt_data(&pmtnum, &adcpost, &adcval, &tac,
                                          &nhit, &sector, &z, &phi);

      if ( (status == 1) )
        {
          // All PMT channels have been processed for this event, go on
          // to the next event
          break;
        }

      if ( (status == 2) )
        {
          // Screwup in data decoding, skip to next pmt
          ipmt++;
          continue;
        }

      //cout<<pmtnum<<" "<<ipmt<<" "<<tact0[pmtnum]<<" "<<pedestal[ipmt]<<" "<<tacpedestal[ipmt]<<" "<<tacsigped[ipmt]<<endl;
      //cout<<tact0[pmtnum]<<"  "<<pmtnum<<endl;
      sector = pmtnum / 1280;
      secpar = (pmtnum % 1280) / 320;

      if (dead[pmtnum] == RICHPMT_ALIVE)
        {
          switch (sector)
            {
            case 0:
              rich_adchistsumWS->Fill(nhit);
              if (nhit > RICH_ADCHIT_DEF_FORTDC 
		  && tac > tacpedestal[pmtnum] + RICH_DEF_HIT * tacsigped[pmtnum] 
		  && tacdead[pmtnum] == RICHPMT_ALIVE
		  ){
                  rich_tdchistsumWS->Fill(tac - tact0[pmtnum]);
                  rich_tdc_first->Fill((float)pmtnum, tac);
                  rich_tdc_second->Fill((float)pmtnum, tac*tac);
                  rich_tdc_hit->Fill((float)pmtnum);
                }
              break;
            case 1:
              rich_adchistsumWN->Fill(nhit);
              if (nhit > RICH_ADCHIT_DEF_FORTDC 
		  && tac > tacpedestal[pmtnum] + RICH_DEF_HIT * tacsigped[pmtnum] 
		  && tacdead[pmtnum] == RICHPMT_ALIVE
		  ){
                  rich_tdchistsumWN->Fill(tac - tact0[pmtnum]);
                  rich_tdc_first->Fill((float)pmtnum, tac);
                  rich_tdc_second->Fill((float)pmtnum, tac*tac);
                  rich_tdc_hit->Fill((float)pmtnum);
                }
              break;
            case 2:
              rich_adchistsumES->Fill(nhit);
              if (nhit > RICH_ADCHIT_DEF_FORTDC 
		  && tac > tacpedestal[pmtnum] + RICH_DEF_HIT * tacsigped[pmtnum] 
		  && tacdead[pmtnum] == RICHPMT_ALIVE
		  ){
                  rich_tdchistsumES->Fill(tac - tact0[pmtnum]);
                  rich_tdc_first->Fill((float)pmtnum, tac);
                  rich_tdc_second->Fill((float)pmtnum, tac*tac);
                  rich_tdc_hit->Fill((float)pmtnum);
                }
              break;
            case 3:
              rich_adchistsumEN->Fill(nhit);
              if (nhit > RICH_ADCHIT_DEF_FORTDC 
		  && tac > tacpedestal[pmtnum] + RICH_DEF_HIT * tacsigped[pmtnum] 
		  && tacdead[pmtnum] == RICHPMT_ALIVE
		  ){
                  rich_tdchistsumEN->Fill(tac - tact0[pmtnum]);
                  rich_tdc_first->Fill((float)pmtnum, tac);
                  rich_tdc_second->Fill((float)pmtnum, tac*tac);
                  rich_tdc_hit->Fill((float)pmtnum);
                }
              break;

            default:
              break;
            }
          if (nhit > RICH_ADCHIT_DEF)
            {
              // This is a ADC hit
              rich_adclivehist->Fill((float)pmtnum);
              rich_adc_hit->Fill((float)pmtnum);
              rich_adc_first->Fill((float)pmtnum, nhit);
              rich_adc_second->Fill((float)pmtnum, nhit*nhit);
              hitmat[pmtnum] = nhit;
	      nhit_sector[sector]++;
	      nhit_secpar[sector][secpar]++;
            }
          if (nhit > RICH_ADCHIT_DEF 
	      && tac > tacpedestal[pmtnum] + RICH_DEF_HIT * tacsigped[pmtnum] 
	      && tacdead[pmtnum] == RICHPMT_ALIVE
	      ){
              // This is a TAC hit
              rich_tdclivehist->Fill((float)pmtnum);
            }
        }
      else
        {     // if(dead[pmtnum]==RICHPMT_ALIVE)
          rich_deadchinlivehist->
	    SetBinContent(rich_deadchinlivehist->FindBin((float)pmtnum),
			  invalid_max);
        }

      ipmt++;

    }

  // contents of rich_nhit_event** are # of hits in recent 10 events.
  if (evtcnt%10==0)
    {
      int bin = (evtcnt/10)%1000+1;
      rich_nhit_eventWS->SetBinContent(bin, nhit_sector[0]);
      rich_nhit_eventWN->SetBinContent(bin, nhit_sector[1]);
      rich_nhit_eventES->SetBinContent(bin, nhit_sector[2]);
      rich_nhit_eventEN->SetBinContent(bin, nhit_sector[3]);

//      if(secpar==0){
          rich_nhit_eventWS1->SetBinContent(bin, nhit_secpar[0][0]);
          rich_nhit_eventWN1->SetBinContent(bin, nhit_secpar[1][0]);
          rich_nhit_eventES1->SetBinContent(bin, nhit_secpar[2][0]);
          rich_nhit_eventEN1->SetBinContent(bin, nhit_secpar[3][0]);
//      }
//      else if(secpar==1){
          rich_nhit_eventWS2->SetBinContent(bin, nhit_secpar[0][1]);
          rich_nhit_eventWN2->SetBinContent(bin, nhit_secpar[1][1]);
          rich_nhit_eventES2->SetBinContent(bin, nhit_secpar[2][1]);
          rich_nhit_eventEN2->SetBinContent(bin, nhit_secpar[3][1]);
//      }
//      else if(secpar==2){
          rich_nhit_eventWS3->SetBinContent(bin, nhit_secpar[0][2]);
          rich_nhit_eventWN3->SetBinContent(bin, nhit_secpar[1][2]);
          rich_nhit_eventES3->SetBinContent(bin, nhit_secpar[2][2]);
          rich_nhit_eventEN3->SetBinContent(bin, nhit_secpar[3][2]);
//      }
//      else if(secpar==3){
          rich_nhit_eventWS4->SetBinContent(bin, nhit_secpar[0][3]);
          rich_nhit_eventWN4->SetBinContent(bin, nhit_secpar[1][3]);
          rich_nhit_eventES4->SetBinContent(bin, nhit_secpar[2][3]);
          rich_nhit_eventEN4->SetBinContent(bin, nhit_secpar[3][3]);
//      }

    }
  else
    {
      int bin = (evtcnt/10)%1000+1;
      int content;

      content = rich_nhit_eventWS->GetBinContent(bin);
      rich_nhit_eventWS->SetBinContent(bin, content+nhit_sector[0]);
      content = rich_nhit_eventWN->GetBinContent(bin);
      rich_nhit_eventWN->SetBinContent(bin, content+nhit_sector[1]);
      content = rich_nhit_eventES->GetBinContent(bin);
      rich_nhit_eventES->SetBinContent(bin, content+nhit_sector[2]);
      content = rich_nhit_eventEN->GetBinContent(bin);
      rich_nhit_eventEN->SetBinContent(bin, content+nhit_sector[3]);

//      if(secpar==0){
          content = rich_nhit_eventWS1->GetBinContent(bin);
          rich_nhit_eventWS1->SetBinContent(bin, content+nhit_secpar[0][0]);
          content = rich_nhit_eventWN1->GetBinContent(bin);
          rich_nhit_eventWN1->SetBinContent(bin, content+nhit_secpar[1][0]);
          content = rich_nhit_eventES1->GetBinContent(bin);
          rich_nhit_eventES1->SetBinContent(bin, content+nhit_secpar[2][0]);
          content = rich_nhit_eventEN1->GetBinContent(bin);
          rich_nhit_eventEN1->SetBinContent(bin, content+nhit_secpar[3][0]);
//      }
//      else if(secpar==1){
          content = rich_nhit_eventWS2->GetBinContent(bin);
          rich_nhit_eventWS2->SetBinContent(bin, content+nhit_secpar[0][1]);
          content = rich_nhit_eventWN2->GetBinContent(bin);
          rich_nhit_eventWN2->SetBinContent(bin, content+nhit_secpar[1][1]);
          content = rich_nhit_eventES2->GetBinContent(bin);
          rich_nhit_eventES2->SetBinContent(bin, content+nhit_secpar[2][1]);
          content = rich_nhit_eventEN2->GetBinContent(bin);
          rich_nhit_eventEN2->SetBinContent(bin, content+nhit_secpar[3][1]);
//      }
//      else if(secpar==2){
          content = rich_nhit_eventWS3->GetBinContent(bin);
          rich_nhit_eventWS3->SetBinContent(bin, content+nhit_secpar[0][2]);
          content = rich_nhit_eventWN3->GetBinContent(bin);
          rich_nhit_eventWN3->SetBinContent(bin, content+nhit_secpar[1][2]);
          content = rich_nhit_eventES3->GetBinContent(bin);
          rich_nhit_eventES3->SetBinContent(bin, content+nhit_secpar[2][2]);
          content = rich_nhit_eventEN3->GetBinContent(bin);
          rich_nhit_eventEN3->SetBinContent(bin, content+nhit_secpar[3][2]);
//      }
//      else if(secpar==3){
          content = rich_nhit_eventWS4->GetBinContent(bin);
          rich_nhit_eventWS4->SetBinContent(bin, content+nhit_secpar[0][3]);
          content = rich_nhit_eventWN4->GetBinContent(bin);
          rich_nhit_eventWN4->SetBinContent(bin, content+nhit_secpar[1][3]);
          content = rich_nhit_eventES4->GetBinContent(bin);
          rich_nhit_eventES4->SetBinContent(bin, content+nhit_secpar[2][3]);
          content = rich_nhit_eventEN4->GetBinContent(bin);
          rich_nhit_eventEN4->SetBinContent(bin, content+nhit_secpar[3][3]);
//      }
    }

  int nerror = get_data->get_err_num();
  for (int ierror = 0;ierror < nerror;ierror++)
    {
      switch ((get_data->get_err(ierror)).err_type)
        {
        case 1:
        case 2:
        case 3:
        case 4:
          rich_par->Fill(1);
          break;

        case 8:
          rich_par->Fill(2);
          break;
        case 16:
          rich_par->Fill(3);
          break;
        case 32:
          rich_par->Fill(4);
          break;

        default :
          break;
        }
    }


  if (verbosity > 0 && evtcnt % 1000 == 0)
    {
      printf("event processing : %d\n",evtcnt);
    }

  
  ring_finder(hitmat, rich_ringhist);
  if (evtcnt == 50000)
    {
      float hitmat[5120];
      int nspike = 0;
      int spikech[5120];
      float mid = 1;
      for (int ich = 0;ich < 5120;ich++)
	{
	  hitmat[ich] = rich_adclivehist->GetBinContent(ich + 1);
	}
      std::sort(hitmat, hitmat + 5120);
      mid = hitmat[2560];
      if (mid <= 0)
	{
	  mid = 1;
	}
      for (int ich = 0;ich < 5120;ich++)
	{
	  // If hit count exceeds error * 40, it is a spike.
	  if (sqrt(mid) * 40 <
	      fabs(rich_adclivehist->GetBinContent(ich + 1) - mid) &&
	      rich_deadchinlivehist->GetBinContent(ich + 1) == 0)
	    {
	      spikech[nspike] = ich;
	      nspike++;
	    }
	}
      if (nspike > 90)
	{
	  OnlMonServer *se = OnlMonServer::instance();
	  se->SetSubsystemStatus(this, OnlMonStatus::NOCALIB);
	}
    }

  if (evtcnt == 20000)
    {

      float pmtadc[RICH_NCHANNEL];
      float pmtadc2[RICH_NCHANNEL];
      int pmtadchit[RICH_NCHANNEL];
      float pmttdc[RICH_NCHANNEL];
      float pmttdc2[RICH_NCHANNEL];
      int pmttdchit[RICH_NCHANNEL];
      float adcmean[RICH_NCHANNEL] , adcrms[RICH_NCHANNEL];
      float tdcmean[RICH_NCHANNEL] , tdcrms[RICH_NCHANNEL];
      int pmtstat[RICH_NCHANNEL];
      int year , month, day, hour, minuit, second, runno;

      time_t orgout = evt->getTime();
      struct tm *evttime = localtime(&orgout);

      runno = (evt->getRunNumber());
      year = (evttime->tm_year) + 1900;
      month = (evttime->tm_mon) + 1;
      day = (evttime->tm_mday);
      hour = (evttime->tm_hour);
      minuit = (evttime->tm_min);
      second = (evttime->tm_sec);

      for (int pmtid = 0;pmtid < RICH_NCHANNEL;pmtid++)
        {
          pmtadchit[pmtid] = (int) rich_adc_hit->GetBinContent(pmtid + 1);
          pmtadc[pmtid] = rich_adc_first->GetBinContent(pmtid + 1);
          pmtadc2[pmtid] = rich_adc_second->GetBinContent(pmtid + 1);

          pmttdchit[pmtid] = (int)rich_tdc_hit->GetBinContent(pmtid + 1);
          pmttdc[pmtid] = rich_tdc_first->GetBinContent(pmtid + 1);
          pmttdc2[pmtid] = rich_tdc_second->GetBinContent(pmtid + 1);

          if (dead[pmtid] == RICHPMT_ALIVE)
            {
              pmtstat[pmtid] = 0;
              if (pmtadchit[pmtid] > 0)
                {
                  adcmean[pmtid] = pmtadc[pmtid] / ((float)pmtadchit[pmtid]);
                  adcrms[pmtid] = sqrt(pmtadc2[pmtid] / ((float)pmtadchit[pmtid]) - pow(pmtadc[pmtid] / ((float)pmtadchit[pmtid]), 2));
                }
              else
                {
                  adcmean[pmtid] = 0.0;
                  adcrms[pmtid] = 0.0;
                }
              if (pmttdchit[pmtid] > 0)
                {
                  tdcmean[pmtid] = pmttdc[pmtid] / ((float)pmttdchit[pmtid]);
                  tdcrms[pmtid] = sqrt(pmttdc2[pmtid] / ((float)pmttdchit[pmtid]) - pow(pmttdc[pmtid] / ((float)pmttdchit[pmtid]), 2));
                }
              else
                {
                  tdcmean[pmtid] = 0.0;
                  tdcrms[pmtid] = 0.0;
                }
            }
          else
            {
              pmtstat[pmtid] = 1;
              pmtadchit[pmtid] = 0;
              pmttdchit[pmtid] = 0;
              adcmean[pmtid] = 0.0;
              adcrms[pmtid] = 0.0;
              tdcmean[pmtid] = 0.0;
              tdcmean[pmtid] = 0.0;
            }
        }

      ostringstream outfilename;
#ifdef DECODE_TEXTFILE

      char this_time[100];
      ofstream fout;
      outfilename.str("");
      outfilename << "run_rich_expert_"
		  << (Onlmonserver->RunNumber()) << ".txt" ;
      fout.open(outfilename.str().c_str(), ios::app);
      if (!fout)
        {
	  ostringstream msg;
          msg << "cannot write expert file" << endl;
	  secd_message(MSG_SEV_INFORMATIONAL, msg.str());
          delete get_data;
          return 1;
        }
      sprintf(this_time, "%04d_%02d_%02d_%02d_%02d_%02d", (evttime->tm_year) + 1900, (evttime->tm_mon) + 1, (evttime->tm_mday), (evttime->tm_hour), evttime->tm_min, evttime->tm_sec);
      printf("RUN_%d  DATA_%s\n",evt->getRunNumber(),this_time);
      fout << "RUN_" << evt->getRunNumber() << "  DATA_" << this_time << endl;
      fout << "PMT" << "  " << "Status" << "  " << "entry" << "  " << "ADCHIT" << "  "
	   << "nHit" << "  " << "nHit RMS" << "  " << "TDCHIT" << "  " << "nHit" << "  "
	   << "nHit" << "  " << "nHit RMS" << endl;
      for (int pmtid = 0;pmtid < RICH_NCHANNEL;pmtid++)
        {
          fout << pmtid << "  " << pmtstat[pmtid] << "  " << evtcnt << "  " << pmtadchit[pmtid] << "  " << adcmean[pmtid] << "  " << adcrms[pmtid] << "  " << pmttdchit[pmtid] << "  " << tdcmean[pmtid] << "  " << tdcmean[pmtid] << endl;
        }
      fout.close();

#endif

#ifdef DECODE_ROOTFILE

      outfilename.str("");
      outfilename << "run_rich_expert_"
		  << (Onlmonserver->RunNumber()) << ".root" ;
      TFile *fin = new TFile(outfilename.str().c_str(), "update");
      TTree *stattree;
      if ( (stattree = (TTree*)fin->Get("stattree")) != NULL)
        {
          stattree->SetBranchAddress("runno", &runno);
          stattree->SetBranchAddress("year", &year);
          stattree->SetBranchAddress("month", &month);
          stattree->SetBranchAddress("day", &day);
          stattree->SetBranchAddress("hour", &hour);
          stattree->SetBranchAddress("minuit", &minuit);
          stattree->SetBranchAddress("second", &second);
          stattree->SetBranchAddress("pmtstat", &pmtstat);
          stattree->SetBranchAddress("evtcnt", &evtcnt);
          stattree->SetBranchAddress("pmtadchit", &pmtadchit);
          stattree->SetBranchAddress("adcmean", &adcmean);
          stattree->SetBranchAddress("adcrms", &adcrms);
          stattree->SetBranchAddress("pmttdchit", &pmttdchit);
          stattree->SetBranchAddress("tdcmean", &tdcmean);
          stattree->SetBranchAddress("tdcrms", &tdcrms);
        }
      else
        {
          stattree = new TTree("stattree", "rich status tree for each run");
        }
      stattree->Branch("runno", &runno, "runno/I");
      stattree->Branch("year", &year, "year/I");
      stattree->Branch("month", &month, "month/I");
      stattree->Branch("day", &day, "day/I");
      stattree->Branch("hour", &hour, "hour/I");
      stattree->Branch("minuit", &minuit, "minuit/I");
      stattree->Branch("second", &second, "second/I");
      stattree->Branch("pmtstat", &pmtstat, "pmtstat[5120]/I");
      stattree->Branch("evtcnt", &evtcnt, "evtcnt/I");
      stattree->Branch("pmtadchit", &pmtadchit, "pmtadchit[5120]/I");
      stattree->Branch("adcmean", &adcmean, "adcmean[5120]/F");
      stattree->Branch("adcrms", &adcrms, "adcrms[5120]/F");
      stattree->Branch("pmttdchit", &pmttdchit, "pmttdchit[5120]/I");
      stattree->Branch("tdcmean", &tdcmean, "tdcmean[5120]/F");
      stattree->Branch("tdcrms", &tdcrms, "tdcrms[5120]/F");

      stattree->Fill();
      fin->cd();
      stattree->Write();
      fin->Close();

      sprintf(this_time, "%04d_%02d_%02d_%02d_%02d_%02d", year, month, day,
              hour, minuit, second);
      printf("%s\n",this_time);

#endif

    }
  delete get_data;
  return 0;
}

int RICHMon::Reset()
{
  // resetting the histograms is not neccessary
  // that is done by the framework
  evtcnt = 0;
  return 0;
}


void RICHMon::ring_finder(float hitmat[], TH1** ringhist)
{

  // Attempts to locate clusters of hits in the RICH event-by-event, and
  // collect statistics on these "rings" in some histograms

  // nrow is the number of 16-PMT rows in the RICH (320)
  // npmt is the number of PMT's in a row (16)
  // nsum is the number of PMT's to sum over when looking for hit clusters


  int nrow = 320, npmt = 16;
  int nsum = 4;
  int hitreg[5120];

  // Make a list of which rows of pmt's contain hits. There are 320 rows.

  int smhit[320];

  // Loop over all rows
  for (int ism = 0;ism < nrow;ism++)
    {
      smhit[ism] = 0;

      // Loop over all PMT's in this row
      for (int ipmt = 0;ipmt < npmt;ipmt++)
        {
          int iaddr = ism * npmt + ipmt;
          if (hitmat[iaddr] > 0)
            {

              // This pmt has a hit
              hitreg[iaddr] = 1;

              // This row has a hit
              smhit[ism] = 1;
            }
          else
            hitreg[iaddr] = 0;
        }
    }

  // Now find which of these hit rows are contiguous

  int smhitkeep = -5, working = 0, hcount = -1;
  int candidate[100][100], numcand[100];

  // Loop over all rows
  for (int ism = 0;ism < nrow;ism++)
    {

      // Was there a hit in this row?
      if (smhit[ism] == 1)
        {
          // There was a hit, was there a hit in the previous row?
          if (ism == smhitkeep + 1)
            {
              // There was a hit in the previous row, icrement the number of
              // consecutive hit rows
              working++;
              smhitkeep++;
            }
          else
            {
              // No hit in the previous row, start a new search for consecutive
              // hit rows
              smhitkeep = ism;
              working = 1;
            }
        }
      else
        {
          // No hit in this row, do we have a set of consecutive hit rows going?
          if (working > 1)
            {
              // We have consective hit rows up to this one, record the details
              hcount++;
              numcand[hcount] = working;

              for (int ic = 0;ic < working;ic++)
                {
                  candidate[hcount][ic] = ism + ic - working;

                }
            }
          working = 0;
        }
    }
  hcount++;

  // So we have clusters of hit rows, now we want to look for clusters of hits,
  // ie. rings, in those clusters of hit rows

  // Make things simple by only looking at clusters of 3,4 or 5 supermodules

  int pmtnum;
  // Loop over all clusters of hit rows
  for (int hc = 0;hc < hcount;hc++)
    {
      // Is this a 3,4 or 5 row cluster?
      if (numcand[hc] == 3 || numcand[hc] == 4 || numcand[hc] == 5 )
        {

          // Throw out any rings which include the first or last row in a sector
          // since we may have lost some of the hits in that case
          // 1st PMT active in sector 4 is 4160, last is 4799
          // Remember that only 1/2 of the PMT's in sector 3 are read out
          // First active row in sector 3 is 260, not 240
          // Last active row in sector 3 is 299, not 319

          if (candidate[hc][0] == 0 || candidate[hc][0] == 80 ||
              candidate[hc][0] == 160 || candidate[hc][0] == 260)
            {
              break;
            }

          if (candidate[hc][0] == 79 || candidate[hc][0] == 159 ||
              candidate[hc][0] == 239 || candidate[hc][0] == 299)
            {
              break;
            }

          float sum, sumkeep = 0;
          int jsm, sumaddr = 0, sumhit, sumpmtnum, sumhitkeep = 0;

          // Slide a nsum pmt wide sum window through this cluster of hit rows, and
          // look for the maximum summed yield. Take the first maximum, for
          // simplicity.

          for (int ipmt = 0;ipmt < npmt - nsum;ipmt++)
            {

              sum = 0;
              sumhit = 0;
              for (int ic = 0;ic < numcand[hc];ic++)
                {

                  jsm = candidate[hc][ic];
                  pmtnum = jsm * npmt + ipmt;

                  for (int isum = 0;isum < nsum;isum++)
                    {
                      int saddr = pmtnum + isum;
                      sum = sum + hitmat[saddr];
                      sumhit = sumhit + hitreg[saddr];
                    }
                }

              if (sum > sumkeep)
                {
                  sumaddr = ipmt;
                  sumkeep = sum;
                  sumhitkeep = sumhit;
                  sumpmtnum = pmtnum;
                }
            }

          // Make cuts to:
          //    Keep only "rings" that have sumkeep values between 2 and 30
          //    Keep only "rings" that have at least 2 PMT's hit, and have less
          //       than all of the PMT's in the sum turned on
          //    Reject rings from sum ranges that begin at ipmt = 0 or 1, or
          //       end at ipmt = 14 or 15. This is to avoid edge effects due to
          //       partial rings and also noisy tubes (which are more common at
          //       the ends of the rows)

          if ( (sumkeep > 2 && sumkeep < 30) &&
               (sumhitkeep > 2 && sumhitkeep < nsum*numcand[hc]) &&
               (sumaddr > 1 && sumaddr < npmt - nsum - 1) )
            {

              // Record the number of PE's in a histogram

              int sector = candidate[hc][1] / 80;

              //	hk6->fillhist(sector,sumkeep);

              // Now get the centroid of the ring

              //	float zcontr=0,norm=0;
              //	float phicontr=0;

              // Loop over all "rings"
              if (sector < 4)
                {
                  ringhist[sector]->Fill(sumkeep);
                }
              /*
              for(int ic=0;ic<numcand[hc];ic++) {

                jsm=candidate[hc][ic];

                // Loop over the sum channels
                       for(int isum=0;isum<nsum;isum++) {

                  pmtnum=jsm*npmt+sumaddr+isum;

                  float z,phi;
                  int sect;
                  geo->get_pmt_coords(pmtnum,&sect,&z,&phi);
                  
                  zcontr=zcontr+hitmat[pmtnum]*z;
                  phicontr=phicontr+hitmat[pmtnum]*phi;
                  norm=norm+hitmat[pmtnum];
                }
              }
                     zcontr=zcontr/norm;
              phicontr=phicontr/norm;
              */

              //hk6->fill_pos_hist(sector,zcontr,phicontr);

            }
        }
    }
}
