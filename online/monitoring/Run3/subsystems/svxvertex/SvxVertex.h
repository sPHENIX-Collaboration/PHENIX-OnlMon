#ifndef __SVXVERTEX_H__
#define __SVXVERTEX_H__

#include <SubsysReco.h>
#include <SvxVertexDefs.h>
#include <vector>

class TH1F;
class TH2F;

class SvxVertex: public SubsysReco
{
 public:
  SvxVertex(const std::string &name = "SVXVERTEX");
  virtual ~SvxVertex();
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode) {return 0;}
  int BeginRun(const int runno);
  int process_event(PHCompositeNode *topNode);
  int Reset(PHCompositeNode *topNode) {return 0;}

 protected:

  TH1F *h_bbc_z;
  TH1F *h_vtx_z;
  TH2F *h_vtx_xy;
  TH1F *h_bbcz_minus_vtxz; 
  TH2F *h_bbcz_vs_vtxz; 
};

#endif
