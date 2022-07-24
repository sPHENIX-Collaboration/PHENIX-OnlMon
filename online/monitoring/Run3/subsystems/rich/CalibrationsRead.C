#include "CalibrationsRead.h"
#include "CrkCal.h"

#include "PHTimeStamp.h"
#include "PdbRichADC.hh"
#include "PdbRichPar.hh"


#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


#define N_CRK_PMT    5120

using namespace std;

  // calibration file formatt :
  //OK 7 -1.4666 2.99641 82.7099 34.4049 265713 37.0294 3.53468 3.0863 2742.34
  //(ch status) (ch number) (pedestal ADC mean value) (pedestal ADC sigma value) (1p.e. peak ADC mean value) (1p.e. peak ADC sigma value) (pedestal peak value) (1p.e. peak value) (2p.e. peak value) (3p.e. peak value) (chi square)

int CalibrationsRead::getcalibs (string calfile, float pedestal[], float sigped[], float pep[], float sigpep[], string taccalfile, float tacpedestal[], float tacsigped[], float tacpep[], float tacsigpep[], int dead[], int tacdead[])

{

  int return_state = 0;

  string status;
  int num;
  float dummy;

  time_t tp;
  time(&tp);
  struct tm *lctime = localtime(&tp);
  PHTimeStamp time = PHTimeStamp(1900 + lctime->tm_year,
                                 lctime->tm_mon, lctime->tm_mday, 0, 0, 0);


  // ADC section

  ifstream fp;
  ifstream fp2;
  fp.open(calfile.c_str());
  ostringstream fname;
  if(!fp){
    if(getenv("ONLMON_MAIN") != NULL)
      {
	fname<< getenv("ONLMON_MAIN")<<"/share/rich_calibration_constants.asc";
	fp2.open(fname.str().c_str());
      }
    if(fp2){
      for (int i = 0;i < 5120;i++)
	{
	  fp2 >> status >> num >> pedestal[i]
	      >> sigped[i] >> pep[i] >> sigpep[i] >> dummy
	      >> dummy >> dummy >> dummy >> dummy;
	  
	  if (status[0] == 'O' && status[1] == 'K' && status[2] == '\0')
	    dead[i] = 0;
	  else
	    {
	      dead[i] = 1;
	    }
	}
      fp2.close();
    }else{
      cout << "calibaration file not found. using FDB." << endl;
      PdbBankManager *bkMngr = PdbBankManager::instance();
      PdbApplication *app = bkMngr->getApplication();
      if (app->startRead())
        {
          app->abort();
          CrkCal<PdbRichADC> adc_cal("adc");
          adc_cal.fetch_DB(time);

          /*
            CrkCal<PdbRichPar> t0_cal("T0");
           CrkCal<PdbRichPar> tac_cal("TAC_GAIN");
            CrkCal<PdbRichPar> slew_cal("SLEW");
            
            t0_cal.fetch_DB(time);
            tac_cal.fetch_DB(time);
            slew_cal.fetch_DB(time);
            
            PdbRichPar t0, tac, slew;
          */


          PdbRichADC adc;
	  
          for (int i = 0;i < N_CRK_PMT;i++)
            {
              int PMT1;
              // int PMT2, PMT3, PMT4;
              float ped, pedw, PE, PEw, Nped, N1, N2, N3, chisqr;
              //      float t0v, tacv, slewv;

              adc_cal.getval(i, adc);

              adc.getValues(PMT1, ped, pedw, PE, PEw, Nped, N1, N2, N3, chisqr);

              /*
                t0_cal.getval(i,t0);
                tac_cal.getval(i,tac);
                slew_cal.getval(i,slew);
                t0.getValues(PMT2,t0v);
                tac.getValues(PMT3,tacv);
                slew.getValues(PMT4,slewv);
              */

              if (PE == 0 || PE < ped)
                { // the channel is disabled
                  dead[PMT1] = 1;
                  tacdead[PMT1] = 1;
                }
              else
                {
                  dead[PMT1] = 0;
                  pedestal[PMT1] = ped;
                  sigped[PMT1] = pedw;
                  pep[PMT1] = PE;
                  sigpep[PMT1] = PEw;
                }

            }
        }
      else
        {
          app->abort();
          cout << "FDB not found."
	       << " using default value." << endl;

          for (int iloop = 0;iloop < 5120;iloop++)
            {
              dead[iloop] = 0;
              pedestal[iloop] = -1.5;
              sigped[iloop] = 2.5;
              pep[iloop] = 70.;
              sigpep[iloop] = 2.5;
            }
          dead[28] = 1;
          dead[89] = 1;
          dead[90] = 1;
          dead[91] = 1;
          dead[92] = 1;
          dead[93] = 1;
          dead[94] = 1;
          dead[95] = 1;
          dead[176] = 1;
          dead[614] = 1;
          dead[715] = 1;
          dead[910] = 1;
          dead[2183] = 1;
          dead[2533] = 1;
          dead[2513] = 1;
          dead[2514] = 1;
          dead[1310] = 1;
          dead[1795] = 1;
          dead[1974] = 1;
          dead[1240] = 1;
          dead[2657] = 1;
          dead[2719] = 1;
          dead[2769] = 1;
          dead[2985] = 1;
          dead[3089] = 1;
          dead[3834] = 1;
          dead[4163] = 1;
          dead[4111] = 1;
          dead[4359] = 1;
          dead[4376] = 1;
          dead[4377] = 1;

        }
    }
  }else if (fp)
    {
      for (int i = 0;i < 5120;i++)
        {
	  fp >> status >> num >> pedestal[i]
	     >> sigped[i] >> pep[i] >> sigpep[i] >> dummy
	     >> dummy >> dummy >> dummy >> dummy;
          if (status[0] == 'O' && status[1] == 'K' && status[2] == '\0')
            dead[i] = 0;
          else
            {
              dead[i] = 1;
            }
        }
      fp.close();
    }

  // TAC section

  ifstream tacfp;
  ifstream tacfp2;
  ostringstream fname2;
  tacfp.open(taccalfile.c_str());
  if (!tacfp){
    if(getenv("ONLMON_MAIN") != NULL)
      {
	fname2<<getenv("ONLMON_MAIN")<<"/share/rich_tac_calibration_constants.asc";
	tacfp2.open(fname2.str().c_str());
      }
    if(tacfp2){
      for (int i=0; i<5120; i++)
        {
          tacfp2 >> status >> num >> tacpedestal[i] >> tacsigped[i] 
		 >> tacpep[i] >> tacsigpep[i];
          if (status[0] == 'O' && status[1] == 'K' && status[2] == '\0')
            tacdead[i] = 0;
          else
            tacdead[i] = 1;
        }
      tacfp2.close();
    }else{
      for (int iloop = 0;iloop < 5120;iloop++)
        {
          tacdead[iloop] = 0;
          tacpedestal[iloop] = 160.;
          tacsigped[iloop] = 1.7;
          tacpep[iloop] = 1.3;
          sigpep[iloop] = 160.;
        }
      
      tacdead[28] = 1;
      tacdead[89] = 1;
      tacdead[90] = 1;
      tacdead[91] = 1;
      tacdead[92] = 1;
      tacdead[93] = 1;
      tacdead[94] = 1;
      tacdead[95] = 1;
      tacdead[176] = 1;
      tacdead[614] = 1;
      tacdead[715] = 1;
      tacdead[910] = 1;
      tacdead[2183] = 1;
      tacdead[2533] = 1;
      tacdead[2513] = 1;
      tacdead[2514] = 1;
      tacdead[1310] = 1;
      tacdead[1795] = 1;
      tacdead[1974] = 1;
      tacdead[1240] = 1;
      tacdead[2657] = 1;
      tacdead[2719] = 1;
      tacdead[2769] = 1;
      tacdead[2985] = 1;
      tacdead[3089] = 1;
      tacdead[3834] = 1;
      tacdead[4163] = 1;
      tacdead[4111] = 1;
      tacdead[4359] = 1;
      tacdead[4376] = 1;
      tacdead[4377] = 1;

      tacdead[1360] = 1;
      tacdead[1361] = 1;
      tacdead[1362] = 1;
      tacdead[1363] = 1;
      tacdead[1364] = 1;
      tacdead[1365] = 1;
      tacdead[1366] = 1;
      tacdead[1367] = 1;
      tacdead[2600] = 1;
      tacdead[2601] = 1;
      tacdead[2602] = 1;
      tacdead[2603] = 1;
      tacdead[2604] = 1;
      tacdead[2605] = 1;
      tacdead[2606] = 1;
      tacdead[2607] = 1;
      tacdead[3176] = 1;
      tacdead[3177] = 1;
      tacdead[3178] = 1;
      tacdead[3179] = 1;
      tacdead[3180] = 1;
      tacdead[3181] = 1;
      tacdead[3182] = 1;
      tacdead[3183] = 1;

      return_state += 2;
    }
  }else if (tacfp)
    {
      for (int i = 0;i < 5120;i++)
        {
          tacfp >> status >> num >> tacpedestal[i] >> tacsigped[i] 
	        >> tacpep[i] >> tacsigpep[i];

          if (status[0] == 'O' && status[1] == 'K' && status[2] == '\0')
            tacdead[i] = 0;
          else
            tacdead[i] = 1;
        }
      tacfp.close();

      return_state += 3;
    }



  return return_state;
}

