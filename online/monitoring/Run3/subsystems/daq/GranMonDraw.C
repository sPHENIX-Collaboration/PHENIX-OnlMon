#include "GranMonDraw.h"
#include "DaqMonDraw.h"
#include "GranuleMonDefs.h"

#include <OnlMonClient.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TROOT.h>
#include <TText.h>

#include <iomanip>
#include <iostream>

using namespace std;

GranMonDraw::GranMonDraw(const char *granule):
  ThisName(granule),
  htmloutflag(0)
{}

GranMonDraw::~GranMonDraw()
{
  while (badpackets.begin() != badpackets.end())
    {
      badpackets.erase(badpackets.begin());
    }
  return;
}

int
GranMonDraw::Init()
{
  DaqMonDraw *daqmon = DaqMonDraw::instance();
  string outhis = "Daq_" + ThisName + "GranStatus";
  TH1 *GranuleOut = new TH1F(outhis.c_str(), ThisName.c_str(), 14, 0, 14);
  daqmon->registerHisto(outhis.c_str(), GranuleOut);
  return 0;
}

int
GranMonDraw::Draw(const char *what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string monivars = "Daq_" + ThisName + "AllVariables";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *monpkt = cl->getHisto(monpkts.c_str());
  int iret = -2;
  if (!monvars)
    {
      cout << "No " << monivars.c_str() << " Histogram" << endl;
      return -1;
    }
  if (!monhis)
    {
      cout << "No " << monhisname.c_str() << " Histogram" << endl;
      return -2;
    }
  if (!monpkt)
    {
      cout << "No " << monpkts.c_str() << " Histogram" << endl;
      return -2;
    }
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  RemoveKnownBad(monhis, monpkt, currentbin);
  if (!strcmp(what, "ALL"))
    {
      iret = DrawAll(what);
    }
  else if (!strcmp(what, Name()))
    {
      iret = DrawThisGranule(what);
    }
  PutBackKnownBad(monhis, currentbin);
  return iret;
}

int
GranMonDraw::DrawThisGranule(const char *what)
{
#define COLBAD 2
#define COLGOOD 3
#define XLABELSIZE 0.09
#define YLABELSIZE 0.09

  OnlMonClient *cl = OnlMonClient::instance();
  string monpkts = "Daq_" + ThisName + "PacketId";
  string monivars = "Daq_" + ThisName + "AllVariables";
  string monhisname = "Daq_" + ThisName + "Monitor";
  TH1 *monpacket = cl->getHisto(monpkts.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  int NPacketThisGranule = monpacket->GetNbinsX();
  int packetmin = (int) monpacket->GetBinContent(1);
  int packetmax = (int) monpacket->GetBinContent(NPacketThisGranule);

  // since we don't know a priory how many packets we have in this Granule
  // we need to create the histograms here after getting the number of
  // packets and their id's
  string histoPacketSum = "Daq_" + ThisName + "PacketSum";
  TH1 *sumpackets = MakeSnglHisto(histoPacketSum, packetmin, packetmax);
  sumpackets->SetFillColor(COLGOOD);

  string histoPacketFemGl1 = "Daq_" + ThisName + "FemGl1Sync";
  TH1 *femgl1packets = MakeSnglHisto(histoPacketFemGl1, packetmin, packetmax);
  string histoPacketFemClk = "Daq_" + ThisName + "FemClk";
  TH1 *femclkpackets = MakeSnglHisto(histoPacketFemClk, packetmin, packetmax);
  string histoPacketFemPar = "Daq_" + ThisName + "FemParity";
  TH1 *femparpackets = MakeSnglHisto(histoPacketFemPar, packetmin, packetmax);
  string histoDcmFemPar = "Daq_" + ThisName + "DcmFemParity";
  TH1 *dcmfemparpackets = MakeSnglHisto(histoDcmFemPar, packetmin, packetmax);
  string histoPacketFemEvt = "Daq_" + ThisName + "FemEvtNo";
  TH1 *femevtpackets = MakeSnglHisto(histoPacketFemEvt, packetmin, packetmax);
  string histoPacketDcmChksum = "Daq_" + ThisName + "DcmChkSum";
  TH1 *dcmchksumpackets = MakeSnglHisto(histoPacketDcmChksum, packetmin, packetmax);
  string histoPacketBadlength = "Daq_" + ThisName + "BadLength";
  TH1 *badlengthpackets = MakeSnglHisto(histoPacketBadlength, packetmin, packetmax);
  string histoPacketGlinkError = "Daq_" + ThisName + "GlinkError";
  TH1 *glinkErrorpackets = MakeSnglHisto(histoPacketGlinkError, packetmin, packetmax);
  string histoPacketSubsyserror = "Daq_" + ThisName + "SubsysError";
  TH1 *subsyserrorpackets = MakeSnglHisto(histoPacketSubsyserror, packetmin, packetmax);
  string histoPacketMissing = "Daq_" + ThisName + "Missing";
  TH1 *packetsmissing = MakeSnglHisto(histoPacketMissing, packetmin, packetmax);

  string histoPacketBytes = "Daq_" + ThisName + "Bytes";
  TH1 *packetsBytes = MakeSnglHisto(histoPacketBytes, packetmin, packetmax);
  packetsBytes->SetFillColor(COLGOOD);

  for (int ibin = 1; ibin <= monpacket->GetNbinsX(); ibin++)
    {
      int ipkt = (int) monpacket->GetBinContent(ibin) - packetmin + 1; // first bin is 1
      double npackets = monhis->GetBinContent(currentbin, ibin, NPACKETBIN);
      double missingpkts = monhis->GetBinContent(currentbin, ibin, MISSINGPKTBIN);
      packetsmissing->SetBinContent(ipkt, missingpkts);
      double nbytes = monhis->GetBinContent(currentbin, ibin, SUMBYTESBIN);
      if (missingpkts > 0)
	{
          npackets -=  missingpkts; // subtract the number of missing packts from npackets
	}
      sumpackets->SetBinContent(ipkt, npackets);
      double bytesperpacket;
      if (npackets > 0)
        {
          bytesperpacket = nbytes / npackets;
        }
      else
        {
          bytesperpacket = 0;
        }
      packetsBytes->SetBinContent(ipkt, bytesperpacket);
      if (npackets <= 0)
	{
	  npackets = 1;
	}
      double femgl1clkerr = (monhis->GetBinContent(currentbin, ibin, FEMGL1CLKBIN)/npackets)*100.;
      femgl1clkerr *= 2; // we only see every 2nd fem/gl1 counter mismatch
      femgl1packets->SetBinContent(ipkt, femgl1clkerr);
      double femclkerr = (monhis->GetBinContent(currentbin, ibin, FEMCLKBIN)/npackets)*100.;
      femclkpackets->SetBinContent(ipkt, femclkerr);
      double femparity = (monhis->GetBinContent(currentbin, ibin, FEMPARITYBIN)/npackets)*100.;
      femparpackets->SetBinContent(ipkt, femparity);
      double dcmfemparity = (monhis->GetBinContent(currentbin, ibin, DCMFEMPARBIN)/npackets)*100.;
      dcmfemparpackets->SetBinContent(ipkt, dcmfemparity);
      femevtpackets->SetBinContent(ipkt, (monhis->GetBinContent(currentbin, ibin, FEMEVTSEQBIN)/npackets)*100.);
      dcmchksumpackets->SetBinContent(ipkt, (monhis->GetBinContent(currentbin, ibin, DCMCHKSUMBIN)/npackets)*100.);
      badlengthpackets->SetBinContent(ipkt, (monhis->GetBinContent(currentbin, ibin, LENGTHBIN)/npackets)*100.);
      glinkErrorpackets->SetBinContent(ipkt, (monhis->GetBinContent(currentbin, ibin, GLINKBIN)/npackets)*100.);
      double subsyserr = (monhis->GetBinContent(currentbin, ibin, SUBSYSTEMBIN)/npackets)*100.;
      subsyserrorpackets->SetBinContent(ipkt, subsyserr);
    }
  return 0;
}

int
GranMonDraw::DrawAll(const char *what)
{
  DaqMonDraw *daqmon = DaqMonDraw::instance();
  OnlMonClient *cl = OnlMonClient::instance();
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monivars = "Daq_" + ThisName + "AllVariables";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  TH1 *monvars = cl->getHisto(monivars.c_str());
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  double nFatalErrors = 0;
  int badguy;
  if (monvars->GetBinContent(GL1FATALBIN) > 0)
    {
      nFatalErrors = monvars->GetBinContent(GL1FATALBIN);
      badguy = 1;
    }
  else if (monvars->GetBinContent(SCALEDFATALBIN) > 0)
    {
      nFatalErrors = monvars->GetBinContent(SCALEDFATALBIN);
      badguy = 4;
    }
  else if (monvars->GetBinContent(SUBSYSFATALBIN) > 0)
    {
      nFatalErrors = monvars->GetBinContent(SUBSYSFATALBIN);
      badguy = 2;
    }
  else if (monvars->GetBinContent(DROPCONTENTBIN) > 0)
    {
      nFatalErrors = monvars->GetBinContent(DROPCONTENTBIN);
      badguy = 3;
    }
  else if (monvars->GetBinContent(EVBERRORBIN) > 0)
    {
      nFatalErrors = monvars->GetBinContent(EVBERRORBIN);
      badguy = 5;
    }
  else
    {
      badguy = 0;
    }
  double GL1sum = 0.;
  double Dcmsum = 0.;
  double femevtsum = 0.;
  double femparsum = 0.;
  double dcmfemparsum = 0.;
  double femclksum = 0.;
  double badlength = 0.;
  double glinksum = 0;
  double subsyssum = 0;
  double ngoodpackets = 0;
  double ntotpkts = 0;
  double missingsum = 0;
  for (int ipkt = 1; ipkt <= monhis->GetNbinsY(); ipkt++)
    {
      GL1sum += monhis->GetBinContent(currentbin, ipkt, FEMGL1CLKBIN)*2.; // we see only every 2nd fem/gl1 counter mismatch
      femclksum += monhis->GetBinContent(currentbin, ipkt, FEMCLKBIN);
      femparsum += monhis->GetBinContent(currentbin, ipkt, FEMPARITYBIN);
      dcmfemparsum += monhis->GetBinContent(currentbin, ipkt, DCMFEMPARBIN);
      femevtsum += monhis->GetBinContent(currentbin, ipkt, FEMEVTSEQBIN);
      Dcmsum += monhis->GetBinContent(currentbin, ipkt, DCMCHKSUMBIN);
      badlength += monhis->GetBinContent(currentbin, ipkt, LENGTHBIN);
      glinksum += monhis->GetBinContent(currentbin, ipkt, GLINKBIN);
      subsyssum += monhis->GetBinContent(currentbin, ipkt, SUBSYSTEMBIN);
      missingsum += monhis->GetBinContent(currentbin, ipkt, MISSINGPKTBIN);
      ngoodpackets += monhis->GetBinContent(currentbin, ipkt, NGOODPACKETBIN);
      ntotpkts += monhis->GetBinContent(currentbin, ipkt, NPACKETBIN);
    }
  double nbadpkts = ntotpkts - ngoodpackets;
  string outhis = "Daq_" + ThisName + "GranStatus";
  TH1 *GranuleOut = daqmon->getHisto(outhis.c_str());
  GranuleOut->Reset();
  GranuleOut->SetBinContent(1, ngoodpackets);
  GranuleOut->SetBinContent(2, nbadpkts);
  GranuleOut->SetBinContent(3, GL1sum);
  GranuleOut->SetBinContent(4, femclksum);
  GranuleOut->SetBinContent(5, femparsum);
  GranuleOut->SetBinContent(6, femevtsum);
  GranuleOut->SetBinContent(7, Dcmsum);
  GranuleOut->SetBinContent(8, badlength);
  GranuleOut->SetBinContent(9, dcmfemparsum);
  GranuleOut->SetBinContent(10, glinksum);
  GranuleOut->SetBinContent(11, subsyssum);
  GranuleOut->SetBinContent(12, missingsum);
  GranuleOut->SetBinContent(13, nFatalErrors);
  GranuleOut->SetBinContent(14, badguy);
  GranuleOut->SetTitle(ThisName.c_str());
  return 0;
}

void
GranMonDraw::identify(ostream& out) const
{
  OnlMonClient *cl = OnlMonClient::instance();
  string monhisname = "Daq_" + ThisName + "Monitor";
  string monpkts = "Daq_" + ThisName + "PacketId";
  string monivars = "Daq_" + ThisName + "AllVariables";
  TH1 *monhis = cl->getHisto(monhisname.c_str());
  if (!monhis)
    {
      out << "No " << monhisname.c_str() << " Histogram" << endl;
      return ;
    }
  TH1 *monpkt = cl->getHisto(monpkts.c_str());
  if (!monpkt)
    {
      out << "No " << monpkts.c_str() << " Histogram" << endl;
      return ;
    }
  TH1 *monvars = cl->getHisto(monivars.c_str());
  if (!monvars)
    {
      out << "No " << monivars.c_str() << " Histogram" << endl;
      return ;
    }
  int currentbin = (int) monvars->GetBinContent(LASTFILLEDBIN);
  static int iwid = 11;
  if (htmloutflag)
    {
      out << "<font color=black> ";
    }
  out << "GranuleMon Object, Name: " << ThisName;
  if (htmloutflag)
    {
      out << "</br>";
    }
  out  << endl;
  out << "Number of packets: " << monhis->GetNbinsY();
  if (htmloutflag)
    {
      out << "</br>";
    }
  out << endl;
  if (htmloutflag)
    {
      out << "<table align=center> <tr><td align=center>";
    }
  out << setw(iwid) << "Pkt";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "LenErr";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "DcmChkSum";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "FEMPar";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "DcmFEMPar";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "FEMClk";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "FEMGL1Clk";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "FEMEvtSeq";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "GlinkErr";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "Subsys";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "Miss";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "NPkts";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "GoodPkt";
  if (htmloutflag)
    {
      out << "</td><td align=center>";
    }
  out << setw(iwid) << "Byt/pkt";
  if (htmloutflag)
    {
      out << "</td></tr></font> ";
    }
  out << endl;

  for (int ipkt = 1; ipkt <= monhis->GetNbinsY(); ipkt++)
    {
      int lengtherr = (int) monhis->GetBinContent(currentbin, ipkt, LENGTHBIN);
      int dcmerr = (int) monhis->GetBinContent(currentbin, ipkt, DCMCHKSUMBIN);
      int parityerr = (int) monhis->GetBinContent(currentbin, ipkt, FEMPARITYBIN);
      int femclkerr = (int) monhis->GetBinContent(currentbin, ipkt, FEMCLKBIN);
      int gl1clkerr = (int) monhis->GetBinContent(currentbin, ipkt, FEMGL1CLKBIN);
      int evtnoerr = (int) monhis->GetBinContent(currentbin, ipkt, FEMEVTSEQBIN);
      int dcmfemparerr = (int) monhis->GetBinContent(currentbin, ipkt, DCMFEMPARBIN);
      int glinkerr = (int) monhis->GetBinContent(currentbin, ipkt, GLINKBIN);
      int subsyserr = (int) monhis->GetBinContent(currentbin, ipkt, SUBSYSTEMBIN);
      int missing = (int) monhis->GetBinContent(currentbin, ipkt, MISSINGPKTBIN);
      double npackets = monhis->GetBinContent(currentbin, ipkt, NPACKETBIN);
      int BytesPerPacket;
      if (npackets > 0)
        {
          BytesPerPacket = (int) (monhis->GetBinContent(currentbin, ipkt, SUMBYTESBIN) / npackets);
        }
      else
        {
          BytesPerPacket = 0;
        }
      if (htmloutflag)
        {
          out << "<tr><td align=center> ";
        }
      if (lengtherr > 0 || dcmerr > 0 || parityerr > 0 || femclkerr > 0 || gl1clkerr > 0 || evtnoerr > 0
          || dcmfemparerr > 0 || glinkerr > 0 || subsyserr > 0 || missing > 0 || npackets <= 0)
        {
          SetHtmlColor(out, 1);
        }
      else
        {
          SetHtmlColor(out, 0);
        }
      out << setw(iwid) << ((int) monpkt->GetBinContent(ipkt));
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, lengtherr);
      out << setw(iwid) << lengtherr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, dcmerr);
      out << setw(iwid) << dcmerr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, parityerr);
      out << setw(iwid) << parityerr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, dcmfemparerr);
      out << setw(iwid) << dcmfemparerr;
      EndFont(out);

      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, femclkerr);
      out << setw(iwid) << femclkerr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, gl1clkerr);
      out << setw(iwid) << gl1clkerr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, evtnoerr);
      out << setw(iwid) << evtnoerr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, glinkerr);
      out << setw(iwid) << glinkerr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, subsyserr);
      out << setw(iwid) << subsyserr;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      SetHtmlColor(out, missing);
      out << setw(iwid) << missing;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }
      if (npackets <= 0)
        {
          SetHtmlColor(out, 1);
        }
      else
        {
          if (htmloutflag)
            {
              out << "<font color=black> ";
            }
        }
      out << setw(iwid) << (int) npackets;
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
          out << "<font color=black> ";
        }
      out << setw(iwid) << ((int) monhis->GetBinContent(currentbin, ipkt, NGOODPACKETBIN));
      EndFont(out);
      if (htmloutflag)
        {
          out << "</td><td align=center>";
        }

      out << setw(iwid) << BytesPerPacket;
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
  return ;
}

void
GranMonDraw::SetHtmlColor(ostream &out, int error) const
{
  if (!htmloutflag)
    {
      return ;
    }
  if (error > 0)
    {
      out << "<font color=red> ";
    }
  else
    {
      out << "<font color=green> ";
    }
  return ;
}

void
GranMonDraw::EndFont(ostream &out) const
{
  if (!htmloutflag)
    {
      return ;
    }
  out << "</font> ";
  return;
}

int
GranMonDraw::SetBadPacket(const int PacketId, const int ibin)
{
  map< int, vector<badpacket> >::iterator baditer = badpackets.find(PacketId);
  vector<badpacket>::iterator biniter;
  struct badpacket newbadpacket;
  newbadpacket.badbin = ibin;
  newbadpacket.savechannel = -1;
  newbadpacket.packetbin = -1;
  newbadpacket.savegood = -1;
  if (baditer != badpackets.end())
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
  badpackets[PacketId] = badguys;
  return 0;
}

int
GranMonDraw::Print(const char* what) const
{
  if (!strcmp(what, "ALL"))
    {
      cout << "Bad Packet and histo bin list:" << endl << endl;
      map< int, vector<badpacket> >::const_iterator baditer;
      vector<badpacket>::const_iterator biniter;
      for (baditer = badpackets.begin(); baditer != badpackets.end();
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
  return 0;
}

int
GranMonDraw::RemoveKnownBad(TH1 *monhis, TH1 *monpkt, const int currentbin)
{
  map< int, vector<badpacket> >::iterator baditer;
  vector<badpacket>::iterator biniter;
  int ipktbin = -1;
  for (baditer = badpackets.begin(); baditer != badpackets.end();
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
      int ngood = (int) monhis->GetBinContent(currentbin, ipktbin, NGOODPACKETBIN);
      int ntotal = (int) monhis->GetBinContent(currentbin, ipktbin, NPACKETBIN);
      for (biniter = baditer->second.begin(); biniter != baditer->second.end();
	   ++biniter)
	{
	  //                cout << "PacketId: " << baditer->first
	  //  		   << " bad bin: " << biniter->badbin
	  //  		   << ", Hcont: " << monhis->GetBinContent(currentbin, ipktbin, biniter->badbin)
	  //  		   << ", good: " << monhis->GetBinContent(currentbin, ipktbin, NGOODPACKETBIN ) << endl;
	  biniter->packetbin = ipktbin;
	  biniter->savegood = ngood;
	  int channelcontent = (int) monhis->GetBinContent(currentbin, ipktbin, biniter->badbin);
	  if (channelcontent < 0)
	    {
	      channelcontent = 0;
	    }
	  biniter->savechannel = channelcontent;
	  monhis->SetBinContent(currentbin, ipktbin, biniter->badbin, 0.);
	  // this is a rough estimate of bad packets here
	  int ngoodtmp = (int) monhis->GetBinContent(currentbin, ipktbin, NGOODPACKETBIN);
	  ngoodtmp += biniter->savechannel;
	  if (ngoodtmp > ntotal)
	    {
	      ngoodtmp = ntotal;
	    }
	  monhis->SetBinContent(currentbin, ipktbin, NGOODPACKETBIN , ngoodtmp);
	}
    }
  return 0;
}

int
GranMonDraw::PutBackKnownBad(TH1 *monhis, const int currentbin)
{
  map< int, vector<badpacket> >::const_iterator baditer;
  vector<badpacket>::const_iterator biniter;
  for (baditer = badpackets.begin(); baditer != badpackets.end();
       ++baditer)
    {
      for (biniter = baditer->second.begin(); biniter != baditer->second.end();
	   ++biniter)
        {
	  monhis->SetBinContent(currentbin, biniter->packetbin , biniter->badbin, biniter->savechannel);
	  monhis->SetBinContent(currentbin, biniter->packetbin , NGOODPACKETBIN, biniter->savegood);
	  //               cout << "Restore PacketId: " << baditer->first
	  // 		   << " bad bin: " << biniter->badbin
	  //               << ", Hcont: " << monhis->GetBinContent(currentbin, biniter->packetbin, biniter->badbin)
	  //               << ", good: " << monhis->GetBinContent(currentbin, biniter->packetbin, NGOODPACKETBIN) << endl;
        }
    }
  return 0;
}

int
GranMonDraw::SetBadPacket(const int PacketId, const string &what)
{
  // mapping of error counter to histogram channel 
  map<string, int> histochannel;
  histochannel["LengthErr"] = LENGTHBIN;
  histochannel["DcmChkSum"] = DCMCHKSUMBIN;
  histochannel["ParityErr"] = FEMPARITYBIN;
  histochannel["FemClkErr"] = FEMCLKBIN;
  histochannel["Gl1SyncErr"] = FEMGL1CLKBIN;
  histochannel["EvtNoErr"] = FEMEVTSEQBIN;
  histochannel["DCMParErr"] = DCMFEMPARBIN;
  histochannel["GlinkErr"] = GLINKBIN;
  histochannel["SubsysErr"] = SUBSYSTEMBIN;
  histochannel["Missing"] = MISSINGPKTBIN;
  map<string, int>::const_iterator iter =  histochannel.find(what);
  if (iter != histochannel.end())
    {
          cout << "Setting Packet " <<  PacketId
               << " Entry " << iter->first
               << " to known bad" << endl;
	  SetBadPacket(PacketId,iter->second);
	  return 0;
    }
  cout << "SetBadPacket: Could not find " << what
       << ", possible strings are:" << endl;
  for (iter = histochannel.begin(); iter != histochannel.end(); ++iter)
    {
      cout << iter->first  << endl;
    }
  return -1;
}

TH1 *
GranMonDraw::MakeSnglHisto(const std::string &histoname, const int packetmin, const int packetmax)
{
  DaqMonDraw *daqmon = DaqMonDraw::instance();
  TH1 *histo = daqmon->getHisto(histoname.c_str());
  if (!histo)
    {
      int nbins = packetmax - packetmin + 1;
      histo = new TH1F(histoname.c_str(),
                       "",
                       nbins, (packetmin - 0.5), (packetmax + 0.5));
      histo->SetFillColor(COLBAD);
      histo->SetStats(0);
      histo->SetLabelSize(YLABELSIZE, "Y");
      histo->SetLabelSize(XLABELSIZE, "X");
      histo->GetXaxis()->SetNdivisions(505);
      daqmon->registerHisto(histoname.c_str(), histo);
    }
  else
    {
      histo->Reset();
    }
  return histo;
}
