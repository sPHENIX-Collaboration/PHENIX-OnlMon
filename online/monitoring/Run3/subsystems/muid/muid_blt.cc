/*
 * File: muid_blt.cc
 *
 * Author: Nobuyuki Kamihara(kamihara@bnl.gov)
 *
 * Date:   2003/03/19 22:10 
 *
 * Discription: This program simulate Quad MLU's and Decision MLU 
 *              and caluculate the 1S, 2D 1D1S, quad-1S, and masked
 *              quad-1S efficiency. 
 *              If you want to use this program, plese check "if flag"
 *              is "ONLINE". 
 * 
 * Update History:
 * - 2003/04/04 23:30 by Nobuyuki Kamihar(kamihara@bnl.gov)
 *   I added "get_eff" function which return efficiency 
 *   and statistics error. 
 *
 * - 2003/04/10 16:30 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I added "get_activated_eff" function which return the numerator 
 *   and the deniminator for activate( mask to 1).
 *   We should use the masked and activated efficiency to get 
 *   broken bit information.
 *
 * - 2003/04/11 09:50 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I changed the matrix name from Deep to Shallow for ONLINE code. 
 *
 * - 2003/04/15 00:20 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I changed the way to get the bit mask pattern by Trigger Helper. 
 *   This prosecc is done in constructor. Herewith, I modified the 
 *   constructor with a integer argument. Someone have to use this 
 *   new constructor. 
 *
 * - 2003/04/18 14:30 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I add setBitMask function which set bit mask configuration
 *   by TriggerHelper. 
 *
 * - 2003/04/26 19:20 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I changed the trigger configuration of minimam bias from BBCLL1<=1  
 *   to BBCLL1&(ZDCN|ZDCS). I added the laser event rejection at set_all
 *   function.   
 *
 * - 2003/05/02 23:50 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I changed the minimum bais form BBCLL1&(ZDCN|ZDCS) to the logical OR 
 *   of BBCLL1>=1, BBCLL1&(ZDCN|ZDCS), ERT_2x2&BBCLL1, ERT_Gamma1&BBCLL1,
 *   ERT_Gamma2&BBCLL1 and ERT_Gamma3&BBCLL1. If these names are changed,
 *   we need to change the name too.
 *
 */

#include "muid_blt.h"
#include "OnlMonServer.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

// This is the flag for selection reading *.dat files
#define ONLINE 0
#define MYCODE 1


using namespace std;

muid_blt::muid_blt()
{
  cout << "muid_blt no argment start" << endl;
  cout << "You have to use this class with a integer argument" << endl;
}

muid_blt::muid_blt(int runNumber)
{
  char filename[1000];
  int data, Bdata, id;

  ver = 0;

  hard = 0;
  hard_bbc = 0;
  soft = 0;
  dec_input = 0;

  if ( clear_matrix() != 0)
    {
      cout << "clear error " << endl;
    }

  for (int i = 0; i < 8; i++)
    {
      quad_mlu_input[i] = 0;
    }

  // set bit mask by Trigger Helper
  setBitMask(runNumber);

  // If you use this code for online monitor, choose "ONLINE" flag.
  if ( ONLINE == 0 )
    {  // for online monitor
      if (!getenv("ONLMON_CALIB"))
        {
          cout << "ONLMON_CALIB directory does not exist." << endl;
          return ;
        }
      //Read if decision file
      sprintf(filename, "%s/mui-decision.dat", getenv("ONLMON_CALIB"));
      FILE *fp = fopen(filename, "r");

      if (fp == NULL)
        {
          printf("%s can not be open\n", filename);
          return ;
        }
      for (int i = 0; i < 4096; i++)
        {
          fscanf(fp, "%x", &data);
          Decision[i] = data;
        }
      fclose(fp);
      printf("%s read done \n", filename);

      //Read in deep logic file
      sprintf(filename, "%s/mui-deep.dat", getenv("ONLMON_CALIB"));
      fp = fopen(filename, "r");
      if (fp == NULL)
        {
          printf("%s can not be open\n", filename);
          return ;
        }
      for (int i = 0; i < 4096; i++)
        {
          fscanf(fp, "%x", &data);
          for ( int j = 0; j < 16; j++)
            {
              Bdata = (data >> j) & 0x01;
              id = (j << 12) + i;
              Deep[id] = Bdata;
            }
        }
      fclose(fp);
      printf("%s read done \n", filename);
      //Read in shallow logic file
      sprintf(filename, "%s/mui-shallow.dat", getenv("ONLMON_CALIB"));
      fp = fopen(filename, "r");
      if (fp == NULL)
        {
          printf("%s can not be open\n", filename);
          return ;
        }
      for (int i = 0; i < 4096; i++)
        {
          fscanf(fp, "%x", &data);
          for ( int j = 0; j < 16; j++)
            {
              Bdata = (data >> j) & 0x01;
              id = (j << 12) + i;
              Shallow[id] = Bdata;
            }
        }
      fclose(fp);
      printf("%s read done \n", filename);

    }
  else
    { // for private code
      // this is for private code. **Plese don't remove below**!!
      for (int ic = 0; ic < 3; ic++)
        {
          if (ic == 0)
            {
              strcpy(filename, "decision.dat");
            }
          else if (ic == 1)
            {
              strcpy(filename, "deep.dat");
            }
          else if (ic == 2)
            {
              strcpy(filename, "shallow.dat");
            }

          FILE *fp = fopen(filename, "r");
          if (fp == NULL)
            {
              printf("%s can not be open\n", filename);
            }

          for (int i = 0; i < 4096; i++)
            {
              fscanf(fp, "%x", &data);
              if (ic == 0 )
                {
                  Decision[i] = data;
                }
              else
                {
                  for ( int j = 0; j < 16; j++)
                    {
                      Bdata = (data >> j) & 0x01;
                      id = (j << 12) + i;
                      if (ic == 1)
                        {
                          Deep[id] = Bdata;
                        }
                      else if (ic == 2)
                        {
                          Shallow[id] = Bdata;
                        }
                    }
                }
            }
          fclose(fp);
          printf("%s read done \n", filename);
        }
    }

}



/*
 *
 * This function makes psudotrigger from prdf information
 *
 */
int muid_blt::get_each_quad_input(int ph[][6], int pv[][6], int q)
{
  if (ver > 10)
    cout << "get_each_quad_input start " << endl;
  int hb0[20];
  int hb1[20];
  int vb0[20];
  int vb1[20];

  int h[20][2];
  int v[20][2];

  for (int iroc = 0; iroc < 20; iroc++)
    {
      hb0[iroc] = 0;
      hb1[iroc] = 0;
      vb0[iroc] = 0;
      vb1[iroc] = 0;

      for (int iword = 0; iword < 4; iword++)
        { // for horizontal bit 0
          if (ph[iroc][iword] != 0 )
            {
              hb0[iroc] = 1;
            }
          if (ver >= 10)
            cout << "hb0 " << iroc << ":" << hb0[iroc] << endl;
        }
      for (int iword = 2; iword < 6; iword++)
        { // for horizontal bit 1
          if (ph[iroc][iword] != 0 )
            {
              hb1[iroc] = 1;
            }
          if (ver >= 10)
            cout << "hb1 " << iroc << ":" << hb1[iroc] << endl;
        }
      for (int iword = 0; iword < 3; iword++)
        { // for vertical bit 0
          if (pv[iroc][iword] != 0 )
            {
              vb0[iroc] = 1;
            }
          if (ver >= 10)
            cout << "vb0 " << iroc << ":" << vb0[iroc] << endl;
        }
      for (int iword = 3; iword < 6; iword++)
        { // for vertical bit 1
          if (pv[iroc][iword] != 0 )
            {
              vb1[iroc] = 1;
            }
          if (ver >= 10)
            cout << "vb1 " << iroc << ":" << vb1[iroc] << endl;
        }

      if ( hb0[iroc] != 0)
        { // horizontal bit 0
          h[iroc][0] = 1;
        }
      else
        {
          h[iroc][0] = 0;
        }
      if (ver >= 5)
        cout << "horizontal, iroc:bit:value " << iroc << ": 0 : " << h[iroc][0] << endl;

      if ( hb1[iroc] != 0)
        { // horizontal bit 1
          h[iroc][1] = 1;
        }
      else
        {
          h[iroc][1] = 0;
        }
      if (ver >= 5)
        cout << "horizontal, iroc:bit:value " << iroc << ": 1 : " << h[iroc][1] << endl;

      if ( vb0[iroc] != 0)
        { // vertical bit 0
          v[iroc][0] = 1;
        }
      else
        {
          v[iroc][0] = 0;
        }
      if (ver >= 5)
        cout << "vertival, iroc:bit:value " << iroc << ": 0 : " << v[iroc][0] << endl;

      if ( vb1[iroc] != 0)
        { // vertical bit 1
          v[iroc][1] = 1;
        }
      else
        {
          v[iroc][1] = 0;
        }
      if (ver >= 5)
        cout << "vertival, iroc:bit:value " << iroc << ": 1 : " << v[iroc][1] << endl;

    } // for loop

  switch ( q )
    {
    case 0 :  // for quad 0
      return (v[12][1] << 15) | (v[13][1] << 14) | (h[15][0] << 13) | (h[14][0] << 12) |  // for gap 3
	(v[8][1] << 11) | (v[9][1] << 10) | (h[11][0] << 9) | (h[10][0] << 8) |  // for gap 2
	(v[4][1] << 7) | (v[5][1] << 6) | (h[7][0] << 5) | (h[6][0] << 4) |  // for gap 1
	//(v[0][1]  <<  3) | (v[1][1]  <<  2) | (h[3][0]  <<  1) | ( (h[2][0]  <<  0) & 0) ; // for gap 0
	(v[0][1] << 3) | (v[1][1] << 2) | (h[3][0] << 1) | (h[2][0] << 0) ; // for gap 0
      break;

    case 1 :  // for quad 1
      return (v[15][1] << 15) | (v[14][1] << 14) | (h[15][1] << 13) | (h[14][1] << 12) |  // for gap 3
	(v[11][1] << 11) | (v[10][1] << 10) | (h[11][1] << 9) | (h[10][1] << 8) |  // for gap 2
	(v[7][1] << 7) | (v[6][1] << 6) | (h[7][1] << 5) | (h[6][1] << 4) |  // for gap 1
	(v[3][1] << 3) | (v[2][1] << 2) | (h[3][1] << 1) | (h[2][1] << 0) ; // for gap 0
      break;

    case 2 :  // for quad 2
      return (v[15][0] << 15) | (v[14][0] << 14) | (h[12][1] << 13) | (h[13][1] << 12) |  // for gap 3
	(v[11][0] << 11) | (v[10][0] << 10) | (h[8][1] << 9) | (h[9][1] << 8) |  // for gap 2
	(v[7][0] << 7) | (v[6][0] << 6) | (h[4][1] << 5) | (h[5][1] << 4) |  // for gap 1
	(v[3][0] << 3) | (v[2][0] << 2) | (h[0][1] << 1) | (h[1][1] << 0) ; // for gap 0
      break;

    case 3 :  // for quad 3
      return (v[12][0] << 15) | (v[13][0] << 14) | (h[12][0] << 13) | (h[13][0] << 12) |  // for gap 3
	(v[8][0] << 11) | (v[9][0] << 10) | (h[8][0] << 9) | (h[9][0] << 8) |  // for gap 2
	(v[4][0] << 7) | (v[5][0] << 6) | (h[4][0] << 5) | (h[5][0] << 4) |  // for gap 1
	(v[0][0] << 3) | (v[1][0] << 2) | (h[0][0] << 1) | (h[1][0] << 0) ; // for gap 0
      break;

    default :
      cout << "can't get information " << endl;
      break;

    }
  return -1;
} // get_each_quad_input


int muid_blt::get_quad_input(int bit[][20][6], int quad[])
{
  if (ver > 10)
    cout << "get_quad_input start" << endl;
  int sh[20][6];
  int sv[20][6];
  int nh[20][6];
  int nv[20][6];

  for (int i = 0; i < 20; i++)
    {
      for (int j = 0; j < 6; j++)
        {
          sh[i][j] = bit[0][i][j];
          sv[i][j] = bit[1][i][j];
          nh[i][j] = bit[2][i][j];
          nv[i][j] = bit[3][i][j];
        }
    }

  for (int i = 0; i < 4; i++)
    {
      quad[i] = get_each_quad_input(sh, sv, i);
    }
  for (int i = 4; i < 8; i++)
    {
      quad[i] = get_each_quad_input(nh, nv, (i - 4));
    }

  if (ver > 10)
    cout << "get_quad_input end " << endl;
  return 1;

} // end get_auad_input


// get each mlu response
//    input
//     config : = 0 for decision
//              = 1 for deep quad
//              = 2 for shallow quad
//     index  : input address
//    return mlu output
//
int muid_blt::get_mlu(int config, int index)
{
  if (ver > 10)
    cout << "get_mlu start" << endl;
  int data;
  if (config == 0)
    {
      data = Decision[index];
    }
  else if (config == 1)
    {
      data = Deep[index];
    }
  else if (config == 2)
    {
      data = Shallow[index];
    }
  else
    {
      return -1;
    }

  return data;

}

// get each quad mlu out
//    input
//       adr : input mlu address
//    return
//     deep bit    at 0th bit
//        shallow bit at 4th bit
//
int muid_blt::get_each_quad(int adr)
{
  if (ver > 10)
    cout << "get_each_quad start " << endl;
  int deep, shallow, out;

  deep = get_mlu(1, adr);
  shallow = get_mlu(2, adr);
  out = (shallow << 4) | deep;
  return out;
}

// get quad mlu output at dec input
//   input 16bit each
//       adr[0] : South quad 0
//           1  :            1
//           2               2
//           3               3
//           4    North quad 0
//           5               1
//           6               2
//           7               3
//      return
//         decision input
//
int muid_blt::get_quad(int adr[]) // input is ptrig[8]
{
  if (ver > 10)
    cout << "get_quad start" << endl;
  int data, d;
  data = 0;
  for (int i = 0; i < 8; i++)
    {
      d = get_each_quad(adr[i]);
      if (i > 3)
        {
          data = data | (d << (i + 4) );
        }
      else
        {
          data = data | (d << i);
        }
    }
  return data;

}


/* get decision output
   input 
   adr : decision mlu input 
   bit 0 : South Quad 0 deep
   1 :            1 deep
   2 :            2 deep
   3 :            3 deep
   4 : South Quad 0 shallow
   5 :            1 shwllow
   6 :            2 shallow
   7 :            3 shallow
   8 : North Quad 0 deep
   9 :            1 deep
   10:            2 deep
   11:            3 deep
   12: North Quad 0 shallow
   13:            1 shallow
   14:            2 shallow
   15:            3 shallow
   return
   decision mlu out
   bit 0 : North 1S
   1 :       2S
   2 :       1D
   3 :       1D1S
   4 :       2D
   5 : South 1S
   6 :       2S
   7 :       1D
   8 :       1D1S
   9 :       2D
*/
int muid_blt::get_dec(int adr)
{
  int adrs, adrn;
  int datas, datan, data;
  adrs = adr & 0xff;
  adrn = (adr >> 8) & 0xff;
  datas = get_mlu(0, adrs);
  datan = get_mlu(0, adrn);
  data = (datas << 5) | datan;
  return data;
}

/* get blt output
   input
   ptrg : psude trigger output
   16bit * 8
   adr[0] : South quad 0
   1  :            1
   2               2
   3               3
   4    North quad 0
   5               1
   6               2
   7               3
   return
   bit 0 : North 1S
   1 :       2S
   2 :       1D
   3 :       1D1S
   4 :       2D
   5 : South 1S
   6 :       2S
   7 :       1D
   8 :       1D1S
   9 :       2D
                                                                                        
*/
int muid_blt::get_blt(int ptrig[])
{
  if (ver > 10)
    cout << " get_blt start" << endl;
  int quadout, decout;
  quadout = get_quad(ptrig);
  decout = get_dec(quadout);
  return decout;
}

/* get global level 1 trigger
   input
   psude trigger output
   return
   The same as real bit mask.
   See the define at top line.
*/
int muid_blt::get_GL1(int ptrig[])
{
  if (ver > 10)
    cout << "get_GL1 start" << endl;
  int data, bltout;

  bltout = get_blt(ptrig);

  data = 0;

  if ( (bltout & (1 << 7) ) > 0 )
    { // for S1D
      data = data | S1D;
    }
  /*
    DS&VTC, Feb 2, 2005: Looks like 2D and 1D1S are mixed up for RUN-5. 
    - this is the correct line for S2D:  
  if ( (bltout & (1 << 9) ) > 0 )
  but we leave the incorrect line below (w. bit 8 instead of 9) in for now.
  */
  if ( (bltout & (1 << 8) ) > 0 ) 
    { // for S2D
      data = data | S2D;
    }
  if ( (bltout & (1 << 8) ) > 0 )
    { // for S1D1S
      data = data | S1D1S;
    }
  if ( (bltout & (1 << 2) ) > 0 )
    { // for N1D
      data = data | N1D;
    }
  /*
    DS&VTC, Feb 2, 2005: Looks like 2D and 1D1S are mixed up for RUN-5. 
    - this is the correct line for N2D:  
  if ( (bltout & (1 << 4) ) > 0 )
  but we leave the incorrect line below (w. bit 3 instead of 4) in for now.
  */
  if ( (bltout & (1 << 3) ) > 0 )
    { // for N2D
      data = data | N2D;
    }
  if ( (bltout & (1 << 3) ) > 0 )
    { // for N1D1S
      data = data | N1D1S;
    }

  if (ver > 10)
    cout << "get_GL1 end" << endl;
  return data;
}

/*
 * This function fills the emulated GL1 information from MuID packets into "soft", 
 * GL1 information into "hard", Scaled GL1 information into "hard_bbc", the emulated 
 * Decision MLU input information into "dec_input" and the emulated Quad MLU input 
 * (psude trigger input) from MuID packets into "quad_mlu_input" array.
 * and fill whole_matrix, quad_matrix, masked_matrix and activated_matrix.
 *
 * These variables and array are private and used by another functions.  
 *
 * This function have to be called once in event loop. 
 *
 */
int muid_blt::set_all(int bit[][20][6], int gl1, int gl1_scaled)
{
  hard = 0;
  hard_bbc = 0;
  soft = 0;

  soft = get_all(bit);
  hard = gl1;
  hard_bbc = gl1_scaled;
  get_quad_input(bit, quad_mlu_input);
  dec_input = get_quad(quad_mlu_input);  // for quad information

  if ( (hard_bbc & MINIBIAS) == 0 )
    return 0; //select  bbc events

  if ( (hard_bbc & (PPG_TEST | PPG_LASER)) != 0 )
    return 0; //reject laser events

  if ( fill_whole_matrix() != 0 )
    {
      cout << "error whole" << endl;
      return -9999;
    }
  if ( fill_quad_matrix() != 0 )
    {
      cout << "error quad" << endl;
      return -9999;
    }

  if ( fill_masked_matrix() != 0 )
    {
      cout << "error mask" << endl;
      return -9999;
    }

  return 0;
}


int muid_blt::get_all(int bit[][20][6])
{
  if (ver > 10)
    cout << " get_all start " << endl;
  int sh[20][6];
  int sv[20][6];
  int nh[20][6];
  int nv[20][6];
  int quad[8];

  for (int i = 0; i < 20; i++)
    {
      for (int j = 0; j < 6; j++)
        {
          sh[i][j] = bit[0][i][j];
          sv[i][j] = bit[1][i][j];
          nh[i][j] = bit[2][i][j];
          nv[i][j] = bit[3][i][j];
        }
    }
  for (int i = 0; i < 4; i++)
    {
      quad[i] = get_each_quad_input(sh, sv, i);
      quad[(i + 4)] = get_each_quad_input(nh, nv, i);
    }

  return get_GL1(quad);
}


/*
 *  This function fills the number of matrix for 
 *  calculation whole efficiency 1D, 2D, 1D1S in both arm.
 *
 */
int muid_blt::fill_whole_matrix(void)
{
  // These arrays are the flag that the sim and gl1 fire or not
  // The array subscript stands for the trigger configuration;
  // 0 : S1D
  // 1 : S2D
  // 2 : S1D1S
  // 3 : N1D
  // 4 : N2D
  // 5 : N1D1S
  int sim[6];
  int gl1[6];
  for (int i = 0; i < 6; i++)
    {
      sim[i] = 0;
      gl1[i] = 0;
    }

  //set flag for trig configuration.
  if ( (soft & S1D) != 0 )
    sim[0] = 1;
  if ( (soft & S2D) != 0 )
    sim[1] = 1;
  if ( (soft & S1D1S) != 0 )
    sim[2] = 1;
  if ( (soft & N1D) != 0 )
    sim[3] = 1;
  if ( (soft & N2D) != 0 )
    sim[4] = 1;
  if ( (soft & N1D1S) != 0 )
    sim[5] = 1;

  if ( (hard & S1D) != 0 )
    gl1[0] = 1;
  if ( (hard & S2D) != 0 )
    gl1[1] = 1;
  if ( (hard & S1D1S) != 0 )
    gl1[2] = 1;
  if ( (hard & N1D) != 0 )
    gl1[3] = 1;
  if ( (hard & N2D) != 0 )
    gl1[4] = 1;
  if ( (hard & N1D1S) != 0 )
    gl1[5] = 1;

  // fill to the matrix
  for (int i = 0; i < 6; i++)
    {
      whole_matrix[i][sim[i]][gl1[i]]++;
    }


  return 0;
}



/*
 *         
 *  This function fill the number of matrix for 
 *  calculation quad efficiency 1D in both arm.
 *
 *  The matrix stands for "quad_matrix[arm][sim][gl1][quad]".
 */
int muid_blt::fill_quad_matrix(void)
{

  int sim = 0;
  int gl1 = 0;

  for (int arm = 0; arm < 2; arm++)
    { //arm 0 is for south, arm 1 is for north.
      //This is opposite the phenix convention.
      for (int quad = 0; quad < 4; quad++)
        {
          sim = 0;
          gl1 = 0;

          // set flag
          if ( arm == 0 )
            {
              if ( (dec_input & (0x0001 << quad) ) != 0 )
                {
                  sim = 1;
                }
              if ( (hard & S1D ) != 0 )
                gl1 = 1;
            }

          else
            {
              if ( (dec_input & (0x0100 << quad) ) != 0 )
                {
                  sim = 1;
                }
              if ( ( hard & N1D ) != 0 )
                gl1 = 1;
            }

          // fill to the matrix
          quad_matrix[arm][sim][gl1][quad]++;

        }
    }

  return 0;
}


/*
 * This function is to fill the informatin for masked efficiency.
 * and activated efficiency.
 *
 * The "mask" means "&0" mask to bit by bit.  
 * The "active" means "|1" mask to bit by bit. 
 *
 * Mptirg[8] store the masked quad_mlu_input[8].
 * mask_matrix[arm][sim][gl1][quad][shift]
 * act_matrix[arm][sim][gl1][quad][shift]
 *
 */
int muid_blt::fill_masked_matrix(void) // get masked quad by quad efficency
{
  if (ver > 10)
    cout << "fill_madked_matrix start" << endl;

  int Mdec_input = 0;
  int sim = 0;
  int gl1 = 0;
  int mask = 0;
  // for getting masked decision mlu input
  int Mptrig[8];
  for (int i = 0; i < 8; i++)
    {
      Mptrig[i] = 0;
    }

  for (int mask_or_act = 0; mask_or_act < 2; mask_or_act++)
    {  // 0 is mask, 1 is act
      for (int shift = 0; shift < 16; shift++)
        {
          for (int quad = 0; quad < 4; quad++)
            {

              for (int q = 0; q < 8; q++)
                {  // Mptrig should be initialize
                  Mptrig[q] = quad_mlu_input[q];
                }

              if (mask_or_act == 0)
                {
                  mask = ( ~(0x1 << shift ) ) & 0xffff; // for mask
                  Mptrig[quad] = quad_mlu_input[quad] & mask; // for south
                  Mptrig[quad + 4] = quad_mlu_input[quad + 4] & mask; // for north

                }
              else
                {
                  mask = ( 0x1 << shift ) & 0xffff; // for activate
                  Mptrig[quad] = quad_mlu_input[quad] | mask;
                  Mptrig[quad + 4] = quad_mlu_input[quad + 4] | mask;
                }

              Mdec_input = get_quad(Mptrig); // for the masked dec input

              for (int arm = 0; arm < 2; arm++)
                {
                  sim = 0;
                  gl1 = 0;
                  // set flag
                  if ( arm == 0 )
                    {
                      if ( ( Mdec_input & (0x0001 << quad) ))
                        { // for south 1D
                          sim = 1;
                        }
                      if ( (hard & S1D ) != 0 )
                        gl1 = 1;
                    }
                  else
                    {
                      if ( ( Mdec_input & (0x0100 << quad )) )
                        { //for north 1D
                          sim = 1;
                        }
                      if ( ( hard & N1D ) != 0 )
                        gl1 = 1;
                    }

                  // fill matrix
                  if (mask_or_act == 0)
                    {
                      mask_matrix[arm][sim][gl1][quad][shift]++;
                    }
                  else
                    {
                      act_matrix[arm][sim][gl1][quad][shift]++;
                    }


                } // arm loop
            } // quad loop
        } // shift loop
    } // mask_or_act  loop

  if (ver > 10)
    cout << "fill_madked_matrix start" << endl;
  return 0;

}


/*
 *
 * Clear all matrix    
 *
 */
int muid_blt::clear_matrix(void)
{

  for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 2; j++)
        {
          for (int k = 0; k < 2; k++)
            {
              whole_matrix[i][j][k] = 0;
            }
        }
    }

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
        {
          for (int k = 0; k < 2; k++)
            {
              for (int l = 0; l < 4; l++)
                {
                  for (int m = 0; m < 16; m++)
                    {
                      mask_matrix[i][j][k][l][m] = 0;
                      act_matrix[i][j][k][l][m] = 0;
                    }
                  quad_matrix[i][j][k][l] = 0;
                }
            }
        }
    }

  cout << "matrix is cleared" << endl;
  return 0;
}

/*
 * This function return numerator and denominator for getting 
 * whole efficiency
 *
 * Input argument should be whole_numerator[12] and whole_denominator[12]
 *
 * The whole_numerator[0] and the whole denominator[0] are
 * for S1D Trigger efficiency 
 *
 * The whole_numerator[1] and the whole denominator[1] are
 * for S1D Data efficiency 
 *
 * The whole_numerator[2] and the whole denominator[2] are
 * for S2D Trigger efficiency 
 *
 * The whole_numerator[3] and the whole denominator[3] are
 * for S2D Data efficiency 
 *
 * The whole_numerator[4] and the whole denominator[4] are
 * for S1D1S Trigger efficiency 
 *
 * The whole_numerator[5] and the whole denominator[5] are
 * for S1D1S Data efficiency 
 *
 * The whole_numerator[6] and the whole denominator[6] are
 * for N1D Trigger efficiency 
 *
 * The whole_numerator[7] and the whole denominator[7] are
 * for N1D Data efficiency 
 *
 * The whole_numerator[8] and the whole denominator[8] are
 * for N2D Trigger efficiency 
 *
 * The whole_numerator[9] and the whole denominator[9] are
 * for N2D Data efficiency 
 *
 * The whole_numerator[10] and the whole denominator[10] are
 * for N1D1D Trigger efficiency 
 *
 * The whole_numerator[11] and the whole denominator[11] are
 * for N1D1S Data Data efficiency 
 *
 */
int muid_blt::get_whole_eff(int whole_numerator[], int whole_denominator[])
{
  if (ver > 10)
    cout << "get_whole_eff start" << endl;

  whole_numerator[0] = whole_matrix[0][1][1];
  whole_numerator[1] = whole_matrix[0][1][1];
  whole_numerator[2] = whole_matrix[1][1][1];
  whole_numerator[3] = whole_matrix[1][1][1];
  whole_numerator[4] = whole_matrix[2][1][1];
  whole_numerator[5] = whole_matrix[2][1][1];
  whole_numerator[6] = whole_matrix[3][1][1];
  whole_numerator[7] = whole_matrix[3][1][1];
  whole_numerator[8] = whole_matrix[4][1][1];
  whole_numerator[9] = whole_matrix[4][1][1];
  whole_numerator[10] = whole_matrix[5][1][1];
  whole_numerator[11] = whole_matrix[5][1][1];

  whole_denominator[0] = whole_matrix[0][1][1] + whole_matrix[0][1][0]; // for trig eff
  whole_denominator[1] = whole_matrix[0][1][1] + whole_matrix[0][0][1]; // for data eff
  whole_denominator[2] = whole_matrix[1][1][1] + whole_matrix[1][1][0]; // for trig eff
  whole_denominator[3] = whole_matrix[1][1][1] + whole_matrix[1][0][1]; // for data eff
  whole_denominator[4] = whole_matrix[2][1][1] + whole_matrix[2][1][0]; // for trig eff
  whole_denominator[5] = whole_matrix[2][1][1] + whole_matrix[2][0][1]; // for data eff
  whole_denominator[6] = whole_matrix[3][1][1] + whole_matrix[3][1][0]; // for trig eff
  whole_denominator[7] = whole_matrix[3][1][1] + whole_matrix[3][0][1]; // for data eff
  whole_denominator[8] = whole_matrix[4][1][1] + whole_matrix[4][1][0]; // for trig eff
  whole_denominator[9] = whole_matrix[4][1][1] + whole_matrix[4][0][1]; // for data eff
  whole_denominator[10] = whole_matrix[5][1][1] + whole_matrix[5][1][0]; // for trig eff
  whole_denominator[11] = whole_matrix[5][1][1] + whole_matrix[5][0][1]; // for data eff


  if (ver > 10)
    cout << "get_whole_eff end" << endl;
  return 0;
}

/*
 * This function return numerator and denominator for getting 
 * quad efficiency.
 *
 * Input argument should be quad_numerator[2][4] and quad_denominator[2][4].
 *
 * The quad_numerator[0][quad] and the quad_denominatro[0][quad] are
 * for S1D quad efficiency. The quad correspond to qudrant number. 
 * 
 * The quad_numerator[1][quad] and the quad_denominatro[1][quad] are
 * for N1D quad efficiency. The quad correspond to qudrant number. 
 
*/
int muid_blt::get_quad_eff(int quad_numerator[][4], int quad_denominator[][4])
{
  for (int arm = 0; arm < 2; arm++)
    {
      for (int quad = 0; quad < 4; quad++)
        {
          quad_numerator[arm][quad] = quad_matrix[arm][1][1][quad];
          quad_denominator[arm][quad] = quad_matrix[arm][1][1][quad] +
                                        quad_matrix[arm][1][0][quad]; // for TCE
        }
    }
  return 0;
}

/*
 * This function returns the array of the numerator and the denominator for getting 
 * mask(mask to 0) efficiency.
 *
 * Input argument should be mask_numerator[2][4][16] and mask_denominator[2][4][16].
 *
 * mask_matrix[arm][sim][gl1][quad][shift]
 * 
 * I used TCE for checking bit error.
 */
int muid_blt::get_masked_eff(int mask_numerator[][4][16], int mask_denominator[][4][16])
{
  for (int arm = 0; arm < 2; arm++)
    {
      for (int quad = 0; quad < 4; quad++)
        {
          for (int shift = 0; shift < 16; shift++)
            {
              mask_numerator[arm][quad][shift] = mask_matrix[arm][1][1][quad][shift];
              mask_denominator[arm][quad][shift] = mask_matrix[arm][1][1][quad][shift] +
                                                   mask_matrix[arm][1][0][quad][shift]; // for TCE
            }
        }
    }
  return 0;
}

/*
 * This function returns the array of the numerator and the denominator for getting 
 * activated(mask to 1 ) efficiency.
 *
 * Input argument should be act_numerator[2][4][16] and act_denominator[2][4][16].
 *
 * act_matrix[arm][sim][gl1][quad][shift]
 *
 * I used DCE for ckecking bit error.
 */
int muid_blt::get_activated_eff(int act_numerator[][4][16], int act_denominator[][4][16])
{

  for (int arm = 0; arm < 2; arm++)
    {
      for (int quad = 0; quad < 4; quad++)
        {
          for (int shift = 0; shift < 16; shift++)
            {
              act_numerator[arm][quad][shift] = act_matrix[arm][1][1][quad][shift];
              act_denominator[arm][quad][shift] = act_matrix[arm][1][1][quad][shift] +
                                                  act_matrix[arm][0][1][quad][shift]; // for DCE
            }
        }
    }

  return 0;
}

/*
 *
 * This function is not member function of muid_blt class.
 *
 * Input is numerator and denominator.
 * Output is efficiency and statistics error.
 *  
 * If a denominator or a numerator is zero, This function return efficiency 
 * is zero and error is one.
 *
 */
int get_eff(int num, int den, float &eff, float &error)
{
  if ( den == 0 || num == 0 )
    {
      eff = 0;
      error = 1;
    }
  else
    {
      float fnum = (float)num;
      float fden = (float)den;
      eff = fnum / fden;

      error = sqrt(eff * (1 - eff) / fden);
    }
  return 0;
}


// set bit mask pattern by Trigger Helper.
int muid_blt::setBitMask(int runNumber)
{
  OnlMonServer *se = OnlMonServer::instance();
  int mask = 0;
  int bitTemp[14];
  string maskName[14] = {"BBCLL1(>1 tubes)", "MUIDS_1D&BBCLL1", "MUIDS_2D&BBCLL1",
                         "MUIDS_1D1S&BBCLL1", "MUIDN_1D&BBCLL1", "MUIDN_2D&BBCLL1",
                         "MUIDN_1D1S&BBCLL1", "PPG(Test Pulse)", "PPG(Laser)", "BBCLL1&(ZDCN|ZDCS)",
                         "ERT_2x2&BBCLL1", "ERT_Gamma1&BBCLL1", "ERT_Gamma2&BBCLL1", "ERT_Gamma3&BBCLL1"};

  for (int i = 0; i < 14; i++)
    {
      mask = se->getLevel1Bit(maskName[i]);
      if (mask != 0 )
        {
          bitTemp[i] = mask;
          cout << "set " << maskName[i] << " bit mask as 0x" << hex << bitTemp[i] << dec << endl;
        }
      else
        {
          bitTemp[i] = 0;
          cout << "NO " << maskName[i] << " bit mask" << endl;
        }
    }
  //  BBC = bitTemp[0];
  BBC = 0x4;
  cout << " muid_blt:setBitMask hard-coding BBC trigger bit to be (hex) " << hex << BBC << dec << endl; 
  S1D = bitTemp[1];
  S2D = bitTemp[2];
  S1D1S = bitTemp[3];
  N1D = bitTemp[4];
  N2D = bitTemp[5];
  N1D1S = bitTemp[6];
  PPG_TEST = bitTemp[7];
  PPG_LASER = bitTemp[8];
  BBCZDC = bitTemp[9];
  ERT2 = bitTemp[10];
  ERTG1 = bitTemp[11];
  ERTG2 = bitTemp[12];
  ERTG3 = bitTemp[13];
  //  MINIBIAS = BBC | BBCZDC | ERT2 | ERTG1 | ERTG2 | ERTG3;
  MINIBIAS = BBC;
  cout << " muid_blt:setBitMask hard-coding MINIBIAS trigger to BBC " << endl;

  cout << " MUID BLT bits (hex)" << hex
       << " N1D " << N1D 
       << " S1D " << S1D 
       << " N2D " << N2D 
       << " S2D " << S2D << dec << endl; 

  return 0;
}
