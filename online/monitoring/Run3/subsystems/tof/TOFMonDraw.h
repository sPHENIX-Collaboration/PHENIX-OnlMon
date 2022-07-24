#ifndef __TOFMONDRAW_H__
#define __TOFMONDRAW_H__

#include "Tof.hh"
#include "OnlMonDraw.h"

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

class TOFMonDraw: public OnlMonDraw
{

 public: 
  TOFMonDraw();
  virtual ~TOFMonDraw();

  int InitTofCanvas0();
  int InitTofCanvas1();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int knownDeadTofSlatTot;
  int knownDeadTofSlat[TOF_NSLAT];
  int deadTofSlatTot;
  int deadTofSlat[TOF_NSLAT];

  int canvasMode; // 0: only one canvas 1: add TOF-Map

  int evtTrigIn;
  int evtTrigInMin;

  float     peak00htof210_v; // TVC Low
  float     peak01htof210_v; // TVC High
  float     peak00htof220_v; // QVC Low
  float     peak01htof220_v; // QVC High
  float     peak00htof230_v; // Min num. of Hit 
  float     peak00htof410_v;
  float     peak01htof410_v;
  float     peak00htof420_v; // Pos. Low
  float     peak01htof420_v; // Pos. High

  // MIP
  int evtTrigInMin_MIP;
  float     Th_MIP;//MIP Threshold
  int       No_MIP;//Number of MIP

  //  TStyle *tofStyle;
  
  TH2 *htofx10;
  TH2 *htofx11;
  TH2 *htofx12;

  TH1 *w;
  TGraph *gtof530;
  float htof530_ymax;
  
  TLine *peak00htof210;
  TLine *peak01htof210;
  TLine *peak00htof220;
  TLine *peak01htof220;
  TLine *peak00htof230;
  TLine *peak00htof420;
  TLine *peak01htof420;

  TLine *tofl00;
  TLine *tofl01;
  TLine *tofl02;
  TLine *tofl03;
  TLine *tofl04;
  TLine *tofl05;
  TLine *tofl06;
  TLine *tofl07;
  TLine *tofl08;
  TLine *tofl10;
  TLine *tofl11;
  TLine *tofl20;
  TLine *tofl21;
  TLine *tofl22;
  TLine *tofl30;
  TLine *tofl31;

  TLine *tof521;
  TLine *tof522;
  TLine *tof523;
  TLine *tof524;
  TLine *tof525;
  TLine *tof526;
  TLine *tof527;
  TLine *tof528;
  TLine *tof529;
  
  TText *tx520;
  TText *tx521;
  TText *tx522;
  TText *tx523;
  TText *tx524;
  TText *tx525;
  TText *tx526;
  TText *tx527;
  TText *tx528;
  TText *tx529;

  TPaveText *toflabel;
  TPaveText *tofmessage;
  TPaveText *tofstatinfo;

  TCanvas *TofCan0;
  TCanvas *TofCan1;
  TPad    *TofPad[2][5];

  TofGeometryObject *tofmongeo;
  TofAddressObject  *tofmonaddress;
};

#endif /*__TOFMONDRAW_H__ */
