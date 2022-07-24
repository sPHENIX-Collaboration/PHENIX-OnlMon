#ifndef VTXPMONDRAW_H__
#define VTXPMONDRAW_H__

#include "GranMonDraw.h"
#include "DaqVtxpMonDefs.h"

class TCanvas;
class TH1;
class TPad;

class VtxpMonDraw: public GranMonDraw
{
 public:
   VtxpMonDraw(const char * granule);
   virtual ~VtxpMonDraw() {}
   virtual void identify(std::ostream& out = std::cout) const;
   virtual int MakeHtml(const std::string &what);
   virtual int SetBadPacket(const int packetid, const std::string &what);
   virtual int SetVtxpBadPacket(const int packetid, const int ibin);
   virtual int Print(const char *what = "ALL") const;

 protected:
   int PutBackVtxpKnownBad(TH1 *monhis);
   int RemoveVtxpKnownBad(TH1 *monhis, TH1 *monpkt);
   int DrawThisSubsystem(const char *what);
   int MakeCanvas(std::string const &name);
   TCanvas *TC1;
   TPad *transparent1;
   TPad *Pad1[(VTXPLASTBIN/2+(VTXPLASTBIN&0x1))*2];
   std::map< int,std::vector<badpacket> > vtxpbadpackets;
   
};


#endif
