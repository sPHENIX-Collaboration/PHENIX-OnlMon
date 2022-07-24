#ifndef __MUI_MON_DRAW_CONST_H__
#define __MUI_MON_DRAW_CONST_H__

// limits for instructions/error reporting
const int MAX_NMAINFRAME_ERRORS = 1;
const float MAX_ROCOK_ERRORFREQ = 0.006;
const int MAX_NBEAM_OKERRORS = 10;
const int MIN_NEVENTS = 2000;
const int MAX_REF_ERROR = 0;

// limits for hit rate. (see CVS log for older values for previous runs and species)
const float MIN_HIT_RATE[2] = { 10.0, 0.1 }; // run16 dAu 62 GeV
const float MAX_HIT_RATE[2] = { 30.0, 3.0 }; // run16 dAu 62 GeV

// some helpful constants
const int NPLOT_PANELS = 5;
const int NPADS = NPLOT_PANELS + 1;
const int NBLT_CANVAS = 5;

const int muidFillColor = 38;
const int muidPadColor = 17;

// lines for reference plotting
const float effMin = 0.5;
const float effMax = 1.5;

// max/min limits for plotting
// no references
const float RmaxROC = 500.;
const float RminROC = 0.1;
const float RmaxHits = 50.;
const float RminHits = 0.01;
// references
const float RmaxRef = 5.0;
// requested by Vince to show zero
//const float RminRef = 0.001;
const float RminRef = -0.2;

// for error reporting
const float nSigma = 2.0; // how far away from the limit can the values be and still be called bad
const int nMinNeededBad = 1; // need at least these many for any given plot to signal problem (above known bad)
const int nKnownBad[2][NPLOT_PANELS] = { {1, 2, 2, 2, 2}, // South
					 {1, 2, 2, 2, 3}}; // North
// explanation: North line 0,1,0,3,0 means no known bad ROCs, one strange horiz HV chain, 3 strange horiz cables 
// there are empty chains; different in the horizontal and vertical directtions
const int NUnUsedH = 10;
const int NUnUsedV = 20;
const int unUsedHChannels[NUnUsedH] = {10, 15, 34, 39, 58, 63, 82, 87, 106, 111};
const int unUsedVChannels[NUnUsedV] = {9, 12, 13, 16, 33, 36, 37, 40, 57, 60, 
				       61, 64, 81, 84, 85, 88, 105, 108, 109, 112};

#endif /* __MUI_MON_DRAW_CONST_H__ */
