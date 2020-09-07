/* ======================================================================================================== */
/* Time2Red - Time2Green :  Versie Traffick / Noord Holland Generator                                       */
/*                                                                                                          */
/* Door:  Marcel Fick                                                                                       */
/* Versie:  0.1                                                                                             */
/* Datum: 1 juli 2017                                                                                       */
/* ======================================================================================================== */
/* Aangepast: M. Scherjon                                                                                   */
/* Versie:    2.0                                                                                           */
/* Datum:     17 maart 2018                                                                                 */
/* ======================================================================================================== */

/* ======================================================================================================== */
 #define NO_WIJZ_2018_03_19   /* uitsturing met 'oude' betrouwbaarheids variabele BT2SG[] en BT2EG[] */


/* include header files */
	#include "t2r_t2g.h"              /* algemene macro's, variablen, en functie declaraties                  */
	#include "t2r_t2g_traffick.h"     /* regelstruktuur specifieke macro's, Variablen, en functie declaraties */

/* Declaratie lokale functies t.b.v. 'traffick' regelstructuur                                              */
/* ===========================================================                                              */
	mulv teg_vas(count fc);
	void teg_shadow(count fc);
	void tegm_shadow(count fc); /* corrigieer T2EGM[] t.b.v. in de shaddow  */
	mulv max_to_traffick(count fc);

/* include files */ 
/* ============= */ 
	#include "vlog3_extra_traffick.c"   /* VLOG3 diverse extra zaken zoals, omgeving, en reden van extra wachttijd */
	#include "t2r_t2g.c"                /* algemene functie definities                                        */
	#include "hvrt_fctiming.c"          /* afhandeling fasecyclus timingsberichten, t.b.v. VLOG en interface  */

/* Definitie functies                                                                                       */
/* ==================                                                                                       */


/* Deze functie dient binnen system_application2() te worden aangeroepen */
void fc_timing_traffick(void)
{

	/* 
	Binnen deze functie dient voor elke regelstruktuur eerst de gewenste gegevens naar de CCOL2SPAT struct te worden geschreven.
	Daarna volgt de aanroep fc_timing_all();
	
	Na deze aanroep is de CCOL2SPAT struct geheel verwerkt en bijgewerkt.

	*/

	count i;
	mulv ttr = NK;

	/* vlog extra info ( vlog3_extra_.. ) */
	bool regen, mist, gladheid;

	regen = mist = gladheid = FALSE;

#ifdef isregen
	if ( IS[isregen] ) regen = TRUE;
#endif

#ifdef ismist
	if ( IS[ismist] ) mist = TRUE;
#endif

#ifdef isgladheid
	if ( IS[isgladheid] ) gladheid = TRUE;
#endif

	proc_omgevingsfactor((bool) regen, (bool) mist, (bool) gladheid);

/* reden voor wachttijd ( vlog3_extra_.. ) */

	proc_reden_voor_wachtijd(); /* verwerken en schrijven 'reden voor eextra wachtijd' naar interface */


	for ( i = 0 ; i < FC_MAX ; i++ ) {
		/* bepaal fc_timings gegevens vanuit applicatie: */
		/* let op: de fc_status is ��n systeemslag verder als in application() */
		/* binnen 'proc_schrijf_tijd_tot_start_groen()' en 'proc_schrijf_tijd_tot_einde_groen()' wordt al dit gecorrigeerd */


		/* bepaald lopende ontruimingstijd */
		ttr = max_to_traffick(i);

		/* Vullen sturct CCOL2SPAT */
		proc_schrijf_status( &CCOL2SPAT[i], (count) i);
		proc_schrijf_extra_status( &CCOL2SPAT[i], (count) i, (bool) !somgk(i), (bool) _GRN[i]);
		proc_schrijf_garantie_rood_tijd( &CCOL2SPAT[i], (count) i);
		proc_schrijf_garantie_groen_tijd( &CCOL2SPAT[i], (count) i);
		proc_schrijf_geel_tijd( &CCOL2SPAT[i], (count) i);
		proc_schrijf_minimum_tijd_tot_realisatie( &CCOL2SPAT[i], (count) i , ttr);
		proc_schrijf_tijd_tot_start_groen( &CCOL2SPAT[i], (count) i , (mulv) T2SG[i]);
		proc_schrijf_tijd_tot_einde_groen( &CCOL2SPAT[i], (count) i , (mulv) T2EG[i]);

		proc_schrijf_max_tijd_rood( &CCOL2SPAT[i], (count) i , (mulv) T2SGM[i]);
		proc_schrijf_max_tijd_groen( &CCOL2SPAT[i], (count) i , (mulv) T2EGM[i]);
		/* todo:  (bijzondere) realisatie */

	}

	fc_timing_all(); /* verwerken en versturen fc_timings berichten */

	/* CCOL2SPAT eventueel uitlezen en verwerken / verklikking */
	for ( i = 0 ; i < FC_MAX ; i++ ) {
		BT2SG[i] = CCOL2SPAT[i].tijd_tot_einde_rood_kans;
		BT2EG[i] = CCOL2SPAT[i].tijd_tot_einde_groen_kans;
	}

}

/* =============================================== */
/* vanaf hier regelstruktuur afhankelijke functies */
/* =============================================== */

/* -------------------------------------------------------------------------------------------------------- */
/* Bepaal afgevlakte groenbehoefte tijdens LOKAAL bedrijf - aanroep voor proc_reset_einde()                 */
/* -------------------------------------------------------------------------------------------------------- */
void proc_groen_new(void)             /* Fik170401                                                        */
{
	count i;
	mulv _old, _meting;
		
	if (PRM[prmfactor] > 10) {
		PRM[prmfactor] = 0;
		CIF_PARM1WIJZAP = -2;
	}
		
	for (i = 0; i < FC_MAX; i++) {

		if (SG[i]) FKG[i] = 0;
		if (TS && G[i] && (FKG[i] < 255) && !MG[i]) FKG[i]++;
			
		/* bepaal alternatieve realisatie aktief                                                              */
		if (!G[i] && !EG[i]) _AREA[i] = FALSE;
			
		if (SFG[i] && (AKTPRG == 0)) {
			if (!(SGD[i] & BIT3) && (!(SGD[i] & BIT0) || (SGD[i] & BIT2)))
				_AREA[i] = TRUE;
		}
		if (SFG[i] && (AKTPRG > 0)) {
			if (!(HVAS[i] & BIT4) && (!(HVAS[i] & BIT0) || (HVAS[i] & BIT3)))
				_AREA[i] = TRUE;
		}
			
		/* overgang van alternatief naar primair                                                              */
		if (G[i] && _AREA[i] && (AKTPRG == 0)) {
			if ((SGD[i] & BIT3) || ((SGD[i] & BIT0) && !(SGD[i] & BIT2)))
				_AREA[i] = FALSE;
		}
		if (G[i] && _AREA[i] && (AKTPRG > 0)) {
			if ((HVAS[i] & BIT4) || ((HVAS[i] & BIT0) && !(HVAS[i] & BIT3)))
				_AREA[i] = FALSE;
		}
			
		if (SSGD) _GRNNEW[i] = _GRNOLD[i] = PRIM[i];
		if (SG[i]) _GRN_meting[i] = TRUE;
			
		if (G[i] && !MG[i] && Z[i]) _GRN_meting[i] = FALSE;
		if (G[i] && !MG[i] && (SGD[i] & BIT7) && !(YM[i] & BIT4)) _GRN_meting[i] = FALSE;
		if (G[i] && WG[i] && (YW[i] & BIT5)) _GRN_meting[i] = FALSE;
			
		if (G[i] && FIXATIE) _GRN_meting[i] = FALSE;
		if (EG[i] && _AREA[i]) _GRN_meting[i] = FALSE;
			
		if (EG[i] && _GRN_meting[i] && (PRM[prmfactor] != 0)) {
			_old = _GRNNEW[i];
			_meting = FKG[i];
			if (FKG[i] > PRIM[i]) _meting = PRIM[i];
			_GRNNEW[i] = (((10 - PRM[prmfactor]) * _GRNOLD[i] + PRM[prmfactor] * _meting) + 5) / 10; /* @@ MS    + 5   i.v.m. afronding */
			_GRNOLD[i] = _old;
		}
			
		if (PRM[prmfactor] == 0) _GRNNEW[i] = _GRNOLD[i] = PRIM[i];
	}
}
 
/* -------------------------------------------------------------------------------------------------------- */
/* Time to Red - Time to Green                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
void Time_to_Red_Time_to_Green(void)  /* Fik170401                                                        */
{
	count i, j, k;
	count i_blk, j_blk;
	mulv ontruim, vorig, correctie;
	mulv _TEG_, _ALTM_;

	_ALTM_ = 0; /* @@ MS  i.v.m. Warning !! */

	for (i = 0; i < FC_MAX; i++) {       /* initialiseer T2SG[] en T2EG[]                                    */
		if ( G[i] ) T2SG[i] = NG;
		else T2SG[i] = NG; /* SPAT_TIJD_MAX; */
		T2EG[i] = NG;
		T2SGM[i] = NG;
		T2EGM[i] = NG;
		EK[i] = FALSE;
	}
		
	if (AKTPRG == 0) {                 
		/* blokken structuur                                                */
		for (i = 0; i < FC_MAX; i++) {
			/* bepaal voor richting in G[]                                    */
			if (G[i] && (_GRNNEW[i] > KGCA[i])) _TEG_ = _GRNNEW[i] - KGCA[i]; /* @@ MS 19-03-2018  KG[i]  gewijzigd in KGCA[i] */
			else _TEG_ = 0;
				
			if (ALTM[i] > _GRNNEW[i]) _ALTM_ = _GRNNEW[i];
			else _ALTM_ = ALTM[i];
				
			if (G[i]) {
				if (A[i] & BIT2) {
					if (KG[i] < _ALTM_) T2EG[i] = (_ALTM_ - KG[i]) * 10;
					else T2EG[i] = 0;
				}
				else T2EG[i] = _TEG_ * 10;

				if (SGD[i] & BIT7) {
					if (TEG[i] * 10 < T2EG[i]) T2EG[i] = TEG[i] * 10;
				}
			}
			if (G[i] && ((SGD[i] & BIT5) || HLPD[i][0])) { 
				/* correctie bij prioriteitsrealisatie                              */
				/* @@ 15-08-2018 Aangepast t.b.v. HLPD[] */
				if ( (HLPD[i][4] > HLPD[i][1]) && (HLPD[i][1] > 0) ) {
					T2EG[i] = (HLPD[i][4] - HLPD[i][1]) * 10;
				}
				else {
					if ( HLPD[i][4] > KG[i] ) {
						T2EG[i] = (HLPD[i][4] - KG[i]) * 10;
					}
					else {
						if ( TEG[i] ) {
							T2EG[i] = TEG[i] * 10;
						}
					}
				}
			}
		}

		for (i = 0; i < FC_MAX; i++) {
			/* bepaal voor richting in RA[]                                     */
			if (RA[i] || R[i] && A[i] && B[i] && !RR[i] && !BL[i]) {   /* @@ 24-01-2018   R[i] && .. */
				T2SG[i] = TTW[i];
				for (j = 0; j < GKFC_MAX[i]; j++) {
					k = TO_pointer[i][j];
					ontruim = 0;
					if (G[k] && (T2EG[k] >= 0)) {
						if (TO_max[k][i] >= 0) ontruim = TGL_max[k] + TO_max[k][i];
						if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
					}
				}
					
				for (k = 0; k < FC_MAX; k++) {
					if (TMPc[i][k]) {
						ontruim = 0;
						if (G[k] && (T2EG[k] >= 0)) {
							if (TMPo[k][i] >= 0) ontruim = TGL_max[k] + TMPo[k][i];
							if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
						}
					}
				}
					
				if (TTP[i] < 255) T2EG[i] = T2SG[i] + _GRNNEW[i] * 10;
				else T2EG[i] = T2SG[i] + _ALTM_ * 10;
					
				if ((TTP[i] < 255) && (SGD[i] & BIT5))  {
					/* correctie bij prioriteitsrealisatie                              */
					correctie = 0;
					if (TTP[i] * 10 > T2SG[i]) correctie = TTP[i] * 10 - T2SG[i];
					T2EG[i] = TEG[i] * 10 - correctie;
				}
				EK[i] = TRUE;
			}
		}
			
		for (i = 0; i < FC_MAX; i++) { 
			/* bepaal voor eerstkomende richting met prioriteit                 */
			if ((TTP[i] < 255) && (T2EG[i] == NG) && (SGD[i] & BIT5)) {
				T2SG[i] = TTW[i];
				for (j = 0; j < GKFC_MAX[i]; j++) {
					k = TO_pointer[i][j];
					ontruim = 0;
					if (T2EG[k] >= 0) {
						if (TO_max[k][i] >= 0) ontruim = TGL_max[k] + TO_max[k][i];
					}
					if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
				}
					
				for (k = 0; k < FC_MAX; k++) {
					if (TMPc[i][k]) {
						ontruim = 0;
						if (T2EG[k] >= 0) {
							if (TMPo[k][i] >= 0) ontruim = TGL_max[k] + TMPo[k][i];
						}
						if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
					}
				}
					
				correctie = 0;
				if (TTP[i] * 10 > T2SG[i]) correctie = TTP[i] * 10 - T2SG[i];
				T2EG[i] = TEG[i] * 10 - correctie;
					
				EK[i] = TRUE;
				for (j = 0; j < GKFC_MAX[i]; j++)  {
					k = TO_pointer[i][j];
					if (EK[k]) EK[i] = FALSE;
				}
				for (k = 0; k < FC_MAX; k++) {
					if (TMPc[i][k]) {
						if (EK[k]) EK[i] = FALSE;
					}
				}
					
			}
		}

		if (SBL) {
			/* corrigeer realisatie geheugen op start blok                      */
			vorig = BLOK - 1;
			if (vorig == 0) vorig = MAX_BLK;
			for (i = 0; i < FC_MAX; i++) {
				if (PBLK[i] == vorig) SGD[i] &= (~BIT2);
			}
		}

		for (i_blk = 0; i_blk < MAX_BLK + 2; i_blk++) { 
			/* bepaal overige richtingen                                        */
			if (BLOK + i_blk > MAX_BLK) j_blk = BLOK + i_blk - MAX_BLK;
			else j_blk = BLOK + i_blk;

			for (i = 0; i < FC_MAX; i++) {

				if ( (PBLK[i] == j_blk) && (T2EG[i] == NG) && !BL[i] ) {  /* @@ 30-03-2018   !BL[] i.v.m. Hulpdienst */
					if ((i_blk < MAX_BLK) && !(SGD[i] & BIT2) || (i_blk >= MAX_BLK) && (SGD[i] & BIT2)) {
							
						T2SG[i] = TTW[i];
						for (j = 0; j < GKFC_MAX[i]; j++) {
							k = TO_pointer[i][j];
							ontruim = 0;
							if (T2EG[k] >= 0) {
								if (TO_max[k][i] >= 0) ontruim = TGL_max[k] + TO_max[k][i];
							}
							if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
						}

						for (k = 0; k < FC_MAX; k++) {
							if (TMPc[i][k]) {
								ontruim = 0;
								if ( T2EG[k] >= 0 ) {
									if (TMPo[k][i] >= 0) ontruim = TGL_max[k] + TMPo[k][i];
								}
								if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
							}
						}
						T2EG[i] = T2SG[i] + _GRNNEW[i] * 10;

						EK[i] = TRUE;
						for (j = 0; j < GKFC_MAX[i]; j++) {
							k = TO_pointer[i][j];
							if (EK[k]) EK[i] = FALSE;
						}
						for (k = 0; k < FC_MAX; k++) {
							if (TMPc[i][k])
							{
								if (EK[k]) EK[i] = FALSE;
							}
						}

					}
				}
			}
		}

	}
	else {
		/* halfstarre structuur                                             */

		for (i = 0; i < FC_MAX; i++) {
			/* bepaal voor richting in G[]                                      */
			if (G[i]) {
				if ((HVAS[i] & BIT0) && !(HVAS[i] & BIT3)) T2EG[i] = teg_vas(i);
				else {
					if (!MG[i]) {
						if (KG[i] < ALTM[i]) T2EG[i] = (ALTM[i] - KG[i]) * 10;
						else T2EG[i] = 0;
					}
					else T2EG[i] = 0;
				}
			}
			if (G[i] && ((SGD[i] & BIT5) || HLPD[i][0])) { 
				/* correctie bij prioriteitsrealisatie                              */
				if ( (HLPD[i][4] > HLPD[i][1]) && (HLPD[i][1] > 0) ) {
					T2EG[i] = (HLPD[i][4] - HLPD[i][1]) * 10;
				}
				else {
					if ( HLPD[i][4] > KG[i] ) {
						T2EG[i] = (HLPD[i][4] - KG[i]) * 10;
					}
					else {
						if ( TEG[i] ) {
							T2EG[i] = TEG[i] * 10;
						}
					}
				}
			}
		}

		for (i = 0; i < FC_MAX; i++) {
			/* bepaal voor richting in RA[]                                     */
			if (RA[i] || R[i] && A[i] && B[i] && !RR[i] && !BL[i]) {   /* @@ 24-01-2018   R[i] && .. */
				T2SG[i] = TTW[i];
				for (j = 0; j < GKFC_MAX[i]; j++) {
					k = TO_pointer[i][j];
					ontruim = 0;
					if (G[k] && (T2EG[k] >= 0)) {
						if (TO_max[k][i] >= 0) ontruim = TGL_max[k] + TO_max[k][i];
						if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
					}
				}

				for (k = 0; k < FC_MAX; k++) {
					if (TMPc[i][k]) {
						ontruim = 0;
						if (G[k] && (T2EG[k] >= 0)) {
							if (TMPo[k][i] >= 0) ontruim = TGL_max[k] + TMPo[k][i];
							if (T2EG[k] + ontruim > T2SG[i]) T2SG[i] = T2EG[k] + ontruim;
						}
					}
				}

				if ((HVAS[i] & BIT0) && !(HVAS[i] & BIT3) || (A[i] & BIT4)) T2EG[i] = T2SG[i] + teg_vas(i);
				else T2EG[i] = T2SG[i] + ALTM[i] * 10;
			}
		}

		for (i = 0; i < FC_MAX; i++) {
			/* bepaal voor overige richtingen                                   */

			if (T2EG[i] == NG) {
				if (STAP < VAS2[i]) {
					T2SG[i] = (VAS2[i] - STAP) * 10;
				}
				else {
					if (STAP == VAS2[i]) {
						T2SG[i] = 0;
					}
					else {
						T2SG[i] = (CTYD - STAP + VAS2[i]) * 10;
					}
				}
				T2EG[i] = teg_vas(i);

				if (!G[i] && (HVAS[i] & BIT3)) {
					/* Corrigeer indien versneld afgesloten                             */
					if ((VAS4[i] > VAS2[i]) && ((STAP <= VAS2[i]) || (STAP >= VAS4[i])) ||
							(VAS4[i] < VAS2[i]) && (STAP <= VAS2[i]) && (STAP >= VAS4[i])) {
						T2SG[i] += (CTYD * 10);
						T2EG[i] += (CTYD * 10);
					}
				}
			}
		}
	}

	for (i = 0; i < FC_MAX; i++) {
		/* zet op NG indien richting niet is aangevraagd                    */
		if (GL[i] || R[i] && !A[i] ) T2SG[i] = T2EG[i] = NG;
		if ( !G[i] && BL[i] ) T2SG[i] = T2EG[i] = NG; /* nog aanpassen t.b.v. Hulpdiensten of file met blokkeering */
		if ( WG[i] && (YW[i]&BIT5) ) T2EG[i] = NG; /* @@ MS 19-03-2018  geen T2EG[] tijdens wachtstand groen */
	}

	for (j = 0; j <= 3; j++) {
		for (i = 0; i < FC_MAX; i++) {
			if (T2EG[i] != NG) teg_shadow(i); /* langer groen door meeverlengen (3x ivm nesting) */
		}
	}

	/* bepalen Maximum tijd */
	if ( AKTPRG == 0 ) {
		for (i = 0; i < FC_MAX; i++) {
			if ( G[i] ) {
				T2EGM[i] = TEG[i] * 10;
			}
			else T2EGM[i] = NG;
		}

		for (i = 0; i < FC_MAX; i++) {
			if ( RA[i] ) {
				T2EGM[i] = T2SG[i] + (TEG[i] * 10);
			}
		}

		for (j = 0; j <= 3; j++) {
			for (i = 0; i < FC_MAX; i++) {
				if (T2EGM[i] != NG) tegm_shadow(i); /* langer groen door meeverlengen (3x ivm nesting) : maximum versie */
			}
		}

		for (i = 0; i < FC_MAX; i++) {
			if ( G[i] ) T2EGM[i] += DZ_tijd[i]; /* veiligheidsgroen toevoegen !*/
			/* todo: */
			/* correctie toevoegen t.b.v. 'mogelijke/toegestane' INGREPEN, die NIET in TEG[] zijn meegenomen: koppeling, OV, Hulpdienst of tovergroen */
		}

		for (i = 0; i < FC_MAX; i++) {
			/* bepaal voor richting in RA[]                                     */
			if (RA[i] || R[i] && A[i] && B[i] && !RR[i] && !BL[i]) {
				if ( T2SG[i] > 0 ) { 
					T2SGM[i] = T2SG[i];
					for (j = 0; j < GKFC_MAX[i]; j++) {
						k = TO_pointer[i][j];
						if ( (T2EGM[k] >= 0) && (T2EG[k]<T2SG[i]) ) {
							if ( T2SGM[i] < T2SG[i] + (T2EGM[k] - T2EG[k]) ) T2SGM[i] = T2SG[i] + (T2EGM[k] - T2EG[k]);
						}
					}
					for (k = 0; k < FC_MAX; k++) {
						if ( TMPc[i][k] && (T2EGM[k] >= 0) && (T2EG[k]<T2SG[i]) ) {
							if ( T2SGM[i] < T2SG[i] + (T2EGM[k] - T2EG[k]) ) T2SGM[i] = T2SG[i] + (T2EGM[k] - T2EG[k]);
						}
					}

					if (TTP[i] < 255) T2EGM[i] = T2SGM[i] + PRIM[i] * 10 + DZ_tijd[i]; /* veiligheidsgroen toevoegen !*/
					else T2EGM[i] = T2SGM[i] + _ALTM_ * 10 + DZ_tijd[i]; /* veiligheidsgroen toevoegen !*/
					
					/* GEEN correctie bij prioriteitsrealisatie                              */
					/*
					if ((TTP[i] < 255) && (SGD[i] & BIT5))  {
						correctie = 0;
						if (TTP[i] * 10 > T2SG[i]) correctie = TTP[i] * 10 - T2SGM[i];
						T2EGM[i] = TEG[i] * 10 - correctie;
					}
					*/
				}

			}
		}
		/* volgende blokken NIET bepalen. */

	}
}

/* -------------------------------------------------------------------------------------------------------- */
/* Procedure bepaal BLWT[] - aanroep na proc_max_wt()                                                       */
/* -------------------------------------------------------------------------------------------------------- */
void proc_blwt_new(void)              /* Fik170401                                                        */
{
	count i, j, k;
	bool voorw;

	for (i = 0; i < FC_MAX; i++) {
		voorw = FALSE;
		for (j = 0; j < FKFC_MAX[i]; j++) {
			k = TO_pointer[i][j];
			if ((WTVFC[k] & BIT0) && RA[k]) voorw = TRUE;
		}
		for (k = 0; k < FC_MAX; k++) {
			if (TMPc[i][k] || TMPf[i][k]) {
				if ((WTVFC[k] & BIT0) && RA[k]) voorw = TRUE;
			}
		}
		BLWT[i] |= voorw && (!G[i] || !(SGD[i] & BIT5) && (AKTPRG == 0));
	}
	for (i = 0; i < FC_MAX; i++) {
		if ((WTVFC[i] & BIT1) && RA[i]) {
			for (j = 0; j < FKFC_MAX[i]; j++) {
				k = TO_pointer[i][j];
				BLWT[k] = TRUE;
			}
			for (k = 0; k < FC_MAX; k++) {
				if (TMPc[i][k] || TMPf[i][k]) {
					BLWT[k] = TRUE;
				}
			}
		}
	}

}

#ifdef NO_WIJZ_2018_03_19
void proc_us_fc_timing( count fc, count ust2sg, count usbt2sg, count ust2eg, count usbt2eg)
{

	if ( ust2sg > NG ) CIF_GUS[ust2sg] = T2SG[fc];
	if ( usbt2sg > NG ) CIF_GUS[usbt2sg] = BT2SG[fc];
	if ( ust2eg > NG ) CIF_GUS[ust2eg] = T2EG[fc];
	if ( usbt2eg > NG ) CIF_GUS[usbt2eg] = BT2EG[fc];

}
#endif


/* -------------------------------------------------------------------------------------------------------- */
/* Tijd tot einde groen halfstar                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
mulv teg_vas(                         /* Fik170401                                                        */
	count fc)
{
	if (STAP < VAS4[fc]) {
		return (VAS4[fc] - STAP) * 10;
	}
	else {
		return (VAS4[fc] + CTYD - STAP) * 10;
	}
}

/* -------------------------------------------------------------------------------------------------------- */
/* In the shadows                                                                                           */
/* -------------------------------------------------------------------------------------------------------- */
void teg_shadow(                      /* Fik170401                                                        */
	count fc)
{
	count i, j, k;
	bool toestemming;
		
	for (i = 0; i < FC_MAX; i++) {
		if (MV_matrix[fc][i] && G[fc] && (G[i] || RA[i] || (A[i] && B[i] && !RR[i] || EK[i]) && !BL[i]) && (T2EG[fc] < T2EG[i]) ) {
			toestemming = TRUE;
			for (j = 0; j < GKFC_MAX[fc]; j++) {
				k = TO_pointer[fc][j];                                 /* Fik181505 toegevoegd && A[k] */
				if ((TO_max[i][k] == NG) && !TMPc[i][k] && !TMPc[k][i] && A[k] && EK[k]) toestemming = FALSE;
			}
			for (k = 0; k < FC_MAX; k++) {                    /* ook TMP conflicten van fc afkluiven */
				if (TMPc[fc][k] || TMPc[k][fc]) {
					if ((TO_max[i][k] == NG) && !TMPc[i][k] && !TMPc[k][i] && A[k] && EK[k]) toestemming = FALSE;
				}
			}
			if (toestemming) {
				T2EG[fc] = T2EG[i];
			}
		}
	}
}

void tegm_shadow(                      /* Fik170401  MS -> maximum                                          */
	count fc)
{
	count i, j, k;
	bool toestemming;
		
	for (i = 0; i < FC_MAX; i++) {
		if (MV_matrix[fc][i] && G[fc] && (G[i] || RA[i] || (A[i] && B[i] && !RR[i] || EK[i]) && !BL[i]) && (T2EGM[fc] < (TEG[i] * 10)) ) {
			toestemming = TRUE;
			for (j = 0; j < GKFC_MAX[fc]; j++) {
				k = TO_pointer[fc][j];
				if ((TO_max[i][k] == NG) && !TMPc[i][fc] && !TMPc[fc][i] && A[k] && EK[k]) toestemming = FALSE;
			}
			for (k = 0; k < FC_MAX; k++) {                    /* ook TMP conflicten van fc afkluiven */
				if (TMPc[fc][k] || TMPc[k][fc]) {
					if ((TO_max[i][k] == NG) && !TMPc[i][k] && !TMPc[k][i] && A[k] && EK[k]) toestemming = FALSE;
				}
			}
			if (toestemming) {
				T2EGM[fc] = TEG[i] * 10;
			}
		}
	}
}


mulv max_to_traffick(count fc)
{
	count j,k;
	mulv result, tmp;

	result = NK;
	
	if ( R[fc] ) {
		for (j = 0; j < GKFC_MAX[fc]; j++) {
			k = TO_pointer[fc][j];
			tmp = NK;
			if ( G[k] ) tmp = TGL_max[k] + TO_max[k][fc];
			if ( GL[k] ) tmp = TGL_max[k] - TGL_timer[k] + TO_max[k][fc];
			if ( R[k] && TO[k][fc] ) tmp = TO_max[k][fc] - TO_timer[k];
			if ( tmp > result ) result = tmp;
		}
		for (k = 0; k < FC_MAX; k++) {
			if ( TMPc[k][fc] ) {
				if ( G[k] ) tmp = TGL_max[k] + TMPo[k][fc];
				if ( GL[k] ) tmp = TGL_max[k] - TGL_timer[k] + TMPo[k][fc];
				if ( R[k] && (TMPo[k][fc] > 0) ) tmp = TMPo[k][fc];
			}
		}
	}

	return(result);

}

