#ifndef __MUI_MONGLOBALS_H__
#define __MUI_MONGLOBALS_H__

#include <mui_mon_const.h>

///Singleton class which holds "global" variables for the server side.
class muiMonGlobals
  {


  public:

    ///Get the pointer to THE instance of muiMonGlobals
    static muiMonGlobals* Instance();

    ///Default destructor
    virtual ~muiMonGlobals() {}

    ///Clear the counters.
    void ClearRun();

    ///Verbosity value.
    int verbose;

    ///Arm active selection
    bool arm_active[MAX_ARM];

    ///Number of selected events.
    long eventNum;

    ///Total number of events.
    long eventNumAll;

    ///Number of events not analyzed.
    long eventNumRejected;

    ///How often to check for dead main frames.
    int dead_mainFrame_freq;

    ///How often to the cable histograms.
    int cable_hist_update_freq;

    ///Number of ROC OK errors.
    int rocOkErrors[MAX_ARM];

    ///Number of BEAM OK errors.
    int beamOkErrors[MAX_ARM];
    
    ///Number of events which contained no muid data packet.
    int noPacketErrors[MAX_ARM];

    ///Number of dead HV chain errors. Not used.
    int deadHvChainErrors[MAX_ARM];

    ///Number of dead ROC errors. Not used.
    int deadRocErrors[MAX_ARM];
    
    ///Number of dead ROC chip errors. Not used.
    int deadChipErrors[MAX_ARM];

    ///Number of dead HV mainframe errors.
    int mainFrameErrors[MAX_ARM];

    ///Number of dead signal cable errors.
    int deadCableErrors[MAX_ARM];

  private:
    ///Default constructor.
    muiMonGlobals();
    ///Pointer to the single instance.
    static muiMonGlobals* pmuiMonGlobals;

};



#endif /* __MUI_MONGLOBALS_H__ */
