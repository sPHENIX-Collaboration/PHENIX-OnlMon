#include <PadMon.h>
#include <PadOnlineRec.h>
#include <PadCalibrationObject.hh>

#include <OnlMonServer.h>

#include <Event.h>
#include <EventTypes.h>
#include <msg_profile.h>


#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>

#include <sys/utsname.h>
#include <iostream>
#include <sstream>

using namespace std;


PadMon::PadMon(): OnlMon("PADMON")
{

  // Instantiate the pcFemThisEvent array of padOnlineRec
  int installedplanes = 0;
  int j, k, packetid;

  memset(packetActive,0,sizeof(packetActive));

  for (j = 0; j < NPLANES; j++)
    {
      if (PC_INSTALLED[j])
        {
          for (k = 0; k < NFEMSPERPLANE; k++)
            {
              packetid = FIRSTPACKET + j * NFEMSPERPLANE + k;
	      // printf("packet id = %i\n",packetid);
	      // pcfem[j][k] = new PadOnlineRec(packetid);
              pcFemThisEvent[j][k] = new PadOnlineRec(packetid);
            }
          installedplanes++;
        } // End "if PCINSTALLED[j]"
    } // End Loop over NPLANES

}


PadMon::~PadMon()
{
  //  cout<<"This is the PadMon destructor..."<<endl;
  int j, k;
  for (j = 0; j < NPLANES; j++)
    {
      if (PC_INSTALLED[j])
        {
          for (k = 0; k < NFEMSPERPLANE; k++)
            {
              // delete pcfem[j][k];
              delete pcFemThisEvent[j][k];
            }
        } // End "if PCINSTALLED[j]"
    } // End Loop over NPLANES
}


int PadMon::Init()
{
  firstEvent = 0;
  lastEvent = 0;

  OnlMonServer *Onlmonserver = OnlMonServer::instance();

  //
  // create FEM objects and histograms
  //
  int j, k;
  char idstring[128];
  char title[128];
  int pc, arm, hv;
  int binnr = 0;
  int MINPACKET = 0, MAXPACKET = 0;
  int installedplanes = 0;

  //initialize packetActive
  for(int i = 1; i < PC_NINSTALLED*NFEMSPERPLANE;i++)
    {
      packetActive[i] = Onlmonserver->IsPacketActive(i+4000);
    }

  for (j = 0; j < NPLANES; j++)
    {
      if (PC_INSTALLED[j])
        {
          // *** set counters for event address check
          for (k = 0; k < NFEMSPERPLANE; k++)
            {
              // packetid = FIRSTPACKET + j * NFEMSPERPLANE + k;
              packeterrornr[binnr] = 0;
              packetanalyzednr[binnr] = 0;
              binnr++;
            }

          pc = j / 2;
          arm = 1 - j % 2;
          switch (pc)
            {
            case 0:
              if ( arm == 1)
                {
                  MINPACKET = 4000;
                  MAXPACKET = 4017;
                }
              else
                {
                  MINPACKET = 4016;
                  MAXPACKET = 4033;
                }
              break;
            case 1:
              MINPACKET = 4032;
              MAXPACKET = 4049;
              break;
            case 2:
              if (arm == 1)
                {
                  MINPACKET = 4064;
                  MAXPACKET = 4081;
                }
              else
                {
                  MINPACKET = 4080;
                  MAXPACKET = 4097;
                }
            }

          // each bin is a fem
          // how many bits per packet per event?
          sprintf (idstring, "padProfPadSum%d", j);
          sprintf (title, "PC %d Arm %d PadSum profile", pc + 1, arm);
          padProfPadSum[installedplanes] = new TProfile(idstring, title,
							NFEMSPERPLANE, -0.5, NFEMSPERPLANE - 0.5);
          Onlmonserver->registerHisto(this, padProfPadSum[installedplanes]);


          // # of fired clusters
          sprintf (idstring, "padProfClustActivity%d", j);
          sprintf (title, "PC %d Arm %d ClustActivity profile", pc + 1, arm);
          padProfClustActivity[installedplanes] = new TProfile (idstring, title,
								NHVSECTORS, -0.5, NHVSECTORS - 0.5);
          Onlmonserver->registerHisto(this, padProfClustActivity[installedplanes]);



          // # of fired pads
          sprintf (idstring, "padMultiplicity%d", j);
          sprintf (title, "PC %d Arm %d Pad Multiplicity", pc + 1, arm);
          padMultiplicity[installedplanes] = new TH1F (idstring, title,
						       2200, 0, 2200);
          Onlmonserver->registerHisto(this, padMultiplicity[installedplanes]);


          sprintf (idstring, "padPadsPerPacket%d", j);
          sprintf(title, "PC %d Arm %d Number of fired pads per packet", pc + 1, arm);
          padPacketidhist[installedplanes] = new TH2F(idstring, title, 16, MINPACKET, MAXPACKET, 2200, 0, 2200);
          Onlmonserver->registerHisto(this, padPacketidhist[installedplanes]);


          // each bin is a HV sector
          // how large are the clusters?
          sprintf (idstring, "padProfClustSize%d", j);
          sprintf (title, "PC %d Arm %d ClustSize profile", pc + 1, arm);
          padProfClustSize[installedplanes] = new TProfile (idstring, title,
							    NHVSECTORS, -0.5, NHVSECTORS - 0.5);
          Onlmonserver->registerHisto(this, padProfClustSize[installedplanes]);
          // ratio 2-cell to 1-cell clusters
          // Avoid doing this event by event, use H1 histos
          // too low ratio => increase HV, too high ratio => lower HV
          //	  sprintf (idstring, "padProfClustRatio%d", j);
          //	  sprintf (title, "PC %d Arm %d ClustRatio histogram", pc+1, arm);
          //	  padProfClustRatio[installedplanes] =  new TH1F(idstring, title,
          //						       NHVSECTORS, -0.5, NHVSECTORS-0.5);
          //          Onlmonserver->registerHisto(this,idstring,padProfClustRatio[installedplanes]);


          for (k = 0; k < NHVSECTORS; k++)
            {
              hv = installedplanes * NHVSECTORS + k;
              sprintf(idstring, "padClustSize%d", hv);
              sprintf(title, "PC %d Arm %d HVSector %d Clustersize", pc + 1, arm, k);
              padClustSize[hv] = new TH1F(idstring, title,
                                          MAX_CLUSTERSIZE + 1, -0.5, MAX_CLUSTERSIZE + 0.5);
              Onlmonserver->registerHisto(this, padClustSize[hv]);
            }

          for (k = 0; k < NFEMSPERPLANE; k++)
            {

              int femid = installedplanes * NFEMSPERPLANE + k;

              sprintf(idstring, "padFemPadxPadz%d", femid);
              sprintf(title, "PC %d Arm %d FEM %d Padx vs Padz", pc + 1, arm, k);
              padFemPadxPadz[femid] = new TH2F(idstring, title,
                                               NPADZ, -0.5, NPADZ - 0.5, NPADX, -0.5, NPADX - 0.5);
              Onlmonserver->registerHisto(this, padFemPadxPadz[femid]);

              sprintf(idstring, "padFemCellxCellz%d", femid);
              sprintf(title, "PC %d Arm %d FEM %d Cellx vs Cellz", pc + 1, arm, k);
              padFemCellxCellz[femid] = new TH2F(idstring, title,
                                                 NCELLZ, -0.5, NCELLZ - 0.5, NWIRES, -0.5, NWIRES - 0.5);
              Onlmonserver->registerHisto(this, padFemCellxCellz[femid]);


            }
          installedplanes++;
        } // End "if PCINSTALLED[j]"

    } // End Loop over NPLANES


  // additions from Sasha, basically to improve appearence of histograms
  //  padClustRatio =  new TH1F("padClustRatio", "", PC_NINSTALLED*NHVSECTORS,
  //                            -0.5, PC_NINSTALLED*NHVSECTORS-0.5);
  //  Onlmonserver->registerHisto(this,"padClustRatio",padClustRatio);
  padClustActivity = new TProfile("padClustActivity", "", PC_NINSTALLED * NHVSECTORS,
                                  -0.5, PC_NINSTALLED * NHVSECTORS - 0.5);
  Onlmonserver->registerHisto(this, padClustActivity);
  padEventError = new TH1F("padEventError", "", PC_NINSTALLED * NFEMSPERPLANE, -0.5, PC_NINSTALLED * NFEMSPERPLANE - 0.5);
  Onlmonserver->registerHisto(this, padEventError);
  padMaxRocs = new TH1F("padMaxRocs", "", PC_NINSTALLED * NFEMSPERPLANE, -0.5, PC_NINSTALLED * NFEMSPERPLANE - 0.5);
  Onlmonserver->registerHisto(this, padMaxRocs);
  padStat = new TH1F("padStat", " ", 6, -0.5, 5.5);
  Onlmonserver->registerHisto(this, padStat);

  //  int iRet = SetUp();
  //  if (iRet < 0 )
  //    {
  //      msg_control *padmsg = new msg_control(MSG_TYPE_MONITORING,
  //                                            MSG_SOURCE_PC,
  //                                            MSG_SEV_WARNING, "PadMonitor");
  //      cout << *padmsg << " PadMon: Error in SetUp() " << endl;
  //      delete padmsg;
  //    }

  return 0;
}


//-------------------------------------------------------------------
int PadMon::SetUp(Event *evt)
{
  int packetid, grow, gcol, badROCtype, badtgltype[3];
  short success = 0;

  //
  // Read bad ROCs from Objy
  //
  PadCalibrationObject* PCO = new PadCalibrationObject();
  PHTimeStamp *currentRunTS = new PHTimeStamp(evt->getTime());
  //  currentRunTS->print();
  PHTimeStamp TS = *currentRunTS;
  delete currentRunTS; // have to delete timestamp (like the packets)
  PCO->setTimeStamp(TS);

  //  Starting in Run5, make change from reading the file from Objy to
  //  reading the bad ROC file for Online Monitoring from a text file
  //  stored in CVS with the monitoring code.  It's done this funky
  //  way to maintain *old* backward compatibility.

  char *newfile = new char[300];
  snprintf(newfile, 299, "%s/%s", 
          (getenv("PADCALIBDIR")  ? getenv("PADCALIBDIR")  : "."),
	  (getenv("PADCALIBNAME") ? getenv("PADCALIBNAME") : "padbadroc.txt")
	  );

  cout << "Attempting to open Bad ROC File: " << newfile << endl;
  PCO->FetchBadROCFromFile(newfile);
  delete [] newfile;

    cout << " Number of bad ROCs Objy: "<<PCO->getNumberBadROC()<<endl;

  for(int ibadroc=0;ibadroc<PCO->getNumberBadROC();ibadroc++){
    packetid=PCO->getPacketidBadROC(ibadroc);
    grow=PCO->getGrowBadROC(ibadroc);
    gcol=PCO->getGcolBadROC(ibadroc);
    badROCtype=PCO->getROCtypeBadROC(ibadroc);
    if(badROCtype == -1){ 
      // unsynch ROC 
      badtgltype[0] = -1;
      badtgltype[1] = -1;
      badtgltype[2] = -1;
    }
    else{
      badtgltype[0]=badROCtype/100;
      badtgltype[1]=(badROCtype%100)/10;
      badtgltype[2]=badROCtype%10;
    }

    // so, which FEM is this?
    int j=(packetid-FIRSTPACKET)/NFEMSPERPLANE;    
    int k=(packetid-FIRSTPACKET)%NFEMSPERPLANE;
    // set bad roc
    if(PC_INSTALLED[j]){ // ok, this guy is installed
	 success+=pcFemThisEvent[j][k]->setBadROC(grow,gcol,badtgltype);
    }

  }
  delete PCO;
  //
  // END Read bad ROCs from Objy
  //

  int binnr = 0;
  for (int j = 0; j < NPLANES; j++)
    {
      if (PC_INSTALLED[j])
        {
          for (int k = 0; k < NFEMSPERPLANE; k++)
            {
              // setbincontent and fill are shifted by one (binnr+1, binnr)
              padMaxRocs->Fill(binnr, MAX_ROCS - pcFemThisEvent[j][k]->getnrbadrocEvtNR());
              binnr++;
            }
        }
    }

  return 0;
}

int PadMon::Reset()
{
  firstEvent = 0;
  lastEvent = 0;
  return 0;

}

int PadMon::process_event(Event *evt)
{
  if (evt->getEvtType() != DATAEVENT)
    {                                                                       
      return 0;                                                             
    }                                                                       
   
  OnlMonServer *se = OnlMonServer::instance();
//   if (!(se->Trigger("ONLMONBBCLL1") ))
//     {
//       return 0;                                                             
//     }       
  // analyze event and fill histograms
  // return -1 if we threw away the event and 0 if we didn't (0 = ok)

  int iphv;
  int ihit;

  float arrayClustActivity[PC_NINSTALLED*NHVSECTORS];
  memset(arrayClustActivity, 0, sizeof(arrayClustActivity));

  int hvsector, sparsehv, clustsize;
  int numFiredPads = 0, packetid = 0;
  int installedplanes = 0;
  int missingplanes = 0;
  int nwires, npads;

  padStat->Fill(0.0);

  // chp: be careful, events are not really ordered
  // since they come in parallel from the atp's 


  if (firstEvent == 0){
      firstEvent = (int)evt->getEvtSequence();
      padStat->SetBinContent(2, firstEvent);
      // Reset nr of bad rocs for evt address check :
      for (int j = 0; j < NPLANES; j++)
	{
	  if (PC_INSTALLED[j])
	    {
	      for (int k = 0; k < NFEMSPERPLANE; k++)
		{
		  pcFemThisEvent[j][k]->setnrbadrocEvtNR(0);
		}
	    }
	}
      int iRet = SetUp(evt);
      if (iRet < 0 ){
	ostringstream msg;
        msg <<  " Error in SetUp() ";
	se->send_message(this,MSG_SOURCE_PC,MSG_SEV_WARNING, msg.str(),1);
      }
  }

  lastEvent = (int)evt->getEvtSequence();
  padStat->SetBinContent(3, lastEvent);


  int pc, arm, status;

  for (int j = 0; j < NPLANES; j++){
    
    if (PC_INSTALLED[j]){
      
      pc = j / 2;
      arm = 1 - j % 2;
      // npchits = 0;
      iphv = j * NHVSECTORS;
    


      for (int k = 0; k < NFEMSPERPLANE; k++){
	

	// status = pcfem[j][k]->processEvent(evt);
	status = pcFemThisEvent[j][k]->processEvent(evt);
	if (status == 0){
	  // No problem
	  npads = pcFemThisEvent[j][k]->getNumberFiredPads();
	  //printf("number of fired pads = %i\n",npads);
	  //put a maximum number of pads that fire??
	  if (npads == -999)
	    {
	      // this is the error return value
	      // for our purposes, it's equivalent with no fired pads
	      npads = 0;
	    }
	}
	else
	  {
	    // Suppress this message, preferred way for pp (JN 021203)
	    // cout<<" Status was not 0, Plane: "<<j<<"  FEM: "<<k<<endl;
	    npads = 0;
	  }
	
	// fill FEM bin histos
	padProfPadSum[installedplanes]->Fill(k, npads);
	
	if (npads > 0)
	  {
	    // something here, hard to find clusters otherwise
	    //
	    // also need to fill clustersize histograms
	    // use HV sectors instead of packets for cluster info
	    //Loop over clusters
	    for (ihit = 0; ihit < pcFemThisEvent[j][k]->getNumberClusters(); ihit++){
	      if (ihit < MAX_CLUSTERS) // then we are sure that the hit was stored
		{
		  clustsize = pcFemThisEvent[j][k]->getClusterSize(ihit);
		  hvsector = pcFemThisEvent[j][k]->getHVSector(iphv, pcFemThisEvent[j][k]->getClusterSectWire(ihit));
		  
		  sparsehv = hvsector - missingplanes * NHVSECTORS;
		  
		  // tmp debug
		  if (sparsehv < 0)
		    {
		      ostringstream msg;
		      msg  << " pad_fill() hvsector " << hvsector
			   << " j " << j
			   << " missingplanes " << missingplanes
			   << " installedplanes " << installedplanes
			   << " k " << k
			   << "wire = " << pcFemThisEvent[j][k]->getClusterSectWire(ihit);
			      se->send_message(this,MSG_SOURCE_PC,MSG_SEV_INFORMATIONAL,msg.str(),1);
		    }
		  else
		    {
		      arrayClustActivity[sparsehv]++;
		      padClustSize[sparsehv]->Fill(clustsize);
		    }
		 
		  // fill histos
	
		  int lochvsect = hvsector % NHVSECTORS;
		  padProfClustSize[installedplanes]->Fill(lochvsect, clustsize);
		

		}
	    } // End loop over number of clusters
	    
	   

	    // Fill the histos with info on how many times each pad fired
	  
	    short padindex;
	    int padz, padx;
	    //printf("Number of fired pads = %i\n",numFiredPads); // different from 0
	    
	    int femid = installedplanes * NFEMSPERPLANE + k;
	    packetid = FIRSTPACKET + j * NFEMSPERPLANE + k;  
	    numFiredPads = pcFemThisEvent[j][k]->getNumberFiredPads();

	    //fills histo with number of fired pads per packet - 03/30/07
	    //if (numFiredPads>200) numFiredPads=200;
	    padPacketidhist[installedplanes]->Fill(packetid,numFiredPads); 
	   
	    //padMultiplicity[installedplanes]->Fill(numFiredPads);  

	    for(int i = 0; i < numFiredPads; i++) {
	      padindex = pcFemThisEvent[j][k]->getFiredPad(i);
	      padx = int(padindex) % NPADX;
	      padz = int(padindex) / NPADX;
	       
	      padFemPadxPadz[femid]->Fill(padz,padx,pcFemThisEvent[j][k]->getNumberOfTimesPadFired(padindex));
	   
	    }
	    
	    // Fill the histos with info on how many times each cell fired
	    // for (int cellz = 0; cellz < NCELLZ; cellz++)
	    //  {
	    //    for (int wire = 0; wire < NWIRES; wire++)
	    //      {
	    //        int cellindex = cellz * NWIRES + wire;
	    //        padFemCellxCellz[femid]->Fill(cellz, wire, pcFemThisEvent[j][k]->getNumberOfTimesCellFired(cellindex));
	    //      }
	    //  }
	    // NEW
	    
	    // printf("Max cells = %i\n",MAX_CELLS);
	    int cellz[MAX_CELLS];
	    int cellwire[MAX_CELLS];
	    int numFiredCells = pcFemThisEvent[j][k]->getFiredCellInfo(MAX_CELLS, cellz, cellwire);
	    

	    //printf("Number of fired cells = %i\n",numFiredCells);
	    
	    for(int i = 0; i < numFiredCells; i++) {
    
	      //#ifdef DEBUG 	      
	      padFemCellxCellz[femid]->Fill(cellz[i], cellwire[i]); 
	      //#endif	      
	  
	    } 
	    
	  } // End if npads>0
      } // End Loop over NFEMSPERPLANE
      
      //padMultiplicity[installedplanes]->Fill(numFiredPads); 

      for (int k = 0; k < NHVSECTORS; k++){
	
	sparsehv = iphv - missingplanes * NHVSECTORS + k;
	
	// renormalize the entries with the number of wires per HV sector
	nwires = nwiresPerHVSector[(j / 2) * 4 + k % 4];
	
	// printf("Number of wires = %i\n",nwires);
	      if (j < 2)
                nwires *= 2; // PC1 wires cover the whole acceptance, not PC2/3
              arrayClustActivity[sparsehv] /= nwires;

	      //#ifdef DEBUG 
              padProfClustActivity[installedplanes]->Fill(k, arrayClustActivity[sparsehv]);
	      //#endif
	    
	      //#ifdef DEBUG 
              padClustActivity->Fill(installedplanes*NHVSECTORS + k, arrayClustActivity[sparsehv]);
	      //#endif
              // This is a kludge to compensate for the 9 ROCs in PC1E masked out in Run 3 (JN 030218) 
              // These 9 ROCs correspond to exactly 1/2 of a HV sector, fill twice to compensate. 
            
	      //#ifdef DEBUG 
	      if( installedplanes == 1 && k==24 )
		padClustActivity->Fill(installedplanes*NHVSECTORS + k, arrayClustActivity[sparsehv]);
	      //#endif
              // End kludge
	      
      } // End Loop over HV sectors
      
      padMultiplicity[installedplanes]->Fill(numFiredPads); 
      installedplanes++;
      
    }
    else
      {
	missingplanes++;	
      } // End "if plane installed"
    
  } // End Loop over planes
  
  
  // event address check below...
  int tmpnrtypes[5];
  memset(tmpnrtypes, 0, sizeof(tmpnrtypes));

  int nrpacketswest = 0;
  int nrpacketseast = 0;

  // *** sum up the contributions from each packet
  for (int j = 0; j < NPLANES; j++){
      if (PC_INSTALLED[j]){
          for (int k = 0; k < NFEMSPERPLANE; k++){
              for (int i = 0;i < 5;i++){
                  tmpnrtypes[i] += pcFemThisEvent[j][k]->getnrCorrEvtNR(i);
                  if (pcFemThisEvent[j][k]->getEvtNR() != -2){
                      if (j % 2 == 0) {
                        nrpacketswest++;
			}
                      else
			{
                        nrpacketseast++;
			}
                    }
                }
            }
        }
    }

  // *** only check events where X nr of packets exist for each arm
  if (nrpacketswest >= NRPACKETS_REQUIRED_WEST && nrpacketseast >= NRPACKETS_REQUIRED_EAST)
    {

      // *** which event nr has the most occurances?
      int tmpmax = 0;
      int correctevttype = 0;
      for (int i = 0;i < 5;i++){
          if (tmpnrtypes[i] > tmpmax){
              tmpmax = tmpnrtypes[i];
              correctevttype = i;
            }
        }

      // *** loop over each packet to see if it disagrees, fill histo
      
      int binnr = 0;
      for (int j = 0; j < NPLANES; j++){
	if (PC_INSTALLED[j]){
	  for (int k = 0; k < NFEMSPERPLANE; k++){
	    if (pcFemThisEvent[j][k]->getEvtNR() != -2){
	      packetanalyzednr[binnr]++;
	      if (pcFemThisEvent[j][k]->getnrCorrEvtNR(correctevttype) < (MAX_ROCS - pcFemThisEvent[j][k]->getnrbadrocEvtNR()))
		{
		  packeterrornr[binnr]++;
		  padEventError->SetBinContent(binnr + 1, (MAX_ROCS - pcFemThisEvent[j][k]->getnrbadrocEvtNR()) 
					       - pcFemThisEvent[j][k]->getnrCorrEvtNR(correctevttype));
		}
	      if (100.0*packeterrornr[binnr] / packetanalyzednr[binnr] <= MAX_ERROR_PER_PACKET)
		{
		  padEventError->SetBinContent(binnr + 1, 0);
		}
	    }
	    binnr++;
	  }
	}
      }
      
      for(int i = 1; i < PC_NINSTALLED*NFEMSPERPLANE; i++){
	if(!packetActive[i])
	  {
	    packetanalyzednr[i-1] = EVENTS_REQUIRED_EVTNR+1;
	    //cout << "Packet " << i << " read as inactive" << endl;
	  }
      }

      packetanalyzednr[69] = EVENTS_REQUIRED_EVTNR+1;
      packetanalyzednr[70] = EVENTS_REQUIRED_EVTNR+1;
      packetanalyzednr[71] = EVENTS_REQUIRED_EVTNR+1;

      // *** fill bin 5 of padstat with lowest value of packetanalyzednr array
      int tmpmin = packetanalyzednr[0];
      for (int i = 1;i < PC_NINSTALLED*NFEMSPERPLANE;i++)
        {
          if (packetanalyzednr[i] < tmpmin)
            {
              tmpmin = packetanalyzednr[i];
	      padStat->SetBinContent(6, i+1);
            }
        }
      padStat->SetBinContent(5, tmpmin);
      
    }

  return 0;
}

