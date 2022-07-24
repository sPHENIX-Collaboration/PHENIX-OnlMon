#ifndef VTXSMONDRAW_H__
#define VTXSMONDRAW_H__

#include "GranMonDraw.h"
#include "DaqVtxsMonDefs.h"

class TCanvas;
class TH1;
class TPad;

class VtxsMonDraw: public GranMonDraw
{
 public:
   VtxsMonDraw(const char * granule);
   virtual ~VtxsMonDraw() {}
   virtual void identify(std::ostream& out = std::cout) const;
   virtual int MakeHtml(const std::string &what);

 protected:
   int DrawThisSubsystem(const char *what);
   int DrawRccBclk(const char *what);
   int DrawInvalidCellId(const int i_rcc, const char *what);
   int DrawBadCellId(const int i_rcc, const char *what);
   int DrawHybrid(const char *what);
   int MakeCanvas(std::string const &name, const int i=0);
   int ExtractMaskedChannels();
   TCanvas *TC[8];
   TPad *transparent[8];
   TPad *Pad0[VTXSPLOT1ROWS*VTXSPLOT1COLS];
   TPad *Pad1[VTXSPLOTHYBRIDROWS*VTXSPLOTHYBRIDCOLS];
   TPad *Pad2[3][VTXSPLOTCELLIDROWS*VTXSPLOTCELLIDCOLS];
   TPad *Pad3[3][VTXSPLOTCELLIDROWS*VTXSPLOTCELLIDCOLS];
   int rccmasked[40][6];
   int hybridmask[40][6][4];
   int currentrun;
   int suppress_masked;
};


#endif
