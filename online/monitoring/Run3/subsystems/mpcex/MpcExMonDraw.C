#include "MpcExMonDraw.h"
#include "MpcExConst.h"
#include <OnlMonClient.h>
#include <OnlMonDB.h>
#include <Exogram.h>
#include <TH3.h>
#include <TH2.h>
#include <MpcExConstants.h>
#include <MpcExMapper.h>

#include <phool.h>
#include <PHTimeStamp.h>
#include <RunToTime.hh>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TH1.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TStyle.h>
#include <TColor.h>
#include <TExec.h>
#include <TString.h>

#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <cstdlib>
#include <math.h>

#include <odbc++/connection.h>
#include <odbc++/setup.h>
#include <odbc++/types.h>
#include <odbc++/errorhandler.h>
#include <sql.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>
#include <odbc++/resultsetmetadata.h>
#include <odbc++/preparedstatement.h>
#include <odbc++/databasemetadata.h>

using namespace odbc;
using namespace std;

MpcExMonDraw::MpcExMonDraw(const char *name):
  OnlMonDraw(name),
  fCurrents(NULL),
  fTemperatures(NULL)
{
  // memset can be used to set an array to zero
  // without knowing its size (works on bytes, init to 1 won't work,
  // init for floatss only works for 0)
  memset(mpcexTC,0,sizeof(mpcexTC));
  memset(mpcexTP,0,sizeof(mpcexTP));
  memset(fChnErr,0,sizeof(fChnErr));
}

MpcExMonDraw::~MpcExMonDraw() {
  if(fCurrents) delete fCurrents;
  if(fTemperatures) delete fTemperatures;
}

int MpcExMonDraw::Init() {
  return 0;
}

int MpcExMonDraw::MakeCanvas(const char *name) {
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  gROOT->SetStyle("Plain");
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_0") ) {
    mpcexTC[0] = new TCanvas("mpcexTC0","PacketScan0", -1, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[0] = new TPad("mpcexTP0","pad0", 0.01, 0.51, 0.49, 0.90, 0);
    mpcexTP[1] = new TPad("mpcexTP1","pad1", 0.51, 0.51, 0.99, 0.90, 0);
    mpcexTP[2] = new TPad("mpcexTP2","pad2", 0.01, 0.11, 0.49, 0.50, 0);
    mpcexTP[3] = new TPad("mpcexTP3","pad3", 0.51, 0.11, 0.99, 0.50, 0);
    mpcexTP[4] = new TPad("mpcexTP4","pad4", 0.01, 0.01, 0.33, 0.10, 0);
    mpcexTP[5] = new TPad("mpcexTP5","pad5", 0.34, 0.01, 0.66, 0.10, 0);
    mpcexTP[6] = new TPad("mpcexTP6","pad6", 0.67, 0.01, 0.99, 0.10, 0);
    mpcexTP[0]->Draw();
    mpcexTP[1]->Draw();
    mpcexTP[2]->Draw();
    mpcexTP[3]->Draw();
    mpcexTP[4]->Draw();
    mpcexTP[5]->Draw();
    mpcexTP[6]->Draw();
    mpcexTC[0]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_1") ) {
    mpcexTC[1] = new TCanvas("mpcexTC1","PacketScan1", xsize/2, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[7] = new TPad("mpcexTP7","pad7", 0.01, 0.51, 0.49, 0.90, 0);
    mpcexTP[8] = new TPad("mpcexTP8","pap8", 0.51, 0.51, 0.99, 0.90, 0);
    mpcexTP[9] = new TPad("mpcexTP9","pad9", 0.01, 0.11, 0.49, 0.50, 0);
    mpcexTP[10] = new TPad("mpcexTP10","pad10", 0.51, 0.11, 0.99, 0.50, 0);
    mpcexTP[11] = new TPad("mpcexTP11","pad11", 0.01, 0.01, 0.33, 0.10, 0);
    mpcexTP[12] = new TPad("mpcexTP12","pad12", 0.34, 0.01, 0.66, 0.10, 0);
    mpcexTP[13] = new TPad("mpcexTP13","pad13", 0.67, 0.01, 0.99, 0.10, 0);
    mpcexTP[7]->Draw();
    mpcexTP[8]->Draw();
    mpcexTP[9]->Draw();
    mpcexTP[10]->Draw();
    mpcexTP[11]->Draw();
    mpcexTP[12]->Draw();
    mpcexTP[13]->Draw();
    mpcexTC[1]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_2") ) {
    mpcexTC[2] = new TCanvas("mpcexTC2","HealthMonitor0", -1, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[14] = new TPad("mpcexTP14","pad14", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[15] = new TPad("mpcexTP15","pad15", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[16] = new TPad("mpcexTP16","pad16", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[17] = new TPad("mpcexTP17","pad17", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[18] = new TPad("mpcexTP18","pad18", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[19] = new TPad("mpcexTP19","pad19", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[14]->Draw();
    mpcexTP[15]->Draw();
    mpcexTP[16]->Draw();
    mpcexTP[17]->Draw();
    mpcexTP[18]->Draw();
    mpcexTP[19]->Draw();
    mpcexTC[2]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_3") ) {
    mpcexTC[3] = new TCanvas("mpcexTC3","HealthMonitor1", xsize/2, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[20] = new TPad("mpcexTP20","pad20", 0.01, 0.61, 0.99, 0.90, 0);
    //mpcexTP[20] = new TPad("mpcexTP20","pad20", 0.01, 0.61, 0.49, 0.90, 0);
    //mpcexTP[21] = new TPad("mpcexTP21","pad21", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[22] = new TPad("mpcexTP22","pad22", 0.01, 0.31, 0.99, 0.60, 0);
    //mpcexTP[22] = new TPad("mpcexTP22","pad22", 0.01, 0.31, 0.49, 0.60, 0);
    //mpcexTP[23] = new TPad("mpcexTP23","pad23", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[24] = new TPad("mpcexTP24","pad24", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[25] = new TPad("mpcexTP25","pad25", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[20]->Draw();
    //mpcexTP[21]->Draw();
    mpcexTP[22]->Draw();
    //mpcexTP[23]->Draw();
    mpcexTP[24]->Draw();
    mpcexTP[25]->Draw();
    mpcexTC[3]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_4") ) {
    mpcexTC[4] = new TCanvas("mpcexTC4","StateOfAffairs", 0, 0, xsize*0.8, ysize);
    gSystem->ProcessEvents();
    mpcexTC[4]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_5") ) {
    mpcexTC[5] = new TCanvas("mpcexTC5","EnergyMonitor0", -1, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[26] = new TPad("mpcexTP26","pad26", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[27] = new TPad("mpcexTP27","pad27", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[28] = new TPad("mpcexTP28","pad28", 0.01, 0.31, 0.99, 0.60, 0);
    //mpcexTP[29] = new TPad("mpcexTP29","pad29", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[30] = new TPad("mpcexTP30","pad30", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[31] = new TPad("mpcexTP31","pad31", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[26]->Draw();
    mpcexTP[27]->Draw();
    mpcexTP[28]->Draw();
    //mpcexTP[29]->Draw();
    mpcexTP[30]->Draw();
    mpcexTP[31]->Draw();
    mpcexTC[5]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_6") ) {
    mpcexTC[6] = new TCanvas("mpcexTC6","EnergyMonitor1", xsize/2, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[32] = new TPad("mpcexTP32","pad32", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[33] = new TPad("mpcexTP33","pad33", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[34] = new TPad("mpcexTP34","pad34", 0.01, 0.31, 0.99, 0.60, 0);
    //mpcexTP[35] = new TPad("mpcexTP35","pad35", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[36] = new TPad("mpcexTP36","pad36", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[37] = new TPad("mpcexTP37","pad37", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[32]->Draw();
    mpcexTP[33]->Draw();
    mpcexTP[34]->Draw();
    //mpcexTP[35]->Draw();
    mpcexTP[36]->Draw();
    mpcexTP[37]->Draw();
    mpcexTC[6]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_7") ) {
    mpcexTC[7] = new TCanvas("mpcexTC7","EnergyMonitor2", -1, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[38] = new TPad("mpcexTP38","pad38", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[39] = new TPad("mpcexTP39","pad39", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[40] = new TPad("mpcexTP40","pad40", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[41] = new TPad("mpcexTP41","pad41", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[42] = new TPad("mpcexTP42","pad42", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[43] = new TPad("mpcexTP43","pad43", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[38]->Draw();
    mpcexTP[39]->Draw();
    mpcexTP[40]->Draw();
    mpcexTP[41]->Draw();
    mpcexTP[42]->Draw();
    mpcexTP[43]->Draw();
    mpcexTC[7]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_8") ) {
    mpcexTC[8] = new TCanvas("mpcexTC8","EnergyMonitor3", xsize/2, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[44] = new TPad("mpcexTP44","pad44", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[45] = new TPad("mpcexTP45","pad45", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[46] = new TPad("mpcexTP46","pad46", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[47] = new TPad("mpcexTP47","pad47", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[48] = new TPad("mpcexTP48","pad48", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[49] = new TPad("mpcexTP49","pad49", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[44]->Draw();
    mpcexTP[45]->Draw();
    mpcexTP[46]->Draw();
    mpcexTP[47]->Draw();
    mpcexTP[48]->Draw();
    mpcexTP[49]->Draw();
    mpcexTC[8]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_9") ) {
    mpcexTC[9] = new TCanvas("mpcexTC9","EnergyMonitor4", -1, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[50] = new TPad("mpcexTP50","pad50", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[51] = new TPad("mpcexTP51","pad51", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[52] = new TPad("mpcexTP52","pad52", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[53] = new TPad("mpcexTP53","pad53", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[54] = new TPad("mpcexTP54","pad54", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[55] = new TPad("mpcexTP55","pad55", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[50]->Draw();
    mpcexTP[51]->Draw();
    mpcexTP[52]->Draw();
    mpcexTP[53]->Draw();
    mpcexTP[54]->Draw();
    mpcexTP[55]->Draw();
    mpcexTC[9]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_10") ) {
    mpcexTC[10] = new TCanvas("mpcexTC10","EnergyMonitor5", xsize/2, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[56] = new TPad("mpcexTP56","pad56", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[57] = new TPad("mpcexTP57","pad57", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[58] = new TPad("mpcexTP58","pad58", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[59] = new TPad("mpcexTP59","pad59", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[60] = new TPad("mpcexTP60","pad60", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[61] = new TPad("mpcexTP61","pad61", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[56]->Draw();
    mpcexTP[57]->Draw();
    mpcexTP[58]->Draw();
    mpcexTP[59]->Draw();
    mpcexTP[60]->Draw();
    mpcexTP[61]->Draw();
    mpcexTC[10]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_11") ) {
    mpcexTC[11] = new TCanvas("mpcexTC11","TriggerMonitor0", -1, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[62] = new TPad("mpcexTP62","pad62", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[63] = new TPad("mpcexTP63","pad63", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[64] = new TPad("mpcexTP64","pad64", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[65] = new TPad("mpcexTP65","pad65", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[66] = new TPad("mpcexTP66","pad66", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[67] = new TPad("mpcexTP67","pad67", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[62]->Draw();
    mpcexTP[63]->Draw();
    mpcexTP[64]->Draw();
    mpcexTP[65]->Draw();
    mpcexTP[66]->Draw();
    mpcexTP[67]->Draw();
    mpcexTC[11]->SetEditable(0);
  }
  if( !strcmp(name,"ALL") || !strcmp(name,"MPCEXMON_12") ) {
    mpcexTC[12] = new TCanvas("mpcexTC12","TriggerMonitor1", xsize/2, 0, xsize/2 , ysize);
    gSystem->ProcessEvents();
    mpcexTP[68] = new TPad("mpcexTP68","pad68", 0.01, 0.61, 0.49, 0.90, 0);
    mpcexTP[69] = new TPad("mpcexTP69","pad69", 0.51, 0.61, 0.99, 0.90, 0);
    mpcexTP[70] = new TPad("mpcexTP70","pad70", 0.01, 0.31, 0.49, 0.60, 0);
    mpcexTP[71] = new TPad("mpcexTP71","pad71", 0.51, 0.31, 0.99, 0.60, 0);
    mpcexTP[72] = new TPad("mpcexTP72","pad72", 0.01, 0.01, 0.49, 0.30, 0);
    mpcexTP[73] = new TPad("mpcexTP73","pad73", 0.51, 0.01, 0.99, 0.30, 0);
    mpcexTP[68]->Draw();
    mpcexTP[69]->Draw();
    mpcexTP[70]->Draw();
    mpcexTP[71]->Draw();
    mpcexTP[72]->Draw();
    mpcexTP[73]->Draw();
    mpcexTC[12]->SetEditable(0);
  }
  return 0;
}

int MpcExMonDraw::Draw(const char *what) {
  int iret = 0;
  int idraw = 0;

  if (!strcmp(what, "ALL") || !strcmp(what, "RAW")) { // 0 1
    iret += DrawRaw();
    idraw++;
  }
  if (!strcmp(what, "ALL") || !strcmp(what, "SEN")) { // 2 3
    iret += DrawSen();
    idraw++;
  }
  if (!strcmp(what, "ALL") || !strcmp(what, "REV")) { // 4
    iret += DrawRev();
    idraw++;
  }
  if (!strcmp(what, "ALL") || !strcmp(what, "ENE")) { // 5 6
    iret += DrawEne();
    idraw++;
  }
  if (!strcmp(what, "ALL") || !strcmp(what, "ENE2")) { // 7 8
    iret += DrawEne2();
    idraw++;
  }
  if (!strcmp(what, "ALL") || !strcmp(what, "ENE3")) { // 9 10
    iret += DrawEne3();
    idraw++;
  }
  if (!strcmp(what, "ALL") || !strcmp(what, "TRG")) { // 11 12
    iret += DrawTrg();
    idraw++;
  }
  if (!idraw) {
    cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
    iret = -1;
  }
  return iret;
}

void MpcExMonDraw::WriteCenter(TString title) {
  TLatex tex;
  tex.SetTextSize(0.02);
  tex.DrawTextNDC(0.5,0.2,title.Data());
}

void MpcExMonDraw::DrawStamp(TString title) {
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *hevt = cl->getHisto("mpcex_events");
  float evts = hevt->GetBinContent(1)/1000;
  time_t evttime = cl->EventTime("CURRENT");
  TLatex tex;
  tex.SetTextSize(0.04);
  tex.DrawTextNDC(0.03,0.96,title.Data());
  tex.DrawTextNDC(0.03,0.92,Form("Run %d; %s; %.1f kEvents",cl->RunNumber(),ctime(&evttime),evts));
}

int MpcExMonDraw::DrawRaw() { // 0 1
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *hptis = cl->getHisto("mpcex_partimeS");
  TH1 *hptin = cl->getHisto("mpcex_partimeN");
  TH1 *hstks = cl->getHisto("mpcex_stackS");
  TH1 *hstkn = cl->getHisto("mpcex_stackN");
  TH1 *hsphs = cl->getHisto("mpcex_statephS");
  TH1 *hsphn = cl->getHisto("mpcex_statephN");
  TH1 *hlengths = cl->getHisto("mpcex_lengthS");
  TH1 *hlengthn = cl->getHisto("mpcex_lengthN");
  TH1 *hgcods = cl->getHisto("mpcex_graycodeS");
  TH1 *hgcodn = cl->getHisto("mpcex_graycodeN");
  TH1 *hfemts = cl->getHisto("mpcex_femtestS");
  TH1 *hfemtn = cl->getHisto("mpcex_femtestN");
  TH1 *hcbts = cl->getHisto("mpcex_cbtestS");
  TH1 *hcbtn = cl->getHisto("mpcex_cbtestN");
  //====================================
  if( !gROOT->FindObject("MPCEXMON_0") ) MakeCanvas("MPCEXMON_0");
  mpcexTC[0]->SetEditable(1);
  mpcexTC[0]->Clear("D");
  //======
  mpcexTP[0]->cd()->SetLogz(1);
  if(hlengths) hlengths->DrawCopy("colz"); else return -1;
  mpcexTP[1]->cd()->SetLogz(1);
  if(hptis) hptis->DrawCopy("colz"); else return -1;
  mpcexTP[2]->cd()->SetLogz(1);
  if(hsphs) hsphs->DrawCopy("colz"); else return -1;
  mpcexTP[3]->cd();
  if(hstks) hstks->DrawCopy("colz"); else return -1;
  //======
  mpcexTP[4]->cd();
  if(hgcods) hgcods->DrawCopy("colz"); else return -1;
  mpcexTP[5]->cd();
  if(hfemts) hfemts->DrawCopy("colz"); else return -1;
  mpcexTP[6]->cd();
  if(hcbts) hcbts->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[0]->cd();
  DrawStamp("MPCEX Packet Scan in South");
  mpcexTC[0]->Update();
  mpcexTC[0]->Show();
  mpcexTC[0]->SetEditable(0);
  //====================================
  if( !gROOT->FindObject("MPCEXMON_1") ) MakeCanvas("MPCEXMON_1");
  mpcexTC[1]->SetEditable(1);
  mpcexTC[1]->Clear("D");
  mpcexTP[7]->cd()->SetLogz(1);
  if(hlengthn) hlengthn->DrawCopy("colz"); else return -1;
  mpcexTP[8]->cd()->SetLogz(1);
  if(hptin) hptin->DrawCopy("colz"); else return -1;
  mpcexTP[9]->cd()->SetLogz(1);
  if(hsphn) hsphn->DrawCopy("colz"); else return -1;
  mpcexTP[10]->cd();
  if(hstkn) hstkn->DrawCopy("colz"); else return -1;
  //======
  mpcexTP[11]->cd();
  if(hgcodn) hgcodn->DrawCopy("colz"); else return -1;
  mpcexTP[12]->cd();
  if(hfemtn) hfemtn->DrawCopy("colz"); else return -1;
  mpcexTP[13]->cd();
  if(hcbtn) hcbtn->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[1]->cd();
  DrawStamp("MPCEX Packet Scan in North");
  mpcexTC[1]->Update();
  mpcexTC[1]->Show();
  mpcexTC[1]->SetEditable(0);
  return 0;
}

int MpcExMonDraw::DrawSen() { // 2 3
  OnlMonClient *cl = OnlMonClient::instance();
  if( !gROOT->FindObject("MPCEXMON_2") ) MakeCanvas("MPCEXMON_2");
  TH1 *hpti = cl->getHisto("mpcex_partime0");
  TH1 *hptiSN = cl->getHisto("mpcex_partimeSN");
  TH1 *hsph = cl->getHisto("mpcex_stateph0");
  TH1 *hstk = cl->getHisto("mpcex_stack0");
  TH1 *hbck = cl->getHisto("mpcex_bclockSN");
  TH1 *hcid = cl->getHisto("mpcex_cidmap");
  TH1 *pcid = cl->getHisto("mpcex_cidavg");
  TH1 *hcidsd = cl->getHisto("mpcex_cidSD");
  TH1 *hcidrg = cl->getHisto("mpcex_cidRG");
  mpcexTC[2]->SetEditable(1);
  mpcexTC[2]->Clear("D");
  //======
  mpcexTP[14]->cd();
  if(hbck) hbck->DrawCopy(); else return -1;
  mpcexTP[15]->cd();
  if(hstk) hstk->DrawCopy(); else return -1;
  mpcexTP[16]->cd();
  if(hpti) hpti->DrawCopy(); else return -1;
  mpcexTP[17]->cd();
  if(hptiSN) hptiSN->DrawCopy(); else return -1;
  mpcexTP[18]->cd()->SetLogy(1);
  if(hsph) hsph->DrawCopy(); else return -1;
  mpcexTP[19]->cd();
  ReadCurrents();
  if(fCurrents) fCurrents->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[2]->cd();
  DrawStamp("MPCEX Health Monitor 0");
  mpcexTC[2]->Update();
  mpcexTC[2]->Show();
  mpcexTC[2]->SetEditable(0);
  //====================================
  if( !gROOT->FindObject("MPCEXMON_2") ) MakeCanvas("MPCEXMON_3");
  mpcexTC[3]->SetEditable(1);
  mpcexTC[3]->Clear("D");
  mpcexTC[3]->cd();
  DrawStamp("MPCEX Health Monitor 1");
  //======
  TLine line;
  line.SetLineWidth(4);
  mpcexTP[20]->cd();
  if(hcid) {
    hcid->DrawCopy("colz");
    line.DrawLine(384,0.5,384,46.5);
  } else return -1;
  //mpcexTP[21]->cd();
  mpcexTP[22]->cd();
  if(pcid) {
    pcid->DrawCopy("hist p");
    line.DrawLine(384,0.5,384,46.5);
    line.SetLineWidth(1);
    line.SetLineColor( kOrange - 3 );
    line.DrawLine(-0.5,23.5,767.5,23.5);    
  } else return -1;
  //mpcexTP[23]->cd();
  mpcexTP[24]->cd();
  if(hcidsd) hcidsd->DrawCopy("colz"); else return -1;
  mpcexTP[25]->cd();
  if(hcidrg) hcidrg->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[3]->Update();
  mpcexTC[3]->Show();
  mpcexTC[3]->SetEditable(0);
  return 0;
}

int MpcExMonDraw::DrawEne() { // 5 6
  OnlMonClient *cl = OnlMonClient::instance();
  if( !gROOT->FindObject("MPCEXMON_5") ) MakeCanvas("MPCEXMON_5");
  TH1 *hhimaps = cl->getHisto("mpcex_himaps");
  TH1 *hhimapn = cl->getHisto("mpcex_himapn");
  TH1 *hlomaps = cl->getHisto("mpcex_lomaps");
  TH1 *hlomapn = cl->getHisto("mpcex_lomapn");
  TH1 *hpadchs = cl->getHisto("mpcex_parscanHS");
  TH1 *hpadcls = cl->getHisto("mpcex_parscanLS");
  TH1 *hpadchn = cl->getHisto("mpcex_parscanHN");
  TH1 *hpadcln = cl->getHisto("mpcex_parscanLN");
  TH1 *hlohis = cl->getHisto("mpcex_lohis");
  TH1 *hlohin = cl->getHisto("mpcex_lohin");
  mpcexTC[5]->SetEditable(1);
  mpcexTC[5]->Clear("D");
  //======
  mpcexTP[26]->cd()->SetLogz(1);
  if(hhimaps) hhimaps->DrawCopy("colz"); else return -1;
  mpcexTP[27]->cd()->SetLogz(1);
  if(hlomaps) hlomaps->DrawCopy("colz"); else return -1;
  mpcexTP[28]->cd()->SetLogz(1);
  if(hlohis) hlohis->DrawCopy("colz"); else return -1;
  mpcexTP[30]->cd()->SetLogz(1);
  if(hpadchs) hpadchs->DrawCopy("colz"); else return -1;
  mpcexTP[31]->cd()->SetLogz(1);
  if(hpadcls) hpadcls->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[5]->cd();
  DrawStamp("MPCEX Energy Monitor 0   [[ STK ]]");
  mpcexTC[5]->Update();
  mpcexTC[5]->Show();
  mpcexTC[5]->SetEditable(0);
  //====================================
  if( !gROOT->FindObject("MPCEXMON_6") ) MakeCanvas("MPCEXMON_6");
  mpcexTC[6]->SetEditable(1);
  mpcexTC[6]->Clear("D");
  //======
  mpcexTP[32]->cd()->SetLogz(1);
  if(hhimapn) hhimapn->DrawCopy("colz"); else return -1;
  mpcexTP[33]->cd()->SetLogz(1);
  if(hlomapn) hlomapn->DrawCopy("colz"); else return -1;
  mpcexTP[34]->cd()->SetLogz(1);
  if(hlohin) hlohin->DrawCopy("colz"); else return -1;
  mpcexTP[36]->cd()->SetLogz(1);
  if(hpadchn) hpadchn->DrawCopy("colz"); else return -1;
  mpcexTP[37]->cd()->SetLogz(1);
  if(hpadcln) hpadcln->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[6]->cd();
  DrawStamp("MPCEX Energy Monitor 1   [[ STK ]]");
  mpcexTC[6]->Update();
  mpcexTC[6]->Show();
  mpcexTC[6]->SetEditable(0);
  return 0;
}

int MpcExMonDraw::DrawEne2() { // 7 8
  OnlMonClient *cl = OnlMonClient::instance();
  if( !gROOT->FindObject("MPCEXMON_7") ) MakeCanvas("MPCEXMON_7");
  TH1 *hhimaps = cl->getHisto("mpcex_phimaps");
  if (!hhimaps)
    {
      return -1;
    }
  TH1 *hhimapn = cl->getHisto("mpcex_phimapn");
  TH1 *hlomaps = cl->getHisto("mpcex_plomaps");
  TH1 *hlomapn = cl->getHisto("mpcex_plomapn");
  TH1 *hmpcs = cl->getHisto("mpcex_mpcS");
  TH1 *hmpcn = cl->getHisto("mpcex_mpcN");
  TH1 *hmpcls = cl->getHisto("mpcex_mpclS");
  TH1 *hmpcln = cl->getHisto("mpcex_mpclN");
  TH1 *hmpctots = cl->getHisto("mpcex_mpcallS");
  TH1 *hmpctotn = cl->getHisto("mpcex_mpcallN");
  TH1 *hmpcerrs = cl->getHisto("mpcex_mpcerrS");
  TH1 *hmpcerrn = cl->getHisto("mpcex_mpcerrN");
  TH1 *hmpcrs = cl->getHisto("mpcex_mpcrS");
  TH1 *hmpcrn = cl->getHisto("mpcex_mpcrN");
  for(int i=0; i!=hmpctots->GetNbinsX(); ++i) {
    float mcs, mcn;
    mcs = hmpcerrs->GetBinContent(i+1);
    mcn = hmpctots->GetBinContent(i+1);
    if(mcn>0) hmpcrs->Fill(mcs/mcn);
    mcs = hmpcerrn->GetBinContent(i+1);
    mcn = hmpctotn->GetBinContent(i+1);
    if(mcn>0) hmpcrn->Fill(mcs/mcn);
  }
  mpcexTC[7]->SetEditable(1);
  mpcexTC[7]->Clear("D");
  //======
  mpcexTP[38]->cd()->SetLogz(1);
  if(hhimaps) hhimaps->DrawCopy("colz"); else return -1;
  mpcexTP[39]->cd()->SetLogz(1);
  if(hlomaps) hlomaps->DrawCopy("colz"); else return -1;
  mpcexTP[40]->cd()->SetLogz(1);
  if(hmpcs) hmpcs->DrawCopy("colz"); else return -1;
  mpcexTP[41]->cd()->SetLogz(1);
  if(hmpcls) hmpcls->DrawCopy("colz"); else return -1;
  mpcexTP[42]->cd()->SetLogy(1);
  if(hmpctots) hmpctots->DrawCopy(""); else return -1;
  if(hmpcerrs) hmpcerrs->DrawCopy("SAME"); else return -1;
  mpcexTP[43]->cd()->SetLogy(0);
  if(hmpcrs) hmpcrs->DrawCopy(); else return -1;
  //======
  mpcexTC[7]->cd();
  DrawStamp("MPCEX Energy Monitor 2   [[ STK PAR ]]");
  mpcexTC[7]->Update();
  mpcexTC[7]->Show();
  mpcexTC[7]->SetEditable(0);
  //====================================
  if( !gROOT->FindObject("MPCEXMON_8") ) MakeCanvas("MPCEXMON_8");
  mpcexTC[8]->SetEditable(1);
  mpcexTC[8]->Clear("D");
  //======
  mpcexTP[44]->cd()->SetLogz(1);
  if(hhimapn) hhimapn->DrawCopy("colz"); else return -1;
  mpcexTP[45]->cd()->SetLogz(1);
  if(hlomapn) hlomapn->DrawCopy("colz"); else return -1;
  mpcexTP[46]->cd()->SetLogz(1);
  if(hmpcn) hmpcn->DrawCopy("colz"); else return -1;
  mpcexTP[47]->cd()->SetLogz(1);
  if(hmpcln) hmpcln->DrawCopy("colz"); else return -1;
  mpcexTP[48]->cd()->SetLogy(1);
  if(hmpctotn) hmpctotn->DrawCopy(""); else return -1;
  if(hmpcerrn) hmpcerrn->DrawCopy("SAME"); else return -1;
  mpcexTP[49]->cd()->SetLogy(0);
  if(hmpcrn) hmpcrn->DrawCopy(); else return -1;
  //if(hhitsn) hhitsn->DrawCopy(); else return -1;
  //if(hlhitsn) hlhitsn->DrawCopy("SAME"); else return -1;
  //======
  mpcexTC[8]->cd();
  DrawStamp("MPCEX Energy Monitor 3   [[ STK PAR ]]");
  mpcexTC[8]->Update();
  mpcexTC[8]->Show();
  mpcexTC[8]->SetEditable(0);
  return 0;
}

int MpcExMonDraw::DrawEne3() { // 9 10
  OnlMonClient *cl = OnlMonClient::instance();
  if( !gROOT->FindObject("MPCEXMON_9") ) MakeCanvas("MPCEXMON_9");
  TH1 *hhitss = cl->getHisto("mpcex_hitsS");
  TH1 *hhitsn = cl->getHisto("mpcex_hitsN");
  TH1 *hlhitss = cl->getHisto("mpcex_lowhitsS");
  TH1 *hlhitsn = cl->getHisto("mpcex_lowhitsN");
  TH1 *hqhlrs = cl->getHisto("mpcex_qhlrS");
  TH1 *hqhlrn = cl->getHisto("mpcex_qhlrN");
  TH1 *hxinghitss = cl->getHisto("mpcex_xinghits");
  TH1 *hxinghitsn = cl->getHisto("mpcex_xinghitn");
  //TH1 *hqmips = cl->getHisto("mpcex_qmipS");
  //TH1 *hqmipn = cl->getHisto("mpcex_qmipN");
  TH1 *hlohis = cl->getHisto("mpcex_plohis");
  TH1 *hlohin = cl->getHisto("mpcex_plohin");
  mpcexTC[9]->SetEditable(1);
  mpcexTC[9]->Clear("D");
  //======
  mpcexTP[50]->cd()->SetLogy(1);
  if(hhitss) hhitss->DrawCopy(); else return -1;
  if(hlhitss) hlhitss->DrawCopy("SAME"); else return -1;
  mpcexTP[51]->cd()->SetLogz(1);
  if(hxinghitss) hxinghitss->DrawCopy("colz"); else return -1;
  mpcexTP[52]->cd()->SetLogz(1);
  if(hlohis) hlohis->DrawCopy("colz"); else return -1;
  mpcexTP[53]->cd()->SetLogz(1);
  if(hqhlrs) hqhlrs->DrawCopy("colz"); else return -1;
  mpcexTP[54]->cd()->SetLogz(1);
  mpcexTP[55]->cd()->SetLogz(1);
  //if(hqmips) hqmips->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[9]->cd();
  DrawStamp("MPCEX Energy Monitor 4   [[ STK PAR ADC ]]");
  mpcexTC[9]->Update();
  mpcexTC[9]->Show();
  mpcexTC[9]->SetEditable(0);
  //====================================
  if( !gROOT->FindObject("MPCEXMON_10") ) MakeCanvas("MPCEXMON_10");
  mpcexTC[10]->SetEditable(1);
  mpcexTC[10]->Clear("D");
  //======
  mpcexTP[56]->cd()->SetLogy(1);
  if(hhitsn) hhitsn->DrawCopy(); else return -1;
  if(hlhitsn) hlhitsn->DrawCopy("SAME"); else return -1;
  mpcexTP[57]->cd()->SetLogz(1);
  if(hxinghitsn) hxinghitsn->DrawCopy("colz"); else return -1;
  mpcexTP[58]->cd()->SetLogz(1);
  if(hlohin) hlohin->DrawCopy("colz"); else return -1;
  mpcexTP[59]->cd()->SetLogz(1);
  if(hqhlrn) hqhlrn->DrawCopy("colz"); else return -1;
  //if(hqmipn) hqmipn->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[10]->cd();
  DrawStamp("MPCEX Energy Monitor 5   [[ STK PAR ADC ]]");
  mpcexTC[10]->Update();
  mpcexTC[10]->Show();
  mpcexTC[10]->SetEditable(0);
  return 0;
}

int MpcExMonDraw::DrawTrg() { // 11 12
  OnlMonClient *cl = OnlMonClient::instance();
  if( !gROOT->FindObject("MPCEXMON_11") ) MakeCanvas("MPCEXMON_11");
  TH1 *hmpcs_t0 = cl->getHisto("mpcex_mpcS_T0");
  TH1 *hmpcn_t0 = cl->getHisto("mpcex_mpcN_T0");
  TH1 *hmpcls_t0 = cl->getHisto("mpcex_mpclS_T0");
  TH1 *hmpcln_t0 = cl->getHisto("mpcex_mpclN_T0");
  TH1 *hmpcs_t1 = cl->getHisto("mpcex_mpcS_T1");
  TH1 *hmpcn_t1 = cl->getHisto("mpcex_mpcN_T1");
  TH1 *hmpcls_t1 = cl->getHisto("mpcex_mpclS_T1");
  TH1 *hmpcln_t1 = cl->getHisto("mpcex_mpclN_T1");
  TH1 *hmpcs_t2 = cl->getHisto("mpcex_mpcS_T2");
  TH1 *hmpcn_t2 = cl->getHisto("mpcex_mpcN_T2");
  TH1 *hmpcls_t2 = cl->getHisto("mpcex_mpclS_T2");
  TH1 *hmpcln_t2 = cl->getHisto("mpcex_mpclN_T2");

  mpcexTC[11]->SetEditable(1);
  mpcexTC[11]->Clear("D");
  //======
  mpcexTP[62]->cd()->SetLogz(1);
  if(hmpcs_t0) hmpcs_t0->DrawCopy("colz"); else return -1;
  mpcexTP[63]->cd()->SetLogz(1);
  if(hmpcls_t0) hmpcls_t0->DrawCopy("colz"); else return -1;
  mpcexTP[64]->cd()->SetLogz(1);
  if(hmpcs_t1) hmpcs_t1->DrawCopy("colz"); else return -1;
  mpcexTP[65]->cd()->SetLogz(1);
  if(hmpcls_t1) hmpcls_t1->DrawCopy("colz"); else return -1;
  mpcexTP[66]->cd()->SetLogz(1);
  if(hmpcs_t2) hmpcs_t2->DrawCopy("colz"); else return -1;
  mpcexTP[67]->cd()->SetLogz(1);
  if(hmpcls_t2) hmpcls_t2->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[11]->cd();
  DrawStamp("MPCEX Trigger Monitor 0   [[ STK PAR ]]");
  mpcexTC[11]->Update();
  mpcexTC[11]->Show();
  mpcexTC[11]->SetEditable(0);
  //====================================
  if( !gROOT->FindObject("MPCEXMON_12") ) MakeCanvas("MPCEXMON_12");
  mpcexTC[12]->SetEditable(1);
  mpcexTC[12]->Clear("D");
  //======
  mpcexTP[68]->cd()->SetLogz(1);
  if(hmpcn_t0) hmpcn_t0->DrawCopy("colz"); else return -1;
  mpcexTP[69]->cd()->SetLogz(1);
  if(hmpcln_t0) hmpcln_t0->DrawCopy("colz"); else return -1;
  mpcexTP[70]->cd()->SetLogz(1);
  if(hmpcn_t1) hmpcn_t1->DrawCopy("colz"); else return -1;
  mpcexTP[71]->cd()->SetLogz(1);
  if(hmpcln_t1) hmpcln_t1->DrawCopy("colz"); else return -1;
  mpcexTP[72]->cd()->SetLogz(1);
  if(hmpcn_t2) hmpcn_t2->DrawCopy("colz"); else return -1;
  mpcexTP[73]->cd()->SetLogz(1);
  if(hmpcln_t2) hmpcln_t2->DrawCopy("colz"); else return -1;
  //======
  mpcexTC[12]->cd();
  DrawStamp("MPCEX Trigger Monitor 1   [[ STK PAR ]]");
  mpcexTC[12]->Update();
  mpcexTC[12]->Show();
  mpcexTC[12]->SetEditable(0);
  return 0;
}

int MpcExMonDraw::DrawRev() {
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *hbckSD = cl->getHisto("mpcex_bclockSD");
  TH1 *hbckSN = cl->getHisto("mpcex_bclockSN");
  TH1 *hptiSD = cl->getHisto("mpcex_partimeSD");
  TH1 *hptiSN = cl->getHisto("mpcex_partimeSN");
  TH1 *hstkSD = cl->getHisto("mpcex_stackSD");
  TH1 *hsphSD = cl->getHisto("mpcex_statephSD");
  TH1 *hcidSD = cl->getHisto("mpcex_cidSD");
  TH1 *hcidRG = cl->getHisto("mpcex_cidRG");
  TH1 *hcidHO = cl->getHisto("mpcex_cidavg");
  TH1 *hstkRG = cl->getHisto("mpcex_stackRG");
  TH1 *hmpcS = cl->getHisto("mpcex_mpcrS");
  TH1 *hmpcN = cl->getHisto("mpcex_mpcrN");
  TH1 *hmpcerrS = cl->getHisto("mpcex_mpcerrS");
  TH1 *hmpcerrN = cl->getHisto("mpcex_mpcerrN");
  TH1 *hmpcallS = cl->getHisto("mpcex_mpcallS");
  TH1 *hmpcallN = cl->getHisto("mpcex_mpcallN");
  if(!hbckSN) return -1;
  if(!hbckSD) return -1;
  if(!hbckSN) return -1;
  if(!hptiSD) return -1;
  if(!hptiSN) return -1;
  if(!hsphSD) return -1;
  if(!hstkSD) return -1;
  if(!hstkRG) return -1;
  if(!hcidSD) return -1;
  if(!hcidRG) return -1;
  if(!hcidHO) return -1;
  if(!hmpcS) return -1;
  if(!hmpcN) return -1;
  if(!hmpcerrS) return -1;
  if(!hmpcerrN) return -1;
  if(!hmpcallS) return -1;
  if(!hmpcallN) return -1;

  bool bSouthOFF = false;
  bool bNorthOFF = false;
  for(int i=0; i!=64; ++i) fChnErr[i] = 0;
  float nentries_cut = hcidSD->GetEntries()/16.0;
  if(nentries_cut<300) return -1;
  float nclkS = hbckSD->GetBinContent(1,1) / hbckSD->GetEntries() * 2.0;
  float nclkN = hbckSD->GetBinContent(2,1) / hbckSD->GetEntries() * 2.0;
  if(nclkS < 1) bSouthOFF = true;
  if(nclkN < 1) bNorthOFF = true;
  float dclk = hbckSN->GetMean();
  float ptiS = hptiSD->GetBinContent(1,1) / hptiSD->GetEntries() * 2.0;
  float ptiN = hptiSD->GetBinContent(2,1) / hptiSD->GetEntries() * 2.0;
  float dpti = hptiSN->GetMean();
  float sph = hsphSD->GetBinContent(1) / hsphSD->GetEntries();
  float stk = hstkSD->GetBinContent(1) / hstkSD->GetEntries();
  float stkrg = hstkRG->GetBinContent(1) / hstkRG->GetEntries();
  float cidsd[16] = {0};
  float cidrg[768] = {0};
  int npktcid = 0;
  int ncidrg = 0;
  int ncidho = 0;
  TString spktcid;
  TString scidrg;
  TString scidho;
  for(int i=0; i!=16; ++i) {
    cidsd[i] = hcidSD->GetBinContent(i+1,1) / hcidSD->GetEntries() * 16.0;
    if(cidsd[i]<0.99) {
      npktcid++;
      spktcid += Form("%d,",i);
      fChnErr[i*4+0] += 1;
      fChnErr[i*4+1] += 1;
      fChnErr[i*4+2] += 1;
      fChnErr[i*4+3] += 1;
    }
  }
  for(int i=0; i!=768; ++i) {
    cidrg[i] = hcidRG->GetBinContent(i+1,1) / hcidRG->GetEntries() * 768.0;
    if(cidrg[i]<0.99) {
      ncidrg++;
      scidrg += Form("%d,",i);
      fChnErr[i/12] += 1;
    }
    if( TMath::Abs( hcidHO->GetBinContent(i+1) - 23.5 ) > 1 ) {
      ncidho++;
      scidho += Form("%d,",i);
      fChnErr[i/12] += 1;
    }
  }

  if( !gROOT->FindObject("MPCEXMON_4") ) MakeCanvas("MPCEXMON_4");
  mpcexTC[4]->SetEditable(1);
  mpcexTC[4]->Clear("D");
  mpcexTC[4]->cd();
  DrawStamp("MPCEX State of Affairs");
  //======
  int feedS = 0;
  int feedN = 0;
  int qfeed = 0;
  bool bRAlarm = false;
  bool bYAlarm = false;
  TString sRAlarm = "";
  TString sYAlarm = "";
  float thr = 0.99;
  TLatex tex;
  tex.SetTextSize(0.03);
  if(bSouthOFF||bNorthOFF) bRAlarm = true;
  if(bSouthOFF) sRAlarm += "SouthArmOFF ";
  if(bNorthOFF) sRAlarm += "NorthArmOFF ";

  //===== NEW: The QUICKFEED procedure should be avoided when more than
  //===== half of the chain are in problem because it takes two times 
  //===== larger than a FEED.
  int nBadChainsS=0;
  for(int i=0;i!=32; i++)
    if(fChnErr[i]!=0) ++nBadChainsS;
  if(nBadChainsS>15) feedS++;
  int nBadChainsN=0;
  for(int i=32;i!=64; i++)
    if(fChnErr[i]!=0) ++nBadChainsN;
  if(nBadChainsN>15) feedN++;
  //===== That's it

  //======
  tex.DrawTextNDC(0.03,0.60,"Error Fractions in Synchronisation of Packets:");

  if(nclkS<thr||nclkN<thr) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "BeamCLK ";
    feedS += nclkS<thr?1:0;
    feedN += nclkN<thr?1:0;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.05,0.57,Form("%.2f (S)   %.2f (N)",1-nclkS,1-nclkN));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.23,0.57,Form("Beam Clock  ||  Mean Delta Ticks (N-S): %.1f",dclk));
  if(ptiS<thr||ptiN<thr) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "PARTime ";
    feedS += ptiS<thr?1:0;
    feedN += ptiN<thr?1:0;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.05,0.54,Form("%.2f (S)   %.2f (N)",1-ptiS,1-ptiN));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.23,0.54,Form("PAR Time  ||  Mean Delta Ticks (N-S): %.1f",dpti));
  if(sph<thr) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "StatePhase ";
    feedS += sph<thr?1:0;
    feedN += sph<thr?1:0;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.11,0.51,Form("%.2f",1-sph));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.23,0.51,"State Phase");
  if(stk<thr) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "Stack ";
    feedS += stk<thr?1:0;
    feedN += stk<thr?1:0;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.11,0.48,Form("%.2f",1-stk));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.23,0.48,"Stack ||");
  if(stkrg<thr) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "StackR ";
    feedS += stkrg<thr?1:0;
    feedN += stkrg<thr?1:0;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.32,0.48,Form("%.2f",1-stkrg));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.37,0.48,"not in-range [1-4]");

  //======
  tex.DrawTextNDC(0.03,0.40,"Synchronisation of CellIDs (thr 1 perCent):");
  if(npktcid>0) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "CellIDSyn ";
    qfeed += 1;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.05,0.37,Form("%d",npktcid));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.08,0.37,Form("Packets with chips not in-sync: %s",spktcid.Data()));
  if(ncidrg>0) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "CellIDRan ";
    qfeed += 1;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.05,0.34,Form("%d",ncidrg));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.08,0.34,Form("Chips with cellID not in-range [1-46]: %s",scidrg.Data()));
  if(ncidho>0) {
    tex.SetTextColor( kRed );
    bRAlarm = true;
    sRAlarm += "CellIDHom ";
    qfeed += 1;
  } else tex.SetTextColor( kGreen-3 );
  tex.DrawTextNDC(0.05,0.31,Form("%d",ncidho));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.08,0.31,Form("Chips with nonuniform cellID: %s",scidho.Data()));
  //======
  tex.DrawTextNDC(0.03,0.22,"Fraction of MPC not seen by MPCEX:");
  hmpcS->Reset();
  hmpcN->Reset();
  float mcs;
  float mcn;
  for(int i=0; i!=hmpcS->GetNbinsX(); ++i) {
    mcs = hmpcerrS->GetBinContent(i+1);
    mcn = hmpcallS->GetBinContent(i+1);
    if(mcn>0) hmpcS->Fill(mcs/mcn);
    mcs = hmpcerrN->GetBinContent(i+1);
    mcn = hmpcallN->GetBinContent(i+1);
    if(mcn>0) hmpcN->Fill(mcs/mcn);
  }
  //==
  mcs = hmpcS->GetMean();
  mcn = hmpcN->GetMean();
  tex.SetTextColor( kGreen-3 );
  if(mcs>0.5) {
    tex.SetTextColor( kYellow-3 );
    if(!bSouthOFF) {
      bYAlarm = true;
      sYAlarm += "MPCS ";
    }
  }
  tex.DrawTextNDC(0.05,0.19,Form("%.2f (S)",mcs));
  tex.SetTextColor( kGreen-3 );
  if(mcn>0.5) {
    tex.SetTextColor( kYellow-3 );
    if(!bNorthOFF) {
      bYAlarm = true;
      sYAlarm += "MPCN ";
    }
  }
  tex.DrawTextNDC(0.15,0.19,Form("%.2f (N)",mcn));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.25,0.19,"Mean");
  //==
  mcs = hmpcS->GetRMS();
  mcn = hmpcN->GetRMS();
  tex.SetTextColor( kGreen-3 );
  if(mcs>0.05) {
    tex.SetTextColor( kYellow-3 );
  }
  tex.DrawTextNDC(0.05,0.16,Form("%.2f (S)",mcs));
  tex.SetTextColor( kGreen-3 );
  if(mcn>0.05) {
    tex.SetTextColor( kYellow-3 );
  }
  tex.DrawTextNDC(0.15,0.16,Form("%.2f (N)",mcn));
  tex.SetTextColor( kBlack );
  tex.DrawTextNDC(0.25,0.16,"RMS");

  if(bRAlarm) {
    MakeOutputFEED();
    tex.SetTextColor( kRed-3 );
    tex.DrawTextNDC( 0.04,0.80, "ALARM!" );
    tex.DrawTextNDC( 0.04,0.75, Form("ECode: %s",sRAlarm.Data()) );
    TString toshift = "Action: STOP THE RUN and ";
    if(feedS>0 && feedN>0)
      toshift += "FEED both arms of ";
    else if(feedS==0 && feedN>0)
      toshift += "FEED NORTH of ";
    else if(feedN==0 && feedS>0)
      toshift += "FEED SOUTH of ";
    else
      toshift += "QUICK FEED ";
    toshift += "the MPCEX.";
    if(bSouthOFF||bNorthOFF) {
      bool bDeliberate = true;
      if(!bSouthOFF) for(int i=0; i!=32; ++i) {
	  if(fChnErr[i]>0) bDeliberate = false;
	}
      if(!bNorthOFF) for(int i=32; i!=64; ++i) {
	  if(fChnErr[i]>0) bDeliberate = false;
	}
      if(bDeliberate)
	toshift = "Was arm intentionally disabled? If not, STOP the run and FEED the MPCEX.";
    }
    tex.DrawTextNDC( 0.04,0.70, toshift.Data() );
  } else if(bYAlarm) {
    tex.SetTextColor( kYellow-3 );
    tex.DrawTextNDC( 0.04,0.80, "WARNING!" );
    tex.DrawTextNDC( 0.04,0.75, Form("ECode: %s",sYAlarm.Data()) );
    tex.DrawTextNDC( 0.04,0.70, "Action: See standing orders (SO) for ECode." );
  } else {
    tex.SetTextColor( kGreen-3 );
    tex.DrawTextNDC( 0.04,0.80, "No known issues." );
    tex.DrawTextNDC( 0.04,0.75, "\"Plane sailing\" for MPC-EX." );
  }
  TLine line;
  line.SetLineWidth(3);
  line.SetLineColor( kBlack );
  if(bYAlarm) line.SetLineColor( kYellow-3 );
  if(bRAlarm) line.SetLineColor( kRed-3 );
  line.DrawLineNDC(0.03,0.675,0.03,0.850);
  line.DrawLineNDC(0.85,0.675,0.85,0.850);
  line.DrawLineNDC(0.03,0.675,0.85,0.675);
  line.DrawLineNDC(0.03,0.850,0.85,0.850);

  //======
  mpcexTC[4]->Update();
  mpcexTC[4]->Show();
  mpcexTC[4]->SetEditable(0);
  return 0;
}

void MpcExMonDraw::MakeOutputFEED() {
  OnlMonClient *cl = OnlMonClient::instance();

  TString sFile;
  if (getenv("ONLMON_MPCEXFEEDDIR")) {
    sFile = getenv("ONLMON_MPCEXFEEDDIR");
    sFile += "/";
  } else {
    sFile = "./";
  }
  sFile += "OnlineMonitoringChainsNeedFeed.txt";
  ofstream oFile1(sFile.Data(),ios::out|ios::trunc); 
  sFile += Form(".%d",cl->RunNumber());
  ofstream oFile2(sFile.Data(),ios::out|ios::trunc); 
  int pktmap[16] = {4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3};
  int ew[8] = {1,0,0,1, 0,1,1,0};
  for(int i=0;i!=64; i++) {
    if(fChnErr[i]==0) continue;
    TString ss = i<32?"S":"N";
    int pkt = (i%32)/4;
    ss += pkt<4?"T":"B";
    int chn = i%4;
    int lyr = pktmap[pkt*2+(chn/2)];
    ss += ew[(i/32)*4+(pkt/4)*2+chn%2] < 1 ?"E":"W";
    ss += Form("%d",lyr);
    oFile1 << ss.Data() << endl;
    oFile2 << ss.Data() << endl;
  }
  oFile1.close();
  oFile2.close();
}

int MpcExMonDraw::MakePS(const char *what) {
  int iret = Draw(what);
  if(iret) return iret;
  OnlMonClient *cl = OnlMonClient::instance();
  for(int i=0; i!=13; ++i)
    if(mpcexTC[i]) {
      mpcexTC[i]->Print( Form("%s_%d_%d.ps",ThisName.data(),i,cl->RunNumber()) );
    }
  return 0;
}

int MpcExMonDraw::MakeHtml(const char *what) {
  int iret = Draw(what);
  if(iret) return iret;
  OnlMonClient *cl = OnlMonClient::instance();
  for(int i=0; i!=13; ++i)
    if(mpcexTC[i]){
      string pngfile = cl->htmlRegisterPage(*this, mpcexTC[i]->GetTitle(), Form("%d",i), "png");
      cl->CanvasToPng(mpcexTC[i], pngfile);
    }
  return 0;
}

void MpcExMonDraw::ReadCurrents() {
  TString dbnames[32] = {"SB0","ST0","SB1","ST1","SB2","ST2","SB3","ST3",
			 "SB4","ST4","SB5","ST5","SB6","ST6","SB7","ST7",
			 "NB0","NT0","NB1","NT1","NB2","NT2","NB3","NT3",
			 "NB4","NT4","NB5","NT5","NB6","NT6","NB7","NT7"};
  if(!fCurrents) {
    fCurrents = new TH2F("mpcex_currents","CURRENT last two hours;;#mu A", 32,-0.5,31.5, 100,1000,3000);
    fCurrents->GetXaxis()->SetNdivisions(505);
    fCurrents->GetYaxis()->SetNdivisions(505);
    for(int i=0; i!=32; ++i)
      fCurrents->GetXaxis()->SetBinLabel(i+1,dbnames[i]);
  }
  fCurrents->Reset();
  std::map<string,int> xref;
  for(int i=0 ; i!=32 ; ++i) xref[dbnames[i].Data()] = i;
  Connection* con;
  try {
    con = DriverManager::getConnection("daq", "phnxrc", "");
  } catch (SQLException& e) {
    return;
  }
  OnlMonClient *cl = OnlMonClient::instance();
  Statement* ste = con->createStatement();
  if(!ste) {
    delete con;
    return;
  }
  ResultSet* res;
  PHTimeStamp ts( cl->EventTime("CURRENT") );
  TDatime stime( ts.getTics() -2*3600);
  TDatime etime( ts.getTics() );
  TString startt = stime.AsSQLString();
  TString endt = etime.AsSQLString();
  TString sqlcmd = "SELECT * FROM mpcexhvinfo";
  sqlcmd += Form(" WHERE time >= \'%s\'",startt.Data());
  sqlcmd += Form(" AND time <= \'%s\'",endt.Data());
  //cout << sqlcmd.Data() << endl;
  try {
    res = ste->executeQuery( sqlcmd.Data() );
  } catch (SQLException& e) {
    delete con;
    return;
  }
  int iread_value = res->findColumn("current");
  int iread_name = res->findColumn("name");
  while( res->next() ) {
    float cur =  res->getFloat(iread_value);
    string thisname   =  res->getString(iread_name).c_str();
    int i = xref.find(thisname)->second;
    fCurrents->Fill( i, cur );
  }
  delete res;
  delete ste;
  delete con;
  return;
}

void MpcExMonDraw::ReadTemperatures() {
  TString dbnames[16] = {"SMPCEXT1Slot0AI00","SMPCEXT2Slot0AI00","SMPCEXT3Slot0AI00","SMPCEXT3Slot2AI07",
			 "SMPCEXB1Slot0AI00","SMPCEXB2Slot0AI00","SMPCEXB3Slot0AI00","SMPCEXB3Slot2AI07",
			 "NMPCEXT1Slot0AI00","NMPCEXT2Slot0AI00","NMPCEXT3Slot0AI00","NMPCEXT3Slot2AI07",
			 "NMPCEXB1Slot0AI00","NMPCEXB2Slot0AI00","NMPCEXB3Slot0AI00","NMPCEXB3Slot2AI07"};
  if(!fTemperatures) {
    fTemperatures = new TH2F("mpcex_temperatures","TEMPERATURE last two hours;;Celcius", 16,-0.5,15.5, 100,10,120);
    fTemperatures->GetXaxis()->SetNdivisions(505);
    fTemperatures->GetYaxis()->SetNdivisions(505);
    for(int i=0; i!=16; ++i)
      fTemperatures->GetXaxis()->SetBinLabel(i+1,dbnames[i]);
  }
  fTemperatures->Reset();
  Connection* con;
  try {
    con = DriverManager::getConnection("opc", "phnxrc", "");
  } catch (SQLException& e) {
    return;
  }
  OnlMonClient *cl = OnlMonClient::instance();
  Statement* ste = con->createStatement();
  if(!ste) {
    delete con;
    return;
  }
  PHTimeStamp ts( cl->EventTime("CURRENT") );
  TDatime stime( ts.getTics() -2*3600);
  TDatime etime( ts.getTics() );
  TString startt = stime.AsSQLString();
  TString endt = etime.AsSQLString();

  for(int it=0; it!=16; ++it) {
    ResultSet* res;
    TString sqlcmd = "SELECT * FROM ";
    sqlcmd += dbnames[it].Data();
    sqlcmd += Form(" WHERE time >= \'%s\'",startt.Data());
    sqlcmd += Form(" AND time <= \'%s\'",endt.Data());
    //sqlcmd += " AND read_value > 0.0 AND read_value < 80.0"; // why this?
    cout << sqlcmd.Data() << endl;
    try {
      res = ste->executeQuery( sqlcmd.Data() );
    } catch (SQLException& e) {
      continue;
    }
    int iread_value = res->findColumn("read_value");
    while( res->next() ) {
      float temp =  res->getFloat(iread_value);
      fTemperatures->Fill( it, temp );
    }
    delete res;
  }
  delete ste;
  delete con;
  return;
}

int MpcExMonDraw::DrawDeadServer(TPad *transparent) {
  transparent->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();  // set to normalized coordinates
  FatalMsg.SetTextAlign(23); // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "MONITOR");
  FatalMsg.SetTextAlign(22); // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21); // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparent->Update();
  return 0;
}
