void run_tof(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonltofmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  OnlMon *m = new TOFMon();

  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");

  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  start_server(prdffile);
  return;
}

void tofDrawInit(int online = 0)
{
  gSystem->Load("libonltofmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();

  // register histos we want with monitor name
  cl->registerHisto("htofinfo", "TOFMON");
  cl->registerHisto("htof210", "TOFMON");
  cl->registerHisto("htof220", "TOFMON");
  cl->registerHisto("htof230", "TOFMON");
  //cl->registerHisto("htof410", "TOFMON");
  cl->registerHisto("htof420", "TOFMON");
  //cl->registerHisto("htof520", "TOFMON");
  cl->registerHisto("htof530", "TOFMON");
  CreateHostList(online);

   cl->requestHistoBySubSystem("TOFMON", 1); // get my histos from server, the second parameter = 1 says I know they are all on the same node
  OnlMonDraw *tofmon = new TOFMonDraw(); // create Drawing Object
  cl->registerDrawer(tofmon); // register with client framework
}

void tofDraw()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("TOFMON", 1);
  cl->Draw("TOFMON");
}

void tofHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->MakeHtml("TOFMON");
}

void tofPS()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->MakePS("TOFMON");
}
