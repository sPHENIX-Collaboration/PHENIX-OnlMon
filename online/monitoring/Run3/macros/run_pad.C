void run_pad(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlpadmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new PadMon();      // create subsystem Monitor object

  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
 
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

  return ;
}

void padDrawInit(int online = 0)
{
  gSystem->Load("libonlpadmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();

  //  cl->registerHisto("padClustRatio", "PADMON");
  cl->registerHisto("padClustActivity", "PADMON");
  cl->registerHisto("padEventError", "PADMON");
  cl->registerHisto("padStat", "PADMON");
  cl->registerHisto("padMaxRocs","PADMON");
  int j, k;
  int installedplanes = 0;
  char idstring[128];
  int pc, hv;
  int NPLANES = 6;
  int NHVSECTORS = 32;

  for (j = 0; j < NPLANES; j++)
    {
      if (j != 3)
        {
          pc = j / 2; // arm = 1-j%2;
          sprintf (idstring, "padProfPadSum%d", j);
          cl->registerHisto(idstring, "PADMON");

          // # of fired clusters
          sprintf (idstring, "padProfClustActivity%d", j);
          cl->registerHisto(idstring, "PADMON");

          sprintf (idstring, "padProfClustSize%d", j);
          cl->registerHisto(idstring, "PADMON");
//           sprintf (idstring, "padProfClustRatio%d", j);
//           cl->registerHisto(idstring, "PADMON");

          for (k = 0; k < NHVSECTORS; k++)
            {
              hv = installedplanes * NHVSECTORS + k;
              sprintf (idstring, "padClustSize%d", hv);
              cl->registerHisto(idstring, "PADMON");
            }
          installedplanes++;
        } // End "if PCINSTALLED[j]"
    } // End Loop over NPLANES
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("PADMON", 1);

  // create Drawing Object; MyMonDraw inherits from OnlMonDraw
  OnlMonDraw *padmon = new PadMonDraw();
  // register with client framework; this pops up a window
  cl->registerDrawer(padmon);
  return ;
}

void padDraw()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("PADMON", 1);         // update histos
  cl->Draw("PADMON");                            // Draw Histos of registered Drawers
  return ;
}

void padPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("PADMON");                         // create PS files
  return ;
}

void padHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("PADMON");                       // create html output
  return ;
}
