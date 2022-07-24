#ifndef __ERTMASKMONHISTO_H__
#define __ERTMASKMONHISTO_H__

#include "ERTMaskMonCommon.h"

class OnlMon;
class TH1;
class TH2;

using namespace ERTMaskMonCommon;

class ERTMaskMonHisto
{
 protected:
   static const int N_NHIT_HIST = 5; // 4x4a, 4x4b, 4x4c, 2x2, RICH for 0 to 4.

 public:
   ERTMaskMonHisto();
   virtual ~ERTMaskMonHisto();
   
 protected:
   TH1 *m_Evtcount;
   TH1 *m_hsm[N_NHIT_HIST];

   void RegisterHistos(OnlMon *myparent);
   int  GetHistos();
};

#endif /* __ERTMASKMONHISTO_H__ */
