
//
// EMC basic clustering class (for PHOOL).
//
// Alexander Bazilevsky Sep-00
//

#include <cmath>
#include <iostream>
#include "phool.h"
#include "EmcClusterReco.h"
#include "mEmcGeometryModule.h"
#include "EmcCluster.h"


#define TABSURD 999.
#define XABSURD -999999.

using namespace std;

EmcClusterReco::EmcClusterReco(mEmcGeometryModule* geom)
{
  int is;
  int arm, sector;
  int nx, ny;
  float txsz, tysz;
  PHMatrix emcrm;
  PHVector emctr;
  float TowerThresh = 0.003;
  float PeakThresh = 0.08;
  float eClMin = 0.02;
  EmcSectorRec *SectorRec;
  SecGeom *SecGeometry;

  for (is = 0; is < MAX_SECTORS_PROCESS; is++)
    {

      if (is < 6)
        {
          SectorRec = new EmcScSectorRec;
          fScSector.push_back(SectorRec);
        }
      else
        {
          SectorRec = new EmcGlSectorRec;
          fGlSector.push_back(SectorRec);
        }



      // Parmeters for Shower reconstruction
      SectorRec->SetTowerThreshold(TowerThresh);
      SectorRec->SetPeakThreshold(PeakThresh);
      SectorRec->SetChi2Limit(2);

      if (is <= 3)
        {
          arm = 0;
          sector = is;
        }
      else
        {
          arm = 1;
          sector = 7 - is;
        }

      geom->GetSectorDim(is, nx, ny);
      geom->GetTowerSize(is, txsz, tysz);
      geom->GetMatrixVector(is, emcrm, emctr);
      //    printf(" Sec %d dim = (%d,%d)  size = (%f,%f)\n", is, nx, ny, txsz, tysz );

      SecGeometry = new SecGeom;
      fSectorGeometries.push_back(SecGeometry);
      SecGeometry->nx = nx;
      SecGeometry->ny = ny;
      SecGeometry->Tower_xSize = txsz;
      SecGeometry->Tower_ySize = tysz;

      SectorRec->SetGeometry(*SecGeometry, &emcrm, &emctr );

    }

  SetMinClusterEnergy(eClMin);

}

EmcClusterReco::~EmcClusterReco()
{

  for ( size_t i = 0; i < fScSector.size(); ++i )
    {
      delete fScSector[i];
    }
  for ( size_t i = 0; i < fGlSector.size(); ++i )
    {
      delete fGlSector[i];
    }
  for ( size_t i = 0; i < fSectorGeometries.size(); ++i )
    {
      delete fSectorGeometries[i];
    }


}

void EmcClusterReco::SetTowerThreshold(float Thresh)
{
  for (int is = 0; is < MAX_SECTORS_PROCESS; is++)
    {
      if (is < 6)
	{
        fScSector[is]->SetTowerThreshold( Thresh );
	}
      else
	{
        fGlSector[is - 6]->SetTowerThreshold( Thresh );
	}
    }
}

void EmcClusterReco::SetTowerThreshold(int is, float Thresh)
{
  if ( is < 0 || is >= MAX_SECTORS_PROCESS )
    {
      printf("!!! mEmcClusterModule::SetTowerThreshold: Wrong sector index: %d\n", is);
      return ;
    }
  if (is < 6)
    {
    fScSector[is]->SetTowerThreshold( Thresh );
    }
  else
    {
    fGlSector[is - 6]->SetTowerThreshold( Thresh );
    }
}

void EmcClusterReco::SetTowerThresholdPbSc(float Thresh)
{
  for (int is = 0; is < 6; is++)
    {
      fScSector[is]->SetTowerThreshold( Thresh );
    }
}

void EmcClusterReco::SetTowerThresholdPbGl(float Thresh)
{
  for (int is = 0; is < 2; is++)
    {
      fGlSector[is]->SetTowerThreshold( Thresh );
    }
}

void EmcClusterReco::SetPeakThreshold(float Thresh)
{
  for (int is = 0; is < MAX_SECTORS_PROCESS; is++)
    {
      if (is < 6)
        {
          fScSector[is]->SetPeakThreshold( Thresh );
        }
      else
        {
          fGlSector[is - 6]->SetPeakThreshold( Thresh );
        }
    }
}

void EmcClusterReco::SetPeakThreshold(int is, float Thresh)
{
  if ( is < 0 || is >= MAX_SECTORS_PROCESS )
    {
      printf("!!! mEmcClusterModule::SetPeakThreshold: Wrong sector index: %d\n", is);
      return ;
    }
  if (is < 6)
    {
    fScSector[is]->SetPeakThreshold( Thresh );
    }
  else
    {
    fGlSector[is - 6]->SetPeakThreshold( Thresh );
    }
}

//////////////////////////////////////////////////////////////////////

int EmcClusterReco::ClusterReco(vector<EmcModule> *HitList, EMCCluster_DataBlock* pClBlock, int nClMax)
{
  int sector, arm;
  int nCl, nPk = 0, nh;
  int nCluster, nPeakArea;
  float chi2, prob;
  float e, ecore, ecorr, ecorecorr;
  float xcg, ycg, xcgm, ycgm, xx, xy, yy, xc, yc, xgl, ygl, zgl;
  float de, dx, dy, dz, tof;
  // float dd, tflash;

  int iy, iz, Nx;
  float Vertex[3] = {0, 0, 0};
  float MinClusterEnergy;
  EmcModule hmax;

  EmcPeakarea pPList[MaxNofPeaks];
  EmcModule peaks[MaxNofPeaks];
  EMCCluster_DataBlock* pclbl;


  nCluster = 0;
  nPeakArea = 0;
  pclbl = pClBlock;

  for (int is = 0; is < MAX_SECTORS_PROCESS; is++)
    {

      EmcSectorRec* SectorRec = (is < 6) ? fScSector[is] : fGlSector[is - 6];
      if (is <= 3)
        {
          arm = 0;
          sector = is;
        }
      else
        {
          arm = 1;
          sector = 7 - is;
        }

      MinClusterEnergy = (is < 6) ? fMinClusterEnergySc : fMinClusterEnergyGl;

      Nx = SectorRec->GetNx();
      SectorRec->SetVertex(&Vertex[0]);
      SectorRec->SetModules(&HitList[is]);
      nCl = SectorRec->FindClusters();
      vector<EmcCluster>* ClusterList = SectorRec->GetClusters();

      if ( nCl < 0 )
        {
          printf(" ?????? EmcClusterReco: Increase parameter MaxLen !!!\n");
          return -1;
        }


      // Start looping on clusters
      // cout <<"Sec" << is << "NClas:" << nCl << endl;

      vector<EmcCluster>::iterator pc;

      for (pc = ClusterList->begin(); pc != ClusterList->end(); pc++)
        {

          nPk = (*pc).GetPeaks(pPList, peaks);
          EmcPeakarea* pp = pPList;
          assert(pp != 0);

          if ( nPk < 0 )
            {

              cout << "Error in EmcClusterReco::ClusterReco: Number of peaks in cluster is more than fgMaxNofPeaks" << endl;
              return -1;
            }


          // Start looping on peakareas

          for (int ip = 0; ip < nPk; ip++)
            {

              if ( pp->GetTotalEnergy() > MinClusterEnergy )
                {

                  if ( nPeakArea > nClMax )
                    {
                      printf("Error in EmcClusterReco::ClusterReco: EMCCluster_DataBlock is too small: %d\n", nClMax);
                      return -1;
                    }

                  nh = pp->GetNofHits();

                  pp->GetChar(&e, &ecorr, &ecore, &ecorecorr,
                              &xcg, &ycg, &xcgm, &ycgm,
                              &xc, &yc, &xgl, &ygl, &zgl,
                              &xx, &xy, &yy, &chi2, &de, &dx, &dy, &dz);
                  prob = pp->GetCL();


                  // ToF calculation in max tower

                  /*
		    dd = sqrt( (Vertex[0]-xgl)*(Vertex[0]-xgl) +
		    (Vertex[1]-ygl)*(Vertex[1]-ygl) +
		    (Vertex[2]-zgl)*(Vertex[2]-zgl) );
		    hmax=pp->GetMaxTower();
		    tof = hmax.tof;
		    tflash = dd/30.;
		    tof -= tflash;
                  */

                  hmax = pp->GetMaxTower();
                  tof = hmax.tof;
                  iy = hmax.ich / Nx;
                  iz = hmax.ich % Nx;


                  // Filling output cluster data block
                  (*pclbl).sec = is;
                  (*pclbl).iz = iz;
                  (*pclbl).iy = iy;
                  (*pclbl).nhit = nh;
                  (*pclbl).e = e;
                  (*pclbl).ecore = ecore;
                  (*pclbl).ecorr = ecorr;
                  (*pclbl).ecorecorr = ecorecorr;
                  (*pclbl).xg = xgl;
                  (*pclbl).yg = ygl;
                  (*pclbl).zg = zgl;
                  (*pclbl).prob = prob;
                  (*pclbl).tof = tof;

                  pclbl++;
                  nPeakArea++;


                } // if e>eClustMin

              pp++;

            } // for(ip) - PeakArea loop

          nCluster++;

        } // for(pc) - Cluster loop

    } // for(is) - Sector loop

  return nPeakArea;
}

// ///////////////////////////////////////////////////////////////////////////

void EmcClusterReco::ToF_Process(
				 EmcModule* phit, float dist, EmcModule hmax,
				 float* ptof, float* petof, float* ptofcorr,
				 float* pdtof,
				 float* ptofmin, float* petofmin, float* ptofmincorr,
				 float* ptofmax, float* petofmax, float* ptofmaxcorr )
{

  int i;
  float tof, tofcorr, etof, tflash;

  //  float dtof;
  float tofmin, tofmincorr, etofmin, tofmax, tofmaxcorr, etofmax;
  EmcModule* ph;

  tof = hmax.tof;
  etof = hmax.amp;

  tofmax = 0;
  tofmin = 999;
  etofmax = 0;
  etofmin = 0;
  ph = phit;

  for (i = 0; i < HITS_TO_TABLE; i++)
    {

      if (ph->amp > 0)
        {

          if (ph->tof > tofmax)
            {
              tofmax = ph->tof;
              etofmax = ph->amp;
            }
          if (ph->tof < tofmin)
            {
              tofmin = ph->tof;
              etofmin = ph->amp;
            }

        }

      ph++;

    }

  tflash = dist / 30.;

  /*
  tof =    max( 0., tof    - tflash );
  tofmin = max( 0., tofmin - tflash );
  tofmax = max( 0., tofmax - tflash );

  // Slewing corrections
  if(etof>0.051) tofcorr=tof-2.08-0.38/pow(etof-0.05,0.70);
  else tofcorr=TABSURD;

  if(etofmin>0.051) tofmincorr=tofmin-2.08-0.38/pow(etofmin-0.05,0.70);
  else tofmincorr = tofcorr;

  if(etofmax>0.051) tofmaxcorr=tofmax-2.08-0.38/pow(etofmax-0.05,0.70);
  else tofmaxcorr = tofcorr;
  */

  tof = tof - tflash;
  tofmin = tofmin - tflash;
  tofmax = tofmax - tflash;

  tofcorr = tof;
  tofmincorr = tofmin;
  tofmaxcorr = tofmax;

  *ptof = tof;
  *petof = etof;
  *ptofcorr = tofcorr;
  *pdtof = 0;
  *ptofmax = tofmax;
  *petofmax = etofmax;
  *ptofmaxcorr = tofmaxcorr;
  *ptofmin = tofmin;
  *petofmin = etofmin;
  *ptofmincorr = tofmincorr;

}


