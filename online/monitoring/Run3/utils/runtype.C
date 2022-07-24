#include "RunDBodbc.h"

#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
  RunDBodbc *rundb = new RunDBodbc();
  //  rundb->Verbosity(1);
  char *badchar = 0;
  if (argc != 2)
    {
      cout << "Need Run number as argument" << endl;
      exit (-1);
    }
  int runno = strtol(argv[1], &badchar, 10);
  if (*badchar != '\0')
    {
      cout << "invalid Run number: " << argv[1] << endl;
      cout << "leftover characters: " << *badchar << endl;
      exit(-1);
    }
  string RType = rundb->RunType(runno);
  cout << "Run number " << runno << " is of type " << RType << endl;
  int iret = -2;
  if (RType == "PHYSICS")
    {
      iret = 0;
    }
  else if (RType == "CALIBRATION")
    {
      iret = 1;
    }
  else if (RType == "JUNK")
    {
      iret = 2;
    }
  else if (RType == "PREJECTED")
    {
      iret = 3;
    }
  else if (RType == "ZEROFIELD")
    {
      iret = 4;
    }
  else if (RType == "VERNIERSCAN")
    {
      iret = 5;
    }
  else if (RType == "LOCALPOLARIMETER")
    {
      iret = 6;
    }
  else if (RType == "PEDESTAL")
    {
      iret = 7;
    }
  else if (RType == "CREJECTED")
    {
      iret = 8;
    }
  else if (RType == "PEDREJECTED")
    {
      iret = 9;
    }
  else if (RType == "UNKNOWN")
    {
      iret = -1;
    }
  else
    {
      cout << "Unknown type: " << RType << endl;
    }
  delete rundb;
  exit(iret);
}
