// Phenix Online Monitoring System
//
// Macro loader for starting Online Monitoring GUI Framework
//
// Author:  Cody McCain (mcm99c@acu.edu)
// Date:    January 2, 2003
//
// See run_Poms.C.README


void StartPoms()
{
  gSystem->Load("libpoms.so");
#define NGRANS 28
  char *Granules[NGRANS] =
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
  PomsMainFrame* pmf;
  pmf = PomsMainFrame::Instance();
  
  // Register SubSystems
  SubSystem *acc = new SubSystem("ACC", "acc");
  acc->AddAction("accDraw(\"ALLADCTDC\")", "Aerogel adc/tdc");
  acc->AddAction("accDraw(\"TDCpedestal\")", "TDCpedestal");
  acc->AddAction(new SubSystemActionDrawPS(acc));
  pmf->RegisterSubSystem(acc);
  
  SubSystem *bbc = new SubSystem("BBC", "bbc");
  bbc->AddAction("bbcDraw(\"VertexMonitor\")", "Bbc Vertex Monitor");
  bbc->AddAction("bbcDraw(\"BbcMonitor\")", "Bbc PMT Monitor");
  bbc->AddAction(new SubSystemActionDrawPS(bbc));
  pmf->RegisterSubSystem(bbc);  

  pmf->RegisterSubSystem("BBCLL1", "bbclvl1");
  
  SubSystem *daq = new SubSystem("daq", "daq");
  daq->AddAction("daqDraw()", "Granule Overview");
  daq->AddAction("daqDraw(\"GLOBAL\")", "fraction corrupt events");
  daq->AddAction("daqDraw(\"LOWRUN\")", "Low Runnumber Events");
  daq->AddAction("daqDraw(\"DAQ%VTXP%SUBSYS\")", "Vtx Pixel Special");
  daq->AddAction("daqDraw(\"DAQ%VTXS%RCCBLK\")", "Vtx Strip RCC/BCLK");
  daq->AddAction("daqDraw(\"DAQ%VTXS%HYBRID\")", "Vtx Strip Hybrids");
  daq->AddAction("daqDraw(\"DAQ%VTXS%INVALIDCELLID0\")", "VtxS Inv CellId RCC0-1");
  daq->AddAction("daqDraw(\"DAQ%VTXS%INVALIDCELLID1\")", "VtxS Inv CellId RCC2-3");
  daq->AddAction("daqDraw(\"DAQ%VTXS%INVALIDCELLID2\")", "VtxS Inv CellId RCC4-5");
  daq->AddAction("daqDraw(\"DAQ%VTXS%CELLID0\")", "VtxS Bad CellId RCC0-1");
  daq->AddAction("daqDraw(\"DAQ%VTXS%CELLID1\")", "VtxS Bad CellId RCC2-3");
  daq->AddAction("daqDraw(\"DAQ%VTXS%CELLID2\")", "VtxS Bad CellId RCC4-5");
  char tmp[100];
  char tmp2[100];
  char *aargh = "%";
  for (int i = 0; i < NGRANS;i++)
    {
      printf("adding granule %s\n",Granules[i]);
      sprintf(tmp, "daqDraw(\"DAQ%s%s\")", aargh, Granules[i]);
      sprintf(tmp2, "%s", Granules[i]);
      printf("%s %s\n", tmp, tmp2);
      daq->AddAction(tmp, tmp2);
    }
  daq->AddAction(new SubSystemActionDrawPS(daq));
  pmf->RegisterSubSystem(daq);

  pmf->RegisterSubSystem("DCH","dch");

  SubSystem *emc  = new SubSystem("EMCal", "emcal");
  emc->AddAction("emcalDraw()", "Status");
  emc->AddAction("emcalDraw(\"PI0\")", "online pi0s");
  emc->AddAction(new SubSystemActionDrawPS(emc));
  pmf->RegisterSubSystem(emc);

  SubSystem *ert = new SubSystem("ERT", "ert");
  ert->AddAction("ertDraw()", "Hot Channels");
  ert->AddAction("ertDraw(\"RF\")", "Rejection Factor");
  ert->AddAction(new SubSystemActionDrawPS(ert));
  pmf->RegisterSubSystem(ert);

  SubSystem *ertlvl1 = new SubSystem("ERTLVL1", "ertlvl1");
  ertlvl1->AddAction("ertlvl1Draw(\"EAST\")", "East Arm");
  ertlvl1->AddAction("ertlvl1Draw(\"WEST\")", "West Arm");
  ertlvl1->AddAction("ertlvl1Draw(\"ROCeffE\")", "ROC Efficiencies East Arm");
  ertlvl1->AddAction("ertlvl1Draw(\"ROCeffW\")", "ROC Efficiencies West Arm");
  ertlvl1->AddAction(new SubSystemActionDrawPS(ertlvl1));
  pmf->RegisterSubSystem(ertlvl1);

  //  pmf->RegisterSubSystem("FCAL", "fcal");

  SubSystem *fvtx = new SubSystem("FVTX", "fvtx");
  fvtx->AddAction("fvtxDraw(\"fvtxSummary\")","Status");
  fvtx->AddAction("fvtxDraw(\"fvtxYieldsByPacket\")","Packet Info");
  fvtx->AddAction("fvtxDraw(\"fvtxYields\")","Hits per wedge");
  fvtx->AddAction("fvtxDraw(\"fvtxAdc\")","ADC");
  fvtx->AddAction("fvtxDraw(\"fvtxChipVsChannelW\")","Chip Vs Channel West");
  fvtx->AddAction("fvtxDraw(\"fvtxChipVsChannelE\")","Chip Vs Channel East");
  fvtx->AddAction("fvtxDraw(\"fvtxYieldsByPacketVsTime\")","Packet Vs t 30s");
  fvtx->AddAction("fvtxDraw(\"fvtxYieldsByPacketVsTimeShort\")","Packet Vs t 1s");
  fvtx->AddAction(new SubSystemActionDrawPS(fvtx));
  pmf->RegisterSubSystem(fvtx);

  SubSystem *gl1 = new SubSystem("GL1", "gl1");
  gl1->AddAction("gl1Draw()", "Bandwidth and rejection");
  gl1->AddAction("gl1Draw(\"bcross0\")", "trigbits 1-8");
  gl1->AddAction("gl1Draw(\"bcross1\")", "trigbits 9-16");
  gl1->AddAction("gl1Draw(\"bcross2\")", "trigbits 17-24");
  gl1->AddAction("gl1Draw(\"bcross3\")", "trigbits 25-32");
  gl1->AddAction(new SubSystemActionDrawPS(gl1));
  pmf->RegisterSubSystem(gl1);

  // SubSystem *locpol = new SubSystem("Online Polarimetry", "localpol");
  // locpol->AddAction("localpolDraw(\"Canvas0\")", "Status"); 
  // locpol->AddAction("localpolDraw(\"Canvas1\")", "Expert"); 
  // locpol->AddAction("localpolDraw(\"Canvas2\")", "XingShift"); 
  // locpol->AddAction(new SubSystemActionDrawPS(locpol));
  // pmf->RegisterSubSystem(locpol);
  
  pmf->RegisterSubSystem("MCR", "mcr");
  
  SubSystem *mpc = new SubSystem("MPC", "mpc");
  mpc->AddAction("mpcDraw(\"POMS1\")", "Energy/Timing");
  mpc->AddAction("mpcDraw(\"POMS2\")", "Crystal Energies");
  mpc->AddAction(new SubSystemActionDrawPS(mpc));  
  pmf->RegisterSubSystem(mpc);
  
  SubSystem *mpcex = new SubSystem("MPCEX", "mpcex");
  mpcex->AddAction("mpcexDraw(\"REV\")","Dashboard");
  mpcex->AddAction("mpcexDraw(\"SEN\")","HealthMonitor");
  mpcex->AddAction("mpcexDraw(\"ENE\")","EnergyMonitor1");
  mpcex->AddAction("mpcexDraw(\"ENE2\")","EnergyMonitor2");
  mpcex->AddAction("mpcexDraw(\"ENE3\")","EnergyMonitor3");
  mpcex->AddAction("mpcexDraw(\"TRG\")","TriggerScan");
  mpcex->AddAction("mpcexDraw(\"RAW\")","PacketScan");
  mpcex->AddAction(new SubSystemActionDrawPS(mpcex));
  pmf->RegisterSubSystem(mpcex);

  pmf->RegisterSubSystem("MUID", "mui");
  
  SubSystem *muidlvl1 = new SubSystem("MUIDLL1", "muidlvl1");
  muidlvl1->AddAction("muidlvl1Draw(\"NORTH\")", "North Arm");
  muidlvl1->AddAction("muidlvl1Draw(\"SOUTH\")", "South Arm");
  muidlvl1->AddAction(new SubSystemActionDrawPS(muidlvl1));
  pmf->RegisterSubSystem(muidlvl1);
  
  pmf->RegisterSubSystem("MUTR", "mutr");

  // SubSystem *mutrig = new SubSystem("MUTRIG", "mutrg");
  // mutrig->AddAction("mutrgDraw()","Summary");
  // mutrig->AddAction("mutrgDraw(\"NORTH\")","North Arm");
  // mutrig->AddAction("mutrgDraw(\"SOUTH\")","South Arm");
  // mutrig->AddAction("mutrgDraw(\"EFFNORTH\")","Efficiency North");
  // mutrig->AddAction("mutrgDraw(\"EFFSOUTH\")","Efficiency South");
  // mutrig->AddAction("mutrgDraw(\"FAKENORTH\")","Fake Rate North");
  // mutrig->AddAction("mutrgDraw(\"FAKESOUTH\")","Fake Rate South");
  // mutrig->AddAction("mutrgDraw(\"ERRORS\")","Errors");
  // mutrig->AddAction(new SubSystemActionDrawPS(mutrig));
  // pmf->RegisterSubSystem(mutrig);

  // SubSystem *mutriglvl1 = new SubSystem("MUTRIGLVL1", "mutriglvl1");
  // mutriglvl1->AddAction("mutriglvl1Draw(\"NORTH\")","North Arm");
  // mutriglvl1->AddAction("mutriglvl1Draw(\"SOUTH\")","South Arm");
  // mutriglvl1->AddAction(new SubSystemActionDrawPS(mutriglvl1));
  // pmf->RegisterSubSystem(mutriglvl1);

  pmf->RegisterSubSystem("PAD", "pad");

  pmf->RegisterSubSystem("PACKETSIZE", "pktsize");

//  pmf->RegisterSubSystem("PBGL U", "pbglu");

//   SubSystem *reco = new SubSystem("RECO", "reco");
//   reco->AddAction("recoDraw(\"FIRST\")", "Bbc 1st");
//   reco->AddAction("recoDraw(\"SECOND\")", "Bbc 2nd");
//   reco->AddAction("recoDraw(\"THIRD\")", "Dch Emc");
//   reco->AddAction("recoDraw(\"FOURTH\")", "PC Rxnp");
//   reco->AddAction(new SubSystemActionDrawPS(reco));
//   pmf->RegisterSubSystem(reco);

  pmf->RegisterSubSystem("RICH", "rich");

  // pmf->RegisterSubSystem("RPC", "rpc");

  // pmf->RegisterSubSystem("RPCTIMING", "rpctiming");

  // SubSystem *spin = new SubSystem("Spin", "spin2");
  // spin->AddAction("spin2Draw(\"Spin2MonDraw_Shifter\")", "ShiftDisplay");
  // spin->AddAction("spin2Draw(\"Spin2MonDraw_Expert\")", "Expert");
  // spin->AddAction(new SubSystemActionDrawPS(spin));  
  // pmf->RegisterSubSystem(spin);

  // SubSystem *spinold = new SubSystem("SpinOld", "spin");
  // spinold->AddAction("spinDraw()", "Status");
  // spinold->AddAction("spinDraw(\"GL1Psum\")", "GL1Psum");
  // spinold->AddAction(new SubSystemActionDrawPS(spinold));
  // pmf->RegisterSubSystem(spinold); 

  SubSystem *svxpixel = new SubSystem("VTXP", "svxpixel");
  svxpixel->AddAction("svxpixelDraw(\"SvxPixelLadderChipWest\")", "West 2D");
  svxpixel->AddAction("svxpixelDraw(\"SvxPixelLadderChipEast\")", "East 2D");
  svxpixel->AddAction("svxpixelDraw(\"SvxPixelLadderWest\")", "West 1D");
  svxpixel->AddAction("svxpixelDraw(\"SvxPixelLadderEast\")", "East 1D");
  svxpixel->AddAction("svxpixelDraw(\"SvxPixelStabilityWest\")", "West Stability");
  svxpixel->AddAction("svxpixelDraw(\"SvxPixelStabilityEast\")", "East Stability");
  svxpixel->AddAction(new SubSystemActionDrawPS(svxpixel));  
  pmf->RegisterSubSystem(svxpixel);

  SubSystem *svxstrip = new SubSystem("VTXS", "svxstrip");
  svxstrip->AddAction("svxstripDraw(\"SvxStripMonLive\")", "Live");
  svxstrip->AddAction("svxstripDraw(\"SvxStripMonNormRCC\")", "Norm. Yield");
  svxstrip->AddAction("svxstripDraw(\"SvxStripMonCellIDEast\")", "Cell East");
  svxstrip->AddAction("svxstripDraw(\"SvxStripMonCellIDWest\")", "Cell West");
  svxstrip->AddAction("svxstripDraw(\"SvxStripMonRCCBClockError\")", "RCC BClock Error");
  svxstrip->AddAction(new SubSystemActionDrawPS(svxstrip));
  pmf->RegisterSubSystem(svxstrip);
  
  //  pmf->RegisterSubSystem("SVXVTX", "svxvertex");

  pmf->RegisterSubSystem("TOFEAST", "tof");
  
  pmf->RegisterSubSystem("TOFWEST", "tofw");
  
  //    pmf->RegisterSubSystem("ZDC", "zdc");

   SubSystem *zdc = new SubSystem("ZDC", "zdc");
   zdc->AddAction("zdcDraw()", "Zdc Vertex");
   //   zdc->AddAction("zdcDraw(\"VETO\")", "zdc Veto counter");
   zdc->AddAction(new SubSystemActionDrawPS(zdc));  
   pmf->RegisterSubSystem(zdc);  
  
  pmf->RegisterSubSystem("ZDCLVL1", "zdclvl1");
  
  pmf->Draw();
  
}
