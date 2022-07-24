#ifndef __RICHMONDRAW_H__
#define __RICHMONDRAW_H__

#include <OnlMonDraw.h>

class TCanvas;
class TH1;
class TPad;
class TPaveText;
class TStyle;
class TBox;
class TLegend;
class TText;
class TLine;

class RICHMonDraw: public OnlMonDraw
{

 public: 
  RICHMonDraw();
  virtual ~RICHMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvas(const char *name);
  int chk_adcsum(TH1* h_adcsum);
  int chk_adcsum0sup(TH1* h_adcsum);

  int chk_tdcsum(TH1* h_tacsum);
  int DrawDeadServer(TPad *transparent);
  TPad *Pad[4];
  TPad *subPad[4][4];
  TPad *Padt[4];
  TPad *subPadt[4][4];
  TPaveText *msg1,*msg2,*msg3,*msg4,*msg5;
  TPaveText *msg_tdc1,*msg_tdc2,*msg_tdc3,*msg_tdc4,*msg_tdc5;
  TH1 *rich_fadclivehist;
  TBox *box_rich_tdc[4];
  TPad *transparent[2];
  TCanvas *TC[2];
  TPad *Pad2[4];
  TPad *subPad2[4][4];
  TLine *l[4];
  TLegend *tdclegend[4];
  TText *tdctext[4];
  
  


  TStyle *rs;
};

#endif /*__RICHMONDRAW_H__ */
