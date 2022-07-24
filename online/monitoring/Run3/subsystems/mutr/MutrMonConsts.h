#ifndef __MUTRMONCONSTS_H__
#define __MUTRMONCONSTS_H__

const int NARMS = 2;
const int NPACKETSARM[NARMS] = { 168, 192 };
const int FIRSTPACKET[NARMS] = { 11001, 11171}; 
const int NTOTSTRIPS[NARMS] = { 19968, 24000}; 
const int NCHANNELS = 128;    // for each packet

const int NSTATIONS = 3;
const int NOCTANTS = 8;     // per station
const int NPLANES[NSTATIONS] = { 6, 6, 4};  // total per station
const int NSTRIPS[NSTATIONS*NARMS] = { 96, 160, 240, // first South, then North
                                       96, 192, 320 };  // total per octant and plane

const int MASK = 0x7ff; // 11 bit range in data sample
const int AMUMASK = 0x3f; // 6 bit range in AMU

const float LINLIMIT = 0.1; // sample3 and sample2 should be linear within 10% to be included.   
#endif // __MUTRMONCONSTS_H__
