#include "ERTMaskMonCommon.h"
#include "ERTMaskMonHisto.h"
#include "EMCalRichDecodeOnline.h"

#include "OnlMonServer.h"
#include "OnlMonClient.h"

#include "TH1.h"
#include "TH2.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace ERTMaskMonCommon;

ERTMaskMonHisto::ERTMaskMonHisto()
{
   m_Evtcount = 0;
   memset(m_hsm, 0, sizeof(m_hsm));
}

ERTMaskMonHisto::~ERTMaskMonHisto()
{
   ;
}

void ERTMaskMonHisto::RegisterHistos(OnlMon *myparent)
{
   OnlMonServer *se = OnlMonServer::instance();
   
   // event number counter
   m_Evtcount = new TH1F("ert_Evtcount", "Evtcount", 4, 0, 4);
   se->registerHisto(myparent, m_Evtcount);
   
   // nhit of each SM
   char hname[128];
   for (int i = 0; i < N_NHIT_HIST; i++)
   {
      int n_sm = i < 4  ?  N_SM_EMC_TOTAL  :  N_SM_RICH_TOTAL;
      sprintf(hname, "ert_hsm_%i", i);
      m_hsm[i] = new TH1F(hname, "", n_sm, 0, n_sm);
      se->registerHisto(myparent, m_hsm[i]);
   }
}

int ERTMaskMonHisto::GetHistos()
{
   m_Evtcount = 0;
   for (int i = 0; i < N_NHIT_HIST; i++) m_hsm[i] = 0;

   OnlMonClient *cl = OnlMonClient::instance();

   m_Evtcount = cl->getHisto("ert_Evtcount");
   if (! m_Evtcount) return 1; // means the server is dead

   char hname[128];
   for (int i = 0; i < N_NHIT_HIST; i++)
   {
      sprintf(hname, "ert_hsm_%i", i);
      m_hsm[i] = cl->getHisto(hname);
   }
   return 0;
}
