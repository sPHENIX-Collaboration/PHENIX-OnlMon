#ifndef __RECOMON_H__
#define __RECOMON_H__

#include <string>
#include <vector>

#include <OnlMon.h>
#include <SubsysReco.h>

class Event;
class PHCompositeNode;

class RecoMon: public OnlMon
{
 public:
  RecoMon();
  virtual ~RecoMon();
  int Init();
  int Init(PHCompositeNode *topNode);
  int BeginRun(const int runno);
  int EndRun(const int runno);
  int InitRun(PHCompositeNode *topNode);
  int process_event(Event *e);
  int Reset();
  int ResetEvent();
  int ResetNodeTreeEvent(PHCompositeNode *topNode);

  int registerSubsystem(SubsysReco *subsystem);
  void identify(std::ostream& = std::cout) const;
  void set_recorunnumber(const int runno) {recorunnumber=runno;}

  PHCompositeNode *TopNode() {return topNode;}

 protected:

  int eventnumber;
  int recorunnumber;
  std::vector<SubsysReco *> Subsystems;
  PHCompositeNode *topNode;

};

#endif /* __RECOMON_H__ */
