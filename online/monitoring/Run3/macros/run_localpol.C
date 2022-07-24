void run_localpol(const char *prdffile = "data.prdf")
{

  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonllocalpolmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new LocalPolMon();       // create subsystem Monitor object
// just to reduce the number of events processed which also should increase the number of scaler events we get, the pedestal trigger fires with 1Hz
  m->AddTrigger("PPG(Pedestal)"); 
  OnlMonServer *se = OnlMonServer::instance();// get pointer to Server Framework
  se->registerMonitor(m);     // register subsystem Monitor with Framework
  start_server(prdffile);

  return ;
}


void localpolDrawInit(const int online = 0)
{
  gSystem->Load("libonllocalpolmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();

  // register histos we want with monitor name
  cl->registerHisto("lp_fillnum","LOCALPOLMON");
  cl->registerHisto("lp_nr_pts","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_bl_NLR","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_bl_NUD","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_bl_SLR","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_bl_SUD","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_yl_NLR","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_yl_NUD","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_yl_SLR","LOCALPOLMON");
  cl->registerHisto("lp_sq_Asym_yl_SUD","LOCALPOLMON");
  cl->registerHisto("lp_x_Asym_NLR","LOCALPOLMON");
  cl->registerHisto("lp_x_Asym_NUD","LOCALPOLMON");
  cl->registerHisto("lp_x_Asym_SLR","LOCALPOLMON");
  cl->registerHisto("lp_x_Asym_SUD","LOCALPOLMON");
  cl->registerHisto("lp_spininfo","LOCALPOLMON");
  cl->registerHisto("lp_time","LOCALPOLMON");
  cl->registerHisto("lp_count_0","LOCALPOLMON");
  cl->registerHisto("lp_count_1","LOCALPOLMON");
  cl->registerHisto("lp_count_2","LOCALPOLMON");
  cl->registerHisto("lp_count_3","LOCALPOLMON");
  cl->registerHisto("lp_count_4","LOCALPOLMON");
  cl->registerHisto("lp_count_5","LOCALPOLMON");
  cl->registerHisto("lp_count_6","LOCALPOLMON");
  cl->registerHisto("lp_count_7","LOCALPOLMON");
  cl->registerHisto("lp_xingshift","LOCALPOLMON"); 

  CreateHostList(online);

  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("LOCALPOLMON", 1);
  OnlMonDraw *localpol = new LocalPolMonDraw();    // create Drawing Object
  cl->registerDrawer(localpol);              // register with client framework
}

void localpolDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("LOCALPOLMON", 1);         // update histos
  cl->Draw("LOCALPOLMON",what);                       // Draw Histos of registered Drawers
}

void localpolPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("LOCALPOLMON");                          // Create PS files
  return;
}

void localpolHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("LOCALPOLMON");                        // Create html output
  return;
}

