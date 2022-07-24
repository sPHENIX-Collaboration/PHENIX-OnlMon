//////////////////////////////////////////////////////////////////
// Origin  :  Jaehyeon Do (doddeng@gmail.com)  Mar/31/2014      //
// Update  :  Carlos Perez (March 2016)                         //
// Update  :  Carlos Perez, John Lajoie (Apr 2016)              //
// Update  :  Carlos Perez (May 2016)                           //
// MPC-EX Online Monitoring code                                //
//////////////////////////////////////////////////////////////////

// Do not use #include "bla.h", it has peculiar
// effects on include paths (it uses . first which can lead to
// very interesting effects), use #include <bla.h> instead
#include "MpcExMon.h"
#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <OnlMonTrigger.h>
#include <Event.h>
#include <msg_profile.h>
#include <msg_control.h>
#include <PHPoint.h>
#include <packet_hbd_fpgashort.h>

#include <mxCalibMaster.h>
#include <mxCalibAuAu16.h>

#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TFile.h>

#include <iostream>
#include <fstream>

using namespace std;

MpcExMon::MpcExMon(const char *name):
  OnlMon(name),
  fMXHEvents(NULL),
  fMXHBeamClockSN(NULL),
  fMXHLengthS(NULL),
  fMXHLengthN(NULL),
  fMXHParTimeS(NULL),
  fMXHParTimeN(NULL),
  fMXHParTimeSN(NULL),
  fMXHParTime0(NULL),
  fMXHStackS(NULL),
  fMXHStackN(NULL),
  fMXHStack0(NULL),
  fMXHStatePhS(NULL),
  fMXHStatePhN(NULL),
  fMXHStatePh0(NULL),
  fMXHFemIDS(NULL),
  fMXHFemIDN(NULL),
  fMXHCheckSumS(NULL),
  fMXHCheckSumN(NULL),
  fMXHGrayCodeS(NULL),
  fMXHGrayCodeN(NULL),
  fMXHFemTestS(NULL),
  fMXHFemTestN(NULL),
  fMXHCBTestS(NULL),
  fMXHCBTestN(NULL),
  fMXHCellIDMap(NULL),
  fMXHHiAdc(NULL),
  fMXHLoAdc(NULL),

  fMXHHiMapS(NULL),
  fMXHLoMapS(NULL),
  fMXHHiMapN(NULL),
  fMXHLoMapN(NULL),
  fMXHLoHiS(NULL),
  fMXHLoHiN(NULL),

  fMXHPHiMapS(NULL),
  fMXHPLoMapS(NULL),
  fMXHPHiMapN(NULL),
  fMXHPLoMapN(NULL),
  fMXHPLoHiS(NULL),
  fMXHPLoHiN(NULL),

  fMXHCrossingHitS(NULL),
  fMXHCrossingHitN(NULL),
  fMXHMPCS(NULL),
  fMXHMPCN(NULL),
  fMXHMPCLS(NULL),
  fMXHMPCLN(NULL),
  fMXHMPCTotS(NULL),
  fMXHMPCTotN(NULL),
  fMXHMPCErrS(NULL),
  fMXHMPCErrN(NULL),

  fMXHMPCrS(NULL),
  fMXHMPCrN(NULL),
  fMXParScanHS(NULL),
  fMXParScanLS(NULL),
  fMXParScanHN(NULL),
  fMXParScanLN(NULL),

  fMXQHLRS(NULL),
  fMXQHLRN(NULL),
  fMXQMIPS(NULL),
  fMXQMIPN(NULL),

  fMXHMPCS_T0(NULL),
  fMXHMPCN_T0(NULL),
  fMXHMPCLS_T0(NULL),
  fMXHMPCLN_T0(NULL),
  fMXHMPCS_T1(NULL),
  fMXHMPCN_T1(NULL),
  fMXHMPCLS_T1(NULL),
  fMXHMPCLN_T1(NULL),
  fMXHMPCS_T2(NULL),
  fMXHMPCN_T2(NULL),
  fMXHMPCLS_T2(NULL),
  fMXHMPCLN_T2(NULL),

  fMXHCellIDAvg(NULL),
  fMXHCellIDSD(NULL),
  fMXHCellIDRG(NULL),
  fMXHBeamClockSD(NULL),
  fMXHParTimeSD(NULL),
  fMXHStackSD(NULL),
  fMXHStackRG(NULL),
  fMXHStatePhSD(NULL),
  fMXHHitsS(NULL),
  fMXHHitsN(NULL),
  fMXHLHitsS(NULL),
  fMXHLHitsN(NULL),
  fCal(NULL)
{
  int hichn[64] = { 61, 57, 53, 49, 45, 41, 37, 33, 29, 25, 21, 17, 13,  9,  5,  1,
		    2,  6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62,
		    66, 70, 74, 78, 82, 86, 90, 94, 98,102,106,110,114,118,122,126,
		   125,121,117,113,109,105,101, 97, 93, 89, 85, 81, 77, 73, 69, 65};
  int lochn[64] = { 63, 59, 55, 51, 47, 43, 39, 35, 31, 27, 23, 19, 15, 11,  7,  3,
		     0,  4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
		    64, 68, 72, 76, 80, 84, 88, 92, 96,100,104,108,112,116,120,124,
		   127,123,119,115,111,107,103, 99, 95, 91, 87, 83, 79, 75, 71, 67};
  for(int i=0; i!=64; ++i) {
    fHiChn[i] = hichn[i];
    fLoChn[i] = lochn[i];
  }
  int pktmx[16] = {21351,21352,21353,21354,21355,21356,21357,21358,  // south
		   21301,21302,21303,21304,21305,21306,21307,21308}; // north
  int pktmap[16] = {4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3};
  for(int i=0; i!=16; ++i) {
    fPktMX[i] = pktmx[i];
    fPktMap[i] = pktmap[i];
  }
  int pktmc[6] = {21101, 21102, 21103, 21104, 21105, 21106};
  for(int i=0; i!=6; ++i) {
    fPktMC[i] = pktmc[i];
  }  
}

MpcExMon::~MpcExMon() {
  if(fCal) delete fCal;
}

TH1F* MpcExMon::Histo1F(TString nam, TString tit, int nb, float bm, float bM, int col) {
  OnlMonServer *se = OnlMonServer::instance();
  TH1F *ret = new TH1F(nam.Data(), tit.Data(), nb, bm, bM);
  ret->SetFillColor(col);
  ret->SetLineColor(col);
  ret->GetXaxis()->SetNdivisions(505);
  ret->GetXaxis()->SetLabelSize(0.05);
  ret->GetXaxis()->SetTitleSize(0.05);
  ret->GetYaxis()->SetNdivisions(505);
  ret->GetYaxis()->SetLabelSize(0.05);
  ret->GetYaxis()->SetTitleSize(0.05);
  ret->SetTickLength(0.015,"Y");
  ret->SetStats(11111);
  se->registerHisto(this, ret);
  return ret;
}

TH2F* MpcExMon::Histo2F(TString nam, TString tit, int nx, float xm, float xM, int ny, float ym, float yM) {
  OnlMonServer *se = OnlMonServer::instance();
  TH2F* ret = new TH2F(nam.Data(), tit.Data(), nx, xm, xM, ny, ym, yM);
  ret->SetStats(111111);
  ret->GetXaxis()->SetNdivisions(505);
  ret->GetXaxis()->SetLabelSize(0.05);
  ret->GetXaxis()->SetTitleSize(0.05);
  ret->GetYaxis()->SetNdivisions(505);
  ret->GetYaxis()->SetLabelSize(0.05);
  ret->GetYaxis()->SetTitleSize(0.05);
  se->registerHisto(this, ret);
  return ret;
}

TProfile* MpcExMon::Profile(TString nam, TString tit, int nx, float xm, float xM, float ym, float yM, int col) {
  OnlMonServer *se = OnlMonServer::instance();
  TProfile* ret = new TProfile(nam.Data(), tit.Data(), nx, xm, xM, ym, yM, "s");
  ret->SetMarkerColor(col);
  ret->SetMarkerStyle(20);
  //ret->SetLineColor(col);
  //ret->SetLineWidth(0);
  ret->GetXaxis()->SetNdivisions(515);
  ret->GetXaxis()->SetLabelSize(0.05);
  ret->GetXaxis()->SetTitleSize(0.05);
  ret->GetYaxis()->SetNdivisions(505);
  ret->GetYaxis()->SetLabelSize(0.05);
  ret->GetYaxis()->SetTitleSize(0.05);
  ret->GetYaxis()->SetRangeUser(ym,yM);
  ret->SetTickLength(0.015,"Y");
  ret->SetStats(111111);
  se->registerHisto(this, ret);
  return ret;
}

int MpcExMon::Init() {
  fMXHEvents = Histo1F("mpcex_events","Events", 10, -0.5, 9.5, kBlue-3);
  fMXHLengthS = Histo2F("mpcex_lengthS","SOUTH;Packet;Data Length", 8, fPktMX[0]-0.5, fPktMX[7]+0.5, 100, 0, 3000);
  fMXHLengthN = Histo2F("mpcex_lengthN","NORTH;Packet;Data Length", 8, fPktMX[8]-0.5, fPktMX[15]+0.5, 100, 0, 3000);
  fMXHParTime0 = Histo1F("mpcex_partime0",Form("PARTIME Packet %d",fPktMX[0]), 120, -0.5, 119.5, kRed-3);
  fMXHParTimeS = Histo2F("mpcex_partimeS","SOUTH;Packet;PARTime", 8, fPktMX[0]-0.5, fPktMX[7]+0.5, 60, 0, 120);
  fMXHParTimeN = Histo2F("mpcex_partimeN","NORTH;Packet;PARTime", 8, fPktMX[8]-0.5, fPktMX[15]+0.5, 60, 0, 120);
  fMXHParTimeSN = Histo1F("mpcex_partimeSN",Form("PARTIME DIFF btw Packets %d %d",fPktMX[8],fPktMX[0]), 11, -5.5, +5.5, kCyan-3);
  fMXHParTimeSD = Histo2F("mpcex_partimeSD","PARTIME;Sync", 2, -0.5, +1.5, 1, -0.5, +0.5);
  fMXHStack0 = Histo1F("mpcex_stack0",Form("STACK in Packet %d",fPktMX[0]), 4, 0.5, 4.5, kBlack);
  fMXHStackS = Histo2F("mpcex_stackS","SOUTH;Packet;Stack", 8, fPktMX[0]-0.5, fPktMX[7]+0.5, 4, 0.5, 4.5);
  fMXHStackN = Histo2F("mpcex_stackN","NORTH;Packet;Stack", 8, fPktMX[8]-0.5, fPktMX[15]+0.5, 4, 0.5, 4.5);
  fMXHStackSD = Histo1F("mpcex_stackSD","STACK;Sync", 1, -0.5, +0.5, kGreen-3);
  fMXHStackRG = Histo1F("mpcex_stackRG","STACK;Range", 1, -0.5, +4.5, kGreen-3);
  fMXHStatePh0 = Histo1F("mpcex_stateph0",Form("STATEPHASE in Packet %d",fPktMX[0]), 256, -0.5, 511.5, kOrange-3);
  fMXHStatePhS = Histo2F("mpcex_statephS","SOUTH;Packet;StatePhase", 8, fPktMX[0]-0.5, fPktMX[7]+0.5, 64, 0, 512);
  fMXHStatePhN = Histo2F("mpcex_statephN","NORTH;Packet;StatePhase", 8, fPktMX[8]-0.5, fPktMX[15]+0.5, 64, 0, 512);
  fMXHStatePhSD = Histo1F("mpcex_statephSD","STATEPHASE;Sync", 1, -0.5, +0.5, kGreen-3);
  fMXHFemIDS = Histo2F("mpcex_femidS","SOUTH;Packet;FEM ID",8, fPktMX[0]-0.5, fPktMX[7]+0.5, 20, -0.5, 19.5);
  fMXHFemIDN = Histo2F("mpcex_femidN","NORTH;Packet;FEM ID",8, fPktMX[8]-0.5, fPktMX[15]+0.5, 20, -0.5, 19.5);
  fMXHCheckSumS = Histo2F("mpcex_checksumS","SOUTH;Packet;Checksum",8, fPktMX[0]-0.5, fPktMX[7]+0.5, 31, -100, +100);
  fMXHCheckSumN = Histo2F("mpcex_checksumN","NORTH;Packet;CheckSum",8, fPktMX[8]-0.5, fPktMX[15]+0.5, 31, -100, +100);
  fMXHBeamClockSD = Histo2F("mpcex_bclockSD","BEAMCLOCK;SYNC SOUTH;SYNC NORTH", 2, -0.5, +1.5, 1, -0.5, +0.5);
  fMXHBeamClockSN = Histo1F("mpcex_bclockSN",Form("BEAMCLOCK DIFF btw Packets %d - %d",fPktMX[8],fPktMX[0]), 11, -5.5, +5.5, kGreen-3);
  fMXHGrayCodeS = Histo2F("mpcex_graycodeS","SOUTH;Packet;GrayCode",8, fPktMX[0]-0.5, fPktMX[7]+0.5, 2, -0.5, 1.5);
  fMXHGrayCodeN = Histo2F("mpcex_graycodeN","NORTH;Packet;GrayCode",8, fPktMX[8]-0.5, fPktMX[15]+0.5, 2, -0.5, 1.5);
  fMXHFemTestS = Histo2F("mpcex_femtestS","SOUTH;Packet;FEM Test",8, fPktMX[0]-0.5, fPktMX[7]+0.5, 2, -0.5, 1.5);
  fMXHFemTestN = Histo2F("mpcex_femtestN","NORTH;Packet;FEM Test",8, fPktMX[8]-0.5, fPktMX[15]+0.5, 2, -0.5, 1.5);
  fMXHCBTestS = Histo2F("mpcex_cbtestS","SOUTH;Packet;CB Test",8, fPktMX[0]-0.5, fPktMX[7]+0.5, 2, -0.5, 1.5);
  fMXHCBTestN = Histo2F("mpcex_cbtestN","NORTH;Packet;CB Test",8, fPktMX[8]-0.5, fPktMX[15]+0.5, 2, -0.5, 1.5);
  fMXHCellIDMap = Histo2F("mpcex_cidmap","CELLID Map;Chip ID;Cell ID", 768,-0.5,767.5, 46,0.5,46.5);
  fMXHCellIDAvg = Profile("mpcex_cidavg","CELLID HOMOGENEITY;Chip ID;Cell ID Average", 768,-0.5,767.5, 0.5,46.5, kRed-3);
  fMXHCellIDSD = Histo2F("mpcex_cidSD","CELLID Synchronisation;Packet ID;Sync", 16, -0.5, 15.5, 1,-0.5,+0.5);
  fMXHCellIDRG = Histo2F("mpcex_cidRG","CELLID in good standing;Chip ID;Range", 768,-0.5,767.5, 1,0.5,46.5);
  fMXHHiAdc = Histo1F("mpcex_hiadc","HiAdc;ADC",256,-0.5,255.5,kRed-3);
  fMXHLoAdc = Histo1F("mpcex_loadc","LoAdc;ADC",256,-0.5,255.5,kBlue-3);

  fMXHHiMapS = Histo2F("mpcex_himaps","SOUTH;Channel Key;HIGH ADC", 24576,-0.5,24575.5, 64,-0.5,+255.5);
  fMXHLoMapS = Histo2F("mpcex_lomaps","SOUTH;Channel Key;LOW ADC", 24576,-0.5,24575.5, 64,-0.5,+255.5);
  fMXHHiMapN = Histo2F("mpcex_himapn","NORTH;Channel Key;HIGH ADC", 24576,24575.5,49151.5, 64,-0.5,+255.5);
  fMXHLoMapN = Histo2F("mpcex_lomapn","NORTH;Channel Key;LOW ADC", 24576,24575.5,49151.5, 64,-0.5,+255.5);
  fMXHLoHiS = Histo2F("mpcex_lohis","SOUTH;Low ADC;High ADC", 64,-0.5,255.5, 64,-0.5,255.5);
  fMXHLoHiN = Histo2F("mpcex_lohin","NORTH;Low ADC;High ADC", 64,-0.5,255.5, 64,-0.5,255.5);

  fMXHPHiMapS = Histo2F("mpcex_phimaps","SOUTH;Channel Key;HIGH ADC_{s}", 24576,-0.5,24575.5, 64,-9.5,+246.5);
  fMXHPLoMapS = Histo2F("mpcex_plomaps","SOUTH;Channel Key;LOW ADC_{s}", 24576,-0.5,24575.5, 64,-9.5,+246.5);
  fMXHPHiMapN = Histo2F("mpcex_phimapn","NORTH;Channel Key;HIGH ADC_{s}", 24576,24575.5,49151.5, 64,-9.5,+246.5);
  fMXHPLoMapN = Histo2F("mpcex_plomapn","NORTH;Channel Key;LOW ADC_{s}", 24576,24575.5,49151.5, 64,-9.5,+246.5);
  fMXHPLoHiS = Histo2F("mpcex_plohis","SOUTH;Low ADC_{s};High ADC_{s}", 64,-9.5,246.5, 64,-9.5,246.5);
  fMXHPLoHiN = Histo2F("mpcex_plohin","NORTH;Low ADC_{s};High ADC_{s}", 64,-9.5,246.5, 64,-9.5,246.5);

  fMXHCrossingHitS = Histo2F("mpcex_xinghits","SOUTH;MAX HITS - 1;GL1 Clock", 50,0,5000, 60,-0.5,119.5);
  fMXHCrossingHitN = Histo2F("mpcex_xinghitn","NORTH;MAX HITS - 1;GL1 Clock", 50,0,5000, 60,-0.5,119.5);
  fMXHMPCS = Histo2F("mpcex_mpcS","SOUTH;MPCEXhi;MPC",100,0,15e4,100,0,8e2);
  fMXHMPCN = Histo2F("mpcex_mpcN","NORTH;MPCEXhi;MPC",100,0,5e4,100,0,3e2);
  fMXHMPCLS = Histo2F("mpcex_mpclS","SOUTH;MPCEXlo;MPC",100,0,4e4,100,0,8e2);
  fMXHMPCLN = Histo2F("mpcex_mpclN","NORTH;MPCEXlo;MPC",100,0,1.5e4,100,0,3e2);

  fMXHMPCTotS = Histo1F("mpcex_mpcallS","SOUTH;MPC",100,0,8e2,kGreen-3);
  fMXHMPCTotN = Histo1F("mpcex_mpcallN","NORTH;MPC",100,0,3e2,kGreen-3);
  fMXHMPCErrS = Histo1F("mpcex_mpcerrS","SOUTH;MPC",100,0,8e2,kRed-3);
  fMXHMPCErrN = Histo1F("mpcex_mpcerrN","NORTH;MPC",100,0,3e2,kRed-3);

  fMXHMPCrS = Histo1F("mpcex_mpcrS","SOUTH;MPCerrorF",100,0,1,kBlack);
  fMXHMPCrN = Histo1F("mpcex_mpcrN","NORTH;MPCerrorF",100,0,1,kBlack);
  fMXParScanHS = Histo2F("mpcex_parscanHS","SOUTH;PAR;HIGH ADC",120,-0.5,119.5,256,-0.5,255.5);
  fMXParScanLS = Histo2F("mpcex_parscanLS","SOUTH;PAR;LOW ADC",120,-0.5,119.5,256,-0.5,255.5);
  fMXParScanHN = Histo2F("mpcex_parscanHN","NORTH;PAR;HIGH ADC",120,-0.5,119.5,256,-0.5,255.5);
  fMXParScanLN = Histo2F("mpcex_parscanLN","NORTH;PAR;LOW ADC",120,-0.5,119.5,256,-0.5,255.5);

  fMXQHLRS = Histo2F("mpcex_qhlrS","SOUTH;Row ID;High over Low",768,-0.5,767.5,100,0,10.0);
  fMXQHLRN = Histo2F("mpcex_qhlrN","NORTH;Row ID;High over Low",768,767.5,1535.5,100,0,10.0);
  fMXQMIPS = Histo2F("mpcex_qmipS","SOUTH;Row ID;ADChi",768,-0.5,767.5,170,0,170);
  fMXQMIPN = Histo2F("mpcex_qmipN","NORTH;Row ID;ADChi",768,767.5,1535.5,170,0,170);

  fMXHMPCS_T0 = Histo2F("mpcex_mpcS_T0","SOUTH;MPCEXhi;MPC",100,0,15e4,100,0,8e2);
  fMXHMPCN_T0 = Histo2F("mpcex_mpcN_T0","NORTH;MPCEXhi;MPC",100,0,5e4,100,0,3e2);
  fMXHMPCLS_T0 = Histo2F("mpcex_mpclS_T0","SOUTH;MPCEXlo;MPC",100,0,4e4,100,0,8e2);
  fMXHMPCLN_T0 = Histo2F("mpcex_mpclN_T0","NORTH;MPCEXlo;MPC",100,0,1.5e4,100,0,3e2);
  fMXHMPCS_T1 = Histo2F("mpcex_mpcS_T1","SOUTH;MPCEXhi;MPC",100,0,15e4,100,0,8e2);
  fMXHMPCN_T1 = Histo2F("mpcex_mpcN_T1","NORTH;MPCEXhi;MPC",100,0,5e4,100,0,3e2);
  fMXHMPCLS_T1 = Histo2F("mpcex_mpclS_T1","SOUTH;MPCEXlo;MPC",100,0,4e4,100,0,8e2);
  fMXHMPCLN_T1 = Histo2F("mpcex_mpclN_T1","NORTH;MPCEXlo;MPC",100,0,1.5e4,100,0,3e2);
  fMXHMPCS_T2 = Histo2F("mpcex_mpcS_T2","SOUTH;MPCEXhi;MPC",100,0,15e4,100,0,8e2);
  fMXHMPCN_T2 = Histo2F("mpcex_mpcN_T2","NORTH;MPCEXhi;MPC",100,0,5e4,100,0,3e2);
  fMXHMPCLS_T2 = Histo2F("mpcex_mpclS_T2","SOUTH;MPCEXlo;MPC",100,0,4e4,100,0,8e2);
  fMXHMPCLN_T2 = Histo2F("mpcex_mpclN_T2","NORTH;MPCEXlo;MPC",100,0,1.5e4,100,0,3e2);

  fMXHHitsS = Histo1F("mpcex_hitsS","SOUTH;HITS",100,0,5000,kBlue-3);
  fMXHHitsN = Histo1F("mpcex_hitsN","NORTH;HITS",100,0,5000,kBlue-3);
  fMXHLHitsS = Histo1F("mpcex_lowhitsS","SOUTH;HITS",100,0,5000,kRed-3);
  fMXHLHitsN = Histo1F("mpcex_lowhitsN","NORTH;HITS",100,0,5000,kRed-3);

  Reset();
  return 0;
}

int MpcExMon::Reset() {
  fMXHEvents->Reset();
  fMXHLengthS->Reset();
  fMXHLengthN->Reset();
  fMXHParTime0->Reset();
  fMXHParTimeS->Reset();
  fMXHParTimeN->Reset();
  fMXHParTimeSN->Reset();
  fMXHParTimeSD->Reset();
  fMXHStack0->Reset();
  fMXHStackS->Reset();
  fMXHStackN->Reset();
  fMXHStackSD->Reset();
  fMXHStackRG->Reset();
  fMXHStatePh0->Reset();
  fMXHStatePhS->Reset();
  fMXHStatePhN->Reset();
  fMXHStatePhSD->Reset();
  fMXHFemIDS->Reset();
  fMXHFemIDN->Reset();
  fMXHCheckSumS->Reset();
  fMXHCheckSumN->Reset();
  fMXHBeamClockSD->Reset();
  fMXHBeamClockSN->Reset();
  fMXHGrayCodeS->Reset();
  fMXHGrayCodeN->Reset();
  fMXHFemTestS->Reset();
  fMXHFemTestN->Reset();
  fMXHCBTestS->Reset();
  fMXHCBTestN->Reset();
  fMXHCellIDMap->Reset();
  fMXHCellIDAvg->Reset();
  fMXHCellIDSD->Reset();
  fMXHCellIDRG->Reset();
  fMXHHiAdc->Reset();
  fMXHLoAdc->Reset();

  fMXHHiMapS->Reset();
  fMXHLoMapS->Reset();
  fMXHHiMapN->Reset();
  fMXHLoMapN->Reset();
  fMXHLoHiS->Reset();
  fMXHLoHiN->Reset();

  fMXHPHiMapS->Reset();
  fMXHPLoMapS->Reset();
  fMXHPHiMapN->Reset();
  fMXHPLoMapN->Reset();
  fMXHPLoHiS->Reset();
  fMXHPLoHiN->Reset();

  fMXHCrossingHitS->Reset();
  fMXHCrossingHitN->Reset();
  fMXHMPCS->Reset();
  fMXHMPCN->Reset();
  fMXHMPCLS->Reset();
  fMXHMPCLN->Reset();

  fMXHMPCTotS->Reset();
  fMXHMPCTotN->Reset();
  fMXHMPCErrS->Reset();
  fMXHMPCErrN->Reset();

  fMXHMPCrS->Reset();
  fMXHMPCrN->Reset();

  fMXQHLRS->Reset();
  fMXQHLRN->Reset();
  fMXQMIPS->Reset();
  fMXQMIPN->Reset();

  fMXHMPCS_T0->Reset();
  fMXHMPCN_T0->Reset();
  fMXHMPCLS_T0->Reset();
  fMXHMPCLN_T0->Reset();
  fMXHMPCS_T1->Reset();
  fMXHMPCN_T1->Reset();
  fMXHMPCLS_T1->Reset();
  fMXHMPCLN_T1->Reset();
  fMXHMPCS_T2->Reset();
  fMXHMPCN_T2->Reset();
  fMXHMPCLS_T2->Reset();
  fMXHMPCLN_T2->Reset();

  fMXHHitsS->Reset();
  fMXHHitsN->Reset();
  fMXHLHitsS->Reset();
  fMXHLHitsN->Reset();
  return 0;
}

int MpcExMon::BeginRun(const int runno) {
  if(fCal) delete fCal;
  fCal = new mxCalibAuAu16();

  //===== QUICKFEED needs a fix every time there is a new run
  TString sFile;
  if (getenv("ONLMON_MPCEXFEEDDIR")) {
    sFile = getenv("ONLMON_MPCEXFEEDDIR");
    sFile += "/";
  } else {
    sFile = "./";
  }
  sFile += "OnlineMonitoringChainsNeedFeed.txt";
  ofstream oFile1(sFile.Data(),ios::out|ios::trunc);
  oFile1.close();
  sFile += Form(".%d",runno);
  ofstream oFile2(sFile.Data(),ios::out|ios::trunc);
  oFile2.close();
  //===== DONE

  return 0;
}

int MpcExMon::EndRun(const int runno) {
  TFile *qa = new TFile(Form("/home/phnxrc/mpcexonlmon/%d_0.root",runno),"RECREATE");
  qa->WriteTObject(fMXQHLRS,fMXQHLRS->GetName());
  qa->WriteTObject(fMXQHLRN,fMXQHLRN->GetName());
  qa->WriteTObject(fMXQMIPS,fMXQMIPS->GetName());
  qa->WriteTObject(fMXQMIPN,fMXQMIPN->GetName());
  qa->Close();
  delete qa;
  return 0;
}

int MpcExMon::process_event(Event *evt) {
  FillHistograms(evt);
  return 0;
}

void MpcExMon::FillHistograms(Event *e) {
  fMXHEvents->Fill(0);
  Packet *p;

  int crossingCounter = -1;
  //============
  // GL1 data
  p = e->getPacket(14001);
  if(p) {
    crossingCounter = p->iValue(0,"CROSSCTR");
    delete p;
  }

  //============
  // GL1 data
  p = e->getPacket(14009);
  if(p) {
    //int pevt0 = p->iValue(0,"EVCLOCK");
    //int pevt1 = p->iValue(1,"EVCLOCK");
    delete p;
  }

  //============
  // MPCEX data
  float mxEne[2] = {0};
  float mxEneL[2] = {0};
  int nlowhits[2] = {0};
  int nhits[2] = {0};
  int bclkSN[2];
  float bclkSD[2] = {0};
  int ptiSN[2];
  float ptiSD[2] = {0};
  int sphT = 0;
  float sphSD = 0;
  int stkT = 0;
  float stkSD = 0;
  bool avoidpar = false;
  for(int iarm=0; iarm!=2; ++iarm) {
    for(int pkt=0; pkt!=8; ++pkt) {
      int ipkt = fPktMX[8*iarm+pkt];
      p = e->getPacket(ipkt);
      if(!p) continue;
      //int evt = p->iValue(0,"EVTNR"); //==
      //int evt = p->iValue(0,"EXTENDEDBCLK"); //==
      //int evt = p->iValue(0,"FIRMWARE"); //==
      //int evt = p->iValue(0,"DETID"); //==
      int fid = p->iValue(0,"FEMID"); //==
      int bclk = p->iValue(0,"BCLCK"); //==
      int cksm0 = p->iValue(0,"PARITY"); //==
      int cksm1 = p->iValue(0,"CALCULATEDPARITY"); //==
      int cbtm = p->iValue(0,"CBTESTMODE"); //==
      int fmtm = p->iValue(0,"FEMTESTMODE"); //==
      int gcod = p->iValue(0,"GRAYDECODING"); //==
      int pti = p->iValue(0,"PARSTTIME"); //==
      int stk = p->iValue(0,"STACK"); //==
      int sph = p->iValue(0,"STATEPHASE"); //==
      if(pkt==0) {
	bclkSN[iarm] = bclk;
	ptiSN[iarm] = pti;
	if(iarm==0) {
	  stkT = stk;
	  sphT = sph;
	}
      }
      bclkSD[iarm] += TMath::Power( bclk - bclkSN[iarm], 2 ) / 8.0;
      ptiSD[iarm] += TMath::Power( pti - ptiSN[iarm], 2 ) / 16.0;
      stkSD += TMath::Power( stk - stkT, 2 ) / 16.0;
      sphSD += TMath::Power( sph - sphT, 2 ) / 16.0;
      fMXHStackRG->Fill(stk);
      if(iarm==0) {
	if(pkt==0) {
	  fMXHParTime0->Fill(pti);
	  fMXHStatePh0->Fill(sph);
	  fMXHStack0->Fill(stk);
	}
	fMXHLengthS->Fill(ipkt,p->getDataLength());
	fMXHParTimeS->Fill(ipkt,pti);
      	fMXHStackS->Fill(ipkt,stk);
      	fMXHStatePhS->Fill(ipkt,sph);
	fMXHFemIDS->Fill(ipkt,fid);
	fMXHGrayCodeS->Fill(ipkt,gcod);
	fMXHFemTestS->Fill(ipkt,fmtm);
	fMXHCBTestS->Fill(ipkt,cbtm);
	fMXHCheckSumS->Fill(ipkt,cksm0-cksm1);
      } else {
	fMXHLengthN->Fill(ipkt,p->getDataLength());
	fMXHParTimeN->Fill(ipkt,pti);
      	fMXHStackN->Fill(ipkt,stk);
      	fMXHStatePhN->Fill(ipkt,sph);
	fMXHFemIDN->Fill(ipkt,fid);
	fMXHGrayCodeN->Fill(ipkt,gcod);
	fMXHFemTestN->Fill(ipkt,fmtm);
	fMXHCBTestN->Fill(ipkt,cbtm);
	fMXHCheckSumN->Fill(ipkt,cksm0-cksm1);
      }
      //=== CUTS (from Tom, John, Andrei)
      if(stk!=1) {
	delete p;
	continue;
      }
      //if(pti==43 || pti==44) {
      if(pti>38&&pti<47) {
	avoidpar = true;
      }
      //===
      int cidT = 0;
      float cidSD = 0;
      for(int chn=0; chn!=4; ++chn) {
        //int lyr = fPktMap[pkt*2+(chn/2)];
        for(int chp=0; chp!=12; ++chp) {
	  int chipid = 8*4*12*iarm+4*12*pkt+12*chn+chp;
          int cellnr = p->iValue(chp,chn,"CELLNR"); //==
	  if(chn==0&&chp==0) cidT = cellnr;
	  cidSD += TMath::Power( cellnr - cidT, 2 ) / 48.0;
	  fMXHCellIDAvg->Fill(chipid,cellnr);
	  fMXHCellIDMap->Fill(chipid,cellnr);
	  fMXHCellIDRG->Fill(chipid,cellnr);
          int adc[128];
          for(int channel=0; channel!=128; ++channel) {
            int thischan = chp*128+channel;
            int val = p->iValue(thischan,chn);
            adc[channel] = val;
          }
          for(int ch=0; ch!=64; ++ch) {
	    int key = iarm*24576+pkt*3072+(chn*12+chp)*64+ch;
	    int senid = key/128;
	    int rowid = senid*4+key%4;
            int lo = fLoChn[ch];
            int hi = fHiChn[ch];
	    int hiadc = adc[hi];
	    int loadc = adc[lo];
	    float hiadcped = hiadc - fCal->GetPHMu()->Get(key);
	    float loadcped = loadc - fCal->GetPLMu()->Get(key);
	    fMXHHiAdc->Fill(hiadc);
	    fMXHLoAdc->Fill(loadc);
	    float sgH = fCal->GetPHSg()->Get(key);
	    float sgL = fCal->GetPLSg()->Get(key);
	    if(sgH<0.5 || sgH>1.5) sgH=-1;
	    if(sgL<0.5 || sgL>1.5) sgL=-1;
	    if(iarm==0) {
	      fMXHHiMapS->Fill(key,hiadc);
	      fMXHLoMapS->Fill(key,loadc);
	      fMXHLoHiS->Fill(loadc,hiadc);
	      fMXParScanHS->Fill(pti,hiadc);
	      fMXParScanLS->Fill(pti,loadc);
	      if(!avoidpar) {
		fMXHPHiMapS->Fill(key,hiadcped);
		fMXHPLoMapS->Fill(key,loadcped);
		if(hiadcped>3*sgH && sgH>0 && loadcped>3*sgL && sgL>0 && hiadc<200) {
		  fMXHPLoHiS->Fill(loadcped,hiadcped);
		  fMXQHLRS->Fill(rowid,hiadcped/loadcped);
		  fMXQMIPS->Fill(rowid,hiadcped);
		}
	      }
	    } else {
	      fMXHHiMapN->Fill(key,hiadc);
	      fMXHLoMapN->Fill(key,loadc);
	      fMXHLoHiN->Fill(loadc,hiadc);
	      fMXParScanHN->Fill(pti,hiadc);
	      fMXParScanLN->Fill(pti,loadc);
	      if(!avoidpar) {
		fMXHPHiMapN->Fill(key,hiadcped);
		fMXHPLoMapN->Fill(key,loadcped);
		if(hiadcped>3*sgH && sgH>0 && loadcped>3*sgL && sgL>0 && hiadc<200) {
		  fMXHPLoHiN->Fill(loadcped,hiadcped);
		  fMXQHLRN->Fill(rowid,hiadcped/loadcped);
		  fMXQMIPN->Fill(rowid,hiadcped);
		}
	      }
	    }
	    if(!avoidpar) {
	      bool hithigh = false;
	      if(hiadcped>5*sgH && sgH>0) {
		mxEne[iarm] += hiadcped;
		nhits[iarm] += 1;
		hithigh = true;
	      }
	      if(loadcped>5*sgL && sgL>0) {
		mxEneL[iarm] += loadcped;
		if(!hithigh) nlowhits[iarm] += 1;
	      }
	    }
          }
        } // end of chp
      } // end of chn
      fMXHCellIDSD->Fill(8*iarm+pkt,cidSD);
      delete p;
    } // end of pkt
  } // end of arm
  fMXHCrossingHitS->Fill(TMath::Max(nhits[0],nlowhits[0])-1,crossingCounter);
  fMXHCrossingHitN->Fill(TMath::Max(nhits[1],nlowhits[1])-1,crossingCounter);
  fMXHBeamClockSN->Fill(bclkSN[1]-bclkSN[0]);
  fMXHBeamClockSD->Fill(0.0,bclkSD[0]);
  fMXHBeamClockSD->Fill(1.0,bclkSD[1]);
  fMXHParTimeSN->Fill(ptiSN[1]-ptiSN[0]);
  fMXHParTimeSD->Fill(0.0,ptiSD[0]);
  fMXHParTimeSD->Fill(1.0,ptiSD[1]);
  fMXHStackSD->Fill(stkSD);
  fMXHStatePhSD->Fill(sphSD);
  fMXHHitsS->Fill(nhits[0]);
  fMXHHitsN->Fill(nhits[1]);
  fMXHLHitsS->Fill(nlowhits[0]);
  fMXHLHitsN->Fill(nlowhits[1]);
  //============================== FAST CHECK MPC
  Packet_hbd_fpgashort *mpcP;
  float enes=0, enen=0;
  for(int i=0; i!=6; ++i) {
    int ipkt = fPktMC[i];
    p = e->getPacket(ipkt);
    if(!p) continue;
    mpcP = dynamic_cast<Packet_hbd_fpgashort*> (p);
    mpcP->setNumSamples( 12 );
    const int nModPerFEM = p->iValue(0,"NRMODULES");
    for(int imod=0; imod!=nModPerFEM; ++imod) {
      for(int j=0; j!=48; j++) {
        int fee576ch = i*nModPerFEM*48 + imod*48 + j;
        int ch = imod*48 + j;
        short isamp_min=0, samp_min=9999, isamp_max=0, samp_max=0;
        for(int isamp=0; isamp!=12; ++isamp) {
          short val = ( p->iValue(ch,isamp) ) & 0xfff;
          if( val<samp_min ) {
            isamp_min=isamp;
            samp_min=val;
          }
          if( val>samp_max ) {
            isamp_max=isamp;
            samp_max=val;
          }
        }
        float energy = samp_max-samp_min;
        if( fee576ch==484 || fee576ch==494 || fee576ch==500 || fee576ch==509 )
          energy /= 10.0;
        if( energy < 10 ) continue;
        if( fee576ch<288 ) enes += energy;
        else enen += energy;
      }
    }
    delete p;
  }
  if(stkT==1&&!avoidpar) {
    fMXHMPCTotS->Fill(enes);
    fMXHMPCTotN->Fill(enen);
    if(mxEne[0]<1&&mxEneL[0]<1) fMXHMPCErrS->Fill(enes);
    if(mxEne[1]<1&&mxEneL[1]<1) fMXHMPCErrN->Fill(enen);
    fMXHMPCS->Fill(mxEne[0],enes);
    fMXHMPCN->Fill(mxEne[1],enen);
    fMXHMPCLS->Fill(mxEneL[0],enes);
    fMXHMPCLN->Fill(mxEneL[1],enen);

    OnlMonServer *se = OnlMonServer::instance();
    if(se->Trigger("BBCLL1(>0 tubes) novertex")) {
      fMXHMPCS_T0->Fill(mxEne[0],enes);
      fMXHMPCN_T0->Fill(mxEne[1],enen);
      fMXHMPCLS_T0->Fill(mxEneL[0],enes);
      fMXHMPCLN_T0->Fill(mxEneL[1],enen);
    }
    if(se->Trigger("MPC_N_B")) {
      fMXHMPCS_T1->Fill(mxEne[0],enes);
      fMXHMPCN_T1->Fill(mxEne[1],enen);
      fMXHMPCLS_T1->Fill(mxEneL[0],enes);
      fMXHMPCLN_T1->Fill(mxEneL[1],enen);
    }
      if(se->Trigger("MPC_S_B")) {
      fMXHMPCS_T2->Fill(mxEne[0],enes);
      fMXHMPCN_T2->Fill(mxEne[1],enen);
      fMXHMPCLS_T2->Fill(mxEneL[0],enes);
      fMXHMPCLN_T2->Fill(mxEneL[1],enen);
    }

  }
  //if(enes>0) fMXHMPCrS->Fill(mxEne[0]/enes);
  //if(enen>0) fMXHMPCrN->Fill(mxEne[1]/enen);
}
