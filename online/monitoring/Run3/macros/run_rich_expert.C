#define RICH_NCHANNEL 5120;
#define RUN2_year 2001 ;
#define RUN3_year 2002 ;
TH1F *hadchit[RICH_NCHANNEL];
TH1F *hadcmean[RICH_NCHANNEL];
TH1F *hadcrms[RICH_NCHANNEL];
TH1F *htdchit[RICH_NCHANNEL];
TH1F *htdcmean[RICH_NCHANNEL];
TH1F *htdcrms[RICH_NCHANNEL];
char *OPTION;

void run_rich_expert(char *option="runno"){
  Int_t opflag=0;
  
  OPTION=option;

  if(option=="runno"){
    cout<<"plot generate as a function of "<<option<<endl;
    opflag=0;
  }else if(option=="date"){
    cout<<"plot generate as a function of "<<option<<endl;
    opflag=1;
  }else{
    cout<<"ilegal option"<<endl;
    opflag=-1;
  }

  TFile *fin;
  if((fin = new TFile("run_rich_expert.root","READ"))==NULL){
    cout<<"run_rich_expert.root doesn't exist"<<endl;
    exit(0);
  }

  Int_t runno, evtcnt;
  Int_t year, month, day, hour, minuit, second;
  Int_t pmtstat[RICH_NCHANNEL];
  Int_t   pmtadchit[RICH_NCHANNEL];
  Float_t adcmean[RICH_NCHANNEL];
  Float_t adcrms[RICH_NCHANNEL];
  Int_t   pmttdchit[RICH_NCHANNEL];
  Float_t tdcmean[RICH_NCHANNEL];
  Float_t tdcrms[RICH_NCHANNEL];

  TTree *rich_tree = (TTree*)fin->Get("stattree");
  rich_tree->SetBranchAddress("runno",&runno);
  rich_tree->SetBranchAddress("year",&year);
  rich_tree->SetBranchAddress("month",&month);
  rich_tree->SetBranchAddress("day",&day);
  rich_tree->SetBranchAddress("hour",&hour);
  rich_tree->SetBranchAddress("minuit",&minuit);
  rich_tree->SetBranchAddress("second",&second);
  rich_tree->SetBranchAddress("pmtstat",&pmtstat);
  rich_tree->SetBranchAddress("evtcnt",&evtcnt);
  rich_tree->SetBranchAddress("pmtadchit",&pmtadchit);
  rich_tree->SetBranchAddress("adcmean",&adcmean);
  rich_tree->SetBranchAddress("adcrms",&adcrms);
  rich_tree->SetBranchAddress("pmttdchit",&pmttdchit);
  rich_tree->SetBranchAddress("tdcmean",&tdcmean);
  rich_tree->SetBranchAddress("tdcrms",&tdcrms);
  
  Float_t date;
  Int_t maxdate=0;
  Int_t mindate=40000000;
  Int_t maxrunno=0;
  Int_t minrunno=40000;

  Int_t nall=rich_tree->GetEntries();
  if(nall==0){
    cout<<"file exists but entry is empty."<<endl;
  }

  for(Int_t i=0;i<nall;i++){
    rich_tree->GetEntry(i);
    date = 0.001*(((year-RUN2_year)*365*24*60*60)+(month-1)*30*24*60*60+(day)*24*60+minuit*60+second);
    if(date>maxdate){
      maxdate=date;
    }
    if(date<mindate){
      mindate=date;
    }
    if(runno>maxrunno){
      maxrunno=runno;
    }
    if(runno<minrunno){
      minrunno=runno;
    }
  }



  char name[100];

  for(Int_t i=0;i<RICH_NCHANNEL;i++){
    if(opflag==0){
      sprintf(name,"hadchit%d",i+1);
      hadchit[i] = new TH1F(name,name,maxrunno-minrunno+2,minrunno-1,maxrunno+1);
      sprintf(name,"hadcmean%d",i+1);
      hadcmean[i] = new TH1F(name,name,maxrunno-minrunno+2,minrunno-1,maxrunno+1);
      sprintf(name,"hadcrms%d",i+1);
      hadcrms[i] = new TH1F(name,name,maxrunno-minrunno+2,minrunno-1,maxrunno+1);
      sprintf(name,"htdchit%d",i+1);
      htdchit[i] = new TH1F(name,name,maxrunno-minrunno+2,minrunno-1,maxrunno+1);
      sprintf(name,"htdcmean%d",i+1);
      htdcmean[i] = new TH1F(name,name,maxrunno-minrunno+2,minrunno-1,maxrunno+1);
      sprintf(name,"htdcrms%d",i+1);
      htdcrms[i] = new TH1F(name,name,maxrunno-minrunno+2,minrunno-1,maxrunno+1);
    }else if(opflag==1){
      sprintf(name,"hadchit%d",i+1);
      hadchit[i] = new TH1F(name,name,maxdate-mindate+2,mindate-1,maxdate+1);
      sprintf(name,"hadcmean%d",i+1);
      hadcmean[i] = new TH1F(name,name,maxdate-mindate+2,mindate-1,maxdate+1);
      sprintf(name,"hadcrms%d",i+1);
      hadcrms[i] = new TH1F(name,name,maxdate-mindate+2,mindate-1,maxdate+1);

      sprintf(name,"htdchit%d",i+1);
      htdchit[i] = new TH1F(name,name,maxdate-mindate+2,mindate-1,maxdate+1);
      sprintf(name,"htdcmean%d",i+1);
      htdcmean[i] = new TH1F(name,name,maxdate-mindate+2,mindate-1,maxdate+1);
      sprintf(name,"htdcrms%d",i+1);
      htdcrms[i] = new TH1F(name,name,maxdate-mindate+2,mindate-1,maxdate+1);
    }
    hadchit[i]->SetMinimum(0); hadchit[i]->SetMaximum(30);
    hadcmean[i]->SetMinimum(0); hadcmean[i]->SetMaximum(20);
    hadcrms[i]->SetMinimum(0); hadcrms[i]->SetMaximum(20);

    htdchit[i]->SetMinimum(0); htdchit[i]->SetMaximum(30);
    htdcmean[i]->SetMinimum(0); htdcmean[i]->SetMaximum(1000);
    htdcrms[i]->SetMinimum(0); htdcrms[i]->SetMaximum(1000);


  }

  for(Int_t i=0;i<nall;i++){
    rich_tree->GetEntry(i);
    date = 0.001*(((year-RUN2_year)*365*24*60*60)+(month-1)*30*24*60*60+(day)*24*60+minuit*60+second);
    for(Int_t j=0;j<RICH_NCHANNEL;j++){
      if(opflag==0){
	hadchit[j]->Fill((Float_t)runno,(Float_t)pmtadchit[j]);
	hadcmean[j]->Fill((Float_t)runno,(Float_t)adcmean[j]);
	hadcrms[j]->Fill((Float_t)runno,(Float_t)adcrms[j]);
	htdchit[j]->Fill((Float_t)runno,(Float_t)pmttdchit[j]);
	htdcmean[j]->Fill((Float_t)runno,(Float_t)tdcmean[j]);
	htdcrms[j]->Fill((Float_t)runno,(Float_t)tdcrms[j]);
      }else if(opflag==1){
	hadchit[j]->Fill((Float_t)date,(Float_t)pmtadchit[j]);
	hadcmean[j]->Fill((Float_t)date,(Float_t)adcmean[j]);
	hadcrms[j]->Fill((Float_t)date,(Float_t)adcrms[j]);
	htdchit[j]->Fill((Float_t)date,(Float_t)pmttdchit[j]);
	htdcmean[j]->Fill((Float_t)date,(Float_t)tdcmean[j]);
	htdcrms[j]->Fill((Float_t)date,(Float_t)tdcrms[j]);
      }
    }

  }
}

void adcDraw(char *info="hit"){
  gStyle->SetTitleW(0.9);
  gStyle->SetTitleH(0.2);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.4);
  gStyle->SetStatFont(12);
  gStyle->SetStatColor(10);
  
  Int_t sec;
  Int_t i, j=0;
  TCanvas *rich_adchit_canvas1 = new TCanvas("Rich_adchit1","Rich_adchit1",1000,800); 
  TCanvas *rich_adchit_canvas2 = new TCanvas("Rich_adchit2","Rich_adchit2",1000,800); 
  rich_adchit_canvas1->Divide(5,8);
  rich_adchit_canvas1->Update();
  rich_adchit_canvas2->Divide(5,8);
  rich_adchit_canvas2->Update();

  for(Int_t i=0;i<RICH_NCHANNEL;i++){
    sec=j%80+1;
    if((sec-1)%16<8){
      rich_adchit_canvas1->cd(76-((sec-1)%16)*5+((sec-1)/16)-40);
      if(OPTION=="runno"){
	if(info=="hit"){
	  hadchit[i]->SetXTitle("runno");
	}else if(info=="mean"){
	  hadcmean[i]->SetXTitle("runno");
	}else if(info=="rms"){
	  hadcrms[i]->SetXTitle("runno");
	}
      }else if(OPTION=="date"){
	if(info=="hit"){
	  hadchit[i]->SetXTitle("date");
	}else if(info=="mean"){
	  hadcmean[i]->SetXTitle("date");
	}else if(info=="rms"){
	  hadcrms[i]->SetXTitle("date");
	}
      }
      hadchit[i]->Draw("BOXES");
    }else{
      rich_adchit_canvas2->cd(76-((sec-1)%16)*5+((sec-1)/16));
      if(OPTION=="runno"){
	if(info=="hit"){
	  hadchit[i]->SetXTitle("runno");
	  hadchit[i]->Draw("BOXES");
	}else if(info=="mean"){
	  hadcmean[i]->SetXTitle("runno");
	  hadcmean[i]->Draw("BOXES");
	}else if(info=="rms"){
	  hadcrms[i]->SetXTitle("runno");
	  hadcrms[i]->Draw("BOXES");
	}
      }else if(OPTION=="date"){
	if(info=="hit"){
	  hadchit[i]->SetXTitle("date");
	  hadchit[i]->Draw("BOXES");
	}else if(info=="mean"){
	  hadcmean[i]->SetXTitle("date");
	  hadcmean[i]->Draw("BOXES");
	}else if(info=="rms"){
	  hadcrms[i]->SetXTitle("date");
	  hadcrms[i]->Draw("BOXES");
	}
      }
    }
    if(sec==80){
      rich_adchit_canvas1->Update();
      rich_adchit_canvas2->Update();
    }
    j++;
  }
}

void tdcDraw(char *info="hit"){
  gStyle->SetTitleW(0.9);
  gStyle->SetTitleH(0.2);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.4);
  gStyle->SetStatFont(12);
  gStyle->SetStatColor(10);
  
  Int_t sec;
  Int_t i, j=0;
  TCanvas *rich_tdchit_canvas1 = new TCanvas("Rich_tdchit1","Rich_tdchit1",1000,800); 
  TCanvas *rich_tdchit_canvas2 = new TCanvas("Rich_tdchit2","Rich_tdchit2",1000,800); 
  rich_tdchit_canvas1->Divide(5,8);
  rich_tdchit_canvas1->Update();
  rich_tdchit_canvas2->Divide(5,8);
  rich_tdchit_canvas2->Update();

  for(Int_t i=0;i<RICH_NCHANNEL;i++){
    sec=j%80+1;
    if((sec-1)%16<8){
      rich_tdchit_canvas1->cd(76-((sec-1)%16)*5+((sec-1)/16)-40);
      if(OPTION=="runno"){
	if(info=="hit"){
	  htdchit[i]->SetXTitle("runno");
	  htdchit[i]->Draw("BOXES");
	}else if(info=="mean"){
	  htdcmean[i]->SetXTitle("runno");
	  htdcmean[i]->Draw("BOXES");
	}else if(info=="rms"){
	  htdcrms[i]->SetXTitle("runno");
	  htdcrms[i]->Draw("BOXES");
	}
      }else if(OPTION=="date"){
	if(info=="hit"){
	  htdchit[i]->SetXTitle("date");
	  htdchit[i]->Draw("BOXES");
	}else if(info=="mean"){
	  htdcmean[i]->SetXTitle("date");
	  htdcmean[i]->Draw("BOXES");
	}else if(info=="rms"){
	  htdcrms[i]->SetXTitle("date");
	  htdcrms[i]->Draw("BOXES");
	}
      }
    }else{
      rich_tdchit_canvas2->cd(76-((sec-1)%16)*5+((sec-1)/16));
      if(OPTION=="runno"){
	if(info=="hit"){
	  htdchit[i]->SetXTitle("runno");
	  htdchit[i]->Draw("BOXES");
	}else if(info=="mean"){
	  htdcmean[i]->SetXTitle("runno");
	  htdcmean[i]->Draw("BOXES");
	}else if(info=="rms"){
	  htdcrms[i]->SetXTitle("runno");
	  htdcrms[i]->Draw("BOXES");
	}
      }else if(OPTION=="date"){
	if(info=="hit"){
	  htdchit[i]->SetXTitle("date");
	  htdchit[i]->Draw("BOXES");
	}else if(info=="mean"){
	  htdcmean[i]->SetXTitle("date");
	  htdcmean[i]->Draw("BOXES");
	}else if(info=="rms"){
	  htdcrms[i]->SetXTitle("date");
	  htdcrms[i]->Draw("BOXES");
	}
      }
    }
    if(sec==80){
      rich_tdchit_canvas1->Update();
      rich_tdchit_canvas2->Update();
    }
    j++;
  }
}

