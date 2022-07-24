#include <corHist.h>
#include <OnlMonServer.h>
#include <TH2.h>
#include <iostream>
#include <sstream>

using namespace std;

corHist::corHist(OnlMon *myparent): obj_hist(myparent)
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  ostringstream name, title;
  for (int igap = 0; igap < GAP_PER_ARM - 1;igap++)
    {
      for (int ipanel = 0; ipanel < PANEL_PER_GAP;ipanel++)
        {
	  name.str(""); title.str("");
          name << "MUI_COR_SH" << igap << "_" << ipanel;
          title << "Hit Corr:South Horizontal,Panel " << ipanel << " Gap" << igap << "-" << (igap + 1);
          cor[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ][igap][ipanel] = new TH2F((name.str()).c_str(), (title.str()).c_str(), 
									       64, -0.5, 63.5, 64, -0.5, 63.5);
          Onlmonserver->registerHisto(parent,cor[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ][igap][ipanel]);
	  
	  name.str(""); title.str("");
          name << "MUI_COR_SV" << igap << "_" << ipanel;
          title << "Hit Corr:South Vertical,Panel " << ipanel << " Gap" << igap << "-" << (igap + 1);	  
          cor[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT][igap][ipanel] = new TH2F((name.str()).c_str(), (title.str()).c_str(), 
									      64, -0.5, 63.5, 64, -0.5, 63.5);
          Onlmonserver->registerHisto(parent,cor[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT][igap][ipanel]);

	  name.str(""); title.str("");
          name << "MUI_COR_NH" << igap << "_" << ipanel;
          title << "Hit Corr:North Horizontal,Panel " << ipanel << " Gap" << igap << "-" << (igap + 1);
          cor[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ][igap][ipanel] = new TH2F((name.str()).c_str(), (title.str()).c_str(), 
									       64, -0.5, 63.5, 64, -0.5, 63.5);
          Onlmonserver->registerHisto(parent,cor[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ][igap][ipanel]);

	  name.str(""); title.str("");
          name << "MUI_COR_NV" << igap << "_" << ipanel;
          title << "Hit Corr:North Vertical,Panel " << ipanel << " Gap" << igap << "-" << (igap + 1);
          cor[MUIMONCOORD::NORTH][MUIMONCOORD::VERT][igap][ipanel] = new TH2F((name.str()).c_str(), (title.str()).c_str(), 
									      64, -0.5, 63.5, 64, -0.5, 63.5);
          Onlmonserver->registerHisto(parent,cor[MUIMONCOORD::NORTH][MUIMONCOORD::VERT][igap][ipanel]);
        }
    }
}

void corHist::event(hit_vector rawHits)
{
  int cross[MAX_ARM][MAX_ORIENTATION][GAP_PER_ARM][PANEL_PER_GAP];
  int posit[MAX_ARM][MAX_ORIENTATION][GAP_PER_ARM][PANEL_PER_GAP];

  //reset area  for correlation histograms
  for (int iarm = 0;iarm < MAX_ARM;iarm++)
    {
      for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
        {
          for (int igap = 0; igap < GAP_PER_ARM; igap++)
            {
              for (int ipanel = 0; ipanel < PANEL_PER_GAP; ipanel++)
                {
                  cross[iarm][iorient][igap][ipanel] = -1;
                  posit[iarm][iorient][igap][ipanel] = -1;
                }
            }
        }
    }

  //fill area for correlation histograms
  hit_iter it;
  for ( it = rawHits.begin();it != rawHits.end();it++)
    {
      posit[(*it).arm][(*it).orient][(*it).plane][(*it).panel] = (*it).twopack;
      cross[(*it).arm][(*it).orient][(*it).plane][(*it).panel]++;
    }

  for (int iarm = 0;iarm < MAX_ARM;iarm++)
    {
      for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
        {
          for (int igap = 0; igap < GAP_PER_ARM - 1; igap++)
            {
              for (int ipanel = 0; ipanel < PANEL_PER_GAP; ipanel++)
                {
                  if ((cross[iarm][iorient][igap][ipanel] == 0) &&
                      (cross[iarm][iorient][igap + 1][ipanel] == 0))
                    {
                      cor[iarm][iorient][igap][ipanel]->
                      Fill(posit[iarm][iorient][igap][ipanel],
                           posit[iarm][iorient][igap + 1][ipanel]);
                    }
                }
            }
        }
    }
}

void corHist::clear()
{
  for (int iarm = 0;iarm < MAX_ARM;iarm++)
    {
      for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
        {
          for (int igap = 0; igap < GAP_PER_ARM - 1; igap++)
            {
              for (int ipanel = 0; ipanel < PANEL_PER_GAP; ipanel++)
                {
                  cor[iarm][iorient][igap][ipanel]->Reset();
                }
            }
        }
    }
}

void corHist::draw()
{
}
