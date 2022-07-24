void run_tofw(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonltofwmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  OnlMon *m = new TOFWMon();
  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  start_server(prdffile);
  return;
}

void tofwDrawInit(int online = 0)
{
  gSystem->Load("libonltofwmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();

  // register histos we want with monitor name
  cl->registerHisto("htofwinfo", "TOFWMON");
  cl->registerHisto("htofw210", "TOFWMON");
  cl->registerHisto("htofw220", "TOFWMON");
  cl->registerHisto("htofw230", "TOFWMON");
  //cl->registerHisto("htofw410", "TOFWMON");
  cl->registerHisto("htofw420", "TOFWMON");
  //cl->registerHisto("htofw520", "TOFMON");
  cl->registerHisto("htofw530", "TOFWMON");
  CreateHostList(online);

  cl->requestHistoBySubSystem("TOFWMON", 1); // get my histos from server, the second parameter = 1 says I know they are all on the same node
  OnlMonDraw *tofwmon = new TOFWMonDraw(); // create Drawing Object
  cl->registerDrawer(tofwmon); // register with client framework
}

void tofwDraw()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("TOFWMON", 1);
  cl->Draw("TOFWMON");
}

void tofwHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->MakeHtml("TOFWMON");
}

void tofwPS()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->MakePS("TOFWMON");
}
