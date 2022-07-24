#ifndef __ONLMONCLIENT_H__
#define __ONLMONCLIENT_H__

#include <OnlMonBase.h>
#include <PortNumber.h>

#include <ctime>
#include <list>
#include <map>
#include <string>
#include <vector>

class ClientHistoList;
class OnlMonDraw;
class OnlMonHtml;
class OnlMonTrigger;
class TCanvas;
class TH1;
class TStyle;

class OnlMonClient: public OnlMonBase
{
 public:
  static OnlMonClient *instance();
  virtual ~OnlMonClient();
  int UpdateServerHistoMap(const std::string &hname, const std::string &hostname);
  void PutHistoInMap(const std::string &hname, const std::string &hostname, const int port);
  void updateHistoMap(const char *hname, TH1 *h1d);
  TH1 *getHisto(const std::string &hname);
  OnlMonDraw *getDrawer(const std::string &name);
  int requestHisto(const char *what = "ALL", const std::string &hostname = "localhost", const int moniport = MONIPORT);
  int requestHistoList(const std::string &hostname, const int moniport, std::list<std::string> &histolist);
  int requestHistoByName(const std::string &what = "ALL");
  int requestHistoBySubSystem(const char *subsystem, int getall = 0);
  void registerHisto(const std::string &hname, const std::string &subsys);
  void Print(const char *what = "ALL");

  void AddServerHost(const char *hostname);
  void registerDrawer(OnlMonDraw *Drawer);
  int ReadHistogramsFromFile(const char *filename);
  int Draw(const char *who = "ALL", const char *what = "ALL");
  int MakePS(const char *who = "ALL", const char *what = "ALL");
  int MakeHtml(const char *who = "ALL", const char *what = "ALL");

  std::string htmlRegisterPage(const OnlMonDraw& drawer,
			       const std::string& path,
			       const std::string& basefilename,
			       const std::string& ext);
  
  void htmlAddMenu(const OnlMonDraw& drawer, const std::string& path,
		   const std::string& relfilename);

  void htmlNamer(const OnlMonDraw& drawer, const std::string& basefilename,
		 const std::string& ext, std::string& fullfilename,
		 std::string& filename);

  int LocateHistogram(const std::string &hname);
  int RunNumber();
  time_t EventTime(const char *which = "CURRENT");
  int SendCommand(const char *hostname, const int port, const char *cmd);

  void SetDisplaySizeX(int xsize) { display_sizex = xsize;}
  void SetDisplaySizeY(int ysize) { display_sizey = ysize;}
  int GetDisplaySizeX() {return display_sizex;}
  int GetDisplaySizeY() {return display_sizey;}
  int CanvasToPng(TCanvas *canvas, std::string const &filename);
  int HistoToPng(TH1 *histo, std::string const &pngfilename, const char *drawopt = "", const int statopt = 11);

  int SaveLogFile(const OnlMonDraw& drawer);
  // interface to OnlMonTrigger class methods
  OnlMonTrigger *OnlTrig();
  void Verbosity(const int i);
  int SetStyleToDefault();
  int isCosmicRun();
  int isStandalone();
  std::string RunType();
  void CacheRunDB(const int runno);

 private:
  OnlMonClient(const char *name="ONLMONCLIENT");
  int DoSomething(const char *who, const char *what, const char *opt);
  void InitAll();

  static OnlMonClient *__instance;
  int display_sizex;
  int display_sizey;
  int cosmicrun;
  int standalone;
  std::string runtype;
  int cachedrun;
  TH1 *clientrunning;
  TStyle *defaultStyle;
  std::map<const std::string, ClientHistoList *> Histo;
  std::map<const std::string, OnlMonDraw*> DrawerList;
  std::vector<std::string> MonitorHosts;
  OnlMonHtml* fHtml;
  OnlMonTrigger *onltrig;
};

#endif /* __ONLMONCLIENT_H__ */
