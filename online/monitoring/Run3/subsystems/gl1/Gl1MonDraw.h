#ifndef __GL1MONDRAW_H__
#define __GL1MONDRAW_H__

#include <string>

#include <OnlMonDraw.h>

class TCanvas;
class TH1;
class TLine;
class TPad;
class TPaveLabel;
class TStyle;
class TText;
class TString;


class Gl1MonDraw: public OnlMonDraw
{

 public:
  Gl1MonDraw();
  virtual ~Gl1MonDraw();

  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvases(const char *name);
  int MakeBCCanvases(const int iwhich);
  int DrawDeadServer();
  int DrawRejection();
  int DrawBCross(const int i);

  float bias1_expected_rejection[32], bias1_expected_rejection_err[32];
  float bias2_expected_rejection[32], bias2_expected_rejection_err[32];
  std::string BBCLL1_NAME, ZDC_NAME, BBCLL1_NOVERTEXCUT_NAME;

  TStyle *gl1Style; 

  TCanvas *gl1_c1;
 
  TText *trigname1[32];
  TText *GL1title;
  TPad *plotpad3;
  TPad *plotpad4;
  TLine *live;
  TText *legend9;
  TText *legend10;
  TLine *one;
  TText *legend7;
  TText *legend8;
  
  TPad *transparent1;
  
  
  TCanvas *gl1_c2;
  
  TText *GL1Rejtitle;
  TPad *plotpad1;
  TPad *plotpad2;
  TText *legend2;
  TText *legend5;
  TText *legend3;
  TText *legend15;
  TText *legend4;
  
  TPad *transparent2;
  
  TText *Crosstitle;
  TPad *plotpad11;
  TPad *plotpad12;
  TPad *plotpad13;
  TText *legend11;
  TText *legend12;
  TText *legend13;
  
  
  TText *Ratiotitle;
  TPad *plotpad5;
  TPad *plotpad6;
  TPad *plotpad7;
  TText *legend18;
  TText *legend19;
  TText *legend20;

  TCanvas *Gl1_bc_canv[4];
  TPad *bc_pad[4][8];
  TPad *bc_transparent[4];
};

#endif /*__GL1MONDRAW_H__ */
