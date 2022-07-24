#include <MutrMonDraw.h>
#include <OnlMonClient.h>
#include <RunDBodbc.h>

#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TLine.h>
#include <TLatex.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

const char *CARM[] =
  { "South", "North"
  };

MutrMonDraw::MutrMonDraw(): OnlMonDraw("MUTRMON")
{
  memset(h1ZSRatioStrips,0,sizeof(h1ZSRatioStrips));
  memset(h1MultEst,0,sizeof(h1MultEst));
  memset(tbar,0,sizeof(tbar));
  memset(tltex,0,sizeof(tltex));
  memset(mutCanv,0,sizeof(mutCanv));
  memset(mutPad,0,sizeof(mutPad));
  memset(pt,0,sizeof(pt));
  return ;
}

MutrMonDraw::~MutrMonDraw()
{

  for (int i = 0;i < 2;i++)
    {
      delete tbar[i];
    }
  for (int i = 0;i < 3;i++)
    {
      delete tltex[i];
    }
  // TPaveTex
  for (int ipt = 0; ipt < 2*NPAVETEX; ipt++)
    {
      delete pt[ipt];
    }
  for (int i = 0; i < NARMS; i++)
    {
      delete h1ZSRatioStrips[i];
      delete h1MultEst[i];
    }
  return;
}

int MutrMonDraw::Init()
{
  // also have two local histo, not handled by the server/client stuff
  if ( !gROOT->FindObject("h1ZSRatioStrips[0]"))
    {
      MakeLocalHistos(0);
    }
  if ( !gROOT->FindObject("h1ZSRatioStrips[1]"))
    {
      MakeLocalHistos(1);
    }
  string mutcalibfile;
  if (!getenv("MUTRCALIBDIR"))
    {
      mutcalibfile = ".";
    }
  else
    {
      mutcalibfile = getenv("MUTRCALIBDIR");
    }
  mutcalibfile = mutcalibfile + "/mutr_knownbadfems.dat";
  getBadFems(mutcalibfile.c_str());

  // TLine  // canvases and pads
  tbar[0] = new TLine();
  tbar[0]->SetLineColor(BCOL);
  tbar[0]->SetLineWidth(LW);
  tbar[0]->SetBit(1);

  tbar[1] = new TLine();
  tbar[1]->SetLineColor(WCOL);
  tbar[1]->SetLineWidth(LW);
  tbar[1]->SetLineStyle(2);
  tbar[1]->SetBit(1);


  tltex[0] = new TLatex();
  tltex[0]->SetTextSize(TXTSIZE);
  tltex[0]->SetLineWidth(LW);
  tltex[0]->SetTextColor(4); // blue

  tltex[1] = new TLatex();
  tltex[1]->SetTextSize(TXTSIZE*0.8);
  tltex[1]->SetLineWidth(LW);
  tltex[1]->SetTextColor(4); // blue

  tltex[2] = new TLatex();
  tltex[2]->SetTextSize(TXTSIZE);
  tltex[2]->SetLineWidth(LW);
  tltex[2]->SetTextColor(1); // black

  return 0;
}

void MutrMonDraw::registerHists()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->registerHisto("mutH1NumEvent", "MUTRMON");
  ostringstream id;
  for (int i = 0; i < NARMS; i++)
    {
      id.str("");
      id << "mutH1ZSRatio" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutHprofPktNchan" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutHprofPktErr" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutHprofHits" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutH1Signal" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutHprofAmplTime" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutHprofAmplTime_nstr" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutH1ClustAdc" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutH1PeakAdc" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      id.str("");
      id << "mutPacketActive" << i ;
      cl->registerHisto(id.str().c_str(), "MUTRMON");
      for (int ista = 0; ista < NSTATIONS; ista++)
        {
          id.str("");
          id << "mutH1PeakAdcSta" << i << ista ;
          cl->registerHisto(id.str().c_str(), "MUTRMON");
          for (int ioct = 0; ioct < NOCTANTS; ioct++)
            {
              id.str("");
              id << "mutH1PeakAdcStaOct" << i << ista << ioct ;
              cl->registerHisto(id.str().c_str(), "MUTRMON");
            }
        }
    }
  return ;
}

int MutrMonDraw::MakeLocalHistos(int iarm)
{
  if ( !gROOT->FindObject("h1ZSRatioStrips[0]") || !gROOT->FindObject("h1ZSRatioStrips[1]") )
    {
      // occupancy
      if (iarm == 0)
        {
          h1ZSRatioStrips[0] = new TH1F("h1ZSRatioStrips[0]",
                                        "S Occupancy (STA3 equiv.)",
                                        NPACKETSARM[0],
                                        FIRSTPACKET[0] - 0.5,
                                        NPACKETSARM[0] + FIRSTPACKET[0] - 0.5);
        }
      if (iarm == 1)
        {
          h1ZSRatioStrips[1] = new TH1F("h1ZSRatioStrips[1]",
                                        "N Occupancy (STA3 equiv.)",
                                        NPACKETSARM[1],
                                        FIRSTPACKET[1] - 0.5,
                                        NPACKETSARM[1] + FIRSTPACKET[1] - 0.5);
        }
      // set titles
      h1ZSRatioStrips[iarm]->SetStats(kFALSE);
      h1ZSRatioStrips[iarm]->GetXaxis()->SetTitle("Packet");
      h1ZSRatioStrips[iarm]->GetXaxis()->SetTitleSize(0.06);
      h1ZSRatioStrips[iarm]->GetXaxis()->SetTitleOffset(0.8);
      h1ZSRatioStrips[iarm]->GetYaxis()->SetTitle("Occ.");
      h1ZSRatioStrips[iarm]->GetYaxis()->SetTitleSize(0.06);
      h1ZSRatioStrips[iarm]->GetYaxis()->SetTitleOffset(0.8);
    }

  if ( !gROOT->FindObject("h1MultEst[0]") || !gROOT->FindObject("h1MultEst[1]") )
    {
      // mult estimate
      if (iarm == 0)
        {
          h1MultEst[0] = new TH1F("h1MultEst[0]",
                                  "S Multiplicity estimate",
                                  NOCTANTS * (NPLANES[0] + NPLANES[1] + NPLANES[2]),
                                  - 0.5,
                                  NOCTANTS * (NPLANES[0] + NPLANES[1] + NPLANES[2]) - 0.5);
        }
      if (iarm == 1)
        {
          h1MultEst[1] = new TH1F("h1MultEst[1]",
                                  "N Multiplicity estimate",
                                  NOCTANTS * (NPLANES[0] + NPLANES[1] + NPLANES[2]),
                                  - 0.5,
                                  NOCTANTS * (NPLANES[0] + NPLANES[1] + NPLANES[2]) - 0.5);
        }
      // set titles
      h1MultEst[iarm]->SetStats(kFALSE);
      h1MultEst[iarm]->GetXaxis()->SetTitle("Plane");
      h1MultEst[iarm]->GetXaxis()->SetTitleSize(0.06);
      h1MultEst[iarm]->GetXaxis()->SetTitleOffset(0.8);
      h1MultEst[iarm]->GetYaxis()->SetTitle("Mult. est.");
      h1MultEst[iarm]->GetYaxis()->SetTitleSize(0.06);
      h1MultEst[iarm]->GetYaxis()->SetTitleOffset(0.8);
    }
  return 0;
}

int MutrMonDraw::getBadFems(const char *filename)
{
  ifstream fin(filename);
  if (!fin)
    {
      cout << "MutrMonDraw::getBadFems - can't open " << filename << " for input" << endl;
      return 1;
    }

  // start fresh
  badfemset.clear();
  int nfems = 0;
  fin >> nfems;

  int packetid = 0, ifem = 0;
  while ( fin.good() && ifem < nfems )
    {
      fin >> packetid;
      if (! fin.good() )
        {
          break;
        }
      badfemset.insert(packetid);
      ifem++;
    }
  cout << " " << ifem << " bad MUTR fems read "
       << nfems << " expected " << endl
       << " List of bad FEMs " << endl;

  set
    <int>::iterator femiter = badfemset.begin();
  while (femiter != badfemset.end())
    {
      cout << *femiter << endl;
      femiter++;
    }

  return 0;
}

int MutrMonDraw::createCanvas(const int arm)
{
  ostringstream id;
  ostringstream info;

  if (arm < 0 || arm >= NARMS)
    {
      cout << " MutrMonDraw::createCanvas - failed for arm : "
	   << arm << " index out of range " << endl;
      return -1;
    }

  int i = arm;
  id.str("");
  info.str("");
  id << "mutCanv" << i ;
  info << "Station 1-3 : Arm " << i << " : " << CARM[i] ;

  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // -0 does not wipe out this stupid menubar
  mutCanv[i] = new TCanvas(id.str().c_str(), info.str().c_str(), -1 - (i * xsize / 2), 0, xsize / 2, ysize);
  gSystem->ProcessEvents();
  createPads(i);
  return 0;
}

int MutrMonDraw::createPads(const int arm)
{
  ostringstream id;
  ostringstream info;
  float xlow, ylow, xup, yup;
  if (arm < 0 || arm >= NARMS)
    {
      cout << " MutrMonDraw::createPads - failed for arm : "
	   << arm << " index out of range " << endl;
      return -1;
    }

  int i = arm;
  mutCanv[i]->cd();
  for (int j = 0; j < NPADSPERARM; j++)
    {
      id.str("");
      info.str("");
      id << "mutPad" << i << j ;
      info << "Station 1-3 : Arm " << i << " : " << CARM[i]
	   << " Pad " << j ;

      xlow = DISTBORD + (j % 2) * PADSIZE;
      ylow = DISTBORD + (1 - j / 2) * PADSIZE;
      xup = xlow + (PADSIZE - 2 * DISTBORD);
      yup = ylow + (PADSIZE - 2 * DISTBORD);
      mutPad[i][j] = new TPad(id.str().c_str(), info.str().c_str(), xlow, ylow, xup, yup, 0);
      // the last argument (0) is the color?
      mutPad[i][j]->Draw();
    }
  return 0;
}

int MutrMonDraw::clearPads(const int arm)
{
  ostringstream posting;
  if (arm < 0 || arm >= NARMS)
    {
      cout << " MutrMonDraw::clearPads - failed for arm : "
	   << arm << " index out of range " << endl;
      return -1;
    }

  int i = arm;
  for (int j = 0; j < NPADSPERARM; j++)
    {
      /* ROOT freaks out if we actually try to delete or close the pad.
      // possibly: the Canvas->Clear() command removes the object
      // but doesn't reset the pointers; so I'm doing it by hand below
      if (mutPad[i][j])
      {
      mutPad[i][j]->Close(); 
      }
      */
      mutPad[i][j] = NULL; // reset pointer
    }
  return 0;
}

int MutrMonDraw::Draw(const char *what)
{
  ostringstream posting;
  // check whether Canvases are still around. If not, recreate them..
  // This is handled in the same way as in the Init routine..
  if (! gROOT->FindObject("mutCanv0") )
    {
      createCanvas(0);
    }
  if (! gROOT->FindObject("mutCanv1") )
    {
      createCanvas(1);
    }
  // check whether server is alive. We do this by asking for
  OnlMonClient *Onlmonclient = OnlMonClient::instance();
  TH1F* mutH1NumEvent = (TH1F*)Onlmonclient->getHisto("mutH1NumEvent");
  int eventNum = 0;
  TText mutr_message;

  RunDBodbc rundb;
  string RunType=rundb.RunType(Onlmonclient->RunNumber());

  // we'll echo output messages to a log file also
  ofstream outfile("mutrclientlog.txt");

  if (mutH1NumEvent)
    {
      eventNum = (int)mutH1NumEvent->GetEntries();
      if (eventNum == 0)
        {
          cout << "MUTR : no events have yet been processed" << endl;
          outfile << "MUTR : no events have yet been processed" << endl;
          mutCanv[0]->Clear();
          clearPads(0);
          mutr_message.DrawTextNDC(0.3, 0.5, "MUTR.S : No events processed!");
          mutCanv[0]->Update();
          mutCanv[1]->Clear();
          clearPads(1);
          mutr_message.DrawTextNDC(0.3, 0.5, "MUTR.N : No events processed!!");
          mutCanv[1]->Update();
          outfile.close();
          return -1;
        }
    }
  else
    {
      cout << "MUTR Server appears dead. Cannot update histograms." << endl ;
      outfile << "MUTR Server appears dead. Cannot update histograms." << endl;
      mutCanv[0]->Clear();
      clearPads(0);
      mutr_message.DrawTextNDC(0.3, 0.5, "MUTR.S : Server appears dead!");
      mutCanv[1]->Clear();
      clearPads(1);
      mutr_message.DrawTextNDC(0.3, 0.5, "MUTR.N : Server appears dead!");
      outfile.close();
      return -1;
    }

  // Pads are explicitly recreated pads, if they were removed by
  // the Canvas->Clear() command.
  if (mutPad[0][0] == NULL)
    {
      mutCanv[0]->Clear(); // make sure old text is removed
      createPads(0);
    }
  if (mutPad[1][0] == NULL)
    {
      mutCanv[1]->Clear();
      createPads(1);
    }

  // initial report to logfile
  // run and time info
  time_t t = Onlmonclient->EventTime();
  tm * localT = localtime(&t);
  posting.str("");
  posting << "Run# " << Onlmonclient->RunNumber() << "   Nevt:" << eventNum
	  << "   Date:" << localT->tm_mon + 1 << "/" << localT->tm_mday
	  << "/" << localT->tm_year + 1900 << " " << localT->tm_hour
	  << ":" << localT->tm_min << endl;
  outfile << posting.str().c_str();

  // setup, ROOT appearance; keep the old settings and put them back in the end
  TStyle* oldStyle = gStyle;
  TStyle* mutStyle = new TStyle(*gStyle);
  gStyle = mutStyle;
  gStyle->Reset();

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleH(0.08);

  gStyle->SetTitleXSize(0.045);
  gStyle->SetTitleYSize(0.045);
  gStyle->SetTitleXOffset(0.99);
  gStyle->SetTitleYOffset(0.95);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  // end of settings

  // cleanup
  // TPaveTex
  for (int ipt = 0; ipt < 2*NPAVETEX; ipt++)
    {
      if (pt[ipt])
        {
          delete pt[ipt];
          pt[ipt] = NULL;
        }
    }

  TAxis *axis;
  double min, max; // of histo
  int bin, binmin, binmax;
  double xmin, xmax, ymin, ymax; // of axis

  const char *ERRORMSG[] =
    { "HOT fem(s), FEED, check calib ! ",
      "MISSING fem(s), FEED, check calib ! ",
      "High activity per plane",
      "Low activity per plane, CHECK HVs !",
      "AMU errors, FEED after the run!",
      "Incorrect Timing.",
      "High gain, CHECK GAS & HV !",
      "Low gain, CHECK GAS & HV !"
    };
  int errorcode;

  // get all the histos
  // everything is declared as TH1
  TH1 *mutH1ZSRatio[NARMS];
  TH1 *mutHprofPktNchan[NARMS];
  TH1 *mutHprofPktErr[NARMS];
  TH1 *mutHprofHits[NARMS];
  TH1 *mutH1Signal[NARMS];
  TH1 *mutHprofAmplTime[NARMS];
  TH1 *mutHprofAmplTime_nstr[NARMS];
  //  TH1 *mutH1ClustAdc[NARMS];
  TH1 *mutH1PeakAdc[NARMS];
  TH1 *mutActivePkt[NARMS];
  // helper to possibly find bad FEMs/packets and
  // not use them when signaling errors
  set<int>::iterator badfemiter = badfemset.begin();

  ostringstream id;
  for (int i = 0; i < NARMS; i++)
    {
      errorcode = 0;
      // check that we had some packet entries
      id.str("");
      id << "mutHprofPktNchan" << i ;
      mutHprofPktNchan[i] = Onlmonclient->getHisto(id.str().c_str());
      int npktentries = 0;
      if (mutHprofPktNchan[i])
        {
          npktentries = (int)mutHprofPktNchan[i]->GetEntries();
          //cout << i << " npktentries = " << npktentries << endl;
          if (npktentries == 0)
            { // no packets found
              mutCanv[i]->Clear();
              clearPads(i);
              posting.str("");
              posting << "MUTR." << CARM[i] << " : no packets found!" << endl;
              mutr_message.DrawTextNDC(0.3, 0.5, posting.str().c_str());
              errorcode++;
            }
        }
      else
        { // no histogram found
          mutCanv[i]->Clear();
          clearPads(i);
          posting.str("");
          posting << "MUTR." << CARM[i] << " : missing histograms!" ;
          mutr_message.DrawTextNDC(0.3, 0.5, posting.str().c_str());
          errorcode++;
        }
      if (errorcode > 0)
        {
          continue; // don't try anything else with this arm..
        }
      id.str("");
      id << "mutH1ZSRatio" << i ;
      mutH1ZSRatio[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutHprofPktErr" << i ;
      mutHprofPktErr[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutHprofHits" << i ;
      mutHprofHits[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutH1Signal" << i ;
      mutH1Signal[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutHprofAmplTime" << i ;
      mutHprofAmplTime[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutHprofAmplTime_nstr" << i ;
      mutHprofAmplTime_nstr[i] = Onlmonclient->getHisto(id.str().c_str());
      //      id.str(""); id << "mutH1ClustAdc" << i ;
      //      mutH1ClustAdc[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutH1PeakAdc" << i ;
      mutH1PeakAdc[i] = Onlmonclient->getHisto(id.str().c_str());
      id.str("");
      id << "mutPacketActive" << i ;
      mutActivePkt[i] = Onlmonclient->getHisto(id.str().c_str());

      mutCanv[i]->cd();
      // 1st plot: nchan per packet

      mutPad[i][0]->cd();
      if (mutHprofPktNchan[i])
        {
          // scale the ratios with how many strips are in each octant
          int ibin, stabin = 0, ista = 0;
          float bincont, binerr;
          h1ZSRatioStrips[i]->Reset();
          float ratioscalefactor[NSTATIONS];
          // calc the scale factor once and for all
          for (ista = 0; ista < NSTATIONS; ista++)
            {
              ratioscalefactor[ista] =
                NSTRIPS[i * 3 + ista] / (float) NSTRIPS[i * 3 + 2];
            }
          ista = 0; // reset the counter; first bins are station 1..
          // we'll calc. the average per station also and put in the plot
          float staaverage[NSTATIONS] = { 0., 0., 0.};
          int stacounts[NSTATIONS] = { 0, 0, 0};
          // for the first two plots with max and min limits
          // keep tracks of which packets and planes are below or above limits
          std::vector<MutOnl> packetsabove;
          std::vector<MutOnl> packetsbelow;
          packetsabove.clear();
          packetsbelow.clear();
          MutOnl mutOnl; // an object to work with

          int packetid = 0;

          for (ibin = 0; ibin < NPACKETSARM[i]; ibin++)
            {
              bincont = (float) mutHprofPktNchan[i]->GetBinContent(ibin + 1);
              binerr = (float) mutHprofPktNchan[i]->GetBinError(ibin + 1);
              if (stabin == (NPACKETS[i*3 + ista]*NPARTS[ista]) )
                {
                  ista++;
                  stabin = 0;
                }
              // for the averages
              staaverage[ista] += bincont;
              stacounts[ista] ++;
              // weight the contents by how many strips there are
              // in the station compared to the last station in the arm
              bincont *= ratioscalefactor[ista];
              binerr *= ratioscalefactor[ista];
              //outfile << "arm " << i << " bin " << ibin << endl;

              h1ZSRatioStrips[i]->SetBinContent(ibin + 1,
                                                (Stat_t) bincont);
              h1ZSRatioStrips[i]->SetBinError(ibin + 1,
                                              (Stat_t) binerr);

              packetid = ibin + FIRSTPACKET[i];
              // check all good packets, i.e. exclude all believed to be bad..
              badfemiter = badfemset.find(packetid);
              if ( badfemiter == badfemset.end() )
                { // this packet is not in the bad set, so include it
                  // in the error checking

                  // boundary checking, is the value outside limits?
                  if (bincont > YMAXPACKETS[i])
                    {
                      mutOnl.id = packetid;
                      mutOnl.val = bincont;
                      mutOnl.err = binerr;
                      packetsabove.push_back(mutOnl);
                    }
                  if (bincont < YMINPACKETS[i])
                    {
		      if (mutActivePkt[i]->GetBinContent(ibin) > 0)
			{
                      mutOnl.id = packetid;
                      mutOnl.val = bincont;
                      mutOnl.err = binerr;
                      packetsbelow.push_back(mutOnl);
			}
		      else
			{
			  cout << "packet " << packetid << " disabled" << endl;
			}
                    }
                }
              stabin++;
            }

          // calc. the averages
          for (ista = 0; ista < NSTATIONS; ista++)
            {
              if (stacounts[ista] > 0)
                { // convert to %
                  staaverage[ista] = 100.0 * staaverage[ista] / stacounts[ista];
                }
              else
                { // this is what we already should have if no counts
                  // were found but I include it here anyway for clarity
                  staaverage[ista] = 0.0;
                }
            }

          axis = h1ZSRatioStrips[i]->GetXaxis();
          //axis->SetRange(binoffset + 1, binoffset + NPACKETSARM[i]);

          // boundary sum checking, was too much outside limits?
          int packetsabovesize = (int) packetsabove.size();
          int packetsbelowsize = (int) packetsbelow.size();
          int paintItRed = 0;
          if (packetsabovesize > MAX_ABOVEPACKETS[i])
            {
              errorcode += 0x1;
              paintItRed = 1;
            }
          if (packetsbelowsize > MAX_BELOWPACKETS[i])
            {
              errorcode += 0x2;
              paintItRed = 1;
            }

          // draw with green color if not error, else paint it red!!
          h1ZSRatioStrips[i]->SetLineColor(3 - paintItRed);

          h1ZSRatioStrips[i]->SetFillColor(5);
          h1ZSRatioStrips[i]->Draw("hist");

          // get histo info
          binmin = axis->GetFirst();
          binmax = axis->GetLast();
          xmin = axis->GetBinLowEdge(binmin);
          xmax = axis->GetBinUpEdge(binmax);

          max = h1ZSRatioStrips[i]->GetMaximum();
          min = h1ZSRatioStrips[i]->GetMinimum();
          ymin = 0.0;
          ymax = max * BSCALE;

          // borders between stations
          // 40 packets in station 1
          xmin += NPARTS[0] * NPACKETS[i * 3];
          tbar[0]->DrawLine(xmin, ymin, xmin, ymax);

          // 64 + iarm*8 packets in station 2: 104 = 64+40..
          xmin += NPARTS[1] * NPACKETS[i * 3 + 1];
          tbar[0]->DrawLine(xmin, ymin, xmin, ymax);

          xmin = axis->GetBinLowEdge(binmin); // restart
          int TXTOFF[3] = {0, 10, 15};
          // station 1,2,3 labels
          tltex[0]->DrawLatex(xmin + TXTOFF[0], max * TXTSCALE, "Station 1");
          tltex[0]->DrawLatex(xmin + 40 + TXTOFF[1],
                              max * TXTSCALE, "Station 2");
          tltex[0]->DrawLatex(xmin + 104 + TXTOFF[2],
                              max * TXTSCALE, "Station 3");
          // also give info on non-scaled zero-supp. factors for the 3 stations
          posting.str("");
          posting << "Mean: " << setprecision(3)
		  << staaverage[0] << " %" ;
          tltex[1]->DrawLatex(xmin + TXTOFF[0],
                              min*TXTSCALE + 0.0002, posting.str().c_str());
          posting.str("");
          posting << "Mean: " << setprecision(3)
		  << staaverage[1] << " %" ;
          tltex[1]->DrawLatex(xmin + 40 + TXTOFF[1],
                              min*TXTSCALE + 0.0002, posting.str().c_str());
          posting.str("");
          posting << "Mean: " << setprecision(3)
		  << staaverage[2] << " %" ;
          tltex[1]->DrawLatex(xmin + 104 + TXTOFF[2],
                              min*TXTSCALE + 0.0002, posting.str().c_str());
          // warning borders
          tbar[1]->DrawLine(xmin, YMINPACKETS[i], xmax, YMINPACKETS[i]);
          tbar[1]->DrawLine(xmin, YMAXPACKETS[i], xmax, YMAXPACKETS[i]);

          // should also print out who had the errors
          if (packetsabovesize > 0)
            {
              outfile << " MutrMonDraw Arm " << CARM[i]
		      << " " << packetsabove.size()
		      << " packets above set limit (hot packet):" << endl;
              for (unsigned int ip = 0; ip < packetsabove.size(); ip++)
                {
                  packetsabove[ip].write(outfile);
                  outfile << "; ";
                }
              outfile << endl;
            }
          if (packetsbelow.size() > 0)
            {
              outfile << " MutrMonDraw Arm " << CARM[i]
		      << " " << packetsbelow.size()
		      << " packets below set limit (missing packet):" << endl;
              for (unsigned int ip = 0; ip < packetsbelow.size(); ip++)
                {
                  packetsbelow[ip].write(outfile);
                  outfile << "; ";
                }
              outfile << endl;
            }
        }

      // 2nd plot: hits per plane
      mutPad[i][1]->cd();
      if (mutHprofHits[i])
        {
          // set titles
          h1MultEst[i]->Reset();
          // the histogram has the occupancy per plane (covering one octant's
          // worth of solid angle)
          // If we multiply the contents by a factor 8 = NOCTANTS
          // we have an estimate of the multiplicity entering the arm

          // we'll go thru the histogram bin by bin for
          // boundary checking, is the value outside limits?
          int ibin, stabin = 0, ista = 0;
          float bincont, binerr;
          float staaverage[NSTATIONS] = { 0., 0., 0.};
          int stacounts[NSTATIONS] = { 0, 0, 0};

          //
          std::vector<MutOnl> planesabove;
          std::vector<MutOnl> planesbelow;
          planesabove.clear();
          planesbelow.clear();
          MutOnl mutOnl; // an object to work with

          for (ibin = 0; ibin < NOCTANTS*(NPLANES[0] + NPLANES[1] + NPLANES[2]);
               ibin++)
            {
              bincont = (float) mutHprofHits[i]->GetBinContent(ibin + 1);
              binerr = (float) mutHprofHits[i]->GetBinError(ibin + 1);

              // here comes the rescaling to the full solid angle
              // of the muon arms
              bincont *= NOCTANTS;
              binerr *= NOCTANTS;
              h1MultEst[i]->SetBinContent(ibin + 1, (Stat_t) bincont);
              h1MultEst[i]->SetBinError(ibin + 1, (Stat_t) binerr);

              if (stabin == (NOCTANTS*NPLANES[ista]) )
                {
                  ista++;
                  stabin = 0;
                }

              staaverage[ista] += bincont;
              stacounts[ista]++;

              if (bincont > YMAXPLANES[i])
                {
                  int octant = stabin / NPLANES[ista];
                  int plane = stabin % NPLANES[ista];
                  mutOnl.id = ista * 100 + octant * 10 + plane;
                  mutOnl.val = bincont;
                  mutOnl.err = binerr;
                  planesabove.push_back(mutOnl);
                }
              if (bincont < YMINPLANES[i])
                {
                  int octant = stabin / NPLANES[ista];
                  int plane = stabin % NPLANES[ista];
                  mutOnl.id = ista * 100 + octant * 10 + plane;
                  mutOnl.val = bincont;
                  mutOnl.err = binerr;
                  planesbelow.push_back(mutOnl);
                }
              stabin++;
            }

          // calc. the averages
          for (ista = 0; ista < NSTATIONS; ista++)
            {
              if (stacounts[ista] > 0)
                {
                  staaverage[ista] = staaverage[ista] / stacounts[ista];
                }
              else
                { // this is what we already should have if no counts
                  // were found but I include it here anyway for clarity
                  staaverage[ista] = 0.0;
                }
            }

          // boundary sum checking, was too much outside limits?
          int planesabovesize = (int) planesabove.size();
          int planesbelowsize = (int) planesbelow.size();
          int paintItRed = 0;
          if (planesabovesize > MAX_ABOVEPLANES[i])
            {
              errorcode += 0x4;
              paintItRed = 1;
            }
          if (planesbelowsize > MAX_BELOWPLANES[i])
            {
              errorcode += 0x8;
              paintItRed = 1;
            }

          h1MultEst[i]->SetLineColor(3 - paintItRed);

          h1MultEst[i]->SetFillColor(5);
          h1MultEst[i]->Draw("hist");

          // get histo info
          axis = h1MultEst[i]->GetXaxis();
          binmin = axis->GetFirst();
          binmax = axis->GetLast();
          xmin = axis->GetBinLowEdge(binmin);
          xmax = axis->GetBinUpEdge(binmax);

          max = h1MultEst[i]->GetMaximum();
          min = h1MultEst[i]->GetMinimum();
          ymin = 0.0;
          ymax = max * BSCALE;

          // borders between stations
          // 8*6 planes in station 1
          tbar[0]->DrawLine(xmin + 48, ymin, xmin + 48, ymax);
          // 8*6 planes in station 2 also: 96 = 48*2
          tbar[0]->DrawLine(xmin + 96, ymin, xmin + 96, ymax);

          int TXTOFF[3] =
            {
              10, 10, 5
            };
          // station 1,2,3 labels
          tltex[0]->DrawLatex(xmin + TXTOFF[0], max * TXTSCALE, "Station 1");
          tltex[0]->DrawLatex(xmin + 48 + TXTOFF[1], max * TXTSCALE, "Station 2");

          tltex[0]->DrawLatex(xmin + 96 + TXTOFF[2], max * TXTSCALE, "Station 3");
          // also give info on non-scaled zero-supp. factors for the 3 stations
          posting.str("");
          posting << "Mean: " << setprecision(3) << staaverage[0] ;
          tltex[1]->DrawLatex(xmin + TXTOFF[0], min*TXTSCALE + 0.2, posting.str().c_str());
          posting.str("");
          posting << "Mean: " << setprecision(3) << staaverage[1] ;
          tltex[1]->DrawLatex(xmin + 48 + TXTOFF[1], min*TXTSCALE + 0.2, posting.str().c_str());
          posting.str("");
          posting << "Mean: " << setprecision(3) << staaverage[2] ;
          tltex[1]->DrawLatex(xmin + 96 + TXTOFF[2], min*TXTSCALE + 0.2, posting.str().c_str());
          // warning borders
          tbar[1]->DrawLine(xmin, YMINPLANES[i], xmax, YMINPLANES[i]);
          tbar[1]->DrawLine(xmin, YMAXPLANES[i], xmax, YMAXPLANES[i]);

          // should also print out who had the errors
          if (planesabovesize > 0)
            {
              outfile << " MutrMonDraw Arm " << CARM[i]
		      << " " << planesabove.size()
		      << " planes above set limit:" << endl;
              for (unsigned int ip = 0; ip < planesabove.size(); ip++)
                {
                  planesabove[ip].write(outfile);
                  outfile << "; ";
                }
              outfile << endl;
            }
          if (planesbelow.size() > 0)
            {
              outfile << " MutrMonDraw Arm " << CARM[i]
		      << " " << planesbelow.size()
		      << " planes below set limit:" << endl;
              for (unsigned int ip = 0; ip < planesbelow.size(); ip++)
                {
                  planesbelow[ip].write(outfile);
                  outfile << "; ";
                }
              outfile << endl;
            }
        }
      int fillcolor = 3; // green

      // 3rd plot: signal
      mutPad[i][2]->cd();
      mutPad[i][2]->SetLeftMargin(1.2);
      if (mutH1Signal[i] && mutHprofPktErr[i] &&
          //	  mutH1ClustAdc[i] && mutH1PeakAdc[i])
          mutH1PeakAdc[i])
        {
          if (mutH1Signal[i]->GetEntries() > 0)
            {
              mutPad[i][2]->SetLogy();
            }
          // set titles
          mutH1Signal[i]->SetStats(0);
          mutH1Signal[i]->GetXaxis()->SetTitle("ADC");
          mutH1Signal[i]->GetXaxis()->SetTitleSize(0.06);
          mutH1Signal[i]->GetXaxis()->SetTitleOffset(0.8);
          mutH1Signal[i]->GetYaxis()->SetTitle("Yield");
          mutH1Signal[i]->GetYaxis()->SetTitleSize(0.05);
          mutH1Signal[i]->GetYaxis()->SetTitleOffset(1.05);

          mutH1Signal[i]->SetMarkerStyle(8);
          mutH1Signal[i]->SetMarkerSize(MSIZE);
          mutH1Signal[i]->SetMinimum(1.0);
          mutH1Signal[i]->Draw("p");
          /*
            mutH1ClustAdc[i]->SetMarkerStyle(8); 
            mutH1ClustAdc[i]->SetMarkerColor(4); 
            mutH1ClustAdc[i]->SetMarkerSize(MSIZE);
            mutH1ClustAdc[i]->Draw("psame");
          */
          mutH1PeakAdc[i]->SetMarkerStyle(8);
          mutH1PeakAdc[i]->SetMarkerColor(4);
          mutH1PeakAdc[i]->SetMarkerSize(MSIZE);
          //	  mutH1PeakAdc[i]->Draw("psame");

          // make a copy and use that for finding maxima etc. in some
          // specified range
          TH1F *mutH1PeakAdcClone = (TH1F*)(mutH1PeakAdc[i]->Clone());
          TF1 *landaufit = new TF1("landaufit", "landau");
          mutH1PeakAdc[i]->Fit(landaufit, "", "psame",
			       FIT_RANGE_PEAKADC_MIN,
			       FIT_RANGE_PEAKADC_MAX);
          float clustermax = landaufit->GetParameter(1);

          double frame_ymax=mutH1Signal[i]->GetMaximum();
          frame_ymax=frame_ymax>=1.0 ? frame_ymax : 1.0;

          // put some information text here
          posting.str("");
          posting << "Cluster peak found at " << setprecision(3) << clustermax ;
          // test the value of the landau peak
          if (clustermax > PEAKMAX)
            {
              errorcode += 0x40;
              tltex[0]->SetTextColor(2);
            }
          else if (clustermax < PEAKMIN)
            {
              errorcode += 0x80;
              tltex[0]->SetTextColor(2);
            }
          tltex[0]->DrawLatex(0,pow(frame_ymax,0.10),posting.str().c_str());
          tltex[0]->SetTextColor(4);
          posting.str("");
          posting << "Single strip" ;

          tltex[2]->DrawLatex(1000,pow(frame_ymax,0.15),posting.str().c_str());
          // let's check the ratio between the Landau peak and the saturated
          // peak. If this is close to 1, we're in trouble..
          // Note that I'm hardcoding the number of bins to find saturation
          // between 150=1000 and 300=2500
          // which could be problematic if binning changes substantially
          mutH1PeakAdcClone->GetXaxis()->SetRange(150, 300);
          float saturatedheight = mutH1PeakAdcClone->GetMaximum();
          float clusterheight = landaufit->Eval(clustermax);
          delete mutH1PeakAdcClone;
          delete landaufit;

          int ratio = 0;
          if (saturatedheight > 0)
            {
              ratio = (int) (clusterheight / saturatedheight + 0.5);
            }
          outfile << " MutrMonDraw Arm " << CARM[i] << ": ClusterLandauPeak "
		  << clusterheight
		  << " ClusterSaturatedPeak "
		  << saturatedheight
		  << " Ratio " << ratio << endl;

          posting.str("");
          posting << "Landau / Saturated max ratio " << ratio ;

          tltex[0]->DrawLatex(0,pow(frame_ymax,0.05),posting.str().c_str());

          // xjiang 02/10/2011. Ignor spike at 1000 bin#51, for i==0, south arm.
           if (i == 0)
           {
             TH1F *mutH1SignalClone1 = (TH1F*)(mutH1Signal[i]->Clone());
             TH1F *mutH1SignalClone2 = (TH1F*)(mutH1Signal[i]->Clone());
             TH1F *mutH1SignalClone3 = (TH1F*)(mutH1Signal[i]->Clone());
             mutH1SignalClone1->GetXaxis()->SetRange(43,47);
             mutH1SignalClone2->GetXaxis()->SetRange(48,55);
             mutH1SignalClone3->GetXaxis()->SetRange(56,60);
             float xjheight1 = mutH1SignalClone1->GetMaximum();
             float xjheight2 = mutH1SignalClone2->GetMaximum();
             float xjheight3 = mutH1SignalClone3->GetMaximum();
             delete mutH1SignalClone1;
             delete mutH1SignalClone2;
             delete mutH1SignalClone3;

              if (xjheight2-2.0*(xjheight1+xjheight3)> 0)
              {
          tltex[1]->SetTextColor(2);
          tltex[1]->DrawLatex(1050,pow(frame_ymax,0.98),"ignore spike@1000");
          tltex[1]->SetTextColor(2);
          tltex[1]->DrawLatex(1050,pow(frame_ymax,0.93),"bad AMUADC pkt11007");
          tltex[1]->SetTextColor(2);
              }
            }
          // end of changes.  xjiang 02/10/2011

          // lets do the packet error check here also
          max = mutHprofPktErr[i]->GetMaximum();
          float nofflimits = 0;
          int packetid = 0;
          if (max > 0)
            { // we need to calculate how many bins/packets that are positive
              // i.e had errors
              axis = mutHprofPktErr[i]->GetXaxis();
              for (bin = axis->GetFirst(); bin <= axis->GetLast(); bin++)
                {
                  if (mutHprofPktErr[i]->GetBinContent(bin) > AMUERRFREQ)
                    {
                      packetid = bin + FIRSTPACKET[i] - 1;
                      outfile << " MutrMon::Draw AMU errors in packet "
			      << packetid << " has "
			      << mutHprofPktErr[i]->GetBinContent(bin)
			      << " errors " << endl;
                      //                      badfemiter = badfemset.find(packetid);
                      //                      if ( badfemiter == badfemset.end() )
                      //                        { // this packet is not in the bad set, so include it
                      //                          // in the error checking
                      //                          nofflimits++;
                      //                        }
                    }
                  nofflimits += mutHprofPktErr[i]->GetBinContent(bin);
                }
              nofflimits = nofflimits / NPACKETSARM[i];
            }
          if (nofflimits > AMUERRFREQ)
            {
              errorcode += 0x10;
              outfile << " MutrMon::Draw average AMU error "
		      << nofflimits << endl;
            }
        }
      // 4th plot: amplitude vs time
      mutPad[i][3]->cd();
      //pt[i*NPAVETEX + 0] = new TPaveText(0., 50., 11., 300., "br");
      //pt[i*NPAVETEX + 0] = new TPaveText(0.05, 0.1, 0.95, 0.7, "NDC");
      // xjiang 01/26/2011. Shrunk the error box size.
      pt[i*NPAVETEX + 0] = new TPaveText(0.20, 0.15, 0.85, 0.3, "NDC");
      //pt[i*NPAVETEX + 0] = new TPaveText(0.05, 0.1, 0.95, 0.7, "br");
      pt[i*NPAVETEX + 0]->SetFillColor(18);
      int nerrors = 0;
      if (mutHprofAmplTime[i])
        {
          // set titles
          mutHprofAmplTime[i]->SetStats(0);
          mutHprofAmplTime[i]->GetXaxis()->SetTitle("Time");
          mutHprofAmplTime[i]->GetXaxis()->SetTitleSize(0.06);
          mutHprofAmplTime[i]->GetXaxis()->SetTitleOffset(0.8);
          mutHprofAmplTime[i]->GetYaxis()->SetTitle("Amplitude");
          mutHprofAmplTime[i]->GetYaxis()->SetTitleSize(0.05);
          mutHprofAmplTime[i]->GetYaxis()->SetTitleOffset(1.05);
          // check where the maximum is. It should be 3rd sample bin : rdiff == 6, bin = 7
          // changed by xjiang 01/27/2011, to check nstr plot from binmax at 7, set ATLIMT=1.005.
          //binmax = mutHprofAmplTime[i]->GetMaximumBin();
          //max = mutHprofAmplTime[i]->GetMaximum();
          //if (binmax != 7 &&
          //    mutHprofAmplTime[i]->GetBinContent(binmax) >
          //    mutHprofAmplTime[i]->GetBinContent(7)*ATLIMIT )
          //  {
          //    errorcode += 0x20;
          //    mutHprofAmplTime[i]->SetLineColor(2);
          //  }
          // else
          //  {
          //    mutHprofAmplTime[i]->SetLineColor(4);
          //  }
          // mutHprofAmplTime[i]->SetLineWidth(2);
          // mutHprofAmplTime[i]->Draw();
	  // if (mutHprofAmplTime_nstr[i]){
	  //  mutHprofAmplTime_nstr[i]->SetLineWidth(2);
	  //  mutHprofAmplTime_nstr[i]->SetLineColor(3);
	  //  mutHprofAmplTime_nstr[i]->Draw("same");
	  // }
          max = mutHprofAmplTime[i]->GetMaximum();
          mutHprofAmplTime[i]->SetLineColor(4);
          mutHprofAmplTime[i]->SetLineWidth(2);
          mutHprofAmplTime[i]->Draw();
          // xj again, 03/10/13 for run13, timing adjusted to delay-6 (instead of delay7 in earlier runs)
          // the peak should be on the right-most bin (binmax=8), according to Itaru
	  if (mutHprofAmplTime_nstr[i]){
             binmax = mutHprofAmplTime_nstr[i]->GetMaximumBin();
             // cout << binmax << endl;
             if (binmax != 8 &&
                mutHprofAmplTime_nstr[i]->GetBinContent(binmax) >
                mutHprofAmplTime_nstr[i]->GetBinContent(8)*ATLIMIT )
             {
                errorcode += 0x20;
                mutHprofAmplTime_nstr[i]->SetLineColor(2);
             }
             else
             {
                mutHprofAmplTime_nstr[i]->SetLineColor(3);
             }
	    mutHprofAmplTime_nstr[i]->SetLineWidth(2);
	    mutHprofAmplTime_nstr[i]->Draw("same");
	  } 
	  // end of change xjiang 3/10/2013.
          // end of change. xjiang 01/27/2011.

          // let's also summarize the status and print all error messages here
          if (errorcode == 0)
            {
              fillcolor = 3;
              posting.str("");
              posting << "MUTR." << CARM[i] << ": All OK! Carry on" ;
            }
          else
            {
              if (eventNum < MUTRMINEVENTS)
                {
                  fillcolor = 5; // yellow, slight warning
                  posting.str("");
                  posting << "MUTR." << CARM[i] << ": Too few events. Draw again" ;
                }
              //              else if (errorcode == 0x10)
              //                {
              //                  fillcolor = AMUERRCOLOR;
              //                  posting.str("");
              //                  posting << "AMU - Try FEED. If persists call expert!" ;
              //                }
              else
                {
                  fillcolor = 2;
                  int errorbit = 0x1;
                  int errorentry = 0;
                  // cout << "errorcode" << hex << errorcode << dec << endl;
                  while (errorbit < 0x100)
                    {
                      if ( (errorcode & errorbit) == errorbit)
                        {
                          ostringstream tmppost;
                  // xj
                  //        cout << "errorcode" << hex << errorcode << dec << endl;
                          tmppost << ERRORMSG[errorentry] ;
                          pt[i*NPAVETEX + 0]->AddText(tmppost.str().c_str());
                          pt[i*NPAVETEX + 0]->SetTextColor(fillcolor);
                          pt[i*NPAVETEX + 0]->SetTextSize(TXTSIZE*0.75);
                          nerrors++;
                        }
                      errorbit *= 2;
                      errorentry++;
                    }
                  pt[i*NPAVETEX + 0]->AddText("If persists, call expert");

		  if(RunType!="PHYSICS" && RunType!="ZEROFIELD"){
		    posting.str("");
		    posting << "(Note: this is a " << RunType
			    << " run.Ask SL)";
		    pt[i*NPAVETEX + 0]->AddText(posting.str().c_str());
		  }

                  posting.str("");
                  posting << "MUTR." << CARM[i] << ": Nerrors " << nerrors
			  << ", Code " << errorcode ;
                }
            }
        }
      if (nerrors != 0)
        {
         pt[i*NPAVETEX + 0]->Draw();
        }
      mutCanv[i]->Update();
      mutCanv[i]->cd();
      // cout << " posting " << posting << endl;

      pt[i*NPAVETEX + 1] = new TPaveText(0.46, 0.475, 0.99, 0.525, "br");
      pt[i*NPAVETEX + 1]->SetFillColor(fillcolor);
      pt[i*NPAVETEX + 1]->AddText(posting.str().c_str());
      pt[i*NPAVETEX + 1]->SetTextColor(1);
      pt[i*NPAVETEX + 1]->SetTextSize(TXTSIZE*0.7);
      pt[i*NPAVETEX + 1]->Draw();

      // run and time info; again..
      posting.str("");
      posting << "Run# " << Onlmonclient->RunNumber() << "   Nevt:" << eventNum
	      << "   Date:" << localT->tm_mon + 1 << "/" << localT->tm_mday
	      << "/" << localT->tm_year + 1900 << " " << localT->tm_hour
	      << ":" << localT->tm_min << endl;
      pt[i*NPAVETEX + 2] = new TPaveText(0.02, 0.475, 0.45, 0.525, "br");
      pt[i*NPAVETEX + 2]->SetFillColor(18);
      pt[i*NPAVETEX + 2]->AddText(posting.str().c_str());
      pt[i*NPAVETEX + 2]->SetTextColor(4);
      pt[i*NPAVETEX + 2]->SetTextSize(TXTSIZE*0.45);
      pt[i*NPAVETEX + 2]->Draw();
      mutCanv[i]->Update();
    }

  // go back to original settings
  gStyle = oldStyle;
  delete mutStyle;

  // since cout messages now went to the outfile only, we'll just echo
  // the contents of that file here instead
  outfile.close();
  system("cat mutrclientlog.txt");
  return 0;
}

int MutrMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }
  for (int i = 0; i < NARMS; i++)
    {
      ostringstream filename;
      filename << ThisName << "_" << i
	       << "_" << cl->RunNumber() << ".ps";
      string savefile = filename.str();
      mutCanv[i]->Print(savefile.c_str());
    }
  return 0;
}

int MutrMonDraw::MakeHtml(const char *what)
{
  int iret = Draw(what);
  if (iret)
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  const char* names[] =
    { "South", "North"
    };

  // "Shift crew" plots.
  for ( int i = 0; i < NARMS; ++i )
    {
      ostringstream si;
      si << i;
      string ofile = cl->htmlRegisterPage(*this, names[i], si.str(), "png");
      cl->CanvasToPng(mutCanv[i], ofile);
    }


  // Expert part : first the log file.
  string ofile = cl->htmlRegisterPage(*this, "Expert/Log file",
                                      "clientlog", "txt");

  string clientlogfile = "mutrclientlog.txt";
  ostringstream cmd;

  cmd << "mv " << clientlogfile << " " << ofile;
  gSystem->Exec(cmd.str().c_str());

  // continue with the expert plotting
  TH1 *mutH1PeakAdcSta[NARMS][NSTATIONS];
  TH1 *mutH1PeakAdcStaOct[NARMS][NSTATIONS][NOCTANTS];

  TF1 landaufit("landaufit", "landau");

  // First, the info for each station in each arm
  for (int i = 0; i < NARMS; i++)
    {
      ostringstream dir, name;
      name << names[i] << "_Sta_Peak_ADC" ;
      dir << "Expert/" << names[i] << " Peak ADC Station";
      string ofile = cl->htmlRegisterPage(*this, dir.str(), name.str(), "html");
      ofstream outfile(ofile.c_str());
      outfile << "<H1>Run number " << cl->RunNumber() << "</H1>" << endl;
      outfile << "<TABLE WIDTH=\"100%\">" << endl;

      for (int ista = 0; ista < NSTATIONS; ista++)
        {
          ostringstream id;
          id.str("");
          id << "mutH1PeakAdcSta" << i << ista ;
          mutH1PeakAdcSta[i][ista] = cl->getHisto(id.str().c_str());

          ostringstream posting;

          if (mutH1PeakAdcSta[i][ista]->GetEntries() > 0)
            {
              mutH1PeakAdcSta[i][ista]->Fit(&landaufit, "", "psame",
					    FIT_RANGE_PEAKADC_MIN,
					    FIT_RANGE_PEAKADC_MAX);
              double clustermax = landaufit.GetParameter(1);

              ostringstream basefilename;
              basefilename << "PeakAdcArm" << i << "Sta" << ista;

              string fullfilename, filename;
              cl->htmlNamer(*this, basefilename.str(), "png",
                            fullfilename, filename);

              cl->HistoToPng(mutH1PeakAdcSta[i][ista], fullfilename.c_str());

              outfile << "<TR><TD WIDTH=\"50%\"><a href=\"" << filename
		      << "\"><IMG BORDER=\"0\" SRC=\""
		      << filename
		      << "\"></a>" << endl;

              // put some information text here
              posting.str("");
              posting << "<TD>Cluster peak " << clustermax;
            }
          else
            {
              posting.str("");
              posting << "<TR><TD>No entries in histogram for arm " << i
		      << " sta <TD>&nbps;" << ista;
            }
          outfile << posting.str() << endl;
        }
      outfile << "</TABLE>" << endl;
      outfile.close();
    }

  // Second, the info for each octant each station in each arm
  for (int i = 0; i < NARMS; i++)
    {
      ostringstream dir, name;
      name << names[i] << "_Sta_Oct_Peak_ADC" ;
      dir << "Expert/" << names[i] << " Peak ADC Station Octant";
      string ofile = cl->htmlRegisterPage(*this, dir.str(), name.str(), "html");
      ofstream outfile(ofile.c_str());
      outfile << "<H1>Run number " << cl->RunNumber() << "</H1>" << endl;
      outfile << "<TABLE WIDTH=\"100%\">" << endl;

      for (int ista = 0; ista < NSTATIONS; ista++)
        {
          for (int ioct = 0; ioct < NOCTANTS; ioct++)
            {
              ostringstream id;
              id << "mutH1PeakAdcStaOct" << i << ista << ioct ;
              mutH1PeakAdcStaOct[i][ista][ioct] = cl->getHisto(id.str().c_str());
              assert(mutH1PeakAdcStaOct[i][ista][ioct] != 0);

              ostringstream posting;

              if (mutH1PeakAdcStaOct[i][ista][ioct]->GetEntries() > 0)
                {
                  mutH1PeakAdcStaOct[i][ista][ioct]->Fit(&landaufit,"","psame",
							 FIT_RANGE_PEAKADC_MIN,
							 FIT_RANGE_PEAKADC_MAX);
                  double clustermax = landaufit.GetParameter(1);

                  ostringstream basefilename;
                  basefilename << "PeakAdcArm" << i << "Sta" << ista << "Oct" << ioct;

                  string fullfilename, filename;
                  cl->htmlNamer(*this, basefilename.str(), "png",
                                fullfilename, filename);

                  cl->HistoToPng(mutH1PeakAdcStaOct[i][ista][ioct], fullfilename.c_str());

                  outfile << "<TR><TD WIDTH=\"50%\"><a href=\"" << filename
			  << "\"><IMG BORDER=\"0\" SRC=\""
			  << filename
			  << "\"></a>" << endl;

                  // put some information text here
                  posting.str("");
                  posting << "<TD>Cluster peak " << clustermax;
                }
              else
                {
                  posting.str("");
                  posting << "<TR><TD>No entries in histogram for arm " << i
			  << " sta <TD>&nbps;" << ista;
                }
              outfile << posting.str() << endl;
            }
        }
      outfile << "</TABLE>" << endl;
      outfile.close();
    }
  cl->SaveLogFile(*this);

  return 0;
}

