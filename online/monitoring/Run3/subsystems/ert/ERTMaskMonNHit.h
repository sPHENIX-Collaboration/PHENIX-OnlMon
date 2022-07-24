#ifndef __ERTMASKMONNHIT_H__
#define __ERTMASKMONNHIT_H__

#include "ERTMaskMonCommon.h"

#include <TH1.h>

#include <string>

class TPad;
class TLine;
class TPaveText;
class TText;

class ERTMaskMonNHit
{
   static const double FONT_SIZE = 0.15;
   static const double LEFT_MARGIN  = 0.05;
   static const double RIGHT_MARGIN = 0.03;

   int m_bl_bad_sect;
   int m_bl_hs_gap[ERTMaskMonCommon::N_ARMSECT];
   int m_bl_sect_cold[ERTMaskMonCommon::N_HALF_SECTOR];

   ERTMaskMonCommon::Detector m_detector;
   std::string m_name;
   int m_nhot;

   TH1* m_hsm;
   TH1* m_hsm_for_draw;
   TH1* m_hsm_masked;
   TH1* m_hsm_hot;

   // painting
   TPaveText* m_pave_title;
   TLine*     m_line_sector[ERTMaskMonCommon::N_ARMSECT];
   TText*     m_label_sector;
   TText*     m_label_hot;

 public:
   ERTMaskMonNHit(const ERTMaskMonCommon::Detector det, const char* short_name);
   virtual ~ERTMaskMonNHit();

   void SetHisto(TH1* hsm) { m_hsm = hsm; }

   int  IsMasked(const int ssm);
   int  IsHot   (const int ssm);
   int  IsColdHalfSect(const int hs);
   int  HasColdHalfSect();
   void SetMask  (const int ssm, const float val);
   void SetHot   (const int ssm, const float val);
   void ResetMask();
   void ResetHot();
   void ResetColdSector();

   ERTMaskMonCommon::Detector GetDetector() { return m_detector; }

   void FindHotWithNHitSigma(const float sigma_cut);
   void FindHotWithNHitThreshold(const float threshold);
   void FindHotWithRFactor(const float rfactor_current, const float rfactor_dlimit);
   float GetAverage(const int nsm_l, const int nsm_h, 
                    const int nsm_skip_l = -1, const int nsm_skip_h = -1);
   int FindHotInRange(const float threshold, const int nsm_l, const int nsm_h, 
                      const int nsm_l_reject = -1, const int nsm_h_reject = -1);
   void FindColdSector();

   void Draw(TPad* pad, int logflag);
   int GetNHot() { return m_nhot; }

   std::string GetName() { return m_name; }
};

#endif /* __ERTMASKMONNHIT_H__ */
