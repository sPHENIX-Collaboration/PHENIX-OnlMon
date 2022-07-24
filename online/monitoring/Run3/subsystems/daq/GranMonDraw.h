#ifndef __GRANMONDRAW_H__
#define __GRANMONDRAW_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>

class TH1;

struct badpacket
{
  int badbin;
  int savechannel;
  int packetbin;
  int savegood;
};

class GranMonDraw
{

 public: 
  GranMonDraw(const char * granule);
  virtual ~GranMonDraw();

  int Init();
  int Draw(const char *what = "ALL");
  const char *Name() const
    {
      return ThisName.c_str();
    }
  virtual void identify(std::ostream& out = std::cout) const;
  void HtmlOut(const int ival) {htmloutflag = ival;}
  virtual int SetBadPacket(const int packetid, const int ibin);
  virtual int SetBadPacket(const int packetid, const std::string &what);
  virtual int Print(const char *what = "ALL") const;
  virtual int MakeHtml(const std::string &what) {return 0;}
  virtual int DrawThisSubsystem(const char *what) {return 0;}

 protected:
  int DrawThisGranule(const char *what);
  int DrawAll(const char *what);
  void SetHtmlColor(std::ostream &out, int error) const;
  void EndFont(std::ostream &out) const;
  int PutBackKnownBad(TH1 *monhis, const int currentbin);
  int RemoveKnownBad(TH1 *monhis, TH1 *monpkt, const int currentbin);
  TH1 *MakeSnglHisto(const std::string &name, const int packetmin, const int packetmax);
  std::string ThisName;
  std::map< int,std::vector<badpacket> > badpackets;
  int htmloutflag;
};

#endif /*__GRANMONDRAW_H__ */
