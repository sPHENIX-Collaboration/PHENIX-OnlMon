#ifndef __ACCMONDRAW_H__
#define __ACCMONDRAW_H__

#include <OnlMonDraw.h>
#include <ACCMonDefs.h>
#include <set>

class TCanvas;
class TH1;
class TF1;
class TLine;
class TPad;
class TPaveText;
class TStyle;
class TText;

class ACCMonDraw: public OnlMonDraw
{

 public: 
  ACCMonDraw(const char *name = "ACCMON");
  virtual ~ACCMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  void AddKnownNoisy(const int channel);
 
 protected:
  int MakeCanvas(const char *name);
  int DrawDeadServer(TPad *transparent);
  int SetHistStyle(TH1 *hist);
  int SetHistStyle2(TH1 *hist);
  int CheckData(const char *name, const int tc_index);
  int CheckADC();
  int CheckTAC();
  int CheckADCTAC();
  int CheckTAC2();
  int DrawACCADCTDC(const char *name);
  int DrawACCTDCped(const char *name);

  int dead[ACC_ALLCHANNEL];
  int spikech[ACC_ALLCHANNEL];
  std::set<int> known_noisy;
  int run;
  int evt;
  int accevt;

  TCanvas *TC[2];
  TText *title[2];
  TText *message[4][4];
  TText *northsouth[2];
  TPad *Pad[9];
  TPad *transparent[2];
  TPad *msg[4];
  TH1 *brind;
  TLine *ln1;
  TStyle *os;
  TF1 *fitfunc;
};

#endif /*__ACCMONDRAW_H__ */
