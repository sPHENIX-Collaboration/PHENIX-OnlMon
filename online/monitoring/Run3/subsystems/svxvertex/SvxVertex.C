#include "SvxVertex.h"

#include <OnlMonServer.h>

#include <BbcOut.h>

#include <Event.h>
#include <packet.h>
#include <PHCentralTrack.h>
#include <Bbc.hh>
#include <BbcOut.h>
#include <DchTrack.h>
#include <SvxRawhitList.h>
#include <SvxCluster.h>
#include <SvxClusterList.h>
#include <SvxSegmentList.h>
#include <SvxRawhitClusterList.h>
#include <SvxClusterContainer.h>
#include "VtxOut.h"

#include <getClass.h>

#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>

#include <Fun4AllReturnCodes.h>

#include <sstream>

#include <gsl/gsl_math.h>

using namespace std;
using namespace findNode;

SvxVertex::SvxVertex(const string &name): SubsysReco(name)
{
  return;
}

SvxVertex::~SvxVertex()
{
  return;
}

int SvxVertex::Init(PHCompositeNode *topNode)
{
  OnlMonServer *se = OnlMonServer::instance();

  ostringstream name;
  h_bbc_z = new TH1F("h_bbc_z", "bbc_z", 600, -30, 30); //bbc vertex;
  h_bbc_z->GetXaxis()->SetTitle("zBBC [cm]");
  h_bbc_z->GetYaxis()->SetTitle("counts");
  se->registerHisto("SVXVERTEXMON", h_bbc_z->GetName(), h_bbc_z);

  h_vtx_z = new TH1F("h_vtx_z", "vtx_z", 600, -30, 30); //vtx vertex;
  h_vtx_z->GetXaxis()->SetTitle("zVTX [cm]");
  h_vtx_z->GetYaxis()->SetTitle("counts");
  se->registerHisto("SVXVERTEXMON", h_vtx_z->GetName(), h_vtx_z);

  h_vtx_xy = new TH2F("h_vtx_xy", "vtx_xy", 200, -1.0, 1.0, 200, -1.0, 1.0); //vtx vertex x vs y;
  h_vtx_xy->GetXaxis()->SetTitle("xVTX [cm]");
  h_vtx_xy->GetYaxis()->SetTitle("yVTX [cm]");
  se->registerHisto("SVXVERTEXMON", h_vtx_xy->GetName(), h_vtx_xy);

  h_bbcz_minus_vtxz = new TH1F("h_bbcz_minus_vtxz", "bbcz_minus_vtxz", 200, -10, 10);
  h_bbcz_minus_vtxz->GetXaxis()->SetTitle("zBBC - zVTX [cm]");
  se->registerHisto("SVXVERTEXMON", h_bbcz_minus_vtxz->GetName(), h_bbcz_minus_vtxz);

  h_bbcz_vs_vtxz = new TH2F("h_bbcz_vs_vtxz", "bbcz_vs_vtxz", 600, -30, 30, 600, -30, 30);
  h_bbcz_vs_vtxz->GetXaxis()->SetTitle("zVTX [cm]");
  h_bbcz_vs_vtxz->GetYaxis()->SetTitle("zBBC [cm]");
  se->registerHisto("SVXVERTEXMON", h_bbcz_vs_vtxz->GetName(), h_bbcz_vs_vtxz);

  return 0;
}

int SvxVertex::BeginRun(const int runno)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  return 0;
}

int SvxVertex::process_event(PHCompositeNode *topNode)
{

  //BBC
  BbcOut *bbcout = getClass<BbcOut>(topNode,"BbcOut");

  if( bbcout == 0 )
    {
      cerr << "Error loading BbcOut node : " << bbcout << endl;
      return EVENT_OK;
    }

  // bbc zvertex
  float bbcz = bbcout->get_VertexPoint();

  //VTX
  VtxOut *vtxout = getClass<VtxOut>(topNode,"VtxOut");

  if( vtxout == 0 )
    {
      cerr << "Error loading vtxOut node : " << vtxout << endl;
      return EVENT_OK;
    }

  PHPoint vtxp_pos(-9999.0, -9999.0, -9999.0);
  vtxp_pos = vtxout->get_Vertex("SVX_PRECISE");
    
  //reject the nan values in vtxp_pos
  if(vtxp_pos.getZ() != vtxp_pos.getZ()) return 0;

  h_bbc_z          ->Fill(bbcz);
  h_vtx_z          ->Fill(vtxp_pos.getZ());
  h_vtx_xy         ->Fill(vtxp_pos.getX(),vtxp_pos.getY());
  h_bbcz_minus_vtxz->Fill(bbcz - vtxp_pos.getZ());
  h_bbcz_vs_vtxz   ->Fill(vtxp_pos.getZ(),bbcz);

  return 0;
}
