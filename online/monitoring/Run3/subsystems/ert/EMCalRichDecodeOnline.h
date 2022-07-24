//////////////////////////////////////////////////////////
//  EMCal/RICH Trigger bit decoding software.
//  See http://www.phenix.bnl.gov/WWW/p/draft/xiewei/EMCal-RICH-Trigger/emc_rich_decode.html,  for details
//  problems report to: xiewei@rcf.rhic.bnl.gov
//
//  2003/02/22 GetRICHtile() is added by F. Kajihara.
//
//////////////////////////////////////////////////////////

#ifndef __EMCalRichDecodeOnline_h__
#define __EMCalRichDecodeOnline_h__

class OnlMon;

class EMCalRichDecodeOnline
{

 public:

  EMCalRichDecodeOnline(const OnlMon *parent = 0);

  virtual ~EMCalRichDecodeOnline() {}

  //-----------------------------
  // reset to initial value
  //
  void Reset();

  //----------------------------------------
  // read in packet data and calculate bit
  //
  void SetPacketData(const int packetID, const int roc, const int word, const long value);
  void Calculate();  //..  do all BLL calculation  and mapping
  void CalculateInv(); // from SM bit to the word

  long GetPacketData(int packetID, int roc, int word);

  //----------------------------------------
  // get all trigger bit.  1: fired, 0: not fired.
  //
  int GetTriggerBitPi0Low() const {return BitPi0Low;}
  int GetTriggerBitPi0High() const {return BitPi0High;}
  int GetTriggerBitPHI() const {return BitPHI;}
  int GetTriggerBitSingleElectron() const {return BitSingleElectron;}
  int GetTriggerBitChargedHadron() const {return BitChargedHadron;}

  //----------------------------------------------
  //  get SM bit information
  //
  int GetBit4x4ASMEMC(const int arm, const int sector, const int sm) const {return Bit4x4ASMEMC[arm][sector][sm];}
  int GetBit4x4BSMEMC(const int arm, const int sector, const int sm) const {return Bit4x4BSMEMC[arm][sector][sm];}
  int GetBit4x4CSMEMC(const int arm, const int sector, const int sm) const {return Bit4x4CSMEMC[arm][sector][sm];}
  int GetBit2x2SMEMC(const int arm, const int sector, const int sm) const {return Bit2x2SMEMC[arm][sector][sm];}
  int GetBit4x5SMRICH(const int arm, const int sector, const int sm) const {return Bit4x5SMRICH[arm][sector][sm];}


  // set SM bit information
  void SetBit4x4ASMEMC(const int arm, const int sector, const int sm) {Bit4x4ASMEMC[arm][sector][sm] = 1;}
  void SetBit4x4BSMEMC(const int arm, const int sector, const int sm) {Bit4x4BSMEMC[arm][sector][sm] = 1;}
  void SetBit4x4CSMEMC(const int arm, const int sector, const int sm) {Bit4x4CSMEMC[arm][sector][sm] = 1;}
  void SetBit2x2SMEMC(const int arm, const int sector, const int sm) {Bit2x2SMEMC[arm][sector][sm] = 1;}
  void SetBit4x5SMRICH(const int arm, const int sector, const int sm) {Bit4x5SMRICH[arm][sector][sm] = 1;}


  //------------------------------------------------------
  //  Get Raw bits information. half sector trigger tile
  //
  bool GetRawBitPi0Low(int& arm, int& sector, int& side);

  bool GetRawBitPi0High(int& arm, int& sector, int& side);

  bool GetRawBitPHI(int& arm1, int& sector1, int& side1,
		    int& arm2, int& sector2, int& side2);

  bool GetRawBitSingleElectron(int& arm, int& sector, int& side);

  bool GetRawBitChargedHadron(int& arm, int& sector, int& side);

  //------------------------------------------------------
  // input:   arm, sector, sm, triggertype
  // output:  ROC, word, bits, packets
  //

  void GetRocWordBitPacket(int arm, int sector, int trgType, int sm, int& roc, int& word, int& packet, int& bitEMC, int& bitRICH);

  //------------------------------------------------------
  //  Get RICH trigger tile number
  //
  void GetRICHtile(int packetID, int roc, int word, long value);

 protected:
  const OnlMon *mymon;
  //__  Fill tile/trigger bits information ...
  void FillRawBit(int arm, int sector, int side, int roc, int word, int bit, int* smcoord, int tileType);
  void FillPHITriggerBit();
  void FillElectronTriggerBit();
  void FillRawPacket(int arm, int sector, int side, int roc, int word, int bit, int* smcoord, int tileType);
  //--------------------------------------
  // get packet, bit position, roc, word
  //
  int GetPacket(int arm);
  void GetBitPosition(int arm, int sector, int sm, int& bitEMC, int& bitRICH);
  void GetRocWord(int arm, int sector, int trgType, int sm, int& roc, int& word);

  int GetArm(const int packetID);

  static const int NSM = 32;       // number of SM in a sector for RICH/EMCal
  static const int NBIT = 16;         // number of bits in a word
  static const int NARM = 2; 		// number of arms  1: east arm,   0: west arm
  static const int NSECTOR = 4;	// number of EMCal/RICH sector in each arm
  static const int NSIDE = 2;         // North: 1,   south: 0
  static const int NROC = 20; 	//number of ROCs ,  from 0 - 19
  static const int NWORD = 6; 	// number of words in a ROC   from 0 - 5
  static const int EASTPACKETID = 14201; // packet ID of east arm
  static const int WESTPACKETID = 14200; // packet ID of west arm

  int bitMask[NBIT]; //__ 0/1 mask to see if a bit is 0/1
  int word4x4ACoord[NSECTOR][NSIDE][NARM];  // for 4x4A, 0: ROC(0-19),
  //           1: word(0-5)
  int word4x4BCoord[NSECTOR][NSIDE][NARM];  // 4x4B
  int word4x4CCoord[NSECTOR][NSIDE][NARM];  // 4x4C
  int word2x2Coord[NSECTOR][NSIDE][NARM];   // 2x2
  int word4x5CoordRICH[NSECTOR][NSIDE][NARM];   // 2x2

  int SMCoordModeAPBGL[NBIT];  //.. bit position of a SM in a word
  int SMCoordModeBPBGL[NBIT];
  int SMCoordModeAPBSC[NBIT];
  int SMCoordModeBPBSC[NBIT];
  int SMCoordModeARICH[NBIT];
  int SMCoordModeBRICH[NBIT];

  int SMBitPosPBSC[NSM];
  int SMBitPosPBGL[NSM];
  int SMBitPosRICH[NSM];

  //---------------------------------------
  //  all data in the two trigger packets
  //
  long PacketData[NARM][NROC][NWORD];

  //---------------------------------------------
  //  bits mode for Half sector level trigger tile
  //
  int Bit4x4AHalfSectEMC[NARM][NSECTOR][NSIDE];
  int Bit4x4BHalfSectEMC[NARM][NSECTOR][NSIDE];
  int Bit4x4CHalfSectEMC[NARM][NSECTOR][NSIDE];
  int Bit2x2HalfSectEMC[NARM][NSECTOR][NSIDE];
  int Bit4x5HalfSectRICH[NARM][NSECTOR][NSIDE];

  //------------------------------------------------
  //  bits mode for SM level trigger tile
  //
  int Bit4x4ASMEMC[NARM][NSECTOR][NSM];
  int Bit4x4BSMEMC[NARM][NSECTOR][NSM];
  int Bit4x4CSMEMC[NARM][NSECTOR][NSM];
  int Bit2x2SMEMC[NARM][NSECTOR][NSM];
  int Bit4x5SMRICH[NARM][NSECTOR][NSM];

  //-----------------------------------------
  //  Blue Logic Trigger bits
  //
  int BitPi0Low;
  int BitPi0High;
  int BitPHI;
  int BitSingleElectron;
  int BitChargedHadron;

  //-----------------------------------------------------
  //  Raw bit information. If there're multiple input,
  //  this is random one.
  //
  int arm4x4A, sector4x4A, sm4x4A, side4x4A;
  int arm4x4B, sector4x4B, sm4x4B, side4x4B;
  int arm4x4C, sector4x4C, sm4x4C, side4x4C;
  int arm2x2, sector2x2, sm2x2, side2x2;
  int earm, esector, eside;
  int phiarm1, phisector1, phiside1, phiarm2, phisector2, phiside2;

  int RICHtriggertile[4][4][16];


};

#endif /*  __EMCalRichDecodeOnline_h__ */


