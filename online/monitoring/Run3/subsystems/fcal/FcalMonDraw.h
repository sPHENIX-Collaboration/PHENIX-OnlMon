#ifndef __FCALMONDRAW_H__
#define __FCALMONDRAW_H__
//
//
// Written by:  Jane M. Burward-Hoy and Gerd J. Kunde
//
// Class:  FcalMonDraw
//
// Description:  This class creates the canvases, labels, titles to display
//               the histograms needed for the online monitoring of data.
//               The histograms are fetched from the Server, and if they 
//               exist, they are displayed.  The MakeHtml function by 
//               default will create all possible canvases/monitors in 
//               an html file (producing *.gif files in the process).  The
//               MakePS function also produces all possible monitors and 
//               creates postscript files of each.  The Draw() default option
//               will only create those histograms used for the Shift Crew
//               during the online monitoring of each run.
//               
//
// Date:  initially created for the d-Au run during January 2003
//
#include <OnlMonDraw.h>
#include <FclConsts.h>

#include <TText.h>

#include <string>

class TCanvas;
class TH1;
class TH2;
class TLine;
class TPad;
class TPaveText;
class TStyle;
class TBox;

const int FCALMONMAX = 2; // total number of possible canvases

class FcalMonDraw: public OnlMonDraw
{

 public: 
  FcalMonDraw(const char *name = "FCALMON");
  virtual ~FcalMonDraw();

  int Draw(const char *what = "SHIFT");
  int DrawMonitor(const char *name);
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");
  int drawDeadChannels(int compass);

 protected:

  int MakeCanvas(const char *name);

  int DrawDeadServer(TPad *transparent);

  // give it the monitor name, monitor index, divisions in x and y:  e.g., 2x2, 1x2, 9x10, etc
  int InitCanvas(const char *name, int monindex, const int &x, const int &y);

  // our array of monitors
  const char *mon[FCALMONMAX];

  // our array of monitor titles
  const char *montitle[FCALMONMAX];

  // our array of canvases, labels, pads, titles for each monitor
  TCanvas *TC[FCALMONMAX];
  TPad *Lab[FCALMONMAX];
  TPad *Pad[FCALMONMAX];
  TText Label[FCALMONMAX];
  TPaveText *Title[FCALMONMAX];
  TPad *transparent[FCALMONMAX];
  TText *deadText;
  TBox *deadBox;

  TStyle *fcalStyle;

};

#endif /*__FCALMONDRAW_H__ */

