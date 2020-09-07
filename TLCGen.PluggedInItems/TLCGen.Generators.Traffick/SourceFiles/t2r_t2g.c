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
 *  Filenaam        : t2r_t2g.c
 *  Ontwerp         : Marc Scherjon
 *  Datum           : 17-03-2018
 *  CCOL            : versie 9.0 en hoger,  Generator PNH versie 15 en hoger
 *
  Revisie:
    17-03-2018 M. Scherjon:  opzet nieuwe versie

*/

/* include files                                                                                          */
/* =============                                                                                          */

  #include "t2r_t2g.h"   /* t.b.v. declaratie globale variabele en macro's t.b.v. FC_timing */


/* Declaratie functies                                                                                    */
/* ===================                                                                                    */
  bool proc_schrijf_status( struct _CCOL2SPAT *ccol2spat, count fc);
  bool proc_schrijf_extra_status( struct _CCOL2SPAT *ccol2spat, count fc, bool no_gk, bool zeker);
  bool proc_schrijf_garantie_rood_tijd( struct _CCOL2SPAT *ccol2spat, count fc);
  bool proc_schrijf_garantie_groen_tijd( struct _CCOL2SPAT *ccol2spat, count fc);
  bool proc_schrijf_tijd_tot_groen( struct _CCOL2SPAT *ccol2spat, count fc , mulv t2sg);
  bool proc_schrijf_tijd_tot_einde_groen( struct _CCOL2SPAT *ccol2spat, count fc , mulv t2eg);
  bool proc_schrijf_max_tijd_rood( struct _CCOL2SPAT *ccol2spat, count fc , mulv t2sgm);
  bool proc_schrijf_max_tijd_groen( struct _CCOL2SPAT *ccol2spat, count fc , mulv t2egm);

/* DEFINITIE ALGEMENE FUNCTIES                                                                            */
/* ===========================                                                                            */

  void set_us_fc_timing( struct _CCOL2SPAT *ccol2spat, count ust2sg, count usbt2sg, count ust2eg, count usbt2eg)
  {

    if ( ust2sg > NG ) CIF_GUS[ust2sg] = ccol2spat->tijd_tot_einde_rood_schatting;
    if ( usbt2sg > NG ) CIF_GUS[usbt2sg] = ccol2spat->tijd_tot_einde_rood_kans;
    if ( ust2eg > NG ) CIF_GUS[ust2eg] = ccol2spat->tijd_tot_einde_groen_schatting;
    if ( usbt2eg > NG ) CIF_GUS[usbt2eg] = ccol2spat->tijd_tot_einde_groen_kans;

  }



/* DEFINITIE FUNCTIES                                                                                     */
/* ==================                                                                                     */

  bool proc_schrijf_status( struct _CCOL2SPAT *ccol2spat, count fc)
  {
  
    ccol2spat->status &= ~SPAT_STATUS_CG_MAX2;
    ccol2spat->status |= CG[fc];
    return(TRUE);

  }



  bool proc_schrijf_extra_status( struct _CCOL2SPAT *ccol2spat, count fc, bool no_gk, bool zeker)
  {

    if ( no_gk > NG ) {
      ccol2spat->status &= ~SPAT_STATUS_RA_NO_GK;
      if ( RA[fc] && (no_gk > 0) )
      ccol2spat->status |= SPAT_STATUS_RA_NO_GK;
    }

    if ( zeker > NG ) {
      ccol2spat->status &= ~SPAT_STATUS_RA_GRN;
      if ( zeker > 0 )
      ccol2spat->status |= SPAT_STATUS_RA_GRN;
    }

    return(TRUE);

  }



  bool proc_schrijf_garantie_rood_tijd( struct _CCOL2SPAT *ccol2spat, count fc)
  {

    mulv _rest_trg = NG;

    if ( !R[fc] ) _rest_trg = TRG_max[fc];
    if ( TRG[fc] ) _rest_trg = TRG_max[fc] - TRG_timer[fc];

    ccol2spat->tgr_tijd = _rest_trg;

    return(TRUE);

  }


  bool proc_schrijf_garantie_groen_tijd( struct _CCOL2SPAT *ccol2spat, count fc)
  {

    mulv _rest_tgg = NG;

    if ( !G[fc] ) _rest_tgg = TGG_max[fc];
    if ( TGG[fc] ) _rest_tgg = TGG_max[fc] - TGG_timer[fc];

    ccol2spat->tgg_tijd = _rest_tgg;

    return(TRUE);

  }

  bool proc_schrijf_geel_tijd( struct _CCOL2SPAT *ccol2spat, count fc)
  {

    mulv _rest_tgl = NG;

    if ( !GL[fc] ) _rest_tgl = TGL_max[fc];
    if ( TGL[fc] ) _rest_tgl = TGL_max[fc] - TGL_timer[fc];

    ccol2spat->tgl_tijd = _rest_tgl;

    return(TRUE);

  }

  bool proc_schrijf_minimum_tijd_tot_realisatie( struct _CCOL2SPAT *ccol2spat, count fc , mulv ttr)
  {

    ccol2spat->min_tijd_tot_realisatie = ttr;

    if ( G[fc] ) {
      ccol2spat->min_tijd_tot_realisatie = NG;
    }

    return(TRUE);

  }


  bool  proc_schrijf_tijd_tot_start_groen( struct _CCOL2SPAT *ccol2spat, count fc , mulv t2sg)
  {

    ccol2spat->tijd_tot_einde_rood = t2sg;

    if ( SG[fc] ) {
      ccol2spat->tijd_tot_einde_rood = NG;
    }

    return(TRUE);

  }

  bool  proc_schrijf_tijd_tot_einde_groen(struct _CCOL2SPAT *ccol2spat, count fc, mulv t2eg)
  {

    ccol2spat->tijd_tot_einde_groen = t2eg;

    if ( SGL[fc] ) {
      ccol2spat->tijd_tot_einde_groen = NG;
    }

    return(TRUE);

  }


  bool  proc_schrijf_max_tijd_rood(struct _CCOL2SPAT *ccol2spat, count fc, mulv t2sgm)
  {

    ccol2spat->max_rood_tijd = t2sgm;

    if ( SG[fc] ) {
      ccol2spat->max_rood_tijd = NG;
    }

    return(TRUE);

  }


  bool  proc_schrijf_max_tijd_groen(struct _CCOL2SPAT *ccol2spat, count fc, mulv t2egm)
  {

    ccol2spat->max_groen_tijd = t2egm;

    if ( SGL[fc] ) {
      ccol2spat->max_groen_tijd = NG;
    }

    return(TRUE);

  }


