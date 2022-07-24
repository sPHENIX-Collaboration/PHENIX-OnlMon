void convert_lut()
{
  // clean up vhdl code
  gSystem->Exec("cat ERT_E_LUT.vdl | grep \"','\" | sed s/\",\"/\" \"/g | sed s/\"'\"//g | sed s/\"(\"//g | sed s/\")\"//g > ERT_E.lut");
  ifstream fin("ERT_E.lut");
  ofstream fout0("lut_ert0.fpga.NEW");
  ofstream fout1("lut_ert1.fpga.NEW");
  short bit;
  for (int arm=0; arm<2; arm++)
    for (int emcal=0; emcal<128; emcal++)
      for (int rich=0; rich<128; rich++)
	{
	  fin >> bit;
	  if ((arm)==0)
	    fout0 << "ElectronLUT[" << arm << "][" << emcal << "]["
		 << rich << "] = " << bit << ";"<< endl;
	  if ((arm)==1)
	    fout1 << "ElectronLUT[" << arm << "][" << emcal << "]["
		 << rich << "] = " << bit << ";"<< endl;
	}
  fout0.close();
  fout1.close();
  fin.close();
}
