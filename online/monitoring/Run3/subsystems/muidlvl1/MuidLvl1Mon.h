#ifndef __MUIDLVL1MON_H__
#define __MUIDLVL1MON_H__

#include "OnlMon.h"
#include "MuidLvl1MonDefs.h"
#include <string>
#include <ctime>

class Event;
class MuIDLl1;
class TH1;
class TH2;
class OnlMonDB; 
class TFile; 
class TNtuple; 

class MuidLvl1Mon: public OnlMon
{ 
 public:
  MuidLvl1Mon();
  virtual ~MuidLvl1Mon();

  int process_event(Event *evt);
  int Reset();
  int BeginRun(const int runno);
  int EndRun(const int runno);
	void SetOfflineVersion(int value){ offline_version = value; }
  
 protected:
  int DBVarInit();
  OnlMonDB *dbvars;

  unsigned int evtcnt;
  time_t LastCommit; 
	int offline_version;

  int SYNCERR[ARMS*BOARDS*FIBERS];
  int HorRoads[ARMS*HORROADS], VerRoads[ARMS*VERROADS];
  int HorRoadsSim[ARMS*HORROADS], VerRoadsSim[ARMS*VERROADS];
  int HorRoadsShal[ARMS*HORROADS], VerRoadsShal[ARMS*VERROADS];
  int HorRoadsShalSim[ARMS*HORROADS], VerRoadsShalSim[ARMS*VERROADS];
  int SHNumEvt, SVNumEvt;
  int NHNumEvt, NVNumEvt;

  MuIDLl1 *simMuIDLl1;

  short SimSyncErr[2][2][5][FIBERS];

  int OneDeepSouth, OneDeepSouthSim, OneDeepSouthGL1; 
  int TwoDeepSouth, TwoDeepSouthSim, OneDeepNorthGL1; 
  int OneDeepNorth, OneDeepNorthSim; 
  int TwoDeepNorth, TwoDeepNorthSim; 
  int OneDeepComb, OneDeepCombSim, OneDeepGL1; 
  int OneDeepCombS, OneDeepCombSimS; 
  int OneDeepCombN, OneDeepCombSimN; 
  int TwoDeepComb, TwoDeepCombSim, TwoDeepCombSimFull, TwoDeepGL1; 
  int TwoDeepCombS, TwoDeepCombSimS; 
  int TwoDeepCombN, TwoDeepCombSimN; 
  int TwoDeepSouthGL1, TwoDeepNorthGL1; 

  int OneShalSouth, OneShalSouthSim; 
  int OneDeepOneShalSouth, OneDeepOneShalSouthSim; 
  int OneShalNorth, OneShalNorthSim; 
  int OneDeepOneShalNorth, OneDeepOneShalNorthSim; 

  int rbitsN1D,rbitsS1D,rbits1D; 
  int rbitsN2D,rbitsS2D,rbits2D; 

  TH1 *MuidLvl1_DataErr[ARMS];
  TH1 *MuidLvl1_SyncErr[ARMS];
  TH1 *MuidLvl1_DataErr2[ARMS];
  TH1 *MuidLvl1_SyncErr2[ARMS];
  TH1 *MuidLvl1_HorRoadNum[ARMS];
  TH1 *MuidLvl1_HorRoadNumShal[ARMS];
  TH1 *MuidLvl1_VerRoadNum[ARMS];
  TH1 *MuidLvl1_VerRoadNumShal[ARMS];
  TH1 *MuidLvl1_HorRoadNumSim[ARMS];
  TH1 *MuidLvl1_VerRoadNumSim[ARMS];
  TH1 *MuidLvl1_HorRoadNumShalSim[ARMS];
  TH1 *MuidLvl1_VerRoadNumShalSim[ARMS];
  TH1 *MuidLvl1_HorRoadCount[ARMS];
  TH1 *MuidLvl1_HorRoadCountShal[ARMS];
  TH1 *MuidLvl1_VerRoadCount[ARMS];
  TH1 *MuidLvl1_VerRoadCountShal[ARMS];
  TH1 *MuidLvl1_HorRoadCountSim[ARMS];
  TH1 *MuidLvl1_VerRoadCountSim[ARMS];
  TH1 *MuidLvl1_HorRoadCountShalSim[ARMS];
  TH1 *MuidLvl1_VerRoadCountShalSim[ARMS];
  TH1 *MuidLvl1_HorRoadCountDif[ARMS];
  TH1 *MuidLvl1_VerRoadCountDif[ARMS];
  TH2 *MuidLvl1_CompareVH[ARMS];
  TH2 *MuidLvl1_CompareSumsV[ARMS];
  TH2 *MuidLvl1_CompareSumsH[ARMS];
  TH1 *MuidLvl1_ESNMismatch[ARMS];
  TH1 *MuidLvl1_ESNCtlMis_V[ARMS];
  TH1 *MuidLvl1_ESNCtlMis_H[ARMS];
  TH1 *MuidLvl1_HorSymEff[ARMS];
  TH1 *MuidLvl1_VerSymEff[ARMS];
  TH1 *MuidLvl1_HorSymEffCut[ARMS];
  TH1 *MuidLvl1_VerSymEffCut[ARMS];
  TH1 *MuidLvl1_HorSymEffCutShal[ARMS];
  TH1 *MuidLvl1_VerSymEffCutShal[ARMS];
  TH1 *MuidLvl1_GL1Eff[ARMS];
	TH1 *MuidLvl1_InfoHist;

};

#endif /* __MUIDLVL1MON_H__ */
