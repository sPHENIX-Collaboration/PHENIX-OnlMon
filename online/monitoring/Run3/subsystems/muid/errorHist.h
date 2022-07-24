#ifndef __ERRORHIST_H__
#define __ERRORHIST_H__

#include <obj_hist.h>
#include <mui_mon_const.h>
#include <mui_hit.h>

///Histogram manager for error counting histograms.
class errorHist: public obj_hist
  {
  public:
    errorHist(OnlMon *myparent);

    virtual ~errorHist(){}

    void draw();

    void event(hit_vector rawHits);

    void clear();

    void analyze(){}

    std::string Name() {return "errorHist";}

  private:
    TH1* rocOkErrHist[MAX_ARM];
    TH1* beamOkErrHist[MAX_ARM];
    TH1* noPacketErrHist[MAX_ARM];
    TH1* deadHvChainErrHist[MAX_ARM];
    TH1* deadRocErrHist[MAX_ARM];
    TH1* deadChipErrHist[MAX_ARM];
    TH1* mainFrameErrHist[MAX_ARM];
    TH1* deadCableErrHist[MAX_ARM];
  };


#endif /* __ERRORHIST_H__ */
