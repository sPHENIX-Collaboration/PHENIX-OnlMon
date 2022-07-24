void loadall()
{
  gSystem->Load("libonlmonclient.so");
  gSystem->Load("libonlaccmon.so");
  gSystem->Load("libonlbbclvl1mon.so");
  gSystem->Load("libonlbbcmon.so");
  gSystem->Load("libonldaqmon.so");
  gSystem->Load("libonldchmon.so");
  gSystem->Load("libonlemcalmon.so");
  gSystem->Load("libonlertmon.so");
  gSystem->Load("libonlertlvl1mon.so");
  //  gSystem->Load("libonlfcalmon.so");
  gSystem->Load("libonlfvtxmon.so");
  gSystem->Load("libonlgl1mon.so");
  //  gSystem->Load("libonllocalpolmon.so");
  gSystem->Load("libonlmcrmon.so");
  gSystem->Load("libonlmpcmon.so");
  gSystem->Load("libonlmpcexmon.so");
  gSystem->Load("libonlmui_mon.so");
  gSystem->Load("libonlmuidlvl1mon.so");
  gSystem->Load("libonlmutrmon.so");
  // gSystem->Load("libonlmutrgmon.so");
  // gSystem->Load("libonlmutriglvl1mon.so");
  gSystem->Load("libonlpadmon.so");
  gSystem->Load("libonlpbglumon.so");
  gSystem->Load("libonlpktsizemon.so");
  //  gSystem->Load("libonlrecomon.so");
  gSystem->Load("libonlrichmon.so");
  //  gSystem->Load("libonlrpcmon.so");
  //  gSystem->Load("libonlrpctimetester.so");
  // gSystem->Load("libonlspinmon.so");
  // gSystem->Load("libonlspin2mon.so");
  gSystem->Load("libonlsvxpixelmon.so");
  gSystem->Load("libonlsvxstripmon.so");
  gSystem->Load("libonlsvxvertex.so");
  gSystem->Load("libonltofmon.so");
  gSystem->Load("libonltofwmon.so");
  gSystem->Load("libonlzdcmon.so");
  gSystem->Load("libonlzdclvl1mon.so");
}

void registerDaq()
{
#define ngran 28
  char *hname[3];
  char tmp[100];
  char * Granules[ngran] =
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
  gSystem->Load("libonldaqmon.so");
  gSystem->Load("libonlmonclient.so");
  OnlMonClient *cl = OnlMonClient::instance();
  for (short int i = 0; i < ngran; i++)
    {
      for (short int j = 0; j < 3; j++)
        {
          sprintf(tmp, "%s%s", Granules[i], hname[j]);
          cl->registerHisto(tmp, "DAQ");
          cout << "registering " << tmp << endl;
        }
    }
  GranMonDraw *Vtxp = NULL; // used later to check if it was created

  DaqMonDraw *daq = DaqMonDraw::instance();
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
  GranMonDraw *Mpc = new GranMonDraw("MPC");
  GranMonDraw *MpcExNorth = new GranMonDraw("MPCEXNORTH");
  GranMonDraw *MpcExSouth = new GranMonDraw("MPCEXSOUTH");
  GranMonDraw *MuidNorth = new GranMonDraw("MUIDNORTH");
  GranMonDraw *MuidSouth = new GranMonDraw("MUIDSOUTH");
  // GranMonDraw *MuTrigNorth = new GranMonDraw("MUTRIGNORTH");
  // GranMonDraw *MuTrigSouth = new GranMonDraw("MUTRIGSOUTH");
  GranMonDraw *MutrNorth = new GranMonDraw("MUTRNORTH");
  //   MutrNorth->SetBadPacket(11315,"EvtNoErr");
  //   MutrNorth->SetBadPacket(11315,"FemClkErr");
  //   MutrNorth->SetBadPacket(11315,"DCMParErr");
  //   MutrNorth->SetBadPacket(11315,"SubsysErr");
  GranMonDraw *MutrSouth = new GranMonDraw("MUTRSOUTH");
  GranMonDraw *PadEast = new GranMonDraw("PADEAST");
  GranMonDraw *PadWest = new GranMonDraw("PADWEST");
  GranMonDraw *RichEast = new GranMonDraw("RICHEAST");
  GranMonDraw *RichWest = new GranMonDraw("RICHWEST");
  // GranMonDraw *Rpc1 = new GranMonDraw("RPC1");
  // GranMonDraw *Rpc3 = new GranMonDraw("RPC3");
  GranMonDraw *TofEast = new GranMonDraw("TOFEAST");
  GranMonDraw *TofWest = new GranMonDraw("TOFWEST");
  GranMonDraw *Vtxp = NULL;
  Vtxp = new VtxpMonDraw("VTXP");
  GranMonDraw *Vtxs = NULL;
  Vtxs = new VtxsMonDraw("VTXS");
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
  // daq->registerGranule(MuTrigSouth);
  // daq->registerGranule(MuTrigNorth);
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
  return ;
}
