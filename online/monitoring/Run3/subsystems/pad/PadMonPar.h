#ifndef PADMONPAR_H
#define PADMONPAR_H

// global
const short FIRSTPACKET = 4001;
const short NARMS = 2;
const short NLAYERS = 3;
const short NPLANES = NLAYERS*NARMS;
const short LASTPACKET = 4096;

// plane
const short NFEMSPERSIDE = 8;
const short NSIDES = 2;
const short NFEMSPERPLANE = NSIDES*NFEMSPERSIDE;
const short NFEMSPERLAYER = NARMS*NFEMSPERPLANE;
// thus we have a maximum of 6*16=96 FEMs with packetids 4001-4096

// run_01 configuration
const short PC1_W_INSTALLED = 1; // packets 4001-4016
const short PC1_E_INSTALLED = 1; 
const short PC2_W_INSTALLED = 1; // should be 1 for 2001
const short PC2_E_INSTALLED = 0;
const short PC3_W_INSTALLED = 1; // should be 1 for 2001
const short PC3_E_INSTALLED = 1; // packets 4081-4096

const short PC_INSTALLED[NPLANES] = { PC1_W_INSTALLED, PC1_E_INSTALLED, 
				      PC2_W_INSTALLED, PC2_E_INSTALLED,
				      PC3_W_INSTALLED, PC3_E_INSTALLED };

const short PC_NINSTALLED = PC1_W_INSTALLED + PC1_E_INSTALLED + PC2_W_INSTALLED + PC2_E_INSTALLED + PC3_W_INSTALLED + PC3_E_INSTALLED;

const short MAX_FEMS = NFEMSPERPLANE * PC_NINSTALLED;

// maximum number of hits per plane
const short MAX_CLUSTERS = 500; 
const short MAX_CLUSTERSIZE = 50;

// chamber
const short NHVPERCHAMBER = 4;
const short NHVSECTORS = 32; // per plane

// to monitor that all the HV is on, the following info is of interest
const short nwiresPerPC1HVSector[NHVPERCHAMBER] = { 9, 20, 20, 9 };
const short nwiresPerPC2HVSector[NHVPERCHAMBER] = { 24, 34, 34, 24 };
const short nwiresPerPC3HVSector[NHVPERCHAMBER] = { 29, 29, 29, 29 };

const short nwiresPerHVSector[NHVPERCHAMBER*NLAYERS] = { 9, 20, 20, 9, 
							 24, 34, 34, 24,
							 29, 29, 29, 29 };
// FEM
// let's first define a couple of constants. Per FEM unless otherwise stated
const short NWIRES = 58;
const short NCELLZ = 106;
const short NPADX = 20;
const short NPADZ = 108;
const short MAX_PADS = 2160; // 108 * 20
const short MAX_CELLS = 6148; // 106 * 58
const short NROWS = 5; // 5 rows in an FEM
const short NROCS = 9; // 9 ROCs in a row
const short MAX_ROCS = NROWS*NROCS;
const short NTGLS = 3; // 3 TGLs in a ROC
const short NCHAN = 16; // 16 channels in a TGL

// let's also define the maximum number of 2 pad combinations 
// (2 pads that could form a cell 
// together are on, but they are not members of any cell). 
const short MAX_2PADCOMB=10000;

// for the bad channel and bad ROC stuff. If we hit these limits for one FEM,
// then the whole FEM is definitely not OK..
const short MAX_BADCH=100; 
const short MAX_BADROC=10;

const float lowRelFEMCut=0.5;
const float highRelFEMCut=2.0;
const float lowRelHVCut=0.5;
const float highRelHVCut=2.0;

// *** constants used for event address check
// the possible evtnrs
const int EVT_NR_TYPE[5]={0x1,0x9,0x5,0xd,0x3};
// packets required in each arm, for the event to be analyzed
const int NRPACKETS_REQUIRED_WEST = 2;
const int NRPACKETS_REQUIRED_EAST = 2;
// max nr of errors accepted for a given package, in percent
const int MAX_ERROR_PER_PACKET = 10;
// less analyzed events for a packet yield screen message to run more events
const int EVENTS_REQUIRED_EVTNR = 25;
// max nr of allowed FEMs with bad address for screen message
const int MAX_ADR_ERR = 2; 
// Minimum number of events needed for html output
const int MIN_NEVENTS_HTML = 2000;
// *************************************************************************
#endif

