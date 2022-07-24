void run_server(char *prdffile="data.prdf")
{
  gSystem->Load("libonltofmon.so");
  gSystem->Load("libonlmonserver_funcs.so");
  OnlMon *m = new TOFMon();
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  pfileopen(prdffile);
}
