#include "SvxStripMonDraw.h"
#include "SvxStripMonDefs.h"

#include <OnlMonClient.h>
#include <OnlMonDB.h>

#include <phool.h>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TPave.h>
#include <TMarker.h>
#include <TBox.h>

#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>

#include <cmath>
#include <sstream>
#include <sys/time.h>

using namespace std;

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

const int ADC = 0;
const int LIVE = 1;
const int SIZE = 2;
const int LADDER = 3;
const int NORMRCC = 4;
const int CELLIDW = 5;
const int CELLIDE = 6;
const int CELLID2 = 7;
const int BCLOCK = 8;
const int PACNT = 9;
const int ERRORS = 10;
const int RCCBCLOCKERROR = 11;
const int RCCBCLOCKE = 12;
const int RCCBCLOCKW = 13;

SvxStripMonDraw::SvxStripMonDraw(const char *name):
  OnlMonDraw(name),
  rcc_msg(0),
  rcc_msg_clock(0)
{

  memset(ig_clockissue,0,sizeof(ig_clockissue));
  memset(ig_hot,0,sizeof(ig_hot));
  memset(ig_dead,0,sizeof(ig_dead));
  memset(rcc_hitrate_low,0,sizeof(rcc_hitrate_low));
  memset(rcc_hitrate_high,0,sizeof(rcc_hitrate_high));
  memset(charge_range,0,sizeof(charge_range));
  memset(multi_range,0,sizeof(multi_range));
  memset(TC,0,sizeof(TC));
  memset(transparent,0,sizeof(transparent));
  memset(TP,0,sizeof(TP));
  memset(ladder_box,0,sizeof(ladder_box));

  canv_name[ADC] = "SvxStripMonADC";
  canv_name[SIZE] = "SvxStripMonSize";
  canv_name[LIVE] = "SvxStripMonLive";
  canv_name[LADDER] = "SvxStripMonLadder";
  canv_name[NORMRCC] = "SvxStripMonNormRCC";
  canv_name[CELLIDW] = "SvxStripMonCellIDWest";
  canv_name[CELLIDE] = "SvxStripMonCellIDEast";
  canv_name[CELLID2] = "SvxStripMonCellID2";
  canv_name[BCLOCK] = "SvxStripMonBClock";
  canv_name[RCCBCLOCKE] = "SvxStripMonRCCBClockEast";
  canv_name[RCCBCLOCKW] = "SvxStripMonRCCBClockWest";
  canv_name[RCCBCLOCKERROR] = "SvxStripMonRCCBClockError";
  canv_name[PACNT] = "SvxStripMonPACnt";
  canv_name[ERRORS] = "SvxStripMonErrors";

  canv_desc[ADC] = "SvxStripMon_Ladder_ADC";
  canv_desc[SIZE] = "SvxStripMon_ADC_BBC_by_Time";
  canv_desc[LIVE] = "SvxStripMon_Detector_Ladder_Actitivty";
  canv_desc[LADDER] = "SvxStripMon_Ladder_Detail_ADC";
  canv_desc[NORMRCC] = "SvxStripMon_Normalized_RCC_Yield";
  canv_desc[CELLIDW] = "SvxStripMon_CellID_RMS_TIME_West";
  canv_desc[CELLIDE] = "SvxStripMon_CellID_RMS_TIME_East";
  canv_desc[CELLID2] = "SvxStripMon_CellID";
  canv_desc[BCLOCK] = "SvxStripMon_BClock";
  canv_desc[RCCBCLOCKE] = "SvxStripMon_RCCBClock_East";
  canv_desc[RCCBCLOCKW] = "SvxStripMon_RCCBClock_West";
  canv_desc[RCCBCLOCKERROR] = "SvxStripMon_RCCBClock_Error";
  canv_desc[PACNT] = "SvxStripMon_PACnt";
  canv_desc[ERRORS] = "SvxStripMon_Errors";

  canv_short[ADC] = "ADC";
  canv_short[SIZE] = "Mean Multiplicity";
  canv_short[LIVE] = "Live";
  canv_short[LADDER] = "";
  canv_short[NORMRCC] = "Normalized Yield";
  canv_short[CELLIDW] = "CellW";
  canv_short[CELLIDE] = "CellE";
  canv_short[CELLID2] = "CellID";
  canv_short[BCLOCK] = "BClock";
  canv_short[RCCBCLOCKE] = "RCCBClockE";
  canv_short[RCCBCLOCKW] = "RCCBClockW";
  canv_short[RCCBCLOCKERROR] = "RCCBClockError";
  canv_short[PACNT] = "PACnt";
  canv_short[ERRORS] = "Errors";

  npad[ADC] = 1;
  npad[LIVE] = 1;
  npad[SIZE] = 2;
  npad[LADDER] = 6;
  npad[NORMRCC] = 3;
  npad[CELLIDW] = 20;
  npad[CELLIDE] = 20;
  npad[CELLID2] = 40;
  npad[BCLOCK] = 40;
  npad[RCCBCLOCKE] = 20;
  npad[RCCBCLOCKW] = 20;
  npad[RCCBCLOCKERROR] = 3;
  npad[PACNT] = 40;
  npad[ERRORS] = 8;

  return ;
}

SvxStripMonDraw::~SvxStripMonDraw()
{
    delete rcc_msg;
    delete rcc_msg_clock;
    for (int ii = 0; ii < 5; ii++)
    {
        delete ladder_box[ii];
    }
}

int SvxStripMonDraw::Init()
{
    //Initialize the cold/hot RCC limits
    //NOTE: This must be changed for each run/species/energy

    //Updated for Run 16 dAu @ 20 GeV 6/1/2016 - kurthill
	  rcc_hitrate_low[0]  = 1e-1;
	  rcc_hitrate_high[0] = 1e2;
  
	  rcc_hitrate_low[1]  = 1e-1;
	  rcc_hitrate_high[1] = 1e2;
  
	  ThreshText = "Cold/Hot thresholds set for Run 16 d+Au 20GeV - 06/01/2016"; 
  
    //Set the ranges used in the multiplicity plots
    //drawn on SvxStripMonSize

    //Updated for Run 16 dAu @ 62 GeV 5/21/2016 - D. McGlinchey
    charge_range[0] = 50e3; //lower Summed ADC/NBBC limit
    charge_range[1] = 175e3; //upper Summed ADC/NBBC limit

    multi_range[0] = 350; //lower Multiplicity limit
    multi_range[1] = 20000; //upper Multiplicity limit

    MultiText = "Limits set for Run 16 d+Au 20GeV - 06/01/2016";


    // Get a list of RCCs with known isues from
    // $ONLMON_CALIB.
    // This includes 2 lists:
    // ig_dead : list of known dead RCCs
    // ig_hot  : list of known hot RCCs
    // First - 0 the arrays
    memset(ig_hot,0,sizeof(ig_hot));
    memset(ig_clockissue,0,sizeof(ig_clockissue));
    memset(ig_dead,0,sizeof(ig_dead));
    read_config_file(ig_hot, "SvxStripKnownHotMap.txt");
    read_config_file(ig_clockissue, "SvxStripKnownClockIssue.txt");
    read_config_file(ig_dead, "SvxStripDeadMap.txt");


    return 0;
}

int SvxStripMonDraw::MakeCanvas(const char *name)
{
    OnlMonClient *cl = OnlMonClient::instance();

    int xsize = cl->GetDisplaySizeX();
    int ysize = cl->GetDisplaySizeY();

    ostringstream tname;

    for (int icanvas = 0; icanvas < ncanvas; icanvas++)
    {
        if ( strcmp(name, "ALL") && strcmp(name, canv_name[icanvas].c_str()) ) continue;

        // CANVAS
        TC[icanvas] = new TCanvas(canv_name[icanvas].c_str(), canv_desc[icanvas].c_str(),
                                  -1, 0, 7. / 8.*xsize, 3. / 4.*ysize);
        TC[icanvas]->SetBorderSize(0);
        TC[icanvas]->SetBottomMargin(0.);
        TC[icanvas]->SetFrameBorderSize(0);
        gSystem->ProcessEvents();

        // PAD(s)
        for (int ipad = 0; ipad < npad[icanvas]; ipad++)
        {
            tname.str("");
            tname << "SvxStripMonPad" << icanvas << "_" << ipad;
            if (npad[icanvas] == 1)
            {
                TP[icanvas][ipad] = new TPad(tname.str().c_str(), "", 0.05, 0.05, 0.95, 0.9, 0.);
            }
            else
            {
                if (icanvas == SIZE)
                {
                    int ny = 2;
                    //int nx=1;
                    float xlo = 0.05;
                    float ylo = 0.05;
                    float xhi = 0.95;
                    float yhi = 0.90;
                    //float xpad=0.02;
                    float ypad = 0.02;

                    //float xsize=(xhi-xlo)/nx;
                    float ysize = (yhi - ylo - ypad) / ny;

                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 xlo, ylo + ipad * (ypad + ysize),
                                                 xhi, ylo + ipad * (ypad + ysize) + ysize,
                                                 0.);
                }
                if (icanvas == LADDER)
                {
                    int nx = 3;
                    int ny = 2;
                    float xlo = 0.05;
                    float xhi = 0.95;
                    float ylo = 0.05;
                    float yhi = 0.90;
                    float xpad = 0.02;
                    float ypad = 0.02;

                    float xsize = (xhi - xlo - 2 * xpad) / nx;
                    float ysize = (yhi - ylo - 2 * ypad) / ny;

                    int ix = ipad % 3;
                    int iy = ipad / 3;
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 xlo + ix * (xpad + xsize),
                                                 ylo + iy * (ypad + ysize),
                                                 xlo + ix * (xpad + xsize) + xsize,
                                                 ylo + iy * (ypad + ysize) + ysize,
                                                 0.);
                }

                if ( icanvas == NORMRCC )
                {
                    const double lox[3] = { 0.00 , 0.00 , 0.70 };
                    const double loy[3] = { 0.45 , 0.00 , 0.45 };
                    const double hix[3] = { 0.70 , 1.00 , 1.00 };
                    const double hiy[3] = { 0.90 , 0.45 , 0.90 };
                    TC[icanvas]->SetBottomMargin(0.);
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lox[ipad], loy[ipad], hix[ipad], hiy[ipad], 0);
                    if (ipad == 0 || ipad == 1)
                    {
                        TC[icanvas]->cd();
                        TP[icanvas][ipad]->SetBottomMargin(0.15);
                        TP[icanvas][ipad]->SetRightMargin(0.02);
                        double lmargin = ( ipad == 0 ) ? 0.1 / hix[0] : 0.10;
                        TP[icanvas][ipad]->SetLeftMargin(lmargin);
                    }
                }

                if ( icanvas == RCCBCLOCKERROR )
                {
                    const double lox[3] = { 0.00 , 0.00 , 0.70 };
                    const double loy[3] = { 0.45 , 0.00 , 0.45 };
                    const double hix[3] = { 0.70 , 1.00 , 1.00 };
                    const double hiy[3] = { 0.90 , 0.45 , 0.90 };
                    TC[icanvas]->SetBottomMargin(0.);
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lox[ipad], loy[ipad], hix[ipad], hiy[ipad], 0);
                    if (ipad == 0 || ipad == 1)
                    {
                        TC[icanvas]->cd();
                        TP[icanvas][ipad]->SetBottomMargin(0.15);
                        TP[icanvas][ipad]->SetRightMargin(0.02);
                        double lmargin = ( ipad == 0 ) ? 0.1 / hix[0] : 0.10;
                        TP[icanvas][ipad]->SetLeftMargin(lmargin);
                    }
                }

                if ( icanvas == CELLIDW || icanvas == CELLIDE )
                {
                    double lowx = 0.25 * (ipad % 4);
                    double lowy = 0.77 - 0.18 * (int) (ipad / 4);
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lowx, lowy, lowx + 0.25, lowy + 0.18, 0);
                }


                if ( icanvas == RCCBCLOCKW || icanvas == RCCBCLOCKE )
                {
                    double lowx = 0.25 * (ipad % 4);
                    double lowy = 0.77 - 0.18 * (int) (ipad / 4);
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lowx, lowy, lowx + 0.25, lowy + 0.18, 0);
                }

                if ( icanvas == CELLID2 )
                {
                    double lowx = 0.125 * (ipad % 8);
                    double lowy = 0.8 - 0.2 * (int) (ipad % 5);
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lowx, lowy, lowx + 0.125, lowy + 0.2, 0);
                }

                if ( icanvas == BCLOCK )
                {
                    double lowx = 0.125 * (ipad % 8);
                    double highx = lowx + 0.125;
                    double lowy = (0.8 - 0.2 * (int) (ipad / 8)) * 0.97;
                    double highy = (lowy + 0.2) * 0.97;
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lowx, lowy, highx, highy, 0);
                    TP[icanvas][ipad]->SetBottomMargin(0.05);
                    TP[icanvas][ipad]->SetRightMargin(0.02);
                    TP[icanvas][ipad]->SetLeftMargin(0.10);
                }

                if ( icanvas == PACNT )
                {
                    double lowx = 0.125 * (ipad % 8);
                    double highx = lowx + 0.125;
                    double lowy = (0.8 - 0.2 * (int) (ipad / 8)) * 0.97;
                    double highy = (lowy + 0.2) * 0.97;
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lowx, lowy, highx, highy, 0);
                    TP[icanvas][ipad]->SetTopMargin(0.05);
                    TP[icanvas][ipad]->SetBottomMargin(0.05);
                    TP[icanvas][ipad]->SetRightMargin(0.01);
                    TP[icanvas][ipad]->SetLeftMargin(0.08);
                }

                if ( icanvas == ERRORS )
                {
                    double lowx = 0.5 * (ipad % 2);
                    double highx = lowx + 0.5;
                    double lowy = (0.75 - 0.25 * (int) (ipad / 2)) * 0.97;
                    double highy = (lowy + 0.25) * 0.97;
                    TP[icanvas][ipad] = new TPad(tname.str().c_str(), "",
                                                 lowx, lowy, highx, highy, 0);
                    TP[icanvas][ipad]->SetBottomMargin(0.05);
                    TP[icanvas][ipad]->SetRightMargin(0.02);
                    TP[icanvas][ipad]->SetLeftMargin(0.10);
                }
            }
            TC[icanvas]->cd();
            TP[icanvas][ipad]->Draw();
        }

        // TRANSPARENT
        tname.str("");
        tname << "SvxStripMonTrans" << icanvas;
        transparent[icanvas] = new TPad(tname.str().c_str(), "this does not show", 0, 0, 1, 1);
        transparent[icanvas]->SetFillStyle(4000);
        transparent[icanvas]->Draw();
    }

    return 0;
}

int SvxStripMonDraw::Draw(const char *what)
{

    initTimeInterval();

    int iret = 0;
    int idraw = 0;
    string cmd(what);
    
    //Plots seen by DM on POMS
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[LIVE]))
    {    
        iret += DrawLive(what);
        idraw++;
    }
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[NORMRCC]))
    {    
        iret += DrawNormRCC(what);
        idraw++;
    }
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[RCCBCLOCKERROR]))
    {    
        iret += DrawRCCBClockError(what);
        idraw++;
    }
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[CELLIDW]))
    {    
        iret += DrawCellID(what, 0);
        idraw++;
    }
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[CELLIDE]))
    {
        iret += DrawCellID(what, 1);
        idraw++;
    }
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[RCCBCLOCKW]))
    {
        iret += DrawRCCBClock(what, 0);
        idraw++;
    }
    if ((cmd == "ALL") || (cmd == "EXPERT") || (cmd == canv_name[RCCBCLOCKE]))
    {
        iret += DrawRCCBClock(what, 1);
        idraw++;
    }


    //Not drawn by POMS
    if ((cmd == "EXPERT") || (cmd == canv_name[ADC]))
    {
        iret += DrawADC(what);
        idraw++;
    }
    if ((cmd == "EXPERT") || (cmd == canv_name[SIZE]))
    {
        iret += DrawSize(what);
        idraw++;
    }
    if ((cmd == "EXPERT") || (cmd == canv_name[BCLOCK]))
    {
        iret += DrawBClock(what);
        idraw++;
    }
    if ((cmd == "EXPERT") || (cmd == canv_name[PACNT]))
    {
        iret += DrawPACnt(what);
        idraw++;
    }
    if ((cmd == "EXPERT") || (cmd == canv_name[ERRORS]))
    {
        iret += DrawErrors(what);
        idraw++;
    }

    if (cmd == canv_name[CELLID2])
    {
        iret += DrawCellID2(what, 1);
        idraw++;
    }
    if (cmd.find("VTXS%") != string::npos )
    {
        string packet_s = cmd.substr(cmd.find("VTXS%") + 5, cmd.size());
        int packet_i = atoi(packet_s.c_str());
        iret += DrawLadder(packet_i);
        idraw++;
    }
    if (!idraw)
    {
        cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
        cout << " Your options are:" << endl;
        for (int i = 0; i < ncanvas; ++i)
            cout << canv_name[i] << endl;
        iret = -1;
    }
    return iret;
}

int SvxStripMonDraw::read_config_file(int data[4][24][6], const string &fname)
{
    const char *calibdir = getenv("ONLMON_CALIB");
    ostringstream parfile("");
    if (calibdir)
    {
        parfile << calibdir << "/";
    }
    else
    {
        cerr << "SvxStripMonDraw::read_config_file Error, "
             << "$ONLMON_CALIB environment variable not set!\n"
             << "Will try reading locally.";
    }
    parfile << fname;

    ifstream infile(parfile.str().c_str(), ifstream::in);
    if (! infile)
    {
        cout << "Could not read file: " << parfile.str() << endl;
        return -1;
    }
    cout << "Reading parameters from " << parfile.str() << endl;
    string line;
    int barrel, ladder, chip;//chip is really rcc
    while ( getline(infile, line) )
    {
        if ( line[0] == '#' || line[0] == '/' ) continue;
        istringstream iss_line(line.data());
        iss_line >> barrel >> ladder >> chip;
        if ( barrel >= 4 || barrel < 2 || ladder < 0 || ladder >= 24 || chip < 0 || chip > 6 ||
                ( barrel == 2 && ladder >= 16 ) ||
                ( barrel == 2 && chip > 5 )
           )
        {
            cerr << "Invalid input line: " << line << endl;
            continue;
        }
        data[barrel][ladder][chip] = 1;
    }
    return 0;
}


int SvxStripMonDraw::DrawNormRCC(const char *what)
{
    printTimeInterval("DrawNormRCC - Start");
    
    int icanvas = NORMRCC;

    if (! gROOT->FindObject("SvxStripMonNormRCC"))
    {
        MakeCanvas("SvxStripMonNormRCC");
    }
    TC[icanvas]->Clear("D");

    // fetch histograms
    OnlMonClient *cl = OnlMonClient::instance();
    TH1 *vtxs_pars = cl->getHisto("vtxs_params");

    ostringstream name;

    TH3 *h3temp = (TH3 *) cl->getHisto("vtxsh1rcccount");
    TH2 *hknownmask = (TH2 *) cl->getHisto("hknownmask");
    TH1 *h1rcc[2][ntype];
    // PLEASE DO NOT EXTRACT ANYTHING FROM THE HISTOGRAMS BEFORE THIS CHECK
    // OTHERWISE THIS WILL CRASH THE HTML FOR ALL OTHER SUBSYSTEMS
    if ( h3temp == NULL || vtxs_pars == NULL || hknownmask == NULL)
    {
        DrawDeadServer(transparent[icanvas]);
        TC[icanvas]->Update();
        return -1;
    }
    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);
    
    /*
    //temporary - Check masked rcc's
    for (int ipkt = 1; ipkt <= hknownmask->GetNbinsX(); ipkt++)
      {
	for (int ircc = 0; ircc < 6; ircc++)
	  {
	    if (hknownmask->GetBinContent(ipkt, ircc+1) == 1)
	      {
		cout << " packet=" << ipkt-1+24101 << " rcc=" << ircc << endl;
	      }
	  }
      }
    */

    const float yscale_minimum = 0.0001;

    for (int barrel = 2; barrel < 4; barrel++)
    {
        for (unsigned int type = 0; type < ntype; type++)
        {
            h3temp->GetYaxis()->SetRange(barrel + 1, barrel + 1);
            h3temp->GetZaxis()->SetRange(type + 1, type + 1);
            h1rcc[barrel - 2][type] = (TH1 *) h3temp->Project3D("x");
            name.str("");
            name << "vtxsh1rcccount_" << barrel << "_" << type;
            h1rcc[barrel - 2][type]->SetName(name.str().c_str());

            if ( nevents > 0 && type == 0 )
            {
                h1rcc[barrel - 2][type]->Scale(1. / nevents);
                h1rcc[barrel - 2][type]->SetMaximum(NCHIP * NCHANNEL);
                h1rcc[barrel - 2][type]->SetMinimum(yscale_minimum);
                TString name = "Barrel: "; name += barrel;
                h1rcc[barrel - 2][type]->SetTitle(name.Data());
            }
        }
    }

    printTimeInterval("DrawNormRCC - Found Histograms");

    const unsigned int nbin[2] = {16 * 5, 24 * 6};

    for (unsigned int ibarrel = 0; ibarrel < 2; ibarrel++)
    {
        const int barrel = ibarrel + 2;
        const int nrcc_per_ladder = (barrel == 2) ? 5 : 6;
        if (nbin[ibarrel] > (unsigned int)h1rcc[ibarrel][0]->GetNbinsX())
        {
            std::cout << "SvxStripMonDraw::DrawNormRCC - "
                      << nbin[ibarrel] << " exceeds " << h1rcc[ibarrel][0]->GetNbinsX()
                      << " for B" << ibarrel
                      << std::endl;
            continue;
        }
    
        for (unsigned int ibin = 1; ibin <= nbin[ibarrel]; ibin++)
        {
            int ladder = (ibin - 1) / nrcc_per_ladder;
            int rcc = (ibin - 1) % nrcc_per_ladder;
            int packetid = ladder_map.get_packetid(barrel, ladder);
	    int packetidx = packetid - VTXS_PACKET_BASE;

	    //cout << " B" << barrel << " L" << ladder << " P" << packetid << " RCC" << rcc
	    // << " mask=" << hknownmask->GetBinContent(packetidx+1, rcc+1)
	    // << endl;

            double content = h1rcc[ibarrel][TYPE_RAW]->GetBinContent(ibin);
            if ( content > rcc_hitrate_high[ibarrel] && ig_hot [barrel][ladder][rcc] == 0 )
            {
                h1rcc[ibarrel][TYPE_HOT]->SetBinContent(ibin, content);
            }
            if ( content < rcc_hitrate_low[ibarrel] &&
		 ig_dead[barrel][ladder][rcc] == 0 &&
		 hknownmask->GetBinContent(packetidx + 1, rcc + 1) == 0)
            {
                h1rcc[ibarrel][TYPE_COLD]->SetBinContent(ibin, content);

                // if chip is totally dead, it won't show up in histo --> completely dead
                if ( content < h1rcc[ibarrel][TYPE_RAW]->GetMinimum() )
                {
                    h1rcc[ibarrel][TYPE_DEAD]->SetBinContent(ibin, 5e-4);
                }
            }
            if ( ig_hot [barrel][ladder][rcc] == 1 )
            {
                h1rcc[ibarrel][TYPE_KNOWNHOT]->SetBinContent(ibin, content);
            }
            // place this here rather than in inside the check on hotness
            // in case a chip goes non-hot and we want to still see that
            // i.e. we think it's hot but it's not.
            if ( ig_dead[barrel][ladder][rcc] == 1 ||
                    hknownmask->GetBinContent(packetidx + 1, rcc + 1) == 1)
            {
                //Rachid: don't confuse the shift crew with green boxes
                h1rcc[ibarrel][TYPE_KNOWNISSUE]->SetBinContent(ibin, content);

                if (content == 0) //CHC: when dead channel has not hit
                {
                    h1rcc[ibarrel][TYPE_KNOWNISSUE]->SetBinContent(ibin, 5e-4);
                }
            }
        }
    }

    printTimeInterval("DrawNormRCC - Manipulated Histograms");


    TLine tl;
    int color[ntype] = {kGray, kRed, kBlue - 6, kGreen + 1, kPink + 9, kBlue - 6};
    int style[ntype] = {1001, 1001, 1001, 1001, 1001, 1001};

    TC[icanvas]->Clear("D");
    for (int barrel = 2; barrel < 4; barrel++)
    {
        const int ipad = barrel - 2;
        TP[icanvas][ipad]->cd();
        TP[icanvas][ipad]->SetLogy(1);
        for (unsigned int type = 0; type < ntype; type++)
        {
            h1rcc[ipad][type]->SetFillColor(color[type]);
            h1rcc[ipad][type]->SetFillStyle(style[type]);
            h1rcc[ipad][type]->SetStats(0);
            h1rcc[ipad][type]->SetTickLength(0.);     // artifical axis will handle it

            h1rcc[ipad][type]->GetXaxis()->SetRange(1, nbin[barrel - 2]);
            h1rcc[ipad][type]->GetYaxis()->SetTitle("N_{Hit}/N_{Event}");
            h1rcc[ipad][type]->SetTitleOffset(0.5 / (ipad + 1) + 0.12 * ipad, "Y");
            h1rcc[ipad][type]->SetTitleSize(0.08, "Y");

            h1rcc[ipad][type]->SetLabelSize(0., "X");
            if (type == 0)
            {
                h1rcc[ipad][type]->DrawCopy("");
                h1rcc[ipad][type]->GetXaxis()->SetLabelSize(0.06);
                h1rcc[ipad][type]->GetYaxis()->SetLabelSize(0.06);
            }
            else if ( type == TYPE_DEAD )
            {
                for (int ibin = 1; ibin <= h1rcc[ipad][type]->GetNbinsX(); ibin++)
                {
                    float content = h1rcc[ipad][type]->GetBinContent(ibin);
                    if ( content < 0.001 ) continue;
                    tl.SetLineWidth(3);
                    tl.SetLineColor(color[type]);
                    tl.SetLineStyle(1);
                    float xval = h1rcc[ipad][type]->GetBinCenter(ibin);
                    tl.DrawLine(xval, 0.02, xval, rcc_hitrate_low[ipad]);
                }
                h1rcc[ipad][type]->SetMarkerStyle(23);
                h1rcc[ipad][type]->SetMarkerColor(color[TYPE_DEAD]);
                h1rcc[ipad][type]->DrawCopy("samep");
            }
            else
            {
                h1rcc[ipad][type]->DrawCopy("same");
            }
        }

        int nladder = (barrel == 2) ? 16 : 24;
        int nrcc_per_ladder = (barrel == 2) ? 5 : 6;

        // ladder demarcations
        tl.SetLineWidth(2);
        tl.SetLineColor(33);
        tl.SetLineStyle(1);
        for (int iladder = 0; iladder < nladder - 1; iladder++)
        {
            tl.DrawLine(nrcc_per_ladder * (iladder + 1) - 0.5, 0,
                        nrcc_per_ladder * (iladder + 1) - 0.5, NCHIP * NCHANNEL);
        }
    
        // maximum/minimum activity limits for hot/cold
        tl.SetLineWidth(1);
        tl.SetLineColor(1);
        tl.SetLineStyle(3);
        double xmin = h1rcc[barrel - 2][0]->GetXaxis()->GetXmin();
        //double xmax = h1rcc[barrel - 2][0]->GetXaxis()->GetXmax();
        double xmax = h1rcc[barrel - 2][0]->GetXaxis()->GetBinLowEdge(nbin[barrel - 2] + 1);
        tl.DrawLine(xmin, rcc_hitrate_low[barrel - 2], xmax, rcc_hitrate_low[barrel - 2]);
        tl.DrawLine(xmin, rcc_hitrate_high[barrel - 2], xmax, rcc_hitrate_high[barrel - 2]);

        TGaxis xax;
        xax.SetTitle("Ladder");
        xax.SetTitleSize(0.08);
        xax.SetTitleOffset(0.7);
        xax.DrawAxis(xmin, 0., xmax, 0., -0.5, nladder - 0.5, nladder);
    }

    // legend for colors
    TP[icanvas][1]->cd();
    float loy = 0.01;
    float hiy = 0.08;
    float lox[5] = {0.115, 0.242, 0.391, 0.549, 0.700};
    float xwid = 0.02;
    TText tex;
    tex.SetTextSize(0.05);
    string label[5] = {"Raw Counts", "Unexpected Hot", "Unexpected Cold", "Known Issue", "Known Hot"};
    for (int ii = 0; ii < 5; ii++)
    {
        double hix = lox[ii] + xwid;
        delete ladder_box[ii];
        ladder_box[ii] = new TPave(lox[ii], loy, hix, hiy, 0, "NDC");
        ladder_box[ii]->SetFillStyle(1001);
        ladder_box[ii]->SetFillColor(color[ii]);
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
    if (rcc_msg == 0)
    {
        rcc_msg = new TPaveText(0.01, 0.15, 0.95, 0.90, "NDC");
        rcc_msg->SetTextSize(0.06);
        rcc_msg->SetTextAlign(13);
        rcc_msg->SetBorderSize(0);
        rcc_msg->SetFillColor(0);
    }

    // number of rcc's which are above the threshold and not on the ignore-hot  list
    const double nhot = h1rcc[0][TYPE_HOT ]->GetEntries() + h1rcc[1][TYPE_HOT ]->GetEntries();
    // number of rcc's which are below the threshold and not on the ignore-cold list
    const double ncold = h1rcc[0][TYPE_COLD]->GetEntries() + h1rcc[1][TYPE_COLD]->GetEntries();
    
    rcc_msg->Clear();
    if(nevents>1000)
      {
	if ( nhot > 10)
	  {
	    rcc_msg->SetTextColor(kRed);
	    rcc_msg->AddText("Error! Too many hot RCCs");
	    rcc_msg->AddText("Check that stripixel bias is ON.");
	    rcc_msg->AddText("Stop the run and start a new one.");
	    rcc_msg->AddText("Contact expert if problem persists.");
	    rcc_msg->AddText("Please put the error message in the logbook.");
	  }
	else if ( ncold > 30) //this means at least 5 ladder is completly dead
	  {
	    rcc_msg->SetTextColor(kRed);
	    rcc_msg->AddText("Error! Too many cold RCCs");
	    rcc_msg->AddText("If half of the barrel is dead,");
	    rcc_msg->AddText("check if the thermocouple is");
	    rcc_msg->AddText("tripped.");
	    rcc_msg->AddText("Stop the run and start a new one.");
	    rcc_msg->AddText("Contact expert if problem persists.");
	    rcc_msg->AddText("Please put the error message in the logbook.");
	  }
	else
	  {
	    rcc_msg->SetTextColor(kBlack);
	    rcc_msg->AddText(Form("Number of Hot RCCs: %d",(int)nhot));
	    rcc_msg->AddText(Form("Number of Cold RCCs: %d",(int)ncold));
	    rcc_msg->AddText("Detector is OK, please proceed with run");
	  }/*
	     else if ( nhot > 4 || ncold > 4)
	     {
	     if ( nhot > 4  )
	     {
	     rcc_msg->SetTextColor(kBlue);
	     rcc_msg->AddText("Error! More than 4 hot RCCs");
	     rcc_msg->AddText("Check that stripixel bias is ON.");
	     }
	     if ( ncold > 4 )
	     {
	     rcc_msg->SetTextColor(kBlue);
	     rcc_msg->AddText("Error! More than 4 dead/cold RCCs");
	     //rcc_msg->AddText("Stop the run and start a new one.");
	     //rcc_msg->AddText("Contact expert if problem persists");
	     }
	     //rcc_msg->AddText("This is known issue,");
	     //rcc_msg->AddText("Please put the error message in the logbook.");
	     rcc_msg->AddText("Please feed VTXS at the end of the run.");
	     }
	     else
	     {
	     rcc_msg->SetTextColor(kGreen);
	     rcc_msg->AddText("OK");
	     }*/
	rcc_msg->Draw();
      }
    TC[icanvas]->Update();
    TC[icanvas]->Show();

    printTimeInterval("DrawNormRCC - Plotted");


    // clean up
    for (int barrel = 0; barrel < 2; barrel++)
    {
        for (unsigned int type = 0; type < ntype; type++)
        {
            delete h1rcc[barrel][type];
        }
    }

    printTimeInterval("DrawNormRCC - End");

    return 0;
}

int SvxStripMonDraw::DrawRCCBClockError(const char *what)
{
    printTimeInterval("DrawRCCBClockError - Start");
    
    int icanvas = RCCBCLOCKERROR;

    if (! gROOT->FindObject("SvxStripMonRCCBClockError"))
    {
        MakeCanvas("SvxStripMonRCCBClockError");
    }
    TC[icanvas]->Clear("D");

    // fetch histograms
    OnlMonClient *cl = OnlMonClient::instance();
    TH1 *vtxs_pars = cl->getHisto("vtxs_params");
    ostringstream name;

    TH2 *h2rccbclockerror = (TH2 *) cl->getHisto("hrccbclockerror");
    //In SvxStripMon.C hrccbclockerror is only filled if rcc is enabled
    //This knownmask is probably useless
    TH2 *hknownmask = (TH2 *) cl->getHisto("hknownmask");
    TH1 *h1rcc[2];
    TH1 *h1rccmask[2];
    TH1 *h1rccissue[2];

    if ( h2rccbclockerror == NULL || vtxs_pars == NULL || hknownmask == NULL)
    {
        DrawDeadServer(transparent[icanvas]);
        TC[icanvas]->Update();
        return -1;
    }

    printTimeInterval("DrawRCCBClockError - Found Histograms");

    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);

    int color[2] = {kCyan,kMagenta};

    TLine tl;
    TText tt;

    const unsigned int nbin[2] = {16 * 5, 24 * 6};

    const double badRCCcut = 0.1;
    int nBadRCC = 0;

    double integ_error = 0.;
    double nAvail = 0.;
    
    TC[icanvas]->Clear("D");
    for (int barrel = 2; barrel < 4; barrel++)
    {
        const int ipad = barrel - 2;
        TP[icanvas][ipad]->cd();
        TP[icanvas][ipad]->SetLogy(1);

	h2rccbclockerror->GetYaxis()->SetRange(barrel + 1, barrel + 1);
	h1rcc[ipad] = (TH1 *) h2rccbclockerror->ProjectionX();// "",barrel + 1, barrel + 1);
	name.str("");
	name << "hrccblockerror_" << barrel;
	h1rcc[ipad]->SetName(name.str().c_str());

	if ( nevents > 0)
	{
	    h1rcc[ipad]->Scale(1./nevents);
	    // h1rcc[barrel - 2]->SetMaximum(NCHIP * NCHANNEL);
	    // h1rcc[barrel - 2]->SetMinimum(yscale_minimum);
	}

	TString title = "Barrel: "; title += barrel;
	h1rcc[ipad]->SetTitle(title.Data());

	h1rcc[ipad]->SetFillColor(kRed);
	h1rcc[ipad]->SetFillStyle(1001);
	h1rcc[ipad]->SetStats(0);
	h1rcc[ipad]->SetTickLength(0.);     // artifical axis will handle it

	h1rcc[ipad]->GetXaxis()->SetRange(1, nbin[barrel - 2]);
	h1rcc[ipad]->SetMaximum(3.);
	h1rcc[ipad]->SetMinimum(0.0001);
	// h1rcc[ipad]->SetMaximum(nevents);
	h1rcc[ipad]->GetYaxis()->SetTitle("N_{ClockError}/N_{Event}");
	h1rcc[ipad]->SetTitleOffset(0.5 / (ipad + 1) + 0.12 * ipad, "Y");
	h1rcc[ipad]->SetTitleSize(0.08, "Y");

	h1rcc[ipad]->SetLabelSize(0., "X");

	h1rcc[ipad]->DrawCopy();

	h1rccmask[ipad] = (TH1*) h1rcc[ipad]->Clone("rccmask");
	h1rccissue[ipad] = (TH1*) h1rcc[ipad]->Clone("rccissue");

	// h1rcc[ipad]->GetXaxis()->SetLabelSize(0.06);
	// h1rcc[ipad]->GetYaxis()->SetLabelSize(0.06);

        int nladder = (barrel == 2) ? 16 : 24;
        int nrcc_per_ladder = (barrel == 2) ? 5 : 6;

	for (unsigned int ibin = 0; ibin < nbin[barrel-2]; ibin++)
	  {
	    int rcc = ibin%nrcc_per_ladder;
	    int ladder = ibin/nrcc_per_ladder;

	    int packetid = ladder_map.get_packetid(barrel, ladder);
	    int packetidx = packetid - VTXS_PACKET_BASE;

	    double tempBC = h1rcc[ipad]->GetBinContent(ibin+1);

	    double percMask = 0.;
	    
	    //count number of bad hybrids in this RCC
	    for (int ihyb = 0; ihyb < 4; ihyb++)
	      {
		percMask += hknownmask->GetBinContent(packetidx + 1, NRCC + (rcc*4) + ihyb+1);
	      }
	    percMask/=4.;

	    if (hknownmask->GetBinContent(packetidx + 1, rcc + 1) == 0 || percMask != 1)//second condition -> not all hybrids masked
	      {
		h1rccmask[ipad]->SetBinContent(ibin+1,0);
	      }
	    else 
	      {
		tempBC = 0.;//set so no contribution to nBadRCC
		percMask = 1.;
	      }

	    // else printf("Known Mask Packet = %i; RCC = %i; Ladder = %i; Barrel = %i\n",packetidx,rcc,ladder,barrel);
	    if (ig_clockissue[barrel][ladder][rcc] == 0)
	      {
		h1rccissue[ipad]->SetBinContent(ibin+1,0);
	      }
	    //no longer shall we exclude known issue rccs from the count.
	    // else 
	    //   {
	    // 	tempBC = 0.;//set so no contribution to nBadRCC
	    // 	percMask = 1.;//I know, a clock issue does not mean masked.  However, I do not want rccs with clock issues to contribute to nAvail.
	    //   }

	    integ_error+=(tempBC*(1.- percMask));
	    nAvail += (1. - percMask);

	    if (tempBC > badRCCcut) nBadRCC++;//this nBadRCC imo is not the proper counter.  integ_error is more sensible.  will probably do away with nBadRCC.
	  }
	
	h1rccmask[ipad]->SetFillColor(color[0]);
	h1rccissue[ipad]->SetFillColor(color[1]);

	h1rccmask[ipad]->DrawCopy("same");
	h1rccissue[ipad]->DrawCopy("same");

        // ladder demarcations
        tl.SetLineWidth(2);
        tl.SetLineColor(33);
        tl.SetLineStyle(1);
        for (int iladder = 0; iladder < nladder; iladder++)
        {
	    if(iladder!=(nladder-1))
	    {
		tl.DrawLine(nrcc_per_ladder * (iladder + 1) - 0.5, 0,
			    // nrcc_per_ladder * (iladder + 1) - 0.5, nevents);
			    nrcc_per_ladder * (iladder + 1) - 0.5, 3.);
	    }
	    int pktid = ladder_map.get_packetid(barrel,iladder);
	    tt.SetTextSize(0.036);
	    double textposx;
	    if (barrel == 2) textposx = (iladder+2.9)*(1./(nladder+3.15));
	    else textposx = (iladder+2.9)*(1./(nladder+3.24));
	    tt.DrawTextNDC(textposx, 0.08, Form("P%i", pktid));   
        }

        tl.SetLineWidth(1);
        tl.SetLineColor(1);
        tl.SetLineStyle(3);
        double xmin = h1rcc[barrel - 2]->GetXaxis()->GetXmin();
        //double xmax = h1rcc[barrel - 2][0]->GetXaxis()->GetXmax();
        double xmax = h1rcc[barrel - 2]->GetXaxis()->GetBinLowEdge(nbin[barrel - 2] + 1);
        tl.DrawLine(xmin, 1., xmax, 1.);
        // tl.DrawLine(xmin, nevents, xmax, nevents);

        TGaxis xax;
        xax.SetTitle("Ladder (Packet)");
        xax.SetTitleSize(0.05);
        xax.SetTitleOffset(1.3);
        xax.DrawAxis(xmin, 0., xmax, 0., -0.5, nladder - 0.5, nladder);
    }
    
    // legend for colors
    TP[icanvas][1]->cd();
    float loy = 0.01;
    float hiy = 0.06;
    float lox[2] = {0.115, 0.242};
    float xwid = 0.02;
    TText tex;
    tex.SetTextSize(0.05);
    string label[2] = {"Masked", "Known Issue"};
    for (int ii = 0; ii < 2; ii++)
    {
        double hix = lox[ii] + xwid;
        delete ladder_box[ii];
        ladder_box[ii] = new TPave(lox[ii], loy, hix, hiy, 0, "NDC");
        ladder_box[ii]->SetFillStyle(1001);
        ladder_box[ii]->SetFillColor(color[ii]);
        ladder_box[ii]->Draw();
        tex.DrawTextNDC(lox[ii] + 0.026, loy + 0.01, label[ii].c_str());
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

    // Error message
    TP[icanvas][2]->cd();
    if (rcc_msg_clock == 0)
      {
	rcc_msg_clock = new TPaveText(0.0, 0.35, 0.95, 0.70, "NDC");
	rcc_msg_clock->SetTextSize(0.048);
	rcc_msg_clock->SetTextAlign(13);
	rcc_msg_clock->SetBorderSize(0);
	rcc_msg_clock->SetFillColor(0);    
      }

    double integ_error_perc = (integ_error*100.)/nAvail;

    rcc_msg_clock->Clear();
    if(nevents>1000)
      {
	if (integ_error_perc > 15.)
	  {
	    rcc_msg_clock->SetTextColor(kRed);
	    rcc_msg_clock->AddText("Error!  Too many RCC Clock Errors.");
	    rcc_msg_clock->AddText("Please stop run then start new run.");
	    rcc_msg_clock->AddText(Form("Total RCC Error Percentage = %.2f%%",integ_error_perc));
	    rcc_msg_clock->AddText("If problem persists call VTX Expert.");
	  }
	else
	  {
	    rcc_msg_clock->SetTextColor(kBlack);
	    rcc_msg_clock->AddText(Form("All Mask Weighted: RCC Count = %.2f",nAvail));
	    rcc_msg_clock->AddText(Form("RCC Error Integral = %.2f",integ_error));
	    rcc_msg_clock->AddText(Form("Total RCC Error Percentage = %.2f%%",integ_error_perc));
	  }
			       
    // rcc_msg_clock->SetTextColor(kBlack);
    // rcc_msg_clock->AddText("Excluding Masked and Known Issue:");
    // rcc_msg_clock->AddText(Form("%i RCCs have a beamclock error",nBadRCC));
    // rcc_msg_clock->AddText(Form("for more than %.1f%% of events.",badRCCcut*100.));

    // if ( nBadRCC > 10)
    //   {
    // 	rcc_msg_clock->SetTextColor(kRed);
    // 	rcc_msg_clock->AddText("Error! Too many hot RCCs");
    // 	rcc_msg_clock->AddText("Check that stripixel bias is ON.");
    // 	rcc_msg_clock->AddText("Stop the run and start a new one.");
    // 	rcc_msg_clock->AddText("Contact expert if problem persists.");
    // 	rcc_msg_clock->AddText("Please put the error message in the logbook.");
    //   }

	rcc_msg_clock->Draw();
      }

    TC[icanvas]->Update();
    TC[icanvas]->Show();

    printTimeInterval("DrawRCCBClockError - Plotted");

    // clean up
    for (int barrel = 0; barrel < 2; barrel++)
    {
    	delete h1rcc[barrel];
    	delete h1rccmask[barrel];
    	delete h1rccissue[barrel];
    }

    printTimeInterval("DrawRCCBClockError - End");

    return 0;
}

int SvxStripMonDraw::DrawADC(const char *what)
{

    printTimeInterval("DrawADC - Start");


    OnlMonClient *cl = OnlMonClient::instance();
    TH2 *htemp = (TH2 *)cl->getHisto("svxstrip_h2sadc");
    TH1 *vtxs_pars = cl->getHisto("vtxs_params");
    if (! gROOT->FindObject("SvxStripMonADC"))
    {
        MakeCanvas("SvxStripMonADC");
    }

    TC[ADC]->Clear("D");
    if (! htemp || ! vtxs_pars )
    {
        DrawDeadServer(transparent[ADC]);
        TC[ADC]->Update();
        return -1;
    }
    TH2 *h2sadc = (TH2 *)htemp->Clone("svxstrip_h2sadc_clone");
    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);
    if (nevents > 0)
    {
        h2sadc->Scale(1. / nevents);
    }

    printTimeInterval("DrawADC - Retrieved Histograms");


    // turn off statistics boxes
    h2sadc->SetStats(0);
    h2sadc->SetMinimum(1. / nevents);
    
    // basic styling
    h2sadc->SetTitleSize(0.06, "Y");
    h2sadc->SetTitleOffset(0.6, "Y");
    h2sadc->SetYTitle("ADC");

    // this is a hack to suppress the x-axis from drawing
    TAxis *axh2sadc = h2sadc->GetXaxis();
    for (int ibin = 0; ibin < 24; ibin++)
    {
        TString desc = ibin;
        axh2sadc->SetBinLabel(17 + ibin, desc);
    }

    TGaxis xaxis_b2;
    TGaxis xaxis_b3;
    xaxis_b2.SetTextColor(2);
    xaxis_b3.SetTextColor(4);
    xaxis_b2.SetLabelColor(2);
    xaxis_b3.SetLabelColor(4);
    xaxis_b2.SetTitleSize(0.06);
    xaxis_b3.SetTitleSize(0.06);
    xaxis_b2.SetTitleOffset(0.7);
    xaxis_b3.SetTitleOffset(0.7);
    xaxis_b2.SetTitle("Barrel 2 Ladder");
    xaxis_b3.SetTitle("Barrel 3 Ladder");
    TLine tl;
    tl.SetLineWidth(8);

    TP[ADC][0]->cd();
    TP[ADC][0]->SetLogz(1);
    h2sadc->DrawCopy("colz");
    delete h2sadc;
    xaxis_b2.DrawAxis(-0.5, -0.5, 15.5, -0.5, -0.5, 15.5, 4, "+");
    xaxis_b3.DrawAxis(15.5, -0.5, 39.5, -0.5, -0.5, 23.5, 6, "+");
    tl.DrawLine(15.5, -0.5, 15.5, 255.5);
    TC[ADC]->Update();

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_SLIVE Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime) << " Events: " << (int) nevents;
    runstring = runnostream.str();
    transparent[ADC]->cd();
    PrintRun.DrawText(0.5, 0.99, runstring.c_str());
    TC[ADC]->Update();

    
    printTimeInterval("DrawADC - End");

    return 0;
}

int SvxStripMonDraw::DrawLive(const char *what)
{

    printTimeInterval("DrawLive - Start");

    
    OnlMonClient *cl = OnlMonClient::instance();

    TH2 *htemp = (TH2 *)cl->getHisto("svxstrip_h2live");
    TH1 *vtxs_pars = cl->getHisto("vtxs_params");

    if (! gROOT->FindObject("SvxStripMonLive"))
    {
        MakeCanvas("SvxStripMonLive");
    }

    TC[LIVE]->Clear("D");
    if ( ! htemp || ! vtxs_pars )
    {
        DrawDeadServer(transparent[LIVE]);
        TC[LIVE]->Update();
        return -1;
    }

    TH2 *h2live = (TH2 *)htemp->Clone("svxstrip_h2live_clone");

    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);
    if (nevents > 0)
    {
        h2live->Scale(1. / nevents);
    }

    printTimeInterval("DrawLive - Retrieved Histograms");


    // turn off statistics boxes
    h2live->SetStats(0);
    //h2live->SetMinimum(1./nevents);
    // basic styling
    h2live->SetTitleSize(0.06, "Y");
    h2live->SetTitleOffset(0.4, "Y");
    h2live->SetYTitle("RCC-Sensor");

    // this is a hack to suppress the x-axis from drawing
    TAxis *axh2live = h2live->GetXaxis();
    for (int ibin = 0; ibin < 24; ibin++)
    {
        TString desc = ibin;
        axh2live->SetBinLabel(17 + ibin, desc);
    }
    
    TGaxis xaxis_b2;
    TGaxis xaxis_b3;
    xaxis_b2.SetTextColor(2);
    xaxis_b3.SetTextColor(4);
    xaxis_b2.SetLabelColor(2);
    xaxis_b3.SetLabelColor(4);
    xaxis_b2.SetTitleSize(0.06);
    xaxis_b3.SetTitleSize(0.06);
    xaxis_b2.SetTitleOffset(0.7);
    xaxis_b3.SetTitleOffset(0.7);
    xaxis_b2.SetTitle("Barrel 2 Ladder");
    xaxis_b3.SetTitle("Barrel 3 Ladder");
    TLine tl;
    tl.SetLineWidth(8);

    TC[LIVE]->cd();
    TP[LIVE][0]->cd();
    h2live->DrawCopy("colz");
    delete h2live;
    xaxis_b2.DrawAxis(-0.5, -0.5, 15.5, -0.5, -0.5, 15.5, 4, "+");
    xaxis_b3.DrawAxis(15.5, -0.5, 39.5, -0.5, -0.5, 23.5, 6, "+");
    tl.DrawLine(15.5, -0.5, 15.5, 71.5);

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_S Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime) << " Events: " << (int) nevents;
    runstring = runnostream.str();
    transparent[LIVE]->cd();
    PrintRun.DrawText(0.5, 0.99, runstring.c_str());
    TC[LIVE]->Update();

    printTimeInterval("DrawLive - End");

    
    return 0;
}

int SvxStripMonDraw::DrawSize(const char *what)
{

    printTimeInterval("DrawSize - Start");


    OnlMonClient *cl = OnlMonClient::instance();

    TH1 *vtxs_pars = (TH1F *)cl->getHisto("vtxs_params");
    TH1 *hsadc_bytime_temp = (TH1F *)cl->getHisto("svxstrip_hsadc_bytime");
    TH1 *hsadc_bytime_nohot_temp = (TH1F *)cl->getHisto("svxstrip_hsadc_bytime_nohot");
    TH1 *hnbbc_bytime_temp = (TH1F *)cl->getHisto("svxstrip_hnbbc_bytime");
    TH1 *hmulti = (TH1F *)cl->getHisto("svxstrip_hmulti");
    if (! gROOT->FindObject("SvxStripMonSize"))
    {
        MakeCanvas("SvxStripMonSize");
    }

    TC[SIZE]->Clear("D");

    if ( !vtxs_pars ||
            !hsadc_bytime_temp ||
            !hnbbc_bytime_temp ||
            !hmulti ||
            !hsadc_bytime_nohot_temp
       )
    {
        DrawDeadServer(transparent[SIZE]);
        TC[SIZE]->Update();
        return -1;
    }

    TH1 *hsadc_bytime = (TH1F *)hsadc_bytime_temp->Clone("svxstrip_hsadc_bytime_clone");
    TH1 *hsadc_bytime_nohot = (TH1F *)hsadc_bytime_nohot_temp->Clone("svxstrip_hsadc_bytime_nohot_clone");
    TH1 *hnbbc_bytime = (TH1F *)hnbbc_bytime_temp->Clone("svxstrip_hnbbc_bytime_clone");

    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);
    if (nevents > 0)
    {
        hsadc_bytime->Divide( hnbbc_bytime );
        hsadc_bytime_nohot->Divide( hnbbc_bytime );
    }

    printTimeInterval("DrawSize - Retrieved Histograms");


    // turn off statistics boxes
    hsadc_bytime->SetStats(0);
    hsadc_bytime_nohot->SetStats(0);

    // basic styling
    //hsadc_bytime->SetTitleSize(0.09,"Y");
    //hsadc_bytime->SetTitleSize(0.09,"X");

    //int lastevtbin = hnbbc_bytime->GetLastBinAbove(0);
    int lastevtbin = hnbbc_bytime->GetNbinsX();
    while ( lastevtbin > 0 )
    {
        if ( hnbbc_bytime->GetBinContent(lastevtbin) > 0 )
        {
            break;
        }
        lastevtbin--;
    }


    TP[SIZE][0]->cd();
    hsadc_bytime->SetMarkerStyle( 20 );
    hsadc_bytime->SetMarkerColor( 4 );
    hsadc_bytime->SetLineColor( 4 );
    hsadc_bytime->SetXTitle( "Event Number" );
    hsadc_bytime->SetYTitle( "Summed ADC/N_{BBC}" );
    hsadc_bytime->SetMinimum( 0 );
    hsadc_bytime->SetAxisRange( 0, hnbbc_bytime->GetBinLowEdge(lastevtbin + 1) - 1 );
    hsadc_bytime->GetXaxis()->SetLabelSize(0.06);
    hsadc_bytime->GetYaxis()->SetLabelSize(0.08);
    hsadc_bytime->GetXaxis()->SetTitleSize(0.06);
    hsadc_bytime->GetYaxis()->SetTitleSize(0.08);
    hsadc_bytime->GetXaxis()->SetTitleOffset(0.65);
    hsadc_bytime->GetYaxis()->SetTitleOffset(0.55);

    /*
    double default_ymax = 400000.;
    double ymax = hsadc_bytime->GetMaximum();
    if ( ymax < default_ymax )
    {
        hsadc_bytime->SetAxisRange( 0, default_ymax, "Y");
    }
    else
    {
        hsadc_bytime->SetAxisRange( 0, 1.5 * ymax, "Y");
    }
    */
    hsadc_bytime->SetAxisRange( 0, 400e3, "Y");

    hsadc_bytime->DrawCopy("P HIST");

    hsadc_bytime_nohot->SetMarkerStyle( 20 );
    hsadc_bytime_nohot->SetMarkerColor( 6 );
    hsadc_bytime_nohot->SetLineColor( 6 );
    hsadc_bytime_nohot->DrawCopy("P HIST SAME");

    TLine lrange;
    lrange.SetLineColor(2);
    lrange.SetLineWidth(1);
    lrange.SetLineStyle(2);

    for (int ii = 0; ii < 2; ii++)
    {
        lrange.DrawLine(0., charge_range[ii],
                        hnbbc_bytime->GetBinLowEdge(lastevtbin + 1) - 1, charge_range[ii]);
    }

    //draw a legend
    TLegend *legsadc = new TLegend(0.75, 0.65, 0.9, 0.9);
    legsadc->SetFillStyle(0);
    legsadc->AddEntry(hsadc_bytime, "All channels", "LP");
    legsadc->AddEntry(hsadc_bytime_nohot, "No hot channels", "LP");
    legsadc->Draw("SAME");


    TP[SIZE][1]->cd();
    //hmulti->Sumw2();
    hmulti->DrawCopy("");
    hmulti->GetXaxis()->SetLabelSize(0.08);
    hmulti->GetYaxis()->SetLabelSize(0.08);
    hmulti->GetXaxis()->SetTitleSize(0.08);
    hmulti->GetYaxis()->SetTitleSize(0.08);
    hmulti->GetXaxis()->SetTitleOffset(0.55);
    hmulti->GetYaxis()->SetLabelSize(0.5);

    for (int ii = 0; ii < 2; ii++)
    {
        lrange.DrawLine(log10(multi_range[ii]), 0., log10(multi_range[ii]), hmulti->GetMaximum());
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_S Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime) << " Events: " << (int) nevents;
    runstring = runnostream.str();
    transparent[SIZE]->cd();
    PrintRun.DrawText(0.5, 0.99, runstring.c_str());

    //create a TText object to denote when thresholds were changed
    TText MultiLabel;
    MultiLabel.SetTextFont(62);
    MultiLabel.SetTextSize(0.02);
    MultiLabel.SetNDC();  // set to normalized coordinates
    MultiLabel.SetTextAlign(23); // center/top alignment
    transparent[SIZE]->cd();
    MultiLabel.DrawText(0.5, 0.95, MultiText.c_str());


    TC[SIZE]->Update();

    delete hsadc_bytime;
    delete hsadc_bytime_nohot;
    delete hnbbc_bytime;

    printTimeInterval("DrawSize - End");

    return 0;
}


int SvxStripMonDraw::DrawCellID(const char *what, int aflg)
{

    printTimeInterval("DrawCellID - Start");

    OnlMonClient *cl = OnlMonClient::instance();
    TH1 *vtxs_pars = cl->getHisto("vtxs_params");

    int mksty[6] = { 21, 23, 20, 22, 25, 30 };
    int mkclr[6] = {  7,  8,  6,  4,  2,  1 };

    stringstream name;
    int CELLID;
    TText t1;
    t1.SetTextColor(2);
    TBox arng;
    arng.SetFillColor(17);

    name.str("");
    if (aflg == 0)
    {
        name << "SvxStripMonCellIDWest";
        CELLID = CELLIDW;
    }
    else
    {
        name << "SvxStripMonCellIDEast";
        CELLID = CELLIDE;
    }

    if (! gROOT->FindObject(name.str().c_str()))
    {
        MakeCanvas(name.str().c_str());
    }

    TC[CELLID]->Clear("D");

    // TH2 *hknownmask = (TH2 *) cl->getHisto("hknownmask");
    TH3 *h3cellpro = (TH3 *) cl->getHisto("hcellpro");
    TH1 *cellp[20][6] = {{0}};
    if (h3cellpro == NULL || vtxs_pars == NULL) // || hknownmask)
    {
        DrawDeadServer(transparent[CELLID]);
        return -1;
    }
    
    //for now not using these two outside following loop. declaring here anyway.
    // float avgrms[20][6] = {{0.}};
    // bool disabledRCC[20][6] = {{0}};

    //let us first get the valid bins. (those actually filled)
    TH1* cellpALL = (TH1 *) h3cellpro->Project3D("x");
    if (cellpALL == NULL)
    {
        DrawDeadServer(transparent[CELLID]);
        return -1;
    }
    int cellpALL_NbinsX = cellpALL->GetNbinsX();
    bool cellp_binOK[cellpALL_NbinsX]; // = {0};
    int nOKbins = 0;

    for (int ibin = 0; ibin < cellpALL_NbinsX; ++ibin)
      {
	if (cellpALL->GetBinContent(ibin + 1) != 0.)
	  {
	    cellp_binOK[ibin] = 1;
	    nOKbins++;
	  }
	else cellp_binOK[ibin] = 0;
      }

    delete cellpALL;

    int bad_rcc_count = 0;
    int disabled_count = 0;
    int zero_count = 0;
    // int zero_dis_count = 0;//some rccs are always disabled or zero...

    for (int ilad = 0; ilad < 20; ilad++)
    {
      if (nOKbins == 0) continue;

        int ipktid;

        if (aflg == 0) // WEST
        {
            ipktid = (ilad < 16) ? ilad  : ilad + 8;
        }
        else        // EAST
        {
            ipktid = (ilad < 8) ? ilad + 16 : ilad + 20;
        }

        pair<int, int> bl = ladder_map.get_barrel_ladder(ipktid);
        int barrel = bl.first;
        int ladder = bl.second;

        TP[CELLID][ilad]->cd();
        TP[CELLID][ilad]->SetTopMargin(0.02);
        TP[CELLID][ilad]->SetRightMargin(0.02);
        TP[CELLID][ilad]->SetLeftMargin(0.10);
        TP[CELLID][ilad]->SetBottomMargin(0.15);

        name.str("");

        for (int ircc = 0; ircc < 6; ircc++)
        {
            name.str("");
            name << "hcellpro_" << ipktid << "_" << ircc;
            h3cellpro->GetYaxis()->SetRange(ipktid + 1, ipktid + 1);
            h3cellpro->GetZaxis()->SetRange(ircc + 1, ircc + 1);
            cellp[ilad][ircc] = (TH1 *) h3cellpro->Project3D("x");
            cellp[ilad][ircc]->SetName(name.str().c_str());

            if (cellp[ilad][ircc] == 0)
            {
                cout << "Not found " << name.str().c_str() << endl;
                DrawDeadServer(transparent[CELLID]);
                return -1;
            }
            else
            {
                cellp[ilad][ircc]->SetMarkerStyle(mksty[ircc]);
                cellp[ilad][ircc]->SetMarkerColor(mkclr[ircc]);

                bool alwaysZero = true;
		bool alwaysDisabled = true;
		float avgrms = 0.;
		int nDisablings = 0;

                for (int ibin = 0; ibin < cellp[ilad][ircc]->GetNbinsX(); ++ibin)
                {
		  if (!cellp_binOK[ibin]) continue;

		  float cellpBC = cellp[ilad][ircc]->GetBinContent(ibin + 1);
		  if (cellpBC != 0.0)
		    {
		      alwaysZero = false;

		      cellp[ilad][ircc]->SetBinError(ibin + 1, 0.0000001);

		      if (cellpBC != -10.) alwaysDisabled = false;
		      else 
			{
			  nDisablings++;
			  cellpBC = 0;//do not allow to contribute to average
			  //can only show so much information
			  //DAQ plots show enabling/disabling problems.
			}
		      avgrms += cellpBC;
		    }
		  else alwaysDisabled = false;
		}
		// cout<<"Barrel = "<<barrel<<".Ladder = "<<ladder<<". RCC = "<<ircc<<". Total RMS = "<<avgrms;
		float nBinDiff = nOKbins - nDisablings;
		if (nBinDiff != 0)
		  avgrms/=(nOKbins-nDisablings);
		// cout<<". nOKbins = "<<nOKbins<<". nDisablings = "<<nDisablings<<". AvgRMS = "<<avgrms<<endl;

		if (!(ircc == 5 && barrel == 2))
		  {
		    if (alwaysZero)
		      {
			zero_count++;
		      }

		    if (alwaysDisabled)
		      {
			disabled_count++;
		      }
		    else if (avgrms < 11.5 || avgrms > 14.5)//includes always zero
		      {
			bad_rcc_count++;
		      }

		    // if (!alwaysDisabled && nDisablings!=0 && avgrms==0)
		    //   {
		    // 	zero_dis_count++;
		    //   }
		  }

                if (ircc == 0)
                {
                    cellp[ilad][ircc]->SetMaximum(21.);
                    cellp[ilad][ircc]->SetMinimum(-1.);
                    //          cellp[ilad][ircc]->SetAxisRange(0.,300.,"X");
                    cellp[ilad][ircc]->GetXaxis()->SetTitle("Time (sec)");
                    cellp[ilad][ircc]->GetXaxis()->SetLabelSize(0.07);
                    cellp[ilad][ircc]->GetXaxis()->SetTitleSize(0.08);
                    cellp[ilad][ircc]->GetXaxis()->SetTitleOffset(0.8);
                    cellp[ilad][ircc]->GetYaxis()->SetTitle("RMS of cell distribution");
                    cellp[ilad][ircc]->GetYaxis()->SetNdivisions(605);
                    cellp[ilad][ircc]->GetYaxis()->SetLabelSize(0.07);
                    cellp[ilad][ircc]->GetYaxis()->SetTitleSize(0.07);
                    cellp[ilad][ircc]->GetYaxis()->SetTitleOffset(0.6);
                    cellp[ilad][ircc]->SetStats(0);
                    cellp[ilad][ircc]->DrawCopy("P");

                    arng.DrawBox(60., 11.5, 3600., 14.5);
                    t1.SetTextSize(0.12);
                    t1.DrawTextNDC(0.8, 0.87, Form("B%iL%02i", barrel, ladder));

                    cellp[ilad][ircc]->DrawCopy("Psame");
                }
                else
                {
                    // if (!(ircc == 5 && ladder == 2))//ZR:Bug
                    if (!(ircc == 5 && barrel == 2))
                    {
                        /*
                        if ((aflg == 0 && ilad == 5 && ircc == 4) //(barrel, ladder, rcc) = (2, 6, 4)
                                || (aflg == 0 && ilad == 14 && ircc == 1) //(barrel, ladder, rcc) = (3, 6, 1)
                                || (aflg == 0 && ilad == 17 && ircc == 5) //(barrel, ladder, rcc) = (3, 9, 5)
                                || (aflg == 1 && ilad == 3 && ircc == 2) //(barrel, ladder, rcc) = (2, 11, 2)
                                || (aflg == 1 && ilad == 3 && ircc == 4) //(barrel, ladder, rcc) = (2, 11, 4)
                                || (aflg == 1 && ilad == 4 && ircc == 1) //(barrel, ladder, rcc) = (2, 12, 1)
                                || (aflg == 1 && ilad == 15 && ircc == 5)) //(barrel, ladder, rcc) = (3, 13, 5)
                        {
                            continue;
                        }
                        else
                        */
                        {
                            cellp[ilad][ircc]->DrawCopy("Psame");
                        }
                    }
                }
            }
        }
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);

    // fill run number and event time into string
    runnostream << ThisName << "_" << canv_short[CELLID] << " (for expert) Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime) << " Events: " << (int) nevents;

    runstring = runnostream.str();
    transparent[CELLID]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    // if (bad_rcc_count >= 60)
    if (bad_rcc_count >= 52)//previous count included rcc 5 barrel 2
    {
        string badrun("STOP THE DAQ! TOO MANY BAD RCCS!");

        TText StopTheDAQ;
        StopTheDAQ.SetTextFont(62);
        StopTheDAQ.SetTextSize(0.1);
        StopTheDAQ.SetTextColor(2);
        StopTheDAQ.SetNDC();         // set to normalized coordinates
        StopTheDAQ.SetTextAlign(23); // center/top alignment
        StopTheDAQ.DrawText(0.5, 0.5, badrun.c_str());
    }

    TMarker mk;
    mk.SetMarkerSize(2.0);
    TText mt;
    mt.SetTextFont(62);
    mt.SetTextSize(0.02);
    mt.SetNDC();

    for (int ircc = 0; ircc < 6; ircc++)
    {
        mk.SetMarkerStyle(mksty[ircc]);
        mk.SetMarkerColor(mkclr[ircc]);
        mk.DrawMarker(ircc * 0.07 + 0.05, 0.03);
        name.str("");
        name << "RCC" << ircc;
        mt.DrawText(ircc * 0.07 + 0.065, 0.02, name.str().c_str());
    }
    // arng.DrawBox(0.55, 0.01, 0.6, 0.04);
    arng.DrawBox(0.48, 0.01, 0.53, 0.04);
    name.str("");
    name << "Valid range";
    // mt.DrawText(0.61, 0.02, name.str().c_str());
    mt.DrawText(0.54, 0.02, name.str().c_str());

    //Extra info with avgrms
    name.str("");
    name << disabled_count;
    name << " Never Enabled. ";
    name << bad_rcc_count;
    name << " Bad AvgRMS (";
    name << zero_count;
    name << " Always RMS0).";
    // mt.SetTextColor(kRed);
    // mt.SetTextSize(0.016);
    mt.DrawText(0.65, 0.02, name.str().c_str());

    TC[CELLID]->Update();

    // clean up
    for (int ilad = 0; ilad < 20; ilad++)
    {
        for (int ircc = 0; ircc < 6; ircc++)
        {
            delete cellp[ilad][ircc];
        }
    }

    printTimeInterval("DrawCellID - End");

    return 0;
}


int SvxStripMonDraw::DrawRCCBClock(const char *what, int aflg)
{

    printTimeInterval("DrawRCCBClock - Start");

    OnlMonClient *cl = OnlMonClient::instance();
    TH1 *vtxs_pars = cl->getHisto("vtxs_params");
    if (!vtxs_pars)
      {
	return -1;
      }
    float nevents = vtxs_pars->GetBinContent(EVENTCOUNTBIN);

    // int mksty[6] = { 21, 23, 20, 22, 25, 30 };
    int mkclr[6] = {  7,  8,  6,  4,  2,  1 };

    bool doLog = true;
    
    // double sclr[6];
    double* sclr;

    double sclrLog[6] = {300., 100., 30., 10., 3., 1.};
    double sclrNoLog[6] = {6., 5., 4., 3., 2., 1.};

    if (doLog)
    {
	sclr = sclrLog;
    }
    else
    {
	sclr = sclrNoLog;
    }


    stringstream name;
    int RCCBCLOCK;
    TText t1;
    t1.SetTextColor(1);
    TBox arng;
    arng.SetFillColor(17);

    name.str("");
    if (aflg == 0)
    {
        name << "SvxStripMonRCCBClockWest";
        RCCBCLOCK = RCCBCLOCKW;
    }
    else
    {
        name << "SvxStripMonRCCBClockEast";
        RCCBCLOCK = RCCBCLOCKE;
    }

    if (! gROOT->FindObject(name.str().c_str()))
    {
        MakeCanvas(name.str().c_str());
    }

    TC[RCCBCLOCK]->Clear("D");


    TH3 *h3rccbclock = (TH3 *) cl->getHisto("hrccbclock");
    TH1 *rccbcp[20][6] = {{0}};
    if (h3rccbclock == NULL || vtxs_pars == NULL)
    {
      cout<<"h3rccbclock = "<<h3rccbclock<<endl;
        DrawDeadServer(transparent[RCCBCLOCK]);
        return -1;
    }

    TLine tline;

    int cancntr = 0;

    for (int ibar = 2; ibar < 4; ibar++)
    {
	int ladmin = (ibar == 2) ? (aflg)*8 : (aflg)*12;//aflg*8;

	int ladmax = (ibar == 2) ? (aflg+1)*8 : (aflg+1)*12;

	for (int ilad = ladmin; ilad < ladmax; ilad++)
	{

	    int pktid = ladder_map.get_packetid(ibar,ilad);
	    int ipkt = pktid - VTXS_PACKET_BASE;

	    TP[RCCBCLOCK][cancntr]->cd();
	    if (doLog) TP[RCCBCLOCK][cancntr]->SetLogy(1);
	    TP[RCCBCLOCK][cancntr]->SetTopMargin(0.02);
	    TP[RCCBCLOCK][cancntr]->SetRightMargin(0.01);
	    TP[RCCBCLOCK][cancntr]->SetLeftMargin(0.10);
	    TP[RCCBCLOCK][cancntr]->SetBottomMargin(0.15);

	    name.str("");

	    int maxrcc = 5 + (ibar - 2);

	    for (int ircc = 0; ircc < maxrcc; ircc++)
	    {
		name.str("");
		name << "hrccbclock_" << ipkt << "_" << ircc;
		h3rccbclock->GetYaxis()->SetRange(ipkt + 1, ipkt + 1);
		h3rccbclock->GetZaxis()->SetRange(ircc + 1, ircc + 1);
		rccbcp[cancntr][ircc] = (TH1 *) h3rccbclock->Project3D("x");
		rccbcp[cancntr][ircc]->SetName(name.str().c_str());
	    

		if (rccbcp[cancntr][ircc] == 0)
		{
		    cout << "Not found " << name.str().c_str() << endl;
		    DrawDeadServer(transparent[RCCBCLOCK]);
		    return -1;
		}
		else
		{
		    if ( nevents > 0)
		    {
			rccbcp[cancntr][ircc]->Scale(sclr[ircc] / nevents);
			// h1rcc[ibar - 2]->SetMaximum(NCHIP * NCHANNEL);
			// h1rcc[ibar - 2]->SetMinimum(yscale_minimum);
		    }

		    // rccbcp[cancntr][ircc]->SetMarkerStyle(mksty[ircc]);
		    // rccbcp[cancntr][ircc]->SetMarkerColor(mkclr[ircc]);
		    rccbcp[cancntr][ircc]->SetFillColor(mkclr[ircc]);
		    rccbcp[cancntr][ircc]->SetLineColor(mkclr[ircc]);

		    if (rccbcp[cancntr][ircc]->GetEntries() == 0)
		    {
		    	// int fillpat = 3017 + ircc%2;
		    	int fillpat = 1001;
		    	rccbcp[cancntr][ircc]->SetFillStyle(fillpat);
		    	rccbcp[cancntr][ircc]->Fill(10.,sclr[ircc]);
		    }
		    else 
		    {
		    	int fillpat = 0;
		    	rccbcp[cancntr][ircc]->SetFillStyle(fillpat);
		    }
			

		    if (ircc == 0)
		    {
			if (doLog) 
			{
			    rccbcp[cancntr][ircc]->SetMaximum(2.*sclr[0]);
			    rccbcp[cancntr][ircc]->SetMinimum(0.001);
			}
			else 
			{
			    rccbcp[cancntr][ircc]->SetMaximum(sclr[0]+1.);
			    rccbcp[cancntr][ircc]->SetMinimum(0.0);
			}

			rccbcp[cancntr][ircc]->GetXaxis()->SetTitle("RCCBClk - FBClk");
			rccbcp[cancntr][ircc]->GetXaxis()->SetLabelSize(0.07);
			rccbcp[cancntr][ircc]->GetXaxis()->SetTitleSize(0.08);
			rccbcp[cancntr][ircc]->GetXaxis()->SetTitleOffset(0.8);
			rccbcp[cancntr][ircc]->SetStats(0);
			rccbcp[cancntr][ircc]->DrawCopy();

			t1.SetTextSize(0.12);
			t1.DrawTextNDC(0.8, 0.87, Form("B%iL%02i", ibar, ilad));
			t1.DrawTextNDC(0.8, 0.77, Form("P%i", pktid));

			tline.SetLineWidth(1);
			tline.SetLineColor(mkclr[ircc]);
			tline.SetLineStyle(3);
			double xmin = rccbcp[cancntr][ircc]->GetXaxis()->GetXmin();
			double xmax = rccbcp[cancntr][ircc]->GetXaxis()->GetXmax();
			tline.DrawLine(xmin, sclr[0], xmax, sclr[0]);

			rccbcp[cancntr][ircc]->DrawCopy("same");
		    }
		    else
		    {
			tline.SetLineWidth(1);
			tline.SetLineColor(mkclr[ircc]);
			tline.SetLineStyle(3);
			double xmin = rccbcp[cancntr][ircc]->GetXaxis()->GetXmin();
			double xmax = rccbcp[cancntr][ircc]->GetXaxis()->GetXmax();
			tline.DrawLine(xmin, sclr[ircc], xmax, sclr[ircc]);

			rccbcp[cancntr][ircc]->DrawCopy("same");

		    }

		}

	    }//ircc

	    cancntr++;

	}//ilad

    }//ibar

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");

    // fill run number and event time into string
    runnostream << ThisName << "_" << canv_short[RCCBCLOCK] << " (for expert) Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime) << " Events: " << (int) nevents;

    runstring = runnostream.str();
    transparent[RCCBCLOCK]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    TText mt;
    mt.SetTextFont(62);
    mt.SetTextSize(0.02);
    mt.SetNDC();

    for (int ircc = 0; ircc < 6; ircc++)
    {
	arng.SetFillColor(mkclr[ircc]);
	arng.DrawBox((ircc+1)*0.14, 0.015, (ircc+1)*0.14 + .04, 0.035);
        name.str("");
        name << "RCC" << ircc;
	name << "*" << sclr[ircc];
        mt.DrawText((ircc+1)*0.14 + .05, 0.02, name.str().c_str());
    }

    TC[RCCBCLOCK]->Update();

    // clean up
    for (int ican = 0; ican < cancntr; ican++)
    {
        for (int ircc = 0; ircc < 6; ircc++)
        {
            delete rccbcp[ican][ircc];
        }
    }

    printTimeInterval("DrawRCCBClock - End");

    return 0;
}

int SvxStripMonDraw::DrawLadder(const int packetid)
{

    printTimeInterval("DrawLadder - Start");


    OnlMonClient *cl = OnlMonClient::instance();

    int spiro = packetid - 24101;

    if (! gROOT->FindObject("SvxStripMonLadder") )
    {
        MakeCanvas("SvxStripMonLadder");
    }
    TC[LADDER]->Clear("D");

    stringstream name;
    TH2 *h2[6];
    for (int rcc = 0; rcc < 6; rcc++)
    {
        name.str("");
        name << "h2svxstrip_adc_" << spiro << "_" << rcc;
        h2[rcc] = (TH2 *)cl->getHisto(name.str().c_str());
        if (h2[rcc] == 0)
        {
            DrawDeadServer(transparent[LADDER]);
            return -1;
        }
        else
        {
            TP[LADDER][rcc]->cd();
            h2[rcc]->DrawCopy("colz");
        }
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_2 Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[LADDER]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());
    TC[LADDER]->Update();

    printTimeInterval("DrawLadder - End");

    return 0;
}

int SvxStripMonDraw::DrawDeadServer(TPad *transparent)
{
    transparent->cd();
    TText FatalMsg;
    FatalMsg.SetTextFont(62);
    FatalMsg.SetTextSize(0.1);
    FatalMsg.SetTextColor(4);
    FatalMsg.SetNDC();  // set to normalized coordinates
    FatalMsg.SetTextAlign(23); // center/top alignment
    FatalMsg.DrawText(0.5, 0.9, "SVXSTRIPMON");
    FatalMsg.SetTextAlign(22); // center/center alignment
    FatalMsg.DrawText(0.5, 0.5, "SERVER");
    FatalMsg.SetTextAlign(21); // center/bottom alignment
    FatalMsg.DrawText(0.5, 0.1, "DEAD");
    transparent->Update();
    return 0;
}

int SvxStripMonDraw::DrawCellID2(const char *, int)
{

    printTimeInterval("DrawCellID2 - Start");

    cout << "SvxStripMonDraw::DrawCellID2()" << endl;
    OnlMonClient *cl = OnlMonClient::instance();

    if (! gROOT->FindObject("SvxStripMonCellID2") )
    {
        MakeCanvas("SvxStripMonCellID2");
    }
    TC[CELLID2]->Clear("D");

    TH3 *h3cellid = (TH3 *) cl->getHisto("hcellid");
    if (h3cellid == NULL)
    {
        DrawDeadServer(transparent[CELLID2]);
        return -1;

    }


    int badrcc = 0;
    for (int ipkt = 0; ipkt < 40; ipkt++)
    {

        stringstream name;
        TH1 *h1[6];
        for (int rcc = 0; rcc < 6; rcc++)
        {
            name.str("");
            name << "hcellid_" << ipkt << "_" << rcc;
            h3cellid->GetYaxis()->SetRange(ipkt + 1, ipkt + 1);
            h3cellid->GetZaxis()->SetRange(rcc + 1, rcc + 1);
            h1[rcc] = (TH1 *) h3cellid->Project3D("x");
            h1[rcc]->SetName(name.str().c_str());

            TP[CELLID2][ipkt]->cd();
            h1[rcc]->DrawCopy();
            //cout << ipkt << " " << rcc << " " << h1[rcc]->GetRMS() << endl;
            if (h1[rcc]->GetRMS() < 11.5 || h1[rcc]->GetRMS() > 14.5)
                ++badrcc;
        }

    }

    //cout << "Total bad: " << badrcc << endl;

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << ThisName << "_2 Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[CELLID2]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());
    TC[CELLID2]->Update();

    printTimeInterval("DrawCellID2 - End");

    return 0;
}

int SvxStripMonDraw::DrawBClock(const char *)
{

    printTimeInterval("DrawBClock - Start");


    OnlMonClient *cl = OnlMonClient::instance();

    if (! gROOT->FindObject("SvxStripMonBClock") )
    {
        MakeCanvas("SvxStripMonBClock");
    }
    TC[BCLOCK]->Clear("D");

    TText PadHeader;
    PadHeader.SetTextFont(62);
    PadHeader.SetTextSize(0.1);
    PadHeader.SetNDC();         // set to normalized coordinates
    PadHeader.SetTextAlign(23); // center/top alignment

    TH2 *h2bclock = (TH2 *) cl->getHisto("hbclock");
    if (h2bclock == NULL)
    {
        DrawDeadServer(transparent[BCLOCK]);
        return -1;
    }

    for (int ipkt = 0; ipkt < VTXS_NPACKET; ipkt++)
    {
        stringstream name;
        name.str("");
        name << "hbclock_" << ipkt;
        TH1 *h1 = (TH1 *) h2bclock->ProjectionX(name.str().c_str(), ipkt + 1, ipkt + 1);

        pair<int, int> bl = ladder_map.get_barrel_ladder(ipkt);
        int barrel = bl.first;
        int ladder = bl.second;

        TP[BCLOCK][ipkt]->cd();
        h1->SetStats(0);
        h1->SetFillColor(3);
        h1->SetTitle("");
        h1->DrawCopy();

        PadHeader.DrawText(0.5, 1., Form("packet %i, B%iL%02i", VTXS_PACKET_BASE + ipkt, barrel, ladder));
        //TP[BCLOCK][ipkt]->Update();
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << "GL1 bclk - FEM bclk Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[BCLOCK]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());
    TC[BCLOCK]->Update();

    printTimeInterval("DrawBClock - End");

    return 0;
}


int SvxStripMonDraw::DrawPACnt(const char *)
{

    printTimeInterval("DrawPACnt - Start");


    OnlMonClient *cl = OnlMonClient::instance();

    if (! gROOT->FindObject(canv_name[PACNT].c_str()) )
    {
        MakeCanvas(canv_name[PACNT].c_str());
    }
    TC[PACNT]->Clear("D");

    TText PadHeader;
    PadHeader.SetTextFont(62);
    PadHeader.SetTextSize(0.1);
    PadHeader.SetNDC();         // set to normalized coordinates
    PadHeader.SetTextAlign(23); // center/top alignment

    TH2 *h2pacnt = (TH2 *) cl->getHisto("hpacnt");
    if (h2pacnt == NULL)
    {
        DrawDeadServer(transparent[PACNT]);
        return -1;

    }

    for (int ipkt = 0; ipkt < 40; ipkt++)
    {

        stringstream name;
        name.str("");
        name << "hpacnt_" << ipkt;
        TH1 *h1 = (TH1 *) h2pacnt->ProjectionX(name.str().c_str(), ipkt + 1, ipkt + 1);

        pair<int, int> bl = ladder_map.get_barrel_ladder(ipkt);
        int barrel = bl.first;
        int ladder = bl.second;

        TP[PACNT][ipkt]->cd();
        h1->SetStats(0);
        h1->SetFillColor(3);
        h1->SetTitle("");
        h1->DrawCopy();

        PadHeader.DrawText(0.5, 1., Form("packet %i, B%iL%02i", VTXS_PACKET_BASE + ipkt, barrel, ladder));
        //TP[PACNT][ipkt]->Update();
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << "PAcnt Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[PACNT]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());
    TC[PACNT]->Update();

    printTimeInterval("DrawPACnt - End");

    return 0;
}

int SvxStripMonDraw::DrawErrors(const char *)
{

    printTimeInterval("DrawErrors - Start");

    int canvid = ERRORS;
    OnlMonClient *cl = OnlMonClient::instance();

    if (! gROOT->FindObject(canv_name[canvid].c_str()) )
    {
        MakeCanvas(canv_name[canvid].c_str());
    }
    TC[canvid]->Clear("D");

    TText PadHeader;
    PadHeader.SetTextFont(62);
    PadHeader.SetTextSize(0.1);
    PadHeader.SetNDC();         // set to normalized coordinates
    PadHeader.SetTextAlign(23); // center/top alignment

    stringstream name;
    TH1 *h1 = 0;
    name.str("");
    name << "htimeouterr";
    h1 = (TH1 *) cl->getHisto(name.str().c_str());
    if (h1 == 0)
    {
        DrawDeadServer(transparent[canvid]);
        return -1;

    }
    else
    {
        TP[canvid][0]->cd();
        h1->SetStats(0);
        h1->SetTitle("");
        h1->SetYTitle("fraction of events");
        h1->GetYaxis()->SetTitleSize(0.1);
        h1->GetYaxis()->SetTitleOffset(0.4);
        h1->SetFillColor(2);
        h1->GetXaxis()->SetLabelSize(0.08);
        h1->GetYaxis()->SetLabelSize(0.08);
        h1->DrawCopy();
    }

    {
        ostringstream desc;
        desc << "LDTB Timeouts";
        PadHeader.DrawText(0.5, 1., desc.str().c_str());
    }

    int colors[] = {3};
    gStyle->SetPalette(1, colors);
    gStyle->SetNumberContours(1);

    TH3 *h3mask = (TH3 *) cl->getHisto("hhybridmask");
    TH2* hknownmask = (TH2*) cl->getHisto("hknownmask");
    if (h3mask == NULL || hknownmask == NULL)
    {
        DrawDeadServer(transparent[canvid]);
        return -1;
    }

    TBox masked;
    masked.SetFillColor(kRed);

    for (int ircc = 0; ircc < 6; ircc++)
    {
        //REMINDER: hhybrid mask has axis packet,hybrid,rcc
        //below code expects 2D packet,hybrid
        name.str("");
        name << "hhybridmask_" << ircc;
        h3mask->GetZaxis()->SetRange(ircc + 1, ircc + 1);
        TH2 *h2 = (TH2 *) h3mask->Project3D("yx");
        h2->SetName(name.str().c_str());
        TH2 *hfailinit = (TH2*) h2->Clone("hfailinit");
        hfailinit->Reset();

        TP[canvid][2 + ircc]->cd();
        h2->SetStats(0);
        h2->SetTitle("");
        h2->SetYTitle("hybrid mask [0,3]");
        h2->GetYaxis()->SetTitleSize(0.1);
        h2->GetYaxis()->SetTitleOffset(0.4);
        h2->SetFillColor(3);
        h2->GetXaxis()->SetLabelSize(0.08);
        h2->GetYaxis()->SetLabelSize(0.08);
        h2->DrawCopy("COL");

        // Plot hybrids that were masked
        // due to failed feed
        for (int ipkt = 1; ipkt <= h2->GetNbinsX(); ipkt++)
        {
            for (int ihybrid = 0; ihybrid < 4; ihybrid++)
            {
	      /*
                if (h2->GetBinContent(ipkt, ircc) == 0 &&
                    hknownmask->GetBinContent(ipkt, ircc) == 0)
                {
                    hfailinit->SetBinContent(ipkt, ircc, 2);
                }
	      */
	      if (h2->GetBinContent(ipkt, ihybrid+1) == 0 &&
		  hknownmask->GetBinContent(ipkt, NRCC + (ircc*4) + ihybrid+1) == 0)
		{
		  masked.DrawBox(ipkt-0.5,ihybrid-0.5,ipkt+0.5,ihybrid+0.5);
		}
            }
        }
        hfailinit->SetFillColor(kRed);
        //hfailinit->DrawCopy("COL SAME");


        ostringstream desc;
        desc << "RCC" << ircc;
        PadHeader.DrawText(0.5, 1., desc.str().c_str());
        //TP[canvid][idib]->Update();
    }

    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();         // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    ostringstream runnostream;
    string runstring;
    time_t evttime = cl->EventTime("CURRENT");
    // fill run number and event time into string
    runnostream << "Run " << cl->RunNumber()
                << ", Time: " << ctime(&evttime);
    runstring = runnostream.str();
    transparent[canvid]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());
    TC[canvid]->Update();

    gStyle->SetPalette(1, 0);
    gStyle->SetNumberContours(20);

    printTimeInterval("DrawErrors - End");

    return 0;
}


int SvxStripMonDraw::MakePS(const char *what)
{
    OnlMonClient *cl = OnlMonClient::instance();
    ostringstream filename;
    int iret = Draw(what);
    if (iret) // on error no ps files please
    {
        return iret;
    }

    filename << ThisName << "_0_" << cl->RunNumber() << ".ps";
    TC[ADC]->Print(filename.str().c_str());

    return 0;
}

int SvxStripMonDraw::MakeHtml(const char *what)
{
    //Draw the normal DM plots
    int iret = Draw(what);

    //Draw the expert plots
    iret += DrawADC("SvxStripMonADC");
    iret += DrawSize("SvxStripMonSize");
    iret += DrawBClock("SvxStripMonBClock");
    iret += DrawRCCBClock("SvxStripMonRCCBClockWest",0);
    iret += DrawRCCBClock("SvxStripMonRCCBClockEast",1);
    iret += DrawRCCBClockError("SvxStripMonRCCBClockError");
    iret += DrawPACnt("SvxStripMonPACnt");
    iret += DrawErrors("SvxStripMonErrors");

    if (iret) // on error no html output please
    {
        return iret;
    }

    OnlMonClient *cl = OnlMonClient::instance();

    // Register the 1st canvas png file to the menu and produces the png file.
    ostringstream cavid;
    for (int icanvas = 0; icanvas < ncanvas; icanvas++)
    {
        if (icanvas == LADDER) continue; // ladder canvas is special
        if (TC[icanvas])
        {
            cavid.str("");
            cavid << icanvas;
            string pngfile = cl->htmlRegisterPage(*this, canv_short[icanvas], cavid.str(), "png");
            cl->CanvasToPng(TC[icanvas], pngfile);
        }
    }

    // TC[LADDER] is a temporary container for whichever ladder you are interested in
    // at that particular moment.
    // therefore, loop over each packet redrawing on this canvas
    // but save the content each time.
    ostringstream path;
    // draw only existing packets for reconfigured strips 
    // (packets 24101-24108 do not exist in Run16)
    for (int packetid = 24109; packetid <= 24140; packetid++)
    {
        pair<int, int> bl = ladder_map.get_barrel_ladder(packetid);
        int barrel = bl.first;
        int ladder = bl.second;


        cavid.str("");
        cavid << LADDER << "_" << packetid;
        path.str("");
        path << "Ladder/" << packetid << " - B" << barrel << "L" << ladder;
        string pngfile = cl->htmlRegisterPage(*this, path.str(), cavid.str(), "png");
        DrawLadder(packetid);
        cl->CanvasToPng(TC[LADDER], pngfile);
    }

    // commented out until here is something - all this will be saved in hpss and it should be somewhat meaningful
    //

    //   string logfile = cl->htmlRegisterPage(*this,"EXPERTS/Log","log","html");
    //   ofstream out(logfile.c_str());
    //   out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
    //       << "</TITLE></HEAD>" << endl;
    //   out << "<P>Some log file output would go here." << endl;
    //   out.close();

    //   string status = cl->htmlRegisterPage(*this,"EXPERTS/Status","status","html");
    //   ofstream out2(status.c_str());
    //   out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
    //       << "</TITLE></HEAD>" << endl;
    //   out2 << "<P>Some status output would go here." << endl;
    //   out2.close();
    cl->SaveLogFile(*this);
    return 0;
}

