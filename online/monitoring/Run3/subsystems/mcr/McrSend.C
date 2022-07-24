#include <McrSend.h>
#include <OnlMonServer.h>

#include <TH1.h>

#include <msg_profile.h>

#include <it_cal/iostream.h>
IT_USING_NAMESPACE_STD
#include <ctime>
#include <omg/orb.hh>
#include <omg/CosNaming.hh>
#include <omg/PortableServer.hh>
#include <ConnectionMgr.h>
#include <Vertex.hh>
#include <rhicDefs.h>

#include <cstdlib> // for getenv()
#include <sstream>

CosNaming::NamingContext * pns;
CosNaming::NamingContext_var rootContext;
CORBA::ORB_var global_orb = CORBA::ORB::_nil();
static const string vertexType[2] =
  {"phenixVertex.BBCLL1", "phenixVertex.Z"
  };

using namespace std;

McrSend::McrSend(const OnlMon *parent)
{
  mymon = parent;
  //  global_orb = CORBA::ORB::_nil();
  int argc = 0;
  global_orb = CORBA::ORB_init(argc, NULL);
  cm = new ConnectionMgr(global_orb);
  cm->bindNs();
  const char *logname = getenv("LOGNAME");
  if (logname && !strcmp(logname, "phnxrc"))
    {
      sendToMcr = 1;
    }
//   else if (logname && !strcmp(logname, "pinkenbu"))
//     {
//       sendToMcr = 1;
//     }
  else
    {
      sendToMcr = 0;
    }
  return ;
}

McrSend::~McrSend()
{
  delete cm;
  if ( !CORBA::is_nil(global_orb) )
    {
      global_orb->shutdown(true);
      global_orb->destroy();
    }
  return ;
}

int
McrSend::SendHisto(TH1 *histo, const time_t histotime, const int runnumber, const int whichvtx)
{
  int status = 0;
  int index = 0;
  CORBA::Object_var objectref = CORBA::Object::_nil();
  Vertex_ptr vertexRef;
  CORBA::String_var vertexname = CORBA::string_alloc(strlen(vertexType[whichvtx].c_str()));
  strcpy(vertexname, vertexType[whichvtx].c_str());

  ostringstream msg;



  // get vertex object reference from the nameserver

  try
    {
      objectref = cm->getCORBAObjectRef(vertexname);
      if (CORBA::is_nil(objectref))
        {
          msg << vertexname << " not in nameserver " ;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_MONITOR, MSG_SEV_SEVEREERROR, msg.str(), 3);
          status = 1;
          return status;
        }
      vertexRef = Vertex::_narrow(objectref);
    }
  catch (CORBA::SystemException& e)
    {
      msg << "Unexpected CORBA exception: " << e ;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_MONITOR, MSG_SEV_SEVEREERROR, msg.str(), 4);
      status = 2;
      return status;
    }

  // initialize the test data

  vertexStruct vdata;

  vdata.startbin = histo->GetXaxis()->GetBinLowEdge(1);
  vdata.endbin = histo->GetXaxis()->GetBinUpEdge(histo->GetNbinsX());
  vdata.numbins = histo->GetNbinsX();
  vdata.mean = histo->GetMean();
  vdata.rms = histo->GetRMS();
  vdata.timestamp = histotime;
  vdata.runnumber = runnumber;
  // now initialize the data array
  vdata.vertexdata.length(histo->GetNbinsX());

  for (index = 0; index < histo->GetNbinsX() ; index++ )
    {
      vdata.vertexdata[index] = histo->GetBinContent(index + 1);
    }

  // now send the data

  if (sendToMcr)
    {
      try
        {
          ostringstream msg;
          msg << "Sending vertex " << vertexType[whichvtx] << " to MCR";
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_MONITOR, MSG_SEV_INFORMATIONAL, msg.str(), 2);
          vertexRef->setVertexData(vdata);
        }
      catch (CORBA::TRANSIENT &te)
        {
          msg << "Transient exception: restart rhic server " ;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_MONITOR, MSG_SEV_ERROR, msg.str(), 5);
          status = 3;
        }
      catch (CORBA::SystemException &sysEx )
        {
          msg << "System Exception : " << sysEx ;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_MONITOR, MSG_SEV_SEVEREERROR, msg.str(), 6);
          status = 4;
        }
    }
  delete vertexRef;

  return status;
}

