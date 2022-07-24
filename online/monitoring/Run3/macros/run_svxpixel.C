void run_svxpixel(const char *prdffile = "data.prdf")
{
  gSystem->Load("libonlmonserver_funcs.so");
  gSystem->Load("libonlsvxpixelmon.so");
  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new SvxPixelMon("SVXPIXELMON");      // create subsystem Monitor object
  m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");

  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}


void svxpixelDrawInit(const int online = 0)
{
  gSystem->Load("libonlsvxpixelmon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name

  ostringstream name;

  cl->registerHisto("vtxp_params","SVXPIXELMON");
  cl->registerHisto("vtxph2map_0_0","SVXPIXELMON");
  cl->registerHisto("vtxph2map_1_0","SVXPIXELMON");
  cl->registerHisto("vtxph2map_0_1","SVXPIXELMON");
  cl->registerHisto("vtxph2map_1_1","SVXPIXELMON");

  TString hname;
  for(int barrel=0; barrel<2; barrel++)
    {
      for(int arm=0; arm<2; arm++)
        {
          for(int type=0; type<6; type++)
            {
              hname="vtxph1chipcount_"; hname+=barrel; hname+="_"; hname+=arm; hname+="_"; hname+=type;
              cl->registerHisto(hname.Data(), "SVXPIXELMON");
            }
        }
    }

  cl->registerHisto("vtxp_multi_time","SVXPIXELMON");
  cl->registerHisto("vtxp_multi_time_2d","SVXPIXELMON");

  /*
  //necessary if plotting deadmaps in POMS
  for(int spiro=0; spiro<60; spiro++)
    {
      for(int chip=0; chip<8; chip++)
        {
          name.str("");
          name<<"vtxp_chipmap_" << spiro << "_" << chip;
          cl->registerHisto(name.str().c_str(),"SVXPIXELMON");
        }
    }
  */


/*
  for(int ipacket=0; ipacket<60 ;ipacket++)
    {
      name.str("");
      name << "vtxp_multi_time_" << ipacket;
      cl->registerHisto(name.str().c_str(),"SVXPIXELMON");
    }
*/



  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("SVXPIXELMON", 1);
  OnlMonDraw *svxpixelmon = new SvxPixelMonDraw("SVXPIXELMON");    // create Drawing Object
  cl->registerDrawer(svxpixelmon);                                 // register with client framework
}

void svxpixelDraw(const char *what="ALL")
{
//  TStopwatch sw;
//  sw.Start();
//  sw.Stop(); cout<<"a1 "<<sw.RealTime()<<endl; sw.Start();
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
//  sw.Stop(); cout<<"a2 "<<sw.RealTime()<<endl; sw.Start();
  cl->requestHistoBySubSystem("SVXPIXELMON");   // update histos
//  sw.Stop(); cout<<"a3 "<<sw.RealTime()<<endl; sw.Start();
  cl->Draw("SVXPIXELMON",what);                 // Draw Histos of registered Drawers
//  sw.Stop(); cout<<"a4 "<<sw.RealTime()<<endl;
}

void svxpixelPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("NONE");                           // Create PS files
  return;
}

void svxpixelHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("NONE");                         // Create html output
  return;
}
