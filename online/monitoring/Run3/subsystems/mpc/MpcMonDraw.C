//#include <MpcMonConsts.h>
#include <MpcMonDraw.h>
#include <MpcMap.h>
#include <OnlMonClient.h>
#include <OnlMonDB.h>

#include <phool.h>

#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TAttText.h>
#include <TLegend.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;
//using namespace mpcmon;
MpcMonDraw::MpcMonDraw(const char *name): 
  OnlMonDraw(name),
  fracVoltage(0),
  slow(0),
  shigh(0),
  nlow(0),
  nhigh(0)
{
  //species= "pp";
  species= "AA";

  // Below is for auau 200 run11
  //sprintf(BBCTRIG0_name,"BBCLL1(>1 tubes)");
  //sprintf(BBCTRIG1_name,"BBCLL1(>1 tubes) CopyA");
  //sprintf(BBCTRIG2_name,"BBCLL1(>1 tubes) novertex");
  // Below is for auau 27 run11
  sprintf(BBCTRIG0_name,"BBCLL1(>1 tubes) narrowvtx");
  sprintf(BBCTRIG1_name,"BBCLL1(>1 tubes) novertex");
  sprintf(BBCTRIG2_name,"BBCLL1(>1 tubes)");
  //sprintf(BBCTRIG2_name,"BBCLL1(narrow)");
  //sprintf(MPC_TRIG0_name,"MPC_A");
  //sprintf(MPC_TRIG1_name,"MPC_B");
  //sprintf(MPC_TRIG2_name,"MPCS_C");
  sprintf(MPC_TRIG0_name,"BBCLL1(>1 tubes) narrowvtx CopyA");
  sprintf(MPC_TRIG1_name,"BBCLL1(>1 tubes) narrowvtx Copy B");
  sprintf(MPC_TRIG2_name,"BBCLL1(>1 tubes) narrowvtx");
  //sprintf(MPC_TRIG3_name,"(MPCS_C & MPCS_C)||(MPCN_C & MPCN_C)");
  sprintf(LEDTRIG_name,"PPG(Laser)");

  for (int icanvas = 0; icanvas < NCANVAS; icanvas++)
    {
      TC[icanvas] = NULL;
      transparent[icanvas] = NULL;
      for (int ipad = 0;ipad < MAXPADS;ipad++)
        {
          Pad[icanvas][ipad] = NULL;
          Leg[icanvas][ipad] = NULL;
        }
    }

  // Initialize nref, sref.  It runs from 1 to 18 (not 0 to 17).
  for (int ixpos=0; ixpos<18; ixpos++)
    {
      for (int iypos=0; iypos<18; iypos++)
        {
          GridToChan[0][ixpos][iypos] = -9999;
          GridToChan[1][ixpos][iypos] = -9999;

          // since sref, nref count 1 to 19 (stupid), 
          // we need to initialize the higher values
          sref[ixpos+1][iypos+1] = -9999.;
          nref[ixpos+1][iypos+1] = -9999.;
        }
    }

  mpcmap = MpcMap::instance();
  
  for(int i = 0; i < 576; i++)
  {
    int temp_gridx = mpcmap->getGridX(i);
    int temp_gridy = mpcmap->getGridY(i);
    if(i < 288)
    {
      if(temp_gridx > -1 && temp_gridy > -1)
      {
        GridToChan[0][temp_gridx][temp_gridy] = i;
      }
    }
    else
    {
      if(temp_gridx > -1 && temp_gridy > -1)
      {
        GridToChan[1][temp_gridx][temp_gridy] = i;
      }
    }

  }

  int ns, xbin, ybin;
  float val;
  //const char *dbase_directory = getenv("MPC_DATABASE");
  const char *dbase_directory = getenv("ONLMON_CALIB");
  TString str = dbase_directory;

  str += "/mpcmon.cfg";
  ParseConfig( str );

  str = dbase_directory;
  str += "/led.ref";
  ifstream led_reference(str);
  //cout << "reading led reference from " << str << endl;
  while(led_reference >> ns >> xbin >> ybin >> val)
    {
      if ( xbin<0 || xbin>17 || ybin<0 || ybin>17 )
        {
          continue;
        }

      //val = 5.0*val/3.0; //to account for the M=30 to M=50 switch
      if (ns>=0&&ns<288)
        {
	  sref[xbin+1][ybin+1] = val;
        }
      else if (ns>=288&&ns<576)
        {
	  nref[xbin+1][ybin+1] = val;
        }
    }

  // Create Style
  TStyle* oldStyle = gStyle;

  mpcStyle = new TStyle("mpcStyle", "MPC Online Monitor Style");
  mpcStyle->SetOptStat(0);
  mpcStyle->SetTitleH(0.075);
  mpcStyle->SetTitleW(0.98);
  mpcStyle->SetPalette(1);
  mpcStyle->SetFrameBorderMode(0);
  mpcStyle->SetCanvasColor(0);
  mpcStyle->SetPadBorderMode(0);
  mpcStyle->SetCanvasBorderMode(0);

  oldStyle->cd(); //mpcStyle made, but current Style is oldStyle
  lines = 0;
  return ;
}//end MpcMonDraw constructor

MpcMonDraw::~MpcMonDraw()
{
  delete lines;
  delete mpcmap;
  return;
}

void
MpcMonDraw::ParseConfig(const char *cfgname)
{
  // Read in the calib/mpcmon.cfg file to get the parameters
  std::cout << "Reading MPC Online Mon Config File mpcmon.cfg..." << endl;

  ifstream infile(cfgname);
  std::string FullLine;      // a complete line in the config file
  std::string label;         // the label

  // get one line first 
  getline(infile, FullLine);
  while ( !infile.eof() )
  {
    
    // skip lines that begin with #, or "//"
    if ( FullLine[0]=='#' || FullLine.substr(0, 2) == "//" )
    {
      getline(infile,FullLine);
      continue;
    }
    
    // make FullLine an istringstream
    istringstream line( FullLine.c_str() );
    
    // get label
    line >> label;
    
    // to lower case
    std::transform(label.begin(), label.end(), label.begin(), (int(*)(int)) std::tolower);
    
    // based on label, fill correct item
    if ( label == "species" )
      {
        line >> species;
        cout << "species\t" << species << endl;
      }
    else if ( label == "lednumcut" )
      {
        line >> southcut >> northcut;
        cout << "lednumcut\t" << southcut << "\t" << northcut << endl;
      }
    else
      {
        // label was not understood
        cout << "************************************************************" << endl;
        cout << "MpcMonDraw::ParseConfig(), ERROR this option is not supported: " << FullLine << endl;
        cout << "************************************************************" << endl;
        cout << FullLine << endl;
        break;
      }
    
    // get next line in file
    getline( infile, FullLine );
  }
  

}

int
MpcMonDraw::Init()
{
  lines = new TH1F("lines", "MPC Hit Time (North=Blue : South=Gray)", 1200,0,12);
  lines->SetLineColor(2);
  lines->SetBinContent(lines->FindBin(6.9),  1e8);
  lines->SetBinContent(lines->FindBin(10.1), 1e8);

  // Line for summed spectra endpoint
  double lowadc = 1000.;
  double highadc = 2000.;
  double lowy=0.0;
  double highy=0.002;

  if ( species == "AA" )
    {
      //spectra_lines->Fill(130000.,100000.);// Au+Au 200 Start
      //spectra_lines->Fill(40000.,100000.);// Au+Au 62 GeV
      //spectra_lines->Fill(20000.,100000.);// Au+Au 39 GeV
      //lowadc = 40000.;	// ???
      //highadc = 100000.;
      //lowadc = 3000.;	// 19.56 GeV
      //highadc = 8000.;
      //lowadc = 20000.;	// Run11 200 GeV
      //highadc = 50000.;
      lowadc = 4000.;	// Run11 27 GeV
      highadc = 8000.;
    }
  else if ( species == "dA" )
    {
      //spectra_lines->Fill(20000.,100000.);
      //spectra_lines->Fill(35000.,100000.);
      //lowadc = 20000.;
      //highadc = 100000.;
      lowadc = 1500.;
      highadc = 3000.;
    }
  else if ( species == "pp" )
    {
      // Need to fill this in when we get p+p data
      //spectra_lines->Fill(6000.,100000.);
      //spectra_lines->Fill(10000.,100000.);
      //spectra_lines->Fill(3000.,100000.); // run09 500
      //spectra_lines->Fill(6000.,100000.); // run09 500
      //lowadc = 1000.;	// Run11_500pp
      //highadc = 2000.;
      //lowadc = 600.;	// Run12_200pp
      //highadc = 3000.;
      lowadc = 8000.;	// Run12_200UU
      highadc = 18000.;
    }
  line1.SetX1(lowadc);
  line1.SetY1(lowy);
  line1.SetX2(lowadc);
  line1.SetY2(highy);
  line2.SetX1(highadc);
  line2.SetY1(lowy);
  line2.SetX2(highadc);
  line2.SetY2(highy);

  line1.SetLineWidth(3);
  line1.SetLineColor(6);
  
  line2.SetLineWidth(3);
  line2.SetLineColor(6);

  return 0;
}

int
MpcMonDraw::MakeCanvas(const char *name)
{
  // the check if a TCanvas has to be created is done before this
  // method is called - no need to check for the existance of TCanvas's
  // or TPads here (TPads belonging to a TCanvas get deleted when the TCanvas is deleted)
  TStyle *oldStyle = gStyle;
  mpcStyle->cd();

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (!strcmp(name, "MpcMon1"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[0] = new TCanvas(name, "MpcMon1 Spectra By Channel", -1, 0, xsize / 2 , ysize);
      gSystem->ProcessEvents(); // otherwise root leaks memory into the X-Server

      Pad[0][0] = new TPad("mpcpad0_0", "bbc trig channel spectra", 0.03, 0.55, 0.50, 0.95, 0);
      Pad[0][0]->Draw();
      Pad[0][1] = new TPad("mpcpad0_1", "mpc trig channel spectra", 0.50, 0.55, 0.97, 0.95, 0);
      Pad[0][1]->Draw();

      Pad[0][2] = new TPad("mpcpad0_2", "South MPC Energy Sum Spectra", 0.01, 0.20, 0.50, 0.55, 0);
      Pad[0][2]->Draw();
      Pad[0][3] = new TPad("mpcpad0_3", "North MPC Energy Sum Spectra", 0.50, 0.20, 0.99, 0.55, 0);
      Pad[0][3]->Draw();

      Pad[0][4] = new TPad("mpcpad0_4", "MPC Reduced Bits",             0.03, 0.03, 0.50, 0.20, 0);
      Pad[0][4]->Draw();
      
      Pad[0][5] = new TPad("mpcpad0_5", "MPC Status", 0.50, 0.03, 0.97, 0.20, 0);
      Pad[0][5]->Draw();

      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
     }
  else if (!strcmp(name, "MpcMon2"))
    {
      TC[1] = new TCanvas(name, "MpcMon2 Crystal Display and Spectra", -(xsize / 2), 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      if (!strcmp(species.c_str(), "pp"))
	{
	  Pad[1][0] = new TPad("mpcpad1_0", "scrystal display",   0.02, 0.21, 0.48, 0.55, 0);
	  Pad[1][0]->Draw();
	  Pad[1][1] = new TPad("mpcpad1_1", "scrystal display, ", 0.52, 0.21, 0.98, 0.55, 0);
	  Pad[1][1]->Draw();
	  Pad[1][2] = new TPad("mpcpad1_2", "smpc spectra",       0.02, 0.56, 0.48, 0.90, 0);
	  Pad[1][2]->Draw();
	  Pad[1][3] = new TPad("mpcpad1_3", "smpc spectra",       0.52, 0.56, 0.98, 0.90, 0);
	  Pad[1][3]->Draw();
	  Pad[1][4] = new TPad("mpcpad1_4", "smpc spectra",       0.02, 0.02, 0.98, 0.19, 0);
	  Pad[1][4]->Draw();
	}
      else if (!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
	{
	  Pad[1][0] = new TPad("mpcpad1_0", "scrystal display",   0.02, 0.21, 0.48, 0.55, 0);
	  Pad[1][0]->Draw();
	  Pad[1][1] = new TPad("mpcpad1_1", "scrystal display, ", 0.52, 0.21, 0.98, 0.55, 0);
	  Pad[1][1]->Draw();
	  Pad[1][2] = new TPad("mpcpad1_2", "ncrystal display",       0.02, 0.56, 0.48, 0.90, 0);
	  Pad[1][2]->Draw();
	  Pad[1][3] = new TPad("mpcpad1_3", "ncrystal display",       0.52, 0.56, 0.98, 0.90, 0);
	  Pad[1][3]->Draw();
	}
      // this one is used to plot the run number on the canvas
      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
    }
  else if (!strcmp(name, "MpcMon3"))
    {
      TC[2] = new TCanvas(name, "MpcMon Timing Monitor", -(xsize / 2), 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      Pad[2][0] = new TPad("mpcpad2_0", "", 0.1, 0.5, 0.9, 0.9, 0);
      Pad[2][0]->Draw();
      Pad[2][1] = new TPad("mpcpad2_1", "", 0.1, 0.05, 0.9, 0.45, 0);
      Pad[2][1]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
      transparent[2]->SetFillStyle(4000);
      transparent[2]->Draw();
    }
  else if (!strcmp(name, "MpcMon4"))
    {
      if (!strcmp(species.c_str(), "pp"))
	{
	  TC[3] = new TCanvas(name, "MpcMon4 Crystal Display and Spectra", -1, 0, xsize / 2, ysize);
	  gSystem->ProcessEvents();
	  Pad[3][0] = new TPad("mpcpad3_0", "ncrystal display",   0.02, 0.21, 0.48, 0.55, 0);
	  Pad[3][0]->Draw();
	  Pad[3][1] = new TPad("mpcpad3_1", "ncrystal display, ", 0.52, 0.21, 0.98, 0.55, 0);
	  Pad[3][1]->Draw();
	  Pad[3][2] = new TPad("mpcpad3_2", "nmpc spectra",       0.02, 0.56, 0.48, 0.90, 0);
	  Pad[3][2]->Draw();
	  Pad[3][3] = new TPad("mpcpad3_3", "nmpc spectra",       0.52, 0.56, 0.98, 0.90, 0);
	  Pad[3][3]->Draw();
	  Pad[3][4] = new TPad("mpcpad3_4", "nmpc spectra",       0.02, 0.02, 0.98, 0.19, 0);
	  Pad[3][4]->Draw();
	}
      else if (!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
	{
	  TC[3] = new TCanvas(name, "MpcMon4 Crystal Occupancy", -1, 0, xsize / 2, ysize / 2);
	  gSystem->ProcessEvents();
	  Pad[3][0] = new TPad("mpcpad3_0", "Occupancy", 0.1, 0.1, 0.9, 0.9, 0);
	  Pad[3][0]->Draw();
	}
      // this one is used to plot the run number on the canvas
      transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
      transparent[3]->SetFillStyle(4000);
      transparent[3]->Draw();
    }

  else if (!strcmp(name, "MpcMon5"))
    {
//      TC[4] = new TCanvas(name, "MpcMon5 Vertex and Ratio", -(xsize / 2), 0, xsize , ysize);
      TC[4] = new TCanvas(name, "MpcMon5 Vertex and Ratio", 0, 0, xsize/2 , ysize);
      gSystem->ProcessEvents();
/*
      Pad[4][0] = new TPad("mpcpad4_0", "mpc vs bbc vertex",   0.1, 0.50, 0.48, 0.69, 0);
      Pad[4][0]->Draw();
      Pad[4][1] = new TPad("mpcpad4_1", "mpc vs bbc north t0, ", 0.1, 0.71, 0.48, 0.9, 0);
      Pad[4][1]->Draw();
      Pad[4][2] = new TPad("mpcpad4_2", "mpc vs bbc south t0, ", 0.52, 0.71, 0.9, 0.9, 0);
      Pad[4][2]->Draw();
      Pad[4][3] = new TPad("mpcpad4_3", "mpc vs bbc event t0, ", 0.52, 0.50, 0.9, 0.69, 0);
      Pad[4][3]->Draw();
*/
      TString tname;
      const int ntriggers = 5;
      for (int irow=0; irow<ntriggers; irow++)
        {
          tname = "mpcpad4_"; tname += irow;
          float ymax = 0.05+(ntriggers-irow)*0.9/ntriggers + 0.01;
          float ymin = 0.05+(ntriggers-irow-1)*0.9/ntriggers - 0.01;
          Pad[4][irow] = new TPad(tname, "mpc trig0 crossing", 0.05, ymin, 0.95, ymax, 0);
          Pad[4][irow]->Draw();
        }
/*
      Pad[4][1] = new TPad("mpcpad4_1", "mpc trig1 crossing", 0.05, 0.17, 0.95, 0.05+3*0.9/4, 0);
      Pad[4][1]->Draw();
      Pad[4][2] = new TPad("mpcpad4_2", "mpc trig2 crossing", 0.05, 0.01, 0.95, 0.05+2*0.9/4, 0);
      Pad[4][2]->Draw();
      Pad[4][3] = new TPad("mpcpad4_3", "mpc trig3 crossing", 0.05, 0.01, 0.95, 0.05+0.9/4, 0);
      Pad[4][3]->Draw();
      Pad[4][4] = new TPad("mpcpad4_4", "mpc trig4 crossing", 0.05, 0.01, 0.95, 0.05, 0);
      Pad[4][4]->Draw();
*/
      // this one is used to plot the run number on the canvas
      transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
      transparent[4]->SetFillStyle(4000);
      transparent[4]->Draw();
    }

  oldStyle->cd();
  return 0;
}//end MakeCanvas function

int MpcMonDraw::Draw(const char *what)
{
  //cout << "what = " << what << endl;
  TStyle *oldStyle = gStyle;
  mpcStyle->cd();
  
  isServerDead=false;
  isVoltageTooLow=false;
  isVoltageTooHigh=false;

  int iret = 0;
  int idraw = 0;
  if (!strcmp(what, "ALL") || !strcmp(what, "POMS1") || !strcmp(what, "FIRST"))
    {
      iret += DrawFirst(what);
      DrawStatusMessage(Pad[0][5]);
      idraw++;
    }

  
  if (!strcmp(what, "ALL") || !strcmp(what, "POMS2") || !strcmp(what, "SECOND"))
    {
      iret += DrawSecond(what);
      idraw++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "POMS1") || !strcmp(what, "THIRD"))
    {
      iret += DrawThird(what);
      idraw++;
    }
  if (!strcmp(what, "ALL") || (!strcmp(what, "POMS2") && !strcmp(species.c_str(), "pp")) || !strcmp(what, "FOURTH"))
    {
      iret += DrawFourth(what);
      idraw++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "POMS5") || !strcmp(what, "FIFTH"))
    {
      iret += DrawFifth(what);
      idraw++;
    }
  
  if (!idraw)
    {
      cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
      iret = -1;
    }

  oldStyle->cd();

  return iret;
}  //end Draw function

int MpcMonDraw::DrawStatusMessage(TPad *msg)
{
  if( !msg ) return 1;

  OnlMonClient *cl = OnlMonClient::instance();
  TH1F *mpc_nevents = (TH1F*) cl->getHisto("mpc_nevents");
  float nevents = 0;

  TH1 *mpc_triginfo = cl->getHisto("mpc_triginfo");
  float nled = 0.;

  TH1 *mpc_bbctrig_ehist_n = cl->getHisto("mpc_bbctrig_ehist_n");
  TH1 *mpc_bbctrig_ehist_s = cl->getHisto("mpc_bbctrig_ehist_s");

  TH1 *smpc_timedist = cl->getHisto("mpcs_timedist");
  TH1 *nmpc_timedist = cl->getHisto("mpcn_timedist");
  TH1 *mpc_ch_timedist = cl->getHisto("mpc_ch_timedist");

  if( !smpc_timedist || !nmpc_timedist || !mpc_ch_timedist ) isServerDead=true;

  if ( !mpc_triginfo ) isServerDead=true;
  else  nled = mpc_triginfo->GetBinContent(LEDTRIG);

  if( !mpc_nevents )
    {
      cout << "did not find mpc_nevents" << endl;
      isServerDead=true;
    }
  else nevents=mpc_nevents->GetBinContent(1);

  msg->cd();
  msg->SetFillColor(kGreen);

  float height = 0.9;

  TText StatusMsg;
  StatusMsg.SetTextFont(62);
  StatusMsg.SetTextSize(0.1);

  float margin=0.03;

  char text[200];
  sprintf(text,"#events processed = %i",(int)nevents);
  StatusMsg.DrawTextNDC(margin,height,text);
  height -= 0.1;

  if( isServerDead )
    {
      msg->SetFillColor(kRed);
      StatusMsg.DrawTextNDC(margin,height,"MPC Monitor Server is Dead!");
      TC[0]->Update();
      height -= 0.1;
    }
 
  sprintf(text,"#led events processed = %i",(int)nled);
  StatusMsg.DrawTextNDC(margin,height,text);
  height -= 0.1;

  if( nled<10 )
    {
      //msg->SetFillColor(kRed);
      StatusMsg.DrawTextNDC(margin,height,"Too few led events, please wait a bit");
      height -= 0.1;
      StatusMsg.DrawTextNDC(margin,height,"  before worrying about any errors.");
      height -= 0.1;
      TC[0]->Update();
    }

  //else if( shigh < 200 && slow < 200 && nhigh < 7 && nlow < 12 )

  sprintf(text,"shigh=%i,nhigh=%i,slow=%i,nlow=%i",shigh,nhigh,slow,nlow);
  StatusMsg.DrawTextNDC(margin,height,text);
  height -= 0.1;

  if(mpc_bbctrig_ehist_n!=0 && mpc_bbctrig_ehist_s!=0)
    {
      if( mpc_bbctrig_ehist_n->GetEntries() > 500)
	{
	  fracVoltageTooLow=0.0;
	  fracVoltageTooHigh=1.00;
	  
	  //fracVoltageTooLow=0.03;
	  //fracVoltageTooHigh=0.04;
	  
	  //fracVoltageTooLow=0.20;
	  //fracVoltageTooHigh=0.30;
	  
	  int nbins_s = mpc_bbctrig_ehist_s->GetNbinsX();
	  int nbins_n = mpc_bbctrig_ehist_n->GetNbinsX();
	  
	  float ntot_s = mpc_bbctrig_ehist_s->Integral(10,nbins_s);
	  float ntot_n = mpc_bbctrig_ehist_n->Integral(10,nbins_n);
	  
	  float nhigh_s = mpc_bbctrig_ehist_s->Integral(45,nbins_s);
	  float nhigh_n = mpc_bbctrig_ehist_n->Integral(30,nbins_n);
	  
	  fracVoltage = 0;
	  if( ntot_s+ntot_n>0 ) fracVoltage=(nhigh_s+nhigh_n)/(ntot_s+ntot_n);
	  
	  //cout << "ntot_s: " << ntot_s << endl;
	  //cout << "ntot_n: " << ntot_n << endl;
	  //cout << "nhigh_s: " << nhigh_s << endl;
	  //cout << "nhigh_n: " << nhigh_n << endl;
	  //cout << "frac: " << fracVoltage << endl;
	  
	  //cout << "south: "
	  //	   << mpc_bbctrig_ehist_s->GetBinCenter(10) << "\t"
	  //	   << mpc_bbctrig_ehist_s->GetBinCenter(45) << "\t"
	  //	   << mpc_bbctrig_ehist_s->GetBinCenter(nbins_s) << endl;
	  
	  //cout << "north: "
	  //	   << mpc_bbctrig_ehist_n->GetBinCenter(10) << "\t"
	  //	   << mpc_bbctrig_ehist_n->GetBinCenter(50) << "\t"
	  //	   << mpc_bbctrig_ehist_n->GetBinCenter(nbins_n) << endl;
	  
	  //if( mpc_bbctrig_ehist_n->GetEntries()>50000)
	  //{
	  if( fracVoltage<fracVoltageTooLow ) isVoltageTooLow=true;
	  if( fracVoltage>fracVoltageTooHigh ) isVoltageTooHigh=true;
	  //}
	}
    }

  if( isVoltageTooLow && nevents>10000)
    {
      msg->SetFillColor(kRed);
      StatusMsg.DrawTextNDC(margin,height,"ATTENTION: HV might be too low.");
      height -= 0.1;
      StatusMsg.DrawTextNDC(margin,height,"   Check if HV is on. If so, please call expert.");
      height -= 0.1;
      TC[0]->Update();
    }

  if( isVoltageTooHigh && nevents>10000)
    {
      msg->SetFillColor(kRed);
      StatusMsg.DrawTextNDC(margin,height,"ATTENTION: HV might be too high.");
      height -= 0.1;
      StatusMsg.DrawTextNDC(margin,height,"    Please call expert.");
      height -= 0.1;
      TC[0]->Update();
    }


  sprintf(text,"fraction within HV range: %.3f",fracVoltage);
  StatusMsg.DrawTextNDC(margin,height,text);
  height -= 0.1;


  int time_max_s = 0;
  int time_max_n = 0;

  if( smpc_timedist ) time_max_s = smpc_timedist->GetMaximumBin();
  if( nmpc_timedist ) time_max_n = nmpc_timedist->GetMaximumBin();

  sprintf(text,"south t-peak = %i, north t-peak = %i", time_max_s,time_max_n);
  StatusMsg.DrawTextNDC(margin,height,text);
  height -= 0.1;

  if( time_max_s<8 || time_max_s>10 )
    {
      //msg->SetFillColor(kRed);
      //sprintf(text,"ATTENTION: south t-peak shift: FEED after run");
      //StatusMsg.DrawTextNDC(margin,height,text);
      //height -= 0.1;
    }
  if( time_max_n<8 || time_max_n>10 )
    {
      msg->SetFillColor(kRed);
      sprintf(text,"ATTENTION: north t-peak shift: FEED after run");
      StatusMsg.DrawTextNDC(margin,height,text);
      height -= 0.1;
    }
      
  TC[0]->Update();
  return 0;
}

int MpcMonDraw::DrawFirst(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *mpcmon_hist2d = cl->getHisto("mpc_bbctrig_2d");
  TH1 *mpcmon_mpctrig_2d = cl->getHisto("mpc_mpctrig_2d");
 
  //TH1 *mpc_scaledowns = cl->getHisto("mpc_scaledowns");
  //TH1 *mpc_triginfo = cl->getHisto("mpc_triginfo");

  //TH1 *mpc_bbctrig_ehist = cl->getHisto("mpc_bbctrig_ehist");
  TH1 *mpc_ehist[3] = {0};
  mpc_ehist[0] = cl->getHisto("mpc_ehist0");
  mpc_ehist[1] = cl->getHisto("mpc_ehist1");
  mpc_ehist[2] = cl->getHisto("mpc_ehist2");

  TH1 *mpc_bbctrig_ehist_n = cl->getHisto("mpc_bbctrig_ehist_n");
  TH1 *mpc_ehist_n[3] = {0};
  mpc_ehist_n[0] = cl->getHisto("mpc_ehist0_n");
  mpc_ehist_n[1] = cl->getHisto("mpc_ehist1_n");
  mpc_ehist_n[2] = cl->getHisto("mpc_ehist2_n");

  TH1 *mpc_bbctrig_ehist_s = cl->getHisto("mpc_bbctrig_ehist_s");
  TH1 *mpc_ehist_s[3] = {0};
  mpc_ehist_s[0] = cl->getHisto("mpc_ehist0_s");
  mpc_ehist_s[1] = cl->getHisto("mpc_ehist1_s");
  mpc_ehist_s[2] = cl->getHisto("mpc_ehist2_s");

  TH1 *mpc_rbits = cl->getHisto("mpc_rbits1");
  TH1 *mpc_rbits_consistency = cl->getHisto("mpc_rbits2");

  if (! gROOT->FindObject("MpcMon1"))
    {
      MakeCanvas("MpcMon1");
    }
  TC[0]->Clear("D");
  Pad[0][0]->cd();

  if (mpcmon_hist2d )
    {
      if( mpcmon_hist2d->GetEntries()==0 )
	{
	  printf("mpcmon_hist2d has 0 entries");
	}
      else
	{
	  mpcmon_hist2d->SetStats(kFALSE);
	  mpcmon_hist2d->DrawCopy("colz");
	  if( mpcmon_hist2d->GetEntries() > 0 )
	    Pad[0][0]->SetLogz(1);
	}
    }
  else
    {
      isServerDead=true;
      //DrawDeadServer(Pad[0][5]);
      //TC[0]->Update();
      return -1;
    }
  
  // here we know the server is alive and those histogram pointers
  // are not null pointer


  mpc_rbits->SetStats(0);
  mpc_rbits_consistency->SetStats(0);
 
  Pad[0][1]->cd();

  if (mpcmon_mpctrig_2d )
    {
      if( mpcmon_mpctrig_2d->GetEntries()==0 )
	{
          printf("mpcmon_mpctrig_2d has 0 entries");
	}
      else
	{
	  mpcmon_mpctrig_2d->SetStats(kFALSE);
	  mpcmon_mpctrig_2d->DrawCopy("colz");
	  if( mpcmon_mpctrig_2d->GetEntries() > 0 )
	    Pad[0][1]->SetLogz(1);
	}
    }

  Pad[0][2]->cd();

  if ( //mpc_ehist[0] && mpc_ehist[1] && mpc_ehist[2] && mpc_bbctrig_ehist &&
       //mpc_ehist_n[0] && mpc_ehist_n[1] && mpc_ehist_n[2] && mpc_bbctrig_ehist_n &&
      mpc_ehist_s[0] && mpc_ehist_s[1] && mpc_ehist_s[2] && mpc_bbctrig_ehist_s )
    {
      mpc_ehist_s[0]->SetStats(kFALSE);
      mpc_ehist_s[1]->SetStats(kFALSE);
      mpc_ehist_s[2]->SetStats(kFALSE);
      mpc_bbctrig_ehist_s->SetStats(kFALSE);

      mpc_bbctrig_ehist_s->SetTitle("South MPC Energy Sum Spectra");
      mpc_ehist_s[0]->SetTitle("South MPC Energy Sum Spectra");
      mpc_ehist_s[1]->SetTitle("South MPC Energy Sum Spectra");
      mpc_ehist_s[2]->SetTitle("South MPC Energy Sum Spectra");
      //mpc_bbctrig_ehist_s->DrawCopy();

      int nbinsx = mpc_bbctrig_ehist_s->GetNbinsX();
      mpc_bbctrig_ehist_s->GetXaxis()->SetRange(2,nbinsx);

      if ( species == "dA")
        {
          mpc_bbctrig_ehist_s->GetXaxis()->SetRange(2,nbinsx/4);
        }
      else if ( species == "pp" )
        {
          //mpc_bbctrig_ehist->GetXaxis()->SetRange(2,nbinsx/8);
          //mpc_bbctrig_ehist_s->GetXaxis()->SetRangeUser(0,3500);
        }

      //double nbbc_tubes = mpc_triginfo->GetBinContent( BBCTRIG0 ) * ( mpc_scaledowns->GetBinContent(BBCTRIG0) + 1 );
      //double nbbc_novtx = mpc_triginfo->GetBinContent( BBCTRIG1 ) * ( mpc_scaledowns->GetBinContent(BBCTRIG1) + 1 );
      //double nbbc_narow = mpc_triginfo->GetBinContent( BBCTRIG2 ) * ( mpc_scaledowns->GetBinContent(BBCTRIG2) + 1 );
      //double nbbc = nbbc_novtx; //leave out narrow for now, only using novts right now anyways
      //mpc_bbctrig_ehist_s->SetMinimum(1.0);
      
//      double nbbc = mpc_bbctrig_ehist_s->GetEntries();
//      double scale_down_bbc1 = (double) mpc_scaledowns->GetBinContent(BBCTRIG1) + 1;
//      double scale_down_mpc0 = (double) mpc_scaledowns->GetBinContent(MPC_TRIG0) + 1;
//      double scale_down_mpc1 = (double) mpc_scaledowns->GetBinContent(MPC_TRIG1) + 1;
//      double scale_down_mpc2 = (double) mpc_scaledowns->GetBinContent(MPC_TRIG2) + 1;

      //cout << "entries = " << mpc_bbctrig_ehist_s->Integral() << endl;
      //cout << "bin1 = " << mpc_bbctrig_ehist_s->GetBinContent(1) << endl;
      
      //mpc_bbctrig_ehist_s->Scale( 1.0/nbbc );
      //cout << "bin1 = " << mpc_bbctrig_ehist_s->GetBinContent(1) << endl;

/*
      mpc_ehist_s[0]->Scale( scale_down_mpc0/nbbc );
      mpc_ehist_s[1]->Scale( scale_down_mpc1/nbbc );
      mpc_ehist_s[2]->Scale( scale_down_mpc2/nbbc );
*/

      mpc_bbctrig_ehist_s->DrawCopy();
      mpc_ehist_s[0]->DrawCopy("same");
      mpc_ehist_s[1]->DrawCopy("same");
      mpc_ehist_s[2]->DrawCopy("same");

      //draw it again just to black histogram is on top
      mpc_bbctrig_ehist_s->DrawCopy("same");

      line1.Draw("same");
      line2.Draw("same");

      // Draw the legend manually (using text)
      TText t1;
      t1.SetTextSize( t1.GetTextSize()*0.8 );
      t1.SetTextAlign(32); //right-centered alginment
      t1.DrawTextNDC(0.88,0.70,"MinBias");
/*
      t1.SetTextColor(2);
      t1.DrawTextNDC(0.88,0.65,"MPC4x4a");
      t1.SetTextColor(3);
      t1.DrawTextNDC(0.88,0.60,"MPC4x4c&&ERT2");
      t1.SetTextColor(4);
      t1.DrawTextNDC(0.88,0.55,"MPC4x4b");
*/

      double nent = 
	mpc_bbctrig_ehist_s->GetEntries() +
	mpc_ehist_s[0]->GetEntries() +
	mpc_ehist_s[1]->GetEntries() +
	mpc_ehist_s[2]->GetEntries();

      //DrawEnergyInstructions(Pad[0][5]);

      if( nent>0 )
	Pad[0][2]->SetLogy(1);

      if(mpc_bbctrig_ehist_s->GetEntries() > 10000)
	{
	  float last = 140000;
	  for(int bin = 2; bin < nbinsx; bin++)
	    if(mpc_bbctrig_ehist_s->GetBinContent(bin) == 0 && mpc_bbctrig_ehist_s->GetBinContent(bin+1) == 0)
	      {
		last = mpc_bbctrig_ehist_s->GetBinCenter(bin);
		break;
	      }

          float last_min_check = 0;
          float last_max_check = mpc_bbctrig_ehist_s->GetBinCenter( mpc_bbctrig_ehist_s->GetNbinsX() );
          if ( species == "AA" )
            {
              //last_min_check = 130000;// Start of AuAu 200
              //last_max_check = 160000;
              //last_min_check = 110000;// AuAu 200 after 10 ch's lost
              //last_max_check = 140000;
              //last_min_check = 40000.;// AuAu 62 GeV
              //last_max_check = 60000.;
              //last_min_check = 3000.;	// AuAu 19.6 GeV
              //last_max_check = 8000.;
              last_min_check = 4000.;	// AuAu 27 GeV
              last_max_check = 8000.;
            }
          else if ( species == "dA" )
            {
              //last_min_check = 18000;
              //last_max_check = 35000;
              last_min_check = 1500; // He3+Au, Run14
              last_max_check = 3000;
            }
          else if ( species == "pp" )
            {
              // Need to update this for p+p
              last_min_check = 2000;
              last_max_check = 5000;
            }

	}
      
    }


  Pad[0][3]->cd();

  if ( mpc_ehist_n[0] && mpc_ehist_n[1] && mpc_ehist_n[2] && mpc_bbctrig_ehist_n )
    {
      mpc_ehist_n[0]->SetStats(kFALSE);
      mpc_ehist_n[1]->SetStats(kFALSE);
      mpc_ehist_n[2]->SetStats(kFALSE);
      mpc_bbctrig_ehist_n->SetStats(kFALSE);
      
      mpc_bbctrig_ehist_n->SetTitle("North MPC Energy Sum Spectra");
      mpc_ehist_n[0]->SetTitle("North MPC Energy Sum Spectra");
      mpc_ehist_n[1]->SetTitle("North MPC Energy Sum Spectra");
      mpc_ehist_n[2]->SetTitle("North MPC Energy Sum Spectra");

      //mpc_bbctrig_ehist_n->DrawCopy();

      int nbinsx = mpc_bbctrig_ehist_n->GetNbinsX();
      mpc_bbctrig_ehist_n->GetXaxis()->SetRange(2,nbinsx);

      if ( species == "dA")
        {
          mpc_bbctrig_ehist_n->GetXaxis()->SetRange(2,nbinsx/4);
        }
      else if ( species == "pp" )
        {
          //mpc_bbctrig_ehist->GetXaxis()->SetRange(2,nbinsx/8);
          //mpc_bbctrig_ehist_n->GetXaxis()->SetRange(2,nbinsx/20);
        }

/*
      double nbbc = mpc_bbctrig_ehist_s->GetEntries();
      //double scale_down_bbc1 = (double) mpc_scaledowns->GetBinContent(BBCTRIG1) + 1;
      double scale_down_mpc0 = (double) mpc_scaledowns->GetBinContent(MPC_TRIG0) + 1;
      double scale_down_mpc1 = (double) mpc_scaledowns->GetBinContent(MPC_TRIG1) + 1;
      double scale_down_mpc2 = (double) mpc_scaledowns->GetBinContent(MPC_TRIG2) + 1;

      mpc_bbctrig_ehist_n->Scale( 1.0/nbbc );
      mpc_ehist_n[0]->Scale( scale_down_mpc0/nbbc );
      mpc_ehist_n[1]->Scale( scale_down_mpc1/nbbc );
      mpc_ehist_n[2]->Scale( scale_down_mpc2/nbbc );
*/

      mpc_bbctrig_ehist_n->DrawCopy();
      mpc_ehist_n[0]->DrawCopy("same");
      mpc_ehist_n[1]->DrawCopy("same");
      mpc_ehist_n[2]->DrawCopy("same");

      mpc_bbctrig_ehist_n->DrawCopy("same");

      line1.Draw("same");
      line2.Draw("same");

      double nent = 
	mpc_bbctrig_ehist_n->GetEntries() +
	mpc_ehist_n[0]->GetEntries() +
	mpc_ehist_n[1]->GetEntries() +
	mpc_ehist_n[2]->GetEntries();

      if( nent>0 )
	Pad[0][3]->SetLogy(1);

      if(mpc_bbctrig_ehist_n->GetEntries() > 5000)
	{
	  float last = 140000;
	  for(int bin = 2; bin < nbinsx; bin++)
	    if(mpc_bbctrig_ehist_n->GetBinContent(bin) == 0 && mpc_bbctrig_ehist_n->GetBinContent(bin+1) == 0)
	      {
		last = mpc_bbctrig_ehist_n->GetBinCenter(bin);
		break;
	      }

          float last_min_check = 0;
          float last_max_check = mpc_bbctrig_ehist_n->GetBinCenter( mpc_bbctrig_ehist_n->GetNbinsX() );
          if ( species == "AA" )
            {
              //last_min_check = 130000;// Start of AuAu 200
              //last_max_check = 160000;
              //last_min_check = 110000;// AuAu 200 after 10 ch's lost
              //last_max_check = 140000;
              //last_min_check = 40000.;// AuAu 62 GeV
              //last_max_check = 60000.;
              //last_min_check = 20000.;// Run11 19.6 GeV
              //last_max_check = 40000.;
              //last_min_check = 25000.;// Run11 200 GeV
              //last_max_check = 50000.;
              last_min_check = 4000.;// Run11 27 GeV
              last_max_check = 8000.;
            }
          else if ( species == "dA" )
            {
              //last_min_check = 18000;
              //last_max_check = 35000;
              last_min_check = 1500;
              last_max_check = 3000;
            }
          else if ( species == "pp" )
            {
              // Need to update this for p+p
              last_min_check = 2000;
              last_max_check = 5000;
            }

	  //if ( last < last_min_check )      DrawVoltageTooLow(transparent[0]);
	  //else if ( last > last_max_check ) DrawVoltageTooHigh(transparent[0]);

	}

      Pad[0][3]->cd();
/*
      TText t; // = new TText();
      char text[200];
    
      double text_height = 0.87;
      double text_right = 0.89;
      t.SetTextSize( t.GetTextSize()*0.8 );
      t.SetTextAlign(32); //right-centered alginment
      sprintf(text,"triggers with >0 scaledown");
      t.DrawTextNDC(text_right,text_height,text);

      unsigned int bbctrig0_scaledown = (unsigned int)mpc_scaledowns->GetBinContent(BBCTRIG0);
      if( bbctrig0_scaledown > 0 )
	{
	  text_height -= 0.05;
	  sprintf(text,"%s = %u",BBCTRIG0_name,bbctrig0_scaledown);
	  t.DrawTextNDC(text_right,text_height,text);
	}

      unsigned int bbctrig1_scaledown = (unsigned int)mpc_scaledowns->GetBinContent(BBCTRIG1);
      if( bbctrig1_scaledown > 0 )
	{
	  text_height -= 0.05;
	  sprintf(text,"%s = %u",BBCTRIG1_name,bbctrig1_scaledown);
	  t.DrawTextNDC(text_right,text_height,text);
	}

      //text_height -= 0.05;

      //sprintf(text,"%s = %u",BBCTRIG2_name,(unsigned int)mpc_scaledowns->GetBinContent(BBCTRIG2));
      //t.DrawTextNDC(0.5,0.70,text);

      //sprintf(text,"%s = %u",LEDTRIG_name,(unsigned int)mpc_scaledowns->GetBinContent(LEDTRIG));
      //t.DrawTextNDC(0.5,0.60,text);

      unsigned int  mpc4x4a_scaledown = (unsigned int)mpc_scaledowns->GetBinContent(MPC_TRIG0);
      if( mpc4x4a_scaledown>0 )
	{
	  text_height -= 0.05;
	  sprintf(text,"%s = %u",MPC_TRIG0_name,mpc4x4a_scaledown);
	  t.SetTextColor(2);
	  t.DrawTextNDC(text_right,text_height,text);
	}

      unsigned int mpc4x4c_scaledown = (unsigned int)mpc_scaledowns->GetBinContent(MPC_TRIG1);
      if( mpc4x4c_scaledown>0 )
	{
	  text_height -= 0.05;
	  sprintf(text,"%s = %u",MPC_TRIG1_name,mpc4x4c_scaledown);
	  t.SetTextColor(3);
	  t.DrawTextNDC(text_right,text_height,text);
	}

      unsigned int mpc4x4b_scaledown = (unsigned int)mpc_scaledowns->GetBinContent(MPC_TRIG2);
      if( mpc4x4b_scaledown>0 )
	{
	  text_height -= 0.05;
	  sprintf(text,"%s = %u",MPC_TRIG2_name,mpc4x4b_scaledown);
	  t.SetTextColor(4);
	  t.DrawTextNDC(text_right,text_height,text);
	}
*/

    }

  Pad[0][4]->cd();
  //cout << "mpc_rbits histogram address = " << mpc_rbits << endl;
  if( mpc_rbits )
    {
      //cout << "mpc_rbits entries = " << mpc_rbits->GetEntries() << endl;
      mpc_rbits->GetXaxis()->SetBinLabel(1,"A");
      mpc_rbits->GetXaxis()->SetBinLabel(2,"B");
      mpc_rbits->GetXaxis()->SetBinLabel(3,"SC0");
      mpc_rbits->GetXaxis()->SetBinLabel(4,"SC1");
      mpc_rbits->GetXaxis()->SetBinLabel(5,"SC2");
      mpc_rbits->GetXaxis()->SetBinLabel(6,"SC3");
      mpc_rbits->GetXaxis()->SetBinLabel(7,"SC4");
      mpc_rbits->GetXaxis()->SetBinLabel(8,"SC5");
      mpc_rbits->GetXaxis()->SetBinLabel(9,"NC0");
      mpc_rbits->GetXaxis()->SetBinLabel(10,"NC1");
      mpc_rbits->GetXaxis()->SetBinLabel(11,"NC2");
      mpc_rbits->GetXaxis()->SetBinLabel(12,"NC3");
      mpc_rbits->GetXaxis()->SetBinLabel(13,"NC4");
      mpc_rbits->GetXaxis()->SetBinLabel(14,"NC5");

      mpc_rbits->GetXaxis()->SetLabelSize(0.08);

      mpc_rbits->DrawCopy();
      if( mpc_rbits->GetEntries() > 0 )
	Pad[0][4]->SetLogy(1);

    }

  Pad[0][5]->cd();
  //draw status stuff here

  Pad[0][5]->SetFillColor(kGreen);

  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.038);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
	      << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[0]->Update();
  return 0;
}  //end DrawFirst function

int MpcMonDraw::DrawSecond(const char *what)
{

  slow = 0;
  shigh = 0;
  nlow = 0;
  nhigh = 0;

  for(int i = 0; i < 576; i++)
    {
      highindex[i] = 0;
      lowindex[i]  = 0;
      highval[i] = 0;
      lowval[i] = 0;
    }

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *smpc_crystal[3] = {0};
  smpc_crystal[0] = cl->getHisto("mpcs_crystal0");
  smpc_crystal[1] = cl->getHisto("mpcs_crystal1");
  smpc_crystal[2] = cl->getHisto("mpcs_crystal2");
  
  TH1 *nmpc_crystal[3] = {0};
  nmpc_crystal[0] = cl->getHisto("mpcn_crystal0");
  nmpc_crystal[1] = cl->getHisto("mpcn_crystal1");
  nmpc_crystal[2] = cl->getHisto("mpcn_crystal2");
  
  TH1 *smpc_bbctrig_crystal_energy = cl->getHisto("mpcs_bbctrig_crystal_energy");
  TH1 *smpc_led_crystal_energy = cl->getHisto("mpcs_led_crystal_energy");
  TH1 *nmpc_led_crystal_energy = cl->getHisto("mpcn_led_crystal_energy");
  TH1 *nmpc_bbctrig_crystal_energy = cl->getHisto("mpcn_bbctrig_crystal_energy");
  if ( smpc_led_crystal_energy!=0 && nmpc_led_crystal_energy!=0 )
    {
      smpc_led_crystal_energy->SetTitle("Relative Energy/Crystal: South LED");
      nmpc_led_crystal_energy->SetTitle("Relative Energy/Crystal: North LED");
    }
  
  TH1 *mpc_triginfo = cl->getHisto("mpc_triginfo");
  
  double nled = 0.;
  if ( mpc_triginfo!=0 ) nled = mpc_triginfo->GetBinContent(LEDTRIG);

  if (! gROOT->FindObject("MpcMon2"))
    {
      MakeCanvas("MpcMon2");
    }
  TC[1]->Clear("D");
  Pad[1][0]->cd();

  //for(int i = 0; i < 7; i++)

  if ( smpc_bbctrig_crystal_energy && nmpc_bbctrig_crystal_energy)
    {
      double nbbc = mpc_triginfo->GetBinContent(BBCTRIG0) + mpc_triginfo->GetBinContent(BBCTRIG1) + mpc_triginfo->GetBinContent(BBCTRIG2);

      if ( nbbc > 0 )
        {
          smpc_bbctrig_crystal_energy->Scale(1.0/nbbc);
        }
      smpc_bbctrig_crystal_energy->SetStats(kFALSE);
      smpc_bbctrig_crystal_energy->DrawCopy("colz");
    }
  else
    {
      isServerDead=true;
      //DrawDeadServer(transparent[1]);
      //TC[1]->Update();
      return -1;
    }
  Pad[1][1]->cd();
  if ( smpc_led_crystal_energy )
    {

      if ( nled > 0 )
        {
          smpc_led_crystal_energy->Scale(1.0/nled);
        }
      for(int i = 1; i < smpc_led_crystal_energy->GetNbinsX()+1; i++)
	for(int j = 1; j < smpc_led_crystal_energy->GetNbinsY()+1; j++)
	  {
	    //cout << "sled0 " << i-1 << " " << j-1 << " " << smpc_led_crystal_energy->GetBinContent(i, j) << endl;
	    float content = smpc_led_crystal_energy->GetBinContent(i, j);
	    if( sref[i][j]>0. )
	      {
		smpc_led_crystal_energy->SetBinContent(i, j, content/sref[i][j]);
		if(smpc_led_crystal_energy->GetBinContent(i, j) < 0.8)
		  {
		    if( GridToChan[0][i-1][j-1] != -9999 )
		      {
			lowindex[slow+nlow] = GridToChan[0][i-1][j-1];
			lowval[slow+nlow] = smpc_led_crystal_energy->GetBinContent(i, j);
			slow++;
		      }
		  }
		if(smpc_led_crystal_energy->GetBinContent(i, j) > 1.2)
		  {
		    if( GridToChan[0][i-1][j-1] != -9999 )
		      {
			highindex[shigh+nhigh] = GridToChan[0][i-1][j-1];
			highval[shigh+nhigh] = smpc_led_crystal_energy->GetBinContent(i, j);
			shigh++;
		      }
		  }
	      }
	    else
	      smpc_led_crystal_energy->SetBinContent(i, j, 0);
	          
	  }
      smpc_led_crystal_energy->SetStats(kFALSE);
      smpc_led_crystal_energy->GetZaxis()->SetRangeUser(0.7,1.3);
      smpc_led_crystal_energy->DrawCopy("colz");
    }
  
  if(!strcmp(species.c_str(), "pp"))
    {
      Pad[1][2]->cd();
      if ( smpc_crystal[0] )
	      {
          double nmpc0 = mpc_triginfo->GetBinContent(MPC_TRIG0);
          if ( nmpc0 > 0 )
            {
              smpc_crystal[0]->Scale(1.0/nmpc0);
            }
          smpc_crystal[0]->SetStats(kFALSE);
          //smpc_crystal[0]->SetMaximum(8);
          smpc_crystal[0]->DrawCopy("colz");
        }
      Pad[1][3]->cd();
      if ( smpc_crystal[1] )
        {
          double nmpc = mpc_triginfo->GetBinContent(MPC_TRIG1);
          if ( nmpc > 0. )
            {
              smpc_crystal[1]->Scale(1.0/nmpc);
            }
          smpc_crystal[1]->SetStats(kFALSE);
          smpc_crystal[1]->DrawCopy("colz");
        }
      Pad[1][4]->cd();
      if ( smpc_crystal[2] )
      {
        double nmpc = mpc_triginfo->GetBinContent(MPC_TRIG2);
        if ( nmpc > 0. )
        {
          smpc_crystal[2]->Scale(1.0/nmpc);
        }
        smpc_crystal[2]->SetStats(kFALSE);
        //smpc_crystal[2]->DrawCopy("colz");
      }
    }
  else if(!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
  {
    Pad[1][2]->cd();
    if ( nmpc_bbctrig_crystal_energy )
    {
      double nbbc = mpc_triginfo->GetBinContent(BBCTRIG0) + mpc_triginfo->GetBinContent(BBCTRIG1) + mpc_triginfo->GetBinContent(BBCTRIG2);
      if ( nbbc > 0 )
      {
        nmpc_bbctrig_crystal_energy->Scale(1.0/nbbc);
      }
      nmpc_bbctrig_crystal_energy->SetStats(kFALSE);
      nmpc_bbctrig_crystal_energy->DrawCopy("colz");
    }
    Pad[1][3]->cd();
    if ( nmpc_led_crystal_energy )
    {
      if ( nled > 0 )
      {
        nmpc_led_crystal_energy->Scale(1.0/nled);
      }

      for(int i = 1; i < nmpc_led_crystal_energy->GetNbinsX()+1; i++)
        for(int j = 1; j < nmpc_led_crystal_energy->GetNbinsY()+1; j++)
        {
          //outfile << 1 << " " << i << " " << j << " " << nmpc_led_crystal_energy->GetBinContent(i, j) << endl;
          float content = nmpc_led_crystal_energy->GetBinContent(i, j);
          if( nref[i][j]>0. )
          {
            nmpc_led_crystal_energy->SetBinContent(i, j, content/nref[i][j]);      
            if(nmpc_led_crystal_energy->GetBinContent(i, j) < 0.8)
            {
              if( GridToChan[1][i-1][j-1] != -9999 )
              {
                lowindex[slow+nlow] = GridToChan[1][i-1][j-1];
                lowval[slow+nlow] = nmpc_led_crystal_energy->GetBinContent(i, j);
                nlow++;
              }
            }
            if(nmpc_led_crystal_energy->GetBinContent(i, j) > 1.2)
            {
              if( GridToChan[1][i-1][j-1] != -9999 )
              {
                highindex[shigh+nhigh] = GridToChan[1][i-1][j-1];
                highval[shigh+nhigh] = nmpc_led_crystal_energy->GetBinContent(i, j);
                nhigh++;
              }
            }
          }
          else
          {
            nmpc_led_crystal_energy->SetBinContent(i, j, 0);
          }
        }
      nmpc_led_crystal_energy->SetStats(kFALSE);
      nmpc_led_crystal_energy->GetZaxis()->SetRangeUser(0.7,1.3);
      nmpc_led_crystal_energy->DrawCopy("colz");
    }
  }

  DrawGridInstructions(transparent[1], nled);
  if(nled > 20)
  {
    if( shigh>southcut || nhigh>northcut)
    {
      if( nhigh>northcut ) // THIS IS TEMPORARY FOR Cu+Au TO INOGRE SOUTH WHILE MPC.S IS BROKEN
      {
        isVoltageTooLow=true;
        //DrawVoltageTooHigh(transparent[1]);
      }
    }
    else if(slow>southcut || nlow>northcut)
    {
      if( nlow>northcut ) // THIS IS TEMPORARY FOR Cu+Au TO INOGRE SOUTH WHILE MPC.S IS BROKEN
      {
        isVoltageTooHigh=true;
        //DrawVoltageTooLow(transparent[1]);
      }
    }
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.038);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
    << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 0.98, runstring.c_str());
  TC[1]->Update();
  //outfile.close();
  return 0;
}//end DrawSecond function

int MpcMonDraw::DrawThird(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *smpc_timedist = cl->getHisto("mpcs_timedist");
  TH1 *nmpc_timedist = cl->getHisto("mpcn_timedist");
  TH1 *mpc_ch_timedist = cl->getHisto("mpc_ch_timedist");
  if (! gROOT->FindObject("MpcMon3"))
    {
      MakeCanvas("MpcMon3");
    }
  TC[2]->Clear("D");
  Pad[2][0]->cd();

  if ( smpc_timedist && nmpc_timedist)
    {
      if(nmpc_timedist->GetMaximum() >= smpc_timedist->GetMaximum())
	{
	  nmpc_timedist->SetStats(kFALSE);
	  nmpc_timedist->DrawCopy();
	  smpc_timedist->SetStats(kFALSE);
	  smpc_timedist->DrawCopy("same");
	}
      else
	{
	  smpc_timedist->SetStats(kFALSE);
	  smpc_timedist->DrawCopy();
	  nmpc_timedist->SetStats(kFALSE);
	  nmpc_timedist->DrawCopy("same");
	}
      lines->DrawCopy("same");
      DrawTimingInstructions(transparent[2]);
    }
  else
    {
      isServerDead=true;
      //DrawDeadServer(transparent[2]);
      //TC[2]->Update();
      return -1;
    }

  //TCanvas *c = new TCanvas();
  //  gSystem->ProcessEvents();
  //TF1 *sgaus = new TF1("sgaus", "gaus");
  //sgaus->SetLineColor(0);
  //smpc_timedist->Fit(sgaus, "Q", "Q", -30, 30);
  //TF1 *ngaus = new TF1("ngaus", "gaus");
  //ngaus->SetLineColor(0);
  //nmpc_timedist->Fit(ngaus, "Q", "Q", -30, 30);
  //delete nmpc_timedist;
  //delete smpc_timedist;
  //delete c;
   
   
  //if(smpc_timedist->GetMean() < 1 || nmpc_timedist->GetMean() < 1 || smpc_timedist->GetMean() > 5 || nmpc_timedist->GetMean() > 5)
  //if(sgaus->GetParameter(1) < 1 || ngaus->GetParameter(1) < 1 || sgaus->GetParameter(1) > 5 || ngaus->GetParameter(1) > 5)
  //if(sgaus->GetParameter(1) < -2 || ngaus->GetParameter(1) < -2 || sgaus->GetParameter(1) > 15 || ngaus->GetParameter(1) > 15)
  // {
  // Taken out for now, it's not really useful (mchiu 20-May-2010).
  //It also causes memory leaks in valgrind that I have spent several unsuccessful hours trying to get rid of if its not commented out (sjw 2/2011)
  //DrawHighVoltageOff(transparent[2]);
  // }

  Pad[2][1]->cd();
  if ( mpc_ch_timedist )
    {
      mpc_ch_timedist->SetStats(kFALSE);
      mpc_ch_timedist->DrawCopy("colz");
      if( mpc_ch_timedist->GetEntries()>0 )
	Pad[2][1]->SetLogz(1);
    }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.038);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
	      << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[2]->cd();
  PrintRun.DrawText(0.5, 0.98, runstring.c_str());
  TC[2]->Update();
  return 0;
}//end DrawThird function

int MpcMonDraw::DrawFourth(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *nmpc_crystal[3] = {0};
  nmpc_crystal[0] = cl->getHisto("mpcn_crystal0");
  nmpc_crystal[1] = cl->getHisto("mpcn_crystal1");
  nmpc_crystal[2] = cl->getHisto("mpcn_crystal2");
  TH1 *nmpc_led_crystal_energy = cl->getHisto("mpcn_led_crystal_energy");
  TH1 *nmpc_bbctrig_crystal_energy = cl->getHisto("mpcn_bbctrig_crystal_energy");

  TH1 *smpc_bbctrig_occupancy = cl->getHisto("mpcs_bbctrig_occupancy");
  TH1 *nmpc_bbctrig_occupancy = cl->getHisto("mpcn_bbctrig_occupancy");
  
  TH1 *mpc_triginfo = cl->getHisto("mpc_triginfo");
  
  double nled = 0;
  if ( mpc_triginfo!=0 ) nled = mpc_triginfo->GetBinContent(LEDTRIG);
      
  if (! gROOT->FindObject("MpcMon4"))
    {
      MakeCanvas("MpcMon4");
    }

  TC[3]->Clear("D");
  Pad[3][0]->cd();
  if(!strcmp(species.c_str(), "AA") || !strcmp(species.c_str(), "dA"))
  {
    if(smpc_bbctrig_occupancy && nmpc_bbctrig_occupancy)
    {
      nmpc_bbctrig_occupancy->SetStats(kFALSE);
      smpc_bbctrig_occupancy->SetStats(kFALSE);
      int smaxbin = smpc_bbctrig_occupancy->GetMaximumBin();
      int nmaxbin = nmpc_bbctrig_occupancy->GetMaximumBin();
      if(nmpc_bbctrig_occupancy->GetBinContent(nmaxbin) > smpc_bbctrig_occupancy->GetBinContent(smaxbin))
      {
        nmpc_bbctrig_occupancy->DrawCopy();
        smpc_bbctrig_occupancy->DrawCopy("same");
      }
      else
      {
        smpc_bbctrig_occupancy->DrawCopy();
        nmpc_bbctrig_occupancy->DrawCopy("same");
      }
      /*
         if(nmpc_bbctrig_occupancy->GetBinContent(1) > smpc_bbctrig_occupancy->GetBinContent(1))
         smpc_bbctrig_occupancy->DrawCopy("same");
         else
         nmpc_bbctrig_occupancy->DrawCopy("same");
         */
    }
    else
    {
      isServerDead=true;
      //DrawDeadServer(transparent[3]);
      //TC[3]->Update();
      return -1;
    }
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.038);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    std::string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_4 Run " << cl->RunNumber()
      << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[3]->cd();
    PrintRun.DrawText(0.5, 0.98, runstring.c_str());
    TC[3]->Update();
    return 0;
  }
  if ( nmpc_bbctrig_crystal_energy )
  {
    double nbbc = mpc_triginfo->GetBinContent(BBCTRIG0) + mpc_triginfo->GetBinContent(BBCTRIG1) + mpc_triginfo->GetBinContent(BBCTRIG2);
    if ( nbbc > 0 )
    {
      nmpc_bbctrig_crystal_energy->Scale(1.0/nbbc);
    }
    nmpc_bbctrig_crystal_energy->SetStats(kFALSE);
    nmpc_bbctrig_crystal_energy->DrawCopy("colz");
  }
  else
  {
    isServerDead=true;
    //DrawDeadServer(transparent[1]);
    //TC[3]->Update();
    return -1;
  }

  Pad[3][1]->cd();
  if ( nmpc_led_crystal_energy )
    {
      if ( nled > 0 )
        {
          nmpc_led_crystal_energy->Scale(1.0/nled);
        }
        
      for(int i = 1; i < nmpc_led_crystal_energy->GetNbinsX()+1; i++)
	for(int j = 1; j < nmpc_led_crystal_energy->GetNbinsY()+1; j++)
	  {
            //outfile << 1 << " " << i << " " << j << " " << nmpc_led_crystal_energy->GetBinContent(i, j) << endl;
            float content = nmpc_led_crystal_energy->GetBinContent(i, j);
            if ( nref[i][j]>0. )
              {
                nmpc_led_crystal_energy->SetBinContent(i, j, content/nref[i][j]);      
                if (nmpc_led_crystal_energy->GetBinContent(i, j) < 0.8)
                  {
		    if( GridToChan[1][i-1][j-1] != -9999 )
		      {
			lowindex[slow+nlow] = GridToChan[1][i-1][j-1];
			lowval[slow+nlow] = nmpc_led_crystal_energy->GetBinContent(i, j);
			nlow++;
		      }
                  }
                if (nmpc_led_crystal_energy->GetBinContent(i, j) > 1.2)
                  {
		    if( GridToChan[1][i-1][j-1] != -9999 )
		      {
			highindex[shigh+nhigh] = GridToChan[1][i-1][j-1];
			highval[shigh+nhigh] = nmpc_led_crystal_energy->GetBinContent(i, j);
			nhigh++;
		      }
                  }
              }
            else
              {
                nmpc_led_crystal_energy->SetBinContent(i, j, 0);
              }
	  }
      nmpc_led_crystal_energy->SetStats(kFALSE);
      nmpc_led_crystal_energy->GetZaxis()->SetRangeUser(0.7,1.3);
      nmpc_led_crystal_energy->DrawCopy("colz");
    }
  else
    {
      isServerDead=true;
      //DrawDeadServer(transparent[1]);
      //TC[3]->Update();
      return -1;
    }
  
  Pad[3][2]->cd();
  if ( nmpc_crystal[0] )
    {
      double nmpc0 = mpc_triginfo->GetBinContent(MPC_TRIG0);
      if ( nmpc0 > 0 )
        {
          nmpc_crystal[0]->Scale(1.0/nmpc0);
        }
      nmpc_crystal[0]->SetStats(kFALSE);
      //nmpc_crystal[0]->SetMaximum(8);
      nmpc_crystal[0]->DrawCopy("colz");
    }

  Pad[3][3]->cd();
  if ( nmpc_crystal[1] )
    {
      double nmpc = mpc_triginfo->GetBinContent(MPC_TRIG1);
      if ( nmpc > 0. )
        {
          nmpc_crystal[1]->Scale(1.0/nmpc);
        }
      nmpc_crystal[1]->SetStats(kFALSE);
      nmpc_crystal[1]->DrawCopy("colz");
    }

  Pad[3][4]->cd();
  if ( nmpc_crystal[2] )
    {
      double nmpc = mpc_triginfo->GetBinContent(MPC_TRIG2);
      if ( nmpc > 0. )
	      {
          nmpc_crystal[2]->Scale(1.0/nmpc);
        }
      nmpc_crystal[2]->SetStats(kFALSE);
      //nmpc_crystal[2]->DrawCopy("colz");
    }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.038);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_4 Run " << cl->RunNumber()
	      << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();

  transparent[3]->cd();
  PrintRun.DrawText(0.5, 0.98, runstring.c_str());
  TC[3]->Update();
  return 0;
}//end DrawFourth function

int MpcMonDraw::DrawFifth(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  //TH1 *mpcvsbbcst0 = cl->getHisto("mpcvsbbcst0");
  //TH1 *mpcvsbbcnt0 = cl->getHisto("mpcvsbbcnt0");
  //TH1 *mpcvsbbcvtx = cl->getHisto("mpcvsbbcvtx");
  //TH1 *mpcvsbbct0 = cl->getHisto("mpcvsbbct0");
  TH1 *mpc0_trig0_crossing = cl->getHisto("mpc0_trig0_crossing");
  TH1 *mpc1_trig0_crossing = cl->getHisto("mpc1_trig0_crossing");
  TH1 *mpc0_trig1_crossing = cl->getHisto("mpc0_trig1_crossing");
  TH1 *mpc1_trig1_crossing = cl->getHisto("mpc1_trig1_crossing");
  TH1 *mpc0_trig2_crossing = cl->getHisto("mpc0_trig2_crossing");
  TH1 *mpc1_trig2_crossing = cl->getHisto("mpc1_trig2_crossing");
  TH1 *mpc0_trig3_crossing = cl->getHisto("mpc0_trig3_crossing");
  TH1 *mpc1_trig3_crossing = cl->getHisto("mpc1_trig3_crossing");
  //TH1 *mpc0_trig4_crossing = cl->getHisto("mpc0_trig4_crossing");
  //TH1 *mpc1_trig4_crossing = cl->getHisto("mpc1_trig4_crossing");

  if (! gROOT->FindObject("MpcMon5"))
    {
      MakeCanvas("MpcMon5");
    }

  TC[4]->Clear("D");
  if (!mpc0_trig0_crossing)
    {
      isServerDead=true;
      //DrawDeadServer(transparent[4]);
      //TC[4]->Update();
      return -1;
    }
/*
  Pad[4][0]->cd();
  if(mpcvsbbcvtx)
    {
      mpcvsbbcvtx->SetStats(kFALSE);
      mpcvsbbcvtx->DrawCopy("colz");
    }
  else
    {
      DrawDeadServer(transparent[4]);
      TC[4]->Update();
      return -1;
    }

  Pad[4][1]->cd();
  if(mpcvsbbcnt0)
    {
      mpcvsbbcnt0->SetStats(kFALSE);
      mpcvsbbcnt0->DrawCopy("colz");
    }

  Pad[4][2]->cd();
  if(mpcvsbbcst0)
    {
      mpcvsbbcst0->SetStats(kFALSE);
      mpcvsbbcst0->DrawCopy("colz");
    }

  Pad[4][3]->cd();
  if(mpcvsbbct0)
    {
      mpcvsbbct0->SetStats(kFALSE);
      mpcvsbbct0->DrawCopy("colz");
    }
*/

  //
  // Trigger Crossings
  //
  Stat_t num_mpc_trig[5][2] = {{0}};
  num_mpc_trig[0][0] = mpc0_trig0_crossing->GetEntries();
  num_mpc_trig[1][0] = mpc0_trig1_crossing->GetEntries();
  num_mpc_trig[2][0] = mpc0_trig2_crossing->GetEntries();
  num_mpc_trig[3][0] = mpc0_trig3_crossing->GetEntries();
  //num_mpc_trig[4][0] = mpc0_trig4_crossing->GetEntries();
  num_mpc_trig[0][1] = mpc1_trig0_crossing->GetEntries();
  num_mpc_trig[1][1] = mpc1_trig1_crossing->GetEntries();
  num_mpc_trig[2][1] = mpc1_trig2_crossing->GetEntries();
  num_mpc_trig[3][1] = mpc1_trig3_crossing->GetEntries();
  //num_mpc_trig[4][1] = mpc1_trig4_crossing->GetEntries();
 
  Pad[4][0]->cd();
  // Trigger 1
  mpc0_trig0_crossing->SetStats(kFALSE);
  mpc1_trig0_crossing->SetStats(kFALSE);
  if( num_mpc_trig[0][0]>num_mpc_trig[0][1] )
    {
      mpc0_trig0_crossing->SetTitle("MPC Trig0 Crossing, North=blue South=gray");
      mpc0_trig0_crossing->DrawCopy();
      mpc1_trig0_crossing->DrawCopy("same");
    }
  else
    {
      mpc1_trig0_crossing->SetStats(kFALSE);
      mpc0_trig0_crossing->SetStats(kFALSE);
      mpc1_trig0_crossing->SetTitle("MPC Trig0 Crossing, North=blue South=gray");
      mpc1_trig0_crossing->DrawCopy();
      mpc0_trig0_crossing->DrawCopy("same");
    }

  Pad[4][1]->cd();
  mpc0_trig1_crossing->SetStats(kFALSE);
  mpc1_trig1_crossing->SetStats(kFALSE);
  if( num_mpc_trig[1][0]>num_mpc_trig[1][1] )
    {
      mpc0_trig1_crossing->SetTitle("MPC Trig1 Crossing, North=blue South=gray");
      mpc0_trig1_crossing->DrawCopy();
      mpc1_trig1_crossing->DrawCopy("same");
    }
  else
    {
      mpc1_trig1_crossing->SetTitle("MPC Trig1 Crossing, North=blue South=gray");
      mpc1_trig1_crossing->DrawCopy();
      mpc0_trig1_crossing->DrawCopy("same");
    }

  Pad[4][2]->cd();
  mpc0_trig2_crossing->SetStats(kFALSE);
  mpc1_trig2_crossing->SetStats(kFALSE);
  if( num_mpc_trig[2][0]>num_mpc_trig[2][1] )
    {
      mpc1_trig2_crossing->SetTitle("MPC Trig2 Crossing, North=blue South=gray");
      mpc0_trig2_crossing->DrawCopy();
      mpc1_trig2_crossing->DrawCopy("same");
    }
  if(mpc0_trig2_crossing)
    {
      mpc1_trig2_crossing->SetTitle("MPC Trig2 Crossing, North=blue South=gray");
      mpc1_trig2_crossing->DrawCopy();
      mpc0_trig2_crossing->DrawCopy("same");
    }
  Pad[4][3]->cd();
  mpc0_trig3_crossing->SetStats(kFALSE);
  mpc1_trig3_crossing->SetStats(kFALSE);
  if( num_mpc_trig[3][0]>num_mpc_trig[3][1] )
    {
      mpc1_trig3_crossing->SetTitle("MPC Trig3 Crossing, North=blue South=gray");
      mpc0_trig3_crossing->DrawCopy();
      mpc1_trig3_crossing->DrawCopy("same");
    }
  if(mpc0_trig3_crossing)
    {
      mpc1_trig3_crossing->SetTitle("MPC Trig3 Crossing, North=blue South=gray");
      mpc1_trig3_crossing->DrawCopy();
      mpc0_trig3_crossing->DrawCopy("same");
    }
/*
  Pad[4][4]->cd();
  mpc0_trig4_crossing->SetStats(kFALSE);
  mpc1_trig4_crossing->SetStats(kFALSE);
  if( num_mpc_trig[4][0]>num_mpc_trig[4][1] )
    {
      mpc1_trig4_crossing->SetTitle("MPC Trig4 Crossing, North=blue South=gray");
      mpc0_trig4_crossing->DrawCopy();
      mpc1_trig4_crossing->DrawCopy("same");
    }
  if(mpc0_trig4_crossing)
    {
      mpc1_trig4_crossing->SetTitle("MPC Trig4 Crossing, North=blue South=gray");
      mpc1_trig4_crossing->DrawCopy();
      mpc0_trig4_crossing->DrawCopy("same");
    }
*/

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.038);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_4 Run " << cl->RunNumber()
	      << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();

  transparent[4]->cd();
  PrintRun.DrawText(0.5, 0.98, runstring.c_str());
  TC[4]->Update();
  return 0;
}//end DrawFourth function

int MpcMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "Mpc MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}//end DrawDeadServer function

int MpcMonDraw::DrawBadReducedBits(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "Mpc MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "N/S Trig problem");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "Call MPC expert");
  transparent->SetFillColor(kRed);
  transparent->Update();
  return 0;
}

int MpcMonDraw::DrawHighVoltageOff(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.08);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "Mpc MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.64, "High Voltage Probably Off");
  FatalMsg.SetTextAlign(21); // center/center alignment
  FatalMsg.DrawText(0.5, 0.38, "Or High Voltage Set Wrong");
  FatalMsg.SetTextAlign(20); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "Check Mpc High Voltage");
  transparent->Update();
  return 0;
}//end DrawHighVoltageOff function

int MpcMonDraw::DrawVoltageTooLow(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.08);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "Mpc MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.6, "High Voltage Set Too Low");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.3, "Check Mpc High Voltage");
  FatalMsg.DrawText(0.5, 0.1, "Check Mpc Online Doc.");
  transparent->Update();
  return 0;
}

int MpcMonDraw::DrawVoltageTooHigh(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.08);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "Mpc MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.6, "High Voltage Set Too High");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.3, "Check Mpc High Voltage");
  FatalMsg.DrawText(0.5, 0.1, "Check Mpc Online Doc.");
  transparent->Update();
  return 0;
}//end DrawVoltageTooHigh function

int MpcMonDraw::DrawEnergyInstructions(TPad *transparent)
{
  transparent->cd();
/*
  TPaveText *FatalMsg = new TPaveText(0.13,0.47,0.40,0.515);
  FatalMsg->SetTextFont(62);
  FatalMsg->SetTextSize(0.02);
  FatalMsg->SetTextColor(4);
  FatalMsg->SetTextAlign(13); // left/top alignment
  FatalMsg->SetFillColor(4000);
  FatalMsg->SetBorderSize(0);
  FatalMsg->AddText("Black Histograms Should End");
  FatalMsg->AddText("Between The Magenta Lines");
  FatalMsg->AddText("(Check North and South Both)");
  FatalMsg->Draw();
*/
/*
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.02);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
//  FatalMsg.DrawText(0.27, 0.51, "Black Histograms Should End ");
  FatalMsg.DrawText(0.27, 0.52, "Black Histograms Should End ");
  FatalMsg.DrawText(0.27, 0.49, "  Between The Magenta Lines     ");
//  FatalMsg.DrawText(0.27, 0.47, "(Check North and South Both)");
  FatalMsg.DrawText(0.27, 0.46, "(Check North and South Both)");
*/
  transparent->Update();
  return 0;
}//end DrawEnergyInstructions function

int MpcMonDraw::DrawTimingInstructions(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.02);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.5, "If Histograms Don't Peak Between Red Lines, Feed MPC at End of Run");
  //FatalMsg.DrawText(0.46, 0.5, "If they don't, do MPC FEED at end of run");
  transparent->Update();
  return 0;
}//end DrawTimingInstructions function

int MpcMonDraw::DrawGridInstructions(TPad *transparent, double nled)
{
  transparent->cd();

  // Print overall message (OK or NOT OK)
  TText OK;
  OK.SetTextFont(62);
  OK.SetTextSize(0.04);
  OK.SetTextColor(3);
  OK.SetNDC();  // set to normalized coordinates  
  if ( nled<20 )
    {
      ostringstream LowEventsMsg;
      LowEventsMsg << "Need more than " << nled 
                   << " LED events, please be patient...";
      OK.SetTextAlign(21); 
      OK.DrawText(0.5, 0.135, LowEventsMsg.str().c_str() );
    }
  //else if( shigh < 7 && slow < 12 && nhigh < 7 && nlow < 12 )
  else if( shigh < 200 && slow < 200 && nhigh < 7 && nlow < 12 )
    {
      OK.SetTextAlign(21); 
      OK.DrawText(0.5, 0.135, "MPC is OK");
    }
  else
    {
      OK.SetTextAlign(21); 
      OK.SetTextColor(2);
      OK.DrawText(0.5, 0.135, "Too many bad towers, NOTIFY MPC EXPERT!");
    }

  // Print number of high or low LED towers
  TString strhigh = "Number of High Towers in South: ";
  strhigh += shigh;
  strhigh += " North: ";
  strhigh += nhigh;

  TString strlow = "Number of Low Towers in South: ";
  strlow += slow;
  strlow += " North: ";
  strlow += nlow;

  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.04);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); 
  FatalMsg.DrawText(0.5, 0.21, strhigh);
  FatalMsg.SetTextAlign(22); 
  FatalMsg.DrawText(0.5, 0.17, strlow);

  // Now print high or low towers (based on LED)
  TText Msg;
  Msg.SetTextFont(62);
  Msg.SetTextSize(0.025);
  Msg.SetTextColor(1);
  Msg.SetNDC();  // set to normalized coordinates
  double ytextpos = 0.12;

  //TString lvals = "Low Towers: ";
  ostringstream lvals;
  lvals << "Low Towers: ";
  if(slow || nlow)
    {

      for(int i = 0; i < nlow + slow; i++)
	{
	  lvals << lowindex[i] << "=" << setprecision(2) << lowval[i] << ",";
          if ( (i%8) == 7 || i == (nlow+slow-1) )
            {
              Msg.SetTextAlign(19); 
              //Msg.DrawText(0.5, ytextpos, lvals.Data());
              Msg.DrawText(0.5, ytextpos, lvals.str().c_str());
              ytextpos -= 0.02;
              lvals.str("");
            }
	}
    }

  //TString hvals = "High Towers: ";
  ostringstream hvals;
  hvals << "High Towers: ";
  if(shigh || nhigh)
    {

      for(int i = 0; i < nhigh + shigh; i++)
	{
          hvals << highindex[i] << "=" << setprecision(2) << highval[i] << ",";
          if ( (i%8) == 7 || i == (nhigh+shigh-1) )
            {
              Msg.SetTextAlign(20); 
              Msg.DrawText(0.5, ytextpos, hvals.str().c_str());
              ytextpos -= 0.02;
              hvals.str("");
            }
	}
    }

  transparent->Update();
  return 0;
}//end DrawGridInstructions function

int MpcMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // on error no ps files please
    {
      return iret;
    }
  filename << ThisName << "_Energy_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");
  if(!strcmp(species.c_str(), "pp"))
    filename << ThisName << "_South_Spectrum_" << cl->RunNumber() << ".ps";
  else if(!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
    filename << ThisName << "_Spectrum_" << cl->RunNumber() << ".ps";
  TC[1]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_Time_" << cl->RunNumber() << ".ps";
  TC[2]->Print(filename.str().c_str());
  filename.str("");
  if(!strcmp(species.c_str(), "pp"))
    filename << ThisName << "_North_Spectrum_" << cl->RunNumber() << ".ps";
  else if(!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
    filename << ThisName << "_Occupancy_" << cl->RunNumber() << ".ps";
  TC[3]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_Vertex_and_Timing_" << cl->RunNumber() << ".ps";
  TC[4]->Print(filename.str().c_str());
  
  return 0;
}//end MakePS function

int MpcMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  std::string pngfile = cl->htmlRegisterPage(*this,"Energy","1","png");
  cl->CanvasToPng(TC[0],pngfile);

  // idem for 2nd canvas.
  if(!strcmp(species.c_str(), "pp"))
    pngfile = cl->htmlRegisterPage(*this,"South Spectrum","2","png");
  if(!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
    pngfile = cl->htmlRegisterPage(*this,"Spectrum","2","png");
  cl->CanvasToPng(TC[1],pngfile);
  
  //3rd Canvas
  pngfile = cl->htmlRegisterPage(*this,"Time","3","png");
  cl->CanvasToPng(TC[2],pngfile);
  // Now register also EXPERTS html pages, under the EXPERTS subfolder.
  
  // idem for 4th canvas.
  if(!strcmp(species.c_str(), "pp"))
    pngfile = cl->htmlRegisterPage(*this,"North Spectrum","4","png");
  else if(!strcmp(species.c_str(), "dA") || !strcmp(species.c_str(), "AA"))
    pngfile = cl->htmlRegisterPage(*this,"Occupancy","4","png");
  
  cl->CanvasToPng(TC[3],pngfile);
  
  pngfile = cl->htmlRegisterPage(*this,"Vertex and Timing","5","png");
  cl->CanvasToPng(TC[4],pngfile);
    
  return 0;
}//end MakeHtml function
