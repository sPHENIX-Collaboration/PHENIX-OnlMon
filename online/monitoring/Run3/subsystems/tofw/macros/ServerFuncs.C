void start_server(const char *prdffile="data.prdf")
{
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  // set the ONLMONBBCLL1 Trigger definition
  se->OnlTrig()->AddBbcLL1TrigName("BBCLL1(>0 tubes)");
  if (!strcmp(prdffile, "etpool"))
    {
     //     gSystem->Load("libcorbamsgbuffer.so");
     //      corba_msg_buffer *enablecorbabuf = new corba_msg_buffer("monitor_event_channel");
      petopen("/tmp/Monitor@etpool");
      prun();
      //      delete enablecorbabuf;
      //  if etpool gets restarted we execute the EndRun and then
      // save the histograms
      se->EndRun(se->RunNumber()); // 
      se->WriteHistoFile();
      cout << "ET pool dead, exiting now" << endl;
      exit(0);
    }
  else
    {
      pfileopen(prdffile);
    }
  return;
}
