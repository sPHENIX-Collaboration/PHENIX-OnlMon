void run_ertlvl1(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlertlvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new ErtLvl1Mon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void ertlvl1DrawInit(const int online = 0)
{
  gSystem->Load("libonlertlvl1mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("ErtLvl1_FFAE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFAW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFBE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFBW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFCE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFCW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TBTE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TBTW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_ERONE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_ERONW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TERONE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TERONW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFASimE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFASimW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFBSimE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFBSimW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFCSimE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_FFCSimW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TBTSimE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TBTSimW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_ERONSimE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_ERONSimW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TERONSimE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_TERONSimW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocEffE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocEffW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocEntriesE", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocEntriesW", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocWord0", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocWord1", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocWordERTLL10", "ERTLVL1MON");
  cl->registerHisto("ErtLvl1_hRocWordERTLL11", "ERTLVL1MON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("ERTLVL1MON", 1);
  OnlMonDraw *ertlvl1mon = new ErtLvl1MonDraw();    // create Drawing Object
  cl->registerDrawer(ertlvl1mon);              // register with client framework
}

void ertlvl1Draw(const char *what = "EAST")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ERTLVL1MON",1); // update histos
  cl->Draw("ERTLVL1MON",what);                 // Draw Histos of registered Drawers
}


void ertlvl1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("ERTLVL1MON");
}

void ertlvl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("ERTLVL1MON");
}
