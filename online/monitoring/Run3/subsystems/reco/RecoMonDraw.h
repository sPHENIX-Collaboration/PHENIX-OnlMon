#ifndef __RECOMONDRAW_H__
#define __RECOMONDRAW_H__

#include <OnlMonDraw.h>

class TCanvas;
class TGraphErrors;
class TPad;

class RecoMonDraw: public OnlMonDraw
{

 public: 
  RecoMonDraw(const char *name = "RECOMON");
  virtual ~RecoMonDraw() {}

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvas(const char *name);
  int DrawDeadServer(TPad *transparent);
  int DrawFirst();
  int DrawSecond();
  int DrawThird();
  int DrawFourth();
  TCanvas *TC[9];
  TPad *transparent[9];
  TPad *Pad[18];
  TPad *Dch;
};

#endif /*__RECOMONDRAW_H__ */
