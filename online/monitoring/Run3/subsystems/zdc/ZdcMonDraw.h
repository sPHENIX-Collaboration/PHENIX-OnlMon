#ifndef __ZDCMONDRAW_H__
#define __ZDCMONDRAW_H__

#include "ZdcMonDefs.h"
#include <OnlMonDraw.h>

class TArc;
class TCanvas;
class TH1;
class TH2;
class TF1;
class TLine;
class TPad;
class TPaveText;
class TProfile;
class TStyle;
class TText;
class OnlMonDB;
class TGraphErrors;


// int NUM_CANV = 16;
// int NUM_PAD = 184; //+40 ch + 1 title 
// int NUM_GRAPH = 48;

const int NUM_CANV = 19;
const int NUM_PAD = 275; // 80 + 8 + 3 = 91
const int NUM_GRAPH = 48;

class ZdcMonDraw: public OnlMonDraw
{

 public:
  ZdcMonDraw();
  virtual ~ZdcMonDraw();

  int Init();
  void InitLine(TLine *line, int lcolor, int lwidth, int lstyle);

  int Draw(const char *what = "ALL");

  int Draw1();
  int Draw2();
  int Draw3();
  int Draw4();
  int Draw5();
  int Draw6();
  int Draw7();
  int Draw8();
  int Draw9();
  int Draw16();
  int Draw17();
  int Draw18();
  int Draw19();


  int DrawHistory(const char *CanvasName, int Nhist);

  void FitGauss(TH1 *histo, double xmin, double xmax);
  void FitLinear(TH1 *histo, double xmin, double xmax);
  void Fit2DGauss(TH2 *histo, double xmin, double xmax, double ymin, double ymax);
  void Draw0thPad(TH2 *histo, int npad, int ncanvas);
  void DrawPaveText(const char *zdc_text, int npad);
  void DrawProfStat(TPaveText *ptext, TH2 *histo, const char * unit, int npad, char type);
  void DrawHisto(TH1 * histo, int npad, bool logy, int lcolor,
		 const char * xtit, const char * ytit, const char * dr_opt);
  void DrawHisto2D(TH2 * histo, int npad, int logz,
		 const char * xtit, const char * ytit, const char * dr_opt);
  void DrawHistoSame(TH1 *histo, int lcolor);
  void DrawProfile(TProfile *profile, int npad, bool logy, int lcolor,
		   const char * xtit, const char * ytit, int i_range);
  void DrawSmdValue(int nsmd, int npad);

  int DrawHistoryPlot(int Ncanv, int Npad, int Nplot, const char * varname);

  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvas1(const char *name);
  int MakeCanvas2(const char *name);
  int MakeCanvas3(const char *name);
  int MakeCanvas4(const char *name);
  int MakeCanvas5(const char *name);
  int MakeCanvas6(const char *name);
  int MakeCanvas7(const char *name);
  int MakeCanvas8(const char *name);
  int MakeCanvas9(const char *name);
  int MakeCanvas16(const char *name);
  int MakeCanvas17(const char *name);
  int MakeCanvas18(const char *name);
  int MakeCanvas19(const char *name);

  int MakeCanvasHistory(const char *name, const char * title, int Nhist);

  int DrawDeadServer(TPad *transparent);

  TGraphErrors *gr[NUM_GRAPH];
  OnlMonDB *dbvars;

  int TimeOffsetTicks;

  TStyle *zdcStyle;
  TCanvas *TC[NUM_CANV];
  TPad *Pad[NUM_PAD];
  TPad *transparent[NUM_CANV];

  TPaveText *label0;
  TPaveText *sysmdt, *sxsmdt, *nysmdt, *nxsmdt; 
  TPaveText *ls1t, *ls2t, *ls3t, *ln1t, *ln2t, *ln3t;
  TPaveText *ls1bt, *ls2bt, *ls3bt, *ln1bt, *ln2bt, *ln3bt;
  // TText *text0;
  TArc *arc[2];
  TLine *line[20];
  TProfile *zvb_prof;
  TH2 *smd_value;
  TH2 *smd_value_good;
  TH2 *smd_value_small;
  double *par;
  double perr;
  };

#endif /*__ZDCMONDRAW_H__ */
