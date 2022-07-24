void run_zdclvl1(char *prdffile = "lvl1data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlzdclvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object, vertex limits = 148.5 cm, 29.7 cm
  OnlMon *m = new ZdcLvl1Mon(0x1,0x7F,0x1,0x7F,0x2D,0x9);      
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void zdclvl1DrawInit(int online = 0)
{
  gSystem->Load("libonlzdclvl1mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("ZdcLvl1_ZDCVertex", "ZDCLVL1MON");
  cl->registerHisto("ZdcLvl1_ZDCSimVertex", "ZDCLVL1MON");
  cl->registerHisto("ZdcLvl1_ZDCVertexDiff", "ZDCLVL1MON");
  cl->registerHisto("ZdcLvl1_ZDCVertexCutA", "ZDCLVL1MON");
  cl->registerHisto("ZdcLvl1_ZDCVertexCutB", "ZDCLVL1MON");
  cl->registerHisto("ZdcLvl1_ZDCVertexNoCut", "ZDCLVL1MON");

  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("ZDCLVL1MON", 1);
  OnlMonDraw *zdclvl1mon = new ZdcLvl1MonDraw();    // create Drawing Object
  cl->registerDrawer(zdclvl1mon);              // register with client framework
}

void zdclvl1Draw()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ZDCLVL1MON",1);         // update histos
  cl->Draw("ZDCLVL1MON");                            // Draw Histos of registered Drawers
}


void zdclvl1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("ZDCLVL1MON");
}

void zdclvl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("ZDCLVL1MON");
}
