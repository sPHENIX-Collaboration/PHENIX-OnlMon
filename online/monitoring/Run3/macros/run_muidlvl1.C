void run_muidlvl1(const char *prdffile = "lvl1data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmuidlvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new MuidLvl1Mon();      // create subsystem Monitor object
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void run_muidlvl1(char *filelist, double garbage)
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmuidlvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MuidLvl1Mon *m = new MuidLvl1Mon();
  m->SetOfflineVersion(1);
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  muidlvl1loopall(filelist,1.0);
  se->EndRun(se->RunNumber());
  se->WriteHistoFile();
  CleanUpServer();
  return ;
}

void muidlvl1loopall(char *filelist, double garbage)
{
	cout << "filelist: " << filelist << endl;
	stringstream ss;
	ss << "/home/phnxrc/level1/online/monitoring/muidlvl1/runs/";
	ss << filelist;
	std::ifstream ifs( ss.str().c_str() );
	std::string line;
	pclose();
	while(std::getline(ifs, line))
	{
		cout << "prun on filen: " << line << endl;
		ss.str("");ss.clear();
		ss << line;
		pfileopen( ss.str().c_str() );
		prun();
	}
	cout << "exiting void muidlvl1loopall(char *filelist, double garbage)" << endl;
	
	return ; 
 }

void muidlvl1DrawInit(const int online = 0)
{
  gSystem->Load("libonlmuidlvl1mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("MuidLvl1_CompareSumsHN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_CompareSumsHS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_CompareSumsVN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_CompareSumsVS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_CompareVHN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_CompareVHS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_DataErrN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_DataErrS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_DataErr2N", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_DataErr2S", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_ESNCtlMis_HN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_ESNCtlMis_HS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_ESNCtlMis_VN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_ESNCtlMis_VS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_ESNMismatchN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_ESNMismatchS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_GL1EffN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_GL1EffS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountDifN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountDifS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountShalN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountShalS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountShalSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountShalSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadCountSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumShalN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumShalS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumShalSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumShalSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorRoadNumSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorSymEffCutN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorSymEffCutS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorSymEffCutShalN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorSymEffCutShalS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorSymEffN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_HorSymEffS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_SyncErrN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_SyncErrS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_SyncErr2N", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_SyncErr2S", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountDifN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountDifS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountShalN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountShalS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountShalSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountShalSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadCountSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumShalN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumShalS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumShalSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumShalSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumSimN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerRoadNumSimS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerSymEffCutN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerSymEffCutS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerSymEffCutShalN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerSymEffCutShalS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerSymEffN", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_VerSymEffS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_GL1EffS", "MUIDLVL1MON");
  cl->registerHisto("MuidLvl1_GL1EffN", "MUIDLVL1MON");
	cl->registerHisto("MuidLvl1_InfoHist", "MUIDLVL1MON");
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MUIDLVL1MON", 1);
  OnlMonDraw *muidlvl1mon = new MuidLvl1MonDraw();    // create Drawing Object
  cl->registerDrawer(muidlvl1mon);              // register with client framework
}

void muidlvl1Draw(const char *what = "SOUTH")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MUIDLVL1MON",1); // update histos
  cl->Draw("MUIDLVL1MON",what);                 // Draw Histos of registered Drawers
}


void muidlvl1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MUIDLVL1MON");
}

void muidlvl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MUIDLVL1MON");
}

 void muidlvl1loopall(int runnumber, int segments)
{

	stringstream ss;
	ss << "ls -1 /common/{a,b,c}{0,1,2,3,4,5,6}/*/*";
	ss << runnumber;
	ss << "*";
	ss << " > muidlvl1_filelist.list";
	system(ss.str().c_str());
	cout << "system command: " << ss.str().c_str() << endl;

	std::ifstream ifs("muidlvl1_filelist.list");
	std::string line;
	int processed_segments = 0;
	pclose();
	while(std::getline(ifs, line))
	{
		if(processed_segments>=segments) continue;
		cout << "prun on filen: " << line << endl;
		ss.str("");ss.clear();
		ss << line;
		pfileopen( ss.str().c_str() );
		prun();
		processed_segments++;
	}
	cout << "exiting muidlvl1loopall(int runnumber, int segments)" << endl;
	
	
	return ; 
 }	 
 
 
 void run_muidlvl1(int runnumber)//char *prdffile = "lvl1data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmuidlvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MuidLvl1Mon *m = new MuidLvl1Mon();
  // m->AddTrigger("BBCLL1(>0 tubes)");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  // start_server(prdffile);
  m->SetOfflineVersion(1);
  muidlvl1loopall(runnumber);
  return ;
}

void muidlvl1loopall(int runnumber)
{
	muidlvl1loopall(runnumber, 99999);
	
	return ; 
 }	 
 