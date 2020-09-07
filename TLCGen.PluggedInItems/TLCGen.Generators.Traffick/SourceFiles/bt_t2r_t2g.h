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
 *  Filenaam        : bt_t2r_t2g.h
 *  Ontwerp         : Marc Scherjon
 *  Datum           : 26-09-2017
 *  CCOL            : versie 9.0 en hoger,  Generator PNH versie 15 en hoger
 *
  Revisie:
    26-09-2017 M. Scherjon:  opzet
    15-03-2018 M. Scherjon:  code format aangepast
    17-03-2018 M. Scherjon:  Array van struct anders ingedeeld.
                             3e argument van proc_geef_bt_t2s() gewijzigd in pointer naar struct
                             3 hulpfuncties t.b.v. struct toegevoegd.
                             vervallen functies verwijderd
    26-03-2018 M. Scherjon:  Betrouwbaarheids registratie voor elke status gebruik van dezelfde functie.
    30-03-2018 M. Scherjon:  Betrouwbaarheids registratie test bits als argument.
    30-03-2018 M. Scherjon:  proc_spat_register_event() argumenten aangepast, SPAT macro's vervallen
    04-05-2018 M. Scherjon:  tekst aanpassing, en ongebruikte / vervallen includes verwijderd ( in bt_t2r_t2g.c ).

*/
  #include "fcvar.h"        /* t.b.v. declaratie FCMAX */


#ifndef BT_SPAT_H

/* macrodefinities */ 
/* =============== */ 
  #define BT_SPAT_MAX                 200     /*  (intern) maximum toegepast aantal buffers per FC */

/* declaratie globale variabelen */
/* ============================= */
  struct _BT_SPAT_T2S {
    mulv volgnummer[BT_SPAT_MAX];          /* volgnummer                */
    mulv timing[BT_SPAT_MAX];              /* lifetime event            */
    mulv t2s[BT_SPAT_MAX];                 /* verzonden voorspelling    */
    mulv bt[BT_SPAT_MAX];                  /* verstuurde kans : is eigenlijk niet nodig, wel voor test */
    mulv afwijking[BT_SPAT_MAX];           /* verschil in procenten positief = te hoog geschat, negatief = te laat geschat */
    mulv score[BT_SPAT_MAX];               /* behaalde resultaat   ( vergelijken met bt )  */
  };

#ifndef BT_SPAT_C
  struct _BT_SPAT_T2S  BT_SPAT_T2SG[FCMAX]; /* buffer historische SPAT EVENTS, t.b.v. berekenen/bepalen betrouwbaarheid. */
  struct _BT_SPAT_T2S  BT_SPAT_T2EG[FCMAX]; /* buffer historische SPAT EVENTS, t.b.v. berekenen/bepalen betrouwbaarheid. */
 #ifndef BT_SPAT
  #define BT_SOURCE_C   /* Dit in commentaar indien als object */
  #define BT_SPAT
 #endif
#else
  struct _BT_SPAT_T2S  BT_SPAT_T2SG[]; /* buffer historische SPAT EVENTS, t.b.v. berekenen/bepalen betrouwbaarheid. */
  struct _BT_SPAT_T2S  BT_SPAT_T2EG[]; /* buffer historische SPAT EVENTS, t.b.v. berekenen/bepalen betrouwbaarheid. */
#endif

  /* declaraties van functies */
  /* ======================== */
  void init_spat_bt(count fcmax, bool init);
  void proc_spat_register_event( struct _BT_SPAT_T2S *_t2s, mulv t2s, bool status_start, bool status_end, bool bt_event, mulv tiendesec); /* @@ 26-03-2018 , @@ 30-03-2018 argumenten voor start, end en event */
  mulv proc_geef_bt_t2s( struct _BT_SPAT_T2S *_t2s, mulv voorspel); /* @@ 17-03-2018 */

  #define BT_SPAT_H

#endif


