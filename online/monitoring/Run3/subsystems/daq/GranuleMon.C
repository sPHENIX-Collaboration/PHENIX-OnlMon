#include "GranuleMon.h"
#include "GranuleMonDefs.h"
#include "DaqMon.h"

#include <OnlMonServer.h>

#include <msg_profile.h>
#include <Event.h>
#include <EventTypes.h>

#include <TH3.h>

#include <boost/foreach.hpp>

#include <cstdlib>
#include <iomanip>
#include <sstream>

using namespace std;

GranuleMon::GranuleMon(const string &name):
  ThisName(name),
  verbosity(0),
  GranuleNumber(~0), // the complement gives 0xFFFF.. independant of bits used
  BeamClock(~0),
  packets_found(0),
  gl1syncerror(0),
  gl1syncok(0),
  nfill(0),
  packetLength(0),
  standard_clock(INIT_STANDARD_CLOCK),
  evtsizesum(0),
  femsequence_check_mask(0xFFFF),
  p(NULL),
  ErrorCounterHisto(NULL),
  packetHisto(NULL),
  variableHisto(NULL)
{
  ResetErrorCounters();
  return ;
}

GranuleMon::~GranuleMon()
{
  packetnumbers.clear();
  return;
}


void GranuleMon::identify(ostream& out) const
{
  static int iwid = 11;
  out << "GranuleMon Object, Name: " << ThisName
      << " Granule No: " << GranuleNumber << endl;
  out << "Number of packets: " << pkt.size() << endl;
  out << "Error Counts: " << endl;
  out << setw(iwid) << "Packet"
       << setw(iwid) << "Length"
       << setw(iwid) << "DcmChkSum"
       << setw(iwid) << "FEMParity"
       << setw(iwid) << "DcmFEMPar"
       << setw(iwid) << "FEMClk"
       << setw(iwid) << "FEMGL1Clk"
       << setw(iwid) << "FEMEvtSeq"
       << setw(iwid) << "GlinkErr"
       << setw(iwid) << "SubsysErr"
       << setw(iwid) << "Missing"
       << setw(iwid) << "NPackets"
       << setw(iwid) << "GoodPkt"
       << setw(iwid) << "Byt/pkt"
       << endl;

  BOOST_FOREACH(packetstruct const &mypkt, pkt)
    {
      int BytesPerPacket;
      if (mypkt.PacketCounter > 0)
	{
	  BytesPerPacket = mypkt.SumBytes / mypkt.PacketCounter;
	}
      else
	{
	  BytesPerPacket = 0;
	}
      out << setw(iwid) << mypkt.PacketId
	   << setw(iwid) << mypkt.BadLength
	   << setw(iwid) << mypkt.DcmCheckSumError
	   << setw(iwid) << mypkt.FEMParityError
	   << setw(iwid) << mypkt.DcmFEMParityError
	   << setw(iwid) << mypkt.FEMClockCounterError
	   << setw(iwid) << mypkt.FEMGL1ClockError
	   << setw(iwid) << mypkt.FEMEvtSeqError
	   << setw(iwid) << mypkt.GlinkError
	   << setw(iwid) << mypkt.SubsystemError
	   << setw(iwid) << mypkt.MissingPackets
	   << setw(iwid) << mypkt.PacketCounter
	   << setw(iwid) << mypkt.GoodPackets
	   << setw(iwid) << BytesPerPacket
	   << endl;
    }

  return ;
}

int GranuleMon::CreateHisto()
{
  OnlMonServer *s = OnlMonServer::instance();
  OnlMon *daqmon = DaqMon::instance();
  string hname = "Daq_" + ThisName + "AllVariables";
  variableHisto = new TH1F(hname.c_str(), hname.c_str(), NBINSVAR, 0, NBINSVAR);
  s->registerHisto(daqmon,variableHisto);
  return 0;
}

int
GranuleMon::CreateRunwiseHisto()
{
  OnlMonServer *s = OnlMonServer::instance();
  OnlMon *daqmon = DaqMon::instance();
  string hname = "Daq_" + ThisName + "Monitor";
  string hname1 = "Daq_" + ThisName + "MonitorTmp";
  ErrorCounterHisto = new TH3F(hname1.c_str(), hname.c_str(), NBINS, 0, NBINS * NUPDATE, pkt.size(), 0, pkt.size(), NBINSPKT, 0., NBINSPKT );
  s->registerHisto(daqmon->Name(),hname.c_str(), ErrorCounterHisto, 1);
  hname = "Daq_" + ThisName + "PacketId";
  hname1 = "Daq_" + ThisName + "PacketIdTmp";
  packetHisto = new TH1F(hname1.c_str(), hname.c_str(), pkt.size(), 0, pkt.size());
  InitPacketIdHisto();
  s->registerHisto(daqmon->Name(),hname.c_str(), packetHisto, 1);
  return 0;
}

int GranuleMon::FillHisto()
{
  DaqMon *daqmon = DaqMon::instance();
  nfill++;
  unsigned int nbinsx = ErrorCounterHisto->GetNbinsX();
  // the following copy/replace is clunky as can be, anyone with better ideas?
  int maxnumpackets = pkt.size(); // cache number of packets so we don't have to check the pkt.size() many times
  if (nfill > nbinsx)
    {
      OnlMonServer *s = OnlMonServer::instance();
      string hname = "Daq_" + ThisName + "Monitor";
      // dirty rename trick to suppress
      // Warning in <TH1::Build>: Replacing existing histogram:
      ErrorCounterHisto->SetName("obsolete");
      TH3F *tmphis = new TH3F(hname.c_str(), hname.c_str(), nbinsx + NBINS, 0, (nbinsx + NBINS) * NUPDATE, pkt.size(), 0, pkt.size(), NBINSPKT , 0., NBINSPKT);
      for (unsigned int ibin = 0; ibin <= nbinsx; ibin++) // copy underflows (bin=0)
        {
          for (int npacket=1; npacket <= maxnumpackets; ++npacket)
            {
              for (unsigned short j = 0; j <= NBINSPKT;j++) // copy underflows (bin=0)
                {
                  tmphis->SetBinContent(ibin, npacket, j, ErrorCounterHisto->GetBinContent(ibin, npacket, j));
                }
            }
        }
      ErrorCounterHisto = tmphis;
      s->registerHisto(daqmon->Name(),hname.c_str(), ErrorCounterHisto, 1); // replace in Histo Map (and delete old one)
    }
  int npacket = 0;
  BOOST_FOREACH(packetstruct &mypkt, pkt)
    {
      npacket++;
      ErrorCounterHisto->SetBinContent(nfill, npacket, LENGTHBIN, mypkt.BadLength);
      ErrorCounterHisto->SetBinContent(nfill, npacket, DCMCHKSUMBIN, mypkt.DcmCheckSumError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, FEMPARITYBIN, mypkt.FEMParityError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, FEMCLKBIN, mypkt.FEMClockCounterError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, FEMGL1CLKBIN, mypkt.FEMGL1ClockError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, FEMEVTSEQBIN, mypkt.FEMEvtSeqError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, NPACKETBIN, mypkt.PacketCounter);
      ErrorCounterHisto->SetBinContent(nfill, npacket, NGOODPACKETBIN, mypkt.GoodPackets);
      ErrorCounterHisto->SetBinContent(nfill, npacket, SUMBYTESBIN, mypkt.SumBytes);
      ErrorCounterHisto->SetBinContent(nfill, npacket, DCMFEMPARBIN, mypkt.DcmFEMParityError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, GLINKBIN, mypkt.GlinkError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, SUBSYSTEMBIN, mypkt.SubsystemError);
      ErrorCounterHisto->SetBinContent(nfill, npacket, MISSINGPKTBIN, mypkt.MissingPackets);
    }
  variableHisto->SetBinContent(LASTFILLEDBIN, nfill);
  variableHisto->SetBinContent(NUMEVENTBIN, daqmon->DaqMonEvts());
  variableHisto->SetBinContent(SIZEPEREVENTBIN, evtsizesum);
  return 0;
}

int GranuleMon::process_event(Event *e)
{
  DaqMon *daqmon = DaqMon::instance();
  packets_found = 0;
  gl1syncerror = 0;
  gl1syncok = 0;
  standard_clock = INIT_STANDARD_CLOCK;
  DoEveryEvent(e);
  for (packetiter = pkt.begin(); packetiter != pkt.end(); ++packetiter)
    {
      IncrementPacketCounterNoMiss();
      p = e->getPacket(packetiter->PacketId);
      if (p)
        {
          packetLength = p->getLength();
          if (packetLength == 6)
            {
              // this happens if we keep the packet headers but drop the
              // packet content (one of the RC debug options)
              delete p;
              packetiter->LastClock = -1; // this packet has been dropped from this event
              if (totaldropcontent < 10000)
                {
                  totaldropcontent++;
                }
              variableHisto->SetBinContent(DROPCONTENTBIN, totaldropcontent);
              continue;
            }
          IncrementPacketCounter();
          // Why the hell not: instead of doing a len>2329 (max of the HBD) 
	  // doing an & with the complement of 0xFFF is equivalent to 
	  // len>4095 but much faster
          if (packetLength & 0xFFFFC000)
            {
              cout << "pkt length: 0x" << hex << packetLength << dec << endl;
              packetiter->BadLength++;
              packetiter->LastClock = -1; // do not dare get clk counter from this packet
              delete p;
              continue;
            }
          packetiter->SumBytes += packetLength << 2; // bitshift by 2 is equal to multiply by 4 (but faster)
          evtsizesum += packetLength << 2;
          if (p->getDebugLength())
            {
              if (DcmCheckSumCheck()) // corrupt data can cause crashes
                {
                  packetiter->LastClock = -1; // do not dare get clk counter from this packet
                  delete p;
                  continue;
                }
            }
          packets_found++; // increment number of found packets for subsystem
          int isGood = 0;
          isGood += FEMParityErrorCheck();
          isGood += DcmFEMParityErrorCheck();
          isGood += GlinkCheck();
          SetBeamClock();  // set the Beam Clock Counter for the following clk checks
          if (daqmon->GL1exists())
            {
              isGood += FEMGL1ClockCounterCheck(); // check GL1 - FEM consistency
            }
          isGood += FEMClockCounterCheck(); // check inter FEM consistency
          isGood += FEMEventSequenceCheck();
          isGood += SubsystemCheck(e, SUBSYS_PACKETEXIST);
          if (isGood == 0)
            {
              packetiter->GoodPackets++;
            }
        }
      else
        {
          packetiter->LastClock = -1; // this packet has been dropped from this event
        }
      MissingPacketCheck(e);
      if (p)
        {
          delete p;
        }
    }
  // Now all packets have been analysed, here come the global checks
  GranuleGL1SynchCheck();
  GranuleSubsystemCheck();
  // number of Granule GL1 sync errors (FATAL!!!)
  variableHisto->SetBinContent(GL1FATALBIN, totalgl1syncerror);
  variableHisto->SetBinContent(SUBSYSFATALBIN, totalsubsystemerror);
  variableHisto->SetBinContent(EVBERRORBIN, totalevberror);
  if (! firstupdate && daqmon->DaqMonEvts() > FIRSTUPDATE)
    {
      firstupdate = 1;
      FillHisto();
    }
  if ( (daqmon->DaqMonEvts() % NUPDATE == 0))
    {
      FillHisto();
    }
  // prevent complete granule from being declared bad because of
  // some glitch, clear list if it has more than 10% of granule pkts as entries
  if (Gl1SyncBad.size() > pkt.size()/10)
    {
      Gl1SyncBad.clear();
    }
  return 0;
}

int 
GranuleMon::InitPacketIdHisto()
{
  unsigned int nbin = 0;
  BOOST_FOREACH(packetstruct &mypkt, pkt)
    {
      nbin++;
      packetHisto->SetBinContent(nbin, mypkt.PacketId);
    }
  return 0;
}

int
GranuleMon::Init()
{
  CreateHisto();
  return 0;
}

int 
GranuleMon::BeginRun(const int runno)
{
  OnlMonServer *se = OnlMonServer::instance();
  struct packetstruct newpacket;
  pkt.clear();
  BOOST_FOREACH(unsigned int pktid, packetnumbers)
    {
       if (se->IsPacketActive(pktid))
         {
           newpacket.PacketId = pktid;
           pkt.push_back(newpacket);
         }
     }
  PacketResetAll();
  CreateRunwiseHisto();
  ResetErrorCounters();
  // clear packets which were declared bad in the previous run
  // and are not used as reference for beam clock counter
  Gl1SyncBad.clear();
  return 0;
}

void
GranuleMon::ResetErrorCounters()
{
  totalsubsystemerror = 0;
  totalgl1syncerror = 0;
  totaldropcontent = 0;
  totalscaledtrigerror = 0;
  totalevberror = 0;
  evtsizesum = 0;
  firstupdate = 0;
  return;
}

int
GranuleMon::PacketResetAll()
{
  nfill = 0;
  BOOST_FOREACH(packetstruct &mypkt, pkt)
    {
      mypkt.LastClock = -1;
    }
  PacketErrorReset();
  return 0;
}

int GranuleMon::PacketErrorReset()
{
  BOOST_FOREACH(packetstruct &mypkt, pkt)
    {
      mypkt.BadLength = 0;
      mypkt.DcmCheckSumError = 0;
      mypkt.FEMParityError = 0;
      mypkt.FEMClockCounterError = 0;
      mypkt.FEMGL1ClockError = 0;
      mypkt.FEMEvtSeqError = 0;
      mypkt.PacketCounter = 0;
      mypkt.GoodPackets = 0;
      mypkt.SumBytes = 0;
      mypkt.DcmFEMParityError = 0;
      mypkt.GlinkError = 0;
      mypkt.SubsystemError = 0;
      mypkt.MissingPackets = 0;
    }
  return 0;
}

int GranuleMon::DcmCheckSumCheck()
{
  int iret = p->getCheckSumStatus();
  switch (iret)
    {
    case 1:
      return 0;

    case 0:
      if (verbosity > 0)
        {
          cout << "Packet " << packetiter->PacketId
	       << " has no chk sum" << endl;
        }
      packetiter->DcmCheckSumError = -1;
      return 0;

    case -1:
      {
        ostringstream errmsg;
        errmsg << "GranuleMon::DcmCheckSumCheck(): Packet " << packetiter->PacketId
	       << " failed chk sum";
        DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 10);
        packetiter->DcmCheckSumError++;
      }
      break;
    default:
      {
        ostringstream errmsg;
        errmsg << "GranuleMon::DcmCheckSumCheck(): Packet " << packetiter->PacketId
	       << " has invalid CheckSumStatus: " << iret;
        DaqMon::instance()->send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 11);
        packetiter->DcmCheckSumError++;
      }
      break;
    }
  return -1;
}


int GranuleMon::FEMParityErrorCheck()
{
  int iret = p->iValue(0, "CHECKPARITY");
  switch (iret)
    {
    case 0:
      packetiter->FEMParityError = -1;
      return 0;

    case 1:
      return 0;

    case -1:
      {
	if (verbosity > 0)
	  {
	    ostringstream errmsg;
	    errmsg << "GranuleMon::FEMParityErrorCheck(): Packet " << packetiter->PacketId
		   << " failed parity check for hitformat " << p->getHitFormat() ;
	    DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 12);
	  }
        packetiter->FEMParityError++;
      }
      break;

    default:
      {
        ostringstream errmsg;
        errmsg << "GranuleMon::FEMParityErrorCheck(): Packet " << packetiter->PacketId
	       << " has invalid CHECKPARITY status" ;
        DaqMon::instance()->send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 13);
        packetiter->FEMParityError++;
      }
      break;
    }
  return -1;
}

int GranuleMon::DcmFEMParityErrorCheck()
{
  if (p->iValue(0, "SUMMARY") & 0x4)
    {
      return 0;
    }
  else
    {
      packetiter->DcmFEMParityError++;
      Gl1SyncBad.insert(packetiter->PacketId);
    }
  return -1;
}

int GranuleMon::GlinkCheck()
{
  if (p->iValue(0, "SUMMARY") & 0x2)
    {
      packetiter->GlinkError++;
      return -1;
    }
  return 0;
}

void GranuleMon::SetBeamClock()
{
  BeamClock = p->iValue(0, "BCLK");
  return ;
}

int GranuleMon::FEMClockCounterCheck()
{
  // the first packet found sets the standard to its beam clock
  if (standard_clock == INIT_STANDARD_CLOCK)
    {
      if (! NoUseForFemClkReference.empty())
        {
          if (NoUseForFemClkReference.find(packetiter->PacketId) != NoUseForFemClkReference.end())
            {
              return 0;
            }
        }
      if (!Gl1SyncBad.empty())
        {
          if (Gl1SyncBad.find(packetiter->PacketId) != Gl1SyncBad.end())
            {
              return 0;
            }
        }
      standard_clock = BeamClock;
    }
  else
    {
      if (BeamClock != standard_clock)
        {
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> FEMClockCounterCheck"
		     << ": Packet: " << packetiter->PacketId
		     << hex
		     << ", standard clock: 0x" << standard_clock
		     << ", local beam clock: 0x" << BeamClock
		     << dec
		     << ", standard clock (dec): " << standard_clock
		     << ", local beam clock (dec): " << BeamClock;
	      if (standard_clock > BeamClock)
		{
		  errmsg << ", diff: 0x" << hex <<  standard_clock - BeamClock << dec;
		}
	      else
		{
		  errmsg << ", diff: 0x" << hex <<  BeamClock - standard_clock << dec;
		}
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 15);
	    }
	  packetiter->FEMClockCounterError++;
	  return -1;
	}
    }
  return 0;
}


int GranuleMon::FEMGL1ClockCounterCheck()
{
  DaqMon *daqmon = DaqMon::instance();
  if (packetiter->LastClock >= 0 && daqmon->LastGL1Clock() >= 0)
    {
      int tmp1 = packetiter->LastClock & 0xFF;
      int tmp2 = BeamClock & 0xFF;
      //       if (verbosity > 0)
      //         {
      //           if ( packetiter->LastClock == 0x0)
      //             {
      //               cout << ThisName << " 0x0 beam clock" << endl;
      //             }
      //         }
      int clock_diff = tmp1 - tmp2;
      if (clock_diff < 0)
        {
          clock_diff += 256;
        }
      if (clock_diff != daqmon->GL1ClockDiff())
        {
          if (verbosity > 0)
            {
        ostringstream errmsg;

              errmsg << ThisName << " <E> FEMGL1ClockCounterCheck: "
		   << ", Packet: " << packetiter->PacketId
		   << ", Granule clock_diff: " << clock_diff
		   << ", GL1 clkdiff: " << daqmon->GL1ClockDiff()
		   << ", diff: " << abs(clock_diff - daqmon->GL1ClockDiff())
		   << ", Gl1 clk: 0x" <<  hex << daqmon->LastGL1Clock()
		   << ", lastclock: 0x" << packetiter->LastClock
		   << ", BeamClock: 0x" << BeamClock
		   << ", tmp1: 0x" << tmp1
		   << ", tmp2: 0x" << tmp2
		   << dec;
	      //              DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 17);
	      cout << errmsg.str() << endl;
            }
          if (NoUseForFemClkReference.find(packetiter->PacketId) == NoUseForFemClkReference.end())
            {
              gl1syncerror++;
            }
          packetiter->FEMGL1ClockError++;
	  // insert packet number into list of packets which should not provide
	  // a reference beam clock for this event
          Gl1SyncBad.insert(packetiter->PacketId);
          // This error is caused by the beam clock being wrong
          // saving the BeamClock as LastClock will just flag the
          // next event as having a GL1 clock error if the beam clock
          // recovered. Setting LastClock to -1 will prevent the GL1 check
          // for the next event using the current bad beam clock as reference
          packetiter->LastClock = -1;
          return -1;
        }
      else
        {
          // gl1 syncs okay for this packet
          gl1syncok++;
        }
    }
  packetiter->LastClock = BeamClock;
  return 0;
}

unsigned int GranuleMon::LocalEventCounter()
{
  return p->iValue(0, "EVTNR");
}

unsigned int GranuleMon::GlobalEventCounter()
{
  DaqMon *daqmon = DaqMon::instance();
  return daqmon->GL1GranuleCounter(GranuleNumber);
}


int GranuleMon::FEMEventSequenceCheck()
{
  DaqMon *daqmon = DaqMon::instance();
  int local_event_counter = LocalEventCounter();
  if (!daqmon->GL1exists())
    {
      if ((local_event_counter&femsequence_check_mask) != ((daqmon->EventNumber() + LocalEventNumberOffset())&femsequence_check_mask))
        {
          if (verbosity > 0)
            {
	      ostringstream errmsg;
              errmsg << ThisName << "  <E> FEMEventSequenceCheck: "
		   << "Packet: " << packetiter->PacketId
		   << ", local_event_counter: 0x" << hex << local_event_counter
		   << ", daqmon->EventNumber() + LocalEventNumberOffset(): 0x"
		   << daqmon->EventNumber() + LocalEventNumberOffset()
		   << dec
		   << ", local_event_counter (dec): " << local_event_counter
		   << ", daqmon->EventNumber() + LocalEventNumberOffset() (dec): "
		   << daqmon->EventNumber() + LocalEventNumberOffset();
	      //	      DaqMon::instance()->send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 16);
	      cout <<  errmsg.str() << endl;
            }
          packetiter->FEMEvtSeqError++;
          return -1;
        }
    }
  else
    {
      if ((local_event_counter & femsequence_check_mask) != ((int) (GlobalEventCounter()&femsequence_check_mask)))
	{
	  if (verbosity > 0)
	    {
	      ostringstream errmsg;
	      errmsg << ThisName << " <E> FEMEventSequenceCheck: "
		     << "Packet: " << packetiter->PacketId
		     << ", local_event_counter: 0x" << hex << local_event_counter
		     << ", GlobalEventCounter(): 0x"
		     << GlobalEventCounter()
		     << dec
		     << ", local_event_counter (dec): " << local_event_counter
		     << ", GlobalEventCounter() (dec): "
		     << GlobalEventCounter();
	      if (GlobalEventCounter() > (unsigned int) local_event_counter)
		{
		  errmsg << ", diff: 0x" << GlobalEventCounter() - local_event_counter << dec;
		}
	      else
		{
		  errmsg << ", diff: 0x" << local_event_counter - GlobalEventCounter() << dec;
		}
	      DaqMon::instance()->send_message(MSG_SEV_ERROR, errmsg.str(), 16);

	    }
	  packetiter->FEMEvtSeqError++;
	  return -1;
	}
    }
  return 0;
}




int
GranuleMon::GranuleGL1SynchCheck()
{
  // only if there were no good gl1 syncs a whole granule
  // can have dropped out. Since this check spans packets from 2 events
  // the previous test to check if there were gl1sync errors
  // for all packets does not work since there is no gl1 sync error
  // if this packet was missing from the previous event
  if (gl1syncok == 0)
    {
      if (gl1syncerror > 0)
        {
          totalgl1syncerror++;
          ostringstream errmsg;
          errmsg << "Granule " << ThisName << " out of sync with GL1 for the "
		 << totalgl1syncerror << " time, No of Packets in this event: "
		 << packets_found << " out of " << pkt.size()
		 << " # err pkts: " << gl1syncerror;
          DaqMon::instance()->send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 14);
	  DaqMon::instance()->SetStatus(OnlMon::ERROR);
          return -1;
        }
      else
        {
          // no packets could be checked (last beam clock = -1)
          return 0;
        }
    }
  totalgl1syncerror = 0;
  return 0;
}

int GranuleMon::SubsystemCheck(Event *evt, const int iwhat)
{
  packetiter->SubsystemError = -1;
  return 0;
}

int
GranuleMon::NoFemClkReference(const unsigned int PacketId)
{
  ostringstream errmsg;
  errmsg << "Adding " << PacketId
	 << " to packets not to be used for Fem Clk reference" ;
  DaqMon::instance()->send_message(MSG_SEV_INFORMATIONAL, errmsg.str(), 15);
  NoUseForFemClkReference.insert(PacketId);
  return 0;
}

int
GranuleMon::LocalEventNumberOffset()
{
  // event counter in Event header in runs with gl1 start at event no 1,
  // standalone start at 0 so the offset between the event counter
  // and the fem event counter which start at 1 is 0 for gl1 runs and -1 for
  // standalone runs
  DaqMon *daq = DaqMon::instance();
  if (daq->Run_Has_Gl1())
    {
      return 0;
    }
  else
    {
      return -1;
    }
}

int
GranuleMon::IncrementPacketCounter()
{
  packetiter->PacketCounter++;
  return 0;
}

int
GranuleMon::MissingPacketCheck(Event *evt)
{
  packetiter->MissingPackets = -1;
  return 0;
}

int
GranuleMon::MissingPacketCheckImpl(Event *evt)
{
  if (p)
    {
      return 0;
    }
  DaqMon *daqmon = DaqMon::instance();
  // Check if granule is active (as soon as the granulecounter increments this is set by the DaqMon
  if (!daqmon->isGranuleActive(GranuleNumber))
    {
      return 0;
    }
  OnlMonServer *se = OnlMonServer::instance();
  if (!se->IsPacketActive(packetiter->PacketId))
    {
      return 0;
    }
  int evttype = evt->getEvtType();
  switch (evttype)

    {
    case DATAEVENT:
      {
        packetiter->MissingPackets++;
        unsigned int iatp = evt->getFrameEntry("FRAMESOURCEID", 0, 0);
        ostringstream errmsg;
        errmsg << ThisName << " <E> Missing Packet Check: "
        << "Packet: " << packetiter->PacketId
        << ", Atp ID : 0x" << hex << iatp << dec;
        daqmon->send_message(MSG_SEV_SEVEREERROR, errmsg.str(), 18);
        daqmon->EventMissesPkts();
        return -1;
      }
    default:
      return 0;
    }
}

int
GranuleMon::IncrementPacketCounterNoMissImpl()
{
  if (DaqMon::instance()->GL1GranuleCounter(GranuleNumber) > 0)
    {
      packetiter->PacketCounter++;
    }
  return 0;
}
