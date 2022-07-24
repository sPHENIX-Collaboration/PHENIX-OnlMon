#ifndef __ERTLl1_h__
#define __ERTLl1_h__

#include <string>

#define MAX_ARM 2
#define MAX_ROC 8
#define WORD_PER_ROC 6
#define BIT_PER_WORD 16
#define MAX_FEM MAX_ARM
#define WORD_PER_FEM WORD_PER_ROC*MAX_ROC // 120
#define HEADER_WORDS 5
#define USR_WORDS 8
#define MAX_WORD WORD_PER_FEM*MAX_FEM // 240
#define CHANNEL_PER_FEM BIT_PER_WORD*WORD_PER_FEM // 1920
#define MAX_CHANNEL CHANNEL_PER_FEM*MAX_FEM // 3840

class Event;
class OnlMon;

struct revLUTentry
{
  int frame;
  int bit;
};


class ERTLl1{

  protected:

  int success;

 public:

  ERTLl1(const OnlMon *parent);
  virtual ~ERTLl1(){}

  int getDataFromERTRocPacket(Event*); 

  int calculate(short SimSyncErr[2][20]);
  void getROCNumber(int arm, int trig, int rocbit[2]);
  int Success() {return success;};

  int TrigFbFA(int iarm) {return (FourByFourA[iarm]>0) ? 1:0;}
  // int TrigFbFA(int iarm) {return (FourByFourA[iarm]);}
  int TrigFbFB(int iarm) {return (FourByFourB[iarm]>0) ? 1:0;}
  int TrigFbFC(int iarm) {return (FourByFourC[iarm]>0) ? 1:0;}
  int TrigTbT(int iarm) {return (TwoByTwo[iarm]>0) ? 1:0;}
  int TrigElectron(int iarm) {return (Electron[iarm]>0) ? 1:0;}
  int TrigTwoElectron(int iarm) {return (TwoElectron[iarm]>0) ? 1:0;}
  int TrigElectronBlueLogic(int iarm) {return (ElectronBlueLogic[iarm]>0) ? 1:0;}
  int TrigRxNP() {return (RxNP>0) ? 1:0;} 
  bool getROCbit(int iarm, int iroc, int iword);

  void load_ertelectron_lut(Event *e);
  int TranslateToROC(const int iroc);
  int TranslateToFiber(const int iroc);

 protected:
  const OnlMon *mymon;
  short ROCData[MAX_ARM][MAX_ROC][WORD_PER_ROC];
  short fiberBitMask[MAX_ARM][MAX_ROC][WORD_PER_ROC*BIT_PER_WORD];
  short fiberBitData[MAX_ARM][MAX_ROC][WORD_PER_ROC*BIT_PER_WORD];
  short ROCn[MAX_ARM][5][128][2];

  unsigned short AlgBitsArray[MAX_ARM][5][128];

  enum ERT_AlgBit_Types {FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO, RICH};
  
  short FourByFourA[MAX_ARM]; 
  short FourByFourB[MAX_ARM]; 
  short FourByFourC[MAX_ARM]; 
  short TwoByTwo[MAX_ARM];
  short Electron[MAX_ARM];
  short ElectronBlueLogic[MAX_ARM];
  short TwoElectron[MAX_ARM];
  short RxNP;
  
  bool SM_2x2_RICH_coincidence[2][4][2]; // [arm][SM#][north/south section] //4 SM per section (north/south)

  bool ElectronLUT[MAX_ARM][128][128];
  bool TwoElectronLUT[MAX_ARM][128][128];

  revLUTentry RocToFiber[WORD_PER_ROC*BIT_PER_WORD];
  bool RICH_used[MAX_ARM][22][WORD_PER_ROC*BIT_PER_WORD];
  bool EMCAL_used[MAX_ARM][22][WORD_PER_ROC*BIT_PER_WORD];

  void getFiberBitMasks();
  int BitCount(int arm, int trig, int start, int stop);
 
};

#endif



