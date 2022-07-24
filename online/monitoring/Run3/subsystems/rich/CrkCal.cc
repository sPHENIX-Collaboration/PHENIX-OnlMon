#include "CrkCal.h"

#include <PdbRichADC.hh>
#include <PdbRichPar.hh>

#include <iostream>

using namespace std;

template<>
CrkCal<PdbRichADC>::CrkCal(const char *dummy)
{
  d_status = 1;
  d_name      = "ADC";
  d_classname = "PdbRichADCBank";
  d_calibname = "calib.rich.adc";
  d_bankID.setInternalValue(1);
}

template<>
void
CrkCal<PdbRichADC>::write_header(ostream& os)
{
  os << "# status PMT Ped PedW PE PEW Nped N1 N2 N3 chisqr" << endl;
}

template<>
CrkCal<PdbRichPar>::CrkCal(const char *parname)
{
  d_status = 1;
  d_classname = "PdbRichParBank";
  d_calibname = "calib.rich.par";

  if( string(parname) == "T0")
    {
      d_name   = "T0";
      d_bankID.setInternalValue(1);
    }
  else if (string(parname) == "TAC_GAIN")
    {
      d_name   = "TAC_GAIN";
      d_bankID.setInternalValue(2);
    }
  else if (string(parname) == "SLEW")
    {
      d_name   = "SLEW";
      d_bankID.setInternalValue(3);
    }
  else
    {
      cout << "UNKNOWN parameter name" << endl;
      d_status = -1;
    }
}

template<>
void
CrkCal<PdbRichPar>::write_header(ostream& os)
{
    os << "# status PMT " << d_name << endl;
}
