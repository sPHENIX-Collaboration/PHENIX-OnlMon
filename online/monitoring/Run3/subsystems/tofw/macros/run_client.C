void run_client()
{
  gSystem->Load("libonltofmon.so");
  gSystem->Load("libonlmonclient.so");
  OnlMonClient *cl = OnlMonClient::instance();

  // register histos we want with monitor name 
  cl->registerHisto("htofinfo","TOFMON"); 
  cl->registerHisto("htof210","TOFMON"); 
  cl->registerHisto("htof220","TOFMON");
  cl->registerHisto("htof230","TOFMON");
  //cl->registerHisto("htof410","TOFMON");
  cl->registerHisto("htof420","TOFMON");
  //cl->registerHisto("htof520","TOFMON");
  cl->registerHisto("htof530","TOFMON");

  cl->AddServerHost("localhost"); // check local host first
  char node[10];
  for (int i=61;i<=78;i++){
    sprintf(node,"rcas20%02d",i);
    cl->AddServerHost(node); // put all rcas machines in search list
  }
  cl->requestHistoBySubSystem("TOFMON",1); // get my histos from server, the second parameter = 1 says I know they are all on the same node
  OnlMonDraw *tofmon = new TOFMonDraw();// create Drawing Object
  cl->registerDrawer(tofmon); // register with client framework
}

void draw_client()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("TOFMON");
  cl->Draw();
}

void makehtml_client()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("TOFMON");
  cl->MakeHtml();
}

void makeps_client()
{
  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("TOFMON");
  cl->MakePS();
}
