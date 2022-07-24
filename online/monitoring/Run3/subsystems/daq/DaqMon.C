#include "DaqMon.h"
#include "GranuleMon.h"
#include "GranuleMonDefs.h"
#include "DaqHistoBinDefs.h"

#include <OnlMonDB.h>
#include <OnlMonServer.h>
#include <OnlMonTrigger.h>

#include <Event.h>
#include <packet_gl1.h>
#include <msg_profile.h>

#include <phool.h>

#include <TH1.h>
#include <Cframe.h>
#include <framePublic.h>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

std::map<FrameTypes,std::string> frametypes;

DaqMon *DaqMon::__instance = NULL;

DaqMon *DaqMon::instance(const char *name)
{
  if (__instance)
    {
      return __instance;
    }
  __instance = new DaqMon(name);
  return __instance;
}

DaqMon::DaqMon(const char *name): 
  OnlMon(name),
  multibufferedTicks(1200)
{
  dbvars = new OnlMonDB(ThisName);
  DBVarInit();
  double hilim = DAQNUMBINS - 0.5;
  OnlMonServer *se = OnlMonServer::instance();
  DaqMonStatus = new TH1F("Daq_MonStatus", "DaqMonStatus", DAQNUMBINS, -0.5, hilim);
  se->registerHisto(this,DaqMonStatus);
  TimeToLastEvent[0] = new TH1F("Daq_TimeToLastEvent0", "Time to last Event enlarged", 131, -0.5, 130.5);
  TimeToLastEvent[1] = new TH1F("Daq_TimeToLastEvent1", "Time to last Event", 2001, 1, 10001);
  TimeToLastEvent[2] = new TH1F("Daq_TimeToLastEvent2", "Time to 2nd last Event", 2001, 1, 20001);
  TimeToLastEvent[3] = new TH1F("Daq_TimeToLastEvent3", "Time to 3rd last Event", 2001, 1, 30001);
  for (int i = 0; i < 4;i++)
    {
      se->registerHisto(this,TimeToLastEvent[i]);
    }
  FrameStatus = new TH1F("Daq_FrameStatus","Frame Status (Event ok)",17,-0.5,16.5);
  se->registerHisto(this,FrameStatus);
  daq_bcross  = new TH1F("Daq_bcross","Bunch Crossing BBCLL1",120,-0.5,119.5);
  se->registerHisto(this,daq_bcross);
  frametypes[dcmFrame] = "dcm";
  frametypes[dcbFrame] = "dcb";
  frametypes[sebFrame] = "seb";
  frametypes[atpFrame] = "atp";

  errorcodes[InvalidFrameHeader] = "InvalidFrameHeader";
  errorcodes[InvalidSourceId]    = "InvalidSourceId";
  errorcodes[InvalidFrameType]   = "InvalidFrameType";
  memset(GranuleArray,0,sizeof(GranuleArray));
  return ;
}

DaqMon::~DaqMon()
{
  while(Granule.begin() != Granule.end())
    {
      delete Granule.back();
      Granule.pop_back();
    }
  delete dbvars;
  __instance=0;
}

int
DaqMon::Init()
{
  last_gl1_clock = -1;
  gl1exists = 0;
  eventnumber = -9999;
  gl1clockdiff = -1;
  gl1_clock_counter = -1;
  memset(gl1_granule_counter, 0, sizeof(gl1_granule_counter));
  memset(granule_active,0,sizeof(granule_active));
  daqmonevts = 0;
  abortgapchkevts = 0;
  totalevents = 0;
  previous_corruptevts = 0;
  corruptevts = 0;
  missingpktevts = 0;
  badframestatusevts = 0;
  clkdiff_to_previous_event = 0xFFFFFFFF;
  bbcll1_trigmask = 0;
  memset(bbcll1_bcross_array,0,sizeof(bbcll1_bcross_array));
  run_has_gl1 = 0;
  eventiscorrupt = 0;
  eventmisspkts = 0;
  eventbadframestatus = 0;
  mark_as_bad = 0;
  memset(framestatusbits,0,sizeof(framestatusbits));
  sourceid_granulemap_initialized = 0;
  granulemap.clear();
  pcffiles.clear();
  sourceid_granulemap.clear();
  ismultieventbuffered = 0;
  return 0;
}

int DaqMon::Reset()
{
  int iret = 0;
  Init();
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      iret += granule->Reset();
    }
  return iret;
}

int DaqMon::BeginRun(const int runno)
{
  int iret = 0;
  Init();
  OnlMonServer *se = OnlMonServer::instance();
  OnlMonTrigger *onltrig = se->OnlTrig();
  string triggername;
  for (int i = 0; i < 32; i++)
    {
      triggername = onltrig->get_lvl1_trig_name(i);
      if (triggername.find("BBCLL1(>") != string::npos && triggername.find("tubes)") != string::npos && triggername.find("novertex") == string::npos)
	{
	  bbcll1_trigmask |= onltrig->getLevel1Bit(triggername);
	}
    }
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      iret += granule->BeginRun(runno);
    }
  return iret;
}

int DaqMon::registerMonitor(GranuleMon *granule)
{
  int iret = granule->Init();
  if (iret)
    {
      ostringstream errmsg;
      errmsg << " DaqMon::registerMonitor(GranuleMon *granule): Error initializing granule "
	     << granule->Name() << ", return code: " << iret ;
      send_message(MSG_SEV_ERROR, errmsg.str(), 1);
      return iret;
    }
  Granule.push_back(granule);
  int index = granule->GranuleNum();
  if (index < 0 || index > 31)
    {
      printf("DaqMon::registerMonitor: Granule %s granuleid %d out of range\n",granule->Name(),index);
    }
  else
    {
      GranuleArray[granule->GranuleNum()] = granule;
    }
  return 0;
}

int DaqMon::process_event(Event *e)
{
  OnlMonServer *se = OnlMonServer::instance();
  eventnumber = e->getEvtSequence();
  totalevents++;
  DaqMonStatus->SetBinContent(DAQTOTALEVENTBIN, totalevents);
  if (e->getEvtType() != 1)
    {
      if (verbosity > 0)
        {
          cout << "DaqMon::process_event: ditching EvtType: " << e->getEvtType() << endl;
        }
      return 0;
    }
  daqmonevts++; // analysed events
  DaqMonStatus->SetBinContent(DAQACCEVENTBIN, daqmonevts);
  // Test if there were some corrupt events since the last call
  if (se->BadEvents())
    {
      ostringstream errmsg;
      errmsg << "<E> Framework event check removed " << se->BadEvents()
	     << " corrupt events from monitoring" << endl;
      send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 2);
      corruptevts += se->BadEvents();
      DaqMonStatus->SetBinContent(DAQCORRUPTEVENTBIN, corruptevts);
      daqmonevts += se->BadEvents();
      se->BadEvents(0);
    }
  if (DcmCheckSumCheck(e))
    {
      EventIsCorrupt();
    }
  if (FrameStatusCheck(e))
    {
      EventHadBadFrameStatus();
    }
  // Test for GL1 data and fill global variables
  Packet *p = e->getPacket(14001);
  if (!p)
    {
      gl1exists = 0;
    }
  else
    {
      gl1exists = 1;
      run_has_gl1 = 1;
      gl1_clock_counter = (p->iValue(0, BEAMCTR0) & 0xFFFF); // BEAMCTR0 defined in packet_gl1.h
      for (int k = 0;k < 32;k++)
        {
          gl1_granule_counter[k] = p->iValue(k, GRANCTR); // GRANCTR defined in packet_gl1.h
	  if (gl1_granule_counter[k] > 0)
	    {
	      if (!granule_active[k])
		{
		  granule_active[k] = 1;
		}
	      if (gl1_granule_counter[k] != (((unsigned int) eventnumber) & 0xFFFF))
		{
		  if (verbosity > 0)
		    {
		  cout << "Granule counter problem for Granule " << k 
		       << " counter: " << gl1_granule_counter[k]
		       << " event num: " << (eventnumber & 0xFFFF) << dec
		       << endl;
		    }
		}
	    }
        }
      // check for abort gap for a few thousand events
      if (se->Trigger() & bbcll1_trigmask)
	{
	  abortgapchkevts++;
          int crossingcounter = p->iValue(0, "CROSSCTR");

	  if (crossingcounter < 0 || crossingcounter > 119)
	    {
	      cout << "bad crossing counter: " << crossingcounter << endl;
	    }
	  else
	    {
	      bbcll1_bcross_array[crossingcounter]++;
	      daq_bcross->SetBinContent((crossingcounter+1),bbcll1_bcross_array[crossingcounter]);
	    }
	  if (abortgapchkevts%500 == 0)
	    {
	      double aveabort = 0.;
	      double avenoabort = 0.;
	      double nbcross = 0;
	      for (int i = 106; i <= 114; i++)
		{
		  aveabort += bbcll1_bcross_array[i];
		  nbcross++;
		}
	      aveabort /= nbcross;
              aveabort /= daqmonevts;
	      nbcross = 0;
	      for (int i = 0; i < 105; i++)
		{
		  avenoabort += bbcll1_bcross_array[i];
		  nbcross++;
		}
	      avenoabort += bbcll1_bcross_array[114];
	      avenoabort += bbcll1_bcross_array[115];
	      avenoabort += bbcll1_bcross_array[117];
	      avenoabort += bbcll1_bcross_array[118];
	      avenoabort += bbcll1_bcross_array[119];
	      nbcross += 4;
	      avenoabort /= nbcross;
              avenoabort /= daqmonevts;
	      // under normal circumstances, aveabort is zero - allowing up to 1% of counts compared to
	      // the average is very generous
	      // if the abort gap is shifted, the avenoabort is averaged over the abort gap
	      // and actually smaller than aveabort. Doing the 1% cut just compensates if the
	      // bunch luminosity is drastically different or we are running with few bunches only
	      //printf("average: %f, limit: %f\n",aveabort,avenoabort/5.);
	      if (aveabort > avenoabort/5.)
		{
		  ostringstream errmsg;
		  errmsg << "Abort Gap shifted, reload GL1 before starting next run";
		  send_message(MSG_SEV_ERROR, errmsg.str(), 5);
		  DaqMonStatus->SetBinContent(DAQBADABORTGAP,1);
		}
	      else
		{
		  DaqMonStatus->SetBinContent(DAQBADABORTGAP,0);
		}
	    }
	}
      delete p;
      if (last_gl1_clock >= 0)
        {
          int tmp1 = last_gl1_clock & 0xFF;    // use only lower 2 bytes like FEM ctrs
          int tmp2 = gl1_clock_counter & 0xFF; // use only lower 2 bytes like FEM ctrs
          gl1clockdiff = tmp1 - tmp2;
          if (gl1clockdiff < 0)
            {
              gl1clockdiff += 256;
            }
        }
    }
  clkdiff_to_previous_event = 0xFFFFFFFF;
  unsigned int prevevt[3];
  memset(prevevt, 0, sizeof(prevevt));
  // for the first - fourth event in a run packet 14009 is useless
  // since whatever it contains for the history can lead to a bogus
  // "forbidden" gl1 clock diff
  if (eventnumber > 5)
    {
      Packet *pkt14009 = e->getPacket(14009);
      if (pkt14009)
	{
	  int parvect = pkt14009->iValue(0, "PARVECT");
	  unsigned int currevt = pkt14009->iValue(0, "EVCLOCK");
	  int prevcnt = 0;
	  for (int i = 1; i < 4; i++) // there are 4 events kept, we start at 1 bc the current one is index 0
	    {
	      // check if the partition vector is identical (previous evt can be
	      // from another partition - we do not want to compare to that one)
	      if (parvect == pkt14009->iValue(i, "PARVECT"))
		{
		  prevevt[prevcnt] = pkt14009->iValue(i, "EVCLOCK");
		  prevcnt++;
		}
	    }
	  for (int i = 0; i < prevcnt; i++)
	    {
	      if (currevt >= prevevt[i])
		{
		  clkdiff_to_previous_event = currevt - prevevt[i];
		}
	      else
		{
		  clkdiff_to_previous_event = currevt + (0xFFFFFFFF - prevevt[i]) + 1;
		}
	      if (i == 0)

		{
		  TimeToLastEvent[0]->Fill(clkdiff_to_previous_event);
		}
	      TimeToLastEvent[i+1]->Fill(clkdiff_to_previous_event);
	      if (clkdiff_to_previous_event < 16)
		{
		  ostringstream errmsg;
		  errmsg << "Clock Tick Diff between 2 events too small: " << clkdiff_to_previous_event;
		  send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 3);
		}
	      if (!ismultieventbuffered)
		{
                if (clkdiff_to_previous_event < multibufferedTicks)
                  {
                    DaqMonStatus->SetBinContent(DAQNOMULTIBUFFER, 0);
                    ismultieventbuffered = 1;
                  }
                else if (totalevents > 1000)
                  {
                    DaqMonStatus->SetBinContent(DAQNOMULTIBUFFER, 1);
                  }
              }

	    }
	}
      delete pkt14009;
    }
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      granule->process_event(e);
    }
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      granule->ResetEvent();
    }
  if (eventiscorrupt)
    {
      corruptevts++;
      DaqMonStatus->SetBinContent(DAQCORRUPTEVENTBIN, corruptevts);
      eventiscorrupt = 0;
    }
  if (eventmisspkts)
    {
      missingpktevts++;
      DaqMonStatus->SetBinContent(DAQMISSINGPACKETEVENTBIN, missingpktevts);
      eventmisspkts = 0;
    }
  if (eventbadframestatus)
    {
      badframestatusevts++;
      DaqMonStatus->SetBinContent(DAQBADFRAMESTATUSBIN,badframestatusevts);
      eventbadframestatus = 0;
    }
  if (gl1exists)
    {
      last_gl1_clock = gl1_clock_counter;
    }
  else
    {
      last_gl1_clock = -1;
    }
  return 0;
}

void DaqMon::identify(ostream& out) const
{
  out << "DaqMon Object" << endl;
  out << "Currently registered Granule Monitors:" << endl;
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      granule->identify(out);
    }
  return ;
}

int
DaqMon::DBVarInit()
{
  string varname;
  varname = "daqcorrupt";
  dbvars->registerVar(varname);
  varname = "daqmisspkt";
  dbvars->registerVar(varname);
  varname = "daqframstat";
  dbvars->registerVar(varname);
  dbvars->DBInit();
  return 0;
}

int
DaqMon::FrameStatusCheck(Event *evt)
{
  int status = evt->getFrameEntry("FRAMESTATUS", 0, 0);
  if (!status)
    {
      framestatusbits[0]++;
      FrameStatus->SetBinContent(1, framestatusbits[0]);
    }
  else
    {
      fill_sourceid_granulemap(); // fill source id info in sourceid_granulemap (check for done once in method)
      ostringstream errmsg;
      unsigned int iatp = evt->getFrameEntry("FRAMESOURCEID", 0, 0);
      //      OnlMonServer *se = OnlMonServer::instance();
//       cout << "DaqMon::FrameStatusCheck(): run: " << se->RunNumber() << " event: " << eventnumber << " status: 0x" << hex << status
//            << " Atp ID: 0x" << iatp << dec;
      //    send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 7);
      int i = 0;
      PHDWORD* frame_ptr = new PHDWORD[evt->getEvtLength()];
      int ncopied = 0;
      int copySuccess = evt->Copy((int*)frame_ptr, evt->getEvtLength(), &ncopied, "DATA");
      if (copySuccess == 0)
        {
          PHDWORD* err_ptr = findFrameErrorStart(frame_ptr);
          UINT len = getFrameErrorLength(frame_ptr);

          int nentries = calcNumErrorsV1(len);
	  if (nentries <= 0)
	    {
              errmsg.str("");
	      errmsg << " Atp ID: 0x" << hex << iatp << dec << " status: " << status
                     << " error block entries: " << nentries << ", error blk length: " << len;
              send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 7);
	    }
          errorEntryV1* entry_ptr = (errorEntryV1*)err_ptr;
          for (int n = 0; n < nentries; n++, entry_ptr++)
            {
              // get the granule which belongs to the source id
              int granid = -1;
              if (entry_ptr->deviceType == 2)
                {
                  map<unsigned int, int>::const_iterator iter = sourceid_granulemap.find(entry_ptr->deviceId);
                  if (iter != sourceid_granulemap.end())
                    {
                      if (iter->second >= 0 && iter->second < 32)
                        {
                          if (GranuleArray[iter->second])
                            {
//                            cout << "Granule for source id 0x" << hex << entry_ptr->deviceId << dec
//                                 << " is " << iter->second << " name: "
//                                 << GranuleArray[iter->second]->Name() << endl;
                              GranuleArray[iter->second]->IncrementEvbError();
                              granid = iter->second;
                            }
                        }
                    }
                }
              errmsg.str("");
              if (granid >= 0)
                {
                  errmsg << " Granule: " << GranuleArray[granid]->Name();
                }
	      else
		{
                  errmsg << " Granule: Unknown";
		}
              errmsg << " Atp ID: 0x" << hex << iatp << dec << " Error entry: " << n <<  " status: " << status 
		     << " severity: " << (int) entry_ptr->severity << " "
		     << " deviceType: " << (int) entry_ptr->deviceType << " (" << frametypes[(FrameTypes)entry_ptr->deviceType] << ") "
		     << " deviceId: " << entry_ptr->deviceId << " (0x" << hex << entry_ptr->deviceId << dec << ") ";
              errmsg     << " errorCode: " << entry_ptr->errorCode << " (" << errorcodes[(daqErrorCodes)entry_ptr->errorCode] << ") "
			 << " detectCode: 0x" << hex << entry_ptr->detectCode << " (seb" <<entry_ptr->detectCode << ")" << dec;
	        send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 7);
            }
        }
      else
        {
              errmsg.str("");
	      errmsg << " Atp ID: 0x" << hex << iatp << dec <<  " status: " << status 
                     << " copying of data failed";
              send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 7);

        }
      delete [] frame_ptr;
      // decode bits
      while (status)
        {
          i++;
          if (status & 0x1)
            {
              framestatusbits[i]++;
              FrameStatus->SetBinContent(i + 1, framestatusbits[i]);
            }
          status = status >> 1;
        }
      return -1;
    }
  return 0;
}

int
DaqMon::DcmCheckSumCheck(Event *e)
{
  int nw = e->getPacketList(plist, 10000);
  int iret = 0;
  if (nw >= 10000)
    {
      string errmsg = "DaqMon::DcmCheckSumCheck(Event *e) Packet list too small, aborting Event";
      send_message(MSG_SEV_SEVEREERROR, errmsg, 4);
      for (int i = 0; i < 10000; i++)
        {
          delete plist[i];
        }
      return -1;
    }
  for (int i = 0; i < nw; i++)
    {
      int check = plist[i]->getCheckSumStatus();
      switch (check)
        {
        case 1:
          break;

        case 0:
          if (verbosity > 2)
            {
              ostringstream errmsg;
              errmsg << "DaqMon::DcmCheckSumCheck(): "
		     << "Event " << EventNumber()
		     << " Packet " << plist[i]->getIdentifier()
		     << " has no dcm chk sum" ;
              send_message(MSG_SEV_WARNING, errmsg.str(), 4);
            }
          break;

        case - 1:
          {
            ostringstream errmsg;
            errmsg << "DaqMon::DcmCheckSumCheck(): "
		   << "Event " << EventNumber()
		   << " Packet " << plist[i]->getIdentifier()
		   << " failed dcm chk sum";
            send_message(MSG_SEV_ERROR, errmsg.str(), 5);
          }
          iret = -1;
          break;
        default:
          {
            ostringstream errmsg;
            errmsg << "DaqMon::DcmCheckSumCheck(): "
		   << "Event " << EventNumber()
		   << " Packet " << plist[i]->getIdentifier()
		   << " has invalid dcm CheckSumStatus: " << check ;
            send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 6);
          }
          break;
        }
      delete plist[i];
    }
  return iret;
}

int
DaqMon::send_message(const int severity, const string &err_message, const int msgtype)
{
  OnlMonServer *se = OnlMonServer::instance();
  int iret = se->send_message(this, MSG_SOURCE_DAQMON, severity, err_message, msgtype);
  return iret;
}

int
DaqMon::EndRun(const int runno)
{
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      if (granule->TotalGl1SyncErrors() > 100)
	{
	  mark_as_bad = 1;
	}
    }
  if (mark_as_bad)
    {
      OnlMonServer *se = OnlMonServer::instance();
      se->SetSubsystemStatus(this, -1);
    }
  else if (badframestatusevts > 0)
    {
      float badframefract = badframestatusevts / (float)totalevents;
      OnlMonServer *se = OnlMonServer::instance();
      if (badframefract > 0.1)
	{
	  se->SetSubsystemStatus(this, -badframestatusevts);
	}
      else
	{
	  se->SetSubsystemStatus(this, badframestatusevts);
	}

  }

  if (daqmonevts > FIRSTUPDATE) // below  FIRSTUPDATE we don't get the histos anyway
    {
      float fraction = float(corruptevts) / ((float)daqmonevts);
      if (verbosity > 0)
        {
          cout << " number of corrupted events : " << corruptevts
	       << ", number of total evts: " << daqmonevts
          << ", fraction of corrupt evts: " << fraction
          << endl;
        }
      dbvars->SetVar("daqcorrupt", fraction, 0., corruptevts);
      fraction = float(missingpktevts) / ((float)daqmonevts);
      if (verbosity > 0)
        {
          cout << " number of missing pkts events : " << missingpktevts
          << ", number of total evts: " << daqmonevts
          << ", fraction of missing pkt evts: " << fraction
          << endl;
        }
      dbvars->SetVar("daqmisspkt", fraction, 0., missingpktevts);
      fraction = float(badframestatusevts) / ((float)daqmonevts);
      if (verbosity > 0)
        {
          cout << " number of events with bad frame status: " << badframestatusevts
          << ", number of total evts: " << daqmonevts
          << ", fraction of bad frame status evts: " << fraction
          << endl;
        }
      dbvars->SetVar("daqframstat", fraction, 0., badframestatusevts);
      dbvars->DBcommit();
    }
  return 0;
}

GranuleMon *
DaqMon::GetMonitor(std::string &name)
{
  BOOST_FOREACH(GranuleMon *granule, Granule)
    {
      if (granule->Name() == name)
	{
	  return granule;
	}
    }
  printf("Monitor %s not found\n",name.c_str());
  return NULL;
}

int
DaqMon::fill_granule_map()
{
  ifstream infile;
  infile.open("/export/software/oncs/online_configuration/GL1/.gl1_granule_names", ifstream::in);
  if (!infile)
    {
      ostringstream msg;
      msg << "fill_granule_map: Could not open /export/software/oncs/online_configuration/GL1/.gl1_granule_names";
      send_message(MSG_SEV_WARNING, msg.str(), 7);
      return -1;
    }
  string FullLine;	// a complete line in the config file
  getline(infile, FullLine);
  int granid;
  boost::char_separator<char> sep(" ");
  while ( !infile.eof())
    {
      boost::tokenizer<boost::char_separator<char> > tok(FullLine, sep);
      boost::tokenizer<boost::char_separator<char> >::iterator it = tok.begin();
      istringstream line;
      line.str(*it);
      line >> granid;
      ++it;
      granulemap[*it] = granid;
      getline(infile, FullLine);

    }
  infile.close();
//   for (map<std::string, int>::iterator ite = granulemap.begin(); ite != granulemap.end(); ite++)
//     {
//       cout << "granule " << ite->first << " has id " << ite->second << endl;
//     }
  return 0;
}


int
DaqMon::fill_sourceid_granulemap()
{
  if (sourceid_granulemap_initialized)
    {
      return 0;
    }
  fill_granule_map();
  OnlMonServer *se = OnlMonServer::instance();
  se->parse_granuleDef(pcffiles);
  string change_from_gran_names[3] = {"AGEL", "TOF_E", "TOF_W"};
  string change_to_gran_names[3] = {"AGEL.W", "TOF.E", "TOF.W"};
  const int n_changes = sizeof(change_to_gran_names)/sizeof(string);
  BOOST_FOREACH(string pcf_file,  pcffiles)
    {
      ostringstream filenam;
      if (getenv("ONLINE_CONFIGURATION"))
        {
          filenam << getenv("ONLINE_CONFIGURATION") << "/rc/hw/";
        }
      filenam << pcf_file;
      string FullLine;	// a complete line in the config file
      ifstream infile;
      infile.open(filenam.str().c_str(), ifstream::in);
      if (!infile)
	{
	  if (filenam.str().find("gl1test.pcf") == string::npos)
	    {
	      ostringstream msg;
	      msg << "fill_sourceid_granulemap: Could not open " << filenam.str();
	      send_message(MSG_SEV_WARNING, msg.str(), 7);
	    }
	  continue;
	}
      getline(infile, FullLine);
      string::size_type pos1;
      while ( !infile.eof())
        {
          if ((pos1 = FullLine.find("sourceid")) != string::npos)
	    {
              boost::char_separator<char> sep(" :");
	      boost::tokenizer<boost::char_separator<char> > tok(FullLine,sep);
	      //	      cout << FullLine << endl;
	      int granid = -1;
	      unsigned int sourceid = 0;
	      for (boost::tokenizer<boost::char_separator<char> >::iterator it=tok.begin(); it!=tok.end(); ++it)
		{
		  if ((*it).find("name") != string::npos)
		    {
		      ++it;
		      // it contains the dcm group names. Some groups have different names than their granule
		      // (AGEL ->AGEL.W, TOF_E -> TOF.E, TOF_W -> TOF.W), so I just replace the bad granule name 
		      // in the dcm group name by the real granule and then do the lookup
		      string tmpstring = *it;
		      for (int i = 0; i < n_changes; i++)
			{
			  pos1 = tmpstring.find(change_from_gran_names[i]);
			  if (pos1 != string::npos)
			    {
			      tmpstring.replace(pos1, change_from_gran_names[i].size(), change_to_gran_names[i]);
			    }
			}
		      for (map<string, int>::const_iterator iter = granulemap.begin(); iter != granulemap.end(); ++iter)
			{
			  if (tmpstring.find(iter->first) != string::npos)

			    {
			      granid = iter->second;
			    }
			}
		    }
		  if ((*it).find("sourceid") != string::npos)
		    {
		      ++it;
		      if (it == tok.end()) // just for safety, check if we don't overshoot
			{
			  printf("Warning, empty field after sourceid:\n");
			}
		      else
			{
			  // in the gl1 pcf file, the source id is a definition
			  if ((*it).find("DCM_GL1_SOURCE_ID") != string::npos)
			    {
			      sourceid = 0x1004;
			    }
			  else
			    {
			      if ((*it).size() > 6)
				{
				  printf("Warning %s does not have expected length of <7\n", (*it).c_str());
				}

			      //  cout << "source id is: " << *it << endl;
                        istringstream sid;
			sid.str(*it);
			sid >> hex >> sourceid;
			//			  cout << "source id parsed: 0x" << hex << sourceid << dec << endl;
			    }
			}
		    }
		}
	      // 	      if (sourceid_granulemap.find(sourceid) != sourceid_granulemap.end())
	      // 		{
	      // 		  printf("duplicate source id: 0x%x\n",sourceid);
	      // 		}
	      sourceid_granulemap[sourceid] = granid;
	    }
          getline( infile, FullLine );
        }
      infile.close();
    }
  sourceid_granulemap_initialized = 1;
  return 0;
}
