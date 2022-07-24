#ifndef __OBJ_HIST_H__
#define __OBJ_HIST_H__

#include "mui_mon_const.h"
#include "mui_hit.h"

#include <OnlMon.h>
#include <string>

class OnlMon;
class TH1;
class TH2;

///Base class for the histogram managers.
class obj_hist
  {
  public:

    ///Default Constructor.
    obj_hist(OnlMon *myparent);

    ///Defatult Destructor.
    virtual ~obj_hist() {}

    ///Data is passed in each event.
    virtual void event(hit_vector rawHits) = 0;

    ///Reset the histograms.
    virtual void clear() {return;}

    ///Do in depth anaysis such as at end of run.
    virtual void analyze() = 0;

    ///Do checks on histograms each event.
    virtual void analyzeEvent()
    {
      //cout<<"Warning: obj_hist::analyzeEvent() unemplemented"<<endl;
    }
    virtual std::string Name() {return "Unknown";}

  protected:
    OnlMon *parent;

};


#endif /* __OBJ_HIST_H__ */
