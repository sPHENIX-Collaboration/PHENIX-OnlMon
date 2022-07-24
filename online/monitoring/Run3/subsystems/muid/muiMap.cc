#include "muiMap.h"
#include "mui_mon_const.h"

#include <OnlMonServer.h>

#include "msg_profile.h"

#include "PHTimeStamp.h"
#include "PdbBankManager.hh"
#include "PdbApplication.hh"
#include "PHTimeStamp.h"
#include "PdbBankID.hh"
#include "PdbCalBank.hh"
#include "PdbMuiChannelMap.hh"
#include "TMuiChannelId.hh"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

const int BUFFER_LENGTH = 256;

muiMap::muiMap(const OnlMon *parent)
{
  mymon = parent;
  memset(channel_map, -1, sizeof(channel_map));
  Fem = -1;
  Roc = -1;
  Word = -1;
  Bit = -1;
  Arm = -1;
  Panel = -1;
  Gap = -1;
  Orient = -1;
  TwoPack = -1;
  muiChannel = new TMuiChannelId();
  return ;
}

muiMap::~muiMap()
{
  if (muiChannel)
    {
      delete muiChannel;
    }
  return ;
}

int
muiMap::fillFromDB()
{
  PdbBankManager *bankManager = PdbBankManager::instance();
  PdbApplication *application = bankManager->getApplication();

  TMuiChannelId mui_chan;

  printf("Now opening FD in readonly mode\n");

  short arm, panel, gap, orient, twopack;

  for ( int i = 0; i < MAX_CHANNEL; i++ )
    {
      channel_map[i] = -1;
    }

  if (application->startRead())
    {
      PHTimeStamp tSearch = PHTimeStamp(2003, 4, 6, 0, 0, 0);
      //PHTimeStamp tSearch = PHTimeStamp(2001, 6, 26, 0, 0, 0);
      //PHTimeStamp tSearch = PHTimeStamp(2000,4,2,0,0,0);
      PdbBankID bankID(12100);
      const char *calibname = "map.mui.mui_address";
      PdbCalBank *muiBank = bankManager->fetchBank("PdbMuiChannelMapBank", bankID, calibname, tSearch);

      PdbMuiChannelMap* calchan;

      short first, last, twopackhi, twopacklo;
      if ( muiBank )
        {
          muiBank->print();
          if ( (int) muiBank->getLength() < MAX_WORD )
            {
              ostringstream msg;
              msg.str("");
              msg << PHWHERE << " PdbMuiChannelMapBank doesn't have enough entries " << endl;
              OnlMonServer *se = OnlMonServer::instance();

              se->send_message(mymon, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 7);
              return -1;
            }
          else
            {
              for (int word = 0; word < MAX_WORD; word++)
                {
                  calchan = (PdbMuiChannelMap*) & muiBank->getEntry( word );
                  calchan->TwoPackRange(twopacklo, twopackhi);
                  calchan->ChannelRange(first, last);

                  if (( first >= 0 ) && (twopacklo >= 0))
                    {
                      for ( short chan = first; chan <= last; chan++ )
                        {
                          calchan->SoftwareIndex(chan, arm, gap, panel, orient, twopack);
                          mui_chan.Set( arm, gap, panel, (EOrient_t)orient, twopack ) ;
                          channel_map[ word * BIT_PER_WORD + chan ]
			    = (int)TwoPackHash( mui_chan );
                        }
                    }
                }
            }
          delete muiBank;
        }
      else
        {
          ostringstream msg;
          msg.str("");
          msg << "main()" << endl
	      << "\tError:" << endl
	      << "\tbankManager returned zero-pointer" << endl;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 7);
          return -1;
        }
      application->commit();
    }
  else
    {
      application->abort();
    }
  return 0;
}

int
muiMap::fillFromFile()
{
  TMuiChannelId mui_chan;
  PdbMuiChannelMap* calchan;

  char linebuf[BUFFER_LENGTH];
  short ROC, Word, Rword, ChanLo, ChanHi;
  short Arm, Orient, Gap, Panel, TwoPackLo, TwoPackHi, LocLo, LocHi;
  unsigned long moduleID;
  int word;
  char fname[BUFFER_LENGTH];

  ostringstream msg;

  if (getenv("ONLMON_CALIB"))
    {
      sprintf(fname, "%s/mui-fem-config.dat", getenv("ONLMON_CALIB"));
    }
  else
    {
      msg.str("");
      msg << PHWHERE << "ONLMON_CALIB directory does not exhist." << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 7);
      return -1;
    }

  ifstream instr1(fname);

  if (!instr1)
    {
      msg.str("");
      msg << PHWHERE << "  error opening FEM config data file" << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 7);
      return -1;
    }

  for ( int i = 0; i < MAX_CHANNEL; i++ )
    { //Initialize the map
      channel_map[i] = -1;
    }

  // Read the data line by line until we reach EOF.
  word = 0;
  while (instr1.getline(linebuf, BUFFER_LENGTH, '\n'))
    {

      if (instr1.gcount() > BUFFER_LENGTH)
        {
          msg.str("");
          msg << PHWHERE << "input buffer too small!  gcount = " << instr1.gcount()
	      << endl;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_MUID, MSG_SEV_ERROR, msg.str(), 7);
          return -1;
        }

      istringstream stringbuf(linebuf, istringstream::in);

      if (stringbuf >> Arm >> Orient >> ROC >> Word >> Rword
          >> ChanLo >> ChanHi
          >> Gap >> Panel >> TwoPackLo >> TwoPackHi >> LocLo >> LocHi)
        {

          if (Arm == kNORTH)
            {
              if (Orient == kHORIZ)
                {
                  moduleID = 0x1000;
                }
              else
                {
                  moduleID = 0x1010;
                }
            }
          else
            {
              if (Orient == kHORIZ)
                {
                  moduleID = 0x0000;
                }
              else
                {
                  moduleID = 0x0010;
                }

            }

          //We don't really need calchan. We could just fill channel_map directly,
          //but I want it to resemble fill_map.

          calchan = new PdbMuiChannelMap();
          calchan->SetSoftwareIndex(Arm, Gap, Panel, Orient);
          calchan->SetHardwareIndex(moduleID, ROC, Rword, Word);
          calchan->SetChannelRange(ChanLo, ChanHi);
          calchan->SetTwoPackRange(TwoPackLo, TwoPackHi);
          calchan->SetLocationRange(LocLo, LocHi);

          //Basically same as fill_map
          short arm, panel, gap, orient, twopack;
          short first, last, twopackhi, twopacklo;

          calchan->TwoPackRange(twopacklo, twopackhi);
          calchan->ChannelRange(first, last);
          //Only read south arm because that is how fill_map does it.
          if (( first >= 0 ) && (twopacklo >= 0) && (word < MAX_WORD))
            {
              for ( short chan = first; chan <= last; chan++ )
                {
                  calchan->SoftwareIndex(chan, arm, gap, panel, orient, twopack);
                  //Here I use SetID instead of Set because it doesn't require a cast.
                  mui_chan.SetID( arm, gap, panel, orient, twopack ) ;
                  //mui_chan.Show();  //This is good for debug
                  channel_map[word * BIT_PER_WORD + chan]
		    = (int)TwoPackHash( mui_chan );
                }
            }
          word++;
        }
      else
        {
          // Skip any header or comment lines.
        }
    }
  instr1.close(); //Close the dat file
  return 0;
}

void
muiMap::setHardwareAddress(const short fem, const short roc, const short word, const short bit)
{
  Fem = fem;
  Roc = roc;
  Word = word;
  Bit = bit;
  int fee_chan = ((Fem * MAX_ROC + Roc ) * WORD_PER_ROC + Word ) * BIT_PER_WORD + Bit;
  if ( fee_chan >= 0 && fee_chan < MAX_CHANNEL )
    {
      muiChannel->Set(channel_map[ fee_chan ]);
      Arm = muiChannel->Arm();
      Gap = muiChannel->Plane();
      Orient = muiChannel->Orient();
      Panel = muiChannel->Panel();
      TwoPack = muiChannel->TwoPack();
    }
  else
    {
      ostringstream msg;
      msg << PHWHERE << " FEE channel out of range " << fee_chan << endl;
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(mymon, MSG_SOURCE_MUID, MSG_SEV_WARNING, msg.str(), 7);
    }
  return ;
}

