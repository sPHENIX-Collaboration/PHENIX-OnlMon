#include <muiMonitor.h>
#include <muiMonGlobals.h>
#include <muiOptions.h>
#include <muiMap.h>
#include <mui_mon_const.h>
#include <mui_hit.h>

#include <OnlMonServer.h>
#include <TMuiChannelId.hh>
#include <msg_profile.h>
#include <Event.h>

#include <iostream>
#include <sstream>

using namespace std;
/*
  fill_var.cc
  read muid packet and fill word array and hit_list array
  Fill also raw_hit array using mapping (channel_map array)
  author Hiroki Sato (satohiro@bnl.gov)
*/


int muiMonitor::fill_var( Event* e )
{
  muiMonGlobals* globals = muiMonGlobals::Instance();
  ostringstream msg;
  //    long header_word[MAX_FEM][HEADER_WORDS];
  long usr_word[MAX_FEM][USR_WORDS];

  static int num_hits = 0;

  short raw_hit[ TMuiChannelId::kTwoPacksMaxTotal ];

  //    mui_hit hit_list[ MAX_CHANNEL];

  short roc_ok[ MAX_ROC ];
  short beam_ok[ MAX_ROC ];

  Packet* mui_packet;
  const int packet_id_offset = 12001;
  int packet_id;
  //  int ifem;

  // variables for zero-suppressed format
  long tmp_word;
  //  long data;
  short iroc, iword, ibit;

  TMuiChannelId mui_chan;
  int fee_chan, ichan;
  //    int phys_chan;

  //clear mui_hit array
  rawHits.erase(rawHits.begin(), rawHits.end());

  num_hits = 0;

  // raw_hit array reset
  for ( int i = 0; i < TMuiChannelId::kTwoPacksMaxTotal; i++)
    {
      raw_hit[i] = 0;
    }

  // get data
  for ( short iarm = 0; iarm < MAX_ARM; iarm++ )
    {
      for ( short iorient = 0; iorient < MAX_ORIENTATION; iorient++)
        {
          short ifem = iarm * MAX_ORIENTATION + iorient;

          packet_id = packet_id_offset + ifem;
          mui_packet = e->getPacket( packet_id );

          // word initialize; needed for zero-suppressed data
          for ( int i = 0; i < WORD_PER_FEM; i++ )
            {
              data_words[ ifem ][i] = 0;
            }

          // Send message to shift person if there is no MuID packet found
	  // and the arm is supposed to be active
          if ( ! mui_packet && globals->arm_active[iarm] )
            {
              globals->noPacketErrors[iarm]++;
              if (opts->getVerbose() >= 0)
                {
		  msg.str("");
                  msg << "No MuID packet " << packet_id << " found. ";
              OnlMonServer *se = OnlMonServer::instance();

		  se->send_message(this,MSG_SOURCE_MUID,MSG_SEV_WARNING, msg.str(),4);
                }
            }
          else
            {
              if ( opts->getVerbose() >= 5 ) {
		printf(" Arm = %d Orientation = %d packet found\n",iarm,iorient);
	      }

              // This is for variable length(zero-suppressed) format

              // mui_packet->iValue(0,"EVTNR") ... "BCLK",...
              //  mui_packet->iValue(i, "USERWORD")

              for ( int iusr_word = 0; iusr_word < USR_WORDS; iusr_word++ )
                {
                  usr_word[ ifem ][ iusr_word ] =
                    mui_packet->iValue(iusr_word, "USERWORD" );

		  if ( opts->getVerbose() >= 10 ) {
		    printf("ifem = %d iusr_word = %d usr_word = %08lx\n",ifem,iusr_word,usr_word[ifem][iusr_word]);
		  }

                }
              for (iroc = 0; iroc < MAX_ROC; iroc++)
                {
                  if ( iroc < 16 )
                    {
                      roc_ok[ iroc ] = short((usr_word[ifem][0] & (1 << iroc)) >> iroc);
                    }
                  else
                    {
                      roc_ok[iroc] = short((usr_word[ifem][1] & (1 << (iroc - 16)))
                                           >> (iroc - 16) );
                    }
                  if ( iroc < 12 )
                    {
                      beam_ok[iroc] = short((usr_word[ifem][1] & (1 << (iroc + 4)))
                                            >> (iroc + 4) );
                    }
                  else
                    {
                      beam_ok[iroc] = short((usr_word[ifem][2] & (1 << (iroc - 12)))
                                            >> (iroc - 12) );
                    }
                  if ( roc_ok[iroc] == 0 )
                    {
                      globals->rocOkErrors[iarm]++;
                      // send message to shift person if ROC status is not good
		      msg.str("");
                      msg << " MuID FEM # " << ifem
			  << " ROC # " << iroc
			  << " Status is not OK. "
			  << "Cycle Low Voltage If Chronic";
              OnlMonServer *se = OnlMonServer::instance();
		      se->send_message(this,MSG_SOURCE_MUID,MSG_SEV_WARNING, msg.str(),5);
                    }
                  if ( beam_ok[iroc] == 0 ) {
                      globals->beamOkErrors[iarm]++;
                      // send message to shift person if ROC status is not good
		      msg.str("");
                      msg << " MuID FEM # " << ifem
			  << " ROC # " << iroc
			  << " Beam Clock Counter is not OK " << endl;
              OnlMonServer *se = OnlMonServer::instance();
		      se->send_message(this,MSG_SOURCE_MUID,MSG_SEV_ERROR, msg.str(),6);
                    }
                  else
                    {
                      for (iword = 0; iword < WORD_PER_ROC; iword++)
                        {
                          tmp_word = mui_packet->iValue(iword, iroc);
                          data_words [ifem] [ iroc * WORD_PER_ROC + iword ] =
                            tmp_word;
                          for ( ibit = 0; ibit < BIT_PER_WORD; ibit++ )
                            {
                              if ( ( tmp_word & (1 << ibit) ) == (1 << ibit) )
                                {
                                  mui_hit temphit;
                                  temphit.arm = iarm;
                                  temphit.orient = iorient;
                                  temphit.fem = ifem;
                                  temphit.roc = iroc;
                                  temphit.word = iword;
                                  temphit.bit = ibit;
                                  hwMap->setHardwareAddress(ifem, iroc, iword, ibit);
                                  //CHANGE TO CHECK RANGES OF FEM ROC...
                                  fee_chan = ((ifem * MAX_ROC + iroc ) * WORD_PER_ROC
                                              + iword ) * BIT_PER_WORD + ibit;
                                  if ( fee_chan >= 0 && fee_chan < MAX_CHANNEL )
                                    {
                                      //phys_chan = channel_map[ fee_chan ];
                                      if ( hwMap->getGap() < TMuiChannelId::kPlanesPerArm
                                           && hwMap->getGap() >= 0
                                           && hwMap->getPanel() < TMuiChannelId::kPanelsPerPlane
                                           && hwMap->getPanel() >= 0
                                           && hwMap->getTwoPack()
                                           < TMuiChannelId::kTwoPacksPerPlaneMax
                                           && hwMap->getTwoPack() >= 0 )
                                        {
                                          temphit.plane = (short)hwMap->getGap();
                                          temphit.panel = (short)hwMap->getPanel();
                                          temphit.twopack = (short)hwMap->getTwoPack();

                                          //  		      if( raw_hit[phys_chan] > 0 ) {
                                          //  			cout << " double hits found " << phys_chan << endl;
                                          //  		      }
                                          //  		      raw_hit[ phys_chan ]++;
                                        }
                                      else
                                        {
                                          if ( opts->getVerbose() > 0 )
                                            {
					      msg.str("");
					      msg << " no such a physcal channel " // << phys_chan
						  << " FEM " << ifem
						  << " ROC = " << iroc
						  << " WORD = " << iword
						  << " BIT = " << ibit;
              OnlMonServer *se = OnlMonServer::instance();

					      se->send_message(this,MSG_SOURCE_MUID,MSG_SEV_WARNING, msg.str(),7);
                                            }
                                          temphit.plane = -1;
                                          temphit.panel = -1;
                                          temphit.twopack = -1;
                                        }
                                    }
                                  else
                                    {
				      msg.str("");
				      msg << " FEE channel out of range " << fee_chan;
              OnlMonServer *se = OnlMonServer::instance();
				      se->send_message(this,MSG_SOURCE_MUID,MSG_SEV_WARNING, msg.str(),8);
                                    }

                                  if ( opts->getVerbose() >= 20 )
                                    {
				      printf("fill_var: Hit found Arm = %d Orient = %d FEM =  %d Roc =  %d Word =  %d Bit = %d plane = %d panel = %d twopack = %d\n", temphit.arm,temphit.orient,temphit.fem,temphit.roc,temphit.word,temphit.bit,temphit.plane,temphit.panel,temphit.twopack);
                                    }
                                  // fill reference arrays
                                  ichan = (iroc * WORD_PER_ROC + iword ) * BIT_PER_WORD
                                          + ibit;

                                  num_hits++;
                                  //  		  cout<<temphit.fem<<" "
                                  //  		      <<temphit.roc<<" "
                                  //  		      <<temphit.word<<" "
                                  //  		      <<temphit.bit<<" "
                                  //  		      <<temphit.arm<<" "
                                  //  		      <<temphit.plane<<" "
                                  //  		      <<temphit.panel<<" "
                                  //  		      <<temphit.orient<<" "
                                  //  		      <<temphit.twopack<<endl;


                                  rawHits.push_back(temphit);

                                } // if there is a hit
                            } // for bit = 0...
                        } // for iword = 0...
                    } // if roc_ok == 1 ...
                } // for roc = 0...
            } // if mui_packet
          delete mui_packet;
        } // for orient...
    } // for arm ...
  return 0;
}

