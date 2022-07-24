//////////////////////////////////////////////////////////////////
// Origin  :  Jaehyeon Do (doddeng@gmail.com)  Mar/31/2014      //
// Update  :  Carlos Perez (March 2016)                         //
// Update  :  Carlos Perez, John Lajoie (Apr 2016)              //
// MPC-EX Online Monitoring code                                // 
//////////////////////////////////////////////////////////////////

#ifndef __MPCEXMON_H__
#define __MPCEXMON_H__

#include <OnlMon.h>
#include <TString.h>

class Event;
class Packet;
class TH1F;
class TH2F;
class TProfile;
class mxCalibMaster;

class MpcExMon: public OnlMon {
 public:
  MpcExMon(const char *name = "MPCEXMON");
  virtual ~MpcExMon();
  int process_event(Event*);
  int Init();
  int BeginRun(const int runno);
  int EndRun(const int runno);
  int Reset();

 protected:
  TH1F *fMXHEvents; //!
  // RAW PLOTS
  TH1F *fMXHBeamClockSN; //!
  TH2F *fMXHLengthS; //!
  TH2F *fMXHLengthN; //!
  TH2F *fMXHParTimeS; //!
  TH2F *fMXHParTimeN; //!
  TH1F *fMXHParTimeSN; //!
  TH1F *fMXHParTime0; //!
  TH2F *fMXHStackS; //!
  TH2F *fMXHStackN; //!
  TH1F *fMXHStack0; //!
  TH2F *fMXHStatePhS; //!
  TH2F *fMXHStatePhN; //!
  TH1F *fMXHStatePh0; //!
  TH2F *fMXHFemIDS; //!
  TH2F *fMXHFemIDN; //!
  TH2F *fMXHCheckSumS; //!
  TH2F *fMXHCheckSumN; //!
  TH2F *fMXHGrayCodeS; //!
  TH2F *fMXHGrayCodeN; //!
  TH2F *fMXHFemTestS; //!
  TH2F *fMXHFemTestN; //!
  TH2F *fMXHCBTestS; //!
  TH2F *fMXHCBTestN; //!
  TH2F *fMXHCellIDMap; //!
  // CONTROL PLOTS
  TH1F *fMXHHiAdc; // !
  TH1F *fMXHLoAdc; // !

  TH2F *fMXHHiMapS; // !
  TH2F *fMXHLoMapS; // !
  TH2F *fMXHHiMapN; // !
  TH2F *fMXHLoMapN; // !
  TH2F *fMXHLoHiS; //!
  TH2F *fMXHLoHiN; //!
  TH2F *fMXHPHiMapS; // !
  TH2F *fMXHPLoMapS; // !
  TH2F *fMXHPHiMapN; // !
  TH2F *fMXHPLoMapN; // !
  TH2F *fMXHPLoHiS; //!
  TH2F *fMXHPLoHiN; //!

  TH2F *fMXHCrossingHitS; //!
  TH2F *fMXHCrossingHitN; //!
  TH2F *fMXHMPCS; //!
  TH2F *fMXHMPCN; //!
  TH2F *fMXHMPCLS; //!
  TH2F *fMXHMPCLN; //!
  TH1F *fMXHMPCTotS; //!
  TH1F *fMXHMPCTotN; //!
  TH1F *fMXHMPCErrS; //!
  TH1F *fMXHMPCErrN; //!

  TH1F *fMXHMPCrS; //!
  TH1F *fMXHMPCrN; //!

  TH2F *fMXParScanHS; //!
  TH2F *fMXParScanLS; //!
  TH2F *fMXParScanHN; //!
  TH2F *fMXParScanLN; //!
  TH2F *fMXQHLRS; //!
  TH2F *fMXQHLRN; //!
  TH2F *fMXQMIPS; //!
  TH2F *fMXQMIPN; //!

  TH2F *fMXHMPCS_T0; //!
  TH2F *fMXHMPCN_T0; //!
  TH2F *fMXHMPCLS_T0; //!
  TH2F *fMXHMPCLN_T0; //!
  TH2F *fMXHMPCS_T1; //!
  TH2F *fMXHMPCN_T1; //!
  TH2F *fMXHMPCLS_T1; //!
  TH2F *fMXHMPCLN_T1; //!
  TH2F *fMXHMPCS_T2; //!
  TH2F *fMXHMPCN_T2; //!
  TH2F *fMXHMPCLS_T2; //!
  TH2F *fMXHMPCLN_T2; //!

  // SENSORS
  TProfile *fMXHCellIDAvg; //!
  TH2F *fMXHCellIDSD; //!
  TH2F *fMXHCellIDRG; //!
  TH2F *fMXHBeamClockSD; //!
  TH2F *fMXHParTimeSD; //!
  TH1F *fMXHStackSD; //!
  TH1F *fMXHStackRG; //!
  TH1F *fMXHStatePhSD; //!
  TH1F *fMXHHitsS; //!
  TH1F *fMXHHitsN; //!
  TH1F *fMXHLHitsS; //!
  TH1F *fMXHLHitsN; //!

 private:
  void FillHistograms(Event*);
  TH1F* Histo1F(TString nam, TString tit, int nb, float bm, float bM, int col);
  TH2F* Histo2F(TString nam, TString tit, int nx, float xm, float xM, int ny, float ym, float yM);
  TProfile *Profile(TString nam, TString tit, int nx, float xm, float xM, float ym, float yM, int col);

  mxCalibMaster *fCal;
  int fHiChn[64];
  int fLoChn[64];
  int fPktMX[16];
  int fPktMC[6];
  int fPktMap[16];
};
#endif /* __MPCEXMON_H__ */
