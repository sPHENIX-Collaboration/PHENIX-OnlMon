#include <PadMonDraw.h>
#include <OnlMonClient.h>
#include <msg_control.h>

#include <TBox.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <stdlib.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

PadMonDraw::PadMonDraw() :
    OnlMonDraw("PADMON")
{
  text = new TText();
  bar = new TLine();
  dashedbar = new TLine();
  pt1 = new TBox();
  //  frame = new TBox();
  padClustRatio = 0;
  padClustRatioRed = 0;
  padClustRatioBlack = 0;
  padClustRatioWatch = 0;
  padClustActivityRed = 0;
  padClustActivityBlack = 0;
  padClustActivityWatch = 0;
  padEventErrorRed = 0;
  padEventErrorBlack = 0;
  return;
}

PadMonDraw::~PadMonDraw()
{
  delete text;
  delete bar;
  delete dashedbar;
  delete pt1;

  delete padClustRatio;
  delete padClustRatioRed;
  delete padClustRatioBlack;
  delete padClustRatioWatch;
  delete padClustActivityRed;
  delete padClustActivityBlack;
  delete padClustActivityWatch;
  delete padEventErrorRed;
  delete padEventErrorBlack;

  //  delete frame;
}

int PadMonDraw::CreateCanvas()
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  cPad = new TCanvas("cPad", "PAD online OS", -1, 0, xsize, ysize);
  gSystem->ProcessEvents();
  // PAD Canvas
  cPad->Range(0, 0, 1, 1);
  cPad->SetFillColor(10);
  cPad->SetBorderMode(0);
  cPad->SetBorderSize(2);
  cPad->SetFrameFillColor(10);
  cPad->SetFrameBorderMode(0);
  cPad->SetTickx();
  cPad->SetTicky();

  cPadTop = new TPad("cPadTop", " ", 0.01, 0.67, 0.99, 0.99);
  cPadTop->Draw();
  cPadTop->cd();
  cPadTop->Range(-14.9611, -0.00688554, 163.783, 0.0612794);
  cPadTop->SetFillColor(10);
  cPadTop->SetBorderSize(2);
  cPadTop->SetLeftMargin(0.09);
  cPadTop->SetRightMargin(0.02);
  cPadTop->SetTopMargin(0.08);
  cPadTop->SetBottomMargin(0.13);

  cPad->cd();
  cPadMiddle = new TPad("cPadMiddle", " ", 0.01, 0.34, 0.99, 0.66);
  cPadMiddle->Draw();
  cPadMiddle->cd();
  cPadMiddle->Range(-18.6526, -0.610983, 162.395, 5.43757);
  cPadMiddle->SetBorderSize(2);
  cPadMiddle->SetFillColor(10);
  cPadMiddle->SetLeftMargin(0.09);
  cPadMiddle->SetRightMargin(0.02);
  cPadMiddle->SetTopMargin(0.08);
  cPadMiddle->SetBottomMargin(0.13);

  cPad->cd();
  cPadBottom = new TPad("cPadBottom", " ", 0.01, 0.01, 0.99, 0.33);
  cPadBottom->Draw();
  cPadBottom->cd();
  cPadBottom->Range(-18.6526, -0.610983, 162.395, 5.43757);
  cPadBottom->SetBorderSize(2);
  cPadBottom->SetFillColor(10);
  cPadBottom->SetLeftMargin(0.09);
  cPadBottom->SetRightMargin(0.02);
  cPadBottom->SetTopMargin(0.08);
  cPadBottom->SetBottomMargin(0.13);

  if (!gROOT->FindObject("padClustRatio"))
  {
    padClustRatio = new TH1F("padClustRatio", "PC1,2,3 ClustRatio", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);

  }
  if (!gROOT->FindObject("padClustRatioBlack"))
  {
    padClustRatioBlack = new TH1F("padClustRatioBlack", "PC1,2,3 ClustRatio", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  }
  if (!gROOT->FindObject("padClustRatioRed"))
  {
    padClustRatioRed = new TH1F("padClustRatioRed", "PC1,2,3 ClustRatio", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  }
  if (!gROOT->FindObject("padClustRatioWatch"))
  {
    padClustRatioWatch = new TH1F("padClustRatioWatch", "PC1,2,3 ClustRatio", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  }
  if (!gROOT->FindObject("padClustActivityBlack"))
  {
    padClustActivityBlack = new TH1F("padClustActivityBlack", "PC1,2,3 ClustActivity", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  }
  if (!gROOT->FindObject("padClustActivityRed"))
  {
    padClustActivityRed = new TH1F("padClustActivityRed", "PC1,2,3 ClustActivity", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  }
  if (!gROOT->FindObject("padClustActivityWatch"))
  {
    padClustActivityWatch = new TH1F("padClustActivityWatch", "PC1,2,3 ClustActivity", PC_NINSTALLED * NHVSECTORS, -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  }
  if (!gROOT->FindObject("padEventErrorBlack"))
  {
    padEventErrorBlack = new TH1F("padEventErrorBlack", "PC1,2,3 EventError", PC_NINSTALLED * NFEMSPERPLANE, -0.5, PC_NINSTALLED * NFEMSPERPLANE - 0.5);
  }
  if (!gROOT->FindObject("padEventErrorRed"))
  {
    padEventErrorRed = new TH1F("padEventErrorRed", "PC1,2,3 EventError", PC_NINSTALLED * NFEMSPERPLANE, -0.5, PC_NINSTALLED * NFEMSPERPLANE - 0.5);
  }
  cPad->SetEditable(0);
  return 0;
}

int PadMonDraw::Draw(const char *what)
{

  // Set the style
  TStyle* oldstyle = gStyle;
  TStyle* padstyle = new TStyle("padstyle", "PAD Online Monitor Style");
  padstyle->Reset();
  padstyle->SetCanvasBorderMode(0);
  padstyle->SetFrameBorderMode(0);
  padstyle->SetPadBorderMode(0);
  padstyle->SetPadColor(0);
  padstyle->SetPadTickX(1);
  padstyle->SetPadTickY(1);
  padstyle->SetCanvasColor(0);
  padstyle->SetStatColor(0);
  padstyle->SetTitleColor(0);
  padstyle->SetLineWidth(1);
  padstyle->SetHistLineWidth(1);
  padstyle->SetTextSize(0.05);
  padstyle->SetOptStat(0);
  padstyle->SetOptTitle(0);
  padstyle->cd();
  //

  // Begin by checking if the TCanvas is still there, if not,
  // recreate it
  if (!gROOT->FindObject("cPad"))
  {
    CreateCanvas();
  }
  cPad->SetEditable(1);

  OnlMonClient *cl = OnlMonClient::instance();

  float aTopMargine = 3.0; // in average
  float rTopMargine = 5.0; // absolute
  int allbins = PC_NINSTALLED * NHVSECTORS;
  int allbinsbottom = PC_NINSTALLED * NFEMSPERPLANE;
  float bin1, bin2, ratio, ratioerr;
  char posting[100];
  char idstring[128];

  TH1 *padStat;
  TH1 *padClustSize[PC_NINSTALLED * NHVSECTORS];
  TH1 *padClustActivity;
  TH1 *padEventError;
  TH1 *padMaxRocs;

  // Check that the histograms exist and that some events have
  // been processed.
  padStat = NULL;
  padStat = cl->getHisto("padStat");
  if (padStat)
  {
    int nevent = (int) padStat->GetBinContent(1);
    if (nevent == 0)
    {
      cPad->Clear("D");
      cout << " PAD: No events processed! " << endl;
      // Write this info also on the canvas
      bin1 = allbins;
      pt1->SetLineWidth(5);
      pt1->SetFillColor(5);
      pt1->DrawBox(-0.5, 1.01 * rTopMargine, bin1 - 0.5, 1.14 * rTopMargine);
      //  frame->SetFillStyle(0);
      //  frame->SetLineWidth(3);
      //  frame->DrawBox(-0.5, 1.01 * rTopMargine, bin1-0.5, 1.14 * rTopMargine);
      text->SetTextAlign(21);
      text->SetTextSize(0.06);
      text->SetTextColor(2);
      text->DrawText(80.0, 1.03 * rTopMargine, "No events processed");
      cPad->Update();
      oldstyle->cd();
      delete padstyle;
      return 0;
    }
  }
  else
  {
    cPad->Clear("D");
    cout << " PAD: Cannot get histograms from server! " << endl;
    // Write this info also on the canvas
    TText FatalMsg;
    FatalMsg.SetTextFont(62);
    FatalMsg.SetTextSize(0.1);
    FatalMsg.SetTextColor(4);
    FatalMsg.SetNDC();  // set to normalized coordinates
    FatalMsg.SetTextAlign(23); // center/top alignment
    FatalMsg.DrawText(0.5, 0.9, "PADMONITOR");
    FatalMsg.SetTextAlign(22); // center/center alignment
    FatalMsg.DrawText(0.5, 0.5, "SERVER");
    FatalMsg.SetTextAlign(21); // center/bottom alignment
    FatalMsg.DrawText(0.5, 0.1, "DEAD");

    cPad->Update();
    oldstyle->cd();
    delete padstyle;
    return -1;
  }

  int i, j, k, installedplanes = 0;
  short message = 0;

  int NRemovedActivity = 4;
  int NRemovedRatio = 4;
  float activity_average = 0;
  float activityerror_average = 0;
  float ratio_average = 0;
  float ratioerror_average = 0;

  float aLimURed = 6.0; // in sigmas
  float aLimUBlack = 3.0;
  float aLimLBlack = 3.0;
  float aLimLRed = 6.0;

  float rLimURed = 2.5; // absolute
  float rLimUBlack = 1.7;
  float rLimLBlack = 1.0;
  float rLimLRed = 0.7;

  float eLimRed = 8.0;  // absolute
  float eLimBlack = 0.0;

  float LowLimitActivity = 0.0002; // absolute
  float LowLimitRatio = 0.7;       // absolute
  int activity_below_limit = 0;
  int activity_above_limit = 0;
  int ratio_below_limit = 0;
  int problematic_sectors = 0;

  float aVec[PC_NINSTALLED * NHVSECTORS];
  float rVec[PC_NINSTALLED * NHVSECTORS];

  padClustActivity = cl->getHisto("padClustActivity");
  padEventError = cl->getHisto("padEventError");
  padMaxRocs = cl->getHisto("padMaxRocs");

  for (i = 0; i < NPLANES; i++)
  {
    if (PC_INSTALLED[i])
    {
      // fill the cluster ratio histogram
      for (k = 0; k < NHVSECTORS; k++)
      {
        j = installedplanes * NHVSECTORS + k;

        sprintf(idstring, "padClustSize%d", j);
        padClustSize[j] = cl->getHisto(idstring);
        bin1 = (float) padClustSize[j]->GetBinContent(2); // bin x = size x-1..
        bin2 = (float) padClustSize[j]->GetBinContent(3);

        if ((bin1 > 0) && (bin2 > 0))
        {
          ratio = bin2 / bin1;
          // the relative error in the ratio is the sqrt of the other relative errors squared
          ratioerr = ratio * sqrt(1.0 / bin1 + 1.0 / bin2); // let's calc the errors explcitly.
          // Don't trust the errors from the histogram..
        }
        else
        {
          //bin1 and bin2 not filled properly, or set <= 0
          ratio = -1.0;
          ratioerr = -1.0;
        }
        padClustRatio->SetBinContent(j + 1, ratio);
        padClustRatio->SetBinError(j + 1, ratioerr);

        aVec[j] = padClustActivity->GetBinContent(j + 1);
        rVec[j] = ratio;

      }
      installedplanes++;
    }
  }

  int idummy = 0;
  idummy = padSortVector(aVec, PC_NINSTALLED * NHVSECTORS, NRemovedActivity);
  idummy = padSortVector(rVec, PC_NINSTALLED * NHVSECTORS, NRemovedRatio);

  // Calculate the truncated mean and standard deviations (JN 030130)
  // First, Activity
  for (int j = NRemovedActivity; j < (PC_NINSTALLED * NHVSECTORS - NRemovedActivity); j++)
  {
    activity_average += aVec[j];
    activityerror_average += aVec[j] * aVec[j];
  }
  activity_average /= (float) (allbins - 2 * NRemovedActivity);
  activityerror_average /= (float) (allbins - 2 * NRemovedActivity);
  activityerror_average -= activity_average * activity_average;
  activityerror_average = sqrt(activityerror_average);
  // Then, Ratio
  for (int j = NRemovedRatio; j < (PC_NINSTALLED * NHVSECTORS - NRemovedRatio); j++)
  {
    ratio_average += rVec[j];
    ratioerror_average += rVec[j] * rVec[j];
  }
  ratio_average /= (float) (allbins - 2 * NRemovedRatio);
  ratioerror_average /= (float) (allbins - 2 * NRemovedRatio);
  ratioerror_average -= ratio_average * ratio_average;
  ratioerror_average = sqrt(ratioerror_average);

  // printf("Average act: %6.4f rat: %6.4f \n", activity_average, ratio_average);
  // printf("Errors  act: %6.4f rat: %6.4f \n", activityerror_average, ratioerror_average);

  if (activity_average > 0)
  {
    aTopMargine = activity_average * aTopMargine;
  }
  else
  {
    aTopMargine = 0.01;
  }

  aLimURed = activity_average + aLimURed * activityerror_average;
  aLimUBlack = activity_average + aLimUBlack * activityerror_average;
  aLimLBlack = activity_average - aLimLBlack * activityerror_average;
  aLimLRed = activity_average - aLimLRed * activityerror_average;
  // Set an absolute lower limit for red (JN 030129)
  if (aLimLRed < 0.0001) aLimLRed = 0.0001;

  //inserted to load external pad_to_watch.txt file
  int watchi = 0;
  string padTemp;
  int padWatch[256];
  bool watchLoaded = true;
  bool watchUsed = false;

  string padcalibdir;
  if (!getenv("PADCALIBDIR"))
  {
    padcalibdir = ".";
  }
  else
  {
    padcalibdir = getenv("PADCALIBDIR");
  }
  string padcalib = padcalibdir + "/pad_to_watch.txt";

  ifstream ifs(padcalib.c_str());
  if (ifs)
  {
    while (getline(ifs, padTemp))
    {
      padWatch[watchi] = atoi(padTemp.c_str());
      watchi++;
    }
  }
  else
  {
    watchLoaded = false; //used later to print error and as a requirement to print data points
  }

  for (j = 0; j < allbins; j++)
  {
    bin1 = padClustActivity->GetBinContent(j + 1);
    bin2 = padClustRatio->GetBinContent(j + 1);

    //removed hardcoded fix for run7 (AO & MI 03/12/07)
    // hardcode in fix to skip the disabled HV sectors for run6pp; AO & DS
    // if (j==149 || j==136 || (j>=92 && j<=95)) { // just set to average
    //	bin1 = activity_average;
    //	bin2 = ratio_average;
    //}
    // end of hardcode fix

    // Set sectors 156, 157, 158 and 159 to average values
    // due to packet errors (AO and MI 05/12/07)

    // if (j==156 || j==157 || j==158 || j==159) { // just set to average

    //	bin1 = activity_average;
    // 	bin2 = ratio_average;

    // }

    // end of hardcoded fix

    if ((bin1 > aLimUBlack) || (bin1 < aLimLBlack))
    {
      padClustActivityBlack->SetBinContent(j + 1, bin1);
      if (bin1 > aTopMargine) padClustActivityBlack->SetBinContent(j + 1, 0.98 * aTopMargine);
    }
    else
    {
      padClustActivityBlack->SetBinContent(j + 1, 0); // not plotted later
    }
    if ((bin1 > aLimURed) || (bin1 < aLimLRed))
    {
      if (bin1 == 0)
      {
        bin1 = 0.00001;
      } // do plot - even if 0
      padClustActivityRed->SetBinContent(j + 1, bin1);
      // If above histo max, set to max (JN 030129)
      if (bin1 > aTopMargine) padClustActivityRed->SetBinContent(j + 1, 0.98 * aTopMargine);
    }
    else
    {
      padClustActivityRed->SetBinContent(j + 1, 0); // not plotted later
    }

    if ((bin2 > rLimUBlack) || (bin2 < rLimLBlack))
    {
      padClustRatioBlack->SetBinContent(j + 1, bin2);
    }
    else
    {
      padClustRatioBlack->SetBinContent(j + 1, -999); // not plotted later
    }
    if ((bin2 > rLimURed) || (bin2 < rLimLRed))
    {
      padClustRatioRed->SetBinContent(j + 1, bin2);
    }
    else
    {
      padClustRatioRed->SetBinContent(j + 1, -999); // not plotted later
    }

    //control code for pad_to_watch change
    watchUsed = false;
    if (watchLoaded && watchi > 0)
    {
      for (int i = 0; i < watchi; i++)
      {
        if (padWatch[i] == j + 1)
        {
          padClustRatioWatch->SetBinContent(j + 1, bin2);
          padClustActivityWatch->SetBinContent(j + 1, bin1);
          if (bin1 > aTopMargine) padClustActivityWatch->SetBinContent(j + 1, 0.98 * aTopMargine);
          watchUsed = true;
          continue;
        }
      }
      if (!watchUsed)
      { // neither plotted later
        padClustRatioWatch->SetBinContent(j + 1, -999);
        padClustActivityWatch->SetBinContent(j + 1, -999);
      }
    }
    else
    {
      padClustRatioWatch->SetBinContent(j + 1, -999);
      padClustActivityWatch->SetBinContent(j + 1, -999);
    } //end pad_to_watch control

    if ((bin1 > aLimURed) || (bin1 < aLimLRed) || (bin2 > rLimURed) || (bin2 < rLimLRed))
    {
      problematic_sectors++;
    }
    if (bin1 > aLimURed)
    {
      activity_above_limit++;
    }
    if (bin1 < LowLimitActivity)
    {
      activity_below_limit++;
    }
    if (bin2 < LowLimitRatio)
    {
      ratio_below_limit++;
    }
  }

  // ****** for event address check
  int NEvtAdrErr = 0;
  for (j = 0; j < allbinsbottom; j++)
  {
    if (j == 70) padEventError->SetBinContent(j + 1, 1); // DPR: ignores 70, which is currently a dummy board 02/17/12 - remove if repaired
    bin1 = padEventError->GetBinContent(j + 1);
    //      cout << bin1 << endl;
    if ((bin1 > eLimBlack))
    {
      padEventErrorBlack->SetBinContent(j + 1, bin1);
    }
    else
    {
      padEventErrorBlack->SetBinContent(j + 1, -999); // not plotted later
    }
    if ((bin1 > eLimRed))
    {
      padEventErrorRed->SetBinContent(j + 1, bin1);
      NEvtAdrErr++;
    }
    else
    {
      padEventErrorRed->SetBinContent(j + 1, -999); // not plotted later
    }
  }

  //ok, let's draw them

  cPadTop->cd();
  padClustActivity->SetMaximum(aTopMargine);

  padClustActivity->SetMarkerStyle(8);
  padClustActivity->SetMarkerColor(4);
  padClustActivity->SetMarkerSize(1.2);
  padClustActivity->GetXaxis()->SetTitle("HV sector Number");
  padClustActivity->GetXaxis()->SetTitleFont(62);
  padClustActivity->GetXaxis()->SetTitleSize(0.06);
  padClustActivity->GetXaxis()->SetTitleOffset(0.8);

  padClustActivity->GetYaxis()->SetTitle("Hits per Wire");
  padClustActivity->GetYaxis()->SetTitleFont(62);
  padClustActivity->GetYaxis()->SetTitleSize(0.065);
  padClustActivity->GetYaxis()->SetTitleOffset(0.6);

  padClustActivityRed->SetMarkerStyle(8);
  padClustActivityRed->SetMarkerColor(2);
  padClustActivityRed->SetMarkerSize(1.2);

  padClustActivityBlack->SetMarkerStyle(8);
  padClustActivityBlack->SetMarkerColor(1);
  padClustActivityBlack->SetMarkerSize(1.2);

  padClustActivityWatch->SetMarkerStyle(8);
  padClustActivityWatch->SetMarkerColor(5);
  padClustActivityWatch->SetMarkerSize(1.2);

  padClustActivity->SetStats(0);
  padClustActivityBlack->SetStats(0);
  padClustActivityRed->SetStats(0);
  padClustActivityWatch->SetStats(0);

  padClustActivity->Draw("p");
  padClustActivityBlack->Draw("psame");
  padClustActivityRed->Draw("psame");
  padClustActivityWatch->Draw("psame");

  for (i = 0; i < installedplanes - 1; i++)
  {
    bin1 = (i + 1) * 32;
    bar->DrawLine(bin1, aTopMargine * 0.02, bin1, aTopMargine * 0.9);
  }

  // hardwired coordinates....
  text->SetTextAlign(11);
  text->SetTextSize(0.05);
  text->SetTextColor(1);
  text->DrawText(8 + 32 * 0, 0.85 * aTopMargine, "PC1 West");
  text->DrawText(8 + 32 * 1, 0.85 * aTopMargine, "PC1 East");
  text->DrawText(8 + 32 * 2, 0.85 * aTopMargine, "PC2 West");
  text->DrawText(8 + 32 * 3, 0.85 * aTopMargine, "PC3 West");
  text->DrawText(8 + 32 * 4, 0.85 * aTopMargine, "PC3 East");

  bin1 = allbins;
  int run = cl->RunNumber();
  int ndataevt = (int) padStat->GetBinContent(1);
  // This gives current time
  //  time_t t = time(0);
  // This gets time when data was taken
  time_t evttime = cl->EventTime("CURRENT");
  sprintf(posting, "PC ONLINE MONITOR:      Run: %d      Date: %s      Events: %d", run, ctime(&evttime), ndataevt);
  if (!watchLoaded)
  {
    sprintf(posting, "PC ONLINE MONITOR:      Run: %d      Date: %s      Events: %d ; Error: pad_to_watch not loaded.", run, ctime(&evttime), ndataevt);
  }
  pt1->SetLineWidth(5);
  pt1->SetFillColor(16);
  pt1->DrawBox(-0.5, 1.01 * aTopMargine, bin1 - 0.5, 1.14 * aTopMargine);
  //  frame->SetFillStyle(0);
  //  frame->SetLineWidth(3);
  //  frame->DrawBox(-0.5, 1.01 * aTopMargine, bin1-0.5, 1.14 * aTopMargine);
  text->SetTextAlign(21);
  text->SetTextSize(0.055);
  text->SetTextColor(4);
  text->DrawText(80.0, 1.03 * aTopMargine, posting);

  cPadMiddle->cd();

  padClustRatio->SetMaximum(rTopMargine);
  padClustRatio->SetMinimum(0); // negative ratios do not make sense
  padClustRatio->SetTitle("");

  padClustRatio->SetMarkerStyle(8);
  padClustRatio->SetMarkerColor(4);
  padClustRatio->SetMarkerSize(1.2);
  padClustRatio->GetXaxis()->SetTitle("HV sector Number");
  padClustRatio->GetXaxis()->SetTitleFont(62);
  padClustRatio->GetXaxis()->SetTitleSize(0.06);
  padClustRatio->GetXaxis()->SetTitleOffset(0.8);

  padClustRatio->GetYaxis()->SetTitle("2/1 cluster ratio");
  padClustRatio->GetYaxis()->SetTitleFont(62);
  padClustRatio->GetYaxis()->SetTitleSize(0.065);
  padClustRatio->GetYaxis()->SetTitleOffset(0.6);

  padClustRatioBlack->SetMarkerStyle(8);
  padClustRatioBlack->SetMarkerColor(1);
  padClustRatioBlack->SetMarkerSize(1.2);

  padClustRatioRed->SetMarkerStyle(8);
  padClustRatioRed->SetMarkerColor(2);
  padClustRatioRed->SetMarkerSize(1.2);

  padClustRatioWatch->SetMarkerStyle(8);
  padClustRatioWatch->SetMarkerColor(5);
  padClustRatioWatch->SetMarkerSize(1.2);

  padClustRatio->SetStats(0);
  padClustRatioBlack->SetStats(0);
  padClustRatioRed->SetStats(0);
  padClustRatioWatch->SetStats(0);

  padClustRatio->Draw("p");
  padClustRatioBlack->Draw("psame");
  padClustRatioRed->Draw("psame");
  padClustRatioWatch->Draw("psame");

  for (i = 0; i < installedplanes - 1; i++)
  {
    bin2 = (i + 1) * 32;
    bar->DrawLine(bin2, rTopMargine * 0.02, bin2, rTopMargine * 0.9);
  }

  dashedbar->SetLineColor(2);
  dashedbar->SetLineStyle(2);
  dashedbar->SetLineWidth(3);
  dashedbar->DrawLine(3, 1.0, allbins - 3, 1.0);

  int badRocStat = (int) padStat->GetBinContent(4);
  if (badRocStat == 999)
  {
    sprintf(posting, "File with bad ROCs not found by server.");
    message = 2;
  }
  else if (ndataevt <= MIN_NEVENTS_HTML)
  {
    sprintf(posting, "Not enough events");
    message = 2;
  }
  else if (activity_below_limit > 20 + watchi)
  {
    sprintf(posting, "Check High Voltage! SEVERAL LOW CHANNELS IN UPPER PANEL ");
    message = 2;
  }
  else if (ndataevt <= 2 * MIN_NEVENTS_HTML)
  {
    sprintf(posting, "Run more events if something looks bad.");
    message = 50;
  }
  else if (activity_below_limit > watchi)
  {
    sprintf(posting, "LOW CHANNELS IN UPPER PANEL. Check High Voltage! ");
    message = 2;
  }
  else if (ratio_below_limit > 50)
  {
    sprintf(posting, "LOW CHANNELS IN MIDDLE PANEL. Make feed into FEMs. If it doesn't help call PC expert.");
    message = 2;
  }
  else if ((problematic_sectors > 10 + watchi) && (ndataevt >= 500))
  {
    sprintf(posting, "Atmospheric pressure is likely high > 1.015 bar. If not, contact PC expert.");
    message = 1;
  }
  else if (activity_above_limit > 0)
  {
    sprintf(posting, "Hot spot(s) detected in upper panel. Not severe. The PC expert will remove it in due time.");
    message = 1;
  }
  //  else if (activity_average < 0.035)
  //    {
  //      sprintf(posting, "Activity lower than expected for MinBias. Check trigger composition.");
  //      message = 2;
  //    }

  if (message > 0)
  {
    bin1 = allbins;
    pt1->SetFillColor(5);
    pt1->DrawBox(-0.5, 1.01 * rTopMargine, bin1 - 0.5, 1.14 * rTopMargine);
    text->SetTextAlign(21);
    text->SetTextSize(0.06);
    text->SetTextColor(message);
    text->DrawText(80.0, 1.03 * rTopMargine, posting);
    //      frame->SetFillStyle(0);
    //      frame->SetLineWidth(3);
    //      frame->DrawBox(-0.5, 1.01 * rTopMargine, bin1-0.5, 1.14 * rTopMargine);
  }

  cPadBottom->cd();

  padEventError->SetMaximum(50);
  padEventError->SetMinimum(-5);
  padEventError->SetMarkerStyle(8);
  padEventError->SetMarkerColor(4);
  padEventError->SetMarkerSize(1);

  padEventError->GetXaxis()->SetTitle("FEM Number");
  padEventError->GetXaxis()->SetTitleFont(62);
  padEventError->GetXaxis()->SetTitleSize(0.06);
  padEventError->GetXaxis()->SetTitleOffset(0.8);
  padEventError->GetYaxis()->SetTitle("#ROCs with address errors");
  padEventError->GetYaxis()->SetTitleFont(62);
  padEventError->GetYaxis()->SetTitleSize(0.065);
  padEventError->GetYaxis()->SetTitleOffset(0.6);

  padEventErrorRed->SetMarkerStyle(8);
  padEventErrorRed->SetMarkerColor(2);
  padEventErrorRed->SetMarkerSize(1);

  padEventErrorBlack->SetMarkerStyle(8);
  padEventErrorBlack->SetMarkerColor(1);
  padEventErrorBlack->SetMarkerSize(1);

  //  padMaxRocs->SetLineStyle(3);
  padMaxRocs->SetLineColor(16);
  padMaxRocs->SetMaximum(50);
  padMaxRocs->SetMinimum(-5);

  padEventError->SetStats(0);
  padMaxRocs->SetStats(0);
  padEventErrorBlack->SetStats(0);
  padEventErrorRed->SetStats(0);

  padEventError->Draw("p0");
  padMaxRocs->Draw("same");
  padEventErrorBlack->Draw("psame");
  padEventErrorRed->Draw("psame");

  for (i = 0; i < installedplanes + 1; i++)
  {
    bin1 = i * 16 - 0.5;
    if (i == 0 || i == installedplanes)
    {
      bar->DrawLine(bin1, -5, bin1, 50);
      //          TLine bar(bin1, -5, bin1, 50);
      //          bar.Draw();
    }
    else
    {
      bar->DrawLine(bin1, -2, bin1, 42);
    }
  }

  message = 0;
  int minnranalyzed = (int) padStat->GetBinContent(5);
  int lastLowAnalyzed = (int) padStat->GetBinContent(6);
  lastLowAnalyzed = lastLowAnalyzed + 4000;
  if (minnranalyzed < EVENTS_REQUIRED_EVTNR)
  {
    sprintf(posting, "%s%d", "At least one packet has too few events. Last packet with too few:  ", lastLowAnalyzed);
    message = 2;
  }
  else if (NEvtAdrErr > MAX_ADR_ERR)
  {
    sprintf(posting, "HIGH POINT(S) IN LOWER PANEL. ROC Event Address Error. Call PC Expert.");
    message = 2;
  }

  if (message > 0)
  {
    bin1 = allbinsbottom;
    pt1->SetFillColor(5);
    pt1->DrawBox(-0.5, 1.01 * 50, bin1 - 0.5, 1.14 * 50);
    text->SetTextAlign(21);
    text->SetTextSize(0.06);
    text->SetTextColor(message);
    text->DrawText(40.0, 1.03 * 50, posting);
    //      frame->SetFillStyle(0);
    //      frame->SetLineWidth(3);
    //      frame->DrawBox(-0.5, 1.01*50, bin1-0.5, 1.14*50);
  }

  cPad->Update();
  oldstyle->cd();
  delete padstyle;
  cPad->SetEditable(0);

  return 0;
}

int PadMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // on error no ps files please
  {
    return iret;
  }
  filename << ThisName << "_" << cl->RunNumber() << ".ps";
  cPad->Print(filename.str().c_str());
  return 0;
}

int PadMonDraw::MakeHtml(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *padStat;
  TH1 *padProfPadSum[PC_NINSTALLED]; // 1 FEM per bin
  TH1 *padProfClustActivity[PC_NINSTALLED]; // 1 HV sector per bin
  TH1 *padProfClustSize[PC_NINSTALLED];
  TH1 *padClustSize[PC_NINSTALLED * NHVSECTORS];
  TH2 *padFemPadxPadz[PC_NINSTALLED * NFEMSPERPLANE];

  padStat = cl->getHisto("padStat");
  // Just in case we look at a root file which doesn't contain padchamber
  // histograms (which is about 90% of all cases), return gracefully
  if (!padStat)
  {
    return -1;
  }
  //  int nevent = (int)padStat->GetBinContent(1);

  char idstring[128];
  const char *planeinfo[] = { "PC1W", "PC1E", "PC2W", "PC2E", "PC3W", "PC3E" };

  char tmpname[128] = { "a" };
  char hardwarename[20];
  //float p2[1000] = {0.0}; // All 1000 values initialized to zero.
  int status, pc, arm;

  // Define subdirectory where the html files will be put
  // Use the environment variable PAD_ONLMON_HTMLDIR if defined
  string htmldir;
  if (getenv("PAD_ONLMON_HTMLDIR"))
  {
    cerr << "PadMonDraw::MakeHtml : PAD_ONLMON_HTMLDIR is defined but is " << "no longer used!" << endl;
  }

  string pngfile, htmlfile;
  string fullfilename, shortfilename;

  int iret = Draw(what);
  if (iret)
  {
    return iret;
  }

  pngfile = cl->htmlRegisterPage(*this, "Monitor Canvas", "display", "png");
  cl->CanvasToPng(cPad, pngfile);

  htmlfile = cl->htmlRegisterPage(*this, "For EXPERTS", "log", "html");
  ofstream outfile(htmlfile.c_str());

  int irun = cl->RunNumber();
  padStat = cl->getHisto("padStat");
  int ndataevt = (int) padStat->GetBinContent(1);
  sprintf(tmpname, "Pad Chamber report for Run %d: analyzed %d events", irun, ndataevt);

  //  outfile << html() << head(title(filename)) << endl;
  outfile << "<HTML>" << endl;
  outfile << "<LINK REL=STYLESHEET HREF=\"style.css\">" << endl;
  outfile << "<TITLE>" << tmpname << "</TITLE>" << endl;
  outfile << "<BODY>" << endl;
  outfile << "<h2>" << tmpname << "</h2>" << endl;

  //  msg_control * evmc = new msg_control(MSG_TYPE_MONITORING,
  // 				       MSG_SOURCE_PC,
  //				       MSG_SEV_WARNING, "Pad Monitor");

  int i, j, k;

  // Calculate the mean padSum and clustActivity 
  float padSumMean = 0.0;
  float padSumErrorMean = 0.0;
  float clustActMean = 0.0;
  float clustActErrorMean = 0.0;
  float padSum[MAX_FEMS];
  float clustAct[PC_NINSTALLED * NHVSECTORS];

  int installedplanes = 0;
  for (i = 0; i < NPLANES; i++)
  {
    if (PC_INSTALLED[i])
    {
      sprintf(idstring, "padProfPadSum%d", i);
      padProfPadSum[installedplanes] = cl->getHisto(idstring);
      sprintf(idstring, "padProfClustActivity%d", i);
      padProfClustActivity[installedplanes] = cl->getHisto(idstring);
      for (k = 0; k < NFEMSPERPLANE; k++)
      {
        j = installedplanes * NFEMSPERPLANE + k;
        padSum[j] = (float) padProfPadSum[installedplanes]->GetBinContent(k + 1); // 1st bin is 1, not 0
      }
      for (k = 0; k < NHVSECTORS; k++)
      {
        j = installedplanes * NHVSECTORS + k;
        clustAct[j] = (float) padProfClustActivity[installedplanes]->GetBinContent(k + 1); // 1st bin is 1, not 0
      }
      installedplanes++;
    }
  }

  int NRemovedSum = 4;
  int NRemovedAct = 4;
  int idummy = 0;
  idummy = padSortVector(padSum, MAX_FEMS, NRemovedSum);
  idummy = padSortVector(clustAct, PC_NINSTALLED * NHVSECTORS, NRemovedAct);

  // Calculate the truncated mean and standard deviations (JN 030130)
  // First, padSum
  for (int j = NRemovedSum; j < (MAX_FEMS - NRemovedSum); j++)
  {
    padSumMean += padSum[j];
    padSumErrorMean += padSum[j] * padSum[j];
  }
  padSumMean /= (float) (MAX_FEMS - 2 * NRemovedSum);
  padSumErrorMean /= (float) (MAX_FEMS - 2 * NRemovedSum);
  padSumErrorMean -= padSumMean * padSumMean;
  padSumErrorMean = sqrt(padSumErrorMean);
  // Then, Ratio
  for (int j = NRemovedAct; j < (PC_NINSTALLED * NHVSECTORS - NRemovedAct); j++)
  {
    clustActMean += clustAct[j];
    clustActErrorMean += clustAct[j] * clustAct[j];
  }
  clustActMean /= (float) (PC_NINSTALLED * NHVSECTORS - 2 * NRemovedAct);
  clustActErrorMean /= (float) (PC_NINSTALLED * NHVSECTORS - 2 * NRemovedAct);
  clustActErrorMean -= clustActMean * clustActMean;
  clustActErrorMean = sqrt(clustActErrorMean);

  //  printf("padSumMean: %6.4f padSumErrorMean: %6.4f \n", padSumMean, padSumErrorMean);
  //  printf("clustActMean: %6.4f clustActErrorMean: %6.4f \n", clustActMean, clustActErrorMean);

  // See where we have deviants..
  int count = 0;
  float val;

  installedplanes = 0;
  for (i = 0; i < NPLANES; i++)
  {
    if (PC_INSTALLED[i])
    {
      for (k = 0; k < NFEMSPERPLANE; k++)
      {
        val = (float) padProfPadSum[installedplanes]->GetBinContent(k + 1);
        if ((val > (highRelFEMCut * padSumMean)) || (val < (lowRelFEMCut * padSumMean)))
        {
          count++;
        }
      }
      installedplanes++;
    }
  }
  outfile << "<h4>Total number of problematic FEMs: " << count << "</h4>" << endl;

  count = 0;
  installedplanes = 0;
  for (i = 0; i < NPLANES; i++)
  {
    if (PC_INSTALLED[i])
    {
      for (k = 0; k < NHVSECTORS; k++)
      {
        val = (float) padProfClustActivity[installedplanes]->GetBinContent(k + 1);
        if ((val > (highRelHVCut * clustActMean)) || (val < (lowRelHVCut * clustActMean)))
        {
          count++;
        }
      }
      installedplanes++;
    }
  }
  outfile << "<h4>Total number of problematic HV sectors: " << count << "</h4>" << endl;

  // long intro text
  outfile << "<p>" << endl << "This document is organized in the following sections. " << endl << "<p>" << endl << "<ol style='margin-top:0in' start=1 type=1>" << endl << "<li><a href=\"#OverviewError\"" << endl << "title=\"OverviewError\">Error finding histograms</a>" << endl << "</li>" << endl
      << "<li><a href=\"#FEMError\"" << endl << "title=\"FEMError\">Details: problematic FEMs/ROCs/TGLs</a>" << endl << "</li>" << endl << "<li><a" << endl << "href=\"#HVError\"" << endl << "title=\"HVError\">Details: problematic HV sectors</a>" << endl << "</li>" << endl << "<p>" << endl
      << "<li><a href=\"#PrelPerf\"" << endl << "title=\"PrelPerf\">Efficiency monitoring. <B>EXPERTS ONLY</B></a>" << endl << "</li>" << endl << "</ol>" << endl << "<p>" << endl << "The PadMon screen watched by the shift crew can be found <a href=" << shortfilename << ">here </a>." << endl << "<p>"
      << endl << "Please direct any questions/comments to " << endl << "<a href=\"mailto:Anders.Oskarsson@hep.lu.se\">" << endl << "Anders Oskarsson</a> or " << endl << "<a href=\"mailto:phenix-pc-l@bnl.gov\">phenix-pc-l</a>." << endl << "<p>" << endl
      << "Hint: Click on the plots to enlarge them if the screen resolution is insufficient." << endl << "<p>" << endl << "All known electronic problems (current reading from database) have been filtered away." << endl << "<p>" << endl
      << "IMPORTANT. Any warning from the PC online monitoring should alert the shift crew to<br>" << endl << "check for tripped HV-channels. In case of a HV trip, you bring the channel up again and<br>" << endl << "wait for the next report from the monitoring." << endl << "<p>" << endl
      << "If there was no tripped channel and the system is still issuing warnings you should call<br>" << endl << "the PC-expert. However before any such action is taken, the quality of the information<br>" << endl << "should be verified.\n<p>" << "Reliable information is defined as:<br>" << endl
      << "<p>" << endl << "<ul><li>Number of events more than: " << 2 * MIN_NEVENTS_HTML << "</li>" << endl << "<li>The five Pad Sum profile histograms should have their data points at roughly the same level. <br>" << endl
      << "Individual bins may, however, deviate both up and down and PC3E has its points <br>" << endl << "more scattered than the others. </li>" << endl << "</ul><p>" << endl << "The online monitoring system can issue two types of warnings: severe and non-severe.<p>" << endl
      << "Severe warnings to the message system are based on histograms in section 1.<br>" << endl << "If checkup of HV trips did not identify the problem and the info was judged to be reliable<br>" << endl << "then call the PC-expert.<br>" << endl << "<p>" << endl
      << "Non-severe warnings to the message system concern anomalies that do not need immediate<br>" << endl << "attention. The PC-expert should be notified by a note in the online logbook.<br>" << endl << "<p>" << endl << "<hr>" << endl;

  //**************************************************************************************
  //*         Section 1 -- Overview                                                      *
  //**************************************************************************************
  outfile << "<h3><a name=\"OverviewError\"></a>" << endl << "Section 1. Histograms used for error finding</h3>" << endl;

  outfile << "<p>The plots are profile histograms. The absolute level depends on the quality of the event sample " << endl << "and on the system (higher in Au+Au than in p+p).\n<p>" << endl << "The left plot shows:<br>" << endl << "PadSum: Number of fired pads per event. Each bin is an FEM.<br>"
      << endl << "Should be: Flat, similar value on all arms.\n<p>" << endl << "The plot to the right shows: <br>" << "ClusterActivity: Number of clusters per event per wire. Each bin is a HVSector.<br>" << endl << "Should be: Flat.\n<p>" << endl << "Look for:<br>" << endl << "<ul>"
      << "<li>\"Hot ROCs\": A spike should be seen in both the left and right plot. <B>NON-SEVERE</B></li>" << endl << "<li>HV problem: One bin lower than the others in the right plot." << "The same in the left plot but a smaller dip than in the right plot. <B>SEVERE</B></li>" << endl
      << "<li>FEM problem: one bin deviating from the others in the left plot." << "Two or four bins close by, devaiting in the right plot. <B>SEVERE</B></li>" << endl << "</ul><p>" << endl << "HV problem is by far the most likely. FEM problems should have been detected by DAQ monitor.\n<p>" << endl
      << "More details for channels with values out of predefined bounds are shown in sections 2 and 3.<br>" << endl;

  installedplanes = 0;
  for (i = 0; i < NPLANES; i++)
  {
    if (PC_INSTALLED[i])
    {

      outfile << "<h3>" << planeinfo[i] << "</h3>" << endl;

      sprintf(tmpname, "PadSum%d_run", i);
      cl->htmlNamer(*this, tmpname, "png", fullfilename, shortfilename);
      cl->HistoToPng(padProfPadSum[installedplanes], fullfilename);

      outfile << "<a href=\"" << shortfilename.c_str() << "\"><IMG WIDTH=45% BORDER=0 SRC=" << shortfilename.c_str() << "></a>" << endl;

      sprintf(tmpname, "PadClustActivity%d_run", i);
      cl->htmlNamer(*this, tmpname, "png", fullfilename, shortfilename);
      cl->HistoToPng(padProfClustActivity[installedplanes], fullfilename);

      outfile << "<a href=\"" << shortfilename.c_str() << "\"><IMG WIDTH=45% BORDER=0 SRC=" << shortfilename.c_str() << "></a><p>" << endl;

      installedplanes++;

    }
  } // End Loop over NPLANES
  outfile << "<hr>" << endl;

  //**************************************************************************************
  //*      Section 2 - FEM errors                                                        *
  //**************************************************************************************
  outfile << "<h3><a name=\"FEMError\"></a>" << endl << "Section 2. Detailed histograms for errors, flagged in PADSum</h3>" << endl;

  outfile << "<p>The plots are 2D histograms and show padz and padx of a packet (FEM).<br>" << endl << "The center of the sector (z=0) is to the left.\n<p>" << endl << "Should be: scatter plot with homogeneous dot distribution.\n<p>" << endl << "Look for:<br>" << endl
      << "dark rectangle  - new hot ROC/TGL. <B>NON-SEVERE</B><br>" << endl << "Empty rectangle - inactive (or masked out) ROC/TGL (but watch out for low statistics). <B>NON-SEVERE</B><br>" << endl << "No dots at all - dead FEM. <B>SEVERE</B><br>" << endl
      << "No or few dots on either upper or lower half - HV-problem. <B>SEVERE</B>\n<p>" << endl << "A ROC has size 12 (in padz) times 4 (in padx).<br>" << endl << "A TGL has size 4 by 4.\n<p>" << endl;

  outfile << "The ROC column is obtained from (padz/12) + 1; // integer division<br>" << endl << "The ROC row is obtained from (padx%20)/5 + 1; // modulo and integer division<p>" << endl;

  int packetid;

  installedplanes = 0;

  for (i = 0; i < NPLANES; i++)
  {

    if (PC_INSTALLED[i])
    {

      outfile << "<h3>" << planeinfo[i] << "</h3>" << endl;

      for (k = 0; k < NFEMSPERPLANE; k++)
      {

        j = installedplanes * NFEMSPERPLANE + k;
        packetid = FIRSTPACKET + i * NFEMSPERPLANE + k;

        val = (float) padProfPadSum[installedplanes]->GetBinContent(k + 1);
        // cout<<" PacketId: "<<packetid<<"  Installed plane: "<<installedplanes<<" k: "<<k<<"  padSumMean: "<<padSumMean<<" val: "<<val<<endl;
        if ((val > (highRelFEMCut * padSumMean)) || (val < (lowRelFEMCut * padSumMean)))
        {

          outfile << "<h4> Packet " << packetid << " out of bounds </h4>" << endl;
          pc = i / 2;
          arm = 1 - i % 2;
          status = getFEMHardwareNotation(pc, arm, k, hardwarename);
          if (status != 0)
          {
            msg_control *padmsg = new msg_control(MSG_TYPE_MONITORING, MSG_SOURCE_PC, MSG_SEV_WARNING, "PadMonitor");

            cout << *padmsg << "pad_html ERROR getFEMHardwareNotation failed" << endl;
            delete padmsg;
          }
          outfile << "<h4> This packetid corresponds to " << hardwarename << "</h4>" << endl;
          outfile << "<p> This packet's PadSum: " << val << " Mean PadSum (all FEMs): " << padSumMean << "<p>" << endl;

          sprintf(tmpname, "PadFemPadxPadz%02d_run", j);
          cl->htmlNamer(*this, tmpname, "png", fullfilename, shortfilename);

          sprintf(idstring, "padFemPadxPadz%d", j);
          padFemPadxPadz[j] = (TH2*) cl->getHisto(idstring);

          cl->HistoToPng(padFemPadxPadz[j], fullfilename);

          outfile << "<a href=\"" << shortfilename.c_str() << "\"><IMG WIDTH=45% BORDER=0 SRC=" << shortfilename.c_str() << "></a><p>" << endl;

          count++;

        } // End if value outside range

      } // FEM loop
      installedplanes++;

    } // End if PC_INSTALLED

  } // End Loop over planes
  outfile << "<hr>" << endl;

  //**************************************************************************************
  //*      Section 3 - HV errors                                                         *
  //**************************************************************************************
  outfile << "<h3><a name=\"HVError\"></a>" << endl << "Section 3. Detailed histograms for errors, flagged in ClusterActivity</h3>" << endl;

  outfile << "<p>The plots are 1D histograms and show the cluster size distribution (number of " << "cells per cluster)<br>" << endl << "for a HV sector which has either too low or too high activity.\n<p>" << endl << "Should be:<br>" << endl
      << "At least as many entries in the bin corresponding to 2-cell clusters <br>" << endl << "as 1-cell clusters. The  Ratio 2cell/1cell should be 1-2. A tail towards more cells.<br>" << endl << "4 cells is often more likely than 3 or 5. Very little above 10 cells.\n<p>" << endl

      << "Look for: <br>" << endl << "<ul><li>HV tripped or dead FEM: Very few fillings (if dead FEM, there has to be at least " << "another bad HV sector). <B>SEVERE</B></li>" << endl << "<li>Too low gain: More 1 cell than 2 cell clusters but the distribution still has a tail. "
      << "<B>NON-SEVERE</B></li>" << endl << "<li>Too high gain: Many more 2 cell than 1 cell clusters. The distribution has a long tail. " << "<B>NON-SEVERE</B></li></ul>\n<p>" << endl;

  installedplanes = 0;
  for (i = 0; i < NPLANES; i++)
  {
    if (PC_INSTALLED[i])
    {
      outfile << "<h3>" << planeinfo[i] << "</h3>" << endl;

      for (k = 0; k < NHVSECTORS; k++)
      {
        j = installedplanes * NHVSECTORS + k;
        val = (float) padProfClustActivity[installedplanes]->GetBinContent(k + 1);
        if ((val > (highRelHVCut * clustActMean)) || (val < (lowRelHVCut * clustActMean)))
        {
          outfile << "<h4> HVSector " << k << " out of bounds </h4>" << endl;
          pc = i / 2;
          arm = 1 - i % 2;
          status = getHVHardwareNotation(pc, arm, k, hardwarename);
          if (status != 0)
          {
            msg_control *padmsg = new msg_control(MSG_TYPE_MONITORING, MSG_SOURCE_PC, MSG_SEV_WARNING, "PadMonitor");

            cout << *padmsg << "pad_html ERROR getHVHardwareNotation failed" << endl;
            delete padmsg;
          }
          outfile << "<h4> The hardware HV channel name is " << hardwarename << "</h4>" << endl;
          outfile << "<p> This HV sector's ClustActivity: " << val << " Mean ClustActivity (all HV sectors): " << clustActMean << "<p>" << endl;

          sprintf(tmpname, "PadClustSize%02d_run", j);
          cl->htmlNamer(*this, tmpname, "png", fullfilename, shortfilename);

          sprintf(idstring, "padClustSize%d", j);
          padClustSize[j] = cl->getHisto(idstring);

          cl->HistoToPng(padClustSize[j], fullfilename);
          outfile << "<a href=\"" << shortfilename.c_str() << "\"><IMG WIDTH=45% BORDER=0 SRC=" << shortfilename.c_str() << "></a><p>" << endl;

          count++;
        } // End if "val" outside range
      } // End HV loop

      installedplanes++;
    } // End if PC_INSTALLED
  } // End Loop over Planes
  outfile << "<hr>" << endl;

  //**************************************************************************************
  //*         Section 4 - Preliminary performance info                                   *
  //**************************************************************************************
  outfile << "<h3><a name=\"PrelPerf\"></a>" << endl << "Section 4. Histograms for efficiency checks. <B>EXPERTS ONLY</B></h3>" << endl;

  outfile << "<p>The plots are profile histograms. Each bin is a HVSector.\n<p>" << endl << "The left plot shows:<br>" << endl << "ClusterSize: Average number of cells per cluster.<br>" << endl << "Should be: Flat at 4-5 cells per cluster.\n<p>" << endl << "The plot on the right shows:<br>" << endl
      << "ClusterRatio: The ratio between 2-cell and 1-cell clusters per event.<br>" << endl << "Should be: All ratios between 1 and 2\n<p>" << endl

      << "Look for:<br>" << endl << "ClusterRatio is most important. If ratio is below 1, check if the HV-sector is among <br>" << endl << "the ones flagged earlier. The cluster cell distribution can be found under section 3. If<br>" << endl
      << "it is not flagged, the histogram has to be extracted from the stored root file.<p>" << "If everything looks like a reasonable" << endl << "distribution the gain has just drifted. It must be corrected by increasing the HV.<br>" << endl << "It should only be done by the PC-expert.\n<p>"
      << endl << "If the ratio is above 2 we are less critical since we don't loose efficency. Decrease HV<br>" << endl << "if it can be done without pushing other channels below ratio=1 in the same bulk supply.<br>" << endl << "This should be authorized by the PC-expert.\n<p>" << endl;

  installedplanes = 0;
  float bin1, bin2, ratio, ratioerr;

  for (i = 0; i < NPLANES; i++)
  {

    if (PC_INSTALLED[i])
    {
      outfile << "<h3>" << planeinfo[i] << "</h3>" << endl;

      sprintf(idstring, "padProfClustSize%d", i);
      padProfClustSize[installedplanes] = cl->getHisto(idstring);

      sprintf(tmpname, "PadProfClustSize%d_run", i);
      cl->htmlNamer(*this, tmpname, "png", fullfilename, shortfilename);

      cl->HistoToPng(padProfClustSize[installedplanes], fullfilename);

      outfile << "<a href=\"" << shortfilename.c_str() << "\"><IMG WIDTH=45% BORDER=0 SRC=" << shortfilename.c_str() << "></a>" << endl;

      TH1F *padProfClustRatio = new TH1F("padProfClustRatio", "Clust Ratio Histogram", NHVSECTORS, -0.5, NHVSECTORS - 0.5);

      // fill the cluster ratio histogram
      for (k = 0; k < NHVSECTORS; k++)
      {

        j = installedplanes * NHVSECTORS + k;
        sprintf(idstring, "padClustSize%d", j);
        padClustSize[j] = cl->getHisto(idstring);

        bin1 = (float) padClustSize[j]->GetBinContent(2); // bin x = size x-1..
        bin2 = (float) padClustSize[j]->GetBinContent(3);

        if ((bin1 > 0) && (bin2 > 0))
        {
          ratio = bin2 / bin1;
          // the relative error in the ratio is the sqrt of the other relative errors squared
          ratioerr = ratio * sqrt(1.0 / bin1 + 1.0 / bin2); // let's calc the errors explcitly.
          // Don't trust the errors from the histogram..
        }
        else
        { // something failed..
          ratio = -1.0;
          ratioerr = -1.0;
        }
        // remember that Fill and SetBinContent are not exactly the same
        padProfClustRatio->SetBinContent(k + 1, ratio);
        padProfClustRatio->SetBinError(k + 1, ratioerr);
      }

      sprintf(tmpname, "PadProfClustRatio%d_run", i);
      cl->htmlNamer(*this, tmpname, "png", fullfilename, shortfilename);
      cl->HistoToPng(padProfClustRatio, fullfilename, "", 0);
      outfile << "<a href=\"" << shortfilename.c_str() << "\"><IMG WIDTH=45% BORDER=0 SRC=" << shortfilename.c_str() << "></a><p>" << endl;

      if (padProfClustRatio)
      {
        delete padProfClustRatio;
      }

      installedplanes++;
    }
  }
  outfile << "<hr>" << endl;
  outfile << "</BODY>" << endl;
  outfile << "</HTML>" << endl;

  outfile.close();

  cl->SaveLogFile(*this);
  return 0;
}

int PadMonDraw::getHVHardwareNotation(short ipc, short iarm, short ihvsectid, char *hvhwname)
{
  if ((ipc < 0) || (ipc > 2) || (iarm < 0) || (iarm > 1) || (ihvsectid < 0) || (ihvsectid >= NHVSECTORS))
  {
    msg_control * padmsg = new msg_control(MSG_TYPE_MONITORING, MSG_SOURCE_PC, MSG_SEV_WARNING, "PadMonitor");

    cout << *padmsg << "padfuncs ERROR getHVHardwareNotation: arguments out of bounds." << "ipc = " << ipc << " iarm = " << iarm << " ihvsectid = " << ihvsectid << endl;
    delete padmsg;
    return -1;
  }
  else
  {
    short chamber = -1;
    short side = -1;
    short hwindex = -1;

    if (ipc == 0)
    {
      chamber = ihvsectid / 4;
      if (iarm == 0)
      {
        side = 1 - (ihvsectid % 2); // 0 and 2 is N on East
        // on this side, wire increases downward so we need to shift
        // hwindex order
        hwindex = (3 - (ihvsectid % 4)) / 2;
      }
      else
      {
        side = ihvsectid % 2; // 0 and 2 is S on West
        hwindex = (ihvsectid % 4) / 2;
      }
    }
    else
    {
      chamber = (ihvsectid % 16) / 4;
      side = ihvsectid / 16; // 0 to 15 is S, 16 to 31 N
      if (iarm == 0)
      {
        // on this side, wire increases downward so we need to shift
        // hwindex order
        hwindex = 3 - ihvsectid % 4;
      }
      else
      {
        hwindex = ihvsectid % 4;
      }
    }
    char charm[5], chside[5];

    if (iarm == 1)
    {
      sprintf(charm, "W"); // West arm
    }
    else
    {
      sprintf(charm, "E"); // East arm
    }

    if (side == 0)
    {
      sprintf(chside, "S"); // South side
    }
    else
    {
      sprintf(chside, "N"); // North side
    }
    char hardwarehv[20];
    // syntax ex. for hvname HV_PC1_W_S_0_0
    sprintf(hardwarehv, "HV_PC%d_%s_%s_%d_%d", ipc + 1, charm, chside, chamber, hwindex);
    strcpy(hvhwname, hardwarehv);

    return 0;
  } // End check input
} // End getHVHardwareNotation()

// ihvsectid should be on a per plane basis, i.e 0 to NFEMSPERPLANE-1
//**********************************************************************
int PadMonDraw::getFEMHardwareNotation(short ipc, short iarm, short ifemid, char *femhwname)
{
  if ((ipc < 0) || (ipc > 2) || (iarm < 0) || (iarm > 1) || (ifemid < 0) || (ifemid >= NFEMSPERPLANE))
  {
    msg_control * padmsg = new msg_control(MSG_TYPE_MONITORING, MSG_SOURCE_PC, MSG_SEV_WARNING, "PadMonitor");

    cout << *padmsg << "padfuncs ERROR getHVHardwareNotation: arguments out of bounds." << "ipc = " << ipc << " iarm = " << iarm << " ifemid = " << ifemid << endl;
    delete padmsg;
    return -1;
  }

  else
  {
    short subsector = ifemid % 8;
    short side = ifemid / 8;

    char charm[5], chside[5];

    if (iarm == 1)
    {
      sprintf(charm, "W"); // West arm
    }
    else
    {
      sprintf(charm, "E"); // East arm
    }

    if (side == 0)
    {
      sprintf(chside, "S"); // South side
    }
    else
    {
      sprintf(chside, "N"); // North side
    }

    char hardwarefem[20];
    // syntax ex. for hvname FEM.PC1.W.N.0
    sprintf(hardwarefem, "FEM.PC%d.%s.%s.%d", ipc + 1, charm, chside, subsector);
    strcpy(femhwname, hardwarefem);

    return 0;
  }
}
/* end getFEMHardwareNotation() */
int PadMonDraw::padSortVector(float val[], int NBins, int NPos)
{
  // This function sorts the elements in val[] such that the 
  // NPos largest elements are stored in the top NPos bins 
  // and the NPos smallest elements are stored in the bottom 
  // NPos bins. It is useful for calculating the truncated mean.
  // JN 030130
  if (NPos > NBins / 2)
  {
    return -1;
  }
  for (int i = 0; i < NPos; i++)
  {
    for (int j = i + 1; j < NBins; j++)
    {
      if (val[j] < val[i])
      {
        float temp = val[i];
        val[i] = val[j];
        val[j] = temp;
      }
      if (val[NBins - j - 1] > val[NBins - i - 1])
      {
        float temp = val[NBins - i - 1];
        val[NBins - i - 1] = val[NBins - j - 1];
        val[NBins - j - 1] = temp;
      }
    }
  }
  return 0;
}
/* end padSortVector */
