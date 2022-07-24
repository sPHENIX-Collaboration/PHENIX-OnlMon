#include "PbGlUMon.h"
#include "PbGlUDBodbc.h"

#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <Event.h>
#include <msg_profile.h>

#include <TH2.h>
#include <TString.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <TSystem.h>

using namespace std;

PbGlUMon::PbGlUMon(const char *name): OnlMon("PbGlUMon")
{
  WRITEDB = 0;
  DEBUGPBGLU = 0;
  NO_ACTION = 1;

  fhvinit = 0;
  he = 0;
  hv_cycle_enabled = 0;
  db = 0;
  return ;
}

PbGlUMon::~PbGlUMon()
{
  delete he;
  delete db;
  return ;
}

int PbGlUMon::Init()
{

  string skipfile = "/home/phnxpbgl/stkav/Usuppression/skips";

  sig_weight[0] = 0;
  sig_weight[1] = 1;
  sig_weight[2] = 2;


  for (int i = 0;i < 9216;i++)
    {
    for (unsigned int j = 0;j < threshold*2;j++)
      {
      for (int k = 0;k < number_of_signals;k++)
	{
        stor[i][j][k] = -1000000;
	}
      }
    }

  for (int i = 0;i < 9216;i++)
    {
    cure_time[i] = -1000000;
    }
  memset(skip,0,sizeof(skip));

  for (int i = 0;i < 9216;i++)
    {
    bbads[i] = -1000000;
    }
  ifstream sin(skipfile.c_str());
  if (sin.is_open())
    {
      int skee;
      while (!sin.eof())
        {
          sin >> skee;
          if (skee < 0 || skee >= 9216)
	    {
            continue;
	    }
          skip[skee] = 1;
        }
    }


  hv_init();

  he	= new TH2F("he", "he", 8 * 12, 0, 8 * 12, 4 * 12 * 2, 0, 4 * 12 * 2);
  fne = 0;
  ped_count = 0;

  hese	= new TH2F("pbglu_hese", "PbGl signals", 8 * 12, 0, 8 * 12, 4 * 12 * 2, 0, 4 * 12 * 2);
  hbese	= new TH2F("pbglu_hbese", "PbGl bads", 8 * 12, 0, 8 * 12, 4 * 12 * 2, 0, 4 * 12 * 2);
  hcese = new TH2F("pbglu_hcese", "PbGl fixed", 8 * 12, 0, 8 * 12, 4 * 12 * 2, 0, 4 * 12 * 2);

  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(this,hese);
  Onlmonserver->registerHisto(this,hbese);
  Onlmonserver->registerHisto(this,hcese);

  // check if we run for real (env var PBGLU_SETHV is set in the .login
  // of the phnxrc account
  if (getenv("ONLMON_PBGL_HV_ENABLE"))
    {
      printf("PBGLUMon: Env var ONLMON_PBGL_HV_ENABLE set, HV recylcing is enabled\n");
      hv_cycle_enabled = 1;
    }
  else
    {
      printf("dummy mode, HV will not be set\n");
    }
  db = new PbGlUDBodbc("PBGLHV");
  return 0;
}

int PbGlUMon::process_event(Event *evt)
{
  time_t ticks = evt->getTime();

  //OnlMonServer *Onlmonserver= OnlMonServer::instance();
  int cur_time = (OnlMonServer::instance())->CurrentTicks();

  Packet *pGL1 = evt->getPacket(14001);
  int ppg = 0;
  if (pGL1)
    {
      unsigned rbib = pGL1->iValue(0, "LIVETRIG");
      ppg = (rbib >> 28) & 3;

      delete pGL1;
    }
  if (ppg == 1)
    //if (Onlmonserver->Trigger("PPG(Pedestal)"))
    {
      if (DEBUGPBGLU)
	{
        printf("Pedestal event!\n");
	}

      fne += 1;
      ped_count++;
      if (DEBUGPBGLU)
	{
        printf("%d %f\n", ped_count, fne);
	}

      if (ped_count % 10 == 0 && fne != 0)
        {
          hese->Reset();
          hese->Add(he, 1);
          hese->Scale(1 / fne);

          hbese->Reset();
          hcese->Reset();
          for (int i = 0;i < 9216;i++)
            {
              if (cur_time - cure_time[i] < min_rep_fix_time)
                {
                  int se, z, y, fem;
                  PbGlGeom(i, se, z, y, fem);
                  he->SetBinContent(z + 1, y + 1 + se*48, 0);
                  hcese->SetBinContent(z + 1, y + 1 + se*48, 1);
                }
              if (cur_time - bbads[i] < min_rep_fix_time)
                {
                  int se, z, y, fem;
                  PbGlGeom(i, se, z, y, fem);
                  hbese->SetBinContent(z + 1, y + 1 + se*48, 1);

                }

            }

        }

      for (int ip = 8109; ip < 8173; ip++)
        {
          Packet *p = evt->getPacket(ip);
          if (p)
            {
              for (int i = 0; i < 144; i++)
                {
                  //int tdc = p->iValue(i, 0);
                  //	int lgpost = p->iValue(i, 2);
                  int lgpre = p->iValue(i, 4);
                  int hgpost = p->iValue(i, 1);
                  int hgpre = p->iValue(i, 3);
                  // 	int lg = - lgpre + lgpost;
                  int hg = - hgpre + hgpost;
                  int pbglch = (ip - 8109) * 144 + i;
                  if (hg > 400)
                    if (check_signal(pbglch, ticks, 2))
                      {
                        if (nomercy(pbglch))
                          {
                            cure_time[pbglch] = ticks;
                          }
                      }
                  if (hgpre == 4095)
                    if (check_signal(pbglch, ticks, 0))
                      {
                        if (nomercy(pbglch))
                          {
                            cure_time[pbglch] = ticks;
                          }
                      }
                  if (lgpre == 4095)
                    if (check_signal(pbglch, ticks, 1))
                      {
                        if (nomercy(pbglch))
                          {
                            cure_time[pbglch] = ticks;
                          }
                      }

                } //for ich
              delete p;
            } //if p
        } //for ip
    } // if pedestal

  return 0;
}


int PbGlUMon::check_signal(const int ch, const int ticks, const int signal)
{
  OnlMonServer *server = OnlMonServer::instance();
  if (ch >= 9216 || ch < 0)
    {
      if (DEBUGPBGLU)
	{
        printf("ERROR: Bad channel! %d\n", ch);
	}
      return -1;
    }
  if (signal < 0 || signal >= number_of_signals)
    {
      if (DEBUGPBGLU)
	{
        printf("ERROR: Bad signal! %d\n", signal);
	}
      return -1;
    }

  int cur_time = server->CurrentTicks();

  if (cure_time[ch] > ticks)
    {
    return 0;
    }
  if (DEBUGPBGLU)
    {
    printf("signal %d for channel %d at %d\n", signal, ch, ticks);
    }
  if (WRITEDB)
    {
      db->AddRow(server->RunNumber(),cur_time,ch,"pbglusig");
    }
  int se, z, y, fem;
  if (PbGlGeom(ch, se, z, y, fem))
    {
      he->Fill(z, y + se*4*12);
    }

  for (int i = threshold * 2 - 1;i > 0;i--)
    {
    stor[ch][i][signal] = stor[ch][i - 1][signal];
    }
  stor[ch][0][signal] = ticks;
  unsigned int sum = 0;
  if (max_fix_time != 0)
    {
    if ( -ticks + cur_time > max_fix_time)
      {
        return 0;
      }
    }
  for (int i = 0;i < number_of_signals;i++)
    {
    for (unsigned int j = 0;j < threshold*2;j++)
      {
      if (stor[ch][j][i] > 0)
	{
        if (ticks - stor[ch][j][i] < max_time)
          {
            sum += sig_weight[i];
          }
	}
      }
    }

  if (sum > threshold)
    {
      if (cur_time - cure_time[ch] < min_rep_fix_time)
        {
          bbads[ch] = cur_time;
          return 0;
        }

      for (int i = 0;i < number_of_signals;i++)
	{
        for (unsigned int j = 0;j < threshold*2;j++)
	  {
          stor[ch][j][i] = -100000;
	  }
	}
      return 1;
    }
    return 0;
}

int
PbGlUMon::nomercy(const int ch)
{
  OnlMonServer *server = OnlMonServer::instance();
  if (NO_ACTION)
    {
      printf("channel %d: chpok! \n", ch);
      if (WRITEDB)
        {
          db->AddRow(server->RunNumber(),time(0),ch,"pbglufix no action");
        }
      return 1;
    }
  if (!fhvinit || skip[ch])
    {
      if (WRITEDB)
        {
          db->AddRow(server->RunNumber(),time(0),ch,"pbglufix skip/no hv");
        }
      return 0;
    }
  char command[100];
  if (DEBUGPBGLU)
    {
      printf("DEBUG: changing hv for module #%d %d->500\n", ch, hvs[ch]);
    } 

  sprintf(command, "hivoc_set_channel %d %d %d", units[ch], nunits[ch], 500);
  if (DEBUGPBGLU)
    {
      printf("DEBUG: issueing : %s\n", command);
    }
  if (WRITEDB)
    {
      db->AddRow(server->RunNumber(),time(0),ch,"pbglufix start");
    }

  if (hv_cycle_enabled)
    {
      gSystem->Exec(command);
    }

  if (DEBUGPBGLU)
    {
      sprintf(command, "hivoc_get_Vref %d %d ", units[ch], nunits[ch]);

      if (hv_cycle_enabled)
        {
          gSystem->Exec(command);
        }
    }
  gSystem->Sleep(5000);
  if (DEBUGPBGLU)
    {
      sprintf(command, "hivoc_get_Vref %d %d ", units[ch], nunits[ch]);
      if (hv_cycle_enabled)
        {
          gSystem->Exec(command);
        }
      printf("DEBUG: changing hv for module #%d 1000->%d\n", ch, hvs[ch]);
    }

  sprintf(command, "hivoc_set_channel %d %d %d", units[ch], nunits[ch], hvs[ch]);

  if (DEBUGPBGLU)
    {
      printf("DEBUG: issueing : %s\n", command);
    }

  if (hv_cycle_enabled)
    {
      gSystem->Exec(command);
    }
  if (DEBUGPBGLU)
    {
      sprintf(command, "hivoc_get_Vref %d %d ", units[ch], nunits[ch]);

      if (hv_cycle_enabled)
        {
          gSystem->Exec(command);
        }
    }
  if (WRITEDB)
    {
      db->AddRow(server->RunNumber(),time(0),ch,"pbglufix done");
    }

  return 1;

}

void PbGlUMon::hv_init()
{
  const char * hv_file = HV_FILE;
  ifstream f(hv_file);
  if (!f.is_open())
    {
      printf("Can't open file %s for mapping. Exiting\n", HV_FILE);
      return ;
    }

  printf("Setting iocondev2 (HIVOC machine) address: %s\n", HIVOC_IP);
  gSystem->Setenv("DAQ_RPC_HOST", HIVOC_IP);

  TString stemp(gSystem->Getenv("PATH"));
  stemp += ":/home/phnxpbgl/pbgl_HV/HIVOC/actual";
  gSystem->Setenv("PATH", stemp.Data());

  char dummy[200];

  f >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;

  int s, r, c, mo, b, subunit, unit, fem, channel, t, hv ;
  for (int i = 0;i < 9216;i++)
    {
      f >> s >> r >> c >> mo >> b >> subunit >> unit >> fem >> channel >> t >> hv;

      int nunit = (subunit - 1) * 24 + mo;
      int absnumber = s * 4608 + fem * 144 + channel;

      hvs[absnumber] = hv;
      units[absnumber] = unit;
      nunits[absnumber] = nunit;
    }
  f.close();
  fhvinit = 1;
}

int PbGlUMon::PbGlGeom(int index1, int& sector, int &zrow, int &yrow, int &femId)
{
  if (index1 < 0 || index1 >= 2*32*144)
    {
      return 0;
    }

  sector = index1 / (32 * 144);

  int id = index1 % (32 * 144);

  int femzrow = (id / 144) % 8;
  int femyrow = (id / 144) / 8;
  int idFem = id % 144;


  int idUnit = idFem / 4;
  yrow = 5 - (idUnit % 6);
  zrow = (idUnit / 6);

  int unit = idFem % 4;
  int y = unit / 2;
  int z = 1 - (unit % 2);

  zrow = femzrow * 12 + zrow * 2 + z;
  yrow = femyrow * 12 + yrow * 2 + y;

  //************************************
  int zSm = zrow / 6;
  int ySm = yrow / 4;

  femId = (11 - ySm) * 8 + (7 - (zSm % 8));
  femId = femId + (1 - zrow / 48) * 144;
  femId = femId + 144 * 2 * sector;

  return 1;
}
