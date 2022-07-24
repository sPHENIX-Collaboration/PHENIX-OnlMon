void run_mutrg(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlmutrgmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  MutrgPar::IS_READ_NSTRIP_DB = false;
  OnlMon *m = new MutrgMon(MutrgMonPar::MONNAME); // create subsystem Monitor object
  //   m->AddTrigger("BBCLL1(>1 tubes)");
  //   m->AddTrigger("BBCLL1(>0 tubes)");
  //   m->AddTrigger("MUIDLL1_N1D||S1D");
  //   m->AddTrigger("(MUIDLL1_N1D||S1D)&BBCLL1(noVtx)");
  //   m->AddTrigger("(MUIDLL1_N1D||S1D)&BBCLL1");

  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

  return ;
}

void mutrgDrawInit(const int online = 0)
{
  gSystem->Load("libonlmutrgmon.so");
  gSystem->Load("libonlmonclient.so");
  //gSystem->Load("libmutrg.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *client = OnlMonClient::instance();

  for (int arm = 0; arm < MutrgPar::NARM; arm++)
    {
      for (int station = 0; station < MutrgPar::NSTATION; station++)
        {
          for (int octant = 0; octant < MutrgPar::NOCTANT; octant++)
            {
              //char hist_name[100];
              //sprintf(hist_name,"mutrg_hit_dist_a%d_s%d_o%d",arm,station,octant);
              //client->registerHisto(hist_name, MutrgMonPar::MONNAME);
              //char hist_name_m[100];
              //sprintf(hist_name_m,"mutrg_hit_dist_m_a%d_s%d_o%d",arm,station,octant);
              //client->registerHisto(hist_name_m, MutrgMonPar::MONNAME);
              char hist_name_b[100];
              sprintf(hist_name_b, "mutrg_hit_dist_b_a%d_s%d_o%d", arm, station, octant);
              client->registerHisto(hist_name_b, MutrgMonPar::MONNAME);
              //char hist_name_mb[100];
              //sprintf(hist_name_mb,"mutrg_hit_dist_mb_a%d_s%d_o%d",arm,station,octant);
              //client->registerHisto(hist_name_mb, MutrgMonPar::MONNAME);
              //char hist_name_mbnv[100];
              //sprintf(hist_name_mbnv,"mutrg_hit_dist_mbnv_a%d_s%d_o%d",arm,station,octant);
              //client->registerHisto(hist_name_mbnv, MutrgMonPar::MONNAME);
              char hist_name_w_mutr[100];
              sprintf(hist_name_w_mutr, "mutrg_hit_dist_w_mutr_a%d_s%d_o%d", arm, station, octant);
              client->registerHisto(hist_name_w_mutr, MutrgMonPar::MONNAME);
            }
        }
    }
  client->registerHisto("mutrg_total_trg", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_total_trg_m", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_total_trg_b", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_total_trg_mb", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_total_trg_mbnv", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_total_trg_w_mutr", MutrgMonPar::MONNAME);

  client->registerHisto("mutrg_error_sum", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_error_mrg", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_error_dcmif", MutrgMonPar::MONNAME);
  client->registerHisto("mutrg_error_parity", MutrgMonPar::MONNAME);

  for (int arm = 0; arm < MutrgPar::NARM; arm++)
    {
      for (int station = 0; station < MutrgPar::NSTATION; station++)
        {
          for (int octant = 0; octant < MutrgPar::NOCTANT; octant++)
            {
              char hist_name[100];
              //sprintf(hist_name,"mutrg_hit_wo_mutr_a%d_s%d_o%d",arm,station,octant);
              //client->registerHisto(hist_name, MutrgMonPar::MONNAME);
              //sprintf(hist_name,"mutrg_hit_nm_mutr_a%d_s%d_o%d",arm,station,octant);
              //client->registerHisto(hist_name, MutrgMonPar::MONNAME);
              sprintf(hist_name, "mutrg_hit_clk0_a%d_s%d_o%d", arm, station, octant);
              client->registerHisto(hist_name, MutrgMonPar::MONNAME);

              sprintf(hist_name, "mutr_hit_ELdist_a%d_s%d_o%d", arm, station, octant);
              client->registerHisto(hist_name, MutrgMonPar::MONNAME);

              sprintf(hist_name, "mutr_hit_w_mutrg_ELdist_a%d_s%d_o%d", arm, station, octant);
              client->registerHisto(hist_name, MutrgMonPar::MONNAME);

              sprintf(hist_name, "mutr_hit_w_mutrg_narrow_ELdist_a%d_s%d_o%d", arm, station, octant);
              client->registerHisto(hist_name, MutrgMonPar::MONNAME);
            }
        }
    }
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  client->requestHistoBySubSystem("MUTRGMON", 1);

  OnlMonDraw *online_mon_draw = new MutrgMonDraw(MutrgMonPar::MONNAME); // create Drawing Object
  client->registerDrawer(online_mon_draw);              // register with client framework register histos we want with monitor name
}

void mutrgDraw(const char *what = "SUMMARY")
{
  OnlMonClient *client = OnlMonClient::instance();  // get pointer to framewrk
  client->requestHistoBySubSystem(MutrgMonPar::MONNAME, 1);        // update histos
  client->Draw(MutrgMonPar::MONNAME, what);                      // Draw Histos of registered Drawers
}

void mutrgPS()
{
  OnlMonClient *client = OnlMonClient::instance();  // get pointer to framewrk
  client->requestHistoBySubSystem(MutrgMonPar::MONNAME, 1);        // update histos
  client->MakePS(MutrgMonPar::MONNAME);                          // Create PS files
  return;
}

void mutrgHtml()
{
  OnlMonClient *client = OnlMonClient::instance();  // get pointer to framewrk
  client->requestHistoBySubSystem(MutrgMonPar::MONNAME, 1);        // update histos
  client->MakeHtml(MutrgMonPar::MONNAME);                        // Create html output
  return;
}
