void run_server(char *prdffile="data.prdf")
{
gSystem->Load("libonlpadmon.so");
gSystem->Load("libonlmonserver_funcs.so");
OnlMon *m = new PadMon();      // create subsystem Monitor object
OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
se->registerMonitor(m);       // register subsystem Monitor with Framework
pfileopen(prdffile);
}
