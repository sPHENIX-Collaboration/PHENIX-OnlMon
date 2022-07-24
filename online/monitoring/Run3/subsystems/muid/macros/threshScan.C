{
//Currently limited to 10mV steps
const int FILES = 8;
const int MAXTHRESH = 120; //Highest threshhold
const int ARM = 1; //0=south; 1=north

/* South files
10 mV steps; higher run number means higher threshold
run_98104_S.root
run_98103_S.root
run_98102_S.root
run_98101_S.root
run_98100_S.root
run_98099_S.root
run_98098_S.root
run_98097_S.root

North files
run_98122_N.root
run_98121_N.root
run_98119_N.root
run_98118_N.root
run_98117_N.root
run_98116_N.root
run_98115_N.root
run_98114_N.root
*/
char fnames[FILES][128];
sprintf(fnames[0],"run_98122_N.root");//120mv
sprintf(fnames[1],"run_98121_N.root");//110mv
sprintf(fnames[2],"run_98119_N.root");//100mv
sprintf(fnames[3],"run_98118_N.root");//90mv
sprintf(fnames[4],"run_98117_N.root");//80mv
sprintf(fnames[5],"run_98116_N.root");//70mv
sprintf(fnames[6],"run_98115_N.root");//60mv
sprintf(fnames[7],"run_98114_N.root");//50mv

TH1F *h1 = new TH1F("h1","Noise Scan: ",FILES,MAXTHRESH-FILES*10+5,MAXTHRESH+5);
TH1F *h2 = new TH1F("h2","Noise Scan: Operational HV with Beam",FILES,MAXTHRESH-FILES*10+5,MAXTHRESH+5);
TH1F *h3 = new TH1F("h3","Noise Scan: Operational HV with Beam",FILES,MAXTHRESH-FILES*10+5,MAXTHRESH+5);

TH1F *VAclone[FILES];
TH1F *HAclone[FILES];

int maxhits = 10;
int maxhits2 = 100;
int maxhits3 = 1000;


TFile *f;
char histName[128];
int thresh = MAXTHRESH;
for(int j=0; j < FILES; j++){
  f = new TFile(fnames[j],"READ");
  TH1F* HA;
  TH1F* VA;
  if (ARM == 0){
    HA = MUI_SHA;   
    VA = MUI_SVA;
  } else {
    HA = MUI_NHA;
    VA = MUI_NVA;
  }

  VAclone[j]=(TH1F*)VA->Clone();
  sprintf(histName,"VA_%d",thresh);
  VAclone[j]->SetName(histName);
  VAclone[j]->SetDirectory(gROOT);
  HAclone[j]=(TH1F*)HA->Clone();
  sprintf(histName,"HA_%d",thresh);
  HAclone[j]->SetName(histName);
  HAclone[j]->SetDirectory(gROOT);

  long events = MUI_NumEvent->GetEntries();
  cout<<fnames[j]<<" had "<< events <<" events"<<endl;
  VAclone[j]->Scale(1./(float)events);
  HAclone[j]->Scale(1./(float)events);

  maxhits = int(10*events/50000.+0.5);
  maxhits2 = int(100*events/50000.+0.5);
  maxhits3 = int(1000*events/50000.+0.5);
  //There at 1920 channels per FEM
  for(int i=1; i < 1921; i++){
    if(HA->GetBinContent(i)> = maxhits){
      h1->Fill(thresh);
    }
    if(VA->GetBinContent(i)> = maxhits){
      h1->Fill(thresh);
    }
    if(HA->GetBinContent(i)> = maxhits2){
      h2->Fill(thresh);
    }
    if(VA->GetBinContent(i)> = maxhits2){
      h2->Fill(thresh);
    }
    if(HA->GetBinContent(i)> = maxhits3){
      h3->Fill(thresh);
    }
    if(VA->GetBinContent(i)> = maxhits3){
      h3->Fill(thresh);
    }

  }
  f->Close();
  thresh = thresh - 10;
}

//3150 is the total number of active channels in an arm.
//This assumes empty channels won't be noisy. This is
//probably wrong at low Vth.

h1->Scale(1.0/3150.0);
h2->Scale(1.0/3150.0);
h3->Scale(1.0/3150.0);
h1->GetXaxis()->SetTitle("Threshold (mV)");
h1->GetYaxis()->SetTitle("Fraction of Chanels With Noise");
h1->SetStats(false);
h1->GetXaxis()->CenterTitle(true);
h1->GetYaxis()->CenterTitle(true);
//TAxis *Xaxis = h1->GetXaxis();
//double tempX = Xaxis->GetX1();
//Xaxis->SetX1(Xaxis->GetX2());
//Xaxis->SetX2(tempX);
TCanvas *c1 = new TCanvas("c1");
c1->SetFrameBorderMode(0);
c1->SetBorderMode(0);
c1->SetFillColor(kWhite);
h1->SetFillColor(kWhite); 
h1->SetMaximum(1);
TLegend* legend = new TLegend(0.7,0.71,0.88,0.88,"Noise Rates");
legend->AddEntry(h1, "> 1KHz");
legend->AddEntry(h2, "> 10KHz");
legend->AddEntry(h3, "> 100KHz");
h1->SetLineWidth(2);
h2->SetLineWidth(2);
h3->SetLineWidth(2);
h1->Draw();
h2->SetLineColor(kRed);
h2->Draw("same");
h3->SetLineColor(kGreen);
h3->Draw("same");
legend->Draw();
c1->SaveAs("noise.eps");

TCanvas *c2 = new TCanvas("c2");
c2->SetFillColor(kWhite);
c2->Divide(1,2);
c2->cd(1);
int maxentriesH=0;
int maxentriesV=0;
int maxindexH =0;
int maxindexV =0;
for (int i=0;i<FILES;i++){
  if(VAclone[i]->GetEntries()>maxentriesV){
    maxentriesV=VAclone[i]->GetEntries();
    maxindexV=i;
  }
  if(HAclone[i]->GetEntries()>maxentriesH){
    maxentriesH=HAclone[i]->GetEntries();
    maxindexH=i;
  }
}
VAclone[maxindexV]->SetFillColor(kWhite);
VAclone[maxindexV]->Draw();
c2->cd(2);
HAclone[maxindexH]->SetFillColor(kWhite);
HAclone[maxindexH]->Draw();
int colors[12]={1,2,3,4,5,6,7,8,9,11,12,13};

TLegend* legend = new TLegend(0.7,0.71,0.88,0.88,"Vth");
for (int i=0;i<FILES;i++){
  c2->cd(1);
  VAclone[i]->SetLineColor(colors[i]);
  VAclone[i]->Draw("Same");
  sprintf(histName,"%dmV",(MAXTHRESH-10*i));
  legend->AddEntry(VAclone[i],histName );
  c2->cd(2);
  HAclone[i]->SetLineColor(colors[i]);
  HAclone[i]->Draw("Same");
}

c2->cd(1);
legend->DrawClone();
c2->cd(2);
legend->DrawClone();
}



