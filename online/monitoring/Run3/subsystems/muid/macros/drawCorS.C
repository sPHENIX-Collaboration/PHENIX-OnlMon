{
//new TFile("laycor.root");
TCanvas *c1= new TCanvas("c1","Hits in Muon ID Panels",10,10,650,850);
   c1->Divide(2,4);

Option_t *option = "box";
bool saveflag = true;
const char *file_type = "ps";
const char *file_name = " ";

c1->cd(1);
MUI_COR_SH0_0->Draw(option);
c1->cd(2);
MUI_COR_SV0_0->Draw(option);
c1->cd(3);
MUI_COR_SH1_0->Draw(option);
c1->cd(4);
MUI_COR_SV1_0->Draw(option);
c1->cd(5);
MUI_COR_SH2_0->Draw(option);
c1->cd(6);
MUI_COR_SV2_0->Draw(option);
c1->cd(7);
MUI_COR_SH3_0->Draw(option);
c1->cd(8);
MUI_COR_SV3_0->Draw(option);
sprintf(file_name,"panel0.%s",file_type);
if (saveflag) c1->SaveAs(file_name);

//char fname = "testme.ps"

//  if (gROOT->IsBatch())  {
//    c1->Print("tempX.eps");
//    gSystem->Exec("pstopnm -ppm -xborder 0 -yborder 0 -portrait tempX.eps");
//    //char a;
//    //sprintf(a,"ppmtogif tempX.eps001.ppm>%s",fname);
//    //gSystem->Exec(a);
//    //gSystem->Exec("rm tempX.eps tempX.eps001.ppm");
//  } else {
//    c1->Print("panel0.ps");
//  }

TCanvas *c2= new TCanvas("c2","Hits in Muon ID Panels",10,10,650,850);
   c2->Divide(2,4);
c2->cd(1);
MUI_COR_SH0_1->Draw(option);
c2->cd(2);
MUI_COR_SV0_1->Draw(option);
c2->cd(3);
MUI_COR_SH1_1->Draw(option);
c2->cd(4);
MUI_COR_SV1_1->Draw(option);
c2->cd(5);
MUI_COR_SH2_1->Draw(option);
c2->cd(6);
MUI_COR_SV2_1->Draw(option);
c2->cd(7);
MUI_COR_SH3_1->Draw(option);
c2->cd(8);
MUI_COR_SV3_1->Draw(option);
sprintf(file_name,"panel1.%s",file_type);
if (saveflag) c2->SaveAs(file_name);

TCanvas *c3= new TCanvas("c3","Hits in Muon ID Panels",10,10,650,850);
   c3->Divide(2,4);
c3->cd(1);
MUI_COR_SH0_2->Draw(option);
c3->cd(2);
MUI_COR_SV0_2->Draw(option);
c3->cd(3);
MUI_COR_SH1_2->Draw(option);
c3->cd(4);
MUI_COR_SV1_2->Draw(option);
c3->cd(5);
MUI_COR_SH2_2->Draw(option);
c3->cd(6);
MUI_COR_SV2_2->Draw(option);
c3->cd(7);
MUI_COR_SH3_2->Draw(option);
c3->cd(8);
MUI_COR_SV3_2->Draw(option);
sprintf(file_name,"panel2.%s",file_type);
if (saveflag) c3->SaveAs(file_name);

TCanvas *c4= new TCanvas("c4","Hits in Muon ID Panels",10,10,650,850);
 c4->Divide(2,4);
c4->cd(1);
MUI_COR_SH0_3->Draw(option);
c4->cd(2);
MUI_COR_SV0_3->Draw(option);
c4->cd(3);
MUI_COR_SH1_3->Draw(option);
c4->cd(4);
MUI_COR_SV1_3->Draw(option);
c4->cd(5);
MUI_COR_SH2_3->Draw(option);
c4->cd(6);
MUI_COR_SV2_3->Draw(option);
c4->cd(7);
MUI_COR_SH3_3->Draw(option);
c4->cd(8);
MUI_COR_SV3_3->Draw(option);
sprintf(file_name,"panel3.%s",file_type);
if (saveflag) c4->SaveAs(file_name);

TCanvas *c5= new TCanvas("c5","Hits in Muon ID Panels",10,10,650,850);
   c5->Divide(2,4);
c5->cd(1);
MUI_COR_SH0_4->Draw(option);
c5->cd(2);
MUI_COR_SV0_4->Draw(option);
c5->cd(3);
MUI_COR_SH1_4->Draw(option);
c5->cd(4);
MUI_COR_SV1_4->Draw(option);
c5->cd(5);
MUI_COR_SH2_4->Draw(option);
c5->cd(6);
MUI_COR_SV2_4->Draw(option);
c5->cd(7);
MUI_COR_SH3_4->Draw(option);
c5->cd(8);
MUI_COR_SV3_4->Draw(option);
sprintf(file_name,"panel4.%s",file_type);
if (saveflag) c5->SaveAs(file_name);

TCanvas *c6= new TCanvas("c6","Hits in Muon ID Panels",10,10,650,850);
   c6->Divide(2,4);
c6->cd(1);
MUI_COR_SH0_5->Draw(option);
c6->cd(2);
MUI_COR_SV0_5->Draw(option);
c6->cd(3);
MUI_COR_SH1_5->Draw(option);
c6->cd(4);
MUI_COR_SV1_5->Draw(option);
c6->cd(5);
MUI_COR_SH2_5->Draw(option);
c6->cd(6);
MUI_COR_SV2_5->Draw(option);
c6->cd(7);
MUI_COR_SH3_5->Draw(option);
c6->cd(8);
MUI_COR_SV3_5->Draw(option);
sprintf(file_name,"panel5.%s",file_type);
if (saveflag) c6->SaveAs(file_name);


}



