#include <DchMonDraw.h>
#include <OnlMonClient.h>

#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

DchMonDraw::DchMonDraw(): OnlMonDraw("DCHMON")
{

  TStyle* oldStyle = gStyle;
  dchStyle = new TStyle("dchStyle", "DCH Online Style");
  // dchStyle->SetStyle(dchStyle);
  dchStyle->Reset();

  colors[0] = MASKCOL;   //17;
  colors[1] = DEADCOL;   //1;
  colors[2] = COLDCOL;   //4;
  colors[3] = GOODCOL;   //3;
  colors[4] = HOTCOL;    //6;
  colors[5] = NOISYCOL;  //2;

  dchStyle->SetPalette(nCOLORS, colors);  //gStyle->SetPalette(5, colors);
  dchStyle->SetFrameBorderMode(0);
  oldStyle->cd();   //cd sets gstyle to oldstyle

  stat_threshold = 300; //this should be encoded too in the dc_info histo
  nevt = 0;
  old_noise_num_00 = 0;
  old_noise_num_01 = 0;
  old_noise_num_10 = 0;
  old_noise_num_11 = 0;
  new_noise_num_00 = 0;
  new_noise_num_01 = 0;
  new_noise_num_10 = 0;
  new_noise_num_11 = 0;
  missing_noise_num_00 = 0;
  missing_noise_num_01 = 0;
  missing_noise_num_10 = 0;
  missing_noise_num_11 = 0;
  hit_num_00 = 0;
  hit_num_01 = 0;
  hit_num_10 = 0;
  hit_num_11 = 0;
  avg_load_00 = 0;
  avg_load_10 = 0;
  avg_load_01 = 0;
  avg_load_11 = 0;
  dead_report = 0;

  dis00 = new TH2F("dis00", "East South", 100, -0.5, 99.5, 46, -0.5, 45.5);
  dis01 = new TH2F("dis01", "East North", 100, -0.5, 99.5, 46, -0.5, 45.5);
  dis10 = new TH2F("dis10", "West South", 100, -0.5, 99.5, 46, -0.5, 45.5);
  dis11 = new TH2F("dis11", "West North", 100, -0.5, 99.5, 46, -0.5, 45.5);

  dis00->SetStats(kFALSE);
  dis01->SetStats(kFALSE);
  dis10->SetStats(kFALSE);
  dis11->SetStats(kFALSE);
  return;
}

DchMonDraw::~DchMonDraw()
{
  delete dchStyle;

  delete dis00;
  delete dis01;
  delete dis10;
  delete dis11;
  delete hitrate_title_pave_text;
  //  delete hitrate_status_pave_text;
  delete hitrate_statistics_pave_text;
  delete hitrate_legend_mask_pave_text;
  delete hitrate_legend_dead_pave_text;
  delete hitrate_legend_cold_pave_text;
  delete hitrate_legend_good_pave_text;
  delete hitrate_legend_hot_pave_text;
  delete hitrate_legend_noisy_pave_text;
  delete pt00x1;  //xuv labels
  delete pt00x2;
  delete pt00u1;
  delete pt00v1;
  delete pt00u2;
  delete pt00v2;
  delete pt01x1;
  delete pt01x2;
  delete pt01u1;
  delete pt01v1;
  delete pt01u2;
  delete pt01v2;
  delete pt10x1;
  delete pt10x2;
  delete pt10u1;
  delete pt10v1;
  delete pt10u2;
  delete pt10v2;
  delete pt11x1;
  delete pt11x2;
  delete pt11u1;
  delete pt11v1;
  delete pt11u2;
  delete pt11v2;
  delete padnoisehit;
  delete paddeadhit;
  delete padloadhit;
  delete padt0hit;
  delete paddvhit;
  delete padswhit;
  delete padbadev;

  delete noise_Sumtitle;
  delete dead_Sumtitle;
  delete load_Sumtitle;
  delete t0_Sumtitle;
  delete vd_Sumtitle;
  delete sw_Sumtitle;
  delete Bad_title;

  return;
}

int DchMonDraw::Init()
{
  float heighttitlebox = 0.03;
  float col3 = 0.8;

  //  hitrate_title_pave_text = new TPaveText(.30, .94, .65, .98);
  hitrate_title_pave_text = new TPaveText(.30, .95, .65, .99);
  hitrate_title_pave_text->AddText("Drift Chamber Hit Rates");
  hitrate_title_pave_text->SetFillColor(8);

  //  hitrate_status_pave_text = new TPaveText(.0, .96, .10, .99);

  // Draws the Bottom Color Legend for MASK(known DEAD),newly DEAD,COLD,GOOD,HOT,NOISY
  int hitrate_leg_border_size = 2;

  hitrate_legend_mask_pave_text = new TPaveText(0.03, .01, 0.113, .03);
  hitrate_legend_mask_pave_text->AddText("Known DEAD");
  hitrate_legend_mask_pave_text->SetFillColor(MASKCOL);
  hitrate_legend_mask_pave_text->SetBorderSize(hitrate_leg_border_size);

  hitrate_legend_dead_pave_text = new TPaveText(0.113, .01, 0.197, .03);
  TText *legdead = hitrate_legend_dead_pave_text->AddText("Newly DEAD");
  legdead->SetTextColor(10);
  hitrate_legend_dead_pave_text->SetFillColor(DEADCOL);
  hitrate_legend_dead_pave_text->SetBorderSize(hitrate_leg_border_size);

  hitrate_legend_cold_pave_text = new TPaveText(0.197, .01, 0.28, .03);
  TText *legcold = hitrate_legend_cold_pave_text->AddText("COLD");
  legcold->SetTextColor(10);
  hitrate_legend_cold_pave_text->SetFillColor(COLDCOL);
  hitrate_legend_cold_pave_text->SetBorderSize(hitrate_leg_border_size);

  hitrate_legend_good_pave_text = new TPaveText(0.28, .01, 0.363, .03);
  hitrate_legend_good_pave_text->AddText("GOOD");
  hitrate_legend_good_pave_text->SetFillColor(GOODCOL);
  hitrate_legend_good_pave_text->SetBorderSize(hitrate_leg_border_size);

  hitrate_legend_hot_pave_text = new TPaveText(0.363, .01, 0.447, .03);
  hitrate_legend_hot_pave_text->AddText("HOT");
  hitrate_legend_hot_pave_text->SetFillColor(HOTCOL);
  hitrate_legend_hot_pave_text->SetBorderSize(hitrate_leg_border_size);

  hitrate_legend_noisy_pave_text = new TPaveText(0.447, .01, 0.53, .03);
  hitrate_legend_noisy_pave_text->AddText("NOISY");
  hitrate_legend_noisy_pave_text->SetFillColor(NOISYCOL);
  hitrate_legend_noisy_pave_text->SetBorderSize(hitrate_leg_border_size);

  //Status bar on right for Noisy,Dead, Hit Rate, T0, DV//DLAN
  ///////////////////////////////////////////////////////////
  float titleheight = 0.02;
  float padheight = 0.11;

  // hitrate_statistics_pave_text = new TPaveText(.75, .94, .99, .99);
  hitrate_statistics_pave_text = new TPaveText(col3 + 0.015 + heighttitlebox, .93, .995, .995);
  hitrate_statistics_pave_text->SetBorderSize(2);

  noise_Sumtitle = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 1 * titleheight - 0 * padheight, 1, .92 - 0 * titleheight - 0 * padheight);
  noise_Sumtitle->AddText("Noisy Channels Status");
  noise_Sumtitle->SetFillColor(7);

  padnoisehit = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 1 * titleheight - 1 * padheight, 1, .92 - 1 * titleheight - 0 * padheight);
  padnoisehit->SetFillColor(19);

  dead_Sumtitle = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 2 * titleheight - 1 * padheight, 1, .92 - 1 * titleheight - 1 * padheight);
  dead_Sumtitle->AddText("Dead Channel Status");
  dead_Sumtitle->SetFillColor(7);

  paddeadhit = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 2 * titleheight - 2 * padheight, 1, .92 - 2 * titleheight - 1 * padheight);
  paddeadhit->SetFillColor(19);

  load_Sumtitle = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 3 * titleheight - 2 * padheight, 1, .92 - 2 * titleheight - 2 * padheight);
  load_Sumtitle->AddText("Hit Rate Status");
  load_Sumtitle->SetFillColor(7);

  padloadhit = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 3 * titleheight - 3 * padheight, 1, .92 - 3 * titleheight - 2 * padheight);
  padloadhit->SetFillColor(19);

  t0_Sumtitle = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 4 * titleheight - 3 * padheight, 1, .92 - 3 * titleheight - 3 * padheight);
  t0_Sumtitle->AddText("T0 Status");
  t0_Sumtitle->SetFillColor(7);

  padt0hit = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 4 * titleheight - 4 * padheight, 1, .92 - 4 * titleheight - 3 * padheight);
  padt0hit->SetFillColor(19);

  vd_Sumtitle = new TPaveText(col3 + 0.02  + heighttitlebox, 0.92 - 5 * titleheight - 4 * padheight, 1, .92 - 4 * titleheight - 4 * padheight);
  vd_Sumtitle->AddText("DV Status");
  vd_Sumtitle->SetFillColor(7);

  paddvhit = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 5 * titleheight - 5 * padheight, 1, .92 - 5 * titleheight - 4 * padheight);
  paddvhit->SetFillColor(19);

  sw_Sumtitle = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 6 * titleheight - 5 * padheight, 1, .92 - 5 * titleheight - 5 * padheight);
  sw_Sumtitle->AddText("SW Eff. Status");
  sw_Sumtitle->SetFillColor(7);

  padswhit = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 6 * titleheight - 6 * padheight, 1, .92 - 6 * titleheight - 5 * padheight);
  padswhit->SetFillColor(19);

  Bad_title = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 7 * titleheight - 6 * padheight, 1, .92 - 6 * titleheight - 6 * padheight);
  Bad_title->AddText("Rejected events Status");
  Bad_title->SetFillColor(7);

  padbadev = new TPaveText(col3 + 0.02 + heighttitlebox, 0.92 - 7 * titleheight - 7 * padheight, 1, .92 - 7 * titleheight - 6 * padheight);
  padbadev->SetFillColor(19);


  //////////////////////////////////////////////////////////////////

  int sec_label_col = 0; //previously set to be 18 (grey), 0=white
  int sec_label_bordsize = 0; //default is 4
  pt00x1 = new TPaveText( -2.47969, 2.55004, -0.639362, 9.44381, "br");
  pt00x1->SetFillColor(sec_label_col);
  pt00x1->SetBorderSize(sec_label_bordsize);
  pt00x1->AddText("X1");
  pt00x2 = new TPaveText( -2.47912, 25.4395, -0.627273, 32.3223, "br");
  pt00x2->SetFillColor(sec_label_col);
  pt00x2->SetBorderSize(sec_label_bordsize);
  pt00x2->AddText("X2");
  pt00u1 = new TPaveText( -2.07015, 13.1115, -1.10226, 16.1207, "br");
  pt00u1->SetFillColor(sec_label_col);
  pt00u1->SetBorderSize(sec_label_bordsize);
  pt00u1->AddText("U1");
  pt00v1 = new TPaveText( -2.05825, 17.9832, -1.09024, 20.9944, "br");
  pt00v1->SetFillColor(sec_label_col);
  pt00v1->SetBorderSize(sec_label_bordsize);
  pt00v1->AddText("V1");
  pt00u2 = new TPaveText( -2.01616, 36.0505, -1.04815, 39.0617, "br");
  pt00u2->SetFillColor(sec_label_col);
  pt00u2->SetBorderSize(sec_label_bordsize);
  pt00u2->AddText("U2");
  pt00v2 = new TPaveText( -1.97407, 41.0692, -1.00606, 44.0804, "br");
  pt00v2->SetFillColor(sec_label_col);
  pt00v2->SetBorderSize(sec_label_bordsize);
  pt00v2->AddText("V2");

  pt01x1 = new TPaveText( -2.47969, 2.55004, -0.639362, 9.44381, "br");
  pt01x1->SetFillColor(sec_label_col);
  pt01x1->SetBorderSize(sec_label_bordsize);
  pt01x1->AddText("X1");
  pt01x2 = new TPaveText( -2.47912, 25.4395, -0.627273, 32.3223, "br");
  pt01x2->SetFillColor(sec_label_col);
  pt01x2->SetBorderSize(sec_label_bordsize);
  pt01x2->AddText("X2");
  pt01u1 = new TPaveText( -2.07015, 13.1115, -1.10226, 16.1207, "br");
  pt01u1->SetFillColor(sec_label_col);
  pt01u1->SetBorderSize(sec_label_bordsize);
  pt01u1->AddText("U1");
  pt01v1 = new TPaveText( -2.05825, 17.9832, -1.09024, 20.9944, "br");
  pt01v1->SetFillColor(sec_label_col);
  pt01v1->SetBorderSize(sec_label_bordsize);
  pt01v1->AddText("V1");
  pt01u2 = new TPaveText( -2.01616, 36.0505, -1.04815, 39.0617, "br");
  pt01u2->SetFillColor(sec_label_col);
  pt01u2->SetBorderSize(sec_label_bordsize);
  pt01u2->AddText("U2");
  pt01v2 = new TPaveText( -1.97407, 41.0692, -1.00606, 44.0804, "br");
  pt01v2->SetFillColor(sec_label_col);
  pt01v2->SetBorderSize(sec_label_bordsize);
  pt01v2->AddText("V2");

  pt10x1 = new TPaveText( -2.47969, 2.55004, -0.639362, 9.44381, "br");
  pt10x1->SetFillColor(sec_label_col);
  pt10x1->SetBorderSize(sec_label_bordsize);
  pt10x1->AddText("X1");
  pt10x2 = new TPaveText( -2.47912, 25.4395, -0.627273, 32.3223, "br");
  pt10x2->SetFillColor(sec_label_col);
  pt10x2->SetBorderSize(sec_label_bordsize);
  pt10x2->AddText("X2");
  pt10u1 = new TPaveText( -2.07015, 13.1115, -1.10226, 16.1207, "br");
  pt10u1->SetFillColor(sec_label_col);
  pt10u1->SetBorderSize(sec_label_bordsize);
  pt10u1->AddText("U1");
  pt10v1 = new TPaveText( -2.05825, 17.9832, -1.09024, 20.9944, "br");
  pt10v1->SetFillColor(sec_label_col);
  pt10v1->SetBorderSize(sec_label_bordsize);
  pt10v1->AddText("V1");
  pt10u2 = new TPaveText( -2.01616, 36.0505, -1.04815, 39.0617, "br");
  pt10u2->SetFillColor(sec_label_col);
  pt10u2->SetBorderSize(sec_label_bordsize);
  pt10u2->AddText("U2");
  pt10v2 = new TPaveText( -1.97407, 41.0692, -1.00606, 44.0804, "br");
  pt10v2->SetFillColor(sec_label_col);
  pt10v2->SetBorderSize(sec_label_bordsize);
  pt10v2->AddText("V2");

  pt11x1 = new TPaveText( -2.47969, 2.55004, -0.639362, 9.44381, "br");
  pt11x1->SetFillColor(sec_label_col);
  pt11x1->SetBorderSize(sec_label_bordsize);
  pt11x1->AddText("X1");
  pt11x2 = new TPaveText( -2.47912, 25.4395, -0.627273, 32.3223, "br");
  pt11x2->SetFillColor(sec_label_col);
  pt11x2->SetBorderSize(sec_label_bordsize);
  pt11x2->AddText("X2");
  pt11u1 = new TPaveText( -2.07015, 13.1115, -1.10226, 16.1207, "br");
  pt11u1->SetFillColor(sec_label_col);
  pt11u1->SetBorderSize(sec_label_bordsize);
  pt11u1->AddText("U1");
  pt11v1 = new TPaveText( -2.05825, 17.9832, -1.09024, 20.9944, "br");
  pt11v1->SetFillColor(sec_label_col);
  pt11v1->SetBorderSize(sec_label_bordsize);
  pt11v1->AddText("V1");
  pt11u2 = new TPaveText( -2.01616, 36.0505, -1.04815, 39.0617, "br");
  pt11u2->SetFillColor(sec_label_col);
  pt11u2->SetBorderSize(sec_label_bordsize);
  pt11u2->AddText("U2");
  pt11v2 = new TPaveText( -1.97407, 41.0692, -1.00606, 44.0804, "br");
  pt11v2->SetFillColor(sec_label_col);
  pt11v2->SetBorderSize(sec_label_bordsize);
  pt11v2->AddText("V2");
  return 0;
}

int DchMonDraw::MakeHitRateCanvas()
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  TStyle* oldStyle = gStyle;
  dchStyle->cd();

  DcHitRateCanvas = new TCanvas("DcHitRateCanvas1", "Drift Chamber Hit Rate", -1, 0, xsize, ysize);
  gSystem->ProcessEvents();
  DcHitRateCanvas->SetWindowPosition(0, 0);
  DcHitRateCanvas->SetFillColor(10);
  DcHitRateCanvas->Draw();

  hitrate_title_pave_text->Draw();
  //  hitrate_status_pave_text->Draw(); // I don't see the point in showing this
  hitrate_statistics_pave_text->Draw();
  hitrate_legend_mask_pave_text->Draw();
  hitrate_legend_dead_pave_text->Draw();
  hitrate_legend_cold_pave_text->Draw();
  hitrate_legend_good_pave_text->Draw();
  hitrate_legend_hot_pave_text->Draw();
  hitrate_legend_noisy_pave_text->Draw();

  //**********************************************************

  //paddc3_00 = new TPad("paddc3_00", "Pad 00", .02, .49, .32, .93);// Old One //DLAN
  paddc3_00 = new TPad("paddc3_00", "Pad 00", .02, .49, .28, .93);// East South Hit Rates Pad
  paddc3_00->SetFillColor(10);
  paddc3_00->Draw();

  //paddc3_01 = new TPad("paddc3_01", "Pad 01", .34, .49, .62, .93);//
  paddc3_01 = new TPad("paddc3_01", "Pad 01", .29, .49, .55, .93);// East North Hit Rates Pad
  paddc3_01->SetFillColor(10);
  paddc3_01->Draw();

  //paddc3_10 = new TPad("paddc3_10", "Pad 10", .02, .03, .32, .47);//
  paddc3_10 = new TPad("paddc3_10", "Pad 10", .02, .03, .28, .47);// West South Hit Rates Pad
  paddc3_10->SetFillColor(10);
  paddc3_10->Draw();

  //paddc3_11 = new TPad("paddc3_11", "Pad 11", .34, .03, .62, .47);//
  paddc3_11 = new TPad("paddc3_11", "Pad 11", .29, .03, .55, .47);// West North Hit Rates Pad
  paddc3_11->SetFillColor(10);
  paddc3_11->Draw();

  // For Timing Distributions

  //paddc3_02 = new TPad("paddc3_02", "Pad 02", .65, .49, .98, .93);// Norm. T dist. DC.E Pad
  paddc3_02 = new TPad("paddc3_02", "Pad 02", .56, .49, .84, .93);// Norm. T dist. DC.E Pad
  paddc3_02->SetFillColor(10);
  paddc3_02->Draw();

  //paddc3_12 = new TPad("paddc3_12", "Pad 12", .65, .03, .99, .47);// Norm. T dist. DC.W Pad
  paddc3_12 = new TPad("paddc3_12", "Pad 12", .56, .03, .84, .47);// Norm. T dist. DC.W Pad
  paddc3_12->SetFillColor(10);
  paddc3_12->Draw();

  noise_Sumtitle->Draw();
  padnoisehit->Draw();
  dead_Sumtitle->Draw();
  paddeadhit->Draw();
  load_Sumtitle->Draw();
  padloadhit->Draw();
  t0_Sumtitle->Draw();
  padt0hit->Draw();
  vd_Sumtitle->Draw();
  paddvhit->Draw();
  sw_Sumtitle->Draw();
  padswhit->Draw();
  Bad_title->Draw();
  padbadev->Draw();

  oldStyle->cd();
  return 0;
}



int DchMonDraw::Draw(const char *what)
{

  TStyle* oldStyle = gStyle;
  dchStyle->cd();
  decodeInfoFromHistogram();

  if (! gROOT->FindObject("DcHitRateCanvas"))
    {
      MakeHitRateCanvas();
    }

  // chp: Sean please leave the return codes intact
  // in the future
  int iret = 0;
  // No more history canvas, so "what" isn't used
  iret += DrawHitRateCanvas();
  DcHitRateCanvas->SetEditable(0);

  oldStyle->cd();
  return iret;
}


int DchMonDraw::DrawHitRateCanvas()
{
  DcHitRateCanvas->SetEditable(1);
  OnlMonClient *cl = OnlMonClient::instance();
  DcHitRateCanvas->cd();
  OnlMonClient *Onlmonclient = OnlMonClient::instance();
  TH1 *h_dc1norm = Onlmonclient->getHisto("h_dc1norm");
  TH1 *h_dc2norm = Onlmonclient->getHisto("h_dc2norm");
  TH1 *dc_dens_00 = Onlmonclient->getHisto("dc_dens_00");
  TH1 *dc_dens_01 = Onlmonclient->getHisto("dc_dens_01");
  TH1 *dc_dens_10 = Onlmonclient->getHisto("dc_dens_10");
  TH1 *dc_dens_11 = Onlmonclient->getHisto("dc_dens_11");
  //  hitrate_status_pave_text->Clear();
  DcHitRateCanvas->cd();

  if (h_dc1norm == NULL || h_dc2norm == NULL || dc_dens_00 == NULL || dc_dens_01 == NULL || dc_dens_10 == NULL || dc_dens_11 == NULL)
    {
      //If Server is dead
      DcHitRateCanvas->Clear("D");
      TText FatalMsg;
      FatalMsg.SetTextFont(62);
      FatalMsg.SetTextSize(0.1);
      FatalMsg.SetTextColor(4);
      FatalMsg.SetNDC();  // set to normalized coordinates
      FatalMsg.SetTextAlign(23); // center/top alignment
      FatalMsg.DrawText(0.5, 0.9, "DCHMONITOR");
      FatalMsg.SetTextAlign(22); // center/center alignment
      FatalMsg.DrawText(0.5, 0.5, "SERVER");
      FatalMsg.SetTextAlign(21); // center/bottom alignment
      FatalMsg.DrawText(0.5, 0.1, "DEAD");
      DcHitRateCanvas->Modified();
      DcHitRateCanvas->Update();

      return 1;
    }

  //   hitrate_title_pave_text->Draw();
  //   hitrate_status_pave_text->Draw();
  //   hitrate_statistics_pave_text->Draw();
  //   hitrate_legend_dead_pave_text->Draw();
  //   hitrate_legend_cold_pave_text->Draw();
  //   hitrate_legend_good_pave_text->Draw();
  //   hitrate_legend_hot_pave_text->Draw();
  //   hitrate_legend_noisy_pave_text->Draw();

  ///////////////////////////
  //runnumber and events
  //////////////////////////
  hitrate_statistics_pave_text->Clear();
  hitrate_statistics_pave_text->SetFillColor(17);
  ostringstream nevt_txt, runNum_txt, time_txt;
  nevt_txt << "Events processed: " << nevt;
  runNum_txt << "Run Number: " << cl->RunNumber();
  time_t evttime = cl->EventTime("CURRENT");
  time_txt << ctime(&evttime);
 //  date_txt << 
  hitrate_statistics_pave_text->AddText(runNum_txt.str().c_str());
  hitrate_statistics_pave_text->AddText(time_txt.str().c_str());
  hitrate_statistics_pave_text->AddText(nevt_txt.str().c_str());
  DcHitRateCanvas->Modified();
  //DcHitRateCanvas->cd();

  paddc3_00->SetPad(.02, .49, .28, .93);
  paddc3_01->SetPad(.29, .49, .55, .93);
  paddc3_10->SetPad(.02, .03, .28, .47);
  paddc3_11->SetPad(.29, .03, .55, .47);
  paddc3_02->SetPad(.56, .49, .84, .93);
  paddc3_12->SetPad(.56, .03, .84, .47);

  //  hitrate_status_pave_text->SetX1NDC(0);
  //  hitrate_status_pave_text->SetX2NDC(0.10);
  //  hitrate_status_pave_text->SetY1NDC(0.96);
  //  hitrate_status_pave_text->SetY2NDC(0.99);
  //  hitrate_status_pave_text->AddText("SERVER: LIVE");
  //  hitrate_status_pave_text->SetFillColor(3);

  paddc3_02->cd();
  //benjil: highlight the bin where the t0 is thought to be from
  TH1F *h_dc1norm_t0 = (TH1F*)h_dc1norm->Clone("h_dc1norm_t0");
  h_dc1norm_t0->Reset();
  h_dc1norm_t0->SetBinContent(tzero_east,h_dc1norm->GetBinContent(tzero_east));
  h_dc1norm_t0->SetFillColor(1);
  h_dc1norm->Draw();
  h_dc1norm_t0->Draw("same");

  paddc3_12->cd();
  TH1F *h_dc2norm_t0 = (TH1F*)h_dc2norm->Clone("h_dc2norm_t0");
  h_dc2norm_t0->Reset();
  h_dc2norm_t0->SetBinContent(tzero_west,h_dc2norm->GetBinContent(tzero_west));
  h_dc2norm_t0->SetFillColor(1);
  h_dc2norm->Draw();
  h_dc2norm_t0->Draw("same");

  //  int planeboundaries[] =
  //  {
  //    11, 15, 19, 31, 35, 39
  //  };

  dis00->Reset();
  dis01->Reset();
  dis10->Reset();
  dis11->Reset();


  //rewrite the data information in a form suitable for presentation
  for (int plane = 0; plane < 40 ; plane++)
    {
      for (int board = 0; board < 80; board++)
        {
          int keystone = board / 4;

          int repeat;

          if (plane < 12)
            {
              repeat = 0;
            }
          else if (plane < 16)
            {
              repeat = 1;
            }
          else if (plane < 20)
            {
              repeat = 2;
            }
          else if (plane < 32)
            {
              repeat = 3;
            }
          else if (plane < 36)
            {
              repeat = 4;
            }
          else
            {
              repeat = 5;
            }

          dis00->SetBinContent(board + keystone + 1, plane + repeat + 1,
                               dc_dens_00->GetBinContent(board + 1, plane + 1));
          dis01->SetBinContent(board + keystone + 1, plane + repeat + 1,
                               dc_dens_01->GetBinContent(board + 1, plane + 1));
          dis10->SetBinContent(board + keystone + 1, plane + repeat + 1,
                               dc_dens_10->GetBinContent(board + 1, plane + 1));
          dis11->SetBinContent(board + keystone + 1, plane + repeat + 1,
                               dc_dens_11->GetBinContent(board + 1, plane + 1));

        } //board
    } //plane


  //redo the axis

  TAxis *xaxis00 = dis00->GetXaxis();
  xaxis00->SetLimits( -0.4, 19.6); //sweet spot
  xaxis00->SetNdivisions(20);
  xaxis00->SetLabelSize(0.03);
  TAxis *yaxis00 = dis00->GetYaxis();
  yaxis00->SetNdivisions(0);

  TAxis *xaxis01 = dis01->GetXaxis();
  xaxis01->SetLimits( -0.4, 19.6); //sweet spot
  xaxis01->SetNdivisions(20);
  xaxis01->SetLabelSize(0.03);
  TAxis *yaxis01 = dis01->GetYaxis();
  yaxis01->SetNdivisions(0);

  TAxis *xaxis10 = dis10->GetXaxis();
  xaxis10->SetLimits( -0.4, 19.6); //sweet spot
  xaxis10->SetNdivisions(20);
  xaxis10->SetLabelSize(0.03);
  TAxis *yaxis10 = dis10->GetYaxis();
  yaxis10->SetNdivisions(0);

  TAxis *xaxis11 = dis11->GetXaxis();
  xaxis11->SetLimits( -0.4, 19.6); //sweet spot
  xaxis11->SetNdivisions(20);
  xaxis11->SetLabelSize(0.03);
  TAxis *yaxis11 = dis11->GetYaxis();
  yaxis11->SetNdivisions(0);


  //////////////////////////////////////////
  //draw paddc3_0 and related gizmos
  //////////////////////////////////////////
  paddc3_00->cd();
  gStyle->SetPalette(nCOLORS, colors);      //added for color change
  dis00->Draw("col");
  paddc3_00->Modified();

  pt00x1->Draw();
  pt00x2->Draw();
  pt00u1->Draw();
  pt00v1->Draw();
  pt00u2->Draw();
  pt00v2->Draw();

  paddc3_00->Modified();
  DcHitRateCanvas->Update();


  //////////////////////////////////////////////////
  // paddc3_01 drawing
  ///////////////////////////////////////////////////

  paddc3_01->cd();
  gStyle->SetPalette(nCOLORS, colors);      //add
  dis01->Draw("col");
  paddc3_01->Modified();

  pt01x1->Draw();
  pt01x2->Draw();
  pt01u1->Draw();
  pt01v1->Draw();
  pt01u2->Draw();
  pt01v2->Draw();

  paddc3_01->Modified();
  DcHitRateCanvas->Update();


  ////////////////////////////////////////////////////////////
  // paddc3_10 drawing
  //////////////////////////////////////////////////////////

  paddc3_10->cd();
  gStyle->SetPalette(nCOLORS, colors);         //add
  dis10->Draw("col");
  //gStyle->SetPalette(1,0);

  paddc3_10->Modified();

  pt01x1->Draw();
  pt01x2->Draw();
  pt01u1->Draw();
  pt01v1->Draw();
  pt01u2->Draw();
  pt01v2->Draw();

  paddc3_10->Modified();
  DcHitRateCanvas->Update();

  /////////////////////////////////////////////////////////
  // paddc3_11 drawing
  ////////////////////////////////////////////////////////

  paddc3_11->cd();
  gStyle->SetPalette(nCOLORS, colors);       //add
  dis11->Draw("col");
  //gStyle->SetPalette(1,0);

  paddc3_11->Modified();

  pt01x1->Draw();
  pt01x2->Draw();
  pt01u1->Draw();
  pt01v1->Draw();
  pt01u2->Draw();
  pt01v2->Draw();


  paddc3_11->Modified();
  DcHitRateCanvas->Update();

  ///////////////////////////////////
  // Draw the Status bar on the right
  //////////////////////////////////

  // The following are various variables defined for Status Text and Color Alerts

  int new_noisy = new_noise_num_00 + new_noise_num_01 + new_noise_num_10 + new_noise_num_11;
  TText *mtext;
  bool anychange = (((missing_noise_num_00 + missing_noise_num_01) > 0) || ((missing_noise_num_10 + missing_noise_num_11) > 0) || (new_noise_num_00 > 0) || (new_noise_num_01 > 0) || (new_noise_num_10 > 0) || (new_noise_num_01 > 0));
  //set_species_load_alerts();

  bool allhigherthanlow = ((avg_load_11 > LLOAD_ALERT_RED) && (avg_load_10 > LLOAD_ALERT_RED) && (avg_load_01 > LLOAD_ALERT_RED) && (avg_load_00 > LLOAD_ALERT_RED));
  bool alllowerthanhigh = ((avg_load_11 < HLOAD_ALERT_RED) && (avg_load_10 < HLOAD_ALERT_RED) && (avg_load_01 < HLOAD_ALERT_RED) && (avg_load_00 < HLOAD_ALERT_RED));
  bool allhigherthanlow1 = ((avg_load_11 > LLOAD_ALERT_YEL) && (avg_load_10 > LLOAD_ALERT_YEL) && (avg_load_01 > LLOAD_ALERT_YEL) && (avg_load_00 > LLOAD_ALERT_YEL));
  bool alllowerthanhigh1 = ((avg_load_11 < HLOAD_ALERT_YEL) && (avg_load_10 < HLOAD_ALERT_YEL) && (avg_load_01 < HLOAD_ALERT_YEL) && (avg_load_00 < HLOAD_ALERT_YEL));
  float fracdveast = fabs(vdrift_east - template_vdrift_east) / template_vdrift_east;
  float fracdvwest = fabs(vdrift_west - template_vdrift_west) / template_vdrift_west;

  // Clear old info from status bar
  padnoisehit->Clear();
  paddeadhit->Clear();
  padloadhit->Clear();
  padt0hit->Clear();
  paddvhit->Clear();
  padswhit->Clear();
  padbadev->Clear();

  ostringstream msg;

  // Noise
  msg << "E:" << new_noisy_east_n << " noisy channels";
  padnoisehit->AddText(msg.str().c_str());
  msg.str("");
  msg << "W:" << new_noisy_west_n << " noisy channels";
  padnoisehit->AddText(msg.str().c_str());
  msg.str("");
  // Dead
  msg << "E:" << new_dead_east_n << " dead channels";
  paddeadhit->AddText(msg.str().c_str());
  msg.str("");
  msg << "W:" << new_dead_west_n << " dead channels";
  paddeadhit->AddText(msg.str().c_str());
  msg.str("");

  // Load
  msg.precision(3);
  msg << "SE: " << avg_load_00;
  padloadhit->AddText(msg.str().c_str());
  msg.str("");
  msg << "NE: " << avg_load_01;
  padloadhit->AddText(msg.str().c_str());
  msg.str("");
  msg << "SW: " << avg_load_10;
  padloadhit->AddText(msg.str().c_str());
  msg.str("");
  msg << "NW: " << avg_load_11;
  padloadhit->AddText(msg.str().c_str());
  msg.str("");

  //T zero
  msg << "T0 East (timebins) = " << tzero_east;
  padt0hit->AddText(msg.str().c_str());
  msg.str("");
  msg << "T0 West (timebins) = " << tzero_west;
  padt0hit->AddText(msg.str().c_str());
  msg.str("");

  // drift velocity
  msg << "DV East = " << vdrift_east*10000 << "#mum/ns";
  paddvhit->AddText(msg.str().c_str());
  msg.str("");
  msg << "DV West = " << vdrift_west*10000 << "#mum/ns";
  paddvhit->AddText(msg.str().c_str());
  msg.str("");

  // SW Eff.
  msg << "SW Eff East: " << eff_east*100 << "%";
  padswhit->AddText(msg.str().c_str());
  msg.str("");
  msg << "SW Eff West: " << eff_west*100 << "%";
  padswhit->AddText(msg.str().c_str());
  msg.str("");

  // Bad evv Eff.
  msg << "Rejected Events:  " << bad_events;
  padbadev->AddText(msg.str().c_str());
  msg.str("");
  if (nevt != 0)
    {
      if (float(bad_events) / nevt*1000.0 < 10) msg << "Relative : " << float(bad_events) / nevt*1000.0 << " x 10^{-1} %";
      if (float(bad_events) / nevt*1000.0 > 10) msg << "Relative : " << float(bad_events) / nevt*100.0 << " %";
    }
  else
    {
      msg << "Relative : 0 %";
    }
  padbadev->AddText(msg.str().c_str());
  msg.str("");

  //////////////////////////////////////////


  /////////////////////////////////////////
  //Text Alerts for the Status Bar on right
  /////////////////////////////////////////

  // Noise
  if (anychange)
    {
      msg << "RELATIVE TO TEMPLATE:";
      mtext = padnoisehit->AddText(msg.str().c_str());
      msg.str("");
      mtext->SetTextSize(0.015);
      mtext->SetTextColor(4);
    }

  if ((missing_noise_num_00 + missing_noise_num_01) > 0)
    {
      msg << "E:" << missing_noise_num_00 + missing_noise_num_01 << " channels recovered";
      padnoisehit->AddText(msg.str().c_str());
      msg.str("");
    }
  if ((missing_noise_num_10 + missing_noise_num_11) > 0)
    {
      msg << "W:" << missing_noise_num_10 + missing_noise_num_11 << " channels recovered";
      padnoisehit->AddText(msg.str().c_str());
      msg.str("");
    }
  if ((new_noise_num_00 + new_noise_num_01) > 0)
    {
      msg << "E:" << new_noise_num_00 + new_noise_num_01 << " new noisy channels";
      padnoisehit->AddText(msg.str().c_str());
      msg.str("");
    }
  if ((new_noise_num_10 + new_noise_num_11) > 0)
    {
      msg << "W:" << new_noise_num_10 << " new noisy channels";
      padnoisehit->AddText(msg.str().c_str());
      msg.str("");
    }

  //Dead
  if ((abs(new_dead_east_n - template_dead_east_n) > 0) || (abs(new_dead_west_n - template_dead_west_n) > 0))
    {
      msg << "DIFFERENCE FROM TEMPLATE :";
      mtext = paddeadhit->AddText(msg.str().c_str());
      msg.str("");
      mtext->SetTextSize(0.013);
      mtext->SetTextColor(4);
    }
  if (abs(new_dead_east_n - template_dead_east_n) > 0)
    {
      msg << "E:" << new_dead_east_n - template_dead_east_n << " dead channels";
      paddeadhit->AddText(msg.str().c_str());
      msg.str("");
    }
  if (abs(new_dead_west_n - template_dead_west_n) > 0)
    {
      msg << "W:" << new_dead_west_n - template_dead_west_n << " dead channels";
      paddeadhit->AddText(msg.str().c_str());
      msg.str("");
    }

  // Load
  // no text alerts for load

  //T zero
  if ((abs(tzero_east - template_tzero_east) > 2) || (abs(tzero_west - template_tzero_west) > 2))
    {
      msg << "DIFFERENCE FROM TEMPLATE :";
      mtext = padt0hit->AddText(msg.str().c_str());
      msg.str("");
      mtext->SetTextSize(0.013);
      mtext->SetTextColor(4);
      msg << "E: " << tzero_east - template_tzero_east << " time bin";
      padt0hit->AddText(msg.str().c_str());
      msg.str("");
      msg << "W: " << tzero_west - template_tzero_west << " time bin";
      padt0hit->AddText(msg.str().c_str());
      msg.str("");
    }

  // drift velocity
  if ((fabs(fracdveast) > 0.02) || (fabs(fracdvwest) > 0.02))
    {
      msg << "DIFFERENCE FROM TEMPLATE:";
      mtext = paddvhit->AddText(msg.str().c_str());
      msg.str("");
      mtext->SetTextSize(0.013);
      mtext->SetTextColor(4);
      msg << "E: " << 100.0*fracdveast << " %";
      paddvhit->AddText(msg.str().c_str());
      msg.str("");
      msg << "W: " << 100.0*fracdvwest << " %";
      paddvhit->AddText(msg.str().c_str());
      msg.str("");
    }

  //SW Efficiency
  //no text alerts for SW Eff.

  ///////////////////////////////////////
  // Color Alerts for Status Bar on Right
  ///////////////////////////////////////
  // noise
  if ((new_noisy) < NOIS_ALERT_YEL)
    {
      padnoisehit->SetFillColor(3);
    }
  else
    {
      if ((new_noisy) < NOIS_ALERT_RED)
        {
          padnoisehit->SetFillColor(5);
        }
      else
        {
          padnoisehit->SetFillColor(2);
        }
    }

  // Dead
  if (((new_dead_east_n - template_dead_east_n) <= DEAD_ALERT_YEL) && ((new_dead_west_n - template_dead_west_n) <= DEAD_ALERT_YEL))
    {
      paddeadhit->SetFillColor(3);
    }
  else
    {
      if (((new_dead_east_n - template_dead_east_n) <= DEAD_ALERT_RED) && ((new_dead_west_n - template_dead_west_n) <= DEAD_ALERT_RED))
        {
          paddeadhit->SetFillColor(5);
        }
      else
        {
          paddeadhit->SetFillColor(2);
        }
    }

  static int minevtforpad = 5000;

  // load
  if (nevt < minevtforpad || (allhigherthanlow1 && alllowerthanhigh1))
    {
      padloadhit->SetFillColor(3);
    }
  else
    {
      //as a temporary fix, just set window to green no matter what 03/25/09  //commented out fix 04/21/09
      padloadhit->SetFillColor(5);
      //padloadhit->SetFillColor(3);
      if (!(allhigherthanlow))
        {
          padloadhit->SetFillColor(2);
          //padloadhit->SetFillColor(3);
        }
      if (!(alllowerthanhigh))
        {
          padloadhit->SetFillColor(2);
          //padloadhit->SetFillColor(3);
        }
    }


  // T zero
  if ((abs(tzero_east - template_tzero_east) < T0_ALERT_YEL) && ((abs(tzero_west - template_tzero_west) < T0_ALERT_YEL)))
    {
      padt0hit->SetFillColor(3);
    }
  else
    {
      if ((abs(tzero_east - template_tzero_east) < T0_ALERT_RED) && ((abs(tzero_west - template_tzero_west) < T0_ALERT_RED)))
        {
          //as a temporary fix, just set window to green no matter what 03/25/09
          padt0hit->SetFillColor(5);
          //padt0hit->SetFillColor(3);
        }
      else
        {
          padt0hit->SetFillColor(2);
          //padt0hit->SetFillColor(3);
        }
    }
  // dv
  if (nevt < minevtforpad  || ((fabs(fracdveast) < DV_ALERT_YEL) && ((fabs(fracdvwest) < DV_ALERT_YEL))))
    {
      paddvhit->SetFillColor(3);
    }
  else
    {
      if ((fracdveast > DV_ALERT_RED) || ((fracdvwest > DV_ALERT_RED)))
        {
          //as a temporary fix, just set window to green no matter what 03/25/09
          paddvhit->SetFillColor(2);
          //paddvhit->SetFillColor(3);
        }
      else
        {
          paddvhit->SetFillColor(5);
          //paddvhit->SetFillColor(3);
        }
    }

  //SW Eff.
  if (nevt < minevtforpad  || (((100*eff_east) > SW_ALERT_YEL) && ((100*eff_west) > SW_ALERT_YEL)))
    {
      padswhit->SetFillColor(3); // Green
    }
  else
    {
      if (((100*eff_east) > SW_ALERT_RED) && ((100*eff_west) > SW_ALERT_RED))
        {
          padswhit->SetFillColor(5); //Yellow
        }
      else
        {
          padswhit->SetFillColor(2);//Red
        }
    }

  // Rejected events
  if (float(bad_events) / nevt*100 < 90)
    {
      padbadev->SetFillColor(3); //Green
    }
  else
    {
      if (float(bad_events) / nevt*100 < 95)
        {
          padbadev->SetFillColor(5); //Yellow
        }
      else
        {
          padbadev->SetFillColor(2); // RED
        }
    }




  //////////////////////////////////////////
  // End Status Bar on Right
  //////////////////////////////////////////

  DcHitRateCanvas->Modified();
  DcHitRateCanvas->Update();
  DcHitRateCanvas->SetEditable(0);
  return 0;
}





int DchMonDraw::decodeInfoFromHistogram()
{

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 * dc_info = cl->getHisto("dc_info");

  if (dc_info == NULL)
    {
      return 1;
    }

  nevt = (int) dc_info->GetBinContent( NEVT_BIN );

  old_noise_num_00 = (int) dc_info->GetBinContent( OLD_NOISE_NUM_00_BIN );
  old_noise_num_01 = (int) dc_info->GetBinContent( OLD_NOISE_NUM_01_BIN );
  old_noise_num_10 = (int) dc_info->GetBinContent( OLD_NOISE_NUM_10_BIN );
  old_noise_num_11 = (int) dc_info->GetBinContent( OLD_NOISE_NUM_11_BIN );

  new_noise_num_00 = (int) dc_info->GetBinContent( NEW_NOISE_NUM_00_BIN );
  new_noise_num_01 = (int) dc_info->GetBinContent( NEW_NOISE_NUM_01_BIN );
  new_noise_num_10 = (int) dc_info->GetBinContent( NEW_NOISE_NUM_10_BIN );
  new_noise_num_11 = (int) dc_info->GetBinContent( NEW_NOISE_NUM_11_BIN );

  missing_noise_num_00 = (int) dc_info->GetBinContent( MISSING_NOISE_NUM_00_BIN );
  missing_noise_num_01 = (int) dc_info->GetBinContent( MISSING_NOISE_NUM_01_BIN );
  missing_noise_num_10 = (int) dc_info->GetBinContent( MISSING_NOISE_NUM_10_BIN );
  missing_noise_num_11 = (int) dc_info->GetBinContent( MISSING_NOISE_NUM_11_BIN );

  avg_load_00 = (float) dc_info->GetBinContent( AVG_LOAD_00_BIN );
  avg_load_01 = (float) dc_info->GetBinContent( AVG_LOAD_01_BIN );
  avg_load_10 = (float) dc_info->GetBinContent( AVG_LOAD_10_BIN );
  avg_load_11 = (float) dc_info->GetBinContent( AVG_LOAD_11_BIN );

  dead_report = (int) dc_info->GetBinContent( DEAD_REPORT_BIN );

  template_noisy_east_n = (int) dc_info->GetBinContent( TEMPLATE_NOISY_EAST_BIN );
  template_noisy_west_n = (int) dc_info->GetBinContent( TEMPLATE_NOISY_WEST_BIN );
  template_dead_east_n = (int) dc_info->GetBinContent( TEMPLATE_DEAD_EAST_BIN );
  template_dead_west_n = (int) dc_info->GetBinContent( TEMPLATE_DEAD_WEST_BIN );
  template_tzero_east = (int) dc_info->GetBinContent( TEMPLATE_TZERO_EAST_BIN );
  template_tzero_west = (int) dc_info->GetBinContent( TEMPLATE_TZERO_WEST_BIN );
  template_vdrift_east = (float) dc_info->GetBinContent( TEMPLATE_VDRIFT_EAST_BIN );
  template_vdrift_west = (float) dc_info->GetBinContent( TEMPLATE_VDRIFT_WEST_BIN );
  template_eff_east = (float) dc_info->GetBinContent( TEMPLATE_EFF_EAST_BIN );
  template_eff_west = (float) dc_info->GetBinContent( TEMPLATE_EFF_WEST_BIN );

  new_noisy_east_n = (int) dc_info->GetBinContent( NOISY_EAST_BIN );
  new_noisy_west_n = (int) dc_info->GetBinContent( NOISY_WEST_BIN );
  new_dead_east_n = (int) dc_info->GetBinContent( DEAD_EAST_BIN );
  new_dead_west_n = (int) dc_info->GetBinContent( DEAD_WEST_BIN );
  tzero_east = (int) dc_info->GetBinContent( TZERO_EAST_BIN );
  tzero_west = (int) dc_info->GetBinContent( TZERO_WEST_BIN );
  vdrift_east = (float) dc_info->GetBinContent( VDRIFT_EAST_BIN );
  vdrift_west = (float) dc_info->GetBinContent( VDRIFT_WEST_BIN );
  eff_east = (float) dc_info->GetBinContent( EFF_EAST_BIN );
  eff_west = (float) dc_info->GetBinContent( EFF_WEST_BIN );

  LLOAD_ALERT_RED = (float) dc_info->GetBinContent( TEMPLATE_LLOAD_ALERT_RED_BIN ); // DLAN
  LLOAD_ALERT_YEL = (float) dc_info->GetBinContent( TEMPLATE_LLOAD_ALERT_YEL_BIN ); // Now we store the color alert values
  HLOAD_ALERT_RED = (float) dc_info->GetBinContent( TEMPLATE_HLOAD_ALERT_RED_BIN ); // in dc_info hist.  They are encode there
  HLOAD_ALERT_YEL = (float) dc_info->GetBinContent( TEMPLATE_HLOAD_ALERT_YEL_BIN ); // after being read from DchTemplates.txt

  bad_events = (int) dc_info->GetBinContent(BADEV_BIN);


  return 1;
}


double DchMonDraw::get_minimum( double a, double b, double c, double d )
{
  double array[4];
  array[0] = a;
  array[1] = b;
  array[2] = c;
  array[3] = d;

  double minimum = a;

  for (int i = 0; i < 4; i++)
    {
      if (array[i] < minimum)
        {
          minimum = array[i];
        }
    }
  return minimum;
}

double DchMonDraw::get_maximum( double a, double b, double c, double d )
{
  double array[4];
  array[0] = a;
  array[1] = b;
  array[2] = c;
  array[3] = d;

  double maximum = a;

  for (int i = 0; i < 4; i++)
    {
      if (array[i] > maximum)
        {
          maximum = array[i];
        }
    }
  return maximum;
}

int DchMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream savefile;

  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  savefile.str("");
  savefile << ThisName << "_3_" << cl->RunNumber() << ".ps";
  DcHitRateCanvas->Print(savefile.str().c_str());

  return 0;
}



int DchMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();

  string pngfile = cl->htmlRegisterPage(*this, "HitRate", "3", "png");
  cl->CanvasToPng(DcHitRateCanvas, pngfile);
  cl->SaveLogFile(*this);

  return 0;
}
