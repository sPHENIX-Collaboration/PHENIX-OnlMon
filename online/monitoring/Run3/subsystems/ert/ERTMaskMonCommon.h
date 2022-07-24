#ifndef __ERTMASKMONCOMMON_H__
#define __ERTMASKMONCOMMON_H__

namespace ERTMaskMonCommon {

   const int N_ARM     = 2; // 0 = West, 1 = East.
   const int N_SECTOR  = 4; // Sector in arm.
   const int N_ARMSECT = 8; // W0...W3 E0...E3.
   const int N_HALF_SECTOR = 16;
   extern const char* SECTOR_NAME[N_ARMSECT];
   extern const char* HALF_SECTOR_NAME[N_HALF_SECTOR];

   const int ARM_EAST = 1;
   const int ARM_WEST = 0;

   const int N_SM_PBGL = 32;
   const int N_SM_PBSC = 18;
   const int N_SM_RICH = 32;
   const int N_SM_EMC_TOTAL  = 172;
   const int N_SM_RICH_TOTAL = 256;

   const int N_DETECTOR = 2;
   enum Detector {
      DET_EMC = 0, DET_RICH = 1
   };

   extern const char* DMUX_DIR_DEFAULT;

   extern const int PACKET_ID[N_ARM]; // CAUTION!  0 = East, 1 = West
   const int N_ROC_LINE = 8;
   extern const int ROC_NUM[N_ROC_LINE];

   extern const int SERIAL_SM_BORDER[N_DETECTOR][N_ARMSECT + 1];
   const int PBGL_START =  72;
   const int PBGL_END   = 136;

   extern const char* TRIG_NAME_MB;
   extern const char* TRIG_NAME_NV;
  
   //extern const char* TRIG_NAME_LP;
   extern const char* TRIG_NAME_ZD;
   int  IsPbGl(const int arm, const int sector);
   int  ArmSectSmToSerialSm(const Detector det, const int arm, const int sector, const int sm);
   void SerialSmToArmSectSm(const Detector det, const int ensm, int& arm, int& sector, int&sm);
};

#endif // __ERTMASKMONCOMMON_H__
