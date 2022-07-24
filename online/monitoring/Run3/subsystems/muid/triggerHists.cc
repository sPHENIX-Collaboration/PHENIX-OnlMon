#include <iostream>
#include "TH1.h"
#include "muiMonGlobals.h"
#include "OnlMonServer.h"
#include "muid_blt.h"
#include "triggerHists.h"

using namespace std;
 
triggerHists::triggerHists(OnlMon *myparent): obj_hist(myparent)
{
  //Create emulator and histograms.
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  triggerEmu = new muid_blt(Onlmonserver->RunNumber());

  bltTrigEff = new TH1F("MUI_bltTrigEff", "whole Trigger Efficiencies", 12 , -0.5, 11.5);
  bltTrigEfe = new TH1F("MUI_bltTrigEfe", "whole Trigger Eff error", 12 , -0.5, 11.5);
  bltTrigEfn = new TH1F("MUI_bltTrigEfn", "whole Trigger Eff nominator", 12 , -0.5, 11.5);
  bltTrigEfd = new TH1F("MUI_bltTrigEfd", "whole Trigger Eff denominator", 12 , -0.5, 11.5);
 
  bltQuadEff = new TH1F("MUI_bltQuadEff", "whole Quadrant Efficiencies", 8 , -0.5, 7.5);

  bltTrigEff0 = new TH1F("MUI_bltTrigEff0", "S0", 16 , -0.5, 15.5);
  bltTrigEff1 = new TH1F("MUI_bltTrigEff1", "S1", 16 , -0.5, 15.5);
  bltTrigEff2 = new TH1F("MUI_bltTrigEff2", "S2", 16 , -0.5, 15.5);
  bltTrigEff3 = new TH1F("MUI_bltTrigEff3", "S3", 16 , -0.5, 15.5);
  bltTrigEff4 = new TH1F("MUI_bltTrigEff4", "N0", 16 , -0.5, 15.5);
  bltTrigEff5 = new TH1F("MUI_bltTrigEff5", "N1", 16 , -0.5, 15.5);
  bltTrigEff6 = new TH1F("MUI_bltTrigEff6", "N2", 16 , -0.5, 15.5);
  bltTrigEff7 = new TH1F("MUI_bltTrigEff7", "N3", 16 , -0.5, 15.5);

  bltTrigEffa0 = new TH1F("MUI_bltTrigEffa0", "S0a", 16 , -0.5, 15.5);
  bltTrigEffa1 = new TH1F("MUI_bltTrigEffa1", "S1a", 16 , -0.5, 15.5);
  bltTrigEffa2 = new TH1F("MUI_bltTrigEffa2", "S2a", 16 , -0.5, 15.5);
  bltTrigEffa3 = new TH1F("MUI_bltTrigEffa3", "S3a", 16 , -0.5, 15.5);
  bltTrigEffa4 = new TH1F("MUI_bltTrigEffa4", "N0a", 16 , -0.5, 15.5);
  bltTrigEffa5 = new TH1F("MUI_bltTrigEffa5", "N1a", 16 , -0.5, 15.5);
  bltTrigEffa6 = new TH1F("MUI_bltTrigEffa6", "N2a", 16 , -0.5, 15.5);
  bltTrigEffa7 = new TH1F("MUI_bltTrigEffa7", "N3a", 16 , -0.5, 15.5);

  //
  Onlmonserver->registerHisto(parent,bltTrigEff);
  Onlmonserver->registerHisto(parent,bltTrigEfe);
  Onlmonserver->registerHisto(parent,bltTrigEfn);
  Onlmonserver->registerHisto(parent,bltTrigEfd);
 
  Onlmonserver->registerHisto(parent,bltQuadEff);

  Onlmonserver->registerHisto(parent,bltTrigEff0);
  Onlmonserver->registerHisto(parent,bltTrigEff1);
  Onlmonserver->registerHisto(parent,bltTrigEff2);
  Onlmonserver->registerHisto(parent,bltTrigEff3);
  Onlmonserver->registerHisto(parent,bltTrigEff4);
  Onlmonserver->registerHisto(parent,bltTrigEff5);
  Onlmonserver->registerHisto(parent,bltTrigEff6);
  Onlmonserver->registerHisto(parent,bltTrigEff7);
 
  Onlmonserver->registerHisto(parent,bltTrigEffa0);
  Onlmonserver->registerHisto(parent,bltTrigEffa1);
  Onlmonserver->registerHisto(parent,bltTrigEffa2);
  Onlmonserver->registerHisto(parent,bltTrigEffa3);
  Onlmonserver->registerHisto(parent,bltTrigEffa4);
  Onlmonserver->registerHisto(parent,bltTrigEffa5);
  Onlmonserver->registerHisto(parent,bltTrigEffa6);
  Onlmonserver->registerHisto(parent,bltTrigEffa7);
}

triggerHists::~triggerHists()
{
  //delete what we made.
  delete bltTrigEff;
  delete bltTrigEfe;
  delete bltTrigEfn;
  delete bltTrigEfd;
  delete bltQuadEff;

  delete bltTrigEff0;
  delete bltTrigEff1;
  delete bltTrigEff2;
  delete bltTrigEff3;
  delete bltTrigEff4;
  delete bltTrigEff5;
  delete bltTrigEff6;
  delete bltTrigEff7;

  delete bltTrigEffa0;
  delete bltTrigEffa1;
  delete bltTrigEffa2;
  delete bltTrigEffa3;
  delete bltTrigEffa4;
  delete bltTrigEffa5;
  delete bltTrigEffa6;
  delete bltTrigEffa7;
  delete triggerEmu;
}

int triggerHists::event(hit_vector rawHits, int liveTrigBits, int scaledTrigBits)
{
  int roc_words[4][20][6];
  for (int i = 0;i < 4; i++)
    {
      for (int j = 0;j < 20; j++)
        {
          for (int k = 0;k < 6; k++)
            {
              roc_words[i][j][k] = 0;
            }
        }
    }

  //Pack the data into the array used by the emulator.
  
  
  for (hit_iter it = rawHits.begin();it != rawHits.end(); it++)
    {
      roc_words[(*it).fem][(*it).roc][(*it).word]
      = roc_words[(*it).fem][(*it).roc][(*it).word] | 1 << (*it).bit;
    }

  triggerEmu->set_all(roc_words, liveTrigBits, scaledTrigBits);

  float   eff[12], ereff[12]; 
  int   nomin[12], denom[12];
  for (int i = 0;i < 12;i++)
    {
           eff[i] = -1;
	 ereff[i] = -1;
         nomin[i] = -1;
         denom[i] = -1;
     }

  float   q_eff[2][4], q_ereff[2][4]; 
  int   q_nomin[2][4], q_denom[2][4];
  for (int i = 0;i < 2;i++)
    {
      for (int j = 0;j < 4;j++)
        {
             q_eff[i][j] = -2;
	   q_ereff[i][j] = -2;
           q_nomin[i][j] = -2;
           q_denom[i][j] = -2;
        }
     }

  float   m_eff[2][4][16], m_ereff[2][4][16]; 
  int   m_nomin[2][4][16], m_denom[2][4][16];
  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 4; j++)
        {
         for (int k = 0; k < 16; k++)
           {
               m_eff[i][j][k] = -3;
	     m_ereff[i][j][k] = -3;
             m_nomin[i][j][k] = -3;
             m_denom[i][j][k] = -3;
           }
         }
      }
 
  float   ma_eff[2][4][16], ma_ereff[2][4][16]; 
  int   ma_nomin[2][4][16], ma_denom[2][4][16];
  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 4; j++)
        {
         for (int k = 0; k < 16; k++)
           {
               ma_eff[i][j][k] = -4;
	     ma_ereff[i][j][k] = -4;
             ma_nomin[i][j][k] = -4;
             ma_denom[i][j][k] = -4;
           }
         }
      }
 
  triggerEmu->get_whole_eff(nomin, denom);
  triggerEmu->get_quad_eff(q_nomin, q_denom);
  triggerEmu->get_masked_eff(m_nomin, m_denom);
  triggerEmu->get_activated_eff(ma_nomin, ma_denom);

  float wh_eff=0, wh_err=0;
  for (int i = 0; i < 12; i++)
    {
      get_eff(nomin[i], denom[i], wh_eff, wh_err );
        eff[i] = wh_eff;
      ereff[i] = wh_err; 
   }    

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 4; j++)
        {
          get_eff(q_nomin[i][j], q_denom[i][j], wh_eff, wh_err );
          q_eff[i][j]    = wh_eff;
          q_ereff[i][j]  = wh_err;           
        }
      }

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 4; j++)
        {
         for (int k = 0; k < 16; k++)
           {
           get_eff(m_nomin[i][j][k], m_denom[i][j][k], wh_eff, wh_err );
            m_eff[i][j][k]    = wh_eff;
            m_ereff[i][j][k]  = wh_err;           
        }
      }
    }

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 4; j++)
        {
         for (int k = 0; k < 16; k++)
           {
           get_eff(ma_nomin[i][j][k], ma_denom[i][j][k], wh_eff, wh_err );
            ma_eff[i][j][k]    = wh_eff;
            ma_ereff[i][j][k]  = wh_err;           
        }
      }
    }

  bltTrigEff->Reset();
  bltTrigEfe->Reset();
  bltTrigEfd->Reset();
  bltTrigEfn->Reset();
  for (int i = 0; i < 12; i++)
    {
          //Fill the histogram with the efficiencies.
           bltTrigEff->Fill(i, eff[i]);         
           bltTrigEfe->Fill(i, ereff[i]);         
           bltTrigEfn->Fill(i, nomin[i]);         
           bltTrigEfd->Fill(i, denom[i]);         
    }

  bltQuadEff->Reset();
  int k = 0;
  for (int i = 0; i < 2; i++)
    {
    for (int j = 0; j < 4; j++)
      {
          //Fill the histogram with the efficiencies.
          bltQuadEff->Fill(k, q_eff[i][j]);         
          k++;        
       }
     }
 
  bltTrigEff0->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff0->Fill(i, m_eff[0][0][i]);
   }
 
  bltTrigEff1->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff1->Fill(i, m_eff[0][1][i]);
    }
 
  bltTrigEff2->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff2->Fill(i, m_eff[0][2][i]);
    }
 
  bltTrigEff3->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff3->Fill(i, m_eff[0][3][i]);
    }

  bltTrigEff4->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff4->Fill(i, m_eff[1][0][i]);
    }

  bltTrigEff5->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff5->Fill(i, m_eff[1][1][i]);
    }

  bltTrigEff6->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff6->Fill(i, m_eff[1][2][i]);
    }

  bltTrigEff7->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEff7->Fill(i, m_eff[1][3][i]);
    }

  bltTrigEffa0->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa0->Fill(i, ma_eff[0][0][i]);
   }
 
  bltTrigEffa1->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa1->Fill(i, ma_eff[0][1][i]);
    }
 
  bltTrigEffa2->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa2->Fill(i, ma_eff[0][2][i]);
    }
 
  bltTrigEffa3->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa3->Fill(i, ma_eff[0][3][i]);
    }

  bltTrigEffa4->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa4->Fill(i, ma_eff[1][0][i]);
    }

  bltTrigEffa5->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa5->Fill(i, ma_eff[1][1][i]);
    }

  bltTrigEffa6->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa6->Fill(i, ma_eff[1][2][i]);
    }

  bltTrigEffa7->Reset();
  for (int i = 0;i < 16;i++)
    {
          //Fill the histogram with the efficiencies.
          bltTrigEffa7->Fill(i, ma_eff[1][3][i]);
    }

  return 0;
}

void triggerHists::clear()
{
  bltTrigEff->Reset();
  bltTrigEfe->Reset();
  bltTrigEfn->Reset();
  bltTrigEfd->Reset();

  bltQuadEff->Reset();

  bltTrigEff0->Reset();
  bltTrigEff1->Reset();
  bltTrigEff2->Reset();
  bltTrigEff3->Reset();
  bltTrigEff4->Reset();  
  bltTrigEff5->Reset();
  bltTrigEff6->Reset();
  bltTrigEff7->Reset();

  bltTrigEffa0->Reset();
  bltTrigEffa1->Reset();
  bltTrigEffa2->Reset();
  bltTrigEffa3->Reset();
  bltTrigEffa4->Reset();  
  bltTrigEffa5->Reset();
  bltTrigEffa6->Reset();
  bltTrigEffa7->Reset();
}


