#include "MuiCommon.hh"
#include "TMuiChannelId.hh" 
//#include "mui_mon.h"

/*
 
 TMuiChannelId::kPlanesPerArm=5;
 TMuiChannelId::kPanelsPerPlane=6;
//326 = 4*59+2*45 > 4*64+2*26 (horizontal > vertical).
 TMuiChannelId::kTwoPacksPerPlaneMax=326;
 TMuiChannelId::kTwoPacksPerPanelMax=64;
 TMuiChannelId::kArmsTotal=2;
 TMuiChannelId::kOrientations=2;
 TMuiChannelId::kPlanesTotal
=TMuiChannelId::kArmsTotal*TMuiChannelId::kPlanesPerArm;
 TMuiChannelId::kPanelsTotal
=TMuiChannelId::kPlanesTotal*TMuiChannelId::kPanelsPerPlane;
 TMuiChannelId::kPanelsXYTotal
=TMuiChannelId::kPanelsTotal*TMuiChannelId::kOrientations;
 TMuiChannelId::kTwoPacksMaxTotal
=TMuiChannelId::kPanelsXYTotal*TMuiChannelId::kTwoPacksPerPanelMax;
 
// Assume a maximum occupancy of 100% for now.
 TMuiChannelId::kHitsPerPanelMax = TMuiChannelId::kTwoPacksPerPanelMax;
 TMuiChannelId::kHitsMaxTotal = TMuiChannelId::kTwoPacksMaxTotal;
 
// Functions for hash lookup based on TMuiChannelId objects.
// Implement these functions outside of the TMuiChannelId class for
// flexibility.
// (See Taligent guide, "Portable Hash")
 
//: Returns the position of the given TMuiChannelId object in the
//: PanelGeo hash table.
 
*/

//size_t PanelHash(const TMuiChannelId& ident)

using namespace std;

short PanelHash(const TMuiChannelId& ident)
{
  return ( (ident.Arm()*TMuiChannelId::kPlanesPerArm + ident.Plane())
           *TMuiChannelId::kPanelsPerPlane + ident.Panel() )
         % TMuiChannelId::kPanelsTotal;
}

//: Returns the position of the given TMuiChannelId object in the
//: TwoPackGeo hash table.
//size_t TwoPackHash(const TMuiChannelId& ident)
short TwoPackHash(const TMuiChannelId& ident)
{
  return ( ( ( (ident.Arm()
                *TMuiChannelId::kPlanesPerArm + ident.Plane() )
               *TMuiChannelId::kPanelsPerPlane + ident.Panel() )
             *TMuiChannelId::kOrientations + ident.Orient() )
           *TMuiChannelId::kTwoPacksPerPanelMax + ident.TwoPack() )
         % TMuiChannelId::kTwoPacksMaxTotal;
}

// Output to a stream.
ostream& operator << (ostream& s, const TMuiChannelId& ident)
{
  if (ident.TwoPack() >= 0)
    {
      if (ident.Orient() == kHORIZ)
        {
          return s << "Arm " << ident.Arm() << " Plane " << ident.Plane()
                 << " Panel " << ident.Panel()
                 << " Two-pack " << ident.TwoPack() << "H";
        }
      else
        {
          return s << "Arm " << ident.Arm() << " Plane " << ident.Plane()
                 << " Panel " << ident.Panel()
                 << " Two-pack " << ident.TwoPack() << "V";
        }

    }
  else
    {
      return s << "Arm " << ident.Arm() << " Plane " << ident.Plane()
             << " Panel " << ident.Panel();

    }

}

int TMuiChannelId::Set( int channel_hash )
{
  int tmp;
  int kTwoPacksPerArm = kTwoPacksMaxTotal / kArmsTotal; // 3840
  int kTwoPacksPerPlane = kTwoPacksPerArm / kPlanesPerArm; // 768
  int kTwoPacksPerPanel = kTwoPacksPerPlane / kPanelsPerPlane; // 128;

  if ( channel_hash > kTwoPacksMaxTotal || channel_hash < 0)
    {
      //    if( verbose > 0 ) {
      // cout << " TMuiChannelId: channel hash is out of range" << endl;
      //    }
      return -1;
    }
  else
    {
      fArm = short( channel_hash / kTwoPacksPerArm );
      tmp = channel_hash - fArm * kTwoPacksPerArm;
      fPlane = short( tmp / kTwoPacksPerPlane );
      tmp = tmp - fPlane * kTwoPacksPerPlane;
      fPanel = short( tmp / kTwoPacksPerPanel );
      tmp = tmp - fPanel * kTwoPacksPerPanel;
      fOrient = EOrient_t( tmp / kTwoPacksPerPanelMax );
      tmp = tmp - fOrient * kTwoPacksPerPanelMax;
      fTwoPack = short( tmp );
      //cout << " hash = " << channel_hash
      // << " Arm = " << fArm
      // << " Plane = " << fPlane
      // << " Panel = " << fPanel
      // << " Orient = " << fOrient
      // << " TwoPack = " << fTwoPack
      // << endl;

    }
  return 0;
}
