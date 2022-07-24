void run_client()
{
  gSystem->Load("libonlpadmon.so");
  gSystem->Load("libonlmonclient.so");

  OnlMonClient *cl = OnlMonClient::instance();

  cl->registerHisto("padProfPadSum0","PADMON");

  // Define host
  cl->AddServerHost("localhost");   // check local host first
  //  char node[10];
  //  for (int i=61;i<=78;i++)
  //    {
  //      sprintf(node,"rcas20%02d",i);
  //      cl->AddServerHost(node);      // put all rcas machines in search list
  //    }
  //  sprintf(node,"rcas2068");
  //  cl->AddServerHost(node);      // put only phnxpad in search list


  // get my histos from server, the second parameter = 1 
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("PADMON", 1);

  // create Drawing Object; MyMonDraw inherits from OnlMonDraw
  OnlMonDraw *padmon = new PadMonDraw();    
  // register with client framework; this pops up a window
  cl->registerDrawer(padmon); 

}

void draw_client(){

  OnlMonClient *cl = OnlMonClient::instance();
  cl->requestHistoBySubSystem("PADMON",1);
  cl->Draw();                            
}

void MakeHtml_client(){
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("PADMON",1);      // update histos
  cl->MakeHtml();
}
