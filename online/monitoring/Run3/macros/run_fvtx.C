void run_fvtx(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlfvtxmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new FvtxMon();

  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  start_server(prdffile);
  return ;
}

void fvtxDrawInit(const int online = 0)
{
  gSystem->Load("libuti.so"); // recoConsts has moved here 
  gSystem->Load("libonlfvtxmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  cl->Verbosity(0);
  CreateHostList(online);
  FvtxMonDraw *fvtxmon = new FvtxMonDraw();    // create Drawing Object
  cl->registerDrawer(fvtxmon);              // register with client framework
  fvtxmon->registerHists();
}

void fvtxDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("FVTXMON", 1);    // update histos
  cl->Draw("FVTXMON",what);                     // Draw Histos of registered Drawers
}

void fvtxPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("FVTXMON");                            // Draw Histos of registered Drawers
}

void fvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("FVTXMON");                            // Draw Histos of registered Drawers
}
