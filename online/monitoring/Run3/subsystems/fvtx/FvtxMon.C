#include <FvtxMon.h>
#include <FvtxGeom.h>
#include <mFvtxUnpack.h>
#include <mFvtxUnpackPar.h>
#include <TFvtxHitMap.h>

#include <OnlMonServer.h>
#include <msg_profile.h>
#include <Event.h>

#include <TH1.h>
#include <TH2.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

FvtxMon::FvtxMon(): OnlMon("FVTXMON")
{
	int iret = 0;
	iret += initHistos();
	NumEvtIn = 0;

	unpacker_par = NULL;
	unpacker = NULL;
	hit_map = NULL;
}

FvtxMon::~FvtxMon()
{
	delete unpacker;
	delete unpacker_par;
	delete hit_map;
}

	int
FvtxMon::initHistos()
{
	ostringstream id, info;

	// let's register all these histos as we go
	OnlMonServer *Onlmonserver = OnlMonServer::instance();

	fvtxH1NumEvent = new TH1F("fvtxH1NumEvent",
			"Number of processed events",
			1, 0.5, 1.5);
	Onlmonserver->registerHisto(this,fvtxH1NumEvent);

	const char *CHARM[] = { "S", "N" }; // arm string id
	const int base_packet_number[NARMS] = {25000, 25100};

	for (int i = 0; i < NARMS; i++) 
	{
		for (int istation=0; istation<NSTATIONS; istation++)
		{
			int nchips = 26;
			if (istation==0) nchips = 10;
			hFvtxDisk[i][istation] = new TH2F(Form("hFvtxDisk%d_%d",i,istation),
					Form("Hit activity for %s arm - station %d; wedge; chip",CHARM[i],istation),
					48, -0.5, 47.5,
					nchips, -0.5, nchips-0.5);
			Onlmonserver->registerHisto(this,hFvtxDisk[i][istation]);

			hFvtxAdc[i][istation] = new TH1F(Form("hFvtxAdc%d_%d",i,istation),
					Form("%s - station %d; ADC value; counts",CHARM[i],istation),
					8, -0.5, 7.5);
			Onlmonserver->registerHisto(this,hFvtxAdc[i][istation]);
		}
		hFvtxYields[i] = new TH1F(Form("hFvtxYields%d",i), 
				Form("Yield per event vs. wedge for %s arm; wedge; N_{Hit}/N_{Event}",CHARM[i]),192, -0.5, 191.5);
		Onlmonserver->registerHisto(this,hFvtxYields[i]);
		hFvtxYieldsByPacket[i] = new TH1F(Form("hFvtxYieldsByPacket%d",i), 
				Form("Yield per event vs. packet for %s arm; packet number - %d; N_{Hit}",CHARM[i],base_packet_number[i]),24, 0.5, 24.5);
		Onlmonserver->registerHisto(this,hFvtxYieldsByPacket[i]);

		hFvtxControlWord[i] = new TH2F(Form("hFvtxControlWord%d",i),
				Form("FEM Control Word for %s arm; packet number - %d; bit",CHARM[i],base_packet_number[i]),
				24, 0.5, 24.5,
				16, -0.5, 15.5);
		Onlmonserver->registerHisto(this,hFvtxControlWord[i]);

		hFvtxYieldsByPacketVsTime[i] = new TH2F(Form("hFvtxYieldsByPacketVsTime%d",i),
				Form("Yield per event of packets vs. time for %s arm; time(min) ; packet number - %d",CHARM[i],base_packet_number[i]),
				960, 0, 480,
				24, 0.5, 24.5);
		Onlmonserver->registerHisto(this,hFvtxYieldsByPacketVsTime[i]);

		hFvtxYieldsByPacketVsTimeShort[i] = new TH2F(Form("hFvtxYieldsByPacketVsTimeShort%d",i),
				Form("Yield per event of packets vs. time for %s arm; time(s) ; packet number - %d",CHARM[i],base_packet_number[i]),
				600, 0, 600,
				24, 0.5, 24.5);
		Onlmonserver->registerHisto(this,hFvtxYieldsByPacketVsTimeShort[i]);
	}

	hFvtxEventNumberVsTime = new TH1F(Form("hFvtxEventNumberVsTime"),
			Form("EventNumberVsTime"),
			960, 0, 480);
	Onlmonserver->registerHisto(this,hFvtxEventNumberVsTime);

	hFvtxEventNumberVsTimeShort = new TH1F(Form("hFvtxEventNumberVsTimeShort"),
			Form("EventNumberVsTimeShort"),
			600, 0, 600);
	Onlmonserver->registerHisto(this,hFvtxEventNumberVsTimeShort);

	// CAA 512 = 128 channels * 4 wedges per ROC; 104 = 26 chips * 4 wedges per ROC
	for (int iroc=0; iroc<NROCS; iroc++) 
	{
		hFvtxChipChannel[iroc] = new TH2F(Form("hFvtxChipChannel_ROC%d",iroc),
				Form("Hits for ROC %d; channel; chip",iroc),
				512, -0.5, 511.5,
				104, -0.5, 103.5);
		Onlmonserver->registerHisto(this,hFvtxChipChannel[iroc]);
	}





	return 0;
}

	int
FvtxMon::BeginRun(const int runno)
{

	// This part is no longer needed since we use calibration database for geometry now
	//
	//  ostringstream calibdir;
	//  calibdir << getenv("FVTXCALIBDIR") << "/";
	//
	//  FvtxGeom::set_public_file_path(calibdir.str().c_str());

	if (!unpacker_par)
	{
		unpacker_par = new mFvtxUnpackPar();
		unpacker_par->set_verbosity((FVTXOO::Verbosity) verbosity);
	}

	if (!unpacker)
	{
		unpacker = new mFvtxUnpack();
		if (!unpacker)
			cout << "FvtxMon::BeginRun - Error loading mFvtxUnpack" << endl;

		unpacker->set_unpacker_par(unpacker_par);

		if (!unpacker -> save_online_info())
			cout <<"FvtxMon::BeginRun - mFvtxUnpack initiate online information buffer"<<endl;
	}

	if (!hit_map)
	{
		hit_map = new TFvtxHitMap();
		if (!hit_map)
			cout << "Error loading mFvtxUnpack" << endl;
	}


	return 0;
}

int FvtxMon::process_event(Event *evt)
{

	NumEvtIn++;
	if (verbosity>2) cout << "FvtxMon::process_event("<<NumEvtIn<<")"<<endl;

	fvtxH1NumEvent->Fill(1); // one entry per event

	if (!unpacker)
	{
		cout << "FvtxMon::process_event("<<NumEvtIn<<") - Error loading mFvtxUnpack"<<endl;
		return -1;
	}

	if (verbosity>2) cout << "FvtxMon::process_event("<<NumEvtIn<<") - Unpacking"<<endl;
	unpacker->process_event(evt, hit_map);
	if (verbosity > 2)
		cout << "FvtxMon::process_event(" << NumEvtIn << ") - Unpacking Done"
			<< endl;


	time_t time = evt->getTime();
	if(start_time < 1) 
	{
		start_time = time;
	}

	hFvtxEventNumberVsTime->Fill((time - start_time)/60.);
	if(time - start_time <= 600)hFvtxEventNumberVsTimeShort->Fill((time - start_time));


	for(int ipacket = 0; ipacket<48; ipacket ++)//loop through all the packets to look for FEM errors
	{
		int this_arm = 0;
		if(ipacket>23){this_arm = 1;}
		const int fem_error = unpacker->get_femerror(ipacket);//this wants an int between 0-48, need to convert it to actual packet later

		if(fem_error !=0)
		{
			for(int j = 0; j<13; j++)//shift through the bits of the FEM control word (13,14,15 not currently used)
			{
				bool set = ( fem_error & (1 << j ) )> 0 ;//bit shift through the error code and see what we get

				//convert from the goofy ipacket to the actual packet number
				int real_packet = 0;
				if (ipacket < 0 || ipacket >= 48) real_packet = 0;
				else if (ipacket < 24) {real_packet = 25001 + ipacket;}
				else {real_packet = 25101 + ipacket - 24;}

				if (this_arm==0 && set) hFvtxControlWord[this_arm]->Fill(real_packet - 25000,j);
				else if (this_arm==1 && set) hFvtxControlWord[this_arm]->Fill(real_packet - 25100,j);

			}
		}
	}


	for (int ihit = 0; ihit < unpacker->get_n_raw_hits(); ihit++)
	{
		const int arm = unpacker->get_arm(ihit);
		const int cage = unpacker->get_cage(ihit);
		const int station = unpacker->get_station(ihit);
		const int sector = unpacker->get_sector(ihit);
		const int chip = unpacker->get_chip(ihit);
		const int adc = unpacker->get_adc_value(ihit);
		const int channel = unpacker->get_channel(ihit);
		const int packet = unpacker->get_packet(ihit);  
		// roc goes from 0 to 23
		// Ordering of ROCs goes physically arm 0, cage 0 (SW), arm 0, cage 1 (SE), arm 1, cage 0 (NW), arm 1, cage 1 (NE)
		int roc = (arm*48 + cage*24 + sector)/4;
		int wedgeInStation = cage*24+sector;  // 0-47
		int wedgeInArm = station*48 + cage*24+sector; //48 wedges per station

		if (arm < 0 || arm >= 2 || station < 0 || station >= 4)
		{
			if (verbosity > 1)
			{
				cout << "FvtxMon::process_event(" << NumEvtIn
					<< ") - Error : wrong hit with arm (" << arm << ") - station (" << station
					<< ")" << endl;
			}
			continue;
		}

		if (adc>0) {
			hFvtxDisk[arm][station]->Fill(wedgeInStation, (chip-1)%26);
			hFvtxAdc[arm][station]->Fill(adc);
			hFvtxYields[arm]->Fill(wedgeInArm);
			if (arm==0) hFvtxYieldsByPacket[arm]->Fill(packet - 25000);
			else if (arm==1) hFvtxYieldsByPacket[arm]->Fill(packet - 25100);
			hFvtxChipChannel[roc]->Fill((sector%4)*128 + channel, station*26 + (chip-1)%26); 
		} 

		if (adc>1) {
			//cout<<"DEBUG: "<<" adc: "<<adc<<" arm: "<<arm<<" start_time: "<<start_time<<" time: "<<time<<" packet: "<<packet<<endl;
			if (arm==0) hFvtxYieldsByPacketVsTime[arm]->Fill((time - start_time)/60., packet - 25000);
			else if (arm==1) hFvtxYieldsByPacketVsTime[arm]->Fill((time - start_time)/60., packet - 25100);
		}

		if(time - start_time <= 600){
			if (arm==0) hFvtxYieldsByPacketVsTimeShort[arm]->Fill((time - start_time), packet - 25000);
			else if (arm==1) hFvtxYieldsByPacketVsTimeShort[arm]->Fill((time - start_time), packet - 25100);
		}
	}

	return 0;
}

	int
FvtxMon::Reset()
{
	NumEvtIn = 0;
	start_time = 0;
	return 0;
}
