#ifndef __MUI_MON_CONST_H__
#define __MUI_MON_CONST_H__

const short MAX_ARM = 2;

//MAX_ARM should be same as TMuiChanneiId::kArmsTotal
const short MAX_ORIENTATION = 2; 
//MAX_ORIENTATION should be same as TMuiChannelId::kOrientations
const short MAX_ROC = 20;

const short WORD_PER_ROC = 6;
const short BIT_PER_WORD = 16;
const short MAX_FEM  = MAX_ARM * MAX_ORIENTATION; // 4
const short WORD_PER_FEM =  WORD_PER_ROC * MAX_ROC; // 120
//const short HEADER_WORDS  = 5;
const short USR_WORDS  = 8;
const short MAX_WORD  = WORD_PER_FEM * MAX_FEM; // 240
const short CHANNEL_PER_FEM  = BIT_PER_WORD * WORD_PER_FEM; // 1920
const short MAX_CHANNEL  = CHANNEL_PER_FEM * MAX_FEM; // 3840
const short PANEL_PER_GAP  = 6;
const short GAP_PER_ARM  = 5; 
//MAX_CHANNEL should be same as TMuiChannelId::kTwoPackMaxTotal(kHitsMaxTotal)
const short MAX_SCALEDTRIG_BIT  = 32;
const short MAX_PSEUDOTRIG_BIT  = 32;

const short CLOCK_TRIGGER_BIT = 1;

const short MAX_CROSSCTR  = 120;
const short MAX_GL1P_SCALERS  = 4;

const short MAX_TOTAL_HITS = 1000;

//const int HIST_FLAG = 1+4+16+32+256;
// 1: original one
// 2: test
// 4: correlation histograms for mapping check
// 8: GL1
// 16: Pseudo Trigger
// 32: Time dependence
// 64: Cross Talks
// 128: GL1P
// 256: Physical Twopacks;
// 512: ROC hit

namespace MUIMONCOORD {enum {SOUTH = 0, NORTH = 1, HORIZ = 0, VERT = 1};};

// should BLT stuff be included or not? Comment out if it comes back in use
//#define MUID_USE_BLT 1
 
#endif /* __MUI_MON_CONST_H__ */


