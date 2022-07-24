#ifndef __MPCEXMONDRAW_H__
#define __MPCEXMONDRAW_H__

#include <OnlMonDraw.h>

class TCanvas;
class TPad;
class TH2F;
class TString;

class MpcExMonDraw: public OnlMonDraw {
 public: 
  MpcExMonDraw(const char *name = "MPCEXMON");
  virtual ~MpcExMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  
 protected:
  int MakeCanvas(const char *name);
  void WriteCenter(TString);
  void DrawStamp(TString);
  int DrawRaw();
  int DrawSen();
  int DrawEne();
  int DrawEne2();
  int DrawEne3();
  int DrawTrg();
  int DrawRev();
  void ReadCurrents();
  void ReadTemperatures();
  int DrawDeadServer(TPad*);
  void MakeOutputFEED();

  TCanvas *mpcexTC[13];
  TPad *mpcexTP[74];
  TH2F *fCurrents;
  TH2F *fTemperatures;
  int fChnErr[64];
};

#endif /*__MPCEXMONDRAW_H__ */
