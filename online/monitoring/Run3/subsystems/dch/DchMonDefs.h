#ifndef __DCHMONDEFS_H__
#define __DCHMONDEFS_H__


//Drift Chamber geometry hard numbers
#define NUMARM 2
#define NUMSIDE 2
#define NUMBOARD 80
#define NUMPLANE 40
#define MAXPACKETLENGTH 1000  

#define DCH_HISTORY_LENGTH 10000


#define ALLEY_WIDTH_RADIANS (3.141592654*4.5)/(8.0*180.0)


#define DV_ALERT_RED 0.08 //fractional changes
#define DV_ALERT_YEL 0.04

#define T0_ALERT_RED 10 //absolute change in timebins
#define T0_ALERT_YEL 7

#define DEAD_ALERT_RED 240  //number of dead channels
#define DEAD_ALERT_YEL 100  //different from template

#define NOIS_ALERT_RED 20  //number of noisy channels
#define NOIS_ALERT_YEL 7   //different from template

#define SW_ALERT_YEL 82
#define SW_ALERT_RED 78  //real 86


#define POINTS 40

#define NOISEARRAYSIZE 1500
#define DEADARRAYSIZE 12800

#define NDCHP 160  //number of Drift Chamber packages per event

#define WidthCut 20

//coloring scheme for hit rate 2D histograms
#define DEADCOL     1    //1:black  dead
#define NOISYCOL    2    //2:red    noisy
#define GOODCOL     3    //3:green  good
#define COLDCOL     4    //4:blue   cold
#define HOTCOL      6    //6:violet hot
#define MASKCOL    17    //17:gray  masked channel (previously dead in DchDead.txt)
#define nCOLORS     6    //total number of colors

#define MASK     0.1    //17:gray  masked channel (previously dead in DchDead.txt)
#define DEAD     1.1    //1:black  dead
#define COLD     2.1    //4:blue   cold
#define GOOD     3.1    //3:green  good
#define HOT      4.1    //6:violet hot
#define NOISY    5.1    //2:red    noisy
#define MIN_HITRATE 0.0 // minimum value in a hitrate plot
#define MAX_HITRATE 6.0 // maximum value in a hitrate plot

#define SIGMA_HITRATE_AUAU 0.115  //sigma of the hit rates distribution
#define SIGMA_HITRATE_DAU  0.180
#define SIGMA_HITRATE_PP   0.180

//encoder-decoder of dc_info histogram definition of bin numbers
//thats the way that DchMon communicates with DchMonDraw, through the dc_info histogram
#define RUNNUM_BIN 1 
#define NEVT_BIN   2

#define OLD_NOISE_NUM_00_BIN 3
#define OLD_NOISE_NUM_01_BIN 4
#define OLD_NOISE_NUM_10_BIN 5
#define OLD_NOISE_NUM_11_BIN 6

#define NEW_NOISE_NUM_00_BIN 7
#define NEW_NOISE_NUM_01_BIN 8
#define NEW_NOISE_NUM_10_BIN 9
#define NEW_NOISE_NUM_11_BIN 10

#define MISSING_NOISE_NUM_00_BIN 11
#define MISSING_NOISE_NUM_01_BIN 12
#define MISSING_NOISE_NUM_10_BIN 13
#define MISSING_NOISE_NUM_11_BIN 14

#define AVG_LOAD_00_BIN 15
#define AVG_LOAD_01_BIN 16
#define AVG_LOAD_10_BIN 17
#define AVG_LOAD_11_BIN 18

#define DEAD_REPORT_BIN 19
#define INFOGROUP_BIN 20

#define TEMPLATE_NOISY_EAST_BIN  21
#define TEMPLATE_NOISY_WEST_BIN  22
#define TEMPLATE_DEAD_EAST_BIN   23
#define TEMPLATE_DEAD_WEST_BIN   24
#define TEMPLATE_TZERO_EAST_BIN  25
#define TEMPLATE_TZERO_WEST_BIN  26
#define TEMPLATE_VDRIFT_EAST_BIN 27
#define TEMPLATE_VDRIFT_WEST_BIN 28
#define TEMPLATE_EFF_EAST_BIN    29
#define TEMPLATE_EFF_WEST_BIN    30

#define NOISY_EAST_BIN     31
#define NOISY_WEST_BIN     32
#define DEAD_EAST_BIN      33
#define DEAD_WEST_BIN      34
#define TZERO_EAST_BIN     35
#define TZERO_WEST_BIN     36
#define VDRIFT_EAST_BIN    37
#define VDRIFT_WEST_BIN    38
#define EFF_EAST_BIN       39
#define EFF_WEST_BIN       40
#define SPECIES_BIN        41
#define TEMPLATE_LLOAD_ALERT_RED_BIN 42 
#define TEMPLATE_LLOAD_ALERT_YEL_BIN 43
#define TEMPLATE_HLOAD_ALERT_RED_BIN 44
#define TEMPLATE_HLOAD_ALERT_YEL_BIN 45
#define BADEV_BIN 46
#define NDC_INFO_BIN 46  //The number of bins in the dc_info histogram 

#endif // __DCHMONDEFS_H__
