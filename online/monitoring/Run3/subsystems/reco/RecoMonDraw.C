// Do not use #include "bla.h", it has peculiar
// effects on include paths (it uses . first which can lead to
// very interesting effects), use #include <bla.h> instead
#include <RecoMonDraw.h>
#include <OnlMonClient.h>

#include <phool.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

RecoMonDraw::RecoMonDraw(const char *name): OnlMonDraw(name)
{
  // memset can be used to set an array to zero
  // without knowing its size (works on bytes, init to 1 won't work,
  // init for floatss only works for 0)
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  return ;
}

int RecoMonDraw::Init()
{
  return 0;
}

int RecoMonDraw::MakeCanvas(const char *name)
{

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  if (!strcmp(name, "RecoMon1"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[0] = new TCanvas(name, "RecoMon Page 1", -1, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[0] = new TPad("recopad1", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[1] = new TPad("recopad2", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[0]->Draw();
      Pad[1]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
      TC[0]->SetEditable(0);
    }

  else if (!strcmp(name, "RecoMon2"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[1] = new TCanvas(name, "RecoMon Page 2", -xsize / 2, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[2] = new TPad("recopad3", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[3] = new TPad("recopad4", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[2]->Draw();
      Pad[3]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
      TC[1]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon3"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[2] = new TCanvas(name, "RecoMon Page 3", -1, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[4] = new TPad("recopad5", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[5] = new TPad("recopad6", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[4]->Draw();
      Pad[5]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
      transparent[2]->SetFillStyle(4000);
      transparent[2]->Draw();
      TC[2]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon4"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[3] = new TCanvas(name, "RecoMon Page 4", -xsize / 2, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[6] = new TPad("recopad7", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[7] = new TPad("recopad8", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[6]->Draw();
      Pad[7]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
      transparent[3]->SetFillStyle(4000);
      transparent[3]->Draw();
      TC[3]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon5"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[4] = new TCanvas(name, "RecoMon Page 5", -1, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[8] = new TPad("recopad9", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[9] = new TPad("recopad10", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[8]->Draw();
      Pad[9]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
      transparent[4]->SetFillStyle(4000);
      transparent[4]->Draw();
      TC[4]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon6"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[5] = new TCanvas(name, "RecoMon Page 6", -xsize / 2, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[10] = new TPad("recopad11", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[11] = new TPad("recopad12", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[10]->Draw();
      Pad[11]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[5] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
      transparent[5]->SetFillStyle(4000);
      transparent[5]->Draw();
      TC[5]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon7"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[6] = new TCanvas(name, "RecoMon Page 7", -1, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[12] = new TPad("recopad13", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[13] = new TPad("recopad14", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[12]->Draw();
      Pad[13]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
      transparent[6]->SetFillStyle(4000);
      transparent[6]->Draw();
      TC[6]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon8"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[7] = new TCanvas(name, "RecoMon Page 8", -xsize / 2, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[14] = new TPad("recopad15", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[15] = new TPad("recopad16", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[14]->Draw();
      Pad[15]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[7] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
      transparent[7]->SetFillStyle(4000);
      transparent[7]->Draw();
      TC[7]->SetEditable(0);
    }

  if (!strcmp(name, "RecoMon9"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[8] = new TCanvas(name, "RecoMon Page 9", -1, 0, xsize / 2 , ysize);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();
      Pad[16] = new TPad("recopad17", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[17] = new TPad("recopad18", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[16]->Draw();
      Pad[17]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[8] = new TPad("transparent8", "this does not show", 0, 0, 1, 1);
      transparent[8]->SetFillStyle(4000);
      transparent[8]->Draw();
      TC[8]->SetEditable(0);
    }

  return 0;

}

int RecoMonDraw::Draw(const char *what)
{
  int iret = 0;
  int idraw = 0;
  if (!strcmp(what, "ALL") || !strcmp(what, "FIRST"))
    {
      iret += DrawFirst();
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "SECOND"))
    {
      iret += DrawSecond();
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "THIRD"))
    {
      iret += DrawThird();
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "FOURTH"))
    {
      iret += DrawFourth();
      idraw ++;
    }
//   if (!strcmp(what, "RecoMon9"))
//     {
//       iret += DrawFirst(what);
//       idraw ++;
//     }

  return iret;

}

int
RecoMonDraw::DrawFirst()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *recomon_hist01 = (TH2 *) cl->getHisto("reco_bbc_pc1");
  TH2 *recomon_hist02 = (TH2 *) cl->getHisto("reco_bbc_pc3");
  TH1 *recomon_hist03 = (TH1 *) cl->getHisto("reco_bbc_qtot");
  TH1 *recomon_hist04 = (TH1 *) cl->getHisto("reco_bbc_ntot");

  if (! gROOT->FindObject("RecoMon1"))
    {
      MakeCanvas("RecoMon1");
    }
  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  Pad[0]->cd();
  if (recomon_hist01)
    {
      recomon_hist01->SetMarkerStyle(20);
      recomon_hist01->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[0]);
      TC[0]->SetEditable(0);
      return -1;
    }
  Pad[1]->cd();
  if (recomon_hist02)
    {
      recomon_hist02->SetMarkerStyle(20);
      recomon_hist02->DrawCopy();
    }
  if (! gROOT->FindObject("RecoMon2"))
    {
      MakeCanvas("RecoMon2");
    }
  TC[1]->SetEditable(1);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (recomon_hist03)
    {
      recomon_hist03->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[1]);
      TC[1]->SetEditable(0);
      return -1;
    }
  Pad[3]->cd();
  if (recomon_hist04)
    {
      recomon_hist04->DrawCopy();
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
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(0);

  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(0);

  return 0;

}

int
RecoMonDraw::DrawSecond()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *recomon_hist05 = (TH1 *) cl->getHisto("reco_bbc_zvtx");
  TH1 *recomon_hist06 = (TH1 *) cl->getHisto("reco_bbc_tzero");
  TH2 *recomon_hist07 = (TH2 *) cl->getHisto("reco_bbc_nns");
  TH2 *recomon_hist08 = (TH2 *) cl->getHisto("reco_bbc_qns");

  if (! gROOT->FindObject("RecoMon3"))
    {
      MakeCanvas("RecoMon3");
    }
  TC[2]->SetEditable(1);
  TC[2]->Clear("D");
  Pad[4]->cd();
  if (recomon_hist05)
    {
      recomon_hist05->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[2]);
      TC[2]->SetEditable(0);
      return -1;
    }
  Pad[5]->cd();
  if (recomon_hist06)
    {
      recomon_hist06->DrawCopy();
    }
  if (! gROOT->FindObject("RecoMon4"))
    {
      MakeCanvas("RecoMon4");
    }
  TC[3]->SetEditable(1);
  TC[3]->Clear("D");
  Pad[6]->cd();
  if (recomon_hist07)
    {
      recomon_hist07->SetMarkerStyle(20);
      recomon_hist07->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[3]);
      TC[3]->SetEditable(0);
      return -1;
    }
  Pad[7]->cd();
  if (recomon_hist08)
    {
      recomon_hist08->SetMarkerStyle(20);
      recomon_hist08->DrawCopy();
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

  transparent[2]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[2]->Update();
  TC[2]->Show();
  TC[2]->SetEditable(0);

  transparent[3]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(0);

  return 0;

}

int
RecoMonDraw::DrawThird()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *recomon_hist09 = (TH1 *) cl->getHisto("reco_dch_ntracks");
  TH2 *recomon_hist10 = (TH2 *) cl->getHisto("reco_dch_pc1");
  TH1 *recomon_hist11 = (TH1 *) cl->getHisto("reco_emc_nclus");
  TH1 *recomon_hist12 = (TH1 *) cl->getHisto("reco_emc_energy");

  if (! gROOT->FindObject("RecoMon5"))
    {
      MakeCanvas("RecoMon5");
    }
  TC[4]->SetEditable(1);
  TC[4]->Clear("D");
  Pad[8]->cd();
  if (recomon_hist09)
    {
      recomon_hist09->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[4]);
      TC[4]->SetEditable(0);
      return -1;
    }
  Pad[9]->cd();
  if (recomon_hist10)
    {
      recomon_hist10->SetMarkerStyle(20);
      recomon_hist10->DrawCopy();
    }
  if (! gROOT->FindObject("RecoMon6"))
    {
      MakeCanvas("RecoMon6");
    }
  TC[5]->SetEditable(1);
  TC[5]->Clear("D");
  Pad[10]->cd();
  if (recomon_hist11)
    {
      recomon_hist11->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[5]);
      TC[5]->SetEditable(0);
      return -1;
    }
  Pad[11]->cd();
  if (recomon_hist12)
    {
      recomon_hist12->DrawCopy();
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

  transparent[4]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(0);

  transparent[5]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[5]->Update();
  TC[5]->Show();
  TC[5]->SetEditable(0);

  return 0;

}

int
RecoMonDraw::DrawFourth()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *recomon_hist13 = (TH1 *) cl->getHisto("reco_pc1_nclus");
  TH1 *recomon_hist14 = (TH1 *) cl->getHisto("reco_pc3_nclus");
  //TH1 *recomon_hist15 = (TH1 *) cl->getHisto("reco_rxn_qtot");
  //TH2 *recomon_hist16 = (TH2 *) cl->getHisto("reco_bbc_rxn");
  if (! gROOT->FindObject("RecoMon7"))
    {
      MakeCanvas("RecoMon7");
    }
  TC[6]->SetEditable(1);
  TC[6]->Clear("D");
  Pad[12]->cd();
  if (recomon_hist13)
    {
      recomon_hist13->DrawCopy();
    }
  else
    {
      DrawDeadServer(transparent[6]);
      TC[6]->SetEditable(0);
      return -1;
    }
  Pad[13]->cd();
  if (recomon_hist14)
    {
      recomon_hist14->DrawCopy();
    }
  if (! gROOT->FindObject("RecoMon8"))
    {
      MakeCanvas("RecoMon8");
    }
  //TC[7]->SetEditable(1);
  //TC[7]->Clear("D");
  //Pad[14]->cd();
  //if (recomon_hist15)
  //  {
  //    recomon_hist15->DrawCopy();
  //  }
  //else
  //  {
  //    DrawDeadServer(transparent[7]);
  //    TC[7]->SetEditable(0);
  //    return -1;
  //  }
  //Pad[15]->cd();
  //if (recomon_hist16)
  //  {
  //    recomon_hist16->SetMarkerStyle(20);
  //    recomon_hist16->DrawCopy();
  //  }

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

  transparent[6]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[6]->Update();
  TC[6]->Show();
  TC[6]->SetEditable(0);

  //transparent[7]->cd();
  //PrintRun.DrawText(0.5, 1., runstring.c_str());
  //TC[7]->Update();
  //TC[7]->Show();
  //TC[7]->SetEditable(0);

  return 0;

}


int RecoMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "RECO MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}

int RecoMonDraw::MakePS(const char *what)
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

int RecoMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  string pngfile = cl->htmlRegisterPage(*this, "Bbc vs PC", "1", "png");
  cl->CanvasToPng(TC[0], pngfile);

  // idem for 2nd canvas.
  pngfile = cl->htmlRegisterPage(*this, "BBC Sum Q,Pmt", "2", "png");
  cl->CanvasToPng(TC[1], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "BBC vtx t0", "3", "png");
  cl->CanvasToPng(TC[2], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "BBC N vs S", "4", "png");
  cl->CanvasToPng(TC[3], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "Dch", "5", "png");
  cl->CanvasToPng(TC[4], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "Emc", "6", "png");
  cl->CanvasToPng(TC[5], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "PC1&3", "7", "png");
  cl->CanvasToPng(TC[6], pngfile);

  //pngfile = cl->htmlRegisterPage(*this, "Rxnp", "8", "png");
  //cl->CanvasToPng(TC[7], pngfile);

  // Now register also EXPERTS html pages, under the EXPERTS subfolder.

  cl->SaveLogFile(*this);
  return 0;
}

