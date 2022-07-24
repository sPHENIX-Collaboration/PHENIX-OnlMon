void run_bbc(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlbbcmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  OnlMon *m = new BBCMon();      // create subsystem Monitor object
  m->AddTrigger("ZDCLL1wide");
  m->AddTrigger("BBCLL1(>0 tubes)");
  m->AddTrigger("BBCLL1(>0 tubes) novertex");
  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  m->AddTrigger("BBCLL1(>0 tubes)_central_narrowvtx");
  m->AddTrigger("PPG(Laser)");
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void run_bbc_multifile(const char *listfile = "listfile")
{
  gSystem->Load("libonlbbcmon.so");
  gSystem->Load("libonlmonserver_funcs.so");
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  OnlMon *m = new BBCMon();      // create subsystem Monitor object
  m->AddTrigger("ZDCLL1wide");
  m->AddTrigger("BBCLL1(>0 tubes)");
  m->AddTrigger("BBCLL1(>0 tubes) novertex");
  m->AddTrigger("PPG(Laser)");
  se->registerMonitor(m);       // register subsystem Monitor with Framework

  ifstream fin(listfile);

  if(fin.bad()){
    cout<<"no listfile"<<endl;
    return;
  }
  char prdffile[256];

  while(!fin.eof()){
    fin>>prdffile;
    cout<<"Open : "<<prdffile<<endl;
    pfileopen(prdffile);
    prun();
    pclose();
  }

  return ;
}

void bbcDrawInit(const int online = 0)
{
  gSystem->Load("libonlbbcmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  char name[256];
  int nTDC = 2;
  int nPMT_BBC = 128;
  int nTRIGGER = 2;
  char *TRIGGER_str[2] = {"minimum_bias", "laser"};
  for ( int tdc = 0 ; tdc < nTDC ; tdc++ )
  {
    sprintf(name , "bbc_tdc%d", tdc);
    cl->registerHisto(name, "BBCMON");

    sprintf(name, "bbc_tdc%d_overflow", tdc );
    cl->registerHisto(name, "BBCMON");

    for ( int i = 0 ; i < nPMT_BBC ; i++ )
    {
      sprintf(name, "bbc_tdc%d_overflow_%03d", tdc, i);
      cl->registerHisto(name, "BBCMON");
    }
  }
  cl->registerHisto("bbc_adc", "BBCMON");

  for ( int trig = 0 ; trig < nTRIGGER ; trig++ )
  {
    sprintf(name, "bbc_nhit_%s", TRIGGER_str[trig] );
    cl->registerHisto(name, "BBCMON");
  }
  cl->registerHisto("bbc_tdc_armhittime", "BBCMON");
  cl->registerHisto("bbc_zvertex", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1", "BBCMON");
  cl->registerHisto("bbc_zvertex_zdc", "BBCMON");
  cl->registerHisto("bbc_zvertex_zdc_scale3", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1_novtx", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1_narrowvtx", "BBCMON");
  cl->registerHisto("bbc_nevent_counter", "BBCMON");
  cl->registerHisto("bbc_tzero_zvtx", "BBCMON");
  cl->registerHisto("bbc_prescale_hist", "BBCMON");
  cl->registerHisto("bbc_avr_hittime", "BBCMON");
  cl->registerHisto("bbc_south_hittime", "BBCMON");
  cl->registerHisto("bbc_north_hittime", "BBCMON");
  cl->registerHisto("bbc_south_chargesum", "BBCMON");
  cl->registerHisto("bbc_north_chargesum", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1_zdc", "BBCMON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("BBCMON", 1);
  OnlMonDraw *bbcmon = new BbcMonDraw();    // create Drawing Object
  cl->registerDrawer(bbcmon);              // register with client framework

  cout << "===================================================" << endl;
  cout << "BBCMon1       : BBC PMT/FEM monitor"                 << endl;
  cout << "BBCMon2       : BBC Timing & Vertex monitor"         << endl;
  cout << "BBCMon3       : For Expert Only"                     << endl;
  cout << "BBCMon4       : Beam Structure and Trigger monitor"  << endl;
  cout << "BbcMonitor    : BBC Mon 1"                           << endl;
  cout << "VertexMonitor : BBC Mon 2 and 4"                     << endl;
  cout << "===================================================" << endl;

  return;
}


/* what : "BBCMon1" -> BBC PMT/FEM monitor
   "BBCMon2" -> BBC Timing & Vertex monitor
   "BBCMon3" -> For Expert Only ( Under development ) 
   "BBCMon4" -> Beam Structure and Trigger monitor
   "BbcMonitor" -> BBC Mon 1
   "VertexMonitor" -> BBC Mon 2 and 4
 */
// PLEASE DO NOT CHANGE THE ARGUMENT VertexMonitor AGAIN WITHOUT CONSULTING
// ME, CHRIS
void bbcDraw(const char *what="VertexMonitor")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("BBCMON", 1);     // update histos
  cl->Draw("BbcMON",what);                      // Draw Histos of registered Drawers
  return;
}

void bbcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("BbcMON");                         // create ps files
  return;
}

void bbcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("BbcMON");                            // create html output
  return;
}

