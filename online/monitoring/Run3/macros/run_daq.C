void run_daq(const char *prdffile = "data.prdf")
{
  //    gROOT->ProcessLine(".T");
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonldaqmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  DaqMon *daq = DaqMon::instance();
  //daq->Verbosity(1);
  OnlMonServer *se = OnlMonServer::instance();
  GranuleMon *Acc = new DaqAccMon();
  GranuleMon *Bbc = new DaqBbcMon();
  GranuleMon *DchEast = new DaqDchMon("EAST");
  GranuleMon *DchWest = new DaqDchMon("WEST");
  GranuleMon *EmcEastTop = new DaqEmcMon("EAST", "TOP");
  GranuleMon *EmcWestTop = new DaqEmcMon("WEST", "TOP");
  GranuleMon *EmcEastBottom = new DaqEmcMon("EAST", "BOTTOM");
  GranuleMon *EmcWestBottom = new DaqEmcMon("WEST", "BOTTOM");
  GranuleMon *ErtEast = new DaqErtMon("EAST");
  GranuleMon *ErtWest = new DaqErtMon("WEST");
  //  GranuleMon *Fcal = new DaqFcalMon("FCAL");
  GranuleMon *Fvtx = new DaqFvtxMon("FVTX");
  GranuleMon *Gl1 = new DaqGl1Mon();
  //  GranuleMon *MpcNorth = new DaqFcalMon("MPC");
  GranuleMon *Mpc = new DaqMpcMon();
  GranuleMon *MpcExNorth = new DaqMpcExMon("NORTH");
  GranuleMon *MpcExSouth = new DaqMpcExMon("SOUTH");
  GranuleMon *MuidSouth = new DaqMuidMon("SOUTH");
  GranuleMon *MuidNorth = new DaqMuidMon("NORTH");
  // GranuleMon *MuTrigNorth = new DaqMuTrigMon("NORTH");
  // GranuleMon *MuTrigSouth = new DaqMuTrigMon("SOUTH");
  GranuleMon *MutrNorth = new DaqMutrMon("NORTH");
  //  MutrNorth->NoFemClkReference(11267);
  //  MutrNorth->NoFemClkReference(11268);
  //MutrNorth->Verbosity(2);
  GranuleMon *MutrSouth = new DaqMutrMon("SOUTH");
  //   MutrSouth->NoFemClkReference(11091);
  //   MutrSouth->NoFemClkReference(11092);
  //   MutrSouth->NoFemClkReference(11154);
  //   MutrSouth->NoFemClkReference(11155);
  //  MutrSouth->Verbosity(2);
  GranuleMon *PadEast = new DaqPadMon("EAST");
  GranuleMon *PadWest = new DaqPadMon("WEST");
  PadWest->NoFemClkReference(4074);
  GranuleMon *RichEast = new DaqRichMon("EAST");
  GranuleMon *RichWest = new DaqRichMon("WEST");
  // GranuleMon *Rpc1 = new DaqRpcMon("RPC1");
  // GranuleMon *Rpc3 = new DaqRpcMon("RPC3");
  GranuleMon *TofEast = new DaqTofMon("EAST");
  TofEast->NoFemClkReference(7006);
  GranuleMon *TofWest = new DaqTofMon("WEST");
  GranuleMon *Vtxp = new DaqVtxpMon("VTXP");
  //  Vtxp->Verbosity(1);
  GranuleMon *Vtxs = new DaqVtxsMon("VTXS");
  // Vtxs->Verbosity(2);
  //  Vtxs->NoFemClkReference(24134);
  GranuleMon *Zdc = new DaqZdcMon();
  daq->registerMonitor(Acc);
  daq->registerMonitor(Bbc);
  daq->registerMonitor(DchEast);
  daq->registerMonitor(DchWest);
  daq->registerMonitor(EmcEastTop);
  daq->registerMonitor(EmcWestTop);
  daq->registerMonitor(EmcEastBottom);
  daq->registerMonitor(EmcWestBottom);
  daq->registerMonitor(ErtEast);
  daq->registerMonitor(ErtWest);
  //  daq->registerMonitor(Fcal);
  daq->registerMonitor(Fvtx);
  daq->registerMonitor(Gl1);
//   daq->registerMonitor(MpcNorth);
  daq->registerMonitor(Mpc);
  daq->registerMonitor(MpcExNorth);
  daq->registerMonitor(MpcExSouth);
  daq->registerMonitor(MuidSouth);
  daq->registerMonitor(MuidNorth);
  // daq->registerMonitor(MuTrigSouth);
  // daq->registerMonitor(MuTrigNorth);
  daq->registerMonitor(MutrSouth);
  daq->registerMonitor(MutrNorth);
  daq->registerMonitor(PadEast);
  daq->registerMonitor(PadWest);
  daq->registerMonitor(RichEast);
  daq->registerMonitor(RichWest);
  // daq->registerMonitor(Rpc1);
  // daq->registerMonitor(Rpc3);
  daq->registerMonitor(TofEast);
  daq->registerMonitor(TofWest);
  daq->registerMonitor(Vtxp);
  daq->registerMonitor(Vtxs);
  daq->registerMonitor(Zdc);

  se->registerMonitor(daq);
  start_server(prdffile);
  return;
}


void daqDrawInit(const int online = 0)
{
  // if online=0 run Au+Au from Run2
  // if online=1 run dAu from Run3
  // if online=2 run pp from Run2
  gSystem->Load("libonldaqmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
#define ngranules 28
  char *hname[3];
  char tmp[100];
  char *Granules[] =
    {
      "ACC",
      "BBC",
      "DCHEAST",
      "DCHWEST",
      "EMCEASTTOP",
      "EMCWESTTOP",
      "EMCEASTBOTTOM",
      "EMCWESTBOTTOM",
      "ERTEAST",
      "ERTWEST",
      "FVTX",
      "GL1",
      "MPC",
      "MPCEXNORTH",
      "MPCEXSOUTH",
      "MUIDNORTH",
      "MUIDSOUTH",
      // "MUTRIGNORTH",
      // "MUTRIGSOUTH",
      "MUTRNORTH",
      "MUTRSOUTH",
      "PADEAST",
      "PADWEST",
      "RICHEAST",
      "RICHWEST",
      // "RPC1",
      // "RPC3",
      "TOFEAST",
      "TOFWEST",
      "VTXP",
      "VTXS",
      "ZDC"
    };
  hname[0] = "Monitor";
  hname[1] = "PacketId";
  hname[2] = "AllVariables";
  OnlMonClient *cl = OnlMonClient::instance();
  for (short int i = 0; i < ngranules; i++)
    {
      for (short int j = 0; j < 3; j++)
        {
          sprintf(tmp, "Daq_%s%s", Granules[i], hname[j]);
          cl->registerHisto(tmp, "DAQ");
          cout << "registering " << tmp << endl;
        }
    }
  cl->registerHisto("Daq_MonStatus", "DAQ");
  cl->registerHisto("Daq_bcross", "DAQ");
  cl->registerHisto("Daq_TimeToLastEvent0", "DAQ");
  cl->registerHisto("Daq_TimeToLastEvent1", "DAQ");
  cl->registerHisto("Daq_TimeToLastEvent2", "DAQ");
  cl->registerHisto("Daq_TimeToLastEvent3", "DAQ");
  cl->registerHisto("Daq_FrameStatus", "DAQ");
  CreateHostList(online);
  DaqMonDraw *daq = DaqMonDraw::instance();
  daq->AbortGapMessage(0);
  cl->registerDrawer(daq);
  GranMonDraw *Acc = new GranMonDraw("ACC");
  GranMonDraw *Bbc = new GranMonDraw("BBC");
  GranMonDraw *DchEast = new GranMonDraw("DCHEAST");
  GranMonDraw *DchWest = new GranMonDraw("DCHWEST");
  GranMonDraw *EmcEastTop = new GranMonDraw("EMCEASTTOP");
  GranMonDraw *EmcWestTop = new GranMonDraw("EMCWESTTOP");
  GranMonDraw *EmcEastBottom = new GranMonDraw("EMCEASTBOTTOM");
  GranMonDraw *EmcWestBottom = new GranMonDraw("EMCWESTBOTTOM");
  GranMonDraw *ErtEast = new GranMonDraw("ERTEAST");
  GranMonDraw *ErtWest = new GranMonDraw("ERTWEST");
  //  GranMonDraw *Fcal = new GranMonDraw("FCAL");
  GranMonDraw *Fvtx = new GranMonDraw("FVTX");
  GranMonDraw *Gl1 = new GranMonDraw("GL1");
  //Gl1->SetBadPacket(14007,"Missing");

  GranMonDraw *Mpc = new GranMonDraw("MPC");
  Mpc->SetBadPacket(21101,"SubsysErr");
  Mpc->SetBadPacket(21102,"SubsysErr");
  Mpc->SetBadPacket(21103,"SubsysErr");
  Mpc->SetBadPacket(21104,"SubsysErr");
  Mpc->SetBadPacket(21105,"SubsysErr");
  Mpc->SetBadPacket(21106,"SubsysErr");
  GranMonDraw *MpcExNorth = new GranMonDraw("MPCEXNORTH");
  GranMonDraw *MpcExSouth = new GranMonDraw("MPCEXSOUTH");
  GranMonDraw *MuidNorth = new GranMonDraw("MUIDNORTH");
  GranMonDraw *MuidSouth = new GranMonDraw("MUIDSOUTH");
  // GranMonDraw *MuTrigNorth = new GranMonDraw("MUTRIGNORTH");
  // GranMonDraw *MuTrigSouth = new GranMonDraw("MUTRIGSOUTH");
  GranMonDraw *MutrNorth = new GranMonDraw("MUTRNORTH");
  //  MutrNorth->SetBadPacket(11267,"DCMParErr");
  //  MutrNorth->SetBadPacket(11267,"FemClkErr");
  //  MutrNorth->SetBadPacket(11267,"Gl1SyncErr");
  //  MutrNorth->SetBadPacket(11268,"DCMParErr");
  //  MutrNorth->SetBadPacket(11268,"FemClkErr");
  //  MutrNorth->SetBadPacket(11268,"Gl1SyncErr");
  //   MutrNorth->SetBadPacket(11269,"EvtNoErr");
  GranMonDraw *MutrSouth = new GranMonDraw("MUTRSOUTH");
  //   MutrSouth->SetBadPacket(11091,"DCMParErr");
  //   MutrSouth->SetBadPacket(11091,"FemClkErr");
  //   MutrSouth->SetBadPacket(11091,"Gl1SyncErr");
  //   MutrSouth->SetBadPacket(11091,"EvtNoErr");
  //   MutrSouth->SetBadPacket(11091,"ParityErr");
  //   MutrSouth->SetBadPacket(11091,"SubsysErr");

  //   MutrSouth->SetBadPacket(11092,"DCMParErr");
  //   MutrSouth->SetBadPacket(11092,"FemClkErr");
  //   MutrSouth->SetBadPacket(11092,"Gl1SyncErr");
  //   MutrSouth->SetBadPacket(11092,"EvtNoErr");
  //   MutrSouth->SetBadPacket(11092,"ParityErr");
  //   MutrSouth->SetBadPacket(11092,"SubsysErr");

  //    MutrSouth->SetBadPacket(11137,"FemClkErr");
  //    MutrSouth->SetBadPacket(11137,"Gl1SyncErr");

  //    MutrSouth->SetBadPacket(11154,"DCMParErr");
  //    MutrSouth->SetBadPacket(11154,"FemClkErr");
  //    MutrSouth->SetBadPacket(11154,"Gl1SyncErr");
  //    MutrSouth->SetBadPacket(11154,"EvtNoErr");
  //    MutrSouth->SetBadPacket(11154,"ParityErr");
  //    MutrSouth->SetBadPacket(11154,"SubsysErr");

  //    MutrSouth->SetBadPacket(11155,"DCMParErr");
  //    MutrSouth->SetBadPacket(11155,"FemClkErr");
  //    MutrSouth->SetBadPacket(11155,"Gl1SyncErr");
  //    MutrSouth->SetBadPacket(11155,"EvtNoErr");
  //    MutrSouth->SetBadPacket(11155,"ParityErr");
  //    MutrSouth->SetBadPacket(11155,"SubsysErr");

  GranMonDraw *PadEast = new GranMonDraw("PADEAST");
  GranMonDraw *PadWest = new GranMonDraw("PADWEST");
  PadWest->SetBadPacket(4074,"ParityErr");
  //   PadWest->SetBadPacket(4010,"FemClkErr");
  //   PadWest->SetBadPacket(4010,"Gl1SyncErr");
  //   PadWest->SetBadPacket(4010,"EvtNoErr");
  //   PadWest->SetBadPacket(4010,"ParityErr");
  GranMonDraw *RichEast = new GranMonDraw("RICHEAST");
  RichEast->SetBadPacket(6025,"EvtNoErr");
  GranMonDraw *RichWest = new GranMonDraw("RICHWEST");
  // GranMonDraw *Rpc1 = new GranMonDraw("RPC1");
  // GranMonDraw *Rpc3 = new GranMonDraw("RPC3");
  GranMonDraw *TofEast = new GranMonDraw("TOFEAST");
  TofEast->SetBadPacket(7006,"Gl1SyncErr");
  TofEast->SetBadPacket(7006,"FemClkErr");
  GranMonDraw *TofWest = new GranMonDraw("TOFWEST");
  GranMonDraw *Vtxp = NULL;
  Vtxp = new VtxpMonDraw("VTXP");
  Vtxp->SetBadPacket(24036,"SubsysErr");
  GranMonDraw *Vtxs = NULL;
  Vtxs = new VtxsMonDraw("VTXS");
//   Vtxs->SetBadPacket(24103, "SubsysErr");
//   Vtxs->SetBadPacket(24133, "SubsysErr");
  GranMonDraw *Zdc = new GranMonDraw("ZDC");
  if (Vtxp)
    {
      cl->registerHisto("Daq_VTXPSubsysStatus","DAQ");
    }
  if (Vtxs)
    {
      cl->registerHisto("Daq_VTXSSubsysStatus","DAQ");
    }

  daq->registerGranule(Acc);
  daq->registerGranule(Bbc);
  daq->registerGranule(DchEast);
  daq->registerGranule(DchWest);
  daq->registerGranule(EmcEastTop);
  daq->registerGranule(EmcWestTop);
  daq->registerGranule(EmcEastBottom);
  daq->registerGranule(EmcWestBottom);
  daq->registerGranule(ErtEast);
  daq->registerGranule(ErtWest);
  //  daq->registerGranule(Fcal);
  daq->registerGranule(Fvtx);
  daq->registerGranule(Gl1);
  daq->registerGranule(Mpc);
  daq->registerGranule(MpcExNorth);
  daq->registerGranule(MpcExSouth);
  daq->registerGranule(MuidNorth);
  daq->registerGranule(MuidSouth);
  // daq->registerGranule(MuTrigNorth);
  // daq->registerGranule(MuTrigSouth);
  daq->registerGranule(MutrNorth);
  daq->registerGranule(MutrSouth);
  daq->registerGranule(PadEast);
  daq->registerGranule(PadWest);
  daq->registerGranule(RichEast);
  daq->registerGranule(RichWest);
  // daq->registerGranule(Rpc1);
  // daq->registerGranule(Rpc3);
  daq->registerGranule(TofEast);
  daq->registerGranule(TofWest);
  daq->registerGranule(Vtxp);
  daq->registerGranule(Vtxs);
  daq->registerGranule(Zdc);
  cl->requestHistoBySubSystem("DAQ", 1);
  return ;
}

void daqDraw(const char *what = "ALL")
{
  OnlMonClient * cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("DAQ", 1);
  OnlMonDraw *daq = DaqMonDraw::instance();
  daq->Draw(what);
}

void daqPS(const char *what = "ALL")
{
  OnlMonDraw *daq = DaqMonDraw::instance();
  daq->MakePS(what);
}

void daqHtml(const char *what = "ALL")
{
  OnlMonDraw *daq = DaqMonDraw::instance();
  daq->MakeHtml(what);
}

// execute
// submitall.sh stop
// submitall.sh start
// from the online_monitoring/scripts directory
void daqOper(const int icnt = 0)
{
  OnlMonClient * cl = OnlMonClient::instance();
  DaqMonDraw *daq = DaqMonDraw::instance();
  daq->CanvasSize(0, 450, 250);
  int i = -1;
  while (i < icnt)
    {
      if (icnt > 0)
        {
          i++;
        }
      cl->requestHistoBySubSystem("DAQ", 1);
      daq->Draw();
      gSystem->Sleep(30000);
    }
  cout << "ENDING, TYPE  root -l daqoperator.C to restart" << endl;
}
