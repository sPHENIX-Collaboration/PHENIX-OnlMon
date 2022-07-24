#ifndef __SVXSTRIPMON_H__
#define __SVXSTRIPMON_H__


#include "SvxStripMonDefs.h"
#include "SvxStripLadderMap.h"

#include <OnlMon.h>
#include <string>

class Event;
class OnlMonDB;
class TH1;
class TH2;
class TH3;

class SvxStripMon: public OnlMon
{
public:
    SvxStripMon(const char *name = "SVXSTRIPMON");
    virtual ~SvxStripMon();

    int process_event(Event *evt);
    int Init();
    int BeginRun(const int runno);
    int Reset();

protected:
    int evtcnt;
    int idummy;

    int gl1bclock_offset[VTXS_NPACKET];

    //BbcCalib *bbccalib;
    //BbcEvent *bevt;
    int read_config_file(int data[2][24][6], const std::string &fname);
    int read_config_file(int data[2][24][6][12], const std::string &fname);
    int ReadFeedConfig();

    static const int ny = 256;

    TH2 *h2adc[VTXS_NPACKET][NRCC];
    TH2 *h2sadc;
    TH2 *h2live;
    TH1 *vtxs_pars;
    TH1 *hsadc_bytime;    // track adc sum above threshold with time
    TH1 *hsadc_bytime_nohot;    // track adc sum above threshold with time
    TH1 *hnbbc_bytime;    // num bbc with time
    TH1 *hmulti;          // detector multiplicity

    TH3 *h1rcc;        // normalized yield

    time_t bor_time;                // beginning of run unix time stamp
    TH1 *hcelltmp[VTXS_NPACKET][NRCC]; //only used internally
    TH3 *hcellpro;
    TH3 *hcellid;
    TH2 *hbclock;
    TH3 *hrccbclock;
    TH2 *hrccbclockerror;
    TH2 *hpacnt;
    TH1 *htimeouterr;
    TH3 *hhybridmask;
    TH2 *hknownmask;

    long _timeout_counter[VTXS_NPACKET];

    int  ig_hot[4][24][6];
    int ig_dead[4][24][6];
    int ig_hotchip[4][24][6][12];

    SvxStripLadderMap ladder_map;
};

#endif /* __SVXSTRIPMON_H__ */

