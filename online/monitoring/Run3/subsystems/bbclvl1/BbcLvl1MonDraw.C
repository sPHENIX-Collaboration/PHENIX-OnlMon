#include <BbcLvl1MonDraw.h>
#include <OnlMonClient.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

BbcLvl1MonDraw::BbcLvl1MonDraw(const char *name): OnlMonDraw(name)
{
  oldStyle = gStyle;
  bbcll1Style = new TStyle("bbcll1Style", "BBCLL1 Online Monitor Style");
  bbcll1Style->SetOptStat(110010);
  bbcll1Style->SetFrameBorderMode(0);
  bbcll1Style->SetCanvasColor(0);
  bbcll1Style->SetStatFontSize(0.06);
  oldStyle->cd();
  TC = 0;
  transparent = NULL;
  line1 = NULL;
  line2 = NULL;
  memset(Lvl1Pad, 0, sizeof(Lvl1Pad));
  memset(Lvl1PadErr, 0, sizeof(Lvl1PadErr));
  BbcLvl1_NorthBadHist = new TH1F("BbcLvl1_NorthBadHist", "South Bad Channels",
                                  64, 0, 63.9);
  BbcLvl1_SouthBadHist = new TH1F("BbcLvl1_SouthBadHist", "North Bad Channels",
                                  64, 0, 63.9);
  line1 = new TLine( -1., -1., -1., 10.);
  line1->SetLineColor(2);
  line1->SetLineWidth(5);
  line1->SetLineStyle(2);
  line2 = new TLine(2., -1., 2., 10.);
  line2->SetLineColor(2);
  line2->SetLineWidth(5);
  line2->SetLineStyle(2);

  return ;
}

BbcLvl1MonDraw::~BbcLvl1MonDraw()
{
  delete bbcll1Style;
  delete line1;
  delete line2;
  delete BbcLvl1_NorthBadHist;
  delete BbcLvl1_SouthBadHist;
}

int BbcLvl1MonDraw::MakeCanvases(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (!strcmp(name, "bbclvl1_c1"))
    {
      TC = new TCanvas(name, "Lvl1Monitor", -1, 0, xsize, ysize);
      gSystem->ProcessEvents();
      ostringstream padname;
      double xlow, ylow, xhi, yhi;
      int ipad = 0;
      for (short int i = 0; i < 2; i++)
        {
          // up to 0,9 to make space for the run number
          ylow = 0.4 + i * (0.5 / 2.);
          yhi = 0.4 + (i + 1) * (0.5 / 2.);
          {
            for (short int j = 0; j < 2; j++)
              {
                xlow = j * 1. / 2.;
                xhi = (j + 1) / 2.;
                padname << "Lvl1Pad" << ipad;
                Lvl1Pad[ipad] = new TPad(padname.str().c_str(), "Lvl1Pad", xlow, ylow, xhi, yhi, 0, 0, 0);
                Lvl1Pad[ipad]->Draw();
                ipad++;
                padname.str("");
              }
          }
        }
      string PadName;
      double xhigh, yhigh;
      for (int ipad = 0; ipad < 8; ipad++)
        {
          PadName = "Lvl1PadErr";
          PadName += ipad;
          if (ipad < 4)
            {
              xlow = ipad * 0.25;
              ylow = 0.2;
              xhigh = (ipad + 1) * 0.25;
              yhigh = 0.4;
            }
          else
            {
              xlow = (ipad - 4) * 0.25;
              ylow = 0.;
              xhigh = (ipad - 3) * 0.25;
              yhigh = 0.2;
            }
          Lvl1PadErr[ipad] = new TPad(PadName.c_str(), "", xlow, ylow, xhigh, yhigh, 0, 0, 0);
          Lvl1PadErr[ipad]->Draw();
        }

      transparent = new TPad("transparent", "this does not show", 0, 0, 1, 1, 0, 0);
      transparent->SetFillStyle(4000);
      transparent->Draw();
    }
  return 0;
}

int BbcLvl1MonDraw::Draw(const char *what)
{
  bbcll1Style->cd();
  OnlMonClient *cl = OnlMonClient::instance();
  // test for canvases and create if deleted
  if (!gROOT->FindObject("bbclvl1_c1"))
    {
      MakeCanvases("bbclvl1_c1");
    }
  TH1 *BbcLvl1_VertHist = cl->getHisto("BbcLvl1_VertHist");
  if (!BbcLvl1_VertHist)
    {
      TC->cd();
      TC->Clear("D");
      transparent->Clear();
      DrawDeadServer(transparent);
      TC->Update();
      cout << "BbcLvl1_VertHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_Vert2Hist = cl->getHisto("BbcLvl1_Vert2Hist");
  if (!BbcLvl1_Vert2Hist)
    {
      cout << "BbcLvl1_Vert2Hist does not exist!" << endl;
      return -1;
    }

  TH1 *BbcLvl1_AverHist = cl->getHisto("BbcLvl1_AverHist");
  if (!BbcLvl1_AverHist)
    {
      cout << "BbcLvl1_AverHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_SHitsHist = cl->getHisto("BbcLvl1_SHitsHist");
  if (!BbcLvl1_SHitsHist)
    {
      cout << "BbcLvl1_SHitsHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_NHitsHist = cl->getHisto("BbcLvl1_NHitsHist");
  if (!BbcLvl1_NHitsHist)
    {
      cout << "BbcLvl1_NHitsHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_VertHistErr = cl->getHisto("BbcLvl1_VertHistErr");
  if (!BbcLvl1_VertHistErr)
    {
      cout << "BbcLvl1_VertHistErr does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_AverHistErr = cl->getHisto("BbcLvl1_AverHistErr");
  if (!BbcLvl1_AverHistErr)
    {
      cout << "BbcLvl1_AverHistErr does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_SHitsHistErr = cl->getHisto("BbcLvl1_SHitsHistErr");
  if (!BbcLvl1_SHitsHistErr)
    {
      cout << "BbcLvl1_SHitsHistErr does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_NHitsHistErr = cl->getHisto("BbcLvl1_NHitsHistErr");
  if (!BbcLvl1_NHitsHistErr)
    {
      cout << "BbcLvl1_NHitsHistErr does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_STimeHistErr = cl->getHisto("BbcLvl1_STimeHistErr");
  if (!BbcLvl1_STimeHistErr)
    {
      cout << "BbcLvl1_STimeHistErr does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_NTimeHistErr = cl->getHisto("BbcLvl1_NTimeHistErr");
  if (!BbcLvl1_NTimeHistErr)
    {
      cout << "BbcLvl1_NTimeHistErr does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_NChAllHist = cl->getHisto("BbcLvl1_NChAllHist");
  if (!BbcLvl1_NChAllHist)
    {
      cout << "BbcLvl1_NChAllHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_SChAllHist = cl->getHisto("BbcLvl1_SChAllHist");
  if (!BbcLvl1_SChAllHist)
    {
      cout << "BbcLvl1_SChAllHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_NChBadHist = cl->getHisto("BbcLvl1_NChBadHist");
  if (!BbcLvl1_NChBadHist)
    {
      cout << "BbcLvl1_NChBadHist does not exist!" << endl;
      return -1;
    }
  TH1 *BbcLvl1_SChBadHist = cl->getHisto("BbcLvl1_SChBadHist");
  if (!BbcLvl1_SChBadHist)
    {
      cout << "BbcLvl1_SChBadHist does not exist!" << endl;
      return -1;
    }

  TH1 *BbcLvl1_DataError = cl->getHisto("BbcLvl1_DataError");
  if (!BbcLvl1_DataError)
    {
      cout << "BbcLvl1_DataError does not exist!" << endl;
      return -1;
    }

  // First canvas:
  TC->cd();
  Lvl1Pad[0]->cd();
  Lvl1Pad[0]->SetBottomMargin(0.13);
  FormatHist(BbcLvl1_VertHist);
  BbcLvl1_VertHist->Draw();
  FormatHist(BbcLvl1_Vert2Hist);
  BbcLvl1_Vert2Hist->SetLineColor(2); 
  BbcLvl1_Vert2Hist->Draw("same");

  Lvl1Pad[1]->cd();
  Lvl1Pad[1]->SetBottomMargin(0.13);
  FormatHist(BbcLvl1_SHitsHist);
  //  BbcLvl1_SHitsHist->SetTitleSize(1.);
  BbcLvl1_SHitsHist->Draw();

  Lvl1Pad[2]->cd();
  Lvl1Pad[2]->SetBottomMargin(0.13);
  FormatHist(BbcLvl1_AverHist);
  BbcLvl1_AverHist->Draw();

  Lvl1Pad[3]->cd();
  Lvl1Pad[3]->SetBottomMargin(0.13);
  FormatHist(BbcLvl1_NHitsHist);
  // BbcLvl1_NHitsHist->SetTitleSize(1.);
  BbcLvl1_NHitsHist->Draw();


  // Second canvas:
  TC->cd();
  transparent->cd();
  transparent->Clear();
  ostringstream MonId;
  MonId << "Bbc Lvl1 Monitor";
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.05);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  PrintRun.DrawText(0.5, 0.99,MonId.str().c_str());
  MonId.str("");
  MonId << "Run " << cl->RunNumber();
  time_t evttime = cl->EventTime("CURRENT");
  MonId << ", Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.945,MonId.str().c_str());

  // Flag the number of "error" events on the canvas - in RED!!!!

  if ( BbcLvl1_DataError->GetEntries() > 10 )
    {

      ostringstream ErrText;
      TText *labeltext = new TText();
      labeltext->SetTextSize(0.075);
      labeltext->SetNDC(kTRUE);
      labeltext->SetTextColor(2);
      labeltext->SetTextAlign(23);

      ErrText.str("");
      //ErrText << "WARNING: Detected " << BbcLvl1_DataError->GetEntries() << " severe errors!" << endl;
      ErrText << "WARNING: Detected >1% severe errors in last 1k events!" << endl;

      labeltext->DrawText( 0.50, 0.15, ErrText.str().c_str());

      delete labeltext;

    }

  Lvl1PadErr[0]->cd();
  Lvl1PadErr[0]->SetBottomMargin(0.13);
  Lvl1PadErr[0]->SetLogy(0);
  if (BbcLvl1_VertHistErr->GetEntries() > 0)
    {
      BbcLvl1_VertHistErr->SetMinimum(1.);
      Lvl1PadErr[0]->SetLogy();
    }
  FormatHist(BbcLvl1_VertHistErr);
  BbcLvl1_VertHistErr->Draw();
  line1->DrawLine( -2., 0, -2., 10);
  line2->DrawLine(3., 0, 3., 10);

  Lvl1PadErr[1]->cd();
  Lvl1PadErr[1]->SetBottomMargin(0.13);
  Lvl1PadErr[1]->SetLogy(0);
  if (BbcLvl1_NHitsHistErr->GetEntries() > 0)
    {
      BbcLvl1_NHitsHistErr->SetMinimum(1.);
      Lvl1PadErr[1]->SetLogy();
    }
  FormatHist(BbcLvl1_NHitsHistErr);
  //  BbcLvl1_NHitsHistErr->SetTitleSize(1.7);
  BbcLvl1_NHitsHistErr->Draw();
  line1->SetX1( -1.);
  line1->SetX2( -1.);
  line1->Draw();
  line2->SetX1(2.);
  line2->SetX2(2.);
  line2->Draw();

  Lvl1PadErr[2]->cd();
  Lvl1PadErr[2]->SetBottomMargin(0.13);
  Lvl1PadErr[2]->SetLogy(0);
  if (BbcLvl1_NTimeHistErr->GetEntries() > 0)
    {
      BbcLvl1_NTimeHistErr->SetMinimum(1.);
      Lvl1PadErr[2]->SetLogy();
    }
  FormatHist(BbcLvl1_NTimeHistErr);
  // BbcLvl1_NTimeHistErr->SetTitleSize(1.7);
  BbcLvl1_NTimeHistErr->Draw();
  line1->Draw();
  line2->Draw();

  Lvl1PadErr[3]->cd();
  Lvl1PadErr[3]->SetBottomMargin(0.13);
  for (int i = 0; i < 64; i++)
    {
      if (BbcLvl1_NChAllHist->GetBinContent(i + 1) != 0)
        {
          BbcLvl1_NorthBadHist->SetBinContent(i + 1, BbcLvl1_NChBadHist->GetBinContent(i + 1) /
                                              BbcLvl1_NChAllHist->GetBinContent(i + 1));
        }
      else
        {
          BbcLvl1_NorthBadHist->SetBinContent(i + 1, 0.);
        }
    }
  FormatHist(BbcLvl1_NorthBadHist);
  BbcLvl1_NorthBadHist->Draw();

  Lvl1PadErr[4]->cd();
  Lvl1PadErr[4]->SetBottomMargin(0.13);
  Lvl1PadErr[4]->SetLogy(0);
  if (BbcLvl1_AverHistErr->GetEntries() > 0)
    {
      BbcLvl1_AverHistErr->SetMinimum(1.);
      Lvl1PadErr[4]->SetLogy();
    }
  FormatHist(BbcLvl1_AverHistErr);
  // BbcLvl1_AverHistErr->SetTitleSize(1.7);

  BbcLvl1_AverHistErr->Draw();
  line1->Draw();
  line2->Draw();

  Lvl1PadErr[5]->cd();
  Lvl1PadErr[5]->SetBottomMargin(0.13);
  Lvl1PadErr[5]->SetLogy(0);
  if (BbcLvl1_SHitsHistErr->GetEntries() > 0)
    {
      BbcLvl1_SHitsHistErr->SetMinimum(1.);
      Lvl1PadErr[5]->SetLogy();
    }
  FormatHist(BbcLvl1_SHitsHistErr);
  //BbcLvl1_SHitsHistErr->SetTitleSize(1.7);
  BbcLvl1_SHitsHistErr->Draw();
  line1->Draw();
  line2->Draw();

  Lvl1PadErr[6]->cd();
  Lvl1PadErr[6]->SetBottomMargin(0.13);
  Lvl1PadErr[6]->SetLogy(0);
  if (BbcLvl1_STimeHistErr->GetEntries() > 0)
    {
      BbcLvl1_STimeHistErr->SetMinimum(1.);
      Lvl1PadErr[6]->SetLogy();
    }
  FormatHist(BbcLvl1_STimeHistErr);
  // BbcLvl1_STimeHistErr->SetTitleSize(1.7);
  BbcLvl1_STimeHistErr->Draw();
  line1->Draw();
  line2->Draw();

  Lvl1PadErr[7]->cd();
  Lvl1PadErr[7]->SetBottomMargin(0.13);
  for (int i = 0; i < 64; i++)
    {
      if (BbcLvl1_SChAllHist->GetBinContent(i + 1) != 0)
        {
          BbcLvl1_SouthBadHist->SetBinContent(i + 1, BbcLvl1_SChBadHist->GetBinContent(i + 1) /
                                              BbcLvl1_SChAllHist->GetBinContent(i + 1));
        }
      else
        {
          BbcLvl1_SouthBadHist->SetBinContent(i + 1, 0.);
        }
    }
  FormatHist(BbcLvl1_SouthBadHist);
  BbcLvl1_SouthBadHist->Draw();

  TC->Update();
  oldStyle->cd();
  return 0;
}

void BbcLvl1MonDraw::FormatHist(TH1 *Hist)
{

  Hist->SetStats(1);
  Hist->SetLineWidth(3);
  Hist->SetLineColor(4);
  Hist->GetXaxis()->SetLabelSize(0.05);
  Hist->GetXaxis()->SetTitleSize(0.05);
  Hist->GetXaxis()->SetTitleOffset(1.02);
  Hist->GetYaxis()->SetLabelSize(0.05);
  return ;
}

int BbcLvl1MonDraw::MakePS(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  TC->Print(filename.str().c_str());
  return 0;
}

int BbcLvl1MonDraw::MakeHtml(const char *what)
{

  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();

  string giffile = cl->htmlRegisterPage(*this, "Monitor", "1", "png");
  cl->CanvasToPng(TC, giffile);

  cl->SaveLogFile(*this);

  return 0;
}

int BbcLvl1MonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "BBCLVL1MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}
