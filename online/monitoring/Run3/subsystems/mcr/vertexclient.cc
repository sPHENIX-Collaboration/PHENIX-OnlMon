
// vertexclient.cc
// client application to test sending vertex data to the cdev server
// linux version uses connection manager to get vertex object reference

#include <stdlib.h>
#include <it_cal/iostream.h>
IT_USING_NAMESPACE_STD
#include <time.h>
#include <omg/orb.hh>
#include <omg/CosNaming.hh>
#include <omg/PortableServer.hh>
#include <ConnectionMgr.h>
#include "Vertex.hh"
#include "rhicDefs.h"


CosNaming::NamingContext * pns;
  CosNaming::NamingContext_var rootContext;


int 
main(int argc, char *argv[] ) {

  int status = 0;
  int index = 0;
  CORBA::Object_var objectref = CORBA::Object::_nil();
  Vertex_ptr vertexRef;
  ConnectionMgr * cm;
  CORBA::String_var vertexname = CORBA::string_alloc(strlen("mvdvertex"));
  strcpy(vertexname,"mvdvertex");

  
  CORBA::ORB_var 
  global_orb = CORBA::ORB::_nil();
  cout << "Initializing " << endl;
  cout << "argc: " << argc << ", argv: " << argv[0] << endl;
  global_orb = CORBA::ORB_init(argc, argv);
	

  // get vertex object reference from the nameserver

   try
    {
	cm = new ConnectionMgr(global_orb);
	cm->bindNs();
	objectref = cm->getCORBAObjectRef(vertexname);
	if (CORBA::is_nil(objectref))
	  {
	    cout << vertexname << " not in nameserver " << endl;
	    return 1;
	  }
	vertexRef = Vertex::_narrow(objectref);

   } catch (CORBA::SystemException& e)
    {
        cout << "Unexpected CORBA exception: " << e << endl;
	return 1;
    }

   // initialize the test data

   cout << "construct vertex data " << endl;
   vertexStruct vdata;
   time_t timenow;
   timenow = time(NULL);

   vdata.startbin = 1;
   vdata.endbin   = 10;
   vdata.numbins  = 10;
   vdata.mean     = 4.5;
   vdata.rms      = .5;
   vdata.timestamp = timenow;
   vdata.runnumber = 35000;
   // now initialize the data array
   vdata.vertexdata.length(10);

   for (index = 0; index < 10 ; index++ )
     {
       vdata.vertexdata[index] = index;
     }

   // now send the data

   cout << "send vertex data" << endl;
   try {
     vertexRef->setVertexData(vdata);

   } catch (CORBA::TRANSIENT &te)
     {
       cout << "Transient exception: restart rhic server " << endl;
     }
   catch (CORBA::SystemException &sysEx ) 
     {
       cout << "System Exception sending vertex  data to rhic server ";
       cout << sysEx << endl;

   }

	
return 0;
}
