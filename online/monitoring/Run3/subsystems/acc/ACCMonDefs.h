#ifndef __ACCMONDEFS_H__
#define __ACCMONDEFS_H__

#define ACC_ADC_CONST_FILE "AccCalib.adcgain.new"
#define ACC_ADCPED_CONST_FILE "AccCalib.adcpedestal.new"
#define ACC_TAC_CONST_FILE "AccCalib.tacpedestal.new"
 
const int ACC_NCHANNEL = 160;
const int ACC_ALLCHANNEL = 320;

#define ACC_NCANVAS 2
#define ACC_NBUFEVT     5000

#define ACC_EV_THRESH 5
#define ACC_HITS_THRESH 5
//#define ACC_PMT_NOHIT 1.5 //Seting for Run4
//#define ACC_PMT_NOHIT 1.5  //Seting for Run5
//#define ACC_PMT_NOHIT 1.0  //Seting for Run5 (pp)
//#define ACC_PMT_NOHIT 1.5 //Seting for Run8 (dAu)
#define ACC_PMT_NOHIT 1.0 //Seting for Run8 (pp)
#define ACC_PMT_HIHIT 6.5
#define ACC_ADCHIT_DEF 0.3
//#define ACC_DEF_HIT 3 //change due to the TAC pedestal shift
#define ACC_DEF_HIT 10
#define ACC_HIT_ERROR_LOOP 100
#define ACC_TACTHRE 1.5

#define ACC_LOWADC_DESCISION 10

#define ACC_SPIKE_DEF 100

#ifndef ACCPMT_HIT
#define ACCPMT_HIT 1
#define ACCPMT_NOHIT 0
#endif

#ifndef ACCPMT_DEAD
#define ACCPMT_DEAD 0
#define ACCPMT_ALIVE 1
#endif

#ifndef PERMIT_ACC_PACKET_ERROR
#define PERMIT_ACC_PACKET_ERROR 0.02
#endif


#endif // __ACCMONDEFS_H__
