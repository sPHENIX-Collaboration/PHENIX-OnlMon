#include <PbGlUMonDraw.h>
#include <OnlMonClient.h>

#include <phool.h>

#include <TCanvas.h>
#include <TPaveText.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TPave.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

PbGlUMonDraw::PbGlUMonDraw(const char *name): OnlMonDraw(name)
{
  gStyle->SetPalette(100, 0);
  gStyle->SetOptTitle(kFALSE);
  gStyle->SetOptStat(0);
  TC[0] = 0;
  transparent[0] = 0;
  pave = 0;
  return ;
}

PbGlUMonDraw::~PbGlUMonDraw()
{
  delete pave;
  return ;
}

int PbGlUMonDraw::Init()
{
  pave = new TPaveText(0.05, 0.05, 0.95, 0.7);
  return 0;
}

int PbGlUMonDraw::MakeCanvas(const char *name)
{

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (!strcmp(name, "PbGlUMon"))
    {
      TC[0] = new TCanvas(name, "PbGl U-base monitor ", -1, 0, xsize / 2 , (int)(ysize * 0.75));
      gSystem->ProcessEvents();
      Pad[0] = new TPad("pbglu_map", "", 0, 0, 1, 0.75,0);
      Pad[1] = new TPad("pbglu_rep", "", 0, 0.75, 1, 1,0);
      Pad[0]->Draw();
      Pad[1]->Draw();

      // this one is used to plot the run number on the canvas
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
      TC[0]->SetEditable(0);
    }
  return 0;
}

int PbGlUMonDraw::Draw(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *hese = (TH2*)cl->getHisto("pbglu_hese");
  TH2 *hbese = (TH2*)cl->getHisto("pbglu_hbese");
  TH2 *hcese = (TH2*)cl->getHisto("pbglu_hcese");
  if (! gROOT->FindObject("PbGlUMon"))
    {
      MakeCanvas("PbGlUMon");
    }
  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  TLine li1;
  TBox tpp;
  pave->Clear();
  if (hese)
    {
      Pad[0]->cd();
      hese->SetStats(0);
      hese->SetMaximum(1.0);
      hese->SetMinimum(0.01);
      hese->DrawCopy("col");
      for (int i1 = 2;i1 < 96;i1 += 2)
        {
          li1.SetLineColor(16);
          li1.SetLineWidth(1);
          li1.DrawLine(i1, 0., i1, 96.);
        }
      for (int i1 = 2;i1 < 96;i1 += 2)
        {
          li1.SetLineColor(16);
          li1.SetLineWidth(1);
          li1.DrawLine(0., i1, 96, i1);
        }
      for (int i1 = 12;i1 < 96;i1 += 12)
        {
          li1.SetLineColor(kBlack);
          li1.SetLineWidth(1);
          li1.DrawLine(0., i1, 96, i1);
        }
      for (int i1 = 12;i1 < 96;i1 += 12)
        {
          li1.SetLineColor(kBlack);
          li1.SetLineWidth(1);
          li1.DrawLine(i1, 0., i1, 96.);
        }

      li1.SetLineColor(kBlack);
      li1.SetLineWidth(2);
      li1.DrawLine(0., 48, 96, 48);
      int nbads = 0;
      for (int i = 0;i < 96;i++)
        for (int j = 0;j < 96;j++)
          {
            if (hcese->GetBinContent(i + 1, j + 1) > 0)
              {
                tpp.SetFillColor(kGreen);
                tpp.DrawBox(i, j, i + 1, j + 1);
              }
            if (hbese->GetBinContent(i + 1, j + 1) > 0)
              {
                tpp.SetFillColor(kGreen);
                tpp.DrawBox(i, j, i + 1, j + 1);
                nbads++;
              }

          }
      Pad[1]->cd();
      if (nbads == 0)
        {
          pave->SetFillColor(kGreen);
          pave->AddText(" ");
          pave->AddText("Everything is OK");
          pave->AddText("Thank you for looking");
          pave->AddText(" ");
        }
      else
        {
          pave->SetFillColor(kRed);
          pave->AddText(" ");
          TString ss("Bad channels: ");
          ss += nbads;
          pave->AddText(ss.Data());
          pave->AddText("This is a PbGl tool, the experts look at it");
//           pave->AddText("send an e-mail to stkav@rcf.rhic.bnl.gov");
//           pave->AddText("with the run number.");
          pave->AddText(" ");
        }
      pave->Draw();


    }
  else
    {
      DrawDeadServer(transparent[0]);
      TC[0]->SetEditable(0);
      return -1;
    }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << " Run " << cl->RunNumber()
	      << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[0]->Update();

  return 0;
}



int PbGlUMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(33); // center/top alignment
  FatalMsg.DrawText(0.9, 0.9, "PBGL-U MONITOR");
  FatalMsg.SetTextAlign(32); // center/center alignment
  FatalMsg.DrawText(0.9, 0.5, "SERVER");
  FatalMsg.SetTextAlign(31); // center/bottom alignment
  FatalMsg.DrawText(0.9, 0.1, "DEAD");
  transparent->Update();

  return 0;
}

int PbGlUMonDraw::MakePS(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // on error no ps files please
    {
      return iret;
    }
  filename << "PbGlUMon" << "_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");
  return 0;
}

int PbGlUMonDraw::MakeHtml(const char *what)
{

  int iret = Draw(what);
  if (iret) // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  string pngfile = cl->htmlRegisterPage(*this, "PbGlU", "1", "png");
  cl->CanvasToPng(TC[0], pngfile);


  cl->SaveLogFile(*this);
  return 0;
}

