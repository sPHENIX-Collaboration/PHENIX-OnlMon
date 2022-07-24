void run_rich(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlrichmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
//  gROOT->ProcessLine(".L ServerFuncs.C");

  OnlMon *m = new RICHMon();      // create subsystem Monitor object
  m->AddTrigger("ONLMONBBCLL1"); 

  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

  return ;
}

void richDrawInit(int online = 0)
{
  gSystem->Load("libonlrichmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("rich_eventcounter", "RICHMON");
  cl->registerHisto("rich_adc_first", "RICHMON");
  cl->registerHisto("rich_adc_hit", "RICHMON");
  cl->registerHisto("rich_adc_pmt", "RICHMON");
  cl->registerHisto("rich_adc_second", "RICHMON");
  cl->registerHisto("rich_tdc_first", "RICHMON");
  cl->registerHisto("rich_tdc_hit", "RICHMON");
  cl->registerHisto("rich_tdc_pmt", "RICHMON");
  cl->registerHisto("rich_tdc_second", "RICHMON");

  cl->registerHisto("rich_adclivehist", "RICHMON");
  cl->registerHisto("rich_tdclivehist", "RICHMON");
  cl->registerHisto("rich_adchistsumEN" , "RICHMON");
  cl->registerHisto("rich_adchistsumES" , "RICHMON");
  cl->registerHisto("rich_adchistsumWN" , "RICHMON");
  cl->registerHisto("rich_adchistsumWS" , "RICHMON");
  cl->registerHisto("rich_deadchinlivehist", "RICHMON");
  cl->registerHisto("rich_par", "RICHMON");
  cl->registerHisto("rich_ringhistEN" , "RICHMON");
  cl->registerHisto("rich_ringhistES" , "RICHMON");
  cl->registerHisto("rich_ringhistWN" , "RICHMON");
  cl->registerHisto("rich_ringhistWS" , "RICHMON");
  cl->registerHisto("rich_tdchistsumEN" , "RICHMON");
  cl->registerHisto("rich_tdchistsumES" , "RICHMON");
  cl->registerHisto("rich_tdchistsumWN" , "RICHMON");
  cl->registerHisto("rich_tdchistsumWS" , "RICHMON");
  cl->registerHisto("rich_nhit_eventEN" , "RICHMON");
  cl->registerHisto("rich_nhit_eventES" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWN" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWS" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWS1" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWS2" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWS3" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWS4" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWN1" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWN2" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWN3" , "RICHMON");
  cl->registerHisto("rich_nhit_eventWN4" , "RICHMON");
  cl->registerHisto("rich_nhit_eventES1" , "RICHMON");
  cl->registerHisto("rich_nhit_eventES2" , "RICHMON");
  cl->registerHisto("rich_nhit_eventES3" , "RICHMON");
  cl->registerHisto("rich_nhit_eventES4" , "RICHMON");
  cl->registerHisto("rich_nhit_eventEN1" , "RICHMON");
  cl->registerHisto("rich_nhit_eventEN2" , "RICHMON");
  cl->registerHisto("rich_nhit_eventEN3" , "RICHMON");
  cl->registerHisto("rich_nhit_eventEN4" , "RICHMON");

  CreateHostList(online);
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("RICHMON", 1);
  OnlMonDraw *richmon = new RICHMonDraw();    // create Drawing Object
  cl->registerDrawer(richmon);              // register with client framework
}

void richDraw()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("RICHMON", 1);         // update histos
  cl->Draw("RICHMON");                            // Draw Histos of registered Drawers
}

void richPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("RICHMON");                            // create PS files
}

void richHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("RICHMON");                            // create Html output
}






