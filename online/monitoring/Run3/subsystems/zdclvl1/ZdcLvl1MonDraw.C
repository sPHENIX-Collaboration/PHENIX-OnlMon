#include <ZdcLvl1MonDraw.h>
#include <OnlMonClient.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

ZdcLvl1MonDraw::ZdcLvl1MonDraw(): OnlMonDraw("ZDCLVL1MON")
{
return;
}

int 
ZdcLvl1MonDraw::MakeCanvases(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (!strcmp(name, "ZdcLvl1Mon_1"))
    {
      ZdcLvl1Canv[0] = new TCanvas(name, "ZdcLvl1Mon System Overview", -1, 0, xsize, ysize);
      gSystem->ProcessEvents();
      ZdcLvl1Canv[0]->cd();
      string PadName;
      double xlow = 0., ylow = 0., xhigh = 0., yhigh = 0.;
      for (int ipad = 0; ipad < 4; ipad++)
        {
          PadName = "ZdcLvl1Pad";
          PadName += ipad;
          if (ipad < 2)
            {
              xlow = ipad * 0.5;
              ylow = 0.49;
              xhigh = (ipad + 1) * 0.5;
              yhigh = 0.94;
            }
          else if( (ipad>=2) && (ipad<4) )
            {
              xlow = (ipad - 2) * 0.5;
              ylow = 0.0;
              xhigh = (ipad - 1) * 0.5;
              yhigh = 0.47;
            }

          ZdcLvl1Pad[0][ipad] = new TPad(PadName.c_str(), "", xlow, ylow, xhigh, yhigh, 0, 0);
          ZdcLvl1Pad[0][ipad]->Draw();
        }
      transparent[0] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
    }

  return 0;
}

int ZdcLvl1MonDraw::Draw(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("ZdcLvl1Mon_1"))
    {
      MakeCanvases("ZdcLvl1Mon_1");
    }

  TH1 *ZdcLvl1_ZDCVertex = cl->getHisto("ZdcLvl1_ZDCVertex");
  if (!ZdcLvl1_ZDCVertex)
    {
      for (int i = 0; i < NUM_CANVASES; i++)
	{
	  ZdcLvl1Canv[i]->cd();
	  ZdcLvl1Canv[i]->Clear("D");
	  transparent[i]->Clear();
	  DrawDeadServer(transparent[i]);
	  ZdcLvl1Canv[i]->Update();
	}
      cout << "ZdcLvl1_ZDCVertex does not exist!" << endl;
      return -1;
  }

  TH1 *ZdcLvl1_ZDCSimVertex = cl->getHisto("ZdcLvl1_ZDCSimVertex");
  if (!ZdcLvl1_ZDCSimVertex)
    {
      cout << "ZdcLvl1_ZDCSimVertex does not exist!" << endl;
      return -1;
    }
  TH1 *ZdcLvl1_ZDCVertexDiff = cl->getHisto("ZdcLvl1_ZDCVertexDiff");
  if (!ZdcLvl1_ZDCVertexDiff)
    {
      cout << "ZdcLvl1_ZDCVertexDiff does not exist!" << endl;
      return -1;
    }
  TH1 *ZdcLvl1_ZDCVertexCutA = cl->getHisto("ZdcLvl1_ZDCVertexCutA");
  if (!ZdcLvl1_ZDCVertexCutA)
    {
      cout << "ZdcLvl1_ZDCVertexCutA does not exist!" << endl;
      return -1;
    }
  TH1 *ZdcLvl1_ZDCVertexCutB = cl->getHisto("ZdcLvl1_ZDCVertexCutB");
  if (!ZdcLvl1_ZDCVertexCutB)
    {
      cout << "ZdcLvl1_ZDCVertexCutB does not exist!" << endl;
      return -1;
    }
  TH1 *ZdcLvl1_ZDCVertexNoCut = cl->getHisto("ZdcLvl1_ZDCVertexNoCut");
  if (!ZdcLvl1_ZDCVertexNoCut)
    {
      cout << "ZdcLvl1_ZDCVertexNoCut does not exist!" << endl;
      return -1;
    }

  // Page 1

  ZdcLvl1Canv[0]->cd();

  ZdcLvl1Pad[0][0]->cd();
  FormatHist(ZdcLvl1_ZDCVertex);
  ZdcLvl1_ZDCVertex->Draw();

  ZdcLvl1Pad[0][1]->cd();
  FormatHist(ZdcLvl1_ZDCSimVertex);
  ZdcLvl1_ZDCSimVertex->Draw();

  ZdcLvl1Pad[0][2]->cd();
  FormatHist(ZdcLvl1_ZDCVertexDiff);
  ZdcLvl1_ZDCVertexDiff->SetMinimum(0.5);
  ZdcLvl1Pad[0][2]->SetLogy();
  ZdcLvl1_ZDCVertexDiff->Draw();

  ZdcLvl1Pad[0][3]->cd();
  FormatHist(ZdcLvl1_ZDCVertexNoCut);
  ZdcLvl1_ZDCVertexNoCut->SetLineColor(1); 
  ZdcLvl1_ZDCVertexNoCut->Draw();
  FormatHist(ZdcLvl1_ZDCVertexCutA);
  ZdcLvl1_ZDCVertexCutA->Draw("same");
  FormatHist(ZdcLvl1_ZDCVertexCutB);
  ZdcLvl1_ZDCVertexCutB->SetLineColor(2); 
  ZdcLvl1_ZDCVertexCutB->Draw("same");

  transparent[0]->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  runnostream << "ZDC LL1 Monitor: Run " << cl->RunNumber();
  time_t evttime = cl->EventTime("CURRENT");
  runnostream << " " << ctime(&evttime) ;
  transparent[0]->Clear();
  PrintRun.DrawText(0.5, 1., runnostream.str().c_str());

  ZdcLvl1Canv[0]->Update();

  return 0;
}

int ZdcLvl1MonDraw::MakePS(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  ZdcLvl1Canv[0]->Print(filename.str().c_str());

  return 0;
}

int ZdcLvl1MonDraw::MakeHtml(const char *what)
{

  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();

  string ofile = cl->htmlRegisterPage(*this,"Monitor","1","png");
  cl->CanvasToPng(ZdcLvl1Canv[0], ofile);
  cl->SaveLogFile(*this);
  
  return 0;
}

void ZdcLvl1MonDraw::FormatHist(TH1 *Hist)
{

  Hist->SetStats(0);
  Hist->SetLineWidth(1);
  Hist->SetLineColor(4);
  Hist->SetTitleSize(0.8);
  Hist->GetXaxis()->SetLabelSize(0.07);
  Hist->GetYaxis()->SetLabelSize(0.07);
  return ;
}

int ZdcLvl1MonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "ZDCLVL1MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}

