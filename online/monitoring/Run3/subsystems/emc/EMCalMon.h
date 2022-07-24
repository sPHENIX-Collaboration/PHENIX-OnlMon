#ifndef __EMCalMON_H__
#define __EMCalMON_H__

#include <OnlMon.h>

class Analys;
class Event;
class OnlMonDB;
class RunDBodbc;
class TH1;

class EMCalMon: public OnlMon 
{
 public:
   EMCalMon(const char *calibData="");
   virtual ~EMCalMon();

   void SetVerbose(bool verb = false) { fVerbose = verb; }

   int  process_event(Event *evt);
   int  Init();
   int  Reset();
   int  EMCalMonInit ();

   int BeginRun(const int runno);
   int EndRun(const int runno);

   // protected:
 private:  
   int DBVarInit();
   int DBSetVar(const char *varname);

   int EmcDetector(int ip,int ch,const char *what);
   int EmcTrig(Event *e);
   unsigned igray(unsigned n, int is);
   int EmcCheck(Event *e);
   int OneCheck(int id,int test,const char *what,int msg_source, int msg_sev);
   int AbsCheck(int id,int test,const char *what,int msg_source, int msg_sev);
   void  avAmplReset(int emcal); //0-All, 1-pbsc, 2-pbgl reset   
     
   void savepedEntry(const int runno, const char* fileName);
   Analys *mAnalys;   
   OnlMonDB *dbvars; 
   RunDBodbc *rd;
       
   int PbglRefId[2][576];    //144*4
   int PbscRefId[2][432]; //144*3
 
   int Check_evt[100];
   int Check_old[100];
   int Check_new[100];
   int Check_Packet[180];   
   
   int IPP;
   int ICH;
   int ievt;
   int RefEvt[3];
   int evt_tmp;

   int N_Pbsc_ref;
   int N_Pbgl_ref;
   int N_Pbsc_ph;
   int N_Pbgl_ph;

   int T_Pbsc_ref;
   int T_Pbgl_ref;
   int T_Pbsc_ph;
   int T_Pbgl_ph;   

   // saved between runs (not registered so no reset)
   TH1 *Refer_PBGL_orig;
   TH1 *ReferPin_PBGL_orig;
   TH1 *Refer_T_PBGL_orig;
   TH1 *ReferPin_T_PBGL_orig;

   TH1 *Refer_PBSC_orig;
   TH1 *ReferPin_PBSC_orig;
   TH1 *Refer_T_PBSC_orig;
   TH1 *ReferPin_T_PBSC_orig;
   TH1 *Refer_TP_PBSC_orig;
   TH1 *Refer_T_TP_PBSC_orig;

   // histos registered with server
   // PbGl:
   TH1 *avPbGl_REF;
   TH1 *avPbGl_SM;
   TH1 *avPbGl_T_REF;
   TH1 *avPbGl_T_SM;
   TH1 *dispPbGl_REF;
   TH1 *dispPbGl_SM;
   TH1 *dispPbGl_T_SM;
   TH1 *PbGl_ref;
   TH1 *PbGl_sm;
   TH1 *PbGl_T_ref;
   TH1 *PbGl_T_sm;
   TH1 *Refer_PBGL;
   // PbSc
   TH1 *avPbSc_REF;
   TH1 *avPbSc_SM;
   TH1 *avPbSc_T_REF;
   TH1 *avPbSc_T_SM;
   TH1 *dispPbSc_REF; 
   TH1 *dispPbSc_SM;
   TH1 *dispPbSc_T_SM;
   TH1 *PbSc_ref;
   TH1 *PbSc_sm;
   TH1 *PbSc_T_ref;
   TH1 *PbSc_T_sm;
   TH1 *PbSc_T_TP;
   TH1 *PbSc_TP;
   // Reference histos
   TH1 *ReferPin_PBGL;
   TH1 *Refer_T_PBGL;
   TH1 *ReferPin_T_PBGL;
   TH1 *Refer_PBSC;
   TH1 *ReferPin_PBSC;
   TH1 *Refer_T_PBSC;
   TH1 *ReferPin_T_PBSC;
   TH1 *Refer_TP_PBSC;
   TH1 *Refer_T_TP_PBSC;  
    
   TH1 *EMCal_Info;
   TH1 *emc_etot;
   TH1 *pihist[2][10];
   
   int   entrPbSc[15552];
   float avADCPbSc[15552];
   float avTACPbSc[15552];   //108*144 
   float av2ADCPbSc[15552];
   float av2TACPbSc[15552];
   float dispADCPbSc[15552];
   float dispTACPbSc[15552];  
   int   fStatResetPbSc[15552];

   int   entrPbScRef[108];
   float avADCPbScRef[108];
   float avTACPbScRef[108]; 
   float av2ADCPbScRef[108];
   float dispADCPbScRef[108];

   //64*144
   int   entrPbGl[9216];
   float avADCPbGl[9216];
   float avTACPbGl[9216]; 
   float av2ADCPbGl[9216];
   float av2TACPbGl[9216];
   float dispADCPbGl[9216];
   float dispTACPbGl[9216]; 
   int   fStatResetPbGl[9216];

   int entrPbGlRef[384];
   float avADCPbGlRef[384];
   float avTACPbGlRef[384];
   float av2ADCPbGlRef[384];
   float dispADCPbGlRef[384];
   unsigned int liveTrig;
   unsigned int scaledTrig;
   unsigned int bit_used;
   bool fVerbose;
 
   int pedallEntry;
   int  pedPbScEntryLG[15552];
   float   avpedPbScLG[15552];
   float  av2pedPbScLG[15552]; 
   float disppedPbScLG[15552];
   int  pedPbScEntryHG[15552];
   float   avpedPbScHG[15552];
   float  av2pedPbScHG[15552]; 
   float disppedPbScHG[15552];
      
};

#endif /*__EMCalMON_H__ */


















































