void run_server(char *prdffile="data.prdf")
{
gSystem->Load("libonlrichmon.so");
gSystem->Load("libonlmonserver_funcs.so");
OnlMon *m = new RICHMon();      // create subsystem Monitor object
OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
se->registerMonitor(m);       // register subsystem Monitor with Framework
pfileopen(prdffile);
}
