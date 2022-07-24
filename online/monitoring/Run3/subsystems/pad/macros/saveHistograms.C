void saveHistograms(const char *padfilename="PadOnlMon.root"){
  OnlMonServer *se = OnlMonServer::instance();
  se->dumpHistos(padfilename);
}
