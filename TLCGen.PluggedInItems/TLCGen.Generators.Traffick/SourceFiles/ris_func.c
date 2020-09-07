/* DECLARATIE RIS SIMULATIE FUNCTIES */
/* ========================== */
/*
 *	Huijskes VRT B.V.
 *	14 Oktoberplein 6
 *	Overloon
 *
 *	Module: functies t.b.v. afhandeling van RIS-interface berichten, en functies t.b.v. ris voertuigen in applicatie
 *
 *	Filenaam        : ris_sim.c
 *  Ontwerp					: P.N.M.S. Huijskes
 *	Datum           : ..-..-2017
 *	CCOL						: versie 9.0 en hoger,  Generator PNH versie ?? en hoger
 *
	Revisie:
		..-..-2017 P.N.M.S. Huijskes:  opzet
               Versie: 0.12, kruispunt 4 toegevoegd
               Versie: 0.13, new_ris_data toegevoegd, alleen update _ABON bij nieuwe of vervallen data
               Versie: 0.14, SRM toegevoegd
               Versie: 0.15, acceleratie niet meenemen in berekeningen bij -17
		12-01-2018 M. Scherjon: Voertuigsnelheidscorrectie toegevoegd.
		19-01-2018 M. Scherjon: Versie 1.0
		05-04-2018 P. Huijskes: Versie 1.95 aanpassingen n.a.v. testen Swarco issuelijst 20180314
		17-07-2018 M. Veenstra: Versie 2.00 gechikt gemaakt voor Dynniq
		22-08-2018 P. Huijskes: Versie 2.00 aanpassing naar RIF interface
		
*/

  

/* -------------------------------------------------------------------------------------------------------- */
/* Include files                                                                                            */
/* -------------------------------------------------------------------------------------------------------- */
  
	#include "sysdef.c"
	#include "cif.inc"
	#include "stdio.h"

	#include "string.h"
#ifdef DYNNIQ
	#include "dynniq_ris_api_adapter.c"
#endif


/* Bepaling locationTime voor simulatieomgeving */
int64_t time_2_nu(void)	
{
	int64_t time = 0;
	int basic_jaar = 1970;
	int i;
	int aantal_schrikkeljaar = (CIF_KLOK[CIF_JAAR] - basic_jaar) / 4; /* In het jaar zelf zal maand 2 de correctie krijgen, dit is tot de jaargrens */

	time += (CIF_KLOK[CIF_JAAR] - basic_jaar) * 365 * 24 * 60 * 60 * 1000;
	time += aantal_schrikkeljaar * 1 * 24 * 60 * 60 * 1000;

	for (i = 0; i < CIF_KLOK[CIF_MAAND] - 1; i++) {	/* Hele maanden */
		time += dagmaand[i] * 24 * 60 * 60 * 1000;
	}
	if ((CIF_KLOK[CIF_MAAND] == 3) && (CIF_KLOK[CIF_JAAR] % 4 == 0)) {
		time += 24 * 60 * 60 * 1000;	/* Extra schrikkeldag */
	}
	time += (CIF_KLOK[CIF_DAG] - 1) * 24 * 60 * 60 * 1000;
	time += CIF_KLOK[CIF_UUR] * 60 * 60 * 1000;
	time += CIF_KLOK[CIF_MINUUT] * 60 * 1000;
	time += CIF_KLOK[CIF_SECONDE] * 1000;
#ifndef NO_CVN_50
	time += CIF_KLOK[CIF_TSECONDE] * 100;
#endif /* NO_CVN_50 */

	return (time);
}

/* Initialisaties */
void wissen_cam(struct Rif_ItsStation a[], int i, const int max)
{
	int j = 0;
	if (i < max) {
		for (j = 0; j < RIF_MAXLANES; j++) {
			a[i].id[0] = '\0';  /* Wanneer ObjectID leeg is, wordt het gehele struct geacht waardeloos te zijn qua inhoud */
			a[i].matches[j].intersection[0] = '\0';
			a[i].matches[j].signalGroup[0] = '\0';	/* Geen signaalgroep ingevuld */
		}
	}
}

void wissen_srm(struct Rif_PrioritizationRequest a[], int i, const int max)
{
	if (i < max) {
		a[i].id[0] = '\0';  /* Wanneer ObjectID leeg is, wordt het gehele struct geacht waardeloos te zijn qua inhoud */
	}
}

void init_its_buffer(void)
{
	int i = 0;

	RIF_ITSSTATION_PB_WRITE = 0;
	RIF_ITSSTATION_PB_READ = 0;

	RIF_PRIOREQUEST_PB_WRITE = 0;
	RIF_PRIOREQUEST_PB_READ = 0;

	RIF_ACTIVEPRIO_AP_READ = 0;
	RIF_ACTIVEPRIO_AP_WRITE = 0;


	for (i = 0; i < RIF_MAX_ITSSTATION_AP; i++) {
		wissen_cam(ITSSTATION_AP, i, RIF_MAX_ITSSTATION_AP);
	}
	for (i = 0; i < RIF_MAX_ITSSTATION_AP; i++) {
		wissen_cam(ITSSTATION_AP_corr, i, RIF_MAX_ITSSTATION_AP);
	}
	for (i = 0; i < RIF_MAX_ITSSTATION_PB; i++) {
		wissen_cam(RIF_ITSSTATION_PB, i, RIF_MAX_ITSSTATION_PB);
	}

	for (i = 0; i < RIF_MAX_PRIO_REQUEST_AP; i++) {
		wissen_srm(PRIOREQUEST_AP, i, RIF_MAX_PRIO_REQUEST_AP);
	}
	for (i = 0; i < RIF_MAX_PRIO_REQUEST_AP; i++) {
		wissen_srm(PRIOREQUEST_AP_corr, i, RIF_MAX_PRIO_REQUEST_AP);
	}
	for (i = 0; i < RIF_MAX_PRIOREQUEST_PB; i++) {
		wissen_srm(RIF_PRIOREQUEST_PB, i, RIF_MAX_PRIOREQUEST_PB);
	}
}

void init_its_abon(mulv fc,	/* richting */
	mulv prmvag1_o,		/* PRM VAG1 ondergrens */
	mulv prmvag1_b,		/* PRM VAG1 bovengrens */
	mulv prmvag2_o,		/*     VAG2 */
	mulv prmvag2_b,
	mulv prmvag3_o,		/*     VAG3 */
	mulv prmvag3_b,
	mulv prmvag4_o,		/*     VAG4 */
	mulv prmvag4_b,
	mulv prmtover_o,	/*     tovergroen */
	mulv prmtover_b,
	mulv prmopvv_o,		/*     openbaar vervoer */
	mulv prmopvv_b,
	mulv prmhlpd_o,		/*     hulpdienst */
	mulv prmhlpd_b,
	mulv prmpeloton_o,
	mulv prmpeloton_b)
{
	_ABON[fc].vag1_o = (prmvag1_o != NG) ? PRM[prmvag1_o] : NG;
	_ABON[fc].vag1_b = (prmvag1_b != NG) ? PRM[prmvag1_b] : NG;
	_ABON[fc].vag2_o = (prmvag2_o != NG) ? PRM[prmvag2_o] : NG;
	_ABON[fc].vag2_b = (prmvag2_b != NG) ? PRM[prmvag2_b] : NG;
	_ABON[fc].vag3_o = (prmvag3_o != NG) ? PRM[prmvag3_o] : NG;
	_ABON[fc].vag3_b = (prmvag3_b != NG) ? PRM[prmvag3_b] : NG;
	_ABON[fc].vag4_o = (prmvag4_o != NG) ? PRM[prmvag4_o] : NG;
	_ABON[fc].vag4_b = (prmvag4_b != NG) ? PRM[prmvag4_b] : NG;

	/* Voor tovergroen, openbaar vervoer, hulpdienst; 2e PRM == NG --> bovengrens 1 meter boven ondergrens */
	_ABON[fc].tover_o = (prmtover_o != NG) ? PRM[prmtover_o] : NG;
	_ABON[fc].tover_b = (prmtover_b != NG) ? PRM[prmtover_b] : NG;
	if ((_ABON[fc].tover_o != NG) && (_ABON[fc].tover_b == NG)) _ABON[fc].tover_b = _ABON[fc].tover_o + MIN_AFSTAND;
	if ((_ABON[fc].tover_o == NG) && (_ABON[fc].tover_b != NG)) _ABON[fc].tover_o = 0;

	_ABON[fc].opvv_o = (prmopvv_o != NG) ? PRM[prmopvv_o] : NG;
	_ABON[fc].opvv_b = (prmopvv_b != NG) ? PRM[prmopvv_b] : NG;
	if ((_ABON[fc].opvv_o != NG) && (_ABON[fc].opvv_b == NG)) _ABON[fc].opvv_b = _ABON[fc].opvv_o + MIN_AFSTAND;
	if ((_ABON[fc].opvv_o == NG) && (_ABON[fc].opvv_b != NG)) _ABON[fc].opvv_o = 0;

	_ABON[fc].hlpd_o = (prmhlpd_o != NG) ? PRM[prmhlpd_o] : NG;
	_ABON[fc].hlpd_b = (prmhlpd_b != NG) ? PRM[prmhlpd_b] : NG;
	if ((_ABON[fc].hlpd_o != NG) && (_ABON[fc].hlpd_b == NG)) _ABON[fc].hlpd_b = _ABON[fc].hlpd_o + MIN_AFSTAND;
	if ((_ABON[fc].hlpd_o == NG) && (_ABON[fc].hlpd_b != NG)) _ABON[fc].hlpd_o = 0;

	_ABON[fc].peloton_o = (prmpeloton_o != NG) ? PRM[prmpeloton_o] : NG;
	_ABON[fc].peloton_b = (prmpeloton_b != NG) ? PRM[prmpeloton_b] : NG;
	if ((_ABON[fc].peloton_o != NG) && (_ABON[fc].peloton_b == NG)) _ABON[fc].peloton_b = _ABON[fc].peloton_o + MIN_AFSTAND;
	if ((_ABON[fc].peloton_o == NG) && (_ABON[fc].peloton_b != NG)) _ABON[fc].peloton_o = 0;

#ifdef TESTOMGEVING
	/*
	_ABON[fc].vag1_o = 0;
	_ABON[fc].vag1_b = 20;
	_ABON[fc].vag2_o = 15;
	_ABON[fc].vag2_b = 35;
	_ABON[fc].vag3_o = 30;
	_ABON[fc].vag3_b = 60;
	_ABON[fc].vag4_o = 50;
	_ABON[fc].vag4_b = 100;
	_ABON[fc].hlpd_o = 0;
	_ABON[fc].hlpd_b = 250;
	_ABON[fc].opvv_o = 0;
	_ABON[fc].opvv_b = 250;
	_ABON[fc].tover_o = 100;
	_ABON[fc].tover_b = 200;
	*/
#endif

	/* Voertuigtypen per ABON */
	_ABON[fc].vag1_voertuig = (RIS_MOTOR | RIS_AUTO | RIS_VOETGANGER | RIS_FIETS | RIS_BUS | RIS_TRAM | RIS_VRACHTWAGEN | RIS_HULPDIENST);
	_ABON[fc].vag2_voertuig = (RIS_MOTOR | RIS_AUTO | RIS_VOETGANGER | RIS_FIETS | RIS_BUS | RIS_TRAM | RIS_VRACHTWAGEN | RIS_HULPDIENST);
	_ABON[fc].vag3_voertuig = (RIS_MOTOR | RIS_AUTO | RIS_VOETGANGER | RIS_FIETS | RIS_BUS | RIS_TRAM | RIS_VRACHTWAGEN | RIS_HULPDIENST);
	_ABON[fc].vag4_voertuig = (RIS_MOTOR | RIS_AUTO | RIS_VOETGANGER | RIS_FIETS | RIS_BUS | RIS_TRAM | RIS_VRACHTWAGEN | RIS_HULPDIENST);
	_ABON[fc].tover_voertuig = (RIS_VRACHTWAGEN);
	_ABON[fc].opvv_voertuig = (RIS_BUS);
	_ABON[fc].hlpd_voertuig = (RIS_HULPDIENST);
	_ABON[fc].peloton_voertuig = (RIS_MOTOR | RIS_AUTO | RIS_VOETGANGER | RIS_FIETS | RIS_BUS | RIS_TRAM | RIS_VRACHTWAGEN | RIS_HULPDIENST);
}





void ris_update_ap(struct Rif_ItsStation a[], int i, const int max)
{
	int j = i + 1;

	if (i < max) {
		while ((j < RIF_MAX_ITSSTATION_AP) && strlen(a[j].id)) {
			j++;
		}
		if ((i != j - 1) && (j > 0) && (j < RIF_MAX_ITSSTATION_AP)) {
			a[i] = a[j - 1];
			wissen_cam(a, j - 1, RIF_MAX_ITSSTATION_AP);
		}
	}
}

void ris_update_ap_srm(struct Rif_PrioritizationRequest a[], int i, const int max)
{
	int j = i + 1;

	if (i < max) {
		while ((j < max) && strlen(a[j].id)) {
			j++;
		}
		if ((i != j - 1) && (j > 0) && (j < max)) {
			a[i] = a[j - 1];
			wissen_srm(a, j - 1, max);
		}
	}
}

bool valid_objectid(rif_string id) {

	bool valid = TRUE;
	int valid_bit = 0;
	int i = 0;

	for (i = 0; i < (int)strlen(id); i++) {
		valid_bit = 0;
		if ((id[i] >= 'a') && (id[i] <= 'z')) valid_bit |= 0x01;
		if ((id[i] >= 'A') && (id[i] <= 'Z')) valid_bit |= 0x02;
		if ((id[i] >= '0') && (id[i] <= '9')) valid_bit |= 0x04;
		if ((id[i] == '_')                  ) valid_bit |= 0x08;
		if ((id[i] == '-')                  ) valid_bit |= 0x10;
		if (valid_bit == 0) valid = FALSE;	/* Als er al 1 karakter onjuist is ... */
	}

	return (valid);
}

void beh_lifetime_cam(struct Rif_ItsStation a[], struct Rif_ItsStation a_corr[], int lifetime)
{
	int i = 0;

	if (TE) {
		while ((i < RIF_MAX_ITSSTATION_AP) && strlen(a[i].id)) {
			a_corr[i].locationTime += TE * 100;
			i++;
		}

		i = 0;
		while ((i < RIF_MAX_ITSSTATION_AP) && strlen(a[i].id)) {
			if (a_corr[i].locationTime >(unsigned int)lifetime) {
				wissen_cam(a, i, RIF_MAX_ITSSTATION_AP); /* nog testen of kruispuntnummer overeenkomt? kan natuurlijk zijn dat er een 2e voertuig anderweg is van 1 naar 0 */
				wissen_cam(a_corr, i, RIF_MAX_ITSSTATION_AP);
				ris_update_ap(a, i, RIF_MAX_ITSSTATION_AP); /* pak het laatste voertuig en zet dat op deze plek */
				ris_update_ap(a_corr, i, RIF_MAX_ITSSTATION_AP);
				/* het kan dus zomaar dat van de gecopieerde versie de lifetime ook te hoog was. die wordt de volgende ronde wel weggegooid. */

				new_cam_data = TRUE;	/* Er is geen nieuw voertuig, maar wel een aanpassing van het buffer */
			}
			i++;
		}
	}
}

void beh_lifetime_srm(struct Rif_PrioritizationRequest a[], struct Rif_PrioritizationRequest a_corr[], int lifetime)
{
	int i = 0;

	if (TE) {
		while ((i < RIF_MAX_PRIO_REQUEST_AP) && strlen(a[i].id)) {
			a_corr[i].eta += TE * 100;
			i++;
		}

		i = 0;
		while ((i < RIF_MAX_PRIO_REQUEST_AP) && strlen(a[i].id)) {
			if (a_corr[i].eta > (unsigned int)lifetime) {
				wissen_srm(a, i, RIF_MAX_PRIO_REQUEST_AP); /* nog testen of kruispuntnummer overeenkomt? kan natuurlijk zijn dat er een 2e voertuig anderweg is van 1 naar 0 */
				wissen_srm(a_corr, i, RIF_MAX_PRIO_REQUEST_AP);
				ris_update_ap_srm(a, i, RIF_MAX_PRIO_REQUEST_AP); /* pak het laatste voertuig en zet dat op deze plek */
				ris_update_ap_srm(a_corr, i, RIF_MAX_PRIO_REQUEST_AP);
				/* het kan dus zomaar dat van de gecopieerde versie de lifetime ook te hoog was. die wordt de volgende ronde wel weggegooid. */

				new_srm_data = TRUE;	/* Er is geen nieuw voertuig, maar wel een aanpassing van het buffer */
			}
			i++;
		}
	}
}

void beh_correctie_buffer(struct Rif_ItsStation a[],			/* ITS buffer gelezen vanuit de PB */
                          struct Rif_ItsStation a_corr[])		/* Gecorrigeerd buffer op basis van locatie */
{
	int i = 0;
	int j = 0;
	float delta_d = 0;

	if (TE) {
		while ((i < RIF_MAX_ITSSTATION_AP) && strlen(a[i].id)) {
			if ((a[i].speed != -1.0) /*&& (a[i].acceleration != -17.0)*/) {
				/* Alleen bij geldige data corrigeren */
#ifdef _VRI_HVRT
				if (strcmp(a[i].id, "0000000001") == 0) {
					i = i;
				}
#endif
				delta_d  = ((float)a[i].speed * (float)(CORRETION_SPEED/100) * (float)a_corr[i].locationTime / (float)1000 );
				if (a[i].acceleration != -17.0) {
					if ( a[i].acceleration >= 0 ) {
						/* versnellen */
						delta_d += ((float)0.5 * (float)a[i].acceleration * (float)(CORRETION_ACCELERATION_UP/100) * (float)a_corr[i].locationTime * (float)a_corr[i].locationTime / (float)1000000);
					}
					else {
						/* vertragen */
						delta_d += ((float)0.5 * (float)a[i].acceleration * (float)(CORRETION_ACCELERATION_DOWN/100) * (float)a_corr[i].locationTime * (float)a_corr[i].locationTime / (float)1000000);
					}
				}
				for (j = 0; j < RIF_MAXLANES; j++) {
					if (strlen(a[i].matches[j].intersection) > 0) {
						a_corr[i].matches[j].distance = (float)a[i].matches[j].distance - (float)delta_d;
					}
				}
#ifdef _VRI_HVRT
				if (strcmp(a[i].id, "0000000001") == 0) {
					i = i;
				}
#endif
			}

			i++;
		}
	}
}



void lees_itsstation_cam(int read)
{
	/* nog even geen rekening houden met 2e kruispunt */

	/* Bestaat het ID reeds? */
	int i = 0;
	int j = 0;

/*	of de j loop er gewoon buiten halen???? */
	if (strlen(RIF_ITSSTATION_PB[read].id) &&
		valid_objectid(RIF_ITSSTATION_PB[read].id)) {	/* Er moet een geldige identificatie zijn */

		/* Allowed characters: �a-z� (ASCII 97 through 122), �A-Z� (ASCII 65 through 90), �0-9�, �_� (underscore, ASCII 95) and ��� (hyphen, ASCII 45). */
		i = 0;

		while ((i < RIF_MAX_ITSSTATION_AP) && (strcmp(RIF_ITSSTATION_PB[read].id, ITSSTATION_AP[i].id) != 0)) {
			i++;
		}
		if (i < RIF_MAX_ITSSTATION_AP) {	/* Er is een gelijk ObjectID gevonden */
			if (RIF_ITSSTATION_PB[read].valid) {
				for (j = 0; j < RIF_MAXLANES; j++) {
					if (strlen(RIF_ITSSTATION_PB[read].matches[j].intersection) > 0) {
						if (strcmp(RIF_ITSSTATION_PB[read].matches[j].intersection, ITSSTATION_AP[i].matches[j].intersection) == 0) {
							/* abon gegevens aanroepen   timestamp omzetten naar lifetime */
							ITSSTATION_AP[i] = RIF_ITSSTATION_PB[read];	/* Als het kruispuntnummer gelijk is, overschrijven ONGEACHT KRUISPUNTNUMMER */
							ITSSTATION_AP_corr[i] = RIF_ITSSTATION_PB[read];
							ITSSTATION_AP_corr[i].locationTime = 0;
							/* als krp != eigen, dan afstandscorrectie */
							if ((strcmp(ITSSTATION_AP[i].matches[j].intersection, SYSTEM_ITS_1) == 0) && (distance_1 != 0)) {
								ITSSTATION_AP     [i].matches[j].distance += distance_1;	/* testen op negativiteit?? Het wordt er toch bij opgeteld??? */
								ITSSTATION_AP_corr[i].matches[j].distance += distance_1;
							}
							if ((strcmp(ITSSTATION_AP[i].matches[j].intersection, SYSTEM_ITS_2) == 0) && (distance_2 != 0)) {
								ITSSTATION_AP     [i].matches[j].distance += distance_2;
								ITSSTATION_AP_corr[i].matches[j].distance += distance_2;
							}
							if ((strcmp(ITSSTATION_AP[i].matches[j].intersection, SYSTEM_ITS_3) == 0) && (distance_3 != 0)) {
								ITSSTATION_AP     [i].matches[j].distance += distance_3;
								ITSSTATION_AP_corr[i].matches[j].distance += distance_3;
							}
							if ((strcmp(ITSSTATION_AP[i].matches[j].intersection, SYSTEM_ITS_4) == 0) && (distance_4 != 0)) {
								ITSSTATION_AP     [i].matches[j].distance += distance_4;
								ITSSTATION_AP_corr[i].matches[j].distance += distance_4;
							}
							/* Er is niet sprake van een nieuw object */
						}
						else {	/* Niet gelijk, en er is al een voertuig van het eigen / zelfde kruispunt ingereden, WEL iets doen  = eigen krp voertuig laten meenemen */
							if ((strcmp(RIF_ITSSTATION_PB[read].matches[j].intersection, SYSTEM_ITS) == 0)) {
								/* abon gegevens aanroepen    timestamp omzetten naar lifetime */
								ITSSTATION_AP[i] = RIF_ITSSTATION_PB[read];
								ITSSTATION_AP_corr[i] = RIF_ITSSTATION_PB[read];
								ITSSTATION_AP_corr[i].locationTime = 0;
								/* geen nieuwe index !! new_ris_data = TRUE;	 Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */
							}
							/* Andersom lijkt niet logisch, als je eenmaal een voertuig van het eigen kruispunt hebt ontvangen, werk je nauwkeuriger en zal je niet meer naar het voertuig op afstand kijken. */
						}
					}
					else {
						break;
					}
				}
			}
			else {
				/* wissen en doorschuiven kan naar 1 functie */
				wissen_cam(ITSSTATION_AP, i, RIF_MAX_ITSSTATION_AP); /* nog testen of kruispuntnummer overeenkomt? kan natuurlijk zijn dat er een 2e voertuig anderweg is van 1 naar 0 */
				wissen_cam(ITSSTATION_AP_corr, i, RIF_MAX_ITSSTATION_AP);
				ris_update_ap(ITSSTATION_AP, i, RIF_MAX_ITSSTATION_AP); /* pak het laatste voertuig en zet dat op deze plek */
				ris_update_ap(ITSSTATION_AP_corr, i, RIF_MAX_ITSSTATION_AP);
				new_cam_data = TRUE;	/* Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */
			}
		}
		else {	/* En nu niet, dus het object is niet gevonden en bestaat dus nog niet. Zoeken naar een eerste vrije positie */
			i = 0;

			if (RIF_ITSSTATION_PB[read].valid) {
				while ((i < RIF_MAX_ITSSTATION_AP) && strlen(ITSSTATION_AP[i].id)) {
					i++;
				}
				if (i < RIF_MAX_ITSSTATION_AP) {
					/* Eerste lege opositie: aboneerwerk uitvoeren en vullen */ /* timestamp omzetten naar lifetime */
					ITSSTATION_AP[i] = RIF_ITSSTATION_PB[read];
					ITSSTATION_AP_corr[i] = RIF_ITSSTATION_PB[read];
					ITSSTATION_AP_corr[i].locationTime = 0;
					new_cam_data = TRUE;	/* Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */
				}
				else {
					/* Wanneer het buffer vol is, wordt het bericht weggegooid.*/
				}
			}
		}

	}
	/* LETOP aan de AP_corr locationTime niet aan eigen tijd relateren, omdat ik geheel niet weet of deze gelijk loopt met de interne klok */
}

void lees_itsstation_srm(int read)
{
	/* nog even geen rekening houden met 2e kruispunt */

	/* Bestaat het ID reeds? */
	int i = 0;

	/*	of de j loop er gewoon buiten halen???? */
	if (strlen(RIF_PRIOREQUEST_PB[read].id) &&
		valid_objectid(RIF_PRIOREQUEST_PB[read].id)) {	/* Er moet een geldige identificatie zijn */

		/* Allowed characters: ‘a-z’ (ASCII 97 through 122), ‘A-Z’ (ASCII 65 through 90), ‘0-9’, ‘_’ (underscore, ASCII 95) and ‘–‘ (hyphen, ASCII 45). */
		i = 0;

		while ((i < RIF_MAX_PRIO_REQUEST_AP) && (strcmp(RIF_PRIOREQUEST_PB[read].id, PRIOREQUEST_AP[i].id /*SYSTEM_ITS*/) != 0)) {
			i++;
		}
		if (i < RIF_MAX_PRIO_REQUEST_AP) {	/* Er is een gelijk ObjectID gevonden */
			if (RIF_PRIOREQUEST_PB[read].valid) {
				PRIOREQUEST_AP[i] = RIF_PRIOREQUEST_PB[read];	/* Als het kruispuntnummer gelijk is, overschrijven ONGEACHT KRUISPUNTNUMMER */
				PRIOREQUEST_AP_corr[i] = RIF_PRIOREQUEST_PB[read];
				PRIOREQUEST_AP_corr[i].eta = 0;
				/* Er is niet sprake van een nieuw object */

					/* Verzenden SSM bericht na ontvangst en update 20180316 */
					/* TODO: genereert SPAM: RIS_ssm_bericht(PRIOREQUEST_AP_corr[i].id, RIF_PRIORITIZATIONSTATE_REQUESTED); */
			}
			else {
				/* wissen en doorschuiven kan naar 1 functie */
				wissen_srm(PRIOREQUEST_AP, i, RIF_MAX_PRIO_REQUEST_AP); /* nog testen of kruispuntnummer overeenkomt? kan natuurlijk zijn dat er een 2e voertuig anderweg is van 1 naar 0 */
				wissen_srm(PRIOREQUEST_AP_corr, i, RIF_MAX_PRIO_REQUEST_AP);
				ris_update_ap_srm(PRIOREQUEST_AP, i, RIF_MAX_PRIO_REQUEST_AP); /* pak het laatste voertuig en zet dat op deze plek */
				ris_update_ap_srm(PRIOREQUEST_AP_corr, i, RIF_MAX_PRIO_REQUEST_AP);
				new_srm_data = TRUE;	/* Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */
			}
		}
		else {	/* En nu niet, dus het object is niet gevonden en bestaat dus nog niet. Zoeken naar een eerste vrije positie */
			i = 0;

			if (RIF_PRIOREQUEST_PB[read].valid) {
				while ((i < RIF_MAX_PRIO_REQUEST_AP) && strlen(PRIOREQUEST_AP[i].id)) {
					i++;
				}
				if (i < RIF_MAX_PRIO_REQUEST_AP) {
					/* Eerste lege opositie: aboneerwerk uitvoeren en vullen */ /* timestamp omzetten naar lifetime */
					PRIOREQUEST_AP[i] = RIF_PRIOREQUEST_PB[read];	/* Als het kruispuntnummer gelijk is, overschrijven ONGEACHT KRUISPUNTNUMMER */
					PRIOREQUEST_AP_corr[i] = RIF_PRIOREQUEST_PB[read];
					PRIOREQUEST_AP_corr[i].eta = 0;
					new_srm_data = TRUE;	/* Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */

					/* Verzenden SSM bericht na ontvangst */
					RIS_ssm_bericht(PRIOREQUEST_AP_corr[i].id, RIF_PRIORITIZATIONSTATE_REQUESTED);
					
				}
				else {
					/* Wanneer het buffer vol is, wordt het bericht weggegooid.*/
				}
			}
		}

	}
}

void lezen_buffer_cam(int *read, int *write)

{
	int h_read = (*read);
	int h_write = (*write);	/* Verzorgd door de PB */
	int i = 0;


	while ((h_read != h_write) && (i < RIF_ITSMAXAANTAL)) {
		i++;							/* Beveiliging per machinaslag op inlezen voertuigen */

		lees_itsstation_cam(h_read);

		h_read++;						/* Als eerste object index = 0 */
		if (h_read >= RIF_MAX_ITSSTATION_PB) h_read = 0;

		/* new_ris_data = TRUE;	   Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */
	}

	(*read) = h_read;
}

void lezen_buffer_srm(int *read, int *write)

{
	int h_read = (*read);
	int h_write = (*write);	/* Verzorgd door de PB */
	int i = 0;


	while ((h_read != h_write) && (i < RIF_ITSMAXAANTAL)) {
		i++;							/* Beveiliging per machinaslag op inlezen voertuigen */

		lees_itsstation_srm(h_read);

		h_read++;						/* Als eerste object index = 0 */
		if (h_read >= RIF_MAX_PRIOREQUEST_PB) h_read = 0;
	}

	(*read) = h_read;
}

mulv bep_voertuig_ingebied(count fc1,
	                      count fc2,
	                      count fc3,
	                      count fc4,
	                      count fc5,
	                      mulv voertuig, 
						  int onder, 
						  int boven, 
						  int pointer[], 
						  int masker)		/* Masker over rijstrook */
{
	count i = 0;
	count j = 0;
	mulv c = 0;

	for (i = 0; i < RIF_MAX_ITSSTATION_AP; i++) {
		pointer[i] = NG;
	}

	i = 0;

	while ((i < RIF_MAX_ITSSTATION_AP) && strlen(ITSSTATION_AP_corr[i].id)) {
		for (j = 0; j < RIF_MAXLANES; j++) {
			if ((1 << ITSSTATION_AP_corr[i].matches[j].lane) & masker) {
				if (strlen(ITSSTATION_AP_corr[i].matches[j].intersection) && ((fc1 != NG) && (strcmp(FC_code[fc1], ITSSTATION_AP_corr[i].matches[j].signalGroup) == 0)) || 
					                                                         ((fc2 != NG) && (strcmp(FC_code[fc2], ITSSTATION_AP_corr[i].matches[j].signalGroup) == 0)) || 
					                                                         ((fc3 != NG) && (strcmp(FC_code[fc3], ITSSTATION_AP_corr[i].matches[j].signalGroup) == 0)) || 
					                                                         ((fc4 != NG) && (strcmp(FC_code[fc4], ITSSTATION_AP_corr[i].matches[j].signalGroup) == 0)) || 
					                                                         ((fc5 != NG) && (strcmp(FC_code[fc5], ITSSTATION_AP_corr[i].matches[j].signalGroup) == 0))) {
					if ((ITSSTATION_AP_corr[i].matches[j].distance + ITSSTATION_AP_corr[i].length >= onder) && (ITSSTATION_AP_corr[i].matches[j].distance < boven) && (ITSSTATION_AP_corr[i].matches[j].distance > 0)) {	/* Lengte voertuig bij uitrijden meenemen */
						switch (ITSSTATION_AP_corr[i].stationType) {
						case RIF_STATIONTYPE_MOPED: {   if (voertuig & (RIS_MOTOR|RIS_FIETS)) {
										pointer[c] = i;
										c++;
						}
									break;
						}
						case RIF_STATIONTYPE_MOTORCYCLE: {   if (voertuig & RIS_MOTOR) {
											 pointer[c] = i;
											 c++;
						}
										 break;
						}
						case RIF_STATIONTYPE_PASSENGERCAR: {   if (voertuig & RIS_AUTO) {
											   pointer[c] = i;
											   c++;
						}
										   break;
						}
						case RIF_STATIONTYPE_PEDESTRIAN: {   if (voertuig & RIS_VOETGANGER) {
											 pointer[c] = i;
											 c++;
						}
										 break;
						}
						case RIF_STATIONTYPE_CYCLIST: {   if (voertuig & RIS_FIETS) {
										  pointer[c] = i;
										  c++;
						}
									  break;
						}
						case RIF_STATIONTYPE_BUS: {   if (voertuig & RIS_BUS) {
									  pointer[c] = i;
									  c++;
						}
								  break;
						}
						case RIF_STATIONTYPE_TRAM: {   if (voertuig & RIS_TRAM) {
									   pointer[c] = i;
									   c++;
						}
								   break;
						}
						case RIF_STATIONTYPE_LIGHTTRUCK: {   if (voertuig & RIS_VRACHTWAGEN) {
											 pointer[c] = i;
											 c++;
						}
										 break;
						}
						case RIF_STATIONTYPE_HEAVYTRUCK: {   if (voertuig & RIS_VRACHTWAGEN) {
											 pointer[c] = i;
											 c++;
						}
										 break;
						}
						case RIF_STATIONTYPE_TRAILER: {   if (voertuig & RIS_VRACHTWAGEN) {
							pointer[c] = i;
							c++;
						}
										  break;
						}
						case RIF_STATIONTYPE_SPECIALVEHICLES: {   if (ITSSTATION_AP_corr[i].role == RIF_VEHICLEROLE_EMERGENCY) {
							if (voertuig & RIS_HULPDIENST) {
								pointer[c] = i;
								c++;
							}
						}
						default :
										  break;
						}
						}
					}
				}
			}
		}
		i++;
	}
	return (c);
}

void beh_voertuig_ingebied_naar_abon(count index,		/* Verwijzing naar het AP_corr buffer */
	                                 count _matches, /* Verwijzing naar de map itsstation */
									 mulv masker)	/* Voertuigmasker */
{

	int i, fc = NG;
        for (i = 0; i < FCMAX; ++i) {
            if (strcmp(FC_code[i], ITSSTATION_AP_corr[index].matches[_matches].signalGroup) == 0) {
                fc = i;
                break;
            }
        }

	if (strlen(ITSSTATION_AP_corr[index].matches[_matches].intersection) && /* .intersection moet een inhoud hebben!! */
		(fc >= 0) && (fc < FCMAX)) { /* Geldig FC? */
		if (_ABON[fc].vag1_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].vag1_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].vag1_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				/* _ABON[fc].vag1[vag1_p]=index (beginnend met 0) */
				_ABON[fc].vag1[_ABON[fc].vag1_p] = index;
				_ABON[fc].vag1_p++;
			}
		}

		if (_ABON[fc].vag2_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].vag2_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].vag2_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].vag2[_ABON[fc].vag2_p] = index;
				_ABON[fc].vag2_p++;
			}
		}

		if (_ABON[fc].vag3_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].vag3_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].vag3_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].vag3[_ABON[fc].vag3_p] = index;
				_ABON[fc].vag3_p++;
			}
		}

		if (_ABON[fc].vag4_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].vag4_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].vag4_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].vag4[_ABON[fc].vag4_p] = index;
				_ABON[fc].vag4_p++;
			}
		}

		if (_ABON[fc].tover_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].tover_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].tover_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].tover[_ABON[fc].tover_p] = index;
				_ABON[fc].tover_p++;
			}
		}

		if (_ABON[fc].opvv_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].opvv_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].opvv_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].opvv[_ABON[fc].opvv_p] = index;
				_ABON[fc].opvv_p++;
			}
		}

		if (_ABON[fc].hlpd_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].hlpd_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].hlpd_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].hlpd[_ABON[fc].hlpd_p] = index;
				_ABON[fc].hlpd_p++;
			}
		}

		if (_ABON[fc].peloton_voertuig & masker) {

			if ((ITSSTATION_AP_corr[index].matches[_matches].distance + ITSSTATION_AP_corr[index].length >= _ABON[fc].peloton_o) &&
				(ITSSTATION_AP_corr[index].matches[_matches].distance < _ABON[fc].peloton_b) && (ITSSTATION_AP_corr[index].matches[_matches].distance > 0)) {

				_ABON[fc].peloton[_ABON[fc].peloton_p] = index;
				_ABON[fc].peloton_p++;
			}
		}

	}
}

void bep_voertuig_ingebied_naar_abon(void)
{
	count i = 0;
	count j = 0;

	/* alle bakjes nog legen en de teller */
	for (j = 0; j < FCMAX; j++) {
		for (i = 0; i < RIF_MAX_ITSSTATION_AP; i++) {
			_ABON[j].vag1[i] = NG;	/* Omdat Fick zonodig NG wilt hebben */
			_ABON[j].vag2[i] = NG;
			_ABON[j].vag3[i] = NG;
			_ABON[j].vag4[i] = NG;
			_ABON[j].tover[i] =  NG;
			_ABON[j].opvv[i] = NG;
			_ABON[j].hlpd[i] = NG;
			_ABON[j].peloton[i] = NG;
		}
		_ABON[j].vag1_p = 0;
		_ABON[j].vag2_p = 0;
		_ABON[j].vag3_p = 0;
		_ABON[j].vag4_p = 0;
		_ABON[j].tover_p = 0;
		_ABON[j].opvv_p = 0;
		_ABON[j].hlpd_p = 0;
		_ABON[j].peloton_p = 0;
	}

	/* Het volledige buffer wordt afgelopen, elk voertuig wordt in een bakje geworpen. De matches kan ook meer dan 1 FC hebben. Dan wordt deze index ook meegegeven;
	   Wat dan wel belangrijk is, is is de matches dit te toetsen aan de verkeerskunidge maatregel */
	i = 0;
	while ((i < RIF_MAX_ITSSTATION_AP) && strlen(ITSSTATION_AP_corr[i].id)) {
		for (j = 0; j < RIF_MAXLANES; j++) {
			if (strlen(ITSSTATION_AP_corr[i].matches[j].intersection)) {
				switch (ITSSTATION_AP_corr[i].stationType) {
				case RIF_STATIONTYPE_MOPED: {	beh_voertuig_ingebied_naar_abon(i, j, RIS_MOTOR);
					break;
				}
				case RIF_STATIONTYPE_MOTORCYCLE: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_MOTOR);
					break;
				}
				case RIF_STATIONTYPE_PASSENGERCAR: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_AUTO);
					break;
				}
				case RIF_STATIONTYPE_PEDESTRIAN: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_VOETGANGER);
					break;
				}
				case RIF_STATIONTYPE_CYCLIST: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_FIETS);
					break;
				}
				case RIF_STATIONTYPE_BUS: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_BUS); /* OV??? */
					break;
				}
				case RIF_STATIONTYPE_TRAM: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_TRAM);
					break;
				}
				case RIF_STATIONTYPE_LIGHTTRUCK: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_VRACHTWAGEN);
					break;
				}
				case RIF_STATIONTYPE_HEAVYTRUCK: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_VRACHTWAGEN);
					break;
				}
				case RIF_STATIONTYPE_TRAILER: {   beh_voertuig_ingebied_naar_abon(i, j, RIS_VRACHTWAGEN);
					break;
				}
				case RIF_STATIONTYPE_SPECIALVEHICLES: {   if (ITSSTATION_AP_corr[i].role == RIF_VEHICLEROLE_EMERGENCY) {
					beh_voertuig_ingebied_naar_abon(i, j, RIS_HULPDIENST);
				}
					break;
				}
				default :
					break;
				}
			}
		}
		i++;
	}
}


count RIS_id_cam(rif_string ObjectID)
{
	count i = NG;

	count j = 0;

	while ((j < RIF_MAX_ITSSTATION_AP) && (strcmp(ITSSTATION_AP_corr[j].id, ObjectID) != 0)) {
		j++;
	}
	if (j != RIF_MAX_ITSSTATION_AP) i = j;

	return (i);
}

count RIS_id_srm(rif_string ObjectID)
{
	count i = NG;

	count j = 0;

	while ((j < RIF_MAX_PRIO_REQUEST_AP) && (strcmp(PRIOREQUEST_AP[j].id, ObjectID) != 0)) {
		j++;
	}
	if (j != RIF_MAX_PRIO_REQUEST_AP) i = j;

	return (i);
}

mulv RIS_itsstation_srm(rif_string ObjectID)
{
	mulv i = NG;

	mulv j = 0;

	while ((j < RIF_MAX_PRIO_REQUEST_AP) && (strcmp(PRIOREQUEST_AP[j].itsStation, ObjectID) != 0)) {
		j++;
	}
	if ((j != RIF_MAX_PRIO_REQUEST_AP) && (PRIOREQUEST_AP[j].id[0] != '\0')) i = j;	/* 20180308 */

	return (i);
}

void bep_tovergroen_4_traffick(void)
{
	count i = 0;
	count j = 0;
	count k = 0;
	count l = 0;
	count m = 0;
	count n = 0;
	count p = 0;
	count q = 0;

	count index[RIF_MAX_ITSSTATION_AP] = { NG };



	for (i = 0; i < FCMAX; i++) {
		if (US_type[i] & MVT_type) {
			k = _ABON[i].tover_p;				/* Aantal tovergroeners vanuit het tover_abon */
			for (j = 0; j < k; j++) {
				index[j] = (count) _ABON[i].tover[j];	/* Onthouden welke indexi aanwezig zijn in dit traject */
			}

			m = 0;
			/* Update bestaande gegevens */
			while ((m < TOVERMAX) && strlen(TOVER_buffer[i][m].ObjectID)) {
				l = RIS_id_cam(TOVER_buffer[i][m].ObjectID);

				n = 0;
				while ((n < k) && (index[n] != l)) {	/* Bepaal index overeenkomst */
					n++;
				}

				if (n != k) {
					/* updaten data bij overeenkomstige index */
						TOVER_buffer[i][n].acceleration = ITSSTATION_AP_corr[l].acceleration;
						for (p = 0; p < RIF_MAXLANES; p++) {
							/* Het kan zijn dat een voertuig die over 2 lanes rijdt, 2 FCs kent */
							if (strlen(ITSSTATION_AP_corr[l].matches[p].intersection) && (strcmp(FC_code[i], ITSSTATION_AP_corr[l].matches[p].signalGroup) == 0)) {
								/* De afstand wordt eenoudigweg overschreven. Gemakshalve ga ik er vanuit de de stopstrepen gelijk liggen */
								TOVER_buffer[i][n].distance = ITSSTATION_AP_corr[l].matches[p].distance;
							}
						}
						TOVER_buffer[i][n].Speed = ITSSTATION_AP_corr[l].speed;
						TOVER_buffer[i][n].vlag = FALSE;
						index[n] = NG;	/* Object bestaat en verwerkt */
				}
				else {
					/* Het voertuig is verdwenen uit het traject, verwijderen uit dit TOVER buffer */
					q = m;
					while ((q < TOVERMAX - 1) && strlen(TOVER_buffer[i][q].ObjectID)) {
						TOVER_buffer[i][q] = TOVER_buffer[i][q + 1];
						q++;
					}
					if (q < TOVERMAX) {	/* Wissen laatste buffer na schuiven */
						TOVER_buffer[i][q].ObjectID[0] = '\0';
						TOVER_buffer[i][q].acceleration = 0;
						TOVER_buffer[i][q].distance = 0;
						TOVER_buffer[i][q].groenbewaking = 0;
						TOVER_buffer[i][q].Speed = 0;
						TOVER_buffer[i][q].vasthouden = 0;
						TOVER_buffer[i][q].vlag = FALSE;
					}
				}
				m++;
			}

			/* als er dan in index[] nog wat staat, toevoegen en de extra fick delen op 0 zetten */
			/* Er is sprake van een nieuw voertuig */
			m = 0;
			while ((m < TOVERMAX) && strlen(TOVER_buffer[i][m].ObjectID)) {
				m++;
			}
			if (m != TOVERMAX) {	/* Er is nog een plekje vrij */
				for (j = 0; j < k; j++) {
					if ((index[j] != NG) && (m < TOVERMAX)) {
						sprintf(TOVER_buffer[i][m].ObjectID, ITSSTATION_AP_corr[index[n]].id);
						TOVER_buffer[i][m].acceleration = ITSSTATION_AP_corr[index[n]].acceleration;
						for (p = 0; p < RIF_MAXLANES; p++) {
							if (strlen(ITSSTATION_AP_corr[index[n]].matches[p].intersection)) {
								TOVER_buffer[i][m].distance = ITSSTATION_AP_corr[index[n]].matches[p].distance;
							}
						}
						TOVER_buffer[i][m].groenbewaking = 0;
						TOVER_buffer[i][m].Speed = ITSSTATION_AP_corr[index[n]].speed;
						TOVER_buffer[i][m].vasthouden = 0;
						TOVER_buffer[i][m].vlag = TRUE;
						index[n] = NG;	/* Object bestaat en toegevoegd */
						m++;
					}
				}
			}
		}
	}
}

void bep_hlpd_4_traffick(void)
{
	count i = 0;
	count j = 0;
	count k = 0;
	count l = 0;
	count m = 0;
	count n = 0;
	count p = 0;
	count q = 0;

	count index[RIF_MAX_ITSSTATION_AP] = { NG };



	for (i = 0; i < FCMAX; i++) {
		for (m = 0; m < HLPD_MAX; m++) {
			HLPD_buffer[i][m].s_vlag = FALSE;
			HLPD_buffer[i][m].e_vlag = FALSE;
		}
		if (US_type[i] & MVT_type) {
			k = _ABON[i].hlpd_p;				/* Aantal hlpd-ers vanuit het hlpd_abon */
			for (j = 0; j < k; j++) {
				index[j] = (count) _ABON[i].hlpd[j];	/* Onthouden welke indexi aanwezig zijn in dit traject */
			}

			m = 0;
			/* Update bestaande gegevens */
			while ((m < HLPD_MAX) && strlen(HLPD_buffer[i][m].ObjectID)) {
				l = RIS_id_cam(HLPD_buffer[i][m].ObjectID);

				n = 0;
				while ((n < k) && (index[n] != l)) {	/* Bepaal index overeenkomst */
					n++;
				}

				if (n != k) {
					/* updaten data bij overeenkomstige index */
					HLPD_buffer[i][n].acceleration = ITSSTATION_AP_corr[l].acceleration;
					for (p = 0; p < RIF_MAXLANES; p++) {
						/* Het kan zijn dat een voertuig die over 2 lanes rijdt, 2 FCs kent */
						if (strlen(ITSSTATION_AP_corr[l].matches[p].intersection) && (strcmp(FC_code[i], ITSSTATION_AP_corr[l].matches[p].signalGroup) == 0)) {
							/* De afstand wordt eenoudigweg overschreven. Gemakshalve ga ik er vanuit de de stopstrepen gelijk liggen */
							HLPD_buffer[i][n].distance = ITSSTATION_AP_corr[l].matches[p].distance;
						}
					}
					HLPD_buffer[i][n].Speed = ITSSTATION_AP_corr[l].speed;
					HLPD_buffer[i][n].s_vlag = FALSE;
					index[n] = NG;	/* Object bestaat en verwerkt */
				}
				else {
					/* Het voertuig is verdwenen uit het traject, verwijderen uit dit HLPD buffer */
					q = m;
					while ((q < HLPD_MAX - 1) && strlen(HLPD_buffer[i][q].ObjectID)) {
						HLPD_buffer[i][q] = HLPD_buffer[i][q + 1];
						q++;
					}
					if (q < HLPD_MAX) {	/* Wissen laatste buffer na schuiven */
						HLPD_buffer[i][q].ObjectID[0] = '\0';
						HLPD_buffer[i][q].acceleration = 0;
						HLPD_buffer[i][q].distance = 0;
						HLPD_buffer[i][q].groenbewaking = 0;
						HLPD_buffer[i][q].Speed = 0;
						HLPD_buffer[i][q].vasthouden = 0;
						HLPD_buffer[i][q].s_vlag = FALSE;
						HLPD_buffer[i][q].e_vlag = TRUE;
					}
				}
				m++;
			}

			/* als er dan in index[] nog wat staat, toevoegen en de extra fick delen op 0 zetten */
			/* Er is sprake van een nieuw voertuig */
			m = 0;
			while ((m < HLPD_MAX) && strlen(HLPD_buffer[i][m].ObjectID)) {
				m++;
			}
			if (m != HLPD_MAX) {	/* Er is nog een plekje vrij */
				for (j = 0; j < k; j++) {
					if ((index[j] != NG) && (m < HLPD_MAX)) {
						sprintf(HLPD_buffer[i][m].ObjectID, ITSSTATION_AP_corr[index[n]].id);
						HLPD_buffer[i][m].acceleration = ITSSTATION_AP_corr[index[n]].acceleration;
						for (p = 0; p < RIF_MAXLANES; p++) {
							if (strlen(ITSSTATION_AP_corr[index[n]].matches[p].intersection)) {
								HLPD_buffer[i][m].distance = ITSSTATION_AP_corr[index[n]].matches[p].distance;
							}
						}
						HLPD_buffer[i][m].groenbewaking = 0;
						HLPD_buffer[i][m].Speed = ITSSTATION_AP_corr[index[n]].speed;
						HLPD_buffer[i][m].vasthouden = 0;
						HLPD_buffer[i][m].s_vlag = TRUE;
						index[n] = NG;	/* Object bestaat en toegevoegd */
						m++;
					}
				}
			}
		}
	}
}

void bep_opvv_4_traffick(void)
{
	count i = 0;
	count j = 0;
	count k = 0;
	count l = 0;
	count m = 0;
	count n = 0;
	count p = 0;
	count q = 0;

	count index[RIF_MAX_ITSSTATION_AP] = { NG };

	for (i = 0; i < FCMAX; i++) {
		for (m = 0; m < OPVV_MAX; m++) {
			OPVV_buffer[i][m].s_vlag = FALSE;
			OPVV_buffer[i][m].e_vlag = FALSE;
		}
		if (US_type[i] & MVT_type) {
			k = _ABON[i].opvv_p;				/* Aantal opvv-ers vanuit het opvv_abon */
			for (j = 0; j < k; j++) {
				index[j] = (count) _ABON[i].opvv[j];	/* Onthouden welke indexi aanwezig zijn in dit traject */
			}

			m = 0;
			/* Update bestaande gegevens */
			while ((m < OPVV_MAX) && strlen(OPVV_buffer[i][m].ObjectID)) {
				l = RIS_id_cam(OPVV_buffer[i][m].ObjectID);

				n = 0;
				while ((n < k) && (index[n] != l)) {	/* Bepaal index overeenkomst */
					n++;
				}

				if (n != k) {
					/* updaten data bij overeenkomstige index */
					OPVV_buffer[i][n].acceleration = ITSSTATION_AP_corr[l].acceleration;
					for (p = 0; p < RIF_MAXLANES; p++) {
						/* Het kan zijn dat een voertuig die over 2 lanes rijdt, 2 FCs kent */
						if (strlen(ITSSTATION_AP_corr[l].matches[p].intersection) && (strcmp(FC_code[i], ITSSTATION_AP_corr[l].matches[p].signalGroup) == 0)) {
							/* De afstand wordt eenoudigweg overschreven. Gemakshalve ga ik er vanuit de de stopstrepen gelijk liggen */
							OPVV_buffer[i][n].distance = ITSSTATION_AP_corr[l].matches[p].distance;
						}
					}
					OPVV_buffer[i][n].Speed = ITSSTATION_AP_corr[l].speed;
					/* OPVV_buffer[i][n].stiptheid = ITSSTATION_AP_corr[l].RoleAttributes.PublicTransport.punctuality; */
					OPVV_buffer[i][n].lijnnummer = ITSSTATION_AP_corr[l].roleAttributes.publicTransport.lineNr;
					OPVV_buffer[i][n].s_vlag = FALSE;
					index[n] = NG;	/* Object bestaat en verwerkt */
				}
				else {
					/* Het voertuig is verdwenen uit het traject, verwijderen uit dit OPVV buffer */
					q = m;
					while ((q < OPVV_MAX - 1) && strlen(OPVV_buffer[i][q].ObjectID)) {
						OPVV_buffer[i][q] = OPVV_buffer[i][q + 1];
						q++;
					}
					if (q < OPVV_MAX) {	/* Wissen laatste buffer na schuiven */
						OPVV_buffer[i][q].ObjectID[0] = '\0';
						OPVV_buffer[i][q].acceleration = 0;
						OPVV_buffer[i][q].distance = 0;
						OPVV_buffer[i][q].groenbewaking = 0;
						OPVV_buffer[i][q].Speed = 0;
						OPVV_buffer[i][q].stiptheid = 0;
						OPVV_buffer[i][q].lijnnummer = 0;
						OPVV_buffer[i][q].vasthouden = 0;
						OPVV_buffer[i][q].s_vlag = FALSE;
						OPVV_buffer[i][q].e_vlag = TRUE;
					}
				}
				m++;
			}

			/* als er dan in index[] nog wat staat, toevoegen en de extra fick delen op 0 zetten */
			/* Er is sprake van een nieuw voertuig */
			m = 0;
			while ((m < OPVV_MAX) && strlen(OPVV_buffer[i][m].ObjectID)) {
				m++;
			}
			if (m != OPVV_MAX) {	/* Er is nog een plekje vrij */
				for (j = 0; j < k; j++) {
					if ((index[j] != NG) && (m < OPVV_MAX)) {
						sprintf(OPVV_buffer[i][m].ObjectID, ITSSTATION_AP_corr[index[n]].id);
						OPVV_buffer[i][m].acceleration = ITSSTATION_AP_corr[index[n]].acceleration;
						for (p = 0; p < RIF_MAXLANES; p++) {
							if (strlen(ITSSTATION_AP_corr[index[n]].matches[p].intersection)) {
								OPVV_buffer[i][m].distance = ITSSTATION_AP_corr[index[n]].matches[p].distance;
							}
						}
						OPVV_buffer[i][m].groenbewaking = 0;
						OPVV_buffer[i][m].Speed = ITSSTATION_AP_corr[index[n]].speed;
						/* PVV_buffer[i][m].stiptheid = ITSSTATION_AP_corr[index[n]].RoleAttributes.PublicTransport.punctuality; */
						OPVV_buffer[i][m].lijnnummer = ITSSTATION_AP_corr[index[n]].roleAttributes.publicTransport.lineNr;
						OPVV_buffer[i][m].vasthouden = 0;
						OPVV_buffer[i][m].s_vlag = TRUE;
						index[n] = NG;	/* Object bestaat en toegevoegd */
						m++;
					}
				}
			}
		}
	}
}

mulv bep_hlpd_melding(mulv fc, mulv zwaai, mulv sirene)
{
  mulv hulp = 0;

  count m = 0;

  while ( (m < HLPD_MAX) ) {
    if ( strlen(HLPD_buffer[fc][m].ObjectID) && HLPD_buffer[fc][m].s_vlag &&
        (ITSSTATION_AP_corr[RIS_id_cam(HLPD_buffer[fc][m].ObjectID)].roleAttributes.lightBarActivated && (zwaai & ZWAAILICHT) || (zwaai == NG)) &&
        (ITSSTATION_AP_corr[RIS_id_cam(HLPD_buffer[fc][m].ObjectID)].roleAttributes.sirenActivated && (sirene & SIRENE) || (sirene == NG)) ) 
      hulp |= 1;
    if ( HLPD_buffer[fc][m].e_vlag &&
        (ITSSTATION_AP_corr[RIS_id_cam(HLPD_buffer[fc][m].ObjectID)].roleAttributes.lightBarActivated && (zwaai & ZWAAILICHT) || (zwaai == NG)) &&
        (ITSSTATION_AP_corr[RIS_id_cam(HLPD_buffer[fc][m].ObjectID)].roleAttributes.sirenActivated && (sirene & SIRENE) || (sirene == NG)) ) 
      hulp |= 2;	/* Bij uitmelding geen test op ObjectID, deze is net verwijderd uit het buffer */
    m++;
  }

  return hulp;

}

void RIS_ssm_bericht(rif_string ObjectID, int prioritizationState)
{
  count i = RIS_id_srm(ObjectID);

  if ( i != NG ) {
    sprintf(RIF_ACTIVEPRIO_AP[RIF_ACTIVEPRIO_AP_WRITE].id, ObjectID);
    RIF_ACTIVEPRIO_AP[RIF_ACTIVEPRIO_AP_WRITE].sequenceNumber = PRIOREQUEST_AP_corr[i].sequenceNumber;
    RIF_ACTIVEPRIO_AP[RIF_ACTIVEPRIO_AP_WRITE].prioState = prioritizationState;

    RIF_ACTIVEPRIO_AP_WRITE++;

    if ( RIF_ACTIVEPRIO_AP_WRITE >= RIF_MAX_ACTIVEPRIO_AP ) RIF_ACTIVEPRIO_AP_WRITE = 0;
  }
}

void RIS_behandeling(void)
{
#ifdef DYNNIQ
  ris_api_adapter_get_data();
#endif
  new_cam_data = FALSE;
  new_srm_data = FALSE;

  new_cam_data_count = 0;
  new_srm_data_count = 0;

  lezen_buffer_cam(&RIF_ITSSTATION_PB_READ, &RIF_ITSSTATION_PB_WRITE);
  lezen_buffer_srm(&RIF_PRIOREQUEST_PB_READ, &RIF_PRIOREQUEST_PB_WRITE);

 #if defined TESTOMGEVING | defined RIS_SIM_AUTOMAAT
	lezen_sim_buffer_cam(&SIM_STATION_READ, &SIM_STATION_WRITE);
	lezen_sim_buffer_srm(&SIM_PRIOREQUEST_READ, &SIM_PRIOREQUEST_WRITE);
 #endif

	beh_lifetime_cam(ITSSTATION_AP , ITSSTATION_AP_corr ,  3000); /* Verwijderen voertuig uit buffer wanneer deze  3000 milliseconden geen update kent */
	beh_lifetime_srm(PRIOREQUEST_AP, PRIOREQUEST_AP_corr, 65600); /* Verwijderen voertuig uit buffer wanneer deze 65600 milliseconden geen update kent */

	beh_correctie_buffer(ITSSTATION_AP, ITSSTATION_AP_corr); /* AP_corr bevat na aanroep per machineslag de gecorrigeerde afstand op basis van a en v */

	while ((new_cam_data_count < RIF_MAX_ITSSTATION_AP) && strlen(ITSSTATION_AP[new_cam_data_count].id)) new_cam_data_count++;
	while ((new_srm_data_count < RIF_MAX_PRIO_REQUEST_AP) && strlen(PRIOREQUEST_AP[new_srm_data_count].id)) new_srm_data_count++;

	/* Vullen van de ABONNEMENTEN */
	if (new_cam_data) bep_voertuig_ingebied_naar_abon();

	/* Vertaalslag van ABON naar tovergroenbuffer */
	bep_tovergroen_4_traffick();

	/* Vertaalslag van ABON naar OPVV/HLPD_buffer */
	bep_opvv_4_traffick();
	bep_hlpd_4_traffick();

	/* Vertaalslag van ABON naar peloton-buffer */
	/* voor fick-->bep_peloton_4_traffick(); */

#ifdef DYNNIQ
	ris_api_adapter_put_data();
#endif
}
