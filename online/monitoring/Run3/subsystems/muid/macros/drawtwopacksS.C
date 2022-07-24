void drawtwopacksS(const char *filename = "run_98232_S.root")
{
TFile *f = TFile::Open(filename);

gStyle->SetTitleH(0.03);   
gStyle->SetTitleW(0.5);
const int ARM = 0; //0=south; 1=north
const char *ARMSTR[2] = {"SOUTH", "NORTH"};
const char *ORIENTSTR[2] = {"HORIZ", "VERT"};

int colors[5] = {kBlack, kRed, 8, kBlue, 28};//{kBlack,kRed,kBlue,kGreen,kYellow};
TGFrame* rootWin = (TGFrame*) gClient->GetRoot();
int display_sizex = rootWin->GetDefaultWidth();
int display_sizey = rootWin->GetDefaultHeight();
TCanvas* C1[2];
C1[0]=new TCanvas("c1_0","",0,0,display_sizex,display_sizey);
C1[1]=new TCanvas("c1_2","",0,0,display_sizex,display_sizey);
C1[0]->SetFillColor(kWhite);
C1[1]->SetFillColor(kWhite);
C1[0]->Divide(3,2);
C1[1]->Divide(3,2);
TH1* h1[5];

TLegend* legend = 0L;
char histname[128];
char info[128];
for(int iorient=0;iorient<2;iorient++){
  
  for(int ipanel=0;ipanel<6;ipanel++){
    for(int igap=0;igap<5;igap++){
      sprintf(histname,"MUI_twopack%d%d%d%d",ARM, igap, ipanel, iorient);
      h1[igap]=(TH1*)gROOT->FindObject(histname);
    }    
    int maxhits=0;
    int maxindex=0;
    for(int igap=0;igap<5;igap++){
      if(h1[igap]->GetMaximum()>maxhits){
        maxhits=h1[igap]->GetMaximum();
        maxindex=igap;
      }
    }

    C1[iorient]->cd(ipanel+1);
    h1[maxindex]->SetStats(false);
    // change title for first one
    sprintf(info,"Arm %d Orient %d Panel %d", ARM, iorient, ipanel);
    h1[maxindex]->SetTitle(info);
    h1[maxindex]->Draw();
    for(int igap=0;igap<5;igap++){
      h1[igap]->SetLineColor(colors[igap]);
      h1[igap]->Draw("same");
    }

    if (ipanel == 1){
      legend = new TLegend(0.65,0.65,0.88,0.88,"Hits");
      legend->AddEntry(h1[0], "Gap 0");
      legend->AddEntry(h1[1], "Gap 1");
      legend->AddEntry(h1[2], "Gap 2");
      legend->AddEntry(h1[3], "Gap 3");
      legend->AddEntry(h1[4], "Gap 4");
      legend->SetTextSize(0.036);
      //legend->SetLineWidth(3);
      legend->DrawClone();

      // arm and orient info also
      Float_t x = 45 + 5*(1-iorient);
      Float_t yarm = maxhits*0.45;
      Float_t yorient = maxhits*0.35;

      armtex = new TLatex(x,yarm,ARMSTR[ARM]);
      armtex->SetTextSize(0.05);
      armtex->SetLineWidth(2);
      armtex->Draw();

      orienttex = new TLatex(x,yorient,ORIENTSTR[iorient]);
      orienttex->SetTextSize(0.05);
      orienttex->SetLineWidth(2);
      orienttex->Draw();
    }
  }
  C1[iorient]->cd();
  Float_t yextra = 0.95;
  // add time
  TDatime dt;
  TText *tdate = new TText(0.99,yextra,dt.AsString());
  tdate->SetNDC();
  tdate->SetTextSize(0.02);
  tdate->SetTextAlign(31);
  tdate->Draw();
  
  // file info   
  TText *tfile = new TText(0.31,yextra,filename);
  tfile->SetNDC();
  tfile->SetTextSize(0.02);
  tfile->SetTextAlign(31);
  tfile->Draw();
  C1[iorient]->Update();
  C1[iorient]->Print();
}

}
