#ifndef __TOFWMONDRAW_H__
#define __TOFWMONDRAW_H__

#include <TOFWMon.h>
#include <OnlMonDraw.h>

class TCanvas;
class TH1;
class TH2;
class TGraph;
class TText;
class TLine;
class TPad;
class TPaveText;

class TofGeometryObject;
class TofAddressObject;

class TOFWMonDraw: public OnlMonDraw
{

 public: 
  TOFWMonDraw();
  virtual ~TOFWMonDraw();

  int InitTofwCanvas0();
  int InitTofwCanvas1();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:

  int canvasMode; // 0: only one canvas 1: add TOFW-Map

  int evtTrigIn;
  int evtTrigInMin;

  float     peak00htofw210_v; // TVC Low
  float     peak01htofw210_v; // TVC High
  float     peak00htofw220_v; // QVC Low
  float     peak01htofw220_v; // QVC High
  float     peak00htofw230_v; // Min num. of Hit 
  float     peak00htofw410_v;
  float     peak01htofw410_v;
  float     peak00htofw420_v; // Pos. Low
  float     peak01htofw420_v; // Pos. High

  //  TStyle *tofwStyle;
  
  TH2 *htofwx10;
  TH2 *htofwx11;
  TH2 *htofwx12;

  TH1 *w;
  TGraph *gtofw530;
  float htofw530_ymax;
  
  TLine *peak00htofw210;
  TLine *peak01htofw210;
  TLine *peak00htofw220;
  TLine *peak01htofw220;
  TLine *peak00htofw420;
  TLine *peak01htofw420;

  TLine *tofwl00;
  TLine *tofwl01;
  TLine *tofwl02;
  TLine *tofwl03;
  TLine *tofwl04;
  TLine *tofwl05;
  TLine *tofwl06;
  TLine *tofwl07;
  TLine *tofwl08;
  TLine *tofwl10;
  TLine *tofwl11;
  TLine *tofwl20;
  TLine *tofwl21;
  TLine *tofwl22;
  TLine *tofwl30;
  TLine *tofwl31;

  TLine *tofw521;
  TLine *tofw522;
  TLine *tofw523;
  TLine *tofw524;
  TLine *tofw525;
  TLine *tofw526;
  TLine *tofw527;
  
  TText *tx520;
  TText *tx521;
  TText *tx522;
  TText *tx523;
  TText *tx524;
  TText *tx525;
  TText *tx526;
  TText *tx527;

  TPaveText *tofwlabel;
  TPaveText *tofwmessage;
  TPaveText *tofwstatinfo;

  TCanvas *TofwCan0;
  TCanvas *TofwCan1;
  TPad    *TofwPad[2][5];

};

#endif /*__TOFWMONDRAW_H__ */
