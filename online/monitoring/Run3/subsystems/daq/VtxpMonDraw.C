#include "VtxpMonDraw.h"
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

static const int NHIST = 7;

VtxpMonDraw::VtxpMonDraw(const char * granule):
  GranMonDraw(granule),
  TC1(NULL),
  transparent1(NULL)
{
  memset(Pad1,0,sizeof(Pad1));
}

int
VtxpMonDraw::DrawThisSubsystem(const char *what)
{
#define COLBAD 2
#define COLGOOD 3
#define XLABELSIZE 0.09
#define YLABELSIZE 0.09
  if (! gROOT->FindObject("DaqVtxpMon1"))
    {
      MakeCanvas("DaqVtxpMon1");
    }
  TC1->cd();
  TC1->Clear("D");
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
      daqmon->DrawDeadServer(transparent1);
      TC1->Update();
      return -1;
    }
  RemoveVtxpKnownBad(subsyspacket, monpacket);


  int NPacketThisGranule = monpacket->GetNbinsX();
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  int packetmin = (int) monpacket->GetBinContent(1);
  int packetmax = (int) monpacket->GetBinContent(NPacketThisGranule);
  string spiroevtcnt = "Daq_"  + ThisName + "SpiroEvtCnt";
  TH1 *histo[NHIST];
  histo[0] = MakeSnglHisto(spiroevtcnt, packetmin, packetmax);
  string spiro_a_loss = "Daq_"  + ThisName + "SpiroALock";
  histo[1]  = MakeSnglHisto(spiro_a_loss, packetmin, packetmax);
  string spiro_b_loss = "Daq_"  + ThisName + "SpiroBLock";
  histo[2] = MakeSnglHisto(spiro_b_loss, packetmin, packetmax);
  string spiro_a_sizeerr = "Daq_"  + ThisName + "SpiroASizeErr";
  histo[3] = MakeSnglHisto(spiro_a_sizeerr, packetmin, packetmax);
  string spiro_b_sizeerr = "Daq_"  + ThisName + "SpiroBSizeErr";
  histo[4] = MakeSnglHisto(spiro_b_sizeerr, packetmin, packetmax);
  string spiroparitystr = "Daq_"  + ThisName + "SpiroParity";
  histo[5]  = MakeSnglHisto(spiroparitystr, packetmin, packetmax);
  string spiro_event_no = "Daq_"  + ThisName + "SpiroEvtNo";
  histo[6] = MakeSnglHisto(spiro_event_no, packetmin, packetmax);
  // string spiro_clk_03 = "Daq_"  + ThisName + "SpiroClk03";
  // histo[7] =  MakeSnglHisto(spiro_clk_03, packetmin, packetmax);
  // string spiro_clk_47 = "Daq_"  + ThisName + "SpiroClk47";
  // histo[8] =  MakeSnglHisto(spiro_clk_47, packetmin, packetmax);

  for (int ibin = 1; ibin <= monpacket->GetNbinsX(); ibin++)
    {
      int ipkt = (int) monpacket->GetBinContent(ibin) - packetmin + 1; // first bin is 1
      double npackets = monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      if (npackets <= 0)
	{
	  npackets = 1;
	}
      double nspiroevtcnt = (subsyspacket->GetBinContent(ibin, SPIROEVTCNTBIN) / npackets) * 100.;
      double nspiroaloss =  (subsyspacket->GetBinContent(ibin, SPIRO_ALOSSLOCKBIN) / npackets) * 100.;
      double nspirobloss =  (subsyspacket->GetBinContent(ibin, SPIRO_B_LOSSLOCKBIN) / npackets) * 100.;
      double nspiroasizeerr = (subsyspacket->GetBinContent(ibin, SPIRO_ASIZEERRORBIN) / npackets) * 100.;
      double nspirobsizeerr = (subsyspacket->GetBinContent(ibin, SPIRO_B_SIZEERRORBIN) / npackets) * 100.;
      double nspiroparity = (subsyspacket->GetBinContent(ibin, SPIROPARITYBIN) / npackets) * 100.;
      double nspiroevtno = (subsyspacket->GetBinContent(ibin, SPIROEVTNOBIN) / npackets) * 100.;
      // double nspiroclk03  = (subsyspacket->GetBinContent(ibin, SPIROCLK03BIN) / npackets) * 100.;
      // double nspiroclk47  = (subsyspacket->GetBinContent(ibin, SPIROCLK47BIN) / npackets) * 100.;
      histo[0]->SetBinContent(ipkt, nspiroevtcnt);
      histo[1]->SetBinContent(ipkt, nspiroaloss);
      histo[2] ->SetBinContent(ipkt, nspirobloss);
      histo[3]->SetBinContent(ipkt, nspiroasizeerr);
      histo[4]->SetBinContent(ipkt, nspirobsizeerr);
      histo[5]->SetBinContent(ipkt, nspiroparity);
      histo[6]->SetBinContent(ipkt, nspiroevtno);
      // histo[7]->SetBinContent(ipkt, nspiroclk03);
      // histo[8]->SetBinContent(ipkt, nspiroclk47);
    }

  const string hname[NHIST] =
    {
      "SpiroEvtCnt" ,
      "SpiroALock",
      "SpiroBLock" ,
      "SpiroASizeErr" ,
      "SpiroBSizeErr" ,
      "SpiroParity" ,
      "SpiroEvtNo"
      // "SpiroClk03" ,
      // "SpiroClk47"
    };

  const string desc[NHIST] =
    {
      "SpiroEvtCnt" ,
      "SpiroALock",
      "SpiroBLock" ,
      "SpiroASizeErr" ,
      "SpiroBSizeErr" ,
      "SpiroParity" ,
      "SpiroEvtNo"
      // "SpiroClk03" ,
      // "SpiroClk47"
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
  for (short int ihist = 0; ihist < NHIST; ihist++)
    {
      HName = "Daq_" + HFirstName + hname[ihist];
      Pad1[ihist]->cd();
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
  transparent1->cd();
  transparent1->SetEditable(kTRUE);
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
  transparent1->Clear();
  time_t evttime = cl->EventTime("CURRENT");
  datestream << "Date: " << ctime(&evttime);
  PrintRun.DrawText(0.5, 0.99, runnostream.str().c_str());
  PrintRun.SetTextSize(0.05);
  PrintRun.SetTextColor(4);
  PrintRun.DrawText(0.5, 0.93, datestream.str().c_str());

  TC1->Update();
  PutBackVtxpKnownBad(subsyspacket);

  return 0;
}

int
VtxpMonDraw::MakeCanvas(std::string const &name)
{
  if (name == "DaqVtxpMon1")
    {
      TC1 = new TCanvas("DaqVtxpMon1", "DaqMon Vtxp", -1, 0, 800, 900);
      gSystem->ProcessEvents();
      transparent1 = new TPad("vtxptransparent1", "this does not show", 0, 0, 1, 1);
      transparent1->SetFillStyle(4000);
      transparent1->Draw();
      transparent1->SetEditable(kFALSE);
      char padname[100];
      int ipad = 0;
      for (short int i = 0; i < VTXPLASTBIN / 2 + (VTXPLASTBIN & 0x1); i++)
	{
	  // up to 0,9 to make space for the run number
	  double ylow = 0.9 * (1 - (i + 1) / 6.);
	  double yhi = 0.9 * (1 - i / 6.);
	  for (short int j = 0; j < 2; j++)
	    {
	      double xlow = j * 1. / 2.;
	      double xhi = (j + 1) / 2.;
	      sprintf(padname, "VtxpPad1%02d", ipad);
	      Pad1[ipad] = new TPad(padname, "this does not show", xlow, ylow, xhi, yhi, 0);
	      Pad1[ipad]->SetFillStyle(4000);
	      Pad1[ipad]->Draw();
	      ipad++;
	    }
	}
      return 0;
    }
  return -1;
}

void
VtxpMonDraw::identify(ostream& out) const
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
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  static int iwid = 15;
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
  out << setw(iwid) << "SpEvtCnt";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpALock";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpBLock";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpASize";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpBSize";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpParity";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpEvtNo";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpClk03";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "SpClk47";
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
      int nspiroevtcnt = (int) subsyspacket->GetBinContent(ibin, SPIROEVTCNTBIN);
      int nspiroaloss =  (int) subsyspacket->GetBinContent(ibin, SPIRO_ALOSSLOCKBIN);
      int nspirobloss =  (int) subsyspacket->GetBinContent(ibin, SPIRO_B_LOSSLOCKBIN);
      int nspiroasizeerr = (int) subsyspacket->GetBinContent(ibin, SPIRO_ASIZEERRORBIN);
      int nspirobsizeerr = (int) subsyspacket->GetBinContent(ibin, SPIRO_B_SIZEERRORBIN);
      int nspiroparity = (int) subsyspacket->GetBinContent(ibin, SPIROPARITYBIN);
      int nspiroevtno = (int) subsyspacket->GetBinContent(ibin, SPIROEVTNOBIN);
      int nspiroclk03  = (int) subsyspacket->GetBinContent(ibin, SPIROCLK03BIN);
      int nspiroclk47  = (int) subsyspacket->GetBinContent(ibin, SPIROCLK47BIN);
      int npackets  = (int) monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      int nval[9];
      nval[0] = nspiroevtcnt;
      nval[1] = nspiroaloss;
      nval[2] = nspirobloss;
      nval[3] = nspiroasizeerr;
      nval[4] = nspirobsizeerr;
      nval[5] = nspiroparity;
      nval[6] = nspiroevtno;
      nval[7] = nspiroclk03;
      nval[8] = nspiroclk47;
      if (htmloutflag)
	{
	  out << "<tr><td align=center> ";
	}
      if (nspiroevtcnt > 0 || nspiroaloss > 0 || nspirobloss > 0 || nspiroasizeerr > 0 || nspirobsizeerr > 0 || nspiroparity > 0
	  || nspiroevtno > 0)
	{
	  SetHtmlColor(out, 1);
	}
      else
	{
	  SetHtmlColor(out, 0);
	}
      out << setw(iwid) << ((int) monpkt->GetBinContent(ibin));
      EndFont(out);
      for (int i = 0; i < 9; i++)
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

  return;
}

int
VtxpMonDraw::MakeHtml(const string &what)
{
  DrawThisSubsystem(what.c_str());
  OnlMonClient *cl = OnlMonClient::instance();
  DaqMonDraw *daqdraw = DaqMonDraw::instance();
  string imgpath = cl->htmlRegisterPage(*daqdraw, "VtxPixel", "vtxpixel", "png");
  cl->CanvasToPng(TC1, imgpath);
  return 0;
}

int
VtxpMonDraw::SetBadPacket(const int PacketId, const string &what)
{
  // mapping of error counter to histogram channel 
  map<string, int> histochannel;
  histochannel["SpiroEvtCnt"] = SPIROEVTCNTBIN;
  histochannel["SpiroALock"] = SPIRO_ALOSSLOCKBIN;
  histochannel["SpiroBLock"] = SPIRO_B_LOSSLOCKBIN;
  histochannel["SpiroASizeErr"] = SPIRO_ASIZEERRORBIN;
  histochannel["SpiroBSizeErr"] = SPIRO_B_SIZEERRORBIN;
  histochannel["SpiroParity"] = SPIROPARITYBIN;
  histochannel["SpiroEvtNo"] = SPIROEVTNOBIN;
  // histochannel["SpiroClk03"] = SPIROCLK03BIN;
  // histochannel["SpiroClk47"] = SPIROCLK47BIN;
   map<string, int>::const_iterator iter =  histochannel.find(what);
  if (iter != histochannel.end())
    {
          cout << "Vtxp: Setting Packet " <<  PacketId
               << " Entry " << iter->first
               << " to known bad" << endl;
	  SetVtxpBadPacket(PacketId,iter->second);
	  return 0;
    }
  int iret = GranMonDraw::SetBadPacket(PacketId, what);
  return iret;
}

int
VtxpMonDraw::SetVtxpBadPacket(const int PacketId, const int ibin)
{
  map< int, vector<badpacket> >::iterator baditer = vtxpbadpackets.find(PacketId);
  vector<badpacket>::iterator biniter;
  struct badpacket newbadpacket;
  newbadpacket.badbin = ibin;
  newbadpacket.savechannel = -1;
  newbadpacket.packetbin = -1;
  newbadpacket.savegood = -1;
  if (baditer != vtxpbadpackets.end())
    {
      for (biniter = baditer->second.begin(); biniter != baditer->second.end();
           ++biniter)
        {
          if (biniter->badbin == ibin)
            {
              cout << "Allready added this bad bin " << ibin << endl;
              return 0;
            }
        }
      baditer->second.push_back(newbadpacket);
      return 0;
    }
  vector <badpacket> badguys;
  badguys.push_back(newbadpacket);
  vtxpbadpackets[PacketId] = badguys;
  return 0;
}

int
VtxpMonDraw::Print(const char* what) const
{
  if (!strcmp(what, "ALL"))
    {
      cout << "VTXP Bad Packet and histo bin list:" << endl << endl;
      map< int, vector<badpacket> >::const_iterator baditer;
      vector<badpacket>::const_iterator biniter;
      for (baditer = vtxpbadpackets.begin(); baditer != vtxpbadpackets.end();
           ++baditer)
        {
          for (biniter = baditer->second.begin(); biniter != baditer->second.end();
               ++biniter)
            {
              cout << "PacketId: " << baditer->first
                   << " bad bin: " << biniter->badbin << endl;
            }
        }
    }
  GranMonDraw::Print(what);
  return 0;
}

int
VtxpMonDraw::RemoveVtxpKnownBad(TH1 *monhis, TH1 *monpkt)
{
  map< int, vector<badpacket> >::iterator baditer;
  vector<badpacket>::iterator biniter;
  int ipktbin = -1;
  for (baditer = vtxpbadpackets.begin(); baditer != vtxpbadpackets.end();
       ++baditer)
    {
      int packetid = baditer->first;
      for (int i = 1; i <= monpkt->GetNbinsX(); i++)
	{
	  if ((int) monpkt->GetBinContent(i) == packetid)
	    {
	      ipktbin = i;
	      break;
	    }
	}
      //            cout << "packetid: " << packetid << ", ipktbin: " << ipktbin
      //  	       << ", reread: " << monpkt->GetBinContent(ipktbin) << endl;
      if (ipktbin <= 0)
	{
	  cout << "Could not find packet : " << packetid << endl;
	  continue;
	}
      for (biniter = baditer->second.begin(); biniter != baditer->second.end();
	   ++biniter)
	{
	  cout << "PacketId: " << baditer->first
	       << " bad bin: " << biniter->badbin
	       << ", Hcont: " << monhis->GetBinContent(ipktbin, biniter->badbin) << endl;
	  biniter->packetbin = ipktbin;
	  int channelcontent = (int) monhis->GetBinContent(ipktbin, biniter->badbin);
	  if (channelcontent < 0)
	    {
	      channelcontent = 0;
	    }
	  biniter->savechannel = channelcontent;
	  monhis->SetBinContent(ipktbin, biniter->badbin, 0.);
	}
    }
  return 0;
}

int
VtxpMonDraw::PutBackVtxpKnownBad(TH1 *monhis)
{
  map< int, vector<badpacket> >::const_iterator baditer;
  vector<badpacket>::const_iterator biniter;
  for (baditer = vtxpbadpackets.begin(); baditer != vtxpbadpackets.end();
       ++baditer)
    {
      for (biniter = baditer->second.begin(); biniter != baditer->second.end();
	   ++biniter)
        {
	  monhis->SetBinContent(biniter->packetbin , biniter->badbin, biniter->savechannel);
	  cout << "Restore PacketId: " << baditer->first
	       << " bad bin: " << biniter->badbin
	       << ", Hcont: " << monhis->GetBinContent(biniter->packetbin, biniter->badbin) << endl;
	}
    }
  return 0;
}
