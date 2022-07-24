#include <MutrMonConsts.h>
#include <MutrMon.h>
#include <MutCalibStrip.hh>
#include <MutDCMChannelMap.h>

#include <MutStrip.h>

#include <OnlMonServer.h>
#include <RunDBodbc.h>

#include <msg_profile.h>

#include <phool.h>
#include <Event.h>
#include <packet.h>


#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


const static int mutr_debug_on = 0; // AMU cell error warnings - don't need these
const static int max_strips_in_cluster = 4;

// constants for sample cuts
const static float Adc23Max = 0.14;
const static float Adc12Slope = 5.5;
const static float Adc12Offset = 0.32;
const static float Adc12Cut = 0.50;
const static float Adc14Slope = -6.7;
const static float Adc14Offset = 0.41;
const static float Adc14Cut = 0.50;

using namespace std;

MutrMon::MutrMon(): OnlMon("MUTRMON")
{
  MutrInit();
}

int
MutrMon::MutrInit()
{
  rejection_mode = true;
  int iret = 0;
  iret += initMappingObjy();
  //iret += initMapping();
  iret += initHistos();
  //  iret += getCalibrations(); // default time should give the latest calibrations
  NumEvtIn = 0;
  NumEvtAna = 0;
  return iret;
}

int
MutrMon::initMapping()
{

  // read mapping files
  mapMutCh.clear(); // just clear the map, it should now be empty

  int iret = 0;
  // arm0/1 (South): Station 1,2,3
  string mutcalibdir;
  if (!getenv("MUTRCALIBDIR"))
    {
      mutcalibdir = ".";
    }
  else
    {
      mutcalibdir = getenv("MUTRCALIBDIR");
    }
  string mutcalib = mutcalibdir + "/mutr_mapsouth.dat";
  iret += txtGetFullMap(0, mutcalib.c_str());
  mutcalib = mutcalibdir + "/mutr_mapnorth.dat";
  iret += txtGetFullMap(1, mutcalib.c_str());

  return iret;
}

int
MutrMon::initMappingObjy(const int year, const int month,
                         const int day, const int hour,
                         const int min, const int sec)
{
  // read mapping files
  mapMutCh.clear(); // just clear the map, it should now be empty

  int iret = 0;
  printf("Getting MUTR mapping: Time is %d %d %d %d %d %d\n",year,month,day,hour,min,sec);
  PHTimeStamp tsearch(year, month, day, hour, min, sec);

  MutArm *pSouth = new MutArm((MUTGEOM::ArmNumber)0, tsearch);
  MutArm *pNorth = new MutArm((MUTGEOM::ArmNumber)1, tsearch);

  int packet, packetid, channel;
  int nactive[2] = {0, 0};

  MutDCMChannelMap *pChSouth = new MutDCMChannelMap(pSouth);
  for (packet = 0; packet < NPACKETSARM[0]; packet++)
    {
      packetid = FIRSTPACKET[0] + packet;
      mutCh.setPacketid(packetid);
      for (channel = 0; channel < NCHANNELS; channel++)
        {
          MutStrip *southstrip = pChSouth->getMutStrip(packetid, channel);
          if (southstrip)
            {
              //if (! southstrip->ChannelIsDead() ) {
              // set all the needed indices
              mutCh.setChannel(channel);

              mutCh.setArm(southstrip->getArm());
              mutCh.setStation(southstrip->getStation());
              mutCh.setOctant(southstrip->getOctant());
              mutCh.setHalfOctant(southstrip->getHalfOctant());
              mutCh.setGap(southstrip->getGap());
              mutCh.setPlane(southstrip->getCathode());
              mutCh.setStrip(southstrip->getStrip());
              // calc the BackPlaneChannel index also
              mutCh.calcBackPlaneChannel();

              // add to the set
              // if some other strip, with the same indices are already
              // in the set, a warning will be issued
              iter = mapMutCh.find(mutCh.getChannelId());
              if ( iter == mapMutCh.end() )
                { // no Doppelganger, insert this one
                  mapMutCh.insert(pair<int, MutOnlineChannel>(mutCh.getChannelId(), mutCh));
                }
              else
                {
		  ostringstream msg;
                  msg << "insert WARNING: A strip with the same indices has already been inserted";
              OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_WARNING, msg.str(),1);
                  mutCh.writeChannelMap();
                }
              nactive[0]++;
              //}
            }
        }
    }

  MutDCMChannelMap *pChNorth = new MutDCMChannelMap(pNorth);
  for (packet = 0; packet < NPACKETSARM[1]; packet++)
    {
      packetid = FIRSTPACKET[1] + packet;
      mutCh.setPacketid(packetid);
      for (channel = 0; channel < NCHANNELS; channel++)
        {
          MutStrip *northstrip = pChNorth->getMutStrip(packetid, channel);
          if (northstrip)
            {
              //if (! northstrip->ChannelIsDead() ) {
              // set all the needed indices
              mutCh.setChannel(channel);

              mutCh.setArm(northstrip->getArm());
              mutCh.setStation(northstrip->getStation());
              mutCh.setOctant(northstrip->getOctant());
              mutCh.setHalfOctant(northstrip->getHalfOctant());
              mutCh.setGap(northstrip->getGap());
              mutCh.setPlane(northstrip->getCathode());
              mutCh.setStrip(northstrip->getStrip());
              // calc the BackPlaneChannel index also
              mutCh.calcBackPlaneChannel();

              // add to the set
              // if some other strip, with the same indices are already
              // in the set, a warning will be issued
              iter = mapMutCh.find(mutCh.getChannelId());
              if ( iter == mapMutCh.end() )
                { // no Doppelganger, insert this one
                  mapMutCh.insert(pair<int, MutOnlineChannel>(mutCh.getChannelId(), mutCh));
                }
              else
                {
		  ostringstream msg;
                  msg.str("");
                  msg << "insert WARNING: A strip with the same indices has already been inserted";
              OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_WARNING, msg.str(),1);
                  mutCh.writeChannelMap();
                }
              nactive[1]++;
              //}
            }
        }
    }
  printf(" nactive Arm 0: %d Arm 1: %d\n",nactive[0],nactive[1]);

  delete pSouth;
  delete pChSouth;
  delete pNorth;
  delete pChNorth;

  return iret;
}

int
MutrMon::initHistos()
{
  ostringstream id, info;
  int nchan;
  float min, max;

  // let's register all these histos as we go
  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  mutH1NumEvent = new TH1F("mutH1NumEvent",
                           "Number of processed events",
                           1, 0.5, 1.5);
  Onlmonserver->registerHisto(this,mutH1NumEvent);

  const char *CHARM[] =
    { "S", "N"
    }
  ; // arm string id

  for (int i = 0; i < NARMS; i++)
    {
      // zero-suppresion ratio
      id.str("");
      info.str("");
      id << "mutH1ZSRatio" << i;
      info << "Zero-Supp Ratio : Arm " << CHARM[i];
      nchan = 1000; // fine-tune?
      min = 0.0005; // fine-tune?
      max = 1.0005;
      // the occupancy is between 0 (no channels fired), and 1 - all fired
      // plot is done in log-log scale
      mutH1ZSRatio[i] = new TH1F(id.str().c_str(), info.str().c_str(),
                                 nchan, min, max);
      Onlmonserver->registerHisto(this,mutH1ZSRatio[i]);

      // nfired chan per packet per event
      id.str("");
      info.str("");
      id << "mutHprofPktNchan" << i;
      info << "Zero-Supp. Ratio : Arm " << CHARM[i];
      nchan = NPACKETSARM[i];
      min = FIRSTPACKET[i] - 0.5;
      max = min + nchan;

      mutHprofPktNchan[i] = new TProfile(id.str().c_str(), info.str().c_str(),
                                         nchan, min, max);
      Onlmonserver->registerHisto(this,mutHprofPktNchan[i]);

      // packet errors per event
      id.str("");
      info.str("");
      id << "mutHprofPktErr" << i;
      info << "AMU cell errors vs packet : Arm " << CHARM[i];
      nchan = NPACKETSARM[i];
      min = FIRSTPACKET[i] - 0.5;
      max = min + nchan;

      mutHprofPktErr[i] = new TProfile(id.str().c_str(), info.str().c_str(),
                                       nchan, min, max);
      Onlmonserver->registerHisto(this,mutHprofPktErr[i]);

      //hits per plane
      id.str("");
      info.str("");
      id << "mutHprofHits" << i;
      info << "NFiredCh vs Plane : Arm " << CHARM[i];
      nchan = NOCTANTS * (NPLANES[0] + NPLANES[1] + NPLANES[2]);
      min = - 0.5;
      max = min + nchan;

      mutHprofHits[i] = new TProfile(id.str().c_str(), info.str().c_str(),
                                     nchan, min, max);
      Onlmonserver->registerHisto(this,mutHprofHits[i]);

      //sample distr
      id.str("");
      info.str("");
      id << "mutH1Signal" << i;
      info << "Signal distribution : Arm " << CHARM[i];
      nchan = 100;
      min = -500;
      max = 2500;

      mutH1Signal[i] = new TH1F(id.str().c_str(), info.str().c_str(),
                                nchan, min, max);
      Onlmonserver->registerHisto(this,mutH1Signal[i]);

      // amplitude vs timing
      nchan = 11;
      min = -0.5;
      max = 10.5;
      id.str("");
      info.str("");
      id << "mutHprofAmplTime" << i;
      info << "Amplitude vs Time : Arm " << CHARM[i];

      mutHprofAmplTime[i] = new TProfile(id.str().c_str(), info.str().c_str(),
                                         nchan, min, max);
      Onlmonserver->registerHisto(this,mutHprofAmplTime[i]);

      id.str("");
      info.str("");
      id << "mutHprofAmplTime_nstr" << i;
      info << "Amplitude vs Time : Arm (non-stereo)" << CHARM[i];
      mutHprofAmplTime_nstr[i] = new TProfile(id.str().c_str(), info.str().c_str(),
                                         nchan, min, max);
      Onlmonserver->registerHisto(this,mutHprofAmplTime_nstr[i]);

      //cluster adc distr
      id.str("");
      info.str("");
      id << "mutH1ClustAdc" << i;
      info << "Cluster Adc : Arm " << CHARM[i];
      nchan = 100;
      min = -500;
      max = 2500;

      mutH1ClustAdc[i] = new TH1F(id.str().c_str(), info.str().c_str(),
                                  nchan, min, max);
      Onlmonserver->registerHisto(this,mutH1ClustAdc[i]);

      //peak adc distr
      id.str("");
      info.str("");
      id << "mutH1PeakAdc" << i;
      info << "Peak Adc : Arm " << CHARM[i];
      nchan = 300;
      min = -500;
      max = 2500;

      mutH1PeakAdc[i] = new TH1F(id.str().c_str(), info.str().c_str(),
                                 nchan, min, max);
      Onlmonserver->registerHisto(this,mutH1PeakAdc[i]);

      //peak adc distr for the stations
      for (int ista = 0; ista < NSTATIONS; ista++)
        {
          id.str("");
          info.str("");
          id << "mutH1PeakAdcSta" << i << ista;
          info << "Peak Adc : Arm " << CHARM[i] << " Sta: " << ista;

          mutH1PeakAdcSta[i][ista] = new TH1F(id.str().c_str(), info.str().c_str(),
                                              nchan, min, max);
          Onlmonserver->registerHisto(this,mutH1PeakAdcSta[i][ista]);

          for (int ioct = 0; ioct < NOCTANTS; ioct++)
            {
              id.str("");
              info.str("");
              id << "mutH1PeakAdcStaOct" << i << ista << ioct;
              info << "Peak Adc : Arm " << i << " Sta: " << ista << " Oct: " << ioct;

              mutH1PeakAdcStaOct[i][ista][ioct] = new TH1F(id.str().c_str(), info.str().c_str(),
							   nchan, min, max);
              Onlmonserver->registerHisto(this,mutH1PeakAdcStaOct[i][ista][ioct]);
            }

        }
    }
  for (int iarm=0; iarm<NARMS;iarm++)
    {
      id.str("");
      id << "mutPacketActive" << iarm;
      info.str("");
      info << "Active Packets " << CHARM[iarm];
      float maxval = NPACKETSARM[iarm] + 0.5;
      mutActivePkt[iarm] = new TH1F(id.str().c_str(),info.str().c_str(),NPACKETSARM[iarm],0.5,maxval);
      Onlmonserver->registerHisto(this,mutActivePkt[iarm]);
    }
  return 0;
}

int
MutrMon::txtGetMap(const int iarm, const int istation,
                   const char* infile)
{
  // the infile is assumed to follow the format of aX_sY_map.dat
  // where X = 1,2, Y = 1,2,3
  // istation == Y - 1 (i.e numbering starts from 0)

  // note that the map files only contain 1/4 of S1 (octant 0,1)
  // and 1/8 of S2/S3 (halfoctant 0,1, unknown octant)
  // don't include -1 entries for octants etc. Those channels
  // are not connected to any strips (often come in groups of 16..)

  ifstream fin(infile);
  if (!fin)
    {
      ostringstream msg;
      msg.str("");
      msg << "can't open " << infile << "for input";
              OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_ERROR, msg.str(),1);
      return 1;
    }

  int oct, qoct, thisoct, thispacketid, thisredpacketid;

  // init values that are not on file
  mutCh.setArm(iarm);
  mutCh.setStation(istation);

  // read from the file as long as we can
  // add strips to set as we go along
  int nlines = 0;
#ifdef DEBUG

  printf(" - reading from %s\nFirst 10 lines on file will be printed\n",infile.c_str());
#endif

  while ( fin.good() && nlines < (NPACKETS[iarm*3 + istation]*NCHANNELS) )
    {
      mutCh.readChannelMap(fin);
      if (! fin.good() )
        {
          break;
        }
#ifdef DEBUG
      // echo first ten lines of file
      if (nlines < 10)
        {
          mutCh.writeChannelMap();
        }
#endif
      // skip non-active strips
      if (mutCh.getStrip() == -1)
        {
          //	  cout << PHWHERE << " skipping this one" << endl;
          continue;
        }

      // make copies for all octants in a loop
      thisoct = mutCh.getOctant();
      thispacketid = mutCh.getPacketid();
      thisredpacketid = mutCh.getReducedPacket( thispacketid );

      for (qoct = 0; qoct < NPARTS[istation]; qoct++)
        {
          if (istation == 0)
            {
              oct = S1OCT[iarm * 8 + 2 * qoct + thisoct]; // thisoct is 0 or 1
            }
          else
            {
              oct = qoct;
            }
          mutCh.setOctant(oct);
          // new octant value changes the real packet id
          // use the reduced one to get it
          thispacketid = mutCh.getRealPacket( thisredpacketid );
          mutCh.setPacketid(thispacketid);

          // add the read strip to the set
          // if some other strip, with the same indices are already
          // in the set, a warning will be issued
          iter = mapMutCh.find(mutCh.getChannelId());
          if ( iter == mapMutCh.end() )
            { // no Doppelganger, insert this one
              mapMutCh.insert(pair<int, MutOnlineChannel>(mutCh.getChannelId(), mutCh));
            }
          else
            {
		  ostringstream msg;
                  msg << "insert WARNING: A strip with the same indices has already been inserted";
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_WARNING, msg.str(),1);
              mutCh.writeChannelMap();
            }
        }
      nlines++;
    }
  fin.close();

#ifdef DEBUG

  printf(" size of set: %d\nFirst 10 lines in set will be printed\n", mapMutCh.size());

  // print the first ten members again
  nlines = 0;
  for (iter = mapMutCh.begin(); iter != mapMutCh.end(); iter++)
    {
      if (nlines < 10)
        {
          ((*iter).second).writeChannelMap();
        }
      nlines++;
    }
#endif
  return 0;
}

int
MutrMon::txtGetFullMap(const int iarm,
                       const char* infile)
{
  // the infile is assumed to follow the full input format
  ifstream fin(infile);
  if (!fin)
    {
  ostringstream msg;
      msg.str("");
      msg << "can't open " << infile
	  << "for input";
              OnlMonServer *se = OnlMonServer::instance();

      se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_ERROR, msg.str(),1);
      return 1;
    }

  // read from the file as long as we can
  // add strips to set as we go along
  int nlines = 0;
#ifdef DEBUG

  msg.str("");
  msg << PHWHERE << " - reading from " << infile
      << endl << "First 10 lines on file will be printed " << endl;
  send_message(MSG_SEV_INFORMATIONAL, msg.str());
#endif

  while ( fin.good() )
    {
      mutCh.readFullChannelMap(fin);
      if (! fin.good() )
        {
          break;
        }
#ifdef DEBUG
      // echo first ten lines of file
      if (nlines < 10)
        {
          mutCh.writeChannelMap(msg);
        }
#endif
      // add the read strip to the set
      // if some other strip, with the same indices are already
      // in the set, a warning will be issued
      iter = mapMutCh.find(mutCh.getChannelId());
      if ( iter == mapMutCh.end() )
        { // no Doppelganger, insert this one
          mapMutCh.insert(pair<int, MutOnlineChannel>(mutCh.getChannelId(), mutCh));
        }
      else
        {
		  ostringstream msg;
                  msg << "insert WARNING: A strip with the same indices has already been inserted";
              OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_WARNING, msg.str(),1);
                  mutCh.writeChannelMap();
        }

      nlines++;
    }
  fin.close();

#ifdef DEBUG

  msg.str("");
  msg << PHWHERE << " size of set: " << mapMutCh.size() << endl
      << "First 10 lines in set will be printed " << endl;
  send_message(MSG_SEV_INFORMATIONAL, msg.str());

  // print the first ten members again
  nlines = 0;
  for (iter = mapMutCh.begin(); iter != mapMutCh.end(); iter++)
    {
      if (nlines < 10)
        {
          ((*iter).second).writeChannelMap(msg);
        }
      nlines++;
    }
#endif

  return 0;
}

// put all the channels
// from the set into a text file. All in one
int
MutrMon::txtPutMap(const char* outfile)
{
  ostringstream msg;
  ofstream fout(outfile);
  if (!fout)
    {
  ostringstream msg;
      msg.str("");
      msg << "can't open " << outfile
	  << "for output";
              OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_ERROR, msg.str(),1);
      return 1;
    }
  // go thru the set and print all to fout

#ifdef DEBUG
  msg.str("");
  msg << PHWHERE << " - writing to " << outfile
      << ", size of set: " << mapMutCh.size() << endl;
  send_message(MSG_SEV_INFORMATIONAL, msg.str());
#endif

  for (iter = mapMutCh.begin(); iter != mapMutCh.end(); iter++)
    {
      ((*iter).second).writeChannelMap();
    }

  fout.close();
  return 0;
}

int
MutrMon::getCalibrations(const int year, const int month,
                         const int day, const int hour,
                         const int min, const int sec)
  //note that usually, data is not taken at the default time
{
  MutCalibStrip* calib = new MutCalibStrip;
  printf("Getting MUTR calibrations: Time is %d %d %d %d %d %d\n",year,month,day,hour,min,sec);
  PHTimeStamp tsearch(year, month, day, hour, min, sec);
  calib->dbGetAll(tsearch);   // use database

  // get calib info from all channels in map
  int nchannels = 0;
  const PdbMutCalibStrip *pstrip = 0;
  // maps are not mutable, so we'll need to overwrite old map with a new one
  map_type newmap;
  newmap.clear();

  int nwarn = 0;
  const int MAXWARN = 10; // suppress endless warnings..
  const int MAXTICS_SOUTHONLY = 1012539600; // eq. to PHTimeStamp t(2002,2,1,0,0,0)
  // - before this time, there was only one arm, so don't expect mapsizes to match

  for (iter = mapMutCh.begin(); iter != mapMutCh.end(); iter++)
    {
      mutCh = ((*iter).second);
      if (tsearch.getTics() < MAXTICS_SOUTHONLY && mutCh.getArm() == 1)
        {
          continue; // don't even bother to look for North strips in old data
        }
      pstrip = calib->getPdbMutCalibStrip(mutCh.getArm(),
                                          mutCh.getStation(),
                                          mutCh.getOctant(),
                                          mutCh.getHalfOctant(),
                                          mutCh.getGap(),
                                          mutCh.getPlane(),
                                          mutCh.getStrip());
      if (pstrip)
        { // calibration values were in database for this guy
          // now copy over the values
          mutCh.copyCalibInfo(pstrip);
          // also insert in newmap
          newmap.insert(pair<int, MutOnlineChannel>(mutCh.getChannelId(),
						    mutCh));
        }
      else
        {
          if (nwarn < MAXWARN)
            {
		  ostringstream msg;
              msg.str("");
              msg << "calibration info missing for this channel";
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_WARNING, msg.str(),1);
              mutCh.writeChannelMap();
              nwarn++;
            }
        }
#ifdef DEBUG
      // echo info for first channels
      if (nchannels < 10)
        {
          msg.str("");
          msg << PHWHERE << endl;
          mutCh.write(msg);
        }
#endif
      nchannels++;
    }

  // overwrite old map if the sizes match, or if the timestamp is old enough to
  // correspond to one arm only
  if ( mapMutCh.size() == newmap.size() || tsearch.getTics() < MAXTICS_SOUTHONLY )
    {
      mapMutCh = newmap;
      printf("overwriting map with database/calib info %d entries stored\n",nchannels);
      newmap.clear();
    }
  else
    {
      printf("not overwriting map with database/calib info %d size mismatch\n",nchannels);
      newmap.clear();
    }

  if (calib)
    {
      delete calib;
    }
  printf("MUTR calibrations retrieved\n");
  return 0;
}

int
MutrMon::BeginRun(const int runno)
{
  OnlMonServer *se = OnlMonServer::instance();

  for (int iarm=0; iarm < NARMS; iarm++)
    {
      for (int ipkt = 0; ipkt < NPACKETSARM[iarm];ipkt++)
	{
	  unsigned int pktid = FIRSTPACKET[iarm] + ipkt;
	  if (se->IsPacketActive(pktid))
	    {
	      mutActivePkt[iarm]->SetBinContent(ipkt,1);
	    }
	  else
	    {
	      printf("MutrMon: packet %u is disabled\n",pktid);
	    }
	}
    }

  return 0;
}

int MutrMon::process_event(Event *e)
{
  //  NumEvtIn++;
  
//   OnlMonServer *Onlmonserver = OnlMonServer::instance();
//   if ((RunType=="PHYSICS" || RunType=="ZEROFIELD") &&
//       !Onlmonserver->Trigger("BBCLL1(>0 tubes)") &&
//       !Onlmonserver->Trigger("BBCLL1(noVertexCut)")) return 0;   // run9 500GeV
  //  if (!Onlmonserver->Trigger("(MUIDLL1_N1D||S1D)&BBCLL1")) return 0;
  //  &&(!Onlmonserver->Trigger("MUIDLL1_S1D"))
  //  &&(!Onlmonserver->Trigger("MUIDLL1_N1D&MUIDLL1_S1D")) )  // run7 cosmicsi
  // if (! Onlmonserver->Trigger("ONLMONBBCLL1") ) // used for run5
  // if (! Onlmonserver->Trigger("ERTLL1_4x4c&BBCLL1") ) // used for run6 
  // if (!Onlmonserver->Trigger("ERTLL1_2x2") ) // run6 62GeV
//  if( (!Onlmonserver->Trigger("ERTLL1_2x2"))&&(!Onlmonserver->Trigger("BBCLL1(>0 tubes)"))&&(!Onlmonserver->Trigger("BBCN||BBCS")) ) // run6 62GeV
//    { // no BBC live interaction trigger - skip this event
//             return 0;
//    }
//   NumEvtAna++;

  mutH1NumEvent->Fill(1); // one entry per event

  int fired_strips[NARMS] = {0, 0}; // total # of strip hits/event
  int fired_pl[NARMS][NSTATIONS][NOCTANTS][NPLANES[0]]; // stat/oct/plane
  memset(fired_pl, 0, sizeof(fired_pl)); // set all to 0
  // not only fired, but also clusters = hits
  int hit_pl[NARMS][NSTATIONS][NOCTANTS][NPLANES[0]]; // stat/oct/plane
  memset(hit_pl, 0, sizeof(hit_pl)); // set all to 0

  int celladdrerror = 0; // 0=good; >0 =bad
  int arm, sta, oct, pla;
  int thisarm, thissta;

  int adc[NSAMPLES];
  float rmsx, pedx;
  float adcr[NSAMPLES];
  int iamu, amucell[NSAMPLES];
  int icelldiff[NSAMPLES]; // difference between samples 0&3, 0&1,1&2, 2&3.
  const int rdiff[3] =
    {
      5, 6, 7
    };
  const int ADCCUT = 50;
  const int nstro_cath[3]={1,0,1};

  // ok, here we go; go over all packets and channels
  MutOnlineChannel low;
  MutOnlineChannel high;
  low.setChannel(0);
  high.setChannel(NCHANNELS - 1);

  iter = mapMutCh.begin();
  int packetid, channel, nchannelson;
  float chanfraction;
  int nclusters;
  MutOnlineClust clust;
  double linratio;

  // loop through mutr packets
  while ( iter != mapMutCh.end() )
    {
      mutCh = (*iter).second;
      arm = mutCh.getArm();
      sta = mutCh.getStation();
      stripset.clear();

      // go thru all packets in this arm, sta
      thisarm = arm;
      thissta = sta;
      while (arm == thisarm && sta == thissta
             && iter != mapMutCh.end() )
        {

          packetid = mutCh.getPacketid();
          high.setPacketid(packetid);
          nchannelson = 0;

          Packet *p = e->getPacket(packetid);
          if ( p && p->getCheckSumStatus() == -1)
            {
  ostringstream msg;
              msg << "Event " << e->getEvtSequence()
		  << " Packet " << packetid
		  << "fails dcm checksum";
              OnlMonServer *se = OnlMonServer::instance();
              se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_ERROR, msg.str(),2);
              delete p;
              p = NULL;
            }

          if (p)
            {
              low.setPacketid(packetid);
              //get lower and upper keys/ids: which channels belong to this packet?
              // since mapMutCh does not change (and therfore the upper_bound)
              // pulling it out of the for loop saves a tremendous amount of
              // cpu time (calculating upper_bound for every iteration was
              // responsible for >20% of the total cpu time of this monitor!!!)
              map_iterator iter2 = mapMutCh.upper_bound(high.getChannelId());
              for (iter = mapMutCh.lower_bound(low.getChannelId());
                   iter != iter2; iter++)
                {
                  mutCh = (*iter).second;
                  channel = mutCh.getChannel();
                  oct = mutCh.getOctant();

                  // get the samples for this channel
                  adc[2] = p->iValue(channel, 2) & MASK;

                  if ( adc[2] != 0 )
                    {  // skip if zero
                      pla = mutCh.getGap() * 2 + mutCh.getPlane();

                      fired_pl[thisarm][thissta][oct][pla]++;
                      fired_strips[thisarm]++;
                      nchannelson++;

                      pedx = mutCh.get_pedestal();

                      for (int isample = 0; isample < NSAMPLES; isample++)
                        {
                          adc[isample] = p->iValue(channel, isample) & MASK;
                          adcr[isample] = pedx - adc[isample];
                        }

                      // argument to good_data check is 1) raw adc values
                      // 2) reversed around pedestal
                      // This is somewhat redudant but convenient
                      bool good_data = sample_is_good(adc, adcr);

                      if (adcr[2] > -200)
                        {
                          mutH1Signal[thisarm]->Fill(adcr[2]); // for all stations
                        }
#ifdef DEBUG
                      else
                        {
		  ostringstream msg;
                          msg.str("");
                          msg << " Strange: packetid " << packetid
			      << " channel " << channel
			      << " adc[2] " << adc[2]
			      << " adcr[2] " << adcr[2]
			      << " pedx " << pedx;
              OnlMonServer *se = OnlMonServer::instance();
                          se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_ERROR, msg.str(),3);
                        }
#endif
                      rmsx = mutCh.get_rms();
                      if (adcr[2] > 3*rmsx &&
                          (good_data || !rejection_mode) )
                        {
                          linratio = abs(adcr[3] / adcr[2] - 1.0);
                          if (linratio < LINLIMIT)
                            {
                              mutStrip.oct = oct;
                              mutStrip.hoct = mutCh.getHalfOctant();
                              mutStrip.pla = pla;
                              mutStrip.strip = mutCh.getStrip();
                              mutStrip.adc = adcr[2];
                              stripset.insert(mutStrip);
                              if ( adcr[2] > ADCCUT )
                                {
				  if (nstro_cath[sta] == mutCh.getPlane() ) {
				    mutHprofAmplTime_nstr[thisarm]->Fill(0., adcr[0]);
				    mutHprofAmplTime_nstr[thisarm]->Fill(rdiff[0], adcr[1]);
				    mutHprofAmplTime_nstr[thisarm]->Fill(rdiff[1], adcr[2]);
				    mutHprofAmplTime_nstr[thisarm]->Fill(rdiff[2], adcr[3]);
				  }else{
    				    mutHprofAmplTime[thisarm]->Fill(0., adcr[0]);
				    mutHprofAmplTime[thisarm]->Fill(rdiff[0], adcr[1]);
				    mutHprofAmplTime[thisarm]->Fill(rdiff[1], adcr[2]);
				    mutHprofAmplTime[thisarm]->Fill(rdiff[2], adcr[3]);
				  }
                                }
                            }
                        }
                    } // passed zero-supp channels only
                } // channel loop

              // packet check
              celladdrerror = 0;
              //check cell vs packet

              // The following code is modified to implement proper amu cell check for
              // 4 buffering. The code which I commented out existed before run05 and
              // was not good for multievent buffering. Imran Younus

              for (iamu = 0; iamu < NSAMPLES; iamu++)
                {
                  amucell[iamu] = p->iValue(iamu, "AMU") & AMUMASK;
                  //cout << amucell[iamu] << "\t";
                }

              icelldiff[0] = amucell[3] - amucell[0];
              icelldiff[1] = amucell[1] - amucell[0];
              icelldiff[2] = amucell[2] - amucell[1];
              icelldiff[3] = amucell[3] - amucell[2];

              for (int i = 0; i < 4; i++)
                if (icelldiff[i] < 0)
                  icelldiff[i] += 64;

              bool goodAMUs = (icelldiff[0] >= 7 && icelldiff[0] <= 18 &&
                               icelldiff[1] >= 5 && icelldiff[1] <= 16 &&
                               icelldiff[2] >= 1 && icelldiff[2] <= 10 &&
                               icelldiff[3] >= 1 && icelldiff[3] <= 10 );


              //               for (iamu = 0; iamu < (NSAMPLES - 1); iamu++)
              //                 {
              //                   icelldiff[iamu] = amucell[iamu + 1] - amucell[iamu];
              // 		  cout << icelldiff[iamu] << "\t";
              //
              // 		  // identical amu-cells/samples are really bad..
              //                   if (icelldiff[iamu] == 0 )

              if (!goodAMUs)
                {
                  // 		  for (iamu = 0; iamu < NSAMPLES; iamu++)
                  // 		    cout << amucell[iamu] << "\t";

                  celladdrerror++;

                  if (mutr_debug_on)
                    {
		  ostringstream msg;
                      msg << "MUTR : INCORRECT CELL ADDRESSES !!! pkt = "
			  << packetid
			  << ", cells = " << amucell[0] << " " << amucell[1] << " "
			  << amucell[2] << " " << amucell[3];
              OnlMonServer *se = OnlMonServer::instance();
                      se->send_message(this,MSG_SOURCE_MUTR,MSG_SEV_ERROR, msg.str(),4);
                    }
                } // loop amu cells

              mutHprofPktErr[thisarm]->Fill(packetid, celladdrerror);

              delete p;
            } // if p
          // check nchannelson outside if (p) condition to also include packets dropped due to zero suppression
          chanfraction = (float) nchannelson / (float) NCHANNELS;
          mutHprofPktNchan[thisarm]->Fill(packetid, chanfraction);

          // move the iterator on to the next packet
          iter = mapMutCh.upper_bound(high.getChannelId());
          if (iter != mapMutCh.end())
            {
              mutCh = (*iter).second;
              thisarm = mutCh.getArm();
              thissta = mutCh.getStation();
            }
          // too much output
          // 	  else
          // 	    {
          // 	     	      cout << PHWHERE << "ERROR: reached end of map" << endl;
          // 	    }
        } // while arm, sta is the same
      if (stripset.size() > 1) // enough strips to make a cluster
        {
          nclusters = strip2clust(arm, sta);
          purge();
          if (nclusters > 0)
            {
              for (unsigned int j = 0; j < clusters.size(); j++)
                {
                  clust = clusters[j];
                  if ( clust.nstrips <= max_strips_in_cluster)
                    {
                      mutH1ClustAdc[arm]->Fill(clust.adcsum);
                      mutH1PeakAdc[arm]->Fill(clust.peakadc);
                      mutH1PeakAdcSta[arm][sta]->Fill(clust.peakadc);
                      mutH1PeakAdcStaOct[arm][sta][clust.oct]->Fill(clust.peakadc);

                      hit_pl[arm][sta][clust.oct][clust.pla]++;
                    }
                }
            }
        }
    }

  int plindex;
  for (arm = 0; arm < NARMS; arm++)
    {
      plindex = 0;
      for (sta = 0; sta < NSTATIONS; sta++)
        {
          for (oct = 0; oct < NOCTANTS; oct++)
            {
              for (pla = 0; pla < NPLANES[sta]; pla++)
                {
                  mutHprofHits[arm]->Fill(plindex, hit_pl[arm][sta][oct][pla]);
                  plindex++;
                }
            }
        }
      // # of total fired strips in this event/mutr_total_ch
      mutH1ZSRatio[arm]->Fill(fired_strips[arm] / (float) NTOTSTRIPS[arm]);
    }

//   if (NumEvtAna > 0 && NumEvtAna % 1000 == 0)
//     {
//       if (verbosity > 0)
//         {
//           printf("Number of events analyzed: %d, which is %f%% of the total!\n",NumEvtAna,float(NumEvtAna) / NumEvtIn*100);
//         }
//     }

  return 0;
}

int MutrMon::strip2clust(const int arm, const int sta, const int oct)
{
  clusters.clear();
  MutOnlineClust newclust;

  // start with the first strip in the set
  stripiter = stripset.begin();
  MutOnlineStrip prevstrip = (*stripiter);
  // cluster = present collection of strips, used locally in this routine
  onlineset_type cluster;
  cluster.clear();
  cluster.insert(prevstrip);
  float adcsum = prevstrip.adc;
  stripiter++;
  int peakstrip = prevstrip.strip;
  float peakadc = prevstrip.adc;
  // go thru the rest
  while (stripiter != stripset.end())
    {
      if ( (*stripiter).oct == prevstrip.oct &&
           (*stripiter).hoct == prevstrip.hoct &&
           (*stripiter).pla == prevstrip.pla &&
           (*stripiter).strip == (prevstrip.strip + 1) )
        { // ok, this is a neighbor to the previous one
          // add to the cluster
          cluster.insert(*stripiter);
          adcsum += (*stripiter).adc;
          if ( (*stripiter).adc > peakadc )
            {
              peakadc = (*stripiter).adc;
              peakstrip = (*stripiter).strip;
            }
        }
      else
        { // ok, we got a new one here
          // if the cluster has more than one strip it's accepted
          if (cluster.size() > 1)
            {
              newclust.oct = prevstrip.oct;
              newclust.hoct = prevstrip.hoct;
              newclust.pla = prevstrip.pla;
              newclust.laststrip = prevstrip.strip;
              newclust.nstrips = cluster.size();
              newclust.adcsum = adcsum;
              newclust.peakstrip = peakstrip;
              newclust.peakadc = peakadc;
              clusters.push_back(newclust);
            }
          // reset and start on another cluster
          cluster.clear();
          cluster.insert(*stripiter);
          adcsum = (*stripiter).adc;
          peakstrip = (*stripiter).strip;
          peakadc = (*stripiter).adc;
        }
      prevstrip = (*stripiter);
      stripiter++;
    }
  // we could have missed the last cluster: check
  if (cluster.size() > 1)
    {
      newclust.oct = prevstrip.oct;
      newclust.hoct = prevstrip.hoct;
      newclust.pla = prevstrip.pla;
      newclust.laststrip = prevstrip.strip;
      newclust.nstrips = cluster.size();
      newclust.adcsum = adcsum;
      newclust.peakstrip = peakstrip;
      newclust.peakadc = peakadc;
      clusters.push_back(newclust);
    }

  return clusters.size();
}

int MutrMon::purge()
{
  // Remove clusters if there are not matching hits in at least more
  // than half the gaps and planes; and of course in the right hoct
  MutOnlineClust clust;
  vector<MutOnlineClust> newclusters;
  newclusters.clear();

  const int PLANEMAX = 6;
  const int MINPLANES = 3;
  int nhits[2*PLANEMAX] = { 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0 };
  // nhits array index is hoct*NPLANEMAX(6) + pla; no hits yet
  int i;
  unsigned int j;

  for (j = 0; j < clusters.size(); j++)
    {
      clust = clusters[j];
      if ( clust.nstrips <= max_strips_in_cluster)
        { // add up the hits in this hoct and plane
          nhits[clust.hoct*PLANEMAX + clust.pla]++;
        }
    }

  // make a hoct scan over the planes
  int hoctsum[2] = { 0, 0};
  for (i = 0; i < 2; i++)
    {
      for (int ii = 0; ii < PLANEMAX; ii++)
        {
          if (nhits[i*PLANEMAX + ii] > 0) // == 1 is probably too strict
            {
              hoctsum[i]++;
            }
        }

      if (hoctsum[i] >= MINPLANES)
        { // there is something here that might be worth keeping
          for (j = 0; j < clusters.size(); j++)
            {
              clust = clusters[j];
              if (clust.hoct == i)
                { // add to the new set
                  newclusters.push_back(clust);
                }
            }
        }
    }
  // overwrite old set
  clusters = newclusters;
  return clusters.size();
}

bool
MutrMon::sample_is_good(const int samp[NSAMPLES], const float adcr[NSAMPLES])
{
  // Remove samples that look like out of time hits or noise
  //
  float adc13, adc23, adc43;
  // difference between samples; normalized by signal (ped - samp[2])
  adc13 = (samp[0] - samp[2]) / adcr[2];
  adc23 = (samp[1] - samp[2]) / adcr[2];
  adc43 = (samp[3] - samp[2]) / adcr[2];
  /*
  cout << " sample_is_good : adc13: " << adc13
       << " adc23: " << adc23
       << " adc43: " << adc43
       << endl;
  */ 
  // Following two cuts are for out of time
  if (abs(adc13 - Adc12Slope*adc23 - Adc12Offset) > Adc12Cut )
    {
      //    cout << " sample_is_good : failed first cut " << endl;
      return false;
    }
  if (abs(adc13 - Adc14Slope*adc43 - Adc14Offset) > Adc14Cut )
    {
      //    cout << " sample_is_good : failed second cut " << endl;
      return false;
    }

  // Noise rejection cut
  if (adc13 + Adc12Slope*adc23 + Adc12Offset < Adc23Max )
    {
      //    cout << " sample_is_good : failed third cut " << endl;
      return false;
    }

  //  cout << " sample_is_good : passed all cuts " << endl;
  return true;
}

