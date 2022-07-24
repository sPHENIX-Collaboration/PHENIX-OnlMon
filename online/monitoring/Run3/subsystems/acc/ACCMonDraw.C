// online monitoring includes
#include "ACCMonDraw.h"
#include "ACCMonDefs.h"

#include <OnlMonClient.h>

// offline includes
#include <phool.h>

// root includes
#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

// System includes
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

ACCMonDraw::ACCMonDraw(const char *name):
  OnlMonDraw(name),
  run(-1),
  evt(0),
  accevt(0),
  brind(NULL)
{
  memset(TC, 0, sizeof(TC));
  memset(Pad, 0, sizeof(Pad));
  memset(transparent, 0, sizeof(transparent));
  memset(msg, 0, sizeof(msg));
  title[0] = new TText(0.25, 0.95, "Aerogel Cherenkov Counter monitor");
  title[1] = new TText();
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  message[j][i] = new TText();
	  message[j][i]->SetTextSize(1);
	}
    }

  northsouth[0] = new TText();
  northsouth[1] = new TText();
  northsouth[0]->SetTextSize(0.1);
  northsouth[1]->SetTextSize(0.1);
  northsouth[0]->SetTextColor(2);
  northsouth[1]->SetTextColor(4);

  ln1 = new TLine(0, ACC_PMT_NOHIT, ACC_ALLCHANNEL, ACC_PMT_NOHIT);
  ln1->SetLineColor(3);


  os = gStyle;
  os->SetOptStat(0);
  os->SetTitleW(0.8);
  //os->SetTitleH(0.08);
  os->SetTitleH(0.11);
  os->SetTitleX(0.1);
  os->SetPadBottomMargin(0.18);
  os->SetTextSize(0.065);
  os->cd();
  fitfunc = new TF1("gaus", "gaus");

  return ;
}

ACCMonDraw::~ACCMonDraw()
{
  for (int i = 0; i < 2; i++)
    {
      delete title[i];
    }
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  delete message[i][j];
	}
    }
  delete northsouth[0];
  delete northsouth[1];
  delete brind;
  delete ln1;
  delete fitfunc;
  return ;
}

int ACCMonDraw::Init()
{
  memset(dead, 0, sizeof(dead));
  return 0;
}

int ACCMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  gStyle->SetTitleFontSize(0.08);
  gStyle->SetTitleFillColor(0);
  if (!strcmp(name, "ACCMon1"))
    {
      // changed the Pad Size (12/03/2007 K.Miki)
      //TC[0] = new TCanvas(name, "ACCMon Granule Overview", -1, 0, 1 * xsize / 2, 9 * ysize / 20);
      TC[0] = new TCanvas(name, "ACCMon Overview", -1, 0, 17 * xsize / 20, 17 * ysize / 20);
      gSystem->ProcessEvents();
      TC[0]->cd();

      Pad[0] = new TPad("pad1", "acc_livehistpad", 0.42, 0.60, 0.99, 0.89, 0, 4, -1);
      Pad[1] = new TPad("pad2", "acc_adcsumpad" , 0.02, 0.30, 0.4, 0.59, 0, 4, -1);
      Pad[2] = new TPad("pad3", "adc_averagepad", 0.42, 0.30, 0.99, 0.59, 0, 4, -1);
      Pad[3] = new TPad("pad4", "acc_tacsumpadpad", 0.02, 0.00, 0.4, 0.29, 0, 4, -1);
      Pad[4] = new TPad("pad5", "tac_averagepad", 0.42, 0.00, 0.99, 0.29, 0, 4, -1);

      msg[0] = new TPad("msg0", "msg0" , 0.02, 0.825, 0.4, 0.895, 0, 4, -1);
      msg[1] = new TPad("msg1", "msg1" , 0.02, 0.75, 0.4, 0.82, 0, 4, -1);
      msg[2] = new TPad("msg2", "msg2" , 0.02, 0.675, 0.4, 0.745, 0, 4, -1);
      msg[3] = new TPad("msg3", "msg3" , 0.02, 0.60, 0.4, 0.67, 0, 4, -1);

      msg[0]->SetFillColor(3);
      msg[1]->SetFillColor(3);
      msg[2]->SetFillColor(3);
      msg[3]->SetFillColor(3);
      msg[0]->SetFillStyle(3001);
      msg[1]->SetFillStyle(3001);
      msg[2]->SetFillStyle(3001);
      msg[3]->SetFillStyle(3001);
      msg[0]->Draw();
      msg[1]->Draw();
      msg[2]->Draw();
      msg[3]->Draw();

      Pad[0]->Draw();
      Pad[1]->Draw();
      Pad[2]->Draw();
      Pad[3]->Draw();
      Pad[4]->Draw();
      Pad[0]->SetGridx();
      Pad[2]->SetGridx();
      Pad[4]->SetGridx();

      transparent[0] = new TPad("transparent1", "this does not show", 0, 0, 1, 1, 0, 0);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
      TC[0]->SetEditable(0);
    }
  else if (!strcmp(name, "ACCMon2"))
    {

      // Modified the Pad size to see detail (12/03/2007 K.Miki)
      //TC[1] = new TCanvas(name, "ACCMon Granule Overview", -1,  1 * ysize / 2, 1 * xsize / 2, 9 * ysize / 20);
      TC[1] = new TCanvas(name, "ACCMon TAC", -1,  0, 17 * xsize / 20, 17 * ysize / 20);
      gSystem->ProcessEvents();
      TC[1]->cd();

      Pad[5] = new TPad("pad6", "acc_ADC-TAC_plot(North)", 0.02, 0.46, 0.48, 0.89, 0, 4, -1);
      Pad[6] = new TPad("pad7", "acc_ADC-TAC_plot(South)" , 0.52, 0.46, 0.98, 0.89, 0, 4, -1);
      Pad[7] = new TPad("pad8", "acc_tacsumpadpad(North)", 0.02, 0.00, 0.48, 0.44, 0, 4, -1);
      Pad[8] = new TPad("pad9", "acc_tacsumpadpad(South)", 0.52, 0.00, 0.98, 0.44, 0, 4, -1);

      Pad[5]->Draw();
      Pad[6]->Draw();
      Pad[7]->Draw();
      Pad[8]->Draw();
      Pad[5]->SetGridx(0);
      Pad[6]->SetGridx(0);

      transparent[1] = new TPad("transparent2", "this does not show", 0, 0, 1, 1, 0, 0);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
      TC[1]->SetEditable(0);
    }
  else
    {
      cout << PHWHERE << " Unknown Canvas: " << name << endl;
    }
  return 0;
}


int ACCMonDraw::DrawACCADCTDC(const char *what = "ACCMon1")
{
  if (! gROOT->FindObject("ACCMon1"))
    {
      MakeCanvas(what);
    }
  OnlMonClient *cl = OnlMonClient::instance();
  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  // Event time & Run number----------------------------------------------
  transparent[0]->cd();
  ostringstream otext;
  time_t evttime = cl->EventTime("CURRENT");
  otext << "Run #: " << cl->RunNumber() << " Date: " << ctime(&evttime);
  title[0]->Draw();
  title[1]->DrawText(0, 0.9, otext.str().c_str());
  int data_stat = CheckData(what, 0);
  if (data_stat != 1)
    {
      return data_stat;
    };
  CheckADC();
  CheckTAC();
  TC[0]->Update();

  return 0;
}

int ACCMonDraw::DrawACCTDCped(const char *what = "ACCMon2")
{

  if (! gROOT->FindObject("ACCMon2"))
    {
      MakeCanvas(what);
    }
  OnlMonClient *cl = OnlMonClient::instance();
  TC[1]->SetEditable(1);
  TC[1]->Clear("D");

  //this is not
  transparent[1]->cd();
  ostringstream otext;
  time_t evttime = cl->EventTime("CURRENT");
  otext << "Run #: " << cl->RunNumber() << " Date: " << ctime(&evttime);
  title[0]->Draw();
  title[1]->DrawText(0, 0.9, otext.str().c_str());
  int data_stat = CheckData(what, 1);

  if (data_stat != 1)
    {
      return data_stat;
    };
  CheckADCTAC();
  CheckTAC2();


  TC[1]->Update();

  return 0;
}

int ACCMonDraw::Draw(const char *what)
{
  gStyle->SetOptStat(0);

  os->cd();
  int iret = 0;
  if (!strcmp(what, "ALL") || !strcmp(what, "ACCMon1") || !strcmp(what, "ALLADCTDC") )
    {
      iret = DrawACCADCTDC("ACCMon1");
    }
  if (iret)
    {
      return iret;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "ACCMon2") || !strcmp(what, "TDCpedestal") )
    {
      iret = DrawACCTDCped("ACCMon2");
    }

  return iret;
}

int ACCMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
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

int ACCMonDraw::DrawDeadServer(TPad *transparent)
{
  //cout<<"star DrawDeadServer"<<endl;
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "AEROGEL MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}

// for Pad 1
int ACCMonDraw::SetHistStyle(TH1 *hist)
{
  hist->SetTitleSize(0.08, "X");
  hist->SetTitleSize(0.10, "Y");
  hist->SetTitleOffset(0.5, "Y");
  hist->SetFillStyle(3003);
  hist->SetFillColor(4);
  hist->SetLabelSize(0.10, "X");
  hist->SetLabelSize(0.09, "Y");

  return 0;
}

// for Pad 2
int ACCMonDraw::SetHistStyle2(TH1 *hist)
{
  hist->SetTitleSize(0.08, "X");
  hist->SetTitleSize(0.08, "Y");
  hist->SetTitleOffset(0.63, "X");
  hist->SetTitleOffset(0.65, "Y");
  hist->SetFillStyle(3003);
  hist->SetFillColor(4);
  hist->SetLabelSize(0.06, "X");
  hist->SetLabelSize(0.05, "Y");

  return 0;
}

int ACCMonDraw::CheckData(const char* what, const int tc_index)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *acc_par = cl->getHisto("acc_par");
  //server dead message---------------------------------------------------
  if (!acc_par)
    {
      if (TC[tc_index])
	{
	  TC[tc_index]->cd();
	  TC[tc_index]->SetEditable(1);
	  TC[tc_index]->Clear("D");
	  DrawDeadServer(transparent[tc_index]);
	  TC[tc_index]->Update();
	  TC[tc_index]->SetEditable(0);
	}
      return -1;
    }
  else
    {
      run = (int)acc_par->GetBinContent(7);
      evt = (int)acc_par->GetBinContent(1);
      accevt = (int)acc_par->GetBinContent(6);
    }
  if(tc_index == 0)
    {
      msg[0]->cd();
      if (accevt == 0)
	{
	  msg[0]->SetFillColor(2);
	  message[0][0]->SetTextSize(0.5);
	  message[0][0]->SetText(0.01, 0.5, "No Aerogel packets");
	  message[0][0]->Draw();
	  return 0;
	}

      if (acc_par->GetBinContent(2) / acc_par->GetBinContent(1) >
	  PERMIT_ACC_PACKET_ERROR)
	{
	  msg[0]->SetFillColor(2);
	  message[0][1]->SetText(0.01, 0.5, "Illeagal Packet");
	  message[0][1]->SetTextSize(0.5);
	  message[0][1]->Draw();
	  message[0][2]->SetTextSize(0.5);
	  message[0][2]->SetText(0.01, 0.5, "See a manual");
	  message[0][2]->Draw();
	}
      else if ((acc_par->GetBinContent(4) + acc_par->GetBinContent(3)) / acc_par->GetBinContent(1)
	       > PERMIT_ACC_PACKET_ERROR * 2
	       || acc_par->GetBinContent(4) / acc_par->GetBinContent(1) > PERMIT_ACC_PACKET_ERROR )
	{
	  msg[0]->SetFillColor(3);
	  message[0][1]->SetText(0.01, 0.5, "Packet Error");
	  message[0][1]->SetTextSize(0.5);
	  message[0][1]->Draw();
	  message[0][2]->SetText(0.01, 0.05, "Reset or reconfigure Aerogel-FEE");
	  message[0][2]->SetTextSize(0.3);
	  message[0][2]->Draw();
	}
      else
	{
	  message[0][0]->SetText(0.01, 0.05, "Raw Data ..OK");
	  message[0][0]->Draw();
	}
    }
  return 1;
}

int ACCMonDraw::CheckADC()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *acc_livehist = cl->getHisto("acc_livehist");
  TH1 *acc_Nadcsum = cl->getHisto("acc_Nadcsum");
  TH1 *acc_Sadcsum = cl->getHisto("acc_Sadcsum");
  TH1 *adc_average = cl->getHisto("adc_average");

  acc_livehist->SetXTitle("PMT #");
  acc_livehist->SetYTitle("Hit Events ");
  adc_average->SetXTitle("PMT #");
  adc_average->SetYTitle("Number of p.e ");
  acc_Nadcsum->SetXTitle("Number of p.e");
  acc_Sadcsum->SetXTitle("Number of p.e");

  SetHistStyle(acc_livehist);
  SetHistStyle(adc_average);
  SetHistStyle(acc_Nadcsum);
  SetHistStyle(acc_Sadcsum);

  acc_Nadcsum->SetLineColor(2);
  acc_Sadcsum->SetLineColor(4);
  acc_Nadcsum->SetFillColor(2);
  acc_Sadcsum->SetFillColor(4);

  if (brind)
    {
      delete brind;
    }
  brind = new TH1F("brind", "", ACC_ALLCHANNEL, 0, ACC_ALLCHANNEL);
  brind->SetLineColor(3);
  brind->SetFillColor(3);

  int deadcounts = 0;
  int deadcountsN = 0;
  int deadcountsS = 0;
  int lowgainsN = 0;
  int lowgainsS = 0;
  int brindflag = 0;
  int brindflagN = 0;
  int brindflagS = 0;
  float adc_av = 0;
  int hitcounts = (int)acc_livehist->Integral();
  ostringstream adcdead;

  for (int ich = 0; ich < ACC_ALLCHANNEL; ich++)
    {
      int hits = (int)acc_livehist->GetBinContent(ich + 1);
      int npe = (int)adc_average->GetBinContent(ich + 1);
      if (hits == -1)
	{
	  dead[ich] = ACCPMT_DEAD;
	}
      else if (hits > 0)
	{
	  dead[ich] = ACCPMT_ALIVE;
	}

      if (dead[ich] == ACCPMT_ALIVE)
	{
	  if (hits == 0)
	    {
	      adc_av = 0;
	      adcdead << ich << ",";
	      if (ich < ACC_NCHANNEL)
		{
		  deadcountsN++;
		}
	      else if (ich >= ACC_NCHANNEL)
		{
		  deadcountsS++;
		}
	    }
	  else
	    {
	      adc_av = ((float)npe / (float)hits);
	    }
	  adc_average->SetBinContent(ich + 1, adc_av);

	  if (adc_av < ACC_PMT_NOHIT)
	    {
	      if (ich < ACC_NCHANNEL)
		{
		  lowgainsN++;
		}
	      else if (ich >= ACC_NCHANNEL)
		{
		  lowgainsS++;
		}
	      if (hits < hitcounts / (ACC_ALLCHANNEL * ACC_HITS_THRESH))
		{
		  adcdead << ich << ",";
		  if (ich < ACC_NCHANNEL)
		    {
		      // Mask the bad channel from disable HV module
		      if(ich != 137 && ich != 141 && ich != 38)
			deadcountsN++;
		    }
		  else if (ich >= ACC_NCHANNEL)
		    {
		      if(ich != 287)
			deadcountsS++;
		    }
		}
	    }
	}
      else if (dead[ich] == ACCPMT_DEAD)
	{
	  adc_average->SetBinContent(ich + 1, 0);
	  brind->SetBinContent(ich + 1, evt * 10);
	  if (ich < ACC_NCHANNEL)
	    {
	      brindflagN++;
	    }
	  else if (ich >= ACC_NCHANNEL)
	    {
	      brindflagS++;
	    }
	}
    }
  deadcounts = deadcountsN + deadcountsS;
  brindflag = brindflagN + brindflagS;

  if (deadcounts == ACC_ALLCHANNEL - brindflag )
    {
      msg[1]->cd();
      msg[1]->SetFillColor(3);
      message[1][0]->SetText(0.05, 0.05, "No ADC hit");
      message[1][0]->SetTextSize(1);
      msg[2]->cd();
      message[2][1]->SetTextSize(0.6);
      message[2][1]->SetText(0.01, 0.5, "Check High Voltage");
      message[2][1]->Draw();
      message[2][2]->SetTextSize(0.6);
      message[2][2]->SetText(0.01, 0.05, "Check Low Voltage");
      message[2][2]->Draw();
    }
  else
    {
      Pad[0]->cd();
      gPad->SetBottomMargin(0.17);
      if (acc_livehist->Integral())
	{
	  Pad[0]->SetLogy();
	}
      acc_livehist->DrawCopy();
      acc_livehist->SetAxisRange(0, 160, "X");

      if (brindflag != 0)
	{
	  brind->DrawCopy("same");
	}
      northsouth[0]->DrawTextNDC(0.2, 0.80, "North");
      northsouth[1]->DrawTextNDC(0.7, 0.80, "South");

      Pad[1]->cd();
      gPad->SetBottomMargin(0.17);
      if (acc_Nadcsum->Integral())
	{
	  Pad[1]->SetLogy();
	}
      acc_Nadcsum->DrawCopy();
      acc_Sadcsum->DrawCopy("same");
      northsouth[0]->DrawTextNDC(0.7, 0.80, "North");
      northsouth[1]->DrawTextNDC(0.7, 0.70, "South");

      Pad[2]->cd();
      gPad->SetBottomMargin(0.17);
      adc_average->DrawCopy();
      if (brindflag != 0)
	{
	  brind->DrawCopy("same");
	}
      northsouth[0]->DrawTextNDC(0.2, 0.80, "North");
      northsouth[1]->DrawTextNDC(0.7, 0.80, "South");
      ln1->Draw();

      if (deadcounts != 0)
	{
	  float ave = hitcounts / (ACC_ALLCHANNEL - brindflag);
	  msg[1]->SetFillColor(2);
	  if ( ave < 100)
	    {
	      msg[1]->cd();
	      message[1][0]->SetText(0.01, 0.05, "Need More Events");
	      message[1][0]->Draw();
	    }
	  else if ( (deadcountsN > 39 && deadcountsN <= ACC_NCHANNEL - brindflagN) ||
		    (deadcountsS > 39 && deadcountsS <= ACC_NCHANNEL - brindflagS) )
	    {
	      msg[2]->SetFillColor(2);
	      msg[2]->cd();
	      message[2][1]->SetTextSize(0.6);
	      message[2][2]->SetTextSize(0.6);
	      message[2][1]->SetText(0.01, 0.5, "Check High Voltage");
	      message[2][1]->Draw();
	      message[2][2]->SetText(0.01, 0.05, "Check Low Voltage");
	      message[2][2]->Draw();

	      message[1][1]->SetTextSize(0.5);
	      message[1][2]->SetTextSize(0.5);
	      if (deadcountsN > 32 && deadcountsN < ACC_NCHANNEL - brindflagN)
		{
		  msg[1]->cd();
		  message[1][1]->SetTextSize(0.3);
		  message[1][1]->SetText(0.01, 0.5, "Too much dead PMTs in North");
		  message[1][1]->Draw();
		}
	      else if (deadcountsN == ACC_NCHANNEL - brindflagN )
		{
		  msg[1]->cd();
		  message[1][1]->SetTextSize(0.6);
		  message[1][1]->SetText(0.01, 0.5, "No ADC hit in North");
		  message[1][1]->Draw();
		}

	      if (deadcountsS > 32 && deadcountsS < ACC_NCHANNEL - brindflagS)
		{
		  msg[1]->cd();
		  message[1][1]->SetTextSize(0.5);
		  message[1][2]->SetText(0.01, 0.05, "Too much dead PMTs in South");
		  message[1][2]->Draw();
		}
	      else if (deadcountsS == ACC_NCHANNEL - brindflagS)
		{
		  msg[1]->cd();
		  message[1][1]->SetTextSize(0.6);
		  message[1][2]->SetText(0.01, 0.05, "No ADC hit in South");
		  message[1][2]->Draw();
		}
	    }
	  else if ( (lowgainsN > 39 && lowgainsN <= ACC_NCHANNEL - brindflagN) ||
		    (lowgainsS > 39 && lowgainsS <= ACC_NCHANNEL - brindflagS) )
	    {
	      msg[2]->SetFillColor(2);
	      msg[2]->cd();
	      message[2][1]->SetTextSize(0.6);
	      message[2][2]->SetTextSize(0.6);
	      message[2][1]->SetText(0.01, 0.5, "Check High Voltage");
	      message[2][1]->Draw();
	      message[2][2]->SetText(0.01, 0.05, "Check Low Voltage");
	      message[2][2]->Draw();

	      message[1][1]->SetTextSize(0.5);
	      message[1][2]->SetTextSize(0.5);
	      if (lowgainsN > 32 && lowgainsN < ACC_NCHANNEL - brindflagN)
		{
		  msg[1]->cd();
		  message[1][1]->SetTextSize(0.6);
		  message[1][1]->SetText(0.01, 0.5, "Feed North FEM");
		  message[1][1]->Draw();
		}
	      if (lowgainsS > 32 && lowgainsS < ACC_NCHANNEL - brindflagS)
		{
		  msg[1]->cd();
		  message[1][1]->SetTextSize(0.6);
		  message[1][2]->SetText(0.01, 0.05, "Feed South FEM");
		  message[1][2]->Draw();
		}
	    }
	  else
	    {
	      msg[2]->SetFillColor(2);
	      msg[1]->cd();
	      message[1][2]->SetTextSize(0.4);
	      message[1][2]->SetText(0.01, 0.5, "Bad ADC channels");
	      message[1][2]->Draw();
	      message[1][1]->SetTextSize(0.6);
	      message[1][1]->SetText(0.01, 0.05, adcdead.str().c_str());
	      message[1][1]->Draw();
	      msg[2]->cd();
	      message[2][3]->SetTextSize(0.7);
	      message[2][3]->SetText(0.01, 0.05, "Check High Voltage");
	      message[2][3]->Draw();
	    }
	}
      else
	{
	  //acc spike finder--------------------------------
	  float mid = hitcounts / ACC_ALLCHANNEL;
	  if (mid <= 0)
	    {
	      mid = 1;
	    }
	  ostringstream noisy;
	  double limit = sqrt(mid) * ACC_SPIKE_DEF;
	  int nspike = 0;
	  for (int ich = 0; ich < ACC_ALLCHANNEL; ich++)
	    {
	      // If hit count exceeds error * 50, it is a spike.
	      if (limit <
		  fabs(acc_livehist->GetBinContent(ich + 1) - mid) &&
		  dead[ich] == ACCPMT_ALIVE && known_noisy.find(ich) == known_noisy.end())
		{
		  spikech[nspike] = ich;
		  noisy << ich << ",";
		  nspike++;
		}
	    }
	  if (nspike > 0)
	    {
	      msg[1]->SetFillColor(3);
	      msg[1]->cd();
	      message[1][1]->SetText(0.01, 0.5, "Noisy PMT #");
	      message[1][1]->SetTextSize(0.4);
	      message[1][1]->Draw();
	      message[1][2]->SetText(0.05, 0.05, noisy.str().c_str());
	      message[1][2]->SetTextSize(0.6);
	      message[1][2]->Draw();
	    }
	  msg[2]->cd();
	  message[2][0]->SetText(0.01, 0.05, "HV & LV  ..OK");
	  message[2][0]->Draw();
	  if (nspike == 0)
	    {
	      msg[1]->cd();
	      message[1][0]->SetText(0.01, 0.05, "ADC  ..OK");
	      message[1][0]->Draw();
	    }
	}
    }

  return 0;
}

int ACCMonDraw::CheckTAC()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *acc_taclivehist = cl->getHisto("acc_taclivehist");
  TH1 *acc_Ntacsum = cl->getHisto("acc_Ntacsum");
  TH1 *acc_Stacsum = cl->getHisto("acc_Stacsum");
  TH1 *tac_average = cl->getHisto("tac_average");

  acc_taclivehist->SetXTitle("PMT #");
  acc_taclivehist->SetYTitle("Hit Events");
  acc_Ntacsum->SetXTitle("TAC Channels");
  acc_Stacsum->SetXTitle("TAC Channels");
  tac_average->SetXTitle("PMT #");
  tac_average->SetYTitle("TAC channel ");

  SetHistStyle(acc_taclivehist);
  SetHistStyle(acc_Ntacsum);
  SetHistStyle(acc_Stacsum);
  SetHistStyle(tac_average);

  acc_Ntacsum->SetLineColor(2);
  acc_Stacsum->SetLineColor(4);
  acc_Ntacsum->SetFillColor(2);
  acc_Stacsum->SetFillColor(4);


  for (int ich = 0; ich < ACC_ALLCHANNEL; ich++)
    {
      int hits = (int)acc_taclivehist->GetBinContent(ich + 1);
      int time = (int)tac_average->GetBinContent(ich + 1);
      if (hits == 0)
	{
	  tac_average->SetBinContent(ich + 1, 0);
	}
      else
	{
	  tac_average->SetBinContent(ich + 1, time / hits);
	}
    }

  Pad[3]->cd();
  gPad->SetBottomMargin(0.17);
  if (acc_Ntacsum->Integral())
    {
      Pad[3]->SetLogy();
    }
  acc_Ntacsum->DrawCopy();
  acc_Stacsum->DrawCopy("same");
  northsouth[0]->DrawTextNDC(0.7, 0.80, "North");
  northsouth[1]->DrawTextNDC(0.7, 0.70, "South");

  Pad[4]->cd();
  gPad->SetBottomMargin(0.17);
  tac_average->DrawCopy();
  brind->DrawCopy("same");
  northsouth[0]->DrawTextNDC(0.2, 0.80, "North");
  northsouth[1]->DrawTextNDC(0.7, 0.80, "South");

  message[3][0]->SetText(0.01, 0.05, "TAC  ..OK");
  msg[3]->cd();
  message[3][0]->Draw();


  return 0;
}

int ACCMonDraw::CheckTAC2()
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *acc_Ntacsum_board[5];
  TH1 *acc_Stacsum_board[5];


  //  int badflagN=0,badflagS=0;
  for(int k = 0; k < 5; k++)
    {
      char temp_name[30];

      Pad[7]->cd();
      gPad->SetBottomMargin(0.12);
      sprintf(temp_name, "acc_Ntacsum_board_%d", k);
      acc_Ntacsum_board[k] = cl->getHisto(temp_name);
      acc_Ntacsum_board[k]->SetXTitle("North TAC Channels");
      SetHistStyle2(acc_Ntacsum_board[k]);
      acc_Ntacsum_board[k]->SetLineColor(k + 1);
      acc_Ntacsum_board[k]->SetFillColor(k + 1);
      acc_Ntacsum_board[k]->Fit(fitfunc, "QM0", "", acc_Ntacsum_board[k]->GetBinCenter(acc_Ntacsum_board[k]->GetMaximumBin()) - 20, acc_Ntacsum_board[k]->GetBinCenter(acc_Ntacsum_board[k]->GetMaximumBin()) + 20);

      Pad[8]->cd();
      gPad->SetBottomMargin(0.12);
      sprintf(temp_name, "acc_Stacsum_board_%d", k);
      acc_Stacsum_board[k] = cl->getHisto(temp_name);
      acc_Stacsum_board[k]->SetXTitle("South TAC Channels");
      SetHistStyle2(acc_Stacsum_board[k]);
      acc_Stacsum_board[k]->SetLineColor(k + 1);
      acc_Stacsum_board[k]->SetFillColor(k + 1);
      acc_Stacsum_board[k]->Fit(fitfunc, "QM0", "", acc_Stacsum_board[k]->GetBinCenter(acc_Stacsum_board[k]->GetMaximumBin()) - 20, acc_Stacsum_board[k]->GetBinCenter(acc_Stacsum_board[k]->GetMaximumBin()) + 20);
    }
  Pad[7]->cd();
  if (acc_Ntacsum_board[0]->Integral())
    {
      Pad[7]->SetLogy();
    }
  acc_Ntacsum_board[0]->DrawCopy();
  for(int k = 1; k < 5; k++)
    {
      acc_Ntacsum_board[k]->DrawCopy("same");
    }

  Pad[8]->cd();
  if (acc_Stacsum_board[0]->Integral())
    {
      Pad[8]->SetLogy();
    }
  acc_Stacsum_board[0]->DrawCopy();
  for(int k = 1; k < 5; k++)
    {
      acc_Stacsum_board[k]->DrawCopy("same");
    }


  return 0;
}

void
ACCMonDraw::AddKnownNoisy(const int channel)
{
  known_noisy.insert(channel);
  return;
}

int ACCMonDraw::CheckADCTAC()
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *acc_adc_tac_N = cl->getHisto("acc_adc_tac_N");
  SetHistStyle2(acc_adc_tac_N);
  TH1 *acc_adc_tac_S = cl->getHisto("acc_adc_tac_S");
  SetHistStyle2(acc_adc_tac_S);


  Pad[5]->cd();
  gPad->SetBottomMargin(0.12);
  acc_adc_tac_N->SetYTitle("North TAC");
  acc_adc_tac_N->SetXTitle("North ADC");
  acc_adc_tac_N->DrawCopy("colz");
  Pad[6]->cd();
  gPad->SetBottomMargin(0.12);
  acc_adc_tac_S->SetYTitle("South TAC");
  acc_adc_tac_S->SetXTitle("South ADC");
  acc_adc_tac_S->DrawCopy("colz");
  Pad[5]->Update();
  Pad[6]->Update();
  return 0;
}

int ACCMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();
  string ofile = cl->htmlRegisterPage(*this, "AccMonitor1", "1", "png");
  cl->CanvasToPng(TC[0], ofile);
  ofile = cl->htmlRegisterPage(*this, "AccMonitor2", "2", "png");
  cl->CanvasToPng(TC[1], ofile);
  cl->SaveLogFile(*this);
  return 0;
}
