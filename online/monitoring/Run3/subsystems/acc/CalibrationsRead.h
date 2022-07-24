#ifndef __CALIBRATIONSREAD_H__
#define __CALIBRATIONSREAD_H__

class CalibrationsRead {

 public:
  CalibrationsRead(){}
  virtual ~CalibrationsRead(){}

  int getcalibs(float *pep,float *sigpep,float *pedestal,float *sigped,float *tacped,float *sigtacped,int *dead,int *tacdead);

};

#endif

