#include <iostream>
#include <fstream>

// please modify input root file (reference run) name

int Write_SvxPixelNominalRate() {

  TFile *fin = new TFile("/common/s6/backup/OnlMon/rootfiles/done/Run_443522_SVXPIXELMON_va039_9081.root");
  ofstream fout("SvxPixelNominalRate.txt");

  char multi_time_name[128];
  TProfile *vtxp_multi_time[60];

  for (int i=0; i<60; i++) {
    sprintf(multi_time_name,Form("vtxp_multi_time_%d",i));
    vtxp_multi_time[i] = (TProfile*)fin->Get(multi_time_name);
    if (i == 0) fout << "#PACKETID" << "\t" << "RATE" << endl;
    fout << i << "\t" << vtxp_multi_time[i]->GetMean(2) << endl;
    //cout << i << "\t" << vtxp_multi_time[i]->GetMean(2) << endl;
    //vtxp_multi_time[i]->Draw();
  }

  return 0;
}
