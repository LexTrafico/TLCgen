/* ======================================================================================================== */
/* appl_pro.c                                                                                               */
/* Versie:                               1.0                                                                */
/* Door:                                 -                                                                  */
/* Datum generatie:                      07-09-2020                                                         */
/*  Tijd generatie:                      08:32:25 (+02:00)                                                  */
/* ======================================================================================================== */
/* Deze include file bevat de functies voor de voertuigafhankelijke regelstructuur SGD+                     */
/* van de freeware CCOL generator Traffick.                                                                 */
/* Deze functies zijn ontwikkeld en geschreven door Marcel Fick.                                            */
/* Versie: 1.0                                                                                              */
/* Datum:  1 oktober 2019                                                                                   */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Include files                                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Header file                                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  #include "pro_fik.h"                  /* procedure header file                                            */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Functie bepaal TI_pointer[][]                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  count TI_pointer(                     /* Fik191001                                                        */
    count fc1,
    count fc2)
  {
#ifdef INTERGROEN
    return KF_pointer[fc1][fc2];
#else
    return TO_pointer[fc1][fc2];
#endif
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Functie bepaal TI_max[][]                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  mulv TI_max(                          /* Fik191001                                                        */
    count fc1,
    count fc2)
  {
#ifndef NO_TIGMAX                       /* intergroentijden                                                 */
    if ((fc1 < FCMAX) && (fc2 < FCMAX))
    {
      return TIG_max[fc1][fc2];
    }
    return NG;
    
#else                                   /* ontruimingstijden                                                */
    if ((fc1 < FCMAX) && (fc2 < FCMAX))
    {
      if (TO_max[fc1][fc2] >= 0) return (TGL_max[fc1] + TO_max[fc1][fc2]);
      if (TO_max[fc1][fc2] <  0) return  TO_max[fc1][fc2];
    }
    return NG;
    
#endif
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Functie bepaal TI[][]                                                                                    */
/* -------------------------------------------------------------------------------------------------------- */
  bool TI(                              /* Fik191001                                                        */
    count fc1,
    count fc2)
  {
#ifndef NO_TIGMAX                       /* intergroentijden                                                 */
    if ((fc1 < FCMAX) && (fc2 < FCMAX))
    {
      if (TIG_max[fc1][fc2] >= 0) return TIG[fc1][fc2];
    }
    return FALSE;
    
#else                                   /* ontruimingstijden                                                */
    if ((fc1 < FCMAX) && (fc2 < FCMAX))
    {
      if (TO_max[fc1][fc2] >= 0) return (GL[fc1] || TO[fc1][fc2]);
    }
    return FALSE;
    
#endif
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Functie bepaal TI_timer[]                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  mulv TI_timer(                        /* Fik191001                                                        */
    count fc)
  {
#ifndef NO_TIGMAX                       /* intergroentijden                                                 */
    if (fc < FCMAX)
    {
      return TIG_timer[fc];
    }
    return 0;
    
#else                                   /* ontruimingstijden                                                */
    if (fc < FCMAX)
    {
      if (G[fc]) return TO_timer[fc];
      if (GL[fc])
      {
        if (TGL[fc])
        {
          return TGL_timer[fc];
        }
        else
        {
          return (TGL_max[fc] + TO_timer[fc]);
        }
      }
      if (R[fc]) return (TGL_max[fc] + TO_timer[fc]);
    }
    return 0;
    
#endif
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie conflicterend gewenst groen                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  bool somfbk(                          /* Fik140101                                                        */
    count fc)
  {
    count j,k;
    for (j=0; j<GKFC_MAX[fc]; j++) {    /* primaire conflicten                                              */
      k = TI_pointer(fc,j);
      if (B[k] || RA[k] || G[k] && (!MG[k] || RW[k])) return TRUE;
    }
    for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) { /* fictieve conflicten                                      */
      k = TI_pointer(fc,j);
      if (AFC[fc] && AFC[k]) {
        if (B[k] || RA[k]) return TRUE;
      }
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPc[fc][k]) {
        if (B[k] || RA[k] || G[k] && (!MG[k] || RW[k])) return TRUE;
      }
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPf[fc][k]) {
        if (AFC[fc] && AFC[k]) {
          if (B[k] || RA[k]) return TRUE;
        }
      }
    }
    
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie conflicterend gewenst groen met wt voorspellers                                              */
/* -------------------------------------------------------------------------------------------------------- */
  bool _somfbk(                         /* Fik140101                                                        */
    count fc)
  {
    count j,k;
    for (j=0; j<GKFC_MAX[fc]; j++) {    /* Primaire conflicten                                              */
      k = TI_pointer(fc,j);
      if (B[k] || RA[k] && !_GRN[k] || G[k] && (!MG[k] || RW[k])) return TRUE;
    }
    for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) { /* Fictieve conflicten                                      */
      k = TI_pointer(fc,j);
      if (AFC[fc] && AFC[k]) {
        if (B[k] || RA[k] && !_GRN[k]) return TRUE;
      }
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPc[fc][k]) {
        if (B[k] || RA[k] && !_GRN[k] || G[k] && (!MG[k] || RW[k])) return TRUE;
      }
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPf[fc][k]) {
        if (AFC[fc] && AFC[k]) {
          if (B[k] || RA[k] && !_GRN[k]) return TRUE;
        }
      }
    }
    
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie conflicterend groen                                                                          */
/* -------------------------------------------------------------------------------------------------------- */
  bool somgk(                           /* Fik110724                                                        */
    count fc)
  {
    count j,k;
    
    for (j=0; j<GKFC_MAX[fc]; j++) {    /* primaire conflicten                                              */
      k = TI_pointer(fc,j);
      if (G[k] || _GRN[k]) return TRUE;
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPc[fc][k]) {
        if (G[k] || _GRN[k]) return TRUE;
      }
    }
    
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie conflicterende aanvraag                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  bool somak(                           /* Fik140101                                                        */
    count fc)
  {
    count j,k;
    
    for (j=0; j<GKFC_MAX[fc]; j++) {
      k = TI_pointer(fc,j);
      if (R[k] && A[k]) return TRUE;
    }
    
    for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) {
      k = TI_pointer(fc,j);
      if (AFC[fc] && AFC[k]) {
        if (R[k] && A[k]) return TRUE;
      }
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPc[fc][k] || TMPf[fc][k] && AFC[fc] && AFC[k]) {
        if (R[k] && A[k]) return TRUE;
      }
    }
    
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie conflicterende aanvraag of garantie roodtijd aktief                                          */
/* -------------------------------------------------------------------------------------------------------- */
  bool somakgr(                         /* Fik180131                                                        */
    count fc)
  {
    count j,k;
    
    for (j=0; j<GKFC_MAX[fc]; j++) {
      k = TI_pointer(fc,j);
      if (!R[k] || TRG[k] || A[k]) return TRUE;
    }
    
    for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) {
      k = TI_pointer(fc,j);
      if (AFC[fc] && AFC[k]) {
        if (!R[k] || TRG[k] || A[k]) return TRUE;
      }
    }
    
    for (k=0; k<FC_MAX; k++) {
      if (TMPc[fc][k] || TMPf[fc][k] && AFC[fc] && AFC[k]) {
        if (!R[k] || TRG[k] || A[k]) return TRUE;
      }
    }
    
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Functie bepaal toestemming twee blokken vooruit realiseren                                               */
/* -------------------------------------------------------------------------------------------------------- */
  bool PPV2_BLOK(                       /* Fik180101                                                        */
    count fc)                           /* fasecyclus                                                       */
  {
    bool PPV = TRUE;
    mulv vorig;
    count i, j, k;
    count fc1, fc2, fc3;
    
    vorig = PBLK[fc] - 1;
    if (vorig == 0) vorig = MAX_BLK;
    
    if (MAX_BLK < 3) PPV = FALSE;
    if (PPV)
    {
      for (j = 0; j < FKFC_MAX[fc]; j++)
      {
        k = TI_pointer(fc,j);
        if (PBLK[k] == vorig) PPV = FALSE;
      }
    }
    
    if (PPV)
    {
      for (i = 0; i < aant_hki; i++)
      {
        fc1 = hki[aant_hki][0];
        fc2 = hki[aant_hki][1];
        fc3 = hki[aant_hki][10];
        if ((fc == fc1) || (fc == fc2) || (fc == fc3))
        {
          if ((fc1 != fc) && (fc1 != NG))
          {
            for (j = 0; j < FKFC_MAX[fc1]; j++)
            {
              k = TI_pointer(fc1,j);
              if (PBLK[k] == vorig) PPV = FALSE;
            }
          }
          if ((fc2 != fc) && (fc2 != NG) && PPV)
          {
            for (j = 0; j < FKFC_MAX[fc2]; j++)
            {
              k = TI_pointer(fc2,j);
              if (PBLK[k] == vorig) PPV = FALSE;
            }
          }
          if ((fc3 != fc) && (fc3 != NG) && PPV)
          {
            for (j = 0; j < FKFC_MAX[fc3]; j++)
            {
              k = TI_pointer(fc3,j);
              if (PBLK[k] == vorig) PPV = FALSE;
            }
          }
        }
      }
    }
    return PPV;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure initialisatie regeling VAS en SGD                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_init_reg(void)              /* Fik190101                                                        */
  {
    count i,j;
    
#ifdef C_ITS_FUNCTIONALITEIT
    count k;
#endif                                  /* *** ifdef -> C_ITS_FUNCTIONALITEIT                               */
    
    SPR  = TRUE;
    SBL  = FALSE;
    WACHT= FALSE;
    BLOK = MAX_BLK = WBLK    = BBLOK  = _dvm   = 0;
    KNIP = BEDRYF  = FIXATIE = SOMEVG = Hreset = FALSE;
    BRUG = SPOOR   = FALSE;
    
#ifdef INGREEP_VIOSS
    BRUG_ONT  = NABRUG  = SBRUG  = EBRUG  = FALSE;
    SPOOR_ONT = NASPOOR = SSPOOR = ESPOOR = FALSE;
#endif
    
    for (i=0; i<FC_MAX; i++) {
      KG[i]  = HG[i]  = KR[i]  = KGCA[i]= KGOM[i]= MAXG[i]= KRVG[i]= TKMX[i]  = PRGR[i]= PRTEG[i]  = 0;
      BAR[i] = SGD[i] = HKI[i] = HKII[i]= BUS1[i]= DZ[i]  = RAT[i] = RATGEL[i]= AIGL[i]= VG1[i]    = FALSE;
      PBLK[i]= TTK[i] = TTR[i] = TTC[i] = TTP[i] = GTTP[i]= TEG[i] = BPI[i]   = TTPX[i]= TEGX[i]   = 0;
      POV[i] = PEL[i] = PEL1[i]= PEL2[i]= FTS[i] = _HOT[i]= STAR[i]= HPRI[i]  = _DCF[i]= VLOG_OV[i]= 0;
      RISPEL[i] = RISPEL1[i] = TOV[i] = VSM[i] = VSM_ref[i] = WSP[i] = WSP_ref[i] = RISPEL2[i] = RISFTS[i] = 0;
      vracht1[i]= vracht2[i] = VFA[i] = RATGN[i] = 0;
      
#ifdef INGREEP_VIOSS
      BRG_PRI[i]= BRG_BLK[i] = BRG_KAP[i] = FALSE;
      BRG_ONT[i]= TRUE;
      
      SPW_PRI[i]= SPW_BLK[i] = SPW_KAP[i] = FALSE;
#endif
      
      LEDST[i]  = FALSE;
      _pri_akt[i]=FALSE;
      AFC[i] = TRUE;
      DZ_tijd[i] = NG;
      
      for (j=0; j<MAX_VTG_VSM; j++) {
        momenten_vsm[i][j][0] = 0;
        momenten_vsm[i][j][1] = 0;
      }
      
      for (j=0; j<MAX_VTG_WSP; j++) {
        momenten_wsp[i][j][0] = 0;
        momenten_wsp[i][j][1] = 0;
        momenten_wsp[i][j][2] = 0;
        momenten_wsp[i][j][3] = 0;
      }
      
#ifdef KAR_AANWEZIG
      
      KARovi[i]= KARovu[i]= FALSE;
      KARhdi[i]= KARhdu[i]= FALSE;
      KARfc[i] = OVUIT[i] = 0;
      
#endif                                  /* *** ifdef -> KAR_AANWEZIG                                        */
      
#ifdef DVM_AANWEZIG
      
      MAX_DVM_OV[i] = 3;
      MAX_DVM_PK[i] = 3;
      
#endif                                  /* *** ifdef -> DVM_AANWEZIG                                        */
      
      ARM[i] = VARM[i]= HVIN[i]= 0;
      for (j=0; j<=5; j++) {
        HLPD[i][j] = 0;
      }
      
      FCA[i] = HVAS[i]= 0;
      VAS1[i]= VAS2[i]= VAS3[i]= VAS4[i]= VAS5[i]= VASR[i]= 0;
      KAPC[i]= 0;
      KVOV[i]= KVPK[i]= KVFC[i]= KVRPK[i]= 0;
      FCDVM[i]=FALSE;
      
      if (TFG_max[i]>TGG_max[i]) ALTM[i]= KAPM[i]= PRIM[i]= _DOS[i] = TFG_max[i]/10;
      else                       ALTM[i]= KAPM[i]= PRIM[i]= _DOS[i] = TGG_max[i]/10;
      
      _FBL[i]= _FIL[i]= 0;
      
      TWV[i] = EGW[i] = TTW[i] = Aled[i] = Adwt[i] = Tdwt[i] = halt_tim[i] = WTVFC[i] = 0;
      BLWT[i]=_GRN[i] = halteer_wtv[i] = FALSE;
      AVR_GEBIED[i]   = 0;
      
#ifdef C_ITS_FUNCTIONALITEIT
      
      FKG[i]         = 0;
      _AREA[i]       = FALSE;
      _GRNNEW[i]     = 0;
      _GRNOLD[i]     = 0;
      _GRN_meting[i] = FALSE;
      VG1_GEBIED[i] = VG2_GEBIED[i] = VG3_GEBIED[i] = VG4_GEBIED[i] = 0;
      VG3_GEBIED_OPEN[i] = FALSE;
      
      tov_eerste[i] = pov_eerste[i] = FALSE;
      for (j = 0; j<RIS_MEET_MAX; j++) pel_meet_voertuig[i][j][0] = '\0';
      for (k = 0; k < RIS_PEL_AANTAL_MAX; k++) {
        for (j = 0; j < RIS_PELBUFFER_MAX; j++) {
          RIS_peloton_ID[i][k][j][0] = '\0';
        }
      }
      for (j = 0; j<RIS_TOVER_MAX; j++) {
        TOVER[i][j].voertuig_id[0] = '\0';
        TOVER[i][j].duur_aanwezig  = 0;
      }
      
      OV_ris[i]  = 0;
      OV_aanw[i] = 0;
      OV_stipt[i]= 0;
      for (j=0; j<RIS_OV_MAX; j++) {
        RIS_OV[i][j].voertuig_id[0] = '\0';
        RIS_OV[i][j].stiptheid_ov   = 0;
        RIS_OV[i][j].duur_aanwezig  = 0;
        RIS_OV[i][j].prio_status    = 0;
      }
      
      HD_ris[i] = 0;
      HD_aanw[i]= 0;
      for (j=0; j<RIS_HLPD_MAX; j++) {
        HULP_DIENST[i][j].voertuig_id[0] = '\0';
        HULP_DIENST[i][j].duur_aanwezig  = 0;
        HULP_DIENST[i][j].prio_status    = 0;
      }
      
#endif                                  /* *** ifdef -> C_ITS_FUNCTIONALITEIT                               */
      
#ifdef TESTOMGEVING
      
      _ha[i] = FALSE;
      
#endif                                  /* *** ifdef -> TESTOMGEVING                                        */
      
      for (j=0; j<FC_MAX; j++) {
        TMPc[i][j] = FALSE;
        TMPf[i][j] = FALSE;
        TMPi[i][j] = 0;
        MV_matrix[i][j] = FALSE;
      }
    }
    
    for (j=0; j<DP_MAX; j++) {
      DF[j]= FALSE;
      _VG4[j]= FALSE;
      _VT4[j]= _VT4_old[j]= 255;
      _TDH[j]= 0;
    }
    
#ifdef KAR_AANWEZIG
    
    for (i=0; i<10; i++)
    {
      kbufjr[i]=0; kbufmd[i]=0; kbufdg[i]=0;
      kbufur[i]=0; kbufmi[i]=0; kbufse[i]=0;
      kbuf02[i]=0; kbuf03[i]=0; kbuf06[i]=0; kbuf07[i]=0;
      kbuf08[i]=0; kbuf11[i]=0; kbuf13[i]=0; kbuf14[i]=0;
      kbuf15[i]=0; kbuf19[i]=0;
    }
    KAR_sim = FALSE;
    
#endif                                  /* *** ifdef -> KAR_AANWEZIG                                        */
    
    SVAS  =SSGD  =OMSCH= N_ts = FALSE;  /* variabelen tbv HALFSTAR                                          */
    GEWPRG=AKTPRG=STAP = 0;
    MKODE =CKODE =HKODE =LKODE= MKODE_oud= 0;
    CTYD  =IPNT  =OPNT = CPNT = 0;
    
    if (aantal_VAS == 0) SSGD = TRUE;
    Straag=Svsnel=Sstart=Sblok=Sdubb=FALSE;
    Sverschil=Sversnel=0;
    
    aant_hki = 0;
    aantal_vtg_fts = 0;
    aantal_dcf_vs  = 0;
    aantal_dcf_gs  = 0;
    
    for (i=0; i<FC_MAX; i++) {
      for (j=0; j<=20; j++) {
        hki[i][j] = 0;
      }
      for (j=0; j<=16; j++) {
        kp_vtg_fts[i][j] = 0;
      }
      for (j=0; j<=5; j++) {
        dcf_vs[i][j] = 0;
        dcf_gs[i][j] = 0;
      }
    }
    
#ifdef _VAS_OV
    
    ELOK = FALSE;
    for (i=0; i<FC_MAX; i++) {
      VAS2a[i] = VAS2b[i] = VAS6[i] = VAS7[i] = 0;
      OVOKVAS[i] = OVAKVAS[i] = 0;
      OVTRVAS[i] = OVAWTVAS[i] = OVINMVVAS[i] = OVAFRTVAS[i] = 0;
      ALTOV[i] = ALTO[i] = ALTE[i] = VAWT[i] = TTO[i] = CRTR[i] = _VTG[i] = 0;
      ARRC[i]  = HKAP[i] = FALSE;
      for (j=0; j<FC_MAX; j++) {
        OVTMPVAS[i][j] = FALSE;
      }
    }
    
#endif                                  /* *** ifdef -> _VAS_OV                                             */
    
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure initialiseer SGD regeling                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void init_sgd(void)                   /* Fik180101                                                        */
  {
    count i,j;
    mulv  volgend,volgend_plus;
    
    BLOK   = 1;
    SOMEVG = SBL = TRUE;
    WACHT  = FALSE;
    
    volgend = BLOK + 1;
    if (volgend > MAX_BLK) volgend = 1;
    
    volgend_plus = volgend + 1;
    if (volgend_plus > MAX_BLK) volgend_plus = 1;
    
    for (i=0; i<FC_MAX; i++) {
      HKI[i] = FCA[i] = MAXG[i]= 0;
      TTK[i] = TTP[i] = TEG[i] = 0;
      POV[i] = PEL[i] = PEL2[i]= FTS[i]= 0;
      RISPEL[i] = RISPEL2[i] = TOV[i]= VSM[i]= VSM_ref[i]= WSP[i]= WSP_ref[i]= RISFTS[i]= 0;
      vracht1[i]= vracht2[i] = 0;
      PRGR[i]= PRTEG[i]= 0;
      
      if (RA[i] || G[i]) A[i] |= BIT2;
      
      if (_GRN[i]) {
        A[i] &= (~BIT2);
        A[i] |=   BIT0;
      }
      
      if (PBLK[i]==BLOK) SGD[i] |=   BIT0;
      else       SGD[i] &= (~BIT0);
      if ((PBLK[i]==volgend) && (MAX_BLK >= 2)) SGD[i] |=   BIT1;
      else                                      SGD[i] &= (~BIT1);
      if ((PBLK[i]==volgend_plus) && PPV2_BLOK(i)) SGD[i] |= BIT1;
      if ((SGD[i]&BIT0) &&
          (GL[i] || TRG[i] || !A[i] || MG[i])) SGD[i] |=  BIT2;
      else                SGD[i] &=(~BIT2);
      SGD[i] &=(~BIT3);
      SGD[i] &=(~BIT4);
      SGD[i] &=(~BIT5);
      SGD[i] &=(~BIT6);
      SGD[i] &=(~BIT7);
      
      for (j=0; j<MAX_VTG_VSM; j++) {
        momenten_vsm[i][j][0] = 0;
        momenten_vsm[i][j][1] = 0;
      }
      
      for (j=0; j<MAX_VTG_WSP; j++) {
        momenten_wsp[i][j][0] = 0;
        momenten_wsp[i][j][1] = 0;
        momenten_wsp[i][j][2] = 0;
        momenten_wsp[i][j][3] = 0;
      }
      
#ifdef C_ITS_FUNCTIONALITEIT
      if (G[i]) tov_eerste[i] = pov_eerste[i] = TRUE;
      for (j=0; j<RIS_MEET_MAX; j++) pel_meet_voertuig[i][j][0] = '\0';
      for (j=0; j<RIS_TOVER_MAX; j++) {
        TOVER[i][j].voertuig_id[0] = '\0';
        TOVER[i][j].duur_aanwezig  = 0;
      }
#endif
      
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure garantie tijden                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_garantie_tijden(void)       /* Fik191001                                                        */
  {
    count i,j;
    bool wijzig = FALSE;
    
    for (i=0; i<FC_MAX; i++) {
      if (TGL_max[i] < TGGL_max[i]) { TGL_max[i] = TGGL_max[i]; wijzig = TRUE; }
      if (TGL_max[i] > 50)          { TGL_max[i] = 50;          wijzig = TRUE; }
      if (TFG_max[i] < TGG_max[i])  { TFG_max[i] = TGG_max[i];  wijzig = TRUE; }
#ifdef INTERGROEN
      for (j=0; j<FC_MAX; j++) {
        if ((TIG_max[i][j] >= 0) && (TGI_max[i][j] >= 0)) {
          if (TIG_max[i][j] < TGI_max[i][j]) { TIG_max[i][j] = TGI_max[i][j]; wijzig = TRUE; }
        }
        if ((TIG_max[j][i] >= 0) && (TGI_max[j][i] >= 0)) {
          if (TIG_max[j][i] < TGI_max[j][i]) { TIG_max[j][i] = TGI_max[j][i]; wijzig = TRUE; }
        }
      }
#else
      for (j=0; j<FC_MAX; j++) {
        if ((TO_max[i][j] >= 0) && (TGO_max[i][j] >= 0)) {
          if (TO_max[i][j] < TGO_max[i][j]) { TO_max[i][j] = TGO_max[i][j]; wijzig = TRUE; }
        }
        if ((TO_max[j][i] >= 0) && (TGO_max[j][i] >= 0)) {
          if (TO_max[j][i] < TGO_max[j][i]) { TO_max[j][i] = TGO_max[j][i]; wijzig = TRUE; }
        }
      }
#endif
    }
    if (wijzig) CIF_PARM1WIJZAP = -2;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure reset variabelen "start" SGD en VAS                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_reset_start(void)           /* Fik190101                                                        */
  {
    count i,j,x;
    bool  YM2;
    
    WBLK = 1;
    SOMEVG = SBL;
    RTFB = HTFB = FALSE;
    for (i=0; i<FC_MAX; i++) {
      for (j=0; j<FC_MAX; j++) {
        TMPc[i][j] = FALSE;
        TMPf[i][j] = FALSE;
        TMPi[i][j] = 0;
        MV_matrix[i][j] = FALSE;
      }
      if (AKTPRG == 0) {
        HVAS[i]= FALSE;
        STAP   = 0;
        
#ifdef _VAS_OV
        
        VAS2a[i] = VAS2b[i] = VAS6[i] = VAS7[i] = 0;
        OVOKVAS[i] = OVAKVAS[i] = 0;
        OVTRVAS[i] = OVAWTVAS[i]= OVINMVVAS[i] = OVAFRTVAS[i] = 0;
        ALTOV[i] = ALTO[i] = ALTE[i] = VAWT[i] = TTO[i] = 0;
        ARRC[i]  = HKAP[i] = FALSE;
        for (j=0; j<FC_MAX; j++) {
          OVTMPVAS[i][j] = FALSE;
        }
        
#endif                                  /* *** ifdef -> _VAS_OV                                             */
        
      } else {
        PRGR[i] = PRTEG[i] = 0;
      }
      
      if (SVAS && !SPR) ESGD |= TRUE;
      if (SVAS || SSGD) {
        HVAS[i]  = FALSE;
        A[i]   &=(~BIT2);
        A[i]   &=(~BIT3);
        A[i]   &=(~BIT4);
        A[i]   &=(~BIT6);
        
#ifdef _VAS_OV
        
        ARRC[i] = FALSE;
        
#endif                                  /* *** ifdef -> _VAS_OV                                             */
        
      }
      YM2 = ((YM[i]&BIT2) != 0);
      if (EVG[i]) SOMEVG |= TRUE;
      if (!G[i])   YV[i]  = FALSE;
      else         Aled[i]= FALSE;
      if (!EVG[i]) MK[i]  = FALSE;
      B[i]=BL[i]=RR[i]=X[i]=RS[i]=RW[i]=YW[i]=FM[i]=YM[i]=Z[i]= FALSE;
      FW[i]   = FALSE;
      
#ifndef ROBUG
      HTVG[i] = TRUE;
#endif
      
#ifdef INGREEP_VIOSS
      if (R[i] &&                     SPW_BLK[i]) BL[i] |= BIT8; /* blokkade door spoor programma           */
      if (G[i] && !VS[i] && !FG[i] && SPW_KAP[i])  Z[i] |= BIT8;
      
      if (R[i] &&                     BRG_BLK[i]) BL[i] |= BIT9; /* blokkade door brug programma            */
      if (G[i] && !VS[i] && !FG[i] && BRG_KAP[i])  Z[i] |= BIT9;
#endif
      
      if (MG[i] && !MK[i] && (AKTPRG == 0) && ((SGD[i]&BIT0) || (SGD[i]&BIT3)) && !(SGD[i]&BIT2)) PRGR[i]=255;
      
      _GRN[i] = ((WTVFC[i]>0) || (_DCF[i]&BIT7)) && RA[i] || _GRN[i] && RA[i];
      
#ifdef INGREEP_VIOSS
      if (BRG_BLK[i]) _GRN[i] = FALSE;
      if (SPW_BLK[i]) _GRN[i] = FALSE;
#endif
      
      if (SG[i] && !AKTPRG && (A[i]&BIT4)) A[i] |= BIT2;
      if ( G[i] || _GRN[i]) A[i] |= BIT0; /* altijd aanvraag indien groen                                   */
      if ( G[i] && !MG[i] && YM2)       /* buffer aanhouden groen agv                                       */
        YM[i] |= BIT2;                  /* ... voetgangerskoppeling                                         */
      BAR[i] = FALSE;                   /* reset blokkeer alt.real.                                         */
      VFA[i] = FALSE;                   /* reset voorlopige fietsaanvraag                                   */
      A[i] &=(~BIT7);                   /* reset start det.aanvraag                                         */
      HKI[i] &=(~BIT4);                 /* reset kap.mg voed.richting                                       */
      HKI[i] &=(~BIT7);                 /* reset NIET kap.mg confl.voed.                                    */
      HKII[i]&=(~BIT3);                 /* reset volgrichting groen                                         */
      BUS1[i] = KVOV[i] = 0;            /* reset BUS aanwezigheidstijd                                      */
      PEL1[i] = KVPK[i] = 0;            /* reset PEL aanwezigheidstijd                                      */
      RISPEL1[i] = KVRPK[i] = 0;        /* reset RIS peloton aanwezigheidstijd                              */
      BPI[i]  = HPRI[i] = 0;            /* reset blokkeer prio.ingreep                                      */
      LEDST[i]= FALSE;                  /* reset detectie storing                                           */
      STAR[i] = 0;                      /* reset star uitverlengen                                          */
      VLOG_OV[i]= 0;                    /* reset VLOG OV- en HLPD bits                                      */
      POV[i] &= (~BIT6);                /* reset BUS opduwen                                                */
      
      POV[i] &= (~BIT1);                /* reset BUS tevroeg aanwezig                                       */
      POV[i] &= (~BIT2);                /* reset BUS op tijd aanwezig                                       */
      POV[i] &= (~BIT3);                /* reset BUS te laat aanwezig                                       */
      
      POV[i] &= (~BIT7);                /* reset BUS tevroeg prioriteit                                     */
      POV[i] &= (~BIT8);                /* reset BUS op tijd prioriteit                                     */
      POV[i] &= (~BIT9);                /* reset BUS te laat prioriteit                                     */
      
      if (SG[i]) { _HOT[i] = 0; VG1[i] = TRUE; }
      if (_HOT[i] > 0)   _HOT[i] += TE; /* onthoud tijd na verlaten koplus                                  */
      if (_HOT[i] > 255) _HOT[i]  = 255; /* ... maximum waarde = 255                                        */
      
      KVFC[i] = 255;
      VAS1[i] = VAS2[i] = VAS3[i] = VAS4[i] = VAS5[i] = 0;
      BLWT[i] = halteer_wtv[i] = FALSE;
      
      ALTM[i] = KAPM[i] = TFG_max[i]/10;
      if ((TGG_max[i]/10) > ALTM[i])
        ALTM[i] = KAPM[i] = TGG_max[i]/10;
      _FBL[i] = TKMX[i] = 0;
    }
    
#ifdef CRSV2CCOL
    proc_reset_crsv_start();
#endif
    
    for (j=0; j<DP_MAX; j++) {
      DF[j] = (OG[j] || BG[j] || FL[j]);
      if (CIF_IS[j]>=CIF_DET_STORING) DF[j] = TRUE;
      
#ifdef isreset
      
      if (CIF_IS[isreset]) {
        TBG_timer[j] = 0;
        TOG_timer[j] = 0;
        TFL_timer[j] = 0;
        BG[j] = OG[j] = FL[j] = DF[j] = 0;
      }
      
#endif                                  /* *** ifdef -> isreset                                             */
      
    }
    /* zet voetgangersfiets tabel in default stand (FK conflicten worden later zonodig teruggezet)          */
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][15] != 0) {
        x = kp_vtg_fts[i][15];
        kp_vtg_fts[i][x] = kp_vtg_fts[i][16];
      }
    }
    
    /* bepaal temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    proc_bepaal_TDH();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure reset variabelen "einde" SGD en VAS                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_reset_einde(void)           /* Fik090211                                                        */
  {
    count i;
    
    Hreset |= SVAS || SSGD;
    RTFB   |= Hreset;
    if (TS) Hreset = FALSE;
    
    SPR = FALSE;
    SSGD= FALSE;
    ESGD= FALSE;
    SVAS= FALSE;
    
#ifdef _VAS_OV
    
    ELOK= FALSE;
    
#endif                                  /* *** ifdef -> _VAS_OV                                             */
    
    for (i=0; i<FC_MAX; i++) {
      if (AKTPRG == 0) {
        if (!(SGD[i]&BIT5) && !(HKI[i]&BIT5)
            && !(HKII[i]&BIT5) && !G[i] && (A[i]&BIT4)) {
          A[i] &= (~BIT4);
          A[i] |=   BIT1;
          A[i] |=   BIT2;
        }
      }
      if (EG[i] || !FG[i] && !(MK[i]&BIT0) && !(MK[i]&BIT1))  { VG1[i]= FALSE; }
      if ((B[i] || RA[i]) && !_GRN[i] && somgk(i) && FIXATIE) { RR[i] = TRUE; }
      if ((B[i] || RA[i]) && !_GRN[i] && !A[i])               { RR[i] = TRUE; }
      if ((AKTPRG == 0) && somfbk(i) && !_GRN[i])             { RR[i] = TRUE; }
      if (!A[i] && (TFB_timer[i]>0) && R[i] && (!_GRN[i] || (Aled[i] == 0))) { BL[i] = TRUE; }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal klokgroen en klokrood                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_klk_gr_r(void)              /* Fik030315                                                        */
  {                                     /* Fik170602                                                        */
    count i,j,k;
    bool reset_OK = FALSE;
    for (i=0; i<FC_MAX; i++) {
      if (SPR) KR[i] = 255;
      if (SG[i]) KG[i] = HG[i] = 0;
      if (EG[i]) KR[i] = 0;
      if (!G[i]) {
        KGCA[i] = KGOM[i] = 0;
        if ((AKTPRG > 0) || (SGD[i]&BIT2) || !(SGD[i]&BIT0) && !(SGD[i]&BIT1)) PRGR[i] = 0;
      }
      if (TS) {
        if ( G[i] && (KG[i]<255))    KG[i]++;
        if ( G[i] && (HG[i]<255))    HG[i]++;
        if ( G[i] && (KGCA[i]<255)) KGCA[i]++;
        if (AKTPRG == 0) {
          if (G[i] && (KGOM[i]<255) && (SGD[i]&BIT4)) KGOM[i]++;
        } else {
          KGOM[i] = KGCA[i];
        }
        if ( G[i] && (PRGR[i]<255) && (AKTPRG == 0) && !(SGD[i]&BIT2) && ((SGD[i]&BIT0) || (SGD[i]&BIT3))) PRGR[i]++;
        if (!G[i] && (KR[i]<255) && !EG[i]) KR[i]++;
      }
      
#ifdef ROBUG
      if (WACHT && WG[i] && (YW[i]&BIT5)) HG[i] = KGCA[i] = KGOM[i] = PRGR[i] = TFG_max[i]/10;
#else
      if (WACHT && WG[i] && (YW[i]&BIT5)) HG[i] = KGCA[i] = KGOM[i] = PRGR[i] = 0;
#endif
      
      if (VG[i] || MG[i]) A[i] &=(~BIT5);
      
      if (!RA[i]) KRVG[i] = 0;
      if (TS && RA[i] && (KRVG[i]<255)) KRVG[i]++;
    }
    for (i=0; i<FC_MAX; i++) {
      reset_OK = TRUE;
      if (FC_type[i] == OV) {
        for (j=0; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if ((BLOK == PBLK[k]) && R[k] && A[k] && !(SGD[k]&BIT2)) reset_OK = FALSE;
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k]) {
            if ((BLOK == PBLK[k]) && R[k] && A[k] && !(SGD[k]&BIT2)) reset_OK = FALSE;
          }
        }
      } else {
        reset_OK = FALSE;
      }
      if (reset_OK && MG[i] && !(YM[i]&BIT4) && !(YM[i]&BIT5)) KGOM[i] = 0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TTR[]                                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bepaal_ttr(void)            /* Fik070207                                                        */
  {                                     /* Fik110724                                                        */
    count i,j,k;
    mulv  max,extra,ontruim;
    
    for (i=0; i<FC_MAX; i++) {
      
      if (G[i]) TTR[i] = TTC[i] = TTW[i] = 0; /* reset TTR                                                  */
      else {
        max = 0;                        /* bepaal TTR                                                       */
        
        if  (GL[i]) max = TGL_max[i]-TGL_timer[i] + TRG_max[i];
        if (TRG[i]) max = TRG_max[i]-TRG_timer[i];
        
        extra = 0;
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (G[k]) {                   /* conflict is groen                                                */
            extra = 2;                  /* 2 sec. extra i.v.m. afrondingsfouten                             */
            if ((TI_max(k,i)>=0) && (TI_max(k,i) > max)) {
              max = TI_max(k,i);
            }
          } else {
            if (!G[k] && (TI_max(k,i)>0) && TI(k,i)) {
              ontruim = TI_max(k,i) - TI_timer(k);
              if (ontruim > max) max = ontruim;
            }
          }
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || !G[k] && (TMPi[k][i]>0)) {
            if (G[k]) {                 /* conflict is groen                                                */
              extra = 2;                /* 2 sec. extra i.v.m. afrondingsfouten                             */
              if (TMPi[k][i] > max) {
                max = TMPi[k][i];
              }
            } else {
              if (!G[k]) {
                ontruim = TMPi[k][i];
                if (ontruim > max) max = ontruim;
              }
            }
          }
        }
        
        if (RA[i] && (extra==2) || RV[i] && (extra==0)) extra = 1;
        TTW[i] =  max;
        TTC[i] = (max/10);
        TTR[i] = (max/10) + extra;
      }
    }
    proc_corr_ttr_vtg();
    dcf_vs_corr_ttr();
    dcf_gs_corr_ttr();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal toestemming prioriteitsingrepen                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_max_wt(void)                /* Fik170907                                                        */
  {
#if defined(twtmmvt) || defined(twtmfts) || defined(twtmvtg)
    count i;
#endif
    
    BLK_PRI_REA = (FILE_VOOR_SS != 0);
    if (AKTPRG == 0) {
      
#ifdef twtmmvt
      
      for (i=0; i<FC_MAX; i++) {
        if ((FC_type[i] != FIETS) && (FC_type[i] != VOETGANGER)) {
          if (R[i] && A[i] && (TFB_timer[i] >= T_max[twtmmvt]/10)) BLK_PRI_REA = TRUE;
        }
      }
      
#endif                                  /* *** ifdef -> twtmmvt                                             */
#ifdef twtmfts
      
      for (i=0; i<FC_MAX; i++) {
        if (FC_type[i] == FIETS) {
          if (R[i] && A[i] && (TFB_timer[i] >= T_max[twtmfts]/10)) BLK_PRI_REA = TRUE;
        }
      }
      
#endif                                  /* *** ifdef -> twtmfts                                             */
#ifdef twtmvtg
      
      for (i=0; i<FC_MAX; i++) {
        if (FC_type[i] == VOETGANGER) {
          if (R[i] && A[i] && (TFB_timer[i] >= T_max[twtmvtg]/10)) BLK_PRI_REA = TRUE;
        }
      }
      
#endif                                  /* *** ifdef -> twtmvtg                                             */
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal fasecyclus instellingen                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fc_instel(                  /* Fik170831                                                        */
    count fc,                           /* fasecyclus                                                       */
    count alt,                          /* prm agar##                                                       */
    count dfst,                         /* prm dfst##                                                       */
    count kap,                          /* prm prag##                                                       */
    count terug,                        /* prm pgtk##                                                       */
    count _hvi,                         /* prm hv##                                                         */
    count _hdub,                        /* t hldp uitmeldbewaking                                           */
    count altva)                        /* prm altva tijdens CRSV                                           */
  {
    mulv _hkap;
    mulv _maxgr = PRIM[fc];
    
#ifdef ROBUG
    if (SCH[schrgv])
    {
      _maxgr = PRIM[fc] - 2;
      if (_maxgr < 0) _maxgr = 0;
    }
#endif
    
    if ((AKTPRG > 0) && !SVAS) _maxgr = MAXG[fc];
    
    if (alt != NG) {
      if ((PRM[alt]/10) > ALTM[fc]) ALTM[fc] = PRM[alt]/10;
      if ((PRM[alt] == 0) && !RA[fc]) BAR[fc] = TRUE;
    }
    
    if (dfst != NG) {
      STAR[fc] = ((_maxgr*PRM[dfst]) / 10) / 10;
    } else {
      STAR[fc] = 0;
    }
    
    if (kap != NG) {
      _hkap = ((_maxgr*PRM[kap]) / 10) / 10;
      if (_hkap > KAPM[fc]) KAPM[fc] = _hkap;
    } else {
      KAPM[fc] = _maxgr;
    }
    
    if (terug != NG) {
      TKMX[fc] = ((_maxgr*PRM[terug]) / 10) / 10;
    } else {
      TKMX[fc] = 0;
    }
    
    if (ALTM[fc] > _maxgr) ALTM[fc] = _maxgr;
    if (STAR[fc] > _maxgr) STAR[fc] = _maxgr;
    if (KAPM[fc] > _maxgr) KAPM[fc] = _maxgr;
    if (TKMX[fc] > _maxgr) TKMX[fc] = _maxgr;
    
#ifdef CRSV2CCOL
    if (AKTPRG > 0) {
      BAR[fc]  = FALSE;
      ALTM[fc] = KAPM[fc] = TFG_max[fc]/10;
      if (TGG_max[fc] > TFG_max[fc]) ALTM[fc] = KAPM[fc] = TGG_max[fc]/10;
      if (altva != NG) {
        if ((PRM[altva]/10) > ALTM[fc]) ALTM[fc] = PRM[altva]/10;
      }
    }
#endif
    
    if (_hvi != NG) {
      if (PRM[_hvi] > 3) {
        PRM[_hvi] = 0;
        CIF_PARM1WIJZAP = -2;
      }                                 /* tijdens aktieve ingreep niet wijzigen                            */
      if (HLPD[fc][0] == 0) HVIN[fc] = PRM[_hvi];
    } else {
      HVIN[fc] = 0;
    }
    if (_hdub != NG) {
      HLPD[fc][4] = T_max[_hdub]/10;
    } else {
      HLPD[fc][4] = 0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure detector aanvragen                                                                             */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_aanvragen(                  /* Fik141219                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* cyclische aanvraag                                               */
    count de1,                          /* detector  de1                                                    */
    count prm1,                         /* parameter de1                                                    */
    count de2,                          /* detector  de2                                                    */
    count prm2,                         /* parameter de2                                                    */
    count de3,                          /* detector  de3                                                    */
    count prm3,                         /* parameter de3                                                    */
    count de4,                          /* detector  de4                                                    */
    count prm4,                         /* parameter de4                                                    */
    count de5,                          /* detector  de5                                                    */
    count prm5,                         /* parameter de5                                                    */
    count de6,                          /* detector  de6                                                    */
    count prm6,                         /* parameter de6                                                    */
    count de7,                          /* detector  de7                                                    */
    count prm7,                         /* parameter de7                                                    */
    count de8,                          /* detector  de8                                                    */
    count prm8,                         /* parameter de8                                                    */
    count de9,                          /* detector  de9                                                    */
    count prm9,                         /* parameter de9                                                    */
    count de10,                         /* detector  de10                                                   */
    count prm10,                        /* parameter de10                                                   */
    count de11,                         /* detector  de11                                                   */
    count prm11,                        /* parameter de11                                                   */
    count de12,                         /* detector  de12                                                   */
    count prm12)                        /* parameter de12                                                   */
  {
    count i;
    bool  aanvraag = FALSE;
    count det,avr;
    
    /* 0: aanvraagfunctie uitgeschakeld                                                                     */
    /* 1: aanvraagfunctie na afloop garantie rood                                                           */
    /* 2: aanvraagfunctie vanaf start rood                                                                  */
    /* 3: aanvraagfunctie vanaf einde groen                                                                 */
    
    if (cyar != NG) {
      if (PRM[cyar] != 0) {
        if ((KR[fc]*10) >= (TGL_max[fc] + PRM[cyar])) aanvraag = TRUE;
      }
    }
    
    for (i=1; i<=12; i++)
    {
      det = NG;
      avr = NG;
      if (i ==  1) { det = de1;  avr = prm1;  }
      if (i ==  2) { det = de2;  avr = prm2;  }
      if (i ==  3) { det = de3;  avr = prm3;  }
      if (i ==  4) { det = de4;  avr = prm4;  }
      if (i ==  5) { det = de5;  avr = prm5;  }
      if (i ==  6) { det = de6;  avr = prm6;  }
      if (i ==  7) { det = de7;  avr = prm7;  }
      if (i ==  8) { det = de8;  avr = prm8;  }
      if (i ==  9) { det = de9;  avr = prm9;  }
      if (i == 10) { det = de10; avr = prm10; }
      if (i == 11) { det = de11; avr = prm11; }
      if (i == 12) { det = de12; avr = prm12; }
      
      if (avr != NG)
      {
        if (PRM[avr] > 3) {
          PRM[avr] = 1;
          CIF_PARM1WIJZAP = -2;
        }
      }
      
      if ((det != NG) && (avr != NG))
      {
        if (R[fc] && DB[det] && !DF[det]) {
          aanvraag |= (PRM[avr] == 1) && !TRG[fc];
          aanvraag |= (PRM[avr] == 2);
        }
        AIGL[fc] |= !G[fc] && DB[det] && !DF[det] && (PRM[avr] == 3);
      }
    }
    
    if (R[fc] && !(A[fc]&BIT1) && (aanvraag || AIGL[fc])) {
      A[fc] |= BIT7;                    /* start detectie aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
    if (R[fc] && A[fc] || G[fc]) AIGL[fc] = FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure voorlopige detector aanvragen fietsrichtingen                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fiets_aanvragen(            /* Fik180101                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* cyclische aanvraag                                               */
    count de1,                          /* detector  de1                                                    */
    count prm1,                         /* parameter de1                                                    */
    count t1,                           /* intrekken de1                                                    */
    count de2,                          /* detector  de2                                                    */
    count prm2,                         /* parameter de2                                                    */
    count t2,                           /* intrekken de2                                                    */
    count de3,                          /* detector  de3                                                    */
    count prm3,                         /* parameter de3                                                    */
    count t3,                           /* intrekken de3                                                    */
    count de4,                          /* detector  de4                                                    */
    count prm4,                         /* parameter de4                                                    */
    count t4,                           /* intrekken de4                                                    */
    count de5,                          /* detector  de5                                                    */
    count prm5,                         /* parameter de5                                                    */
    count t5,                           /* intrekken de5                                                    */
    count de6,                          /* detector  de6                                                    */
    count prm6,                         /* parameter de6                                                    */
    count t6,                           /* intrekken de6                                                    */
    count de7,                          /* detector  de7                                                    */
    count prm7,                         /* parameter de7                                                    */
    count t7,                           /* intrekken de7                                                    */
    count de8,                          /* detector  de8                                                    */
    count prm8,                         /* parameter de8                                                    */
    count t8)                           /* intrekken de8                                                    */
  {
    count i;
    bool  aanvraag = FALSE;
    bool  cyclisch = FALSE;
    count det,avr,tyd;
    
    /* 0: aanvraagfunctie uitgeschakeld                                                                     */
    /* 1: aanvraagfunctie na afloop garantie rood                                                           */
    /* 2: aanvraagfunctie vanaf start rood                                                                  */
    /* 3: aanvraagfunctie vanaf einde groen                                                                 */
    
    if (cyar != NG) {
      if (PRM[cyar] != 0) {
        if ((KR[fc]*10) >= (TGL_max[fc] + PRM[cyar])) cyclisch = TRUE;
      }
    }
    
    for (i=1; i<=8; i++)
    {
      det = NG;
      avr = NG;
      tyd = NG;
      aanvraag = FALSE;
      if (i == 1) { det = de1;  avr = prm1; tyd = t1; }
      if (i == 2) { det = de2;  avr = prm2; tyd = t2; }
      if (i == 3) { det = de3;  avr = prm3; tyd = t3; }
      if (i == 4) { det = de4;  avr = prm4; tyd = t4; }
      if (i == 5) { det = de5;  avr = prm5; tyd = t5; }
      if (i == 6) { det = de6;  avr = prm6; tyd = t6; }
      if (i == 7) { det = de7;  avr = prm7; tyd = t7; }
      if (i == 8) { det = de8;  avr = prm8; tyd = t8; }
      
      if (avr != NG)
      {
        if (PRM[avr] > 3) {
          PRM[avr] = 1;
          CIF_PARM1WIJZAP = -2;
        }
      }
      
      if ((det != NG) && (avr != NG))
      {
        if (R[fc] && DB[det] && !DF[det]) {
          aanvraag |= (PRM[avr] == 1) && !TRG[fc];
          aanvraag |= (PRM[avr] == 2);
        }
        aanvraag |= !G[fc] && DB[det] && !DF[det] && (PRM[avr] == 3);
      }
      
      if (tyd != NG) {
        AT[tyd] = FALSE;
        RT[tyd] = aanvraag;
        if (RT[tyd] || T[tyd]) VFA[fc] |= BIT0;
        if (G[fc]) {
          AT[tyd] = T[tyd];
          RT[tyd] = FALSE;
        }
        if (det != NG) {
          if (D[det] && !DF[det] && (DE_type[det] == KOPLUS)) VFA[fc] |= BIT1;
        }
        if (T_max[tyd] == 0) {
          cyclisch |= aanvraag;
          AIGL[fc] |= GL[fc] && aanvraag;
        }
      } else {
        cyclisch |= aanvraag;
        AIGL[fc] |= GL[fc] && aanvraag;
      }
    }
    
    if (R[fc] && !(A[fc]&BIT1) && (cyclisch || AIGL[fc])) {
      A[fc] |= BIT7;                    /* start detectie aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
    if (R[fc] && A[fc] || G[fc]) AIGL[fc] = FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal fietsaanvragen                                                                          */
/* -------------------------------------------------------------------------------------------------------- */
  void bepaal_fiets_aanvragen(void)     /* Fik180101                                                        */
  {
    count i;
    /* VFA bit0: voorlopige aanvraag aanwezig                                                               */
    /* VFA bit1: koplus bezet dus aanvraag niet intrekken                                                   */
    
    for (i=0; i<FC_MAX; i++)
    {
      if (RV[i] && (Aled[i] == 0) && (FC_type[i] == FIETS) && !(A[i]&BIT1))
      {
        if (VFA[i]&BIT0) {
          if (!(A[i]&BIT9)) A[i] |= BIT7; /* start detectie aanvraag                                        */
          A[i] |= BIT9;
        }
        else {
          if (!(VFA[i]&BIT1) && !(A[i]&BIT1) && !(A[i]&BIT3) &&
              !(A[i]&BIT5) && !(A[i]&BIT6) && !(A[i]&BIT8)) A[i] = FALSE;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure drukknop aanvraag                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_aanv_drk(                   /* Fik120307                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* cyc.aanvraag                                                     */
    count schdst,                       /* det.storing                                                      */
    count drk,                          /* drukknop                                                         */
    count mmdrk,                        /* MM  drk                                                          */
    count avr)                          /* PRM drk                                                          */
  {
    bool aanvraag = FALSE;
    
    /* 0: aanvraagfunctie uitgeschakeld                                                                     */
    /* 1: aanvraagfunctie na afloop garantie rood                                                           */
    /* 2: aanvraagfunctie vanaf start rood                                                                  */
    /* 3: aanvraagfunctie vanaf einde groen                                                                 */
    
    if (avr != NG)
    {
      if (PRM[avr] > 3) {
        PRM[avr] = 3;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (cyar != NG) {
      if (PRM[cyar] != 0) {
        if ((KR[fc]*10) >= (TGL_max[fc] + PRM[cyar])) aanvraag = TRUE;
      }
    }
    
    if (EG[fc]) MM[mmdrk] = FALSE;
    if (!G[fc] && D[drk] && !DF[drk]) {
      MM[mmdrk] |=  R[fc] && (PRM[avr] == 1) && !TRG[fc];
      MM[mmdrk] |=  R[fc] && (PRM[avr] == 2);
      MM[mmdrk] |= !G[fc] && (PRM[avr] == 3);
    }
    
    if (R[fc] && DF[drk] && SCH[schdst]) MM[mmdrk] |= TRUE;
    
    if (R[fc] && !(A[fc]&BIT1) && (MM[mmdrk] || aanvraag)) {
      A[fc] |= BIT7;                    /* Start detectie-aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure cyclische aanvraag                                                                             */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_aanv_cyc(                   /* Fik120514                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar)                         /* cyc.aanvraag                                                     */
  {
    bool dex = FALSE;
    if (cyar != NG) {
      if (PRM[cyar] != 0) {
        if ((KR[fc]*10) >= (TGL_max[fc] + PRM[cyar])) dex = TRUE;
      }
    }
    if (R[fc] && !(A[fc]&BIT1) && dex) {
      A[fc] |= BIT7;                    /* Start detectie-aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure SET detectie-aanvraag                                                                          */
/* -------------------------------------------------------------------------------------------------------- */
  void set_A(                           /* Fik090210                                                        */
    count fc)                           /* fasecyclus                                                       */
  {
    if (R[fc] && !(A[fc]&BIT1)) {
      A[fc] |= BIT7;                    /* Start detectie-aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure SET mee-aanvraag fiets                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void set_fts_MA(                      /* Fik191001                                                        */
    count fc1,                          /* fasecyclus MEE                                                   */
    count fc2)                          /* fasecyclus MET                                                   */
  {
    if (R[fc1] && !(A[fc1]&BIT1)) {
      if (R[fc2] && (A[fc2]&BIT1)) {
        A[fc1] |= BIT7;                 /* Start detectie-aanvraag                                          */
        A[fc1] |= BIT1;
        A[fc1] &= (~BIT3);
        A[fc1] &= (~BIT6);
      }
    }
    if (R[fc1] && !(A[fc1]&BIT1)) {
      if (R[fc2] && (A[fc2]&BIT9)) {
        if (!(A[fc1]&BIT9)) A[fc1] |= BIT7;
        A[fc1] |= BIT9;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure aanhouden verlenggroen                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_verlengen(                  /* Fik180101                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tt,                           /* bep.maximum                                                      */
    count de1,                          /* detector  de1                                                    */
    count prm1,                         /* parameter de1                                                    */
    count de2,                          /* detector  de2                                                    */
    count prm2,                         /* parameter de2                                                    */
    count de3,                          /* detector  de3                                                    */
    count prm3,                         /* parameter de3                                                    */
    count de4,                          /* detector  de4                                                    */
    count prm4,                         /* parameter de4                                                    */
    count de5,                          /* detector  de5                                                    */
    count prm5,                         /* parameter de5                                                    */
    count de6,                          /* detector  de6                                                    */
    count prm6,                         /* parameter de6                                                    */
    count de7,                          /* detector  de7                                                    */
    count prm7,                         /* parameter de7                                                    */
    count de8,                          /* detector  de8                                                    */
    count prm8,                         /* parameter de8                                                    */
    count de9,                          /* detector  de9                                                    */
    count prm9,                         /* parameter de9                                                    */
    count de10,                         /* detector  de10                                                   */
    count prm10,                        /* parameter de10                                                   */
    count de11,                         /* detector  de11                                                   */
    count prm11,                        /* parameter de11                                                   */
    count de12,                         /* detector  de12                                                   */
    count prm12)                        /* parameter de12                                                   */
  {
    count i;
    count det,avg;
    
#ifdef prmdhdvmkop
    
    bool TDHdvm;
    mulv dvm_tdh;
    
#endif                                  /* *** ifdef -> prmdhdvmkop                                         */
    
    RT[tt] = SG[fc];
    
    for (i=1; i<=12; i++)
    {
      det = NG;
      avg = NG;
      if (i ==  1) { det = de1;  avg = prm1;  }
      if (i ==  2) { det = de2;  avg = prm2;  }
      if (i ==  3) { det = de3;  avg = prm3;  }
      if (i ==  4) { det = de4;  avg = prm4;  }
      if (i ==  5) { det = de5;  avg = prm5;  }
      if (i ==  6) { det = de6;  avg = prm6;  }
      if (i ==  7) { det = de7;  avg = prm7;  }
      if (i ==  8) { det = de8;  avg = prm8;  }
      if (i ==  9) { det = de9;  avg = prm9;  }
      if (i == 10) { det = de10; avg = prm10; }
      if (i == 11) { det = de11; avg = prm11; }
      if (i == 12) { det = de12; avg = prm12; }
      
      if (avg != NG)
      {
        if (PRM[avg] > 15) {
          PRM[avg] = 0;
          CIF_PARM1WIJZAP = -2;
        }
      }
      
      if (det != NG)
      {
        if ((DE_type[det] == KOPLUS) && ED[det] && !DF[det] && TRG[fc]) _HOT[fc] = 1;
      }
      
      if ((det != NG) && (avg != NG))
      {
        
#ifdef prmdhdvmkop
        
        TDHdvm  = FALSE;
        dvm_tdh = 0;
        if (DE_type[det] == KOPLUS)    dvm_tdh = PRM[prmdhdvmkop];
        if (DE_type[det] == LANGE_LUS) dvm_tdh = PRM[prmdhdvmlang];
        if (DE_type[det] == VERWEGLUS) dvm_tdh = PRM[prmdhdvmver];
        
#endif                                  /* *** ifdef -> prmdhdvmkop                                         */
        
        if (PRM[avg]&BIT0)
        {
          if (G[fc] && (RT[tt] || T[tt])) {
            if (TDH[det] && !DF[det] && !EVG[fc]) MK[fc] |= BIT0;
          }
        }
        
        if (PRM[avg]&BIT1)
        {
          if (TDH[det] && !DF[det] && !EVG[fc]) MK[fc] |= BIT1;
        }
        
        if (PRM[avg]&BIT2)
        {
          if (TDH[det] && !DF[det] && !EVG[fc]) MK[fc] |= BIT2;
        }
        
#ifdef prmdhdvmkop
        
        if (FCDVM[fc] && (FC_type[fc] == AUTO)) {
          if (!DF[det] && (_TDH[det] < dvm_tdh)) TDHdvm = TRUE;
        }
        
        if (PRM[avg]&BIT0)
        {
          if (G[fc] && (RT[tt] || T[tt]) && TDHdvm && !EVG[fc]) MK[fc] |= BIT0;
        }
        
        if (PRM[avg]&BIT1)
        {
          if (TDHdvm  && !EVG[fc]) MK[fc] |= BIT1;
        }
        
        if (PRM[avg]&BIT2)
        {
          if (TDHdvm  && !EVG[fc]) MK[fc] |= BIT2;
        }
        
#endif                                  /* *** ifdef -> prmdhdvmkop                                         */
        
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure verlengen op basis van Meester Drukknop                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vag1_vtg1(                  /* Fik180901                                                        */
    count fc,                           /* fasecyclus                                                       */
    count drk,                          /* drukknop                                                         */
    count verl)                         /* verlengen gewenst                                                */
  {
    if (SG[fc]) MM[verl] = FALSE;
    if (FG[fc] && !DF[drk] && SD[drk]) MM[verl] = TRUE;
    
    if (MM[verl] && TDH[drk] && !DF[drk] && !EVG[fc]) MK[fc] |= BIT1;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure aanhouden veiligheidsgroen (VAG4)                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vag4(                       /* Fik180408                                                        */
    count fc,                           /* fasecyclus                                                       */
    count max4,                         /* T VAG4                                                           */
    count maxvt,                        /* maximale volgtijd                                                */
    count de1,                          /* detector  de1                                                    */
    count prm1,                         /* parameter de1                                                    */
    count de2,                          /* detector  de2                                                    */
    count prm2,                         /* parameter de2                                                    */
    count de3,                          /* detector  de3                                                    */
    count prm3,                         /* parameter de3                                                    */
    count de4,                          /* detector  de4                                                    */
    count prm4,                         /* parameter de4                                                    */
    count de5,                          /* detector  de5                                                    */
    count prm5,                         /* parameter de5                                                    */
    count de6,                          /* detector  de6                                                    */
    count prm6,                         /* parameter de6                                                    */
    count de7,                          /* detector  de7                                                    */
    count prm7,                         /* parameter de7                                                    */
    count de8,                          /* detector  de8                                                    */
    count prm8,                         /* parameter de8                                                    */
    count de9,                          /* detector  de9                                                    */
    count prm9,                         /* parameter de9                                                    */
    count de10,                         /* detector  de10                                                   */
    count prm10,                        /* parameter de10                                                   */
    count de11,                         /* detector  de11                                                   */
    count prm11,                        /* parameter de11                                                   */
    count de12,                         /* detector  de12                                                   */
    count prm12)                        /* parameter de12                                                   */
  {
    count i;
    count det,avg;
    bool  vgroen;
    bool  dizo = FALSE;
    bool  nul_mag = TRUE;
    mulv  aantal = 0;
    mulv  max_volgtijd = NG;
    if (maxvt != NG) max_volgtijd = T_max[maxvt];
    
#ifdef C_ITS_FUNCTIONALITEIT
    nul_mag = FALSE;
#endif
    
    RT[max4] = G[fc] && (!MG[fc] || YM[fc]) && (T_max[max4] > 0);
    
    if ( (RT[max4] || T[max4]) && (T_max[max4] > DZ_tijd[fc]) ) DZ_tijd[fc] = T_max[max4];
    
    if (!G[fc]) {
      DZ[fc] = FALSE;
      DZ_tijd[fc] = NG;
    }
    if ((WG[fc] || VG[fc] || MG[fc]) && !MK[fc]) DZ[fc] |= TRUE; /* hiaat gemeten                           */
    
    for (i=1; i<=12; i++)
    {
      det = NG;
      avg = NG;
      if (i ==  1) { det = de1;  avg = prm1;  }
      if (i ==  2) { det = de2;  avg = prm2;  }
      if (i ==  3) { det = de3;  avg = prm3;  }
      if (i ==  4) { det = de4;  avg = prm4;  }
      if (i ==  5) { det = de5;  avg = prm5;  }
      if (i ==  6) { det = de6;  avg = prm6;  }
      if (i ==  7) { det = de7;  avg = prm7;  }
      if (i ==  8) { det = de8;  avg = prm8;  }
      if (i ==  9) { det = de9;  avg = prm9;  }
      if (i == 10) { det = de10; avg = prm10; }
      if (i == 11) { det = de11; avg = prm11; }
      if (i == 12) { det = de12; avg = prm12; }
      
      if (avg != NG)
      {
        if (PRM[avg] > 15) {
          PRM[avg] = 0;
          CIF_PARM1WIJZAP = -2;
        }
      }
      
      if ((det != NG) && (avg != NG))
      {
        if (PRM[avg]&BIT3)
        {
          if (SD[det] && !DF[det] && (_VT4_old[det] > 0) && ((_VT4_old[det] <= max_volgtijd) || (max_volgtijd == NG))) {
            _VG4[det] = TRUE;
          }
          if (!TDH[det] || DF[det]) _VG4[det] = FALSE;
          if (_VG4[det]) dizo = TRUE;
        }
        else
        {
          _VG4[det] = FALSE;
        }
      }
    }
    
    aantal = 0;
#ifdef C_ITS_FUNCTIONALITEIT
    aantal = VG4_GEBIED[fc];
#endif
    
    vgroen = MG[fc] && DZ[fc] && (dizo || (aantal >= 2)) && !(SGD[fc]&BIT7) && (RT[max4] || T[max4] ||
               (T_max[max4] == 0) && nul_mag);
    if ((KGCA[fc]>=PRIM[fc]) && (T_max[max4] == 0)) vgroen = FALSE;
    
#ifdef CRSV2CCOL
    if (AKTPRG > 0) vgroen = FALSE;
#endif
    
    if (MG[fc] && vgroen) {
      YM[fc] |= BIT6;
      if (!EVG[fc]) MK[fc] |= BIT3;     /* tbv kwaliteitscentrale                                           */
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Richtinggevoelig meetpunt                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  void rg_meetpunt(                     /* Fik180101                                                        */
    count det1,                         /* detector 1 vanaf de stopstreep                                   */
    count det2,                         /* detector 2 vanaf de stopstreep                                   */
    count trgv,                         /* richtinggevoelige hiaattijd                                      */
    count traf,                         /* max.onbezettijd detector 2                                       */
    count _rgv)                         /* MM rgv meetpunt bezet                                            */
  {
    /* MM bit 0: meetpunt bezet tbv aanvraag                                                                */
    /* MM bit 1: richtinggevoelige hiaatmeting aktief                                                       */
    /* (als 1 detector stoort neemt de werkende detector de verlengfunctie over)                            */
    /* MM bit 2: richtinggevoelige hiaatmeting aktief tijdens DVM of FILE stroomopwaarts (RWS)              */
    /* MM bit 3: puls koplus nabij stopstreep verlaten                                                      */
    /* MM bit 4: puls start bezet richting gevoelig meetpunt                                                */
    /* MM bit 5: richting gevoelig hiaatmeting aktief tbv veiligheidsgroen                                  */
    
    bool TDHdvm = FALSE;
    
#ifdef prmdhdvmkop
    
    mulv dvm_tdh= 0;
    if (DE_type[det1] == KOPLUS)    dvm_tdh = PRM[prmdhdvmkop];
    if (DE_type[det1] == LANGE_LUS) dvm_tdh = PRM[prmdhdvmlang];
    if (DE_type[det1] == VERWEGLUS) dvm_tdh = PRM[prmdhdvmver];
    if (!DF[det1] && (_TDH[det1] < dvm_tdh)) TDHdvm = TRUE;
    
    dvm_tdh= 0;
    if (DE_type[det2] == KOPLUS)    dvm_tdh = PRM[prmdhdvmkop];
    if (DE_type[det2] == LANGE_LUS) dvm_tdh = PRM[prmdhdvmlang];
    if (DE_type[det2] == VERWEGLUS) dvm_tdh = PRM[prmdhdvmver];
    if (!DF[det2] && (_TDH[det2] < dvm_tdh)) TDHdvm = TRUE;
    
#endif                                  /* *** ifdef -> prmdhdvmkop                                         */
    
    MM[_rgv] &= (~BIT4);
    RT[trgv] = FALSE;
    if (DF[det1] || DF[det2]) {
      MM[_rgv] &= (~BIT0);
      if (DF[det1] && !DF[det2]) {
        RT[trgv] = D[det2];
      }
      if (DF[det2] && !DF[det1]) {
        RT[trgv] = D[det1];
      }
    } else {
      if (SD[det1] && (D[det2] || (_TDH[det2]<=T_max[traf]))) {
        MM[_rgv] |= BIT0;
        MM[_rgv] |= BIT4;
      }
      if (!D[det1] && !D[det2] && (_TDH[det2]> T_max[traf]) ) {
        MM[_rgv] &= (~BIT0);
      }
      if (MM[_rgv]&BIT0) RT[trgv] = TRUE;
    }
    
    if (RT[trgv] || T[trgv]) MM[_rgv] |= BIT1;
    else MM[_rgv] &= (~BIT1);
    
    if ((MM[_rgv]&BIT0) || D[det1] && DF[det2] || D[det2] && DF[det1]) MM[_rgv] |= BIT2;
    else if (!TDHdvm)  MM[_rgv] &= (~BIT2);
    
    MM[_rgv] &= (~BIT3);
    if ((DE_type[det1] == KOPLUS) && ED[det1] && !DF[det1]) MM[_rgv] |= BIT3;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Richtinggevoelige aanvraag                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_aanv_rga(                   /* Fik120804                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* PRM cyclische aanvraag                                           */
    count _rgv,                         /* MM  rga meetpunt bezet                                           */
    count avr)                          /* PRM aanvraag toegestaan                                          */
  {
    bool aanvraag = FALSE;
    
    /* 0: aanvraagfunctie uitgeschakeld                                                                     */
    /* 1: aanvraagfunctie na afloop garantie rood                                                           */
    /* 2: aanvraagfunctie vanaf start rood                                                                  */
    /* 3: aanvraagfunctie vanaf einde groen                                                                 */
    
    if (avr != NG)
    {
      if (PRM[avr] > 3) {
        PRM[avr] = 1;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (cyar != NG) {
      if (PRM[cyar] != 0) {
        if ((KR[fc]*10) >= (TGL_max[fc] + PRM[cyar])) aanvraag = TRUE;
      }
    }
    
    if (avr != NG)
    {
      if (R[fc] && (MM[_rgv]&BIT0)) {
        aanvraag |= (PRM[avr] == 1) && !TRG[fc];
        aanvraag |= (PRM[avr] == 2);
      }
      AIGL[fc] |= !G[fc] && (MM[_rgv]&BIT0) && (PRM[avr] == 3);
    }
    
    if (R[fc] && !(A[fc]&BIT1) && (aanvraag || AIGL[fc])) {
      A[fc] |= BIT7;                    /* start detectie aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
    if (R[fc] && A[fc] || G[fc]) AIGL[fc] = FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Richtinggevoelige fietsaanvraag                                                                          */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fiets_rga(                  /* Fik180101                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* PRM cyclische aanvraag                                           */
    count _rgv,                         /* MM  rga meetpunt bezet                                           */
    count avr,                          /* PRM aanvraag toegestaan                                          */
    count tyd)                          /* T   intrekken aanvraag                                           */
  {
    bool aanvraag = FALSE;
    bool cyclisch = FALSE;
    
    /* 0: aanvraagfunctie uitgeschakeld                                                                     */
    /* 1: aanvraagfunctie na afloop garantie rood                                                           */
    /* 2: aanvraagfunctie vanaf start rood                                                                  */
    /* 3: aanvraagfunctie vanaf einde groen                                                                 */
    
    if (avr != NG)
    {
      if (PRM[avr] > 3) {
        PRM[avr] = 1;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (cyar != NG) {
      if (PRM[cyar] != 0) {
        if ((KR[fc]*10) >= (TGL_max[fc] + PRM[cyar])) cyclisch = TRUE;
      }
    }
    
    if (avr != NG)
    {
      if (R[fc] && (MM[_rgv]&BIT0)) {
        aanvraag |= (PRM[avr] == 1) && !TRG[fc];
        aanvraag |= (PRM[avr] == 2);
      }
      aanvraag |= !G[fc] && (MM[_rgv]&BIT0) && (PRM[avr] == 3);
    }
    
    if (tyd != NG) {
      AT[tyd] = FALSE;
      RT[tyd] = aanvraag;
      if (RT[tyd] || T[tyd]) VFA[fc] |= BIT0;
      if (G[fc]) {
        AT[tyd] = T[tyd];
        RT[tyd] = FALSE;
      }
      if (T_max[tyd] == 0) {
        cyclisch |= aanvraag;
        AIGL[fc] |= GL[fc] && aanvraag;
      }
    } else {
      cyclisch |= aanvraag;
      AIGL[fc] |= GL[fc] && aanvraag;
    }
    
    if (R[fc] && !(A[fc]&BIT1) && (cyclisch || AIGL[fc])) {
      A[fc] |= BIT7;                    /* start detectie aanvraag                                          */
      A[fc] |= BIT1;
      A[fc] &= (~BIT3);
      A[fc] &= (~BIT6);
    }
    if (R[fc] && A[fc] || G[fc]) AIGL[fc] = FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Richtinggevoelig verlengen                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void rgv_verlengen(                   /* Fik180101                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tbep,                         /* beperkt meetkriterium                                            */
    count _rgv,                         /* MM  rgv meetpunt bezet                                           */
    count avg)                          /* PRM verlengfunctie                                               */
  {
    RT[tbep] = SG[fc];
    if ((MM[_rgv]&BIT3) && TRG[fc]) _HOT[fc] = 1;
    
    if (PRM[avg] > 15) {
      PRM[avg] = 0;
      CIF_PARM1WIJZAP = -2;
    }
    if (PRM[avg]&BIT0)
    {
      if (G[fc] && (RT[tbep] || T[tbep])) {
        if ((MM[_rgv]&BIT1) && !EVG[fc]) MK[fc] |= BIT0;
      }
      if (FCDVM[fc] && (FC_type[fc] == AUTO) && G[fc] && (RT[tbep] || T[tbep])) {
        if ((MM[_rgv]&BIT2) && !EVG[fc]) MK[fc] |= BIT0;
      }
    }
    
    if (PRM[avg]&BIT1)
    {
      if (G[fc] && (MM[_rgv]&BIT1) && !EVG[fc]) MK[fc] |= BIT1;
      if (FCDVM[fc] && (FC_type[fc] == AUTO) && G[fc]) {
        if ((MM[_rgv]&BIT2) && !EVG[fc]) MK[fc] |= BIT1;
      }
    }
    
    if (PRM[avg]&BIT2)
    {
      if (G[fc] && (MM[_rgv]&BIT1) && !EVG[fc]) MK[fc] |= BIT2;
      if (FCDVM[fc] && (FC_type[fc] == AUTO) && G[fc]) {
        if ((MM[_rgv]&BIT2) && !EVG[fc]) MK[fc] |= BIT2;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure aanhouden veiligheidsgroen (richtingsgevoelig)                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vag4_rgv(                   /* Fik180408                                                        */
    count fc,                           /* fasecyclus                                                       */
    count max4,                         /* T VAG4                                                           */
    count maxvt,                        /* maximale volgtijd                                                */
    count _rgv,                         /* MM  rgv meetpunt bezet                                           */
    count _vt_rgv,                      /* MM  rgv meetpunt volgtijd                                        */
    count avg)                          /* PRM verlengfunctie                                               */
  {
    bool vgroen;
    bool nul_mag = TRUE;
    mulv max_volgtijd = NG;
    if (maxvt != NG) max_volgtijd = T_max[maxvt];
    
#ifdef C_ITS_FUNCTIONALITEIT
    nul_mag = FALSE;
#endif
    
    RT[max4] = G[fc] && (!MG[fc] || YM[fc] && (YM[fc] != BIT6)) && (T_max[max4] > 0);
    
    if ( (RT[max4] || T[max4]) && (T_max[max4] > DZ_tijd[fc]) ) DZ_tijd[fc] = T_max[max4];
    
    if (SPR) MM[_vt_rgv] = 255;
    
    if (!G[fc]) {
      DZ[fc] = FALSE;
      DZ_tijd[fc] = NG;
    }
    if ((WG[fc] || VG[fc] || MG[fc]) && !MK[fc]) DZ[fc] |= TRUE; /* hiaat gemeten                           */
    
    if (PRM[avg] > 15) {
      PRM[avg] = 0;
      CIF_PARM1WIJZAP = -2;
    }
    
    if (PRM[avg]&BIT3)
    {
      if ( (MM[_rgv]&BIT4) && (MM[_vt_rgv] > 0) && ((MM[_vt_rgv] <= max_volgtijd) || (max_volgtijd == NG)))
        MM[_rgv] |= BIT5;
      if (!(MM[_rgv]&BIT1)) MM[_rgv] &= (~BIT5);
    } else {
      MM[_rgv] &= (~BIT5);
    }
    
    if (MM[_rgv]&BIT4) {
      MM[_vt_rgv] = 0;
    } else {
      MM[_vt_rgv] += TE;
    }
    if (MM[_vt_rgv] > 255) MM[_vt_rgv] = 255;
    
    vgroen = MG[fc] && DZ[fc] && (MM[_rgv]&BIT5) && !(SGD[fc]&BIT7) &&
             (RT[max4] || T[max4] || (T_max[max4] == 0) &&  nul_mag);
    if ((KGCA[fc]>=PRIM[fc]) && (T_max[max4] == 0)) vgroen = FALSE;
    
#ifdef CRSV2CCOL
    if (AKTPRG > 0) vgroen = FALSE;
#endif
    
    if (MG[fc] && vgroen) {
      YM[fc] |= BIT6;
      if (!EVG[fc]) MK[fc] |= BIT3;     /* tbv kwaliteitscentrale                                           */
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Maatregelen bij detectie storing auto                                                                    */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_det_mvt(                    /* Kinzel 17-05-2019 - Fik191026 geen aanvraag als NUL ingesteld    */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count de1,                          /* index koplus                                                     */
    count de2,                          /* index lange lus                                                  */
    count de3,                          /* index 1e verweglus                                               */
    count de4,                          /* index 2e verweglus                                               */
    count de5,                          /* index 3e verweglus                                               */
    count tdhvkop,                      /* index tijdelement vervangende hiaattijd koplus                   */
    count tdhv1,                        /* index tijdelement vervangende hiaattijd 1e verweglus             */
    count tdhv2,                        /* index tijdelement vervangende hiaattijd 2e verweglus             */
    count tdhv3)                        /* index tijdelement vervangende hiaattijd 3e verweglus             */
  {
    bool vertraag = FALSE;
    bool VERLENG_TDH = FALSE;
    bool maatregel_aan = TRUE;
    
    if (sch_aan != NG)
    {
      if (!SCH[sch_aan]) {
        maatregel_aan = FALSE;
      }
    }
    
    if (ta != NG) {
      RT[ta] = SPR || SR[fc];
      if (RT[ta] || T[ta] || (T_max[ta] == 0)) {
        vertraag = TRUE;
      }
    }
    
    if ((de1 != NG) && (tdhvkop != NG)) {
      RT[tdhvkop] = D[de1] && !DF[de1];
    }
    
    if (maatregel_aan) {
      
      /* vervangende verlengfunctie koplus                                                                  */
      if ((de1 != NG) && (de2 != NG) && (tdhvkop != NG)) {
        if (!DF[de1] && DF[de2] && !EVG[fc] && (RT[tdhvkop] || T[tdhvkop])) {
          MK[fc] |= BIT1;
        }
      }
      
      /* tijdvertraagde aanvraag bij defecte kop- en lange lus                                              */
      /* en star verlengen gedurende een percentage van de max. groentijd                                   */
      if ((de1 != NG) && (de2 == NG)) {
        if (DF[de1] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
        if (DF[de1] && !EVG[fc] && (KG[fc]<=STAR[fc])) {
          MK[fc] |= (BIT1 | BIT2);
        }
      }
      
      if ((de2 != NG) && (de1 == NG)) {
        if (DF[de2] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
        if (DF[de2] && !EVG[fc] && (KG[fc]<=STAR[fc])) {
          MK[fc] |= (BIT1 | BIT2);
        }
      }
      
      if ((de1 != NG) && (de2 != NG)) {
        if (DF[de1] && DF[de2] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
        if (DF[de1] && DF[de2] && !EVG[fc] && (KG[fc]<=STAR[fc])) {
          MK[fc] |= (BIT1 | BIT2);
        }
      }
      
      /* vervangende hiaattijden 1e verweglus                                                               */
      if (de3 != NG) {
        VERLENG_TDH = FALSE;
        if (de2 != NG)              { if (DF[de2])            VERLENG_TDH = TRUE; } /* lange lus            */
        if (de4 != NG && de5 != NG) { if (DF[de4] && DF[de5]) VERLENG_TDH = TRUE; } /* 2e en 3e verweglus   */
        else if (de4 != NG)         { if (DF[de4])            VERLENG_TDH = TRUE; } /* 2e verweglus         */
        else if (de5 != NG)         { if (DF[de5])            VERLENG_TDH = TRUE; } /* 3e verweglus         */
        if (!DF[de3] && VERLENG_TDH && (_TDH[de3] < T_max[tdhv1]) && !EVG[fc]) {
          MK[fc] |= BIT2;
        }
      }
      
      /* vervangende hiaattijden 2e verweglus                                                               */
      if (de4 != NG) {
        VERLENG_TDH = FALSE;
        if (de3 != NG) { if (DF[de3]) VERLENG_TDH = TRUE; } /* 1e verweglus                                 */
        if (!DF[de4] && VERLENG_TDH && (_TDH[de4] < T_max[tdhv2]) && !EVG[fc]) {
          MK[fc] |= BIT2;
        }
      }
      
      /* vervangende hiaattijden 3e verweglus                                                               */
      if (de5 != NG) {
        VERLENG_TDH = FALSE;
        if (de3 != NG) { if (DF[de3]) VERLENG_TDH = TRUE; } /* 1e verweglus                                 */
        if (de4 != NG) { if (DF[de4]) VERLENG_TDH = TRUE; } /* 2e verweglus                                 */
        if (!DF[de5] && VERLENG_TDH && (_TDH[de5] < T_max[tdhv3]) && !EVG[fc]) {
          MK[fc] |= BIT2;
        }
      }
    }
    
    /* variabelen vullen t.b.v. uitangssignalen detectiestoring                                             */
    if ((de1 != NG) && (de2 == NG)) {
      if (DF[de1]) LEDST[fc] |= TRUE;
    }
    if ((de2 != NG) && (de1 == NG)) {
      if (DF[de2]) LEDST[fc] |= TRUE;
    }
    if ((de1 != NG) && (de2 != NG)) {
      if (DF[de1] && DF[de2]) LEDST[fc] |= TRUE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Maatregelen bij detectie storing fiets                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_det_fts(                    /* Kinzel 17-05-2019                                                */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count dk,                           /* index drukknop                                                   */
    count de1,                          /* index koplus 1                                                   */
    count de2)                          /* index koplus 2                                                   */
  {
    bool vertraag = FALSE;
    bool all_fout = TRUE;
    bool maatregel_aan = TRUE;
    
    if (sch_aan != NG)
    {
      if (!SCH[sch_aan]) {
        maatregel_aan = FALSE;
      }
    }
    
    if (ta != NG) {
      RT[ta] = SPR || SR[fc];
      if (RT[ta] || T[ta]) {
        vertraag = TRUE;
      }
    }
    
    if (maatregel_aan) {
      
      if (dk  != NG) if (!DF[dk] ) all_fout = FALSE;
      if (de1 != NG) if (!DF[de1]) all_fout = FALSE;
      if (de2 != NG) if (!DF[de2]) all_fout = FALSE;
      
      /* tijdvertraagde aanvraag bij defecte drukknop en koplussen                                          */
      if (all_fout && !vertraag) {
        set_A(fc);
      }
      
      /* star verlengen gedurende een percentage van de max. groentijd                                      */
      if ((de1 != NG) && (de2 == NG)) {
        if (DF[de1] && !EVG[fc] && (KG[fc]<STAR[fc])) {
          MK[fc] |= (BIT1 | BIT2);
        }
      }
      if ((de2 != NG) && (de1 == NG)) {
        if (DF[de2] && !EVG[fc] && (KG[fc]<STAR[fc])) {
          MK[fc] |= (BIT1 | BIT2);
        }
      }
      if ((de1 != NG) && (de2 != NG)) {
        if (DF[de1] && DF[de2] && !EVG[fc] && (KG[fc]<STAR[fc])) {
          MK[fc] |= (BIT1 | BIT2);
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Maatregelen bij detectie storing voetganger                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_det_vtg(                    /* Kinzel 17-05-2019                                                */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count dk1,                          /* index drukknop 1                                                 */
    count dk2)                          /* index drukknop 2                                                 */
  {
    bool vertraag = FALSE;
    bool maatregel_aan = TRUE;
    
    if (sch_aan != NG)
    {
      if (!SCH[sch_aan]) {
        maatregel_aan = FALSE;
      }
    }
    
    if (ta != NG) {
      RT[ta] = SPR || SR[fc];
      if (RT[ta] || T[ta]) {
        vertraag = TRUE;
      }
    }
    
    if (maatregel_aan) {
      
      /* tijdvertraagde aanvraag bij defecte drukknop en koplussen                                          */
      if (dk1 != NG) {
        if (DF[dk1] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
      }
      if (dk2 != NG) {
        if (DF[dk2] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Maatregelen bij detectie storing bus                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_det_bus(                    /* Kinzel 17-05-2019                                                */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count de1,                          /* index koplus 1                                                   */
    count de2)                          /* index koplus 2                                                   */
  {
    bool vertraag = FALSE;
    bool maatregel_aan = TRUE;
    
    if (sch_aan != NG)
    {
      if (!SCH[sch_aan]) {
        maatregel_aan = FALSE;
      }
    }
    
    if (ta != NG) {
      RT[ta] = SPR || SR[fc];
      if (RT[ta] || T[ta]) {
        vertraag = TRUE;
      }
    }
    
    if (maatregel_aan) {
      if ((de1 != NG) && (de2 == NG)) {
        if (DF[de1] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
      }
      if ((de2 != NG) && (de1 == NG)) {
        if (DF[de2] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
      }
      if ((de1 != NG) && (de2 != NG)) {
        if (DF[de1] && DF[de2] && R[fc] && !TRG[fc] && !vertraag) {
          set_A(fc);
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure LHOVRA functie R                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_LHOVRA_R(                   /* Fik191001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count sch)                          /* schakelaar                                                       */
  {
    count j,k;
    mulv ontruim,max,extra;
    
    if (SCH[sch]) {
      max = 0;
      if (somgk(fc)) extra = 2;
      else           extra = 0;
      
      for (j=0; j<GKFC_MAX[fc]; j++) {  /* primaire conflicten                                              */
        k = TI_pointer(fc,j);
        
        if (!G[fc] && (TI_max(k,fc) > TGL_max[k]) && (_HOT[k] > 0)) { /* ontruimingstijd > 0                */
          if ((_HOT[k] < TI_max(k,fc)-TGL_max[k]) && (_HOT[k] < 255)) X[fc] |= BIT6;
          ontruim = TI_max(k,fc) - TGL_max[k] - _HOT[k];
          if (ontruim > max) max = ontruim;
        }
      }
      /* werk TTR[] administratie bij                                                                       */
      if (RA[fc] && (extra==2) || RV[fc] && (extra==0)) extra = 1;
      if (  max              > TTW[fc]) TTW[fc] =  max;
      if (( max/10)          > TTC[fc]) TTC[fc] = (max/10);
      if (((max/10) + extra) > TTR[fc]) TTR[fc] = (max/10) + extra;
      proc_corr_ttr_vtg();
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal wachtstand-groen                                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_wgr_sgd(                    /* Fik180901                                                        */
    count fc,                           /* fasecyclus                                                       */
    count wgr)                          /* prm wgr                                                          */
  {
    count j,k;
    bool  wachtGR = TRUE;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    if (wgr == NG) wachtGR = FALSE;
    else {
      if (PRM[wgr] > 2) {
        PRM[wgr] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (wgr != NG) {
      if ((PRM[wgr] == 0) || (PRM[wgr] == 1) && R[fc] || (_FIL[fc]&BIT0)) wachtGR = FALSE;
    }
    
    if (GL[fc] || TRG[fc] || K[fc] || (SGD[fc]&BIT4) || (HKI[fc]&BIT2)
        || (R[fc] && (BAR[fc] || RR[fc]))) wachtGR = FALSE;
    
    if (R[fc] && ((HKII[fc]&BIT2) || (HKII[fc]&BIT4))) wachtGR = FALSE;
    
    if (wachtGR) {
      for (j=0; j<GKFC_MAX[fc]; j++) {
        k = TI_pointer(fc,j);
        if (A[k] && !BL[k] || B[k] || RA[k] || G[k] || !G[fc] && (GL[k] || TRG[k])) wachtGR = FALSE;
      }
      for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) {
        k = TI_pointer(fc,j);
        if (AFC[fc] && AFC[k]) {
          if (R[k] && !TRG[k] && (A[k] && !BL[k] || B[k] || RA[k])) wachtGR = FALSE;
        }
      }
      for (k=0; k<FC_MAX; k++) {
        if (TMPc[fc][k]) {
          if (A[k] && !BL[k] || B[k] || RA[k] || G[k] || !G[fc] && (GL[k] || TRG[k])) wachtGR = FALSE;
        }
      }
      for (k=0; k<FC_MAX; k++) {
        if (TMPf[fc][k] && AFC[fc] && AFC[k]) {
          if (R[k] && !TRG[k] && (A[k] && !BL[k] || B[k] || RA[k])) wachtGR = FALSE;
        }
      }
    }
    
    if (wachtGR) {
      if (G[fc]) {                      /* wachtstand-richting is groen                                     */
        
        A[fc] |= BIT5;
        YW[fc] |= BIT5;
        RW[fc] |= BIT5;
        FM[fc] = FALSE;
        
      } else {                          /* wachtstand-richting is rood                                      */
        
        A[fc] |= BIT5;
      }
    } else { if (!B[fc] && !RA[fc]) A[fc] &= (~BIT5); }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie meeverlenggroen                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  bool hf_mvg(void)                     /* Fik030405 - Fik070725                                            */
  {
    count i;
    
    for (i=0; i<FC_MAX; i++) {
      if (R[i] && A[i] && !BL[i] || _GRN[i] || G[i] && (!MG[i] || RW[i] || MK[i] ||
            (YM[i]&BIT4) || (HLPD[i][0])) && (!WG[i] || !(YW[i]&BIT5)))
      return TRUE;
    }
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie bepaal variabel meeverlenggroen                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  bool var_mg_sgd(                      /* Fik110724                                                        */
    count fc)
  {
    bool mvg_extra = TRUE;
    count j,k;
    
    if ((HKI[fc]&BIT2) && (HKI[fc]&BIT3) || (HKII[fc]&BIT0) || (TTK[fc] == 0)) mvg_extra = FALSE;
    
    if (MG[fc] && mvg_extra) {
      for (j=0; j<GKFC_MAX[fc]; j++) {
        k = TI_pointer(fc,j);
        if ((RA[k] || (HKII[k]&BIT7)) && (TTC[k]<=TI_max(fc,k)/10)) mvg_extra = FALSE;
        if (RA[k] && (KRVG[k] > 10)) mvg_extra = FALSE;
      }
    }
    
    if (MG[fc] && mvg_extra) {
      for (k=0; k<FC_MAX; k++) {
        if (TMPc[fc][k]) {
          if ((RA[k] || (HKII[k]&BIT7)) && (TTC[k] <= TMPi[fc][k]/10)) mvg_extra = FALSE;
          if (RA[k] && (KRVG[k] > 10)) mvg_extra = FALSE;
        }
      }
    }
    if (mvg_extra) return TRUE;
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure meeverlenggroen totdat conflict kan realiseren                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_mvg_sgd(                    /* Fik180309                                                        */
    count fc,                           /* fasecyclus                                                       */
    count mvg)                          /* prm mvg                                                          */
  {
    count i;
    bool  tmvg;
    bool  mvg_mag = TRUE;
    
    if (mvg == NG) mvg_mag = FALSE;
    else {
      if (PRM[mvg] > 2) {
        PRM[mvg] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (mvg != NG) {
      if ((PRM[mvg] == 0) || (PRM[mvg] == 1) && !MK[fc] && (AVR_GEBIED[fc] == 0)) mvg_mag = FALSE;
    }
    if (_FIL[fc]&BIT0) mvg_mag = FALSE;
    
    if (mvg != NG) {
      if (mvg_mag && (PRM[mvg] == 2)) {
        for (i=0; i<FC_MAX; i++) {
          if ((TI_max(fc,i) == NG) && !TMPc[fc][i] && !TMPc[i][fc]) {
            MV_matrix[fc][i] = TRUE;
          }
        }
      }
    }
    
    if (!(HKII[fc]&BIT3)) {             /* de volgrichting is NIET groen                                    */
      
      if (mvg_mag && (!somfbk(fc) && !BAR[fc] || var_mg_sgd(fc))) YM[fc] |= BIT0;
      
    } else {                            /* de volgrichting is WEL  groen                                    */
      tmvg = FALSE;
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || G[i] && (!WG[i] || SWG[i])
            && (!MG[i] || MK[i] || (AVR_GEBIED[i] > 0) || (YM[i]&BIT4) || HLPD[i][0])) tmvg = TRUE;
      }
      if (mvg_mag && tmvg && (!somfbk(fc) && !BAR[fc] || var_mg_sgd(fc))) YM[fc] |= BIT0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure meeverlengen overige voorwaarden                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_mee_mvg_sgd(                /* Fik191001                                                        */
    count fc1,                          /* fasecyclus van                                                   */
    count fc2,                          /* fasecyclus met                                                   */
    count sch)                          /* schakelaar                                                       */
  {
    count i;
    bool  t1mvg;
    bool  t2mvg;
    
    t1mvg = G[fc2] && (!MG[fc2] || YM[fc2] || RS[fc2] || RW[fc2]) && SCH[sch] && !(_FIL[fc1]&BIT0);
    
    if (SCH[sch] && !(_FIL[fc1]&BIT0)) {
      MV_matrix[fc1][fc2] = TRUE;
    }
    
    if (!(HKII[fc1]&BIT3)) {            /* De volgrichting is NIET groen                                    */
      
      if (t1mvg && (!somfbk(fc1) && !BAR[fc1] || var_mg_sgd(fc1))) YM[fc1] |= BIT0;
      
    } else {                            /* De volgrichting is WEL  groen                                    */
      t2mvg = FALSE;
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || G[i] && (!WG[i] || SWG[i])
            && (!MG[i] || MK[i] || (AVR_GEBIED[i] > 0) || (YM[i]&BIT4) || HLPD[i][0])) t2mvg = TRUE;
      }
      if (t1mvg && t2mvg && (!somfbk(fc1) && !BAR[fc1] || var_mg_sgd(fc1)))
        YM[fc1] |= BIT0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal maximum tijdmeting                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_maxgr_sgd(void)             /* Fik140101                                                        */
  {
    count i,j,k;
    count verw;
    
    for (i=0; i<FC_MAX; i++) {
      
      if (TS || SSGD) {                 /* Bepaal (fictieve)conflict-aanvraag                               */
        SGD[i] &= (~BIT4);
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (R[k] && A[k] && !BL[k] || (HKI[k]&BIT0)) SGD[i] |= BIT4;
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if (R[k] && A[k] && !BL[k] || (HKI[k]&BIT0)) SGD[i] |= BIT4;
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            if (R[k] && A[k] && !BL[k] || (HKI[k]&BIT0)) SGD[i] |= BIT4;
          }
        }
      }
      /* bepaal aanhouden verlenggroen                                                                      */
      
      if (G[i] && !SG[i] && !FG[i]) {
        YV[i] &= (~BIT3);
        verw = 0;
        if (YV[i]&BIT1) verw = 1;
        if (YV[i]&BIT2) verw = 2;
        if (WG[i] && !EFG[i] && !(YV[i]&BIT0) && !(YV[i]&BIT1)) verw = 2;
        
        if (verw == 0) {
          if  (MK[i]&BIT0)    YV[i] |= BIT0;
          else verw++;
        }
        
        if (verw == 1) {
          if  (MK[i]&BIT1)  { YV[i] = FALSE; YV[i] |= BIT1; }
          else verw++;
        }
        
        if (verw == 2) {
          if ((MK[i]&BIT2) || (MK[i]&BIT3) || WG[i]) {
            YV[i] = FALSE;
            YV[i] |= BIT2;
            if (!(MK[i]&BIT2) && !(MK[i]&BIT3)) YV[i] |= BIT3;
          } else {
            verw++;
            YV[i] |= BIT3;
          }
        }
        
        if ((verw > 2) || (KGCA[i]>=PRIM[i]))       FM[i] = TRUE;
        if ((PRGR[i]>=PRIM[i]) && (KG[i]>=ALTM[i])) FM[i] = TRUE;
        if ((_FIL[i]&BIT0) && (KG[i]>=_DOS[i]))     FM[i] = TRUE;
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* resetten hki-bits                                                */
      HKI[i]  &= (~BIT0);
      HKI[i]  &= (~BIT2);
      HKI[i]  &= (~BIT3);
      
      HKII[i] &= (~BIT2);
      HKII[i] &= (~BIT4);
      HKII[i] &= (~BIT7);
      
      SGD[i]  &= (~BIT6);               /* Reset prioriteitsblokkade                                        */
      SGD[i]  &= (~BIT7);
    }
    proc_alt_vtg_sgd();
    dcf_vs_corr_alt();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal realisatie                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_rea_sgd(void)               /* Fik180101                                                        */
  {
    count snel,volgend,i;
    count een_vooruit = 0;
    bool vooruit = FALSE;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    proc_Xvtg_sgd();
    for (i=0; i<FC_MAX; i++) {
      
      snel = volgend = 0;
      if (PBLK[i] != 0) {
        volgend = PBLK[i] + 1;
        if (volgend > MAX_BLK) volgend = 1;
        
        if (SBL && (BLOK == volgend)) {
          SGD[i] &= (~BIT2);
        }
        een_vooruit = PBLK[i] - 1;
        if (een_vooruit == 0) een_vooruit = MAX_BLK;
        
        vooruit = PPV2_BLOK(i);
        if (vooruit) {
          snel = een_vooruit - 1;
        } else {
          snel = een_vooruit;
        }
        if (snel == 0) snel = MAX_BLK;
        
        if (BLOK == WBLK) {
          if (A[i] && !(A[i]&BIT5) && !BL[i] && (BLOK != PBLK[i])) WBLK = FALSE;
        }
      }
      
      if (BLOK == PBLK[i]) {
        SGD[i] |=   BIT0;
        SGD[i] &= (~BIT3);
        
        if ((A[i]&BIT2) && !(SGD[i]&BIT2)
            && !(SGD[i]&BIT6) && !(SGD[i]&BIT7) && !MG[i]) {
          A[i] |=   BIT0;
          A[i] &= (~BIT2);
          A[i] &= (~BIT4);
        }
        /* ALTIJD een machineslag MG## in CCOL                                                              */
        
        if (G[i] && (PRGR[i] < HG[i])) PRGR[i] = HG[i];
        
        if (R[i] && !TRG[i] && (!A[i] || BL[i])) SGD[i] |= BIT2;
        
        /* if (G[i] && (HG[i]   >= PRIM[i])) SGD[i] |= BIT2;                                                */
        if (G[i] && (PRGR[i] >= PRIM[i])) SGD[i] |= BIT2;
        
        /* if ( G[i] && (PRGR[i] >= TKMX[i]) && (SGD[i]&BIT7)) SGD[i] |= BIT2;                              */
        if (MG[i] && (PRGR[i] >= TKMX[i]) && ( SGD[i]&BIT6)) SGD[i] |= BIT2;
        if (MG[i] && (PRGR[i] >= TKMX[i]) && (_FIL[i]&BIT0)) SGD[i] |= BIT2;
        
        if ((GL[i] || R[i] && TRG[i]) && !(SGD[i]&BIT6)) {
          if ((PRGR[i] == 0) || (PRGR[i] >= TKMX[i])) SGD[i] |= BIT2;
        }
        if ( G[i] && (YV[i]&BIT3))          SGD[i] |= BIT2;
        if (MG[i] && !MK[i])                SGD[i] |= BIT2;
        if (WG[i] && !MK[i] && (A[i]&BIT5)) SGD[i] |= BIT2;
        
        if (WACHT && (BLOK==WBLK) && (R[i] && A[i] && !BL[i] || G[i] && !MG[i])) SGD[i] &= (~BIT2);
        
        if (MG[i] && MK[i] && (A[i]&BIT2) && !(SGD[i]&BIT2)
            && !(SGD[i]&BIT6)
            && !(SGD[i]&BIT7)) {
          A[i] &= (~BIT2);
          RW[i] |=   BIT1;
          FM[i]  =  FALSE;
        }
        
        if (R[i] && !TRG[i] && !(SGD[i]&BIT2) && A[i] && !somfbk(i)
            && !(SGD[i]&BIT6) && !(X[i]&BIT3) && !BL[i]
            && (!somgk(i) || !FIXATIE)) B[i] |= BIT1;
      } else {
        SGD[i] &= (~BIT0);
      }
      
      if ((MAX_BLK >= 2) && ((BLOK == snel) || (BLOK == een_vooruit))) {
        SGD[i] |= BIT1;
        if (SBL && (BLOK == snel)) PRGR[i] = 0;
        
        if (R[i] && !TRG[i] && (!A[i] || BL[i]) && (PRGR[i] > 0)) SGD[i] |= BIT2;
        if (G[i] && (SGD[i]&BIT3) && (PRGR[i] < HG[i])) PRGR[i] = HG[i];
        
        if ((SGD[i]&BIT3) && (MG[i] || EG[i])) {
          if (HG[i]   >= PRIM[i]) SGD[i] |= BIT2;
          if (PRGR[i] >= PRIM[i]) SGD[i] |= BIT2;
          
          if ((PRGR[i] >= TKMX[i]) && ( SGD[i]&BIT7)) SGD[i] |= BIT2;
          if ((PRGR[i] >= TKMX[i]) && ( SGD[i]&BIT6)) SGD[i] |= BIT2;
          if ((PRGR[i] >= TKMX[i]) && (_FIL[i]&BIT0)) SGD[i] |= BIT2;
          
          SGD[i] &= (~BIT3);
        }
      } else {
        SGD[i] &= (~BIT1);
      }
      if (!(SGD[i]&BIT0) && !(SGD[i]&BIT1)) {
        SGD[i] &= (~BIT3);
        PRTEG[i] = 0;
      } else {
        if (!G[i] && !(SGD[i]&BIT2) && (PRGR[i] > 0)) {
          if (PRIM[i] > PRGR[i]) PRTEG[i] = PRIM[i] - PRGR[i];
          else           PRTEG[i] = 0;
          if (PRTEG[i] < ALTM[i]) PRTEG[i] = ALTM[i];
        } else {
          PRTEG[i] = 0;
        }
      }
    }
    proc_rea_vtg_sgd();
    dcf_vs_rea();
    dcf_gs_rea();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal vooruitrealiseren                                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_ppv_sgd(void)               /* Fik180101                                                        */
  {
    count i,j,k;
    bool  ppv;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    for (i=0; i<FC_MAX; i++) {
      
      if (!somfbk(i) && (SGD[i]&BIT1) &&  !(SGD[i]&BIT2) && !BL[i]
          &&  !(X[i]&BIT3) && (!(SGD[i]&BIT6) || G[i] && (SGD[i]&BIT3))
          && (G[i] || R[i] && !TRG[i] && A[i])
          && (!somgk(i) || !FIXATIE)) {
        
        ppv = TRUE;
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if ((SGD[k]&BIT0) && !(SGD[k]&BIT2)) ppv = FALSE;
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT0) && !(SGD[k]&BIT2)
                && (!G[k] || (X[k]&BIT3))) ppv = FALSE;
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k]) {
            if ((SGD[k]&BIT0) && !(SGD[k]&BIT2)) ppv = FALSE;
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPf[i][k]) {
            if (AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT0) && !(SGD[k]&BIT2)
                  && (!G[k] || (X[k]&BIT3))) ppv = FALSE;
            }
          }
        }
        
        if (ppv) {
          if (!G[i]) B[i] |= BIT1;
          A[i] |=   BIT0;
          A[i] &= (~BIT2);
          A[i] &= (~BIT4);
          SGD[i] |= BIT3;
        } else {
          if (!G[i]) SGD[i] &= (~BIT3); /* toevoeging Fik131027                                             */
        }
      } else { SGD[i] &= (~BIT3); }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TEG                                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_TEG_sgd(void)               /* Fik021208                                                        */
  {                                     /* Fik100414                                                        */
    count i;
    mulv  TEG_OV,_HTEGmax,HMAX,KLK_GR;
    
    for (i=0; i<FC_MAX; i++) {
      
      KLK_GR = KGCA[i];
      if (PRGR[i] > KGCA[i]) KLK_GR = PRGR[i];
      
      if (SGD[i]&BIT5) { TEG_OV = TEG[i]; }
      else        { TEG_OV = 0;  }
      
      if ((FTS[i]&BIT5) || (RISFTS[i]&BIT5) || (_FIL[i]&BIT5)) {
        if (G[i]) A[i] &= (~BIT2);
        else {
          if (PRTEG[i] == 0) {
            if (PRIM[i] > TEG_OV) TEG_OV = PRIM[i];
          } else {
            if (PRTEG[i]> TEG_OV) TEG_OV = PRTEG[i];
          }
        }
      }
      /* Bij een hulpdienstingreep is bijschakelen altijd toegestaan                                        */
      if ((HLPD[i][0]&BIT1) && (HLPD[i][1] > 0) || (HLPD[i][0]&BIT2) || (HLPD[i][0]&BIT0)) {
        if (G[i]) TEG_OV = 120;
      }
      
      if (!G[i] || MG[i] || WG[i] && (TVG_max[i] == 0)) { TEG[i] = 0;  }
      else {
        if (A[i]&BIT2) {
          if (KG[i] >= ALTM[i])  { TEG[i] = 0; }
          else       { TEG[i] = ALTM[i] - KG[i]; }
        } else {
          if (KG[i] >= ALTM[i]) { _HTEGmax = 0; }
          else                  { _HTEGmax = ALTM[i] - KG[i]; }
          
          if (KLK_GR >= PRIM[i]) { HMAX = 0; }
          else            { HMAX = PRIM[i] - KLK_GR; }
          
          if (HMAX > _HTEGmax) _HTEGmax = HMAX;
          
          if (SGD[i]&BIT7) {
            if (  KG[i] >= KAPM[i]) { TEG[i] = 0; }
            else            { TEG[i] = KAPM[i] - KG[i]; }
            if (KVFC[i] > TEG[i])  TEG[i] = KVFC[i];
            if (TEG[i] > _HTEGmax) TEG[i] = _HTEGmax;
          } else {
            TEG[i] = _HTEGmax;
          }
        }
      }
      if (TEG_OV > TEG[i]) TEG[i] = TEG_OV;
    }
    proc_teg_vtg_sgd();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TTP binnen aktief BLOK                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_TTP_primair(void)           /* Fik991211                                                        */
  {                                     /* Fik191001                                                        */
    count i,j,k;
    mulv  ontruim,restant;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    for (i=0; i<FC_MAX; i++) {
      
      if (!(SGD[i]&BIT5) && !(HKI[i]&BIT5) && !(HKII[i]&BIT5)) {
        
        if (!(SGD[i]&BIT0) || (SGD[i]&BIT2)) { TTP[i] = 255; }
        else {
          
          if (G[i] && !(X[i]&BIT4)) { TTP[i] = FALSE; }
          else {
            
            TTP[i]=FALSE;
            if  (GL[i]) TTP[i]=(TGL_max[i]-TGL_timer[i]+TRG_max[i])/10;
            if (TRG[i]) TTP[i]=(TRG_max[i]-TRG_timer[i])/10;
            
            if (_FIL[i]&BIT0) {
              restant = 0;
              if ((_FBL[i] + ((TGL_max[i] + TRG_max[i])/10)) > KR[i])
                restant = _FBL[i] + ((TGL_max[i] + TRG_max[i])/10) - KR[i];
              if (restant > TTP[i]) TTP[i] = restant;
            }
            
            for (j=0; j<GKFC_MAX[i]; j++) {
              k = TI_pointer(i,j);
              ontruim = 0;
              if (G[k]) {               /* Conflict is groen                                                */
                if (TI_max(k,i)>=0) {
                  ontruim = TEG[k] + (TI_max(k,i)/10);
                } else {
                  ontruim = TEG[k];
                }
              } else {                  /* Conflict is NIET groen                                           */
                
                if ((SGD[k]&BIT5) || (HKI[k]&BIT5) || (HKII[k]&BIT5)) {
                  
                  if (TEG[k]) {         /* Conflict wordt nog groen                                         */
                    
                    if (TI_max(k,i)>=0)
                      ontruim = TEG[k] + (TI_max(k,i)/10);
                    else
                    ontruim = TEG[k];
                    
                  } else {              /* Conflict wordt NIET groen                                        */
                    
                    if (!G[k] && (TI_max(k,i)>=0) && TI(k,i))
                      ontruim =  (TI_max(k,i)-TI_timer(k))/10;
                  }
                  
                } else {                /* NIET groen en geen prioriteitsrealisatie                         */
                  
                  if (!G[k] && (TI_max(k,i)>=0) && TI(k,i)) {
                    ontruim =  (TI_max(k,i)-TI_timer(k))/10;
                  }
                }
              }
              if (ontruim>TTP[i]) { TTP[i] = ontruim; if (TTP[i] > 254) TTP[i] = 254; }
            }
            
            for (k=0; k<FC_MAX; k++) {
              if (TMPc[i][k] || !G[k] && (TMPi[k][i]>0)) {
                ontruim = 0;
                if (G[k]) {             /* Conflict is groen                                                */
                  
                  ontruim = TEG[k] + TMPi[k][i]/10;
                  
                } else {                /* Conflict is NIET groen                                           */
                  
                  if ((SGD[k]&BIT5) || (HKI[k]&BIT5) || (HKII[k]&BIT5)) {
                    
                    if (TEG[k]) {       /* Conflict wordt nog groen                                         */
                      
                      ontruim = TEG[k] + TMPi[k][i]/10;
                      
                    } else {            /* Conflict wordt NIET groen                                        */
                      
                      if (!G[k])
                        ontruim = TMPi[k][i]/10;
                    }
                    
                  } else {              /* NIET groen en geen prioriteitsrealisatie                         */
                    
                    if (!G[k]) {
                      ontruim = TMPi[k][i]/10;
                    }
                  }
                }
                if (ontruim>TTP[i]) { TTP[i] = ontruim; if (TTP[i] > 254) TTP[i] = 254; }
              }
            }
            
            if (PRGR[i] == 0) TEG[i] = TTP[i] + PRIM[i];
            else              TEG[i] = TTP[i] + PRTEG[i];
          }
        }
      }
    }
    proc_ttp_vtg_sgd();
    dcf_vs_corr_ttp();
    dcf_gs_corr_ttp();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TTP voor vooruitschakelen                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_TTP_versneld(void)          /* Fik991211                                                        */
  {                                     /* Fik110724                                                        */
    count i,j,k;
    mulv  ontruim,restant;
    
    for (i=0; i<FC_MAX; i++) {
      
      if (((SGD[i]&BIT1) && !(SGD[i]&BIT2) && (R[i] && !TRG[i] && A[i]
              ||  (HKI[i]&BIT0)) || G[i] && (SGD[i]&BIT3))
        && !(SGD[i]&BIT5)
        && !(HKI[i]&BIT5) && !(HKII[i]&BIT5)) {
        
        if (G[i] && (SGD[i]&BIT3) && !(X[i]&BIT4)) { TTP[i] = FALSE; }
        else {
          
          TTP[i]=FALSE;
          if  (GL[i]) TTP[i]=(TGL_max[i]-TGL_timer[i]+TRG_max[i])/10;
          if (TRG[i]) TTP[i]=(TRG_max[i]-TRG_timer[i])/10;
          
          if (_FIL[i]&BIT0) {
            restant = 0;
            if ((_FBL[i] + ((TGL_max[i] + TRG_max[i])/10)) > KR[i])
              restant = _FBL[i] + ((TGL_max[i] + TRG_max[i])/10) - KR[i];
            if (restant > TTP[i]) TTP[i] = restant;
          }
          
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            ontruim = 0;
            if (G[k]) {                 /* Conflict is groen                                                */
              if (TI_max(k,i)>=0) {
                ontruim = TEG[k] + (TI_max(k,i)/10);
              } else {
                ontruim = TEG[k];
              }
            } else {                    /* Conflict is NIET groen                                           */
              
              if (TEG[k]) {             /* Conflict wordt nog groen                                         */
                
                if (TI_max(k,i)>=0)
                  ontruim = TEG[k] + (TI_max(k,i)/10);
                else
                ontruim = TEG[k];
                
              } else {                  /* Conflict wordt NIET groen                                        */
                
                if (!G[k] && (TI_max(k,i)>=0) && TI(k,i))
                  ontruim = (TI_max(k,i)-TI_timer(k))/10;
              }
            }
            if (ontruim>TTP[i]) { TTP[i] = ontruim; if (TTP[i] > 254) TTP[i] = 254; }
          }
          
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || !G[k] && (TMPi[k][i]>0)) {
              ontruim = 0;
              if (G[k]) {               /* Conflict is groen                                                */
                
                ontruim = TEG[k] + TMPi[k][i]/10;
                
              } else {                  /* Conflict is NIET groen                                           */
                
                if (TEG[k]) {           /* Conflict wordt nog groen                                         */
                  
                  ontruim = TEG[k] + TMPi[k][i]/10;
                  
                } else {                /* Conflict wordt NIET groen                                        */
                  
                  if (!G[k])
                    ontruim = TMPi[k][i]/10;
                }
              }
              if (ontruim>TTP[i]) { TTP[i] = ontruim; if (TTP[i] > 254) TTP[i] = 254; }
            }
          }
          
          if (PRGR[i] == 0) TEG[i] = TTP[i] + PRIM[i];
          else              TEG[i] = TTP[i] + PRTEG[i];
        }
      }
    }
    proc_ttp_vtg_sgd();
    dcf_vs_corr_ttp();
    dcf_gs_corr_ttp();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal tijd tot realisatie tbv blokkade alternatieve realisatie                                */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bepaal_TTPX(void)           /* Fik110724                                                        */
  {
    count i,j,k;
    count i_blk,j_blk;
    mulv  max,ontruim;
    
    for (i=0; i<FC_MAX; i++) {          /* initialiseer TTPX[] en TEGX[]                                    */
      TTPX[i] = 255;
      TEGX[i] = 0;
    }
    
    if (AKTPRG == 0) {
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || G[i]) {
          TTPX[i] = TTP[i];
          if (G[i]) TTPX[i] = 0;
          if (TTPX[i] < 255) {
            if (SGD[i]&BIT5)  TEGX[i] = TEG[i];
            else {
              if (G[i]) TEGX[i] = TEG[i];
              else {
                if (PRTEG[i] == 0) TEGX[i] = TTP[i] + PRIM[i];
                else               TEGX[i] = TTP[i] + PRTEG[i];
              }
            }
            if (!G[i] && (TTR[i] > TTPX[i])) TTPX[i] = TTR[i];
          }
        }
        if (_GRN[i] && RA[i] && !RR[i] && (TTR[i] != TTPX[i])) {
          TTPX[i] = TTR[i];
          if (PRTEG[i] == 0) TEGX[i] = TTPX[i] + PRIM[i];
          else       TEGX[i] = TTPX[i] + PRTEG[i];
        }
      }
      for (i_blk=0; i_blk<(MAX_BLK+2); i_blk++) {
        if ((BLOK+i_blk)>MAX_BLK) j_blk = (BLOK+i_blk) - MAX_BLK;
        else                      j_blk = (BLOK+i_blk);
        for (i=0; i<FC_MAX; i++) {
          if (PBLK[i] == j_blk) {
            if ((i_blk <  MAX_BLK) && !(SGD[i]&BIT2) ||
                (i_blk >= MAX_BLK) &&  (SGD[i]&BIT2)) {
              if ((R[i] && A[i] && !BL[i]) && (TTPX[i] == 255)) {
                max = TTR[i];
                
                for (j=0; j<GKFC_MAX[i]; j++) {
                  k = TI_pointer(i,j);
                  if (TEGX[k]>0) {
                    ontruim = TI_max(k,i)/10;
                  } else {
                    ontruim = 0;
                  }
                  if ((TEGX[k] + ontruim) > max) max = TEGX[k] + ontruim;
                }
                
                for (k=0; k<FC_MAX; k++) {
                  if (TMPc[i][k]) {
                    if (TEGX[k]>0) {
                      ontruim = TMPi[k][i]/10;
                    } else {
                      ontruim = 0;
                    }
                    if ((TEGX[k] + ontruim) > max) max = TEGX[k] + ontruim;
                  }
                }
                
                TTPX[i] = max;
                if (TTPX[i] == 255) TTPX[i] = 254;
                if (PRTEG[i] == 0) TEGX[i] = TTPX[i] + PRIM[i];
                else               TEGX[i] = TTPX[i] + PRTEG[i];
              }
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal BAR                                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_BAR_sgd(void)               /* Fik150327                                                        */
  {
    count i,j,k;
    bool  kapmvg;
    mulv  ontruim;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    proc_BAR_wtv_sgd();                 /* Bij prioriteitsrealisatie voorrang voor WTV richtingen           */
    /* proc_bepaal_TTPX();                                                                                  */
    
    for (i=0; i<FC_MAX; i++) {
      if (FIXATIE && somgk(i) || BL[i]) BAR[i] = TRUE;
      if (BAR[i]) HKI[i] |= BIT7;
    }
    
    for (i=0; i<FC_MAX; i++) {
      
      TTK[i] = 255;                     /* Initialiseer TTK##                                               */
      if (!G[i]) {                      /* Richting is NIET groen                                           */
        
        kapmvg = TRUE;
        for (j=0; j<GKFC_MAX[i]; j++) { /* Primaire conflicten                                              */
          
          k = TI_pointer(i,j);
          if (B[k] || RA[k] || G[k] && !MG[k] ||  MG[k] &&  RW[k]) BAR[i] = TRUE;
          if (B[k] || RA[k] || G[k] && !MG[k] && !WG[k] || SWG[k]) kapmvg = FALSE;
          ontruim = 0;
          if (TI_max(i,k)>=0) ontruim = TI_max(i,k)/10;
          if ((TTR[i] + ALTM[i] + ontruim) > TTP[k]) {
            if (!FIXATIE) BAR[i] = TRUE;
            kapmvg = FALSE;
          }
          /* if ((TTR[i] + ALTM[i] + ontruim) > TTPX[k]) {                                                  */
          /* if (!FIXATIE && !G[k]) BAR[i] = TRUE;                                                          */
          /* kapmvg = FALSE;                                                                                */
          /* }                                                                                              */
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k]) {
            if (B[k] || RA[k] || G[k] && !MG[k] ||  MG[k] &&  RW[k]) BAR[i] = TRUE;
            if (B[k] || RA[k] || G[k] && !MG[k] && !WG[k] || SWG[k]) kapmvg = FALSE;
            ontruim = TMPi[i][k]/10;
            if ((TTR[i] + ALTM[i] + ontruim) > TTP[k]) {
              if (!FIXATIE) BAR[i] = TRUE;
              kapmvg = FALSE;
            }
          }
        }
        
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) { /* Fictieve conflicten                                    */
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k] && (FC_type[i] == VOETGANGER)) {
            if (B[k] || RA[k] || G[k] && !MG[k] ||  MG[k] &&  RW[k]) BAR[i] = TRUE;
            if (B[k] || RA[k] || G[k] && !MG[k] && !WG[k] || SWG[k]) kapmvg = FALSE;
          }
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPf[i][k]) {
            if (AFC[i] && AFC[k] && (FC_type[i] == VOETGANGER)) {
              if (B[k] || RA[k] || G[k] && !MG[k] ||  MG[k] &&  RW[k]) BAR[i] = TRUE;
              if (B[k] || RA[k] || G[k] && !MG[k] && !WG[k] || SWG[k]) kapmvg = FALSE;
            }
          }
        }
        
        if (BAR[i] && kapmvg && R[i] && !TRG[i] && A[i] && !(HKI[i]&BIT7)) X[i] |= BIT7;
        
      } else {                          /* Richting is groen                                                */
        
        for (j=0; j<GKFC_MAX[i]; j++) {
          
          k = TI_pointer(i,j);
          if (!BL[k]) {
            ontruim = 0;
            if (TI_max(i,k)>=0) ontruim = TI_max(i,k)/10;
            if ((TEG[i] == 0) && (ontruim >= TTP[k] ) && !FIXATIE) BAR[i]=TRUE;
            /* if ((TEG[i] == 0) && (ontruim >= TTPX[k]) && !FIXATIE) BAR[i]=TRUE;                          */
            
            if (TTP[k]<255) {           /* Bepaal Tijd Tot Kappen                                           */
              
              if (TTP[k]>ontruim) {
                
                if (TTK[i]>TTP[k]-ontruim) TTK[i] = TTP[k] - ontruim;
              } else { TTK[i] = FALSE; }
            }
            /* if (TTPX[k]<255) {                                                                           */
            /* if (TTPX[k]>ontruim) {                                                                       */
            /* if (TTK[i]>TTPX[k]-ontruim) TTK[i] = TTPX[k] - ontruim;                                      */
            /* } else { TTK[i] = FALSE; }                                                                   */
            /* }                                                                                            */
          }
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k]) {
            if (!BL[k]) {
              ontruim  = TMPi[i][k]/10;
              if ((TEG[i] == 0) && (ontruim >= TTP[k] ) && !FIXATIE) BAR[i]=TRUE;
              if (TTP[k]<255) {         /* Bepaal Tijd Tot Kappen                                           */
                if (TTP[k]>ontruim) {
                  if (TTK[i]>TTP[k]-ontruim) TTK[i] = TTP[k] - ontruim;
                } else { TTK[i] = FALSE; }
              }
            }
          }
        }
        
      }
      if (_GRN[i]) BAR[i] = FALSE;
    }
    if (!FIXATIE)
    {
      proc_bar_vtg_sgd();
      dcf_vs_bar();
      dcf_gs_bar();
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure voorrang bij alternatieve realisaties                                                          */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_BAR_wtv_sgd(void)           /* Fik191001                                                        */
  {
    count i,j,k;
    bool voorw = FALSE;
    
    for (i=0; i<FC_MAX; i++) {          /* is er een richting met prioriteit?                               */
      if ((SGD[i]&BIT5) || (HLPD[i][0] > 0)) voorw = TRUE;
    }
    /* zo ja, dan WTV richtingen voorrang bij alternatieve realisatie                                       */
    if (voorw) {
      for (i=0; i<FC_MAX; i++) {
        if ((WTVFC[i] > 0) && !(SGD[i]&BIT6) && !(HPRI[i]&BIT7) && R[i] && !TRG[i] && A[i] && !BAR[i]) {
          
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (!(SGD[k]&BIT5) && (HLPD[k][0] == 0) && (WTVFC[k] == 0)) BAR[k] = TRUE;
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if (!(SGD[k]&BIT5) && (HLPD[k][0] == 0) && (WTVFC[k] == 0)) BAR[k] = TRUE;
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if (!(SGD[k]&BIT5) && (HLPD[k][0] == 0) && (WTVFC[k] == 0)) BAR[k] = TRUE;
            }
          }
          
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal alternatieve realisatie                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_ALT_sgd(void)               /* Fik180101                                                        */
  {
    count i,j,k;
    bool  altr;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    if (!SOMEVG) {
      for (i=0; i<FC_MAX; i++) {
        
        if (!BAR[i] && RV[i] && !TRG[i] && A[i] && !(A[i]&BIT7) && !B[i]) {
          
          altr = TRUE;
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i]) && A[k]) altr = FALSE;
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i]) && A[k]) altr = FALSE;
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i]) && A[k]) altr = FALSE;
            }
          }
          
          if (altr && !somfbk(i) && !BL[i]) { A[i] |= BIT2;
            B[i] |= BIT2;
          }
        }
        if (RA[i] && BAR[i] &&  (A[i]&BIT2) && !(HKII[i]&BIT1) && !_GRN[i]
            && !(SGD[i]&BIT5)) RR[i] = TRUE;
        if ( G[i] && BAR[i] && ((A[i]&BIT2) ||  (SGD[i]&BIT7))
            && !( YM[i]&BIT4)) {
          if (KG[i] > ALTM[i]) { FM[i] = TRUE; }
        }
      }
    }                                   /* Blk./Afk. door prioriteitsrealisatie onafh. van SOMEVG           */
    
    for (i=0; i<FC_MAX; i++) {
      if (RA[i] && (SGD[i]&BIT6) && !BL[i]) { /* overgang primair -> alternatief                            */
        A[i] |= BIT2;
        SGD[i] &= (~BIT3);
      }
      if (R[i] && BAR[i] && (SGD[i]&BIT6) && !(HKII[i]&BIT1) && !_GRN[i]) RR[i] = TRUE;
      if (G[i] && BAR[i] && (SGD[i]&BIT7) && !(YM[i]&BIT4)) {
        if (KG[i] > KAPM[i]) FM[i] = TRUE;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal BLOK-overgang                                                                           */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_nblok_sgd(void)             /* Fik001104                                                        */
  {
    count i;
    
    SBL = TRUE;
    
    if (WBLK == BLOK) {
      SBL   = FALSE;
      WACHT = TRUE;
    } else {
      if (!WACHT) {
        for (i=0; i<FC_MAX; i++) {
          if ((SGD[i]&BIT0) && !(SGD[i]&BIT2)) SBL = FALSE;
        }
      } else { WACHT = SBL = FALSE; }
    }
    if (SBL) {
      BLOK++;
      if (BLOK > MAX_BLK) BLOK = 1;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal FIXATIE                                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fixatie(void)               /* Fik070815                                                        */
  {
    count i;
    if (FIXATIE) {
      for (i=0; i<FC_MAX; i++) {
        if (G[i]) {
          YM[i] |= BIT0;
          Z[i]  = FALSE;
        }
      }
    }
    HTFB |= FIXATIE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure buffer fasecyclus-informatie                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fc_info(                    /* Fik191012                                                        */
    count fc,                           /* fasecyclus                                                       */
    count _sgd,                         /* mm sgd##                                                         */
    count _ttp,                         /* mm ttp##                                                         */
    count _teg,                         /* mm teg##                                                         */
    count _hki)                         /* mm hki##                                                         */
  {
    if (AKTPRG == 0) {
      MM[_ttp] = TTP[fc];
      MM[_teg] = TEG[fc];
      
      MM[_sgd] = 0;
      if (SGD[fc]&BIT0) MM[_sgd]+=1;
      if (SGD[fc]&BIT1) MM[_sgd]+=2;
      if (SGD[fc]&BIT2) MM[_sgd]+=4;
      if (SGD[fc]&BIT3) MM[_sgd]+=8;
      if (SGD[fc]&BIT4) MM[_sgd]+=16;
      if (SGD[fc]&BIT5) MM[_sgd]+=32;
      if (SGD[fc]&BIT6) MM[_sgd]+=64;
      if (SGD[fc]&BIT7) MM[_sgd]+=128;
      
      if (_hki != NG) {
        MM[_hki] = 0;
        if (HKI[fc]&BIT0) MM[_hki]+=1;
        if (HKI[fc]&BIT1) MM[_hki]+=2;
        if (HKI[fc]&BIT2) MM[_hki]+=4;
        if (HKI[fc]&BIT3) MM[_hki]+=8;
        if (HKI[fc]&BIT4) MM[_hki]+=16;
        if (HKI[fc]&BIT5) MM[_hki]+=32;
        if (HKI[fc]&BIT6) MM[_hki]+=64;
        if (HKI[fc]&BIT7) MM[_hki]+=128;
      }
      
      if (R[fc]) AR[fc] = PR[fc] = FALSE;
      
      if ((B[fc] || RA[fc]) && !RR[fc] && !BL[fc] || G[fc] && !MG[fc]) {
        if ((BLOK == PBLK[fc]) && !(SGD[fc]&BIT2) || (SGD[fc]&BIT3)) PR[fc] |= TRUE;
        AR[fc] = !PR[fc];
      }
      
      if (((B[fc] || RA[fc]) && !RR[fc] && !BL[fc] || G[fc]) && (SGD[fc]&BIT5)) BR[fc] = TRUE;
      else                                                                      BR[fc] = FALSE;
      
    } else {
      
      if (R[fc]) AR[fc] = PR[fc] = FALSE;
      
      if ((B[fc] || RA[fc]) && !RR[fc] && !BL[fc] || G[fc] && !MG[fc]) {
        if ((HVAS[fc]&BIT0) && !(HVAS[fc]&BIT3) || (HVAS[fc]&BIT4)) PR[fc] |= TRUE;
        AR[fc] = !PR[fc];
      }
      
#ifdef VAS_OV
      
      if (((B[fc] || RA[fc]) && !RR[fc] && !BL[fc] || G[fc]) && (HVAS[fc]&PRI)) BR[fc] = TRUE;
      else                                                                      BR[fc] = FALSE;
      
#else                                   /* *** ifdef -> VAS_OV                                              */
      
      BR[fc] = FALSE;
      
#endif                                  /* *** ifdef -> VAS_OV                                              */
      
#ifdef CRSV2CCOL
      if (((B[fc] || RA[fc]) && !RR[fc] && !BL[fc] || G[fc]) && (TTPR[fc] < 255)) BR[fc] = TRUE;
      else                                                                        BR[fc] = FALSE;
#endif
      
      MM[_ttp] = TTP[fc];
      MM[_teg] = TEG[fc];
      MM[_sgd] = HVAS[fc];
      
      if (_hki != NG) {
        MM[_hki] = 0;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure schakel op klok                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  bool proc_schakelklok(                /* datum 23-02-2004                                                 */
    count uur1,                         /* inschakeltijd  uren                                              */
    count min1,                         /* inschakeltijd  minuten                                           */
    count uur2,                         /* uitschakeltijd uren                                              */
    count min2,                         /* uitschakeltijd minuten                                           */
    count dag)                          /* dagcode                                                          */
  {
    mulv hdag=CIF_KLOK[CIF_DAGSOORT];
    if(hdag==0) hdag+=7;                /* zondag                                                           */
    
    if(klok_periode(PRM[uur1],PRM[min1],PRM[uur2],PRM[min2])) {
      return(!(  PRM[dag]<  8 &&  PRM[dag]!=hdag
          || PRM[dag]== 8 &&  hdag > 5
          || PRM[dag]== 9 && (hdag < 6 || hdag > 7)
          || PRM[dag]> 10));
      
    }
    else return(FALSE);
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal klokgeschakelde maximumgroentijd                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void maxgroen(                        /* Fik180101                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tvg1,                         /* prm tvoc##  -> ochtendspits                                      */
    count tvg2,                         /* prm tvav##  -> avondspits                                        */
    count tvg3,                         /* prm tvda##  -> dalperiode                                        */
    count tvg4,                         /* prm tvka##  -> koopavond                                         */
    count tvg5,                         /* prm tvza##  -> zaterdag                                          */
    count tvg6,                         /* prm tvna##  -> nacht                                             */
    count tvg7,                         /* prm tvex##  -> extra                                             */
    count mmf1,                         /* mm  fso1    -> fso 1 aktief                                      */
    count tvgf1,                        /* prm tvfi##1 -> max.gr fso 1                                      */
    count mmf2,                         /* mm  fso2    -> fso 2 aktief                                      */
    count tvgf2,                        /* prm tvfi##2 -> max.gr fso 1                                      */
    count tvgf3,                        /* prm tvfi##3 -> max fso 1en2                                      */
    count dvm11,                        /* prm tvdvm11 -> DVM progr.11                                      */
    count dvm12,                        /* prm tvdvm12 -> DVM progr.12                                      */
    count dvm13,                        /* prm tvdvm13 -> DVM progr.13                                      */
    count dvm14,                        /* prm tvdvm14 -> DVM progr.14                                      */
    count dvm15,                        /* prm tvdvm15 -> DVM progr.15                                      */
    count dvm16)                        /* prm tvdvm16 -> DVM progr.16                                      */
  {
#ifdef ROBUG
    mulv tmg_oud = TVG_basis[fc];
#else
    mulv tmg_oud = TVG_max[fc];
#endif
    
    mulv _tmg = PRM[tvg3]/10;
    mulv dvm_max;
    mulv klok_max;
    mulv file_max;
    
#ifdef ROBUG
    if (SCH[schrgv]) tmg_oud = TVG_basis[fc];
    else             tmg_oud = TVG_max[fc];
#endif
    
    /* bepaal maximum groen volgens de klok                                                                 */
    if (prog_ochtend   && (tvg1 != NG)) _tmg = PRM[tvg1]/10;
    if (prog_avond     && (tvg2 != NG)) _tmg = PRM[tvg2]/10;
    if (prog_koopavond && (tvg4 != NG)) _tmg = PRM[tvg4]/10;
    if (prog_zaterdag  && (tvg5 != NG)) _tmg = PRM[tvg5]/10;
    if (prog_nacht     && (tvg6 != NG)) _tmg = PRM[tvg6]/10;
    if (prog_extra     && (tvg7 != NG)) _tmg = PRM[tvg7]/10;
    
    klok_max = _tmg;
    
    if ((_dvm == 1) && (tvg1 != NG)) _tmg = PRM[tvg1]/10;
    if ((_dvm == 2) && (tvg2 != NG)) _tmg = PRM[tvg2]/10;
    if ((_dvm == 3) && (tvg3 != NG)) _tmg = PRM[tvg3]/10;
    if ((_dvm == 4) && (tvg4 != NG)) _tmg = PRM[tvg4]/10;
    if ((_dvm == 5) && (tvg5 != NG)) _tmg = PRM[tvg5]/10;
    if ((_dvm == 6) && (tvg6 != NG)) _tmg = PRM[tvg6]/10;
    if ((_dvm == 7) && (tvg7 != NG)) _tmg = PRM[tvg7]/10;
    
    file_max = _tmg;
    
    /* alleen fso maxima indien geen LOKALE of HALFSTARRE dvm van kracht                                    */
    if (_dvm < 11) {
      if ((mmf1 != NG) && (tvgf1 != NG)) { /* max. agv fso 1                                                */
        if (mmf2 == NG) {
          if (MM[mmf1] && PRM[tvgf1] != 0) file_max = PRM[tvgf1]/10;
        } else {
          if (MM[mmf1] && !MM[mmf2] && (PRM[tvgf1] != 0)) file_max = PRM[tvgf1]/10;
        }
      }
      
      if ((mmf2 != NG) && (tvgf2 != NG)) { /* max. agv fso 2                                                */
        if (mmf1 == NG) {
          if (MM[mmf2] && PRM[tvgf2] != 0) file_max = PRM[tvgf2]/10;
        } else {
          if (MM[mmf2] && !MM[mmf1] && (PRM[tvgf2] != 0)) file_max = PRM[tvgf2]/10;
        }
      }
      
      /* max. agv fso 1 en fso 2                                                                            */
      if ((mmf1 != NG) && (tvgf1 != NG) && (mmf2 != NG) && (tvgf2 != NG) && (tvgf3 != NG)) {
        if (MM[mmf1] && MM[mmf2] && (PRM[tvgf3] != 0)) file_max = PRM[tvgf3]/10;
      }
    }
    
    dvm_max = klok_max;
    
    if ((_dvm == 11) && (dvm11 != NG) && (PRM[dvm11] != 0)) dvm_max = PRM[dvm11]/10;
    if ((_dvm == 12) && (dvm12 != NG) && (PRM[dvm12] != 0)) dvm_max = PRM[dvm12]/10;
    if ((_dvm == 13) && (dvm13 != NG) && (PRM[dvm13] != 0)) dvm_max = PRM[dvm13]/10;
    if ((_dvm == 14) && (dvm14 != NG) && (PRM[dvm14] != 0)) dvm_max = PRM[dvm14]/10;
    if ((_dvm == 15) && (dvm15 != NG) && (PRM[dvm15] != 0)) dvm_max = PRM[dvm15]/10;
    if ((_dvm == 16) && (dvm16 != NG) && (PRM[dvm16] != 0)) dvm_max = PRM[dvm16]/10;
    
    if (AKTPRG > 20) dvm_max = MAXG[fc];
    
    if ((dvm_max > klok_max) && (_dvm > 10)) {
      FCDVM[fc] = TRUE;
    } else {
      FCDVM[fc] = FALSE;
    }
    
#ifdef RWS_SPECIFIEK
    if (file_max > _tmg) FCDVM[fc] = TRUE;
#endif
    
    if (file_max != _tmg) _tmg = file_max;
    if (_dvm >= 11)       _tmg =  dvm_max;
    
    if (SPR || R[fc] || G[fc] && !MG[fc]) {
      PRIM[fc] = TFG_max[fc]/10;
      if ((TGG_max[fc]/10) > PRIM[fc]) PRIM[fc] = TGG_max[fc]/10;
      if (_tmg > PRIM[fc]) PRIM[fc] = _tmg;
#ifdef ROBUG
      if (PRIM[fc]>(TFG_max[fc]/10)) TVG_basis[fc] = (PRIM[fc]*10) - TFG_max[fc];
      else                           TVG_basis[fc] = 0;
      if (!SCH[schrgv]) TVG_max[fc] = TVG_basis[fc];
#else
      if (PRIM[fc]>(TFG_max[fc]/10)) TVG_max[fc] = (PRIM[fc]*10) - TFG_max[fc];
      else                           TVG_max[fc] = 0;
#endif
    }
    
#ifdef ROBUG
    if (!SCH[schrgv] && TS && (tmg_oud != TVG_max[fc])) CIF_PARM1WIJZAP = -2;
#else
    if (TS && (tmg_oud != TVG_max[fc])) CIF_PARM1WIJZAP = -2;
#endif
    
    _DOS[fc] = PRIM[fc];
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure mee-realisatie tijdens LOKAAL bedrijf                                                          */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_mee_rea_sgd(                /* Fik130224                                                        */
    count fc1,                          /* fasecyclus MEE                                                   */
    count fc2,                          /* fasecyclus MET                                                   */
    count sch)                          /* schakelaar                                                       */
  {
    if (R[fc1] && !TRG[fc1] && !K[fc1] && !BL[fc1] && !somfbk(fc1) && !somgk(fc1)
        && SCH[sch] && !BAR[fc1] && !(_FIL[fc1]&BIT0)
        && (SG[fc2] || FG[fc2] || SWG[fc2])) {
      if ((SG[fc2] || FG[fc2]) &&  R[fc1] && !A[fc1]) A[fc1] |= BIT2;
      if ((SG[fc2] || FG[fc2]) && RV[fc1] && !B[fc1]) B[fc1] |= BIT2;
      RR[fc1] = FALSE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure status snelheidsmeetpunt                                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void status_snelheid_mp(              /* Fik180901                                                        */
    count isv,                          /* ingangssignaal waarop snelheid is afgebeeld                      */
    count vmin,                         /* PRM ondergrens waarbij meetpunt aanspreekt                       */
    count vmax,                         /* PRM bovengrens waarbij meetpunt afvalt                           */
    count status)                       /* MM  status meetpunt                                              */
  {
    mulv snelheid = 0;
    
    if (CIF_IS[isv] && (CIF_IS[isv] != IS_old[isv]) && /* nieuwe  melding voertuig                          */
        !(CIF_IS[isv]&BIT12) && !(CIF_IS[isv]&BIT15)) /* normale rijrichting en betrouwbaar                 */
    {
      if (CIF_IS[isv]&BIT0) snelheid += 1;
      if (CIF_IS[isv]&BIT1) snelheid += 2;
      if (CIF_IS[isv]&BIT2) snelheid += 4;
      if (CIF_IS[isv]&BIT3) snelheid += 8;
      if (CIF_IS[isv]&BIT4) snelheid += 16;
      if (CIF_IS[isv]&BIT5) snelheid += 32;
      if (CIF_IS[isv]&BIT6) snelheid += 64;
      if (CIF_IS[isv]&BIT7) snelheid += 128;
    }
    
    /* bij een ondergrens van 0 komt het meetpunt per definitie nooit op                                    */
    if (snelheid < PRM[vmin]) MM[status] = TRUE;
    if (snelheid > PRM[vmax]) MM[status] = FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal ongehinderde rijtijd OV                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_rijtijd_ov(                 /* Fik170831                                                        */
    mulv fc,                            /* fasecyclus                                                       */
    mulv de1,                           /* koplus                                                           */
    mulv de2,                           /* lange lus                                                        */
    mulv _gewsg,                        /* gewsg  fc specifiek                                              */
    mulv _corkop,                       /* corkop fc specifiek                                              */
    mulv _cordet,                       /* cordet fc specifiek                                              */
    mulv _corsec)                       /* coraec fc specifiek                                              */
  {
    /* GTTP[] is het gewenste startgroen moment van de bus in tienden van seconden voor de stopstreep       */
    /* GTTP[] loopt alleen op, dit om flipperen in het afkap- en blokkeer mechaniek te voorkomen            */
    
    mulv gewsg  = TGG_max[fc];
    mulv corkop = 20;
    mulv cordet = 60;
    mulv corsec = 4;
    
#ifdef prmcorkop
    corkop = PRM[prmcorkop];
#endif
    
#ifdef prmcordet
    cordet = PRM[prmcordet];
#endif
    
#ifdef prmcorsec
    cordet = PRM[prmcorsec];
#endif
    
#ifdef CRSV2CCOL
    if (ovp[fc][1] != NG) gewsg = PRM[ovp[fc][1]];
#endif
    
    if (_gewsg  != NG) gewsg  = PRM[_gewsg] ;
    if (_corkop != NG) corkop = PRM[_corkop];
    if (_cordet != NG) cordet = PRM[_cordet];
    if (_corsec != NG) corsec = PRM[_corsec];
    
    if (FC_type[fc] == OV) corsec = 0;
    
    if (G[fc] || GL[fc] || TRG[fc]) GTTP[fc] = gewsg; /* gewenst start groen bij leeg wegvak                */
    
    if (de1 != NG) {
      if (R[fc] && !TRG[fc] && (D[de1] || DF[de1])) { /* gewenst start groen als koplus bezet               */
        if (GTTP[fc] < gewsg + corkop) GTTP[fc] = gewsg + corkop;
        
        if (de2 != NG) {                /* correctie per seconde als detectie bezet blijft                  */
          if (R[fc] && !TRG[fc] && (D[de1] || DF[de1]) && (D[de2] || DF[de2]) && TS) {
            if (GTTP[fc] < 1000) GTTP[fc] += corsec;
          }
        } else {
          if (R[fc] && !TRG[fc] && (D[de1] || DF[de1]) && TS) {
            if (GTTP[fc] < 1000) GTTP[fc] += corsec;
          }
        }
        
        if (de2 != NG) {
          if (D[de2] || DF[de2]) {      /* gewenst start groen als ook lange lus bezet raakt                */
            if (GTTP[fc] < gewsg + cordet)
              GTTP[fc] = gewsg + cordet;
          }
        }
      }
      
    } else {                            /* geen koplus                                                      */
      
      if (de2 != NG) {
        if (R[fc] && !TRG[fc] && (D[de2] || DF[de2])) { /* gewenst start groen als lange lus bezet          */
          if (GTTP[fc] < gewsg + cordet) GTTP[fc] = gewsg + cordet;
          /* correctie per seconde als detectie bezet blijft                                                */
          if (R[fc] && !TRG[fc] && (D[de2] || DF[de2]) && TS) {
            if (GTTP[fc] < 1000) GTTP[fc] += corsec;
          }
        }
      }
    }
#ifdef VAS_OV
    CRTR[fc] = GTTP[fc]/10;             /* in VAS heeft CRTR[] dezelfde functionaliteit maar dan in 
                                           seconden                                                         */
#endif
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV PRIORITEITSINGREEP                                                                        */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal prioriteit bij fic.conflicterende prioriteitsaanvragen                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_PRI_sgd(void)               /* Fik140719 op aangeven Peter Smit                                 */
  {                                     /* Fik190219                                                        */
    count i,j,k;
    mulv _confl_prio = 0;
    bool pel_pri[FCMAX];
    bool fts_pri[FCMAX];
    bool fiets_prio = FALSE;
    
#ifdef prmprio
    
    if (PRM[prmprio] > 4) {
      PRM[prmprio] = 0;
      CIF_PARM1WIJZAP = -2;
    }
    _confl_prio = PRM[prmprio];
    
#endif                                  /* *** ifdef -> prmprio                                             */
    
    for (i=0; i<FC_MAX; i++) {
      pel_pri[i] = fts_pri[i] = FALSE;
      if (PEL[i]&BIT5) pel_pri[i] = TRUE;
      if ((RISPEL[i]&BIT5) || (TOV[i]&BIT5) || (VSM[i]&BIT5) || (WSP[i]&BIT5)) PEL[i] |= BIT5;
      
      if (FTS[i]&BIT5) fts_pri[i] = TRUE;
      if (RISFTS[i]&BIT5) FTS[i] |= BIT5;
    }
    
    for (i=0; i<FC_MAX; i++) {
      if ((SGD[i]&BIT5) && !(FTS[i]&BIT5)) _pri_akt[i] = TRUE; /* bepaal aktieve prioriteit                 */
      else                                 _pri_akt[i] = FALSE;
      /* ... fiets komt altijd na andere prioriteiten                                                       */
      
      if ((POV[i]&BIT5) || (PEL[i]&BIT5) || (_FIL[i]&BIT5)) SGD[i] |=   BIT5; /* bepaal prioriteit gewenst  */
      else                           { _pri_akt[i] = FALSE; SGD[i] &= (~BIT5); }
      if (!(POV[i]&BIT5) && !(PEL[i]&BIT5)) TEG[i] = FALSE; /* idg TEG bepaling in proc_TEG_sgd()           */
    }
    
    for (i=0; i<FC_MAX; i++) {          /* V-smile gaat altijd voor                                         */
      if ((SGD[i]&BIT5) && (VSM[i]&BIT5)) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (SGD[k]&BIT5) SGD[k] &= (~BIT5);
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && !(VSM[k]&BIT5)) SGD[k] &= (~BIT5);
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] && AFC[i] && AFC[k]) {
            if (SGD[k]&BIT5) SGD[k] &= (~BIT5);
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPf[i][k] && AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && !(VSM[k]&BIT5)) SGD[k] &= (~BIT5);
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* nafile gaat voor tenzij BUS enof PEL al groen                    */
      if ((SGD[i]&BIT5) && (_FIL[i]&BIT5)) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (SGD[k]&BIT5) {
            if (!G[k] && !(_FIL[k]&BIT5)) SGD[k] &= (~BIT5); /* nafile gaat idg altijd voor                 */
            if (G[k]) {                 /* tijdens groen alleen als kappen conflicterende prio toegestaan   */
              if ((POV[k]&BIT5) && !(PEL[k]&BIT5) && (PEL[i]&BIT5) && (_confl_prio == 4) ||
                  (PEL[k]&BIT5) && !(POV[k]&BIT5) && (POV[i]&BIT5) && (_confl_prio == 2)) {
                SGD[k] &= (~BIT5);
              } else {
                SGD[i] &= (~BIT5);
              }
            }
          }
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if (SGD[k]&BIT5) {
              if (!G[k] && !(_FIL[k]&BIT5)) SGD[k] &= (~BIT5); /* nafile gaat idg altijd voor               */
              if (G[k]) {               /* tijdens groen alleen als kappen conflicterende prio toegestaan   */
                if ((POV[k]&BIT5) && !(PEL[k]&BIT5) && (PEL[i]&BIT5) && (_confl_prio == 4) ||
                    (PEL[k]&BIT5) && !(POV[k]&BIT5) && (POV[i]&BIT5) && (_confl_prio == 2)) {
                  SGD[k] &= (~BIT5);
                } else {
                  SGD[i] &= (~BIT5);
                }
              }
            }
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            if (SGD[k]&BIT5) {
              if (!G[k] && !(_FIL[k]&BIT5)) SGD[k] &= (~BIT5); /* nafile gaat idg altijd voor               */
              if (G[k]) {               /* tijdens groen alleen als kappen conflicterende prio toegestaan   */
                if ((POV[k]&BIT5) && !(PEL[k]&BIT5) && (PEL[i]&BIT5) && (_confl_prio == 4)
                    ||  (PEL[k]&BIT5) && !(POV[k]&BIT5) && (POV[i]&BIT5) && (_confl_prio == 2)) SGD[k] &= (~BIT5);
                else                                                                        SGD[i] &= (~BIT5);
              }
            }
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* onderling conflicterende nafile                                  */
      if ((SGD[i]&BIT5) && (_FIL[i]&BIT5)) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if ((SGD[k]&BIT5) && (_FIL[k]&BIT5)) {
            if (G[i] || !G[i] && !G[k] && (TFB_timer[i] >= TFB_timer[k])) SGD[k] &= (~BIT5);
            else                                                          SGD[i] &= (~BIT5);
          }
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && (_FIL[k]&BIT5)) {
              if (G[i] || !G[i] && !G[k] && (TFB_timer[i] >= TFB_timer[k])) SGD[k] &= (~BIT5);
              else                                                          SGD[i] &= (~BIT5);
            }
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && (_FIL[k]&BIT5)) {
              if (G[i] || !G[i] && !G[k] && (TFB_timer[i] >= TFB_timer[k])) SGD[k] &= (~BIT5);
              else                                                          SGD[i] &= (~BIT5);
            }
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* overgebleven nafile gaan nu altijd voor                          */
      if ((SGD[i]&BIT5) && (_FIL[i]&BIT5)) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          SGD[k] &= (~BIT5);
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            SGD[k] &= (~BIT5);
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            SGD[k] &= (~BIT5);
          }
        }
      }
    }
    
    if (_confl_prio == 0) {             /* aktieve prioriteit gaat voor                                     */
      for (i=0; i<FC_MAX; i++) {
        if ((SGD[i]&BIT5) && _pri_akt[i]) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            SGD[k] &= (~BIT5);
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              SGD[k] &= (~BIT5);
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              SGD[k] &= (~BIT5);
            }
          }
        }
      }
      
      for (i=0; i<FC_MAX; i++) {        /* daarna bus voor peloton                                          */
        if ((SGD[i]&BIT5) && (POV[i]&BIT5)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (SGD[k]&BIT5) {
              if (G[k] || !G[k] && !G[i] && (POV[k]&BIT5) && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
            }
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if (SGD[k]&BIT5) {
                if (G[k] || !G[k] && !G[i] && (POV[k]&BIT5) && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if (SGD[k]&BIT5) {
                if (G[k] || !G[k] && !G[i] && (POV[k]&BIT5) && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
        }
      }
      
      for (i=0; i<FC_MAX; i++) {        /* daarna peloton langst aanwezig                                   */
        if ((SGD[i]&BIT5) && (PEL[i]&BIT5)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (SGD[k]&BIT5) {
              if (G[k] || !G[k] && !G[i] && (PEL[k]&BIT5) && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
            }
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if (SGD[k]&BIT5) {
                if (G[k] || !G[k] && !G[i] && (PEL[k]&BIT5) && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if (SGD[k]&BIT5) {
                if (G[k] || !G[k] && !G[i] && (PEL[k]&BIT5) && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
        }
      }
    }
    
    if ((_confl_prio == 1) || (_confl_prio == 2)) { /* bus gaat voor peloton                                */
      for (i=0; i<FC_MAX; i++) {
        if ((SGD[i]&BIT5) && (POV[i]&BIT5)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if ((SGD[k]&BIT5) && (POV[k]&BIT5)) {
              if (G[k] || !G[k] && !G[i] && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
            }
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (POV[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (POV[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
        }
        
        if ((SGD[i]&BIT5) && !(POV[i]&BIT5) && (PEL[i]&BIT5)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if ((SGD[k]&BIT5) && (POV[k]&BIT5)) {
              if (G[k] || !G[k] && !G[i] || G[i] && (_confl_prio == 2)) SGD[i] &= (~BIT5);
            }
            if ((SGD[k]&BIT5) && !(POV[k]&BIT5) && (PEL[k]&BIT5)) {
              if (G[k] || !G[k] && !G[i] && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
            }
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (POV[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] || G[i] && (_confl_prio == 2)) SGD[i] &= (~BIT5);
              }
              if ((SGD[k]&BIT5) && !(POV[k]&BIT5) && (PEL[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (POV[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] || G[i] && (_confl_prio == 2)) SGD[i] &= (~BIT5);
              }
              if ((SGD[k]&BIT5) && !(POV[k]&BIT5) && (PEL[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
        }
      }
    }
    
    if ((_confl_prio == 3) || (_confl_prio == 4)) { /* peloton gaat voor bus                                */
      for (i=0; i<FC_MAX; i++) {
        if ((SGD[i]&BIT5) && (PEL[i]&BIT5)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if ((SGD[k]&BIT5) && (PEL[k]&BIT5)) {
              if (G[k] || !G[k] && !G[i] && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
            }
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (PEL[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (PEL[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (PEL1[k] >= PEL1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
        }
        if ((SGD[i]&BIT5) && !(PEL[i]&BIT5) && (POV[i]&BIT5)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if ((SGD[k]&BIT5) && (PEL[k]&BIT5)) {
              if (G[k] || !G[k] && !G[i] || G[i] && (_confl_prio == 4)) SGD[i] &= (~BIT5);
            }
            if ((SGD[k]&BIT5) && !(PEL[k]&BIT5) && (POV[k]&BIT5)) {
              if (G[k] || !G[k] && !G[i] && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
            }
          }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            if (AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (PEL[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] || G[i] && (_confl_prio == 4)) SGD[i] &= (~BIT5);
              }
              if ((SGD[k]&BIT5) && !(PEL[k]&BIT5) && (POV[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
              if ((SGD[k]&BIT5) && (PEL[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] || G[i] && (_confl_prio == 4)) SGD[i] &= (~BIT5);
              }
              if ((SGD[k]&BIT5) && !(PEL[k]&BIT5) && (POV[k]&BIT5)) {
                if (G[k] || !G[k] && !G[i] && (BUS1[k] >= BUS1[i])) SGD[i] &= (~BIT5);
              }
            }
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* fietsprioriteit gewenst                                          */
      if (FTS[i]&BIT5) SGD[i] |= BIT5;
    }
    
    for (i=0; i<FC_MAX; i++) {          /* fiets altijd na andere prioriteit                                */
      if (FTS[i]&BIT5) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if ((SGD[k]&BIT5) && !(FTS[k]&BIT5)) SGD[i] &= (~BIT5);
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && !(FTS[k]&BIT5)) SGD[i] &= (~BIT5);
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && !(FTS[k]&BIT5)) SGD[i] &= (~BIT5);
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* onderling conflicterende fietsprioriteit op basis van wachttijd  */
      if ((SGD[i]&BIT5) && (FTS[i]&BIT5)) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if ((SGD[k]&BIT5) && (FTS[k]&BIT5) && (TFB_timer[k] >= TFB_timer[i])) SGD[i] &= (~BIT5);
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && (FTS[k]&BIT5) && (TFB_timer[k] >= TFB_timer[i])) SGD[i] &= (~BIT5);
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            if ((SGD[k]&BIT5) && (FTS[k]&BIT5) && (TFB_timer[k] >= TFB_timer[i])) SGD[i] &= (~BIT5);
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* onderling conflicterend SGD bit5 nooit toegestaan                */
      if (SGD[i]&BIT5) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          SGD[k] &= (~BIT5);
        }
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            SGD[k] &= (~BIT5);
          }
        }
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            SGD[k] &= (~BIT5);
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {          /* reset variabelen indien geen prioriteit                          */
      if (!(SGD[i]&BIT5)) {
        KWCOV[i] &= (~BIT2);
        TEG[i] = FALSE;
        YM[i] &= (~BIT4);
      }
      PEL[i] &= (~BIT5);
      if (pel_pri[i]) PEL[i] |= BIT5;
      
      FTS[i] &= (~BIT5);
      if (fts_pri[i]) FTS[i] |= BIT5;
    }
    
    /* SGD bit5 doorgeven aan gekoppelde richtingen                                                         */
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fiets_prio = FALSE;
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] && (SGD[kp_vtg_fts[i][j]]&BIT5)) fiets_prio = TRUE;
          }
        }
        if (fiets_prio) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              SGD[kp_vtg_fts[i][j]] |= BIT5;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal prioriteitsblokkade                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_Xpr_sgd(void)               /* Fik170916                                                        */
  {
    count i,j,k;
    
    bool HALTEER = FALSE;
#ifdef schhaltwtv
    HALTEER = SCH[schhaltwtv];
#endif
    
    for (i=0; i<FC_MAX; i++) {
      if ((SGD[i]&BIT5) || (HKI[i]&BIT5) && (HKI[i]&BIT1) || (HKII[i]&BIT5)) {
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          SGD[k] |= BIT6;
          if (G[i] && (POV[i]&BIT5) && HALTEER) halteer_wtv[k] = TRUE;
          if (KAPC[i] > 0) SGD[k] |= BIT7;
          if ((KAPC[i]&BIT0) && (KVOV[i] < KVFC[k])) KVFC[k] = KVOV[i];
          /* KVFC[] = tijd totdat afkapvertraging van conflicterende prioriteitsingreep is verstreken       */
          if ((KAPC[i]&BIT1) && (KVPK[i] < KVFC[k])) KVFC[k] = KVPK[i];
          if ((KAPC[i]&BIT2)                       ) KVFC[k] = 0;
          
          if ((KAPC[i]&BIT3) && (KVRPK[i] < KVFC[k])) KVFC[k] = KVRPK[i];
        }
        
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
          k = TI_pointer(i,j);
          if (AFC[i] && AFC[k]) {
            SGD[k] |= BIT6;
            if (G[i] && (POV[i]&BIT5) && HALTEER) halteer_wtv[k] = TRUE;
            if (KAPC[i] > 0) SGD[k] |= BIT7;
            if ((KAPC[i]&BIT0) && (KVOV[i] < KVFC[k])) KVFC[k] = KVOV[i];
            /* KVFC[] = tijd totdat afkapvertraging van conflicterende prioriteitsingreep is verstreken     */
            if ((KAPC[i]&BIT1) && (KVPK[i] < KVFC[k])) KVFC[k] = KVPK[i];
            if ((KAPC[i]&BIT2)                       ) KVFC[k] = 0;
            
            if ((KAPC[i]&BIT3) && (KVRPK[i] < KVFC[k])) KVFC[k] = KVRPK[i];
          }
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k] && AFC[i] && AFC[k]) {
            SGD[k] |= BIT6;
            if (G[i] && (POV[i]&BIT5) && HALTEER) halteer_wtv[k] = TRUE;
            if (KAPC[i] > 0) SGD[k] |= BIT7;
            if ((KAPC[i]&BIT0) && (KVOV[i] < KVFC[k])) KVFC[k] = KVOV[i];
            /* KVFC[] = tijd totdat afkapvertraging van conflicterende prioriteitsingreep is verstreken     */
            if ((KAPC[i]&BIT1) && (KVPK[i] < KVFC[k])) KVFC[k] = KVPK[i];
            if ((KAPC[i]&BIT2)                       ) KVFC[k] = 0;
            
            if ((KAPC[i]&BIT3) && (KVRPK[i] < KVFC[k])) KVFC[k] = KVRPK[i];
          }
        }
      }
    }
    proc_Xpr_sgd_file();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TTP voor prioriteitsrealisaties                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_TTP_prioriteit(void)        /* Fik980421                                                        */
  {                                     /* Fik110724                                                        */
    count i,j,k;
    mulv  ontruim,restant;
    
    for (i=0; i<FC_MAX; i++) {
      
      if (!(SGD[i]&BIT5) && !(HKI[i]&BIT5) && !(HKII[i]&BIT5)) { TTP[i] = 255; }
      else {
        
        if (G[i]) { TTP[i] = FALSE; }
        else {
          
          TTP[i]=FALSE;
          if  (GL[i]) TTP[i]=(TGL_max[i]-TGL_timer[i]+TRG_max[i])/10;
          if (TRG[i]) TTP[i]=(TRG_max[i]-TRG_timer[i])/10;
          
          if (_FIL[i]&BIT0) {
            restant = 0;
            if ((_FBL[i] + ((TGL_max[i] + TRG_max[i])/10)) > KR[i])
              restant = _FBL[i] + ((TGL_max[i] + TRG_max[i])/10) - KR[i];
            if (restant > TTP[i]) TTP[i] = restant;
          }
          
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TI_pointer(i,j);
            ontruim = 0;
            if (G[k]) {                 /* Conflict is groen                                                */
              if (TI_max(k,i)>=0) {
                ontruim = TEG[k] + (TI_max(k,i)/10);
              } else {
                ontruim = TEG[k];
              }
            } else {                    /* Conflict is NIET groen                                           */
              if (!G[k] && (TI_max(k,i)>=0) && TI(k,i)) {
                ontruim =  (TI_max(k,i)-TI_timer(k))/10;
              }
            }
            if (ontruim>TTP[i]) { TTP[i] = ontruim; if (TTP[i] > 254) TTP[i] = 254; }
          }
          
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || !G[k] && (TMPi[k][i]>0)) {
              ontruim = 0;
              if (G[k]) {               /* Conflict is groen                                                */
                
                ontruim = TEG[k] + TMPi[k][i]/10;
                
              } else {                  /* Conflict is NIET groen                                           */
                if (!G[k]) {
                  ontruim = TMPi[k][i]/10;
                }
              }
              if (ontruim>TTP[i]) { TTP[i] = ontruim; if (TTP[i] > 254) TTP[i] = 254; }
            }
          }
          
          if (SGD[i]&BIT5) TEG[i] += TTP[i];
          else {
            if (A[i]&BIT2) {
              if ((TTP[i] + ALTM[i]) > TEG[i]) TEG[i] = TTP[i] + ALTM[i];
            }
            else {
              if (PRGR[i] == 0) {
                if ((TTP[i] + PRIM[i]) > TEG[i]) TEG[i] = TTP[i] + PRIM[i];
              } else {
                if ((TTP[i] + PRTEG[i])> TEG[i]) TEG[i] = TTP[i] + PRTEG[i];
              }
            }
          }
        }
        
        if (!somfbk(i) && !(HKI[i]&BIT6) && R[i] && !TRG[i]
            &&  ((YM[i]&BIT4) || (HKI[i]&BIT1) && (HKI[i]&BIT5) || (_FIL[i]&BIT5) || (FTS[i]&BIT5) || (RISFTS[i]&BIT5)
              || (HKII[i]&BIT5))) {
          A[i] |= BIT4;
          B[i] |= BIT4;
        }
      }
    }
    proc_ttp_vtg_sgd();
    dcf_vs_corr_ttp();
    dcf_gs_corr_ttp();
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal aanwezigheid PELOTON                                                                    */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_buf_pel_pri(                /* Fik120906                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kops,                         /* koppelsignaal                                                    */
    count inmv,                         /* T inmeldvertraging                                               */
    count afkv,                         /* T afkapvertraging                                                */
    count afst,                         /* T afstand rijtijd                                                */
    count nmax,                         /* T maximumgroen na ingreep                                        */
    count tbl,                          /* T blokkeer herhaalde prio.                                       */
    count prio,                         /* PRM prioriteitscode                                              */
    count awt1,                         /* aanwezigheidsduur 1e pel                                         */
    count awt2,                         /* aanwezigheidsduur 2e pel                                         */
    count awt3)                         /* aanwezigheidsduur 3e pel                                         */
  {
    mulv ondermax = 255;
    mulv HPEL;
    mulv UITMELD;
    mulv extra,_TEG;
    bool bep_TEG;
    bool pel_uitmelding;
    mulv pri_code;
    mulv instel_onm;
    bool _reset_pel;
    
    _TEG = 0;                           /* voorkom warning                                                  */
    
    if (prio != NG) {                   /* controleer instellingen                                          */
      if (PRM[prio] > 3) {
        PRM[prio] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (PRM[prio] == 0) BPI[fc] |= BIT1;
    instel_onm = PRIM[fc];
    
    if (CIF_IS[kops] && !IS_old[kops] && !(BPI[fc]&BIT1) && (!BLK_PRI_REA || !G[fc])) {
                                        /* Behandeling inmelding                                            */
      if (!MM[awt1]) MM[awt1] = 1;
      else if (!MM[awt2]) MM[awt2] = 1;
      else if (!MM[awt3]) MM[awt3] = 1;
    }
    
    if (TS) {                           /* Bewerk buffer                                                    */
      if (MM[awt1] && (MM[awt1]<1000)) MM[awt1]++;
      if (MM[awt2] && (MM[awt2]<1000)) MM[awt2]++;
      if (MM[awt3] && (MM[awt3]<1000)) MM[awt3]++;
    }
    
    pel_uitmelding = FALSE;
    if ((MM[awt1] > ((T_max[inmv]/10) + (T_max[afst]/10))) && G[fc] && !SG[fc] && !FG[fc]) {
      MM[awt1] = MM[awt2];
      MM[awt2] = MM[awt3];
      MM[awt3] = FALSE;
      pel_uitmelding = TRUE;
    }
    
    if (!G[fc]) KAPC[fc] &= (~BIT1);
    pri_code = 0;                       /* geen ingreep                                                     */
    if (MM[awt1]) {
      if (PRM[prio] == 1) pri_code = 1; /* aanhouden groen                                                  */
      if (PRM[prio] == 2) pri_code = 2; /* prio. zonder afbreken                                            */
      if (PRM[prio] == 3) pri_code = 3; /* prio. met    afbreken                                            */
    }
    
#ifdef DVM_AANWEZIG
    
    if (pri_code > MAX_DVM_PK[fc]) pri_code = MAX_DVM_PK[fc];
    
#endif                                  /* *** ifdef -> DVM_AANWEZIG                                        */
    
    if (geen_pel && (pri_code > 1)) pri_code = 1; /* geen prioriteitsrealisatie                             */
    if (G[fc]    && (pri_code > 0)) pri_code = 3; /* per definitie                                          */
    if (pri_code == 3) KAPC[fc] |= BIT1;
    
    /* Als bpi hier al waar dan lopende prioriteit afbreken !                                               */
    if (BL[fc] || (BPI[fc]&BIT1) || (_FIL[fc]&BIT0)) {
      MM[awt1]=MM[awt2]=MM[awt3]=0;
    }
    
    /* Altijd aanvraag na afloop inmeldvertraging, tenzij prioriteitscode "0"                               */
    _reset_pel = (A[fc]&BIT3) && !(A[fc]&BIT1) && !(A[fc]&BIT6) && !(A[fc]&BIT8);
    if (R[fc] && !TRG[fc] && (MM[awt1] > (T_max[inmv]/10)) && (HPRI[fc] == 0) && !A[fc] && (pri_code > 0)) {
      A[fc] |=   BIT3;
    }
    if (R[fc] && ((MM[awt1] == 0) || BLK_PRI_REA || (HPRI[fc] > 0) || (pri_code == 0))) {
      A[fc] &= (~BIT3);
    }
    if (_reset_pel && !(A[fc]&BIT3) && !_GRN[fc]) {
      A[fc] = FALSE;
    }
    
    RT[tbl] = G[fc] && (SGD[fc]&BIT5);
    AT[tbl] = T[tbl] && R[fc] && !TRG[fc] && A[fc] && !(SGD[fc]&BIT4) && !(HKII[fc]&BIT4);
    
    if (!G[fc] && (T[tbl] || BLK_PRI_REA || (pri_code <= 1)))         BPI[fc] |= BIT1;
    if ( G[fc] && BLK_PRI_REA && (!(SGD[fc]&BIT5) || pel_uitmelding)) BPI[fc] |= BIT1;
    if ((HPRI[fc] > 0) || BLWT[fc] || FIXATIE) BPI[fc] |= BIT1;
    
    if (!(BPI[fc]&BIT1) && (MM[awt1] > (T_max[inmv]/10))) PEL1[fc] = MM[awt1] - (T_max[inmv]/10);
    if (PEL1[fc] < 255 - instel_onm) ondermax = PEL1[fc] + instel_onm;
    
    if (PEL1[fc] >= (T_max[afkv]/10)) KVPK[fc] = 0;
    else                              KVPK[fc] = (T_max[afkv]/10) - PEL1[fc];
    
    bep_TEG = FALSE;
    if (PEL1[fc] && (!G[fc] || G[fc] && (KGOM[fc] < ondermax))) {
      if (!G[fc]) _TEG = (T_max[afst] + T_max[nmax]) / 10;
      YM[fc] |= BIT4;
      PEL[fc] |= BIT5;
      bep_TEG |= TRUE;
    } else { PEL[fc] &= (~BIT5); if (!G[fc]) KAPC[fc] &= (~BIT1); }
    
    if (G[fc] && (PEL[fc]&BIT5) && (SGD[fc]&BIT5)) {
      A[fc] &= (~BIT2);
      PEL[fc] |= BIT6;
      PEL2[fc] = T_max[nmax]/10;
    }
    
    if (G[fc] && !(PEL[fc]&BIT5) && (PEL[fc]&BIT6)) {
      if (TS && (PEL2[fc] > 0)) PEL2[fc]--;
      if (!MK[fc] || (_FIL[fc]&BIT0) || (HPRI[fc] > 0)) PEL2[fc] = 0;
      if (PEL2[fc] > 0) {
        YM[fc] |= BIT4;
        bep_TEG |= TRUE;
      } else {
        PEL[fc] &= (~BIT6);
      }
    }
    
    if (G[fc] && (PEL[fc]&BIT5) && bep_TEG) { /* bepaal TEG[] bij aktieve peloton ingreep                   */
      
      UITMELD = (T_max[afst]+T_max[nmax])/10;
      
      if (G[fc] && (!MG[fc] || MK[fc]) && (KG[fc] < PEL1[fc]))
        extra = PEL1[fc] - KG[fc];
      else extra = FALSE;
      UITMELD   += extra;
      
      if (UITMELD >= PEL1[fc]) _TEG = UITMELD - PEL1[fc]; /* 1e peloton                                     */
      else          _TEG = FALSE;
      
      if (MM[awt2] > (T_max[inmv]/10)) HPEL = MM[awt2] - (T_max[inmv]/10); /* 2e peloton                    */
      else            HPEL = FALSE;
      
      UITMELD = (T_max[afst]+T_max[nmax])/10;
      if (G[fc] && (!MG[fc] || MK[fc]) && (KG[fc] < HPEL))
        extra = HPEL - KG[fc];
      else extra = FALSE;
      UITMELD   += extra;
      
      if (HPEL < 255 - instel_onm) ondermax = HPEL + instel_onm;
      if (HPEL && (KGOM[fc] < ondermax) && (UITMELD >= HPEL)) _TEG = UITMELD - HPEL;
      
      if (MM[awt3] > (T_max[inmv]/10)) HPEL = MM[awt3] - (T_max[inmv]/10); /* 3e peloton                    */
      else            HPEL = FALSE;
      
      UITMELD = (T_max[afst]+T_max[nmax])/10;
      if (G[fc] && (!MG[fc] || MK[fc]) && (KG[fc] < HPEL))
        extra = HPEL - KG[fc];
      else extra = FALSE;
      UITMELD   += extra;
      
      if (HPEL < 255 - instel_onm) ondermax = HPEL + instel_onm;
      if (HPEL && (KGOM[fc] < ondermax) && (UITMELD >= HPEL)) _TEG = UITMELD - HPEL;
    }
    
    if (G[fc] && !(PEL[fc]&BIT5) && (PEL[fc]&BIT6)) {
      _TEG = PEL2[fc];                  /* bepaal TEG[] na aktieve peloton ingreep                          */
      PEL[fc] |= BIT5;                  /* prioriteitsbit tbv regelstructuur                                */
    }
    
    if (bep_TEG && ((PEL[fc]&BIT5) || (PEL[fc]&BIT6))) {
      if (POV[fc]&BIT5) {
        if (_TEG > TEG[fc]) TEG[fc] = _TEG; /* bepaal resultante TEG[] afhankelijk van BUS aanwezig         */
      } else {
        TEG[fc] = _TEG;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure uitgaande pelotonkoppeling                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_pel_uit(                    /* Fik191001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count de1,                          /* koplus 1                                                         */
    count de2,                          /* koplus 2                                                         */
    count de3,                          /* koplus 3                                                         */
    count tmeet,                        /* T meetperiode                                                    */
    count tmaxth,                       /* T max.hiaat                                                      */
    count grens,                        /* PRM grenswaarde                                                  */
    count us_uit,                       /* koppelsignaal                                                    */
    count mvtg,                         /* MM aantal vtg                                                    */
    count muit)                         /* MM uitsturing aktief                                             */
  {
    bool ED1,ED2,ED3;
    
    if (de1 != NG) ED1 = ED[de1] && !R[fc];
    else           ED1 = FALSE;
    
    if (de2 != NG) ED2 = ED[de2] && !R[fc];
    else           ED2 = FALSE;
    
    if (de3 != NG) ED3 = ED[de3] && !R[fc];
    else           ED3 = FALSE;
    
    AT[tmeet]  = FALSE;
    AT[tmaxth] = FALSE;
    RT[tmeet]  = RT[tmaxth] = SG[fc] || TGG[fc] || G[fc] && !T[tmeet] && !T[tmaxth] && (ED1 || ED2 || ED3);
    
    if ((ED1 || ED2 || ED3) && (RT[tmeet] || T[tmeet])) {
      if (ED1) MM[mvtg] += 1;
      if (ED2) MM[mvtg] += 1;
      if (ED3) MM[mvtg] += 1;
      RT[tmaxth] = TRUE;
    }
    
    if (ET[tmaxth] && !RT[tmaxth]) {
      AT[tmeet] = TRUE;
      MM[mvtg]   = 0;
    }
    
    if (TS && (MM[muit] > 0)) MM[muit]--;
    if ((PRM[grens] > 0) && (MM[mvtg] >= PRM[grens])) {
      MM[mvtg]   = 0;
      MM[muit]   = 3;
      AT[tmeet]  = TRUE;
      AT[tmaxth] = TRUE;
      RT[tmaxth] = FALSE;
    }
    CIF_GUS[us_uit] = BEDRYF && (MM[muit] > 0);
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure fiets prioriteit                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fts_prio(                   /* Fik180308                                                        */
    count fc,                           /* fasecyclus                                                       */
    count dk1,                          /* MM drukknop 1                                                    */
    count dk2,                          /* MM drukknop 2                                                    */
    count ogwt,                         /* T min.wachttijd                                                  */
    count tbl,                          /* T blokkeringstijd                                                */
    count prio)                         /* PRM prio code                                                    */
  {
    count i,j;
    count kpvtg;
    bool kop_pri = FALSE;
    bool kop_mog = FALSE;
    bool adrk = FALSE;
    mulv pri_code = 0;
    
    if (prio != NG) {                   /* controleer instellingen                                          */
      if (PRM[prio] > 2) {
        PRM[prio] = 0;
        CIF_PARM1WIJZAP = -2;
      }
      pri_code = PRM[prio];
    }
    if (geen_fts) pri_code = 0;         /* geen prioriteitsrealisatie                                       */
    
    for (i=0; i<aantal_vtg_fts; i++) {  /* gekoppelde fietsprioriteit aktief?                               */
      kop_mog = FALSE;
      if (kp_vtg_fts[i][0] > 0) {
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] == fc) kop_mog = TRUE;
        }
      }
      if (kop_mog) {
        for (j=1; j<=8; j++) {
          kpvtg = kp_vtg_fts[i][j];
          if (kpvtg != NG) {
            if (G[kpvtg] && (SGD[kpvtg]&BIT5)) kop_pri = TRUE;
          }
        }
      }
    }
    
    if (dk1 != NG) if (R[fc] && (MM[dk1] || (Aled[fc] > 0))) adrk = TRUE;
    if (dk2 != NG) if (R[fc] && (MM[dk2] || (Aled[fc] > 0))) adrk = TRUE;
    
    if (!SG[fc] && !FG[fc]) FTS[fc] &= (~BIT5); /* reset fietsprioriteit                                    */
    /* op SG[] wordt richting als primair gemarkeerd                                                        */
    RT[tbl] = G[fc] && ((SGD[fc]&BIT5) || kop_pri);
    AT[tbl] = T[tbl] && R[fc] && !TRG[fc] && A[fc] && !(SGD[fc]&BIT4) && !(HKII[fc]&BIT4) && !somgk(fc);
    
    if (!G[fc] && ((HPRI[fc] > 0) || BLWT[fc] || T[tbl] || (pri_code == 0))) BPI[fc] |= BIT2;
    if (!G[fc] && !(SGD[fc]&BIT5) && (BLK_PRI_REA || (_dvm > 10))) BPI[fc] |= BIT2;
    
    if (R[fc] && adrk && (TFB_timer[fc] > (T_max[ogwt]/10))) {
      if (!(BPI[fc]&BIT2) && !BL[fc]) FTS[fc] |= BIT5;
    }
    if (!G[fc]) KAPC[fc] &= (~BIT2);
    if (G[fc] && (pri_code > 0)) pri_code = 2; /* per definitie                                             */
    if (pri_code == 2) KAPC[fc] |= BIT2;
    if (!G[fc] && !(FTS[fc]&BIT5)) KAPC[fc] &= (~BIT2);
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV KOPPELINGEN LANGZAAM VERKEER                                                              */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure definitie koppelingen langzaam verkeer                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_def_kop_lvk(                /* Fik120310                                                        */
    
    /* Bij een half getrapte oversteek zijn:                                                                */
    /* fc1 buitenste lantaarn = volgrichting van fc3                                                        */
    /* fc2 binnenste lantaarn = voeding      van fc3                                                        */
    
    count fc1,                          /* fasecyclus 1 voedend voetganger 1                                */
    count fc2,                          /* fasecyclus 2 volgrichting vtg fc1                                */
    count fc3,                          /* fasecyclus 3 voedend voetganger 3                                */
    count fc4,                          /* fasecyclus 4 volgrichting vtg fc3                                */
    count fc5,                          /* fasecyclus 5 gekoppelde fietser 5                                */
    count fc6,                          /* fasecyclus 6 volgrichting fts fc5                                */
    count fc7,                          /* fasecyclus 7 gekoppelde fietser 7                                */
    count fc8,                          /* fasecyclus 8 volgrichting fts fc7                                */
    count drkmm1,                       /* mm drukknop buitenzijde fc1                                      */
    count drkmm3,                       /* mm drukknop buitenzijde fc3                                      */
    count kpt1,                         /* t koppeltijd vanaf fc1 (sg of eg)                                */
    count kpt3,                         /* t koppeltijd vanaf fc3 (sg of eg)                                */
    count kpt56,                        /* t koppeltijd fc5 ->fc6 (eg -> eg)                                */
    count kpt78)                        /* t koppeltijd fc7 ->fc8 (eg -> eg)                                */
  {
    count i;
    bool _wtv = FALSE;
    
    /* bepaal werkwijze                                                                                     */
    kp_vtg_fts[aantal_vtg_fts][0] = 0;  /* werkwijze niet valide                                            */
    if ((fc1 != NG) && (fc2 == NG) && (fc3 == NG) && (fc4 == NG)) kp_vtg_fts[aantal_vtg_fts][0] = 1;
    /* werkwijze lvk oversteek                                                                              */
    if ((fc1 == NG) && (fc2 == NG) && (fc3 == NG) && (fc4 != NG)) kp_vtg_fts[aantal_vtg_fts][0] = 1;
    if ((fc1 != NG) && (fc2 == NG) && (fc3 == NG) && (fc4 != NG)) kp_vtg_fts[aantal_vtg_fts][0] = 1;
    if ((fc1 == NG) && (fc2 == NG) && (fc3 == NG) && (fc4 == NG)) {
      if ((fc5 != NG) || (fc6 != NG) || (fc7 != NG) || (fc8 != NG)) kp_vtg_fts[aantal_vtg_fts][0] = 1;
    }
    
    if ((fc1 != NG) && (fc2 == NG) && (fc3 != NG) && (fc4 == NG)) kp_vtg_fts[aantal_vtg_fts][0] = 2;
    /* werkwijze gov oversteek                                                                              */
    if ((fc1 != NG) && (fc2 != NG) && (fc3 == NG) && (fc4 == NG)) kp_vtg_fts[aantal_vtg_fts][0] = 3;
    /* werkwijze bbv oversteek                                                                              */
    if ((fc1 == NG) && (fc2 == NG) && (fc3 != NG) && (fc4 != NG)) kp_vtg_fts[aantal_vtg_fts][0] = 3;
    /* werkwijze bbv oversteek                                                                              */
    if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG) && (fc4 != NG)) kp_vtg_fts[aantal_vtg_fts][0] = 4;
    /* werkwijze trp oversteek                                                                              */
    if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG) && (fc4 == NG)) kp_vtg_fts[aantal_vtg_fts][0] = 5;
    /* half getrapte oversteek                                                                              */
    
    if ((fc5 == NG) && (fc6 != NG) || (fc7 == NG) && (fc8 != NG)) kp_vtg_fts[aantal_vtg_fts][0] = 0;
    /* werkwijze niet valide                                                                                */
    
    kp_vtg_fts[aantal_vtg_fts][1] = fc1;
    kp_vtg_fts[aantal_vtg_fts][2] = fc2;
    kp_vtg_fts[aantal_vtg_fts][3] = fc3;
    kp_vtg_fts[aantal_vtg_fts][4] = fc4;
    kp_vtg_fts[aantal_vtg_fts][5] = fc5;
    kp_vtg_fts[aantal_vtg_fts][6] = fc6;
    kp_vtg_fts[aantal_vtg_fts][7] = fc7;
    kp_vtg_fts[aantal_vtg_fts][8] = fc8;
    kp_vtg_fts[aantal_vtg_fts][9] = drkmm1;
    kp_vtg_fts[aantal_vtg_fts][10]= drkmm3;
    kp_vtg_fts[aantal_vtg_fts][11]= kpt1;
    kp_vtg_fts[aantal_vtg_fts][12]= kpt3;
    kp_vtg_fts[aantal_vtg_fts][13]= kpt56;
    kp_vtg_fts[aantal_vtg_fts][14]= kpt78;
    kp_vtg_fts[aantal_vtg_fts][15]= 0;  /* tbv van schakelen fictieve conflicten                            */
    kp_vtg_fts[aantal_vtg_fts][16]= 0;
    
    for (i=1; i<=8; i++) {
      if (kp_vtg_fts[aantal_vtg_fts][i] != NG) {
        if (WTVFC[kp_vtg_fts[aantal_vtg_fts][i]]&BIT0) _wtv = TRUE;
      }
    }
    if (_wtv) {
      for (i=1; i<=8; i++) {
        if (kp_vtg_fts[aantal_vtg_fts][i] != NG) {
          if (!(WTVFC[kp_vtg_fts[aantal_vtg_fts][i]]&BIT0)) WTVFC[kp_vtg_fts[aantal_vtg_fts][i]] |= BIT1;
        }
      }
    }
    if (aantal_vtg_fts < FC_MAX - 1) aantal_vtg_fts++;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TTR tbv langzaam verkeersoversteken                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_corr_ttr_vtg(void)          /* Fik180308                                                        */
  {
    count i,j;
    mulv  max_ttr;
    mulv  max_ttc;
    mulv  max_ttw;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        max_ttr = max_ttc = max_ttw = 0;
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if ((j <= 4) && G[kp_vtg_fts[i][j]]) KGCA[kp_vtg_fts[i][j]] = KG[kp_vtg_fts[i][j]];
            if  (TTR[kp_vtg_fts[i][j]] > max_ttr) max_ttr = TTR[kp_vtg_fts[i][j]];
            if  (TTC[kp_vtg_fts[i][j]] > max_ttc) max_ttc = TTC[kp_vtg_fts[i][j]];
            if ((TTW[kp_vtg_fts[i][j]] > max_ttw)
                && (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]] || GL[kp_vtg_fts[i][j]]
                  || TRG[kp_vtg_fts[i][j]])) max_ttw = TTW[kp_vtg_fts[i][j]];
          }
        }
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (!G[kp_vtg_fts[i][j]]) TTR[kp_vtg_fts[i][j]] = max_ttr;
            /* geen correctie TTC[] ivm variabel meeverlenggroen                                            */
            /* if (!G[kp_vtg_fts[i][j]]) TTC[kp_vtg_fts[i][j]] = max_ttc;                                   */
            if (!G[kp_vtg_fts[i][j]] && (TTW[kp_vtg_fts[i][j]]<max_ttw)) TTW[kp_vtg_fts[i][j]] = max_ttw;
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TTP tbv langzaam verkeersoversteken                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_ttp_vtg_sgd(void)           /* Fik180308                                                        */
  {
    count i,j;
    mulv  max_ttp;
    mulv  correctie;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        max_ttp = 0;
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if ((TTP[kp_vtg_fts[i][j]] < 255)
                && (TTP[kp_vtg_fts[i][j]] > max_ttp)) max_ttp = TTP[kp_vtg_fts[i][j]];
          }
        }
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if ((TTP[kp_vtg_fts[i][j]] < 255)
                && (TTP[kp_vtg_fts[i][j]] < max_ttp)) {
              correctie = max_ttp - TTP[kp_vtg_fts[i][j]];
              TTP[kp_vtg_fts[i][j]] += correctie;
              TEG[kp_vtg_fts[i][j]] += correctie;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure vaste meeaanvragen tbv langzaam verkeeroversteken                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_mav_vtg(void)               /* Fik121104                                                        */
  {
    mulv i,j;
    bool vtg_aanvraag;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    mulv kpa1,kpa3,kpt1,kpt3;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        
        fc1 = kp_vtg_fts[i][1];
        fc2 = kp_vtg_fts[i][2];
        fc3 = kp_vtg_fts[i][3];
        fc4 = kp_vtg_fts[i][4];
        fc5 = kp_vtg_fts[i][5];
        fc6 = kp_vtg_fts[i][6];
        fc7 = kp_vtg_fts[i][7];
        fc8 = kp_vtg_fts[i][8];
        kpa1= kp_vtg_fts[i][9];
        kpa3= kp_vtg_fts[i][10];
        kpt1= kp_vtg_fts[i][11];
        kpt3= kp_vtg_fts[i][12];
        
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] && (Aled[kp_vtg_fts[i][j]] > 0)) set_A(kp_vtg_fts[i][j]);
          }
        }
        
        vtg_aanvraag = FALSE;
        if (kp_vtg_fts[i][0] == 1) {    /* langzaam verkeer oversteek                                       */
          for (j=1; j<=4; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]]) vtg_aanvraag = TRUE;
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (kpa1 != NG) {
              if (R[fc1] && A[fc1] && MM[kpa1]) {
                set_A(fc3);
                vtg_aanvraag = TRUE;
              }
            } else {                    /* altijd koppeling als geen buitenste drukknop aanwezig maar wel   */
              /* koppeltijd gedefinieeerd                                                                   */
              if (R[fc1] && A[fc1] && (kpt1 != NG)) {
                set_A(fc3);
                vtg_aanvraag = TRUE;
              }
            }
            if (kpa3 != NG) {
              if (R[fc3] && A[fc3] && MM[kpa3]) {
                set_A(fc1);
                vtg_aanvraag = TRUE;
              }
            } else {                    /* altijd koppeling als geen buitenste drukknop aanwezig maar wel   */
              /* koppeltijd gedefinieeerd                                                                   */
              if (R[fc3] && A[fc3] && (kpt3 != NG)) {
                set_A(fc1);
                vtg_aanvraag = TRUE;
              }
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
          /* werkwijze binnenste-buiten of getrapte oversteek                                               */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (R[fc1] && A[fc1] || G[fc1]) {
              set_A(fc2);
              vtg_aanvraag = TRUE;
            }
            if (R[fc2] && A[fc2] && (kp_vtg_fts[i][0] == 3)) {
              vtg_aanvraag = TRUE;
            }
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (R[fc3] && A[fc3] || G[fc3]) {
              set_A(fc4);
              vtg_aanvraag = TRUE;
            }
            if (R[fc4] && A[fc4] && (kp_vtg_fts[i][0] == 3)) {
              vtg_aanvraag = TRUE;
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (kpa1 != NG) {
              if (R[fc2] && A[fc2] && MM[kpa1]) {
                set_A(fc3);
                vtg_aanvraag = TRUE;
              }
            } else {                    /* altijd koppeling als geen buitenste drukknop aanwezig maar wel   */
              /* koppeltijd gedefinieeerd                                                                   */
              if (R[fc2] && A[fc2] && (kpt1 != NG)) {
                set_A(fc3);
                vtg_aanvraag = TRUE;
              }
            }
            if (kpa3 != NG) {
              if (R[fc3] && A[fc3] && MM[kpa3])  {
                set_A(fc1);
                vtg_aanvraag = TRUE;
              }
            } else {                    /* altijd koppeling als geen buitenste drukknop aanwezig maar wel   */
              /* koppeltijd gedefinieeerd                                                                   */
              if (R[fc3] && A[fc3] && (kpt3 != NG)) {
                set_A(fc1);
                vtg_aanvraag = TRUE;
              }
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) {
          if (R[fc5] && R[fc6] && (A[fc6]&BIT1)) set_A(fc5);
          if (R[fc5] && A[fc5] || G[fc5]) {
            if (R[fc6]) set_A(fc6);
          } else {
            A[fc6] = FALSE;
          }
        }
        
        if ((fc7 != NG) && (fc8 != NG)) {
          if (R[fc7] && R[fc8] && (A[fc8]&BIT1)) set_A(fc7);
          if (R[fc7] && A[fc7] || G[fc7]) {
            if (R[fc8]) set_A(fc8);
          } else {
            A[fc8] = FALSE;
          }
        }
        
        if (vtg_aanvraag) {             /* fietsers altijd mee met buitenste voetganger                     */
          for (j=5; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) set_A(kp_vtg_fts[i][j]);
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure schakel fictieve conflicten                                                                    */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fk_vtg(void)                /* Fik170831                                                        */
  {
    mulv i,j,x,fc;
    bool vtg_aanvraag;
    mulv aantal_aanvragen;
    mulv aantal_rood_aanvragen;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    bool SCH_altijd_FK = FALSE;
    
#ifdef schaltijd_FK
    
    SCH_altijd_FK = SCH[schaltijd_FK];
    
#endif                                  /* *** ifdef -> schaltijd_FK                                        */
    
#ifdef CRSV2CCOL
    
    SCH_altijd_FK |= (AKTPRG > 0);
    
#endif                                  /* *** ifdef -> CRSV2CCOL                                           */
    
    for (i=0; i<aantal_vtg_fts; i++) {
      
      /* alleen bij gescheiden, getrapte en half getrapte oversteek                                         */
      if ((kp_vtg_fts[i][0] == 2) || (kp_vtg_fts[i][0] == 4) || (kp_vtg_fts[i][0] == 5)) {
        
        fc1 = kp_vtg_fts[i][1];
        fc2 = kp_vtg_fts[i][2];
        fc3 = kp_vtg_fts[i][3];
        fc4 = kp_vtg_fts[i][4];
        fc5 = kp_vtg_fts[i][5];
        fc6 = kp_vtg_fts[i][6];
        fc7 = kp_vtg_fts[i][7];
        fc8 = kp_vtg_fts[i][8];
        
        vtg_aanvraag = FALSE;
        aantal_aanvragen = aantal_rood_aanvragen = x = 0;
        for (j=1; j<=8; j++) {          /* bepaal aantal aanvragen                                          */
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] &&  A[kp_vtg_fts[i][j]] ||
                G[kp_vtg_fts[i][j]] || GL[kp_vtg_fts[i][j]] || TRG[kp_vtg_fts[i][j]]) {
              aantal_aanvragen++;
              if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]]) aantal_rood_aanvragen++;
              if (j<=4) {
                x = j;
                vtg_aanvraag = TRUE;
              }
            }
          }
        }
        
        if (vtg_aanvraag && (aantal_aanvragen == 1) && (aantal_rood_aanvragen == 1) && !SCH_altijd_FK) {
          
          /* controleer of vtg dezelfde als in de vorige machineslag, dan verder niets te doen              */
          if (x == kp_vtg_fts[i][15]) kp_vtg_fts[i][x] = NG;
          else {
            /* vorige machineslag nog geen voetganger apart gezet                                           */
            if (kp_vtg_fts[i][15] == 0) {
              /* verwijder fiktieve konflikten voetganger die apart wordt gezet                             */
              
              if ((kp_vtg_fts[i][0] == 2) && ((x == 1) || (x == 3)) ||
                  (kp_vtg_fts[i][0] == 4) && ((x == 2) || (x == 4)) ||
                  (kp_vtg_fts[i][0] == 5) && ((x == 1) || (x == 3))) {
                
                kp_vtg_fts[i][15] = x;
                kp_vtg_fts[i][16] = kp_vtg_fts[i][x];
                kp_vtg_fts[i][x]  = NG;
                
                fc = kp_vtg_fts[i][16];
                if (fc != NG) AFC[fc] = FALSE;
              }
            } else {
              /* zet eerst oude fiktieve konflikten terug                                                   */
              
              /* gescheiden oversteek                                                                       */
              if ((kp_vtg_fts[i][0] == 2) && ((kp_vtg_fts[i][15] == 1) || (kp_vtg_fts[i][15] == 3))) {
                fc = kp_vtg_fts[i][16];
                if (fc != NG) AFC[fc] = TRUE;
              }
              
              /* getrapte oversteek                                                                         */
              if ((kp_vtg_fts[i][0] == 4) && ((kp_vtg_fts[i][15] == 2) || (kp_vtg_fts[i][15] == 4))) {
                fc = kp_vtg_fts[i][16];
                if (fc != NG) AFC[fc] = TRUE;
              }
              
              /* half getrapte oversteek                                                                    */
              if ((kp_vtg_fts[i][0] == 5) && ((kp_vtg_fts[i][15] == 1) || (kp_vtg_fts[i][15] == 3))) {
                fc = kp_vtg_fts[i][16];
                if (fc != NG) AFC[fc] = TRUE;
              }
              
              kp_vtg_fts[i][15] = 0;
              kp_vtg_fts[i][16] = 0;
              
              /* verwijder fiktieve konflikten voetganger die apart wordt gezet                             */
              
              kp_vtg_fts[i][15] = x;
              kp_vtg_fts[i][16] = kp_vtg_fts[i][x];
              kp_vtg_fts[i][x]  = NG;
              fc = kp_vtg_fts[i][16];
              if (fc != NG) AFC[fc] = FALSE;
            }
          }
        } else {
          /* zet zonodig oude fiktieve konflikten terug                                                     */
          if (kp_vtg_fts[i][15] != 0) {
            
            /* gescheiden oversteek                                                                         */
            if ((kp_vtg_fts[i][0] == 2) && ((kp_vtg_fts[i][15] == 1) || (kp_vtg_fts[i][15] == 3))) {
              fc = kp_vtg_fts[i][16];
              if (fc != NG) AFC[fc] = TRUE;
            }
            
            /* getrapte oversteek                                                                           */
            if ((kp_vtg_fts[i][0] == 4) && ((kp_vtg_fts[i][15] == 2) || (kp_vtg_fts[i][15] == 4))) {
              fc = kp_vtg_fts[i][16];
              if (fc != NG) AFC[fc] = TRUE;
            }
            
            /* half getrapte oversteek                                                                      */
            if ((kp_vtg_fts[i][0] == 5) && ((kp_vtg_fts[i][15] == 1) || (kp_vtg_fts[i][15] == 3))) {
              fc = kp_vtg_fts[i][16];
              if (fc != NG) AFC[fc] = TRUE;
            }
            
            kp_vtg_fts[i][15] = 0;
            kp_vtg_fts[i][16] = 0;
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer PRIM[], ALTM[] en KAPM[] tbv langzaam verkeeroversteken                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_alt_vtg_sgd(void)           /* Fik110716                                                        */
  {
    mulv i;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    mulv kpt1,kpt3,kpt56,kpt78;
    bool kpa1,kpa3;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fc1  = kp_vtg_fts[i][1];
        fc2  = kp_vtg_fts[i][2];
        fc3  = kp_vtg_fts[i][3];
        fc4  = kp_vtg_fts[i][4];
        fc5  = kp_vtg_fts[i][5];
        fc6  = kp_vtg_fts[i][6];
        fc7  = kp_vtg_fts[i][7];
        fc8  = kp_vtg_fts[i][8];
        /* altijd koppeling als geen buitenste drukknop aanwezig maar wel koppeltijd gedefinieeerd          */
        if (kp_vtg_fts[i][9]  != NG) kpa1 = (MM[kp_vtg_fts[i][9]] != 0);
        else                         kpa1 = TRUE;
        if (kp_vtg_fts[i][10] != NG) kpa3 = (MM[kp_vtg_fts[i][10]] != 0);
        else                         kpa3 = TRUE;
        
        if (kp_vtg_fts[i][13] != NG) kpt56 = T_max[kp_vtg_fts[i][13]]/10;
        else                         kpt56 = 0;
        if (kp_vtg_fts[i][14] != NG) kpt78 = T_max[kp_vtg_fts[i][14]]/10;
        else                         kpt78 = 0;
        
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          kpt1 = kp_vtg_fts[i][11];
          kpt3 = kp_vtg_fts[i][12];
          if ((fc1 != NG) && (fc3 != NG)) {
            if (kpt3 != NG) {
              if (!G[fc1] || SG[fc3] && kpa3 || RT[kpt3] || T[kpt3]) {
                if ((T_max[kpt3]/10) > PRIM[fc1]) PRIM[fc1] = (T_max[kpt3]/10);
                if ((T_max[kpt3]/10) > ALTM[fc1]) ALTM[fc1] = (T_max[kpt3]/10);
                if ((T_max[kpt3]/10) > KAPM[fc1]) KAPM[fc1] = (T_max[kpt3]/10);
              }
            }
            if (kpt1 != NG) {
              if (!G[fc3] || SG[fc1] && kpa1 || RT[kpt1] || T[kpt1]) {
                if ((T_max[kpt1]/10) > PRIM[fc3]) PRIM[fc3] = (T_max[kpt1]/10);
                if ((T_max[kpt1]/10) > ALTM[fc3]) ALTM[fc3] = (T_max[kpt1]/10);
                if ((T_max[kpt1]/10) > KAPM[fc3]) KAPM[fc3] = (T_max[kpt1]/10);
              }
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
                                        /* werkwijze binnenste-buiten of getrapte oversteek                 */
          if (kp_vtg_fts[i][11] != NG) kpt1 = T_max[kp_vtg_fts[i][11]]/10;
          else                         kpt1 = 0;
          if (kp_vtg_fts[i][12] != NG) kpt3 = T_max[kp_vtg_fts[i][12]]/10;
          else                         kpt3 = 0;
          if ((fc1 != NG) && (fc2 != NG)) {
            if ((PRIM[fc1] + kpt1) > PRIM[fc2]) PRIM[fc2] = PRIM[fc1] + kpt1;
            if ((ALTM[fc1] + kpt1) > ALTM[fc2]) ALTM[fc2] = ALTM[fc1] + kpt1;
            if ((KAPM[fc1] + kpt1) > KAPM[fc2]) KAPM[fc2] = KAPM[fc1] + kpt1;
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if ((PRIM[fc3] + kpt3) > PRIM[fc4]) PRIM[fc4] = PRIM[fc3] + kpt3;
            if ((ALTM[fc3] + kpt3) > ALTM[fc4]) ALTM[fc4] = ALTM[fc3] + kpt3;
            if ((KAPM[fc3] + kpt3) > KAPM[fc4]) KAPM[fc4] = KAPM[fc3] + kpt3;
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          kpt1 = kp_vtg_fts[i][11];
          kpt3 = kp_vtg_fts[i][12];
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (kpt3 != NG) {
              if (!G[fc1] || SG[fc3] && kpa3 || RT[kpt3] || T[kpt3]) {
                if ((T_max[kpt3]/10) > PRIM[fc1]) PRIM[fc1] = (T_max[kpt3]/10);
                if ((T_max[kpt3]/10) > ALTM[fc1]) ALTM[fc1] = (T_max[kpt3]/10);
                if ((T_max[kpt3]/10) > KAPM[fc1]) KAPM[fc1] = (T_max[kpt3]/10);
              }
            }
            if (kpt1 != NG) {
              if (!G[fc3] || SG[fc2] && kpa1 || RT[kpt1] || T[kpt1]) {
                if ((T_max[kpt1]/10) > PRIM[fc3]) PRIM[fc3] = (T_max[kpt1]/10);
                if ((T_max[kpt1]/10) > ALTM[fc3]) ALTM[fc3] = (T_max[kpt1]/10);
                if ((T_max[kpt1]/10) > KAPM[fc3]) KAPM[fc3] = (T_max[kpt1]/10);
              }
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) {
          if ((PRIM[fc5] + kpt56) > PRIM[fc6]) PRIM[fc6] = PRIM[fc5] + kpt56;
          if ((ALTM[fc5] + kpt56) > ALTM[fc6]) ALTM[fc6] = ALTM[fc5] + kpt56;
          if ((KAPM[fc5] + kpt56) > KAPM[fc6]) KAPM[fc6] = KAPM[fc5] + kpt56;
        }
        if ((fc7 != NG) && (fc8 != NG)) {
          if ((PRIM[fc7] + kpt78) > PRIM[fc8]) PRIM[fc8] = PRIM[fc7] + kpt78;
          if ((ALTM[fc7] + kpt78) > ALTM[fc8]) ALTM[fc8] = ALTM[fc7] + kpt78;
          if ((KAPM[fc7] + kpt78) > KAPM[fc8]) KAPM[fc8] = KAPM[fc7] + kpt78;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure uitstel primaire realisatie tbv langzaam verkeeroversteken                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_Xvtg_sgd(void)              /* Fik110716                                                        */
  {                                     /* Fik181101 bugfix Poncin voetgangersoversteek                     */
    mulv i;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fc1  = kp_vtg_fts[i][1];
        fc2  = kp_vtg_fts[i][2];
        fc3  = kp_vtg_fts[i][3];
        fc4  = kp_vtg_fts[i][4];
        fc5  = kp_vtg_fts[i][5];
        fc6  = kp_vtg_fts[i][6];
        fc7  = kp_vtg_fts[i][7];
        fc8  = kp_vtg_fts[i][8];
        
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (somfbk(fc1) || somfbk(fc3)) {
              X[fc1] |= BIT3;
              X[fc3] |= BIT3;
              if (fc2 != NG) X[fc2] |= BIT3; /* Fik181101                                                   */
              if (fc4 != NG) X[fc4] |= BIT3; /* Fik181101                                                   */
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 4) {    /* werkwijze getrapte oversteek                                     */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (somfbk(fc1)) X[fc2] |= BIT3;
            if (somfbk(fc2)) X[fc1] |= BIT3; /* Fik181101                                                   */
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (somfbk(fc3)) X[fc4] |= BIT3;
            if (somfbk(fc4)) X[fc3] |= BIT3; /* Fik181101                                                   */
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (somfbk(fc1) || somfbk(fc2) || somfbk(fc3)) {
              X[fc1] |= BIT3;
              X[fc2] |= BIT3;
              X[fc3] |= BIT3;
              if (fc4 != NG) X[fc4] |= BIT3; /* Fik181101                                                   */
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 6) {    /* werkwijze half getrapte oversteek                                */
          if ((fc3 != NG) && (fc4 != NG) && (fc1 != NG)) {
            if (somfbk(fc3) || somfbk(fc4) || somfbk(fc1)) {
              X[fc3] |= BIT3;
              X[fc4] |= BIT3;
              X[fc1] |= BIT3;
              if (fc2 != NG) X[fc2] |= BIT3; /* Fik181101                                                   */
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) {
          if (somfbk(fc5)) X[fc6] |= BIT3;
        }
        
        if ((fc7 != NG) && (fc8 != NG)) {
          if (somfbk(fc7)) X[fc8] |= BIT3;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer realisatie-geheugen tbv langzaam verkeeroversteken                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_rea_vtg_sgd(void)           /* Fik110716                                                        */
  {
    mulv i,j;
    bool voorw;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fc1  = kp_vtg_fts[i][1];
        fc2  = kp_vtg_fts[i][2];
        fc3  = kp_vtg_fts[i][3];
        fc4  = kp_vtg_fts[i][4];
        fc5  = kp_vtg_fts[i][5];
        fc6  = kp_vtg_fts[i][6];
        fc7  = kp_vtg_fts[i][7];
        fc8  = kp_vtg_fts[i][8];
        
        voorw = FALSE;
        if (fc5 != NG) { if (G[fc5] && ((SGD[fc5]&BIT0) || (SGD[fc5]&BIT3))) voorw = TRUE; }
        if (fc7 != NG) { if (G[fc7] && ((SGD[fc7]&BIT0) || (SGD[fc7]&BIT3))) voorw = TRUE; }
        
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (!G[kp_vtg_fts[i][j]] && !A[kp_vtg_fts[i][j]]) SGD[kp_vtg_fts[i][j]] |= BIT2;
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (!G[fc1] && !A[fc1] && G[fc3] && ((SGD[fc3]&BIT0) || (SGD[fc3]&BIT3))) {
              SGD[fc1] |= BIT2;
            }
            if (!G[fc3] && !A[fc3] && G[fc1] && ((SGD[fc1]&BIT0) || (SGD[fc1]&BIT3))) {
              SGD[fc3] |= BIT2;
            }
            if (!(SGD[fc1]&BIT2) && !G[fc1] || !(SGD[fc3]&BIT2) && !G[fc3]) {
              SGD[fc1] &= (~BIT2);
              SGD[fc3] &= (~BIT2);
            }
            if ((SGD[fc1]&BIT2) && G[fc3]) X[fc3] &= (~BIT3);
            if ((SGD[fc3]&BIT2) && G[fc1]) X[fc1] &= (~BIT3);
          }
        }
        
        if (kp_vtg_fts[i][0] == 3) {    /* werkwijze binnenste-buiten oversteek                             */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (!G[fc1] && !A[fc1] && G[fc2] && ((SGD[fc2]&BIT0) || (SGD[fc2]&BIT3))) {
              SGD[fc1] |= BIT2;
            }
            if (!(SGD[fc1]&BIT2)) SGD[fc2] &= (~BIT2);
            if (!(SGD[fc2]&BIT2) && !G[fc2]) SGD[fc1] &= (~BIT2);
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (!G[fc3] && !A[fc3] && G[fc4] && ((SGD[fc4]&BIT0) || (SGD[fc4]&BIT3))) {
              SGD[fc3] |= BIT2;
            }
            if (!(SGD[fc3]&BIT2)) SGD[fc4] &= (~BIT2);
            if (!(SGD[fc4]&BIT2) && !G[fc4]) SGD[fc3] &= (~BIT2);
          }
        }
        
        if (kp_vtg_fts[i][0] == 4) {    /* werkwijze getrapte oversteek                                     */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (!G[fc1] && !A[fc1] && G[fc2] && ((SGD[fc2]&BIT0) || (SGD[fc2]&BIT3))) {
              SGD[fc1] |= BIT2;
            }
            if (!(SGD[fc1]&BIT2)) SGD[fc2] &= (~BIT2);
            if (!(SGD[fc2]&BIT2) && !G[fc2]) SGD[fc1] &= (~BIT2);
            
            if ((SGD[fc1]&BIT2) && G[fc2]) X[fc2] &= (~BIT3);
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (!G[fc3] && !A[fc3] && G[fc4] && ((SGD[fc4]&BIT0) || (SGD[fc4]&BIT3))) {
              SGD[fc3] |= BIT2;
            }
            if (!(SGD[fc3]&BIT2)) SGD[fc4] &= (~BIT2);
            if (!(SGD[fc4]&BIT2) && !G[fc4]) SGD[fc3] &= (~BIT2);
            
            if ((SGD[fc3]&BIT2) && G[fc4]) X[fc4] &= (~BIT3);
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (!G[fc2] && !A[fc2] && G[fc3] && ((SGD[fc3]&BIT0) || (SGD[fc3]&BIT3))) {
              SGD[fc2] |= BIT2;
            }
            if (!G[fc3] && !A[fc3] && G[fc1] && ((SGD[fc1]&BIT0) || (SGD[fc1]&BIT3))) {
              SGD[fc3] |= BIT2;
            }
            if (!(SGD[fc1]&BIT2) && !G[fc1] || !(SGD[fc2]&BIT2) && !G[fc2] || !(SGD[fc3]&BIT2) && !G[fc3]) {
              SGD[fc1] &= (~BIT2);
              SGD[fc2] &= (~BIT2);
              SGD[fc3] &= (~BIT2);
            }
            if ((SGD[fc2]&BIT2) && G[fc3]) X[fc3] &= (~BIT3);
            if ((SGD[fc3]&BIT2) && G[fc1]) X[fc1] &= (~BIT3);
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) {
          if (!G[fc5] && !A[fc5] && G[fc6] && ((SGD[fc6]&BIT0) || (SGD[fc6]&BIT3))) {
            SGD[fc5] |= BIT2;
          }
          if (!(SGD[fc5]&BIT2)) SGD[fc6] &= (~BIT2);
          if (!(SGD[fc6]&BIT2) && !G[fc6]) SGD[fc5] &= (~BIT2);
          
          if ((SGD[fc5]&BIT2) && G[fc6]) X[fc6] &= (~BIT3);
        }
        
        if ((fc7 != NG) && (fc8 != NG)) {
          if (!G[fc7] && !A[fc7] && G[fc8] && ((SGD[fc8]&BIT0) || (SGD[fc8]&BIT3))) {
            SGD[fc7] |= BIT2;
          }
          if (!(SGD[fc7]&BIT2)) SGD[fc8] &= (~BIT2);
          if (!(SGD[fc8]&BIT2) && !G[fc8]) SGD[fc7] &= (~BIT2);
          
          if ((SGD[fc7]&BIT2) && G[fc8]) X[fc8] &= (~BIT3);
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TEG[] tbv langzaam verkeeroversteken                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_teg_vtg_sgd(void)           /* Fik181101                                                        */
  {
    mulv i;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    mulv kpt1,kpt3,kpt56,kpt78;
    bool kpa1,kpa3;
    mulv naloop12,naloop34;
    mulv naloop56,naloop78;
    mulv maxteg;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fc1  = kp_vtg_fts[i][1];
        fc2  = kp_vtg_fts[i][2];
        fc3  = kp_vtg_fts[i][3];
        fc4  = kp_vtg_fts[i][4];
        fc5  = kp_vtg_fts[i][5];
        fc6  = kp_vtg_fts[i][6];
        fc7  = kp_vtg_fts[i][7];
        fc8  = kp_vtg_fts[i][8];
        kpt1 = kp_vtg_fts[i][11];
        kpt3 = kp_vtg_fts[i][12];
        kpt56= kp_vtg_fts[i][13];
        kpt78= kp_vtg_fts[i][14];
        /* altijd koppeling als geen buitenste drukknop aanwezig maar wel koppeltijd gedefinieeerd          */
        if (kp_vtg_fts[i][9]  != NG) kpa1 = (MM[kp_vtg_fts[i][9]] != 0);
        else                         kpa1 = TRUE;
        if (kp_vtg_fts[i][10] != NG) kpa3 = (MM[kp_vtg_fts[i][10]] != 0);
        else                         kpa3 = TRUE;
        
        naloop12 = naloop34 = naloop56 = naloop78 = 0;
        
        if ((kp_vtg_fts[i][0] == 1) || (kp_vtg_fts[i][0] == 2) || (kp_vtg_fts[i][0] == 5)) {
          /* werkwijze gescheiden of half getrapte oversteek                                                */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (kpt3 != NG) {
              if (SG[fc1] && kpa3 && ((T_max[kpt3]/10) > TEG[fc1])) TEG[fc1] = T_max[kpt3]/10;
              if (G[fc1] && T[kpt3] && ((T_max[kpt3] - T_timer[kpt3])/10 > TEG[fc1])) {
                TEG[fc1] = (T_max[kpt3] - T_timer[kpt3])/10;
              }
            }
            if (kpt1 != NG) {
              if (SG[fc3] && kpa1 && ((T_max[kpt1]/10) > TEG[fc3])) TEG[fc3] = T_max[kpt1]/10;
              if (G[fc3] && T[kpt1] && ((T_max[kpt1] - T_timer[kpt1])/10 > TEG[fc3])) {
                TEG[fc3] = (T_max[kpt1] - T_timer[kpt1])/10;
              }
            }
            if (kp_vtg_fts[i][0] == 1) {
              maxteg = 0;
              if (fc1 != NG) { if (G[fc1] && (TEG[fc1] > maxteg)) maxteg = TEG[fc1]; }
              if (fc2 != NG) { if (G[fc2] && (TEG[fc2] > maxteg)) maxteg = TEG[fc2]; }
              if (fc3 != NG) { if (G[fc3] && (TEG[fc3] > maxteg)) maxteg = TEG[fc3]; }
              if (fc4 != NG) { if (G[fc4] && (TEG[fc4] > maxteg)) maxteg = TEG[fc4]; }
              
              if (fc1 != NG) { if (G[fc1]) TEG[fc1] = maxteg; }
              if (fc2 != NG) { if (G[fc2]) TEG[fc2] = maxteg; }
              if (fc3 != NG) { if (G[fc3]) TEG[fc3] = maxteg; }
              if (fc4 != NG) { if (G[fc4]) TEG[fc4] = maxteg; }
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
                                        /* werkwijze binnenste-buiten of getrapte oversteek                 */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt1 != NG) naloop12 = T_max[kpt1]/10;
            if (G[fc1] && (TEG[fc1] + naloop12 > TEG[fc2])) TEG[fc2] = TEG[fc1] + naloop12;
            if (!G[fc1] && (naloop12 > 0)) {
              if (T[kpt1] && ((T_max[kpt1] - T_timer[kpt1])/10 > TEG[fc2])) {
                TEG[fc2] = (T_max[kpt1] - T_timer[kpt1])/10;
              }
            }
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (kpt3 != NG) naloop34 = T_max[kpt3]/10;
            if (G[fc3] && (TEG[fc3] + naloop34 > TEG[fc4])) TEG[fc4] = TEG[fc3] + naloop34;
            if (!G[fc3] && (naloop34 > 0)) {
              if (T[kpt3] && ((T_max[kpt3] - T_timer[kpt3])/10 > TEG[fc4])) {
                TEG[fc4] = (T_max[kpt3] - T_timer[kpt3])/10;
              }
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) {
          if (kpt56 != NG) naloop56 = T_max[kpt56]/10;
          if (G[fc5] && (TEG[fc5] + naloop56 > TEG[fc6])) TEG[fc6] = TEG[fc5] + naloop56;
          if (!G[fc5] && (naloop56 > 0)) {
            if (T[kpt56] && ((T_max[kpt56] - T_timer[kpt56])/10 > TEG[fc6])) {
              TEG[fc6] = (T_max[kpt56] - T_timer[kpt56])/10;
            }
          }
        }
        
        if ((fc7 != NG) && (fc8 != NG)) {
          if (kpt78 != NG) naloop78 = T_max[kpt78]/10;
          if (G[fc7] && (TEG[fc7] + naloop78 > TEG[fc8])) TEG[fc8] = TEG[fc7] + naloop78;
          if (!G[fc7] && (naloop78 > 0)) {
            if (T[kpt78] && ((T_max[kpt78] - T_timer[kpt78])/10 > TEG[fc8])) {
              TEG[fc8] = (T_max[kpt78] - T_timer[kpt78])/10;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer BAR[] tbv langzaam verkeeroversteken                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bar_vtg_sgd(void)           /* Fik120505                                                        */
  {
    mulv i,j,k,x,i_fts;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    mulv kpt1,kpt3,kpt56,kpt78;
    mulv naloop13,naloop31;
    mulv naloop12,naloop34;
    mulv naloop56,naloop78;
    bool door_BAR,voorw;
    bool kapmvg;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fc1  = kp_vtg_fts[i][1];
        fc2  = kp_vtg_fts[i][2];
        fc3  = kp_vtg_fts[i][3];
        fc4  = kp_vtg_fts[i][4];
        fc5  = kp_vtg_fts[i][5];
        fc6  = kp_vtg_fts[i][6];
        fc7  = kp_vtg_fts[i][7];
        fc8  = kp_vtg_fts[i][8];
        kpt1 = kp_vtg_fts[i][11];
        kpt3 = kp_vtg_fts[i][12];
        kpt56= kp_vtg_fts[i][13];
        kpt78= kp_vtg_fts[i][14];
        
        naloop13 = naloop31 = 0;
        naloop12 = naloop34 = naloop56 = naloop78 = 0;
        
        for (j=1; j<=8; j++) {          /* altijd BAR[] indien geel of garantie rood                        */
          if (kp_vtg_fts[i][j] != NG) {
            if (GL[kp_vtg_fts[i][j]] || R[kp_vtg_fts[i][j]] && TRG[kp_vtg_fts[i][j]]) {
              BAR[kp_vtg_fts[i][j]] |= TRUE;
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (kpt1 != NG) naloop13 = T_max[kpt1]/10;
            if (!G[fc1] && G[fc3] && !BAR[fc1]) {
              if (TTK[fc3] < (TTR[fc1] + naloop13)) BAR[fc1] = TRUE;
            }
            if (kpt3 != NG) naloop31 = T_max[kpt3]/10;
            if (!G[fc3] && G[fc1] && !BAR[fc3]) {
              if (TTK[fc1] < (TTR[fc3] + naloop31)) BAR[fc3] = TRUE;
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
          /* werkwijze binnenste-buiten of getrapte oversteek                                               */
          if ((fc1 != NG) && (fc2 != NG)) { /* in ALTM[fc4] is nalooptijd vanaf fc3 al verwerkt             */
            if (kpt1 != NG) naloop12 = T_max[kpt1]/10; /* + 1 ivm mogelijke afrondingsverschillen           */
            if (!G[fc1] && G[fc2] && !BAR[fc1]) {
              if (TTK[fc2] <= (TTR[fc1] + ALTM[fc2] + 1)) BAR[fc1] = TRUE;
            }
            if (MG[fc1] && G[fc2] && !BAR[fc1]) {
              if (TTK[fc2] <= ((TGL_max[fc1]/10) + naloop12 + 1)) BAR[fc1] = TRUE;
            }
          }
          if ((fc3 != NG) && (fc4 != NG)) { /* in ALTM[fc2] is nalooptijd vanaf fc1 al verwerkt             */
            if (kpt3 != NG) naloop34 = T_max[kpt3]/10; /* + 1 ivm mogelijke afrondingsverschillen           */
            if (!G[fc3] && G[fc4] && !BAR[fc3]) {
              if (TTK[fc4] <= (TTR[fc3] + ALTM[fc4] + 1)) BAR[fc3] = TRUE;
            }
            if (MG[fc3] && G[fc4] && !BAR[fc3]) {
              if (TTK[fc4] <= ((TGL_max[fc3]/10) + naloop34 + 1)) BAR[fc3] = TRUE;
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (kpt1 != NG) naloop13 = T_max[kpt1]/10;
            if (!G[fc2] && G[fc3] && !BAR[fc2]) {
              if (TTK[fc3] < (TTR[fc2] + naloop13)) BAR[fc2] = TRUE;
            }
            if (kpt3 != NG) naloop31 = T_max[kpt3]/10;
            if (!G[fc3] && G[fc1] && !BAR[fc3]) {
              if (TTK[fc1] < (TTR[fc3] + naloop31)) BAR[fc3] = TRUE;
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) { /* in ALTM[fc6] is nalooptijd vanaf fc5 al verwerkt               */
          if (kpt56 != NG) naloop56 = T_max[kpt56]/10; /* + 1 ivm mogelijke afrondingsverschillen           */
          if (!G[fc5] && G[fc6] && !BAR[fc5]) {
            if (TTK[fc6] <= (TTR[fc5] + ALTM[fc6] + 1)) BAR[fc5] = TRUE;
          }
          if (G[fc6] && !BAR[fc5]) {
            if (MG[fc5] && (TTK[fc6] <= ((TGL_max[fc5]/10) + naloop56 + 1))) BAR[fc5] = TRUE;
            for (i_fts=0; i_fts<FC_MAX; i_fts++) {
              if ((TI_max(fc5,i_fts) >= 0) || (TI_max(fc5,i_fts) == GK) || TMPc[fc5][i_fts]) {
                j = i_fts;
                if (R[j] && !TRG[j] && A[j]) {
                  kapmvg = TRUE;
                  for (x=0; x<FC_MAX; x++) {
                    if ((TI_max(j,x) >= 0) || (TI_max(j,x) == GK) || TMPc[j][x]) {
                      k =  x;
                      if (B[k] || RA[k] || SG[k] || FG[k] || (WG[k] || VG[k]) && (k != fc6)) kapmvg = FALSE;
                    }
                    if (kapmvg && (R[fc5] || MG[fc5])) BAR[fc5] = TRUE;
                  }
                }
              }
            }
          }
        }
        
        if ((fc7 != NG) && (fc8 != NG)) { /* in ALTM[fc8] is nalooptijd vanaf fc7 al verwerkt               */
          if (kpt78 != NG) naloop78 = T_max[kpt78]/10; /* + 1 ivm mogelijke afrondingsverschillen           */
          if (!G[fc7] && G[fc8] && !BAR[fc7]) {
            if (TTK[fc8] <= (TTR[fc7] + ALTM[fc8] + 1)) BAR[fc7] = TRUE;
          }
          if (G[fc8] && !BAR[fc7]) {
            if (MG[fc7] && (TTK[fc8] <= ((TGL_max[fc7]/10) + naloop78 + 1))) BAR[fc7] = TRUE;
            for (i_fts=0; i_fts<FC_MAX; i_fts++) {
              if ((TI_max(fc7,i_fts) >= 0) || (TI_max(fc7,i_fts) == GK) || TMPc[fc7][i_fts]) {
                j = i_fts;
                if (R[j] && !TRG[j] && A[j]) {
                  kapmvg = TRUE;
                  for (x=0; x<FC_MAX; x++) {
                    if ((TI_max(j,x) >= 0) || (TI_max(j,x) == GK) || TMPc[j][x]) {
                      k = x;
                      if (B[k] || RA[k] || SG[k] || FG[k] || (WG[k] || VG[k]) && (k != fc8)) kapmvg = FALSE;
                    }
                    if (kapmvg && (R[fc7] || MG[fc7])) BAR[fc7] = TRUE;
                  }
                }
              }
            }
          }
        }
        
        for (j=1; j<=8; j++) {          /* correctie BAR ivm aanhouden mvg gekoppelde richtingen            */
          if (kp_vtg_fts[i][j] != NG) {
            if (!G[kp_vtg_fts[i][j]]) {
              for (x=1; x<=8; x++) {
                if (kp_vtg_fts[i][x] != NG) {
                  if (x != j) {
                    if (G[kp_vtg_fts[i][x]] && (TTR[kp_vtg_fts[i][j]] >= TTK[kp_vtg_fts[i][x]])) {
                      BAR[kp_vtg_fts[i][j]] |= TRUE;
                    }
                  }
                }
              }
            }
          }
        }
        
        door_BAR = FALSE;
        if (fc1 != NG) if (GL[fc1] || TRG[fc1] || R[fc1] && A[fc1] && BAR[fc1]) door_BAR = TRUE;
        if (fc2 != NG) if (GL[fc2] || TRG[fc2] || R[fc2] && A[fc2] && BAR[fc2]) door_BAR = TRUE;
        if (fc3 != NG) if (GL[fc3] || TRG[fc3] || R[fc3] && A[fc3] && BAR[fc3]) door_BAR = TRUE;
        if (fc4 != NG) if (GL[fc4] || TRG[fc4] || R[fc4] && A[fc4] && BAR[fc4]) door_BAR = TRUE;
        if (fc5 != NG) if (GL[fc5] || TRG[fc5] || R[fc5] && A[fc5] && BAR[fc5]) door_BAR = TRUE;
        if (fc6 != NG) if (GL[fc6] || TRG[fc6] || R[fc6] && A[fc6] && BAR[fc6]) door_BAR = TRUE;
        if (fc7 != NG) if (GL[fc7] || TRG[fc7] || R[fc7] && A[fc7] && BAR[fc7]) door_BAR = TRUE;
        if (fc8 != NG) if (GL[fc8] || TRG[fc8] || R[fc8] && A[fc8] && BAR[fc8]) door_BAR = TRUE;
        
        if (door_BAR) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (!G[kp_vtg_fts[i][j]]) BAR[kp_vtg_fts[i][j]] |= TRUE;
            }
          }
        }
        
        voorw = FALSE;                  /* nooit retourrood bij wachttijdvoorspeller                        */
        if (fc1 != NG) voorw |= _GRN[fc1];
        if (fc2 != NG) voorw |= _GRN[fc2];
        if (fc3 != NG) voorw |= _GRN[fc3];
        if (fc4 != NG) voorw |= _GRN[fc4];
        if (fc5 != NG) voorw |= _GRN[fc5];
        if (fc6 != NG) voorw |= _GRN[fc6];
        if (fc7 != NG) voorw |= _GRN[fc7];
        if (fc8 != NG) voorw |= _GRN[fc8];
        
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (!G[kp_vtg_fts[i][j]]) BAR[kp_vtg_fts[i][j]] = FALSE;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure afwikkeling koppelingen langzaam verkeer                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_afw_lvk_sgd(void)           /* Fik120105                                                        */
  {
    mulv i,j;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    mulv kpa1,kpa3;
    mulv kpt1,kpt3,kpt56,kpt78;
    bool naloop13,naloop31;
    bool naloop12,naloop34;
    bool naloop56,naloop78;
    bool voorw,xu;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      if (kp_vtg_fts[i][0] > 0) {
        fc1  = kp_vtg_fts[i][1];
        fc2  = kp_vtg_fts[i][2];
        fc3  = kp_vtg_fts[i][3];
        fc4  = kp_vtg_fts[i][4];
        fc5  = kp_vtg_fts[i][5];
        fc6  = kp_vtg_fts[i][6];
        fc7  = kp_vtg_fts[i][7];
        fc8  = kp_vtg_fts[i][8];
        kpa1 = kp_vtg_fts[i][9];
        kpa3 = kp_vtg_fts[i][10];
        kpt1 = kp_vtg_fts[i][11];
        kpt3 = kp_vtg_fts[i][12];
        kpt56= kp_vtg_fts[i][13];
        kpt78= kp_vtg_fts[i][14];
        
        if (kpt1  != NG) RT[kpt1] = FALSE;
        if (kpt3  != NG) RT[kpt3] = FALSE;
        if (kpt56 != NG) RT[kpt56]= FALSE;
        if (kpt78 != NG) RT[kpt78]= FALSE;
        
        naloop13 = naloop31 = FALSE;
        naloop12 = naloop34 = naloop56 = naloop78 = FALSE;
        
        /* Correctie BL[] indien RA[] en wachttijdvoorspeller                                               */
        /* --------------------------------------------------                                               */
        voorw = FALSE;
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (_GRN[kp_vtg_fts[i][j]]) voorw = TRUE;
          }
        }
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              BL[kp_vtg_fts[i][j]] = FALSE;
              if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]]) _GRN[kp_vtg_fts[i][j]] = TRUE;
            }
          }
        }
        
        voorw = FALSE;                  /* altijd gelijktijdig starten wachtstand                           */
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (GL[kp_vtg_fts[i][j]] || TRG[kp_vtg_fts[i][j]] || K[kp_vtg_fts[i][j]]) voorw |= TRUE;
          }
        }
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              BAR[kp_vtg_fts[i][j]] |= !G[kp_vtg_fts[i][j]];
            }
          }
        }
        
        proc_mav_vtg();                 /* set zonodig mee aanvragen                                        */
        
        voorw = FALSE;
        if (fc1 != NG) voorw |= _GRN[fc1];
        if (fc2 != NG) voorw |= _GRN[fc2];
        if (fc3 != NG) voorw |= _GRN[fc3];
        if (fc4 != NG) voorw |= _GRN[fc4];
        if (fc5 != NG) voorw |= _GRN[fc5];
        if (fc6 != NG) voorw |= _GRN[fc6];
        if (fc7 != NG) voorw |= _GRN[fc7];
        if (fc8 != NG) voorw |= _GRN[fc8];
        
        if (voorw) {                    /* nooit retourrood bij wachttijdvoorspeller                        */
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (!G[kp_vtg_fts[i][j]]) BAR[kp_vtg_fts[i][j]] = FALSE;
            }
          }
        }
        /* herstart koppeltijden voetgangers                                                                */
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (kpt1 != NG) {
              if (kpa1 != NG) {         /* geen drk maar wel koppeltijd, dan altijd koppeling               */
                RT[kpt1] = SG[fc1] && MM[kpa1];
              } else {
                RT[kpt1] = SG[fc1];
              }
            }
            if (kpt3 != NG) {
              if (kpa3 != NG) {         /* geen drk maar wel koppeltijd, dan altijd koppeling               */
                RT[kpt3] = SG[fc3] && MM[kpa3];
              } else {
                RT[kpt3] = SG[fc3];
              }
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
                                        /* werkwijze binnenste-buiten of getrapte oversteek                 */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt1 != NG) RT[kpt1] = G[fc1];
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (kpt3 != NG) RT[kpt3] = G[fc3];
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (kpt1 != NG) {
              if (kpa1 != NG) {         /* geen drk maar wel koppeltijd, dan altijd koppeling               */
                RT[kpt1] = SG[fc2] && MM[kpa1];
              } else {
                RT[kpt1] = SG[fc2];
              }
            }
            if (kpt3 != NG) {
              if (kpa3 != NG) {         /* geen drk maar wel koppeltijd, dan altijd koppeling               */
                RT[kpt3] = SG[fc3] && MM[kpa3];
              } else {
                RT[kpt3] = SG[fc3];
              }
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) { /* herstart koppeltijden fietsers                                 */
          if (kpt56 != NG) RT[kpt56] = G[fc5];
        }
        if ((fc7 != NG) && (fc8 != NG)) {
          if (kpt78 != NG) RT[kpt78] = G[fc7];
        }
        /* vasthouden groen nalooprichting voetgangers                                                      */
        if (kp_vtg_fts[i][0] == 2) {    /* werkwijze gescheiden oversteek                                   */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (kpt1 != NG) naloop13 = RT[kpt1] || T[kpt1];
            else            naloop13 = FALSE;
            if (B[fc1] || RA[fc1] || naloop13) {
              YW[fc3] |= BIT3;
              RW[fc3] |= BIT3;
              FM[fc3]  = FALSE;
              if (naloop13) RW[fc3] |= BIT4; /* tbv hulpdienstingreep                                       */
            }
            if (kpt3 != NG) naloop31 = RT[kpt3] || T[kpt3];
            else            naloop31 = FALSE;
            if (B[fc3] || RA[fc3] || naloop31) {
              YW[fc1] |= BIT3;
              RW[fc1] |= BIT3;
              FM[fc1]  = FALSE;
              if (naloop31) RW[fc1] |= BIT4; /* tbv hulpdienstingreep                                       */
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
                                        /* werkwijze binnenste-buiten of getrapte oversteek                 */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt1 != NG) naloop12 = RT[kpt1] || T[kpt1];
            else            naloop12 = (G[fc1] != 0);
            if (B[fc1] || RA[fc1] || naloop12) {
              YW[fc2] |= BIT3;
              RW[fc2] |= BIT3;
              FM[fc2]  = FALSE;
              if (naloop12) {
                RW[fc2]   |= BIT4;      /* tbv hulpdienstingreep                                            */
                HKII[fc1] |= BIT3;
                HKI[fc1]  |= BIT3; if (SGD[fc2]&BIT4) HKI[fc1] |= BIT2;
              }
            }
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (kpt3 != NG) naloop34 = RT[kpt3] || T[kpt3];
            else            naloop34 = (G[fc3] != 0);
            if (B[fc3] || RA[fc3] || naloop34) {
              YW[fc4] |= BIT3;
              RW[fc4] |= BIT3;
              FM[fc4]  = FALSE;
              if (naloop34) {
                RW[fc4]   |= BIT4;      /* tbv hulpdienstingreep                                            */
                HKII[fc3] |= BIT3;
                HKI[fc3]  |= BIT3; if (SGD[fc4]&BIT4) HKI[fc3] |= BIT2;
              }
            }
          }
        }
        
        if (kp_vtg_fts[i][0] == 5) {    /* werkwijze half getrapte oversteek                                */
          if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
            if (kpt1 != NG) naloop13 = RT[kpt1] || T[kpt1];
            else            naloop13 = FALSE;
            if (B[fc2] || RA[fc2] || naloop13) {
              YW[fc3] |= BIT3;
              RW[fc3] |= BIT3;
              FM[fc3]  = FALSE;
              if (naloop13) RW[fc3] |= BIT4; /* tbv hulpdienstingreep                                       */
            }
            if (kpt3 != NG) naloop31 = RT[kpt3] || T[kpt3];
            else            naloop31 = FALSE;
            if (B[fc3] || RA[fc3] || naloop31) {
              YW[fc1] |= BIT3;
              RW[fc1] |= BIT3;
              FM[fc1]  = FALSE;
              if (naloop31) RW[fc1] |= BIT4; /* tbv hulpdienstingreep                                       */
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) { /* vasthouden groen nalooprichting fietsers                       */
          if (kpt56 != NG) naloop56 = RT[kpt56] || T[kpt56];
          else             naloop56 = (G[fc5] != 0);
          if (B[fc5] || RA[fc5] || naloop56) {
            YW[fc6] |= BIT3;
            RW[fc6] |= BIT3;
            FM[fc6]  = FALSE;
            if (naloop56) {
              RW[fc6]   |= BIT4;        /* tbv hulpdienstingreep                                            */
              HKII[fc5] |= BIT3;
              HKI[fc5]  |= BIT3; if (SGD[fc6]&BIT4) HKI[fc5] |= BIT2;
            }
          }
        }
        
        if ((fc7 != NG) && (fc8 != NG)) { /* vasthouden groen nalooprichting fietsers                       */
          if (kpt78 != NG) naloop78 = RT[kpt78] || T[kpt78];
          else             naloop78 = (G[fc7] != 0);
          if (B[fc7] || RA[fc7] || naloop78) {
            YW[fc8] |= BIT3;
            RW[fc8] |= BIT3;
            FM[fc8]  = FALSE;
            if (naloop78) {
              RW[fc8]   |= BIT4;        /* tbv hulpdienstingreep                                            */
              HKII[fc7] |= BIT3;
              HKI[fc7]  |= BIT3; if (SGD[fc8]&BIT4) HKI[fc7] |= BIT2;
            }
          }
        }
        
        voorw = FALSE;                  /* vasthouden meeverlenggroen                                       */
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if ((RV[kp_vtg_fts[i][j]] && B[kp_vtg_fts[i][j]] || RA[kp_vtg_fts[i][j]]) &&
                !RR[kp_vtg_fts[i][j]]) {
              voorw |= TRUE;
            }
          }
        }
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (G[kp_vtg_fts[i][j]]) YM[kp_vtg_fts[i][j]] |= BIT3;
            }
          }
        }
        
        voorw = FALSE;                  /* beurt tbv gelijke start                                          */
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if ((RV[kp_vtg_fts[i][j]] && B[kp_vtg_fts[i][j]] || RA[kp_vtg_fts[i][j]]) &&
                A[kp_vtg_fts[i][j]] && !RR[kp_vtg_fts[i][j]])
            voorw |= TRUE;
          }
        }
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              if (R[kp_vtg_fts[i][j]] && !TRG[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]]) {
                B[kp_vtg_fts[i][j]] |= BIT3;
                RR[kp_vtg_fts[i][j]] = FALSE;
              }
            }
          }
        }
        
        voorw = FALSE;                  /* uitstel tbv gelijke start                                        */
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            xu = TRUE;
            if (RA[kp_vtg_fts[i][j]] && !K[kp_vtg_fts[i][j]] && !X[kp_vtg_fts[i][j]] &&
                !RR[kp_vtg_fts[i][j]] || G[kp_vtg_fts[i][j]] || RV[kp_vtg_fts[i][j]] &&
                !TRG[kp_vtg_fts[i][j]] && !A[kp_vtg_fts[i][j]]) {
              xu = FALSE;
            }
            if (xu) voorw = TRUE;
          }
        }
        if (voorw) {
          for (j=1; j<=8; j++) {
            if (kp_vtg_fts[i][j] != NG) {
              X[kp_vtg_fts[i][j]] |= BIT3;
            }
          }
        }
        /* realisatie nalooprichting voetgangers                                                            */
        if ((kp_vtg_fts[i][0] == 2) || (kp_vtg_fts[i][0] == 5)) {
                                        /* werkwijze gescheiden of half getrapte oversteek                  */
          if ((fc1 != NG) && (fc3 != NG)) {
            if (G[fc1] && (RW[fc1]&BIT3) && (SGD[fc3]&BIT2) &&
                ((SGD[fc1]&BIT0) || (SGD[fc1]&BIT3))) {
              SGD[fc1] |= BIT2;
            }
            if (G[fc3] && (RW[fc3]&BIT3) && (SGD[fc1]&BIT2) &&
                ((SGD[fc3]&BIT0) || (SGD[fc3]&BIT3))) {
              SGD[fc3] |= BIT2;
            }
          }
        }
        
        if ((kp_vtg_fts[i][0] == 3) || (kp_vtg_fts[i][0] == 4)) {
                                        /* werkwijze binnenste-buiten of getrapte oversteek                 */
          if ((fc1 != NG) && (fc2 != NG)) {
            if (G[fc2] && (RW[fc2]&BIT3) && (SGD[fc1]&BIT2) &&
                ((SGD[fc2]&BIT0) || (SGD[fc2]&BIT3))) {
              SGD[fc2] |= BIT2;
            }
          }
          if ((fc3 != NG) && (fc4 != NG)) {
            if (G[fc4] && (RW[fc4]&BIT3) && (SGD[fc3]&BIT2) &&
                ((SGD[fc4]&BIT0) || (SGD[fc4]&BIT3))) {
              SGD[fc4] |= BIT2;
            }
          }
        }
        
        if ((fc5 != NG) && (fc6 != NG)) { /* realisatie nalooprichting fietsers                             */
          if (G[fc6] && (RW[fc6]&BIT3) && (SGD[fc5]&BIT2) &&
              ((SGD[fc6]&BIT0) || (SGD[fc6]&BIT3))) {
            SGD[fc6] |= BIT2;
          }
        }
        
        if ((fc7 != NG) && (fc8 != NG)) {
          if (G[fc8] && (RW[fc8]&BIT3) && (SGD[fc7]&BIT2) &&
              ((SGD[fc8]&BIT0) || (SGD[fc8]&BIT3))) {
            SGD[fc8] |= BIT2;
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedures waitsignalen fietsers                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_wait_fts(                   /* Fik070314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count ws,                           /* waitsignaal                                                      */
    count dk)                           /* drukknop                                                         */
  {
    if (BEDRYF) {
      CIF_GUS[ws] = R[fc] && (A[fc] || MM[dk]) || GL[fc] && !EG[fc] && MM[dk];
    } else {
      CIF_GUS[ws] = FALSE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedures waitsignalen voetgangers                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_wait_vtg(                   /* Fik070314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count ws,                           /* waitsignaal                                                      */
    count dk)                           /* drukknop                                                         */
  {
    if (BEDRYF) {
      CIF_GUS[ws] = !G[fc] && !EG[fc] && MM[dk];
    } else {
      CIF_GUS[ws] = FALSE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Bepaal _TDH[]                                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bepaal_TDH(void)            /* Fik120520                                                        */
  {
    count i;
    
    for (i=0; i<DP_MAX; i++) {
      if (D[i]) _TDH[i] = 0;
      else _TDH[i] += TE;               /* onthoud duur onbezet detector                                    */
      if (_TDH[i] > 255) _TDH[i] = 255; /* ... maximum waarde 255                                           */
      
      if (SD[i]) {
        _VT4_old[i] = _VT4[i];
        _VT4[i] = 0;
      }
      else _VT4[i] += TE;               /* onthoud volgtijd detector                                        */
      if (_VT4[i] > 255) _VT4[i] = 255; /* ... maximum waarde 255                                           */
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure US-signalen tbv OV ingreep                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_us_ov(                      /* Fik180901 - op aangeven van Marc Scherjon                        */
    count fc,                           /* fasecyclus                                                       */
    count priv,                         /* PRM bus te vroeg                                                 */
    count prit,                         /* PRM bus op tijd                                                  */
    count pril,                         /* PRM bus te laat                                                  */
    count stp1,                         /* stiptheidscode 1e bus                                            */
    count stp2,                         /* stiptheidscode 2e bus                                            */
    count stp3,                         /* stiptheidscode 3e bus                                            */
    count us_ov,                        /* US bus aanwezig                                                  */
    count ovv,                          /* US bus te vroeg aanwezig                                         */
    count ovt,                          /* US bus op tijd  aanwezig                                         */
    count ovl,                          /* US bus te laat  aanwezig                                         */
    count povv,                         /* US bus te vroeg prioriteit                                       */
    count povt,                         /* US bus op tijd  prioriteit                                       */
    count povl)                         /* US bus te laat  prioriteit                                       */
  {
    bool _tevroeg = FALSE;
    bool _optijd  = FALSE;
    bool _telaat  = FALSE;
    
    if (BEDRYF) {
      if ((priv != NG) && (PRM[priv] > 0)) {
        if ((MM[stp1] == 1) || (MM[stp2] == 1) || (MM[stp3] == 1)) _tevroeg = TRUE;
      }
      if ((prit != NG) && (PRM[prit] > 0)) {
        if ((MM[stp1] == 2) || (MM[stp2] == 2) || (MM[stp3] == 2)) _optijd  = TRUE;
      }
      if ((pril != NG) && (PRM[pril] > 0)) {
        if ((MM[stp1] == 3) || (MM[stp2] == 3) || (MM[stp3] == 3)) _telaat  = TRUE;
      }
      
      if (ovv != NG) {
        CIF_GUS[ovv] = _tevroeg;
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[ovv] |= OV_aanw[fc] && (OV_stipt[fc] && (OV_stipt[fc] < -(T_max[tovv]/10)));
#endif
      }
      if (ovt != NG) {
        CIF_GUS[ovt] = _optijd;
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[ovt] |= OV_aanw[fc] && (OV_stipt[fc] >= -(T_max[tovv]/10)) && (OV_stipt[fc] <= (T_max[tovt]/10));
#endif
      }
      if (ovl != NG) {
        CIF_GUS[ovl] = _telaat;
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[ovl] |= OV_aanw[fc] && (OV_stipt[fc] > (T_max[tovt]/10));
#endif
      }
      
      if (povv != NG) {
        CIF_GUS[povv] = (AKTPRG == 0) && _tevroeg && G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5) && (MM[stp1] == 1);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[povv]|= (AKTPRG == 0) &&             G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5) && 
                        OV_aanw[fc] && (OV_stipt[fc] < -(T_max[tovv]/10)); 
#endif
      }
      if (povt != NG) {
        CIF_GUS[povt] = (AKTPRG == 0) && _optijd  && G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5) && (MM[stp1] == 2);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[povt]|= (AKTPRG == 0) &&             G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5) && 
                        OV_aanw[fc] && (OV_stipt[fc] >= -(T_max[tovv]/10)) && (OV_stipt[fc] <= (T_max[tovt]/10)); 
#endif
      }
      if (povl != NG) {
        CIF_GUS[povl] = (AKTPRG == 0) && _telaat  && G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5) && (MM[stp1] == 3);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[povl]|= (AKTPRG == 0) &&             G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5) && 
                        OV_aanw[fc] && (OV_stipt[fc] > (T_max[tovt]/10)); 
#endif
      }
      
      if (us_ov != NG) {
        CIF_GUS[us_ov]  = KNIP && ((MM[stp1] > 0) || (MM[stp2] > 0) || (MM[stp3] > 0));
        CIF_GUS[us_ov] |= (AKTPRG == 0) && G[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[us_ov] |= KNIP && (OV_aanw[fc] > 0);
#endif
      }
      
#ifdef CRSV2CCOL
      if (povv != NG) {
        CIF_GUS[povv] |= (AKTPRG > 0) && _tevroeg && G[fc] && (TTPR[fc] == 0) && (MM[stp1]==1);
      }
      if (povt != NG) {
        CIF_GUS[povt] |= (AKTPRG > 0) && _optijd  && G[fc] && (TTPR[fc] == 0) && (MM[stp1]==2);
      }
      if (povl != NG) {
        CIF_GUS[povl] |= (AKTPRG > 0) && _telaat  && G[fc] && (TTPR[fc] == 0) && (MM[stp1]==3);
      }
      
      if (us_ov != NG) {
        CIF_GUS[us_ov] |= (AKTPRG > 0) && G[fc] && (TTPR[fc] == 0);
      }
#endif
      
#ifdef _VAS_OV
      
#ifndef CRSV2CCOL
      if (povv != NG) {
        CIF_GUS[povv] |= (AKTPRG > 0) && _tevroeg && G[fc] && (HVAS[fc]&PRI) && (MM[stp1] == 1);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[povv] |= (AKTPRG > 0) &&             G[fc] && (HVAS[fc]&PRI) && OV_aanw[fc] && 
                         (OV_stipt[fc] && (OV_stipt[fc] < -(T_max[tovv]/10))); 
#endif
      }
      if (povt != NG) {
        CIF_GUS[povt] |= (AKTPRG > 0) && _optijd  && G[fc] && (HVAS[fc]&PRI) && (MM[stp1] == 2);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[povt] |= (AKTPRG > 0) &&             G[fc] && (HVAS[fc]&PRI) && OV_aanw[fc] && 
                         (OV_stipt[fc] >= -(T_max[tovv]/10)) && (OV_stipt[fc] <= (T_max[tovt]/10)); 
#endif
      }
      if (povl != NG) {
        CIF_GUS[povl] |= (AKTPRG > 0) && _telaat  && G[fc] && (HVAS[fc]&PRI) && (MM[stp1] == 3);
#ifdef C_ITS_FUNCTIONALITEIT
        CIF_GUS[povl] |= (AKTPRG > 0) &&             G[fc] && (HVAS[fc]&PRI) && OV_aanw[fc] && 
                         (OV_stipt[fc] > (T_max[tovt]/10)); 
#endif
      }
      
      if (us_ov != NG) {
        CIF_GUS[us_ov] |= (AKTPRG > 0) && G[fc] && (HVAS[fc]&PRI);
      }
#endif
      
#endif                                  /* ifdef -> _VAS_OV                                                 */
      
    } else {
      
      if (us_ov != NG) CIF_GUS[us_ov]= FALSE;
      
      if ( ovv  != NG) CIF_GUS[ovv]  = FALSE;
      if ( ovt  != NG) CIF_GUS[ovt]  = FALSE;
      if ( ovl  != NG) CIF_GUS[ovl]  = FALSE;
      
      if (povv  != NG) CIF_GUS[povv] = FALSE;
      if (povt  != NG) CIF_GUS[povt] = FALSE;
      if (povl  != NG) CIF_GUS[povl] = FALSE;
      
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Functie bepaal filemeetpunt                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void file_meting(                     /* Fik100403                                                        */
    count file,                         /* file-melding                                                     */
    count afval,                        /* hulpfunctie  !file                                               */
    count fdet,                         /* file-detector                                                    */
    count dbez,                         /* bezettijd ->  file                                               */
    count drij,                         /* rijtijd   -> !file                                               */
    count dafv,                         /* afvalvertraging                                                  */
    count tab)                          /* afvalbewaking TYD                                                */
  {
    RT[dbez] = RT[drij] = RT[dafv] = AT[dafv] = FALSE;
    
    if (SD[fdet]) RT[dbez] = RT[drij] = TRUE;
    
    if (D[fdet] && !RT[dbez] && !T[dbez]) {
      MM[file]  = TRUE;
      MM[afval] = FALSE;
      AT[dafv]  = TRUE;
    }
    if (ED[fdet] && (RT[drij] || T[drij]) && MM[file] && !MM[afval]) {
      MM[afval] = TRUE;
      RT[dafv]  = TRUE;
    }
    if (D[fdet] && !RT[drij] && !T[drij] && MM[file]) {
      MM[afval] = FALSE;
      AT[dafv]  = TRUE;
    }
    if (!RT[dafv] && !T[dafv] && !D[fdet] && MM[file] && MM[afval]) {
      MM[file]  = FALSE;
      MM[afval] = FALSE;
    }
    RT[tab] = D[fdet];
    if (!RT[tab] && !T[tab] || DF[fdet]) {
      MM[file]  = FALSE;
      MM[afval] = FALSE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal FILE voor de stopstreep                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_FILE_voorss(                /* Fik100719                                                        */
    count de1,                          /* 1e FILE detector                                                 */
    count mm1,                          /* 1e FILE detector                                                 */
    count sch1,                         /* 1e FILE de aktief                                                */
    count de2,                          /* 2e FILE detector                                                 */
    count mm2,                          /* 2e FILE detector                                                 */
    count sch2,                         /* 2e FILE de aktief                                                */
    count de3,                          /* 3e FILE detector                                                 */
    count mm3,                          /* 3e FILE detector                                                 */
    count sch3,                         /* 3e FILE de aktief                                                */
    count de4,                          /* 4e FILE detector                                                 */
    count mm4,                          /* 4e FILE detector                                                 */
    count sch4,                         /* 4e FILE de aktief                                                */
    count mmfil,                        /* FILE ingr. aktief                                                */
    count mmprg,                        /* FILE prog. aktief                                                */
    count tfiprg)                       /* duur file programma                                              */
  {
    bool fil1 = FALSE;
    bool fil2 = FALSE;
    bool fil3 = FALSE;
    bool fil4 = FALSE;
    
    if (de1 != NG) {                    /* bepaal FILE aktief                                               */
      if (mm1 != NG) {
        if ( MM[mm1] &&  SCH[sch1]) fil1 = TRUE;
        if (!MM[mm1] || !SCH[sch1]) fil1 = FALSE;
      } else {
        if (  DB[de1] && !DF[de1] &&  SCH[sch1] || (MM[mmfil]&BIT0)) fil1 = TRUE;
        if (!TDH[de1] ||  DF[de1] || !SCH[sch1])                     fil1 = FALSE;
      }
    }
    if (de2 != NG) {
      if (mm2 != NG) {
        if ( MM[mm2] &&  SCH[sch2]) fil2 = TRUE;
        if (!MM[mm2] || !SCH[sch2]) fil2 = FALSE;
      } else {
        if (  DB[de2] && !DF[de2] &&  SCH[sch2] || (MM[mmfil]&BIT1)) fil2 = TRUE;
        if (!TDH[de2] ||  DF[de2] || !SCH[sch2])                     fil2 = FALSE;
      }
    }
    if (de3 != NG) {
      if (mm3 != NG) {
        if ( MM[mm3] &&  SCH[sch3]) fil3 = TRUE;
        if (!MM[mm3] || !SCH[sch3]) fil3 = FALSE;
      } else {
        if (  DB[de3] && !DF[de3] &&  SCH[sch3] || (MM[mmfil]&BIT2)) fil3 = TRUE;
        if (!TDH[de3] ||  DF[de3] || !SCH[sch3])                     fil3 = FALSE;
      }
    }
    if (de4 != NG) {
      if (mm4 != NG) {
        if ( MM[mm4] &&  SCH[sch4]) fil4 = TRUE;
        if (!MM[mm4] || !SCH[sch4]) fil4 = FALSE;
      } else {
        if (  DB[de4] && !DF[de4] &&  SCH[sch4] || (MM[mmfil]&BIT3)) fil4 = TRUE;
        if (!TDH[de4] ||  DF[de4] || !SCH[sch4])                     fil4 = FALSE;
      }
    }
    
    MM[mmfil] = 0;                      /* sommeer afzonderlijke file meldingen                             */
    if (fil1 || fil2 || fil3 || fil4) {
      if (fil1) MM[mmfil] |= BIT0;
      if (fil2) MM[mmfil] |= BIT1;
      if (fil3) MM[mmfil] |= BIT2;
      if (fil4) MM[mmfil] |= BIT3;
    }
    
    RT[tfiprg] = (MM[mmfil] > 0);       /* bepaal file programma aktief                                     */
    MM[mmprg]  = RT[tfiprg] || T[tfiprg];
    FILE_VOOR_SS |= MM[mmprg];
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal FILE voorbij stopstreep                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_FILE_nass(                  /* Fik100403                                                        */
    count de1,                          /* 1e FILE detector                                                 */
    count mm1,                          /* 1e FILE detector                                                 */
    count sch1,                         /* 1e FILE de aktief                                                */
    count de2,                          /* 2e FILE detector                                                 */
    count mm2,                          /* 2e FILE detector                                                 */
    count sch2,                         /* 2e FILE de aktief                                                */
    count de3,                          /* 3e FILE detector                                                 */
    count mm3,                          /* 3e FILE detector                                                 */
    count sch3,                         /* 3e FILE de aktief                                                */
    count de4,                          /* 4e FILE detector                                                 */
    count mm4,                          /* 4e FILE detector                                                 */
    count sch4,                         /* 4e FILE de aktief                                                */
    count mmfil,                        /* FILE ingr. aktief                                                */
    count mmnafil,                      /* NAFILE     aktief                                                */
    count mmnafc1,                      /* NAFILE BTvolgorde                                                */
    count mmnafc2,                      /* NAFILE BTvolgorde                                                */
    count mmnafc3,                      /* NAFILE BTvolgorde                                                */
    count prmna,                        /* keuze NAFILE                                                     */
    count fc1,                          /* 1e fasecyclus                                                    */
    count prm1,                         /* ingreep fc1                                                      */
    count dos1,                         /* doseermaximum fc1                                                */
    count blk1,                         /* duur blokkeer fc1                                                */
    count fc2,                          /* 1e fasecyclus                                                    */
    count prm2,                         /* ingreep fc2                                                      */
    count dos2,                         /* doseermaximum fc2                                                */
    count blk2,                         /* duur blokkeer fc2                                                */
    count fc3,                          /* 1e fasecyclus                                                    */
    count prm3,                         /* ingreep fc3                                                      */
    count dos3,                         /* doseermaximum fc3                                                */
    count blk3)                         /* duur blokkeer fc3                                                */
  {
    bool fil1 = FALSE;
    bool fil2 = FALSE;
    bool fil3 = FALSE;
    bool fil4 = FALSE;
    bool aanhouden;
    mulv _na_file;
    bool voorwaarde;
    
    if (prm1 != NG) {                   /* controleer instellingen                                          */
      if (PRM[prm1] > 2) {
        PRM[prm1] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (prm2 != NG) {
      if (PRM[prm2] > 2) {
        PRM[prm2] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (prm3 != NG) {
      if (PRM[prm3] > 2) {
        PRM[prm3] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (prmna != NG) {
      if (PRM[prmna] > 2) {
        PRM[prmna] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    _na_file = PRM[prmna];
    
    if (de1 != NG) {                    /* bepaal FILE aktief                                               */
      if (mm1 != NG) {
        if ( MM[mm1] &&  SCH[sch1]) fil1 = TRUE;
        if (!MM[mm1] || !SCH[sch1]) fil1 = FALSE;
      } else {
        if (  DB[de1] && !DF[de1] &&  SCH[sch1] || (MM[mmfil]&BIT0)) fil1 = TRUE;
        if (!TDH[de1] ||  DF[de1] || !SCH[sch1])                     fil1 = FALSE;
      }
    }
    if (de2 != NG) {
      if (mm2 != NG) {
        if ( MM[mm2] &&  SCH[sch2]) fil2 = TRUE;
        if (!MM[mm2] || !SCH[sch2]) fil2 = FALSE;
      } else {
        if (  DB[de2] && !DF[de2] &&  SCH[sch2] || (MM[mmfil]&BIT1)) fil2 = TRUE;
        if (!TDH[de2] ||  DF[de2] || !SCH[sch2])                     fil2 = FALSE;
      }
    }
    if (de3 != NG) {
      if (mm3 != NG) {
        if ( MM[mm3] &&  SCH[sch3]) fil3 = TRUE;
        if (!MM[mm3] || !SCH[sch3]) fil3 = FALSE;
      } else {
        if (  DB[de3] && !DF[de3] &&  SCH[sch3] || (MM[mmfil]&BIT2)) fil3 = TRUE;
        if (!TDH[de3] ||  DF[de3] || !SCH[sch3])                     fil3 = FALSE;
      }
    }
    if (de4 != NG) {
      if (mm4 != NG) {
        if ( MM[mm4] &&  SCH[sch4]) fil4 = TRUE;
        if (!MM[mm4] || !SCH[sch4]) fil4 = FALSE;
      } else {
        if (  DB[de4] && !DF[de4] &&  SCH[sch4] || (MM[mmfil]&BIT3)) fil4 = TRUE;
        if (!TDH[de4] ||  DF[de4] || !SCH[sch4])                     fil4 = FALSE;
      }
    }
    
    if (SPR) {                          /* bepaal continu langstwachtende                                   */
      MM[mmnafc1] = MM[mmnafc2] = MM[mmnafc3] = 255;
      if (fc1 != NG) MM[mmnafc1] = fc1;
      if (fc2 != NG) {
        if (MM[mmnafc1] == 255) MM[mmnafc1] = fc2;
        else                    MM[mmnafc2] = fc2;
      }
      if (fc3 != NG) {
        if (MM[mmnafc1] == 255) MM[mmnafc1] = fc3;
        else {
          if (MM[mmnafc2] == 255) MM[mmnafc2] = fc3;
          else                    MM[mmnafc3] = fc3;
        }
      }
    } else {
      if (fc1 != NG) {                  /* +2 zodat nooit last van afrondingsverschillen                    */
        if (G[fc1] || R[fc1] && !A[fc1] && (KR[fc1] > ((TGL_max[fc1] + TRG_max[fc1])/10 + 2))) {
          if (MM[mmnafc1] == fc1) MM[mmnafc1] = 255;
          if (MM[mmnafc2] == fc1) MM[mmnafc2] = 255;
          if (MM[mmnafc3] == fc1) MM[mmnafc3] = 255;
          _FIL[fc1] &= (~BIT2);
          if (R[fc1]) _FIL[fc1] &= (~BIT5);
        }
        _FIL[fc1] &= (~BIT0);
        _FIL[fc1] &= (~BIT1);
      }
      if (fc2 != NG) {
        if (G[fc2] || R[fc2] && !A[fc2] && (KR[fc2] > ((TGL_max[fc2] + TRG_max[fc2])/10 + 2))) {
          if (MM[mmnafc1] == fc2) MM[mmnafc1] = 255;
          if (MM[mmnafc2] == fc2) MM[mmnafc2] = 255;
          if (MM[mmnafc3] == fc2) MM[mmnafc3] = 255;
          _FIL[fc2] &= (~BIT2);
          if (R[fc2]) _FIL[fc2] &= (~BIT5);
        }
        _FIL[fc2] &= (~BIT0);
        _FIL[fc2] &= (~BIT1);
      }
      if (fc3 != NG) {
        if (G[fc3] || R[fc3] && !A[fc3] && (KR[fc3] > ((TGL_max[fc3] + TRG_max[fc3])/10 + 2))) {
          if (MM[mmnafc1] == fc3) MM[mmnafc1] = 255;
          if (MM[mmnafc2] == fc3) MM[mmnafc2] = 255;
          if (MM[mmnafc3] == fc3) MM[mmnafc3] = 255;
          _FIL[fc3] &= (~BIT2);
          if (R[fc3]) _FIL[fc3] &= (~BIT5);
        }
        _FIL[fc3] &= (~BIT0);
        _FIL[fc3] &= (~BIT1);
      }
      if (MM[mmnafc1] == 255) { MM[mmnafc1] = MM[mmnafc2]; MM[mmnafc2] = MM[mmnafc3]; MM[mmnafc3] = 255; }
      if (MM[mmnafc2] == 255) { MM[mmnafc2] = MM[mmnafc3]; MM[mmnafc3] = 255; }
      if (fc1 != NG) {
        if ((MM[mmnafc1] != fc1) && (MM[mmnafc2] != fc1) && (MM[mmnafc3] != fc1)) {
          if (MM[mmnafc1] == 255) MM[mmnafc1] = fc1;
          else {
            if (MM[mmnafc2] == 255) MM[mmnafc2] = fc1;
            else                    MM[mmnafc3] = fc1;
          }
        }
      }
      if (fc2 != NG) {
        if ((MM[mmnafc1] != fc2) && (MM[mmnafc2] != fc2) && (MM[mmnafc3] != fc2)) {
          if (MM[mmnafc1] == 255) MM[mmnafc1] = fc2;
          else {
            if (MM[mmnafc2] == 255) MM[mmnafc2] = fc2;
            else                    MM[mmnafc3] = fc2;
          }
        }
      }
      if (fc3 != NG) {
        if ((MM[mmnafc1] != fc3) && (MM[mmnafc2] != fc3) && (MM[mmnafc3] != fc3)) {
          if (MM[mmnafc1] == 255) MM[mmnafc1] = fc3;
          else {
            if (MM[mmnafc2] == 255) MM[mmnafc2] = fc3;
            else                    MM[mmnafc3] = fc3;
          }
        }
      }
    }
    
    if (MM[mmnafil] == 0) _na_file = PRM[prmna]; /* werking nafile niet wijzigen tijdens nafile             */
    else                  _na_file = MM[mmnafil];
    
    voorwaarde = FALSE;
    if (fc1 != NG) { if (SGD[fc1]&BIT5) voorwaarde = TRUE; }
    if (fc2 != NG) { if (SGD[fc2]&BIT5) voorwaarde = TRUE; }
    if (fc3 != NG) { if (SGD[fc3]&BIT5) voorwaarde = TRUE; }
    
    if (BLK_PRI_REA && (MM[mmnafil] == 2) && !voorwaarde) {
                                        /* prioriteit nog niet in werking en kan ook niet verstrekt worden  */
      MM[mmnafil]= 1;
      _na_file   = 1;
      voorwaarde = FALSE;
      if (fc1 != NG) { if (_FIL[fc1]&BIT5) voorwaarde = TRUE; }
      if (fc2 != NG) { if (_FIL[fc2]&BIT5) voorwaarde = TRUE; }
      if (fc3 != NG) { if (_FIL[fc3]&BIT5) voorwaarde = TRUE; }
      
      if (voorwaarde) {                 /* kies cyclus doseren ipv prioriteit langstwachtende               */
        if (fc1 != NG) { _FIL[fc1] |= BIT2; _FIL[fc1] &= (~BIT5); }
        if (fc2 != NG) { _FIL[fc2] |= BIT2; _FIL[fc2] &= (~BIT5); }
        if (fc3 != NG) { _FIL[fc3] |= BIT2; _FIL[fc3] &= (~BIT5); }
      }
    }
    
    if (!fil1 && !fil2 && !fil3 && !fil4 && (MM[mmfil]>0)) {
      MM[mmfil]   = 0;
      MM[mmnafil] = 1;
      if (_na_file == 0) MM[mmnafil] = FALSE;
      if (_na_file == 1) {
        if (fc1 != NG) _FIL[fc1] |= BIT2;
        if (fc2 != NG) _FIL[fc2] |= BIT2;
        if (fc3 != NG) _FIL[fc3] |= BIT2;
      }
      if (_na_file == 2) {
        MM[mmnafil] = 2;
        if (fc1 != NG) {
          if (PRM[prm1] != NG) {
            if ((PRM[prm1] == 2) && (MM[mmnafc1] == fc1) && R[fc1] && !TRG[fc1] && A[fc1]) {
              _FIL[fc1] |= BIT5;
            }
          }
        }
        if (fc2 != NG) {
          if (PRM[prm2] != NG) {
            if ((PRM[prm2] == 2) && (MM[mmnafc1] == fc2) && R[fc2] && !TRG[fc2] && A[fc2]) {
              _FIL[fc2] |= BIT5;
            }
          }
        }
        if (fc3 != NG) {
          if (PRM[prm3] != NG) {
            if ((PRM[prm3] == 2) && (MM[mmnafc1] == fc3) && R[fc3] && !TRG[fc3] && A[fc3]) {
              _FIL[fc3] |= BIT5;
            }
          }
        }
      }
    }
    
    if (fil1 || fil2 || fil3 || fil4) {
      MM[mmfil] = 0;
      if (fil1) MM[mmfil] |= BIT0;
      if (fil2) MM[mmfil] |= BIT1;
      if (fil3) MM[mmfil] |= BIT2;
      if (fil4) MM[mmfil] |= BIT3;
      
      MM[mmnafil] = 0;
      if (fc1 != NG) _FIL[fc1] = FALSE;
      if (fc2 != NG) _FIL[fc2] = FALSE;
      if (fc3 != NG) _FIL[fc3] = FALSE;
    }
    
    if ((MM[mmfil]>0) || (MM[mmnafil]>0)) {
      if ((fc1 != NG) && (dos1 != NG)) if (_DOS[fc1] > (T_max[dos1]/10)) _DOS[fc1] = T_max[dos1]/10;
      if ((fc2 != NG) && (dos2 != NG)) if (_DOS[fc2] > (T_max[dos2]/10)) _DOS[fc2] = T_max[dos2]/10;
      if ((fc3 != NG) && (dos3 != NG)) if (_DOS[fc3] > (T_max[dos3]/10)) _DOS[fc3] = T_max[dos3]/10;
      
      if ((fc1 != NG) && (blk1 != NG)) if (_FBL[fc1] < (T_max[blk1]/10)) _FBL[fc1] = T_max[blk1]/10;
      if ((fc2 != NG) && (blk2 != NG)) if (_FBL[fc2] < (T_max[blk2]/10)) _FBL[fc2] = T_max[blk2]/10;
      if ((fc3 != NG) && (blk3 != NG)) if (_FBL[fc3] < (T_max[blk3]/10)) _FBL[fc3] = T_max[blk3]/10;
      
      if ((fc1 != NG) && (prm1 != NG)) {
        if (PRM[prm1]&BIT0) _FIL[fc1] |= BIT0; /* fc1 doseren                                               */
        if (PRM[prm1]&BIT1) _FIL[fc1] |= BIT1; /* fc1 blokkeren                                             */
      }
      if ((fc2 != NG) && (prm2 != NG)) {
        if (PRM[prm2]&BIT0) _FIL[fc2] |= BIT0; /* fc2 doseren                                               */
        if (PRM[prm2]&BIT1) _FIL[fc2] |= BIT1; /* fc2 blokkeren                                             */
      }
      if ((fc3 != NG) && (prm3 != NG)) {
        if (PRM[prm3]&BIT0) _FIL[fc3] |= BIT0; /* fc3 doseren                                               */
        if (PRM[prm3]&BIT1) _FIL[fc3] |= BIT1; /* fc3 blokkeren                                             */
      }
      if (MM[mmnafil]>0) {
        if (fc1 != NG) {
          if (_FIL[fc1]&BIT5) {
            _FIL[fc1] &= (~BIT0);
            _FIL[fc1] &= (~BIT1);
            if (G[fc1] && (WG[fc1] && !SWG[fc1]) || MG[fc1] || EG[fc1]) _FIL[fc1] &= (~BIT5);
          }
          if ((_FIL[fc1]&BIT5) && (fc2 != NG)) _FIL[fc2] &= (~BIT5);
          if ((_FIL[fc1]&BIT5) && (fc3 != NG)) _FIL[fc3] &= (~BIT5);
        }
        if (fc2 != NG) {
          if (_FIL[fc2]&BIT5) {
            _FIL[fc2] &= (~BIT0);
            _FIL[fc2] &= (~BIT1);
            if (G[fc2] && (WG[fc2] && !SWG[fc2]) || MG[fc2] || EG[fc2]) _FIL[fc2] &= (~BIT5);
          }
          if ((_FIL[fc2]&BIT5) && (fc1 != NG)) _FIL[fc1] &= (~BIT5);
          if ((_FIL[fc2]&BIT5) && (fc3 != NG)) _FIL[fc3] &= (~BIT5);
        }
        if (fc3 != NG) {
          if (_FIL[fc3]&BIT5) {
            _FIL[fc3] &= (~BIT0);
            _FIL[fc3] &= (~BIT1);
            if (G[fc3] && (WG[fc3] && !SWG[fc3]) || MG[fc3] || EG[fc3]) _FIL[fc3] &= (~BIT5);
          }
          if ((_FIL[fc3]&BIT5) && (fc1 != NG)) _FIL[fc1] &= (~BIT5);
          if ((_FIL[fc3]&BIT5) && (fc2 != NG)) _FIL[fc2] &= (~BIT5);
        }
        aanhouden = FALSE;
        if (fc1 != NG) { if ((_FIL[fc1]&BIT2) || (_FIL[fc1]&BIT5)) aanhouden = TRUE; }
        if (fc2 != NG) { if ((_FIL[fc2]&BIT2) || (_FIL[fc2]&BIT5)) aanhouden = TRUE; }
        if (fc3 != NG) { if ((_FIL[fc3]&BIT2) || (_FIL[fc3]&BIT5)) aanhouden = TRUE; }
        if (!aanhouden) {
          MM[mmnafil] = 0;
          if (fc1 != NG) _FIL[fc1] = FALSE;
          if (fc2 != NG) _FIL[fc2] = FALSE;
          if (fc3 != NG) _FIL[fc3] = FALSE;
        }
      }
    } else {                            /* geen file of nafile dan altijd reset _FIL[]                      */
      if (fc1 != NG) _FIL[fc1] = FALSE;
      if (fc2 != NG) _FIL[fc2] = FALSE;
      if (fc3 != NG) _FIL[fc3] = FALSE;
    }
    
    if (fc1 != NG) {
      if (_FIL[fc1]&BIT1) _FIL[fc1] |=   BIT0;
      if (MM[mmnafil]==1) _FIL[fc1] &= (~BIT1);
      if (R[fc1] && (_FIL[fc1]&BIT0)) BAR[fc1] = TRUE;
      if (R[fc1] && (_FIL[fc1]&BIT1))  BL[fc1] = TRUE;
      if ((TFG_max[fc1]/10) > _DOS[fc1]) _DOS[fc1] = TFG_max[fc1]/10;
      if ((TGG_max[fc1]/10) > _DOS[fc1]) _DOS[fc1] = TGG_max[fc1]/10;
      if (_FIL[fc1]&BIT0) TKMX[fc1] = 0;
    }
    if (fc2 != NG) {
      if (_FIL[fc2]&BIT1) _FIL[fc2] |=   BIT0;
      if (MM[mmnafil]==1) _FIL[fc2] &= (~BIT1);
      if (R[fc2] && (_FIL[fc2]&BIT0)) BAR[fc2] = TRUE;
      if (R[fc2] && (_FIL[fc2]&BIT1))  BL[fc2] = TRUE;
      if ((TFG_max[fc2]/10) > _DOS[fc2]) _DOS[fc2] = TFG_max[fc2]/10;
      if ((TGG_max[fc2]/10) > _DOS[fc2]) _DOS[fc2] = TGG_max[fc2]/10;
      if (_FIL[fc2]&BIT0) TKMX[fc2] = 0;
    }
    if (fc3 != NG) {
      if (_FIL[fc3]&BIT1) _FIL[fc3] |=   BIT0;
      if (MM[mmnafil]==1) _FIL[fc3] &= (~BIT1);
      if (R[fc3] && (_FIL[fc3]&BIT0)) BAR[fc3] = TRUE;
      if (R[fc3] && (_FIL[fc3]&BIT1))  BL[fc3] = TRUE;
      if ((TFG_max[fc3]/10) > _DOS[fc3]) _DOS[fc3] = TFG_max[fc3]/10;
      if ((TGG_max[fc3]/10) > _DOS[fc3]) _DOS[fc3] = TGG_max[fc3]/10;
      if (_FIL[fc3]&BIT0) TKMX[fc3] = 0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure blokkade bij FILE ingreep                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_Xpr_sgd_file(void)          /* Fik170802                                                        */
  {
    count i;
    
    for (i=0; i<FC_MAX; i++) {
      SGD[i] &= (~BIT8);
      if (R[i] && (_FIL[i]&BIT0)) {
        if (KR[i] <= (((TGL_max[i]+TRG_max[i])/10) + _FBL[i])) {
          SGD[i] |= BIT6;
          SGD[i] |= BIT8;
        }
      }
    }
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN [ Gelijkstart ]                                                            */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure definitie deelconflicten gelijke start                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_def(                      /* Fik110801                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count ma12,                         /* sch meerealisatie fc1 met fc2                                    */
    count ma21,                         /* sch meerealisatie fc1 met fc2                                    */
    count ti12,                         /* t ontruimingstijd fc1 -> fc2                                     */
    count ti21)                         /* t ontruimingstijd fc2 -> fc1                                     */
  {
    count i;
    bool _wtv = FALSE;
    
    dcf_gs[aantal_dcf_gs][0] = fc1;
    dcf_gs[aantal_dcf_gs][1] = fc2;
    dcf_gs[aantal_dcf_gs][2] = ma12;
    dcf_gs[aantal_dcf_gs][3] = ma21;
    dcf_gs[aantal_dcf_gs][4] = ti12;
    dcf_gs[aantal_dcf_gs][5] = ti21;
    
    for (i=0; i<=1; i++) {
      if (WTVFC[dcf_gs[aantal_dcf_gs][i]]&BIT0) _wtv = TRUE;
    }
    if (_wtv) {
      for (i=0; i<=1; i++) {
        if (!(WTVFC[dcf_gs[aantal_dcf_gs][i]]&BIT0)) WTVFC[dcf_gs[aantal_dcf_gs][i]] |= BIT1;
      }
    }
    if (aantal_dcf_gs < FC_MAX - 1) aantal_dcf_gs++;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Bepaal of twee richtingen een gelijke start hebben                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  bool dcf_gelijk(                      /* Fik110801                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2)                          /* fasecyclus 2                                                     */
  {
    count i;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      if ((dcf_gs[i][0] == fc1) && (dcf_gs[i][1] == fc2) ||
          (dcf_gs[i][0] == fc2) && (dcf_gs[i][1] == fc1)) return TRUE;
    }
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal tijdelijke conflictmatrix tbv deelconflicten gelijke start                              */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_confl(void)               /* Fik110801                                                        */
  {
    mulv i,j;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv ti12,ti21;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      ma12 = dcf_gs[i][2];
      ma21 = dcf_gs[i][3];
      ti12 = dcf_gs[i][4];
      ti21 = dcf_gs[i][5];
      
      WTVFC[fc1] = FALSE;               /* combinatie dcf_gs en wt voorspeller nog niet mogelijk!           */
      WTVFC[fc2] = FALSE;
      
      if (AKTPRG > 0) {                 /* combinatie dcf-gs en prioriteit ov tijdens VAS nog niet 
                                           mogelijk!                                                        */
        HPRI[fc1] |= BIT0;
        HPRI[fc2] |= BIT0;
      }
      
      RT[ti12]  = G[fc1];
      RT[ti21]  = G[fc2];
      
#ifndef INTERGROEN
      RT[ti12] |= GL[fc1];
      RT[ti21] |= GL[fc2];
#endif
      
      if (RT[ti12])     TMPi[fc1][fc2] = T_max[ti12];
      else if (T[ti12]) TMPi[fc1][fc2] = T_max[ti12] - T_timer[ti12];
      
      if (RT[ti21])     TMPi[fc2][fc1] = T_max[ti21];
      else if (T[ti21]) TMPi[fc2][fc1] = T_max[ti21] - T_timer[ti21];
      
#ifndef INTERGROEN
      if ( G[fc1]) TMPi[fc1][fc2] += TGL_max[fc1];
      if (GL[fc1]) TMPi[fc1][fc2] += TGL_max[fc1] - TGL_timer[fc1];
      
      if ( G[fc2]) TMPi[fc2][fc1] += TGL_max[fc2];
      if (GL[fc2]) TMPi[fc2][fc1] += TGL_max[fc2] - TGL_timer[fc2];
#endif
      
      if (R[fc1] && G[fc2] || R[fc2] && G[fc1]) {
        if (!TMPc[fc1][fc2] || !TMPc[fc2][fc1]) NEST = TRUE;
        TMPc[fc1][fc2] = TMPc[fc2][fc1] = TRUE;
      }
      
      if (!G[fc1] && !G[fc2] && (GL[fc1] || TRG[fc1] || A[fc1] || SCH[ma12])) {
        
        for (j=0; j<FC_MAX; j++) {
          
          if ((TI_max(fc1,j) >= 0) || (TI_max(fc1,j) <= -3) || TMPc[fc1][j]) {
            if ((j != fc2) && ((TI_max(fc2,j) == -1) || (TI_max(fc2,j) == -2))) {
              if (!TMPc[fc2][j] || !TMPc[j][fc2]) NEST = TRUE;
              TMPc[fc2][j] = TMPc[j][fc2] = TRUE;
            }
          }
          
          if ((TI_max(fc1,j) == -2) || TMPf[fc1][j]) {
            if ((j != fc2) && (TI_max(fc2,j) == -1) && !TMPc[fc2][j]) {
              if (!TMPf[fc2][j] || !TMPf[j][fc2]) NEST = TRUE;
              TMPf[fc2][j] = TMPf[j][fc2] = TRUE;
            }
          }
        }
      }
      
      if (!G[fc2] && !G[fc1] && (GL[fc2] || TRG[fc2] || A[fc2] || SCH[ma21])) {
        
        for (j=0; j<FC_MAX; j++) {
          
          if ((TI_max(fc2,j) >= 0) || (TI_max(fc2,j) <= -3) || TMPc[fc2][j]) {
            if ((j != fc1) && ((TI_max(fc1,j) == -1) || (TI_max(fc1,j) == -2))) {
              if (!TMPc[fc1][j] || !TMPc[j][fc1]) NEST = TRUE;
              TMPc[fc1][j] = TMPc[j][fc1] = TRUE;
            }
          }
          
          if ((TI_max(fc2,j) == -2) || TMPf[fc2][j]) {
            if ((j != fc1) && (TI_max(fc1,j) == -1) && !TMPc[fc1][j]) {
              if (!TMPf[fc1][j] || !TMPf[j][fc1]) NEST = TRUE;
              TMPf[fc1][j] = TMPf[j][fc1] = TRUE;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TTR tbv deelconflicten gelijke start                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_corr_ttr(void)            /* Fik110801                                                        */
  {
    mulv i,j;
    mulv max_ttr;
    mulv max_ttc;
    mulv max_ttw;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      max_ttr = max_ttc = max_ttw = 0;
      for (j=0; j<=1; j++) {
        if (dcf_gs[i][j] != NG) {
          if  (TTR[dcf_gs[i][j]] > max_ttr) max_ttr = TTR[dcf_gs[i][j]];
          if  (TTC[dcf_gs[i][j]] > max_ttc) max_ttc = TTC[dcf_gs[i][j]];
          if ((TTW[dcf_gs[i][j]] > max_ttw) &&
              (R[dcf_gs[i][j]] && A[dcf_gs[i][j]] || GL[dcf_gs[i][j]] || TRG[dcf_gs[i][j]])) {
            max_ttw = TTW[dcf_gs[i][j]];
          }
        }
      }
      for (j=0; j<=1; j++) {
        if (dcf_gs[i][j] != NG) {
          if (!G[dcf_gs[i][j]]) TTR[dcf_gs[i][j]] = max_ttr;
          /* geen correctie TTC[] ivm variabel meeverlenggroen; voor combinatie met hki wel nodig!          */
          /* if (!G[dcf_gs[i][j]]) TTC[dcf_gs[i][j]] = max_ttc;                                             */
          if (!G[dcf_gs[i][j]]) TTW[dcf_gs[i][j]] = max_ttw;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TTP tbv deelconflicten gelijke start                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_corr_ttp(void)            /* Fik110801                                                        */
  {
    mulv i,j;
    mulv max_ttp;
    mulv correctie;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      max_ttp = 0;
      for (j=0; j<=1; j++) {
        if (dcf_gs[i][j] != NG) {
          if ((TTP[dcf_gs[i][j]] < 255)
              && (TTP[dcf_gs[i][j]] > max_ttp)) max_ttp = TTP[dcf_gs[i][j]];
        }
      }
      for (j=0; j<=1; j++) {
        if (dcf_gs[i][j] != NG) {
          if ((TTP[dcf_gs[i][j]] < 255)
              && (TTP[dcf_gs[i][j]] < max_ttp)) {
            correctie = max_ttp - TTP[dcf_gs[i][j]];
            TTP[dcf_gs[i][j]] += correctie;
            TEG[dcf_gs[i][j]] += correctie;
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure vaste meeaanvragen tbv deelconflicten gelijke start                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_mav(void)                 /* Fik110801                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma12,ma21;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      
      fc1 = dcf_gs[i][0];
      fc2 = dcf_gs[i][1];
      ma12= dcf_gs[i][2];
      ma21= dcf_gs[i][3];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (R[fc1] && A[fc1] && !BL[fc1] && R[fc2] && SCH[ma21] && !BL[fc2]) set_A(fc2);
        if (R[fc2] && A[fc2] && !BL[fc2] && R[fc1] && SCH[ma12] && !BL[fc1]) set_A(fc1);
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer realisatie-geheugen tbv deelconflicten gelijke start                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_rea(void)                 /* Fik120124                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (G[fc1] && ((SGD[fc1]&BIT0) || (SGD[fc1]&BIT3))) {
          if (G[fc1] && !G[fc2] && (GL[fc2] || TRG[fc2] || !A[fc2] || (A[fc2]&BIT7))) SGD[fc2] |= BIT2;
        }
        if (G[fc2] && ((SGD[fc2]&BIT0) || (SGD[fc2]&BIT3))) {
          if (G[fc2] && !G[fc1] && (GL[fc1] || TRG[fc1] || !A[fc1] || (A[fc1]&BIT7))) SGD[fc1] |= BIT2;
        }
        if (!G[fc1] && !G[fc2]) {
          if (!(SGD[fc1]&BIT2) && (GL[fc2] || TRG[fc2] || R[fc2] && A[fc2])) SGD[fc2] &= (~BIT2);
          if (!(SGD[fc2]&BIT2) && (GL[fc1] || TRG[fc1] || R[fc1] && A[fc1])) SGD[fc1] &= (~BIT2);
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer BAR[] tbv deelconflicten gelijke start                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_bar(void)                 /* Fik110801                                                        */
  {
    mulv i,j;
    mulv fc1,fc2;
    mulv ma12,ma21;
    bool door_BAR;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      ma12 = dcf_gs[i][2];
      ma21 = dcf_gs[i][3];
      
      if ((fc1 != NG) && (fc2 != NG)) { /* altijd BAR[] indien geel of garantie rood                        */
        if (GL[fc1] || R[fc1] && TRG[fc1]) BAR[fc1] |= TRUE;
        if (GL[fc2] || R[fc2] && TRG[fc2]) BAR[fc2] |= TRUE;
        
        door_BAR = FALSE;
        if (GL[fc1] || TRG[fc1] || R[fc1] && BAR[fc1] && !BL[fc1] && (A[fc1] || SCH[ma12])) door_BAR = TRUE;
        if (GL[fc2] || TRG[fc2] || R[fc2] && BAR[fc2] && !BL[fc2] && (A[fc2] || SCH[ma21])) door_BAR = TRUE;
        
        if (door_BAR) {
          for (j=0; j<=1; j++) {
            if (dcf_gs[i][j] != NG) {
              if (!G[dcf_gs[i][j]]) BAR[dcf_gs[i][j]] |= TRUE;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure afwikkeling deelconflicten gelijke start                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_afw_sgd(void)             /* Fik120124                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv ti12,ti21;
    bool xu1,xu2;
    
    dcf_gs_mav();                       /* set zonodig mee aanvragen                                        */
    for (i=0; i<aantal_dcf_gs; i++) {
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      ma12 = dcf_gs[i][2];
      ma21 = dcf_gs[i][3];
      ti12 = dcf_gs[i][4];
      ti21 = dcf_gs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) { /* altijd gelijktijdig starten wachtstand                           */
        if (GL[fc1] || TRG[fc1] || K[fc1] || RT[ti21] || T[ti21] ||
            GL[fc2] || TRG[fc2] || K[fc2] || RT[ti12] || T[ti12]) {
          BAR[fc1] |= !G[fc1];
          BAR[fc2] |= !G[fc2];
        }
        
        /* beurt tbv gelijke start                                                                          */
        
        if ((RV[fc1] && B[fc1] || RA[fc1]) && A[fc1] && !RR[fc1] && !BL[fc1]) {
          if (R[fc2] && !TRG[fc2] && A[fc2] && !BL[fc2]) {
            B[fc2] |= BIT3;
            RR[fc2] = FALSE;
          }
        }
        if ((RV[fc2] && B[fc2] || RA[fc2]) && A[fc2] && !RR[fc2] && !BL[fc2]) {
          if (R[fc1] && !TRG[fc1] && A[fc1] && !BL[fc1]) {
            B[fc1] |= BIT3;
            RR[fc1] = FALSE;
          }
        }
        
        /* uitstel tbv gelijke start                                                                        */
        xu1 = xu2 = TRUE;               /* !B[] ter voorbereiding op FILE nass                              */
        if (RA[fc1] && !K[fc1] && !RT[ti21] && !T[ti21] && !X[fc1] && !RR[fc1] && !BL[fc1]
            || RV[fc1] && !TRG[fc1] && (!A[fc1] || !B[fc1] || BL[fc1])) xu1 = FALSE;
        if (RA[fc2] && !K[fc2] && !RT[ti12] && !T[ti12] && !X[fc2] && !RR[fc2] && !BL[fc2]
            || RV[fc2] && !TRG[fc2] && (!A[fc2] || !B[fc2] || BL[fc2])) xu2 = FALSE;
        
        if (xu1 || xu2) {
          if (!G[fc1]) X[fc1] |= BIT3;
          if (!G[fc2]) X[fc2] |= BIT3;
        }
      }
    }
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN [ Synchroonstart ]                                                         */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure definitie deelconflicten synchroon start                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_def(                      /* Fik110801                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count ma12,                         /* sch meerealisatie fc1 met fc2                                    */
    count ma21,                         /* sch meerealisatie fc2 met fc1                                    */
    count ti12,                         /* t ontruimingstijd fc1 -> fc2                                     */
    count vs21)                         /* t voorstarttijd   fc2 -> fc1                                     */
  {
    dcf_vs[aantal_dcf_vs][0] = fc1;
    dcf_vs[aantal_dcf_vs][1] = fc2;
    dcf_vs[aantal_dcf_vs][2] = ma12;
    dcf_vs[aantal_dcf_vs][3] = ma21;
    dcf_vs[aantal_dcf_vs][4] = ti12;
    dcf_vs[aantal_dcf_vs][5] = vs21;
    
    if (aantal_dcf_vs < FC_MAX - 1) aantal_dcf_vs++;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Bepaal of twee richtingen een synchroon start hebben                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  mulv dcf_sync(                        /* Fik110801                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2)                          /* fasecyclus 2                                                     */
  {
    count i;
    /* return voorstarttijd fc2 tov fc1 of NG indien geen synchroonstart                                    */
    for (i=0; i<aantal_dcf_vs; i++) {
      if ((dcf_vs[i][0] == fc1) && (dcf_vs[i][1] == fc2)) return dcf_vs[i][5];
    }
    return NG;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal tijdelijke conflictmatrix tbv deelconflicten synchroon start                            */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_confl(void)               /* Fik180901                                                        */
  {
    mulv i,j;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv ti12,vs21;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      ma12 = dcf_vs[i][2];
      ma21 = dcf_vs[i][3];
      ti12 = dcf_vs[i][4];
      vs21 = dcf_vs[i][5];
      
      WTVFC[fc1] = FALSE;               /* combinatie dcf_vs en wt voorspeller nog niet mogelijk!           */
      /* WTVFC[fc2] = FALSE; -> wel voor parallel richting                                                  */
      
      if (AKTPRG > 0) {                 /* combinatie dcf-vs en prioriteit ov tijdens VAS nog niet 
                                           mogelijk!                                                        */
        HPRI[fc1] |= BIT0;
        /* HPRI[fc2] |= BIT0; -> wel voor parallel richting                                                 */
      }
      
      RT[ti12]  = G[fc1];
#ifndef INTERGROEN
      RT[ti12] |= GL[fc1];
#endif
      RT[vs21] = SG[fc2];
      
      if (RT[ti12])     TMPi[fc1][fc2] = T_max[ti12];
      else if (T[ti12]) TMPi[fc1][fc2] = T_max[ti12] - T_timer[ti12];
      
#ifndef INTERGROEN
      if ( G[fc1]) TMPi[fc1][fc2] += TGL_max[fc1];
      if (GL[fc1]) TMPi[fc1][fc2] += TGL_max[fc1] - TGL_timer[fc1];
#endif
      
      if (R[fc2] && G[fc1]) {
        if (!TMPc[fc1][fc2] || !TMPc[fc2][fc1]) NEST = TRUE;
        TMPc[fc1][fc2] = TMPc[fc2][fc1] = TRUE;
      }
      
      if (!G[fc2] && !G[fc1] && (GL[fc2] || TRG[fc2] || A[fc2] || SCH[ma21])) {
        
        for (j=0; j<FC_MAX; j++) {
          
          if ((TI_max(fc2,j) >= 0) || (TI_max(fc2,j) <= -3) || TMPc[fc2][j]) {
            if ((j != fc1) && ((TI_max(fc1,j) == -1) || (TI_max(fc1,j) == -2))) {
              if (!TMPc[fc1][j] || !TMPc[j][fc1]) NEST = TRUE;
              TMPc[fc1][j] = TMPc[j][fc1] = TRUE;
            }
          }
          
          if ((TI_max(fc2,j) == -2) || TMPf[fc2][j]) {
            if ((j != fc1) && (TI_max(fc1,j) == -1) && !TMPc[fc1][j]) {
              if (!TMPf[fc1][j] || !TMPf[j][fc1]) NEST = TRUE;
              TMPf[fc1][j] = TMPf[j][fc1] = TRUE;
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TTR tbv deelconflicten synchroon start                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_corr_ttr(void)            /* Fik180901                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma21;
    mulv vs21;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      ma21 = dcf_vs[i][3];
      vs21 = dcf_vs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (!G[fc1] && !G[fc2]) {
          if (TTR[fc2] > TTR[fc1]) TTR[fc1] = TTR[fc2];
          /* geen correctie TTC[] ivm variabele meeverlenggroen; voor combinatie met hki wel nodig!         */
        }
        if (!G[fc1] && (!G[fc2] || SG[fc2])) { /* MS                                                        */
          if (((TTW[fc2] + T_max[vs21]) > TTW[fc1]) &&
              (R[fc2] && (A[fc2] || SCH[ma21]) || GL[fc2] || TRG[fc2] || SG[fc2])) {
            TTW[fc1] = TTW[fc2] + T_max[vs21];
          }
        }
        if (!G[fc1] && !SG[fc2] && G[fc2] && T[vs21]) {
          if ((TTW[fc2] + T_max[vs21] - T_timer[vs21]) > TTW[fc1]) {
            TTW[fc1] = TTW[fc2] + T_max[vs21] - T_timer[vs21];
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer ALTM[] tbv deelconflicten synchroon start                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_corr_alt(void)            /* Fik110801                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv vs21;
    mulv correctie[FCMAX];
    
    for (i=0; i<FC_MAX; i++) {
      correctie[i] = 0;
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      vs21 = dcf_vs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (!G[fc1]) {
          if (!G[fc2] || !RA[fc1] && (SG[fc2] || T[vs21])) {
            if (T_max[vs21] > correctie[fc1]) correctie[fc1] = T_max[vs21];
          }
        }
      }
    }
    
    for (i=0; i<FC_MAX; i++) {
      if ((AKTPRG == 0) || !_GRN[i]) {
        ALTM[i] += correctie[i]/10;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TTP tbv deelconflicten synchroon start                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_corr_ttp(void)            /* Fik180901                                                        */
  {
    count i;
    count fc1,fc2;
    count vs21;
    mulv  correctie;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      vs21 = dcf_vs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) { /* MS                                                               */
        if (!G[fc1] && (TTP[fc1] < 255) && (TTP[fc2] < 255) && ((TTP[fc2] + T_max[vs21]/10) > TTP[fc1])) {
          correctie = TTP[fc2] + (T_max[vs21]/10) - TTP[fc1];
          if ((TTP[fc1] + correctie) > 254) correctie = 254 - TTP[fc1];
          TTP[fc1] += correctie;
          TEG[fc1] += correctie;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer realisatie-geheugen tbv deelconflicten synchroon start                               */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_rea(void)                 /* Fik130907                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (G[fc1] && ((SGD[fc1]&BIT0) || (SGD[fc1]&BIT3))) {
          if (G[fc1] && !G[fc2] && !(SGD[fc2]&BIT6) && (GL[fc2] || TRG[fc2] || !A[fc2] || (A[fc2]&BIT7)))
            SGD[fc2] |= BIT2;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer BAR[] tbv deelconflicten synchroon start                                             */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_bar(void)                 /* Fik110801                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma21;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      ma21 = dcf_vs[i][3];
      
      if ((fc1 != NG) && (fc2 != NG)) { /* altijd BAR[] indien geel of garantie rood                        */
        if (GL[fc1] || R[fc1] && TRG[fc1]) BAR[fc1] |= TRUE;
        if (GL[fc2] || R[fc2] && TRG[fc2]) BAR[fc2] |= TRUE;
        
        if (GL[fc2] || TRG[fc2] || R[fc2] && BAR[fc2] && !BL[fc2] && (A[fc2] || SCH[ma21])) {
          if (!G[fc1]) BAR[fc1] |= TRUE;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure afwikkeling deelconflicten synchroon start                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_afw_sgd(void)             /* Fik131013                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv ti12,vs21;
    bool xu1,xu2;
    
    dcf_gs_mav();                       /* set zonodig mee aanvragen voor richtingen in gelijkstart         */
    
    for (i=0; i<aantal_dcf_gs; i++) {   /* eerst beurt afhandeling richtingen in gelijkstart                */
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if ((RV[fc1] && B[fc1] || RA[fc1]) && A[fc1] && !RR[fc1] && !BL[fc1]) {
          if (R[fc2] && !TRG[fc2] && A[fc2] && !BL[fc2]) {
            B[fc2] |= BIT3;
            RR[fc2] = FALSE;
          }
        }
        if ((RV[fc2] && B[fc2] || RA[fc2]) && A[fc2] && !RR[fc2] && !BL[fc2]) {
          if (R[fc1] && !TRG[fc1] && A[fc1] && !BL[fc1]) {
            B[fc1] |= BIT3;
            RR[fc1] = FALSE;
          }
        }
      }
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {   /* reset _DCF bitjes                                                */
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      
      /* _DCF bit sturing                                                                                   */
      /* BIT0: Fasecyclus wacht op deelconflict met voorstart                                               */
      /* BIT1: Fasecyclus wacht op afloop (fictieve)ontruimingstijd                                         */
      /* BIT2: Fasecyclus met voorstart wordt in MG[] gehouden totdat deelconflict ook groen toont          */
      /* BIT7: Fasecyclus nooit meer retourrood                                                             */
      
      _DCF[fc1] &= (~BIT0);
      _DCF[fc1] &= (~BIT1);
      _DCF[fc1] &= (~BIT2);
      
      _DCF[fc2] &= (~BIT0);
      _DCF[fc2] &= (~BIT1);
      _DCF[fc2] &= (~BIT2);
      
      if (!RA[fc1]) _DCF[fc1] &= (~BIT7);
      if (!RA[fc2]) _DCF[fc2] &= (~BIT7);
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      ma12 = dcf_vs[i][2];
      ma21 = dcf_vs[i][3];
      ti12 = dcf_vs[i][4];
      vs21 = dcf_vs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) { /* fc1 alleen wachtstand als geen conflicten van fc2 aanwezig       */
        if (GL[fc1] || TRG[fc1] || K[fc1] ||
            GL[fc2] || TRG[fc2] || K[fc2] || RT[ti12] || T[ti12]) {
          BAR[fc1] |= !G[fc1];
        }
        
        /* set zonodig mee aanvraag                                                                         */
        /* ------------------------                                                                         */
        if (RA[fc1] && !RR[fc1] && !K[fc1] && !BL[fc1] && SCH[ma21] && !(_FIL[fc2]&BIT0)) {
          if (R[fc2] && !TRG[fc2] && !K[fc2] && !RT[ti12] && !T[ti12] && !BL[fc2]) set_A(fc2);
        }
        
        /* Beurt t.b.v. synchroon start                                                                     */
        /* ----------------------------                                                                     */
        if ((RV[fc1] && B[fc1] || RA[fc1]) && A[fc1] && !RR[fc1] && !BL[fc1]) {
          if (R[fc2] && !TRG[fc2] && !K[fc2] && !RT[ti12] && !T[ti12] && !BL[fc2] &&
              A[fc2] && !(_FIL[fc2]&BIT0)) {
            B[fc2] |= BIT3;
            RR[fc2] = FALSE;
            _DCF[fc1] |= BIT7;
            _DCF[fc2] |= BIT7;
          }
        }
        
        xu1 = xu2 = TRUE;
        if (RA[fc1] && !K[fc1] && !RT[vs21] && !T[vs21] &&
            (G[fc2] || RV[fc2] && !TRG[fc2] && (!SCH[ma21] || BL[fc2]) &&
              (!A[fc2] || !B[fc2] && !K[fc2] && !RT[ti12] && !T[ti12]))) xu1 = FALSE;
        
        if (RA[fc2] && !K[fc2] && !RT[ti12] && !T[ti12] && !BL[fc2] ) xu2 = FALSE;
        
        if (xu1) {
          if (RA[fc1]) X[fc1] = TRUE;
          _DCF[fc1] |= BIT0;
        }
        if (xu2) {
          if (RA[fc2]) X[fc2] = TRUE;
          _DCF[fc2] |= BIT1;
        }
      }
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {   /* richting in voorstart vasthouden totdat hoofdrichting bijkomt    */
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      if (G[fc2] && (RV[fc1] && B[fc1] && !RR[fc1] && !BL[fc1] || RA[fc1])) {
        YM[fc2] = TRUE;
        _DCF[fc2] |= BIT2;
      }
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {   /* bij drie-traps-deelconflict zonodig groen afbreken               */
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      if (G[fc1] && (_DCF[fc1]&BIT2) && RA[fc2] && !BL[fc2]) {
        if (MG[fc1] && !(SGD[fc1]&BIT5)) Z[fc1] = TRUE;
        if (VG[fc1] && (KG[fc1] > ALTM[fc1])) FM[fc1] = TRUE;
      }
    }
  }
  
