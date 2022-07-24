#ifndef __SVXPIXELMONDRAW_H__
#define __SVXPIXELMONDRAW_H__

#include <OnlMonDraw.h>
#include "SvxPixelMonDefs.h"
class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;
class TStyle;
class TPaveText;
class TPave;
class TLatex;
class TLine;
class TBox;

/*
 * SvxPixelMonDraw analyzes and draws the histograms on canvases.
 * 3 sets of 2 (East/West) canvases are drawn. All histograms are
 * created and filled in SvxPixelMon.
 *
 * -- Canvases --
 *
 * SvxPixelLadderChipWest(East) : Plots 2D hitrate distributions
 *                                for each layer (chip v ladder).
 *
 * SvxPixelLadderWest(East) : Plots 1D hitrate vs chip distributions
 *                            for each layer. These are color coded
 *                            based on hitrate for cold/good/hot,
 *                            known issue, known hot.
 *
 * SvxPixelStabilityWest(East) : Plots profiles of normalized hitrate
 *                               vs time(?) for each ladder.
 *
 * -- Functions --
 *
 * Init : Set all the conditions that vary by run/species/energy.
 *        This includes reading chips with known issues from file,
 *        reading average hit rates from file, setting the limits
 *        for cold/hot chips drawn on SvxPixelLadderWest(East), and
 *        setting the Z-axis limits for the 2D histograms drawn on
 *        SvxPixelLadderChipWest(East).
 *
 * Draw : this function is the main routine to draw monitors.
 *        Type of canvases which you want to display is necessary
 *        as input.
 *          - ALL (Default)          : All canvases as detailed above
 *          - SvxPixelLadderChipWest : SvxPixelLadderChipWest only
 *          - SvxPixelLadderChipEast : SvxPixelLadderChipEast only
 *          - SvxPixelLadderWest     : SvxPixelLadderWest only
 *          - SvxPixelLadderEast     : SvxPixelLadderEast only
 *          - SvxPixelStabilityWest  : SvxPixelStabilityWest only
 *          - SvxPixelStabilityEast  : SvxPixelStabilityEast only
 *          - VTXP%(packetno)        : Calls DrawHalfLadder (not drawn with ALL)
 *          - SvxPixelDeadMapWest    : SvxPixelDeadMapWest only (not drawn with ALL)
 *          - SvxPixelDeadMapEast    : SvxPixelDeadMapEast only (not drawn with ALL)
 *
 * MakeCanvas : this function is a subroutine for Draw function and is
 *              to prepare the canvases. Type of canvases is necessary
 *              as input.
 *
 * DrawStability : Draws SvxPixelStabilityWest(East). Normalizes hitrate
 *                 for each ladder based on file input in Init via
 *                 ratemap[].
 *
 * DrawLadder : Draws SvxPixelLadderWest(East). Utilizes the
 *              chip_hitrate_low[] & chip_hitrate_high[] values set in
 *              Init to determine cold/hot chips & error message.
 *              Also masks known dead chips and known hot chips as read
 *              from file in Init via ig_dead[][][] & ig_hot[][][].
 *
 * DrawLadderChip : Draws SvxPixelLadderChipWest(East). Sets the Z-axis
 *                  limits of the 2D hitrate plots based on values set
 *                  in Init via hitmap_Zmin & hitmap_Zmax.
 *
 * DrawDeadMap : Draws SvxPixelDeadMapWest(East). Catagorizes pixels
 *               into dead/cold/good/hot and displays the result.
 *               This canvas is NOT included if "ALL" is passed to
 *               Draw().
 *
 */

class SvxPixelMonDraw: public OnlMonDraw
{

public:
    SvxPixelMonDraw(const char *name = "SVXPIXELMON");
    virtual ~SvxPixelMonDraw();

    int Init();
    int Draw(const char *what = "ALL");
    int MakePS(const char *what = "ALL");
    int MakeHtml(const char *what = "ALL");

protected:
    int MakeCanvas(const char *name);
    int DrawLadder(const char *what);
    int DrawLadderChip(const char *what);
    int DrawHalfLadder(const int packetid);
    int DrawStability(const char *what);
    int DrawDeadMap(const char *what);

    int read_config_file(unsigned int data[2][20][16], const std::string fname);
    int read_stability_file();

    int GetCountStatus(float avg, int lyr); //used for hot/cold status.

    int npad[ncanvas];
    TCanvas *TC[ncanvas];
    TPad *TP[ncanvas][nmaxpad]; // TPad
    TPad *transparent[ncanvas]; // TPad Transparent
    std::string canv_desc[ncanvas];  //description of content
    std::string canv_name[ncanvas];  //global name (must be unique)
    std::string canv_short[ncanvas]; //short summary (for html output)
    TLatex *titleStb; //title for stabilityhist
    TLatex *legendStb;//legend for stabilityhist
    TLine *maxLineStb[2][3];//line for max limit west_east/pad
    TLine *minLineStb[2][3];//line for min limit west_east/pad


    //  OnlMonDB *dbvars;

    //int DrawHistory(const char *what = "ALL");
    int DrawDeadServer(TPad *transparent);
    int TimeOffsetTicks;

    const unsigned int WEST;
    const unsigned int EAST;

    unsigned int ig_dead[2][20][16];
    unsigned int ig_hot [2][20][16];

    double chip_hitrate_low[2];  //[barrel], lower limit of chip hitrate - chips below marked cold
    double chip_hitrate_high[2]; //[barrel], upper limit of chip hitrate - chips above marked hot
    std::string ThreshText; //string containing text for chip thresholds

    double hitmap_Zmin; //Z-axis lower limit for 2D hitmap plots
    double hitmap_Zmax; //Z-axis upper limit for 2D hitmap plots

    float    m_stability_rate[60];

    unsigned int bad_ig_file;
    TPaveText *ladder_err_msg[2];
    TPave *ladder_box[5];

    TBox *deadBox;

};

#endif /*__SVXPIXELMONDRAW_H__ */
