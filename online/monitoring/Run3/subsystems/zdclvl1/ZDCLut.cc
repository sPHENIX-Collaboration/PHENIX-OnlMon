#include <ZDCLut.h>
#include <ZdcLvl1MonDefs.h>
#include <OnlMonServer.h>

#include <msg_profile.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

ZDCLut::ZDCLut(const OnlMon *parent)
{
  mymon = parent;
  string line, newline;
  char *board_loc;
  const char *space;
  unsigned int dummy1, dummy2, value;
  int board, channel, entry = 0;
  int number = 0, ch = -1;

  // Open the file with the FEMLUT directory list:
  // space=getenv("ZDCLUTCONFIG");
  if (!strlen(zdcll1map.c_str()))
    {
      ostringstream msg;
      msg << "ZDCLut: Can't open LUT file = " << zdcll1map << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 3);
      success = 0;
      return ;
    }
  ifstream fin(zdcll1map.c_str());
  if (!fin)
    {
      ostringstream msg;
      msg << "ZDCLut: Can't open FEMLUT directory file list" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 3);
      success = 0;
      return ;
    }
  else
    {

      while (fin)
        {
          number++;
          getline(fin, line);

          if (line.find("init") == string::npos)
            {
              if (line.find("end") == string::npos)
                {
                  if (line.find("001_ch") == string::npos)
                    {
                      if (line.find("130_ch") == string::npos)
                        {
		      // JGL 2/25/09 - need to add this now that we have SMD boards 
                      if (line.find("board01") != string::npos)
                        {
                          space = strstr(line.c_str(), " ");
                          if (space)
                            {

                              // Auto-allocate these to avoid overflows
                              char channelpath[line.length()];
                              char temp2[line.length()];
                              char temp1[line.length()];

                              strcpy(channelpath, space + 1);

                              board_loc = strstr(channelpath, "board");
                              if (board_loc)
                                {
                                  strcpy(temp1, board_loc + 5);
                                  board = atoi(&temp1[0]);  
                                  strcpy(temp2, board_loc + 14);
                                  channel = atoi(&temp2[0]);
                                  if (ch != channel)
                                    {
                                      entry = 0;
                                      ch = channel;
                                    }
                                  if (entry >= 2048)
                                    {
                                      ostringstream msg;
                                      msg << "ZDCLl1: Error in entry " << entry;
                                      OnlMonServer *se = OnlMonServer::instance();
                                      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 4);
                                      success = 0;
                                      return ;
                                    }

                                  ifstream fp(channelpath);
                                  if (!fp)
                                    {
                                      ostringstream msg;
                                      msg << "ZDCLut: Can't open file " << channelpath;
                                      OnlMonServer *se = OnlMonServer::instance();
                                      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 5);
                                      success = 0;
                                      return ;
                                    }

                                  // skip two header lines
                                  getline(fp, newline);
                                  getline(fp, newline);

                                  getline(fp, newline);
                                  while (newline.length() > 0)
                                    {
                                      const char *newlineptr = newline.c_str();
                                      sscanf(newlineptr, "%x%x%x", &dummy1, &dummy2, &value);
                                      if (dummy1 != 1 || dummy2 != 0xd0)
                                        {
                                          ostringstream msg;
                                          msg << "ZDCLut: wrong line in file " << channelpath << "." << newline;
                                          OnlMonServer *se = OnlMonServer::instance();
                                          se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 6);
                                          success = 0;
                                          return ;
                                        }
                                      LUT[channel][entry] = (unsigned char)value;
                                      //printf("channel=%d entry=%d newline=%s\n",channel,entry,newlineptr);
                                      entry++;
                                      getline(fp, newline);
                                    }

                                }
                            }
                        }
			}
		    }
                }
            }
        }
    }

  success = 1;
}


void ZDCLut::fillLL1array(unsigned char* ll1array, const int* T2array)
{
  int ch, value;
  for (ch = 0;ch < 8;ch++)
    {
      value = T2array[ch] / 2;
      if (value < 0 || value >= 2048)
        {
          ll1array[ch] = 0;
        }
      else
        {
          ll1array[ch] = LUT[ch][value];
        }
    } // end loop over channels
  return ;
}
