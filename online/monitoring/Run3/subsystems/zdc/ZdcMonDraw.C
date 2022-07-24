#include "ZdcMonDraw.h"

#include <OnlMonClient.h>
#include <OnlMonDB.h>

#include <phool.h>

#include <TArc.h>
#include <TCanvas.h>
#include <TDatime.h>
#include <TF1.h>
#include <TF2.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>
#include <TLatex.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

ZdcMonDraw::ZdcMonDraw(): OnlMonDraw("ZdcMON")
{
  printf("Starting ZdcMonDraw...\n");

  par = new double[6]; //parameters will be filled in this array

  TStyle* oldStyle = gStyle;
  zdcStyle = new TStyle("zdcStyle", "ZDC Online Monitor Style");
  zdcStyle->SetOptStat(0);
  zdcStyle->SetTitleH(0.075);
  zdcStyle->SetTitleW(0.98);
  zdcStyle->SetPalette(1);
  zdcStyle->SetFrameBorderMode(0);
  zdcStyle->SetCanvasColor(0);
  zdcStyle->SetPadBorderMode(0);
  zdcStyle->SetCanvasBorderMode(0);
  oldStyle->cd(); //zdcStyle made, but current Style is oldStyle
  for (int i = 0; i < NUM_CANV; ++i )
    {
      TC[i] = NULL;
    }
  for (int i = 0; i < NUM_CANV; ++i )
    {
      transparent[i] = NULL;
    }
  for (int i = 0; i < NUM_PAD; ++i )
    {
      Pad[i] = NULL;
    }

  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  for (int i = 0;i < NUM_GRAPH;i++)
    {
      gr[i] = NULL;
    }
  dbvars = new OnlMonDB(ThisName);
  label0 = NULL;
  sysmdt = NULL;
  sxsmdt = NULL;
  nysmdt = NULL;
  nxsmdt = NULL;
  ls1t = NULL;
  ls2t = NULL;
  ls3t = NULL;
  ln1t = NULL;
  ln2t = NULL;
  ln3t = NULL;
  ls1bt = NULL;
  ls2bt = NULL;
  ls3bt = NULL;
  ln1bt = NULL;
  ln2bt = NULL;
  ln3bt = NULL;
  zvb_prof = NULL;

  return ;
}

ZdcMonDraw::~ZdcMonDraw()
{
  delete zdcStyle;
  delete label0;

  for (int i = 0;i < NUM_GRAPH;i++)
    {
      delete gr[i];
    }
  for (int i = 0; i < 20; i++)
    {
      delete line[i];
    }
  delete arc[0];
  delete arc[1];
  delete [] par;
  delete sysmdt;
  delete sxsmdt;
  delete nysmdt;
  delete nxsmdt;
  delete ls1t;
  delete ls2t;
  delete ls3t;
  delete ln1t;
  delete ln2t;
  delete ln3t;
  delete ls1bt;
  delete ls2bt;
  delete ls3bt;
  delete ln1bt;
  delete ln2bt;
  delete ln3bt;
  delete zvb_prof;
  delete dbvars;
}

void ZdcMonDraw::InitLine(TLine *line, int lcolor, int lwidth, int lstyle)
{
  //defining the attributes of a line
  line->SetLineColor(lcolor);
  line->SetLineWidth(lwidth);
  line->SetLineStyle(lstyle);
}

int ZdcMonDraw::Init() //initialising, called by framework, do not make canvases
{
  //OnlMonClient *cl = OnlMonClient::instance();
  //long int RunNumber = cl->RunNumber();

  //making lines
  /*
  line[0] = new TLine(50., -1., 50., 1000.);   // for run8  200 GeV d+Au
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(200., -1., 200., 1000.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(1000., -1., 1000., 1000.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(2000., -1., 2000., 1000.);
  InitLine(line[3], 2, 1, 2);
  */
  /*
  line[0] = new TLine(1600., -1., 1600., 20.);   // for run4/7/10  200 GeV Au+Au
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(2000., -1., 2000., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(1600., -1., 1600., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(2000., -1., 2000., 20.);
  InitLine(line[3], 2, 1, 2);
*/
  
   line[0] = new TLine(1400., -1., 1400., 20.);   // for run11  200 GeV Au+Au, run12 UU, run14 200 GeV Au+Au, run16 200 GeV Au+Au
   InitLine(line[0], 2, 1, 2);
   line[1] = new TLine(2000., -1., 2000., 20.);
   InitLine(line[1], 2, 1, 2);
   line[2] = new TLine(1400., -1., 1400., 20.);
   InitLine(line[2], 2, 1, 2);
   line[3] = new TLine(2000., -1., 2000., 20.);
   InitLine(line[3], 2, 1, 2);
  
  /*
  line[0] = new TLine(800., -1., 800., 20.);   // for run10  62 GeV Au+Au
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(1800., -1., 1800., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(800., -1., 800., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(1800., -1., 1800., 20.);
  InitLine(line[3], 2, 1, 2);
  */
  /* 
  line[0] = new TLine(50., -1., 50., 20.);  // for 62.4 GeV p+p
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(50., -1., 50., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(50., -1., 50., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(50., -1., 50., 20.);
  InitLine(line[3], 2, 1, 2);
  */
  /*     
  line[0] = new TLine(300., -1., 300., 20.);  // for run12 200 GeV p+p, run15 200GeV pp
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(300., -1., 300., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(300., -1., 300., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(300., -1., 300., 20.);
  InitLine(line[3], 2, 1, 2);
  */

  /*
  line[0] = new TLine(150., -1., 150., 20.);  // for run13 500 GeV p+p
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(250., -1., 250., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(150., -1., 150., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(250., -1., 250., 20.);
  InitLine(line[3], 2, 1, 2);
  */

  /*
  line[0] = new TLine(250., -1., 250., 20.);  // for 500 GeV p+p
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(250., -1., 250., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(250., -1., 250., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(250., -1., 250., 20.);
  */
  /*
  line[0] = new TLine(50., -1., 50., 200.);  // for run5 200 GeV Cu+Cu
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(150., -1., 150., 200.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(50., -1., 50., 200.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(150., -1., 150., 200.);
  InitLine(line[3], 2, 1, 2);
  *//*
  line[0] = new TLine(22., -1., 22., 200.);  // for run5 62.4 GeV Cu+Cu
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(42., -1., 42., 200.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(22., -1., 22., 200.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(42., -1., 42., 200.);
  InitLine(line[3], 2, 1, 2);
  */
  /*
  line[0] = new TLine(300., -1., 300., 20.);  // for run15 200GeV pAu
  InitLine(line[0], 2, 1, 2);
  line[1] = new TLine(300., -1., 300., 20.);
  InitLine(line[1], 2, 1, 2);
  line[2] = new TLine(1400., -1., 1400., 20.);
  InitLine(line[2], 2, 1, 2);
  line[3] = new TLine(2000., -1., 2000., 20.);
  InitLine(line[3], 2, 1, 2);
  */
  line[4] = new TLine(0., -0.135, 200000., -0.135);
  InitLine(line[4], 8, 1, 1);
  line[5] = new TLine(0., 0.455, 200000., 0.455);
  InitLine(line[5], 8, 1, 1);
  line[6] = new TLine(0., -0.695, 200000., -0.695);
  InitLine(line[6], 8, 1, 1);
  line[7] = new TLine(0., -0.425, 200000., -0.425);
  InitLine(line[7], 8, 1, 1);

  line[8] = new TLine(0.0, 530, 200000., 530);
  InitLine(line[8], 8, 1, 1);
  line[9] = new TLine(0.0, 780, 200000., 780);
  InitLine(line[9], 8, 1, 1);
  line[10] = new TLine(0., 950, 200000., 950);
  InitLine(line[10], 8, 1, 1);
  line[11] = new TLine(0., 560, 200000., 560);
  InitLine(line[11], 8, 1, 1);
  line[12] = new TLine(0., 850, 200000., 850);
  InitLine(line[12], 8, 1, 1);
  line[13] = new TLine(0., 720, 200000., 720);
  InitLine(line[13], 8, 1, 1);

  line[14] = new TLine(0., 3400, 200000., 3400);
  InitLine(line[14], 8, 1, 1);
  line[15] = new TLine(0., 2800, 200000., 2800);
  InitLine(line[15], 8, 1, 1);
  line[16] = new TLine(0., 2500, 200000., 2500);
  InitLine(line[16], 8, 1, 1);
  line[17] = new TLine(0., 3400, 200000., 3400);
  InitLine(line[17], 8, 1, 1);
  line[18] = new TLine(0., 2500, 200000., 2500);
  InitLine(line[18], 8, 1, 1);
  line[19] = new TLine(0., 2800, 200000., 2800);
  InitLine(line[19], 8, 1, 1);

  arc[0] = new TArc(0., 0., 2.);
  arc[0]->SetLineColor(2);
  arc[0]->SetLineWidth(2);
  arc[0]->SetFillStyle(4000);
  arc[1] = new TArc(0.2, -0.5, 2.);  // run16 AuAu200
  // arc[1] = new TArc(-2.5, 0., 2.); //run15pAu200
  arc[1]->SetLineColor(2);
  arc[1]->SetLineWidth(2);
  arc[1]->SetFillStyle(4000);
  return 0;
}


void ZdcMonDraw::FitGauss(TH1 * histo, double xmin, double xmax)
{
  cout << "Fitting gaussian..." << endl;
  TF1 *fitfunc = new TF1("fitfunc", "gaus", xmin, xmax);
  histo->Fit("fitfunc", "RN");
  fitfunc->GetParameters(par);
  delete fitfunc;
  cout << "Gaussian fitted." << endl;
}

void ZdcMonDraw::FitLinear(TH1 * histo, double xmin, double xmax)
{
  cout << "Fitting linear..." << endl;
  TF1 *fitfunc = new TF1("fitfunc", "pol1", xmin, xmax);
  histo->Fit("fitfunc", "RN");
  fitfunc->GetParameters(par);
  perr = fitfunc->GetParError(1);
  delete fitfunc;
  cout << "Linear fitted." << endl;
}

double Gauss2D(double *x, double *params) //not used currently
{
  double temp;
  temp =
    (sqrt(params[0] / M_PI) * exp( -(x[0] - params[1] * (x[0] - params[1]) / (2 * params[0] * params[0]))) + params[2]) *
    (sqrt(params[3] / M_PI) * exp( -(x[1] - params[4] * (x[1] - params[4]) / (2 * params[3] * params[3]))) + params[5]);
  return temp;
}

void ZdcMonDraw::Fit2DGauss(TH2 * histo, double xmin, double xmax, double ymin, double ymax)
{
  cout << "Fitting 2D gaussian..." << endl;
  TF2 *fitfunc = new TF2("fitfunc", Gauss2D, xmin, xmax, ymin, ymax, 6);
  cout << "TF2 created." << endl;
  histo->Fit("fitfunc", "R0");
  cout << "Getting parameters..." << endl;
  fitfunc->GetParameters(par);
  delete fitfunc;
  cout << "2D gaussian fitted." << endl;
  //not used currently
}

void ZdcMonDraw::Draw0thPad(TH2 * histo, int npad, int ncanvas)
  //drawing the title pad
{
  if (!label0)
    {
      label0 = new TPaveText(0.20, 0.55, 0.80, 1.0);
      label0->SetTextAlign(23);
      label0->AddText("ZDC ONLINE MONITOR");
    }
  TText text0;
  text0.SetTextSize(0.25);
  OnlMonClient *client = OnlMonClient::instance();
  TC[ncanvas]->cd();
  ostringstream otext;
  otext.str("");
  if ( histo )
    {
      otext << "   Run " << client->RunNumber() << ", ";               // a quick hack - if any cuts are applied to the zdc histos, the
      otext << (histo->GetEntries() / 40) << " events processed ";     // # of events processed != the histo contents
      time_t evttime = client->EventTime("CURRENT");
      otext << " Date:" << ctime(&evttime) ;
    }

  Pad[npad]->cd();
  Pad[npad]->Clear();
  label0->Draw();
  text0.DrawText(0.0 , 0.25, otext.str().c_str());
  transparent[ncanvas]->Clear();
}

void ZdcMonDraw::DrawPaveText(const char * zdc_text, int npad)
  //draw a pavetext
{
  cout << "Drawing text..." << endl;
  Pad[npad]->Clear();
  Pad[npad]->cd();
  TLatex TL;
  TL.SetTextSize(0.5);
  TL.DrawLatex(0.01, 0.02, zdc_text);
  cout << "Text drawn." << endl;
}

void ZdcMonDraw::DrawProfStat(TPaveText *ptext, TH2 *histo,
                              const char * unit, int npad, char type)
  //draw the statistics pad used by canvases 3,4,5
{
  float mean = 0.;
  float rms = 0.;
  float nent = 1.;
  int pr[4];
  for (int i=0;i<4;i++)
    {
      pr[i] = 4;
    }
  //setting precisions of numbers
  if (type == 'a')
    {
      pr[0] = 4;
      pr[1] = 2;
      pr[2] = 3;
    }
  if (type == 'b')
    {
      pr[0] = 4;
      pr[1] = 2;
      pr[2] = 3;
    }
  if (histo)
    {
      mean = histo->GetMean(2);
      rms = histo->GetRMS(2);
      nent = histo->GetEntries();
    }
  Pad[npad]->cd();
/*
  if (!ptext)
    {
      ptext = new TPaveText(0.01, 0.02, 1.0, 1.0, "brNDC");
    }
  ptext->SetBorderSize(1);
  ptext->SetFillColor(18);
  ptext->SetTextColor(1);
  ptext->SetTextFont(1);
  ptext->SetTextAlign(13);
  ptext->Clear();
*/
  //writing the texts
  TLatex TL;
  TL.SetTextSize(0.3);
  ostringstream stext;
  stext.str("");
  stext << "Average = (" << setprecision(pr[0]) << mean << " +/- "
	<< setprecision(pr[1]) << rms / sqrt(nent) << ") \n" << unit;
  TL.DrawLatex(0.01, 0.52, stext.str().c_str());
  stext.str("");
  stext << "RMS = " << setprecision(pr[2]) << rms << unit
	<< setprecision(6) << ", # of entries = " << nent;
  TL.DrawLatex(0.01, 0.02, stext.str().c_str());

//  ptext->AddText(stext.str().c_str());
//  ptext->Draw();
}

void ZdcMonDraw::DrawHisto(TH1 *histo, int npad, bool logy, int lcolor,
                           const char * xtit, const char * ytit, const char * dr_opt = "ALL")
  //drawing a histogram with options named above and below
{
  Pad[npad]->cd();
  if (histo)
    {
      if (logy)
        {
          Pad[npad]->SetLogy();
        }
      histo->GetXaxis()->SetTitle(xtit);
      histo->GetYaxis()->SetTitle(ytit);
      histo->SetLineColor(lcolor);
      if(strcmp(dr_opt,"colz")==0)
	{
	  Pad[npad]->SetLogz();
	  histo->SetMinimum(1);
	}
      histo->Draw(dr_opt);
    }
}

void ZdcMonDraw::DrawHisto2D(TH2 *histo, int npad, int logz, const char * xtit, const char * ytit, const char * dr_opt = "ALL")
  //drawing a histogram with options named above and below
{
  Pad[npad]->cd();
  if (histo)
    {
      Pad[npad]->SetLogz(logz);
     
      histo->GetXaxis()->SetTitle(xtit);
      histo->GetYaxis()->SetTitle(ytit);
     
      if(strcmp(dr_opt,"colz")==0)
	{
	  Pad[npad]->SetLogz();
	}
      histo->Draw(dr_opt);
    }
}

void ZdcMonDraw::DrawProfile(TProfile *profile, int npad, bool logy, int lcolor,
                             const char * xtit, const char * ytit, int i_range)
  //drawing a profile histogrgram with options named above and below
{
  Pad[npad]->cd();
  if (logy)
    {
      Pad[npad]->SetLogy();
    }
  profile->GetXaxis()->SetTitle(xtit);
  profile->GetXaxis()->SetRange(0, i_range);
  profile->GetYaxis()->SetTitle(ytit);
  profile->SetLineColor(lcolor);
  profile->Draw();
}

void ZdcMonDraw::DrawHistoSame(TH1 *histo, int lcolor)
  //drawing a histogram in the pad that was activated at last, not overwriting the last histo.
{
  if (histo)
    {
      histo->SetLineColor(lcolor);
      histo->Draw("SAME");
    }
}

void ZdcMonDraw::DrawSmdValue(int nsmd, int npad)
  //draw the pads used by canvases 6 and 7, not used currently
{
  ostringstream name;
  name.str("");
  name << "smd_histo_" << nsmd + 1;
  TH1D * smd_histo = smd_value->ProjectionX(name.str().c_str(), nsmd + 1, nsmd + 1);
  name.str("");
  name << "smd_histo_good_" << nsmd + 1;
  TH1D * smd_histo_good = smd_value_good->ProjectionX(name.str().c_str(), nsmd + 1, nsmd + 1);
  name.str("");
  name << "smd_histo_small_" << nsmd + 1;
  TH1D * smd_histo_small = smd_value_small->ProjectionX(name.str().c_str(), nsmd + 1, nsmd + 1);
  const char * zero = "";
  const char * title_GeV = "Energy [GeV]";
  DrawHisto(smd_histo, npad, true, 4, title_GeV, zero, zero);
  DrawHistoSame(smd_histo_good, 1);
  DrawHistoSame(smd_histo_small, 2);
}

int ZdcMonDraw::MakeCanvas1(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  if (! gROOT->FindObject(name) )
    {
      TC[0] = new TCanvas(name, "Zdc Main Monitor", -1, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      TC[0] -> SetFillColor(0);
    }

  TC[0]->cd();
  cout << "Creating first canvas..." << endl;
  Pad[0] = new TPad("zdc_titlepad1", "zdc_titlepad1", 0.00, 0.92, 1.00, 1.00, 0);
  Pad[1] = new TPad("nhit_north", "zdc_nhit_north", 0.00, 0.65, 0.50, 0.92, 0);
  Pad[2] = new TPad("nhit_south", "zdc_nhit_south", 0.50, 0.65, 1.00, 0.92, 0);
  Pad[3] = new TPad("north_text", "zdc_north_text", 0.00, 0.62, 0.50, 0.65, 0);
  Pad[4] = new TPad("south_text", "zdc_north_text", 0.50, 0.62, 1.00, 0.65, 0);
  Pad[5] = new TPad("zdc_vertex", "zdc_vertex", 0.00, 0.35, 0.50, 0.62, 0);
  Pad[6] = new TPad("zdc_vertex_b", "zdc_vertex_b", 0.50, 0.35, 1.00, 0.62, 0);
  Pad[7] = new TPad("zdc_vertex_text1", "zdc_vertex_text1", 0.00, 0.31, 1.00, 0.35, 0);
  Pad[8] = new TPad("smd_xy_north", "xy_north", 0.00, 0.04, 0.50, 0.31, 0);
  Pad[9] = new TPad("smd_xy_south", "xy_south", 0.50, 0.04, 1.00, 0.31, 0);
  Pad[10] = new TPad("smd_north_pos", "smd_north_pos", 0.00, 0.00, 0.50, 0.04, 0);
  Pad[11] = new TPad("smd_south_pos", "smd_south_pos", 0.50, 0.00, 1.00, 0.04, 0);
  cout << "Creating pads of first canvas..." << endl;
  Pad[0]->Draw();
  Pad[1]->Draw();
  Pad[2]->Draw();
  Pad[3]->Draw();
  Pad[4]->Draw();
  Pad[5]->Draw();
  Pad[6]->Draw();
  Pad[7]->Draw();
  Pad[8]->Draw();
  Pad[9]->Draw();
  Pad[10]->Draw();
  Pad[11]->Draw();
  cout << "Pads of first canvas created." << endl;

  transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
  transparent[0]->SetFillStyle(4000);
  transparent[0]->Draw();
  cout << "First canvas created." << endl;
  return 0;
}

int ZdcMonDraw::MakeCanvas2(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[1] = new TCanvas(name, "Smd expert data", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[1]->cd();
  cout << "Creating second canvas..." << endl;
  Pad[12] = new TPad("zdc_titlepad2", "zdc_titlepad2", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[13] = new TPad("smd_ver_north", "smd_ver_north", 0.00, 0.65, 0.25, 0.92, 0);
  Pad[14] = new TPad("smd_ver_south", "smd_ver_south", 0.25, 0.65, 0.50, 0.92, 0);
  Pad[15] = new TPad("smd_hor_north", "smd_hor_north", 0.50, 0.65, 0.75, 0.92, 0);
  Pad[16] = new TPad("smd_hor_south", "smd_hor_south", 0.75, 0.65, 1.00, 0.92, 0);
  Pad[17] = new TPad("hor_north", "hor_north", 0.00, 0.38, 0.25, 0.65, 0);
  Pad[18] = new TPad("ver_north", "ver_north", 0.25, 0.38, 0.50, 0.65, 0);
  Pad[19] = new TPad("sum_ver_north", "sum_ver_north", 0.50, 0.38, 0.75, 0.65, 0);
  Pad[20] = new TPad("sum_ver_south", "sum_ver_south", 0.75, 0.38, 1.00, 0.65, 0);
  Pad[21] = new TPad("sum_hor_north", "sum_hor_north", 0.00, 0.11, 0.25, 0.38, 0);
  Pad[22] = new TPad("sum_hor_south", "sum_hor_south", 0.25, 0.11, 0.50, 0.38, 0);
  cout << "Creating pads of second canvas..." << endl;
  Pad[12]->Draw();
  Pad[13]->Draw();
  Pad[14]->Draw();
  Pad[15]->Draw();
  Pad[16]->Draw();
  Pad[17]->Draw();
  Pad[18]->Draw();
  Pad[19]->Draw();
  Pad[20]->Draw();
  Pad[21]->Draw();
  Pad[22]->Draw();
  cout << "Pads of second canvas created." << endl;
  transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
  transparent[1]->SetFillStyle(4000);
  transparent[1]->Draw();
  cout << "Second canvas created." << endl;
  return 0;
}


int ZdcMonDraw::MakeCanvas3(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[2] = new TCanvas(name, "Smd positon values", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[2]->cd();
  cout << "Creating third canvas..." << endl;
  Pad[23] = new TPad("zdc_titlepad3", "zdc_titlepad3", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[24] = new TPad("smd_yt_south_c", "smd_yt_south", 0.00, 0.50, 0.50, 0.92, 0);
  Pad[25] = new TPad("smd_yn_text", "smd_yn_text", 0.00, 0.46, 0.50, 0.50, 0);
  Pad[26] = new TPad("smd_xt_south_c", "smd_xt_south", 0.50, 0.50, 1.00, 0.92, 0);
  Pad[27] = new TPad("smd_yn_text", "smd_yn_text", 0.50, 0.46, 1.00, 0.50, 0);
  Pad[28] = new TPad("smd_yt_north_c", "smd_yt_south", 0.00, 0.04, 0.50, 0.46, 0);
  Pad[29] = new TPad("smd_yn_text", "smd_yn_text" , 0.00, 0.00, 0.50, 0.04, 0);
  Pad[30] = new TPad("smd_xt_north_c", "smd_xt_south", 0.50, 0.04, 1.00, 0.46, 0);
  Pad[31] = new TPad("smd_yn_text", "smd_yn_text", 0.50, 0.00, 1.00, 0.04, 0);

  cout << "Creating pads of third canvas..." << endl;
  Pad[23]->Draw();
  Pad[24]->Draw();
  Pad[25]->Draw();
  Pad[26]->Draw();
  Pad[27]->Draw();
  Pad[28]->Draw();
  Pad[29]->Draw();
  Pad[30]->Draw();
  Pad[31]->Draw();

  cout << "Pads of third canvas created." << endl;
  transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
  transparent[2]->SetFillStyle(4000);
  transparent[2]->Draw();
  cout << "Third canvas created." << endl;
  return 0;
}

int ZdcMonDraw::MakeCanvas4(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[3] = new TCanvas(name, "Led energy values", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[3]->cd();
  cout << "Creating fourth canvas..." << endl;
  Pad[32] = new TPad("zdc_titlepad4", "zdc_titlepad4", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[33] = new TPad("zdc_laser_south1", "zdc_laser_south1", 0.00, 0.65, 0.50, 0.92, 0);
  Pad[34] = new TPad("zdc_ls1_text", "zdc_ls1_text", 0.00, 0.6133, 0.50, 0.65, 0);
  Pad[35] = new TPad("zdc_laser_north1", "zdc_laser_north1", 0.50, 0.65, 1.00, 0.92, 0);
  Pad[36] = new TPad("zdc_ls2_text", "zdc_ls2_text", 0.50, 0.6133, 1.00, 0.65, 0);
  Pad[37] = new TPad("zdc_laser_south2", "zdc_laser_south2", 0.00, 0.3433, 0.50, 0.6133, 0);
  Pad[38] = new TPad("zdc_ls3_text", "zdc_ls3_text", 0.00, 0.3067, 0.50, 0.3433, 0);
  Pad[39] = new TPad("zdc_laser_north2", "zdc_laser_north2", 0.50, 0.3433, 1.00, 0.6133, 0);
  Pad[40] = new TPad("zdc_ln1_text", "zdc_ln1_text", 0.50, 0.3067, 1.00, 0.3433, 0);
  Pad[41] = new TPad("zdc_laser_south3", "zdc_laser_south3", 0.00, 0.0367, 0.50, 0.3067, 0);
  Pad[42] = new TPad("zdc_ln2_text", "zdc_ln2_text", 0.00, 0.00, 0.50, 0.0367, 0);
  Pad[43] = new TPad("zdc_laser_north3", "zdc_laser_north3", 0.50, 0.0367, 1.00, 0.3067, 0);
  Pad[44] = new TPad("zdc_ln3_text", "zdc_ln3_text", 0.50, 0.00, 1.00, 0.0367, 0);

  cout << "Creating pads of fourth canvas..." << endl;
  Pad[32]->Draw();
  Pad[33]->Draw();
  Pad[34]->Draw();
  Pad[35]->Draw();
  Pad[36]->Draw();
  Pad[37]->Draw();
  Pad[38]->Draw();
  Pad[39]->Draw();
  Pad[40]->Draw();
  Pad[41]->Draw();
  Pad[42]->Draw();
  Pad[43]->Draw();
  Pad[44]->Draw();

  cout << "Pads of fourth canvas created." << endl;
  transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
  transparent[3]->SetFillStyle(4000);
  transparent[3]->Draw();
  cout << "Fourth canvas created." << endl;
  return 0;
}

int ZdcMonDraw::MakeCanvas5(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[4] = new TCanvas(name, "Led timing values", -xsize / 2, 0, xsize / 2, ysize );
      gSystem->ProcessEvents();
    }
  TC[4]->cd();
  cout << "Creating fifth canvas..." << endl;
  Pad[45] = new TPad("zdc_titlepad5", "zdc_titlepad5", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[46] = new TPad("zdc_laser_south1b", "zdc_laser_south1b", 0.00, 0.65, 0.50, 0.92, 0);
  Pad[47] = new TPad("zdc_ls1b_text", "zdc_ls1b_text", 0.00, 0.6133, 0.50, 0.65, 0);
  Pad[48] = new TPad("zdc_laser_north1b", "zdc_laser_north1b", 0.50, 0.65, 1.00, 0.92, 0);
  Pad[49] = new TPad("zdc_ls2b_text", "zdc_ls2b_text", 0.50, 0.6133, 1.00, 0.65, 0);
  Pad[50] = new TPad("zdc_laser_south2b", "zdc_laser_south2b", 0.00, 0.3433, 0.50, 0.6133, 0);
  Pad[51] = new TPad("zdc_ls3b_text", "zdc_ls3b_text", 0.00, 0.3067, 0.50, 0.3433, 0);
  Pad[52] = new TPad("zdc_laser_north2b", "zdc_laser_north2b", 0.50, 0.3433, 1.00, 0.6133, 0);
  Pad[53] = new TPad("zdc_ln1b_text", "zdc_ln1b_text", 0.50, 0.3067, 1.00, 0.3433, 0);
  Pad[54] = new TPad("zdc_laser_south3b", "zdc_laser_south3b", 0.00, 0.0367, 0.50, 0.3067, 0);
  Pad[55] = new TPad("zdc_ln2b_text", "zdc_ln2b_text", 0.00, 0.00, 0.50, 0.0367, 0);
  Pad[56] = new TPad("zdc_laser_north3b", "zdc_laser_north3b", 0.50, 0.0367, 1.00, 0.3067, 0);
  Pad[57] = new TPad("zdc_ln3b_text", "zdc_ln3b_text", 0.50, 0.00, 1.00, 0.0367, 0);

  cout << "Creating pads of fifth canvas..." << endl;
  Pad[45]->Draw();
  Pad[46]->Draw();
  Pad[47]->Draw();
  Pad[48]->Draw();
  Pad[49]->Draw();
  Pad[50]->Draw();
  Pad[51]->Draw();
  Pad[52]->Draw();
  Pad[53]->Draw();
  Pad[54]->Draw();
  Pad[55]->Draw();
  Pad[56]->Draw();
  Pad[57]->Draw();

  cout << "Pads of fifth canvas created." << endl;
  transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
  transparent[4]->SetFillStyle(4000);
  transparent[4]->Draw();
  cout << "Fifth canvas created." << endl;
  return 0;
}

int ZdcMonDraw::MakeCanvas6(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[5] = new TCanvas(name, "Smd south values", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[5]->cd();
  cout << "Creating sixt canvas..." << endl;
  Pad[58] = new TPad("zdc_titlepad6", "zdc_titlepad6", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[59] = new TPad("smd_value_s1", "smd_value_s1", 0.00, 0.69, 0.50, 0.92, 0);
  Pad[60] = new TPad("smd_value_s2", "smd_value_s2", 0.50, 0.69, 1.00, 0.92, 0);
  Pad[61] = new TPad("smd_value_s3", "smd_value_s3", 0.00, 0.46, 0.50, 0.69, 0);
  Pad[62] = new TPad("smd_value_s4", "smd_value_s4", 0.50, 0.46, 1.00, 0.69, 0);
  Pad[63] = new TPad("smd_value_s5", "smd_value_s5", 0.00, 0.23, 0.50, 0.46, 0);
  Pad[64] = new TPad("smd_value_s6", "smd_value_s6", 0.50, 0.23, 1.00, 0.46, 0);
  Pad[65] = new TPad("smd_value_s7", "smd_value_s7", 0.00, 0.00, 0.50, 0.23, 0);
  Pad[66] = new TPad("smd_value_s8", "smd_value_s8", 0.50, 0.00, 1.00, 0.23, 0);

  cout << "Creating pads of sixth canvas..." << endl;
  Pad[58]->Draw();
  Pad[59]->Draw();
  Pad[60]->Draw();
  Pad[61]->Draw();
  Pad[62]->Draw();
  Pad[63]->Draw();
  Pad[64]->Draw();
  Pad[65]->Draw();
  Pad[66]->Draw();

  cout << "Pads of sixth canvas created." << endl;
  transparent[5] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
  transparent[5]->SetFillStyle(4000);
  transparent[5]->Draw();
  cout << "Sixth canvas created." << endl;
  return 0;
}

int ZdcMonDraw::MakeCanvas7(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[6] = new TCanvas(name, "Smd north values", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[6]->cd();
  cout << "Creating seventh canvas..." << endl;
  Pad[67] = new TPad("zdc_titlepad7", "zdc_titlepad7", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[68] = new TPad("smd_value_n1", "smd_value_n1", 0.00, 0.69, 0.50, 0.92, 0);
  Pad[69] = new TPad("smd_value_n2", "smd_value_n2", 0.50, 0.69, 1.00, 0.92, 0);
  Pad[70] = new TPad("smd_value_n3", "smd_value_n3", 0.00, 0.46, 0.50, 0.69, 0);
  Pad[71] = new TPad("smd_value_n4", "smd_value_n4", 0.50, 0.46, 1.00, 0.69, 0);
  Pad[72] = new TPad("smd_value_n5", "smd_value_n5", 0.00, 0.23, 0.50, 0.46, 0);
  Pad[73] = new TPad("smd_value_n6", "smd_value_n6", 0.50, 0.23, 1.00, 0.46, 0);
  Pad[74] = new TPad("smd_value_n7", "smd_value_n7", 0.00, 0.00, 0.50, 0.23, 0);
  Pad[75] = new TPad("smd_value_n8", "smd_value_n8", 0.50, 0.00, 1.00, 0.23, 0);

  cout << "Creating pads of seventh canvas..." << endl;
  Pad[67]->Draw();
  Pad[68]->Draw();
  Pad[69]->Draw();
  Pad[70]->Draw();
  Pad[71]->Draw();
  Pad[72]->Draw();
  Pad[73]->Draw();
  Pad[74]->Draw();
  Pad[75]->Draw();

  cout << "Pads of seventh canvas created." << endl;
  transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
  transparent[6]->SetFillStyle(4000);
  transparent[6]->Draw();
  cout << "Seventh canvas created." << endl;
  return 0;
}

int ZdcMonDraw::MakeCanvas8(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[7] = new TCanvas(name, "Zdc ratio values", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[7]->cd();
  cout << "Creating eight' canvas..." << endl;
  Pad[76] = new TPad("zdc_titlepad8", "zdc_titlepad8", 0.00, 0.92, 1.00, 1.0, 0);
  Pad[77] = new TPad("zdc_ratio_south", "zdc_ratio_south", 0.00, 0.48, 1.00, 0.92, 0);
  Pad[78] = new TPad("zdc_ratio_north", "zdc_ratio_north", 0.00, 0.01, 1.00, 0.47, 0);

  cout << "Creating pads of eight' canvas..." << endl;
  Pad[76]->Draw();
  Pad[77]->Draw();
  Pad[78]->Draw();

  cout << "Pads of eight' canvas created." << endl;
  transparent[7] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
  transparent[7]->SetFillStyle(4000);
  transparent[7]->Draw();
  cout << "Eight' canvas created." << endl;

  return 0;
}

int ZdcMonDraw::MakeCanvas9(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[8] = new TCanvas(name, "ZDC versus BBC vertex", -xsize / 2, 0, xsize / 2, ysize );
      gSystem->ProcessEvents();
    }
  TC[8]->cd();
  cout << "Creating ninth canvas..." << endl;
  Pad[79] = new TPad("zdc_titlepad9", "zdc_titlepad9", 0.00, 0.92, 1.00, 1.00, 0);
  Pad[80] = new TPad("zdc_vertx_bbc", "zdc_vertx_bbc", 0.00, 0.54, 1.00, 0.92, 0);
  Pad[81] = new TPad("zdc_vrtx_text", "zdc_vrtx_text", 0.00, 0.50, 1.00, 0.54, 0);
  /*Ciprian Gal 110211*/
  for(int i=0;i<3;i++)
    {
      char hname[256],title[256];
      sprintf(hname,"zdc_adc_s_ind_pad%d",i);
      sprintf(title,"zdc_adc_s_ind_%d",i);
      Pad[137+i] = new TPad(hname, title, 0.33*i, 0.25, 0.33*(i+1), 0.50, 0);

      sprintf(hname,"zdc_adc_n_ind_pad%d",i);
      sprintf(title,"zdc_adc_n_ind_%d",i);
      Pad[140+i] = new TPad(hname, title, 0.33*i, 0.00,0.33*(i+1), 0.25, 0);
    }
  /*Ciprian Gal 110211*/

  cout << "Creating pads of ninth canvas..." << endl;
  Pad[79]->Draw();
  Pad[80]->Draw();
  Pad[81]->Draw();
  for(int i=0;i<6;i++)
    {
      Pad[137+i]->Draw();
    }
  cout << "Pads of ninth canvas created." << endl;
  transparent[8] = new TPad("transparent8", "this does not show", 0, 0, 1, 1);
  transparent[8]->SetFillStyle(4000);
  transparent[8]->Draw();
  cout << "Ninth canvas created." << endl;

  return 0;
}

int ZdcMonDraw::MakeCanvas16(const char *name)
{ // ADC vs TDC

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[15] = new TCanvas(name, "ADC versus TDC", -xsize, 0, xsize, ysize );
      gSystem->ProcessEvents();
    }
  TC[15]->cd();
  cout << "Creating 16th canvas..." << endl;

  Pad[143] = new TPad("zdc_titlepad16", "zdc_titlepad16", 0.25, 0.92, 0.75, 1.00, 0); 

  for(int i=0;i<40;i++)
    {
      char hname[256],title[256];
      sprintf(hname,"zdc_adc_vs_tdc_pad%d",i);
      sprintf(title,"zdc_adc_vs_tdc_pad_%d",i);
      Pad[144+i] = new TPad(hname, title, (i%8)*0.125, 0.92-(0.92/5.0)*(i/8+1), (i%8+1)*0.125, 0.92-(0.92/5.0)*(i/8), 0);
    }

  cout << "Creating pads of 16th canvas..." << endl;
  for(int i=-1;i<40;i++)
    Pad[144+i]->Draw();

  cout << "Pads of 16th canvas created." << endl;
  transparent[15] = new TPad("transparent16", "this does not show", 0, 0, 1, 1);
  transparent[15]->SetFillStyle(4000);
  transparent[15]->Draw();
  cout << "16th canvas created." << endl;

  return 0;
}

int ZdcMonDraw::MakeCanvas17(const char *name)
{
  int ncanv = 16;
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[ncanv] = new TCanvas(name, "Front Veto ADC versus ZDC SMD TDC", -xsize, 0, xsize, ysize );
      gSystem->ProcessEvents();
    }
  TC[ncanv]->cd();
  cout << "Creating "<<ncanv+1<<"th canvas..." << endl;

  Pad[184] = new TPad("zdc_titlepad17", "zdc_titlepad17", 0.25, 0.92, 0.75, 1.00, 0); 

  for(int i=0;i<40;i++)
    {
      char hname[256],title[256];
      sprintf(hname,"fveto_adc_vs_zdctdc_pad%d",i);
      sprintf(title,"fveto_adc_vs_zdctdc_pad_%d",i);
      Pad[185+i] = new TPad(hname, title, (i%8)*0.125, 0.92-(0.92/5.0)*(i/8+1), (i%8+1)*0.125, 0.92-(0.92/5.0)*(i/8), 0);
    }

  cout << "Creating pads of "<<ncanv+1<<"th canvas..." << endl;
  for(int i=-1;i<40;i++)
    Pad[185+i]->Draw();

  cout << "Pads of "<<ncanv+1<<"th canvas created." << endl;
  transparent[ncanv] = new TPad("transparent17", "this does not show", 0, 0, 1, 1);
  transparent[ncanv]->SetFillStyle(4000);
  transparent[ncanv]->Draw();
  cout << ncanv+1<<"th canvas created." << endl;

  return 0;
}

int ZdcMonDraw::MakeCanvas18(const char *name)
{
  int ncanv = 17;
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[ncanv] = new TCanvas(name, "Rear Veto ADC versus ZDC SMD TDC", -xsize, 0, xsize, ysize );
      gSystem->ProcessEvents();
    }
  TC[ncanv]->cd();
  cout << "Creating "<<ncanv+1<<"th canvas..." << endl;

  Pad[225] = new TPad("zdc_titlepad18", "zdc_titlepad18", 0.25, 0.92, 0.75, 1.00, 0); 

  for(int i=0;i<40;i++)
    {
      char hname[256],title[256];
      sprintf(hname,"rveto_adc_vs_zdctdc_pad%d",i);
      sprintf(title,"rveto_adc_vs_zdctdc_pad_%d",i);
      Pad[226+i] = new TPad(hname, title, (i%8)*0.125, 0.92-(0.92/5.0)*(i/8+1), (i%8+1)*0.125, 0.92-(0.92/5.0)*(i/8), 0);
    }

  cout << "Creating pads of "<<ncanv+1<<"th canvas..." << endl;
  for(int i=-1;i<40;i++)
    Pad[226+i]->Draw();

  cout << "Pads of "<<ncanv+1<<"th canvas created." << endl;
  transparent[ncanv] = new TPad("transparent18", "this does not show", 0, 0, 1, 1);
  transparent[ncanv]->SetFillStyle(4000);
  transparent[ncanv]->Draw();
  cout << ncanv+1<<"th canvas created." << endl;

  return 0;
}

int ZdcMonDraw::MakeCanvas19(const char *name)
{
  int ncanv = 18;
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (! gROOT->FindObject(name) )
    {
      TC[ncanv] = new TCanvas(name, "Veto ADC versus position", -xsize, 0, xsize, ysize );
      gSystem->ProcessEvents();
    }
  TC[ncanv]->cd();
  cout << "Creating "<<ncanv+1<<"th canvas..." << endl;

  Pad[266] = new TPad("zdc_titlepad19", "zdc_titlepad19", 0.25, 0.92, 0.75, 1.00, 0); 

  for(int i=0;i<8;i++)
    {
      char hname[256],title[256];
      sprintf(hname,"veto_adc_vs_pos_pad%d",i);
      sprintf(title,"veto_adc_vs_pos_pad_%d",i);
      Pad[267+i] = new TPad(hname, title, (i%4)*0.25, 0.92-(0.92/2.0)*(i/4+1), (i%4+1)*0.25, 0.92-(0.92/2.0)*(i/4), 0);
    }

  cout << "Creating pads of "<<ncanv+1<<"th canvas..." << endl;
  for(int i=-1;i<8;i++)
    Pad[267+i]->Draw();

  cout << "Pads of "<<ncanv+1<<"th canvas created." << endl;
  transparent[ncanv] = new TPad("transparent19", "this does not show", 0, 0, 1, 1);
  transparent[ncanv]->SetFillStyle(4000);
  transparent[ncanv]->Draw();
  cout << ncanv+1<<"th canvas created." << endl;

  return 0;
}


int ZdcMonDraw::MakeCanvasHistory(const char *name, const char *title, int Nhist)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  int Ncanv = Nhist + 9;
  if (! gROOT->FindObject(name) )
    {
      TC[Ncanv] = new TCanvas(name, title, -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
    }
  TC[Ncanv]->cd();
  int Npad0 = 82 + Nhist * 9;
  cout << "Creating history canvas..." << endl;
  ostringstream pname;
  pname.str("");
  pname << "zdc_titlepad" << Ncanv + 1;
  Pad[Npad0] = new TPad(pname.str().c_str(), pname.str().c_str(), 0.00, 0.92, 1.00, 1.00, 0);
  for (int i = 0;i < 8;i++)
    {
      int ix = i % 2; //to see if it's odd or even
      int iy = 3 - (i - i % 2) / 2; //to see, in which line (0,1,2,3) is it
      float px = ix * 0.5; //even pads: 0.00 ... 0.50, odd pads: 0.50 ... 1.00
      float py = iy * 0.23; //0th line: 0.69 ... 0.92
      pname.str("");
      pname << "zdc_ppghist_" << Nhist*8 + i;
      Pad[Npad0 + 1 + i] =
        new TPad(pname.str().c_str(), pname.str().c_str(), px, py, px + 0.5, py + 0.23);
    }
  cout << "Creating pads of history canvas..." << endl;
  Pad[Npad0]->Draw();
  for (int i = 0;i < 8;i++)
    {
      Pad[Npad0 + 1 + i]->Draw();
    }
  cout << "Pads of history canvas created." << endl;
  ostringstream trname;
  trname.str("");
  trname << "transparent" << Ncanv;
  transparent[Ncanv] = new TPad(trname.str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[Ncanv]->SetFillStyle(4000);
  transparent[Ncanv]->Draw();
  cout << "History canvas created." << endl;

  return 0;
}


int ZdcMonDraw::Draw(const char *what)
  //draw function, called by framework, must call all 'Draw's
{
  string that = string(what);
  int iret01 = 0, iret02 = 0, iret03 = 0, iret04 = 0, iret05 = 0, iret06 = 0, iret07 = 0;
  int iret08 = 0, iret09 = 0, iret10 = 0, iret11 = 0, iret12 = 0, iret13 = 0, iret14 = 0;
  int iret15 = 0, iret16 = 0, iret17 = 0, iret18 = 0, iret19 = 0;

  cout << " drawing offline shifter canvases " << endl;
  iret01 = Draw1();

  iret09 = Draw9();

  if ( (("HTML" == that) || ("VETO" == that)))
    {
      iret16 = Draw16();
      iret17 = Draw17();
      iret18 = Draw18();
      iret19 = Draw19();
    }

  if ( (("HTML" == that) || ("EXPERT" == that)))
    {
      cout << " drawing expert canvases for the record " << endl;

      iret10 = DrawHistory("Zdc PPG History", 0);
      iret11 = DrawHistory("Smd South Y PPG History", 1);
      iret12 = DrawHistory("Smd South X PPG History", 2);
      iret13 = DrawHistory("Smd North Y PPG History", 3);
      iret14 = DrawHistory("Smd North X PPG History", 4);
      iret15 = DrawHistory("Smd Position History", 5);
      if ( "HTML" == that )
        {
          iret02 = Draw2();
          iret03 = Draw3();
          iret04 = Draw4();
          iret05 = Draw5();
          iret06 = Draw6();
          iret07 = Draw7();
          iret08 = Draw8();
        }
    }
  
  if(iret01 || iret02 || iret03 || iret04 || iret05 || iret06 || iret07 || iret08 || 
     iret09 || iret10 || iret11 || iret12 || iret13 || iret14 || iret15 || iret16 || iret17 || iret18 || iret19  )
    return 1;
  return 0;
}


int ZdcMonDraw::Draw1()
{
  cout << "Drawing first canvas..." << endl;

  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  int iret = 0;

  // get pointer for each histogram
  TH1 *zdc_adc_north = cl->getHisto("zdc_adc_north");
  TH1 *zdc_adc_south = cl->getHisto("zdc_adc_south");
  TH1 *zdc_vertex = cl->getHisto("zdc_vertex");
  TH1 *zdc_vertex_b = cl->getHisto("zdc_vertex_b");
  TH2 *smd_xy_north = dynamic_cast <TH2*> (cl->getHisto("smd_xy_north"));
  TH2 *smd_xy_south = dynamic_cast <TH2*> (cl->getHisto("smd_xy_south"));

  TH2 *smd_y_s = dynamic_cast <TH2*> (cl->getHisto("smd_yt_south"));
  TH2 *smd_x_s = dynamic_cast <TH2*> (cl->getHisto("smd_xt_south"));
  TH2 *smd_y_n = dynamic_cast <TH2*> (cl->getHisto("smd_yt_north"));
  TH2 *smd_x_n = dynamic_cast <TH2*> (cl->getHisto("smd_xt_north"));
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));

  //titles
  const char * zero = "";
  const char * colz = "colz";
  const char * ztitle_cm = "z [cm]";
  const char * xtitle_cm = "x [cm]";
  const char * ytitle_cm = "y [cm]";
  //const char * xtitle_GeV = "Energy [GeV]";
  const char * xtitle_GeV = "ADC";

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon1" << "." << RunNumber;
  if (! gROOT->FindObject( cname.str().c_str() ))
    {
      MakeCanvas1( cname.str().c_str() );
    }

  Draw0thPad(zdc_value, 0, 0);

  //texts
  ostringstream n_zdc_text, s_zdc_text, zdc_vertex_text1, zdc_vertex_text2, s_smd_text, n_smd_text;
  n_zdc_text.str("");
  s_zdc_text.str("");
  zdc_vertex_text1.str("");
  zdc_vertex_text2.str("");
  s_smd_text.str("");
  n_smd_text.str("");
  float vertex_position = 0.;
  float smd_pos[4];
  for (int i = 0; i < 4; i++)
    {
      smd_pos[i] = 0.;
    }

  transparent[0]->Clear();
  if (!zdc_adc_north)
    {
      DrawDeadServer(transparent[0]);
      oldStyle->cd();
      return -1;
    }

  cout << "Drawing first pad..." << endl;
  DrawHisto(zdc_adc_north, 1, true, 1, xtitle_GeV, zero, zero);
  line[0]->Draw();
  line[1]->Draw();

  /* //uncomment in Au or d, comment out in p
  float north_n_energy = 0.;
  if (zdc_adc_north)
  {
    FitGauss(zdc_adc_north, 50, 200);
    north_n_energy = par[1] ; //par[1] given by FitGauss
    n_zdc_text << "single n energy = " << setprecision(4) << north_n_energy << " GeV";
    cout << n_zdc_text.str().c_str() << endl;
    n_zdc_text << " " ;
    DrawPaveText(n_zdc_text.str().c_str(), 3);
  }
  */
  cout << "First pad drawn." << endl;

  cout << "Drawing second pad..." << endl;
  DrawHisto(zdc_adc_south, 2, true, 1, xtitle_GeV, zero, zero);
  line[2]->Draw();
  line[3]->Draw();
  /* //uncomment in Au or d, comment out p
  float south_n_energy = 0.;
  if (zdc_adc_south)
  {
    FitGauss(zdc_adc_south, 1000, 2000);
    south_n_energy = par[1];
    s_zdc_text << "few n energy = " << setprecision(4) << south_n_energy << " GeV";
    cout << s_zdc_text.str().c_str() << endl;
    s_zdc_text << " ";
    DrawPaveText(s_zdc_text.str().c_str(), 4);
  }
  */
  cout << "Second pad drawn." << endl;

  cout << "Drawing remaining pads of first canvas..." << endl;
  DrawHisto(zdc_vertex, 5, true , 1, ztitle_cm, zero, zero);
  DrawHisto(zdc_vertex_b, 6, true , 1, ztitle_cm, zero, zero);
  if (zdc_vertex_b)
  {
    FitGauss(zdc_vertex_b, -200, 200);
    vertex_position = par[1];
    zdc_vertex_text1 << "ZDC vertex position = " << setprecision(4) << vertex_position << " cm";
    cout << zdc_vertex_text1.str().c_str() << endl;
    DrawPaveText(zdc_vertex_text1.str().c_str(), 7);
  }

  DrawHisto(smd_xy_north, 8, false , 1, xtitle_cm, ytitle_cm, colz);
  arc[0]->Draw();
  //Fit2DGauss(smd_xy_north, -4., 4., -4., 4.); //not working
  DrawHisto(smd_xy_south, 9, false , 1, xtitle_cm, ytitle_cm, colz);
  arc[1]->Draw();

  smd_pos[0] = smd_y_s->GetMean(2);
  smd_pos[1] = smd_x_s->GetMean(2);
  s_smd_text << "x = " << setprecision(4) << smd_pos[1]
	     << " cm, y =" << setprecision(4) << smd_pos[0] << " cm";
  DrawPaveText(s_smd_text.str().c_str(), 10);

  smd_pos[2] = smd_y_n->GetMean(2);
  smd_pos[3] = smd_x_n->GetMean(2);
  n_smd_text << "x = " << setprecision(4) << smd_pos[3]
	     << " cm, y =" << setprecision(4) << smd_pos[2] << " cm";
  DrawPaveText(n_smd_text.str().c_str(), 11);

  cout << "Pads of first canvas drawn." << endl;

  cout << "First canvas drawn. " << endl;
  TC[0]->Update();
  oldStyle->cd();
  return iret;
}


int ZdcMonDraw::Draw2()
{
  cout << "Drawing second canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  int iret = 0;

  // get pointer for each histogram
  TH1 *smd_hor_south = cl->getHisto("smd_hor_south");
  TH1 *smd_ver_south = cl->getHisto("smd_ver_south");
  TH1 *smd_hor_north = cl->getHisto("smd_hor_north");
  TH1 *smd_ver_north = cl->getHisto("smd_ver_north");
  TH1 *smd_hor_north_small = cl->getHisto("smd_hor_north_small");
  TH1 *smd_ver_north_small = cl->getHisto("smd_ver_north_small");
  TH1 *smd_hor_north_good = cl->getHisto("smd_hor_north_good");
  TH1 *smd_ver_north_good = cl->getHisto("smd_ver_north_good");
  TH2 *zdc_hor_north = dynamic_cast <TH2*> (cl->getHisto("zdc_hor_north"));
  TH2 *zdc_ver_north = dynamic_cast <TH2*> (cl->getHisto("zdc_ver_north"));
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  TH1 *smd_sum_hor_south = cl->getHisto("smd_sum_hor_south");
  TH1 *smd_sum_ver_south = cl->getHisto("smd_sum_ver_south");
  TH1 *smd_sum_hor_north = cl->getHisto("smd_sum_hor_north");
  TH1 *smd_sum_ver_north = cl->getHisto("smd_sum_ver_north");
  const char * zero = "";
  const char * xtitle_cm = "x [cm]";
  const char * ytitle_cm = "y [cm]";
  const char * xtitle_ADC = "Energy [ADC counts]";
  const char * xtitle_GeV = "Energy [GeV]";
  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon2" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas2(cname.str().c_str());
    }
  Draw0thPad(zdc_value, 12, 1);

  cout << "Drawing remaining pads of second canvas..." << endl;

  transparent[1]->Clear();
  if (!smd_ver_north_good)
    {
      DrawDeadServer(transparent[1]);
      oldStyle->cd();
      return -1;
    }

  //drawing histos
  DrawHisto(smd_ver_north, 13, false, 4, xtitle_cm, zero, zero);
  DrawHistoSame(smd_ver_north_small, 2);
  DrawHistoSame(smd_ver_north_good, 1);
  DrawHisto(smd_ver_south, 14, false, 1, xtitle_cm, zero, zero);
  DrawHisto(smd_hor_north, 15, false, 4, ytitle_cm, zero, zero);
  DrawHistoSame(smd_hor_north_small, 2);
  DrawHistoSame(smd_hor_north_good, 1);

  DrawHisto(smd_hor_south, 16, false, 1, ytitle_cm, zero, zero);
  DrawHisto(zdc_hor_north, 17, false, 1, xtitle_GeV, ytitle_cm, zero);
  DrawHisto(zdc_ver_north, 18, false, 1, xtitle_GeV, xtitle_cm, zero);
  DrawHisto(smd_sum_ver_north, 19, true, 1, xtitle_ADC, zero, zero);
  DrawHisto(smd_sum_ver_south, 20, true, 1, xtitle_ADC, zero, zero);
  DrawHisto(smd_sum_hor_north, 21, true, 1, xtitle_ADC, zero, zero);
  DrawHisto(smd_sum_hor_south, 22, true, 1, xtitle_ADC, zero, zero);

  cout << "Pads of second canvas drawn." << endl;
  cout << "Second canvas drawn." << endl;

  TC[1]->Update();
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw3()
{
  cout << "Drawing third canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;

  // get pointer for each histogram
  TH2 *smd_yt_south = dynamic_cast <TH2*> (cl->getHisto("smd_yt_south"));
  TH2 *smd_xt_south = dynamic_cast <TH2*> (cl->getHisto("smd_xt_south"));
  TH2 *smd_yt_north = dynamic_cast <TH2*> (cl->getHisto("smd_yt_north"));
  TH2 *smd_xt_north = dynamic_cast <TH2*> (cl->getHisto("smd_xt_north"));

  TProfile *smd_avy_south = 0, *smd_avx_south = 0, *smd_avy_north = 0, *smd_avx_north = 0;

  if(smd_yt_south!=NULL)
    smd_avy_south = smd_yt_south->ProfileX("smd_avy_south");
  if(smd_xt_south!=NULL)
    smd_avx_south = smd_xt_south->ProfileX("smd_avx_south");
  if(smd_yt_north!=NULL)
    smd_avy_north = smd_yt_north->ProfileX("smd_avy_north");
  if(smd_xt_north!=NULL)
    smd_avx_north = smd_xt_north->ProfileX("smd_avx_north");

  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  //  const char * zero = "";
  const char * title_xcm = "x [cm]";
  const char * title_ycm = "y [cm]";
  const char * title_num = "Event number";
  const char * cm = " cm";

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon3" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas3(cname.str().c_str());
    }
  Draw0thPad(zdc_value, 23, 2);
  cout << "Drawing remaining pads of third canvas..." << endl;

  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[2]->Clear();
  if (!smd_xt_north)
    {
      DrawDeadServer(transparent[2]);
      oldStyle->cd();
      return -1;
    }
  //getting the number of events (no filter in zdc_value, so this contains all * 40
  double d_range = 100000. / 500.;
  if (zdc_value)
    {
      d_range = zdc_value->GetEntries() / (40. * 500.);
    }
  int i_range = (int)d_range;

  //drawing
  DrawProfile(smd_avy_south, 24, false, 1, title_num, title_ycm, i_range);
  line[4]->Draw();
  DrawProfStat(sysmdt, smd_yt_south, cm, 25, 'a');
  DrawProfile(smd_avx_south, 26, false, 1, title_num, title_xcm, i_range);
  line[5]->Draw();
  DrawProfStat(sxsmdt, smd_xt_south, cm, 27, 'a');
  DrawProfile(smd_avy_north, 28, false, 1, title_num, title_ycm, i_range);
  line[6]->Draw();
  DrawProfStat(nysmdt, smd_yt_north, cm, 29, 'a');
  DrawProfile(smd_avx_north, 30, false, 1, title_num, title_xcm, i_range);
  line[7]->Draw();
  DrawProfStat(nxsmdt, smd_xt_north, cm, 31, 'a');

  cout << "Pads of third canvas drawn." << endl;
  cout << "Third canvas drawn." << endl;

  TC[2]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw4()
{
  cout << "Drawing fourth canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;

  // get pointer for each histogram

  TH2 *zdc_laser_south1 = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_south1"));
  TH2 *zdc_laser_south2 = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_south2"));
  TH2 *zdc_laser_south3 = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_south3"));
  TH2 *zdc_laser_north1 = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_north1"));
  TH2 *zdc_laser_north2 = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_north2"));
  TH2 *zdc_laser_north3 = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_north3"));
  TProfile *zdc_lasave_s1 = 0, *zdc_lasave_s2 = 0, *zdc_lasave_s3 = 0;
  TProfile *zdc_lasave_n1 = 0, *zdc_lasave_n2 = 0, *zdc_lasave_n3 = 0;
  if(zdc_laser_south1)
    zdc_lasave_s1 = zdc_laser_south1 ->ProfileX("zdc_lasave_s1");
  if(zdc_laser_south2)
    zdc_lasave_s2 = zdc_laser_south2 ->ProfileX("zdc_lasave_s2");
  if(zdc_laser_south3)
    zdc_lasave_s3 = zdc_laser_south3 ->ProfileX("zdc_lasave_s3");
  if(zdc_laser_north1)
    zdc_lasave_n1 = zdc_laser_north1 ->ProfileX("zdc_lasave_n1");
  if(zdc_laser_north2)
    zdc_lasave_n2 = zdc_laser_north2 ->ProfileX("zdc_lasave_n2");
  if(zdc_laser_north3)
    zdc_lasave_n3 = zdc_laser_north3 ->ProfileX("zdc_lasave_n3");
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  //  const char * zero = "";
  const char * title_ADC = "Energy [ADC counts]";
  const char * title_num = "Event number";
  const char * ADC = "";

  float led_mean[6];
  float led_rms[6];
  ostringstream led_av_text[6];
  for (int i = 0; i < 6; i++)
    {
      led_mean[i] = 0.;
      led_rms[i] = 0.;
      led_av_text[i].str("");
    }

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon4" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas4(cname.str().c_str());
    }
  Draw0thPad(zdc_value, 32, 3);
  cout << "Drawing remaining pads of fourth canvas..." << endl;

  transparent[3]->Clear();
  if (!zdc_laser_south3)
    {
      DrawDeadServer(transparent[3]);
      oldStyle->cd();
      return -1;
    }
  double d_range = 100000 / 500;
  if (zdc_value)
    {
      d_range = zdc_value->GetEntries() / (40 * 500);
    }
  int i_range = (int)d_range;

  DrawProfile(zdc_lasave_s1, 33, false, 1, title_num, title_ADC, i_range);
  line[8]->Draw();
  DrawProfStat(ls1t, zdc_laser_south1, ADC, 34, 'b');
  DrawProfile(zdc_lasave_n1, 35, false, 1, title_num, title_ADC, i_range);
  line[9]->Draw();
  DrawProfStat(ls2t, zdc_laser_north1, ADC, 36, 'b');
  DrawProfile(zdc_lasave_s2, 37, false, 1, title_num, title_ADC, i_range);
  line[10]->Draw();
  DrawProfStat(ls3t, zdc_laser_south2, ADC, 38, 'b');
  DrawProfile(zdc_lasave_n2, 39, false, 1, title_num, title_ADC, i_range);
  line[11]->Draw();
  DrawProfStat(ln1t, zdc_laser_north2, ADC, 40, 'b');
  DrawProfile(zdc_lasave_s3, 41, false, 1, title_num, title_ADC, i_range);
  line[12]->Draw();
  DrawProfStat(ln2t, zdc_laser_south3, ADC, 42, 'b');
  DrawProfile(zdc_lasave_n3, 43, false, 1, title_num, title_ADC, i_range);
  line[13]->Draw();
  DrawProfStat(ln3t, zdc_laser_north3, ADC, 44, 'b');

  cout << "Pads of fourth canvas drawn." << endl;
  cout << "Fourth canvas drawn." << endl;

  TC[3]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw5()
{
  cout << "Drawing fifth canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;

  // get pointer for each histogram
  TH2 *zdc_laser_south1b = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_south1b"));
  TH2 *zdc_laser_south2b = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_south2b"));
  TH2 *zdc_laser_south3b = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_south3b"));
  TH2 *zdc_laser_north1b = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_north1b"));
  TH2 *zdc_laser_north2b = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_north2b"));
  TH2 *zdc_laser_north3b = dynamic_cast <TH2*> (cl->getHisto("zdc_laser_north3b"));
  TProfile *zdc_lasave_s1b = 0, *zdc_lasave_s2b = 0, *zdc_lasave_s3b = 0;
  TProfile *zdc_lasave_n1b = 0, *zdc_lasave_n2b = 0, *zdc_lasave_n3b = 0;
  if(zdc_laser_south1b)
    zdc_lasave_s1b = zdc_laser_south1b ->ProfileX("zdc_lasave_s1b");
  if(zdc_laser_south2b)
    zdc_lasave_s2b = zdc_laser_south2b ->ProfileX("zdc_lasave_s2b");
  if(zdc_laser_south3b)
    zdc_lasave_s3b = zdc_laser_south3b ->ProfileX("zdc_lasave_s3b");
  if(zdc_laser_north1b)
    zdc_lasave_n1b = zdc_laser_north1b ->ProfileX("zdc_lasave_n1b");
  if(zdc_laser_north2b)
    zdc_lasave_n2b = zdc_laser_north2b ->ProfileX("zdc_lasave_n2b");
  if(zdc_laser_north3b)
    zdc_lasave_n3b = zdc_laser_north3b ->ProfileX("zdc_lasave_n3b");
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  //const char * zero = "";
  const char * title_ADC = "Energy [ADC counts]";
  const char * title_num = "Event number";
  const char * ADC = "";

  float led_mean[6];
  float led_rms[6];
  ostringstream led_av_text[6];
  for (int i = 0; i < 6; i++)
    {
      led_mean[i] = 0.;
      led_rms[i] = 0.;
      led_av_text[i].str("");
    }

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon5" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas5(cname.str().c_str());
    }
  Draw0thPad(zdc_value, 45, 4);
  cout << "Drawing remaining pads of fifth canvas..." << endl;

  transparent[4]->Clear();
  if (!zdc_laser_south3b)
    {
      DrawDeadServer(transparent[4]);
      oldStyle->cd();
      return -1;
    }
  double d_range = 100000 / 500;
  if (zdc_value)
    {
      d_range = zdc_value->GetEntries() / (40 * 500);
    }
  int i_range = (int)d_range;

  DrawProfile(zdc_lasave_s1b, 46, false, 1, title_num, title_ADC, i_range);
  line[14]->Draw();
  DrawProfStat(ls1bt, zdc_laser_south1b, ADC, 47, 'b');
  DrawProfile(zdc_lasave_n1b, 48, false, 1, title_num, title_ADC, i_range);
  line[15]->Draw();
  DrawProfStat(ls2bt, zdc_laser_north1b, ADC, 49, 'b');
  DrawProfile(zdc_lasave_s2b, 50, false, 1, title_num, title_ADC, i_range);
  line[16]->Draw();
  DrawProfStat(ls3bt, zdc_laser_south2b, ADC, 51, 'b');
  DrawProfile(zdc_lasave_n2b, 52, false, 1, title_num, title_ADC, i_range);
  line[17]->Draw();
  DrawProfStat(ln1bt, zdc_laser_north2b, ADC, 53, 'b');
  DrawProfile(zdc_lasave_s3b, 54, false, 1, title_num, title_ADC, i_range);
  line[18]->Draw();
  DrawProfStat(ln2bt, zdc_laser_south3b, ADC, 55, 'b');
  DrawProfile(zdc_lasave_n3b, 56, false, 1, title_num, title_ADC, i_range);
  line[19]->Draw();
  DrawProfStat(ln3bt, zdc_laser_north3b, ADC, 57, 'b');

  cout << "Pads of fifth canvas drawn." << endl;
  cout << "Fifth canvas drawn." << endl;

  TC[4]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw6() //not used currently
{
  cout << "Drawing sixth canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  int iret = 0;

  // get pointer for each histogram
  smd_value = dynamic_cast <TH2*> (cl->getHisto("smd_value"));
  smd_value_good = dynamic_cast <TH2*> (cl->getHisto("smd_value_good"));
  smd_value_small = dynamic_cast <TH2*> (cl->getHisto("smd_value_small"));
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon6" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas6(cname.str().c_str());
    }
  Draw0thPad(zdc_value, 58, 5);

  cout << "Drawing remaining pads of sixth canvas..." << endl;

  transparent[5]->Clear();
  if (!smd_value_good)
    {
      DrawDeadServer(transparent[5]);
      oldStyle->cd();
      return -1;
    }

  DrawSmdValue(0, 59);
  DrawSmdValue(1, 60);
  DrawSmdValue(2, 61);
  DrawSmdValue(3, 62);
  DrawSmdValue(4, 63);
  DrawSmdValue(5, 64);
  DrawSmdValue(6, 65);
  DrawSmdValue(7, 66);

  cout << "Pads of sixth canvas drawn." << endl;
  cout << "Sixth canvas drawn." << endl;

  TC[5]->Update();
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw7() //not used currently
{
  cout << "Drawing seventh canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  int iret = 0;

  // get pointer for each histogram
  smd_value = dynamic_cast <TH2*> (cl->getHisto("smd_value"));
  smd_value_good = dynamic_cast <TH2*> (cl->getHisto("smd_value_good"));
  smd_value_small = dynamic_cast <TH2*> (cl->getHisto("smd_value_small"));
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon7" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas7(cname.str().c_str());
    }
  Draw0thPad(zdc_value, 67, 6);

  cout << "Drawing remaining pads of seventh canvas..." << endl;

  transparent[6]->Clear();
  if (!smd_value_good)
    {
      DrawDeadServer(transparent[6]);
      oldStyle->cd();
      return -1;
    }

  DrawSmdValue(8, 68);
  DrawSmdValue(9, 69);
  DrawSmdValue(10, 70);
  DrawSmdValue(11, 71);
  DrawSmdValue(12, 72);
  DrawSmdValue(13, 73);
  DrawSmdValue(14, 74);
  DrawSmdValue(15, 75);

  cout << "Pads of seventh canvas drawn." << endl;
  cout << "Seventh canvas drawn." << endl;

  TC[6]->Update();
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw8()
{
  cout << "Drawing eight' canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;

  // get pointer for each histogram
  TH2 *zdc_ratio_south = dynamic_cast <TH2*> (cl->getHisto("zdc_ratio_south"));
  TH2 *zdc_ratio_north = dynamic_cast <TH2*> (cl->getHisto("zdc_ratio_north"));

  const char * zero = "";

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon8" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas8(cname.str().c_str());
    }

  cout << "Drawing remaining pads of eight' canvas..." << endl;
  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[7]->Clear();
  if (!zdc_ratio_north && !zdc_ratio_south)
    {
      DrawDeadServer(transparent[7]);
      oldStyle->cd();
      return -1;
    }

  //drawing
  Draw0thPad(zdc_ratio_north, 76, 7);
  cout << "Pads of eigth' canvas drawn." << endl;
  DrawHisto(zdc_ratio_north, 77, false, 1, zero, zero, zero);
  DrawHisto(zdc_ratio_south, 78, false, 1, zero, zero, zero);
  cout << "Eight' canvas drawn." << endl;

  TC[7]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw9()
{
  cout << "Drawing ninth canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;

  const char * zero = "";
  const char * xtitle_GeV = "ADC";

  // get pointer for each histogram
  TH2 *zdc_vs_bbc = dynamic_cast <TH2*> (cl->getHisto("zdc_vs_bbc"));
  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));

  /*Ciprian Gal 110211*/
  TH1 *zdc_adc_n_ind[3];
  TH1 *zdc_adc_s_ind[3];
  for(int i=0;i<3;i++)
    {
      char hname[256];
      sprintf(hname,"zdc_adc_n_ind_%d",i);
      zdc_adc_n_ind[i] = (cl->getHisto(hname));
      sprintf(hname,"zdc_adc_s_ind_%d",i);
      zdc_adc_s_ind[i] = (cl->getHisto(hname));
    }

  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon9" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas9(cname.str().c_str());
    }
  cout << "Drawing remaining pads of ninth canvas..." << endl;
  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[8]->Clear();
  if (!zdc_vs_bbc || !zdc_value)
    {
      DrawDeadServer(transparent[8]);
      oldStyle->cd();
      return -1;
    }

  //drawing

  /*Ciprian Gal 110211*/
  for(int i=0;i<3;i++)
    {
      DrawHisto(zdc_adc_s_ind[i], 137+i, true, 1, xtitle_GeV, zero, zero);
      DrawHisto(zdc_adc_n_ind[i], 140+i, true, 1, xtitle_GeV, zero, zero);
    }
  /*Ciprian Gal 110211*/

  Draw0thPad(zdc_value, 79, 8);
  cout << "Pads of ninth canvas drawn." << endl;
  //DrawHisto(zdc_vs_bbc, 80, false, 1, "ZDC Zvertex", "BBC Zvertex", zero);

  zvb_prof = zdc_vs_bbc->ProfileY("zvb_prof");
  
  //DS: changed fit range to be within the BBCLL1 vertex cut
  FitLinear(zvb_prof, -10, 10);

  double zdc_to_bbc = par[1]; //par[0,1] are given by FitLinear
  double zdc_to_bbc_err = perr; //perr[0,1] are given by FitLinear
  int i_range = 128;
  DrawProfile(zvb_prof, 80, false, 1, "ZDC Zvertex", "BBC Zvertex", i_range);

  ostringstream ztb_text;
  ztb_text.str("");
  ztb_text << "ZDC to BBC ratio = " << setprecision(3) << zdc_to_bbc << " +- " << setprecision(2) << zdc_to_bbc_err;
  cout << ztb_text.str().c_str() << endl;

  DrawPaveText(ztb_text.str().c_str(), 81);

  cout << "Ninth canvas drawn." << endl;

  TC[8]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw16()
{
  cout << "Drawing 16th canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;
  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon16" << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas16(cname.str().c_str());
    }

  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  if (!zdc_value )
    {
      DrawDeadServer(transparent[15]);
      oldStyle->cd();
      return -1;
    }

  TH2* zdc_adc_vs_tdc[40];
  for(int i=0; i<40; i++)
    {
      char name[64];
      sprintf(name,"zdc_adc_vs_tdc_ch%d",i);
      zdc_adc_vs_tdc[i] = dynamic_cast <TH2*> (cl->getHisto(name));
    }

  cout << "Drawing remaining pads of 16th canvas..." << endl;
  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[15]->Clear();

  
  Draw0thPad(zdc_value, 143, 15);

  for(int i=0; i<40; i++)
    {
      //DrawHisto2D(TH2 *histo, int npad, int logz, const char * xtit, const char * ytit, const char * dr_opt = "ALL")
      if(i<8)
	DrawHisto2D(zdc_adc_vs_tdc[i], 144+i, 1, "ZDC ADC value", "ZDC TDC value", "colz");
      else
	DrawHisto2D(zdc_adc_vs_tdc[i], 144+i, 1, "SMD ADC value", "SMD TDC value", "colz");
    }

  cout << "Pads of 16th canvas drawn." << endl;


  cout << "16th canvas drawn." << endl;

  TC[15]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw17()
{
  int ncanv=16;

  cout << "Drawing "<<ncanv+1<<"th canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;
  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon"<<ncanv+1 << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas17(cname.str().c_str());
    }

  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  if (!zdc_value )
    {
      DrawDeadServer(transparent[ncanv]);
      oldStyle->cd();
      return -1;
    }

  TH2* fveto_adc_vs_tdc[40];
  for(int i=0; i<40; i++)
    {
      char name[64];
      sprintf(name,"fveto_adc_vs_tdc_ch%d",i);
      fveto_adc_vs_tdc[i] = dynamic_cast <TH2*> (cl->getHisto(name));
    }

  cout << "Drawing remaining pads of "<<ncanv+1<<"th canvas..." << endl;
  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[ncanv]->Clear();

  
  Draw0thPad(zdc_value, 184, ncanv);

  for(int i=0; i<40; i++)
    {
      //DrawHisto2D(TH2 *histo, int npad, int logz, const char * xtit, const char * ytit, const char * dr_opt = "ALL")
      if(i<8)
	DrawHisto2D(fveto_adc_vs_tdc[i] , 185+i, 1, "Front veto ADC value", "ZDC TDC value", "colz");
      else
	DrawHisto2D(fveto_adc_vs_tdc[i] , 185+i, 1, "Front veto ADC value", "SMD TDC value", "colz");
    }

  cout << "Pads of "<<ncanv+1<<"th canvas drawn." << endl;


  cout << ncanv+1<<"th canvas drawn." << endl;

  TC[ncanv]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw18()
{
  int ncanv=17;

  cout << "Drawing "<<ncanv+1<<"th canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;
  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon"<<ncanv+1 << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas18(cname.str().c_str());
    }

  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  if (!zdc_value )
    {
      DrawDeadServer(transparent[ncanv]);
      oldStyle->cd();
      return -1;
    }

  TH2* rveto_adc_vs_tdc[40];
  for(int i=0; i<40; i++)
    {
      char name[64];
      sprintf(name,"rveto_adc_vs_tdc_ch%d",i);
      rveto_adc_vs_tdc[i] = dynamic_cast <TH2*> (cl->getHisto(name));
    }

  cout << "Drawing remaining pads of "<<ncanv+1<<"th canvas..." << endl;
  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[ncanv]->Clear();

  
  Draw0thPad(zdc_value, 225, ncanv);

  for(int i=0; i<40; i++)
    {
      //DrawHisto2D(TH2 *histo, int npad, int logz, const char * xtit, const char * ytit, const char * dr_opt = "ALL")
      if(i<8)
	DrawHisto2D( rveto_adc_vs_tdc[i] , 226+i, 1, "Rear veto ADC value", "ZDC TDC value", "colz");
      else
	DrawHisto2D( rveto_adc_vs_tdc[i] , 226+i, 1, "Rear veto ADC value", "SMD TDC value", "colz");
    }

  cout << "Pads of "<<ncanv+1<<"th canvas drawn." << endl;


  cout << ncanv+1<<"th canvas drawn." << endl;

  TC[ncanv]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::Draw19()
{
  int ncanv=18;

  cout << "Drawing "<<ncanv+1<<"th canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);
  int iret = 0;
  long int RunNumber = cl->RunNumber();
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon"<<ncanv+1 << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()))
    {
      MakeCanvas19(cname.str().c_str());
    }

  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));
  if (!zdc_value )
    {
      DrawDeadServer(transparent[ncanv]);
      oldStyle->cd();
      return -1;
    }

  TH2* veto_adc_vs_pos[8];
  for(int i=0; i<8; i++)
    {
      char name[64];
      sprintf(name,"veto_adc_vs_pos_%d",i);
      veto_adc_vs_pos[i] = dynamic_cast <TH2*> (cl->getHisto(name));
    }

  cout << "Drawing remaining pads of "<<ncanv+1<<"th canvas..." << endl;
  //this is just for putting the SERVER DEAD signal is some histos do not exist
  transparent[ncanv]->Clear();

  
  Draw0thPad(zdc_value, 266, ncanv);

  DrawHisto2D(veto_adc_vs_pos[0], 267, 1, "Front veto ADC value", "South SMD Y position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[1], 268, 1, "Front veto ADC value", "South SMD X position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[2], 269, 1, "Rear veto ADC value", "South SMD Y position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[3], 270, 1, "Rear veto ADC value", "South SMD X position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[4], 271, 1, "Front veto ADC value", "North SMD Y position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[5], 272, 1, "Front veto ADC value", "North SMD X position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[6], 273, 1, "Rear veto ADC value",  "North SMD Y position (cm)", "colz");
  DrawHisto2D(veto_adc_vs_pos[7], 274, 1, "Rear veto ADC value",  "North SMD X position (cm)", "colz");

  cout << "Pads of "<<ncanv+1<<"th canvas drawn." << endl;


  cout << ncanv+1<<"th canvas drawn." << endl;

  TC[ncanv]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::MakePS(const char *what)
{
  int iret = Draw("HTML");
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename[NUM_CANV];
  for ( int i = 0; i < NUM_CANV; i++ )
    {
      if ( TC[i] )
        {
          filename[i] << ThisName << "." << cl->RunNumber() << "_" << i << ".ps";
          TC[0]->Print(filename[i].str().c_str());
        }
    }
  return 0;
}

int ZdcMonDraw::MakeHtml(const char *what)
{
  int iret = Draw("HTML");
  if (iret)
    {
      return iret;
    }
  ostringstream name[NUM_CANV], basename[NUM_CANV];
  string pngfile[NUM_CANV];
  OnlMonClient *cl = OnlMonClient::instance();
  for (int i = 0; i < NUM_CANV;i++)
    {
      name[i].str("");
      basename[i].str("");
    }

  name[0] << "Main Monitor";
  name[1] << "Smd expert data";
  name[2] << "Smd position values";
  name[3] << "Led energy values";
  name[4] << "Led timing values";
  name[5] << "Smd values south";
  name[6] << "Smd values north";
  name[7] << "Zdc ratios";
  name[8] << "Zdc to Bbc zvertex";
  name[9] << "Zdc ppg history";
  name[10] << "Smd south y history";
  name[11] << "Smd south x history";
  name[12] << "Smd north y history";
  name[13] << "Smd north x history";
  name[14] << "Pos., vertex and en. history";
  name[15] << "Adc vs Tdc";
  name[16] << "Front Veto Adc vs ZDC SMD Tdc";
  name[17] << "Rear Veto Adc vs ZDC SMD Tdc";
  name[18] << "Charge Veto Adc vs SMD Position";

  for ( int i = 0; i < NUM_CANV; i++ )
    {
      if ( TC[i] )
        {
          cout << "Generating html for " << name[i].str().c_str() << endl;
          basename[i] << i;
          pngfile[i] = cl->htmlRegisterPage(*this, name[i].str(), basename[i].str(), "png");
          cl->CanvasToPng(TC[i], pngfile[i]);
        }
    }
  cl->SaveLogFile(*this);

  return 0;
}

int ZdcMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "ZDCMONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}


int ZdcMonDraw::DrawHistory(const char *CanvasName, int Nhist)
{
  cout << "Drawing history canvas..." << endl;
  OnlMonClient *cl = OnlMonClient::instance();
  TStyle* oldStyle = gStyle;
  zdcStyle->cd();
  zdcStyle->SetOptStat(0);

  TH2 *zdc_value = dynamic_cast <TH2*> (cl->getHisto("zdc_value"));

  long int RunNumber = cl->RunNumber();
  int Ncanv = 9 + Nhist;
  ostringstream cname;
  cname.str("");
  cname << "ZdcMon" << Ncanv + 1 << "." << RunNumber;
  if (! gROOT->FindObject(cname.str().c_str()) )
    {
      MakeCanvasHistory(cname.str().c_str(), CanvasName, Nhist);
    }
  int Npad = 82 + 9 * Nhist;
  Draw0thPad(zdc_value, Npad, Ncanv);
  Npad = 83 + 9 * Nhist;
  ostringstream varname;
  varname.str("");
  int iret;
  if (Nhist < 5)
    {
      for (int i = 0;i < 8;i++)
        {
          varname.str("");
          varname << "zdcmonppg" << Nhist*8 + i;
          iret = DrawHistoryPlot(Ncanv, Npad + i, Nhist * 8 + i, varname.str().c_str());
        }
    }
  else
    {
      for (int i = 0;i < 4;i++)
        {
          varname.str("");
          varname << "smd_pos_" << i;
          iret = DrawHistoryPlot(Ncanv, Npad + i, Nhist * 8 + i, varname.str().c_str());
        }
      iret = DrawHistoryPlot(Ncanv, Npad + 4, Nhist * 8 + 4, "zdc_energy_south");
      iret = DrawHistoryPlot(Ncanv, Npad + 5, Nhist * 8 + 5, "zdc_energy_north");
      iret = DrawHistoryPlot(Ncanv, Npad + 6, Nhist * 8 + 6, "zdc_vertex");
    }
  transparent[Ncanv]->Clear();
  if (!zdc_value)
    {
      DrawDeadServer(transparent[Ncanv]);
      oldStyle->cd();
      return iret;
    }

  cout << "History canvas drawn." << endl;

  TC[Ncanv]->Update();
  zdcStyle->SetOptStat(0);
  oldStyle->cd();
  return iret;
}

int ZdcMonDraw::DrawHistoryPlot(int Ncanv, int Npad, int Nplot, const char * varname_c)
{
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  vector<float> var;
  vector<float> varerr;
  vector<time_t> timestamp;
  vector<int> runnumber;
  // this sets the time range from whihc values should be returned
  time_t begin = 0; // begin of time (1.1.1970)
  time_t end = time(NULL); // current time (right NOW)
  ostringstream varname;
  varname.str("");
  varname << varname_c;
  iret = dbvars->GetVar(begin, end, varname.str(), timestamp, runnumber, var, varerr);
  if (iret)
    {
      cout << PHWHERE << " Error in db access " << Nplot << endl;
      return 0;
    }
  cout << "Drawing " << varname.str().c_str() << endl;
  if (var.size() == 0)
    {
      cout << "no values to plot for " << varname.str().c_str() << endl;
      return 0;
    }
  // timestamps come sorted in ascending order
  float *x = new float[var.size()];
  float *y = new float[var.size()];
  float *ex = new float[var.size()];
  float *ey = new float[var.size()];
  int n = var.size();
  for (unsigned int j = 0; j < var.size();j++)
    {
      x[j] = timestamp[j] - TimeOffsetTicks;
      y[j] = var[j];
      ex[j] = 0;
      ey[j] = varerr[j];
    }
  Pad[Npad]->cd();
  if (gr[Nplot])
    {
      delete gr[Nplot];
    }

  gr[Nplot] = new TGraphErrors(n, x, y, ex, ey);

  gr[Nplot]->SetMarkerColor(4);
  gr[Nplot]->SetMarkerStyle(21);
  gr[Nplot]->SetTitle(varname.str().c_str());
  gr[Nplot]->Draw("ALP");

  //gr[Nplot]->GetXaxis()->SetTimeDisplay(1);
  gr[Nplot]->GetXaxis()->SetLabelSize(0.08);
  // the x axis labeling looks like crap
  // please help me with this, the SetNdivisions
  // don't do the trick

  gr[Nplot]->GetXaxis()->SetNdivisions(5);
  gr[Nplot]->GetXaxis()->SetTimeOffset(TimeOffsetTicks);
  gr[Nplot]->GetXaxis()->SetTimeFormat("%m/%d");
  gr[Nplot]->GetYaxis()->SetLabelSize(0.07);
  gr[Nplot]->GetYaxis()->SetNdivisions(5);

  delete [] x;
  delete [] y;
  delete [] ex;
  delete [] ey;
  return iret;
}


