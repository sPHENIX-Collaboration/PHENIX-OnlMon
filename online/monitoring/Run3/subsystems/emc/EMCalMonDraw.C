#include <EMCalMonDraw.h>
#include <OnlMonDB.h>
#include <OnlMonClient.h>
#include <EmcAnalys.h>

#include <TArrow.h>
#include <TAttText.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TImage.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>


using namespace std;

#define REF_EVENTMIN     5

#define PBGL_ADCMIN       10    // led amplitude less 10 (dead channel)
#define PBGL_RMS_ADCMAX   0.25  // RMS of led more 25% (noisy ADC)
#define PBGL_RMS_TACMAX   50    // RMS for TAC more 50 (noisy TAC)
#define PBGL_OUT_ADCMIN   0.2   // led amplitude too low  (x above average)
#define PBGL_OUT_ADCMAX   2.3   // led amplitude too high (x above average)  ~ 3500
#define PBGL_OUT_TACMIN   200   // min TAC value in led
#define PBGL_OUT_TACMAX   3000  // max TAC value in led

#define PBSC_ADCMIN       4    // laser amplitude less 4 (dead channel)
#define PBSC_RMS_ADCMAX   0.25  // RMS of laser more 25% (noisy ADC)
#define PBSC_RMS_TACMAX   50    // RMS for TAC more 50   (noisy TAC)
#define PBSC_OUT_ADCMIN   0.2   // laser amplitude too low  (x above average)
#define PBSC_OUT_ADCMAX   2.5   // laser amplitude too high (x above average)
#define PBSC_OUT_TACMIN   200   // min TAC value in laser
#define PBSC_OUT_TACMAX   3000  // max TAC value in laser

#define DEAD_ALARM_LIMIT  2.5   // percent limit to alarm shift crew
#define ADC_ALARM_LIMIT   3.5   // percent limit to alarm shift crew


EMCalMonDraw::EMCalMonDraw(): OnlMonDraw("EMCalMON")
{
  PbSc_tp = new TH1F("PBSC_tp" , " Test pulse ", 200, -0.001, 2.);
  PbSc_tp->SetXTitle("adc/adc(time0)");

  normPbSc_sm = new TH1F("normPBSC_sm", "normalized PMTs", 200, -0.001, 2.0);
  normPbSc_sm->SetXTitle("normadc/normadc(time0)");

  PbGl_Stat_Blind = new TH2F("PbGl_Stat_Blind", "PbGl blind or dead LED", 96, 0, 96, 96, 0, 96);
  PbGl_Stat_NoiseADC = new TH2F("PbGl_Stat_NoiseADC", "PbGl noise ADC", 96, 0, 96, 96, 0, 96);
  PbGl_Stat_NoiseTAC = new TH2F("PbGl_Stat_NoiseTAC", "PbGl noise TAC", 96, 0, 96, 96, 0, 96);
  PbGl_Stat_PROBL = new TH2F("PbGl_Stat_PROBL", "PbGl status", 96, 0, 96, 96, 0, 96);

  PbSc_Stat_Blind = new TH2F("PbSc_Stat_Blind", "PbSc blind or dead Laser", 72, 0, 72, 216, 0, 216);
  PbSc_Stat_NoiseADC = new TH2F("PbSc_Stat_NoiseADC", "PbSc noise ADC", 72, 0, 72, 216, 0, 216);
  PbSc_Stat_NoiseTAC = new TH2F("PbSc_Stat_NoiseTAC", "PbSc noise TAC", 72, 0, 72, 216, 0, 216);
  PbSc_Stat_PROBL = new TH2F("PbSc_Stat_PROBL", "PbSc status", 72, 0, 72, 216, 0, 216);

  TStyle* oldStyle = gStyle;  // save current style to a pointer oldStyle

  EmcStyle = new TStyle("c1Style", "my style 1");
  EmcStyle->Reset();
  EmcStyle->SetCanvasColor(0);
  EmcStyle->SetCanvasBorderMode(0);
  EmcStyle->SetPadColor(21);
  EmcStyle->SetPadBorderMode(0);
  EmcStyle->SetTitleColor(0);
  EmcStyle->SetStatColor(0);
  //   EmcStyle->SetOptTitle(1);

  oldStyle->cd();            // back to the saved style

  EmcLabel[0] = new TPaveText(0.01, 0.96, 0.66, 0.99, "br");
  EmcLabel[1] = new TPaveText(0.01, 0.46, 0.99, 0.497, "br");
  EmcLabel[2] = new TPaveText(0.67, 0.84, 0.99, 0.94, "br");
  EmcLabel[3] = new TPaveText(0.67, 0.95, 0.99, 0.99, "br");
  EmcLabel[4] = new TPaveText(0.67, 0.511, 0.99, 0.83, "brNDC");
  EmcStatLabel[0] = new TPaveText(0.02, 0.96, 0.19, 0.99, "br");
  EmcStatLabel[1] = new TPaveText(0.81, 0.96, 0.98, 0.99, "br");
  EmcStatLabel[2] = new TPaveText(0.21, 0.96, 0.79, 0.99, "br");
  EmcStatLabel[3] = new TPaveText(0.02, 0.16, 0.22, 0.19, "br");
  EmcStatLabel[4] = new TPaveText(0.22, 0.16, 0.41, 0.19, "br");
  EmcStatLabel[5] = new TPaveText(0.41, 0.16, 0.60, 0.19, "br");
  EmcStatLabel[6] = new TPaveText(0.60, 0.16, 0.79, 0.19, "br");
  EmcStatLabel[7] = new TPaveText(0.79, 0.16, 0.98, 0.19, "br");
  EmcStatLabel[8] = new TPaveText(0.02, 0.08, 0.49, 0.15, "br");
  EmcStatLabel[9] = new TPaveText(0.51, 0.08, 0.98, 0.15, "br");
  EmcStatLabel[10] = new TPaveText(0.02, 0.01, 0.98, 0.07, "br");
  for (int i = 0; i < 10; i++)
    {
      if (i < 2)
        {
          EmcLabelPi0[i] = new TPaveText(0.60, 0.57, 0.99, 1.0, "brNDC");
        }
      else
        {
          EmcLabelPi0[i] = new TPaveText(0.42, 0.57, 0.99, 1.0, "brNDC");
        }
    }
  arrowPbgl[0] = new TArrow(0.8, 1.0, 1.2, 1.0, 0.03, "<|>");
  arrowPbgl[1] = new TArrow(0.7, 2.0, 1.3, 2.0, 0.03, "<|>");
  arrowPbgl[2] = new TArrow( -400, 1.0, 400, 1.0, 0.03, "<|>");
  arrowPbgl[3] = new TArrow( -600, 2.0, 600, 2.0, 0.03, "<|>");
  arrowPbsc[0] = new TArrow(0.8, 1.0, 1.2, 1.0, 0.03, "<|>");
  arrowPbsc[1] = new TArrow(0.7, 4.0, 1.3, 4.0, 0.03, "<|>");
  arrowPbsc[2] = new TArrow(0.7, 4.0, 1.3, 4.0, 0.03, "<|>");
  arrowPbsc[3] = new TArrow( -400, 1.0, 400, 1.0, 0.03, "<|>");
  arrowPbsc[4] = new TArrow( -600, 4.0, 600, 4.0, 0.03, "<|>");
  arrowPbsc[5] = new TArrow(0.9, 1.0, 1.1, 1.0, 0.03, "<|>");

  fTr = 0; //choice trigger 0= BBCLL1(>1 tubes), 1= ERTLL1_4x4&BBCLL1

  fpol2 = new TF1("fpol2", "[0] + [1]*x + [2]*x*x", 0.1, 0.25);
  fpol2->SetParNames("p0", "p1", "p2");
  fpol2->SetLineColor(9);
  fitp0 = new TF1("fitp0", "pol2(0)+gaus(3)");
  fitp0->SetLineColor(9);


  dbvars = new OnlMonDB(ThisName);
  for (int i = 0; i < 2; i++)
    {
      statDeadServer[i] = 0;
    }

  return ;
}

EMCalMonDraw::~EMCalMonDraw()
{
  for (int i = 0; i < 4;i++)
    {
      delete arrowPbgl[i];
    }
  for (int i = 0; i < 6;i++)
    {
      delete arrowPbsc[i];
    }
  delete EmcStyle;


  for (int i = 0; i < 5;i++)
    {
      delete EmcLabel[i];
    }
  for (int i = 0; i < 11;i++)
    {
      delete EmcStatLabel[i];
    }
  for (int i = 0; i < 10;i++)
    {
      delete EmcLabelPi0[i];
    }
  delete PbGl_Stat_Blind;
  delete PbGl_Stat_NoiseADC;
  delete PbGl_Stat_NoiseTAC;
  delete PbGl_Stat_PROBL;


  delete PbSc_Stat_Blind;
  delete PbSc_Stat_NoiseADC;
  delete PbSc_Stat_NoiseTAC;
  delete PbSc_Stat_PROBL;

  delete PbSc_tp;

  delete normPbSc_sm;


  delete dbvars;
  delete fpol2;
  delete fitp0;

  return ;
}


int EMCalMonDraw::Init()
{

  return 0;
}

int EMCalMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  float ysizeLoc = ysize - (float) ysize * 0.1;
  float xsizeLoc = xsize - 4.0;
      ostringstream runnostream;
      string runstring;


  if (!strcmp(name, "EmcMon1"))
    {
      c_Emc[0] = new TCanvas(name, "EMCal Monitor", -1, 0, (int) xsizeLoc / 2, (int) ysizeLoc);
      gSystem->ProcessEvents();

      // +++++++++++++++++++++++++++ PBGL ++++++++++++++++++++++++++++++++++++++++++++++

      pad_PbGl[0] = new TPad("pad1_PbGl", "Pin", 0.01, 0.74, 0.33, 0.95, 21);
      pad_PbGl[1] = new TPad("pad2_PbGl", "PMT", 0.34, 0.74, 0.66, 0.95, 21);
      pad_PbGl[2] = new TPad("pad3_PbGl", "Pin (Time)", 0.01, 0.51, 0.33, 0.73, 21);
      pad_PbGl[3] = new TPad("pad4_PbGl", "PMT (Time)", 0.34, 0.51, 0.66, 0.73, 21);
      for (int i = 0; i < 4; i++)
        {
          arrowPbgl[i]->SetFillColor(8);
          arrowPbgl[i]->SetFillStyle(1001);
          pad_PbGl[i]->Draw();
        }

      EmcLabel[0]->Clear();
      EmcLabel[0]->AddText("EMCal PBGL");
      EmcLabel[0]->SetFillColor(31);
      EmcLabel[0]->SetBorderSize(3);
      EmcLabel[0]->Draw();

      // +++++++++++++++++++++++++++ PBSC ++++++++++++++++++++++++++++++++++++++++++++++++
      pad_PbSc[0] = new TPad("pad1_PbSc", "Pin", 0.01, 0.24, 0.33, 0.45, 21);
      pad_PbSc[1] = new TPad("pad2_PbSc", "PMT", 0.34, 0.24, 0.66, 0.45, 21);
      // pad_PbSc[2] = new TPad("pad5_PbSc", "Test pulse", 0.67, 0.24, 0.99, 0.45, 21);
      pad_PbSc[2] = new TPad("pad5_PbSc", "normalized PMT", 0.67, 0.24, 0.99, 0.45, 21);
      pad_PbSc[3] = new TPad("pad3_PbSc", "Pin (Time)", 0.01, 0.02, 0.33, 0.23, 21);
      pad_PbSc[4] = new TPad("pad4_PbSc", "PMT (Time) ", 0.34, 0.02, 0.66, 0.23, 21);
      // pad_PbSc[5] = new TPad("pad6_PbSc", "Test pulse (Time)", 0.67, 0.02, 0.99, 0.23, 21);
      pad_PbSc[5] = new TPad("pad6_PbSc", "Test pulse", 0.67, 0.02, 0.99, 0.23, 21);

      for (int i = 0; i < 6; i++)
        {
          if (i > 1)
            {
              arrowPbsc[i]->SetFillColor(8);
              arrowPbsc[i]->SetFillStyle(1001);
            }
          pad_PbSc[i]->Draw();
        }


      EmcLabel[1]->Clear();
      EmcLabel[1]->AddText("EMCal PBSC");
      EmcLabel[1]->SetFillColor(31);
      EmcLabel[1]->SetBorderSize(3);
      EmcLabel[1]->Draw();
      // ++++++++++++++++++++++++++++++++++++ EMC ++++++++++++++++++++++++++++++++++++++

      EmcLabel[2]->Clear();
      EmcLabel[2]->AddText("Call Expert - if any of:");
      EmcLabel[2]->AddText("1. Peak outside limits");
      EmcLabel[2]->AddText("   (where range indicated)");
      EmcLabel[2]->AddText("2. Multiple peaks");
      // EmcLabel[2]->AddText("3. More 10 entries with ADC=0");
      EmcLabel[2]->SetTextColor(50);
      EmcLabel[2]->SetFillColor(33);
      EmcLabel[2]->SetTextAlign(12);
      EmcLabel[2]->SetBorderSize(2);
      EmcLabel[2]->Draw();

      EmcLabel[3]->Clear();
      EmcLabel[3]->SetFillColor(45);
      EmcLabel[3]->SetBorderSize(2);
      EmcLabel[3]->Draw();

      EmcLabel[4]->Clear();
      EmcLabel[4]->SetTextFont(62);
      EmcLabel[4]->SetTextColor(12);
      // EmcLabel[4]->SetTextSize(0.017);
      EmcLabel[4]->SetTextSize(0.02);
      EmcLabel[4]->SetTextAlign(12);
      EmcLabel[4]->AddText(0.02, 0.96, "Waiting for laser event");
      EmcLabel[4]->SetFillColor(20);
      EmcLabel[4]->SetBorderSize(2);
      EmcLabel[4]->Draw();

      //      transparent[0] = new TPad("transparent1", "this does not show", 0.67, 0.511, 0.99, 0.83 );
      transparent[0] = new TPad("transparent1", "this does not show", 0, 0, 1, 1 );
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
      c_Emc[0]->SetEditable(0);
    }
  else if (!strcmp(name, "EmcMon2"))
    {
      c_Emc[1] = new TCanvas(name, "EMCal Status Monitor", -((int) xsizeLoc / 2 + 4), 0, (int) xsizeLoc / 2, (int) ysizeLoc);
      gSystem->ProcessEvents();
      //+++++++++++++++++++++++++++++++++++ PBGL status ++++++++++++++++++++++++++++++++++

      pad1_PbGl_Stat = new TPad("pad1_PbGl_Stat", "PBGL status", 0.02, 0.2, 0.49, 0.95, 21);
      pad1_PbGl_Stat->Draw();

      EmcStatLabel[0]->Clear();
      EmcStatLabel[0]->AddText("EMCal PBGL");
      EmcStatLabel[0]->SetFillColor(31);
      EmcStatLabel[0]->SetBorderSize(3);
      EmcStatLabel[0]->Draw();

      EmcStatLabel[8]->Clear();
      EmcStatLabel[8]->SetFillColor(20);
      EmcStatLabel[8]->SetBorderSize(2);
      EmcStatLabel[8]->SetTextAlign(12);
      EmcStatLabel[8]->Draw();

      // +++++++++++++++++++++++++++++++++++ PBSC status ++++++++++++++++++++++++++++++++++
      pad1_PbSc_Stat = new TPad("pad1_PbSc_Stat", "PBSC status", 0.51, 0.2, 0.98, 0.95, 21);
      pad1_PbSc_Stat->Draw();

      EmcStatLabel[1]->Clear();
      EmcStatLabel[1]->AddText("EMCal PBSC");
      EmcStatLabel[1]->SetFillColor(31);
      EmcStatLabel[1]->SetBorderSize(3);
      EmcStatLabel[1]->Draw();

      EmcStatLabel[9]->Clear();
      EmcStatLabel[9]->SetFillColor(20);
      EmcStatLabel[9]->SetBorderSize(2);
      EmcStatLabel[9]->SetTextAlign(12);
      EmcStatLabel[9]->Draw();

      // ++++++++++++++++++++++++++++++++++++++++ EMC status +++++++++++++++++++++++++++++++

      EmcStatLabel[2]->Clear();
      EmcStatLabel[2]->SetFillColor(31);
      EmcStatLabel[2]->SetBorderSize(3);
      EmcStatLabel[2]->Draw();

      EmcStatLabel[3]->Clear();
      EmcStatLabel[3]->AddText("Dead or blind");
      EmcStatLabel[3]->SetFillColor(12);
      EmcStatLabel[3]->SetTextColor(20);
      EmcStatLabel[3]->SetBorderSize(1);
      EmcStatLabel[3]->Draw();

      EmcStatLabel[4]->Clear();
      EmcStatLabel[4]->AddText("Noisy ADC");
      EmcStatLabel[4]->SetFillColor(2);
      EmcStatLabel[4]->SetBorderSize(1);
      EmcStatLabel[4]->Draw();

      EmcStatLabel[5]->Clear();
      EmcStatLabel[5]->AddText("Noisy TAC");
      EmcStatLabel[5]->SetFillColor(50);
      EmcStatLabel[5]->SetBorderSize(1);
      EmcStatLabel[5]->Draw();

      EmcStatLabel[6]->Clear();
      EmcStatLabel[6]->AddText("Out of range");
      EmcStatLabel[6]->SetFillColor(5);
      EmcStatLabel[6]->SetBorderSize(1);
      EmcStatLabel[6]->Draw();

      EmcStatLabel[7]->Clear();
      EmcStatLabel[7]->AddText("GOOD");
      EmcStatLabel[7]->SetFillColor(8);
      EmcStatLabel[7]->SetBorderSize(1);
      EmcStatLabel[7]->Draw();

      //
      // the limits 2,5% and 3.5% are checked in the status message to change background and text color to alert the shiftcrew
      // they are in DEAD_ALARM_LIMIT and ADC_ALARM_LIMIT

      EmcStatLabel[8]->Clear();
      EmcStatLabel[8]->AddText("PBGL status:");
      EmcStatLabel[8]->AddText(runstring.c_str());

      EmcStatLabel[10]->Clear();

      //      EmcStatLabel[10]->AddText("Call Expert if any of the following occurs: # of dead or blind channels >= 2.5%,");
      //      EmcStatLabel[10]->AddText("# of noisy ADC or noisy TAC channels >= 3.5%");
       
      runnostream.str("");
      runnostream << "Call Expert if any of the following occurs: # of dead or blind channels >= " << DEAD_ALARM_LIMIT << "%";
      runstring = runnostream.str();
      EmcStatLabel[10]->AddText(runstring.c_str());

      runnostream.str("");
      runnostream << "# of noisy ADC or noisy TAC channels >= " << ADC_ALARM_LIMIT << "%";
      runstring = runnostream.str();
      EmcStatLabel[10]->AddText(runstring.c_str());

      EmcStatLabel[10]->SetTextColor(50);
      EmcStatLabel[10]->SetFillColor(33);
      EmcStatLabel[10]->SetTextAlign(12);
      EmcStatLabel[10]->SetBorderSize(2);
      EmcStatLabel[10]->Draw();

      transparent[1] = new TPad("transparent2", "this does not show", 0, 0, 1, 1 );
      //      transparent[1] = new TPad("transparent2", "this does not show", 0.02, 0.02, 0.98, 0.19);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
      //c_Emc[1]->SetEditable(0);

    }
  else if (!strcmp(name, "EmcMon3"))
    {

      c_Emc[2] = new TCanvas(name, "EMCal pi0 Monitor", -1, 0, (int) xsizeLoc, (int) ysizeLoc / 3);
      gSystem->ProcessEvents();
      c_Emc[2]->cd();
      pad2_EmcPi0[0] = new TPad("PBSC_PiO", "PBSC identified pi0", 0.01, 0.01, 0.49, 0.99, 19);
      pad2_EmcPi0[1] = new TPad("PBGL_PiO", "PBGL identified pi0", 0.51, 0.01, 0.99, 0.99, 19);

      for (int i = 0; i < 2; i++)
        pad2_EmcPi0[i]->Draw();

      // texpi0 = new TLatex();
      // texpi0->SetTextSize(0.04);
      const char* strEMCal[] =
        {
          "PBSC identified pi0", "PBGL identified pi0"
        };
      for (int i = 0; i < 2; i++)
        {
          pad2_EmcPi0[i]->cd();
          EmcLabelPi0[i]->Clear();
          EmcLabelPi0[i]->AddText(strEMCal[i]);
          EmcLabelPi0[i]->SetTextColor(1);
          EmcLabelPi0[i]->SetFillColor(18);
          EmcLabelPi0[i]->SetTextAlign(12);
          EmcLabelPi0[i]->SetBorderSize(2);
          EmcLabelPi0[i]->Draw();
        }
      c_Emc[2]->SetEditable(0);
    }
  else if (!strcmp(name, "EmcMon4"))
    {

      c_Emc[3] = new TCanvas(name, "EMCal pi0 Monitor", -1, (int) ysizeLoc / 3 + 27, (int) xsizeLoc, (int)(ysizeLoc - ysizeLoc / 3));
      gSystem->ProcessEvents();
      c_Emc[3]->cd();
      // pad2_EmcPi0[0] = new TPad("PBSC_PiO", "PBSC identified pi0", 0.01, 0.01, 0.49, 0.95, 21);
      pad2_EmcPi0[2] = new TPad("W0_PiO", "PBSC identified pi0", 0.01, 0.51, 0.24, 0.99, 19);
      pad2_EmcPi0[3] = new TPad("W1_PiO", "PBSC identified pi0", 0.25, 0.51, 0.48, 0.99, 19);
      pad2_EmcPi0[4] = new TPad("W2_PiO", "PBSC identified pi0", 0.49, 0.51, 0.72, 0.99, 19);
      pad2_EmcPi0[5] = new TPad("E0_PiO", "PBGL identified pi0", 0.76, 0.51, 0.99, 0.99, 19);

      pad2_EmcPi0[6] = new TPad("W3_PiO", "PBSC identified pi0", 0.01, 0.01, 0.24, 0.49, 19);
      pad2_EmcPi0[7] = new TPad("E2_PiO", "PBSC identified pi0", 0.25, 0.01, 0.48, 0.49, 19);
      pad2_EmcPi0[8] = new TPad("E3_PiO", "PBSC identified pi0", 0.49, 0.01, 0.72, 0.49, 19);
      pad2_EmcPi0[9] = new TPad("E1_PiO", "PBGL identified pi0", 0.76, 0.01, 0.99, 0.49, 19);
      for (int i = 2; i < 10; i++)
        pad2_EmcPi0[i]->Draw();

      const char* strEMCalSect[] =
        {"PBSC W0 identified pi0", "PBSC W1 identified pi0", "PBSC W2 identified pi0", "PBGL E0 identified pi0",
         "PBSC W3 identified pi0", "PBSC E2 identified pi0", "PBSC E3 identified pi0", "PBGL E1 identified pi0"
        };
      for (int i = 2; i < 10; i++)
        {
          pad2_EmcPi0[i]->cd();
          EmcLabelPi0[i]->Clear();
          EmcLabelPi0[i]->AddText(strEMCalSect[i - 2]);
          EmcLabelPi0[i]->SetTextColor(1);
          EmcLabelPi0[i]->SetFillColor(18);
          EmcLabelPi0[i]->SetTextAlign(12);
          EmcLabelPi0[i]->SetBorderSize(2);
          EmcLabelPi0[i]->Draw();
        }

      c_Emc[3]->SetEditable(0);
    }

  return 0;
}


int EMCalMonDraw::Draw(const char *what)
{

  int iret = 0;
  int idraw = 0;
  TStyle* oldStyle = gStyle;

  if (!strcmp(what, "ALL") || !strcmp(what, "FIRST") || !strcmp(what, "SHIFT"))
    {
      iret += DrawEmcMon1(what);
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "SECOND") || !strcmp(what, "SHIFT"))
    {
      iret += DrawEmcMon2(what);
      idraw ++;
    }

  if (!strcmp(what, "ALL") || !strcmp(what, "THIRD") || !strcmp(what, "PI0"))
    {
      iret += DrawEmcMon3(what);
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "FOURTH") || !strcmp(what, "PI0"))
    {
      iret += DrawEmcMon4(what);
      idraw ++;
    }
  oldStyle->cd();

  if (!idraw)
    {
      iret = -1;
    }
  return iret;

}



int EMCalMonDraw::DrawEmcMon1(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *EMCal_Info = cl->getHisto("EMCal_Info");

  normPbSc_sm->Reset();
  PbSc_tp->Reset();

  if (!gROOT->FindObject("EmcMon1"))
    {
      MakeCanvas("EmcMon1");
    }
  c_Emc[0]->SetEditable(1);

  for (int i = 0; i < 4; i++)
    pad_PbGl[i]->Clear("D");
  for (int i = 0; i < 6; i++)
    pad_PbSc[i]->Clear("D");


  if (!EMCal_Info)
    {
      statDeadServer[0] = 1;
      //   pad_PbSc[2]->cd();
      // PbSc_tp->Draw();
      //normPbSc_sm->Draw();
      pad_PbSc[5]->cd();
      PbSc_tp->Draw();
      DrawDeadServer(transparent[0]);
      // c_Emc[0]->Modified();
      // c_Emc[0]->Update();
      c_Emc[0]->SetEditable(0);
      return -1;
    }

  if (statDeadServer[0])
    {
      transparent[0]->cd();
      transparent[0]->Clear();
      transparent[0]->Update();
      statDeadServer[0] = 0;
    }

  int RefEvt = (int) EMCal_Info->GetBinContent(1);

  TH1 *PbGl_sm = cl->getHisto("PBGL_sm");
  TH1 *PbGl_ref = cl->getHisto("PBGL_ref");
  TH1 *PbGl_T_ref = cl->getHisto("PBGL_T_ref");
  TH1 *PbGl_T_sm = cl->getHisto("PBGL_T_sm");

  EmcStyle->cd();
  EmcStyle->SetOptStat(1);
  EmcStyle->SetStatY(0.9); 
  EmcStyle->SetStatX(0.9); 
  EmcStyle->SetStatW(0.25); 
  EmcStyle->SetStatH(0.25); 
  // =======================   Draw PBGL  result =======================
  if (PbGl_ref->GetEntries() <= 0)
    {
      pad_PbGl[0]->SetLogy(0);
    }
  else
    {
      pad_PbGl[0]->SetLogy(1);
      pad_PbGl[0]->cd();
      PbGl_ref->SetMinimum(0.2);
      PbGl_ref->GetXaxis()->SetLabelSize(0.05);
      PbGl_ref->GetXaxis()->SetTitleSize(0.055);
      PbGl_ref->GetXaxis()->SetTitleOffset(0.9);
      PbGl_ref->GetYaxis()->SetLabelSize(0.05);
      PbGl_ref->DrawCopy();
      arrowPbgl[0]->Draw();
    }

  if (PbGl_sm->GetEntries() <= 0)
    {
      pad_PbGl[1]->SetLogy(0);
    }
  else
    {
      pad_PbGl[1]->SetLogy(1);
      pad_PbGl[1]->cd();
      PbGl_sm->SetMinimum(0.2);
      PbGl_sm->GetXaxis()->SetLabelSize(0.05);
      PbGl_sm->GetXaxis()->SetTitleSize(0.055);
      PbGl_sm->GetXaxis()->SetTitleOffset(0.9);
      PbGl_sm->GetYaxis()->SetLabelSize(0.05);
      PbGl_sm->DrawCopy();
      arrowPbgl[1]->Draw();
    }

  if (PbGl_T_ref->GetEntries() <= 0)
    {
      pad_PbGl[2]->SetLogy(0);
    }
  else
    {
      pad_PbGl[2]->SetLogy(1);
      pad_PbGl[2]->cd();
      PbGl_T_ref->SetMinimum(0.2);
      PbGl_T_ref->GetXaxis()->SetLabelSize(0.05);
      PbGl_T_ref->GetXaxis()->SetTitleSize(0.055);
      PbGl_T_ref->GetXaxis()->SetTitleOffset(0.9);
      PbGl_T_ref->GetYaxis()->SetLabelSize(0.05);
      PbGl_T_ref->DrawCopy();
      arrowPbgl[2]->Draw();
    }

  if (PbGl_T_sm->GetEntries() <= 0)
    {
      pad_PbGl[3]->SetLogy(0);
    }
  else
    {
      pad_PbGl[3]->SetLogy(1);
      pad_PbGl[3]->cd();
      PbGl_T_sm->SetMinimum(0.2);
      PbGl_T_sm->GetXaxis()->SetLabelSize(0.05);
      PbGl_T_sm->GetXaxis()->SetTitleSize(0.055);
      PbGl_T_sm->GetXaxis()->SetTitleOffset(0.9);
      PbGl_T_sm->GetYaxis()->SetLabelSize(0.05);
      PbGl_T_sm->DrawCopy();
      arrowPbgl[3]->Draw();
    }

  //Draw PBSC  result ==========================================================================
  TH1 *PbSc_sm = cl->getHisto("PBSC_sm");
  TH1 *PbSc_ref = cl->getHisto("PBSC_ref");
  TH1 *PbSc_T_ref = cl->getHisto("PBSC_T_ref");
  TH1 *PbSc_T_sm = cl->getHisto("PBSC_T_sm");

  TH1 *PbSc_TP = cl->getHisto("PBSC_TP");
  TH1 *Refer_TP = cl->getHisto("Refer_TP_PBSC");


  if (RefEvt)
    {
      TH1 *avPbSc_SM = cl->getHisto("avPBSC_SM");
      TH1 *avPbSc_REF = cl->getHisto("avPBSC_REF");
      TH1 *Refer_PBSC = cl->getHisto("Refer_PBSC");
      TH1 *ReferPin_PBSC = cl->getHisto("ReferPin_PBSC");

      for (int ip = 0; ip < 108; ip++)
        {
          float rAmpl_t, rAmpl_t0;
          float rAmpl_ratio = 0.0;
          if (ip == 26)
            {
              rAmpl_t = avPbSc_REF->GetBinContent(avPbSc_REF->FindBin(ip - 1));
              rAmpl_t0 = ReferPin_PBSC->GetBinContent(ReferPin_PBSC->FindBin(ip - 1));

            }
          else
            {
              rAmpl_t = avPbSc_REF->GetBinContent(avPbSc_REF->FindBin(ip));
              rAmpl_t0 = ReferPin_PBSC->GetBinContent(ReferPin_PBSC->FindBin(ip));
            }

          if (rAmpl_t != 0)
            {
              rAmpl_ratio = rAmpl_t0 / rAmpl_t;
            }

          for (int ich = 0; ich < 144; ich++)
            {
              int ch = ip * 144 + ich;
              float Ampl_t = avPbSc_SM->GetBinContent(avPbSc_SM->FindBin(ch));
              float Ampl_t0 = Refer_PBSC->GetBinContent(Refer_PBSC->FindBin(ch));
              float Ampl_ratio = 0.0;
              if (Ampl_t0 != 0)
                {
                  Ampl_ratio = Ampl_t / Ampl_t0;
                }
              normPbSc_sm->Fill(rAmpl_ratio * Ampl_ratio);

            }
        }

    }
  int RefEvtTp = (int) EMCal_Info->GetBinContent(2);
  if (RefEvtTp) //test pulse
    {
      for (int IP = 0; IP < 108; IP++)
        {
          int nch = Refer_TP->FindBin(IP);
          float Ampl = Refer_TP->GetBinContent(nch);

          if (Ampl)
            {
              PbSc_tp->Fill(PbSc_TP->GetBinContent(nch) / Ampl);
            }
        }
    }

  if (PbSc_ref->GetEntries() <= 0)
    {
      pad_PbSc[0]->SetLogy(0);
    }
  else
    {
      pad_PbSc[0]->SetLogy(1);
      pad_PbSc[0]->cd();
      PbSc_ref->SetMinimum(0.2);
      PbSc_ref->GetXaxis()->SetLabelSize(0.05);
      PbSc_ref->GetXaxis()->SetTitleSize(0.055);
      PbSc_ref->GetXaxis()->SetTitleOffset(0.9);
      PbSc_ref->GetYaxis()->SetLabelSize(0.05);
      PbSc_ref->DrawCopy();
      // arrowPbsc[0]->Draw();
    }

  if (PbSc_sm->GetEntries() <= 0)
    {
      pad_PbSc[1]->SetLogy(0);
    }
  else
    {
      pad_PbSc[1]->SetLogy(1);
      pad_PbSc[1]->cd();
      PbSc_sm->SetMinimum(0.2);
      PbSc_sm->GetXaxis()->SetLabelSize(0.05);
      PbSc_sm->GetXaxis()->SetTitleSize(0.055);
      PbSc_sm->GetXaxis()->SetTitleOffset(0.9);
      PbSc_sm->GetYaxis()->SetLabelSize(0.05);
      PbSc_sm->DrawCopy();
      // arrowPbsc[1]->Draw();
    }

  if (normPbSc_sm->GetEntries() <= 0)
    {
      pad_PbSc[2]->SetLogy(0);
    }
  else
    {
      pad_PbSc[2]->SetLogy(1);
      pad_PbSc[2]->cd();
      normPbSc_sm->SetMinimum(0.2);
      normPbSc_sm->GetXaxis()->SetLabelSize(0.05);
      normPbSc_sm->GetXaxis()->SetTitleSize(0.055);
      normPbSc_sm->GetXaxis()->SetTitleOffset(0.9);
      normPbSc_sm->GetYaxis()->SetLabelSize(0.05);
      normPbSc_sm->DrawCopy();
      arrowPbsc[2]->Draw();
    }

  if (PbSc_T_ref->GetEntries() <= 0)
    {
      pad_PbSc[3]->SetLogy(0);
    }
  else
    {
      pad_PbSc[3]->SetLogy(1);
      pad_PbSc[3]->cd();
      PbSc_T_ref->SetMinimum(0.2);
      PbSc_T_ref->GetXaxis()->SetLabelSize(0.05);
      PbSc_T_ref->GetXaxis()->SetTitleSize(0.055);
      PbSc_T_ref->GetXaxis()->SetTitleOffset(0.9);
      PbSc_T_ref->GetYaxis()->SetLabelSize(0.05);
      PbSc_T_ref->DrawCopy();
      arrowPbsc[3]->Draw();
    }

  if (PbSc_T_sm->GetEntries() <= 0)
    {
      pad_PbSc[4]->SetLogy(0);
    }
  else
    {
      pad_PbSc[4]->SetLogy(1);
      pad_PbSc[4]->cd();
      PbSc_T_sm->SetMinimum(0.2);
      PbSc_T_sm->GetXaxis()->SetLabelSize(0.05);
      PbSc_T_sm->GetXaxis()->SetTitleSize(0.055);
      PbSc_T_sm->GetXaxis()->SetTitleOffset(0.9);
      PbSc_T_sm->GetYaxis()->SetLabelSize(0.05);
      PbSc_T_sm->DrawCopy();
      arrowPbsc[4]->Draw();
    }

  if (RefEvtTp)
    {

      if (PbSc_tp->GetEntries() <= 0)
        {
          pad_PbSc[5]->SetLogy(0);
        }
      else
	{
	  pad_PbSc[5]->SetLogy(1);
	  pad_PbSc[5]->cd();
	  PbSc_tp->SetMinimum(0.2);
	  PbSc_tp->GetXaxis()->SetLabelSize(0.05);
	  PbSc_tp->GetXaxis()->SetTitleSize(0.055);
          PbSc_tp->GetXaxis()->SetTitleOffset(0.9);
	  PbSc_tp->GetYaxis()->SetLabelSize(0.05);
	  PbSc_tp->DrawCopy();
	  arrowPbsc[5]->Draw();
        }
    }

  ostringstream runnostream;
  string runstring;

  time_t evttime = cl->EventTime("CURRENT");
  runnostream << "Run# " << cl->RunNumber();
  runstring = runnostream.str();
  EmcLabel[3]->Clear();
  EmcLabel[3]->AddText(runstring.c_str());

  runnostream.str("");
  runnostream << "Time: " << ctime(&evttime);
  runstring = runnostream.str();
  EmcLabel[3]->AddText(runstring.c_str());

  runnostream.str("");
  runnostream << "Events total: " << (int) EMCal_Info->GetBinContent(0)
	      << ", ref: " << RefEvt;
  runstring = runnostream.str();
  EmcLabel[4]->Clear();

  EmcLabel[4]->AddText(0.02, 0.96, runstring.c_str());
  EmcLabel[4]->AddText(0.02, 0.90, "Lost packets :");


  if (RefEvt)
    {
      float numbPack, tempPack = 0;
      float sX = 0.02, sY = 0.89;
      int IPtemp = 0, sumPack = 0;
      int chan;

      for (int IP = 0; IP < 180; IP++)
        {
          chan = EMCal_Info->FindBin(IP + 10);
          numbPack = EMCal_Info->GetBinContent(chan);

          if (numbPack > 0.0 )
            {
              sumPack++;
              IPtemp++;
              if (sY < 0.0)
                continue;
              if (numbPack == tempPack && IP == IPtemp )
                {
                  if (numbPack != EMCal_Info->GetBinContent(EMCal_Info->FindBin(IP + 11)) )
                    {
                      runnostream.str("");
                      runnostream << " - " << setw(3) << chan - 11;
                      runstring = runnostream.str();
                      EmcLabel[4]->AddText(sX + 0.35, sY, runstring.c_str());
                    }
                }
              else
                {
                  tempPack = numbPack;
                  IPtemp = IP;
                  runnostream.str("");
                  runnostream << setw(6) << setprecision(3) << (numbPack / RefEvt)*100.0 << "% :";
                  runnostream << setw(3) << chan - 11;
                  runstring = runnostream.str();
                  sY -= 0.05;
                  EmcLabel[4]->AddText(sX, sY, runstring.c_str());
                }

            } // if numb packet

        }
      if (sumPack)
        {
          runnostream.str("");
          runnostream << "Yes, sum=" << sumPack;
          runstring = runnostream.str();
          EmcLabel[4]->AddText(0.50, 0.90, runstring.c_str());
        }
      else
        EmcLabel[4]->AddText(0.50, 0.90, "No");

    }
  c_Emc[0]->cd();

  for (int i = 3; i < 5; i++)
    {
      EmcLabel[i]->Draw();
    }
  c_Emc[0]->Update();
  c_Emc[0]->SetEditable(0);
  return 0;
}

int EMCalMonDraw::DrawEmcMon2(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *EMCal_Info = cl->getHisto("EMCal_Info");

  if (!gROOT->FindObject("EmcMon2"))
    {
      MakeCanvas("EmcMon2");
    }

  if (!EMCal_Info)
    {
      statDeadServer[1] = 1;
      // PbGl_Stat_GOOD->Reset();
      pad1_PbGl_Stat->SetFrameFillColor(21);
      pad1_PbGl_Stat->cd();
      // PbGl_Stat_GOOD->Draw("box");
      drawBoxGrid(PbGl_Stat_PROBL, lGrid);

      // PbSc_Stat_GOOD->Reset();
      pad1_PbSc_Stat->SetFrameFillColor(21);
      pad1_PbSc_Stat->cd();
      // PbSc_Stat_GOOD->Draw("box");
      drawBoxGrid(PbSc_Stat_PROBL, lGrid);

      // c_Emc[1]->Modified();
      // c_Emc[1]->Update();

      DrawDeadServer(transparent[1]);
      return -1;
    }

  if (statDeadServer[1])
    {

      transparent[1]->cd();
      transparent[1]->Clear();
      transparent[1]->Update();
      statDeadServer[1] = 0;
    }

  EmcStyle->cd();
  EmcStyle->SetOptStat(0);
  c_Emc[1]->cd();

  ostringstream runnostream;
  string runstring;

  time_t evttime = cl->EventTime("CURRENT");
  runnostream << "Run# " << cl->RunNumber() << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  EmcStatLabel[2]->Clear();
  EmcStatLabel[2]->AddText(runstring.c_str());
  EmcStatLabel[2]->Draw();
  runnostream.str("");


  int refEvt = (int) EMCal_Info->GetBinContent(1);
  if (refEvt < REF_EVENTMIN)
    {
      runnostream << "Laser events has too few: " << refEvt;
      runstring = runnostream.str();

      EmcStatLabel[8]->Clear();
      EmcStatLabel[8]->AddText("PBGL status:");
      EmcStatLabel[8]->AddText(runstring.c_str());
      PbGl_Stat_PROBL->Reset();
      pad1_PbGl_Stat->SetFrameFillColor(21);
      pad1_PbGl_Stat->cd();
      EmcStatLabel[8]->Draw();
      PbGl_Stat_PROBL->Draw("box");
      drawBoxGrid(PbGl_Stat_PROBL, lGrid);
      pad1_PbGl_Stat->Update();

      EmcStatLabel[9]->Clear();
      EmcStatLabel[9]->AddText("PBSC status:");
      EmcStatLabel[9]->AddText(runstring.c_str());
      PbSc_Stat_PROBL->Reset();
      pad1_PbSc_Stat->SetFrameFillColor(21);
      pad1_PbSc_Stat->cd();
      EmcStatLabel[9]->Draw();
      PbSc_Stat_PROBL->Draw("box");
      drawBoxGrid(PbSc_Stat_PROBL, lGrid);
      c_Emc[1]->Update();

      return 0;
    }

  int Femlinear, SectorFEM, iST;
  int ix, iy, nChan;
  float avADC, avTAC;
  float sum, averageAmpl;
  int valid;
  float rmsADC, rmsTAC, rmsADCRel;
  bool deadL, noiseADC, noiseTAC, outADC, outTAC;

  int EmcStatus[2][3];
  // memset(EmcStatus, 0, sizeof(int)*3*2);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      EmcStatus[i][j] = 0;

  // TLine Grid;
  // +++++++++++++++++++++++++++++++++++++ PBGL +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  TH1 *avPbGl_SM = cl->getHisto("avPBGL_SM");
  TH1 *avPbGl_T_SM = cl->getHisto("avPBGL_T_SM");
  TH1 *dispPbGl_SM = cl->getHisto("dispPBGL_SM");
  TH1 *dispPbGl_T_SM = cl->getHisto("dispPBGL_T_SM");

  // PbGl_Stat_GOOD->Reset();
  PbGl_Stat_PROBL->Reset();
  PbGl_Stat_Blind->Reset();
  PbGl_Stat_NoiseADC->Reset();
  PbGl_Stat_NoiseTAC->Reset();

  // PbGl_Stat_GOOD->SetLineColor(8);
  // PbGl_Stat_GOOD->SetFillColor(8);
  PbGl_Stat_PROBL->SetLineColor(5);
  PbGl_Stat_PROBL->SetFillColor(5);
  PbGl_Stat_Blind->SetLineColor(12);
  PbGl_Stat_Blind->SetFillColor(12);
  PbGl_Stat_NoiseADC->SetLineColor(2);
  PbGl_Stat_NoiseADC->SetFillColor(2);
  PbGl_Stat_NoiseTAC->SetLineColor(50);
  PbGl_Stat_NoiseTAC->SetFillColor(50);

  sum = 0;
  valid = 0;
  for (int IP = 0; IP < 9216; IP++)
    {
      nChan = avPbGl_SM->FindBin(IP);
      avADC = avPbGl_SM->GetBinContent(nChan);
      if (avADC >= 10)
        {
          sum += avADC;
          valid++;
        }
    }
  if (valid)
    averageAmpl = sum / valid;
  else
    averageAmpl = 1.0;

  // cout << "PBGL averageAmpl" << averageAmpl << endl;

  //64*144
  for (int IP = 0; IP < 9216; IP++)
    {
      Femlinear = iCHiSMT(IP % 144);
      SectorFEM = IP / 144;
      iST = SMiSMTiST(SectorFEM, Femlinear, 2);
      ix = iST % 96;
      iy = iST / 96;
      nChan = avPbGl_SM->FindBin(IP);
      avADC = avPbGl_SM->GetBinContent(nChan);
      nChan = avPbGl_T_SM->FindBin(IP);
      avTAC = avPbGl_T_SM->GetBinContent(nChan);
      nChan = dispPbGl_SM->FindBin(IP);
      rmsADC = sqrt(dispPbGl_SM->GetBinContent(nChan));
      nChan = dispPbGl_T_SM->FindBin(IP);
      rmsTAC = sqrt(dispPbGl_T_SM->GetBinContent(nChan));
      deadL = false;
      noiseADC = false;
      outADC = false;
      noiseTAC = false;
      outTAC = false;


      // PbGl_Stat_GOOD->Fill(ix, iy);
      if (avADC < averageAmpl*PBGL_OUT_ADCMIN || avADC > averageAmpl*PBGL_OUT_ADCMAX)
        {
          outADC = true;
        }
      //  if (rmsTAC > 0.0 && (avTAC < PBGL_OUT_TACMIN || avTAC > PBGL_OUT_TACMAX))
      if (avTAC < PBGL_OUT_TACMIN || avTAC > PBGL_OUT_TACMAX) 
        {
          outTAC = true;
        }
      if (outADC || outTAC)
        {
          PbGl_Stat_PROBL->Fill(ix, iy);
        }
      if (rmsTAC > PBGL_RMS_TACMAX)
        {
          PbGl_Stat_NoiseTAC->Fill(ix, iy);
          noiseTAC = true;
          EmcStatus[0][2]++;
        }

      if (avADC < PBGL_ADCMIN)
        {
          deadL = true;
          PbGl_Stat_Blind->Fill(ix, iy);
          rmsADCRel = 0.499;
          EmcStatus[0][0]++;
        }
      else
        {
          rmsADCRel = rmsADC / avADC;
          if (rmsADCRel > PBGL_RMS_ADCMAX)
            {
              PbGl_Stat_NoiseADC->Fill(ix, iy);
              noiseADC = true;
              EmcStatus[0][1]++;
            }
        }


    }

  pad1_PbGl_Stat->cd();
  pad1_PbGl_Stat->SetFrameFillColor(8);
  // PbGl_Stat_PROBL->Draw("box");
  // PbGl_Stat_GOOD->Draw("box same");
  PbGl_Stat_PROBL->Draw("box");
  PbGl_Stat_NoiseTAC->Draw("box same");
  PbGl_Stat_NoiseADC->Draw("box same");
  PbGl_Stat_Blind->Draw("box same");
  drawBoxGrid(PbGl_Stat_PROBL, lGrid);

  // +++++++++++++++++++++++++++++++++++++ PBSC +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  TH1 *avPbSc_SM = cl->getHisto("avPBSC_SM");
  TH1 *avPbSc_T_SM = cl->getHisto("avPBSC_T_SM");
  TH1 *dispPbSc_SM = cl->getHisto("dispPBSC_SM");
  TH1 *dispPbSc_T_SM = cl->getHisto("dispPBSC_T_SM");

  // PbSc_Stat_GOOD->Reset();
  PbSc_Stat_PROBL->Reset();
  PbSc_Stat_Blind->Reset();
  PbSc_Stat_NoiseADC->Reset();
  PbSc_Stat_NoiseTAC->Reset();

  // PbSc_Stat_GOOD->SetLineColor(8);
  // PbSc_Stat_GOOD->SetFillColor(8);
  PbSc_Stat_PROBL->SetLineColor(5);
  PbSc_Stat_PROBL->SetFillColor(5);
  PbSc_Stat_Blind->SetLineColor(12);
  PbSc_Stat_Blind->SetFillColor(12);
  PbSc_Stat_NoiseADC->SetLineColor(2);
  PbSc_Stat_NoiseADC->SetFillColor(2);
  PbSc_Stat_NoiseTAC->SetLineColor(50);
  PbSc_Stat_NoiseTAC->SetFillColor(50);

  sum = 0;
  valid = 0;
  for (int IP = 0; IP < 15552; IP++)
    {
      nChan = avPbSc_SM->FindBin(IP);
      avADC = avPbSc_SM->GetBinContent(nChan);
      if (avADC >= 4)
        {
          sum += avADC;
          valid++;
        }
    }
  if (valid)
    averageAmpl = sum / valid;
  else
    averageAmpl = 1.0;

  //cout << "PBSc averageAmpl" << averageAmpl << endl;
  //108*144
  for (int IP = 0; IP < 15552; IP++)
    {

      Femlinear = iCHiSMT(IP % 144);
      SectorFEM = IP / 144;
      iST = SMiSMTiST(SectorFEM, Femlinear, 1);
      ix = iST % 72;
      iy = iST / 72;
      nChan = avPbSc_SM->FindBin(IP);
      avADC = avPbSc_SM->GetBinContent(nChan);
      nChan = avPbSc_T_SM->FindBin(IP);
      avTAC = avPbSc_T_SM->GetBinContent(nChan);
      nChan = dispPbSc_SM->FindBin(IP);
      rmsADC = sqrt(dispPbSc_SM->GetBinContent(nChan));
      nChan = dispPbSc_T_SM->FindBin(IP);
      rmsTAC = sqrt(dispPbSc_T_SM->GetBinContent(nChan));
      deadL = false;
      noiseADC = false;
      outADC = false;
      noiseTAC = false;
      outTAC = false;


      //  PbSc_Stat_GOOD->Fill(ix, iy);
      if (avADC < averageAmpl*PBSC_OUT_ADCMIN || avADC > averageAmpl*PBSC_OUT_ADCMAX)
        {
          outADC = true;
        }
      // if (rmsTAC > 0.0 && (avTAC < PBSC_OUT_TACMIN || avTAC > PBSC_OUT_TACMAX))
      if (avTAC < PBSC_OUT_TACMIN || avTAC > PBSC_OUT_TACMAX)  
        {
          outTAC = true;
        }
      if (outADC || outTAC)
        {
          PbSc_Stat_PROBL->Fill(ix, iy);
        }
      if (rmsTAC > PBSC_RMS_TACMAX)
        {
          PbSc_Stat_NoiseTAC->Fill(ix, iy);
          noiseTAC = true;
          EmcStatus[1][2]++;
        }

      if (avADC < PBSC_ADCMIN)
        {
          deadL = true;
          PbSc_Stat_Blind->Fill(ix, iy);
          rmsADCRel = 0.499;
          EmcStatus[1][0]++;
        }
      else
        {
          rmsADCRel = rmsADC / avADC;
          if (rmsADCRel > PBSC_RMS_ADCMAX)
            {
              PbSc_Stat_NoiseADC->Fill(ix, iy);
              noiseADC = true;
              EmcStatus[1][1]++;
            }
        }

    }

  pad1_PbSc_Stat->cd();
  pad1_PbSc_Stat->SetFrameFillColor(8);
  // PbSc_Stat_PROBL->Draw("box");
  // PbSc_Stat_GOOD->Draw("box same");
  PbSc_Stat_PROBL->Draw("box");
  PbSc_Stat_NoiseTAC->Draw("box same");
  PbSc_Stat_NoiseADC->Draw("box same");
  PbSc_Stat_Blind->Draw("box same");
  drawBoxGrid(PbSc_Stat_PROBL, lGrid);

  EmcStatLabel[8]->Clear();
  if( ((EmcStatus[0][0] / 9216.0)*100.0 >= DEAD_ALARM_LIMIT) || ((EmcStatus[0][1] / 9216.0)*100.0 >= ADC_ALARM_LIMIT) || ((EmcStatus[0][2] / 9216.0)*100.0 >= ADC_ALARM_LIMIT)){
      EmcStatLabel[8]->SetFillColor(kPink+9);
      EmcStatLabel[8]->SetTextColor(kWhite);
      EmcStatLabel[8]->AddText("PBGL status: Please call expert");
  }
  else{
      EmcStatLabel[8]->AddText("PBGL status:");
  }

  runnostream.str("");
  runnostream << "Dead or blind channels - " << setw(6) << setprecision(3);
  runnostream << (EmcStatus[0][0] / 9216.0)*100.0 << "%";
  runstring = runnostream.str();
  EmcStatLabel[8]->AddText(runstring.c_str());

  runnostream.str("");
  runnostream << "Noisy ADC  - " << setw(6) << setprecision(3);
  runnostream << (EmcStatus[0][1] / 9216.0)*100.0 << "%, noisy TAC - ";
  runnostream << (EmcStatus[0][2] / 9216.0)*100.0 << "%";
  runstring = runnostream.str();
  EmcStatLabel[8]->AddText(runstring.c_str());
  // EmcStatLabel[8]->Draw();

  EmcStatLabel[9]->Clear();
  if( ((EmcStatus[1][0] / 15552.0)*100.0 >= DEAD_ALARM_LIMIT) || ((EmcStatus[1][1] / 15552.0)*100.0 >= ADC_ALARM_LIMIT) || ((EmcStatus[1][2] / 15552.0)*100.0 >= ADC_ALARM_LIMIT) ) {
      EmcStatLabel[9]->SetFillColor(kPink+9);
      EmcStatLabel[9]->SetTextColor(kWhite);
      EmcStatLabel[9]->AddText("PBSC status: Please call expert");
  }
  else{
      EmcStatLabel[9]->AddText("PBSC status:");
  }

  runnostream.str("");
  runnostream << "Dead or blind channels - " << setw(6) << setprecision(3);
  runnostream << (EmcStatus[1][0] / 15552.0)*100.0 << "%";
  runstring = runnostream.str();
  EmcStatLabel[9]->AddText(runstring.c_str());

  runnostream.str("");
  runnostream << "Noisy ADC  - " << setw(6) << setprecision(3);
  runnostream << (EmcStatus[1][1] / 15552.0)*100.0 << "%, noisy TAC - ";
  runnostream << (EmcStatus[1][2] / 15552.0)*100.0 << "%";
  runstring = runnostream.str();
  EmcStatLabel[9]->AddText(runstring.c_str());
  // EmcStatLabel[9]->Draw();
  // c_Emc[1]->Modified();

  c_Emc[1]->Update();

  return 0;

}


int EMCalMonDraw::DrawEmcMon3(const char *what)
{
  TH1 *mchist[2][2];
  OnlMonClient *cl = OnlMonClient::instance();

  const char *mchname[2][2] = {
    {"BBCLL1_PBSC_2_6GeV_c", "BBCLL1_PBGL_2_6GeV_c" },
    {"Gamma3_PBSC_2_6GeV_c", "Gamma3_PBGL_2_6GeV_c" }
  };
  const char *mchtitle[2][2] = {
    {"BBCLL1_PBSC_2_6GeV_c", "BBCLL1_PBGL_2_6GeV_c"},
    {"ERTLL1_4x4_BBCLL1_PBSC_2_6GeV_c", "ERTLL1_4x4_BBCLL1_PBGL_2_6GeV_c"}
  }; 
  
  for (int itr = 0; itr < 2; itr++)  //itr: 0= BBCLL1, 1= Gamma3 & BBCLL1
    {
      for (int ih = 0; ih < 2; ih++)
        {
          mchist[itr][ih] = cl->getHisto(mchname[itr][ih]);
	// check if histo exists (if this gets called by
	// the html for another subsystem, all pointers
	// are 0 and it crashes when setting the NameTitle
	  if (!mchist[itr][ih])
	    {
	      return -1;
	    }
          mchist[itr][ih]->SetNameTitle(mchname[itr][ih], mchtitle[itr][ih]);  
        }
    }
  if (!gROOT->FindObject("EmcMon3"))
    {
      MakeCanvas("EmcMon3");
    }
  c_Emc[2]->SetEditable(1);
  c_Emc[2]->Clear("D");

  for (int ih = 0; ih < 2; ih++)
    {
      mchist[fTr][ih]->SetLineColor(30);
      mchist[fTr][ih]->SetLineWidth(2);
      mchist[fTr][ih]->SetMarkerStyle(24);
      mchist[fTr][ih]->SetMarkerSize(0.2);
      mchist[fTr][ih]->SetMarkerColor(3);
    }

  EmcStyle->cd();
  EmcStyle->SetOptStat(0);

  TSpectr *spec = new TSpectr(10);
  const char* strEMCal[] =
    {
      "PBSC identified pi0",
      "PBGL identified pi0"
    };


  for (int i = 0; i < 2; i++)
    {
      pad2_EmcPi0[i]->cd();
      EmcLabelPi0[i]->Clear();
      EmcLabelPi0[i]->AddText(strEMCal[i]);
      TH1 *mccopy = (TH1*) mchist[fTr][i]->Clone();
      int stat = 0;
      double xp, yp = 0, ypl, ypr, polV = 0;
      m_inv[i] = 0;
      errm_inv[i] = 0;
      sigm_inv[i] = 0;
      errsigm_inv[i] = 0;
      int npeak = spec->Search(mccopy, 7);
      // int npeak = spec->Search(mccopy, 9);
      if (npeak)
        {
          float *xpeaks = spec->GetPositionX();
          for (int ip = 0; ip < npeak; ip++)
            {
              if (xpeaks[ip] > 0.1 && xpeaks[ip] < 0.2)
                {
                  xp = xpeaks[ip];
                  yp = mccopy->GetBinContent(mccopy->GetXaxis()->FindBin(xp));
                  ypl = mccopy->GetBinContent(mccopy->GetXaxis()->FindBin(xp - 0.01));
                  ypr = mccopy->GetBinContent(mccopy->GetXaxis()->FindBin(xp + 0.01));
                  fpol2->SetParameters(100, 0.1, 0.1);
                  mccopy->Fit("fpol2", "qn", "RM", 0.07, 0.25);
                  polV = fpol2->Eval(xp);
                  if ((yp - sqrt(yp) > polV) && (ypl - sqrt(ypl) > fpol2->Eval(xp - 0.01)) &&
                      (ypr - sqrt(ypr) > fpol2->Eval(xp + 0.01)))
                    stat = 1;
                  break;
                }
            }

        }
      if (stat)
        {
          fitp0->SetParameters(100, 0.1, 0.1, yp - polV, 0.148, 0.02);
          fitp0->SetParLimits(3, 10.0, 10000000.0);
          fitp0->SetParLimits(4, 0.07, 0.25);
          fitp0->SetParLimits(5, 0.005, 0.05);
          mchist[fTr][i]->Fit("fitp0", "sames", "RM", 0.07, 0.25);
          fpol2->SetParameters(fitp0->GetParameter(0), fitp0->GetParameter(1), fitp0->GetParameter(2));
          mchist[fTr][i]->DrawCopy();
          fpol2->DrawCopy("same");
          m_inv[i] = fitp0->GetParameter(4);
          errm_inv[i] = fitp0->GetParError(4);
          sigm_inv[i] = fitp0->GetParameter(5);
          errsigm_inv[i] = fitp0->GetParError(5);

          EmcLabelPi0[i]->AddText( Form("chi2/ndf %5.4g", fitp0->GetChisquare() / fitp0->GetNDF()) );
          EmcLabelPi0[i]->AddText( Form("const %5.4g±%5.4g", fitp0->GetParameter(3), fitp0->GetParError(3)) );
          EmcLabelPi0[i]->AddText( Form("mean %5.4g±%5.4g", m_inv[i], errm_inv[i]) );
          EmcLabelPi0[i]->AddText( Form("sigma %5.4g±%5.4g", sigm_inv[i], errsigm_inv[i]) );
          EmcLabelPi0[i]->Draw();

        }
      else
        {
          mchist[fTr][i]->DrawCopy();
          EmcLabelPi0[i]->AddText( Form("entry %g", mchist[fTr][i]->GetEntries()) );
          EmcLabelPi0[i]->AddText( Form("mean %5.4g", mchist[fTr][i]->GetMean()) );
          EmcLabelPi0[i]->AddText( Form("rms %5.4g", mchist[fTr][i]->GetRMS()) );
          EmcLabelPi0[i]->Draw();
        }
      drawLimitHist(0.1, 0.2, mchist[fTr][i], lGrid);
    }

  EmcStyle->SetOptStat(0);
  c_Emc[2]->Update();
  c_Emc[2]->SetEditable(0);

  delete spec;

  return 0;
}


int EMCalMonDraw::DrawEmcMon4(const char *what)
{

  TH1 *mchist[2][8];
  OnlMonClient *cl = OnlMonClient::instance();

  const char *mchname[2][8] = {
    {"BBCLL1_PBSC_W0_2_6GeV_c", "BBCLL1_PBSC_W1_2_6GeV_c", "BBCLL1_PBSC_W2_2_6GeV_c", "BBCLL1_PBGL_E0_2_6GeV_c",
     "BBCLL1_PBSC_W3_2_6GeV_c", "BBCLL1_PBSC_E2_2_6GeV_c", "BBCLL1_PBSC_E3_2_6GeV_c", "BBCLL1_PBGL_E1_2_6GeV_c" },

    {"Gamma3_PBSC_W0_2_6GeV_c", "Gamma3_PBSC_W1_2_6GeV_c", "Gamma3_PBSC_W2_2_6GeV_c", "Gamma3_PBGL_E0_2_6GeV_c",
     "Gamma3_PBSC_W3_2_6GeV_c", "Gamma3_PBSC_E2_2_6GeV_c", "Gamma3_PBSC_E3_2_6GeV_c", "Gamma3_PBGL_E1_2_6GeV_c" }

  };
  const char *mchtitle[2][8] = {
    {"BBCLL1_PBSC_W0_2_6GeV_c", "BBCLL1_PBSC_W1_2_6GeV_c", "BBCLL1_PBSC_W2_2_6GeV_c", "BBCLL1_PBGL_E0_2_6GeV_c",
     "BBCLL1_PBSC_W3_2_6GeV_c", "BBCLL1_PBSC_E2_2_6GeV_c", "BBCLL1_PBSC_E3_2_6GeV_c", "BBCLL1_PBGL_E1_2_6GeV_c"},
    {"ERTLL1_4x4_BBCLL1_W0_2_6GeV_c", "ERTLL1_4x4_BBCLL1_W1_2_6GeV_c", "ERTLL1_4x4_BBCLL1_W2_2_6GeV_c",
     "ERTLL1_4x4_BBCLL1_E0_2_6GeV_c", "ERTLL1_4x4_BBCLL1_W3_2_6GeV_c", "ERTLL1_4x4_BBCLL1_E2_2_6GeV_c",
     "ERTLL1_4x4_BBCLL1_E3_2_6GeV_c", "ERTLL1_4x4_BBCLL1_E1_2_6Gev_c"}    
  }; 

  for (int itr = 0; itr < 2; itr++)  //itr: 0= BBCLL1, 1= Gamma3 & BBCLL1
    {
      for (int ih = 0; ih < 8; ih++)
	{
	  mchist[itr][ih] = cl->getHisto(mchname[itr][ih]);
	  // check if histo exists (if this gets called by
	  // the html for another subsystem, all pointers
	  // are 0 and it crashes when setting the NameTitle
	  if (!mchist[itr][ih])
	    {
	      return -1;
	    }
	  mchist[itr][ih]->SetNameTitle(mchname[itr][ih], mchtitle[itr][ih]);
	}
    }
  if (!gROOT->FindObject("EmcMon4"))
    {
      MakeCanvas("EmcMon4");
    }
  for (int ih = 0; ih < 8; ih++)
    {
      mchist[fTr][ih]->SetLineColor(30);
      mchist[fTr][ih]->SetLineWidth(2);
      mchist[fTr][ih]->SetMarkerStyle(24);
      mchist[fTr][ih]->SetMarkerSize(0.2);
      mchist[fTr][ih]->SetMarkerColor(3);
    }
  c_Emc[3]->SetEditable(1);
  EmcStyle->cd();
  EmcStyle->SetOptStat(0);
  c_Emc[3]->Clear("D");

  const char* strEMCalSect[] =
    {"PBSC W0 identified pi0", "PBSC W1 identified pi0", "PBSC W2 identified pi0", "PBGL E0 identified pi0",
     "PBSC W3 identified pi0", "PBSC E2 identified pi0", "PBSC E3 identified pi0", "PBGL E1 identified pi0"
    };

  TSpectr *spec = new TSpectr(10);

  for (int i = 0; i < 8; i++)
    {
      pad2_EmcPi0[i + 2]->cd();
      EmcLabelPi0[i + 2]->Clear();
      EmcLabelPi0[i + 2]->AddText("            ");
      EmcLabelPi0[i + 2]->AddText(strEMCalSect[i]);

      TH1 *mccopy = (TH1*) mchist[fTr][i]->Clone();
      int stat = 0;
      double xp, yp = 0, ypl, ypr, polV = 0;
      m_inv[i] = 0;
      errm_inv[i] = 0;
      sigm_inv[i] = 0;
      errsigm_inv[i] = 0;

      int npeak = spec->Search(mccopy, 7);
      // int npeak = spec->Search(mccopy, 9);
      if (npeak)
        {
          float *xpeaks = spec->GetPositionX();

          for (int ip = 0; ip < npeak; ip++)
            {
              if (xpeaks[ip] > 0.1 && xpeaks[ip] < 0.2)
                {
                  xp = xpeaks[ip];
                  yp = mccopy->GetBinContent(mccopy->GetXaxis()->FindBin(xp));
                  ypl = mccopy->GetBinContent(mccopy->GetXaxis()->FindBin(xp - 0.01));
                  ypr = mccopy->GetBinContent(mccopy->GetXaxis()->FindBin(xp + 0.01));
                  mccopy->Fit("fpol2", "qn", "RM", 0.07, 0.25);
                  polV = fpol2->Eval(xp);
                  if ((yp - sqrt(yp) > polV) && (ypl - sqrt(ypl) > fpol2->Eval(xp - 0.01)) &&
                      (ypr - sqrt(ypr) > fpol2->Eval(xp + 0.01)))
                    stat = 1;
                  break;
                }
            }
        }
      if (stat)
        {
          // fitp0->SetParameters(100,0.1,0.1, yp-polV, xp,0.02);
          fitp0->SetParameters(100, 0.1, 0.1, yp - polV, 0.148, 0.02);
          fitp0->SetParLimits(3, 10.0, 10000000.0);
          fitp0->SetParLimits(4, 0.07, 0.25);
          fitp0->SetParLimits(5, 0.005, 0.05);
          mchist[fTr][i]->Fit("fitp0", "sames", "RM", 0.07, 0.25);
          fpol2->SetParameters(fitp0->GetParameter(0), fitp0->GetParameter(1), fitp0->GetParameter(2));
          mchist[fTr][i]->DrawCopy();
          fpol2->DrawCopy("same");
          m_inv[i] = fitp0->GetParameter(4);
          errm_inv[i] = fitp0->GetParError(4);
          sigm_inv[i] = fitp0->GetParameter(5);
          errsigm_inv[i] = fitp0->GetParError(5);

          EmcLabelPi0[i + 2]->AddText( Form("chi2/ndf %5.4g", fitp0->GetChisquare() / fitp0->GetNDF()) );
          EmcLabelPi0[i + 2]->AddText( Form("const %5.4g±%5.4g", fitp0->GetParameter(3), fitp0->GetParError(3)) );
          EmcLabelPi0[i + 2]->AddText( Form("mean %5.4g±%5.4g", m_inv[i], errm_inv[i]) );
          EmcLabelPi0[i + 2]->AddText( Form("sigma %5.4g±%5.4g", sigm_inv[i], errsigm_inv[i]) );
          EmcLabelPi0[i + 2]->Draw();
        }
      else
        {
          mchist[fTr][i]->DrawCopy();
          EmcLabelPi0[i + 2]->AddText( Form("entry %g", mchist[fTr][i]->GetEntries()) );
          EmcLabelPi0[i + 2]->AddText( Form("mean %5.4g", mchist[fTr][i]->GetMean()) );
          EmcLabelPi0[i + 2]->AddText( Form("rms %5.4g", mchist[fTr][i]->GetRMS()) );
          EmcLabelPi0[i + 2]->Draw();
        }
      drawLimitHist(0.1, 0.2, mchist[fTr][i], lGrid);
    }

  EmcStyle->SetOptStat(0);
  c_Emc[3]->Update();
  c_Emc[3]->SetEditable(0);

  delete spec;

  return 0;
}




int EMCalMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);

  if (iret)
    {
      return iret;
    }
  filename << ThisName << "_EMCal_" << cl->RunNumber() << ".ps";
  c_Emc[0]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_EMCalStat_" << cl->RunNumber() << ".ps";
  c_Emc[1]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_EMCalPi0_" << cl->RunNumber() << ".ps";
  c_Emc[2]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_EMCalPi0Sector_" << cl->RunNumber() << ".ps";
  c_Emc[3]->Print(filename.str().c_str());

  return 0;
}


int EMCalMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();

  string pngfile = cl->htmlRegisterPage(*this, "Overview", "", "png");
  cl->CanvasToPng(c_Emc[0], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "Status", "Status", "png");
  cl->CanvasToPng(c_Emc[1], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "Expert/Identified Pi0 ", "Pi0", "png");
  cl->CanvasToPng(c_Emc[2], pngfile);

  pngfile = cl->htmlRegisterPage(*this, "Expert/Identified Pi0 in sectors", "Pi0sec", "png");
  cl->CanvasToPng(c_Emc[3], pngfile);

  cl->SaveLogFile(*this);

  return 0;
}

int EMCalMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "EMCALMONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}



int EMCalMonDraw::drawLimitHist(float min, float max, TH1 *h, TLine &l)
{
  TAxis *y = h->GetYaxis();

  float ymin = y->GetXmin();
  float ymax = y->GetXmax();

  int bin = h->GetNbinsX();

  float xmin = h->GetBinCenter((int) (bin * min + 1));
  float xmax = h->GetBinCenter((int) (bin * max + 1));
  l.SetLineWidth(2);
  l.SetLineColor(2);
  l.SetLineStyle(2);

  l.DrawLineNDC(xmin + 0.075, ymin + 0.1, xmin + 0.075, ymax - 0.1);
  l.DrawLineNDC(xmax + 0.055, ymin + 0.1, xmax + 0.055, ymax - 0.1);

  l.SetLineWidth(1);
  l.SetLineColor(1);
  l.SetLineStyle(1);

  return 1;
}


int EMCalMonDraw::drawBoxGrid(TH2 *h, TLine &l)
{
  TAxis *x = ((TH1*) h)->GetXaxis();
  TAxis *y = ((TH1*) h)->GetYaxis();

  // short color= 21;
  // h->SetLabelColor(color, "y");

  if ((h->GetNbinsX() % 12) || (h->GetNbinsY() % 12))
    return 0;
  int xMin = (int) x->GetXmin();
  int xMax = (int) x->GetXmax();
  int yMin = (int) y->GetXmin();
  int yMax = (int) y->GetXmax();

  int ifemMax, ismMax;
  int smBeg, smEnd, smStepX, smStepY;
  int module;
  int femBeg = yMin / 12;
  int femEnd = yMax / 12;
  const char * strEmcSector[] =
    {"WO", "W1", "W2", "W3", "E2", "E3", "E0", "E1"
    };
  int shiftSect;

  if (xMax == 72)          // PbSc
    {
      ifemMax = xMax / 12;
      ismMax = 36;
      smBeg = femBeg * 6;
      smEnd = femEnd * 6;
      smStepX = 2;
      smStepY = 2;
      module = 36;
      shiftSect = 0;
      // TText *PbScSectorText;

    }
  else if (xMax == 96)
    {   //PbGl
      ifemMax = xMax / 12;
      ismMax = 16;
      smBeg = femBeg * 3;
      smEnd = femEnd * 3;
      smStepX = 6;
      smStepY = 4;
      module = 48;
      shiftSect = 6;
    }
  else
    return 0;

  TText SectorText;
  SectorText.SetTextSize(0.04);
  SectorText.SetTextColor(46);
  for (int i = yMin / module; i < yMax / module; i++)
    {
      SectorText.DrawText(xMax + 1, i*module + module / 2 - 1, strEmcSector[i + shiftSect]);
    }


  int ifem;
  l.SetLineWidth(2);
  l.SetLineColor(13);
  for (ifem = 0; ifem <= ifemMax; ifem++)
    {
      int xfem = ifem * 12;
      l.DrawLine(xfem, yMin, xfem, yMax);
    }
  for (ifem = femBeg; ifem <= femEnd; ifem++)
    {
      int yfem = ifem * 12;
      l.DrawLine(xMin, yfem, xMax, yfem);
    }

  int ism;
  l.SetLineWidth(1);
  for (ism = 0; ism < ismMax; ism++)
    {
      int xsm = ism * smStepX;
      l.DrawLine(xsm, yMin, xsm, yMax);
    }
  for (ism = smBeg; ism < smEnd; ism++)
    {
      int ysm = ism * smStepY;
      l.DrawLine(xMin, ysm, xMax, ysm);
    }

  l.SetLineColor(2);
  for (ism = yMin + module; ism < yMax; )
    {
      l.DrawLine(xMin, ism, xMax, ism);
      ism += module;
    }

  l.SetLineWidth(1);
  l.SetLineColor(1);

  return 1;
}


int EMCalMonDraw::iCHiSMT(int iCH)
{
  //  This is a very preliminary version which may well change at
  //  a later time. It looks a bit complicated but it will be used
  //  only to establish look-up-tables for individual EmcFeeCrates
  int ASIC = iCH / 24;          //  2 tower wide columns
  int preamp = (iCH % 24) / 4;      //  2 tower wide rows counted
  //  from above (0 at the top)
  int input = iCH % 4;
  //  convert input into x/y for cells within 'module' scope
  int xc, yc;
  yc = input / 2;
  xc = 1 - input % 2;
  return 12*((5 - preamp)*2 + yc) + ASIC*2 + xc;
}

int EMCalMonDraw::SMiSMTiST(int iSM, int iSMTower, int emc)
{
  int x, bin, y, xx, yy;

  if (emc == 2)
    {
      x = iSM % 8;
      y = iSM / 8;
      bin = 96;
    }   //PbGl
  else
    if (emc == 1)
      {
        y = iSM / 6;
        x = iSM - y * 6;
        bin = 72;
      } //PbSc
    else
      return -1;

  yy = iSMTower / 12;
  xx = iSMTower - yy * 12;


  return (y*12 + yy)*bin + x*12 + xx;
}
