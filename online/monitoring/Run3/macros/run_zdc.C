void run_zdc(const char *prdffile="data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlzdcmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  OnlMon *m = new ZDCMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance();// get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}

void zdcDrawInit(int online = 0)
{
  gSystem->Load("libonlzdcmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
// register histos we want with monitor name
  cl->registerHisto("zdc_adc_north","ZDCMON");
  cl->registerHisto("zdc_adc_south","ZDCMON");
  /*Ciprian Gal 110211*/
  for(int i=0;i<3;i++)
    {
      char hname[256];
      sprintf(hname,"zdc_adc_n_ind_%d",i);
      cl->registerHisto(hname,"ZDCMON");
      sprintf(hname,"zdc_adc_s_ind_%d",i);
      cl->registerHisto(hname,"ZDCMON");
    }

  cl->registerHisto("zdc_hor_north","ZDCMON");
  cl->registerHisto("zdc_laser_north1","ZDCMON");
  cl->registerHisto("zdc_laser_north1b","ZDCMON");
  cl->registerHisto("zdc_laser_north2","ZDCMON");
  cl->registerHisto("zdc_laser_north2b","ZDCMON");
  cl->registerHisto("zdc_laser_north3","ZDCMON");
  cl->registerHisto("zdc_laser_north3b","ZDCMON");
  cl->registerHisto("zdc_laser_south1","ZDCMON");
  cl->registerHisto("zdc_laser_south1b","ZDCMON");
  cl->registerHisto("zdc_laser_south2","ZDCMON");
  cl->registerHisto("zdc_laser_south2b","ZDCMON");
  cl->registerHisto("zdc_laser_south3","ZDCMON");
  cl->registerHisto("zdc_laser_south3b","ZDCMON");
  cl->registerHisto("zdc_ratio_north","ZDCMON");
  cl->registerHisto("zdc_ratio_south","ZDCMON");
  cl->registerHisto("zdc_value","ZDCMON");
  cl->registerHisto("zdc_ver_north","ZDCMON");
  cl->registerHisto("zdc_vertex","ZDCMON");
  cl->registerHisto("zdc_vertex_b","ZDCMON");
  cl->registerHisto("zdc_vs_bbc","ZDCMON");

  cl->registerHisto("smd_hor_north","ZDCMON");
  cl->registerHisto("smd_hor_north_good","ZDCMON");
  cl->registerHisto("smd_hor_north_small","ZDCMON");
  cl->registerHisto("smd_hor_south","ZDCMON");
  cl->registerHisto("smd_value","ZDCMON");
  cl->registerHisto("smd_value_good","ZDCMON");
  cl->registerHisto("smd_value_small","ZDCMON");
  cl->registerHisto("smd_ver_north","ZDCMON");
  cl->registerHisto("smd_ver_north_good","ZDCMON");
  cl->registerHisto("smd_ver_north_small","ZDCMON");
  cl->registerHisto("smd_ver_south","ZDCMON");
  cl->registerHisto("smd_xt_north","ZDCMON");
  cl->registerHisto("smd_xt_south","ZDCMON");
  cl->registerHisto("smd_xy_north","ZDCMON");
  cl->registerHisto("smd_xy_south","ZDCMON");
  cl->registerHisto("smd_yt_north","ZDCMON");
  cl->registerHisto("smd_yt_south","ZDCMON");
  cl->registerHisto("smd_sum_hor_north","ZDCMON");
  cl->registerHisto("smd_sum_hor_south","ZDCMON");
  cl->registerHisto("smd_sum_ver_north","ZDCMON");
  cl->registerHisto("smd_sum_ver_south","ZDCMON");
  char name[64];
  for(int i=0; i<40; i++)
    {
      sprintf(name,"fveto_adc_vs_tdc_ch%d",i);
      cl->registerHisto(name,"ZDCMON");
      sprintf(name,"rveto_adc_vs_tdc_ch%d",i);
      cl->registerHisto(name,"ZDCMON");
      sprintf(name,"zdc_adc_vs_tdc_ch%d",i);
      cl->registerHisto(name,"ZDCMON");
    }
  for(int i=0; i<8; i++)
    {
      sprintf(name,"veto_adc_vs_pos_%d",i);
      cl->registerHisto(name,"ZDCMON");
    }
  CreateHostList(online);

 // get my histos from server, the second parameter = 1 
// says I know they are all on the same node
  cl->requestHistoBySubSystem("ZDCMON", 1);
  OnlMonDraw *zdcmon = new ZdcMonDraw();    // create Drawing Object
  cl->registerDrawer(zdcmon);              // register with client framework
}

void zdcDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ZDCMON",1);      // update histos
  cl->Draw("ZdcMON",what);                      // Draw Histos of registered Drawers
}
void zdcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("ZdcMON");                         // create PS files
}

void zdcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("ZdcMON");                       // create html output
}
