void graphsNV(const char *fname = "NorthVert.ps"){
  TCanvas *c1= new TCanvas("c1","North Vetical",10,10,1200,900);
  TPaveLabel *Label1 = new TPaveLabel (0.25,1.005,0.75,0.985,fname,"");
  Label1->SetBorderSize(0);
  Label1->SetFillColor(0); 
  Label1->Draw("");
  c1->Divide(4,5);
  c1->cd(1);
  MUI_NV00->Draw();
  c1->cd(2);
  MUI_NV01->Draw();
  c1->cd(3);
  MUI_NV02->Draw();
  c1->cd(4);
  MUI_NV03->Draw();
  c1->cd(5);
  MUI_NV10->Draw();
  c1->cd(6);
  MUI_NV11->Draw();
  c1->cd(7);
  MUI_NV12->Draw();
  c1->cd(8);
  MUI_NV13->Draw();
  c1->cd(9);
  MUI_NV20->Draw();
  c1->cd(10);
  MUI_NV21->Draw();
  c1->cd(11);
  MUI_NV22->Draw();
  c1->cd(12);
  MUI_NV23->Draw();
  c1->cd(13);
  MUI_NV30->Draw();
  c1->cd(14);
  MUI_NV31->Draw();
  c1->cd(15);
  MUI_NV32->Draw();
  c1->cd(16);
  MUI_NV33->Draw();
  c1->cd(17);
  MUI_NV40->Draw();
  c1->cd(18);
  MUI_NV41->Draw();
  c1->cd(19);
  MUI_NV42->Draw();
  c1->cd(20);
  MUI_NV43->Draw();
  c1->SaveAs(fname);
}



