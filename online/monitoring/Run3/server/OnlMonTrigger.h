#ifndef ONLMONTRIGGER_H_
#define ONLMONTRIGGER_H_

#include <TriggerUtilities.h>

#include <map>
#include <set>
#include <string>

class TriggerHelper;
class TrigInfo;

#define NGL1TRIG 32

class OnlMonTrigger
{
 public:
  OnlMonTrigger();
  virtual ~OnlMonTrigger();
  void Print(const char *what = "ALL") const;
// define your own trigger name and bitmask
  void TrigMask(const std::string &name, const unsigned int bitmask); 
  
  int FillTrigInfo();
  int FillTrigInfoFromFile();
  void RunNumber(const int irun) {runnumber = irun;}

  // replaced triggerhelper/triggerobject methods 
  unsigned int getLevel1Bit(const std::string &name);
  bool didLevel1TriggerFire(const std::string &name);

  unsigned int get_lvl1_trig_scale_down(const unsigned int i);
  unsigned int get_lvl1_trig_scale_down_bybit(const unsigned int i);

  unsigned int get_lvl1_trig_bitmask(const unsigned int i);
  unsigned int get_lvl1_trig_bitmask_bybit(const unsigned int i);

  unsigned int get_lvl1_trig_bit(const unsigned int i);

  unsigned int get_lvl1_trig_index(const unsigned int i);
  unsigned int get_lvl1_trig_index_bybit(const unsigned int i);

  std::string get_lvl1_trig_name(const unsigned int i);
  std::string get_lvl1_trig_name_bybit(const unsigned int i);
  void AddBbcLL1TrigName(const std::string &trgname);

 protected:
  int ClearTrigInfo();
  int SetupTriggerHelper();
  unsigned int GetBitFromTriggerFile(const char *trigname) const;
  int send_message(const int severity, const std::string &err_message) const;
  int screwupmessage(const std::string &method, const unsigned int bit) const;
  int MyRunNumber;
  int MyRunNumberBit;
  int MyRunNumberTriggerHelper;
  int runnumber;
  TriggerUtilities tu;
  TriggerHelper *th;
  std::map<const std::string, unsigned int> trigmask;
  TrigInfo *triginfo[NGL1TRIG];
  std::set<std::string> BbcLL1TrigNames;
  std::set<unsigned char> trigbits;
};

#endif // __ONLMONTRIGGER_H_
