#ifndef __MUTRMONDRAWCONSTS_H__
#define __MUTRMONDRAWCONSTS_H__

const int LW = 1; // line width
const int BCOL = 4; // border color
const int WCOL = 2; // warning color
const float TXTSIZE = 0.045;
const float TXTSCALE = 1.0; // how far above the max should the text be?
const float BSCALE = 1.0; // how far above the max should the borders go?
const float MSIZE = 0.5; // marker size

// constants for how to position error messages
const float ERRORSPACE = 0.1;
const float ERROROFF = 0.1;

// warning limits for histos in pad 0 and 1: take the average value and the thresholds are respectively 10% less and 10% time the value.
// index is arm
// pad 0 (left plots on the monitoring): packets

// thresholds for missing fem: minimum value
const float YMINPACKETS[2] = { 0.00001, 0.00001};// limits for any system and energy

// threshold for hot packets : maximum value
//const float YMAXPACKETS[2] = { 0.02, 0.02}; // limits for p+p, any energy
//const float YMAXPACKETS[2] = { 0.06, 0.06}; // limits for p+p, 500 and 200 GeV Run09
// const float YMAXPACKETS[2] = { 0.15, 0.15}; // limits for 200GeV AuAu Run7, Run10
// const float YMAXPACKETS[2] = { 0.25, 0.15}; // limits for 510GeV pp Run13 xj:040113.
const float YMAXPACKETS[2] = { 0.25, 0.25}; // limits for 510GeV pp Run13 xj:041813.
//const float YMAXPACKETS[2] = { 0.1, 0.1}; // these limits were set for 200GeV CuCu run

// when should we warn? (how many bins can be above or below set limits)
const int MAX_ABOVEPACKETS[2] = { 0, 0};
const int MAX_BELOWPACKETS[2] = { 0, 0};

// pad 1 (right plots on the monitoring): planes
// thresholds for low occupancy packets:  
//const float YMINPLANES[2] = { 0.005, 0.005};// limits for any system and energy
const float YMINPLANES[2] = { 0.00005, 0.00005};// May 24, 2016 yuhw

// thresholds for noisy channels: maximal value
//const float YMAXPLANES[2] = { 2., 2.}; // limits for p+p Run-08
//const float YMAXPLANES[2] = { 7., 7.}; // limits for p+p 500GeV Run09, Run12
//const float YMAXPLANES[2] = { 4., 4.}; // limits for p+p 200GeV Run09
const float YMAXPLANES[2] = { 40., 40.}; // limits for 200GeV AuAu Run7 and Run10
//const float YMAXPLANES[2] = { 30., 30.}; //limits for 62GeV AuAu Run10
//const float YMAXPLANES[2] = { 3., 7.}; // limits for 63GeV CuCu
//const float YMAXPLANES[2] = { 10., 20.}; // limits for 200GeV CuCu run

// when should we warn? (how many bins can be above or below set limits)
const int MAX_ABOVEPLANES[2] = { 0, 0};
const int MAX_BELOWPLANES[2] = { 0, 0};

// limits to determine whether gain is ok : from cluster peak adc distr. 
const int PEAKMIN = 130;
// 4/29/11 C. Aidala - Change PEAKMAX from 300 to 330 to avoid errors in monitoring while reassessing HV settings
const int PEAKMAX = 330; 
const float FIT_RANGE_PEAKADC_MIN=70.0;
const float FIT_RANGE_PEAKADC_MAX=500.0;

// limit to determine whether timing is ok. Max allowable binmax/bin7 ratio 
// xjiang 1/27/2011. set to 1.005, was 1.01 earlier
// xjiang 2/12/2011. set back to 1.01, after physics started in run11.
const float ATLIMIT = 1.01;

// limit for packet/AMU cell errors. More than this gives a warning
const int AMUERRLIMIT = 2; 
const int AMUERRCOLOR = 2; // red
//const float AMUERRFREQ = 0.005;
const float AMUERRFREQ = 0.007; // default is 0.005. Changed by I.Nakagawa (May 20, 2014)

// canvas appearance
const int NPADSPERARM = 4;
const float DISTBORD = 0.025; // distance to border
const float PADSIZE = 0.5; // we have a (0-1)x(0-1) space to place out our pads on
// each of the 4 will thus take up a 0.5x0.5 space

const int NBAR = 8;// per arm: 2 pads * (2 station borders + 2 warning limits)
const int NTEX = 15;// per arm: 15 pieces of tex
const int NPAVETEX = 3;// per arm: 3 pieces of tpavetex

// we need at least these many events
const int MUTRMINEVENTS = 2500;

#endif // __MUTRMONDRAWCONSTS_H__
