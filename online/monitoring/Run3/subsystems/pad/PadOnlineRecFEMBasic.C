//---------------------------------------------------------------  
//                                                                 
// Created by: David Silvermyr 
// Modified for Run 3 by: Joakim Nystrand and Henrik Tydesjo
//                                                                 
// Description: Implementation of padOnlineRecFEMBasic class 
//                                                                 
//---------------------------------------------------------------- 
#include <iostream> 
#include "Event.h"
#include "packet.h"
#include "PadOnlineRecFEMBasic.h"  

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

// constructor 
padOnlineRecFEMBasic::padOnlineRecFEMBasic(short ipacketid) 
{ 
  packetid=ipacketid; 
  decodePacketid();

  if ( layer==1 ) 
    {
    pc2check = 1;
    }
  else 
    {
    pc2check = -1;
    }

  numberBadCh=0;
  numberBadROC=0;

  // *** for event address check
  // set the badrocEvtNR array to zero (good as default)
  nrbadrocEvtNR=0;
  for (int i=0;i<MAX_ROCS;i++) {
    badrocEvtNR[i]=0;
  }

  diagonalAdj = 1; // allow diagonal adj. cells to be treated as neighbours

  reset(); // reset all counters
  // Reset badchannel here, since this was removed from the reset function (JN 021203)
  for(int k=0; k<MAX_PADS; k++)
    {
badchannel[k]=0;
    }

} 

//********************************************************************** 
int padOnlineRecFEMBasic::processEvent(Event * evt) 
{
  short k,nrpads,nrcells;
  short kt,km,kb,cellz,cellwire,zmod3;
  short realkt,realkm,realkb;
  const short NPOSS = 3;
  short possible[NPOSS];
  short j,kbmodX;

  numberOfFiredPads = 0;
  numberOfFiredCells = 0;
  numberOfClusters = 0;

  if (iFlag==-1) {
    cerr << "padOnlineRecFEMBasic::processEvents ERROR  not initialized. \n";
    return -999;
  }
  iFlag=0; // reset the Flag..
  Packet * p = evt->getPacket(packetid);
  if (p)
    {
      iFlag=1; // we have read the right packet this event
      numberProcEvents++;

      // *** used in the check for bad event addresses
      for (int ROC = 0; ROC<9; ROC++) {
	int ctrval =  p->iValue(8-ROC,"CHECKCTR") & 0xfffff;
	for (int ROW = 0; ROW<5; ROW++) {
	  if (badrocEvtNR[ROC*NROWS+ROW]==0) { // check that the ROC is not bad
	    int bitshift=16-ROW*4;
	    int segctrval=(ctrval>>bitshift & 0xf);
	    if (segctrval==EVT_NR_TYPE[0]) {
	      nrCorrEvtNR[0]++;
	    }
	    else if (segctrval==EVT_NR_TYPE[1]) {
	      nrCorrEvtNR[1]++;
	    }
	    else if (segctrval==EVT_NR_TYPE[2]) {
	      nrCorrEvtNR[2]++;
	    }
	    else if (segctrval==EVT_NR_TYPE[3]) {
	      nrCorrEvtNR[3]++;
	    }
	    else if (segctrval==EVT_NR_TYPE[4]) {
	      nrCorrEvtNR[4]++;
	    }
	  }
	}
      }
      
      int tmpmax = 0;
      EvtNR = -1;
      for (int i=0;i<5;i++) {
	if (nrCorrEvtNR[i]>tmpmax) {
	  tmpmax=nrCorrEvtNR[i];
	  EvtNR=i;
	}
      }

      // get all the 20-bit data-word info
      int nw = 0; // number of words
      int tmppadsOn[NPADZ] = {0};
      p->fillIntArray(tmppadsOn,NPADZ,&nw);

      for (int i = 0; i < NPADZ; i++) {
	padsOn[i] = tmppadsOn[i];
      }

      if ( pc2check==1 ) {
	// PC2 has a channel swap in real data
	// that we should account for
	for (int i = 0; i < NPADZ; i++) {
	  int padcol = 11 - i%12;
	  if ( (padcol == 6) || (padcol == 7) )
	    {
	      // switching 2:11 and 2:13 for PC2 (holes 19 and 44)
	      if (padcol == 6)
		{
		  padsOn[i] = tmppadsOn[i] & 0xddddd; // only take the other channels
		  padsOn[i] += ( tmppadsOn[i-1] & 0x88888 ) >>  0x2;
		  // and add some from the other (shifted)
		}
	      else // padcol == 7
		{
		  padsOn[i] = tmppadsOn[i] & 0x77777; // only take the other channels
		  padsOn[i] += ( tmppadsOn[i+1] & 0x22222 ) << 0x2;
		  // and add some from the other (shifted)
		}
	    }
	}
      }

      nrpads=0;

      short padOn[MAX_PADS];
      // ok, we now have the 20-bit data word info. Let's now get the 
      // individual pad info. We did the PC2 swap above already

      for (int iword=0; iword<NPADZ; iword++) {
	for (int j=0; j<NPADX; j++) { // data is contained in lowest 20 bits (20 = NPADX)
	  k = (NPADZ-iword)*NPADX - j - 1;
	  // PC2 swap doesn't cross TGL boundaries so realk vs k doesn't matter for bad TGL search
	  // so just keep the one index..
	  int ival_unmasked = ( (padsOn[iword] >> j) & 0x1 );
	    	    
	  if ( ival_unmasked && !badchannel[k]) {
	    padK[nrpads] = k;
	    nrpads++;
	    fired_pad[k]++;
	    padOn[k] = k;
	  }
	  else {
	    padOn[k] = -1;
	  }
	}
      }
#ifdef DEBUG 
      cout << "nrpads= " << nrpads << endl;
#endif
      numberOfFiredPads=nrpads;

      nrcells=0;      
      // new, faster, better cell rec.
      // which possible comb. do we have?
      short posskmkbdiff[3];
      short possktkmdiff[3];
      // one is always possible
      posskmkbdiff[0] = NPADX;
      possktkmdiff[0] = NPADX;

      for (realkb=0; realkb<(MAX_PADS-(2*NPADX-1));realkb++) 
	{
	  kb = padOn[realkb];
	  
	  if ( kb>-1 ) // realkb was actually on
	    {
	      cellz = realkb/NPADX;
	      zmod3 = cellz%3;

	      // this could probably be coded more elegantly..
	      switch (zmod3)
		{
		case 0:	    
		  posskmkbdiff[1] = NPADX;
		  possktkmdiff[1] = NPADX+1;
		  posskmkbdiff[2] = NPADX+1;
		  possktkmdiff[2] = NPADX;
		  break;
		  // 0,1 give the same km; 1,2 give the same kt
		case 1:	    
		  posskmkbdiff[1] = NPADX;
		  possktkmdiff[1] = NPADX-1;
		  posskmkbdiff[2] = NPADX+1;
		  possktkmdiff[2] = NPADX-1;
		  break;
		  // 0,1 the same km; 0,2 give the same kt
		case 2:	    
		  posskmkbdiff[1] = NPADX-1;
		  possktkmdiff[1] = NPADX;
		  posskmkbdiff[2] = NPADX-1;
		  possktkmdiff[2] = NPADX+1;
		  break;
		  // 1,2 give the same km; 0,2 give the same kt

		default:
		  cout << "padOnlineRecFEMBasic::processEvent ERROR "
		       << "No such number. How did this happen?\n";
		  break;

		}

	      for (j = 0; j < NPOSS; j++)
		possible[j]=1;

	      for (j = 0; j < NPOSS; j++)
		{
		  kbmodX = realkb%NPADX; 
		  if ( (j==0) || ( (possible[j]) && 
				   // ok, need to check if j>0 comb. still are possible 
				   ( ( (kbmodX!=0) && (kbmodX!=(NPADX-1)) ) || 
				     // fine: not on the edge
				     ( (kbmodX==0) && ((posskmkbdiff[j]-NPADX)>=0) && 
				       // lower edge: can't subtract
				       ((posskmkbdiff[j]+possktkmdiff[j]-2*NPADX)>=0) ) || 
				     ( (kbmodX==(NPADX-1)) && ((posskmkbdiff[j]-NPADX)<=0) && 
				       // higher edge: can't add
				       ((posskmkbdiff[j]+possktkmdiff[j]-2*NPADX)<=0) ) ) ) )
		    {

		      realkm = realkb + posskmkbdiff[j];
		      if (realkm<MAX_PADS)
			{
			  km = padOn[realkm];
			  if ( km>-1 ) // realkm was actually on
			    {
			      realkt = realkm + possktkmdiff[j];
			      if (realkt<MAX_PADS)
				{			  
				  kt = padOn[realkt];
				  if ( kt>-1 ) // realkt was actually on
				    {
				      cellwire = ( (kbmodX + realkm%NPADX + realkt%NPADX)*3 )/3; 
			  			      
				      cellZ[nrcells]=cellz;
				      cellWire[nrcells]=cellwire;
				      nrcells++;
				      
				      fired_cell[cellz*NWIRES+cellwire]++;
				    } 
				  else // kt not on 
				    {
				      if ( ( (zmod3>0) && (j==0) ) ||
					   ( (zmod3==0) && (j==1) ) )
					possible[2]=-1; // skip the last one: it has the same kt
				    } 
				}
			      else // kt out of bounds
				{
				  if ( ( (zmod3>0) && (j==0) ) ||
				       ( (zmod3==0) && (j==1) ) )
				    possible[2]=-1; // skip the last one: it has the same kt
				} 
			    } 
			  else // km not on 
			    {
			      if ( ( (zmod3<2) && (j==0) ) ||
				   ( (zmod3==2) && (j==1) ) ) 
				possible[j+1]=-1; // skip the next one: it has the same km 
			    }		      
			}
		      else // km out of bounds
			{
			  if ( ( (zmod3<2) && (j==0) ) ||
			       ( (zmod3==2) && (j==1) ) ) 
			    possible[j+1]=-1; // skip the next one: it has the same km 
			}		      
		    }
		}
	    }
	}
      
      numberOfFiredCells=nrcells;
#ifdef DEBUG 
      cout << "nrcells= " << nrcells << endl;    
#endif
      // let's also do clustering
      cell2clust();
      
      delete p;      
      
    } // if (p)    	
  else { 
    // ******** extra careful if the packet does not exist... *********
    EvtNR=-2;

    //#ifdef DEBUG 
    // we now drop packets so this is just clutters the output
    //cerr << "Packet " << packetid << " could not be found \n";

    //#endif
    return -1;
  }
  
  return 0;
}
//********************************************************************** 
short padOnlineRecFEMBasic::getNumberFiredPads() 
{ 
  // This is the number of fired pad in one event! 
  // JN 021203
  if (iFlag==1) { 
    return numberOfFiredPads;    
  } 
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getNumberFiredPads ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getNumberFiredPads ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getNumberFiredPads() */
//********************************************************************** 
short padOnlineRecFEMBasic::getFiredPad(short i) 
{ 
  if (iFlag==1) { 
    if ( (i>-1) && (i<numberOfFiredPads) )
      return padK[i];
    else {
      cerr << "padOnlineRecFEMBasic:getFiredPad ERROR Argument out of bounds \n"; 
      return -1;
    }
  } 
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getFiredPad ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getFiredPad ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getFiredPad() */
//********************************************************************** 
short padOnlineRecFEMBasic::getNumberFiredCells() 
{ 
  if (iFlag==1)  
    return numberOfFiredCells;  
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getNumberFiredCells ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getNumberFiredCells ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getNumberFiredCells() */
//********************************************************************** 
short padOnlineRecFEMBasic::getFiredCellZ(short i) 
{ 
  if (iFlag==1) { 
    if ( (i>-1) && (i<numberOfFiredCells) )
      return cellZ[i];
    else {
      cerr << "padOnlineRecFEMBasic:getFiredCellZ ERROR Argument out of bounds \n"; 
      return -1;
    }
  } 
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getFiredCellZ ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getFiredCellZ ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getFiredCellZ() */
//********************************************************************** 
short padOnlineRecFEMBasic::getFiredCellWire(short i) 
{ 
  if (iFlag==1) { 
    if ( (i>-1) && (i<numberOfFiredCells) )
      return cellWire[i];
    else {
      cerr << "padOnlineRecFEMBasic:getFiredCellWire ERROR Argument out of bounds \n"; 
      return -1;
    }
  } 
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getFiredCellWire ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getFiredCellWire ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getFiredCellWire() */
//********************************************************************** 
short padOnlineRecFEMBasic::getFiredCellSectZ(short i) 
{ 
  if (iFlag==1) { 
    if ( (i>-1) && (i<numberOfFiredCells) ) {
      short zval = cellZ[i];
      
      if (side==0) // reverse z 
	zval = (NCELLZ-1) - zval;
      else // add on to this side
	zval += NCELLZ;
      return zval;
    }
    else {
      cerr << "padOnlineRecFEMBasic:getFiredCellSectZ ERROR Argument out of bounds \n"; 
      return -1;
    }
  } 
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getFiredCellSectZ ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getFiredCellSectZ ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getFiredCellZ() */
//********************************************************************** 
short padOnlineRecFEMBasic::getFiredCellSectWire(short i) 
{ 
  if (iFlag==1) { 
    if ( (i>-1) && (i<numberOfFiredCells) ) {
      short wireval = cellWire[i];
      short sectwire = getSectWire(wireval);

      return sectwire;
    }
    else {
      cerr << "padOnlineRecFEMBasic:getFiredCellSectWire ERROR Argument out of bounds \n"; 
      return -1;
    }
  } 
  else if (iFlag==0) {
    cerr << "padOnlineRecFEMBasic:getFiredCellSectWire ERROR The object is ready to take data but has not done so yet, or a reset has been made \n"; 
    cerr << " packetid = " << packetid << endl;
    return -999;
  } 
  else {
    cerr << "padOnlineRecFEMBasic:getFiredCellSectWire ERROR  not initialized. \n";
    return -999;
  }
}  
/* end getFiredCellSectWire() */
//********************************************************************** 
void padOnlineRecFEMBasic::print() 
{ 
  // Print the parameter information 
  if(iFlag==-1) { 
    cerr << "padOnlineRecFEMBasic:print ERROR  not initialized. \n";
  } 
  else {
    cout << "PACKETID= " << packetid << endl;  
    if (iFlag==0) {
      cout << "The object is ready to take data but has not done so yet"
	   << " or a reset has been made \n";
    }
    else {
      cout << "Number of fired pads= " << numberOfFiredPads << endl;  
      cout << "Number of fired cells= " << numberOfFiredCells << endl;  
    }
  }
}  
/* end print() */
//********************************************************************** 
void padOnlineRecFEMBasic::reset() 
{ 
  if ( (packetid>4000) && (packetid<4097) ) {
    iFlag=0;
#ifdef DEBUG 
    cout << "You are ready to take data with packetid " << packetid << endl;
#endif
  }
  else {
    iFlag=-1;
#ifdef DEBUG 
    cout << "You should change your packetid " << packetid 
	 << " before attemtping to rec. data" << endl;
#endif
  }

  numberProcEvents=0;
  
  memset(fired_pad, 0, sizeof(fired_pad));
  memset(fired_cell, 0, sizeof(fired_cell));

  numberOfClusters=0;
  
  int imax = MAX_CLUSTERS;
  if (numberOfClusters < MAX_CLUSTERS)
    {
      imax = numberOfClusters;
    }
  for (int i=0; i<imax; i++) {
    cluster[i].sectz = 0.0;
    cluster[i].sectwire = 0.0;
    cluster[i].size = 0;
  }

  // Reset variables for event adress check
  for (int i=0;i<5;i++) {
    nrCorrEvtNR[i]=0;
  }


}  
/* end reset() */
//********************************************************************** 
int padOnlineRecFEMBasic::getNumberOfTimesPadFired(short k)
{ 
  // Get number of times this pad fired
  // This is accumulated over all events analysed 
  // JN 021203
  if ( (k>=0) && (k<MAX_PADS) ) 
    return fired_pad[k];
  else {
    cerr << "padOnlineRec ERROR getNumberOfTimesPadFired variables out of bounds.." << endl; 
    return -1; 
  } 
} 
/* end getNumberNumberOfTimesPadFired(short k) */
//********************************************************************** 
int padOnlineRecFEMBasic::getNumberOfTimesCellFired(short k)
{ 
  // Get number of times this pad fired
  if ( (k>=0) && (k<MAX_CELLS) ) 
    return fired_cell[k];
  else {
    cerr << "padOnlineRec ERROR getNumberOfTimesCellFired variables out of bounds.." << endl; 
    return -1; 
  } 
} 
/* end getNumberNumberOfTimesCellFired(short k) */
//**********************************************************************
short padOnlineRecFEMBasic::setBadROC(int grow, int gcol, int badtgltype[NTGLS])
{
  short tgl,channel,i,k,padcol,padrow;
  short roctype=0,factor=100;
  if ( (grow<NROWS) && (grow>-1) && (gcol<NROCS) && (gcol>-1) ) {
    // ok, the indices are within the allowed range
    // loop over all channels in this ROC
    for (tgl=0; tgl<NTGLS; tgl++) {
      if (badtgltype[tgl]!=0) { 
	roctype+=badtgltype[tgl]*factor;
	for (channel=0; channel<NCHAN; channel++) { 
	  i=tgl*NCHAN+channel;
	  // the 3 lines below are obvious for people who are familiar with the pad pattern
	  // I apologize to everyone else..
	  padcol=i/4;
	  padrow=i%4;
	  k=gcol*240+padcol*NPADX+grow*4+padrow;
	  badchannel[k]=badtgltype[tgl];
	}
      }
      factor/=10;
    }

    // *** for event address check
    if (badtgltype[0]!=0) {
      badrocEvtNR[gcol*NROWS+grow]=1;
      nrbadrocEvtNR++;
    } 

    if ( (numberBadROC<MAX_BADROC) && (numberBadROC>-1) ) {
      badroc[numberBadROC].grow = grow;
      badroc[numberBadROC].gcol = gcol;
      badroc[numberBadROC].roctype = roctype;
    }
    numberBadROC++;
    return 0;
  } 
  else 
    return -1;
}
//**********************************************************************
short padOnlineRecFEMBasic::setBadCh(int channelid, int badpadtype)
{
  if ( (channelid<MAX_PADS) && (channelid>-1) ) { 
    // ok, the index are within the allowed range
    badchannel[channelid]=badpadtype;
    if ( (numberBadCh<MAX_BADCH) && (numberBadCh>-1) ) {
      badch[numberBadCh].channelid = channelid;
      badch[numberBadCh].padtype = badpadtype;
    }
    numberBadCh++;
    return 0;
  } 
  else 
    return -1;
}
//**********************************************************************
short padOnlineRecFEMBasic::transformPC2K(short k)
{
  // PC2: 4033-4064, 2 channelconnections per ROC were interchanged
  // switching 2:11 and 2:13 for PC2 (holes 19 and 44), from padEvtToRaw.cc
  // Note: The switch is the same from realk to k as from k to realk.. 
	
  short k2;    
  short padcolumn = (k%240)/NPADX; // 0 to 11
  short padrow = k%4; // 0 to 3
  if ( (padcolumn==6) && (padrow==2) ) k2=k+18;
  else if ( (padcolumn==7) && (padrow==0) ) k2=k-18;
  else k2=k;
	
  return k2;
}    
//********************************************************************** 
void padOnlineRecFEMBasic::decodePacketid() 
{ 
  if ( (packetid>4000) && (packetid<4097) ) 
    {
#ifdef DEBUG 
      cout << "Decoding packetid " << packetid << endl;
#endif
      short packetred = packetid-FIRSTPACKET;
      layer = packetred/NFEMSPERLAYER;
      arm = 1 - (packetred%NFEMSPERLAYER)/NFEMSPERPLANE;
      side = (packetred%NFEMSPERPLANE)/NFEMSPERSIDE;
      subsector = packetred%NFEMSPERSIDE; // same as sector for PC1, sector/2 for PC2/3

      hvsectorstart = 0;
      if (layer==0) // PC 1
	hvsectorstart += 4*subsector;
      else  // PC2/3
	{
	  hvsectorstart += 4*(subsector/2);
	  if (side==1) 
	    hvsectorstart += NHVSECTORS/2;
	}  

      modsubsect = subsector%2;
      offset = 0;      
      if (layer>0) {
	if ( ((arm==side) && (modsubsect!=0)) ||
	     ((arm!=side) && (modsubsect==0)) ) offset=1;
    
	if ( (layer==2) && (arm==1) )  // PC3 West has MB facing IP
	  offset=1-offset; // should be the other way around..
      }
    }
  else {
    cerr << "padOnlineRecFEMBasic::decodePacketid ERROR You should change your packetid " << packetid << " before attemtping to rec. data" << endl;
    layer = -1;
    arm = -1;
    side = -1;
    subsector = -1;
    hvsectorstart = -1;
    offset = -1;
    modsubsect = -1;
  }
}  
/* end decodePacketid() */
//********************************************************************** 
int padOnlineRecFEMBasic::OnlineArrayFill(int ip,int *theHVarray)
{
  int hv = hvsectorstart + ip;
  int i,addition,wireval;
  int hvsector = -999;
  // go thru all cells, figure out which hvsector they belong to and add to the array
  for (i = 0; i < numberOfFiredCells; i++)
    {
      wireval = getFiredCellSectWire(i);        
      addition = 0;
      while ( (wireval>0) && (addition<4) ) 
	{
	  wireval -= nwiresPerHVSector[layer*4+addition];
	  addition++; 
	}
      hvsector = hv + addition - 1;

      // got hvsector = -1 during run with insure
      
      if (hvsector >= 0 && hvsector < NPLANES*NHVSECTORS)
	{
	  theHVarray[hvsector]++;
	}
      else
	{
	  return -999;
	}
    }
  
  return hvsector;
}
/* end OnlineArrayFill() */
//********************************************************************** 
int padOnlineRecFEMBasic::OnlineArrayFillRun(int ip,int *theHVarray)
{
  int i,hvsector,wireval;
  int nfired;

  // go thru all cells, figure out which hvsector they belong to and add to the array
  for (i = 0; i < MAX_CELLS; i++)
    {
      nfired = fired_cell[i];
      wireval = i%NWIRES;
      hvsector = getHVSector(ip,wireval);
      
      theHVarray[hvsector]+=nfired;
    }
  
  return hvsector;
}
/* end OnlineArrayFillRun() */
//********************************************************************** 
int padOnlineRecFEMBasic::OnlineArrayFillTGLRun(int ip,int *theTGLSumArray,
						float *theChanSigmaArray)
{
  int tglnumber = ip;
  int grow,gcol,tgl,channel,k,kstart;

  int nfired;
  float mean,var,sigma;

  // go thru all channels, figure out which tgl they belong to and add to the array
  for (grow = 0; grow < NROWS; grow++)
    {
      for (gcol = 0; gcol < NROCS; gcol++)
	{
	  for (tgl = 0; tgl < NTGLS; tgl++)
	    { 
	      nfired = 0;
	      kstart = gcol*240 + (tgl*4)*20 + grow*4; 
	      // sorry, still a few magic numbers here..

	      for (channel = 0; channel < NCHAN; channel++)
		{
		  k = kstart + (channel/4)*20 + channel%4;
		  nfired += fired_pad[k];
		}
      	      theTGLSumArray[tglnumber]=nfired;

	      mean = nfired/(1.0*NCHAN); // sample mean
	      var = 0.0; // sample variance
	      for (channel = 0; channel < NCHAN; channel++)
		{
		  k = kstart + (channel/4)*20 + channel%4;
		  var += (fired_pad[k]-mean)*(fired_pad[k]-mean);
		}
	      var = var/(1.0*NCHAN - 1.0); // not the best estimate, but unbiased
	      sigma = sqrt(var);

	      theChanSigmaArray[tglnumber]=sigma;
	      tglnumber++;
	    }
	}
    }
  
  return tglnumber;
}
/* end OnlineArrayFillTGLRun() */

//********************************************************************** 
int padOnlineRecFEMBasic::cell2clust()
{
  int i, ncells;
  numberOfClusters = 0;

  ncells = numberOfFiredCells;

  if ( ncells<=0 )
    return ncells;

  // algorithm from PadRec by PN
  
  // fill a cell-list from the FEM objects
  list<Cell> cells;
  Cell newCell;

#ifdef DEBUG
  cout << "cell info: i z w \n";
#endif

  for (i=0; i<ncells; i++) 
    {
      newCell.z = cellZ[i];
      newCell.w = cellWire[i];
#ifdef DEBUG 
      cout << i << " " << newCell.z << " " << newCell.w  << endl;
#endif
      cells.push_back(newCell);
    }
  
  // turn the list into clusters
  list<Cell> newCluster;
  short z0, w0, z, w, nincluster;
  int avew, avez;
  
#ifdef DEBUG 
  cout << "Setup completed. ncells = " << ncells << " " << cells.size() << endl;
#endif

  while (cells.size()>0) { 
    
#ifdef DEBUG
    cout << "ncells = " << cells.size() << endl;
#endif

    // Get the first cell from the list, store it and erase it
    CI ci = cells.begin();
    w0 = (*ci).w;
    z0 = (*ci).z;
    newCell.w = w0;
    newCell.z = z0;
    
    newCluster.clear();
    newCluster.push_back(newCell);
    cells.erase(ci);
    
    nincluster = 1; // size of newCluster is 1..
    avez=z0;
    avew=w0;
    
    // Find the neighbors to all members of the new cluster
    CI nci = newCluster.begin();
    
    while (nci != newCluster.end())
      {
	w0 = (*nci).w;
	z0 = (*nci).z;
	ci = cells.begin();
	
	// Find the neighbours to this cell and store them in the same list
	// added a cut so that we don't make too large clusters
	while ( (ci != cells.end()) && (nincluster<MAX_CLUSTERSIZE) )
	  {
	    w = (*ci).w;
	    z = (*ci).z;
	    
	    if (cell_adjacent(z0, w0, z, w))
	      {
		// Add cell to the new cluster
		newCell.w = w;
		newCell.z = z;
		newCluster.push_back(newCell);
		avez += z;
		avew += w;
		nincluster++;
		
		// Remove cell from the list
		cells.erase(ci--);
	      }
	    ci++;
	  }
	nci++;
      }
    
    // newCluster has been formed and grabbed all its neighbours. 
    // Now, it's time for us to store it, if possible
#ifdef DEBUG 
    cout << "cluster info: i sect size z wire\n";
#endif
    if (numberOfClusters<MAX_CLUSTERS) {
      cluster[numberOfClusters].size = nincluster; // should be eq to newCluster.size()
      if (nincluster!=0) {
	cluster[numberOfClusters].sectz = avez/(1.0*nincluster);
	cluster[numberOfClusters].sectwire = avew/(1.0*nincluster);
#ifdef DEBUG 
	cout << numberOfClusters << " "
	     << cluster[numberOfClusters].size << " "
	     << cluster[numberOfClusters].sectz << " "
	     << cluster[numberOfClusters].sectwire << endl;
#endif
      }
    }
    numberOfClusters++;
  }

#ifdef DEBUG
  cout << "numberOfClusters = " << numberOfClusters << endl;
#endif
  return 0;
}
//********************************************************************** 
bool padOnlineRecFEMBasic::cell_adjacent(short z1, short wire1, short z2, short wire2)
{
  // returns true if 2 cells are adjacent.. 
  short zdiff, wirediff;
  
  zdiff=abs(z1-z2); 
  if (zdiff > 1) return false;
  wirediff=abs(wire1-wire2); 
  if (wirediff>1) return false;
  //  if (wirediff<0) wirediff=-wirediff;
  //  if (zdiff<0) zdiff=-zdiff;
  if ((zdiff<=1) && (wirediff<=1)) {
    if ( (diagonalAdj==-1) && (zdiff==1) && (wirediff==1) ) 
      return false; // diagonally adjacent cells in 
    // the same cluster is not allowed
    else
      return true;
  }
  else
    return false;   
}
//********************************************************************** 
int padOnlineRecFEMBasic::getHVSector(int ip, int wire)
{
  short wireval = wire;
  short sectwire = getSectWire(wireval);

  int addition = 0;
  while ( (sectwire>=0) && (addition<4) ) {
    sectwire -= nwiresPerHVSector[layer*4+addition];
    addition++; 
  }
  int hvsector = ip + hvsectorstart + addition - 1;
  
  return hvsector;
}
/* end  */
//********************************************************************** 
int padOnlineRecFEMBasic::getHVSector(int ip, float wire)
{
  wire += 0.49999; // not 0.5
  short wireval = (short) wire;
  short sectwire = getSectWire(wireval);

  int addition = 0;
  while ( (sectwire>=0) && (addition<4) ) {
    sectwire -= nwiresPerHVSector[layer*4+addition];
    addition++; 
  }
  int hvsector = ip + hvsectorstart + addition - 1;
  
  return hvsector;
}
//********************************************************************** 
int padOnlineRecFEMBasic::getSectWire(int wire)
{
  short wireval = wire;
      
  if ( (layer==0) && (side==0) ) // reverse wire 
    wireval = (NWIRES-1) - wireval;
      
  if (layer>0) {
    wireval+=offset*NWIRES;
    if (side==0) // reverse wire 
      wireval = (2*NWIRES-1) - wireval;
  }

  return wireval;
}

