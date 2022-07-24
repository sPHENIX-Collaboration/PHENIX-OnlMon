#ifndef __FCALMON_H__
#define __FCALMON_H__
//
//
//  Written by: Jane M. Burward-Hoy and Gerd J. Kunde
//
//  Class:  FcalMon
//
//  Description:  This class obtains ADC, TDC values from the packet id for 
//                the Forward Hadron Calorimeter (Fcal).  The two packet ids
//                correspond to north and south.  Single and two-dimensional
//                histograms are created and are registered in the Online 
//                Monitor server.  In the default constructor for this class,
//                the channel map is initialized via the initChannelConfig()
//                function, the histograms are created and registered using 
//                the initAndRegisterHistos(), and the BBC/ZDC packets are 
//                used to obtain information for FCAL/BBC correlation 
//                histograms after initializing these two subsystems using
//                the function initBBCandZDC().  
//              
//                Profile histograms store the mean and rms for a given 
//                value of interest (ADC, TDC) and are only used to store 
//                this information for retrieval in the registered histograms 
//                in the monitor (at present, a Profile histogram cannot be 
//                registered in the monitor).
//
//  Date:  initially created for the d-Au run during January 2003
//
#include "OnlMon.h"
#include "FclIndexer.h"
#include <string>

class Event;
class TH1;
class TH2;
class TProfile;
class FclCalib;
class FclIndexer;

class FcalMon: public OnlMon
{
 
 public:
  FcalMon();
  virtual ~FcalMon();

  int process_event(Event *evt);
  int Init();
  int Reset();
  int BeginRun(const int runnumber);

 protected:

  void initCalib();
  int initAndRegisterHistos();
  int getPostMinusPre(int post, int pre);
  int formatChanHist(TH1 *thisHisto);
  
  int evtcnt; // counter of events processed so far
  unsigned long ppg_trigger_mask;

  // these profile histograms store the mean and rms for the given 
  // event processing - they are only used to store this information
  // for retrieval in the registered histograms in the event monitor

  TProfile *hpFCAL_LOADC_MEAN[2];

  TH2 *h2FCAL_LOADC[2];
  //Calibrated Logain ADC
  TH2 *h2FCAL_LOADC_CORRECTED[2];

  FclCalib      *fclcalib[2];
  FclIndexer* indexer;

};

#endif  /* __FCALMON_H__ */
