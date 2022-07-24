#ifndef __PBGLUDBODBC_H__
#define __PBGLUDBODBC_H__

#include <OnlMonBase.h>

#include <ctime>
#include <map>
#include <string>
#include <vector>

class PbGlUDBodbc: public OnlMonBase
{

 public:
  PbGlUDBodbc(const std::string &name);
  virtual ~PbGlUDBodbc();
  int CheckAndCreateTable();
  int AddRow(const int runnumber, const time_t unixtime, const int packetid, const std::string &what);

 private:

  int GetConnection();
  std::string dbname;
  std::string dbowner;
  std::string dbpasswd;
  std::string table;
  int tableexist;
};

#endif
