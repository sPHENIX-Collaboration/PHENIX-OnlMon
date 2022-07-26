#include "PktSizeMonDraw.h"
#include "PktSizeDBodbc.h"
#include "PktSizeCommon.h"
#include <RunDBodbc.h>
#include <OnlMonClient.h>

#include <phool.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraph.h>
#include <TH2.h>
#include <TLine.h>
#include <TMarker.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>


using namespace std;

// make history for currently noisy packets, not packets whose size has
// changed significantly with respect to previous runs
#define CURRENTNOISY

static int bytelimit = 220;
static float MAXSIZEDISP = 1000.;
static float MAXPKTDISP = 26000.;
static float GRANTXTOFFSET = 20.;

PktSizeMonDraw::PktSizeMonDraw(const char *name): OnlMonDraw(name)
{
  memset(transparent, 0, sizeof(transparent));
  memset(TC, 0, sizeof(TC));
  memset(Pad, 0, sizeof(Pad));
  db = 0;
  rd = 0;
  lastrun = 0;
  return ;
}

PktSizeMonDraw::~PktSizeMonDraw()
{
  packetmap.clear();
  knownbig.clear();
  activepackets.clear();
  delete rd;
  delete db;
  return ;
}

int
PktSizeMonDraw::Init()
{
  PktSizeCommon::fillgranules(granulepacketlimits);
  db = new PktSizeDBodbc(ThisName);
  rd = new RunDBodbc();
  return 0;
}

int
PktSizeMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (!strcmp(name, "PktSizeMon0"))
    {
      // xpos (-1) negative: do not draw menu bar
      TC[0] = new TCanvas(name, "Packet Size Monitor", -1, 0, xsize / 2 , ysize);
      gSystem->ProcessEvents();
      Pad[0] = new TPad("pktpad1", "who needs this?", 0.1, 0.05, 0.9, 0.9, 0);
      Pad[0]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
    }
  else if (!strcmp(name, "PktSizeMon1"))
    {
      // xpos negative: do not draw menu bar
      TC[1] = new TCanvas(name, "Packet Size History", -xsize / 2, 0, xsize / 2, ysize);
      gSystem->ProcessEvents();
      Pad[1] = new TPad("pktpad1", "who needs this?", 0.1, 0.05, 0.9, 0.9, 0);
      Pad[1]->Draw();
      // this one is used to plot the run number on the canvas
      transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
    }
  return 0;
}

int
PktSizeMonDraw::Draw(const char *what)
{
  int iret = 0;
  int idraw = 0;
  if (!strcmp(what, "ALL") || !strcmp(what, "FIRST"))
    {
      iret += DrawFirst(what);
      idraw ++;
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "HISTORY"))
    {
      iret += DrawHistory(what);
      idraw ++;
    }
  if (!idraw)
    {
      cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
      iret = -1;
    }
  return iret;
}


int PktSizeMonDraw::DrawFirst(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (! gROOT->FindObject("PktSizeMon0"))
    {
      MakeCanvas("PktSizeMon0");
    }
  TC[0]->Clear("D");
  TH1 *pktsize_hist = cl->getHisto("pktsize_hist");
  if (!pktsize_hist)
    {
      DrawDeadServer(transparent[0]);
      TC[0]->Update();;
      return -1;
    }
  FillPacketMap(pktsize_hist);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  int runnumber = cl->RunNumber();
  runnostream << "Packet Size Display Run " << runnumber
	      << ", Time: " << ctime(&evttime);
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 0.98, runnostream.str().c_str());
  runnostream.str("");
  runnostream << "Based on " << pktsize_hist->GetBinContent(0) << " Events";
  PrintRun.DrawText(0.5, 0.94, runnostream.str().c_str());
  Pad[0]->cd();
  TH2 *htmp = new TH2F("pktsize", "", 2, 0, MAXPKTDISP, 2, 0, MAXSIZEDISP);
  htmp->SetStats(kFALSE);
  htmp->GetXaxis()->SetNoExponent();
  htmp->GetXaxis()->SetTitle("Packet Id");
  htmp->DrawCopy();
  delete htmp;
  TLine tl;
  tl.DrawLine(0, bytelimit, MAXPKTDISP, bytelimit);
  TMarker marker;
  TText outtxt;
  outtxt.SetTextFont(62);
  outtxt.SetTextSize(0.02);
  outtxt.SetTextColor(1);
  outtxt.SetTextAlign(13);
  TLine ta;
  TMarker ArrowHead;
  ArrowHead.SetMarkerSize(1.5);
  ArrowHead.SetMarkerColor(2);
  ArrowHead.SetMarkerStyle(26);

  ta.SetLineColor(2);
  ta.SetLineStyle(1);
  map<int, map<unsigned int, float> >::const_iterator iter;
  map<unsigned int, float>::const_iterator piter;
  iter = packetmap.find(runnumber);
  float ypos = MAXSIZEDISP-100.;
  int foundnoisypacket = 0;
  if (iter != packetmap.end())
    {

      marker.SetMarkerStyle(22);
      marker.SetMarkerSize(2);
      marker.SetMarkerColor(2);
      for (piter = iter->second.begin(); piter != iter->second.end(); piter++)
        {
          if (piter->second > bytelimit)
            {
              if (!IsKnownBig(piter->first, piter->second))
                {
		  float linetopend = 9;
                  marker.DrawMarker((double)(piter->first), piter->second);
                  ostringstream pktid;
                  pktid << setprecision(3) << piter->first << "(" << piter->second << " w)" ;
                  int xpos = ((piter->first) / 1000) * 1000;
		  if (xpos == 21000)
		    {
                      xpos = ((piter->first) / 100) * 100;
		      if (xpos == 21300)
			{
			  linetopend = 50;
			}
		    }
		  else if (xpos == 7000)
		    {
                      xpos = ((piter->first) / 100) * 100;
		      if (xpos == 7100)
			{
			  linetopend = 50;
			}
		    }
		  else if (xpos == 24000)
		    {
                      xpos = ((piter->first) / 100) * 100;
		      if (xpos == 24100)
			{
			  linetopend = 50;
			}
		    }
		  float arrowoffset = linetopend+6;
                  ta.DrawLine(xpos, 0, xpos, MAXSIZEDISP+linetopend);
                  ArrowHead.DrawMarker(xpos, MAXSIZEDISP+arrowoffset);
                  outtxt.DrawText(xpos, ypos, pktid.str().c_str());
                  ypos -= 20;
                  foundnoisypacket = 1;
                }
            }
        }
    }
  if (!foundnoisypacket)
    {
      outtxt.SetNDC();
      outtxt.SetTextSize(0.05);
      outtxt.SetTextColor(3);
      outtxt.SetTextAlign(22);
      outtxt.DrawText(0.5, 0.7, "Congratulations");
      outtxt.DrawText(0.5, 0.5, "No");
      outtxt.DrawText(0.5, 0.3, "Noisy Packets");
    }
  TText granlabel;
  granlabel.SetTextAlign(12);
  granlabel.SetTextAngle(90);
  map<string, pair<unsigned int, unsigned int> >::const_iterator graniter;
  for (graniter = granulepacketlimits.begin(); graniter != granulepacketlimits.end(); graniter++)
    {
      granlabel.SetTextSize(0.03);
      int xpos = ((graniter->second.first) / 1000) * 1000;
      if (xpos != 14000)
        {
	  if (xpos == 24000)
	    {
              granlabel.SetTextSize(0.015);
              xpos = ((graniter->second.first) / 100) * 100;
	      if (xpos == 24000)
		{
		  granlabel.DrawText(xpos, MAXSIZEDISP + 20, "VTXP");
		}
	      else
		{
		  granlabel.DrawText(xpos, MAXSIZEDISP + 70, "VTXS");
		}
	    }
	  else if (xpos == 7000)
	    {
              xpos = ((graniter->second.first) / 100) * 100;
              granlabel.SetTextSize(0.015);
	      if (xpos == 7000)
		{
		  granlabel.DrawText(xpos, MAXSIZEDISP + 20, "TOFE");
		}
	      else
		{
		  granlabel.DrawText(xpos, MAXSIZEDISP + 70, "TOFW");
		}
	    }
	  else if (xpos == 21000)
	    {
              xpos = ((graniter->second.first) / 100) * 100;
              granlabel.SetTextSize(0.015);
	      if (xpos == 21100)
		{
		  granlabel.DrawText(xpos, MAXSIZEDISP + 20, "MPC");
		}
	      else
		{
		  granlabel.DrawText(xpos, MAXSIZEDISP + 70, "MPCEX");
		}

	    }
	  else
	    {
	      granlabel.DrawText(xpos, MAXSIZEDISP + GRANTXTOFFSET, (graniter->first).c_str());
	    }
        }
    }
  TC[0]->Update();
  return 0;
}

int
PktSizeMonDraw::FillPacketMap(const TH1 *pktsize_hist)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int runnumber = cl->RunNumber();
  map<int, map<unsigned int, float> >::iterator iter;
  iter = packetmap.find(runnumber);
  if (iter != packetmap.end())
    {
      iter->second.clear();
    }
  else
    {
      map<unsigned int, float> newmap;
      packetmap[runnumber] = newmap;
      iter = packetmap.find(runnumber);
    }
  for (int i = 1; i <= pktsize_hist->GetNbinsX(); i++)
    {
      unsigned int packetid = (unsigned int) pktsize_hist->GetBinError(i);
      iter->second[packetid] = pktsize_hist->GetBinContent(i);
    }
  ExtractActivePackets(iter->second); // make set with all currently active packets
  return 0;
}

int
PktSizeMonDraw::DrawDeadServer(TPad *transparent)
{
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "PKTSIZE MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}


int
PktSizeMonDraw::MakePS(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  ostringstream filename;
  int iret = Draw(what);
  if (iret) // on error no ps files please
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

int
PktSizeMonDraw::MakeHtml(const char *what)
{
  // bbc
  AddKnownBig(1002,225);
  AddKnownBig(1003,225);
  // mpc
  for (int i=21101; i<=21106; i++)
    {
       AddKnownBig(i,300);
    }
  // mpc-ex
  // north
  for (int i=21301; i<=21308; i++)
    {
      AddKnownBig(i,600);
    }
  // south
  for (int i=21351; i<=21358; i++)
    {
       AddKnownBig(i,600);
    }

  // zdc
  AddKnownBig(13001,175);
  // vtxp
  for (int i=24001; i<24061;i++)
    {
       AddKnownBig(i,400);
    }
  // vtxs
  for (int i=24101; i<24141;i++)
    {
       AddKnownBig(i,400);
    }
  int iret = Draw(what);
  if (iret) // on error no html output please
    {
      return iret;
    }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  string pngfile = cl->htmlRegisterPage(*this, "PktSize", "1", "png");
  cl->CanvasToPng(TC[0], pngfile);
  pngfile = cl->htmlRegisterPage(*this, "History", "2", "png");
  cl->CanvasToPng(TC[1], pngfile);

  cl->SaveLogFile(*this);

  return 0;
}

int
PktSizeMonDraw::DrawHistory(const char *what)
{
  if (! gROOT->FindObject("PktSizeMon1"))
    {
      MakeCanvas("PktSizeMon1");
    }
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *pktsize_hist = cl->getHisto("pktsize_hist");
  if (!pktsize_hist)
    {
      DrawDeadServer(transparent[1]);
      TC[1]->Update();
      return -1;
    }
  // prevent runnumber=0/-1 from startup to screw things up
  if (cl->RunNumber() <= 0)
    {
      return 0;
    }
  if (!lastrun)
    {
      lastrun = cl->RunNumber();
      loadpreviousruns();
    }
  if (lastrun != cl->RunNumber())
    {
      // plot only physics run history
      // if the last run was physics, read it from DB and
      // enter into list
      // if not, remove the packet entries from this run from internal list
      if (rd->RunType(lastrun) == "PHYSICS")
        {
          ReplaceRunFromDB(lastrun);
          CleanOldRuns(20);
        }
      else
        {
          RemoveRun(lastrun);
        }
      lastrun = cl->RunNumber();
    }
  MakeNoisyCandidates();
  int firstrun = *(runlist.begin());
  // somehow I see run=0, no idea where this comes from
  // this led to infinite (or very long) loops in the erase
//   if ( firstrun < 300000)
//     {
//       Print("RUNS");
//       runlist.erase(runlist.begin());
//       firstrun = *(runlist.begin());
//     }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  int runnumber = cl->RunNumber();
  ostringstream runnostream;
  time_t evttime = cl->EventTime("CURRENT");
  runnostream << "Packet Size History Run " << runnumber
	      << ", Time: " << ctime(&evttime);
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 0.98, runnostream.str().c_str());
  Pad[1]->cd();
  TH2 *htmp = new TH2F("noisepkts", "", 2, firstrun - 2, lastrun + 2, 2, 0, MAXSIZEDISP);
  htmp->SetStats(kFALSE);
  htmp->GetXaxis()->SetNoExponent();
  htmp->GetXaxis()->SetTitle("Run");
  htmp->GetXaxis()->SetLabelSize(0.025);
  htmp->DrawCopy();
  delete htmp;
  TGraph gr;
  TMarker tr;
  TText tx;
  set<unsigned int>::const_iterator iter;
  int icnt = 0;
  for (iter = noisypackets.begin(); iter != noisypackets.end(); iter++)
    {
      PlotNoisy(gr, tr, tx, *iter, icnt);
      icnt++;
    }
  TLine tl;
  tl.DrawLine(firstrun - 2, bytelimit, lastrun + 2, bytelimit);
  TC[1]->Update();

  return 0;
}

int
PktSizeMonDraw::loadpreviousruns(const int nruns)
{
  set<int>::const_iterator iter;
  rd->GetRunNumbers(runlist, "PHYSICS", nruns, lastrun);

  for (iter = runlist.begin(); iter != runlist.end(); iter++)
    {
      map<unsigned int, float> pkts;
      FillRunPacketList(pkts, *iter);
      packetmap[*iter] = pkts;
    }
  return 0;
}

int
PktSizeMonDraw::AddKnownBig(const unsigned int packetid, const float maxsize)
{
  knownbig[packetid] = maxsize;
  return 0;
}

int
PktSizeMonDraw::IsKnownBig(const unsigned int packetid, const float size)
{
  map<unsigned int, float>::const_iterator iter = knownbig.find(packetid);
  if (iter != knownbig.end())
    {
      if (iter->second > size)
        {
          return 1;
        }
    }
  return 0;
}

int
PktSizeMonDraw::ReplaceRunFromDB(const int runno)
{
  map<unsigned int, float> pkts;
  FillRunPacketList(pkts, runno);
  packetmap[runno] = pkts;
  return 0;
}


int
PktSizeMonDraw::FillRunPacketList(map<unsigned int, float> &pkts, const int runnumber)
{
  map<string, std::pair<unsigned int, unsigned int> >::const_iterator graniter;
  for (graniter = granulepacketlimits.begin(); graniter != granulepacketlimits.end(); graniter++)
    {
      db->GetPacketContent(pkts, runnumber, graniter->first);
    }
  return 0;
}

int
PktSizeMonDraw::CleanOldRuns(const unsigned int maxrun)
{
  if (verbosity > 0)
    {
      cout << "Size before cleanup: " << packetmap.size() << endl;
    }
  while (packetmap.size() > maxrun + 1) // current run is in this map -> maxrun+1
    {
      int runno = (packetmap.begin())->first;
      runlist.erase(runno);
      packetmap.erase(packetmap.begin());
    }
  if (verbosity > 0)
    {
      cout << "Size after cleanup: " << packetmap.size() << endl;
    }
  return 0;
}

int
PktSizeMonDraw::RemoveRun(const int runno)
{
  map<int, map<unsigned int, float> >::iterator iter;
  iter = packetmap.find(runno);
  if (iter != packetmap.end())
    {
      packetmap.erase(iter);
      runlist.erase(runno);
    }
  else
    {
      cout << "Could not find run " << runno << " in list of runs" << endl;
    }
  return 0;
}


void
PktSizeMonDraw::Print(const string &what) const
{
  if (what == "PACKETS" || what == "ALL")
    {
      map<int, map<unsigned int, float> >::const_iterator iter;
      map<unsigned int, float>::const_iterator piter;
      for (iter = packetmap.begin(); iter != packetmap.end(); iter++)
	{
	  for (piter = iter->second.begin(); piter != iter->second.end(); piter++)
	    {
	      cout << "Run " << iter->first
		   << ", packetid: " << piter->first
		   << ", size: " << piter->second
		   << endl;
	    }
	}
    }
  if (what == "RUNS" || what == "ALL")
    {
      set<int>::const_iterator riter;
      cout << "List of runs in Run set: " << endl;
      for (riter = runlist.begin(); riter != runlist.end(); riter++)
	{
	  cout << "Run " << *riter << endl;
	}
    }
  return ;
}

int
PktSizeMonDraw::ExtractActivePackets(const map<unsigned int, float> &packetsize)
{
  map<unsigned int, float>::const_iterator piter;
  for (piter = packetsize.begin(); piter != packetsize.end(); piter++)
    {
      activepackets.insert(piter->first);
    }
  return 0;
}

int
PktSizeMonDraw::MakeNoisyCandidates()
{
  noisypackets.clear();
#ifdef CURRENTNOISY
  OnlMonClient *cl = OnlMonClient::instance();
  int runnumber = cl->RunNumber();
  map<int, map<unsigned int, float> >::const_iterator iter;
  map<unsigned int, float>::const_iterator piter;
  iter = packetmap.find(runnumber);
  if (iter != packetmap.end())
    {
      for (piter = iter->second.begin(); piter != iter->second.end(); piter++)
        {
          if (piter->second > bytelimit)
            {
              if (!IsKnownBig(piter->first, piter->second))
                {
                  noisypackets.insert(piter->first);
                }
            }
        }
    }

#else
  set<unsigned int>::const_iterator piter;
  map<int, map<unsigned int, float> >::const_iterator siter;
  map<unsigned int, float>::const_iterator psizeiter;
  for (piter = activepackets.begin(); piter != activepackets.end(); piter++)
    {
      float size = -1;
      for (siter = packetmap.begin(); siter != packetmap.end(); siter++)
        {
          psizeiter = siter->second.find(*piter);
          if (psizeiter != siter->second.end())
            {
              if (size < 0)
                {

                  size = psizeiter->second;
                }
              else
                {
                  if (psizeiter->second > 200 && fabs(psizeiter->second - size) > size / 5.)
                    {
                      if (verbosity > 0)
                        {
                          cout << "Adding noisy packet " << *piter
                               << " old size " << size
                               << " current size " << psizeiter->second
                               << " current Run " << siter->first
                               << endl;
                        }
                      noisypackets.insert(*piter);
                    }
                  size = psizeiter->second;
                }
            }
        }
    }
#endif
  return noisypackets.size();
}

int
PktSizeMonDraw::PlotNoisy(TGraph &gr, TMarker &tr, TText &tx, const unsigned int ipkt, const unsigned int icnt)
{
  int imarker = 20;
  int icol = 1;
  map<int, map<unsigned int, float> >::const_iterator siter;
  map<unsigned int, float>::const_iterator psizeiter;
  vector<double> runno, meansize;

  for (siter = packetmap.begin(); siter != packetmap.end(); siter++)
    {
      psizeiter = siter->second.find(ipkt);
      if (psizeiter != siter->second.end())
        {
          runno.push_back((double) siter->first);
          meansize.push_back(psizeiter->second);
        }
    }
  double *x = new double[runno.size()];
  double *y = new double[runno.size()];
  for (unsigned int i = 0; i < runno.size(); i++)
    {
      x[i] = runno[i];
      y[i] = meansize[i];
    }
  icol += icnt - (icnt / 9) * 9;
  imarker += icnt / 9;
  gr.SetMarkerStyle(imarker);
  gr.SetMarkerColor(icol);
  gr.SetLineColor(icol);
  gr.SetMarkerSize(1);
  gr.DrawGraph(runno.size(), x, y, "LP");
  tr.SetNDC();
  tr.SetMarkerStyle(imarker);
  tr.SetMarkerColor(icol);
  tr.DrawMarker(x[0], (MAXSIZEDISP -100) - 20*icnt);
  ostringstream pktstring;
  pktstring << ipkt;
  tx.SetTextColor(icol);
  tx.SetTextSize(0.03);
  double txtXcoord = x[0] + (lastrun - x[0]) / 70;
  tx.SetTextAlign(12);
  tx.DrawText(txtXcoord, (MAXSIZEDISP -100) - 20*icnt, pktstring.str().c_str());
  delete [] x;
  delete [] y;
  return 0;
}
