//online monitoring includes
#include <RICHMonDraw.h>
#include <RICHMonDefs.h>
#include <OnlMonClient.h>


// offline includes
#include <phool.h>

// root includes
#include <TBox.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

// System includes
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <sstream>
#include <iostream>
#include <string>

//#define RICH_AuAu62
//#define RICH_AuAu39
//#define RICH_pp500
#define RICH_pp200

//#define BOOK_HISTOS 
#undef BOOK_HISTOS 

using namespace std;

RICHMonDraw::RICHMonDraw()
{
  ThisName = "RICHMON";
  rich_fadclivehist = 0;
  TStyle *os = gStyle;
  rs = new TStyle("richstyle", "rich online monitor style");
  rs->SetOptTitle(0);
  rs->SetOptStat(0);
  rs->SetFrameBorderMode(0);
  rs->SetPadBorderMode(0);
  os->cd();
  /*
  tdclegend[0]= new TLegend(0.15, 0.5, 0.5, 0.8);
  tdclegend[0]->SetFillColor(10);
  tdclegend[2]= new TLegend(0.15, 0.5, 0.5, 0.8);
  tdclegend[2]->SetFillColor(10);
  tdclegend[1]= new TLegend(0.15, 0.5, 0.5, 0.8);
  tdclegend[1]->SetFillColor(10);
  tdclegend[3]= new TLegend(0.15, 0.5, 0.5, 0.8);
  tdclegend[3]->SetFillColor(10);
  */
  
  tdctext[0]= new TText(0.45, 0.3, "ACCEPT AREA");
  tdctext[0]->SetNDC(10);
  tdctext[1]= new TText(0.45, 0.3, "ACCEPT AREA");
  tdctext[1]->SetNDC(10);
  tdctext[2]= new TText(0.45, 0.3, "ACCEPT AREA");  // 0.15, 0.7
  tdctext[2]->SetNDC(10);
  tdctext[3]= new TText(0.45, 0.3, "ACCEPT AREA");
  tdctext[3]->SetNDC(10);
    

  for (int i=0;i<4;i++)
    {
      box_rich_tdc[i]=new TBox(RICH_TACHIT_LOW, -0.3, RICH_TACHIT_HIGH, 1.);
      box_rich_tdc[i]->SetFillColor(5); 
      //      tdclegend[i]->AddEntry(box_rich_tdc[i],"ACCEPT AREA","f");

   }

  l[0]=new TLine(1280,0,1280,1021);
  l[0]->SetLineColor(3);
  l[0]->SetLineWidth(2);
  l[1]=new TLine(2560,0,2560,1021);
  l[1]->SetLineColor(3);
  l[1]->SetLineWidth(2);
  l[2]=new TLine(3840,0,3840,1021);
  l[2]->SetLineColor(3);
  l[2]->SetLineWidth(2);
  l[3]=new TLine(5120,0,5120,1021);
  l[3]->SetLineColor(3);
  l[3]->SetLineWidth(2);
  msg1 = 0;
  msg2 = 0;
  msg3 = 0;
  msg4 = 0;
  msg5 = 0;
  msg_tdc1 = 0;
  msg_tdc2 = 0;
  msg_tdc3 = 0;
  msg_tdc4 = 0;
  msg_tdc5 = 0;
  return ;
}

RICHMonDraw::~RICHMonDraw()
{
  delete rs;
  for (int i=0;i<4;i++)
    {
      delete l[i];
      delete tdctext[i];
      delete box_rich_tdc[i];
    }
  delete msg1;
  delete msg2;
  delete msg3;
  delete msg4;
  delete msg5;
  delete msg_tdc1;
  delete msg_tdc2;
  delete msg_tdc3;
  delete msg_tdc4;
  delete msg_tdc5;
  delete rich_fadclivehist;
  return;
}

int RICHMonDraw::Init()
{
  return 0;
}

int RICHMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  TStyle *os = gStyle;
  rs->cd();
  if (!strcmp(name, "RICHMon1"))
    {
      TC[0] = new TCanvas(name, "RICHMon Granule Overview of ADC", -1, 0, (int)(0.493*xsize), (int)(0.493*ysize));
      gSystem->ProcessEvents();

      Pad[0] = new TPad("rich_adclivepad", "rich_adclivehist",
                        0.05, 0.51, 0.49, 0.99, 0);
      Pad[1] = new TPad("rich_ringpad" , "photo-electrons per ring" ,
                        0.05, 0.01, 0.49, 0.49, 0);
      Pad[2] = new TPad("rich_adcsumpad" , "rich_adchistsum" ,
                        0.51, 0.51, 0.95, 0.99, 0);
      Pad[3] = new TPad("rich_msgpad" , "message pad" ,
                        0.51, 0.01, 0.95, 0.49, 0);
      Pad[0]->Draw();
      Pad[1]->Draw();
      Pad[2]->Draw();
      Pad[3]->Draw();


      Pad[1]->cd();
      subPad[1][0] =
        new TPad("rich_ringhistWS", "rich_ringhistWS", 0.05, 0.46, 0.49, 0.90, 0);
      subPad[1][1] =
        new TPad("rich_ringhistWN", "rich_ringhistWN", 0.05, 0.01, 0.49, 0.45, 0);
      subPad[1][2] =
        new TPad("rich_ringhistES", "rich_ringhistES", 0.51, 0.46, 0.95, 0.90, 0);
      subPad[1][3] =
        new TPad("rich_ringhistEN", "rich_ringhistEN", 0.51, 0.01, 0.95, 0.45, 0);

      subPad[1][0]->Draw();
      subPad[1][1]->Draw();
      subPad[1][2]->Draw();
      subPad[1][3]->Draw();

      Pad[2]->cd();

      subPad[2][0] =
        new TPad("rich_adcsumWS", "rich_adchistsumWS", 0.05, 0.46, 0.49, 0.90, 0);
      subPad[2][1] =
        new TPad("rich_adcsumWN", "rich_adchistsumWN", 0.05, 0.01, 0.49, 0.45, 0);
      subPad[2][2] =
        new TPad("rich_adcsumES", "rich_adchistsumES", 0.51, 0.46, 0.95, 0.90, 0);
      subPad[2][3] =
        new TPad("rich_adcsumEN", "rich_adchistsumEN", 0.51, 0.01, 0.95, 0.45, 0);

      subPad[2][0]->Draw();
      subPad[2][1]->Draw();
      subPad[2][2]->Draw();
      subPad[2][3]->Draw();

      Pad[0]->SetLogy();
      subPad[1][0]->SetLogy();
      subPad[1][1]->SetLogy();
      subPad[1][2]->SetLogy();
      subPad[1][3]->SetLogy();
      subPad[2][0]->SetLogy();
      subPad[2][1]->SetLogy();
      subPad[2][2]->SetLogy();
      subPad[2][3]->SetLogy();
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1 );
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();               
      
      TC[0]->Update();
    }

  else if (!strcmp(name, "RICHMon2"))
    {

      TC[1] = new TCanvas(name, "RICHMon Granule Overview of TDC", (int)(-0.5*xsize), 0, (int)(0.493*xsize), (int)(0.493*ysize));
      gSystem->ProcessEvents();

      Pad2[0] = new TPad("rich_tdclivepad", "rich_tdclivehist",
			 0.05, 0.51, 0.49, 0.99, 0);

      Pad2[1] = new TPad("rich_tdcsumpad" , "rich_tdchistsum" ,
                        0.51, 0.51, 0.95, 0.99, 0);

      Pad2[2] = new TPad("rich_pmt_vs_tdc", "rich_pmt_vs_tdc",
			 0.05, 0.01, 0.49, 0.49, 0);
      Pad2[3] = new TPad("rich_msgpad_tdc" , "message pad for tdc" ,
			 0.51, 0.01, 0.95, 0.49, 0);
      Pad2[0]->Draw();
      Pad2[1]->Draw();
      Pad2[2]->Draw();
      Pad2[3]->Draw();

      // ***** TDC sum **************//


      Pad2[1]->cd();

      subPad2[1][0] =
        new TPad("rich_tdcsumWS", "rich_tdchistsumWS", 0.05, 0.46, 0.49, 0.90, 0);
      subPad2[1][1] =
        new TPad("rich_tdcsumWN", "rich_tdchistsumWN", 0.05, 0.01, 0.49, 0.45, 0);
      subPad2[1][2] =
        new TPad("rich_tdcsumES", "rich_tdchistsumES", 0.51, 0.46, 0.95, 0.90, 0);
      subPad2[1][3] =
        new TPad("rich_tdcsumEN", "rich_tdchistsumEN", 0.51, 0.01, 0.95, 0.45, 0);
      
      Pad2[0]->SetLogy();
      subPad2[1][0]->Draw();
      subPad2[1][1]->Draw();
      subPad2[1][2]->Draw();
      subPad2[1][3]->Draw();
      subPad2[1][0]->SetLogy();
      subPad2[1][1]->SetLogy();
      subPad2[1][2]->SetLogy();
      subPad2[1][3]->SetLogy();

      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1 );
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();               

      TC[1]->Update();

    }
  else
    {
      cout << PHWHERE << " Unknown Canvas: " << name << endl;
    }
  os->cd();

  return 0;
}


int RICHMonDraw::Draw(const char *what)
{
  if (! gROOT->FindObject("RICHMon2"))
    {
      MakeCanvas("RICHMon2");
    }
  if (! gROOT->FindObject("RICHMon1"))
    {
      MakeCanvas("RICHMon1");
    }
  TStyle *os = gStyle;
  rs->cd();
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *rich_adclivehist = cl->getHisto("rich_adclivehist");
  if (! rich_adclivehist)
    {
      for (int it = 0; it < 2;it++)
	{
	  DrawDeadServer(transparent[it]);
	  TC[it]->Update();
	}
      return -1;
    }
  else
    {
      for (int it = 0; it < 2;it++)
	{
	  transparent[it]->cd();
	  transparent[it]->Clear();
	  transparent[it]->Update();
	}
  }

  TH1 *rich_tdclivehist = cl->getHisto("rich_tdclivehist");
  TH1 *rich_adchistsumWS = cl->getHisto("rich_adchistsumWS");
  TH1 *rich_adchistsumWN = cl->getHisto("rich_adchistsumWN");
  TH1 *rich_adchistsumES = cl->getHisto("rich_adchistsumES");
  TH1 *rich_adchistsumEN = cl->getHisto("rich_adchistsumEN");

  TH1 *rich_adc_second = cl->getHisto("rich_adc_second");
  TH1 *rich_adc_first = cl->getHisto("rich_adc_first");
  TH1 *rich_adc_hit = cl->getHisto("rich_adc_hit");

  TH1 *rich_adc_pmt = cl->getHisto("rich_adc_pmt");

  TH1 *rich_deadchinlivehist = cl->getHisto("rich_deadchinlivehist");
  TH1 *rich_par = cl->getHisto("rich_par");

  TH1 *rich_ringhist[4];
  rich_ringhist[0] = cl->getHisto("rich_ringhistWS");
  rich_ringhist[1] = cl->getHisto("rich_ringhistWN");
  rich_ringhist[2] = cl->getHisto("rich_ringhistES");
  rich_ringhist[3] = cl->getHisto("rich_ringhistEN");

  TH1 *rich_tdchistsumWS = cl->getHisto("rich_tdchistsumWS");
  TH1 *rich_tdchistsumWN = cl->getHisto("rich_tdchistsumWN");
  TH1 *rich_tdchistsumES = cl->getHisto("rich_tdchistsumES");
  TH1 *rich_tdchistsumEN = cl->getHisto("rich_tdchistsumEN");

  TH1 *rich_tdc_second = cl->getHisto("rich_tdc_second");
  TH1 *rich_tdc_first = cl->getHisto("rich_tdc_first");
  TH1 *rich_tdc_hit = cl->getHisto("rich_tdc_hit");


  TH1 *rich_tdc_pmt = cl->getHisto("rich_tdc_pmt");

  TH1 *rich_eventcounter = cl->getHisto("rich_eventcounter");
  int event = rich_eventcounter->GetEntries();

  TH1 *rich_nhit_eventWS = cl->getHisto("rich_nhit_eventWS");
  TH1 *rich_nhit_eventWN = cl->getHisto("rich_nhit_eventWN");
  TH1 *rich_nhit_eventES = cl->getHisto("rich_nhit_eventES");
  TH1 *rich_nhit_eventEN = cl->getHisto("rich_nhit_eventEN");
 
  // For Preamp power check purpose
  TH1 *rich_nhit_eventWS1 = cl->getHisto("rich_nhit_eventWS1");
  TH1 *rich_nhit_eventWS2 = cl->getHisto("rich_nhit_eventWS2");
  TH1 *rich_nhit_eventWS3 = cl->getHisto("rich_nhit_eventWS3");
  TH1 *rich_nhit_eventWS4 = cl->getHisto("rich_nhit_eventWS4");

  TH1 *rich_nhit_eventWN1 = cl->getHisto("rich_nhit_eventWN1");
  TH1 *rich_nhit_eventWN2 = cl->getHisto("rich_nhit_eventWN2");
  TH1 *rich_nhit_eventWN3 = cl->getHisto("rich_nhit_eventWN3");
  TH1 *rich_nhit_eventWN4 = cl->getHisto("rich_nhit_eventWN4");

  TH1 *rich_nhit_eventES1 = cl->getHisto("rich_nhit_eventES1");
  TH1 *rich_nhit_eventES2 = cl->getHisto("rich_nhit_eventES2");
  TH1 *rich_nhit_eventES3 = cl->getHisto("rich_nhit_eventES3");
  TH1 *rich_nhit_eventES4 = cl->getHisto("rich_nhit_eventES4");

  TH1 *rich_nhit_eventEN1 = cl->getHisto("rich_nhit_eventEN1");
  TH1 *rich_nhit_eventEN2 = cl->getHisto("rich_nhit_eventEN2");
  TH1 *rich_nhit_eventEN3 = cl->getHisto("rich_nhit_eventEN3");
  TH1 *rich_nhit_eventEN4 = cl->getHisto("rich_nhit_eventEN4");

  int tripws = 0;
  int tripwn = 0;
  int tripes = 0;
  int tripen = 0;

  double hitnum_ws = 0.;
  double hitnum_wn = 0.;
  double hitnum_es = 0.;
  double hitnum_en = 0.;

  // For Preamp power check purpose
  double hitnum_ws1 = 0.;
  double hitnum_ws2 = 0.;
  double hitnum_ws3 = 0.;
  double hitnum_ws4 = 0.;

  double hitnum_wn1 = 0.;
  double hitnum_wn2 = 0.;
  double hitnum_wn3 = 0.;
  double hitnum_wn4 = 0.;

  double hitnum_es1 = 0.;
  double hitnum_es2 = 0.;
  double hitnum_es3 = 0.;
  double hitnum_es4 = 0.;

  double hitnum_en1 = 0.;
  double hitnum_en2 = 0.;
  double hitnum_en3 = 0.;
  double hitnum_en4 = 0.;

  if( event > 10000 ){
    /*
      for(int i=0;i<1280;i++)    hitnum_ws += rich_adc_hit->GetBinContent(i+1);
      for(int i=1280;i<2560;i++) hitnum_wn += rich_adc_hit->GetBinContent(i+1);
      for(int i=2560;i<3840;i++) hitnum_es += rich_adc_hit->GetBinContent(i+1);
      for(int i=3840;i<5120;i++) hitnum_en += rich_adc_hit->GetBinContent(i+1);
    */

    /// sum # of hits in the sector over recent 10,000 events
    /// rich_nhit_event** have 1,000 bins and one of bin contents
    /// corresponds to # of hits is 10 events.
    /// i.e. rich_nhit_event** have informations of # of hits of recent
    /// 10,000 events
    for(int ibin=1; ibin<=1000; ibin++){
      hitnum_ws += rich_nhit_eventWS->GetBinContent(ibin);
      hitnum_wn += rich_nhit_eventWN->GetBinContent(ibin);
      hitnum_es += rich_nhit_eventES->GetBinContent(ibin);
      hitnum_en += rich_nhit_eventEN->GetBinContent(ibin);

      // For Preamp power check purpose
      hitnum_ws1 += rich_nhit_eventWS1->GetBinContent(ibin);
      hitnum_ws2 += rich_nhit_eventWS2->GetBinContent(ibin);
      hitnum_ws3 += rich_nhit_eventWS3->GetBinContent(ibin);
      hitnum_ws4 += rich_nhit_eventWS4->GetBinContent(ibin);
 
//      hitnum_ws4 = 0.0;

      hitnum_wn1 += rich_nhit_eventWN1->GetBinContent(ibin);
      hitnum_wn2 += rich_nhit_eventWN2->GetBinContent(ibin);
      hitnum_wn3 += rich_nhit_eventWN3->GetBinContent(ibin);
      hitnum_wn4 += rich_nhit_eventWN4->GetBinContent(ibin);

      hitnum_es1 += rich_nhit_eventES1->GetBinContent(ibin);
      hitnum_es2 += rich_nhit_eventES2->GetBinContent(ibin);
      hitnum_es3 += rich_nhit_eventES3->GetBinContent(ibin);
      hitnum_es4 += rich_nhit_eventES4->GetBinContent(ibin);

      hitnum_en1 += rich_nhit_eventEN1->GetBinContent(ibin);
      hitnum_en2 += rich_nhit_eventEN2->GetBinContent(ibin);
      hitnum_en3 += rich_nhit_eventEN3->GetBinContent(ibin);
      hitnum_en4 += rich_nhit_eventEN4->GetBinContent(ibin);
    }

#ifdef RICH_AuAu62
    double normal_ws = 5.58794;
    double normal_wn = 5.65894;
    double normal_es = 5.61807;
    double normal_en = 5.25511;
#endif 

#ifdef RICH_AuAu39
    double normal_ws = 5.04508;
    double normal_wn = 5.15819;
    double normal_es = 5.06781;
    double normal_en = 4.6877;
#endif 

#ifdef RICH_pp500
    double normal_ws = 0.2477;
    double normal_wn = 0.2692;
    double normal_es = 0.2075;
    double normal_en = 0.2358;
#endif 

#ifdef RICH_pp200
    double normal_ws = 0.28;
    double normal_wn = 0.38;
    double normal_es = 0.25;
    double normal_en = 0.28;
#endif 

    //  < Less than 60 % of typical hits/events  >
    //   Detection of high/low voltage trip
    // 
    /*
      if(hitnum_ws/event < normal_ws * 0.6) tripws = 1;
      if(hitnum_wn/event < normal_wn * 0.6) tripwn = 1;
      if(hitnum_es/event < normal_es * 0.6) tripes = 1;
      if(hitnum_en/event < normal_en * 0.6) tripen = 1;
    */
    if(hitnum_ws/10000 < normal_ws * 0.6) tripws = 1;
    if(hitnum_wn/10000 < normal_wn * 0.6) tripwn = 1;
    if(hitnum_es/10000 < normal_es * 0.6) tripes = 1;
    if(hitnum_en/10000 < normal_en * 0.6) tripen = 1;    

    // For Preamp power check purpose
    if(hitnum_ws1/10000 < normal_ws * 0.6 * 0.25) tripws = 1;
    if(hitnum_ws2/10000 < normal_ws * 0.6 * 0.25) tripws = 1;
    if(hitnum_ws3/10000 < normal_ws * 0.6 * 0.25) tripws = 1;
    if(hitnum_ws4/10000 < normal_ws * 0.6 * 0.25) tripws = 1;

    /*
    if(hitnum_wn1/10000 < normal_wn * 0.6 * 0.25) tripwn = 1;
    if(hitnum_wn2/10000 < normal_wn * 0.6 * 0.25) tripwn = 1;
    if(hitnum_wn3/10000 < normal_wn * 0.6 * 0.25) tripwn = 1;
    if(hitnum_wn4/10000 < normal_wn * 0.6 * 0.25) tripwn = 1;

    if(hitnum_es1/10000 < normal_es * 0.6 * 0.25) tripes = 1;
    if(hitnum_es2/10000 < normal_es * 0.6 * 0.25) tripes = 1;
    if(hitnum_es3/10000 < normal_es * 0.6 * 0.25) tripes = 1;
    if(hitnum_es4/10000 < normal_es * 0.6 * 0.25) tripes = 1;

    if(hitnum_en1/10000 < normal_en * 0.6 * 0.25) tripen = 1;    
    if(hitnum_en2/10000 < normal_en * 0.6 * 0.25) tripen = 1;    
    if(hitnum_en3/10000 < normal_en * 0.6 * 0.25) tripen = 1;    
    if(hitnum_en4/10000 < normal_en * 0.6 * 0.25) tripen = 1;    
    */

  }

  int pmt;
  float pmthit[5120];
  float pmttac[5120];
  float pmttac2[5120];
  float pmtadchit[5120];
  float pmtadc[5120];
  float pmtadc2[5120];

  for(int bin=0;bin<5120;bin++){
    pmt = bin;
    pmthit[pmt] = rich_tdc_hit->GetBinContent(bin+1);
    pmttac[pmt] = rich_tdc_first->GetBinContent(bin+1);
    pmttac2[pmt] = rich_tdc_second->GetBinContent(bin+1);

    if(pmthit[pmt]<=0){
      rich_tdc_pmt -> SetBinContent(pmt+1,0);
      rich_tdc_pmt -> SetBinError(pmt+1,0);
    }else{
      rich_tdc_pmt -> SetBinContent(pmt+1,pmttac[pmt]/((float)pmthit[pmt]));
      rich_tdc_pmt -> SetBinError(pmt+1,sqrt(pmttac2[pmt]/((float)pmthit[pmt])-pow(pmttac[pmt]/((float)pmthit[pmt]),2)));
    }
    pmtadchit[pmt] = rich_adc_hit->GetBinContent(bin+1);
    pmtadc[pmt] = rich_adc_first->GetBinContent(bin+1);
    pmtadc2[pmt] = rich_adc_second->GetBinContent(bin+1);
    if(pmtadchit[pmt]<=0){
      rich_adc_pmt -> SetBinContent(pmt+1,0);
      rich_adc_pmt -> SetBinError(pmt+1,0);
    }else{
      rich_adc_pmt -> SetBinContent(pmt+1,pmtadc[pmt]/((float)pmtadchit[pmt]));
      rich_adc_pmt -> SetBinError(pmt+1,sqrt(pmtadc2[pmt]/((float)pmtadchit[pmt])-pow(pmtadc[pmt]/((float)pmtadchit[pmt]),2)));
    }
  }
    
  TText title;
  title.SetNDC(); // set to normalized coordinates
  Pad[1]->cd();
  title.SetTextAlign(23); // center/top alignment
  title.SetTextSize(0.06); 
  title.DrawText(0.5, 0.99, "photo-electrons per ring for each sector");
  title.SetTextSize(0.2);
  title.SetTextAlign(33); // right/top alignment
  for (int irnp = 0;irnp < 4;irnp++)
    {
      if (rich_ringhist[0])
        {
          subPad[1][irnp]->cd();
          rich_ringhist[irnp]->SetXTitle("npe");
	  if((rich_ringhist[irnp]->GetEntries())>0){
	    rich_ringhist[irnp]->Draw();
	  }else{
	    rich_ringhist[irnp]->Fill((float)0,(float)rich_par->GetBinContent(1));
	    rich_ringhist[irnp]->Draw();
	  }
          switch (irnp)
            {
            case 0:
              title.DrawText(0.9, 0.87, "W.S.");
              break;
            case 1:
              title.DrawText(0.9, 0.87, "W.N.");
              break;
            case 2:
              title.DrawText(0.9, 0.87, "E.S.");
              break;
            case 3:
              title.DrawText(0.9, 0.87, "E.N.");
              break;
            }

        }
    }

  if (!(rich_adclivehist && rich_adchistsumWS && rich_par))
    {
      Pad[3]->cd();
      msg1 = new TPaveText(0.05, 0.87, 0.95, 0.99);
      msg1->SetFillColor(5);
      msg1->AddText("Server is not running!");
      msg1->Draw();
      TC[0]->Update();
      return 1; 
    }

  if (!(rich_tdclivehist && rich_tdchistsumWS && rich_par))
    {
      Pad2[3]->cd();
      msg_tdc1 = new TPaveText(0.05, 0.87, 0.95, 0.99);
      msg_tdc1->SetFillColor(5);
      msg_tdc1->AddText("Server is not running!");
      msg_tdc1->Draw();
      TC[1]->Update();
      return 1;
    }

  int Format = 1006;
  int run = -1;
  if (rich_par)
    {
      Format = (int)rich_par->GetBinContent(6);
      run = (int)rich_par->GetBinContent(7);
    }

  int nspike = 0;
  int spikech[5120];
  float mid = 1;
  if (rich_adclivehist)
    {
      if (rich_fadclivehist)
        {
          delete rich_fadclivehist;
        }

      rich_fadclivehist = new TH2F("rich_fadclivehist",
                                   "N of hit of each RICH PMT on ADC",
                                   1, 0, RICH_NCHANNEL, 1, 0.5,
                                   rich_adclivehist->GetMaximum() * 2);
      rich_fadclivehist->SetXTitle("PMT #");

      // Spike detection
      float hitmat[5120];
      for (int ich = 0;ich < 5120;ich++)
        {
          hitmat[ich] = rich_adclivehist->GetBinContent(ich + 1);
        }
      sort(hitmat, hitmat + 5120);
      mid = hitmat[2560];
      if (mid <= 0)
	{
        mid = 1;
	}
      for (int ich = 0;ich < 5120;ich++)
        {
          // If hit count exceeds error * 50, it is a spike.
          if (sqrt(mid) * RICH_SPIKE_DEF <
              fabs(rich_adclivehist->GetBinContent(ich + 1) - mid) &&
              rich_deadchinlivehist->GetBinContent(ich + 1) == 0)
            {
              spikech[nspike] = ich;
              nspike++;
            }
        }
    }
  else if (rich_tdclivehist)
    {
      if (rich_fadclivehist)
        {
          delete rich_fadclivehist;
        }
      rich_fadclivehist = new TH2F("rich_fadclivehist",
                                   "N of hit of each RICH PMT on ADC",
                                   1, 0, RICH_NCHANNEL, 1, 0.5,
                                   rich_tdclivehist->GetMaximum() * 2);
      rich_fadclivehist->SetXTitle("PMT #");
    }

  int adcstatws = 0;
  int adcstatwn = 0;
  int adcstates = 0;
  int adcstaten = 0;

  int tdcstatws = 0;
  int tdcstatwn = 0;
  int tdcstates = 0;
  int tdcstaten = 0;


  Pad[0]->cd();
  Pad[0]->Clear();
  TText Title;
  Title.SetNDC();  // set to normalized coordinates
  Title.SetTextAlign(13);
  if (rich_adclivehist)
    {
      rich_fadclivehist->Draw();
      if (rich_deadchinlivehist)
        {
          rich_deadchinlivehist->Draw("same");
        }
      rich_adclivehist->Draw("same");
      Title.DrawText(0.1, 0.95,
                     "N of hit of each RICH PMT on ADC");
    }
  else
    {
      Title.DrawText(0.1, 0.95,
                     "server is not running or histo is not present");
    }
  Pad[0]->Update();

  Pad2[0]->cd();
  Pad2[0]->Clear();
  if (rich_tdclivehist)
    {
      rich_fadclivehist->Draw();
      if (rich_deadchinlivehist)
        {
          rich_deadchinlivehist->Draw("same");
        }
      rich_tdclivehist->Draw("same");
      Title.DrawText(0.1, 0.95,
                     "N of hit of each RICH PMT on TDC");
    }
  else
    {
      Title.DrawText(0.1, 0.95,
                     "server is not running or histo is not present");
    }

  TText SecTitle;
  SecTitle.SetNDC();  // set to normalized coordinates
  Pad[2]->cd();
  SecTitle.SetTextAlign(23); // center/top alignment
  SecTitle.SetTextSize(0.07);
  SecTitle.DrawText(0.5, 0.95, "ADC sum of Each sector");

  SecTitle.SetTextAlign(33); // right/top alignment
  SecTitle.SetTextSize(0.2);
  subPad[2][0]->cd();
  subPad[2][0]->Clear();
  subPad[2][0]->SetFillColor(10);
  if (rich_adchistsumWS)
    {
      if (Format == 1106)
        {
          adcstatws = chk_adcsum0sup(rich_adchistsumWS);
        }
      else
        {
          adcstatws = chk_adcsum(rich_adchistsumWS);
        }
      rich_adchistsumWS->SetAxisRange( -150, 900);
      rich_adchistsumWS->SetLabelSize(0.08);
      rich_adchistsumWS->SetXTitle("npe");
      rich_adchistsumWS->Draw();
      SecTitle.DrawText(0.9, 0.85, "W.S.");

    }
  else
    {
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "W.S. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  subPad[2][1]->cd();
  subPad[2][1]->Clear();
  subPad[2][1]->SetFillColor(10);
  if (rich_adchistsumWN)
    {
      if (Format == 1106)
        {
          adcstatwn = chk_adcsum0sup(rich_adchistsumWN);
        }
      else
        {
          adcstatwn = chk_adcsum(rich_adchistsumWN);
        }
      rich_adchistsumWN->SetAxisRange( -150, 900);
      rich_adchistsumWN->SetLabelSize(0.08);
      rich_adchistsumWN->SetXTitle("npe");
      rich_adchistsumWN->Draw();
      SecTitle.DrawText(0.9, 0.85, "W.N.");
    }
  else
    {
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "W.N. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  subPad[2][2]->cd();
  subPad[2][2]->Clear();
  subPad[2][2]->SetFillColor(10);
  if (rich_adchistsumES)
    {
      if (Format == 1106)
        {
          adcstates = chk_adcsum0sup(rich_adchistsumES);
        }
      else
        {
          adcstates = chk_adcsum(rich_adchistsumES);
        }
      rich_adchistsumES->SetAxisRange( -150, 900);
      rich_adchistsumES->SetLabelSize(0.08);
      rich_adchistsumES->SetXTitle("npe");
      rich_adchistsumES->Draw();
      SecTitle.DrawText(0.9, 0.85, "E.S.");
    }
  else
    {
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "E.S. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  subPad[2][3]->cd();
  subPad[2][3]->Clear();
  subPad[2][3]->SetFillColor(10);
  if (rich_adchistsumEN)
    {
      if (Format == 1106)
        {
          adcstaten = chk_adcsum0sup(rich_adchistsumEN);
        }
      else
        {
          adcstaten = chk_adcsum(rich_adchistsumEN);
        }
      rich_adchistsumEN->SetAxisRange( -150, 900);
      rich_adchistsumEN->SetLabelSize(0.08);
      rich_adchistsumEN->SetXTitle("npe");
      rich_adchistsumEN->Draw();
      SecTitle.DrawText(0.9, 0.85, "E.N.");
    }
  else
    {
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "E.N. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  // ************ tdc part *************// 

  TC[1]->cd();

  float maxcount_tdc;
  float meantdcWS=-100;
  float meantdcWN=-100;
  float meantdcES=-100;
  float meantdcEN=-100;

  SecTitle.SetNDC();  // set to normalized coordinates
  Pad2[1]->cd();
  SecTitle.SetTextAlign(23); // center/top alignment
  SecTitle.SetTextSize(0.07);
  SecTitle.DrawText(0.5, 0.95, "TDC sum of Each sector");

  subPad2[1][0]->cd();
  subPad2[1][0]->Clear();
  subPad2[1][0]->SetFillColor(10);
  SecTitle.SetTextAlign(33); // right/top alignment
  SecTitle.SetTextSize(0.15);
  if (rich_tdchistsumWS)
    {
      if(rich_tdchistsumWS->GetEntries()==0){
	rich_tdchistsumWS->Fill((float)-600,(float)rich_par->GetBinContent(1));
      }
      tdcstatws = chk_tdcsum(rich_tdchistsumWS);
      maxcount_tdc = rich_tdchistsumWS->GetMaximum();
      rich_tdchistsumWS->SetMinimum(0.5);
      rich_tdchistsumWS->SetMaximum(1.5*maxcount_tdc);
      rich_tdchistsumWS->SetXTitle("TDC chanel");
      rich_tdchistsumWS->Draw();
      meantdcWS = rich_tdchistsumWS->GetMean();
      //box_rich_tdc[0]->SetY2(log10(1.45*maxcount_tdc));
      box_rich_tdc[0]->SetY2(1.45*maxcount_tdc);
      box_rich_tdc[0]->Draw("same");
      //      tdclegend[0]->Draw("same");
      tdctext[0]->Draw("same");
      rich_tdchistsumWS->Draw("same");
      SecTitle.DrawText(0.9, 0.85, "W.S.");
    }
  else
    {
      //cout<<"a"<<endl;
      tdcstatws = 130;
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "W.S. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  subPad2[1][2]->cd();
  subPad2[1][2]->Clear();
  subPad2[1][2]->SetFillColor(10);
  SecTitle.SetTextAlign(33); // right/top alignment
  SecTitle.SetTextSize(0.15);
  if (rich_tdchistsumES)
    {
      if(rich_tdchistsumES->GetEntries()==0){
	rich_tdchistsumES->Fill((float)-600,(float)rich_par->GetBinContent(1));
      }
      tdcstates = chk_tdcsum(rich_tdchistsumES);
      maxcount_tdc = rich_tdchistsumES->GetMaximum();
      rich_tdchistsumES->SetMinimum(0.5);
      rich_tdchistsumES->SetMaximum(1.5*maxcount_tdc);
      rich_tdchistsumES->SetXTitle("TDC chanel");
      rich_tdchistsumES->Draw();
      meantdcES = rich_tdchistsumES->GetMean();
      //      box_rich_tdc[2]->SetY2(log10(1.45*maxcount_tdc));
      box_rich_tdc[2]->SetY2(1.45*maxcount_tdc);


      box_rich_tdc[2]->Draw("same");
      //      tdclegend[2]->Draw("same");
      tdctext[2]->Draw("same");
      SecTitle.DrawText(0.9, 0.85, "E.S.");
      rich_tdchistsumES->Draw("same");
    }
  else
    {
      tdcstates = 130;
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "E.S. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  subPad2[1][1]->cd();
  subPad2[1][1]->Clear();
  subPad2[1][1]->SetFillColor(10);
  SecTitle.SetTextAlign(33); // right/top alignment
  SecTitle.SetTextSize(0.15);
  if (rich_tdchistsumWN)
    {
      if(rich_tdchistsumWN->GetEntries()==0){
	rich_tdchistsumWN->Fill((float)-600,(float)rich_par->GetBinContent(1));
      }
      tdcstatwn = chk_tdcsum(rich_tdchistsumWN);
      maxcount_tdc = rich_tdchistsumWN->GetMaximum();
      rich_tdchistsumWN->SetMaximum(1.5*maxcount_tdc);
      rich_tdchistsumWN->SetMinimum(0.5);
      rich_tdchistsumWN->SetXTitle("TDC channel");
      rich_tdchistsumWN->Draw();
      meantdcWN  = rich_tdchistsumWN->GetMean();
      //box_rich_tdc[1]->SetY2(log10(1.45*maxcount_tdc));
      box_rich_tdc[1]->SetY2(1.45*maxcount_tdc);
      box_rich_tdc[1]->Draw("same");
      //      tdclegend[1]->Draw("same");
      tdctext[1]->Draw("same");
      rich_tdchistsumWN->Draw("same");
      SecTitle.DrawText(0.9, 0.85, "W.N.");

    }
  else
    {
      tdcstatwn = 130;
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "W.N. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }
  
  subPad2[1][3]->cd();
  subPad2[1][3]->Clear();
  subPad2[1][3]->SetFillColor(10);
  SecTitle.SetTextAlign(33); // right/top alignment
  SecTitle.SetTextSize(0.15);
  if (rich_tdchistsumEN)
    {
      if(rich_tdchistsumEN->GetEntries()==0){
	rich_tdchistsumEN->Fill((float)-600,(float)rich_par->GetBinContent(1));
      }
      tdcstaten = chk_tdcsum(rich_tdchistsumEN);
      maxcount_tdc = rich_tdchistsumEN->GetMaximum();
      rich_tdchistsumEN->SetMaximum(1.5*maxcount_tdc);
      rich_tdchistsumEN->SetMinimum(0.5);
      rich_tdchistsumEN->SetXTitle("TDC chanel");
      rich_tdchistsumEN->Draw();
      meantdcEN  = rich_tdchistsumEN->GetMean();
      //box_rich_tdc[3]->SetY2(log10(1.45*maxcount_tdc));
      box_rich_tdc[3]->SetY2(1.45*maxcount_tdc);
      box_rich_tdc[3]->Draw("same");
      //      tdclegend[3]->Draw("same");
      tdctext[3]->Draw("same");
      rich_tdchistsumEN->Draw("same");
      SecTitle.DrawText(0.9, 0.85, "E.N."); 

    }
  else
    {
      tdcstaten = 130;
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "E.N. histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  

  Pad2[2]->cd();
  Title.SetNDC();  // set to normalized coordinates
  Title.SetTextAlign(13);


  TText lsector[4];
  char secname[10];

  if (rich_tdc_pmt)
    {
      rich_tdc_pmt->SetXTitle("PMT #");
      rich_tdc_pmt->SetLineColor(4);
      rich_tdc_pmt->SetMarkerStyle(4);
      rich_tdc_pmt->SetMarkerColor(2);
      rich_tdc_pmt->SetMarkerSize(0.2);
      rich_tdc_pmt->SetMinimum(0);
      rich_tdc_pmt->SetMaximum(1023);
      rich_tdc_pmt->Draw("BOXES");
      Title.DrawText(0.1, 0.95,
                     "TDC MAP of each RICH PMT");
      for(int i=0;i<4;i++){
	switch(i){
	case 0:
	  sprintf(secname,"W.S");
	  break;
	case 1:
	  sprintf(secname,"W.N");
	  break;
	case 2:
	  sprintf(secname,"E.S");	
	  break;
	case 3:
	  sprintf(secname,"E.N");
	  break;
	}
	l[i]->Draw("same");
	lsector[i].SetTextAlign(13);
	lsector[i].SetTextSize(0.07);
	lsector[i].DrawText(1280*i+200, 1000, secname);
      }
    }
  else
    {
      SecTitle.SetTextAlign(22); // center/center alignment
      SecTitle.SetTextSize(0.1);
      SecTitle.DrawText(0.99, 0.5, "PMT vs TDC histo is not present");
      SecTitle.SetTextSize(0.2);
      SecTitle.SetTextAlign(33); // right/top alignment
    }

  // ************ end of TDC part ************//

  // ************ error message *************//

  int adc_good=1;
  int tdc_good=1;
  


  Pad[3]->cd();

  ostringstream str_msg;
  ostringstream str_msg_tdc;

  if (msg1)
    {
      delete msg1;
      msg1 = NULL;
    }
  msg1 = new TPaveText(0.05, 0.87, 0.95, 0.99);

  if (msg2)
    {
      delete msg2;
      msg2 = NULL;
    }
  if (msg3)
    {
      delete msg3;
      msg3 = NULL;
    }
  if (msg4)
    {
      delete msg4;
      msg4 = NULL;
    }
  if (msg5)
    {
      delete msg5;
      msg5 = NULL;
    }

  if(msg_tdc1){
    delete msg_tdc1;
    msg_tdc1 = NULL;
  }
  msg_tdc1 = new TPaveText(0.05, 0.87, 0.95, 0.99);
  if(msg_tdc2){
    delete msg_tdc2;
    msg_tdc2 = NULL;
  }
  if(msg_tdc3){
    delete msg_tdc3;
    msg_tdc3 = NULL;
  }
  if(msg_tdc4){
    delete msg_tdc4;
    msg_tdc4 = NULL;
  }
  if(msg_tdc5){
    delete msg_tdc5;
    msg_tdc5 = NULL;
  }


  msg1->SetFillColor(10);
  str_msg << "Run #"<<run;
  msg1->AddText(str_msg.str().c_str());

  msg_tdc1->SetFillColor(10);
  str_msg_tdc<< "Run #"<<run;
  msg_tdc1->AddText(str_msg_tdc.str().c_str());

  msg3 = new TPaveText(0.05, 0.7, 0.95, 0.85);
  msg_tdc3 = new TPaveText(0.05, 0.7, 0.95, 0.85);

  str_msg.str("");
  str_msg_tdc.str("");

  if (rich_par->GetBinContent(1) > 1000.)
    {
      if (rich_par->GetBinContent(2)/rich_par->GetBinContent(1) >
          PERMIT_RICH_PACKET_ERROR)
        {
          msg3->SetFillColor(2);
          msg3->AddText("Error #1");
          msg3->AddText("Illegal packet");
          msg_tdc3->SetFillColor(2);
          msg_tdc3->AddText("Error #1");
          msg_tdc3->AddText("Illegal packet");
        }
      else if ((rich_par->GetBinContent(4) + rich_par->GetBinContent(3)) /
               rich_par->GetBinContent(1) > PERMIT_RICH_PACKET_ERROR * 2
               ||
               rich_par->GetBinContent(4)/rich_par->GetBinContent(1) >
               PERMIT_RICH_PACKET_ERROR )
        {
          msg3->SetFillColor(2);
          msg3->AddText("Error #3");
          msg3->AddText("Reset or reconfigure RICH-FEE");
          msg_tdc3->SetFillColor(2);
          msg_tdc3->AddText("Error #3");
          msg_tdc3->AddText("Reset or reconfigure RICH-FEE");
        }
      else
        {
          if ((rich_par->GetBinContent(4) + rich_par->GetBinContent(3)) /
              rich_par->GetBinContent(1) > PERMIT_RICH_PACKET_ERROR)
            {
              msg3->SetFillColor(5);
              msg3->AddText("Error #2");
              msg3->AddText("Report to phenix-rich-l");
              msg_tdc3->SetFillColor(5);
              msg_tdc3->AddText("Error #2");
              msg_tdc3->AddText("Report to phenix-rich-l");
            }
          else
            {
              msg3->SetFillColor(10);
              msg3->AddText("Raw Data O.K.");
              msg_tdc3->SetFillColor(10);
              msg_tdc3->AddText("Raw Data O.K.");
            }

          msg2 = new TPaveText(0.05, 0.55, 0.95, 0.69);

          if (adcstatws > 110 || adcstatwn > 110 ||
              adcstates > 110 || adcstaten > 110 )
            {
              msg2->SetFillColor(2);
              msg2->AddText("Error #11");
              str_msg<<"Check HV and LV of ";
              if (adcstatws > 110)
                {
                  str_msg<< "W.S. ";
                  subPad[2][0]->SetFillColor(2);
                }
              if (adcstatwn > 110)
                {
                  str_msg<< "W.N. ";
                  subPad[2][1]->SetFillColor(2);
                }
              if (adcstates > 110)
                {
                  str_msg<< "E.S. ";
                  subPad[2][2]->SetFillColor(2);
                }
              if (adcstaten > 110)
                {
                  str_msg<< "E.N. ";
                  subPad[2][3]->SetFillColor(2);
                }
              msg2->AddText(str_msg.str().c_str());
	      str_msg.str("");
            }
          else if (adcstatws || adcstatwn ||
                   adcstates || adcstaten )
            {
              msg2->SetFillColor(2);
              msg2->AddText("Error #10");
              str_msg<< "Check HV and LV of ";
              if (adcstatws)
                {
                  str_msg<< "W.S. ";
                  subPad[2][0]->SetFillColor(2);
                }
              if (adcstatwn)
                {
                  str_msg<< "W.N. ";
                  subPad[2][1]->SetFillColor(2);
                }
              if (adcstates)
                {
                  str_msg<< "E.S. ";
                  subPad[2][2]->SetFillColor(2);
                }
              if (adcstaten)
                {
                  str_msg<< "E.N. ";
                  subPad[2][3]->SetFillColor(2);
                }

              msg2->AddText(str_msg.str().c_str());
	      str_msg.str("");
            }
	  else if ( tripws || tripwn || tripes || tripen )
	    {
	      msg2->SetFillColor(2);
              msg2->AddText("Error #12");
              str_msg<<"Low hit rate of ";
	      if (tripws) str_msg<< "WS:" << hitnum_ws;
	      if (tripwn) str_msg<< " WN:" << hitnum_wn;
	      if (tripes) str_msg<< " ES:" << hitnum_es;
	      if (tripen) str_msg<< " EN:" << hitnum_en;
              msg2->AddText(str_msg.str().c_str());
	      str_msg.str("");
	    }
          else
            {
              msg2->SetFillColor(10);
              msg2->AddText("ADC is OK");
	      adc_good=0;
	    }
          msg2->Draw();
	  msg_tdc2 = new TPaveText(0.05, 0.55, 0.95, 0.69);
	  if(tdcstatws>100 || tdcstatwn>100 ||
	     tdcstates>100 || tdcstaten>100 ){


	    msg_tdc2->SetFillColor(2);
	    msg_tdc2->AddText("Error #11");
	    str_msg_tdc<< "Check HV and LV of ";
	    if(tdcstatws>100){
	      str_msg_tdc<<"W.S ";
	      subPad2[1][0]->SetFillColor(2);
	    }
	    if(tdcstatwn>100){
	      str_msg_tdc<<"W.N ";
	      subPad2[1][1]->SetFillColor(2);
	    }
	    if(tdcstates>100){
	      str_msg_tdc<<"E.S ";
	      subPad2[1][2]->SetFillColor(2);
	    }
	    if(tdcstaten>100){
	      str_msg_tdc<<"E.N ";
	      subPad2[1][3]->SetFillColor(2);
	    }
	    msg_tdc2->AddText(str_msg_tdc.str().c_str());
	    str_msg_tdc.str("");
	  }
	  else{
	    msg_tdc2->SetFillColor(10);
	    msg_tdc2->AddText("TDC is OK");
	    tdc_good=0;
	  }
	  Pad2[3]->cd();
	  msg_tdc2->Draw();
	  Pad[3]->cd();
	  if(adc_good==0 && tdc_good==0){ 
	  msg4 = new TPaveText(0.05, 0.40, 0.95, 0.54);
	  if (nspike > 0)
	    {
	      msg4->SetFillColor(5);
	      msg4->AddText("Error #20");
	      msg4->AddText("Spike detected");
	    }
	  else
	    {
	      msg4->SetFillColor(20);
	      // sprintf(str_msg, "Mid-val of hit is %4f", mid);
	      msg4->AddText("RICH is fine now");
	    }
	  Pad[3]->cd();
	  msg4->Draw();
	  Pad2[3]->cd();
	  msg4->Draw();
	  }	    
        }
    }
  else
    {
      Pad[3]->cd();
      msg3->SetFillColor(3);
      msg3->AddText("needs more events (>1000)");
      msg_tdc3->SetFillColor(3);
      msg_tdc3->AddText("needs more events (>1000)");
    }
  Pad[3]->cd();
  msg3->Draw();

  str_msg.str("");
  str_msg_tdc.str("");

  msg5 = new TPaveText(0.05, 0.01, 0.95, 0.39);
  str_msg<<"Entry: "<< rich_par->GetBinContent(1);
  msg5->AddText(str_msg.str().c_str());
  str_msg.str("");
  str_msg << "Mid-Hit : "<< mid;
  /*
    str_msg << "Rate : "
    << hitnum_ws <<" | "<< hitnum_wn <<" | "
    << hitnum_es <<" | "<< hitnum_en;
  */
  msg5->AddText(str_msg.str().c_str());
  str_msg.str("");
  msg5->SetFillColor(10);
  msg5->Draw();
  msg1->Draw();

  Pad2[3]->cd();
  msg_tdc1->Draw();
  msg_tdc3->Draw();
  msg_tdc5 = new TPaveText(0.05, 0.01, 0.95, 0.39);
  str_msg_tdc<<"Entry: "<< rich_par->GetBinContent(1);
  msg_tdc5->AddText(str_msg_tdc.str().c_str());
  str_msg_tdc.str("");

  str_msg_tdc<<"Mean TDC of WS: "<< meantdcWS << " WN: "<< meantdcWN;
  msg_tdc5->AddText(str_msg_tdc.str().c_str());
  str_msg_tdc.str("");

  str_msg_tdc<<"Mean TDC of ES: "<< meantdcES<< "  EN: "<< meantdcEN;
  msg_tdc5->AddText(str_msg_tdc.str().c_str());
  msg_tdc5->SetFillColor(10);
  msg_tdc5->Draw();
  str_msg_tdc.str("");

  TC[0]->Update();
  TC[1]->Update();
  os->cd();

  
  /*
  TFile *fin = new TFile("rich_tdc_pmt.root","RECREATE");
  rich_tdc_pmt ->Write("rich_tdc_pmt");
  fin->Close();
  */

#ifdef BOOK_HISTOS
  ostringstream outfile;
  outfile<< "RICHMON_"<<cl->RunNumber()<<".root";
  TFile *fin = new TFile(outfile,"RECREATE");
  if(fin){
    fin->cd();
    rich_adclivehist ->Write("rich_adclivehist");
    rich_tdclivehist ->Write("rich_tdclivehist");
    rich_adchistsumWS ->Write("rich_adchistsumWS");
    rich_adchistsumWN ->Write("rich_adchistsumWN");
    rich_adchistsumES ->Write("rich_adchistsumES");
    rich_adchistsumEN ->Write("rich_adchistsumEN");
    rich_adc_second ->Write("rich_adc_second");
    rich_adc_first ->Write("rich_adc_first");
    rich_adc_hit ->Write("rich_adc_hit");
    rich_adc_pmt ->Write("rich_adc_pmt");
    rich_deadchinlivehist ->Write("rich_deadchinlivehist");
    rich_par ->Write("rich_par");
    rich_ringhist[0] ->Write("rich_ringhistWS");
    rich_ringhist[1] ->Write("rich_ringhistWN");
    rich_ringhist[2] ->Write("rich_ringhistES");
    rich_ringhist[3] ->Write("rich_ringhistEN");
    rich_tdchistsumWS ->Write("rich_tdchistsumWS");
    rich_tdchistsumWN ->Write("rich_tdchistsumWN");
    rich_tdchistsumES ->Write("rich_tdchistsumES");
    rich_tdchistsumEN ->Write("rich_tdchistsumEN");
    rich_tdc_second ->Write("rich_tdc_second");
    rich_tdc_first ->Write("rich_tdc_first");
    rich_tdc_hit ->Write("rich_tdc_hit");
    rich_tdc_pmt ->Write("rich_tdc_pmt");
  }else{
    cout<<" cannot open file!"<<endl;
  }
#endif
  
  return 0;
}

int RICHMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  filename << ThisName << "_ADC_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_TDC_" << cl->RunNumber() << ".ps";
  TC[1]->Print(filename.str().c_str());
  return 0;
}

int RICHMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();

  string ofile[2];
  ofile[0] = cl->htmlRegisterPage(*this,"ADC monitor ","1","png");
  ofile[1] = cl->htmlRegisterPage(*this,"TDC monitor", "2","png");
  for (int i=0; i<2;i++)
    {
      cl->CanvasToPng(TC[i],ofile[i]);
    }
  cl->SaveLogFile(*this);
  
  return 0;
}



int RICHMonDraw::chk_adcsum(TH1* h_adcsum)
{
  float pe = h_adcsum->Integral(41, 220);
  float ped = h_adcsum->Integral(0, 40);

  // First, avoid "divided by 0" and "Log->inf"
  if (ped == 0)
    {
      if ( pe > 0 )
        {
          return 130;   // No pedestal
        }
    }
  if (pe == 0)
    {
      if ( ped >= 0 ) // catch ped = 0
        {
          return 110;   // No signal
        }
    }
  float log_pe_ped = log10(pe / ped);

  if (log_pe_ped > RICH_LOWADC_DESCISION)
    {
      return 120;   // So little pedestal
    }
  if (log_pe_ped < - RICH_LOWADC_DESCISION)
    {
      return 100;   // So little signal
    }
  return 0;
}




int RICHMonDraw::chk_adcsum0sup(TH1* h_adcsum)
{
  return chk_adcsum(h_adcsum);
}

int RICHMonDraw::chk_tdcsum(TH1* h_tdcsum)
{
  int bin_lo = h_tdcsum->FindBin(RICH_TACHIT_LOW);
  int bin_hi = h_tdcsum->FindBin(RICH_TACHIT_HIGH);
  int bin_tot = h_tdcsum->GetNbinsX();
  float signal = h_tdcsum->Integral(bin_lo, bin_hi);
  float pedlo = h_tdcsum->Integral(0, bin_lo-1);
  float pedhi = h_tdcsum->Integral(bin_hi+1, bin_tot);
  
  float total = pedlo+signal+pedhi;

  if(total==0){
    return 110 ;   // No entry
  }
  float ratio = signal/total;

  if(ratio<RICH_LOWTDC_DESCISION){
    return 120 ;  // No signal
  }
  else {
    return 0;
  }

}

/*
  int RICHMonDraw::chk_adcsum(TH1* h_adcsum)
  {
 
  int val_return;
  // Find pedestal
  int pedpeak = h_adcsum->GetMaximumBin();
 
  if (fabs(h_adcsum->GetBinCenter(pedpeak)) > 100)
  {
  val_return = 10; // pedestal peak position is illegal
  }
  else
  {
  // Fit pedestal
  TF1 *f1 = new TF1("f1", "gaus", -150, h_adcsum->GetBinCenter(pedpeak));
  h_adcsum->Fit("f1", "Q", "E", -150, h_adcsum->GetBinCenter(pedpeak));
  TF1 *pedfit = new TF1("f2", "gaus", -150, h_adcsum->GetBinCenter(pedpeak) +
  f1->GetParameter(2));
  h_adcsum->Fit("f2", "Q", "E", -150,
  h_adcsum->GetBinCenter(pedpeak) + f1->GetParameter(2));
 
  // Get 1 p.e. peak value
  h_adcsum->SetAxisRange(pedfit->GetParameter(1) + pedfit->GetParameter(2)*5,
  h_adcsum->GetXaxis()->GetXmax(), "X");
 
  double pep = h_adcsum->GetMaximum();
  if (h_adcsum->GetBinContent(pedpeak) > 0)
  {
  if (TMath::Log10(h_adcsum->GetBinContent(pedpeak) /
  h_adcsum->GetBinContent(pep) )
              < (float) RICH_LOWADC_DESCISION)
            {
              val_return = 100; // 1 p.e. is too small
            }
          else
            {
              val_return = 0;
            }
        }
      else
        {
          val_return = 20;
        }
 
      delete f1, pedfit;
    }
 
  return val_return;
}
*/

int 
RICHMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(7);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "RICH MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}
