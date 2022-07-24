#ifndef __SVXSTRIPMONDEFS_H__
#define __SVXSTRIPMONDEFS_H__

#include <string>

const int NRCC = 6;
const int NCHANNEL = 128;
const int NCHIP = 12;

static const int VTXS_NPACKET = 40;
static const int VTXS_PACKET_BASE = 24101;
static const int NRCC_PER_LADDER[4] = {0, 0, 5, 6};

enum { EVENTCOUNTBIN = 1, NUMBINS = 1 };
const unsigned int ntype = 6;
// order is important to drawing
enum { TYPE_RAW, TYPE_HOT, TYPE_COLD, TYPE_KNOWNISSUE, TYPE_KNOWNHOT, TYPE_DEAD};    // the order has been chosen this way on purpose
const std::string desc_type[ntype] = {"RAW", "HOT", "COLD", "KNOWNISSUE", "KNOWNHOT", "DEAD"};
#endif
