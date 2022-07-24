/*
 * File: muid_blt.h
 *
 * Author: Nobuyuki Kamihara(kamihara@bnl.gov)
 * Date: 2003/03/19 22:10
 *
 * This file is the header file for muid_blt.cc
 *
 * History:
 * - 2003/04/08 20:00 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I add the "get_activated_eff" function. 
 * - 2003/04/15 00:20 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I add the variables for bit mask pattern. These bit pattern
 *   were difined by "#devine" in old version. I add anthor constructor.
 *   
 * - 2003/04/18 14:30 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I add setBitMask function which set bit mask configuration
 *   by TriggerHelper. 
 *
 * - 2003/04/26 19:20 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I added the trigger configuration of PPG_TEST and PPG_LASER 
 *   due to reject the laser events.
 *
 * - 2003/05/02 23:20 by Nobuyuki Kamihara(kamihara@bnl.gov)
 *   I added the trigger configuration of MINIBAIS, BBCZDC,
 *   ERT2, ERTG1, ERTG2 and ERTG3. 
 *   MINIBAIS =  BBCZDC | ERT2 | ERTG1 | ERTG2 | ERTG3     
 */

#ifndef __MUID_BLT__
#define __MUID_BLT__


int get_eff(int num, int den, float &eff, float &error);

class muid_blt
{

 private:
  int ver; // for verbose mode
  int Decision[0x1000];
  int Deep[0x10000];
  int Shallow[0x10000];

  // whole_matrix[trig_config][sim][gl1]
  int whole_matrix[6][2][2]; // for 2D, 1D, 1D1S eff
  // quad_matrix[arm][sim][gl1][quad]
  int quad_matrix[2][2][2][4]; // for quad by quad eff
  // mask_matrix[arm][sim][gl1][quad][shift][mask_or_act]
  int mask_matrix[2][2][2][4][16]; // for masked eff
  // act_matrix[arm][sim][gl1][quad][shift][mask_or_act]
  int act_matrix[2][2][2][4][16]; // for activated eff
  int hard; // for gl1
  int hard_bbc; // for gl1 scaled
  int soft; // for sim

  // Bit mask pattern
  int MINIBIAS;
  int S1D;
  int S2D;
  int S1D1S;
  int N1D;
  int N2D;
  int N1D1S;
  int PPG_TEST;
  int PPG_LASER;
  int BBC;
  int BBCZDC;
  int ERT2;
  int ERTG1;
  int ERTG2;
  int ERTG3;


  // for quad efficiency
  int quad_mlu_input[8];
  int dec_input;

  // emulate BLT
  int get_each_quad_input(int ph[][6], int pv[][6], int q);
  int get_quad_input(int bit[][20][6], int quad[]);
  int get_mlu(int config, int index);
  int get_each_quad(int adr);
  int get_quad(int adr[]);
  int get_dec(int adr);
  int get_blt(int ptrig[]);
  int get_GL1(int ptrig[]);


  // get efficiency using emulation of BLT
  int get_all(int bit[][20][6]);
  int check_quad(int dec_input, int arm, int depth);
  int fill_whole_matrix(void);
  int fill_quad_matrix(void);
  int fill_masked_matrix(void);

 public:
  muid_blt();

  muid_blt(int runNumber);

  virtual ~muid_blt(){};

  int set_all(int bit[][20][6], int gl1, int gl1_scaled);

  int clear_matrix(void);

  int get_whole_eff(int whole_numerator[], int whole_denominator[]);

  int get_quad_eff(int quad_numerator[2][4], int quad_denominator[2][4]);

  int get_masked_eff(int mask_numerator[][4][16], int mask_denominator[][4][16]);

  int get_activated_eff(int act_numerator[][4][16], int act_denominator[][4][16]);

  int setBitMask(int run_number);


};





#endif //__MUID_BLT__
