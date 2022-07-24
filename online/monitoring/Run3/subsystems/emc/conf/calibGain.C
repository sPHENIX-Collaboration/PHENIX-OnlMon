
calibGain()
{
  gSystem->SetIncludePath("-I${HOME}/install-new/include -I${OFFLINE_MAIN}/include");
  gSystem->Load("libPgCalInstance.so");
  gSystem->Load("libemcOMpg.so");
  
  int const NSM   = 172;
  int const NSect = 8;             
  int const towers = 144*NSM;

  int towerId, iS, iST, iSM, iSMT, iFemCh;
  
  float encal, norm0, normt; 
  int ntrack = towers;
  float  calibcoeff;
  float fECalAtT0[towers];
   
  emcCalFEM *gains[NSM];


  PHTimeStamp now;
  //now.setToSystemTime(); // For Example:  now.set(2003,3,1,0,0,0);
  now.set(2005, 5, 12, 0, 8, 15);
  
 //   TFile * hbase = new TFile("gain.root","recreate");

 //   TTree * TreeEvent = new TTree("TreeEvent"," dbase info");
 //   TreeEvent->Branch("ntrack",&ntrack,"ntrack/I");
 //   TreeEvent->Branch("encal",encal,"encal[ntrack]/F");
 //   TreeEvent->Branch("norm0",norm0,"norm0[ntrack]/F");
 //   TreeEvent->Branch("normt",normt,"normt[ntrack]/F");
 //   TreeEvent->Branch("calibcoeff",calibcoeff ,"calibcoeff[ntrack]/i"); 
 
 
  emcDataManager * dm   = emcDataManager::GetInstance();

for(int itest=0; itest < 10; itest++)
 {
   cout << "Step " << itest << endl;

for(iSM=0; iSM<NSM; iSM++){
    // int code = emcCalFEM::FEMCode(iSM, 0, 0, 0);
    gains[iSM] = new emcGainFEM(iSM);
    gains[iSM]->SetSource(emcManageable::kDB_Pg);  
    bool ok   = dm->Read(*gains[iSM], now);

    // bool ok = dm->Read(*gains[iSM], now, code);
    if(ok){
      cout<<"<collectGains> SM "<<iSM<<" gains collected"<<endl;
    }
    else {
      cerr << "cannot get gains for SM: " << iSM
	       << endl;
    } 
  } 


  emcCalibrationData* sector[NSect];
  for (iS = 0; iS < NSect; ++iS) 
    {
      sector[iS] = new emcCalibrationData(emcCalibrationData::kIniCal,iS);

      sector[iS]->SetSource(emcManageable::kDB_Pg);
      bool ok = dm->Read(*(sector[iS]),now);
      if (!ok) 
	{
	  cerr << "<E> emcRawDataCalibratorV2::GetECalAtT0 : "
	       << "cannot get sector " << iS 
	       << " from source=" 
	       << emcManageable::GetStorageName(fSource)
	       << endl;
	}
    }  

 
  float norm[4608], oldnorm[4608];
  float normSec[NSect],  oldnormSec[NSect];         
  float rms, rmsold;
  
    

  ofstream fout("calibgain.dat", ios_base::out);
  
  for (towerId = 0; towerId<towers; towerId++) 
    {
      
      EmcIndexer::iPXiSiST(towerId, iS, iST) ;
      
      float one;
      
      // EmcIndexer::iPXiSiST(towerId, iS, iSM, iSMT);
      // int iFemCh = EmcIndexer::iSMTiCH(iSMT);
     

      if (iS < 6) 
	{
	  encal = sector[iS]->GetValue(iST,0);
	  norm0 = sector[iS]->GetValue(iST,1);
	  one   = sector[iS]->GetValue(iST,2);
	  assert(one==1.0);
	  fECalAtT0[towerId] = encal*norm0 ;   
	}
      else
	{
	  encal = sector[iS]->GetValue(iST,0) *
	  sector[iS]->GetValue(iST,1)*
	  sector[iS]->GetValue(iST,2); 
         
	  fECalAtT0[towerId] = encal;
	}

      EmcIndexer::PXPXSM144CH(towerId, iSM, iFemCh);
   
      normt = gains[iSM]->getValue(iFemCh,0);

      if(normt)
         calibcoeff = fECalAtT0[towerId] / normt ;
      else
	calibcoeff = 0;
 
    
      //  cout << "sec " << iS << "sm " << iSM << "tw " << iSMT << endl;
      fout << towerId << " " <<  calibcoeff << endl;
   
     // TreeEvent->Fill();       
    }
   // hbase->Write();
   //  hbase->Close();

    fout.close();

    for (iS = 0; iS < NSect; ++iS) 
    {
       delete sector[iS];
    }
    for (int i=0; i < NSM; i++)
    {
       delete gains[i];
    }
 
  }//end for test....       

}
