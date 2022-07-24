#include "SvxStripMon.h"
#include "SvxStripMonDefs.h"

#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <Event.h>
#include <msg_profile.h>
#include <msg_control.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TRandom3.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

// this is an ugly hack, the gcc optimizer has a bug which
// triggers the uninitialized variable warning which
// stops compilation because of our -Werror 
#include <boost/version.hpp> // to get BOOST_VERSION
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 4 && BOOST_VERSION == 105700 )
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma message "ignoring bogus gcc warning in boost header lexical_cast.hpp"
#include <boost/lexical_cast.hpp>
#pragma GCC diagnostic warning "-Wuninitialized"
#else
#include <boost/lexical_cast.hpp>
#endif
// boost::split makes this version gcc 4.4.7  barf otherwise
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 4)

#pragma GCC diagnostic ignored "-Warray-bounds"

#endif
// boost::split makes this version gcc 4.3.2  barf otherwise
// #if (__GNUC__ == 4 && __GNUC_MINOR__ == 3)

// #pragma GCC diagnostic ignored "-Warray-bounds"

// #endif

using namespace std;

enum {TRGMESSAGE = 1, FILLMESSAGE = 2};

// replace multiple white spaces with single white space
// copied from the web
bool
consecutiveWhiteSpace( char a, char b )
{
    return isspace(a) && isspace(b);
}

SvxStripMon::SvxStripMon(const char *name): 
  OnlMon(name),
  evtcnt(0),
  idummy(0),
  h2sadc(NULL),
  h2live(NULL),
  vtxs_pars(NULL),
  hsadc_bytime(NULL),
  hsadc_bytime_nohot(NULL),
  hnbbc_bytime(NULL),
  hmulti(NULL),
  h1rcc(NULL),
  bor_time(0),
  hcellpro(NULL),
  hcellid(NULL),
  hbclock(NULL),
  hrccbclock(NULL),
  hrccbclockerror(NULL),
  hpacnt(NULL),
  htimeouterr(NULL),
  hhybridmask(NULL),
  hknownmask(NULL)
{
  // only intializers, its hard to debug if code crashes already
  // during a new SvxStripMon()
  memset(gl1bclock_offset,0,sizeof(gl1bclock_offset));
  memset(h2adc,0,sizeof(h2adc));
  memset(hcelltmp,0,sizeof(hcelltmp));
  memset(_timeout_counter,0,sizeof(_timeout_counter));
  memset(ig_hot,0,sizeof(ig_hot));
  memset(ig_dead,0,sizeof(ig_dead));
  memset(ig_hotchip,0,sizeof(ig_hotchip));
  return ;
}

SvxStripMon::~SvxStripMon()
{
    for (int ipacket = 0; ipacket < VTXS_NPACKET; ipacket++)
    {
        for (int rcc = 0; rcc < NRCC; rcc++)
        {
            delete hcelltmp[ipacket][rcc];
        }
    }
    return ;
}

int SvxStripMon::Init()
{
    // use printf for stuff which should go the screen but not into the message
    // system (all couts are redirected)

    // Initialize BBC Module
    //bbccalib = new BbcCalib();
    //bevt = new BbcEvent();

    OnlMonServer *se = OnlMonServer::instance();

    ostringstream name;
    ostringstream desc;

    h2sadc = new TH2F("svxstrip_h2sadc", ";;", 40, -0.5, 39.5, ny, -0.5, ny - 0.5);
    se->registerHisto(this, h2sadc);
    h2live = new TH2F("svxstrip_h2live", ";;", 40, -0.5, 39.5, NRCC * NCHIP, -0.5, NRCC * NCHIP - 0.5);
    se->registerHisto(this, h2live);
    vtxs_pars = new TH1D("vtxs_params", "vtxstrip parameters", NUMBINS, 0, NUMBINS);
    se->registerHisto(this, vtxs_pars);
    hsadc_bytime = new TH1F("svxstrip_hsadc_bytime", ";;", 10000, 0, 1000000);
    se->registerHisto(this, hsadc_bytime);
    hnbbc_bytime = new TH1F("svxstrip_hnbbc_bytime", ";;", 10000, 0, 1000000);
    se->registerHisto(this, hnbbc_bytime);
    hsadc_bytime_nohot = new TH1F("svxstrip_hsadc_bytime_nohot", ";;", 10000, 0, 1000000);
    se->registerHisto(this, hsadc_bytime_nohot);
    hmulti = new TH1F("svxstrip_hmulti", ";Log_{10} Multiplicity;Counts", 256, 0.1, log10(344065 - 0.5));
    se->registerHisto(this, hmulti);

    const int ntimebin = 120;
    const int max_time = 60 * 60;
    const int min_time = 0;

    int nbin = 6 * 24; //nrcc * nladder for B3
    h1rcc = new TH3F("vtxsh1rcccount", ";;;",
                     nbin, -0.5, nbin - 0.5, //nrcc*nladder
                     4, -0.5, 3.5, //barrel
                     ntype, -0.5, ntype - 0.5 //ntype
	);
    se->registerHisto(this, h1rcc);

    hrccbclockerror = new TH2F("hrccbclockerror", ";;;",
                     nbin, -0.5, nbin - 0.5, //nrcc*nladder
                     4, -0.5, 3.5 //barrel
	);
    se->registerHisto(this, hrccbclockerror);


    hcellpro = new TH3F("hcellpro", "",
                        ntimebin, min_time, max_time,
                        VTXS_NPACKET, -0.5, VTXS_NPACKET - 0.5, //packet
                        NRCC, -0.5, NRCC - 0.5 //RCC
	);
    se->registerHisto(this, hcellpro);



    hcellid = new TH3I("hcellid", "",
                       70, -0.5, 69.5,
                       VTXS_NPACKET, -0.5, VTXS_NPACKET - 0.5, //packet
                       NRCC, -0.5, NRCC - 0.5 //RCC
	);
    se->registerHisto(this, hcellid);


    hrccbclock = new TH3I("hrccbclock", "",
			   21, -10.5, 10.5,
			   VTXS_NPACKET, -0.5, VTXS_NPACKET - 0.5, //packet
			   NRCC, -0.5, NRCC - 0.5 //RCC
	);
    se->registerHisto(this, hrccbclock);


    hbclock = new TH2I("hbclock", "",
                       21, -10.5, 10.5,
                       VTXS_NPACKET, -0.5, VTXS_NPACKET - 0.5 //packet
	);
    se->registerHisto(this, hbclock);


    hpacnt = new TH2I("hpacnt", "",
                      65, -0.5, 64.5,
                      VTXS_NPACKET, -0.5, VTXS_NPACKET - 0.5 //packet
	);
    se->registerHisto(this, hpacnt);


    htimeouterr = new TH1F("htimeouterr", "", 40, 0.5, 40.5);
    se->registerHisto(this, htimeouterr);


    hhybridmask = new TH3I("hhybridmask", "",
                           40, 0.5, 40.5, //packet
                           4, -0.5, 3.5, //hybrid
                           NRCC, -0.5, NRCC - 0.5 //RCC
	);
    se->registerHisto(this, hhybridmask);


    hknownmask = new TH2F("hknownmask", "known mask",
                          VTXS_NPACKET, -0.5, VTXS_NPACKET + 0.5,
                          NRCC + NRCC * 4, -0.5, NRCC + NRCC * 4 + 0.5);
    se->registerHisto(this, hknownmask);

    for (int ipacket = 0; ipacket < VTXS_NPACKET; ipacket++)
    {
        for (int rcc = 0; rcc < NRCC; rcc++)
        {
            name.str("");
            name << "h2svxstrip_adc_" << ipacket << "_" << rcc;
            desc.str("");
            desc << "Packet: " << VTXS_PACKET_BASE + ipacket
                 << " RCC: " << rcc << ";Channel;ADC";
            int nx = NCHIP * NCHANNEL;
            h2adc[ipacket][rcc] = new TH2F(name.str().c_str(),
                                           desc.str().c_str(),
                                           64, -0.5, nx - 0.5,
                                           64, -0.5, ny - 0.5);
            se->registerHisto(this, h2adc[ipacket][rcc]);

            //only used internally, so no need to make 3D.
            name.str("");
            name << "hcelltmp_" << ipacket << "_" << rcc;
            hcelltmp[ipacket][rcc] = new TH1F(name.str().c_str(), "",
                                              46, 0.5, 46.5);
        }
    }
    Reset();
    return 0;
}

int
SvxStripMon::BeginRun(const int runno)
{
    // if you need to read calibrations on a run by run basis
    // this is the place to do it
    OnlMonServer *se = OnlMonServer::instance();
    bor_time = se->BorTicks() ; // Begin of run time

    read_config_file(ig_hot, "SvxStripKnownHotMap.txt");
    read_config_file(ig_dead, "SvxStripDeadMap.txt");
    read_config_file(ig_hotchip, "SvxStripKnownHotMapChip.txt");
    ReadFeedConfig();

    return 0;
}

int
SvxStripMon::process_event(Event *evt)
{
    evtcnt++;
    vtxs_pars->SetBinContent(EVENTCOUNTBIN, evtcnt);
    hnbbc_bytime->Fill( evtcnt );

    //OnlMonServer *se = OnlMonServer::instance();
    //if ( se->Trigger( "BBCLL1(>1 tubes)" ) )
    // For now we only use BB triggers

    time_t time = evt->getTime() - bor_time ;
    TRandom3 rnd;
    //iValue returns int. changing some unsigned to signed
    int gl1bclock = 0;

    Packet *p = evt->getPacket(14001);
    if (p)
    {
      gl1bclock = (p->iValue(0,"BEAMCTR0") & 0xffff);

        delete p;
    }

    unsigned int multiplicity = 0;
    for (int ipacket = 0; ipacket < VTXS_NPACKET; ipacket++)
    {
        int packetid = VTXS_PACKET_BASE + ipacket;
        Packet *p = evt->getPacket(packetid);
        if (!p) continue;

        int fbclk = p->iValue(0,"BCLCK") & 0xffff;  // Word Type 4 (FEM BCLK)
	int flagword = p->iValue(0, "FLAG");
        int pacnt = p->iValue(0,"PARSTCTR");// Word Type 10-2 (PARST counter)
        int ldtbd = flagword & 0x80; // Word Type 10-3 (LDTB data bit)

        if (ldtbd)
            _timeout_counter[ipacket]++;

        htimeouterr->SetBinContent(ipacket + 1, float(_timeout_counter[ipacket]) / evtcnt);

        //       hpkterr[ipkt]->SetBinContent(1,1);
        //     if(fifth>0)
        //       hpkterr[ipkt]->SetBinContent(2,1);

        int rcc_enabled[6] = {0};
        for (int ircc = 0; ircc < 6; ++ircc)
        {
            // if (rawdata[nwords - 4] & (1 << ircc))
            //     rcc_enabled[5 - ircc] = 1;
	  rcc_enabled[ircc] = p->iValue(ircc, "ENABLED");
	  
        }

        //calculate the difference between the GL1 bclk and
        //the FEM bclk once. This will be used to test if
        //things have gone awry.
        if (gl1bclock_offset[ipacket] == 0)
        {
	  gl1bclock_offset[ipacket] = fbclk - gl1bclock;
        }

        //Now take the difference between the GL1 and FEM bclk
        //and compare it to the offset calculated above.
        hbclock->Fill(fbclk - gl1bclock - gl1bclock_offset[ipacket], ipacket);

        hpacnt->Fill(pacnt, ipacket);

	for (int ircc = 0; ircc < 6; ircc++)
	  {
	    if ( evtcnt % 1000 == 0 && evtcnt > 0 )
	      {
		// bool enabledHybrid = false;

                // for (unsigned int ih = 0; ih < 4; ++ih)
		//   {
		//     if ( hhybridmask->GetBinContent(ipacket + 1, ih + 1, ircc + 1) > 0 )
		//       {
		// 	enabledHybrid = true;
		// 	continue;
		//       }
		//   }
		// if (enabledHybrid)
		int binx = hcellpro->GetXaxis()->FindBin(time);
		if (hcelltmp[ipacket][ircc]->GetEntries() > 0)
		  {
                    float tmpRMS = hcelltmp[ipacket][ircc]->GetRMS();
                    hcellpro->SetBinContent(binx, ipacket + 1, ircc + 1, tmpRMS);
                    hcellpro->SetBinError(binx, ipacket + 1, ircc + 1, 0.0000001);
		  }
                else
		  {
                    hcellpro->SetBinContent(binx, ipacket + 1, ircc + 1, -10);
                    hcellpro->SetBinError(binx, ipacket + 1, ircc + 1, 0.0000001);
		  }
                hcelltmp[ipacket][ircc]->Reset();
	      }
	    else
	      {
		int rcc_hybrid = p->iValue(ircc, "RCCHYBRID");

		int minh = -1;

                for (unsigned int ih = 0; ih < 4; ++ih)
		  {
		    if ((rcc_hybrid>>ih) & 0x1)
		      {
			hhybridmask->SetBinContent(ipacket + 1, ih + 1, ircc + 1, 1);
			if (minh < 0) minh = ih;
		      }
		  }

		if (!rcc_enabled[ircc] || minh<0) continue;

		int cellid = p->iValue(ircc,minh*3,"CELLID");//check cellid of first chip of hybrid minh
		hcelltmp[ipacket][ircc]->Fill(cellid);
		hcellid->Fill(cellid, ipacket, ircc);
	      }
	  }

        pair<int, int> bl = ladder_map.get_barrel_ladder(ipacket);
        int barrel = bl.first;
        int ladder = bl.second;

        for (int rcc = 0; rcc < NRCC; rcc++)
        {
            int nrcc_hit = 0;
            for (int chip = 0; chip < NCHIP; chip++)
            {
                for (int i = 0; i < NCHANNEL; i++)
                {
                    int channel = chip * 128 + i;
                    int adc = p->iValue(rcc, channel);
                    if (adc < 1) continue;

                    h2adc[ipacket][rcc]->Fill(channel, adc);

                    //this indexes all strip ladders from 0-39
                    int iladder = ladder_map.get_ladder_index(ipacket);

                    //remove hot rcc
                    if ((ig_hot[barrel][ladder][rcc] != 1) && (ig_hotchip[barrel][ladder][rcc][chip] != 1))
                    {
                        double binc = h2sadc->GetBinContent(iladder + 1, adc + 1);
                        binc += 1.;
                        h2sadc->SetBinContent(iladder + 1,adc + 1, binc);
                    }

                    if (adc > 35 && adc < 256)
                    {
                        h2live->Fill(iladder,rcc * NCHIP + chip);


                        float nbbc = vtxs_pars->GetBinContent(EVENTCOUNTBIN);
                        hsadc_bytime->Fill( nbbc, adc );

                        //remove hot rccs or chips
                        if ((ig_hot[barrel][ladder][rcc] != 1) && (ig_hotchip[barrel][ladder][rcc][chip] != 1))
                        {
                            hsadc_bytime_nohot->Fill( nbbc, adc );
                            multiplicity++;
                        }
                    }

                    if ((adc > 50) && (ig_hotchip[barrel][ladder][rcc][chip] != 1))
                    {                     
		      nrcc_hit++;
                    }

                } // channel
            } // chip
			
            int rccbin = 1 + ladder * NRCC_PER_LADDER[barrel] + rcc;
            double old_content = h1rcc->GetBinContent(rccbin, barrel + 1, TYPE_RAW + 1);
            double new_content = old_content + nrcc_hit;
            h1rcc->SetBinContent(rccbin, barrel + 1, TYPE_RAW + 1, new_content);

	    if (rcc_enabled[rcc])
	    {	      
		int rccbclk = p->iValue(rcc, "RCCBCLK");
		hrccbclock->Fill(rccbclk - (int)fbclk, ipacket, rcc);
		if (fabs(rccbclk - (int)fbclk) > 1)
		{
		    hrccbclockerror->Fill(rccbin-1,barrel);
		}
	    }
        } // rcc

        delete p;
    }
    hmulti->Fill(log10(multiplicity));
    return 0;
}

int SvxStripMon::Reset()
{
    // reset our internal counters
    evtcnt = 0;
    idummy = 0;
    memset(_timeout_counter,0,sizeof(_timeout_counter));
    return 0;
}

int SvxStripMon::read_config_file(int data[4][24][6], const string &fname)
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
    int barrel, ladder, chip;
    while ( getline(infile, line) )
    {
        if ( line[0] == '#' || line[0] == '/' ) continue;
        istringstream iss_line(line.data());
        iss_line >> barrel >> ladder >> chip;
        if ( barrel >= 4 || barrel < 2 ||
	     ( barrel == 2 && ladder >= 16 ) ||
	     ( barrel == 3 && ladder >= 24 ) ||
	     ladder < 0 || chip < 0 ||
	     ( barrel == 2 && chip > 5 ) ||
	     ( barrel == 3 && chip > 6 )
	    )
        {
            cerr << "Invalid input line: " << line << endl;
            continue;
        }
        data[barrel][ladder][chip] = 1;
    }
    return 0;
}
int SvxStripMon::read_config_file(int data[4][24][6][12], const string &fname)
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
    int barrel, ladder, rcc, chip;
    while ( getline(infile, line) )
    {
        if ( line[0] == '#' || line[0] == '/' ) continue;
        istringstream iss_line(line.data());
        iss_line >> barrel >> ladder >> rcc >> chip;
        if ( barrel >= 4 || barrel < 2 || 
	     ladder < 0 || ladder >= 24 ||
	     rcc < 0 || rcc >= 5 ||
	     chip < 0 || chip >= 12 ||
	     ( barrel == 2 && ladder >= 16 ) ||
	     ( barrel == 2 && rcc > 5 )
	    )
        {
            cerr << "Invalid input line: " << line << endl;
            continue;
        }
        data[barrel][ladder][rcc][chip] = 1;
    }
    return 0;
}


int
SvxStripMon::ReadFeedConfig()
{
    ifstream eastconf;
    string FullLine;  // a complete line in the config file
    string filename[2] = {"/home/phnxvtx/slowcontrol/StripixelSC/configfiles/vmeconfig_east_comm.txt",
                          "/home/phnxvtx/slowcontrol/StripixelSC/configfiles/vmeconfig_west_comm.txt"
    };

    for (int ifile = 0; ifile < 2; ifile++)
    {
        eastconf.open(filename[ifile].c_str(), ifstream::in);
        getline(eastconf, FullLine);
        while ( !eastconf.eof())
        {

            FullLine.erase( std::unique(FullLine.begin(), FullLine.end(), consecutiveWhiteSpace), FullLine.end() );
            vector<string> tokens;
            boost::split(tokens, FullLine, boost::is_any_of(" "));
            vector<string> ladder;
            boost::split(ladder, tokens[0], boost::is_any_of(":"));
            unsigned int mypacket = 0;


            int tmpbarrel, tmpladder;
            sscanf(ladder[2].c_str(),"B%d_L%d",&tmpbarrel,&tmpladder);
            mypacket = ladder_map.get_packetid(tmpbarrel,tmpladder);

            //cout << "ladder: " << ladder[2].c_str() << " barrel=" << tmpbarrel << " ladder=" << tmpladder << " packet " << mypacket << endl;

            ifstream rccconf;
            string confline;
            string conffilename = "/home/phnxvtx/slowcontrol/StripixelSC/" + tokens[2];
            rccconf.open(conffilename.c_str(), ifstream::in);
            getline(rccconf, confline);
            int maskval[7];
            memset(maskval, 0, sizeof(maskval));
            int icnt = 0;
            while (!rccconf.eof())
            {
                vector<string> mask;
                boost::split(mask, confline, boost::is_any_of(" "));
                // rcc mask is first
                try
                {
                    maskval[icnt] = boost::lexical_cast<int>(mask[0]);
                }
                catch (const boost::bad_lexical_cast &e)
                {
                    cout << "could not convert " << mask[0] << " to int" << endl;
                }
                icnt++;
                getline(rccconf, confline);
            }
            rccconf.close();

            int index = mypacket - VTXS_PACKET_BASE; // index start at zero
            for (int i = 0; i < 6; i++)
            {
                if ( maskval[0] >> i & 0x1)
                {
		    //cout << "packet " << mypacket << " masked rcc no " << i << endl;
                    hknownmask->SetBinContent(index+1, i + 1, 1);
                    for (int j = 0; j < 4; j++)
                    {
                        hknownmask->SetBinContent(index+1, NRCC + (i * 4) + j + 1, 1);
                    }
                }
            }
            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if ( maskval[i + 1] >> j & 0x1)
                    {
			//cout  << "packet " << mypacket << " RCC " << i << " hybrid " << j << " disabled" << endl;
                        hknownmask->SetBinContent(index+1, NRCC + (i * 4) + j + 1, 1);
                    }
                }
            }

            getline(eastconf , FullLine );
        }
        eastconf.close();
    }

    return 0;
}
