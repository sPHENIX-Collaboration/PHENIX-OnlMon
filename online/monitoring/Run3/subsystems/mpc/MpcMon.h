#ifndef __MpcMON_H__
#define __MpcMON_H__

#include <OnlMon.h>
#include <string>


class Event;
class TH1;
class TH2;
class BbcCalib;
class BbcEvent;
class MpcCalib;
class MpcMap;

#define NSAMPLES 12
#define NCHAN_PER_MOD 48

class MpcMon: public OnlMon
{
 public:
  MpcMon(const char *name = "MPCMON");
  virtual ~MpcMon();

  // inherited from OnlMon
  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset() {return 0;}

  int setup_calibration(const int runno);
 
 private:
  //for BBC coincidence histograms
  BbcCalib *bbccalib;
  BbcEvent *bevt;
  MpcCalib *mpccalib;
  MpcMap   *mpcmap;

  unsigned int n_trig_mpc_inconsistent[2];

  static const int NMPCTRIGS = 5;	// Max number of MPC trigs
  unsigned int n_trig_bbc[NMPCTRIGS];
  unsigned int n_trig_mpc[NMPCTRIGS];
  unsigned int n_trig_mpcany;
  unsigned int n_trig_led;
  int gridx[576];
  int gridy[576];
  int online_ch[576];
  float tdc2ns[576];
  float slewcor0[576];
  float slewcor1[576];
  float t0[576];
  float mpc_zvtx;

 protected:

  int nevents;

  unsigned int BBCTRIG0_scaledown;
  unsigned int BBCTRIG1_scaledown;
  unsigned int BBCTRIG2_scaledown;
  unsigned int LEDTRIG_scaledown;
  unsigned int MPCTRIG_scaledown[NMPCTRIGS];
  unsigned int MPCTRIGANY_scaledown;

  TH1 *mpc_scaledowns;

  std::string BBCTRIG0_name;
  std::string BBCTRIG1_name;
  std::string BBCTRIG2_name;
  std::string LEDTRIG_name;
  std::string MPCTRIG0_name;
  std::string MPCTRIG1_name;
  std::string MPCTRIG1s_name; // we added this for split triggers
  std::string MPCTRIG1n_name;
  std::string MPCTRIG2_name;
  std::string MPCTRIG2s_name;
  std::string MPCTRIG2n_name;
  std::string MPCTRIG3_name;
  std::string MPCTRIG3s_name;
  std::string MPCTRIG3n_name;
  std::string MPCTRIG4_name;
  std::string MPCTRIGANY_name;

  static const int BBCTRIG0 = 1; //"BBCLL1(>0 tubes)"
  static const int BBCTRIG1 = 2; //"BBCLL1(>0 tubes) novertex"
  static const int BBCTRIG2 = 3; //"BBCLL1(narrow)"
  static const int LEDTRIG  = 4; //"PPG(Laser)"
  static const int MPCTRIG0 = 5; //"MPC_A
  static const int MPCTRIG1 = 6; //"MPC_B
  static const int MPCTRIG2 = 7; //"MPCC&C same side
  static const int MPCTRIG3 = 8; //"MPCC_ERT2 
  static const int MPCTRIG4 = 9; //"MPCC&C opp side
  static const int MPCTRIGANY = 10;
  
  // Histograms

  //  1st TCanvas ******************************************8
  TH2 *mpcmon_hist2d;		// channel vs energy
  TH2 *mpcmon_mpctrig_2d;	// channel vs energy

  TH1 *mpc_bbctrig_ehist;	// spectrum for bbc triggers
  TH1 *mpc_ehist[NMPCTRIGS];		// spectrum for 4 mpc triggers

  TH1 *mpc_bbctrig_ehist_n;	// spectrum for bbc triggers
  TH1 *mpc_ehist_n[NMPCTRIGS];		// spectrum for 4 mpc triggers
  
  TH1 *mpc_bbctrig_ehist_s;	// spectrum for bbc triggers
  TH1 *mpc_ehist_s[NMPCTRIGS];		// spectrum for 4 mpc triggers

  TH1 *mpc_rbits;
  TH1 *mpc_rbits_consistency;

  TH1 *mpc_trig_crossing[NMPCTRIGS][2]; // [MPC Trigger][Arm]

  /////////////////////////////////////////////

  TH2 *mpcledhist2d;		// channel vs energy, LED events

  TH1 *mpc_nevents;
  TH1 *smpc_bbctrig_occupancy;   // number of towers lit up in sbbc triggered event
  TH1 *nmpc_bbctrig_occupancy;   // number of towers lit up in nbbc triggered event
  TH2 *mpc_ch_timedist;		// tdc distribution by channel
  TH1 *smpc_timedist;		// hit time distribution
  TH1 *nmpc_timedist;		// hit time distribution
  TH2 *mpc_slewdist;		// slew distribution
  TH2 *smpc_crystal[NMPCTRIGS];		// energy in each crystal, MPC 4x4a events
  TH2 *smpc_bbctrig_crystal_energy; // energy in each crystal, BBC events
  TH2 *smpc_bbctrig_crystal_count; // number of times each south crystal is occupied, BBC events
  TH2 *smpc_led_crystal_energy;	// energy in each crystal, LED events
  TH2 *nmpc_crystal[NMPCTRIGS];		// energy in each crystal, MPC 4x4a events
  TH2 *nmpc_bbctrig_crystal_energy; // energy in each crystal, BBC events
  TH2 *nmpc_bbctrig_crystal_count; // number of times each north crystal is occupied, BBC events
  TH2 *nmpc_led_crystal_energy;	// energy in each crystal, LED events
  TH2 *mpcvsbbcst0;             //south mpc vs bbc t0
  TH2 *mpcvsbbcnt0;             //north mpc vs bbc t0
  TH2 *mpcvsbbcvtx;             //mpc vs bbc vertex
  TH2 *mpcvsbbct0;              //mpc vs bbc event t0
  
  TH1 *mpc_vtx;  //vtx recontruction

  TH1 *mpc_triginfo;		// number of triggers
};

#endif /* __MpcMON_H__ */

