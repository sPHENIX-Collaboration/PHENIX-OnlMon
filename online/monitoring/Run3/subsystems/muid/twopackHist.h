#ifndef __TWOPACKHIST_H__
#define __TWOPACKHIST_H__

#include <obj_hist.h>
#include <mui_mon_const.h>
#include <mui_hit.h>

///Histogram manager for twopack and HV chain hit distributions.
class twopackHist: public obj_hist
  {
  public:
    twopackHist(OnlMon *myparent);

    virtual ~twopackHist() {}

    void event(hit_vector rawHits);

    void clear() {}

    void analyze(){}

    std::string Name() {return "twopackHist";}

  private:
    TH1* twopack[MAX_ARM][GAP_PER_ARM][PANEL_PER_GAP][MAX_ORIENTATION];
    TH1* HVHits[MAX_ARM][MAX_ORIENTATION];
  };


#endif /* __TWOPACKHIST_H__ */
