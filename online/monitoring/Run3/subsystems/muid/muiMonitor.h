#ifndef __MUIMONITOR_H__
#define __MUIMONITOR_H__

#include <mui_hit.h>

#include <OnlMon.h>
#include <mui_mon_const.h>

#include <vector>

class Event;
class muiOptions;
class muiMap;
class obj_hist;
class corHist;
class gl1Obj;
class gl1pObj;
class TH1;
#ifdef MUID_USE_BLT
class triggerHists;
#endif
/**
 * Top level class for muid monitor server side.
 *
 * This is what the framework knows about.
 */
class muiMonitor: public OnlMon
  {
  public:
    
    ///Default Constructor.
    muiMonitor();

    //Default Destructor.
    virtual ~muiMonitor();

    /**
     *Returns a pointer to the option object.
     *This allows setting various options, such as database usage.
     */
    muiOptions* getOptions();

    ///Reset the histograms.
    int clear_hist();

    ///Examine the histograms at end of run. Not currently used.
    void analyze_hist();

    ///Examine the histograms each event.
    void analyze_event();

    ///Retrun number of events which have been looked at so far.
    int get_eventNum();

    ///This is called from framework event loop. Event is passed in to be analyzed.
    int process_event( Event * e);

    ///Prints a few statistics to stdout at end of run.
    void run_summary();

    ///Prints a summary of errors to stdout at end of run.
    void error_summary();

    int BeginRun(const int runnumber);
    int EndRun(const int runnumber);
    int Init();
    int Reset() {return 0;}

  private:

   ///Processes data events which meet our cuts.
    void process_data( Event * e);
    
    ///Unpacks the muid data. Does basic checks on user words.
    int fill_var( Event* e );
    
    ///Pass data to histograms.
    int fill_hist();

    ///Get reference data
    int getReferences();
    
    ///Pointer to options object.
    muiOptions* opts;
    
    ///Pointer to hardware to softwoare map object.
    muiMap* hwMap;

    ///Total number of hits in muid so far.
    long total_hits;

    ///Array of ROC data words. Carried over from last run. Should be removed.
    long data_words[MAX_FEM][WORD_PER_FEM];

    ///The histogram managers.
    std::vector<obj_hist*> hist_managers;

    ///A vector of the raw hits for the current event.
    hit_vector rawHits;

    ///Data and histogram manager for GL1.
    gl1Obj* gl1;

    ///Data and histogram manager for GL1P.
    gl1pObj* gl1p;

#ifdef MUID_USE_BLT
    ///Data and histogram manager for blue logic trigger.
    triggerHists* trig;
#endif

    // Reference histograms
    ///Hits per ROC reference histogram.
    TH1* ROCtotalREF_orig[MAX_ARM];
    
    ///Hits per HV chain reference histogram.
    TH1* HVHitsREF_orig[MAX_ARM][MAX_ORIENTATION];

    ///Hits per signal cable reference histogram.
    TH1* CableHitsREF_orig[MAX_ARM][MAX_ORIENTATION];

  };

#endif /* __MUIMONITOR_H__ */
