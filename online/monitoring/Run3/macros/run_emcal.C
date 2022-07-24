void run_emcal(const char *prdffile = "emcdata.prdf")
{
  // for the database stuff

  const char * emcref = "$EMCREFROOT";

  if (emcref == NULL)
    {
      Error("getenv", "Load doesn't environment variable:EMCREFROOT");
      return ;
    }
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlemcalmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new EMCalMon(emcref);
  
  m->AddTrigger("BBCLL1(>0 tubes)");
  m->AddTrigger("ERT_4x4a&BBCLL1");
  m->AddTrigger("ERT_4x4b");
  m->AddTrigger("ERT_4x4a");

  m->AddTrigger("PPG(Pedestal)");
  m->AddTrigger("PPG(Test Pulse)");
  m->AddTrigger("PPG(Laser)");
  OnlMonServer *se = OnlMonServer::instance();
  se->registerMonitor(m);
  start_server(prdffile);
  return;
}

void run_emcal_multifile(const char *listfile = "listevent")
{
   // for the database stuff

  const char * emcref = "$EMCREFROOT";

  if (emcref == NULL)
  {
      Error("getenv", "Load doesn't environment variable:EMCREFROOT");
      return ;
  }
   gSystem->Load("libonlemcalmon.so");
   gSystem->Load("libonlmonserver_funcs.so");
   gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  EMCalMon *m = new EMCalMon(emcref);
  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");
  // m->AddTrigger("BBCLL1(>0 tubes) novertex");
  // m->AddTrigger("ERT_4x4b");
  // m->AddTrigger("ERT_4x4a&BBCLL1(noVtx)");
  // m->AddTrigger("ERT_4x4c&BBCLL1(noVtx)");
  m->AddTrigger("ERT_4x4a&BBCLL1");
  m->AddTrigger("ERT_4x4b");
  m->AddTrigger("ERT_4x4a");
  //  m->AddTrigger("ERT_4x4b&BBCLL1");
  // m->AddTrigger("ERT_4x4c&BBCLL1(narrow)"); 
  //m->AddTrigger("ERT_4x4c&BBCLL1");
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  //  se->OnlTrig()->AddBbcLL1TrigName("BBCLL1(>1 tubes)");

  ifstream fin(listfile);

  if(fin.bad()){
    cout<<"no listfile"<<endl;
    return;
  }
  char prdffile[255], filename[80];
 

  while(!fin.eof()){
    prdffile[0]='\0';
    fin>>prdffile;     
    cout<<"Open : "<<prdffile<<endl;
    pfileopen(prdffile);
    // gROOT->ProcessLine("prun()");
    prun();
    // pclose();     
  }

  sprintf(filename, "emc_%d.root", se->RunNumber());
  se->dumpHistos(filename);
  se->EndRun(se->RunNumber());
}

void emcalDrawInit(const int online = 0)
{
  gSystem->Load("libonlemcalmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");

  const char *histNames[ ] =
  {

    "PBGL_sm",
    "PBGL_ref",
    "PBGL_T_sm",
    "PBGL_T_ref",

    "PBSC_sm",
    "PBSC_ref",
    "PBSC_T_sm",
    "PBSC_T_ref",

    "Refer_PBGL",
    "ReferPin_PBGL",
    "Refer_T_PBGL",
    "ReferPin_T_PBGL",

    "Refer_PBSC",
    "ReferPin_PBSC",
    "Refer_T_PBSC",
    "ReferPin_T_PBSC",
    "Refer_TP_PBSC",
    "Refer_T_TP_PBSC",
    "PBSC_TP",
    "PBSC_T_TP", 
    
    "avPBGL_SM",
    "avPBGL_T_SM",
    "dispPBGL_SM",
    "dispPBGL_T_SM",
    
    "avPBGL_REF",
    "avPBGL_T_REF",
    "dispPBGL_REF",
    
    "avPBSC_SM",
    "avPBSC_T_SM",
    "dispPBSC_SM", 
    "dispPBSC_T_SM",

    "avPBSC_REF",
    "avPBSC_T_REF",
    "dispPBSC_REF",   
  
    "EMCal_Info" 
  };
  const int numbHists = 35;  

  OnlMonClient *cl = OnlMonClient::instance();
  for (int i = 0; i < numbHists; i++)
  {
      cl->registerHisto(histNames[i], "EMCalMON");
  }

  const  char *mchtitle[2][10] = { 
              {"BBCLL1_PBSC_2_6GeV_c","BBCLL1_PBSC_W0_2_6GeV_c","BBCLL1_PBSC_W1_2_6GeV_c","BBCLL1_PBSC_W2_2_6GeV_c", 
                                      "BBCLL1_PBSC_W3_2_6GeV_c","BBCLL1_PBSC_E2_2_6GeV_c","BBCLL1_PBSC_E3_2_6GeV_c", 
               "BBCLL1_PBGL_2_6GeV_c","BBCLL1_PBGL_E0_2_6GeV_c","BBCLL1_PBGL_E1_2_6GeV_c" },
              {"Gamma3_PBSC_2_6GeV_c","Gamma3_PBSC_W0_2_6GeV_c","Gamma3_PBSC_W1_2_6GeV_c","Gamma3_PBSC_W2_2_6GeV_c", 
                                      "Gamma3_PBSC_W3_2_6GeV_c","Gamma3_PBSC_E2_2_6GeV_c","Gamma3_PBSC_E3_2_6GeV_c", 
               "Gamma3_PBGL_2_6GeV_c","Gamma3_PBGL_E0_2_6GeV_c","Gamma3_PBGL_E1_2_6GeV_c" } 
  };                
    

  for(int itr = 0; itr < 2; itr++)
      for(int ih= 0; ih < 10; ih++)
      {
      
         cl->registerHisto(mchtitle[itr][ih], "EMCalMON");
      }

  cl->registerHisto("emc_etot", "EMCalMON");
  CreateHostList(online);

  // says I know they are all on the same node
  cl->requestHistoBySubSystem("EMCalMON", 1);
  OnlMonDraw *emcmon = new EMCalMonDraw();    // create Drawing Object
  cl->registerDrawer(emcmon);              // register with client framework

}

void emcalDraw(const char *what="SHIFT")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("EMCalMON",1);    // update histos
  cl->Draw("EMCalMON", what);

}

void emcalPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("EMCalMON");                       // Make PS files
}

void emcalHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("EMCalMON");                     // create html output
}



// if you want to use this method do not read file with 
// .x run_emcal.C("prdffile")
// do 
// .L run_emcal.C
//  run_emcal("prdffile")
// saveReference()
void saveReference(const char *filename="emc_ref.root")
{
  OnlMonServer *se = OnlMonServer::instance();
  se->dumpHistos(filename);
}
