#include "SvxPixelMonDraw.h"
#include <OnlMonClient.h>
#include <OnlMonDB.h>

#include <phool.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TText.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TLatex.h>
#include <TPave.h>
#include <TBox.h>

#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#include <sys/time.h>

#include <TGaxis.h>

using namespace std;

static const int color[10] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
static const int style[10] = {20, 20, 20, 20, 20, 21, 21, 21, 21, 21};
static const int pxlstyle[10] = {24, 24, 24, 24, 24, 25, 25, 25, 25, 25};

const string arm_desc[2] = {"West", "East"};

const int L2WEST = 0;
const int L2EAST = 1;
const int L1WEST = 2;
const int L1EAST = 3;
const int CHIPMAP = 4;
const int STBWEST = 5;
const int STBEAST = 6;
const int DEADEAST = 7;
const int DEADWEST = 8;

////////////////////////////

timeval start_time, end_time;
void initTimeInterval()
{
    gettimeofday(&end_time, NULL);
}

void printTimeInterval(const char *comment)
{
    bool debug = false;

    start_time = end_time;
    gettimeofday( &end_time, NULL );

    double timeval = (end_time.tv_sec - start_time.tv_sec) +
                     (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    if (debug) cout << "time " << comment << " : " << timeval << endl;
}
////////////////////////////


SvxPixelMonDraw::~SvxPixelMonDraw()
{
    delete ladder_err_msg[0];
    delete ladder_err_msg[1];
    delete titleStb;
    delete legendStb;

    for (int we = 0; we < 2; we++)
    {
        for (int ipad = 0; ipad < 3; ipad++)
        {
            delete maxLineStb[we][ipad];
            delete minLineStb[we][ipad];
        }
    }

    for (int ii = 0; ii < 5; ii++)
    {
        delete ladder_box[ii];
    }

    delete deadBox;
}

SvxPixelMonDraw::SvxPixelMonDraw(const char *name):
    WEST(0),
    EAST(1),
    bad_ig_file(0)
{
    ThisName = name;

    canv_name[L2WEST] = "SvxPixelLadderChipWest";
    canv_name[L2EAST] = "SvxPixelLadderChipEast";
    canv_name[L1WEST] = "SvxPixelLadderWest";
    canv_name[L1EAST] = "SvxPixelLadderEast";
    canv_name[CHIPMAP] = "SvxPixelChipMap";
    canv_name[STBEAST] = "SvxPixelStabilityEast";
    canv_name[STBWEST] = "SvxPixelStabilityWest";
    canv_name[DEADEAST] = "SvxPixelDeadMapEast";
    canv_name[DEADWEST] = "SvxPixelDeadMapWest";

    canv_desc[L2WEST] = "VTXP Ladder Chip West";
    canv_desc[L2EAST] = "VTXP Ladder Chip East";
    canv_desc[L1WEST] = "VTXP Ladder West";
    canv_desc[L1EAST] = "VTXP Ladder East";
    canv_desc[CHIPMAP] = "Detailed Chip Map";
    canv_desc[STBEAST] = "Stability Plot East";
    canv_desc[STBWEST] = "Stability Plot West";
    canv_desc[DEADEAST] = "Dead Map Plot East";
    canv_desc[DEADWEST] = "Dead Map Plot West";

    canv_short[L2WEST] = "LadderChipWest";
    canv_short[L2EAST] = "LadderChipEast";
    canv_short[L1WEST] = "LadderWest";
    canv_short[L1EAST] = "LadderEast";
    canv_short[CHIPMAP] = "ChipMap";
    canv_short[STBEAST] = "Stability_East";
    canv_short[STBWEST] = "Stability_West";
    canv_short[DEADEAST] = "DeadEast";
    canv_short[DEADWEST] = "DeadWest";

    npad[L2WEST] = 3;
    npad[L2EAST] = 3;
    npad[L1WEST] = 3;
    npad[L1EAST] = 3;
    npad[CHIPMAP] = 8;
    npad[STBWEST] = 3;
    npad[STBEAST] = 3;
    npad[DEADEAST] = 3;
    npad[DEADWEST] = 3;

    for (int icanvas = 0; icanvas < ncanvas; icanvas++)
    {
        TC[icanvas] = 0;
        transparent[icanvas] = 0;
        for (int ipad = 0; ipad < nmaxpad; ipad++)
        {
            TP[icanvas][ipad] = 0;
        }
    }

    ladder_err_msg[0] = 0;
    ladder_err_msg[1] = 0;

    titleStb = new TLatex();
    legendStb = new TLatex();
    for (int we = 0; we < 2; we++)
    {
        for (int ipad = 0; ipad < 3; ipad++)
        {
            maxLineStb[we][ipad] = new TLine(-50, 1, 5450, 1);
            minLineStb[we][ipad] = new TLine(-50, 1, 5450, 1);
        }
    }

    for (int ii = 0; ii < 5; ii++)
    {
        ladder_box[ii] = 0;
    }

    //initialize the dead channel graphics and the reference box
    deadBox = new TBox;

    // init stability rate with 0
    memset(m_stability_rate, 0, sizeof(m_stability_rate));
}

int SvxPixelMonDraw::Init()
{

    //Initialize the cold/hot chip hitrate limits used in DrawLadder()
    //These are run/species/energy dependent!
    //Set for Run 16 AuAu 200 GeV - T.MOON 20 Jun 2016
    chip_hitrate_low[0]  = 1;  // B0 cold
    chip_hitrate_high[0] = 20; // B0 hot
    
    //Set for Run 16 AuAu 200 GeV - T.MOON 20 Jun 2016
    chip_hitrate_low[1]  = 0.1; // B1 cold
    chip_hitrate_high[1] = 5;   // B1 hot
  
    ThreshText = "Cold/Hot thresholds set for Run16 Au+Au 200 GeV - Jun 20, 2016";

    //Initialize the Z-axis limit for the 2D hitmap plots in DrawLadderChip()
    //These are run/species/energy dependent!
    //Set for Run 16 AuAu 200 GeV - T.MOON 20 Jun 2016
    hitmap_Zmin = 1e-4;
    hitmap_Zmax = 100;

    // Get a list of chips with known isues from
    // $ONLMON_CALIB.
    // This includes 2 lists:
    // ig_dead : list of known dead chips
    // ig_hot  : list of known hot chips
    // First - 0 the arrays
    for ( int barrel = 0; barrel < NBARREL; barrel++ )
    {
        for ( int ladder = 0; ladder < 20; ladder++ )
        {
            for ( int chip = 0; chip < 16; chip++ )
            {
                ig_dead[barrel][ladder][chip] = 0;
                ig_hot [barrel][ladder][chip] = 0;
            }
        }
    }

    int iret[2] = {0, 0};
    iret[0] = read_config_file(ig_dead, "SvxPixelDeadMap.txt");
    iret[1] = read_config_file(ig_hot , "SvxPixelKnownHotMap.txt");

    read_stability_file();

    return 0;
}

int SvxPixelMonDraw::read_config_file(unsigned int data[2][20][16], const std::string fname)
{
    const char *calibdir = getenv("ONLMON_CALIB");
    ostringstream parfile("");
    if (calibdir)
    {
        parfile << calibdir << "/";
    }
    else
    {
        cerr << "SvxPixelMonDraw::read_config_file Error, $ONLMON_CALIB environment variable not set!\nWill try reading locally.";
    }
    parfile << fname;

    ifstream infile(parfile.str().c_str(), ifstream::in);
    if (! infile)
    {
        cout << "Could not read file: " << parfile.str() << endl;
        return -1;
    }
    cout << endl << "Reading parameters from " << parfile.str() << endl;
    string line;
    int barrel, ladder, chip;
    while ( getline(infile, line) )
    {
        if ( line[0] == '#' || line[0] == '/' ) continue;
        istringstream iss_line(line.data());
        iss_line >> barrel >> ladder >> chip;
        if ( barrel >= NBARREL || barrel < 0 || ladder >= 20 || ladder < 0 || chip < 0 || chip >= 16 )
        {
            cerr << "Invalid input line: " << line << endl;
	    continue;
        }
        data[barrel][ladder][chip] = 1;
    }
    return 0;
}


int SvxPixelMonDraw::MakeCanvas(const char *name)
{
    OnlMonClient *cl = OnlMonClient::instance();
    int xsize = cl->GetDisplaySizeX();
    int ysize = cl->GetDisplaySizeY();

    ostringstream tname;
    ostringstream tdesc;

    for (int icanvas = 0; icanvas < ncanvas; icanvas++)
    {
        if ( strcmp(name, "ALL") && strcmp(name, canv_name[icanvas].c_str()) ) continue;

        // CANVAS
        TC[icanvas] = new TCanvas(canv_name[icanvas].c_str(),
                                  canv_desc[icanvas].c_str(),
                                  -1, 0, 7. / 8.*xsize, 3. / 4.*ysize);
        gSystem->ProcessEvents();

        // PAD(s);
        if (
            icanvas == L2WEST || icanvas == L2EAST ||
            icanvas == L1WEST || icanvas == L1EAST ||
            icanvas == DEADWEST || icanvas == DEADEAST
        )
        {
            // pad coordinates
            const double lox[3] = {0.000 , 0.000  , 0.557 };
            const double loy[3] = {0.450 , 0.000  , 0.450 };
            const double hix[3] = {0.557 , 1.000  , 1.000 };
            const double hiy[3] = {0.900 , 0.450  , 0.900 };

            for (int ipad = 0; ipad < 3; ipad++)
            {
                tname.str("");
                tname << "SvxPixelMonPad" << icanvas << "_" << ipad;

                TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                             lox[ipad], loy[ipad], hix[ipad], hiy[ipad],
                                             0);

                if (ipad == 1)
                {
                    double tmarg = TP[icanvas][ipad]->GetLeftMargin();
                    TP[icanvas][ipad]->SetLeftMargin(tmarg / 2.);
                }
                if (ipad == 0 || ipad == 1)
                {
                    TC[icanvas]->cd();
                    TP[icanvas][ipad]->SetBottomMargin(0.15);
                    TP[icanvas][ipad]->SetRightMargin(0.02);
                    double lmargin = ( ipad == 0 ) ? 0.2 : 0.111;
                    TP[icanvas][ipad]->SetLeftMargin(lmargin);
                }
                TP[icanvas][ipad]->Draw();
            }
        }
        if (icanvas == CHIPMAP)
        {
            TC[icanvas]->cd();
            for (int ipad = 0; ipad < 8; ipad++)
            {
                tname.str("");
                tname << "SvxPixelMonPad" << icanvas << "_" << ipad;
                int nx = 4;
                int ny = 2;
                int ix = ipad % nx;
                int iy = ipad / nx;
                float lox = 0. + (ix + 0) * 1. / nx;
                float hix = 0. + (ix + 1) * 1. / nx;
                float loy = 0. + (iy + 0) * 0.9 / ny;
                float hiy = 0. + (iy + 1) * 0.9 / ny;
                TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                             lox, loy, hix, hiy,
                                             0);
                TP[icanvas][ipad]->SetRightMargin(0.15);
                TP[icanvas][ipad]->Draw();
            }
        }
        if (icanvas == STBEAST || icanvas == STBWEST)
        {
            TC[icanvas]->cd();
            int ny = 3;
            for (int ipad = 0; ipad < npad[icanvas]; ipad++)
            {
                tname.str("");
                tname << "SvxPixelMonPad" << icanvas << "_" << ipad;
                float local_lox = 0.;
                float local_hix = 1.;
                float local_loy = 0.9 * double(ipad) / ny;
                float local_hiy = 0.9 * double(ipad + 1) / ny;

                TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                             local_lox, local_loy, local_hix, local_hiy,
                                             0);
                TP[icanvas][ipad]->SetBottomMargin(0.15);
                TP[icanvas][ipad]->Draw();

            }
        }

        // TRANSPARENT
        TC[icanvas]->cd();
        tname.str("");
        tname << "SvxPixelMonTrans" << icanvas;
        transparent[icanvas] = new TPad(tname.str().c_str(),
                                        "this does not show",
                                        0, 0, 1, 1);
        transparent[icanvas]->SetFillStyle(4000);
        transparent[icanvas]->Draw();
    }

    return 0;
}

int SvxPixelMonDraw::Draw(const char *what)
{
    //cout<<"draw start"<<endl;
    initTimeInterval();

    int iret = 0;
    int idraw = 0;
    if (!strcmp(what, "ALL") || !strcmp(what, "SvxPixelLadderChipWest"))
    {
        iret += DrawLadderChip("SvxPixelLadderChipWest");
        idraw ++;
    }
    if (!strcmp(what, "ALL") || !strcmp(what, "SvxPixelLadderChipEast"))
    {
        iret += DrawLadderChip("SvxPixelLadderChipEast");
        idraw ++;
    }
    if (!strcmp(what, "ALL") || !strcmp(what, "SvxPixelLadderWest"))
    {
        iret += DrawLadder("SvxPixelLadderWest");
        idraw ++;
    }
    if (!strcmp(what, "ALL") || !strcmp(what, "SvxPixelLadderEast"))
    {
        iret += DrawLadder("SvxPixelLadderEast");
        idraw ++;
    }
    if (!strcmp(what, "ALL") || !strcmp(what, "SvxPixelStabilityWest"))
    {
        iret += DrawStability("SvxPixelStabilityWest");
        idraw ++;
    }
    if (!strcmp(what, "ALL") || !strcmp(what, "SvxPixelStabilityEast"))
    {
        iret += DrawStability("SvxPixelStabilityEast");
        idraw ++;
    }
    //for now only draw deadmap if we ask for it
    if (!strcmp(what, "SvxPixelDeadMapEast"))
    {
        iret += DrawDeadMap("SvxPixelDeadMapEast");
        idraw++;
    }
    if (!strcmp(what, "SvxPixelDeadMapWest"))
    {
        iret += DrawDeadMap("SvxPixelDeadMapWest");
        idraw++;
    }
    string cmd(what);
    if (cmd.find("VTXP%") != string::npos )
    {
        string packet_s = cmd.substr(cmd.find("VTXP%") + 5, cmd.size());
        int packet_i = atoi(packet_s.c_str());
        iret += DrawHalfLadder(packet_i);
        idraw++;
    }
    if (!idraw)
    {
        cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
        iret = -1;
    }

    printTimeInterval("draw end");
    return iret;
}

int SvxPixelMonDraw::DrawHalfLadder(const int packetid)
{
    int icanvas = CHIPMAP;
    if (! gROOT->FindObject("SvxPixelChipMap"))
    {
        MakeCanvas("SvxPixelChipMap");
    }
    TC[icanvas]->Clear("D");

    const float zmin = pow(10., -5.);
    const float zmax = pow(10., -1.);

    OnlMonClient *cl = OnlMonClient::instance();
    TH2 *h2chip[8];
    TH1 *vtxp_pars = cl->getHisto("vtxp_params");

    // set the color palette
    gStyle->SetPalette(1);


    // fetch histograms
    int spiro = packetid - vtxp_packet_base;
    ostringstream name;
    for (int chip = 0; chip < NCHIP; chip++)
    {
        name.str("");
        name << "vtxp_chipmap_" << spiro << "_" << chip;
        TH2 *htemp = (TH2 *)cl->getHisto(name.str());
        if ( htemp == 0 || vtxp_pars == 0)
        {
            DrawDeadServer(transparent[icanvas]);
            TC[icanvas]->Update();
            return -1;
        }
        name << "_clone";
        h2chip[chip] = (TH2 *)htemp->Clone(name.str().c_str());

        float nevents = vtxp_pars->GetBinContent(EVENTCOUNTBIN);
        if ( nevents > 0 )
        {
            h2chip[chip]->Scale(1. / nevents);
        }

        TP[icanvas][chip]->cd();
        h2chip[chip]->SetStats(0);
        h2chip[chip]->SetMaximum(zmax);
        h2chip[chip]->SetMinimum(zmin);
        TP[icanvas][chip]->SetLogz(1);
        h2chip[chip]->Draw("colz");
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_" << icanvas << " Run@ " << cl->RunNumber() << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[icanvas]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    TC[icanvas]->Update();

    // clean up
    for (int chip = 0; chip < NCHIP; chip++)
    {
        //delete h2chip[chip];
    }

    return 0;
}

int SvxPixelMonDraw::DrawStability(const char *what)
{
    const  float rate_range = 0.25;
    unsigned int icanvas    = STBEAST;
    unsigned int arm        = EAST;

    string cmd(what);
    if ( cmd.find("East") == string::npos ) // if East is NOT found, the arm is West
    {
        icanvas = STBWEST;
        arm = WEST;
    }

    if (! gROOT->FindObject(what))
    {
        MakeCanvas(what);
    }
    TC[icanvas]->Clear("D");

    TText PrintDiag;
    PrintDiag.SetTextFont(62);
    PrintDiag.SetTextSize(0.06);
    PrintDiag.SetNDC();         // set to normalized coordinates
    PrintDiag.SetTextAlign(13); //
    ostringstream diagstream;
    diagstream << "Vtx-Pixel " << arm_desc[arm];
    PrintDiag.DrawText(0.35, 0.95, diagstream.str().c_str());
    //  TC[icanvas]->Range(0,0,1,1);

    OnlMonClient *cl = OnlMonClient::instance();
    //TProfile*   pmulti[30];
    TH1D   *pmulti[30];
    TH1        *vtxp_pars    = cl->getHisto("vtxp_params");
    TProfile2D *vtxp_pmulti2d = (TProfile2D *)cl->getHisto("vtxp_multi_time_2d");

    if (
        vtxp_pars == NULL ||
        vtxp_pmulti2d == NULL
    )
    {
        DrawDeadServer(transparent[icanvas]);
        TC[icanvas]->Update();
        //std::cout << "SvxPixelMonDraw::DrawStability - no histos" << std::endl;
        return -1;
    }

    // fetch histograms
    ostringstream name;
    //spiro = packetID-1
    //mapping
    //spiro  Barrel   Ladder   South/North      West/East
    //00-04     0       5-9          N              E
    //05-14     1      10-19         N              E
    //15-19     0       5-9          S              E
    //20-29     1      10-19         S              E
    //30-34     0       0-4          N              W
    //35-44     1       0-9          N              W
    //45-49     0       0-4          S              W
    //50-59     1       0-9          S              W

    unsigned int lo_spiro = (arm == WEST) ? 30 :  0;
    unsigned int hi_spiro = (arm == WEST) ? 60 : 30;
    static const int SOUTH = 0;
    static const int NORTH = 1;
    bool firstfill[3] = {true, true, true};

    for (unsigned int spiro = lo_spiro; spiro < hi_spiro; spiro++)
    {
        int barrel = (spiro % 15 <= 4) ? 0 : 1;

        int south_north = ((spiro / 15) % 2 == 0) ? NORTH : SOUTH;
        int ladder;
        if      (spiro <= 14)
        {
            ladder = spiro + 5;
        }
        else if (spiro <= 29)
        {
            ladder = spiro - 10;
        }
        else if (spiro <= 34)
        {
            ladder = spiro - 30;
        }
        else if (spiro <= 44)
        {
            ladder = spiro - 35;
        }
        else if (spiro <= 49)
        {
            ladder = spiro - 45;
        }
        else
        {
            ladder = spiro - 50;
        }

        name.str("");
        name << "vtxp_multiprof_time_" << spiro;
        vtxp_pmulti2d->GetXaxis()->SetRange(spiro + 1, spiro + 1);

	unsigned int pmindex = spiro - lo_spiro;
        pmulti[pmindex] = (TH1D *)vtxp_pmulti2d->ProjectionY(name.str().c_str());

        //name.str("");
        //name << "vtxp_multi_time_" << spiro;
        //pmulti[spiro]=(TProfile*)cl->getHisto(name.str());

        if ( pmulti[pmindex] == 0 )
        {
            DrawDeadServer(transparent[icanvas]);
            TC[icanvas]->Update();
            return -1;
        }

        //normalize with the nominal rate
        pmulti[pmindex]->SetMaximum(2);
        if (m_stability_rate[spiro] > 0.)
        {
            pmulti[pmindex]->Scale(1. / m_stability_rate[spiro]);
        }


        static const int mstyle = 20;
        static const int mcolor[2][5] = {{1, 2, 3, 4, 6}, {7, 8, 9, 11, 12}}; //south_north,ladder%5
        int ipad;
        int ladder0, ladder1;
        if (barrel == 0)
        {
            ipad = 2;
            ladder0 = (ladder / 5) * 5;
            ladder1 = (ladder / 5) * 5 + 4;
        }
        else    //barrel==1
        {
            ipad = 2 - ((ladder % 10) / 5 + 1);
            ladder0 = (ladder / 5) * 5;
            ladder1 = (ladder / 5) * 5 + 4;
        }

        TP[icanvas][ipad]->cd();

        pmulti[pmindex]->SetMarkerStyle(mstyle);
        pmulti[pmindex]->SetMarkerColor(mcolor[south_north][ladder % 5]);
        pmulti[pmindex]->SetStats(0);
        pmulti[pmindex]->SetXTitle("time(sec)");
        pmulti[pmindex]->GetYaxis()->SetTitle("normalized hit rate");
        pmulti[pmindex]->GetYaxis()->SetTitleSize(0.09);
        pmulti[pmindex]->GetYaxis()->SetLabelSize(0.09);
        pmulti[pmindex]->GetYaxis()->SetNdivisions(10);
        pmulti[pmindex]->GetYaxis()->SetTickLength(0.01);
        pmulti[pmindex]->GetYaxis()->SetRangeUser(0,1.5);
        pmulti[pmindex]->SetTitleOffset(0.3, "Y");

        pmulti[pmindex]->GetXaxis()->SetLabelSize(0.07);
        double xmin = pmulti[pmindex]->GetXaxis()->GetXmin();
        double xmax = pmulti[pmindex]->GetXaxis()->GetXmax();
        TGaxis xax;
        xax.SetTitle("time (sec)");
        xax.SetTitleSize(0.1);
        xax.SetTitleOffset(0.7);
        xax.SetLabelSize(0.0);
        xax.SetNdivisions(10);
        double wmin = pmulti[pmindex]->GetYaxis()->GetXmin();
        double wmax = pmulti[pmindex]->GetYaxis()->GetXmax();
        xax.DrawAxis(xmin, 0., xmax, 0., wmin, wmax, wmax - wmin);


        if (firstfill[ipad])
        {
            firstfill[ipad] = false;

            pmulti[pmindex]->SetTitleSize(0.4);

            pmulti[pmindex]->Draw("L");

            char title[100];
            titleStb->SetTextSize(0.1);
            sprintf(title, "Barrel %1d  Ladder %02d-%02d", barrel, ladder0, ladder1);
            //titleStb->DrawLatex(0,2.0,title);
            titleStb->DrawTextNDC(0.10, 0.90, title);

            legendStb->SetTextSize(0.06);
            char legend[100];
            for (int sn = 0; sn < 2; sn++)
            {
                for (int il = ladder0; il <= ladder1; il++)
                {
                    if (sn == 0)
                    {
                        sprintf(legend, "B%1d L%2d S", barrel, il);
                    }
                    else
                    {
                        sprintf(legend, "B%1d L%2d N", barrel, il);
                    }

                    legendStb->SetTextColor(mcolor[sn][il % 5]);
                    //legendStb->DrawLatex(4800+300*sn,1.85-(il-ladder0)*0.15,legend);
                    legendStb->DrawTextNDC(0.8 + 0.05 * sn, 0.82 - (il - ladder0) * 0.07, legend);
                }
            }

            maxLineStb[arm][ipad]->SetX1(-50);
            maxLineStb[arm][ipad]->SetX2(5450);
            maxLineStb[arm][ipad]->SetY1(1 + rate_range);
            maxLineStb[arm][ipad]->SetY2(1 + rate_range);
            maxLineStb[arm][ipad]->SetLineWidth(3);
            maxLineStb[arm][ipad]->SetLineStyle(2);
            maxLineStb[arm][ipad]->Draw();

            minLineStb[arm][ipad]->SetX1(-50);
            minLineStb[arm][ipad]->SetX2(5450);
            minLineStb[arm][ipad]->SetY1(1 - rate_range);
            minLineStb[arm][ipad]->SetY2(1 - rate_range);
            minLineStb[arm][ipad]->SetLineWidth(3);
            minLineStb[arm][ipad]->SetLineStyle(2);
            minLineStb[arm][ipad]->Draw();
        }
        else
        {
            pmulti[pmindex]->Draw("Lsame");
        }
    }


    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_" << icanvas << " Run@ " << cl->RunNumber() << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[icanvas]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    TC[icanvas]->Update();

    return 0;
}

int SvxPixelMonDraw::DrawLadder(const char *what)
{
    int icanvas = L1EAST;
    unsigned int arm = EAST;
    string cmd(what);

    if ( cmd.find("East") == string::npos ) // if East is NOT found, the arm is West
    {
        icanvas = L1WEST;
        arm = WEST;
    }

    if (! gROOT->FindObject(what))
    {
        MakeCanvas(what);
    }
    TC[icanvas]->Clear("D");

    // fetch histograms
    OnlMonClient *cl = OnlMonClient::instance();
    TH1 *h1chipcount[2][ntype];

    TH1 *vtxp_pars = cl->getHisto("vtxp_params");
    if ( vtxp_pars == 0 )
    {
        DrawDeadServer(transparent[icanvas]);
        TC[icanvas]->Update();
        return -1;
    }
    float nevents = vtxp_pars->GetBinContent(EVENTCOUNTBIN);

    ostringstream name;
    const float yaxis_minimum = 1e-6;
    for (int barrel = 0; barrel < 2; barrel++)
    {
        for (int type = 0; type < ntype; type++)
        {
            name.str("");
            name << "vtxph1chipcount_" << barrel << "_" << arm << "_" << type;
            TH1 *htemp = cl->getHisto(name.str());

            if ( htemp == 0 )
            {
                DrawDeadServer(transparent[icanvas]);
                TC[icanvas]->Update();
                return -1;
            }

            name.str("");
            name << "vtxph1chipcount_" << barrel << "_" << arm << "_" << type << "_clone";
            h1chipcount[barrel][type] = (TH1 *)htemp->Clone(name.str().c_str());

            if ( nevents > 0 && type == 0 )
            {
                h1chipcount[barrel][type]->Scale(1. / nevents);
                h1chipcount[barrel][type]->SetMinimum(yaxis_minimum);
                //h1chipcount[barrel][type]->SetMaximum(NCOL*NROW);   // every pixel fires every event
                h1chipcount[barrel][type]->SetMinimum(yaxis_minimum);
                h1chipcount[barrel][type]->SetMaximum(NCOL * NROW); // every pixel fires every event
            }
        }
    }

    // find the hot and cold chips
    unsigned int deadchip_sum = 0;
    unsigned int hotchip_sum = 0;

    for (unsigned int barrel = 0; barrel < 2; barrel++)
    {
        unsigned int nbin = h1chipcount[barrel][0]->GetNbinsX();
        for (unsigned int ibin = 1; ibin <= nbin; ibin++)
        {
            int ladder = (ibin - 1) / 16;
            if (arm == EAST) ladder += (barrel + 1) * 5;
            int chip = (ibin - 1) % 16;

            double content = h1chipcount[barrel][0]->GetBinContent(ibin);
            if ( content > chip_hitrate_high[barrel] && ig_hot[barrel][ladder][chip] == 0 )
            {
                h1chipcount[barrel][TYPE_HOT]->SetBinContent(ibin, content);
                hotchip_sum++;
            }
            if ( content < chip_hitrate_low[barrel] && ig_dead[barrel][ladder][chip] == 0 )
            {
                h1chipcount[barrel][TYPE_COLD]->SetBinContent(ibin, content);

                if ( content < yaxis_minimum)
                {
                    h1chipcount[barrel][TYPE_DEAD]->SetBinContent(ibin, 2e-6);
                }
                deadchip_sum++;
            }
            // place this here rather than in inside the check on hotness
            // in case a chip goes non-hot and we want to still see that
            // i.e. we think it's hot but it's not.
            if ( ig_hot[barrel][ladder][chip] == 1) //for known hot
            {
                h1chipcount[barrel][TYPE_KNOWNHOT]->SetBinContent(ibin, content);
            }
            if (ig_dead[barrel][ladder][chip] == 1 ) //for known dead/mask
            {
                //Rachid:don't confuse the shift crew with green boxes
                h1chipcount[barrel][TYPE_KNOWNISSUE]->SetBinContent(ibin, content);

                if ( content < yaxis_minimum)
                {
                    h1chipcount[barrel][TYPE_KNOWNISSUE]->SetBinContent(ibin, 2e-6);
                }
            }
        }
    }

    TLine tl;

    static const int color[ntype] = {kGray, kRed, kBlue - 6, kGreen + 1, kPink + 9, kBlue - 6};
    static const int style[ntype] = {1001, 1001, 1001, 1001, 1001, 0};
    TC[icanvas]->Clear("D");
    for (int barrel = 0; barrel < 2; barrel++)
    {
        TP[icanvas][barrel]->cd();
        TP[icanvas][barrel]->SetLogy(1);

        for (int type = 0; type < ntype; type++)
        {
            h1chipcount[barrel][type]->SetFillColor(color[type]);
            h1chipcount[barrel][type]->SetFillStyle(style[type]);
            h1chipcount[barrel][type]->SetStats(0);
            h1chipcount[barrel][type]->SetTickLength(0.);     // artifical axis will handle it

            h1chipcount[barrel][type]->GetYaxis()->SetTitle("N_{Hit}/N_{Event}");
            h1chipcount[barrel][type]->SetTitleOffset(1. / (barrel + 1) + 0.05 * barrel, "Y");
            h1chipcount[barrel][type]->SetTitleSize(0.08, "Y");

            h1chipcount[barrel][type]->SetLabelSize(0., "X");
            if (type == 0)
            {
                h1chipcount[barrel][type]->DrawCopy("");
            }
            else if ( type == TYPE_DEAD )
            {
                for (int ibin = 1; ibin <= h1chipcount[barrel][type]->GetNbinsX(); ibin++)
                {
                    float content = h1chipcount[barrel][type]->GetBinContent(ibin);
                    if ( content < 0.001 ) continue;
                    tl.SetLineWidth(3);
                    tl.SetLineColor(color[type]);
                    tl.SetLineStyle(1);
                    float xval = h1chipcount[barrel][type]->GetBinCenter(ibin);
                    tl.DrawLine(xval, 2 * yaxis_minimum, xval, 1e-4);
                }
                h1chipcount[barrel][type]->SetMarkerStyle(23);
                h1chipcount[barrel][type]->SetMarkerColor(color[TYPE_DEAD]);
                h1chipcount[barrel][type]->DrawCopy("samep");
            }
            else
            {
                h1chipcount[barrel][type]->DrawCopy("same");
            }
        }

        // demarcations between ladders
        int nladder = (barrel == 0) ? 5 : 10;
        tl.SetLineWidth(2);
        tl.SetLineColor(33);
        tl.SetLineStyle(1);
        for (int iladder = 0; iladder < nladder - 1; iladder++)
        {
            tl.DrawLine(16 * iladder + 15.5, 0, 16 * iladder + 15.5, NROW * NCOL);
        }

        // lines showing min/max thresholds on yields/event
        tl.SetLineWidth(1);
        tl.SetLineColor(1);
        tl.SetLineStyle(3);
        double xmin = h1chipcount[barrel][0]->GetXaxis()->GetXmin();
        double xmax = h1chipcount[barrel][0]->GetXaxis()->GetXmax();
        tl.DrawLine(xmin, chip_hitrate_low[barrel], xmax, chip_hitrate_low[barrel]);
        tl.DrawLine(xmin, chip_hitrate_high[barrel], xmax, chip_hitrate_high[barrel]);

        double wmin = -1;
        double wmax = -1;
        if ( arm == WEST && barrel == 0 )
        {
            wmin = -0.5;
            wmax = 4.5;
        }
        else if ( arm == WEST && barrel == 1 )
        {
            wmin = -0.5;
            wmax = 9.5;
        }
        else if ( arm == EAST && barrel == 0 )
        {
            wmin = 4.5;
            wmax = 9.5;
        }
        else if ( arm == EAST && barrel == 1 )
        {
            wmin = 9.5;
            wmax = 19.5;
        }

        TGaxis xax;
        xax.SetTitle("Ladder");
        xax.SetTitleSize(0.08);
        xax.SetTitleOffset(0.7);
        xax.DrawAxis(xmin, 0., xmax, 0., wmin, wmax, wmax - wmin);
    }

    // legend for colors
    TP[icanvas][1]->cd();
    float loy = 0.01;
    float hiy = 0.08;
    //float lox[4]={0.115,0.224,0.359,0.485};
    float lox[5] = {0.115, 0.242, 0.391, 0.549, 0.700};
    float xwid = 0.02;
    TText tex;
    tex.SetTextSize(0.05);
    string label[5] = {"Raw Counts", "Unexpected Hot", "Unexpected Cold", "Known Issue", "KNOWN HOT"};
    for (int ii = 0; ii < 5; ii++)
    {
        if (ladder_box[ii] == 0)
        {
            double hix = lox[ii] + xwid;
            ladder_box[ii] = new TPave(lox[ii], loy, hix, hiy, 0, "NDC");
            ladder_box[ii]->SetFillStyle(1001);
            ladder_box[ii]->SetFillColor(color[ii]);
        }
        ladder_box[ii]->Draw();
        tex.DrawTextNDC(lox[ii] + 0.026, loy + 0.02, label[ii].c_str());
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_" << icanvas
                << " Run@ " << cl->RunNumber()
                << ", Time: " << ctime(&evttime)
                << " Events: " << (int) nevents;
    runstring = runnostream.str();
    transparent[icanvas]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    //create a TText object to denote when thresholds were changed
    TText ThreshLabel;
    ThreshLabel.SetTextFont(62);
    ThreshLabel.SetTextSize(0.02);
    ThreshLabel.SetNDC();  // set to normalized coordinates
    ThreshLabel.SetTextAlign(23); // center/top alignment
    transparent[icanvas]->cd();
    ThreshLabel.DrawText(0.5, 0.95, ThreshText.c_str());

    // Error message
    TP[icanvas][2]->cd();
    if ( ladder_err_msg[arm] == 0 )
    {
        ladder_err_msg[arm] = new TPaveText(0.05, 0.15, 0.95, 0.9, "NDC");
        ladder_err_msg[arm]->SetTextSize(0);
        ladder_err_msg[arm]->SetTextAlign(13);
        ladder_err_msg[arm]->SetBorderSize(0);
        ladder_err_msg[arm]->SetFillColor(0);
    }
    ladder_err_msg[arm]->Clear();

    if ( 12 < hotchip_sum )
    {
        ladder_err_msg[arm]->AddText("Too many hot chips!!");
        ladder_err_msg[arm]->AddText("FEED after the run");
	//        ladder_err_msg[arm]->AddText("Stop the run and FEED");
	ladder_err_msg[arm]->AddText("If status doesn't change after FEED, contact an expert");
        ladder_err_msg[arm]->SetTextColor(color[TYPE_HOT]);
    }
    else if (12 < deadchip_sum)
    {
        ladder_err_msg[arm]->AddText("Too many cold chips!!");
        ladder_err_msg[arm]->AddText("FEED after the run");
	//        ladder_err_msg[arm]->AddText("Stop the run and FEED");
	ladder_err_msg[arm]->AddText("If status doesn't change after FEED, contact an expert");
        ladder_err_msg[arm]->SetTextColor(color[TYPE_COLD]);
    }
    else if ( 0 < hotchip_sum && hotchip_sum <= 12)
    {
        //ladder_err_msg[arm]->AddText("less than 12 hot chips");
        ladder_err_msg[arm]->AddText("FEED after the run");
        ladder_err_msg[arm]->SetTextColor(color[TYPE_HOT]);
    }
    else if ( 0 < deadchip_sum && deadchip_sum <= 12)
    {
        ladder_err_msg[arm]->AddText("FEED after the run");
        ladder_err_msg[arm]->SetTextColor(color[TYPE_COLD]);
    }
    else
    {
        ladder_err_msg[arm]->AddText("OK");
        ladder_err_msg[arm]->SetTextColor(kGreen);
    }
    if (bad_ig_file)
    {
        ladder_err_msg[arm]->AddText("No Dead Map List");
    }
    ladder_err_msg[arm]->Draw();


    TC[icanvas]->Update();

    // clean up
    for (int barrel = 0; barrel < 2; barrel++)
    {
        for (int type = 0; type < ntype; type++)
        {
            delete h1chipcount[barrel][type];
        }
    }

    return 0;
}

int SvxPixelMonDraw::DrawLadderChip(const char *what)
{
    printTimeInterval("start DrawLadderChip");

    int icanvas = L2EAST;
    unsigned int arm = EAST;
    string cmd(what);
    if ( cmd.find("East") == string::npos ) // if East is NOT found, the arm is West
    {
        icanvas = L2WEST;
        arm = WEST;
    }

    // timer writing
    printTimeInterval("make canvas");

    if (! gROOT->FindObject(what))
    {
        MakeCanvas(what);
    }

    // timer writing
    printTimeInterval("make canvas");


    TC[icanvas]->cd();
    TC[icanvas]->Clear("D");

    OnlMonClient *cl = OnlMonClient::instance();
    TH2 *h2map[NBARREL];

    ostringstream name;
    for (int barrel = 0; barrel < 2; barrel++)
    {
        name.str("");
        name << "vtxph2map_" << barrel << "_" << arm;
        h2map[barrel] = (TH2F *)cl->getHisto(name.str().c_str());
        if (! h2map[barrel] )
        {
            DrawDeadServer(transparent[icanvas]);
            TC[icanvas]->Update();
            return -1;
        }
        h2map[barrel]->SetTickLength(0., "x");
        h2map[barrel]->SetTickLength(0., "y");
    }

    // timer writing
    printTimeInterval("hist");


    TString title;
    TText PrintBarrel;
    PrintBarrel.SetTextFont(62);
    PrintBarrel.SetTextSize(0.08);
    PrintBarrel.SetNDC();         // set to normalized coordinates
    PrintBarrel.SetTextAlign(23); // center/top alignment

    TH1 *vtxp_pars = cl->getHisto("vtxp_params");
    float nevents = vtxp_pars->GetBinContent(EVENTCOUNTBIN);
    //  float hotth[2]={50./25.6/4.*nevents,30./12.8/4.*nevents}; // per chip rate
    //float hotth[2]={15./25.6/4.*nevents,7.5/12.8/4.*nevents}; // per chip rate

    // timer writing
    printTimeInterval("param");

    TC[icanvas]->cd();
    for (int barrel = 0; barrel < 2; barrel++)
    {
        TC[icanvas]->cd();
        TP[icanvas][barrel]->cd();
        TP[icanvas][barrel]->cd()->SetLogz();

        h2map[barrel]->SetLabelSize(0., "X");
        h2map[barrel]->SetTitleSize(0., "X");
        h2map[barrel]->SetLabelSize(0., "Y");
        h2map[barrel]->SetTitleSize(0., "Y");
        h2map[barrel]->SetStats(0);
        //h2map[barrel]->SetMaximum(hotth[barrel]);
        h2map[barrel]->Scale(1 / nevents);
        h2map[barrel]->GetZaxis()->SetRangeUser(hitmap_Zmin, hitmap_Zmax);
        h2map[barrel]->Draw("colz");

        int num_ladder = 5;
        if (barrel == 1) num_ladder = 10;
        int num_chip = 16;

        for (int i_ladder = 0; i_ladder < num_ladder; i_ladder++)
        {
            int temp_ladder = i_ladder;
            if (arm == EAST && barrel == 0) temp_ladder = i_ladder + 5;
            if (arm == EAST && barrel == 1) temp_ladder = i_ladder + 10;

            for (int i_chip = 0; i_chip < num_chip; i_chip++)
            {
                /*
                if(ig_dead[barrel][temp_ladder][i_chip] == 1 || ig_hot[barrel][temp_ladder][i_chip] == 1)
                  {
                    float xi = i_ladder * 256;
                    float xf = (i_ladder+1) * 256;
                    float yi = 0;
                    float yf = 0;
                    if(arm == EAST)
                      {
                        yi = (i_chip) * 32;//chip 0 is starts from top
                        yf = (i_chip+1) * 32;
                      }
                    if(arm == WEST)
                      {
                        yi = (num_chip-i_chip) * 32;//chip 0 is starts from top
                        yf = (num_chip-i_chip-1) * 32;
                      }

                    if(ig_dead[barrel][temp_ladder][i_chip] == 1)
                      {
                        deadBox->SetFillColor(kGray+1);
                      }
                    else if(ig_hot[barrel][temp_ladder][i_chip] == 1)
                      {
                        deadBox->SetFillColor(kPink+1);
                      }
                    //deadBox->SetFillStyle(3004);
                    deadBox->DrawBox(xi-0.5,yi-0.5,xf-0.5,yf-0.5);
                  }
                */
            }
        }


        TAxis *yaxis = h2map[barrel]->GetYaxis();
        double xmin = -2.;
        double ymin = yaxis->GetXmin();
        double xmax = -2.;
        double ymax = yaxis->GetXmax();
        TGaxis iz_axis;
        title = "iz_axis_"; title += barrel;
        iz_axis.SetName(title);
        iz_axis.SetTitleOffset(1. / (barrel + 1) + 0.05 * barrel);
        iz_axis.SetTitleSize(0.08);
        iz_axis.SetTitle("Chip");
        iz_axis.SetTickSize(0.);
        iz_axis.SetLineWidth(0.);
        iz_axis.CenterTitle(1);

        if (arm == WEST)
        {
            iz_axis.SetLabelOffset(-0.02 * (2 - barrel));
            iz_axis.DrawAxis(xmin, ymax, xmax - 0.001, ymin, -0.5, 15.5, 16, "BS");
        }
        else
        {
            iz_axis.SetLabelOffset(0.02 * (2 - barrel));
            iz_axis.DrawAxis(xmin, ymin, xmax, ymax, -0.5, 15.5, 16, "BS");
        }


        TAxis *xaxis = h2map[barrel]->GetXaxis();
        xmin = xaxis->GetXmin();
        ymin = -0.5;
        xmax = xaxis->GetXmax();
        ymax = -0.5;
        double wmin = -1;
        double wmax = -1;
        if ( arm == WEST && barrel == 0 )
        {
            wmin = -0.5;
            wmax = 4.5;
        }
        if ( arm == WEST && barrel == 1 )
        {
            wmin = -0.5;
            wmax = 9.5;
        }
        if ( arm == EAST && barrel == 0 )
        {
            wmin = 4.5;
            wmax = 9.5;
        }
        if ( arm == EAST && barrel == 1 )
        {
            wmin = 9.5;
            wmax = 19.5;
        }
        TGaxis iy_axis;
        iy_axis.SetTitle("Ladder");
        iy_axis.SetTickSize(0.);
        iy_axis.SetLineWidth(0.);
        iy_axis.SetTitleSize(0.08);
        iy_axis.SetTitleOffset(0.7);
        iy_axis.DrawAxis(xmin, ymin, xmax, ymax, wmin, wmax, wmax - wmin);

        title = "Barrel: "; title += barrel;
        PrintBarrel.DrawText(0.32 / (barrel + 1), 0.965, title);
    }

    // timer writing
    printTimeInterval("drawhit");

    // diagnostic information
    TP[icanvas][2]->cd();
    TText PrintDiag;
    PrintDiag.SetTextFont(62);
    PrintDiag.SetTextSize(0.12);
    PrintDiag.SetNDC();         // set to normalized coordinates
    PrintDiag.SetTextAlign(13); //
    ostringstream diagstream;
    diagstream << "Vtx-Pixel " << arm_desc[arm];
    PrintDiag.DrawText(0.2, 1., diagstream.str().c_str());
    TC[icanvas]->Range(0, 0, 1, 1);

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); //
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");

    // fill run number and event time into string
    runnostream << ThisName << "_" << icanvas << " Run@ " << cl->RunNumber()
                << ", Time: " << ctime(&evttime)
                << " Events: " << (int) nevents;
    runstring = runnostream.str();
    transparent[icanvas]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    // timer writing
    printTimeInterval("diag");

    TC[icanvas]->Update();


    // timer writing
    printTimeInterval("can update");

    return 0;
}

int SvxPixelMonDraw::DrawDeadMap(const char *what)
{

    printTimeInterval("start DrawDeadMap");

    int icanvas = DEADEAST;
    unsigned int arm = EAST;
    string cmd(what);
    if ( cmd.find("East") == string::npos ) // if East is NOT found, the arm is West
    {
        icanvas = DEADWEST;
        arm = WEST;
    }

    if (! gROOT->FindObject(what))
    {
        MakeCanvas(what);
    }

    // timer writing
    printTimeInterval("make canvas");


    OnlMonClient *cl = OnlMonClient::instance();
    TH2 *h2map[NBARREL][NLADDER][NCHIP * 2];

    // get the number of events
    TH1 *vtxp_pars = cl->getHisto("vtxp_params");
    if (vtxp_pars == 0)
    {
        DrawDeadServer(transparent[icanvas]);
        TC[icanvas]->Update();
        std::cout << "SvxPixelMonDraw::DrawDeadMap - No vtxp_params" << std::endl;
        return -1;
    }

    float nevents = vtxp_pars->GetBinContent(EVENTCOUNTBIN);
    if ( nevents < 1e3 ) //not worth the effor if < 1k events
    {

        TC[icanvas]->cd();
        TC[icanvas]->Clear("D");
        TP[icanvas][2]->cd();
        TText tevent;
        tevent.SetTextAlign(23);
        tevent.SetTextFont(62);
        tevent.SetTextSize(0.04);
        tevent.SetNDC();
        tevent.DrawText(0.5, 1., Form("Only %.0f events, waiting for >1k events.", nevents));
        //DrawDeadServer(transparent[icanvas]);
        TC[icanvas]->Update();
        std::cout << "SvxPixelMonDraw::DrawDeadMap - less than 1k events(" << nevents << ")." << std::endl;

        return 0;
    }


    //keep track of the percent of pixels with each status
    // 0 : good
    // 1 : dead
    // 2 : cold
    // 3 : hot
    float perc_status[4] = {0};
    float npixels = 0;

    // fetch histograms
    ostringstream name;
    //spiro = packetID-1
    //mapping
    //spiro  Barrel   Ladder   South/North      West/East
    //00-04     0       5-9          N              E
    //05-14     1      10-19         N              E
    //15-19     0       5-9          S              E
    //20-29     1      10-19         S              E
    //30-34     0       0-4          N              W
    //35-44     1       0-9          N              W
    //45-49     0       0-4          S              W
    //50-59     1       0-9          S              W

    unsigned int lo_spiro = (arm == WEST) ? 30 :  0;
    unsigned int hi_spiro = (arm == WEST) ? 60 : 30;
    static const int SOUTH = 0;
    static const int NORTH = 1;

    for (unsigned int spiro = lo_spiro; spiro < hi_spiro; spiro++)
    {
        int barrel = (spiro % 15 <= 4) ? 0 : 1;

        int south_north = ((spiro / 15) % 2 == 0) ? NORTH : SOUTH;
        int ladder;
        if      (spiro <= 14)
        {
            ladder = spiro + 5;
        }
        else if (spiro <= 29)
        {
            ladder = spiro - 10;
        }
        else if (spiro <= 34)
        {
            ladder = spiro - 30;
        }
        else if (spiro <= 44)
        {
            ladder = spiro - 35;
        }
        else if (spiro <= 49)
        {
            ladder = spiro - 45;
        }
        else
        {
            ladder = spiro - 50;
        }

        for (int chip = 0; chip < NCHIP; chip++)
        {
            name.str("");
            name << "vtxp_chipmap_" << spiro << "_" << chip;
            TH2 *htemp = (TH2 *)cl->getHisto(name.str());
            if ( htemp == NULL )
            {
                DrawDeadServer(transparent[icanvas]);
                TC[icanvas]->Update();
                std::cout << "SvxPixelMonDraw::DrawDeadMap - No " << name.str().c_str()
                          << " " << spiro << " " << chip
                          << std::endl;

                return -1;
            }

            int ichip = chip + (south_north * NCHIP);

            name << "_clone";
            h2map[barrel][ladder][ichip] = (TH2 *) htemp->Clone(name.str().c_str());

            //now get the pixel status
            for (int ibinx = 1; ibinx <= h2map[barrel][ladder][ichip]->GetNbinsX(); ibinx++)
            {
                for (int ibiny = 1; ibiny <= h2map[barrel][ladder][ichip]->GetNbinsY(); ibiny++)
                {
                    float avg = h2map[barrel][ladder][ichip]->GetBinContent(ibinx, ibiny);
                    avg = avg / nevents;
                    int status = GetCountStatus(avg, barrel);

                    h2map[barrel][ladder][ichip]->SetBinContent(ibinx, ibiny, status);

                    //count this status
                    if (status == 0)
                        perc_status[0]++;
                    else if (status == 1)
                        perc_status[1]++;
                    else if (status == 9)
                        perc_status[2]++;
                    else if (status == 11)
                        perc_status[3]++;
                    else
                    {
                        std::cout << "SvxPixelMonDraw::DrawDeadMap - what status? " << status << std::endl;
                    }
                    npixels++;
                }
            }

            h2map[barrel][ladder][ichip]->SetTickLength(0., "x");
            h2map[barrel][ladder][ichip]->SetTickLength(0., "y");
            h2map[barrel][ladder][ichip]->GetZaxis()->SetRangeUser(0, 12);
            h2map[barrel][ladder][ichip]->SetStats(0);
            h2map[barrel][ladder][ichip]->SetTitle("");
        }


    }

    // timer writing
    printTimeInterval("make hists");


    TC[icanvas]->cd();
    TC[icanvas]->Clear("D");

    Int_t palette[6];
    palette[0] = kBlack;//dead
    palette[1] = kAzure - 2; //cold
    palette[2] = kGreen + 2; //unstable
    palette[3] = kRed;//hot
    palette[4] = kBlue + 2; //verycold
    palette[5] = kRed + 2; //veryhot
    gStyle->SetPalette(6, palette);

    TText label;
    label.SetTextFont(63);
    label.SetTextAlign(22);
    label.SetTextSize(10);
    label.SetNDC();

    for (int barrel = 0; barrel < 2; barrel++)
    {
        TC[icanvas]->cd();
        TP[icanvas][barrel]->cd();


        int num_ladder = 5;
        if (barrel == 1) num_ladder = 10;
        int num_chip = 16;

        TP[icanvas][barrel]->Divide(num_chip + 1, num_ladder + 1, 0.001, 0.001);

        for (int i_ladder = 0; i_ladder < num_ladder; i_ladder++)
        {
            int temp_ladder = i_ladder;
            if (arm == EAST && barrel == 0) temp_ladder = i_ladder + 5;
            if (arm == EAST && barrel == 1) temp_ladder = i_ladder + 10;

            for (int i_chip = 0; i_chip < num_chip + 1; i_chip++)
            {
                int pad = i_chip + (num_chip + 1) * i_ladder + 1;

                TPad *tmp = (TPad *) TP[icanvas][barrel]->GetPrimitive(Form("SvxPixelMonPad%i_%i_%i", icanvas, barrel, pad));
                if (!tmp)
                {
                    std::cout << "SvxPixelMonDraw::DrawDeadMap - hmm, no pad "
                              << icanvas << " " << barrel << " " << pad
                              << std::endl;
                    continue;
                }

                tmp->SetTopMargin(0.0);
                tmp->SetRightMargin(0.0);
                tmp->SetBottomMargin(0.0);
                tmp->SetLeftMargin(0.0);

                tmp->cd();

                if (i_chip == num_chip)
                {
                    label.DrawText(0.5, 0.5, Form("B%iL%i", barrel, temp_ladder));
                }
                else
                {
                    h2map[barrel][temp_ladder][i_chip]->Draw("col");
                }

            }
        }

    }

    // timer writing
    printTimeInterval("drawhit");


    //print the percentage of each status
    TP[icanvas][2]->cd();

    TText tperc;
    tperc.SetTextAlign(12);
    tperc.SetTextFont(62);
    tperc.SetTextSize(0.08);
    tperc.SetNDC();

    // good
    tperc.SetTextColor(kGray + 2);
    tperc.DrawText(0.25, 0.7, Form("Good Pixels : %.2f%%", perc_status[0] / npixels * 100.));

    // dead
    tperc.SetTextColor(palette[0]);
    tperc.DrawText(0.25, 0.6, Form("Dead Pixels : %.2f%%", perc_status[1] / npixels * 100.));

    // cold
    tperc.SetTextColor(palette[1]);
    tperc.DrawText(0.25, 0.5, Form("Cold pixels : %.2f%%", perc_status[2] / npixels * 100.));

    // hot
    tperc.SetTextColor(palette[3]);
    tperc.DrawText(0.25, 0.4, Form("Hot pixels : %.2f%%", perc_status[3] / npixels * 100.));


    // diagnostic information
    TP[icanvas][2]->cd();
    TText PrintDiag;
    PrintDiag.SetTextFont(62);
    PrintDiag.SetTextSize(0.12);
    PrintDiag.SetNDC();         // set to normalized coordinates
    PrintDiag.SetTextAlign(13); //
    ostringstream diagstream;
    diagstream << "Vtx-Pixel " << arm_desc[arm];
    PrintDiag.DrawText(0.2, 1., diagstream.str().c_str());
    TC[icanvas]->Range(0, 0, 1, 1);

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); //
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");

    // fill run number and event time into string
    runnostream << ThisName << "_" << icanvas << " Run@ " << cl->RunNumber()
                << ", Time: " << ctime(&evttime)
                << " Events:" << (int) nevents;
    runstring = runnostream.str();
    transparent[icanvas]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    // timer writing
    printTimeInterval("diag");

    TC[icanvas]->Update();


    // timer writing
    printTimeInterval("can update");

    return 0;
}


int SvxPixelMonDraw::DrawDeadServer(TPad *transparent)
{
    transparent->cd();
    TText FatalMsg;
    FatalMsg.SetTextFont(62);
    FatalMsg.SetTextSize(0.1);
    FatalMsg.SetTextColor(4);
    FatalMsg.SetNDC();  // set to normalized coordinates
    FatalMsg.SetTextAlign(23); // center/top alignment
    FatalMsg.DrawText(0.5, 0.9, "MY MONITOR");
    FatalMsg.SetTextAlign(22); // center/center alignment
    FatalMsg.DrawText(0.5, 0.5, "SERVER");
    FatalMsg.SetTextAlign(21); // center/bottom alignment
    FatalMsg.DrawText(0.5, 0.1, "DEAD");
    transparent->Update();
    return 0;
}

int SvxPixelMonDraw::MakePS(const char *what)
{
    OnlMonClient *cl = OnlMonClient::instance();
    ostringstream filename;
    int iret = Draw(what);
    if (iret) // on error no ps files please
    {
        return iret;
    }
    filename << ThisName << "_hitchip_" << cl->RunNumber() << ".ps";
    TC[0]->Print(filename.str().c_str());
    filename.str("");

    return 0;
}

int SvxPixelMonDraw::MakeHtml(const char *what)
{
    //Draw the standard DM plots
    int iret = Draw(what);

    //Draw the expert plots
    iret += Draw("SvxPixelDeadMapWest");
    iret += Draw("SvxPixelDeadMapEast");

    if (iret) // on error no html output please
    {
        return iret;
    }

    OnlMonClient *cl = OnlMonClient::instance();

    ostringstream cavid;
    for (int icanvas = 0; icanvas < ncanvas; icanvas++)
    {
        if (icanvas == CHIPMAP)                   continue; // chipmap canvas is special
        cavid.str("");
        cavid << icanvas;
        if (TC[icanvas])
        {
            string pngfile = cl->htmlRegisterPage(*this, canv_short[icanvas], cavid.str(), "png");
            cl->CanvasToPng(TC[icanvas], pngfile);
        }
    }

    // TC[CHIPMAP] is a temporary container for whichever half-ladder you are interested in
    // at that particular moment.
    // therefore, loop over each packet redrawing on this canvas
    // but save the content each time.
    ostringstream path;
    for (int packetid = vtxp_packet_base; packetid < vtxp_packet_base + vtxp_npacket; packetid++)
    {
        cavid.str("");
        cavid << CHIPMAP << "_" << packetid;
        path.str("");
        path << "HalfLadder/" << packetid;
        string pngfile = cl->htmlRegisterPage(*this, path.str(), cavid.str(), "png");
        DrawHalfLadder(packetid);
        cl->CanvasToPng(TC[CHIPMAP], pngfile);
    }

    return 0;
}

int SvxPixelMonDraw::read_stability_file()
{
    ostringstream ratefilename;
    if ( getenv("ONLMON_CALIB") )
    {
        ratefilename << getenv("ONLMON_CALIB") << "/";
    }
    ratefilename << "SvxPixelNominalRate.txt";

    ifstream ratefile(ratefilename.str().c_str(), ifstream::in);
    if (! ratefile)
    {
        cout << "Failed to read rate map parameters from " << ratefilename.str().c_str() << endl;
        return -3;
    }
    cout << endl << "Reading rate map parameters from " << ratefilename.str().c_str() << endl;

    string rateline;
    while ( (getline(ratefile, rateline)) )
    {
        if ( rateline[0] == '#' || rateline[0] == '/' ) continue;
        istringstream iss_line(rateline.data());
        int spiro;
        float rate;
        iss_line >> spiro;
        iss_line >> rate;
        if (rate == 0) rate = 1;
        m_stability_rate[spiro] = rate;
    }
    ratefile.close();

    return 0;
}

int SvxPixelMonDraw::GetCountStatus(float avg, int lyr)
{
    //int ichip = chip+ladder*chip;

    float cold_thresh = 0;
    float hot_thresh = 1;

    if (lyr == 0)
    {
        cold_thresh = 2.4e-04;
        hot_thresh = 7.2e-03;
    }
    else if (lyr == 1)
    {
        cold_thresh = 7.05e-05;
        hot_thresh = 2.1e-03;
    }
    else
    {
        cout << "SvxPixelMonDraw::GetCountStatus - lyr="
             << lyr << " is out of bonds, must be 0 || 1."
             << endl;
        return 1; //dead
    }


    //designate the status
    if (avg == 0)
    {
        return 1; //dead
    }
    else if (avg < cold_thresh)
    {
        return 9; //cold
    }
    else if (avg > hot_thresh)
    {
        return 11; //hot
    }
    else
    {
        return 0; //good
    }

    /*
      float middle = 0;
      float avg_dist_low_fit_bound_B0 = 0;
      float avg_dist_high_fit_bound_B0 = 0;

      float avg_dist_low_fit_bound_B1 = 0;
      float avg_dist_high_fit_bound_B1 = 0;

      if (lyr == 0)
      {
          avg_dist_low_fit_bound_B0 = 0.00075;
          avg_dist_high_fit_bound_B0 = 0.00165;

          middle = (avg_dist_low_fit_bound_B0 + avg_dist_high_fit_bound_B0) / 2.0;
      }
      else if (lyr == 1)
      {
          avg_dist_low_fit_bound_B1 = 1.8e-4;
          avg_dist_high_fit_bound_B1 = 5.25e-4;

          middle = (avg_dist_low_fit_bound_B1 + avg_dist_high_fit_bound_B1) / 2.0;
      }
      if (avg == 0)
          return 1; //dead
      else if (avg < middle / 10.0)
          return 9; //cold
      else if (avg > middle * 3.0)
          return 11; //hot
      else return 0; //good
    */
}
