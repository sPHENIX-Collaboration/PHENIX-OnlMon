int ana_root( char* histo = "MUI_CABLE_H", int channel = 0, 
	      char* title = "Hit Rate",
	      int run_number_first = 4,
	      int run_number_last = 6 , TH1F* h1) {

  int  max_bin = run_number_last - run_number_first + 1;
  
  Int_t entries = 0;
  TH1F* rate = new TH1F("rate",title,max_bin,
			run_number_first - 0.5,
			run_number_last + 0.5);
  rate->SetXTitle("LVL-1 delay (BCLK)");  
  rate->SetYTitle("Hit rate");  
  rate->SetLineWidth(2);  

  Char_t root_file[50];


  for( Int_t run_number = run_number_first;
       run_number<=run_number_last; run_number++ ) {
    sprintf( root_file, "root/run%d.root", run_number );
    TFile* f = new TFile( root_file );

    if (gROOT->FindObject( "MUI_NumEvent" )) {
      entries = ((TH1F*)gROOT->FindObject( "MUI_NumEvent" ))->GetBinContent(1);
    } else {
      entries = 0;
    }
//      cout << " Run number = " << run_number 
//  	 << " number of events is " << entries << endl;
    TH1F* htmp = (TH1F*)gROOT->FindObject( histo );
    if( !htmp ) {
      cout << " no such histogram " <<  histo << endl;
      f->Close();
      exit(-1);
    }

    rate->SetBinContent( run_number - run_number_first +1, htmp->GetBinContent(channel+1)/entries);
    rate->SetBinError( run_number - run_number_first +1, sqrt(htmp->GetBinContent(channel+1))/entries);
    
    f->Close();
  }

  if (rate->Integral()>0.){
    double scaleFactor = 1./rate->Integral();
    rate->Scale(scaleFactor);
  }

//   cout<< "rate->GetMaximumBin() "<<rate->GetMaximumBin()<<endl;
  h1->Fill(rate->GetMean());
  rate->DrawCopy();
  delete rate;
}

void time_scan(int arm = 0, int run_number_first = 8, int run_number_last = 10){


TCanvas* c1 = new TCanvas("c1","new canvas", 1100, 850);
 c1->SetFillColor(10);
 char* orient;
 char word[50];
 char title[50];

//  gStyle->SetTitleH(0.07);
//  gStyle->SetTitleW(0.6);
//  gStyle->SetTitleXSize(0.05);
//  gStyle->SetTitleYSize(0.05);
//  gStyle->SetTitleXOffset(0.025);
//  gStyle->SetTitleYOffset(0.025);

 TH1F* h1 = new TH1F("h1","ALL",40,8.5,12.5);
 for (int plane=0; plane < 5; plane++){
   for (int orientation=0; orientation<2;orientation++){
     c1->Clear();
     c1->Divide(6,4,0,0);
     if( orientation == 0 ) {
       orient = "_H";
     }
     else {
       orient = "_V";
     }
     
     int chan = plane * 24;
     for (int i=1; i< 25; i++){
       c1->cd(i);
       sprintf( title, "ROC%d,Cable%d,%s", chan/6, chan%6, orient );
       if (arm ==0){
	 sprintf( word, "MUI_CABLE%s", orient);
       } else {
	 sprintf( word, "MUI_N_CABLE%s", orient);
       }
       ana_root(word, chan, title, run_number_first, run_number_last, h1);
       chan++;
     }
     sprintf( title, "plots/Time_scan%s%d.ps",orient, plane);
     c1->SaveAs(title);
     sprintf( title, "plots/Time_scan%s%d.gif",orient, plane);
     c1->SaveAs(title);
 
   }
 }

//   TCanvas* c2 = new TCanvas("c2","new canvas", 400, 400);
//   c2->SetFillColor(kWhite);
//   c2->SetBorderMode(0);
//   c2->SetFrameBorderMode(0);
//   h1->SetStats(kFALSE);
//   h1->SetXTitle("LVL-1 delay (BCLK)");
//   h1->SetTitle("Distribution of the means");
//   h1->Draw();

}

