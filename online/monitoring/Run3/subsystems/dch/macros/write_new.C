//use a GOOD run and run this macro to create dead/noise/templates files
//run an appropriate number of events, for AuAu 1000 events seem ok.

void write_new(char *prdffile="data.prdf", char *prdffile1="data.prdf")
{
  gSystem->Load("libonlmonserver.so");
  gSystem->Load("libonlmonserver_funcs.so");
  //gSystem->Load("libonlinerecorder.so");
  gSystem->Load("libonldchmon.so");

  DchMon *dchmon = new DchMon();

  /*   These are no longer used; all thresholds, etc. are written and read from DchTemplates.txt
       i.e. , no more DchTemplates_Species.txt files
  //pp
  //dchmon->set_running_average(1);
  //dchmon->set_update_rate(10000);
  //dchmon->set_stat_threshold(4000);
  //dchmon->set_dead_threshold(0.001);
  //dchmon->set_noise_threshold(1.0);
  //dchmon->set_templates_dAu();

  //AuAu
  dchmon->set_running_average(0);
  dchmon->set_update_rate(1000);
  dchmon->set_stat_threshold(300);
  dchmon->set_dead_threshold(0.03);
  dchmon->set_noise_threshold(1.0);
  dchmon->set_templates_AuAu();

  //dAu
  //dchmon->set_templates_dAu();
*/ //DLAN

  //Set Everything that is stored in DchTemplates.txt that is _not_ calculated:
  //This is the only place where all the species associated thresholds, etc.
  //are set; this macro can be used to write a new DchTemplates.txt file.
  //AuAu
  dchmon->set_running_average(0);
  dchmon->set_update_rate(1000);
  dchmon->set_stat_threshold(300);
  dchmon->set_dead_threshold(0.03);
  dchmon->set_noise_threshold(1.0);
  //set_HIT_PARAMS() arguments are: SIGMA_HITRATE, LOW_GOOD, HIGH_GOOD, LOW_HOT, HIGH_HOT, LOW_COLD, HIGH_COLD 
  dchmon->set_HIT_PARAMS(0.18,-3.,3.,3.,6.,-5.2.,-3.); //for AuAu
  //VGR dchmon->set_HIT_PARAMS(0.015,-3.,3.,3.,6.,-6.,-3.); //for AuAu

  //set_ALERT_PARAMS arguments are: LLOAD_ALERT_RED, HLOAD_ALERT_RED, LLOAD_ALERT_YEL, HLOAD_ALERT_YEL  
  dchmon->set_ALERT_PARAMS(0.12,0.550,0.16,0.590); //for AuAu 
  //VGR dchmon->set_ALERT_PARAMS(11,22,12,21); //for AuAu 
  //set_ALERT_PARAMS(0.4,3.5,0.8,2.6); //for dAu 
  //set_ALERT_PARAMS(0.12,0.550,0.16,0.590); //for pp 

  OnlMonServer *se = OnlMonServer::instance(); 
  //  se->OnlTrig()->AddBbcLL1TrigName("BBCLL1(>0 tubes)"); // removed benjil 10Dec2009
  se->registerMonitor(dchmon);    
  pfileopen(prdffile);
  prun(-1);
  pfileopen(prdffile1);
  prun(-1);

  dchmon->write_all_files(); // No more species //DLAN	
}

