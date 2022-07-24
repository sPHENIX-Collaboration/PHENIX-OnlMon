#ifndef __MUIMAP_H__
#define __MUIMAP_H__

#include "mui_mon_const.h"
#include "TMuiChannelId.hh"

class OnlMon;

///Hardware to physical location map class.
class muiMap
  {
  public:

    ///Default constructor.
    muiMap(const OnlMon *parent);

    ///Default destructor.
    virtual ~muiMap();

    ///Use the objectivity database to fill the map.
    int fillFromDB();

    ///Use mui-fem-config.dat text file to fill the map.
    int fillFromFile();

    /**Set the hardware address you want mapped. 
       Might be nice if this retuned a struct with arm, panel... */
    void setHardwareAddress(const short fem, const short roc, const short word, const short bit);

    ///Get arm associated with hardware address.
    short getArm() const {return Arm;}

    ///Get panel associated with hardware address.
    short getPanel() const {return Panel;}

    ///Get gap (layer) associated with hardware address.
    short getGap() const {return Gap;}

    ///Get orientation associated with hardware address.
    short getOrient() const {return Orient;}

    ///Get TwoPack number associated with hardware address.
    short getTwoPack() const {return TwoPack;}

  private:
    const OnlMon *mymon;
    int channel_map[MAX_CHANNEL];
    short Fem;
    short Roc;
    short Word;
    short Bit;
    short Arm;
    short Panel;
    short Gap;
    short Orient;
    short TwoPack;
    TMuiChannelId* muiChannel;
  };


#endif /* __MUIMAP_H__ */
