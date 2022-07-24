void displayInit(const int online = 1)
{
  loadAllSubsys();
  InitAllSubsys(online);
  return ;
}

void loadAllSubsys()
{
  gROOT->ProcessLine(".L run_acc.C");
  gROOT->ProcessLine(".L run_bbc.C");
  gROOT->ProcessLine(".L run_bbclvl1.C");
  gROOT->ProcessLine(".L run_daq.C");
  gROOT->ProcessLine(".L run_dch.C");
  gROOT->ProcessLine(".L run_emcal.C");
  gROOT->ProcessLine(".L run_ert.C");
  gROOT->ProcessLine(".L run_ertlvl1.C");
  //  gROOT->ProcessLine(".L run_fcal.C");
  gROOT->ProcessLine(".L run_gl1.C");
  //  gROOT->ProcessLine(".L run_localpol.C");
  //  gROOT->ProcessLine(".L run_lvl2.C");
  gROOT->ProcessLine(".L run_mcr.C");
  gROOT->ProcessLine(".L run_mpc.C");
  gROOT->ProcessLine(".L run_mui.C");
  gROOT->ProcessLine(".L run_muidlvl1.C");
  gROOT->ProcessLine(".L run_mutr.C");
  //  gROOT->ProcessLine(".L run_mutrg.C");
//   gROOT->ProcessLine(".L run_mutriglvl1.C");
  gROOT->ProcessLine(".L run_pad.C");
  //   gROOT->ProcessLine(".L run_pbglu.C");
  gROOT->ProcessLine(".L run_pktsize.C");
  gROOT->ProcessLine(".L run_rich.C");
  //  gROOT->ProcessLine(".L run_rpc.C");
  //  gROOT->ProcessLine(".L run_spin.C");
  //  gROOT->ProcessLine(".L run_svxstrip.C");
  //  gROOT->ProcessLine(".L run_svxpixel.C");
  gROOT->ProcessLine(".L run_tof.C");
  gROOT->ProcessLine(".L run_tofw.C");
  gROOT->ProcessLine(".L run_zdc.C");
  gROOT->ProcessLine(".L run_zdclvl1.C");
}

void InitAllSubsys(const int online = 0)
{
  accDrawInit(online);
  bbcDrawInit(online);
  bbclvl1DrawInit(online);
  daqDrawInit(online);
  dchDrawInit(online);
  emcalDrawInit(online);
  ertDrawInit(online);
  ertlvl1DrawInit(online);
  //  fcalDrawInit(online);
  gl1DrawInit(online);
  //  localpolDrawInit(online);
  //  lvl2DrawInit(online);
  mcrDrawInit(online);
  mpcDrawInit(online);
  muiDrawInit(online);
  muidlvl1DrawInit(online);
  mutrDrawInit(online);
//   mutrgDrawInit(online);
//   mutriglvl1DrawInit(online);
  padDrawInit(online);
  //   pbgluDrawInit(online);
  pktsizeDrawInit(online);
  richDrawInit(online);
  //  rpcDrawInit(online);
  //  spinDrawInit(online);
  //  svxpixelDrawInit(online);
  //  svxstripDrawInit(online);
  tofDrawInit(online);
  tofwDrawInit(online);
  zdcDrawInit(online);
  zdclvl1DrawInit(online);
}

void displayLoop(const int n = -1)
{
#define NDRAW 27
  const char *drawfunc[NDRAW] =
    {
      "accDraw(\"ALLADCTDC\");",
      "accDraw(\"TDCpedestal\");",
      "bbcDraw();",
      "bbcDraw(\"BbcMonitor\");",
      "bbclvl1Draw();",
      "daqDraw();",
      "daqDraw(\"GLOBAL\");",
      "daqDraw(\"LOWRUN\");",
      "dchDraw();",
      "emcalDraw();",
      "ertDraw();",
      "ertlvl1Draw(\"EAST\");",
      "ertlvl1Draw(\"WEST\");",
      "gl1Draw();",
      "mcrDraw();",
      "mpcDraw();",
      "muiDraw();",
      "muidlvl1Draw(\"NORTH\");",
      "muidlvl1Draw(\"SOUTH\");",
      "mutrDraw();",
      "padDraw();",
      "pktsizeDraw();",
      "richDraw();",
      // "svxpixelDraw(\"SvxPixelLadderWest\");",
      // "svxpixelDraw(\"SvxPixelLadderEast\");",
      "tofDraw();",
      "tofwDraw();",
      "zdcDraw();",
      "zdclvl1Draw();"
    };
  //  int nloop = 0;
  // now we always have in allCanvases a list of all currently
  // existing canvases
  TSeqCollection* allCanvases = gROOT->GetListOfCanvases();
  // this TList will hold the old canvases we want to delete
  TList *canvaslist = new TList();
  //  int nloop = 0;
  int breakloop = 0;
  while (1)
    {
      for (int j = 0; j < NDRAW; j++)
        {
          int lastindex = allCanvases->LastIndex();
          // save the old canvases so they can be deleted after
          // the new one is drawn
          for (int i = 0; i <= lastindex; i++)
            {
              canvaslist->AddLast(allCanvases->At(i));
            }
          cout << "executing " << drawfunc[j] << endl;
          gROOT->ProcessLine(drawfunc[j]);

          for (int i = 0; i <= lastindex; i++)
            {
              TCanvas *canvas = (TCanvas *) allCanvases->First();
              //              cout << "Deleting Canvas " << canvas->GetName() << endl;
              delete canvas;
            }
	  //  	  while (canvaslist->LastIndex() >= 0)
	  //  	    {
	  //	      delete canvaslist->First();
	  // 	      TCanvas *canvas = (TCanvas *) canvaslist->First();
	  //       // this deletes all canvases in the list at once
	  // 	      //	      delete canvas;
	  // 	      //	      canvaslist->Delete();
	  // 	    }
          gSystem->Exec("xsetroot -solid black");
          FILE *ftest = fopen("nosleep", "r");
          if (!ftest)
            {
              gSystem->Sleep(10000);
            }
          else
            {
              cout << "No sleeping before redrawing" << endl;
              fclose(ftest);
            }
          ftest = fopen("stopdisplayloop", "r");
          if (ftest)
            {
              fclose(ftest);
              breakloop = 1;
              break;
            }
        }
      if (breakloop)
        {
          break;
        }
      //      nloop++;
    }
  // delete last existing canvases
  // this allCanvases->Delete() caused segfaults sometimes
  // allCanvases->Delete();
  // so put all canvases into a TList and delte them then
  while ((canvas = (TCanvas *) allCanvases->First()))
    {
      //       cout << "Deleting Canvas " << canvas->GetName() << endl;
      delete canvas;
    }
  OnlMonClient *cl = OnlMonClient::instance();
  delete cl;
  gSystem->Exit(0);
}
