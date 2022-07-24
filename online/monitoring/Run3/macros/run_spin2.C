const char* SpinMonName = "SPINMON";

void run_spin2(const char *prdffile = "etpool")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlspin2mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new Spin2Mon(SpinMonName); // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance();// get pointer to Server Framework
  se->registerMonitor(m); // register subsystem Monitor with Framework
  start_server(prdffile); // does the OnlMon call the appropriate member functions..?
  return;
}

void spin2DrawInit(int online = 0)
{
  gSystem->Load("libonlspin2mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  CreateHostList(online);
  Spin2MonDraw *spinmon = new Spin2MonDraw(SpinMonName);    // create Drawing Object
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  OnlMonClient *cl = OnlMonClient::instance();
  cl->Verbosity(0);
  cl->requestHistoBySubSystem(SpinMonName, 1);
  cl->registerDrawer(spinmon); 
}

void spin2Draw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem(SpinMonName,1);  // wp - seems to be issue here..
  cl->Draw(SpinMonName,what);                   // Draw Histos of registered Drawers
}

void spin2PS(const char* what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS(SpinMonName,what);                        // create ps files
}

void spin2Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml(SpinMonName);                      // create html output
}
