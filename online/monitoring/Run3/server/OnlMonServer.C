#include "OnlMonServer.h"
#include "OnlMon.h"
#include "OnlMonStatusDB.h"
#include "OnlMonTrigger.h"

#include <MessageSystem.h>

#include <msg_profile.h>
#include <msg_control.h>
#include <Event.h>
#include <phool.h>

#include <PdbApplication.hh>
#include <RunToTime.hh>

#include <PHCompositeNode.h>
#include <recoConsts.h>


#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>

#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <zlib.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/utsname.h>
#include <vector>

using namespace std;

OnlMonServer *OnlMonServer::__instance = NULL;

// root TFile compression level
static const int compression_level = 3;

OnlMonServer *OnlMonServer::instance()
{
  if (__instance)
    {
      return __instance;
    }
  __instance = new OnlMonServer("OnlMonServer");
  return __instance;
}

OnlMonServer::OnlMonServer(const string &name): OnlMonBase(name)
{
  pthread_mutex_init(&mutex, NULL);
  serverthreadid = 0;
  runnumber = -1;
  eventnumber = 0;
  MsgSystem[ThisName] = new MessageSystem(ThisName);
  onltrig = new OnlMonTrigger();
  statusDB = new OnlMonStatusDB();
  RunStatusDB = new OnlMonStatusDB("onlmonrunstatus");
  InitAll();
  return ;
}

OnlMonServer::~OnlMonServer()
{
  pthread_mutex_lock(&mutex);
  if (int tret = pthread_cancel(serverthreadid))
    {
      cout << PHWHERE << "pthread cancel returned error: " << tret << endl;
    }
  delete serverrunning;
  pthread_mutex_destroy(&mutex);
  while (MonitorList.begin() != MonitorList.end())
    {
      delete MonitorList.back();
      MonitorList.pop_back();
    }
  delete topNode;
  delete onltrig;
  delete statusDB;
  delete RunStatusDB;

  while (Histo.begin() != Histo.end())
    {
      delete Histo.begin()->second;
      Histo.erase(Histo.begin());
    }

  while (MsgSystem.begin() != MsgSystem.end())
    {
      delete MsgSystem.begin()->second;
      MsgSystem.erase(MsgSystem.begin());
    }
  return ;
}

void OnlMonServer::InitAll()
{
  if (gROOT->FindObject("ClientRunning"))
    {
      ostringstream msg;
      msg << "Don't run Server and Client in same session, exiting" ;
      send_message(MSG_SEV_FATAL, msg.str(), 1);
      exit(1);
    }
  serverrunning = new TH1F("ServerRunning", "ServerRunning", 1, 0, 1);
  unsigned int inittrig = 0;
  for (int i = 0; i < 3; i++)
    {
      Trigger(inittrig, i);
    }
  currentticks = 0;
  borticks = 0;
  badevents = 0;
  activepacketsinit = 0;
  scaledtrigmask = 0xFFFFFFFF;
  scaledtrigmask_used = 0;
  standalone = 0;
  cosmicrun = 0;
  TriggerConfig = "UNKNOWN";
  RunType = "UNKNOWN";
  topNode = new PHCompositeNode("TOP");
  return ;
}

void
OnlMonServer::dumpHistos(const char *filename)
{
  map<const string, TH1 *>::const_iterator hiter;
  TFile *hfile = new TFile(filename, "RECREATE", "Created by Online Monitor", compression_level);
  for (hiter = Histo.begin(); hiter != Histo.end(); ++hiter)
    {
      hiter->second->Write();
    }
  hfile->Close();
  delete hfile;
  return;
}

void 
OnlMonServer::registerCommonHisto(TH1 *h1d)
{
  registerHisto("COMMON",h1d->GetName(), h1d, 0);
  CommonHistoSet.insert(h1d->GetName());
  return ;
}

void 
OnlMonServer::registerHisto(const OnlMon *monitor, TH1 *h1d)
{
  registerHisto(monitor->Name(), h1d->GetName(), h1d, 0);
  return ;
}

void 
OnlMonServer::registerHisto(const char *monitorname, const char *hname, TH1 *h1d, const int replace)
{
  MonitorHistoSet[monitorname].insert(hname);
  registerHisto(hname, h1d, replace);
  return ;
}

void 
OnlMonServer::registerHisto(const char *hname, TH1 *h1d, const int replace)
{
  const string tmpstr = hname;
  map<const string, TH1 *>::const_iterator histoiter = Histo.find(tmpstr);
  ostringstream msg;
  int histoexist;
  TH1 *delhis;
  if (histoiter != Histo.end())
    {
      delhis = histoiter->second;
      histoexist = 1;
    }
  else
    {
      delhis = 0;
      histoexist = 0;
    }
  if (histoexist && replace == 0)
    {
      msg << "Histogram " << hname << " already registered, I won't overwrite it" ;
      send_message(MSG_SEV_WARNING, msg.str(), 2);
      msg.str("");
      msg << "Use a different name and try again" << endl;
      send_message(MSG_SEV_WARNING, msg.str(), 2);
    }
  else
    {
      if (verbosity > 1)
        {
          if (strcmp(h1d->GetName(), tmpstr.c_str()))
            {
              msg.str("");
              msg << PHWHERE << "Histogram " << h1d->GetName()
		  << " at " << h1d << " renamed to " << tmpstr ;
              send_message(MSG_SEV_INFORMATIONAL, msg.str(), 3);
            }
        }
      Histo[tmpstr] = h1d;
      if (delhis)
        {
          delete delhis;
        }
      h1d->SetName(tmpstr.c_str());
    }
  return ;
}

OnlMon *
OnlMonServer::getMonitor(const char *name)
{
  BOOST_FOREACH(OnlMon * mon, MonitorList)
    {
      if (!strcmp(name, mon->Name()))
	{
	  return mon;
	}
    }
  cout << "Could not locate monitor" << name << endl;
  return NULL;
}

void
OnlMonServer::registerMonitor(OnlMon *Monitor)
{
  BOOST_FOREACH(OnlMon * mon, MonitorList)
    {
      if (!strcmp(Monitor->Name(), mon->Name()))
	{
          ostringstream msg;
          msg << "Monitor " << Monitor->Name() << " already registered, I won't overwrite it" ;
          send_message(MSG_SEV_SEVEREERROR, msg.str(), 4);
          msg.str("");
          msg << "Use a different name and try again" ;
          send_message(MSG_SEV_SEVEREERROR, msg.str(), 4);
          return ;
        }
    }
  MonitorList.push_back(Monitor);
  MsgSystem[Monitor->Name()] = new MessageSystem(Monitor->Name());
  Monitor->InitCommon(this);
  Monitor->Init();
  return ;
}

TH1 *OnlMonServer::getHisto(const unsigned int ihisto) const
{
  map<const string, TH1 *>::const_iterator histoiter = Histo.begin();
  unsigned int size = Histo.size();
  if (Verbosity() > 3)
    {
      ostringstream msg;

      msg << "Map contains " << size << " Elements" ;
      send_message(MSG_SEV_INFORMATIONAL, msg.str(), 5);
    }
  if (ihisto < size)
    {
      for (unsigned int i = 0; i < ihisto; i++)
        {
          ++histoiter;
        }
      return histoiter->second;
    }
  else
    {
      ostringstream msg;
      msg << "OnlMonServer::getHisto: ERROR Invalid histogram number: "
	  << ihisto << ", maximum number is " << size ;
      send_message(MSG_SEV_ERROR, msg.str(), 6);
    }
  return NULL;
}

const char *
OnlMonServer::getHistoName(const unsigned int ihisto) const
{
  map<const string, TH1 *>::const_iterator histoiter = Histo.begin();
  unsigned int size = Histo.size();
  if (verbosity > 3)
    {
      ostringstream msg;
      msg << "Map contains " << size << " Elements" ;
      send_message(MSG_SEV_INFORMATIONAL, msg.str(), 5);
    }
  if (ihisto < size)
    {
      for (unsigned int i = 0; i < ihisto; i++)
        {
          ++histoiter;
        }
      return histoiter->first.c_str();
    }
  else
    {
      ostringstream msg;
      msg << "OnlMonServer::getHisto: ERROR Invalid histogram number: "
	  << ihisto << ", maximum number is " << size ;
      send_message(MSG_SEV_ERROR, msg.str(), 6);

    }
  return NULL;
}

TH1 *
OnlMonServer::getHisto(const string &hname) const
{
  map<const string, TH1 *>::const_iterator histoiter = Histo.find(hname);
  if (histoiter != Histo.end())
    {
      return histoiter->second;
    }
  ostringstream msg;

  msg << "OnlMonServer::getHisto: ERROR Unknown Histogram " << hname
      << ", The following are implemented: " ;
  send_message(MSG_SEV_ERROR, msg.str(), 7);
  Print("HISTOS");
  return NULL;
}

int
OnlMonServer::process_event(Event *evt)
{
  int i = 0;
  vector<OnlMon *>::iterator iter;

  for (iter = MonitorList.begin(); iter != MonitorList.end(); ++iter)
    {
      i += (*iter)->process_event_common(evt);
    }
  for (iter = MonitorList.begin(); iter != MonitorList.end(); ++iter)
    {
      i += (*iter)->ResetEvent();
    }

  return i;
}

int 
OnlMonServer::Reset()
{
  int i = 0;
  vector<OnlMon *>::iterator iter;
  for (iter = MonitorList.begin(); iter != MonitorList.end(); ++iter)
    {
      i += (*iter)->Reset();
    }
  map<const string, TH1 *>::const_iterator hiter;
  for (hiter = Histo.begin(); hiter != Histo.end(); ++hiter)
    {
      hiter->second->Reset();
    }
  eventnumber = 0;
  map<string, MessageSystem *>::const_iterator miter;
  for (miter = MsgSystem.begin(); miter != MsgSystem.end(); ++miter)
    {
      miter->second->Reset();
    }
  return i;
}

int 
OnlMonServer::BeginRun(const int runno)
{
  int i = 0;
  if (onltrig)
    {
      onltrig->RunNumber(runno);
    }

  i = CacheRunDB(runno);
  if (i)
    {
      printf("bad return code from CacheRunDB(%d): %d\n",runno,i);
      i=0;
    }

  vector<OnlMon *>::iterator iter;
  activepacketsinit = 0;
  scaledtrigmask = 0xFFFFFFFF;
  scaledtrigmask_used = 0;
  for (iter = MonitorList.begin(); iter != MonitorList.end(); ++iter)
    {
      (*iter)->BeginRunCommon(runno,this);
      i += (*iter)->BeginRun(runno);
    }
  DisconnectDB();
  return i;
}

int OnlMonServer::EndRun(const int runno)
{
  int i = 0;
  vector<OnlMon *>::iterator iter;
  for (iter = MonitorList.begin(); iter != MonitorList.end(); ++iter)
    {
      i += (*iter)->EndRun(runno);
    }
  return i;
}

void
OnlMonServer::Print(const char *what) const
{
  if (!strcmp(what, "ALL") || !strcmp(what, "HISTOS"))
    {
      set<string> cached_hists;
      std::map<std::string, std::set<std::string> >::const_iterator mhistiter;
      printf("--------------------------------------\n\n");
      printf("List of Assigned histograms in OnlMonServer:\n");
      for (mhistiter =  MonitorHistoSet.begin(); mhistiter != MonitorHistoSet.end();  ++mhistiter)
	{
	  set<string> hists = mhistiter->second;
	  set<string>::const_iterator siter;
	  for (siter = hists.begin(); siter != hists.end(); ++siter)
	    {
	      printf("%s: %s\n",(mhistiter->first).c_str(),(*siter).c_str());
	      cached_hists.insert(*siter);
	    }
	}
      // loop over the map and print out the content (name and location in memory)
      printf("\n--------------------------------------\n\n");
      printf("List of unassigned Histograms in OnlMonServer:\n");
      map<const string, TH1 *>::const_iterator hiter;
      for (hiter = Histo.begin(); hiter != Histo.end(); ++hiter)
	{
	  if (cached_hists.find(hiter->first) == cached_hists.end())
	    {
	      printf("%s is at 0x%08x\n",(hiter->first).c_str(),(unsigned int)hiter->second);
	    }
	}
      printf("\n");
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "MONITOR"))
    {
      // loop over the map and print out the content (name and location in memory)
      printf("--------------------------------------\n\n");
      printf("List of Monitors with registered histos in OnlMonServer:\n");

      vector<OnlMon *>::const_iterator miter;
      std::map<std::string, std::set<std::string> >::const_iterator mhisiter;
      for (miter = MonitorList.begin(); miter != MonitorList.end(); ++miter)
        {
          printf("%s\n",(*miter)->Name());
	  mhisiter = MonitorHistoSet.find((*miter)->Name());
	  set<string>::const_iterator siter;
	  if (mhisiter != MonitorHistoSet.end())
	    {
	      set<string> hists = mhisiter->second;
	      for (siter = hists.begin(); siter != hists.end(); ++siter)
		{
		  printf("%s: %s\n",(*miter)->Name(),(*siter).c_str());
		}
	    }
        }
      printf("\n");
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "TRIGGER"))
    {
      if (onltrig)
        {
          onltrig->Print(what);
        }
    }
  if (!strcmp(what, "ALL") || !strcmp(what, "ACTIVE"))
    {
      printf("--------------------------------------\n\n");
      printf("List of active packets:\n");
      set<unsigned int>::const_iterator iter;
      for (iter = activepackets.begin(); iter != activepackets.end(); ++iter)
	{
	  printf("%d\n",*iter);
	}
    }
  return ;
}

int
OnlMonServer::Trigger(const string &trigname, const unsigned short int i)
{
  unsigned int ibit = getLevel1Bit(trigname);

  if (trigger[i] & ibit)
    {
      return 1;
    }
  return 0;

}

void
OnlMonServer::RunNumber(const int irun)
{
 runnumber = irun;
 recoConsts *rc = recoConsts::instance();
 rc->set_IntFlag("RUNNUMBER",irun);
 return;
}



int
OnlMonServer::WriteHistoFile()
{
  utsname ThisNode;
  uname(&ThisNode);
  string nn = ThisNode.nodename;
  string mm = nn.substr(0, nn.find(".")); // strip the domain (all chars after first .)
  ostringstream dirname, filename;
  // filename is Run_<runno>_<monitor>_<nodename>.root
  if (getenv("ONLMON_SAVEDIR"))
    {
      dirname << getenv("ONLMON_SAVEDIR") << "/";
    }
  int irun = RunNumber();
  std::map<std::string, std::set<std::string> >::const_iterator mhistiter;
  // assignedhists set stores all encountered histograms so histos which are not accounted
  // for can be saved
  set<string> assignedhists = CommonHistoSet;
  set<string>::const_iterator siter;
  map<const string, TH1 *>::const_iterator hiter;
  for (mhistiter =  MonitorHistoSet.begin(); mhistiter != MonitorHistoSet.end();  ++mhistiter)
    {
      if (mhistiter->first == "COMMON")
        {
          continue;
        }
      filename.str("");
      filename << dirname.str() << "Run_" << irun << "_" << mhistiter->first << "_" << mm << "_"  << PortNumber() << ".root";
      TFile *hfile = new TFile(filename.str().c_str(), "RECREATE", "Created by Online Monitor", compression_level);
      for (siter = CommonHistoSet.begin(); siter != CommonHistoSet.end(); ++siter)
        {
          hiter = Histo.find(*siter);
          if (hiter != Histo.end())
            {
              hiter->second->Write();
            }
          else
            {
              cout << "could not locate histogram " << *siter << endl;
            }
        }
      for (siter = (mhistiter->second).begin(); siter != (mhistiter->second).end(); ++siter)
        {
          hiter = Histo.find(*siter);
          assignedhists.insert(*siter);
          if (hiter != Histo.end())
            {
              hiter->second->Write();
            }
          else
            {
              cout << "could not locate histogram " << *siter << endl;
            }
        }
      hfile->Close();
      delete hfile;
    }
  TFile *hfile = 0;
  filename.str("");
  filename << dirname.str() << "Run_" << irun << "_" << mm << "_"
	   << PortNumber() << ".root";
  for (hiter = Histo.begin(); hiter != Histo.end(); ++hiter)
    {
      if (assignedhists.find(hiter->first) == assignedhists.end())
        {
          if (!hfile)
            {
              hfile = new TFile(filename.str().c_str(), "RECREATE", "Created by Online Monitor", compression_level);
            }
          hiter->second->Write();
        }
    }
  if (hfile)
    {
      hfile->Close();
      delete hfile;
    }
  return 0;
}

int
OnlMonServer::send_message(const OnlMon *Monitor, const int msgsource, const int severity, const string &err_message, const int msgtype) const
{
  int iret = -1;
  map<string, MessageSystem *>::const_iterator iter = MsgSystem.find(Monitor->Name());
  ostringstream msg("Run ");
  int irun = RunNumber();
  msg << "Run " << irun << " Event# " << eventnumber << ": " << err_message;
  if (iter != MsgSystem.end())
    {

      iret = iter->second->send_message(msgsource, severity, msg.str(), msgtype);
    }
  else
    {
      // in case there are messages from the ctor before the subsystem is registered
      // print them out from the server
      iter = MsgSystem.find(ThisName);
      iret = iter->second->send_message(msgsource, severity, msg.str(), 0);
    }
  if (severity > MSG_SEV_WARNING)
    {
      WriteLogFile(Monitor->Name(), err_message);
    }
  return iret;
}

int
OnlMonServer::send_message(const int severity, const string &err_message, const int msgtype) const
{

  map<string, MessageSystem *>::const_iterator iter = MsgSystem.find(ThisName);
  int iret = iter->second->send_message(MSG_SOURCE_UNSPECIFIED, severity, err_message, msgtype);
  return iret;
}

OnlMonTrigger *
OnlMonServer::OnlTrig()
{
  if (!onltrig)
    {
      onltrig = new OnlMonTrigger();
    }
  return onltrig;
}

void
OnlMonServer::TrigMask(const string &trigname, const unsigned int bitmask)
{
  onltrig->TrigMask(trigname, bitmask);
  return ;
}

unsigned int
OnlMonServer::getLevel1Bit(const string &name)
{
  return onltrig->getLevel1Bit(name);
}

unsigned int
OnlMonServer::AddToTriggerMask(const string &name)
{
  unsigned int mask = getLevel1Bit(name);
  unsigned int newmask = AddScaledTrigMask(mask);
  return newmask;
}

unsigned int
OnlMonServer::AddScaledTrigMask(const unsigned int mask)
{
  if (mask)
    {
      if (!scaledtrigmask_used)
        {
          scaledtrigmask = 0;
          scaledtrigmask_used = 1;
        }
      scaledtrigmask |= mask;
    }
  return scaledtrigmask;
}


int
OnlMonServer::WriteLogFile(const string &name, const string &message) const
{

  int irun = RunNumber();
  // if no run is opened yet, I don't want to produce a -1 file which will
  // increase in size indefinitely
  if (irun <= 0)
    {
      return 0;
    }
  // no logifles for junk and calibration runs
  if (RunType == "JUNK" || RunType == "CALIBRATION")
    {
      return 0;
    }
  ostringstream logfilename, msg;
  const char *logdir = getenv("ONLMON_LOGDIR");
  if (logdir)
    {
      logfilename << logdir << "/";
    }
  logfilename << name << "_" << irun << ".log.gz" ;
  gzFile fout = gzopen(logfilename.str().c_str(), "a9");
  time_t curticks = CurrentTicks();
  string timestr = ctime(&curticks);
  string::size_type backslpos;
  // get rid of this damn end line of ctime
  if ((backslpos = timestr.find('\n')) != string::npos)
    {
      timestr.erase(backslpos);
    }
  msg << timestr
      << ", EventNo " << eventnumber
      << ": " << message;
  gzprintf(fout, "%s\n", msg.str().c_str());
  gzclose(fout);
  return 0;
}

int
OnlMonServer::parse_granuleDef(set<string> &pcffilelist)
{
  ostringstream filenam;
  if (getenv("ONLINE_CONFIGURATION"))
    {
      filenam << getenv("ONLINE_CONFIGURATION") << "/rc/hw/";
    }
  filenam << "granuleDef.pcf";
  string FullLine;	// a complete line in the config file
  ifstream infile;
  infile.open(filenam.str().c_str(), ifstream::in);
  if (!infile)
    {
      if (filenam.str().find("gl1test.pcf") == string::npos)
	{
	  ostringstream msg;
	  msg << "Could not open " << filenam.str();
	  send_message(MSG_SEV_ERROR, msg.str(), 7);
	}
      return -1;
    }
  getline(infile, FullLine);
  boost::char_separator<char> sep("/");
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  while ( !infile.eof())
    {
      if (FullLine.find("label:") != string::npos)
        {
	  tokenizer tokens(FullLine,sep);
	  tokenizer::iterator tok_iter = tokens.begin();
	  ++tok_iter;
	  if (verbosity > 1)
	    {
	      cout << "pcf file: " << *tok_iter << endl;
	    }
	  pcffilelist.insert(*tok_iter);
	}
      getline( infile, FullLine );
    }
  infile.close();
  return 0;
}

int
OnlMonServer::LoadLL1Packets()
{
  ostringstream filenam;
  if (getenv("ONLINE_CONFIGURATION"))
    {
      filenam << getenv("ONLINE_CONFIGURATION") << "/rc/hw/";
    }
  filenam << "Level1DD.pcf";
  string FullLine;	// a complete line in the config file
  ifstream infile;
  infile.open(filenam.str().c_str(), ifstream::in);
  if (!infile)
    {
      ostringstream msg;
      msg << "Could not open " << filenam.str();
      send_message(MSG_SEV_ERROR, msg.str(), 7);
      return -1;

    }
  getline(infile, FullLine);
  string::size_type pos1;
  while ( !infile.eof())
    {
      if (FullLine.find("LEVEL1_HITFORMAT") != string::npos)
        {
          if ((pos1 = FullLine.find("autogenerate_default0")) != string::npos)
            {
              FullLine.erase(0, pos1); // erase all before autogenerate_default0 string
              if ((pos1 = FullLine.find(",")) != string::npos)
                {
                  FullLine.erase(0, pos1 + 1); // erase all before and including , string
                  // erase the next 20 fields separated by , (hitformat and size)
                  int n = 0;
                  while (n++ <= 20)
                    {
                      if ((pos1 = FullLine.find(",")) != string::npos)
                        {
                          FullLine.erase(0, pos1 + 1); // erase all before and including , string
                        }
                    }
                  // whats left is the comma separated list of ll1 packets
                  while (FullLine.size() > 0)
                    {
                      pos1 = FullLine.find(",");
                      string packetidstr;
                      if (pos1 != string::npos)
                        {
                          packetidstr = FullLine.substr(0, pos1);
                          FullLine.erase(0, pos1 + 1);
                        }
                      else
                        {
                          packetidstr = FullLine;
                          FullLine.erase();
                        }
                      istringstream line;
                      line.str(packetidstr);
                      unsigned int packetid;
                      line >> packetid;
                      if (packetid > 0)
                        {
                          activepackets.insert(packetid);
                        }
                    }
                }
            }
        }
      getline( infile, FullLine );
    }

  infile.close();
  return 0;
}

int
OnlMonServer::LoadActivePackets()
{
  clearactivepackets();
  // LL1 packets are special
  LoadLL1Packets();
  set<string> pcffiles;
  parse_granuleDef(pcffiles);
  set<string>::const_iterator piter;
  for (piter = pcffiles.begin(); piter != pcffiles.end(); ++piter)
    {
      parse_pcffile(*piter);
    }
  return 0;
}

void
OnlMonServer::parse_pcffile(const string &lfn)
{
  ostringstream filenam;
  if (getenv("ONLINE_CONFIGURATION"))
    {
      filenam << getenv("ONLINE_CONFIGURATION") << "/rc/hw/";
    }
  filenam << lfn;

  set<unsigned int> disabled_packets;
  string FullLine;	// a complete line in the config file
  ifstream infile;
  infile.open(filenam.str().c_str(), ifstream::in);
  if (!infile)
    {
      if (filenam.str().find("gl1test.pcf") == string::npos)
	{
	  ostringstream msg;
	  msg << "LoadActivePackets: Could not open " << filenam.str();
	  send_message(MSG_SEV_WARNING, msg.str(), 7);
	}
      return;
    }
  getline(infile, FullLine);
  int checkreadoutflag = 0;
  string::size_type pos1;
  string::size_type pos2;
  vector<unsigned int> pktids;
  while ( !infile.eof())
    {
      // find the line with packet ids
      if ((pos1 = FullLine.find("packetid")) != string::npos)
	{
	  FullLine.erase(0, pos1); // erase all before packetid string
	  while ((pos1 = FullLine.find(":")) != string::npos)
	    {
	      pos2 = FullLine.find(",");
	      // search the int between the ":" and the ","
	      string packetidstr = FullLine.substr(pos1 + 1, pos2 - (pos1 + 1));
	      istringstream line;
	      line.str(packetidstr);
	      unsigned int packetid;
	      line >> packetid;
	      if (packetid > 0)
		{
		  pktids.push_back(packetid);
		}
	      // erase this entry from the line
	      FullLine.erase(0, pos2 + 1);
	    }
	  checkreadoutflag = 1;
	}
      if (checkreadoutflag)
	{
	  if (FullLine.find("readout") != string::npos)
	    {
	      if (FullLine.find("readout1") != string::npos)
		{
		  if (FullLine.find("YES") != string::npos)
		    {
		      activepackets.insert(pktids[0]);
		    }
		  else
		    {
		      disabled_packets.insert(pktids[0]);
		    }
		}
	      else if (FullLine.find("readout2") != string::npos)
		{
		  if (FullLine.find("YES") != string::npos)
		    {
		      activepackets.insert(pktids[1]);
		    }
		  else
		    {
		      disabled_packets.insert(pktids[1]);
		    }
		  checkreadoutflag = 0;
		  pktids.clear();
		}
	      else
		{
		  // only add packets if the readout is set to yes
		  if (FullLine.find("YES") != string::npos)
		    {
		      vector<unsigned int>::const_iterator viter;
		      for (viter = pktids.begin(); viter != pktids.end(); ++viter)
			{
			  activepackets.insert(*viter);
			}
		    }
		  else
		    {
		      vector<unsigned int>::const_iterator viter;
		      for (viter = pktids.begin(); viter != pktids.end(); ++viter)
			{
			  disabled_packets.insert(*viter);
			}
		    }
		  checkreadoutflag = 0;
		  pktids.clear();
		}
	    }
	}
      getline( infile, FullLine );
    }
  infile.close();
  if (verbosity > 1)
    {
      set<unsigned int>::const_iterator iter;
      cout << "active packets: " << endl;
      for (iter = activepackets.begin(); iter != activepackets.end(); ++iter)
	{
	  cout << *iter << endl;
	}
      cout << "inactive packets: " << endl;
      for (iter = disabled_packets.begin(); iter != disabled_packets.end(); ++iter)
	{
	  cout << *iter << endl;
	}
    }
}

int
OnlMonServer::IsPacketActive(const unsigned int ipkt)
{
  if (!activepacketsinit)
    {
      LoadActivePackets();
      activepacketsinit = 1;
    }
  set<unsigned int>::const_iterator iter = activepackets.find(ipkt);
  if (iter != activepackets.end())
    {
      return 1;
    }
  if (activepackets.empty()) // if list is empty, something is wrong, claim packet as active
    {
      ostringstream msg;
      msg << "List of Active Packets empty";
      send_message(MSG_SEV_ERROR, msg.str(), 8);
      return 1;
    }
  return 0;
}

int
OnlMonServer::CacheRunDB(const int runno)
{
  RunType = "UNKNOWN";
  TriggerConfig = "UNKNOWN";
  standalone = 0;
  cosmicrun = 0;
  borticks = 0;

  if (runno == 0xFEE2DCB) // dcm2 standalone runs have this runnumber
    {
      TriggerConfig = "StandAloneMode";
      standalone = 1;
      return 0;
    }
  odbc::Connection *con = 0;
  odbc::Statement* query = 0;
  ostringstream cmd;
  int iret = 0;
  try
    {
      con = odbc::DriverManager::getConnection("daq", "phnxrc", "");
    }
  catch (odbc::SQLException& e)
    {
      printf(" Exception caught during DriverManager::getConnection, Message: %s\n",e.getMessage().c_str());
      return -1;
    }

  query = con->createStatement();
  cmd << "select runnumber from run where runnumber = " << runno;
  odbc::ResultSet *rs = 0;
  int ncount = 10;
  while (ncount > 0)
    {
      try
	{
	  rs = query->executeQuery(cmd.str());
	}
      catch (odbc::SQLException& e)
	{
	  printf("Exception caught for query %s\nMessage: %s",cmd.str().c_str(), e.getMessage().c_str());
	}
      if (!rs->next())
	{
	  printf("run table query did not give any result, run %d not in DB yet countdown %d\n",runno,ncount);
	  ncount--;
	  sleep(10);
	  delete rs;
	}
      else
      {
        delete rs;
        break;
      }
  }
  cmd.str("");
  cmd << "SELECT runtype,triggerconfig,brunixtime FROM RUN  WHERE RUNNUMBER = "
      << runno;
  if (verbosity > 0)
    {
      printf("command: %s\n",cmd.str().c_str());
    }
      try
	{
	  rs = query->executeQuery(cmd.str());
	}
      catch (odbc::SQLException& e)
	{
	  printf("Exception caught for query %s\nMessage: %s",cmd.str().c_str(), e.getMessage().c_str());
	}
  if (rs->next())
    {
      RunType = rs->getString("runtype");
      TriggerConfig = rs->getString("triggerconfig");
      borticks = rs->getInt("brunixtime");
      if (TriggerConfig == "StandAloneMode")
	{
	  standalone = 1;
	}
      else
	{
	  standalone = 0;
	}
      if (TriggerConfig.find("Cosmic") != string::npos)
	{
	  cosmicrun = 1;
	}
      else
	{
	  cosmicrun = 0;
	}
    }
  else
    {
      iret = -1;
    }
  delete con;
  //  printf("CacheRunDB: runno: %d, RunType %s\n",runno,RunType.c_str());
  return iret;
}

int
OnlMonServer::SetSubsystemStatus(OnlMon *Monitor, const int status)
{
  if (GetRunType() != "JUNK")
    {
      statusDB->UpdateStatus(Monitor->Name(), runnumber, status);
    }
  return 0;
}

int
OnlMonServer::SetSubsystemRunStatus(OnlMon *Monitor, const int status)
{
  if (GetRunType() == "PHYSICS")
    {
      RunStatusDB->UpdateStatus(Monitor->Name(), runnumber, status);
    }
  return 0;
}

int
OnlMonServer::LookAtMe(OnlMon *Monitor, const int level, const std::string &message)
{
  cout << "got a LookAtMe from " << Monitor->Name()
       << ", level: " << level
       << ", message: " << message
       << endl;
  return 0;
}


int
OnlMonServer::DisconnectDB()
{
  PdbApplication *app = PdbApplication::instance();
  app->DisconnectDB();
  RunToTime *rt = RunToTime::instance();
  rt->DisconnectDB();
  return 0;
}
