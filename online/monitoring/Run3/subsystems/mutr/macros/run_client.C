void run_client()
{
  gSystem->Load("libonlmutrmon.so");
  gSystem->Load("libonlmonclient.so");

  OnlMonClient *cl = OnlMonClient::instance();
    cl->AddServerHost("localhost");   // check local host first

  OnlMonDraw *mutrmon = new MutrMonDraw();    // create Drawing Object
  cl->registerDrawer(mutrmon);              // register with client framework
}

void draw_client()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MUTRMON");         // update histos
  cl->Draw();                            // Draw Histos of registered Drawers
}
