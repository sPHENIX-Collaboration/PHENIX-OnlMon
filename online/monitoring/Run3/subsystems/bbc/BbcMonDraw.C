#include "BbcMonDraw.h"
#include <OnlMonClient.h>

#include <BbcCalib.hh>

#include <TArc.h>
#include <TArrow.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLine.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TSpectrum.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
//#include <unistd.h>

using namespace std;

// #define DEBUG

#ifdef DEBUG
#define ifdelete(x) if(x!=NULL){ cout << "Delete " << #x << endl; delete x;x=NULL;}
#define ifnew(t,x) {if(x!=NULL){ cout << "Delete " << #x << endl; delete x;}cout << "New "<< #x << endl;x = new t;}
#define PRINT_DEBUG(x) cout<<x<<endl
#else
#define ifdelete(x) if(x!=NULL){ delete x;x=NULL;}
#define ifnew(t,x) {if(x!=NULL){ delete x;}x = new t;}
#define PRINT_DEBUG(x) {};
#endif

// #define ifdelete(x) 1

// x position of trigger, scale factor and vtx mean
//static float xpos[3] = {0.30, 0.50, 0.75};
static float xpos[4] = {0.20, 0.35, 0.55, 0.75};


int BbcMonDraw::ClearWarning( void )
{
  for ( int i = 0 ; i < nWarning ; i++ )
    {
      ifdelete( PaveWarning[i] );
      ifdelete( ArcWarning[i] );
    }
  nWarning = 0;

  if ( PaveWarnings )
    {
      PaveWarnings->Clear();
    }
  // if( PadWarnings )
  //PaveWarnings->Draw();
  return 0;
}


int BbcMonDraw::Warning( TPad *pad, const float x, const float y, const int r, const string& msg)
{
  static int brink = 1;
  float x1, x2, y1, y2;

  x1 = pad->GetX1() * 0.97 + pad->GetX2() * 0.03;
  x2 = pad->GetX1() * 0.03 + pad->GetX2() * 0.97;
  // pave rises on top if error point is lower
  if ( y > pad->GetY1()*0.5 + pad->GetY2()*0.5 )
    {
      y1 = pad->GetY1() + (pad->GetY2() - pad->GetY1()) * 0.10;
      y2 = pad->GetY1() + (pad->GetY2() - pad->GetY1()) * 0.30;
    }
  else
    {
      y1 = pad->GetY1() + (pad->GetY2() - pad->GetY1()) * 0.70;
      y2 = pad->GetY1() + (pad->GetY2() - pad->GetY1()) * 0.90;
    }

  int newWarning = nWarning;
  for ( int i = 0 ; i < nWarning ; i++ )
    {
      if ( PadWarning[i] == pad )
        {
          newWarning = i;
        }
    }

  ifnew( TArc(x, y, r) , ArcWarning[newWarning] );
  ArcWarning[newWarning]->SetLineWidth( r );
  ArcWarning[newWarning]->SetLineColor( 2 );

  ifnew( TPaveText(x1, y1, x2, y2), PaveWarning[newWarning] );
  PaveWarning[newWarning]->AddText(msg.c_str());
  ArcWarning[newWarning]->SetLineColor( 2 + 3*brink );
  PaveWarning[newWarning]->SetFillColor( 5 + 5*brink );
  PaveWarning[newWarning]->SetTextColor( 2 );
  PaveWarning[newWarning]->SetLineColor( 2 );
  PaveWarning[newWarning]->SetLineWidth( 2 );

  if ( newWarning == nWarning )
    {
      PadWarning[newWarning] = pad;
      nPadWarning[newWarning] = 1;
      nWarning++;
    }
  else
    {
      ostringstream wmsg;
      wmsg << "... and other " << nPadWarning[newWarning] << " warnings" ;
      nPadWarning[newWarning]++;
      PaveWarning[newWarning]->AddText(wmsg.str().c_str());
      wmsg.str("");
    }
  //PaveWarning[newWarning]->AddText( "Call BBC Expert. If this run is a PHYSICS run " );
  PaveWarning[newWarning]->AddText( "Do Quick-feed BBC" );
  PaveWarning[newWarning]->AddText( "If it is NOT fixed by feed at PHYSICS run, call BBC experts" );
  //PaveWarning[newWarning]->AddText( "If it is NOT fixed by quick feed at PHYSICS run, conform to the BBC standing orders" );

  if ( PaveWarnings )
    {
      PaveWarnings->AddText(msg.c_str());
    }

  if ( PadWarnings && PaveWarnings)
    {
      PaveWarnings->Draw();
    }
  // ArcWarning[newWarning]->Draw();
  PaveWarning[newWarning]->Draw();

  // brink = 1 - brink;

  if ( nWarning == MAX_WARNING - 2 )
    {
      string bmsg = "Too Many Warnings";
      Warning(pad, x, y, 0, bmsg);
      bmsg.erase();
    }

  return 0;
}


BbcMonDraw::BbcMonDraw(const char *name): 
  OnlMonDraw(name),
  bbccalib(NULL),
  PaveTop(NULL),
  TextTop(NULL),
  nWarning(0),
  PadnHitStatus(NULL),
  TextnHitStatus(NULL),
  AvrHitTime(NULL),
  PadAvrHitTime(NULL),
  ArrowAvrHitTime(NULL),
  TextAvrHitTime(NULL),
  FitAvrHitTime(NULL),
  SouthHitTime(NULL),
  PadSouthHitTime(NULL),
  ArrowSouthHitTime(NULL),
  TextSouthHitTime(NULL),
  FitSouthHitTime(NULL),
  NorthHitTime(NULL),
  PadNorthHitTime(NULL),
  ArrowNorthHitTime(NULL),
  TextNorthHitTime(NULL),
  FitNorthHitTime(NULL),
  SouthChargeSum(NULL),
  NorthChargeSum(NULL),
  PadChargeSum(NULL),
  TextSouthChargeSum(NULL),
  TextNorthChargeSum(NULL),
  TextTzeroZvtx(NULL),
  PadBbcSummary(NULL),
  TextBbcSummaryZvertex(NULL),
  TextBbcSummaryTrigRate(NULL),
  Zvtx(NULL),
  Zvtx_bbll1(NULL),
  Zvtx_zdc(NULL),
  Zvtx_zdc_scale3(NULL),
  Zvtx_bbll1_novtx(NULL),
  Zvtx_bbll1_narrowvtx(NULL),
  Zvtx_bbll1_zdc(NULL),
  PadZvtx(NULL),
  FitZvtx(NULL),
  ArrowZvtx(NULL),
  TextZvtx(NULL),
  TextZvtxNorth(NULL),
  TextZvtxSouth(NULL),
  ArmHit(NULL),
  PadArmHit(NULL),
  TextArmHit(NULL),
  ArcArmHit(NULL),
  Adc(NULL),
  PadAdc(NULL),
  PadButton(NULL),
  PadMultiView(NULL),
  MultiView1F(NULL),
  MultiView2F(NULL),
  PadWarnings(NULL),
  PaveWarnings(NULL),
  PadZVertex(NULL),
  PadTzeroZVertex(NULL),
  PadZVertexSummary(NULL),
  TextZVertexExpress(NULL),
  TextZVertexNotice(NULL),
  TzeroZvtx(NULL),
  Prescale_hist(NULL),
  tspec(NULL)
{

  TStyle* oldStyle = gStyle;
  bbcStyle = new TStyle("bbcStyle", "BBC Online Monitor Style");
  bbcStyle->SetOptStat(0);
  bbcStyle->SetTitleH(0.075);
  bbcStyle->SetTitleW(0.98);
  bbcStyle->SetPalette(1);
  bbcStyle->SetFrameBorderMode(0);
  bbcStyle->SetPadBorderMode(0);
  bbcStyle->SetCanvasBorderMode(0);
  oldStyle->cd();

  PRINT_DEBUG("Class constructing...");

  // ------------------------------------------------------
  // Canvas and Histogram

  memset(TC,NULL,sizeof(TC));
  memset(transparent,NULL,sizeof(transparent));
  memset(PadTop,NULL,sizeof(PadTop));
  memset(PaveWarning,NULL,sizeof(PaveWarning));
  memset(ArcWarning,NULL,sizeof(ArcWarning));
  memset(PadWarning,NULL,sizeof(PadWarning));
  memset(nPadWarning,NULL,sizeof(nPadWarning));
  // ------------------------------------------------------
  // Graph for visualization

  // for the 1st Page
  memset(TdcOver,NULL,sizeof(TdcOver));
  memset(PadTdcOver,NULL,sizeof(PadTdcOver));
  memset(LineTdcOver,NULL,sizeof(LineTdcOver));
  memset(BoxTdcOver,NULL,sizeof(BoxTdcOver));
  memset(FrameTdcOver,NULL,sizeof(FrameTdcOver));
  memset(PadnHit,NULL,sizeof(PadnHit));
  memset(nHit,NULL,sizeof(nHit));
  memset(LinenHit,NULL,sizeof(LinenHit));
  memset(BoxnHit,NULL,sizeof(BoxnHit));
  memset(FramenHit,NULL,sizeof(FramenHit));

  // for 2nd Page

  memset(HitTime,NULL,sizeof(HitTime));
  memset(PadHitTime,NULL,sizeof(PadHitTime));
  memset(LineHitTime,NULL,sizeof(LineHitTime));
  memset(ArrowHitTime,NULL,sizeof(ArrowHitTime));
  memset(TextHitTime,NULL,sizeof(TextHitTime));
  memset(FitHitTime,NULL,sizeof(FitHitTime));
  memset(LineAvrHitTime,NULL,sizeof(LineAvrHitTime));

  memset(LineNorthHitTime,NULL,sizeof(LineNorthHitTime));
  memset(LineSouthHitTime,NULL,sizeof(LineSouthHitTime));

  memset(TextBbcSummaryHitTime,NULL,sizeof(TextBbcSummaryHitTime));
  memset(TextBbcSummaryGlobalOffset,NULL,sizeof(TextBbcSummaryGlobalOffset));

  memset(LineZvtx,NULL,sizeof(LineZvtx));

  memset(LineTzeroZvtx,NULL,sizeof(LineTzeroZvtx));

  memset(TextZVertex,NULL,sizeof(TextZVertex));
  memset(TextZVertex_scale,NULL,sizeof(TextZVertex_scale));
  memset(TextZVertex_mean,NULL,sizeof(TextZVertex_mean));


  FitZvtxBBLL1NoVtx = new TF1("FitZvtxBBLL1NoVtx", "gaus");
  TextZVtxStatus[0] = new TLatex;
  TextZVtxStatus[1] = new TLatex;

  PRINT_DEBUG("Class constructing...Done");

  return ;
}


BbcMonDraw::~BbcMonDraw()
{

  ifdelete( bbccalib );

  // ------------------------------------------------------
  // Canvas and Histogram

  ifdelete( PaveTop );
  ifdelete( TextTop );

  for ( int i = 0 ; i < MAX_WARNING ; i++ )
    {
      ifdelete( PaveWarning[i] );
      ifdelete( ArcWarning[i] );
    }


  // ------------------------------------------------------
  // Graph for visualization

  // for the 1st Page

  for ( int side = 0 ; side < nSIDE ; side++ )
    {
      for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
        {
          ifdelete( TdcOver[tdc][side] );
          ifdelete( LineTdcOver[tdc][side][0] );
          ifdelete( LineTdcOver[tdc][side][1] );
          ifdelete( BoxTdcOver[tdc][side] );
        }

      for ( int trig = 0 ; trig < nTRIGGER ; trig++ )
        {
          ifdelete( nHit[trig][side] );
          ifdelete( LinenHit[trig][side][0] );
          ifdelete( LinenHit[trig][side][1] );
          ifdelete( BoxnHit[trig][side] );
        }
    }
  ifdelete( TextnHitStatus );

  // for 2nd Page

  for ( int side = 0 ; side < nSIDE ; side++ )
    {
      for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
        {

          ifdelete( HitTime[tdc][side] );
          ifdelete( LineHitTime[tdc][side][0] );
          ifdelete( LineHitTime[tdc][side][1] );
          ifdelete( ArrowHitTime[tdc][side] );
          ifdelete( TextHitTime[tdc][side] );
          ifdelete( FitHitTime[tdc][side] );
        }
    }

  ifdelete( AvrHitTime );
  ifdelete( LineAvrHitTime[0] );
  ifdelete( LineAvrHitTime[1] );
  ifdelete( ArrowAvrHitTime );
  ifdelete( TextAvrHitTime );
  ifdelete( FitAvrHitTime );

  ifdelete( NorthHitTime );
  ifdelete( LineNorthHitTime[0] );
  ifdelete( LineNorthHitTime[1] );
  ifdelete( ArrowNorthHitTime );
  ifdelete( TextNorthHitTime );
  ifdelete( FitNorthHitTime );
  ifdelete( SouthHitTime );
  ifdelete( LineSouthHitTime[0] );
  ifdelete( LineSouthHitTime[1] );
  ifdelete( ArrowSouthHitTime );
  ifdelete( TextSouthHitTime );
  ifdelete( FitSouthHitTime );

  ifdelete( NorthChargeSum );
  ifdelete( SouthChargeSum );
  ifdelete( TextSouthChargeSum );
  ifdelete( TextNorthChargeSum );

  ifdelete( TextBbcSummaryHitTime[0] );
  ifdelete( TextBbcSummaryHitTime[1] );
  ifdelete( TextBbcSummaryGlobalOffset[0] );
  ifdelete( TextBbcSummaryGlobalOffset[1] );
  ifdelete( TextBbcSummaryZvertex );
  ifdelete( TextBbcSummaryTrigRate );

  ifdelete( Zvtx );
  ifdelete( FitZvtx );
  ifdelete( LineZvtx[0] );
  ifdelete( LineZvtx[1] );
  ifdelete( ArrowZvtx );
  ifdelete( TextZvtx );
  ifdelete( TextZvtxNorth );
  ifdelete( TextZvtxSouth );

  ifdelete( ArmHit );
  ifdelete( TextArmHit );
  ifdelete( ArcArmHit );

  // for 3rd Page
  ifdelete( Adc );
  ifdelete( MultiView1F );
  ifdelete( MultiView2F );
  ifdelete( PaveWarnings );

  // for 4th Page
  ifdelete( Zvtx_bbll1 );
  ifdelete( Zvtx_zdc );
  ifdelete( Zvtx_zdc_scale3 );
  ifdelete( Zvtx_bbll1_novtx );
  ifdelete( Zvtx_bbll1_narrowvtx );
  ifdelete( Zvtx_bbll1_zdc );
  ifdelete( TzeroZvtx );
  ifdelete ( TextZVertexExpress );
  ifdelete( TextZVertexNotice );

  ifdelete( LineTzeroZvtx[0] );
  ifdelete( LineTzeroZvtx[1] );
  ifdelete( LineTzeroZvtx[2] );
  ifdelete( LineTzeroZvtx[3] );
  ifdelete( TextTzeroZvtx );

  //for (int i = 0; i < 4; i++)//need a look
  for (int i = 0; i < 5; i++)
    {
      ifdelete ( TextZVertex[i] );
      ifdelete ( TextZVertex_scale[i] );
      ifdelete ( TextZVertex_mean[i] );
    }
  delete FitZvtxBBLL1NoVtx;
  delete TextZVtxStatus[0];
  delete TextZVtxStatus[1];


  ifdelete ( Prescale_hist );
  delete tspec;

  return ;
}

int BbcMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if ( strlen(name) < 1 || strcmp(name, "BBCMon1") == 0 )
    {
      cout << "Creating Canvas BBCMon1..." << endl;

      TC[0] = new TCanvas("BBCMon1", "BBC PMT/FEM status view for Shift crew", -1, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      TC[0]->cd();

      PadTop[0] = new TPad("PadTop0" , "PadTop0" , 0.00, 0.90, 1.00, 1.00, 0, 0, 0);
      PadTdcOver[0][1] = new TPad("PadTdcOverNorth0", "PadTdcOverNorth0", 0.50, 0.30, 1.00, 0.60, 0, 0, 0);
      PadTdcOver[0][0] = new TPad("PadTdcOverSouth0", "PadTdcOverSouth0", 0.00, 0.30, 0.50, 0.60, 0, 0, 0);
      PadTdcOver[1][1] = new TPad("PadTdcOverNorth1", "PadTdcOverNorth1", 0.50, 0.60, 1.00, 0.90, 0, 0, 0);
      PadTdcOver[1][0] = new TPad("PadTdcOverSouth1", "PadTdcOverSouth1", 0.00, 0.60, 0.50, 0.90, 0, 0, 0);
      PadnHit[1] = new TPad("PadnHitNorth" , "PadnHitNorth" , 0.50, 0.03, 1.00, 0.30, 0, 0, 0);
      PadnHit[0] = new TPad("PadnHitSouth" , "PadnHitSouth" , 0.00, 0.03, 0.50, 0.30, 0);
      PadnHitStatus = new TPad("PadnHitStatus" , "PadnHitStatus" , 0.00, 0.00, 1.00, 0.03, 0, 0, 0);


      PadTop[0]->Draw();
      for ( int side = 0 ; side < nSIDE ; side++ )
        {
          for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
            {
              PadTdcOver[tdc][side]->Draw();
            }
          PadnHit[side]->Draw();
          PadnHitStatus->Draw();

          ifnew( TBox( 0.5, BBC_nHIT_MB_MIN[side],
                       nPMT_1SIDE_BBC + .5, BBC_nHIT_MB_MAX[side] ), BoxnHit[0][side] );
          BoxnHit[0][side]->SetFillColor( 5 );
          BoxnHit[0][side]->SetLineColor( 3 );

          ifnew( TBox( 0.5, BBC_nHIT_LASER_MIN[side],
                       nPMT_1SIDE_BBC + .5, BBC_nHIT_LASER_MAX[side] ), BoxnHit[1][side] );
          BoxnHit[1][side]->SetFillColor( 7 );
          BoxnHit[1][side]->SetLineColor( 4 );

        }

      for ( int side = 0 ; side < nSIDE ; side++ )
        {
          for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
            {
              ifnew( TBox( 0.5, BBC_TDC_OVERFLOW_REGULAR_MIN,
                           nPMT_1SIDE_BBC + .5, BBC_TDC_OVERFLOW_REGULAR_MAX ), BoxTdcOver[tdc][side] );
              BoxTdcOver[tdc][side]->SetFillColor( 5 );
              BoxTdcOver[tdc][side]->SetLineColor( 3 );

            }
        }
      ifnew( TText, TextnHitStatus );
      TextnHitStatus->SetTextSize(0.7);
      TextnHitStatus->SetText(0.05, 0.5, "Red Square : Collision Event  / Blue Triangle : Laser Event");
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1, 0, 0);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
    }

  if ( strlen(name) < 1 || strcmp(name, "BBCMon2") == 0 )
    {
      cout << "Creating Canvas BBCMon2..." << endl;

      TC[1] = new TCanvas("BBCMon2", "Beam status view for Shift crew", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      TC[1]->cd();
      PadTop[1] = new TPad("PadTop1" , "PadTop1" , 0.00, 0.90, 1.00, 1.00, 0, 0, 0);
      PadArmHit = new TPad("PadArmHit" , "PadArmHit" , 0.50, 0.00, 1.00, 0.40, 0, 0, 0);
      PadSouthHitTime = new TPad("PadHitTimeSouth1", "PadHitTimeSouth1", 0.00, 0.55, 0.50, 0.90, 0, 0, 0);
      PadNorthHitTime = new TPad("PadHitTimeNorth1", "PadHitTimeNorth1", 0.50, 0.55, 1.00, 0.90, 0, 0, 0);
      PadBbcSummary = new TPad("PadBbcSummary" , "PadBbcSummary" , 0.00, 0.40, 1.00, 0.55, 0, 0, 0);
      PadAvrHitTime = new TPad("PadAvrHitTime" , "PadAvrHitTime" , 0.00, 0.00, 0.50, 0.40, 0, 0, 0);

      PadArmHit->SetLogz();

      PadTop[1]->Draw();
      PadArmHit->Draw();
      PadNorthHitTime->Draw();
      PadSouthHitTime->Draw();
      PadBbcSummary->Draw();
      if ( PadAvrHitTime )
        {
          PadAvrHitTime->Draw();
        }

      ifnew( TText, TextBbcSummaryHitTime[0] );
      ifnew( TText, TextBbcSummaryHitTime[1] );
      ifnew( TText, TextBbcSummaryZvertex );
      ifnew( TText, TextBbcSummaryGlobalOffset[0] );
      ifnew( TText, TextBbcSummaryGlobalOffset[1] );
      //TextBbcSummaryHitTime->SetTextSize(0.2);
      TextBbcSummaryHitTime[0]->SetTextSize(0.2);
      TextBbcSummaryHitTime[1]->SetTextSize(0.2);
      TextBbcSummaryZvertex->SetTextSize(0.2);
      TextBbcSummaryGlobalOffset[0]->SetTextSize(0.2);
      TextBbcSummaryGlobalOffset[1]->SetTextSize(0.2);

      ifnew( TArc(10.5, 10.5, 4, 0, 360), ArcArmHit );
      ArcArmHit->SetFillStyle(4000);
      //ifnew( TArc(10.5, 10.5, 4), ArcArmHit );
      ifnew( TText, TextArmHit);
      TextArmHit->SetTextSize(0.08);
      TextArmHit->SetText(10, 4, "Good region");

      // PadSouthHitTime = new TPad("PadSouthHitTime", "PadSouthHitTime", 0.00, 0.55, 0.50, 0.90, 0);
      // PadNorthHitTime = new TPad("PadNorthHitTime", "PadNorthHitTime", 0.50, 0.55, 1.00, 0.90, 0);

      if ( PadAvrHitTime )
        {
          ifnew( TLine( BBC_MIN_REGULAR_TDC1_MEAN, 0,
                        BBC_MIN_REGULAR_TDC1_MEAN, 10000 ),
                 LineAvrHitTime[0] );
          ifnew( TLine( BBC_MAX_REGULAR_TDC1_MEAN, 0,
                        BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                 LineAvrHitTime[1] );

          ifnew( TArrow( BBC_MIN_REGULAR_TDC1_MEAN, 10000,
                         BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                 ArrowAvrHitTime );
          ArrowAvrHitTime->SetOption("<>");

          ifnew( TText , TextAvrHitTime );
          TextAvrHitTime->SetTextSize(0.08);
          TextAvrHitTime->SetText(BBC_MAX_REGULAR_TDC1_MEAN, 10000, " good mean");

          LineAvrHitTime[0]->SetLineColor(9);
          LineAvrHitTime[1]->SetLineColor(9);
          ArrowAvrHitTime->SetLineColor(9);
          TextAvrHitTime->SetTextColor(9);
        }

      if ( PadSouthHitTime )
        {
          ifnew( TLine( BBC_MIN_REGULAR_TDC1_MEAN, 0,
                        BBC_MIN_REGULAR_TDC1_MEAN, 10000 ),
                 LineSouthHitTime[0] );
          ifnew( TLine( BBC_MAX_REGULAR_TDC1_MEAN, 0,
                        BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                 LineSouthHitTime[1] );

          ifnew( TArrow( BBC_MIN_REGULAR_TDC1_MEAN, 10000,
                         BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                 ArrowSouthHitTime );
          ArrowSouthHitTime->SetOption("<>");

          ifnew( TText , TextSouthHitTime );
          TextSouthHitTime->SetTextSize(0.08);
          TextSouthHitTime->SetText(BBC_MAX_REGULAR_TDC1_MEAN, 10000, " good mean");

          LineSouthHitTime[0]->SetLineColor(9);
          LineSouthHitTime[1]->SetLineColor(9);
          ArrowSouthHitTime->SetLineColor(9);
          TextSouthHitTime->SetTextColor(9);
        }


      if ( PadNorthHitTime )
        {
          ifnew( TLine( BBC_MIN_REGULAR_TDC1_MEAN, 0,
                        BBC_MIN_REGULAR_TDC1_MEAN, 10000 ),
                 LineNorthHitTime[0] );
          ifnew( TLine( BBC_MAX_REGULAR_TDC1_MEAN, 0,
                        BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                 LineNorthHitTime[1] );

          ifnew( TArrow( BBC_MIN_REGULAR_TDC1_MEAN, 10000,
                         BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                 ArrowNorthHitTime );
          ArrowNorthHitTime->SetOption("<>");

          ifnew( TText , TextNorthHitTime );
          TextNorthHitTime->SetTextSize(0.08);
          TextNorthHitTime->SetText(BBC_MAX_REGULAR_TDC1_MEAN, 10000, " good mean");

          LineNorthHitTime[0]->SetLineColor(9);
          LineNorthHitTime[1]->SetLineColor(9);
          ArrowNorthHitTime->SetLineColor(9);
          TextNorthHitTime->SetTextColor(9);
        }

      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1, 0, 0);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
    }

  if ( strcmp(name, "BBCMon3") == 0 )
    {
      cout << "Creating Canvas BBCMon3..." << endl;

      TC[2] = new TCanvas("BBCMon3", "BBC status view for Expert", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      TC[2]->cd();

      PadTop[2] = new TPad("PadTop2" , "PadTop2" , 0.00, 0.90, 1.00, 1.00, 0, 0, 0);
      PadTop[2]->Draw();

      PadHitTime[1][0] = new TPad("PadHitTimeSouth1", "PadHitTimeSouth1", 0.00, 0.60, 0.50, 0.90, 0, 0, 0);
      PadHitTime[1][1] = new TPad("PadHitTimeNorth1", "PadHitTimeNorth1", 0.50, 0.60, 1.00, 0.90, 0, 0, 0);
      PadHitTime[1][0]->SetLeftMargin(0.17);
      PadHitTime[1][1]->SetLeftMargin(0.17);
      PadHitTime[1][0]->Draw();
      PadHitTime[1][1]->Draw();

      for ( int side = 0 ; side < nSIDE ; side++ )
        {
          for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
            {
              ifnew( TLine( BBC_MIN_REGULAR_TDC1_MEAN, 0,
                            BBC_MIN_REGULAR_TDC1_MEAN, 10000 ),
                     LineHitTime[tdc][side][0] );
              ifnew( TLine( BBC_MAX_REGULAR_TDC1_MEAN, 0,
                            BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                     LineHitTime[tdc][side][1] );

              ifnew( TArrow( BBC_MIN_REGULAR_TDC1_MEAN, 10000,
                             BBC_MAX_REGULAR_TDC1_MEAN, 10000 ),
                     ArrowHitTime[tdc][side] );
              ArrowHitTime[tdc][side]->SetOption("<>");

              ifnew( TText , TextHitTime[tdc][side] );
              TextHitTime[tdc][side]->SetTextSize(0.08);
              TextHitTime[tdc][side]->SetText(BBC_MAX_REGULAR_TDC1_MEAN, 10000, " good mean");

              LineHitTime[tdc][side][0]->SetLineColor(9);
              LineHitTime[tdc][side][1]->SetLineColor(9);
              ArrowHitTime[tdc][side]->SetLineColor(9);
              TextHitTime[tdc][side]->SetTextColor(9);

            }
        }
      PadZvtx = new TPad("PadZvtx" , "PadZvtx" , 0.00, 0.30, 0.50, 0.60, 0, 0, 0);

      // for Zvtx
      if (PadZvtx)
        PadZvtx->SetLeftMargin(0.17);
      if ( PadZvtx )
        PadZvtx->Draw();
      if ( PadZvtx )
        {
          ifnew( TLine( BBC_MIN_REGULAR_ZVERTEX_MEAN, 0,
                        BBC_MIN_REGULAR_ZVERTEX_MEAN, 10000 ),
                 LineZvtx[0] );
          ifnew( TLine( BBC_MAX_REGULAR_ZVERTEX_MEAN, 0,
                        BBC_MAX_REGULAR_ZVERTEX_MEAN, 10000 ),
                 LineZvtx[1] );

          ifnew( TArrow( BBC_MIN_REGULAR_ZVERTEX_MEAN, 10000,
                         BBC_MAX_REGULAR_ZVERTEX_MEAN, 10000 ),
                 ArrowZvtx );
          ArrowZvtx->SetOption("<>");


          ifnew( TText , TextZvtx );
          TextZvtx->SetTextSize(0.08);
          TextZvtx->SetText(BBC_MAX_REGULAR_ZVERTEX_MEAN, 10000, " good mean");
          ifnew( TText , TextZvtxNorth );
          TextZvtxNorth->SetTextSize(0.05);
          TextZvtxNorth->SetText(130, 0, "North");
          ifnew( TText , TextZvtxSouth );
          TextZvtxSouth->SetTextSize(0.05);
          TextZvtxSouth->SetText( -160, 0, "South");

          LineZvtx[0]->SetLineColor(9);
          LineZvtx[1]->SetLineColor(9);
          ArrowZvtx->SetLineColor(9);
          TextZvtx->SetTextColor(9);
        }

      PadChargeSum = new TPad("PadChargeSum" , "PadCHargeSum" , 0.50, 0.30, 1.00, 0.60, 0, 0, 0);
      PadChargeSum->SetLogy();
      PadChargeSum->Draw();

      if ( PadChargeSum )
        {
          ifnew( TText , TextNorthChargeSum );
	  // SetNDC(): this will use normalized coordinates (0-1/0-1) for drawing no matter what the histogram uses
          TextNorthChargeSum->SetNDC();
          TextNorthChargeSum->SetTextSize(0.08);
          TextNorthChargeSum->SetText(0.3, 0.8, "--North");    //for p+p
          TextNorthChargeSum->SetTextColor(4);

          ifnew( TText , TextSouthChargeSum );
	  // SetNDC(): this will use normalized coordinates (0-1/0-1) for drawing no matter what the histogram uses
          TextSouthChargeSum->SetNDC();
          TextSouthChargeSum->SetTextSize(0.08);
          TextSouthChargeSum->SetText(0.6, 0.8, "--South");    //for p+p
          TextSouthChargeSum->SetTextColor(2);

        }


      PadAdc = new TPad("PadAdc" , "PadAdc" , 0.00, 0.00, 1.00, 0.30, 0, 0, 0);
      PadAdc->SetLogz();
      PadAdc->Draw();
      transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
      transparent[2]->SetFillStyle(4000);
      transparent[2]->Draw();

      //PadHitTime[1][0]->SetLogy();
      //PadHitTime[1][1]->SetLogy();



      /*
	ifnew( TPad("PadButton" , "PadButton" , 0.00, 0.70, 1.00, 0.90, 0), PadButton);
	ifnew( TPad("PadMultiView" , "PadMultiView" , 0.10, 0.20, 1.00, 0.70, 0), PadMultiView);
	ifnew( TPad("PadWarnings" , "PadWarnings" , 0.00, 0.00, 1.00, 0.20, 0), PadWarnings);
	PadMultiView->Draw();
	PadWarnings->Draw();
	PadButton->Draw();
	ifnew( TPaveText(0.05, 0.05, 0.95, 0.95) , PaveWarnings );
      */
    }
  //
  // 4th Page
  if ( strlen(name) < 1 || strcmp(name, "BBCMon4") == 0 )
    {
      cout << "Creating Canvas BBCMon4..." << endl;

      //ifnew( TText, TextBbcSummaryTrigRate );
      ifnew( TLatex, TextBbcSummaryTrigRate );
      //TextBbcSummaryTrigRate->SetTextSize(0.109);
      TextBbcSummaryTrigRate->SetTextSize(0.08);
      //TextBbcSummaryTrigRate->SetTextSize(0.075);

      TC[3] = new TCanvas("BBCMon4", "Beam/Trigger status view for Shift clew", -1, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      TC[3]->cd();
      PadTop[3] = new TPad("PadTop3" , "PadTop3" , 0.00, 0.90, 1.00, 1.00, 0, 0, 0);
      PadZVertex = new TPad("PadZVertex" , "PadZVertex" , 0.00, 0.60, 1.00, 0.90, 0, 0, 0);
      PadZVertexSummary = new TPad("PadZVertexSummary" , "PadZVertexSummary" , 0.00, 0.40, 1.00, 0.60, 0, 0, 0);
      PadTzeroZVertex = new TPad("PadTzeroZVertex" , "PadTzeroZVertex" , 0.00, 0.00, 1.00, 0.40, 0, 0, 0);

      PadTop[3]->Draw();
      //PadZVertex->SetLogy();
      PadZVertex->Draw();
      PadTzeroZVertex->Draw();
      PadZVertexSummary->Draw();

      PadZVertexSummary->cd();
      ifnew( TText, TextZVertexExpress );
      TextZVertexExpress->SetTextSize(0.08);
      TextZVertexExpress->SetText(0.1, 0.40, "< All histograms are scaled by scaled factor>");

      //ifnew( TText, TextZVertexNotice );
      //TextZVertexNotice->SetTextSize(0.08);
      //TextZVertexNotice->SetText(0.05, 0.75, "< Z vertex deviation may NOT be due to BBC, don't page expert easily! >");

      //for (int i = 0; i < 4; i++)
      for (int i = 0; i < 5; i++)//Run11 pp
        {
          ifnew( TText, TextZVertex[i] );
          TextZVertex[i]->SetTextColor(BBC_COLOR_ZVTX[i]);
          //TextZVertex[i]->SetTextSize(0.15);
          TextZVertex[i]->SetTextSize(0.08);

          ifnew( TText, TextZVertex_scale[i] );
          ifnew( TText, TextZVertex_mean[i] );
          TextZVertex_scale[i]->SetTextColor(BBC_COLOR_ZVTX[i]);
          TextZVertex_mean[i]->SetTextColor(BBC_COLOR_ZVTX[i]);
          TextZVertex_scale[i]->SetTextSize(0.08);
          TextZVertex_mean[i]->SetTextSize(0.08);

        }
      TextZVertex[0]->SetText(xpos[0], 0.65, "Zbbc [BBLL1]");//RUN11 pp
      TextZVertex[1]->SetText(xpos[1], 0.65, "Zzdc [ZDCLL1wide]");//RUN11 pp
      TextZVertex[2]->SetText(xpos[2], 0.65, "Zbbc [BBCLL1(noVtx)]");
      TextZVertex[3]->SetText(xpos[3], 0.65, "Zbbc [BBCLL1(narrowVtx)]");//Run11 pp
      TextZVertex[4]->SetText(0.06, 0.65, "Z    [Trigger]");//Run11 pp
      //TextZVertex[3]->SetText(0.06, 0.65, "Z    [Trigger]");//Run11 pp
      // TextZVertex[0]->SetText(0.20, 0.69, "Zbbc [BBLL1]");
      // TextZVertex[1]->SetText(0.35, 0.61, "Zzdc [ZDCLL1wide]");
      // TextZVertex[2]->SetText(0.50, 0.69, "Zbbc [BBCLL1(noVtxCut)]");
      //     TextZVertex[3]->SetText(0.65, 0.61, "Zbbc [BBCLL1(narrowVtxCut)]");
      //TextZVertex[3]->SetText(0.05, 0.65, "Z    [Trigger]");
      //     TextZVertex[4]->SetText(0.05, 0.65, "Z    [Trigger]");
      TC[3]->cd();
      transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1, 0, 0);
      transparent[3]->SetFillStyle(4000);
      transparent[3]->Draw();

      ifnew( TLine( BBC_MIN_REGULAR_ZVERTEX_MEAN, -6,
                    BBC_MIN_REGULAR_ZVERTEX_MEAN, 16 ),
             LineTzeroZvtx[0] );
      ifnew( TLine( BBC_MAX_REGULAR_ZVERTEX_MEAN, -6,
                    BBC_MAX_REGULAR_ZVERTEX_MEAN, 16 ),
             LineTzeroZvtx[1] );
      ifnew( TLine( -200, BBC_MIN_REGULAR_TDC1_MEAN - 5,
                    200, BBC_MIN_REGULAR_TDC1_MEAN - 5 ),
             LineTzeroZvtx[2] );
      ifnew( TLine( -200, BBC_MAX_REGULAR_TDC1_MEAN - 5,
                    200, BBC_MAX_REGULAR_TDC1_MEAN - 5 ),
             LineTzeroZvtx[3] );
      ifnew( TText, TextTzeroZvtx );

    }

  //

  /*
    if ( ( strlen(name) < 1 || strcmp(name, "BBCMon1") == 0 ) ||
    ( strlen(name) < 1 || strcmp(name, "BBCMon2") == 0 ) ||
    ( strlen(name) < 1 || strcmp(name, "BBCMon3") == 0 ) ||
    ( strlen(name) < 1 || strcmp(name, "BBCMon4") == 0 ) )
  */
  {
    ifnew( TPaveText(0.05, 0.65, 0.70, 0.95), PaveTop );
    PaveTop->AddText("BBC ONLINE MONITOR");
    ifnew( TText, TextTop );
    TextTop->SetTextSize(0.25);
  }


  cout << "Initialize completed" << endl;

  return 0;
}




/****************************************************************************************
 * Initialize BbcMonDraw class
 *
 *   Create Canvas if the canvas is not avaiable
 */
int BbcMonDraw::Init()
{
  if (!getenv("BBCCALIBRATION"))
    {
      cout << "environment variable BBCCALIBRATION not set" << endl;
      cout << "please source the setup_onlmon.csh script and try again" << endl;
      cout << "exiting NOW" << endl;
      exit(1);
    }

  PRINT_DEBUG("Initialize BbcMonDraw");
  // Load BBC Calib

  ifnew( BbcCalib() , bbccalib );

  // BBC Calib
  if (strcmp(getenv("BBCCALIBRATION"), "PDBCAL") == 0)
    {
      // store from PdbCal
      // time is from run 66302(Mon Jan 20 11:36:17 2003).
      PHTimeStamp time_bbccalib(2003, 1, 22, 12, 0, 0, 0);
      int calib_version = 1004;
      bbccalib->restore(time_bbccalib, calib_version);
    }
  else if (strcmp(getenv("BBCCALIBRATION"), "FILE") == 0)
    {
      // store from File
      //
      if (!getenv("BBCCALIBDIR"))
        {
          cout << "Environment variable BBCCALIBDIR not set" << endl;
          bbccalib->restore("./BbcCalib");
        }
      else
        {
          ostringstream otmp;
          otmp << getenv("BBCCALIBDIR") << "/BbcCalib";
          // need a cast from (const char *) to (char *)
          bbccalib->restore((char *) otmp.str().c_str());
          otmp.str("");
        }
    }

  tspec = new TSpectrum(5); // 5 peaks is enough - we have 4

  PRINT_DEBUG("Initialize BbcMonDraw completed");


  return 0;
}




int BbcMonDraw::Draw(const char *what)
{
  TStyle* oldStyle = gStyle;
  bbcStyle->cd();

  if (!gROOT->FindObject("BBCMon1"))
    {
      TC[0] = NULL;
      if (!strcmp(what, "ALL") || !strcmp(what, "BBCMon1") || !strcmp(what, "BbcMonitor") )
        {
          MakeCanvas("BBCMon1");
        }
    }
  if (! gROOT->FindObject("BBCMon2"))
    {
      TC[1] = NULL;
      if (!strcmp(what, "ALL") || !strcmp(what, "BBCMon2") || !strcmp(what, "VertexMonitor") )
        {
          MakeCanvas("BBCMon2");
        }
    }
  if (!gROOT->FindObject("BBCMon3"))
    {
      TC[2] = NULL;
      if (!strcmp(what, "ALL") || !strcmp(what, "BBCMon3") || !strcmp(what, "BbcMonitor") )
        {
          MakeCanvas("BBCMon3");
        }
    }
  // 4th canvas
  // if (! gROOT->FindObject("BBCMon4"))
  if (! gROOT->FindObject("BBCMon4"))
    {
      TC[3] = NULL;
      if (!strcmp(what, "ALL") || !strcmp(what, "BBCMon4") || !strcmp(what, "VertexMonitor"))
        {
          MakeCanvas("BBCMon4");
        }
    }
  // Histogram
  TH2 *bbc_adc;
  TH2 *bbc_tdc[nTDC];
  TH2 *bbc_tdc_overflow[nTDC];
  TH1 *bbc_tdc_overflow_each[nTDC][nPMT_BBC];
  TH1 *bbc_nhit[nTRIGGER];
  TH2 *bbc_tdc_armhittime;
  TH1 *bbc_zvertex;
  TH1 *bbc_zvertex_bbll1;
  TH1 *bbc_zvertex_zdc;
  TH1 *bbc_zvertex_zdc_scale3;
  TH1 *bbc_zvertex_bbll1_novtx;
  TH1 *bbc_zvertex_bbll1_narrowvtx;
  TH1 *bbc_zvertex_bbll1_zdc;
  TH1 *bbc_nevent_counter;
  TH2 *bbc_tzero_zvtx;
  TH1 *bbc_avr_hittime;
  TH1 *bbc_south_hittime;
  TH1 *bbc_north_hittime;
  TH1 *bbc_south_chargesum;
  TH1 *bbc_north_chargesum;
  TH1 *bbc_prescale_hist;

  ClearWarning();

  ostringstream otext;
  string text;
  string textok;
  //ostringstream textok;
  ostringstream name;

  // ---------------------------------------------------------------------------------
  // get pointer for each histrams

  OnlMonClient *cl = OnlMonClient::instance();

  PRINT_DEBUG("Start Getting Histogram");

  for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
    {
      name << "bbc_tdc" << tdc ;
      bbc_tdc[tdc] = static_cast<TH2 *> (cl->getHisto(name.str().c_str()));
      name.str("");

      name << "bbc_tdc" << tdc << "_overflow" ;
      bbc_tdc_overflow[tdc] =  static_cast<TH2 *> (cl->getHisto(name.str().c_str()));
      name.str("");

      for ( int i = 0 ; i < nPMT_BBC ; i++ )
        {
          name << "bbc_tdc" << tdc << "_overflow_" << setw(3) << setfill('0') << i ;
          bbc_tdc_overflow_each[tdc][i] = cl->getHisto(name.str().c_str());
          name.str("");
        }
    }

  bbc_adc =  static_cast<TH2 *> (cl->getHisto("bbc_adc"));
  ifdelete( Adc );
  for (int i = 0; i < nCANVAS; i++)
    {
      if ( TC[i] )
        {
          transparent[i]->Clear(); // clear dead server msg if it was printed before
        }
    }

  if ( bbc_adc )
    {
      Adc =  static_cast<TH2 *> (bbc_adc->Clone());
    }
  else
    {
      // here I assume server is dead and there must be a message saying so
      for (int i = 0; i < nCANVAS; i++)
        {
          if (TC[i])
            {
              TC[i]->cd();
              TC[i]->Clear("D");
              DrawDeadServer(transparent[i]);
              TC[i]->Update();
            }
        }
      return -1;
    }
  for ( int trig = 0 ; trig < nTRIGGER ; trig++ )
    {
      name << "bbc_nhit_" << TRIGGER_str[trig] ;
      bbc_nhit[trig] =  static_cast<TH2 *> (cl->getHisto(name.str().c_str()));
      name.str("");
    }

  bbc_tdc_armhittime =  static_cast<TH2 *> (cl->getHisto("bbc_tdc_armhittime"));
  ifdelete( ArmHit );
  ArmHit =  static_cast<TH2 *> (bbc_tdc_armhittime->Clone());

  bbc_zvertex = cl->getHisto("bbc_zvertex");
  ifdelete( Zvtx );
  Zvtx =  static_cast<TH1 *> (bbc_zvertex->Clone());

  bbc_zvertex_bbll1 = cl->getHisto("bbc_zvertex_bbll1");
  ifdelete( Zvtx_bbll1 );
  Zvtx_bbll1 =  static_cast<TH1 *> (bbc_zvertex_bbll1->Clone());

  bbc_zvertex_zdc = cl->getHisto("bbc_zvertex_zdc");
  ifdelete( Zvtx_zdc );
  Zvtx_zdc =  static_cast<TH1 *> (bbc_zvertex_zdc->Clone());

  bbc_zvertex_zdc_scale3 = cl->getHisto("bbc_zvertex_zdc_scale3");
  ifdelete( Zvtx_zdc_scale3 );
  Zvtx_zdc_scale3 =  static_cast<TH1 *> (bbc_zvertex_zdc_scale3->Clone());

  bbc_zvertex_bbll1_novtx = cl->getHisto("bbc_zvertex_bbll1_novtx");
  ifdelete( Zvtx_bbll1_novtx );
  Zvtx_bbll1_novtx =  static_cast<TH1 *> (bbc_zvertex_bbll1_novtx->Clone());

  bbc_zvertex_bbll1_narrowvtx = cl->getHisto("bbc_zvertex_bbll1_narrowvtx");
  ifdelete( Zvtx_bbll1_narrowvtx );
  Zvtx_bbll1_narrowvtx =  static_cast<TH1 *> (bbc_zvertex_bbll1_narrowvtx->Clone());

  bbc_zvertex_bbll1_zdc = cl->getHisto("bbc_zvertex_bbll1_zdc");
  ifdelete( Zvtx_bbll1_zdc );
  Zvtx_bbll1_zdc =  static_cast<TH1 *> (bbc_zvertex_bbll1_zdc->Clone());

  bbc_nevent_counter = cl->getHisto("bbc_nevent_counter");

  bbc_tzero_zvtx = static_cast<TH2 *> (cl->getHisto("bbc_tzero_zvtx"));
  ifdelete( TzeroZvtx );
  TzeroZvtx =  static_cast<TH2 *> (bbc_tzero_zvtx->Clone());

  bbc_avr_hittime = cl->getHisto("bbc_avr_hittime");
  ifdelete( AvrHitTime );
  AvrHitTime =  static_cast<TH1 *> (bbc_avr_hittime->Clone());

  bbc_north_hittime = cl->getHisto("bbc_north_hittime");
  ifdelete( NorthHitTime );
  NorthHitTime =  static_cast<TH1 *> (bbc_north_hittime->Clone());

  bbc_south_hittime = cl->getHisto("bbc_south_hittime");
  ifdelete( SouthHitTime );
  SouthHitTime =  static_cast<TH1 *> (bbc_south_hittime->Clone());

  bbc_south_chargesum = cl->getHisto("bbc_south_chargesum");
  ifdelete( SouthChargeSum );
  SouthChargeSum =  static_cast<TH1 *> (bbc_south_chargesum->Clone());

  bbc_north_chargesum = cl->getHisto("bbc_north_chargesum");
  ifdelete( NorthChargeSum );
  NorthChargeSum =  static_cast<TH1 *> (bbc_north_chargesum->Clone());

  bbc_prescale_hist = cl->getHisto("bbc_prescale_hist");
  ifdelete( Prescale_hist );
  Prescale_hist =  static_cast<TH1 *> (bbc_prescale_hist->Clone());

  PRINT_DEBUG("Start Creating graphs");

  // Create HitTime projection ------------------------------------------

  for ( int side = 0 ; side < nSIDE ; side++ )
    {
      for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
        {
          ifdelete( HitTime[tdc][side] );

          name << "FitHitTime" << SIDE_Str[side] << tdc ;
          HitTime[tdc][side] = bbc_tdc[tdc]->ProjectionY(name.str().c_str(), side * 64, side * 64 + 63);
          name.str("");

	  name << SIDE_Str[side] << " BBC TDC" << tdc << " Distribution" ;
	  //name << SIDE_Str[side] << " BBC TDC" << tdc << " Distribution(Trigger:BBLL1)" ; // Run14 AuAu 15GeV 2014.02.23
          HitTime[tdc][side]->SetTitle(name.str().c_str());
          ifnew( TF1(name.str().c_str(), "gaus"), FitHitTime[tdc][side] );
          name.str("");
          FitHitTime[tdc][side]->SetLineWidth(1); // 0.05 was used fro run3
          FitHitTime[tdc][side]->SetLineColor(4);
        }
    }

  // Create ZVertex Fit

  ifnew( TF1("FitZvtx" , "gaus"), FitZvtx );
  ifnew( TF1("FitAvrHitTime" , "gaus"), FitAvrHitTime );
  ifnew( TF1("FitNorthHitTime" , "gaus"), FitNorthHitTime );
  ifnew( TF1("FitSouthHitTime" , "gaus"), FitSouthHitTime );
  FitZvtx->SetLineWidth(1); // 0.05 was used for run3
  FitZvtx->SetLineColor(4);


  // ------------------------------------------------------------------------------
  // making nHit TGraph

  double pmt[ nPMT_1SIDE_BBC];
  double zero[nPMT_1SIDE_BBC];
  double nhitPmt[nTRIGGER][nSIDE][nPMT_1SIDE_BBC];
  double nhit_total = bbc_nevent_counter->GetBinContent(1);
  double nhit[nTRIGGER];
  nhit[0] = bbc_nevent_counter->GetBinContent(2);
  nhit[1] = bbc_nevent_counter->GetBinContent(3);


  for ( int i = 0 ; i < nPMT_1SIDE_BBC ; i++ )
    {
      pmt[i] = i + 1;
      zero[i] = 0;
    }


  PRINT_DEBUG("Creating nHit Grapth");

  for ( int side = 0 ; side < nSIDE ; side++ )
    {
      for ( int trig = 0 ; trig < nTRIGGER ; trig++ )
        {
          for ( int i = 0 ; i < nPMT_1SIDE_BBC ; i++ )
            {
              if ( nhit[trig] != 0 )
                {
                  //RUN12: to ignore laser rate for ch8, fiber is broken.
                  if (side == 0 && trig == 1 && i == 7 )
                    {
                      nhitPmt[trig][side][i] = 999;
                    }
                  else
                    {
                      nhitPmt[trig][side][i] = bbc_nhit[trig]->GetBinContent(i + side * nPMT_1SIDE_BBC + 1) / nhit[trig];
                    }
                }
              else
                {
                  nhitPmt[trig][side][i] = 0;
                }
            }
          ifnew( TGraph(nPMT_1SIDE_BBC, pmt, nhitPmt[trig][side] ), nHit[trig][side] );
          // nHit[trig][side]->GetXaxis()->SetTitleSize(  0.05);
          // nHit[trig][side]->GetYaxis()->SetTitleSize(  0.05);
          // nHit[trig][side]->GetXaxis()->SetTitleOffset(0.70);
          // nHit[trig][side]->GetYaxis()->SetTitleOffset(1.75);
        }
    }
  PRINT_DEBUG("Creating OverFlow Grapth");

  // Create TDC Overflow Graph

  double tdcOverMean[nTDC][nSIDE][nPMT_1SIDE_BBC];
  double tdcOverErrY[nTDC][nSIDE][nPMT_1SIDE_BBC];
  for (int itdc = 0; itdc < nTDC; itdc++)
    {
      for (int iside = 0; iside < nSIDE; iside++)
        {
          for (int i = 0; i < nPMT_1SIDE_BBC; i++)
            {
              tdcOverMean[itdc][iside][i] = 0.0;
              tdcOverErrY[itdc][iside][i] = 0.0;
            }
        }
    }

  for ( int side = 0 ; side < nSIDE ; side++ )
    {
      for ( int i = 0 ; i < nPMT_1SIDE_BBC ; i++ )
        {
          tdcOverMean[0][side][i] = bbc_tdc_overflow_each[0][i + side * nPMT_1SIDE_BBC]->GetMean() /
	    bbccalib->getOverflow0()->getCalibPar(i)->getDeviation();
          tdcOverErrY[0][side][i] = bbc_tdc_overflow_each[0][i + side * nPMT_1SIDE_BBC]->GetRMS() /
	    bbccalib->getOverflow0()->getCalibPar(i)->getDeviation();

          tdcOverMean[1][side][i] = bbc_tdc_overflow_each[1][i + side * nPMT_1SIDE_BBC]->GetMean() /
	    bbccalib->getOverflow1()->getCalibPar(i)->getDeviation();
          tdcOverErrY[1][side][i] = bbc_tdc_overflow_each[1][i + side * nPMT_1SIDE_BBC]->GetRMS() /
	    bbccalib->getOverflow1()->getCalibPar(i)->getDeviation();

          for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
            {
              ifnew( TGraphErrors(nPMT_1SIDE_BBC, pmt,
                                  tdcOverMean[tdc][side], zero,
                                  tdcOverErrY[tdc][side] ), TdcOver[tdc][side] );
            }
        }
    }

  // Redraw each Pad

  // ------------------------------------------------------------------------
  // Draw 1st Page
  // ------------------------------------------------------------------------
  ostringstream msg;
  PRINT_DEBUG("Drawing Graphs on Canvas");

  // Make TopPave
  time_t evttime = cl->EventTime("CURRENT");
  otext.str("");
  otext << "Run #" << cl->RunNumber();
  otext << " Events: " << nhit_total;
  otext << " Date:" << ctime(&evttime) ;
  text = otext.str();
  TextTop->SetText(0.01, 0.25, text.c_str());


  if ( TC[0] )
    {
      TC[0]->cd();

      // -------------------------------------------------------------------------
      PadTop[0]->cd();
      PaveTop->Draw();
      TextTop->Draw();


      // TDC

      for ( int side = 0 ; side < nSIDE ; side++ )
        {
          for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
            {
              PadTdcOver[tdc][side]->cd();
              // ifdelete( FrameTdcOver[tdc][side] );
              //FrameTdcOver[tdc][side] = TC[0]->DrawFrame( 0.5, -5, 64.5, 5);
              FrameTdcOver[tdc][side] = gPad->DrawFrame( 0.5, -5, 64.5, 5);

              BoxTdcOver[tdc][side]->Draw();

	      name << SIDE_Str[side] << " BBC TDC" << tdc << " Distribution" ;
              FrameTdcOver[tdc][side]->SetTitle(name.str().c_str());
              name.str("");

              FrameTdcOver[tdc][side]->GetXaxis()->SetTitle("PMT Number");
              FrameTdcOver[tdc][side]->GetXaxis()->SetTitleSize(0.05);
              FrameTdcOver[tdc][side]->GetXaxis()->SetLabelSize(0.05);
              FrameTdcOver[tdc][side]->GetYaxis()->SetTitle("Deviation [#sigma]");
              FrameTdcOver[tdc][side]->GetYaxis()->SetTitleSize( 0.07);
              FrameTdcOver[tdc][side]->GetYaxis()->SetTitleOffset(0.50);
              FrameTdcOver[tdc][side]->GetYaxis()->SetLabelSize( 0.05);
              TdcOver[tdc][side]->SetMarkerStyle(21);
              TdcOver[tdc][side]->SetMarkerSize(0.5);
              TdcOver[tdc][side]->SetMarkerColor(2);
              TdcOver[tdc][side]->Draw("P");

              // Check Warning
              if ( nhit[0] > 100 )
                {
                  for ( int i = 0 ; i < nPMT_1SIDE_BBC ; i++ )
                    {
                      if ( tdcOverMean[tdc][side][i] == 0 && tdcOverErrY[tdc][side][i] == 0 )
                        {
                          msg.str("");
                          msg << "Stop the run (ch " << i + 1 << " is out of the range)" ;
                          string wmsg = msg.str();
                          Warning( PadTdcOver[tdc][side], i, tdcOverMean[tdc][side][i], 1, wmsg);
                          wmsg.erase();
                          msg.str("");
                        }
                      if ( tdcOverMean[tdc][side][i] < BBC_TDC_OVERFLOW_REGULAR_MIN )
                        {
                          msg.str("");
                          msg << "ch " << i + 1 << " is too low ( " << fixed << setprecision(1) << tdcOverMean[tdc][side][i] << " #sigma)" ;
                          string wmsg = msg.str();
                          Warning( PadTdcOver[tdc][side], i, tdcOverMean[tdc][side][i], 1, wmsg);
                          wmsg.erase();
                          msg.str("");
                        }
                      if ( tdcOverMean[tdc][side][i] > BBC_TDC_OVERFLOW_REGULAR_MAX )
                        {
                          msg.str("");
                          msg << "ch " << i + 1 << " is too high ( " << fixed << setprecision(1) << tdcOverMean[tdc][side][i] << " #sigma)" ;
                          string wmsg = msg.str();
                          Warning( PadTdcOver[tdc][side], i, tdcOverMean[tdc][side][i], 1, wmsg);
                          wmsg.erase();
                          msg.str("");
                        }
                      if ( tdcOverMean[tdc][side][i] > BBC_TDC_OVERFLOW_REGULAR_RMS_MAX )
                        {
                          msg.str("");
                          msg << "ch " << i + 1 << " is too wide ( " << fixed << setprecision(1) << tdcOverErrY[tdc][side][i] << " #sigma)" ;
                          string wmsg = msg.str();
                          Warning( PadTdcOver[tdc][side], i, tdcOverMean[tdc][side][i], 1, wmsg);
                          wmsg.erase();
                          msg.str("");
                        }
                    }
                }
            }

          PadnHit[side]->cd();
          // ifdelete( FramenHit[side] );
          //FramenHit[side] = TC[0]->DrawFrame( 0.5, 0, 64.5, 1);
          FramenHit[side] = gPad->DrawFrame( 0.5, 0, 64.5, 1);
          BoxnHit[0][side]->Draw();
          BoxnHit[1][side]->Draw();

          name << SIDE_Str[side] << " BBC number of Hit per Event" ;
          FramenHit[side]->SetTitle(name.str().c_str());
          name.str("");

          FramenHit[side]->GetXaxis()->SetTitle("PMT Number");
          FramenHit[side]->GetXaxis()->SetTitleSize(0.05);
          FramenHit[side]->GetXaxis()->SetLabelSize(0.05);
          FramenHit[side]->GetYaxis()->SetTitleSize(0.05);
          FramenHit[side]->GetYaxis()->SetLabelSize(0.05);
          nHit[0][side]->SetMarkerStyle(21);
          nHit[0][side]->SetMarkerSize(0.5);
          nHit[0][side]->SetMarkerColor(2);
          nHit[0][side]->Draw("P");
          nHit[1][side]->SetMarkerStyle(22);
          nHit[1][side]->SetMarkerSize(0.5);
          nHit[1][side]->SetMarkerColor(4);
          nHit[1][side]->Draw("P");


          for ( int i = 0 ; i < nPMT_1SIDE_BBC ; i++ )
            {
              if ( nhit[0] > 100 )
                {
                  if ( nhitPmt[0][side][i] < BBC_nHIT_MB_MIN[side] )
                    {
                      //RUN11: to ignore hit rate since ch29 before FEM input is dead.
                      //RUN11: to ignore hit rate since the gain for ch40 is unstable.
                      //if( (side==0)&&(i==29 || i==40))
                      //{
                      //if(i == 29){
                      //   cout << "ch29 : Ignore hit rate into ch29" << endl;
                      //}
                      //else {
                      //   cout << "ch40 : Ignore hit rate into ch40" << endl;
                      //}
                      //	continue;
                      //}

                      msg.str("");
                      msg << "Too low hit-rate into ch " << i + 1 << " ("
			  << fixed << setprecision(2) << nhitPmt[0][side][i]
			  << "/"   << setprecision(0) << nhit[0] << "evt)" ;
                      string wmsg = msg.str();
                      Warning( PadnHit[side], i, nhitPmt[0][side][i], 1, wmsg);
                      wmsg.erase();
                      msg.str("");

                    }
                  if ( nhitPmt[0][side][i] > BBC_nHIT_MB_MAX[side] )
                    {
                      msg.str("");
                      msg << "Too high hit-rate into ch " << i + 1 << " ("
			  << fixed << setprecision(2) << nhitPmt[0][side][i]
			  << "/"   << setprecision(0) << nhit[0] << "evt)" ;
                      string wmsg = msg.str();
                      Warning( PadnHit[side], i, nhitPmt[0][side][i], 1, wmsg);
                      wmsg.erase();
                      msg.str("");
                    }
                }

              if ( nhit[1] > 0 )
                {
                  if ( nhitPmt[1][side][i] < BBC_nHIT_LASER_MIN[side] )
                    {
                      if (side == 0)
                        {
                          //RUN12: to ignore lack of laser rate, since fiber of ch7 is broken.
                          if (i == 7)
                            {
                              cout << "ch7(S8) : Ignore hit rate of laser" << endl;
                              continue;
                            }
                          msg.str("");
                          msg << "Lack of laser's hit into ch " << i + 1 << " ("
			      << fixed << setprecision(2) << nhitPmt[1][side][i]
			      << "/"   << setprecision(0) << nhit[1] << "evt)" ;
                          string wmsg = msg.str();
                          Warning( PadnHit[side], i, nhitPmt[1][side][i], 1, wmsg);
                          wmsg.erase();
                          msg.str("");
                        }
                      else if (side == 1) //North
                        {
                          msg.str("");
                          msg << "Lack of laser's hit into ch " << i + 1 << " ("
			      << fixed << setprecision(2) << nhitPmt[1][side][i]
			      << "/"   << setprecision(0) << nhit[1] << "evt)" ;
                          string wmsg = msg.str();
                          Warning( PadnHit[side], i, nhitPmt[1][side][i], 1, wmsg);
                          wmsg.erase();
                          msg.str("");
                        }
                    }
                }
            }
          //if ( nhit[0] == 0 )
          //Warning( PadnHit[side], 1, 0, 1, "No Event" );
          //if ( nhit[1] == 0 )
          //Warning( PadnHit[side], 1, 0, 1, "No Laser Event" );
        }


      PadnHitStatus->cd();
      TextnHitStatus->Draw();

      //  cout << "Got Histgram Got-Pad 0" << endl;
    }

  // ------------------------------------------------------------------------
  // Draw 2nd Page
  // ------------------------------------------------------------------------
  if ( TC[1] )
    {
      TC[1]->cd();

      PadTop[1]->cd();
      PaveTop->Draw();
      TextTop->Draw();




      if ( PadAvrHitTime )
        {
          PadAvrHitTime->cd();
          AvrHitTime->Draw();

          float rangemin;
          float rangemax;
	  int npeak = tspec->Search(AvrHitTime,2,"goff"); // finds the highest peak, draws marker
	  if (npeak < 3) // no center peak
	    {
	      AvrHitTime->Fit("FitAvrHitTime", "QN0L");
	      rangemin = FitAvrHitTime->GetParameter(1) - FitAvrHitTime->GetParameter(2);
              rangemax = FitAvrHitTime->GetParameter(1) + FitAvrHitTime->GetParameter(2);
	    }
       	  else
	    {
	      float *peakpos = tspec->GetPositionX();
	      float centerpeak = peakpos[0];
	      float sidepeak[2];
	      if (peakpos[2] > peakpos[1])
		{
		  sidepeak[0] = peakpos[1];
		  sidepeak[1] = peakpos[2];
		}
	      else
		{
		  sidepeak[1] = peakpos[1];
		  sidepeak[0] = peakpos[2];
		}
	      rangemin = centerpeak - (centerpeak-sidepeak[0])/2.;
	      rangemax = centerpeak + (sidepeak[1]-centerpeak)/2.;
	    }

	  FitAvrHitTime->SetRange(rangemin,rangemax);
          AvrHitTime->Fit("FitAvrHitTime", "QRL");
          FitAvrHitTime->Draw("same");

          float height = AvrHitTime->GetMaximum();
          FitAvrHitTime->Draw("same");

          LineAvrHitTime[1]->SetY2(height);
          LineAvrHitTime[0]->SetY2(height);
          ArrowAvrHitTime->SetY1(height*0.90);
          ArrowAvrHitTime->SetY2(height*0.90);
          TextAvrHitTime->SetY( height*0.88);
          LineAvrHitTime[0]->Draw();
          LineAvrHitTime[1]->Draw();
          ArrowAvrHitTime->Draw();
          TextAvrHitTime->Draw();

        }


      if ( PadSouthHitTime )
        {
          PadSouthHitTime->cd();
          SouthHitTime->Draw();
          float rangemin;
          float rangemax;
	  int npeak = tspec->Search(SouthHitTime,2,"goff"); // finds the highest peak, draws marker
	  if (npeak < 3) // no center peak
	    {
	      SouthHitTime->Fit("FitSouthHitTime", "QN0L");
	      rangemin = FitSouthHitTime->GetParameter(1) - FitSouthHitTime->GetParameter(2);
              rangemax = FitSouthHitTime->GetParameter(1) + FitSouthHitTime->GetParameter(2);
	    }
       	  else
	    {
	      float *peakpos = tspec->GetPositionX();
	      float centerpeak = peakpos[0];
	      float sidepeak[2];
	      if (peakpos[2] > peakpos[1])
		{
		  sidepeak[0] = peakpos[1];
		  sidepeak[1] = peakpos[2];
		}
	      else
		{
		  sidepeak[1] = peakpos[1];
		  sidepeak[0] = peakpos[2];
		}
	      rangemin = centerpeak - (centerpeak-sidepeak[0])/2.;
	      rangemax = centerpeak + (sidepeak[1]-centerpeak)/2.;
	    }

	  FitSouthHitTime->SetRange(rangemin,rangemax);
          SouthHitTime->Fit("FitSouthHitTime", "QRL");
          FitSouthHitTime->Draw("same");

          float height = SouthHitTime->GetMaximum();
          FitSouthHitTime->Draw("same");

          LineSouthHitTime[1]->SetY2(height);
          LineSouthHitTime[0]->SetY2(height);
          ArrowSouthHitTime->SetY1(height*0.90);
          ArrowSouthHitTime->SetY2(height*0.90);
          TextSouthHitTime->SetY( height*0.88);
          LineSouthHitTime[0]->Draw();
          LineSouthHitTime[1]->Draw();
          ArrowSouthHitTime->Draw();
          TextSouthHitTime->Draw();

        }

      if ( PadNorthHitTime )
        {
          PadNorthHitTime->cd();
          NorthHitTime->Draw();
          float rangemin;
          float rangemax;
	  int npeak = tspec->Search(NorthHitTime,2,"goff"); // finds the highest peak, draws marker
	  if (npeak < 3) // no center peak
	    {
	      NorthHitTime->Fit("FitNorthHitTime", "QN0L");
	      rangemin = FitNorthHitTime->GetParameter(1) - FitNorthHitTime->GetParameter(2);
              rangemax = FitNorthHitTime->GetParameter(1) + FitNorthHitTime->GetParameter(2);
	    }
       	  else
	    {
	      float *peakpos = tspec->GetPositionX();
	      float centerpeak = peakpos[0];
	      float sidepeak[2];
	      if (peakpos[2] > peakpos[1])
		{
		  sidepeak[0] = peakpos[1];
		  sidepeak[1] = peakpos[2];
		}
	      else
		{
		  sidepeak[1] = peakpos[1];
		  sidepeak[0] = peakpos[2];
		}
	      rangemin = centerpeak - (centerpeak-sidepeak[0])/2.;
	      rangemax = centerpeak + (sidepeak[1]-centerpeak)/2.;
	    }

	  FitNorthHitTime->SetRange(rangemin,rangemax);
          NorthHitTime->Fit("FitNorthHitTime", "QRL");
          FitNorthHitTime->Draw("same");

          float height = NorthHitTime->GetMaximum();
          FitNorthHitTime->Draw("same");

          LineNorthHitTime[1]->SetY2(height);
          LineNorthHitTime[0]->SetY2(height);
          ArrowNorthHitTime->SetY1(height*0.90);
          ArrowNorthHitTime->SetY2(height*0.90);
          TextNorthHitTime->SetY( height*0.88);
          LineNorthHitTime[0]->Draw();
          LineNorthHitTime[1]->Draw();
          ArrowNorthHitTime->Draw();
          TextNorthHitTime->Draw();

        }

      PadArmHit->cd();
      if ( ArmHit )
        {
          ArmHit->Draw("colz");
          ArcArmHit->Draw();
          TextArmHit->Draw();
        }


      PadBbcSummary->cd();
      otext.str("");
      otext << "South:" << ((float)int(FitSouthHitTime->GetParameter(1)*10)) / 10 << "[ns]  ";
      otext << "North:" << ((float)int(FitNorthHitTime->GetParameter(1)*10)) / 10 << "[ns]  ";
      otext << "...  ";
      if (BBC_MIN_REGULAR_TDC0_MEAN < FitNorthHitTime->GetParameter(1) &&
          BBC_MAX_REGULAR_TDC0_MEAN > FitNorthHitTime->GetParameter(1) &&
          BBC_MIN_REGULAR_TDC0_MEAN < FitSouthHitTime->GetParameter(1) &&
          BBC_MAX_REGULAR_TDC0_MEAN > FitSouthHitTime->GetParameter(1))
        //otext << "OK";
        textok = "                                                         OK";
      else
        {
          textok = " ";
          if ( BBC_MIN_WORNING_STATISTICS_FOR_ZVERTEX_MEAN > Zvtx->GetEntries() )
            otext << "Too low statistics";
          else
            otext << "Change Global-Offset on V124";
        }
      text = otext.str();
      TextBbcSummaryHitTime[0]->SetText(0.01, 0.75, text.c_str() );
      TextBbcSummaryHitTime[0]->Draw();
      text = textok.c_str();
      //TextBbcSummaryHitTime[1]->SetText(0.65, 0.75, text.c_str() );
      TextBbcSummaryHitTime[1]->SetText(0.01, 0.75, text.c_str() );
      TextBbcSummaryHitTime[1]->SetTextColor(3);
      TextBbcSummaryHitTime[1]->Draw();

      // Global offset
      float delay = (BBC_DEFAULT_OFFSET -
                     ((FitNorthHitTime->GetParameter(1) + FitSouthHitTime->GetParameter(1)) * 0.5)); //[ns]
      otext.str("");
      otext << "   Global offset : ";
      if ( int( -2*delay) == 0 )
        {
          //otext << "need not to move ... OK";
          //otext << " ... OK";
          textok = "                                    ... OK";
        }
      else
        {
          textok = " ";
          if (!(BBC_MIN_REGULAR_TDC0_MEAN < FitNorthHitTime->GetParameter(1) &&
                BBC_MAX_REGULAR_TDC0_MEAN > FitNorthHitTime->GetParameter(1) &&
                BBC_MIN_REGULAR_TDC0_MEAN < FitSouthHitTime->GetParameter(1) &&
                BBC_MAX_REGULAR_TDC0_MEAN > FitSouthHitTime->GetParameter(1)))
            {
              //otext << "Need to move " << int( -2*delay) << "count";
              otext << "Need to call BBC expert: " << int( -2*delay) << "count shifted";
            }
          else
            {
              //otext << " ... OK";
              textok = "                            OK";
            }
        }
      //otext << "...     )" ;
      text = otext.str();
      TextBbcSummaryGlobalOffset[0]->SetText(0.01, 0.50, text.c_str() );
      TextBbcSummaryGlobalOffset[0]->Draw();
      text = textok;
      //TextBbcSummaryGlobalOffset[1]->SetText(0.35, 0.50, text.c_str() );
      TextBbcSummaryGlobalOffset[1]->SetText(0.01, 0.50, text.c_str() );
      TextBbcSummaryGlobalOffset[1]->SetTextColor(3);
      TextBbcSummaryGlobalOffset[1]->Draw();
      textok = " ";


      // ZVertex
      /*
	otext.str("");
	otext << "Mean ZVertex:" << ((float)int(FitZvtx->GetParameter(1)*10)) / 10 << "[cm] ";
	if(Zvtx->GetEntries() > BBC_MIN_WORNING_STATISTICS_FOR_ZVERTEX_MEAN ) {
	otext << "( sigma " << ((float)int(FitZvtx->GetParameter(2)*10)) / 10 << "cm)";
	}else{
	// otext << "( RMS " << ((float)int(Zvtx->GetRMS()*10)) / 10 << "cm)";
	}
	otext << " ... ";
	if ( BBC_MIN_REGULAR_ZVERTEX_MEAN < FitZvtx->GetParameter(1) &&
	BBC_MAX_REGULAR_ZVERTEX_MEAN > FitZvtx->GetParameter(1) )
	otext << "OK";
	else {
	if( BBC_MIN_WORNING_STATISTICS_FOR_ZVERTEX_MEAN > Zvtx->GetEntries() )
	otext << "Too low statistics";
	else
	otext << "Ask SL to contact MCR";
	}
	text = otext.str();
	TextBbcSummaryZvertex->SetText(0.01, 0.25, text.c_str() );
      */

      TextBbcSummaryZvertex->SetText(0.01, 0.25, "Shown data are triggered by BBLL1 |z|<130cm" );
      TextBbcSummaryZvertex->Draw();
    }

  // ------------------------------------------------------------------------
  // Draw 3rd Page
  // ------------------------------------------------------------------------
  if ( TC[2] )
    {
      TC[2]->cd();

      PadTop[2]->cd();
      PaveTop->Draw();
      TextTop->Draw();

      for ( int tdc = 1 ; tdc <= 1 ; tdc++ )
        {
          for ( int side = 0 ; side < nSIDE ; side++ )
            {
              PadHitTime[tdc][side]->cd();

              HitTime[tdc][side]->Draw();
	      float rangemin;
	      float rangemax;
	      int npeak = tspec->Search(HitTime[tdc][side],2,"goff"); // finds the highest peak, draws marker
	      if (npeak < 3) // no center peak
		{
		  FitHitTime[tdc][side]->SetRange( TDC_FIT_MIN, TDC_FIT_MAX );
		  HitTime[tdc][side]->Fit( FitHitTime[tdc][side]->GetName(), "QRNL");
		  rangemax = min( TDC_FIT_MAX ,
				  FitHitTime[tdc][side]->GetParameter(1) + FitHitTime[tdc][side]->GetParameter(2) );
		  rangemin = max( TDC_FIT_MIN ,
				  FitHitTime[tdc][side]->GetParameter(1) - FitHitTime[tdc][side]->GetParameter(2) );
		}
	      else
		{
		  float *peakpos = tspec->GetPositionX();
		  float centerpeak = peakpos[0];
		  float sidepeak[2];
		  if (peakpos[2] > peakpos[1])
		    {
		      sidepeak[0] = peakpos[1];
		      sidepeak[1] = peakpos[2];
		    }
		  else
		    {
		      sidepeak[1] = peakpos[1];
		      sidepeak[0] = peakpos[2];
		    }
		  rangemin = centerpeak - (centerpeak-sidepeak[0])/2.;
		  rangemax = centerpeak + (sidepeak[1]-centerpeak)/2.;
		}

              FitHitTime[tdc][side]->SetRange(rangemin, rangemax );

              HitTime[tdc][side]->Fit( FitHitTime[tdc][side]->GetName(), "QRL");
              float height = HitTime[tdc][side]->GetMaximum();
              FitHitTime[tdc][side]->Draw("same");

              LineHitTime[tdc][side][1]->SetY2(height);
              LineHitTime[tdc][side][0]->SetY2(height);
              ArrowHitTime[tdc][side ]->SetY1(height*0.90);
              ArrowHitTime[tdc][side ]->SetY2(height*0.90);
              TextHitTime[tdc][side ]->SetY( height*0.88);
              LineHitTime[tdc][side][0]->Draw();
              LineHitTime[tdc][side][1]->Draw();
              ArrowHitTime[tdc][side ]->Draw();
              TextHitTime[tdc][side ]->Draw();
            }
        }

      // PadWarnings->cd();
      // PaveWarnings->Draw();

      if ( PadZvtx )
        {
          PadZvtx->cd();
          Zvtx->Draw();
          Zvtx->Fit("FitZvtx", "QN0L");
          FitZvtx->SetRange(FitZvtx->GetParameter(1) - FitZvtx->GetParameter(2)*2,
                            FitZvtx->GetParameter(1) + FitZvtx->GetParameter(2)*2 );
          Zvtx->Fit("FitZvtx", "QRL");
          FitZvtx->Draw("same");

          float height = Zvtx->GetMaximum();
          FitZvtx->Draw("same");

          LineZvtx[1]->SetY2(height);
          LineZvtx[0]->SetY2(height);
          ArrowZvtx->SetY1(height*0.90);
          ArrowZvtx->SetY2(height*0.90);
          TextZvtx->SetY( height*0.88);
          LineZvtx[0]->Draw();
          LineZvtx[1]->Draw();
          ArrowZvtx->Draw();
          TextZvtx->Draw();
          TextZvtxNorth->Draw();
          TextZvtxSouth->Draw();

        }

      if ( PadChargeSum )
        {
          PadChargeSum->cd();
          NorthChargeSum->SetLineColor(4);
          SouthChargeSum->SetLineColor(2);
          NorthChargeSum->Draw();
          SouthChargeSum->Draw("same");

	  //       float height = NorthChargeSum->GetMaximum();
	  //       TextNorthChargeSum->SetY( height*0.88);
	  //       TextSouthChargeSum->SetY( height*0.88);
          TextNorthChargeSum->SetTextColor(4);
          TextSouthChargeSum->SetTextColor(2);
          TextNorthChargeSum->Draw();
          TextSouthChargeSum->Draw();
        }

      if ( PadAdc )
        {
          PadAdc->cd();
          Adc->Draw("colz");
        }

      //  bbc_t0_pave->Draw("same");

    }

  // ------------------------------------------------------------------------
  // Draw 4th Page
  // ------------------------------------------------------------------------
  bbcStyle->cd();
  if ( TC[3] )
    {
      PadTop[3]->cd();
      PaveTop->Draw();
      TextTop->Draw();

      PadZVertex->cd();

      Zvtx_bbll1->SetLineColor(4);
      Zvtx_bbll1->SetFillColor(4);
      Zvtx_zdc->SetLineColor(2);
      Zvtx_zdc_scale3->SetLineColor(2);
      Zvtx_bbll1_novtx->SetLineColor(7);
      Zvtx_bbll1_novtx->SetLineWidth(2);
      Zvtx_bbll1_narrowvtx->SetLineColor(6);//Run11 pp
      Zvtx_bbll1_narrowvtx->SetLineWidth(2);

      // Get Maximum at the inside of BBC which is 130cm from center;
      float maxEntries = 10;
      
      for ( int i = 0 ; i < Zvtx_bbll1->GetNbinsX() ; i++ )
        {
          if ( fabs(Zvtx_bbll1->GetBinCenter(i)) < 130 )
            {
              if ( maxEntries < Zvtx_bbll1->GetBinContent(i + 1) )
                {
                  maxEntries = Zvtx_bbll1->GetBinContent(i + 1);
                }
	      // 	if ( maxEntries < Zvtx_zdc->GetBinContent(i + 1) )
	      // 	{
	      // 	  maxEntries = Zvtx_zdc->GetBinContent(i + 1);
	      // 	}
	      // 	if ( maxEntries < Zvtx_bbll1_novtx->GetBinContent(i + 1) )
	      // 	{
	      // 	  maxEntries = Zvtx_bbll1_novtx->GetBinContent(i + 1);
	      // 	}
            }
        }

      // Fit No-Vertex Distribution
      FitZvtxBBLL1NoVtx->SetRange( -50, 50);
      FitZvtxBBLL1NoVtx->SetLineColor(7);
      Zvtx_bbll1_novtx->Fit("FitZvtxBBLL1NoVtx", "LRQ");

      // here we get the relative scaling right to put all on the same plot
      // the binning might be different, so we first find the bins corresponding to
      // +- 20cm and then get the Integral corrected for the binwidth
      // this is then used to get the histograms on the same scale
      int lowbin = Zvtx_zdc->GetXaxis()->FindBin(-20.);
      int hibin = Zvtx_zdc->GetXaxis()->FindBin(20.);
      double zdc_count = Zvtx_zdc->Integral(lowbin, hibin, "width");

      lowbin = Zvtx_bbll1_novtx->GetXaxis()->FindBin(-20.);
      hibin = Zvtx_bbll1_novtx->GetXaxis()->FindBin(20.);
      double bbc_count_novtx  = Zvtx_bbll1_novtx->Integral(lowbin, hibin, "width");
      ///RUN12
      // lowbin = Zvtx_bbll1_narrowvtx->GetXaxis()->FindBin(-20.);
      // hibin = Zvtx_bbll1_narrowvtx->GetXaxis()->FindBin(20.);
      //      double bbc_count_narrowvtx  = Zvtx_bbll1_narrowvtx->Integral(lowbin, hibin, "width");
      /////
      lowbin = Zvtx_bbll1->GetXaxis()->FindBin(-20.);
      hibin = Zvtx_bbll1->GetXaxis()->FindBin(20.);
      double bbc_count = Zvtx_bbll1->Integral(lowbin, hibin, "width");
      cout << "the ratio of integral (-30cm < ZVertex < 30cm) between BBLL1 without vtx cut and ZDC : " << bbc_count_novtx / zdc_count << endl ;
      //cout << "the ratio of integral (-30cm < ZVertex < 30cm) between BBLL1 without vtx cut  and BBLL1 with BBCZ < |30cm|  : " << bbc_count_novtx/bbc_count << endl ;
      // Draw ZVertex triggerd variable trigger
      Zvtx_bbll1->SetMaximum( maxEntries*1.05 );
      Zvtx_bbll1->SetTitle("Bbc ZVertex (south<-->north)");
      //PadZVertex->DrawFrame(-160,0,160,maxEntries*1.05,"Bbc ZVertex (south<-->north)");
      //cout << "maxEntries " << maxEntries << endl;
      //Zvtx_bbll1_novtx->Draw("hist");
      //Zvtx_bbll1->Scale(bbc_count_novtx/bbc_count);

      // just in case the bbcll1 with vtx cut histo is empty
      // draw the novertex cut (happens during setup)
      if (Zvtx_bbll1->GetEntries() > 0)
	{
	  Zvtx_bbll1->Draw("hist");
	  Zvtx_bbll1_novtx->Draw("samehist");
	}
      else
	{
	  Zvtx_bbll1_novtx->Draw("hist");
	}
      //Zvtx_bbll1_narrowvtx->Scale(bbc_count/bbc_count_narrowvtx);//Run12
      Zvtx_bbll1_narrowvtx->Draw("samehist");
      Zvtx_zdc->Scale(bbc_count / zdc_count);
      Zvtx_zdc->Draw("samehist");
      //FitZvtxBBLL1NoVtx->Draw("same");

      /* Old code by hachiya-san
         if ( Zvtx_bbll1->GetMaximum() > Zvtx_zdc_scale3->GetMaximum() )
         {
         Zvtx_bbll1->SetTitle("Bbc ZVertex");
         Zvtx_bbll1->Draw("hist");
         Zvtx_zdc_scale3->Draw("samehist");
         Zvtx_bbll1_novtx->Draw("samehist");
         }
         else
         {
         Zvtx_zdc_scale3->SetTitle("Bbc ZVertex");
         Zvtx_zdc_scale3->Draw("hist");
         Zvtx_bbll1->Draw("samehist");
         Zvtx_bbll1_novtx->Draw("samehist");
         }
      */

      // trigger rate between BBCLL1 and Zvertex within +- BBC_ZVERTEX_CUT_FOR_TRIG_RATE
      // bbll1, zdc, bbll1_novtx

      float trig_rate[3], trig_rate_err[3];
      double nevent[3];
      memset(trig_rate, 0, sizeof(trig_rate));
      memset(trig_rate_err, 0, sizeof(trig_rate_err));
      memset(nevent, 0, sizeof(nevent));

      CalculateTriggerRates(trig_rate[0], trig_rate_err[0], nevent[0],
                            trig_rate[1], trig_rate_err[1], nevent[1],
                            trig_rate[2], trig_rate_err[2], nevent[2]);

      float sigma_zdc = 0.0;
      float sigma_zdc_err = 0.0;
      float effic_bbc = 0.0;
      float effic_bbc_err = 0.0;
      float beamInZdc = 0.0;
      float beamInZdc_err = 0.0;
      float beamInBbc = 0.0;
      float beamInBbc_err = 0.0;

      BeamMonitoring( sigma_zdc, sigma_zdc_err, effic_bbc, effic_bbc_err,
                      beamInZdc, beamInZdc_err, beamInBbc, beamInBbc_err);

      ostringstream trig_rate_text;
      trig_rate_text << " #sigma_{ZDC} = "   << fixed << setprecision(3)  << sigma_zdc
		     << " #epsilon_{BBC} = "                                           << effic_bbc
		     << " {}^{beam in acceptance}_{       ZDC      } = "               << beamInZdc
		     << " {}^{beam in acceptance}_{       BBC      } = "               << beamInBbc
	;
      TextBbcSummaryTrigRate->SetText(0.02, 0.05, trig_rate_text.str().c_str());
      trig_rate_text.str("");

      //float xpos[3] = {0.1, 0.34, 0.58};
      //    float xpos[3] = {0.30, 0.50, 0.75};
      PadZVertexSummary->cd();
      TextZVertexExpress->Draw();
      //TextZVertexNotice->Draw();

      TH1 *Zvtx_array[4];//with narrow
      //TH1 *Zvtx_array[3];
      Zvtx_array[0] = Zvtx_bbll1;
      Zvtx_array[1] = Zvtx_zdc;
      Zvtx_array[2] = Zvtx_bbll1_novtx;
      Zvtx_array[3] = Zvtx_bbll1_narrowvtx;

      { // Show status of ZVertex
        int i = 0;
        //for ( i = 0; i < 3; i++)
        for ( i = 0; i < 4; i++)//Run11 pp narrow verex
          {
            TextZVertex[i]->Draw();


            // scale factor ---------------------------------------------------------------------
            //	cout << "  " << i << " " << Prescale_hist->GetBinContent(i + 1) << endl;
            otext.str("");
            //otext << "( "<< Prescale_hist->GetBinContent(i+1)<<" )";
            //otext << "( "<< Prescale_hist->GetBinContent(i+1)<<" )"<<" ";
            //otext << nevent[i] ;
            //otext.precision(8);
            otext << " ( " << Prescale_hist->GetBinContent(i + 1) << " ) " << " ";
            //otext << nevent[i]/Prescale_hist->GetBinContent(i+1) ;
            otext << Zvtx_array[i]->GetEntries() ;

            text = otext.str();
            TextZVertex_scale[i]->SetText(xpos[i], 0.50, text.c_str());
            TextZVertex_scale[i]->Draw();

            // mean and RMS ---------------------------------------------------------------------
            otext.str("");
            otext << ((float)int(Zvtx_array[i]->GetMean()*10)) / 10.0 << "cm ( "
		  << ((float)int(Zvtx_array[i]->GetRMS()*10)) / 10.0 << " cm) " ;
            text = otext.str();


            TextZVertex_mean[i]->SetText(xpos[i], 0.25, text.c_str());
            TextZVertex_mean[i]->Draw();
            //otext.precision(6);
          }

        TextZVertex[i]->Draw();
        TextZVertex_scale[i]->SetText(0.00, 0.50, "(Scale Fac.) #Evt.");
        TextZVertex_scale[i]->Draw();
        //TextZVertex_mean[i]->SetText(0.05, 0.25, "Vertex Mean (RMS)");
        TextZVertex_mean[i]->SetText(0.00, 0.25, "Vertex Mean (RMS)");
        TextZVertex_mean[i]->Draw();

        // Draw Status
        otext.str("");
        otext << " Z_{BBLL1 w/o Vtx}^{Fit}= " << ((float)int(FitZvtxBBLL1NoVtx->GetParameter(1)*10)) / 10.0
	  //	      << " #pm " << ((float)int(FitZvtxBBLL1NoVtx->GetParError(1)*10))/10.0
	      << " cm ( #sigma = " << int(FitZvtxBBLL1NoVtx->GetParameter(2))
	  //	      << " #pm " << ((float)int(FitZvtxBBLL1NoVtx->GetParError(2)*10))/10.0
	      << " cm) ...  ";
        if ( fabs(FitZvtxBBLL1NoVtx->GetParameter(1)) > 10)
          {
            textok = " ";
            // Seems wrong vertex
            if ( nevent[2] / Prescale_hist->GetBinContent(2 + 1) < 200 )
              {
                otext << "Low Statistics";
              }
            else
              {
                otext << "See Z_{ZDC} or Ask SL";
              }
          }
        else
          {
            //otext << "OK";
            textok = "                                                             OK";
          }

        text = otext.str();
        TextZVtxStatus[0]->SetText(0.0, 0.85, text.c_str());
        TextZVtxStatus[0]->SetTextSize(0.12);
        TextZVtxStatus[0]->Draw();
        text = textok;
        //TextZVtxStatus[1]->SetText(0.6, 0.85, text.c_str());
        TextZVtxStatus[1]->SetText(0.0, 0.85, text.c_str());
        TextZVtxStatus[1]->SetTextSize(0.12);
        TextZVtxStatus[1]->SetTextColor(3);
        TextZVtxStatus[1]->Draw();

        TextBbcSummaryTrigRate->Draw();
      }

      PadTzeroZVertex->cd();
      TzeroZvtx->Draw("colz");

      // insert line
      LineTzeroZvtx[0]->Draw();
      LineTzeroZvtx[1]->Draw();
      LineTzeroZvtx[2]->Draw();
      LineTzeroZvtx[3]->Draw();

      // insert text
      TextTzeroZvtx->SetText(10, 4, "Good region");
      TextTzeroZvtx->Draw();
    }

  if ( TC[0] )
    {
      TC[0]->Update();
    }
  if ( TC[1] )
    {
      TC[1]->Update();
    }
  if ( TC[2] )
    {
      TC[2]->Update();
    }
  if ( TC[3] )
    {
      TC[3]->Update();
    }

  // cout << "Got Histgram Got-Pad 1" << endl;

  oldStyle->cd();
  return 0;
}

/*

int BbcMonDraw::DrawTdcOverflow(
TPad* Pad, TH2* Hist,
TH1D *Hist1D[nPMT_1SIDE_BBC],
TArrow* ArrowLow, TText* TextLow,
TArrow* ArrowHi,  TText* TextHi
)
{
int i,j;
ostringstream otext;
string text;

Pad->cd();
Hist->SetFillColor(1);
Hist->SetLineColor(1);
Hist->Draw("BOX");

float vHi = -9999 , vLow = 9999; // Value
int iHi   =     0 , iLow =    0; // i
int rHi   =     0 , rLow =    0; // RMS
for( i=0 ; i<nPMT_1SIDE_BBC ; i++ )
{
TH1D *p = Hist->ProjectionY("p",i,i);
float v = p->GetMean();
if( v < vLow )
{
vLow = v;
iLow = i;
rLow = (int)p->GetRMS();
   }
   if( v > vHi )
   {
   vHi = v;
   iHi = i;
   rHi = (int)p->GetRMS();
   }
   delete p;
   }

   ArrowLow->SetY2( vLow+2*rLow );
   ArrowLow->SetX1( iLow );
   ArrowLow->SetX2( iLow );
   ArrowLow->SetOption( ">" );
   ArrowLow->SetLineWidth(2);
   ArrowLow->SetY1( VIEW_OVERFLOW_MAX - (VIEW_OVERFLOW_MAX-VIEW_OVERFLOW_MIN)*0.05 );
   ArrowLow->SetLineColor(4);
   ArrowLow->Draw();

   otext.str("");
   otext << "Lowest : " << ((float)int(vLow*10))/10 << "ch(N" << iLow << ")...";
   if( vHi < BBC_TDC_OVERFLOW_REGULAR_MAX )
   otext << "OK";
   else
   otext << "Call Expert";
   otext ;
   text = otext.str();
   TextLow->SetText( 3,
   VIEW_OVERFLOW_MAX -
   (VIEW_OVERFLOW_MAX-VIEW_OVERFLOW_MIN)*0.05,
   text.c_str() );
   TextLow->SetTextColor(4);
   TextLow->Draw();

   ArrowHi->SetY2( vHi-2*rHi );
   ArrowHi->SetX1( iHi );
   ArrowHi->SetX2( iHi );
   ArrowHi->SetOption( ">" );
   ArrowHi->SetLineWidth(2);
   ArrowHi->SetY1( VIEW_OVERFLOW_MIN + (VIEW_OVERFLOW_MAX-VIEW_OVERFLOW_MIN)*0.05 );
   ArrowHi->SetLineColor(2);
   ArrowHi->Draw();

otext.str("");
otext << "Highest : " << ((float)int(vHi*10))/10 << "ch(N" << iHi << ")...";
if( vHi < BBC_TDC_OVERFLOW_REGULAR_MAX )
otext << "OK";
else
otext << "Call Expert";
text = otext.str();
TextHi->SetText( 3,
    VIEW_OVERFLOW_MIN + (VIEW_OVERFLOW_MAX-VIEW_OVERFLOW_MIN)*0.01,
    text.c_str() );
TextHi->SetTextColor(2);
TextHi->Draw();

return 0;
}

*/


int BbcMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // iret != 0 means error during drawing
    {
      return iret;
    }

  // first canvas
  filename << ThisName << "1_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");

  // second canvas
  filename << ThisName << "2_" << cl->RunNumber() << ".ps";
  TC[1]->Print(filename.str().c_str());
  filename.str("");

  // third canvas
  filename << ThisName << "3_" << cl->RunNumber() << ".ps";
  TC[2]->Print(filename.str().c_str());
  filename.str("");

  // TC[3] is the canvas for the vertex monitor
  if (TC[3])
    {
      filename << ThisName << "4_" << cl->RunNumber() << ".ps";
      TC[3]->Print(filename.str().c_str());
      filename.str("");
    }

  return 0;
}

int BbcMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  vector<string> path;

  path.push_back("TDC Overflow and number of hit for each PMT");
  path.push_back("Timing Monitor");
  path.push_back("EXPERT/raw TDC&ADC ,charge sum and raw vertex");
  path.push_back("Vertex monitor");

  for ( size_t i = 0; i < 4; ++i )
    {
      if (TC[i])
        {
          ostringstream name;
          name << i;
          string imgpath = cl->htmlRegisterPage(*this, path[i], name.str(), "png");
          cl->CanvasToPng(TC[i], imgpath);
        }
    }
  cl->SaveLogFile(*this);
  return 0;
}


int BbcMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "BBCMONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}

void BbcMonDraw::CalculateTriggerRates(float &trig_rate0, float &trig_rate_err0, double &nevent0,
                                       float &trig_rate1, float &trig_rate_err1, double &nevent1,
                                       float &trig_rate2, float &trig_rate_err2, double &nevent2)
{
  // check the binning of histogram to integral.

  TH1 *h[5] = {Zvtx_bbll1, Zvtx_zdc, Zvtx_bbll1, Zvtx_zdc, Zvtx_bbll1_novtx};
  double trig_entry[5] = {0, 0, 0, 0, 0};
  // 0:bbll1(|Vtx|< 30cm),  1:zdc(30cm) 2:bbll1(|Vtx|<130cm),  3:zdc(130cm) 4:bbll1_novtx(130cm),
  double ZVERTEX_CUT[5] = {30., 30., 130., 130., 130.};

  // min limit for zvertex
  for (int ihist = 0; ihist < 5; ihist++)
    {
      int nbin = h[ihist]->GetNbinsX();

      for (int ibin = 0; ibin < nbin; ibin++)
        {

          if ( fabs( h[ihist]->GetBinCenter(ibin + 1) ) < ZVERTEX_CUT[ihist] )
            {
              trig_entry[ihist] += h[ihist]->GetBinContent(ibin + 1);
            }

        }
    }

  //BBLL1 / ZDC (|Vtx|<30cm)
  if (trig_entry[0] > 0 && trig_entry[1] > 0)
    {
      trig_rate0 = trig_entry[0] / trig_entry[1];
      trig_rate_err0 = trig_rate0 * ( ( (1.0 / trig_entry[0]) + (1.0 / trig_entry[1]) ) );
    }

  //BBLL1 / ZDC (|Vtx|<130cm)
  if (trig_entry[2] > 0 && trig_entry[3] > 0)
    {
      trig_rate1 = trig_entry[2] / trig_entry[3];
      trig_rate_err1 = trig_rate1 * ( ( (1.0 / trig_entry[2]) + (1.0 / trig_entry[3]) ) );
    }

  //BBLL1 / BBLL1(noVtxCut) (|Vtx|<130cm)
  if (trig_entry[2] > 0 && trig_entry[4] > 0)
    {
      trig_rate2 = trig_entry[2] / trig_entry[4];
      trig_rate_err2 = trig_rate2 * ( ( (1.0 / trig_entry[2]) + (1.0 / trig_entry[4]) ) );
    }

  // Number of Events at |Vtx|<130cm
  nevent0 = trig_entry[2]; //BBLL1
  nevent1 = trig_entry[3]; //ZDC
  nevent2 = trig_entry[4]; //BBLL1(noVtxCut)

  //cout << "Trigger rate : " << trig_rate0 << "+-" << trig_rate_err0 << endl;
  //cout << "Trigger rate : " << trig_rate1 << "+-" << trig_rate_err1 << endl;
  //cout << "Trigger rate : " << trig_rate2 << "+-" << trig_rate_err2 << endl;

}


void BbcMonDraw::BeamMonitoring(float &sigma_zdc, float &sigma_zdc_err,
                                float &effic_bbc, float &effic_bbc_err,
                                float &beamInZdc, float &beamInZdc_err,
                                float &beamInBbc, float &beamInBbc_err)
{

  TH1 *h[10] = {Zvtx_bbll1_zdc, Zvtx_bbll1,
                Zvtx_bbll1_zdc, Zvtx_zdc,
                Zvtx_bbll1, Zvtx_zdc,
                Zvtx_bbll1, Zvtx_bbll1_novtx,
                Zvtx_bbll1_zdc, Zvtx_bbll1
               };

  //double trig_entry[10]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  double trig_entry[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  // 0:bbll1&&zdcns,  1:bbll1 2:bbll1&&zdc(|Vtx|<30cm),  3:zdc(30cm) 4:bbll1 5:zdc 6:bbll1 7:bbll1_novtx(130cm),
  double ZVERTEX_CUT[10] = {30., 30., 30., 30., 400., 400., 130., 130., 400., 400.};
  float sigma_zdc_no = 0.0;

  // min limit for zvertex
  for (int ihist = 0; ihist < 10; ihist++)
    {
      int nbin = h[ihist]->GetNbinsX();

      for (int ibin = 0; ibin < nbin; ibin++)
        {

          if ( fabs( h[ihist]->GetBinCenter(ibin + 1) ) < ZVERTEX_CUT[ihist] )
            {
              trig_entry[ihist] += h[ihist]->GetBinContent(ibin + 1);
            }

        }
    }

  //BBLL1 / ZDC (|Vtx|<30cm)
  if (trig_entry[0] > 0 && trig_entry[1] > 0)
    {
      sigma_zdc = trig_entry[0] / trig_entry[1];
      sigma_zdc_err = sigma_zdc * ( ( (1.0 / trig_entry[0]) + (1.0 / trig_entry[1]) ) );
    }

  if (trig_entry[2] > 0 && trig_entry[3] > 0)
    {
      effic_bbc = trig_entry[2] / trig_entry[3];
      effic_bbc_err = effic_bbc * ( ( (1.0 / trig_entry[2]) + (1.0 / trig_entry[3]) ) );
    }

  if (trig_entry[4] > 0 && trig_entry[5] > 0)
    {
      beamInZdc = (trig_entry[4] / trig_entry[5]) * 0.2;
      beamInZdc_err = beamInZdc * ( ( (1.0 / trig_entry[4]) + (1.0 / trig_entry[5]) ) );
    }

  if (trig_entry[6] > 0 && trig_entry[7] > 0)
    {
      beamInBbc = trig_entry[6] / trig_entry[7];
      beamInBbc_err = beamInBbc * ( ( (1.0 / trig_entry[6]) + (1.0 / trig_entry[7]) ) );
    }

  if (trig_entry[8] > 0 && trig_entry[9] > 0)
    {
      sigma_zdc_no = trig_entry[8] / trig_entry[9];
    }

  cout << "sigma_zdc (|Vtx|<30cm)  : " << sigma_zdc << endl;
  cout << "sigma_zdc (no Vtx Cut)  : " << sigma_zdc_no << endl;

}


