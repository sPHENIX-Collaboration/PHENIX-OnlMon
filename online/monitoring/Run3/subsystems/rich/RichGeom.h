#include "RICHMonDefs.h"

#ifndef __RICHGEOM__
#define __RICHGEOM__

class RichGeom {

 public:
  RichGeom();
  ~RichGeom();
  int get_pmtnum(int ich,int fem_id);
  int get_fem_address(int pmtnum,int *fem_id,int *chan);
  int get_pmt_coords(int pmtnum,int *sect,float *z,float *phi);
};

#endif
