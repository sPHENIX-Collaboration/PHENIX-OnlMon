#include <ERTMaskMonCommon.h>
#include <ERTMaskMonRFactor.h>

#include <TAxis.h>
#include <TGraph.h>
#include <TMarker.h>
#include <TPad.h>
#include <TPaveText.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <odbc++/connection.h>
#include <odbc++/resultset.h>

using namespace std;
using namespace odbc;
using namespace ERTMaskMonCommon;

ERTMaskMonRFactor::ERTMaskMonRFactor(const char* trigger_name, const char* short_name)
{
   m_trigger_name = trigger_name;
   m_trigger_short_name = short_name;

   m_brun = 0;
   m_erun = 0;
   m_has_rfactor_history = false;

   m_pave_title = new TPaveText(LEFT_MARGIN+0.05, 0.9, 0.6, 1.0, "NDC");

   m_graph  = new TGraph();
   m_graph_low = new TGraph();
   m_graph_high = new TGraph();
   m_marker = new TMarker();
}

ERTMaskMonRFactor::~ERTMaskMonRFactor()
{
   delete m_pave_title;
   delete m_graph;
   delete m_graph_low;
   delete m_graph_high;
   delete m_marker;
   m_rfactor.clear();
   return;
}

//////////////////////////////

void ERTMaskMonRFactor::GetRFactorHistory(const Connection* con, const int brun, const int erun)
{
  if(brun==m_brun&&erun==m_erun&&m_has_rfactor_history) return;
  
  m_brun = brun;
  m_erun = erun;
  m_has_rfactor_history = false;
  m_rfactor.clear();
  
  cout << "Getting the rejection factor history for " << m_trigger_name << " from database." << endl;
  
  Statement* stmt = const_cast<Connection*>(con)->createStatement();
  
  ostringstream cmd;
  cmd << "select run.runnumber, tr_ert.scalerberlive, tr_mb.scalerberscaled, tr_mb.scaledown "
      << "from run, trigger as tr_ert, trigger as tr_mb "
      << "where run.runnumber >= " << m_brun
      // << " and run.runnumber!=302719 "
      // << " and run.runnumber!=302720 "
      // << " and run.runnumber!=302742 "
      // << " and run.runnumber!=302744 "
      // << " and run.runnumber!=306266 "
      // << " and run.runnumber!=345758 "
      // << " and run.runnumber!=346666 "
      << " and run.runnumber!=346667 ";
  if (m_erun > 0) cmd << " and run.runnumber<" << m_erun;
  
  cmd << " and run.runnumber = tr_ert.runnumber"
      << " and tr_ert.runnumber = tr_mb.runnumber"
      << " and run.eventsinrun >= " << EVENTSINRUN_MIN
      << " and run.runtype = 'PHYSICS' and run.runstate = 'ENDED'"
      << " and tr_ert.name = '" << m_trigger_name << "'"
      << " and tr_mb.name = '" << TRIG_NAME_MB << "'";
  // if you get a database error message uncomment the following line and
  // check if the sql command actually goes through (normally the trigger name
  // was changed and the sql cmd returns zero rows)
  //  cout << "command: " << cmd.str() << endl;
  ResultSet* rs = 0;
  try 
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Fatal Exception caught during stmt->executeQuery(" << cmd.str() << ")" << endl;
      cout << "Message: " << e.getMessage() << endl;
      delete rs;
      return;
    }
  
  while ( rs->next() )
    {
      int run;
      int sc_ert;
      double sc_mb;
      unsigned int sc_mb_scaled;
      unsigned int sc_mb_scaledown;
      try 
	{
	  run    = rs->getInt(1);
	  sc_ert = rs->getInt(2);
	  sc_mb_scaled  = (unsigned int)rs->getInt(3); 
	  sc_mb_scaledown  = (unsigned int)rs->getInt(4); 
	}
      catch (SQLException& e)
	{
	  cout << "Fatal Exception caught during rs->getInt([123])" << endl;
	  cout << "Message: " << e.getMessage() << endl;
	  delete rs;
	  return;
	}
      //Code used in Run 9 for high luminosity in pp500
      //REMOVED FOR Run10
      /*
      //code for overflow of nv scalar
      int ss=8;
      sc_ert=sc_ert/ss;
      sc_nv=sc_nv/ss;
      sc_zd=sc_zd/ss;
      
      //for loop added in run9 to account for high luminosity in pp500
      for(int i = 1; i < ss; i++)
      {
      if((float)sc_nv/sc_zd < 9){sc_nv+=0x20000000;}
      else{break;} 
      }
      */
      sc_mb = (double(sc_mb_scaledown) + 1) * double(sc_mb_scaled);
      m_rfactor[run] = (sc_ert > 0  ?  (double)sc_mb / sc_ert  :  0);
    }
  delete rs;
  m_has_rfactor_history = true;
}//ERTMaskMonRFactor::GetRFactorHistory

//////////////////////////////

void ERTMaskMonRFactor::UpdateRFactor(const Connection* con, const int runnumber, const int bl_require_evt_min, const int bl_require_physics)
{
  Statement* stmt = const_cast<Connection*>(con)->createStatement();
  
  ostringstream cmd;
  cmd << "select run.runstate, tr_ert.scalerberlive, tr_mb.scalerberscaled, tr_mb.scaledown, tr_ert.scalerupdateraw, tr_mb.scalerupdatescaled, tr_mb.scaledown "
      << "from run, trigger as tr_ert, trigger as tr_mb "
      << "where run.runnumber = " << runnumber
      << " and run.runnumber = tr_ert.runnumber"
      << " and tr_ert.runnumber = tr_mb.runnumber";
  if (bl_require_evt_min) cmd << " and run.eventsinrun >= " <<EVENTSINRUN_MIN;
  if (bl_require_physics) cmd << " and run.runtype = 'PHYSICS'";
  cmd << " and tr_ert.name = '" << m_trigger_name << "'"
      << " and tr_mb.name = '" << TRIG_NAME_MB << "'";
  
  ResultSet* rs = 0;
  try 
    {
      rs = stmt->executeQuery(cmd.str());
    } 
  catch (SQLException& e)
    {
      cout << "Fatal Exception caught during stmt->executeQuery(" << cmd.str() << ")" << endl;
      cout << "Message: " << e.getMessage() << endl;
      delete rs;
      return;
    }
  
  if ( rs->next() ) 
    { // only one entry
      string runstate;
      int sc_ber_ert;
      double sc_ber_mb;
      unsigned int sc_ber_mb_scaled;
      unsigned int sc_ber_mb_scaledown;
      int sc_upd_ert;
      double sc_upd_mb;
      unsigned int sc_upd_mb_scaled;
      unsigned int sc_upd_mb_scaledown;
      try 
	{
	  runstate   = rs->getString(1);
	  sc_ber_ert = rs->getInt(2);
	  sc_ber_mb_scaled  = (unsigned int)rs->getInt(3);
	  sc_ber_mb_scaledown  = (unsigned int)rs->getInt(4);
	  sc_upd_ert = rs->getInt(5);
	  sc_upd_mb_scaled  = (unsigned int)rs->getInt(6);
	  sc_upd_mb_scaledown  = (unsigned int)rs->getInt(7);
	} 
      catch (SQLException& e)
	{
	  cout << "Fatal Exception caught during rs->getString(1) etc." << endl;
	  cout << "Message: " << e.getMessage() << endl;
	  delete rs;
	  return;
	}
      
      float rf = 0;
      
      if (runstate == "ENDED")
	{
	  sc_ber_mb = (double(sc_ber_mb_scaledown) + 1) * double(sc_ber_mb_scaled);
	  if (sc_ber_ert > 0) rf = (double)sc_ber_mb / sc_ber_ert;
	}
      else 
	{
	  sc_upd_mb = (double(sc_upd_mb_scaledown) + 1) * double(sc_upd_mb_scaled);
	  if (sc_upd_ert > 0) rf = (double)sc_upd_mb / sc_upd_ert;
	}
      
      m_rfactor[runnumber] = rf;
    }
  else if (m_rfactor.find(runnumber) != m_rfactor.end()) 
    {
      m_rfactor.erase(runnumber);
    }
  
  delete rs;
}

void ERTMaskMonRFactor::Draw(TPad* pad, const int runnumber)
{
  pad->cd();
  pad->SetLeftMargin (LEFT_MARGIN);
  pad->SetRightMargin(RIGHT_MARGIN);
  
  pad->SetGridy(kTRUE);
    
  int lower_bound=0;
  int upper_bound=1000;

  //Run15 PP200
  // if(m_trigger_short_name.compare("4x4B")==0)
  //     {
  //       lower_bound = 500;
  //       upper_bound = 15000;
  //     } 
  //   else if(m_trigger_short_name.compare("4x4A&BBCLL1")==0)
  //     {
  //       lower_bound = 0;
  //       upper_bound = 5000;
  //     }
  //   else if(m_trigger_short_name.compare("4x4C&BBCLL1")==0)
  //     {
  //       lower_bound = 0;
  //       upper_bound = 4000;
  //     }
  //   else if(m_trigger_short_name.compare("EleBBC")==0)
  //     {
  //       lower_bound = 0;
  //       upper_bound = 8000; 
  //     }
  //   else if(m_trigger_short_name.compare("MPCS_2x2")==0)
  //     {
  //       lower_bound = 0;
  //       upper_bound = 5e4;
  //     }
  //   else if(m_trigger_short_name.compare("MPCN_2x2")==0)
  //     {
  //       lower_bound = 0;
  //       upper_bound = 6e4;
  //     }
  
  //Run16 AuAu
  if(m_trigger_short_name.compare("4x4A&BBCLL1")==0)
    {                                                                              
      lower_bound = 0;                                                           
      upper_bound = 80;                                                         
    }     
  else if(m_trigger_short_name.compare("4x4B&BBCLL1")==0)
    {                
      lower_bound = 0;
      upper_bound = 140;
    }
  else if(m_trigger_short_name.compare("4x4C&BBCLL1")==0)
    {
      lower_bound = 0;
      upper_bound = 60;
    }
  else if(m_trigger_short_name.compare("Elec&BBCLL1")==0)
    {
      lower_bound = 0;
      upper_bound = 60;
    }
  else if(m_trigger_short_name.compare("ERTLL1_2x2")==0)
    {
      lower_bound = 0;
      upper_bound = 60;
    }
  else if(m_trigger_short_name.compare("4x4b")==0)
    {
      lower_bound = 0;
      upper_bound = 120;
    }




  int n_point = 0;
  int n_point_low = 0;
  int n_point_high = 0;
  for(RFactorMap_t::iterator i = m_rfactor.begin(); i != m_rfactor.end(); ++i) 
    {
      int runnumber = i->first;
      float rfactor = i->second;
      
      //normal points
      if(rfactor<upper_bound)
	{
	  m_graph->SetPoint(n_point, runnumber, rfactor);
	  n_point++;
	}
      
      //low points
      if(rfactor<lower_bound)
	{
	  m_graph_low->SetPoint(n_point_low, runnumber, rfactor);
	  n_point_low++;
	}

      if(rfactor>upper_bound)
	{
	  m_graph_high->SetPoint(n_point_high, runnumber, upper_bound);
	  n_point_high++;
	}
      
    }
  
  if (n_point == 0)
    { // dummy point to draw axis
      m_graph->SetPoint(0, 0, 0);
      n_point++;
    }
  
  
  float draw_upper_bound = 1.1*upper_bound;
  m_graph->SetMinimum(0);
  m_graph->SetMaximum((int)draw_upper_bound);
  m_graph->SetMarkerStyle(MARKER_STYLE);
  m_graph->Set(n_point);
  m_graph->GetXaxis()->SetNoExponent();
  m_graph->GetYaxis()->SetNdivisions(505);
  m_graph->GetXaxis()->SetLabelSize(FONT_SIZE);
  m_graph->GetYaxis()->SetLabelSize(FONT_SIZE);
  m_graph->Draw("AP");

  if(n_point_low!=0)
    {
      m_graph_low->SetMarkerStyle(MARKER_STYLE);
      m_graph_low->SetMarkerColor(4);
      m_graph_low->Set(n_point_low);
      m_graph_low->Draw("PSAME");
    }
  
  if(n_point_high!=0)
    {
      m_graph_high->SetMarkerStyle(22);
      m_graph_high->SetMarkerColor(2);
      m_graph_high->Set(n_point_high);
      m_graph_high->Draw("PSAME");
    }  

  // draw title        
  ostringstream oss_title;
  oss_title << m_trigger_short_name << " rejection factor";
  if (m_rfactor.find(runnumber) != m_rfactor.end())
    {
      oss_title << " (" << (int)m_rfactor[runnumber] << " in current run)";
    }
  m_pave_title->Clear();
  m_pave_title->AddText(oss_title.str().c_str());
  m_pave_title->SetBorderSize(1);
  m_pave_title->Draw();
    
  // current run
  if (m_rfactor.find(runnumber) != m_rfactor.end())
  {
    m_marker->SetMarkerStyle(MARKER_STYLE);
    m_marker->SetMarkerColor(kCyan);      
    m_marker->DrawMarker(runnumber, m_rfactor[runnumber]);
  }
}

float ERTMaskMonRFactor::GetRFactor(const int runnumber)
{
  if (m_rfactor.find(runnumber) != m_rfactor.end()) return m_rfactor[runnumber];
  else return -1.0;
}
