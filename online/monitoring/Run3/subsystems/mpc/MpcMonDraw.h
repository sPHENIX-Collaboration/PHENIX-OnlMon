#ifndef __MpcMONDRAW_H__
#define __MpcMONDRAW_H__

#include <OnlMonDraw.h>

#include <string>

class OnlMonDB;
class MpcMap;
class TCanvas;
class TH1;
class TPad;
class TLegend;
class TStyle;
//class TLine;
class string;
#include <TLine.h>

static const int NCANVAS = 5;	
static const int MAXPADS = 8;

class MpcMonDraw: public OnlMonDraw
{

 public: 
  MpcMonDraw(const char *name = "MPCMON");
  virtual ~MpcMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  void ParseConfig(const char *fname);
  int MakeCanvas(const char *name);
  int DrawFirst(const char *what = "ALL");
  int DrawSecond(const char *what = "ALL");
  int DrawThird(const char *what = "ALL");
  int DrawFourth(const char *what = "ALL");
  int DrawFifth(const char *what = "ALL");
  int DrawDeadServer(TPad *transparent);
  int DrawHighVoltageOff(TPad *transparent);
  int DrawEnergyInstructions(TPad *transparent);
  int DrawTimingInstructions(TPad *transparent);
  int DrawGridInstructions(TPad *transparent, double nled);
  int DrawVoltageTooLow(TPad *transparent);
  int DrawVoltageTooHigh(TPad *transparent);
  int DrawBadReducedBits(TPad *transparent);
  int DrawStatusMessage(TPad *msg);

  bool isServerDead;
  bool isVoltageTooLow;
  bool isVoltageTooHigh;

  float fracVoltageTooLow;
  float fracVoltageTooHigh;
  float fracVoltage;

  unsigned int BBCTRIG0_scaledown;
  unsigned int BBCTRIG1_scaledown;
  unsigned int BBCTRIG2_scaledown;
  unsigned int LEDTRIG_scaledown;
  unsigned int MPCTRIG0_scaledown;
  unsigned int MPCTRIG1_scaledown;
  unsigned int MPC_TRIGANY_scaledown;

  char BBCTRIG0_name[200];
  char BBCTRIG1_name[200];
  char BBCTRIG2_name[200];
  char LEDTRIG_name[200];
  char MPC_TRIG0_name[200];
  char MPC_TRIG1_name[200];
  char MPC_TRIG2_name[200];
  char MPC_TRIGANY_name[200];

  static const int BBCTRIG0 = 1; //"BBCLL1(>0 tubes)"
  static const int BBCTRIG1 = 2; //"BBCLL1(>0 tubes) novertex"
  static const int BBCTRIG2 = 3; //"BBCLL1(narrow)"
  static const int LEDTRIG  = 4; //"PPG(Laser)"
  static const int MPC_TRIG0 = 5; //"MPC4x4a"
  static const int MPC_TRIG1 = 6; //"MPC4x4b
  static const int MPC_TRIG2 = 7; //"MPC4x4c&ert"
  static const int MPC_TRIGANY = 8;

  
  int TimeOffsetTicks;

  MpcMap *mpcmap;

  std::string species;
  TH1 *lines;		// Range of vertex mean
  TStyle  *mpcStyle;
  TLine line1;
  TLine line2;

  TCanvas *TC[NCANVAS];
  TPad *transparent[NCANVAS];

  TPad *Pad[NCANVAS][MAXPADS];
  TLegend *Leg[NCANVAS][MAXPADS];
  int GridToChan[2][18][18];

  int southcut;       //how many south led have to be out of range before drawing error message
  int northcut;       //how many north led have to be out of range before drawing error message
  float sref[19][19]; //hold refference led values for south
  float nref[19][19]; //hold refference led values for north
  int slow;           //number of low towers in south
  int shigh;          //number of high towers in south
  int nlow;           //number of low towers in north
  int nhigh;          //number of high towers in north
  int highindex[576]; //channel number that is high
  int lowindex[576];  //channel number that is low
  float highval[576]; //value of a high channel
  float lowval[576];  //value of a low channel
};

#endif /*__MpcMONDRAW_H__ */
