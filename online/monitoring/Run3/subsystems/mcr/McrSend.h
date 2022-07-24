#ifndef __MCRSEND_H__
#define __MCRSEND_H__

#include <ctime>
#include <string>

class ConnectionMgr;
class OnlMon;
class TH1;

class McrSend
{
 public:
  McrSend(const OnlMon *parent);
  virtual ~McrSend();
  int SendHisto(TH1 *histo, const time_t histotime, const int runnumber, const int vtxtype);

 protected:
  ConnectionMgr *cm;
  int sendToMcr;
  const OnlMon *mymon;
 
};
#endif
