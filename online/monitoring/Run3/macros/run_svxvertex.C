#include <sstream>
#include <string>

void run_svxvertex(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlrecomon.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libfun4allfuncs.so");
  gSystem->Load("libsvx.so");
  gSystem->Load("libonlsvxvertex.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  gSystem->ListLibraries();

  const char* calib_env = gSystem->Getenv("ONLMON_CALIB");

  RecoMon *reco = new RecoMon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  int runnumber = 456590;
  reco->set_recorunnumber(runnumber);
  reco->AddTrigger("BBCLL1(>1 tubes) narrowvtx");

  se->registerMonitor(reco);       // register subsystem Monitor with Framework
  // here starts the cut and paste from the reconstruction macro
  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("SVXACTIVE", 1);

  SubsysReco *head = new HeadReco();
  SubsysReco *sync = new SyncReco();
  SubsysReco *trig = new TrigReco();

  BbcReco *bbc     = new BbcReco();
  bbc->setBbcVtxError( 2.0 );

  SubsysReco *vtx  = new VtxReco();
  VtxReject  *vrej = new VtxReject("VTXREJ");
  vrej->SetVertexRange(-15,15);
  vrej->SetBbcChargeRange(10,200);

  reco->registerSubsystem(head);
  reco->registerSubsystem(sync);
  reco->registerSubsystem(trig);
  reco->registerSubsystem(bbc);
  reco->registerSubsystem(vtx);
  reco->registerSubsystem(vrej);

  SvxParManager *svxpar = new SvxParManager();
  svxpar->set_ReadStripHotDeadFromFile(1);
  svxpar->set_StripHotDeadFileName(Form("%s/run407951_strip_hotdeadChannels.txt",calib_env));
  svxpar->set_StripHotDeadReadoutsFileName(Form("%s/run407951_strip_hotdeadReadouts.txt",calib_env));

  svxpar->set_UseRefDiffPixelMap(1);
  svxpar->set_ReadRefDiffPixelMapFromFile(1);
  svxpar->set_RefDiffPixelMapFiles(Form("%s/blank_pixel_refmap.txt",calib_env),
                                   Form("%s/pixel_deadmap_run14auau200_run407951.dat",calib_env),
                                   Form("%s/chip_deadmap_run14auau200_run407951.dat",calib_env));
  svxpar->set_UseStripThresholdDatbase(false);
  svxpar->Load_ThresholdFile(Form("%s/threshold.h",calib_env));
  svxpar->set_GeometryFileName(Form("%s/svxPISA_Run14AuAu200_dchp2_v8.par",calib_env));
  svxpar->Verbosity(0);
  reco->registerSubsystem(svxpar);
 
 
  SvxDecode *svxdecode = new SvxDecode();
  svxdecode->includePixel(true);
  svxdecode->includeStripixel(true);
  svxdecode->setAdcOffset(24);
  svxdecode->setAdcCutoff(-24);
  reco->registerSubsystem(svxdecode);


  SvxReco *svxrec = new SvxReco();
  svxrec->Verbosity(0);
  //svxrec->set_UseStripThresholdDatbase(true);
  //svxrec->set_StripixelAdcSumThreshold(0);
  reco->registerSubsystem(svxrec);


  SubsysReco *svxvtxseedfinder = new SvxPriVertexSeedFinder();
  svxvtxseedfinder->Verbosity(0);
  reco->registerSubsystem(svxvtxseedfinder);


  SvxStandAloneReco *svxstandalone = new SvxStandAloneReco();
  svxstandalone->Verbosity(0);
  svxstandalone->setVertexRecoFlag(2);
  svxstandalone->setWindowScale(6);
  reco->registerSubsystem(svxstandalone);


  SvxPrimVertexFinder *svxprimvtxfinder = new SvxPrimVertexFinder();
  svxprimvtxfinder->Verbosity(0);
  //svxprimvtxfinder->set_ppflag(true);
  reco->registerSubsystem(svxprimvtxfinder);

  SubsysReco *svmon   = new SvxVertex();
  reco->registerSubsystem(svmon);

  start_server(prdffile);

  return ;
}

void svxvertexDrawInit(int online = 0)
{
  gSystem->Load("libonlsvxvertex.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  OnlMonClient *cl = OnlMonClient::instance();

  cl->registerHisto("h_bbc_z","SVXVERTEXMON");
  cl->registerHisto("h_vtx_z","SVXVERTEXMON");
  cl->registerHisto("h_vtx_xy","SVXVERTEXMON");
  cl->registerHisto("h_bbcz_minus_vtxz","SVXVERTEXMON");
  cl->registerHisto("h_bbcz_vs_vtxz","SVXVERTEXMON");

  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("SVXVERTEXMON", 1);
  OnlMonDraw *svxvertex = new SvxVertexDraw("SVXVERTEXMON");    // create Drawing Object
  cl->registerDrawer(svxvertex);                                 // register with client framework
}

void svxvertexDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();    // get pointer to framewrk
  cl->requestHistoBySubSystem("SVXVERTEXMON", 1); // update histos
  cl->Draw("SVXVERTEXMON",what);                  // Draw Histos of registered Drawers
}

void svxvertexPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("SVXVERTEXMON");                         // create PS files
  return ;
}

void svxvertexHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("SVXVERTEXMON");                       // create html output
  return ;
}
