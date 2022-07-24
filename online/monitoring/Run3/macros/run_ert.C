void run_ert(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlertmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new ERTMaskMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance();// get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void ertDrawInit(const int online = 0)
{
  gSystem->Load("libonlertmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name

  //----------------------------------------------------------------------
  cl->registerHisto("ert_Evtcount", "ERTMaskMON");

  //----------------------------------------------------------------------
  
  cl->registerHisto("ert_hsm_0", "ERTMaskMON");
  cl->registerHisto("ert_hsm_1", "ERTMaskMON");
  cl->registerHisto("ert_hsm_2", "ERTMaskMON");
  cl->registerHisto("ert_hsm_3", "ERTMaskMON");
  cl->registerHisto("ert_hsm_4", "ERTMaskMON");
  
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("ERTMaskMON", 1);
  OnlMonDraw *ertmon = new ERTMaskMonDraw();    // create Drawing Object
  cl->registerDrawer(ertmon);              // register with client framework
  return;
}

//void ertDraw(const char *what = "LIN")
void ertDraw(const char *what = "LOG")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ERTMaskMON", 1); // update histos
  cl->Draw("ERTMaskMon",what);            // Draw Histos of registered Drawers
  return;
}

void ertPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("ERTMaskMon");                     // Create PS files
  return;
}

void ertHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("ERTMaskMon");                   // Create html output
  return;
}

