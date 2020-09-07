/* DEFINITIES SPAT FUNCTIES */
/* ======================== */
/*
 *  Huijskes VRT B.V.
 *  14 Oktoberplein 6
 *  Overloon
 *
 *
 *  Module: functies t.b.v. bijhouden en bepalen van betrouwbaarheid van uitgestuurde SPAT data 
 *
 *  Filenaam        : t2r_t2g.h
 *  Ontwerp         : Marc Scherjon
 *  Datum           : 17-03-2018
 *  CCOL            : versie 9.0 en hoger,  Generator PNH versie 15 en hoger
 *
  Revisie:
    17-03-2018 M. Scherjon:  opzet nieuwe versie

*/
  #include "fcvar.h"    /* t.b.v. declaratie FCMAX en CG_.. */
  #include "kfvar.h"    /* ? */
  #include "cif.inc"    /* CVN Interface */
  #include "control.h"  /* TE */

#ifndef T2R_T2G_H

/* macrodefinities */ 
/* =============== */

/* kopie van definition of values of CG[]		*/
  #define SPAT_STATUS_RA          CG_RA
  #define SPAT_STATUS_VS          CG_VS
  #define SPAT_STATUS_FG          CG_FG
  #define SPAT_STATUS_WG          CG_WG
  #define SPAT_STATUS_VG          CG_VG
  #define SPAT_STATUS_MG          CG_MG
  #define SPAT_STATUS_GL          CG_GL
  #define SPAT_STATUS_RV          CG_RV
  #define SPAT_STATUS_CG_MAX      CG_MAX   /* = 8 */
  #define SPAT_STATUS_CG_MAX2     0x0f     /* = 15 */
  #define SPAT_STATUS_RA_NO_GK    0x10     /* RA en geen conflictrichting in groen */
  #define SPAT_STATUS_RA_GRN      0x20     /* RA en Groen worden is zeker */
  #define SPAT_STATUS_RA_MAX      0xf0     /* = 240 */


/* Fasecyclus Realisatie */
  #define SPAT_REAL_PR            0x01
  #define SPAT_REAL_FPR           0x01 /* = PR */
  #define SPAT_REAL_AR            0x02
  #define SPAT_REAL_MR            0x04
  #define SPAT_REAL_BR            0x08
  #define SPAT_REAL_OV            0x10
  #define SPAT_REAL_HV            0x20


/* declaratie globale variabelen */
/* ============================= */
/* Voor uitwisselen tussen FC_timing en regelstruktuur */
struct _CCOL2SPAT {

  /* variabelen vanuit de regelstruktuur moeten worde ngeschreven t.b.v. berekenden en versturen van FC_TIMING */
  mulv status;
  mulv tgr_tijd;                      /* restand garantie rood tijd */
  mulv tgg_tijd;                      /* restand garantie groen tijd */
  mulv tgl_tijd;                      /* restand geel tijd */
  mulv min_tijd_tot_realisatie;       /* restand (deelconflict-)ontruimingstijd ( ook eventuele voorstart-, gelijkstart ) */
  mulv tijd_tot_einde_rood;           /* voorspelling vanuit regelstruktuur: 'wachttijdvoorspeller tijd' */
  mulv tijd_tot_einde_groen;          /* voorspelling einde groen */
  mulv max_groen_tijd;                /* maximum groentijd */
  mulv max_rood_tijd;                 /* maximum roodtijd */
  mulv realisatie;                    /* realisatie vorm  PR, AR, BR , ... */
  
  /* variabelen die door de FC_timing worden gevuld met de actuele gegevens die ook op de interface staan  */
  mulv tijd_tot_start_rood;             /* tijd tot start rood                                             */
  mulv tijd_tot_einde_rood_minimum;     /* tijd tot einde rood - minimum                                   */
  mulv tijd_tot_einde_rood_maximum;     /* tijd tot einde rood - maximum                                   */
  mulv tijd_tot_einde_rood_schatting;   /* tijd tot einde rood - schatting                                 */
  mulv tijd_tot_einde_rood_kans;        /* kans dat schatting juist is                                     */
  mulv tijd_tot_start_rood_volgende;    /* tijd tot start rood in de volgende cyclus                       */

  mulv tijd_tot_start_groen;            /* tijd tot start groen                                            */
  mulv tijd_tot_einde_groen_minimum;    /* tijd tot einde groen - minimum                                  */
  mulv tijd_tot_einde_groen_maximum;    /* tijd tot einde groen - maximum                                  */
  mulv tijd_tot_einde_groen_schatting;  /* tijd tot einde groen - schatting                                */
  mulv tijd_tot_einde_groen_kans;       /* kans dat schatting juist is                                     */
  mulv tijd_tot_start_groen_volgende;   /* tijd tot start groen in de volgende cyclus                      */

  mulv tijd_tot_start_geel;             /* tijd tot start geel                                             */
  mulv tijd_tot_einde_geel_minimum;     /* tijd tot einde geel - minimum                                   */
  mulv tijd_tot_einde_geel_maximum;     /* tijd tot einde geel - maximum                                   */
  mulv tijd_tot_einde_geel_schatting;   /* tijd tot einde geel - schatting                                 */
  mulv tijd_tot_einde_geel_kans;        /* kans dat schatting juist is                                     */
  mulv tijd_tot_start_geel_volgende;    /* tijd tot start geel in de volgende cyclus                       */

  mulv event_vlag;
  mulv mask_rood;
  mulv mask_geel;
  mulv mask_groen;

};

#ifndef T2R_T2G_C
  struct _CCOL2SPAT   CCOL2SPAT[FCMAX];
 #ifndef T2R_T2G
  #define T2R_T2G_SOURCE_C   /* Dit in commentaar indien als object */
  #define T2R_T2G
 #endif
#else
  struct _CCOL2SPAT   CCOL2SPAT[];
#endif

  /* declaraties van algemene functies */
  /* ================================= */
  void fc_timing_all(void);

  void schrijf_us_fc_timing( struct _CCOL2SPAT *ccol2spat, count ust2sg, count usbt2sg, count ust2eg, count usbt2eg);


  #define T2R_T2G_H

#endif


