#ifndef __ERTMASKMONDRAW_H__
#define __ERTMASKMONDRAW_H__

#include <OnlMonDraw.h>
#include <ERTMaskMonCommon.h>
#include <ERTMaskMonHisto.h>

#include <string>
class TCanvas;
class TPad;
class TPaveText;
class TStyle;

class EMCalRichDecodeOnline;
class ERTMaskMonNHit;
class ERTMaskMonRFactor;

namespace odbc {
   class Connection;
};

using namespace ERTMaskMonCommon;

class ERTMaskMonDraw: public OnlMonDraw, public ERTMaskMonHisto
{
   static const int N_RF_PLOT = 8;

 public:
   ERTMaskMonDraw();
   virtual ~ERTMaskMonDraw();

   int Init();
   int Draw(const char *what = "ALL");
   int MakePS(const char *what = "ALL");
   int MakeHtml(const char *what = "ALL");

 protected:
   void SetERTExpertDefault();
   void ReadERTExpert();
   void SetExptDmuxDir (const char* line); // rootcint doesn't allow istringstream...
   void SetExptEvtMin   (const char* line);
   void SetExptRunRFHis (const char* line);
   void SetExptHotThres (const char* line);
   void SetExptRFDLimit (const char* line);
   void SetExptSigmaCut (const char* line);
   void SetExptWatchCold(const char* line);

   int FindNHitIndex   (const char* name);
   int FindRFactorIndex(const char* name);
   
   void GetMaskedChannel();
   void FindHotChannel();
   void FindHalfSectorGap();
   void GetRFactorHistory();

   void DrawNHit(const int logflag);
   void DrawRFactor();
   void DrawHeader();
   void DrawInfoMessage();
   void DrawErrorMessage();
   void DrawDeadServer();
   void MakeCanvas();
   void CheckByText();

   int MakeNewDMUX();

   int m_run_number;
   int m_run_number_pre;
   std::string m_run_type;
   int m_n_event;
   int m_n_event_analyzed;
   int m_trig_sel_server;
   int m_cross_sel_server;

   odbc::Connection* m_con; // DB object used by ERTMaskMonRFactor's to get RF
   int CreateConnection();
   int DeleteConnection();
   void GetRunType();

   // ERTExpert
   int m_n_event_min;
   float m_sigma_cut_emc;
   float m_sigma_cut_rich;
   float m_hot_threshold[N_NHIT_HIST]; // SM's whose N-of-hits are above this value is labeled hot
   int   m_rf_idx       [N_NHIT_HIST]; // Index of "m_rfactor[]" that is used to find hot SM's in each m_nhit.
   float m_dlimit       [N_NHIT_HIST]; // rfactor lower limit for m_rfactor[m_rf_idx[ ]].
   int   m_bl_watch_cold[N_NHIT_HIST]; // whether watch large difference between half sectors
   int   m_brun_rfactor_history;
   int   m_erun_rfactor_history;

   // masked tiles
   std::string m_DMUX_dir;
   std::string m_DMUXout_dir;

   ERTMaskMonNHit* m_nhit[N_NHIT_HIST];
   ERTMaskMonRFactor* m_rfactor[N_RF_PLOT];

   // painting
   TCanvas* m_canvas;
   TPad* m_pad_title;
   TPad* m_pad_nhit   [N_NHIT_HIST];
   TPad* m_pad_rfactor[N_RF_PLOT];
   TPaveText* m_pave_info_msg;
   TPaveText* m_pave_error_msg;

   TStyle *m_ertStyle;
   TStyle *m_globalStyle;

   EMCalRichDecodeOnline *m_DECODE;
};

#endif /*__ERTMASKMONDRAW_H__ */
