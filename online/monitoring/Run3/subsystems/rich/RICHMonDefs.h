#ifndef __RICHMONDEFS_H__
#define __RICHMONDEFS_H__

#define RICH_ADC_CONST_FILE "rich_calibration_constants.asc"
#define RICH_TAC_CONST_FILE "rich_tac_calibration_constants.asc"
#define RICH_TAC_T0_CONST_FILE "rich_tac_t0_constants.asc"
 
#define RICH_NCHANNEL    5120
//#define RICH_NCHANNEL    600
#define RICH_NBUFEVT     5000


#define RICH_ADCHIT_DEF 0.4
#define RICH_ADCHIT_DEF_FORTDC 0.7
#define RICH_DEF_HIT 3
#define RICH_HIT_ERROR_LOOP 100

#define RICH_LOWADC_DESCISION 5
#define RICH_LOWTDC_DESCISION 0.33

#define RICH_SPIKE_DEF 50


#define RICH_TACHIT_LOW -100
#define RICH_TACHIT_HIGH 150

#ifndef RICHPMT_HIT
#define RICHPMT_HIT 1
#define RICHPMT_NOHIT 0
#endif

#ifndef RICHPMT_DEAD
#define RICHPMT_DEAD 1
#define RICHPMT_ALIVE 0
#endif

#ifndef PERMIT_RICH_PACKET_ERROR
#define PERMIT_RICH_PACKET_ERROR 0.02
#endif


#endif // __RICHMONDEFS_H__
