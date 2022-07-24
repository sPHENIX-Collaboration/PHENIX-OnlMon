void run_dch(char *prdffile = "data.prdf", int pp = 1)
{
  gSystem->Load("libonldchmon.so");
  gSystem->Load("libonlmonserver_funcs.so");
  DchMon *dchmon = new DchMon();
  dchmon->set_averages_on_the_fly(1);
  dchmon->set_running_average(0);

  
  
/*
  if (pp)
    {
      dchmon->set_templates_pp();
      dchmon->set_dead_threshold(0.001);
      dchmon->set_noise_threshold(0.5);
      dchmon->set_update_rate(10000);
      dchmon->set_stat_threshold(300);
    }
  else
    {
	dchmon->set_templates_AuAu();
	dchmon->set_update_rate(1000);
	dchmon->set_dead_threshold(0.03);
	dchmon->set_noise_threshold(1.0);
	//dchmon->set_templates_dAu();
    }
  */ // All thresholds are included in a single template file DchTemplates.txt -- No more species
  

  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(dchmon);
  if (!strcmp(prdffile, "etpool"))
    {
      petopen("/tmp/Monitor@etpool");
      prun();
      se->WriteHistoFile(); // if etpool gets restarted, save histos
    }
  else
    {
      pfileopen(prdffile);
    }
  return;
}

void dchDrawInit(int online = 0)
{
  gSystem->Load("libonlmonclient.so");
  //  gSystem->Load("libonlinerecorder.so");
  gSystem->Load("libonldchmon.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *client = OnlMonClient::instance();
  // register histos we want with monitor name
  client->registerHisto("h_dc1norm" , "DCHMON");
  client->registerHisto("h_dc2norm" , "DCHMON");
  client->registerHisto("ek1_1" , "DCHMON");
  client->registerHisto("ek2_2" , "DCHMON");
  client->registerHisto("ek3_3" , "DCHMON");
  client->registerHisto("ek4_4" , "DCHMON");
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

  client->registerHisto("dc_new_dead_east_n" , "DCHMON");
  client->registerHisto("dc_new_dead_west_n" , "DCHMON");
  client->registerHisto("dc_new_noisy_east_n" , "DCHMON");
  client->registerHisto("dc_new_noisy_west_n" , "DCHMON");
  client->registerHisto("dc_vdrift_east" , "DCHMON");
  client->registerHisto("dc_vdrift_west" , "DCHMON");
  client->registerHisto("dc_tzero_east" , "DCHMON");
  client->registerHisto("dc_tzero_west" , "DCHMON");
  client->registerHisto("dc_eff_east" , "DCHMON");
  client->registerHisto("dc_eff_west" , "DCHMON");
  client->registerHisto("dc_new_dead_east_n_history" , "DCHMON");
  client->registerHisto("dc_new_dead_west_n_history" , "DCHMON");
  client->registerHisto("dc_new_noisy_east_n_history" , "DCHMON");
  client->registerHisto("dc_new_noisy_west_n_history" , "DCHMON");
  client->registerHisto("dc_vdrift_east_history" , "DCHMON");
  client->registerHisto("dc_vdrift_west_history" , "DCHMON");
  client->registerHisto("dc_tzero_east_history" , "DCHMON");
  client->registerHisto("dc_tzero_west_history" , "DCHMON");
  client->registerHisto("dc_eff_east_history" , "DCHMON");
  client->registerHisto("dc_eff_west_history" , "DCHMON");
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
