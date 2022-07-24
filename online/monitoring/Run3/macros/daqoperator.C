void daqoperator()
{
gROOT->ProcessLine(".L run_daq.C");
daqDrawInit(1);
daqOper();
gROOT->ProcessLine(".q");
}
