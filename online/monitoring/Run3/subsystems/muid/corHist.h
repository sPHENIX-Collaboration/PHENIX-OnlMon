#ifndef __CORHIST_H__
#define __CORHIST_H__

#include <obj_hist.h>
#include <mui_mon_const.h>
#include <mui_hit.h>

class OnlMon;
/**
 *Histogram manager for correlation histograms.
 *These show when a twopack in one gap fired, where did a twopack fire in the next.
*/
class corHist: public obj_hist
  {
  public:
    corHist(OnlMon *myparent);

    virtual ~corHist(){}

    void draw();

    void event(hit_vector rawHits);

    void clear();

    void analyze(){}

    std::string Name() {return "corHist";}

  private:
    TH2* cor[MAX_ARM][MAX_ORIENTATION][GAP_PER_ARM - 1][PANEL_PER_GAP];
    
  };


#endif /* __CORHIST_H__ */
