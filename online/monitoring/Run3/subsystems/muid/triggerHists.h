#ifndef __TRIGGERHISTS_H__
#define __TRIGGERHISTS_H__

#include "mui_mon_const.h"
#include "mui_hit.h"
#include <obj_hist.h>

class OnlMon;
class TH1;
class muid_blt;

///Histogram manager for BLT trigger efficiency histograms.
class triggerHists: public obj_hist
  {
  public:

    triggerHists(OnlMon *myparent);

    virtual ~triggerHists();

    int event(hit_vector rawHits, int liveTrigBits, int scaledTrigBits);

    void clear();

    void analyze() {return;}

   private:

    TH1* bltTrigEff;
    TH1* bltTrigEfe;
    TH1* bltTrigEfn;
    TH1* bltTrigEfd;
    
    TH1* bltQuadEff;    
    TH1* bltTrigEff0;
    TH1* bltTrigEff1;
    TH1* bltTrigEff2;
    TH1* bltTrigEff3;
    TH1* bltTrigEff4;
    TH1* bltTrigEff5;
    TH1* bltTrigEff6;
    TH1* bltTrigEff7;
    TH1* bltTrigEffa0;
    TH1* bltTrigEffa1;
    TH1* bltTrigEffa2;
    TH1* bltTrigEffa3;
    TH1* bltTrigEffa4;
    TH1* bltTrigEffa5;
    TH1* bltTrigEffa6;
    TH1* bltTrigEffa7;
    muid_blt* triggerEmu;

  };

#endif /* __TRIGGERHISTS_H__ */
