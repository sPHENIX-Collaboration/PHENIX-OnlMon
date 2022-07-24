#ifndef __ORIGHIST_H__
#define __ORIGHIST_H__

#include "obj_hist.h"
#include "mui_mon_const.h"
#include "mui_hit.h"

class OnlMon;

/**Histogram manager for many basic histograms.
 *These should probably be broken into more managers.
 */
class origHist: public obj_hist
  {
  public:
    origHist(OnlMon *myparent);

    virtual ~origHist(){}

    void draw();

    void event(hit_vector rawHits);

    void clear();

    void analyze(){}


    void analyzeEvent();

    std::string Name() {return "origHist";}

  private:
    TH1* num_event;
    
    ///FEM raw hit histograms. MUI_(S-N)(H-V)A
    TH1* all[MAX_FEM];
    TH1* bad[MAX_FEM];

    ///ROC raw hit histograms. MUI_(S-N)(H-V)(plane)(roc)
    TH1* roc[MAX_FEM][MAX_ROC];
    TH2* roc2d[MAX_FEM];

    TH1* cable[MAX_FEM];
    TH1* prevcable[MAX_FEM];
    TH1* cable_rate[MAX_FEM];
    TH1* roc_totals[MAX_FEM];
    TH1* plane_hist[MAX_ARM];
    TH1* panel_hist[MAX_ARM];
    TH1* mainFrame[MAX_ARM];
    TH1* Num_Hits;

  };


#endif /* __ORIGHIST_H__ */
