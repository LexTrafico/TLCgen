/* ======================================================================================================== */
/* Fick CCOL generator                                                              Versie 1.7 / 4 sep 2020 */
/* -------------------------------------------------------------------------------------------------------- */
/* Kruispuntnummer:                      0001                                                               */
/* Locatie:                              -                                                                  */
/* Plaats:                               -                                                                  */
/* Ontwerp:                              -                                                                  */
/* User interface:                       Traffick Solutions - Ferry van den Heuvel                          */
/* -------------------------------------------------------------------------------------------------------- */
/* pro_fik.h                                                                                                */
/* Include-file tbv genereren van kruispunt 0001.                                                           */
/* Versie:                               1.0                                                                */
/* Door:                                 -                                                                  */
/* Datum generatie:                      07-09-2020                                                         */
/*  Tijd generatie:                      08:32:25 (+02:00)                                                  */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure header file                                                                                    */
/* -------------------------------------------------------------------------------------------------------- */
/* Datum         : 1 oktober 2019                                                                           */
/* Versie        : 1.0                                                                                      */
/* Programmeur   : Marcel Fick                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  
  count TI_pointer(count fc1, count fc2); /* Fik191001                                                      */
  mulv TI_max(count fc1, count fc2);    /* Fik191001                                                        */
  bool TI(count fc1, count fc2);        /* Fik191001                                                        */
  mulv TI_timer(count fc);              /* Fik191001                                                        */
  
  bool somfbk(count fc);                /* Fik980326                                                        */
  bool _somfbk(count fc);               /* Fik100312                                                        */
  bool somgk(count fc);                 /* Fik020612                                                        */
  bool somak(count fc);                 /* Fik070829                                                        */
  bool somakgr(count fc);               /* Fik180131                                                        */
  
  void proc_init_reg(void);             /* Fik100206                                                        */
  void init_sgd(void);                  /* Fik050908                                                        */
  void proc_garantie_tijden(void);      /* Fik070313                                                        */
  void proc_reset_start(void);          /* Fik100206                                                        */
  void proc_reset_einde(void);          /* Fik090211                                                        */
  void proc_klk_gr_r(void);             /* Fik050908                                                        */
  void proc_bepaal_ttr(void);           /* Fik090306                                                        */
  void proc_max_wt(void);               /* Fik100301                                                        */
  
  void proc_fc_instel(                  /* Fik170831                                                        */
    count fc,                           /* fasecyclus                                                       */
    count alt,                          /* prm agar##                                                       */
    count dfst,                         /* prm dfst##                                                       */
    count kap,                          /* prm prag##                                                       */
    count terug,                        /* prm pgtk##                                                       */
    count _hvi,                         /* prm hv##                                                         */
    count _hdub,                        /* t hldp uitmeldbewaking                                           */
    count altva);                       /* prm altva tijdens CRSV                                           */
  
  void proc_aanvragen(                  /* Fik120728                                                        */
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
    count prm12);                       /* parameter de12                                                   */
  
  void proc_aanv_drk(                   /* Fik120728                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* cyc.aanvraag                                                     */
    count schdst,                       /* det.storing                                                      */
    count drk,                          /* drukknop                                                         */
    count mmdrk,                        /* MM  drk                                                          */
    count avr);                         /* PRM drk                                                          */
  
  void proc_aanv_drk1(                  /* Fik090306                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* cyc.aanvraag                                                     */
    count schdst,                       /* det.storing                                                      */
    count dk1,                          /* drukknp 1                                                        */
    count mm1,                          /* MM  drk 1                                                        */
    count avr1);                        /* SCH drk 1                                                        */
  
  void proc_aanv_drk2(                  /* Fik090306                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* cyc.aanvraag                                                     */
    count schdst,                       /* det.storing                                                      */
    count dk1,                          /* drukknp 1                                                        */
    count mm1,                          /* MM  drk 1                                                        */
    count avr1,                         /* SCH drk 1                                                        */
    count dk2,                          /* drukknp 2                                                        */
    count mm2,                          /* MM  drk 2                                                        */
    count avr2);                        /* SCH drk 2                                                        */
  
  void proc_aanv_cyc(                   /* Fik090306                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar);                        /* cyc.aanvraag                                                     */
  
  void set_A(count fc);                 /* Fik090210                                                        */
  void set_fts_MA(                      /* Fik180309                                                        */
    count fc1,                          /* fasecyclus MEE                                                   */
    count fc2);                         /* fasecyclus MET                                                   */
  
  void proc_verlengen(                  /* Fik120602                                                        */
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
    count prm12);                       /* parameter de12                                                   */
  
  void proc_vag4(                       /* Fik171016                                                        */
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
    count prm12);                       /* parameter de12                                                   */
  
  void rg_meetpunt(                     /* Fik120602                                                        */
    count det1,                         /* detector 1 vanaf de stopstreep                                   */
    count det2,                         /* detector 2 vanaf de stopstreep                                   */
    count trgv,                         /* richtinggevoelige hiaattijd                                      */
    count traf,                         /* max.onbezettijd detector 2                                       */
    count _rgv);                        /* MM rgv meetpunt bezet                                            */
  
  void proc_aanv_rga(                   /* Fik120804                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* PRM cyclische aanvraag                                           */
    count _rgv,                         /* MM  rga meetpunt bezet                                           */
    count avr);                         /* PRM aanvraag toegestaan                                          */
  
  void rga_aanvraag(                    /* Fik100322                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cyar,                         /* PRM cyclische aanvraag                                           */
    count _rgv,                         /* MM  rga meetpunt bezet                                           */
    count sch);                         /* SCH aanvraag toegestaan                                          */
  
  void rgv_verlengen(                   /* Fik120602                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tbep,                         /* beperkt meetkriterium                                            */
    count _rgv,                         /* MM  rgv meetpunt bezet                                           */
    count avg);                         /* PRM verlengfunctie                                               */
  
  void proc_vag4_rgv(                   /* Fik120602                                                        */
    count fc,                           /* fasecyclus                                                       */
    count max4,                         /* T VAG4                                                           */
    count maxvt,                        /* maximale volgtijd                                                */
    count _rgv,                         /* MM  rgv meetpunt bezet                                           */
    count _vt_rgv,                      /* MM  rgv meetpunt volgtijd                                        */
    count avg);                         /* PRM verlengfunctie                                               */
  
  void proc_det_mvt(                    /* Kinzel 17-05-2019                                                */
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
    count tdhv3);                       /* index tijdelement vervangende hiaattijd 3e verweglus             */
  
  void proc_det_fts(                    /* Kinzel 17-05-2019                                                */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count dk,                           /* index drukknop                                                   */
    count de1,                          /* index koplus 1                                                   */
    count de2);                         /* index koplus 2                                                   */
  
  void proc_det_vtg(                    /* Kinzel 17-05-2019                                                */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count dk1,                          /* index drukknop 1                                                 */
    count dk2);                         /* index drukknop 2                                                 */
  
  void proc_det_bus(                    /* Kinzel 17-05-2019                                                */
    count fc,                           /* index fasecyclus                                                 */
    count sch_aan,                      /* index schakelaar maatregel bij storing                           */
    count ta,                           /* index tijdelement tijdvertraging cyclische aanvraag              */
    count de1,                          /* index koplus 1                                                   */
    count de2);                         /* index koplus 2                                                   */
  
  void proc_LHOVRA_R(                   /* Fik100305                                                        */
    count fc,                           /* fasecyclus                                                       */
    count sch);                         /* schakelaar                                                       */
  
  void proc_wgr_sgd(                    /* Fik100206                                                        */
    count fc,                           /* fasecyclus                                                       */
    count wgr);                         /* prm wgr                                                          */
  
  bool hf_mvg(void);                    /* Fik070725                                                        */
  
  void proc_mvg_sgd(                    /* Fik100206                                                        */
    count fc,                           /* fasecyclus                                                       */
    count mvg);                         /* prm mvg                                                          */
  
  void proc_mee_mvg_sgd(                /* Fik100206                                                        */
    count fc1,                          /* fasecyclus van                                                   */
    count fc2,                          /* fasecyclus met                                                   */
    count sch);                         /* schakelaar                                                       */
  
  void proc_maxgr_sgd(void);            /* Fik100206                                                        */
  void proc_rea_sgd(void);              /* Fik100201                                                        */
  void proc_ppv_sgd(void);              /* Fik070815                                                        */
  void proc_TEG_sgd(void);              /* Fik100206                                                        */
  void proc_TTP_primair(void);          /* Fik100206                                                        */
  void proc_TTP_versneld(void);         /* Fik100206                                                        */
  void proc_bepaal_TTPX(void);          /* Fik100330                                                        */
  void proc_BAR_sgd(void);              /* Fik090616                                                        */
  void proc_BAR_wtv_sgd(void);          /* Fik100313                                                        */
  void proc_ALT_sgd(void);              /* Fik091110                                                        */
  void proc_nblok_sgd(void);            /* Fik001104                                                        */
  void proc_fixatie(void);              /* Fik070815                                                        */
  
  void proc_fc_info(                    /* Fik191210                                                        */
    count fc,                           /* fasecyclus                                                       */
    count _sgd,                         /* mm sgd##                                                         */
    count _ttp,                         /* mm ttp##                                                         */
    count _teg,                         /* mm teg##                                                         */
    count _hki);                        /* mm hki##                                                         */
  
  bool proc_schakelklok(                /* Fik040223                                                        */
    count uur1,                         /* inschakeltijd  uren                                              */
    count min1,                         /* inschakeltijd  minuten                                           */
    count uur2,                         /* uitschakeltijd uren                                              */
    count min2,                         /* uitschakeltijd minuten                                           */
    count dag);                         /* dagcode                                                          */
  
  void maxgroen(                        /* Fik100719                                                        */
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
    count dvm11,                        /* prm tvdmv11 -> DVM progr.11                                      */
    count dvm12,                        /* prm tvdmv12 -> DVM progr.12                                      */
    count dvm13,                        /* prm tvdmv13 -> DVM progr.13                                      */
    count dvm14,                        /* prm tvdmv14 -> DVM progr.14                                      */
    count dvm15,                        /* prm tvdmv15 -> DVM progr.15                                      */
    count dvm16);                       /* prm tvdmv16 -> DVM progr.16                                      */
  
  void proc_mee_rea_sgd(                /* Fik070818                                                        */
    count fc1,                          /* fasecyclus MEE                                                   */
    count fc2,                          /* fasecyclus MET                                                   */
    count sch);                         /* schakelaar                                                       */
  
  void proc_PRI_sgd(void);              /* Fik120107                                                        */
  void proc_Xpr_sgd(void);              /* Fik070815                                                        */
  void proc_TTP_prioriteit(void);       /* Fik100206                                                        */
  
  void proc_buf_bus_kar(                /* Fik100301                                                        */
    count fc,                           /* fasecyclus                                                       */
    count inmv,                         /* T inmeldvertraging                                               */
    count afkv,                         /* T afkapvertraging                                                */
    count uitb,                         /* T uitmeldbewaking                                                */
    count tbl,                          /* T blokkeer herhaalde prio.                                       */
    count priv,                         /* PRM prioriteit te vroeg                                          */
    count prit,                         /* PRM prioriteit op tijd                                           */
    count pril,                         /* PRM prioriteit te laat                                           */
    count awt1,                         /* aanwezigheidsduur 1e bus                                         */
    count awt2,                         /* aanwezigheidsduur 2e bus                                         */
    count awt3,                         /* aanwezigheidsduur 3e bus                                         */
    count stp1,                         /* stiptheid 1e bus                                                 */
    count stp2,                         /* stiptheid 2e bus                                                 */
    count stp3);                        /* stiptheid 3e bus                                                 */
  
  void proc_buf_pel_pri(                /* Fik100301                                                        */
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
    count awt3);                        /* aanwezigheidsduur 3e pel                                         */
  
  void proc_pel_uit(                    /* Fik100306                                                        */
    count fc,                           /* fasecyclus                                                       */
    count de1,                          /* koplus 1                                                         */
    count de2,                          /* koplus 2                                                         */
    count de3,                          /* koplus 3                                                         */
    count tmeet,                        /* T meetperiode                                                    */
    count tmaxth,                       /* T max.hiaat                                                      */
    count grens,                        /* PRM grenswaarde                                                  */
    count us_uit,                       /* koppelsignaal                                                    */
    count mvtg,                         /* MM aantal vtg                                                    */
    count muit);                        /* MM uitsturing aktief                                             */
  
  void proc_fts_prio(                   /* Fik100304                                                        */
    count fc,                           /* fasecyclus                                                       */
    count dk1,                          /* MM drukknop 1                                                    */
    count dk2,                          /* MM drukknop 2                                                    */
    count ogwt,                         /* T min.wachttijd                                                  */
    count tbl,                          /* T blokkeringstijd                                                */
    count prio);                        /* PRM prio code                                                    */
  
  void proc_hlpd_ingreep(void);         /* Fik100307                                                        */
  void proc_hlpd_naloop(                /* Fik100308                                                        */
    count tnl12,                        /* T naloop arm 1 -> arm 2                                          */
    const mulv _arm1,                   /* constante: arm 1                                                 */
    const mulv _arm2);                  /* constante: arm 2                                                 */
  
  void proc_aanv_hlpd(void);            /* Fik100308                                                        */
  void proc_forceer_hlpd(void);         /* Fik100308                                                        */
  
  void proc_def_kop_lvk(                /* Fik100129                                                        */
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
    count kpt78);                       /* t koppeltijd fc7 ->fc8 (eg -> eg)                                */
  
  void proc_corr_ttr_vtg(void);         /* Fik100129                                                        */
  void proc_ttp_vtg_sgd(void);          /* Fik100129                                                        */
  void proc_mav_vtg(void);              /* Fik100129                                                        */
  void proc_fk_vtg(void);               /* Fik120617                                                        */
  void proc_alt_vtg_sgd(void);          /* Fik100129                                                        */
  void proc_Xvtg_sgd(void);             /* Fik100129                                                        */
  void proc_rea_vtg_sgd(void);          /* Fik100129                                                        */
  void proc_teg_vtg_sgd(void);          /* Fik100201                                                        */
  void proc_bar_vtg_sgd(void);          /* Fik100201                                                        */
  void proc_afw_lvk_sgd(void);          /* Fik100201                                                        */
  
  void proc_wait_fts(                   /* Fik070314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count ws,                           /* waitsignaal                                                      */
    count dk);                          /* drukknop                                                         */
  
  void proc_wait_vtg(                   /* Fik070314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count ws,                           /* waitsignaal                                                      */
    count dk);                          /* drukknop                                                         */
  
  void proc_ratel(                      /* Fik100205                                                        */
    count fc,                           /* fasecyclus                                                       */
    count dk,                           /* drukknop                                                         */
    count dkmm);                        /* MM drukknop                                                      */
  
  void proc_KARov_melding(              /* Fik140322                                                        */
    count Sinm,                         /* Inm.voor CCOL applicatie                                         */
    count Suit);                        /* Uit.voor CCOL applicatie                                         */
  
  void proc_KARhd_melding(              /* Fik140322                                                        */
    count Sinm,                         /* Inm.voor CCOL applicatie                                         */
    count Suit);                        /* Uit.voor CCOL applicatie                                         */
  
  void ringbufferKAR(void);             /* Fik141109                                                        */
  void bewaak_OG_kar(void);             /* Fik100204                                                        */
  void proc_vlog_mon5_buffer(void);     /* Fik120526                                                        */
  
  void init_kar_log(void);              /* Fik090208                                                        */
  void log_time_stamp(void);            /* Fik090801                                                        */
  void log_kar_event(void);             /* Fik090101                                                        */
  
  void log_kar_info(                    /* Fik090101                                                        */
    count fc,                           /* LOG fasecyclus                                                   */
    count de1,                          /* LOG detector 1                                                   */
    count de2,                          /* LOG detector 2                                                   */
    count de3,                          /* LOG detector 3                                                   */
    count prm);                         /* LOG instelling                                                   */
  
  void log_fc_event(void);              /* Fik090101                                                        */
  void log_de_event(void);              /* Fik090101                                                        */
  
  void dvm_bus_max_pri(                 /* Fik111126                                                        */
    count fc,                           /* fasecyclus                                                       */
    count dvm_pr1,                      /* PRM ovdvm##1                                                     */
    count dvm_pr2,                      /* PRM ovdvm##2                                                     */
    count dvm_pr3,                      /* PRM ovdvm##3                                                     */
    count dvm_pr4,                      /* PRM ovdvm##4                                                     */
    count dvm_pr5,                      /* PRM ovdvm##5                                                     */
    count dvm_pr6);                     /* PRM ovdvm##6                                                     */
  
  void dvm_pel_max_pri(                 /* Fik111126                                                        */
    count fc,                           /* fasecyclus                                                       */
    count dvm_pr1,                      /* PRM peldvm##1                                                    */
    count dvm_pr2,                      /* PRM peldvm##2                                                    */
    count dvm_pr3,                      /* PRM peldvm##3                                                    */
    count dvm_pr4,                      /* PRM peldvm##4                                                    */
    count dvm_pr5,                      /* PRM peldvm##5                                                    */
    count dvm_pr6);                     /* PRM peldvm##6                                                    */
  
  void proc_bepaal_prg_DVM(void);       /* Fik100215                                                        */
  void proc_bepaal_TDH(void);           /* Fik100215                                                        */
  
  void proc_us_ov(                      /* Fik120211                                                        */
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
    count povl);                        /* US bus te laat  prioriteit                                       */
  
  void file_meting(                     /* Fik100403                                                        */
    count file,                         /* file-melding                                                     */
    count afval,                        /* hulpfunctie  !file                                               */
    count fdet,                         /* file-detector                                                    */
    count dbez,                         /* bezettijd ->  file                                               */
    count drij,                         /* rijtijd   -> !file                                               */
    count dafv,                         /* afvalvertraging                                                  */
    count tab);                         /* afvalbewaking TYD                                                */
  
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
    count tfiprg);                      /* duur file programma                                              */
  
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
    count prm2,                         /* ingreep fc1                                                      */
    count dos2,                         /* doseermaximum fc2                                                */
    count blk2,                         /* duur blokkeer fc2                                                */
    count fc3,                          /* 1e fasecyclus                                                    */
    count prm3,                         /* ingreep fc1                                                      */
    count dos3,                         /* doseermaximum fc3                                                */
    count blk3);                        /* duur blokkeer fc3                                                */
  
  void proc_Xpr_sgd_file(void);         /* Fik100206                                                        */
  
  void proc_hki_aktief(                 /* Fik070812                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count hki_gew,                      /* hki gewenst                                                      */
    count hki_akt);                     /* hki aktief                                                       */
  
  void proc_kies_hki_sgd(               /* Fik070818                                                        */
    count fc3,                          /* volgrichting                                                     */
    count fc1,                          /* voedende fc1                                                     */
    count fc2,                          /* voedende fc2                                                     */
    count kopgew13,                     /* hki gew.fc1->3                                                   */
    count kopgew23,                     /* hki gew.fc2->3                                                   */
    count koppri13,                     /* aktiveer hki13                                                   */
    count koppri23,                     /* aktiveer hki23                                                   */
    count kopakt13,                     /* hki13 = aktief                                                   */
    count kopakt23);                    /* hki23 = aktief                                                   */
  
  void voorrang_hki(                    /* Fik100316                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2);                         /* fasecyclus 2                                                     */
  
  void proc_TTR_hki_sgd(                /* Fik070820                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count fc3,                          /* fasecyclus 3                                                     */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kpakt13,                      /* kop.aktief13                                                     */
    count kvs12,                        /* vst.fc1->fc2                                                     */
    count kvs13);                       /* vst.fc1->fc3                                                     */
  
  void proc_Xpr_hki_sgd(                /* Fik060326                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count fc3,                          /* fasecyclus 3                                                     */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kpakt13,                      /* kop.aktief13                                                     */
    count fc4,                          /* 2e voeding van fc2                                               */
    count fc5,                          /* 2e voeding van fc3                                               */
    count kpakt42,                      /* kop.aktief42                                                     */
    count kpakt53);                     /* kop.aktief53                                                     */
  
  void proc_Xhk_sgd(                    /* Fik070815                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count fc3,                          /* fasecyclus 3                                                     */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kpakt13,                      /* kop.aktief13                                                     */
    count kvs12,                        /* vst.fc1->fc2                                                     */
    count kvs13);                       /* vst.fc1->fc3                                                     */
  
  void proc_rea_hki_sgd(                /* Fik060423                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count fc3,                          /* fasecyclus 3                                                     */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kpakt13);                     /* kop.aktief13                                                     */
  
  void proc_TEG_hki_sgd(                /* Fik060414                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count kopakt,                       /* koppeling aktief                                                 */
    count kpt1,                         /* k.tijd vanaf sg1                                                 */
    count kpt2,                         /* k.tijd vanaf eg1                                                 */
    count kpt3);                        /* dm fc2 na hki                                                    */
  
  void proc_TTP_hki_sgd(                /* Fik060326                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count fc3,                          /* fasecyclus 3                                                     */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kpakt13,                      /* kop.aktief13                                                     */
    count kvs12,                        /* vst.fc1->fc2                                                     */
    count kvs13);                       /* vst.fc1->fc3                                                     */
  
  void proc_TEG2hki_sgd(                /* Fik060326                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count kpt1,                         /* k.tijd vanaf sg1                                                 */
    count kpt2,                         /* k.tijd vanaf eg1                                                 */
    count kpt3);                        /* dm fc2 na hki                                                    */
  
  void proc_BAR_bam(                    /* Fik060326                                                        */
    count fc1);                         /* volgend                                                          */
  
  void proc_BAR_bam_hki(void);          /* Fik040425                                                        */
  void BAR_hki_sgd(                     /* Fik070815                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count kpt1,                         /* k.tijd vanaf sg1                                                 */
    count kpt2,                         /* k.tijd vanaf eg1                                                 */
    count kpt3);                        /* dm fc2 na hki                                                    */
  
  void proc_BAR_hki_sgd(                /* Fik070815                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count kpt1,                         /* k.tijd vanaf sg1                                                 */
    count kpt2,                         /* k.tijd vanaf eg1                                                 */
    count kpt3);                        /* dm fc2 na hki                                                    */
  
  void proc_ALT_hki_sgd(                /* Fik090129                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count fc3,                          /* fasecyclus 3                                                     */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kpakt13);                     /* kop.aktief13                                                     */
  
  void proc_hki_sgd(                    /* Fik070818                                                        */
    count fc1,                          /* fasecylus 1                                                      */
    count fc2,                          /* fasecylus 2                                                      */
    count kpakt12,                      /* kop.aktief12                                                     */
    count kvs12,                        /* vst. fc1 tov fc2                                                 */
    count kpt21,                        /* k.tijd vanaf sg1                                                 */
    count kpt22,                        /* k.tijd vanaf eg1                                                 */
    count kpt23,                        /* dm fc2 na hki                                                    */
    count hki12,                        /* koplus fc1 bezet                                                 */
    count fc3,                          /* fasecylus 3                                                      */
    count kpakt13,                      /* kop.aktief13                                                     */
    count kvs13,                        /* vst. fc1 tov fc3                                                 */
    count kpt31,                        /* k.tijd vanaf sg1                                                 */
    count kpt32,                        /* k.tijd vanaf eg1                                                 */
    count kpt33,                        /* dm fc3 na hki                                                    */
    count hki13);                       /* koplus fc1 bezet                                                 */
  
  void proc_blwt(void);                 /* Fik100312                                                        */
  void proc_blwt_vov(void);             /* Fik100312                                                        */
  void proc_bepaal_TWV(void);           /* Fik100313                                                        */
  void proc_corr_TWV(void);             /* Fik100312                                                        */
  void proc_wt_voorspeller(void);       /* Fik070922                                                        */
  
  void proc_wt_us(                      /* Fik070919                                                        */
    count fc,                           /* fasecyclus                                                       */
    count us0,                          /* bit 0                                                            */
    count us1,                          /* bit 1                                                            */
    count us2,                          /* bit 2                                                            */
    count us3,                          /* bit 3                                                            */
    count us4,                          /* bit 4                                                            */
    count _bus);                        /* bus aanwezig                                                     */
  
  void proc_vas_progkeuze_slaaf(        /* datum 22-07-2010                                                 */
    count tt1,                          /* Index timer tijdvertr. omschakelen                               */
    count mckode);                      /* Index geheugenelement met kode complexcoordinator                */
  
  void proc_vas_stappenraster(bool ssync);
  void proc_bepaal_MKODE(void);
  void proc_vas_progkeuze_master(count tt1);
  void proc_vas_master_raster(count tt_s);
  
  void init_vas(void);                  /* Fik100226                                                        */
  void proc_cyc_vas(                    /* Fik110314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cvas1,                        /* PRM[cvas1]                                                       */
    count cvas2);                       /* PRM[cvas2]                                                       */
  
  void proc_cyc_vas_dvm(                /* Fik110314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count cdvm1,                        /* PRM[cdvm1]                                                       */
    count cdvm2);                       /* PRM[cdvm2]                                                       */
  
  void proc_vas_parm(                   /* Fik050625                                                        */
    count ctyd,                         /* Cyclustijd                                                       */
    count insc,                         /* Inschakelpunt                                                    */
    count omsc,                         /* Omschakelpunt                                                    */
    count sync);                        /* Synchronisatiepunt                                               */
  
  void proc_corr_vasr(                  /* Fik100414                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tl1s,                         /* start primair 1e rea.                                            */
    count tl1e,                         /* einde primair 1e rea.                                            */
    count tl2s,                         /* start primair 2e rea.                                            */
    count tl2e);                        /* einde primair 2e rea.                                            */
  
  void proc_akt_vas(                    /* Fik020218                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tl1,                          /* einde uitstel                                                    */
    count tl2,                          /* start primair                                                    */
    count tl3,                          /* start verleng                                                    */
    count tl4,                          /* einde primair                                                    */
    count tl5);                         /* einde groen                                                      */
  
  void proc_prim_vas(void);
  void proc_bteg_vas(void);
  void proc_bttp_vas(void);
  void proc_altt_vas(void);
  void proc_altr_vas(void);
  void proc_maxgr_vas(void);
  
  void proc_mee_rea_vas(                /* Fik100206                                                        */
    count fc1,                          /* fasecyclus MEE                                                   */
    count fc2,                          /* fasecyclus MET                                                   */
    count sch);                         /* schakelaar                                                       */
  
  void proc_wgr_vas(                    /* Fik020313                                                        */
    count fc,                           /* Fik100206                                                        */
    count wgr);
  
  bool hf_mvg_vas(void);
  bool var_mg_vas(count fc);
  void proc_mvg_vas(                    /* Fik100414                                                        */
    count fc,                           /* fasecyclus                                                       */
    count mvg);                         /* sch mvg                                                          */
  
  void proc_mee_mvg_vas(                /* Fik100414                                                        */
    count fc1,                          /* fasecyclus van                                                   */
    count fc2,                          /* fasecyclus met                                                   */
    count sch);                         /* schakelaar                                                       */
  
  void proc_vas_res_bus(                /* Fik100517                                                        */
    count awt1,                         /* aanwezigheidsduur 1e bus                                         */
    count awt2,                         /* aanwezigheidsduur 2e bus                                         */
    count awt3,                         /* aanwezigheidsduur 3e bus                                         */
    count stp1,                         /* stiptheidscode 1e bus                                            */
    count stp2,                         /* stiptheidscode 2e bus                                            */
    count stp3);                        /* stiptheidscode 3e bus                                            */
  
  void proc_vas_res_pel(                /* Fik100314                                                        */
    count ap1,                          /* awt peloton 1                                                    */
    count ap2,                          /* awt peloton 2                                                    */
    count ap3);                         /* awt peloton 3                                                    */
  
  void proc_fca_vtgvas(void);           /* Fik100616                                                        */
  void proc_prm_vtgvas(void);           /* Fik100616                                                        */
  void proc_bar_vtgvas(void);           /* Fik100618                                                        */
  void proc_alt_vtgvas(void);           /* Fik100618                                                        */
  void proc_teg_vtgvas(void);           /* Fik100621                                                        */
  void proc_afw_lvk_vas(void);          /* Fik100621                                                        */
  
  void proc_hskp_vas(                   /* Fik100309                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kopsig,                       /* koppelsignaal                                                    */
    count ingr,                         /* PRM ingreep aktief                                               */
    count afstrt);                      /* T afstand rijtijd                                                */
  
  void proc_hskp_vas_dvm(               /* Fik100316                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kopsig,                       /* koppelsignaal                                                    */
    count ingr,                         /* PRM ingreep aktief                                               */
    count afstrt);                      /* T afstand rijtijd                                                */
  
  void proc_hskp_vas_uit(               /* Fik100309                                                        */
    count fc,                           /* fasecyclus                                                       */
    count kopsig,                       /* koppelsignaal                                                    */
    count mintijd);                     /* T min.duur ks                                                    */
  
  void proc_vas_vov_init(void);         /* Fik100314                                                        */
  void init_vas_vov(void);              /* Fik090207                                                        */
  void proc_akt_vas_vov(                /* Fik091224                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tl1,                          /* einde uitstel                                                    */
    count tl2,                          /* start primair                                                    */
    count tl3,                          /* start verleng                                                    */
    count tl4,                          /* einde primair                                                    */
    count tl5);                         /* einde groen                                                      */
  
  void proc_prim_vas_vov(void);         /* Fik090210                                                        */
  void proc_corr_akt_vas(void);         /* Fik100314                                                        */
  bool uitstel_aanvraag(count fc);
  void proc_bus_aanv_vas_vov(           /* Fik090530                                                        */
    count fc,                           /* fasecyclus                                                       */
    count awt,                          /* aanwezigheidstijd                                                */
    count inmv,                         /* inmeldvertraging                                                 */
    count uitb,                         /* uitmeldbewaking                                                  */
    count awt4);                        /* verlosaanvraag                                                   */
  
  void proc_bus_verl_vas(               /* Fik090724                                                        */
    count fc,                           /* fasecyclus                                                       */
    count awt,                          /* aanwezigheidstijd                                                */
    count inmv,                         /* inmeldvertraging                                                 */
    count awt4);                        /* verlosaanvraag                                                   */
  
  mulv bepaal_ttov(mulv vas2, mulv k);
  void proc_alto_vas(                   /* PH020210 - Fik091224                                             */
    count fc,                           /* Fasecyclus t.b.v. OV ingreep                                     */
    count toviv,                        /* Inmeldvertraging                                                 */
    count tovub,                        /* Afstandrijtijd                                                   */
    count awt1,                         /* Aanwezigheidstijd oudste bus                                     */
    const mulv corr,                    /* Rekenkundige correctie                                           */
    const mulv altos,                   /* Afbreekmethode gekoppelde richtingen                             */
    count awt4);                        /* Verlosaanvraag                                                   */
  
  void proc_corr_ovak_vtg(void);
  void proc_def_vas_vov(
    count fc,
    count inmv,
    count afrt,
    count meat1,
    count meat4);
  
  bool lw_prio_vas_1(
    int  fc1,
    mulv awt1,
    mulv inmv1,
    mulv afrt1,
    int  fc2,
    mulv awt2,
    mulv inmv2,
    mulv afrt2);
  
  void conflict_ov_vas(void);
  void proc_alto2_vas(                  /* PH020218                                                         */
    count fc,                           /* Richting t.g.v. OV ingreep                                       */
    count toviv,                        /* Inmeldvertraging                                                 */
    count awt,                          /* Aanwezigheidstijd bus                                            */
    const mulv altos,                   /* Afbreekmethode gekoppelde richtingen                             */
    count awt4);                        /* verlosaanvraag                                                   */
  
  void proc_blok_confl_vas(void);
  bool ov2ttp(count fc);
  
  void proc_vas_bus_vov(                /* Fik100314                                                        */
    count fc,                           /* fasecyclus                                                       */
    count inmv,                         /* T inmeldvertraging                                               */
    count uitb,                         /* T uitmeldbewaking                                                */
    count priv,                         /* PRM prioriteit te vroeg                                          */
    count prit,                         /* PRM prioriteit op tijd                                           */
    count pril,                         /* PRM prioriteit te laat                                           */
    count awt1,                         /* aanwezigheidsduur 1e bus                                         */
    count awt2,                         /* aanwezigheidsduur 2e bus                                         */
    count awt3,                         /* aanwezigheidsduur 3e bus                                         */
    count stp1,                         /* stiptheidscode 1e bus                                            */
    count stp2,                         /* stiptheidscode 2e bus                                            */
    count stp3);                        /* stiptheidscode 3e bus                                            */
  
  void proc_crtr_vas_0(
    count fc,
    count dkop,
    count dlang);
  
  void proc_crtr_vastl(
    count fc,
    count d1_uit,
    count d2_uit,
    count d3_uit,
    count d1_verl,
    count d2_verl,
    count d3_verl,
    count d1_in,
    count d2_in,
    count d3_in);
  
  void proc_blwt_vov(void);
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN GELIJKE START TIJDENS LOKAAL BEDRIJF                                       */
/* ======================================================================================================== */
  void dcf_gs_def(                      /* Fik110801                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count ma12,                         /* sch meerealisatie fc1 met fc2                                    */
    count ma21,                         /* sch meerealisatie fc1 met fc2                                    */
    count ti12,                         /* t intergroen- of ontruimingstijd fc1 -> fc2                      */
    count ti21);                        /* t intergroen- of ontruimingstijd fc2 -> fc1                      */
  
  bool dcf_gelijk(                      /* Fik110801                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2);                         /* fasecyclus 2                                                     */
  
  void dcf_gs_confl(void);              /* Fik110801                                                        */
  void dcf_gs_corr_ttr(void);           /* Fik110801                                                        */
  void dcf_gs_corr_ttp(void);           /* Fik110801                                                        */
  void dcf_gs_mav(void);                /* Fik110801                                                        */
  void dcf_gs_rea(void);                /* Fik110801                                                        */
  void dcf_gs_bar(void);                /* Fik110801                                                        */
  void dcf_gs_afw_sgd(void);            /* Fik110801                                                        */
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN GELIJKE START TIJDENS HALFSTAR BEDRIJF                                     */
/* ======================================================================================================== */
  void dcf_gs_vas_fca(void);            /* Fik110801                                                        */
  void dcf_gs_vas_prm(void);            /* Fik110801                                                        */
  void dcf_gs_vas_bar(void);            /* Fik110801                                                        */
  void dcf_gs_afw_vas(void);            /* Fik110801                                                        */
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN SYNCHROON START (= VOORSTART) TIJDENS LOKAAL BEDRIJF                       */
/* ======================================================================================================== */
  void dcf_vs_def(                      /* Fik110901                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2,                          /* fasecyclus 2                                                     */
    count ma12,                         /* sch meerealisatie fc1 met fc2                                    */
    count ma21,                         /* sch meerealisatie fc1 met fc2                                    */
    count ti12,                         /* t intergroen- of ontruimingstijd fc1 -> fc2                      */
    count vs21);                        /* t voorstarttijd   fc2 -> fc1                                     */
  
  mulv dcf_sync(                        /* Fik110901                                                        */
    count fc1,                          /* fasecyclus 1                                                     */
    count fc2);                         /* fasecyclus 2                                                     */
  
  void dcf_vs_confl(void);              /* Fik110901                                                        */
  void dcf_vs_corr_ttr(void);           /* Fik110901                                                        */
  void dcf_vs_corr_alt(void);           /* Fik110901                                                        */
  void dcf_vs_corr_ttp(void);           /* Fik110901                                                        */
  void dcf_vs_rea(void);                /* Fik110901                                                        */
  void dcf_vs_bar(void);                /* Fik110901                                                        */
  void dcf_vs_afw_sgd(void);            /* Fik110901                                                        */
  
/* ======================================================================================================== */
/* PROCEDURES TBV DEELCONFLICTEN SYNCHROON START (= VOORSTART) TIJDENS HALFSTAR BEDRIJF                     */
/* ======================================================================================================== */
  void dcf_vs_vas_fca(void);            /* Fik110901                                                        */
  void dcf_vs_vas_prm(void);            /* Fik110901                                                        */
  void dcf_vs_vas_bar(void);            /* Fik110901                                                        */
  void dcf_vs_afw_vas(void);            /* Fik110901                                                        */
  
