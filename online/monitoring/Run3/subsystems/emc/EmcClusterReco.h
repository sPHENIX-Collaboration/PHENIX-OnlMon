#ifndef __EMCCLUSTERRECO_H__
#define __EMCCLUSTERRECO_H__

//
// EMC basic clustering class (for PHOOL).
//
// Alexander Bazilevsky Sep-00
//

#include "EmcScSectorRec.h"
#include "EmcGlSectorRec.h"
#include <vector>


struct EMCCluster_DataBlock
{
  int evno;
  int bcc;
  int sec;
  int iz;
  int iy;
  int nhit;
  float e;
  float ecorr;
  float ecore;
  float ecorecorr;
  float xg;
  float yg;
  float zg;
  float tof;
  float prob;
};

class mEmcGeometryModule;
class EmcModule;


class EmcClusterReco
{

public:

  EmcClusterReco( mEmcGeometryModule* );
  virtual ~EmcClusterReco();
  void SetMinClusterEnergy(float eClMin){ fMinClusterEnergySc=eClMin; fMinClusterEnergyGl=eClMin; }
  void SetMinClusterEnergyPbSc(float eClMin){ fMinClusterEnergySc=eClMin; }
  void SetMinClusterEnergyPbGl(float eClMin){ fMinClusterEnergyGl=eClMin; }
  void SetTowerThreshold(float Thresh);
  void SetTowerThreshold(int is, float Thresh);
  void SetTowerThresholdPbSc(float Thresh);
  void SetTowerThresholdPbGl(float Thresh);
  void SetPeakThreshold(float Thresh);
  void SetPeakThreshold(int is, float Thresh);
  int ClusterReco(std::vector<EmcModule>*, EMCCluster_DataBlock*,  int nClMax);
  //  PHBoolean  event(PHCompositeNode * root);
  void ToF_Process( EmcModule* phit, float dist, EmcModule hmax,
		    float* ptof,    float* petof,    float* ptofcorr,
		    float* pdtof,
		    float* ptofmin, float* petofmin, float* ptofmincorr,
		    float* ptofmax, float* petofmax, float* ptofmaxcorr );

private: 

  //EmcScSectorRec ScSector[6];
  //EmcGlSectorRec GlSector[2];
  std::vector<EmcSectorRec*> fScSector;
  std::vector<EmcSectorRec*> fGlSector;  
  std::vector<SecGeom*> fSectorGeometries;  
 
  float fMinClusterEnergySc;
  float fMinClusterEnergyGl;
  
  static const int MAX_SECTORS_PROCESS=8;
  static const int MaxNofPeaks=10;
  static const int HITS_TO_TABLE=16;
};

#endif /*__EMCCLUSTERRECO_H__*/
