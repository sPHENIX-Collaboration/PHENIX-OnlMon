#include <OnlMonClient.h>
#include <OnlMonTrigger.h>

#include <ERTMaskMonCommon.h>
#include <ERTMaskMonDraw.h>
#include <ERTMaskMonNHit.h>
#include <ERTMaskMonRFactor.h>
#include <EMCalRichDecodeOnline.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>

using namespace std;
using namespace odbc;
using namespace ERTMaskMonCommon;

ERTMaskMonDraw::ERTMaskMonDraw(): OnlMonDraw("ERTMaskMon")
{
  m_run_number = -1;
  m_run_number_pre = -1;
  m_run_type = "";
  m_n_event = 0;
  m_n_event_analyzed = 0;

  extern TStyle* gStyle;
  m_globalStyle = gStyle;

  m_ertStyle = new TStyle("ertStyle", "ertStyle");
  m_ertStyle->SetCanvasColor(kWhite);
  m_ertStyle->SetPadColor(kWhite);
  m_ertStyle->SetFrameBorderMode(0);
  m_ertStyle->SetOptStat(0);
  m_ertStyle->SetOptFit(0);
  m_ertStyle->SetOptTitle(1);
  m_ertStyle->SetOptDate(0);
  m_ertStyle->SetOptFile(0);

  m_pave_info_msg  = new TPaveText(0.05, 0.12, 0.48, 0.02, "NDC");
  m_pave_error_msg = new TPaveText(0.52, 0.12, 0.95, 0.02, "NDC");

  m_globalStyle->cd();

  m_DECODE = new EMCalRichDecodeOnline();
  memset(m_rfactor,0,sizeof(m_rfactor));
  memset(m_nhit,0,sizeof(m_nhit));
}

ERTMaskMonDraw::~ERTMaskMonDraw()
{
   for (int i = 0;i < N_NHIT_HIST;i++) {
      delete m_nhit[i];
   }
   for (int i = 0;i < N_RF_PLOT;i++) {
      delete m_rfactor[i];
   }

   delete m_ertStyle;
   
   delete m_pave_info_msg;
   delete m_pave_error_msg;
   
   delete m_DECODE;
   return;
}

int ERTMaskMonDraw::Init()
{
  SetERTExpertDefault();

  ostringstream parafile;
  if (getenv("ERTMASKCALIBOUT"))
    {
      parafile << getenv("ERTMASKCALIBOUT");
    }
  else
    {
      parafile << "./";
    }
  m_DMUXout_dir = parafile.str();

  // you must set m_nhit[i] (i = 0...N_NHIT_HIST) to 0 or new...
  m_nhit[0] = new ERTMaskMonNHit(DET_EMC,  "4x4A");
  m_nhit[1] = new ERTMaskMonNHit(DET_EMC,  "4x4B");
  m_nhit[2] = new ERTMaskMonNHit(DET_EMC,  "4x4C");
  m_nhit[3] = new ERTMaskMonNHit(DET_EMC,  "2x2");
  m_nhit[4] = new ERTMaskMonNHit(DET_RICH, "RICH");
  
  // you must set m_rfactor[i] (i = 0...N_RF_PLOT) to 0 or new...
  m_rfactor[0] = new ERTMaskMonRFactor("ERT_4x4a&BBCLL1", "4x4A&BBCLL1");
//  m_rfactor[1] = new ERTMaskMonRFactor("ERT_4x4b&BBCLL1", "4x4B&BBCLL1");
  m_rfactor[1] = new ERTMaskMonRFactor("ERT_4x4b", "4x4B");
//  m_rfactor[2] = new ERTMaskMonRFactor("ERT_4x4c&BBCLL1", "4x4C&BBCLL1");
  m_rfactor[2] = new ERTMaskMonRFactor("ERT_4x4b", "4x4B_copy1");
  m_rfactor[3] = new ERTMaskMonRFactor("ERTLL1_Electron&BBCLL1",   "Elec&BBCLL1");
  m_rfactor[4] = new ERTMaskMonRFactor("ERTLL1_2x2", "ERTLL1_2x2");
//  m_rfactor[5] = new ERTMaskMonRFactor("ERT_4x4b", "4x4b");
  m_rfactor[5] = new ERTMaskMonRFactor("ERT_4x4b", "4x4b_copy2");
 
  return 0;
}

void ERTMaskMonDraw::SetERTExpertDefault()
{
  m_DMUX_dir = DMUX_DIR_DEFAULT;

  m_n_event_min = 0;

  m_sigma_cut_emc  = -1;
  m_sigma_cut_rich = -1;
  for (int i = 0; i < N_NHIT_HIST; i++) {
     m_hot_threshold[i] = -1.0;
     m_rf_idx       [i] = -1;
     m_dlimit       [i] = -1.0;
     m_bl_watch_cold[i] = false;
  }
  m_brun_rfactor_history = 166030; // Run05 begin
  m_erun_rfactor_history = -1;
}

void ERTMaskMonDraw::ReadERTExpert()
{
  ifstream infile;
  ostringstream parafile;
  if (getenv("ERTMASKCALIB"))
    {
      parafile << getenv("ERTMASKCALIB") << "/";
    }
  parafile << "ERT_expert.txt";
  
  infile.open(parafile.str().c_str(), ifstream::in);
  //infile.open("/home/phnxlvl1/isyoon/online/monitoring/Run3/subsystems/ert/calib/ERT_expert.txt", ifstream::in);
  
  if (! infile) return;
  cout << endl << "Reading parameters from " << parafile.str().c_str() << endl;

  string line;
  while ( getline(infile, line) )
    {
      // skip comment lines
      if (line[0] == '/' || line[0] == '#') continue;

      istringstream iss_line(line.c_str());
      string label;
      iss_line >> label;
      std::transform(label.begin(), label.end(), label.begin(), (int(*)(int))toupper);
      const char* line_remain = line.c_str() + label.length();

      if      (label == "DMUX_DIR")
        {
          SetExptDmuxDir  (line_remain);
        }
      else if (label == "EVENT_MIN")
        {
          SetExptEvtMin   (line_remain);
        }
      else if (label == "RUN_RF_HISTORY")
        {
          SetExptRunRFHis (line_remain);
        }
      else if (label == "HOT_THRESHOLD")
        {
          SetExptHotThres (line_remain);
        }
      else if (label == "RF_DLIMIT")
        {
          SetExptRFDLimit (line_remain);
        }
      else if (label == "SIGMA_CUT")
        {
          SetExptSigmaCut (line_remain);
        }
      else if (label == "WATCH_COLD_SECT")
        {
          SetExptWatchCold(line_remain);
        }
    }
  infile.close();
}

void ERTMaskMonDraw::SetExptDmuxDir(const char* line)
{
   istringstream iss(line);
   iss >> m_DMUX_dir;
   cout << "  DMUX dir = " << m_DMUX_dir << endl;
}

void ERTMaskMonDraw::SetExptEvtMin(const char* line)
{
   istringstream iss(line);
   iss >> m_n_event_min;
   cout << "  Min. number of events = " << m_n_event_min << endl;
}

void ERTMaskMonDraw::SetExptRunRFHis(const char* line)
{
   istringstream iss(line);
   iss >> m_brun_rfactor_history >> m_erun_rfactor_history;
   cout << "  Begin & end run of RF history = " << m_brun_rfactor_history << "  "
        << m_erun_rfactor_history << endl;
}

void ERTMaskMonDraw::SetExptHotThres(const char* line)
{
   istringstream iss(line);
   string nhit_name;
   iss >> nhit_name;
   cout << "  Threshold for N of hits  (" << nhit_name << " hits) = ";
   int idx = FindNHitIndex(nhit_name.c_str());
   if (idx >= 0) {
      iss >> m_hot_threshold[idx];
      cout << m_hot_threshold[idx] << endl;
   }
}

void ERTMaskMonDraw::SetExptRFDLimit(const char* line)
{
  istringstream iss(line);
  string nhit_name, trig_name;
  iss >> nhit_name >> trig_name;
  cout << "  RF lower limit  (" << nhit_name << " hits, " << trig_name << " trigger) = ";
   int idx_nhit = FindNHitIndex(nhit_name.c_str());
   int idx_rf   = FindRFactorIndex(trig_name.c_str());
     
   if (idx_nhit >= 0 && idx_rf >= 0) {
      m_rf_idx[idx_nhit] = idx_rf;
      iss >> m_dlimit[idx_nhit];
      cout << m_dlimit[idx_nhit] << endl;
   }
}

void ERTMaskMonDraw::SetExptSigmaCut(const char* line)
{
   istringstream iss(line);
   iss >> m_sigma_cut_emc >> m_sigma_cut_rich;
   cout << "  Sigma cut = " << m_sigma_cut_emc << " " 
        << m_sigma_cut_rich << endl;
}

void ERTMaskMonDraw::SetExptWatchCold(const char* line)
{
   istringstream iss(line);
   string nhit_name;
   iss >> nhit_name;
   cout << "  Whether to watch cold half sector (" << nhit_name << " hits) = ";
   int idx = FindNHitIndex(nhit_name.c_str());
   if (idx >= 0) {
      iss >> m_bl_watch_cold[idx];
      cout << m_bl_watch_cold[idx] << endl;
   }
}

int ERTMaskMonDraw::FindNHitIndex(const char* name)
{
   for (int i = 0; i < N_NHIT_HIST; i++) {
      if (m_nhit[i] && 
          strcasecmp(name, m_nhit[i]->GetName().c_str()) == 0) {
         return i;
      }
   }
   return -1;
}

int ERTMaskMonDraw::FindRFactorIndex(const char* name)
{
   for (int i = 0; i < N_RF_PLOT; i++) {
      if (m_rfactor[i] &&
          strcasecmp(name, m_rfactor[i]->GetTriggerShortName().c_str()) == 0) {
         return i;
      }
   }
   return -1;
}

int ERTMaskMonDraw::Draw(const char *what)
{
  int flag_log  = strcasecmp(what, "LOG")  == 0 ? 1 : 0;
  int flag_rf   = strcasecmp(what, "RF")   == 0 ? 1 : 0;
  int flag_text = strcasecmp(what, "TEXT") == 0 ? 1 : 0;
  if (GetHistos() != 0) { // failed to get histos
     DrawDeadServer();
     return -1;
  }
  m_n_event          = static_cast <int> (m_Evtcount->GetBinContent(1));
  m_n_event_analyzed = static_cast <int> (m_Evtcount->GetBinContent(2));
  m_trig_sel_server  = static_cast <int> (m_Evtcount->GetBinContent(3));
  m_cross_sel_server = static_cast <int> (m_Evtcount->GetBinContent(4));

  SetERTExpertDefault();
  ReadERTExpert();
  //  GetRFactorHistory();

  // get and check run number and type
  OnlMonClient *cl = OnlMonClient::instance();
  m_run_number = cl->RunNumber();

  // update rfactor
  if (CreateConnection() == 0) {
     GetRunType();

     GetRFactorHistory();
     for (int i = 0; i < N_RF_PLOT; i++) {
        if (! m_rfactor[i]) 
	  {
            continue;
	  }
        // do not require EVENTSINRUN_MIN and PHYSICS for current run
        m_rfactor[i]->UpdateRFactor(m_con, m_run_number, false, false);
        
        // finalize RF in previous run
        if (m_run_number_pre > 0 && m_run_number != m_run_number_pre &&
            m_run_type == "PHYSICS") {
           m_rfactor[i]->UpdateRFactor(m_con, m_run_number_pre);
        }
     }
     m_run_number_pre = m_run_number;
     DeleteConnection();
  }

  // calculate hot channels
  for (int i = 0; i < N_NHIT_HIST; i++) {
     if (m_nhit[i]) m_nhit[i]->SetHisto(m_hsm[i]);
  }
  GetMaskedChannel();
  FindHotChannel();
  FindHalfSectorGap();

  // draw or text output
  if (flag_text) {
     CheckByText();
     return 0;
  } else {
     m_ertStyle->cd();
     if (! flag_rf) DrawNHit(flag_log);
     else           DrawRFactor();
     m_canvas->Update();
     m_canvas->SetEditable(kFALSE);
     m_globalStyle->cd();
  }
  
  printf(
     "\n"
     "**********************************************\n"
     "   Canvas of ERT online monitor is created.   \n"
     "**********************************************\n"
     );
  return 0;
}

void ERTMaskMonDraw::DrawNHit(const int flag_log)
{
   // make canvas and pads
   if (! gROOT->FindObject("ERTMaskMon")) {
      MakeCanvas();
      // a pad with index "i" is used to show "m_nhit[i]"
      if (m_nhit[0]) m_pad_nhit[0] = new TPad("pad_nhit_0", "", 0, 0.85, 1, 0.71);
      if (m_nhit[1]) m_pad_nhit[1] = new TPad("pad_nhit_1", "", 0, 0.71, 1, 0.57);
      if (m_nhit[2]) m_pad_nhit[2] = new TPad("pad_nhit_2", "", 0, 0.57, 1, 0.43);
      if (m_nhit[3]) m_pad_nhit[3] = new TPad("pad_nhit_3", "", 0, 0.43, 1, 0.29);
      if (m_nhit[4]) m_pad_nhit[4] = new TPad("pad_nhit_4", "", 0, 0.29, 1, 0.15);
   }
   DrawHeader();

   // draw nhit histos
   for (int i = 0; i < N_NHIT_HIST; i++) {
      m_pad_title->cd();
      if (m_nhit[i]) {
	m_pad_nhit[i]->Draw();
	m_nhit[i]->Draw(m_pad_nhit[i], flag_log);
      }
   }

   // draw messages
   DrawInfoMessage();
   DrawErrorMessage();
}

void ERTMaskMonDraw::DrawRFactor()
{
   // make canvas and pads
   if (! gROOT->FindObject("ERTMaskMon")) {
      MakeCanvas();
      // a pad with index "i" is used to show "m_rfactor[i]"
      //    +-------------------+-------------------+
      //    |         0         |         4         |
      //    +-------------------+-------------------+
      //    |         1         |         5         |
      //    +-------------------+-------------------+
      //    |         2         |         6         |
      //    +-------------------+-------------------+
      //    |         3         |         7         |
      //    +-------------------+-------------------+
      if (m_rfactor[0]) m_pad_rfactor[0] = new TPad("pad_rfactor_0", "", 0.0, 0.850, 0.5, 0.675);
      if (m_rfactor[1]) m_pad_rfactor[1] = new TPad("pad_rfactor_1", "", 0.0, 0.675, 0.5, 0.500);
      if (m_rfactor[2]) m_pad_rfactor[2] = new TPad("pad_rfactor_2", "", 0.0, 0.500, 0.5, 0.325);
      if (m_rfactor[3]) m_pad_rfactor[3] = new TPad("pad_rfactor_3", "", 0.0, 0.325, 0.5, 0.150);
      if (m_rfactor[4]) m_pad_rfactor[4] = new TPad("pad_rfactor_4", "", 0.5, 0.850, 1.0, 0.675);
      if (m_rfactor[5]) m_pad_rfactor[5] = new TPad("pad_rfactor_5", "", 0.5, 0.675, 1.0, 0.500);
      if (m_rfactor[6]) m_pad_rfactor[6] = new TPad("pad_rfactor_6", "", 0.5, 0.500, 1.0, 0.325);
      if (m_rfactor[7]) m_pad_rfactor[7] = new TPad("pad_rfactor_7", "", 0.5, 0.325, 1.0, 0.150);
   }
   DrawHeader();

   // draw rfactor plots
   for (int i = 0; i < N_RF_PLOT; i++) {
      m_pad_title->cd();
      if (m_rfactor[i]) {
	m_pad_rfactor[i]->Draw();
	m_rfactor[i]->Draw(m_pad_rfactor[i], m_run_number);
      }
   }

   // draw messages
   DrawInfoMessage();
   DrawErrorMessage();
}

void ERTMaskMonDraw::DrawHeader()
{
  OnlMonClient *cl = OnlMonClient::instance();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.05);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment

  ostringstream oss_run_and_date;
  time_t evttime = cl->EventTime("CURRENT");

  oss_run_and_date << "Run " << m_run_number << "   " << ctime(&evttime);
  m_pad_title->cd();
  PrintRun.DrawText(0.5, 0.98, "ERT Online Monitor");
  PrintRun.DrawText(0.5, 0.92, oss_run_and_date.str().c_str());
}

void ERTMaskMonDraw::DrawInfoMessage()
{
  ostringstream oss_message;
  m_pave_info_msg->Clear();

  // check run type
  oss_message << m_run_type << " RUN";
  m_pave_info_msg->AddText(oss_message.str().c_str());

  // show the number of events processed and analyzed
  oss_message.str("");
  oss_message << "Events: " << m_n_event << " processed, "
              << m_n_event_analyzed << " analyzed";
  m_pave_info_msg->AddText(oss_message.str().c_str());

  // draw server-side setting if not default
  if (m_trig_sel_server != 0 || m_cross_sel_server >= 0) {
     oss_message.str("");
     oss_message << "Evt sel: ";
     if (m_trig_sel_server != 0) {
        oss_message << "trig 0x" << hex << m_trig_sel_server << dec << "  ";
     }
     if (m_cross_sel_server >= 0) {
        oss_message << "xing " << m_cross_sel_server;
     }
     m_pave_info_msg->AddText(oss_message.str().c_str());
  }

  m_pad_title->cd();
  m_pave_info_msg->SetTextSize(0);
  m_pave_info_msg->SetTextAlign(22);
  m_pave_info_msg->Draw();
}

void ERTMaskMonDraw::DrawErrorMessage()
{
  ostringstream oss_message;
  m_pave_error_msg->Clear();

  int is_physics = (m_run_type == "PHYSICS");
  int bit_problem = 0;
  enum Problem {
     PROB_DB = 0x1, PROB_HOT = 0x2, PROB_WARM = 0x4, PROB_COLD_SECT = 0x8
  };
  
  // check hot tiles and half sector gap
  for (int i = 0; i < N_NHIT_HIST; i++) {
     if (! m_nhit[i]) continue;
     
     float rfactor = -1;
     if (m_rf_idx[i] >= 0) {
        ERTMaskMonRFactor* obj_rf = m_rfactor[m_rf_idx[i]];
        rfactor = obj_rf->GetRFactor(m_run_number);
        if (rfactor < 0 && m_rf_idx[i]!=4 && m_rf_idx[i]!=5) {
           oss_message.str("");
           oss_message << obj_rf->GetTriggerShortName() 
                       << " ... database problem in RF calculation.";
          m_pave_error_msg->AddText(oss_message.str().c_str());
          bit_problem |= PROB_DB;
        }
     }
     
     int nhot = m_nhit[i]->GetNHot();
     if (nhot > 0) {
        oss_message.str("");
        oss_message << m_nhit[i]->GetName() 
                    <<  " ... " << nhot << " hot tile.";
        m_pave_error_msg->AddText(oss_message.str().c_str());

        if      (rfactor < 0)             bit_problem = PROB_HOT;
        else if (rfactor < m_dlimit[i]/2) bit_problem = PROB_HOT;
        else if (rfactor < m_dlimit[i]  ) bit_problem = PROB_WARM;
     }

     if (m_nhit[i]->HasColdHalfSect()) {
        oss_message.str("");
        oss_message << m_nhit[i]->GetName()
                    << " ... few hits in half sector";
        for (int ihs = 0; ihs < N_HALF_SECTOR; ihs++) {
           if (m_nhit[i]->IsColdHalfSect(ihs)) {
              oss_message << " " << HALF_SECTOR_NAME[ihs];
           }
        }
        m_pave_error_msg->AddText(oss_message.str().c_str());
        bit_problem = PROB_COLD_SECT;
     }
  }
  
  // draw an OK (or to-do) message
  if (! is_physics) {
     m_pave_error_msg->SetFillColor(kWhite);
  } else if (! bit_problem) {
     m_pave_error_msg->AddText("OK.");
     m_pave_error_msg->SetFillColor(kGreen);
  } else {
     m_pave_error_msg->AddText(""); // make a blank line
     if (bit_problem & PROB_HOT) {
        m_pave_error_msg->AddText("Stop the run and feed to the subsystems specified in the OnlMon doc.");
        m_pave_error_msg->SetFillColor(kRed);
     } else if (bit_problem & PROB_WARM) {
        m_pave_error_msg->AddText("Check whether DAQ live time is fair.");
        m_pave_error_msg->AddText("If not, stop the run and feed to the subsystems specified in the OnlMon doc.");
        m_pave_error_msg->SetFillColor(kYellow);
     } else if (bit_problem & PROB_COLD_SECT) {
        m_pave_error_msg->AddText("Stop the run and feed to the subsystems specified in the OnlMon doc.");
        m_pave_error_msg->SetFillColor(kRed);
     } else if (bit_problem & PROB_DB) {
        m_pave_error_msg->AddText("Call ERT expert if the database problem lasts 10 min.");
        m_pave_error_msg->SetFillColor(kYellow);
     }
  }
  if (m_n_event < m_n_event_min) {
     m_pave_error_msg->AddText("(not enough events now)");
  }
  
  m_pad_title->cd();
  m_pave_error_msg->SetTextSize(0);
  m_pave_error_msg->SetTextAlign(22);
  m_pave_error_msg->Draw();
}

void ERTMaskMonDraw::DrawDeadServer()
{
  cerr << "The ERT OnlMon server might be dead." << endl << flush;

  if (! gROOT->FindObject("ERTMaskMon")) {
     MakeCanvas();
  }
  m_pad_title->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.05);
  FatalMsg.SetTextColor(kRed);
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawTextNDC(0.5, 0.5, "ERTMONITOR SERVER DEAD");
  m_pad_title->Update();
}

void ERTMaskMonDraw::MakeCanvas()
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = static_cast<int>(0.9 * cl->GetDisplaySizeX());
  int ysize = static_cast<int>(0.9 * cl->GetDisplaySizeY());

  m_canvas = new TCanvas("ERTbitMon", "ERT GL1 bit Monitor", -1, 0, xsize, ysize);
  gSystem->ProcessEvents();

  // this one is used to plot the header (run number etc.) or error messages
  m_pad_title = new TPad("ert_pad_title", "",
                         0, 0, 1, 1, 0, 0);
  m_pad_title->SetFillStyle(4000);
  m_pad_title->Draw();
}

int ERTMaskMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int runnumber = cl->RunNumber();
  ostringstream oss_fname;

  int iret = Draw(what);
  if (iret != 0) return iret;
  oss_fname << ThisName << "_2_" << runnumber << ".ps";
  m_canvas->Print(oss_fname.str().c_str());

  iret = Draw("LOG");
  if (iret != 0) return iret;
  oss_fname.str("");
  oss_fname << ThisName << "_4_" << runnumber << ".ps";
  m_canvas->Print(oss_fname.str().c_str());

  iret = Draw("RF");
  if (iret != 0) return iret;
  oss_fname.str("");
  oss_fname << ThisName << "_6_" << runnumber << ".ps";
  m_canvas->Print(oss_fname.str().c_str());

  return 0;
}

int ERTMaskMonDraw::MakeHtml(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();

  int iret = Draw(what);
  if (iret != 0) return iret;
  string pngfile = cl->htmlRegisterPage(*this, "Hot Channels (lin)", "2", "png");
  cl->CanvasToPng(m_canvas, pngfile);

  iret = Draw("LOG");
  if (iret != 0) return iret;
  pngfile = cl->htmlRegisterPage(*this, "Hot Channels (log)", "4", "png");
  cl->CanvasToPng(m_canvas, pngfile);
  cl->SaveLogFile(*this);

  iret = Draw("RF");
  if (iret != 0) return iret;
  pngfile = cl->htmlRegisterPage(*this, "Rejection", "6", "png");
  cl->CanvasToPng(m_canvas, pngfile);
  cl->SaveLogFile(*this);

  return 0;
}

void ERTMaskMonDraw::GetMaskedChannel()
{
   for (int i = 0; i < N_NHIT_HIST; i++) {
      m_nhit[i]->ResetMask();
   }

  ostringstream fname;
  char buf[256];
  ifstream infile;
  unsigned long hh1, hh2, hhmask;
  unsigned long hdmy;

  m_DECODE->Reset();

  for (int iarm = 0; iarm < N_ARM; iarm++) {
     for (int iroc = 0;iroc < N_ROC_LINE; iroc++) {
        fname.str("");
        fname << m_DMUX_dir << "/DMUX" << iarm << "_" << (ROC_NUM[iroc] + 2) << "Write.hex" ;
        infile.open(fname.str().c_str());
        if (!infile) {
           cout << "no input file : " << fname.str() << endl;
        } else {
           for (int ii = 0; ii < 3; ii++) 
              infile.getline(buf, 256); // read and discard
           for (int iword = 5; iword >= 0; iword--) {
              infile.getline(buf, 256);
              sscanf(buf, "%lx %lx %lx %lx %lx", &hdmy, &hdmy, &hdmy, &hh1, &hh2);
              hhmask = hh1 * 0x100 + hh2;
              m_DECODE->SetPacketData(PACKET_ID[iarm], ROC_NUM[iroc], iword, hhmask);
           }
        }
        infile.close();
     }
  }
  
  m_DECODE->Calculate();
  
  for (int arm = 0; arm < N_ARM; arm++) {
     for (int sector = 0; sector < N_SECTOR; sector++) {
        // EMCal part
        int n_sm = IsPbGl(arm, sector) ? N_SM_PBGL : N_SM_PBSC;
        for (int sm = 0; sm < n_sm; sm++) {
           int ssm = ArmSectSmToSerialSm(DET_EMC, arm, sector, sm);
           m_nhit[0]->SetMask(ssm, m_DECODE->GetBit4x4ASMEMC(arm, sector, sm));
           m_nhit[1]->SetMask(ssm, m_DECODE->GetBit4x4BSMEMC(arm, sector, sm));
           m_nhit[2]->SetMask(ssm, m_DECODE->GetBit4x4CSMEMC(arm, sector, sm));
           m_nhit[3]->SetMask(ssm, m_DECODE->GetBit2x2SMEMC (arm, sector, sm));
        }
        
        // RICH part
        for (int sm = 0; sm < N_SM_RICH; sm++) {
           int ssm = ArmSectSmToSerialSm(DET_RICH, arm, sector, sm);
           m_nhit[4]->SetMask(ssm, m_DECODE->GetBit4x5SMRICH(arm, sector, sm));
        }
     }
  }
}

void ERTMaskMonDraw::FindHotChannel()
{
   for (int i = 0; i < N_NHIT_HIST; i++) {
      if (! m_nhit[i]) continue;
      float  hot_th = m_hot_threshold[i];
      int rf_idx = m_rf_idx[i];
      float dlimit = m_dlimit[i];
      float sigma_cut = (m_nhit[i]->GetDetector() == DET_EMC) ?
         m_sigma_cut_emc : m_sigma_cut_rich;

      if (hot_th >= 0) {
         m_nhit[i]->FindHotWithNHitThreshold(hot_th);
      } else if (rf_idx >= 0) {
	if (m_rfactor[rf_idx]) {
         float rfactor = m_rfactor[rf_idx]->GetRFactor(m_run_number);
         if (rfactor > 0) m_nhit[i]->FindHotWithRFactor(rfactor, dlimit);
	}
      } else if (sigma_cut >= 0) {
         m_nhit[i]->FindHotWithNHitSigma(sigma_cut);
      } else {
         m_nhit[i]->ResetHot();
      }
   }
   MakeNewDMUX();
}

void ERTMaskMonDraw::FindHalfSectorGap()
{
   for (int i = 0; i < N_NHIT_HIST; i++) {
      if (! m_nhit[i]) continue;
      if (m_bl_watch_cold[i]) m_nhit[i]->FindColdSector();
      else                    m_nhit[i]->ResetColdSector();
   }
}

//////////////////////////////

void ERTMaskMonDraw::GetRFactorHistory()
{
  for(int i=0;i<N_RF_PLOT;++i)
    {
      if(m_rfactor[i]) 
	{
	  // this function does nothing if it already has its history
	  cout << "Load History from run " << m_brun_rfactor_history << " to " << m_erun_rfactor_history << endl;
	  m_rfactor[i]->GetRFactorHistory(m_con, m_brun_rfactor_history, m_erun_rfactor_history);
	}
    }
}//void ERTMaskMonDraw::GetRFactorHistory

//////////////////////////////

int ERTMaskMonDraw::MakeNewDMUX()
{
  m_DECODE->Reset();

  for (int arm = 0; arm < 2; arm++) {
     for (int sector = 0; sector < 4; sector++) {
        // EMCal
        int n_sm = IsPbGl(arm, sector) ? N_SM_PBGL : N_SM_PBSC;
        for (int sm = 0; sm < n_sm; sm++) {
           int ssm = ArmSectSmToSerialSm(DET_EMC, arm, sector, sm);
           if (m_nhit[0]->IsHot(ssm)) {
              m_DECODE->SetBit4x4ASMEMC(arm, sector, sm);
           }
           if (m_nhit[1]->IsHot(ssm)) {
              m_DECODE->SetBit4x4BSMEMC(arm, sector, sm);
           }
           if (m_nhit[2]->IsHot(ssm)) {
              m_DECODE->SetBit4x4CSMEMC(arm, sector, sm);
           }
           if (m_nhit[3]->IsHot(ssm)) {
              m_DECODE->SetBit2x2SMEMC(arm, sector, sm);
           }
        }
        // RICH
        for (int sm = 0; sm < N_SM_RICH; sm++) {
           int ssm = ArmSectSmToSerialSm(DET_RICH, arm, sector, sm);
           if (m_nhit[4]->IsHot(ssm)) {
              m_DECODE->SetBit4x5SMRICH(arm, sector, sm);
           }
        }
     }
  }
  
  m_DECODE->CalculateInv();
  
  char buf[256];
  ifstream ifile;
  ofstream ofile;
  int wd[5], pkd;

  ostringstream fname;
  // modify Write.hex
  for (int iarm = 0; iarm < N_ARM; iarm++) {
     for (int iroc = 0; iroc < N_ROC_LINE; iroc++) {
        fname.str("");
        fname << m_DMUX_dir << "/DMUX" << iarm << "_"
              << (ROC_NUM[iroc] + 2) << "Write.hex" ;
        ifile.open(fname.str().c_str());
        
        fname.str("");
        fname << m_DMUXout_dir << "/DMUX" << iarm << "_"
              << (ROC_NUM[iroc] + 2) << "Write.hex" ;
        ofile.open(fname.str().c_str(), ofstream::out);
        
        if (ifile && ofile) {
           // first 3 lines are just copied
           for (int ii = 0; ii < 3; ii++) {
              ifile.getline(buf, 256);
              ofile << buf << endl;
           }
           for (int iword = 5; iword >= 0; iword--) {
              pkd = 0xffff;
              pkd = m_DECODE->GetPacketData(PACKET_ID[iarm], ROC_NUM[iroc], iword);
              ifile.getline(buf, 256);
              sscanf(buf, "%x %x %x %x %x", &wd[0], &wd[1], &wd[2], &wd[3], &wd[4]);
              // 4th and 5th columns (8 bits each) contains channnel mask info
              wd[3] |= (pkd >> 8);
              wd[4] |= (pkd & 0xff);
              sprintf(buf, "%02X %02X %02X %02X %02X", wd[0], wd[1], wd[2], wd[3], wd[4]);
              ofile << buf << endl;
           }
        } else {
           cout << "no input or output file" << endl;
        }
        ifile.close();
        ofile.close();
     }
  }
  
  // modify Read.rsp
  for (int iarm = 0; iarm < N_ARM; iarm++) {
     for (int iroc = 0; iroc < N_ROC_LINE; iroc++) {
        fname.str("");
        fname << m_DMUX_dir << "/DMUX" << iarm << "_"
              << (ROC_NUM[iroc] + 2) << "Read.rsp" ;
        ifile.open(fname.str().c_str());
        
        fname.str("");
        fname << m_DMUXout_dir << "/DMUX" << iarm << "_"
              << (ROC_NUM[iroc] + 2) << "Read.rsp" ;
        ofile.open(fname.str().c_str(), ofstream::out);
        if (ifile && ofile) {
           // first 5 lines are just copied
           for (int ii = 0; ii < 5; ii++) {
              ifile.getline(buf, 256);
              ofile << buf << endl;
           }
           for (int iword = 5; iword >= 0; iword--) {
              pkd = 0xffff;
              pkd = m_DECODE->GetPacketData(PACKET_ID[iarm], ROC_NUM[iroc], iword);
              ifile.getline(buf, 256);
              sscanf(buf, "%x %x %x %x %x", &wd[0], &wd[1], &wd[2], &wd[3], &wd[4]);
              // 4th and 5th columns (8 bits each) contains channnel mask info
              wd[3] = wd[3] | (pkd >> 8);
              wd[4] = wd[4] | (pkd & 0xff);
              sprintf(buf, "%02X %02X %02X %02X %02X", wd[0], wd[1], wd[2], wd[3], wd[4]);
              ofile << buf << endl;
           }
        } else {
           cout << "no input or output file : " << endl;
        }
        ifile.close();
        ofile.close();
     }
  }
  return 0;
}

void ERTMaskMonDraw::CheckByText()
{
  int arm, sec, sm;

  for (int ssm = 0; ssm < N_SM_EMC_TOTAL; ssm++) {
     SerialSmToArmSectSm(DET_EMC, ssm, arm, sec, sm);
     int i4x4a = (int)m_hsm[0]->GetBinContent(ssm + 1);
     int i4x4b = (int)m_hsm[1]->GetBinContent(ssm + 1);
     int i4x4c = (int)m_hsm[2]->GetBinContent(ssm + 1);
     int i2x2 =  (int)m_hsm[3]->GetBinContent(ssm + 1);
     char m4x4a = ' ';
     char m4x4b = ' ';
     char m4x4c = ' ';
     char m2x2  = ' ';
     if (m_nhit[0]->IsMasked(ssm)) m4x4a = '#';
     if (m_nhit[1]->IsMasked(ssm)) m4x4b = '#';
     if (m_nhit[2]->IsMasked(ssm)) m4x4c = '#';
     if (m_nhit[3]->IsMasked(ssm)) m2x2  = '#';
     
     printf("EMC %2d %2d %2d %5d%1c %5d%1c %5d%1c %5d%1c\n",
            arm, sec, sm,
            i4x4a, m4x4a, i4x4b, m4x4b, i4x4c, m4x4c, i2x2, m2x2);
  }
  for (int ssm = 0; ssm < N_SM_RICH_TOTAL; ssm++) {
     SerialSmToArmSectSm(DET_RICH, ssm, arm, sec, sm);
     int irich = (int)m_hsm[4]->GetBinContent(ssm + 1);
     char mrich = ' ';
     if (m_nhit[4]->IsMasked(ssm)) mrich = '#';
     
     printf("RICH %2d %2d %2d %5d%1c\n",
            arm, sec, sm, irich, mrich);
  }
}

int ERTMaskMonDraw::CreateConnection()
{
   m_con = 0;
   cout << "Making a connection to the 'daq' database... " << flush;
   try {
      m_con = DriverManager::getConnection("daq", "phnxrc", "");
   } catch (SQLException& e) {
      cout << " Fatal Exception caught during DriverManager::getConnection" << endl;
      cout << "Message: " << e.getMessage() << endl;
      m_con = 0;
      return 1;
   }
   cout << "done." << endl;
   return 0;
}

int ERTMaskMonDraw::DeleteConnection()
{
   if (m_con) delete m_con;
   m_con = 0;
   return 0;
}

void ERTMaskMonDraw::GetRunType()
{
   m_run_type = "UNKNOWN";

   Statement* stmt = m_con->createStatement();
   ostringstream cmd;
   cmd << "select runtype from run where runnumber = " << m_run_number;
   ResultSet* rs = 0;
   try {
      rs = stmt->executeQuery(cmd.str());
   } catch (SQLException& e) {
      cout << "Fatal Exception caught during stmt->executeQuery(" << cmd.str() << ")" << endl;
      cout << "Message: " << e.getMessage() << endl;
      delete rs;
      return;
   }

   if ( rs->next() ) { // only one entry
      try {
         m_run_type = rs->getString(1);
      } catch (SQLException& e) {
         cout << "Fatal Exception caught during rs->getString(1)" << endl;
         cout << "Message: " << e.getMessage() << endl;
         delete rs;
         return;
      }
   }
   delete rs;
}
