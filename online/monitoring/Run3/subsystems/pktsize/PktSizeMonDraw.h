#ifndef __PKTSIZEMONDRAW_H__
#define __PKTSIZEMONDRAW_H__

#include "OnlMonDraw.h"
#include <map>
#include <set>

class RunDBodbc;
class PktSizeDBodbc;
class TCanvas;
class TGraph;
class TH1;
class TMarker;
class TPad;
class TText;

class PktSizeMonDraw: public OnlMonDraw
{

 public: 
  PktSizeMonDraw(const char *name = "PKTSIZEMON");
  virtual ~PktSizeMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  int loadpreviousruns(const int nruns=5);
  int AddKnownBig(const unsigned int packetid, const float maxsize);
  void Print(const std::string &what = "ALL") const;
  int CleanOldRuns(const unsigned int maxrun);
  int RemoveRun(const int runno);

 protected:
  int MakeCanvas(const char *name);
  int DrawFirst(const char *what = "ALL");
  int DrawHistory(const char *what = "ALL");
  int DrawDeadServer(TPad *transparent);
  int FillPacketMap(const TH1 *histo);
  int FillRunPacketList(std::map<unsigned int, float> &pkts, const int runnumber);
  int ExtractActivePackets(const std::map<unsigned int, float> &packetsize);
  int MakeNoisyCandidates();
  int IsKnownBig(const unsigned int packetid, const float size);
  int PlotNoisy(TGraph &gr, TMarker &tr, TText &tx, const unsigned int ipkt, const unsigned int icnt);
  int ReplaceRunFromDB(const int runno);

  TCanvas *TC[2];
  TPad *transparent[2];
  TPad *Pad[2];
  PktSizeDBodbc *db;
  int lastrun;
  RunDBodbc *rd;
  std::map<int, std::map<unsigned int, float> > packetmap;
  std::map<std::string, std::pair<unsigned int, unsigned int> > granulepacketlimits;
  std::map<unsigned int, float> knownbig;
  std::set<unsigned int> activepackets;
  std::set<unsigned int> noisypackets;
  std::set<int> runlist;
};

#endif /*__PKTSIZEMONDRAW_H__ */
