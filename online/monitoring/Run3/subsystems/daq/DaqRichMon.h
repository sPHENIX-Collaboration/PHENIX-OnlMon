#ifndef __DAQRICHMON_H__
#define __DAQRICHMON_H__

#include <GranuleMon.h>

class DaqRichMon: public GranuleMon
{
 public:
  DaqRichMon(const char *arm);
  virtual ~DaqRichMon() {}

  int Init();

  int DcmFEMParityErrorCheck();
  int FEMParityErrorCheck();

 protected:

};

#endif /* __DAQRICHMON_H__ */
