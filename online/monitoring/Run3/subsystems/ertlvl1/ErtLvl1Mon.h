#ifndef __ERTLVL1MON_H__
#define __ERTLVL1MON_H__

#include "OnlMon.h"
#include "ErtLvl1MonDefs.h"
#include <string>
#include <ctime>

class Event;
class ERTLl1;
class TH1;
class TH2;
class OnlMonDB;

class ErtLvl1Mon: public OnlMon
{
 public:
  ErtLvl1Mon();
  virtual ~ErtLvl1Mon();

  int process_event(Event *evt);
  int Reset();
  int BeginRun(const int runno);

  bool checkEventTrigger(Event *event);

 protected:
  unsigned int evtcnt;
  time_t LastCommit;
  OnlMonDB *dbvars;
  int DBVarInit();

  int ErtLL1Packet[ARMS][10];
  int *MuidRocs;
  void CreateHistograms();
  void RegisterHistograms();

  int DATAERR[ARMS*FIBERS];
  int FFA[ARMS], FFB[ARMS], FFC[ARMS], TBT[ARMS], ERON[ARMS], TERON[ARMS];
  int FFASim[3][ARMS], FFBSim[3][ARMS], FFCSim[3][ARMS];
  int TBTSim[3][ARMS], ERONSim[3][ARMS], TERONSim[3][ARMS];

  long int rawevtcnt;
  bool IsBlueLogic[6];

  int EvtCounter[ARMS];
  int MasterCounter[4][ARMS][6]; //6 = trigger index

  ERTLl1 *simERTLl1;
  TH1 *ErtLvl1_FFA[ARMS];
  TH1 *ErtLvl1_FFB[ARMS];
  TH1 *ErtLvl1_FFC[ARMS];
  TH1 *ErtLvl1_TBT[ARMS];
  TH1 *ErtLvl1_ERON[ARMS];
  TH1 *ErtLvl1_TERON[ARMS];
  TH1 *ErtLvl1_FFASim[ARMS];
  TH1 *ErtLvl1_FFBSim[ARMS];
  TH1 *ErtLvl1_FFCSim[ARMS];
  TH1 *ErtLvl1_TBTSim[ARMS];
  TH1 *ErtLvl1_ERONSim[ARMS];
  TH1 *ErtLvl1_TERONSim[ARMS];

  TH1 *hRocEff[ARMS];
  TH1 *hRocEntries[ARMS];
  TH2 *hRocWord[ARMS];
  TH2 *hRocWordERTLL1[ARMS];
};

#endif /* __ERTLVL1MON_H__ */
