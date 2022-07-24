// Online Monitoring includes
#include <Gl1MonDraw.h>
#include <OnlMonClient.h>
#include <OnlMonTrigger.h>
#include <phool.h>

// ROOT includes
#include <TBox.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

// System includes
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

Gl1MonDraw::Gl1MonDraw(): OnlMonDraw("GL1MON")
{

  TStyle *oldStyle = gStyle;
  gl1Style = new TStyle("gl1Style", "gl1Style");
  gl1Style->SetFrameBorderMode(0);
  gl1Style->SetCanvasColor(0);
  gl1Style->SetPadBorderMode(0);
  gl1Style->SetCanvasBorderMode(0);
  oldStyle->cd();

  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream hname;
      hname.fill('0');
  
  for (int i = 0;i < 32;i++)
    {
      trigname1[i] = new TText();
      unsigned int ishi = 1 << i;
      hname.str("");
      hname << hex << "GL1_CrossCtr_0x" << setw(8) << ishi << dec;
      cl->registerHisto(hname.str(), "GL1MON");
    }
  GL1title = new TText(0.1, 0.95, "");
  live = new TLine(0, 0, 32, 32);
  legend9 = new TText(8.0, 0.30, "Live Fraction by Trigger");
  legend10 = new TText(8.0, 0.15, "");
  one = new TLine(0, 1.0, 32, 1.0);
  legend7 = new TText(8.0, 0.30, "Scaled Scalers: Scaler events / Data Events");
  legend8 = new TText(8.0, 0.15, "");


  GL1Rejtitle = new TText(0.1, 0.95, "");
  legend4 = new TText(20.2, 3.65, "");
  legend3 = new TText(3, 90, "GL1 Trigger Bandwidth (Percent)");
  legend15 = new TText(2, 79, "");


  Crosstitle = new TText(0.1, 0.95, "");
  legend11 = new TText(8.0, 9.0, "BBCLL1 Bunch Crossing Pattern");
  legend12 = new TText(8.0, 7.0, "BBCLL1 noVertexCut Bunch Crossing Pattern");
  legend13 = new TText(8.0, 5.0, "ZDC Bunch Crossing Pattern");



  Ratiotitle = new TText(0.1, 0.95, "");
  legend18 = new TText(8.0, 4.0, "");
  legend19 = new TText(8.0, 4.0, "");
  legend20 = new TText(8.0, 4.0, "");

  return ;
}

Gl1MonDraw::~Gl1MonDraw()
{

  delete gl1Style;

  for (int i = 0;i < 32;i++)
    {
      delete trigname1[i];
    }
  delete GL1title;
  delete live;
  delete legend9;
  delete legend10;
  delete one;
  delete legend7;
  delete legend8;


  delete GL1Rejtitle;
  delete legend4;
  delete legend3;
  delete legend15;


  delete Crosstitle;
  delete legend11;
  delete legend12;
  delete legend13;


  delete Ratiotitle;
  delete legend18;
  delete legend19;
  delete legend20;

  return;
}

int
Gl1MonDraw::MakeCanvases(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  TStyle *oldStyle = gStyle;

  gl1Style->cd();

  if (!strcmp(name, "Gl1_c1"))
    {
      gl1_c1 = new TCanvas("Gl1_c1", "GL1 Scalers", -1, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      gl1_c1->Range(0, 0, 1, 1);
      gl1_c1->SetBorderSize(2);

      gl1_c1->cd();

      plotpad5 = new TPad("plotpad5", "", 0.01, 0.63, 0.99, 0.95,0);
      plotpad5->Draw();
      plotpad6 = new TPad("plotpad6", "", 0.01, 0.31, 0.99, 0.63,0);
      plotpad6->Draw();
      plotpad2 = new TPad("plotpad2", "", 0.01, 0.01, 0.99, 0.31,0);
      plotpad2->Draw();

      transparent1 = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent1->SetFillStyle(4000);
      transparent1->Draw();
      transparent1->SetEditable(kFALSE);

      if (verbosity)
        {
          cout << "Create objects displayed in gl1_c1" << endl;
        }


    }


  if (!strcmp(name, "Gl1_c2"))
    {
      gl1_c2 = new TCanvas("Gl1_c2", "GL1 Crossing Counter", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      gl1_c1->Range(0, 0, 1, 1);
      gl1_c1->SetBorderSize(2);

      gl1_c2->cd();


      plotpad11 = new TPad("plotpad11", "", 0.01, 0.01, 0.99, 0.31,0);
      plotpad11->Draw();
      plotpad12 = new TPad("plotpad12", "", 0.01, 0.32, 0.99, 0.62,0);
      plotpad12->Draw();
      plotpad13 = new TPad("plotpad13", "", 0.01, 0.63, 0.99, 0.93,0);
      plotpad13->Draw();

      transparent2 = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
      transparent2->SetFillStyle(4000);
      transparent2->Draw();
      transparent2->SetEditable(kFALSE);

      if (verbosity)
        {
          cout << "Create objects displayed in gl1_c3" << endl;
        }

    }

  oldStyle->cd();

  return 0;
}

int
Gl1MonDraw::Draw(const char *what)
{
  int iret = 0;
  string drawwhat = what;
  int drawall = 0;
  if (drawwhat.find("ALL") != string::npos)
    {
      drawall = 1;
    }
  if (drawwhat.find("bcross") != string::npos || drawall)
    {
      ostringstream numfind;
      for (int i=0; i<4;i++)
	{
	  numfind.str("");
	  numfind << i;
	  if (drawwhat.find(numfind.str()) != string::npos || drawall)
	    {
              iret += DrawBCross(i);
	    }
	}

    }
  if (strcmp(what,"REJECTION") == 0 || drawall)
	{
	  iret += DrawRejection();
	}
  return iret;
}

int
Gl1MonDraw::DrawBCross(const int iwhich)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TText title;
  title.SetNDC();
  title.SetTextAlign(23);
  int runnumber = cl->RunNumber();
  MakeBCCanvases(iwhich);
  TH1 *bcross[8];
  ostringstream hname;
  hname.fill('0');
  Gl1_bc_canv[iwhich]->cd();
  for (int i = 0; i < 8; i++)
    {
      hname .str("");
      unsigned int ishift = 1 << (i + iwhich * 8);
      hname << hex << "GL1_CrossCtr_0x" << setw(8) << ishift << dec;
      bcross[i] = cl->getHisto(hname.str());
      if (!bcross[i])
        {
          cout << "could not locate " << hname.str() << endl;
          DrawDeadServer();
          return -1;
        }
      bc_pad[iwhich][i]->cd();
      if (bcross[i]->Integral() > 0)
        {
	  bcross[i]->SetMinimum(0.5);
          bc_pad[iwhich][i]->SetLogy(1);
        }
      else
        {
          bc_pad[iwhich][i]->SetLogy(0);
        }
      bcross[i]->SetStats(0);
      string htitle = bcross[i]->GetTitle();
      bcross[i]->SetTitle("");
      bcross[i]->SetXTitle("Bunch Crossing");
      bcross[i]->GetXaxis()->SetLabelSize(0.05);
      bcross[i]->DrawCopy();
      bcross[i]->SetTitle(htitle.c_str());
      title.SetTextColor(4);
      title.SetTextSize(0.07);
      title.DrawText(0.5, 0.99, htitle.c_str());
    }
  unsigned int ishiftbegin = 1 << (iwhich * 8);
  unsigned int ishiftend = 1 << (7 + iwhich * 8);
  time_t evttime = cl->EventTime("CURRENT");

  bc_transparent[iwhich]->cd();
  bc_transparent[iwhich]->Clear();
  title.SetTextColor(1);
  title.SetTextSize(0.03);
  hname .str("");
  hname << "Run: " << runnumber << ", Date: " << ctime(&evttime);
  title.DrawText(0.5, 0.995, hname.str().c_str());
  hname.str("");

  hname << "Bunch Crossing Pattern for Trigger bits 0x" << hex << setw(8)
	<< ishiftbegin << " to 0x" << setw(8) << ishiftend << dec;
  title.DrawText(0.5, 0.96, hname.str().c_str());

  Gl1_bc_canv[iwhich]->Update();
  return 0;
}

int
Gl1MonDraw::MakeBCCanvases(const int iwhich)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  ostringstream canvname, canvtitle;
  canvname << "Gl1_bc_canv" << iwhich;
  canvtitle << "GL1 BunchCrossing " << iwhich;
  if (! gROOT->FindObject(canvname.str().c_str()))
    {
      Gl1_bc_canv[iwhich] = new TCanvas(canvname.str().c_str(), canvtitle.str().c_str(), -1, 0, xsize, ysize);
      ostringstream tpname;
      float ymax = 0.93;
      for (int i = 0; i < 4; i++)
        {
          tpname.str("");
          tpname << "bcpad" << iwhich << "_" << i;
          float ymin = ymax - 0.23;
          bc_pad[iwhich][i] =  new TPad(tpname.str().c_str(), "", 0.01, ymin, 0.495, ymax, 0);
          tpname.str("");
          tpname << "bcpad" << i + 4;
          bc_pad[iwhich][i+4] = new TPad(tpname.str().c_str(), "", 0.505, ymin, 0.99, ymax, 0);
          ymax -= 0.23;
          bc_pad[iwhich][i]->Draw();
          bc_pad[iwhich][i+4]->Draw();
        }
      tpname.str("");
      tpname << "bc_transparent" << iwhich;
      bc_transparent[iwhich] = new TPad(tpname.str().c_str(), "", 0, 0, 1, 1);
      bc_transparent[iwhich]->SetFillStyle(4000);
      bc_transparent[iwhich]->Draw();
    }
  return 0;
}

int
Gl1MonDraw::DrawRejection()
    {
  OnlMonClient *cl = OnlMonClient::instance();
  int runnumber = cl->RunNumber();

  if (! gROOT->FindObject("Gl1_c1"))
    {
      MakeCanvases("Gl1_c1");
    }
  if (! gROOT->FindObject("Gl1_c2"))
    {
      MakeCanvases("Gl1_c2");
    }


  TH1 *Gl1_RawHist = cl->getHisto("Gl1_RawHist");
  if (!Gl1_RawHist)
    {
      cout << "Gl1_RawHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_LiveHist = cl->getHisto("Gl1_LiveHist");
  if (!Gl1_LiveHist)
    {
      cout << "Gl1_LiveHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ScaledHist = cl->getHisto("Gl1_ScaledHist");
  if (!Gl1_ScaledHist)
    {
      cout << "Gl1_ScaledHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ScaledHist_BBCLL1 = cl->getHisto("Gl1_ScaledHist_BBCLL1");
  if (!Gl1_ScaledHist_BBCLL1)
    {
      cout << "Gl1_ScaledHist_BBCLL1 does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ScaledHist_ZDC = cl->getHisto("Gl1_ScaledHist_ZDC");
  if (!Gl1_ScaledHist_ZDC)
    {
      cout << "Gl1_ScaledHist_ZDC does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ScalerRawHist = cl->getHisto("Gl1_ScalerRawHist");
  if (!Gl1_ScalerRawHist)
    {
      cout << "Gl1_ScalerRawHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ScalerLiveHist = cl->getHisto("Gl1_ScalerLiveHist");
  if (!Gl1_ScalerLiveHist)
    {
      cout << "Gl1_ScalerLiveHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ScalerScaledHist = cl->getHisto("Gl1_ScalerScaledHist");
  if (!Gl1_ScalerScaledHist)
    {
      cout << "Gl1_ScalerScaledHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_RejectHist = cl->getHisto("Gl1_RejectHist");
  if (!Gl1_RejectHist)
    {
      cout << "Gl1_RejectHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_ExpectRejectHist = cl->getHisto("Gl1_ExpectRejectHist");
  if (!Gl1_ExpectRejectHist)
    {
      cout << "Gl1_ExpectRejectHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_bandwidthplot = cl->getHisto("Gl1_bandwidthplot");
  if (!Gl1_bandwidthplot)
    {
      cout << "Gl1_bandwidthplot does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_CrossCounter_bbcll1 = cl->getHisto("Gl1_CrossCounter_bbcll1");
  if (!Gl1_CrossCounter_bbcll1)
    {
      cout << "Gl1_CrossCounter_bbcll1 does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_CrossCounter_bbcll1_nvc = cl->getHisto("Gl1_CrossCounter_bbcll1_nvc");
  if (!Gl1_CrossCounter_bbcll1_nvc)
    {
      cout << "Gl1_CrossCounter_bbcll1_nvc does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_CrossCounter_zdc = cl->getHisto("Gl1_CrossCounter_zdc");
  if (!Gl1_CrossCounter_zdc)
    {
      cout << "Gl1_CrossCounter_zdc does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_present_count = cl->getHisto("Gl1_present_count");
  if (!Gl1_present_count)
    {
      cout << "Gl1_present_count does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_present_rate = cl->getHisto("Gl1_present_rate");
  if (!Gl1_present_rate)
    {
      cout << "Gl1_present_rate does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_StorageHist = cl->getHisto("Gl1_StorageHist");
  if (!Gl1_StorageHist)
    {
      cout << "Gl1_StorageHist does not exist!" << endl;
      DrawDeadServer();
      return -1;
    }
  TH1 *Gl1_RejectRatio_1 = cl->getHisto("Gl1_RejectRatio_1");
  if (!Gl1_RejectRatio_1)
    {
      cout << "Gl1_RejectRatio_1 does not exist!" << endl;
      DrawDeadServer();
    }
  TH1 *Gl1_RejectRatio_2 = cl->getHisto("Gl1_RejectRatio_2");
  if (!Gl1_RejectRatio_2)
    {
      cout << "Gl1_RejectRatio_2 does not exist!" << endl;
      DrawDeadServer();
    }

  TStyle *oldStyle = gStyle;
  gl1Style->cd();

  double total_size = Gl1_StorageHist->GetBinContent(2);

  float trigsize[32];
  for (int ii = 0; ii < 32; ii++)
    {
      trigsize[ii] = Gl1_StorageHist->GetBinContent(ii + 5);
    }
  
  int BBCLL1_bit = -1;
  int bias1_bit = -1;
  int bias2_bit = -1;
  int bias3_bit = -1;

  // Read in the list of prescale values
  ostringstream filename;
  ifstream infile;

  filename.str("");
  if (getenv("GL1REJECTION"))
    {
      filename << getenv("GL1REJECTION");
    }
  else
    {
      filename << "gl1_expected_rejection.list";
    }

  infile.open(filename.str().c_str(), ios_base::in);

  if (infile)
    {
      int i = 0;
      while (
	     infile
	     >> bias1_expected_rejection[i] >> bias1_expected_rejection_err[i]
	     >> bias2_expected_rejection[i] >> bias2_expected_rejection_err[i]
	     )
	{
	  i++;
	  if (i > 32)
	    {
	      cout << PHWHERE << filename << " is corrupt or has wrong format, correct it";
	      exit(1);
	    }
	}

    infile.close();
    infile.clear();
    }
  else
    {
      cout << "Could not open file " << filename.str()
	  << " no expected rejection" ;
      return 0;
    }

  filename.str("");
  if (getenv("GL1MINBIAS"))
    {
      filename << getenv("GL1MINBIAS");
    }
  else
    {
      filename << "gl1_isminbias.list";
    }

  infile.open(filename.str().c_str(), ios_base::in);
  
  if (infile)
    {
      getline(infile, BBCLL1_NAME);
      getline(infile, ZDC_NAME);
      getline(infile, BBCLL1_NOVERTEXCUT_NAME);
      infile.close();
      infile.clear();
    }
  else
    {
      cout << "Could not open file " << filename.str()
	   << " no minbias trigger definitions" << endl;
      return 0;
    }

  for (int i = 1;i < 32;i++)
    {
      // Below we will decide these bits:  BBCLL1_bit ZDC_bit BBCLL1_novertex_bit
      if (cl->OnlTrig()->get_lvl1_trig_name_bybit(i) == BBCLL1_NAME)
        {
	  //cout<<"find bbcll1 at bit "<<i<<endl;
	  BBCLL1_bit = i;
          bias1_bit = i;
        }
      else if ( cl->OnlTrig()->get_lvl1_trig_name_bybit(i) == ZDC_NAME)
        {
	  //cout<<"find zdc at bit "<<i<<endl;
          bias2_bit = i;
        }
      else if (cl->OnlTrig()->get_lvl1_trig_name_bybit(i) == BBCLL1_NOVERTEXCUT_NAME)
        {
	  //cout<<"find bbc_novertexcut at bit "<<i<<endl;
          bias3_bit = i;
        }
    } 


  //////////////////////////////////////////////////////////////////////////////
  ///////////// This section displays the trigger rejection factors and
  ///////////// bandwidths
  //////////////////////////////////////////////////////////////////////////////

  double trigreject[32];

  gl1_c1->cd();
  transparent1->cd();
  transparent1->SetEditable(kTRUE);
  transparent1->Clear();
  // Draw a global picture title
  ostringstream ctitle;

  ctitle.str("");
  ctitle << "Measured Rejection - Run " << runnumber;
  GL1title->SetText(0.1, 0.95, ctitle.str().c_str());
  GL1title->SetNDC();
  GL1title->SetTextSize(0.05);
  GL1title->Draw();


  // Plot the rejection factors

  // Based on BBCLL1

  plotpad5->cd();
  plotpad5->SetLogy();
  plotpad5->SetGrid();
  plotpad5->SetRightMargin(0.0077428);

  if (bias1_bit >= 0)
    {
      float minbias_triggers = (float) Gl1_ScaledHist->GetBinContent(1 + bias1_bit);

      for (int i = 1;i < 32;i++)
        {
          float livecount = Gl1_ScaledHist_BBCLL1->GetBinContent(1 + i);
          if (livecount > 0)
            {
              trigreject[i] = minbias_triggers/livecount;
            }

          else
            {
              trigreject[i] = 9999999999.;
            }

          Gl1_RejectRatio_1->SetBinContent(1 + i, trigreject[i]);

        }
    }
  else
    {
      for (int i = 1;i <= 32;i++)
        {
          Gl1_RejectRatio_1->SetBinContent(i, 9999999999.);
        }
    }

  Gl1_RejectRatio_1->SetStats(0);
  Gl1_RejectRatio_1->SetMinimum(0.1);
  Gl1_RejectRatio_1->SetMaximum(10000);
  Gl1_RejectRatio_1->SetMarkerColor(4);
  Gl1_RejectRatio_1->SetMarkerStyle(20);
  Gl1_RejectRatio_1->DrawCopy("ep");

  TBox Rejection1Ref[32]; 

  for (int i = 0;i < 32;i++) {
    // Markers only for channels with rejection
    if(bias1_expected_rejection[i]>1.0){
      //Gl1_RejectRatio_1_ref->SetBinContent(i, bias1_expected_rejection[i]);
      //Gl1_RejectRatio_1_ref->SetBinError(i, bias1_expected_rejection_err[i]);
      
      Rejection1Ref[i].SetFillStyle(1001);
      Rejection1Ref[i].SetFillColor(3);
      Rejection1Ref[i].SetLineColor(3);
      Rejection1Ref[i].DrawBox(i+0.2-1.0,
				   log10(bias1_expected_rejection[i]-bias1_expected_rejection_err[i]),
			          (i+0.8)-1.0,
				   log10(bias1_expected_rejection[i]+bias1_expected_rejection_err[i]) );

    }
  }
  Gl1_RejectRatio_1->DrawCopy("epsame");


  // Check measured rejections vs. expected - write error message if necessary
  // This is tuned for Run-5 pp ERT rejections only. 

  bool err = false; 
  
  TText legend4x4a;
  for (int i = 0;i < 32;i++) {
  
    switch(i){

    case 10:
      {
        double check = Gl1_RejectRatio_1->GetBinContent(i);
	if( ((check>bias1_expected_rejection[i]+bias1_expected_rejection_err[i]) ||
	    (check<bias1_expected_rejection[i]-bias1_expected_rejection_err[i])) && 
	    (check!=9999999999.) ){

	  legend4x4a.SetTextColor(2);
	  legend4x4a.SetTextSize(0.07);
	  //legend4x4a.DrawText(8, 9, "ERT_4x4a&BBCLL1 rejection out of bounds!");
	  
	  err = true; 

	}

        break;
      }

    case 12:
      {
        double check = Gl1_RejectRatio_1->GetBinContent(i);
	if( ((check>bias1_expected_rejection[i]+bias1_expected_rejection_err[i]) ||
	    (check<bias1_expected_rejection[i]-bias1_expected_rejection_err[i])) && 
	    (check!=9999999999.) ){

	  legend4x4a.SetTextColor(2);
	  legend4x4a.SetTextSize(0.07);
	  //legend4x4a.DrawText(8, 3, "ERT_4x4b&BBCLL1 rejection out of bounds!");
	  
	  err = true; 

	}

        break;
      }

    case 13:
      {
        double check = Gl1_RejectRatio_1->GetBinContent(i);
	if( ((check>bias1_expected_rejection[i]+bias1_expected_rejection_err[i]) ||
	    (check<bias1_expected_rejection[i]-bias1_expected_rejection_err[i])) && 
	    (check!=9999999999.) ){

	  legend4x4a.SetTextColor(2);
	  legend4x4a.SetTextSize(0.07);
	  //legend4x4a.DrawText(8, 1, "ERT_4x4c&BBCLL1 rejection out of bounds!");
	  
	  err = true; 

	}

        break;
      }

    case 14:
      {
        double check = Gl1_RejectRatio_1->GetBinContent(i);
	if( ((check>bias1_expected_rejection[i]+bias1_expected_rejection_err[i]) ||
	    (check<bias1_expected_rejection[i]-bias1_expected_rejection_err[i])) && 
	    (check!=9999999999.) ){

	  legend4x4a.SetTextColor(2);
	  legend4x4a.SetTextSize(0.07);
	  //legend4x4a.DrawText(8, 0.3, "ERT_E&BBCLL1 rejection out of bounds!");
	  
	  err = true; 

	}

        break;
      }

    default:
      break;

    }


  }

  TText legendERTErr;
  if (err)
    {

      legendERTErr.SetTextColor(1);
      legendERTErr.SetTextSize(0.05);
      //legendERTErr.DrawText(1, 0.15, "Check no.of events, ERT/ERTLVL1 monitor and take action if required.");

    }

  legend18->SetY(2000);
  legend18->SetX(1.0);
  ostringstream ignore;
  ignore << BBCLL1_NAME << ": Ignore for now";
  //  legend18->SetTitle(BBCLL1_NAME.c_str());
  legend18->SetTitle(ignore.str().c_str());
  legend18->SetTextColor(4);
  legend18->SetTextSize(0.07);
  legend18->Draw();

  plotpad5->Update();

  // Based on ZDCNS

  plotpad6->cd();
  plotpad6->SetLogy();
  plotpad6->SetGrid();
  plotpad6->SetRightMargin(0.0077428);

  if (bias2_bit >= 0)
    {
      float minbias_triggers = (float) Gl1_ScaledHist->GetBinContent(1 + bias2_bit);

      for (int i = 1;i < 32;i++)
        {
          float livecount = Gl1_ScaledHist_ZDC->GetBinContent(1 + i);
          if (livecount > 0)
            {
              trigreject[i] = minbias_triggers/livecount;
            }

          else
            {
              trigreject[i] = 9999999999.;
            }

          Gl1_RejectRatio_2->SetBinContent(1 + i, trigreject[i]);

        }
    }
  else
    {
      for (int i = 1;i <= 32;i++)
        {
          Gl1_RejectRatio_2->SetBinContent(i, 9999999999.);
        }
    }

  Gl1_RejectRatio_2->SetStats(0);
  Gl1_RejectRatio_2->SetMinimum(0.1);
  Gl1_RejectRatio_2->SetMaximum(10000);
  Gl1_RejectRatio_2->SetMarkerColor(4);
  Gl1_RejectRatio_2->SetMarkerStyle(20);
  Gl1_RejectRatio_2->DrawCopy("ep");

  TBox Rejection2Ref[32]; 

  for (int i = 0;i < 32;i++) {
    // Markers only for channels with rejection
    if (bias2_expected_rejection[i] > 1.0)
      {
	Rejection2Ref[i].SetFillStyle(1001);
	Rejection2Ref[i].SetFillColor(3);
	Rejection2Ref[i].SetLineColor(3);
	Rejection2Ref[i].DrawBox(i + 0.2 - 1.0,
				 log10(bias2_expected_rejection[i] - bias2_expected_rejection_err[i]),
				 (i + 0.8) - 1.0,
				 log10(bias2_expected_rejection[i] + bias2_expected_rejection_err[i]) );

  }
  }
  Gl1_RejectRatio_2->DrawCopy("epsame");

  legend19->SetY(2000);
  legend19->SetX(1.0);
  ignore.str("");
  ignore << ZDC_NAME << ": Ignore for now";
  //  legend19->SetTitle(ZDC_NAME.c_str());
  legend19->SetTitle(ignore.str().c_str());
  legend19->SetTextColor(4);
  legend19->SetTextSize(0.07);
  legend19->Draw();

  plotpad6->Update();


  ////////////////////////////////////////////////////////////////////////////
  ////////////////// Now plot the fraction of bandwidth used by each trigger
  ////////////////////////////////////////////////////////////////////////////

  float total_bandwidth = 0;
  for (int i = 0;i < 32;i++)
    {
      total_bandwidth += float(trigsize[i])/total_size;
      if(trigsize[i]>0)
	Gl1_bandwidthplot->SetBinContent(i + 1, 100.0*(trigsize[i] / total_size));
      else
	Gl1_bandwidthplot->SetBinContent(i + 1, 0.0);
    }

  plotpad2->cd();
  plotpad2->SetFillColor(0);
  plotpad2->Range( -4.08805, -12.3214, 31.7777, 112.107);
  plotpad2->SetBorderSize(2);
  plotpad2->SetRightMargin(0.0077428);
  plotpad2->SetGrid();

  Gl1_bandwidthplot->SetLabelOffset(100, "x");
  Gl1_bandwidthplot->SetStats(0);
  Gl1_bandwidthplot->SetMinimum(0);
  Gl1_bandwidthplot->SetMaximum(100);
  Gl1_bandwidthplot->SetFillColor(38);
  Gl1_bandwidthplot->DrawCopy();


  legend3->SetTextColor(2);
  legend3->SetTextSize(0.09);
  legend3->Draw();

  // Now add the rate information to the bandwidth plot for the minbias trigger

  float rawbbcll1, livebbcll1, liveraw_ratio;
  rawbbcll1 = livebbcll1 = liveraw_ratio = -1.0;
  if(BBCLL1_bit>=0)
    {
      rawbbcll1 = Gl1_present_rate->GetBinContent(1 + BBCLL1_bit);
      livebbcll1 = Gl1_present_rate->GetBinContent(1 + BBCLL1_bit + 32);
      liveraw_ratio = livebbcll1 / rawbbcll1;
    }

  Stat_t NumBadScaledEvents = Gl1_StorageHist->GetBinContent(37);

  if(NumBadScaledEvents!=0){

      legend15->SetTextColor(2);
      legend15->SetTextSize(0.10);
      ostringstream errstring;
      errstring << "Detected " << NumBadScaledEvents << " zero scaled words!"<< endl;
      legend15->DrawText(0, 70, errstring.str().c_str());
      legend15->DrawText(0, 40, "Stop Run and Reload Trigger!");
    
  }
  else if(Gl1_StorageHist->GetBinContent(38)>0){

      legend15->SetTextColor(2);
      legend15->SetTextSize(0.10);
      ostringstream errstring;
      errstring << "Detected " << Gl1_StorageHist->GetBinContent(38) << " scaled/live mismatch!"<< endl;
      legend15->DrawText(0, 70, errstring.str().c_str());
 
  }
  else if(Gl1_StorageHist->GetBinContent(39)>0){

      legend15->SetTextColor(2);
      legend15->SetTextSize(0.10);
      ostringstream errstring;
      errstring << "Detected " << Gl1_StorageHist->GetBinContent(39) << " scaled/raw mismatch!"<< endl;
      legend15->DrawText(0, 70, errstring.str().c_str());
 
  }
  else if(Gl1_StorageHist->GetBinContent(40)>0){

      legend15->SetTextColor(2);
      legend15->SetTextSize(0.10);
      ostringstream errstring;
      errstring << "Detected " << Gl1_StorageHist->GetBinContent(40) << " live/raw mismatch!"<< endl;
      legend15->DrawText(0, 70, errstring.str().c_str());
 
  }
  else{

    if(total_bandwidth>0.98) {
 
      ostringstream ratestring;
      ratestring << BBCLL1_NAME << " trigger: Live "
	         << Gl1_present_rate->GetBinContent(1 + BBCLL1_bit + 32)
	         << " Raw " 
	         << Gl1_present_rate->GetBinContent(1 + BBCLL1_bit)
	         << " Live Fraction " << liveraw_ratio;
  
      legend15->SetText(2, 79, ratestring.str().c_str());
      legend15->SetTextColor(1);
      legend15->SetTextSize(0.05);
      //legend15->Draw();
    }
    else {

      if(total_size>0.0){
        legend15->SetTextColor(2);
        legend15->SetTextSize(0.12);
        legend15->DrawText(0, 70, "Bandwidth Sum Error!");
        legend15->DrawText(0, 50, "Stop Run and Reload Trigger!");
        legend15->DrawText(0, 30, "  (OR too few events)");

      }

    }
    
  }
  plotpad2->Update();

  for (unsigned int itrig = 0; itrig < 32; itrig++)
    {
      trigname1[itrig]->SetTextAngle(90);
      trigname1[itrig]->SetTextSize(0.04);
      string tempname = cl->OnlTrig()->get_lvl1_trig_name_bybit(itrig);
 
      trigname1[itrig]->SetText((float) (itrig + 0.75), -4.0, cl->OnlTrig()->get_lvl1_trig_name_bybit(itrig).c_str());
      trigname1[itrig]->Draw();
    }
 
  gl1_c1->Update();
 
 
  ///////////////////////////////////////////////////////////////////////////
  ////////////////////// The bucket by bucket trigger yields are plotted here
  ////////////////////////////////////////////////////////////////////////////

  //  gl1_c2->cd();
  transparent2->cd();
  transparent2->SetEditable(kTRUE);
  transparent2->Clear();
  // Draw a global picture title
  ctitle.str("");
  ctitle << "Crossing Patterns - Run " << runnumber;
  Crosstitle->SetText(0.1, 0.95, ctitle.str().c_str());
  //Crosstitle->SetFillColor(16);
  Crosstitle->SetNDC();
  Crosstitle->SetTextSize(0.05);
  Crosstitle->Draw();

  float scale;

  plotpad11->cd();

  Gl1_CrossCounter_bbcll1->SetXTitle("Bunch Crossing");
  Gl1_CrossCounter_bbcll1->SetMinimum(0.5);
  Gl1_CrossCounter_bbcll1->DrawCopy();
  if(Gl1_CrossCounter_bbcll1->Integral() > 0) plotpad11->SetLogy(1);
  else plotpad11->SetLogy(0);

  scale = Gl1_CrossCounter_bbcll1->GetMaximum() * 0.3;

  legend11->SetText(8.0, scale, "BBCLL1 Bunch Crossing Pattern");
  legend11->SetTextColor(4);
  legend11->SetTextSize(0.05);
  legend11->Draw();

  // What was this supposed to check for? JGL 5/6/05
  //if(Gl1_CrossCounter_zdc->Integral()>100)
  //  if(Gl1_CrossCounter_bbcll1->Integral()<Gl1_CrossCounter_zdc->Integral()/50.0)
  //    {
  //	legend11->SetText(0.0, 0.5, "Stop the Run!");
  //	legend11->SetTextColor(2);
  //	legend11->SetTextSize(0.2);
  //	legend11->Draw();
  //	legend11->DrawText(0.0, -0.5, "Please reload Trigger!");
  //    }
  
  plotpad11->Update();

  plotpad12->cd();

  Gl1_CrossCounter_bbcll1_nvc->SetXTitle("Bunch Crossing");
  Gl1_CrossCounter_bbcll1_nvc->SetMinimum(0.5);
  Gl1_CrossCounter_bbcll1_nvc->DrawCopy();
  if(Gl1_CrossCounter_bbcll1_nvc->Integral() > 0) plotpad12->SetLogy(1);
  else plotpad12->SetLogy(0);

  scale = Gl1_CrossCounter_bbcll1_nvc->GetMaximum() * 0.3;

  legend12->SetText(8.0, scale, "BBCLL1 noVertexCut Bunch Crossing Pattern");
  legend12->SetTextColor(4);
  legend12->SetTextSize(0.05);
  legend12->Draw();

  plotpad12->Update();

  plotpad13->cd();

  Gl1_CrossCounter_zdc->SetXTitle("Bunch Crossing");
  Gl1_CrossCounter_zdc->SetMinimum(0.5);
  Gl1_CrossCounter_zdc->DrawCopy();
  if(Gl1_CrossCounter_zdc->Integral() > 0) plotpad13->SetLogy(1);
  else plotpad13->SetLogy(0);

  scale = Gl1_CrossCounter_zdc->GetMaximum() * 0.3;

  legend13->SetText(8.0, scale, "ZDC Bunch Crossing Pattern");
  legend13->SetTextColor(4);
  legend13->SetTextSize(0.05);
  legend13->Draw();

  plotpad13->Update();

  gl1_c2->Update();

 
  /////////////////////////////////////////////////

  oldStyle->cd();

  return 0;
}

int Gl1MonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  gl1_c1->Print(filename.str().c_str());

  filename.str("");
  filename << ThisName << "_2_" << cl->RunNumber() << ".ps";
  gl1_c2->Print(filename.str().c_str());

  return 0;
}

int Gl1MonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  string pngfile = cl->htmlRegisterPage(*this, "Measured Rejection", "1", "png");
  cl->CanvasToPng(gl1_c1, pngfile);

  pngfile = cl->htmlRegisterPage(*this, "Crossing Pattern", "2", "png");
  cl->CanvasToPng(gl1_c2, pngfile);
  ostringstream index, title;
  title.fill('0');
  for (int i=0; i<4;i++)
    {
      index.str("");
      index << (i+3);
      title.str("");
      title << "Xing Pattern bit " << setw(2) << 1+ i*8 << " to " << setw(2) << 8 +i*8;
      pngfile = cl->htmlRegisterPage(*this, title.str().c_str(), index.str().c_str(), "png");
      cl->CanvasToPng(Gl1_bc_canv[i], pngfile);
    }
  cl->SaveLogFile(*this);

  return 0;
}


int Gl1MonDraw::DrawDeadServer()
{

  TText FatalMsg;

  if (gROOT->FindObject("Gl1_c1"))
    {
      gl1_c1->cd();
      gl1_c1->Clear("D");
      transparent1->Clear();
      transparent1->SetEditable(kTRUE);
      transparent1->cd();

      FatalMsg.SetTextFont(62);
      FatalMsg.SetTextSize(0.1);
      FatalMsg.SetTextColor(4);
      FatalMsg.SetNDC();  // set to normalized coordinates
      FatalMsg.SetTextAlign(23); // center/top alignment
      FatalMsg.DrawText(0.5, 0.9, "GL1");
      FatalMsg.SetTextAlign(22); // center/center alignment
      FatalMsg.DrawText(0.5, 0.5, "SERVER");
      FatalMsg.SetTextAlign(21); // center/bottom alignment
      FatalMsg.DrawText(0.5, 0.1, "DEAD");

      transparent1->Update();
      gl1_c1->Update();
    }

  if (gROOT->FindObject("Gl1_c2"))
    {
      gl1_c2->cd();
      gl1_c2->Clear("D");
      transparent2->Clear();
      transparent2->SetEditable(kTRUE);
      transparent2->cd();

      FatalMsg.SetTextFont(62);
      FatalMsg.SetTextSize(0.1);
      FatalMsg.SetTextColor(4);
      FatalMsg.SetNDC();  // set to normalized coordinates
      FatalMsg.SetTextAlign(23); // center/top alignment
      FatalMsg.DrawText(0.5, 0.9, "GL1");
      FatalMsg.SetTextAlign(22); // center/center alignment
      FatalMsg.DrawText(0.5, 0.5, "SERVER");
      FatalMsg.SetTextAlign(21); // center/bottom alignment
      FatalMsg.DrawText(0.5, 0.1, "DEAD");

      transparent2->Update();
      gl1_c2->Update();
    }
  ostringstream canvname;
  for (int iwhich = 0; iwhich < 4; iwhich++)
    {
      canvname.str("");
      canvname << "Gl1_bc_canv" << iwhich;
      if (gROOT->FindObject(canvname.str().c_str()))
	{
	  Gl1_bc_canv[iwhich]->cd();
	  Gl1_bc_canv[iwhich]->Clear("D");
	  bc_transparent[iwhich]->cd();
	  bc_transparent[iwhich]->Clear();
	  FatalMsg.SetTextFont(62);
	  FatalMsg.SetTextSize(0.1);
	  FatalMsg.SetTextColor(4);
	  FatalMsg.SetNDC();  // set to normalized coordinates
	  FatalMsg.SetTextAlign(23); // center/top alignment
	  FatalMsg.DrawText(0.5, 0.9, "GL1");
	  FatalMsg.SetTextAlign(22); // center/center alignment
	  FatalMsg.DrawText(0.5, 0.5, "SERVER");
	  FatalMsg.SetTextAlign(21); // center/bottom alignment
	  FatalMsg.DrawText(0.5, 0.1, "DEAD");
	  bc_transparent[iwhich]->Update();
	  Gl1_bc_canv[iwhich]->Update();
      }
  }
  return 0;
}
