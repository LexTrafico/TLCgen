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
  #include "fcvar.h"      /* fasecyclus variabelen en declaratie FCMAX */
  #include "kfvar.h"      /* conflict variabelen */
  #include "cif.inc"      /* interface */
  #include "control.h"    /* t.b.v. TE */

#ifndef T2R_T2G_TRAFFICK_H

  /* macrodefinities */ 
  /* =============== */ 
  #define T2R_T2G_TRAFFICK


  /* declaratie globale variabelen */
  /* ============================= */
  mulv T2SG[FCMAX];                 /* tijd tot start groen volgens TRAFFICK                            */
  mulv T2EG[FCMAX];                 /* tijd tot einde groen volgens TRAFFICK                            */

  mulv T2SGM[FCMAX];                /* maximum tijd tot start groen volgens TRAFFICK                    */
  mulv T2EGM[FCMAX];                /* maximum tijd tot einde groen volgens TRAFFICK                    */

#ifdef NO_WIJZ_2018_03_19
  /* 17-03-2018: variabelen vrij voor gebruik binnen traffick */
  mulv T2SG_old[FCMAX];             /* tijd tot start groen volgens CCOL vorige machineslag             */
  mulv T2EG_old[FCMAX];             /* tijd tot einde groen volgens CCOL vorige machineslag             */
  
  mulv BT2SG[FCMAX];                /* betrouwbaarheid tijd tot start groen volgens CCOL                */
  mulv BT2EG[FCMAX];                /* betrouwbaarheid tijd tot einde groen volgens CCOL                */
#endif

  /* declaraties van functies */
  /* ======================== */
  void proc_groen_new(void);
  void Time_to_Red_Time_to_Green(void);
  void proc_blwt_new(void);

#ifdef NO_WIJZ_2018_03_19
  void proc_us_fc_timing(count fc, count ust2sg, count usbt2sg, count ust2eg, count usbt2eg);
#endif

  void fc_timing_traffick(void);


  #define T2R_T2G_TRAFFICK_H

#endif


