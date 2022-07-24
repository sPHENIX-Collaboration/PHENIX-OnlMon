#include <cstdio>
#include "corba_msg_buffer.h"
#include "Messages.h"

CORBA::ORB_var orb;
PortableServer::POA_var root_poa;

corba_msg_buffer::corba_msg_buffer(const char *channelname)
{
  int iarg = 0;
  orb = CORBA::ORB_init(iarg,NULL);
  CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
  root_poa = PortableServer::POA::_narrow(obj);
  msg = new Messages(orb,root_poa,const_cast <char *>(channelname),256);
  return;
}

corba_msg_buffer::~corba_msg_buffer()
{
  delete msg;
  return;
}
