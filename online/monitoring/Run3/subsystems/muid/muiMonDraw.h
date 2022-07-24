#ifndef __MUIMONDRAW_H__
#define __MUIMONDRAW_H__

#include <mui_mon_const.h>
#include <mui_mon_draw_const.h>
#include <OnlMonDraw.h>

class TCanvas;
class TH1;
class TStyle;
class TLine; // Added 05/19/2003 TVC

///Client side drawer class for muid monitor. 
class muiMonDraw: public OnlMonDraw
  {

  public:
    ///Default constructor.
    muiMonDraw();

    ///Default destructor.
    virtual ~muiMonDraw();

    ///Initailize values.
    int Init();

    ///Top level drawing method.
    int Draw(const char *what = "ALL");
    
    ///Generate postscript file. Could use lots of improvement.
    int MakePS(const char *what = "ALL");

    ///Generate html/gif files. Could use improvement.
    int MakeHtml(const char *what = "ALL");
    
    ///Use reference histograms to normalize distributions?
    void UseReferences(bool useref = true) {
      useReferences = useref;
    }

  protected:

    ///Pointer to south monitor canvas.
    TCanvas* muid_canvasS;

    ///Pointer to north monitor canvas.
    TCanvas* muid_canvasN;
    
    ///Pointer to twopack hit distribution canvases.
    TCanvas* twopackCanvas[MAX_ARM][MAX_ORIENTATION];

#ifdef MUID_USE_BLT
    ///Pointer to the BLT canvases
     TCanvas* muid_blt_canvas[NBLT_CANVAS];  // vasily   
#endif

    ///Fromat histogram in a standard way.
    void setupHist(TH1* muiHist, const char* XTitle, const char* YTitle);

    ///Does the real work of drawing monitor canvases.
    int real_draw_muid(TCanvas* muid_canvas, const short arm);

    ///Register histogram names with the client framework.
    void registerHists();

    ///Does the real work of drawing twopack hit distributions.
    int DrawTwopacks();

#ifdef MUID_USE_BLT
   ///Does the real work of drawing blt hit distributions.
    int DrawBlt(const int blt_canvas = 0, const float th = 0.30, const float tw = 0.06,
		const int sx = 900, const int sy = 660);
#endif

    ///Histogram which covers empty horizontal signal cable positions.
    TH1* emptyHChannels;

    ///Histogram which covers empty vertical signal cable positions.
    TH1* emptyVChannels;

    ///Hits per ROC histogram.
    TH1* ROCtotal;

    ///Hits per HV chain histogram.
    TH1* HVHits[MAX_ORIENTATION];

    ///Hits per signal cable histogram.
    TH1* CableHits[MAX_ORIENTATION];

    ///eff min/max lines
    TLine* lmin[NPLOT_PANELS];
    TLine* lmax[NPLOT_PANELS];

    ///Use reference histograms flag.
    bool useReferences;

    ///Pointer to muid style.
    TStyle* muiStyle;

  };

#endif /*__MUIMONDRAW_H__ */
