void run_bbclvl1(const char *prdffile = "lvl1data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlbbclvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new BbcLvl1Mon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void bbclvl1DrawInit(const int online = 0)
{
  gSystem->Load("libonlbbclvl1mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("BbcLvl1_VertHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_Vert2Hist","BBCLVL1MON"); 
  cl->registerHisto("BbcLvl1_AverHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_SHitsHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_NHitsHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_VertHistErr", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_AverHistErr", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_SHitsHistErr", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_NHitsHistErr", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_STimeHistErr", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_NTimeHistErr", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_NChAllHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_SChAllHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_NChBadHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_SChBadHist", "BBCLVL1MON");
  cl->registerHisto("BbcLvl1_DataError", "BBCLVL1MON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("BBCLVL1MON", 1);
  OnlMonDraw *bbclvl1mon = new BbcLvl1MonDraw();    // create Drawing Object
  cl->registerDrawer(bbclvl1mon);              // register with client framework
}

void bbclvl1Draw()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("BBCLVL1MON", 1);         // update histos
  cl->Draw("BBCLVL1MON");                            // Draw Histos of registered Drawers
}


void bbclvl1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("BBCLVL1MON");
}

void bbclvl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("BBCLVL1MON");
}

