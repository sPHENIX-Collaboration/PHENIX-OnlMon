#include <TH1.h>
#include <TH2.h>
#include <msg_profile.h>
#include <Event.h>
#include <muiMonGlobals.h>
#include "OnlMonServer.h"
#include <gl1Obj.h>

#include <iostream>
#include <sstream>

using namespace std;

gl1Obj::gl1Obj(OnlMon *myparent): obj_hist(myparent)
{
  TrigMask = new TH1F("MUI_TrigMask", "MUI TRIGMASK", MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5);
  ScaledTrig = new TH1F("MUI_ScaledTrig", "GL1 SCALEDTRIG", MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5);
  LiveTrig = new TH1F("MUI_LiveTrig", "GL1 LIVETRIG", MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5);
  ScaledTrig_TotalHits = new TH2F("MUI_ScaledTrig_TotalHits", "GL1 SCALEDTRIG vs MuID total hits",
                                  MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5, 100, 0, MAX_TOTAL_HITS);
  CrossCtr = new TH1F("MUI_CrossCtr", "GL1 Bunch Cross Counter", MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  ScaledTrig_CrossCtr = new TH2F("MUI_ScaledTrig_CrossCtr", "GL1 SCALEDTRIG vs Cross Counter",
                                 MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5, MAX_CROSSCTR, 0, MAX_CROSSCTR - 0.5);
  CrossCtr_TotalHits = new TH2F("MUI_CrossCtr_TotalHits", "GL1 Cross Counter vs MuID Total Hits",
                                MAX_CROSSCTR, 0, MAX_CROSSCTR - 0.5, 100, 0, MAX_TOTAL_HITS);
  ReducedBits0 = new TH1F("MUI_ReducedBits0", "GL1 ReducedBits 0", MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5);
  ReducedBits1 = new TH1F("MUI_ReducedBits1", "GL1 ReducedBits 1", MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5);
  ReducedBits0_CrossCtr = new TH2F("MUI_ReducedBits0_CrossCtr", "GL1 ReducedBits0 vs Cross Counter",
                                   MAX_SCALEDTRIG_BIT , -0.5, MAX_SCALEDTRIG_BIT - 0.5, MAX_CROSSCTR, 0, MAX_CROSSCTR - 0.5);

  // register some of the basic histos 
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(parent,TrigMask);
  Onlmonserver->registerHisto(parent,ScaledTrig);
  Onlmonserver->registerHisto(parent,LiveTrig);

}

gl1Obj::~gl1Obj()
{
  delete ScaledTrig_TotalHits;
  delete CrossCtr;
  delete ScaledTrig_CrossCtr;
  delete CrossCtr_TotalHits;
  delete ReducedBits0;
  delete ReducedBits1;
  delete ReducedBits0_CrossCtr;
}

long gl1Obj::getScaledtrig()
{
  return scaledtrig;
}

long gl1Obj::getLivetrig()
{
  return livetrig;
}

long gl1Obj::getCrossctr()
{
  return crossctr;
}

long gl1Obj::getReducedbits0()
{
  return reducedbits0;
}

long gl1Obj::getReducedbits1()
{
  return reducedbits1;
}

int
gl1Obj::event( Event* e)
{
  muiMonGlobals* globals = muiMonGlobals::Instance();

  // Get scaledtrig from GL1 packet
  Packet* packet_gl1 = e->getPacket( 14001 );

  ostringstream msg;
  if ( packet_gl1 )
    {
      if ( globals->verbose >= 5 )
        {
          printf("GL1 Packet found:\n");
        }
      scaledtrig = packet_gl1->iValue(0, "SCALEDTRIG");
      if ( globals->verbose >= 5 )
        {
          printf("GL1 SCALEDTRIG = 0x%08lx\n", scaledtrig);
        }
      livetrig = packet_gl1->iValue(0, "LIVETRIG");
      if ( globals->verbose >= 5 )
        {
          printf("GL1 LIVETRIG = 0x%08lx\n", livetrig);
        }
      crossctr = packet_gl1->iValue(0, "CROSSCTR");
      if ( globals->verbose >= 5 )
        {
          printf("GL1 cross counter = 0x%08lx\n", crossctr);
        }
      reducedbits0 = packet_gl1->iValue(0, "RBITS0");
      if ( globals->verbose >= 5 )
        {
          printf("GL1 Reduced Bits0 0x%08lx\n", reducedbits0);
        }
      //cout << " Event = " << eventNum
      //   << " GL1 reduced bits0 = " << hex << reducedbits0 << dec
      //   << " GL1 LIVETRIG = " << hex << livetrig << dec
      //   << endl;
      //}

      reducedbits1 = packet_gl1->iValue(0, "RBITS1");
      if ( globals->verbose >= 5 )
        {
          printf("GL1 Reduced Bits1 0x%08lx\n", reducedbits1);
        }
      delete packet_gl1;
    }
  else
    {
      scaledtrig = 0;
      livetrig = 0;
      crossctr = -1;
      reducedbits0 = 0;
      reducedbits1 = 0;
      if ( globals->verbose >= 0 )
        {
          printf("GL1 packet not found\n");
        }
    }
  return 0;
}


int gl1Obj::fill(hit_vector rawHits)
{
  for ( int i = 0; i < MAX_SCALEDTRIG_BIT; i++ )
    {
      if ( scaledtrig & ( 1 << i ) )
        {
          ScaledTrig->Fill( i );
          ScaledTrig_TotalHits->Fill( i, rawHits.size() );
          if ( (crossctr >= 0) && (crossctr < MAX_CROSSCTR) )
            {
              ScaledTrig_CrossCtr->Fill( i, crossctr );
            }
        }
      if ( livetrig & ( 1 << i ) )
        {
          LiveTrig->Fill( i );
        }
      if ( reducedbits0 & ( 1 << i ) )
        {
          ReducedBits0->Fill( i );
          if ( (crossctr >= 0) && (crossctr < MAX_CROSSCTR) )
            {
              ReducedBits0_CrossCtr->Fill( i, crossctr );
            }
        }
      if ( reducedbits1 & ( 1 << i ) )
        {
          ReducedBits1->Fill( i );
        }
    }

  //  for ( int i = 0; i < MAX_SCALEDTRIG_BIT; i++ )
  //  {}

  if ( (crossctr >= 0) && (crossctr < MAX_CROSSCTR) )
    {
      CrossCtr->Fill( crossctr );
      CrossCtr_TotalHits->Fill( crossctr, rawHits.size());
    }
  else
    {
      //cout << " GL1 cross counter is out of range " << crossctr << endl;
      return -1;
    }
  return 0;
}

int gl1Obj::setTrigMask(unsigned int trigmask)
{
  printf("gl1Obj::setTrigMask %08x\n",trigmask);
  TrigMask->Reset();
  if (trigmask == 0)
    {
      return 0; // nothing to set
    }

  // just on or off for each bin/bit
  for ( int i = 0; i < MAX_SCALEDTRIG_BIT; i++ )
    {
      if ( trigmask & ( 1 << i ) )
        {
          TrigMask->Fill( i, 1. );
        }
      else
        {
          TrigMask->Fill( i, 0. );
        }
    }
  return 0;
}

void gl1Obj::clear()
{
  ScaledTrig->Reset();
  LiveTrig->Reset();
  ScaledTrig_TotalHits->Reset();
  ReducedBits0->Reset();
  ReducedBits1->Reset();
  ReducedBits0_CrossCtr->Reset();
  CrossCtr->Reset();
  ScaledTrig_CrossCtr->Reset();
  CrossCtr_TotalHits->Reset();
}

