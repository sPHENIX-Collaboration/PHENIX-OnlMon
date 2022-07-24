#include <TOFWMonDraw.h>
#include <OnlMonClient.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TLine.h>
#include <TText.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

TOFWMonDraw::TOFWMonDraw() : OnlMonDraw("TOFWMON")
{
  // Config
  ///////////////////////////

  canvasMode = 0;

  evtTrigInMin = 100;

  peak00htofw210_v = 1500.; // TVC Low
  peak01htofw210_v = 3000.; // TVC High
  peak00htofw220_v = 10.; // QVC Low
  peak01htofw220_v = 500.; // QVC High
  peak00htofw230_v = 1.; // Mim Num of Hit
  //peak00htofw410_v = 50.; // tmp
  //peak01htofw410_v = 80.; // tmp
  peak00htofw420_v = -5.; // Pos. Low
  peak01htofw420_v = 5.; // Pos. High

  htofw530_ymax = 1000;//0.0070;


  // hist
  //////////////////////////
  htofwx10 = new TH2F("htofwx10", "TOFW Dead Strip Map", 400, -200, 200, 400, -280, 90);
  htofwx11 = new TH2F("htofwx11", "erorr", 400, -200, 200, 400, -280, 90);
  htofwx12 = new TH2F("htofwx12", "erorr", 400, -200, 200, 400, -280, 90);

  w = new TH1F("wtofw","",TOFW_NSTRIP,0,TOFW_NSTRIP);
  gtofw530 = new TGraph(TOFW_NSTRIP);
  gtofw530->SetTitle("MIP peak vs strip ID");

  peak00htofw210 = new TLine(peak00htofw210_v, 0., peak00htofw210_v, 10000.);
  peak01htofw210 = new TLine(peak01htofw210_v, 0., peak01htofw210_v, 10000.);
  peak00htofw220 = new TLine(peak00htofw220_v, 0., peak00htofw220_v, 10000.);
  peak01htofw220 = new TLine(peak01htofw220_v, 0., peak01htofw220_v, 10000.);
  peak00htofw420 = new TLine(peak00htofw420_v, 0., peak00htofw420_v, 10000.);
  peak01htofw420 = new TLine(peak01htofw420_v, 0., peak01htofw420_v, 10000.);

  peak00htofw210->SetLineStyle(2);
  peak01htofw210->SetLineStyle(2);
  peak00htofw220->SetLineStyle(2);
  peak00htofw420->SetLineStyle(2);
  peak01htofw420->SetLineStyle(2);

  peak00htofw210->SetLineColor(2);
  peak01htofw210->SetLineColor(2);
  peak00htofw220->SetLineColor(2);
  peak00htofw420->SetLineColor(2);
  peak01htofw420->SetLineColor(2);

  peak00htofw210->SetLineWidth(2);
  peak01htofw210->SetLineWidth(2);
  peak00htofw220->SetLineWidth(2);
  peak00htofw420->SetLineWidth(2);
  peak01htofw420->SetLineWidth(2);

  //line for dead strip information
  tofwl00 = new TLine( -200.,  -80., -200.,   80);
  tofwl01 = new TLine( -150.,  -80., -150.,   80);
  tofwl02 = new TLine( -100.,  -80., -100.,   80);
  tofwl03 = new TLine(  -50.,  -80.,  -50.,   80);
  tofwl04 = new TLine(    0.,  -80.,    0.,   80);
  tofwl05 = new TLine(   50.,  -80.,   50.,   80);
  tofwl06 = new TLine(  100.,  -80.,  100.,   80);
  tofwl07 = new TLine(  150.,  -80.,  150.,   80);
  tofwl08 = new TLine(  200.,  -80.,  200.,   80);
  tofwl10 = new TLine( -200.,   80.,  200.,   80);
  tofwl11 = new TLine( -200.,  -80.,  200.,  -80);
  tofwl20 = new TLine(  -50., -270.,  -50., -120);
  tofwl21 = new TLine(    0., -270.,    0., -120);
  tofwl22 = new TLine(   50., -270.,   50., -120);
  tofwl30 = new TLine(  -50., -120.,   50., -120);
  tofwl31 = new TLine(  -50., -270.,   50., -270);

  //line for diviation of box & bus
  tofw521 =new TLine(64.5      ,0,64.5      ,htofw530_ymax);
  tofw522 =new TLine(64.5+64   ,0,64.5+64   ,htofw530_ymax);
  tofw523 =new TLine(64.5+64*2.,0,64.5+64*2.,htofw530_ymax);
  tofw524 =new TLine(64.5+64*3.,0,64.5+64*3.,htofw530_ymax);
  tofw525 =new TLine(64.5+64*4.,0,64.5+64*4.,htofw530_ymax);
  tofw526 =new TLine(64.5+64*5.,0,64.5+64*5.,htofw530_ymax);
  tofw527 =new TLine(64.5+64*6.,0,64.5+64*6.,htofw530_ymax);
  
  tofw521->SetLineStyle(2);
  tofw522->SetLineStyle(2);
  tofw523->SetLineStyle(2);
  tofw524->SetLineStyle(2);
  tofw525->SetLineStyle(2);  
  tofw526->SetLineStyle(2);  
  tofw527->SetLineStyle(2);  

  tofw521->SetLineColor(4);
  tofw522->SetLineColor(4);
  tofw523->SetLineColor(4);
  tofw524->SetLineColor(4);
  tofw525->SetLineColor(4);  
  tofw526->SetLineColor(4);  
  tofw527->SetLineColor(4);  

  tofw521->SetLineWidth(1);
  tofw522->SetLineWidth(1);
  tofw523->SetLineWidth(1);
  tofw524->SetLineWidth(1);
  tofw525->SetLineWidth(1);  
  tofw526->SetLineWidth(1);  
  tofw527->SetLineWidth(1);  

  float ypos_text = htofw530_ymax * 0.90;
  tx520=new TText(32      ,ypos_text,"0~1");
  tx521=new TText(32+64*1.,ypos_text,"2~3");
  tx522=new TText(32+64*2.,ypos_text,"4~5");
  tx523=new TText(32+64*3.,ypos_text,"6~7");
  tx524=new TText(32+64*4.,ypos_text,"8~9");
  tx525=new TText(32+64*5.,ypos_text,"10~11");
  tx526=new TText(32+64*6.,ypos_text,"12~13");
  tx527=new TText(32+64*7.,ypos_text,"14~15");
  
  tx520->SetTextSize(0.075);
  tx521->SetTextSize(0.075);
  tx522->SetTextSize(0.075);
  tx523->SetTextSize(0.075);
  tx524->SetTextSize(0.075);
  tx525->SetTextSize(0.075);
  tx526->SetTextSize(0.075);
  tx527->SetTextSize(0.075);  

  tx520->SetTextColor(4);
  tx521->SetTextColor(4);
  tx522->SetTextColor(4);
  tx523->SetTextColor(4);
  tx524->SetTextColor(4);
  tx525->SetTextColor(4);
  tx526->SetTextColor(4);
  tx527->SetTextColor(4);  


  tofwlabel    = new TPaveText(0.01, 0.91, 0.99, 0.97);
  tofwmessage  = new TPaveText(0.71, 0.78, 0.99, 0.89);
  tofwstatinfo = new TPaveText(0.71, 0.60, 0.99, 0.75);
  
  tofwmessage ->SetTextSize(0.02);
  tofwstatinfo->SetTextSize(0.02);
  return ;
}

TOFWMonDraw::~TOFWMonDraw()
{
  delete gtofw530;
  delete peak00htofw210;
  delete peak01htofw210;
  delete peak00htofw220;
  delete peak01htofw220;
  delete peak00htofw420;
  delete peak01htofw420;

  delete tofwl00;
  delete tofwl01;
  delete tofwl02;
  delete tofwl03;
  delete tofwl04;
  delete tofwl05;
  delete tofwl06;
  delete tofwl07;
  delete tofwl08;
  delete tofwl10;
  delete tofwl11;
  delete tofwl20;
  delete tofwl21;
  delete tofwl22;
  delete tofwl30;
  delete tofwl31;

  delete tofw521;
  delete tofw522;
  delete tofw523;
  delete tofw524;
  delete tofw525;
  delete tofw526;
  delete tofw527;
  
  delete tx520;
  delete tx521;
  delete tx522;
  delete tx523;
  delete tx524;
  delete tx525;
  delete tx526;
  delete tx527;

  delete tofwlabel;
  delete tofwmessage;
  delete tofwstatinfo;

  delete w;
  delete htofwx10;
  delete htofwx11;
  delete htofwx12;

  return;
}

int TOFWMonDraw::InitTofwCanvas0()
{

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  TofwCan0 = new TCanvas("TofwCanvas0", "TOFW Mon Granule Overview", -1, 0, xsize, ysize);
  gSystem->ProcessEvents();
  TofwCan0->SetFillColor(41);
  TofwPad[0][3] = new TPad("tofwpad00", "tofwpad00", 0.01, 0.60, 0.70, 0.88, 0);
  TofwPad[0][1] = new TPad("tofwpad01", "tofwpad01", 0.01, 0.31, 0.50, 0.59, 0);
  TofwPad[0][0] = new TPad("tofwpad03", "tofwpad03", 0.51, 0.31, 0.99, 0.59, 0);
  TofwPad[0][2] = new TPad("tofwpad02", "tofwpad02", 0.01, 0.01, 0.99, 0.30, 0);
  TofwPad[0][0]->Draw();
  TofwPad[0][1]->Draw();
  TofwPad[0][2]->Draw();
  TofwPad[0][3]->Draw();
  return 0;
}

int TOFWMonDraw::InitTofwCanvas1()
{
  TofwCan1 = new TCanvas("TofwCanvas1", "TOFW Mon Geo.", -10, 10, 1000, 700);
  gSystem->ProcessEvents();
  TofwCan1->SetFillColor(24);
  TofwPad[1][0] = new TPad("tofwpad10", "tofwpad10", 0.03, 0.03, 0.97, 0.97, 0);
  TofwPad[1][0]->SetLeftMargin(0.05);
  TofwPad[1][0]->SetRightMargin(0.05);
  TofwPad[1][0]->Draw();
  return 0;
}
int TOFWMonDraw::Draw(const char *what)
{
  if (! gROOT->FindObject("TofwCanvas0"))
    {
      InitTofwCanvas0();
    }
  if (canvasMode > 1)
    {
      if (! gROOT->FindObject("TofwCanvas1"))
        {
          InitTofwCanvas1();
        }
    }

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *htofwinfo = cl->getHisto("htofwinfo");
  TH1 *htofw210 = cl->getHisto("htofw210");
  TH1 *htofw220 = cl->getHisto("htofw220");
  TH1 *htofw420 = cl->getHisto("htofw420");
  TProfile *htofw530 = (TProfile *)cl->getHisto("htofw530");
  
  htofwx10->Reset();
  htofwx11->Reset();
  htofwx12->Reset();

  if (!(htofwinfo && htofwx10))
  //if (!(htofwinfo))
    {
      cout << "Server Dead" << endl;

      tofwlabel->Clear();
      tofwlabel->SetLabel("TOFW ONLINE MONITOR");
      tofwlabel->AddText(" ");
      tofwlabel->AddText("Server Dead");
      tofwlabel->SetFillColor(20);

      // Label(Message)
      tofwmessage->Clear();
      tofwmessage->SetLabel("Message");
      tofwmessage->AddText("Restart Server!");
      tofwmessage->SetFillColor(5);

      TofwCan0->cd();
      tofwlabel->Draw();
      tofwmessage->Draw();

      return -1;
    }
  else
    {

      for(int istrip=0;istrip<TOFW_NSTRIP;istrip++){
	int nbin=htofw530->GetBin(istrip);
	double conbin=htofw530->GetBinContent(nbin);
	gtofw530->SetPoint(istrip,istrip,conbin);
      }

      ostringstream otext;
      ostringstream tmptext;

      htofw210->SetFillColor(30);
      htofw220->SetFillColor(30);
      htofw420->SetFillColor(30);

      gtofw530->SetMarkerColor(2);
      gtofw530->SetMarkerStyle(20);
      gtofw530->SetMarkerSize(0.5);

      htofwx10->SetMarkerColor(30);
      htofwx10->SetMarkerStyle(20);
      htofwx11->SetMarkerColor(13);
      htofwx11->SetMarkerStyle(20);
      htofwx12->SetMarkerColor(2);
      htofwx12->SetMarkerStyle(20);

      float label_size_x = 0.050;
      float label_size_y = 0.050;
      float title_size   = 0.050;

      htofw210->GetXaxis()->SetTitle("TVC[ch]");
      htofw210->GetXaxis()->SetTitleSize(title_size);
      htofw210->GetXaxis()->SetLabelSize(label_size_x);
      htofw210->GetXaxis()->SetLabelOffset(0);
      htofw210->GetYaxis()->SetLabelSize(label_size_y);

      htofw220->GetXaxis()->SetTitle("QVC[ch]");
      htofw220->GetXaxis()->SetTitleSize(title_size);
      htofw220->GetXaxis()->SetLabelSize(label_size_x);
      htofw220->GetXaxis()->SetLabelOffset(0);
      htofw220->GetYaxis()->SetLabelSize(label_size_y);

      htofw420->GetXaxis()->SetTitle("hit position[cm]");
      htofw420->GetXaxis()->SetTitleSize(title_size);
      htofw420->GetXaxis()->SetLabelSize(label_size_x);
      htofw420->GetXaxis()->SetLabelOffset(0);
      htofw420->GetYaxis()->SetLabelSize(label_size_y);

      gtofw530->GetXaxis()->SetLabelSize(label_size_x);
      gtofw530->GetXaxis()->SetLabelOffset(0);
      gtofw530->GetYaxis()->SetLabelSize(label_size_y);

      w->SetTitle("MIP peak vs stripID");
      w->GetXaxis()->SetTitle("stripID");
      w->GetXaxis()->SetTitleSize(title_size);
      w->GetXaxis()->SetLabelSize(label_size_x);
      w->GetXaxis()->SetLabelOffset(0);
      w->GetYaxis()->SetTitle("MIP peak");
      w->GetYaxis()->SetTitleSize(title_size);
      w->GetYaxis()->SetLabelSize(label_size_y);


      // Event Status ( Info of TrigSelection & CutPara. ...)
      ///////////////////////////////////////////////////////
      int cutTrigLow  = (int)htofwinfo->GetBinContent(1);
      int cutTrigHigh = (int)htofwinfo->GetBinContent(2);
      int cutTofwQVC   = (int)htofwinfo->GetBinContent(3);
      int cutTofwTc_0  = (int)htofwinfo->GetBinContent(4);
      int cutTofwTc    = (int)htofwinfo->GetBinContent(5);
      float cutTofwChg = (float)htofwinfo->GetBinContent(6);
      int evtcnt      = (int)htofwinfo->GetBinContent(7);
      evtTrigIn       = (int)htofwinfo->GetBinContent(8);

      time_t evttime = cl->EventTime("CURRENT");

      // Cout
      cout << "Run: " << cl->RunNumber()
	   << " Time:" << ctime(&evttime);
      cout << "Event: " << evtTrigIn << "/" << evtcnt
	   << " [CutPara of Hit] "
	   << cutTrigLow << "< Trig <" << cutTrigHigh
	   << " TVC: " << cutTofwTc
	   << " Chg: " << cutTofwChg
	   << " [DrawingCut] "
	   << " TVC: " << cutTofwTc_0
	   << " QVC: " << cutTofwQVC << endl;

      // Label(Top)
      tofwlabel->Clear();
      tofwlabel->SetLabel("TOFW ONLINE MONITOR");
      tofwlabel->SetFillColor(20);
      tofwlabel->AddText(" ");
      otext << " Run:" << cl->RunNumber();
      otext << " Time:" << ctime(&evttime) << "  ";
      tofwlabel->AddText(otext.str().c_str());

      // Label(Message)
      tofwmessage->Clear();
      tofwmessage->SetLabel("Message");
      tofwmessage->AddText(" ");
      // Label(Status)
      tofwstatinfo->Clear();
      tofwstatinfo->SetLabel("Status");
      tofwstatinfo->AddText(" ");

      // Label(Status)
      tmptext << "Event ( trig / all )";
      tofwstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << evtTrigIn << " / " << evtcnt;
      tofwstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << "[CutPara.of Hit]";
      tofwstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << cutTrigLow << " <Trig< " << cutTrigHigh;
      tofwstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << "TVC: " << cutTofwTc << " ch";
      tofwstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      int saveprec = cout.precision();
      cout.precision(3);
      tmptext << "Charge: " << cutTofwChg << " GeV";
      tofwstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      cout .precision(saveprec);
       
      // Peak Check!! //////////////////////////////////
      if (htofw210)
	{
	  //int max_high_bin = htofw210->GetMaximumBin();
	  int max_high_bin =0;
	  int max_tvcw =0;
	  for(int ibin=0; ibin<3500; ibin++){
	    if(htofw210->GetBinContent(ibin+1)>max_tvcw){
	      max_tvcw = htofw210->GetBinContent(ibin+1);
	      max_high_bin = ibin+1;
	    }
	  }
	  float max_bin_v = htofw210->GetBinCenter(max_high_bin);
	  if (max_bin_v < peak00htofw210_v || peak01htofw210_v < max_bin_v)
	    {
	      tmptext.str("");
	      tmptext << "Check TVC Hist !!";
	      tofwmessage->AddText(tmptext.str().c_str());
	      //flag_message = 1;
	    }
	}
      if (htofw220)
	{
	  int max_high_bin = htofw220->GetMaximumBin();
	  float max_bin_v = htofw220->GetBinCenter(max_high_bin);
	  if (max_bin_v < peak00htofw220_v || peak01htofw220_v < max_bin_v)
	    {
	      tmptext.str("");
	      tmptext << "Check Charge Hist !!";
	      tofwmessage->AddText(tmptext.str().c_str());
	      //flag_message = 1;
	    }
	}
      if (htofw420)
	{
	  int max_high_bin = htofw420->GetMaximumBin();
	  float max_bin_v = htofw420->GetBinCenter(max_high_bin);
	  if (max_bin_v < peak00htofw420_v || peak01htofw420_v < max_bin_v)
	    {
	      tmptext.str("");
	      tmptext << "Check Position Hist !!";
	      tofwmessage->AddText(tmptext.str().c_str());
	      //flag_message = 1;
	    }
	}


      // Draw //////////////////////////////////////////
      TofwCan0->cd();
      tofwlabel->Draw();
      tofwmessage->Draw();
      tofwstatinfo->Draw();

      TofwCan0->cd();
      TofwPad[0][0]->cd();
      
      if (htofw210)
        {
          htofw210->Draw();
          int max_high_bin = htofw210->GetMaximumBin();
          float max_high;
          if (max_high_bin > 0)
            {
              max_high = htofw210->GetBinContent(max_high_bin);
            }
          else
            {
              max_high = 1000;
            }
          peak00htofw210->SetY2(max_high);
          peak01htofw210->SetY2(max_high);
          peak00htofw210->Draw();
          peak01htofw210->Draw();
        }
      TofwPad[0][1]->cd();
      if (htofw220)
        {
          htofw220->Draw();
          int max_high_bin = htofw220->GetMaximumBin();
          float max_high;
          if (max_high_bin > 0)
            {
              max_high = htofw220->GetBinContent(max_high_bin);
            }
          else
            {
              max_high = 1000;
            }
          peak00htofw220->SetY2(max_high);
          peak01htofw220->SetY2(max_high);
          peak00htofw220->Draw();
          peak01htofw220->Draw();
        }
      TofwPad[0][3]->cd();
      if (htofw420)
        {
          htofw420->Draw();
          int max_high_bin = htofw420->GetMaximumBin();
          float max_high;
          if (max_high_bin > 0)
            {
              max_high = htofw420->GetBinContent(max_high_bin);
            }
          else
            {
              max_high = 1000;
            }
          peak00htofw420->SetY2(max_high);
          peak01htofw420->SetY2(max_high);
          peak00htofw420->Draw();
          peak01htofw420->Draw();
         }
      TofwPad[0][2]->cd();
      TofwPad[0][2]->Clear();
      TofwPad[0][2]->SetGridy();

      if (gtofw530)
        {

	  w->SetMaximum(htofw530_ymax);
	  w->SetMinimum(0);
	  w->SetStats(0000);
	  w->Draw();

	  tofw521->Draw();
	  tofw522->Draw();
	  tofw523->Draw();
	  tofw524->Draw();
	  tofw525->Draw();
	  tofw526->Draw();
	  tofw527->Draw();


	  tx520->Draw();
	  tx521->Draw();
	  tx522->Draw();
	  tx523->Draw();
	  tx524->Draw();
	  tx525->Draw();
	  tx526->Draw();
	  tx527->Draw();

	  gtofw530->Draw("P");
	  gtofw530->SetMaximum(htofw530_ymax);
	  gtofw530->SetMinimum(-10);

        }

      TofwCan0->Update();

      if (canvasMode > 1)
        {
          TofwCan1->cd();
          TofwPad[1][0]->cd();
          if (htofwx10 && htofwx11 && htofwx12)
            {
              htofwx10->Draw();
              htofwx11->Draw("same");
              htofwx12->Draw("same");

              tofwl00->Draw();
              tofwl01->Draw();
              tofwl02->Draw();
              tofwl03->Draw();
              tofwl04->Draw();
              tofwl05->Draw();
              tofwl06->Draw();
              tofwl07->Draw();
              tofwl08->Draw();

              tofwl10->Draw();
              tofwl11->Draw();

              tofwl20->Draw();
              tofwl21->Draw();
              tofwl22->Draw();

              tofwl30->Draw();
              tofwl31->Draw();

            }
          TofwCan1->Update();
        }
    }
  return 0;
  
}

  int TOFWMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << Name() << "_" << cl->RunNumber() << ".ps";
  TofwCan0->Print(filename.str().c_str());
  return 0;
}

  int TOFWMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
    if (iret)
      {
        return iret;
      }
  OnlMonClient *cl = OnlMonClient::instance();

  string ofile = cl->htmlRegisterPage(*this,"Monitor","1","png");
  cl->CanvasToPng(TofwCan0, ofile);
  cl->SaveLogFile(*this);
  
  return 0;
}
