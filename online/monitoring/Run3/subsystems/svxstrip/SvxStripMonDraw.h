#ifndef __SVXSTRIPMONDRAW_H__
#define __SVXSTRIPMONDRAW_H__

#include <OnlMonDraw.h>
#include "SvxStripLadderMap.h"

class TCanvas;
class TPad;
class TLegend;
class TPaveText;
class TPave;

/*
 * SvxStripMonDraw controls the analysis and draws the histograms on canvases.
 * A total of 9 canvases are drawn.
 *
 * -- Functions --
 *
 * Init : Set all the conditions that vary by run/species/energy. These include
 *        loading the files of known dead/hot rcc's, and setting the thresholds
 *        for cold/hot rccs used in DrawNormRCC.
 *
 * Draw : This function is the main routine to draw the monitor canvases.
 *        The canvase name is required as input.
 *           - ALL (Default)     : Draws all canvases.
 *           - canvas_name[]     : Draws the specific canvas
 *           - VTXS%(packetno)   : Calls DrawLadder(packetno)
 *
 * MakeCanvas : A subroutine for the Draw function. It creates and initializes
 *              the desired canvases. The canvas name is required as input.
 *
 * DrawADC :
 *
 * DrawLive :
 *
 * DrawSize :
 *
 * DrawLadder :
 *
 * DrawNormRCC : Draws SvxStripMonNormRCC. Utilizing the thresholds set in init
 *               it determines the number of cold/hot rccs and prints a
 *               corresponding message to the canvas.
 *
 * DrawCellID :
 *
 * DrawCellID2 :
 *
 * DrawBClock :
 *
 * DrawPACnt :
 *
 * DrawErrors :
 *
 */
class SvxStripMonDraw: public OnlMonDraw
{

public:
    SvxStripMonDraw(const char *name = "SVXSTRIPMON");
    virtual ~SvxStripMonDraw();

    int Init();
    int Draw(const char *what = "ALL");
    int MakePS(const char *what = "ALL");
    int MakeHtml(const char *what = "ALL");

protected:
    int MakeCanvas(const char *name);

    int DrawLive(const char *what = "ALL");
    int DrawNormRCC(const char *what = "ALL");
    int DrawRCCBClockError(const char *what = "ALL");
    int DrawCellID(const char *what = "ALL", int aflg = 0);

    int DrawADC(const char *what = "ALL");
    int DrawSize(const char *what = "ALL");
    int DrawLadder(const int ladder);
    int DrawCellID2(const char *what = "ALL", int aflg = 0);
    int DrawBClock(const char *what = "ALL");
    int DrawRCCBClock(const char *what = "ALL", int aflg = 0);
    int DrawPACnt(const char *what = "ALL");
    int DrawErrors(const char *what = "ALL");

    int DrawDeadServer(TPad *transparent);
    int read_config_file(int data[2][24][6], const std::string &fname);

    static const int ncanvas = 14;
    int npad[ncanvas];
    static const int nmaxpad = 40;
    TCanvas *TC[ncanvas];
    TPad *TP[ncanvas][nmaxpad]; // TPad
    TPad *transparent[ncanvas]; // TPad Transparent
    std::string canv_desc[ncanvas];  //description of content
    std::string canv_name[ncanvas];  //global name (must be unique)
    std::string canv_short[ncanvas]; //short summary (for html output)

    int ig_clockissue[4][24][6]; //[barrel][ladder][rcc], 0: no known issue, 1: known hotclock issue
    int  ig_hot[4][24][6]; //[barrel][ladder][rcc], 0: no known issue, 1: known hot
    int ig_dead[4][24][6]; //[barrel][ladder][rcc], 0: no known issue, 1: known dead

    double rcc_hitrate_low[2];  //[barrel], lower limit of chip hitrate - chips below marked cold
    double rcc_hitrate_high[2]; //[barrel], upper limit of chip hitrate - chips above marked hot
    std::string ThreshText; //string containing text for chip thresholds


    double charge_range[2]; //controls the charge limits in ""
    double multi_range[2]; //controls the multiplicity range in ""
    std::string MultiText; //string containing text for multiplicity thresholds

    TPave *ladder_box[5];
    TPaveText *rcc_msg;
    TPaveText *rcc_msg_clock;

    SvxStripLadderMap ladder_map;
};

#endif /*__SVXSTRIPMONDRAW_H__ */
