void run_gl1(const char *prdffile = "data.prdf")
{

  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlgl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new Gl1Mon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

void gl1DrawInit(const int online = 0)
{

  gSystem->Load("libonlgl1mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("Gl1_RawHist", "GL1MON");
  cl->registerHisto("Gl1_LiveHist", "GL1MON");
  cl->registerHisto("Gl1_ScaledHist", "GL1MON");
  cl->registerHisto("Gl1_ScaledHist_BBCLL1", "GL1MON");
  cl->registerHisto("Gl1_ScaledHist_ZDC", "GL1MON");
  cl->registerHisto("Gl1_ScalerRawHist", "GL1MON" );
  cl->registerHisto("Gl1_ScalerLiveHist", "GL1MON" );
  cl->registerHisto("Gl1_ScalerScaledHist", "GL1MON" );
  cl->registerHisto("Gl1_RejectHist", "GL1MON" );
  cl->registerHisto("Gl1_ExpectRejectHist", "GL1MON" );
  cl->registerHisto("Gl1_bandwidthplot", "GL1MON" );
  cl->registerHisto("Gl1_CrossCounter_bbcll1", "GL1MON" );
  cl->registerHisto("Gl1_CrossCounter_zdc", "GL1MON" );
  cl->registerHisto("Gl1_CrossCounter_bbcll1_nvc", "GL1MON" );
  cl->registerHisto("Gl1_RejectRatio_1", "GL1MON" );
  cl->registerHisto("Gl1_RejectRatio_2", "GL1MON" );
  cl->registerHisto("Gl1_StorageHist", "GL1MON" );
  cl->registerHisto("Gl1_present_count", "GL1MON" );
  cl->registerHisto("Gl1_present_rate", "GL1MON" );

  CreateHostList(online);

  cl->requestHistoBySubSystem("GL1MON", 1);
  OnlMonDraw *gl1mon = new Gl1MonDraw();    // create Drawing Object
  cl->registerDrawer(gl1mon);              // register with client framework
  return;
}

void gl1Draw(const char *what = "REJECTION")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("GL1MON", 1);         // update histos
  cl->Draw("GL1MON",what);                     // Draw Histos of registered Drawers
  return;
}

void gl1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("GL1MON");                        // Create PS files
  return;
}

void gl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("GL1MON");                      // Create html output
  return;
}

