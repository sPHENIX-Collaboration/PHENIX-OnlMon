#include "SvxPixelMon.h" 
#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <Event.h>
#include <msg_profile.h>
#include <msg_control.h>


#include <cstdlib>


#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

SvxPixelMon::SvxPixelMon(const char *name): OnlMon(name), 
                                            evtcnt(0),
                                            bor_time(0),
                                            WEST(0),
                                            EAST(1),
                                            SOUTH(0),
                                            NORTH(1)
{
  return ;
}

SvxPixelMon::~SvxPixelMon()
{
  return ;
}

int SvxPixelMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();

  desc_arm[WEST]="WEST";
  desc_arm[EAST]="EAST";
  desc_side[NORTH]="NORTH";
  desc_side[SOUTH]="SOUTH";

  ostringstream name;
  ostringstream desc;

  /// Histograms for hit counts summed up for all pixel in a ladder
  vtxp_pars = new TH1D("vtxp_params","vtxpixel parameters",NUMBINS,0,NUMBINS);
  se->registerHisto(this,vtxp_pars);

  const int ntimebin=64;
  const int max_time=60*90+60;
  const int min_time=-60;
  pmulti_time=new TProfile("vtxp_multi_time","",ntimebin,min_time,max_time);
  se->registerHisto(this,pmulti_time);

  pmulti2d = new TProfile2D("vtxp_multi_time_2d","", 60,0,60, ntimebin,min_time,max_time);
  se->registerHisto(this,pmulti2d);

  for(int spiro=0; spiro<60; spiro++)
    {
      name.str(""); desc.str("");
      name<<"vtxp_multi_time_" << spiro;
      pmulti[spiro]=new TProfile(name.str().c_str(),"",ntimebin,min_time,max_time);
      se->registerHisto(this,pmulti[spiro]);

      for(int chip=0; chip<8; chip++)
        {
          name.str(""); desc.str("");
          name<<"vtxp_chipmap_" << spiro << "_" << chip;
          desc<<"Packet: " << vtxp_packet_base+spiro << " Chip: " << chip;
          h2chipmap[spiro][chip]=new TH2F(name.str().c_str(),desc.str().c_str(),32,-0.5,31.5,256,-0.5,255.5);
          se->registerHisto(this,h2chipmap[spiro][chip]);
        }
    }

  for(int barrel=0; barrel<2; barrel++)
    {
      for(int arm=0; arm<2; arm++)
        {
          for(int type=0; type<ntype; type++)
            {
              name.str(""); desc.str("");
              name << "vtxph1chipcount_" << barrel << "_" << arm << "_" << type;
              desc << "Barrel: " << barrel << "-" << desc_arm[arm] << " " << desc_type[type];
              int nbin=(barrel+1)*16*5;
              h1chipcount[barrel][arm][type]=new TH1F(name.str().c_str(),desc.str().c_str(),nbin,-0.5,nbin-0.5);
              se->registerHisto(this,h1chipcount[barrel][arm][type]);
            }
          name.str(""); desc.str("");
          name << "vtxph2map_" << barrel << "_" << arm;
          const int ncol=32*16;            // 32 columns/chip, 16 chips/ladder
          const int ncol_bin=64;
          const int nrow=256*5*(barrel+1); // 256 rows  /chip, 5/10 ladders for barrel 0/1 in one arm
          const int nrow_bin=160*(barrel+1);
          h2map[barrel][arm]=new TH2F(name.str().c_str(),"",nrow_bin,-0.5,nrow-0.5,ncol_bin,-0.5,ncol-0.5);
          se->registerHisto(this,h2map[barrel][arm]);
        }
    }

  h1detmult=new TH1F("hvtxph1detmult","Vertex-Pixel Multiplicity;Multiplicity;Counts",1024,-0.5,1023.5);
  se->registerHisto(this,h1detmult);
  
  
  for ( int packet = 0; packet < 60; packet++ )
    {
      for ( int chip = 0; chip < 8; chip++ )
	{
	  for(int column = 0; column < 32; column++)
	    {
	      ig_hotc[packet][chip][column] = 0;
	    }  
	}
    }


  int iret = read_config_file(ig_hotc, "SvxPixelMaskedColumns.txt");
  iret = 0;

  return 0;
}

int SvxPixelMon::read_config_file(unsigned int data[60][8][32], const std::string fname)
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
  int packet,chip,column;
  while ( getline(infile, line) )
    {
      if ( line[0] == '#' || line[0] == '/' ) continue;
      istringstream iss_line(line.data());
      iss_line >> packet >> chip >> column;
      if ( packet > 24060 || packet < 24001 || chip < 0 || chip >= 8 || column >=32 || column < 0)
	{
	  cerr << "Invalid input line: " << line << endl;
	  continue;
        }
      data[packet-24001][chip][column] = 1;
    }
  return 0;
}


int SvxPixelMon::BeginRun(const int runno)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  OnlMonServer* se=OnlMonServer::instance();
  bor_time=se->BorTicks() ;// Begin of run time

  return 0;
}

int SvxPixelMon::process_event(Event *evt)
{
  int chip_number, chip_bin;

  if (evt->getEvtType() != 1)
    {
      return 0;
    }
  evtcnt++;
  
  vtxp_pars->SetBinContent(EVENTCOUNTBIN,evtcnt);
  time_t time=evt->getTime() - bor_time ;
  unsigned int detector_multi=0;
  for(int ipacket=0; ipacket<vtxp_npacket; ipacket++)
    {
      unsigned int half_ladder_multi=0;
      int packetid=vtxp_packet_base+ipacket;

      Packet *p = evt->getPacket(packetid);
      if(!p)
        {
          //cout << "could not find packet: " << packetid << " , skipping..." << endl;
          continue;
        }

      unsigned int barrel=9999;
      unsigned int ladder=9999;
      unsigned int side=9999;
      unsigned int arm=9999;

      if( ipacket < 30 ) arm=EAST;
      else arm=WEST;

      if( ipacket % 30 < 15 ) side=NORTH;
      else side=SOUTH;

      if ( ipacket<5  ) 
        {
          barrel = 0;
          ladder = ipacket+5;
        } 
      else if ( ipacket<15 )
        {
          barrel = 1;
          ladder = ipacket+5;
        }
      else if ( ipacket<20 )
        {
          barrel = 0;
          ladder = ipacket-10;
        }
      else if ( ipacket<30 ) 
        {
          barrel = 1;
          ladder = ipacket-10;
        } 
      else if ( ipacket<35 ) 
        {
          barrel = 0;
          ladder = ipacket-30;
        } 
      else if ( ipacket<45 ) 
        {
          barrel = 1;
          ladder = ipacket-35;
        } 
      else if ( ipacket<50 ) 
        {
          barrel = 0;
          ladder = ipacket-45;
        } 
      else
        {
          barrel = 1;
          ladder = ipacket-50;
        }
  
      for(int ichip=0; ichip<8; ichip++)
        {
          //int sensor = (side==0) ? ichip/4+2 : ichip/4;
          /// sensor numbering : from south(=0) to north(=3) for west arm
          ///                    from north(=0) to south(=3) for east arm
          for(int irow=0; irow<255; irow++)
            {
              int rowhit=p->iValue(ichip,irow);
              if( rowhit==0 ) continue;  // no need to go through the whole thing if it's zero
              for(int icolumn=0; icolumn<32; icolumn++)
                {
		  if(ig_hotc[ipacket][ichip][icolumn] == 1) continue;
                  if( rowhit & ( 1 << icolumn ) )
                    {
                      //int iladder=barrel*10+ladder;
                      if(arm == WEST){
			chip_number=(7-ichip)+side*8;
			chip_bin = ((barrel==0) ? (ichip+(1-side)*8)+16*(ladder%5) : (ichip+(1-side)*8)+16*(ladder%10));
		      }
                      else{
			chip_number=ichip+side*8;
			chip_bin = ((barrel==0) ? (ichip+side*8)+16*(ladder%5) : (ichip+side*8)+16*(ladder%10));
		      }
                      // bin for the 1D chipcount histogram
                      int phibin;
                      if(arm == WEST)phibin = (255-irow)+ladder*256;
                      else phibin =(barrel==0) ? (255-irow)+(ladder-5)*256 : (255-irow)+(ladder-10)*256 ;
                      /// (ichip,icolumn)=(0,0), iz=31; (ichip,icolumn)=(0,31), iz=0
                      int ybin=phibin;
                      int zbin;
                      if(arm == WEST) zbin=32*(chip_number+1)-(32-(icolumn-1));
                      else zbin=32*(chip_number+1)-icolumn-1;
                      //int iz=32*(ichip%4+1)-icolumn-1;
                      //int senSec = SvxAddressObject.getPixelSensorSection(iz);
                      /*
                        if (icolumn>0 && icolumn<31) {
                        senSec = 6-ichip%4*2;
                        } else if (icolumn==0) {
                        senSec = (ichip%4==3) ? 0 : 6-ichip%4*2-1;
                        } else {
                        senSec = (ichip%4==0) ? 6 : 6-ichip%4*2+1;
                        }
                      */

                      /*
                      SvxRawhit *rhit = d_rhit->addRawhit();
                      rhit->set_svxSection(0);
                      rhit->set_layer(barrel);
                      rhit->set_ladder(ladder);
                      rhit->set_sensor(sensor);
                      rhit->set_sensorSection(senSec);
                      rhit->set_sensorReadout(0);
                      rhit->set_sensorType(1);
                      rhit->set_channel(irow*32+icolumn);
                      rhit->set_adc(1);
                      rhit->set_pixelROC(ichip);
                      rhit->set_pixelModule(module);
                      */
                      /*
                      if(arm==EAST)
                        {
                          ybin=(barrel==0) ? 10*256-phibin : 20*256-phibin;
                        }
                      */
                      // speeds up filling, only works if NBINS = nrow or ncolumn
                      double binc = h2chipmap[ipacket][ichip]->GetBinContent(icolumn+1,irow+1);
                      binc += 1.;
                      h2chipmap[ipacket][ichip]->SetBinContent(icolumn+1,irow+1,binc);
		      


                      h1chipcount[barrel][arm][0]->Fill(chip_bin);
                      h2map[barrel][arm]->Fill(ybin,zbin);
                      detector_multi++;
                      half_ladder_multi++;
                    }
                }
            }
        }
      pmulti[ipacket]->Fill(time,half_ladder_multi);
      pmulti2d->Fill(ipacket,time,half_ladder_multi);
      delete p;
    }

  pmulti_time->Fill(time,detector_multi);
  return 0;
}

int SvxPixelMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int SvxPixelMon::DBVarInit()
{
  return 0;
}
