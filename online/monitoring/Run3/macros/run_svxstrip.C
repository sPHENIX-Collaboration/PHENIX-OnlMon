#include <iostream>
#include <sstream>
using namespace std;

void run_svxstrip(const char *prdffile = "data.prdf")
{
    gSystem->Load("libonlmonserver_funcs.so");
    gSystem->Load("libsvx.so");
    gSystem->Load("libonlsvxstripmon.so");
    gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

    OnlMon *m = new SvxStripMon("SVXSTRIPMON");      // create subsystem Monitor object
    m->AddTrigger("BBCLL1(>0 tubes) narrowvtx");

    OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
    se->registerMonitor(m);       // register subsystem Monitor with Framework

    start_server(prdffile);

    return ;
}


void svxstripDrawInit(const int online = 0)
{
    gSystem->Load("libonlsvxstripmon.so");
    gSystem->Load("libonlmonclient.so");
    gROOT->ProcessLine(".L CommonFuncs.C");
    OnlMonClient *cl = OnlMonClient::instance();
    // register histos we want with monitor name

    stringstream name;
    stringstream desc;

    const int vtxs_packet_base = 24101;

    //Only register histograms needed by POMS
    cl->registerHisto("vtxs_params", "SVXSTRIPMON");
    cl->registerHisto("vtxsh1rcccount", "SVXSTRIPMON");
    cl->registerHisto("hcellpro", "SVXSTRIPMON");
    cl->registerHisto("hrccbclock", "SVXSTRIPMON");
    cl->registerHisto("hrccbclockerror", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_h2live", "SVXSTRIPMON");
    cl->registerHisto("hknownmask", "SVXSTRIPMON");

    /*
    //these aren't needed by POMS
    cl->registerHisto("svxstrip_hmulti", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_h2sadc", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_hsadc_bytime", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_hnbbc_bytime", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_hsadc_bytime_nohot", "SVXSTRIPMON");
    cl->registerHisto("hcellid", "SVXSTRIPMON");
    cl->registerHisto("hbclock", "SVXSTRIPMON");
    cl->registerHisto("hpacnt", "SVXSTRIPMON");
    cl->registerHisto("htimeouterr", "SVXSTRIPMON");
    cl->registerHisto("hhybridmask", "SVXSTRIPMON");
    */


    /*
    //Only necessary if calling DrawLadder() in POMS
    for (int ipacket = 0; ipacket < 40; ipacket++)
    {
        for (int rcc = 0; rcc < 6; rcc++)
        {
            name.str(string());
            name << "h2svxstrip_adc_" << ipacket << "_" << rcc;
            cl->registerHisto(name.str().c_str(), "SVXSTRIPMON");
        }
    }
    */

    CreateHostList(online);
    // get my histos from server, the second parameter = 1
    // says I know they are all on the same node
    cl->requestHistoBySubSystem("SVXSTRIPMON", 1);
    OnlMonDraw *svxstripmon = new SvxStripMonDraw("SVXSTRIPMON");    // create Drawing Object
    cl->registerDrawer(svxstripmon);              // register with client framework
}

void svxstripDraw(const char *what = "ALL")
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->requestHistoBySubSystem("SVXSTRIPMON");         // update histos
    cl->Draw("SVXSTRIPMON", what);                      // Draw Histos of registered Drawers
}

void svxstripPS()
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->MakePS("NONE");                          // Create PS files
    return;
}

void svxstripHtml()
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->MakeHtml("NONE");                        // Create html output
    return;
}

void svxstripDrawExpertInit(const int online = 0)
{
    gSystem->Load("libonlsvxstripmon.so");
    gSystem->Load("libonlmonclient.so");
    gROOT->ProcessLine(".L CommonFuncs.C");
    OnlMonClient *cl = OnlMonClient::instance();
    // register histos we want with monitor name

    stringstream name;
    stringstream desc;

    const int vtxs_packet_base = 24101;

    //Only register histograms needed by POMS
    cl->registerHisto("vtxs_params", "SVXSTRIPMON");
    cl->registerHisto("vtxsh1rcccount", "SVXSTRIPMON");
    cl->registerHisto("hcellpro", "SVXSTRIPMON");
    cl->registerHisto("hrccbclock", "SVXSTRIPMON");
    cl->registerHisto("hrccbclockerror", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_h2live", "SVXSTRIPMON");
    cl->registerHisto("hknownmask", "SVXSTRIPMON");
    
    //these aren't needed by POMS
    cl->registerHisto("svxstrip_hmulti", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_h2sadc", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_hsadc_bytime", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_hnbbc_bytime", "SVXSTRIPMON");
    cl->registerHisto("svxstrip_hsadc_bytime_nohot", "SVXSTRIPMON");
    cl->registerHisto("hcellid", "SVXSTRIPMON");
    // cl->registerHisto("hbclock", "SVXSTRIPMON");
    cl->registerHisto("hpacnt", "SVXSTRIPMON");
    cl->registerHisto("htimeouterr", "SVXSTRIPMON");
    cl->registerHisto("hhybridmask", "SVXSTRIPMON");
    


    
    //Only necessary if calling DrawLadder() in POMS
    for (int ipacket = 0; ipacket < 40; ipacket++)
    {
        for (int rcc = 0; rcc < 6; rcc++)
        {
            name.str(string());
            name << "h2svxstrip_adc_" << ipacket << "_" << rcc;
            cl->registerHisto(name.str().c_str(), "SVXSTRIPMON");
        }
    }
    

    CreateHostList(online);
    // get my histos from server, the second parameter = 1
    // says I know they are all on the same node
    cl->requestHistoBySubSystem("SVXSTRIPMON", 1);
    OnlMonDraw *svxstripmon = new SvxStripMonDraw("SVXSTRIPMON");    // create Drawing Object
    cl->registerDrawer(svxstripmon);              // register with client framework
}

void svxstripDrawExpert(const char *what = "EXPERT")
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->requestHistoBySubSystem("SVXSTRIPMON");         // update histos
    cl->Draw("SVXSTRIPMON", what);                      // Draw Histos of registered Drawers
}

