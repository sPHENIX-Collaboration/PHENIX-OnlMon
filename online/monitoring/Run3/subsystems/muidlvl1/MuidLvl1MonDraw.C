#include <MuidLvl1MonDraw.h>
#include <MuidLvl1MonDefs.h>

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
#include <TPaveText.h>

#include <fstream>
#include <sstream>

enum infohist_indexes{NOTHING, OFFLINE_INDEX, NUM_EVTS_INDEX};
#define STATUS_PAD_INDEX 15

using namespace std;

MuidLvl1MonDraw::MuidLvl1MonDraw(): OnlMonDraw("MUIDLVL1MON")
{
  TStyle *oldstyle = gStyle;
  localStyle = new TStyle("muidlvl1style", "MuID LL1 Online Monitor style");

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
  localStyle -> SetTitleX(0.10);    // x of top left corner of title box
  localStyle -> SetTitleY(0.999);   // y of top left corner of title box
  localStyle -> SetTitleH(0.12);
  localStyle -> SetTitleW(0.8);
  localStyle -> SetLabelSize(0.5, "xyz");
  localStyle -> SetTitleSize(0.5, "xyz");
  oldstyle->cd();
  for (int i = 0; i<NUM_CANVASES; i++)
    {
			tptext_status[i] = new TPaveText(0.0,0.0,1.0,1.0,"NDC");
      MuidLvl1Canv[i] = 0;
      transparent[i] = 0;
      for (int j=0; j<MAX_PADS_PER_CANVAS;j++)
	{
	  MuidLvl1Pad[i][j] = 0;
	}
    }
		
		
}

int MuidLvl1MonDraw::MakeCanvases(const char *name)
{
  if (!strcmp(name, "MuidLvl1Mon_S"))
    {
      DrawCommonCanvas(0);
    }

  else if (!strcmp(name, "MuidLvl1Mon_N"))
    {
      DrawCommonCanvas(1);
    }
  else
    {
      return -1;
    }
  return 0;
}

int
MuidLvl1MonDraw::DrawCommonCanvas(const int index)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  ostringstream PadName, Title;
  const string ArmNS[2] ={"S", "N"};
  const string ArmNSFull[2] = {"South", "North"};
  Title << "MuidLvl1Mon " << ArmNSFull[index]
	<< " System Overview" ;
  PadName << "MuidLvl1Mon_" << ArmNS[index] ;
  MuidLvl1Canv[index] = new TCanvas(PadName.str().c_str(), Title.str().c_str(), -1, 0, xsize, ysize);
  gSystem->ProcessEvents();
  //  MuidLvl1Canv[index]->cd();
  double xlow, ylow, xhigh, yhigh;
  for (int ipad = 0; ipad < MAX_PADS_PER_CANVAS; ipad++)
    {
      PadName.str("");
      PadName << "MuidLvl1Pad" << ArmNS[index] << ipad ;
      if (ipad < 4)
        {
          xlow = ipad * 0.25;
          ylow = 0.73;
          xhigh = (ipad + 1) * 0.25;
          yhigh = 0.94;
        }
      else if ( (ipad >= 4) && (ipad < 8) )
        {
          xlow = (ipad - 4) * 0.25;
          ylow = 0.49;
          xhigh = (ipad - 3) * 0.25;
          yhigh = 0.71;
        }
      else if ( (ipad >= 8) && (ipad < 12) )
        {
          xlow = (ipad - 8) * 0.25;
          ylow = 0.25;
          xhigh = (ipad - 7) * 0.25;
          yhigh = 0.47;
        }
      else if(ipad==STATUS_PAD_INDEX)
	{
          xlow = (ipad - 12) * 0.25;
          ylow = 0.0;
          xhigh = (ipad - 11) * 0.25;
          yhigh = 0.23;
        }
      // else if(ipad==16)
			// {
          // xlow = 3 * 0.25;
          // ylow = 0.0;
          // xhigh = 4 * 0.25;
          // yhigh = 0.08;
        // }
      else
        {
          xlow = (ipad - 12) * 0.25;
          ylow = 0.0;
          xhigh = (ipad - 11) * 0.25;
          yhigh = 0.23;
        }
      MuidLvl1Pad[index][ipad] = new TPad(PadName.str().c_str(), "", xlow, ylow, xhigh, yhigh, 0, 0);
      MuidLvl1Pad[index][ipad]->Draw();
    }
		
		
  PadName.str("");
  PadName << "transparent" << ArmNS[index] ;
  transparent[index] = new TPad(PadName.str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[index]->SetFillStyle(4000);
  transparent[index]->Draw();

  return 0;
}

int
MuidLvl1MonDraw::Draw(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  const string ArmNS[2] = {"S", "N"};
  int index = 0;
  if (!strcmp(what,"ALL"))
  {
    int iret = 0;
    iret += Draw("SOUTH");
    iret += Draw("NORTH");
    return iret;
  }
  if (!strcmp(what, "NORTH"))
    {
      index = 1;
    }
  string NS = "MuidLvl1Mon_" + ArmNS[index];
  if (!gROOT->FindObject(NS.c_str()))
    {
      if (MakeCanvases(NS.c_str()))
	{
	  cout << PHWHERE << " Unknown Canvas: " << NS.c_str() << endl;
	  return -1;
	}
    }
  MuidLvl1Canv[index]->SetEditable(1);
  TStyle *oldstyle = gStyle;

  string htit = "MuidLvl1_DataErr" + ArmNS[index];
  MuidLvl1_DataErr[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_DataErr[index])
    {
      MuidLvl1Canv[index]->cd();
      MuidLvl1Canv[index]->Clear("D");
      transparent[index]->Clear();
      DrawDeadServer(transparent[index]);
      MuidLvl1Canv[index]->Update();
      cout << "MuidLvl1_DataErr[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_DataErr2" + ArmNS[index];
  MuidLvl1_DataErr2[index] = cl->getHisto(htit.c_str());

  if (!MuidLvl1_DataErr2[index])
    {
      cout << "MuidLvl1_DataErr2[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_SyncErr" + ArmNS[index];
  MuidLvl1_SyncErr[index] = cl->getHisto(htit.c_str());

  if (!MuidLvl1_SyncErr[index])
    {
      cout << "MuidLvl1_SyncErr[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_SyncErr2" + ArmNS[index];
  MuidLvl1_SyncErr2[index] = cl->getHisto(htit.c_str());

  if (!MuidLvl1_SyncErr2[index])
    {
      cout << "MuidLvl1_SyncErr2[" << index << "] does not exist!" << endl;
      return -1;
    }


  htit = "MuidLvl1_HorRoadNum" + ArmNS[index];
  MuidLvl1_HorRoadNum[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadNum[index])
    {
      cout << "MuidLvl1_HorRoadNum[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadNumShal" + ArmNS[index];
  MuidLvl1_HorRoadNumShal[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadNumShal[index])
    {
      cout << "MuidLvl1_HorRoadNumShal[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadNum" + ArmNS[index];
  MuidLvl1_VerRoadNum[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadNum[index])
    {
      cout << "MuidLvl1_VerRoadNum[" << index << "[ does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadNumShal" + ArmNS[index];
  MuidLvl1_VerRoadNumShal[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadNumShal[index])
    {
      cout << "MuidLvl1_VerRoadNumShal[" << index << "[ does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadNumSim" + ArmNS[index];
  MuidLvl1_HorRoadNumSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadNumSim[index])
    {
      cout << "MuidLvl1_HorRoadNumSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadNumShalSim" + ArmNS[index];
  MuidLvl1_HorRoadNumShalSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadNumShalSim[index])
    {
      cout << "MuidLvl1_HorRoadNumShalSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadNumSim" + ArmNS[index];
  MuidLvl1_VerRoadNumSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadNumSim[index])
    {
      cout << "MuidLvl1_VerRoadNumSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadNumShalSim" + ArmNS[index];
  MuidLvl1_VerRoadNumShalSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadNumShalSim[index])
    {
      cout << "MuidLvl1_VerRoadNumShalSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadCount" + ArmNS[index];
  MuidLvl1_HorRoadCount[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadCount[index])
    {
      cout << "MuidLvl1_HorRoadCount[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadCountShal" + ArmNS[index];
  MuidLvl1_HorRoadCountShal[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadCountShal[index])
    {
      cout << "MuidLvl1_HorRoadCountShal[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadCount" + ArmNS[index];
  MuidLvl1_VerRoadCount[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadCount[index])
    {
      cout << "MuidLvl1_VerRoadCount[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadCountShal" + ArmNS[index];
  MuidLvl1_VerRoadCountShal[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadCountShal[index])
    {
      cout << "MuidLvl1_VerRoadCountShal[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadCountSim" + ArmNS[index];
  MuidLvl1_HorRoadCountSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadCountSim[index])
    {
      cout << "MuidLvl1_HorRoadCountSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadCountShalSim" + ArmNS[index];
  MuidLvl1_HorRoadCountShalSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadCountShalSim[index])
    {
      cout << "MuidLvl1_HorRoadCountShalSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadCountSim" + ArmNS[index];
  MuidLvl1_VerRoadCountSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadCountSim[index])
    {
      cout << "MuidLvl1_VerRoadCountSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadCountShalSim" + ArmNS[index];
  MuidLvl1_VerRoadCountShalSim[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadCountShalSim[index])
    {
      cout << "MuidLvl1_VerRoadCountShalSim[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorRoadCountDif" + ArmNS[index];
  MuidLvl1_HorRoadCountDif[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorRoadCountDif[index])
    {
      cout << "MuidLvl1_HorRoadCountDif[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerRoadCountDif" + ArmNS[index];
  MuidLvl1_VerRoadCountDif[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerRoadCountDif[index])
    {
      cout << "MuidLvl1_VerRoadCountDif[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_CompareVH" + ArmNS[index];
  MuidLvl1_CompareVH[index] = dynamic_cast <TH2 *> (cl->getHisto(htit.c_str()));
  if (!MuidLvl1_CompareVH[index])
    {
      cout << "MuidLvl1_CompareVH[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_CompareSumsH" + ArmNS[index];
  MuidLvl1_CompareSumsH[index] = dynamic_cast <TH2 *> (cl->getHisto(htit.c_str()));
  if (!MuidLvl1_CompareSumsH[index])
    {
      cout << "MuidLvl1_CompareSumsH[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_CompareSumsV" + ArmNS[index];
  MuidLvl1_CompareSumsV[index] = dynamic_cast <TH2 *> (cl->getHisto(htit.c_str()));
  if (!MuidLvl1_CompareSumsV[index])
    {
      cout << "MuidLvl1_CompareSumsV[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_ESNMismatch" + ArmNS[index];
  MuidLvl1_ESNMismatch[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_ESNMismatch[index])
    {
      cout << "MuidLvl1_ESNMismatch[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_ESNCtlMis_V" + ArmNS[index];
  MuidLvl1_ESNCtlMis_V[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_ESNCtlMis_V[index])
    {
      cout << "MuidLvl1_ESNCtlMis_V[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_ESNCtlMis_H" + ArmNS[index];
  MuidLvl1_ESNCtlMis_H[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_ESNCtlMis_H[index])
    {
      cout << "MuidLvl1_ESNCtlMis_H[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorSymEff" + ArmNS[index];
  MuidLvl1_HorSymEff[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorSymEff[index])
    {
      cout << "MuidLvl1_HorSymEff[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerSymEff" + ArmNS[index];
  MuidLvl1_VerSymEff[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerSymEff[index])
    {
      cout << "MuidLvl1_VerSymEff[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorSymEffCut" + ArmNS[index];
  MuidLvl1_HorSymEffCut[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorSymEffCut[index])
    {
      cout << "MuidLvl1_HorSymEffCut[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerSymEffCut" + ArmNS[index];
  MuidLvl1_VerSymEffCut[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerSymEffCut[index])
    {
      cout << "MuidLvl1_VerSymEffCut[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_HorSymEffCutShal" + ArmNS[index];
  MuidLvl1_HorSymEffCutShal[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_HorSymEffCutShal[index])
    {
      cout << "MuidLvl1_HorSymEffShalCut[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_VerSymEffCutShal" + ArmNS[index];
  MuidLvl1_VerSymEffCutShal[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_VerSymEffCutShal[index])
    {
      cout << "MuidLvl1_VerSymEffShalCut[" << index << "] does not exist!" << endl;
      return -1;
    }

  htit = "MuidLvl1_GL1Eff" + ArmNS[index];
  MuidLvl1_GL1Eff[index] = cl->getHisto(htit.c_str());
  if (!MuidLvl1_GL1Eff[index])
    {
      cout << "MuidLvl1_GL1Eff[" << index << "] does not exist!" << endl;
      return -1;
    }

  MuidLvl1_InfoHist = cl->getHisto("MuidLvl1_InfoHist");
  if (!MuidLvl1_InfoHist)
    {
      cout << "MuidLvl1_InfoHist does not exist!" << endl;
      return -1;
    }
		
		

  // Fill Canvases
  localStyle->cd();

  bool LogScaleSet;

  MuidLvl1Canv[index]->cd();

  MuidLvl1Pad[index][0]->cd();
  LogScaleSet = false;
  for (int j = 0;j < 101;j++)
    {
      if (MuidLvl1_SyncErr[index]->GetBinContent(index + 1) > 0)
        {
          MuidLvl1Pad[index][0]->SetLogy(1);
          LogScaleSet = true;
          break;
        }
    }
  if (!LogScaleSet)
    {
      MuidLvl1Pad[index][0]->SetLogy(0);
    }
  FormatHist(MuidLvl1_SyncErr[index]);
  MuidLvl1_SyncErr[index]->SetLineColor(2);
  MuidLvl1_SyncErr[index]->DrawCopy();

  MuidLvl1Pad[index][4]->cd();
  LogScaleSet = false;
  for (int j = 0;j < 101;j++)
    {
      if (MuidLvl1_DataErr[index]->GetBinContent(index + 1) > 0)
        {
          MuidLvl1Pad[index][4]->SetLogy(1);
          LogScaleSet = true;
          break;
        }
    }
  if (!LogScaleSet)
    {
      MuidLvl1Pad[index][4]->SetLogy(0);
    }
  FormatHist(MuidLvl1_DataErr[index]);
  MuidLvl1_DataErr[index]->SetLineColor(2);
  MuidLvl1_DataErr[index]->DrawCopy();

  /*
  MuidLvl1Pad[index][1]->cd();
  FormatHist(MuidLvl1_HorRoadCountShal[index]);
  MuidLvl1_HorRoadCountShal[index]->SetLineColor(2);
  MuidLvl1_HorRoadCountShal[index]->DrawCopy();
  FormatHist(MuidLvl1_HorRoadCount[index]);
  MuidLvl1_HorRoadCount[index]->DrawCopy("same");
  */
  MuidLvl1Pad[index][1]->cd();
  FormatHist(MuidLvl1_HorRoadCount[index]);
  MuidLvl1_HorRoadCount[index]->SetLineColor(2);
  MuidLvl1_HorRoadCount[index]->DrawCopy();
  FormatHist(MuidLvl1_HorRoadCountShal[index]);
  MuidLvl1_HorRoadCountShal[index]->DrawCopy("same");

  /*
  MuidLvl1Pad[index][5]->cd();
  FormatHist(MuidLvl1_VerRoadCountShal[index]);
  MuidLvl1_VerRoadCountShal[index]->SetLineColor(2);
  MuidLvl1_VerRoadCountShal[index]->DrawCopy();
  FormatHist(MuidLvl1_VerRoadCount[index]);
  MuidLvl1_VerRoadCount[index]->DrawCopy("same");
  */
  MuidLvl1Pad[index][5]->cd();
  FormatHist(MuidLvl1_VerRoadCount[index]);
  MuidLvl1_VerRoadCount[index]->SetLineColor(2);
  MuidLvl1_VerRoadCount[index]->DrawCopy();
  FormatHist(MuidLvl1_VerRoadCountShal[index]);
  MuidLvl1_VerRoadCountShal[index]->DrawCopy("same");

  /*
  MuidLvl1Pad[index][2]->cd();
  FormatHist(MuidLvl1_HorRoadCountShalSim[index]);
  MuidLvl1_HorRoadCountShalSim[index]->SetLineColor(2);
  MuidLvl1_HorRoadCountShalSim[index]->DrawCopy();
  FormatHist(MuidLvl1_HorRoadCountSim[index]);
  MuidLvl1_HorRoadCountSim[index]->DrawCopy("same");
  */
  MuidLvl1Pad[index][2]->cd();
  FormatHist(MuidLvl1_HorRoadCountSim[index]);
  MuidLvl1_HorRoadCountSim[index]->SetLineColor(2);
  MuidLvl1_HorRoadCountSim[index]->DrawCopy();
  FormatHist(MuidLvl1_HorRoadCountShalSim[index]);
  MuidLvl1_HorRoadCountShalSim[index]->DrawCopy("same");

  /*
  MuidLvl1Pad[index][6]->cd();
  FormatHist(MuidLvl1_VerRoadCountShalSim[index]);
  MuidLvl1_VerRoadCountShalSim[index]->SetLineColor(2);
  MuidLvl1_VerRoadCountShalSim[index]->DrawCopy();
  FormatHist(MuidLvl1_VerRoadCountSim[index]);
  MuidLvl1_VerRoadCountSim[index]->DrawCopy("same");
  */
  MuidLvl1Pad[index][6]->cd();
  FormatHist(MuidLvl1_VerRoadCountSim[index]);
  MuidLvl1_VerRoadCountSim[index]->SetLineColor(2);
  MuidLvl1_VerRoadCountSim[index]->DrawCopy();
  FormatHist(MuidLvl1_VerRoadCountShalSim[index]);
  MuidLvl1_VerRoadCountShalSim[index]->DrawCopy("same");

  /*
  MuidLvl1Pad[index][3]->cd();
  FormatHist(MuidLvl1_HorSymEffCut[index]);
  MuidLvl1_HorSymEffCut[index]->DrawCopy();
  FormatHist(MuidLvl1_HorSymEffCutShal[index]);
  MuidLvl1_HorSymEffCutShal[index]->SetLineColor(2);
  MuidLvl1_HorSymEffCutShal[index]->DrawCopy("same");

  MuidLvl1Pad[index][7]->cd();
  FormatHist(MuidLvl1_VerSymEffCut[index]);
  MuidLvl1_VerSymEffCut[index]->DrawCopy();
  FormatHist(MuidLvl1_VerSymEffCutShal[index]);
  MuidLvl1_VerSymEffCutShal[index]->SetLineColor(2);
  MuidLvl1_VerSymEffCutShal[index]->DrawCopy("same");
  */
  MuidLvl1Pad[index][3]->cd();
  FormatHist(MuidLvl1_HorSymEffCut[index]);
  MuidLvl1_HorSymEffCut[index]->SetLineColor(2);
  MuidLvl1_HorSymEffCut[index]->DrawCopy();
  FormatHist(MuidLvl1_HorSymEffCutShal[index]);
  //MuidLvl1_HorSymEffCutShal[index]->DrawCopy("same");

  MuidLvl1Pad[index][7]->cd();
  FormatHist(MuidLvl1_VerSymEffCut[index]);
  MuidLvl1_VerSymEffCut[index]->SetLineColor(2);
  MuidLvl1_VerSymEffCut[index]->DrawCopy();
  FormatHist(MuidLvl1_VerSymEffCutShal[index]);
  //MuidLvl1_VerSymEffCutShal[index]->DrawCopy("same");

  MuidLvl1Pad[index][8]->cd();
  FormatHist(MuidLvl1_SyncErr2[index]);
  MuidLvl1_SyncErr2[index]->DrawCopy();

  MuidLvl1Pad[index][12]->cd();
  FormatHist(MuidLvl1_DataErr2[index]);
  MuidLvl1_DataErr2[index]->DrawCopy();

  MuidLvl1Pad[index][9]->cd();
  if ( (MuidLvl1_HorRoadNum[index]->GetEntries() > 0) && (MuidLvl1_HorRoadNumSim[index]->GetEntries() > 0) )
    {
      MuidLvl1Pad[index][9]->SetLogy(1);
    }
  else
    {
      MuidLvl1Pad[index][9]->SetLogy(0);
    }
  FormatHist(MuidLvl1_HorRoadNum[index]);
  MuidLvl1_HorRoadNum[index]->DrawCopy();
  FormatHist(MuidLvl1_HorRoadNumSim[index]);
  MuidLvl1_HorRoadNumSim[index]->SetLineColor(2);
  MuidLvl1_HorRoadNumSim[index]->DrawCopy("same");

  MuidLvl1Pad[index][13]->cd();
  if ( (MuidLvl1_VerRoadNum[index]->GetEntries() > 0) && (MuidLvl1_VerRoadNumSim[index]->GetEntries() > 0) )
    {
      MuidLvl1Pad[index][13]->SetLogy(1);
    }
  else
    {
      MuidLvl1Pad[index][13]->SetLogy(0);
    }
  FormatHist(MuidLvl1_VerRoadNum[index]);
  MuidLvl1_VerRoadNum[index]->DrawCopy();
  FormatHist(MuidLvl1_VerRoadNumSim[index]);
  MuidLvl1_VerRoadNumSim[index]->SetLineColor(2);
  MuidLvl1_VerRoadNumSim[index]->DrawCopy("same");

  MuidLvl1Pad[index][10]->cd();
  if ( (MuidLvl1_HorRoadNumShal[index]->GetEntries() > 0) && (MuidLvl1_HorRoadNumShalSim[index]->GetEntries() > 0) )
    {
      MuidLvl1Pad[index][10]->SetLogy(1);
    }
  else
    {
      MuidLvl1Pad[index][10]->SetLogy(0);
    }
  FormatHist(MuidLvl1_HorRoadNumShal[index]);
  MuidLvl1_HorRoadNumShal[index]->DrawCopy();
  FormatHist(MuidLvl1_HorRoadNumShalSim[index]);
  MuidLvl1_HorRoadNumShalSim[index]->SetLineColor(2);
  MuidLvl1_HorRoadNumShalSim[index]->DrawCopy("same");

  MuidLvl1Pad[index][14]->cd();
  if ( (MuidLvl1_VerRoadNumShal[index]->GetEntries() > 0) && (MuidLvl1_VerRoadNumShalSim[index]->GetEntries() > 0) )
    {
      MuidLvl1Pad[index][14]->SetLogy(1);
    }
  else
    {
      MuidLvl1Pad[index][14]->SetLogy(0);
    }
  FormatHist(MuidLvl1_VerRoadNumShal[index]);
  MuidLvl1_VerRoadNumShal[index]->DrawCopy();
  FormatHist(MuidLvl1_VerRoadNumShalSim[index]);
  MuidLvl1_VerRoadNumShalSim[index]->SetLineColor(2);
  MuidLvl1_VerRoadNumShalSim[index]->DrawCopy("same");

  //MuidLvl1Pad[index][10]->cd();
  //FormatHist(MuidLvl1_CompareSumsH[index]);
  //MuidLvl1_CompareSumsH[index]->DrawCopy();

  //MuidLvl1Pad[index][14]->cd();
  //FormatHist(MuidLvl1_CompareSumsV[index]);
  //MuidLvl1_CompareSumsV[index]->DrawCopy();

  MuidLvl1Pad[index][11]->cd();
  FormatHist(MuidLvl1_CompareVH[index]);
  MuidLvl1_CompareVH[index]->DrawCopy();

  //MuidLvl1Pad[index][STATUS_PAD_INDEX]->cd();
  //FormatHist(MuidLvl1_ESNMismatch[index]);
  //MuidLvl1_ESNMismatch[index]->DrawCopy();

  // Add the titles, run number, etc:

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
      runnostream << "MUID LL1 SOUTH : Run " << cl->RunNumber();
    }
  else
    {
      runnostream << "MUID LL1 NORTH : Run " << cl->RunNumber();
    }

  time_t evttime = cl->EventTime("CURRENT");
  runnostream << " " << ctime(&evttime) ;
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  transparent[index]->SetFillStyle(4000);  // default is transparent
  transparent[index]->SetEditable(kFALSE);

  // Add text for GL1 efficiencies

  // A decision is made here - do we display this in a little box
  // in the corner, or in the middle of the pad? If there is no
  // MuIDLL1 data for the simulator to operate on then the
  // histograms will be empty. 

  if(MuidLvl1_HorRoadCount[index]->GetEntries()>0){ 

    // Histo has entries, so we go in the corner
		stringstream ss;
    MuidLvl1Pad[index][STATUS_PAD_INDEX]->cd();
		MuidLvl1Pad[index][STATUS_PAD_INDEX]->Clear();
		
		
		tptext_status[index]->Clear();
		tptext_status[index]->SetTextSize(0.1);

		int offline = (int)MuidLvl1_InfoHist->GetBinContent(OFFLINE_INDEX);
		ss.clear();ss.str("");
		if(offline)	ss << "Offline Postprocessor Output";
		else ss << "Online Output";
		tptext_status[index]->AddText(ss.str().c_str());
		
		int NumEvts = MuidLvl1_InfoHist->GetBinContent(NUM_EVTS_INDEX);
		ss.clear();ss.str("");
		ss << "Num Evts Processed: ";
		ss << NumEvts;
		tptext_status[index]->AddText(ss.str().c_str());
		
		ss.clear();ss.str("");
		ss << "SIMULATOR MODE  ";
		tptext_status[index]->AddText(ss.str().c_str());
		
		ss.clear();ss.str("");
		ss << "GL1 Eff. (1D)  = ";
		ss << (float)MuidLvl1_GL1Eff[index]->GetBinContent(1);
		tptext_status[index]->AddText(ss.str().c_str());
		
		ss.clear();ss.str("");
		ss << "GL1 Eff. (2D)  = ";
		ss << (float)MuidLvl1_GL1Eff[index]->GetBinContent(2) << endl;
		tptext_status[index]->AddText(ss.str().c_str());
		
		ss.clear();ss.str("");
		ss << "Ratio 1D GL1/Sim  = ";
		ss << (float)MuidLvl1_GL1Eff[index]->GetBinContent(5) << endl;
		tptext_status[index]->AddText(ss.str().c_str());
		
		ss.clear();ss.str("");
		ss << "Ratio 2D GL1/Sim  = ";
		ss << (float)MuidLvl1_GL1Eff[index]->GetBinContent(6) << endl;
		tptext_status[index]->AddText(ss.str().c_str());
		
		tptext_status[index]->Draw();
		MuidLvl1Pad[index][STATUS_PAD_INDEX]->Update();
		
  }
  else{

    transparent[index]->SetEditable(kTRUE);
    transparent[index]->SetFillStyle(4075);  // No longer transparent
    transparent[index]->cd();

    ostringstream EffText; 
    TText *labeltext = new TText();
    labeltext->SetTextSize(0.10);
    labeltext->SetNDC(kTRUE);
    labeltext->SetTextAlign(23);

    EffText.str("");

    float checkNum = (float)MuidLvl1_GL1Eff[index]->GetBinContent(1);
    if( !(checkNum>0) && !(checkNum<0) && !(checkNum==0) ){
      EffText  << " GL1 Eff. (1D) = (too few events) " << endl;  
      labeltext->SetTextSize(0.08); 
    }
    else{
      EffText  << " GL1 Eff. (1D)  = "   
	   << (float)MuidLvl1_GL1Eff[index]->GetBinContent(1) << endl;  
    }

    labeltext->DrawText( 0.5, 0.75, EffText.str().c_str());

    EffText.str("");

    /*
    // test for NAN
    checkNum = (float)MuidLvl1_GL1Eff[index]->GetBinContent(3);
    if( !(checkNum>0) && !(checkNum<0) && !(checkNum==0) ){
      EffText  << " GL1 Eff. (1H) = (too few events) " << endl;  
      labeltext->SetTextSize(0.08); 
    }
    else{
      EffText  << " GL1 Eff. (1H)  = "
	   << (float)MuidLvl1_GL1Eff[index]->GetBinContent(3) << endl;  
    }

    labeltext->DrawText( 0.5, 0.5, EffText.str().c_str());

    EffText.str("");
    */

    /*
    // test for NAN
    checkNum = (float)MuidLvl1_GL1Eff[index]->GetBinContent(4);
    if( !(checkNum>0) && !(checkNum<0) && !(checkNum==0) ){
      EffText  << " GL1 Eff. (1D1S) = (too few events) " << endl;  
      labeltext->SetTextSize(0.08); 
    }
    else{
      EffText  << " GL1 Eff. (1D1S)  = "
	   << (float)MuidLvl1_GL1Eff[index]->GetBinContent(4) << endl;
    }
    */
    
    // test for NAN
    checkNum = (float)MuidLvl1_GL1Eff[index]->GetBinContent(2);
    if( !(checkNum>0) && !(checkNum<0) && !(checkNum==0) ){
      EffText  << " GL1 Eff. (2D) = (too few events) " << endl;  
      labeltext->SetTextSize(0.08); 
    }
    else{
      EffText  << " GL1 Eff. (2D)  = "
	   << (float)MuidLvl1_GL1Eff[index]->GetBinContent(2) << endl;
    }

    labeltext->DrawText( 0.5, 0.15, EffText.str().c_str());

    delete labeltext; 
    transparent[index]->SetEditable(kFALSE);

  }
	
  // Finish up

  MuidLvl1Canv[index]->Update();
  MuidLvl1Canv[index]->SetEditable(kFALSE);
  oldstyle->cd();

  return 0;
}

int 
MuidLvl1MonDraw::MakePS(const char *what)
{

  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename, filename2;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  MuidLvl1Canv[0]->Print(filename.str().c_str());
  filename2 << ThisName << "_2_" << cl->RunNumber() << ".ps";
  MuidLvl1Canv[1]->Print(filename2.str().c_str());

  return 0;
}

int MuidLvl1MonDraw::MakeHtml(const char *what)
{

  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();
  string ofile = cl->htmlRegisterPage(*this, "South", "1", "png");
  cl->CanvasToPng(MuidLvl1Canv[0], ofile);
  ofile = cl->htmlRegisterPage(*this, "North", "2", "png");
  cl->CanvasToPng(MuidLvl1Canv[1], ofile);
  cl->SaveLogFile(*this);

  return 0;
}

void MuidLvl1MonDraw::FormatHist(TH1 *Hist)
{

  Hist->SetStats(0);
  Hist->SetLineWidth(1);
  Hist->SetLineColor(4);
  Hist->SetTitleSize(0.8);
  Hist->GetXaxis()->SetLabelSize(0.07);
  Hist->GetYaxis()->SetLabelSize(0.07);
  return ;
}

int MuidLvl1MonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.06);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "MUIDLVL1MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.65, "SERVER");
  FatalMsg.SetTextAlign(22); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.4, "DEAD OR");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.15, "NO EVENTS WITH MUIDLL1 PKTS YET");
  transparent->Update();
  return 0;
}

MuidLvl1MonDraw::~MuidLvl1MonDraw()
{
  for (int i = 0; i<NUM_CANVASES; i++)
  {
			delete tptext_status[i];
	}

	return;
}