#include <McrMonDraw.h>
#include <McrMonHistoBinDefs.h>
#include <OnlMonClient.h>
#include <OnlMonDB.h>
#include <OnlMonDBReturnCodes.h>

#include <phool.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraph.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

McrMonDraw::McrMonDraw(const char *name): OnlMonDraw(name)
{
  memset(TC,0,sizeof(TC));
  memset(transparent,0,sizeof(transparent));
  memset(Pad,0,sizeof(Pad));
  memset(tgr,0,sizeof(tgr));
  // this TimeOffsetTicks is neccessary to get the time axis right
  // except for an offset between EST and CERN time zone
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars = new OnlMonDB(ThisName);
  for (int i=0; i<LAST;i++)
    {
      setrmsrange(i,20.,80.);
      setvtxrange(i,-20.,20.);
    }
  return ;
}

McrMonDraw::~McrMonDraw()
{
  delete dbvars;
  for (int i = 0;i < 4;i++)
    {
      delete tgr[i];
    }
  return ;
}

int
McrMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (!strcmp(name, "McrMon1"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[0] = new TCanvas(name, "What Mcr Sees", -1, 0, xsize / 2 , ysize);
      gSystem->ProcessEvents();
      Pad[0] = new TPad("mcrpad1", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[1] = new TPad("mcrpad2", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      // this one is used to plot the run number on the canvas
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      Pad[0]->Draw();
      Pad[1]->Draw();
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
      //      TC[0]->SetEditable(0);
    }
  else if (!strcmp(name, "McrMon2"))
    {
      // xpos negative: do not draw menu bar
      TC[1] = new TCanvas(name, "Vertex History", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      Pad[2] = new TPad("mcrpad3", "who needs this?", 0.1, 0.6875, 0.9, 0.9, 0);
      Pad[3] = new TPad("mcrpad4", "who needs this?", 0.1, 0.475, 0.9, 0.6875, 0);
      Pad[4] = new TPad("mcrpad5", "who needs this?", 0.1, 0.26, 0.9, 0.475, 0);
      Pad[5] = new TPad("mcrpad6", "who needs this?", 0.1, 0.05, 0.9, 0.26, 0);
      for (int i = 2; i < 6;i++)
        {
          Pad[i]->SetGridx();
          Pad[i]->SetGridy();
          Pad[i]->Draw();
        }
      // this one is used to plot the run number on the canvas
      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
      //TC[1]->SetEditable(0);
    }
  return 0;
}

int McrMonDraw::Draw(const char *what)
{
  int iret = 0;
  int idraw = 0;
  if (!strcmp(what, "ALL") || !strcmp(what, "FIRST"))
    {
      iret += DrawFirst(what);
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "HISTORY"))
    {
      iret += DrawHistory(what);
      idraw ++;
    }
  if (!idraw)
    {
      cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
      iret = -1;
    }
  return iret;
}

int McrMonDraw::DrawFirst(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  zvertexhist[BBC] = dynamic_cast <TH2 *> (cl->getHisto("mcr_bbcll1_zvertex"));
  zvertexhist[ZDC] = dynamic_cast <TH2 *> (cl->getHisto("mcr_zdcll1_zvertex"));
  if (! gROOT->FindObject("McrMon1"))
    {
      MakeCanvas("McrMon1");
    }
  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  const char *htitle[2] =
    {"BbcLL1 Vtx", "ZdcLL1 Vtx"
    };
  TText AnyText;
  for (int i = 0; i < LAST; i++)
    {
      Pad[i]->cd();
      if (zvertexhist[i])
        {
	  int projectthis = 1; // if empty just plot the first entry
	  TH1D *tmp = zvertexhist[i]->ProjectionY();
	  for (int j = 1; j < tmp->GetNbinsX(); j++)
	    {
	      if (tmp->GetBinContent(j) <= 0)
		{
		  projectthis = j - 1;
		  break;
		}
	    }
	  delete tmp;
	  if (projectthis <= 0)
	    {
	      ostringstream stats;
	      TH1 *mcrstat = cl->getHisto("mcr_stat");
	      AnyText.SetNDC();
	      AnyText.SetTextAlign(22);
	      stats << htitle[i] << ":Need " << mcrstat->GetBinError(i + 1) << ", got "
		    << mcrstat->GetBinContent(i + 1) << " Events";
	      AnyText.DrawText(0.5, 0.5, stats.str().c_str());
	    }
	  else
	    {
	      ostringstream atxt;
	      tmp = zvertexhist[i]->ProjectionX("mcr_tmp", projectthis, projectthis);
	      tmp->SetTitle("");
	      tmp->SetName(htitle[i]);
	      tmp->SetStats(0);
	      tmp->DrawCopy();
	      AnyText.SetNDC();
              AnyText.SetTextAlign(13);
              AnyText.DrawText(0.15, 0.85, htitle[i]);
              atxt << "Mean: " << tmp->GetMean();
              AnyText.DrawText(0.15, 0.77,atxt.str().c_str() );
	      atxt.str("");  
              atxt << "RMS: " << tmp->GetRMS();
              AnyText.DrawText(0.15, 0.69,atxt.str().c_str() );
	      delete tmp;
	    }
	}
      else
        {
          DrawDeadServer(transparent[0]);
          TC[0]->SetEditable(0);
          return -1;
        }
    }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "MCR Vtx Display Run " << cl->RunNumber()
	      << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 0.98, runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(0);
  return 0;
}

int
McrMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "MCR MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}

int
McrMonDraw::DrawDeadRhicServer(TPad *transparent, const int status)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(2);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "RESTART");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.7, "RHIC SERVER");
  ostringstream stat;
  stat << "Status " << status;
  FatalMsg.DrawText(0.5, 0.5, stat.str().c_str());
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.SetTextSize(0.05);
  FatalMsg.SetTextColor(4);
  FatalMsg.DrawText(0.5, 0.3, "and wait 5 minutes for the next");
  FatalMsg.DrawText(0.5, 0.2, "send attempt which will clear");
  FatalMsg.DrawText(0.5, 0.1, "this message if successful");
  transparent->Update();
  return 0;
}

int
McrMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // on error no ps files please
    {
      return iret;
    }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_2_" << cl->RunNumber() << ".ps";
  TC[1]->Print(filename.str().c_str());
  return 0;
}

int
McrMonDraw::MakeHtml(const char *what)
{
  setrmsrange(0,30,60);
  setvtxrange(0,-10,10);
  setrmsrange(1,30,60);
  setvtxrange(1,-10,10);
  int iret = Draw(what);
  if (iret) // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  string pngfile = cl->htmlRegisterPage(*this, "Vertex for Mcr", "1", "png");
  cl->CanvasToPng(TC[0], pngfile);

  // idem for 2nd canvas.
  pngfile = cl->htmlRegisterPage(*this, "Vertex History", "2", "png");
  cl->CanvasToPng(TC[1], pngfile);
  cl->SaveLogFile(*this);

  return 0;
}

int
McrMonDraw::DrawHistory(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TText AnyText;
  int iret = 0;
  int ipad[LAST] = {2, 4};
  const char *htitle[4] =
    {"bbcll1 vtx", "bbcll1 RMS", "zdcll1 vtx", "zdcll1 RMS"
    };
  // you need to provide the following vectors
  // which are filled from the db
  vector<float> var;
  vector<float> varerr;
  vector<time_t> timestamp;
  vector<int> runnumber;
  string varname[LAST] = {"bbcll1vtx", "zdcll1vtx"};
  // this sets the time range from which values should be returned
  time_t end = cl->EventTime();
  if (end == 0)
    {
      end = time(NULL); // current time (right NOW)
    }
  time_t begin = end - 24 * 60 * 60; // previous 24 hours
  end += 600; // add 10 minutes to search window so we do not miss the
  // current timestp

  if (! gROOT->FindObject("McrMon2"))
    {
      MakeCanvas("McrMon2");
    }
  TH1 *mcrerror = cl->getHisto("mcr_error");
  TC[1]->Clear("D");
  if (!mcrerror)
    {
      DrawDeadServer(transparent[1]);
      return -1;
    }
  int status = (int) mcrerror->GetBinContent(RHICSERVERSTATUSBIN);
  if (status)
    {
      cout << "Rhic Server problem" << endl;
      DrawDeadRhicServer(transparent[1], status);
      return 0;
    }
  for (int i = 0; i < LAST; i++)
    {
      iret = dbvars->GetVar(begin, end, varname[i], timestamp, runnumber, var, varerr);
      if (iret)
        {
          switch (iret)
            {
            case DBNOENTRIES:
              AnyText.SetNDC();
              AnyText.SetTextAlign(22);
              AnyText.DrawText(0.5, 0.5, "No DB Entries within last 24 hours");
              break;
            default:
              cout << PHWHERE << "DB Error: retcode " << iret << endl;
              break;
            }
          break;
        }
      // timestamps come sorted in ascending order
      float *x = new float[var.size()];
      float *y = new float[var.size()];
      float *ey = new float[var.size()];
      int n = 0;
      for (unsigned int j = 0; j < var.size();j++)
        {
          //       cout << "timestamp: " << ctime(&timestamp[i])
          // 	   << ", run: " << runnumber[i]
          // 	   << ", var: " << var[i]
          // 	   << ", varerr: " << varerr[i]
          // 	   << endl;
          if (var[j] > -999)
            {
              x[n] = timestamp[j] - TimeOffsetTicks;
              y[n] = var[j];
              ey[n] = varerr[j];
              n++;
            }
        }
      Pad[ipad[i]]->cd();
      if (tgr[2*i])
        {
          delete tgr[2*i];
          tgr[2*i] = 0;
        }
      if (tgr[2*i + 1])
        {
          delete tgr[2*i + 1];
          tgr[2*i + 1] = 0;
        }
      int graphtimebegin = begin - TimeOffsetTicks;
      int graphtimeend = (end - TimeOffsetTicks) + 3600; // add one hour
      TH2 *h1 = new TH2F("h1","", 2, graphtimebegin, graphtimeend, 2, vtxrange[i][0], vtxrange[i][1]);
//       h1->SetMinimum( -15);
//       h1->SetMaximum(15);
      h1->SetStats(kFALSE);
      h1->GetXaxis()->SetTimeDisplay(1);
      h1->GetYaxis()->SetLabelSize(0.06);
      h1->GetXaxis()->SetLabelSize(0.06);
      h1->GetXaxis()->SetTimeOffset(TimeOffsetTicks+6*3600); // compensate for EST/Switzerland
      h1->GetXaxis()->SetTimeFormat("%H:%M");
      h1->DrawCopy();
      delete h1;
      TLine tl;
      AnyText.SetNDC();
      AnyText.SetTextAngle(90);
      AnyText.SetTextAlign(21);
      AnyText.SetTextSize(0.075);
      AnyText.DrawText(0.05,0.5,htitle[2 * i]);
      AnyText.SetTextAngle(0);
      if (n <= 0)
	{
	  AnyText.SetNDC();
	  AnyText.SetTextAlign(22);
          AnyText.SetTextSize(0.4);
	  AnyText.DrawText(0.5, 0.5, "No Entries");
	}
      else
	{
	  tgr[2*i] = new TGraph(n, x, y);
	  tgr[2*i]->SetMarkerColor(4);
	  tgr[2*i]->SetMarkerStyle(21);
	  tgr[2*i]->Draw("LP");
	  tl.SetLineColor(1);
	  tl.SetLineStyle(3);
	  tl.DrawLine(graphtimebegin, 0, graphtimeend, 0);
	}
      Pad[ipad[i] + 1]->cd();
      h1 = new TH2F("h1", "", 2, graphtimebegin, graphtimeend, 2, rmsrange[i][0], rmsrange[i][1]);
//       h1->SetMinimum( 0);
//       h1->SetMaximum(100);
      h1->SetStats(kFALSE);
      h1->GetXaxis()->SetTimeDisplay(1);
      h1->GetYaxis()->SetLabelSize(0.06);
      h1->GetXaxis()->SetLabelSize(0.06);
      h1->GetXaxis()->SetTimeOffset(TimeOffsetTicks+6*3600); // compensate for EST/Switzerland
      h1->GetXaxis()->SetTimeFormat("%H:%M");

      h1->DrawCopy();
      delete h1;
      AnyText.SetNDC();
      AnyText.SetTextAngle(90);
      AnyText.SetTextAlign(21);
      AnyText.SetTextSize(0.075);
      AnyText.DrawText(0.05,0.5,htitle[2 * i + 1]);
      AnyText.SetTextAngle(0);
      if (n <= 0)
        {
          AnyText.SetNDC();
          AnyText.SetTextAlign(22);
          AnyText.SetTextSize(0.4);
          AnyText.DrawText(0.5, 0.5, "No Entries");
        }
      else
        {
          tgr[2*i + 1] = new TGraph(n, x, ey);
          tgr[2*i + 1]->SetMarkerColor(3);
          tgr[2*i + 1]->SetMarkerStyle(22);
          tgr[2*i + 1]->Draw("LP");

          tl.SetLineColor(1);
          tl.SetLineStyle(3);
          tl.DrawLine(graphtimebegin, 0, graphtimeend, 0);
        }
      //  gr[0]->GetXaxis()->SetTimeDisplay(1);
      //gr[0]->GetXaxis()->SetLabelSize(0.03);
      // the x axis labeling looks like crap
      // please help me with this, the SetNdivisions
      // don't do the trick
      //gr[0]->GetXaxis()->SetNdivisions(-1006);
      //gr[0]->GetXaxis()->SetTimeOffset(TimeOffsetTicks);
      //gr[0]->GetXaxis()->SetTimeFormat("%Y/%m/%d %H:%M");
      delete [] x;
      delete [] y;
      delete [] ey;
    }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "MCR Vtx History"
  << " Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 0.98, runstring.c_str());
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.95, "Rhic Server okay");
  TC[1]->Update();

  return 0;
}

void
McrMonDraw::setrmsrange(const int i, const float val1, const float val2 )
{
  rmsrange[i][0] = val1;
  rmsrange[i][1] = val2;
  return;
}

void
McrMonDraw::setvtxrange(const int i, const float val1, const float val2 )
{
  vtxrange[i][0] = val1;
  vtxrange[i][1] = val2;
  return;
}
