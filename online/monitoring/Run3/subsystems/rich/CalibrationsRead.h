#ifndef __CALIBRATIONSREAD_H__
#define __CALIBRATIONSREAD_H__

#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>


class CalibrationsRead {

 public:
  CalibrationsRead(){}
  virtual ~CalibrationsRead(){}

  int getcalibs (std::string calfile, float pedestal[],float sigped[],float pep[],float sigpep[],std::string taccalfile,float tacpedestal[],float tacsigped[],float tacpep[],float tacsigpep[],int dead[],int tacdead[] );
};

#endif

