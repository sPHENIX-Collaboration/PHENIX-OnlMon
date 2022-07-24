#ifndef __FVTXMON_H__
#define __FVTXMON_H__


#include <OnlMon.h>
#include <FvtxMonConsts.h>


class mFvtxUnpack;
class mFvtxUnpackPar;
class TFvtxHitMap;

class Event;

class TH1;
class TH2;

class FvtxMon: public OnlMon
{
	public:
		FvtxMon();
		virtual ~FvtxMon();
		// required
		int process_event(Event *evt);
		int BeginRun(const int runno);
		int Reset();

		int initHistos();

	protected:
		int NumEvtIn, NumEvtAna;

		time_t start_time;

		// local histo pointers
		TH2* hFvtxDisk[NARMS][NSTATIONS]; 
		TH1* hFvtxAdc[NARMS][NSTATIONS];  
		TH1* fvtxH1NumEvent;
		TH1* hFvtxYields[NARMS];  
		TH1* hFvtxYieldsByPacket[NARMS]; // For each arm
		TH2* hFvtxChipChannel[NROCS]; // For each ROC
		TH2* hFvtxControlWord[NARMS]; //
		TH2* hFvtxYieldsByPacketVsTime[NARMS]; //
		TH1* hFvtxEventNumberVsTime; //
		TH2* hFvtxYieldsByPacketVsTimeShort[NARMS]; //
		TH1* hFvtxEventNumberVsTimeShort; //

		mFvtxUnpackPar * unpacker_par;
		mFvtxUnpack * unpacker;
		TFvtxHitMap * hit_map;
};

#endif /* __FVTXMON_H__ */
