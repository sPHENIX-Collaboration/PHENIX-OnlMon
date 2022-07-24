void graphsSV(const char *fname = "SouthVert.ps"){
  TCanvas *c1= new TCanvas("c1","South Vetical",10,10,1200,900);
  TPaveLabel *Label1 = new TPaveLabel (0.25,1.005,0.75,0.985,fname,"");
  Label1->SetBorderSize(0);
  Label1->SetFillColor(0); 
  Label1->Draw("");
  c1->Divide(4,5);
  c1->cd(1);
  MUI_SV00->Draw();
  c1->cd(2);
  MUI_SV01->Draw();
  c1->cd(3);
  MUI_SV02->Draw();
  c1->cd(4);
  MUI_SV03->Draw();
  c1->cd(5);
  MUI_SV10->Draw();
  c1->cd(6);
  MUI_SV11->Draw();
  c1->cd(7);
  MUI_SV12->Draw();
  c1->cd(8);
  MUI_SV13->Draw();
  c1->cd(9);
  MUI_SV20->Draw();
  c1->cd(10);
  MUI_SV21->Draw();
  c1->cd(11);
  MUI_SV22->Draw();
  c1->cd(12);
  MUI_SV23->Draw();
  c1->cd(13);
  MUI_SV30->Draw();
  c1->cd(14);
  MUI_SV31->Draw();
  c1->cd(15);
  MUI_SV32->Draw();
  c1->cd(16);
  MUI_SV33->Draw();
  c1->cd(17);
  MUI_SV40->Draw();
  c1->cd(18);
  MUI_SV41->Draw();
  c1->cd(19);
  MUI_SV42->Draw();
  c1->cd(20);
  MUI_SV43->Draw();
  c1->SaveAs(fname);
}



