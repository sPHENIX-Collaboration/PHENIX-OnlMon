void run_mutriglvl1(char *prdffile)
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutriglvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MutrigLvl1Mon *m = new MutrigLvl1Mon();
  m->SetOfflineVersion(0);
  // m->AddTrigger("BBCLL1(>0 tubes)");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

void run_mutriglvl1(char *filelist, double garbage)
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutriglvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MutrigLvl1Mon *m = new MutrigLvl1Mon();
  m->SetOfflineVersion(1);
  // m->AddTrigger("BBCLL1(>0 tubes)");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  // start_server(prdffile);
  mutriglvl1loopall(filelist,1.0);
  se->EndRun(se->RunNumber()); // 
  se->WriteHistoFile();
  CleanUpServer();
  return ;
}

void run_mutriglvl1(char *filelist, double garbage, double garbage2)
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutriglvl1mon.so");
	gSystem->Load("libonlmuidlvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MutrigLvl1Mon *mutrig = new MutrigLvl1Mon();
  mutrig->SetOfflineVersion(1);
	MuidLvl1Mon *muid = new MuidLvl1Mon();
  muid->SetOfflineVersion(1);
  // m->AddTrigger("BBCLL1(>0 tubes)");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(mutrig);       // register subsystem Monitor with Framework
	se->registerMonitor(muid);
  // start_server(prdffile);
  mutriglvl1loopall(filelist,1.0);
  se->EndRun(se->RunNumber()); // 
  se->WriteHistoFile();
  CleanUpServer();
  return ;
}


void run_mutriglvl1(int runnumber)//char *prdffile = "lvl1data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutriglvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MutrigLvl1Mon *m = new MutrigLvl1Mon();
  // m->AddTrigger("BBCLL1(>0 tubes)");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  // start_server(prdffile);
  m->SetOfflineVersion(1);
  mutriglvl1loopall(runnumber);
  return ;
}

void run_mutriglvl1(int segments, int nothing)//char *prdffile = "lvl1data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutriglvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  OnlMon *m = new MutrigLvl1Mon();
  // m->AddTrigger("BBCLL1(>0 tubes)");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  // start_server(prdffile);
  m->SetOfflineVersion(1);
  mutriglvl1loopall_segments(segments);
  return ;
}

void run_mutriglvl1()
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutriglvl1mon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  // create subsystem Monitor object
  MutrigLvl1Mon *m = new MutrigLvl1Mon();
  // m->AddTrigger("BBCLL1(>0 tubes) novertex");  // high efficiency triggers selection at et pool
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  // start_server(prdffile);
  m->SetOfflineVersion(1);
  mutriglvl1loopall();
  return ;
}

void mutriglvl1DrawInit(int online = 0)
{
  gSystem->Load("libonlmutriglvl1mon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  
  // register histos we want with monitor name
  for(int arm_i=0; arm_i<4; arm_i++){
	for(int oct_i=0; oct_i<8; oct_i++){
		cl->registerHisto(Form("MutrigLvl1_SimCountArmTrigOct%d%d",arm_i,oct_i), "MUTRIGLVL1MON");
		
		cl->registerHisto(Form("MutrigLvl1_RealAndSimCountArmTrigOct%d%d",arm_i,oct_i), "MUTRIGLVL1MON");
		// cl->registerHisto(Form("MutrigLvl1_Efficiency_vs_Time_Numerator%d%d",arm_i,oct_i), "MUTRIGLVL1MON");
		// cl->registerHisto(Form("MutrigLvl1_Efficiency_vs_Time_Denomintaor%d%d",arm_i,oct_i), "MUTRIGLVL1MON");
		
		}
	
	cl->registerHisto(Form("MutrigLvl1_RealCountArmTrig%d",arm_i), "MUTRIGLVL1MON");
	cl->registerHisto(Form("MutrigLvl1_SimCountArmTrig%d",arm_i), "MUTRIGLVL1MON");
	cl->registerHisto(Form("MutrigLvl1_RealAndSimCountArmTrig%d",arm_i), "MUTRIGLVL1MON");
	
  }
  cl->registerHisto("MutrigLvl1_InfoHist", "MUTRIGLVL1MON");
  cl->registerHisto("MutrigLvl1_ExpertInfoHist", "MUTRIGLVL1MON");

  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MUTRIGLVL1MON", 1);
  OnlMonDraw *mutriglvl1mon = new MutrigLvl1MonDraw();    // create Drawing Object
  cl->registerDrawer(mutriglvl1mon);              // register with client framework
}

void mutriglvl1Draw(const char *what = "SOUTH")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MUTRIGLVL1MON",1);         // update histos
  cl->Draw("MUTRIGLVL1MON",what);                            // Draw Histos of registered Drawers
}

void mutriglvl1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MUTRIGLVL1MON");
}

void mutriglvl1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MUTRIGLVL1MON");
}

void mutriglvl1PDF()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePDF("MUTRIGLVL1MON");
}
// to use this mutriglvl1loopall() do the following: 
// comment out "start_server(prdffile)"
// change void run_mutriglvl1(char *prdffile = "lvl1data.prdf") to void run_mutriglvl1()//char *prdffile = "lvl1data.prdf")
// root -l
// .x run_mutriglvl1.C
// mutriglvl1loopall()
void mutriglvl1loopall()
{

	char *filen[] =
	{
	
	// "/common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-000036836*.PRDFF"
	
"/common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368366-0000.PRDFF",
"/common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368366-0001.PRDFF",
"/common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368366-0002.PRDFF",
"/common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368366-0003.PRDFF",
"/common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368366-0004.PRDFF"
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368367-0000.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368367-0001.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368367-0002.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368367-0003.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368367-0004.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368368-0000.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368368-0001.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368368-0002.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368368-0003.PRDFF
// /common/p0/TestPRDFF/Run12pp500/EVENTDATA_P00-0000368368-0004.PRDFF

	};
	
	pclose();
	cout << "size of filen/4: " << sizeof(filen)/4 << endl;
	for (int i = 0; i < int(sizeof(filen)/4); i++){
		cout << "prun on filen[#]: " << i << endl;
		pfileopen(filen[i]);
		prun();
	}
	
	return ; 
 }	 
 
void mutriglvl1loopall(char *filelist, double garbage)
{
	cout << "filelist: " << filelist << endl;
	stringstream ss;
	ss << "/home/phnxrc/mutriglvl1/runs/";
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
	cout << "exiting void mutriglvl1loopall(char *filelist, double garbage)" << endl;
	
	return ; 
 }	
 
 void mutriglvl1loopall(int runnumber, int segments)
{

	stringstream ss;
	ss << "ls -1 /common/{a,b,c}{0,1,2,3,4,5,6}/*/*";
	ss << runnumber;
	ss << "*";
	ss << " > mutriglvl1_filelist.list";
	system(ss.str().c_str());
	cout << "system command: " << ss.str().c_str() << endl;

	std::ifstream ifs("mutriglvl1_filelist.list");
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
	cout << "exiting mutriglvl1loopall(int runnumber, int segments)" << endl;
	
	
	return ; 
 }	 
 
void mutriglvl1loopall_segments(int segments)
{
	int runnumber = 0;
	stringstream ss( stringstream::in | stringstream::out);
	std::ifstream in("cron_mutriglvl1_runlist.list");
	std::ofstream out("temp_cron_mutriglvl1_runlist.list");
	std::string line;
	while(std::getline(in, line))
	{
		ss.str("");ss.clear();
		ss << line;
		ss >> runnumber;
	}
	
	in.clear(); // clear bad state after eof
	in.seekg( 0 );
	
	while(std::getline(in, line)){
		int _line;
		ss.str("");ss.clear();
		ss << line;
		ss >> _line;
		if(runnumber==_line) continue;
		out << line << endl;
	}
	
	cout << "runnumber " << runnumber << endl;
	
	in.close();
	out.close();
	remove("cron_mutriglvl1_runlist.list");
	rename("temp_cron_mutriglvl1_runlist.list","cron_mutriglvl1_runlist.list");
	
	cout << "mutriglvl1loopall(runnumber, segments): " << runnumber << " " << segments << endl;
	if(runnumber) mutriglvl1loopall(runnumber, segments);
	else cout << "no runs to process, exiting" << endl;
	
	return ; 
 }	 
 
void mutriglvl1loopall(int runnumber)
{
	mutriglvl1loopall(runnumber, 99999);
	
	return ; 
 }	 