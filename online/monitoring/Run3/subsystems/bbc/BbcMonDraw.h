#ifndef __BBCMONDRAW_H__
#define __BBCMONDRAW_H__


#include "BbcMonDefs.h"
#include <OnlMonDraw.h>



#define MAX_WARNING 16

class BbcCalib;
class TArc;
class TArrow;
class TBox;
class TCanvas;
class TF1;
class TGraph;
class TGraphErrors;
class TH1;
class TH2;
class TLatex;
class TLine;
class TPad;
class TPaveText;
class TText;
class TSpectrum;
class TStyle;

class BbcMonDraw: public OnlMonDraw
{

  public:
    BbcMonDraw(const char *name = "BbcMON");
    virtual ~BbcMonDraw();

    int Init();
    int MakeCanvas(const char *name);
    int Draw(const char *what = "ALL");
    int MakePS(const char *what = "ALL");
    int MakeHtml(const char *what = "ALL");


    /* Variable Functions */


  protected:
    TStyle *bbcStyle;

  private:
    int DrawDeadServer(TPad *transparent);
    int CreateLabel();
    int Warning( TPad * pad, const float x, const float y, const int r, const std::string& msg);
    int ClearWarning( void );

    void CalculateTriggerRates(float &trig_rate0, float &trig_rate_err0,double &nevent0,
	float &trig_rate1, float &trig_rate_err1,double &nevent1,
	float &trig_rate2, float &trig_rate_err2,double &nevent2);

    void BeamMonitoring(float &sigma_zdc, float &sigma_zdc_err,float &effic_bbc, float &effic_bbc_err,float &beamInZdc, float &beamInZdc_err,float &beamInBbc, float &beamInBbc_err);



    BbcCalib *bbccalib;

    TCanvas *TC[nCANVAS];
    TPad * transparent[nCANVAS];

    // ------------------------------------------------------
    // Graph for visualization


    // for all Pages
    TPad * PadTop[nCANVAS];
    TPaveText * PaveTop;
    TText * TextTop;

    TPaveText * PaveWarning[MAX_WARNING];
    TArc * ArcWarning[MAX_WARNING];
    TPad * PadWarning[MAX_WARNING];
    int nPadWarning[MAX_WARNING];
    int nWarning;

    // for the 1st Page
    TGraphErrors * TdcOver[nTDC][nSIDE];
    TPad * PadTdcOver[nTDC][nSIDE];
    TLine * LineTdcOver[nTDC][nSIDE][2];
    TBox * BoxTdcOver[nTDC][nSIDE];
    TH1 *FrameTdcOver[nTDC][nSIDE];

    TGraph * nHit[nTRIGGER][nSIDE];
    TPad * PadnHit [nSIDE];
    TLine *LinenHit[nTRIGGER][nSIDE][2];
    TBox * BoxnHit[nTRIGGER][nSIDE];
    TPad * PadnHitStatus;
    TText *TextnHitStatus;
    TH1 *FramenHit[nSIDE];

    // for 2nd Page
    TH1 * HitTime[nTDC][nSIDE];
    TPad * PadHitTime[nTDC][nSIDE];
    TLine * LineHitTime[nTDC][nSIDE][2];
    TArrow *ArrowHitTime[nTDC][nSIDE];
    TText * TextHitTime[nTDC][nSIDE];
    TF1 * FitHitTime[nTDC][nSIDE];

    TH1        * AvrHitTime;
    TPad    * PadAvrHitTime;
    TLine  * LineAvrHitTime[2];
    TArrow *ArrowAvrHitTime;
    TText  * TextAvrHitTime;
    TF1     * FitAvrHitTime;

    TH1        * SouthHitTime;
    TPad    * PadSouthHitTime;
    TLine  * LineSouthHitTime[2];
    TArrow *ArrowSouthHitTime;
    TText  * TextSouthHitTime;
    TF1     * FitSouthHitTime;

    TH1        * NorthHitTime;
    TPad    * PadNorthHitTime;
    TLine  * LineNorthHitTime[2];
    TArrow *ArrowNorthHitTime;
    TText  * TextNorthHitTime;
    TF1     * FitNorthHitTime;

    TH1        * SouthChargeSum;
    TH1        * NorthChargeSum;
    TPad    * PadChargeSum;
    TText  * TextSouthChargeSum;
    TText  * TextNorthChargeSum;

    TLine  * LineTzeroZvtx[4];
    TText  * TextTzeroZvtx;

    TPad * PadBbcSummary;
    //TText * TextBbcSummaryHitTime;
    TText * TextBbcSummaryHitTime[2];
    TText * TextBbcSummaryGlobalOffset[2];
    TText * TextBbcSummaryZvertex;
    //TText * TextBbcSummaryTrigRate;
    TLatex * TextBbcSummaryTrigRate;

    TH1 * Zvtx;
    TH1 * Zvtx_bbll1;
    TH1 * Zvtx_zdc;
    TH1 * Zvtx_zdc_scale3;
    TH1 * Zvtx_bbll1_novtx;
    TH1 * Zvtx_bbll1_narrowvtx;
    TH1 * Zvtx_bbll1_zdc;
    TPad * PadZvtx;
    TF1 * FitZvtx;
    TLine * LineZvtx[2];
    TArrow *ArrowZvtx;
    TText * TextZvtx;
    TText * TextZvtxNorth;
    TText * TextZvtxSouth;

    TH2 * ArmHit;
    TPad * PadArmHit;
    TText * TextArmHit;
    TArc * ArcArmHit;

    // for 3rd Page
    // It is for Only Expert
    TH2 * Adc;
    TPad *PadAdc;

    TPad * PadButton;
    TPad * PadMultiView;
    TH1 * MultiView1F;
    TH2 * MultiView2F;
    TPad * PadWarnings;
    TPaveText * PaveWarnings;

    // for 4th Page
    // Vertex Monitor
    TPad  * PadZVertex;
    TPad  * PadTzeroZVertex;
    TPad  * PadZVertexSummary;
    TText * TextZVertexExpress;
    TText * TextZVertexNotice;

    //  TText * TextZVertex[3];
    //TText * TextZVertex_scale[3];
    //TText * TextZVertex[4];
    //TText * TextZVertex_scale[4];
    //TText * TextZVertex_mean[4];
    TText * TextZVertex[5];//RUN11 pp
    TText * TextZVertex_scale[5];//RUN11 pp
    TText * TextZVertex_mean[5];//RUN11 pp

    TF1 *FitZvtxBBLL1NoVtx;
    TLatex *TextZVtxStatus[2];

    TH2 * TzeroZvtx;

    // Scale down factor
    TH1 * Prescale_hist;
    TSpectrum *tspec;
};

#endif /*__BBCMONDRAW_H__ */
