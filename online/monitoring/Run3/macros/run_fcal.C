void run_fcal(const char *prdffile = "fcaldata.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlfcalmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new FcalMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

  return ;
}

void fcalDrawInit(const int online = 0)
{

  gSystem->Load("libonlfcalmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();
 
  //Register histos
  // histogram names
  const int ROWTOT = 12;
  const int COLTOT = 12;
  // CINT does not get a const int HISTMAX = 37 declaration
  // as an array size
#define HISTMAX 4
  const char *hname[] =
    {
      "fcal_south_loadc",
      "fcal_south_loadc_corrected",
      //
      "fcal_north_loadc",
      "fcal_north_loadc_corrected"
    };


  // register histos we want with monitor name

  for (int i = 0; i < HISTMAX; i++)
    {
      cl->registerHisto(hname[i], "FCALMON");
    }

  CreateHostList(online); 
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("FCALMON", 1);
  OnlMonDraw *fcalmon = new FcalMonDraw();    // create Drawing Object 
  cl->registerDrawer(fcalmon);              // register with client framework
}

void fcalDraw(const char* what="SHIFT")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("FCALMON",1);         // update histos
  cl->Draw("FCALMON",what);                            // Draw Histos of registered Drawers
}

void fcalPS(const char* what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("FCALMON",what);                            // create ps files
  return;
}

void fcalHtml(const char* what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("FCALMON",what);                            // create html output
  return;
}


  
