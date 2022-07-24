#include <origHist.h>
#include <muiMonGlobals.h>
#include <OnlMonServer.h>

#include <msg_profile.h>
#include <msg_control.h>

#include <phool.h>

#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>

#include <iostream>
#include <sstream>

using namespace std;
/////////////////////////////////////////////////////////////////////////////
origHist::origHist(OnlMon *myparent): obj_hist(myparent)
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  //   cout << "origHist contructed" << endl;
  num_event = new TH1F("MUI_NumEvent", "Number of events", 1, 0.5, 1.5);
  Onlmonserver->registerHisto(parent,num_event);

  all[0] = new TH1F("MUI_SHA", "South Horiz All", 1920, -0.5, 1919.5);
  all[1] = new TH1F("MUI_SVA", "South Vert All", 1920, -0.5, 1919.5);
  all[2] = new TH1F("MUI_NHA", "North Horiz All", 1920, -0.5, 1919.5);
  all[3] = new TH1F("MUI_NVA", "North Vert All", 1920, -0.5, 1919.5);
  for (int ih = 0; ih<4; ih++) { 
    Onlmonserver->registerHisto(parent,all[ih]); 
  }

  bad[0] = new TH1F("MUI_SHB", "South Horiz Bad", 1920, -0.5, 1919.5);
  bad[1] = new TH1F("MUI_SVB", "South Vert Bad", 1920, -0.5, 1919.5);
  bad[2] = new TH1F("MUI_NHB", "North Horiz Bad", 1920, -0.5, 1919.5);
  bad[3] = new TH1F("MUI_NVB", "North Vert Bad", 1920, -0.5, 1919.5);
  for (int ih = 0; ih<4; ih++) { 
    Onlmonserver->registerHisto(parent,bad[ih]); 
  }

  cable[0] = new TH1F("MUI_CABLE_H", "South Horiz cable", 120, -0.5, 119.5);
  cable[1] = new TH1F("MUI_CABLE_V", "South Vert cable", 120, -0.5, 119.5);
  cable[2] = new TH1F("MUI_N_CABLE_H", "North Horiz cable", 120, -0.5, 119.5);
  cable[3] = new TH1F("MUI_N_CABLE_V", "North Vert cable", 120, -0.5, 119.5);
  for (int ih = 0; ih<4; ih++) {  
    Onlmonserver->registerHisto(parent,cable[ih]); 
  }

  prevcable[0] = new TH1F("MUI_OLDCABLE_H", "South Horiz cable", 120, -0.5, 119.5);
  prevcable[1] = new TH1F("MUI_OLDCABLE_V", "South Vert cable", 120, -0.5, 119.5);
  prevcable[2] = new TH1F("MUI_N_OLDCABLE_H", "North Horiz cable", 120, -0.5, 119.5);
  prevcable[3] = new TH1F("MUI_N_OLDCABLE_V", "North Vert cable", 120, -0.5, 119.5);
  for (int ih = 0; ih<4; ih++) { 
    Onlmonserver->registerHisto(parent,prevcable[ih]); 
  }

  cable_rate[0] = new TH1F("MUI_S_H_CABLE_RATE", "South Horiz cable hit rate", 120, -0.5, 119.5);
  cable_rate[1] = new TH1F("MUI_S_V_CABLE_RATE", "South Vert cable hit rate", 120, -0.5, 119.5);
  cable_rate[2] = new TH1F("MUI_N_H_CABLE_RATE", "North Horiz cable hit rate", 120, -0.5, 119.5);
  cable_rate[3] = new TH1F("MUI_N_V_CABLE_RATE", "North Vert cable hit rate", 120, -0.5, 119.5);
  for (int ih = 0; ih<4; ih++) {  
    Onlmonserver->registerHisto(parent,cable_rate[ih]); 
  }

  roc_totals[0] = new TH1F("MUI_S_H_ROC_TOTALS", "South Horiz Hits per ROC", 20, -0.5, 19.5);
  roc_totals[1] = new TH1F("MUI_S_V_ROC_TOTALS", "South Vert Hits per ROC", 20, -0.5, 19.5);
  roc_totals[2] = new TH1F("MUI_N_H_ROC_TOTALS", "North Horiz Hits per ROC", 20, -0.5, 19.5);
  roc_totals[3] = new TH1F("MUI_N_V_ROC_TOTALS", "North Vert Hits per ROC", 20, -0.5, 19.5);
  for (int ih = 0; ih<4; ih++) {  
    Onlmonserver->registerHisto(parent,roc_totals[ih]); 
  }

  ostringstream name, title;
  for (int i = 0; i < MAX_ROC;i++)
    {
      int iplane = i / 4;
      int iroc = i % 4;
      name.str("");
      title.str("");
      name << "MUI_SH" << iplane << iroc;
      title << "South Horiz Plane" << iplane << " Roc" << iroc;
      roc[0][i] = new TH1F((name.str()).c_str(), (title.str()).c_str(), 96, -0.5, 95.5);

      name.str("");
      title.str("");
      name << "MUI_SV" << iplane << iroc;
      title << "South Vertical Plane" << iplane << " Roc" << iroc;
      roc[1][i] = new TH1F((name.str()).c_str(), (title.str()).c_str(), 96, -0.5, 95.5);

      name.str("");
      title.str("");
      name << "MUI_NH" << iplane << iroc;
      title << "North Horiz Plane" << iplane << " Roc" << iroc;
      roc[2][i] = new TH1F((name.str()).c_str(), (title.str()).c_str(), 96, -0.5, 95.5);

      name.str("");
      title.str("");
      name << "MUI_NV" << iplane << iroc;
      title << "North Vertical Plane" << iplane << " Roc" << iroc;
      roc[3][i] = new TH1F((name.str()).c_str(), (title.str()).c_str(), 96, -0.5, 95.5);

      for (int ih = 0; ih<4; ih++) {  
	Onlmonserver->registerHisto(parent,roc[ih][i]); 
      }
    }

  roc2d[0] = new TH2F("MUI_SH2D", "South Horiz", 15, -0.5, 14.5, 128, -0.5, 127.5);
  roc2d[1] = new TH2F("MUI_SV2D", "South Vert", 192, -0.5, 191.5, 10, -0.5, 9.5);
  roc2d[2] = new TH2F("MUI_NH2D", "North Horiz", 15, -0.5, 14.5, 128, -0.5, 127.5);
  roc2d[3] = new TH2F("MUI_NV2D", "North Vert", 192, -0.5, 191.5, 10, -0.5, 9.5);
  for (int ih = 0; ih<4; ih++) {  
    Onlmonserver->registerHisto(parent,roc2d[ih]); 
  }

  Num_Hits = new TH1F("MUI_TotalHits", "Total Hits", 100, 0, MAX_TOTAL_HITS);
  Onlmonserver->registerHisto(parent,Num_Hits);

  plane_hist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SPlane", "South Total Hits in each Plane", 5, -0.5, 4.5);
  plane_hist[MUIMONCOORD::NORTH] = new TH1F("MUI_NPlane", "North Total Hits in each Plane", 5, -0.5, 4.5);
  panel_hist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SPanel", "South Total Hits in each Panel", 6, -0.5, 5.5);
  panel_hist[MUIMONCOORD::NORTH] = new TH1F("MUI_NPanel", "North Total Hits in each Panel", 6, -0.5, 5.5);
  mainFrame[MUIMONCOORD::SOUTH] = new TH1F("MUI_SmainFrameHits", "South Hits Per Main Frame", 3, -0.5, 2.5);
  mainFrame[MUIMONCOORD::NORTH] = new TH1F("MUI_NmainFrameHits", "North Hits Per Main Frame", 3, -0.5, 2.5);

  for (int ih = 0; ih<2; ih++) {  
    Onlmonserver->registerHisto(parent,plane_hist[ih]); 
    Onlmonserver->registerHisto(parent,panel_hist[ih]); 
    Onlmonserver->registerHisto(parent,mainFrame[ih]); 
  }
}

/////////////////////////////////////////////////////////////////////////////
void origHist::event(hit_vector hits)
{
  muiMonGlobals* globals = muiMonGlobals::Instance();

  hit_iter it;
  num_event->Fill(1);
  Num_Hits->Fill(hits.size());
  for ( it = hits.begin();it != hits.end();it++)
    {
      all[(*it).fem]->Fill(((*it).roc * WORD_PER_ROC
                            + (*it).word) * BIT_PER_WORD + (*it).bit );
      roc[(*it).fem][(*it).roc]->Fill( (*it).word * BIT_PER_WORD + (*it).bit );
      roc_totals[(*it).fem]->Fill((*it).roc);
      cable[(*it).fem]->Fill( (*it).roc * WORD_PER_ROC + (*it).word );


      if ( (*it).plane >= 0 )
        {
          plane_hist[(*it).arm]->Fill( (*it).plane );
          panel_hist[(*it).arm]->Fill( (*it).panel );
          //Hardcoded mainframe mapping
          if ((*it).arm == 0)
            {
              if (((*it).panel == 0) || ((*it).panel == 2))
                {
                  mainFrame[MUIMONCOORD::SOUTH]->Fill( 0 );
                }
              else if (((*it).panel == 1) || ((*it).panel == 4))
                {
                  mainFrame[MUIMONCOORD::SOUTH]->Fill( 1 );
                }
              else if (((*it).panel == 3) || ((*it).panel == 5))
                {
                  mainFrame[MUIMONCOORD::SOUTH]->Fill( 2 );
                }
            }
          else
            {
              if (((*it).panel == 1) || ((*it).panel == 4))
                {
                  mainFrame[MUIMONCOORD::NORTH]->Fill( 1 );
                }
              else if ((*it).orient == 0)
                {
                  mainFrame[MUIMONCOORD::NORTH]->Fill( 0 );
                }
              else if ((*it).orient == 1)
                {
                  mainFrame[MUIMONCOORD::NORTH]->Fill( 2 );
                }
            }
        }
      else
        {
          //        cout << " Event " << eventNum
          ostringstream msg;
          msg << " no physical channel "
	      << " ROC " << (*it).roc
	      << " WORD " << (*it).word
	      << " BIT " << (*it).bit
	      << " hits = " << hits.size();
          msg_control *muimsg = new msg_control(MSG_TYPE_MONITORING,
                                                MSG_SOURCE_MUID,
                                                MSG_SEV_ERROR, "MUID origHist");
          cout << *muimsg << PHWHERE << msg << endl;
          delete muimsg;
          string ThisName = "MUIDMONITOR";
          OnlMonServer *se = OnlMonServer::instance();
          se->WriteLogFile(ThisName, msg.str());

          bad[(*it).fem]->Fill(((*it).roc*WORD_PER_ROC + (*it).word) * BIT_PER_WORD + (*it).bit );
        }
    }

  if (globals->eventNum % globals->cable_hist_update_freq == 0)
    {
      for (int i = 0;i < MAX_FEM;i++)
        {
          prevcable[i]->Reset();
          prevcable[i]->Add(cable[i]);
          cable[i]->Reset();
        }
    }

  //TVC 05/18/2003 - I think that this is not correct code.
  // I have commented it out and replaced with the marked line.
  //  int cableEvents = 0;
  //  if (globals->eventNum < globals->cable_hist_update_freq)
  //    {
  //     cableEvents = globals->eventNum % globals->cable_hist_update_freq;
  //    }
  //  else
  //    {
  //      cableEvents = globals->cable_hist_update_freq + globals->eventNum % globals->cable_hist_update_freq;
  //    }
  //  int cableEvents = globals->eventNum; // This is the corrected line mentioned immediately above.
  for (int i = 0;i < MAX_FEM;i++)
    {
      cable_rate[i]->Reset();
      cable_rate[i]->Add(cable[i]);
      cable_rate[i]->Add(prevcable[i]);
      //Easiest thing for now is to avoid scaling this guy here.
      //I should probably use the unscaled histogram, but I don't quite see how.
      // Comment out for now.
      //TVC 05/18/2003
      //      cable_rate[i]->Scale(1. / cableEvents);
    }

}

/////////////////////////////////////////////////////////////////////////////
void origHist::clear()
{
  num_event->Reset();
  for ( short arm = 0; arm < MAX_ARM; arm++ )
    {
      plane_hist[arm]->Reset();
      panel_hist[arm]->Reset();
      mainFrame[arm]->Reset();
      for ( short orient = 0; orient < MAX_ORIENTATION; orient++ )
        {
          int ifem = arm * MAX_ORIENTATION + orient;
          all[ifem]->Reset();
          bad[ifem]->Reset();
          cable[ifem]->Reset();
          prevcable[ifem]->Reset();
          cable_rate[ifem]->Reset();
          roc_totals[ifem]->Reset();
          for ( int iroc = 0; iroc < MAX_ROC; iroc++ )
            {
              roc[ifem][iroc]->Reset();
            }
          roc2d[ifem]->Reset();
        }
    }
  Num_Hits->Reset();
}

/////////////////////////////////////////////////////////////////////////////
void origHist::draw()
{
  TCanvas *muiOrigCanvas = new TCanvas("muiOrigCanvas", "Raw Hits", -200, 200, 400, 600);
  muiOrigCanvas->Divide(1, 4);
  for (int i = 0; i < MAX_FEM; i++)
    {
      TH1F* temp1 = (TH1F*) all[i];
      muiOrigCanvas->cd(i + 1);
      temp1->Draw();
    }
}

/////////////////////////////////////////////////////////////////////////////
void origHist::analyzeEvent()
{
  muiMonGlobals* globals = muiMonGlobals::Instance();


  if ( (globals->eventNum % globals->dead_mainFrame_freq == 0) && (globals->eventNum > 0) )
    {
      ostringstream msg;
      msg << "Probable failure of : " << endl;
      int nfailures = 0;
      if (mainFrame[MUIMONCOORD::SOUTH]->GetBinContent(1) < 1)
        {
          globals->mainFrameErrors[MUIMONCOORD::SOUTH]++;
          nfailures++;
          msg << " south upper panel HV main frame. " << endl;
        }
      if (mainFrame[MUIMONCOORD::SOUTH]->GetBinContent(2) < 1)
        {
          globals->mainFrameErrors[MUIMONCOORD::SOUTH]++;
          nfailures++;
          msg << " south small panel HV main frame. " << endl;
        }
      if (mainFrame[MUIMONCOORD::SOUTH]->GetBinContent(3) < 1)
        {
          globals->mainFrameErrors[MUIMONCOORD::SOUTH]++;
          nfailures++;
          msg << " south lower panel HV main frame. " << endl;
        }
      if (mainFrame[MUIMONCOORD::NORTH]->GetBinContent(1) < 1)
        {
          globals->mainFrameErrors[MUIMONCOORD::NORTH]++;
          nfailures++;
          msg << " north horizontal HV main frame. " << endl;
        }
      if (mainFrame[MUIMONCOORD::NORTH]->GetBinContent(2) < 1)
        {
          globals->mainFrameErrors[MUIMONCOORD::NORTH]++;
          nfailures++;
          msg << " north small panel HV main frame. " << endl;
        }
      if (mainFrame[MUIMONCOORD::NORTH]->GetBinContent(3) < 1)
        {
          globals->mainFrameErrors[MUIMONCOORD::NORTH]++;
          nfailures++;
          msg << " north vertical HV main frame. ";
        }

      if (nfailures > 0)
        {
          msg_control *muimsg = new msg_control(MSG_TYPE_MONITORING,
                                                MSG_SOURCE_MUID,
                                                MSG_SEV_ERROR, "MUID origHist");
          cout << *muimsg << PHWHERE << msg << endl;
          delete muimsg;
          string ThisName = "MUIDMONITOR";
          OnlMonServer *se = OnlMonServer::instance();
          se->WriteLogFile(ThisName, msg.str());
        }

      mainFrame[MUIMONCOORD::NORTH]->Reset();
      mainFrame[MUIMONCOORD::SOUTH]->Reset();
    }
}






