#include "ACCMon.h"
#include "AccGeom.h"
#include "GetAccPmtData.h"
#include "CalibrationsRead.h"

#include <OnlMonServer.h>

#include <Event.h>
#include <msg_profile.h>
#include <msg_control.h>

#include <TH1.h>
#include <TH2.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

ACCMon::ACCMon(const char *name): OnlMon(name)
{


  evtcnt = 0;

  chanperpacket = ACC_NCHANNEL;  //  The number of PMT's per packet
  packetsperevent = 2; // The number of packets per event
  OnlMonServer *se = OnlMonServer::instance();
  acc_livehist = new TH1F("acc_livehist", "Number of hits for each ACC PMT on ADC",
			  ACC_ALLCHANNEL, 0, ACC_ALLCHANNEL);

  se->registerHisto(this, acc_livehist);
  acc_Nadcsum = new TH1F("acc_Nadcsum", "Number of ADC counts", 350, -2, 27);
  se->registerHisto(this, acc_Nadcsum);
  acc_Sadcsum = new TH1F("acc_Sadcsum", "Number of ADC counts", 350, -2, 27);
  se->registerHisto(this, acc_Sadcsum);
  adc_average = new TH1F("adc_average", "Average Np.e of each PMT",
			 ACC_ALLCHANNEL, 0, ACC_ALLCHANNEL);

  se->registerHisto(this, adc_average);
  acc_Ntacsum = new TH1F("acc_Ntacsum", "Number of TAC counts", 1025, -50, 974);
  se->registerHisto(this, acc_Ntacsum);
  acc_Stacsum = new TH1F("acc_Stacsum", "Number of TAC counts", 1025, -50, 974);
  se->registerHisto(this, acc_Stacsum);
  tac_average = new TH1F("tac_average", "Average Hit Time of each PMT"
			 , ACC_ALLCHANNEL, 0, ACC_ALLCHANNEL);
  se->registerHisto(this, tac_average);

  acc_taclivehist = new TH1F("acc_taclivehist", "Number of hits for each ACC PMT onTAC",
			     ACC_ALLCHANNEL, 0, ACC_ALLCHANNEL);

  se->registerHisto(this, acc_taclivehist);
  acc_par = new TH1F("acc_par", "Aerogel parameter", 8, 0, 8);

  se->registerHisto(this, acc_par);


  for(int k = 0; k < 5; k++)
    {
      char graph_name[30];
      sprintf(graph_name, "acc_Ntacsum_board_%d", k);
      //acc_Ntacsum_board[k] = new TH1F(graph_name, "North TAC Pedestal", 192, -128, 64);
      acc_Ntacsum_board[k] = new TH1F(graph_name, "North TAC Pedestal", 192, -80, 112);
      se->registerHisto(this, acc_Ntacsum_board[k]);
      sprintf(graph_name, "acc_Stacsum_board_%d", k);
      //acc_Stacsum_board[k] = new TH1F(graph_name, "South TAC Pedestal", 192, -128, 64);
      acc_Stacsum_board[k] = new TH1F(graph_name, "South TAC Pedestal", 192, -80, 112);
      se->registerHisto(this, acc_Stacsum_board[k]);

    }

  acc_adc_tac_N = new TH2F("acc_adc_tac_N", "ADC/TAC North", 64, -50, 974, 64, -50, 974);
  se->registerHisto(this, acc_adc_tac_N);
  acc_adc_tac_S = new TH2F("acc_adc_tac_S", "ADC/TAC South", 64, -50, 974, 64, -50, 974);
  se->registerHisto(this, acc_adc_tac_S);


  /*
    0: N of evt processed
    1: N of packet format error
    2: N of several "ffff" pattern appered
    3: N of so much "ffff" pattern appered
    4: N of too much "ffff" pattern appered
    5: packet format
    6: Run #
  */
  //adcgainfile = ACC_ADC_CONST_FILE;
  //adcpedfile = ACC_ADCPED_CONST_FILE;
  //tacpedfile = ACC_TAC_CONST_FILE;

  calread = new CalibrationsRead();
  //calread_status = calread->getcalibs(adcgainfile.c_str(), pep, sigpep, adcpedfile.c_str(), pedestal, sigped,
  //                                      tacpedfile.c_str(), tacpedestal, tacsigped, dead, tacdead);
  calread_status = calread->getcalibs(pep, sigpep, pedestal, sigped, tacpedestal, tacsigped, dead, tacdead);
}

ACCMon::~ACCMon()
{
  delete calread;
}

int ACCMon::process_event(Event *evt)
{
  evtcnt++;

  acc_par->Fill(0);

  Packet *p = 0;
  p = evt->getPacket(17001);

  if (p)
    {
      acc_par->Fill(5);
      delete p;
    }

  for (int j = 0; j < ACC_NCHANNEL; j++)
    {
      hitmat[j] = 0;
      hittime[j] = 0;
    }

  int print_hits = 0;
  GetAccPmtData *get_data =
    new GetAccPmtData(evt, &status, pedestal, sigped, pep, sigpep, print_hits);

  int ipmt = 0;
  while (ipmt < packetsperevent * chanperpacket)
    {
      // The call to get_data->get_pmt_data() returns the PMT number, ADC
      // value, TAC value, hit status (yes=1, no=0), sector, Z value and PHI
      // for the next PMT channel in the event. If all PMT channels have
      // been read out, the function returns status=1 and we move on to the
      // next event.

      int pmtnum = 0, sector = 0, adcval = 0, adcpost = 0, tac = 0;
      float nhit = 0;
      int status = get_data->get_pmt_data(&pmtnum, &adcpost, &adcval, &tac,
					  &nhit, &sector);

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
      if (dead[pmtnum] == ACCPMT_ALIVE)
	{
	  if(ipmt < 160)
	    {
	      acc_Nadcsum->Fill(nhit);
	    }
	  else
	    {
	      acc_Sadcsum->Fill(nhit);
	    }

	  if (nhit > ACC_ADCHIT_DEF)
	    {
	      // This is a ADC hit
	      acc_livehist->Fill(pmtnum);
	      adc_average->AddBinContent(pmtnum + 1, nhit);
	      //	  hitmat[pmtnum]+=nhit;
	    }
	  else if(dead[ipmt] == ACCPMT_DEAD)
	    {
	      acc_livehist->SetBinContent(ipmt + 1, -1);
	    }
	  if(ipmt < 160)
	    {
	      acc_Ntacsum->Fill(tac - tacpedestal[ipmt]);
	    }
	  else
	    {
	      acc_Stacsum->Fill(tac - tacpedestal[ipmt]);
	    }

	  if(ipmt < 160)
	    {
	      if(tac - tacpedestal[ipmt] > 40)acc_adc_tac_N->Fill(adcval - pedestal[ipmt], tac - tacpedestal[ipmt]);
	    }
	  else
	    {
	      if(tac - tacpedestal[ipmt] > 40)acc_adc_tac_S->Fill(adcval - pedestal[ipmt], tac - tacpedestal[ipmt]);
	    }

	  if(tac > 0)
	    {

	      if(ipmt < 32)
		{
		  acc_Ntacsum_board[0]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 64)
		{
		  acc_Ntacsum_board[1]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 96)
		{
		  acc_Ntacsum_board[2]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 128)
		{
		  acc_Ntacsum_board[3]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 160)
		{
		  acc_Ntacsum_board[4]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 192)
		{
		  acc_Stacsum_board[0]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 224)
		{
		  acc_Stacsum_board[1]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 256)
		{
		  acc_Stacsum_board[2]->Fill(tac - tacpedestal[ipmt]);
		}
	      else if(ipmt < 288)
		{
		  acc_Stacsum_board[3]->Fill(tac - tacpedestal[ipmt]);
		}
	      else
		{
		  acc_Stacsum_board[4]->Fill(tac - tacpedestal[ipmt]);
		}

	      if (tac > tacpedestal[ipmt] + ACC_DEF_HIT * tacsigped[ipmt] &&
		  dead[pmtnum] == ACCPMT_ALIVE)
		{
		  acc_taclivehist->Fill(pmtnum);
		  tac_average->AddBinContent(pmtnum + 1, (float)(tac - tacpedestal[ipmt]));
		}
	    }
	}
      else if(dead[ipmt] == ACCPMT_DEAD)
	{
	  acc_livehist->SetBinContent(ipmt + 1, -1);
	  acc_taclivehist->SetBinContent(ipmt + 1, -1);
	}
      ipmt++;
    }

  int nerror = get_data->get_err_num();
  for (int ierror = 0; ierror < nerror; ierror++)
    {
      switch ((get_data->get_err(ierror)).err_type)
	{
	case 1:
	  acc_par->Fill(1);
	  break;
	case 2:
	  acc_par->Fill(1);
	  break;
	case 3:
	  acc_par->Fill(1);
	  break;
	case 4:
	  acc_par->Fill(1);
	  break;
	case 8:
	  acc_par->Fill(2);
	  break;
	case 16:
	  acc_par->Fill(3);
	  break;
	case 32:
	  acc_par->Fill(4);
	  break;

	default :
	  break;
	}
    }
  delete get_data;
  return 0;
}

int ACCMon::Reset()
{
  // resetting the histograms is not neccessary
  // that is done by the framework
  evtcnt = 0;
  return 0;
}
