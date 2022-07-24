#include "DaqVtxpMon.h"
#include "DaqVtxpMonDefs.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <OnlMonServer.h>

#include <msg_profile.h>
#include <Event.h>
#include <packet.h>

#include <TH2F.h>

#include <boost/foreach.hpp>

#include <cstdlib>
#include <sstream>

using namespace std;

DaqVtxpMon::DaqVtxpMon(const string &arm):
  h2status(NULL)
{
  if (arm == "VTXP")
    {
      ThisName = "VTXP";
    }
  else
    {
      cout << "invalid arm: " << arm << endl;
      exit(-2);
    }
  return ;
}

int
DaqVtxpMon::Init()
{
  unsigned int ipktmin;
  unsigned int ipktmax;
  if (ThisName == "VTXP")
    {
      GranuleNumber = GRAN_NO_VTXP;
      ipktmin = 24001;
      ipktmax = 24060;
    }
  else
    {
      cout << "invalid subsystem name: " << Name() << endl;
      exit(1);
    }
  for (unsigned int ipkt = ipktmin; ipkt <= ipktmax; ipkt++)
    {
      packetnumbers.insert(ipkt);
    }
  GranuleMon::Init();
  return 0;
}

int
DaqVtxpMon::DcmFEMParityErrorCheck()
{
  if(p->iValue(0, "PARITYOK"))
    {
      return 0;
    }
  else
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> DcmFEMParityErrorCheck: "
		 << "Packet: " << packetiter->PacketId;
	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 21);
	}
      packetiter->DcmFEMParityError++;
      Gl1SyncBad.insert(packetiter->PacketId);
    }
  return -1;
}

int
DaqVtxpMon::GlinkCheck()
{
  packetiter->GlinkError = -1;
  return 0;
}

void
DaqVtxpMon::SetBeamClock()
{
  BeamClock = p->iValue(0, "BCLCK");
  return;
}

int
DaqVtxpMon::SubsystemCheck(Event *evt, const int iwhat)
{
  int iret = 0;
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return iret;
    }
  int have_subsystem_error = 0;
  int spiroevncnt = p->iValue(0, "SPIROEVENTCOUNT");
  map<unsigned int, vtxppacketerrors>::iterator mypktiter = mypkt.find(packetiter->PacketId);
  if (mypktiter == mypkt.end())
    {
      vtxppacketerrors pkterr(packetiter->PacketId);
      mypkt[packetiter->PacketId] = pkterr;
      mypktiter = mypkt.find(packetiter->PacketId);
    }

  if (spiroevncnt > 4)
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> SubsystemCheck: "
		 << "Packet: " << packetiter->PacketId
		 << " buffered evts: " << spiroevncnt;
	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 21);
	}
      mypktiter->second.spirobadcount++;
      have_subsystem_error = 1;
      iret = -1;
    }
  // chuck's status word, lower 4 bits are for number of buffered events
  int status = (p->iValue(0, "STATUS")) & 0xFFF0;
  if (status)
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> SubsystemCheck: "
		 << "Packet: " << packetiter->PacketId
		 << hex << " status: 0x" << status << dec;
	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 22);
	}
      have_subsystem_error = 1;
      iret = -1;
      if (p->iValue(0, "SPIRO_A_LOSSLOCK"))
	{
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> SubsystemCheck: "
		     << "Packet: " << packetiter->PacketId
		     << " Spiro A lossed lock";
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 23);
	    }
	  mypktiter->second.spiroAlosslock++;
	}
      if (p->iValue(0, "SPIRO_B_LOSSLOCK"))
	{
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> SubsystemCheck: "
		     << "Packet: " << packetiter->PacketId
		     << " Spiro B lossed lock";
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 24);
	    }
	  mypktiter->second.spiroBlosslock++;
	}
      if (p->iValue(0, "SPIRO_A_SIZEERROR"))
	{
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> SubsystemCheck: "
		     << "Packet: " << packetiter->PacketId
		     << " Spiro A size error";
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 25);
	    }
	  mypktiter->second.spiroAsizeerror++;
	}
      if (p->iValue(0, "SPIRO_B_SIZEERROR"))
	{
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> SubsystemCheck: "
		     << "Packet: " << packetiter->PacketId
		     << " Spiro B size error";
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 26);
	    }
	  mypktiter->second.spiroBsizeerror++;
	}
      if (p->iValue(0, "SPIROPARITYERROR"))
	{
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> SubsystemCheck: "
		     << "Packet: " << packetiter->PacketId
		     << " Spiro parity error";
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 27);
	    }
	  mypktiter->second.spiroparity++;
	}
    }
  // now check event numbers of spiro boards if they agree
  int spiroevt03 = p->iValue(0, "EVTNR03");
  int spiroevt47 = p->iValue(0, "EVTNR47");
  if (spiroevt03 != spiroevt47)
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> SubsystemCheck: "
		 << "Packet: " << packetiter->PacketId
		 << " Spiro event number mismatch: "
		 << hex << " spiroevt0-3: 0x" << spiroevt03
		 << " spiroevt4-7: 0x" << spiroevt47
		 << dec;
	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 28);
	}
      have_subsystem_error = 1;
      iret = -1;
      mypktiter->second.spiroeventnum++;
    }
  // check beam clock and negated beam clock for consistency
  int beamclk03 = p->iValue(0, "S_BCLCK03");
  int beamclk47 = p->iValue(0, "S_BCLCK47");
  int negbeamclk03 = p->iValue(0, "NEGBCLK03");
  int negbeamclk47 = p->iValue(0, "NEGBCLK47");
  if (beamclk03 & negbeamclk03)
    {
      if (verbosity > 0)
  	{
  	  ostringstream errmsg;
  	  errmsg << ThisName << " <E> SubsystemCheck: "
  		 << "Packet: " << packetiter->PacketId
  		 << " Spiro beam clock error 0-3: 0x"
  		 << hex << beamclk03
  		 << " neg: 0x" << negbeamclk03
  		 << dec;
  	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 29);
  	}
      //      have_subsystem_error = 1;
      //      iret = -1;
      mypktiter->second.spiroclk03++;
    }
  if (beamclk47 & negbeamclk47)
    {
      if (verbosity > 0)
  	{
  	  ostringstream errmsg;
  	  errmsg << ThisName << " <E> SubsystemCheck: "
  		 << "Packet: " << packetiter->PacketId
  		 << " Spiro beam clock error 4-7: 0x"
  		 << hex << beamclk47
  		 << " neg: 0x" << negbeamclk47
  		 << dec;
  	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 30);
  	}
      //      have_subsystem_error = 1;
      //      iret = -1;
      mypktiter->second.spiroclk47++;
    }
  packetiter->SubsystemError+=have_subsystem_error;
  return iret;
}

int
DaqVtxpMon::BeginRun(const int runno)
{
  OnlMonServer *se = OnlMonServer::instance();
  OnlMon *daqmon = DaqMon::instance();
  mypkt.clear();
  BOOST_FOREACH(unsigned int pktid, packetnumbers)
    {
      if (se->IsPacketActive(pktid))
        {
	  vtxppacketerrors pkterr;
	  pkterr.PacketId = pktid;
	  mypkt[pktid] = pkterr;
        }
    }
  h2status = new TH2F("Daq_VTXPSubsysStatus_tmp", "Daq_VTXPSubsysStatus", mypkt.size(), 0 , mypkt.size(),  VTXPLASTBIN, -0.5, VTXPLASTBIN + 0.5);
  se->registerHisto(daqmon->Name(), "Daq_VTXPSubsysStatus", h2status, 1);
  int iret = GranuleMon::BeginRun(runno);
  return iret;
}

int
DaqVtxpMon::FillHisto()
{
  GranuleMon::FillHisto();
  map<unsigned int, vtxppacketerrors>::const_iterator iter;
  int index = 0;
  for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
    {
      index++;
      h2status->SetBinContent(index,SPIROEVTCNTBIN,iter->second.spirobadcount);
      h2status->SetBinContent(index,SPIRO_ALOSSLOCKBIN,iter->second.spiroAlosslock);
      h2status->SetBinContent(index,SPIRO_B_LOSSLOCKBIN,iter->second.spiroBlosslock);
      h2status->SetBinContent(index,SPIRO_ASIZEERRORBIN,iter->second.spiroAsizeerror);
      h2status->SetBinContent(index,SPIRO_B_SIZEERRORBIN,iter->second.spiroBsizeerror);
      h2status->SetBinContent(index,SPIROPARITYBIN,iter->second.spiroparity);
      h2status->SetBinContent(index,SPIROEVTNOBIN,iter->second.spiroeventnum);
      h2status->SetBinContent(index,SPIROCLK03BIN,iter->second.spiroclk03);
      h2status->SetBinContent(index,SPIROCLK47BIN,iter->second.spiroclk47);
    }
    return 0;
}

vtxppacketerrors::vtxppacketerrors(const unsigned int ipkt):
    PacketId(ipkt)
{
    Reset();
}

void
vtxppacketerrors::Reset()
{
    spirobadcount = 0;
    spiroAlosslock = 0;
    spiroBlosslock = 0;
    spiroAsizeerror = 0;
    spiroBsizeerror = 0;
    spiroparity = 0;
    spiroeventnum = 0;
    spiroclk03 = 0;
    spiroclk47 = 0;
}


