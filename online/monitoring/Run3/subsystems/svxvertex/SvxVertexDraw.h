#ifndef __SVXVERTEXDRAW_H__
#define __SVXVERTEXDRAW_H__

#include <OnlMonDraw.h>

class TCanvas;
class TGraphErrors;
class TPad;

class SvxVertexDraw: public OnlMonDraw
{

 public: 
  SvxVertexDraw(const char *name = "SVXVERTEXMON");
  virtual ~SvxVertexDraw() {}

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvas(const char *name);
  int DrawDeadServer(TPad *transparent);
  int DrawVertex();

  TCanvas *TC[1];
  TPad *transparent[1];
  TPad *TP[1][6];
};

#endif /*__SVXVERTEXDRAW_H__ */
