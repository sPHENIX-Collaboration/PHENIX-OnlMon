#include <muiMonDraw.h>
#include <OnlMonClient.h>

#include <mui_mon_const.h>
#include <muiMonGlobals.h>

#include <phool.h>

#include <TCanvas.h>
#include <TFile.h>
#include <TGFrame.h>
#include <TH1.h>
#include <TImage.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>


#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// help with histogram and string stuff
const char *ONL_MUID_ARMSTR[MAX_ARM] =
  {"South", "North"
  };
const char *ONL_MUID_ASTR[MAX_ARM] =
  {"S", "N"
  };
const char *ONL_MUID_ORIENTSTR[MAX_ORIENTATION] =
  {"Horizontal", "Vertical"
  };
const char *ONL_MUID_OSTR[MAX_ORIENTATION] =
  {"H", "V"
  };

using namespace std;

/////////////////////////////////////////////////////////////////
muiMonDraw::muiMonDraw(): OnlMonDraw("MUIDMONITOR")
{
  useReferences = true;

  muid_canvasS = 0L;
  muid_canvasN = 0L;
  emptyHChannels = 0L;
  emptyVChannels = 0L;
  ROCtotal = 0L;

#ifdef MUID_USE_BLT

  for (int iblt = 0; iblt < NBLT_CANVAS; iblt++)
    {
      muid_blt_canvas[iblt] = 0L; //vasily
    }
#endif
  for (int iori = 0; iori < MAX_ORIENTATION; iori++)
    {
      HVHits[iori] = 0L;
      CableHits[iori] = 0L;
      for (int iarm = 0; iarm < MAX_ARM; iarm++)
        {
          twopackCanvas[iarm][iori] = 0L;
        }
    }

  muiStyle = 0L;
  for (int li = 0; li < NPLOT_PANELS; li++)
    {
      lmin[li] = 0L;
      lmax[li] = 0L;
    }

  return ;
}

/////////////////////////////////////////////////////////////////
muiMonDraw::~muiMonDraw()
{
  delete emptyHChannels;
  delete emptyVChannels;
  delete ROCtotal;
  for (int iori = 0; iori < MAX_ORIENTATION; iori++)
    {
      delete HVHits[iori];
      delete CableHits[iori];
    }

  delete muiStyle;
  for (int li = 0; li < NPLOT_PANELS; li++)
    {
      if (lmin[li])
        delete lmin[li];
      if (lmax[li])
        delete lmax[li];
    }
}

/////////////////////////////////////////////////////////////////
int muiMonDraw::Init()
{
  TStyle* oldStyle = gStyle;
  muiStyle = new TStyle("muiStyle", "muid style");
  muiStyle->SetTitleH(0.08);
  muiStyle->SetTitleW(0.70);
  muiStyle->SetFrameBorderMode(0);
  muiStyle->SetPadBorderMode(0);
  muiStyle->SetCanvasBorderMode(0);
  muiStyle->cd();

  int nROC = MAX_ROC * MAX_ORIENTATION; // 40
  int nCable = 120;
  int nHoriz_HV = 80;
  int nVert_HV = 70;

  emptyHChannels = new TH1F("emptyHChannels", "", nCable, -0.5, nCable - 0.5);
  emptyVChannels = new TH1F("emptyVChannels", "", nCable, -0.5, nCable - 0.5);
  ROCtotal = new TH1F("ROCtotal", "Hits Per ROC", nROC, 0.5, nROC + 0.5);
  HVHits[MUIMONCOORD::HORIZ] = new TH1F("HorizHVHits", "Horiz HV Chain", nHoriz_HV, -0.5, nHoriz_HV - 0.5);
  HVHits[MUIMONCOORD::VERT] = new TH1F("VertHVHits", "Vert HV Chain", nVert_HV, -0.5, nVert_HV - 0.5);
  CableHits[MUIMONCOORD::HORIZ] = new TH1F("HorizCableHits", "Horiz Cable", nCable, -0.5, nCable - 0.5);
  CableHits[MUIMONCOORD::VERT] = new TH1F("VertCableHits", "Vert Cable", nCable, -0.5, nCable - 0.5);

  //Force Error Storage to make proper division.

  HVHits[MUIMONCOORD::HORIZ]->Sumw2();
  HVHits[MUIMONCOORD::VERT]->Sumw2();
  CableHits[MUIMONCOORD::HORIZ]->Sumw2();
  CableHits[MUIMONCOORD::VERT]->Sumw2();

  //Set up hitograms to block out unused cables
  emptyHChannels->Reset();
  emptyVChannels->Reset();
  emptyHChannels->SetFillColor(1);
  for (int i = 0; i < NUnUsedH; i++)
    {
      emptyHChannels->SetBinContent(unUsedHChannels[i], 100);
    }
  emptyVChannels->SetFillColor(1);
  for (int i = 0; i < NUnUsedV; i++)
    {
      emptyVChannels->SetBinContent(unUsedVChannels[i], 100);
    }

  // Initialize lines to draw min/max ranges for hit distributions wrt reference histograms.
  if (useReferences)
    {
      for (int li = 0; li < NPLOT_PANELS; li++)
        {
          lmin[li] = new TLine(0.0, effMin, 200.0, effMin);
          lmax[li] = new TLine(0.0, effMax, 200.0, effMax);
          lmin[li]->SetLineWidth(2);
          lmax[li]->SetLineWidth(2);
        }
    }


  oldStyle->cd();
  registerHists();
  return 0;

}

/////////////////////////////////////////////////////////////////
void muiMonDraw::setupHist(TH1* muiHist, const char* XTitle, const char* YTitle)
{
  muiHist->SetLabelSize(0.06, "X");
  muiHist->SetLabelSize(0.06, "Y");
  muiHist->SetXTitle(XTitle);
  muiHist->SetYTitle(YTitle);
  muiHist->SetTitleOffset(1.3, "X");
  muiHist->SetTitleOffset(0.8, "Y");
  muiHist->SetTitleSize(0.06, "X");
  muiHist->SetTitleSize(0.06, "Y");
  muiHist->SetFillColor(muidFillColor);
  muiHist->SetStats(false);
  return ;
}

/////////////////////////////////////////////////////////////////
void muiMonDraw::registerHists()
{
  static int registered = 0;
  if (registered)
    {
      return ;
    }
  registered = 1;
  OnlMonClient *cl = OnlMonClient::instance();

  cl->registerHisto("MUI_NumEvent", "MUIDMONITOR");
  cl->registerHisto("MUI_SHA", "MUIDMONITOR");
  cl->registerHisto("MUI_SVA", "MUIDMONITOR");
  cl->registerHisto("MUI_SH2D", "MUIDMONITOR");
  cl->registerHisto("MUI_SV2D", "MUIDMONITOR");
  cl->registerHisto("MUI_NHA", "MUIDMONITOR");
  cl->registerHisto("MUI_NVA", "MUIDMONITOR");
  cl->registerHisto("MUI_NH2D", "MUIDMONITOR");
  cl->registerHisto("MUI_NV2D", "MUIDMONITOR");
  cl->registerHisto("MUI_SrocOkErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NrocOkErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SbeamOkErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NbeamOkErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SnoPacketErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NnoPacketErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SmainFrameErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NmainFrameErrHist", "MUIDMONITOR");

  cl->registerHisto("MUI_S_H_CABLE_RATE", "MUIDMONITOR");
  cl->registerHisto("MUI_S_V_CABLE_RATE", "MUIDMONITOR");
  cl->registerHisto("MUI_N_H_CABLE_RATE", "MUIDMONITOR");
  cl->registerHisto("MUI_N_V_CABLE_RATE", "MUIDMONITOR");

  cl->registerHisto("MUI_S_H_ROC_TOTALS", "MUIDMONITOR");
  cl->registerHisto("MUI_S_V_ROC_TOTALS", "MUIDMONITOR");
  cl->registerHisto("MUI_N_H_ROC_TOTALS", "MUIDMONITOR");
  cl->registerHisto("MUI_N_V_ROC_TOTALS", "MUIDMONITOR");

  cl->registerHisto("MUI_SH_HVhits", "MUIDMONITOR");
  cl->registerHisto("MUI_SV_HVhits", "MUIDMONITOR");
  cl->registerHisto("MUI_NH_HVhits", "MUIDMONITOR");
  cl->registerHisto("MUI_NV_HVhits", "MUIDMONITOR");

  cl->registerHisto("MUI_TrigMask", "MUIDMONITOR");

#ifdef MUID_USE_BLT

  cl->registerHisto("MUI_bltTrigEff", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEfe", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEfn", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEfd", "MUIDMONITOR");
  cl->registerHisto("MUI_bltQuadEff", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff0", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff1", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff2", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff3", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff4", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff5", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff6", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEff7", "MUIDMONITOR");

  cl->registerHisto("MUI_bltTrigEffa0", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa1", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa2", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa3", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa4", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa5", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa6", "MUIDMONITOR");
  cl->registerHisto("MUI_bltTrigEffa7", "MUIDMONITOR");
#endif

  ostringstream name;
  for (int i = 0; i < MAX_ROC;i++)
    {
      int iplane = i / 4;
      int iroc = i % 4;
      for (int iarm = 0; iarm < MAX_ARM; iarm++)
        {
          for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
            {
              name.str("");
              name << "MUI_" << ONL_MUID_ASTR[iarm] << ONL_MUID_OSTR[iorient]
		   << iplane << iroc;
              cl->registerHisto((name.str()).c_str(), "MUIDMONITOR");
            }
        }
    }

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int igap = 0; igap < GAP_PER_ARM; igap++)
        {
          for (int ipanel = 0; ipanel < PANEL_PER_GAP; ipanel++)
            {
              for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
                {
                  name.str("");
                  name << "MUI_twopack" << iarm << igap << ipanel << iorient;
                  cl->registerHisto((name.str()).c_str(), "MUIDMONITOR");
                }
            }
        }
    }

  for (int iarm = 0; iarm < 2; iarm++)
    {
      // ROC references
      name.str("");
      name << "MUI_ROCtotalREF" << iarm;
      cl->registerHisto((name.str()).c_str(), "MUIDMONITOR");
      for (int iori = 0; iori < 2; iori++)
        {
          name.str("");
          name << "MUI_HVHitsREF" << iarm << iori;
          cl->registerHisto((name.str()).c_str(), "MUIDMONITOR");
          name.str("");
          name << "MUI_CableHitsREF" << iarm << iori;
          cl->registerHisto((name.str()).c_str(), "MUIDMONITOR");
        }
    }

  // also register some previously forgotten/unregistered histograms
  // MUI_COR plots
  for (int igap = 0; igap < GAP_PER_ARM - 1;igap++)
    {
      for (int ipanel = 0; ipanel < PANEL_PER_GAP;ipanel++)
        {
          for (int iarm = 0; iarm < MAX_ARM; iarm++)
            {
              for (int iorient = 0; iorient < MAX_ORIENTATION; iorient++)
                {
                  name.str("");
                  name << "MUI_COR_" << ONL_MUID_ASTR[iarm] << ONL_MUID_OSTR[iorient]
		       << igap << "_" << ipanel;
                  cl->registerHisto((name.str()).c_str(), "MUIDMONITOR");
                }
            }
        }
    }

  // some more CABLE plots
  cl->registerHisto("MUI_CABLE_H", "MUIDMONITOR");
  cl->registerHisto("MUI_CABLE_V", "MUIDMONITOR");
  cl->registerHisto("MUI_N_CABLE_H", "MUIDMONITOR");
  cl->registerHisto("MUI_N_CABLE_V", "MUIDMONITOR");
  cl->registerHisto("MUI_N_OLDCABLE_H", "MUIDMONITOR");
  cl->registerHisto("MUI_N_OLDCABLE_V", "MUIDMONITOR");
  cl->registerHisto("MUI_OLDCABLE_H", "MUIDMONITOR");
  cl->registerHisto("MUI_OLDCABLE_V", "MUIDMONITOR");

  // some other misc. bits and pieces
  cl->registerHisto("MUI_NHB", "MUIDMONITOR");
  cl->registerHisto("MUI_NVB", "MUIDMONITOR");
  cl->registerHisto("MUI_SHB", "MUIDMONITOR");
  cl->registerHisto("MUI_SVB", "MUIDMONITOR");

  cl->registerHisto("MUI_NPanel", "MUIDMONITOR");
  cl->registerHisto("MUI_NPlane", "MUIDMONITOR");
  cl->registerHisto("MUI_NdeadCableErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NdeadChipErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NdeadHvChainErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NdeadRocErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_NmainFrameHits", "MUIDMONITOR");
  cl->registerHisto("MUI_SPanel", "MUIDMONITOR");
  cl->registerHisto("MUI_SPlane", "MUIDMONITOR");
  cl->registerHisto("MUI_SdeadCableErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SdeadChipErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SdeadHvChainErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SdeadRocErrHist", "MUIDMONITOR");
  cl->registerHisto("MUI_SmainFrameHits", "MUIDMONITOR");
  cl->registerHisto("MUI_TotalHits", "MUIDMONITOR");
  cl->registerHisto("MUI_TotalHits_TimeDep", "MUIDMONITOR");

  cl->requestHistoBySubSystem("MUIDMONITOR", 1);
  return ;
}

/////////////////////////////////////////////////////////////////
int muiMonDraw::real_draw_muid(TCanvas* muid_canvas, const short arm)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream msg;
  if ((arm != MUIMONCOORD::NORTH) && (arm != MUIMONCOORD::SOUTH))
    {
      cout << PHWHERE << " real_draw_muid: Invalid arm" << endl;
      return -1;
    }

  registerHists();
  bool SetLog[NPADS] = {0, 1, 1, 1, 1, 1};
  TText muid_message;

  if (arm == MUIMONCOORD::NORTH)
    {
      muid_message.SetText(0.05, 0.85, "North MuID Summary Page");
    }
  else
    {
      muid_message.SetText(0.05, 0.85, "South MuID Summary Page");
    }

  ostringstream muidText;

  TH1* tempH1;
  TH1* tempH2;
  int runNum = cl->RunNumber();
  tempH1 = cl->getHisto("MUI_NumEvent");
  int eventNum = 0;
  if (tempH1)
    {
      eventNum = (int)tempH1->GetEntries();
      if ( !(eventNum>0) ) { // no events yet 
	cout << PHWHERE << " MUID Server : No events yet!"
	     << " no need to update histograms." << endl;
	muid_canvas->Clear();
	muid_message.DrawTextNDC(0.3, 0.5, "MUID Server : No events yet!");
	return -2;
      }
    }
  else
    {
      cout << PHWHERE << " MUID Server appears dead!"
	   << " Cannot update histograms." << endl;
      muid_canvas->Clear();
      muid_message.DrawTextNDC(0.3, 0.5, "MUID Server appears dead!");
      return -2;
    }

  int total_hits[2] = {0, 0}; //
  tempH1 = cl->getHisto("MUI_SHA");
  tempH2 = cl->getHisto("MUI_SVA");
  if (tempH1 && tempH2)
    {
      total_hits[0] = (int)(tempH1->GetEntries() + tempH2->GetEntries());
    }
  tempH1 = cl->getHisto("MUI_NHA");
  tempH2 = cl->getHisto("MUI_NVA");
  if (tempH1 && tempH2)
    {
      total_hits[1] = (int)(tempH1->GetEntries() + tempH2->GetEntries());
    }

  float hit_rate = (float)total_hits[arm] / (float)eventNum;

  int rocOkErrors[2] = {0, 0};
  tempH1 = cl->getHisto("MUI_SrocOkErrHist");
  tempH2 = cl->getHisto("MUI_NrocOkErrHist");
  if (tempH1 && tempH2)
    {
      rocOkErrors[MUIMONCOORD::SOUTH] = (int)tempH1 ->GetBinContent(1);
      rocOkErrors[MUIMONCOORD::NORTH] = (int)tempH2 ->GetBinContent(1);
    }
  int beamOkErrors[2] = {0, 0};
  tempH1 = cl->getHisto("MUI_SbeamOkErrHist");
  tempH2 = cl->getHisto("MUI_NbeamOkErrHist");
  if (tempH1 && tempH2)
    {
      beamOkErrors[MUIMONCOORD::SOUTH] = (int)tempH1 ->GetBinContent(1);
      beamOkErrors[MUIMONCOORD::NORTH] = (int)tempH2 ->GetBinContent(1);
    }
  int noPacketErrors[2] = {0, 0};
  tempH1 = cl->getHisto("MUI_SnoPacketErrHist");
  tempH2 = cl->getHisto("MUI_NnoPacketErrHist");
  if (tempH1 && tempH2)
    {
      noPacketErrors[MUIMONCOORD::SOUTH] = (int)tempH1 ->GetBinContent(1);
      noPacketErrors[MUIMONCOORD::NORTH] = (int)tempH2 ->GetBinContent(1);
    }
  int mainFrameErrors[2] = {0, 0};
  tempH1 = cl->getHisto("MUI_SmainFrameErrHist");
  tempH2 = cl->getHisto("MUI_NmainFrameErrHist");
  if (tempH1 && tempH2)
    {
      mainFrameErrors[MUIMONCOORD::SOUTH] = (int)tempH1 ->GetBinContent(1);
      mainFrameErrors[MUIMONCOORD::NORTH] = (int)tempH2 ->GetBinContent(1);
    }

  //FIXME Temporary. Need to get these!
  //   int coin_1d = 0;
  //   int soft_1d = 0;
  //   int hard_1d = 0;
  //   int events_mb = 0;

  TH1* FullHVHits[MAX_ARM][MAX_ORIENTATION];
  TH1* cable_rate[MAX_FEM];
  TH1* HitsPerROC[MAX_ARM][MAX_ORIENTATION];

  HitsPerROC[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ] = cl->getHisto("MUI_S_H_ROC_TOTALS");
  HitsPerROC[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT] = cl->getHisto("MUI_S_V_ROC_TOTALS");
  HitsPerROC[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ] = cl->getHisto("MUI_N_H_ROC_TOTALS");
  HitsPerROC[MUIMONCOORD::NORTH][MUIMONCOORD::VERT] = cl->getHisto("MUI_N_V_ROC_TOTALS");

  cable_rate[0] = cl->getHisto("MUI_S_H_CABLE_RATE");
  cable_rate[1] = cl->getHisto("MUI_S_V_CABLE_RATE");
  cable_rate[2] = cl->getHisto("MUI_N_H_CABLE_RATE");
  cable_rate[3] = cl->getHisto("MUI_N_V_CABLE_RATE");

  FullHVHits[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ] = cl->getHisto("MUI_SH_HVhits");
  FullHVHits[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT] = cl->getHisto("MUI_SV_HVhits");
  FullHVHits[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ] = cl->getHisto("MUI_NH_HVhits");
  FullHVHits[MUIMONCOORD::NORTH][MUIMONCOORD::VERT] = cl->getHisto("MUI_NV_HVhits");

  // also get the references; now registered, and not only in memory..
  ///Hits per ROC reference histogram.
  TH1* ROCtotalREF[MAX_ARM];
  ///Hits per HV chain reference histogram.
  TH1* HVHitsREF[MAX_ARM][MAX_ORIENTATION];
  ///Hits per signal cable reference histogram.
  TH1* CableHitsREF[MAX_ARM][MAX_ORIENTATION];

  ostringstream name;
  for (int iarm = 0; iarm < 2; iarm++)
    {
      // ROC references
      name.str("");
      name << "MUI_ROCtotalREF" << iarm;
      ROCtotalREF[iarm] = cl->getHisto((name.str()).c_str());
      for (int iori = 0; iori < 2; iori++)
        {
          name.str("");
          name << "MUI_HVHitsREF" << iarm << iori;
          HVHitsREF[iarm][iori] = cl->getHisto((name.str()).c_str());
          name.str("");
          name << "MUI_CableHitsREF" << iarm << iori;
          CableHitsREF[iarm][iori] = cl->getHisto((name.str()).c_str());
        }
    }

#ifdef MUID_USE_BLT
  float dataEff[6];
  float trigEff[6];
  int datanmdn[6];
  int trignmdn[6];

  TH1* trigEffHist = cl->getHisto("MUI_bltTrigEff");
  TH1* trigEfeHist = cl->getHisto("MUI_bltTrigEfe");
  TH1* trigEfnHist = cl->getHisto("MUI_bltTrigEfn");
  TH1* trigEfdHist = cl->getHisto("MUI_bltTrigEfd");
  TH1* quadEffHist = cl->getHisto("MUI_bltQuadEff");
  // TH1* quadEfeHist = cl->getHisto("MUI_bltQuadEfe");
  // TH1* quadEfnHist = cl->getHisto("MUI_bltQuadEfn");
  // TH1* quadEfdHist = cl->getHisto("MUI_bltQuadEfd");
  for (int i = 0; i < 6; i++)
    {
      dataEff[i] = -1;
      trigEff[i] = -1;
      datanmdn[i] = -1;
      trignmdn[i] = -1;
    }

  float quadEff[4];
  for (int i = 0; i < 4; i++)
    {
      quadEff[i] = -1;
    }

  if (trigEffHist)
    {
      dataEff[0] = trigEffHist->GetBinContent(arm * 6 + 2);
      trigEff[0] = trigEffHist->GetBinContent(arm * 6 + 1);
      dataEff[1] = trigEffHist->GetBinContent(arm * 6 + 6);
      trigEff[1] = trigEffHist->GetBinContent(arm * 6 + 5);
      dataEff[2] = trigEffHist->GetBinContent(arm * 6 + 4);
      trigEff[2] = trigEffHist->GetBinContent(arm * 6 + 3);
    }

  if (trigEfeHist)
    {
      dataEff[3] = trigEfeHist->GetBinContent(arm * 6 + 2);
      trigEff[3] = trigEfeHist->GetBinContent(arm * 6 + 1);
      dataEff[4] = trigEfeHist->GetBinContent(arm * 6 + 6);
      trigEff[4] = trigEfeHist->GetBinContent(arm * 6 + 5);
      dataEff[5] = trigEfeHist->GetBinContent(arm * 6 + 4);
      trigEff[5] = trigEfeHist->GetBinContent(arm * 6 + 3);
    }

  if (trigEfnHist)
    {
      datanmdn[0] = int(trigEfnHist->GetBinContent(arm * 6 + 2));
      trignmdn[0] = int(trigEfnHist->GetBinContent(arm * 6 + 1));
      datanmdn[1] = int(trigEfnHist->GetBinContent(arm * 6 + 6));
      trignmdn[1] = int(trigEfnHist->GetBinContent(arm * 6 + 5));
      datanmdn[2] = int(trigEfnHist->GetBinContent(arm * 6 + 4));
      trignmdn[2] = int(trigEfnHist->GetBinContent(arm * 6 + 3));
    }

  if (trigEfdHist)
    {
      datanmdn[3] = int(trigEfdHist->GetBinContent(arm * 6 + 2));
      trignmdn[3] = int(trigEfdHist->GetBinContent(arm * 6 + 1));
      datanmdn[4] = int(trigEfdHist->GetBinContent(arm * 6 + 6));
      trignmdn[4] = int(trigEfdHist->GetBinContent(arm * 6 + 5));
      datanmdn[5] = int(trigEfdHist->GetBinContent(arm * 6 + 4));
      trignmdn[5] = int(trigEfdHist->GetBinContent(arm * 6 + 3));
    }

  if (quadEffHist)
    {
      quadEff[0] = quadEffHist->GetBinContent(arm * 4 + 1);
      quadEff[1] = quadEffHist->GetBinContent(arm * 4 + 2);
      quadEff[2] = quadEffHist->GetBinContent(arm * 4 + 3);
      quadEff[3] = quadEffHist->GetBinContent(arm * 4 + 4);
    }
#endif
  muid_canvas->Clear();

  ROCtotal->Reset();
  HVHits[MUIMONCOORD::HORIZ]->Reset();
  HVHits[MUIMONCOORD::VERT]->Reset();
  HVHits[MUIMONCOORD::HORIZ]->Add(FullHVHits[arm][MUIMONCOORD::HORIZ]);
  HVHits[MUIMONCOORD::VERT]->Add(FullHVHits[arm][MUIMONCOORD::VERT]);

  CableHits[MUIMONCOORD::HORIZ]->Reset();
  CableHits[MUIMONCOORD::VERT]->Reset();
  CableHits[MUIMONCOORD::HORIZ]->Add(cable_rate[arm*2]);
  CableHits[MUIMONCOORD::VERT]->Add(cable_rate[arm*2 + 1]);

  //Make the hits per ROC histo.
  for (int j = 0; j < MAX_ROC; j++)
    {
      ROCtotal->SetBinContent(j + 1, HitsPerROC[arm][MUIMONCOORD::HORIZ]->GetBinContent(j + 1));
      ROCtotal->SetBinContent(j + 1 + MAX_ROC, HitsPerROC[arm][MUIMONCOORD::VERT]->GetBinContent(j + 1));
      ROCtotal->SetBinError(j + 1, sqrt(HitsPerROC[arm][MUIMONCOORD::HORIZ]->GetBinContent(j + 1)));
      ROCtotal->SetBinError(j + 1 + MAX_ROC, sqrt(HitsPerROC[arm][MUIMONCOORD::VERT]->GetBinContent(j + 1)));
    }

  //Set up the page
  //FIXME I LEAK TPADS? May not be true sice canvas reset.
  TPad* muid_pad[NPADS];
  muid_pad[0] = new TPad("muid_pad1", "Total Number of Hits", 0, 0.66, 0.5, 1);
  muid_pad[1] = new TPad("muid_pad2", "Total Number of Hits: Timedependence", 0.5, 0.66, 1, 1);
  muid_pad[2] = new TPad("muid_pad3", "Plane Hits", 0, 0.33, 0.5, 0.66);
  muid_pad[3] = new TPad("muid_pad4", "Panel Hits", 0.5, 0.33, 1, 0.66);
  muid_pad[4] = new TPad("muid_pad5", "Plane Hits", 0, 0, 0.5, 0.33);
  muid_pad[5] = new TPad("muid_pad6", "Panel Hits", 0.5, 0, 1, 0.33);
  muid_canvas->Clear();
  muid_canvas->cd();

  for (int i = 0; i < NPADS; i++)
    {
      muid_pad[i]->SetFillColor(muidPadColor);
      muid_pad[i]->SetFrameFillColor(10);
      muid_pad[i]->SetTopMargin(0.12);
      muid_pad[i]->SetBottomMargin(0.15);
      muid_pad[i]->SetLeftMargin(0.1);
      muid_pad[i]->SetRightMargin(0.05);
      if (!useReferences)
        {
          muid_pad[i]->SetLogy(SetLog[i]);
        }
      muid_pad[i]->Draw();
    }

  // keep tab on how well the results agree with reference
  int referror = 0;
  // revised code to handle reference histogram divisions TVC 05/19/2003
  ostringstream yTitle;
  float scaleFactor = 100. / eventNum;

  if (useReferences)
    {
      yTitle << "Measured Rate/Expected";
    }
  else
    {
      yTitle << "100*Hits/Event";
    }

  // Everybody scaled by 100/event number.
  ROCtotal->Scale(scaleFactor);
  HVHits[MUIMONCOORD::HORIZ]->Scale(scaleFactor);
  HVHits[MUIMONCOORD::VERT]->Scale(scaleFactor);
  CableHits[MUIMONCOORD::HORIZ]->Scale(scaleFactor);
  CableHits[MUIMONCOORD::VERT]->Scale(scaleFactor);

  // Divide by reference histograms if they are specified
  if (useReferences)
    {
      ROCtotal->Divide(ROCtotalREF[arm]);
      HVHits[MUIMONCOORD::HORIZ]->Divide(HVHitsREF[arm][MUIMONCOORD::HORIZ]);
      HVHits[MUIMONCOORD::VERT]->Divide(HVHitsREF[arm][MUIMONCOORD::VERT]);
      CableHits[MUIMONCOORD::HORIZ]->Divide(CableHitsREF[arm][MUIMONCOORD::HORIZ]);
      CableHits[MUIMONCOORD::VERT]->Divide(CableHitsREF[arm][MUIMONCOORD::VERT]);
    }

  setupHist(ROCtotal, "ROC#", (yTitle.str()).c_str());
  setupHist(HVHits[MUIMONCOORD::HORIZ], "HV Chain", (yTitle.str()).c_str());
  setupHist(HVHits[MUIMONCOORD::VERT], "HV Chain", (yTitle.str()).c_str());
  setupHist(CableHits[MUIMONCOORD::HORIZ], "Signal Cable", (yTitle.str()).c_str());
  setupHist(CableHits[MUIMONCOORD::VERT], "Signal Cable", (yTitle.str()).c_str());

  if (useReferences)
    {
      ROCtotal->SetMaximum(RmaxRef);
      ROCtotal->SetMinimum(RminRef);

      HVHits[MUIMONCOORD::HORIZ]->SetMaximum(RmaxRef);
      HVHits[MUIMONCOORD::HORIZ]->SetMinimum(RminRef);

      HVHits[MUIMONCOORD::VERT]->SetMaximum(RmaxRef);
      HVHits[MUIMONCOORD::VERT]->SetMinimum(RminRef);

      CableHits[MUIMONCOORD::HORIZ]->SetMaximum(RmaxRef);
      CableHits[MUIMONCOORD::HORIZ]->SetMinimum(RminRef);

      CableHits[MUIMONCOORD::VERT]->SetMaximum(RmaxRef);
      CableHits[MUIMONCOORD::VERT]->SetMinimum(RminRef);
    }
  else
    {
      ROCtotal->SetMaximum(RmaxROC);
      ROCtotal->SetMinimum(RminROC);

      HVHits[MUIMONCOORD::HORIZ]->SetMaximum(RmaxHits);
      HVHits[MUIMONCOORD::HORIZ]->SetMinimum(RminHits);

      HVHits[MUIMONCOORD::VERT]->SetMaximum(RmaxHits);
      HVHits[MUIMONCOORD::VERT]->SetMinimum(RminHits);

      CableHits[MUIMONCOORD::HORIZ]->SetMaximum(RmaxHits);
      CableHits[MUIMONCOORD::HORIZ]->SetMinimum(RminHits);

      CableHits[MUIMONCOORD::VERT]->SetMaximum(RmaxHits);
      CableHits[MUIMONCOORD::VERT]->SetMinimum(RminHits);
    }

  // Plot hit distributions or ratios wrt reference histograms.
  if (useReferences)
    {
      cout << PHWHERE
	   << " MUID - Comparing with ref; arm " << arm << endl;
      // For the ref agreement check, for now, we only worry about whether
      // too many are below the min value or not..
      int nlow[NPLOT_PANELS];
      int nhigh[NPLOT_PANELS];
      for (int ip = 0; ip < NPLOT_PANELS; ip++)
        {
          nlow[ip] = 0;
          nhigh[ip] = 0;
        }

      // First plot's actions commented, others similar
      muid_pad[1]->cd();                               // move to right pad
      muid_pad[1]->SetLeftMargin(0.16);                // make room for y-title
      ROCtotal->GetYaxis()->SetTitleOffset(1.2);       // Move title off labels
      ROCtotal->SetMarkerStyle(20);                    // Make points into red circles
      ROCtotal->SetMarkerSize(1);
      ROCtotal->SetMarkerColor(2);
      ROCtotal->DrawCopy("PE2");                       // Draw points plus boxes covering 1-sigma errors
      lmin[0]->SetX1(ROCtotal->GetXaxis()->GetXmin()); // Edit min/max line x-extents
      lmin[0]->SetX2(ROCtotal->GetXaxis()->GetXmax());
      lmax[0]->SetX1(ROCtotal->GetXaxis()->GetXmin());
      lmax[0]->SetX2(ROCtotal->GetXaxis()->GetXmax());
      lmin[0]->Draw();                                 // Draw min/max lines
      lmax[0]->Draw();
      for (int ibin = 0; ibin < ROCtotal->GetNbinsX(); ibin++)
        {
          Stat_t bincont = ROCtotal->GetBinContent(ibin + 1);
          Stat_t binerr = ROCtotal->GetBinError(ibin + 1);
          if ( (bincont + binerr*nSigma) < effMin )
            {
              cout << PHWHERE << " ROC low ibin " << ibin
		   << " bincont " << bincont
		   << " binerr " << binerr << endl;
              nlow[0]++;
            }
          if ( (bincont - binerr*nSigma) > effMax )
            {
              nhigh[0]++;
            }
        }

      muid_pad[2]->cd();
      muid_pad[2]->SetLeftMargin(0.16);
      HVHits[MUIMONCOORD::HORIZ]->GetYaxis()->SetTitleOffset(1.2);
      HVHits[MUIMONCOORD::HORIZ]->SetMarkerStyle(20);
      HVHits[MUIMONCOORD::HORIZ]->SetMarkerSize(0.7);
      HVHits[MUIMONCOORD::HORIZ]->SetMarkerColor(2);
      HVHits[MUIMONCOORD::HORIZ]->DrawCopy("PE2");
      lmin[1]->SetX1(HVHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmin());
      lmin[1]->SetX2(HVHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmax());
      lmax[1]->SetX1(HVHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmin());
      lmax[1]->SetX2(HVHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmax());
      lmin[1]->Draw();
      lmax[1]->Draw();
      for (int ibin = 0; ibin < HVHits[MUIMONCOORD::HORIZ]->GetNbinsX(); ibin++)
        {
          Stat_t bincont = HVHits[MUIMONCOORD::HORIZ]->GetBinContent(ibin + 1);
          Stat_t binerr = HVHits[MUIMONCOORD::HORIZ]->GetBinError(ibin + 1);
          if ( (bincont + binerr*nSigma) < effMin )
            {
              cout << PHWHERE << "HV HORIZ low ibin " << ibin
		   << " bincont " << bincont
		   << " binerr " << binerr << endl;
              nlow[1]++;
            }
          if ( (bincont - binerr*nSigma) > effMax )
            {
              nhigh[1]++;
            }
        }

      muid_pad[3]->cd();
      muid_pad[3]->SetLeftMargin(0.16);
      HVHits[MUIMONCOORD::VERT]->GetYaxis()->SetTitleOffset(1.2);
      HVHits[MUIMONCOORD::VERT]->SetMarkerStyle(20);
      HVHits[MUIMONCOORD::VERT]->SetMarkerSize(0.7);
      HVHits[MUIMONCOORD::VERT]->SetMarkerColor(2);
      HVHits[MUIMONCOORD::VERT]->DrawCopy("PE2");
      lmin[2]->SetX1(HVHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmin());
      lmin[2]->SetX2(HVHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmax());
      lmax[2]->SetX1(HVHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmin());
      lmax[2]->SetX2(HVHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmax());
      lmin[2]->Draw();
      lmax[2]->Draw();
      for (int ibin = 0; ibin < HVHits[MUIMONCOORD::VERT]->GetNbinsX(); ibin++)
        {
          Stat_t bincont = HVHits[MUIMONCOORD::VERT]->GetBinContent(ibin + 1);
          Stat_t binerr = HVHits[MUIMONCOORD::VERT]->GetBinError(ibin + 1);
          if ( (bincont + binerr*nSigma) < effMin )
            {
              cout << PHWHERE << "HV VERT low ibin " << ibin
		   << " bincont " << bincont
		   << " binerr " << binerr << endl;
              nlow[2]++;
            }
          if ( (bincont - binerr*nSigma) > effMax )
            {
              nhigh[2]++;
            }
        }

      int iempty = 0; // for the unused cables
      muid_pad[4]->cd();
      muid_pad[4]->SetLeftMargin(0.16);
      CableHits[MUIMONCOORD::HORIZ]->GetYaxis()->SetTitleOffset(1.2);
      CableHits[MUIMONCOORD::HORIZ]->SetMarkerStyle(20);
      CableHits[MUIMONCOORD::HORIZ]->SetMarkerSize(0.5);
      CableHits[MUIMONCOORD::HORIZ]->SetMarkerColor(2);
      CableHits[MUIMONCOORD::HORIZ]->DrawCopy("PE2");
      emptyHChannels->Draw("Same");
      lmin[3]->SetX1(CableHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmin());
      lmin[3]->SetX2(CableHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmax());
      lmax[3]->SetX1(CableHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmin());
      lmax[3]->SetX2(CableHits[MUIMONCOORD::HORIZ]->GetXaxis()->GetXmax());
      lmin[3]->Draw();
      lmax[3]->Draw();
      for (int ibin = 0; ibin < CableHits[MUIMONCOORD::HORIZ]->GetNbinsX(); ibin++)
        {
          if (iempty < NUnUsedH)
            {
              if ( (ibin + 1) == unUsedHChannels[iempty] )
                {
                  // this is an unused cable; let's just move on
                  iempty++;
                  continue;
                }
            }

          Stat_t bincont = CableHits[MUIMONCOORD::HORIZ]->GetBinContent(ibin + 1);
          Stat_t binerr = CableHits[MUIMONCOORD::HORIZ]->GetBinError(ibin + 1);
          if ( (bincont + binerr*nSigma) < effMin )
            {
              cout << PHWHERE << "Cable HORIZ low ibin " << ibin
		   << " bincont " << bincont
		   << " binerr " << binerr << endl;
              nlow[3]++;
            }
          if ( (bincont - binerr*nSigma) > effMax )
            {
              nhigh[3]++;
            }
        }

      iempty = 0;
      muid_pad[5]->cd();
      muid_pad[5]->SetLeftMargin(0.16);
      CableHits[MUIMONCOORD::VERT]->GetYaxis()->SetTitleOffset(1.2);
      CableHits[MUIMONCOORD::VERT]->SetMarkerStyle(20);
      CableHits[MUIMONCOORD::VERT]->SetMarkerSize(0.5);
      CableHits[MUIMONCOORD::VERT]->SetMarkerColor(2);
      CableHits[MUIMONCOORD::VERT]->DrawCopy("PE2");
      emptyVChannels->Draw("Same");
      lmin[4]->SetX1(CableHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmin());
      lmin[4]->SetX2(CableHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmax());
      lmax[4]->SetX1(CableHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmin());
      lmax[4]->SetX2(CableHits[MUIMONCOORD::VERT]->GetXaxis()->GetXmax());
      lmin[4]->Draw();
      lmax[4]->Draw();
      for (int ibin = 0; ibin < CableHits[MUIMONCOORD::VERT]->GetNbinsX(); ibin++)
        {
          if (iempty < NUnUsedV)
            {
              if ( (ibin + 1) == unUsedVChannels[iempty] )
                {
                  // this is an unused cable; let's just move on
                  iempty++;
                  continue;
                }
            }

          Stat_t bincont = CableHits[MUIMONCOORD::VERT]->GetBinContent(ibin + 1);
          Stat_t binerr = CableHits[MUIMONCOORD::VERT]->GetBinError(ibin + 1);
          if ( (bincont + binerr*nSigma) < effMin )
            {
              cout << PHWHERE << "Cable VERT low ibin " << ibin
		   << " bincont " << bincont
		   << " binerr " << binerr << endl;
              nlow[4]++;
            }
          if ( (bincont - binerr*nSigma) > effMax )
            {
              nhigh[4]++;
            }
        }

      // make a loop over nlow/nhigh; presently only nlow is checked,
      // and bitcode into referror
      for (int ip = 0; ip < NPLOT_PANELS; ip++)
        {
          if ( nlow[ip] >= (nMinNeededBad + nKnownBad[arm][ip]) )
            {
              referror |= (1 << ip) ;
              cout << PHWHERE << " arm " << arm
		   << " plot " << ip << " nlow " << nlow[ip] << endl;
            }
        }
    }
  else
    {
      muid_pad[1]->cd();
      ROCtotal->DrawCopy("HIST");
      muid_pad[2]->cd();
      HVHits[MUIMONCOORD::HORIZ]->DrawCopy("HIST");
      muid_pad[3]->cd();
      HVHits[MUIMONCOORD::VERT]->DrawCopy("HIST");
      muid_pad[4]->cd();
      CableHits[MUIMONCOORD::HORIZ]->DrawCopy("HIST");
      emptyHChannels->Draw("Same");
      muid_pad[5]->cd();
      CableHits[MUIMONCOORD::VERT]->DrawCopy("HIST");
      emptyVChannels->Draw("Same");
    }

  //Output summary text. Should be made more elegant and robust
  float num_lines = 10.;
#ifdef MUID_USE_BLT

  num_lines = 14.;
#endif

  int line = 0;
  muid_pad[0]->cd();
  muid_message.SetTextColor(1);
  muid_message.SetTextSize(0.5 / num_lines);
  muidText << ONL_MUID_ARMSTR[arm] << " MuID: Run " << runNum
	   << " NEvt " << eventNum;
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());

  // FIXME - need real trigger info here
  TH1* TrigMask = cl->getHisto("MUI_TrigMask");

  int trigmask = 0;
  if (TrigMask)
    {
      for ( int itrig = 0; itrig < MAX_SCALEDTRIG_BIT; itrig++ )
        {
          if (TrigMask->GetBinContent(itrig + 1) == 1)
            {
              trigmask |= ( 1 << itrig );
            }
        }
    cout << PHWHERE << " Trigger Mask histogram found : trigmask = 0x" 
	 << hex << trigmask << dec << endl; 
    }
  else {
    cout << PHWHERE << " Trigger Mask histogram not found " << endl; 
  }

  muidText.str("");
  muidText << "Trigger Selection: 0x" << hex << trigmask << dec;
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());

  int okcolor = 1;
  int warncolor = 2;
  int lightwarncolor = 5;
  int goodcolor = 8;

  ostringstream runnostream;
  string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "Time: " << ctime(&evttime);
  runstring = runnostream.str();
  // transparent[0]->cd();
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), runstring.c_str());

  if ( (hit_rate > MAX_HIT_RATE[arm]) || (hit_rate < MIN_HIT_RATE[arm]) )
    {
      muid_message.SetTextColor(warncolor);
    }
  else
    {
      muid_message.SetTextColor(okcolor);
    }
  muid_message.SetTextSize(0.6 / num_lines); // increase font size somewhat
  muidText.str("");
  muidText << "Hit Rate: " << setprecision(2) << hit_rate;
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
  muidText.str("");
  muidText << "ROC Readout Errors: " << rocOkErrors[arm];
  if (rocOkErrors[arm] > 0)
    {
      muid_message.SetTextColor(warncolor);
    }
  else
    {
      muid_message.SetTextColor(okcolor);
    }
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
  muidText.str("");
  muidText << "ROC BCLK Errors: " << beamOkErrors[arm];
  if (beamOkErrors[arm] > 0)
    {
      muid_message.SetTextColor(warncolor);
    }
  else
    {
      muid_message.SetTextColor(okcolor);
    }
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
  muidText.str("");
  muidText << "# Packet Errors: " << noPacketErrors[arm];
  if (noPacketErrors[arm] > 0)
    {
      muid_message.SetTextColor(lightwarncolor);
    }
  else
    {
      muid_message.SetTextColor(okcolor);
    }
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());

  //Print warning instructions
  muid_message.SetTextSize(0.7 / num_lines); // increase font size somewhat
  if ( (hit_rate > MAX_HIT_RATE[arm]) || (hit_rate < MIN_HIT_RATE[arm]) )
    {
      muid_message.SetTextColor(warncolor);
      muidText.str("");
      if ( hit_rate < MIN_HIT_RATE[arm] ) { // too low
	muidText << "Check that GAS is OK.";
      }
      else { // too high
	muidText << "Check HV Curr. & Bkgds.";
      }

      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
      muidText.str("");
      muidText << "Hit Rate out of bounds!";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
    }
  else if (mainFrameErrors[arm] > MAX_NMAINFRAME_ERRORS)
    {
      muid_message.SetTextColor(warncolor);
      muidText.str("");
      muidText << "Check that HV is OK.";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
      muidText.str("");
      muidText << "Ask SA1 if HV GUI is alive";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
    }
  else if ((float)rocOkErrors[arm] / (float)eventNum > MAX_ROCOK_ERRORFREQ)
    {
      muid_message.SetTextColor(warncolor);
      muidText.str("");
      muidText << "Instructions: Reset Low Voltage.";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
      muidText.str("");
      muidText << "FEED MuID. Page MuID if continues.";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
    }
  else if (beamOkErrors[arm] > MAX_NBEAM_OKERRORS)
    {
      muid_message.SetTextColor(warncolor);
      muidText.str("");
      muidText << "Instructions: Restart Run.";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
      muidText.str("");
      muidText << "Page MuID if continues.";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
    }
  else if ((eventNum) < MIN_NEVENTS)
    {
      muid_message.SetTextColor(warncolor);
      muidText.str("");
      muidText << "Instructions: Too few events for";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
      muidText.str("");
      muidText << "good statistics.";
      line++;
      muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
    }
  else
    {
      if (!useReferences)
        {
          muid_message.SetTextColor(warncolor);
          muidText.str("");
          muidText << "Info: Reference check is not done";
          line++;
          muid_message.SetTextColor(okcolor);
          muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
          muidText.str("");
          muidText << "SEEMS OK/GOOD!";
          line++;
          muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
        }
      else
        {
          // look into how the results agree with reference
          if (referror > MAX_REF_ERROR)
            {
              muid_message.SetTextColor(warncolor);
              muidText.str("");
              muidText << "Results not in agreement with ref.: " << hex << referror << dec;
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
              muidText.str("");
              muidText << "Check HV for trips. Page MUID if continues";
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
            }
          else
            {
              muid_message.SetTextColor(goodcolor);
              muidText.str("");
              muidText << "RESULTS AGREE WITH REFERENCE!";
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
              muidText.str("");
              muidText << " SEEMS OK/GOOD!";
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (muidText.str()).c_str());
            }
        }
    }
#ifdef MUID_USE_BLT
  char muidBLTText[128];
  if (trigEff[0] < 0.9 && trigEff[3] < 0.2 || dataEff[0] < 0.9 && dataEff[3] < 0.2 )
    {
      muid_message.SetTextColor(2);
    }
  else
    {
      muid_message.SetTextColor(1);
    }

  if (trignmdn[3] > 1 && datanmdn[3] > 1)
    {
      sprintf(muidBLTText, "1D      T/D  eff: %.2f +- %.2f    %.2f +- %.2f", trigEff[0], trigEff[3], dataEff[0], dataEff[3]);
    }
  else
    {
      muid_message.SetTextColor(2);
      sprintf(muidBLTText, "1D      T/D  eff: N/A +/- N/A      N/A +/- N/A ");
    }

  if (trignmdn[3] == 1 && datanmdn[3] == 1)
    {
      muid_message.SetTextColor(2);
      sprintf(muidBLTText, "1D     T/D  eff: 1.00(1.00 +- N/A)  1.00(1.00 +- N/A)");
    }

  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);

  if (trigEff[1] < 0.9 && trigEff[4] < 0.2 || dataEff[1] < 0.9 && dataEff[4] < 0.2 )
    {
      muid_message.SetTextColor(2);
    }
  else
    {
      muid_message.SetTextColor(1);
    }

  if (trignmdn[4] > 0 && datanmdn[4] > 0)
    {
      sprintf(muidBLTText, "1D1S T/D  eff: %.2f +- %.2f    %.2f +- %.2f", trigEff[1], trigEff[4], dataEff[1], dataEff[4]);
    }
  else
    {
      muid_message.SetTextColor(2);
      sprintf(muidBLTText, "1D1S TCE/DCE  eff: N/A +/- N/A      N/A +/- N/A ");
    }

  if (trignmdn[4] == 1 && datanmdn[4] == 1)
    {
      muid_message.SetTextColor(2);
      sprintf(muidBLTText, "1D1S  T/D  eff: 1.00(1.00 +- N/A)  1.00(1.00 +- N/A) ");
    }

  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);

  if (trigEff[2] < 0.9 && trigEff[5] < 0.2 || dataEff[2] < 0.9 && dataEff[5] < 0.2 )
    {
      muid_message.SetTextColor(2);
    }
  else
    {
      muid_message.SetTextColor(1);
    }

  if (trignmdn[5] > 0 && datanmdn[5] > 0)
    {
      sprintf(muidBLTText, "2D      T/D  eff: %.2f +- %.2f    %.2f +- %.2f", trigEff[2], trigEff[5], dataEff[2], dataEff[5]);
    }
  else
    {
      muid_message.SetTextColor(2);
      sprintf(muidBLTText, "2D      T/D  eff: N/A +/- N/A      N/A +/- N/A ");
    }

  if (trignmdn[5] == 1 && datanmdn[5] == 1)
    {
      muid_message.SetTextColor(2);
      sprintf(muidBLTText, "2D     T/D  eff: 1.00(1.00 +- N/A)  1.00(1.00 +- N/A) ");
    }

  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);

  sprintf(muidBLTText, "(1D) Quad eff:     %.2f  %.2f               %.2f  %.2f ", quadEff[0], quadEff[1], quadEff[2], quadEff[3]);
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
#endif

  muid_canvas->cd();
  muid_canvas->Update();

  return 0;
}

int muiMonDraw::DrawTwopacks()
{
  registerHists();
  OnlMonClient *cl = OnlMonClient::instance();
  int colors[5] = {1, 2, 8, 4, 28};

  if (!gROOT->FindObject("MUIC_twopack00"))
    {
      twopackCanvas[MUIMONCOORD::SOUTH][MUIMONCOORD::HORIZ] = new TCanvas("MUIC_twopack00", "South Horizontal", -1, 0, 900, 660);
      gSystem->ProcessEvents();
    }
  if (!gROOT->FindObject("MUIC_twopack01"))
    {
      twopackCanvas[MUIMONCOORD::SOUTH][MUIMONCOORD::VERT] = new TCanvas("MUIC_twopack01", "South Vertical", -1, 0, 900, 660);
      gSystem->ProcessEvents();
    }
  if (!gROOT->FindObject("MUIC_twopack10"))
    {
      twopackCanvas[MUIMONCOORD::NORTH][MUIMONCOORD::HORIZ] = new TCanvas("MUIC_twopack10", "North Horizontal", -1, 0, 900, 660);
      gSystem->ProcessEvents();
    }
  if (!gROOT->FindObject("MUIC_twopack11"))
    {
      twopackCanvas[MUIMONCOORD::NORTH][MUIMONCOORD::VERT] = new TCanvas("MUIC_twopack11", "North Vertical", -1, 0, 900, 660);
      gSystem->ProcessEvents();
    }

  for (int i = 0;i < 2;i++)
    {
      for (int j = 0;j < 2;j++)
        {
          twopackCanvas[i][j]->Clear();
          twopackCanvas[i][j]->SetFillColor(kWhite);
          twopackCanvas[i][j]->Divide(3, 2);
        }
    }

  TH1* h1[5];

  ostringstream name;
  for (int iarm = 0;iarm < 2;iarm++)
    {
      for (int iorient = 0;iorient < 2;iorient++)
        {
          for (int ipanel = 0;ipanel < 6;ipanel++)
            {
              for (int igap = 0;igap < 5;igap++)
                {
                  name.str("");
                  name << "MUI_twopack" << iarm << igap << ipanel << iorient;
                  h1[igap] = (TH1*)(cl->getHisto((name.str()).c_str()))->Clone();
                  if (!h1[igap])
                    return 1;
                }
              double maxhits = 0;
              int maxindex = 0;
              for (int igap = 0;igap < 5;igap++)
                {
                  if (h1[igap]->GetMaximum() > maxhits)
                    {
                      maxhits = h1[igap]->GetMaximum();
                      maxindex = igap;
                    }
                }
              twopackCanvas[iarm][iorient]->cd(ipanel + 1);
              h1[maxindex]->SetStats(false);
              ostringstream Namestream;
              string HistName;
              Namestream << "Twopack Hits: " << ONL_MUID_ARMSTR[iarm]
			 << ONL_MUID_OSTR[iorient];
              Namestream << "Panel " << ipanel;
              HistName = Namestream.str();
              h1[maxindex]->SetTitle(HistName.c_str());
              h1[maxindex]->Draw();
              for (int igap = 0;igap < 5;igap++)
                {
                  h1[igap]->SetLineColor(colors[igap]);
                  h1[igap]->SetStats(false);
                  //h1[igap]->Scale(1./(float)NumEvent->GetEntries());
                  h1[igap]->Draw("same");
                }
              if (ipanel == 1)
                {
                  TLegend* muidlegend = new TLegend(0.65, 0.65, 0.88, 0.88, "Hits");
                  muidlegend->AddEntry(h1[0], "Gap 0");
                  muidlegend->AddEntry(h1[1], "Gap 1");
                  muidlegend->AddEntry(h1[2], "Gap 2");
                  muidlegend->AddEntry(h1[3], "Gap 3");
                  muidlegend->AddEntry(h1[4], "Gap 4");
                  muidlegend->SetBit(1); //Make deleteable
                  muidlegend->AppendPad();
                }
            }
          twopackCanvas[iarm][iorient]->cd();
          twopackCanvas[iarm][iorient]->Update();
        }
    }

  return 0;
}

#ifdef MUID_USE_BLT
int muiMonDraw::DrawBlt(const int blt_canvas, const float th, const float tw,
                        const int sx, const int sy)
{
  muiStyle->SetTitleH(th);
  muiStyle->SetTitleW(tw);
  muiStyle->cd();

  registerHists();
  OnlMonClient *cl = OnlMonClient::instance();

  char blt_canvas_name[64];
  sprintf(blt_canvas_name, "muid_blt_canvas%d", blt_canvas);
  if (!gROOT->FindObject(blt_canvas_name))
    {
      char blt_canvas_title[64];
      sprintf(blt_canvas_title, "MUID BLT%d", blt_canvas);
      muid_blt_canvas[blt_canvas] = new TCanvas(blt_canvas_name, blt_canvas_title, -1, 0, sx, sy);
      gSystem->ProcessEvents();
    }
  muid_blt_canvas[blt_canvas]->Clear();
  muid_blt_canvas[blt_canvas]->cd();

  TPad* blt_pad[8]; //vasily
  bool draw_mode = false;
  if ( (blt_canvas % 3) == 0 )
    { // canvas 0 and 3 have associated pads
      draw_mode = true;
      blt_pad[0] = new TPad("blt_pad0", "Total Number of Hits S0", 0.0, 0.60, 0.5, 0.8);
      blt_pad[1] = new TPad("blt_pad1", "Total Number of Hits S1", 0.5, 0.60, 1.0, 0.8);
      blt_pad[2] = new TPad("blt_pad2", "Total Number of Hits S2", 0.0, 0.40, 0.5, 0.6);
      blt_pad[3] = new TPad("blt_pad3", "Total Number of Hits S3", 0.5, 0.40, 1.0, 0.6);
      blt_pad[4] = new TPad("blt_pad4", "Total Number of Hits N0", 0.0, 0.20, 0.5, 0.4);
      blt_pad[5] = new TPad("blt_pad5", "Total Number of Hits N1", 0.5, 0.20, 1.0, 0.4);
      blt_pad[6] = new TPad("blt_pad6", "Total Number of Hits N2", 0.0, 0.00, 0.5, 0.2);
      blt_pad[7] = new TPad("blt_pad7", "Total Number of Hits N3", 0.5, 0.00, 1.0, 0.2);

      for (int i = 0; i < 8; i++)
        {
          blt_pad[i]->SetFillColor(muidPadColor);
          blt_pad[i]->SetFrameFillColor(10);
          blt_pad[i]->SetTopMargin(0.12);
          blt_pad[i]->SetBottomMargin(0.15);
          blt_pad[i]->SetLeftMargin(0.1);
          blt_pad[i]->SetRightMargin(0.05);
          blt_pad[i]->Draw();
        }
    }
  int line = 0;
  float num_lines = 24.;
  char muidBLTText[128];
  ostringstream msg;
  TText muid_message;
  muid_message.SetTextSize(0.5 / num_lines);

  TH1* tempH1;
  int runNum = cl->RunNumber();
  tempH1 = cl->getHisto("MUI_NumEvent");

  int eventNum = 0;
  if (tempH1)
    {
      eventNum = (int)tempH1->GetEntries();
    }
  else
    {
      muid_message.DrawTextNDC(0.3, 0.5, "MUID Server appears dead!");
      return -1;
    }

  muid_message.SetTextColor(1);
  msg.str("");
  msg << "Expert Page: MuID BLT Details: Masked ";
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (msg.str()).c_str());
  msg.str("");
  msg << "MuID Summary: Run " << runNum;
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (msg.str()).c_str());
  msg.str("");
  msg << "Total Events Examined: " << eventNum;
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), (msg.str()).c_str());

  ostringstream runnostream;
  string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "Time: " << ctime(&evttime);
  runstring = runnostream.str();
  // transparent[0]->cd();
  line++;
  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), runstring.c_str());

  // Here starts the unique parts for the various canvases..
  // except for the pad part above
  if (blt_canvas != 2)
    { // case 2 is rather different from the others
      TH1* trigEffHist[8];
      char blt_histid[32];
      char blt_histbase[20];
      if (blt_canvas < 2)
        {
          sprintf(blt_histbase, "MUI_bltTrigEff");
        }
      else
        {
          sprintf(blt_histbase, "MUI_bltTrigEffa");
        }

      // this
      char xBLTTitle[128];
      char yBLTTitle[128];
      sprintf(yBLTTitle, "Hits");
      for (int ihist = 0; ihist < 8; ihist++)
        {
          sprintf(blt_histid, "%s%d", blt_histbase, ihist);
          trigEffHist[ihist] = cl->getHisto(blt_histid);
          int iarm = ihist / 4;
          if (draw_mode)
            {
              sprintf(xBLTTitle, "%s Bits%d", ONL_MUID_ARMSTR[iarm], ihist % 4);
              setupHist(trigEffHist[ihist], xBLTTitle, yBLTTitle);
              blt_pad[ihist]->cd();
              trigEffHist[ihist]->SetMaximum(1.2);
              trigEffHist[ihist]->DrawCopy();
            }
          else
            { // text_mode..
              int irest = ihist % 4;
              if (irest == 0)
                {
                  muid_message.SetTextColor(1);
                  sprintf(muidBLTText, " %s Arm ", ONL_MUID_ARMSTR[iarm]);
                  line++;
                  muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
                }
              float blt[16];
              for (int i = 0; i < 16; i++)
                {
                  blt[i] = -1;
                }

              if (trigEffHist[ihist])
                {
                  for (int i = 0; i < 16; i++)
                    {
                      blt[i] = trigEffHist[ihist]->GetBinContent(i + 1);
                    }
                }

              sprintf(muidBLTText, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f ",
                      blt[0], blt[1], blt[2], blt[3], blt[4], blt[5], blt[6], blt[7] );
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
              sprintf(muidBLTText, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f ",
                      blt[8], blt[9], blt[10], blt[11], blt[12], blt[13], blt[14], blt[15] );
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
            }
        } // ihist loop
    }
  else
    { // blt_canvas == 2
      const int NTYPES = 3;
      const char *TYPESTR[NTYPES] =
        { "1D", "1D1S", "2D"
        };
      const int NVAL = NTYPES * 2; // 6
      const int NQUAD = 4;

      TH1* trigEffHist = cl->getHisto("MUI_bltTrigEff");
      TH1* trigEfeHist = cl->getHisto("MUI_bltTrigEfe");
      TH1* trigEfnHist = cl->getHisto("MUI_bltTrigEfn");
      TH1* trigEfdHist = cl->getHisto("MUI_bltTrigEfd");
      TH1* quadEffHist = cl->getHisto("MUI_bltQuadEff");

      for (int arm = 0; arm < 2; arm++)
        {
          muid_message.SetTextColor(1);
          sprintf(muidBLTText, " %s Arm ", ONL_MUID_ARMSTR[arm]);
          line++;
          muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
          sprintf(muidBLTText, "                              TCE                          DCE");
          line++;
          muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);

          for (int itype = 0; itype < NTYPES; itype++)
            {
              int localtype = itype;
              if (itype > 0)
                {
                  localtype = NTYPES - itype; // switch the last two
                }
              int data_eff_index = arm * NVAL + localtype * 2 + 2;
              int trig_eff_index = arm * NVAL + localtype * 2 + 1;

              float dataEff = -1, trigEff = -1;
              if (trigEffHist)
                {
                  dataEff = (float) trigEffHist->GetBinContent(data_eff_index);
                  trigEff = (float) trigEffHist->GetBinContent(trig_eff_index);
                }

              float dataEfe = -1, trigEfe = -1;
              if (trigEfeHist)
                {
                  dataEfe = (float) trigEfeHist->GetBinContent(data_eff_index);
                  trigEfe = (float) trigEfeHist->GetBinContent(trig_eff_index);
                }

              int dataEfn = -1, trigEfn = -1;
              if (trigEfnHist)
                {
                  dataEfn = (int) trigEfnHist->GetBinContent(data_eff_index);
                  trigEfn = (int) trigEfnHist->GetBinContent(trig_eff_index);
                }

              int dataEfd = -1, trigEfd = -1;
              if (trigEfdHist)
                {
                  dataEfd = (int) trigEfdHist->GetBinContent(data_eff_index);
                  trigEfd = (int) trigEfdHist->GetBinContent(trig_eff_index);
                }

              if (trigEff < 0.9 && trigEfe < 0.2 || dataEff < 0.9 && dataEfe < 0.2 )
                {
                  muid_message.SetTextColor(2);
                }
              else
                {
                  muid_message.SetTextColor(1);
                }

              if (trigEfd > 0 && dataEfd > 0)
                {
                  sprintf(muidBLTText, "%s    T/D  eff: %.2f +- %.2f    %.2f +- %.2f",
                          TYPESTR[itype], trigEff, trigEfe, dataEff, dataEfe);
                }
              else
                {
                  muid_message.SetTextColor(2);
                  sprintf(muidBLTText, "%s    T/D  eff: N/A +/- N/A      N/A +/- N/A ",
                          TYPESTR[itype]);
                }

              if (trigEfd == 1 && dataEfd == 1)
                {
                  muid_message.SetTextColor(2);
                  sprintf(muidBLTText, "%s   T/D  eff: 1.00(1.00 +- N/A)  1.00(1.00 +- N/A)",
                          TYPESTR[itype]);
                }
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
              sprintf(muidBLTText, "%s  N D:           %d      %d                %d      %d ",
                      TYPESTR[itype], trigEfn, trigEfd, dataEfn, dataEfd);
              line++;
              muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
            } // type

          float quadEff[NQUAD];
          for (int i = 0; i < NQUAD; i++)
            {
              quadEff[i] = -1;
            }
          if (quadEffHist)
            {
              quadEff[0] = quadEffHist->GetBinContent(arm * 4 + 1);
              quadEff[1] = quadEffHist->GetBinContent(arm * 4 + 2);
              quadEff[2] = quadEffHist->GetBinContent(arm * 4 + 3);
              quadEff[3] = quadEffHist->GetBinContent(arm * 4 + 4);
            }

          sprintf(muidBLTText, "(1D) Quad eff:     %.2f      %.2f              %.2f      %.2f ",
                  quadEff[0], quadEff[1], quadEff[2], quadEff[3]);
          line++;
          muid_message.DrawTextNDC(0.05, (1. - line / num_lines), muidBLTText);
        } // arm
    }
  if (draw_mode)
    {
      muid_blt_canvas[blt_canvas]->cd();
      muid_blt_canvas[blt_canvas]->Update();
    }
  return 0;
}

#endif 
/////////////////////////////////////////////////////////////////

int muiMonDraw::Draw(const char *what)
{
  int iret = 0;
  TStyle* oldStyle = gStyle;
  muiStyle->cd();
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  if (!strcmp(what, "ALL") || !strcmp(what, "MUID_ALL") || !strcmp(what, "MUID_BASIC"))
    {
      if (! gROOT->FindObject("muid_canvasS"))
        {
          muid_canvasS = new TCanvas("muid_canvasS", "South MuID Monitor", -1, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
        }
      iret = real_draw_muid(muid_canvasS, MUIMONCOORD::SOUTH);
      if (iret && iret != -2)
        {
          oldStyle->cd();
          return iret;
        }
      if (! gROOT->FindObject("muid_canvasN") )
        {
          muid_canvasN = new TCanvas("muid_canvasN", "North MuID Monitor", -xsize / 2, 0, xsize / 2, ysize);
          gSystem->ProcessEvents();
        }
      iret = real_draw_muid(muid_canvasN, MUIMONCOORD::NORTH);
      if (iret)
        {
          oldStyle->cd();
          return iret;
        }

    }
  if (!strcmp(what, "MUID_ALL") || !strcmp(what, "MUID_TWOPACKS"))
    {
      DrawTwopacks();
    }

#ifdef MUID_USE_BLT
  if (!strcmp(what, "MUID_ALL") || !strcmp(what, "MUID_BLT"))
    {
      DrawBlt(0, 0.30, 0.06, 900, 660);
      DrawBlt(1, 0.24, 0.35, 900, 940);
      DrawBlt(3, 0.30, 0.06, 900, 660);
      DrawBlt(4, 0.07, 0.70, 900, 940);
      DrawBlt(2, 0.08, 0.70, 900, 660);
    }
#endif


  oldStyle->cd();
  return iret;
}

/////////////////////////////////////////////////////////////////
int muiMonDraw::MakePS(const char *what)
{
  TStyle* oldStyle = gStyle;
  muiStyle->cd();

  ostringstream psstream;
  string psfile;
  OnlMonClient *cl = OnlMonClient::instance();

  Draw("ALL");

  psstream << ThisName << "_" << cl->RunNumber() << "_1.ps";
  psfile = psstream.str();
  muid_canvasS->Print(psfile.c_str());
  psstream.str("");

  psstream << ThisName << "_" << cl->RunNumber() << "_2.ps";
  psfile = psstream.str();
  muid_canvasN->Print(psfile.c_str());
  oldStyle->cd();
  return 0;
}

/////////////////////////////////////////////////////////////////
int muiMonDraw::MakeHtml(const char *what)
{
  TStyle* oldStyle = gStyle;
  muiStyle->cd();

  int iret = Draw("MUID_ALL");
  if (iret)
    {
      oldStyle->cd();
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();
  string ofile = cl->htmlRegisterPage(*this, "South Monitor", "1", "png");
  cl->CanvasToPng(muid_canvasS, ofile);

  ofile = cl->htmlRegisterPage(*this, "North Monitor", "2", "png");
  cl->CanvasToPng(muid_canvasN, ofile);

  ofile = cl->htmlRegisterPage(*this, "South H Twopacks", "3", "png");
  cl->CanvasToPng(twopackCanvas[0][0], ofile);

  ofile = cl->htmlRegisterPage(*this, "South V Twopacks", "4", "png");
  cl->CanvasToPng(twopackCanvas[0][1], ofile);

  ofile = cl->htmlRegisterPage(*this, "North H Twopacks", "5", "png");
  cl->CanvasToPng(twopackCanvas[1][0], ofile);

  ofile = cl->htmlRegisterPage(*this, "North V Twopacks", "6", "png");
  cl->CanvasToPng(twopackCanvas[1][1], ofile);


#ifdef MUID_USE_BLT

  string pngfile[5];
  pngfile[0] = cl->htmlRegisterPage(*this, "BLT Mask Histograms", "7", "png");
  pngfile[1] = cl->htmlRegisterPage(*this, "BLT Mask Details", "8", "png");
  pngfile[2] = cl->htmlRegisterPage(*this, "BLT General Details", "11", "png");
  pngfile[3] = cl->htmlRegisterPage(*this, "BLT Activated Histograms", "9", "png");
  pngfile[4] = cl->htmlRegisterPage(*this, "BLT Activated Details", "10", "png");
  for (int i = 0; i < 5;i++)
    {
      cl->CanvasToPng(muid_blt_canvas[i], pngfile[i]);
    }

#endif
  cl->SaveLogFile(*this);
  oldStyle->cd();
  return 0;

}

