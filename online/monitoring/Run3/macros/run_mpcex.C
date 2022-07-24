void run_mpcex(const char *prdffile = "rc-0419267-MPCEX.S-0-0.prdf") {
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmpcexmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  OnlMon *onmon = new MpcExMon("MPCEXMON");      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(onmon);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}


void mpcexDrawInit(const int online = 0) {
  gSystem->Load("libonlmpcexmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name

  cl->registerHisto("mpcex_events","MPCEXMON");
  cl->registerHisto("mpcex_lengthS","MPCEXMON");
  cl->registerHisto("mpcex_lengthN","MPCEXMON");
  cl->registerHisto("mpcex_partimeS","MPCEXMON");
  cl->registerHisto("mpcex_partimeN","MPCEXMON");
  cl->registerHisto("mpcex_partime0","MPCEXMON");
  cl->registerHisto("mpcex_partimeSN","MPCEXMON");
  cl->registerHisto("mpcex_partimeSD","MPCEXMON");
  cl->registerHisto("mpcex_stack0","MPCEXMON");
  cl->registerHisto("mpcex_stackS","MPCEXMON");
  cl->registerHisto("mpcex_stackN","MPCEXMON");
  cl->registerHisto("mpcex_stackSD","MPCEXMON");
  cl->registerHisto("mpcex_stackRG","MPCEXMON");
  cl->registerHisto("mpcex_stateph0","MPCEXMON");
  cl->registerHisto("mpcex_statephS","MPCEXMON");
  cl->registerHisto("mpcex_statephN","MPCEXMON");
  cl->registerHisto("mpcex_statephSD","MPCEXMON");
  cl->registerHisto("mpcex_cidmap","MPCEXMON");
  cl->registerHisto("mpcex_cidavg","MPCEXMON");
  cl->registerHisto("mpcex_cidSD","MPCEXMON");
  cl->registerHisto("mpcex_cidRG","MPCEXMON");
  cl->registerHisto("mpcex_hiadc","MPCEXMON");
  cl->registerHisto("mpcex_loadc","MPCEXMON");
  cl->registerHisto("mpcex_himaps","MPCEXMON");
  cl->registerHisto("mpcex_lomaps","MPCEXMON");
  cl->registerHisto("mpcex_himapn","MPCEXMON");
  cl->registerHisto("mpcex_lomapn","MPCEXMON");
  cl->registerHisto("mpcex_lohis","MPCEXMON");
  cl->registerHisto("mpcex_lohin","MPCEXMON");
  cl->registerHisto("mpcex_phimaps","MPCEXMON");
  cl->registerHisto("mpcex_plomaps","MPCEXMON");
  cl->registerHisto("mpcex_phimapn","MPCEXMON");
  cl->registerHisto("mpcex_plomapn","MPCEXMON");
  cl->registerHisto("mpcex_plohis","MPCEXMON");
  cl->registerHisto("mpcex_plohin","MPCEXMON");
  cl->registerHisto("mpcex_xinghits","MPCEXMON");
  cl->registerHisto("mpcex_xinghitn","MPCEXMON");
  cl->registerHisto("mpcex_femidS","MPCEXMON");
  cl->registerHisto("mpcex_femidN","MPCEXMON");
  cl->registerHisto("mpcex_checksumS","MPCEXMON");
  cl->registerHisto("mpcex_checksumN","MPCEXMON");
  cl->registerHisto("mpcex_bclockSD","MPCEXMON");
  cl->registerHisto("mpcex_bclockSN","MPCEXMON");
  cl->registerHisto("mpcex_graycodeS","MPCEXMON");
  cl->registerHisto("mpcex_graycodeN","MPCEXMON");
  cl->registerHisto("mpcex_femtestS","MPCEXMON");
  cl->registerHisto("mpcex_femtestN","MPCEXMON");
  cl->registerHisto("mpcex_cbtestS","MPCEXMON");
  cl->registerHisto("mpcex_cbtestN","MPCEXMON");
  cl->registerHisto("mpcex_mpcS","MPCEXMON");
  cl->registerHisto("mpcex_mpcN","MPCEXMON");
  cl->registerHisto("mpcex_mpclS","MPCEXMON");
  cl->registerHisto("mpcex_mpclN","MPCEXMON");
  cl->registerHisto("mpcex_mpcallS","MPCEXMON");
  cl->registerHisto("mpcex_mpcallN","MPCEXMON");
  cl->registerHisto("mpcex_mpcerrS","MPCEXMON");
  cl->registerHisto("mpcex_mpcerrN","MPCEXMON");
  cl->registerHisto("mpcex_mpcrS","MPCEXMON");
  cl->registerHisto("mpcex_mpcrN","MPCEXMON");
  cl->registerHisto("mpcex_parscanHS","MPCEXMON");
  cl->registerHisto("mpcex_parscanLS","MPCEXMON");
  cl->registerHisto("mpcex_parscanHN","MPCEXMON");
  cl->registerHisto("mpcex_parscanLN","MPCEXMON");
  cl->registerHisto("mpcex_hitsS","MPCEXMON");
  cl->registerHisto("mpcex_hitsN","MPCEXMON");
  cl->registerHisto("mpcex_lowhitsS","MPCEXMON");
  cl->registerHisto("mpcex_lowhitsN","MPCEXMON");
  cl->registerHisto("mpcex_qhlrS","MPCEXMON");
  cl->registerHisto("mpcex_qhlrN","MPCEXMON");
  cl->registerHisto("mpcex_qmipS","MPCEXMON");
  cl->registerHisto("mpcex_qmipN","MPCEXMON");
  cl->registerHisto("mpcex_mpcS_T0","MPCEXMON");
  cl->registerHisto("mpcex_mpcN_T0","MPCEXMON");
  cl->registerHisto("mpcex_mpclS_T0","MPCEXMON");
  cl->registerHisto("mpcex_mpclN_T0","MPCEXMON");
  cl->registerHisto("mpcex_mpcS_T1","MPCEXMON");
  cl->registerHisto("mpcex_mpcN_T1","MPCEXMON");
  cl->registerHisto("mpcex_mpclS_T1","MPCEXMON");
  cl->registerHisto("mpcex_mpclN_T1","MPCEXMON");
  cl->registerHisto("mpcex_mpcS_T2","MPCEXMON");
  cl->registerHisto("mpcex_mpcN_T2","MPCEXMON");
  cl->registerHisto("mpcex_mpclS_T2","MPCEXMON");
  cl->registerHisto("mpcex_mpclN_T2","MPCEXMON");

  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MPCEXMON", 1);
  OnlMonDraw *mpcexmon = new MpcExMonDraw("MPCEXMON");// create Drawing Object
  cl->registerDrawer(mpcexmon);              // register with client framework
}

void mpcexDraw(const char *what="ALL") {
  OnlMonClient *cl = OnlMonClient::instance();// get pointer to framewrk
  cl->requestHistoBySubSystem("MPCEXMON");  // update histos
  cl->Draw("MPCEXMON",what);                // Draw Histos of registered Drawers
}

void mpcexPS() {
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MPCEXMON");                           // Create PS files
  return;
}

void mpcexHtml() {
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MPCEXMON");                         // Create html output
  return;
}
