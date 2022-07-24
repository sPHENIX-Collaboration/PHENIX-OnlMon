#include <EMCalRichDecodeOnline.h>
#include <OnlMonServer.h>

#include <msg_profile.h>
#include <phool.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;


EMCalRichDecodeOnline::EMCalRichDecodeOnline(const OnlMon *parent)
{
  mymon = parent;
  for (int i = 0; i < NBIT; i++)
    {
      // bitMask[i] = (int)pow(2, i);
      bitMask[i] = 1 << i; // chp: shift is way faster than pow(2,i)
    }

  //-------------------------
  //  read map of SM to words
  //
  FILE* fp;
  ostringstream parafile;
  if (getenv("ERTMASKCALIB"))
    {
      parafile << getenv("ERTMASKCALIB") << "/";
    }

  parafile << "SM_Word_map.dat";
  if ((fp = fopen(parafile.str().c_str(), "r")) == NULL)
    {
      OnlMonServer * se = OnlMonServer::instance();
      ostringstream msg;
      msg << "EMCalRichDecodeOnline.C: can not find file "
	  << parafile.str().c_str() << " exit now !! **";
      if (mymon)
        {
          se->send_message(mymon, MSG_SOURCE_ERT, MSG_SEV_FATAL, msg.str(), 1);
        }
      else
        {
          cout << msg.str() << endl;
        }
      exit(99);
    }

  //... 4x4A ..
  for (int k = 0; k < 2; k++)
    {
      for (int j = 0; j < 4; j++)
        {
          fscanf(fp, "%d%d", &word4x4ACoord[j][k][0], &word4x4ACoord[j][k][1]);
        }
    }
  //... 4x4B ..
  for (int k = 0; k < 2; k++)
    {
      for (int j = 0; j < 4; j++)
        {
          fscanf(fp, "%d%d", &word4x4BCoord[j][k][0], &word4x4BCoord[j][k][1]);
        }
    }
  //... 4x4C ..
  for (int k = 0; k < 2; k++)
    {
      for (int j = 0; j < 4; j++)
        {
          fscanf(fp, "%d%d", &word4x4CCoord[j][k][0], &word4x4CCoord[j][k][1]);
        }
    }

  //..  2x2 ....
  for (int k = 0; k < 2; k++)
    {
      for (int j = 0; j < 4; j++)
        {
          fscanf(fp, "%d%d", &word2x2Coord[j][k][0], &word2x2Coord[j][k][1]);
        }
    }
  //..  RICH ....
  for (int k = 0; k < 2; k++)
    {
      for (int j = 0; j < 4; j++)
        {
          fscanf(fp, "%d%d", &word4x5CoordRICH[j][k][0], &word4x5CoordRICH[j][k][1]);
        }
    }
  //Initialize arrays to -1
  memset(SMBitPosPBSC, -1, sizeof(SMBitPosPBSC));
  memset(SMBitPosPBGL, -1, sizeof(SMBitPosPBGL));
  memset(SMBitPosRICH, -1, sizeof(SMBitPosRICH));

  for (int i = 0; i < NBIT; i++)
    {
      fscanf(fp, "%d", &SMCoordModeAPBGL[i]);
    }
  for (int i = 0; i < NBIT; i++)
    {
      fscanf(fp, "%d", &SMCoordModeBPBGL[i]);
    }
  for (int i = 0; i < NBIT; i++)
    {
      fscanf(fp, "%d", &SMCoordModeAPBSC[i]);
    }
  for (int i = 0; i < NBIT; i++)
    {
      fscanf(fp, "%d", &SMCoordModeBPBSC[i]);
    }
  for (int i = 0; i < NBIT; i++)
    {
      fscanf(fp, "%d", &SMCoordModeARICH[i]);
    }
  for (int i = 0; i < NBIT; i++)
    {
      fscanf(fp, "%d", &SMCoordModeBRICH[i]);
    }
  for (int i = 0; i < NBIT; i++)
    {
      if (SMCoordModeAPBSC[i] != -1)
        {
          SMBitPosPBSC[SMCoordModeAPBSC[i]] = i;
        }
      SMBitPosPBGL[SMCoordModeAPBGL[i]] = i;
      SMBitPosRICH[SMCoordModeARICH[i]] = i;

      if (SMCoordModeBPBSC[i] != -1)
        {
          SMBitPosPBSC[SMCoordModeBPBSC[i]] = i;
        }
      SMBitPosPBGL[SMCoordModeBPBGL[i]] = i;
      SMBitPosRICH[SMCoordModeBRICH[i]] = i;
    }

  fclose(fp);
  return ;
}

//_____________________
//________________________________
void EMCalRichDecodeOnline::Reset()
{
  memset(Bit4x4AHalfSectEMC, 0, sizeof(Bit4x4AHalfSectEMC));
  memset(Bit4x4BHalfSectEMC, 0, sizeof(Bit4x4BHalfSectEMC));
  memset(Bit4x4CHalfSectEMC, 0, sizeof(Bit4x4CHalfSectEMC));
  memset(Bit2x2HalfSectEMC, 0, sizeof(Bit2x2HalfSectEMC));
  memset(Bit4x5HalfSectRICH, 0, sizeof(Bit4x5HalfSectRICH));

  memset(Bit4x4ASMEMC, 0, sizeof(Bit4x4ASMEMC));
  memset(Bit4x4BSMEMC, 0, sizeof(Bit4x4BSMEMC));
  memset(Bit4x4CSMEMC, 0, sizeof(Bit4x4CSMEMC));
  memset(Bit2x2SMEMC, 0, sizeof(Bit2x2SMEMC));

  memset(Bit4x5SMRICH, 0, sizeof(Bit4x5SMRICH));

  BitPi0Low = 0;
  BitPi0High = 0;
  BitPHI = 0;
  BitSingleElectron = 0;
  BitChargedHadron = 0;

  arm4x4A = -1;
  sector4x4A = -1;
  side4x4A = -1;
  arm4x4B = -1;
  sector4x4B = -1;
  side4x4B = -1;
  arm4x4C = -1;
  sector4x4C = -1;
  side4x4C = -1;
  arm2x2 = -1;
  sector2x2 = -1;
  side2x2 = -1;
  earm = -1;
  esector = -1;
  eside = -1;
  phiarm1 = -1;
  phisector1 = -1;
  phiside1 = -1;
  phiarm2 = -1;
  phisector2 = -1;
  phiside2 = -1;
  memset(PacketData, 0, sizeof(PacketData));
}
//________________________________
void EMCalRichDecodeOnline::SetPacketData(const int packetID, const int roc, const int word, const long value)
{

  int arm = GetArm(packetID);
  if (arm < 0)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "**  unknown packet " << packetID;
      if (mymon)
        {
          se->send_message(mymon, MSG_SOURCE_ERT, MSG_SEV_WARNING, msg.str(), 1);
        }
      else
        {
          cout << msg.str() << endl;
        }

      return ;
    }

  PacketData[arm][roc][word] = value;
}

long
EMCalRichDecodeOnline::GetPacketData(int packetID, int roc, int word)
{

  int arm = GetArm(packetID);
  if (arm < 0)
    {
      OnlMonServer *se = OnlMonServer::instance();
      ostringstream msg;
      msg << "**  unknown packet " << packetID;
      if (mymon)
        {
          se->send_message(mymon, MSG_SOURCE_ERT, MSG_SEV_WARNING, msg.str(), 1);
        }
      else
        {
          cout << msg.str() << endl;
        }

      return 0;
    }

  return PacketData[arm][roc][word];
}

//________________________________
void EMCalRichDecodeOnline::Calculate()
{
  int *emcsmcoord, *richsmcoord;

  for (int arm = 0; arm < NARM; arm++)
    {
      for (int sector = 0; sector < NSECTOR; sector++)
        {
          for (int side = 0; side < NSIDE; side++)
            {

              if (arm == 1)
                {
                  emcsmcoord = side ? SMCoordModeBPBSC : SMCoordModeAPBSC;
                  richsmcoord = side ? SMCoordModeBRICH : SMCoordModeARICH;
                }
              else
                {
                  emcsmcoord = side ? SMCoordModeAPBSC : SMCoordModeBPBSC;
                  richsmcoord = side ? SMCoordModeARICH : SMCoordModeBRICH;
                }

              if (arm == 1 && (sector == 0 || sector == 1))  //__PBGL
                emcsmcoord = side ? SMCoordModeBPBGL : SMCoordModeAPBGL;

              int roc1 = word4x4ACoord[sector][side][0];
              int word1 = word4x4ACoord[sector][side][1];
              int roc2 = word4x4BCoord[sector][side][0];
              int word2 = word4x4BCoord[sector][side][1];
              int roc3 = word4x4CCoord[sector][side][0];
              int word3 = word4x4CCoord[sector][side][1];
              int roc4 = word2x2Coord[sector][side][0];
              int word4 = word2x2Coord[sector][side][1];
              int roc5 = word4x5CoordRICH[sector][side][0];
              int word5 = word4x5CoordRICH[sector][side][1];

              for (int bit = 0; bit < NBIT; bit++)
                {
                  //...  4x4A ...
                  FillRawBit(arm, sector, side, roc1, word1, bit, emcsmcoord, 0);

                  //...  4x4B ...
                  FillRawBit(arm, sector, side, roc2, word2, bit, emcsmcoord, 1);

                  //...  4x4C ...
                  FillRawBit(arm, sector, side, roc3, word3, bit, emcsmcoord, 2);

                  //...  2x2 ...
                  FillRawBit(arm, sector, side, roc4, word4, bit, emcsmcoord, 3);

                  //...  4x5 RICH ...
                  FillRawBit(arm, sector, side, roc5, word5, bit, richsmcoord, 4);
                }
            }

        }
    }
  FillPHITriggerBit();
  FillElectronTriggerBit();
}

void EMCalRichDecodeOnline::CalculateInv()
{
  int *emcsmcoord, *richsmcoord;

  for (int arm = 0; arm < NARM; arm++)
    {
      for (int sector = 0; sector < NSECTOR; sector++)
        {
          for (int side = 0; side < NSIDE; side++)
            {

              if (arm == 1)
                {
                  emcsmcoord = side ? SMCoordModeBPBSC : SMCoordModeAPBSC;
                  richsmcoord = side ? SMCoordModeBRICH : SMCoordModeARICH;
                }
              else
                {
                  emcsmcoord = side ? SMCoordModeAPBSC : SMCoordModeBPBSC;
                  richsmcoord = side ? SMCoordModeARICH : SMCoordModeBRICH;
                }

              if (arm == 1 && (sector == 0 || sector == 1))  //__PBGL
                {
                  emcsmcoord = side ? SMCoordModeBPBGL : SMCoordModeAPBGL;
                }

              int roc1 = word4x4ACoord[sector][side][0];
              int word1 = word4x4ACoord[sector][side][1];
              int roc2 = word4x4BCoord[sector][side][0];
              int word2 = word4x4BCoord[sector][side][1];
              int roc3 = word4x4CCoord[sector][side][0];
              int word3 = word4x4CCoord[sector][side][1];
              int roc4 = word2x2Coord[sector][side][0];
              int word4 = word2x2Coord[sector][side][1];
              int roc5 = word4x5CoordRICH[sector][side][0];
              int word5 = word4x5CoordRICH[sector][side][1];

              for (int bit = 0; bit < NBIT; bit++)
                {
                  //...  4x4A ...
                  FillRawPacket(arm, sector, side, roc1, word1, bit, emcsmcoord, 0);

                  //...  4x4B ...
                  FillRawPacket(arm, sector, side, roc2, word2, bit, emcsmcoord, 1);

                  //...  4x4C ...
                  FillRawPacket(arm, sector, side, roc3, word3, bit, emcsmcoord, 2);

                  //...  2x2 ...
                  FillRawPacket(arm, sector, side, roc4, word4, bit, emcsmcoord, 3);

                  //...  4x5 RICH ...
                  FillRawPacket(arm, sector, side, roc5, word5, bit, richsmcoord, 4);
                }
            }
        }
    }

}
//___________________________________
void EMCalRichDecodeOnline::FillRawBit(int arm, int sector, int side, int roc, int word, int bit, int* smcoord, int tileType)
{
  if ((PacketData[arm][roc][word] & bitMask[bit]) == bitMask[bit])
    {

      int sm = smcoord[bit];
      if (sm == -1)
        {
          return ;  //__ non-existing SM
        }

      if (tileType == 0)
        { //__ 4x4A __

          Bit4x4AHalfSectEMC[arm][sector][side] = 1;
          Bit4x4ASMEMC[arm][sector][sm] = 1;

          BitPi0Low = 1;  //.. fill the trigger bits by the way

          arm4x4A = arm;
          sector4x4A = sector;
          side4x4A = side;

        }
      else if (tileType == 1)
        { //__ 4x4B __

          Bit4x4BHalfSectEMC[arm][sector][side] = 1;
          Bit4x4BSMEMC[arm][sector][sm] = 1;

          BitPi0High = 1;  //.. fill the trigger bits by the way

          arm4x4B = arm;
          sector4x4B = sector;
          side4x4B = side;

        }
      else if (tileType == 2)
        { //__ 4x4C __

          Bit4x4CHalfSectEMC[arm][sector][side] = 1;
          Bit4x4CSMEMC[arm][sector][sm] = 1;

          arm4x4C = arm;
          sector4x4C = sector;
          side4x4C = side;

        }
      else if (tileType == 3)
        { //__ 2x2 __

          Bit2x2HalfSectEMC[arm][sector][side] = 1;
          Bit2x2SMEMC[arm][sector][sm] = 1;

          BitChargedHadron = 1;  //.. fill the trigger bits by the way

          arm2x2 = arm;
          sector2x2 = sector;
          side2x2 = side;

        }
      else if (tileType == 4)
        { //__  RICH 4x5 __

          Bit4x5HalfSectRICH[arm][sector][side] = 1;
          Bit4x5SMRICH[arm][sector][sm] = 1;

        }
    }
}

void EMCalRichDecodeOnline::FillRawPacket(int arm, int sector, int side, int roc, int word, int bit, int* smcoord, int tileType)
{
  int sm = smcoord[bit];
  if (sm == -1)
    {
      return ;  //__ non-existing SM
    }

  if (tileType == 0)
    { //__ 4x4A __
      if (Bit4x4ASMEMC[arm][sector][sm] == 1)
        {
          PacketData[arm][roc][word] = (PacketData[arm][roc][word] | bitMask[bit]);
        }
    }
  else if (tileType == 1)
    { //__ 4x4B __

      if (Bit4x4BSMEMC[arm][sector][sm] == 1)
        {
          PacketData[arm][roc][word] = (PacketData[arm][roc][word] | bitMask[bit]);
        }
    }
  else if (tileType == 2)
    { //__ 4x4C __
      if (Bit4x4CSMEMC[arm][sector][sm] == 1)
        {
          PacketData[arm][roc][word] = (PacketData[arm][roc][word] | bitMask[bit]);
        }
    }
  else if (tileType == 3)
    { //__ 2x2 __
      if (Bit2x2SMEMC[arm][sector][sm] == 1)
        {
          PacketData[arm][roc][word] = (PacketData[arm][roc][word] | bitMask[bit]);
        }
    }
  else if (tileType == 4)
    { //__  RICH 4x5 __
      if (Bit4x5SMRICH[arm][sector][sm] == 1)
        {
          PacketData[arm][roc][word] = (PacketData[arm][roc][word] | bitMask[bit]);
        }
    }
}

//_________________________________________
void EMCalRichDecodeOnline::FillPHITriggerBit()
{
  int ncount = 0;
  for (int arm = 0; arm < NARM; arm++)
    {
      for (int sector = 0; sector < NSECTOR; sector++)
        {
          for (int side = 0; side < NSIDE; side++)
            {
              if (Bit4x5HalfSectRICH[arm][sector][side] == 1)
                {

                  if (!ncount)
                    {
                      ncount++;
                    }
                  if (ncount == 1 && arm != phiarm1 && phiarm1 != -1)
                    {
                      ncount++;
                    }

                  if (ncount == 1)
                    {
                      phiarm1 = arm;
                      phisector1 = sector;
                      phiside1 = side;
                    }
                  if (ncount == 2)
                    {
                      phiarm2 = arm;
                      phisector2 = sector;
                      phiside2 = side;

                      BitPHI = 1;

                      return ;
                    }
                }
            }
        }
    }

  //---------------------------------------------
  // if ncount==1 in this event, the following
  // 3 variable are not zero but you want them 0
  //  since the trigger is not fired.
  //
  phiarm1 = -1;
  phisector1 = -1;
  phiside1 = -1;
}
//___________________________________
void EMCalRichDecodeOnline::FillElectronTriggerBit()
{ // half sector trigger tile

  for (int arm = 0; arm < NARM; arm++)
    {
      for (int sector = 0; sector < NSECTOR; sector++)
        {
          for (int side = 0; side < NSIDE; side++)
            {
              if (Bit2x2HalfSectEMC[arm][sector][side] == 1 &&
                  Bit4x5HalfSectRICH[arm][sector][side] == 1)
                {

                  BitSingleElectron = 1;

                  earm = arm;
                  esector = sector;
                  eside = side;

                  break;
                }
            }
        }
    }

}
//________________________________
bool EMCalRichDecodeOnline::GetRawBitPi0Low(int& arm, int& sector, int& side)
{
  if (arm4x4A != -1)
    {
      arm = arm4x4A;
      sector = sector4x4A;
      side = side4x4A;

      return true;

    }
  return false;
}
//________________________________
bool EMCalRichDecodeOnline::GetRawBitPi0High(int& arm, int& sector, int& side)
{
  if (arm4x4B != -1)
    {
      arm = arm4x4B;
      sector = sector4x4B;
      side = side4x4B;

      return true;

    }
  return false;
}

//________________________________
bool EMCalRichDecodeOnline::GetRawBitPHI(int& arm1, int& sector1, int& side1,
					 int& arm2, int& sector2, int& side2)
{
  if (phiarm1 != -1)
    {
      arm1 = phiarm1;
      sector1 = phisector1;
      side1 = phiside1;

      arm2 = phiarm2;
      sector2 = phisector2;
      side2 = phiside2;

      return true;

    }
  return false;
}
//________________________________
bool EMCalRichDecodeOnline::GetRawBitSingleElectron(int& arm, int& sector, int& side)
{
  if (earm != -1)
    {
      arm = earm;
      sector = esector;
      side = eside;

      return true;
    }
  return false;
}
//________________________________
bool EMCalRichDecodeOnline::GetRawBitChargedHadron(int& arm, int& sector, int& side)
{
  if (arm2x2 != -1)
    {
      arm = arm2x2;
      sector = sector2x2;
      side = side2x2;

      return true;
    }
  return false;
}
//________________________________
int EMCalRichDecodeOnline::GetPacket(int arm)
{
  /*
    if (arm == 1)
    {
    return 14201; //__ east arm
    }
    if (arm == 0)
    {
    return 14200; //__ west arm
    }
    return -1;
  */ 
  // if it comes with an invalid arm here things are totally screwed up anyway
  // and the return code is not even checked. So this is faster
  return 14200 + arm;
}

//________________________________
void EMCalRichDecodeOnline::GetBitPosition(int arm, int sector, int sm, int& bitEMC, int& bitRICH)
{
  int* SMBitPos;

  if (arm == 1 && (sector == 0 || sector == 1))
    {
      SMBitPos = SMBitPosPBGL;
    }
  else
    {
      SMBitPos = SMBitPosPBSC;
    }
  bitEMC = SMBitPos[sm];
  bitRICH = SMBitPosRICH[sm];
  return ;
}

//________________________________
void EMCalRichDecodeOnline::GetRocWord(int arm, int sector, int trgType, int sm, int& roc, int& word)
{
  //--------------------------------
  //  determine the side of the SM
  //
  int sideEMC, sideRICH;

  if (arm == 1)
    { //__ PBSC
      if ((sm / 3) % 2 == 0)
        {
          sideEMC = 0;  //__ south
        }
      else
        {
          sideEMC = 1;  //__ north
        }
    }
  else
    {
      if ((sm / 3) % 2 == 0)
        {
          sideEMC = 1;  //__ north
        }
      else
        {
          sideEMC = 0;  //__ south
        }
    }

  if (arm == 1 && (sector == 0 || sector == 1))
    { //__PBGL

      if ((sm / 4) % 2 == 0)
        {
          sideEMC = 0;  //__ south
        }
      else
        {
          sideEMC = 1;  //__ south
        }
    }

  if (arm == 1)
    { //__ RICH
      if ((sm / 4) % 2 == 0)
        {
          sideRICH = 0;  //__ south
        }
      else
        {
          sideRICH = 1;  //__ north
        }
    }
  else
    {
      if ((sm / 4) % 2 == 0)
        {
          sideRICH = 1;  //__ north
        }
      else
        {
          sideRICH = 0;  //__ south
        }
    }

  switch (trgType)
    {
    case 0:
      { //__ 4x4A
        roc = word4x4ACoord[sector][sideEMC][0];
        word = word4x4ACoord[sector][sideEMC][1];
        break;
      }

    case 1:
      { //__ 4x4B
        roc = word4x4BCoord[sector][sideEMC][0];
        word = word4x4BCoord[sector][sideEMC][1];
        break;
      }

    case 2:
      { //__ 4x4C
        roc = word4x4CCoord[sector][sideEMC][0];
        word = word4x4CCoord[sector][sideEMC][1];
        break;
      }

    case 3:
      { //__ 2x2
        roc = word2x2Coord[sector][sideEMC][0];
        word = word2x2Coord[sector][sideEMC][1];
        break;
      }

    case 4:
      { //__ 4x5 RICH
        roc = word4x5CoordRICH[sector][sideRICH][0];
        word = word4x5CoordRICH[sector][sideRICH][1];
        break;
      }
    default:
      {
        OnlMonServer * se = OnlMonServer::instance();
        ostringstream msg;
        msg << "EMCalRichDecodeOnline.C: Unknown Trigtype " << trgType;
        if (mymon)
          {
            se->send_message(mymon, MSG_SOURCE_ERT, MSG_SEV_ERROR, msg.str(), 1);
          }
        else
          {
            cout << msg.str() << endl;
          }
        break;
      }
    }
  return ;
}
//________________________________
void EMCalRichDecodeOnline::GetRocWordBitPacket(int arm, int sector, int trgType, int sm, int& roc, int& word, int& packet, int& bitEMC, int& bitRICH)
{
  GetBitPosition(arm, sector, sm, bitEMC, bitRICH);
  packet = GetPacket(arm);
  GetRocWord(arm, sector, trgType, sm, roc, word);
  return ;
}

//________________________________
void EMCalRichDecodeOnline::GetRICHtile(int packetID, int roc, int word, long value)
{
  memset(RICHtriggertile, 0, sizeof(RICHtriggertile));

  int RICHarm = GetArm(packetID);
  if (RICHarm < 0)
    {
      OnlMonServer * se = OnlMonServer::instance();
      ostringstream msg;
      msg << "Error packetID for RICH! " << packetID;
      if (mymon)
        {
          se->send_message(mymon, MSG_SOURCE_ERT, MSG_SEV_FATAL, msg.str(), 1);
        }
      else
        {
          cout << msg.str() << endl;
        }
      return ;
    }

  int RICHside;
  if (roc == 17)
    { //--------------- Get word data for RICH South side.
      RICHside = 2 * RICHarm;
    }
  else if (roc == 19)
    { //--------------- Get word data for RICH North side.
      RICHside = 2 * RICHarm + 1;
    }
  else
    {
      //cout << "No ROC number for RICH!" << endl;
      return ;
    }

  int RICHsector = word;
  printf("RICH side = %d,\t RICH sector = %d\n", RICHside, RICHsector);
  for (int triggertile = 0; triggertile < NBIT; triggertile++)
    {
      if ((value >> triggertile) & 0x1)
        {
          RICHtriggertile[RICHside][RICHsector][triggertile] = 1;
          printf("RICH tile = %d\t", triggertile);
        }
    }
  printf("\n");

  return ;
}

int
EMCalRichDecodeOnline::GetArm(const int packetID)
{
  switch (packetID)
    {
    case EASTPACKETID:
      return 1;
    case WESTPACKETID:
      return 0;
    default:
      OnlMonServer * se = OnlMonServer::instance();
      ostringstream msg;
      msg << "unknown packet " << packetID;
      if (mymon)
        {
          se->send_message(mymon, MSG_SOURCE_ERT, MSG_SEV_FATAL, msg.str(), 1);
        }
      else
        {
          cout << msg.str() << endl;
        }
    }
  return -1;
}
