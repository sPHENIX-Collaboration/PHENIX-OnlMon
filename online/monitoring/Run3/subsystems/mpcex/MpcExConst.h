// MPCEX ONLINE MON. constant list 
#ifndef __MPCEXCONST_H__
#define __MPCEXCONST_H__

enum {nPacket = 16};
enum {nROC = 24};
enum {nChannelMax = 256};
enum {nDynRange = 256};
enum {nLayer = 8};
enum {nArm = 2};
enum {nSensor = 24};

const int HitThreshold = 80;
const int HitLowThreshold_Packet = 2250; 
const int HitHighThreshold_Packet = 2300;//2750;
const int HitLowThreshold_Chip = 95; 
const int HitHighThreshold_Chip = 100; //125;  
const int MinimumEvent = 100; 

const int NominalMIP = 1500; // Nominal MIP point for high_q
const int OverflowThreshold = 4000; // Threshold for low_q overflow
const int sensor_id_map[6][6] = {{-1, 3, 8,12,16,-1},//y=0 & x read down [x][y]
				 { 0, 4, 9,13,17,21},
				 { 1, 5,-1,-1,18,22},
				 { 2, 6,-1,-1,19,23},
				 {-1, 7,10,14,20,-1},
				 {-1,-1,11,15,-1,-1}};

#endif
