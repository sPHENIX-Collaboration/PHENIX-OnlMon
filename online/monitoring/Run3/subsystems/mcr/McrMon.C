#include <McrMon.h>
#include <McrSend.h>
#include <McrMonHistoBinDefs.h>
#include <OnlMonServer.h>
#include <OnlMonDB.h>

#include <BbcEvent.hh>
#include <BbcCalib.hh>
#include <ZdcEvent.hh>
#include <ZdcCalib.hh>

#include <Event.h>
#include <EventTypes.h>

#include <TH1.h>
#include <TH2.h>

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

static const int ZDC_PACKET_ID = 13001;
static const int BBC_PACKET_ID = 1002;
enum {BBCDET, ZDCDET, LAST};
static const int nHistoBins = 100;
static const int UPDATETICKSMCR = 300;
static const int UPDATETICKSDB = 650;

string varname[2] = {"bbcll1vtx", "zdcll1vtx"};

McrMon::McrMon(const char *thisname): OnlMon(thisname)
{
  for (int i = 0; i < LAST;i++)
    {
      zvertexhist[i] = 0;
      zvertexDB[i] = 0;
      zvertex[i] = 0;
      nBins[i] = 250;
      vtxrange[i] = 250;
    }

  zdccalib = 0;
  zevt = 0;
  bbccalib = 0;
  bevt = 0;

  runnumber = 0;
  dbvars = new OnlMonDB(ThisName);
  DBVarInit();
  mcrsend = new McrSend(this);
  SetGoodEvents(4000, 0);
  SetGoodEvents(1000, 1);
  VertexCalc("ZDC");
  return ;
}

McrMon::~McrMon()
{
  delete bbccalib;
  delete bevt;
  delete dbvars;
  delete mcrsend;
  delete zdccalib;
  delete zevt;
  for (int i = 0; i < 2;i++)
    {
      delete zvertexDB[i];
      delete zvertex[i];
    }
  return ;
}

void
McrMon::SetGoodEvents(const unsigned int i, const unsigned int j)
{
  if (j < LAST)
    {
      nGoodEvents[j] = i;
    }
  else
    {
      printf("Bad index for arry, min: 0, max: %d, your choice: %d\n", LAST, j);
    }
  return ;
}

int
McrMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();
  zvertex[BBCLL1] = new TH1F("bbcll1_zvertex",
                             "BBCLL1 ZVertex",
                             nBins[BBCLL1], -vtxrange[0], vtxrange[0]);
  zvertex[ZDCLL1] = new TH1F("zdcll1_zvertex",
                             "ZDC NS ZVertex",
                             nBins[ZDCLL1], -vtxrange[1], vtxrange[1]);
  for (int i = 0; i < LAST; i++)
    {
      zvertexDB[i] = static_cast <TH1 *>(zvertex[i]->Clone());
      zvertexDB[i]->SetName(varname[i].c_str());
    }

  mcrerror = new TH1F("mcr_error", "Mcr Error histogram", MCRMONHISTOBINS, -0.5, (MCRMONHISTOBINS - 0.5));
  se->registerHisto(this,mcrerror);
  mcrstat = new TH1F("mcr_stat", "Mcr Statistics histogram", LAST, -0.5, (LAST - 0.5));
  se->registerHisto(this,mcrstat);
  if (vtxdetstr == "BBC")
    {
      vtxdet = BBCDET;
      bbccalib = new BbcCalib();
      bevt = new BbcEvent();
    }
  else if (vtxdetstr == "ZDC")
    {
      vtxdet = ZDCDET;
      zdccalib = new ZdcCalib();
      zevt = new ZdcEvent();
    }
  else
    {
      cout << PHWHERE << " Invalid vtx detector choice: " << vtxdetstr << endl;
      exit(1);
    }
  Reset();
  return 0;
}

int
McrMon::Create2dHistos()
{
  static const char *name[] =
    {"mcr_bbcll1_zvertex", "mcr_zdcll1_zvertex"
    };
  zvertexhist[BBCLL1] = new TH2F("mcr_1", "BBCLL1 ZVertex History",
                                 nBins[BBCLL1], -vtxrange[0], vtxrange[0],
                                 nHistoBins, -0.5, (nHistoBins - 0.5) );
  zvertexhist[ZDCLL1] = new TH2F("mcr_2", "ZDCLL1 ZVertex History",
                                 nBins[ZDCLL1], -vtxrange[1], vtxrange[1],
                                 nHistoBins, -0.5, (nHistoBins - 0.5) );
  OnlMonServer *se = OnlMonServer::instance();
  for (int i = 0; i < LAST;i++)
    {
      se->registerHisto(this->Name(),name[i], zvertexhist[i], 1);
    }
  return 0;
}

int McrMon::Reset()
{
  for (int i = 0; i < LAST; i++)
    {
      nevts[i] = 0;
      zvertex[i]->Reset();
      zvertexDB[i]->Reset();
      ifill[i] = 0;
      currentslice[i] = 1;
      LastUpdateMcr[i] = 0;
      LastUpdateDB[i] = 0;
      enoughEventsDB[i] = 0;
      TotalEvents[i] = 0;
    }
  Create2dHistos();
  return 0;
}

int
McrMon::process_event(Event *evt)
{
  if (evt->getEvtType() != DATAEVENT)
    {
      return 0;
    }
  OnlMonServer *se = OnlMonServer::instance();
  int dbcommit = 0;
  if ((vtxdet == ZDCDET && evt->existPacket(ZDC_PACKET_ID)) || (vtxdet == BBCDET && evt->existPacket(BBC_PACKET_ID)))
    {
      for (int i = 0;i < LAST;i++)
        {
          // 	  cout << "Trigger: 0x" << hex << se->Trigger(2)
          // 	       << " TrigBits: 0x" << trigbit[i] << dec << endl;
          if (se->Trigger(2) & trigbit[i])
            {
              float zvtx;
              if (vtxdet == ZDCDET)
                {
                  zevt->Clear();
                  zevt->setRawData(evt);
                  zevt->calculate();
                  zvtx = zevt->getZvertex();
                }
              else if (vtxdet == BBCDET)
                {
                  bevt->Clear();
                  bevt->setRawData(evt);
                  bevt->calculate();
                  zvtx = bevt->getZVertex();
                }
              else
                {
                  cout << PHWHERE << " Bad vtxdet: " << vtxdet << endl;
                  exit(1);
                }
              handleVertex(zvtx, i, 1);
            }

          if (!enoughEventsDB[i])
            {
              if (zvertexDB[i]->GetEntries() < nGoodEvents[i])
                {
                  enoughEventsDB[i] = 0;
                }
              else
                {
                  enoughEventsDB[i] = 1;
                }
            }

          if (enoughEventsDB[i] && se->CurrentTicks() - LastUpdateDB[i] > UPDATETICKSDB)
            {
              dbcommit = 1;
            }
        }

      // here we know we should commit stuff to our DB
      if (dbcommit)
        {
          for (int j = 0; j < LAST;j++)
            {
              // only commit values from histos above a certain no of events
              if (zvertexDB[j]->GetEntries() > nGoodEvents[j])
                {
                  dbvars->SetVar(varname[j],
                                 zvertexDB[j]->GetMean(),
                                 zvertexDB[j]->GetRMS(),
                                 zvertexDB[j]->GetEntries());
                }
              else
                {
                  dbvars->SetVar(varname[j], -9999, 0, 0);
                }
            }
          if (dbvars->DBcommit() == 0)
            {

              for (int j = 0; j < LAST;j++)
                {
                  // only reset histos which were above threshold
                  if (zvertexDB[j]->GetEntries() > nGoodEvents[j])
                    {

                      enoughEventsDB[j] = 0;
                      LastUpdateDB[j] = se->CurrentTicks();
                      zvertexDB[j]->Reset();
                    }
                }
            }

        }
    }
  //   else
  //     {
  //       printf("no ZDC packet\n");
  //     }
  return 0;
}

void
McrMon::setup_calibration(const int runnumber)
{
  if (runnumber > 0)
    {
      if (zdccalib)
        {
          ostringstream zdccalibdir;
          if (getenv("ZDCCALIBDIR"))
            {
              zdccalibdir << getenv("ZDCCALIBDIR");
            }
          zdccalibdir << "/ZdcCalib" ;
          zdccalib->restore(zdccalibdir.str().c_str());
          PHTimeStamp now;
          now.setToSystemTime();
          zevt->setCalibDataAll(zdccalib, now);
        }
      if (bbccalib)
        {
          if (!getenv("BBCCALIBDIR"))
            {
              OnlMonServer *se = OnlMonServer::instance();
              ostringstream msg;
              msg << "Environment variable BBCCALIBDIR not set";
              se->send_message(this, MSG_SOURCE_BBC, MSG_SEV_ERROR, msg.str(), 2);

              bbccalib->restore("./BbcCalib");
            }
          else
            {
              ostringstream otmp;
              otmp << getenv("BBCCALIBDIR") << "/BbcCalib";
              bbccalib->restore(otmp.str().c_str());
            }
          bevt->setCalibDataAll(bbccalib);
        }

    }
  return ;
}


int
McrMon::BeginRun(const int runno)
{
  // setup run information
  OnlMonServer *se = OnlMonServer::instance();
  runnumber = runno;
  setup_calibration(runno);
  set<string>::const_iterator iter;
  for (int i = 0; i < LAST; i++)
    {
      trigbit[i] = 0;
      for (iter = triggername[i].begin(); iter != triggername[i].end(); iter++)
	{
	  trigbit[i] |= se->getLevel1Bit(*iter);
	  // here we set the trigger mask in the et pool,
	  // our monitoring won't get events of other triggers
	  // (but still all other event types like scaler,begin run etc.)
	  // adding multiple triggers is possible (they will be or'ed)
	  se->AddToTriggerMask(*iter);
      }
  }

  // store the required counts (the reset after each run deletes the histo content)
  // +1 because histogram bins start at 1, not zero
  mcrstat->SetBinError(BBCLL1 + 1, nGoodEvents[BBCLL1]);
  mcrstat->SetBinError(ZDCLL1 + 1, nGoodEvents[ZDCLL1]);
  return 0;
}

int
McrMon::Update2d(const int iwhat)
{
  ifill[iwhat]++;
  unsigned int nbinsy = zvertexhist[iwhat]->GetNbinsY();
  if (ifill[iwhat] > nbinsy)
    {
      string hisname = zvertexhist[iwhat]->GetName();
      TH2 *tmphis = new TH2F("newguy", zvertexhist[iwhat]->GetTitle(), zvertexhist[iwhat]->GetNbinsX(), -vtxrange[iwhat], vtxrange[iwhat], (nbinsy + nHistoBins), -0.5, (nbinsy + nHistoBins - 0.5));
      for (int ibin = 0; ibin <= zvertexhist[iwhat]->GetNbinsX(); ibin++)
        {
          for (int jbin = 0; jbin <= zvertexhist[iwhat]->GetNbinsY(); jbin++)
            {
              tmphis->SetBinContent(ibin, jbin, zvertexhist[iwhat]->GetBinContent(ibin, jbin));
              tmphis->SetBinError(ibin, jbin, zvertexhist[iwhat]->GetBinError(ibin, jbin));
            }
        }
      zvertexhist[iwhat] = tmphis;
      OnlMonServer *se = OnlMonServer::instance();
      se->registerHisto(this->Name(),hisname.c_str(), zvertexhist[iwhat], 1);
    }
  for (int ibin = 0; ibin <= zvertex[iwhat]->GetNbinsX(); ibin++)
    {
      zvertexhist[iwhat]->SetBinContent(ibin, currentslice[iwhat], zvertex[iwhat]->GetBinContent(ibin));
      zvertexhist[iwhat]->SetBinError(ibin, currentslice[iwhat], zvertex[iwhat]->GetBinError(ibin));
    }
  currentslice[iwhat]++;
  zvertex[iwhat]->Reset();
  return 0;
}

// histogram GetMean() and GetRMS() give identical values than the ones
// computed by hand here
// int
// McrMon::CalculateMeanRMS(const int iwhat)
// {
//   mean[iwhat] = 0;
//   for (unsigned int i = 0; i<nevts[iwhat]; i++)
//     {
//       mean[iwhat] += vtxpos[i][iwhat];
//     }
//   mean[iwhat] /= nevts[iwhat];
//   RMS[iwhat] = 0;
//   for (unsigned int i = 0; i<nevts[iwhat]; i++)
//     {
//       RMS[iwhat] += (vtxpos[i][iwhat]-mean[iwhat])*(vtxpos[i][iwhat]-mean[iwhat]);
//     }
//   RMS[iwhat] = sqrt(RMS[iwhat]/nevts[iwhat]);
//   return 0;
// }

int
McrMon::handleVertex(const float vtx, const int iwhat, const int SendToMcr)
{
  if (fabs(vtx) < vtxrange[iwhat])
    {
      zvertex[iwhat]->Fill( vtx );
      zvertexDB[iwhat]->Fill( vtx );
      nevts[iwhat]++;
      TotalEvents[iwhat]++;
      mcrstat->SetBinContent(iwhat + 1, TotalEvents[iwhat]);
      OnlMonServer *se = OnlMonServer::instance();
      time_t CurrTicks = se->CurrentTicks();
      if (nevts[iwhat] > nGoodEvents[iwhat])
        {
          if (CurrTicks - LastUpdateMcr[iwhat] > UPDATETICKSMCR)
            {

              if (SendToMcr)
                {
                  int status = mcrsend->SendHisto(zvertex[iwhat], CurrTicks, se->RunNumber(), iwhat);
                  mcrerror->SetBinContent(RHICSERVERSTATUSBIN, status);
                }
              LastUpdateMcr[iwhat] = CurrTicks;
              Update2d(iwhat);
              nevts[iwhat] = 0;
            }
        }
    }
  return 0;
}

int
McrMon::DBVarInit()
{
  for (int i = 0; i < LAST;i++)
    {
      dbvars->registerVar(varname[i]);
    }
  dbvars->DBInit();
  return 0;
}
