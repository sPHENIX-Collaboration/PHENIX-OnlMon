#ifndef __SVXPIXELMONDEFS_H__
#define __SVXPIXELMONDEFS_H__

#include <string>

enum{ EVENTCOUNTBIN = 1, NUMBINS = 1 };

const int ntype=6;
enum{ TYPE_RAW, TYPE_HOT, TYPE_COLD, TYPE_KNOWNISSUE, TYPE_KNOWNHOT, TYPE_DEAD };
const std::string desc_type[ntype]={"RAW","HOT","COLD","KNOWNISSUE","KNOWNHOT","DEAD"}; // order is important to drawing

const int NBARREL=2;
const int NLADDER=30;
const int NCHIP=8;
const int NCOL=32;
const int NROW=256;

const int ncanvas=9;
const int nmaxpad=15;

const int vtxp_npacket=60;
const int vtxp_packet_base=24001;

#endif
