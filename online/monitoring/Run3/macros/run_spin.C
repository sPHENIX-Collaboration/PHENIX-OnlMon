const char* SpinOldMonName = "SPINOLDMON";

void run_spin(const char *prdffile = "etpool")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlspinmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new SpinMon(SpinOldMonName);           // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance();// get pointer to Server Framework
  se->registerMonitor(m);         // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}



void spinDrawInit(int online = 0)
{
  gSystem->Load("libonlspinmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  CreateHostList(online);
  //  OnlMonDraw *mymon = new SpinMonDraw(SpinOldMonName);    // create Drawing Object
  SpinMonDraw *mymon = new SpinMonDraw(SpinOldMonName);    // create Drawing Object
  // this should be called before OnlMonClient::requestHistosBySubSystem
  mymon->RegisterToClient();
  
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem(SpinOldMonName, 1);
  cl->registerDrawer(mymon);              // register with client framework
  
  // cout << "Spin Monitor Canvases are initialized" << endl;
}

void spinDraw(const char *what = "shiftcrew")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem(SpinOldMonName,1);  // update histos
  cl->Draw(SpinOldMonName,what);                   // Draw Histos of registered Drawers
}

void spinPS(const char* what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS(SpinOldMonName,what);                        // create ps files
}

void spinHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml(SpinOldMonName);                      // create html output
}
