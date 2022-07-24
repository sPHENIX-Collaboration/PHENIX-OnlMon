// Online Monitoring includes
#include <EmcAnalys.h>

#include <OnlMonServer.h>
#include <OnCalDBodbc.h>

#include <Event.h>
#include <msg_profile.h>
#include <EmcClusterReco.h>

// Online/Offline includes
#include <mEmcGeometryModule.h>
#include <EmcCluster.h>
#include <emcCalFEM.h>
#include <emcGainFEM.h>
#include <emcCalibrationDataHelper.h>
#include <EmcIndexer.h>
#include <BbcEvent.hh>
#include <BbcCalib.hh>
#include <ZdcEvent.hh>
#include <ZdcCalib.hh>

// ROOT includes
#include <TH1.h>

// system includes
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

TSpectr::TSpectr()
{
  int n = kMaxNumberPeaks;
  fMaxPeaks = n;
  fPositionX = new Float_t[n];
  fFWHM = new Float_t[n];
  memset(fArea, 0, sizeof(fArea));
  memset(fAreaErr, 0, sizeof(fAreaErr));
}

TSpectr::TSpectr(int maxpositions)
{
  int n = maxpositions;
  if (n <= 0)
    {
      n = 1;
    }
  else if (n > kMaxNumberPeaks)
    {
      n = kMaxNumberPeaks;
    }
  fMaxPeaks = n;
  fPositionX = new Float_t[n];
  fFWHM = new Float_t[n];
  memset(fArea, 0, sizeof(fArea));
  memset(fAreaErr, 0, sizeof(fAreaErr));
}

TSpectr::~TSpectr()
{
  delete[]fPositionX;
  delete[]fFWHM;
}

//****************************************************************************************
// PEAK SEARCH FUNCTION
// Used  Yn,m = (1/Nn,m)* sum(Ck,n,m * y(j+k)), where: -m <= k <= m, m = 2 or 3 or 4 or 5
// n = 0-smooth, 1- smooth 1-st derivation , 2- smooth 2-nd derivation ...
//****************************************************************************************
int TSpectr::Search(TH1 *hist, int nsmoothCoeff)
{
  fNPeaks = 0;
  memset(fPositionX, 0, sizeof(fPositionX));
  memset(fFWHM, 0, sizeof(fFWHM));

  if (!hist)
    return 0;
  if (hist->GetDimension() > 1)
    {
      return 0;
    }
  int first = hist->GetXaxis()->GetFirst();
  int last = hist->GetXaxis()->GetLast();
  int size = last - first + 1;
  float *source = new float[size];
  for (int i = 0; i < size; i++)
    {
      source[i] = (float) hist->GetBinContent(i + first);
    }
  int cdv1[11], cdv2[11], cdv4[11];
  for (int i = 0; i < 11; i++)
    {
      cdv1[i] = 0;
      cdv2[i] = 0;
      cdv4[i] = 0;
    }
  float cN1, cN2, cN4;
  float dwidth;
  switch (nsmoothCoeff)
    {
    case 5:              //cN4=cN2*cN2 cdv4=cdv2*cdv2...
      cN1 = 12.0;
      cN2 = 7.0;
      cN4 = 49;
      cdv1[2] = 0;
      cdv2[2] = -2;
      cdv4[2] = 4;
      cdv1[0] = 1;
      cdv1[1] = -8;
      cdv1[3] = 8;
      cdv1[4] = -1;
      cdv2[0] = 2;
      cdv2[1] = -1;
      cdv2[3] = -1;
      cdv2[4] = 2;
      cdv4[0] = 4;
      cdv4[1] = 1;
      cdv4[3] = 1;
      cdv4[4] = 4;
      dwidth = 1.0 + ((float) cdv2[3]) / ((float) (cdv2[3] - cdv2[4]));
      break;
    case 7:
      cN1 = 252.0;
      cN2 = 42.0;
      cN4 = 1764.0;
      cdv1[3] = 0;
      cdv2[3] = -4;
      cdv4[3] = 16;
      cdv1[0] = 22;
      cdv1[1] = -67;
      cdv1[2] = -58;
      cdv1[4] = 58;
      cdv1[5] = 67;
      cdv1[6] = -22;
      cdv2[0] = 5;
      cdv2[1] = 0;
      cdv2[2] = -3;
      cdv2[4] = -3;
      cdv2[5] = 0;
      cdv2[6] = 5;
      cdv4[0] = 25;
      cdv4[1] = 0;
      cdv4[2] = 9;
      cdv4[4] = 9;
      cdv4[5] = 0;
      cdv4[6] = 25;
      dwidth = 1.0 + ((float) cdv2[4]) / ((float) (cdv2[4] - cdv2[5]));
      break;
    case 9:
      cN1 = 1188.0;
      cN2 = 462.0;
      cN4 = 213444.0;
      cdv1[4] = 0;
      cdv2[4] = -20;
      cdv4[4] = 400;
      cdv1[0] = 86;
      cdv1[1] = -142;
      cdv1[2] = -193;
      cdv1[3] = -126;
      cdv1[5] = 126;
      cdv1[6] = 193;
      cdv1[7] = 142;
      cdv1[8] = -86;
      cdv2[0] = 28;
      cdv2[1] = 7;
      cdv2[2] = -8;
      cdv2[3] = -17;
      cdv2[5] = -17;
      cdv2[6] = -8;
      cdv2[7] = 7;
      cdv2[8] = 28;
      cdv4[0] = 784;
      cdv4[1] = 49;
      cdv4[2] = 64;
      cdv4[3] = 289;
      cdv4[5] = 289;
      cdv4[6] = 64;
      cdv4[7] = 49;
      cdv4[8] = 784;
      dwidth = 2.0 + ((float) cdv2[6]) / ((float) (cdv2[6] - cdv2[7]));
      break;
    case 11:
      cN1 = 5148.0;
      cN2 = 429.0;
      cN4 = 184041.0;
      cdv1[5] = 0;
      cdv2[5] = -10;
      cdv4[5] = 100;
      cdv1[0] = 300;
      cdv1[1] = -294;
      cdv1[2] = -532;
      cdv1[3] = -503;
      cdv1[4] = -296;
      cdv1[6] = 296;
      cdv1[7] = 503;
      cdv1[8] = 532;
      cdv1[9] = 294;
      cdv1[10] = -300;
      cdv2[0] = 15;
      cdv2[1] = 6;
      cdv2[2] = -1;
      cdv2[3] = -6;
      cdv2[4] = -9;
      cdv2[6] = -9;
      cdv2[7] = -6;
      cdv2[8] = -1;
      cdv2[9] = 6;
      cdv2[10] = 15;
      cdv4[0] = 225;
      cdv4[1] = 36;
      cdv4[2] = 1;
      cdv4[3] = 36;
      cdv4[4] = 49;
      cdv4[6] = 81;
      cdv4[7] = 36;
      cdv4[8] = 1;
      cdv4[9] = 36;
      cdv4[10] = 225;
      dwidth = 3.0 + ((float) cdv2[8]) / ((float) (cdv2[8] - cdv2[9]));
      break;
    default:
      nsmoothCoeff = 9;
      cN1 = 1188.0;
      cN2 = 462.0;
      cN4 = 213444.0;
      cdv1[4] = 0;
      cdv2[4] = -20;
      cdv4[4] = 400;
      cdv1[0] = 86;
      cdv1[1] = -142;
      cdv1[2] = -193;
      cdv1[3] = -126;
      cdv1[5] = 126;
      cdv1[6] = 193;
      cdv1[7] = 142;
      cdv1[8] = -86;
      cdv2[0] = 28;
      cdv2[1] = 7;
      cdv2[2] = -8;
      cdv2[3] = -17;
      cdv2[5] = -17;
      cdv2[6] = -8;
      cdv2[7] = 7;
      cdv2[8] = 28;
      cdv4[0] = 784;
      cdv4[1] = 49;
      cdv4[2] = 64;
      cdv4[3] = 289;
      cdv4[5] = 289;
      cdv4[6] = 64;
      cdv4[7] = 49;
      cdv4[8] = 784;
      dwidth = 2.0 + ((float) cdv2[6]) / ((float) (cdv2[6] - cdv2[7]));
    }
  int m2 = (nsmoothCoeff - 1) / 2;
  int m1 = m2 + 1;
  int m3 = m2;
  if (m3 < 4)
    m3 = 4;
  float rrr = -3;  // exceeding  the second derivative of its error
  float dt = 3;   // exceeding a netarea pick over his error
  float drv2[11], r1, dr1 = 0; // 2 derivative & her error...
  float r, rr, rz = 0;
  float sigm[2];
  for (int i = 0; i < 11; i++)
    drv2[i] = 0.0;

  int nright = size - m1 - 3;
  float s1 = 1.0, s2 = 0.0;
  int ipeak = 0;
  for (int j = m1 + 1; j < nright; j++)
    {
      if (ipeak >= fMaxPeaks)
        break;
      s2 = 0.0;
      for (int i = 0, ii = -m2; i < nsmoothCoeff; i++, ii++)
        s2 = s2 + cdv1[i] * source[j + ii];
      s2 /= cN1;
      if (s1 < 0.0 || s2 >= 0.0)
        {
          s1 = s2;
          continue;
        }
      r = 0.0;
      for (int jj = j - 1; jj <= j; jj++)
        {
          r1 = 0.0;
          dr1 = 0;
          for (int i = 0, ii = -m2; i < nsmoothCoeff; i++, ii++)
            {
              r1 = r1 + cdv2[i] * source[j + ii];
              dr1 = dr1 + cdv4[i] * source[j + ii];
            }
          r1 /= cN2;
          dr1 /= cN4;
          if (dr1 == 0.0)
            break;
          if (r1 < 0)
            {
              rr = r1 / sqrt(dr1);
              if (rr < r)
                r = rr;
            }
        }
      if (dr1 == 0 || r > rrr)
        continue;
      int m = 0;
      for (int jj = j - m3, k = 0; jj <= j + m3; jj++)
        {
          r1 = 0.0;
          for (int i = 0, ii = -m2; i < nsmoothCoeff; i++, ii++)
            {
              r1 = r1 + cdv2[i] * source[jj + ii];
            }
          r1 /= cN2;
          if (r1 <= 0.0)
            m++;
          if (jj >= j - 4 || jj <= j + 4)
            {
              drv2[k] = r1;
              k++;
            }
        }
      //******************************************************************************
      //To be in routine "m < m1", but if you do increase of sensitivity then "m < m2"
      //******************************************************************************
      if (m < m2)
        continue;
      int k = j - 1, l = j;
      float spik = 0.0;
      float stot = source[k] + source[l];
      for (m = 1; m <= 30; m++)
        {
          float rfon = (source[k - 1] + source[k - 2] + source[k - 3] + source[l + 1] + source[l + 2] + source[l + 3]) / 3.0 * (float) m;
          if ((stot - rfon) <= spik)
            break;
          spik = stot - rfon;
          rz = rfon;
          k -= 1;
          l += 1;
          stot = stot + (source[k] + source[l]);
          if (k < 3)
            break;
        }
      // if(m < 4)  continue;  //may be remove ....
      stot = rz + spik;
      if (rz == 0.0)
        rz = 1.0;
      float dd = spik / sqrt(rz);   //10.0
      if (dd < dt || spik <= 15.0)
        continue;

      fArea[ipeak] = spik;
      if (spik != 0.0)
        {
          fAreaErr[ipeak] = sqrt(rz + stot);
        }
      else
        {
          fAreaErr[ipeak] = 0.0;
        }

      fPositionX[ipeak] = (float) j - 1 + s1 / (s1 - s2);
      // Attention !!! Channel with 0
      //*************************************************************************
      // Use for calculation the boundary peak
      // Calcilate FWHM in assumption the gauss peak [fwhm= 2*sqrt(2*M_LN2)*sigma]
      // Sigma defined from condition equality 2 derivative null (with left & right)
      // Calculation straight line coeffcients  (least square method [LSM])
      //*************************************************************************
      if (m1 == 5)
        {
          m = 1;
        }
      else
        {
          m = 0;
        }
      k = -3;
      if ((fPositionX[ipeak] - (int) fPositionX[ipeak]) >= 0.5)
        m += 1;
      for (int l = 0; l < 2; l++)
        {
          float cx = 0.0, cy = 0.0;
          float cxy = 0.0, cxq = 0.0;
          if (m1 != 5)
            {
              m += l;
              k += l;
            }
          for (int numb = 0; numb < 3; numb++)
            {
              cx = cx + (float) k ;
              cy = cy + drv2[m];
              cxy = cxy + ((float) k) * drv2[m];
              cxq = cxq + (float) (k * k);
              m += 1;
              k += 1;
            }
          float ac = (cx * cy - 3.0 * cxy) / (cx * cx - 3.0 * cxq) ;
          float bc = (cy - ac * cx) / 3.0;
          if (ac == 0.0)
            {
              sigm[0] = 1.0;
              break;
            }
          sigm[l] = ( -bc / ac);
        }
      if (sigm[0] > 0.0 || sigm[1] < 0.0)
        continue;

      fFWHM[ipeak] = 1.1774 * (sigm[1] - sigm[0]) / dwidth; //Fwhm= 2.35482*sigma, 2.35482/2 == 1.7741
      if (fFWHM[ipeak] > 10.0)
        fFWHM[ipeak] = 10.0;     //the size of a FWHM in bins !!!

      ipeak++;
      s1 = s2;
    }
  for (int i = 0; i < ipeak; i++)
    {
      int bin = first + int(fPositionX[i] + 0.5);
      fPositionX[i] = hist->GetBinCenter(bin);
      // cout <<" Peak=" << fPositionX[i] << " FWHM=" << fFWHM[i] << "[bin]" << " BinWidth=" <<  hist->GetBinWidth(size) << endl;
    }
  delete [] source;

  fNPeaks = ipeak;
  return fNPeaks;
}

Analys::Analys(const OnlMon *parent)
{
  mymon = parent;
  fCalib = false;
  fUseOnlyLowGain = false;

  ThresholdADC = 10;
  TowerThresholdPbSc = 0.010;
  TowerThresholdPbGl = 0.015;

  mEmcGeometry = new mEmcGeometryModule();
  mEmcCluster = new EmcClusterReco(mEmcGeometry);

  mEmcCluster->SetTowerThresholdPbSc(TowerThresholdPbSc);
  mEmcCluster->SetTowerThresholdPbGl(TowerThresholdPbGl);

  mEmcCluster->SetMinClusterEnergyPbSc(0.200);
  mEmcCluster->SetMinClusterEnergyPbGl(0.200);

  mEMCClusterData = new EMCCluster_DataBlock [NMAXCL];
  EmcCl = new EMCClusters [NMAXCL];
  EmcClPair = new EMCClusterPair [NMAXPAIR];

  //BBC 
  mBbcEvent = new BbcEvent();
  mBbcCalib = new BbcCalib();
  //ZDC
  mZdcEvent = new ZdcEvent();
  mZdcCalib = new ZdcCalib();

  memset(emc_twrHit, 0, sizeof(emc_twrHit));
  memset(emc_twrEntry, 0, sizeof(emc_twrEntry));
  oncalDB = new OnCalDBodbc();
}


Analys::~Analys()
{

  delete mBbcEvent;
  delete mBbcCalib;
  delete mZdcEvent;
  delete mZdcCalib;
  delete mEmcCluster;
  delete mEmcGeometry;

  delete [] mEMCClusterData;
  delete [] EmcCl;
  delete [] EmcClPair;
  if (oncalDB)
    {
      delete oncalDB;
    }
}


void Analys::Setup(const int runno)
{

  memset(fDataError, 0, sizeof(fDataError));
  memset(fEmcWarn, 0, sizeof(fEmcWarn));
  memset(fEmcCal, 0, sizeof(fEmcCal));

  const char *warnFile = getenv("EMCWARNDATA");
  int towerId, emc_warn;

  if (warnFile)
    {
      ifstream fin (warnFile);
      if (fin)
        {
          while (fin >> towerId >> emc_warn)
            {
              if (emc_warn && towerId >= 0 && towerId < NCHANNEL)
                {
                  SetDataWarn(towerId, CHANNEL_DISABLED);
                }
            }
          fin.close();
        }
      else
        {
          ostringstream errmsg;
          errmsg << "Can't open the warning file \"" << warnFile << "\"" << endl;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_ERROR, errmsg.str(), 6);
        }

    }
  else
    {
      ostringstream errmsg;
      errmsg << "You need to set \"EMCWARNDATA\" variable ";
      errmsg << "to specify the source of the warning map" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_PBSC, MSG_SEV_ERROR, errmsg.str(), 6);
    }

  const char *strBBCCalib = getenv("BBCCALIBRATION");
  const char *strEMCCalib = getenv("EMCCALIBRATION");

  fCalib = false;

  if (!strBBCCalib)
    {
      ostringstream errmsg;
      errmsg << "You need to set \"BBCCALIBRATION\" variable ";
      errmsg << "to specify the source of the constants" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
      return ;
    }
  if (!strEMCCalib)
    {
      ostringstream errmsg;
      errmsg << "You need to set \"EMCCCALIBRATION\" variable ";
      errmsg << "to specify the source of the calibration coefficients" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
      return ;
    }

  //BBC calibration
  if (strcmp(strBBCCalib, "PDBCAL") == 0)
    {
      PHTimeStamp time_bbc;
      time_bbc.setToSystemTime();
      // store from PdbCal
      int calib_version = 1004;

      mBbcCalib->restore(time_bbc, calib_version);

    }
  else if (strcmp(strBBCCalib, "FILE") == 0)
    {
      // store from File
      if (!getenv("BBCCALIBDIR"))
        {
          ostringstream errmsg;
          errmsg << "Environment variable BBCCALIBDIR not set";
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
          mBbcCalib->restore("./BbcCalib");
          return ;
        }
      else
        {
          ostringstream otmp;
          otmp << getenv("BBCCALIBDIR") << "/BbcCalib";
          // need a cast from (const char *) to (char *)
          mBbcCalib->restore(const_cast <char *> (otmp.str().c_str()) );
        }
    }
  else
    {
      ostringstream errmsg;
      errmsg << "BBC calibration constants didn`t load" << endl;
      errmsg << "You need to set \"BBCCALIBRATION\" variable ";
      errmsg << "in \"Objy\" or \"FILE\"" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
      return ;
    }
  mBbcEvent->setCalibDataAll(mBbcCalib);
  
 //ZDC calibration
  ostringstream  otmp;
  if (getenv("ZDCCALIBDIR"))
    {
      otmp << getenv("ZDCCALIBDIR")<< "/ZdcCalib";
      mZdcCalib->restore(const_cast <char *> (otmp.str().c_str()) );
      PHTimeStamp now;
      now.setToSystemTime();
      mZdcEvent->setCalibDataAll(mZdcCalib, now);   
    }
  else
    {
      ostringstream errmsg;
      errmsg << "You need to set \"ZDCCALIBDIR\" variable ";
      errmsg << "to specify the source of the constants" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
      return;     
    }        
       

  //EMC calibration

  if (strcmp(strEMCCalib, "DB") == 0)
    {
      int ntowers = 144 * 172;

      int runnumber = oncalDB->GetLastCalibratedRun(runno);
      printf("GetLastCalibratedRun(%i): %i\n",runno,runnumber);
      emcCalibrationDataHelper cdh(runnumber, false);
      int coeffcount = 0;
      for (int towerId = 0; towerId < ntowers; towerId++)
        {
          int absFEM, FEMchannel;
          EmcIndexer::PXPXSM144CH(towerId, absFEM, FEMchannel);

          const emcGainFEM* gains = dynamic_cast <const emcGainFEM *> (cdh.getCalibration(absFEM, "Gains"));
          double normt = gains->getValue(FEMchannel, 0);

          fEmcCal[towerId] = 0.0;
          if (normt > 0.0)
            {
              fEmcCal[towerId] = cdh.getEnergyCalibration(towerId) / normt;
              if (fEmcCal[towerId] > 0.00009 && fEmcCal[towerId] < 0.005)
                {
                  coeffcount++;
                }
            }
	  //	   printf("%i %f\n",towerId,fEmcCal[towerId]);
        }
      float defectcoeff = (1.0 - coeffcount / (float)ntowers) * 100.0;
      if (defectcoeff > 30.0)
        {
          ostringstream errmsg;
          errmsg << "" << defectcoeff << "% the EMC calibration coefficients in the run #" << runnumber << "have not a valid state" << endl;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_WARNING, errmsg.str(), 7);
        }
    }
  else if (strcmp(strEMCCalib, "FILE") == 0)
    {

      const char *calibFile = getenv("EMCCALIBDATA");
      float calibCoeff;
      int towerId;
      ostringstream errmsg;

      if (!calibFile)
        {
          errmsg << "EMC calibration coefficients didn`t load" << endl;
          errmsg << "You need to set \"EMCCCALIBDATA\" variable";
        }
      ifstream fin (calibFile);
      if (fin)
        {
          while (fin >> towerId >> calibCoeff)
            {
              if (towerId >= 0 && towerId < NCHANNEL)
                {
                  fEmcCal[towerId] = calibCoeff;
                  if (fEmcCal[towerId] == 0.)
                    SetDataWarn(towerId, ECALIB_DISABLED);
                }
            }
          fin.close();
        }
      else
        {
          errmsg << "Can't open the calibration file " << calibFile << endl;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
          return ;
        }

    }
  else
    {
      ostringstream errmsg;
      errmsg << "EMC calibration coefficients didn`t load" << endl;
      errmsg << "You need to set \"EMCCCALIBRATION\" variable ";
      errmsg << "in \"DB\" or \"FILE\"" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(), 6);
      return ;
    }

  fCalib = true;

  //**** warn test beg
  //  ofstream fout("calibtest.dat", ios_base::out);
  //  if (!fout)
  //    {
  //      ostringstream errmsg;
  //      errmsg << "Error create file: calibtest.dat" << endl;
  //          OnlMonServer  *se = OnlMonServer::instance();
  //      se->send_message(mymon,MSG_SOURCE_PBGL, MSG_SEV_SEVEREERROR, errmsg.str(),6);
  //      return ;
  //    }
  //  for (int i = 0; i < NCHANNEL; i++)
  //    {
  //      fout << i << " " << fEmcCal[i] << endl;
  //    }
  //  fout.close();

  //**** warn test end

}


void Analys::SetDataWarn(int towerId, int errCode)
{
  int isec, itwsec, isch;
  int ix, iy, ix0, iy0, xmax, ymax, ymax_2;

  if (towerId < 15552)
    {
      xmax = 72;
      ymax = 72;
      isec = towerId / 2592;
      itwsec = towerId % 2592;

      isch = isec * 2592;
    }
  else
    {
      xmax = 96;
      ymax = 96;
      isec = (towerId - 15552) / 4608;
      itwsec = (towerId - 15552) % 4608;
      isch = 15552 + isec * 4608;
    }
  iy0 = itwsec / ymax;
  ix0 = itwsec % xmax;
  ymax_2 = ymax / 2;

  for (int dx = -1; dx <= 1; dx++ )
    {
      for (int dy = -1; dy <= 1; dy++ )
        {
          ix = ix0 + dx;
          iy = iy0 + dy;
          if (ix >= 0 && ix < xmax && iy >= 0 && iy < ymax_2)
            {
              int towerid = isch + iy * ymax + ix;
              if (towerid < NCHANNEL)
                {
                  fEmcWarn[towerid] |= errCode;
                }
              else
                {
                  ostringstream warnmsg;
                  warnmsg << "SetDataWarn() is towerid: " << towerid << endl;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(mymon, MSG_SOURCE_PBGL, MSG_SEV_WARNING, warnmsg.str(), 8);
                }
            }
        }
    }
}


void Analys::SaveTowerHits(const char *fileName)
{
  ofstream fout(fileName, ios_base::out);
  if (!fout)
    {
      return ;
    }

  fout << emc_twrEntry[0] << " " << emc_twrEntry[1] << " " << emc_twrEntry[2] << endl;
  for (int towerId = 0; towerId < NCHANNEL; towerId++)
    {
      fout << towerId << " " << emc_twrHit[0][towerId] << " " << emc_twrHit[1][towerId]
	   << " " << emc_twrHit[2][towerId] << endl;
    }

  fout.close();
}


int Analys::ProcessBBC_ZDC(Event *ev)
{

  bbc_z = -9999;
  bbc_t0 = -9999;
  zdc_z = -9999;
  int ret = 0;
  Packet *pBBC = ev->getPacket(1001);
  Packet *npBBC = ev->getPacket(1002);
  Packet *spBBC = ev->getPacket(1003);
  Packet *pZDC = ev->getPacket(13001);

  if (!pBBC && (!npBBC || !spBBC))
    {
      cout << "Error in ProcessBBC: No BBC packet" << endl;
    }
  else 
     {
       // calculate BBC
       mBbcEvent->Clear();
       mBbcEvent->setRawData(ev);
       mBbcEvent->calculate();

       bbc_z = mBbcEvent->getZVertex();
       bbc_t0 = mBbcEvent->getTimeZero();
       ret |= 1;
       // cout << "BBC_Z:" << bbc_z << "BBC_T0:" << bbc_t0 << endl;
    }
  if (pZDC)
    {
      // calculate ZDC
      mZdcEvent->Clear();
      mZdcEvent->setRawData(ev);
      mZdcEvent->calculate();
  
      zdc_z = mZdcEvent->getZvertex();
      ret |= 2;
     // cout << "ZDC_Z:" << zdc_z << endl;
    }

  if (pBBC)
    { 
      delete pBBC;
    }
  if (npBBC)
    {
      delete npBBC;
    }
  if (spBBC)
    {
      delete spBBC;
    }
  if (pZDC)
    {
      delete pZDC;
    }

  return ret;
}


void Analys::ProcessPhysEMC(Event *ev)
{

  int first_packet = 8001;

  int towerId, arm, isec, itwsec;

  float adc, energy, etot = 0;

  EmcModule vhit;

  // emc_ncl =   0;
  // emc_etot =  0.;
  emc_npair = 0;

  for (int isec = 0; isec < NSEC; isec++)
    {
      HitList[isec].erase(HitList[isec].begin(), HitList[isec].end());
    }

  etot = 0.;

  struct emcChannelLongList ecl[144];
  int nw;
  int NumberOfChannels;

  bool LowGain = true;
  float scale = 1.0;

  for (int iFEM = 0; iFEM < NFEM; iFEM++)
    {
      Packet *p = ev->getPacket(first_packet + iFEM);
      if (p)
        {
          if (iFEM < 108)
            {
              isec = iFEM / 18;
            }
          else
            {
              isec = 6 + (iFEM - 108) / 32;
            }
          arm = ((isec < 4) ? 0 : 1);
          // if(arm ==1 ) isec = ((isec -=6)<0)? isec+4 : isec;

          NumberOfChannels = p->fillIntArray ( (int *) ecl, sizeof(emcChannelLongList)*144, &nw, "SPARSE");
          if (NumberOfChannels > 144)
            {
              ostringstream infomsg;
              printf("FEM: NumberOfChannels: %d %d\n", iFEM, NumberOfChannels);
              NumberOfChannels = -1;
            }
          for (int i = 0; i < NumberOfChannels; i++)
            {
              int errCode = 0;
              // bool  LowGain = true;
              //float scale = 1.0;


              int ich = ecl[i].channel;
              if (ich < 0 || ich >= 144)
                {
                  printf("FEM:  out range off channel: %d %d\n", iFEM, ich);
                  break ;
                }

              int lgpre = 4095 - ecl[i].lowpre;
              int lgpost = 4095 - ecl[i].lowpost;
              int hgpre = 4095 - ecl[i].highpre;
              int hgpost = 4095 - ecl[i].highpost;
              int tac = 4095 - ecl[i].time;
              if (lgpre < LG_MIN || lgpre > LG_MAX)
                {
                  errCode |= LG_PRE_OUT;
                }
              if (lgpost < LG_MIN || lgpost > LG_MAX)
                {
                  errCode |= LG_POST_OUT;
                }
              if (hgpre < HG_MIN || hgpre > HG_MAX)
                {
                  errCode |= HG_PRE_OUT;
                }
              if (hgpost < HG_MIN || hgpost > HG_MAX)
                {
                  errCode |= HG_POST_OUT;
                }
              if (tac < TAC_MIN || tac > TAC_MAX)
                {
                  errCode |= TAC_OUT;
                }

              int lg = lgpre - lgpost;
              int hg = hgpre - hgpost;
              //towerId = iFEM * 144 + ich;

              towerId = EmcIndexer::PXSM144iCH_iPX(iFEM, ich);
              fDataError[towerId] |= errCode;

              if (iFEM < 108)
                {
                  itwsec = towerId % 2592;
                  if (!fUseOnlyLowGain)
                    {
                      LowGain = (lg > 192.) || (errCode & 0xC);
                      scale = 15.4;
                    }
                  if (LowGain)
                    {
                      adc = scale * lg; //15.22
                    }
                  else
                    {
                      adc = hg;
                    }
                }
              else
                {
                  itwsec = (towerId - 15552) % 4608;
                  if (!fUseOnlyLowGain)
                    {
                      LowGain = (lg > 170.) || (errCode & 0x003C) || (hg < 0. && lg > 50.);
                      scale = 15.33;
                    }
                  if (LowGain)
                    {
                      adc = scale * lg;
                    }
                  else
                    {
                      adc = hg;
                    }
                }

              //cout << towerId << " " << adc << " " << tac << endl;
              // if(!(errCode & TAC_OUT) && adc > ThresholdADC ) {

              energy = adc * fEmcCal[towerId];
              vhit.ich = itwsec;
              vhit.adc = adc;
              vhit.tac = tac;
              vhit.warnmap = fEmcWarn[towerId];
              vhit.amp = energy;

              if (errCode & TAC_OUT)
                {
                  vhit.tof = -9999;
                }
              else
                {
                  vhit.tof = tac;   //sectors rotate for arm1 may be ...
                  //   if( arm == 1 ) isec = 7 - isec;
                }

              if ( adc > ThresholdADC )
                {
                  HitList[isec].push_back(vhit);

                  etot += energy;

                  if (energy > 0.2)
                    {
                      emc_twrHit[0][towerId]++;
                      emc_twrEntry[0]++;
                    }
                  if (energy > 1.0)
                    {
                      emc_twrHit[1][towerId]++;
                      emc_twrEntry[1]++;
                    }
                  if (energy > 3.0)
                    {
                      emc_twrHit[2][towerId]++;
                      emc_twrEntry[2]++;
                    }

                }


            } // end for channel
          if (p)
            {
              delete p;
            }
        }

    }

  int nCl = 0;
  int sec = 0, id = 0, iy = 0, iz = 0;
  int ism = 0, id_sec = 0;
  emc_etot = etot;
  nCl = mEmcCluster->ClusterReco(HitList, mEMCClusterData, NMAXCL);

  if (nCl < 0)
    {
      nCl = 0;
    }
  emc_ncl = nCl;

  for (int icl = 0; icl < nCl; icl++)
    {
      sec = mEMCClusterData[icl].sec;
      iy = mEMCClusterData[icl].iy;
      iz = mEMCClusterData[icl].iz;
      if (sec >= 0 && sec <= 5)
        {
          ism = iy / 12 * 6 + iz / 12;
          ism += sec * 18;
          id_sec = iy * 72 + iz;
        }
      else if ( sec >= 6 && sec <= 7 )
        {
          ism = iy / 12 * 8 + iz / 12;
          ism += 108 + (sec - 6) * 32;
          id_sec = iy * 96 + iz;
        }
      else
        {
          printf("Error in ProcessEMC: Wrong sector number: %d\n", sec);
        }

      EmcCl[icl].emc_sec = sec;
      EmcCl[icl].emc_sm = ism;
      EmcCl[icl].emc_id = id_sec;
      if (sec < 6)
        {
          id = id_sec + 2592 * sec;
        }
      else
        {
          id = id_sec + 4608 * (sec - 6) + 15552;
        }

      //added by karatsu (contact: karatsu@scphys.kyoto-u.ac.jp)
      //float tmp_etot = mEMCClusterData[icl].e;
      // int tmp_nhit = mEMCClusterData[icl].nhit;
      // if( tmp_etot>0.2 && tmp_nhit==1 ) { fEmcWarn[id] |= CHANNEL_DISABLED; }
      ////

      EmcCl[icl].emc_warn = fEmcWarn[id];
      EmcCl[icl].emc_nh = mEMCClusterData[icl].nhit;
      EmcCl[icl].emc_e = mEMCClusterData[icl].e;
      EmcCl[icl].emc_ecorr = mEMCClusterData[icl].ecorr;
      EmcCl[icl].emc_ecore = mEMCClusterData[icl].ecore;
      EmcCl[icl].emc_ecorecorr = mEMCClusterData[icl].ecorecorr;
      EmcCl[icl].emc_x = mEMCClusterData[icl].xg;
      EmcCl[icl].emc_y = mEMCClusterData[icl].yg;
      EmcCl[icl].emc_z = mEMCClusterData[icl].zg;
      EmcCl[icl].emc_tof = mEMCClusterData[icl].tof;
      EmcCl[icl].emc_prob = mEMCClusterData[icl].prob;

    }


}

int Analys::ProcessPi0()
  // Should be called after ProcessEMC
{

  static const float eGMin = 0.2;
  static const float ptPi0Min = 2;

  float xyz1[3], xyz2[3], e1, e2;
  int sec1, sec2;   // ipt, itr, is;
  float px1, px2, py1, py2, pt;
  bool good1, good2;


  if (abs(bbc_z) > BBCZ && abs(zdc_z) > ZDCZ)
    {
      // cout << "BBC_Z and ZDC_Z are wrong " << abs(bbc_z) << " " << abs(zdc_z) << endl;
       return 0;
    }

  int npi0 = 0;
  emc_npair = 0;

  if ( emc_ncl <= 1 )
    {
      return 0;
    }

  for (int i1 = 0; i1 < emc_ncl - 1; i1++)
    {

      e1 = EmcCl[i1].emc_ecorecorr;
      sec1 = EmcCl[i1].emc_sec;
      //modfied by karatsu (contact: karatsu@scphys.kyoto-u.ac.jp)
      //good1 = EmcCl[i1].emc_warn == 0; //you need changed...
      good1 = 1;

      if (e1 > eGMin && good1)
        {
          xyz1[0] = EmcCl[i1].emc_x;
          xyz1[1] = EmcCl[i1].emc_y;
          xyz1[2] = EmcCl[i1].emc_z;

          for (int i2 = i1 + 1; i2 < emc_ncl; i2++)
            {
              e2 = EmcCl[i2].emc_ecorecorr;
              sec2 = EmcCl[i2].emc_sec;
              good2 = EmcCl[i2].emc_warn == 0;
              xyz2[0] = EmcCl[i2].emc_x;
              xyz2[1] = EmcCl[i2].emc_y;
              xyz2[2] = EmcCl[i2].emc_z;

              px1 = e1 * xyz1[0] / sqrt(xyz1[0] * xyz1[0] + xyz1[1] * xyz1[1] + xyz1[2] * xyz1[2]);
              px2 = e2 * xyz2[0] / sqrt(xyz2[0] * xyz2[0] + xyz2[1] * xyz2[1] + xyz2[2] * xyz2[2]);
              py1 = e1 * xyz1[1] / sqrt(xyz1[0] * xyz1[0] + xyz1[1] * xyz1[1] + xyz1[2] * xyz1[2]);
              py2 = e2 * xyz2[1] / sqrt(xyz2[0] * xyz2[0] + xyz2[1] * xyz2[1] + xyz2[2] * xyz2[2]);
              pt = sqrt((px1 + px2) * (px1 + px2) + (py1 + py2) * (py1 + py2));

              if (e2 > eGMin && good2 && pt > ptPi0Min && xyz1[0]*xyz2[0] > 0)
                {
                  EmcClPair[npi0].emc_pt = pt;
                  EmcClPair[npi0].emc_eg1 = e1;
                  EmcClPair[npi0].emc_eg2 = e2;
                  EmcClPair[npi0].emc_sec1 = sec1;
                  EmcClPair[npi0].emc_sec2 = sec2;

                  EmcClPair[npi0].emc_pr1 = EmcCl[i1].emc_prob;
                  EmcClPair[npi0].emc_pr2 = EmcCl[i2].emc_prob;
                  EmcClPair[npi0].emc_tof1 = EmcCl[i1].emc_tof;
                  EmcClPair[npi0].emc_tof2 = EmcCl[i2].emc_tof;
                  EmcClPair[npi0].emc_dgg = sqrt( (xyz2[0] - xyz1[0]) * (xyz2[0] - xyz1[0]) +
                                                  (xyz2[1] - xyz1[1]) * (xyz2[1] - xyz1[1]) +
                                                  (xyz2[2] - xyz1[2]) * (xyz2[2] - xyz1[2]) );

                  EmcClPair[npi0].emc_m = InvMass(EmcCl[i1].emc_e, xyz1, EmcCl[i2].emc_e, xyz2 );
                  EmcClPair[npi0].emc_mc = InvMass(e1, xyz1, e2, xyz2 );

                  npi0++;
                }
            }     // for(int i2
        }       // end if (e1 > eGMin)
    }           // for(int i1

  emc_npair = npi0;

  return 1;
}


float Analys::InvMass( float e1, float* xyz1, float e2, float* xyz2 )
{
  float p1[3], p2[3];
  float l1 = sqrt(xyz1[0] * xyz1[0] + xyz1[1] * xyz1[1] + xyz1[2] * xyz1[2]);
  float l2 = sqrt(xyz2[0] * xyz2[0] + xyz2[1] * xyz2[1] + xyz2[2] * xyz2[2]);
  if ( l1 <= 0. || l2 <= 0. )
    {
      return 0.;
    }
  for ( int i = 0; i < 3; i++ )
    {
      p1[i] = e1 * xyz1[i] / l1;
      p2[i] = e2 * xyz2[i] / l2;
    }
  float p1p1 = p1[0] * p1[0] + p1[1] * p1[1] + p1[2] * p1[2];
  float p2p2 = p2[0] * p2[0] + p2[1] * p2[1] + p2[2] * p2[2];
  float p1p2 = p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2];
  float mass = sqrt((sqrt(p1p1 * p2p2) - p1p2) * 2);
  return mass;
}

