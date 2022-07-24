#ifndef __GL1POBJ_H__
#define __GL1POBJ_H__

#include <mui_mon_const.h>
#include <mui_hit.h>

#include <string>

class Event;
class TH1;
class TH2;

///Histogram/data manager for GL1P.
class gl1pObj
  {
  public:

    gl1pObj();

    virtual ~gl1pObj();

    int event( Event* e, const long crossctr);

    void clear();

    int analyze() {return 0;}

  private:

    TH1* GL1P_CrossCtr[ MAX_GL1P_SCALERS ];
    TH1* GL1P_Scaler[ MAX_GL1P_SCALERS ];
    TH2* GL1_GL1P[ MAX_GL1P_SCALERS ];

  };

#endif /* __GL1POBJ_H__ */
