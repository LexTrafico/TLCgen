/* DECLARATIE SPAT FUNCTIES */
/* ======================== */
/*
 *  Huijskes VRT B.V.
 *  14 Oktoberplein 6
 *  Overloon
 *
 *
 *  Module: functies t.b.v. bijhouden en bepalen van betrouwbaarheid van uitgestuurde SPAT data 
 *
 *  Filenaam        : hvrt_fctiming.c
 *  Ontwerp         : Marc Scherjon
 *  Datum           : 17-03-2018
 *  CCOL            : versie 9.0 en hoger,  Generator PNH versie ?? en hoger
 *
  Revisie:
    19-03-2018 M. Scherjon:  opzet nieuwe versie
    29-03-2018 M. Scherjon:  toevoeging maximum einde
    30-03-2018 M. Scherjon:  proc_spat_register_event() argumenten aangepast
    11-04-2018 M. Scherjon:  test op FC_type toegevoegd -> GEEN SPAT data versturen indien GEEN (geldig) FC_type 

*/
 
/* uitschakelen betrouwbaarheid volgens MS */
/* #define NO_SPAT_BT */

/* include files                                                                                          */
/* =============                                                                                          */
  #include "sysdef.c"
  #include "cif.inc"                    /* declaratie C-interface                                           */
  #include "fcvar.h"                    /* declaratie fasecycli variabelen                                  */

/* macrodefinities */
/* =============== */

#if defined HANDSHAKING | defined TESTOMGEVING
  #define SPAT_HANDSHAKING_CCOL               /* @@ 23-02-2018 : Gebruik dezelfde HANDSHAKING als Ccol: Wacht na start groen op WUS[] == GUS[] */
  /* Hier moet ook MinEndTime van Groen op wachten !  Dus geldt dan de #define SPAT_HANDSHAKING_MINENDTIME alleen voor GROEN */
#endif

/*  #define NO_SPAT_TIME_TOT_START_R  */          /* @@ 23-04-2018 : Geen positieve waarde op START_TIME ROOD sturen */
/*  #define NO_SPAT_TIME_TOT_START_GL */          /* @@ 23-04-2018 : Geen positieve waarde op START_TIME GEEL sturen */
/*  #define NO_SPAT_TIME_TOT_START_GN */          /* @@ 23-04-2018 : Geen positieve waarde op START_TIME GROEN sturen */
  #define NO_SPAT_TIME_TOT_START_ALL          /* @@ 23-04-2018 : Geen positieve waarde op START_TIME sturen */

/*  #define SPAT_HANDSHAKING_MINENDTIME */    /* @@ 23-01-2018 : NIET uitsturen MinEndTime indien GUS[] != WUS[] */
  #define SPAT_CHANGE_TIME2NG                 /* @@ 26-01-2018 : Stuur een Event indien tijd naar onbekend. */
  #define SPAT_CHANGE_TIME2MIN                /* @@ 26-01-2018 : Stuur een Event indien tijd eind waarde bereikt. */
  #define SPAT_MAX_TO_T2G_CORR      4         /* @@ 22-01-2018:  aantal /te extra op lopende ontruimingstijd t.g.v. gus_wus afwijking */
  #define SPAT_T2SG_CORR_R          3         /* @@ 24-01-2018:  extra /te op voorspelling t.g.v. gus_wus afwijking */
  #define SPAT_T2SR_CORR_GL         2         /* @@ 24-01-2018:  extra /te indien geel t.g.v. gus_wus afwijking */

  #define SPAT_TIJD_MAX            32767      /* maximale waarde CIF_FC_TIMING[][] (type Time en type TimeRemaining) */
  #define SPAT_TIJD_MIN           -32767      /* minimale waarde CIF_FC_TIMING[][2] (type Time) */
  #define SPAT_TIJD_ONBEKEND      -32768      /* onbekende waarde CIF_FC_TIMING[][2] (type Time) */
  #define SPAT_REMAIN_ONBEKEND      NG        /* -1 : onbekende waarde CIF_FC_TIMING[][] (type TimeRemaining) */

  #define SPAT_CHANGE_DIFF_TIME     20        /* /te afwijking in voorspelling ( en maximum ) voor EVENT */
  #define SPAT_CHANGE_DIFF_BT       10        /* pecentage afwijking in betrouwbaarheid voor EVENT */

  /* BIT1 t/m BIT6 komen overeen met het V-log 3 'Optie masker' : Blz 36 Document V-log 3 protocol en definities versie 2.5.2 */
  #define SPAT_CHANGE_STATE_START     0x0100                                              /* (  256 ) Status Start vlag */
  #define SPAT_CHANGE_STATE_END       0x0200                                              /* (  512 ) Status End vlag */
  #define SPAT_CHANGE_STATE           (CIF_TIMING_MASKER_START|SPAT_CHANGE_STATE_START)   /* (    2 + 256 ) Status verandering => tevens start tijd */
  #define SPAT_CHANGE_TIME            CIF_TIMING_MASKER_START                             /* (    2 ) => Start_tijd naar waarde of naar onbekend (plafond bereikt) */
  #define SPAT_CHANGE_MINTIME_START   CIF_TIMING_MASKER_MINIMUM                           /* (    4 ) minimum tijd van NG naar getal */
  #define SPAT_CHANGE_MINTIME_NG      CIF_TIMING_MASKER_MINIMUM                           /* (    4 ) minimum tijd van getal naar NG */
  #define SPAT_CHANGE_MAX_START       CIF_TIMING_MASKER_MAXIMUM                           /* (    8 ) maximum tijd van NG naar getal */
  #define SPAT_CHANGE_MAX_DIFF        CIF_TIMING_MASKER_MAXIMUM                           /* (    8 ) maximum tijd afwijking tussen eerder event en nieuwe berekening */
  #define SPAT_CHANGE_MAX_NG          CIF_TIMING_MASKER_MAXIMUM                           /* (    8 ) maximum tijd van getal naar NG */
  #define SPAT_CHANGE_LIKELY_START    CIF_TIMING_MASKER_VOORSPELD                         /* (   16 ) voorspelling van NG naar getal */
  #define SPAT_CHANGE_LIKELY_DIFF     CIF_TIMING_MASKER_VOORSPELD                         /* (   16 ) voorspelling afwijking tussen eerder event en nieuwe berekening */
  #define SPAT_CHANGE_LIKELY_NG       CIF_TIMING_MASKER_VOORSPELD                         /* (   16 ) voorspelling van getal naar NG */
  #define SPAT_CHANGE_BT_START        CIF_TIMING_MASKER_BETROUWBAARHEID                   /* (   32 ) betrouwbaarheid van NG naar getal */
  #define SPAT_CHANGE_BT_DIFF         CIF_TIMING_MASKER_BETROUWBAARHEID                   /* (   32 ) betrouwbaarheid afwijking tussen eerder event en nieuwe berekening */
  #define SPAT_CHANGE_BT_NG           CIF_TIMING_MASKER_BETROUWBAARHEID                   /* (   32 ) betrouwbaarheid van getal naar NG */
  #define SPAT_CHANGE_BT_TEST         0x1000                                              /* ( 4096 ) betrouwbaarheid updaten ? of indien NG, dan -> elke 5 sec registratie van voorspelling, i.v.m. versneld 'vullen' van data */

  #define SPAT_CHANGE_MAX             CIF_TIMING_MASKER_MAXIMUM
  #define SPAT_CHANGE_LIKELY          CIF_TIMING_MASKER_VOORSPELD
  #define SPAT_CHANGE_BT              CIF_TIMING_MASKER_BETROUWBAARHEID

  #define SPAT_EVENT_BT              SPAT_CHANGE_BT | SPAT_CHANGE_LIKELY | SPAT_CHANGE_BT_TEST

/* Volledig bericht met alle velden actief : */
  #define TIMING_MASK_ALL         (CIF_TIMING_MASK_EVENTSTATE | CIF_TIMING_MASK_STARTTIME | \
                                  CIF_TIMING_MASK_MINENDTIME | CIF_TIMING_MASK_MAXENDTIME | \
                                  CIF_TIMING_MASK_LIKELYTIME | CIF_TIMING_MASK_CONFIDENCE | \
                                  CIF_TIMING_MASK_NEXTTIME)

/* Verplichte 'velden'   @@ 20-12-2017 */
/*  #define TIMING_MASK_MUST        (CIF_TIMING_MASK_EVENTSTATE | CIF_TIMING_MASK_MINENDTIME)  vervallen: CIF_TIMING_MASK_MINENDTIME is alleen nodig indien er een 'timing' aanwezig is. Dat is altijd voor de actieve status, maar mogelijk NIET altijd voor de 'volgende' */
  #define TIMING_MASK_TIMING      (CIF_TIMING_MASK_STARTTIME | CIF_TIMING_MASK_MINENDTIME | \
                                  CIF_TIMING_MASK_MAXENDTIME | CIF_TIMING_MASK_LIKELYTIME | \
                                  CIF_TIMING_MASK_CONFIDENCE | CIF_TIMING_MASK_NEXTTIME)
  #define TIMING_MASK_GEWEEST     0x8000

  #define SPAT_ALL_US_type        (MVT_type | FTS_type | VTG_type | OV_type)

/* Declaratie globale variabele ( alleen voor gebruik binnen deze fucntie !!                            */
/* ============================                                                                         */
struct _SPAT_buf {

  /* variabelen vanuit de regelstruktuur moeten worde ngeschreven t.b.v. berekenden en versturen van FC_TIMING */
/*  mulv status;                      */
/*  mulv tgr_tijd;                    */  /* restand garantie rood tijd */
/*  mulv tgg_tijd;                    */  /* restand garantie groen tijd */
/*  mulv tgl_tijd;                    */  /* restand geel tijd */
/*  mulv min_tijd_tot_realisatie;     */  /* restand (deelconflict-)ontruimingstijd ( ook eventuele voorstart-, gelijkstart ) */
/*  mulv tijd_tot_einde_rood;         */  /* voorspelling vanuit regelstruktuur: 'wachttijdvoorspeller tijd' */
/*  mulv tijd_tot_einde_groen;        */  /* voorspelling einde groen */
/*  mulv max_groen_tijd;              */  /* maximum groentijd, indien deze 'zeker' is. */
/*  mulv realisatie;                  */  /* realisatie vorm  PR, AR, BR , ... */
  
  /* variabelen die door de FC_timing worden gevuld met de actuele gegevens die ook op de interface staan  */
  mulv tijd_tot_start_rood;             /* tijd tot start rood                                             */
  mulv tijd_tot_einde_rood_minimum;     /* tijd tot einde rood - minimum                                   */
  mulv tijd_tot_einde_rood_maximum;     /* tijd tot einde rood - maximum                                   */
  mulv tijd_tot_einde_rood_schatting;   /* tijd tot einde rood - schatting                                 */
  mulv tijd_tot_einde_rood_kans;        /* kans dat schatting juist is                                     */
  mulv tijd_tot_start_rood_volgende;    /* tijd tot start rood in de volgende cyclus                       */

  mulv old_tijd_tot_einde_rood_minimum;     /* tijd tot einde rood - minimum                                   */
  mulv old_tijd_tot_einde_rood_maximum;     /* tijd tot einde rood - maximum                                   */
  mulv old_tijd_tot_einde_rood_schatting;   /* tijd tot einde rood - schatting                                 */
/*  mulv old_tijd_tot_einde_rood_kans;      */  /* kans dat schatting juist is                                     */
  mulv old_tijd_tot_start_rood_volgende;    /* tijd tot start rood in de volgende cyclus                       */

  mulv tijd_tot_start_groen;            /* tijd tot start groen                                            */
  mulv tijd_tot_einde_groen_minimum;    /* tijd tot einde groen - minimum                                  */
  mulv tijd_tot_einde_groen_maximum;    /* tijd tot einde groen - maximum                                  */
  mulv tijd_tot_einde_groen_schatting;  /* tijd tot einde groen - schatting                                */
  mulv tijd_tot_einde_groen_kans;       /* kans dat schatting juist is                                     */
  mulv tijd_tot_start_groen_volgende;   /* tijd tot start groen in de volgende cyclus                      */

  mulv old_tijd_tot_einde_groen_minimum;    /* tijd tot einde groen - minimum                                  */
  mulv old_tijd_tot_einde_groen_maximum;    /* tijd tot einde groen - maximum                                  */
  mulv old_tijd_tot_einde_groen_schatting;  /* tijd tot einde groen - schatting                                */
/*  mulv old_tijd_tot_einde_groen_kans;     */  /* kans dat schatting juist is                                     */
  mulv old_tijd_tot_start_groen_volgende;   /* tijd tot start groen in de volgende cyclus                      */

  mulv tijd_tot_start_geel;             /* tijd tot start geel                                             */
  mulv tijd_tot_einde_geel_minimum;     /* tijd tot einde geel - minimum                                   */
  mulv tijd_tot_einde_geel_maximum;     /* tijd tot einde geel - maximum                                   */
  mulv tijd_tot_einde_geel_schatting;   /* tijd tot einde geel - schatting                                 */
  mulv tijd_tot_einde_geel_kans;        /* kans dat schatting juist is                                     */
  mulv tijd_tot_start_geel_volgende;    /* tijd tot start geel in de volgende cyclus                       */

  mulv old_tijd_tot_einde_geel_minimum;     /* tijd tot einde geel - minimum                                   */
  mulv old_tijd_tot_einde_geel_maximum;     /* tijd tot einde geel - maximum                                   */
  mulv old_tijd_tot_einde_geel_schatting;   /* tijd tot einde geel - schatting                                 */
/*  mulv old_tijd_tot_einde_geel_kans;      */  /* kans dat schatting juist is                                     */
  mulv old_tijd_tot_start_geel_volgende;    /* tijd tot start geel in de volgende cyclus                       */

/* Bepaling wijzigings vlaggen en masker voor de SPAT data */
  mulv event_vlag;               /* buffer event wijzigings vlag SPAT bericht                       */
  mulv mask_rood;                /* event masker SPAT bericht tbv rood                              */
  mulv mask_geel;                /* event masker SPAT bericht tbv geel                              */
  mulv mask_groen;               /* event masker SPAT bericht tbv groen                             */

  mulv tijd_event_vlag;          /* tijd vorig event wijzigings vlag SPAT bericht                       */

};

  struct _SPAT_buf   SPAT_buf[FCMAX];



/* aanroep betrouwbaarheids functie */
#ifndef NO_SPAT_BT
  #include "bt_t2r_t2g.h"

 #ifdef BT_SOURCE_C
  #include "bt_t2r_t2g.c"
 #endif
#endif

  /* Declaratie algemene functies                                                                            */
  /* ============================                                                                            */


  /* Declaratie interne functies                                                                             */
  /* ============================                                                                            */
  s_int16 set_fctiming(count fc, count eventnr, s_int16 wijz, s_int16 mask, s_int16 eventState, s_int16 startTime,
                        s_int16 minEndTime, s_int16 maxEndTime, s_int16 likelyTime, s_int16 confidence, s_int16 nextTime);
  s_int16 reset_fctiming(count fc, count eventnr);
  bool kopieer_fc_timing2appl(count fc);
  void init_fc_timing(void);
  void bepaal_fc_timing_init(void);
  void bepaal_fc_timing(count fc);


/* DEFINITIE FCTIMING FUNCTIES                                                                            */
/* ===========================                                                                            */


/* functie voor het wegschrijven van CIF_FC_TIMING[] event                                                */
/* -------------------------------------------------------                                                */

s_int16 set_fctiming(count fc, count eventnr, s_int16 wijz, s_int16 mask, s_int16 eventState, s_int16 startTime,
                    s_int16 minEndTime, s_int16 maxEndTime, s_int16 likelyTime, s_int16 confidence, s_int16 nextTime)
{
  if ((fc < FC_MAX) && (eventnr < CIF_MAX_EVENT)) {

    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_MASK] = mask;

    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_EVENTSTATE] = eventState;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_STARTTIME] = startTime;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_MINENDTIME] = minEndTime;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_MAXENDTIME] = maxEndTime;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_LIKELYTIME] = likelyTime;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_CONFIDENCE] = confidence;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_NEXTTIME] = nextTime;

    /* @@ 26-01-2018 MS */
    if (wijz > 0) CIF_FC_TIMING_WIJZ[fc] |= TRUE;      /* zet wijzigvlag                                  */

    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

/* functie voor het resetten van CIF_FC_TIMING[] event                                                      */
/* ---------------------------------------------------                                                      */

s_int16 reset_fctiming(count fc, count eventnr)
{
  if ((fc < FC_MAX) && (eventnr < CIF_MAX_EVENT)) {

    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_MASK] = 0;

    /* @@ MS 22-12-2017  op NG / ONBEKEND */
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_EVENTSTATE] = NG;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_STARTTIME] = SPAT_TIJD_ONBEKEND;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_MINENDTIME] = SPAT_REMAIN_ONBEKEND;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_MAXENDTIME] = SPAT_REMAIN_ONBEKEND;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_LIKELYTIME] = SPAT_REMAIN_ONBEKEND;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_CONFIDENCE] = NG;
    CIF_FC_TIMING[fc][eventnr][CIF_TIMING_NEXTTIME] = SPAT_REMAIN_ONBEKEND;

    /*  CIF_FC_TIMING_WIJZ[fc]= TRUE; ->  NIET bij een reset */

    return (TRUE);
  }
  else {
    return (FALSE);
  }
}


/* functie afhandeling FC_TIMING[] events                                                                   */
/* --------------------------                                                                               */

void fc_timing_all(void)
{
  register count i, _vlag_;
  register mulv mask_timing = FALSE;
  static s_int16 WPS_old = CIF_STAT_ONGEDEF;    /* oude programmastatus                                      */

  bepaal_fc_timing_init();

  for ( i = 0; i < FC_MAX; i++ ) {
    _vlag_ = 0;

    if ( US_type[i] & SPAT_ALL_US_type ) {

      switch ( CIF_WPS[CIF_PROG_STATUS] ) {

      case CIF_STAT_GEDOOFD:                  /* Gedoofd                                                    */
        if ( CIF_WPS[CIF_PROG_STATUS] != WPS_old ) {
          set_fctiming((count) i,            /* fc                                                         */
              (count) 0,
              (s_int16) TRUE,                 /* wijzigingsvlag                                             */
              (s_int16) CIF_TIMING_MASK_EVENTSTATE, /* mask -> alleen status !!                             */
              (s_int16) CIF_TIMING_GEDOOFD,   /* eventState                                                 */
              (s_int16) SPAT_TIJD_ONBEKEND,   /* startTime                                                  */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* minEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* maxEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* likelyTime                                                 */
              (s_int16) NG,                   /* confidence                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND); /* nextTime                                                  */
          _vlag_ ++;
          while ( _vlag_ < 4 ) { /* @@ MS:  altijd minimaal ��n reset_fc_timing() toevoegen, en/of 'de rest' t/m 4 wissen */
            reset_fctiming((count) i, (count) _vlag_);
            _vlag_++;
          }
        }
        break;

      case CIF_STAT_KP:                 /* Geel knipperen                                                   */
        if ( CIF_WPS[CIF_PROG_STATUS] != WPS_old ) {
          set_fctiming((count) i,      /* fc                                                               */
              (count) 0,
              (s_int16) TRUE,                 /* wijzigingsvlag                                             */
              (s_int16) CIF_TIMING_MASK_EVENTSTATE, /* mask -> alleen status !!                             */
              (s_int16) CIF_TIMING_GEEL_KNIPPEREN, /* eventState                                            */
              (s_int16) SPAT_TIJD_ONBEKEND,   /* startTime                                                  */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* minEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* maxEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* likelyTime                                                 */
              (s_int16) NG,                   /* confidence                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND); /* nextTime                                                  */
          _vlag_ ++;
          while ( _vlag_ < 4 ) { /* @@ MS:  altijd minimaal ��n reset_fc_timing() toevoegen, en/of 'de rest' t/m 4 wissen */
            reset_fctiming((count) i, (count) _vlag_);
            _vlag_++;
          }
        }
        break;

      case CIF_STAT_GEEL:                 /* Vast Geel inschakelen   @@ 28-01-2018                            */
        if ( CIF_WPS[CIF_PROG_STATUS] != WPS_old ) {
          if ( US_type[i] == VTG_type ) {
            set_fctiming((count) i,      /* fc                                                               */
                (count) 0,
                (s_int16) TRUE,                 /* wijzigingsvlag                                             */
                (s_int16) CIF_TIMING_MASK_EVENTSTATE, /* mask -> alleen status !!                             */
                (s_int16) CIF_TIMING_ROOD,      /* eventState                                                 */
                (s_int16) SPAT_TIJD_ONBEKEND,   /* startTime                                                  */
                (s_int16) SPAT_REMAIN_ONBEKEND, /* minEndTime                                                 */
                (s_int16) SPAT_REMAIN_ONBEKEND, /* maxEndTime                                                 */
                (s_int16) SPAT_REMAIN_ONBEKEND, /* likelyTime                                                 */
                (s_int16) NG,                   /* confidence                                                 */
                (s_int16) SPAT_REMAIN_ONBEKEND); /* nextTime                                                  */
          }
          else {
            set_fctiming((count) i,      /* fc                                                               */
                (count) 0,
                (s_int16) TRUE,                 /* wijzigingsvlag                                             */
                (s_int16) CIF_TIMING_MASK_EVENTSTATE, /* mask -> alleen status !!                             */
                (s_int16) CIF_TIMING_GEEL,      /* eventState                                                 */
                (s_int16) SPAT_TIJD_ONBEKEND,   /* startTime                                                  */
                (s_int16) SPAT_REMAIN_ONBEKEND, /* minEndTime                                                 */
                (s_int16) SPAT_REMAIN_ONBEKEND, /* maxEndTime                                                 */
                (s_int16) SPAT_REMAIN_ONBEKEND, /* likelyTime                                                 */
                (s_int16) NG,                   /* confidence                                                 */
                (s_int16) SPAT_REMAIN_ONBEKEND); /* nextTime                                                  */
          }
          _vlag_ ++;
          while ( _vlag_ < 4 ) { /* @@ MS:  altijd minimaal ��n reset_fc_timing() toevoegen, en/of 'de rest' t/m 4 wissen */
            reset_fctiming((count) i, (count) _vlag_);
            _vlag_++;
          }
        }
        break;

      case CIF_STAT_AR:                 /* Alles rood                                                       */
        if ( CIF_WPS[CIF_PROG_STATUS] != WPS_old ) {
          set_fctiming((count) i,      /* fc                                                               */
              (count) 0,
              (s_int16) TRUE,                 /* wijzigingsvlag                                             */
              (s_int16) CIF_TIMING_MASK_EVENTSTATE, /* mask -> alleen status !!                             */
              (s_int16) CIF_TIMING_ROOD,      /* eventState                                                 */
              (s_int16) SPAT_TIJD_ONBEKEND,   /* startTime                                                  */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* minEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* maxEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* likelyTime                                                 */
              (s_int16) NG,                   /* confidence                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND); /* nextTime                                                  */
          _vlag_ ++;
          while ( _vlag_ < 4 ) { /* @@ MS:  altijd minimaal ��n reset_fc_timing() toevoegen, en/of 'de rest' t/m 4 wissen */
            reset_fctiming((count) i, (count) _vlag_);
            _vlag_++;
          }
        }
        break;

      case CIF_STAT_REG:                /* Regelen                                                          */
        bepaal_fc_timing((count) i);

        _vlag_ = 0;

        if ( R[i] && (SPAT_buf[i].mask_rood & TIMING_MASK_ALL) && !(SPAT_buf[i].mask_rood & TIMING_MASK_GEWEEST) ) { /* R[] toegevoegd: volgorde op basis van status */
          mask_timing = (SPAT_buf[i].mask_rood & TIMING_MASK_TIMING) ? CIF_TIMING_MASK_MINENDTIME : FALSE;
          set_fctiming((count) i,    /* fc                                                               */
              (count) _vlag_,
              (s_int16) (SPAT_buf[i].event_vlag & (1 << CIF_TIMING_ROOD)),  /* Wijzigingsvlag                           */
              (s_int16) (CIF_TIMING_MASK_EVENTSTATE | mask_timing | (SPAT_buf[i].mask_rood & TIMING_MASK_ALL)), /* mask */
              (s_int16) CIF_TIMING_ROOD, /* eventState                                                    */
              (s_int16) SPAT_buf[i].tijd_tot_start_rood, /* startTime                                    */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_minimum, /* minEndTime                           */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_maximum, /* maxEndTime                           */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_schatting, /* likelyTime                         */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_kans, /* confidence                              */
              (s_int16) SPAT_buf[i].tijd_tot_start_rood_volgende); /* nextTime                           */
          _vlag_++;
          SPAT_buf[i].mask_rood |= TIMING_MASK_GEWEEST; /* is geweest (t.b.v. volgorde op basis van status ) */
        }
        if ( !GL[i] && (SPAT_buf[i].mask_groen & TIMING_MASK_ALL) && !(SPAT_buf[i].mask_groen & TIMING_MASK_GEWEEST) ) { /* !GL[] toegevoegd: volgorde op basis van status */
          mask_timing = (SPAT_buf[i].mask_groen & TIMING_MASK_TIMING) ? CIF_TIMING_MASK_MINENDTIME : FALSE;
          set_fctiming((count) i,   /* fc                                                               */
              (count) _vlag_,
              (s_int16) (SPAT_buf[i].event_vlag & (1 << CIF_TIMING_GROEN)),  /* Wijzigingsvlag                          */
              (s_int16) (CIF_TIMING_MASK_EVENTSTATE | mask_timing | (SPAT_buf[i].mask_groen & TIMING_MASK_ALL)), /* mask */
              (s_int16) CIF_TIMING_GROEN, /* eventState                                                   */
              (s_int16) SPAT_buf[i].tijd_tot_start_groen, /* startTime                                   */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_minimum, /* minEndTime                          */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_maximum, /* maxEndTime                          */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_schatting, /* likelyTime                        */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_kans, /* confidence                             */
              (s_int16) SPAT_buf[i].tijd_tot_start_groen_volgende); /* nextTime                          */
          _vlag_++;
          SPAT_buf[i].mask_groen |= TIMING_MASK_GEWEEST; /* is geweest (t.b.v. volgorde op basis van status ) */
        }
        if ( (SPAT_buf[i].mask_geel & TIMING_MASK_ALL) && !(SPAT_buf[i].mask_geel & TIMING_MASK_GEWEEST) ) {
          mask_timing = (SPAT_buf[i].mask_geel & TIMING_MASK_TIMING) ? CIF_TIMING_MASK_MINENDTIME : FALSE;
          set_fctiming((count) i,   /* fc                                                               */
              (count) _vlag_,
              (s_int16) (SPAT_buf[i].event_vlag & (1 << CIF_TIMING_GEEL)),  /* Wijzigingsvlag                          */
              (s_int16) (CIF_TIMING_MASK_EVENTSTATE | mask_timing | (SPAT_buf[i].mask_geel & TIMING_MASK_ALL)), /* mask */
              (s_int16) CIF_TIMING_GEEL, /* eventState                                                    */
              (s_int16) SPAT_buf[i].tijd_tot_start_geel, /* startTime                                    */
              (s_int16) SPAT_buf[i].tijd_tot_einde_geel_minimum, /* minEndTime                           */
              (s_int16) SPAT_buf[i].tijd_tot_einde_geel_maximum, /* maxEndTime                           */
              (s_int16) SPAT_buf[i].tijd_tot_einde_geel_schatting, /* likelyTime                         */
              (s_int16) SPAT_buf[i].tijd_tot_einde_geel_kans, /* confidence                              */
              (s_int16) SPAT_buf[i].tijd_tot_start_geel_volgende); /* nextTime                           */
          _vlag_++;
        }
        if ( (SPAT_buf[i].mask_rood & TIMING_MASK_ALL) && !(SPAT_buf[i].mask_rood & TIMING_MASK_GEWEEST) ) {
          mask_timing = (SPAT_buf[i].mask_rood & TIMING_MASK_TIMING) ? CIF_TIMING_MASK_MINENDTIME : FALSE;
          set_fctiming((count) i,   /* fc                                                               */
              (count) _vlag_,
              (s_int16) (SPAT_buf[i].event_vlag & (1 << CIF_TIMING_ROOD)),  /* Wijzigingsvlag                          */
              (s_int16) (CIF_TIMING_MASK_EVENTSTATE | mask_timing | (SPAT_buf[i].mask_rood & TIMING_MASK_ALL)), /* mask */
              (s_int16) CIF_TIMING_ROOD, /* eventState                                                    */
              (s_int16) SPAT_buf[i].tijd_tot_start_rood, /* startTime                                    */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_minimum, /* minEndTime                           */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_maximum, /* maxEndTime                           */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_schatting, /* likelyTime                         */
              (s_int16) SPAT_buf[i].tijd_tot_einde_rood_kans, /* confidence                              */
              (s_int16) SPAT_buf[i].tijd_tot_start_rood_volgende); /* nextTime                           */
          _vlag_++;
        }
        if ( (SPAT_buf[i].mask_groen & TIMING_MASK_ALL) && !(SPAT_buf[i].mask_groen & TIMING_MASK_GEWEEST) ) {
          mask_timing = (SPAT_buf[i].mask_groen & TIMING_MASK_TIMING) ? CIF_TIMING_MASK_MINENDTIME : FALSE;
          set_fctiming((count) i,   /* fc                                                               */
              (count) _vlag_,
              (s_int16) (SPAT_buf[i].event_vlag & (1 << CIF_TIMING_GROEN)),  /* Wijzigingsvlag                         */
              (s_int16) (CIF_TIMING_MASK_EVENTSTATE | mask_timing | (SPAT_buf[i].mask_groen & TIMING_MASK_ALL)), /* mask */
              (s_int16) CIF_TIMING_GROEN, /* eventState                                                   */
              (s_int16) SPAT_buf[i].tijd_tot_start_groen, /* startTime                                   */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_minimum, /* minEndTime                          */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_maximum, /* maxEndTime                          */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_schatting, /* likelyTime                        */
              (s_int16) SPAT_buf[i].tijd_tot_einde_groen_kans, /* confidence                             */
              (s_int16) SPAT_buf[i].tijd_tot_start_groen_volgende); /* nextTime                          */
          _vlag_++;
        }

        while ( _vlag_ < 4 ) { /* @@ MS:  altijd minimaal ��n reset_fc_timing() toevoegen, en/of 'de rest' t/m 4 wissen */
          reset_fctiming((count) i, (count) _vlag_);
          _vlag_++;
        }
        /*          } */

        kopieer_fc_timing2appl((count) i);

        break;

      default:
        if ( CIF_WPS[CIF_PROG_STATUS] != WPS_old ) {
          set_fctiming((count) i,       /* fc                                                             */
              (count) 0,
              (s_int16) TRUE,                 /* wijzigingsvlag                                             */
              (s_int16) CIF_TIMING_MASK_EVENTSTATE, /* mask                                                 */
              (s_int16) CIF_TIMING_ONBEKEND,  /* eventState                                                 */
              (s_int16) SPAT_TIJD_ONBEKEND,   /* startTime                                                  */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* minEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* maxEndTime                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND, /* likelyTime                                                 */
              (s_int16) NG,                   /* confidence                                                 */
              (s_int16) SPAT_REMAIN_ONBEKEND); /* nextTime                                                  */
          _vlag_ ++;
          while ( _vlag_ < 4 ) { /* @@ MS:  altijd minimaal ��n reset_fc_timing() toevoegen, en/of 'de rest' t/m 4 wissen */
            reset_fctiming((count) i, (count) _vlag_);
            _vlag_++;
          }
        }
        break;
      }
    }
  }
  WPS_old = CIF_WPS[CIF_PROG_STATUS];
}



/* -------------------------------------------------------------------------------------------------------- */
/* Initialiseer SPAT_buf                                                                                   */
/* -------------------------------------------------------------------------------------------------------- */
void init_fc_timing(void)             /* MS 19-03-2018                                                      */
{
  count i,j;

  for (i = 0; i < FC_MAX; i++) {

    SPAT_buf[i].tijd_tot_start_rood = SPAT_TIJD_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_rood_minimum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_rood_maximum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_rood_schatting = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_rood_kans = NG;
    SPAT_buf[i].tijd_tot_start_rood_volgende = SPAT_REMAIN_ONBEKEND;

    SPAT_buf[i].old_tijd_tot_einde_rood_minimum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].old_tijd_tot_einde_rood_maximum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].old_tijd_tot_einde_rood_schatting = SPAT_REMAIN_ONBEKEND;
/*    SPAT_buf[i].old_tijd_tot_einde_rood_kans = NG; */
    SPAT_buf[i].old_tijd_tot_start_rood_volgende = SPAT_REMAIN_ONBEKEND;

    SPAT_buf[i].tijd_tot_start_groen = SPAT_TIJD_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_groen_minimum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_groen_maximum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_groen_schatting = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_groen_kans = NG;
    SPAT_buf[i].tijd_tot_start_groen_volgende = SPAT_REMAIN_ONBEKEND;

    SPAT_buf[i].old_tijd_tot_einde_groen_minimum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].old_tijd_tot_einde_groen_maximum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].old_tijd_tot_einde_groen_schatting = SPAT_REMAIN_ONBEKEND;
/*    SPAT_buf[i].old_tijd_tot_einde_groen_kans = NG; */
    SPAT_buf[i].old_tijd_tot_start_groen_volgende = SPAT_REMAIN_ONBEKEND;

    SPAT_buf[i].tijd_tot_start_geel = SPAT_TIJD_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_geel_minimum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_geel_maximum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_geel_schatting = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].tijd_tot_einde_geel_kans = NG;
    SPAT_buf[i].tijd_tot_start_geel_volgende = SPAT_REMAIN_ONBEKEND;

    SPAT_buf[i].old_tijd_tot_einde_geel_minimum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].old_tijd_tot_einde_geel_maximum = SPAT_REMAIN_ONBEKEND;
    SPAT_buf[i].old_tijd_tot_einde_geel_schatting = SPAT_REMAIN_ONBEKEND;
/*    SPAT_buf[i].old_tijd_tot_einde_geel_kans = NG; */
    SPAT_buf[i].old_tijd_tot_start_geel_volgende = SPAT_REMAIN_ONBEKEND;

    SPAT_buf[i].event_vlag = FALSE;
    SPAT_buf[i].mask_rood = FALSE;
    SPAT_buf[i].mask_geel = FALSE;
    SPAT_buf[i].mask_groen = FALSE;
    SPAT_buf[i].tijd_event_vlag = NG;

    kopieer_fc_timing2appl((count) i);

    for ( j= 0; j < CIF_MAX_EVENT ; j++ ) {
      reset_fctiming((count) i, (count) j);
    }

  }
}

/* -------------------------------------------------------------------------------------------------------- */
/* Init SPAT_buffer                                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
void bepaal_fc_timing_init(void)     /*                                                                     */
{
  static bool wps_old = CIF_STAT_ONGEDEF;
  
  /* niet regelen dan terug naar initialisatie                                                            */
  if ((CIF_WPS[CIF_PROG_STATUS] != CIF_STAT_REG) || SAPPLPROG) { /*  || SAPPLPROG  ( en ook in volgende regel ) hierdoor altijd INIT -> ook in PC */
    if ((wps_old != CIF_WPS[CIF_PROG_STATUS]) || SAPPLPROG) { /* @@ MS   ��n keer doorlopen is voldoende */
      init_fc_timing();
#ifdef BT_SPAT
      init_spat_bt(FC_MAX, FALSE); /* betrouwbaarheids registratie op 0% => NIET REGELEN */
#endif
    }
  }
  else {
    /* REGELEN */
    if (wps_old != CIF_WPS[CIF_PROG_STATUS]) {
#ifdef BT_SPAT
      init_spat_bt(FC_MAX, TRUE); /* init betrouwbaarheids registratie op 'start' REGELEN */
#endif
    }
  }

  wps_old = CIF_WPS[CIF_PROG_STATUS];

}


bool kopieer_fc_timing2appl(count i)
{
  bool result = FALSE;

  CCOL2SPAT[i].tijd_tot_start_rood = SPAT_buf[i].tijd_tot_start_rood;
  CCOL2SPAT[i].tijd_tot_einde_rood_minimum = SPAT_buf[i].tijd_tot_einde_rood_minimum;
  CCOL2SPAT[i].tijd_tot_einde_rood_maximum = SPAT_buf[i].tijd_tot_einde_rood_maximum;
  CCOL2SPAT[i].tijd_tot_einde_rood_schatting = SPAT_buf[i].tijd_tot_einde_rood_schatting;
  CCOL2SPAT[i].tijd_tot_einde_rood_kans = SPAT_buf[i].tijd_tot_einde_rood_kans;
  CCOL2SPAT[i].tijd_tot_start_rood_volgende = SPAT_buf[i].tijd_tot_start_rood_volgende;

  CCOL2SPAT[i].tijd_tot_start_groen = SPAT_buf[i].tijd_tot_start_groen;
  CCOL2SPAT[i].tijd_tot_einde_groen_minimum = SPAT_buf[i].tijd_tot_einde_groen_minimum;
  CCOL2SPAT[i].tijd_tot_einde_groen_maximum = SPAT_buf[i].tijd_tot_einde_groen_maximum;
  CCOL2SPAT[i].tijd_tot_einde_groen_schatting = SPAT_buf[i].tijd_tot_einde_groen_schatting;
  CCOL2SPAT[i].tijd_tot_einde_groen_kans = SPAT_buf[i].tijd_tot_einde_groen_kans;
  CCOL2SPAT[i].tijd_tot_start_groen_volgende = SPAT_buf[i].tijd_tot_start_groen_volgende;

  CCOL2SPAT[i].tijd_tot_start_geel = SPAT_buf[i].tijd_tot_start_geel;
  CCOL2SPAT[i].tijd_tot_einde_geel_minimum = SPAT_buf[i].tijd_tot_einde_geel_minimum;
  CCOL2SPAT[i].tijd_tot_einde_geel_maximum = SPAT_buf[i].tijd_tot_einde_geel_maximum;
  CCOL2SPAT[i].tijd_tot_einde_geel_schatting = SPAT_buf[i].tijd_tot_einde_geel_schatting;
  CCOL2SPAT[i].tijd_tot_einde_geel_kans = SPAT_buf[i].tijd_tot_einde_geel_kans;
  CCOL2SPAT[i].tijd_tot_start_geel_volgende = SPAT_buf[i].tijd_tot_start_geel_volgende;

  CCOL2SPAT[i].event_vlag = SPAT_buf[i].event_vlag;
  CCOL2SPAT[i].mask_rood = SPAT_buf[i].mask_rood&~TIMING_MASK_GEWEEST;
  CCOL2SPAT[i].mask_geel = SPAT_buf[i].mask_geel&~TIMING_MASK_GEWEEST;
  CCOL2SPAT[i].mask_groen = SPAT_buf[i].mask_groen&~TIMING_MASK_GEWEEST;

  return(result);

}


/* -------------------------------------------------------------------------------------------------------- */
/* Bepaal SPAT_buf                                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
void bepaal_fc_timing(count i)     /* MS 19-03-2018                                                         */
{

  bool elke_5_sec;      /* vlag */
  /* @@ 26-01-2018 */
  mulv change_Rood;     /* wijzigingsvlag rood */
  mulv change_Groen;    /* wijzigingsvlag groen */
  mulv change_Geel;     /* wijzigingsvlag geel */
  mulv tmp = FALSE;

#ifndef AUTOMAAT
  if ( TE==5 ) /* high speed */
    elke_5_sec = ((CIF_KLOK[CIF_SEC_TELLER] % 5) == 1) && TS;
  else 
#endif
    elke_5_sec = ((CIF_KLOK[CIF_TSEC_TELLER] % 50) == 1) && TE;


  SPAT_buf[i].event_vlag = FALSE;
  SPAT_buf[i].mask_rood = FALSE;
  SPAT_buf[i].mask_geel = FALSE;
  SPAT_buf[i].mask_groen = FALSE;

  change_Rood = FALSE;
  change_Groen = FALSE;
  change_Geel = FALSE;

  if (SG[i]) { /* correctie op SG, omdat T2SG pas in application() wordt bijgewerkt. */
    CCOL2SPAT[i].tijd_tot_einde_rood = NG;
  }
  if (SGL[i]) { /* correctie op SGL, omdat T2EG pas in application() wordt bijgewerkt. */
    CCOL2SPAT[i].tijd_tot_einde_groen = NG;
  }

  /* @@ 25-01-2018 :   volgorde gewijzigd.
  Start-tijd. nu vooraan, daarna Minimum tijd, en daarna voorspelling (einde tijd) en betrouwbaarheid */

  /* -------------------------------------                                                            */
  /* bepaal tijd tot start rood                                                                       */
  /* ------------------------------------                                                             */
  if (SR[i]) {
    SPAT_buf[i].tijd_tot_start_rood = 0;
    change_Rood |= SPAT_CHANGE_STATE;
  }
  else {
    if (R[i]) {
      if (SPAT_buf[i].tijd_tot_start_rood == SPAT_TIJD_ONBEKEND) {
        /* @@ MS 17-05-2018 :  SR[] gemist ? -> Corrigeer starttijd eventueel op basis van verstreken (Garantie) roodtijd */
        SPAT_buf[i].tijd_tot_start_rood = 0;
        if ( TRG_timer[i] < TRG_max[i] ) SPAT_buf[i].tijd_tot_start_rood = TRG_timer[i] - TRG_max[i];
        change_Rood |= SPAT_CHANGE_TIME;
      }
      /* als rood dan tijd tot start rood bijwerken tot het plafond van -32767 */
      if (SPAT_buf[i].tijd_tot_start_rood - TE > SPAT_TIJD_MIN) {
        SPAT_buf[i].tijd_tot_start_rood -= TE;
      }
      else {
#ifdef SPAT_CHANGE_TIME2MIN
        if (SPAT_buf[i].tijd_tot_start_rood != SPAT_TIJD_MIN)  /* EVENT als plafond is bereikt */
          change_Rood |= SPAT_CHANGE_TIME;
#endif
        SPAT_buf[i].tijd_tot_start_rood = SPAT_TIJD_MIN;
      }
    }
    if (ER[i]) { /* @@ 26-03-2018  Einde rood */
      change_Rood |= SPAT_CHANGE_STATE_END;
    }
    if (G[i]) {
#if !defined (NO_SPAT_TIME_TOT_START_R) && !defined (NO_SPAT_TIME_TOT_START_ALL)
      /* als groen dan tijd tot start rood is gelijk aan T2EG[] + geeltijd */
      /* niet in gebruik
      if (CCOL2SPAT[i].tijd_tot_einde_groen == NG) SPAT_buf[i].tijd_tot_start_rood = SPAT_TIJD_ONBEKEND;
      if (CCOL2SPAT[i].tijd_tot_einde_groen >= 0) SPAT_buf[i].tijd_tot_start_rood = CCOL2SPAT[i].tijd_tot_einde_groen + TGL_max[i];
      */
 #ifdef SPAT_CHANGE_TIME2NG
      if (SPAT_buf[i].tijd_tot_start_rood != SPAT_TIJD_ONBEKEND) /* EVENT als waarde naar onbekend */
        change_Rood |= SPAT_CHANGE_TIME;
 #endif
      SPAT_buf[i].tijd_tot_start_rood = SPAT_TIJD_ONBEKEND;
#else
 #ifdef SPAT_CHANGE_TIME2NG
      if (SPAT_buf[i].tijd_tot_start_rood != SPAT_TIJD_ONBEKEND) /* EVENT als waarde naar onbekend */
        change_Rood |= SPAT_CHANGE_TIME;
 #endif
      SPAT_buf[i].tijd_tot_start_rood = SPAT_TIJD_ONBEKEND;
#endif
    }
    if (GL[i]) {                 /* als geel dan aftellen tot 0 afhankelijk van ingestelde geeltijd  */
#if !defined (NO_SPAT_TIME_TOT_START_R) && !defined (NO_SPAT_TIME_TOT_START_ALL)
      if (SGL[i]) change_Rood |= SPAT_CHANGE_STATE; /* ROOD EVENT op SGL[] */
/*      SPAT_buf[i].tijd_tot_start_rood = TGL_max[i] - TGL_timer[i]; */   /* volgens CCOL */
      SPAT_buf[i].tijd_tot_start_rood = CCOL2SPAT[i].tgl_tijd; /* aangeleverd vanuit appl */
#else
 #ifdef SPAT_CHANGE_TIME2NG
      if (SPAT_buf[i].tijd_tot_start_rood != SPAT_TIJD_ONBEKEND) /* EVENT als waarde naar onbekend */
        change_Rood |= SPAT_CHANGE_TIME;
 #endif
      SPAT_buf[i].tijd_tot_start_rood = SPAT_TIJD_ONBEKEND;
#endif
    }
  }

  if (SPAT_buf[i].tijd_tot_start_rood > SPAT_TIJD_ONBEKEND) SPAT_buf[i].mask_rood |= CIF_TIMING_MASKER_START;

  /* -------------------------------------                                                            */
  /* bepaal tijd tot start groen                                                                      */
  /* -------------------------------------                                                            */
  if (SG[i]) {
    SPAT_buf[i].tijd_tot_start_groen = 0;
    change_Groen |= SPAT_CHANGE_STATE;
  }
  else {
    if (G[i]) {           /* als groen dan tijd tot start groen bijwerken tot het plafond van -32767 */
      if (SPAT_buf[i].tijd_tot_start_groen - TE > SPAT_TIJD_MIN) {
        SPAT_buf[i].tijd_tot_start_groen -= TE;
      }
      else {
#ifdef SPAT_CHANGE_TIME2MIN
        if (SPAT_buf[i].tijd_tot_start_groen != SPAT_TIJD_MIN)  /* EVENT als plafond is bereikt */
          change_Groen |= SPAT_CHANGE_TIME;
#endif
        SPAT_buf[i].tijd_tot_start_groen = SPAT_TIJD_MIN;
      }
    }
    else {
      if (EG[i]) { /* @@ 26-03-2018  Einde groen */
        change_Groen |= SPAT_CHANGE_STATE_END;
      }
#if !defined (NO_SPAT_TIME_TOT_START_GN) && !defined (NO_SPAT_TIME_TOT_START_ALL)
      /* tijd tot start groen WEL versturen indien groen worden zeker is */
      if ( (CCOL2SPAT[i].status & SPAT_STATUS_RA_GRN) && (CCOL2SPAT[i].tijd_tot_einde_rood >= 0) ) {
        if (SPAT_buf[i].tijd_tot_start_groen == SPAT_TIJD_ONBEKEND) /* EVENT op START */
          change_Groen |= SPAT_CHANGE_TIME;
        SPAT_buf[i].tijd_tot_start_groen = CCOL2SPAT[i].tijd_tot_einde_rood + SPAT_T2SG_CORR_R;
      }
      else {
#endif
#ifdef SPAT_CHANGE_TIME2NG
        if (SPAT_buf[i].tijd_tot_start_groen != SPAT_TIJD_ONBEKEND)  /* EVENT op onbekend*/
          change_Groen |= SPAT_CHANGE_TIME;
#endif
        SPAT_buf[i].tijd_tot_start_groen = SPAT_TIJD_ONBEKEND;
#if !defined (NO_SPAT_TIME_TOT_START_GN) && !defined (NO_SPAT_TIME_TOT_START_ALL)
      }
#endif
    }
  }

  if (SPAT_buf[i].tijd_tot_start_groen > SPAT_TIJD_ONBEKEND) SPAT_buf[i].mask_groen |= CIF_TIMING_MASKER_START;


  /* ------------------------------------                                                             */
  /* bepaal tijd tot start geel                                                                       */
  /* ------------------------------------                                                             */
  if (SGL[i]) {
    SPAT_buf[i].tijd_tot_start_geel = 0;
    change_Geel |= SPAT_CHANGE_STATE;
  }
  else {
    if (GL[i]){                 /* als geel dan tijd tot start geel bijwerken tot het plafond van -32767 */
      if (SPAT_buf[i].tijd_tot_start_geel - TE > SPAT_TIJD_MIN) {
        SPAT_buf[i].tijd_tot_start_geel -= TE;
      }
      else {
#ifdef SPAT_CHANGE_TIME2MIN
        if (SPAT_buf[i].tijd_tot_start_geel != SPAT_TIJD_MIN) { /* EVENT als plafond is bereikt */
          change_Geel |= SPAT_CHANGE_TIME;
        }
#endif
        SPAT_buf[i].tijd_tot_start_geel = SPAT_TIJD_MIN;
      }
    }
    else {
      if (EGL[i]) { /* @@ 26-03-2018  Einde geel */
        change_Geel |= SPAT_CHANGE_STATE_END;
      }
#if !defined (NO_SPAT_TIME_TOT_START_GL) && !defined (NO_SPAT_TIME_TOT_START_ALL)
      /* als niet geel dan is tijd_tot_start_geel gelijk aan T2EG[]       */
      /*          niet in gebruik
                  if (CCOL2SPAT[i].tijd_tot_einde_groen == NG) SPAT_buf[i].tijd_tot_start_geel = SPAT_TIJD_ONBEKEND;
                  if (CCOL2SPAT[i].tijd_tot_einde_groen >= 0) SPAT_buf[i].tijd_tot_start_geel = CCOL2SPAT[i].tijd_tot_einde_groen;
                  */
      /*   GEEN EVENT   22-01-2018
      #ifdef SPAT_CHANGE_TIME2NG
      if (SPAT_buf[i].tijd_tot_start_geel != SPAT_TIJD_ONBEKEND )
      change_Geel |= SPAT_CHANGE_TIME;
      #endif
      */
      SPAT_buf[i].tijd_tot_start_geel = SPAT_TIJD_ONBEKEND;
#else
      SPAT_buf[i].tijd_tot_start_geel = SPAT_TIJD_ONBEKEND;
#endif
    }
  }

  if (SPAT_buf[i].tijd_tot_start_geel > SPAT_TIJD_ONBEKEND) SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_START;


  /* ---------------------------------- */
  /* bepaal tijd tot einde rood minimum */
  /* ---------------------------------- */
  /* @@ 24-01-2018  verplaatst t.b.v. kontrole T2EG > _minimum */
  tmp = SPAT_buf[i].tijd_tot_einde_rood_minimum;

  SPAT_buf[i].tijd_tot_einde_rood_minimum = SPAT_REMAIN_ONBEKEND;

  if (R[i]
#ifdef SPAT_HANDSHAKING_MINENDTIME
        && (CIF_GUS[i] == CIF_WUS[i]) /* @@ 23-01-2018 wacht op WUS[] */
#endif
        ) {
    if (SPAT_buf[i].tijd_tot_start_rood != SPAT_TIJD_ONBEKEND) {
      if ( (!RA[i] || (CCOL2SPAT[i].status & SPAT_STATUS_RA_NO_GK)) && (SPAT_buf[i].tijd_tot_start_rood + TRG_max[i] > 0)  ) /* @@ geen MinEndTime indien conflict groen */
        SPAT_buf[i].tijd_tot_einde_rood_minimum = SPAT_buf[i].tijd_tot_start_rood + TRG_max[i];
    }
    /* 22-12-2017    dit is nog de vraag , of dit wel goed is ? */
    if ( RA[i] && (CCOL2SPAT[i].status & SPAT_STATUS_RA_NO_GK) && (CCOL2SPAT[i].min_tijd_tot_realisatie > SPAT_buf[i].tijd_tot_einde_rood_minimum) ) {  /* @@ PH 12-11-2017    MS:  alleen indien richting ook groen wordt, en ontruiming loopt. 21-03-2018 weer actief */
      SPAT_buf[i].tijd_tot_einde_rood_minimum = CCOL2SPAT[i].min_tijd_tot_realisatie;
    }
  }
  /* ----------------------------------------------------- */
  /* bepaal wijziging van NG naar tijd of van tijd naar NG */
  /* ----------------------------------------------------- */
  if ((tmp != SPAT_REMAIN_ONBEKEND) && (SPAT_buf[i].tijd_tot_einde_rood_minimum == SPAT_REMAIN_ONBEKEND)) {
    change_Rood |= SPAT_CHANGE_MINTIME_NG; /* indien RR[] of somgk(i) */
  }
  if ((tmp == SPAT_REMAIN_ONBEKEND) && (SPAT_buf[i].tijd_tot_einde_rood_minimum != SPAT_REMAIN_ONBEKEND)) {
    change_Rood |= SPAT_CHANGE_MINTIME_START; /* indien SR[] en !somgk[], of laatste conflict GEEL en RA[] */
  }

  if (SPAT_buf[i].tijd_tot_einde_rood_minimum > SPAT_REMAIN_ONBEKEND) SPAT_buf[i].mask_rood |= CIF_TIMING_MASKER_MINIMUM;

  /* ----------------------------------- */
  /* bepaal tijd tot einde groen minimum */
  /* ----------------------------------- */
  tmp = SPAT_buf[i].tijd_tot_einde_groen_minimum;

  SPAT_buf[i].tijd_tot_einde_groen_minimum = SPAT_REMAIN_ONBEKEND;

  if (G[i]
#if defined SPAT_HANDSHAKING_MINENDTIME | defined SPAT_HANDSHAKING_CCOL
        && (CIF_GUS[i] == CIF_WUS[i])  /* @@ 23-01-2018 wacht op WUS[] */   /* @@ -23-02-2018 als in sysdef HANDSHAKING actief, dan ook hier */
#endif
        ) {
    if (SPAT_buf[i].tijd_tot_start_groen != SPAT_TIJD_ONBEKEND) {
#ifdef SPAT_HANDSHAKING_CCOL
      /*          @@ 23-02-2018   gewijzigd t.b.v. HANDSHAKING MET CCOL   */
      if ( TGG[i] ) {
        SPAT_buf[i].tijd_tot_einde_groen_minimum = TGG_max[i] - TGG_timer[i];   /* volgens Ccol */
/*        SPAT_buf[i].tijd_tot_einde_groen_minimum = CCOL2SPAT[i].tgg_tijd; is flauweluk: test op TTG ? */ /* aangeleverd vanuit appl */
      }
#else
      if (SPAT_buf[i].tijd_tot_start_groen + TGG_max[i] > 0) 
        SPAT_buf[i].tijd_tot_einde_groen_minimum = SPAT_buf[i].tijd_tot_start_groen + TGG_max[i];
#endif
    }
  }
  /* ----------------------------------------------------- */
  /* bepaal wijziging van NG naar tijd of van tijd naar NG */
  /* ----------------------------------------------------- */
  if ((tmp != SPAT_REMAIN_ONBEKEND) && (SPAT_buf[i].tijd_tot_einde_groen_minimum == SPAT_REMAIN_ONBEKEND)) {
    change_Groen |= SPAT_CHANGE_MINTIME_NG; /* einde TGG */
  }
  if ((tmp == SPAT_REMAIN_ONBEKEND) && (SPAT_buf[i].tijd_tot_einde_groen_minimum != SPAT_REMAIN_ONBEKEND)) {
    change_Groen |= SPAT_CHANGE_MINTIME_START; /* @@ 23-02-2018 Nu wel nodig omdat TGG later kan starten ( pas als WUS[] == GUS[] ) */
  }

  if (SPAT_buf[i].tijd_tot_einde_groen_minimum > SPAT_REMAIN_ONBEKEND) SPAT_buf[i].mask_groen |= CIF_TIMING_MASKER_MINIMUM;


  /* ---------------------------------- */
  /* bepaal tijd tot einde geel minimum */
  /* ---------------------------------- */
  tmp = SPAT_buf[i].tijd_tot_einde_geel_minimum;

  SPAT_buf[i].tijd_tot_einde_geel_minimum = SPAT_REMAIN_ONBEKEND;

  if (GL[i]
#ifdef SPAT_HANDSHAKING_MINENDTIME
      && (CIF_GUS[i] == CIF_WUS[i]) /* @@ 23-01-2018 wacht op WUS[] */
#endif
      ) {
    if (SPAT_buf[i].tijd_tot_start_geel != SPAT_TIJD_ONBEKEND) {
      if (SPAT_buf[i].tijd_tot_start_geel + TGL_max[i] > 0)
        SPAT_buf[i].tijd_tot_einde_geel_minimum = SPAT_buf[i].tijd_tot_start_geel + TGL_max[i];
    }
  }

  /* ----------------------------------------------------- */
  /* bepaal wijziging van NG naar tijd of van tijd naar NG */
  /* ----------------------------------------------------- */
  if ((tmp != SPAT_REMAIN_ONBEKEND) && (SPAT_buf[i].tijd_tot_einde_geel_minimum == SPAT_REMAIN_ONBEKEND)) {
    change_Geel |= SPAT_CHANGE_MINTIME_NG;
  }
  if ((tmp == SPAT_REMAIN_ONBEKEND) && (SPAT_buf[i].tijd_tot_einde_geel_minimum != SPAT_REMAIN_ONBEKEND)) {
    change_Geel |= SPAT_CHANGE_MINTIME_START;
  }

  if (SPAT_buf[i].tijd_tot_einde_geel_minimum > SPAT_REMAIN_ONBEKEND) SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_MINIMUM;


  /* --------------------------------------------------------                                         */
  /* --------------------------------------------------------                                         */
  /* MAXIMUM END TIME ---------------------------------------                                         */
  /* --------------------------------------------------------                                         */
  /* --------------------------------------------------------                                         */

  /* --------------------------------------------------------                                         */
  /* bepaal of de applicatie T2SG_max[] of T2EG_max[] heeft gewijzigd                                         */
  /* --------------------------------------------------------                                         */

  /* @@ 9-3-2018  aanpassing i.v.m. het op '0' blijven hangen van T2SG_max[] */
  if ( CCOL2SPAT[i].max_rood_tijd == 0 ) {
    if ( (SPAT_buf[i].tijd_tot_einde_rood_maximum == SPAT_REMAIN_ONBEKEND) ) CCOL2SPAT[i].max_rood_tijd = NG;  /* niet op '0'starten, dan maar naar NG */
    if ( (SPAT_buf[i].tijd_tot_einde_rood_maximum == 0) && TE ) CCOL2SPAT[i].max_rood_tijd = NG;  /* geen '0' blijven sturen, dan maar naar NG */
  }

  /* wijziging in voorspelling testen 'Start Groen maximum' */
  if ((SPAT_buf[i].old_tijd_tot_einde_rood_maximum == NG) && (CCOL2SPAT[i].max_rood_tijd != NG) ) {
    change_Rood |= SPAT_CHANGE_MAX_START;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_rood_maximum != NG) && (CCOL2SPAT[i].max_rood_tijd == NG)) {
    change_Rood |= SPAT_CHANGE_MAX_NG;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_rood_maximum >= 0) && (CCOL2SPAT[i].max_rood_tijd >= 0)) {
    if ((CCOL2SPAT[i].max_rood_tijd > SPAT_buf[i].tijd_tot_einde_rood_maximum) && (CCOL2SPAT[i].max_rood_tijd - SPAT_buf[i].old_tijd_tot_einde_rood_maximum > SPAT_CHANGE_DIFF_TIME)) {
      change_Rood |= SPAT_CHANGE_MAX_DIFF;         /* tijd is significant langer geworden */
    }
    if ((SPAT_buf[i].old_tijd_tot_einde_rood_maximum > CCOL2SPAT[i].max_rood_tijd) && (SPAT_buf[i].old_tijd_tot_einde_rood_maximum - CCOL2SPAT[i].max_rood_tijd > SPAT_CHANGE_DIFF_TIME) ) {
      change_Rood |= SPAT_CHANGE_MAX_DIFF;         /* tijd is significant korter geworden */
    }
  }

  /* @@ 9-3-2018  aanpassing i.v.m. het op '0' blijven hangen van T2EG_max[] */
  if ( CCOL2SPAT[i].max_groen_tijd == 0 ) {
    if ( (SPAT_buf[i].tijd_tot_einde_groen_maximum == SPAT_REMAIN_ONBEKEND) ) CCOL2SPAT[i].max_groen_tijd = NG;  /* niet op '0'starten, dan maar naar NG */
    if ( (SPAT_buf[i].tijd_tot_einde_groen_maximum == 0) && TE ) CCOL2SPAT[i].max_groen_tijd = NG;  /* geen '0' blijven sturen, dan maar naar NG */
  }

  /* wijziging in voorspelling testen 'Einde Groen schatting' */
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_maximum == NG) && (CCOL2SPAT[i].max_groen_tijd != NG)) {
    change_Groen |= SPAT_CHANGE_MAX_START;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_maximum != NG) && (CCOL2SPAT[i].max_groen_tijd == NG)) {
    change_Groen |= SPAT_CHANGE_MAX_NG;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_maximum >= 0) && (CCOL2SPAT[i].max_groen_tijd >= 0)) {
    if ((CCOL2SPAT[i].tijd_tot_einde_groen > SPAT_buf[i].old_tijd_tot_einde_groen_maximum) && (CCOL2SPAT[i].max_groen_tijd - SPAT_buf[i].old_tijd_tot_einde_groen_maximum > SPAT_CHANGE_DIFF_TIME)) {
      change_Groen |= SPAT_CHANGE_MAX_DIFF;         /* tijd is significant langer geworden */
    }
    if ((SPAT_buf[i].old_tijd_tot_einde_groen_maximum > CCOL2SPAT[i].max_groen_tijd) && (SPAT_buf[i].old_tijd_tot_einde_groen_maximum - CCOL2SPAT[i].max_groen_tijd > SPAT_CHANGE_DIFF_TIME) ) {
      change_Groen |= SPAT_CHANGE_MAX_DIFF;         /* tijd is significant korter geworden */
    }
  }

  /* ------------------------------------- */
  /* Schrijf tijd tot einde rood maximum   */
  /* ------------------------------------- */
  tmp = SPAT_buf[i].tijd_tot_einde_rood_maximum;
  SPAT_buf[i].tijd_tot_einde_rood_maximum = SPAT_REMAIN_ONBEKEND;
  if (CCOL2SPAT[i].max_rood_tijd >= 0) {
    if ( (CCOL2SPAT[i].max_rood_tijd > 0) || (tmp > 0) )
      SPAT_buf[i].tijd_tot_einde_rood_maximum = CCOL2SPAT[i].max_rood_tijd + SPAT_T2SG_CORR_R;
    if ( (CCOL2SPAT[i].max_rood_tijd == 0) && (tmp > 0) ) {
      if ( TE ) {
        if ( tmp > TE ) tmp -= TE;
        else tmp = 0;
      }
      if ( tmp < SPAT_buf[i].tijd_tot_einde_rood_maximum )
        SPAT_buf[i].tijd_tot_einde_rood_maximum = tmp;
    }
    SPAT_buf[i].mask_rood |= CIF_TIMING_MASKER_MAXIMUM;  /* meld dat er een maximum is */
  }

  /* -------------------------------------- */
  /* Schrijf tijd tot einde groen schatting */
  /* -------------------------------------- */
  SPAT_buf[i].tijd_tot_einde_groen_maximum = SPAT_REMAIN_ONBEKEND;
  if (CCOL2SPAT[i].max_groen_tijd >= 0) {
    SPAT_buf[i].tijd_tot_einde_groen_maximum = CCOL2SPAT[i].max_groen_tijd;
    SPAT_buf[i].mask_groen |= CIF_TIMING_MASKER_MAXIMUM;  /* meld dat er een maximum is */
  }

  /* MAXIMUM EINDE */


  /* --------------------------------------------------------                                         */
  /* bepaal of de applicatie T2SG[] of T2EG[] heeft gewijzigd                                         */
  /* --------------------------------------------------------                                         */

  /* @@ 9-3-2018  aanpassing i.v.m. het op '0' blijven hangen van T2SG[] */
  if ( CCOL2SPAT[i].tijd_tot_einde_rood == 0 ) {
    if ( (SPAT_buf[i].tijd_tot_einde_rood_schatting == SPAT_REMAIN_ONBEKEND) ) CCOL2SPAT[i].tijd_tot_einde_rood = NG;  /* niet op '0'starten, dan maar naar NG */
    if ( (SPAT_buf[i].tijd_tot_einde_rood_schatting == 0) && TE ) CCOL2SPAT[i].tijd_tot_einde_rood = NG;  /* geen '0' blijven sturen, dan maar naar NG */
  }

  /* wijziging in voorspelling testen 'Start Groen schatting' */
  if ((SPAT_buf[i].old_tijd_tot_einde_rood_schatting == NG) && (CCOL2SPAT[i].tijd_tot_einde_rood != NG) ) {
    change_Rood |= SPAT_CHANGE_LIKELY_START;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_rood_schatting != NG) && (CCOL2SPAT[i].tijd_tot_einde_rood == NG)) {
    change_Rood |= SPAT_CHANGE_LIKELY_NG;
    if (SPAT_buf[i].tijd_tot_einde_rood_kans > NG) change_Rood |= SPAT_CHANGE_BT_NG;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_rood_schatting >= 0) && (CCOL2SPAT[i].tijd_tot_einde_rood >= 0)) {
    if ((CCOL2SPAT[i].tijd_tot_einde_rood > SPAT_buf[i].tijd_tot_einde_rood_schatting) && (CCOL2SPAT[i].tijd_tot_einde_rood - SPAT_buf[i].old_tijd_tot_einde_rood_schatting > SPAT_CHANGE_DIFF_TIME)) {
      change_Rood |= SPAT_CHANGE_LIKELY_DIFF;         /* tijd is significant langer geworden */
    }
    if ((SPAT_buf[i].old_tijd_tot_einde_rood_schatting > CCOL2SPAT[i].tijd_tot_einde_rood) && (SPAT_buf[i].old_tijd_tot_einde_rood_schatting - CCOL2SPAT[i].tijd_tot_einde_rood > SPAT_CHANGE_DIFF_TIME) ) {
      change_Rood |= SPAT_CHANGE_LIKELY_DIFF;         /* tijd is significant korter geworden */
    }
    /* @@ 19-03-2018   niet meer in gebruik 
    if ( (CCOL2SPAT[i].status & SPAT_STATUS_RA_GRN) && 
      (CCOL2SPAT[i].tijd_tot_einde_rood_schatting != SPAT_buf[i].old_tijd_tot_einde_rood_schatting) ) change_Rood |= SPAT_CHANGE_LIKELY_DIFF; 
    */
    /* als groen worden zeker is, ook bij een kleine correctie al een event versturen */
  }

  /* @@ 9-3-2018  aanpassing i.v.m. het op '0' blijven hangen van T2EG[] */
  if ( CCOL2SPAT[i].tijd_tot_einde_groen == 0 ) {
    if ( (SPAT_buf[i].tijd_tot_einde_groen_schatting == SPAT_REMAIN_ONBEKEND) ) CCOL2SPAT[i].tijd_tot_einde_groen = NG;  /* niet op '0'starten, dan maar naar NG */
    if ( (SPAT_buf[i].tijd_tot_einde_groen_schatting == 0) && TE ) CCOL2SPAT[i].tijd_tot_einde_groen = NG;  /* geen '0' blijven sturen, dan maar naar NG */
  }

  /* wijziging in voorspelling testen 'Einde Groen schatting' */
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_schatting == NG) && (CCOL2SPAT[i].tijd_tot_einde_groen != NG)) {
    change_Groen |= SPAT_CHANGE_LIKELY_START;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_schatting != NG) && (CCOL2SPAT[i].tijd_tot_einde_groen == NG)) {
    change_Groen |= SPAT_CHANGE_LIKELY_NG;
    if (SPAT_buf[i].tijd_tot_einde_groen_kans > NG) change_Groen |= SPAT_CHANGE_BT_NG;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_schatting >= 0) && (CCOL2SPAT[i].tijd_tot_einde_groen >= 0)) {
    if ((CCOL2SPAT[i].tijd_tot_einde_groen > SPAT_buf[i].old_tijd_tot_einde_groen_schatting) && (CCOL2SPAT[i].tijd_tot_einde_groen - SPAT_buf[i].old_tijd_tot_einde_groen_schatting > SPAT_CHANGE_DIFF_TIME)) {
      change_Groen |= SPAT_CHANGE_LIKELY_DIFF;         /* tijd is significant langer geworden */
    }
    if ((SPAT_buf[i].old_tijd_tot_einde_groen_schatting > CCOL2SPAT[i].tijd_tot_einde_groen) && (SPAT_buf[i].old_tijd_tot_einde_groen_schatting - CCOL2SPAT[i].tijd_tot_einde_groen > SPAT_CHANGE_DIFF_TIME) ) {
      change_Groen |= SPAT_CHANGE_LIKELY_DIFF;         /* tijd is significant korter geworden */
    }
  }

  /* ------------------------------------- */
  /* Schrijf tijd tot einde rood schatting */
  /* ------------------------------------- */
  tmp = SPAT_buf[i].tijd_tot_einde_rood_schatting;
  SPAT_buf[i].tijd_tot_einde_rood_schatting = SPAT_REMAIN_ONBEKEND;
  if (CCOL2SPAT[i].tijd_tot_einde_rood >= 0) {
    if ( (CCOL2SPAT[i].tijd_tot_einde_rood > 0) || (tmp > 0) )
      SPAT_buf[i].tijd_tot_einde_rood_schatting = CCOL2SPAT[i].tijd_tot_einde_rood + SPAT_T2SG_CORR_R;
    if ( (CCOL2SPAT[i].tijd_tot_einde_rood == 0) && (tmp > 0) ) {
      if ( TE ) {
        if ( tmp > TE ) tmp -= TE;
        else tmp = 0;
      }
      if ( tmp < SPAT_buf[i].tijd_tot_einde_rood_schatting )
        SPAT_buf[i].tijd_tot_einde_rood_schatting = tmp;
    }
    SPAT_buf[i].mask_rood |= CIF_TIMING_MASKER_VOORSPELD;  /* meld dat er voorspelling is */
  }

  /* -------------------------------------- */
  /* Schrijf tijd tot einde groen schatting */
  /* -------------------------------------- */
  SPAT_buf[i].tijd_tot_einde_groen_schatting = SPAT_REMAIN_ONBEKEND;
  if (CCOL2SPAT[i].tijd_tot_einde_groen >= 0) {
    SPAT_buf[i].tijd_tot_einde_groen_schatting = CCOL2SPAT[i].tijd_tot_einde_groen;
    SPAT_buf[i].mask_groen |= CIF_TIMING_MASKER_VOORSPELD;  /* meld dat er voorspelling is */
  }

  /* ------------------------------------- */
  /* Schrijf tijd tot einde geel schatting */
  /* ------------------------------------- */
  if (GL[i]) {
    SPAT_buf[i].tijd_tot_einde_geel_maximum = TGL_max[i] - TGL_timer[i] + SPAT_T2SR_CORR_GL;
    SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_MAXIMUM;   /* meld dat er een maximum tijd is */

    SPAT_buf[i].tijd_tot_einde_geel_schatting = TGL_max[i] - TGL_timer[i] + SPAT_T2SR_CORR_GL;
    SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_VOORSPELD; /* meld dat er voorspelling is */

    if (SGL[i]) {
/*    change_Geel |= SPAT_CHANGE_MAXIMUM;  niet nodig, er is al een SPAT_CHANGE_STATE */
      change_Geel |= SPAT_CHANGE_LIKELY; /* 22-01-2018 ��nmalig event op SGL[] */
      change_Geel |= SPAT_CHANGE_BT; /* 22-01-2018 ��nmalig event op SGL[]  de '100%' kans komt verderop */
    }
  }
  else {
    if (CCOL2SPAT[i].tijd_tot_einde_groen >= 0) {
      /* @@ 29-03-2018: MS GEEN einde geel maximum indien niet geel
      SPAT_buf[i].tijd_tot_einde_geel_maximum = CCOL2SPAT[i].tijd_tot_einde_groen + TGL_max[i] + SPAT_T2SR_CORR_GL;
      SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_MAXIMUM; 
      change_Geel |= change_Groen&SPAT_CHANGE_MAX;
      */
      SPAT_buf[i].tijd_tot_einde_geel_schatting = CCOL2SPAT[i].tijd_tot_einde_groen + TGL_max[i] + SPAT_T2SR_CORR_GL;
      SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_VOORSPELD;  /* meld dat er voorspelling is */
      change_Geel |= change_Groen&SPAT_CHANGE_LIKELY;

    }
    else {
      if (SPAT_buf[i].tijd_tot_einde_geel_maximum != SPAT_REMAIN_ONBEKEND)
        change_Geel |= SPAT_CHANGE_MAX_NG;
      SPAT_buf[i].tijd_tot_einde_geel_maximum = SPAT_REMAIN_ONBEKEND;

      if (SPAT_buf[i].tijd_tot_einde_geel_schatting != SPAT_REMAIN_ONBEKEND)
        change_Geel |= change_Groen&SPAT_CHANGE_LIKELY_NG;
      SPAT_buf[i].tijd_tot_einde_geel_schatting = SPAT_REMAIN_ONBEKEND;
    }
  }

#ifdef BT_SPAT
  /* ------------------------------------- */
  /* bepaal test en update betrouwbaarheid */
  /* ------------------------------------- */
  if (change_Rood && (CCOL2SPAT[i].tijd_tot_einde_rood >= 0)) {
    /* indien er een wijziging is, dan altijd betrouwbaarheid testen, en registratie uitvoeren */
    change_Rood |= SPAT_CHANGE_BT_TEST;
    if (R[i] && (CCOL2SPAT[i].tijd_tot_einde_groen >= 0)) { /* en ook bij de 'volgende' status */
      change_Groen |= SPAT_CHANGE_BT_TEST;
    }
  }
  if (change_Groen && (CCOL2SPAT[i].tijd_tot_einde_groen >= 0)) {
    /* indien er een wijziging is, dan altijd betrouwbaarheid testen, en registratie uitvoeren */
    change_Groen |= SPAT_CHANGE_BT_TEST;
    if (!R[i] && (CCOL2SPAT[i].tijd_tot_einde_rood >= 0)) { /* en ook bij de 'volgende' status */
      /* dit kan eigenlijk niet. Tijdens groen is er nooit een voorspelling voor startgroen */
      change_Rood |= SPAT_CHANGE_BT_TEST;
    }
  }

  /* ---------------------------------------------------------- */
  /* wijziging in betrouwbaarheid testen 'tijd tot start Groen' */
  /* ---------------------------------------------------------- */
  if ((CCOL2SPAT[i].tijd_tot_einde_rood >= 0) && (change_Rood || elke_5_sec )) {
    tmp = proc_geef_bt_t2s( &BT_SPAT_T2SG[i], (mulv)CCOL2SPAT[i].tijd_tot_einde_rood);
    if (tmp <= NG) {
      if (SPAT_buf[i].tijd_tot_einde_rood_kans != NG) {
        change_Rood |= SPAT_CHANGE_BT_NG;
      }
      else if ( (tmp < NG) && elke_5_sec ) {  
        /* bij onvoldoende data (tmp == -2) , ��n keer per 5 sec, en betrouwbaarheid == -2 ( => geen / te weinig data ) */
        change_Rood |= SPAT_CHANGE_BT_TEST;  /* t.b.v. alleen registratie !! */
      }
    }
    else {
      if (change_Rood&SPAT_CHANGE_BT_TEST) { /* update betrouwbaarheid gewenst */
        change_Rood |= SPAT_CHANGE_BT; /* nieuwe betrouwbaarheid berekenen en versturen */
      }
      if (SPAT_buf[i].tijd_tot_einde_rood_kans == NG) {
        change_Rood |= SPAT_CHANGE_BT_START;
      }
      else {
        if ((SPAT_buf[i].tijd_tot_einde_rood_kans > tmp) && (SPAT_buf[i].tijd_tot_einde_rood_kans - tmp > SPAT_CHANGE_DIFF_BT)) {
          change_Rood |= SPAT_CHANGE_BT_DIFF;
        }
        if ((SPAT_buf[i].tijd_tot_einde_rood_kans < tmp) && (tmp - SPAT_buf[i].tijd_tot_einde_rood_kans > SPAT_CHANGE_DIFF_BT)) {
          change_Rood |= SPAT_CHANGE_BT_DIFF;
        }
      }
    }
  }


  /* ---------------------------------------------------------- */
  /* wijziging in betrouwbaarheid testen 'tijd tot einde Groen' */
  /* ---------------------------------------------------------- */
  if ((CCOL2SPAT[i].tijd_tot_einde_groen >= 0) && (change_Groen || elke_5_sec )) {
    tmp = proc_geef_bt_t2s( &BT_SPAT_T2EG[i], (mulv) CCOL2SPAT[i].tijd_tot_einde_groen);
    if (tmp <= NG) {
      if (SPAT_buf[i].tijd_tot_einde_groen_kans != NG) {
        change_Groen |= SPAT_CHANGE_BT_NG;
      }
      else if ( (tmp < NG) && elke_5_sec ) {  /* ��n keer per 5 sec, en betrouwbaarheid == -2 ( => geen / te weinig data ) */
        change_Groen |= SPAT_CHANGE_BT_TEST;  /* alleen registratie !! */
      }
    }
    else {
      if (change_Groen&SPAT_CHANGE_BT_TEST) { /* update betrouwbaarheid gewenst */
        change_Groen |= SPAT_CHANGE_BT; /* nieuwe betrouwbaarheid berekenen en versturen */
      }
      if (SPAT_buf[i].tijd_tot_einde_groen_kans == NG) {
        change_Groen |= SPAT_CHANGE_BT_START;
      }
      else {
        if ((SPAT_buf[i].tijd_tot_einde_groen_kans > tmp) && (SPAT_buf[i].tijd_tot_einde_groen_kans - tmp > SPAT_CHANGE_DIFF_BT)) {
          change_Groen |= SPAT_CHANGE_BT_DIFF;
        }
        if ((SPAT_buf[i].tijd_tot_einde_groen_kans < tmp) && (tmp - SPAT_buf[i].tijd_tot_einde_groen_kans > SPAT_CHANGE_DIFF_BT)) {
          change_Groen |= SPAT_CHANGE_BT_DIFF;
        }
      }
    }
  }

  /* De registratie van de voorspellingen t.b.v. de bepaling van de betrouwbaarheid */
  /* @@ 26-03-2018  nieuwe aanroep */
  /* @@ 30-03-2018 : 3 argumenten , daardoor geen SPAT_ Macros in BT_ */
  proc_spat_register_event(&BT_SPAT_T2SG[i], (mulv) CCOL2SPAT[i].tijd_tot_einde_rood, 
        (bool) change_Rood&SPAT_CHANGE_STATE_START, (bool) change_Rood&(SPAT_CHANGE_STATE_END), (bool) change_Rood&(SPAT_EVENT_BT), (mulv) TE);
  proc_spat_register_event(&BT_SPAT_T2EG[i], (mulv) CCOL2SPAT[i].tijd_tot_einde_groen, 
        (bool) change_Groen&SPAT_CHANGE_STATE_START, (bool) change_Groen&(SPAT_CHANGE_STATE_END), (bool) change_Groen&(SPAT_EVENT_BT), (mulv) TE);

  if (CCOL2SPAT[i].tijd_tot_einde_rood == NG) SPAT_buf[i].tijd_tot_einde_rood_kans = NG; /* 8-3-2018 extra beveiliging: GEEN Betrouwbaarheid indien er geen voorspelling is */

  tmp = SPAT_buf[i].tijd_tot_einde_rood_kans; /* 'oude' waarde onthouden */
  if (change_Rood&(SPAT_CHANGE_LIKELY | SPAT_CHANGE_BT)) {
    /* nieuwe betrouwbaarheid berekenen */
    SPAT_buf[i].tijd_tot_einde_rood_kans = proc_geef_bt_t2s( &BT_SPAT_T2SG[i], (mulv) CCOL2SPAT[i].tijd_tot_einde_rood);
    if ( SPAT_buf[i].tijd_tot_einde_rood_kans < NG ) SPAT_buf[i].tijd_tot_einde_rood_kans = NG;
    SPAT_buf[i].tijd_tot_einde_rood_kans = SPAT_buf[i].tijd_tot_einde_rood_kans;
  }
  else {
    SPAT_buf[i].tijd_tot_einde_rood_kans = tmp; /* 'oude' waarde terugschrijven */
  }

  if (CCOL2SPAT[i].tijd_tot_einde_groen == NG) SPAT_buf[i].tijd_tot_einde_groen_kans = NG; /* 8-3-2018 extra beveiliging: GEEN Betrouwbaarheid indien er geen voorspelling is */

  tmp = SPAT_buf[i].tijd_tot_einde_groen_kans; /* 'oude' waarde onthouden */
  if (change_Groen&(SPAT_CHANGE_LIKELY | SPAT_CHANGE_BT)) {
    /* nieuwe betrouwbaarheid berekenen */
    SPAT_buf[i].tijd_tot_einde_groen_kans = proc_geef_bt_t2s(  &BT_SPAT_T2EG[i], (mulv) CCOL2SPAT[i].tijd_tot_einde_groen);
    if ( SPAT_buf[i].tijd_tot_einde_groen_kans < NG ) SPAT_buf[i].tijd_tot_einde_groen_kans = NG;
    SPAT_buf[i].tijd_tot_einde_groen_kans = SPAT_buf[i].tijd_tot_einde_groen_kans;
  }
  else { 
    SPAT_buf[i].tijd_tot_einde_groen_kans = tmp; /* 'oude' waarde doorschrijven */
  }

  if (GL[i]) {
    SPAT_buf[i].tijd_tot_einde_geel_kans = 100;
    SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_BETROUWBAARHEID;  /* meld dat er een betrouwbaarheid is */
  }
  else {
    /* betrouwbaarheid groen doorschrijven naar geel */
    /* 8-3-2018  ALTIJD behelve tijdens geel */
    SPAT_buf[i].tijd_tot_einde_geel_kans = SPAT_buf[i].tijd_tot_einde_groen_kans;
    if (SPAT_buf[i].tijd_tot_einde_groen_kans >= 0) {
      SPAT_buf[i].mask_geel |= CIF_TIMING_MASKER_BETROUWBAARHEID;  /* meld dat er een betrouwbaarheid is */
    }
  }

  if (SPAT_buf[i].tijd_tot_einde_rood_kans >= 0) {
    SPAT_buf[i].mask_rood |= CIF_TIMING_MASKER_BETROUWBAARHEID;  /* meld dat er een betrouwbaarheid is */
  }

  if (SPAT_buf[i].tijd_tot_einde_groen_kans >= 0) {
    SPAT_buf[i].mask_groen |= CIF_TIMING_MASKER_BETROUWBAARHEID;  /* meld dat er een betrouwbaarheid is */
  }
#endif

  /* reset BITs voor registratie, geen data verzenden indien er geen andere bits actief zijn. */
  change_Rood &= TIMING_MASK_ALL;
  change_Geel &= TIMING_MASK_ALL;
  change_Groen &= TIMING_MASK_ALL;

  /* @@ 24-01-2018 */
  /* deze correcties wordt NIET gevlagd, en ook NIET bijgehouden. Normaal gesproken zou dit niet nodig zijn.
  Echter wordt de minimum tijden (tijdelijk) gecorrigeerd t.b.v. GUS-WUS afwijking en berichten-stroom afwijkingen aan Swarco zijde */

  /* beveiliging voorspelling mag niet kleiner zijn als _minimum tijd */
  if ((SPAT_buf[i].tijd_tot_einde_rood_schatting >= 0) && (SPAT_buf[i].tijd_tot_einde_rood_minimum > 0)) {
    if (SPAT_buf[i].tijd_tot_einde_rood_schatting < SPAT_buf[i].tijd_tot_einde_rood_minimum) {
      SPAT_buf[i].tijd_tot_einde_rood_schatting = SPAT_buf[i].tijd_tot_einde_rood_minimum;
    }
  }

  /* beveiliging starttijd mag niet kleiner zijn als _minimum tijd van 'vorige status */
  /* Starttijd NOOIT aanpassen !!
  if ((SPAT_buf[i].tijd_tot_start_rood >= 0) && (SPAT_buf[i].tijd_tot_einde_geel_minimum > 0)) {
    if (SPAT_buf[i].tijd_tot_start_rood < SPAT_buf[i].tijd_tot_einde_geel_minimum) {
      SPAT_buf[i].tijd_tot_start_rood = SPAT_buf[i].tijd_tot_einde_geel_minimum;
    }
  }
  */

  /* beveiliging voorspelling mag niet kleiner zijn als _minimum tijd */
  if ((SPAT_buf[i].tijd_tot_einde_groen_schatting >= 0) && (SPAT_buf[i].tijd_tot_einde_groen_minimum > 0)) {
    if (SPAT_buf[i].tijd_tot_einde_groen_schatting < SPAT_buf[i].tijd_tot_einde_groen_minimum) {
      SPAT_buf[i].tijd_tot_einde_groen_schatting = SPAT_buf[i].tijd_tot_einde_groen_minimum;
    }
  }

  /* beveiliging starttijd mag niet kleiner zijn als _minimum tijd van 'vorige status */
  /* Starttijd NOOIT aanpassen !!
  if ((SPAT_buf[i].tijd_tot_start_groen >= 0) && (SPAT_buf[i].tijd_tot_einde_rood_minimum > 0)) {
    if (SPAT_buf[i].tijd_tot_start_groen < SPAT_buf[i].tijd_tot_einde_rood_minimum) {
      SPAT_buf[i].tijd_tot_start_groen = SPAT_buf[i].tijd_tot_einde_rood_minimum;
    }
  }
  */

  /* beveiliging voorspelling mag niet kleiner zijn als _minimum tijd */
  if ((SPAT_buf[i].tijd_tot_einde_geel_schatting >= 0) && (SPAT_buf[i].tijd_tot_einde_geel_minimum > 0)) {
    if (SPAT_buf[i].tijd_tot_einde_geel_schatting < SPAT_buf[i].tijd_tot_einde_geel_minimum) {
      SPAT_buf[i].tijd_tot_einde_geel_schatting = SPAT_buf[i].tijd_tot_einde_geel_minimum;
    }
  }

  /* beveiliging starttijd mag niet kleiner zijn als _minimum tijd van 'vorige status */
  /* Starttijd NOOIT aanpassen !!
  if ((SPAT_buf[i].tijd_tot_start_geel >= 0) && (SPAT_buf[i].tijd_tot_einde_groen_minimum > 0)) {
    if (SPAT_buf[i].tijd_tot_start_geel < SPAT_buf[i].tijd_tot_einde_groen_minimum) {
      SPAT_buf[i].tijd_tot_start_geel = SPAT_buf[i].tijd_tot_einde_groen_minimum;
    }
  }
  */

  /* @@ 29-03-2018 : MS */
  /* beveiliging maximumtijd mag niet kleiner zijn als voorspellings tijd */
  if ((SPAT_buf[i].tijd_tot_einde_rood_maximum >= 0) && (SPAT_buf[i].tijd_tot_einde_rood_schatting > 0)) {
    if (SPAT_buf[i].tijd_tot_einde_rood_maximum < SPAT_buf[i].tijd_tot_einde_rood_schatting) {
      SPAT_buf[i].tijd_tot_einde_rood_maximum = SPAT_buf[i].tijd_tot_einde_rood_schatting;
    }
  }
  /* beveiliging maximumtijd mag niet kleiner zijn als voorspellings tijd */
  if ((SPAT_buf[i].tijd_tot_einde_groen_maximum >= 0) && (SPAT_buf[i].tijd_tot_einde_groen_schatting > 0)) {
    if (SPAT_buf[i].tijd_tot_einde_groen_maximum < SPAT_buf[i].tijd_tot_einde_groen_schatting) {
      SPAT_buf[i].tijd_tot_einde_groen_maximum = SPAT_buf[i].tijd_tot_einde_groen_schatting;
    }
  }
  /* beveiliging maximumtijd mag niet kleiner zijn als voorspellings tijd */
  if ((SPAT_buf[i].tijd_tot_einde_geel_maximum >= 0) && (SPAT_buf[i].tijd_tot_einde_geel_schatting > 0)) {
    if (SPAT_buf[i].tijd_tot_einde_geel_maximum < SPAT_buf[i].tijd_tot_einde_geel_schatting) {
      SPAT_buf[i].tijd_tot_einde_geel_maximum = SPAT_buf[i].tijd_tot_einde_geel_schatting;
    }
  }

  /* Doorschrijven Masker en wijzigings vlaggen */
  if (change_Rood&TIMING_MASK_ALL) {
    SPAT_buf[i].event_vlag |= (1 << CIF_TIMING_ROOD);
  }

  if (change_Groen&TIMING_MASK_ALL) {
    SPAT_buf[i].event_vlag |= (1 << CIF_TIMING_GROEN);
  }

  if (change_Geel&TIMING_MASK_ALL) {
    SPAT_buf[i].event_vlag |= (1 << CIF_TIMING_GEEL);
  }

  /* bijwerken _old indien wijzigingsvlag actief */
  if (SPAT_buf[i].event_vlag) {
    SPAT_buf[i].old_tijd_tot_einde_rood_schatting = CCOL2SPAT[i].tijd_tot_einde_rood;
    SPAT_buf[i].old_tijd_tot_einde_groen_schatting = CCOL2SPAT[i].tijd_tot_einde_groen;
    SPAT_buf[i].old_tijd_tot_einde_rood_maximum = CCOL2SPAT[i].max_rood_tijd;
    SPAT_buf[i].old_tijd_tot_einde_groen_maximum = CCOL2SPAT[i].max_groen_tijd;
  }

  if ((SPAT_buf[i].old_tijd_tot_einde_rood_schatting >= 0) && (CCOL2SPAT[i].tijd_tot_einde_rood >= 0)) {
    if (SPAT_buf[i].old_tijd_tot_einde_rood_schatting >= TE) SPAT_buf[i].old_tijd_tot_einde_rood_schatting -= TE;
    else SPAT_buf[i].old_tijd_tot_einde_rood_schatting = 0;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_schatting >= 0) && (CCOL2SPAT[i].tijd_tot_einde_groen >= 0)) {
    if (SPAT_buf[i].old_tijd_tot_einde_groen_schatting >= TE) SPAT_buf[i].old_tijd_tot_einde_groen_schatting -= TE;
    else SPAT_buf[i].old_tijd_tot_einde_groen_schatting = 0;
  }

  if ((SPAT_buf[i].old_tijd_tot_einde_rood_maximum >= 0) && (CCOL2SPAT[i].max_rood_tijd >= 0)) {
    if (SPAT_buf[i].old_tijd_tot_einde_rood_maximum >= TE) SPAT_buf[i].old_tijd_tot_einde_rood_maximum -= TE;
    else SPAT_buf[i].old_tijd_tot_einde_rood_maximum = 0;
  }
  if ((SPAT_buf[i].old_tijd_tot_einde_groen_maximum >= 0) && (CCOL2SPAT[i].max_groen_tijd >= 0)) {
    if (SPAT_buf[i].old_tijd_tot_einde_groen_maximum >= TE) SPAT_buf[i].old_tijd_tot_einde_groen_maximum -= TE;
    else SPAT_buf[i].old_tijd_tot_einde_groen_maximum = 0;
  }

}

