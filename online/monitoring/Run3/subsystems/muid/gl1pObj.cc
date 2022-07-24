/* gl1p.cc
   create, fill and clear  GL1P histogram
   read GL1P packet
   Author Hiroki Sato(satohiro@bnl.gov)
*/

#include "TH1.h"
#include "TH2.h"
#include "muiMonGlobals.h"
#include "gl1pObj.h"
#include "OnlMonServer.h"

#include "Event.h"
#include "msg_profile.h"
#include "msg_control.h"

#include <iostream>
#include <sstream>

using namespace std;

gl1pObj::gl1pObj()
{
  GL1P_CrossCtr[0] = new TH1F("MUI_GL1P_CrossCtr_A", "GL1P Bunch Cross Counter A",
                              MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  GL1P_CrossCtr[1] = new TH1F("MUI_GL1P_CrossCtr_B", "GL1P Bunch Cross Counter B",
                              MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  GL1P_CrossCtr[2] = new TH1F("MUI_GL1P_CrossCtr_C", "GL1P Bunch Cross Counter C",
                              MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  GL1P_CrossCtr[3] = new TH1F("MUI_GL1P_CrossCtr_D", "GL1P Bunch Cross Counter D",
                              MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);

  GL1_GL1P[0] = new TH2F("MUI_GL1_GL1P_A", "GL1 vs GL1P_A Bunch Cross Counter",
                         MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5, MAX_CROSSCTR, -0.5, MAX_CROSSCTR);
  GL1_GL1P[1] = new TH2F("MUI_GL1_GL1P_B", "GL1 vs GL1P_B Bunch Cross Counter",
                         MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5, MAX_CROSSCTR, -0.5, MAX_CROSSCTR);
  GL1_GL1P[2] = new TH2F("MUI_GL1_GL1P_C", "GL1 vs GL1P_C Bunch Cross Counter",
                         MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5, MAX_CROSSCTR, -0.5, MAX_CROSSCTR);
  GL1_GL1P[3] = new TH2F("MUI_GL1_GL1P_D", "GL1 vs GL1P_D Bunch Cross Counter",
                         MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5, MAX_CROSSCTR, -0.5, MAX_CROSSCTR);

  GL1P_Scaler[0] = new TH1F("MUI_GL1P_Scaler_A", "GL1P Scaler A", MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  GL1P_Scaler[1] = new TH1F("MUI_GL1P_Scaler_B", "GL1P Scaler B", MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  GL1P_Scaler[2] = new TH1F("MUI_GL1P_Scaler_C", "GL1P Scaler C", MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);
  GL1P_Scaler[3] = new TH1F("MUI_GL1P_Scaler_D", "GL1P Scaler D", MAX_CROSSCTR, -0.5, MAX_CROSSCTR - 0.5);

}

gl1pObj::~gl1pObj()
{
  for ( int i = 0; i < MAX_GL1P_SCALERS; i++ )
    {
      delete GL1P_Scaler[i];
      delete GL1P_CrossCtr[i];
      delete GL1_GL1P[i];
    }
}

int gl1pObj::event( Event* e , const long crossctr)
{
  muiMonGlobals* globals = muiMonGlobals::Instance();
  long GL1P_crossctr = -1;
  long GL1P_scaler = -1;

  // Get scaledtrig from GL1P packet
  Packet* packet_gl1p = e->getPacket( 14008 );

  ostringstream msg;
  if ( packet_gl1p )
    {
      if ( globals->verbose > 0 )
        {
          printf(" Event = %ld GL1 CrossCtr = %ld\n", globals->eventNumAll, crossctr);
        }
      for ( int i = 0; i < MAX_GL1P_SCALERS; i++ )
        {
          GL1P_crossctr = -1;
          GL1P_scaler = -1;
          GL1P_crossctr = packet_gl1p->iValue(i, "CLOCK");
          GL1P_scaler = packet_gl1p->iValue(i, "SCALER");
          if ( globals->verbose > 0 )
            {
              printf(" GL1P # %d CROSSCTR = %ld SCALER = %ld", i, GL1P_crossctr, GL1P_scaler);
            }

          if ( (GL1P_crossctr >= 0) && (GL1P_scaler >= 0) )
            {
              if ( (crossctr >= 0) && (crossctr < MAX_CROSSCTR) )
                {
                  GL1_GL1P[i]->Fill( crossctr, GL1P_crossctr);
                }
              //if( GL1P_crossctr[i] == crossctr ) {
              //check if GL1P and GL1 has same cross counter
              //for( long count = 0; count<GL1P_scaler; count++ ) {
              GL1P_Scaler[i]->Fill( GL1P_crossctr, GL1P_scaler);
              //}
              GL1P_CrossCtr[i]->Fill( GL1P_crossctr);

              //}
              //else {
              //if( globals->verbose >= 0 ) {
              //  cout << " GL1 crossctr = " << crossctr
              //       << " and GL1P # " << i
              //       << " crossctr = " << GL1P_crossctr[i]
              //      << " mismatch " << endl;
              //}
              //}
            }
          else
            {
              if ( globals->verbose >= 0 )
                {
                  printf("GL1P # %d cross counter is out of range %ld\n", i, crossctr);
                }
              //return -1;
            }

        }
      delete packet_gl1p;
    }
  else
    {
      if ( globals->verbose > 0 )
        {
          printf("GL1P packet not found\n");
        }
    }
  return 0;
}

void gl1pObj::clear()
{
  for ( int i = 0; i < MAX_GL1P_SCALERS; i++ )
    {
      GL1P_Scaler[i]->Reset();
      GL1P_CrossCtr[i]->Reset();
      GL1_GL1P[i]->Reset();
    }
}

