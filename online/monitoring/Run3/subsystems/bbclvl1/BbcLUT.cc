#include "BbcLUT.h"
#include "BbcLvl1MsgTypes.h"
#include "OnlMonServer.h"

#include "msg_profile.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>


using namespace std;

BbcLUT::BbcLUT(const OnlMon *parentmon)
{
  mymon = parentmon;
  string line, newline;
  const char *space, *board_loc;
  const char *lineptr;
  unsigned int dummy1, dummy2, value;
  int channel, entry = 0;
  int number = 0, ch = -1;

  // Open the file with the FEMLUT directory list:
  // space=getenv("BBCLUTCONFIG");
  space = "/export/software/oncs/online_configuration/Arcnet/bb/acf/bbl1_triggermap.acf";
  if (!strlen(space))
    {

      ostringstream msg;
      msg << "BbcLUT: Can't open LUT file " << space;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      success = 0;
      return ;
    }
  ifstream fin(space);
  if (!fin)
    {
      ostringstream msg;
      msg << "BbcLUT: Can't open FEMLUT directory file list" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
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
                      if (line.find("106_ch") == string::npos)
                        {

                          lineptr = line.c_str();
                          space = strstr(lineptr, " ");
                          if (space)
                            {

                              // Auto-allocate these to avoid overflows
                              char channelpath[line.length()];
                              char temp2[line.length()];

                              strcpy(channelpath, space + 1);

                              board_loc = strstr(channelpath, "board");
                              if (board_loc)
                                {
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
                                      msg << "BbcLl1: Error in entry " << entry << endl;
                                      OnlMonServer *se = OnlMonServer::instance();

                                      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), LUTFORMAT);
                                      success = 0;
                                      return ;
                                    }

                                  ifstream fp(channelpath);
                                  if (!fp)
                                    {
                                      ostringstream msg;
                                      msg << "BbcLut: Can't open file " << channelpath << endl;
                                      OnlMonServer *se = OnlMonServer::instance();

                                      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
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
                                          msg << "BbcLut: wrong line in file " << channelpath << "." << newline << endl;
                                          OnlMonServer *se = OnlMonServer::instance();
                                          se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), LUTFORMAT);
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

  success = 1;
}


void BbcLUT::fillLL1array(unsigned char* ll1array, const int* T2array)
{
  int ch, value;
  for (ch = 0;ch < 128;ch++)
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
