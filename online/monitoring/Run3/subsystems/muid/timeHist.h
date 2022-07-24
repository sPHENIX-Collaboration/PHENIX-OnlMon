#ifndef __TIMEHIST_H__
#define __TIMEHIST_H__

#include <obj_hist.h>
#include <mui_mon_const.h>
#include <mui_hit.h>

class OnlMon;
class TH2;

///Histogram manager for time dependence of hits histogram.
class timeHist: public obj_hist
  {
  public:
    timeHist(OnlMon *myparent);

    virtual ~timeHist(){}

    void draw();

    void event(hit_vector rawHits);

    void clear();

    void analyze(){}

    std::string Name() {return "timeHist";}

  private:
    TH2* total_hits_timedep;
    static const int events_accumulate = 10;
    static const int max_timebin = 1000;
    int clear_overflow; // histograms are cleared when the time bin reaches maximum

};


#endif /* __TIMEHIST_H__ */
