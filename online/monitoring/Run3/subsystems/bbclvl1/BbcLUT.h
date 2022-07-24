#ifndef __BBCLUT__
#define __BBCLUT__

class OnlMon;

class BbcLUT {

 public: 

  BbcLUT(const OnlMon *parentmon);
  virtual ~BbcLUT() {}

  unsigned char getLL1time(int channel, int T2) {
    if(channel>=0 && channel<128 && T2>=0 && T2<4096)
      return LUT[channel][T2>>1];
    else return 0;
  };

  void fillLL1array(unsigned char* ll1array, const int* T2array);

  int Success(){return success;};

 protected:
  const OnlMon *mymon;
  unsigned char LUT[128][2048];
  int success;

};
#endif




