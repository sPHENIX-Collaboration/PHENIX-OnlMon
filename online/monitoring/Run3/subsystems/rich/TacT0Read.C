#include <TacT0Read.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>


using namespace std;

int TacT0Read::gett0 (string tact0file, float tact0[]){
  int return_state = 0;
  int num;
  ostringstream fname;
  const char *pardir = getenv("ONLMON_MAIN");

  ifstream tacfp;
  ifstream tacfp2;

  tacfp.open(tact0file.c_str());

  if(tacfp.fail()){
    if ( (getenv("ONLMON_MAIN") != NULL))
      {
	fname << pardir << "/share/rich_tac_t0_constants.asc";
	tacfp2.open(fname.str().c_str());

	if(tacfp2.fail())
	  {
	    for (int iloop = 0;iloop < 5120;iloop++)
	      {
		if(iloop==0){
		  cout<<"tac T0 file doesn't exist "<<endl;
		}
		tact0[iloop]=0.0;
	      }
	    return_state +=2;
	  }else{
	    for (int i = 0;i < 5120;i++)
	      {
		tacfp2>>num>>tact0[i];
		//cout<<num<<" ----- "<<tact0[i]<<endl;
	      }
	    tacfp2.close();
	  }
      }
  }
  else
    {
      for (int i = 0;i < 5120;i++)
        {
          tacfp>>num>>tact0[i];
	  //cout<<num<<" "<<tact0[i]<<endl;
        }
      tacfp.close();
    }
  return return_state;
}

