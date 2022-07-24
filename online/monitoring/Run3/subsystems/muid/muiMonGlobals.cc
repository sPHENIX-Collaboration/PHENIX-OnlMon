#include "muiMonGlobals.h"

muiMonGlobals* muiMonGlobals::pmuiMonGlobals = 0L;

muiMonGlobals* muiMonGlobals::Instance()
{
  if (! pmuiMonGlobals)
    {
      pmuiMonGlobals = new muiMonGlobals;
    }
  return pmuiMonGlobals;
}

muiMonGlobals::muiMonGlobals()
{

  verbose = -1;
  ClearRun();
  cable_hist_update_freq = 1000000;
  dead_mainFrame_freq = 1000000;
  for (int i = 0;i < MAX_ARM;i++)
    {
      arm_active[i] = true;
    }
  return ;
}

void 
muiMonGlobals::ClearRun()
{
  eventNum = 0;
  eventNumAll = 0;
  eventNumRejected = 0;
  for (int i = 0; i < MAX_ARM;i++)
    {
      rocOkErrors[i] = 0;
      beamOkErrors[i] = 0;
      noPacketErrors[i] = 0;
      deadHvChainErrors[i] = 0;
      deadRocErrors[i] = 0;
      deadChipErrors[i] = 0;
      mainFrameErrors[i] = 0;
      deadCableErrors[i] = 0;
    }
}
