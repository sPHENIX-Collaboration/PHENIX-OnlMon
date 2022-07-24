
Reference(Char_t *old_ref,  Char_t *new_ref, Int_t emc)
{
  // emc 0 - PBGL, 1 -PBSC

  
   if(emc > 1 || emc < 0)
     {
       cout << "Wrong emc parameter:" << emc <<"PBGL=0 PBSC=1" << endl;
      return;
     }
   const Char_t * str_ref = "emc_ref1.root"; 
   
   if(emc == 0)
   {
     TFile *pbsc = new TFile(old_ref);
     TFile *pbgl = new TFile(new_ref);
  
   }
   else {
     TFile *pbsc = new TFile(new_ref);
     TFile *pbgl = new TFile(old_ref);
   }
   TFile *ref =  new TFile(str_ref, "RECREATE");   
      
   TH1F *EMCal_Info =      (TH1F*) pbsc->Get("EMCal_Info");
   TH1D *FrameWorkVars =   (TH1D*) pbsc->Get("FrameWorkVars");     

   TH1F *PBGL_T_ref =      (TH1F*) pbgl->Get("PBGL_T_ref");
   TH1F *PBGL_T_sm =       (TH1F*) pbgl->Get("PBGL_T_sm"); 
   TH1F *PBGL_ref =        (TH1F*) pbgl->Get("PBGL_ref");
   TH1F *PBGL_sm =         (TH1F*) pbgl->Get("PBGL_sm");
   TH1F *ReferPin_PBGL =   (TH1F*) pbgl->Get("ReferPin_PBGL");
   TH1F *ReferPin_T_PBGL = (TH1F*) pbgl->Get("ReferPin_T_PBGL");
   TH1F *Refer_PBGL =      (TH1F*) pbgl->Get("Refer_PBGL");
   TH1F *Refer_T_PBGL =    (TH1F*) pbgl->Get("Refer_T_PBGL"); 
   TH1F *avPBGL_REF =      (TH1F*) pbgl->Get("avPBGL_REF");
   TH1F *avPBGL_SM =       (TH1F*) pbgl->Get("avPBGL_SM");
   TH1F *avPBGL_T_REF =    (TH1F*) pbgl->Get("avPBGL_T_REF");
   TH1F *avPBGL_T_SM =     (TH1F*) pbgl->Get("avPBGL_T_SM");
   TH1F *dispPBGL_SM =     (TH1F*) pbgl->Get("dispPBGL_SM");
   TH1F *dispPBGL_T_SM =   (TH1F*) pbgl->Get("dispPBGL_T_SM");
   
   TH1F *PBSC_TP =         (TH1F*) pbsc->Get("PBSC_TP");
   TH1F *PBSC_T_TP =       (TH1F*) pbsc->Get("PBSC_T_TP");
   TH1F *PBSC_T_ref =      (TH1F*) pbsc->Get("PBSC_T_ref");
   TH1F *PBSC_T_sm =       (TH1F*) pbsc->Get("PBSC_T_sm");
   TH1F *PBSC_ref =        (TH1F*) pbsc->Get("PBSC_ref");
   TH1F *PBSC_sm =         (TH1F*) pbsc->Get("PBSC_sm");
   TH1F *ReferPin_PBSC =   (TH1F*) pbsc->Get("ReferPin_PBSC");
   TH1F *ReferPin_T_PBSC = (TH1F*) pbsc->Get("ReferPin_T_PBSC");
   TH1F *Refer_PBSC =      (TH1F*) pbsc->Get("Refer_PBSC");
   TH1F *Refer_TP_PBSC =   (TH1F*) pbsc->Get("Refer_TP_PBSC");
   TH1F *Refer_T_PBSC =    (TH1F*) pbsc->Get("Refer_T_PBSC");
   TH1F *Refer_T_TP_PBSC = (TH1F*) pbsc->Get("Refer_T_TP_PBSC");
   TH1F *avPBSC_REF =      (TH1F*) pbsc->Get("avPBSC_REF");
   TH1F *avPBSC_SM =       (TH1F*) pbsc->Get("avPBSC_SM");
   TH1F *avPBSC_T_REF =    (TH1F*) pbsc->Get("avPBSC_T_REF");
   TH1F *avPBSC_T_SM =     (TH1F*) pbsc->Get("avPBSC_T_SM");
   TH1F *dispPBSC_SM =     (TH1F*) pbsc->Get("dispPBSC_SM");
   TH1F *dispPBSC_T_SM =   (TH1F*) pbsc->Get("dispPBSC_T_SM");

   ref->Add(EMCal_Info);
   ref->Add(FrameWorkVars);

   ref->Add(PBGL_T_ref);
   ref->Add(PBGL_T_sm);
   ref->Add(PBGL_ref);
   ref->Add(PBGL_sm);
   ref->Add(PBSC_TP);
   ref->Add(PBSC_T_TP);
   ref->Add(PBSC_T_ref);
   ref->Add(PBSC_T_sm);
   ref->Add(PBSC_ref);
   ref->Add(PBSC_sm);
   ref->Add(ReferPin_PBGL);
   ref->Add(ReferPin_PBSC);
   ref->Add(ReferPin_T_PBGL);
   ref->Add(ReferPin_T_PBSC);
   ref->Add(Refer_PBGL);
   ref->Add(Refer_PBSC);
   ref->Add(Refer_TP_PBSC);
   ref->Add(Refer_T_PBGL);
   ref->Add(Refer_T_PBSC);
   ref->Add(Refer_T_TP_PBSC);
   ref->Add(avPBGL_REF);
   ref->Add(avPBGL_SM);
   ref->Add(avPBGL_T_REF);
   ref->Add(avPBGL_T_SM);
   ref->Add(avPBSC_REF);
   ref->Add(avPBSC_SM);
   ref->Add(avPBSC_T_REF);
   ref->Add(avPBSC_T_SM);
   ref->Add(dispPBGL_SM);
   ref->Add(dispPBGL_T_SM);
   ref->Add(dispPBSC_SM);
   ref->Add(dispPBSC_T_SM);
 
   ref->Write();
   ref->Close();
   pbsc->Close();
   pbgl->Close(); 
}
