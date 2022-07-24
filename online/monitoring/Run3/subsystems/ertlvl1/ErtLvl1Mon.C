#include <ErtLvl1Mon.h>
#include <ERTLl1.h>
#include <OnlMonServer.h>
#include <OnlMonDB.h>
#include <Event.h>
#include <msg_profile.h>
#include <TH1.h>
#include <TH2.h>
#include <cmath>
#include <iostream>
#include <sstream>

#define PPG_Pedestal 0x10000000
#define PPG_TestPulse 0x20000000
#define PPG_Laser 0x40000000

#define BBCLL1 0x00000000
#define ERTLL1_E_BBCLL1 0x00000000
#define ERTLL1_2x2 0x00000000

#define ERT_4x4a_GD 0x00020000
#define ERT_4x4b_GD 0x00040000
#define ERT_4x4c_GD 0x00000001

#define NULL_TRIG 0x00000000

enum {index_4x4a, index_4x4b, index_4x4c, index_2x2, index_Electron, index_TwoElectron};
enum Arms_enum { EAST, WEST };
enum Counter_enum { RealAndNoSimCounter_index, RealAndSimCounter_index, SimAndNoRealCounter_index, CounterSum_index };

using namespace std;

ErtLvl1Mon::ErtLvl1Mon(): OnlMon("ERTLVL1MON")
{
  // Create a simulator
  simERTLl1 = new ERTLl1(this);

  // Set up database
  dbvars = new OnlMonDB(ThisName); // use monitor name for db table name
  DBVarInit();

  // Reset internal counters
  Reset();

  CreateHistograms();

  RegisterHistograms();

}

ErtLvl1Mon::~ErtLvl1Mon()
{
  delete simERTLl1;
  delete dbvars;
}

int ErtLvl1Mon::BeginRun(const int runno)
{

  Reset();
  // Clear the emulator roc-word counter histograms
  for (int iarm = 0; iarm < 2; iarm++)
    {
      if (hRocWord[iarm]) hRocWord[iarm]->Reset();
      if (hRocWordERTLL1[iarm]) hRocWordERTLL1[iarm]->Reset();
    }
  return 0;

}

int ErtLvl1Mon::process_event(Event *event)
{

  rawevtcnt++;

  // Only data events
  if (event->getEvtType() != 1) return 0;
  if (!checkEventTrigger(event)) return 0;
  if (evtcnt == 0) simERTLl1->load_ertelectron_lut(event);

  evtcnt++;

  if (simERTLl1->getDataFromERTRocPacket(event) != 0)
    {
      cout << "failed getting sim data" << endl;
      return -1;
    }
  bool printevent = false;

  bool ERTLL1fired[2][6];
  Packet *p = event->getPacket(14001);
  if (!p)
    {
      return -1;
    }
  int rawtrigword = p->iValue(0, "RAWTRIG");
  int gran_dis_bits = p->iValue(0, "GDISABLE");

  // please get all your packet values before this. The following code is
  // long and deleting the packet pointer at the end will just invite
  // memory leaks (anyone returning anywhere will leave the packet
  // allocated)
  delete p;
  p = NULL; // just so code which uses it down the line segfaults

  for (int arm_i = 0; arm_i < 2; arm_i++)
    {
      ERTLL1fired[arm_i][index_4x4a] = (gran_dis_bits & ERT_4x4a_GD);
      ERTLL1fired[arm_i][index_4x4b] = (gran_dis_bits & ERT_4x4b_GD);
      ERTLL1fired[arm_i][index_4x4c] = (gran_dis_bits & ERT_4x4c_GD);

      ERTLL1fired[arm_i][index_2x2] =  (rawtrigword & ERTLL1_2x2);

      ERTLL1fired[arm_i][index_Electron] = (rawtrigword & ERTLL1_E_BBCLL1);
      ERTLL1fired[arm_i][index_TwoElectron] = (rawtrigword & NULL_TRIG);
    }
  int BBCLL1_fired = (rawtrigword & BBCLL1);

  bool ertfired = false;

  bool simERTLL1fired[2][6]; //[arm][trig]
  memset(simERTLL1fired, false, sizeof(simERTLL1fired));

  int trigger_dependency[6];
  for (int itrig = 0; itrig < 6; itrig++) trigger_dependency[itrig] = 1; //set all triggers to have no dependency
  trigger_dependency[index_Electron] = BBCLL1_fired;

  for (int itrig = 0; itrig < 6; itrig++) IsBlueLogic[itrig] = false;
  IsBlueLogic[index_4x4a] = true;
  IsBlueLogic[index_4x4b] = true;
  IsBlueLogic[index_4x4c] = true;

  // zero out sync error flags for the simulator
  short SimSyncError[ARMS][FIBERS];
  memset(SimSyncError, 0, sizeof(SimSyncError));

  simERTLl1->calculate(SimSyncError);

  for (int iarm = 0; iarm < 2; iarm++)
    {
      simERTLL1fired[iarm][index_4x4a] = simERTLl1->TrigFbFA(iarm);
      simERTLL1fired[iarm][index_4x4b] = simERTLl1->TrigFbFB(iarm);
      simERTLL1fired[iarm][index_4x4c] = simERTLl1->TrigFbFC(iarm);

      simERTLL1fired[iarm][index_2x2] = simERTLl1->TrigTbT(iarm);

      if (!IsBlueLogic[index_Electron])
	{
	  simERTLL1fired[iarm][index_Electron] = simERTLl1->TrigElectron(iarm);
	}
      else
	{
	  simERTLL1fired[iarm][index_Electron] = simERTLl1->TrigElectronBlueLogic(iarm);
	}
      simERTLL1fired[iarm][index_TwoElectron] = simERTLl1->TrigTwoElectron(iarm);
    }

  for (int arm_i = 0; arm_i < 2; arm_i++)
    {
      for (int itrig = 0; itrig < 6; itrig++)
	{
	  if (ERTLL1fired[arm_i][itrig]) ertfired = true;
	}
    }

  for (int iarm = 0; iarm < 2; iarm++)
    {

      EvtCounter[iarm]++;

      for (int itrig = 0; itrig < 6; itrig++)
	{
	  if (!trigger_dependency[itrig])
	    {
	      continue;
	    }

	  if (simERTLL1fired[iarm][itrig])
	    {
	      ertfired = true;

	      if (ERTLL1fired[iarm][itrig])
		{

		  MasterCounter[RealAndSimCounter_index][iarm][itrig]++;//(LL1 & Sim) "real AND sim fired, incease counter"
		}
	      else
		{
		  MasterCounter[SimAndNoRealCounter_index][iarm][itrig]++;//(No GL1) "real didn't fire but sim did, increase counter"
		  //the LL1 didn't fire but the sim did
		}
	    } //if simERTLL1fired

	  if (! (simERTLL1fired[EAST][itrig] || simERTLL1fired[WEST][itrig]) && ERTLL1fired[iarm][itrig])
	    {
	      MasterCounter[RealAndNoSimCounter_index][iarm][itrig]++;//(No Sim) [0] means "real fired but sim didn't, increase counter"
	    }
	} // for itrig
    } // for iarm

  for (int iarm = 0; iarm < 2; iarm++)
    {
      for (int itrig = 0; itrig < 6; itrig++)
	{
	  MasterCounter[CounterSum_index][iarm][itrig] = MasterCounter[RealAndNoSimCounter_index][iarm][itrig] +
	    MasterCounter[RealAndSimCounter_index][iarm][itrig] +
	    MasterCounter[SimAndNoRealCounter_index][iarm][itrig];
	}
    }

  for (int iarm = 0; iarm < 2; iarm++)
    {
      for (int i = 0; i < 3; i++)
	{
	  if (EvtCounter[iarm] > 0)
	    {
	      ErtLvl1_FFASim[iarm]->SetBinContent(4 * i + 3, (float)(MasterCounter[i][iarm][index_4x4a]) / (float)(EvtCounter[iarm]) );
	      ErtLvl1_FFBSim[iarm]->SetBinContent(4 * i + 3, (float)(MasterCounter[i][iarm][index_4x4b]) / (float)(EvtCounter[iarm]) );
	      ErtLvl1_FFCSim[iarm]->SetBinContent(4 * i + 3, (float)(MasterCounter[i][iarm][index_4x4c]) / (float)(EvtCounter[iarm]) );
	      ErtLvl1_TBTSim[iarm]->SetBinContent(4 * i + 3, (float)(MasterCounter[i][iarm][index_2x2]) / (float)(EvtCounter[iarm]) );
	      ErtLvl1_ERONSim[iarm]->SetBinContent(4 * i + 3, (float)(MasterCounter[i][iarm][index_Electron]) / (float)(EvtCounter[iarm]) );
	      ErtLvl1_TERONSim[iarm]->SetBinContent(4 * i + 3, (float)(MasterCounter[i][iarm][index_TwoElectron]) / (float)(EvtCounter[iarm]) );
	    }
	}
    }

  for (int iarm = 0; iarm < 2; iarm++)
    {
      if (EvtCounter[iarm] > 0)
	{
	  ErtLvl1_FFA[iarm]->SetBinContent(6, (float)(MasterCounter[CounterSum_index][iarm][index_4x4a]) / (float)(EvtCounter[iarm]) );
	  ErtLvl1_FFB[iarm]->SetBinContent(6, (float)(MasterCounter[CounterSum_index][iarm][index_4x4b]) / (float)(EvtCounter[iarm]) );
	  ErtLvl1_FFC[iarm]->SetBinContent(6, (float)(MasterCounter[CounterSum_index][iarm][index_4x4c]) / (float)(EvtCounter[iarm]) );
	  ErtLvl1_TBT[iarm]->SetBinContent(6, (float)(MasterCounter[CounterSum_index][iarm][index_2x2]) / (float)(EvtCounter[iarm]) );
	  ErtLvl1_ERON[iarm]->SetBinContent(6, (float)(MasterCounter[CounterSum_index][iarm][index_Electron]) / (float)(EvtCounter[iarm]) );
	  ErtLvl1_TERON[iarm]->SetBinContent(6, (float)(MasterCounter[CounterSum_index][iarm][index_TwoElectron]) / (float)(EvtCounter[iarm]) );
	}
    }

  //  string armname[2] = {"EAST","WEST"};
  int ROCmap[8][6] = {{0, 1, 0, 1, 2, 3},
		      {2, 3, 0, 1, 2, 3},
		      { -1, -1, 0, 1, 2, 3},
		      {0, 1, 0, 1, 2, 3},
		      {2, 3, 0, 1, 2, 3},
		      { -1, -1, 0, 1, 2, 3},
		      {4, 4, -1, 4, 4, -1},
		      {4, 4, -1, 4, 4, -1}
  };

  for (int iarm = 0; iarm < 2; iarm++)
    {

      if (!ertfired) continue;
      printevent = true;
      //      cout << " ERT fired in " << armname[iarm].c_str() << ", event " << rawevtcnt << ". " << endl;
      for (int i = 0; i < 8; i++)
	{

	  for (int bitword_i = 0; bitword_i < 6; bitword_i++)
	    {

	      if (simERTLl1->getROCbit(iarm, i, bitword_i))
		{
		  int itrig = ROCmap[i][bitword_i];

		  if (!trigger_dependency[itrig])
		    {
		      continue;
		    }

		  hRocWord[iarm]->Fill(bitword_i, i);

		  if (ERTLL1fired[EAST][itrig] || ERTLL1fired[WEST][itrig])
		    hRocWordERTLL1[iarm]->Fill(bitword_i, i);
		  // }
		} // if simERTLl1's ROC bit is high
	    } // for bitword: 0->6
	} // for i: 0->8
    } // for iarm


  // DB commits
  string database_trigstrings[2][6]; // [arm][trig]

  database_trigstrings[0][0] = "ERTLL14x4aEff_E";
  database_trigstrings[0][1] = "ERTLL14x4bEff_E";
  database_trigstrings[0][2] = "ERTLL14x4cEff_E";
  database_trigstrings[0][3] = "ERTLL12x2Eff_E";
  database_trigstrings[0][4] = "ERTLL1ElectronEff_E";
  database_trigstrings[0][5] = "ERTLL1TwoElectronEff_E";

  database_trigstrings[1][0] = "ERTLL14x4aEff_W";
  database_trigstrings[1][1] = "ERTLL14x4bEff_W";
  database_trigstrings[1][2] = "ERTLL14x4cEff_W";
  database_trigstrings[1][3] = "ERTLL12x2Eff_W";
  database_trigstrings[1][4] = "ERTLL1ElectronEff_W";
  database_trigstrings[1][5] = "ERTLL1TwoElectronEff_W";

  OnlMonServer *se = OnlMonServer::instance();
  if (se->CurrentTicks() > (LastCommit + 600))   // 10 minutes
    {

      int no_div_zero = 1;
      for (int iarm = 0; iarm < 2; iarm++)
	{
	  for (int itrig = 0; itrig < 6; itrig++)
	    {

	      if (itrig == index_TwoElectron)
		{
		  continue;
		}

	      if (MasterCounter[1][iarm][itrig] + MasterCounter[2][iarm][itrig] > 0)
		{
		  continue;
		}
	      no_div_zero = 0;
	    }
	}

      if (no_div_zero > 0)
	{
	  //printf(" Committing efficiencies to database, seconds since last commit = %ld\n", se->CurrentTicks() - LastCommit);

	  float eff, eff_err;

	  for (int iarm = 0; iarm < 2; iarm++)
	    {
	      for (int itrig = 0; itrig < 6; itrig++)
		{

		  if (itrig == index_TwoElectron)
		    {
		      continue;
		    }

		  eff = (float)MasterCounter[RealAndSimCounter_index][iarm][itrig] / (float)(MasterCounter[RealAndSimCounter_index][iarm][itrig] + MasterCounter[SimAndNoRealCounter_index][iarm][itrig]);
		  eff_err = sqrt(eff * (1.0 - eff) / (float)(MasterCounter[RealAndSimCounter_index][iarm][itrig] + MasterCounter[SimAndNoRealCounter_index][iarm][itrig]) );

		  dbvars->SetVar(database_trigstrings[iarm][itrig], eff, eff_err, (float) evtcnt);
		}
	    }

	  dbvars->DBcommit();

	}

      LastCommit = se->CurrentTicks();
    }

  // All Done!
  if ((evtcnt % 10000) == 0)
    {

      for (int iarm = 0; iarm < 2; iarm++)
	{

	  for (int iroc = 0; iroc < 8; iroc++)
	    {

	      for (int bitword_i = 0; bitword_i < 6; bitword_i++)
		{

		  // using a algorithm that count all emulated hits, not only one for each GL1 trigger. CLS 02/01/2010
		  float total_cnt = hRocWord[iarm]->GetBinContent(bitword_i + 1, iroc + 1);
		  float fired_cnt = hRocWordERTLL1[iarm]->GetBinContent(bitword_i + 1, iroc + 1);
		  hRocEff[iarm]->SetBinContent(iroc * 6 + bitword_i + 1, (total_cnt) ? fired_cnt / total_cnt : 0);
		  hRocEntries[iarm]->SetBinContent(iroc * 6 + bitword_i + 1, total_cnt);
		}
	    }//for iroc
	}//for iarm
    }//if((evtcnt%10000)==0)
  return 0;
}

bool ErtLvl1Mon::checkEventTrigger(Event *event)
{
  Packet* p;
  if ((p = event->getPacket(14001)))
    {
      long int livtrig = p->iValue(0, "RAWTRIG");
      delete p;
      if (livtrig & PPG_Pedestal)
	{
	  return false;   //The PPGs set all the ROC bits high
	}
      if (livtrig & PPG_TestPulse)
	{
	  return false;   //doesn't represent true efficiency
	}
      if (livtrig & PPG_Laser)
	{
	  return false;
	}
      //if (livtrig&Run11_BBCLL1novertex) return true;  // BBCLL1 events
    }
  return true;
}

int ErtLvl1Mon::DBVarInit()
{
  // variable names are not case sensitive
  string varname;
  varname = "ERTLL12x2Eff_E";
  dbvars->registerVar(varname);
  varname = "ERTLL14x4aEff_E";
  dbvars->registerVar(varname);
  varname = "ERTLL14x4bEff_E";
  dbvars->registerVar(varname);
  varname = "ERTLL14x4cEff_E";
  dbvars->registerVar(varname);
  varname = "ERTLL1ElectronEff_E";
  dbvars->registerVar(varname);
  varname = "ERTLL1TwoElectronEff_E";
  dbvars->registerVar(varname);
  varname = "ERTLL12x2Eff_W";
  dbvars->registerVar(varname);
  varname = "ERTLL14x4aEff_W";
  dbvars->registerVar(varname);
  varname = "ERTLL14x4bEff_W";
  dbvars->registerVar(varname);
  varname = "ERTLL14x4cEff_W";
  dbvars->registerVar(varname);
  varname = "ERTLL1ElectronEff_W";
  dbvars->registerVar(varname);
  varname = "ERTLL1TwoElectronEff_W";
  dbvars->registerVar(varname);
  //dbvars->Print();
  dbvars->DBInit();
  return 0;
}


int ErtLvl1Mon::Reset()
{
  evtcnt = 0;
  rawevtcnt = 0;
  LastCommit = 0;

  memset(EvtCounter, 0, sizeof(EvtCounter));
  memset(MasterCounter, 0, sizeof(MasterCounter));

  // Clear the trigger counters
  memset(FFA, 0, sizeof(FFA));
  memset(FFB, 0, sizeof(FFB));
  memset(FFC, 0, sizeof(FFC));
  memset(TBT, 0, sizeof(TBT));
  memset(ERON, 0, sizeof(ERON));
  memset(TERON, 0, sizeof(TERON));
  memset(FFASim, 0, sizeof(FFASim));
  memset(FFBSim, 0, sizeof(FFBSim));
  memset(FFCSim, 0, sizeof(FFCSim));
  memset(TBTSim, 0, sizeof(TBTSim));
  memset(ERONSim, 0, sizeof(ERONSim));
  memset(TERONSim, 0, sizeof(TERONSim));

  return 0;
}

void ErtLvl1Mon::CreateHistograms()
{
  const string ArmEW[2] = {"E", "W"};
  for (int iarm = 0; iarm < 2; iarm++)
    {
      std::string ErtHistogramNames[6];
      ErtHistogramNames[index_4x4a] = "ErtLvl1_FFA";
      ErtHistogramNames[index_4x4b] = "ErtLvl1_FFB";
      ErtHistogramNames[index_4x4c] = "ErtLvl1_FFC";
      ErtHistogramNames[index_2x2] = "ErtLvl1_TBT";
      ErtHistogramNames[index_Electron] = "ErtLvl1_ERON";
      ErtHistogramNames[index_TwoElectron] = "ErtLvl1_TERON";

      std::string ErtHistogramTitles[6];
      ErtHistogramTitles[index_4x4a] = "4x4a Trigger Frequency";
      ErtHistogramTitles[index_4x4b] = "4x4b Trigger Frequency";
      ErtHistogramTitles[index_4x4c] = "4x4c Trigger Frequency";
      ErtHistogramTitles[index_2x2] = "2x2 Trigger Frequency";
      ErtHistogramTitles[index_Electron] = "Electron Trigger Frequency";
      ErtHistogramTitles[index_TwoElectron] = "2-Electron Trigger Frequency";

      ErtLvl1_FFA[iarm] = new TH1F((ErtHistogramNames[index_4x4a] + ArmEW[iarm]).c_str(), ErtHistogramTitles[index_4x4a].c_str(), 12, -1.5, 1.5);
      ErtLvl1_FFB[iarm] = new TH1F((ErtHistogramNames[index_4x4b] + ArmEW[iarm]).c_str(), ErtHistogramTitles[index_4x4b].c_str(), 12, -1.5, 1.5);
            ErtLvl1_FFC[iarm] = new TH1F((ErtHistogramNames[index_4x4c] + ArmEW[iarm]).c_str(), ErtHistogramTitles[index_4x4c].c_str(), 12, -1.5, 1.5);
            ErtLvl1_TBT[iarm] = new TH1F((ErtHistogramNames[index_2x2] + ArmEW[iarm]).c_str(), ErtHistogramTitles[index_2x2].c_str(), 12, -1.5, 1.5);
            ErtLvl1_ERON[iarm] = new TH1F((ErtHistogramNames[index_Electron] + ArmEW[iarm]).c_str(), ErtHistogramTitles[index_Electron].c_str(), 12, -1.5, 1.5);
            ErtLvl1_TERON[iarm] = new TH1F((ErtHistogramNames[index_TwoElectron] + ArmEW[iarm]).c_str(), ErtHistogramTitles[index_TwoElectron].c_str(), 12, -1.5, 1.5);

            ErtLvl1_FFASim[iarm] = new TH1F((ErtHistogramNames[index_4x4a] + "Sim" + ArmEW[iarm]).c_str(), (ErtHistogramTitles[index_4x4a] + " (Sim.)").c_str(), 12, -1.5, 1.5);
            ErtLvl1_FFBSim[iarm] = new TH1F((ErtHistogramNames[index_4x4b] + "Sim" + ArmEW[iarm]).c_str(), (ErtHistogramTitles[index_4x4b] + " (Sim.)").c_str(), 12, -1.5, 1.5);
            ErtLvl1_FFCSim[iarm] = new TH1F((ErtHistogramNames[index_4x4c] + "Sim" + ArmEW[iarm]).c_str(), (ErtHistogramTitles[index_4x4c] + " (Sim.)").c_str(), 12, -1.5, 1.5);
            ErtLvl1_TBTSim[iarm] = new TH1F((ErtHistogramNames[index_2x2] + "Sim" + ArmEW[iarm]).c_str(), (ErtHistogramTitles[index_2x2] + " (Sim.)").c_str(), 12, -1.5, 1.5);
            ErtLvl1_ERONSim[iarm] = new TH1F((ErtHistogramNames[index_Electron] + "Sim" + ArmEW[iarm]).c_str(), (ErtHistogramTitles[index_Electron] + " (Sim.)").c_str(), 12, -1.5, 1.5);
            ErtLvl1_TERONSim[iarm] = new TH1F((ErtHistogramNames[index_TwoElectron] + "Sim" + ArmEW[iarm]).c_str(), (ErtHistogramTitles[index_TwoElectron] + " (Sim.)").c_str(), 12, -1.5, 1.5);

            string htit = "ErtLvl1_hRocEff" + ArmEW[iarm];
            hRocEff[iarm] = new TH1F(htit.c_str(), "Roc Efficiencies by Group",
                                     48, -0.5, 47.5);

            htit = "ErtLvl1_hRocEntries" + ArmEW[iarm];
            hRocEntries[iarm] = new TH1F(htit.c_str(), "Entries per ROC",
                                         48, -0.5, 47.5);

            hRocWord[iarm] = new TH2F(Form("ErtLvl1_hRocWord%d", iarm), "ERT;word bit;Slot Fiber", 6, -0.5, 5.5, 8, -0.5, 7.5);
            hRocWordERTLL1[iarm] = new TH2F(Form("ErtLvl1_hRocWordERTLL1%d", iarm), "ERTLL1;word bit;Slot Fiber", 6, -0.5, 5.5, 8, -0.5, 7.5);
        }
}

void ErtLvl1Mon::RegisterHistograms()
{


    for (int i = 0; i < 2; i++)
        {
            OnlMonServer *se = OnlMonServer::instance();

            se->registerHisto(this, hRocWord[i]);
            se->registerHisto(this, hRocWordERTLL1[i]);
            se->registerHisto(this, ErtLvl1_FFA[i]);
            se->registerHisto(this, ErtLvl1_FFB[i]);
            se->registerHisto(this, ErtLvl1_FFC[i]);
            se->registerHisto(this, ErtLvl1_TBT[i]);
            se->registerHisto(this, ErtLvl1_ERON[i]);
            se->registerHisto(this, ErtLvl1_TERON[i]);
            se->registerHisto(this, ErtLvl1_FFASim[i]);
            se->registerHisto(this, ErtLvl1_FFBSim[i]);
            se->registerHisto(this, ErtLvl1_FFCSim[i]);
            se->registerHisto(this, ErtLvl1_TBTSim[i]);
            se->registerHisto(this, ErtLvl1_ERONSim[i]);
            se->registerHisto(this, ErtLvl1_TERONSim[i]);
            se->registerHisto(this, hRocEff[i]);
            se->registerHisto(this, hRocEntries[i]);
        }
}



