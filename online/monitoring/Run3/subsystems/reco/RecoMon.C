#include "RecoMon.h"

#include <OnlMonServer.h>

#include <SubsysReco.h>

#include <Fun4AllReturnCodes.h>
#include <Event.h>
#include <EventTypes.h>
#include <RunToTime.hh>
#include <PHCompositeNode.h>
#include <PHNodeIterator.h>
#include <PHNodeReset.h>
#include <PHDataNode.h>
#include <PHTimeStamp.h>
#include <recoConsts.h>

using namespace std;


RecoMon::RecoMon(): OnlMon("RECO")
{
  eventnumber = 0;
  recorunnumber = 0;
  return ;
}

RecoMon::~RecoMon()
{
  while (Subsystems.begin() != Subsystems.end())
    {
      delete Subsystems.back();
      Subsystems.pop_back();
    }
  return;
}

int
RecoMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();
  topNode = se->TopNode();
  Init(topNode);
  return 0;
}

int
RecoMon::Init(PHCompositeNode *topNode)
{
  Event *evt = NULL;
  PHCompositeNode *dcmNode, *dstNode, *runNode, *parNode, *evaNode, *geaNode, *dchNode;
  dcmNode = new PHCompositeNode("DCM");
  topNode->addNode(dcmNode);
  dchNode = new PHCompositeNode("DCH");
  topNode->addNode(dchNode);
  dstNode = new PHCompositeNode("DST");
  topNode->addNode(dstNode);
  evaNode = new PHCompositeNode("EVA");
  topNode->addNode(evaNode);
  geaNode = new PHCompositeNode("GEA");
  topNode->addNode(geaNode);
  runNode = new PHCompositeNode("RUN");
  topNode->addNode(runNode);
  parNode = new PHCompositeNode("PAR");
  topNode->addNode(parNode);
  topNode->addNode(new PHDataNode<Event>(evt, "PRDF"));
  return 0;
}

int
RecoMon::BeginRun(const int runno)
{
  static int ifirst = 1;
  int iret = 0;
  if (ifirst)
    {
      ifirst = 0;
      recoConsts *rc = recoConsts::instance();
      rc->set_IntFlag("RUNNUMBER", recorunnumber);
      RunToTime* runTime = RunToTime::instance();
      PHTimeStamp *ts(runTime->getBeginTime(recorunnumber));
      PHTimeStamp BeginRunTimeStamp = *ts;
      rc->set_TimeStamp(BeginRunTimeStamp);
      delete ts;
      iret =   InitRun(topNode);
    }
  return iret;
}

int
RecoMon::InitRun(PHCompositeNode *topNode)
{
  int iret = 0;
  vector<SubsysReco *>::iterator sysiter;
  for (sysiter = Subsystems.begin(); sysiter != Subsystems.end(); sysiter++)
    {
      if (verbosity > 0)
        {
          cout << "InitRun for Subsystem " << (*sysiter)->Name() << endl;
        }
      iret += (*sysiter)->InitRun(topNode);
    }
  return iret;
}

int
RecoMon::process_event(Event *e)
{
  PHNodeIterator mainIter(topNode);
  PHDataNode<Event> *EvtNode = dynamic_cast<PHDataNode<Event>* > (mainIter.findFirst("PHDataNode", "PRDF"));
  EvtNode->setData(e);
  if (verbosity > 0)
    {
      mainIter.print();
    }
  vector<SubsysReco *>::iterator sysiter;
  eventnumber++;
  if (eventnumber % 1000 == 0)
    {
      cout << "Number of Evts " << eventnumber << endl;
    }

  switch (e->getEvtType())
    {
    case REJECTEDEVENT:
    case DATAEVENT:
      //   case DATA1EVENT: currently defined as 1 like DATAEVENT
    case DATA2EVENT:
    case DATA3EVENT:
    case BEGRUNEVENT:
    case SCALEREVENT:
      break;
    case ENDRUNEVENT:
    case RUNINFOEVENT:
    case SPILLONEVENT:
    case SPILLOFFEVENT:
      return 0;
      break;
    default:
      cout << "Unknown Eventtype: " << e->getEvtType() << endl;
      return 0;
    }
  int iret = 0;
  int iretsum = 0;
  for (sysiter = Subsystems.begin(); sysiter != Subsystems.end(); sysiter++)
    {
      if (verbosity > 0)
        {
          cout << "Process Subsystem " << (*sysiter)->Name() << endl;
        }
      if ( (iret = (*sysiter)->process_event(topNode)))
        {
          switch (iret)
            {
            case ABORTEVENT:
              if (verbosity > 0)
                {
                  cout << PHWHERE << "Aborting Event" << endl;
                }
              break;
            default:
              cout << PHWHERE << "Unknown return code from process event: "
                   << iret
                   << " from SubsysReco " << (*sysiter)->Name() << endl;
            }
	  goto cleanup;
        }
      iretsum += iret;
    }
 cleanup:
  return iretsum;
}

int
RecoMon::ResetEvent()
{
  int iret = 0;
  ResetNodeTreeEvent(topNode);
  PHNodeIterator mainIter(topNode);
  PHDataNode<Event> *EvtNode = dynamic_cast<PHDataNode<Event>* > (mainIter.findFirst("PHDataNode", "PRDF"));
  EvtNode->setData(0);
  return iret;
}

int
RecoMon::EndRun(const int runno)
{
  int iret = 0;
  return iret;
}


int RecoMon::registerSubsystem(SubsysReco *subsystem)
{
  int iret = subsystem->Init(topNode);
  if (iret)
    {
      cout << PHWHERE << " Error initializing subsystem "
           << subsystem->Name() << ", return code: " << iret << endl;
      return iret;
    }
  if (verbosity > 0)
    {
      cout << "Registering Subsystem " << subsystem->Name() << endl;
    }
  Subsystems.push_back(subsystem);
  return 0;
}

void RecoMon::identify(ostream& out) const
{
  out << "RecoMon Object" << endl;
  out << "Currently registered Subsystem Recos:" << endl;
  vector<SubsysReco *>::const_iterator recoiter;
  for (recoiter = Subsystems.begin(); recoiter != Subsystems.end(); recoiter++)
    {
      (*recoiter)->Print("ALL");
    }
  topNode->print();
  return ;
}

int
RecoMon::Reset()
{
  return 0;
}

int RecoMon::ResetNodeTreeEvent(PHCompositeNode *topNode)
{
  const char *ResetNodeList[] = {"DCM", "DST", "EVA"};
  int iret = 0;
  vector<SubsysReco *>::iterator sysiter;
  for (sysiter = Subsystems.begin(); sysiter != Subsystems.end(); sysiter++)
    {
      if (verbosity > 0)
        {
          cout << "Resetting Subsystem " << (*sysiter)->Name() << endl;
        }
      iret += (*sysiter)->ResetEvent(topNode);
    }
  PHNodeIterator mainIter(topNode);
  PHNodeReset reset;
  for (int i = 0; i < 3; i++)
    {
      if (mainIter.cd(ResetNodeList[i]))
        {
          mainIter.forEach(reset);
          mainIter.cd();
        }
    }
  return iret;
}
