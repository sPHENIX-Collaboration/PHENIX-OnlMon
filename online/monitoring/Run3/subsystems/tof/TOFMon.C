#include <TOFMon.h>
#include <OnlMonServer.h>

#include <TofAddressObject.hh>
#include <TofCalibObject.hh>

#include <Event.h>
#include <msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TPaveText.h>
#include <TProfile.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

TOFMon::TOFMon(): OnlMon("TOFMON")
{

  debug = 0;

  printf("TOF monitoring starting\n") ;

  cutTofQVC = 6;
  cutTofTc_0 = 3880;
  //cutTofTc_0 = 3800;
  cutTofTc = 30;
  cutTofChg = 10;
  cutTofChg_low = 0.00200;
  //Mar 12 2006 Back to this value to cut low charge noises

  //cutTofChg_low = 0.00001; // Mar 11 2006
  cutTofChg_high = 0.00450;

  evtcnt = 0;
  evtTrigIn = 0;

  htofinfo = new TH1F("htofinfo", "TOFMonInfo" , 20, 0, 20);
  htof210 = new TH1F("htof210", "TVC (tc3 and tc4)" , 820, 0, 8200);
  htof220 = new TH1F("htof220", "Charge (qc1 - qc3)", 500, 0, 1000);
  htof230 = new TH1F("htof230", "Num. of Hit :Slat", TOF_NSLAT, 0, TOF_NSLAT);
  htof420 = new TH1F("htof420", "Hit Position" , 400, -20, 20);
  htof530 = new TProfile("htof530", "MIP peak vs slat", TOF_NSLAT, 0, TOF_NSLAT);

  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->registerHisto(this, htofinfo);
  Onlmonserver->registerHisto(this, htof210);
  Onlmonserver->registerHisto(this, htof220);
  Onlmonserver->registerHisto(this, htof230);
  Onlmonserver->registerHisto(this, htof420);
  Onlmonserver->registerHisto(this, htof530);

  tofmonaddress = new TofAddressObject();
  tofmoncalib = new TofCalibObject();
  string parafile_dir( getenv("TOFPARADIR") );

  ostringstream parafile;
  parafile << parafile_dir.c_str() << "/toffemmap.txt";
  tofmonaddress->fetchFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofPedestal.txt";
  tofmoncalib->fetchPedestalFromFile(parafile.str().c_str(), tofmonaddress);
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofTvcConv.txt";
  tofmoncalib->fetchTvcConvFromFile(parafile.str().c_str(), tofmonaddress);
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofQvcConv.txt";
  tofmoncalib->fetchQvcConvFromFile(parafile.str().c_str(), tofmonaddress);
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofSlewPar.txt";
  tofmoncalib->fetchSlewParFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofToffset.txt";
  tofmoncalib->fetchToffsetFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofYoffset.txt";
  tofmoncalib->fetchYoffsetFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofVelocity.txt";
  tofmoncalib->fetchVelocityFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofElossConv.txt";
  tofmoncalib->fetchElossConvFromFile(parafile.str().c_str());
  parafile.str("");

  parafile << parafile_dir.c_str() << "/tofGlobalT.txt";
  tofmoncalib->fetchGlobalTFromFile(parafile.str().c_str());
  parafile.str("");


}

TOFMon::~TOFMon()
{
  delete tofmonaddress;
  delete tofmoncalib;
  return;
}

int TOFMon::Reset()
{
  evtcnt = 0;
  evtTrigIn = 0;
  //evtTrigIn_ERT = 0;
  // chp: registered histograms are resetted by the server
  return 0;
}

int 
TOFMon::process_event(Event *evt)
{

  evtcnt++;

  const int cutTrigLow = 0x00000002;
  const int cutTrigHigh = 0x00000080;
  //const int ERTTrig = 0x00000400;   // ERTLL1_4x4c
  //const int BBCERTTrig = 0x00001000;   // ERTLL1_4x4c&BBCLL1
  //  const int ZDCNSTrig = 0x00000200; // (ZDCN|ZDCS)
  // const int BBCZDC = 0x00000400; // BBCLL1&(ZDCN|ZDCS)

  Packet *p;

  if (evtcnt % 5000 == 0)
    {
      printf("TOFMon:Processing Event %d\n", evtcnt) ;
    }
  if (debug)
    {
      printf("TOFMon:Processing Event: %d\n", evtcnt);
    }

  if (evt->getEvtType() == 1)
    {

      // Init. /////////////////////////////////////////////////

      for (int icrt = 0; icrt < 8; icrt++)
        {
          int maxbd = 12;
          if (icrt != 3 && icrt != 7)
            {
              maxbd = 16;
            }
          for (int ibd = 0; ibd < maxbd; ibd++)
            {
              for (int ich = 0; ich < 16; ich++)
                {
                  int slatID = tofmonaddress->getSlatID(icrt, ibd, ich);
                  int pmtID = tofmonaddress->getPmt(icrt, ibd, ich);
                  tofQc1_[pmtID][slatID] = -1;
                  tofQc2_[pmtID][slatID] = -1;
                  tofTc3_[pmtID][slatID] = -1;
                  tofTc4_[pmtID][slatID] = -1;
                }

	    }
	}

      // Trig Event //////////////////////////////////////////
      if (debug)
	{
	  cout << "Trig: ";
	}

      evtTrigIn++;

      // Tof Event //////////////////////////////////////////////

      if (debug)
        {
          cout << "Fetch: ";
        }
      	for (int icrt = 0; icrt < 8; icrt++)
        { //-------Data Fetch
	  //if(icrt!=0) continue;
          p = evt->getPacket(7001 + icrt);
          if (p)
            {
              int maxbd = 12;
              if (icrt != 3 && icrt != 7)
                {
                  maxbd = 16;
                }
              for (int ibd = 0; ibd < maxbd; ibd++)
                {
                  for (int ich = 0; ich < 16; ich++)
                    {
                      int slatID = tofmonaddress->getSlatID(icrt, ibd, ich);
                      int pmtID = tofmonaddress->getPmt(icrt, ibd, ich);
                      int iCh = 16 * ibd + ich;
                      tofQc1_[pmtID][slatID] = p->iValue(iCh, "QC1");
                      tofQc2_[pmtID][slatID] = p->iValue(iCh, "QC3");
                      tofTc3_[pmtID][slatID] = p->iValue(iCh, "TC3");
                      tofTc4_[pmtID][slatID] = p->iValue(iCh, "TC4");
                    }
                }
              delete p;
            } // End of if(p){
        } // End of Data Fetch

      	//for (int icrt = 0; icrt < 8; icrt++)
        //{ //-------Data Fetch
        //  p = evt->getPacket(7001 + icrt);
        //  if (p)
        //    {
        //      int maxbd = 12;
        //      if (icrt != 3 && icrt != 7)
        //        {
        //          maxbd = 16;
        //        }
        //      for (int ibd = 0; ibd < maxbd; ibd++)
        //        {
        //          for (int ich = 0; ich < 16; ich++)
        //            {
        //              int slatID = tofmonaddress->getSlatID(icrt, ibd, ich);
        //              int pmtID = tofmonaddress->getPmt(icrt, ibd, ich);
        //              int iCh = 16 * ibd + ich;
        //              tofQc1_[pmtID][slatID] = p->iValue(iCh, "QC1");
        //              tofQc2_[pmtID][slatID] = p->iValue(iCh, "QC3");
        //              tofTc3_[pmtID][slatID] = p->iValue(iCh, "TC3");
        //              tofTc4_[pmtID][slatID] = p->iValue(iCh, "TC4");
        //            }
        //        }
        //      delete p;
        //    } // End of if(p){
        //} // End of Data Fetch


      // Hist Fill ///////////////////////////////////////

      // PMT by PMT
      if (debug)
        {
          cout << "PMT: ";
        }
      for (int slatID = 0; slatID < TOF_NSLAT; slatID++)
        {
	  //if(slatID<288||383<slatID) continue;//D
	  //if(slatID<288||319<slatID) continue;//D,FEM Crate6
	  //if(slatID<320||383<slatID) continue;//D,FEM Crate4,mod8-15
	  //if(slatID<320||327<slatID) continue;//D,FEM Crate4,mod8,slat(32-39), no signal
	  //if(slatID<328||335<slatID) continue;//D,FEM Crate4,mod9,slat(40-47), good
	  //if(slatID<336||343<slatID) continue;//D,FEM Crate4,mod10,slat(48-55), good
	  //if(slatID<344||351<slatID) continue;//D,FEM Crate4,mod11,slat(56-63), good
	  //if(slatID<352||359<slatID) continue;//D,FEM Crate4,mod12,slat(64-71), good
	  //if(slatID<360||367<slatID) continue;//D,FEM Crate4,mod13,slat(72-79), good
	  //if(slatID<368||375<slatID) continue;//D,FEM Crate4,mod14,slat(80-87), good
	  //if(slatID<376||383<slatID) continue;//D,FEM Crate4,mod15,slat(88-95), good
	  //if(319<slatID && slatID<328) continue;//except for FEM Crate4,mod8,slat(32-39) it is temporally
          for (int pmtID = 0; pmtID < 2; pmtID++)
            {

              int nowTofQc1_0 = tofQc1_[pmtID][slatID];
              int nowTofQc2_0 = tofQc2_[pmtID][slatID];
              int nowTofTc3_0 = tofTc3_[pmtID][slatID];
              int nowTofTc4_0 = tofTc4_[pmtID][slatID];

              if ( nowTofQc1_0 > 0 && nowTofQc2_0 > 0 && nowTofTc3_0 > 0 && nowTofTc4_0 > 0)
                { // For 0 Sup.Check

                  // QVC //
                  int nowTofQVC = nowTofQc1_0 - nowTofQc2_0;
                  if (nowTofQVC > cutTofQVC)
                    {
                      htof220->Fill(nowTofQVC);	//--- "QVC (qc3 - qc1)"
                    }

                  // TVC //
                  if (nowTofTc3_0 < cutTofTc_0)
                  //if (nowTofTc3_0 < cutTofTc_0 && nowTofTc3_0 > 1000)
                    {
                      htof210->Fill(nowTofTc3_0);	//--- "TVC (tc3 or tc4)"
                    }
                  else
                    {
                      // Tc3 Pede
                    }

                  if (nowTofTc4_0 < cutTofTc_0)
                  //if (nowTofTc4_0 < cutTofTc_0 && nowTofTc4_0 > 1000)
                    {
                      htof210->Fill(nowTofTc4_0 + 4096);  //--- "TVC (tc3 or tc4)"
                    }
                  else
                    {
                      // Tc4 Pede
                    }

                } // End of. "For 0 Sup.Check"
            } // End of for( pmtID=0; pmtID<=1; pmtID++){
        } // End of for(slatID =0; slatID<= 959; slatID++){


      // slat by slat
      if (debug)
        {
          cout << "Slat: ";
        }
      for (int slatID = 0; slatID < TOF_NSLAT; slatID++)
        {
          int nowTofQc1_0 = tofQc1_[0][slatID];
          int nowTofQc2_0 = tofQc2_[0][slatID];
          int nowTofTc3_0 = tofTc3_[0][slatID];
          int nowTofTc4_0 = tofTc4_[0][slatID];
          float PedeTc3_0 = tofmoncalib->getTvcPede(0, slatID);
          float PedeTc4_0 = PedeTc3_0; // Tmp: pede(tc4) is same as pede(tc3), isn't it?

          int nowTofQc1_1 = tofQc1_[1][slatID];
          int nowTofQc2_1 = tofQc2_[1][slatID];
          int nowTofTc3_1 = tofTc3_[1][slatID];
          int nowTofTc4_1 = tofTc4_[1][slatID];
          float PedeTc3_1 = tofmoncalib->getTvcPede(1, slatID);
          float PedeTc4_1 = PedeTc3_1;

          if ( nowTofQc1_0 > 0 && nowTofQc2_0 > 0 && nowTofTc3_0 > 0 && nowTofTc4_0 > 0
               && nowTofQc1_1 > 0 && nowTofQc2_1 > 0 && nowTofTc3_1 > 0 && nowTofTc4_1 > 0)
            { // For 0 Sup.Check

              int nowQVC_0 = nowTofQc1_0 - nowTofQc2_0;
              int nowQVC_1 = nowTofQc1_1 - nowTofQc2_1;
              //float convQVC_0 = tofmoncalib->getQvcConv(0,slatID);
              //float convQVC_1 = tofmoncalib->getQvcConv(1,slatID);
              //float nowTofChg = sqrt(nowQVC_0 * convQVC_0 * nowQVC_1 * convQVC_1)/1000.;
              float ElossConv = tofmoncalib->getElossConv(slatID);
              float nowTofChg = 0;
              if (((double)nowQVC_0) * ((double)nowQVC_1) >= 0) nowTofChg = ElossConv * sqrt(((double)nowQVC_0) * ((double)nowQVC_1));

              //float AvenowQVC = (nowQVC_0+nowQVC_1)/2.;
              //float aveTvc = (nowTofTvc_0+nowTofTvc_1)/2.;



              if ( (nowTofTc3_0 < PedeTc3_0 - cutTofTc || nowTofTc4_0 < PedeTc4_0 - cutTofTc ) &&
                   (nowTofTc3_1 < PedeTc3_1 - cutTofTc || nowTofTc4_1 < PedeTc4_1 - cutTofTc ) &&
                   (nowTofChg > cutTofChg_low ) )
                { // tc3 or tc4 Cut && Charge Cut

                  htof230->Fill(slatID); // Num of Hit : Slat
                  //htof510->Fill(slatID,AvenowQVC);
                  //htof520->Fill(slatID,AvenowQVC);

                  if ( nowTofChg < cutTofChg_high )
                    {
                      htof530->Fill(slatID, nowTofChg);
                    }

                  const int TVC_DIFF_TC3_TC4 = 81; // Tmp 81ch? What?
                  float convTVC_0 = tofmoncalib->getTvcConv(0, slatID);
                  float convTVC_1 = tofmoncalib->getTvcConv(1, slatID);
                  float nowTofTvc_0 = nowTofTc3_0;
                  if (nowTofTc3_0 > nowTofTc4_0)
                    {
                      nowTofTvc_0 = nowTofTc4_0 + TVC_DIFF_TC3_TC4;
                    }
                  float nowTofTvc_1 = nowTofTc3_1;
                  if (nowTofTc3_1 > nowTofTc4_1)
                    {
                      nowTofTvc_1 = nowTofTc4_1 + TVC_DIFF_TC3_TC4;
                    }

                  float nowTofTime_0 = nowTofTvc_0 * convTVC_0;
                  float nowTofTime_1 = nowTofTvc_1 * convTVC_1;

                  //float Ttof = ( nowTofTime_0 + nowTofTime_1 ) / 2.;
                  float Tdif = ( nowTofTime_0 - nowTofTime_1 ) / 2.;

                  //htof410->Fill(Ttof);
                  htof420->Fill(Tdif);

                } // End of tc3 or tc4 Cut && Charge Cut
            } // End of for( pmtID=0; pmtID<=1; pmtID++){
        } // End of for(slatID =0; slatID<= 959; slatID++){
    } // End of if(evt->getEvtType()==1){
  else
    {
      //    if(evt->getEvtType()== 9){cout<<"Start Event"<<endl;}
      //else if(evt->getEvtType()==12){cout<<"End Event"<<endl;}
      if (debug)
        {
          cout << "Check TOFMon.C: evt->getEvtType()" << endl;
        }
    }
  if (debug)
    {
      cout << "Done" << endl;
    }

  htofinfo->SetBinContent(1, cutTrigLow);
  htofinfo->SetBinContent(2, cutTrigHigh);
  htofinfo->SetBinContent(3, cutTofQVC);
  htofinfo->SetBinContent(4, cutTofTc_0);
  htofinfo->SetBinContent(5, cutTofTc);
  htofinfo->SetBinContent(6, cutTofChg_low);
  htofinfo->SetBinContent(7, evtcnt);
  htofinfo->SetBinContent(8, evtTrigIn);
  //htofinfo->SetBinContent(9, evtTrigIn_ERT);

  return 0;
}

