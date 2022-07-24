void run_client()
{
  gSystem->Load("libonlrichmon.so");
  gSystem->Load("libonlmonclient.so");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("rich_adclivehist","RICHMON");
  cl->registerHisto("rich_tdclivehist","RICHMON");
  cl->registerHisto("rich_adchistsumWS" ,"RICHMON");
  cl->registerHisto("rich_adchistsumWN" ,"RICHMON");
  cl->registerHisto("rich_adchistsumES" ,"RICHMON");
  cl->registerHisto("rich_adchistsumEN" ,"RICHMON");
  cl->registerHisto("rich_deadchinlivehist","RICHMON");
  cl->registerHisto("rich_par","RICHMON");

  cl->AddServerHost("localhost");   // check local host first

  // says I know they are all on the same node
  cl->requestHistoBySubSystem("RICHMON", 1);
  OnlMonDraw *mymon = new RICHMonDraw();    // create Drawing Object
  cl->registerDrawer(mymon);              // register with client framework
}

void draw_client()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("RICHMON");         // update histos
  cl->Draw();                            // Draw Histos of registered Drawers
}
