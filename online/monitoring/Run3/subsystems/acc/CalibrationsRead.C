#include "CalibrationsRead.h"

#include "ACCMonDefs.h"

#include <PHTimeStamp.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

  // calibration file format:
  //1 30.1345 13.5643 3.3552 
  //(ch status) (ch mean) (ch sigma_mean) (ch error_mean) (cg chi square)

int CalibrationsRead::getcalibs(float *pep,float *sigpep,float *pedestal,float *sigped,float *tacped,float *sigtacped,int *dead,int *tacdead)
{
  int return_state = 0;
  int status;
  float dummy;

  time_t tp;
  time(&tp);
  struct tm *lctime = localtime(&tp);
  PHTimeStamp time = PHTimeStamp(1900 + lctime->tm_year,
                                 lctime->tm_mon, lctime->tm_mday, 0, 0, 0);

  // ADC gain section
  FILE *fp = 0;
  char fname[200];
  if ((getenv("ONLMON_MAIN") != NULL))
    {
      sprintf(fname, "%s/AccCalib.adcgain.new",
              getenv("ONLMON_CALIB"));
      fp = fopen(fname, "r");
    }
  if (fp)
    {
      for (int i = 0;i < 320;i++)
        {
          fscanf(fp, "%d %f %f %f %f", &status, &pep[i], &sigpep[i], &dummy, &dummy);
	  if (status==0) { dead[i] = ACCPMT_DEAD;}
	  else if (status==1) { dead[i] = ACCPMT_ALIVE;}
        }
      fclose(fp);
    }

  // ADC pedestal section
  if ((getenv("ONLMON_MAIN") != NULL))
    {
      sprintf(fname, "%s/AccCalib.adcpedestal.new",
              getenv("ONLMON_CALIB"));
      fp = fopen(fname, "r");
    }
  if (fp)
    {
      for (int i = 0;i < 320;i++)
        {
          fscanf(fp, "%d %f %f %f %f", &status, &pedestal[i], &sigped[i], &dummy, &dummy);
        }
      fclose(fp);
    }
  // TAC section

  if ((getenv("ONLMON_MAIN") != NULL))
    {
      sprintf(fname, "%s/AccCalib.tdcpedestal.new",
              getenv("ONLMON_CALIB"));
      fp = fopen(fname, "r");
    }

  if (fp)
    {
      for (int i = 0;i < 320;i++)
        {
          fscanf(fp, "%d %f %f %f %f", &status, &tacped[i], &sigtacped[i], &dummy, &dummy);
	  tacdead[i] = status;
        }
      fclose(fp);
    }
  else
    {
      for (int iloop = 0;iloop < 320;iloop++)
        {
          tacdead[iloop] = 0;
          tacped[iloop] = 160.;
          sigtacped[iloop] = 1.7;
        }
      return_state += 2;
    }

  return return_state;
}
