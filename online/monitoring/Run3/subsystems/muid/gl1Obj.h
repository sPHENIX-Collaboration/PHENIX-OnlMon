#ifndef __GL1OBJ_H__
#define __GL1OBJ_H__

#include <mui_mon_const.h>
#include <mui_hit.h>
#include <obj_hist.h>

class TH1;
class TH2;

class Event;
class OnlMon;

///Histogram/data manager for GL1.
class gl1Obj: public obj_hist
  {
  public:

    gl1Obj(OnlMon *myparent);

    virtual ~gl1Obj();

    int event( Event* e);
    void event(hit_vector rawHits) {return;}
    int fill(hit_vector rawHits);

    int setTrigMask(unsigned int trigmask);

    void clear();

    void analyze() {return;}

    long getScaledtrig();

    long getLivetrig();

    long getCrossctr();

    long getReducedbits0();

    long getReducedbits1();


  private:

    long scaledtrig;
    long livetrig;
    long crossctr;
    long reducedbits0;
    long reducedbits1;

    TH1* TrigMask;
    TH1* ScaledTrig;
    TH1* LiveTrig;
    TH2* ScaledTrig_TotalHits;
    TH1* CrossCtr;
    TH2* ScaledTrig_CrossCtr;
    TH2* CrossCtr_TotalHits;
    TH1* ReducedBits0;
    TH1* ReducedBits1;
    TH2* ReducedBits0_CrossCtr;

  };

#endif /* __GL1OBJ_H__ */
