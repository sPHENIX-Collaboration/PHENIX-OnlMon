#ifndef __MUTRMONDRAW_H__
#define __MUTRMONDRAW_H__

#include <MutrMonConsts.h>
#include <MutrMonDrawConsts.h>
#include <MutOnlineChannel.hh>
#include <OnlMonDraw.h>

#include <iostream> 
#include <set> // for the static bad FEMs list


class TCanvas;
class TLatex;
class TLine;
class TPad;
class TPaveText;
class TH1;

class MutOnl
{ 
 public:
  MutOnl() { id = 0; val = 0; err = 0; }
  MutOnl(const int i, const float v, const float e) 
    { id = i; val = v; err = e; }
  virtual ~MutOnl(){}
  void write(std::ostream& os) const 
    { os << id << ": " << val << " pm " << err << " "; }

  int id;
  float val;
  float err;
}; 

class MutrMonDraw: public OnlMonDraw
{

 public: 
  MutrMonDraw();
  virtual ~MutrMonDraw();

  int Init();
  void registerHists();
  int Draw(const char *what = "ALL");

  int createCanvas(const int arm = 0);
  int createPads(const int arm = 0);
  int clearPads(const int arm = 0);
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 private:
  int MakeLocalHistos(int);
  int getBadFems(const char *filename = "mutr_knownbadfems.dat");

  TCanvas *mutCanv[NARMS];
  TPad *mutPad[NARMS][NPADSPERARM];
  TH1 *h1ZSRatioStrips[NARMS];
  TH1 *h1MultEst[NARMS];
  std::set <int> badfemset;

  TPaveText *pt[NARMS*NPAVETEX];
  TLine *tbar[2];
  TLatex *tltex[3];
  TH1 *mutActivePkt[NARMS];
};

#endif /*__MUTRMONDRAW_H__ */
