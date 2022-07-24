#ifndef GRANULEDEFS_H__
#define GRANULEDEFS_H__

#define PKT_HDR 6
#define CAV_WRD 1

// Bins in histogram
#define LENGTHBIN    1
#define DCMCHKSUMBIN 2
#define FEMPARITYBIN 3
#define FEMCLKBIN    4
#define FEMGL1CLKBIN 5
#define FEMEVTSEQBIN 6
#define NPACKETBIN   7
#define NGOODPACKETBIN 8
#define SUMBYTESBIN 9
#define DCMFEMPARBIN 10
#define GLINKBIN 11
#define SUBSYSTEMBIN 12
#define MISSINGPKTBIN 13
#define NBINSPKT 13


#define LASTFILLEDBIN 1
#define NUMEVENTBIN 2
#define GL1FATALBIN 3
#define SUBSYSFATALBIN 4
#define DROPCONTENTBIN 5
#define SIZEPEREVENTBIN 6
#define SCALEDFATALBIN 7
#define EVBERRORBIN 8
#define NBINSVAR 8


// granule definitions are hidden in
// /export/software/oncs/online_configuration/GL1/.gl1_granule_names
#define GRAN_NO_GL1 0
#define GRAN_NO_BBC 2
#define GRAN_NO_ZDC 3
#define GRAN_NO_VTXP 4
#define GRAN_NO_DCH_WEST 5
#define GRAN_NO_PAD_WEST 6
#define GRAN_NO_RPC3 7
#define GRAN_NO_MPCEX_NORTH 7
#define GRAN_NO_RICH_WEST 8
#define GRAN_NO_EMC_WEST_BOTTOM 9
#define GRAN_NO_EMC_WEST_TOP 10
#define GRAN_NO_DCH_EAST 11
#define GRAN_NO_MPCEX_SOUTH 12
#define GRAN_NO_VTXS 13
#define GRAN_NO_TOF_EAST 14
#define GRAN_NO_RICH_EAST 15
#define GRAN_NO_EMC_EAST_TOP 16
#define GRAN_NO_EMC_EAST_BOTTOM 17
#define GRAN_NO_MUTR_SOUTH 18

#define GRAN_NO_MUTR_NORTH 20
#define GRAN_NO_MUID_NORTH 21
#define GRAN_NO_ERT_EAST 22
#define GRAN_NO_ERT_WEST 23
#define GRAN_NO_FCAL 24
#define GRAN_NO_ACC 25
#define GRAN_NO_RPC1 26
#define GRAN_NO_FVTX 27
#define GRAN_NO_TOF_WEST 28
#define GRAN_NO_MUID_SOUTH 29
#define GRAN_NO_PAD_EAST 30

#define SUBSYS_PACKETEXIST 1
#define SUBSYS_PACKETMISS 2

static const unsigned int INIT_STANDARD_CLOCK = 0xFFFFFFFF;

#define NUPDATE 1000   // update after so many events
#define FIRSTUPDATE 100 // first update after so many events to get quick feedback

#endif

