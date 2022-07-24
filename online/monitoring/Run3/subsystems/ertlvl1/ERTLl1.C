
#include <ERTLl1.h>

#include <OnlMonServer.h>
#include <ErtLvl1MonDefs.h>

#include <Event.h>
#include <packet.h>

#include <PdbBankManager.hh>
#include <PdbApplication.hh>
#include <PHTimeStamp.h>
#include <PdbBankID.hh>
#include <PdbCalBank.hh>
#include <PdbErtLutEmcTile.hh>
#include <ErtUtils.h>

#include <msg_profile.h>

#include <iostream>
#include <cstdlib>

#include <sstream>

using namespace std;

ERTLl1::ERTLl1(const OnlMon *parent)
{
  mymon = parent;
  int FiberToRocLUT[6][16] = {
    {92, 93, 19, 85, 18, 73, 0, 10, 30, 23, 24, 27, 34, 44, 68, 65},
    {50, 94, 82, 54, 76, 20, 15, 28, 31, 1, 32, 26, 29, 69, 67, 60},
    {56, 91, 80, 90, 78, 75, 16, 6, 2, 22, 36, 25, 12, 62, 70, 43},
    {63, 51, 81, 52, 83, 88, 72, 7, 35, 4, 38, 5, 11, 61, 48, 45},
    {49, 87, 17, 53, 84, 89, 71, 8, 42, 21, 40, 33, 14, 57, 66, 37},
    {59, 86, 77, 55, 79, 95, 74, 9, 47, 3, 46, 41, 13, 58, 64, 39},
  };

  // Generate the reverse LUT

  for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 16; j++)
        {
          RocToFiber[FiberToRocLUT[i][j]].frame = i;
          RocToFiber[FiberToRocLUT[i][j]].bit = j;
        }
    }

  // Set up the fiber bit masks

  getFiberBitMasks();

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int itrig = 0; itrig < 5; itrig++)
        {
          for (int i = 0; i < 128; i++)
            {
              ROCn[iarm][itrig][i][0] = 99;
              ROCn[iarm][itrig][i][1] = 99;
            }
        }
    }

#include "roc_map_E.fpga"
#include "roc_map_W.fpga"

  // Set up the electron and twoelectron LUT's
  memset(ElectronLUT,0,sizeof(ElectronLUT));

#include <lut_rich2e.fpga>

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int rich1 = 0; rich1 < 128; rich1++)
        {
          for (int rich2 = 0; rich2 < 128; rich2++)
            {
              TwoElectronLUT[iarm][rich1][rich2] = TwoElectronLUT_Input[iarm][rich1][rich2];
            }
        }
    }

}

int ERTLl1::getDataFromERTRocPacket(Event *e)
{

  Packet* roc_packet;
  const int packet_id_offset = 14201;
  int packet_id;
  int status = 0;

  // variables for zero-suppressed format
  short iroc, iword;
  short roc_ok[MAX_ROC], beam_ok[MAX_ROC];
  long usr_word[MAX_FEM][USR_WORDS];
  memset(ROCData,0,sizeof(ROCData));

  for ( short iarm = 0; iarm < MAX_ARM; iarm++ )
    {

      // Packet 14200 is WEST ARM
      // Packet 14201 is EAST ARM

      packet_id = packet_id_offset - iarm;
      roc_packet = e->getPacket( packet_id );

      // no MuID packet found
      if ( ! roc_packet )
        {
          ostringstream msg;
          msg << "ERTLl1: WARNING - Missing Packet = " << packet_id << endl;
          OnlMonServer *se = OnlMonServer::instance();
          se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 2);
          status = -1;
        }
      else
        {
          // This is for variable length(zero-suppressed) format

          for ( int iusr_word = 0; iusr_word < USR_WORDS; iusr_word++ )
            {
              usr_word[ iarm ][ iusr_word ] =
                roc_packet->iValue(iusr_word, "USERWORD" );
            }

          // iroc no longer maps directly to slot in the ERT

          for (iroc = 0; iroc < MAX_ROC; iroc++)
            {

              // Translate between iroc and slot
              int sroc = TranslateToROC(iroc);

              if ( sroc < 16 )
                {
                  roc_ok[ iroc ] = short((usr_word[iarm][0] & (1 << sroc)) >> sroc);
                }
              else
                {
                  roc_ok[iroc] = short((usr_word[iarm][1] & (1 << (sroc - 16)))
                                       >> (sroc - 16) );
                }
              if ( sroc < 12 )
                {
                  beam_ok[iroc] = short((usr_word[iarm][1] & (1 << (sroc + 4)))
                                        >> (sroc + 4) );
                }
              else
                {
                  beam_ok[iroc] = short((usr_word[iarm][2] & (1 << (sroc - 12)))
                                        >> (sroc - 12) );
                }

              if ( (roc_ok[iroc] == 0) || (beam_ok[iroc] == 0) )
                {
                  // ROC status is not good
                  ostringstream msg;
                  msg << "ERTLl1: WARNING - ROC " << iroc << " status is bad, arm = " << iarm << endl;
                  OnlMonServer *se = OnlMonServer::instance();
                  se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_ERROR, msg.str(), 3);
                }

	      //Used for masking
              for (iword = 0; iword < WORD_PER_ROC; iword++)
                {

                  // We've got the ROC data the MuID LL1 needs...
                  ROCData[iarm][iroc][iword] = roc_packet->iValue(iword, sroc);
				  // if(ROCData[0][3][1]){ //0=east, 3-1 = fiber-bitword
				  	// ROCData[0][3][1] = (~0x2)&ROCData[0][3][1]; //(~0x2 masks the SECOND bit of the bitword
				  // }
				  // if(ROCData[1][4][2]){ //1=west, 4-2 = fiber-bitword
				  	// ROCData[1][4][2] = (~0x1)&ROCData[1][4][2]; //~0x1 masks the FIRST bit of the bitword
				  // }

                } // for iword = 0...

            } // for roc = 0...

        } // if roc_packet

      delete roc_packet;

    } // for arm ...
    
    //Mapping from http://www.phenix.bnl.gov/WWW/trigger/pp/c-arm/Run3/roc/erteastpacket.png
    // SM_2x2_RICH_coincidence[SM number][south or north] // W0S2 would be number=0, north_or_south=0
    memset(SM_2x2_RICH_coincidence, 0, sizeof(SM_2x2_RICH_coincidence));
	//All of this is for use by the Blue Logic trigger which has difference Logic for the Electron trigger than the LL1 has
	for(int iarm=0; iarm<2; iarm++){
		SM_2x2_RICH_coincidence[iarm][0][0] = ROCData[iarm][0][5]&&ROCData[iarm][6][0];  //2x2 && RICH
		SM_2x2_RICH_coincidence[iarm][0][1] = ROCData[iarm][1][1]&&ROCData[iarm][7][0];
		SM_2x2_RICH_coincidence[iarm][1][0] = ROCData[iarm][1][5]&&ROCData[iarm][6][1];
		SM_2x2_RICH_coincidence[iarm][1][1] = ROCData[iarm][2][5]&&ROCData[iarm][7][1];
		SM_2x2_RICH_coincidence[iarm][2][0] = ROCData[iarm][3][5]&&ROCData[iarm][6][3];
		SM_2x2_RICH_coincidence[iarm][2][1] = ROCData[iarm][4][1]&&ROCData[iarm][7][3];
		SM_2x2_RICH_coincidence[iarm][3][0] = ROCData[iarm][4][5]&&ROCData[iarm][6][4];
		SM_2x2_RICH_coincidence[iarm][3][1] = ROCData[iarm][5][5]&&ROCData[iarm][7][4];
	}

	ElectronBlueLogic[0] = 0;
	ElectronBlueLogic[1] = 0;
	
	for(int iarm=0; iarm<2; iarm++){
		for(int iSMnum=0; iSMnum<4; iSMnum++){
			for(int iNS=0; iNS<2; iNS++){
				ElectronBlueLogic[iarm] = ElectronBlueLogic[iarm]||SM_2x2_RICH_coincidence[iarm][iSMnum][iNS];
			}
		}
	}
	
  return status;
}

void ERTLl1::getFiberBitMasks()
{

  // Set up the fiber bit mask - a zero means the channel is masked OFF

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int iroc = 0; iroc < MAX_ROC; iroc++)
        {
          for (int ibit = 0; ibit < WORD_PER_ROC*BIT_PER_WORD; ibit++)
            {
              fiberBitMask[iarm][iroc][ibit] = 1;
            }
        }
    }

}

int ERTLl1::calculate(short SimSyncErr[2][20])
{

  // Zero the fiber data arrays

  memset(fiberBitData, 0, sizeof(fiberBitData));

  // Map the ROC bits into fiber bits
  // (note that iroc==fiber)
  // Note that the fiber bit number mapping is complicated by the fact
  // that in the FPGA code Harold mapped the fiber inputs to the MuID mapping
  // as <bit><frame>, not <frame><bit> as you would expect.

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      for (int iroc = 0; iroc < MAX_ROC; iroc++)
        {
          int sfib = TranslateToFiber(iroc);
          if (SimSyncErr[iarm][sfib] == 0)
            {
              for (int iword = 0; iword < WORD_PER_ROC; iword++)
                {
                  for (int bit = 0; bit < BIT_PER_WORD; bit++)
                    {
                      if ( (ROCData[iarm][iroc][iword]&(0x1 << bit)) )
                        {
                          int frame = RocToFiber[(iword * BIT_PER_WORD) + bit].frame;
                          int fbit = RocToFiber[(iword * BIT_PER_WORD) + bit].bit;
                          int dbit = frame + fbit * 6;
                          if (fiberBitMask[iarm][iroc][dbit] != 0)
                            fiberBitData[iarm][iroc][dbit] = 1;
                        }
                    }
                }
            }
          else
            {
              // Fibers with sync errors get all bits set high
              //for(int i=0; i<WORD_PER_ROC*BIT_PER_WORD; i++) fiberBitData[iarm][iroc][i] = 1;
              // Updated 4/9/05 JGL - fibers with sync errors now LOW
              for (int i = 0; i < WORD_PER_ROC*BIT_PER_WORD; i++)
                fiberBitData[iarm][iroc][i] = 0;
            }
        }
    }

  // Process Algorithm here...

  // Map ROC bits into algorithm bits

  memset(AlgBitsArray, 0, sizeof(AlgBitsArray));

#include "emc_rich_map_E.fpga"
#include "emc_rich_map_W.fpga"

  // Perform LUT operations for electron and twoelectron triggers

  short ElectronFired[MAX_ARM];
  short TwoElectronFired[MAX_ARM];

  memset(RICH_used, 0, sizeof(RICH_used));
  memset(EMCAL_used, 0, sizeof(EMCAL_used));
  
  for (int arm = 0; arm < MAX_ARM; arm++)
    {

      // Electron LUT

      ElectronFired[arm] = 0;
      
      for (int emcal = 0; emcal < 128; emcal++)
        {
          for (int rich = 0; rich < 128; rich++)
            {
              if ( ElectronLUT[arm][emcal][rich] &&
                   AlgBitsArray[arm][TWOBYTWO][emcal] &&
                   AlgBitsArray[arm][RICH][rich] )
                {
                  ElectronFired[arm] = 1;
		  RICH_used[arm][ROCn[arm][RICH][rich][0]][ROCn[arm][RICH][rich][1]] = true;
		  EMCAL_used[arm][ROCn[arm][TWOBYTWO][emcal][0]][ROCn[arm][TWOBYTWO][emcal][1]] = true;
                }
            }
        }

      // Two Electron LUT

      TwoElectronFired[arm] = 0;
      for (int rich1 = 0; rich1 < 128; rich1++)
        {
          for (int rich2 = 0; rich2 < 128; rich2++)
            {
              if ( TwoElectronLUT[arm][rich1][rich2] &&
                   AlgBitsArray[arm][RICH][rich1] &&
                   AlgBitsArray[arm][RICH][rich2] )
                {
                  TwoElectronFired[arm] = 1;
                }
            }
        }

    }

  // Check algorithm bits for each trigger

  for (int iarm = 0; iarm < MAX_ARM; iarm++)
    {
      FourByFourA[iarm] = BitCount(iarm, FOURBYFOURA, 0, 127);
      FourByFourB[iarm] = BitCount(iarm, FOURBYFOURB, 0, 127);
      FourByFourC[iarm] = BitCount(iarm, FOURBYFOURC, 0, 127);
      TwoByTwo[iarm] = BitCount(iarm, TWOBYTWO, 0, 127);
      Electron[iarm] = ElectronFired[iarm];
      TwoElectron[iarm] = TwoElectronFired[iarm];
    }


  // JGL 4/4/2007 added proceessing of RxNP trigger

  bool RxNP_south_inner = (fiberBitData[EAST][5][53]!=0); 
  bool RxNP_south_outer = (fiberBitData[EAST][5][3]!=0); 
  bool RxNP_north_inner = (fiberBitData[EAST][5][29]!=0); 
  bool RxNP_north_outer = (fiberBitData[EAST][5][35]!=0); 

  RxNP = ((RxNP_south_inner || RxNP_south_outer) && (RxNP_north_inner || RxNP_north_outer)) ? 1:0;  

  return 1;
}

int ERTLl1::BitCount(int arm, int trig, int start, int stop)
{

  int BitCt = 0;

  for (int bit = start; bit <= stop; bit++)
    {
      if (AlgBitsArray[arm][trig][bit] != 0)
        {
          BitCt++;
        }
    }

  return BitCt;

}

void ERTLl1::getROCNumber(int arm, int trig, int rocbit[])
{
  int ROCNumber = 99;
  int BitNumber = 99;

  int btc = BitCount(arm, trig, 0, 127);

  rocbit[0] = 99;
  rocbit[1] = 99;

  if (btc == 1)
    {
      for (int i = 0; i < 128; i++)
        {
          if (AlgBitsArray[arm][trig][i] != 0)
            {
              ROCNumber = ROCn[arm][trig][i][0];
              BitNumber = ROCn[arm][trig][i][1];

	      // check if RICH was used in ERT_Electron
	      if (trig==RICH && !RICH_used[arm][ROCNumber][BitNumber])
		continue;

              if (ROCNumber != -1 && BitNumber != -1)
                {
                  rocbit[0] = ROCNumber < 4 ? ROCNumber : ROCNumber - 12;
                  rocbit[1] = BitNumber;
                }
            }
        }
    }

}


int
ERTLl1::TranslateToROC(const int iroc)
{

  int sroc;

  switch (iroc)
    {
    case 0:
      sroc = 1;
      break;
    case 1:
      sroc = 3;
      break;
    case 2:
      sroc = 6;
      break;
    case 3:
      sroc = 10;
      break;
    case 4:
      sroc = 12;
      break;
    case 5:
      sroc = 15;
      break;
    case 6:
      sroc = 17;
      break;
    case 7:
      sroc = 19;
      break;
    default:
      sroc = -1;
    }

  return sroc;

}

int ERTLl1::TranslateToFiber(const int iroc)
{

  if ( (iroc < 0) || (iroc > 7) )
    {
      return -1;
    }

  if (iroc < 4)
    {
      return iroc;
    }
  return (iroc + 12);
}

void ERTLl1::load_ertelectron_lut(Event *event)
{
  int bank_id = 0;
  Packet* p;
  if ((p = event->getPacket(p_ERTLL1_E)))
      bank_id = p->iValue(0, "E_ALGVER");
  else
    {
      cout << "ERTLL1_E packet not found." << endl;
    }                                                                                                                                                                                
  delete p;

  if (!bank_id)
    { 
      cout << "Can't get ERT_E LUTR version from ERTLL1 packet. Using hardcoded number." << endl;
      bank_id = 1001;
      //      memset(ElectronLUT,true,sizeof(ElectronLUT));
      //      return;
    }

  cout << "bank_id " << bank_id << endl;

  PdbErtLutEmcTile *ertlutemctile;
  
  PdbBankManager *bankManager = PdbBankManager::instance();
  PdbApplication *application = bankManager->getApplication ();
  
  if (application->startRead ())
    {
      PHTimeStamp tSearch = PHTimeStamp (2008,12,31, 0, 0, 0);  // does not matter
      PdbBankID bankID;
      bankID.setInternalValue (bank_id);

      PdbCalBank *ertlutbank = 0;
      ertlutbank =  bankManager->fetchBank ("PdbErtLutEmcTileBank", bankID,"ertlut", tSearch);

      if(ertlutbank)
	{
	  int length = ertlutbank->getLength();
	  for (int i = 0; i < length; i++)
	    {
	      int arm = i/128;
	      arm = (1-arm);
	      int emc_trigtile = i%128;
	      ertlutemctile = (PdbErtLutEmcTile *) & (ertlutbank->getEntry (i));
	      if (!ertlutemctile)
		{
		  ostringstream msg;
		  msg << "ERTLl1: PdbErtLutEmcTile for emc tile " << emc_trigtile << " arm " << arm << " mot found" << endl;
		  OnlMonServer *se = OnlMonServer::instance();
		  se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_WARNING, msg.str(), 2);
		  continue;
		}
	      for (int j=0; j<ertlutemctile->getNumberAssociated(); j++)
		{
		  int rich_trigtile = ertlutemctile->getAssociatedRichTile(j);
		  // we are currently using the LUT for MaxMom > 0.9 GeV/c . CLS
		  if (ertlutemctile->getAssociatedMaxMom(j)<0.9) continue;
		  ElectronLUT[arm][emc_trigtile][rich_trigtile] = true;
		}
	    }
	  delete ertlutbank;

	}
      else
	{
	  ostringstream msg;
	  msg << "ERTLl1:  Failed to read bank, exit!" << endl;
	  OnlMonServer *se = OnlMonServer::instance();
	  se->send_message(mymon, MSG_SOURCE_LVL1, MSG_SEV_FATAL, msg.str(), 2);
	  exit(-1);
	}
    }
}

bool ERTLl1::getROCbit(int iarm, int iroc, int iword)
{
  if (ROCData[iarm][iroc][iword]==0) return false;

  int ROCmap[8][6] = {{FOURBYFOURA, FOURBYFOURB, FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO},
                      {FOURBYFOURC, TWOBYTWO,    FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO},
                      {-1,          -1,          FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO},
                      {FOURBYFOURA, FOURBYFOURB, FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO},
                      {FOURBYFOURB, TWOBYTWO,    FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO},
                      {-1,          -1,          FOURBYFOURA, FOURBYFOURB, FOURBYFOURC, TWOBYTWO},
                      {4,            4,          -1,          4,           4,           -1},
                      {4,            4,          -1,          4,           4,           -1}};

  if (ROCmap[iroc][iword] == FOURBYFOURA && FourByFourA[iarm]==0) return false;
  if (ROCmap[iroc][iword] == FOURBYFOURB && FourByFourB[iarm]==0) return false;
  if (ROCmap[iroc][iword] == FOURBYFOURC && FourByFourC[iarm]==0) return false;
  if (ROCmap[iroc][iword] == TWOBYTWO && TwoByTwo[iarm]==0) return false;
  if (ROCmap[iroc][iword] == 4 && Electron[iarm]==0) return false;

  if (iroc>5) // RICH used in ERT_Electron
    {
      for (int ibit=iword*BIT_PER_WORD; ibit<(iword+1)*BIT_PER_WORD; ibit++)
	if (RICH_used[iarm][TranslateToFiber(iroc)][ibit])
	  {
	    //	    cout << "RICH fired: " << iarm << " " << TranslateToFiber(iroc) << " " << ibit << endl;
	    return true;
	  }
      return false;  // no enabled RICH SM was used in ERT_Electron
    }

  return true;
}
