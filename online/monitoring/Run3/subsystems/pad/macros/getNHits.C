void getNHits(const Int_t iplane=-1){
  const short nwiresPerHVSector[12] = { 9, 20, 20, 9, 
                                       24, 34, 34, 24,
                                       29, 29, 29, 29 };

  char idstring[128];
  char localstring[128];  
  TH1* localPadStat;

  OnlMonServer *se = OnlMonServer::instance();

  localPadStat = se->getHisto("padStat");
  Int_t NEvent = localPadStat->GetBinContent(1);
  //  cout<<" NEvent = "<<NEvent<<endl;

  TH1 *localProfClustActivity[5]; // 1 HV sector per bin

  Int_t installedplanes=0;
  for(Int_t j = 0; j<6; j++){   
    if( j != 3 ){

      sprintf (idstring, "padProfClustActivity%d", j);
      sprintf (localstring, "localProfClustActivity%d", j);
            
      localProfClustActivity[installedplanes] = se->getHisto(idstring);

      Int_t NHits=0;
      for(Int_t k = 0; k<32; k++){
         // renormalize the entries with the number of wires per HV sector
         Int_t nwires = nwiresPerHVSector[(j/2)*4+k%4];
         if (j<2) nwires *= 2; // PC1 wires cover the whole acceptance
         Float_t bincontent = 
           localProfClustActivity[installedplanes]->GetBinContent(k);
	 //         cout<<" k: "<<k<<"  nwires: "<<nwires<<"  nhits: "<<bincontent<<endl;
         NHits = NHits + bincontent*nwires*NEvent;
      }
      installedplanes++;

      Float_t meanNHits = (Float_t) NHits/NEvent;
      Float_t errNHits = sqrt(NHits)/NEvent;
   
      if( j==0 && (iplane==-1 || iplane==0))
        printf(" Plane: 0  (PC 1 W)   <NHits>: %7.3f  +/- %7.3f \n",meanNHits,errNHits);
      if( j==1 && (iplane==-1 || iplane==1))
        printf(" Plane: 1  (PC 1 E)   <NHits>: %7.3f  +/- %7.3f \n",meanNHits,errNHits);
      if( j==2 && (iplane==-1 || iplane==2))
        printf(" Plane: 2  (PC 2 W)   <NHits>: %7.3f  +/- %7.3f \n",meanNHits,errNHits);
      if( j==3 )
        cout<<" ERROR. Plane 3 (PC 2 E) not installed"<<endl;
      if( j==4 && (iplane==-1 || iplane==4))
        printf(" Plane: 4  (PC 3 W)   <NHits>: %7.3f  +/- %7.3f \n",meanNHits,errNHits);
      if( j==5 && (iplane==-1 || iplane==5))
        printf(" Plane: 5  (PC 3 E)   <NHits>: %7.3f  +/- %7.3f \n",meanNHits,errNHits);

    }
  }
}
