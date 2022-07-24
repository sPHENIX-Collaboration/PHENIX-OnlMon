#ifndef __PADONLINERECFEMBASIC_H 
#define __PADONLINERECFEMBASIC_H 
//---------------------------------------------------------------------------  
//                                                                 
// Created by: David Silvermyr 
// Modified for RUN3 by Joakim Nystrand and Henrik Tydesjo
//                                                                 
// Description: Header for PadOnlineRecFEMBasic class 
//                                                                 
//---------------------------------------------------------------------------- 

#include <list>

#include "PadMonPar.h"


// from PadRec: PN
// Cell type
struct Cell { short w, z; };

// STL cell list iterator
typedef std::list<Cell>::iterator CI;

class Event;

/** 
   This is an OnlineRec PAD FEM class. 
   @memo PAD OnlineRec FEM Class 
*/ 
class padOnlineRecFEMBasic {  
   
public:
   
  // Constructor 
  padOnlineRecFEMBasic(short ipacketid);  
   
  // public member functions 

  // the main routine, where everything happens.. 
  virtual int processEvent(Event * evt);

  // takes all cells and reconstructs clusters
  int cell2clust();
  
  // some simple feedback routines 
  short getNumberFiredPads(); 
  short getFiredPad(short i);

  // limits
  short getMaxNumberClusters() const { return MAX_CLUSTERS; } 
  short getMaxNumberCellsInCluster() const { return MAX_CLUSTERSIZE; } 
  
  // cell and cluster info
  short getNumberFiredCells(); 
  // local coord
  short getFiredCellZ(short i);
  short getFiredCellWire(short i);
  // also local coord but on sector instead of FEM level
  short getFiredCellSectZ(short i);
  short getFiredCellSectWire(short i);

  // a routine to print information about our FEM 
  void print(); 
  // a reset routine for all counters
  virtual void reset();

  // info about how many events we analyzed 
  int getNumberProcEvents() { return numberProcEvents; } 

  int getNumberOfTimesPadFired(short k);
  int getNumberOfTimesCellFired(short k);

  // set, get bad ROC and bad channel routines.
  // perhaps only set is needed in this basic version
  // but I thought it was nice to have it all together
  short setBadROC(int grow, int gcol, int badtgltype[NTGLS]);
  short setBadCh(int channelid, int badpadtype); 

  short getNumberBadCh() { return numberBadCh; } 
  short getChannelidBadCh(short i)
    { return ( ((i>=0) && (i<numberBadCh)) ? badch[i].channelid : -1); }
  short getPadtypeBadCh(short i) 
    { return ( ((i>=0) && (i<numberBadCh)) ? badch[i].padtype : -1); }

  short getNumberBadROC() { return numberBadROC; } 
  short getGrowBadROC(short i) 
    { return ( ((i>=0) && (i<numberBadROC)) ? badroc[i].grow : -1); }
  short getGcolBadROC(short i) 
    { return ( ((i>=0) && (i<numberBadROC)) ? badroc[i].gcol : -1); }
  short getROCtypeBadROC(short i) 
    { return ( ((i>=0) && (i<numberBadROC)) ? badroc[i].roctype : -1); }

  // screw up with the lines on PC2: a couple of channels were interchanged
  short transformPC2K(short k);

  // calc. layer,arm,side, subsector once and for all
  void decodePacketid();

  // the object must be able to tell us who he is
  short getPacketid() const { return packetid; }

  // online interfaces
  // instructions from Chris P.
  // ip is the pointer to where we should start fill stuff into theHVarray
  // (it tells us where our layer starts)
  // called once per event
  int OnlineArrayFill(int ip,int *theHVarray);
  // called once per run (i.e if you don't care about the evt-by-evt progress)
  int OnlineArrayFillRun(int ip,int *theHVarray);

  // anc. array with TGL info: for ROC monitoring/calibration
  // done once per run
  int OnlineArrayFillTGLRun(int ip,int *theTGLSumArray,
			    float *theChanSigmaArray);
  
  // reconstruction feedback 
  short getNumberClusters() const { return numberOfClusters; }  
  short getClusterSize(short i) const 
    { return ( ((i>=0) && (i<numberOfClusters)) ? cluster[i].size : -1); }
  float getClusterSectZ(short i) const
    { return ( ((i>=0) && (i<numberOfClusters)) ? cluster[i].sectz : -1); }
  float getClusterSectWire(short i) const
    { return ( ((i>=0) && (i<numberOfClusters)) ? cluster[i].sectwire : -1); }
  
  bool cell_adjacent(short z1, short wire1, short z2, short wire2);
  
  // help routine for clustering
  void doNotAllowDiagonalAdjCells() { diagonalAdj=-1; }
  void allowDiagonalAdjCells() { diagonalAdj=1; }

  // translation routine
  int getHVSector(int ip, int wire);
  int getHVSector(int ip, float wire);
  
  int getSectWire(int wire);

  // *** methods for event address check
  int getEvtNR(void){return EvtNR;}
  int getnrCorrEvtNR(int for_evtnr){return nrCorrEvtNR[for_evtnr];}
  int getnrbadrocEvtNR(void){return nrbadrocEvtNR;}
  void setnrbadrocEvtNR(int value){nrbadrocEvtNR=value;}

 protected: 
 
  // our FEM must know who he is.. 
  short packetid;
  // derived from the packetid
  short layer,arm,side,subsector,hvsectorstart;
  short modsubsect,offset;

  // *** variables for the event address check
  // most occuring (possible) evtnr for this packet
  int EvtNR;
  // the nr of occurances of evtnrs
  int nrCorrEvtNR[5];
  // badroc status of all rocs (1=bad,0=good)
  int badrocEvtNR[MAX_ROCS];
  // nr of bad rocs
  int nrbadrocEvtNR;

  // necessary to transform indices?
  short pc2check;

  // some raw numbers: very safe to look at (hard to get wrong) 
  short numberOfFiredPads;
  short numberOfFiredCells;

  short padK[MAX_PADS]; // this event
  short cellZ[MAX_CELLS];
  short cellWire[MAX_CELLS];

  // Status of the FEM (-1: not ready to take data, 0: ready 1: has read) 
  short iFlag; 
 
  // counter for the number of events we processed
  int numberProcEvents;

  unsigned char fired_pad[MAX_PADS];
  unsigned char fired_cell[MAX_CELLS];

  // info on bad channels
  short badchannel[MAX_PADS];

  short numberBadCh;
  typedef struct  
  { 
    short packetid; 
    short channelid; 
    short padtype; 
  } ch; 
  ch badch[MAX_BADCH];  
   
  short numberBadROC; 
  typedef struct 
  { 
    short packetid; 
    short grow; 
    short gcol; 
    short roctype; 
  } roc; 
  roc badroc[MAX_BADROC];

  short numberOfClusters;
  typedef struct 
  { 
    // where?
    float sectz; 
    float sectwire; 
    // how big?
    short size; 
  } clust; 
  clust cluster[MAX_CLUSTERS];

  // to select whether we allow diagonal adjacent cells or not 
  short diagonalAdj;

  // for alt. unpacking
  // 20-bit word info
  int padsOn[NPADZ];
};  
 
#endif 
