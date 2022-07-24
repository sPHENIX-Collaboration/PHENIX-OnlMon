void run_reco(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlrecomon.so");
  gSystem->Load("libfun4allfuncs.so");
  //gSystem->Load("librxnp_subsysreco.so");   
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  RecoMon *reco = new RecoMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  reco->set_recorunnumber(340515);
  reco->AddTrigger("BBCLL1(>0 tubes)");
  //  reco->Verbosity(2);
  se->registerMonitor(reco);       // register subsystem Monitor with Framework
  SubsysReco *head    = new HeadReco();
  SubsysReco *trig    = new TrigReco();
  SubsysReco *bbc     = new BbcReco();
  SubsysReco *zdc     = new ZdcReco();
  SubsysReco *ert     = new ErtReco();
  SubsysReco *t0      = new T0Reco();
  PadReco *pad     = new PadReco();
  pad->NoRawDataCheck();
  SubsysReco *vtx     = new VtxReco();
  //  SubsysReco *hbd     = new HbdReco();
  DchReco *dch     = new DchReco();
  dch->NoRawDataCheck();
  SubsysReco *emc     = new EmcReco3();
  //SubsysReco *rxnp    = new RxnpReco();
  //SubsysReco *rp      = new RpSumXYReco();
  //  SubsysReco *cgl     = new CglReco();
  SubsysReco *histos  = new RecoHistos();
  reco->registerSubsystem(head);
  reco->registerSubsystem(trig);
  reco->registerSubsystem(bbc);
  reco->registerSubsystem(zdc);
  reco->registerSubsystem(ert);
  reco->registerSubsystem(t0);
  reco->registerSubsystem(pad);
  //reco->registerSubsystem(hbd);
  reco->registerSubsystem(vtx);
  reco->registerSubsystem(dch);
  reco->registerSubsystem(emc);
  reco->registerSubsystem(new EmcClusterContainerResurrector());
  //reco->registerSubsystem(rxnp);
  //reco->registerSubsystem(rp);
  //reco->registerSubsystem(cgl);
  reco->registerSubsystem(histos);

  start_server(prdffile);

  return ;
}

void recoDrawInit(int online = 0)
{
  gSystem->Load("libonlrecomon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();
  cl->registerHisto("reco_bbc_pc1","RECOMON");
  cl->registerHisto("reco_bbc_pc3","RECOMON");
  cl->registerHisto("reco_bbc_qtot","RECOMON");
  cl->registerHisto("reco_bbc_ntot","RECOMON");
  cl->registerHisto("reco_bbc_zvtx","RECOMON");
  cl->registerHisto("reco_bbc_tzero","RECOMON");
  cl->registerHisto("reco_bbc_nns","RECOMON");
  cl->registerHisto("reco_bbc_qns","RECOMON");
  cl->registerHisto("reco_dch_ntracks","RECOMON");
  cl->registerHisto("reco_emc_nclus","RECOMON");
  cl->registerHisto("reco_pc1_nclus","RECOMON");
  cl->registerHisto("reco_pc2_nclus","RECOMON");
  cl->registerHisto("reco_pc3_nclus","RECOMON");
  cl->registerHisto("reco_emc_energy","RECOMON");
  cl->registerHisto("reco_dch_mom","RECOMON");
  cl->registerHisto("reco_dch_pc1","RECOMON");
  //cl->registerHisto("reco_rxn_qtot","RECOMON");
  //cl->registerHisto("reco_bbc_rxn","RECOMON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("RECOMON", 1);

  // create Drawing Object; MyMonDraw inherits from OnlMonDraw
  OnlMonDraw *reco = new RecoMonDraw();
  // register with client framework; this pops up a window
  cl->registerDrawer(reco);
  return ;
}

void recoDraw(const char *what="FIRST")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("RECOMON", 1);         // update histos
  cl->Draw("RECOMON",what);                     // Draw Histos of registered Drawers
  return ;
}

void recoPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("RECOMON");                         // create PS files
  return ;
}

void recoHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("RECOMON");                       // create html output
  return ;
}
