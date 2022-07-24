void run_dch(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonldchmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  DchMon *dchmon = new DchMon();
  dchmon->set_averages_on_the_fly(1);
  dchmon->set_running_average(0);
  // this is how to set a trigger selection
  // multiple triggers can be selected
  dchmon->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(dchmon);
  start_server(prdffile);
  return;
}

void dchDrawInit(const int online = 0)
{
  gSystem->Load("libonlmonclient.so");
  gSystem->Load("libonldchmon.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *client = OnlMonClient::instance();
  // register histos we want with monitor name
  client->registerHisto("h_dc1norm" , "DCHMON");
  client->registerHisto("h_dc2norm" , "DCHMON");
  client->registerHisto("dch_ek1_1" , "DCHMON");
  client->registerHisto("dch_ek2_2" , "DCHMON");
  client->registerHisto("dch_ek3_3" , "DCHMON");
  client->registerHisto("dch_ek4_4" , "DCHMON");
  client->registerHisto("dc_info" , "DCHMON");
  client->registerHisto("dc_dens_00" , "DCHMON");
  client->registerHisto("dc_dens_01" , "DCHMON");
  client->registerHisto("dc_dens_10" , "DCHMON");
  client->registerHisto("dc_dens_11" , "DCHMON");
  client->registerHisto("dc_good_distro" , "DCHMON");
  client->registerHisto("dc_dead_distro" , "DCHMON");
  client->registerHisto("dc_noisy_distro", "DCHMON");
  client->registerHisto("dc_hot_distro" , "DCHMON");
  client->registerHisto("dc_cold_distro" , "DCHMON");
  client->registerHisto("h_dc5" , "DCHMON");

  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  client->requestHistoBySubSystem("DCHMON", 1);
  OnlMonDraw *dchmon = new DchMonDraw();    // create Drawing Object
  client->registerDrawer(dchmon);              // register with client framework
}

void dchDraw(const char *what="SECOND")//"FIRST","SECOND" Draws HC,HR Canvas
{
  OnlMonClient *client = OnlMonClient::instance();  // get pointer to framewrk
  client->requestHistoBySubSystem("DCHMON", 1);         // update histos
  client->Draw("DCHMON",what);                           // Draw Histos of registered Drawers
}

void dchPS()
{
  OnlMonClient *client = OnlMonClient::instance();  // get pointer to framewrk
  client->MakePS("DCHMON");                        // Make PS files
}

void dchHtml()
{
  OnlMonClient *client = OnlMonClient::instance();  // get pointer to framewrk
  client->MakeHtml("DCHMON");                       // create html output
}
