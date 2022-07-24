#ifndef __ERTMASKMONRFACTOR_H__
#define __ERTMASKMONRFACTOR_H__

#include <string>
#include <map>
#include "ERTMaskMonCommon.h"

class TGraph;
class TMarker;
class TPad;
class TPaveText;

using namespace ERTMaskMonCommon;

namespace odbc {
   class Connection;
};

class ERTMaskMonRFactor
{
 public:
   ERTMaskMonRFactor(const char* trigger_name, const char* short_name);
   virtual ~ERTMaskMonRFactor();

   void GetRFactorHistory(const odbc::Connection* con, const int brun, const int erun);
   int  HasRFactorHistory() { return m_has_rfactor_history; }
   void UpdateRFactor(const odbc::Connection* con, const int runnumber, const int bl_require_evt_min = true, const int bl_require_physics = true);
   void Draw(TPad* pad, const int runnumber);
   float GetRFactor(const int runnumber);

   std::string GetTriggerName() { return m_trigger_name; }
   std::string GetTriggerShortName() { return m_trigger_short_name; }

 protected:
   static const int EVENTSINRUN_MIN = 100000;

   static const double FONT_SIZE = 0.08;
   static const double LEFT_MARGIN  = 0.08;
   static const double RIGHT_MARGIN = 0.07;

   static const int MARKER_STYLE = 20;

   std::string m_trigger_name;
   std::string m_trigger_short_name;

   int m_brun;
   int m_erun;
   int m_has_rfactor_history;
   typedef std::map<int, float> RFactorMap_t;
   RFactorMap_t m_rfactor;
   TGraph* m_graph;
   TGraph* m_graph_low;
   TGraph* m_graph_high;
   TMarker* m_marker;

   // painting
   TPaveText* m_pave_title;
};

#endif /* __ERTMASKMONRFACTOR_H__ */
