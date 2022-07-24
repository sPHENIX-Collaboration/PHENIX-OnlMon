void run_pktsize(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlpktsizemon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  OnlMon *m = new PktSizeMon();      // create subsystem Monitor object
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void pktsizeDrawInit(const int online = 0)
{
  gSystem->Load("libonlpktsizemon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("pktsize_hist", "PKTSIZEMON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("PKTSIZEMON", 1);
  PktSizeMonDraw *pktsizemon = new PktSizeMonDraw();    // create Drawing Object
  cl->registerDrawer(pktsizemon);              // register with client framework
  pktsizemon->AddKnownBig(1002,225);
  pktsizemon->AddKnownBig(1003,225);
  // mpc
  pktsizemon->AddKnownBig(21101,300);
  pktsizemon->AddKnownBig(21102,300);
  pktsizemon->AddKnownBig(21103,300);
  pktsizemon->AddKnownBig(21104,300);
  pktsizemon->AddKnownBig(21105,300);
  pktsizemon->AddKnownBig(21106,300);
  // mpcex north
  for (int i=21301; i <=21308; i++)
    {
      pktsizemon->AddKnownBig(i,600);
    }
  // mpcex south
  for (int i=21351; i <=21358; i++)
    {
      pktsizemon->AddKnownBig(i,600);
    }
  // tofw
  pktsizemon->AddKnownBig(7102,210);
  pktsizemon->AddKnownBig(7104,210);
  // zdc
  pktsizemon->AddKnownBig(13001,175);
  // vtxp
  for (int i=24001; i<24061;i++)
    {
       pktsizemon->AddKnownBig(i,400);
    }
  pktsizemon->AddKnownBig(24054,550);
  // vtxs
  for (int i=24101; i<24141;i++)
    {
       pktsizemon->AddKnownBig(i,400);
    }

  return;
}


void pktsizeDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("PKTSIZEMON", 1);         // update histos
  cl->Draw("PKTSIZEMON",what);                            // Draw Histos of registered Drawers
  return;
}

void pktsizePS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("PKTSIZEMON");                            // create ps files
  return;
}

void pktsizeHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("PKTSIZEMON");                            // create html output
  return;
}

