#include <errorHist.h>
#include <muiMonGlobals.h>
#include <OnlMonServer.h>
#include <TH1.h>

using namespace std;

errorHist::errorHist(OnlMon *myparent): obj_hist(myparent)
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  rocOkErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SrocOkErrHist", "South ROC OK Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,rocOkErrHist[MUIMONCOORD::SOUTH]);
  rocOkErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NrocOkErrHist", "North ROC OK Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,rocOkErrHist[MUIMONCOORD::NORTH]);

  beamOkErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SbeamOkErrHist", "South Beam OK Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,beamOkErrHist[MUIMONCOORD::SOUTH]);
  beamOkErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NbeamOkErrHist", "North Beam OK Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,beamOkErrHist[MUIMONCOORD::NORTH]);

  noPacketErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SnoPacketErrHist", "South No Packet Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,noPacketErrHist[MUIMONCOORD::SOUTH]);
  noPacketErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NnoPacketErrHist", "North No Packet Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,noPacketErrHist[MUIMONCOORD::NORTH]);

  deadHvChainErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SdeadHvChainErrHist", "South Dead HV Chain Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadHvChainErrHist[MUIMONCOORD::SOUTH]);
  deadHvChainErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NdeadHvChainErrHist", "North Dead HV Chain Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadHvChainErrHist[MUIMONCOORD::NORTH]);

  deadRocErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SdeadRocErrHist", "South Dead ROC Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadRocErrHist[MUIMONCOORD::SOUTH]);
  deadRocErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NdeadRocErrHist", "North Dead ROC Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadRocErrHist[MUIMONCOORD::NORTH]);

  deadChipErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SdeadChipErrHist", "South Dead Chip Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadChipErrHist[MUIMONCOORD::SOUTH]);
  deadChipErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NdeadChipErrHist", "North Dead Chip Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadChipErrHist[MUIMONCOORD::NORTH]);

  mainFrameErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SmainFrameErrHist", "South Dead Main Frame Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,mainFrameErrHist[MUIMONCOORD::SOUTH]);
  mainFrameErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NmainFrameErrHist", "North Dead MaSOUTHin Frame Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,mainFrameErrHist[MUIMONCOORD::NORTH]);

  deadCableErrHist[MUIMONCOORD::SOUTH] = new TH1F("MUI_SdeadCableErrHist", "South Dead Cable Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadCableErrHist[MUIMONCOORD::SOUTH]);
  deadCableErrHist[MUIMONCOORD::NORTH] = new TH1F("MUI_NdeadCableErrHist", "North Dead Cable Errors", 1, -0.5, 0.5);
  Onlmonserver->registerHisto(parent,deadCableErrHist[MUIMONCOORD::NORTH]);

}


void errorHist::event(hit_vector rawHits)
{
  muiMonGlobals* globals = muiMonGlobals::Instance();

  //  clear(); // not needed when we use SetBinContent instead of Fill..
  for (int i = 0;i < 2;i++)
    {
      rocOkErrHist[i]->SetBinContent(1, globals->rocOkErrors[i]);
      beamOkErrHist[i]->SetBinContent(1, globals->beamOkErrors[i]);
      noPacketErrHist[i]->SetBinContent(1, globals->noPacketErrors[i]);
      deadHvChainErrHist[i]->SetBinContent(1, globals->deadHvChainErrors[i]);
      deadRocErrHist[i]->SetBinContent(1, globals->deadRocErrors[i]);
      deadChipErrHist[i]->SetBinContent(1, globals->deadChipErrors[i]);
      mainFrameErrHist[i]->SetBinContent(1, globals->mainFrameErrors[i]);
      deadCableErrHist[i]->SetBinContent(1, globals->deadCableErrors[i]);
    }
}

void errorHist::clear()
{
  for (int i = 0;i < 2;i++)
    {
      rocOkErrHist[i]->Reset();
      beamOkErrHist[i]->Reset();
      noPacketErrHist[i]->Reset();
      deadHvChainErrHist[i]->Reset();
      deadRocErrHist[i]->Reset();
      deadChipErrHist[i]->Reset();
      mainFrameErrHist[i]->Reset();
      deadCableErrHist[i]->Reset();
    }
}

void errorHist::draw()
{
}



