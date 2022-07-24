#include "MuidLvl1Mon.h"
#include <MuIDLl1.h>

#include <OnlMonServer.h>
#include <OnlMonDB.h>

#include <Event.h>
#include <msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TFile.h>

#include <cmath>
#include <iostream>
#include <sstream>

// Trigger definitions - change these if the triggers change

// PP200Run12/PP510Run12
//#define MUIDLL1_N1D_OR_S1D_AND_BBCLL1_NO_VTX        0x00020000
//#define MUIDLL1_N2D_OR_S2D_AND_BBCLL1_NO_VTX        0x00010000
//#define BBCLL1_NO_VTX                               0x00000002
//#define AllForwardArmTriggers 0xFFFF0200

// He3AuRun14
//#define MUIDLL1_N1D_AND_BBCLL1                      0x00100000
//#define MUIDLL1_S1D_AND_BBCLL1                      0x00200000
//#define BBCLL1                                      0x00000020
//#define MUIDLL1_N2D_AND_BBCLL1                      0x00040000  
//#define MUIDLL1_S2D_AND_BBCLL1                      0x00080000  

// PP200Run15
//#define MUIDLL1_N1D_AND_BBCLL1                      0x00400000
//#define MUIDLL1_S1D_AND_BBCLL1                      0x00800000
//#define BBCLL1                                      0x00000002
//#define MUIDLL1_N2D_AND_BBCLL1                      0x00100000  
//#define MUIDLL1_S2D_AND_BBCLL1                      0x00200000

// pAu200Run15
#define MUIDLL1_N1D_AND_BBCLL1                      0x00002000
#define MUIDLL1_S1D_AND_BBCLL1                      0x00004000
#define BBCLL1                                      0x00000010
#define MUIDLL1_N2D_AND_BBCLL1                      0x01000000  
#define MUIDLL1_S2D_AND_BBCLL1                      0x02000000

// AuAuRun16
//#define MUIDLL1_N1D_AND_BBCLL1                      0x00100000
//#define MUIDLL1_S1D_AND_BBCLL1                      0x00200000
//#define BBCLL1                                      0x00000020
//#define MUIDLL1_N2D_AND_BBCLL1                      0x00040000  
//#define MUIDLL1_S2D_AND_BBCLL1                      0x00080000  


enum infohist_indexes{NOTHING, OFFLINE_INDEX, NUM_EVTS_INDEX};

using namespace std;

bool firstInit = true; 

MuidLvl1Mon::MuidLvl1Mon(): OnlMon("MUIDLVL1MON")
{

	offline_version = 0;

  if(!firstInit) return; 

  firstInit = false;

  // Clear counters

  Reset();
 
  // Set up database

  dbvars = new OnlMonDB(ThisName); // use monitor name for db table name
  DBVarInit();

  // Create a simulator

  simMuIDLl1 = new MuIDLl1("NO_DIRECTORY_PATH");
  if(NOISE_CUT!=0) simMuIDLl1->setROCNoiseCut(true);
  if(PANEL_ALG!=0) {
    simMuIDLl1->setNewPanelAlg(true); 
  }
  else{
    simMuIDLl1->setNewPanelAlg(false); 
  }

  // Create Histograms
  const string ArmNS[2] = {"S", "N"};
  for (int i = 0; i < 2; i++)
    {
      string htit = "MuidLvl1_DataErr" + ArmNS[i];
      MuidLvl1_DataErr[i] = new TH1F(htit.c_str(),
                                     "Number of ROC bits set in matched 1D events",
                                     100, 0., 300.);

      htit = "MuidLvl1_SyncErr" + ArmNS[i];
      MuidLvl1_SyncErr[i] = new TH1F(htit.c_str(),
                                     "Number of ROC bits set in unmatched 1D events",
                                     100, 0., 300.);

      htit = "MuidLvl1_DataErr2" + ArmNS[i];
      MuidLvl1_DataErr2[i] = new TH1F(htit.c_str(),
                                     "Number of ROC bits set in matched 2D events",
                                     100, 0., 1000.);

      htit = "MuidLvl1_SyncErr2" + ArmNS[i];
      MuidLvl1_SyncErr2[i] = new TH1F(htit.c_str(),
                                     "Number of ROC bits set in unmatched 2D events",
                                     100, 0., 1000.);

      htit = "MuidLvl1_HorRoadNum" + ArmNS[i];
      MuidLvl1_HorRoadNum[i] = new TH1F(htit.c_str(),
                                        "No. of Horizontal Deep Symsets in Vector (1D)",
                                        20, 0., 20.0);

      htit = "MuidLvl1_HorRoadNumShal" + ArmNS[i];
      MuidLvl1_HorRoadNumShal[i] = new TH1F(htit.c_str(),
                                            "No. of Horizontal Deep Symsets in Vector (2D)",
                                            200, 0., 200.0);

      htit = "MuidLvl1_VerRoadNum" + ArmNS[i];
      MuidLvl1_VerRoadNum[i] = new TH1F(htit.c_str(),
                                        "No. of Vertical Deep Symsets in Vector (1D)",
                                        20, 0., 20.0);

      htit = "MuidLvl1_VerRoadNumShal" + ArmNS[i];
      MuidLvl1_VerRoadNumShal[i] = new TH1F(htit.c_str(),
                                            "No. of Vertical Deep Symsets in Vector (2D)",
                                            200, 0., 200.0);

      htit = "MuidLvl1_HorRoadNumSim" + ArmNS[i];
      MuidLvl1_HorRoadNumSim[i] = new TH1F(htit.c_str(),
                                           "No. of Horiz. Symsets in Vector (SIMULATED)",
                                           20, 0., 20.0);

      htit = "MuidLvl1_VerRoadNumSim" + ArmNS[i];
      MuidLvl1_VerRoadNumSim[i] = new TH1F(htit.c_str(),
                                           "No. of Vert. Symsets in Vector (SIMULATED)",
                                           20, 0., 20.0);

      htit = "MuidLvl1_HorRoadNumShalSim" + ArmNS[i];
      MuidLvl1_HorRoadNumShalSim[i] = new TH1F(htit.c_str(),
					       "No. of Horiz. Shallow Symsets in Vector (SIMULATED)",
					       20, 0., 20.0);

      htit = "MuidLvl1_VerRoadNumShalSim" + ArmNS[i];
      MuidLvl1_VerRoadNumShalSim[i] = new TH1F(htit.c_str(),
					       "No. of Vert. Shallow Symsets in Vector (SIMULATED)",
					       20, 0., 20.0);

      htit = "MuidLvl1_HorRoadCount" + ArmNS[i];
      MuidLvl1_HorRoadCount[i] = new TH1F(htit.c_str(),
                                          "Horizontal Symset Frequency",
                                          HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_HorRoadCountShal" + ArmNS[i];
      MuidLvl1_HorRoadCountShal[i] = new TH1F(htit.c_str(),
                                              "Horizontal Symset Frequency",
                                              HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerRoadCount" + ArmNS[i];
      MuidLvl1_VerRoadCount[i] = new TH1F(htit.c_str(),
                                          "Vertical Symset Frequency",
                                          VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_VerRoadCountShal" + ArmNS[i];
      MuidLvl1_VerRoadCountShal[i] = new TH1F(htit.c_str(),
                                              "Vertical Symset Frequency",
                                              VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_HorRoadCountSim" + ArmNS[i];
      MuidLvl1_HorRoadCountSim[i] = new TH1F(htit.c_str(),
                                             "Horizontal Symset Frequency (SIM)",
                                             HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerRoadCountSim" + ArmNS[i];
      MuidLvl1_VerRoadCountSim[i] = new TH1F(htit.c_str(),
                                             "Vertical Symset Frequency (SIM)",
                                             VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_HorRoadCountShalSim" + ArmNS[i];
      MuidLvl1_HorRoadCountShalSim[i] = new TH1F(htit.c_str(),
						 "Horizontal Symset Frequency (SIM)",
						 HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerRoadCountShalSim" + ArmNS[i];
      MuidLvl1_VerRoadCountShalSim[i] = new TH1F(htit.c_str(),
						 "Vertical Symset Frequency (SIM)",
						 VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_HorRoadCountDif" + ArmNS[i];
      MuidLvl1_HorRoadCountDif[i] = new TH1F(htit.c_str(),
                                             "Horizontal Symset Frequency (SIM - LL1)",
                                             HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerRoadCountDif" + ArmNS[i];
      MuidLvl1_VerRoadCountDif[i] = new TH1F(htit.c_str(),
                                             "Vertical Symset Frequency (SIM - LL1)",
                                             VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_CompareVH" + ArmNS[i];
      MuidLvl1_CompareVH[i] = new TH2F(htit.c_str(),
                                       "Vertical Roads vs. Horizontal Roads (1D)",
                                       30, -0.5, 5.5, 30, -0.5, 5.5);

      htit = "MuidLvl1_CompareSumsV" + ArmNS[i];
      MuidLvl1_CompareSumsV[i] = new TH2F(htit.c_str(),
                                          "UNUSED",
                                          30, -0.5, 5.5, 30, -0.5, 5.5);

      htit = "MuidLvl1_CompareSumsH" + ArmNS[i];
      MuidLvl1_CompareSumsH[i] = new TH2F(htit.c_str(),
                                          "UNUSED",
                                          30, -0.5, 5.5, 30, -0.5, 5.5);

      htit = "MuidLvl1_ESNMismatch" + ArmNS[i];
      MuidLvl1_ESNMismatch[i] = new TH1F(htit.c_str(),
                                         "ESN Mismatch (-1=Hor, 1= Ver)",
                                         3, -1.5, 1.5);

      htit = "MuidLvl1_ESNCtlMis_V" + ArmNS[i];
      MuidLvl1_ESNCtlMis_V[i] = new TH1F(htit.c_str(),
                                         "Alg Chips ESN Mismatch CTL Chip (Vert.)",
                                         5, 0.5, 5.5);

      htit = "MuidLvl1_ESNCtlMis_H" + ArmNS[i];
      MuidLvl1_ESNCtlMis_H[i] = new TH1F(htit.c_str(),
                                         "Alg Chips ESN Mismatch CTL Chip (Horiz)",
                                         5, 0.5, 5.5);

      htit = "MuidLvl1_HorSymEff" + ArmNS[i];
      MuidLvl1_HorSymEff[i] = new TH1F(htit.c_str(),
                                       "Horizontal Symset Efficiency (LL1/Simulated)",
                                       HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerSymEff" + ArmNS[i];
      MuidLvl1_VerSymEff[i] = new TH1F(htit.c_str(),
                                       "Vertical Symset Efficiency (LL1/Simulated)",
                                       VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_HorSymEffCut" + ArmNS[i];
      MuidLvl1_HorSymEffCut[i] = new TH1F(htit.c_str(),
                                          "Horizontal Symset Efficiency (LL1/Simulated, Cut)",
                                          HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerSymEffCut" + ArmNS[i];
      MuidLvl1_VerSymEffCut[i] = new TH1F(htit.c_str(),
                                          "Vertical Symset Efficiency (LL1/Simulated, Cut)",
                                          VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_HorSymEffCutShal" + ArmNS[i];
      MuidLvl1_HorSymEffCutShal[i] = new TH1F(htit.c_str(),
                                              "Horizontal Symset Efficiency (LL1/Simulated, Cut)",
                                              HORROADS, 0., float(HORROADS));

      htit = "MuidLvl1_VerSymEffCutShal" + ArmNS[i];
      MuidLvl1_VerSymEffCutShal[i] = new TH1F(htit.c_str(),
                                              "Vertical Symset Efficiency (LL1/Simulated, Cut)",
                                              VERROADS, 0., float(VERROADS));

      htit = "MuidLvl1_GL1Eff" + ArmNS[i];
      MuidLvl1_GL1Eff[i] = new TH1F(htit.c_str(),
                                    "Trigger Efficiency As Measured by GL1 (1D/2D/1S/1S1D)",
                                    6, 0., 6.0);

    }
		
      MuidLvl1_InfoHist = new TH1I("MuidLvl1_InfoHist","InfoHist",2, 0, 2);

  // Register Histograms

  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  for (int i = 0; i < 2;i++)
    {
      Onlmonserver->registerHisto(this, MuidLvl1_DataErr[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_SyncErr[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_DataErr2[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_SyncErr2[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadNum[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadNum[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadNumShal[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadNumShal[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadNumSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadNumSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadNumShalSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadNumShalSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadCount[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadCount[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadCountShal[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadCountShal[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadCountSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadCountSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadCountShalSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadCountShalSim[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorRoadCountDif[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerRoadCountDif[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_CompareVH[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_CompareSumsV[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_CompareSumsH[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_ESNMismatch[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_ESNCtlMis_V[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_ESNCtlMis_H[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorSymEff[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerSymEff[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorSymEffCut[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerSymEffCut[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_HorSymEffCutShal[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_VerSymEffCutShal[i]);
      Onlmonserver->registerHisto(this, MuidLvl1_GL1Eff[i]);
    }
		Onlmonserver->registerHisto(this, MuidLvl1_InfoHist);

}

MuidLvl1Mon::~MuidLvl1Mon()
{
  delete simMuIDLl1;
  if (dbvars)
    {
      delete dbvars;
    }

}

int MuidLvl1Mon::EndRun(const int runno)
{
  

  return 0;

}

int MuidLvl1Mon::BeginRun(const int runno)
{

  Reset();
  return 0;

}

int MuidLvl1Mon::process_event(Event *event)
{
  // Only data events
  if (event->getEvtType() != 1) return 0; 
	
  // Check triggers with GL1 trigger bits
 
  bool S1DFound = false; 
  bool S1DFoundBoth = false;
  bool N1DFound = false; 
  bool N1DFoundBoth = false;

  bool S2DFound = false; 
  bool S2DFoundBoth = false;
  bool N2DFound = false; 
  bool N2DFoundBoth = false;
  
  int numBitsH[2] = {0,0}; 
  int numBitsV[2] = {0,0}; 

  Packet* p;
  if ((p = event->getPacket(14001))){

    //unsigned int livetrig = p->iValue(0, "LIVETRIG");
    unsigned int rawtrig = p->iValue(0, "RAWTRIG");
    //unsigned int scaledtrig = p->iValue(0, "SCALEDTRIG");
    //unsigned int rbits0 = p->iValue(0,"RBITS0"); 
    unsigned int rbits1 = p->iValue(0,"RBITS1");
    //unsigned int grandis = p->iValue(0,"GDISABLE");

    unsigned char SHD = (rbits1&0x30000)>>16;
    unsigned char SVD = (rbits1&0x300000)>>20;

    unsigned char NHD = (rbits1&0x3000000)>>24;
    unsigned char NVD = (rbits1&0x30000000)>>28;
    
    bool SimRun = false;

    // Standard 1D/1S efficiency checks only for SCALED BBCLL1 trigger

    //int MuidNotScaled = scaledtrig&(~AllForwardArmTriggers);
		
    //if( MuidNotScaled && (rawtrig&BBCLL1_NO_VTX)!=0){
    
    if(rawtrig&BBCLL1){ //AuAuRun14

      //if( (scaledtrig&BBCLL1)!=0 ){       
	  
      if(!SimRun){

        // Run the simulator

        simMuIDLl1->getDataFromMuidPacket(event); 

        // Run Simulated MuID LL1 Trigger

        simMuIDLl1->calculate(SimSyncErr);

      }

      evtcnt++;

      // Check results against GL1 bits

      if ( (rawtrig&MUIDLL1_N1D_AND_BBCLL1)!=0 ) OneDeepNorthGL1++; 
      if ( (rawtrig&MUIDLL1_S1D_AND_BBCLL1)!=0 ) OneDeepSouthGL1++; 
      if ( (rawtrig&MUIDLL1_N2D_AND_BBCLL1)!=0 ) TwoDeepNorthGL1++; 
      if ( (rawtrig&MUIDLL1_S2D_AND_BBCLL1)!=0 ) TwoDeepSouthGL1++; 
      //if ( (rawtrig&MUIDLL1_N1D_OR_S1D_AND_BBCLL1_NO_VTX)!=0 ) OneDeepGL1++; 
      //if ( (rawtrig&MUIDLL1_N2D_OR_S2D_AND_BBCLL1_NO_VTX)!=0 ) TwoDeepGL1++; 

      if(NHD>0 && NVD>0) rbitsN1D++;
      if(SHD>0 && SVD>0) rbitsS1D++;
      if( (NHD>0 && NVD>0) || (SHD>0 && SVD>0)) rbits1D++;

      if(NHD>1 && NVD>1) rbitsN2D++;
      if(SHD>1 && SVD>1) rbitsS2D++;
      if( ((NHD>1 && NVD>1) || (SHD>1 && SVD>1)) || ( (NHD>0 && NVD>0)&&(SHD>0 && SVD>0) ) ) rbits2D++;
 
      //MuIDLL1_S2D&BBCLL1
      if(simMuIDLl1->GL1_2Deep_S()){
        TwoDeepSouthSim++;
	if((rawtrig&MUIDLL1_S2D_AND_BBCLL1)!=0){
	    TwoDeepSouth++;
	}
      }

      //MuIDLL1_N2D&BBCLL1
      if(simMuIDLl1->GL1_2Deep_N()){
	TwoDeepNorthSim++;
	if((rawtrig&MUIDLL1_N2D_AND_BBCLL1)!=0){
	  TwoDeepNorth++;
        }
      }
       
      //MuIDLL1_S1D&BBCLL1
      if(simMuIDLl1->GL1_1Deep_S()) {

        OneDeepSouthSim++;
        S1DFound = true;

        if ((rawtrig&MUIDLL1_S1D_AND_BBCLL1)!=0){

          OneDeepSouth++;
	  S1DFoundBoth = true; 

	}
	else{
	  S1DFoundBoth = false; 
	}

      }
      else
	S1DFound = false; 


      //MuIDLL1_N1D&BBCLL1
      if(simMuIDLl1->GL1_1Deep_N()){

          OneDeepNorthSim++;
	  N1DFound = true;

          if ((rawtrig&MUIDLL1_N1D_AND_BBCLL1)!=0){
            OneDeepNorth++;
	    N1DFoundBoth = true; 

	  }
	  else
	    N1DFoundBoth = false;
	    
      }
      else
	N1DFound = false; 

      /*
      
      //(MuIDLL1_N1D || MUIDLL1_S1D) && BBCLL1

      if( simMuIDLl1->GL1_1Deep_N() || simMuIDLl1->GL1_1Deep_S() )
      {
        OneDeepCombSim++;
	if(simMuIDLl1->GL1_1Deep_N()) {
	  OneDeepCombSimN++;
	  N1DFound = true;
	}
	if(simMuIDLl1->GL1_1Deep_S()) {
	  OneDeepCombSimS++; 
	  S1DFound = true;
	}
        if (rawtrig&MUIDLL1_N1D_OR_S1D_AND_BBCLL1_NO_VTX) {
	  OneDeepComb++;
	  if(S1DFound) {
	    OneDeepCombS++; 
	    S1DFoundBoth = true;
	  }
	  if(N1DFound) {
	    OneDeepCombN++; 
	    N1DFoundBoth = true;
	  }
	}

      }

      //((MuIDLL1_N2D || MUIDLL1_S2D) || (N1D && S2D) ) && BBCLL1

      // Test only the 2D component
      // Only test events that don't look like ringing in the MuID

      //if( simMuIDLl1->GL1_2Deep_N() || simMuIDLl1->GL1_2Deep_S() || (simMuIDLl1->GL1_1Deep_N() && simMuIDLl1->GL1_1Deep_S()))
      //if( (simMuIDLl1->GL1_2Deep_N() && (numBitsH[1]<50) && (numBitsV[1]<50)) || 
      //	  (simMuIDLl1->GL1_2Deep_S() && (numBitsH[0]<50) && (numBitsV[0]<50)) )
      if( simMuIDLl1->GL1_2Deep_N() || simMuIDLl1->GL1_2Deep_S() )
      {
        TwoDeepCombSim++;
	//if(simMuIDLl1->GL1_2Deep_N() || (simMuIDLl1->GL1_1Deep_N() && simMuIDLl1->GL1_1Deep_S()) ) N2DFound = true;
	//if(simMuIDLl1->GL1_2Deep_S() || (simMuIDLl1->GL1_1Deep_N() && simMuIDLl1->GL1_1Deep_S()) ) S2DFound = true;
	if(simMuIDLl1->GL1_2Deep_N() ) {
	  TwoDeepCombSimN++; 
	  N2DFound = true;
	}
	if(simMuIDLl1->GL1_2Deep_S() ) {
	  TwoDeepCombSimS++; 
	  S2DFound = true;
	}
        if (rawtrig&MUIDLL1_N2D_OR_S2D_AND_BBCLL1_NO_VTX) {
	  TwoDeepComb++;
	  if(S2DFound) {
	    TwoDeepCombS++;
	    S2DFoundBoth = true;
	  }
	  if(N2DFound) {
	    TwoDeepCombN++;
	    N2DFoundBoth = true;
	  }
	}
      }

      // Keep track of full trigger for GL1 ratio

      if( simMuIDLl1->GL1_2Deep_N() || simMuIDLl1->GL1_2Deep_S() || (simMuIDLl1->GL1_1Deep_N() && simMuIDLl1->GL1_1Deep_S())){
	TwoDeepCombSimFull++; 
      }

      */

      // Fill histograms with simulated symset vectors
 
      long SimSymVec[5];
      int SHNumRoadsSim = 0, SVNumRoadsSim = 0;
      int NHNumRoadsSim = 0, NVNumRoadsSim = 0;
      int SHNumRoadsSimShal = 0, SVNumRoadsSimShal = 0;
      int NHNumRoadsSimShal = 0, NVNumRoadsSimShal = 0;

      SHNumEvt++; 
      SVNumEvt++; 
      NHNumEvt++; 
      NVNumEvt++; 

      numBitsH[0] = simMuIDLl1->getNumROCBitsSet(0,0); 
      numBitsV[0] = simMuIDLl1->getNumROCBitsSet(0,1); 
      numBitsH[1] = simMuIDLl1->getNumROCBitsSet(1,0); 
      numBitsV[1] = simMuIDLl1->getNumROCBitsSet(1,1);

      if(S1DFound){
	  
	// South Horizontal
	if(PANEL_ALG!=0){

	  for(int panel=0; panel<PANELS_PER_GAP; panel++){
	    simMuIDLl1->getPanelSymsetVector(0, 0, panel, SimSymVec);

	    for (int i = 0; i < 5; i++){
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {

			if( (panel==0) && ( ((i*32+j)<58) || ((i*32+j)>117) )) continue;  
			if( (panel==1) && ( ((i*32+j)<70) || ((i*32+j)>115) )) continue;  
			if( (panel==2) && ( ((i*32+j)<58) || ((i*32+j)>117) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>59) )) continue;  
			if( (panel==4) && ( ((i*32+j)<1) || ((i*32+j)>45) )) continue;  
			if( (panel==5) && ( ((i*32+j)<1) || ((i*32+j)>59) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			     SHNumRoadsSim++;
			     HorRoadsSim[i*32 + j]++;
			     if(S1DFoundBoth) HorRoads[i*32 +j]++; 
			  }
		      }
		  }
	    }

	  }

	}
	else{
	  simMuIDLl1->getSymsetVector(0, 0, SimSymVec);

	  for (int i = 0; i < 5; i++){
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {


			if ( SimSymVec[i]&(0x1 << j) )
			  {
			     SHNumRoadsSim++;
			     HorRoadsSim[i*32 + j]++;
			     if(S1DFoundBoth) HorRoads[i*32 +j]++; 
			  }
		      }
		  }
	  }

	}

	MuidLvl1_HorRoadNum[0]->Fill(SHNumRoadsSim);
	if (SHNumEvt > 0) {
	  for (int i = 0; i < HORROADS; i++) {
	    MuidLvl1_HorRoadCountSim[0]->SetBinContent(i + 1, (float)(HorRoadsSim[i]) / (float)(SHNumEvt));
	    MuidLvl1_HorRoadCount[0]->SetBinContent(i + 1, (float)(HorRoads[i]) / (float)(SHNumEvt));
	  }
	}

	// South Vertical
	if(PANEL_ALG!=0){

	  for(int panel=0; panel<PANELS_PER_GAP; panel++){
	    simMuIDLl1->getPanelSymsetVector(0, 1, panel, SimSymVec);

	    for (int i = 0; i < 5; i++){

		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if( (panel==0) && ( ((i*32+j)<91) || ((i*32+j)>155) )) continue;  
			if( (panel==1) && ( ((i*32+j)<65) || ((i*32+j)>90) )) continue;  
			if( (panel==2) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==4) && ( ((i*32+j)<64) || ((i*32+j)>90) )) continue;  
			if( (panel==5) && ( ((i*32+j)<91) || ((i*32+j)>154) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    SVNumRoadsSim++;
			    VerRoadsSim[i*32 + j]++;
			    if(S1DFoundBoth) VerRoads[i*32 +j]++; 


			  }
		      }
		  }
	     }

	  }

	}
	else{

	  simMuIDLl1->getSymsetVector(0, 1, SimSymVec);

	  for (int i = 0; i < 5; i++){

		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    SVNumRoadsSim++;
			    VerRoadsSim[i*32 + j]++;
			    if(S1DFoundBoth) VerRoads[i*32 +j]++; 
			  }
		      }
		  }
	   }

	}

	MuidLvl1_VerRoadNum[0]->Fill(SVNumRoadsSim);
	if (SVNumEvt > 0) {
	  for (int i = 0; i < VERROADS; i++){
	    MuidLvl1_VerRoadCountSim[0]->SetBinContent(i + 1, (float)(VerRoadsSim[i]) / (float)(SVNumEvt));
	    MuidLvl1_VerRoadCount[0]->SetBinContent(i + 1, (float)(VerRoads[i]) / (float)(SVNumEvt));
	  }
	}
 
	if(S1DFoundBoth)
	  MuidLvl1_DataErr[0]->Fill(numBitsH[0]+numBitsV[0]); 
	else
	  MuidLvl1_SyncErr[0]->Fill(numBitsH[0]+numBitsV[0]); 


	MuidLvl1_CompareVH[0]->Fill(SHNumRoadsSim,SVNumRoadsSim); 

      }

       if(N1DFound){

	// North Horizontal
	if(PANEL_ALG!=0){

	  for(int panel=0; panel<PANELS_PER_GAP; panel++){
	    simMuIDLl1->getPanelSymsetVector(1, 0, panel, SimSymVec);

	    for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {
			if( (panel==0) && ( ((i*32+j)<59) || ((i*32+j)>117) )) continue;  
			if( (panel==1) && ( ((i*32+j)<71) || ((i*32+j)>117) )) continue;  
			if( (panel==2) && ( ((i*32+j)<59) || ((i*32+j)>117) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>60) )) continue;  
			if( (panel==4) && ( ((i*32+j)<1) || ((i*32+j)>46) )) continue;  
			if( (panel==5) && ( ((i*32+j)<1) || ((i*32+j)>60) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NHNumRoadsSim++;
			    HorRoadsSim[i*32 + j + HORROADS]++;
			    if(N1DFoundBoth) HorRoads[i*32 + j + HORROADS]++; 
			  }
		      }
		  }
	     }

	  }

	}
	else{

	  simMuIDLl1->getSymsetVector(1, 0, SimSymVec);

	  for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {
			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NHNumRoadsSim++;
			    HorRoadsSim[i*32 + j + HORROADS]++;
			    if(N1DFoundBoth) HorRoads[i*32 + j + HORROADS]++; 
			  }
		      }
		  }
	   }

	}

	MuidLvl1_HorRoadNum[1]->Fill(NHNumRoadsSim);
	if (NHNumEvt > 0){
	  for (int i = 0; i < HORROADS; i++){
	    MuidLvl1_HorRoadCountSim[1]->SetBinContent(i + 1, (float)(HorRoadsSim[i + HORROADS]) / (float)(NHNumEvt));
	    MuidLvl1_HorRoadCount[1]->SetBinContent(i + 1, (float)(HorRoads[i+HORROADS]) / (float)(NHNumEvt));
	  }
	}


	// North Vertical

	if(PANEL_ALG!=0){

	    for(int panel=0; panel<PANELS_PER_GAP; panel++){
	      simMuIDLl1->getPanelSymsetVector(1, 1, panel, SimSymVec);
	      for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if( (panel==0) && ( ((i*32+j)<91) || ((i*32+j)>155) )) continue;  
			if( (panel==1) && ( ((i*32+j)<65) || ((i*32+j)>90) )) continue;  
			if( (panel==2) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==4) && ( ((i*32+j)<65) || ((i*32+j)>90) )) continue;  
			if( (panel==5) && ( ((i*32+j)<90) || ((i*32+j)>154) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NVNumRoadsSim++;
			    VerRoadsSim[i*32 + j + VERROADS]++;
			    if(N1DFoundBoth) VerRoads[i*32 + j + VERROADS]++;
			  }
		      }
		  }
	      }

	    }

	}
	else{

	    simMuIDLl1->getSymsetVector(1, 1, SimSymVec);
	    for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NVNumRoadsSim++;
			    VerRoadsSim[i*32 + j + VERROADS]++;
			    if(N1DFoundBoth) VerRoads[i*32 + j + VERROADS]++;
			  }
		      }
		  }
	      }

	}


	MuidLvl1_VerRoadNum[1]->Fill(NVNumRoadsSim);
	if (NVNumEvt > 0){
	  for (int i = 0; i < VERROADS; i++){
	    MuidLvl1_VerRoadCountSim[1]->SetBinContent(i + 1, (float)(VerRoadsSim[i + VERROADS]) / (float)(NVNumEvt));
	    MuidLvl1_VerRoadCount[1]->SetBinContent(i + 1, (float)(VerRoads[i + VERROADS]) / (float)(NVNumEvt));
	  }
	}
   
	if(N1DFoundBoth)
	  MuidLvl1_DataErr[1]->Fill(numBitsH[1]+numBitsV[1]); 
	else
	  MuidLvl1_SyncErr[1]->Fill(numBitsH[1]+numBitsV[1]); 

	MuidLvl1_CompareVH[1]->Fill(NHNumRoadsSim,NVNumRoadsSim); 

      }

      // 2D trigger distributions


      if(S2DFound){
	  
	// South Horizontal
	if(PANEL_ALG!=0){

	  for(int panel=0; panel<PANELS_PER_GAP; panel++){
	    simMuIDLl1->getPanelSymsetVector(0, 0, panel, SimSymVec);

	    for (int i = 0; i < 5; i++){
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {

			if( (panel==0) && ( ((i*32+j)<58) || ((i*32+j)>117) )) continue;  
			if( (panel==1) && ( ((i*32+j)<70) || ((i*32+j)>115) )) continue;  
			if( (panel==2) && ( ((i*32+j)<58) || ((i*32+j)>117) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>59) )) continue;  
			if( (panel==4) && ( ((i*32+j)<1) || ((i*32+j)>45) )) continue;  
			if( (panel==5) && ( ((i*32+j)<1) || ((i*32+j)>59) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			     SHNumRoadsSimShal++;
			     HorRoadsShalSim[i*32 + j]++;
			     if(S2DFoundBoth) HorRoadsShal[i*32 + j]++;
			  }
		      }
		  }
	    }

	  }

	}
	else{
	  simMuIDLl1->getSymsetVector(0, 0, SimSymVec);

	  for (int i = 0; i < 5; i++){
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {


			if ( SimSymVec[i]&(0x1 << j) )
			  {
			     SHNumRoadsSimShal++;
			     HorRoadsShalSim[i*32 + j]++;
			     if(S2DFoundBoth) HorRoadsShal[i*32 + j]++;
			  }
		      }
		  }
	  }

	}

	MuidLvl1_HorRoadNumShal[0]->Fill(SHNumRoadsSimShal);
	if (SHNumEvt > 0) {
	  for (int i = 0; i < HORROADS; i++) {
	    MuidLvl1_HorRoadCountShalSim[0]->SetBinContent(i + 1, (float)(HorRoadsShalSim[i]) / (float)(SHNumEvt));
	    MuidLvl1_HorRoadCountShal[0]->SetBinContent(i + 1, (float)(HorRoadsShal[i]) / (float)(SHNumEvt));
	  }
	}

	// South Vertical
	if(PANEL_ALG!=0){

	  for(int panel=0; panel<PANELS_PER_GAP; panel++){
	    simMuIDLl1->getPanelSymsetVector(0, 1, panel, SimSymVec);

	    for (int i = 0; i < 5; i++){

		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if( (panel==0) && ( ((i*32+j)<91) || ((i*32+j)>155) )) continue;  
			if( (panel==1) && ( ((i*32+j)<65) || ((i*32+j)>90) )) continue;  
			if( (panel==2) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==4) && ( ((i*32+j)<64) || ((i*32+j)>90) )) continue;  
			if( (panel==5) && ( ((i*32+j)<91) || ((i*32+j)>154) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    SVNumRoadsSimShal++;
			    VerRoadsShalSim[i*32 + j]++;
			    if(S2DFoundBoth) VerRoadsShal[i*32 +j]++; 
			  }
		      }
		  }
	     }

	  }

	}
	else{

	  simMuIDLl1->getSymsetVector(0, 1, SimSymVec);

	  for (int i = 0; i < 5; i++){

		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    SVNumRoadsSimShal++;
			    VerRoadsShalSim[i*32 + j]++;
			    if(S2DFoundBoth) VerRoadsShal[i*32 +j]++; 
			  }
		      }
		  }
	   }

	}

	MuidLvl1_VerRoadNumShal[0]->Fill(SVNumRoadsSimShal);
	if (SVNumEvt > 0) {
	  for (int i = 0; i < VERROADS; i++){
	    MuidLvl1_VerRoadCountShalSim[0]->SetBinContent(i + 1, (float)(VerRoadsShalSim[i]) / (float)(SVNumEvt));
	    MuidLvl1_VerRoadCountShal[0]->SetBinContent(i + 1, (float)(VerRoadsShal[i]) / (float)(SVNumEvt));
	  }
	}
 
        //cout << " S2D: " << SHNumRoadsSimShal << " " << SVNumRoadsSimShal << endl; 

	if(S2DFoundBoth)
	  MuidLvl1_DataErr2[0]->Fill(numBitsH[0]+numBitsV[0]); 
	else
	  MuidLvl1_SyncErr2[0]->Fill(numBitsH[0]+numBitsV[0]); 

      }


       if(N2DFound){

	// North Horizontal
	if(PANEL_ALG!=0){

	  for(int panel=0; panel<PANELS_PER_GAP; panel++){
	    simMuIDLl1->getPanelSymsetVector(1, 0, panel, SimSymVec);

	    for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {
			if( (panel==0) && ( ((i*32+j)<59) || ((i*32+j)>117) )) continue;  
			if( (panel==1) && ( ((i*32+j)<71) || ((i*32+j)>117) )) continue;  
			if( (panel==2) && ( ((i*32+j)<59) || ((i*32+j)>117) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>60) )) continue;  
			if( (panel==4) && ( ((i*32+j)<1) || ((i*32+j)>46) )) continue;  
			if( (panel==5) && ( ((i*32+j)<1) || ((i*32+j)>60) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NHNumRoadsSimShal++;
			    HorRoadsShalSim[i*32 + j + HORROADS]++;
			    if(N2DFoundBoth) HorRoadsShal[i*32 + j + HORROADS]++; 
			  }
		      }
		  }
	     }

	  }

	}
	else{

	  simMuIDLl1->getSymsetVector(1, 0, SimSymVec);

	  for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < HORROADS )
		      {
			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NHNumRoadsSimShal++;
			    HorRoadsShalSim[i*32 + j + HORROADS]++;
			    if(N2DFoundBoth) HorRoadsShal[i*32 + j + HORROADS]++; 
			  }
		      }
		  }
	   }

	}

	MuidLvl1_HorRoadNumShal[1]->Fill(NHNumRoadsSimShal);
	if (NHNumEvt > 0){
	  for (int i = 0; i < HORROADS; i++){
	    MuidLvl1_HorRoadCountShalSim[1]->SetBinContent(i + 1, (float)(HorRoadsShalSim[i + HORROADS]) / (float)(NHNumEvt));
	    MuidLvl1_HorRoadCountShal[1]->SetBinContent(i + 1, (float)(HorRoadsShal[i+HORROADS]) / (float)(NHNumEvt));
	  }
	}


	// North Vertical

	if(PANEL_ALG!=0){

	    for(int panel=0; panel<PANELS_PER_GAP; panel++){
	      simMuIDLl1->getPanelSymsetVector(1, 1, panel, SimSymVec);
	      for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if( (panel==0) && ( ((i*32+j)<91) || ((i*32+j)>155) )) continue;  
			if( (panel==1) && ( ((i*32+j)<65) || ((i*32+j)>90) )) continue;  
			if( (panel==2) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==3) && ( ((i*32+j)<1) || ((i*32+j)>64) )) continue;  
			if( (panel==4) && ( ((i*32+j)<65) || ((i*32+j)>90) )) continue;  
			if( (panel==5) && ( ((i*32+j)<90) || ((i*32+j)>154) )) continue;  

			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NVNumRoadsSimShal++;
			    VerRoadsShalSim[i*32 + j + VERROADS]++;
			    if(N2DFoundBoth) VerRoadsShal[i*32 + j + VERROADS]++;
			  }
		      }
		  }
	      }

	    }

	}
	else{

	    simMuIDLl1->getSymsetVector(1, 1, SimSymVec);
	    for (int i = 0; i < 5; i++)
	      {
		for (int j = 0; j < 32; j++)
		  {
		    if ( (i*32 + j) < VERROADS)
		      {
			if ( SimSymVec[i]&(0x1 << j) )
			  {
			    NVNumRoadsSimShal++;
			    VerRoadsShalSim[i*32 + j + VERROADS]++;
			    if(N2DFoundBoth) VerRoadsShal[i*32 + j + VERROADS]++;
			  }
		      }
		  }
	      }

	}


	MuidLvl1_VerRoadNumShal[1]->Fill(NVNumRoadsSimShal);
	if (NVNumEvt > 0){
	  for (int i = 0; i < VERROADS; i++){
	    MuidLvl1_VerRoadCountShalSim[1]->SetBinContent(i + 1, (float)(VerRoadsShalSim[i + VERROADS]) / (float)(NVNumEvt));
	    MuidLvl1_VerRoadCountShal[1]->SetBinContent(i + 1, (float)(VerRoadsShal[i + VERROADS]) / (float)(NVNumEvt));
	  }
	}
   
	if(N2DFoundBoth)
	  MuidLvl1_DataErr2[1]->Fill(numBitsH[1]+numBitsV[1]); 
	else
	  MuidLvl1_SyncErr2[1]->Fill(numBitsH[1]+numBitsV[1]); 

      }

      // Fill efficiency histograms

      for (int i = 0; i < HORROADS; i++){


	if(HorRoadsSim[i]!=0){ 	   
	  MuidLvl1_HorSymEffCut[0]->SetBinContent(i + 1, (float)(HorRoads[i]) / (float)(HorRoadsSim[i]));
	  MuidLvl1_HorSymEff[0]->SetBinContent(i + 1, (float)(HorRoads[i]) / (float)(HorRoadsSim[i]));
	}
	else{
	  MuidLvl1_HorSymEff[0]->SetBinContent(i + 1, 0.0);
	  MuidLvl1_HorSymEffCut[0]->SetBinContent(i + 1, 0.0);
	}

	if(HorRoadsShalSim[i]!=0)
	  MuidLvl1_HorSymEffCutShal[0]->SetBinContent(i + 1, (float)(HorRoadsShal[i]) / (float)(HorRoadsShalSim[i]));
	else
	  MuidLvl1_HorSymEffCutShal[0]->SetBinContent(i + 1, 0.0);

	if(HorRoadsSim[i+HORROADS]>0){ 
	  MuidLvl1_HorSymEffCut[1]->SetBinContent(i + 1, (float)(HorRoads[i+HORROADS]) / (float)(HorRoadsSim[i+HORROADS]));
	  MuidLvl1_HorSymEff[1]->SetBinContent(i + 1, (float)(HorRoads[i+HORROADS]) / (float)(HorRoadsSim[i+HORROADS]));
	}
	else{
	  MuidLvl1_HorSymEff[1]->SetBinContent(i + 1, 0.0);
	  MuidLvl1_HorSymEffCut[1]->SetBinContent(i + 1, 0.0);
	}

	if (HorRoadsShalSim[i + HORROADS] != 0)
	  MuidLvl1_HorSymEffCutShal[1]->SetBinContent(i + 1, (float)(HorRoadsShal[i + HORROADS]) / (float)(HorRoadsShalSim[i + HORROADS]));
	else
	  MuidLvl1_HorSymEffCutShal[1]->SetBinContent(i + 1, 0.0);

      }

      for (int i = 0; i < VERROADS; i++){

	if(VerRoadsSim[i]!=0){ 
	  MuidLvl1_VerSymEffCut[0]->SetBinContent(i + 1, (float)(VerRoads[i]) / (float)(VerRoadsSim[i]));
	  MuidLvl1_VerSymEff[0]->SetBinContent(i + 1, (float)(VerRoads[i]) / (float)(VerRoadsSim[i]));
	}
	else{
	  MuidLvl1_VerSymEff[0]->SetBinContent(i + 1, 0.0);
	  MuidLvl1_VerSymEffCut[0]->SetBinContent(i + 1, 0.0);
	}

	if(VerRoadsShalSim[i]!=0)
	  MuidLvl1_VerSymEffCutShal[0]->SetBinContent(i + 1, (float)(VerRoadsShal[i]) / (float)(VerRoadsShalSim[i]));
	else
	  MuidLvl1_VerSymEffCutShal[0]->SetBinContent(i + 1, 0.0);

	if (VerRoadsSim[i + VERROADS] != 0){
	  MuidLvl1_VerSymEff[1]->SetBinContent(i + 1, (float)(VerRoads[i + VERROADS]) / (float)(VerRoadsSim[i + VERROADS]));
	  MuidLvl1_VerSymEffCut[1]->SetBinContent(i + 1, (float)(VerRoads[i + VERROADS]) / (float)(VerRoadsSim[i + VERROADS]));
	}
	else{
	  MuidLvl1_VerSymEff[1]->SetBinContent(i + 1, 0.0);
	  MuidLvl1_VerSymEffCut[1]->SetBinContent(i + 1, 0.0);
	}

	if (VerRoadsShalSim[i + VERROADS] != 0)
	  MuidLvl1_VerSymEffCutShal[1]->SetBinContent(i + 1, (float)(VerRoadsShal[i + VERROADS]) / (float)(VerRoadsShalSim[i + VERROADS]));
	else
	  MuidLvl1_VerSymEffCutShal[1]->SetBinContent(i + 1, 0.0);

      }

      MuidLvl1_GL1Eff[0]->SetBinContent(1, (float)OneDeepSouth / (float) OneDeepSouthSim);
      //MuidLvl1_GL1Eff[0]->SetBinContent(1, (float)OneDeepCombS / (float) OneDeepCombSimS);
      MuidLvl1_GL1Eff[0]->SetBinContent(2, (float)TwoDeepSouth / (float) TwoDeepSouthSim);
      //MuidLvl1_GL1Eff[0]->SetBinContent(2, (float)TwoDeepCombS / (float) TwoDeepCombSimS);
      //MuidLvl1_GL1Eff[0]->SetBinContent(3, (float)OneShalSouth / (float) OneShalSouthSim);
      //MuidLvl1_GL1Eff[0]->SetBinContent(4, (float)OneDeepOneShalSouth / (float) OneDeepOneShalSouthSim);
      MuidLvl1_GL1Eff[0]->SetBinContent(5, (float)OneDeepSouthGL1 / (float) OneDeepSouthSim);
      //MuidLvl1_GL1Eff[0]->SetBinContent(5, (float)OneDeepGL1 / (float) OneDeepCombSim);
      MuidLvl1_GL1Eff[1]->SetBinContent(1, (float)OneDeepNorth / (float) OneDeepNorthSim);
      //MuidLvl1_GL1Eff[1]->SetBinContent(1, (float)OneDeepCombN / (float) OneDeepCombSimN);
      MuidLvl1_GL1Eff[1]->SetBinContent(2, (float)TwoDeepNorth / (float) TwoDeepNorthSim);
      //MuidLvl1_GL1Eff[1]->SetBinContent(2, (float)TwoDeepCombN / (float) TwoDeepCombSimN);
      //MuidLvl1_GL1Eff[1]->SetBinContent(3, (float)OneShalNorth / (float) OneShalNorthSim);
      //MuidLvl1_GL1Eff[1]->SetBinContent(4, (float)OneDeepOneShalNorth / (float) OneDeepOneShalNorthSim);
      MuidLvl1_GL1Eff[1]->SetBinContent(5, (float)OneDeepNorthGL1 / (float) OneDeepNorthSim);
      //MuidLvl1_GL1Eff[1]->SetBinContent(5, (float)OneDeepGL1 / (float) OneDeepCombSim);

      MuidLvl1_GL1Eff[0]->SetBinContent(6, (float)TwoDeepSouthGL1 / (float) TwoDeepSouthSim);
      MuidLvl1_GL1Eff[1]->SetBinContent(6, (float)TwoDeepNorthGL1 / (float) TwoDeepNorthSim);

      //MuidLvl1_GL1Eff[0]->SetBinContent(6, (float)TwoDeepGL1 / (float) TwoDeepCombSimFull);
      //MuidLvl1_GL1Eff[1]->SetBinContent(6, (float)TwoDeepGL1 / (float) TwoDeepCombSimFull);

    }

    delete p;
    
  }

	MuidLvl1_InfoHist->SetBinContent(OFFLINE_INDEX,offline_version);
	MuidLvl1_InfoHist->SetBinContent(NUM_EVTS_INDEX,evtcnt);
 
  // Check to see if the DB should be updated

  OnlMonServer *se = OnlMonServer::instance();

  //if (se->CurrentTicks() > (LastCommit + 20))
  if (se->CurrentTicks() > (LastCommit + 120))
    {  
      
      cout << " Events = " << evtcnt << endl; 

      // efficiencies to DB every 2 minutes

      // diagnostics - use printfs which do not go into the message system
      
      printf("1DS Sim = %d   GL1 1DS = %d ratio = %f\n",OneDeepSouthSim,OneDeepSouth,(float)OneDeepSouth/(float)OneDeepSouthSim);
      printf("1DN Sim = %d   GL1 1DN = %d ratio = %f\n",OneDeepNorthSim,OneDeepNorth,(float)OneDeepNorth/(float)OneDeepNorthSim);
      printf("N1D ratio of reduced bits to GL1 = %f\n", (float)rbitsN1D/(float)OneDeepNorthGL1); 
      printf("S1D ratio of reduced bits to GL1 = %f\n", (float)rbitsS1D/(float)OneDeepSouthGL1); 

      printf("\n"); 
      printf("2DS Sim = %d   GL1 2DS = %d ratio = %f\n",TwoDeepSouthSim,TwoDeepSouth,(float)TwoDeepSouth/(float)TwoDeepSouthSim);
      printf("2DN Sim = %d   GL1 2DN = %d ratio = %f\n",TwoDeepNorthSim,TwoDeepNorth,(float)TwoDeepNorth/(float)TwoDeepNorthSim);      
      printf("S2D ratio of reduced bits to GL1 = %f\n", (float)rbitsS2D/(float)TwoDeepSouthGL1); 
      printf("N2D ratio of reduced bits to GL1 = %f\n", (float)rbitsN2D/(float)TwoDeepNorthGL1); 

      printf("\n"); 
      printf("S1D ratio of Sim to reduced bits = %f\n", (float)OneDeepSouthSim/(float)rbitsS1D); 
      printf("N1D ratio of Sim to reduced bits = %f\n", (float)OneDeepNorthSim/(float)rbitsN1D); 
      printf("S2D ratio of Sim to reduced bits = %f\n", (float)TwoDeepSouthSim/(float)rbitsS2D); 
      printf("N2D ratio of Sim to reduced bits = %f\n", (float)TwoDeepNorthSim/(float)rbitsN2D); 

      printf("\n"); 
      printf("S1D GL1 ratio (GL1/Sim) = %f\n", (float)OneDeepSouthGL1/(float)OneDeepSouthSim); 
      printf("N1D GL1 ratio (GL1/Sim) = %f\n", (float)OneDeepNorthGL1/(float)OneDeepNorthSim); 
      printf("S2D GL1 ratio (GL1/Sim) = %f\n", (float)TwoDeepSouthGL1/(float)TwoDeepSouthSim); 
      printf("N2D GL1 ratio (GL1/Sim) = %f\n", (float)TwoDeepNorthGL1/(float)TwoDeepNorthSim); 

      printf("\n"); 
      printf("S1D (GL1/Sim+GL1) ratio = %f\n", (float)OneDeepSouthGL1/(float)OneDeepSouth); 
      printf("N1D (GL1/Sim+GL1) ratio = %f\n", (float)OneDeepNorthGL1/(float)OneDeepNorth); 
      printf("S2D (GL1/Sim+GL1) ratio = %f\n", (float)TwoDeepSouthGL1/(float)TwoDeepSouth); 
      printf("N2D (GL1/Sim+GL1) ratio = %f\n", (float)TwoDeepNorthGL1/(float)TwoDeepNorth); 
      printf("\n"); 
      
      if ( (OneDeepSouthSim > 0) && (OneDeepNorthSim > 0) )
	//if ( (OneDeepCombSim > 0) )
        {

          ostringstream msg;
          msg << "Committing efficiencies to database, seconds since last commit = " << se->CurrentTicks() - LastCommit;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(this, MSG_SOURCE_LVL1, MSG_SEV_INFORMATIONAL, msg.str(), 1);

          float eff, eff_err;
       
          eff = (float)OneDeepSouth / (float)OneDeepSouthSim;
          eff_err = sqrt(eff * (1.0 - eff) / (float)OneDeepSouthSim);
          dbvars->SetVar("MuidLL1S1D_GL1eff", eff, eff_err, (float) evtcnt);

          eff = (float)OneDeepNorth / (float)OneDeepNorthSim;
          eff_err = sqrt(eff * (1.0 - eff) / (float)OneDeepNorthSim);
          dbvars->SetVar("MuidLL1N1D_GL1eff", eff, eff_err, (float) evtcnt);

	  /*

          eff = (float)OneDeepCombS / (float)OneDeepCombSimS;
          eff_err = sqrt(eff * (1.0 - eff) / (float)OneDeepCombSimS);
          dbvars->SetVar("MuidLL1S1D_GL1eff", eff, eff_err, (float) evtcnt);
          eff = (float)OneDeepCombN / (float)OneDeepCombSimN;
          eff_err = sqrt(eff * (1.0 - eff) / (float)OneDeepCombSimN);
          dbvars->SetVar("MuidLL1N1D_GL1eff", eff, eff_err, (float) evtcnt);

	  if(TwoDeepCombSimS > 0) {
            eff = (float)TwoDeepCombS / (float)TwoDeepCombSimS;
            eff_err = sqrt(eff * (1.0 - eff) / (float)TwoDeepCombSimS);
            dbvars->SetVar("MuidLL1S2D_GL1eff", eff, eff_err, (float) evtcnt);
	  }
	  if(TwoDeepCombSimN > 0) {
            eff = (float)TwoDeepCombN / (float)TwoDeepCombSimN;
            eff_err = sqrt(eff * (1.0 - eff) / (float)TwoDeepCombSimN);
            dbvars->SetVar("MuidLL1N2D_GL1eff", eff, eff_err, (float) evtcnt);
	  }

	  */
       

          if (TwoDeepSouthSim > 0)
            {
              eff = (float)TwoDeepSouth / (float)TwoDeepSouthSim;
              eff_err = sqrt(eff * (1.0 - eff) / (float)TwoDeepSouthSim);
              dbvars->SetVar("MuidLL1S2D_GL1eff", eff, eff_err, (float) evtcnt);
            }
          else
            dbvars->SetVar("MuidLL1S2D_GL1eff", 0.0, 0.0, (float) evtcnt);
	  	    
          if (TwoDeepNorthSim > 0)
            {
              eff = (float)TwoDeepNorth / (float)TwoDeepNorthSim;
              eff_err = sqrt(eff * (1.0 - eff) / (float)TwoDeepNorthSim);
              dbvars->SetVar("MuidLL1N2D_GL1eff", eff, eff_err, (float) evtcnt);
            }
          else
            dbvars->SetVar("MuidLL1N2D_GL1eff", 0.0, 0.0, (float) evtcnt);   

          dbvars->DBcommit();

        }

      LastCommit = se->CurrentTicks();

    }


  return 0;

}


int MuidLvl1Mon::Reset()
{

  // Reset internal counters

  evtcnt = 0;

  OneDeepSouth = 0;
  OneDeepSouthSim = 0;
  OneDeepSouthGL1 = 0; 
  OneDeepNorth = 0;
  OneDeepNorthSim = 0;
  OneDeepNorthGL1 = 0; 
  TwoDeepNorth = 0;
  TwoDeepNorthSim = 0;
  TwoDeepSouth = 0;
  TwoDeepSouthSim = 0;
  OneShalSouth = 0;
  OneShalSouthSim = 0;
  OneShalNorth = 0;
  OneShalNorthSim = 0;
  OneDeepOneShalSouth = 0;
  OneDeepOneShalSouthSim = 0;
  OneDeepOneShalNorth = 0;
  OneDeepOneShalNorthSim = 0;
  OneDeepComb = 0;
  OneDeepCombN = 0;
  OneDeepCombS = 0;
  OneDeepCombSim = 0;
  OneDeepCombSimN = 0;
  OneDeepCombSimS = 0;
  OneDeepGL1 = 0; 
  TwoDeepComb = 0;
  TwoDeepCombN = 0;
  TwoDeepCombS = 0;
  TwoDeepCombSim = 0;
  TwoDeepCombSimN = 0;
  TwoDeepCombSimS = 0;
  TwoDeepCombSimFull = 0;
  TwoDeepGL1 = 0; 
  TwoDeepNorthGL1 = 0; 
  TwoDeepSouthGL1 = 0; 

  LastCommit = 0;

  SHNumEvt = 0; 
  SVNumEvt = 0; 
  NHNumEvt = 0; 
  NVNumEvt = 0; 

  rbitsN1D = 0; 
  rbitsS1D = 0; 
  rbits1D = 0; 
  rbitsN2D = 0; 
  rbitsS2D = 0; 
  rbits2D = 0; 


  // Clear error and road counters
  memset(SYNCERR, 0, sizeof(SYNCERR));

  memset(HorRoads, 0, sizeof(HorRoads));
  memset(HorRoadsSim, 0, sizeof(HorRoadsSim));
  memset(HorRoadsShal, 0, sizeof(HorRoadsShal));
  memset(HorRoadsShalSim, 0, sizeof(HorRoadsShalSim));

  memset(VerRoads, 0, sizeof(VerRoads));
  memset(VerRoadsSim, 0, sizeof(VerRoadsSim));
  memset(VerRoadsShal, 0, sizeof(VerRoadsShal));
  memset(VerRoadsShalSim, 0, sizeof(VerRoadsShalSim));


  // COMPLETELY zero out sync error flags
  memset(SimSyncErr, 0, sizeof(SimSyncErr));

  // Set known bad fibers:
  // JGL - don't set these, these fibers seems to have a high rate of 
  // sync errors but are not *always* bad. Setting these bits will cause
  // an overestimate of the simulated triggers, and an underestimate of the 
  // efficiency. 5/14/2011
  
  /*

  // SH2 fiber bad - JGL 02/04/2010
  SimSyncErr[0][0][0][2] = 1;
  SimSyncErr[0][0][1][2] = 1;
  SimSyncErr[0][0][2][2] = 1;
  SimSyncErr[0][0][3][2] = 1;
  SimSyncErr[0][0][4][2] = 1;

  // NH5 fiber bad - JGL 12/9/2007
  SimSyncErr[1][0][0][5] = 1;
  SimSyncErr[1][0][1][5] = 1;
  SimSyncErr[1][0][2][5] = 1;
  SimSyncErr[1][0][3][5] = 1;
  SimSyncErr[1][0][4][5] = 1;

  */

  return 0;
}


int MuidLvl1Mon::DBVarInit()
{
  // variable names are not case sensitive
  string varname;
  varname = "MuidLL1S1D_GL1eff";
  dbvars->registerVar(varname);
  varname = "MuidLL1S2D_GL1eff";
  dbvars->registerVar(varname);
  varname = "MuidLL1N1D_GL1eff";
  dbvars->registerVar(varname);
  varname = "MuidLL1N2D_GL1eff";
  dbvars->registerVar(varname);
  dbvars->DBInit();
  return 0;
}
