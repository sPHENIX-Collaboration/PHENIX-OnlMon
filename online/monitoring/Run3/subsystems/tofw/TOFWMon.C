#include "TOFWMon.h"
#include "OnlMonServer.h"


#include "Event.h"
#include "msg_profile.h"
#include "msg_control.h"

#include "TH1.h"
#include "TH2.h"
#include "TPaveText.h"
#include "TProfile.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

TOFWMon::TOFWMon(): OnlMon("TOFWMON")
{

  debug = 0;

  //ostringstream msg;
  //msg << "TOFW monitoring starting" ;
  //send_message(MSG_SEV_INFORMATIONAL, msg.str());
  
  cutTofwQVC = 15;
  cutTofwTc_0 = 3600;
  cutTofwTc = 30;
  cutTofwChg = 10;
  cutTofwChg_low = 0.00200; 
  cutTofwChg_high = 0.00450;

  evtcnt = 0;
  evtTrigIn = 0;
 
  htofwinfo = new TH1F("htofwinfo", "TOFWMonInfo" , 20, 0, 20);
  htofw210 = new TH1F("htofw210", "TVC (tc3 and tc4)" , 820, 0, 8200);
  htofw220 = new TH1F("htofw220", "Charge (qc1 - qc3)", 500, 0, 1000);
  htofw230 = new TH1F("htofw230", "Num. of Hit :Slat", TOFW_NSTRIP, 0, TOFW_NSTRIP);
  htofw420 = new TH1F("htofw420", "Hit Position" , 400, -20, 20);
  htofw530 = new TProfile("htofw530", "mean ADC vs slat", TOFW_NSTRIP, 0, TOFW_NSTRIP);

  cout<<"tofw histogram initilization"<<endl;

  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(this, htofwinfo);
  Onlmonserver->registerHisto(this, htofw210);
  Onlmonserver->registerHisto(this, htofw220);
  Onlmonserver->registerHisto(this, htofw230);
  Onlmonserver->registerHisto(this, htofw420);
  Onlmonserver->registerHisto(this, htofw530);

  cout<<"OK"<<endl;
}

int TOFWMon::Reset()
{
  evtcnt = 0;
  evtTrigIn = 0;
  //evtTrigIn_ERT = 0;
  // chp: registered histograms are resetted by the server
  return 0;
}


int TOFWMon::process_event(Event *evt)
{
  evtcnt++;
  const int packet[4]={7104, 7102, 7103, 7101};
  const int cutTrigLow = 0x00000002;
  const int cutTrigHigh = 0x00000080;
  const int BBCTrig = 0x00000004;   // BBCLL1
     //const int ERTTrig = 0x00000400;   // ERTLL1_4x4c
  const int BBCERTTrig = 0x00001000;   // ERTLL1_4x4c&BBCLL1
     //  const int ZDCNSTrig = 0x00000200; // (ZDCN|ZDCS)
     //  const int BBCZDC = 0x00000400; // BBCLL1&(ZDCN|ZDCS)

  Packet *p;

  if (evtcnt % 1000 == 0)
    {
      //ostringstream msg;
      //msg << "TOFWMon:Processing Event " << evtcnt ;
      //send_message(MSG_SEV_INFORMATIONAL, msg.str());
      printf("TOFWMon:Processing Event %d\n",evtcnt) ; 
    }

  if (debug)
    {
      cout << "TOFWMon:Processing Event: " << evtcnt << ": ";
    }

  if (evt->getEvtType() == 1)
    {

      // Init. /////////////////////////////////////////////////
      
      int gl1_hit = 0;
      for (int stripID = 0;stripID < TOFW_NSTRIP; stripID++)
        {
          for (int endID = 0; endID < 2; endID++)
            {
	      tofwQc1[endID][stripID] = -1;
	      tofwQc2[endID][stripID] = -1;
	      tofwTc3[endID][stripID] = -1;
	      tofwTc4[endID][stripID] = -1;
	    }
        }
      
      for (int chamberID = 0; chamberID < 128; chamberID++)
	{
	  for (int stripID = 0;stripID < 4; stripID++)
	    {
	      for (int endID = 0; endID < 2; endID++)
		{
		tofwQcnew1[endID][stripID][chamberID] = -1;
		tofwQcnew2[endID][stripID][chamberID] = -1;
		tofwTcnew3[endID][stripID][chamberID] = -1;
		tofwTcnew4[endID][stripID][chamberID] = -1;
		}
	    }
	}
      
      // Trig Event //////////////////////////////////////////
      
      if (debug)
        {
          cout << "Trig: ";
        }
      
      p = evt->getPacket(14001);
      if (p)
        {
          int trig_GL1 = p->iValue(0, "SCALEDTRIG");
          delete p;

	  if ((trig_GL1 & BBCERTTrig) == 0 && (trig_GL1 & BBCTrig) == 0)
            {  // require BBCLL1 triger
              if (debug)
                {
                  cout << "out: ";
                }
              evtTrigIn++;
              gl1_hit = 1;
            }
          else
            {
              evtTrigIn++;
              gl1_hit = 1;
              if (debug)
                {
                   cout << "in: ";
                }
            }

	}
      // End of if(p){
     
      // Tofw Event //////////////////////////////////////////////
      if (gl1_hit)
	{
          //if(true){
          if (debug)
            {
              cout << "Fetch: ";
            }
          for (int icrt = 0;icrt < 4;icrt++)//tofw has 4 crates
            { //-------Data Fetch
              p = evt->getPacket(packet[icrt]);
              if (p)
                {
                  int maxbd = 16;
                  for (int ibd = 0; ibd < maxbd;ibd++)
                    {
                      for (int ich = 0; ich < 16;ich++)
			{
			  int stripID = icrt*32*4+(15-ibd)*2*4+ich/8*4+ich%4;
			  int endID = ich%8/4;

                          int iCh = 16 * ibd + ich;
                          tofwQc1[endID][stripID] = p->iValue(iCh, "QC1");
                          tofwQc2[endID][stripID] = p->iValue(iCh, "QC3");
                          tofwTc3[endID][stripID] = p->iValue(iCh, "TC3");
                          tofwTc4[endID][stripID] = p->iValue(iCh, "TC4");

			  tofwQcnew1[endID][stripID%4][stripID/4] = tofwQc1[endID][stripID];
			  tofwQcnew2[endID][stripID%4][stripID/4] = tofwQc2[endID][stripID];
			  tofwTcnew3[endID][stripID%4][stripID/4] = tofwTc3[endID][stripID];
			  tofwTcnew4[endID][stripID%4][stripID/4] = tofwTc4[endID][stripID];
                        }
                    }
                  delete p;
                } // End of if(p){
            } // End of Data Fetch

          // Hist Fill ///////////////////////////////////////

          // End by End
          if (debug)
            {
              cout << "END: ";
            }

	  int maxadc[2];
	  //int maxtdc[2];
	  //int maxstrip=-9999;
	  for (int chamberID = 0; chamberID < 128; chamberID++)
	    {
	      maxadc[0] = -9999;
	      maxadc[1] = -9999;
	      //maxstrip=-9999;
	      for (int stripID = 0; stripID < 4; stripID++)
		{
		  for (int endID = 0; endID < 2; endID++)
		    {
		      
		      //maxtdc[endID] = -9999;
		      int nowTofwQc1_0 = tofwQcnew1[endID][stripID][chamberID];
		      int nowTofwQc2_0 = tofwQcnew2[endID][stripID][chamberID];
		      int nowTofwTc3_0 = tofwTcnew3[endID][stripID][chamberID];
		      int nowTofwTc4_0 = tofwTcnew4[endID][stripID][chamberID];
		      
		      if ( nowTofwQc1_0 > 0 && nowTofwQc2_0 > 0 && 
			   nowTofwTc3_0 > 0 && nowTofwTc4_0 > 0)
			{ // For 0 Sup.Check
			  // QVC //
			  int nowTofwQVC = nowTofwQc1_0 - nowTofwQc2_0;
			  
			  if(nowTofwQVC>maxadc[endID]&&(nowTofwTc3_0<cutTofwTc_0 ||nowTofwTc4_0<cutTofwTc_0))
			    { 
			      maxadc[endID] = nowTofwQVC;
			      //maxadc[endID] = nowTofwTc3_0;
			      //maxstrip = stripID;
			    }
			  // TVC //
			  if (nowTofwTc3_0 < cutTofwTc_0 )
			    {
			      htofw210->Fill(nowTofwTc3_0);	//--- "TVC (tc3 or tc4)"
			    }
			  
			  if (nowTofwTc4_0 < cutTofwTc_0)
			    {
			      htofw210->Fill(nowTofwTc4_0 + 4096);  //--- "TVC (tc3 or tc4)"
			    }			  
			} // End of. "For 0 Sup.Check"
		    } // End of for( endID=0; endID<=1; endID++){
		} // End of for(stripID =0; stripID<= 4; stripID++){
	      
	      if(maxadc[0]>cutTofwQVC && maxadc[1]>cutTofwQVC){
		htofw220->Fill(maxadc[0]);	//--- "QVC (qc1 - qc2)"
		htofw220->Fill(maxadc[1]);	//--- "QVC (qc1 - qc2)"
	      }
	    }// End of for(chamberID =0; chamberID<= 128; chamberID++){

          // strip by strip
          for (int stripID = 0; stripID < TOFW_NSTRIP; stripID++)
            {
              int nowTofwQc1_0 = tofwQc1[0][stripID];
              int nowTofwQc2_0 = tofwQc2[0][stripID];
              int nowTofwTc3_0 = tofwTc3[0][stripID];
              int nowTofwTc4_0 = tofwTc4[0][stripID];
              float PedeTc3_0 = 3920;
              float PedeTc4_0 = PedeTc3_0;

              int nowTofwQc1_1 = tofwQc1[1][stripID];
              int nowTofwQc2_1 = tofwQc2[1][stripID];
              int nowTofwTc3_1 = tofwTc3[1][stripID];
              int nowTofwTc4_1 = tofwTc4[1][stripID];
              float PedeTc3_1 = 3920;
              float PedeTc4_1 = PedeTc3_1;

              if ( nowTofwQc1_0 > nowTofwQc2_0 && nowTofwQc2_0 > 0 && nowTofwTc3_0 > 0 && nowTofwTc4_0 > 0
                   && nowTofwQc1_1 > nowTofwQc2_1 && nowTofwQc2_1 > 0 && nowTofwTc3_1 > 0 && nowTofwTc4_1 > 0)
                { // For 0 Sup.Check

                  int nowQVC_0 = nowTofwQc1_0 - nowTofwQc2_0;
                  int nowQVC_1 = nowTofwQc1_1 - nowTofwQc2_1;
                  //float convQVC_0 = 18.0;
                  //float convQVC_1 = 18.0;
                  float nowTofwChg = sqrt(1.0* nowQVC_0 * nowQVC_1 );
		  //float ElossConv = tofmoncalib->getElossConv(stripID);
		  // float nowTofwChg = ElossConv * sqrt(((double)nowQVC_0) * ((double)nowQVC_1));

                  //float AvenowQVC = (nowQVC_0+nowQVC_1)/2.;
                  //float aveTvc = (nowTofwTvc_0+nowTofwTvc_1)/2.;

                  if ( (nowTofwTc3_0 < PedeTc3_0 - cutTofwTc || nowTofwTc4_0 < PedeTc4_0 - cutTofwTc ) &&
                       (nowTofwTc3_1 < PedeTc3_1 - cutTofwTc || nowTofwTc4_1 < PedeTc4_1 - cutTofwTc ) &&
                       (nowTofwChg > cutTofwChg_low ) )
                    { // tc3 or tc4 Cut && Charge Cut

                      //htof230->Fill(stripID); // Num of Hit : Strip
                      //htof510->Fill(stripID,AvenowQVC);
                      //htof520->Fill(stripID,AvenowQVC);

                      //if ( nowTofwChg < cutTofwChg_high )
		      //{
		      htofw530->Fill(stripID, nowTofwChg);
		      //}

                      const int TVC_DIFF_TC3_TC4 = 81; // Tmp 81ch? What?
                      float convTVC_0 = 0.029;//tofmoncalib->getTvcConv(0, stripID);
                      float convTVC_1 = 0.029;//tofmoncalib->getTvcConv(1, stripID);
                      float nowTofwTvc_0 = nowTofwTc3_0;
                      if (nowTofwTc3_0 > nowTofwTc4_0)
                        {
                          nowTofwTvc_0 = nowTofwTc4_0 + TVC_DIFF_TC3_TC4;
                        }
                      float nowTofwTvc_1 = nowTofwTc3_1;
                      if (nowTofwTc3_1 > nowTofwTc4_1)
                        {
                          nowTofwTvc_1 = nowTofwTc4_1 + TVC_DIFF_TC3_TC4;
                        }

                      float nowTofwTime_0 = nowTofwTvc_0 * convTVC_0;
                      float nowTofwTime_1 = nowTofwTvc_1 * convTVC_1;

                      //float Ttof = ( nowTofwTime_0 + nowTofwTime_1 ) / 2.;
                      float Tdif = ( nowTofwTime_0 - nowTofwTime_1 ) / 2.;

                      //htofw410->Fill(Ttof);
                      htofw420->Fill(Tdif);

                    } // End of tc3 or tc4 Cut && Charge Cut
                } // End of for( pmtID=0; pmtID<=1; pmtID++){
            } // End of for(stripID =0; stripID<= 959; stripID++){
	} // End of if(gl1_hit){
  
    } // End of if(evt->getEvtType()==1){
  else
    {
      if (debug)
        {
          cout << "Check TOFMon.C: evt->getEvtType()" << endl;
        }
    }
  if (debug)
    {
      cout << "Done" << endl;
    }

  htofwinfo->SetBinContent(1, cutTrigLow);
  htofwinfo->SetBinContent(2, cutTrigHigh);
  htofwinfo->SetBinContent(3, cutTofwQVC);
  htofwinfo->SetBinContent(4, cutTofwTc_0);
  htofwinfo->SetBinContent(5, cutTofwTc);
  htofwinfo->SetBinContent(6, cutTofwChg_low);
  htofwinfo->SetBinContent(7, evtcnt);
  htofwinfo->SetBinContent(8, evtTrigIn);
  //htofwinfo->SetBinContent(9, evtTrigIn_ERT);

  return 0;
}
/*
int
TOFWMon::send_message(const int severity, const string &err_message) const
{
  // check $ONLINE_MAIN/include/msg_profile.h for MSG defs
  // if you do not find your subsystem, do not initialize it and drop me a line
  msg_control *Message = new msg_control(MSG_TYPE_MONITORING,
					 MSG_SOURCE_TOF,
					 severity, Name());
  cout << *Message << err_message << endl;
  delete Message;
  // dump message into my logfile which will be saved inside the html
  // output (no INFORMATIONAL messages)
  if (severity > MSG_SEV_INFORMATIONAL)
    {
      OnlMonServer *se = OnlMonServer::instance();
      se->WriteLogFile(ThisName, err_message);
    }

  return 0;
}
*/
