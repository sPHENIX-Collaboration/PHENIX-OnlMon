#include "RichGeom.h"

RichGeom::RichGeom() {

}

RichGeom::~RichGeom() {

}

int RichGeom::get_pmtnum(int ich,int fem_id) {

  // Determine pmtnum from fem_id and location in packet
  // ich is the channel in the packet (0-159)
  // row_map contains the cable numbers in order of readout

  int row_map[10] = {5,0,6,1,7,2,8,3,9,4};

  int row=ich/16;
  int chan=ich-row*16;
  int cable=row_map[row];
  int pmtnum=cable*16+chan;

  pmtnum=pmtnum+fem_id*160;

  return pmtnum;
}

int RichGeom::get_fem_address(int pmtnum,int *rfem_id,int *rchan) {

  // Determine the fem address from the pmt number
  // cable_map contains the row (16 channels per row) for each cable

  int cable_map[10] = {1,3,5,7,9,0,2,4,6,8};

  int fem_id=pmtnum/160;
  int rel_pmtnum=pmtnum-fem_id*160;
  int cable=rel_pmtnum/16;
  int cable_pmtnum=rel_pmtnum-cable*16;
  int row=cable_map[cable];
  int chan=row*16+cable_pmtnum;

  *rfem_id=fem_id;
  *rchan=chan;
  
  return 0;
}

int RichGeom::get_pmt_coords(int pmtnum,int *sect,float *z,float *phi) {

  // Determine the PMT coordinates from the pmt number

  // The first 1280 PMT's are in the south end of the west RICH, sector 0
  // The second 1280 are in the north end of the west RICH, sector 1
  // The third 1280 are in the south end of the east RICH, sector 2
  // The fourth 1280 are in the north end of the east RICH, sector 3

  float pmt_phi_min= -33.0365;
  float pmt_dphi= 1.11216;

  float z_pmt[32]={141.227,150.056,158.885,167.714,176.543,185.372,194.201,
		   203.03,211.859,220.688,229.532,237.948,246.055,253.983,
		   260.134,264.587,145.53,154.359,163.188,172.017,180.846,
		   189.675,198.504,207.333,216.162,225.217,233.848,242.047,
		   250.061,257.907,262.362,266.815};

  int row=pmtnum/16;
  int zloc=pmtnum-row*16;
  int sector=row/80;
  int iphi=row-sector*80;

  *phi = (pmt_phi_min + (iphi+0.5)*(pmt_dphi));  

  if(((sector == 1 || sector == 2) && (iphi%2 == 1)) ||
     ((sector == 0 || sector == 3) && (iphi%2 == 0))) {
    *z = z_pmt[zloc];
  } else {
    *z = z_pmt[zloc + 16];
  }
  if( sector%2 == 0) *z = -*z;   // Z < 0

  *sect=sector;

  return 0;
}
