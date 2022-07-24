#include <TOFMonDraw.h>
#include <OnlMonClient.h>

#include <TofAddressObject.hh>
#include <TofGeometryObject.hh>

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

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

TOFMonDraw::TOFMonDraw() : OnlMonDraw("TOFMON")
{
  // Config
  ///////////////////////////

  canvasMode = 0;
  
  //evtTrigInMin = 100;
  evtTrigInMin = 100;
  evtTrigInMin_MIP = 30000;
  
  peak00htof210_v = 2000.; // TVC Low
  peak01htof210_v = 3000.; // TVC High
  peak00htof220_v = 10.; // QVC Low
  peak01htof220_v = 100.; // QVC High
  peak00htof230_v = 1.; // Mim Num of Hit
  //peak00htof410_v = 50.; // tmp
  //peak01htof410_v = 80.; // tmp
  peak00htof420_v = -5.; // Pos. Low
  peak01htof420_v = 5.; // Pos. High

  htof530_ymax = 0.0070;

  Th_MIP       = 0.001;
  No_MIP       = 90;

  // Style
  // tofStyle = new TStyle("tofStyle","tofStyle");

  // TOF Obj
  /////////////////////////////////////
  ostringstream parafile;
  char *parafile_dir = getenv("TOFPARADIR");

  tofmonaddress = new TofAddressObject();
  parafile << parafile_dir << "/toffemmap.txt";
  tofmonaddress->fetchFromFile(parafile.str().c_str());
  parafile.str("");

  tofmongeo = new TofGeometryObject();
  parafile << parafile_dir << "/tofpanelgeo.txt";
  tofmongeo->fetchFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir << "/tofKnownDeadSlat.conf";
  ifstream deadslatfile;
  deadslatfile.open(parafile.str().c_str());
  knownDeadTofSlatTot = 0;
  for (int slatID = 0; slatID < TOF_NSLAT; slatID++)
    {
      knownDeadTofSlat[slatID] = 0;
      deadTofSlat[slatID]      = 0;
    }
  if (!deadslatfile)
    {
      cout << "TofMon: ERROR: in \"KnownDeadSlat.conf\" loading." << endl;
    }
  else
    {
      int slatID;
      int slatID2=0;
      while (deadslatfile)
        {
          deadslatfile >> slatID;
	  if(slatID2 != slatID)
	    {
	      slatID2=slatID;
	      if (slatID >= 0 && slatID < TOF_NSLAT)
		{
		  knownDeadTofSlatTot++;
		  
		  cout<<"knowDeadTofSlatTot"<< knownDeadTofSlatTot<<"   slatID"<<slatID<<endl;
		  
		  knownDeadTofSlat[(int)slatID] = 1;
		}
	      else
		{
		  cout << "error : in TOF parafile loading" << endl;
		}
	    }
	}
    }
  deadslatfile.close();
  
  // hist
  //////////////////////////
  htofx10 = new TH2F("htofx10", "TOF Dead Slat Map", 400, -200, 200, 400, -280, 90);
  htofx11 = new TH2F("htofx11", "erorr", 400, -200, 200, 400, -280, 90);
  htofx12 = new TH2F("htofx12", "erorr", 400, -200, 200, 400, -280, 90);

  w = new TH1F("wtof","",TOF_NSLAT,0,TOF_NSLAT);
  gtof530 = new TGraph(TOF_NSLAT);
  gtof530->SetTitle("MIP peak vs slat ID");

  peak00htof210 = new TLine(peak00htof210_v, 0., peak00htof210_v, 10000.);
  peak01htof210 = new TLine(peak01htof210_v, 0., peak01htof210_v, 10000.);
  peak00htof220 = new TLine(peak00htof220_v, 0., peak00htof220_v, 10000.);
  peak01htof220 = new TLine(peak01htof220_v, 0., peak01htof220_v, 10000.);
  peak00htof230 = new TLine(0., peak00htof230_v, TOF_NSLAT, peak00htof230_v );
  //peak00htof410 = new TLine(peak00htof410_v, 0., peak00htof410_v, 10000.);
  //peak01htof410 = new TLine(peak01htof410_v, 0., peak01htof410_v, 10000.);
  peak00htof420 = new TLine(peak00htof420_v, 0., peak00htof420_v, 10000.);
  peak01htof420 = new TLine(peak01htof420_v, 0., peak01htof420_v, 10000.);

  peak00htof210->SetLineStyle(2);
  peak01htof210->SetLineStyle(2);
  peak00htof220->SetLineStyle(2);
  peak01htof220->SetLineStyle(2);
  peak00htof230->SetLineStyle(2);
  //peak00htof410->SetLineStyle(2);
  //peak01htof410->SetLineStyle(2);
  peak00htof420->SetLineStyle(2);
  peak01htof420->SetLineStyle(2);

  peak00htof210->SetLineColor(2);
  peak01htof210->SetLineColor(2);
  peak00htof220->SetLineColor(2);
  peak01htof220->SetLineColor(2);
  peak00htof230->SetLineColor(2);
  //peak00htof410->SetLineColor(2);
  //  peak01htof410->SetLineColor(2);
  peak00htof420->SetLineColor(2);
  peak01htof420->SetLineColor(2);

  peak00htof210->SetLineWidth(2);
  peak01htof210->SetLineWidth(2);
  peak00htof220->SetLineWidth(2);
  peak01htof220->SetLineWidth(2);
  peak00htof230->SetLineWidth(2);
  //peak00htof410->SetLineWidth(2);
  //peak01htof410->SetLineWidth(2);
  peak00htof420->SetLineWidth(2);
  peak01htof420->SetLineWidth(2);

  //line for dead slat information
  tofl00 = new TLine( -200.,  -80., -200.,   80);
  tofl01 = new TLine( -150.,  -80., -150.,   80);
  tofl02 = new TLine( -100.,  -80., -100.,   80);
  tofl03 = new TLine(  -50.,  -80.,  -50.,   80);
  tofl04 = new TLine(    0.,  -80.,    0.,   80);
  tofl05 = new TLine(   50.,  -80.,   50.,   80);
  tofl06 = new TLine(  100.,  -80.,  100.,   80);
  tofl07 = new TLine(  150.,  -80.,  150.,   80);
  tofl08 = new TLine(  200.,  -80.,  200.,   80);
  tofl10 = new TLine( -200.,   80.,  200.,   80);
  tofl11 = new TLine( -200.,  -80.,  200.,  -80);
  tofl20 = new TLine(  -50., -270.,  -50., -120);
  tofl21 = new TLine(    0., -270.,    0., -120);
  tofl22 = new TLine(   50., -270.,   50., -120);
  tofl30 = new TLine(  -50., -120.,   50., -120);
  tofl31 = new TLine(  -50., -270.,   50., -270);

  //line for diviation of panel
  tof521 =new TLine(95.5      ,0,95.5      ,htof530_ymax);
  tof522 =new TLine(95.5+96   ,0,95.5+96   ,htof530_ymax);
  tof523 =new TLine(95.5+96*2.,0,95.5+96*2.,htof530_ymax);
  tof524 =new TLine(95.5+96*3.,0,95.5+96*3.,htof530_ymax);
  tof525 =new TLine(95.5+96*4.,0,95.5+96*4.,htof530_ymax);
  tof526 =new TLine(95.5+96*5.,0,95.5+96*5.,htof530_ymax);
  tof527 =new TLine(95.5+96*6.,0,95.5+96*6.,htof530_ymax);
  tof528 =new TLine(95.5+96*7.,0,95.5+96*7.,htof530_ymax);
  tof529 =new TLine(95.5+96*8.,0,95.5+96*8.,htof530_ymax);
  
  tof521->SetLineStyle(2);
  tof522->SetLineStyle(2);
  tof523->SetLineStyle(2);
  tof524->SetLineStyle(2);
  tof525->SetLineStyle(2);  
  tof526->SetLineStyle(2);  
  tof527->SetLineStyle(2);  
  tof528->SetLineStyle(2);  
  tof529->SetLineStyle(2);  

  tof521->SetLineColor(4);
  tof522->SetLineColor(4);
  tof523->SetLineColor(4);
  tof524->SetLineColor(4);
  tof525->SetLineColor(4);  
  tof526->SetLineColor(4);  
  tof527->SetLineColor(4);  
  tof528->SetLineColor(4);  
  tof529->SetLineColor(4);  

  tof521->SetLineWidth(1);
  tof522->SetLineWidth(1);
  tof523->SetLineWidth(1);
  tof524->SetLineWidth(1);
  tof525->SetLineWidth(1);  
  tof526->SetLineWidth(1);  
  tof527->SetLineWidth(1);  
  tof528->SetLineWidth(1);  
  tof529->SetLineWidth(1);  

  float ypos_text = htof530_ymax * 0.90;
  tx520=new TText(40      ,ypos_text,"A");
  tx521=new TText(40+96*1.,ypos_text,"B");
  tx522=new TText(40+96*2.,ypos_text,"C");
  tx523=new TText(40+96*3.,ypos_text,"D");
  tx524=new TText(40+96*4.,ypos_text,"F");
  tx525=new TText(40+96*5.,ypos_text,"G");
  tx526=new TText(40+96*6.,ypos_text,"H");
  tx527=new TText(40+96*7.,ypos_text,"I");
  tx528=new TText(40+96*8.,ypos_text,"E");
  tx529=new TText(40+96*9.,ypos_text,"J");
  
  tx520->SetTextSize(0.075);
  tx521->SetTextSize(0.075);
  tx522->SetTextSize(0.075);
  tx523->SetTextSize(0.075);
  tx524->SetTextSize(0.075);
  tx525->SetTextSize(0.075);
  tx526->SetTextSize(0.075);
  tx527->SetTextSize(0.075);  
  tx528->SetTextSize(0.075);
  tx529->SetTextSize(0.075);

  tx520->SetTextColor(4);
  tx521->SetTextColor(4);
  tx522->SetTextColor(4);
  tx523->SetTextColor(4);
  tx524->SetTextColor(4);
  tx525->SetTextColor(4);
  tx526->SetTextColor(4);
  tx527->SetTextColor(4);  
  tx528->SetTextColor(4);
  tx529->SetTextColor(4);

  toflabel    = new TPaveText(0.01, 0.91, 0.99, 0.97);
  tofmessage  = new TPaveText(0.71, 0.78, 0.99, 0.89);
  tofstatinfo = new TPaveText(0.71, 0.60, 0.99, 0.75);
  
  tofmessage ->SetTextSize(0.02);
  tofstatinfo->SetTextSize(0.02);
  return ;
}

TOFMonDraw::~TOFMonDraw()
{
  delete tofmongeo;
  delete tofmonaddress;
  delete w;

  delete htofx10;
  delete htofx11;
  delete htofx12;
  delete gtof530;
  delete peak00htof210;
  delete peak01htof210;
  delete peak00htof220;
  delete peak01htof220;
  delete peak00htof230;
  delete peak00htof420;
  delete peak01htof420;

  delete tofl00;
  delete tofl01;
  delete tofl02;
  delete tofl03;
  delete tofl04;
  delete tofl05;
  delete tofl06;
  delete tofl07;
  delete tofl08;
  delete tofl10;
  delete tofl11;
  delete tofl20;
  delete tofl21;
  delete tofl22;
  delete tofl30;
  delete tofl31;

  delete tof521;
  delete tof522;
  delete tof523;
  delete tof524;
  delete tof525;
  delete tof526;
  delete tof527;
  delete tof528;
  delete tof529;
  
  delete tx520;
  delete tx521;
  delete tx522;
  delete tx523;
  delete tx524;
  delete tx525;
  delete tx526;
  delete tx527;
  delete tx528;
  delete tx529;

  delete toflabel;
  delete tofmessage;
  delete tofstatinfo;
  return;
}

int TOFMonDraw::InitTofCanvas0()
{

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  TofCan0 = new TCanvas("TofCanvas0", "TOF Mon Granule Overview", -1, 0, xsize, ysize);
  gSystem->ProcessEvents();
  TofCan0->SetFillColor(29);
  TofPad[0][3] = new TPad("tofpad00", "tofpad00", 0.01, 0.60, 0.70, 0.88, 0);
  TofPad[0][1] = new TPad("tofpad01", "tofpad01", 0.01, 0.31, 0.50, 0.59, 0);
  TofPad[0][0] = new TPad("tofpad03", "tofpad03", 0.51, 0.31, 0.99, 0.59, 0);
  TofPad[0][2] = new TPad("tofpad02", "tofpad02", 0.01, 0.01, 0.99, 0.30, 0);
  TofPad[0][0]->Draw();
  TofPad[0][1]->Draw();
  TofPad[0][2]->Draw();
  TofPad[0][3]->Draw();
  return 0;
}
int TOFMonDraw::InitTofCanvas1()
{
  TofCan1 = new TCanvas("TofCanvas1", "TOF Mon Geo.", -10, 10, 1200, 600);
  gSystem->ProcessEvents();
  TofCan1->SetFillColor(29);
  TofPad[1][0] = new TPad("tofpad10", "tofpad10", 0.03, 0.03, 0.97, 0.97, 0);
  TofPad[1][0]->SetLeftMargin(0.05);
  TofPad[1][0]->SetRightMargin(0.05);
  TofPad[1][0]->Draw();
  return 0;
}
int TOFMonDraw::Draw(const char *what)
{
  if (! gROOT->FindObject("TofCanvas0"))
    {
      InitTofCanvas0();
    }
  if (canvasMode > 1)
    {
      if (! gROOT->FindObject("TofCanvas1"))
        {
          InitTofCanvas1();
        }
    }

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *htofinfo = cl->getHisto("htofinfo");
  TH1 *htof210 = cl->getHisto("htof210");
  TH1 *htof220 = cl->getHisto("htof220");
  TH1 *htof230 = cl->getHisto("htof230");
  //TH1 *htof410 = cl->getHisto("htof410");
  TH1 *htof420 = cl->getHisto("htof420");
  //TH2 *htof510 = (TH2 *)cl->getHisto("htof510");
  //TProfile *htof520 = (TProfile *)cl->getHisto("htof520");
  TProfile *htof530 = (TProfile *)cl->getHisto("htof530");
  
  htofx10->Reset();
  htofx11->Reset();
  htofx12->Reset();

  if (!(htofinfo && htofx10))
    {
      cout << "Server Dead" << endl;

      toflabel->Clear();
      toflabel->SetLabel("TOF ONLINE MONITOR");
      toflabel->AddText(" ");
      toflabel->AddText("Server Dead");
      toflabel->SetFillColor(20);

      // Label(Message)
      tofmessage->Clear();
      tofmessage->SetLabel("Message");
      tofmessage->AddText("Restart Server!");
      tofmessage->SetFillColor(5);

      TofCan0->cd();
      toflabel->Draw();
      tofmessage->Draw();

      return -1;
    }

  int n_0count[TOF_NPANEL];
  memset(n_0count, 0, sizeof(n_0count));
  
  for(int islat=0;islat<TOF_NSLAT;islat++){
    int nbin=htof530->GetBin(islat);
    double conbin=htof530->GetBinContent(nbin);
    //double errbin=htof530->GetBinError(islat);
    int ipan = (int)islat/96;
    
    if(knownDeadTofSlat[islat])//not to draw known dead slat
      {
	gtof530->SetPoint(islat,islat,-3);
      }else{
      if(conbin<Th_MIP) n_0count[ipan]++;
      gtof530->SetPoint(islat,islat,conbin);
      //gtof530->SetPointError(islat,0.5,errbin);
    }
  }
      int flag_message = 0;
      
      ostringstream otext;
      ostringstream tmptext;

      htof210->SetFillColor(38);
      htof220->SetFillColor(38);
      htof230->SetMarkerColor(9);
      htof230->SetMarkerStyle(6);
      //htof410->SetFillColor(38);
      htof420->SetFillColor(38);

      //htof520->SetMarkerColor(38);
      /*
      htof520->SetMarkerColor(2);
      htof520->SetMarkerStyle(20);
      htof520->SetMarkerSize(0.5);
      htof520->SetLineColor(2);
      htof520->SetLineWidth(2);
      */

      gtof530->SetMarkerColor(2);
      gtof530->SetMarkerStyle(20);
      gtof530->SetMarkerSize(0.5);

      htofx10->SetMarkerColor(30);
      htofx10->SetMarkerStyle(20);
      htofx11->SetMarkerColor(13);
      htofx11->SetMarkerStyle(20);
      htofx12->SetMarkerColor(2);
      htofx12->SetMarkerStyle(20);

      float label_size_x = 0.050;
      float label_size_y = 0.050;
      float title_size   = 0.050;

      htof210->GetXaxis()->SetTitle("TVC[ch]");
      htof210->GetXaxis()->SetTitleSize(title_size);
      htof210->GetXaxis()->SetLabelSize(label_size_x);
      htof210->GetXaxis()->SetLabelOffset(0);
      htof210->GetYaxis()->SetLabelSize(label_size_y);

      htof220->GetXaxis()->SetTitle("QVC[ch]");
      htof220->GetXaxis()->SetTitleSize(title_size);
      htof220->GetXaxis()->SetLabelSize(label_size_x);
      htof220->GetXaxis()->SetLabelOffset(0);
      htof220->GetYaxis()->SetLabelSize(label_size_y);

      htof230->GetXaxis()->SetLabelSize(label_size_x);
      htof230->GetXaxis()->SetLabelOffset(0);
      htof230->GetYaxis()->SetLabelSize(label_size_y);

      //htof410->GetXaxis()->SetLabelSize(label_size_x);
      //htof410->GetXaxis()->SetLabelOffset(0);
      //htof410->GetYaxis()->SetLabelSize(label_size_y);

      htof420->GetXaxis()->SetTitle("hit position[cm]");
      htof420->GetXaxis()->SetTitleSize(title_size);
      htof420->GetXaxis()->SetLabelSize(label_size_x);
      htof420->GetXaxis()->SetLabelOffset(0);
      htof420->GetYaxis()->SetLabelSize(label_size_y);

      //htof510->GetXaxis()->SetLabelSize(label_size_x);
      //htof510->GetXaxis()->SetLabelOffset(0);
      //htof510->GetYaxis()->SetLabelSize(label_size_y);

      //htof520->GetXaxis()->SetLabelSize(label_size_x);
      //htof520->GetXaxis()->SetLabelOffset(0);
      //htof520->GetYaxis()->SetLabelSize(label_size_y);

      gtof530->GetXaxis()->SetLabelSize(label_size_x);
      gtof530->GetXaxis()->SetLabelOffset(0);
      gtof530->GetYaxis()->SetLabelSize(label_size_y);

      w->SetTitle("MIP peak vs slatID");
      w->GetXaxis()->SetTitle("slatID");
      w->GetXaxis()->SetTitleSize(title_size);
      w->GetXaxis()->SetLabelSize(label_size_x);
      w->GetXaxis()->SetLabelOffset(0);
      w->GetYaxis()->SetTitle("MIP peak");
      w->GetYaxis()->SetTitleSize(title_size);
      w->GetYaxis()->SetLabelSize(label_size_y);


      // Event Status ( Info of TrigSelection & CutPara. ...)
      ///////////////////////////////////////////////////////
      int cutTrigLow  = (int)htofinfo->GetBinContent(1);
      int cutTrigHigh = (int)htofinfo->GetBinContent(2);
      int cutTofQVC   = (int)htofinfo->GetBinContent(3);
      int cutTofTc_0  = (int)htofinfo->GetBinContent(4);
      int cutTofTc    = (int)htofinfo->GetBinContent(5);
      float cutTofChg = (float)htofinfo->GetBinContent(6);
      int evtcnt      = (int)htofinfo->GetBinContent(7);
      evtTrigIn       = (int)htofinfo->GetBinContent(8);

      time_t evttime = cl->EventTime("CURRENT");


      // Cout
      cout << "Run: " << cl->RunNumber()
	   << " Time:" << ctime(&evttime);
      cout << "Event: " << evtTrigIn << "/" << evtcnt
	   << " [CutPara of Hit] "
	   << cutTrigLow << "< Trig <" << cutTrigHigh
	   << " TVC: " << cutTofTc
	   << " Chg: " << cutTofChg
	   << " [DrawingCut] "
	   << " TVC: " << cutTofTc_0
	   << " QVC: " << cutTofQVC << endl;

      // Label(Top)
      toflabel->Clear();
      toflabel->SetLabel("TOF ONLINE MONITOR");
      toflabel->SetFillColor(20);
      toflabel->AddText(" ");
      otext << " Run:" << cl->RunNumber();
      otext << " Time:" << ctime(&evttime) << "  ";
      toflabel->AddText(otext.str().c_str());

      // Label(Message)
      tofmessage->Clear();
      tofmessage->SetLabel("Message");
      tofmessage->AddText(" ");
      // Label(Status)
      tofstatinfo->Clear();
      tofstatinfo->SetLabel("Status");
      tofstatinfo->AddText(" ");

      // Label(Status)
      tmptext << "Event ( trig / all )";
      tofstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << evtTrigIn << " / " << evtcnt;
      tofstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << "[CutPara.of Hit]";
      tofstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << cutTrigLow << " <Trig< " << cutTrigHigh;
      tofstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      tmptext << "TVC: " << cutTofTc << " ch";
      tofstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      int saveprec = cout.precision();
      cout.precision(3);
      tmptext << "Charge: " << cutTofChg << " GeV";
      tofstatinfo->AddText(tmptext.str().c_str());
      tmptext.str("");
      cout .precision(saveprec);

      // Check Dead slat
      deadTofSlatTot = 0;
      if (evtTrigIn < evtTrigInMin){
	tmptext << "More Events!";
	cout << tmptext.str() << endl;
	tofmessage->AddText(tmptext.str().c_str());
	tofmessage->SetFillColor(5);
	tmptext.str("");
      }else{
	cout << "[ no hits slats ]" << endl;
	for (int slatID = 0; slatID < TOF_NSLAT; slatID++){
	  deadTofSlat[slatID] = 0;
	  if (knownDeadTofSlat[slatID] == 0){
	    // Known Dead Slat.
	    float nhit = htof230->GetBinContent(slatID + 1);
	    if (nhit < peak00htof230_v){
	      deadTofSlatTot++;
	      deadTofSlat[slatID] = 1;
	      cout << "Slat: " << slatID << ", Hit: " << nhit << endl;
	    }
	  }//if(known dead slat)
	}//for()
      }//if(trig/)
      
      if (deadTofSlatTot == 0){
	tmptext << "Fine.";
	cout << tmptext.str() << endl;
	tofmessage->AddText(tmptext.str().c_str());
	tofmessage->SetFillColor(30);
	tmptext.str("");
      }else{
	tmptext << "no hits slats: " << deadTofSlatTot;
	cout << tmptext.str() << endl;
	tofmessage->AddText(tmptext.str().c_str());
	tofmessage->SetFillColor(20);
	tofmessage->SetTextSize(0.02);
	tmptext.str("");
	
	//if( deadTofSlatTot / (TOF_NSLAT - (float)knownDeadTofSlatTot) * 100. > 1.)
	//  {
	//    tofmessage->Clear();
	//    tofmessage->SetLabel("Message");
	//    tofmessage->AddText(" ");
	//    tmptext << "TooMuchDead: " << deadTofSlatTot;
	//    tofmessage->AddText(tmptext.str().c_str());
	//    flag_message = 1;
	//  }

      }//if(deadTofSlatTot)


      //MIP peak flag
      if (evtTrigIn >evtTrigInMin_MIP)
	{
	  int Bad_MIP_Flag=0;
	  for(int ipan=0;ipan<TOF_NPANEL;ipan++)
	    {
	      if(n_0count[ipan]>No_MIP) Bad_MIP_Flag=1;
	    }//ipannel
	  
	  if(Bad_MIP_Flag==1)
	    {
	      tmptext << "Lots of small MIP peaks "<< endl;
	      tmptext << "Check Standing order!!";
	      cout << tmptext.str() << endl;
	      tofmessage->AddText(tmptext.str().c_str());
	      tofmessage->SetFillColor(kRed);
	      tofmessage->SetTextSize(0.02);
	      tmptext.str("");
	    }
	}
      
      // Peak Check!! //////////////////////////////////
      if (htof210)
	{
	  int max_high_bin = htof210->GetMaximumBin();
	  float max_bin_v = htof210->GetBinCenter(max_high_bin);
	  if (max_bin_v < peak00htof210_v || peak01htof210_v < max_bin_v)
	    {
	      tmptext.str("");
	      tmptext << "Check TVC Hist !!";
	      tofmessage->AddText(tmptext.str().c_str());
	      flag_message = 1;
	    }
	}
      if (htof220)
	{
	  int max_high_bin = htof220->GetMaximumBin();
	  float max_bin_v = htof220->GetBinCenter(max_high_bin);
	  if (max_bin_v < peak00htof220_v || peak01htof220_v < max_bin_v)
	    {
	      tmptext.str("");
	      tmptext << "Check Charge Hist !!";
	      tofmessage->AddText(tmptext.str().c_str());
	      flag_message = 1;
	    }
	}
      if (htof420)
	{
	  int max_high_bin = htof420->GetMaximumBin();
	  float max_bin_v = htof420->GetBinCenter(max_high_bin);
	  if (max_bin_v < peak00htof420_v || peak01htof420_v < max_bin_v)
	    {
	      tmptext.str("");
	      tmptext << "Check Position Hist !!";
	      tofmessage->AddText(tmptext.str().c_str());
	      flag_message = 1;
	    }
	}
      
      // Call TOF ??
      if (flag_message)
	{
/*	  tmptext.str("");
	  tmptext << "Pls Call Pager \"x2340\" NOW!";
	  cout << tmptext.str() << endl;
	  tofmessage->AddText(tmptext.str().c_str());*/
	  tofmessage->SetFillColor(2);

	}

      //}

      // Gem.
      for (int slatID = 0; slatID < TOF_NSLAT; slatID++)
        {
          float y = tofmongeo->getYpos(slatID);
          float z = tofmongeo->getZpos(slatID);
          htofx10->Fill(z, y);
          if (knownDeadTofSlat[slatID] == 1)
            {
              htofx11->Fill(z, y);
            }
          if (deadTofSlat[slatID] == 1)
            {
              htofx12->Fill(z, y);
            }
        }


      // Draw //////////////////////////////////////////
      TofCan0->cd();
      toflabel->Draw();
      tofmessage->Draw();
      tofstatinfo->Draw();

      //tofStyle->SetOptStat(0);
      //tofStyle->SetTitleW(0.5);
      //tofStyle->SetTitleH(0.1);
      //tofStyle->SetTitleY(0.99);

      //gStyle->SetTitleW(0.5);
      //gStyle->SetTitleH(0.1);
      //gStyle->SetTitleY(0.99);

      TofCan0->cd();
      TofPad[0][0]->cd();
      
      if (htof210)
        {
          htof210->Draw();
          int max_high_bin = htof210->GetMaximumBin();
          float max_high;
          if (max_high_bin > 0)
            {
              max_high = htof210->GetBinContent(max_high_bin);
            }
          else
            {
              max_high = 1000;
            }
          peak00htof210->SetY2(max_high);
          peak01htof210->SetY2(max_high);
          peak00htof210->Draw();
          peak01htof210->Draw();
        }
      TofPad[0][1]->cd();
      if (htof220)
        {
          htof220->Draw();
          int max_high_bin = htof220->GetMaximumBin();
          float max_high;
          if (max_high_bin > 0)
            {
              max_high = htof220->GetBinContent(max_high_bin);
            }
          else
            {
              max_high = 1000;
            }
          peak00htof220->SetY2(max_high);
          peak01htof220->SetY2(max_high);
          peak00htof220->Draw();
          peak01htof220->Draw();
        }
      TofPad[0][3]->cd();
      if (htof420)
        {
          htof420->Draw();
          int max_high_bin = htof420->GetMaximumBin();
          float max_high;
          if (max_high_bin > 0)
            {
              max_high = htof420->GetBinContent(max_high_bin);
            }
          else
            {
              max_high = 1000;
            }
          peak00htof420->SetY2(max_high);
          peak01htof420->SetY2(max_high);
          peak00htof420->Draw();
          peak01htof420->Draw();
         }
      TofPad[0][2]->cd();
      TofPad[0][2]->Clear();
      TofPad[0][2]->SetGridy();
      //TofPad[0][2]->SetGridx();
      if (gtof530)
        {
	  //htof510->Draw("COLZ");
	  //peak00htof230->Draw();]
	  
	  //htof520->Draw();
	  //htof520->SetMaximum(4000);
	  //htof520->SetMinimum(0);

	  w->SetMaximum(htof530_ymax);
	  w->SetMinimum(0);
	  w->SetStats(0000);
	  w->Draw();

	  tof521->Draw();
	  tof522->Draw();
	  tof523->Draw();
	  tof524->Draw();
	  tof525->Draw();
	  tof526->Draw();
	  tof527->Draw();
	  tof528->Draw();
	  tof529->Draw();

	  tx520->Draw();
	  tx521->Draw();
	  tx522->Draw();
	  tx523->Draw();
	  tx524->Draw();
	  tx525->Draw();
	  tx526->Draw();
	  tx527->Draw();
	  tx528->Draw();
	  tx529->Draw();

	  gtof530->Draw("P");
	  gtof530->SetMaximum(htof530_ymax);
	  gtof530->SetMinimum(0);

        }
      TofCan0->Update();

      if (canvasMode > 1)
        {
          TofCan1->cd();
          TofPad[1][0]->cd();
          if (htofx10 && htofx11 && htofx12)
            {
              htofx10->Draw();
              htofx11->Draw("same");
              htofx12->Draw("same");

              tofl00->Draw();
              tofl01->Draw();
              tofl02->Draw();
              tofl03->Draw();
              tofl04->Draw();
              tofl05->Draw();
              tofl06->Draw();
              tofl07->Draw();
              tofl08->Draw();

              tofl10->Draw();
              tofl11->Draw();

              tofl20->Draw();
              tofl21->Draw();
              tofl22->Draw();

              tofl30->Draw();
              tofl31->Draw();

            }
          TofCan1->Update();
        }
      
    
  return 0;
  
}

      int TOFMonDraw::MakePS(const char *what)
  {
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << Name() << "_" << cl->RunNumber() << ".ps";
  TofCan0->Print(filename.str().c_str());
  return 0;
}

  int TOFMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
    if (iret)
      {
        return iret;
      }
  OnlMonClient *cl = OnlMonClient::instance();

  string ofile = cl->htmlRegisterPage(*this,"Monitor","1","png");
  cl->CanvasToPng(TofCan0, ofile);
  ofile = cl->htmlRegisterPage(*this,"Text Summary","tofsummary","html");
  ofstream outfile(ofile.c_str());
  //ofstream outfile("/phenix/data20/mayap/tmp/tof_onlmon.html"); 

  outfile << "<HTML>" << endl;
  outfile << "<HEAD>" << endl;
  outfile << "<TITLE>TOF Monitor Run " << cl->RunNumber() << "</TITLE>" << endl;
  outfile << "</HEAD>" << endl;

  outfile << "<BODY text=\"#000000\" bgcolor=\"#FFFFFF\" link=\"#0000EE\" vlink=\"#551A8B\" alink=\"#FF0000\">" << endl;
  outfile << "<p><b><font face=\"Arial\" size=\"4\"><font color=\"#000099\">";
  outfile << "Run: " << cl->RunNumber() << "</font></font></b></p>" << endl;


  outfile << "<font face=\"Arial\" size=\"3\">" << endl;

  //  outfile << "<font color=\"#FF00FF\">Numbers of this color  </font>"<<"

  //is known that there is no hit. This is becasue we are currently exchanging the 
  //16 FEM boards (256 FEM channels = 128 slats) due to the incient of LV on Feb/6/2003. <br>"<<endl;

  outfile << "<p><ul><b>";
  outfile << "[ Known Dead Slat ]<br></b>" << endl;
  outfile << "total: " << knownDeadTofSlatTot << "<br>" << endl;
  for (int slatID = 0; slatID < TOF_NSLAT; slatID++)
    {
      if (knownDeadTofSlat[slatID])
        {
	  //	  if((slatID>=384 && slatID<=415)||(slatID>=480 && slatID<=511)||(slatID>=576 && slatID<=607)||(slatID>=672 && slatID<=703))
	  //	    { outfile << "<font color=\"#FF00FF\">"<<slatID<<"</font>"<<", ";
	  //	    }
	  //	  else
	  //        { 
	  outfile << slatID << ", ";
	  //	    }
        }
    }
  outfile << "</ul></p>" << endl;

  outfile << "<p><ul><b>";
  outfile << "[ No signal Slat(s) ]<br></b>" << endl;
  if (evtTrigIn < evtTrigInMin)
    {
      outfile << "<font color=\"#990000\">Need More Event!</font>" << endl;
    }
  else
    {
      outfile << "total: " << deadTofSlatTot << "<br>" << endl;
      for (int slatID = 0; slatID < TOF_NSLAT; slatID++)
        {
             if (deadTofSlat[slatID])
	           { 
	       //	       if((slatID>=384 && slatID<=415)||(slatID>=480 && slatID<=511)||(slatID>=576 && slatID<=607)||(slatID>=672 && slatID<=703))
	  //	      { outfile << "<font color=\"#FF00FF\">"<<slatID<<"</font>"<<", ";
	  //	      }
	  //	    else
	  //	      { 
		     outfile << slatID << ", ";
		//    }
		   }
        }
    }
  outfile << "</ul></p>" << endl;

  outfile << "</font>" << endl;

  outfile << "</body>" << endl;
  outfile << "</HTML>" << endl;
  outfile.close();
  cl->SaveLogFile(*this);
  
  return 0;
}
