void run_acc(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlaccmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  OnlMon *m = new ACCMon();      // create subsystem Monitor object
  // set trigger we want to use
  //  m->AddTrigger("BBCLL1(>0 tubes)");
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void accDrawInit(const int online = 0)
{
  gSystem->Load("libonlaccmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("acc_adc_tac_N", "ACCMON");
  cl->registerHisto("acc_adc_tac_S", "ACCMON");
  cl->registerHisto("acc_Nadcsum", "ACCMON");
  cl->registerHisto("acc_Ntacsum", "ACCMON");
  cl->registerHisto("acc_Ntacsum_board_0", "ACCMON");
  cl->registerHisto("acc_Ntacsum_board_1", "ACCMON");
  cl->registerHisto("acc_Ntacsum_board_2", "ACCMON");
  cl->registerHisto("acc_Ntacsum_board_3", "ACCMON");
  cl->registerHisto("acc_Ntacsum_board_4", "ACCMON");
  cl->registerHisto("acc_Sadcsum", "ACCMON");
  cl->registerHisto("acc_Stacsum", "ACCMON");
  cl->registerHisto("acc_Stacsum_board_0", "ACCMON");
  cl->registerHisto("acc_Stacsum_board_1", "ACCMON");
  cl->registerHisto("acc_Stacsum_board_2", "ACCMON");
  cl->registerHisto("acc_Stacsum_board_3", "ACCMON");
  cl->registerHisto("acc_Stacsum_board_4", "ACCMON");
  cl->registerHisto("acc_livehist", "ACCMON");
  cl->registerHisto("acc_par", "ACCMON");
  cl->registerHisto("acc_taclivehist", "ACCMON");
  cl->registerHisto("adc_average", "ACCMON");
  cl->registerHisto("tac_average", "ACCMON");

  CreateHostList(online);
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("ACCMON", 1);
  ACCMonDraw *accmon = new ACCMonDraw();    // create Drawing Object
  accmon->AddKnownNoisy(41);
  accmon->AddKnownNoisy(232);
  accmon->AddKnownNoisy(271);
  cl->registerDrawer(accmon);              // register with client framework
}

void accDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ACCMON", 1);     // update histos
  cl->Draw("ACCMON",what);             // Draw Histos of registered Drawers
}

void accPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ACCMON", 1);         // update histos
  cl->MakePS("ACCMON");                            // create PS files
}

void accHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("ACCMON", 1);         // update histos
  cl->MakeHtml("ACCMON");                            // create Html output
}







