#ifndef __PBGLUMONDRAW_H__
#define __PBGLUMONDRAW_H__

#include <OnlMonDraw.h>

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;
class TPaveText;



class PbGlUMonDraw: public OnlMonDraw
{

 public: 
  PbGlUMonDraw(const char *name = "PBGLUMON");
  virtual ~PbGlUMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvas(const char *name);
  int DrawDeadServer(TPad *transparent);

  TCanvas *TC[1];
  TPad *transparent[1];
  TPad *Pad[2];
  TPaveText *pave;
};

#endif /*__MYMONDRAW_H__ */
