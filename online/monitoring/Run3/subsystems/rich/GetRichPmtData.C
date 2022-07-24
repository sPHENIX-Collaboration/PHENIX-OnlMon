/* The class GetRichPmtdata is used to read and decode PHENIX raw data format
 * from a specified file or a ddpool. The sequence of events is:
 *
 *       The calling program creates a new object of class GetRichPmtData,
 *       giving the new object the pointer to an event.
 *
 *       The calling program then loops over all 5,120 PMT channels, reading
 *       out 1 PMT channel at a time by repeatedly calling the function
 *       GetRichPmtData::get_pmt_data. (The get_pmt_data function 
 *       automatically creates new packet pointers when the data in each 
 *       packet are exhausted).
 *
 *       Finally, the calling program deletes the GetRichPmtData object.
 *
 *  An example of the use of this class can be found in RichOnlineMonitor.C
 *
 */

#include "GetRichPmtData.h"
#include "RichGeom.h"

#include "Event.h"
#include "packet.h"

#include <fstream>
#include <unistd.h>

GetRichPmtData::GetRichPmtData(Event *event, int *stat, float *ped, float *sigp, float *pp, float *sgpp, int pr_hts)
{

  geo = new RichGeom();

  evt = event;
  print_hits = pr_hts;

  p = 0;

  pnum = 6001;   // The Rich packets start at 6001 for each event
  packetsperevent = 40; // Actually there are only 32, but there is a gap
  wordsperpacket = 0; // Depends on the data format, returned with packet data

  //  rawadcval= new int(160);
  //  rawadcpval= new int(160);
  //  rawtacval= new int(160);
  //  packetdata= new int(1000);

  pedestal = ped;
  sigped = sigp;
  pep = pp;
  sigpep = sgpp;

  ich = 0;

  int status = 0;
  *stat = status;

  nerr = 0;

}

GetRichPmtData::~GetRichPmtData()
{
  if (p)
    {
      delete p;
    }
  delete geo;
  //  delete rawadcval;
  //  delete rawadcpval;
  //  delete rawtacval;
  //  delete packetdata;
}

int GetRichPmtData::get_all_data_at_once(int ip)
{

  int ch_no;


  // Note that ich is set to zero in get_next_packet when each new packet
  // is opened

  // ip is set to -1 in initialization, so we have started a new
  // event, therefore we need a new packet
  //
  int status = get_next_packet(ip);
  if (status == 1)
    {
      return 1;
    }

  // get_packet is passed...
  //  fflush(stdout);

  // Since this is a new event, reread the run number
  run = evt->getRunNumber();

  edate = evt->getDate();
  etime = evt->getTime();

  int tac = 0, adcpost = 0, adcpre = 0;
  int event_id;

  // In this section, Check the data format!

  ch_no = 0;
  while (1)
    {

      if (ch_no >= 160)
        {
          break;
        }

      tac = p->iValue(ch_no, 0);   // TAC value
      adcpost = p->iValue(ch_no, 1);   // ADC post value
      adcpre = p->iValue(ch_no, 2);   // ADC pre value

      rawadcval[ch_no] = adcpost - adcpre;
      rawadcpval[ch_no] = adcpost;
      rawadcpreval[ch_no] = adcpre;
      rawtacval[ch_no] = tac;
      ch_no++;
    }

  event_id = p->iValue(0, "EVTNR");

  // data check for "ffff" clusters.
  int reg_adcpre[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int reg_adcpost[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int reg_tac[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  int and_adcpre = 0xcff;
  int and_adcpost = 0xcff;
  int and_tac = 0xcff;

  int ffff = 0, preffff = 0, postffff = 0, tacffff = 0;

  for (int chk = 0;chk < 160;chk++)
    {
      for (int reg = 7;reg > 0;reg--)
        {
          reg_adcpre[reg] = reg_adcpre[reg - 1];
          reg_adcpost[reg] = reg_adcpost[reg - 1];
          reg_tac[reg] = reg_tac[reg - 1];
        }

      reg_adcpre[0] = rawadcpreval[chk];
      reg_adcpost[0] = rawadcpval[chk];
      reg_tac[0] = rawtacval[chk];

      and_adcpre = 0x3ff;
      and_adcpost = 0x3ff;
      and_tac = 0x3ff;

      for (int reg = 0;reg < 8;reg++)
        {
          and_adcpre &= reg_adcpre[reg];
          and_adcpost &= reg_adcpost[reg];
          and_tac &= reg_tac[reg];
        }

      if (and_adcpre == 0x3ff)
        {
          for (int cls = 0;cls < 8;cls++)
            {
              reg_adcpre[cls] = 0;
            }
          ffff++;
          preffff++;
        }
      if (and_adcpost == 0x3ff)
        {
          for (int cls = 0;cls < 8;cls++)
            {
              reg_adcpost[cls] = 0;
            }
          ffff++;
          postffff++;
        }
      if (and_tac == 0x3ff)
        {
          for (int cls = 0;cls < 8;cls++)
            {
              reg_tac[cls] = 0;
            }
          ffff++;
          tacffff++;
        }
    }

  if (ffff > 20 || preffff > 8 || postffff > 8 || tacffff > 8)
    {
      pckt_err[nerr].err_type = 32;
      pckt_err[nerr].pckt = ip;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = ffff;
      nerr++;
    }
  else if (preffff > 3 || postffff > 3 || tacffff > 3)
    {
      pckt_err[nerr].err_type = 16;
      pckt_err[nerr].pckt = ip;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = ffff;
      nerr++;
    }
  else if (ffff > 0)
    {
      pckt_err[nerr].err_type = 8;
      pckt_err[nerr].pckt = ip;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = ffff;
      nerr++;
    }


  return 0;
}

int GetRichPmtData::get_next_packet(int fem_id)
{

  // If there is an existing packet pointer, free the memory
  if (p)
    {
      delete p;
      p = 0;
    }

  // Get the next packet and length of it

  if (!(p = evt->getPacket(fem_id)))
    { /* cout << "NO pacekt" << endl;*/
      return 1;
    }
  // Check the Hit Format
  Format = p->getHitFormat();

  // Used for packet error check
  int trail_err = 0;
  int head_err = 0;


  int flag_word, detector_id, event_id;
  int user_word[8];

  user_word[0] = p->iValue(0, "USERWORD");
  user_word[1] = p->iValue(1, "USERWORD");
  user_word[2] = p->iValue(2, "USERWORD");
  user_word[3] = p->iValue(3, "USERWORD");
  user_word[4] = p->iValue(4, "USERWORD");
  user_word[5] = p->iValue(5, "USERWORD");
  user_word[6] = p->iValue(6, "USERWORD");
  user_word[7] = p->iValue(7, "USERWORD");

  flag_word = p->iValue(0, "FLAG");
  event_id = p->iValue(0, "EVTNR");
  detector_id = p->iValue(0, "ID");

  for (int pck_chk = 0;pck_chk < 8;pck_chk++)
    {
      if (user_word[pck_chk] != (pck_chk + 1) * 0x10)
        {
          trail_err++;
        }
    }
  if (trail_err > 0)
    {
      pckt_err[nerr].err_type = 1;
      pckt_err[nerr].pckt = fem_id;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = trail_err;
      nerr++;
    }

  if (detector_id != 0x0600)
    {
      pckt_err[nerr].err_type = 2;
      pckt_err[nerr].pckt = fem_id;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = detector_id;
      nerr++;
      head_err++;
    }
  if (flag_word != 0x0063 )
    {
      pckt_err[nerr].err_type = 3;
      pckt_err[nerr].pckt = fem_id;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = flag_word;
      nerr++;
      head_err++;
    }
  if ((trail_err + head_err) > PERMIT_RICH_PACKET_ERROR)
    {
      pckt_err[nerr].err_type = 4;
      pckt_err[nerr].pckt = fem_id;
      pckt_err[nerr].evt = event_id;
      pckt_err[nerr].opt_info = trail_err + head_err;
      nerr++;
      return 1;
    }

  return 0;
}

int GetRichPmtData::get_pmt_data(int *pmtn, int *adcp, int *adcv, int *tacv
                                 , float *nh, int *sect, float *zed, float *ph)
{


  if (ich % 160 == 0)
    {
      int j;

      // Now, the software is remade to take all the data at once.

      for (j = 0;j < 160;j++)
        {
          rawadcval[j] = rawadcpval[j] = rawtacval[j] = -900;
        }
      int fem_id = (ich / 1280) * 10 + (ich % 1280) / 160 + 6001;
      get_all_data_at_once(fem_id);
    }


  int pmtnum = geo->get_pmtnum(ich % 160, ich / 160);

  // Here comes get_pmt_data...

  if (pmtnum < 0 || pmtnum > 5119)
    {
      // Some screwup in decoding the pmt number, punt on this event
      //Screwup in data decoding, skip rest of this event
      return 1;
    }

  // Now check adcval to see if it is a hit. If so, determine the location
  // of the hit in z and phi

  int sector = 0;
  float nhit = 0;
  float z = 0, phi = 0;
  if ( (float)rawadcval[ich % 160] > (pedestal[pmtnum] + 5*sigped[pmtnum]) && rawadcval[ich % 160] < 600 )
    {

      geo->get_pmt_coords(pmtnum, &sector, &z, &phi);

      if (pep[pmtnum] <= 5)
        {
          pep[pmtnum] = 80;
        }

      nhit = ((float)rawadcval[ich % 160] - pedestal[pmtnum]) / (pep[pmtnum] - pedestal[pmtnum]);

    }

  *pmtn = pmtnum;
  *adcv = rawadcval[ich % 160];
  *adcp = rawadcpval[ich % 160];
  *tacv = rawtacval[ich % 160];
  *nh = nhit;
  *sect = sector;
  *zed = z;
  *ph = phi;

  // increment the channel counter for next time
  ich++;

  return 0;
}

int GetRichPmtData::get_run_number(long *runout)
{

  *runout = run;

  return 0;
}

int GetRichPmtData::get_date(int *dt, int *tm)
{

  *dt = edate;
  *tm = etime;

  return 0;
}

int GetRichPmtData::decode_data_1X06(int rawtac, int rawadcpre, int rawadcpost, int *tac, int *adcval, int *adcpost, int *channel_no, int *adcpre)
{



  //
  // These are for new Format proposed by Mickey on May 31, 2001
  //  All the bits of data derived are inverted (Logical NOT)
  //
  *tac = rawtac & 0x03ff;      // Read the data in the first 10 bits
  *adcpre = rawadcpre & 0x03ff;
  *adcpost = rawadcpost & 0x03ff;

  *adcval = *adcpost - *adcpre;

  *channel_no = (rawtac & 0xfff00000) >> 20;

  return 0;
}

int GetRichPmtData::decode_data_406(int rawtac, int rawadcpre, int rawadcpost, int *tac, int *adcval, int *adcpost, int *channel_no, int *adcpre)
{


  int part_channelno;

  //
  // These are for new Format proposed by Mickey on May 31, 2001
  //  All the bits of data derived are inverted (Logical NOT)
  //
  *tac = rawtac & 0x03ff;      // Read the data in the first 10 bits
  *adcpre = rawadcpre & 0x03ff;
  *adcpost = rawadcpost & 0x03ff;

  *adcval = *adcpost - *adcpre;

  part_channelno = (rawtac & 0xfff00000) >> 20;
  *channel_no = ((part_channelno % 256) / 2 - 1) + (part_channelno / 256 - 1) * 32;

  return 0;
}
