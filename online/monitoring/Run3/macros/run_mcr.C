void run_mcr(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmcrmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  McrMon *m = new McrMon();      // create subsystem Monitor object
  // 0 is bbcll1, 1 is zdcll1
  m->SetGoodEvents(1000,0);
  m->SetGoodEvents(1000,1);
  m->SetVtxRange(120,0);
  m->SetVtxRange(120,1);
  m->SetnBins(120,0);
  m->SetnBins(120,1);
  m->VertexCalc("BBC");
  //  m->AddTriggerName("BBCLL1(>1 tubes) novertex",0); 
  m->AddTriggerName("BBCLL1(>0 tubes) novertex",0);
  m->AddTriggerName("ZDCNS",1);
  m->AddTriggerName("ZDCLL1wide",1);
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void mcrDrawInit(const int online = 0)
{
  gSystem->Load("libonlmcrmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("mcr_bbcll1_zvertex", "MCRMON");
  cl->registerHisto("mcr_error", "MCRMON");
  cl->registerHisto("mcr_stat", "MCRMON");
  cl->registerHisto("mcr_zdcll1_zvertex", "MCRMON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MCRMON", 1);
  McrMonDraw *mcrmon = new McrMonDraw();    // create Drawing Object
  // set ranges for BBCLL1 (index 0) and ZDCLL1 (index 1)
  mcrmon->setrmsrange(0,30,60);
  mcrmon->setvtxrange(0,-10,10);
  mcrmon->setrmsrange(1,30,60);
  mcrmon->setvtxrange(1,-10,10);
  cl->registerDrawer(mcrmon);              // register with client framework
  return;
}


void mcrDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MCRMON", 1);         // update histos
  cl->Draw("MCRMON",what);                            // Draw Histos of registered Drawers
  return;
}

void mcrPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MCRMON");                            // create ps files
  return;
}

void mcrHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MCRMON");                            // create html output
  return;
}

