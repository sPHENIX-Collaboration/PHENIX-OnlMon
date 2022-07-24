// Do not use #include "bla.h", it has peculiar
// effects on include paths (it uses . first which can lead to
// very interesting effects), use #include <bla.h> instead
#include <SvxVertexDraw.h>
#include <OnlMonClient.h>

#include <phool.h>

#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TGaxis.h>

#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

SvxVertexDraw::SvxVertexDraw(const char *name): OnlMonDraw(name)
{

}

int SvxVertexDraw::Init()
{
  return 0;
}

int SvxVertexDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();

  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  ostringstream tname;
  ostringstream tdesc;

  if (!strcmp(name, "SvxVertex_Vertex"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[0] = new TCanvas(name, "VTX Vertex", -1, 0, xsize*7./8. , ysize*3./4.);
      // root is pathetic, whenever a new TCanvas is created root piles up
      // 6kb worth of X11 events which need to be cleared with
      // gSystem->ProcessEvents(), otherwise your process will grow and
      // grow and grow but will not show a definitely lost memory leak
      gSystem->ProcessEvents();

      float lox[6]={0.00, 0.33, 0.66, 0.00, 0.33, 0.66};
      float hix[6]={0.33, 0.66, 1.00, 0.33, 0.66, 1.00};
      float loy[6]={0.45, 0.45, 0.45, 0.00, 0.00, 0.00};
      float hiy[6]={0.90, 0.90, 0.90, 0.45, 0.45, 0.45};

      for(int ii=0; ii<6; ii++)
	{
	  TC[0]->cd();
	  tname.str("");
	  tname<<"SvxVertex_Vertex_"<<ii;
	  TP[0][ii]=new TPad(tname.str().c_str(),"",lox[ii],loy[ii],hix[ii],hiy[ii],0);
	  TP[0][ii]->Draw();
	}
      // this one is used to plot the run number on the canvas
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
    }

  return 0;
}

int SvxVertexDraw::Draw(const char *what)
{
  int iret = 0;
  int idraw = 0;
  if (!strcmp(what, "ALL") || !strcmp(what, "SvxVertex_Vertex"))
    {
      iret += DrawVertex();
      idraw ++;
    }

  return iret;
}

int SvxVertexDraw::DrawVertex()
{
  int icanvas=0;
  OnlMonClient *cl = OnlMonClient::instance();

  if (! gROOT->FindObject("SvxVertex_Vertex"))
    {
      MakeCanvas("SvxVertex_Vertex");
    }
  TC[icanvas]->Clear("D");

  TH1F *h_bbc_z           = (TH1F*)cl->getHisto("h_bbc_z");
  TH1F *h_vtx_z           = (TH1F*)cl->getHisto("h_vtx_z");
  TH2F *h_vtx_xy          = (TH2F*)cl->getHisto("h_vtx_xy");
  TH1F *h_bbcz_minus_vtxz = (TH1F*)cl->getHisto("h_bbcz_minus_vtxz");
  TH2F *h_bbcz_vs_vtxz    = (TH2F*)cl->getHisto("h_bbcz_vs_vtxz");

  if (   !h_bbc_z 
      || !h_vtx_z
      || !h_vtx_xy 
      || !h_bbcz_minus_vtxz 
      || !h_bbcz_vs_vtxz )
    {
      DrawDeadServer(transparent[icanvas]);
      TC[icanvas]->Update();
      return -1;
    }

  TP[icanvas][0]->cd();
  TP[icanvas][0]->SetTickx();
  TP[icanvas][0]->SetTicky();
  h_bbc_z->Draw();
  TP[icanvas][0]->Update();

  TP[icanvas][1]->cd();
  TP[icanvas][1]->SetTickx();
  TP[icanvas][1]->SetTicky();
  h_vtx_z->Draw();
  TP[icanvas][1]->Update();

  TP[icanvas][2]->cd();
  TP[icanvas][2]->SetTickx();
  TP[icanvas][2]->SetTicky();
  h_vtx_xy->GetYaxis()->SetTitleOffset(1.4);
  h_vtx_xy->Draw("colz");
  TP[icanvas][2]->Update();

  TP[icanvas][3]->cd();
  TP[icanvas][3]->SetTickx();
  TP[icanvas][3]->SetTicky();
  h_bbcz_minus_vtxz->Draw();
  TP[icanvas][3]->Update();

  TP[icanvas][4]->cd();
  TP[icanvas][4]->SetTickx();
  TP[icanvas][4]->SetTicky();
  h_bbcz_vs_vtxz->Draw("colz");
  TP[icanvas][4]->Update();


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

  transparent[icanvas]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[icanvas]->Update();

  return 0;
}

int SvxVertexDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "SVXVERTEX MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}

int SvxVertexDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // on error no ps files please
    {
      return iret;
    }
  filename.str("");
  filename << ThisName << "_0_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());

  return 0;
}

int SvxVertexDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  string pngfile = cl->htmlRegisterPage(*this, "Svx Vertex", "0", "png");
  cl->CanvasToPng(TC[0], pngfile);

  cl->SaveLogFile(*this);
  return 0;
}

