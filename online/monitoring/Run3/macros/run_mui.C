void run_mui(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmui_mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  muiMonitor *m = new muiMonitor();      // create subsystem Monitor object
  m->getOptions()->setVerbose(0);
  // ONLMONBBCLL1 (min bias definition) is added in start_server method below
  m->AddTrigger("ONLMONBBCLL1");
  // this adds a cut on live triggers. The cut is done on the monitor process
  // end, so you pay the penalty to transfer events but it will still increase
  // the rate if you are limited by the number of incoming events 
  // (if bbcll1 is heavily downscaled in favor of the bbcll1 novtx)
  //  m->AddLiveTrigger("BBCLL1(>0 tubes)");


  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

/////////////////////////////////////////////////////////////////
void muiDrawInit(const int online = 0)
{
  gSystem->Load("libonlmui_mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  //  cl->requestHistoBySubSystem("MUIDMONITOR", 1);
  muiMonDraw *muimon = new muiMonDraw();    // create Drawing Object
  muimon->UseReferences();                  // comment out to stop using reference histograms
  cl->registerDrawer(muimon);               // register with client framework
}

/////////////////////////////////////////////////////////////////
void muiDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MUIDMONITOR", 1);   // update histos
  cl->Draw("MUIDMONITOR",what);                         // Draw Histos of registered Drawers
}

/////////////////////////////////////////////////////////////////
void muiPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MUIDMONITOR");                    // Create PS files
}

/////////////////////////////////////////////////////////////////
void muiHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MUIDMONITOR");                  // Create html output
}










