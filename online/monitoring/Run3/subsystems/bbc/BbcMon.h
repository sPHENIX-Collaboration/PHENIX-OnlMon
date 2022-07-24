#ifndef __BBCMON_H__
#define __BBCMON_H__

#include "BbcMonDefs.h"
#include <OnlMon.h>

#include <ctime>
#include <string>


class BbcCalib;
class BbcEvent;
class Event;
class OnlMonDB;
class TH1;
class ZdcCalib;
class ZdcEvent;

class BBCMon: public OnlMon
{
  public:
    BBCMon(const char *name = "BbcMON");
    virtual ~BBCMon();

    int process_event(Event *evt);
    int Init() { return Reset(); };
    int Reset();
    int BeginRun(const int runno);
    int EndRun(const int runno);

  private:
    void setup_runinfo(const int runnumber=-1);
    void setup_calibration(const int runnumber=-1);
    void read_trigger_decision(const unsigned long scaledtrig=0);
    int DBVarInit();

    BbcCalib *bbccalib;
    ZdcCalib *zdccalib;
    BbcEvent *bevt;
    ZdcEvent *zevt;
    OnlMonDB *dbvars;

    // Trigger Definition -- from DB.
    // bbll1[] contains a list of all of the bits for BBLL1 trigger copies, 
    // present and past
    int              numbbll1;
    unsigned long    bbll1_trig[32];
    bool             bbll1_ok;
    float            bbll1_scale_factor_buf[32];
    float            bbll1_scale_factor;

    unsigned long    laser_trig;
    bool             laser_ok;

    unsigned long    zdc_trig;
    bool             zdc_ok;
    float            zdc_scale_factor;
    float            zdc_scale_adjust;

    unsigned long    bbll1_novtx_trig;
    bool             bbll1_novtx_ok;
    float            bbll1_novtx_scale_factor;

    unsigned long    bbll1_narrowvtx_trig;
    bool             bbll1_narrowvtx_ok;
    float            bbll1_narrowvtx_scale_factor;

    time_t           bbc_start_time;

    unsigned int     evtcnt;

    TH1 *bbc_adc;
    TH1 *bbc_tdc[nTDC];
    TH1 *bbc_tdc_overflow[nTDC];
    TH1 *bbc_tdc_overflow_each[nTDC][nPMT_BBC];
    TH1 *bbc_nhit[nTRIGGER];

    TH1 *bbc_tdc_armhittime;
    TH1 *bbc_nevent_counter;

    TH1 *bbc_zvertex;
    TH1 *bbc_zvertex_bbll1;
    TH1 *bbc_zvertex_zdc;
    TH1 *bbc_zvertex_zdc_scale3;
    TH1 *bbc_zvertex_bbll1_novtx;
    TH1 *bbc_zvertex_bbll1_narrowvtx;//Run11 pp
    TH1 *bbc_zvertex_bbll1_zdc;

    TH1 *bbc_tzero_zvtx;
    TH1 *bbc_avr_hittime;
    TH1 *bbc_south_hittime;
    TH1 *bbc_north_hittime;
    TH1 *bbc_south_chargesum;
    TH1 *bbc_north_chargesum;
    TH1* bbc_prescale_hist; 
};

#endif /* __BBCMON_H__ */
