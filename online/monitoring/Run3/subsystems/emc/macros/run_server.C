void run_server(char *prdffile="data.prdf")
{
  // for the database stuff
  
  const char *emcref = "$EMCREFROOT";

  if(emcref == NULL)
  {
    Error("getenv", "Load doesn't environment variable:EMCREFROOT"); 
    return;
  }
  gSystem->Load("libonlemcalmon.so");
  gSystem->Load("libonlmonserver_funcs.so");

  
  EMCalMon *m = new EMCalMon(emcref);
       

  OnlMonServer *se = OnlMonServer::instance(); 
  se->registerMonitor(m);       
  pfileopen(prdffile);
}





