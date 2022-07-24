#ifndef __FVTXMONDRAW_H__
#define __FVTXMONDRAW_H__

#include <FvtxMonConsts.h>
#include <FvtxMonDrawConsts.h>
#include <OnlMonDraw.h>

#include <set> // for the static bad FEMs list

class TPie;
class TCanvas;
class TLatex;
class TLine;
class TText;
class TPad;
class TPaveText;
class TH1;
class TH2;

class RunDBodbc;

class FvtxMonDraw: public OnlMonDraw
{

	public: 
		FvtxMonDraw();
		virtual ~FvtxMonDraw();

		int Init();
		void registerHists();
		int Draw(const char *what = "ALL");
		int MakePS(const char *what = "ALL");
		int MakeHtml(const char *what = "ALL");
		static const int NROCSperSTATION=12; 
		std::string rocnames[NROCSperSTATION];

	private:
		int MakeCanvas(const char *name);
		int DrawSummary(const char *what = "ALL", int arm = 0);
		int DrawYieldsByWedge(const char *what = "ALL", int arm = 0);
		int DrawYieldsByPacket(const char *what = "ALL", int arm = 0);
		int DrawYieldsByPacketVsTime(const char *what = "ALL", int arm = 0);
		int DrawYieldsByPacketVsTimeShort(const char *what = "ALL", int arm = 0);
		int DrawAdc(const char *what = "ALL", int arm = 0);
		int DrawChipVsChannel(const char *what = "ALL", int arm = 0, int side = 0);
		int DrawControlWord(const char *what = "ALL", int arm = 0);

		int DrawDeadServer(TPad *transparent);


		void MakeMark(const int DCM = 0);
		void MakeMark_cw(const int DCM = 0);

		void MakeMarkVsTime(const int DCM = 0);

		std::set <int> badfemset;

		static const int ncanvas = 18;//add 2 for FEM control word =14
		int npad[ncanvas];
		static const int nmaxpad = 6;
		TCanvas *TC[ncanvas];
		TPad *TP[ncanvas][nmaxpad];
		TPad *transparent[ncanvas];
		std::string canv_name[ncanvas]; // Global name (must be unique)
		std::string canv_desc[ncanvas]; // Canvas description

		TPie *pieFvtxDisk;
		TPie *pieROCname;
		TPie *pieCenter;
		TPaveText *ptArmStation[NARMS][NSTATIONS];
		TPaveText *tsta0;
		TPaveText *tsta1;
		TPaveText *tsta2;
		TPaveText *tsta3;
		TLine *line1;
		TLine *line2;
		TLine *line3; 
		TLine *line_ccs1; 
		TLine *line_ccs2; 
		TLine *line_ccs3; 
		TLine *line_ccs4; 
		TLine *line_ccs5; 
		TLine *line_ccs6; 
		TLine *lineROC[12];
		TText *textROC[NARMS][12];

		TLine *lineROC_cw[12];
		TText *textROC_cw[NARMS][12];
		TText *text_cw_bit[13];

		TLine *lineROC_time[12];
		TText *textROC_time[NARMS][12];

		int m_run_number;
		std::string m_run_type;

		RunDBodbc *rundb_odbc;
};

#endif /*__FVTXMONDRAW_H__ */
