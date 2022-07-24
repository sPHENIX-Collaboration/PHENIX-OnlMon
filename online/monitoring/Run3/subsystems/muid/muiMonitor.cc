#include <OnlMonServer.h>
#include <muiOptions.h>
#include <muiMap.h>

#include <obj_hist.h>
#include <gl1Obj.h>
#include <gl1pObj.h>
#include <muiMonitor.h>

#include <mui_mon_const.h>
#include <origHist.h>
#include <twopackHist.h>
#include <corHist.h>
#include <timeHist.h>
#include <errorHist.h>
#include <muiMonGlobals.h>
#ifdef MUID_USE_BLT
#include <triggerHists.h>
#endif

#include <Event.h>
#include <EventTypes.h>
#include <msg_profile.h>
#include <phool.h>

#include <TH1.h>
#include <TFile.h>

#include <cmath>
#include <cstdlib>
#include <sstream>

using namespace std;

muiMonitor::muiMonitor(): OnlMon("MUIDMONITOR")
{
  opts = new muiOptions();
  hwMap = 0;
  gl1 = 0;
  gl1p = 0;

  total_hits = 0;
  memset(data_words, 0, sizeof(data_words));
  // references
  memset(ROCtotalREF_orig, 0, sizeof(ROCtotalREF_orig));
  memset(HVHitsREF_orig, 0, sizeof(HVHitsREF_orig));
  memset(CableHitsREF_orig, 0, sizeof(CableHitsREF_orig));
#ifdef MUID_USE_BLT
  trig = 0;
#endif
  return ;
}

//////////////////////////////////////////////////////
muiMonitor::~muiMonitor()
{
  delete hwMap;
  delete opts;
  delete gl1;
  delete gl1p;
#ifdef MUID_USE_BLT

  delete trig;
#endif

  while (hist_managers.begin() != hist_managers.end())
    {
      delete hist_managers.back();
      hist_managers.pop_back();
    }
  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      delete ROCtotalREF_orig[iarm];
      for (int iori = 0; iori < MAX_ORIENTATION; iori++)
        {
          delete HVHitsREF_orig[iarm][iori];
          delete CableHitsREF_orig[iarm][iori];
        }
    }
   muiMonGlobals* globals = muiMonGlobals::Instance();
   delete globals;
  return ;
}

//////////////////////////////////////////////////////
int
muiMonitor::Init()
{
  static int initialized = 0;

  if (initialized)
    {
      return 0 ;
    }
  else
    {
      initialized = 1;
    }

  printf(" initializing\n");

  hwMap = new muiMap(this);
  if (opts->useDB())
    {
      if (hwMap->fillFromDB() != 0)
        {
          ostringstream msg;
          if (hwMap->fillFromFile() != 0)
            {
              msg << "Hw map fill: DB failure and Text file failure";
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 1);
            }
          else
            {
              msg << "Hw map fill: DB failure, Text file filling worked";
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 1);
            }
        }
      if (opts->getVerbose() > 0)
        {
          printf(" Hardware Map Filled From DB\n");
        }
    }
  else
    {
      if (hwMap->fillFromFile() != 0)
        {
          ostringstream msg;
          OnlMonServer *se = OnlMonServer::instance();
          if (hwMap->fillFromDB() != 0)
            {
              msg << "Hw map fill: File and Db failure";
              se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 1);
            }
          else
            {
              msg << "Hw map fill: File failure, Db filling worked." ;
              se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 1);
            }
        }
      if (opts->getVerbose() > 0)
        {
          printf(" Hardware Map Filled From File\n") ;
        }
    }

  //create hist managers
  hist_managers.clear(); // unnecessary really, but it never hurts to do a real init
  obj_hist* origHists = new origHist(this);
  hist_managers.push_back(origHists);
  obj_hist* twopackHists = new twopackHist(this);
  hist_managers.push_back(twopackHists);
  obj_hist* corHists = new corHist(this);
  hist_managers.push_back(corHists);
  obj_hist* timeHists = new timeHist(this);
  hist_managers.push_back(timeHists);
  obj_hist* errorHists = new errorHist(this);
  hist_managers.push_back(errorHists);

  gl1 = new gl1Obj(this);
  gl1p = new gl1pObj();


  // get reference histograms
  int refstatus = getReferences();
  if (refstatus != 0)
    {
      ostringstream msg;
      OnlMonServer *se = OnlMonServer::instance();
      msg << " MUID retrieval of references failed !! ";
      se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 1);
    }
  return 0;
}

//////////////////////////////////////////////////////
int muiMonitor::getReferences()
{
  int nROC = MAX_ROC * MAX_ORIENTATION; // 40
  int nCable = 120;
  int nHV[2] = { 80, 70 }; // for the two orientations

  ostringstream name;
  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      name.str("");
      name << "MUI_ROCtotalREF" << iarm;
      ROCtotalREF_orig[iarm] = new TH1F((name.str()).c_str(), (name.str()).c_str(),
                                        nROC, 0.5, nROC + 0.5);
      ROCtotalREF_orig[iarm]->Sumw2();
      for (int iori = 0; iori < MAX_ORIENTATION; iori++)
        {
          name.str("");
          name << "MUI_HVHitsREF" << iarm << iori;
          HVHitsREF_orig[iarm][iori] = new TH1F((name.str()).c_str(), (name.str()).c_str(),
                                                nHV[iori], -0.5, nHV[iori] - 0.5);
          HVHitsREF_orig[iarm][iori]->Sumw2();
          name.str("");
          name << "MUI_CableHitsREF" << iarm << iori;
          CableHitsREF_orig[iarm][iori] = new TH1F((name.str()).c_str(), (name.str()).c_str(),
						   nCable, -0.5, nCable - 0.5);
          CableHitsREF_orig[iarm][iori]->Sumw2();
        }
    }

  // Get reference histograms
  TFile* file = 0L;
  if (getenv("MUIMONREF"))
    {
      file = new TFile(getenv("MUIMONREF"));
    }
  if ((!file) || (!file->IsOpen()))
    {
      ostringstream msg;
      msg << "Could not open reference file";
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 1);
      return 1;
    }

  HVHitsREF_orig[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ]->Add((TH1F*)file->Get("MUI_SH_HVhits"));
  HVHitsREF_orig[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT]->Add((TH1F*)file->Get("MUI_SV_HVhits"));
  HVHitsREF_orig[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ]->Add((TH1F*)file->Get("MUI_NH_HVhits"));
  HVHitsREF_orig[MUIMONCOORD::NORTH][MUIMONCOORD::VERT]->Add((TH1F*)file->Get("MUI_NV_HVhits"));

  CableHitsREF_orig[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ]->Add((TH1F*)file->Get("MUI_S_H_CABLE_RATE"));
  CableHitsREF_orig[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT]->Add((TH1F*)file->Get("MUI_S_V_CABLE_RATE"));
  CableHitsREF_orig[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ]->Add((TH1F*)file->Get("MUI_N_H_CABLE_RATE"));
  CableHitsREF_orig[MUIMONCOORD::NORTH][MUIMONCOORD::VERT]->Add((TH1F*)file->Get("MUI_N_V_CABLE_RATE"));

  TH1F* HitsPerROCtemp[MAX_ARM][MAX_ORIENTATION];
  HitsPerROCtemp[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ] = (TH1F*)file->Get("MUI_S_H_ROC_TOTALS");
  HitsPerROCtemp[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT] = (TH1F*)file->Get("MUI_S_V_ROC_TOTALS");
  HitsPerROCtemp[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ] = (TH1F*)file->Get("MUI_N_H_ROC_TOTALS");
  HitsPerROCtemp[MUIMONCOORD::NORTH][MUIMONCOORD::VERT] = (TH1F*)file->Get("MUI_N_V_ROC_TOTALS");

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int iroc = 0; iroc < MAX_ROC; iroc++)
        {
          ROCtotalREF_orig[iarm]->SetBinContent(iroc + 1,
                                                HitsPerROCtemp[iarm][MUIMONCOORD::HORIZ]->GetBinContent(iroc + 1));
          ROCtotalREF_orig[iarm]->SetBinContent(iroc + 1 + MAX_ROC,
                                                HitsPerROCtemp[iarm][MUIMONCOORD::VERT]->GetBinContent(iroc + 1));
          ROCtotalREF_orig[iarm]->SetBinError(iroc + 1,
                                              sqrt(HitsPerROCtemp[iarm][MUIMONCOORD::HORIZ]->GetBinContent(iroc + 1)));
          ROCtotalREF_orig[iarm]->SetBinError(iroc + 1 + MAX_ROC,
                                              sqrt(HitsPerROCtemp[iarm][MUIMONCOORD::VERT]->GetBinContent(iroc + 1)));
        }
    }

  float scalefactor = (100. / ((TH1F*)file->Get("MUI_NumEvent"))->GetEntries());

  // now scale all the histograms
  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      ROCtotalREF_orig[iarm]->Scale(scalefactor);
      //      ROCtotalREF_orig[iarm]->Print();
      for (int iori = 0; iori < MAX_ORIENTATION; iori++)
        {
          HVHitsREF_orig[iarm][iori]->Scale(scalefactor);
          CableHitsREF_orig[iarm][iori]->Scale(scalefactor);
          //	  HVHitsREF_orig[iarm][iori]->Print();
          //	  CableHitsREF_orig[iarm][iori]->Print();
        }
    }

  file->Close();
  delete file;

  OnlMonServer *se = OnlMonServer::instance();
  // finally, now register them all with the server..
  // not the original versions; these will be overwritten! so, register a copy of them
  TH1* ROCtotalREF[MAX_ARM];
  TH1* HVHitsREF[MAX_ARM][MAX_ORIENTATION];
  TH1* CableHitsREF[MAX_ARM][MAX_ORIENTATION];

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      ROCtotalREF[iarm] = (TH1 *) ROCtotalREF_orig[iarm]->Clone();
      se->registerHisto(this,ROCtotalREF[iarm]);

      for (int iori = 0; iori < MAX_ORIENTATION; iori++)
        {
          HVHitsREF[iarm][iori] = (TH1 *) HVHitsREF_orig[iarm][iori]->Clone();
          se->registerHisto(this,HVHitsREF[iarm][iori]);
          CableHitsREF[iarm][iori] = (TH1 *) CableHitsREF_orig[iarm][iori]->Clone();
          se->registerHisto(this,CableHitsREF[iarm][iori]);
        }
    }
  //  se->dumpHistos("dump.root");
  // done
  return 0;
}

//////////////////////////////////////////////////////
muiOptions* muiMonitor::getOptions()
{
  return opts;
}

//////////////////////////////////////////////////////
int muiMonitor::clear_hist()
{
  muiMonGlobals* globals = muiMonGlobals::Instance();
  globals->ClearRun();
  total_hits = 0;

  //loop over hist_managers
  vector<obj_hist*>::iterator hist_iter;
  for (hist_iter = hist_managers.begin();hist_iter != hist_managers.end();hist_iter++)
    {
      (*hist_iter)->clear();
    }
  gl1->clear();
  gl1p->clear();
  return 0;
}

//////////////////////////////////////////////////////
void muiMonitor::analyze_hist()
{
  vector<obj_hist*>::iterator hist_iter;
  for (hist_iter = hist_managers.begin();hist_iter != hist_managers.end();hist_iter++)
    {
      (*hist_iter)->analyze();
    }
  gl1->analyze();
  gl1p->analyze();
}

//////////////////////////////////////////////////////
int muiMonitor::get_eventNum()
{
  muiMonGlobals* globals = muiMonGlobals::Instance();
  return globals->eventNum;
}

//////////////////////////////////////////////////////
int muiMonitor::fill_hist()
{
  vector<obj_hist*>::iterator hist_iter;
  for (hist_iter = hist_managers.begin();hist_iter != hist_managers.end();hist_iter++)
    {
      (*hist_iter)->event(rawHits);
    }
  return 0;
}

//////////////////////////////////////////////////////
void muiMonitor::analyze_event()
{
  vector<obj_hist*>::iterator hist_iter;
  for (hist_iter = hist_managers.begin();hist_iter != hist_managers.end();hist_iter++)
    {
      (*hist_iter)->analyzeEvent();
    }
}

//////////////////////////////////////////////////////
int
muiMonitor::process_event( Event * e)
{


  int evt_type = e->getEvtType();
  ostringstream msg;

  if ( evt_type == DATAEVENT )
    {
      process_data( e );
    }
  else
    {
      if (evt_type != SCALEREVENT && evt_type != REJECTEDEVENT
          && evt_type != ENDRUNEVENT && evt_type != BEGRUNEVENT)
        {
          if ( opts->getVerbose() >= 0 )
            {
              msg.str("");
              msg << "  Unknown event type " << evt_type ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 1);
            }
        }
    }

  return 0;
}

//////////////////////////////////////////////////////
void muiMonitor::process_data( Event * e)
{
  muiMonGlobals* globals = muiMonGlobals::Instance();

  globals->eventNumAll++;
  ostringstream msg;
  // read GL1 packet
  if ( !gl1->event( e ) && opts->getVerbose() >= 5 )
    {
      printf("GL1 was read\n") ;
    }

  // read GL1P packet
  if ( !gl1p->event( e, gl1->getCrossctr() ) && opts->getVerbose() >= 5 )
    {
      printf("GL1P was read\n") ;
    }

#ifdef MUID_USE_BLT
  // read MuID packet and fill hit arrays BEFORE TRIGGER SELECTION!!
  if ( !fill_var( e ) && opts->getVerbose() >= 5 )
    {
      printf("Event  was read\n") ;
    }

  // Call Nobu's BLT emulator for every event - he does his own event selection to boost statistics.
  trig->event(rawHits, gl1->getLivetrig(), gl1->getScaledtrig());
#endif

  globals->eventNum++; //Only count events which pass trigger cut
  if ( opts->getVerbose() > 0 )
    {
      printf("Selected Event = %ld\n", globals->eventNum) ;
    }

  // if we have explicitly selected clock triggers Chris advises (for completeness)
  // to verify that the live trigger does not have any other bit sets
  // i.e. that the clock triggers do not overlap with any interaction triggers
  if ( opts->get_trigger() == CLOCK_TRIGGER_BIT )
    {
      if ( gl1->getLivetrig() != opts->get_trigger() )
        {

          msg.str("");
          msg << PHWHERE << " Clock trigger Event has other live bits set also; livetrig = 0x"
	      << hex << gl1->getLivetrig() << dec
	      << " - skipping event " ;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_INFORMATIONAL, msg.str(), 2);

          return;
        }
    }

#ifdef MUID_USE_BLT
  // do nothing, unpacking already done..
#else
  // read MuID packet and fill hit arrays AFTER TRIGGER SELECTION!!
  if ( !fill_var( e ) && opts->getVerbose() >= 5 )
    {
      printf("Event  was read\n") ;
    }
#endif
  int nraw_hits = (int) rawHits.size();
  if ( nraw_hits > opts->get_max_mult() )
    {
//       msg.str("");
//       msg << PHWHERE << " Event has too many hits : " << nraw_hits
// 	  << " is more than " << opts->get_max_mult()
// 	  << " - skipping event " ;
//       OnlMonServer *se = OnlMonServer::instance();

//       se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 2);
      return ;
    }
  else if ( nraw_hits < opts->get_min_mult() )
    {
//       msg.str("");
//       msg << PHWHERE << " Event has too few hits : " << nraw_hits
// 	  << " is less than " << opts->get_min_mult()
// 	  << " - skipping event " ;
//       OnlMonServer *se = OnlMonServer::instance();

//       se->send_message(this, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 3);
      return ;
    }

  total_hits += rawHits.size();

  // fill histograms using the hit array.
  if ( !fill_hist() && opts->getVerbose() >= 5 )
    {
      printf("Histograms were filled\n") ;
    }

  analyze_event();

}

/////////////////////////////////////////////////////////////////////////////
int
muiMonitor::BeginRun(const int runNum)
{
#ifdef MUID_USE_BLT
  static int first = 1;
  if (first)
    {
      trig = new triggerHists();
      first = 0;
    }
#endif
  ostringstream msg;
  if ( opts->getVerbose() >= 0 )
    {
      printf(" Run # %d Started\n", runNum) ;
    }
  if ( opts->getAutoClearHists() )
    {
      clear_hist();
    }

  OnlMonServer *se = OnlMonServer::instance();
  // since all histograms are resetted at the end of a run
  // we loose the reference histograms and they have to be refilled from
  // a local copy
  // for the first run they are not resetted and contain already
  // data, so the integral is > 1 and we don't add the local copy


  // opts is what we use for the event selection. save the info in
  // a gl1 histogram also..
  // use long since previous authors seemed to like that..
  gl1->setTrigMask( se->ScaledTrigMask() );

  ostringstream title;
  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      // ROC references
      title.str("");
      title << "MUI_ROCtotalREF" << iarm;
      TH1 *hROC = se->getHisto((title.str()).c_str());
      if (hROC->Integral() < 1) // double-> do not check == 0
        {
          hROC->Add(ROCtotalREF_orig[iarm], 1.);
        }
      for (int iori = 0; iori < MAX_ORIENTATION; iori++)
        {
          title.str("");
          title << "MUI_HVHitsREF" << iarm << iori;
          TH1 *hHV = se->getHisto((title.str()).c_str());
          if (hHV->Integral() < 1) // double-> do not check == 0
            {
              hHV->Add(HVHitsREF_orig[iarm][iori], 1.);
            }

          title.str("");
          title << "MUI_CableHitsREF" << iarm << iori;
          TH1 *hCable = se->getHisto((title.str()).c_str());
          if (hCable->Integral() < 1) // double-> do not check == 0
            {
              hCable->Add(CableHitsREF_orig[iarm][iori], 1.);
            }
        }
    }
  //
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
int
muiMonitor::EndRun(const int run_number)
{
  ostringstream msg;
  if ( opts->getVerbose() >= 0 )
    {
      printf(" Run # %d Ended\n", run_number) ;
      run_summary();
    }

  error_summary();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
void muiMonitor::run_summary()
{
  muiMonGlobals* globals = muiMonGlobals::Instance();

  ostringstream msg;
  if ( globals->eventNum > 0 )
    {
      printf("Total selected events = %ld Total hits = %ld rate = %f.2\n", globals->eventNum, total_hits, (float)total_hits / globals->eventNum);

      printf(" Rejected Events = %ld All events = %ld\n", globals->eventNumRejected, globals->eventNumAll);
    }
  else
    {
      printf(" - no event filled") ;
    }
}

/////////////////////////////////////////////////////////////////////////////
void muiMonitor::error_summary()
{
  muiMonGlobals* globals = muiMonGlobals::Instance();

  if ( opts->getVerbose() > 0 )
    {
      printf("%d South ROC OK Errors\n", globals->rocOkErrors[0]) ;
      printf("%d South Beam OK Errors\n", globals->beamOkErrors[0]);
      printf("%d South No MuID Packet Errors\n", globals->noPacketErrors[0]);
      printf("%d South Dead Cable Errors\n", globals->deadCableErrors[0]);
      printf("%d South Dead Main Frame Errors\n", globals->mainFrameErrors[0]);
      printf("%d North ROC OK Errors\n", globals->rocOkErrors[1]);
      printf("%d North Beam OK Errors\n", globals->beamOkErrors[1]);
      printf("%d North No MuID Packet Errors\n", globals->noPacketErrors[1]);
      printf("%d North Dead Cable Errors\n", globals->deadCableErrors[1]);
      printf("%d North Dead Main Frame Errors\n", globals->mainFrameErrors[1]);
    }

  // flag "beam OK errors" in the database also
  if ( globals->beamOkErrors[0]>0 || globals->beamOkErrors[1]>0 ) {
    int beamOkCode = 0;
    if (globals->beamOkErrors[0]>0 ) beamOkCode = 1;

    if (globals->beamOkErrors[1]>0 ) beamOkCode += 2;
    OnlMonServer *se = OnlMonServer::instance();
    se->SetSubsystemStatus(this, -beamOkCode);
  }
}

