#include "DaqVtxsMon.h"
#include "DaqVtxsMonDefs.h"
#include "DaqMon.h"
#include "GranuleMonDefs.h"

#include <OnlMonServer.h>

#include <msg_profile.h>
#include <Event.h>
#include <packet.h>

#include <TH2.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>

// this is an ugly hack, the gcc optimizer has a bug which
// triggers the uninitialized variable warning which
// stops compilation because of our -Werror 
#include <boost/version.hpp> // to get BOOST_VERSION
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 4 && BOOST_VERSION == 105700 )
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma message "ignoring bogus gcc warning in boost header lexical_cast.hpp"
#include <boost/lexical_cast.hpp>
#pragma GCC diagnostic warning "-Wuninitialized"
#else
#include <boost/lexical_cast.hpp>
#endif
// boost::split makes this version gcc 4.4.7  barf otherwise
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 4)

#pragma GCC diagnostic ignored "-Warray-bounds"

#endif 
// // boost::split makes this version gcc 4.3.2  barf otherwise
// #if (__GNUC__ == 4 && __GNUC_MINOR__ == 3)

// #pragma GCC diagnostic ignored "-Warray-bounds"

// #endif 

using namespace std;

// replace multiple white spaces with single white space
// copied from the web
bool 
consecutiveWhiteSpace( char a, char b ){ return isspace(a) && isspace(b); }

std::map<int, int> badcellid;

DaqVtxsMon::DaqVtxsMon(const string &arm):
  h2status(NULL),
  ipktmin(24101),
  ipktmax(24140),
  nsubsystemerrors(0)
{
  if (arm == "VTXS")
    {
      ThisName = "VTXS";
    }
  else
    {
      cout << "invalid arm: " << arm << endl;
      exit(-2);
    }
  memset(last_cellid, 0xFF, sizeof(last_cellid));
  memset(last_bclk, 0xFF, sizeof(last_bclk));
  memset(rccmasked,0,sizeof(rccmasked));
  memset(hybridmask,0,sizeof(hybridmask));
  ladderpktmap["B2_L00"] = 24101;
  ladderpktmap["B2_L01"] = 24102;
  ladderpktmap["B2_L02"] = 24103;
  ladderpktmap["B2_L03"] = 24104;
  ladderpktmap["B2_L05"] = 24105;
  ladderpktmap["B2_L06"] = 24106;
  ladderpktmap["B2_L07"] = 24107;
  ladderpktmap["B2_L04"] = 24108;
  ladderpktmap["B3_L00"] = 24109;
  ladderpktmap["B3_L01"] = 24110;
  ladderpktmap["B3_L02"] = 24111;
  ladderpktmap["B3_L03"] = 24112;
  ladderpktmap["B3_L04"] = 24113;
  ladderpktmap["B3_L05"] = 24114;
  ladderpktmap["B3_L06"] = 24115;
  ladderpktmap["B3_L07"] = 24116;

  ladderpktmap["B2_L08"] = 24117;
  ladderpktmap["B2_L09"] = 24118;
  ladderpktmap["B2_L10"] = 24119;
  ladderpktmap["B2_L11"] = 24120;
  ladderpktmap["B2_L12"] = 24121;
  ladderpktmap["B2_L13"] = 24122;
  ladderpktmap["B2_L14"] = 24123;
  ladderpktmap["B2_L15"] = 24124;
  ladderpktmap["B3_L08"] = 24125;
  ladderpktmap["B3_L09"] = 24126;
  ladderpktmap["B3_L10"] = 24127;
  ladderpktmap["B3_L11"] = 24128;
  ladderpktmap["B3_L12"] = 24129;
  ladderpktmap["B3_L14"] = 24130;
  ladderpktmap["B3_L15"] = 24131;
  ladderpktmap["B3_L13"] = 24132;

  ladderpktmap["B3_L16"] = 24133;
  ladderpktmap["B3_L17"] = 24134;
  ladderpktmap["B3_L18"] = 24135;
  ladderpktmap["B3_L19"] = 24136;
  ladderpktmap["B3_L20"] = 24137;
  ladderpktmap["B3_L21"] = 24138;
  ladderpktmap["B3_L22"] = 24139;
  ladderpktmap["B3_L23"] = 24140;
  return ;
}

int
DaqVtxsMon::Init()
{
  if (ThisName == "VTXS")
    {
      GranuleNumber = GRAN_NO_VTXS;
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
  // set the number of rccs in each packet (Barrel 2 -> 5, Barrel 3 -> 6)
  for (unsigned int i = ipktmin; i <= ipktmax; i++)
    {
      if (i <= 24108 || (i >= 24117 && i <= 24124))
	{
	  numrccs[i] = 5;
	}
      else
	{
	  numrccs[i] = 6;
	}
    }
  GranuleMon::Init();
  return 0;
}

int 
DaqVtxsMon::BeginRun(const int runno)
{
  nsubsystemerrors = 0;
  memset(last_cellid,0xFF,sizeof(last_cellid));
  memset(last_bclk,0xFF,sizeof(last_bclk));
  OnlMonServer *se = OnlMonServer::instance();
  OnlMon *daqmon = DaqMon::instance();
  mypkt.clear();
  BOOST_FOREACH(unsigned int pktid, packetnumbers)
    {
      if (se->IsPacketActive(pktid))
        {
	  vtxspacketerrors pkterr;
	  pkterr.PacketId = pktid;
	  for (int i=numrccs[pktid]; i<6; i++)
	    {
	      pkterr.rcc[i] = -1;
	      pkterr.disabled_bclk[i] = -1;
	      for (int j = 0; j<4; j++)
		{
		  pkterr.hybrid[i][j] = -1;
		}
	      for (int j = 0; j < 12; j++)
		{
		  pkterr.cellid[i][j] = -1;
		  pkterr.invalid_cellid[i][j] = -1;
		  pkterr.stuck_cellid[i][j] = -1;
                  pkterr.disabled_cellid[i][j] = -1;
		}
	    }
	  mypkt[pktid] = pkterr;
        }
    }

  h2status = new TH2F("Daq_VTXSSubsysStatus_tmp","Daq_VTXSSubsysStatus",mypkt.size(),-0.5,mypkt.size()+0.5,  VTXSLASTBIN, -0.5, VTXSLASTBIN+0.5);
  se->registerHisto(daqmon->Name(), "Daq_VTXSSubsysStatus", h2status, 1);
  ReadFeedConfig();
  int iret = GranuleMon::BeginRun(runno);
  return iret;
}



int 
DaqVtxsMon::DcmFEMParityErrorCheck()
{
  // in case the parity calculations are removed from either FEM or DCM
  //packetiter->DcmFEMParityError = -1;
  //return 0;

  int fem_parity = p->iValue(0, "PARITY");  // parity of packet sent by FEM
  int dcm_parity = p->iValue(0, "DCMPARITY");  // parity of the packet received by the DCM
  if ( fem_parity != dcm_parity )
    {
      if (verbosity > 0)
        {
          ostringstream errmsg;
          errmsg << ThisName << " <E> DcmFEMParityErrorCheck: "
		 << "Packet: " << packetiter->PacketId
		 << ", FEM Parity: 0x" << hex << fem_parity
		 << ", DCM Parity: 0x" << dcm_parity
		 << dec;
          DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 21);
        }
      packetiter->DcmFEMParityError++;
      // Elsewhere in the online monitoring, Chris compares the beam
      // clock counters between the packets of a given group.  This
      // Gl1SyncBad is a vector of packetid's which have problems for
      // this event and which should not be used as a comparison.
      Gl1SyncBad.insert(packetiter->PacketId);
      return -1;
    }
  return 0;
}

int
DaqVtxsMon::GlinkCheck()
{
  packetiter->GlinkError = -1;
  return 0;
}

unsigned int
DaqVtxsMon::LocalEventCounter()
{
  // event counter starts at 0
  int iev = p->iValue(0, "EVTNR");
  iev++;
  return iev; // 12 bit event number
}


void
DaqVtxsMon::SetBeamClock()
{
  BeamClock = p->iValue(0, "BCLCK");
  return;
}

int
DaqVtxsMon::SubsystemCheck(Event *evt, const int iwhat)
{
  if (iwhat != SUBSYS_PACKETEXIST)
    {
      return 0;
    }

  int have_subsystem_error = 0;
  map<unsigned int, vtxspacketerrors>::iterator mypktiter = mypkt.find(packetiter->PacketId);
  if (mypktiter == mypkt.end())
    {
      vtxspacketerrors pkterr(packetiter->PacketId);
      mypkt[packetiter->PacketId] = pkterr;
      mypktiter = mypkt.find(packetiter->PacketId);
    }
  int flagword = p->iValue(0, "FLAG");
  if (flagword & 0x80) // 7th bit is LDTB timeout bit -> communication timeout with ladder -> bad
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> SubsystemCheck: "
		 << "Packet: " << packetiter->PacketId
		 << " LDTB timeout";
	  DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 21);
	}
      //packetiter->SubsystemError++;
      mypktiter->second.ldtbtimeout++;
      return 0;
    }
  int rccenabled[6];
  int temporary_cell_disabled[6][12]; // array for disabled cells where we don't know yet if this is caused by an actual error
  memset(temporary_cell_disabled,0,sizeof(temporary_cell_disabled));
  int temporary_rcc_disabled[6];
  memset(temporary_rcc_disabled,0,sizeof(temporary_rcc_disabled));

  unsigned int number_active_rcc = 0;
  for (int ircc = 0; ircc < 6; ircc++)
    {
      rccenabled[ircc] = p->iValue(ircc, "ENABLED");
      if (rccenabled[ircc])
	{
	  ++number_active_rcc;
	}
      else
	{
	  if (ircc < numrccs[packetiter->PacketId])
            {
	      int index = packetiter->PacketId - ipktmin;
	      if (! rccmasked[index][ircc])
		{
  	          have_subsystem_error = 1;
		}
		  // else
		  //   {
		  //     cout << "no subsys err for packet " << packetiter->PacketId << " rcc " << ircc << endl;
		  //   }
	      mypktiter->second.rcc[ircc]++;
	      mypktiter->second.disabled_bclk[ircc]++;
	      for (int i = 0; i < 12; i++)
		{
		  mypktiter->second.disabled_cellid[ircc][i]++;
		}
            }
	}

    }
  // if everything is disabled no point in checking anything
  if (number_active_rcc == 0)
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> SubsystemCheck: "
		 << "Packet: " << packetiter->PacketId
		 << " no active rccs";
	  //		      cout << errmsg.str() << endl;
		DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 22);
	}
      //      packetiter->SubsystemError++;
      mypktiter->second.noactivercc++;
      return 0;
    }
  // rcc beam clock counter check
  for (int ircc = 0; ircc < 6; ircc++)
    {
      if (rccenabled[ircc])
	{
	  int stuck_beam_clk = 0;
	  int rccblk = p->iValue(ircc, "RCCBCLK");
	  // if beam clocks is identical to previous value we might have a stuck counter
	  if (last_bclk[ircc] == rccblk)
	    {
	      mypktiter->second.stuck_bclk[ircc]++;
	      if (mypktiter->second.stuck_bclk[ircc] > 2) // have 3 identical beam clocks in a row to avoid accidental matches
		{
		  stuck_beam_clk = 1;
		  if (verbosity > 1)
		    {
		      ostringstream errmsg;
		      errmsg << ThisName << " <E> SubsystemCheck: "
			     << "Packet: " << packetiter->PacketId
			     << " beamclock stuck current 0x" << hex << rccblk
			     << ", last: 0x"
			     <<  last_bclk[ircc]
			     << dec << endl;
		      //		      cout << errmsg.str() << endl;
		      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 23);
		    }
		}
	      else
		{
		  // less than 3 identical beam clocks, wait and see but mark disabled without error
		  // so subsequent tests do not get screwed
	          temporary_rcc_disabled[ircc] = 1;
		  if (verbosity > 2)
		    {
		      ostringstream errmsg;
		      errmsg << ThisName << " <W> SubsystemCheck: "
			     << "Packet: " << packetiter->PacketId
			     << " beamclock maybe stuck current 0x" << hex << rccblk
			     << ", last: 0x"
			     <<  last_bclk[ircc]
			     << dec << endl;
		      //		      cout << errmsg.str() << endl;
		      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 24);
		    }

		  for (int j = 0; j < 12; j++)
		    {
		      temporary_cell_disabled[ircc][j] = 1;
		    }
		  continue;
		}
	    }
	  else
	    {
	      mypktiter->second.stuck_bclk[ircc] = 0;
	    }
	  int ibeam = BeamClock;
	  // this is only entered when the above stuck beam clock threshold of 2 is exceeded (stuck_beam_clk)
	  if (stuck_beam_clk || fabs(rccblk - ibeam) > 1)
	    {
	      rccenabled[ircc] = 0;
	      --number_active_rcc;
	      if (verbosity > 1)
		{
		  ostringstream errmsg;
		  errmsg << ThisName << " <E> SubsystemCheck: "
			 << "Packet: " << packetiter->PacketId
			 << " beamclock 0x" << hex << ibeam
			 << ", rcc " << ircc << ": 0x"
			 <<  p->iValue(ircc, "RCCBCLK")
			 << dec 
			 << ", diff: " << rccblk-ibeam << " " <<  endl;
		  //		      cout << errmsg.str() << endl;
		      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 25);
		}
	      have_subsystem_error = 1;
	      mypktiter->second.bclk[ircc]++;
	      for (int i = 0; i < 12; i++)
		{
		  mypktiter->second.disabled_cellid[ircc][i]++;
		}
	    }
	}
    }
  // if everything is disabled no point in checking anything
  if (number_active_rcc == 0)
    {
      if (verbosity > 0)
	{
	  ostringstream errmsg;
	  errmsg << ThisName << " <E> SubsystemCheck: "
		 << "Packet: " << packetiter->PacketId
		 << " no active rccs after beam clock check";
	  //		      cout << errmsg.str() << endl;
	       DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 26);
	}
      //      packetiter->SubsystemError++;
      return 0;
    }
  // check the hybrid status
  int hybrid_enabled[6][4];
  memset(hybrid_enabled,0,sizeof(hybrid_enabled));
  for (int ircc = 0; ircc < 6; ircc++)
    {
      if (rccenabled[ircc] && !temporary_rcc_disabled[ircc])
	{
	  int hybridenabled = p->iValue(ircc,"RCCHYBRID");
	  for (int i=0; i<4; i++)
	    {
	      if (!((hybridenabled >> i) & 0x1))
		{
		  int index = packetiter->PacketId - ipktmin;
		  if (! hybridmask[index][ircc][i])
		    {
		      have_subsystem_error = 1;
		    }
		  // else
		  //   {
		  //     cout << "no subsys err for packet " << packetiter->PacketId << " rcc " << ircc << " hybrid " << i << endl;
		  //   }
		  mypktiter->second.hybrid[ircc][i]++;
		}
	      else
		{
		  hybrid_enabled[ircc][i] = 1;
		}
	    }
	}
    }

  // now the cell id checks
  map<int, int> cellidmap;
  int number_active_chips = 0;
  int cached_cellid[6][12];
  int disabled_cellid[6][12];
  memset(disabled_cellid, 0,sizeof(disabled_cellid));
  memset(cached_cellid, 0xFF,sizeof(cached_cellid));
  for (int ircc = 0; ircc < 6; ircc++)
    {
      if (rccenabled[ircc] && !temporary_rcc_disabled[ircc])
	{
	  for (int chip = 0; chip < 12; chip++)
	    {
	      int cell_id = p->iValue(ircc, chip, "CELLID");
	      if (hybrid_enabled[ircc][chip/3] && cell_id != CELLIDSVX4BUSY)
		{ 
		  cached_cellid[ircc][chip] = cell_id;
		  if (cell_id > MAXCELLID || cell_id < MINCELLID)
		    {
		      badcellid[cell_id]++;
		      if ( !mypktiter->second.disabled_cellid[ircc][chip])
			{
			  have_subsystem_error = 1;
			  // if (mypktiter->first == 24111 || mypktiter->first ==24115)
			  //   {
			  //     cout << "packet " << mypktiter->first << ", rcc: " << ircc
			  // 	   << ", chip " << chip << ", cellid: " << cell_id
			  // 	   << endl;
			  //   }
			  mypktiter->second.invalid_cellid[ircc][chip]++;
			}
		    }
		  else
		    {
		      cellidmap[cell_id]++;
		      ++number_active_chips;
		    }
		}
	      else
		{
		  cached_cellid[ircc][chip] = -1;
		  disabled_cellid[ircc][chip] = 1;
		}
	    }
	}
      else
	{
	  for (int chip=0; chip<12; chip++)
	    {
	      disabled_cellid[ircc][chip] = 1;
	    }
	}
    }
  // now all cells we should not look at are in the  disabled_cellid array
  if (!cellidmap.empty()) // in case the rccs are all temporarily disabledor or all cells invalid, this map is empty
    {
      if (cellidmap.size() > 1)
	{
	  have_subsystem_error = 1;
	  map<int, int>::const_iterator iter;
	  int saveindex = -1;
	  int maxcnt = 0;
	  for (iter = cellidmap.begin(); iter != cellidmap.end(); ++iter)
	    {
	      if (iter->second > maxcnt)
		{
		  maxcnt = iter->second;
		  saveindex = iter->first; // saveindex contains the cell id with the max counts
		}
	    }
	  // if our maximum number of cell ids which agree is less than half of the total
	  // number of cells we do not have a majority vote mark all bad or if the majority has an invalid cellid
	  if (maxcnt < number_active_chips / 2)
	    {
	      for (int ircc = 0; ircc < 6; ircc++)
		{
		  for (int chip = 0; chip < 12; chip++)
		    {
		      if ( !mypktiter->second.disabled_cellid[ircc][chip] && !temporary_rcc_disabled[ircc] && !disabled_cellid[ircc][chip])
			{
			  mypktiter->second.cellid[ircc][chip]++;
			}
		    }
		}
	    }
	  else // mark every one which does not agree with the majority as bad
	    {
	      for (int ircc = 0; ircc < 6; ircc++)
		{
		  if (temporary_rcc_disabled[ircc])
		    {
		      continue;
		    }
		  for (int chip = 0; chip < 12; chip++)
		    {
		      if (disabled_cellid[ircc][chip])
			{
			  continue;
			}
		      if (cached_cellid[ircc][chip] != saveindex)
			{
			  mypktiter->second.cellid[ircc][chip]++;
			}
		    }
		}
	    }
	}
    }
  //  packetiter->SubsystemError += have_subsystem_error;
  nsubsystemerrors += have_subsystem_error;
  return 0;
}        




vtxspacketerrors::vtxspacketerrors(const unsigned int ipkt):
  PacketId(ipkt)
{
  Reset();
}

void
vtxspacketerrors::Reset()
{
  ldtbtimeout = 0;
  noactivercc = 0;
  memset(rcc,0,sizeof(rcc));
  memset(bclk,0,sizeof(bclk));
  memset(stuck_bclk,0,sizeof(bclk));
  memset(disabled_bclk,0,sizeof(disabled_bclk));
  memset(hybrid,0,sizeof(hybrid));
  memset(cellid,0,sizeof(cellid));
  memset(invalid_cellid,0,sizeof(invalid_cellid));
  memset(disabled_cellid,0,sizeof(disabled_cellid));
  memset(stuck_cellid,0,sizeof(stuck_cellid));
}

int
DaqVtxsMon::FillHisto()
{
  GranuleMon::FillHisto();
  map<unsigned int, vtxspacketerrors>::const_iterator iter;
  int index = 0;
  for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
    {
      index++;
      h2status->SetBinContent(index,LDTBTIMEOUTBIN,iter->second.ldtbtimeout);
      h2status->SetBinContent(index,NORCCBIN,iter->second.noactivercc);
      for (int i=0; i<6; i++)
	{
	  int ibin = i+RCCBEGINBIN;
	  h2status->SetBinContent(index,ibin,iter->second.rcc[i]);
	  ibin = i + BCLKBEGINBIN;
	  h2status->SetBinContent(index,ibin,iter->second.bclk[i]);
	  for (int j=0; j<4; j++)
	    {
	      ibin = (i*4+j)+HYBRIDBEGINBIN;
	      h2status->SetBinContent(index,ibin,iter->second.hybrid[i][j]);
	    }
	  for (int j=0; j<12; j++)
	    {
	      ibin = (i*12+j) + CELLIDINVBEGINBIN;
	      h2status->SetBinContent(index,ibin,iter->second.invalid_cellid[i][j]);
	       ibin = (i*12+j) + CELLIDBEGINBIN;
	       h2status->SetBinContent(index,ibin,iter->second.cellid[i][j]);
	      // if (iter->second.cellid[i][j] < 0)
	      //  	{
	      //  	  cout << "cont " << iter->second.invalid_cellid[i][j]
	      //  	       << ", ircc: " << i << ", ichip: " << j << ", index " << ibin
	      //  	       << endl;
	      //  	}
	    }
	}

    }
    return 0;
}
 
void
DaqVtxsMon::identify(ostream& out) const
{
  GranuleMon::identify(out);
  map<unsigned int, vtxspacketerrors>::const_iterator iter;
  int iwid = 10;
  ostringstream rccstr;
  out <<  setw(iwid) << "Packet";
  for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  rccstr.str("");
	  rccstr << "RCC" << i << "H" << j;
	  out << setw(iwid)  << rccstr.str();
	}
    }
  out << endl;
  for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
    {
      out <<  setw(iwid) << iter->first;
      for (int i = 0; i < 3; i++)
	{
	  for (int j = 0; j < 4; j++)
	    {
	      out <<  setw(iwid)  << iter->second.hybrid[i][j];
	    }
	}
      out << endl;
    }

  out <<  setw(iwid) << "Packet";
  for (int i = 3; i < 6; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  rccstr.str("");
	  rccstr  << "RCC" << i << "H" << j;
	  out  << setw(iwid) << rccstr.str();
	}
    }
  out << endl;
  for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
    {
      out <<  setw(iwid) << iter->first;
      for (int i = 3; i < 6; i++)
	{
	  for (int j = 0; j < 4; j++)
	    {
	      out <<  setw(iwid) << iter->second.hybrid[i][j];
	    }
	}
      out << endl;
    }
  out << "VTXS Cell Id" << endl;
  iwid = 8;
  for (int ircc = 0; ircc < 6; ircc++)
    {
      out << "RCC " << ircc << endl;
      out <<  setw(iwid) << "Packet";
      for (int j = 0; j < 12; j++)
	{
	  ostringstream ostr;
	  ostr.str("");
	  ostr << "Cell" << j;
	  out <<  setw(iwid) << ostr.str();
	}
      out << endl;
      for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
	{
	  out <<  setw(iwid) << iter->first;
	  for (int j = 0; j < 12; j++)
	    {
	      out <<  setw(iwid) << iter->second.cellid[ircc][j];
	    }
	  out << endl;
	}

      out << endl;
    }

  out << "VTXS Bad Cell Id" << endl;
  iwid = 8;
  for (int ircc = 0; ircc < 6; ircc++)
    {
      out << "RCC " << ircc << endl;
      out <<  setw(iwid) << "Packet";
      for (int j = 0; j < 12; j++)
	{
	  ostringstream ostr;
	  ostr.str("");
	  ostr << "Cell" << j;
	  out <<  setw(iwid) << ostr.str();
	}
      out << endl;
      for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
	{
	  out <<  setw(iwid) << iter->first;
	  for (int j = 0; j < 12; j++)
	    {
	      out <<  setw(iwid) << iter->second.cellid[ircc][j];
	    }
	  out << endl;
	}

      out << endl;
    }

  out << "VTXS Invalid Cell Id" << endl;
  iwid = 8;
  for (int ircc = 0; ircc < 6; ircc++)
    {
      out << "RCC " << ircc << endl;
      out <<  setw(iwid) << "Packet";
      for (int j = 0; j < 12; j++)
	{
	  ostringstream ostr;
	  ostr.str("");
	  ostr << "Cell" << j;
	  out <<  setw(iwid) << ostr.str();
	}
      out << endl;
      for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
	{
	  out <<  setw(iwid) << iter->first;
	  for (int j = 0; j < 12; j++)
	    {
	      out <<  setw(iwid) << iter->second.invalid_cellid[ircc][j];
	    }
	  out << endl;
	}

      out << endl;
    }

  // map<int,int>::const_iterator it;
  // cout << "Bad Cell Ids:" << endl;
  // for (it=badcellid.begin(); it != badcellid.end(); it++)
  //   {
  //     cout << "id " << it->first << ", n: " << it->second << endl;
  //   }
  return;
}

int
DaqVtxsMon::ReadFeedConfig()
{
  ifstream eastconf;
  string FullLine;	// a complete line in the config file
  string filename[2] = {"/home/phnxvtx/slowcontrol/StripixelSC/configfiles/vmeconfig_east_comm.txt",
			"/home/phnxvtx/slowcontrol/StripixelSC/configfiles/vmeconfig_west_comm.txt"};
  memset(rccmasked,0,sizeof(rccmasked));
  memset(hybridmask,0,sizeof(hybridmask));
  for (int ifile=0; ifile<2; ifile++)
    {
      eastconf.open(filename[ifile].c_str(), ifstream::in);
      getline(eastconf, FullLine);
      while ( !eastconf.eof())
	{
	  FullLine.erase( std::unique(FullLine.begin(), FullLine.end(), consecutiveWhiteSpace), FullLine.end() );
	  vector<string> tokens;
	  boost::split(tokens, FullLine, boost::is_any_of(" "));
	  vector<string> ladder;
	  boost::split(ladder, tokens[0], boost::is_any_of(":"));
	  unsigned int mypacket = 0;
	  // cout << "ladder: " << ladder[2] << " packet " << ladderpktmap.find(ladder[2])->second << endl;
	  map<string, unsigned int>::const_iterator liter = ladderpktmap.find(ladder[2]);
	  if (liter != ladderpktmap.end())
	    {
	      mypacket = liter->second;
	    }
	  else
	    {
	      cout << "could not find " << ladder[2] << " in ladderpktmap" << endl;
	    }
	  ifstream rccconf;
	  string confline;
	  string conffilename = "/home/phnxvtx/slowcontrol/StripixelSC/" + tokens[2];
	  rccconf.open(conffilename.c_str(), ifstream::in);
	  getline(rccconf, confline);
	  int maskval[7];
	  memset(maskval, 0, sizeof(maskval));
	  int icnt = 0;
	  while(!rccconf.eof())
	    {
	      vector<string> mask;
	      boost::split(mask, confline, boost::is_any_of(" "));
	      // rcc mask is first
	      try
		{
		  maskval[icnt] = boost::lexical_cast<int>(mask[0]);
		}
	      catch(const boost::bad_lexical_cast &e)
		{
		  cout << "could not convert " << mask[0] << " to int" << endl;
		}
	      icnt++;
	      getline(rccconf, confline);
	    }
	  rccconf.close();
	  map<unsigned int, vtxspacketerrors>::const_iterator iter;
	  int index = 0;
	  for (iter = mypkt.begin(); iter != mypkt.end(); ++iter)
	    {
	      index++;
	      if (iter->first == mypacket)
		{
		  int index = mypacket - ipktmin; // index start at zero
		  for (int i = 0; i < 6; i++)
		    {
		      if ( maskval[0] >> i & 0x1)
			{
			  //cout << "packet " << mypacket << " masked rcc no " << i << endl;
			  rccmasked[index][i] = 1;
			  h2status->SetBinContent(index, DISABLEDRCCBEGINBIN + i, 1);
			  for (int j = 0; j < 4; j++)
			    {
			      hybridmask[index][i][j] = 1;
			      h2status->SetBinContent(index, DISABLEDHYBRIDBEGINBIN + (i * 4) + j, 1);
			    }
			}
		    }
		  for (int i = 0; i < 6; i++)
		    {
		      for (int j = 0; j < 4; j++)
			{
			  if ( maskval[i + 1] >> j & 0x1)
			    {
			      hybridmask[index][i][j] = 1;
			      // cout  << "packet " << mypacket << " RCC " << i << " hybrid " << j << " disabled" << endl;
			      h2status->SetBinContent(index, DISABLEDHYBRIDBEGINBIN + (i * 4) + j, 1);
			    }
			}
		    }
		}
	    }
	  getline(eastconf , FullLine );
	}
      eastconf.close();
    }

  return 0;
}
