#include "ERTMaskMonCommon.h"

const char* ERTMaskMonCommon::DMUX_DIR_DEFAULT =
"/export/software/oncs/online_configuration/Arcnet/ert/hex/DMUX";

const char* ERTMaskMonCommon::SECTOR_NAME[N_ARMSECT] = {
   "W0", "W1", "W2", "W3", "E0", "E1", "E2", "E3"
};

const char* ERTMaskMonCommon::HALF_SECTOR_NAME[N_HALF_SECTOR] = {
   "W0-N", "W0-S", "W1-N", "W1-S", "W2-N", "W2-S", "W3-N", "W3-S", 
   "E0-S", "E0-N", "E1-S", "E1-N", "E2-S", "E2-N", "E3-S", "E3-N"
};

const int ERTMaskMonCommon::PACKET_ID[N_ARM] = { 14201, 14200 };
const int ERTMaskMonCommon::ROC_NUM[N_ROC_LINE] = {
   1, 3, 6, 10, 12, 15, 17, 19
};

const int ERTMaskMonCommon::SERIAL_SM_BORDER[N_DETECTOR][N_ARMSECT + 1] = {
   { 0, 18, 36, 54,  72, 104, 136, 154, 172 },
   { 0, 32, 64, 96, 128, 160, 192, 224, 256 }
};

const char* ERTMaskMonCommon::TRIG_NAME_MB = "BBCLL1(>0 tubes) narrowvtx";
const char* ERTMaskMonCommon::TRIG_NAME_NV = "BBCLL1(>0 tubes) novertex";
//const char* ERTMaskMonCommon::TRIG_NAME_LP = "BBCLL1(noVtx)&(ZDCN||ZDCS) ";
const char* ERTMaskMonCommon::TRIG_NAME_ZD = "ZDCLL1wide";
int ERTMaskMonCommon::IsPbGl(const int arm, const int sector)
{
   if (arm == ARM_EAST && sector < 2) return true;
   else                               return false;
}

int  ERTMaskMonCommon::ArmSectSmToSerialSm(const Detector det, const int arm, const int sector, const int sm)
{
   if (det == DET_EMC)
   {
      if (arm == 0) // west
      {
         return sector * N_SM_PBSC + sm;
      }
      else if (arm == 1) // east
      {
         if (sector <= 1)
            return  N_SECTOR           * N_SM_PBSC + sector*N_SM_PBGL + sm;
         else
            return (N_SECTOR+sector-2) * N_SM_PBSC +      2*N_SM_PBGL + sm;
      }
      return -1;
   }
   else // RICH
   {
      return (arm * N_SECTOR + sector) * N_SM_RICH + sm;
   }
}

void ERTMaskMonCommon::SerialSmToArmSectSm(const Detector det, const int ssm, int& arm, int& sector, int&sm)
{
   if (det == DET_EMC)
   {
      if (0 <= ssm && ssm < PBGL_START)
      {
         arm    = 0;
         sector = ssm / N_SM_PBSC;
         sm     = ssm % N_SM_PBSC;
      }
      else if (PBGL_START <= ssm && ssm < PBGL_END)
      {
         arm    = 1;
         sector = (ssm - PBGL_START) / N_SM_PBGL;
         sm     = (ssm - PBGL_START) % N_SM_PBGL;
      }
      else if (PBGL_END <= ssm && ssm < N_SM_EMC_TOTAL)
      {
         arm    = 1;
         sector = (ssm - PBGL_END) / N_SM_PBSC + 2;
         sm     = (ssm - PBGL_END) % N_SM_PBSC;
      }
      else
      {
         arm    = -1;
         sector = -1;
         sm     = -1;
      }
   }
   else // RICH
   {
      arm    = (ssm / N_SM_RICH) / N_SECTOR;
      sector = (ssm / N_SM_RICH) % N_SECTOR;
      sm     =  ssm % N_SM_RICH;
   }
}
