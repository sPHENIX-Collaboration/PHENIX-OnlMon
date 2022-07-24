#ifndef __MUTRMON_H__
#define __MUTRMON_H__

#include <map>
#include <set>
#include <vector>
#include <string>

#include <OnlMon.h>

#include <MutOnlineChannel.hh>
#include <MutrMonConsts.h>

class TH1;
class TProfile;

typedef std::map<int, MutOnlineChannel> map_type;
typedef map_type::iterator map_iterator;

// for the clustering, we'll use a set for the strips
typedef struct 
{ 
  int oct;
  int hoct;
  int pla;
  int strip;
  float adc;
} MutOnlineStrip;

typedef struct 
{ // cluster = a continuous set of strips
  int oct;
  int hoct;
  int pla;
  int laststrip; 
  int nstrips; 
  float adcsum;
  int peakstrip;
  float peakadc;
} MutOnlineClust;

class OnlineOrderFunc
{
 public:
  bool operator() (const MutOnlineStrip p1, const MutOnlineStrip p2) const 
  { 
    // return 1 if ptr1 should be before ptr2, else 0
    // this will lead to a sorting in order oct, hoct, gap, strip 
    return ( p1.oct < p2.oct ||
	     (p1.oct == p2.oct && (p1.hoct < p2.hoct ||
				   (p1.hoct == p2.hoct && (p1.pla < p2.pla ||
							   (p1.pla == p2.pla && p1.strip < p2.strip))))) );
  } 
};
typedef std::set< MutOnlineStrip, OnlineOrderFunc > onlineset_type;
typedef onlineset_type::iterator onlineset_iterator;

class Event;

class MutrMon: public OnlMon
{
 public:
  MutrMon();
  virtual ~MutrMon(){}

  // required
  int process_event(Event *evt);
  int BeginRun(const int runno);

  // we only need to reset the histograms which is done
  // by the server framework
  int MutrInit(); 
  // 
  int initHistos();
  int initMapping();
  // get mapping from Objy - not txt files
  int initMappingObjy(const int year = 2004, const int month = 6, 
		      const int day = 30, const int hour = 22,
		      const int min = 59, const int sec = 0); 

  int txtGetMap(const int iarm = 0, const int istation = 0, 
		const char* infile = "a1_s1_map.dat");
  int txtGetFullMap(const int iarm = 0, 
		    const char* infile = "mapsouth.dat");
  int txtPutMap(const char* outfile = "map.dat");
  int getCalibrations(const int year = 2004, const int month = 6, 
		      const int day = 30, const int hour = 22,
		      const int min = 59, const int sec = 0);
  int strip2clust(const int arm = 0, const int sta = 0, const int oct = 0);
  int purge();
  bool sample_is_good(const int adc[NSAMPLES], const float adcr[NSAMPLES]);

  void set_rejection_mode(bool b) { rejection_mode = b; }
  bool get_rejection_mode() const { return rejection_mode; }

 protected:

  bool rejection_mode;
  // a map of all channels
  map_type mapMutCh; // this our set of MutCalibChannels
  map_iterator iter; // and the associated iterator
  MutOnlineChannel mutCh; // a strip/channel object

  std::string RunType;
  int NumEvtIn, NumEvtAna;

  // for the clustering
  onlineset_type stripset;
  onlineset_iterator stripiter;
  MutOnlineStrip mutStrip;
  std::vector<MutOnlineClust> clusters; 
  // local histo pointers
  TH1 *mutH1ZSRatio[NARMS];
  TProfile *mutHprofPktNchan[NARMS];
  TProfile *mutHprofPktErr[NARMS];
  TProfile *mutHprofHits[NARMS];
  TH1 *mutH1Signal[NARMS];
  TProfile *mutHprofAmplTime[NARMS];
  TProfile *mutHprofAmplTime_nstr[NARMS];
  TH1 *mutH1ClustAdc[NARMS];
  TH1 *mutH1PeakAdc[NARMS];
  TH1 *mutH1PeakAdcSta[NARMS][NSTATIONS];// for the stations
  TH1 *mutH1PeakAdcStaOct[NARMS][NSTATIONS][NOCTANTS];// for the stations and octants
  TH1 *mutH1NumEvent;
  TH1 *mutActivePkt[NARMS];
};

#endif /* __MUTRMON_H__ */
