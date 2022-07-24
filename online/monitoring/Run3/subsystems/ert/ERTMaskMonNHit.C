#include "ERTMaskMonCommon.h"
#include "ERTMaskMonNHit.h"

#include "TH1.h"
#include "TPad.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TText.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace ERTMaskMonCommon;

ERTMaskMonNHit::ERTMaskMonNHit(const Detector det, const char* name)
{
   m_detector = det;
   m_name = name;
   m_nhot = 0;

   int n_sm = m_detector == DET_EMC  ?  N_SM_EMC_TOTAL  :  N_SM_RICH_TOTAL;

   char hname[128];
   sprintf(hname, "ert_hsm_for_draw_%s", m_name.c_str());
   m_hsm_for_draw = new TH1F(hname, "", n_sm, 0, n_sm);
   sprintf(hname, "ert_hsm_masked_%s", m_name.c_str());
   m_hsm_masked = new TH1F(hname, "", n_sm, 0, n_sm);
   sprintf(hname, "ert_hsm_hot_%s", m_name.c_str());
   m_hsm_hot = new TH1F(hname, "", n_sm, 0, n_sm);

   string htitle = m_name + " hits per trigger tile";
   m_pave_title = new TPaveText(LEFT_MARGIN, 0.9, 0.3, 1.0, "blNDC");
   m_pave_title->AddText(htitle.c_str());

   for (int ias = 0; ias < N_ARMSECT; ias++) {
      m_line_sector[ias] = new TLine(SERIAL_SM_BORDER[m_detector][ias], 0, 
                                     SERIAL_SM_BORDER[m_detector][ias], 1);
   }
   m_label_sector = new TText();
   m_label_hot    = new TText();
}

ERTMaskMonNHit::~ERTMaskMonNHit()
{
   delete m_hsm_for_draw;
   delete m_hsm_masked;
   delete m_hsm_hot;

   for (int ias = 0; ias < N_ARMSECT; ias++) delete m_line_sector[ias];
   delete m_pave_title;
   delete m_label_sector;
   delete m_label_hot;
}

int ERTMaskMonNHit::IsMasked(const int ssm)
{
   int bin = m_hsm_masked->FindBin(ssm);
   if (m_hsm_masked->GetBinContent(bin) > 0) return true;
   else                                      return false;
}

int ERTMaskMonNHit::IsHot(const int ssm)
{
   int bin = m_hsm_hot->FindBin(ssm);
   if (m_hsm_hot->GetBinContent(bin) > 0) return true;
   else                                   return false;
}

int ERTMaskMonNHit::IsColdHalfSect(const int hs)
{
   return m_bl_sect_cold[hs];
}

int ERTMaskMonNHit::HasColdHalfSect()
{
   for (int i = 0; i < N_HALF_SECTOR; i++) {
      if (IsColdHalfSect(i)) return true;
   }
   return false;
}

void ERTMaskMonNHit::SetMask(const int ssm, const float val)
{
   int bin = m_hsm_masked->FindBin(ssm);
   m_hsm_masked->SetBinContent(bin, val);
}

void ERTMaskMonNHit::SetHot(const int ssm, const float val)
{
   int bin = m_hsm_hot->FindBin(ssm);
   m_hsm_hot->SetBinContent(bin, val);
}

void ERTMaskMonNHit::ResetMask()
{
   m_hsm_masked->Reset();
}

void ERTMaskMonNHit::ResetHot()
{
   m_hsm_hot->Reset();
   m_nhot = 0;
}

void ERTMaskMonNHit::ResetColdSector()
{
   memset(m_bl_sect_cold, 0 , sizeof(m_bl_sect_cold));
}

void ERTMaskMonNHit::FindHotWithNHitSigma(const float sigma_cut)
{
   ResetHot();

   float threshold;
   if (m_detector == DET_EMC) {
      // PbGl
      threshold = sigma_cut * GetAverage(PBGL_START, PBGL_END);
      m_nhot += FindHotInRange(threshold, PBGL_START, PBGL_END);
      // PbSc
      threshold = sigma_cut * GetAverage(0, N_SM_EMC_TOTAL, PBGL_START, PBGL_END);
      m_nhot += FindHotInRange(threshold, 0, N_SM_EMC_TOTAL, PBGL_START, PBGL_END);
   } else { // RICH
      threshold = sigma_cut * GetAverage(0, N_SM_RICH_TOTAL);
      m_nhot = FindHotInRange(threshold, 0, N_SM_RICH_TOTAL);
   }
}

void ERTMaskMonNHit::FindHotWithNHitThreshold(const float threshold)
{
   ResetHot();

   if (m_detector == DET_EMC) {
      // PbGl
      m_nhot += FindHotInRange(threshold, PBGL_START, PBGL_END);
      // PbSc
      m_nhot += FindHotInRange(threshold, 0, N_SM_EMC_TOTAL, PBGL_START, PBGL_END);
   } else { // RICH
      m_nhot = FindHotInRange(threshold, 0, N_SM_RICH_TOTAL);
   }
}

void ERTMaskMonNHit::FindHotWithRFactor(const float rfactor_current, const float rfactor_dlimit)
{
   ResetHot();

   float nhit_total = m_hsm->Integral();
   if (nhit_total <= 0) return;
   TH1* h1_copy = (TH1*)m_hsm->Clone("h1_copy");
   float rfactor_wo_hot = rfactor_current;

   while (rfactor_wo_hot <= rfactor_dlimit) {
      int   ibin = h1_copy->GetMaximumBin();
      float nhit = h1_copy->GetMaximum();

      h1_copy  ->SetBinContent(ibin, 0.0);
      m_hsm_hot->SetBinContent(ibin, nhit);
      m_nhot++;

      float nhit_wo_hot = h1_copy->Integral();
      if (nhit_wo_hot <= 0) break;
      rfactor_wo_hot = rfactor_current * nhit_total / nhit_wo_hot;
   }

   delete h1_copy;
}

float ERTMaskMonNHit::GetAverage(
   const int nsm_l, const int nsm_h, 
   const int nsm_l_reject, const int nsm_h_reject)
{
   int nn = 0;
   float sum = 0;
   for (int ism = nsm_l ;ism < nsm_h ;ism++) {
      int ibin = m_hsm->FindBin(ism);
      if (m_hsm_masked->GetBinContent(ibin) == 0 &&
          ! (nsm_l_reject <= ism && ism < nsm_h_reject) ) {
         sum += m_hsm->GetBinContent(ibin);
         nn++;
      }
   }
   return sum / nn;
}

int ERTMaskMonNHit::FindHotInRange(
   const float threshold, const int nsm_l, const int nsm_h, 
   const int nsm_skip_l, const int nsm_skip_h)
{
   int nhot = 0;
   for (int ism = nsm_l; ism < nsm_h; ism++) {
      int ibin = m_hsm->FindBin(ism);
      float nhit = m_hsm->GetBinContent(ibin);
      if (! (nsm_skip_l <= ism && ism < nsm_skip_h) && nhit >= threshold ) {
         SetHot(ism, nhit);
         nhot++;
      }
   }
   return nhot;
}

void ERTMaskMonNHit::FindColdSector()
{
   ResetColdSector();

   int n_sm_used[N_HALF_SECTOR];
   int n_sm_few_hit[N_HALF_SECTOR];
   float hit_min[N_HALF_SECTOR];
   memset(n_sm_used, 0, sizeof(n_sm_used));
   memset(n_sm_few_hit, 0, sizeof(n_sm_few_hit));
   for (int i = 0; i < N_HALF_SECTOR; i++) hit_min[i] = -1;

   for (int ias = 0; ias < N_ARMSECT; ias++) {
      int sm_start = SERIAL_SM_BORDER[m_detector][ias];
      int n_sm     = SERIAL_SM_BORDER[m_detector][ias+1] - sm_start;
      int n_row    = (n_sm == 32)  ?  8  :  6;

      for (int ism = 0; ism < n_sm; ism++) {
         if (IsMasked(sm_start + ism)) continue;

         float cont = m_hsm->GetBinContent(sm_start + ism + 1);
         int ihs = 2 * ias + (ism % n_row < n_row / 2  ?  0  :  1);
         n_sm_used[ihs]++;
         if (cont < 3) n_sm_few_hit[ihs]++;
         if (hit_min[ihs] < 0 || hit_min[ihs] > cont) hit_min[ihs] = cont;
      }
   }

   int bl_enough_stat = false;//true if at least one half sect has enough stat.
   for (int i_hs = 0; i_hs < N_HALF_SECTOR; i_hs++) {
      // We cannot directly use the mean values of hits in each half sector
      // because of noisy tiles.  Instead, use "hit_min" to assure enough stat.
      if (hit_min[i_hs] > 5) bl_enough_stat = true;
      if (n_sm_few_hit[i_hs] > n_sm_used[i_hs] / 2 ) m_bl_sect_cold[i_hs] = true;
   }
   if (! bl_enough_stat) ResetColdSector();
}

void ERTMaskMonNHit::Draw(TPad* pad, int logflag)
{
   float hist_max = m_hsm->GetMaximum();
   if (hist_max <= 0) logflag = false;
   if (logflag) pad->SetLogy(kTRUE);
   else         pad->SetLogy(kFALSE);

   float h_ratio  = logflag  ?  0.40  :  0.85;

   pad->cd();
   pad->SetGridy(kTRUE);

   pad->SetLeftMargin (LEFT_MARGIN);
   pad->SetRightMargin(RIGHT_MARGIN);

   m_hsm->SetStats(kFALSE);

   m_hsm         ->SetFillColor(16);
   m_hsm_for_draw->SetFillColor(16);

   m_hsm_masked->SetLineColor(kGreen);
   m_hsm_masked->SetFillColor(kGreen);
   m_hsm_masked->SetFillStyle(1001);

   m_hsm_hot->SetLineColor(kRed);
   m_hsm_hot->SetFillColor(kRed);
   m_hsm_hot->SetFillStyle(1001);

   m_hsm->GetYaxis()->SetTickLength(0.01);
   m_hsm->GetYaxis()->SetNdivisions(3);

   // set contents of hsm_for_draw
   for (int ibin = 1; ibin <= m_hsm->GetNbinsX(); ibin++) {
      float cont = (ibin % 2 == 0)  ?  m_hsm->GetBinContent(ibin)  :  0;
      m_hsm_for_draw->SetBinContent(ibin, cont);
   }

   // adjust height of masked histo
   if (m_hsm_masked->GetMaximum() != 0) {
      float height = (hist_max == 0)  ?  h_ratio  :  h_ratio * hist_max;
      m_hsm_masked->Scale(height / m_hsm_masked->GetMaximum());
   }

   m_hsm         ->SetLabelSize(FONT_SIZE, "Y");
   m_hsm         ->Draw();
   m_hsm_for_draw->Draw("same");
   m_hsm_hot     ->Draw("same");
   m_hsm_masked  ->Draw("same");
   pad->RedrawAxis();
   pad->RedrawAxis("g");

   // draw title
   m_pave_title->SetBorderSize(1);
   m_pave_title->Draw();

   // draw sector names
   const int color_sector_item = 38;
   for (int ias = 0; ias < N_ARMSECT; ias++) {
      float ssm_center = 
         (SERIAL_SM_BORDER[m_detector][ias] + 
          SERIAL_SM_BORDER[m_detector][ias + 1]) / 2.0;

      m_label_sector->SetTextColor(color_sector_item);
      m_label_sector->SetTextSize(FONT_SIZE);
      m_label_sector->SetTextAlign(21); // center/bottom
      m_label_sector->DrawText(ssm_center, h_ratio * hist_max, SECTOR_NAME[ias]);
   }
   // draw separator of sectors
   for (int ias = 1; ias < N_ARMSECT; ias++) {
      m_line_sector[ias]->SetLineWidth(2);
      m_line_sector[ias]->SetLineStyle(1);
      m_line_sector[ias]->SetLineColor(color_sector_item);
      m_line_sector[ias]->SetY2(hist_max);
      m_line_sector[ias]->Draw();
   }

   // draw hot SM numbers
//   m_label_hot->SetTextColor(kRed);
//   m_label_hot->SetTextSize(FONT_SIZE);
//   m_label_hot->SetTextAlign(11); // left/bottom
//   for (int issm = 0; issm < SERIAL_SM_BORDER[m_detector][N_ARMSECT]; issm++) {
//      double cont = m_hsm_hot->GetBinContent(issm + 1);
//      if (cont > 0) {
//         int arm, sector, sm;
//         SerialSmToArmSectSm(m_detector, issm, arm, sector, sm);
//         char sm_name[8];
//         sprintf(sm_name, "SM%i", sm);
//         m_label_hot->DrawText(issm, cont, sm_name);
//      }
//   }
}
