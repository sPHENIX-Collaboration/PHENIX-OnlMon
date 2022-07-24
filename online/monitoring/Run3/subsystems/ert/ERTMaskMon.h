#ifndef __ERTMASKMON_H__
#define __ERTMASKMON_H__

#include "OnlMon.h"
#include "ERTMaskMonCommon.h"
#include "ERTMaskMonHisto.h"

class Event;
class Packet;
class TH1;
class TH2;
class TTree;
class EMCalRichDecodeOnline;

using namespace ERTMaskMonCommon;

class ERTMaskMon: public OnlMon, public ERTMaskMonHisto
{
 public:
   ERTMaskMon(const char *name = "ERTMaskMon");
   virtual ~ERTMaskMon();

   int process_event(Event *evt);
   int Reset() {return 0;}
   int BeginRun(const int runno);
   
 protected:
   void SetTrigSel(void);
   void MakeTree();
   
  // trigger and crossing selection
   int m_trig_sel;
   int m_cross_sel;
   int m_bl_tree_output; // Boolean to enable/disable tree output.

   // Bit masks set in BeginRun function.
   unsigned int m_CLOCK; 
   unsigned int m_BBC;
   unsigned int m_UP;
   unsigned int m_BBCNVtx;
   unsigned int m_BBCNrw;
   unsigned int m_ZDCw;
   unsigned int m_ZDCn;
   unsigned int m_ZDCNoS;
   //unsigned int m_BBCNVtxZDCNoS;

   unsigned int m_2x2;
   unsigned int m_4x4a;
   unsigned int m_4x4b;
   unsigned int m_4x4c;
   unsigned int m_Ele;

   unsigned int m_2x2BBC;
   unsigned int m_4x4aBBC;
   unsigned int m_4x4bBBC;
   unsigned int m_4x4cBBC;
   unsigned int m_EleBBC;
   
    
   // variables for tree
   TTree* m_tree; // Output tree for expert analysis.
   unsigned int b_evt;
   int b_xing;
   unsigned int b_tb_live;
   unsigned int b_tb_scaled;
   int b_n_4x4a, b_n_4x4b, b_n_4x4c, b_n_2x2;
   int b_as_4x4a[N_SM_EMC_TOTAL];
   int b_as_4x4b[N_SM_EMC_TOTAL];
   int b_as_4x4c[N_SM_EMC_TOTAL];
   int b_as_2x2 [N_SM_EMC_TOTAL];
   int b_sm_4x4a[N_SM_EMC_TOTAL];
   int b_sm_4x4b[N_SM_EMC_TOTAL];
   int b_sm_4x4c[N_SM_EMC_TOTAL];
   int b_sm_2x2 [N_SM_EMC_TOTAL];
   int b_ssm_4x4a[N_SM_EMC_TOTAL];
   int b_ssm_4x4b[N_SM_EMC_TOTAL];
   int b_ssm_4x4c[N_SM_EMC_TOTAL];
   int b_ssm_2x2 [N_SM_EMC_TOTAL];

   EMCalRichDecodeOnline *m_DECODE;
};

#endif /* __ERTMASKMON_H__ */
