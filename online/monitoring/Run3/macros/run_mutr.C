void run_mutr(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutrmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  //  OnlMon *m = new MutrMon();
  // the following 2 lines replaces the line above for tmp testing
  MutrMon *m = new MutrMon();
  m->getCalibrations(2009, 3, 22, 14, 0, 0); // valid until end of run
  m->AddTrigger("BBCLL1(>0 tubes)");
  
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  start_server(prdffile);
  return ;
}

void mutrDrawInit(const int online = 0)
{
  gSystem->Load("libuti.so"); // recoConsts has moved here 
  gSystem->Load("libonlmutrmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  CreateHostList(online);
  MutrMonDraw *mutrmon = new MutrMonDraw();    // create Drawing Object
  cl->registerDrawer(mutrmon);              // register with client framework
  mutrmon->registerHists();
}

void mutrDraw()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MUTRMON", 1);         // update histos
  cl->Draw("MUTRMON");                            // Draw Histos of registered Drawers
}

void mutrPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MUTRMON");                            // Draw Histos of registered Drawers
}

void mutrHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MUTRMON");                            // Draw Histos of registered Drawers
}
