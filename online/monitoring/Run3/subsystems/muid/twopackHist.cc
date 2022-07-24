#include <iostream>
#include <sstream>
#include "TH1.h"
#include "OnlMonServer.h"
#include "twopackHist.h"

using namespace std;

twopackHist::twopackHist(OnlMon *myparent): obj_hist(myparent)
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  ostringstream name, title;
  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int igap = 0; igap < GAP_PER_ARM; igap++)
        {
          for (int ipanel = 0; ipanel < PANEL_PER_GAP; ipanel++)
            {
              for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
                {
		  name.str(""); title.str("");
                  name << "MUI_twopack" << iarm << igap << ipanel << iorient;
                  title << "Arm" << iarm << " Gap" << igap << " Panel" << ipanel 
			       << " Orient" << iorient << " 2packhist",
                  twopack[iarm][igap][ipanel][iorient] = new TH1F((name.str()).c_str(), (title.str()).c_str(), 
								       66, -0.5, 65.5);
                  Onlmonserver->registerHisto(parent,twopack[iarm][igap][ipanel][iorient]);
                }
            }
        }
    }

  HVHits[0][0] = new TH1F("MUI_SH_HVhits", "South Horiz hits per HV chain", 80, -0.5, 79.5);
  Onlmonserver->registerHisto(parent,HVHits[0][0]);
  HVHits[0][1] = new TH1F("MUI_SV_HVhits", "South Vert hits per HV chain", 70, -0.5, 69.5);
  Onlmonserver->registerHisto(parent,HVHits[0][1]);
  HVHits[1][0] = new TH1F("MUI_NH_HVhits", "North Horiz hits per HV chain", 80, -0.5, 79.5);
  Onlmonserver->registerHisto(parent,HVHits[1][0]);
  HVHits[1][1] = new TH1F("MUI_NV_HVhits", "North Vert hits per HV chain", 70, -0.5, 69.5);
  Onlmonserver->registerHisto(parent,HVHits[1][1]);

}


void twopackHist::event(hit_vector rawHits)
{
  hit_iter it;
  for ( it = rawHits.begin();it != rawHits.end();it++)
    {
      twopack[(*it).arm][(*it).plane][(*it).panel][(*it).orient]->Fill((*it).twopack);
    }

  static const double HVdivisionX1[6][2]
  =
    {
      {
        18.5, 21.5
      }
      , { -0.5, -0.5}, {18.5, 21.5}, {19.5, 21.5}, { -0.5, -0.5}, {19.5, 21.5}
    };
  static const double HVdivisionX2[6][2]
  =
    {
      {
        38.5, 43.5
      }
      , {22.5, -0.5}, {38.5, 43.5}, {39.5, 43.5}, {22.5, -0.5}, {39.5, 43.5}
    };

  TH1* MuidTempHist;
  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int iori = 0; iori < MAX_ORIENTATION; iori++)
        {
          int counter = 1;
          HVHits[iarm][iori]->Reset();
          for (int igap = 0; igap < GAP_PER_ARM; igap++)
            {
              for (int ipanel = 0; ipanel < PANEL_PER_GAP; ipanel++)
                {
                  MuidTempHist = twopack[iarm][igap][ipanel][iori];
                  if (HVdivisionX1[ipanel][iori] != -0.5)
                    {
                      HVHits[iarm][iori]->SetBinContent(counter++, MuidTempHist->Integral(1, (int)(HVdivisionX1[ipanel][iori] + 0.5)));
                    }
                  if (HVdivisionX2[ipanel][iori] != -0.5)
                    {
                      HVHits[iarm][iori]->SetBinContent(counter++, MuidTempHist->Integral((int)(HVdivisionX1[ipanel][iori] + 1.5),
                                      (int)(HVdivisionX2[ipanel][iori] + 0.5)));
                    }
                  HVHits[iarm][iori]->SetBinContent(counter++, MuidTempHist->Integral((int)(HVdivisionX2[ipanel][iori] + 1.5), 65));
                }
            }
        }
    }


}

