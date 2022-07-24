#include "BbcLvl1Mon.h"
#include "BbcLl1.h"
#include "BbcLvl1MsgTypes.h"

#include <OnlMonServer.h>
#include <OnlMonTrigger.h>

#include <packet_gl1.h>
#include <Event.h>
#include <msg_control.h>

#include <TH1.h>
#include <TH2.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

// granule definitions are hidden in
// /export/software/oncs/online_configuration/GL1/.gl1_granule_names
#define GRAN_NO_BBC 2

BbcLvl1Mon::BbcLvl1Mon(const char *name): OnlMon(name)
{
  trigignoremask = 0x0;
  ConvertToCM = 1.5;
  ConvertToNS = 0.1;
  FailedRB = 0;
  EvtCount = 0;
  evtcnt = 0;
  goodevt = 0;
  badevt = 0; 
  bbc_in_partition = 0;

  BBCLvl1 = 0;
  bbcll1_mapdate = 0;
  bbcll1_mapdir = "/export/software/oncs/online_configuration/LL1/BBC/BBCTEMPDB_DIR";

  OnlMonServer *se = OnlMonServer::instance();

  BbcLvl1_VertHist = new TH1F("BbcLvl1_VertHist", "LL1 Vertex",
                              (int) (300 / ConvertToCM), -150, 150);
  BbcLvl1_VertHist->SetXTitle("Vertex, cm");
  se->registerHisto(this,BbcLvl1_VertHist);

  BbcLvl1_Vert2Hist = new TH1F("BbcLvl1_Vert2Hist", "LL1 Vertex2",
                              (int) (300 / ConvertToCM), -150, 150);
  BbcLvl1_Vert2Hist->SetXTitle("Vertex, cm");
  se->registerHisto(this,BbcLvl1_Vert2Hist);

  BbcLvl1_AverHist = new TH1F("BbcLvl1_AverHist", "LL1 Time Average",
                              300, 0, 30);
  BbcLvl1_AverHist->SetXTitle("Average Time, ns");
  se->registerHisto(this,BbcLvl1_AverHist);

  BbcLvl1_SHitsHist = new TH1F("BbcLvl1_SHitsHist", "LL1 South Multiplicity",
                               70, 0, 70);
  BbcLvl1_SHitsHist->SetXTitle("South hits");
  se->registerHisto(this,BbcLvl1_SHitsHist);
  BbcLvl1_NHitsHist = new TH1F("BbcLvl1_NHitsHist", 
			       "LL1 North Multiplicity",
                               70, 0, 70);
  BbcLvl1_NHitsHist->SetXTitle("North hits");
  se->registerHisto(this,BbcLvl1_NHitsHist);

  BbcLvl1_VertHistErr = new TH1F("BbcLvl1_VertHistErr",
                                 "VERTEX: PREDICTED - REAL",
                                 (int) (30 / ConvertToCM), -15, 15);
  BbcLvl1_VertHistErr->SetXTitle("dVtx, cm");
  se->registerHisto(this,BbcLvl1_VertHistErr);

  BbcLvl1_AverHistErr = new TH1F("BbcLvl1_AverHistErr",
                                 "TIME AVERAGE: PREDICTED - REAL",
                                 20, -10, 10);
  BbcLvl1_AverHistErr->SetXTitle("dTime, ns");
  se->registerHisto(this,BbcLvl1_AverHistErr);

  BbcLvl1_SHitsHistErr = new TH1F("BbcLvl1_SHitsHistErr",
				  "SOUTH MULT: PREDICTED - REAL",
				  20, -10, 10);
  BbcLvl1_SHitsHistErr->SetXTitle("dSHits");
  se->registerHisto(this,BbcLvl1_SHitsHistErr);

  BbcLvl1_NHitsHistErr = new TH1F("BbcLvl1_NHitsHistErr",
                                  "NORTH MULT: PREDICTED - REAL",
                                  20, -10, 10);
  BbcLvl1_NHitsHistErr->SetXTitle("dNHits");
  se->registerHisto(this,BbcLvl1_NHitsHistErr);

  BbcLvl1_STimeHistErr = new TH1F("BbcLvl1_STimeHistErr",
                                  "SOUTH TIME: PREDICTED - REAL",
                                  20, -10, 10);
  BbcLvl1_STimeHistErr->SetXTitle("dSTime, ns");
  se->registerHisto(this,BbcLvl1_STimeHistErr);

  BbcLvl1_NTimeHistErr = new TH1F("BbcLvl1_NTimeHistErr",
                                  "NORTH TIME: PREDICTED - REAL",
                                  20, -10, 10);
  BbcLvl1_NTimeHistErr->SetXTitle("dNTime, ns");
  se->registerHisto(this,BbcLvl1_NTimeHistErr);

  BbcLvl1_NChAllHist = new TH1F("BbcLvl1_NChAllHist",
                                "North Channels in All Events",
                                64, 0, 63.9);
  BbcLvl1_NChAllHist->SetXTitle("All North hits");
  se->registerHisto(this,BbcLvl1_NChAllHist);
  BbcLvl1_SChAllHist = new TH1F("BbcLvl1_SChAllHist",
                                "South Channels in All Events",
                                64, 0, 63.9);
  BbcLvl1_SChAllHist->SetXTitle("All South hits");
  se->registerHisto(this,BbcLvl1_SChAllHist);

  BbcLvl1_NChBadHist = new TH1F("BbcLvl1_NChBadHist",
                                "North Channels in Bad Events",
                                64, 0, 63.9);
  BbcLvl1_NChBadHist->SetXTitle("Bad North hits");
  se->registerHisto(this,BbcLvl1_NChBadHist);

  BbcLvl1_SChBadHist = new TH1F("BbcLvl1_SChBadHist",
                                "South Channels in Bad Events",
                                64, 0, 63.9);
  BbcLvl1_SChBadHist->SetXTitle("Bad South hits");
  se->registerHisto(this,BbcLvl1_SChBadHist);

  BbcLvl1_DataError = new TH1F("BbcLvl1_DataError", "Errors history",
                               10000, 0, 10000);
  BbcLvl1_DataError->SetXTitle("Event# div. by 100");
  se->registerHisto(this,BbcLvl1_DataError);
}

BbcLvl1Mon::~BbcLvl1Mon()
{
  delete BBCLvl1;
  return;
}

int
BbcLvl1Mon::BeginRun(const int runno)
{
  if (check_bbcll1_map_date())
    {
      if (BBCLvl1)
        {
          delete BBCLvl1;
        }
      BBCLvl1 = new BbcLl1(bbcll1_mapdir.c_str(), this);
    }
  OnlMonServer *se = OnlMonServer::instance();
  for (int i = 0; i < 32; i++)
    {
      int hexvalue = 1;
      hexvalue = hexvalue << i;
      string trigname = se->OnlTrig()->get_lvl1_trig_name(i);
      if (trigname.find("PPG") != string::npos)
        {
          trigignoremask |= hexvalue;
        }
    }
  return 0;
}

int BbcLvl1Mon::process_event(Event *event)
{

  evtcnt++;
  if (event->getEvtType() != 1)
    {
      return 0;

    }
  if (! bbc_in_partition)
    {
      Packet *gl1packet = event->getPacket(14001);
      if (gl1packet)
	{
	  if (gl1packet->iValue(GRAN_NO_BBC, GRANCTR) > 0)
	    {
	      bbc_in_partition = 1;
	    }
	  else
	    {
	      delete gl1packet;
	      sleep(1);
	      return 0;
	    }
          delete gl1packet;
	}
    }
  // remove laser triggers giving spike at 0
  OnlMonServer *se = OnlMonServer::instance();
  if (se->Trigger() & trigignoremask)
    {
      //     cout << "ditching trigger 0x" << hex << se->Trigger() << dec << endl;
      return 0;
    }

  int warnings = 0;
  int errors = 0;
  ostringstream msgstr;

  int Bbc_data_status = BBCLvl1->getDataFromBbcPacket(event);
  if (!Bbc_data_status)
    {
      OnlMonServer *se = OnlMonServer::instance();
      msgstr.str("");
      msgstr << "Event " << event->getEvtSequence()
	     << " has empty BBC data packet";
      se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msgstr.str(), NOBBCPKT);
    }
  else
    { //BBC data ok

      // Apply the FEM LUT to T2[128] (go from 12 bits to 8 bits...):
      BBCLvl1->bbcFEMLUT->fillLL1array(BBCLvl1->LL1T2, BBCLvl1->T2);

      BBCLvl1->calculate();

      int LL1_data_status = BBCLvl1->getDataFromLL1Packet(event);
      if (!LL1_data_status)
        {
          OnlMonServer *se = OnlMonServer::instance();
          msgstr.str("");
          msgstr << "Event " << event->getEvtSequence()
		 << " has empty LL1 data packet" ;
          se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msgstr.str(), NOBBCLL1PKT);
        }
      else
        { //LL1 data OK

          if (BBCLvl1->getMultiplicityNorth() == 0 && BBCLvl1->getMultiplicitySouth() == 0)
            {
              return 0;
            }
          goodevt++;

          //if(BBCLvl1->getMultNorthOut()!=0 && BBCLvl1->getMultSouthOut()!=0){

          NorthHits = BBCLvl1->getMultiplicityNorth();
          SouthHits = BBCLvl1->getMultiplicitySouth();
          Vertex = BBCLvl1->getZVertex();
          Average = BBCLvl1->getTimeAverage();
          ArmTimeSouth = BBCLvl1->getArmTimeSouth();
          ArmTimeNorth = BBCLvl1->getArmTimeNorth();
          VertexOK = BBCLvl1->getVertexOK();
          Vertex2OK = BBCLvl1->getVertex2OK();
          HitsInput = BBCLvl1->getRB11();
          BB_LL1_RB = BBCLvl1->getBBRB2();
          SouthHitsOut = BBCLvl1->getMultSouthOut();
          NorthHitsOut = BBCLvl1->getMultNorthOut();
          VertexOut = BBCLvl1->getVertexOut();
          AverageOut = BBCLvl1->getTimeAveOut();
          ArmTimeNorthOut = (2 * AverageOut + VertexOut) / 2;
          ArmTimeSouthOut = (2 * AverageOut - VertexOut) / 2;
          VertexOKOut = BBCLvl1->getVtxOKOut();
          HitsInputOut = BBCLvl1->getRB11Out();
          BB_LL1_RB_Out = BBCLvl1->getBBRB2Out();

          // if(BB_LL1_RB_Out==0) FailedRB++;
          //BbcLvl1_DataError->SetBinContent(3,FailedRB);
          //BbcLvl1_DataError->SetBinContent(2,EvtCount++);

          //if(VertexOut!=0 && AverageOut!=255 && BB_LL1_RB_Out!=0){
          // Fill temporary histograms:
          for (int iPmt = 0; iPmt < 64; iPmt++)
            {
              if (BBCLvl1->chann_stat[iPmt] > 0)
                {
                  BbcLvl1_NChAllHist->Fill(iPmt);
                  if (NorthHits != NorthHitsOut)
                    {
                      BbcLvl1_NChBadHist->Fill(iPmt);
                    }
                  if (SouthHits > 0 && NorthHits > 0 && VertexOK > 0 && abs(ArmTimeNorth - ArmTimeNorthOut) >= 2)
                    {
                      BbcLvl1_NChBadHist->Fill(iPmt);
                    }
                }
              if (BBCLvl1->chann_stat[iPmt + 64] > 0)
                {
                  BbcLvl1_SChAllHist->Fill(iPmt);
                  if (SouthHits != SouthHitsOut)
                    {
                      BbcLvl1_SChBadHist->Fill(iPmt);
                    }
                  if (SouthHits > 0 && NorthHits > 0 && VertexOK > 0 && abs(ArmTimeSouth - ArmTimeSouthOut) >= 2)
                    {
                      BbcLvl1_SChBadHist->Fill(iPmt);
                    }
                }
            }

          // Fill monitoring histograms:
          if (SouthHits)
            {
              BbcLvl1_SHitsHist->Fill(SouthHitsOut, 1.0);
            }
          if (NorthHits)
            {
              BbcLvl1_NHitsHist->Fill(NorthHitsOut, 1.0);
            }
          if (SouthHits > 0 && NorthHits > 0 && VertexOK > 0)
            {
	      float VertexDiff = Vertex - VertexOut;
              BbcLvl1_VertHistErr->Fill((VertexDiff), 1.0);
              BbcLvl1_VertHist->Fill(VertexOut*ConvertToCM, 1.0);
              BbcLvl1_STimeHistErr->Fill((ArmTimeSouth - ArmTimeSouthOut), 1.0);
              BbcLvl1_NTimeHistErr->Fill((ArmTimeNorth - ArmTimeNorthOut), 1.0);
              // Send error messages:
              // Vertex:
              if (abs((int)VertexDiff) == 2 && Vertex != 0)
                {
                  warnings++;
                }
              if (abs((int)VertexDiff) > 2 && Vertex != 0)
                {
                  errors++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Error: Vertex off vtxin: " << Vertex
			 << ", vtxout: " << VertexOut
			 << " Shit: " << NorthHits
			 << " Nhit: " << SouthHits
			 << " Shitout: " << NorthHitsOut
			 << " Nhitout: " << SouthHitsOut ;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_WARNING, msgstr.str(), VTXDIFF);
                }
            } //end if both arms hit and VertexOK
          if (SouthHits > 0 && NorthHits > 0 && Vertex2OK > 0)
            {
              BbcLvl1_Vert2Hist->Fill(VertexOut*ConvertToCM, 1.0);
	    }
          if (SouthHits > 0 || NorthHits > 0)
            {
              BbcLvl1_AverHistErr->Fill((Average - AverageOut), 1.0);
              if (SouthHits > 0 || SouthHitsOut > 0)
                {
                  BbcLvl1_SHitsHistErr->Fill((SouthHits - SouthHitsOut), 1.0);
                }
              if (NorthHits > 0 || NorthHitsOut > 0)
                {
                  BbcLvl1_NHitsHistErr->Fill((NorthHits - NorthHitsOut), 1.0);
                }
              if (SouthHits > 0 && NorthHits > 0 && VertexOK > 0)
                {
                  BbcLvl1_AverHist->Fill(AverageOut*ConvertToNS, 1.0);
                }

              // Error messages:
              // Time Average:
              if (abs(Average - AverageOut) == 2 && Average != 255)
                {
                  warnings++;
                }
              if (abs(Average - AverageOut) > 2 && Average != 255)
                {
		  //TRK removed because of bad monitor FIFO 12/31/2009
                  //errors++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Error: Time Average off Averin: " << Average
			 << ", Averout: " << AverageOut ;
		  //TRK removed because of bad monitor FIFO 12/31/2009
                  //OnlMonServer *se = OnlMonServer::instance();
                  //se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msgstr.str(), TIMEDIFF);
                }

              // South Multiplicity:
              if (abs(SouthHits - SouthHitsOut) == 1)
                {
                  warnings++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Warning: North Multiplicity shift" ;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_WARNING, msgstr.str(), TIMESOUTH);
                }
              if (abs(SouthHits - SouthHitsOut) > 1)
                {
		  //TRK removed because of bad monitor FIFO 12/31/2009
                  //errors++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << "- Error: North Multiplicity off" ;
                  //TRK removed because of bad monitor FIFO 12/31/2009
                  //OnlMonServer *se = OnlMonServer::instance();
                  //se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msgstr.str(), MULTSOUTH);
                }

              // North Multiplicity:
              if (abs(NorthHits - NorthHitsOut) == 1)
                {
                  warnings++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Warning: South Multiplicity shift" ;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_WARNING, msgstr.str(), MULTNORTH);
                }
              if (abs(NorthHits - NorthHitsOut) > 1)
                {
		  //TRK removed because of bad monitor FIFO 12/31/2009
                  //errors++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Error: South Multiplicity off" ;
                  //TRK removed because of bad monitor FIFO 12/31/2009
                  //OnlMonServer *se = OnlMonServer::instance();
                  //se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msgstr.str(), MULTNORTH);
                }

              // VertexOK bit:
              if (VertexOK != VertexOKOut && Vertex != 0 && Average != 255)
                {
                  errors++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Error: VertexOK unmatch" ;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_WARNING, msgstr.str(), VERTEXOK);
                }

              // Lvl1 reduced Bits:
              if (BB_LL1_RB != BB_LL1_RB_Out && VertexOK != 0)
                {
                  errors++;
                  msgstr.str("");
                  msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
			 << " - Error:  BbcLvl1 Reduced Bits unmatch " << BB_LL1_RB
			 << "/" << BB_LL1_RB_Out ;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msgstr.str(), REDBITS);
                }

              // Hits Input:
              if (HitsInput != HitsInputOut)
                {
                  warnings++;
                }
            } //end if Nhits>0 or SHits>0

          if (warnings > 3)
            {
              msgstr.str("");
              msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
		     << " - Error: Too many warnings!" ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_WARNING, msgstr.str(), NUMWARN);
            }
          if (errors > 2)
            {
              msgstr.str("");
              msgstr << "BbcLvl1Mon Event " << event->getEvtSequence()
		     << "- Fatal: Too many errors!" ;
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_SEVEREERROR, msgstr.str(), NUMFATAL);
            }
        } //end else
    } //end else

  // Clear the error histogram every 1000 events

  if((goodevt%1000)==0){
    BbcLvl1_DataError->Reset(); 

    if(goodevt!=0) {
      msgstr.str("");
      msgstr << "BbcLvl1Mon - running bad event fraction is " << (float)badevt/(float)goodevt;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_SEVEREERROR, msgstr.str(), NUMFATAL);
      if (BbcLvl1_DataError->GetEntries() > 10)
	{
	  SetStatus(OnlMon::ERROR); // set subsystem status to error for this run
	}
    }

  }

  if (errors > 0)
    {
      badevt++;
      BbcLvl1_DataError->Fill(goodevt / 100);
    }
  return 0;
}

int
BbcLvl1Mon::Reset()
{
  // reset the flag which says if the bbc is in the current partition
  bbc_in_partition = 0;
  // reset our internal counters
  evtcnt = 0;
  return 0;
}

int
BbcLvl1Mon::check_bbcll1_map_date()
{
  DIR *dirp;
  struct dirent *entry;
  struct stat attrib;
  time_t newest_file = 0;
  if ( (dirp = opendir(bbcll1_mapdir.c_str())) )
    {
      while ((entry = readdir(dirp)))
        {
          string fullname = bbcll1_mapdir + "/" + entry->d_name;
          stat(fullname.c_str(), &attrib);
	  // S_ISREG is a posix macro which checks if this is a regular file (exclude dirs here)
          if (! S_ISREG(attrib.st_mode))
            {
              continue;
            }

          time_t mtime = attrib.st_mtime;
	  if (mtime > newest_file)
	    {
	      newest_file = mtime;
	    }
        }
      closedir(dirp);
    }
  if (bbcll1_mapdate < newest_file)

    {
      bbcll1_mapdate = newest_file;
      return 1;
    }
  return 0;
}
