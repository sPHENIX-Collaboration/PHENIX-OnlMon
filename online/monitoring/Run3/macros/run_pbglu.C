void run_pbglu(const char *prdffile = "etpool")
{

  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlpbglumon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  PbGlUMon *m = new PbGlUMon("PbGlUMon");
  
  m->setAction(1);
  m->setWriteDBMode(1);
  m->setDebugMode(1);
  m->AddTrigger("PPG(Pedestal)");
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  start_server(prdffile);
  return;
}

void pbgluDrawInit(const int online = 0)
{
  gSystem->Load("libonlpbglumon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");


  OnlMonClient *cl = OnlMonClient::instance();
//  cl->Verbosity(1);
  cl->registerHisto("pbglu_hese", "PbGlUMon");
  cl->registerHisto("pbglu_hbese", "PbGlUMon");
  cl->registerHisto("pbglu_hcese", "PbGlUMon");

  CreateHostList(online);

  // says I know they are all on the same node
  cl->requestHistoBySubSystem("PbGlUMon", 1);
  OnlMonDraw *pb = new PbGlUMonDraw("PbGlUMon");    // create Drawing Object
  cl->registerDrawer(pb);              // register with client framework
}

void pbgluDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("PbGlUMon",1);    // update histos
  cl->Draw("PbGlUMon", what);
}

void pbgluPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("PbGlUMon");                       // Make PS files
}

void pbgluHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("PbGlUMon");                     // create html output
}



