#ifndef __CORBA_MSG_BUFFER_H
#define __CORBA_MSG_BUFFER_H

class msg_buffer;

class corba_msg_buffer
{
 public:
  corba_msg_buffer(const char *channelname = "demo_event_channel_0");

  virtual ~corba_msg_buffer();

  protected:

  msg_buffer *msg;
};


#endif 
