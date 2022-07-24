const int nfems =   172;
const int ntowers = 144*nfems;




void calibDataHelper(int runnumber)
{
  gSystem->Load("libPgCalInstance.so");
  gSystem->Load("libemcOMpg.so");
 
  float ecal[ntowers];

  emcCalibrationDataHelper cdh(runnumber, false);

  //ofstream fout("caltest.dat", ios_base::out);
  for(int towerId = 0; towerId < ntowers; towerId++)
    {      
      // int iS, ist;
      // EmcIndexer::iPXiSiST(towerId, iS, ist);   
      // const emcCalibrationData* inical = cdh.getCalibrationData("IniCal",iS);  

    
      int absFEM, FEMchannel; 
      EmcIndexer::PXPXSM144CH(towerId, absFEM, FEMchannel);

      // const emcCalFEM *cfem = cdh.getCalibration(absFEM,"Gains");
      // double normt = cfem->getValue(FEMchannel, 0);
 
      const emcGainFEM* gains = dynamic_cast <const emcGainFEM*> cdh.getCalibration(absFEM,"Gains");
      double normt = gains->getValue(FEMchannel, 0);    
      // gains->Print(cout,1);
      ecal[towerId] = 0.0;
      if(normt > 0.0) 
	{
	  ecal[towerId] = cdh.getEnergyCalibration(towerId)/normt;  
	}
      // fout << normt << " " << norm1 << endl;

    }
  // fout.close();

  char filename[80];
  sprintf(filename, "cal%d.dat", runnumber);
  ofstream fout(filename, ios_base::out);
  
  for(int towerId = 0; towerId<ntowers; towerId++)
    {
      fout << towerId << " " << ecal[towerId] << endl; 

    }


  fout.close();
  
  
}

