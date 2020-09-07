/* ======================================================================================================== */
/* PROCEDURES TBV C-ITS FUNCTIONALITEIT                                                                     */
/* ======================================================================================================== */

/* Ontwerp	: Marcel Fick en Patrick Huijskes                                                           */
/* Versie       : 2.00                                                                                      */
/* Datum        : 1 september 2018                                                                          */

/* -------------------------------------------------------------------------------------------------------- */
/* Include files                                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
 #include "pro_fik.h"
 #include "ris_func_traffick.h"

/* -------------------------------------------------------------------------------------------------------- */
/* Functie controleer of ObjectID aanwezig in gebied                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  int vtg_in_gebied(
    rif_string ObjectID,
    mulv       aantal)
  {
    int i,j;
    int volgnummer = NG;
    
    for (i = 0; i < aantal; i++) {
		if (ITSSTATION_AP_corr[BUFFER_pointer[i]].id[0] != '\0') {
			for (j = 0; j < RIS_TOVER_MAX; j++) {
          if (strcmp(ObjectID, ITSSTATION_AP_corr[BUFFER_pointer[i]].id) == 0) {
            volgnummer = BUFFER_pointer[i];
            break;
          }
          if (volgnummer != NG) break;
        }
      }
      if (volgnummer != NG) break;
    }
    return volgnummer;
  }
  

/* -------------------------------------------------------------------------------------------------------- */
/* Procedure aanvraag vanuit het RIS                                                                        */
/* -------------------------------------------------------------------------------------------------------- */
  void RIS_aanvraag(                    /* Fik170911                                                        */
    count fc,                           /* fasecyclus                                                       */
    mulv afstand,                       /* afstand tot stopstreep waarna aanvraag vanuit RIS                */
    mulv voertuig)                      /* voertuig type                                                    */
  {
    if ( PRM[afstand] > 0 )
      AVR_GEBIED[fc] = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, 0, PRM[afstand], BUFFER_pointer, RIJSTROOK_ALLE);
    else AVR_GEBIED[fc] = 0;
    if (R[fc] && !TRG[fc] && (AVR_GEBIED[fc] > 0)) set_A(fc);
  }
  
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
    mulv voertuig)                      /* voertuig type                                                    */
  {

    bool _tvag1 = TRUE;  /* @@ MS */

    if ( tvag1 > NG ) {  /* @@ MS */
      _tvag1 = RT[tvag1] = SG[fc];
      _tvag1 |= T[tvag1];
    }
    
    VG1_GEBIED[fc] = VG2_GEBIED[fc] = VG3_GEBIED[fc] = VG4_GEBIED[fc] = 0;
    VG3_GEBIED_OPEN[fc] |= WG[fc];
    
    if ( (vag1b > NG) && (vag1o > NG) ) {  /* @@ MS */
      if (PRM[vag1b] > PRM[vag1o]) {
        VG1_GEBIED[fc] = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, PRM[vag1o], PRM[vag1b], BUFFER_pointer, 255);
      }
    }
    
    if ( (vag2b > NG) && (vag2o > NG) ) {  /* @@ MS */
/*      if ( voertuig & RIS_VOETGANGER ) {  @@ MS  niet bekend hoe dit in RIS-dat wordt aangeleverd  
        if (PRM[vag2b] < PRM[vag2o]) {
          VG2_GEBIED[fc] = bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, -PRM[vag2b], -PRM[vag2o], BUFFER_pointer, 255);
        }
      }
      else { */
        if (PRM[vag2b] > PRM[vag2o]) {
          VG2_GEBIED[fc] = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, PRM[vag2o], PRM[vag2b], BUFFER_pointer, 255);
        }
/*      } */
    }
    
    if ( (vag3b > NG) && (vag3o > NG) ) {  /* @@ MS */
/*      if ( voertuig & RIS_VOETGANGER ) {  @@ MS  niet bekend hoe dit in RIS-dat wordt aangeleverd  
        if ( PRM[vag3b] < PRM[vag3o] ) {
          VG3_GEBIED[fc] = bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, -PRM[vag3b], -PRM[vag3o], BUFFER_pointer, 255);
        }
      }
      else { */
        if ( PRM[vag3b] > PRM[vag3o] ) {
          VG3_GEBIED[fc] = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, PRM[vag3o], PRM[vag3b], BUFFER_pointer, 255);
        }
/*      } */
    }
    if (VG[fc] && (VG3_GEBIED[fc] == 0)) VG3_GEBIED_OPEN[fc] = FALSE;

    if ( (vag4b > NG) && (vag4o > NG) ) {  /* @@ MS */
      if (PRM[vag4b] > PRM[vag4o]) {
        VG4_GEBIED[fc] = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig, PRM[vag4o], PRM[vag4b], BUFFER_pointer, RIJSTROOK_ALLE);
      }
    }
    
    if ((VG1_GEBIED[fc] > 0) && G[fc] && _tvag1     && !EVG[fc]) MK[fc] |= BIT0;  /* @@ MS _tvag1 */
    if ((VG2_GEBIED[fc] > 0)                        && !EVG[fc]) MK[fc] |= BIT1;
    if ((VG3_GEBIED[fc] > 0) && VG3_GEBIED_OPEN[fc] && !EVG[fc]) MK[fc] |= BIT2;
    if ((VG4_GEBIED[fc] > 0)                        && !EVG[fc]) MK[fc] |= BIT2;

  }
  
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
    mulv voertuig_type)                 /* voertuigtype                                                     */
  {
    count i , j;
    mulv aantal_actueel;
    mulv aantal_nieuw;
    mulv begin_afstand;
    mulv eind_afstand = PRM[afstand];
    bool nieuw;
    
    begin_afstand = eind_afstand - 6;
    if (begin_afstand < 0)
    {
      eind_afstand -= begin_afstand;
      begin_afstand = 0;
    }
    
    /* actuele aantal voertuigen in gebied, kunnen er meerdere zijn eventueel over meerdere rijstroken      */
    aantal_actueel = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig_type, begin_afstand, eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
    if (aantal_actueel > RIS_MEET_MAX) aantal_actueel = RIS_MEET_MAX;
    
    /* bepaal aantal nieuwe voertuigen door huidige voertuigen te vergelijken met vorige voertuigen         */
    aantal_nieuw = 0;
    for (i = 0; i < aantal_actueel; i++)
    {
		if (ITSSTATION_AP_corr[BUFFER_pointer[i]].id[0] != '\0') {
        nieuw = TRUE;	/* HVRT 2017 */
			for (j = 0; j < RIS_MEET_MAX; j++) {
				if (strcmp(pel_meet_voertuig[fc][j], ITSSTATION_AP_corr[BUFFER_pointer[i]].id) == 0) {
            nieuw = FALSE;
          }
          if (!nieuw || pel_meet_voertuig[fc][j][0] == '\0') break;
        }
        if (nieuw) {
          aantal_nieuw++;
        }
      }
    }
    
    /* copieer actuele voertuigen naar peloton buffer                                                       */
    for (i = 0; i < aantal_actueel; i++)
    {
      sprintf(pel_meet_voertuig[fc][i], ITSSTATION_AP_corr[BUFFER_pointer[i]].id);
    }
    for (i = aantal_actueel; i < RIS_MEET_MAX; i++)
    {
      pel_meet_voertuig[fc][i][0] = '\0';
    }
    
    MM[meetpunt] += aantal_nieuw;
    if (MM[meetpunt] > 10000) MM[meetpunt] = 10000;
    
    AT[meetperiode] = FALSE;
    RT[meetperiode] = RT[grenshiaat] = !T[meetperiode] && !T[grenshiaat] && (PRM[voertuig_crit] > 0) && 
                      (aantal_nieuw > 0); 
    
    if ((aantal_nieuw > 0) && (RT[meetperiode] || T[meetperiode])) RT[grenshiaat] = TRUE;
    
    if (ET[grenshiaat] && !RT[grenshiaat])
    {
      AT[meetperiode] = TRUE;
      MM[meetpunt] = 0;
    }
    
    MM[ris_peloton] = 0;
    if (MM[meetpunt] >= PRM[voertuig_crit])
    {
      MM[meetpunt] = 0;
      AT[meetperiode] = TRUE;
      if (PRM[voertuig_crit] > 0) MM[ris_peloton] = 1;
    }
  }

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
    mulv voertuig_type)
  {
    mulv ondermax = 255;
    mulv HPEL;
    mulv UITMELD;
    mulv extra, _TEG;
    bool bep_TEG;
    bool pel_uitmelding;
    mulv pri_code;
    mulv instel_onm;
    bool _reset_pel;
    count i, j, k;
    mulv aantal_actueel;
    mulv begin_afstand;
    mulv eind_afstand = PRM[prmafstand];
    float min_snelheid = (float)((float)PRM[snelheid] / 3.6);
    
    begin_afstand = eind_afstand - 6;
    if (begin_afstand < 0)
    {
      eind_afstand -= begin_afstand;
      begin_afstand = 0;
    }

    _TEG = 0; /* @@ MS  i.v.m. warning */

    if ( prio != NG )
    {                                   /* controleer instellingen                                          */
      if (PRM[prio] > 3)
      {
        PRM[prio] = 0;
        CIF_PARM1WIJZAP = -2;
      }
    }
    if (PRM[prio] == 0) BPI[fc] |= BIT3;
    instel_onm = PRIM[fc];


    aantal_actueel = bep_voertuig_ingebied( fc, NG, NG, NG, NG, voertuig_type, PRM[prmvag3o], eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
    if ( aantal_actueel > RIS_PELBUFFER_MAX ) aantal_actueel = RIS_PELBUFFER_MAX;

    if ( MM[ris_peloton] && !(BPI[fc] & BIT3) && (!BLK_PRI_REA || !G[fc]) ) {
      /* Behandeling inmelding                                                                              */
      if ( !MM[awt1] ) {
        MM[awt1] = 1;

        /* Peloton is gemeten, overzetten naar RIS-peloton-ID buffer 0 @@ PH snelheid peloton */
        if ( RIS_peloton_ID[fc][0][0][0] == '\0' ) {	/* Het buffer is leeg */

          for ( j = 0; j < aantal_actueel; j++ ) {
            sprintf(RIS_peloton_ID[fc][0][j], ITSSTATION_AP_corr[BUFFER_pointer[j]].id);
          }
          for ( j = aantal_actueel; j < RIS_PELBUFFER_MAX; j++ ) {
            RIS_peloton_ID[fc][0][j][0] = '\0';
          }
        }

      }
      else {
        if ( !MM[awt2] ) {
          MM[awt2] = 1;

          /* Peloton is gemeten, overzetten naar RIS-peloton-ID buffer 1 @@ PH snelheid peloton */
          if ( RIS_peloton_ID[fc][1][0][0] == '\0' ) {	/* Het buffer is leeg */

            for ( j = 0; j < aantal_actueel; j++ ) {
              sprintf(RIS_peloton_ID[fc][1][j], ITSSTATION_AP_corr[BUFFER_pointer[j]].id);
            }
            for ( j = aantal_actueel; j < RIS_PELBUFFER_MAX; j++ ) {
              RIS_peloton_ID[fc][1][j][0] = '\0';
            }
          }

        }
        else {
          if ( !MM[awt3] ) {
            MM[awt3] = 1;

            /* Peloton is gemeten, overzetten naar RIS-peloton-ID buffer 2 @@ PH snelheid peloton */
            if ( RIS_peloton_ID[fc][2][0][0] == '\0' ) {	/* Het buffer is leeg */

              for ( j = 0; j < aantal_actueel; j++ ) {
                sprintf(RIS_peloton_ID[fc][2][j], ITSSTATION_AP_corr[BUFFER_pointer[j]].id);
              }
              for ( j = aantal_actueel; j < RIS_PELBUFFER_MAX; j++ ) {
                RIS_peloton_ID[fc][2][j][0] = '\0';
              }
            }

          }
        }
      }
    }
 
  /* Bijhouden inhoud buffer PH */
  aantal_actueel = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig_type, PRM[prmvag3o], eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
  if ( aantal_actueel > RIS_PELBUFFER_MAX ) aantal_actueel = RIS_PELBUFFER_MAX;

  for ( i = 0; i < RIS_PEL_AANTAL_MAX; i++ ) {
    j = 0;
    while ( (j < RIS_PELBUFFER_MAX) && (RIS_peloton_ID[fc][i][j][0] != '\0') ) {
      k = 0;
      while ( (k >= 0) && (k < aantal_actueel) && (strcmp(RIS_peloton_ID[fc][i][j], ITSSTATION_AP_corr[BUFFER_pointer[k]].id) != 0) ) {
        k++;
      }
      if ( k >= aantal_actueel ) {	/* Voertuig uit het buffer gereden, de strcmp is dus niet gelijk aan 0 geworden */
        k = 0;
        while ( (k >= 0) && (k < RIS_PELBUFFER_MAX) && (RIS_peloton_ID[fc][i][k][0] != '\0') ) {	/* Zoek tot leeg voertuig */
          k++;
        }
        if ( (k > 0) && (k < RIS_PELBUFFER_MAX) ) { /* k == 0 dan is het buffer al leeg */
          sprintf(RIS_peloton_ID[fc][i][j], RIS_peloton_ID[fc][i][k - 1]);
          RIS_peloton_ID[fc][i][k - 1][0] = '\0';
        }
      }

      if ( ITSSTATION_AP_corr[RIS_id_cam(RIS_peloton_ID[fc][i][j])].speed < min_snelheid ) {	/* Te langzaam rijdend */
        k = 0;
        while ( (k >= 0) && (k < RIS_PELBUFFER_MAX) && (RIS_peloton_ID[fc][i][k][0] != '\0') ) {
          k++;
        }
        if ( (k > 0) && (k < RIS_PELBUFFER_MAX) ) { /* k == 0 dan is het buffer al leeg */
          sprintf(RIS_peloton_ID[fc][i][j], RIS_peloton_ID[fc][i][k - 1]);
          RIS_peloton_ID[fc][i][k - 1][0] = '\0';
        }
      }
      j++;
    }
  }

    if (TS)
    {                                   /* Bewerk buffer                                                    */
      if (MM[awt1] && (MM[awt1] < 1000)) MM[awt1]++;
      if (MM[awt2] && (MM[awt2] < 1000)) MM[awt2]++;
      if (MM[awt3] && (MM[awt3] < 1000)) MM[awt3]++;
    }
    
    pel_uitmelding = FALSE;
    if (((MM[awt1] > ((T_max[inmv] / 10) + (T_max[afst] / 10))) || (RIS_peloton_ID[fc][0][0][0] == '\0') && (MM[awt1] > 0)) && G[fc] && !SG[fc] && !FG[fc])
    {
      MM[awt1] = MM[awt2];
      MM[awt2] = MM[awt3];
      MM[awt3] = FALSE;

    /* Leegschuiven RIS_peloton_ID buffers */
    for (i = 0; i < RIS_PELBUFFER_MAX; i++) sprintf(RIS_peloton_ID[fc][0][i], RIS_peloton_ID[fc][1][i]);
    for (i = 0; i < RIS_PELBUFFER_MAX; i++) sprintf(RIS_peloton_ID[fc][1][i], RIS_peloton_ID[fc][2][i]);
    for (i = 0; i < RIS_PELBUFFER_MAX; i++) RIS_peloton_ID[fc][2][i][0] = '\0';

      pel_uitmelding = TRUE;

    }
    
    if (!G[fc]) KAPC[fc] &= (~BIT3);
    pri_code = 0;                       /* geen ingreep                                                     */
	if (MM[awt1]) {
      if (PRM[prio] == 1) pri_code = 1; /* aanhouden groen                                                  */
      if (PRM[prio] == 2) pri_code = 2; /* prio. zonder afbreken                                            */
      if (PRM[prio] == 3) pri_code = 3; /* prio. met    afbreken                                            */
    }
    
#ifdef DVM_AANWEZIG
    
    if (pri_code > MAX_DVM_PK[fc]) pri_code = MAX_DVM_PK[fc];
    
#endif                                  /* ifdef -> DVM_AANWEZIG                                            */
    
    if (geen_pel && (pri_code > 1)) pri_code = 1; /* geen prioriteitsrealisatie                             */
    if (G[fc] && (pri_code > 0)) pri_code = 3; /* per definitie                                             */
    if (pri_code == 3) KAPC[fc] |= BIT3;
    
    /* Als bpi hier al waar dan lopende prioriteit afbreken !                                               */
	if (BL[fc] || (BPI[fc] & BIT3) || (_FIL[fc] & BIT0)) {
      MM[awt1] = MM[awt2] = MM[awt3] = 0;

      for (i = 0; i < RIS_PEL_AANTAL_MAX; i++) {
        for (j = 0; j < RIS_PELBUFFER_MAX; j++) {
          RIS_peloton_ID[fc][i][j][0] = '\0';
        }
      }

    }
    
    /* Altijd aanvraag na afloop inmeldvertraging, tenzij prioriteitscode 0                                 */
    _reset_pel = (A[fc] & BIT8) && !(A[fc] & BIT1) && !(A[fc] & BIT3) && !(A[fc] & BIT6);
	if (R[fc] && !TRG[fc] && (MM[awt1] > (T_max[inmv] / 10)) && (HPRI[fc] == 0) && !A[fc] && (pri_code > 0)) {
      A[fc] |= BIT8;
    }
	if (R[fc] && !_GRN[fc] && ((MM[awt1] == 0) || BLK_PRI_REA || (HPRI[fc] > 0) || (pri_code == 0))) {
      A[fc] &= (~BIT8);
    }
	if (_reset_pel && !(A[fc] & BIT8)) {
      A[fc] = FALSE;
    }
    
    RT[tbl] = G[fc] && (SGD[fc] & BIT5);
    AT[tbl] = T[tbl] && R[fc] && !TRG[fc] && A[fc] && !(SGD[fc] & BIT4) && !(HKII[fc] & BIT4);
    
    if (!G[fc] && (T[tbl] || BLK_PRI_REA || (pri_code <= 1))) BPI[fc] |= BIT3;
    if (G[fc] && BLK_PRI_REA && (!(SGD[fc] & BIT5) || pel_uitmelding)) BPI[fc] |= BIT3;
    if ((HPRI[fc] > 0) || BLWT[fc] || FIXATIE) BPI[fc] |= BIT3;
    
    if (!(BPI[fc] & BIT3) && (MM[awt1] > (T_max[inmv] / 10))) RISPEL1[fc] = MM[awt1] - (T_max[inmv] / 10);
    if (RISPEL1[fc] < 255 - instel_onm) ondermax = RISPEL1[fc] + instel_onm;
    
    if (RISPEL1[fc] >= (T_max[afkv] / 10)) KVRPK[fc] = 0;
    else KVRPK[fc] = (T_max[afkv] / 10) - RISPEL1[fc];
    
    bep_TEG = FALSE;
	if (RISPEL1[fc] && (!G[fc] || G[fc] && (KGOM[fc] < ondermax))) {
      if (!G[fc]) _TEG = (T_max[afst] + T_max[nmax]) / 10;
      YM[fc] |= BIT4;
      RISPEL[fc] |= BIT5;
      bep_TEG |= TRUE;
    }
    else { RISPEL[fc] &= (~BIT5); if (!G[fc]) KAPC[fc] &= (~BIT3); }
    
	if (G[fc] && (RISPEL[fc] & BIT5) && (SGD[fc] & BIT5)) {
      A[fc] &= (~BIT2);
      RISPEL[fc] |= BIT6;
      RISPEL2[fc] = T_max[nmax] / 10;
    }
    
	if (G[fc] && !(RISPEL[fc] & BIT5) && (RISPEL[fc] & BIT6)) {
      if (TS && (RISPEL2[fc] > 0)) RISPEL2[fc]--;
      if (!MK[fc] || (_FIL[fc] & BIT0) || (HPRI[fc] > 0)) RISPEL2[fc] = 0;
		if (RISPEL2[fc] > 0) {
        YM[fc] |= BIT4;
        bep_TEG |= TRUE;
      }
		else {
        RISPEL[fc] &= (~BIT6);
      }
    }
    
    if (G[fc] && (RISPEL[fc] & BIT5) && bep_TEG)
    {                                   /* bepaal TEG[] bij aktieve RIS peloton ingreep                     */
      
      UITMELD = (T_max[afst] + T_max[nmax]) / 10;
      
      if (G[fc] && (!MG[fc] || MK[fc]) && (KG[fc] < RISPEL1[fc]))
        extra = RISPEL1[fc] - KG[fc];
      else extra = FALSE;
      UITMELD += extra;
      
      if (UITMELD >= RISPEL1[fc]) _TEG = UITMELD - RISPEL1[fc]; /* 1e peloton                               */
      else _TEG = FALSE;
      
      if (MM[awt2] > (T_max[inmv] / 10)) HPEL = MM[awt2] - (T_max[inmv] / 10); /* 2e peloton                */
      else HPEL = FALSE;
      
      UITMELD = (T_max[afst] + T_max[nmax]) / 10;
      if (G[fc] && (!MG[fc] || MK[fc]) && (KG[fc] < HPEL))
        extra = HPEL - KG[fc];
      else extra = FALSE;
      UITMELD += extra;
      
      if (HPEL < 255 - instel_onm) ondermax = HPEL + instel_onm;
      if (HPEL && (KGOM[fc] < ondermax) && (UITMELD >= HPEL)) _TEG = UITMELD - HPEL;
      
      if (MM[awt3] > (T_max[inmv] / 10)) HPEL = MM[awt3] - (T_max[inmv] / 10); /* 3e peloton                */
      else HPEL = FALSE;
      
      UITMELD = (T_max[afst] + T_max[nmax]) / 10;
      if (G[fc] && (!MG[fc] || MK[fc]) && (KG[fc] < HPEL))
        extra = HPEL - KG[fc];
      else extra = FALSE;
      UITMELD += extra;
      
      if (HPEL < 255 - instel_onm) ondermax = HPEL + instel_onm;
      if (HPEL && (KGOM[fc] < ondermax) && (UITMELD >= HPEL)) _TEG = UITMELD - HPEL;
    }
    
    if (G[fc] && !(RISPEL[fc] & BIT5) && (RISPEL[fc] & BIT6))
    {
      _TEG = RISPEL2[fc];               /* bepaal TEG[] na aktieve peloton ingreep                          */
      RISPEL[fc] |= BIT5;               /* prioriteitsbit tbv regelstructuur                                */
    }
    
    if (bep_TEG && ((RISPEL[fc] & BIT5) || (RISPEL[fc] & BIT6)))
    {
      if ((POV[fc] & BIT5) || (PEL[fc] & BIT5))
      {
        if (_TEG > TEG[fc]) TEG[fc] = _TEG; /* bepaal resultante TEG[] afhankelijk van BUS of PEL aanwezig  */
      }
      else
      {
        TEG[fc] = _TEG;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS tovergroen ingreep                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_tover(                  /* Fik171001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count bewaak,                       /* T bewakingstijd                                                  */
    count afstand,                      /* PRM afstand tot stopstreep                                       */
    count min_v,                        /* PRM minimum snelheid                                             */
    count max_v)                        /* PRM maximum snelheid                                             */
  {
    bool tov_aanwezig  = FALSE;
    count i, j;
    mulv begin_afstand, restant;
    mulv eind_afstand  = PRM[afstand];
    mulv voertuig_type = RIS_VRACHTWAGEN;
    mulv tov_TEG       = 0;
    mulv aantal_actueel= 0;
    
    float min_speed = (float)(PRM[min_v]/3.6);
    float max_speed = (float)(PRM[max_v]/3.6);
    
    bool nieuw      = FALSE;
    bool geplaatst  = FALSE;
    bool kijk_naar_groen_duur = FALSE;
    
    begin_afstand = eind_afstand - 6;
    if (begin_afstand < 0)
    {
      eind_afstand -= begin_afstand;
      begin_afstand = 0;
    }
    
    tov_eerste[fc] |= SG[fc];
    if (MG[fc] && (SGD[fc]&BIT5) && (TOV[fc]&BIT5)) tov_eerste[fc] = FALSE;
    
    /* actuele aantal voertuigen in gebied, kunnen er meerdere zijn eventueel over meerdere rijstroken      */
    if (!(BPI[fc]&BIT5) && !BLK_PRI_REA && (HPRI[fc] == 0) && !BLWT[fc] && !FIXATIE &&
        (!G[fc] || G[fc] && (tov_eerste[fc] || (KGOM[fc] < PRIM[fc])))) {
      aantal_actueel = (mulv) bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig_type, begin_afstand, eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
      if (aantal_actueel > RIS_MEET_MAX) aantal_actueel = RIS_MEET_MAX;
    } else {
      aantal_actueel = 0;
    }
    
    for (i = 0; i < aantal_actueel; i++)
    {
      if (ITSSTATION_AP_corr[BUFFER_pointer[i]].id[0] != '\0')
      {
        nieuw = TRUE;
        for (j = 0; j < RIS_TOVER_MAX; j++)
        {
          if (strcmp(TOVER[fc][j].voertuig_id, ITSSTATION_AP_corr[BUFFER_pointer[i]].id) == 0) nieuw = FALSE;
          if (((ITSSTATION_AP_corr[BUFFER_pointer[i]].speed) < min_speed) ||
              ((ITSSTATION_AP_corr[BUFFER_pointer[i]].speed) > max_speed)) nieuw = FALSE;
          if (!nieuw) break;
        }
        if (nieuw)
        {
          geplaatst = FALSE;
          for (j = 0; j < RIS_TOVER_MAX; j++)
          {
            if (TOVER[fc][j].voertuig_id[0] == '\0')
            {
              sprintf(TOVER[fc][j].voertuig_id, ITSSTATION_AP_corr[BUFFER_pointer[i]].id);
              TOVER[fc][j].duur_aanwezig  = 0;
              geplaatst = TRUE;
            }
            if (geplaatst) break;
          }
        }
      }
    }
    
    /* werk aanwezigheidstijd bij                                                                           */
    if (TE)
    {
      for (i = 0; i < RIS_TOVER_MAX; i++)
      {
        if (TOVER[fc][i].voertuig_id[0] != '\0')
        {
          if (TOVER[fc][i].duur_aanwezig < 10000) TOVER[fc][i].duur_aanwezig += TE;
        }
      }
    }
    
    /* bewaak aanwezige voertuigen                                                                          */
    kijk_naar_groen_duur = TRUE;
    if (G[fc] && MG[fc] && !MK[fc]) kijk_naar_groen_duur = FALSE;
    
    aantal_actueel = bep_voertuig_ingebied(fc, NG, NG, NG, NG, voertuig_type, 0, eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
    
    for (i = 0; i < RIS_TOVER_MAX; i++)
    {
      if (TOVER[fc][i].voertuig_id[0] != '\0')
      {
        j = (vtg_in_gebied(TOVER[fc][i].voertuig_id, aantal_actueel));
        if (j >= 0)                     /* j >= 0, dan is het voertuig nog bekend in het RIS                */
        {
          if ((TOVER[fc][i].duur_aanwezig < 0) || (TOVER[fc][i].duur_aanwezig >= 10000) ||
              (TOVER[fc][i].duur_aanwezig >= T_max[bewaak]) && ((KG[fc] >= T_max[bewaak]/10) || !kijk_naar_groen_duur))
          {
            TOVER[fc][i].voertuig_id[0] = '\0';
            TOVER[fc][i].duur_aanwezig  = 0;
          }
          else                          /* bepaal maximale groenduur tot stopstreep passage                 */
          {
            if (TOVER[fc][i].duur_aanwezig > T_max[bewaak]) restant = 0;
            else                                            restant = T_max[bewaak] - TOVER[fc][i].duur_aanwezig;
            if (restant/10 > tov_TEG) tov_TEG = restant/10;
          }
        }
        else                            /* voertuig is niet meer bekend in het RIS en kan dus gewist worden */
        {
          TOVER[fc][i].voertuig_id[0] = '\0';
          TOVER[fc][i].duur_aanwezig  = 0;
        }
        if (TOVER[fc][i].voertuig_id[0] != '\0') tov_aanwezig = TRUE;
      }
    }
    
    if (G[fc] && tov_aanwezig)
    {
      YM[fc]  |= BIT4;
      TOV[fc] |= BIT5;
    }
    else { TOV[fc] &= (~BIT5); }
    
    if (TOV[fc]&BIT5)
    {
      if ((POV[fc]&BIT5) || (PEL[fc]&BIT5) || (RISPEL[fc]&BIT5))
      {
        if (tov_TEG > TEG[fc]) TEG[fc] = tov_TEG; /* bepaal TEG[] afhankelijk van BUS of PEL aanwezig       */
      }
      else
      {
        TEG[fc] = tov_TEG;
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS bus ingreep                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
void proc_RIS_bus_ingreep(            /* Fik171106                                                          */
  count fc,                           /* fasecyclus                                                         */
  count bewaak,                       /* T bewakingstijd                                                    */
  count tblokkeer,                    /* T blokkeringstijd                                                  */
  count afstand,                      /* PRM afstand tot stopstreep                                         */
  count lyn1,                         /* PRM buslijn 1                                                      */
  count lynx)                         /* PRM buslijn x                                                      */
{
  count i, j, l, k;
  mulv eind_afstand  = PRM[afstand];
  mulv voertuig_type = RIS_BUS | RIS_TRAM;
  mulv aantal_actueel= 0;
  int stiptheid_ccol= 0;
  mulv srm = NG;

  bool overig_OK  = FALSE;
  bool nieuw      = FALSE;
  bool geplaatst  = FALSE;
  bool kijk_naar_groen_duur = FALSE;
  bool hulp       = FALSE;

  bool blokkeer = FALSE;  
  mulv instel_onm = PRIM[fc];

	count fc2    = NG;
	count fc3    = NG;
	count fc4    = NG;
	count fc5    = NG;

  KWCOV[fc]   = FALSE;
  OV_ris[fc]  = 0;
  OV_aanw[fc] = 0;
  OV_stipt[fc]= 0;

  j = 2;
  /* bepaal voertuigen op dezelfde arm als fc */
  for (i = 0; i < FC_MAX; i++) {
    if ((ARM[fc] == ARM[i]) && (fc != i)) {
      if (j == 2) fc2 = i;
      if (j == 3) fc3 = i;
      if (j == 4) fc4 = i;
      if (j == 5) fc5 = i;
      j++;
    }
  }

  if (tblokkeer != NG) {
    blokkeer = !G[fc] && T[tblokkeer];
	}
	else {
    blokkeer = FALSE;
  }

  pov_eerste[fc] |= SG[fc];
  if (MG[fc] && (SGD[fc]&BIT5) && (POV[fc]&BIT5)) pov_eerste[fc] = FALSE;
    
  /* actuele aantal voertuigen in gebied, kunnen er meerdere zijn eventueel over meerdere rijstroken      */
  if (!(BPI[fc]&BIT0) && !BLK_PRI_REA && (HPRI[fc] == 0) && !BLWT[fc] && !FIXATIE &&
      (!G[fc] || G[fc] && (pov_eerste[fc] || (KGOM[fc] < PRIM[fc])))) {
    aantal_actueel = bep_voertuig_ingebied(fc, fc2, fc3, fc4, fc5, voertuig_type, 0, eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
    if (aantal_actueel > RIS_MEET_MAX) aantal_actueel = RIS_MEET_MAX;
	}
	else {
    aantal_actueel = 0;
  }
    
    for (i = 0; i < aantal_actueel; i++) {
      if (ITSSTATION_AP_corr[BUFFER_pointer[i]].id[0] != '\0') {
        nieuw = TRUE;
        for (j = 0; j < RIS_OV_MAX; j++) {
          if (strcmp(RIS_OV[fc][j].voertuig_id, ITSSTATION_AP_corr[BUFFER_pointer[i]].id) == 0) nieuw = FALSE;
          if (nieuw) {
            srm = RIS_itsstation_srm(ITSSTATION_AP_corr[BUFFER_pointer[i]].id);
              if (srm >= 0) {
              /* PRIOREQUEST_AP[srm].subrole == VSR_BUS vooralsnog niet meegenomen, moet ook gelden voor overig openbaar vervoer */
              overig_OK = FALSE;
              if ((lyn1 != NG) && (lynx != NG)) {
                for (l = lyn1; l < lynx; l++) {
                  if ((PRM[l] == 255) || (PRM[l] == ITSSTATION_AP_corr[BUFFER_pointer[i]].roleAttributes.publicTransport.lineNr)) {
                    if (((PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_REQUEST) || (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_UPDATE)) &&
                         (PRIOREQUEST_AP[srm].role == RIF_VEHICLEROLE_PUBLICTRANSPORT) && (strcmp(PRIOREQUEST_AP[srm].signalGroup, FC_code[fc]) == 0)) {
                      overig_OK = TRUE;
                      stiptheid_ccol = PRIOREQUEST_AP[srm].punctuality;
                      if ((stiptheid_ccol < -3600) || (stiptheid_ccol > 3600)) stiptheid_ccol = 0;
                    }
                  }
                  if (overig_OK) break;
                }
              } else {                    /* lijnnummer selectie op NG dan geen controle op lijnnummers ! */
                if (((PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_REQUEST) || (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_UPDATE)) &&
                     (PRIOREQUEST_AP[srm].role == RIF_VEHICLEROLE_PUBLICTRANSPORT) && (strcmp(PRIOREQUEST_AP[srm].signalGroup, FC_code[fc]) == 0)) {
                  overig_OK = TRUE;
                  stiptheid_ccol = PRIOREQUEST_AP[srm].punctuality;
                  if ((stiptheid_ccol < -3600) || (stiptheid_ccol > 3600)) stiptheid_ccol = 0;
                }
              }
              if (!overig_OK) {
		nieuw = FALSE;
	      }
	    } 
	    else {
              nieuw = FALSE;
            }
          }
          if (!nieuw) break;
        }
        if (nieuw) {
        KWCOV[fc]   |= BIT0;
        VLOG_OV[fc] |= BIT1;
        geplaatst = FALSE;

          for (j = 0; j < RIS_OV_MAX; j++) {
            if (RIS_OV[fc][j].voertuig_id[0] == '\0') {
              sprintf(RIS_OV[fc][j].voertuig_id, ITSSTATION_AP_corr[BUFFER_pointer[i]].id);
							RIS_OV[fc][j].stiptheid_ov  = (mulv) stiptheid_ccol;
              RIS_OV[fc][j].duur_aanwezig = 0;
              RIS_OV[fc][j].prio_status   = 0;
              geplaatst = TRUE;
            }
            if (geplaatst) break;
          }
        }
      }
    }
    
    /* werk aanwezigheidstijd bij                                                                           */
    if (TE) {
      for (i = 0; i < RIS_OV_MAX; i++) {
        if (RIS_OV[fc][i].voertuig_id[0] != '\0') {
          if (RIS_OV[fc][i].duur_aanwezig < 10000) RIS_OV[fc][i].duur_aanwezig += TE;
        }
      }
    }
    
    /* bewaak aanwezige voertuigen                                                                          */
    kijk_naar_groen_duur = TRUE;
    if (G[fc] && MG[fc] && !MK[fc]) kijk_naar_groen_duur = FALSE;
    
    aantal_actueel = bep_voertuig_ingebied(fc, fc2, fc3, fc4, fc5, voertuig_type, 0, eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
    
    for ( i = 0; i < RIS_OV_MAX; i++ ) {
      if ( RIS_OV[fc][i].voertuig_id[0] != '\0' ) {
        j = RIS_id_cam(RIS_OV[fc][i].voertuig_id);
        hulp = FALSE;	/* Nog binnen het traject CAM ? */
        for ( k = 0; k < aantal_actueel; k++ ) {
          hulp |= (BUFFER_pointer[k] == j);
        }
        if ( !hulp ) j = NG;

        srm = RIS_itsstation_srm(RIS_OV[fc][i].voertuig_id);
        if ( (j >= 0) && (srm >= 0) &&
            ((PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_REQUEST) || (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_UPDATE)) &&
            (PRIOREQUEST_AP[srm].role == RIF_VEHICLEROLE_PUBLICTRANSPORT) && (strcmp(PRIOREQUEST_AP[srm].signalGroup, FC_code[fc]) == 0) ) {
              /* j >= 0, dan is het voertuig nog bekend in het RIS                */
              /* srm >= 0, dan is het SRM bericht nog aanwezig in het RIS         */

			if (PRIOREQUEST_AP_corr[srm].eta < 1000) {	/* 20180405 */
				RIS_OV[fc][i].prio_status = 0;	/* Alle bits resetten */
			}

          if ( (RIS_OV[fc][i].duur_aanwezig < 0) || (RIS_OV[fc][i].duur_aanwezig >= 10000) ||
              (RIS_OV[fc][i].duur_aanwezig >= T_max[bewaak]) && ((KG[fc] >= T_max[bewaak] / 10) || !kijk_naar_groen_duur) ) {
            RIS_OV[fc][i].duur_aanwezig = 10000;
            RIS_OV[fc][i].prio_status = 0;
            if ( !(RIS_OV[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE)) ) {
              RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
              RIS_OV[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
              RIS_OV[fc][i].prio_status |= 0xff;	 /* Alle bits opzetten, er mogen geen overige berichten worden verzonden */
            }
          }
          else {
            if ( blokkeer ) {
              if ( !(RIS_OV[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_RESERVICELOCKED)) ) {
                RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_RESERVICELOCKED);
                RIS_OV[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_RESERVICELOCKED);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_GRANTED);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
              }
            }
            else { /* blokkeringstijd is afgelopen en uitmeldbewaking is nog niet verstreken */
              if ( G[fc] && (SGD[fc] & BIT5) && (POV[fc] & BIT5) ) {
                if ( (KGOM[fc] + 1 - (RIS_OV[fc][i].duur_aanwezig / 10)) < instel_onm ) {
                  if ( !(RIS_OV[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_GRANTED)) ) {
                    RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_GRANTED);
                    RIS_OV[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_GRANTED);
                    RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                  }
                }
                else {
                  if ( !(RIS_OV[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE)) ) {
                    RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
                    RIS_OV[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
                    RIS_OV[fc][i].prio_status |= 0xff;	 /* Alle bits opzetten, er mogen geen overige berichten worden verzonden */
                  }
                }
              }
            }
          }
        }
        else {
          if ( (srm == NG) || /* CAM bericht niet testen, kan kortstonding weg zijn :( */
              (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_NONE) ||
              (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_CANCELLATION) ) {
            RIS_OV[fc][i].voertuig_id[0] = '\0';
            RIS_OV[fc][i].stiptheid_ov = 0;
            RIS_OV[fc][i].duur_aanwezig = 0;
            RIS_OV[fc][i].prio_status = 0;
          }
          else {
				/* CAM is afgekeurd of niet meer aanwezig, maar nog wel een SRM actief */
				/* Bus kent prioriteit, CAM weggevallen, geen reden tot berichtenverkeer via SSM */
				/*
            if ( blokkeer ) {
              if ( !(RIS_OV[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_RESERVICELOCKED)) ) {
                RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_RESERVICELOCKED);
                RIS_OV[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_RESERVICELOCKED);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_GRANTED);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
              }
            }
            else {
              if ( !(RIS_OV[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC)) ) {
                RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                RIS_OV[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                RIS_OV[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_GRANTED);
              }
					}*/
          }
        }
      }
    }

    /* tbv koppeling met KAR functies */
    for ( i = 0; i < RIS_OV_MAX; i++ ) {
      if ( RIS_OV[fc][i].voertuig_id[0] != '\0' ) {
        j = RIS_id_cam(RIS_OV[fc][i].voertuig_id);
			srm = RIS_itsstation_srm(RIS_OV[fc][i].voertuig_id); /* HVRT20180611: ingreep werd ten onrechte op CAM vastgehouden */
			if ( (srm >= 0) && (RIS_OV[fc][i].duur_aanwezig < 10000) ) {
          OV_ris[fc] = 1;
          if ( RIS_OV[fc][i].duur_aanwezig > OV_aanw[fc] ) OV_aanw[fc] = RIS_OV[fc][i].duur_aanwezig;
          if ( RIS_OV[fc][i].stiptheid_ov  > OV_stipt[fc] ) OV_stipt[fc] = RIS_OV[fc][i].stiptheid_ov;
        }
      }
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS hulpdienst ingreep                                                                         */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_hulpdienst(             /* Fik171006                                                        */
    count fc,                           /* fasecyclus                                                       */
    count bewaak,                       /* T bewakingstijd                                                  */
    count afstand)                      /* PRM afstand tot stopstreep                                       */
  {
    count i, j, k;
    mulv aantal_actueel;
    mulv eind_afstand  = PRM[afstand];
    mulv voertuig_type = RIS_HULPDIENST | RIS_AUTO | RIS_MOTOR | RIS_VRACHTWAGEN; /* auto, motor en vrachtwagen puur voor de test TINT1 */
    mulv srm = NG;

    bool overig_OK  = FALSE;
    bool nieuw      = FALSE;
    bool geplaatst  = FALSE;
    bool kijk_naar_groen_duur = FALSE;
    bool hulp = FALSE;

    count fc2 = NG;
    count fc3 = NG;
    count fc4 = NG;
    count fc5 = NG;

    j = 2;
    /* bepaal voertuigen op dezelfde arm als fc */
    for (i = 0; i < FC_MAX; i++) {
      if ((ARM[fc] == ARM[i]) && (fc != i)) {
        if (j == 2) fc2 = i;
        if (j == 3) fc3 = i;
        if (j == 4) fc4 = i;
        if (j == 5) fc5 = i;
        j++;
      }
    }

    HD_ris[fc]  &= (~BIT0);             /* BIT0= aanwezig; BIT1= hulpwaarde tbv koppeling met KAR functies  */
    HD_aanw[fc] = 10000;
        
    /* actuele aantal voertuigen in gebied, kunnen er meerdere zijn eventueel over meerdere rijstroken      */
    aantal_actueel = bep_voertuig_ingebied(fc, fc2, fc3, fc4, fc5, voertuig_type, 0, eind_afstand, BUFFER_pointer, RIJSTROOK_ALLE);
    if (aantal_actueel > RIS_HLPD_MAX) aantal_actueel = RIS_HLPD_MAX;
    
    for (i = 0; i < aantal_actueel; i++) {
      if (ITSSTATION_AP_corr[BUFFER_pointer[i]].id[0] != '\0') {
        nieuw = TRUE;
        for (j = 0; j < RIS_HLPD_MAX; j++) {
          if (strcmp(HULP_DIENST[fc][j].voertuig_id, ITSSTATION_AP_corr[BUFFER_pointer[i]].id) == 0) nieuw = FALSE;
          if (nieuw) {
            srm = RIS_itsstation_srm(ITSSTATION_AP_corr[BUFFER_pointer[i]].id);
	      if ( srm >= 0 ) {
              overig_OK = FALSE;
              if (((PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_REQUEST) || (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_UPDATE)) &&
                  (PRIOREQUEST_AP[srm].role == RIF_VEHICLEROLE_EMERGENCY)  && (strcmp(PRIOREQUEST_AP[srm].signalGroup, FC_code[fc]) == 0) &&
                 /* (PRIOREQUEST_AP[srm].subrole == VSR_EMERGENCY) && vervallen PH 20180316 */
                  ITSSTATION_AP_corr[BUFFER_pointer[i]].roleAttributes.lightBarActivated && (ITSSTATION_AP_corr[BUFFER_pointer[i]].roleAttributes.sirenActivated || !SCH[schsirene])) {
                  overig_OK = TRUE;
              }
	      if (!overig_OK) {
		nieuw = FALSE;
  	      }
            } else {
              nieuw = FALSE;
            }
          }
          if (!nieuw) break;
        }
        if (nieuw) {
          geplaatst = FALSE;
          for (j = 0; j < RIS_HLPD_MAX; j++) {
            if (HULP_DIENST[fc][j].voertuig_id[0] == '\0') {
              sprintf(HULP_DIENST[fc][j].voertuig_id, ITSSTATION_AP_corr[BUFFER_pointer[i]].id);
              HULP_DIENST[fc][j].duur_aanwezig = 0;
              HULP_DIENST[fc][j].prio_status   = 0;
              geplaatst = TRUE;
            }
            if (geplaatst) break;
          }
        }
      }
    }
    
    /* bewaak aanwezige voertuigen                                                                          */
    kijk_naar_groen_duur = TRUE;
    if (G[fc] && MG[fc] && !MK[fc]) kijk_naar_groen_duur = FALSE;

    for ( i = 0; i < RIS_HLPD_MAX; i++ ) {  /*aantal_actueel niet gebruikt CAM kan weggevallen zijn !!! */
      if ( HULP_DIENST[fc][i].voertuig_id[0] != '\0' ) {
        j = RIS_id_cam(HULP_DIENST[fc][i].voertuig_id);
        hulp = FALSE;	/* Nog binnen het traject CAM ? */
        for ( k = 0; k < aantal_actueel; k++ ) {
          hulp |= (BUFFER_pointer[k] == j);
        }
        if ( !hulp ) j = NG;

        srm = RIS_itsstation_srm(HULP_DIENST[fc][i].voertuig_id);
        if ( (j >= 0) && (srm >= 0) &&
            ((PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_REQUEST) || (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_UPDATE)) &&
            (PRIOREQUEST_AP[srm].role == RIF_VEHICLEROLE_EMERGENCY) && (strcmp(PRIOREQUEST_AP[srm].signalGroup, FC_code[fc]) == 0) &&
                 /* (PRIOREQUEST_AP[srm].subrole == VSR_EMERGENCY) && vervallen PH 20180316 */
            ITSSTATION_AP_corr[BUFFER_pointer[i]].roleAttributes.lightBarActivated /* && (ITSSTATION_AP_corr[BUFFER_pointer[i]].roleAttributes.sirenActivated || !SCH[schsirene])*/ ) {
              /* j >= 0, dan is het voertuig nog bekend in het RIS                */
              /* srm >= 0, dan is het SRM bericht nog aanwezig in het RIS         */

			if (PRIOREQUEST_AP_corr[srm].eta < 1000) {	/* 20180405 */
				HULP_DIENST[fc][i].prio_status = 0;	/* Alle bits resetten */
			}

          if ( (HULP_DIENST[fc][i].duur_aanwezig < 0) || (HULP_DIENST[fc][i].duur_aanwezig >= 10000) ||
              (HULP_DIENST[fc][i].duur_aanwezig >= T_max[bewaak]) && ((KG[fc] >= T_max[bewaak] / 10) || !kijk_naar_groen_duur) ) {
            HULP_DIENST[fc][i].duur_aanwezig = 10000;
            if ( !(HULP_DIENST[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_MAXPRESENCE)) ) {
              RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_MAXPRESENCE);
              HULP_DIENST[fc][i].prio_status |= 0xff;	 /* Alle bits opzetten, er mogen geen overige berichten worden verzonden */
            }
          }
        }
        else {
          if ( (srm == NG) || /* CAM bericht niet testen, kan kortstonding weg zijn :( */
              (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_NONE) ||
              (PRIOREQUEST_AP[srm].requestType == RIF_PRIORITYREQUESTTYPE_CANCELLATION)/* ||
              !ITSSTATION_AP_corr[BUFFER_pointer[i]].roleAttributes.lightBarActivated */ ) { /* Niet testen op een CAM bericht !!! */
            /* Legen en doorschuiven buffer */
            for ( k = i; k < RIS_HLPD_MAX - 1; k++ ) {
              sprintf(HULP_DIENST[fc][k].voertuig_id, HULP_DIENST[fc][k + 1].voertuig_id);
              HULP_DIENST[fc][k].duur_aanwezig = HULP_DIENST[fc][k + 1].duur_aanwezig;
              HULP_DIENST[fc][k].prio_status = HULP_DIENST[fc][k + 1].prio_status;
            }
            HULP_DIENST[fc][RIS_HLPD_MAX - 1].voertuig_id[0] = '\0';
            HULP_DIENST[fc][RIS_HLPD_MAX - 1].duur_aanwezig = 0;
            HULP_DIENST[fc][RIS_HLPD_MAX - 1].prio_status = 0;
          }
          else {
            if ( !(HULP_DIENST[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC)) ) {
              RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
              HULP_DIENST[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
              HULP_DIENST[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_GRANTED); /* GRANTED wordt gereset als wordt teruggevallen */
            }
          }
        }
      }
    }

    /* werk aanwezigheidstijd bij                                                                           */
    for ( i = 0; i < RIS_HLPD_MAX; i++ ) {
      if ( HULP_DIENST[fc][i].voertuig_id[0] != '\0' ) {
        j = RIS_id_cam(HULP_DIENST[fc][i].voertuig_id);
        hulp = FALSE;
        for ( k = 0; k < aantal_actueel; k++ ) {
          hulp |= (BUFFER_pointer[k] == j);
        }
        if ( !hulp ) j = NG;	/* Geen CAM voertuig meer in het meetgebied */

        if ( HULP_DIENST[fc][i].duur_aanwezig < 10000 ) HULP_DIENST[fc][i].duur_aanwezig += TE;

        if ( (j >= 0) && (HULP_DIENST[fc][i].duur_aanwezig < 10000) ) {
          HD_ris[fc] |= BIT0;
          if ( HULP_DIENST[fc][i].duur_aanwezig < HD_aanw[fc] ) HD_aanw[fc] = HULP_DIENST[fc][i].duur_aanwezig;

          srm = RIS_itsstation_srm(HULP_DIENST[fc][i].voertuig_id);
          if ( srm >= 0 ) {
            if ( G[fc] && HLPD[fc][0] ) {
              if ( !(HULP_DIENST[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_GRANTED)) ) {
                RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_GRANTED);
                HULP_DIENST[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_GRANTED);
              }
            }
            else {
              if ( !(HULP_DIENST[fc][i].prio_status & (1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC)) && (HULP_DIENST[fc][i].duur_aanwezig >= 10) ) {
                RIS_ssm_bericht(PRIOREQUEST_AP[srm].id, RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                HULP_DIENST[fc][i].prio_status |= (1 << RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC);
                HULP_DIENST[fc][i].prio_status &= ~(1 << RIF_PRIORITIZATIONSTATE_GRANTED); /* GRANTED wordt gereset als wordt teruggevallen */
              }
            }
          }
        }
      }
    }
    if ( !(HD_ris[fc] & BIT0) ) HD_aanw[fc] = 0;    /* wis aanwezigheidstijd indien geen HLPD aanwezig */
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure RIS fiets prioriteit                                                                           */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_RIS_fts_prio(               /* Fik170922                                                        */
    count fc,                           /* fasecyclus                                                       */
    count min_fts,                      /* PRM minimum aantal fietsers                                      */
    count afstand,                      /* PRM afstand tot de stopstreep                                    */
    count ogwt,                         /* T min.wachttijd                                                  */
    count tbl,                          /* T blokkeringstijd                                                */
    count prio)                         /* PRM prio code                                                    */
  {
    count i,j;
    count kpvtg; /* @@ MS */
    bool kop_pri = FALSE;
    bool kop_mog = FALSE;
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
          kpvtg = kp_vtg_fts[i][j]; /* @@ MS */
          if (kpvtg != NG) {
            if (G[kpvtg] && (SGD[kpvtg]&BIT5)) kop_pri = TRUE;
          }
        }
      }
    }
    
    if (G[fc] && !SG[fc] && !FG[fc] || R[fc] && !A[fc]) RISFTS[fc] &= (~BIT0);
    if ((bep_voertuig_ingebied(fc, NG, NG, NG, NG, RIS_FIETS, 0, PRM[afstand], BUFFER_pointer, RIJSTROOK_ALLE)) >= PRM[min_fts]) {
      if (R[fc] && !TRG[fc] && A[fc] && (PRM[min_fts] > 0)) RISFTS[fc] |= BIT0;
    }
    
    if (!SG[fc] && !FG[fc]) RISFTS[fc] &= (~BIT5); /* reset fietsprioriteit                                 */
    /* op SG[] wordt richting als primair gemarkeerd                                                        */
    RT[tbl] = G[fc] && ((SGD[fc]&BIT5) || kop_pri);
    AT[tbl] = T[tbl] && R[fc] && !TRG[fc] && A[fc] && !(SGD[fc]&BIT4) && !(HKII[fc]&BIT4) && !somgk(fc);
    
    if (!G[fc] && ((HPRI[fc] > 0) || BLWT[fc] || T[tbl] || (pri_code == 0))) BPI[fc] |= BIT4;
    if (!G[fc] && !(SGD[fc]&BIT5) && (BLK_PRI_REA || (_dvm > 10))) BPI[fc] |= BIT4;
    
    if (R[fc] && A[fc] && (RISFTS[fc]&BIT0) && (TFB_timer[fc] > (T_max[ogwt]/10))) {
      if (!(BPI[fc]&BIT4) && !BL[fc]) RISFTS[fc] |= BIT5;
    }
    if (!G[fc]) KAPC[fc] &= (~BIT4);
    if (G[fc] && (pri_code > 0)) pri_code = 2; /* per definitie                                             */
    if (pri_code == 2) KAPC[fc] |= BIT4;
    if (!G[fc] && !(RISFTS[fc]&BIT5)) KAPC[fc] &= (~BIT4);
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure display RIS prioriteit - auto                                                                  */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_us_ris_mvt(                 /* Fik171001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count awt1,                         /* MM aanwezigheidstijd ris peloton                                 */
    count usrispel,                     /* US ris peloton ingreep                                           */
    count usristov)                     /* US ris tovergroen ingreep                                        */
  {
    if (usrispel != NG) {
      CIF_GUS[usrispel] = BEDRYF && (AKTPRG == 0) && (MM[awt1] > 0) && (SGD[fc]&BIT5) && (RISPEL[fc]&BIT5);
    }
    if (usristov != NG) {
      CIF_GUS[usristov] = BEDRYF && (AKTPRG == 0) && (SGD[fc]&BIT5) && (TOV[fc]&BIT5);
    }
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* Procedure display RIS prioriteit - fiets                                                                 */
/* -------------------------------------------------------------------------------------------------------- */
  void proc_us_ris_fts(                 /* Fik171001                                                        */
    count fc,                           /* fasecyclus                                                       */
    count usrisfts)                     /* US ris fiets prioriteit                                          */
  {
    if (usrisfts != NG) {
      CIF_GUS[usrisfts] = BEDRYF && (AKTPRG == 0) && (RISFTS[fc]&BIT0) && (G[fc] || KNIP);
    }
  }
  

