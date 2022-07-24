#include <Gl1Mon.h>

#include <OnlMonServer.h>
#include <OnlMonTrigger.h>
#include <OnlMonDB.h>

#include <Event.h>
#include <msg_control.h>
#include <EventTypes.h>

#include <TH1.h>

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// fix bad live trigger fifo data
#define FIX_BAD_LIVE_FIFO

using namespace std;

Gl1Mon::Gl1Mon(): OnlMon("GL1MON")
{
  Reset();

  Gl1_RawHist = new TH1F("Gl1_RawHist", "GL1-1 Raw triggers",                                       32, 0.5, 32.5);
  Gl1_LiveHist = new TH1F("Gl1_LiveHist", "GL1-1 Live Triggers",                                    32, 0.5, 32.5);
  Gl1_ScaledHist = new TH1F("Gl1_ScaledHist", "GL1-1 Scaled Triggers",                              32, 0.5, 32.5);
  Gl1_ScaledHist_BBCLL1 = new TH1F("Gl1_ScaledHist_BBCLL1", "GL1-1 Scaled Triggers (BBCLL1 reqd.)", 32, 0.5, 32.5);
  Gl1_ScaledHist_ZDC = new TH1F("Gl1_ScaledHist_ZDC", "GL1-1 Scaled Triggers (ZDCNS reqd.)",        32, 0.5, 32.5);
  Gl1_ScalerRawHist = new TH1F("Gl1_ScalerRawHist", "Raw triggers",                                 32, 0.5, 32.5);
  Gl1_ScalerLiveHist = new TH1F("Gl1_ScalerLiveHist", "Live Triggers",                              32, 0.5, 32.5);
  Gl1_ScalerScaledHist = new TH1F("Gl1_ScalerScaledHist", "Scaled Triggers",                        32, 0.5, 32.5);
  Gl1_RejectHist = new TH1F("Gl1_RejectHist", "Trigger Rejections based on BBCLL>=1",               32, 0.5, 32.5);
  Gl1_ExpectRejectHist = new TH1F("Gl1_ExpectRejectHist", "Expected Rejections",                    32, 0.5, 32.5);
  Gl1_bandwidthplot = new TH1F("Gl1_bandwidthplot", "Trigger Bandwidths",                           32, 0.5, 32.5);
  Gl1_CrossCounter_bbcll1 = new TH1F("Gl1_CrossCounter_bbcll1", "BBC Crossing Counter",                     120, -0.5, 119.5);
  Gl1_CrossCounter_bbcll1_nvc = new TH1F("Gl1_CrossCounter_bbcll1_nvc", "BBC noVertexCut Crossing Counter", 120, -0.5, 119.5);
  Gl1_CrossCounter_zdc = new TH1F("Gl1_CrossCounter_zdc", "ZDC Crossing Counter",                           120, -0.5, 119.5);
  Gl1_present_count = new TH1F("Gl1_present_count", "Temporary storage hist",                       96, 0, 96);
  Gl1_present_rate = new TH1F("Gl1_present_rate", "Temporary storage hist",                         96, 0, 96);
  Gl1_RejectRatio_1 = new TH1F("Gl1_RejectRatio_1", "Rejection Factor Ratio Based on",              32, 0.5, 32.5);
  Gl1_RejectRatio_2 = new TH1F("Gl1_RejectRatio_2", "Rejection Factor Ratio Based on",              32, 0.5, 32.5);


  //The Histogram Gl1_StorageHist is used to transfer data from server to client, and now we
  //use only 37 bins of 64. You can use others freely. Histograms is now the only way we find
  //to transfer data from server to client, and it can only transfer numbers.

  // bin 1: unused
  // bin 2: total size of all events 
  // bin 3: unused
  // bin 4: unused
  // bin 5-36: event-size weighted sum of triggers
  // bin 37: total detected events with 0 scaled trigger word
  // bin 38: total detected events with scaled/live mismatch
  // bin 39: total detected events with scaled/raw mismatch
  // bin 40: raw trigger word not equal to live trigger word

  Gl1_StorageHist = new TH1F("Gl1_StorageHist", "Temporary storage hist", 64, 0, 64);
  // register histograms with server otherwise client won't get them

  OnlMonServer *se = OnlMonServer::instance();

  se->registerHisto(this,Gl1_RawHist);
  se->registerHisto(this,Gl1_LiveHist);
  se->registerHisto(this,Gl1_ScaledHist);
  se->registerHisto(this,Gl1_ScaledHist_BBCLL1);
  se->registerHisto(this,Gl1_ScaledHist_ZDC);
  se->registerHisto(this,Gl1_ScalerRawHist);
  se->registerHisto(this,Gl1_ScalerLiveHist);
  se->registerHisto(this,Gl1_ScalerScaledHist);
  se->registerHisto(this,Gl1_RejectHist);
  se->registerHisto(this,Gl1_ExpectRejectHist);
  se->registerHisto(this,Gl1_bandwidthplot);
  se->registerHisto(this,Gl1_CrossCounter_bbcll1);
  se->registerHisto(this,Gl1_CrossCounter_bbcll1_nvc);
  se->registerHisto(this,Gl1_CrossCounter_zdc);
  se->registerHisto(this,Gl1_present_count);
  se->registerHisto(this,Gl1_present_rate);
  se->registerHisto(this,Gl1_StorageHist);
  se->registerHisto(this,Gl1_RejectRatio_1);
  se->registerHisto(this,Gl1_RejectRatio_2);

  ostringstream hname, htitle;
      hname.fill('0');
  for (int i=0;i<32;i++)
    {
      hname.str("");
      htitle.str("");
      unsigned int ishi = 1 << i;
      hname << hex << "GL1_CrossCtr_0x" << setw(8) << ishi << dec;
      htitle << hex << "0x" << ishi << dec;
      GL1_AllCross[i] = new TH1F(hname.str().c_str(),htitle.str().c_str(),120, -0.5, 119.5);
      se->registerHisto(this,GL1_AllCross[i]);
    }


  // Set up database

  dbvars = new OnlMonDB(ThisName); // use monitor name for db table name
  DBVarInit();

  return ;
}

Gl1Mon::~Gl1Mon()
{
  delete dbvars;
}


int Gl1Mon::BeginRun(const int runno)
{
  OnlMonServer *se = OnlMonServer::instance();
  Gl1_StorageHist->SetBinContent(3, (float)(runno));

  // Read in the list of prescale values
  ostringstream filename;

  // Read in the list of expected rejection factors
  filename.str("");

  if (getenv("GL1REJECTION"))
    {
      filename << getenv("GL1REJECTION");
    }
  else
    {
      filename << "gl1_expected_rejection.list";
    }

  ifstream infile;
  infile.open(filename.str().c_str(), ios_base::in);
  if (infile)
    {
      int i = 0;
      while (
	     infile
	     >> bias1_expected_rejection[i] >> bias1_expected_rejection_err[i]
	     >> bias2_expected_rejection[i] >> bias2_expected_rejection_err[i]
	     )
	{
	  i++;
	  if (i > 32)
	    {
	      ostringstream msg;
	      msg << filename << " is corrupt or has wrong format, correct it";
	      se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_FATAL, msg.str(),1);
	      exit(1);
	    }
	}
      infile.close();
      infile.clear();
    }
  else
    {
      ostringstream msg;
      msg << "Could not open file " << filename.str()
	  << " no expected rejection" ;
      se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),1);
    return 0;
  }

  filename.str("");

  if (getenv("GL1MINBIAS"))
    {
      filename << getenv("GL1MINBIAS");
    }
  else
    {
      filename << "gl1_isminbias.list";
    }
  infile.open(filename.str().c_str(), ios_base::in);

  if (infile)
    {
      getline(infile, BBCLL1_NAME);
      getline(infile, ZDC_NAME);
      getline(infile, BBCLL1_NOVERTEXCUT_NAME);
      //cout << BBCLL1_NAME <<"\t\t" << ZDC_NAME <<"\t\t" << BBCLL1_NOVERTEXCUT_NAME << endl;
      infile.close();
      infile.clear();
    }
  else
    {
      ostringstream msg;
      msg << "Could not open file " << filename.str()
	  << " no minbias trigger definitions" ;
      se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),1);
      return 0;
    }
  ostringstream htitle;
  htitle.fill('0');
  for (int i = 0; i < 32; i++)
    {
      printf("%s: Bit %02d: %s\n",Name(),i,se->OnlTrig()->get_lvl1_trig_name_bybit(i).c_str());
      htitle.str("");
      // the trigger name used in the client is this title
      unsigned int ishi = 1 << i;
      htitle << hex << "Bit: 0x" << setw(8) << ishi << dec << ", Name: ";
      htitle << se->OnlTrig()->get_lvl1_trig_name_bybit(i); 
      GL1_AllCross[i]->SetTitle(htitle.str().c_str());
    }

  for (int i = 0;i < 32;i++)
    {
      // Below we will decide these bits:  BBCLL1_bit NTCn_bit ZDC_bit BBCLL1_novertex_bit

      if (se->OnlTrig()->get_lvl1_trig_name_bybit(i) == BBCLL1_NAME)
        {
	  //cout<<"find bbcll1 at bit "<<i<<endl;
          BBCLL1_bit_mask = (1 << i);
        }
      else if ( se->OnlTrig()->get_lvl1_trig_name_bybit(i) == ZDC_NAME)
        {
 	  //cout<<"find zdc at bit "<<i<<endl;
          ZDC_bit_mask = (1 << i);
        }
      else if (se->OnlTrig()->get_lvl1_trig_name_bybit(i) == BBCLL1_NOVERTEXCUT_NAME)
        {
	  //cout<<"find bbc_novertexcut at bit "<<i<<endl;
          BBCLL1_NOVERTEXCUT_bit_mask = (1 << i);
        }
    }
  return 0;

}


int Gl1Mon::Reset()
{

  BBCLL1_bit_mask = 0;
  ZDC_bit_mask = 0;
  BBCLL1_NOVERTEXCUT_bit_mask = 0;

  runbegin_time = 0;
  initial_seconds = 0; // time of first scaler event
  time_now = 0;        // time of each scaler event

  // interval between each scaler event to the first scaler event. 
  // If it is the first scaler event, then the interval between it and first event
  time = 0; 

  last_time = 0;       // time of each event
  scaler_init = 0;     // Set to 0 at reset, to 1 at first event, to 2 at first scaler event
  evtcnt = 0;

  minbias_triggers = 0;
  total_size = 0;

  LastCommit = 0; 

  zdcRaw = 0;
  bbczdcRaw = 0;
  bbcRaw = 0;
  zdcLive = 0;
  bbczdcLive = 0;
  bbcLive = 0;
  zdcScaled = 0;
  bbczdcScaled = 0;
  bbcScaled = 0;

  memset(bias1_expected_rejection, 0, sizeof(bias1_expected_rejection));
  memset(bias1_expected_rejection_err, 0, sizeof(bias1_expected_rejection_err));
  memset(bias2_expected_rejection, 0, sizeof(bias2_expected_rejection));
  memset(bias2_expected_rejection_err, 0, sizeof(bias2_expected_rejection_err));
  memset(trigsize, 0, sizeof(trigsize));
  memset(initial_countRaw, 0, sizeof(initial_countRaw));
  memset(initial_countLive, 0, sizeof(initial_countLive));
  memset(initial_countScaled, 0, sizeof(initial_countScaled));
  memset(present_countRaw, 0, sizeof(present_countRaw));
  memset(present_countLive, 0, sizeof(present_countLive));
  memset(present_countScaled, 0, sizeof(present_countScaled));

  NumBadScaledEvents = 0; 
  NumRawNELiveEvents = 0;
  NumBadScalLiveEvents = 0;
  NumBadScalRawEvents = 0;

  return 0;

}


int Gl1Mon::process_event(Event *event)
{
  evtcnt++;

  if (event->getEvtType() == BEGRUNEVENT || event->getEvtType() == ENDRUNEVENT)
    {
      return 0;
    }

  // get the event size
  float event_size = event->getEvtLength();
  if(event->getEvtType()==1) total_size += event_size; // DATA events only
  Gl1_StorageHist->SetBinContent(2, total_size);

  time_now = event->getTime();
  if(scaler_init == 0) {
    scaler_init = 1;
    runbegin_time = time_now;
  }

  OnlMonServer *se = OnlMonServer::instance();

  // If this is a scaler event, extract the scaler values and increment
  // the rate counters
  if (event->getEvtType() == SCALEREVENT )
    {

      // This is a scaler event
      if (Verbosity() > 0)
        {
          printf("Scaler event in gl1Monitor\n");
        }

      // Get special scaler packet

      Packet *p;
      p = event->getPacket(900);
      if (p)
        {

          // Is this the first scaler event found in this session of monitoring?
          // If so, initialize the counters
          if (scaler_init < 2)
            {

              scaler_init = 2;
              initial_seconds = time_now;
	      time = initial_seconds - runbegin_time;

              if (Verbosity() > 0)
                {
                  printf("Initializing scaler counters\n");
                }

              for (int i = 0;i < 32;i++)
                {

                   int count_iValue = se->OnlTrig()->get_lvl1_trig_index_bybit(i);
                   initial_countRaw[i] = p->iValue(count_iValue, "RAWSCALERS");
                   initial_countLive[i] = p->iValue(count_iValue, "LIFESCALERS");
                   initial_countScaled[i] = p->iValue(count_iValue, "SCALEDSCALERS");

                    present_countRaw[i] =  initial_countRaw[i];
                    present_countLive[i] =  initial_countLive[i];
                    present_countScaled[i] =  initial_countScaled[i];

		    Gl1_present_count->SetBinContent(1 + i, present_countRaw[i]);
		    Gl1_present_count->SetBinContent(1 + i + 32, present_countLive[i]);
		    Gl1_present_count->SetBinContent(1 + i + 64, present_countScaled[i]);

		    Gl1_present_rate->SetBinContent(1 + i, (float)present_countRaw[i] / time);
		    Gl1_present_rate->SetBinContent(1 + i + 32, (float)present_countLive[i] / time);
		    Gl1_present_rate->SetBinContent(1 + i + 64, (float)present_countScaled[i] / time);

                }


              delete p;

              return 0;

            }

          else
            {
              time = time_now - initial_seconds;

              // Now find the accumulated scaler counts and rates for each bit

              for (int i = 0;i < 32;i++)
                {

                  int count_iValue = se->OnlTrig()->get_lvl1_trig_index_bybit(i);
                  present_countRaw[i] = p->iValue(count_iValue, "RAWSCALERS") - initial_countRaw[i];
                  present_countLive[i] = p->iValue(count_iValue, "LIFESCALERS") - initial_countLive[i];
                  present_countScaled[i] = p->iValue(count_iValue, "SCALEDSCALERS") - initial_countScaled[i];

                  Gl1_present_count->SetBinContent(1 + i, present_countRaw[i]);
                  Gl1_present_count->SetBinContent(1 + i + 32, present_countLive[i]);
                  Gl1_present_count->SetBinContent(1 + i + 64, present_countScaled[i]);

                  Gl1_present_rate->SetBinContent(1 + i, (float)present_countRaw[i] / time);
                  Gl1_present_rate->SetBinContent(1 + i + 32, (float)present_countLive[i] / time);
                  Gl1_present_rate->SetBinContent(1 + i + 64, (float)present_countScaled[i] / time);

                }

              delete p;
              return 1;
            }
        }
      else
        {
	  ostringstream msg;
          msg << "Could not find the scaler packet in scaler event!" ;
	  se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_ERROR, msg.str(),2);
        }

      return 0; // done with scaler event

    }

  // If we got this far, this is a data event
  // get the run number for use in Draw()

  // Get the GL1 trigger packet adrress

  Packet *p = event->getPacket(14001);

  if (!p)
    {
      ostringstream msg;
      msg << "Event has no Gl1 packet! skip it" ;
      se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_ERROR, msg.str(),3);
      event->identify();
      return 0;
    }

  time = time_now - runbegin_time;

  // Get the raw trigger values
  int rawtrigword = p->iValue(0, "RAWTRIG");
  int livetrigword = p->iValue(0, "LIVETRIG");
  int scaledtrigword = p->iValue(0, "SCALEDTRIG");
  
#ifdef FIX_BAD_LIVE_FIFO

  // fix bad live trigger fifo data for triggers 24-32
  //cout << " old live word = 0x" << hex << livetrigword << dec << endl; 
  livetrigword = livetrigword&0x00FFFFFF;
  for (int i = 24; i < 32; i++)
    {
      if (!se->OnlTrig()->get_lvl1_trig_bitmask_bybit(i))
	{
	  livetrigword |= ((0x1 << i) & rawtrigword);
      }
  }
  //cout << " raw = 0x" << hex << rawtrigword << " new live word = 0x" << livetrigword << dec << endl; 
  //cout << hex << " 0x"<< se->Trigger(1)<< endl; 

#endif

  // Now get the bunch crossing information
  int crossingcounter = p->iValue(0, "CROSSCTR");
  delete p;
  float Temp_Value_for_Fill;

  // Loop over the triggers and find out how many triggers fired 
  // for this event.

  int NumTriggersFired = 0; 
  for (int i = 0;i < 32;i++)
    {
      if (scaledtrigword & (1 << i)) 
	{
	  int ibin = crossingcounter + 1; // shift from 0-119 t0 1-120 (first bin is 1, not 0)
	  double cnt = GL1_AllCross[i]->GetBinContent(ibin) + 1;
	  GL1_AllCross[i]->SetBinContent(ibin,cnt);
          NumTriggersFired++; 
	}
    }

  // If the number of fired triggers is zero, there is a problem with the 
  // scaled trigger word - this is a SEVERE error and the DAQ needs to 
  // be stopped! Flag this for the drawing code....

  if (NumTriggersFired == 0)
    {
      NumBadScaledEvents++;
      Gl1_StorageHist->SetBinContent(37, NumBadScaledEvents);
      ostringstream msg;
      msg << " Event with scaled trigger word = 0 detected! Total = " << NumBadScaledEvents << " bad events." << endl;
      se->send_message(this, MSG_SOURCE_GL1, MSG_SEV_FATAL, msg.str(), 4);
      SetStatus(OnlMon::ERROR); // set subsystem status
    }

  // also check the live and raw words

  if(livetrigword==0){
    ostringstream msg;
    msg << " Event with live trigger word = 0 detected!" << endl;
    se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),5);
  }

  if(rawtrigword==0){
    ostringstream msg;
    msg << " Event with raw trigger word = 0 detected!" << endl;
    se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),6);
  }

  // Demand consistency between the trigger words!
  
  if( (rawtrigword&livetrigword)!=livetrigword ){
    NumRawNELiveEvents++;
    Gl1_StorageHist->SetBinContent(40, NumRawNELiveEvents);
    ostringstream msg;
    msg << " Mismatched Raw and Live trigger words, raw = 0x" << hex << rawtrigword 
	<< " live = 0x " << livetrigword << endl;
    se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),7);
  }

  if( ((scaledtrigword&livetrigword)!=scaledtrigword) ){
    NumBadScalLiveEvents++;
    Gl1_StorageHist->SetBinContent(38, NumBadScalLiveEvents);
    ostringstream msg;
    msg << " Mismatched Scaled and Live trigger words, scaled = 0x" << hex << scaledtrigword 
	<< " live = 0x " << livetrigword << endl;
    se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),8);
  }

  if( ((scaledtrigword&rawtrigword)!=scaledtrigword) ){
    NumBadScalRawEvents++;
    Gl1_StorageHist->SetBinContent(39, NumBadScalRawEvents);
    ostringstream msg;
    msg << " Mismatched Scaled and Raw trigger words, scaled = 0x" << hex << scaledtrigword 
	<< " raw = 0x " << rawtrigword << endl;
    se->send_message(this,MSG_SOURCE_GL1,MSG_SEV_SEVEREERROR, msg.str(),9);
  }

  // Now loop over the triggers and allocate the bandwidth.
  // Each trigger is "charged" a bandwidth equal to the event size 
  // divided by the number of scaled triggers that fired. 

  for (int i = 0;i < 32;i++)
    {

      if (rawtrigword & (1 << i))
        {
          Temp_Value_for_Fill = Gl1_RawHist->GetBinContent(i + 1) + 1.0;
          Gl1_RawHist->SetBinContent(i + 1, Temp_Value_for_Fill);
        }

      if (livetrigword & (1 << i) )
        {
          Temp_Value_for_Fill = Gl1_LiveHist->GetBinContent(i + 1) + 1.0;
          Gl1_LiveHist->SetBinContent(i + 1, Temp_Value_for_Fill);
	}

      if (scaledtrigword & (1 << i) )
        {
          Temp_Value_for_Fill = Gl1_ScaledHist->GetBinContent(i + 1) + 1.0;
          Gl1_ScaledHist->SetBinContent(i + 1, Temp_Value_for_Fill);
          trigsize[i] += (float)event_size/(float)NumTriggersFired;
	}

      // The next two histograms keep track of "if trig X (scaled) fired, what 
      // other triggers also fired?"  These can be used to get the rejection relative
      // to trigger X.

      if ( (livetrigword & (1 << i)) && (scaledtrigword & BBCLL1_bit_mask) )
        {
          Temp_Value_for_Fill = Gl1_ScaledHist_BBCLL1->GetBinContent(i + 1) + 1.0;
          Gl1_ScaledHist_BBCLL1->SetBinContent(i + 1, Temp_Value_for_Fill);
        }

      if ( (livetrigword & (1 << i)) && (scaledtrigword & ZDC_bit_mask) )
        {
          Temp_Value_for_Fill = Gl1_ScaledHist_ZDC->GetBinContent(i + 1) + 1.0;
          Gl1_ScaledHist_ZDC->SetBinContent(i + 1, Temp_Value_for_Fill);
        }

      Gl1_StorageHist->SetBinContent(i + 5, trigsize[i]);

    }


  // This is a BBCLL1 trigger
  if (livetrigword & BBCLL1_bit_mask)
    {
      Gl1_CrossCounter_bbcll1->Fill(crossingcounter);
    }


  // Is this a ZDC trigger?
  if (livetrigword & ZDC_bit_mask)
    {
      Gl1_CrossCounter_zdc->Fill(crossingcounter);
    }

  // Is this an bbcll1 no vtx cut trigger?
  if (livetrigword & BBCLL1_NOVERTEXCUT_bit_mask)
    {
      Gl1_CrossCounter_bbcll1_nvc->Fill(crossingcounter);
    }

  last_time = time_now;

  // Update DB with run quality flags

  //OnlMonServer *se = OnlMonServer::instance();
  if(se->CurrentTicks()> (LastCommit + 600)){  // 10 minutes


    dbvars->SetVar("GL1_NumBadScaledEvents", NumBadScaledEvents, 0, (float) evtcnt);
    dbvars->SetVar("GL1_NumRawNELiveEvents", NumRawNELiveEvents, 0, (float) evtcnt);
    dbvars->SetVar("GL1_NumBadScalLiveEvents", NumBadScalLiveEvents, 0, (float) evtcnt);
    dbvars->SetVar("GL1_NumBadScalRawEvents", NumBadScalRawEvents, 0, (float) evtcnt);

    dbvars->DBcommit();

    LastCommit = se->CurrentTicks(); 

  }

  return 0;
}



int Gl1Mon::DBVarInit()
{
  // variable names are not case sensitive
  string varname;
  varname = "GL1_NumBadScaledEvents";
  dbvars->registerVar(varname);
  varname = "GL1_NumRawNELiveEvents";
  dbvars->registerVar(varname);
  varname = "GL1_NumBadScalRawEvents";
  dbvars->registerVar(varname);
  varname = "GL1_NumBadScalLiveEvents";
  dbvars->registerVar(varname);

  dbvars->DBInit();

  return 0; 

}

int 
Gl1Mon::EndRun(const int runno)
{
  // this is the scaled trigger =0x0 where the
  // scaled trigger just hangs
  // runs are not analyzable
  if (NumBadScaledEvents > 100)
    {
      OnlMonServer *se = OnlMonServer::instance();
      se->SetSubsystemStatus(this,-1);
    }
  return 0;
}

