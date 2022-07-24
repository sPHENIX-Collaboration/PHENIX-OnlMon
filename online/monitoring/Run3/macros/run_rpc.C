void run_rpc(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlrpcmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new RpcMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}


void rpcDrawInit(const int online = 0)
{
  gSystem->Load("libonlrpcmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  //  cl->registerHisto("mymon_hist1", "RPCMON");
  //  cl->registerHisto("mymon_hist2", "RPCMON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  OnlMonDraw *rpcmon = new RpcMonDraw();    // create Drawing Object
  cl->registerDrawer(rpcmon);              // register with client framework
  cl->requestHistoBySubSystem("RPCMON", 1);
}

void rpcDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("RPCMON");         // update histos
  cl->Draw("RPCMON",what);                       // Draw Histos of registered Drawers
}

void rpcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("RPCMON");                          // Create PS files
  return;
}

void rpcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("RPCMON");                        // Create html output
  return;
}

