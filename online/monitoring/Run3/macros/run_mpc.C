void run_mpc(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmpcmon.so");
  gSystem->Load("libmpc.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new MpcMon();      // create MPC Monitor object


  // Run16dAu200
  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  m->AddTrigger("BBCLL1(>0 tubes)");
  m->AddTrigger("MPC_N_A");
  m->AddTrigger("MPC_N_B");
  m->AddTrigger("MPC_S_A");
  m->AddTrigger("MPC_S_B");

  m->AddTrigger("PPG(Laser)");


  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);
  start_server(prdffile);

  return ;
}


void mpcDrawInit(const int online = 0)
{
  gSystem->Load("libonlmpcmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
                     
  cl->registerHisto("mpc_nevents","MPCMON");
  cl->registerHisto("mpc_bbctrig_2d","MPCMON");
  cl->registerHisto("mpc_mpctrig_2d","MPCMON");

  cl->registerHisto("mpc_scaledowns","MPCMON");
  cl->registerHisto("mpc_bbctrig_ehist","MPCMON");
  cl->registerHisto("mpc_ehist0","MPCMON");
  cl->registerHisto("mpc_ehist1","MPCMON");
  cl->registerHisto("mpc_ehist2","MPCMON");

  cl->registerHisto("mpc_bbctrig_ehist_n","MPCMON");
  cl->registerHisto("mpc_ehist0_n","MPCMON");
  cl->registerHisto("mpc_ehist1_n","MPCMON");
  cl->registerHisto("mpc_ehist2_n","MPCMON");

  cl->registerHisto("mpc_bbctrig_ehist_s","MPCMON");
  cl->registerHisto("mpc_ehist0_s","MPCMON");
  cl->registerHisto("mpc_ehist1_s","MPCMON");
  cl->registerHisto("mpc_ehist2_s","MPCMON");

  cl->registerHisto("mpc_rbits1","MPCMON");
  cl->registerHisto("mpc_rbits2","MPCMON");

  cl->registerHisto("mpc_ch_timedist","MPCMON");
  cl->registerHisto("mpcs_bbctrig_crystal_energy","MPCMON");
  cl->registerHisto("mpcn_bbctrig_crystal_energy","MPCMON");
  cl->registerHisto("mpcs_timedist","MPCMON");
  cl->registerHisto("mpcn_timedist","MPCMON");
  cl->registerHisto("mpcs_crystal0","MPCMON");
  cl->registerHisto("mpcs_crystal1","MPCMON");
  //cl->registerHisto("mpcs_crystal2","MPCMON");
  cl->registerHisto("mpcn_crystal0","MPCMON");
  cl->registerHisto("mpcn_crystal1","MPCMON");
  //cl->registerHisto("mpcn_crystal2","MPCMON");
  cl->registerHisto("mpcs_led_crystal_energy","MPCMON");
  cl->registerHisto("mpcn_led_crystal_energy","MPCMON");
  cl->registerHisto("mpc_led_hist2d","MPCMON");
  cl->registerHisto("mpc_slewdist","MPCMON");
  cl->registerHisto("mpc_triginfo","MPCMON");
  cl->registerHisto("mpcs_bbctrig_occupancy","MPCMON");
  cl->registerHisto("mpcn_bbctrig_occupancy","MPCMON");
  cl->registerHisto("mpcvsbbcst0", "MPCMON");
  cl->registerHisto("mpcvsbbcnt0", "MPCMON");
  cl->registerHisto("mpcvsbbcvtx", "MPCMON");
  cl->registerHisto("mpcvsbbct0", "MPCMON");
  cl->registerHisto("mpc0_trig0_crossing", "MPCMON");
  cl->registerHisto("mpc1_trig0_crossing", "MPCMON");
  cl->registerHisto("mpc0_trig1_crossing", "MPCMON");
  cl->registerHisto("mpc1_trig1_crossing", "MPCMON");
  cl->registerHisto("mpc0_trig2_crossing", "MPCMON");
  cl->registerHisto("mpc1_trig2_crossing", "MPCMON");
  cl->registerHisto("mpc0_trig3_crossing", "MPCMON");
  cl->registerHisto("mpc1_trig3_crossing", "MPCMON");
  //cl->registerHisto("mpc0_trig4_crossing", "MPCMON");
  //cl->registerHisto("mpc1_trig4_crossing", "MPCMON");

  // these histograms are registered but not plotted
  // in the Client printout they show up as unknown
  // if they are not registered under some subsystem
  // registering under MPCMON would just create load because
  // they are copied from the server, so MPCUNUSED sounds like
  // a good name for the "subsystem"
  cl->registerHisto("mpcn_bbctrig_crystal_count", "MPCUNUSED");
  cl->registerHisto("mpcs_bbctrig_crystal_count", "MPCUNUSED");

  CreateHostList(online);
  // get mpc histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MPCMON", 1);
  OnlMonDraw *mpcmon = new MpcMonDraw();
  cl->registerDrawer(mpcmon); 

}

void mpcTestDraw()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk

  TH1 *h[2];
  h[0]= (TH1*) cl->getHisto("mpc_bbctrig_ehist_south_0");
  h[1]= (TH1*) cl->getHisto("mpc_bbctrig_ehist_north_0");

  TCanvas *c = new TCanvas("c","c",800,500);
  c->Divide(2,1,0.0001,0.0001);
  c->cd(1);
  h[0]->Draw();
  c->cd(2);
  h[1]->Draw();

  return;
}

void mpcDraw(const char *what="POMS1")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MPCMON");         // update histos
  //cl->Draw("MPCMON",what);                       // Draw Histos of registered Drawers
  cl->Draw("MPCMON",what);                       // Draw Histos of registered Drawers
}

void mpcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MPCMON");                          // Create PS files
  return;
}

void mpcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MPCMON");                        // Create html output
  return;
}
