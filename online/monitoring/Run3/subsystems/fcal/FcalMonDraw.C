#include <FcalMonDraw.h>
#include <OnlMonClient.h>

#include <TArrow.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TImage.h>
#include <TLine.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cstdlib> // getenv
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

FcalMonDraw::FcalMonDraw(const char *name): OnlMonDraw(name)
{
  memset(transparent,0,sizeof(transparent));
  memset(TC,0,sizeof(TC));
  int m = 0;
  mon[m++] = "FCALMON0"; //Shiftcrew
  mon[m++] = "FCALMON1";

  m = 0;
  montitle[m++] = "FCAL Gain-Corrected Online Monitor"; // FCALMON0 --shift crew
  montitle[m++] = "FCAL Low ADC Online Monitor";        // FCALMON1
  for (int i=0; i<FCALMONMAX;i++)
    {
      if (i<m)
	{
      Title[i] = new TPaveText(0.05, 0.65, 0.70, 0.95);
      Title[i]->AddText(montitle[i]);
	}
      else
	{
	Title[i] = 0;
    }
    }

  TStyle *oldStyle = gStyle;
  fcalStyle = new TStyle("fcalStyle", "FCAL Online Monitor Style");
  fcalStyle->SetOptStat(0);
  fcalStyle->SetHistLineColor(6);
  fcalStyle->SetHistFillColor(6);
  fcalStyle->SetHistLineWidth(3);
  //  fcalStyle->SetPadGridX(1);
  //fcalStyle->SetPadGridY(1);
  fcalStyle->SetPadColor(0);
  fcalStyle->SetPadBorderMode(0);
  fcalStyle->SetCanvasColor(0);
  fcalStyle->SetCanvasBorderMode(0);
  fcalStyle->SetPalette(1);
  fcalStyle->SetFrameBorderMode(0);
  oldStyle->cd();

  //initialize the dead channel graphics and the reference box
  deadText = new TText;
  deadBox = new TBox;

  return ;
}

FcalMonDraw::~FcalMonDraw()
{
  for (int i=0; i<FCALMONMAX;i++)
    {
      delete Title[i];
    }
  delete fcalStyle;
  delete deadText;
  delete deadBox;
  return;
}

int FcalMonDraw::MakeCanvas(const char *name)
{

  ostringstream stream;
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  // south
  for (int i = 0; i < FCALMONMAX; i++)
    {  // loop over all monitors in array

      if (!strcmp(name, mon[i]))
        {
          // create our monitor canvas
          int originX = -100-i*25;
          int originY = 0+i*25;
          int sizeX = xsize/3;
          int sizeY = ysize*3/4;
          if (originX == 0)
            {
              originX = 1;
            }
          // negative xpos removes damned menu bar
          TC[i] = new TCanvas(mon[i], montitle[i], originX, originY, sizeX, sizeY);
          gSystem->ProcessEvents();

          stream << mon[i] << "Lab"; // label pad

          // make a pad for our monitor label
          Lab[i] = new TPad(stream.str().c_str(), stream.str().c_str(), 0.00, 0.90, 1.00, 1.00, 0);
          Lab[i]->Draw();
          stream.str("");

          // this is our monitor label
          Label[i].SetTextSize(0.3);


          // create our pad for histograms (to be divided accordingly)
          stream << mon[i] << "Pad"; // pad for histograms
          Pad[i] = new TPad(stream.str().c_str(), stream.str().c_str(), 0.0, 0.0, 1.0, 0.90, 0);
          stream.str("");

          stream << "transparent" << i ;
          transparent[i] = new TPad(stream.str().c_str(), "this does not show", 0, 0, 1, 1);
          transparent[i]->SetFillStyle(4000);
          stream.str("");

        }
    }

  return 0;

}

int FcalMonDraw::InitCanvas(const char *what, int monindex, const int &x, const int &y)
{

  OnlMonClient *cl = OnlMonClient::instance();
  time_t evttime = cl->EventTime("CURRENT");
  ostringstream otext;
  otext.str("");
  otext << "Run #" << cl->RunNumber() << " Date " << ctime(&evttime) ;

  if (!gROOT->FindObject(what))
    {
      MakeCanvas(what);
      Pad[monindex]->Divide(x, y);
      Pad[monindex]->Draw();
      transparent[monindex]->Draw();
    }
  TC[monindex]->cd();
  Lab[monindex]->cd();
  Lab[monindex]->Clear();
  Label[monindex].SetText(0.05, 0.25, otext.str().c_str());
  Label[monindex].Draw();
  Title[monindex]->Draw();

  return 0;

}

int FcalMonDraw::Draw(const char *what)
{

  //  TStyle *oldStyle = gStyle;
  //fcalStyle->cd();
  int iret = 0;
  if ( !strcmp(what, "ALL") )
    {
      // for experts, draw all monitors
      cout << "Drawing all " << FCALMONMAX << " monitors." << endl;
      for (int i = 0; i < FCALMONMAX; i++)
        {
          iret += DrawMonitor(mon[i]);
        }
    }
  else if ( !strcmp(what, "SHIFT") )
    {
      // for shift crew only (yes, the word "all" is confusing, but this is what the default is for the online monitoring)
      //cout << "Drawing 6/" << FCALMONMAX << " monitors for Online Monitoring of data (shift crew)." << endl;
      //cout << "Note:  MakeHtml() by default will print all monitors for both shift crew and experts." << endl;
      iret += DrawMonitor(mon[0]);
      //      iret += DrawMonitor(mon[20]);

    }

  //oldStyle->cd();
  return iret;

}


int FcalMonDraw::DrawMonitor(const char *name)
{

  TStyle *oldStyle = gStyle;
  fcalStyle->cd();


  OnlMonClient *cl = OnlMonClient::instance();

  ostringstream idstring;

  if ( !strcmp(name, mon[0]))
    {
      //cout << "Drawing " << mon[0] << endl;
      InitCanvas(name, 0, 1, 2); // 2x2 canvas

      TH2* h2FCALNorth_loadc_corr;
      TH2* h2FCALSouth_loadc_corr;

      TC[0]->SetEditable(1);

      Pad[0]->cd(1);
      h2FCALNorth_loadc_corr = dynamic_cast<TH2 *>(cl->getHisto("fcal_north_loadc_corrected"));
      gPad->SetLogz();
      if (h2FCALNorth_loadc_corr)
	{
	  h2FCALNorth_loadc_corr->DrawCopy("col4z");
	  drawDeadChannels(FCALNORTH);
	}
      else
	{
	  DrawDeadServer(transparent[0]);
	  TC[0]->SetEditable(0);
	  oldStyle->cd();
	  return -1;
	}
      
      Pad[0]->cd(2);
      gPad->SetLogz();
      h2FCALSouth_loadc_corr = dynamic_cast<TH2 *>(cl->getHisto("fcal_south_loadc_corrected"));
      if (h2FCALSouth_loadc_corr)
	{
	  h2FCALSouth_loadc_corr->DrawCopy("col4z");
	  drawDeadChannels(FCALSOUTH);
	}
      else
	{
	  DrawDeadServer(transparent[0]);
	  TC[0]->SetEditable(0);
	  oldStyle->cd();
	  return -1;
	}
      
      TC[0]->Update();
      TC[0]->Show();
      TC[0]->SetEditable(0);
    }
  else if( !strcmp(name, mon[1]))
    {
      //cout << "Drawing " << mon[1] << endl;
      InitCanvas(name, 1, 1, 2); // 2x2 canvas

      TH2* h2FCALNorth_loadc;
      TH2* h2FCALSouth_loadc;

      TC[1]->SetEditable(1);

      Pad[1]->cd(1);
      h2FCALNorth_loadc = dynamic_cast<TH2 *>(cl->getHisto("fcal_north_loadc"));
      gPad->SetLogz();
      if (h2FCALNorth_loadc)
	{
	  h2FCALNorth_loadc->DrawCopy("col4z");
	  drawDeadChannels(FCALNORTH);
	}
      else
	{
	  DrawDeadServer(transparent[1]);
	  TC[1]->SetEditable(0);
	  oldStyle->cd();
	  return -1;
	}
      
      Pad[1]->cd(2);
      gPad->SetLogz();
      h2FCALSouth_loadc = dynamic_cast<TH2 *>(cl->getHisto("fcal_south_loadc"));
      if (h2FCALSouth_loadc)
	{
	  h2FCALSouth_loadc->DrawCopy("col4z");
	  drawDeadChannels(FCALSOUTH);
	}
      else
	{
	  DrawDeadServer(transparent[1]);
	  TC[1]->SetEditable(0);
	  oldStyle->cd();
	  return -1;
	}
      
      TC[1]->Update();
      TC[1]->Show();
      TC[1]->SetEditable(0);
    }
  else
    {
      cout << "FcalMonDraw::Draw Unknown monitor " << name << endl;
      oldStyle->cd();
      return -1;
    }

  oldStyle->cd();
  return 0;

}

int FcalMonDraw::drawDeadChannels(int det)
{

  ostringstream filename;
  ifstream infile;

  filename.str("");
  if (getenv("FCALDEADCHANNELMAP"))
    {
      filename << getenv("FCALDEADCHANNELMAP");
    }
  else
    {
      filename << "calib/fcal_deadchannel_map.dat";
    }

  infile.open(filename.str().c_str(),ios_base::in);

  int compass = -1;
  int column = -1;
  int row = -1;

  if(infile)
    {
      while (!infile.eof() )
	{
	  infile >> compass >> column >> row;
	  if(compass > -1 && compass == det) {
	    //bdead = new TBox(column-0.5,row-0.5,column+0.5,row+0.5);
	    deadBox->SetFillColor(1);
	    deadBox->DrawBox(column-0.5,row-0.5,column+0.5,row+0.5);
	    //tdead = new TText(column, row, "Dead");
	    deadText->SetTextAlign(22);
	    deadText->SetTextColor(10);
	    deadText->DrawText(column, row, "Dead");
	    
	  }
	}
      infile.close();
    }
  else
    {
      cout << "Could not open file " << filename.str()
	   << " no fcal dead channels loaded " << endl;
      return -1;
    }

  return 0;

}


int FcalMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream stream;
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }

  stream.str("");
  for (int i = 0 ; i < FCALMONMAX; i++)
    {

      if (TC[i])  // print all monitors
        {
          stream << mon[i] << "_Run" << cl->RunNumber() << ".ps";
          TC[i]->Print(stream.str().c_str());
        }
      stream.str("");
    }

  return 0;

}

int FcalMonDraw::MakeHtml(const char *what)
{

  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  OnlMonClient *cl = OnlMonClient::instance();

  ostringstream snum, name;

  for (int i = 0; i < FCALMONMAX; i++)
    {
      if (TC[i]) // scan through and print monitors
        {
          if (i == 0)
            {
              //Shift Crew
              snum << (i + 1) ;
              name << montitle[i] ;
              string pngfile = cl->htmlRegisterPage(*this, name.str().c_str(), snum.str().c_str(), "png");
              cl->CanvasToPng(TC[i], pngfile);
              snum.str("");
              name.str("");
            }
          else
            {
              //Expert only
              snum << (i + 1) ;
              name << "Experts/" << montitle[i] ;
              string pngfile = cl->htmlRegisterPage(*this, name.str().c_str(), snum.str().c_str(), "png");
              cl->CanvasToPng(TC[i], pngfile);
              snum.str("");
              name.str("");
            }
        }
    }
  cl->SaveLogFile(*this);

  return 0;
}

int FcalMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "FCAL");
  FatalMsg.SetTextAlign(22); // center/center alignment

  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}



