
class OnlMon;

OnlMon* m;

void run_lvl2(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonllvl2mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  recoConsts* rc = recoConsts::instance();

  rc->set_IntFlag("LVL2_USE_ASCII_DB",1);
  rc->set_CharFlag("LVL2_DB_DIR",gSystem->Getenv("LVL2CALIB_DATA"));
  //rc->set_IntFlag("LVL2_REAL_DATA",1);
  //rc->set_IntFlag("LVL2_YEAR",8);
  //rc->set_CharFlag("TRIGSETUPFILE","Run8dAuTriggerSetupFile");
  //rc->set_CharFlag("TRIGSETUPFILE","Run8ppTriggerSetupFile");
  //rc->set_CharFlag("Run2Lvl2AlgoName","");

  OnlMonServer* se = OnlMonServer::instance(); // get pointer to Server Framework
  m = new Lvl2Mon();      // create subsystem Monitor object
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

  return;
}

void finish_lvl2()
{
  delete m;
}

void lvl2DrawInit(const int online = 0)
{
  gSystem->Load("libonllvl2mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();

  Lvl2MonDraw* lvl2mon = new Lvl2MonDraw();    // create Drawing Object
  std::string name = lvl2mon->Name();

  // register histos we want with monitor name
  cl->registerHisto("Live_Lvl2Mon_AlgoRejFact", name.c_str());

  for (unsigned int i=0; i<32; i++)
    {
      char hname[1024];
      sprintf(hname,"Online_AcceptHist_Lvl2Algo%0d",i);
      cl->registerHisto(hname, name.c_str());
    }

  CreateHostList(online);

  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem(name.c_str(), 1);

  cl->registerDrawer(lvl2mon);              // register with client framework
}

void lvl2Draw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("LVL2MON",1);         // update histos
  cl->Draw("LVL2MON",what);                       // Draw Histos of registered Drawers
}

void lvl2PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("LVL2MON");                          // Create PS files
  return;
}

void lvl2Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("LVL2MON");                        // Create html output
  return;
}
