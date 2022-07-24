#include "GranuleMonDefs.h"
#include "DaqMonDraw.h"
#include "GranMonDraw.h"
#include "DaqHistoBinDefs.h"
#include <HistoBinDefs.h>
#include <OnlMonClient.h>
#include <OnlMonDB.h>
#include <OnlMonDBReturnCodes.h>
#include <RunDBodbc.h>

#include <phool.h>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TImage.h>
#include <TLine.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm> // for min_element
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// boost::split makes this version gcc 4.4.7  barf otherwise
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 4)

#pragma GCC diagnostic ignored "-Warray-bounds"

#endif 
// boost::split makes this version gcc 4.3.2  barf otherwise
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 3)

#pragma GCC diagnostic ignored "-Warray-bounds"

#endif 

using namespace std;

// for finding the smallest non zero element
class FindMinNotZero
{
public:
  bool operator() (float newx, float x)
  {
    if (x <= 0)
      {
        x = 1000;
      }
    return ((newx > 0) && (x > newx));
  }
};

DaqMonDraw *DaqMonDraw::__instance = NULL;

DaqMonDraw *DaqMonDraw::instance(const char *name)
{
  if (__instance)
    {
      return __instance;
    }
  __instance = new DaqMonDraw(name);
  return __instance;
}

DaqMonDraw::DaqMonDraw(const char *name): 
  OnlMonDraw(name),
  htmloutflag(0),
  drawstop(0),
  rundb(NULL),
  abortgapmessage(1),
  multibufferedTicks(1200)
{
  CanvasSize( -1, 0, 0);
  dbvars = new OnlMonDB(ThisName);
  memset(gr, 0, sizeof(gr));
  return ;
}

DaqMonDraw::~DaqMonDraw()
{
  while (Granule.begin() != Granule.end())
    {
      delete Granule.back();
      Granule.pop_back();
    }
  for (int i = 0; i < 3*NRTYPES; i++)
    {
      delete gr[i];
    }
  delete rundb;
  delete dbvars;
  map<const string, TH1 *>::iterator hiter;
  for (hiter = HistoMap.begin(); hiter != HistoMap.end(); ++hiter)
    {
      delete hiter->second;
    }
  HistoMap.clear();
  __instance = NULL;
  return ;
}

int DaqMonDraw::MakeCanvas(const char *name)
{

  if (!strcmp(name, "DaqMon1"))
    {
      TC1 = new TCanvas("DaqMon1", "DaqMon Granule Overview", -1, 0,
                        canvassize[0][0], canvassize[0][1] );
      gSystem->ProcessEvents();
      char padname[10];
      int ipad = 0;
      for (short int i = 0; i < PLOTROWS; i++)
        {
          // up to 0,9 to make space for the run number
          double ylow = 0.9 * (1 - (i + 1) / (float)PLOTROWS );
          double yhi = 0.9 * (1 - i / (float)PLOTROWS);
          for (short int j = 0; j < PLOTCOLUMNS; j++)
            {
              double xlow = j * 1. / float(PLOTCOLUMNS);
              double xhi = (j + 1) / float(PLOTCOLUMNS);
              sprintf(padname, "Pad1%02d", ipad);
              Pad1[ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
              Pad1[ipad]->Draw();
              Pad1[ipad]->SetEditable(kFALSE);
              ipad++;
            }
        }
      transparent1 = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent1->SetFillStyle(4000);
      transparent1->Draw();
      transparent1->SetEditable(kFALSE);
    }
  else if (!strcmp(name, "DaqMon2"))
    {
      TC2 = new TCanvas("DaqMon2", "DaqMon Granule Status", -1, 0, 800, 900);
      gSystem->ProcessEvents();
      transparent2 = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
      transparent2->SetFillStyle(4000);
      transparent2->Draw();
      transparent2->SetEditable(kFALSE);
      char padname[10];
      int ipad = 0;
      for (short int i = 0; i < 6; i++)
        {
          // up to 0,9 to make space for the run number
          double ylow = 0.9 * (1 - (i + 1) / 6.);
          double yhi = 0.9 * (1 - i / 6.);
          for (short int j = 0; j < 2; j++)
            {
              double xlow = j * 1. / 2.;
              double xhi = (j + 1) / 2.;
              sprintf(padname, "Pad2%02d", ipad);
              Pad2[ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
              Pad2[ipad]->SetFillStyle(4000);
              Pad2[ipad]->Draw();
              //          Pad2[ipad]->SetEditable(kFALSE);
              ipad++;
            }
        }
    }
  else if (!strcmp(name, "DaqMon3"))
    {
      TC3 = new TCanvas("DaqMon3", "DaqMon Global Status", -1, 0, 800, 900);
      gSystem->ProcessEvents();
      transparent3 = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
      transparent3->SetFillStyle(4000);
      transparent3->Draw();
      transparent3->SetEditable(kFALSE);
      Pad3[0] = new TPad("Pad300", "this does not show", 0, 0.67, 1, 1, 0);
      Pad3[0]->SetFillStyle(4000);
      Pad3[0]->Draw();
      Pad3[1] = new TPad("Pad301", "this does not show", 0, 0.33, 1, 0.67, 0);
      Pad3[1]->SetFillStyle(4000);
      Pad3[1]->Draw();
      Pad3[2] = new TPad("Pad302", "this does not show", 0, 0, 1, 0.33, 0);
      Pad3[2]->SetFillStyle(4000);
      Pad3[2]->Draw();
    }
  else if (!strcmp(name, "DaqMon4"))
    {
      TC4 = new TCanvas("DaqMon4", "DaqMon Low Runnumber Events", -1, 0, 800, 900);
      gSystem->ProcessEvents();
      transparent4 = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
      transparent4->SetFillStyle(4000);
      transparent4->Draw();
      transparent4->SetEditable(kFALSE);
      Pad4[0] = new TPad("Pad400", "this does not show", 0, 0.7, 0.5, 0.9, 0);
      Pad4[0]->SetFillStyle(4000);
      Pad4[0]->Draw();
      Pad4[1] = new TPad("Pad401", "this does not show", 0.5, 0.7, 1., 0.9, 0);
      Pad4[1]->SetFillStyle(4000);
      Pad4[1]->Draw();

      Pad4[2] = new TPad("Pad402", "this does not show", 0, 0.5, 0.5, 0.7, 0);
      Pad4[2]->SetFillStyle(4000);
      Pad4[2]->Draw();
      Pad4[3] = new TPad("Pad403", "this does not show", 0.5, 0.5, 1., 0.7, 0);
      Pad4[3]->SetFillStyle(4000);
      Pad4[3]->Draw();

      Pad4[4] = new TPad("Pad404", "this does not show", 0, 0.2, 1, 0.5, 0);
      Pad4[4]->SetFillStyle(4000);
      Pad4[4]->Draw();
      Pad4[5] = new TPad("Pad405", "this does not show", 0, 0, 0.5, 0.2, 0);
      Pad4[5]->SetFillStyle(4000);
      Pad4[5]->Draw();
      Pad4[6] = new TPad("Pad406", "this does not show", 0.5, 0, 1, 0.2, 0);
      Pad4[6]->SetFillStyle(4000);
      Pad4[6]->Draw();
    }
  else if (!strcmp(name, "DaqMon5"))
    {
      TC5 = new TCanvas("DaqMon5", "DaqMon Vtxp", -1, 0, 1200, 500);
      gSystem->ProcessEvents();
      Pad5 = new TPad("Pad500", "this does not show", 0.0, 0.0, 1.0, 0.9);
      Pad5->SetFillStyle(4000);
      Pad5->SetFillColor(0);
      Pad5->Draw();
      transparent5 = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
      transparent5->SetFillStyle(4000);
      transparent5->Draw();
      transparent5->SetEditable(kFALSE);
    }
  else
    {
      cout << "You are ordering an unknown canvas: " << name << endl;
      return -1;
    }
  return 0;
}

int DaqMonDraw::Draw(const char *what)
{
  if (!strcmp(what, "ALL") || !strcmp(what, "DAQ"))
    {
      return (DrawAllGranules("ALL"));
    }
  else if (!strcmp(what, "GLOBAL"))
    {
      return (DrawGlobal("ALL"));
    }
  else if (!strcmp(what, "LOWRUN"))
    {
      return (DrawLowRun("ALL"));
    }
  // a little cmd parsing, Command syntax is DAQ%<subsystem>%<what>
  string cmd = what;
  vector<string> tokens;
  boost::split(tokens,cmd,boost::is_any_of("%"));
  int numtok = tokens.size();
  switch(numtok)
    {
    case 1:
      cout << "option " << tokens[0] << " not implemented" << endl;
      return -1;
      break;
    case 2:
      return DrawGranule(tokens[1].c_str());
      break;
    case 3:
      {
      map<string, GranMonDraw *>::iterator iter = granmap.find(tokens[1]);
      if (iter != granmap.end())
	{
	  return iter->second->DrawThisSubsystem(tokens[2].c_str());
	}
      }
      break;
    default:
      break;
    }
  return -1;
}

int DaqMonDraw::DrawVtxpGranule(const int packetid)
{
  if (packetid < 24001 || packetid > 24060)
    {
      cout << "DaqMonDraw::DrawVtxpGranule Error passing invalid packetid " << packetid << endl;
    }

  OnlMonClient *cl = OnlMonClient::instance();
  const int spiro = packetid - 24001;
  if (packetid >=  24041 && packetid <= 24045)
    {
      return -1;
    }
  ostringstream hname("");
  hname << "Daq_svxpixel_h1statusbit_" << spiro;
  TH1* hstatus = cl->getHisto(hname.str().c_str());
  if (! hstatus)
    {
      return -1; // bail out if histo for this packet does not exist (we run with a vtxp subset)
    }

  if (! gROOT->FindObject("DaqMon5"))
    {
      MakeCanvas("DaqMon5");
    }

  TC5->cd();
  TC5->Clear("D");
  TC5->SetLogy(1);

  transparent5->cd();
  transparent5->SetEditable(kTRUE);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.07);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream, datestream;
  //string runstring;
  runnostream << "Run " << cl->RunNumber() << " Packetid: " << packetid;
  hstatus->SetMinimum(0.1);
  Pad5->cd();
  Pad5->SetLogy(1);
  hstatus->SetFillColor(0);
  hstatus->SetStats(0);
  hstatus->SetMinimum(0.1);
  hstatus->Draw();

  transparent5->Clear();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC5->Update();
  return 0;
}

int
DaqMonDraw::DrawGlobal(const char *what)
{
  if (! gROOT->FindObject("DaqMon3"))
    {
      MakeCanvas("DaqMon3");
    }
  OnlMonClient *cl = OnlMonClient::instance();
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  vector<float> var;
  vector<float> varerr;
  vector<time_t> timestamp;
  vector<int> runnumber;
  // this sets the time range from which values should be returned
  time_t end = cl->EventTime();              // current event time
  if (end == 0)
    {
      end = time(NULL); // current time (right NOW)
    }

  time_t begin = end - 24 * 60 * 60; // previous 24 hours
  end += 600; // add 10 minutes to search window so we do not miss the current timestp
  TH2 *h1 = NULL;
  vector<int>::iterator minrun, maxrun;
  float *x = NULL;
  float *y = NULL;
  float *ex = NULL;
  float *ey = NULL;
  int n = 0;
  static const string rtyp[NRTYPES] =
    {"PHYSICS", "JUNK", "CALIBRATION", "PEDESTAL", "PREJECTED", "ZEROFIELD", "VERNIERSCAN", "LOCALPOLARIMETER"
    };
  static const int mcol[NRTYPES] =
    {
      2, 3, 4, 1, 8, 6, 7, 9
    };
  static const float ypos[NRTYPES] = {0.53, 0.58, 0.63, 0.68, 0.73, 0.78, 0.83, 0.88};
  TText runtype;
  runtype.SetTextFont(62);
  runtype.SetTextSize(0.04);
  runtype.SetNDC();
  runtype.SetTextAlign(13);

  Pad3[0]->cd();
  Pad3[0]->SetLogy();
  string varname = "daqcorrupt";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  int foundcorrupt = 1;
  TText canvmsg;
  switch (iret)
    {
    case DBOKAY:
      foundcorrupt = 1;
      break;
    case DBNOENTRIES:
      canvmsg.SetTextFont(62);
      canvmsg.SetTextSize(0.08);
      canvmsg.SetTextColor(3);
      canvmsg.SetNDC();  // set to normalized coordinates
      canvmsg.SetTextAlign(23); // center/top alignment
      canvmsg.DrawText(0.5, 0.7, "Congratulations");
      canvmsg.DrawText(0.5, 0.5, "No Corrupt Events");
      canvmsg.DrawText(0.5, 0.3, "for the last 24 hours");
      foundcorrupt = 0;
      break;
    default:
      cout << PHWHERE << " ReturnCode: " << iret
           << " Error in db access, no data?" << endl;
      return 0;
    }
  if (foundcorrupt)
    {
      // timestamps come sorted in ascending order
      float histomin = 0.00005;
      // find the minimum non zero value of this vector
      vector<float>::iterator minelement = std::min_element(var.begin(), var.end(), FindMinNotZero());
      if (minelement != var.end())
        {
          float vecmin = (*minelement) / 2;
          if (vecmin <= 0)
            {
              //            TText canvmsg;
              canvmsg.SetTextFont(62);
              canvmsg.SetTextSize(0.08);
              canvmsg.SetTextColor(3);
              canvmsg.SetNDC();  // set to normalized coordinates
              canvmsg.SetTextAlign(23); // center/top alignment
              canvmsg.DrawText(0.5, 0.7, "Congratulations");
              canvmsg.DrawText(0.5, 0.5, "No Corrupt Events");
              canvmsg.DrawText(0.5, 0.3, "for the last 24 hours");
              goto nocorrupt;
            }
          if (vecmin < histomin)
            {
              histomin = vecmin;
            }
        }
      minrun = std::min_element(runnumber.begin(), runnumber.end());
      int graphtimebegin = *minrun - 10;
      maxrun = std::max_element(runnumber.begin(), runnumber.end());
      int graphtimeend = *maxrun + 10;

      h1 = new TH2F("h1", "Corrupted Event fraction last 24 hours", 2, graphtimebegin, graphtimeend, 2, histomin, 1.1);
      h1->SetMinimum(histomin);
      h1->SetMaximum(1.1);
      h1->SetStats(kFALSE);
      h1->GetXaxis()->SetNoExponent();
      h1->GetXaxis()->SetTitle("Run");
      h1->GetXaxis()->SetLabelSize(0.03);
      h1->DrawCopy();
      delete h1;
      x = new float[var.size()];
      y = new float[var.size()];
      ex = new float[var.size()];
      ey = new float[var.size()];
      n = var.size();
      for (int j = 0; j < NRTYPES; j++)
        {
          if (gr[j])
            {
              delete gr[j];
              gr[j] = 0;
            }
          for (unsigned int i = 0; i < var.size(); i++)
            {
              x[i] = runnumber[i];
              if (GetRunType(runnumber[i]) == rtyp[j])
                {
                  y[i] = var[i];
                }
              else
                {
                  y[i] = 0;
                }

              ex[i] = 0;
              ey[i] = varerr[i];
            }
          gr[j] = new TGraphErrors(n, x, y, ex, ey);
          gr[j]->SetMarkerColor(mcol[j]);
          gr[j]->SetLineColor(mcol[j]);
          gr[j]->SetMarkerStyle(21);
          gr[j]->Draw("LP");
          runtype.SetTextColor(mcol[j]);
          runtype.DrawText(0.7, ypos[j], rtyp[j].c_str());
        }
      delete [] x;
      delete [] y;
      delete [] ex;
      delete [] ey;
    }
 nocorrupt:
  varname = "daqmisspkt";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  Pad3[1]->cd();
  Pad3[1]->SetLogy();
  int foundmissing = 1;
  switch (iret)
    {
    case DBOKAY:
      foundmissing = 1;
      break;
    case DBNOENTRIES:
      canvmsg.SetTextFont(62);
      canvmsg.SetTextSize(0.08);
      canvmsg.SetTextColor(3);
      canvmsg.SetNDC();  // set to normalized coordinates
      canvmsg.SetTextAlign(23); // center/top alignment
      canvmsg.DrawText(0.5, 0.7, "Congratulations");
      canvmsg.DrawText(0.5, 0.5, "No Missing Packets");
      canvmsg.DrawText(0.5, 0.3, "for the last 24 hours");
      foundmissing = 0;
      break;
    default:
      cout << PHWHERE << " ReturnCode: " << iret
           << " Error in db access, no data?" << endl;
      return 0;
    }
  if (foundmissing)
    {
      float histomin = 0.00005;

      // find the minimum non zero value of this vector
      vector<float>::iterator minelement = std::min_element(var.begin(), var.end(), FindMinNotZero());
      if (minelement != var.end())
        {
          float vecmin = (*minelement) / 2;
          if (vecmin <= 0)
            {
              TText canvmsg;
              canvmsg.SetTextFont(62);
              canvmsg.SetTextSize(0.08);
              canvmsg.SetTextColor(3);
              canvmsg.SetNDC();  // set to normalized coordinates
              canvmsg.SetTextAlign(23); // center/top alignment
              canvmsg.DrawText(0.5, 0.7, "Congratulations");
              canvmsg.DrawText(0.5, 0.5, "No Missing Packet Events");
              canvmsg.DrawText(0.5, 0.3, "for the last 24 hours");
              goto nomissing;
            }
          if (vecmin < histomin)
            {
              histomin = vecmin;
            }
        }
      minrun = std::min_element(runnumber.begin(), runnumber.end());
      int graphtimebegin = *minrun - 10;
      maxrun = std::max_element(runnumber.begin(), runnumber.end());
      int graphtimeend = *maxrun + 10;

      h1 = new TH2F("h1", "Missing Packet Event fraction last 24 hours", 2, graphtimebegin, graphtimeend, 2, histomin, 1.1);
      h1->SetMinimum(histomin);
      h1->SetMaximum(1.1);
      h1->SetStats(kFALSE);
      h1->GetXaxis()->SetNoExponent();
      h1->GetXaxis()->SetTitle("Run");
      h1->GetXaxis()->SetLabelSize(0.03);
      h1->DrawCopy();
      delete h1;
      // timestamps come sorted in ascending order
      x = new float[var.size()];
      y = new float[var.size()];
      ex = new float[var.size()];
      ey = new float[var.size()];
      n = var.size();
      static const int ioff = NRTYPES;
      for (int j = 0; j < NRTYPES; j++)
        {
          if (gr[j + ioff])
            {
              delete gr[j + ioff];
              gr[j + ioff] = 0;
            }
          for (unsigned int i = 0; i < var.size(); i++)
            {
              x[i] = runnumber[i];
              if (GetRunType(runnumber[i]) == rtyp[j])
                {
                  y[i] = var[i];
                }
              else
                {
                  y[i] = 0;
                }

              ex[i] = 0;
              ey[i] = varerr[i];
            }
          gr[j + ioff] = new TGraphErrors(n, x, y, ex, ey);
          gr[j + ioff]->SetMarkerColor(mcol[j]);
          gr[j + ioff]->SetLineColor(mcol[j]);
          gr[j + ioff]->SetMarkerStyle(21);
          gr[j + ioff]->Draw("LP");
          runtype.SetTextColor(mcol[j]);
          runtype.DrawText(0.7, ypos[j], rtyp[j].c_str());
        }
      delete [] x;
      delete [] y;
      delete [] ex;
      delete [] ey;
    }
 nomissing:
  varname = "daqframstat";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  Pad3[2]->cd();
  Pad3[2]->SetLogy();
  int foundbadfram = 1;
  switch (iret)
    {
    case DBOKAY:
      foundbadfram = 1;
      break;
    case DBNOENTRIES:
      canvmsg.SetTextFont(62);
      canvmsg.SetTextSize(0.08);
      canvmsg.SetTextColor(3);
      canvmsg.SetNDC();  // set to normalized coordinates
      canvmsg.SetTextAlign(23); // center/top alignment
      canvmsg.DrawText(0.5, 0.7, "Congratulations");
      canvmsg.DrawText(0.5, 0.5, "No Events with bad frame status");
      canvmsg.DrawText(0.5, 0.3, "for the last 24 hours");
      foundbadfram = 0;
      break;
    default:
      cout << PHWHERE << " ReturnCode: " << iret
           << " Error in db access, no data?" << endl;
      return 0;
    }
  if (foundbadfram)
    {
      float histomin = 0.00005;

      // find the minimum non zero value of this vector
      vector<float>::iterator minelement = std::min_element(var.begin(), var.end(), FindMinNotZero());
      if (minelement != var.end())
        {
          float vecmin = (*minelement) / 2;
          if (vecmin <= 0)
            {
              TText canvmsg;
              canvmsg.SetTextFont(62);
              canvmsg.SetTextSize(0.08);
              canvmsg.SetTextColor(3);
              canvmsg.SetNDC();  // set to normalized coordinates
              canvmsg.SetTextAlign(23); // center/top alignment
              canvmsg.DrawText(0.5, 0.7, "Congratulations");
              canvmsg.DrawText(0.5, 0.5, "No Events with bad frame status");
              canvmsg.DrawText(0.5, 0.3, "for the last 24 hours");
              goto nobadframe;
            }
          if (vecmin < histomin)
            {
              histomin = vecmin;
            }
        }
      minrun = std::min_element(runnumber.begin(), runnumber.end());
      int graphtimebegin = *minrun - 10;
      maxrun = std::max_element(runnumber.begin(), runnumber.end());
      int graphtimeend = *maxrun + 10;

      h1 = new TH2F("h1", "Bad Frame Status Event fraction last 24 hours", 2, graphtimebegin, graphtimeend, 2, histomin, 1.1);
      h1->SetMinimum(histomin);
      h1->SetMaximum(1.1);
      h1->SetStats(kFALSE);
      h1->GetXaxis()->SetNoExponent();
      h1->GetXaxis()->SetTitle("Run");
      h1->GetXaxis()->SetLabelSize(0.03);
      h1->DrawCopy();
      delete h1;
      // timestamps come sorted in ascending order
      x = new float[var.size()];
      y = new float[var.size()];
      ex = new float[var.size()];
      ey = new float[var.size()];
      n = var.size();
      static const int ioff = 2 * NRTYPES;
      for (int j = 0; j < NRTYPES; j++)
        {
          if (gr[j + ioff])
            {
              delete gr[j + ioff];
              gr[j + ioff] = 0;
            }
          for (unsigned int i = 0; i < var.size(); i++)
            {
              x[i] = runnumber[i];
              if (GetRunType(runnumber[i]) == rtyp[j])
                {
                  y[i] = var[i];
                }
              else
                {
                  y[i] = 0;
                }

              ex[i] = 0;
              ey[i] = varerr[i];
            }
          gr[j + ioff] = new TGraphErrors(n, x, y, ex, ey);
          gr[j + ioff]->SetMarkerColor(mcol[j]);
          gr[j + ioff]->SetLineColor(mcol[j]);
          gr[j + ioff]->SetMarkerStyle(21);
          gr[j + ioff]->Draw("LP");
          runtype.SetTextColor(mcol[j]);
          runtype.DrawText(0.7, ypos[j], rtyp[j].c_str());
        }
      delete [] x;
      delete [] y;
      delete [] ex;
      delete [] ey;
    }
 nobadframe:
  TC3->Update();
  return 0;
}

int DaqMonDraw::DrawGranule(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();

  int iret = -1;
  vector<GranMonDraw *>::const_iterator miter;
  for (miter = Granule.begin(); miter != Granule.end(); ++miter)
    {
      if (!strcmp(what, (*miter)->Name()))
        {
          iret = (*miter)->Draw(what);
        }
    }
  if (! gROOT->FindObject("DaqMon2"))
    {
      MakeCanvas("DaqMon2");
    }

  TC2->cd();
  TC2->Clear("D");
  if (iret)
    {
      DrawDeadServer(transparent2);
      TC2->Update();
      return iret;
    }

  const int NHIST = 12;
  const char *hname[NHIST] =
    { "PacketSum" ,
      "Bytes",
      "FemGl1Sync" ,
      "FemClk" ,
      "FemParity" ,
      "DcmFemParity",
      "FemEvtNo" ,
      "DcmChkSum" ,
      "BadLength",
      "GlinkError",
      "SubsysError",
      "Missing"
    };

  const char *desc[NHIST] =
    { "PacketSum" ,
      "Bytes/packet",
      "Gl1SyncErr" ,
      "FemClkErr" ,
      "ParityErr" ,
      "DCMParErr" ,
      "EvtNoErr" ,
      "DcmChkSum" ,
      "LengthErr",
      "GlinkErr",
      "SubsysErr",
      "Missing"
    };
  //  Pad2[0]->SetEditable(kTRUE);
  string HFirstName = what;
  string HName;
  int igood = 0;
  TText description;
  description.SetTextFont(62);
  description.SetTextSize(0.13);
  description.SetNDC();  // set to normalized coordinates
  description.SetTextAlign(23); // center/top alignment
  description.SetTextAngle(270);
  for (short int ihist = 0; ihist < NHIST; ihist++)
    {
      HName = "Daq_" + HFirstName + hname[ihist];
      Pad2[ihist]->cd();
      map<const string, TH1 *>::const_iterator hiter = HistoMap.find(HName);
      if (hiter != HistoMap.end())
        {
          if (hiter->second->Integral() < 0)
            {
              description.SetTextAngle(0);
              ostringstream outtxt;
              outtxt << desc[ihist] << " Not Implemented";
              description.DrawText(0.5, 0.5, outtxt.str().c_str());
              description.SetTextAngle(270);
            }
          else if (hiter->second->Integral() == 0)
            {
              Pad2[ihist]->SetLogy(0);
              hiter->second->SetMinimum(0);
              if (ihist > 1)
                {
                  hiter->second->GetYaxis()->SetTitleOffset(0.45);
                  hiter->second->GetYaxis()->SetTitleSize(0.1);
                  hiter->second->SetMaximum(105.);
                  hiter->second->GetYaxis()->SetTitle("Error Rate (%)");
                }
              hiter->second->Draw();
              description.DrawText(0.97, 0.5, desc[ihist]);
            }
          else
            {
              // here come the errors
              if (ihist > 1)
                {
                  Pad2[ihist]->SetLogy(0);
                  hiter->second->SetMaximum(105.);
		  double histominimum = 0;
                  for (int i = 1; i <= hiter->second->GetNbinsX(); i++)
                    {
                      if (hiter->second->GetBinContent(i) <= 0)
                        {
                          hiter->second->SetBinContent(i, 1.e-21);
                        }
                      else if (hiter->second->GetBinContent(i) > 1.e-21
                               && hiter->second->GetBinContent(i) < 20)
                        {
                          Pad2[ihist]->SetLogy(1);
                          hiter->second->SetMaximum(150.);
			  if (histominimum == 0)
			    {
			      histominimum = hiter->second->GetBinContent(i);
			    }
			  else
			    {
			      if (histominimum > hiter->second->GetBinContent(i))
				{
				  histominimum = hiter->second->GetBinContent(i);
				}
			    }
                          hiter->second->SetMinimum(0.1);
                        }
                    }
                  hiter->second->SetMinimum(histominimum/10.);
                  hiter->second->GetYaxis()->SetNoExponent();
                  hiter->second->GetYaxis()->SetTitleOffset(0.45);
                  hiter->second->GetYaxis()->SetTitleSize(0.1);
                  hiter->second->GetYaxis()->SetTitle("Error Rate (%)");
                }
              else
                {
                  hiter->second->SetMinimum(0);
                }

              hiter->second->Draw();
              description.DrawText(0.97, 0.5, desc[ihist]);
            }
          igood ++;
        }
      else
        {
          cout << "Histogram " << HName << " not found" << endl;
        }
      Pad2[ihist]->Update();
    }
  transparent2->cd();
  transparent2->SetEditable(kTRUE);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.07);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream, datestream;
  //string runstring;
  if (igood) // subsystem histograms exist
    {
      runnostream << HFirstName << " Run " << cl->RunNumber();
    }
  else // possible typo in granule name
    {
      runnostream << "Can't find Granule " << HFirstName << ", Typo?";
    }
  //  runstring = runnostream.str();
  transparent2->Clear();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC2->Update();

  return 0;
}

int
DaqMonDraw::DrawAllGranules(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *goodchan, *badchan;
  int iret = -1;
  vector<GranMonDraw *>::const_iterator miter;
  for (miter = Granule.begin(); miter != Granule.end(); ++miter)
    {
      iret = (*miter)->Draw(what);
      if (iret < 0)
        {
          break;
        }
    }
  map<const string, TH1 *>::const_iterator hiter;
  if (! gROOT->FindObject("DaqMon1"))
    {
      MakeCanvas("DaqMon1");
    }
  TC1->cd();
  TC1->Clear("D");
  if (iret)
    {
      if (iret == -1)
        {
          DrawDeadServer(transparent1);
        }
      else
        {
          DrawNoEvents(transparent1);
        }
      TC1->Update();
      return iret;
    }
  int ipad = 0;
  int iFatal = 0;
  int badguy = 0;
  const int BINMAX = 12;
  goodchan = new TH1F("tmp1", "", BINMAX, 0, BINMAX);
  goodchan->SetFillColor(3);
  goodchan->SetStats(0);
  goodchan->SetLabelSize(0.09, "Y");
  goodchan->SetLabelSize(0.0, "X");
  badchan = new TH1F("tmp2", "", BINMAX, 0, BINMAX);
  badchan->SetFillColor(2);
  badchan->SetStats(0);
  badchan->SetLabelSize(0.09, "Y");
  badchan->SetLabelSize(0.0, "X"); // no x axis label
  for (hiter = HistoMap.begin(); hiter != HistoMap.end(); ++hiter)
    {
      if (ipad >= PLOTCOLUMNS*PLOTROWS - 1) // we might need one pad to draw the bunchcrossing histo
        {
          cout << "too many subsystems for current no of rows/columns" << endl;
          cout << "Update PLOTCOLUMNS and PLOTROWS in DaqMonDraw.h and recompile" << endl;
          exit(1);
        }
      string HName = hiter->second->GetName();
      if (HName.find("GranStatus") == string::npos) // string::npos is return value for substring not found
        {
          continue;
        }
      Pad1[ipad]->cd();
      Pad1[ipad]->SetEditable(kTRUE);
      Pad1[ipad]->SetLogy();
      goodchan->SetBinContent(1, hiter->second->GetBinContent(1));
      int maxbad = 0;
      int integral_bad = 0;
      if (hiter->second->Integral() <= 0)
        {
          Pad1[ipad]->SetLogy(0); // set to linear scale
        }
      for (unsigned short int j = 2; j <= BINMAX; j++)
        {
          if (hiter->second->GetBinContent(j) > 0)
            {
              badchan->SetBinContent(j, hiter->second->GetBinContent(j));
              integral_bad +=(int) hiter->second->GetBinContent(j);
              if (maxbad < hiter->second->GetBinContent(j))
                {
                  maxbad = (int) hiter->second->GetBinContent(j);
                }
            }
          else
            {
              badchan->SetBinContent(j, 1.e-3);
            }
        }
      TText thissys;
      thissys.SetTextAlign(23); // left top alignment
      thissys.SetTextFont(62);
      thissys.SetTextSize(0.1);
      thissys.SetNDC();  // set to normalized coordinates
      TText thiserror;
      thiserror.SetTextFont(62);
      thiserror.SetTextSize(0.08);
      thiserror.SetTextAngle(90.);
      thiserror.SetTextAlign(12);
      thiserror.SetNDC();
      if (goodchan->GetMaximum() >= maxbad)
        {
          // if goodchan contains entries, set the minimum to 0.1 so we can see single counts
          if (goodchan->Integral() > 0)
            {
              goodchan->SetMinimum(0.1);
            }
          else
            {
              goodchan->SetMinimum( -1111);
            }
          goodchan->DrawCopy();
          badchan->DrawCopy("same");
        }
      else
        {
          if (integral_bad > 0)
            {
              badchan->SetMinimum(0.1);
            }
          else
            {
              badchan->SetMinimum( -1111);
            }
          badchan->DrawCopy();
          if (goodchan->Integral() > 0) // if log scale and no good entries -> error msg for empty histo
            {
              goodchan->DrawCopy("same");
            }
        }
      float histowidth = 1 - Pad1[ipad]->GetLeftMargin() - Pad1[ipad]->GetRightMargin(); // get size of histo in pad coordinates
      float xincrement = histowidth / BINMAX;
      float xcoordinate = Pad1[ipad]->GetLeftMargin() + xincrement / 2.;
      thissys.DrawText(0.5, 0.99, hiter->second->GetTitle());
      thiserror.DrawText(xcoordinate, 0.15, "Good");
      xcoordinate += xincrement;
      thiserror.DrawText(xcoordinate, 0.15, "Bad");
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(3) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "GL1Clk");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(4) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "FEMClk");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(5) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "Parity");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(6) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "EvtNo");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(7) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "DcmChkSum");
        }
      xcoordinate += xincrement;
      thiserror.DrawText(xcoordinate, 0.15, "Length");
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(9) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "DCMFemPar");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(10) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "Glink");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(11) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "Subsys");
        }
      xcoordinate += xincrement;
      if (hiter->second->GetBinContent(12) < 0)
        {
          thiserror.DrawText(xcoordinate, 0.15, "N/A");
        }
      else
        {
          thiserror.DrawText(xcoordinate, 0.15, "Missing");
        }
      if (hiter->second->GetBinContent(13) > 0)
        {
          TText FatalMsg;
          FatalMsg.SetTextAlign(23); // left top alignment
          FatalMsg.SetTextFont(62);
          FatalMsg.SetTextSize(0.15);
          FatalMsg.SetNDC();  // set to normalized coordinates
          ostringstream fatalmsg;
          if (iFatal < hiter->second->GetBinContent(13))
            {
              iFatal = (int) hiter->second->GetBinContent(13);
            }
          fatalmsg << hiter->second->GetBinContent(13) << " times";
          badguy = (int) hiter->second->GetBinContent(14);
          string badmsg;
          switch (badguy)
            {
            case 1:
              badmsg = "out of GL1 sync";
              drawstop = 1;
              break;
            case 2:
              badmsg = "Fatal Error";
              drawstop = 1;
              break;
            case 3:
              badmsg = "Pkt Cont drop";
              drawstop = 1;
              break;
            case 4:
              badmsg = "TrigBits Error";
              drawstop = 1;
              break;
            case 5:
              badmsg = "Bad Frame Status";
              break;
            default:
              badmsg = "BUG: Unknown Error";
              break;
            }
          FatalMsg.SetTextColor(kAzure - 2);
          FatalMsg.DrawText(0.5, 0.85, badmsg.c_str());
          FatalMsg.DrawText(0.5, 0.65, fatalmsg.str().c_str());
        }
      Pad1[ipad]->Update();
      Pad1[ipad]->SetEditable(kFALSE);
      ipad++;
    }
  TH1 *DaqMonStatus = cl->getHisto("Daq_MonStatus");
  int badabortgap = static_cast<int>(DaqMonStatus->GetBinContent(DAQBADABORTGAP));
  if (badabortgap)
    {
      Pad1[ipad]->cd();
      Pad1[ipad]->SetEditable(kTRUE);

      TH1 *daq_bcross = cl->getHisto("Daq_bcross");
      daq_bcross->SetStats(kFALSE);
      daq_bcross->SetTitle("");
      daq_bcross->SetLabelSize(0.09, "Y");
      daq_bcross->SetLabelSize(0.09, "X");
      daq_bcross->DrawCopy();
      TH1 *abort_cross = static_cast<TH1 *> (daq_bcross->Clone());
      for (int i = 1; i < 107; i++)
        {
          abort_cross->SetBinContent(i, -1111);
        }
      for (int i = 116; i <= 120; i++)
        {
          abort_cross->SetBinContent(i, -1111);
        }
      abort_cross->SetFillColor(2);
      abort_cross->DrawCopy("SAME");
      TText thissys;
      thissys.SetTextAlign(23); // left top alignment
      thissys.SetTextFont(62);
      thissys.SetTextSize(0.1);
      thissys.SetNDC();  // set to normalized coordinates
      thissys.DrawText(0.5, 0.99, "Bunch Xing");
      TText thiserror;
      thiserror.SetTextFont(62);
      thiserror.SetTextSize(0.08);
      thiserror.SetTextAngle(90.);
      thiserror.SetTextAlign(12);
      thiserror.SetTextColor(kBlue);
      thiserror.DrawText(111, 1, "Abort Gap");
      delete abort_cross;
    }
  transparent1->cd();
  transparent1->SetEditable(kTRUE);
  transparent1->Clear();
  int currentevents = static_cast<int>(DaqMonStatus->GetBinContent(DAQTOTALEVENTBIN));
  int badframestatusevents = static_cast<int>(DaqMonStatus->GetBinContent(DAQBADFRAMESTATUSBIN));
  int nomultieventbuffering = static_cast<int>(DaqMonStatus->GetBinContent(DAQNOMULTIBUFFER));
  int badframestop = 0;
  float badframefract = badframestatusevents / (float)currentevents;
  //  badframefract = 0.;
  // check for empty granules message only if we have the first update (otherwise granule plots are empty)
  // fraction should be smaller 10% (hickups seem to happen at startup, but 10 out of 100 events should
  // trigger this)
  if (badframefract > 0.2 && currentevents > FIRSTUPDATE)
    {
      TText FatalMsg;
      ostringstream fatalstream;
      FatalMsg.SetTextSize(0.06);
      TText addmsg;
      addmsg.SetTextFont(62);
      addmsg.SetTextSize(0.03);
      addmsg.SetTextColor(6);
      addmsg.SetNDC();  // set to normalized coordinates
      addmsg.SetTextAlign(13); // center/top alignment
      fatalstream << "Bad Frame Status fraction " << setprecision(3) << badframefract;
      addmsg.DrawText(0.05, 0.93, fatalstream.str().c_str());
      fatalstream.str("");
      badframestop = 0;
    }
  if (! abortgapmessage)
    {
      badabortgap = 0;
    }
  if ((iFatal > 5 && drawstop) || badframestop || badabortgap || nomultieventbuffering) // 5 strikes and you are out
    {
      TText FatalMsg;
      FatalMsg.SetTextFont(62);
      FatalMsg.SetTextSize(0.1);
      FatalMsg.SetTextColor(6);
      FatalMsg.SetNDC();  // set to normalized coordinates
      FatalMsg.SetTextAlign(23); // center/top alignment
      ostringstream fatalstream;
      if (badframestop)
        {
          FatalMsg.SetTextSize(0.06);
          TText addmsg;
          addmsg.SetTextFont(62);
          addmsg.SetTextSize(0.04);
          addmsg.SetTextColor(6);
          addmsg.SetNDC();  // set to normalized coordinates
          addmsg.SetTextAlign(23); // center/top alignment
          fatalstream << "Bad Frame Status fraction " << setprecision(3) << badframefract << ": if no msg, check for empty granule";
          addmsg.DrawText(0.5, 0.93, fatalstream.str().c_str());
          fatalstream.str("");
        }
      if (badabortgap)
        {
          FatalMsg.SetTextSize(0.06);
          TText addmsg;
          addmsg.SetTextFont(62);
          addmsg.SetTextSize(0.04);
          addmsg.SetTextColor(6);
          addmsg.SetNDC();  // set to normalized coordinates
          addmsg.SetTextAlign(23); // center/top alignment
          fatalstream << "Abort Gap shifted: Reload the GL1 configuration before starting the next run";
          addmsg.DrawText(0.5, 0.93, fatalstream.str().c_str());
          fatalstream.str("");
          fatalstream << "Run " << cl->RunNumber();
        }
      if (nomultieventbuffering)
        {
          FatalMsg.SetTextSize(0.06);
          TText addmsg;
          addmsg.SetTextFont(62);
          addmsg.SetTextSize(0.04);
          addmsg.SetTextColor(6);
          addmsg.SetNDC();  // set to normalized coordinates
          addmsg.SetTextAlign(23); // center/top alignment
          fatalstream << "Multi Event Buffering Disabled???, Check Lvl1 Count in RunCtrl";
          addmsg.DrawText(0.5, 0.93, fatalstream.str().c_str());
          fatalstream.str("");
          fatalstream << "Run " << cl->RunNumber();
        }
      else
        {
          fatalstream << "Run " << cl->RunNumber() << ": STOP THE DAQ";
        }
      FatalMsg.DrawText(0.5, 0.98, fatalstream.str().c_str());
    }
  else // everything is okay
    {
      TText PrintRun;
      PrintRun.SetTextFont(62);
      PrintRun.SetTextSize(0.08);
      PrintRun.SetNDC();  // set to normalized coordinates
      PrintRun.SetTextAlign(23); // center/top alignment
      ostringstream runnostream, datestream;
      runnostream << "Run " << cl->RunNumber();
      PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
      time_t evttime = cl->EventTime("CURRENT");
      datestream << "Date: " << ctime(&evttime);
      PrintRun.SetTextSize(0.035);
      PrintRun.SetTextColor(4);
      PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());
    }
  transparent1->SetEditable(kFALSE);
  TC1->Update();
  delete goodchan;
  delete badchan;
  return 0;
}

int
DaqMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  string savefile;
  if (Draw(what))
    {
      return -1;
    }
  filename << ThisName << "_" << cl->RunNumber() << ".ps";
  savefile = filename.str();
  TC1->Print(savefile.c_str());
  return 0;
}

int
DaqMonDraw::MakeHtml(const char *what)
{
  // if Draw fails we don't want html output since
  // most probably this was called from a file which
  // didn't contain daq monitor histograms
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();
  htmloutflag = 1; // set this so identify does coloring of printout

  string imgpath;
  transparent1->SetEditable(kTRUE);
  transparent1->cd();
  TText notPoms;
  notPoms.SetTextFont(62);
  notPoms.SetTextSize(0.023);
  notPoms.SetTextColor(6);
  notPoms.SetNDC();
  notPoms.SetTextAlign(12);
  notPoms.DrawText(0.69, 0.97, "The awful Truth here, unlike POMS");
  notPoms.DrawText(0.69, 0.93, "known bad packets are not suppressed");

  imgpath = cl->htmlRegisterPage(*this, "Monitor", "", "png");
  cl->CanvasToPng(TC1, imgpath);
  iret = Draw("GLOBAL");
  if (iret)
    {
      return iret;
    }

  imgpath = cl->htmlRegisterPage(*this, "Fraction Corrupt", "corrupt", "png");
  cl->CanvasToPng(TC3, imgpath);

  iret = Draw("LOWRUN");
  if (iret)
    {
      return iret;
    }

  imgpath = cl->htmlRegisterPage(*this, "Low Runnumber", "lowrun", "png");
  cl->CanvasToPng(TC4, imgpath);

  vector<GranMonDraw *>::const_iterator miter;
  for (miter = Granule.begin(); miter != Granule.end(); ++miter)
    {
      string name = (*miter)->Name();
      int iret = DrawGranule(name.c_str());
      if (iret)
        {
          continue;
        }
      ostringstream menuname;
      menuname << "Status/" << name;
      imgpath = cl->htmlRegisterPage(*this, menuname.str(), name, "png");
      cl->CanvasToPng(TC2, imgpath);
    }
  BOOST_FOREACH(GranMonDraw * grandraw, Granule)
    {
      grandraw->MakeHtml("SUBSYS");
    }
  string htmlpath = cl->htmlRegisterPage(*this, "PacketSummary",
                                         "PacketSummary", "html");

  ofstream outfile(htmlpath.c_str());
  outfile << "<HTML>" << endl;
  outfile << "<HEAD>" << endl;
  outfile << "<TITLE>Daq Monitor Packet Summary</TITLE>" << endl;
  outfile << "</HEAD>" << endl;
  htmloutflag = 1;
  identify(outfile);
  htmloutflag = 0;

  cl->SaveLogFile(*this);

  return 0;
}

void
DaqMonDraw::registerHisto(TH1 *h1d, const int replace)
{
  map<const string, TH1 *>::const_iterator histoiter = HistoMap.find(h1d->GetName());
  if (histoiter != HistoMap.end() && replace == 0)
    {
      cout << "Histogram " << h1d->GetName() << " already registered, I won't overwrite it" << endl;
      cout << "Use a different name and try again" << endl;
    }
  else
    {
      HistoMap[h1d->GetName()] = h1d;
    }
  return ;
}

void
DaqMonDraw::registerHisto(const char *hname, TH1 *h1d, const int replace)
{
  map<const string, TH1 *>::const_iterator histoiter = HistoMap.find(hname);
  if (histoiter != HistoMap.end() && replace == 0)
    {
      cout << "Histogram " << hname << " already registered, I won't overwrite it" << endl;
      cout << "Use a different name and try again" << endl;
    }
  else
    {
      HistoMap[hname] = h1d;
    }
  return ;
}

TH1 *
DaqMonDraw::getHisto(const char *hname) const
{
  map<const string, TH1 *>::const_iterator histoiter = HistoMap.find(hname);
  if (histoiter != HistoMap.end())
    {
      return histoiter->second;
    }
  return NULL;
}

void
DaqMonDraw::Print(const char *what) const
{
  if (!strcmp(what, "ALL") || !strcmp(what, "HISTOS"))
    {
      // loop over the map and print out the content (name and location in memory)
      cout << "--------------------------------------" << endl << endl;
      cout << "List of Histograms in DaqMonDraw:" << endl;

      map<const string, TH1 *>::const_iterator hiter;
      for (hiter = HistoMap.begin(); hiter != HistoMap.end(); ++hiter)
        {
          cout << hiter->first << " is " << hiter->second << endl;
        }
      cout << endl;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "MONITOR"))
    {
      // loop over the map and print out the content (name and location in memory)
      cout << "--------------------------------------" << endl << endl;
      cout << "List of Drawers in DaqMonDraw:" << endl;

      vector<GranMonDraw *>::const_iterator miter;
      for (miter = Granule.begin(); miter != Granule.end(); ++miter)
        {
          cout << (*miter)->Name() << endl;
        }
      cout << endl;
    }
  return ;
}

void
DaqMonDraw::registerGranule(GranMonDraw *granule)
{
  BOOST_FOREACH(GranMonDraw * grandraw, Granule)
    {
      if (!strcmp(grandraw->Name(), granule->Name()))
        {
	  cout << "Monitor " << granule->Name() << " already registered, I won't overwrite it" << endl;
	  cout << "Use a different name and try again" << endl;
          return ;
        }
    }
  if (granule->Init() == 0)
    {
      Granule.push_back(granule);
      granmap[granule->Name()] = granule;
    }
  return ;
}

void
DaqMonDraw::identify(ostream& out) const
{
  out << "DaqMonDraw Object" << endl;
  out << "Currently registered Granule Drawers:" << endl;

  BOOST_FOREACH(GranMonDraw * grandraw, Granule)
    {
      if (htmloutflag)
        {
          grandraw->HtmlOut(htmloutflag);
        }
      grandraw->identify(out);
    }
  return ;
}


int
DaqMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->SetEditable(kTRUE);
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "DAQMONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.2, "DEAD");
  transparent->Update();
  return 0;
}

int
DaqMonDraw::DrawNoEvents(TPad *transparent)
{
  transparent->SetEditable(kTRUE);
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(3);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "DaqMon got");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "no event");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "(yet)");
  transparent->Update();
  return 0;
}

int
DaqMonDraw::CanvasSize(const int i, const int xsize, const int ysize)
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (i < 0)
    {
      canvassize[0][0] = cl->GetDisplaySizeX();
      canvassize[0][1] = cl->GetDisplaySizeY();
      return 0;
    }
  else if (i > 0)
    {
      cout << "Invalid Canvas - we start from 0" << endl;
      return -1;
    }
  if (xsize <= 0 || ysize <= 0 || xsize > 2000 || ysize > 2000)
    {
      cout << "Invalid Canvas size: X: " << xsize << ", Y: " << ysize << endl;
      return -1;
    }
  canvassize[i][0] = xsize;
  canvassize[i][1] = ysize;
  return 0;
}

int
DaqMonDraw::DrawLowRun(const char *what)
{
  if (! gROOT->FindObject("DaqMon4"))
    {
      MakeCanvas("DaqMon4");
    }
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *DaqMonStatus = cl->getHisto("Daq_MonStatus");
  TH1 *FrameWorkVars = cl->getHisto("FrameWorkVars");
  TH1 *TimeToLastEvent[4];
  TimeToLastEvent[0] = cl->getHisto("Daq_TimeToLastEvent0");
  TimeToLastEvent[1] = cl->getHisto("Daq_TimeToLastEvent1");
  TimeToLastEvent[2] = cl->getHisto("Daq_TimeToLastEvent2");
  TimeToLastEvent[3] = cl->getHisto("Daq_TimeToLastEvent3");
  TH1 *FrameStatus = cl->getHisto("Daq_FrameStatus");
  TC4->cd();
  TC4->Clear("D");
  if (!DaqMonStatus)
    {
      DrawDeadServer(transparent4);
      TC4->Update();
      return -1;
    }

  TText mbuftxt;
  mbuftxt.SetTextFont(62);
  mbuftxt.SetTextSize(0.06);
  mbuftxt.SetTextColor(2);
  mbuftxt.SetTextAlign(13);
  TLine tl;
  tl.SetLineWidth(2);
  tl.SetLineColor(2);
  Pad4[0]->cd();
  Pad4[0]->SetLogx();
  double maxhis = TimeToLastEvent[1]->GetMaximum();
  TimeToLastEvent[1]->SetStats(0);
  TimeToLastEvent[1]->SetTitle("");
  TimeToLastEvent[1]->SetXTitle("Time to Last Event (ticks)");
  TimeToLastEvent[1]->GetXaxis()->SetLabelSize(0.05);
  TimeToLastEvent[1]->GetXaxis()->SetTitleOffset(0.9);
  TimeToLastEvent[1]->GetXaxis()->SetTitleSize(0.06);
  int xbinnum = TimeToLastEvent[1]->GetXaxis()->FindFixBin(multibufferedTicks);
  int fbin = TimeToLastEvent[1]->FindFirstBinAbove(0);
  TimeToLastEvent[1]->DrawCopy();
  mbuftxt.DrawText(2, maxhis / 2, "Forbidden");
  tl.DrawLine(15.5, 0, 15.5, maxhis);
  tl.SetLineColor(3);
  tl.DrawLine(multibufferedTicks, 0, multibufferedTicks, maxhis);
  mbuftxt.SetTextAlign(23);
  mbuftxt.SetTextColor(3);
  mbuftxt.DrawText(200, maxhis, "Multi Event Buffered");
  if (xbinnum < fbin) // no counts in multi event buffered range
    {
      if (TimeToLastEvent[1]->Integral() > 1000)
	{
	  mbuftxt.SetTextColor(2);
	  mbuftxt.SetTextSize(0.065);
	  mbuftxt.DrawText(140, maxhis / 2, "No Events detected here");
	  mbuftxt.DrawText(140, maxhis / 3, "Multi Event Buffering disabled?");
	  mbuftxt.DrawText(140, maxhis / 4, "runctrl Level 1 Count should be 4");
      }
  }

  Pad4[1]->cd();
  maxhis = TimeToLastEvent[0]->GetMaximum();
  TimeToLastEvent[0]->SetStats(0);
  TimeToLastEvent[0]->SetTitle("");
  TimeToLastEvent[0]->SetXTitle("Time to Last Event (ticks)");
  TimeToLastEvent[0]->GetXaxis()->SetLabelSize(0.05);
  TimeToLastEvent[0]->GetXaxis()->SetTitleOffset(0.9);
  TimeToLastEvent[0]->GetXaxis()->SetTitleSize(0.06);
  TimeToLastEvent[0]->DrawCopy();
  tl.SetLineColor(2);
  tl.DrawLine(15.5, 0, 15.5, maxhis);
  mbuftxt.SetTextSize(0.05);
  mbuftxt.SetTextAlign(23);
  mbuftxt.SetTextColor(2);
  mbuftxt.DrawText(8, maxhis / 2, "Forbidden");

  Pad4[2]->cd();
  Pad4[2]->SetLogx();
  maxhis = TimeToLastEvent[2]->GetMaximum();
  TimeToLastEvent[2]->SetStats(0);
  TimeToLastEvent[2]->SetTitle("");
  TimeToLastEvent[2]->SetXTitle("Time to 2nd Last Event (ticks)");
  TimeToLastEvent[2]->GetXaxis()->SetLabelSize(0.05);
  TimeToLastEvent[2]->GetXaxis()->SetTitleOffset(0.9);
  TimeToLastEvent[2]->GetXaxis()->SetTitleSize(0.06);
  TimeToLastEvent[2]->DrawCopy();
  mbuftxt.SetTextSize(0.06);
  mbuftxt.SetTextColor(2);
  mbuftxt.DrawText(5, maxhis / 2, "Forbidden");
  tl.SetLineColor(2);
  tl.DrawLine(31.5, 0, 31.5, maxhis);
  tl.SetLineColor(3);
  tl.DrawLine(2*multibufferedTicks, 0, 2*multibufferedTicks, maxhis);
  mbuftxt.SetTextAlign(23);
  mbuftxt.SetTextColor(3);
  mbuftxt.DrawText(250, maxhis, "Multi Event Buffered");

  Pad4[3]->cd();
  Pad4[3]->SetLogx();
  maxhis = TimeToLastEvent[3]->GetMaximum();
  TimeToLastEvent[3]->SetStats(0);
  TimeToLastEvent[3]->SetTitle("");
  TimeToLastEvent[3]->SetXTitle("Time to 3rd Last Event (ticks)");
  TimeToLastEvent[3]->GetXaxis()->SetLabelSize(0.05);
  TimeToLastEvent[3]->GetXaxis()->SetTitleOffset(0.8);
  TimeToLastEvent[3]->GetXaxis()->SetTitleSize(0.06);
  TimeToLastEvent[3]->DrawCopy();
  mbuftxt.SetTextColor(2);
  mbuftxt.DrawText(8, maxhis / 2, "Forbidden");
  tl.SetLineColor(2);
  tl.DrawLine(47.5, 0, 47.5, maxhis);
  tl.SetLineColor(3);
  tl.DrawLine(3*multibufferedTicks, 0, 3*multibufferedTicks, maxhis);
  mbuftxt.SetTextAlign(23);
  mbuftxt.SetTextColor(3);
  mbuftxt.DrawText(300, maxhis, "Multi Event Buffered");

  Pad4[4]->cd();
  map<string, double> aversize;
  int nbins = GetQuantityPerEvent(aversize, SIZEPEREVENTBIN);
  TH1 *eventsize = new TH1F("eventsize", "", nbins, -0.5, nbins - 0.5);
  eventsize->SetStats(0);
  eventsize->SetLabelOffset(100, "x");
  map<string, double>::const_iterator averiter;
  int ibin = 0;
  for (averiter = aversize.begin(); averiter != aversize.end(); ++averiter)
    {
      ibin++;
      eventsize->SetBinContent(ibin, averiter->second);
    }
  eventsize->SetLineColor(2);
  eventsize->SetLineWidth(2);
  eventsize->GetYaxis()->SetTitle("kB");
  eventsize->GetYaxis()->SetTitleSize(0.06);
  eventsize->GetYaxis()->SetTitleOffset(0.5);
  eventsize->DrawCopy();
  TText systxt;

  systxt.SetTextAngle(90);
  systxt.SetTextSize(0.04);
  systxt.SetTextAlign(12);
  ibin = 0;
  double maxY = eventsize->GetMaximum();
  for (averiter = aversize.begin(); averiter != aversize.end(); ++averiter)
    {
      systxt.DrawText(ibin, maxY / 20., (averiter->first).c_str());
      ibin++;
    }
  systxt.SetNDC();
  systxt.SetTextSize(0.05);
  systxt.SetTextAngle(0);
  systxt.SetTextAlign(22);
  systxt.DrawText(0.5, 0.85, "Average Event Size in Bytes per subsystem");
  Pad4[4]->Update();

  Pad4[5]->cd();
  if (FrameStatus->Integral() > 0)
    {
      Pad4[5]->SetLogy();
      FrameStatus->SetMinimum(0.1);
    }
  else
    {
      Pad4[5]->SetLogy(0);
      FrameStatus->SetMinimum(0);
    }
  FrameStatus->SetStats(0);
  FrameStatus->SetTitle("");
  FrameStatus->SetFillColor(3);
  FrameStatus->GetXaxis()->SetTitle("Bit");
  FrameStatus->GetXaxis()->SetLabelSize(0.05);
  FrameStatus->GetXaxis()->SetTitleSize(0.06);
  FrameStatus->GetXaxis()->SetTitleOffset(0.7);
  FrameStatus->GetYaxis()->SetLabelSize(0.05);
  FrameStatus->DrawCopy();
  FrameStatus->SetBinContent(1, 1.e-3);
  FrameStatus->SetFillColor(2);
  FrameStatus->DrawCopy("same");
  systxt.SetNDC();
  systxt.SetTextSize(0.08);
  systxt.SetTextAngle(0);
  systxt.SetTextAlign(22);
  systxt.DrawText(0.5, 0.85, "Event Frame Status Bits");
  Pad4[5]->Update();

  Pad4[6]->cd();
  int currentrunlowevents = static_cast<int>(FrameWorkVars->GetBinContent(LOWRUNEVENTBIN));
  int currentevents = static_cast<int>(DaqMonStatus->GetBinContent(DAQTOTALEVENTBIN));
  int corruptevents = static_cast<int>(DaqMonStatus->GetBinContent(DAQCORRUPTEVENTBIN));
  int badframestatusevents = static_cast<int>(DaqMonStatus->GetBinContent(DAQBADFRAMESTATUSBIN));
  int missingpktsevents = static_cast<int>(DaqMonStatus->GetBinContent(DAQMISSINGPACKETEVENTBIN));
  int earlyevents = static_cast<int>(FrameWorkVars->GetBinContent(EARLYEVENTNUMBIN));
  unsigned int earlyeventticks = static_cast<unsigned int>(FrameWorkVars->GetBinContent(EARLYEVENTTIMEBIN));
  unsigned int borticks = static_cast<unsigned int>(FrameWorkVars->GetBinContent(BORTIMEBIN));

  ostringstream msg;
  msg << "Sum of Low Runnumber Evts (NOT CRITICAL!): " << currentrunlowevents;
  TText Outtxt;
  Outtxt.SetTextFont(62);
  Outtxt.SetTextSize(0.08);
  Outtxt.SetNDC();
  Outtxt.SetTextAlign(13);
  Outtxt.DrawText(0.1, 0.95, msg.str().c_str());
  msg.str("");
  msg << "Number of Corrupt Events: " << corruptevents;
  if (corruptevents > 0)
    {
      Outtxt.SetTextColor(2);
    }
  else
    {
      Outtxt.SetTextColor(1);
    }

  Outtxt.DrawText(0.1, 0.85, msg.str().c_str());

  msg.str("");
  msg << "Number of Missing Packet Events: " << missingpktsevents;
  if (missingpktsevents > 0)
    {
      Outtxt.SetTextColor(2);
    }
  else
    {
      Outtxt.SetTextColor(1);
    }
  Outtxt.DrawText(0.1, 0.75, msg.str().c_str());

  msg.str("");
  msg << "Number of Events with Bad Frame Status: " << badframestatusevents;
  if (badframestatusevents > 0)
    {
      Outtxt.SetTextColor(2);
    }
  else
    {
      Outtxt.SetTextColor(1);
    }
  Outtxt.DrawText(0.1, 0.65, msg.str().c_str());

  Outtxt.SetTextColor(1);
  msg.str("");
  msg << "Number of Early Events: " << earlyevents;
  if (earlyevents > 0)
    {
      Outtxt.SetTextColor(2);
      msg << ", T-Stamp " <<  borticks - earlyeventticks << " secs before BOR";
    }
  else
    {
      Outtxt.SetTextColor(1);
    }
  Outtxt.DrawText(0.1, 0.55, msg.str().c_str());

  msg.str("");
  msg << "Number of Current Run Events: " << currentevents;
  Outtxt.SetTextColor(1);
  Outtxt.DrawText(0.1, 0.45, msg.str().c_str());

  Outtxt.SetTextAlign(23);
  Outtxt.DrawText(0.5, 0.32, "If Low Runnumber Evts keep increasing after");
  Outtxt.DrawText(0.5, 0.25, "ALL buffer boxes (EXCEPT phnxbox7)");
  Outtxt.DrawText(0.5, 0.17, "got more than 1000 events, make note in elog");


  transparent4->cd();
  transparent4->SetEditable(kTRUE);

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.06);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream, datestream;
  runnostream << "Low Runnumber Check";
  transparent4->Clear();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Run " << cl->RunNumber() << ", Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());
  TC4->Update();
  delete eventsize;
  return 0;
}

string
DaqMonDraw::GetRunType(const int runno)
{
  OnlMonClient *cl = OnlMonClient::instance();
  string runtype;
  if (!rundb)
    {
      rundb = new RunDBodbc();
    }
  static time_t purgeticks = 0;
  if (!purgeticks)
    {
      purgeticks = cl->EventTime();
    }
  if ((cl->EventTime() - purgeticks) > 100000)
    {
      runtypecache.clear();
      purgeticks = cl->EventTime();
    }
  map<int, string>::iterator iter = runtypecache.find(runno);
  if (iter != runtypecache.end())
    {
      runtype = iter->second;
    }
  else
    {
      runtype = rundb->RunType(runno);
      runtypecache[runno] = runtype;
      if (verbosity > 0)
        {
          cout << "Adding Run " << runno
               << " of type " << runtype
               << " to cache" << endl;
        }
    }
  return runtype;
}

unsigned int
DaqMonDraw::GetQuantityPerEvent(map<string, double> &quantmap, const int histobin)
{
  OnlMonClient *cl = OnlMonClient::instance();
  vector<GranMonDraw *>::const_iterator miter;
  for (miter = Granule.begin(); miter != Granule.end(); ++miter)
    {
      string sysname = (*miter)->Name();
      string histoname = "Daq_" + sysname + "AllVariables";
      TH1 *monvars = cl->getHisto(histoname.c_str());
      if (!monvars)
        {
          cout << "Histogram " << histoname << " missing" << endl;
          continue;
        }
      double sumsize = monvars->GetBinContent(SIZEPEREVENTBIN);
      double events = monvars->GetBinContent(NUMEVENTBIN);
      //       cout << "system " << sysname << " saw " << events << " tot size " << sumsize
      // 	   << ", aver: " <<  sumsize/events << endl;

      if (events > 0)
        {
          quantmap[sysname] = (sumsize / events) / 1000.; // in kB
        }
      else
        {
          quantmap[sysname] = 0;
        }

    }
  return quantmap.size();
}
