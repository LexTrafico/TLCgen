/* ======================================================================================================== */
/* appl_vas.c                                                                                               */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Include files                                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
#ifdef _VAS_OV
  #include "vasovinc.c"                 /* tbv VAS_OV                                                       */
#endif                                  /* *** ifdef -> _VAS_OV                                             */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure VAS programma-keuze SLAAF                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_progkeuze_slaaf(        /* datum 22-07-2010                                                 */
    count tt1,                          /* Index timer tijdvertr. omschakelen                               */
    count mckode)                       /* Index geheugenelement met kode complexcoordinator                */
  {
    count i;
    
    /* CKODE -> kode vanaf complex-coordinator                                                              */
    /* HKODE -> buffer CKODE t.b.v. start-puls                                                              */
    /* LKODE -> kode in lokale automaat (volgt vertraagt CKODE)                                             */
    
    RT[tt1] = FALSE;
    CKODE = MM[mckode];
    
    if (negeer_cpc) CKODE = 0;
#ifdef schnegeer_cc
    if (SCH[schnegeer_cc]) CKODE = 0;
#endif
    
#ifndef prmctijd21
    if (CKODE > aantal_VAS) CKODE = 0;
#else
    if (CKODE > aantal_VAS) {
      if ((CKODE < 21) || (CKODE > 26)) CKODE = 0;
    }
#endif
    
    if (CKODE == 1) {
#ifndef prmctijd1
      CKODE = 0;
#else
      if (PRM[prmctijd1] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 2) {
#ifndef prmctijd2
      CKODE = 0;
#else
      if (PRM[prmctijd2] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 3) {
#ifndef prmctijd3
      CKODE = 0;
#else
      if (PRM[prmctijd3] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 4) {
#ifndef prmctijd4
      CKODE = 0;
#else
      if (PRM[prmctijd4] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 5) {
#ifndef prmctijd5
      CKODE = 0;
#else
      if (PRM[prmctijd5] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 6) {
#ifndef prmctijd6
      CKODE = 0;
#else
      if (PRM[prmctijd6] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 7) {
#ifndef prmctijd7
      CKODE = 0;
#else
      if (PRM[prmctijd7] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 8) {
#ifndef prmctijd8
      CKODE = 0;
#else
      if (PRM[prmctijd8] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 9) {
#ifndef prmctijd9
      CKODE = 0;
#else
      if (PRM[prmctijd9] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 21) {
#ifndef prmctijd21
      CKODE = 0;
#else
      if (PRM[prmctijd21] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 22) {
#ifndef prmctijd22
      CKODE = 0;
#else
      if (PRM[prmctijd22] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 23) {
#ifndef prmctijd23
      CKODE = 0;
#else
      if (PRM[prmctijd23] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 24) {
#ifndef prmctijd24
      CKODE = 0;
#else
      if (PRM[prmctijd24] == 0) CKODE = 0;
#endif
    }
    if (CKODE == 25) {
#ifndef prmctijd25
      CKODE = 0;
#else
      if (PRM[prmctijd25] == 0) CKODE = 0;
#endif
    }
    
    if (CKODE == 26) {
#ifndef prmctijd26
      CKODE = 0;
#else
      if (PRM[prmctijd26] == 0) CKODE = 0;
#endif
    }
    
    if (HKODE != CKODE) { HKODE   = CKODE;
      RT[tt1] = TRUE;
    }
    if ((LKODE != CKODE) && !RT[tt1] && !T[tt1] || SPR) LKODE = CKODE;
    
    GEWPRG = LKODE;
    OMSCH = (AKTPRG != GEWPRG);
    
    if (((STAP == OPNT) || !OPNT) && OMSCH || SPR) {
      
#ifdef _VAS_OV
      
      if (!AKTPRG && GEWPRG) ELOK = TRUE;
      
#endif                                  /* *** ifdef -> _VAS_OV                                             */
      
      AKTPRG = GEWPRG;
      if (AKTPRG) SVAS = TRUE;
      else        SSGD = TRUE;
    }
    CTYD = IPNT = OPNT = CPNT = FALSE;
    
    RT[tvfix]=FALSE;
    AT[tvfix]=T[tvfix] && (AKTPRG==0);
    
    if (SVAS) {
      for (i=0; i<FC_MAX; i++) {
        VASR[i] = 1;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal stappenraster SLAAF                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_stappenraster(          /* datum 12-04-2003 - 24-09-2005                                    */
    bool ssync)
  {
    count i;
    
    RT[tvfix] |= FIXATIE;
    if (ssync && !SVAS && AKTPRG && !OMSCH
        && !RT[tvfix] && !T[tvfix]) {
      /* Start synchronisatie                                                                               */
      /* --------------------                                                                               */
      Sverschil = Sversnel = Straag = Svsnel = Sstart = Sblok = Sdubb = FALSE;
      
      /* Bepaal "vooruitlopen" t.o.v. het stappenraster                                                     */
      /* ----------------------------------------------                                                     */
      if (STAP>=CPNT) { Sverschil = STAP - CPNT; }
      else       { Sverschil = CTYD + STAP - CPNT; }
      
      /* Als "vooruitlopen" <= CTYD dan vertragen stappenraster                                             */
      /* ------------------------------------------------------                                             */
      if ((CTYD/2)>=Sverschil) { Straag = TRUE; Sblok = FALSE; }
      else {
        
        /* Bepaal "achterlopen" t.o.v. het stappenraster                                                    */
        /* ---------------------------------------------                                                    */
        Sverschil = CTYD - Sverschil;
        
        /* Als "achterlopen" <= 10% v/d CTYD dan versnellen stappenraster                                   */
        /* --------------------------------------------------------------                                   */
        if ((CTYD/10)>=Sverschil) { Svsnel = TRUE; Sversnel = 10; }
        else {
          
          /* Herstart stappenraster indien vertragen EN versnellen ongewenst                                */
          /* ---------------------------------------------------------------                                */
          Sstart    = TRUE;
          Sverschil = FALSE;
        }
      }
    }
    
    if (TS && Straag) {                 /* Vertraag stappenraster                                           */
      if (Sverschil > 0) {
        Sblok = !Sblok;
        if (Sblok) Sverschil--;
      } else { Sverschil = Straag = Sblok = FALSE; }
    }
    
    if (TS && Svsnel) {                 /* Versnel stappenraster                                            */
      if (Sverschil > 0) {
        Sversnel++;
        if (Sversnel >= 10) {
          Sverschil--;
          Sdubb    = TRUE;
          Sversnel = 1;
        }
      } else { Sverschil = Sversnel = Svsnel = Sdubb = FALSE; }
    }
    
    if (SVAS || OMSCH) {                /* ALTIJD reset synchronisatie bij start VAS                        */
      if (SVAS) STAP  = IPNT;
      Sverschil = Sversnel = Straag = Svsnel = Sstart = Sblok = Sdubb = FALSE;
    }
    if (!FIXATIE) {
      if ( TS && !SVAS && (!Sblok || !Straag))  STAP++;
      if (!TS &&            Sdubb &&  Svsnel) { STAP++;
        Sdubb = FALSE;
        N_ts  = TRUE;
      } else { N_ts = FALSE; }
    } else { N_ts = FALSE; }
    
    if (STAP>CTYD) STAP = 1;
    
    if (!AKTPRG) {                      /* ALTIJD reset synchronisatie bij einde VAS                        */
      STAP   = FALSE;                   /* ... OF fixatie                                                   */
      Sverschil = Sversnel = Straag = Svsnel = Sstart = Sblok = Sdubb = FALSE;
    }
    if (FIXATIE) {
      Sverschil = Sversnel = Straag = Svsnel = Sstart = Sblok = Sdubb = FALSE;
    }
    
    if (Sstart) {
      STAP   = CPNT;
      Sstart = FALSE;
      SVAS   = TRUE;
      for (i=0; i<FC_MAX; i++) {
        VASR[i] = 1;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal MKODE MASTER                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bepaal_MKODE(void)
  {
    MKODE = 0;
    
    if (cpc_prog_vas9) MKODE = 9;
    if (cpc_prog_vas8) MKODE = 8;
    if (cpc_prog_vas7) MKODE = 7;
    if (cpc_prog_vas6) MKODE = 6;
    if (cpc_prog_vas5) MKODE = 5;
    if (cpc_prog_vas4) MKODE = 4;
    if (cpc_prog_vas3) MKODE = 3;
    if (cpc_prog_vas2) MKODE = 2;
    if (cpc_prog_vas1) MKODE = 1;
    
    if ((_dvm <= 10) && (_dvm >  0)) MKODE = _dvm;
    if ((_dvm <= 20) && (_dvm > 10)) MKODE = 0;
    if ( _dvm >  20) MKODE = _dvm;
    
#ifdef schstreng_va
    if (SCH[schstreng_va]) MKODE = 0;
#endif
    
    if (MKODE == 1) {
#ifndef prmctijd1
      MKODE = 0;
#else
      if (PRM[prmctijd1] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 2) {
#ifndef prmctijd2
      MKODE = 0;
#else
      if (PRM[prmctijd2] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 3) {
#ifndef prmctijd3
      MKODE = 0;
#else
      if (PRM[prmctijd3] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 4) {
#ifndef prmctijd4
      MKODE = 0;
#else
      if (PRM[prmctijd4] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 5) {
#ifndef prmctijd5
      MKODE = 0;
#else
      if (PRM[prmctijd5] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 6) {
#ifndef prmctijd6
      MKODE = 0;
#else
      if (PRM[prmctijd6] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 7) {
#ifndef prmctijd7
      MKODE = 0;
#else
      if (PRM[prmctijd7] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 8) {
#ifndef prmctijd8
      MKODE = 0;
#else
      if (PRM[prmctijd8] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 9) {
#ifndef prmctijd9
      MKODE = 0;
#else
      if (PRM[prmctijd9] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 21) {
#ifndef prmctijd21
      MKODE = 0;
#else
      if (PRM[prmctijd21] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 22) {
#ifndef prmctijd22
      MKODE = 0;
#else
      if (PRM[prmctijd22] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 23) {
#ifndef prmctijd23
      MKODE = 0;
#else
      if (PRM[prmctijd23] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 24) {
#ifndef prmctijd24
      MKODE = 0;
#else
      if (PRM[prmctijd24] == 0) MKODE = 0;
#endif
    }
    if (MKODE == 25) {
#ifndef prmctijd25
      MKODE = 0;
#else
      if (PRM[prmctijd25] == 0) MKODE = 0;
#endif
    }
    
    if (MKODE == 26) {
#ifndef prmctijd26
      MKODE = 0;
#else
      if (PRM[prmctijd26] == 0) MKODE = 0;
#endif
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure VAS programma-keuze MASTER                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_progkeuze_master(       /* Fik090207                                                        */
    count tt1)                          /* Tijdvertr. omschakelen                                           */
  {
    count i;
    
    /* CKODE -> kode vanaf complex-coordinator                                                              */
    /* HKODE -> buffer CKODE t.b.v. start-puls                                                              */
    /* LKODE -> kode in lokale automaat (volgt vertraagt CKODE)                                             */
    
    RT[tt1] = FALSE;
    CKODE   = MKODE;
    if (HKODE != CKODE) { HKODE   = CKODE;
      RT[tt1] = TRUE;
    }
    if ((LKODE != CKODE) && !RT[tt1] && !T[tt1] || SPR) LKODE = CKODE;
    
    GEWPRG = LKODE;
    OMSCH = (AKTPRG != GEWPRG);
    
    if (((STAP == OPNT) || !OPNT) && OMSCH || SPR) {
      AKTPRG = GEWPRG;
      if (AKTPRG) SVAS = TRUE;
      else        SSGD = TRUE;
    }
    CTYD = IPNT = OPNT = CPNT = FALSE;
    
    RT[tvfix]=FALSE;
    AT[tvfix]=T[tvfix] && (AKTPRG==0);
    
    if (SVAS) {
      for (i=0; i<FC_MAX; i++) {
        VASR[i] = 1;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal stappenraster MASTER                                                                    */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_master_raster(          /* Fik060901                                                        */
    count tt_s)                         /* Synchronisatie-tyd                                               */
  {
    RT[tvfix] |= (FIXATIE ? 1 : 0);
    RT[tt_s] = FALSE;
    
    if (SVAS) STAP = IPNT;
    if (TS && !SVAS && !FIXATIE) STAP++;
    if (STAP>CTYD)   STAP = 1;
    
    if (!AKTPRG) {
      STAP = FALSE;
    } else {
      if (TS && (STAP == CPNT) && !OMSCH && !RT[tvfix] && !T[tvfix]) RT[tt_s] = TRUE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure initialiseer VAS regeling                                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void init_vas(void)                   /* Fik100226                                                        */
  {
    count i;
    
    BLOK  = 0;
    SBL   = FALSE;
    WACHT = FALSE;
    
    for (i=0; i<FC_MAX; i++) {
      SGD[i] = HKI[i] = FCA[i] = 0;
      POV[i] = PEL[i] = PEL2[i]= FTS[i]= 0;
      TTK[i] = TTP[i] = 0;
      if (G[i]) {
        if (KG[i] >= ALTM[i]) { TEG[i] = 0; }
        else                  { TEG[i] = ALTM[i] - KG[i]; }
      } else { TEG[i] = 0; }
      if (_GRN[i]) TEG[i] = (TTW[i]/10) + ALTM[i];
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal cyclische aanvragen                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_cyc_vas(                    /* Fik110314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cvas1,                        /* PRM[cvasxx1]                                                     */
    count cvas2)                        /* PRM[cvasxx2]                                                     */
  {
    HVAS[fc] &=(~BIT5);
    
    if (cvas1 != NG) {
      if ((aantal_VAS == 1) && (PRM[cvas1] >   1) ||
          (aantal_VAS == 2) && (PRM[cvas1] >   3) ||
          (aantal_VAS == 3) && (PRM[cvas1] >   7) ||
          (aantal_VAS == 4) && (PRM[cvas1] >  15) ||
          (aantal_VAS == 5) && (PRM[cvas1] >  31) ||
          (aantal_VAS == 6) && (PRM[cvas1] >  63) ||
          (aantal_VAS == 7) && (PRM[cvas1] > 127) ||
          (aantal_VAS == 8) && (PRM[cvas1] > 255) ||
          (aantal_VAS == 9) && (PRM[cvas1] > 511)) {
        PRM[cvas1] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (cvas2 != NG) {
      if ((aantal_VAS == 1) && (PRM[cvas2] >   1) ||
          (aantal_VAS == 2) && (PRM[cvas2] >   3) ||
          (aantal_VAS == 3) && (PRM[cvas2] >   7) ||
          (aantal_VAS == 4) && (PRM[cvas2] >  15) ||
          (aantal_VAS == 5) && (PRM[cvas2] >  31) ||
          (aantal_VAS == 6) && (PRM[cvas2] >  63) ||
          (aantal_VAS == 7) && (PRM[cvas2] > 127) ||
          (aantal_VAS == 8) && (PRM[cvas2] > 255) ||
          (aantal_VAS == 9) && (PRM[cvas2] > 511)) {
        PRM[cvas2] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (!BL[fc])
    {
      if (((VASR[fc] == 1) || (cvas2 == NG)) && (cvas1 != NG))
      {
        if ((AKTPRG==1) && (PRM[cvas1]&BIT0)) HVAS[fc] |= BIT5;
        if ((AKTPRG==2) && (PRM[cvas1]&BIT1)) HVAS[fc] |= BIT5;
        if ((AKTPRG==3) && (PRM[cvas1]&BIT2)) HVAS[fc] |= BIT5;
        if ((AKTPRG==4) && (PRM[cvas1]&BIT3)) HVAS[fc] |= BIT5;
        if ((AKTPRG==5) && (PRM[cvas1]&BIT4)) HVAS[fc] |= BIT5;
        if ((AKTPRG==6) && (PRM[cvas1]&BIT5)) HVAS[fc] |= BIT5;
        if ((AKTPRG==7) && (PRM[cvas1]&BIT6)) HVAS[fc] |= BIT5;
        if ((AKTPRG==8) && (PRM[cvas1]&BIT7)) HVAS[fc] |= BIT5;
        if ((AKTPRG==9) && (PRM[cvas1]>=256)) HVAS[fc] |= BIT5;
      }
      if (((VASR[fc] == 2) || (cvas1 == NG)) && (cvas2 != NG))
      {
        if ((AKTPRG==1) && (PRM[cvas2]&BIT0)) HVAS[fc] |= BIT5;
        if ((AKTPRG==2) && (PRM[cvas2]&BIT1)) HVAS[fc] |= BIT5;
        if ((AKTPRG==3) && (PRM[cvas2]&BIT2)) HVAS[fc] |= BIT5;
        if ((AKTPRG==4) && (PRM[cvas2]&BIT3)) HVAS[fc] |= BIT5;
        if ((AKTPRG==5) && (PRM[cvas2]&BIT4)) HVAS[fc] |= BIT5;
        if ((AKTPRG==6) && (PRM[cvas2]&BIT5)) HVAS[fc] |= BIT5;
        if ((AKTPRG==7) && (PRM[cvas2]&BIT6)) HVAS[fc] |= BIT5;
        if ((AKTPRG==8) && (PRM[cvas2]&BIT7)) HVAS[fc] |= BIT5;
        if ((AKTPRG==9) && (PRM[cvas2]>=256)) HVAS[fc] |= BIT5;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal cyclische aanvragen tijdens DVM                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_cyc_vas_dvm(                /* Fik110314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cdvm1,                        /* PRM[cdvmxx1]                                                     */
    count cdvm2)                        /* PRM[cdvmxx2]                                                     */
  
  {
    if (cdvm1 != NG)
    {
      if (PRM[cdvm1] > 63)
      {
        PRM[cdvm1] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (cdvm2 != NG)
    {
      if (PRM[cdvm2] > 63)
      {
        PRM[cdvm2] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    
    if (!BL[fc])
    {
      if (((VASR[fc] == 1) || (cdvm2 == NG)) && (cdvm1 != NG))
      {
        if ((AKTPRG==21) && (PRM[cdvm1]&BIT0)) HVAS[fc] |= BIT5;
        if ((AKTPRG==22) && (PRM[cdvm1]&BIT1)) HVAS[fc] |= BIT5;
        if ((AKTPRG==23) && (PRM[cdvm1]&BIT2)) HVAS[fc] |= BIT5;
        if ((AKTPRG==24) && (PRM[cdvm1]&BIT3)) HVAS[fc] |= BIT5;
        if ((AKTPRG==25) && (PRM[cdvm1]&BIT4)) HVAS[fc] |= BIT5;
        if ((AKTPRG==26) && (PRM[cdvm1]&BIT5)) HVAS[fc] |= BIT5;
      }
      if (((VASR[fc] == 2) || (cdvm1 == NG)) && (cdvm2 != NG))
      {
        if ((AKTPRG==21) && (PRM[cdvm2]&BIT0)) HVAS[fc] |= BIT5;
        if ((AKTPRG==22) && (PRM[cdvm2]&BIT1)) HVAS[fc] |= BIT5;
        if ((AKTPRG==23) && (PRM[cdvm2]&BIT2)) HVAS[fc] |= BIT5;
        if ((AKTPRG==24) && (PRM[cdvm2]&BIT3)) HVAS[fc] |= BIT5;
        if ((AKTPRG==25) && (PRM[cdvm2]&BIT4)) HVAS[fc] |= BIT5;
        if ((AKTPRG==26) && (PRM[cdvm2]&BIT5)) HVAS[fc] |= BIT5;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal aktuele VAS-parameters                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_parm(                   /* Fik050625                                                        */
    count ctyd,                         /* Cyclustijd                                                       */
    count insc,                         /* Inschakelpunt                                                    */
    count omsc,                         /* Omschakelpunt                                                    */
    count sync)                         /* Synchronisatiepunt                                               */
  {
    CTYD = PRM[ctyd];
    IPNT = PRM[insc];
    OPNT = PRM[omsc];
    CPNT = PRM[sync];
    
    if ((IPNT>CTYD) || !IPNT) IPNT = 1;
    if ( OPNT>CTYD)     OPNT = FALSE;
    if ((CPNT>CTYD) || !CPNT) CPNT = 1;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer VASR[]                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_corr_vasr(                  /* Fik100414                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tl1s,                         /* start primair 1e rea.                                            */
    count tl1e,                         /* einde primair 1e rea.                                            */
    count tl2s,                         /* start primair 2e rea.                                            */
    count tl2e)                         /* einde primair 2e rea.                                            */
  {
    if (VASR[fc] == 1) {
      if ((PRM[tl1s] == 0) || (PRM[tl1e] == 0) || (PRM[tl1s] == PRM[tl1e])) VASR[fc] = 2;
    }
    if (VASR[fc] == 2) {
      if ((PRM[tl2s] == 0) || (PRM[tl2e] == 0) || (PRM[tl2s] == PRM[tl2e])) VASR[fc] = 1;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal aktueel primair gebied                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_akt_vas(                    /* Fik120506                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tl1,                          /* einde uitstel                                                    */
    count tl2,                          /* start primair                                                    */
    count tl3,                          /* start verleng                                                    */
    count tl4,                          /* einde primair                                                    */
    count tl5)                          /* einde groen                                                      */
  {
    mulv SPRIM;
    mulv EPRIM;
    mulv VASX;
    mulv _kap;
    
    /* Bepaal max.groentijd VAS tbv KWC                                                                     */
    MAXG[fc] = 0;
    if ((PRM[tl2] > 0) && (PRM[tl4] > 0) && (PRM[tl2] <= CTYD) && (PRM[tl4] <= CTYD)) {
      if (PRM[tl4] >= PRM[tl2]) {
        MAXG[fc] = PRM[tl4] - PRM[tl2];
      } else {
        MAXG[fc] = CTYD -
                   PRM[tl2] + PRM[tl4];
      }
    }
    
    _kap = KAPM[fc];
    
    /* KAPM[fc] = ((MAXG[fc]*_kap)/10) / 10; Fik150328                                                      */
    if ((TGG_max[fc]/10) > KAPM[fc]) KAPM[fc] = TGG_max[fc]/10;
    if ((TFG_max[fc]/10) > KAPM[fc]) KAPM[fc] = TFG_max[fc]/10;
    if (KAPM[fc] > MAXG[fc]) KAPM[fc] = MAXG[fc];
    if (STAP == PRM[tl2]) HVAS[fc] &= (~BIT4); /* Reset versnelde realisatie                                */
    
    if (tl1 != NG) VAS1[fc] = PRM[tl1];
    else           VAS1[fc] = 0;
    if (HVAS[fc]&BIT4) { VAS2[fc] = STAP; }
    else        { VAS2[fc] = PRM[tl2]; }
    SPRIM    = PRM[tl2];
    EPRIM    = PRM[tl4];
    
    if ((tl3 != NG) && !(_FIL[fc]&BIT0)) VAS3[fc] = PRM[tl3];
    else                                 VAS3[fc] = 0;
    
    VAS4[fc] = PRM[tl4];
    if (tl5 != NG) VAS5[fc] = PRM[tl5];
    else           VAS5[fc] = 0;
    
    /* Bepaal primair gebied conform instellingen                                                           */
    /* ------------------------------------------                                                           */
    
    HVAS[fc] &= (~BIT7);
    if ((SPRIM != 0)  && (VAS4[fc] != 0) && (SPRIM != VAS4[fc])) {
      if (VAS4[fc]>SPRIM) {
        if ((STAP>=SPRIM) && (STAP<VAS4[fc])) HVAS[fc] |= BIT7;
      } else {
        if ((STAP>=SPRIM) || (STAP<VAS4[fc])) HVAS[fc] |= BIT7;
      }
    }
    
    if ((VAS2[fc] != 0) && (VAS4[fc] != 0) && (VAS2[fc] != VAS4[fc]) && !BL[fc]) {
      
      /* Bepaal primair gebied incl.vooruitrealiseren                                                       */
      /* --------------------------------------------                                                       */
      
      if (VAS4[fc]>VAS2[fc]) {          /* Start primair VOOR einde primair                                 */
        
        if ((STAP>=VAS2[fc]) && (STAP<VAS4[fc]) && !(HVAS[fc]&BIT3)) {
          HVAS[fc] |= BIT0;
          if ((STAP<VAS3[fc]) && (VAS3[fc]<=VAS4[fc]))
          { HVAS[fc] |=   BIT2 ; }
          else   { HVAS[fc] &= (~BIT2); }
        } else {
          HVAS[fc] &= (~BIT0);
          HVAS[fc] &= (~BIT2);
          HVAS[fc] &= (~BIT4);
        }
        
      } else {                          /* Start primair NA einde primair                                   */
        
        if (((STAP>=VAS2[fc]) || (STAP<VAS4[fc])) && !(HVAS[fc]&BIT3)) {
          HVAS[fc] |= BIT0;
          if ((VAS3[fc]!=0) &&
              ((STAP>=VAS2[fc]) && ((STAP<VAS3[fc])
                  || (VAS3[fc]<=VAS4[fc]))
              ||  (STAP<=VAS4[fc]) &&  (STAP<VAS3[fc])
              && (VAS3[fc]<=VAS4[fc])))
          { HVAS[fc] |=   BIT2 ; }
          else   { HVAS[fc] &= (~BIT2); }
        } else {
          HVAS[fc] &= (~BIT0);
          HVAS[fc] &= (~BIT2);
          HVAS[fc] &= (~BIT4);
        }
      }
      
      /* Bepaal blokkeer overgang naar groen                                                                */
      /* -----------------------------------                                                                */
      
      if ((VAS1[fc] != 0) && (VAS1[fc] != VAS4[fc])) {
        
        if (VAS1[fc]>VAS4[fc]) {        /* Start uitstel VOOR einde uitstel                                 */
          
          if ((STAP>=VAS4[fc]) && (STAP<VAS1[fc])
              || (HVAS[fc]&BIT3)) { HVAS[fc] |=   BIT1 ;
          } else {               HVAS[fc] &= (~BIT1); }
          
        } else {                        /* Start uitstel NA einde uitstel                                   */
          
          if ((STAP>=VAS4[fc]) || (STAP<VAS1[fc])
              || (HVAS[fc]&BIT3)) { HVAS[fc] |=   BIT1 ;
          } else {               HVAS[fc] &= (~BIT1); }
        }
      } else { HVAS[fc] &= (~BIT1); }
      
      /* Bepaal forceer overgang naar rood                                                                  */
      /* ---------------------------------                                                                  */
      
      if (VAS1[fc] != 0) VASX = VAS1[fc]; /* Einde NIET-groen-gebied                                        */
      else       VASX = VAS2[fc];       /* afh. van def.VAS1[fc]                                            */
      
      if ((VAS5[fc] != 0) && (VASX != VAS5[fc])) {
        
        if (VASX>VAS5[fc]) {            /* Start forceer VOOR einde forceer                                 */
          
          if ((STAP>=VAS5[fc]) && (STAP<VASX)
              || (HVAS[fc]&BIT3) && !G[fc] && !_GRN[fc]) { HVAS[fc] |=   BIT6 ;
          } else {                           HVAS[fc] &= (~BIT6); }
          
        } else {                        /* Start forceer NA einde forceer                                   */
          
          if ((STAP>=VAS5[fc]) || (STAP<VASX)
              || (HVAS[fc]&BIT3) && !G[fc] && !_GRN[fc]) { HVAS[fc] |=   BIT6 ;
          } else {                           HVAS[fc] &= (~BIT6); }
        }
      } else { HVAS[fc] &= (~BIT6); }
      
    } else { HVAS[fc] &= (~BIT0);       /* Indien Start en/of einde primair NIET                            */
      HVAS[fc] &= (~BIT1);              /* gedefinieerd dan richting ALTIJD als                             */
      HVAS[fc] &= (~BIT2);              /* gerealiseerd beschouwen                                          */
      
      if (!BL[fc] || (SPRIM == 0) || (EPRIM == 0) || (SPRIM == EPRIM)) {
        HVAS[fc] |= BIT3 ;
      } else {
        if ((SPRIM < EPRIM) &&  (STAP >= SPRIM) && (STAP < EPRIM) ) HVAS[fc] |= BIT3;
        if ((SPRIM > EPRIM) && ((STAP >= SPRIM) || (STAP < EPRIM))) HVAS[fc] |= BIT3;
        if ((STAP  < SPRIM) &&  (STAP + TTR[fc] >= SPRIM))          HVAS[fc] |= BIT3;
        if (STAP  >=SPRIM) {
          SPRIM += CTYD;
          if ((STAP  < SPRIM) && (STAP + TTR[fc] >= SPRIM)) HVAS[fc] |= BIT3;
        }
      }
      
      HVAS[fc] &= (~BIT4);
      HVAS[fc] &= (~BIT5);
      HVAS[fc] &= (~BIT6);
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal primaire realisatie                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_prim_vas(void)              /* Fik120207                                                        */
  {
    count i,j,k;
    mulv  ontruim;
    bool  ppv;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    if (AKTPRG) {
      
      for (i=0; i<FC_MAX; i++) {
        ppv = FALSE;
        
        /* Bepaal overslag bij start VAS-programma                                                          */
        /* ---------------------------------------                                                          */
        if (SVAS) {
          if (((HVAS[i]&BIT0) || ((TTP[i]+MAXG[i])<=(TTR[i]+ALTM[i]))) && !G[i] && !_GRN[i]) {
            HVAS[i] |= BIT3;
            TTP[i]  = 255;
            X[i] |= BIT1;               /* Registratie overslag                                             */
          }
          if (!(HVAS[i]&BIT3)) {
            for (j=0; j<GKFC_MAX[i]; j++) {
              k = TO_pointer[i][j];
              
              if (G[k] || _GRN[k]) {    /* Conflict is groen                                                */
                
                /* Bepaal overslag door alternatief conflict                                                */
                /* -----------------------------------------                                                */
                if (TO_max[k][i]>=0) { ontruim = (TGL_max[k]+TO_max[k][i])/10 + TEG[k]; }
                else                 { ontruim = TEG[k]; }
                if (TTP[i]<=ontruim) {
                  HVAS[i] |= BIT3;
                  TTP[i]  = 255;
                  X[i] |= BIT1;         /* Registratie overslag                                             */
                }
              }
            }
            
            for (k=0; k<FC_MAX; k++) {
              if (TMPc[i][k]) {
                
                if (G[k] || _GRN[k]) {  /* Conflict is groen                                                */
                  
                  /* Bepaal overslag door alternatief conflict                                              */
                  /* -----------------------------------------                                              */
                  ontruim = (TGL_max[k]+TMPo[k][i])/10 + TEG[k];
                  if (TTP[i]<=ontruim) {
                    HVAS[i] |= BIT3;
                    TTP[i]  = 255;
                    X[i] |= BIT1;       /* Registratie overslag                                             */
                  }
                }
              }
            }
            
          }
        }
        
        /* Bepaal afkappen/blokkeren conflicterende groenrealisaties                                        */
        /* ---------------------------------------------------------                                        */
        if (!(HVAS[i]&BIT3)) {
          ppv = TRUE;
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
            
            if (G[k] || _GRN[k]) {      /* Conflict is groen                                                */
              
              /* Bepaal overslag door alternatief conflict                                                  */
              /* -----------------------------------------                                                  */
              if (TO_max[k][i]>=0) { ontruim =((TGL_max[k]+TO_max[k][i])/10)
                  +TEG[k]; }
              else       { ontruim = TEG[k]; }
              if ((TTP[i]<=ontruim) && (TTP[k]>TTP[i])
                  &&   R[i] && !TRG[i] && !(HVAS[i]&BIT5)
                  &&  !(FCA[i]&BIT0)
                  &&  !(FCA[i]&BIT1) && !_GRN[i]
                  && (!A[i] || (A[i]&BIT7))) {
                HVAS[i] |= BIT3;
                TTP[i]  = 255;
                X[i] |= BIT1;           /* Registratie overslag                                             */
              }
              
              if (!(HVAS[i]&BIT3)) {
                if (TO_max[k][i]>=0) { ontruim =(TGL_max[k]+TO_max[k][i])/10; }
                else         { ontruim = FALSE; }
                
                /* Bepaal tijd tot afkappen door primair conflict                                           */
                /* ----------------------------------------------                                           */
                if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                    ||  (FCA[i]&BIT0)
                    ||  (FCA[i]&BIT1)) {
                  if (TTP[i]<=ontruim)        { TTK[k] = FALSE; }
                  else if ((TTP[i]-ontruim)<TTK[k]) { TTK[k] = TTP[i]-ontruim;}
                }
                
                if ((TTP[i]<=ontruim) && ((TTP[k]>TTP[i])
                      ||  (TTP[k]==0))) {
                  if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                      ||  (FCA[i]&BIT0)
                      ||  (FCA[i]&BIT1)) {
                    BAR[k] |= TRUE;
                    Z[k] |= BIT1;
                  }
                } else { ppv = FALSE; }
                
                if (FCA[k]&BIT0) {      /* Fic.aanv. gekoppelde.vtg. aanwezig                               */
                  
                  if (TO_max[k][i]>=0) {ontruim =(TGL_max[k]+TO_max[k][i])/10;}
                  else           {ontruim = FALSE; }
                  ontruim = ontruim + TTR[k] + ALTM[k];
                  if ((TTP[i]<=ontruim) && ((TTP[k]>TTP[i])
                        ||  (TTP[k]==0))) {
                    if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                        ||  (FCA[i]&BIT0)
                        ||  (FCA[i]&BIT1)) {
                      BAR[k] |= TRUE;
                    }
                  } else { ppv = FALSE; }
                }
                
              }
            } else {                    /* Conflict is NIET groen                                           */
              
              if (TO_max[k][i]>=0) { ontruim =((TGL_max[k]+TO_max[k][i])/10)
                  +ALTM[k]; }
              else       { ontruim = ALTM[k]; }
              if ((TTP[i]<=ontruim+TTR[k]) && (TTP[k]>TTP[i])) {
                if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                    ||  (FCA[i]&BIT0)
                    ||  (FCA[i]&BIT1)) {
                  BAR[k] |= TRUE;
                }
              } else { ppv = FALSE; }
            }
          }
          
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k]) {
              
              if (G[k] || _GRN[k]) {    /* Conflict is groen                                                */
                
                /* Bepaal overslag door alternatief conflict                                                */
                /* -----------------------------------------                                                */
                ontruim =(TGL_max[k]/10) + (TMPo[k][i]/10) + TEG[k];
                if ((TTP[i]<=ontruim) && (TTP[k]>=TTP[i])
                    &&   R[i] && !TRG[i] && !(HVAS[i]&BIT5)
                    &&  !(FCA[i]&BIT0)
                    &&  !(FCA[i]&BIT1) && !_GRN[i]
                    && (!A[i] || (A[i]&BIT7))) {
                  HVAS[i] |= BIT3;
                  TTP[i]  = 255;
                  X[i] |= BIT1;         /* Registratie overslag                                             */
                }
                
                if (!(HVAS[i]&BIT3)) {
                  ontruim = (TGL_max[k]+TMPo[k][i])/10;
                  
                  /* Bepaal tijd tot afkappen door primair conflict                                         */
                  /* ----------------------------------------------                                         */
                  if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                      ||  (FCA[i]&BIT0)
                      ||  (FCA[i]&BIT1)) {
                    if (TTP[i]<=ontruim)              { TTK[k] = FALSE; }
                    else if ((TTP[i]-ontruim)<TTK[k]) { TTK[k] = TTP[i]-ontruim;}
                  }
                  
                  if ((TTP[i]<=ontruim) && ((TTP[k]>=TTP[i])
                        ||  (TTP[k]==0))) {
                    if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                        ||  (FCA[i]&BIT0)
                        ||  (FCA[i]&BIT1)) {
                      BAR[k] |= TRUE;
                      Z[k] |= BIT1;
                    }
                  } else { ppv = FALSE; }
                  
                  if (FCA[k]&BIT0) {    /* Fic.aanv. gekoppelde.vtg. aanwezig                               */
                    
                    ontruim = (TGL_max[k]+TMPo[k][i])/10;
                    ontruim = ontruim + TTR[k] + ALTM[k];
                    if ((TTP[i]<=ontruim) && ((TTP[k]>=TTP[i])
                          ||  (TTP[k]==0))) {
                      if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                          ||  (FCA[i]&BIT0)
                          ||  (FCA[i]&BIT1)) {
                        BAR[k] |= TRUE;
                      }
                    } else { ppv = FALSE; }
                  }
                  
                }
              } else {                  /* Conflict is NIET groen                                           */
                
                if (dcf_gelijk(i,k) || (dcf_sync(i,k) != NG) || (dcf_sync(k,i) != NG)) {
                  if ((TTP[i]<=TTR[k]) && (TTP[k]>=TTP[i])) {
                    if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                        ||  (FCA[i]&BIT0)
                        ||  (FCA[i]&BIT1)) {
                      BAR[k] |= TRUE;
                    }
                  } else { ppv = FALSE; }
                  
                } else {
                  ontruim = (TGL_max[k]+TMPo[k][i])/10 + ALTM[k];
                  if ((TTP[i]<=ontruim+TTR[k]) && (TTP[k]>=TTP[i])) {
                    if (G[i] || _GRN[i] || GL[i] || TRG[i] || A[i] || (HVAS[i]&BIT5)
                        ||  (FCA[i]&BIT0)
                        ||  (FCA[i]&BIT1)) {
                      BAR[k] |= TRUE;
                    }
                  } else { ppv = FALSE; }
                }
              }
            }
          }
          
          for (j=0; j<FKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
            if (TTP[i]>TTP[k]) ppv = FALSE;
          }
          
          for (k=0; k<FC_MAX; k++) {
            if (TMPc[i][k] || TMPf[i][k]) {
              if (TTP[i]>=TTP[k]) ppv = FALSE;
            }
          }
        }
        
        /* Bepaal primaire realisatie                                                                       */
        /* --------------------------                                                                       */
        if ((ppv || (A[i]&BIT4)) && !(HVAS[i]&BIT0) && !(HVAS[i]&BIT3)) {
          if (G[i]) {
            HVAS[i] |=  BIT4;
            RW[i] |=  BIT1;
            FM[i]  =  FALSE;
            A[i] &=(~BIT4);
          }
          else { if (RA[i]) A[i] |= BIT4;
          }
        } else { A[i] &= (~BIT4);       /* Resetten indien alsnog RR[i]                                     */
        }
        
        if ((HVAS[i]&BIT0) && !(HVAS[i]&BIT3) && R[i] && !TRG[i]) {
          if (HVAS[i]&BIT5) {    A[i] |= BIT2; A[i] &= (~BIT3); A[i] &= (~BIT6); }
          if (A[i])         {    B[i] |= BIT1; }
          else              { HVAS[i] |= BIT3; }
        }
        if ((HVAS[i]&BIT0) && (MG[i] && !RW[i] && !(HVAS[i]&BIT2) || EG[i]))
          HVAS[i] |= BIT3;
        if  (HVAS[i]&BIT3) TTP[i] = 255;
        
        /* Bepaal blokkeer overgang naar groen                                                              */
        /* -----------------------------------                                                              */
        if ((HVAS[i]&BIT1) && !G[i] && !_GRN[i]) {
          BAR[i]  = TRUE;
          X[i] |= BIT2;
        }
        
        /* Bepaal forceer overgang naar rood                                                                */
        /* ---------------------------------                                                                */
        if (HVAS[i]&BIT6) {
          if (!G[i] && !_GRN[i] && !ppv) {
            BAR[i]  = TRUE;
            X[i] |= BIT2;
          }
          if (G[i] && (KG[i]>=ALTM[i]))
          {  Z[i] |= BIT2; }
        }
        
        /* Bepaal vasthouden in wachtgroen                                                                  */
        /* -------------------------------                                                                  */
        if ((HVAS[i]&BIT2) && G[i])  {
          RW[i] |= BIT1;
          YW[i] |= BIT1;
        }
        
        /* Reset realisatie-geheugen op einde primaire gebied                                               */
        /* --------------------------------------------------                                               */
        if ((STAP==VAS4[i]) && (TS || N_ts)) {
          HVAS[i] &= (~BIT3);
          HVAS[i] &= (~BIT4);
          if (VASR[i] == 1) VASR[i] = 2;
          else              VASR[i] = 1;
        }
      }
      
      /* Doorschrijven primaire realisaties; 2x ivm nesting                                                 */
      for (i = 0; i <= 1; i++)
      {
        dcf_vs_vas_prm();
        dcf_gs_vas_prm();
        proc_prm_vtgvas();
      }
      
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TEG##                                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bteg_vas(void)              /* Fik110801                                                        */
  {
    count i;
    
    /* corrigeer ALTM[] ivm deelconflicten synchroonstart                                                   */
    dcf_vs_corr_alt();
    
    for (i=0; i<FC_MAX; i++)
    {
      if (G[i])
      {
        if (TS && (TEG[i]>0)) TEG[i]--;
        if (MG[i] && (KG[i] >= (TGG_max[i]/10))) TEG[i] = FALSE;
        if (SG[i]) TEG[i] = ALTM[i];
      }
      else
      {
        TEG[i] = FALSE;
      }
      
      if (_GRN[i]) TEG[i] = (TTW[i]/10) + ALTM[i];
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal TTP[]                                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bttp_vas(void)
  {
    count i;
    
    for (i=0; i<FC_MAX; i++) {
      
      if (AKTPRG) {
        if (HVAS[i]&BIT0) { TTP[i] = FALSE; }
        else {
          if ((TS || N_ts || SVAS) && !(HVAS[i]&BIT3)) {
            if (STAP<VAS2[i]) { TTP[i] = VAS2[i] - STAP; }
            else        { TTP[i] = CTYD    - STAP + VAS2[i]; }
          }
        }
      } else { TTP[i] = 255; }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal toestemming alternatieve realisatie                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_altt_vas(void)              /* Fik120207                                                        */
  {
    count i,j,k;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    for (i=0; i<FC_MAX; i++) {
      
      if (!BAR[i] && R[i] && !TRG[i] && !_GRN[i]) {
        
        for (j=0; j<GKFC_MAX[i]; j++) { /* Primaire conflicten                                              */
          k = TO_pointer[i][j];
          if (B[k] || RA[k] ||  G[k] && !(YM[k]&BIT2) && !MG[k]
              || MG[k] && !(YM[k]&BIT2) &&  RW[k]) BAR[i] = TRUE;
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k]) {
            if (B[k] || RA[k] ||  G[k] && !(YM[k]&BIT2) && !MG[k]
                || MG[k] && !(YM[k]&BIT2) &&  RW[k]) BAR[i] = TRUE;
          }
        }
        
        for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) { /* Fictieve conflicten                                    */
          k = TO_pointer[i][j];
          if (B[k] || RA[k]) BAR[i] = TRUE;
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPf[i][k]) {
            if (B[k] || RA[k]) BAR[i] = TRUE;
          }
        }
        
      } else { if (!G[i] && !_GRN[i]) BAR[i] = TRUE; }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure alternatieve realisatie                                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_altr_vas(void)              /* Fik120207                                                        */
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
    
    /* Corrigeer BAR[]; 2x ivm nesting                                                                      */
    for (i = 0; i <= 1; i++)
    {
      dcf_vs_vas_bar();
      dcf_gs_vas_bar();
      proc_bar_vtgvas();
    }
    
    for (i=0; i<FC_MAX; i++) {
      
      if (!BAR[i] && R[i] && !TRG[i] && A[i] && !(A[i]&BIT7)) {
        
        altr = TRUE;
        for (j=0; j<FKFC_MAX[i]; j++) {
          k = TO_pointer[i][j];
          if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i])) altr = FALSE;
        }
        
        for (k=0; k<FC_MAX; k++) {
          if (TMPc[i][k] || TMPf[i][k]) {
            if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i])) altr = FALSE;
          }
        }
        
        if (altr && !somfbk(i) && AKTPRG) B[i] |= BIT2;
      }
      if (RA[i] && !_GRN[i] && (BAR[i] || somfbk(i)) && !(HVAS[i]&BIT0)) RR[i] = TRUE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal maximumgroen                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_maxgr_vas(void)             /* Fik110801                                                        */
  {
    count i;
    count verw=0;
    
    for (i=0; i<FC_MAX; i++) {
      
      TTK[i] = 255;                     /* Initialisatie TTK                                                */
      
      /* Bepaal aanhouden verlenggroen                                                                      */
      
      if (G[i] && !SG[i] && !FG[i]) {
        verw = 0;
        if (YV[i]&BIT1) verw = 1;
        if (YV[i]&BIT2) verw = 2;
        if (WG[i] && !EFG[i] && !(YV[i]&BIT0) && !(YV[i]&BIT1)) verw = 2;
        
        if (verw == 0) {
          if (MK[i]&BIT0)   YV[i] |= BIT0;
          else verw++;
        }
        
        if (verw == 1) {
          if (MK[i]&BIT1) { YV[i] = FALSE; YV[i] |= BIT1; }
          else verw++;
        }
        
        if (verw == 2) {
          if ((MK[i]&BIT2) ||
              (MK[i]&BIT3) || WG[i]) { YV[i] = FALSE; YV[i] |= BIT2; }
          else verw++;
        }
        
        if ((verw > 2) || (KGCA[i]>=PRIM[i]) && !(HVAS[i]&BIT0)) FM[i] = TRUE;
        if ((_FIL[i]&BIT0) && (KG[i]>=_DOS[i]))                  FM[i] = TRUE;
      }
      
      FCA[i] &= (~BIT0);                /* reset FCA[]                                                      */
      FCA[i] &= (~BIT1);
    }
    
    /* Bepaal FCA[]; 2x ivm nesting                                                                         */
    for (i = 0; i <= 1; i++)
    {
      dcf_vs_vas_fca();
      dcf_gs_vas_fca();
      proc_fca_vtgvas();
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure mee-realisatie tijdens VAS                                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_mee_rea_vas(                /* Fik100206                                                        */
    count fc1,                          /* fasecyclus MEE                                                   */
    count fc2,                          /* fasecyclus MET                                                   */
    count sch)                          /* schakelaar                                                       */
  {
    if (R[fc1] && !TRG[fc1] && !K[fc1] && !BL[fc1] && !somfbk(fc1) && !somgk(fc1) && SCH[sch] && !BAR[fc1] 
        && !(_FIL[fc1]&BIT0) 
        && (SG[fc2] || FG[fc2] || SWG[fc2])) {
      if ((SG[fc2] || FG[fc2]) &&  R[fc1] && !A[fc1]) A[fc1] |= BIT2;
      if ((SG[fc2] || FG[fc2]) && RV[fc1] && !B[fc1]) B[fc1] |= BIT2;
      RR[fc1] = FALSE;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure wachtstandgroen                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_wgr_vas(                    /* Fik020313                                                        */
    count fc,                           /* Fik120207                                                        */
    count wgr)
  {
    count j,k;
    bool  wacht = TRUE;
    bool  t_wgr = TRUE;
    
    /* update temporary conflictmatrix                                                                      */
    NEST = TRUE;
    while (NEST)
    {
      NEST = FALSE;
      dcf_vs_confl();
      dcf_gs_confl();
    }
    
    if (wgr == NG) t_wgr = FALSE;
    else {
      if (PRM[wgr] > 2) {
        PRM[wgr] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (wgr != NG) {
      if ((PRM[wgr] == 0) || (PRM[wgr] == 1) && R[fc] || (_FIL[fc]&BIT0)) t_wgr = FALSE;
    }
    
    if (!t_wgr || GL[fc] ||  TRG[fc] ||  K[fc]
        || (R[fc] && (BAR[fc] || RR[fc]))) wacht = FALSE;
    
    if (wacht) {
      for (j=0; j<FKFC_MAX[fc]; j++) {  /* Primaire en/of fictieve conflicten                               */
        k = TO_pointer[fc][j];
        if (A[k] && !BL[k] || B[k] || RA[k] || G[k] || GL[k] || TRG[k]) wacht = FALSE;
      }
      for (k=0; k<FC_MAX; k++) {
        if (TMPc[fc][k] || TMPf[fc][k]) {
          if (A[k] && !BL[k] || B[k] || RA[k] || G[k] || GL[k] || TRG[k]) wacht = FALSE;
        }
      }
    }
    if (wacht) {
      if (G[fc]) {                      /* Wachtstand-richting is groen                                     */
        
        A[fc] |= BIT5;
        YW[fc] |= BIT5;
        RW[fc] |= BIT5;
        FM[fc] = FALSE;
        
      } else {                          /* Wachtstand-richting is rood                                      */
        
        A[fc] |= BIT5;
      }
    } else { if (!B[fc] && !RA[fc]) A[fc] &= (~BIT5); }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie meeverlenggroen                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  bool hf_mvg_vas(void)                 /* Fik050925                                                        */
  {                                     /* Fik180309                                                        */
    count i;
    
    for (i=0; i<FC_MAX; i++) {
      
#ifndef _VAS_OV
      
      if (R[i] && A[i] && !BL[i] || _GRN[i] || G[i] && (!MG[i] || RW[i] || MK[i] || (AVR_GEBIED[i] > 0) || HLPD[i][0]) 
          && (!WG[i] || !(YW[i]&BIT5)))
      return TRUE;
      
#else                                   /* *** ifndef -> _VAS_OV                                            */
      
      if (R[i] && A[i] && !BL[i] || _GRN[i] || G[i] && (!MG[i] || RW[i] || MK[i] || (AVR_GEBIED[i] > 0) || 
          HLPD[i][0] || (HVAS[i]&PRI)) 
          && (!WG[i] || !(YW[i]&BIT5)))
      return TRUE;
      
#endif                                  /* *** ifndef -> _VAS_OV                                            */
      
    }
    return FALSE;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Hulpfunctie bepaal variabel meeverlenggroen                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  bool var_mg_vas(                      /* Fik110801                                                        */
    count fc)
  {
    bool mvg_extra = TRUE;
    count j,k;
    
    if (Z[fc]) mvg_extra = FALSE;
    
    if (MG[fc] && mvg_extra) {
      for (j=0; j<GKFC_MAX[fc]; j++) {
        k = TO_pointer[fc][j];
        if (RA[k] && (TTC[k]<=((TGL_max[fc]+TO_max[fc][k])/10))) mvg_extra = FALSE;
        if (RA[k] && (KRVG[k] > 10)) mvg_extra = FALSE;
      }
    }
    
    if (MG[fc] && mvg_extra) {
      for (k=0; k<FC_MAX; k++) {
        if (TMPc[fc][k]) {
          if (RA[k] && (TTC[k]<=((TGL_max[fc]+TMPo[fc][k])/10))) mvg_extra = FALSE;
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
  void proc_mvg_vas(                    /* Fik170701                                                        */
    count fc,                           /* fasecyclus                                                       */
    count mvg)                          /* prm mvg                                                          */
  {
    bool t_mvg = TRUE;
    bool t_gcf = FALSE;
    count i,j,k;
    
    if (mvg == NG) t_mvg = FALSE;
    else {
      if (PRM[mvg] > 2) {
        PRM[mvg] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (mvg != NG) {
      if ((PRM[mvg] == 0) || (PRM[mvg] == 1) && !MK[fc] && (AVR_GEBIED[fc] == 0)) t_mvg = FALSE;
    }
    
    if (mvg != NG) {
      if (t_mvg && (PRM[mvg] == 2)) {
        for (i=0; i<FC_MAX; i++) {
          if ((TO_max[fc][i] == NG) && !TMPc[fc][i] && !TMPc[i][fc]) {
            MV_matrix[fc][i] = TRUE;
          }
        }
      }
    }
    
    if (!(HKII[fc]&BIT3)) {             /* de volgrichting is NIET groen                                    */
      
      if (t_mvg && (!somfbk(fc) || var_mg_vas(fc)) && !(_FIL[fc]&BIT0)) YM[fc] |= BIT0;
      
    } else {                            /* de volgrichting is WEL groen                                     */
      
      t_gcf = FALSE;
      
#ifndef _VAS_OV
      
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || _GRN[i]
            || G[i] && (!MG[i] || RW[i] || MK[i] || (AVR_GEBIED[i] > 0) || HLPD[i][0]) && !(YM[i]&BIT2)
            && (!WG[i] || !(RW[i]&BIT5))) t_gcf = TRUE;
      }
      
#else                                   /* *** ifndef -> _VAS_OV                                            */
      
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || _GRN[i]
            || G[i] && (!MG[i] || RW[i] || MK[i] || (AVR_GEBIED[i] > 0) || HLPD[i][0] || (HVAS[i]&PRI)) && !(YM[i]&BIT2)
            && (!WG[i] || !(RW[i]&BIT5))) t_gcf = TRUE;
      }
      
#endif                                  /* *** ifndef -> _VAS_OV                                            */
      
      for (j=0; j<GKFC_MAX[fc]; j++) {
        k = TO_pointer[fc][j];
        if (R[k] && !TRG[k] && A[k] && !BAR[k]) t_gcf = FALSE;
      }
      
      for (k=0; k<FC_MAX; k++) {
        if (TMPc[fc][k]) {
          if (R[k] && !TRG[k] && A[k] && !BAR[k]) t_gcf = FALSE;
        }
      }
      
      if (t_mvg && !somfbk(fc) && !(_FIL[fc]&BIT0) && t_gcf) YM[fc] |= BIT0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure meeverlengen overige voorwaarden                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_mee_mvg_vas(                /* Fik100414                                                        */
    count fc1,                          /* fasecyclus van                                                   */
    count fc2,                          /* fasecyclus met                                                   */
    count sch)                          /* schakelaar                                                       */
  {
    bool t_gcf = FALSE;
    count i,j,k;
    
    if (SCH[sch] && !(_FIL[fc1]&BIT0)) {
      MV_matrix[fc1][fc2] = TRUE;
    }
    
    if (!(HKII[fc1]&BIT3)) {            /* de volgrichting is NIET groen                                    */
      
      if (G[fc2] && SCH[sch] && (!somfbk(fc1) || var_mg_vas(fc1)) && !(_FIL[fc1]&BIT0)) YM[fc1] |= BIT0;
      
    } else {                            /* de volgrichting is WEL groen                                     */
      
      t_gcf = FALSE;
      
#ifndef _VAS_OV
      
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || _GRN[i]
            || G[i] && (!MG[i] || RW[i] || MK[i] || (AVR_GEBIED[i] > 0) || HLPD[i][0]) && !(YM[i]&BIT2)
            && (!WG[i] || !(RW[i]&BIT5))) t_gcf = TRUE;
      }
      
#else                                   /* *** ifndef -> _VAS_OV                                            */
      
      for (i=0; i<FC_MAX; i++) {
        if (R[i] && A[i] && !BL[i] || _GRN[i]
            || G[i] && (!MG[i] || RW[i] || MK[i] || (AVR_GEBIED[i] > 0) || HLPD[i][0] || (HVAS[i]&PRI)) && !(YM[i]&BIT2)
            && (!WG[i] || !(RW[i]&BIT5))) t_gcf = TRUE;
      }
      
#endif                                  /* *** ifndef -> _VAS_OV                                            */
      
      for (j=0; j<GKFC_MAX[fc1]; j++) {
        k = TO_pointer[fc1][j];
        if (R[k] && !TRG[k] && A[k] && !BAR[k]) t_gcf = FALSE;
      }
      
      if (G[fc2] && SCH[sch] && !somfbk(fc1) && !(_FIL[fc1]&BIT0) && t_gcf) YM[fc1] |= BIT0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure reset buffers OV ingreep tijdens halfstar bedrijf                                              */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_res_bus(                /* Fik100517                                                        */
    count awt1,                         /* aanwezigheidsduur 1e bus                                         */
    count awt2,                         /* aanwezigheidsduur 2e bus                                         */
    count awt3,                         /* aanwezigheidsduur 3e bus                                         */
    count stp1,                         /* stiptheidscode 1e bus                                            */
    count stp2,                         /* stiptheidscode 2e bus                                            */
    count stp3)                         /* stiptheidscode 3e bus                                            */
  {
    if (AKTPRG > 0) {
      MM[awt1] = MM[awt2] = MM[awt3] = 0;
      MM[stp1] = MM[stp2] = MM[stp3] = 0;
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure reset buffers peloton koppeling tijdens halfstar bedrijf                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_vas_res_pel(                /* Fik100314                                                        */
    count ap1,                          /* awt peloton 1                                                    */
    count ap2,                          /* awt peloton 2                                                    */
    count ap3)                          /* awt peloton 3                                                    */
  {
    if (AKTPRG > 0) {
      MM[ap1] = MM[ap2] = MM[ap3] = 0;
    }
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV KOPPELINGEN LANGZAAM VERKEER                                                              */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal FCA[]                                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_fca_vtgvas(void)            /* Fik110801                                                        */
  {
    mulv i,j,x;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      
      if (kp_vtg_fts[i][0] != 0) {      /* werkwijze valide                                                 */
        
        for (j=1; j<=4; j++) {          /* voetgangers naar fietsers                                        */
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]] && !(A[kp_vtg_fts[i][j]]&BIT7) || 
                        (HVAS[kp_vtg_fts[i][j]]&BIT5)) { 
              for (x=5; x<=8; x++) {
                if (kp_vtg_fts[i][x] != NG) FCA[kp_vtg_fts[i][x]] |= BIT1;
              }
            }
          }
        }
        
        for (j=5; j<=8; j++) {          /* fietsers naar voetgangers                                        */
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]] && !(A[kp_vtg_fts[i][j]]&BIT7) || 
                        (HVAS[kp_vtg_fts[i][j]]&BIT5)) { 
              for (x=1; x<=4; x++) {
                if (kp_vtg_fts[i][x] != NG) FCA[kp_vtg_fts[i][x]] |= BIT1;
              }
            }
          }
        }
        
        for (j=1; j<=4; j++) {          /* voetgangers naar voetgangers                                     */
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]] && !(A[kp_vtg_fts[i][j]]&BIT7) || 
                        (HVAS[kp_vtg_fts[i][j]]&BIT5)) { 
              
              if (kp_vtg_fts[i][0] == 1) { /* werkwijze langzaam verkeer oversteek                          */
                for (x=1; x<=4; x++) {
                  if ((kp_vtg_fts[i][x] != NG) && (j != x)) FCA[kp_vtg_fts[i][x]] |= BIT1;
                }
              }
              if (kp_vtg_fts[i][0] == 2) { /* werkwijze gescheiden oversteek                                */
                for (x=1; x<=4; x++) {
                  if ((kp_vtg_fts[i][x] != NG) && (j != x)) FCA[kp_vtg_fts[i][x]] |= BIT0;
                }
              }
              if (kp_vtg_fts[i][0] == 3) { /* werkwijze binnenste buiten oversteek                          */
                for (x=1; x<=4; x++) {
                  if ((kp_vtg_fts[i][x] != NG) && (j != x)) FCA[kp_vtg_fts[i][x]] |= BIT0;
                }
              }
              if (kp_vtg_fts[i][0] == 4) { /* werkwijze getrapte oversteek                                  */
                for (x=1; x<=4; x++) {
                  if ((kp_vtg_fts[i][x] != NG) && (j != x)) {
                    if ((j <= 2) && (x <= 2) || (j > 2) && (x > 2)) FCA[kp_vtg_fts[i][x]] |= BIT0;
                    else                                            FCA[kp_vtg_fts[i][x]] |= BIT1;
                  }
                }
              }
            }
          }
        }
        
        for (j=5; j<=8; j++) {          /* fietsers naar fietsers                                           */
          if (kp_vtg_fts[i][j] != NG) {
            if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]] && !(A[kp_vtg_fts[i][j]]&BIT7) || 
                        (HVAS[kp_vtg_fts[i][j]]&BIT5)) { 
              for (x=5; x<=8; x++) {
                if ((kp_vtg_fts[i][x] != NG) && (j != x)) {
                  if ((j == 5) && (x == 6) || (j == 7) && (x == 8)) FCA[kp_vtg_fts[i][x]] |= BIT0;
                  else                                              FCA[kp_vtg_fts[i][x]] |= BIT1;
                }
              }
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer primaire realisatie                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_prm_vtgvas(void)            /* Fik100616                                                        */
  {
    mulv i,j,x;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (G[kp_vtg_fts[i][j]] && (HVAS[kp_vtg_fts[i][j]]&BIT4)) {
            for (x=1; x<=8; x++) {
              if ((kp_vtg_fts[i][x] != NG) && (j != x)) {
                if (G[kp_vtg_fts[i][x]] && !(HVAS[kp_vtg_fts[i][x]]&BIT0) && !(HVAS[kp_vtg_fts[i][x]]&BIT3)
                    && !(HVAS[kp_vtg_fts[i][x]]&BIT4)) {
                  HVAS[kp_vtg_fts[i][x]] |=  BIT4;
                  RW[kp_vtg_fts[i][x]] |=  BIT1;
                  A[kp_vtg_fts[i][x]] &=(~BIT4);
                }
              }
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer BAR                                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_bar_vtgvas(void)            /* Fik100618                                                        */
  {
    mulv i,j,x;
    mulv fc5,fc6,fc7,fc8;
    bool _BAR,_BAR_a;
    bool voorw;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      
      fc5 = kp_vtg_fts[i][5];
      fc6 = kp_vtg_fts[i][6];
      fc7 = kp_vtg_fts[i][7];
      fc8 = kp_vtg_fts[i][8];
      _BAR = _BAR_a = FALSE;
      
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (BAR[kp_vtg_fts[i][j]]) _BAR = TRUE;
          if (GL[kp_vtg_fts[i][j]] || TRG[kp_vtg_fts[i][j]]
              || BAR[kp_vtg_fts[i][j]] && R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]]) _BAR_a = TRUE;
        }
      }
      
      if (_BAR) {
        for (j=1; j<=4; j++) {          /* effect _BAR op voetgangersrichtingen                             */
          if (kp_vtg_fts[i][j] != NG) {
            BAR[kp_vtg_fts[i][j]] = TRUE;
            if (R[kp_vtg_fts[i][j]] && A[kp_vtg_fts[i][j]]) _BAR_a = TRUE;
          }
        }
        if ((fc5 != NG) && (fc6 != NG)) { /* effect _BAR op fietsrichtingen                                 */
          if (BAR[fc5] || BAR[fc6]) {
            BAR[fc5] = BAR[fc6] = TRUE;
            if (R[fc5] && A[fc5]) _BAR_a = TRUE;
            if (R[fc6] && A[fc6]) _BAR_a = TRUE;
          }
        }
        if ((fc7 != NG) && (fc8 != NG)) {
          if (BAR[fc7] || BAR[fc8]) {
            BAR[fc7] = BAR[fc8] = TRUE;
            if (R[fc7] && A[fc7]) _BAR_a = TRUE;
            if (R[fc8] && A[fc8]) _BAR_a = TRUE;
          }
        }
      }
      
      if (_BAR_a) {                     /* doorschrijven BAR[] naar overige richtingen                      */
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (!G[kp_vtg_fts[i][j]]) BAR[kp_vtg_fts[i][j]] = TRUE;
          }
        }
      }
      for (j=1; j<=8; j++) {            /* set BAR[] indien TTR[] >= TTK[] van gekoppelde richting in groen */
        if (kp_vtg_fts[i][j] != NG) {   /* TTR[]'s van gekoppelde richtingen zijn per definitie gelijk !    */
          if (!G[kp_vtg_fts[i][j]] && !BAR[kp_vtg_fts[i][j]]) {
            for (x=1; x<=8; x++) {
              if ((kp_vtg_fts[i][x] != NG) && (j != x)) {
                if (G[kp_vtg_fts[i][x]] && (TTR[kp_vtg_fts[i][j]]>=TTK[kp_vtg_fts[i][x]])) BAR[kp_vtg_fts[i][j]] = TRUE;
              }
            }
          }
        }
      }
      
      voorw = FALSE;                    /* correctie BAR[] igv wachttijdvoorspeller                         */
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (_GRN[kp_vtg_fts[i][j]]) voorw = TRUE;
        }
      }
      if (voorw) {
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (!G[kp_vtg_fts[i][j]]) BAR[kp_vtg_fts[i][j]] = FALSE;
          }
        }
      }
    }
    
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer ALTM[]                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_alt_vtgvas(void)            /* Fik110717                                                        */
  {
    mulv i;
    mulv fc1,fc2,fc3,fc5,fc6,fc7,fc8;
    mulv kpt12,kpt21,kpt23,kpt31,kpt56,kpt78;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      
      if (kp_vtg_fts[i][0] == 2) {      /* werkwijze gescheiden oversteek                                   */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][3];
        kpt12 = kp_vtg_fts[i][11];
        kpt21 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (fc2 != NG)) {
          if (kpt21 != NG) {
            if (((T_max[kpt21]/10) > ALTM[fc1]) && !G[fc2]) ALTM[fc1] = T_max[kpt21]/10;
          }
          if (kpt12 != NG) {
            if (((T_max[kpt12]/10) > ALTM[fc2]) && !G[fc1]) ALTM[fc2] = T_max[kpt12]/10;
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 3) {      /* werkwijze binnenste buiten oversteek                             */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
        } else {
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
        }
        
        if ((fc1 != NG) && (fc2 != NG)) {
          if (kpt12 != NG) {
            if (ALTM[fc1]+(T_max[kpt12]/10) > ALTM[fc2])
              ALTM[fc2] = ALTM[fc1] + (T_max[kpt12]/10);
          } else {
            if (ALTM[fc1] > ALTM[fc2])
              ALTM[fc2] = ALTM[fc1];
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 4) {      /* werkwijze getrapte oversteek                                     */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt12 != NG) {
              if (ALTM[fc1]+(T_max[kpt12]/10) > ALTM[fc2])
                ALTM[fc2] = ALTM[fc1] + (T_max[kpt12]/10);
            } else {
              if (ALTM[fc1] > ALTM[fc2])
                ALTM[fc2] = ALTM[fc1];
            }
          }
        }
        if ((kp_vtg_fts[i][3] != NG) && (kp_vtg_fts[i][4] != NG)) {
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt12 != NG) {
              if (ALTM[fc1]+(T_max[kpt12]/10) > ALTM[fc2])
                ALTM[fc2] = ALTM[fc1] + (T_max[kpt12]/10);
            } else {
              if (ALTM[fc1] > ALTM[fc2])
                ALTM[fc2] = ALTM[fc1];
            }
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 5) {      /* werkwijze half getrapte oversteek                                */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][2];
        fc3   = kp_vtg_fts[i][3];
        kpt23 = kp_vtg_fts[i][11];
        kpt31 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
          if (kpt31 != NG) {
            if (((T_max[kpt31]/10) > ALTM[fc1]) && !G[fc3]) ALTM[fc1] = T_max[kpt31]/10;
          }
          if (kpt23 != NG) {
            if (((T_max[kpt23]/10) > ALTM[fc3]) && !G[fc2]) ALTM[fc3] = T_max[kpt23]/10;
          }
        }
      }
      
      fc5   = kp_vtg_fts[i][5];         /* correctie ALTM[] ivm fietskoppelingen                            */
      fc6   = kp_vtg_fts[i][6];
      fc7   = kp_vtg_fts[i][7];
      fc8   = kp_vtg_fts[i][8];
      kpt56 = kp_vtg_fts[i][13];
      kpt78 = kp_vtg_fts[i][14];
      
      if ((fc5 != NG) && (fc6 != NG)) {
        if (kpt56 != NG) {
          if ((ALTM[fc5] + (T_max[kpt56]/10)) > ALTM[fc6]) ALTM[fc6] = ALTM[fc5] + (T_max[kpt56]/10);
        } else {
          if (ALTM[fc5] > ALTM[fc6])
            ALTM[fc6] = ALTM[fc5];
        }
      }
      
      if ((fc7 != NG) && (fc8 != NG)) {
        if (kpt78 != NG) {
          if ((ALTM[fc7] + (T_max[kpt78]/10)) > ALTM[fc8]) ALTM[fc8] = ALTM[fc7] + (T_max[kpt78]/10);
        } else {
          if (ALTM[fc7] > ALTM[fc8])
            ALTM[fc8] = ALTM[fc7];
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer TEG[]                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_teg_vtgvas(void)            /* Fik110717                                                        */
  {
    mulv i;
    mulv fc1,fc2,fc3,fc4,fc5,fc6,fc7,fc8;
    mulv kpt12,kpt21,kpt23,kpt31,kpt56,kpt78;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      
      fc1   = kp_vtg_fts[i][1];         /* bepaal TEG[] voetgangers op SG[]                                 */
      fc2   = kp_vtg_fts[i][2];
      fc3   = kp_vtg_fts[i][3];
      fc4   = kp_vtg_fts[i][4];
      
      if (fc1 != NG) {
        if (SG[fc1]) {
          TEG[fc1] = TFG_max[fc1]/10;
          if ((TGG_max[fc1]/10) > TEG[fc1])
            TEG[fc1] = TGG_max[fc1]/10;
        }
      }
      
      if (fc2 != NG) {
        if (SG[fc2]) {
          TEG[fc2] = TFG_max[fc2]/10;
          if ((TGG_max[fc2]/10) > TEG[fc2])
            TEG[fc2] = TGG_max[fc2]/10;
        }
      }
      
      if (fc3 != NG) {
        if (SG[fc3]) {
          TEG[fc3] = TFG_max[fc3]/10;
          if ((TGG_max[fc3]/10) > TEG[fc3])
            TEG[fc3] = TGG_max[fc3]/10;
        }
      }
      
      if (fc4 != NG) {
        if (SG[fc4]) {
          TEG[fc4] = TFG_max[fc4]/10;
          if ((TGG_max[fc4]/10) > TEG[fc4])
            TEG[fc4] = TGG_max[fc4]/10;
        }
      }
      
      if (kp_vtg_fts[i][0] == 2) {      /* werkwijze gescheiden oversteek                                   */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][3];
        kpt12 = kp_vtg_fts[i][11];
        kpt21 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (fc2 != NG)) {
          if (SG[fc1] && (kpt12 != NG)) {
            if (((T_max[kpt12]/10) > TEG[fc2]) && G[fc2]) TEG[fc2] = T_max[kpt12]/10;
          }
          if (SG[fc2] && (kpt21 != NG)) {
            if (((T_max[kpt21]/10) > TEG[fc1]) && G[fc1]) TEG[fc1] = T_max[kpt21]/10;
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 3) {      /* werkwijze binnenste buiten oversteek                             */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
        } else {
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
        }
        
        if ((fc1 != NG) && (fc2 != NG)) {
          if (G[fc1] || _GRN[fc1]) {
            if (kpt12 != NG) {
              if (((TEG[fc1] + (T_max[kpt12]/10)) > TEG[fc2]) && (G[fc2] || _GRN[fc2])) {
                TEG[fc2] = (T_max[kpt12]/10)  + TEG[fc1];
              }
            } else {
              if ((G[fc2] || _GRN[fc2]) && (TEG[fc1] > TEG[fc2])) TEG[fc2] = TEG[fc1];
            }
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 4) {      /* werkwijze getrapte oversteek                                     */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (G[fc1] || _GRN[fc1]) {
              if (kpt12 != NG) {
                if (((TEG[fc1] + (T_max[kpt12]/10)) > TEG[fc2]) && (G[fc2] || _GRN[fc2])) {
                  TEG[fc2] = (T_max[kpt12]/10)  + TEG[fc1];
                }
              } else {
                if ((G[fc2] || _GRN[fc2]) && (TEG[fc1] > TEG[fc2])) TEG[fc2] = TEG[fc1];
              }
            }
          }
        }
        if ((kp_vtg_fts[i][3] != NG) && (kp_vtg_fts[i][4] != NG)) {
          
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (G[fc1] || _GRN[fc1]) {
              if (kpt12 != NG) {
                if (((TEG[fc1] + (T_max[kpt12]/10)) > TEG[fc2]) && (G[fc2] || _GRN[fc2])) {
                  TEG[fc2] = (T_max[kpt12]/10)  + TEG[fc1];
                }
              } else {
                if ((G[fc2] || _GRN[fc2]) && (TEG[fc1] > TEG[fc2])) TEG[fc2] = TEG[fc1];
              }
            }
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 5) {      /* werkwijze half getrapte oversteek                                */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][2];
        fc3   = kp_vtg_fts[i][3];
        kpt23 = kp_vtg_fts[i][11];
        kpt31 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
          if (SG[fc2] && (kpt23 != NG)) {
            if (((T_max[kpt23]/10) > TEG[fc3]) && G[fc3]) TEG[fc3] = T_max[kpt23]/10;
          }
          if (SG[fc3] && (kpt31 != NG)) {
            if (((T_max[kpt31]/10) > TEG[fc1]) && G[fc1]) TEG[fc1] = T_max[kpt31]/10;
          }
        }
      }
      
      fc5   = kp_vtg_fts[i][5];         /* bepaal TEG[] fietsers                                            */
      fc6   = kp_vtg_fts[i][6];
      fc7   = kp_vtg_fts[i][7];
      fc8   = kp_vtg_fts[i][8];
      kpt56 = kp_vtg_fts[i][13];
      kpt78 = kp_vtg_fts[i][14];
      
      if ((fc5 >= 0) && (fc6 >= 0)) {
        if (G[fc5] || _GRN[fc5]) {
          if (((TEG[fc5] + (T_max[kpt56]/10)) > TEG[fc6]) && (G[fc6] || _GRN[fc6])) TEG[fc6] = 
                    (T_max[kpt56]/10) + TEG[fc5]; 
        }
      }
      
      if ((fc7 >= 0) && (fc8 >= 0)) {
        if (G[fc7] || _GRN[fc7]) {
          if (((TEG[fc7] + (T_max[kpt78]/10)) > TEG[fc8]) && (G[fc8] || _GRN[fc8])) TEG[fc8] = 
                    (T_max[kpt78]/10) + TEG[fc7]; 
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure afwikkeling koppelingen langzaam verkeer                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_afw_lvk_vas(void)           /* Fik120105                                                        */
  {
    mulv i,j,x;
    mulv fc1,fc2,fc3,fc5,fc6,fc7,fc8;
    mulv kpa12,kpa21,kpa23,kpa31;
    mulv kpt12,kpt21,kpt23,kpt31,kpt56,kpt78;
    bool voorw,xu;
    
    for (i=0; i<aantal_vtg_fts; i++) {
      
      /* Correctie BL[] indien RA[] en wachttijdvoorspeller                                                 */
      /* --------------------------------------------------                                                 */
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
      
      /* Altijd gelijktijdig starten wachtstand                                                             */
      /* --------------------------------------                                                             */
      voorw = FALSE;
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (GL[kp_vtg_fts[i][j]] || TRG[kp_vtg_fts[i][j]] || K[kp_vtg_fts[i][j]]) voorw = TRUE;
        }
      }
      if (voorw) {
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            BAR[kp_vtg_fts[i][j]] |= !G[kp_vtg_fts[i][j]];
          }
        }
      }
      
      /* Opzetten cyc.prim.aanvraag indien gekoppelde richting versneld inkomt                              */
      /* ---------------------------------------------------------------------                              */
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (R[kp_vtg_fts[i][j]] && !TRG[kp_vtg_fts[i][j]] && (HVAS[kp_vtg_fts[i][j]]&BIT5)) {
            for (x=1; x<=8; x++) {
              if ((kp_vtg_fts[i][x] != NG) && (x != j)) {
                if (A[kp_vtg_fts[i][x]]&BIT4) {
                  A[kp_vtg_fts[i][j]] |= BIT2;
                  A[kp_vtg_fts[i][j]] |= BIT4;
                  A[kp_vtg_fts[i][j]] &= (~BIT3);
                  A[kp_vtg_fts[i][j]] &= (~BIT6);
                }
              }
            }
          }
        }
      }
      
      /* set zonodig mee aanvragen                                                                          */
      /* -------------------------                                                                          */
      proc_mav_vtg();
      
      /* Herstart koppeltijden voetgangers                                                                  */
      /* ---------------------------------                                                                  */
      if (kp_vtg_fts[i][0] == 2) {      /* werkwijze gescheiden oversteek                                   */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][3];
        kpa12 = kp_vtg_fts[i][9];
        kpa21 = kp_vtg_fts[i][10];
        kpt12 = kp_vtg_fts[i][11];
        kpt21 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (kpa12 != NG) && (kpt12 != NG)) {
          RT[kpt12] = SG[fc1] && MM[kpa12];
        } else {
          if (kpt12 != NG) RT[kpt12] = FALSE;
        }
        if ((fc2 != NG) && (kpa21 != NG) && (kpt21 != NG)) {
          RT[kpt21] = SG[fc2] && MM[kpa21];
        } else {
          if (kpt21 != NG) RT[kpt21] = FALSE;
        }
      }
      
      if (kp_vtg_fts[i][0] == 3) {      /* werkwijze binnenste buiten oversteek                             */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
        } else {
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
        }
        
        if ((fc1 != NG) && (kpt12 != NG)) RT[kpt12] = G[fc1];
        /* if (G[fc1]) A[fc1] |=  BIT6;  was ivm MVG voedende richting                                      */
        /* else       A[fc1] &=(~BIT6); wordt tzt opgelost dmv hkii                                         */
      }
      
      if (kp_vtg_fts[i][0] == 4) {      /* werkwijze getrapte oversteek                                     */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
          
          if ((fc1 != NG) && (kpt12 != NG)) RT[kpt12] = G[fc1];
          /* if (G[fc1]) A[fc1] |=  BIT6;  was ivm MVG voedende richting                                    */
          /* else       A[fc1] &=(~BIT6); wordt tzt opgelost dmv hkii                                       */
        }
        if ((kp_vtg_fts[i][3] != NG) && (kp_vtg_fts[i][4] != NG)) {
          
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
          
          if ((fc1 != NG) && (kpt12 != NG)) RT[kpt12] = G[fc1];
          /* if (G[fc1]) A[fc1] |=  BIT6;  was ivm MVG voedende richting                                    */
          /* else       A[fc1] &=(~BIT6); wordt tzt opgelost dmv hkii                                       */
        }
      }
      
      if (kp_vtg_fts[i][0] == 5) {      /* werkwijze half getrapte oversteek                                */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][2];
        fc3   = kp_vtg_fts[i][3];
        kpa23 = kp_vtg_fts[i][9];
        kpa31 = kp_vtg_fts[i][10];
        kpt23 = kp_vtg_fts[i][11];
        kpt31 = kp_vtg_fts[i][12];
        
        if ((fc2 != NG) && (kpa23 != NG) && (kpt23 != NG)) {
          RT[kpt23] = SG[fc2] && MM[kpa23];
        } else {
          if (kpt23 != NG) RT[kpt23] = FALSE;
        }
        if ((fc3 != NG) && (kpa31 != NG) && (kpt31 != NG)) {
          RT[kpt31] = SG[fc3] && MM[kpa31];
        } else {
          if (kpt31 != NG) RT[kpt31] = FALSE;
        }
      }
      
      /* Vasthouden groen naloop-richting voetgangers                                                       */
      /* --------------------------------------------                                                       */
      if (kp_vtg_fts[i][0] == 2) {      /* werkwijze gescheiden oversteek                                   */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][3];
        kpt12 = kp_vtg_fts[i][11];
        kpt21 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (fc2 != NG)) {
          voorw = FALSE;
          if (kpt12 != NG) {
            voorw = ET[kpt12];
            if (RT[kpt12] || T[kpt12] || B[fc1] || RA[fc1]) {
              YW[fc2] |= BIT3;
              RW[fc2] |= BIT3;
              FM[fc2]  = FALSE;
              RW[fc2] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          } else {
            if (B[fc1] || RA[fc1]) {
              YW[fc2] |= BIT3;
              RW[fc2] |= BIT3;
              FM[fc2]  = FALSE;
              RW[fc2] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          }
          if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3)) YM[fc2] |=  BIT2;
          else if (!voorw && !SVG[fc2])       YM[fc2] &=(~BIT2);
          
          voorw = FALSE;
          if (kpt21 != NG) {
            voorw = ET[kpt21];
            if (RT[kpt21] || T[kpt21] || B[fc2] || RA[fc2]) {
              YW[fc1] |= BIT3;
              RW[fc1] |= BIT3;
              FM[fc1]  = FALSE;
              RW[fc1] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          } else {
            if (B[fc2] || RA[fc2]) {
              YW[fc1] |= BIT3;
              RW[fc1] |= BIT3;
              FM[fc1]  = FALSE;
              RW[fc1] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          }
          if ((WG[fc1] || VG[fc1] || MG[fc1]) && (RW[fc1]&BIT3)) YM[fc1] |=  BIT2;
          else if (!voorw && !SVG[fc1])       YM[fc1] &=(~BIT2);
          
          if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
            if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
              HVAS[fc2] |= BIT3;
              TTP[fc2]  = 255;
            }
          }
          if ((HVAS[fc2]&BIT3) && G[fc1] && (YM[fc1]&BIT2)) {
            if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
              HVAS[fc1] |= BIT3;
              TTP[fc1]  = 255;
            }
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 3) {      /* werkwijze binnenste buiten oversteek                             */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
        } else {
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
        }
        
        if ((fc1 != NG) && (fc2 != NG)) {
          if (B[fc1] || RA[fc1]) {
            YW[fc2] |= BIT3;
            RW[fc2] |= BIT3;
            FM[fc2]  = FALSE;
          }
          if (kpt12 != NG) {
            if (RT[kpt12] || T[kpt12]) {
              YW[fc2] |= BIT3;
              RW[fc2] |= BIT3;
              FM[fc2]  = FALSE;
              RW[fc2] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          }
        }
        if (fc2 != NG) {
          if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3)) YM[fc2] |= BIT2;
          else {
            voorw = FALSE;
            if (kpt12 != NG) voorw = ET[kpt12];
            if (!voorw && !SVG[fc2]) YM[fc2] &= (~BIT2);
          }
        }
        if ((fc1 != NG) && (fc2 != NG)) {
          if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
            if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
              HVAS[fc2] |= BIT3;
              TTP[fc2]  = 255;
            }
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 4) {      /* werkwijze getrapte oversteek                                     */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (B[fc1] || RA[fc1]) {
              YW[fc2] |= BIT3;
              RW[fc2] |= BIT3;
              FM[fc2]  = FALSE;
            }
            if (kpt12 != NG) {
              if (RT[kpt12] || T[kpt12]) {
                YW[fc2] |= BIT3;
                RW[fc2] |= BIT3;
                FM[fc2]  = FALSE;
                RW[fc2] |= BIT4;        /* tbv hulpdienstingreep                                            */
              }
            }
          }
          if (fc2 != NG) {
            if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3)) YM[fc2] |= BIT2;
            else {
              voorw = FALSE;
              if (kpt12 != NG) voorw = ET[kpt12];
              if (!voorw && !SVG[fc2]) YM[fc2] &= (~BIT2);
            }
          }
          if ((fc1 != NG) && (fc2 != NG)) {
            if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
              if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
                HVAS[fc2] |= BIT3;
                TTP[fc2]  = 255;
              }
            }
          }
        }
        
        if ((kp_vtg_fts[i][3] != NG) && (kp_vtg_fts[i][4] != NG)) {
          
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (B[fc1] || RA[fc1]) {
              YW[fc2] |= BIT3;
              RW[fc2] |= BIT3;
              FM[fc2]  = FALSE;
            }
            if (kpt12 != NG) {
              if (RT[kpt12] || T[kpt12]) {
                YW[fc2] |= BIT3;
                RW[fc2] |= BIT3;
                FM[fc2]  = FALSE;
                RW[fc2] |= BIT4;        /* tbv hulpdienstingreep                                            */
              }
            }
          }
          if (fc2 != NG) {
            if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3)) YM[fc2] |= BIT2;
            else {
              voorw = FALSE;
              if (kpt12 != NG) voorw = ET[kpt12];
              if (!voorw && !SVG[fc2]) YM[fc2] &= (~BIT2);
            }
          }
          if ((fc1 != NG) && (fc2 != NG)) {
            if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
              if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
                HVAS[fc2] |= BIT3;
                TTP[fc2]  = 255;
              }
            }
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 5) {      /* werkwijze half getrapte oversteek                                */
        
        fc1   = kp_vtg_fts[i][1];
        fc2   = kp_vtg_fts[i][2];
        fc3   = kp_vtg_fts[i][3];
        kpt12 = kp_vtg_fts[i][11];
        kpt21 = kp_vtg_fts[i][12];
        
        if ((fc1 != NG) && (fc2 != NG) && (fc3 != NG)) {
          voorw = FALSE;
          if (kpt23 != NG) {
            voorw = ET[kpt23];
            if (RT[kpt23] || T[kpt23] || B[fc2] || RA[fc2]) {
              YW[fc3] |= BIT3;
              RW[fc3] |= BIT3;
              FM[fc3]  = FALSE;
              RW[fc3] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          } else {
            if (B[fc2] || RA[fc2]) {
              YW[fc3] |= BIT3;
              RW[fc3] |= BIT3;
              FM[fc3]  = FALSE;
              RW[fc3] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          }
          if ((WG[fc3] || VG[fc3] || MG[fc3]) && (RW[fc3]&BIT3)) YM[fc3] |=  BIT2;
          else if (!voorw && !SVG[fc3])       YM[fc3] &=(~BIT2);
          
          voorw = FALSE;
          if (kpt31 != NG) {
            voorw = ET[kpt31];
            if (RT[kpt31] || T[kpt31] || B[fc3] || RA[fc3]) {
              YW[fc1] |= BIT3;
              RW[fc1] |= BIT3;
              FM[fc1]  = FALSE;
              RW[fc1] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          } else {
            if (B[fc3] || RA[fc3]) {
              YW[fc1] |= BIT3;
              RW[fc1] |= BIT3;
              FM[fc1]  = FALSE;
              RW[fc1] |= BIT4;          /* tbv hulpdienstingreep                                            */
            }
          }
          if ((WG[fc1] || VG[fc1] || MG[fc1]) && (RW[fc1]&BIT3)) YM[fc1] |=  BIT2;
          else if (!voorw && !SVG[fc1])       YM[fc1] &=(~BIT2);
          
          if ((HVAS[fc2]&BIT3) && G[fc3] && (YM[fc3]&BIT2)) {
            if ((HVAS[fc2]&BIT7) == (HVAS[fc3]&BIT7)) {
              HVAS[fc3] |= BIT3;
              TTP[fc3]  = 255;
            }
          }
          if ((HVAS[fc3]&BIT3) && G[fc1] && (YM[fc1]&BIT2)) {
            if ((HVAS[fc3]&BIT7) == (HVAS[fc1]&BIT7)) {
              HVAS[fc1] |= BIT3;
              TTP[fc1]  = 255;
            }
          }
        }
      }
      
      /* Herstart koppeltijden fietsers                                                                     */
      /* ------------------------------                                                                     */
      fc5   = kp_vtg_fts[i][5];
      fc6   = kp_vtg_fts[i][6];
      fc7   = kp_vtg_fts[i][7];
      fc8   = kp_vtg_fts[i][8];
      kpt56 = kp_vtg_fts[i][13];
      kpt78 = kp_vtg_fts[i][14];
      
      if ((fc5 != NG) && (fc6 != NG) && (kpt56 != NG)) {
        RT[kpt56] = G[fc5];
      } else {
        if (kpt56 != NG) RT[kpt56] = FALSE;
      }
      
      if ((fc7 != NG) && (fc8 != NG) && (kpt78 != NG)) {
        RT[kpt78] = G[fc7];
      } else {
        if (kpt78 != NG) RT[kpt78] = FALSE;
      }
      
      /* Vasthouden groen naloop-richting fietsers                                                          */
      /* -----------------------------------------                                                          */
      if ((fc5 != NG) && (fc6 != NG)) {
        voorw = FALSE;
        if (kpt56 != NG) {
          voorw = ET[kpt56];
          if (RT[kpt56] || T[kpt56] || B[fc5] || RA[fc5]) {
            YW[fc6]   |= BIT3;
            RW[fc6]   |= BIT3;
            FM[fc6]    = FALSE;
            RW[fc6]   |= BIT4;          /* tbv hulpdienstingreep                                            */
            HKII[fc5] |= BIT3;
          }
        }
        if ((WG[fc6] || VG[fc6] || MG[fc6]) && (RW[fc6]&BIT3)) YM[fc6] |=  BIT2;
        else if (!voorw && !SVG[fc6])       YM[fc6] &=(~BIT2);
      }
      if ((fc7 != NG) && (fc8 != NG)) {
        voorw = FALSE;
        if (kpt78 != NG) {
          voorw = ET[kpt78];
          if (RT[kpt78] || T[kpt78] || B[fc7] || RA[fc7]) {
            YW[fc8]   |= BIT3;
            RW[fc8]   |= BIT3;
            FM[fc8]    = FALSE;
            RW[fc8]   |= BIT4;          /* tbv hulpdienstingreep                                            */
            HKII[fc7] |= BIT3;
          }
        }
        if ((WG[fc8] || VG[fc8] || MG[fc8]) && (RW[fc8]&BIT3)) YM[fc8] |=  BIT2;
        else if (!voorw && !SVG[fc8])       YM[fc8] &=(~BIT2);
      }
      
      /* Vasthouden meeverlenggroen                                                                         */
      /* --------------------------                                                                         */
      voorw = FALSE;                    /* gekoppelde richtingen onderling                                  */
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if ((RV[kp_vtg_fts[i][j]] && B[kp_vtg_fts[i][j]] || RA[kp_vtg_fts[i][j]]) && 
                    !RR[kp_vtg_fts[i][j]]) voorw |= TRUE; 
        }
      }
      if (voorw) {
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (G[kp_vtg_fts[i][j]]) YM[kp_vtg_fts[i][j]] |= BIT3;
          }
        }
      }
      
      voorw = FALSE;                    /* voedende fietsers met voetgangers                                */
      for (j=1; j<=4; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (G[kp_vtg_fts[i][j]] && (!WG[kp_vtg_fts[i][j]] || !(RW[kp_vtg_fts[i][j]]&BIT5))
              && (!MG[kp_vtg_fts[i][j]] ||  (RW[kp_vtg_fts[i][j]]&BIT3))) voorw = TRUE;
        }
      }
      if (voorw) {
        if (kp_vtg_fts[i][5] != NG) { if (G[kp_vtg_fts[i][5]] && !somfbk(kp_vtg_fts[i][5])) 
                                YM[kp_vtg_fts[i][5]] |= BIT1; } 
        if (kp_vtg_fts[i][7] != NG) { if (G[kp_vtg_fts[i][7]] && !somfbk(kp_vtg_fts[i][7])) 
                                YM[kp_vtg_fts[i][7]] |= BIT1; } 
        
      }
      
      /* Beurt t.b.v. gelijke start                                                                         */
      /* --------------------------                                                                         */
      voorw = FALSE;
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if ((RV[kp_vtg_fts[i][j]] && B[kp_vtg_fts[i][j]] || RA[kp_vtg_fts[i][j]]) && A[kp_vtg_fts[i][j]] 
                    && !RR[kp_vtg_fts[i][j]]) voorw |= TRUE; 
        }
      }
      if (voorw) {
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if ((R[kp_vtg_fts[i][j]] && !TRG[kp_vtg_fts[i][j]]) && A[kp_vtg_fts[i][j]]) {
              B[kp_vtg_fts[i][j]] |= BIT3;
              RR[kp_vtg_fts[i][j]] = FALSE;
            }
          }
        }
        
      }
      
      /* Uitstel t.b.v. gelijke start                                                                       */
      /* ----------------------------                                                                       */
      voorw = FALSE;
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          xu = TRUE;
          if (RA[kp_vtg_fts[i][j]] && !K[kp_vtg_fts[i][j]] && !X[kp_vtg_fts[i][j]] && !RR[kp_vtg_fts[i][j]]
              || G[kp_vtg_fts[i][j]] || RV[kp_vtg_fts[i][j]] && !TRG[kp_vtg_fts[i][j]] && !A[kp_vtg_fts[i][j]]) xu = FALSE;
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
      
      /* Altijd gelijktijdig overslag                                                                       */
      /* ----------------------------                                                                       */
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if ((X[kp_vtg_fts[i][j]]&BIT1) || G[kp_vtg_fts[i][j]] && (RW[kp_vtg_fts[i][j]]&BIT1)) {
            for (x=1; x<=8; x++) {
              if ((kp_vtg_fts[i][x] != NG) && (x != j)) {
                if ((X[kp_vtg_fts[i][j]]&BIT1) || !G[kp_vtg_fts[i][x]]) {
                  if ((HVAS[kp_vtg_fts[i][j]]&BIT7) == (HVAS[kp_vtg_fts[i][x]]&BIT7)) {
                    HVAS[kp_vtg_fts[i][x]] |= BIT3;
                    TTP[kp_vtg_fts[i][x]]  = 255;
                  }
                }
              }
            }
          }
        }
      }
      
      /* Beeindig groen voedende richting voetgangers                                                       */
      /* --------------------------------------------                                                       */
      if (kp_vtg_fts[i][0] == 3) {      /* werkwijze binnenste buiten oversteek                             */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
        } else {
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
        }
        
        if ((fc1 != NG) && (fc2 != NG) && (kpt12 != NG)) {
          if (kpt12 != NG) {
            if (G[fc1] && G[fc2] && (TTK[fc2] <= (T_max[kpt12]/10))) Z[fc1] |= BIT1;
          } else {
            if (G[fc1] && G[fc2] && (TTK[fc2] == 0)) Z[fc1] |= BIT1;
          }
        }
      }
      
      if (kp_vtg_fts[i][0] == 4) {      /* werkwijze getrapte oversteek                                     */
        
        if ((kp_vtg_fts[i][1] != NG) && (kp_vtg_fts[i][2] != NG)) {
          
          fc1   = kp_vtg_fts[i][1];
          fc2   = kp_vtg_fts[i][2];
          kpt12 = kp_vtg_fts[i][11];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt12 != NG) {
              if (G[fc1] && G[fc2] && (TTK[fc2] <= (T_max[kpt12]/10))) Z[fc1] |= BIT1;
            } else {
              if (G[fc1] && G[fc2] && (TTK[fc2] == 0)) Z[fc1] |= BIT1;
            }
          }
        }
        
        if ((kp_vtg_fts[i][3] != NG) && (kp_vtg_fts[i][4] != NG)) {
          
          fc1   = kp_vtg_fts[i][3];
          fc2   = kp_vtg_fts[i][4];
          kpt12 = kp_vtg_fts[i][12];
          
          if ((fc1 != NG) && (fc2 != NG)) {
            if (kpt12 != NG) {
              if (G[fc1] && G[fc2] && (TTK[fc2] <= (T_max[kpt12]/10))) Z[fc1] |= BIT1;
            } else {
              if (G[fc1] && G[fc2] && (TTK[fc2] == 0)) Z[fc1] |= BIT1;
            }
          }
        }
      }
      
      /* Beeindig groen voedende richting fietsers                                                          */
      /* -----------------------------------------                                                          */
      if ((fc5 != NG) && (fc6 != NG)) {
        if (kpt56 != NG) {
          if (G[fc5] && G[fc6] && (TTK[fc6] <= (T_max[kpt56]/10))) Z[fc5] |= BIT1;
        } else {
          if (G[fc5] && G[fc6] && (TTK[fc6] == 0)) Z[fc5] |= BIT1;
        }
      }
      if ((fc7 != NG) && (fc8 != NG)) {
        if (kpt78 != NG) {
          if (G[fc7] && G[fc8] && (TTK[fc8] <= (T_max[kpt78]/10))) Z[fc7] |= BIT1;
        } else {
          if (G[fc7] && G[fc8] && (TTK[fc8] == 0)) Z[fc7] |= BIT1;
        }
      }
      
#ifdef _VAS_OV
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if ((YW[kp_vtg_fts[i][j]]&BIT3) || _GRN[kp_vtg_fts[i][j]]) HVAS[kp_vtg_fts[i][j]] |=   VTG_NA;
          else                                                       HVAS[kp_vtg_fts[i][j]] &= (~VTG_NA);
        }
      }
      
      voorw = TRUE;
      for (j=1; j<=8; j++) {
        if (kp_vtg_fts[i][j] != NG) {
          if (K[kp_vtg_fts[i][j]] || G[kp_vtg_fts[i][j]] || GL[kp_vtg_fts[i][j]] || TRG[kp_vtg_fts[i][j]]) 
                    voorw = FALSE; 
        }
      }
      if (voorw) {
        for (j=1; j<=8; j++) {
          if (kp_vtg_fts[i][j] != NG) {
            if (!SRA[kp_vtg_fts[i][j]] && RA[kp_vtg_fts[i][j]]) X[kp_vtg_fts[i][j]] = FALSE;
          }
        }
      }
#endif
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure halfstarre koppeling IN                                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_hskp_vas(                   /* Fik100309                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kopsig,                       /* koppelsignaal                                                    */
    count ingr,                         /* PRM ingreep aktief                                               */
    count afstrt)                       /* T afstand rijtijd                                                */
  {
    bool _aktief = FALSE;
    /* controleer instelling                                                                                */
    if ((aantal_VAS == 1) && (PRM[ingr] >   1) ||
        (aantal_VAS == 2) && (PRM[ingr] >   3) ||
        (aantal_VAS == 3) && (PRM[ingr] >   7) ||
        (aantal_VAS == 4) && (PRM[ingr] >  15) ||
        (aantal_VAS == 5) && (PRM[ingr] >  31) ||
        (aantal_VAS == 6) && (PRM[ingr] >  63) ||
        (aantal_VAS == 7) && (PRM[ingr] > 127) ||
        (aantal_VAS == 8) && (PRM[ingr] > 255) ||
        (aantal_VAS == 9) && (PRM[ingr] > 511)) {
      PRM[ingr] = 0;
      CIF_PARM1WIJZAP = -2;
    }
    /* ingreep aktief?                                                                                      */
    if ((AKTPRG==1) && (PRM[ingr]&BIT0)) _aktief = TRUE;
    if ((AKTPRG==2) && (PRM[ingr]&BIT1)) _aktief = TRUE;
    if ((AKTPRG==3) && (PRM[ingr]&BIT2)) _aktief = TRUE;
    if ((AKTPRG==4) && (PRM[ingr]&BIT3)) _aktief = TRUE;
    if ((AKTPRG==5) && (PRM[ingr]&BIT4)) _aktief = TRUE;
    if ((AKTPRG==6) && (PRM[ingr]&BIT5)) _aktief = TRUE;
    if ((AKTPRG==7) && (PRM[ingr]&BIT6)) _aktief = TRUE;
    if ((AKTPRG==8) && (PRM[ingr]&BIT7)) _aktief = TRUE;
    if ((AKTPRG==9) && (PRM[ingr]>=256)) _aktief = TRUE;
    
#ifdef DVM_AANWEZIG
    
    if ((AKTPRG > 0) && (AKTPRG < 10)) {
      RT[afstrt] = (AKTPRG > 0) && CIF_IS[kopsig] && _aktief;
      AT[afstrt] = T[afstrt] && !_aktief;
    } else {
      RT[afstrt] = AT[afstrt] = FALSE;
    }
    
#else                                   /* *** ifdef -> DVM_AANWEZIG                                        */
    
    RT[afstrt] = (AKTPRG > 0) && CIF_IS[kopsig] && _aktief;
    AT[afstrt] = T[afstrt] && !_aktief;
    
#endif                                  /* *** ifdef -> DVM_AANWEZIG                                        */
    
    if ((AKTPRG > 0) && (HVAS[fc]&BIT0) && (RT[afstrt] || T[afstrt])) MK[fc] |= BIT2;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure halfstarre koppeling IN tijdens DVM                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_hskp_vas_dvm(               /* Fik100316                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kopsig,                       /* koppelsignaal                                                    */
    count ingr,                         /* PRM ingreep aktief                                               */
    count afstrt)                       /* T afstand rijtijd                                                */
  {
    bool _aktief = FALSE;
    /* controleer instelling                                                                                */
    if (PRM[ingr] > 63) {
      PRM[ingr] = 0;
      CIF_PARM1WIJZAP = -2;
    }
    /* ingreep aktief?                                                                                      */
    if ((AKTPRG==21) && (PRM[ingr]&BIT0)) _aktief = TRUE;
    if ((AKTPRG==22) && (PRM[ingr]&BIT1)) _aktief = TRUE;
    if ((AKTPRG==23) && (PRM[ingr]&BIT2)) _aktief = TRUE;
    if ((AKTPRG==24) && (PRM[ingr]&BIT3)) _aktief = TRUE;
    if ((AKTPRG==25) && (PRM[ingr]&BIT4)) _aktief = TRUE;
    if ((AKTPRG==26) && (PRM[ingr]&BIT5)) _aktief = TRUE;
    
    if ((AKTPRG == 0) || (AKTPRG > 10)) {
      RT[afstrt] |= (AKTPRG > 0) && CIF_IS[kopsig] && _aktief;
      AT[afstrt] |= T[afstrt] && !_aktief;
    }
    
    if ((AKTPRG > 0) && (HVAS[fc]&BIT0) && (RT[afstrt] || T[afstrt])) MK[fc] |= BIT2;
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure halfstarre koppeling UIT                                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_hskp_vas_uit(               /* Fik100309                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kopsig,                       /* koppelsignaal                                                    */
    count mintijd)                      /* T min.duur ks                                                    */
  {
    bool _prim = FALSE;
    
    _prim = (AKTPRG > 0) && (HVAS[fc]&BIT0) && (!(HVAS[fc]&BIT4) || (VAS3[fc] == 0));
    RT[mintijd] = BEDRYF && !CIF_GUS[kopsig] && G[fc] && _prim;
    CIF_GUS[kopsig] = BEDRYF && (_prim || RT[mintijd] || T[mintijd]) && G[fc];
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN GELIJKE START TIJDENS HALFSTAR BEDRIJF                                     */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal FCA[] tbv deelconflicten gelijke start                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_vas_fca(void)             /* Fik110801                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (R[fc1] && A[fc1] && !(A[fc1]&BIT7) || (HVAS[fc1]&BIT5)) FCA[fc2] |= BIT1;
        if (R[fc2] && A[fc2] && !(A[fc2]&BIT7) || (HVAS[fc2]&BIT5)) FCA[fc1] |= BIT1;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer primaire realisatie tbv deelconflicten gelijke start                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_vas_prm(void)             /* Fik110801                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (G[fc1] && (HVAS[fc1]&BIT4)) {
          if (G[fc2] && !(HVAS[fc2]&BIT0) && !(HVAS[fc2]&BIT3) && !(HVAS[fc2]&BIT4)) {
            HVAS[fc2] |= BIT4;
            RW[fc2]   |= BIT1;
            A[fc2] &= (~BIT4);
          }
        }
        if (G[fc2] && (HVAS[fc2]&BIT4)) {
          if (G[fc1] && !(HVAS[fc1]&BIT0) && !(HVAS[fc1]&BIT3) && !(HVAS[fc1]&BIT4)) {
            HVAS[fc1] |= BIT4;
            RW[fc1]   |= BIT1;
            A[fc1] &= (~BIT4);
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer BAR[] tbv deelconflicten gelijke start                                               */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_vas_bar(void)             /* Fik110801                                                        */
  {
    mulv i,j;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv to12,to21;
    bool door_BAR;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      ma12 = dcf_gs[i][2];
      ma21 = dcf_gs[i][3];
      to12 = dcf_gs[i][4];
      to21 = dcf_gs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        /* altijd BAR[] indien geel of garantie rood                                                        */
        if (GL[fc1] || R[fc1] && TRG[fc1]) BAR[fc1] |= TRUE;
        if (GL[fc2] || R[fc2] && TRG[fc2]) BAR[fc2] |= TRUE;
        
        door_BAR = FALSE;
        if (R[fc1] && !TRG[fc1] && BAR[fc1] && !BL[fc1] && (A[fc1] || SCH[ma12])) door_BAR = TRUE;
        if (R[fc2] && !TRG[fc2] && BAR[fc2] && !BL[fc2] && (A[fc2] || SCH[ma21])) door_BAR = TRUE;
        
        if (door_BAR) {
          for (j=0; j<=1; j++) {
            if (dcf_gs[i][j] != NG) {
              if (!G[dcf_gs[i][j]]) BAR[dcf_gs[i][j]] |= TRUE;
            }
          }
        }
        
        /* if (!G[fc1] && !G[fc2] && BAR[fc2] && ((_FIL[fc2]&BIT0) || (_FIL[fc2]&BIT1)) &&                  */
        /* (((HVAS[fc2]&BIT5) || A[fc2]) && !BL[fc2] &&                                                     */
        /* (TTP[fc2] <= TTR[fc1] + ALTM[fc1] + (TGL_max[fc1] + T_max[to12])/10))) BAR[fc1] = TRUE;          */
        
        /* if (!G[fc2] && !G[fc1] && BAR[fc1] && ((_FIL[fc1]&BIT0) || (_FIL[fc1]&BIT1)) &&                  */
        /* (((HVAS[fc1]&BIT5) || A[fc1]) && !BL[fc1] &&                                                     */
        /* (TTP[fc1] <= TTR[fc2] + ALTM[fc2] + (TGL_max[fc2] + T_max[to21])/10))) BAR[fc2] = TRUE;          */
        
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure afwikkeling deelconflicten gelijke start                                                       */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_gs_afw_vas(void)             /* Fik110801                                                        */
  {
    mulv i,j,x;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv to12,to21;
    bool voorw,xu1,xu2;
    
    for (i=0; i<aantal_dcf_gs; i++) {
      fc1  = dcf_gs[i][0];
      fc2  = dcf_gs[i][1];
      ma12 = dcf_gs[i][2];
      ma21 = dcf_gs[i][3];
      to12 = dcf_gs[i][4];
      to21 = dcf_gs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        
        /* Altijd gelijktijdig starten wachtstand                                                           */
        /* --------------------------------------                                                           */
        voorw = FALSE;
        if (GL[fc1] || TRG[fc1] || K[fc1] || RT[to21] || T[to21] ||
            GL[fc2] || TRG[fc2] || K[fc2] || RT[to12] || T[to12]) {
          BAR[fc1] |= !G[fc1];
          BAR[fc2] |= !G[fc2];
        }
        
        /* Opzetten cyc.prim.aanvraag indien gekoppelde richting versneld inkomt                            */
        /* ---------------------------------------------------------------------                            */
        if (R[fc1] && !TRG[fc1] && (HVAS[fc1]&BIT5) && (A[fc2]&BIT4)) {
          A[fc1] |= BIT2;
          A[fc1] |= BIT4;
          A[fc1] &= (~BIT3);
          A[fc1] &= (~BIT6);
        }
        
        if (R[fc2] && !TRG[fc2] && (HVAS[fc2]&BIT5) && (A[fc1]&BIT4)) {
          A[fc2] |= BIT2;
          A[fc2] |= BIT4;
          A[fc2] &= (~BIT3);
          A[fc2] &= (~BIT6);
        }
        
        /* set zonodig mee aanvragen                                                                        */
        /* -------------------------                                                                        */
        dcf_gs_mav();
        
        /* Beurt t.b.v. gelijke start                                                                       */
        /* --------------------------                                                                       */
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
        
        /* Uitstel t.b.v. gelijke start                                                                     */
        /* ----------------------------                                                                     */
        xu1 = xu2 = TRUE;
        if (RA[fc1] && !K[fc1] && !RT[to21] && !T[to21] && !X[fc1] && !RR[fc1] && !BL[fc1]
            || RV[fc1] && !TRG[fc1] && (!A[fc1] || !B[fc1])) xu1 = FALSE;
        if (RA[fc2] && !K[fc2] && !RT[to12] && !T[to12] && !X[fc2] && !RR[fc2] && !BL[fc2]
            || RV[fc2] && !TRG[fc2] && (!A[fc2] || !B[fc2])) xu2 = FALSE;
        
        if (xu1 || xu2) {
          if (!G[fc1]) X[fc1] |= BIT3;
          if (!G[fc2]) X[fc2] |= BIT3;
        }
        
        /* Altijd gelijktijdig overslag                                                                     */
        /* ----------------------------                                                                     */
        for (j=0; j<=1; j++) {
          if ((X[dcf_gs[i][j]]&BIT1) || G[dcf_gs[i][j]] && (RW[dcf_gs[i][j]]&BIT1)) {
            for (x=0; x<=1; x++) {
              if (x != j) {
                if ((X[dcf_gs[i][j]]&BIT1) || !G[dcf_gs[i][x]]) {
                  if ((HVAS[dcf_gs[i][j]]&BIT7) == (HVAS[dcf_gs[i][x]]&BIT7)) {
                    HVAS[dcf_gs[i][x]] |= BIT3;
                    TTP[dcf_gs[i][x]]  = 255;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN SYNCHROON START TIJDENS HALFSTAR BEDRIJF                                   */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal FCA[] tbv deelconflicten voorstart                                                      */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_vas_fca(void)             /* Fik110901                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (VASR[fc1] == VASR[fc2]) {
          if (R[fc1] && A[fc1] && !(A[fc1]&BIT7) || (HVAS[fc1]&BIT5)) FCA[fc2] |= BIT1;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer primaire realisatie tbv deelconflicten synchroon start                               */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_vas_prm(void)             /* Fik110901                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        if (VASR[fc1] == VASR[fc2]) {
          if (G[fc1] && (HVAS[fc1]&BIT4)) {
            if (G[fc2] && !(HVAS[fc2]&BIT0) && !(HVAS[fc2]&BIT3) && !(HVAS[fc2]&BIT4)) {
              HVAS[fc2] |= BIT4;
              RW[fc2]   |= BIT1;
              A[fc2] &= (~BIT4);
            }
          }
          if (G[fc2] && (HVAS[fc2]&BIT4)) {
            if (G[fc1] && !(HVAS[fc1]&BIT0) && !(HVAS[fc1]&BIT3) && !(HVAS[fc1]&BIT4)) {
              HVAS[fc1] |= BIT4;
              RW[fc1]   |= BIT1;
              A[fc1] &= (~BIT4);
            }
          }
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure corrigeer BAR[] tbv deelconflicten synchroon start                                             */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_vas_bar(void)             /* Fik110901                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma21;
    mulv to12;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      ma21 = dcf_vs[i][3];
      to12 = dcf_vs[i][4];
      
      if ((fc1 != NG) && (fc2 != NG)) {
        /* altijd BAR[] indien geel of garantie rood                                                        */
        if (GL[fc1] || R[fc1] && TRG[fc1]) BAR[fc1] |= TRUE;
        if (GL[fc2] || R[fc2] && TRG[fc2]) BAR[fc2] |= TRUE;
        
        if (R[fc2] && !TRG[fc2] && BAR[fc2] && !BL[fc2] && (A[fc2] || SCH[ma21])) {
          if (!G[fc1]) BAR[fc1] |= TRUE;
        }
        
        /* if (!G[fc1] && !G[fc2] && BAR[fc2] && ((_FIL[fc2]&BIT0) || (_FIL[fc2]&BIT1)) &&                  */
        /* (((HVAS[fc2]&BIT5) || A[fc2]) && !BL[fc2] &&                                                     */
        /* (TTP[fc2] <= TTR[fc1] + ALTM[fc1] + (TGL_max[fc1] + T_max[to12])/10))) BAR[fc1] = TRUE;          */
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure afwikkeling deelconflicten synchroon start                                                     */
/* -------------------------------------------------------------------------------------------------------- */
  void dcf_vs_afw_vas(void)             /* Fik111023                                                        */
  {
    mulv i;
    mulv fc1,fc2;
    mulv ma12,ma21;
    mulv to12,vs21;
    bool xu1,xu2;
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      
      _DCF[fc1] &= (~BIT0);
      _DCF[fc1] &= (~BIT1);
      
      _DCF[fc2] &= (~BIT0);
      _DCF[fc2] &= (~BIT1);
      
      if (!RA[fc1]) _DCF[fc1] &= (~BIT7);
      if (!RA[fc2]) _DCF[fc2] &= (~BIT7);
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      ma12 = dcf_vs[i][2];
      ma21 = dcf_vs[i][3];
      to12 = dcf_vs[i][4];
      vs21 = dcf_vs[i][5];
      
      if ((fc1 != NG) && (fc2 != NG)) { /* fc1 alleen wachtstand als geen conflicten van fc2 aanwezig       */
        if (GL[fc1] || TRG[fc1] || K[fc1] ||
            GL[fc2] || TRG[fc2] || K[fc2] || RT[to12] || T[to12]) {
          BAR[fc1] |= !G[fc1];
        }
        
        /* Opzetten cyc.prim.aanvraag indien gekoppelde richting versneld inkomt                            */
        /* ---------------------------------------------------------------------                            */
        if (VASR[fc1] == VASR[fc2]) {
          if (R[fc1] && !TRG[fc1] && (HVAS[fc1]&BIT5) && (A[fc2]&BIT4)) {
            A[fc1] |= BIT2;
            A[fc1] |= BIT4;
            A[fc1] &= (~BIT3);
            A[fc1] &= (~BIT6);
          }
          
          if (R[fc2] && !TRG[fc2] && (HVAS[fc2]&BIT5) && (A[fc1]&BIT4)) {
            A[fc2] |= BIT2;
            A[fc2] |= BIT4;
            A[fc2] &= (~BIT3);
            A[fc2] &= (~BIT6);
          }
        }
        
        /* set zonodig mee aanvraag                                                                         */
        /* ------------------------                                                                         */
        if (RA[fc1] && !RR[fc1] && !K[fc1] && !BL[fc1] && SCH[ma21]) {
          if (R[fc2] && !TRG[fc2] && !K[fc2] && !RT[to12] && !T[to12] && !BL[fc2]) set_A(fc2);
        }
        
        /* Beurt t.b.v. synchroon start                                                                     */
        /* ----------------------------                                                                     */
        if ((RV[fc1] && B[fc1] || RA[fc1]) && A[fc1] && !RR[fc1] && !BL[fc1]) {
          if (R[fc2] && !TRG[fc2] && !K[fc2] && !RT[to12] && !T[to12] && !BL[fc2] && A[fc2]) {
            B[fc2] |= BIT3;
            RR[fc2] = FALSE;
            _DCF[fc1] |= BIT7;
            _DCF[fc2] |= BIT7;
          }
        }
        
        /* Uitstel t.b.v. gelijke start                                                                     */
        /* ----------------------------                                                                     */
        xu1 = xu2 = TRUE;
        if (RA[fc1] && !K[fc1] && !RT[vs21] && !T[vs21] && !X[fc1] && !RR[fc1] && !BL[fc1] &&
            ( G[fc2] || RV[fc2] && !TRG[fc2] && !K[fc2] && !RT[to12] && !T[to12] && (!A[fc2] || !B[fc2]))) xu1 = FALSE;
        
        if (RA[fc2] && !K[fc2] && !RT[to12] && !T[to12] && !BL[fc2]) xu2 = FALSE;
        
        if (xu1) {
          X[fc1] = TRUE;
          _DCF[fc1] |= BIT0;
        }
        if (xu2) {
          X[fc2] = TRUE;
          _DCF[fc2] |= BIT1;
        }
        
        /* Altijd gelijktijdig overslag                                                                     */
        /* ----------------------------                                                                     */
        if (VASR[fc1] == VASR[fc2]) {
          if ((X[fc1]&BIT1) || G[fc1] && (RW[fc1]&BIT1)) {
            if ((X[fc1]&BIT1) || !G[fc2]) {
              if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
                HVAS[fc2] |= BIT3;
                TTP[fc2]  = 255;
              }
            }
          }
        }
      }
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      if (G[fc2] && (RV[fc1] && B[fc1] && !RR[fc1] && !BL[fc1] || RA[fc1])) {
        YM[fc2] = TRUE;
        _DCF[fc2] |= BIT2;
      }
    }
    
    for (i=0; i<aantal_dcf_vs; i++) {
      fc1  = dcf_vs[i][0];
      fc2  = dcf_vs[i][1];
      if (G[fc1] && (_DCF[fc1]&BIT2) && RA[fc2] && !BL[fc2]) {
        if (MG[fc1]) Z[fc1] = TRUE;     /* fc1 nog geen OV prioriteit                                       */
        if (VG[fc1] && (KG[fc1] > ALTM[fc1])) FM[fc1] = TRUE;
      }
    }
  }
  
