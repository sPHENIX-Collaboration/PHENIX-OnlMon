void CreateHostList(int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  char node[10];
  if (!online)
    {
      cl->AddServerHost("localhost");   // check local host first
      if (!strncmp("rcas", getenv("HOST"), 4)) // check if we are on rcas
        {
          cl->AddServerHost("rplay26");
          for (int i = 61;i <= 78;i++)
            {
              sprintf(node, "rcas20%02d", i);
              cl->AddServerHost(node);      // put all rcas machines in search list
            }
        }
      else // if not rcas it's va
        {
          for (int i = 1;i <= 75;i++)
            {
	      if (i != 58) // take out objy machine
		{
		  sprintf(node, "va%03d", i);
		  cl->AddServerHost(node);      // put all va machines in search list
		}
            }
        }
    }
  else if (online == 1)
    {
      for (int i = 17;i <= 29;i++)
        {
	  if (i == 28)
	    {
	      continue;
	    }
          sprintf(node, "va%03d", i);
          cl->AddServerHost(node);      // put monitoring va machines in search list
        }
    }
  else
    {
      cl->AddServerHost("localhost");   // check local host first
    }
}
