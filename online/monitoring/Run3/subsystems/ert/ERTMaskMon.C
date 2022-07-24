#include <ERTMaskMonCommon.h>
#include <ERTMaskMon.h>

#include <EMCalRichDecodeOnline.h>
#include <OnlMonServer.h>

#include <Event.h>
#include <packet.h>

#include <phool.h> // EAST/WEST Definition

#include <TH1.h>
#include <TH2.h>
#include <TTree.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace ERTMaskMonCommon;

ERTMaskMon::ERTMaskMon(const char *name) : OnlMon(name)
{
  // set the trigger bits to zero in case someone has
  // again a funny idea to just comment them out
  m_CLOCK   = 0;
  m_BBC     = 0;
  m_BBCNVtx = 0;
  m_BBCNrw = 0;
  m_ZDCw    = 0;
  m_ZDCn    = 0;
  m_ZDCNoS  = 0;
  //m_BBCNVtxZDCNoS = 0;

  m_2x2  = 0;
  m_4x4b = 0;
  m_4x4a = 0;
  m_4x4c = 0;
  m_Ele  = 0;

  m_2x2BBC  = 0;
  m_4x4aBBC = 0;
  m_4x4bBBC = 0;
  m_4x4cBBC = 0;
  m_EleBBC  = 0;
  
  m_UP      = 0;

  RegisterHistos(this);
  SetTrigSel();
  m_DECODE = new EMCalRichDecodeOnline(this);

  // Set to "true" if you want to make a tree output for expert analyses.
  // You may need to modify lines where the tree are made and filled.
  m_bl_tree_output = false;
//  m_bl_tree_output = true;  // KO
  if (m_bl_tree_output) MakeTree();
  return;
}

ERTMaskMon::~ERTMaskMon()
{
   if (m_bl_tree_output) {
      delete m_tree;
   }
   if (m_DECODE) {
      delete m_DECODE;
   }
}

int ERTMaskMon::process_event(Event *evt)
{
  if (evt->getErrorCode())
    { // do not analyse corrupt events
      return 0;
    }

  m_Evtcount->AddBinContent(1);

  m_DECODE->Reset();

  unsigned int tb_live   = 0; // tb = trigger bit
  unsigned int tb_scaled = 0;
  int crossword = 0;
  unsigned int acptctr   = 0;
  Packet *ptrig = evt->getPacket(14001);
  if (ptrig) {
     tb_live   = ptrig->iValue(0, "LIVETRIG");
     tb_scaled = ptrig->iValue(0, "SCALEDTRIG");
     crossword = ptrig->iValue(0, "CROSSCTR");
     acptctr   = ptrig->iValue(0, "ACPTCTR");
     delete ptrig;
     // crossing selection with conf file
     if (m_cross_sel >= 0 && crossword != m_cross_sel) {
        return 0;
     }
  } else if (m_trig_sel == -1) {
     tb_live   = 0xffffffff;
     tb_scaled = 0xffffffff;
  }

  if (m_bl_tree_output) {
     b_evt       = acptctr;
     b_xing      = crossword;
     b_tb_live   = tb_live;
     b_tb_scaled = tb_scaled;
  }

  //
  // trigger selection here
  //
  unsigned int bit_used = 
    m_2x2         | m_4x4a    | m_4x4b    | m_4x4c       | m_Ele         |
    m_2x2BBC      | m_4x4aBBC | m_4x4bBBC | m_4x4cBBC    | m_EleBBC      |
    m_BBC         | m_BBCNVtx | m_BBCNrw  | m_ZDCw       | m_ZDCn        | 
    m_ZDCNoS      | m_UP;
     
  unsigned int trig_accept       = tb_scaled & bit_used;
  unsigned int trig_force_accept = tb_scaled & m_trig_sel;
  if ( (  m_trig_sel && ! trig_force_accept) ||
       (! m_trig_sel && ! trig_accept ) )
    {
      return 0;
    }
  m_Evtcount->AddBinContent(2);

  //
  // get EMC RICH packet to calculate all tile/trigger bits
  //
  for (int iarm = 0; iarm < N_ARM; iarm++)
    {
      Packet *emc_rich_packet = evt->getPacket(PACKET_ID[iarm]);
      if (! emc_rich_packet)
        continue;

      for (int iroc = 0; iroc < 20; iroc++)
        {
          for (int iword = 0; iword < 6; iword++)
            {
              long word = emc_rich_packet->iValue(iword, iroc);
              //  load packet data into decode variable
              m_DECODE->SetPacketData(PACKET_ID[iarm], iroc, iword, word);
            }
        }
      delete emc_rich_packet;
    }
  m_DECODE->Calculate();

  if (m_bl_tree_output) {
     b_n_4x4a = 0;
     b_n_4x4b = 0;
     b_n_4x4c = 0;
     b_n_2x2  = 0;
  }

  //--------------------------------------------
  // Then you can do your monitering work.
  //--------------------------------------------
  for (int arm = 0; arm < N_ARM; arm++)
    {
      for (int sector = 0; sector < N_SECTOR; sector++)
        {
          // EMCal part
          int n_sm = IsPbGl(arm, sector) ? N_SM_PBGL : N_SM_PBSC;
          for (int sm = 0; sm < n_sm; sm++)
            {
              int ssm = ArmSectSmToSerialSm(DET_EMC, arm, sector, sm);
              int bit_4x4a = m_DECODE->GetBit4x4ASMEMC(arm, sector, sm);
              int bit_4x4b = m_DECODE->GetBit4x4BSMEMC(arm, sector, sm);
              int bit_4x4c = m_DECODE->GetBit4x4CSMEMC(arm, sector, sm);
              int bit_2x2  = m_DECODE->GetBit2x2SMEMC (arm, sector, sm);
              m_hsm[0]->Fill(ssm, bit_4x4a);
              m_hsm[1]->Fill(ssm, bit_4x4b);
              m_hsm[2]->Fill(ssm, bit_4x4c);
              m_hsm[3]->Fill(ssm, bit_2x2);

              if (m_bl_tree_output) {
                 int as = arm * 4 + sector;
                 if (bit_4x4a) {
                    b_as_4x4a[b_n_4x4a] = as;
                    b_sm_4x4a[b_n_4x4a] = sm;
                    b_ssm_4x4a[b_n_4x4a++] = ssm;
                 }
                 if (bit_4x4b) {
                    b_as_4x4b[b_n_4x4b] = as;
                    b_sm_4x4b[b_n_4x4b] = sm;
                    b_ssm_4x4b[b_n_4x4b++] = ssm;
                 }
                 if (bit_4x4c) {
                    b_as_4x4c[b_n_4x4c] = as;
                    b_sm_4x4c[b_n_4x4c] = sm;
                    b_ssm_4x4c[b_n_4x4c++] = ssm;
                 }
                 if (bit_2x2) {
                    b_as_2x2 [b_n_2x2] = as;
                    b_sm_2x2 [b_n_2x2] = sm;
                    b_ssm_2x2 [b_n_2x2++] = ssm;
                 }
              }
            }

          // RICH part
          for ( int sm = 0;sm < N_SM_RICH ;sm++)
            {
              int ssm = ArmSectSmToSerialSm(DET_RICH, arm, sector, sm);
              m_hsm[4]->Fill(ssm, m_DECODE->GetBit4x5SMRICH(arm, sector, sm));
            }
        }
    }

  if (m_bl_tree_output) m_tree->Fill();
  return 0;
}

void ERTMaskMon::SetTrigSel(void)
{
  m_trig_sel = 0;
  m_cross_sel = -1;

  ostringstream parafile;
  if (getenv("ERTMASKCALIB"))
    {
      parafile << getenv("ERTMASKCALIB") << "/";
    }
  parafile << "ERT_expert.txt";

  ifstream infile(parafile.str().c_str(), ifstream::in);
  if (infile)
    {
      while ( !infile.eof() )
        {
          string one_line;	// a complete line in the config file
          getline(infile, one_line);
          // skip lines that begin with '/' or '#'
          if ( one_line[0] == '/' || one_line[0] == '#' )
            {
              continue;
            }

          // make one_line an istringstream
          istringstream line( one_line.c_str() );
          string label;
          line >> label;
          // transform string to upper characters only (the cast at the
          // end is sadly neccessary for gcc 3.2)
          transform(label.begin(), label.end(), label.begin(), (int(*)(int))toupper);
          if (label == "TRG")
            {
              line >> hex >> m_trig_sel;
            }
          else if (label == "CROSS")
            {
              line >> m_cross_sel;
            }
        }
      infile.close();
    }

  printf("Trig_sel  = %x\nCross_sel = %d\n", m_trig_sel, m_cross_sel);
}

void ERTMaskMon::MakeTree()
{
   m_tree = new TTree("ert_tree", "Tree for Expert Analysis");
   m_tree->Branch("evt",  &b_evt,  "evt/i");
   m_tree->Branch("xing", &b_xing, "xing/I");
   m_tree->Branch("tb_live",  &b_tb_live,   "tb_live/i");
   m_tree->Branch("tb_scald", &b_tb_scaled, "tb_scaled/i");
   m_tree->Branch("n_4x4a", &b_n_4x4a, "n_4x4a/I");
   m_tree->Branch("n_4x4b", &b_n_4x4b, "n_4x4b/I");
   m_tree->Branch("n_4x4c", &b_n_4x4c, "n_4x4c/I");
   m_tree->Branch("n_2x2 ", &b_n_2x2,  "n_2x2/I");
   m_tree->Branch("as_4x4a", b_as_4x4a, "as_4x4a[n_4x4a]/I");
   m_tree->Branch("as_4x4b", b_as_4x4b, "as_4x4b[n_4x4b]/I");
   m_tree->Branch("as_4x4c", b_as_4x4c, "as_4x4c[n_4x4c]/I");
   m_tree->Branch("as_2x2",  b_as_2x2,  "as_2x2[n_2x2]/I");
   m_tree->Branch("sm_4x4a", b_sm_4x4a, "sm_4x4a[n_4x4a]/I");
   m_tree->Branch("sm_4x4b", b_sm_4x4b, "sm_4x4b[n_4x4b]/I");
   m_tree->Branch("sm_4x4c", b_sm_4x4c, "sm_4x4c[n_4x4c]/I");
   m_tree->Branch("sm_2x2",  b_sm_2x2,  "sm_2x2[n_2x2]/I");
   m_tree->Branch("ssm_4x4a", b_ssm_4x4a, "ssm_4x4a[n_4x4a]/I");
   m_tree->Branch("ssm_4x4b", b_ssm_4x4b, "ssm_4x4b[n_4x4b]/I");
   m_tree->Branch("ssm_4x4c", b_ssm_4x4c, "ssm_4x4c[n_4x4c]/I");
   m_tree->Branch("ssm_2x2",  b_ssm_2x2,  "ssm_2x2[n_2x2]/I");
}

int ERTMaskMon::BeginRun(const int runno)
{
  OnlMonServer *se = OnlMonServer::instance();

  m_CLOCK   = se->getLevel1Bit("Clock");
  m_BBC     = se->getLevel1Bit(TRIG_NAME_MB);//BBCLL1(>1 tubes)
  m_BBCNVtx = se->getLevel1Bit(TRIG_NAME_NV);//BBCLL1(>1 tubes) novertex
  m_BBCNrw  = se->getLevel1Bit("BBCLL1(>1 tubes) narrowvtx");
  m_ZDCw    = se->getLevel1Bit(TRIG_NAME_ZD);//ZDCLL1wide
  m_ZDCn    = se->getLevel1Bit("ZDCLL1narrow");
  m_ZDCNoS  = se->getLevel1Bit("ZDCNS");
  //m_BBCNVtxZDCNoS = se->getLevel1Bit(TRIG_NAME_LP);//BBCLL1(noVtx)&(ZDCN||ZDCS)

  m_2x2  = se->getLevel1Bit("ERTLL1_2x2");
  m_4x4b = se->getLevel1Bit("ERT_4x4b"); 

  m_4x4aBBC = se->getLevel1Bit("ERT_4x4a&BBCLL1");
  m_4x4bBBC = se->getLevel1Bit("ERT_4x4b&BBCLL1");
  m_4x4cBBC = se->getLevel1Bit("ERT_4x4c&BBCLL1");
  m_EleBBC  = se->getLevel1Bit("ERTLL1_Electron&BBCLL1");
  
  //m_UP      = se->getLevel1Bit("UltraPeriph");

  m_Evtcount->SetBinContent(3, m_trig_sel);
  m_Evtcount->SetBinContent(4, m_cross_sel);

  return 0;
}
