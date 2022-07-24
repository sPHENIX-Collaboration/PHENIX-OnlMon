void run_server(char *prdffile="data.prdf")
{
  // for the database stuff
  gSystem->Load("libPdbCal.so");
  gSystem->Load("libvtx.so");
  gSystem->Load("libmut.so");
  //
  gSystem->Load("libonlmutrmon.so");
  gSystem->Load("libonlmonserver_funcs.so");

  //  OnlMon *m = new MutrMon();  
  // the following 2 lines replaces the line above for tmp Run-2 testing
  MutrMon *m = new MutrMon();
  m->getCalibrations(2001,10,29,20,19,0); // valid for data.prdf      

  OnlMonServer *se = OnlMonServer::instance(); 
  se->registerMonitor(m);       
  pfileopen(prdffile);
}
