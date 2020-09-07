/* DECLARATIE RIS SIMULATIE FUNCTIES */
/* ========================== */
/*
 *	Huijskes VRT B.V.
 *	14 Oktoberplein 6
 *	Overloon
 *
 *
 *	Module: functies t.b.v. simulatie van RIS voertuigen in test omgeving
 *
 *	Filenaam        : ris_sim.c
 *  Ontwerp					: P.N.M.S. Huijskes
 *	Datum           : ..-..-2017
 *	CCOL						: versie 9.0 en hoger,  Generator PNH versie ?? en hoger
 *
	Revisie:
		  -12-2017 P.N.M.S. Huijskes:  opzet
		09-01-2018 M. Scherjon: simulatie berichten apart van pb buffers 
		11-01-2018 M. Scherjon: srm- uitmelding berichten toegevoegd
		19-01-2018 M. Scherjon: Versie 1.0
		08-03-2018 M. Scherjon: RIS_sim_gewenst() toegevoegd
		
*/

  #include "cif.inc"			/* i.v.m. CIF_KLOK[] */
  #include "control.h"    /* i.v.m. TE */

  #include "stdlib.h"
  #include "limits.h"

  #define SIMVOERTUIGMAX 20			/* Dit buffer moet ruimte hebben voor alle voertuigsoorten */

   #define RIF_MAX_SIM_ITSSTATION   100   /* Grootte buffer interface t.b.v. CAM berichten procesbesturing    */
   #define RIF_MAX_SIM_PRIOREQUEST   25   /*                          t.b.v. SRM berichten procesbesturing    */

struct _sim {
	rif_string sim_kruispunt[SIMVOERTUIGMAX];
	int sim_random[SIMVOERTUIGMAX];
	int sim_offset_random[SIMVOERTUIGMAX];
	float sim_snelheid[SIMVOERTUIGMAX];
	float sim_d_snelheid[SIMVOERTUIGMAX];
	float sim_afstand[SIMVOERTUIGMAX];
	float sim_d_afstand[SIMVOERTUIGMAX];
	int sim_lane[SIMVOERTUIGMAX][RIF_MAXLANES];
	int aantal[SIMVOERTUIGMAX];							/* Aantal gevulde lanes */

};

struct _sim SIM_object[FCMAX][RIF_MAXLANES];

#define SIMBUFFMAX FCMAX * 100
struct Rif_ItsStation ITSSTATION_SIM[SIMBUFFMAX];
struct Rif_PrioritizationRequest PRIOREQUEST_SIM[SIMBUFFMAX];

struct Rif_ItsStation SIM_STATION[RIF_MAX_SIM_ITSSTATION];
struct Rif_PrioritizationRequest SIM_PRIOREQUEST[RIF_MAX_SIM_PRIOREQUEST];

long objectid = 500;

rif_int	SIM_STATION_WRITE;
rif_int	SIM_STATION_READ;
rif_int	SIM_PRIOREQUEST_WRITE;
rif_int	SIM_PRIOREQUEST_READ;


void RIS_sim_init(void)
{
	int i = 0;
	int j = 0;
	int n = 0;
	int m = 0;

	for (i = 0; i < SIMVOERTUIGMAX; i++) {
		for (j = 0; j < FCMAX; j++) {
			for (m = 0; m < RIF_MAXLANES; m++) {
				SIM_object[j][m].sim_kruispunt[i][0] = '\0'; /* vervanging t.b.v.  warning */
				SIM_object[j][m].sim_random[i] = 0;
				SIM_object[j][m].sim_offset_random[i] = 0;
				SIM_object[j][m].sim_snelheid[i] = 0;
				SIM_object[j][m].sim_d_snelheid[i] = 0;
				SIM_object[j][m].sim_afstand[i] = 0;
				SIM_object[j][m].sim_d_afstand[i] = 0;
				for (n = 0; n < RIF_MAXLANES; n++) {
					SIM_object[j][m].sim_lane[i][n] = NG;
				}
				SIM_object[j][m].aantal[i] = 0;
			}
		}
	}

	SIM_STATION_WRITE = 0;
	SIM_STATION_READ = 0;
	SIM_PRIOREQUEST_WRITE = 0;
	SIM_PRIOREQUEST_READ = 0;

	for (i = 0; i < RIF_MAX_SIM_ITSSTATION; i++) {
		wissen_cam(ITSSTATION_SIM, i, RIF_MAX_SIM_ITSSTATION);
	}
	for (i = 0; i < RIF_MAX_SIM_PRIOREQUEST; i++) {
		wissen_srm(PRIOREQUEST_SIM, i, RIF_MAX_SIM_PRIOREQUEST);
	}

}

void RIS_sim_init_fc(mulv fc, rif_string krp, mulv voertuig_type, int random, int offset_random, int snelheid, int d_snelheid, int afstand, int d_afstand, int lane_bit)
{
	int m = 0;
	int n = 0;
	int lane = SIM_object[fc][0].aantal[voertuig_type];

	while (offset_random > 3600) offset_random -= 3600;

	if (lane < RIF_MAXLANES) {
		sprintf(SIM_object[fc][lane].sim_kruispunt[voertuig_type], krp);
		SIM_object[fc][lane].sim_random[voertuig_type] = random;
		SIM_object[fc][lane].sim_offset_random[voertuig_type] = offset_random;
		SIM_object[fc][lane].sim_snelheid[voertuig_type] = (float)(snelheid / 3.6);
		SIM_object[fc][lane].sim_d_snelheid[voertuig_type] = (float)(d_snelheid / 3.6);
		if (SIM_object[fc][lane].sim_d_snelheid[voertuig_type] * 10 > SIM_object[fc][lane].sim_snelheid[voertuig_type]) SIM_object[fc][lane].sim_d_snelheid[voertuig_type] = SIM_object[fc][lane].sim_snelheid[voertuig_type] / 10;
		SIM_object[fc][lane].sim_afstand[voertuig_type] = (float)(afstand);
		SIM_object[fc][lane].sim_d_afstand[voertuig_type] = (float)(d_afstand);
		n = 0;
		for (m = 0; m < RIF_MAXLANES; m++) {
			while ((n < RIF_MAXLANES) && (((1 << n) & lane_bit) == 0)) {
				//if (((1 << n) & lane_bit) > 0) 
				n++;
			}
			if (((1 << n) & lane_bit) > 0) {
				SIM_object[fc][lane].sim_lane[voertuig_type][m] = n;
			}
			n++;
		}
		SIM_object[fc][0].aantal[voertuig_type]++;
	}
}

void RIS_sim_generatie(void)
{
	float upper = 0;
	int i = 0;
	int j = 0;
	long k = 0;
	int l = 0;
	int m = 0;
	int n = 0;

	if (TE) {
		upper = (float)3600 * 10 / TE; /* maal 10 i.v.m. aantal slagen per seconden en daarmee de rand() kans */
		for (i = 0; i < SIMVOERTUIGMAX; i++) {
			for (j = 0; j < FCMAX; j++) {
				for (m = 0; m < RIF_MAXLANES; m++) {
					k = (long)(rand() * upper / RAND_MAX);
					if ((SIM_object[j][m].sim_offset_random[i] + SIM_object[j][m].sim_random[i] <= 3600) && (k >= SIM_object[j][m].sim_offset_random[i]) && (k < (SIM_object[j][m].sim_offset_random[i] + SIM_object[j][m].sim_random[i])) ||
						(SIM_object[j][m].sim_offset_random[i] + SIM_object[j][m].sim_random[i] >  3600) && (k >= SIM_object[j][m].sim_offset_random[i]) && (k < (SIM_object[j][m].sim_offset_random[i] + SIM_object[j][m].sim_random[i] - 3600))) {
						l = 0;
						while ((l < SIMBUFFMAX) && strlen(ITSSTATION_SIM[l].id)) {
							l++;
						}
						if (l < SIMBUFFMAX) {
							sprintf(ITSSTATION_SIM[l].id, "%ld", objectid);

							k = (long)(2 * SIM_object[j][m].sim_d_snelheid[i] * rand() / RAND_MAX);
							ITSSTATION_SIM[l].speed = SIM_object[j][m].sim_snelheid[i] - SIM_object[j][m].sim_d_snelheid[i] + k;
							ITSSTATION_SIM[l].Speed_org = ITSSTATION_SIM[l].speed;
							if (ITSSTATION_SIM[l].Speed_org < 0) {
								l = l;
							}
							ITSSTATION_SIM[l].acceleration = 0;

							k = (long)(2 * SIM_object[j][m].sim_d_afstand[i] * rand() / RAND_MAX);
							for (n = 0; n < RIF_MAXLANES; n++) {
								if (SIM_object[j][m].sim_lane[i][n] >= 0) {
									ITSSTATION_SIM[l].matches[n].distance = SIM_object[j][m].sim_afstand[i] + k;
									ITSSTATION_SIM[l].locationTime = /*TIME19702001 + */time_2_nu();
									//if (lane4sim[m] >= 0) {	/* Alleen bij een geldige lane vullen vanaf positie 0 */
									sprintf(ITSSTATION_SIM[l].matches[n].intersection, SIM_object[j][m].sim_kruispunt[i]);
									//}
									ITSSTATION_SIM[l].stationType = i;
									sprintf(ITSSTATION_SIM[l].matches[n].signalGroup, FC_code[j]);
									ITSSTATION_SIM[l].matches[n].lane = SIM_object[j][m].sim_lane[i][n]; //hier de shift zetten met n
								}
							}
							ITSSTATION_SIM[l].valid = 1;

							switch (ITSSTATION_SIM[l].stationType) {
							case RIF_STATIONTYPE_BUS: {   
								ITSSTATION_SIM[l].roleAttributes.publicTransport.companyNr = 3;
								ITSSTATION_SIM[l].roleAttributes.publicTransport.embarkation = TRUE;
								ITSSTATION_SIM[l].roleAttributes.publicTransport.journeyNr = 1234;
								ITSSTATION_SIM[l].roleAttributes.publicTransport.lineNr = 102;
								//ITSSTATION_SIM[l].roleAttributes.publicTransport.punctuality = 2000;
								ITSSTATION_SIM[l].roleAttributes.publicTransport.serviceNr = 1;

								PRIOREQUEST_SIM[l].valid = 1;
								sprintf(PRIOREQUEST_SIM[l].id, "%s_%s", ITSSTATION_SIM[l].id, ITSSTATION_SIM[l].matches[0].intersection);
								sprintf(PRIOREQUEST_SIM[l].itsStation, ITSSTATION_SIM[l].id);
								sprintf(PRIOREQUEST_SIM[l].signalGroup, FC_code[j]);
								PRIOREQUEST_SIM[l].punctuality = 3599;	/* Te laat */
								PRIOREQUEST_SIM[l].role = RIF_VEHICLEROLE_PUBLICTRANSPORT;
								PRIOREQUEST_SIM[l].requestType = RIF_PRIORITYREQUESTTYPE_REQUEST;
								sprintf(PRIOREQUEST_SIM[l].intersection, ITSSTATION_SIM[l].matches[0].intersection);
								break;
							}
							case RIF_STATIONTYPE_SPECIALVEHICLES: {
								ITSSTATION_SIM[l].roleAttributes.lightBarActivated = TRUE;
								ITSSTATION_SIM[l].roleAttributes.sirenActivated = TRUE;
								ITSSTATION_SIM[l].role = RIF_VEHICLEROLE_EMERGENCY;

								PRIOREQUEST_SIM[l].valid = 1;
								sprintf(PRIOREQUEST_SIM[l].id, "%s_%s", ITSSTATION_SIM[l].id, ITSSTATION_SIM[l].matches[0].intersection);
								sprintf(PRIOREQUEST_SIM[l].itsStation, ITSSTATION_SIM[l].id);
								sprintf(PRIOREQUEST_SIM[l].signalGroup, FC_code[j]);
								PRIOREQUEST_SIM[l].role = RIF_VEHICLEROLE_EMERGENCY;
								PRIOREQUEST_SIM[l].subrole = RIF_VEHICLESUBROLE_EMERGENCY;
								PRIOREQUEST_SIM[l].requestType = RIF_PRIORITYREQUESTTYPE_REQUEST;
								sprintf(PRIOREQUEST_SIM[l].intersection, ITSSTATION_SIM[l].matches[0].intersection);
								break;
							}
							default: {
								break;
							}
							}


							/* voertuiglengten */
							switch (ITSSTATION_SIM[l].stationType) {
							case RIF_STATIONTYPE_PASSENGERCAR: {   
								ITSSTATION_SIM[l].length = 5.5;
								break;
							}
							case RIF_STATIONTYPE_HEAVYTRUCK:   {   
								ITSSTATION_SIM[l].length = 18.0;
								break;
							}
							case RIF_STATIONTYPE_BUS:          {   
								ITSSTATION_SIM[l].length = 18.0;
								break;
							}
							case RIF_STATIONTYPE_SPECIALVEHICLES: {  
								if (ITSSTATION_SIM[l].role == RIF_VEHICLEROLE_EMERGENCY) {
									ITSSTATION_SIM[l].length = 18.0;
								}
								break;
							}
							default: {
								break;
							}
							}

						}
						objectid++;
						if ( objectid > 100000 ) objectid = 500;
					}
				}
			}
		}
	}
}


void RIS_sim_behandel_cam(void)
{
	int i = 0;
	int j = 0;
	count hfc = 0;
	uint64_t dt;
	float tmp_dt = 1.5000 ;

	for (i = 0; i < SIMVOERTUIGMAX; i++) {
		if (strlen(ITSSTATION_SIM[i].id)) {
			if (ITSSTATION_SIM[i].valid == 0) {
				ITSSTATION_SIM[i].id[0] = '\0'; /* vervanging t.b.v.  warning */
			}
			else {
				dt = time_2_nu() - ITSSTATION_SIM[i].locationTime;
				if (dt >= 1000) {
					tmp_dt = (float) (dt / 1000.000);
					for (j = 0; j < RIF_MAXLANES; j++) {
						if (strlen(ITSSTATION_SIM[i].matches[j].intersection)) {

							ITSSTATION_SIM[i].acceleration = 0;
							for (hfc = 0; hfc < FC_MAX; hfc++) {
								if (strcmp(FC_code[hfc], ITSSTATION_SIM[i].matches[j].signalGroup) == 0) {
									if (!G[hfc] && (ITSSTATION_SIM[i].matches[j].distance > 10.000) && (ITSSTATION_SIM[i].matches[j].distance < ITSSTATION_SIM[i].speed * tmp_dt /* reactietijd */ + (float) 0.500 * ITSSTATION_SIM[i].acceleration * tmp_dt * tmp_dt /* ITSSTATION_SIM[i].speed / 1.500*/ /* negatieve versnelling */)) {
										ITSSTATION_SIM[i].acceleration = -1.500;
									}
									if ((G[hfc]  || (ITSSTATION_SIM[i].matches[j].distance < 0.000)) && (ITSSTATION_SIM[i].speed < ITSSTATION_SIM[i].Speed_org)) {
										ITSSTATION_SIM[i].acceleration = 1.500;
									}
								}
							}


							ITSSTATION_SIM[i].speed += (float) 0.500 * ITSSTATION_SIM[i].acceleration * tmp_dt * tmp_dt;	/* klopt dit zo wel?? */



							if (ITSSTATION_SIM[i].speed > ITSSTATION_SIM[i].Speed_org) {
								ITSSTATION_SIM[i].speed = ITSSTATION_SIM[i].Speed_org;
							}
							if (ITSSTATION_SIM[i].speed < 0) {
								ITSSTATION_SIM[i].speed = 0;
								ITSSTATION_SIM[i].acceleration = 0;
							}

							ITSSTATION_SIM[i].matches[j].distance -= (float)ITSSTATION_SIM[i].speed * tmp_dt - (float) 0.500 * (float)ITSSTATION_SIM[i].acceleration * tmp_dt * tmp_dt;
							ITSSTATION_SIM[i].locationTime = /*TIME19702001 + */time_2_nu();

							/* @@ MS   bepaal uitmelding SRM */
							if ( ITSSTATION_SIM[i].matches[j].distance < -30 ) {
								/* voor alle voertuigen die een SRM actief hebben en afstand meer als 30m na de stopstreep */
								if ( strlen(PRIOREQUEST_SIM[i].id) && (PRIOREQUEST_SIM[i].valid == 1) ) {
									PRIOREQUEST_SIM[i].requestType = RIF_PRIORITYREQUESTTYPE_CANCELLATION;   /*  UITMELDING */
								}
							}

							if (ITSSTATION_SIM[i].matches[j].distance < -100) {
								ITSSTATION_SIM[i].matches[j].distance = -100;
/*								for (m = 0; m < RIF_MAXLANES; m++) { */
/*									ITSSTATION_SIM[i].matches[m].intersection[0] = '\0'; */ /* vervanging t.b.v.  warning */
/*									ITSSTATION_SIM[i].matches[m].distance = 0; */
/*									ITSSTATION_SIM[i].matches[m].lane = 0; */
/*									ITSSTATION_SIM[i].matches[m].offset = 0; */
/*									ITSSTATION_SIM[i].matches[m].signalGroup[0] = '\0'; */
/*								} */
								ITSSTATION_SIM[i].valid = 0;
							}
						}
					}
					if (ITSSTATION_SIM[i].valid != 0) {
/*					ITSSTATION_SIM[i].locationTime = 0; */
						/* schrijf naar RIS-SIM-interface */
						SIM_STATION[SIM_STATION_WRITE] = ITSSTATION_SIM[i];
						/* verplaatst RIS-SIM-interface pointer */
						SIM_STATION_WRITE++;
						if (SIM_STATION_WRITE >= RIF_MAX_SIM_ITSSTATION) SIM_STATION_WRITE = 0;

						if (ITSSTATION_SIM[i].valid == 0) ITSSTATION_SIM[i].id[0] = '\0';
					}
				}
			}
			/* ITSSTATION_SIM[i].locationTime += TE * 100; */
		}
	}
}

void RIS_sim_behandel_srm(void)
{
	int i = 0;

	for (i = 0; i < SIMVOERTUIGMAX; i++) {
		if (strlen(PRIOREQUEST_SIM[i].id)) {
			if (PRIOREQUEST_SIM[i].valid == 0) {
				PRIOREQUEST_SIM[i].id[0] = '\0'; /* vervanging t.b.v.  warning */
			}
		}

		if ( strlen(PRIOREQUEST_SIM[i].id) && (PRIOREQUEST_SIM[i].valid == 1) && (PRIOREQUEST_SIM[i].requestType != RIF_PRIORITYREQUESTTYPE_NONE) ) {
			/* een requestType NONE => wordt NIET verzonden */

			PRIOREQUEST_SIM[i].eta = time_2_nu(); /* waarom ? */

			/* schrijf naar RIS-SIM-interface */
			SIM_PRIOREQUEST[SIM_PRIOREQUEST_WRITE] = PRIOREQUEST_SIM[i];
			/* verplaatst RIS-SIM-interface pointer */
			SIM_PRIOREQUEST_WRITE++;
			if (SIM_PRIOREQUEST_WRITE >= RIF_MAX_SIM_PRIOREQUEST) SIM_PRIOREQUEST_WRITE = 0;

			if ((PRIOREQUEST_SIM[i].valid == 1) && (PRIOREQUEST_SIM[i].requestType == RIF_PRIORITYREQUESTTYPE_REQUEST)) {
				PRIOREQUEST_SIM[i].requestType = RIF_PRIORITYREQUESTTYPE_NONE; /* na een inmelding op NONE zodat maar ��n bericht wordt verzonden */
			}
			/* na een uitmeldbericht pas wissen */
			if ( (PRIOREQUEST_SIM[i].valid == 1) && (PRIOREQUEST_SIM[i].requestType == RIF_PRIORITYREQUESTTYPE_CANCELLATION)) {
				/* Eenmalig schrijven */
/*				PRIOREQUEST_SIM[i].id[0] = '\0'; */
				PRIOREQUEST_SIM[i].valid = 0;
			}
		}

	}
}

void RIS_sim_behandel(void)
{
	RIS_sim_behandel_cam();
	RIS_sim_behandel_srm();
}


#if defined TESTOMGEVING | defined RIS_SIM_AUTOMAAT

void lees_sim_station_cam(int read)
{
	/* nog even geen rekening houden met 2e kruispunt */

	/* Bestaat het ID reeds? */
	int i = 0;
	int j = 0;

/* 	of de j loop er gewoon buiten halen???? */
	if (strlen(SIM_STATION[read].id) &&
		valid_objectid(SIM_STATION[read].id)) {	/* Er moet een geldige identificatie zijn */

		/* Allowed characters: �a-z� (ASCII 97 through 122), �A-Z� (ASCII 65 through 90), �0-9�, �_� (underscore, ASCII 95) and ��� (hyphen, ASCII 45). */
		i = 0;

		while ((i < RIF_MAX_ITSSTATION_AP) && (strcmp(SIM_STATION[read].id, ITSSTATION_AP[i].id) != 0)) {
			i++;
		}
		if (i < RIF_MAX_ITSSTATION_AP) {	/* Er is een gelijk ObjectID gevonden */
			if (SIM_STATION[read].valid) {
				for (j = 0; j < RIF_MAXLANES; j++) {
					if (strlen(SIM_STATION[read].matches[j].intersection) > 0) {
						if (strcmp(SIM_STATION[read].matches[j].intersection, ITSSTATION_AP[i].matches[j].intersection) == 0) {
							/* abon gegevens aanroepen    timestapm omzetten naar lifetime */
							ITSSTATION_AP[i] = SIM_STATION[read];	/* Als het kruispuntnummer gelijk is, overschrijven ONGEACHT KRUISPUNTNUMMER */
							ITSSTATION_AP_corr[i] = SIM_STATION[read];
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
							if ((strcmp(SIM_STATION[read].matches[j].intersection, SYSTEM_ITS) == 0)) {
								/* abon gegevens aanroepen   timestapm omzetten naar lifetime */
								ITSSTATION_AP[i] = SIM_STATION[read];
								ITSSTATION_AP_corr[i] = SIM_STATION[read];
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

			if (SIM_STATION[read].valid) {
				while ((i < RIF_MAX_ITSSTATION_AP) && strlen(ITSSTATION_AP[i].id)) {
					i++;
				}
				if (i < RIF_MAX_ITSSTATION_AP) {
					/* Eerste lege opositie: aboneerwerk uitvoeren en vullen */ /* timestamp omzetten naar lifetime */
					ITSSTATION_AP[i] = SIM_STATION[read];
					ITSSTATION_AP_corr[i] = SIM_STATION[read];
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

void lees_sim_station_srm(int read)
{
	/* nog even geen rekening houden met 2e kruispunt */

	/* Bestaat het ID reeds? */
	int i = 0;

	/*	of de j loop er gewoon buiten halen???? */
	if (strlen(SIM_PRIOREQUEST[read].id) &&
		valid_objectid(SIM_PRIOREQUEST[read].id)) {	/* Er moet een geldige identificatie zijn */

		/* Allowed characters: �a-z� (ASCII 97 through 122), �A-Z� (ASCII 65 through 90), �0-9�, �_� (underscore, ASCII 95) and ��� (hyphen, ASCII 45). */
		i = 0;

		while ((i < RIF_MAX_PRIO_REQUEST_AP) && (strcmp(SIM_PRIOREQUEST[read].id, PRIOREQUEST_AP[i].id /*SYSTEM_ITS*/) != 0)) {
			i++;
		}
		if (i < RIF_MAX_PRIO_REQUEST_AP) {	/* Er is een gelijk ObjectID gevonden */
			if (SIM_PRIOREQUEST[read].valid) {
				PRIOREQUEST_AP[i] = SIM_PRIOREQUEST[read];	/* Als het kruispuntnummer gelijk is, overschrijven ONGEACHT KRUISPUNTNUMMER */
				PRIOREQUEST_AP_corr[i] = SIM_PRIOREQUEST[read];
				PRIOREQUEST_AP_corr[i].eta = 0;
				/* Er is niet sprake van een nieuw object */
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

			if (SIM_PRIOREQUEST[read].valid) {
				while ((i < RIF_MAX_PRIO_REQUEST_AP) && strlen(PRIOREQUEST_AP[i].id)) {
					i++;
				}
				if (i < RIF_MAX_PRIO_REQUEST_AP) {
					/* Eerste lege opositie: aboneerwerk uitvoeren en vullen */ /* timestamp omzetten naar lifetime */
					PRIOREQUEST_AP[i] = SIM_PRIOREQUEST[read];	/* Als het kruispuntnummer gelijk is, overschrijven ONGEACHT KRUISPUNTNUMMER */
					PRIOREQUEST_AP_corr[i] = SIM_PRIOREQUEST[read];
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

void lezen_sim_buffer_cam(int *read, int *write)

{
	int h_read = (*read);
	int h_write = (*write);	/* Verzorgd door de RIS_SIM */
	int i = 0;


	while ((h_read != h_write) && (i < RIF_ITSMAXAANTAL)) {
		i++;							/* Beveiliging per machinaslag op inlezen voertuigen */

		lees_sim_station_cam(h_read);

		h_read++;						/* Als eerste object index = 0 */
		if (h_read >= RIF_MAX_SIM_ITSSTATION) h_read = 0;

		/* new_ris_data = TRUE;	 Er is een nieuw bericht gelezen --> update wenselijk van de _ABON */ 
	}

	(*read) = h_read;
}

void lezen_sim_buffer_srm(int *read, int *write)

{
	int h_read = (*read);
	int h_write = (*write);	/* Verzorgd door de PB */
	int i = 0;


	while ((h_read != h_write) && (i < RIF_ITSMAXAANTAL)) {
		i++;							/* Beveiliging per machinaslag op inlezen voertuigen */

		lees_sim_station_srm(h_read);

		h_read++;						/* Als eerste object index = 0 */
		if (h_read >= RIF_MAX_SIM_PRIOREQUEST) h_read = 0;
	}

	(*read) = h_read;
}

 #endif



bool RIS_sim_gewenst(count i_sim, bool init)
{
	/* 
	de functie geeft als return waarde TRUE of FALSE.
	hiermee kan de aanroep van de RIS_sim_generatie() worden IN of UIT geschakeld.

	indien voor isrissim de waarde NG wordt opgegeven, dan wordt altijd naar het 2e argument gekeken.
	indien er wel een ingang is gedefinieerd, dan geld init alleen tijdens opstarten van de exe.

	Voorbeeld voor in de k...reb.c :

#if defined TESTOMGEVING | defined RIS_SIM_AUTOMAAT
 #ifdef isrissim
		if ( RIS_sim_gewenst(isrissim, FALSE) ) 
 #endif
			RIS_sim_generatie();
		RIS_sim_behandel();
#endif

maar kan ook zo:

		if ( RIS_sim_gewenst(NG, SCH[schris_sim]) )
			RIS_sim_generatie();

of zo:

 #ifdef isrissim
		if ( RIS_sim_gewenst(isrissim, SCH[schris_sim]) ) 
 #else
		if ( RIS_sim_gewenst(NG, SCH[schris_sim]) )
 #endif
			RIS_sim_generatie();

	*/


	bool knip = FALSE;
	bool sappl = TRUE;
	bool result = FALSE;

	/* Zet dit in commentaar als je NIET wil knipperen ! */
	knip = (CIF_KLOK[CIF_TSEC_TELLER]%15)<2; /* knipperen snelheid 0,75 Hz (1,5 sec) en 'puls' lengte 0,2 sec */
	knip &= (TE < 5); /* niet meer knipperen tijdens High Speed run ( TE is dan 5 ) */


	if ( init == FALSE ) sappl = FALSE;

	if ( i_sim != NG ) {
#ifndef AUTOMAAT
		if ( SAPPLPROG ) {
			IS_key[i_sim] = sappl;
		}
		if ( IS_key[i_sim]&0x01 ) {
			if ( knip ) {
/*					IS_key[i_sim] |= 0x02; */ /* Paars */
/*					IS_key[i_sim] |= 0x04; */ /* Licht Rood */
					IS_key[i_sim] |= 0x08; /* Roze */
			}
			else {
				IS_key[i_sim] &= 0x01; /* licht blauw */
			}
		}
		else 
			IS_key[i_sim] = FALSE;
#endif

		if ( CIF_IS[i_sim] ) result = TRUE;
	}
	else {
		result = sappl;
	}

	return (result);

}
