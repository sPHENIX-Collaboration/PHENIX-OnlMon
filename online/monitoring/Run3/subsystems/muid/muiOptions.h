#ifndef __MUIOPTIONS_H__
#define __MUIOPTIONS_H__
#include "mui_mon_const.h"

///Object which holds option paramaters for muid monitoring.
class muiOptions
  { //++CINT
  public:
    
    ///Default Contructor.
    muiOptions();

    ///Default Destructor.
    ~muiOptions();

    ///Set Verbosity value. 0 low 10 high.
    void setVerbose(int verb);

    ///Get current verbosity value.
    int getVerbose();

    ///Set arm active value. false = off, true = on
    void setArmActive(int arm, bool b);

    ///Get current arm active value. false = off, true = on
    bool getArmActive(int arm);    

    ///Should histograms be cleared between runs.
    void setAutoClearHists(bool aut);

    ///Get current histogram clearing setting.
    bool getAutoClearHists();

    /**
     *Set the trigger selection. -1 is no selection. 
     *Should be broken into live and scaled?
     */
    void set_trigger(long trigBits);

    ///Prints some help for set_trigger.
    void set_trigger();

    ///Get current trigger selections.
    long get_trigger();

    ///Should histograms be to file saved automatically. Not used.
    void auto_save(short i);

    ///Should map be read from database.
    void setUseDB(bool useDB);

    ///Get current setting for reading from database.
    bool useDB();

    ///Skip events with too large hit multiplicity
    void set_max_mult(int i) { max_mult = i; }
    int get_max_mult() { return max_mult; }
    ///or too small
    void set_min_mult(int i) { min_mult = i; }
    int get_min_mult() { return min_mult; }

  private:

    ///Verbosity setting.
    int verbose;

    ///Arm active selection
    bool arm_active[MAX_ARM];

    ///Automatically clear histograms setting.
    bool auto_histo_clear;

    ///Trigger selection.
    long livetrig_accept;

    ///Automatically save histograms setting. Not used.
    short auto_histo_save;

    ///Read from database setting.
    bool fill_from_db;

    ///Skip events with too large, or too low, hit multiplicity
    int max_mult;
    int min_mult;
  };

#endif /* __MUIOPTIONS_H__ */
