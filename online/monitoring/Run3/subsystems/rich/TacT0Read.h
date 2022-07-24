#ifndef __TACT0READ_H__
#define __TACT0READ_H__

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class TacT0Read {
 public:
  TacT0Read(){}
  virtual ~TacT0Read(){}

  int gett0 (std::string tact0file, float tact0[]);
};

#endif
