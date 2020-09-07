/* ======================================================================================================== */
/* PROCEDURES TBV C-ITS FUNCTIONALITEIT                                                                     */
/* ======================================================================================================== */

/* Ontwerp	: Marcel Fick en Patrick Huijskes                                                           */
/* Versie       : 2.00                                                                                      */
/* Datum        : 1 september 2018                                                                          */

/* -------------------------------------------------------------------------------------------------------- */
/* Functie controleer of ObjectID aanwezig in gebied                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  int vtg_in_gebied(
    rif_string ObjectID,
    mulv       aantal);

/* -------------------------------------------------------------------------------------------------------- */
/* Procedure aanvraag vanuit het RIS                                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void RIS_aanvraag(                    /* Fik170911                                                        */
    count fc,                           /* fasecyclus                                                       */
    mulv afstand,                       /* afstand tot stopstreep waarna aanvraag vanuit RIS                */
    mulv voertuig);                     /* voertuig type                                                    */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure verlengen IVER methodiek vanuit het RIS                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void RIS_verlengen(                   /* Fik170927                                                        */
    count fc,                           /* fasecyclus                                                       */
    count tvag1,                        /* 1e VAG tijd                                                      */
    count vag1o,                        /* ondergrens VAG1                                                  */
    count vag1b,                        /* bovengrens VAG1                                                  */
    count vag2o,                        /* ondergrens VAG2                                                  */
    count vag2b,                        /* bovengrens VAG2                                                  */
    count vag3o,                        /* ondergrens VAG3                                                  */
    count vag3b,                        /* bovengrens VAG3                                                  */
    count vag4o,                        /* ondergrens VAG4                                                  */
    count vag4b,                        /* bovengrens VAG4                                                  */
    mulv voertuig);                     /* voertuig type                                                    */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal RIS peloton                                                                             */
/* -------------------------------------------------------------------------------------------------------- */
  void RIS_peloton(                     /* Fik170915                                                        */
    count fc,                           /* fasecyclus                                                       */
    count meetpunt,                     /* MM meetpunt    - aantal getelde voertuigen                       */
    count ris_peloton,                  /* MM ris_peloton - startpuls                                       */
    count afstand,                      /* PRM afstand tot stopstreep                                       */
    count voertuig_crit,                /* PRM voertuig criterium                                           */
    count meetperiode,                  /* T meetperiode                                                    */
    count grenshiaat,                   /* T grenshiaat                                                     */
    mulv voertuig_type);                /* voertuigtype                                                     */

/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS peloton ingreep                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_peloton(                /* Fik170915                                                        */
    count fc,                           /* fasecyclus                                                       */
    count ris_peloton,                  /* MM koppelsignaal (= puls signaal)                                */
    count inmv,                         /* T inmeldvertraging                                               */
    count afkv,                         /* T afkapvertraging                                                */
    count afst,                         /* T afstand rijtijd                                                */
    count nmax,                         /* T maximumgroen na ingreep                                        */
    count tbl,                          /* T blokkeer herhaalde prioriteit                                  */
    count prio,                         /* PRM prioriteitscode                                              */
    count awt1,                         /* aanwezigheidsduur 1e pel                                         */
    count awt2,                         /* aanwezigheidsduur 2e pel                                         */
    count awt3,                         /* aanwezigheidsduur 3e pel                                         */
    count prmafstand,
    count prmvag3o,
    count snelheid,
    mulv voertuig_type);

/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS tovergroen ingreep                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_tover(                  /* Fik171001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count bewaak,                       /* T bewakingstijd                                                  */
    count afstand,                      /* PRM afstand tot stopstreep                                       */
    count min_v,                        /* PRM minimum snelheid                                             */
    count max_v);                       /* PRM maximum snelheid                                             */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS bus ingreep                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_bus_ingreep(            /* Fik171106                                                        */
    count fc,                           /* fasecyclus                                                       */
    count bewaak,                       /* T bewakingstijd                                                  */
    count tblokkeer,                    /* T blokkeringstijd                                                */
    count afstand,                      /* PRM afstand tot stopstreep                                       */
    count lyn1,                         /* PRM buslijn 1                                                    */
    count lynx);                        /* PRM buslijn x                                                    */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS hulpdienst ingreep                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_hulpdienst(             /* Fik171006                                                        */
    count fc,                           /* fasecyclus                                                       */
    count bewaak,                       /* T bewakingstijd                                                  */
    count afstand);                     /* PRM afstand tot stopstreep                                       */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS fiets prioriteit                                                                           */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_fts_prio(               /* Fik170922                                                        */
    count fc,                           /* fasecyclus                                                       */
    count min_fts,                      /* PRM minimum aantal fietsers                                      */
    count afstand,                      /* PRM afstand tot de stopstreep                                    */
    count ogwt,                         /* T min.wachttijd                                                  */
    count tbl,                          /* T blokkeringstijd                                                */
    count prio);                        /* PRM prio code                                                    */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure display RIS prioriteit - auto                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_us_ris_mvt(                 /* Fik171001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count awt1,                         /* MM aanwezigheidstijd ris peloton                                 */
    count usrispel,                     /* US ris peloton ingreep                                           */
    count usristov);                    /* US ris tovergroen ingreep                                        */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure display RIS prioriteit - fiets                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_us_ris_fts(                 /* Fik171001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count usrisfts);                    /* US ris fiets prioriteit                                          */
