void graphsSH(const char *fname = "SouthHoriz.ps"){
  TCanvas *c1= new TCanvas("c1","South Horizontal",10,10,1200,900);
  TPaveLabel *Label1 = new TPaveLabel (0.25,1.005,0.75,0.985,fname,"");
  Label1->SetBorderSize(0);
  Label1->SetFillColor(0); 
  Label1->Draw("");
  c1->Divide(4,5);
  c1->cd(1);
  MUI_SH00->Draw();
  c1->cd(2);
  MUI_SH01->Draw();
  c1->cd(3);
  MUI_SH02->Draw();
  c1->cd(4);
  MUI_SH03->Draw();
  c1->cd(5);
  MUI_SH10->Draw();
  c1->cd(6);
  MUI_SH11->Draw();
  c1->cd(7);
  MUI_SH12->Draw();
  c1->cd(8);
  MUI_SH13->Draw();
  c1->cd(9);
  MUI_SH20->Draw();
  c1->cd(10);
  MUI_SH21->Draw();
  c1->cd(11);
  MUI_SH22->Draw();
  c1->cd(12);
  MUI_SH23->Draw();
  c1->cd(13);
  MUI_SH30->Draw();
  c1->cd(14);
  MUI_SH31->Draw();
  c1->cd(15);
  MUI_SH32->Draw();
  c1->cd(16);
  MUI_SH33->Draw();
  c1->cd(17);
  MUI_SH40->Draw();
  c1->cd(18);
  MUI_SH41->Draw();
  c1->cd(19);
  MUI_SH42->Draw();
  c1->cd(20);
  MUI_SH43->Draw();
  c1->SaveAs(fname);
}



