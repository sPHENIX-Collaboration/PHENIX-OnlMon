void run_rpctiming(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libmutoo_subsysreco");
  gSystem->Load("libonlrecomon.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libfun4allfuncs.so");
  gSystem->Load("libmuon_subsysreco.so" );
  gSystem->Load("librpc_subsysreco.so" );
  gSystem->Load("libonlrpctimetester.so");

  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  RecoMon *reco = new RecoMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  reco->set_recorunnumber(362227);
  reco->AddTrigger("MUON_S_SG3 && (MUIDLL1_S1D || S1H) && BBCLL1(noVtx)");
  reco->AddTrigger("MUON_N_SG3 && (MUIDLL1_N1D || N1H) && BBCLL1(noVtx)");
  reco->AddTrigger("MUON_N_SG3&BBCLL1");
  reco->AddTrigger("((MUIDLL1_N2D||S2D)||(N1D&S1D))&BBCLL1(noVtx)");
  reco->AddTrigger("(MUIDLL1_N1D||S1D)&BBCLL1(noVtx)");
  reco->AddTrigger("(MUON_S_SG1&MUIDLL1_S1D)||(MUON_N_SG1&MUIDLL1_N1D)&BBCLL1(noVtx)");
  reco->AddTrigger("MUON_S_SG1_RPC3_1_A||B||C&BBCLL1(noVtx)");
  reco->AddTrigger("MUON_N_SG1_RPC3_1_A||B||C&BBCLL1(noVtx)");
  //  reco->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  se->registerMonitor(reco);       // register subsystem Monitor with Framework
  // here starts the cut and paste from the reconstruction macro
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag( "RpcGeomType", 3 ); // was 2 in run11pp500 but updated to 3 to include RPC1

  SubsysReco *head    = new HeadReco();
  SubsysReco *trig    = new TrigReco();
  BbcReco *bbc     = new BbcReco();
  bbc->setBbcVtxError( 2.0 );
  SubsysReco *vtx     = new VtxReco();
  reco->registerSubsystem(head);
  reco->registerSubsystem(trig);
  reco->registerSubsystem(bbc);
  reco->registerSubsystem(vtx);

  SubsysReco *unpack = new MuonUnpackPRDF();
  reco->registerSubsystem(unpack);
  SubsysReco *muioo  = new MuiooReco();
  SubsysReco *mutoo  = new MuonDev();
  reco->registerSubsystem(muioo);
  reco->registerSubsystem(mutoo);
  SubsysReco* unpackrpc = new RpcUnpackPRDF();
  SubsysReco* rpcreco = new RpcReco();
  reco->registerSubsystem(unpackrpc);
  reco->registerSubsystem(rpcreco);


  OnlMon *rpctmon = new RpcTimetester();
  se->registerMonitor(rpctmon);

  start_server(prdffile);

  return ;
}

void rpctimingDrawInit(int online = 0)
{
  gSystem->Load("libonlrpctimetester.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();

  for (int iarm = 0; iarm < 2; iarm++)
    {
      char tp[255];
      char title[255];
      for (int ieff = 0; ieff < 3; ieff++)
        {
          sprintf(tp, "goodrpctime_arm%d_station%d", iarm, ieff);
          cl->registerHisto( tp, "RPCTIMINGMON");
          sprintf(tp, "refrpctime_arm%d_station%d", iarm, ieff);
          cl->registerHisto( tp, "RPCTIMINGMON");
          sprintf(tp, "feedrpctime_arm%d_station%d", iarm, ieff);
          cl->registerHisto( tp, "RPCTIMINGMON");

          sprintf(tp, "rpceff_arm%d_ieff%d", iarm, ieff);
          cl->registerHisto(tp, "RPCTIMINGMON");
        }//ieff
    }//iarm
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("RPCTIMINGMON", 1);
  OnlMonDraw *rpctimetester = new RpcTimetesterDraw("RPCTIMINGMON");    // create Drawing Object
  cl->registerDrawer(rpctimetester);                                 // register with client framework
}

void rpctimingDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();    // get pointer to framewrk
  cl->requestHistoBySubSystem("RPCTIMINGMON", 1); // update histos
  cl->Draw("RPCTIMINGMON", what);                 // Draw Histos of registered Drawers
}

void rpctimingPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("RPCTIMINGMON");                         // create PS files
  return ;
}

void rpctimingHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("RPCTIMINGMON");                       // create html output
  return ;
}
