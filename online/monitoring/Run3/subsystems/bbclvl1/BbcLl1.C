#include "BbcLl1.h"
#include "BbcLvl1MsgTypes.h"
#include "OnlMonServer.h"

#include "msg_profile.h"

#include "phool.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

BbcLl1::BbcLl1(const char* CONFIG_path, const OnlMon *parentmon)
{
  mymon = parentmon;
  T2N = T2;
  T2S = T2 + 64;
  LL1T2N = LL1T2;
  LL1T2S = LL1T2 + 64;

  bbcFEMLUT = new BbcLUT(mymon);
  success = bbcFEMLUT->Success();
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "Can't open BbcLut LUT file ";
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  char CONFIG_path_current[BUFSIZ];

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/bbc1_masks_north.msk");
  strcat(CONFIG_path_current, filename);
  success = getNorthMask(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open South Mask file (using getNorthMask)" << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/bbc1_masks_south.msk");
  strcat(CONFIG_path_current, filename);
  success = getSouthMask(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open North Mask file (using getSouthMask)" << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/bbc1_ranges_north.rng");
  strcat(CONFIG_path_current, filename);
  success = getNorthRange(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open South Range file (using getNorthRange)" << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/bbc1_ranges_south.rng");
  strcat(CONFIG_path_current, filename);
  success = getSouthRange(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open North Range file (using getSouthRange)" << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/multlimits.lim");
  strcat(CONFIG_path_current, filename);
  success = getMultiplicityLimits(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open Multiplicity Limits file " << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/vertex.vtx");
  strcat(CONFIG_path_current, filename);
  success = getVertexLimits(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open Vertex Limits file " << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

  strcpy(CONFIG_path_current, CONFIG_path);
  strcpy(filename, "/BBCLL1.lut");
  strcat(CONFIG_path_current, filename);
  success = getBbcLut(CONFIG_path_current);
  if (!success)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "BbcLl1: Can't open Bbc LL1 LUT file " << CONFIG_path_current;
      se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
      return ;
    }

}

BbcLl1::~BbcLl1()
{
  delete bbcFEMLUT;
}

int BbcLl1::getDataFromBbcPacket(Event *event)
{

  static const int Bbcid[3] =
    {
      1001, 1003, 1002
    };
  int nPmt = NumberOfPmt;

  Packet* p;

  if ((p = event->getPacket(Bbcid[0])) != 0)
    {
      for (int ipmt = 0; ipmt < nPmt; ipmt++)
        {
          T2[ipmt] = p->iValue(ipmt, "T2");
        }
    }
  else if ((p = event->getPacket(Bbcid[1])) != 0)
    {
      for (int ipmt = 64; ipmt < nPmt; ipmt++)
        {
          T2[ipmt] = p->iValue(ipmt - 64, "T2");
        }
      delete p;
      if ((p = event->getPacket(Bbcid[2])) != 0)
        {
          for (int ipmt = 0; ipmt < 64; ipmt++)
            {
              T2[ipmt] = p->iValue(ipmt, "T2");
            }
        }
      else
        {
          OnlMonServer *se = OnlMonServer::instance();
          ostringstream msg;
          msg << "BbcLl1: Only one of two bbc packets found";
          se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), FILEOPEN);
          return 0;
        }
    }
  else
    {
      return 0;
    }
  delete p;

  return 1;
}

int BbcLl1::getDataFromLL1Packet(Event *event)
{

  static const int LL1id = 14002;

  Packet* p;

  if ((p = event->getPacket(LL1id)) != 0)
    {

      VertexOut = p->iValue(0, "VERTEX");
      //TRK -- compensate for stuck bit in new BBCLL1 board (3/26/2010)
      if(VertexOut>=128){
	VertexOut-=128;
      }
      AverageOut = p->iValue(0, "TIME");
      NHitsOut = p->iValue(0, "NHITS");
      SHitsOut = p->iValue(0, "SHITS");
      HitsInOut = p->iValue(0, "HITSIN");
      BBLL1RBOut = p->iValue(0, "GL1RB");
      VertexOKOut = p->iValue(0, "VTXOK");

    }
  else
    {
      return 0;
    }
  delete p;

  return 1;
}

int BbcLl1::getDataFromGL1Packet(Event *event)
{

  static const int GL1id = 14001;

  Packet* p;

  if ((p = event->getPacket(GL1id)) != 0)
    {

      BeamCntr0 = p->iValue(0, "BEAMCTR0");
      BeamCntr1 = p->iValue(0, "BEAMCTR1");
    }
  else
    {
      return 0;
    }
  delete p;

  return 1;
}

int BbcLl1::getNorthMask(char *filename)
{

  int line;
  int NLine[64];

  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getNorthMask - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      for (int i = 0; i < 64; i++)
        {
          fin >> NLine[i];
          if (fin.eof())
            break;
        }
      for (int chan = 0; chan < 64; chan++)
        {
          line = (7 - chan / 8) * 8 + (1 - (chan % 8) / 4) * 4 + (chan % 4);
          maskN[chan] = NLine[line];
        }
    }

  return 1;
}

int BbcLl1::getSouthMask(char *filename)
{

  int line;
  int SLine[64];

  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getSouthMask - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      for (int i = 0; i < 64; i++)
        {
          fin >> SLine[i];
          if (fin.eof())
            {
              break;
            }
        }
      for (int chan = 0; chan < 64; chan++)
        {
          line = (7 - chan / 8) * 8 + (1 - (chan % 8) / 4) * 4 + (chan % 4);
          maskS[chan] = SLine[line];
        }
    }

  return 1;
}

int BbcLl1::getNorthRange(char *filename)
{

  int temp;
  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getNorthRange - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      for (int iBoard = 0; iBoard < 8; iBoard++)
        {
          // "0"=Upper Limit, "1"=Lower Limit
          for (int iLimit = 0; iLimit < 2; iLimit++)
            {
              fin >> hex >> temp;
              rangeN[iLimit][iBoard] = temp;
              if (fin.eof())
                {
                  break;
                }
            }
        }
    }

  return 1;
}

int BbcLl1::getSouthRange(char *filename)
{

  int temp;
  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getSouthRange - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      for (int iBoard = 0; iBoard < 8; iBoard++)
        {
          // "0"=Upper Limit, "1"=Lower Limit
          for (int iLimit = 0; iLimit < 2; iLimit++)
            {
              fin >> hex >> temp;
              rangeS[iLimit][iBoard] = temp;
              if (fin.eof())
                {
                  break;
                }
            }
        }
    }

  return 1;
}

int BbcLl1::getMultiplicityLimits(char *filename)
{

  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getSouthRange - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      // Order: [S][Low], [S][High], [N][Low], [N][High]
      for (int i = 0; i < 2; i++)
        {
          for (int j = 0; j < 2; j++)
            {
              fin >> MultLimits[i][j];
              if (fin.eof())
                {
                  break;
                }
            }
        }
    }

  return 1;
}

int BbcLl1::getVertexLimits(char *filename)
{

  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getSouthRange - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      fin >> VertexLow;
      fin >> VertexHigh;
      fin >> VertexLow2;
      fin >> VertexHigh2; 
    }

  return 1;
}

int BbcLl1::getBbcLut(char *filename)
{

  // Open the file
  ifstream fin(filename);
  if (!fin)
    {
      cout << "BbcLl1::getBbcLut - Cannot open file!" << endl;
      return 0;
    }
  else
    {
      for (int i = 0; i < 32; i++)
        {
          fin >> BbcLut[i];
          if (fin.eof())
            {
              break;
            }
        }
    }

  return 1;
}

int BbcLl1::calculate()
{

  int counterS = 0;
  int counterN = 0;
  int sumS = 0;
  int sumN = 0;
  int iBoard;

  for (int iPmt = 0; iPmt < 128; iPmt++)
    chann_stat[iPmt] = 0;

  for (int iPmt = 0; iPmt < 64; iPmt++)
    {
      iBoard = iPmt / 8;
      if (maskS[iPmt] == 0 && LL1T2S[iPmt] >= rangeS[1][iBoard] && LL1T2S[iPmt] <= rangeS[0][iBoard])
        {
          chann_stat[iPmt + 64] = 1;
          sumS = sumS + LL1T2S[iPmt];
          counterS++;
        }
      if (maskN[iPmt] == 0 && LL1T2N[iPmt] >= rangeN[1][iBoard] && LL1T2N[iPmt] <= rangeN[0][iBoard])
        {
          chann_stat[iPmt] = 1;
          sumN = sumN + LL1T2N[iPmt];
          counterN++;
        }
    }
  nHitPmt[0] = counterS;
  if (counterS != 0)
    {
      ArmHitTime[0] = sumS / counterS;
    }
  else
    {
      ArmHitTime[0] = 255;
    }
  nHitPmt[1] = counterN;
  if (counterN != 0)
    {
      ArmHitTime[1] = sumN / counterN;
    }
  else
    {
      ArmHitTime[1] = 255;
    }

  if (nHitPmt[0] >= MultLimits[0][0] && nHitPmt[0] <= MultLimits[0][1])
    {
      SouthMultOK = 1;
    }
  else
    {
      SouthMultOK = 0;
    }
  if (nHitPmt[1] >= MultLimits[1][0] && nHitPmt[1] <= MultLimits[1][1])
    {
      NorthMultOK = 1;
    }
  else
    {
      NorthMultOK = 0;
    }

  TimeAverage = (ArmHitTime[0] + ArmHitTime[1]) / 2;

  if ((SouthMultOK + NorthMultOK) < 2)
    ZVertex=-999;
  else
    ZVertex = (ArmHitTime[1] - ArmHitTime[0]);

  RB11 = ( (nHitPmt[0] >> 1) & 0x3F ) | ( ( (nHitPmt[1] >> 2) & 0x1F ) << 6 );

  if (ZVertex >= VertexLow && ZVertex <= VertexHigh &&
      SouthMultOK == 1 && NorthMultOK == 1)
    {
      VertexOK = 1;
    }
  else
    {
      VertexOK = 0;
    }

  if (ZVertex >= VertexLow2 && ZVertex <= VertexHigh2 &&
      SouthMultOK == 1 && NorthMultOK == 1)
    {
      Vertex2OK = 1;
    }
  else
    {
      Vertex2OK = 0;
    }

  int index = NorthMultOK + SouthMultOK * 2 + VertexOK * 4 + Vertex2OK * 16; // FEM Unreliable not included
  BBRB2 = BbcLut[index];

  return 1;
}
