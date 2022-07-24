#include "AccGeom.h"

int AccGeom::get_pmtnum(int ich, int fem_id)
{
  // Determine pmtnum from fem_id and location in packet
  // ich is the channel in the packet (0-159)
  // row_map contains the cable numbers in order of readout
  int npacket = (fem_id-17001)/4;
  int pmtnum = ich + 160*npacket;

  return pmtnum;
}

int AccGeom::get_fem_address(int pmtnum, int *rfem_id, int *rchan)
{

  int fem_id = pmtnum / 160;
  int chan = pmtnum;

  *rfem_id = fem_id;
  *rchan = chan;

  return 0;
}


int AccGeom::get_pmt_coords(int pmtnum, int *sect)
{

  // Determine the PMT coordinates from the pmt number

  // The first 160 PMT's are in the North
  // The second 160 are in the South

  int sector = pmtnum / 8;

  *sect = sector;

  return 0;
}


void AccGeom::get_boxid(int pmtnum, int *boxid)
{

  //There are 160 segments in Aerogel Cherenkov Counter,
  //each segments has 2PMTs in it .

  //Determin Box ID(1-160) and determin PMTs location Left or Right.

  int colum = pmtnum / 40;
  int sector = pmtnum / 8;
  int side = (pmtnum % 4) / 2;
  int ud = pmtnum % 2;

  int Boxid = colum * 10 + sector * 2 + side * 10 + ud + 1;

  *boxid = Boxid;

}

void AccGeom::get_pmtid(int boxid, int *pmtleft, int *pmtright)
{

  //Determin PMT ID from the Box ID of Aerogel Counter

  int bid = boxid - 1;
  int colum = bid / 20;
  int side = (bid / 10) % 2;
  int raw = (bid % 10) / 2;
  int ud = bid % 2;

  int pmtR = colum * 40 + raw * 8 + side * 2 + ud;

  *pmtright = pmtR;
  *pmtleft = pmtR + 4;


}
/*
  int AccGeom::get_pmt_coords(int pmtnum,int *sect,float *z,float *phi) {
 
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
*/
