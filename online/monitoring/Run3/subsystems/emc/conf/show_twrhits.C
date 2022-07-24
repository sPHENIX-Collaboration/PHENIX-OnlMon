int iCHiSMT(int  iCH) 
{ 
  //  This is a very preliminary version which may well change at 
  //  a later time. It looks a bit complicated but it will be used
  //  only to establish look-up-tables for individual EmcFeeCrates
  int ASIC  = iCH/24;          //  2 tower wide columns
  int preamp = (iCH%24)/4;      //  2 tower wide rows counted 
                                    //  from above (0 at the top)
  int input  = iCH%4;
  //  convert input into x/y for cells within 'module' scope
  int xc, yc;
  yc = input/2;
  xc = 1-input%2;
  return 12*((5-preamp)*2+yc)+ASIC*2+xc;
}

int SMiSMTiST(int iSM ,int iSMTower, int emc)
{ 
  int x, bin, y, xx, yy;

  if(emc == 2){x=iSM%8; y=iSM/8; bin=96;}   //PbGl
  else 
  if(emc == 1){y=iSM/6; x=iSM-y*6; bin=72;} //PbSc
  else
    return -1;

  yy = iSMTower/12; xx = iSMTower - yy*12;


  return (y*12+yy)*bin+x*12+xx; 
}

int drawBoxGrid(TH2 *h)
{
   TAxis *x = ((TH1*) h)->GetXaxis();
   TAxis *y = ((TH1*) h)->GetYaxis();
   
   TLine l; 
   // short color= 21;
   // h->SetLabelColor(color, "y"); 

   if((h->GetNbinsX() % 12) || (h->GetNbinsY() % 12))
     return 0; 
   int xMin = (int) x->GetXmin(); int xMax = (int) x->GetXmax();
   int yMin = (int) y->GetXmin(); int yMax = (int) y->GetXmax();

   int ifemMax, ismMax;
   int smBeg, smEnd, smStepX,smStepY;  
   int module;
   int femBeg = yMin/12; int femEnd = yMax/12;
   const char * strEmcSector[] = {"WO","W1","W2","W3","E2","E3","E0","E1"};     
   int shiftSect;  

   if(xMax == 72)          // PbSc        
   {  
      ifemMax = xMax/12;     ismMax = 36;
      smBeg= femBeg*6; smEnd =femEnd*6; 
      smStepX = 2;     smStepY = 2; module = 36;
      shiftSect = 0;  
      // TText *PbScSectorText;
      
   }
   else if(xMax == 96) {   //PbGl         
      ifemMax = xMax/12;     ismMax  = 16;
      smBeg= femBeg*3; smEnd =femEnd*3; 
      smStepX = 6;     smStepY = 4; module = 48;
      shiftSect = 6;
   }
   else return 0;

   TText SectorText;
   SectorText.SetTextSize(0.04);
   SectorText.SetTextColor(46); 
   for(int i=yMin/module; i < yMax/module; i++)
   { 
      SectorText.DrawText(xMax+1, i*module+module/2-1,  strEmcSector[i+shiftSect]);
   }
   
   int ism;  
   l.SetLineWidth(1);
   for(ism= 0; ism < ismMax; ism++){
      int xsm= ism*smStepX;
      l.SetLineColor(1);
      l.DrawLine(xsm, yMin, xsm, yMax);
      for(int idsm =2; idsm < smStepX;)
	{
           l.SetLineColor(16);
           l.DrawLine(xsm+idsm, yMin, xsm+idsm, yMax);
	   idsm += 2;  
	}      
   }   
   for(ism= smBeg; ism < smEnd; ism++){     
       int ysm= ism*smStepY;
       l.SetLineColor(1);   
       l.DrawLine(xMin, ysm, xMax, ysm);
       for(int idsm =2; idsm < smStepY;)
         {
           l.SetLineColor(16);
	   l.DrawLine(xMin, ysm+idsm, xMax, ysm+idsm);
	   idsm += 2;
	 }        	
   } 
        
   int ifem;
   l.SetLineWidth(2);
   l.SetLineColor(13);
   for(ifem=0; ifem <= ifemMax; ifem++){	   
      int xfem= ifem*12;        
      l.DrawLine(xfem, yMin, xfem,  yMax);
   }   
   for(ifem=femBeg; ifem <= femEnd; ifem++){
      int yfem= ifem*12;   
      l.DrawLine(xMin, yfem, xMax, yfem);		
   } 

   l.SetLineColor(2);
   for(ism= yMin+module; ism < yMax; )
   {
      l.DrawLine(xMin, ism,  xMax, ism);
      ism += module; 
   } 
   
   l.SetLineWidth(1); 
   l.SetLineColor(1);     
   
  
   return 1;

}

void addWarnFEM(TH2* warn_emc, int iFEM=0)
{

  int nx =  warn_emc->GetNbinsX();
  int ny =  warn_emc->GetNbinsY();
  int ich_sec, isec;
  int ix, iy;
    
  if (nx == 72)
    { 
      if (iFEM <= 0 || iFEM >= 108)
	{
	  cout << "Wrong number of FEM [0, 107]: " << iFEM << endl; 
	}  
      for (int ich = 0;  ich < 144; ich++)
	{ 
          isec = iFEM/18;               
          ich_sec = SMiSMTiST(iFEM - isec*18, iCHiSMT(ich), 1);	   
          iy = ich_sec/nx + isec*36;
          ix = ich_sec%nx;
          warn_emc->Fill(ix, iy, 1); 
	}
    }
  else if (nx = 96)
    {
      if (iFEM <= 0 || iFEM >= 64)
	{
	  cout << "Wrong number of FEM [0, 63]: " << iFEM << endl; 
	}

      for (int ich = 0;  ich < 144; ich++)
	{ 
          isec = iFEM/32;
          ich_sec = SMiSMTiST(iFEM - isec*32, iCHiSMT(ich), 2);
          iy = ich_sec/nx + isec*48;
          ix = ich_sec%nx;
          warn_emc->Fill(ix, iy, 1);
	  // cout << "x=" << ix << " y=" << iy << endl;  
	} 
    }
          
} 


void saveWarnTower(char *filename="emc_warn.dat", TH2* warn_emc, int thresh)
{
  
  const int NCH = 24768; 
  int warn[NCH];

  for(int i =0; i < NCH; i++) warn[i] = 0;

  int nx =  warn_emc->GetNbinsX();
  int ny =  warn_emc->GetNbinsY(); 
       
  int id;  
  int index = 0;
  for( int iy=0; iy<ny; iy++ ) 
      for( int ix=0; ix<nx; ix++ ) 
      {        
        if(warn_emc->GetBinContent(ix+1, iy+1) > thresh)
	{
	    if(nx == 72)
	    { 
              id = iy*72 +ix; 
            }
	    else if(nx == 96){
              id = 15552 + iy*96 +ix; 
            }
            if(id < NCH)
	    {
              warn[id] += 1;
              index++;
            }
            else
              cout << "Wrong tower id: " << id << endl;
	}
      }
  cout << "X " << nx << " Y " << ny << endl;
  cout << "Number of hot towers " << index << endl;
  ofstream fout(filename, ios_base::out);
  if(!fout)
    { 
      cout << "Error create file: " << filename << endl;
      return;  
    }

  int beg =0, end = NCH;
  if(nx == 72)
    {   
      end = 15552;

    }
  else if(nx == 96)
    {
      beg = 15552;
    }  
  for(int i=beg; i < end; i++)
    fout << i << " " << warn[i] << endl;; 
  fout.close();
}
                                                       
void show_twrhits(const char *filename="emc_twrhits.dat", int chH = 0, int geom = 0)
{

  if(chH < 0 || chH > 5) 
  {
    cout << "Number of histogram [0,5] outrange: " << chH << endl;
    return;
  }

  const int NENERGY  =  3;
  const int NCHANNELS = 24768;

  //  int warn[NCHANNELS];

  //  gROOT->Reset();
  
  float fhits[NENERGY]; 
  int ich, iy, ix;
  int isec, ich_sec, ich, iy, ix;      
  //  int IP, Femlinear, SectorFEM, iST;
             
  // for( int i=0; i<NCHANNELS; i++ ) warn[i]=0;  

  char strbuf[80];
  FILE *pfile = fopen(filename, "r");  

  if(!pfile)
  {
    printf("Error opening file %s", filename);
    return; 
  }

  TH2* TwrHits[6];
    
  TH2* TwrHits[0] = (TH2*) gROOT->FindObject("PBSC_");
  TH2* TwrHits[1] = (TH2*) gROOT->FindObject("PBSC_1");
  TH2* TwrHits[2] = (TH2*) gROOT->FindObject("PBSC_3");

  TH2* TwrHits[3] = (TH2*) gROOT->FindObject("PBGL_");
  TH2* TwrHits[4] = (TH2*) gROOT->FindObject("PBGL_1");
  TH2* TwrHits[5] = (TH2*) gROOT->FindObject("PBGL_3");

  if(!TwrHits[0])
     TwrHits[0] = new TH2F("PBSC_", "PBSC_",  72, 0,72, 216, 0, 216);  
  else
     TwrHits[0]->Reset();
  if(!TwrHits[1])
     TwrHits[1] = new TH2F("PBSC_1", "PBSC_1",  72, 0,72, 216, 0, 216);
  else
     TwrHits[1]->Reset();
  if(!TwrHits[2])
     TwrHits[2] = new TH2F("PBSC_3", "PBSC_3",  72, 0,72, 216, 0, 216);
  else
     TwrHits[2]->Reset();

  if(!TwrHits[3])
     TwrHits[3] = new TH2F("PBGL_", "PBGL_",  96, 0, 96, 96, 0, 96);
  else
     TwrHits[3]->Reset();
  if(!TwrHits[4])
     TwrHits[4] = new TH2F("PBGL_1", "PBGL_1",  96, 0, 96, 96, 0, 96);
  else
     TwrHits[4]->Reset();
  if(!TwrHits[5])
     TwrHits[5] = new TH2F("PBGL_3", "PBGL_3",  96, 0, 96, 96, 0, 96);
  else
     TwrHits[5]->Reset();
 
 
  char strbFile[256];  
  if(!fgets(strbFile, 255 , pfile))
  return;
  cout << strbFile << endl;  
  char *token;
  char *search = " ";
  while (fgets(strbFile, 255, pfile) ) 
  { 
    // cout << strbFile << endl; 
    token = strtok(strbFile, search);
    // cout << token << endl;   
         
     if(token)
     {      
        ich = atoi(token);
	// cout << ich << endl;
        for(int i=0; i<NENERGY; i++) 
	{        
          token = strtok(0, search);  
          if(!token)
	  {
            fhits[i]=0;
	    break;
          }
          fhits[i] = atof(token);
	  // cout << fhits[i] << endl;  
          if(ich >=0 && ich < 15552)
	  {
	    if(geom)
	      {    
                ich = SMiSMTiST(ich/144, iCHiSMT(ich % 144), 1);
	      }
                      
	     ich_sec = ich%2592;         
	     isec = ich/2592;	   
	     iy = ich_sec/72 + isec*36;
	     ix = ich_sec%72;
             TwrHits[i]->Fill(ix,iy, fhits[i]);
	     // if(ix == 30 && iy== 200)
	     // cout << "ch=" << ich << " x" << ix <<" iy" << iy << endl;   
             
          }
          else if(ich >= 15552 && ich < NCHANNELS){
            
            int pbgl_ch = ich - 15552;   
            if(geom)
	      {       
                pbgl_ch = SMiSMTiST(pbgl_ch/144, iCHiSMT(pbgl_ch % 144), 2);             
	      }

            ich_sec = (pbgl_ch)%4608;  
            isec = (pbgl_ch)/4608;
	    iy = ich_sec/96+ isec*48;
	    ix = ich_sec%96;  
            TwrHits[i+3]->Fill(ix,iy, fhits[i]);
          }                        
         
        }// end for
        
     }

  }
  fclose(pfile);

  TFile *hfile = new TFile("htwr_hits.root", "RECREATE", "EMC towers hits");
  hfile->Write();
  hfile->Close();
   
  TCanvas* c1 = gROOT->FindObject("c1");
  if(!c1)
     c1 = new TCanvas();
  
  TwrHits[chH]->Draw();  
  drawBoxGrid(TwrHits[chH]);

  c1->Update();
  // PbScTwrHits0->Draw("same");
  //  gPad->AddExec("ex1",".x leftclick.C");

}
