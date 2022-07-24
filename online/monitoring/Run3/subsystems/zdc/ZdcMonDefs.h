#ifndef __ZDCMONDEFS_H__
#define __ZDCMONDEFS_H__

const int num_pmt = 8;  // number of PMT
const int tdc_max = 4096;  // TDC number of max
const int tdc_min_overflow = 2700;  // TDC number of max
const int tdc_max_overflow = 4000;  // TDC number of max
const float max_armhittime = 22; // Minimum of Tdc-ArmHitTime
const float min_armhittime = 0; // Maximum od Tdc-ArmHitTime
const float min_zvertex = -150;
const float max_zvertex = +150;
const float TDC_CONVERSION_FACTOR = 0.007;

// the mean of TDC between south and north should be this value.
static const float ZDC_DEFAULT_OFFSET = 10.5; 

static const int ZDC_PACKET_ID  = 13001;

#endif /*__ZDCMONDEFS_H__ */
