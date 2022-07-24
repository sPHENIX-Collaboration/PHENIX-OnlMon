#ifndef __PADMON_H__
#define __PADMON_H__

#include "PadMonPar.h"
#include <OnlMon.h>

class Event;
class PadCalibrationObject;
class PadOnlineRec;
class PHCompositeNode;
class TH1;
class TH2;
class TProfile;



class PadMon: public OnlMon
{
 public:
  PadMon();
  virtual ~PadMon();

  int process_event(Event *evt);
  int Init(); 
  int SetUp(Event *evt);
  int Reset();

 protected:

  int send_message(const int severity, const std::string &err_message);
  
  PadOnlineRec *pcFemThisEvent[NPLANES][NFEMSPERPLANE];
  PadCalibrationObject *PCOnew;

  // Event counters
  int firstEvent,lastEvent;

  // *** needed for event address check
  int packeterrornr[PC_NINSTALLED*NFEMSPERPLANE];
  int packetanalyzednr[PC_NINSTALLED*NFEMSPERPLANE];
  int packetActive[200];

  TH1 *padStat;
  TProfile *padProfPadSum[PC_NINSTALLED]; // 1 FEM per bin

  TProfile *padProfClustActivity[PC_NINSTALLED]; // 1 HV sector per bin
  TH1 *padMultiplicity[PC_NINSTALLED];
  TProfile *padProfClustSize[PC_NINSTALLED]; // 1 HV sector per bin
  TH2 *padPacketidhist[PC_NINSTALLED]; 
  
  // summary for PadMonDraw::Draw()
  TProfile *padClustActivity;
  TH1 *padEventError;
  TH1 *padMaxRocs;

  // Detailed histos - displayed when something is wrong
  // 1 per FEM

  TH2 *padFemPadxPadz[PC_NINSTALLED*NFEMSPERPLANE];
  TH2 *padFemCellxCellz[PC_NINSTALLED*NFEMSPERPLANE];
  TH1 *padClustSize[PC_NINSTALLED*NHVSECTORS];

};

#endif /* __PADMON_H__ */

