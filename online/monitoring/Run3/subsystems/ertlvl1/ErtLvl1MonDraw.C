#include <ErtLvl1MonDraw.h>
#include <ErtLvl1MonDefs.h>

#include <OnlMon.h>
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

using namespace std;

enum {index_4x4a, index_4x4b, index_4x4c, index_2x2, index_Electron, index_TwoElectron};

ErtLvl1MonDraw::ErtLvl1MonDraw(): OnlMonDraw("ERTLVL1MON")
{
  TStyle *oldstyle = gStyle;
  localStyle = new TStyle("ertlvl1style", "ERT LL1 Online Monitor style");

  localStyle -> SetOptStat(0);
  localStyle -> SetOptFit(0);
  localStyle -> SetCanvasColor(10);
  localStyle -> SetCanvasBorderMode(0);
  localStyle -> SetFrameFillColor(0);
  localStyle -> SetFrameBorderMode(0);
  localStyle -> SetFrameBorderSize(1);
  localStyle -> SetFrameLineColor(1);
  localStyle -> SetPadColor(0);
  localStyle -> SetPadBorderMode(0);
  localStyle -> SetPadBorderSize(0);
  localStyle -> SetPadColor(0);
  localStyle -> SetTitleX(0.28);    // x of top left corner of title box
  localStyle -> SetTitleY(0.999);   // y of top left corner of title box
  localStyle -> SetTitleH(0.10);
  localStyle -> SetTitleW(0.71);
  localStyle -> SetLabelSize(0.5, "xyz");
  localStyle -> SetTitleSize(0.5, "xyz");
  oldstyle->cd();
  for (int i = 0; i < NUM_CANVASES; i++)
    {
      ErtLvl1Canv[i] = 0;
      transparent[i] = 0;
      for (int j = 0; j < MAX_PADS_PER_CANVAS; j++)
	{
	  ErtLvl1Pad[i][j] = 0;
	}
    }

  for (int i = 0; i < 5; i++)
    {
      labeltext[i] = new TText();
    }

}

ErtLvl1MonDraw::~ErtLvl1MonDraw()
{
  for (int i = 0; i < 5; i++)
    {
      delete labeltext[i];
    }
  delete localStyle;
  return;
}


int ErtLvl1MonDraw::MakeCanvases(const char *name)
{
  if (!strcmp(name, "ErtLvl1Mon_E"))
    {
      DrawCommonCanvas(0);
    }
  else if (!strcmp(name, "ErtLvl1Mon_W"))
    {
      DrawCommonCanvas(1);
    }
  else if (!strcmp(name, "ROCeff_E"))
    {
      DrawCommonCanvas(2);
    }
  else if (!strcmp(name, "ROCeff_W"))
    {
      DrawCommonCanvas(3);
    }
  else
    {
      return 1;
    }

  return 0;
}

int
ErtLvl1MonDraw::DrawCommonCanvas(const int index)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  xsize -= 40;
  ysize -= 30;
  double xlow = 0., ylow = 0., xhigh = 0., yhigh = 0.;
  ostringstream PadName, Title;
  const string ArmEW[2] = {"E", "W"};
  const string ArmEWFull[2] = {"East", "West"};
  if (index < 2)
    {
      Title << "ErtLvl1Mon "
	    << ArmEWFull[index]
	    << " System Overview";
      PadName << "ErtLvl1Mon_" << ArmEW[index] ;
      ErtLvl1Canv[index] = new TCanvas(PadName.str().c_str(), Title.str().c_str(), -1, 0, xsize, ysize);
      gSystem->ProcessEvents();
      ErtLvl1Canv[index]->cd();
      for (int ipad = 0; ipad < MAX_PADS_PER_CANVAS; ipad++)
	{
	  PadName.str("");
	  PadName << "ErtLvl1Pad" << ipad ;
	  if (ipad < 4)
	    {
	      xlow = (ipad * 0.25) + 0.005;
	      ylow = 0.49;
	      xhigh = ((ipad + 1) * 0.25) - 0.005;
	      yhigh = 0.94;
	    }
	  else if ( (ipad >= 4) && (ipad <= 7) )
	    {
	      xlow = ((ipad - 4) * 0.25) + 0.005;
	      ylow = 0.0;
	      xhigh = ((ipad - 3) * 0.25) - 0.005;
	      yhigh = 0.47;
	    }
	  else if (ipad == 8)
	    {
	      xlow = (3 * 0.25) + 0.005;
	      xhigh = (4 * 0.25) - 0.005;
	      ylow = 0.0;
	      yhigh = 0.23;
	    }

	  ErtLvl1Pad[index][ipad] = new TPad(PadName.str().c_str(), "", xlow, ylow, xhigh, yhigh, 0, 0);
	  ErtLvl1Pad[index][ipad]->SetRightMargin(0.04);
	  ErtLvl1Pad[index][ipad]->SetLeftMargin(0.16);
	  ErtLvl1Pad[index][ipad]->Draw();
	}
      PadName.str("");
      PadName << "transparent" << ArmEW[index] ;
      transparent[index] = new TPad(PadName.str().c_str(), "this does not show", 0, 0, 1, 1);
      transparent[index]->SetFillStyle(4000);
      transparent[index]->Draw();
    }
  else
    {
      Title << "ErtLvl1Mon "
	    << ArmEWFull[index - 2]
	    << " ROC Efficiency";
      PadName << "ROCeff_" << ArmEW[index - 2] ;
      ErtLvl1Canv[index] = new TCanvas(PadName.str().c_str(), Title.str().c_str(), -1, 0, xsize, ysize);
      gSystem->ProcessEvents();
      ErtLvl1Canv[index]->cd();
      Title.str("");
      Title << "ROC Efficiencies "
	    << ArmEWFull[index - 2]
	    << " Run: " << cl->RunNumber();
      labeltext[4]->SetText(0.05, 0.93, Title.str().c_str());
      labeltext[4]->SetTextSize(0.08);
      labeltext[4]->Draw();

      for (int ipad = 0; ipad < 2; ipad++)
	{
	  PadName.str("");
	  PadName << "ROCeff_" << ArmEW[index - 2] << "_" << ipad;

	  xlow = 0.0;
	  xhigh = 0.99;

	  if (ipad == 0)
	    {
	      ylow = 0.51;
	      yhigh = 0.9;
	    }
	  else
	    {
	      ylow = 0;
	      yhigh = 0.49;
	    }

	  ErtLvl1Pad[index][ipad] = new TPad(PadName.str().c_str(), "", xlow, ylow, xhigh, yhigh, 0, 0);
	  ErtLvl1Pad[index][ipad]->SetRightMargin(0.005);
	  ErtLvl1Pad[index][ipad]->SetLeftMargin(0.1);
	  if (ipad == 0)
	    ErtLvl1Pad[index][ipad]->SetBottomMargin(0.0045);
	  else
	    ErtLvl1Pad[index][ipad]->SetBottomMargin(0.1);
	  ErtLvl1Pad[index][ipad]->SetTopMargin(0);
	  ErtLvl1Pad[index][ipad]->SetGridx();
	  ErtLvl1Pad[index][ipad]->Draw();
	}
    }
  return 0;
}

int
ErtLvl1MonDraw::DrawRocEff(const int iarm)
{
  OnlMonClient *cl = OnlMonClient::instance();

  const string ArmEW[2] = {"E", "W"};
  string EW = "ROCeff_" + ArmEW[iarm];
  if (!gROOT->FindObject(EW.c_str()))
    {
      if (MakeCanvases(EW.c_str()))
	{
	  cout << PHWHERE << " Unknown Canvas: " << EW.c_str() << endl;
	  return -1;
	}
    }

  ErtLvl1Canv[iarm + 2]->SetEditable(kTRUE);

  TStyle* oldstyle = gStyle;
  localStyle->cd();
  localStyle->SetOptTitle(0);
  string htit = "ErtLvl1_hRocEntries" + ArmEW[iarm];

  ErtLvl1Pad[iarm + 2][0]->cd();
  TH1* hRocEntries = cl->getHisto(htit.c_str());
  if (!hRocEntries)
    {
      cout << PHWHERE << " Unknown Histogram " << htit.c_str() << endl;
      return -1;
    }
  for (int ib = 1; ib <= hRocEntries->GetNbinsX(); ib++)
    hRocEntries->GetXaxis()->SetBinLabel(ib, Form("%d", (ib - 1) % 6));

  hRocEntries->GetYaxis()->SetTitle("Entries");
  hRocEntries->SetLineWidth(2);
  hRocEntries->Draw("");

  ErtLvl1Pad[iarm + 2][1]->cd();
  htit = "ErtLvl1_hRocEff" + ArmEW[iarm];
  TH1* hRocEff = cl->getHisto(htit.c_str());
  if (!hRocEff)
    {
      cout << PHWHERE << " Unknown Histogram " << htit.c_str() << endl;
      return -1;
    }

  int indxroc[8] = {0, 1, 2, 3, 16, 17, 18, 19};
  for (int ib = 1; ib <= hRocEff->GetNbinsX(); ib++)
    {
      int fiber = indxroc[(ib - 1) / 6];
      int bit = ((ib - 1) % 6);
      if (!((fiber == 2 && (bit == 0 || bit == 1)) || (fiber == 17 && (bit == 0 || bit == 1)) ||
	    (fiber == 18 && (bit == 2 || bit == 5)) || (fiber == 19 && (bit == 2 || bit == 5))))
	{

	  hRocEff->GetXaxis()->SetBinLabel(ib, Form("%d-%d", indxroc[(ib - 1) / 6], (ib - 1) % 6));
	}
    }
  hRocEff->SetMinimum(0);
  hRocEff->SetMaximum(1.5);
  hRocEff->SetMarkerStyle(20);
  hRocEff->SetMarkerColor(2);
  hRocEff->GetXaxis()->SetTitle("Fiber - Bit word");
  hRocEff->GetYaxis()->SetTitle("Efficiency");
  hRocEff->Draw("p");

  ErtLvl1Canv[iarm + 2]->Update();
  localStyle->SetOptTitle(1);
  ErtLvl1Canv[iarm + 2]->SetEditable(kFALSE);
  oldstyle->cd();

  return 0;
}

int ErtLvl1MonDraw::Draw(const char *what)
{

  if (!strcmp(what, "ALL"))
    {
      int iret = 0;
      iret += Draw("EAST");
      iret += Draw("WEST");
      iret += Draw("ROCeff");
      return iret;
    }

  if (!strcmp(what, "ROCeff"))
    {
      int iret = 0;
      iret += Draw("ROCeffE");
      iret += Draw("ROCeffW");
      return iret;
    }

  if (!strcmp(what, "ROCeffE"))
    return DrawRocEff(0);

  if (!strcmp(what, "ROCeffW"))
    return DrawRocEff(1);

  OnlMonClient *cl = OnlMonClient::instance();

  int index = 0;
  if (!strcmp(what, "WEST"))
    {
      index = 1;
    }
  const string ArmEW[2] = {"E", "W"};
  string EW = "ErtLvl1Mon_" + ArmEW[index];
  if (!gROOT->FindObject(EW.c_str()))
    {
      if (MakeCanvases(EW.c_str()))
	{
	  cout << PHWHERE << " Unknown Canvas: " << EW.c_str() << endl;
	  return -1;
	}
    }
  ErtLvl1Canv[index]->SetEditable(kTRUE);
  string htit = "ErtLvl1_FFA" + ArmEW[index];
  ErtLvl1_FFA[index] = cl->getHisto(htit.c_str());
  // this check for the first histogram is very important
  // if the server died or the html is generated
  // you will get called when your histograms are all
  // NULL, check for the first one and declare the server
  // dead is what you need to do. Otherwise you will
  // at least crash the html output and I will find you ...
  if (!ErtLvl1_FFA[index])
    {
      ErtLvl1Canv[index]->cd();
      ErtLvl1Canv[index]->Clear("D");
      transparent[index]->Clear();
      DrawDeadServer(transparent[index]);
      ErtLvl1Canv[index]->Update();
      cout << "ErtLvl1_DataErr[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "ErtLvl1_FFB" + ArmEW[index];
  ErtLvl1_FFB[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_FFC" + ArmEW[index];
  ErtLvl1_FFC[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_TBT" + ArmEW[index];
  ErtLvl1_TBT[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_ERON" + ArmEW[index];
  ErtLvl1_ERON[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_TERON" + ArmEW[index];
  ErtLvl1_TERON[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_FFASim" + ArmEW[index];
  ErtLvl1_FFASim[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_FFBSim" + ArmEW[index];
  ErtLvl1_FFBSim[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_FFCSim" + ArmEW[index];
  ErtLvl1_FFCSim[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_TBTSim" + ArmEW[index];
  ErtLvl1_TBTSim[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_ERONSim" + ArmEW[index];
  ErtLvl1_ERONSim[index] = cl->getHisto(htit.c_str());

  htit = "ErtLvl1_TERONSim" + ArmEW[index];
  ErtLvl1_TERONSim[index] = cl->getHisto(htit.c_str());
  TStyle *oldstyle = gStyle;
  localStyle->cd();

  // Create labels

  labeltext[0]->SetText( -0.8, 0, "No Sim" );
  labeltext[1]->SetText( 0.2, 0, "GL1  & Sim");
  labeltext[2]->SetText( 1.2, 0, "No GL1");

  for (int i = 0; i < 3; i++)
    {
      labeltext[i]->SetTextAngle(90);
      labeltext[i]->SetTextSize(0.06);
    }
  labeltext[3]->SetTextSize(0.06);

  //Fill Canvases
  bool IsTriggerActive[6];
  for (int itrig = 0; itrig < 6; itrig++) IsTriggerActive[itrig] = false;
  IsTriggerActive[index_4x4a] = true;
  IsTriggerActive[index_4x4b] = true;

  bool LogScaleSet;
  bool LogScaleSetSim;
  ostringstream EffTitle;

  ErtLvl1Pad[index][0]->cd();
  LogScaleSet = false;
  LogScaleSetSim = false;
  for (int j = 0; j < ErtLvl1_FFA[index]->GetNbinsX() + 1; j++)
    {
      if (ErtLvl1_FFA[index]->GetBinContent(index + 1) > 0)
	LogScaleSet = true;
      if (ErtLvl1_FFASim[index]->GetBinContent(index + 1) > 0)
	LogScaleSetSim = true;
    }
  if (!LogScaleSet || !LogScaleSetSim)
    {
      ErtLvl1Pad[index][0]->SetLogy(0);
    }
  else
    {
      ErtLvl1Pad[index][0]->SetLogy(1);
    }
  FormatHist(ErtLvl1_FFA[index]);
  ErtLvl1_FFA[index]->SetLabelColor(0, "x");
  if (IsTriggerActive[index_4x4a]) ErtLvl1_FFA[index]->DrawCopy();
  FormatHist(ErtLvl1_FFASim[index]);
  ErtLvl1_FFASim[index]->SetLineColor(2);
  if (IsTriggerActive[index_4x4a]) ErtLvl1_FFASim[index]->DrawCopy("same");

  for (int i = 0; i < 3; i++)
    {
      if (IsTriggerActive[index_4x4a]) labeltext[i]->Draw();
    }

  if (IsTriggerActive[index_4x4a]) EffTitle << " Efficiency = "
					    << (float)ErtLvl1_FFASim[index]->GetBinContent(7) / (float)(ErtLvl1_FFASim[index]->GetBinContent(7) +
													ErtLvl1_FFASim[index]->GetBinContent(11)) << endl;
  else EffTitle << "This should be blank" << endl;

  labeltext[3]->SetNDC(kTRUE);
  labeltext[3]->DrawText( 0.35, 0.05, EffTitle.str().c_str());
  EffTitle.str("");

  ErtLvl1Pad[index][1]->cd();
  LogScaleSet = false;
  LogScaleSetSim = false;
  for (int j = 0; j < ErtLvl1_FFB[index]->GetNbinsX() + 1; j++)
    {
      if (ErtLvl1_FFB[index]->GetBinContent(index + 1) > 0)
	LogScaleSet = true;
      if (ErtLvl1_FFBSim[index]->GetBinContent(index + 1) > 0)
	LogScaleSetSim = true;
    }
  if (!LogScaleSet || !LogScaleSetSim)
    {
      ErtLvl1Pad[index][1]->SetLogy(0);
    }
  else
    {
      ErtLvl1Pad[index][1]->SetLogy(1);
    }
  FormatHist(ErtLvl1_FFB[index]);
  ErtLvl1_FFB[index]->SetLabelColor(0, "x");

  if (IsTriggerActive[index_4x4b]) ErtLvl1_FFB[index]->DrawCopy();
  FormatHist(ErtLvl1_FFBSim[index]);
  ErtLvl1_FFBSim[index]->SetLineColor(2);
  if (IsTriggerActive[index_4x4b]) ErtLvl1_FFBSim[index]->DrawCopy("same");

  for (int i = 0; i < 3; i++)
    {
      if (IsTriggerActive[index_4x4b]) labeltext[i]->Draw();
    }

  if (IsTriggerActive[index_4x4b]) EffTitle << " Efficiency = "
					    << (float)ErtLvl1_FFBSim[index]->GetBinContent(7) / (float)(ErtLvl1_FFBSim[index]->GetBinContent(7) +
													ErtLvl1_FFBSim[index]->GetBinContent(11)) << endl;
  else EffTitle << "This should be blank" << endl;

  labeltext[3]->DrawText( 0.35, 0.05, EffTitle.str().c_str());
  EffTitle.str("");

  ErtLvl1Pad[index][2]->cd();
  LogScaleSet = false;
  LogScaleSetSim = false;
  for (int j = 0; j < ErtLvl1_FFC[index]->GetNbinsX() + 1; j++)
    {
      if (ErtLvl1_FFC[index]->GetBinContent(index + 1) > 0)
	LogScaleSet = true;
      if (ErtLvl1_FFCSim[index]->GetBinContent(index + 1) > 0)
	LogScaleSetSim = true;
    }
  if (!LogScaleSet || !LogScaleSetSim)
    {
      ErtLvl1Pad[index][2]->SetLogy(0);
    }
  else
    {
      ErtLvl1Pad[index][2]->SetLogy(1);
    }
  FormatHist(ErtLvl1_FFC[index]);
  ErtLvl1_FFC[index]->SetLabelColor(0, "x");
  if (IsTriggerActive[index_4x4c]) ErtLvl1_FFC[index]->DrawCopy();
  FormatHist(ErtLvl1_FFCSim[index]);
  ErtLvl1_FFCSim[index]->SetLineColor(2);
  if (IsTriggerActive[index_4x4c]) ErtLvl1_FFCSim[index]->DrawCopy("same");

  for (int i = 0; i < 3; i++)
    {
      if (IsTriggerActive[index_4x4c]) labeltext[i]->Draw();
    }

  if (IsTriggerActive[index_4x4c]) EffTitle << " Efficiency = "
					    << (float)ErtLvl1_FFCSim[index]->GetBinContent(7) / (float)(ErtLvl1_FFCSim[index]->GetBinContent(7) +
													ErtLvl1_FFCSim[index]->GetBinContent(11)) << endl;
  else EffTitle << "This should be blank" << endl;

  labeltext[3]->DrawText( 0.35, 0.05, EffTitle.str().c_str());
  EffTitle.str("");

  ErtLvl1Pad[index][4]->cd();
  LogScaleSet = false;
  LogScaleSetSim = false;
  for (int j = 0; j < ErtLvl1_TBT[index]->GetNbinsX() + 1; j++)
    {
      if (ErtLvl1_TBT[index]->GetBinContent(index + 1) > 0)
	LogScaleSet = true;
      if (ErtLvl1_TBTSim[index]->GetBinContent(index + 1) > 0)
	LogScaleSetSim = true;
    }
  if (!LogScaleSet || !LogScaleSetSim)
    {
      ErtLvl1Pad[index][4]->SetLogy(0);
    }
  else
    {
      ErtLvl1Pad[index][4]->SetLogy(1);
    }
  FormatHist(ErtLvl1_TBT[index]);
  ErtLvl1_TBT[index]->SetLabelColor(0, "x");
  if (IsTriggerActive[index_2x2]) ErtLvl1_TBT[index]->DrawCopy();
  FormatHist(ErtLvl1_TBTSim[index]);
  ErtLvl1_TBTSim[index]->SetLineColor(2);
  if (IsTriggerActive[index_2x2]) ErtLvl1_TBTSim[index]->DrawCopy("same");

  for (int i = 0; i < 3; i++)
    {
      if (IsTriggerActive[index_2x2]) labeltext[i]->Draw();
    }

  if (IsTriggerActive[index_2x2]) EffTitle << " Efficiency = "
					   << (float)ErtLvl1_TBTSim[index]->GetBinContent(7) / (float)(ErtLvl1_TBTSim[index]->GetBinContent(7) +
												       ErtLvl1_TBTSim[index]->GetBinContent(11)) << endl;
  else EffTitle << "This should be blank" << endl;

  labeltext[3]->DrawText( 0.35, 0.05, EffTitle.str().c_str());
  EffTitle.str("");


  ErtLvl1Pad[index][5]->cd();
  LogScaleSet = false;
  LogScaleSetSim = false;
  for (int j = 0; j < ErtLvl1_TERON[index]->GetNbinsX() + 1; j++)
    {
      if (ErtLvl1_TERON[index]->GetBinContent(index + 1) > 0)
	LogScaleSet = true;
      if (ErtLvl1_TERONSim[index]->GetBinContent(index + 1) > 0)
	LogScaleSetSim = true;
    }
  if (!LogScaleSet || !LogScaleSetSim)
    {
      ErtLvl1Pad[index][5]->SetLogy(0);
    }
  else
    {
      ErtLvl1Pad[index][5]->SetLogy(1);
    }
  FormatHist(ErtLvl1_TERON[index]);
  ErtLvl1_TERON[index]->SetLabelColor(0, "x");
  if(IsTriggerActive[index_TwoElectron]) ErtLvl1_TERON[index]->DrawCopy();
  FormatHist(ErtLvl1_TERONSim[index]);
  ErtLvl1_TERONSim[index]->SetLineColor(2);
  if(IsTriggerActive[index_TwoElectron]) ErtLvl1_TERONSim[index]->DrawCopy("same");

  for(int i = 0; i < 3; i++)
    {
      if(IsTriggerActive[index_TwoElectron]) labeltext[i]->Draw();
    }

  if(IsTriggerActive[index_TwoElectron]) EffTitle << " Efficiency = "
						  << (float)ErtLvl1_TERONSim[index]->GetBinContent(7) / (float)(ErtLvl1_TERONSim[index]->GetBinContent(7) +
														ErtLvl1_TERONSim[index]->GetBinContent(11)) << endl;
  else EffTitle << "This should be blank" << endl;
  labeltext[3]->DrawText( 0.35, 0.05, EffTitle.str().c_str());
  EffTitle.str("");


  ErtLvl1Pad[index][6]->cd();
  LogScaleSet = false;
  LogScaleSetSim = false;
  for (int j = 0; j < ErtLvl1_ERON[index]->GetNbinsX() + 1; j++)
    {
      if (ErtLvl1_ERON[index]->GetBinContent(index + 1) > 0)
	LogScaleSet = true;
      if (ErtLvl1_ERONSim[index]->GetBinContent(index + 1) > 0)
	LogScaleSetSim = true;
    }
  if (!LogScaleSet || !LogScaleSetSim)
    {
      ErtLvl1Pad[index][6]->SetLogy(0);
    }
  else
    {
      ErtLvl1Pad[index][6]->SetLogy(1);
    }
  FormatHist(ErtLvl1_ERON[index]);
  ErtLvl1_ERON[index]->SetLabelColor(0, "x");
  if (IsTriggerActive[index_Electron]) ErtLvl1_ERON[index]->DrawCopy();
  FormatHist(ErtLvl1_ERONSim[index]);
  ErtLvl1_ERONSim[index]->SetLineColor(2);
  if (IsTriggerActive[index_Electron]) ErtLvl1_ERONSim[index]->DrawCopy("same");

  for (int i = 0; i < 3; i++)
    {
      if (IsTriggerActive[index_Electron]) labeltext[i]->Draw();
    }

  if (IsTriggerActive[index_Electron]) EffTitle << " Efficiency = "
						<< (float)ErtLvl1_ERONSim[index]->GetBinContent(7) / (float)(ErtLvl1_ERONSim[index]->GetBinContent(7) +
													     ErtLvl1_ERONSim[index]->GetBinContent(11)) << endl;
  else EffTitle << "This should be blank" << endl;

  labeltext[3]->DrawText( 0.35, 0.05, EffTitle.str().c_str());
    EffTitle.str("");

    // Done with pads
    transparent[index]->SetEditable(kTRUE);
    transparent[index]->cd();
    transparent[index]->Clear();
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.05);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;

    if (index == 0)
        {
            runnostream << "ERT LL1 EAST : Run " << cl->RunNumber();
        }
    else
        {
            runnostream << "ERT LL1 WEST : Run " << cl->RunNumber();
        }

    time_t evttime = cl->EventTime("CURRENT");
    runnostream << " " << ctime(&evttime) ;
    PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
    transparent[index]->SetEditable(kFALSE);

    ErtLvl1Canv[index]->Update();
    ErtLvl1Canv[index]->SetEditable(kFALSE);
    oldstyle->cd();

    return 0;
}

int ErtLvl1MonDraw::MakePS(const char *what)
{

    OnlMonClient *cl = OnlMonClient::instance();
    ostringstream filename;
    int iret = Draw(what);
    if (iret)
        {
            return iret;
        }
    for (int ic = 0; ic < NUM_CANVASES; ic++)
        {
            filename.str("");
            filename << ThisName << "_" << ic + 1 << "_" << cl->RunNumber() << ".ps";
            ErtLvl1Canv[ic]->Print(filename.str().c_str());
        }

    return 0;
}

int ErtLvl1MonDraw::MakeHtml(const char *what)
{

    int iret = Draw(what);
    if (iret)
        {
            return iret;
        }
    OnlMonClient *cl = OnlMonClient::instance();
    string ofile = cl->htmlRegisterPage(*this, "East", "1", "png");
    cl->CanvasToPng(ErtLvl1Canv[0], ofile);
    ofile = cl->htmlRegisterPage(*this, "West", "2", "png");
    cl->CanvasToPng(ErtLvl1Canv[1], ofile);
    ofile = cl->htmlRegisterPage(*this, "ROC Eff East", "3", "png");
    cl->CanvasToPng(ErtLvl1Canv[2], ofile);
    ofile = cl->htmlRegisterPage(*this, "ROC Eff West", "4", "png");
    cl->CanvasToPng(ErtLvl1Canv[3], ofile);
    cl->SaveLogFile(*this);

    return 0;
}

void ErtLvl1MonDraw::FormatHist(TH1 *Hist)
{

    Hist->SetStats(0);
    Hist->SetLineWidth(1);
    Hist->SetLineColor(4);
    Hist->SetTitleSize(0.9);
    Hist->GetXaxis()->SetLabelSize(0.06);
    Hist->GetYaxis()->SetLabelSize(0.05);
    return ;
}

int ErtLvl1MonDraw::DrawDeadServer(TPad *transparent)
{
    transparent->cd();
    TText FatalMsg;
    FatalMsg.SetTextFont(62);
    FatalMsg.SetTextSize(0.06);
    FatalMsg.SetTextColor(4);
    FatalMsg.SetNDC();  // set to normalized coordinates
    FatalMsg.SetTextAlign(23); // center/top alignment
    FatalMsg.DrawText(0.5, 0.9, "ERTLVL1MONITOR");
    FatalMsg.SetTextAlign(22); // center/center alignment
    FatalMsg.DrawText(0.5, 0.65, "SERVER");
    FatalMsg.SetTextAlign(22); // center/bottom alignment
    FatalMsg.DrawText(0.5, 0.4, "DEAD OR");
    FatalMsg.SetTextAlign(21); // center/bottom alignment
    FatalMsg.DrawText(0.5, 0.15, "NO EVENTS WITH ERTLL1 PKTS YET");
    transparent->Update();
    return 0;
}



