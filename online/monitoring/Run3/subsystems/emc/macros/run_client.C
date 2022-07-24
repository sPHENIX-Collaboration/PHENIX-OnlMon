
const char *histNames[ ] = {

      "PBGL_sm",
      "PBGL_SM",
      "PBGL_ref",
      "PBGL_REF",    
      "PBGL_T_sm",  
      "PBGL_T_SM",
      "PBGL_T_ref",  
      "PBGL_T_REF", 
       
      "PBSC_sm", 
      "PBSC_SM", 
      "PBSC_ref", 
      "PBSC_REF",
      "PBSC_T_sm",
      "PBSC_T_SM", 
      "PBSC_T_ref",    
      "PBSC_T_REF", 

      "PBGL_SM;1",
      "PBGL_REF;1",
      "PBGL_T_SM;1",
      "PBGL_T_REF;1",
      
      "PBSC_SM;1",   
      "PBSC_REF;1",
      "PBSC_T_SM;1",
      "PBSC_T_REF;1"    
};

const int numbHists = 24;

void run_client()
{
  gSystem->Load("libonlemcalmon.so");
  gSystem->Load("libonlmonclient.so");
  OnlMonClient *cl = OnlMonClient::instance();
  
  for(int i =0; i < numbHists; i++)
  {
     cl->registerHisto(histNames[i],"EMCalMON");
  }
  
  cl->AddServerHost("localhost");   // check local host first
  
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("EMCalMON", 1);
  OnlMonDraw *mymon = new EMCalMonDraw();    // create Drawing Object
  cl->registerDrawer(mymon);              // register with client framework
}

void draw_client()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("EMCalMON");         // update histos
  cl->Draw();                            // Draw Histos of registered Drawers
}









