#include "VtxsMonDraw.h"
#include "GranMonDraw.h"
#include "GranuleMonDefs.h"
#include "DaqMonDraw.h"

#include <OnlMonClient.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

using namespace std;

#define COLBAD 2
#define COLGOOD 3
#define XLABELSIZE 0.09
#define YLABELSIZE 0.09

static const int NHIST1 = 14;
static const int NHIST2 = 24;

VtxsMonDraw::VtxsMonDraw(const char * granule):
  GranMonDraw(granule),
  currentrun(0),
  suppress_masked(1)
{
  memset(TC,0,sizeof(TC));
  memset(transparent,0,sizeof(transparent));
  memset(Pad0,0,sizeof(Pad0));
  memset(Pad1,0,sizeof(Pad1));
  memset(Pad2,0,sizeof(Pad2));
  memset(Pad3,0,sizeof(Pad3));
  memset(rccmasked,0,sizeof(rccmasked));
  memset(hybridmask,0,sizeof(hybridmask));
}

int
VtxsMonDraw::DrawThisSubsystem(const char *what)
{
  ExtractMaskedChannels(); // 
  if (!strcmp(what,"RCCBLK") || !strcmp(what,"ALL") )
    {
      DrawRccBclk(what);
    }
  if (!strcmp(what,"HYBRID") || !strcmp(what,"ALL") )
    {
      DrawHybrid(what);
    }
  if (!strcmp(what,"INVALIDCELLID0") || !strcmp(what,"ALL") )
     {
       DrawInvalidCellId(0,what);
     }
  if (!strcmp(what,"INVALIDCELLID1") || !strcmp(what,"ALL") )
     {
       DrawInvalidCellId(1,what);
     }
  if (!strcmp(what,"INVALIDCELLID2") || !strcmp(what,"ALL") )
     {
       DrawInvalidCellId(2,what);
     }
  if (!strcmp(what,"CELLID0") || !strcmp(what,"ALL") )
     {
       DrawBadCellId(0,what);
     }
  if (!strcmp(what,"CELLID1") || !strcmp(what,"ALL") )
     {
       DrawBadCellId(1,what);
     }
  if (!strcmp(what,"CELLID2") || !strcmp(what,"ALL") )
     {
       DrawBadCellId(2,what);
     }
  if (!strcmp(what, "MASK"))
    {
      suppress_masked = 1;
    }
  if (!strcmp(what, "UMASK"))
    {
      suppress_masked = 0;
    }
  return 0;
}

int
VtxsMonDraw::DrawRccBclk(const char *what)
{
  if (! gROOT->FindObject("DaqVtxsMon0"))
    {
      MakeCanvas("DaqVtxsMon0");
    }
  TC[0]->cd();
  TC[0]->Clear("D");
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monivars = "Daq_" + ThisName + "AllVariables";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string subsyspkts = "Daq_" + ThisName + "SubsysStatus";
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monpacket = cl->getHisto(monpkts.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *subsyspacket = cl->getHisto(subsyspkts.c_str());
  if (! monpacket)
    {
      DaqMonDraw *daqmon = DaqMonDraw::instance();
      daqmon->DrawDeadServer(transparent[0]);
      TC[0]->Update();
      return -1;
    }


  int NPacketThisGranule = monpacket->GetNbinsX();
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  int packetmin = (int) monpacket->GetBinContent(1);
  int packetmax = (int) monpacket->GetBinContent(NPacketThisGranule);
  TH1 *histo[NHIST1];
  string ldtb_timeout = "Daq_"  + ThisName + "ldtbtimeout";
  histo[0] = MakeSnglHisto(ldtb_timeout, packetmin, packetmax);
  string no_rcc = "Daq_"  + ThisName + "norcc";
  histo[1]  = MakeSnglHisto(no_rcc, packetmin, packetmax);
  string rcc_0 = "Daq_"  + ThisName + "rcc_0";
  histo[2]  = MakeSnglHisto(rcc_0, packetmin, packetmax);
  string rcc_1 = "Daq_"  + ThisName + "rcc_1";
  histo[3]  = MakeSnglHisto(rcc_1, packetmin, packetmax);
  string rcc_2 = "Daq_"  + ThisName + "rcc_2";
  histo[4]  = MakeSnglHisto(rcc_2, packetmin, packetmax);
  string rcc_3 = "Daq_"  + ThisName + "rcc_3";
  histo[5]  = MakeSnglHisto(rcc_3, packetmin, packetmax);
  string rcc_4 = "Daq_"  + ThisName + "rcc_4";
  histo[6]  = MakeSnglHisto(rcc_4, packetmin, packetmax);
  string rcc_5 = "Daq_"  + ThisName + "rcc_5";
  histo[7]  = MakeSnglHisto(rcc_5, packetmin, packetmax);
  string bclk_0 = "Daq_"  + ThisName + "bclk_0";
  histo[8]  = MakeSnglHisto(bclk_0, packetmin, packetmax);
  string bclk_1 = "Daq_"  + ThisName + "bclk_1";
  histo[9]  = MakeSnglHisto(bclk_1, packetmin, packetmax);
  string bclk_2 = "Daq_"  + ThisName + "bclk_2";
  histo[10]  = MakeSnglHisto(bclk_2, packetmin, packetmax);
  string bclk_3 = "Daq_"  + ThisName + "bclk_3";
  histo[11]  = MakeSnglHisto(bclk_3, packetmin, packetmax);
  string bclk_4 = "Daq_"  + ThisName + "bclk_4";
  histo[12]  = MakeSnglHisto(bclk_4, packetmin, packetmax);
  string bclk_5 = "Daq_"  + ThisName + "bclk_5";
  histo[13]  = MakeSnglHisto(bclk_5, packetmin, packetmax);

  for (int ibin = 1; ibin <= monpacket->GetNbinsX(); ibin++)
    {
      int packetid =  (int) monpacket->GetBinContent(ibin) - packetmin;
      int ipkt = packetid + 1; // first bin is 1
      double npackets = monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (npackets <= 0)
	{
	  npackets = 1;
	}
      double nldtbtimeout = (subsyspacket->GetBinContent(ibin, LDTBTIMEOUTBIN) / npackets) * 100.;
      double nnorcc =  (subsyspacket->GetBinContent(ibin, NORCCBIN) / npackets) * 100.;
      histo[0]->SetBinContent(ipkt, nldtbtimeout);
      histo[1]->SetBinContent(ipkt, nnorcc);
      double nrcc[6];
      double nbclk[6];
      for (int k=0; k<6;k++)
	{
	      nrcc[k] =  (subsyspacket->GetBinContent(ibin, RCCBEGINBIN+k) / npackets) * 100.;
	      nbclk[k] =  (subsyspacket->GetBinContent(ibin, BCLKBEGINBIN+k) / npackets) * 100.;
	  if (suppress_masked && rccmasked[packetid][k])
	    {
	      cout << "packet " << packetid+packetmin << "masking rcc " << k << endl;
	      cout << "was " << nrcc[k] << ", bclk: " << nbclk[k] << endl;
	      nrcc[k] = 0;
	      nbclk[k] = 0;
	    }
	  else
	    {
	      nrcc[k] =  (subsyspacket->GetBinContent(ibin, RCCBEGINBIN+k) / npackets) * 100.;
	      nbclk[k] =  (subsyspacket->GetBinContent(ibin, BCLKBEGINBIN+k) / npackets) * 100.;
	    }
	  histo[2+k]->SetBinContent(ipkt, nrcc[k]);
	  histo[8+k] ->SetBinContent(ipkt, nbclk[k]);
	}
    }

  const string hname[NHIST1] =
    {
      "ldtbtimeout" ,
      "norcc",
      "rcc0",
      "rcc1",
      "rcc2",
      "rcc3",
      "rcc4",
      "rcc5",
      "bclk0",
      "bclk1",
      "bclk2",
      "bclk3",
      "bclk4",
      "bclk5"
    };

  const string desc[NHIST1] =
    {
      "LDTB Timeout" ,
      "No RCC",
      "RCC 0",
      "RCC 1",
      "RCC 2",
      "RCC 3",
      "RCC 4",
      "RCC 5",
      "BCLK 0",
      "BCLK 1",
      "BCLK 2",
      "BCLK 3",
      "BCLK 4",
      "BCLK 5"
    };
  //  Pad2[0]->SetEditable(kTRUE);
  string HFirstName = what;
  string HName;
  int igood = 0;
  TText description;
  description.SetTextFont(62);
  description.SetTextSize(0.13);
  description.SetNDC();  // set to normalized coordinates
  description.SetTextAlign(23); // center/top alignment
  description.SetTextAngle(270);
  for (short int ihist = 0; ihist < NHIST1; ihist++)
    {
      HName = "Daq_" + HFirstName + hname[ihist];
      Pad0[ihist]->cd();
      if (histo[ihist])
	{
	  if (histo[ihist]->Integral() < 0)
	    {
	      description.SetTextAngle(0);
	      ostringstream outtxt;
	      outtxt << desc[ihist] << " Not Implemented";
	      description.DrawText(0.5, 0.5, outtxt.str().c_str());
	      description.SetTextAngle(270);
	    }
	  else if (histo[ihist]->Integral() == 0)
	    {
	      Pad0[ihist]->SetLogy(0);
	      histo[ihist]->SetMinimum(0);
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->SetMaximum(105.);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");
	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());
	    }
	  else
	    {
	      // here come the errors
	      Pad0[ihist]->SetLogy(0);
	      histo[ihist]->SetMaximum(105.);
	      double histominimum = 0;
	      for (int i = 1; i <= histo[ihist]->GetNbinsX(); i++)
		{
		  if (histo[ihist]->GetBinContent(i) <= 0)
		    {
		      histo[ihist]->SetBinContent(i, 1.e-21);
		    }
		  else if (histo[ihist]->GetBinContent(i) > 1.e-21
			   && histo[ihist]->GetBinContent(i) < 20)
		    {
		      Pad0[ihist]->SetLogy(1);
		      histo[ihist]->SetMaximum(150.);
		      if (histominimum == 0)
			{
			  histominimum = histo[ihist]->GetBinContent(i);
			}
		      else
			{
			  if (histominimum > histo[ihist]->GetBinContent(i))
			    {
			      histominimum = histo[ihist]->GetBinContent(i);
			    }
			}
		      histo[ihist]->SetMinimum(0.1);
		    }
		}
	      histo[ihist]->SetMinimum(histominimum / 10.);
	      histo[ihist]->GetYaxis()->SetNoExponent();
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");

	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());
	    }
	  igood ++;
	}
      else
	{
	  cout << "Histogram " << HName << " not found" << endl;
	}
      Pad0[ihist]->Update();
    }
  transparent[0]->cd();
  transparent[0]->SetEditable(kTRUE);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.07);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  ostringstream runnostream, datestream;
  //string runstring;
  if (igood) // subsystem histograms exist
    {
      runnostream << HFirstName << " Run " << cl->RunNumber();
    }
  else // possible typo in granule name
    {
      runnostream << "Can't find Granule " << HFirstName << ", Typo?";
    }
  //  runstring = runnostream.str();
  transparent[0]->Clear();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC[0]->Update();

  return 0;
}

int
VtxsMonDraw::DrawHybrid(const char *what)
{
  if (! gROOT->FindObject("DaqVtxsMon1"))
    {
      MakeCanvas("DaqVtxsMon1");
    }
  TC[1]->cd();
  TC[1]->Clear("D");
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monivars = "Daq_" + ThisName + "AllVariables";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string subsyspkts = "Daq_" + ThisName + "SubsysStatus";
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monpacket = cl->getHisto(monpkts.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *subsyspacket = cl->getHisto(subsyspkts.c_str());
  if (! monpacket)
    {
      DaqMonDraw *daqmon = DaqMonDraw::instance();
      daqmon->DrawDeadServer(transparent[1]);
      TC[1]->Update();
      return -1;
    }


  int NPacketThisGranule = monpacket->GetNbinsX();
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  int packetmin = (int) monpacket->GetBinContent(1);
  int packetmax = (int) monpacket->GetBinContent(NPacketThisGranule);
  TH1 *histo[NHIST2];
  string hname[NHIST2];
  string desc[NHIST2];
  ostringstream hnamestr;
  int ihist = 0;
  for (int ircc = 0; ircc < 6; ircc++)
    {
      for (int ihybrid = 0; ihybrid < 4; ihybrid++)
	{
	  hnamestr.str("");
	  hnamestr << "Daq_"  << ThisName << "_" << ircc << "_" << ihybrid;
	  histo[ihist] = MakeSnglHisto(hnamestr.str(), packetmin, packetmax);
	  hnamestr.str("");
	  hnamestr << "_" << ircc << "_" << ihybrid;
	  hname[ihist] = hnamestr.str();
	  hnamestr.str("");
	  hnamestr << "Hybrid " << ihybrid;
	  desc[ihist] = hnamestr.str();
	  ihist++;
	}
    }
  for (int ibin = 1; ibin <= monpacket->GetNbinsX(); ibin++)
    {
      int ipkt = (int) monpacket->GetBinContent(ibin) - packetmin + 1; // first bin is 1
      double npackets = monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (npackets <= 0)
	{
	  npackets = 1;
	}
      for (int i=0; i<NHIST2; i++)
	{
	  int rcc = i/4;
	  int ihybrid = i-(rcc*4);
	  double rval = (subsyspacket->GetBinContent(ibin, HYBRIDBEGINBIN+i) / npackets) * 100.;
	  if (suppress_masked && hybridmask[ipkt-1][rcc][ihybrid])
	    {
	      //	      cout << "packet " << ipkt+packetmin-1 << " bin " << i << " rcc " << rcc << " hybrid " << ihybrid << " masked" << endl;
	      rval = 0;
	    }
	  if (rval < 0)
	    {
	      rval = 0;
	    }
	  histo[i]->SetBinContent(ipkt, rval);
	}
    }

  string HFirstName = what;
  string HName;
  int igood = 0;
  TText description;
  description.SetTextFont(62);
  description.SetTextSize(0.13);
  description.SetNDC();  // set to normalized coordinates
  description.SetTextAlign(23); // center/top alignment
  description.SetTextAngle(270);
  for (short int ihist = 0; ihist < NHIST2; ihist++)
    {
      HName = "Daq_" + HFirstName + hname[ihist];
      Pad1[ihist]->cd();
      description.SetTextAngle(270);
      if (histo[ihist])
	{
	  if (histo[ihist]->Integral() < 0)
	    {
	      description.SetTextAngle(0);
	      ostringstream outtxt;
	      outtxt << desc[ihist] << " Not Implemented";
	      description.DrawText(0.5, 0.5, outtxt.str().c_str());
	      description.SetTextAngle(270);
	    }
	  else if (histo[ihist]->Integral() == 0)
	    {
	      Pad1[ihist]->SetLogy(0);
	      histo[ihist]->SetMinimum(0);
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->SetMaximum(105.);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");
	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());

	    }
	  else
	    {
	      // here come the errors
	      Pad1[ihist]->SetLogy(0);
	      histo[ihist]->SetMaximum(105.);
	      double histominimum = 0;
	      for (int i = 1; i <= histo[ihist]->GetNbinsX(); i++)
		{
		  if (histo[ihist]->GetBinContent(i) <= 0)
		    {
		      histo[ihist]->SetBinContent(i, 1.e-21);
		    }
		  else if (histo[ihist]->GetBinContent(i) > 1.e-21
			   && histo[ihist]->GetBinContent(i) < 20)
		    {
		      Pad1[ihist]->SetLogy(1);
		      histo[ihist]->SetMaximum(150.);
		      if (histominimum == 0)
			{
			  histominimum = histo[ihist]->GetBinContent(i);
			}
		      else
			{
			  if (histominimum > histo[ihist]->GetBinContent(i))
			    {
			      histominimum = histo[ihist]->GetBinContent(i);
			    }
			}
		      histo[ihist]->SetMinimum(0.1);
		    }
		}
	      histo[ihist]->SetMinimum(histominimum / 10.);
	      histo[ihist]->GetYaxis()->SetNoExponent();
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");

	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());
	    }
	  igood ++;
	}
      else
	{
	  cout << "Histogram " << HName << " not found" << endl;
	}
      Pad1[ihist]->Update();
    }
  transparent[1]->cd();
  transparent[1]->SetEditable(kTRUE);
  transparent[1]->Clear();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  PrintRun.SetTextSize(0.05);
  ostringstream rccstr;
  for (int i=0; i<6; i++)
    {
      double xlow = i * 1. / (float)VTXSPLOTHYBRIDCOLS ;
      double xhi = (i + 1) / (float)VTXSPLOTHYBRIDCOLS;
      double xpos = (xlow+xhi)/2.;
      rccstr.str("");
      rccstr << "RCC " << i;
      PrintRun.DrawText(xpos,0.87,rccstr.str().c_str());
    }
  PrintRun.SetTextSize(0.07);
  ostringstream runnostream, datestream;
  //string runstring;
  if (igood) // subsystem histograms exist
    {
      runnostream << "Disabled Hybrids Run " << cl->RunNumber();
    }
  else // possible typo in granule name
    {
      runnostream << "Can't find " << HFirstName << ", Typo?";
    }
  //  runstring = runnostream.str();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC[1]->Update();

  return 0;
}

int
VtxsMonDraw::DrawInvalidCellId(const int i_rcc, const char *what)
{
  int NHISTCELLID = 24;
  ostringstream canvasname;
  canvasname << "DaqVtxsCellMon" << i_rcc;
  if (! gROOT->FindObject(canvasname.str().c_str()))
    {
      MakeCanvas("DaqVtxsCellMon",i_rcc);
    }

  OnlMonClient *cl = OnlMonClient::instance();

  int ioffset = 2 + i_rcc;
  TC[ioffset]->cd();
  TC[ioffset]->Clear("D");


  string monhisname = "Daq_" + ThisName + "Monitor";
  string monivars = "Daq_" + ThisName + "AllVariables";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string subsyspkts = "Daq_" + ThisName + "SubsysStatus";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monpacket = cl->getHisto(monpkts.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *subsyspacket = cl->getHisto(subsyspkts.c_str());
  if (! monpacket)
    {
      DaqMonDraw *daqmon = DaqMonDraw::instance();
      daqmon->DrawDeadServer(transparent[ioffset]);
      TC[ioffset]->Update();
      return -1;
    }


  int NPacketThisGranule = monpacket->GetNbinsX();
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  int packetmin = (int) monpacket->GetBinContent(1);
  int packetmax = (int) monpacket->GetBinContent(NPacketThisGranule);
  TH1 *histo[NHISTCELLID];
  string hname[NHISTCELLID];
  string desc[NHISTCELLID];
  ostringstream hnamestr;
  int ihist = 0;
  for (int ircc = 0; ircc < 2; ircc++)
    {
      for (int ichip = 0; ichip < 12; ichip++)
	{
	  hnamestr.str("");
	  hnamestr << "Daq_"  << ThisName << "_" << ircc + i_rcc << "_" << ichip;
	  histo[ihist] = MakeSnglHisto(hnamestr.str(), packetmin, packetmax);
	  hnamestr.str("");
	  hnamestr << "_" << ircc + i_rcc << "_" << ichip;
	  hname[ihist] = hnamestr.str();
	  hnamestr.str("");
	  hnamestr << "Chip " << ichip;
	  desc[ihist] = hnamestr.str();
	  ihist++;
	}
    }

  for (int ibin = 1; ibin <= monpacket->GetNbinsX(); ibin++)
    {
      int ipkt = (int) monpacket->GetBinContent(ibin) - packetmin + 1; // first bin is 1
      double npackets = monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (npackets <= 0)
	{
	  npackets = 1;
	}
      for (int i=0; i<NHISTCELLID; i++)
	{
	  double rval = (subsyspacket->GetBinContent(ibin, CELLIDINVBEGINBIN+i+i_rcc*NHISTCELLID) / npackets) * 100.;
	  if (rval < 0)
	    {
	      rval = 0;
	    }
	  histo[i]->SetBinContent(ipkt, rval);
	}
    }

  string HFirstName = what;
  string HName;
  int igood = 0;
  TText description;
  description.SetTextFont(62);
  description.SetTextSize(0.13);
  description.SetNDC();  // set to normalized coordinates
  description.SetTextAlign(23); // center/top alignment
  description.SetTextAngle(270);
  for (short int ihist = 0; ihist < NHISTCELLID; ihist++)
    {
      HName = "Daq_" + HFirstName + hname[ihist];
      Pad2[i_rcc][ihist]->cd();
      description.SetTextAngle(270);
      if (histo[ihist])
	{
	  if (histo[ihist]->Integral() < 0)
	    {
	      description.SetTextAngle(0);
	      ostringstream outtxt;
	      outtxt << desc[ihist] << " Not Implemented";
	      description.DrawText(0.5, 0.5, outtxt.str().c_str());
	      description.SetTextAngle(270);
	    }
	  else if (histo[ihist]->Integral() == 0)
	    {
	      Pad2[i_rcc][ihist]->SetLogy(0);
	      histo[ihist]->SetMinimum(0);
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->SetMaximum(105.);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");
	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());

	    }
	  else
	    {
	      // here come the errors
	      Pad2[i_rcc][ihist]->SetLogy(0);
	      histo[ihist]->SetMaximum(105.);
	      double histominimum = 0;
	      for (int i = 1; i <= histo[ihist]->GetNbinsX(); i++)
		{
		  if (histo[ihist]->GetBinContent(i) <= 0)
		    {
		      histo[ihist]->SetBinContent(i, 1.e-21);
		    }
		  else if (histo[ihist]->GetBinContent(i) > 1.e-21
			   && histo[ihist]->GetBinContent(i) < 20)
		    {
		      Pad2[i_rcc][ihist]->SetLogy(1);
		      histo[ihist]->SetMaximum(150.);
		      if (histominimum == 0)
			{
			  histominimum = histo[ihist]->GetBinContent(i);
			}
		      else
			{
			  if (histominimum > histo[ihist]->GetBinContent(i))
			    {
			      histominimum = histo[ihist]->GetBinContent(i);
			    }
			}
		      histo[ihist]->SetMinimum(0.1);
		    }
		}
	      histo[ihist]->SetMinimum(histominimum / 10.);
	      histo[ihist]->GetYaxis()->SetNoExponent();
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");

	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());
	    }
	  igood ++;
	}
      else
	{
	  cout << "Histogram " << HName << " not found" << endl;
	}
      Pad2[i_rcc][ihist]->Update();
    }

  transparent[ioffset]->cd();
  transparent[ioffset]->SetEditable(kTRUE);
  transparent[ioffset]->Clear();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  PrintRun.SetTextSize(0.05);
  ostringstream rccstr;
  for (int i=0; i<6; i++)
    {
      double xlow = i * 1. / (float)VTXSPLOTCELLIDCOLS;
      double xhi = (i + 1) / (float)VTXSPLOTCELLIDCOLS;
      double xpos = (xlow+xhi)/2.;
      rccstr.str("");
      if (i < 3)
	{
	  rccstr << "RCC " << 2*i_rcc;
	}
      else
	{
	  rccstr << "RCC " << 2*i_rcc+1;
	}
      PrintRun.DrawText(xpos,0.87,rccstr.str().c_str());
    }
  PrintRun.SetTextSize(0.07);
  ostringstream runnostream, datestream;
  //string runstring;
  if (igood) // subsystem histograms exist
    {
      runnostream << "Invalid Cell Ids RCC" << i_rcc*2 << "/" << i_rcc*2+1 << " Run " << cl->RunNumber();
    }
  else // possible typo in granule name
    {
      runnostream << "Can't find Granule " << HFirstName << ", Typo?";
    }
  //  runstring = runnostream.str();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC[ioffset]->Update();

  return 0;
}

int
VtxsMonDraw::DrawBadCellId(const int i_rcc, const char *what)
{
  int NHISTCELLID = 24;
  ostringstream canvasname;
  canvasname << "DaqVtxsBadCellMon" << i_rcc;
  if (! gROOT->FindObject(canvasname.str().c_str()))
    {
      MakeCanvas("DaqVtxsBadCellMon",i_rcc);
    }

  OnlMonClient *cl = OnlMonClient::instance();

  int ioffset = 5 + i_rcc;
  TC[ioffset]->cd();
  TC[ioffset]->Clear("D");


  string monhisname = "Daq_" + ThisName + "Monitor";
  string monivars = "Daq_" + ThisName + "AllVariables";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string subsyspkts = "Daq_" + ThisName + "SubsysStatus";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monpacket = cl->getHisto(monpkts.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *subsyspacket = cl->getHisto(subsyspkts.c_str());
  if (! monpacket)
    {
      DaqMonDraw *daqmon = DaqMonDraw::instance();
      daqmon->DrawDeadServer(transparent[ioffset]);
      TC[ioffset]->Update();
      return -1;
    }


  int NPacketThisGranule = monpacket->GetNbinsX();
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  int packetmin = (int) monpacket->GetBinContent(1);
  int packetmax = (int) monpacket->GetBinContent(NPacketThisGranule);
  TH1 *histo[NHISTCELLID];
  string hname[NHISTCELLID];
  string desc[NHISTCELLID];
  ostringstream hnamestr;
  int ihist = 0;
  for (int ircc = 0; ircc < 2; ircc++)
    {
      for (int ichip = 0; ichip < 12; ichip++)
	{
	  hnamestr.str("");
	  hnamestr << "Daq_"  << ThisName << "_" << ircc + i_rcc << "_" << ichip;
	  histo[ihist] = MakeSnglHisto(hnamestr.str(), packetmin, packetmax);
	  hnamestr.str("");
	  hnamestr << "_" << ircc + i_rcc << "_" << ichip;
	  hname[ihist] = hnamestr.str();
	  hnamestr.str("");
	  hnamestr << "Chip " << ichip;
	  desc[ihist] = hnamestr.str();
	  ihist++;
	}
    }

  for (int ibin = 1; ibin <= monpacket->GetNbinsX(); ibin++)
    {
      int ipkt = (int) monpacket->GetBinContent(ibin) - packetmin + 1; // first bin is 1
      double npackets = monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (npackets <= 0)
	{
	  npackets = 1;
	}
      for (int i=0; i<NHISTCELLID; i++)
	{
	  double rval = (subsyspacket->GetBinContent(ibin, CELLIDBEGINBIN+i+i_rcc*NHISTCELLID) / npackets) * 100.;
	  if (rval < 0)
	    {
	      rval = 0;
	    }
	  histo[i]->SetBinContent(ipkt, rval);
	}
    }

  string HFirstName = what;
  string HName;
  int igood = 0;
  TText description;
  description.SetTextFont(62);
  description.SetTextSize(0.13);
  description.SetNDC();  // set to normalized coordinates
  description.SetTextAlign(23); // center/top alignment
  description.SetTextAngle(270);
  for (short int ihist = 0; ihist < NHISTCELLID; ihist++)
    {
      HName = "Daq_" + HFirstName + hname[ihist];
      Pad3[i_rcc][ihist]->cd();
      description.SetTextAngle(270);
      if (histo[ihist])
	{
	  if (histo[ihist]->Integral() < 0)
	    {
	      description.SetTextAngle(0);
	      ostringstream outtxt;
	      outtxt << desc[ihist] << " Not Implemented";
	      description.DrawText(0.5, 0.5, outtxt.str().c_str());
	      description.SetTextAngle(270);
	    }
	  else if (histo[ihist]->Integral() == 0)
	    {
	      Pad3[i_rcc][ihist]->SetLogy(0);
	      histo[ihist]->SetMinimum(0);
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->SetMaximum(105.);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");
	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());

	    }
	  else
	    {
	      // here come the errors
	      Pad3[i_rcc][ihist]->SetLogy(0);
	      histo[ihist]->SetMaximum(105.);
	      double histominimum = 0;
	      for (int i = 1; i <= histo[ihist]->GetNbinsX(); i++)
		{
		  if (histo[ihist]->GetBinContent(i) <= 0)
		    {
		      histo[ihist]->SetBinContent(i, 1.e-21);
		    }
		  else if (histo[ihist]->GetBinContent(i) > 1.e-21
			   && histo[ihist]->GetBinContent(i) < 20)
		    {
		      Pad3[i_rcc][ihist]->SetLogy(1);
		      histo[ihist]->SetMaximum(150.);
		      if (histominimum == 0)
			{
			  histominimum = histo[ihist]->GetBinContent(i);
			}
		      else
			{
			  if (histominimum > histo[ihist]->GetBinContent(i))
			    {
			      histominimum = histo[ihist]->GetBinContent(i);
			    }
			}
		      histo[ihist]->SetMinimum(0.1);
		    }
		}
	      histo[ihist]->SetMinimum(histominimum / 10.);
	      histo[ihist]->GetYaxis()->SetNoExponent();
	      histo[ihist]->GetYaxis()->SetTitleOffset(0.45);
	      histo[ihist]->GetYaxis()->SetTitleSize(0.1);
	      histo[ihist]->GetYaxis()->SetTitle("Error Rate (%)");

	      histo[ihist]->Draw();
	      description.DrawText(0.97, 0.5, desc[ihist].c_str());
	    }
	  igood ++;
	}
      else
	{
	  cout << "Histogram " << HName << " not found" << endl;
	}
      Pad3[i_rcc][ihist]->Update();
    }

  transparent[ioffset]->cd();
  transparent[ioffset]->SetEditable(kTRUE);
  transparent[ioffset]->Clear();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  PrintRun.SetTextSize(0.05);
  ostringstream rccstr;
  for (int i=0; i<6; i++)
    {
      double xlow = i * 1. / (float)VTXSPLOTCELLIDCOLS;
      double xhi = (i + 1) / (float)VTXSPLOTCELLIDCOLS;
      double xpos = (xlow+xhi)/2.;
      rccstr.str("");
      if (i < 3)
	{
	  rccstr << "RCC " << 2*i_rcc;
	}
      else
	{
	  rccstr << "RCC " << 2*i_rcc+1;
	}
      PrintRun.DrawText(xpos,0.87,rccstr.str().c_str());
    }
  PrintRun.SetTextSize(0.07);
  ostringstream runnostream, datestream;
  //string runstring;
  if (igood) // subsystem histograms exist
    {
      runnostream << "Bad Cell Ids RCC" << i_rcc*2 << "/" << i_rcc*2+1 << " Run " << cl->RunNumber();
    }
  else // possible typo in granule name
    {
      runnostream << "Can't find Granule " << HFirstName << ", Typo?";
    }
  //  runstring = runnostream.str();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC[ioffset]->Update();

  return 0;
}

int
VtxsMonDraw::MakeCanvas(std::string const &name, const int i_rcc)
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (name == "DaqVtxsMon0")
    {
      TC[0] = new TCanvas("DaqVtxsMon0", "DaqMon Vtxs", -1, 0,  cl->GetDisplaySizeX(),  cl->GetDisplaySizeY());
      gSystem->ProcessEvents();
      transparent[0] = new TPad("vtxstransparent0", "this does not show", 0, 0, 1, 1);
      transparent[0]->SetFillStyle(4000);
      transparent[0]->Draw();
      transparent[0]->SetEditable(kFALSE);
      char padname[100];
      int ipad = 0; 
      for (short int i = 0; i < VTXSPLOT1ROWS; i++)
	{
	  // up to 0,9 to make space for the run number
	  double ylow = 0.9 * (1 - (i + 1) /(float)VTXSPLOT1ROWS );
	  double yhi = 0.9 * (1 - i /(float)VTXSPLOT1ROWS );
	  for (short int j = 0; j < VTXSPLOT1COLS; j++)
	    {
	      double xlow = j * 1. / (float)VTXSPLOT1COLS ;
	      double xhi = (j + 1) / (float)VTXSPLOT1COLS;
	      sprintf(padname, "VtxsPad0%02d", ipad);
	      Pad0[ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
	      Pad0[ipad]->SetFillStyle(4000);
	      Pad0[ipad]->Draw();
	      ipad++;
	    }
	}
      return 0;
    }
  else if (name == "DaqVtxsMon1")
    {
      TC[1] = new TCanvas("DaqVtxsMon1", "DaqMon Vtxs", -1, 0,  cl->GetDisplaySizeX(),  cl->GetDisplaySizeY());
      gSystem->ProcessEvents();
      transparent[1] = new TPad("vtxstransparent1", "this does not show", 0, 0, 1, 1);
      transparent[1]->SetFillStyle(4000);
      transparent[1]->Draw();
      transparent[1]->SetEditable(kFALSE);
      char padname[100];
      int ipad = 0; 
      for (short int j = 0; j < VTXSPLOTHYBRIDCOLS; j++)
	{
	  double xlow = j * 1. / (float)VTXSPLOTHYBRIDCOLS ;
	  double xhi = (j + 1) / (float)VTXSPLOTHYBRIDCOLS;
	  for (short int i = 0; i < VTXSPLOTHYBRIDROWS; i++)
	    {
	      // up to 0,9 to make space for the run number
	      double ylow = 0.85 * (1 - (i + 1) /(float)VTXSPLOTHYBRIDROWS );
	      double yhi = 0.85 * (1 - i /(float)VTXSPLOTHYBRIDROWS );
	      sprintf(padname, "VtxsPad1%02d", ipad);
	      Pad1[ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
	      Pad1[ipad]->SetFillStyle(4000);
	      Pad1[ipad]->Draw();
	      ipad++;
	    }
	}
      return 0;
    }
  else if (name == "DaqVtxsCellMon")
    {
      int ioffset = 2 + i_rcc;
      ostringstream canvname;
      canvname << "DaqVtxsCellMon" << i_rcc;
      TC[ioffset] = new TCanvas(canvname.str().c_str(), "DaqMon Vtxs", -1, 0,  cl->GetDisplaySizeX(),  cl->GetDisplaySizeY());
      gSystem->ProcessEvents();
      transparent[ioffset] = new TPad("vtxstransparent1", "this does not show", 0, 0, 1, 1);
      transparent[ioffset]->SetFillStyle(4000);
      transparent[ioffset]->Draw();
      transparent[ioffset]->SetEditable(kFALSE);
      char padname[100];
      int ipad = 0; 
      for (short int j = 0; j < VTXSPLOTCELLIDCOLS; j++)
	{
	  double xlow = j * 1. / (float)VTXSPLOTCELLIDCOLS ;
	  double xhi = (j + 1) / (float)VTXSPLOTCELLIDCOLS;
	  for (short int i = 0; i <VTXSPLOTCELLIDROWS ; i++)
	    {
	      // up to 0,9 to make space for the run number
	      double ylow = 0.85 * (1 - (i + 1) /(float)VTXSPLOTCELLIDROWS );
	      double yhi = 0.85 * (1 - i /(float)VTXSPLOTCELLIDROWS );
	      sprintf(padname, "VtxsPad2%d_%02d", ioffset, ipad);
	      Pad2[i_rcc][ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
	      Pad2[i_rcc][ipad]->SetFillStyle(4000);
	      Pad2[i_rcc][ipad]->Draw();
	      ipad++;
	    }
	}
      return 0;
    }
  else if (name == "DaqVtxsBadCellMon")
    {
      int ioffset = 5 + i_rcc;
      ostringstream canvname;
      canvname << "DaqVtxsBadCellMon" << i_rcc;
      TC[ioffset] = new TCanvas(canvname.str().c_str(), "DaqMon Vtxs", -1, 0,  cl->GetDisplaySizeX(),  cl->GetDisplaySizeY());
      gSystem->ProcessEvents();
      transparent[ioffset] = new TPad("vtxstransparent1", "this does not show", 0, 0, 1, 1);
      transparent[ioffset]->SetFillStyle(4000);
      transparent[ioffset]->Draw();
      transparent[ioffset]->SetEditable(kFALSE);
      char padname[100];
      int ipad = 0; 
      for (short int j = 0; j < VTXSPLOTCELLIDCOLS; j++)
	{
	  double xlow = j * 1. / (float)VTXSPLOTCELLIDCOLS ;
	  double xhi = (j + 1) / (float)VTXSPLOTCELLIDCOLS;
	  for (short int i = 0; i <VTXSPLOTCELLIDROWS ; i++)
	    {
	      // up to 0,9 to make space for the run number
	      double ylow = 0.85 * (1 - (i + 1) /(float)VTXSPLOTCELLIDROWS );
	      double yhi = 0.85 * (1 - i /(float)VTXSPLOTCELLIDROWS );
	      sprintf(padname, "VtxsPad3%d_%02d", ioffset, ipad);
	      Pad3[i_rcc][ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
	      Pad3[i_rcc][ipad]->SetFillStyle(4000);
	      Pad3[i_rcc][ipad]->Draw();
	      ipad++;
	    }
	}
      return 0;
    }
  return -1;
}

void
VtxsMonDraw::identify(ostream& out) const
{
  // first execute identify for common packet errors
  GranMonDraw::identify(out);
  OnlMonClient *cl = OnlMonClient::instance();
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string monivars = "Daq_" + ThisName + "AllVariables";
  string subsyspkts = "Daq_" + ThisName + "SubsysStatus";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monpkt = cl->getHisto(monpkts.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *subsyspacket = cl->getHisto(subsyspkts.c_str());
  if (!subsyspacket)
    {
      cout << "could not find " << subsyspkts << endl;
      return;
    }

  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  static int iwid = 10;
  if (htmloutflag)
    {
      out << "<font color=black> ";
    }
  out << ThisName << " Subsystem specific errors";
  out << endl;
  if (htmloutflag)
    {
      out << "</br>" << endl;
      out << "<table align=center> <tr><td align=center>";
    }
  out << setw(iwid) << "Pkt";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "LDTB";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "No RCC";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "RCC 0";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "RCC 1";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "RCC 2";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "RCC 3";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "RCC 4";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "RCC 5";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "BCLK 0";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "BCLK 1";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "BCLK 2";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "BCLK 3";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "BCLK 4";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "BCLK 5";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "NPkts";
  if (htmloutflag)
    {
      out << "</td></tr></font> ";
    }
  out << endl;
  for (int ibin = 1; ibin <= monhis->GetNbinsY(); ibin++)
    {
      int nval[14];
      int nldtbtimeout = (int) subsyspacket->GetBinContent(ibin, LDTBTIMEOUTBIN);
      int nnorcc =  (int) subsyspacket->GetBinContent(ibin, NORCCBIN);
      nval[0] = nldtbtimeout;
      nval[1] = nnorcc;
      for (int i = 0; i < 6; i++)
	{
	  nval[i + RCCBEGINBIN -1] = (int) subsyspacket->GetBinContent(ibin, RCCBEGINBIN + i);
	  nval[i + BCLKBEGINBIN -2] = (int) subsyspacket->GetBinContent(ibin, BCLKBEGINBIN + i);
	}
      int npackets  = (int) monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (htmloutflag)
	{
	  out << "<tr><td align=center> ";
	}
      SetHtmlColor(out, 0);
      for (int j=0; j< 14; j++)
	{
	  if ( nval[j] > 0)
	    {
	      SetHtmlColor(out, 1);
	      break;
	    }
	}
      out << setw(iwid) << ((int) monpkt->GetBinContent(ibin));
      EndFont(out);
      for (int i = 0; i < 14; i++)
	{
	  if (htmloutflag)
	    {
	      out << "</td><td align=center>";
	    }
	  SetHtmlColor(out, nval[i]);
	  out << setw(iwid) << nval[i];
	  EndFont(out);
	}
      if (htmloutflag)
	{
	  out << "</td><td align=center>";
	  out << "<font color=black> ";
	}
      out << setw(iwid) << npackets;
      EndFont(out);
      out << endl;
      if (htmloutflag)
	{
	  out << "</td></tr>";
	}
    }
  if (htmloutflag)
    {
      out << "</table>" << endl << "<p>" << endl;
    }

  string coltitle[14];
  ostringstream colstr;
  coltitle[0] = "Pkt";
  coltitle[13] = "Npkts";
  int itit = 1;
  for (int ircc =0; ircc < 3; ircc++)
    {
      for (int ihybrid =0; ihybrid < 4; ihybrid++)
	{
	  colstr.str("");
	  colstr << "RCC" << ircc << "H" << ihybrid;
	  coltitle[itit] = colstr.str();
	  itit++;
	}
    }
  if (htmloutflag)
    {
      out << "<font color=black> ";
    }
  out << ThisName << " Hybrids RCC0-RCC2";
  out << endl;
  if (htmloutflag)
    {
      out << "</br>" << endl;
      out << "<table align=center>" << endl << "<tr>"; 
    }
  for (int i=0; i<14;i++)
    {
      if (htmloutflag)
	{
	  out << "<td align=center>";
	}
      out << setw(iwid) << coltitle[i];
      if (htmloutflag)
	{
	  out << "</td>";
	}
    }
  if (htmloutflag)
    {
      out << "</tr></font> ";
    }
  out << endl;

  for (int ibin = 1; ibin <= monhis->GetNbinsY(); ibin++)
    {
      int nval[12];
      for (int i = 0; i < 12; i++)
	{
	  nval[i] = (int) subsyspacket->GetBinContent(ibin,HYBRIDBEGINBIN  + i);
	}
      int npackets  = (int) monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (htmloutflag)
	{
	  out << "<tr><td align=center> ";
	}
      SetHtmlColor(out, 0);
      for (int j=0; j< 12; j++)
	{
	  if ( nval[j] > 0)
	    {
	      SetHtmlColor(out, 1);
	      break;
	    }
	}
      out << setw(iwid) << ((int) monpkt->GetBinContent(ibin));
      EndFont(out);
      for (int i = 0; i < 12; i++)
	{
	  if (htmloutflag)
	    {
	      out << "</td><td align=center>";
	    }
	  SetHtmlColor(out, nval[i]);
	  out << setw(iwid) << nval[i];
	  EndFont(out);
	}
      if (htmloutflag)
	{
	  out << "</td><td align=center>";
	  out << "<font color=black> ";
	}
      out << setw(iwid) << npackets;
      EndFont(out);
      out << endl;
      if (htmloutflag)
	{
	  out << "</td></tr>";
	}
    }
  if (htmloutflag)
    {
      out << "</table>" << endl << "<p>" << endl;
    }

  //------------------------------------------------
  coltitle[0] = "Pkt";
  coltitle[13] = "Npkts";
  itit = 1;
  for (int ircc =3; ircc < 6; ircc++)
    {
      for (int ihybrid =0; ihybrid < 4; ihybrid++)
	{
	  colstr.str("");
	  colstr << "RCC" << ircc << "H" << ihybrid;
	  coltitle[itit] = colstr.str();
	  itit++;
	}
    }
  if (htmloutflag)
    {
      out << "<font color=black> ";
    }
  out << ThisName << " Hybrids RCC3-RCC5";
  out << endl;
  if (htmloutflag)
    {
      out << "</br>" << endl;
      out << "<table align=center>" << endl << "<tr>"; 
    }
  for (int i=0; i<14;i++)
    {
      if (htmloutflag)
	{
	  out << "<td align=center>";
	}
      out << setw(iwid) << coltitle[i];
      if (htmloutflag)
	{
	  out << "</td>";
	}
    }
  if (htmloutflag)
    {
      out << "</tr></font> ";
    }
  out << endl;

  for (int ibin = 1; ibin <= monhis->GetNbinsY(); ibin++)
    {
      int nval[12];
      for (int i = 0; i < 12; i++)
	{
	  nval[i] = (int) subsyspacket->GetBinContent(ibin,HYBRIDBEGINBIN  + i + 12);
	}
      int npackets  = (int) monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (htmloutflag)
	{
	  out << "<tr><td align=center> ";
	}
      SetHtmlColor(out, 0);
      for (int j=0; j< 12; j++)
	{
	  if ( nval[j] > 0)
	    {
	      SetHtmlColor(out, 1);
	      break;
	    }
	}
      out << setw(iwid) << ((int) monpkt->GetBinContent(ibin));
      EndFont(out);
      for (int i = 0; i < 12; i++)
	{
	  if (htmloutflag)
	    {
	      out << "</td><td align=center>";
	    }
	  SetHtmlColor(out, nval[i]);
	  out << setw(iwid) << nval[i];
	  EndFont(out);
	}
      if (htmloutflag)
	{
	  out << "</td><td align=center>";
	  out << "<font color=black> ";
	}
      out << setw(iwid) << npackets;
      EndFont(out);
      out << endl;
      if (htmloutflag)
	{
	  out << "</td></tr>";
	}
    }
  if (htmloutflag)
    {
      out << "</table>" << endl << "<p>" << endl;
    }

  //------------------------------------------------
  coltitle[0] = "Pkt";
  coltitle[13] = "Npkts";
  itit = 1;
  for (int ichip =0; ichip < 12; ichip++)
    {
      colstr.str("");
      colstr << "CHIP" << ichip;
      coltitle[itit] = colstr.str();
      itit++;
    }
  for (int ircc =0; ircc < 6; ircc++)
    {
      if (htmloutflag)
	{
	  out << "<font color=black> ";
	}
      out << endl;
      out << ThisName << " Chips Mismatched Ids RCC" << ircc;
      out << endl;
      if (htmloutflag)
	{
	  out << "</br>" << endl;
	  out << "<table align=center>" << endl << "<tr>"; 
	}
      for (int i=0; i<14;i++)
	{
	  if (htmloutflag)
	    {
	      out << "<td align=center>";
	    }
	  out << setw(iwid) << coltitle[i];
	  if (htmloutflag)
	    {
	      out << "</td>";
	    }
	}
      if (htmloutflag)
	{
	  out << "</tr></font> ";
	}
      out << endl;

      for (int ibin = 1; ibin <= monhis->GetNbinsY(); ibin++)
	{
	  int nval[12];
	  for (int i = 0; i < 12; i++)
	    {
	      nval[i] = (int) subsyspacket->GetBinContent(ibin,CELLIDBEGINBIN  + i +ircc*12 );
 // 	      if (nval[i] < 0)
 // 		{
 // 		  cout << "cont " << nval[i] << ", ircc: " << ircc << ", ichip " << i << ", index: " << CELLIDBEGINBIN  + i +ircc*12
 // << endl;
 // 		}
	    }
	  int npackets  = (int) monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
	  if (htmloutflag)
	    {
	      out << "<tr><td align=center> ";
	    }
	  SetHtmlColor(out, 0);
	  for (int j=0; j< 12; j++)
	    {
	      if ( nval[j] > 0)
		{
		  SetHtmlColor(out, 1);
		  break;
		}
	    }
	  out << setw(iwid) << ((int) monpkt->GetBinContent(ibin));
	  EndFont(out);
	  for (int i = 0; i < 12; i++)
	    {
	      if (htmloutflag)
		{
		  out << "</td><td align=center>";
		}
	      SetHtmlColor(out, nval[i]);
	      out << setw(iwid) << nval[i];
	      EndFont(out);
	    }
	  if (htmloutflag)
	    {
	      out << "</td><td align=center>";
	      out << "<font color=black> ";
	    }
	  out << setw(iwid) << npackets;
	  EndFont(out);
	  out << endl;
	  if (htmloutflag)
	    {
	      out << "</td></tr>";
	    }
	}
      if (htmloutflag)
	{
	  out << "</table>" << endl << "<p>" << endl;
	}
    }
  //------------------------------------------------
  coltitle[0] = "Pkt";
  coltitle[13] = "Npkts";
  itit = 1;
  for (int ichip =0; ichip < 12; ichip++)
    {
      colstr.str("");
      colstr << "CHIP" << ichip;
      coltitle[itit] = colstr.str();
      itit++;
    }
  for (int ircc =0; ircc < 6; ircc++)
    {
      if (htmloutflag)
	{
	  out << "<font color=black> ";
	}
      out << endl;
      out << ThisName << " Invalid Chips Ids RCC" << ircc;
      out << endl;
      if (htmloutflag)
	{
	  out << "</br>" << endl;
	  out << "<table align=center>" << endl << "<tr>"; 
	}
      for (int i=0; i<14;i++)
	{
	  if (htmloutflag)
	    {
	      out << "<td align=center>";
	    }
	  out << setw(iwid) << coltitle[i];
	  if (htmloutflag)
	    {
	      out << "</td>";
	    }
	}
      if (htmloutflag)
	{
	  out << "</tr></font> ";
	}
      out << endl;

      for (int ibin = 1; ibin <= monhis->GetNbinsY(); ibin++)
	{
	  int nval[12];
	  for (int i = 0; i < 12; i++)
	    {
	      nval[i] = (int) subsyspacket->GetBinContent(ibin,CELLIDINVBEGINBIN  + i +ircc*12 );
// 	      if (nval[i] < 0)
// 		{
// 		  cout << "cont " << nval[i] << ", ircc: " << ircc << ", ichip " << i << ", index: " << CELLIDINVBEGINBIN  + i +ircc*12
// << endl;
// 		}
	    }
	  int npackets  = (int) monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
	  if (htmloutflag)
	    {
	      out << "<tr><td align=center> ";
	    }
	  SetHtmlColor(out, 0);
	  for (int j=0; j< 12; j++)
	    {
	      if ( nval[j] > 0)
		{
		  SetHtmlColor(out, 1);
		  break;
		}
	    }
	  out << setw(iwid) << ((int) monpkt->GetBinContent(ibin));
	  EndFont(out);
	  for (int i = 0; i < 12; i++)
	    {
	      if (htmloutflag)
		{
		  out << "</td><td align=center>";
		}
	      SetHtmlColor(out, nval[i]);
	      out << setw(iwid) << nval[i];
	      EndFont(out);
	    }
	  if (htmloutflag)
	    {
	      out << "</td><td align=center>";
	      out << "<font color=black> ";
	    }
	  out << setw(iwid) << npackets;
	  EndFont(out);
	  out << endl;
	  if (htmloutflag)
	    {
	      out << "</td></tr>";
	    }
	}
      if (htmloutflag)
	{
	  out << "</table>" << endl << "<p>" << endl;
	}
    }
  return;
}

int
VtxsMonDraw::MakeHtml(const string &what)
{
  suppress_masked = 0;
  DrawThisSubsystem("ALL");
  OnlMonClient *cl = OnlMonClient::instance();
  DaqMonDraw *daqdraw = DaqMonDraw::instance();
  string imgpath = cl->htmlRegisterPage(*daqdraw, "VtxstripRCC", "vtxstrip0", "png");
  cl->CanvasToPng(TC[0], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxstripHybrid", "vtxstriphybrid", "png");
  cl->CanvasToPng(TC[1], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxsInvalidCellrcc0-1", "vtxstripinvcellrcc0-1", "png");
  cl->CanvasToPng(TC[2], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxsInvalidCellrcc2-3", "vtxstripinvcellrcc2-3", "png");
  cl->CanvasToPng(TC[3], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxsInvalidCellrcc4-5", "vtxstripinvcellrcc4-5", "png");
  cl->CanvasToPng(TC[4], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxsBadCellrcc0-1", "vtxstripbadcellrcc0-1", "png");
  cl->CanvasToPng(TC[5], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxsBadCellrcc2-3", "vtxstripbadcellrcc2-3", "png");
  cl->CanvasToPng(TC[6], imgpath);
  imgpath = cl->htmlRegisterPage(*daqdraw, "VtxsBadCellrcc4-5", "vtxstripbadcellrcc4-5", "png");
  cl->CanvasToPng(TC[7], imgpath);
  return 0;
}

int
VtxsMonDraw::ExtractMaskedChannels()
{
  OnlMonClient *cl = OnlMonClient::instance();
  int runno = cl->RunNumber();
  if (runno == currentrun)
    {
      return 0;
    }
  currentrun = runno;
  memset(rccmasked,0,sizeof(rccmasked));
  memset(hybridmask,0,sizeof(hybridmask));
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string subsyspkts = "Daq_" + ThisName + "SubsysStatus";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monpkt = cl->getHisto(monpkts.c_str());
  TH1 *subsyspacket = cl->getHisto(subsyspkts.c_str());
  int packetmin = (int) monpkt->GetBinContent(1);
  for (int ibin = 1; ibin <= monhis->GetNbinsY(); ibin++)
    {
      unsigned int packetid = static_cast<unsigned int> (monpkt->GetBinContent(ibin)) - packetmin;
	  cout << "packet " << packetid << " bin " << ibin << endl;
      for (int i = 0; i < 6; i++)
	{
	  cout << "rcc " << i << " val: " << subsyspacket->GetBinContent(ibin,DISABLEDRCCBEGINBIN+i) << endl;
	  if (subsyspacket->GetBinContent(ibin,DISABLEDRCCBEGINBIN+i) == 1)
	    {
	      rccmasked[packetid][i] = 1;
	    }
	  for (int j=0; j<4; j++)
	    {
	      if (subsyspacket->GetBinContent(ibin,DISABLEDHYBRIDBEGINBIN+i*4+j) == 1)
		{
		  hybridmask[packetid][i][j] = 1;
		  cout << "hybrid " << j << " disabled" << endl;
		}
	    }
	}
    }
  return 0;
}

