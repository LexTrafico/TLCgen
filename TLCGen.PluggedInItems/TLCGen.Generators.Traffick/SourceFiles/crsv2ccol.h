/*
 *
 * File:		crsv2ccol.c
 * Datum:		1 juni 2015
 * Versie:		1.00
 * Ontwerp:		M.J.M. Fick
 *
 * Bugfix:		Voorkom te vroeg inkomen voedende richting bij harde koppeling
 * Datum:		11 oktober 2015
 * Versie:		1.01
 * Ontwerp:		M.J.M. Fick
 *
 * Wijziging:           Geschikt gemaakt voor Traffick CCOL generator
 *                      Diverse verbeteringen op aangeven van Willem Kinzel en Patrick Huijskes
 * Datum:               31 augustus 2017
 * Versie:		2.00
 * Door:                M.J.M. Fick
 * Copyright:		Movensis (c) 2015-2017
 *
 * Wijziging:           procedure reset peloton buffers toegevoegd
 * Datum:               11 september 2017
 * Versie:		2.01
 * Door:                M.J.M. Fick
 * Copyright:		Movensis (c) 2015-2017
 *
 */

#define CRSV2CCOL_versie 200    /* versie CRSV2CCOL                     */

bool HFIX;			/* Fixatie toegestaan			*/
bool No_Sync;			/* Geen synchronisatie toestaan         */

mulv fts[FCMAX][5];		/* Def.fts.vtg.koppelingen		*/
mulv bbv[FCMAX][9];		/* Def.vtg.koppelingen [ progressief ]	*/
mulv gov[FCMAX][29];		/* Def.vtg.koppelingen [ gescheiden  ]	*/
mulv hki[FCMAX][21];		/* Def.hki.koppelingen [ serieel ]	*/
mulv ovp[FCMAX][19];		/* Def.OV-prioriteit			*/
mulv aant_fts;			/* Aantal aanwezige fts.vtg.koppelingen */
mulv aant_bbv,aant_gov;		/* Aantal aanwezige vtg.koppelingen	*/
mulv aant_hki;			/* Aantal aanwezige hki.koppelingen	*/

mulv XG[FCMAX];			/* Hulpwaarde groen voedende richting	*/
mulv XG_hki[FCMAX][FCMAX];	/* XG[] uitgesplitst naar koppeling     */
bool _XG[FCMAX];		/* Voedende richting hki heeft XG[]     */
bool _TTR_HK[FCMAX];		/* Volgrichting hki nog realiseren      */
mulv VA[FCMAX];                 /* Buffer aanvraag signaalgroep		*/
mulv RW_hki[FCMAX][FCMAX];	/* RW[] uitgesplitst naar koppeling	*/
mulv BW_hki[FCMAX][FCMAX];	/* Bewaak doorkoppeling fc1->fc2->fc3   */
bool BHK[FCMAX];		/* Blokkeer tbv inschakelen hki         */
bool BLW[FCMAX];		/* Blokkeer wachtstand aanvraag		*/
mulv KNP[FCMAX];		/* Klok na prioriteit			*/
mulv KNP_hki[FCMAX][FCMAX];	/* KNP[] uitgesplitst naar koppeling	*/
mulv FCA_hki[FCMAX][FCMAX];	/* FCA[] uitgesplitst naar koppeling    */
mulv TSPR[FCMAX];		/* Tijd tot start primaire realisatie	*/
mulv TEPR[FCMAX];		/* Tijd tot einde primaire realisatie	*/
mulv PEG[FCMAX];		/* Tijd tot einde groen bij confl.prio. */
mulv TTR_WV[FCMAX];		/* Tijd tot alternatieve realisatie	*/
mulv TTR_HK[FCMAX];		/* Tijd tot hki realisatie	        */
mulv TTR_HK_oud[FCMAX];		/* Tijd tot hki realisatie	        */
mulv TSV[FCMAX];		/* Tijd tot realisatie bij SVAS		*/
mulv VTTP[FCMAX];		/* TTP ten behoeve van vooruitschakelen	*/
mulv UTTP[FCMAX];		/* Uiterste tijd tot primaire realisatie*/
mulv TTU[FCMAX];		/* Tijd tot einde uitstel		*/
mulv TPVU[FCMAX];		/* Max.tijd voor einde.uitst. tbv prio. */
mulv TTPG[FCMAX];		/* Tijd tot primair.gr tbv WTvoorspeller*/
mulv SVS2[FCMAX];		/* VAS array -> start primair bij SVAS  */
mulv PVAS[FCMAX];		/* VAS array -> tbv prioriteit OV	*/
mulv PVAS_hki[FCMAX][FCMAX];	/* PVAS[] uitgesplitst naar koppeling   */
				/* alleen BIT1 is uitgesplitst          */
mulv PVS2[FCMAX];		/* VAS array -> tbv prioriteit OV	*/
mulv TTPR[FCMAX];		/* Tijd tot prioriteitsrealisatie       */
mulv CTTP[FCMAX];		/* Correctie TTP[] ivm OV.prioriteit	*/
mulv SDSN[FCMAX];		/* Tijd tot start doorsnijden           */
mulv EDSN[FCMAX];		/* Uiterste tijd tot terugkomen         */
mulv KPGR[FCMAX];		/* Klok primair groen                   */
mulv DSSP[FCMAX];		/* Tijd tot doorsnijden start OV.prio.  */
mulv DSEP[FCMAX];		/* Tijd tot doorsnijden einde OV.prio.  */
mulv TSP[FCMAX];		/* Tijd tot start OV.prio.mogelijkheid  */
mulv TEP[FCMAX];		/* Tijd tot einde OV.prio.mogelijkheid  */
mulv TEG_OV[FCMAX];		/* Tijd tot einde groen agv OV.prio.    */
mulv VS2M[FCMAX];		/* VAS array -> uiterste start primair  */

mulv _tts;			/* bijbehorende tijd tot stopstreep     */
mulv _vtg;			/* bijbehorende voertuignummer          */
mulv _vgr;			/* bijbehorende groentijd voor passage  */
mulv _pri;			/* bijbehorende prioriteitsvlag         */

mulv TTK_hki[FCMAX];		/* TTK door eerstvolgend conflict       */
mulv TEG_hki[FCMAX];		/* TEG van  lopend       conflict       */

bool WTV_FC[FCMAX];		/* Wachttijdvoorspeller aanwezig        */
bool CON_BUS[FCMAX];		/* Conflicterende BUS ingreep aktief    */
mulv HALT[FCMAX];		/* Halteer aflopen wachttijdvoorspeller */
mulv Adwt[FCMAX];		/* Aktuele tijdsduur sinds doven led    */
mulv Tdwt[FCMAX];		/* Tijdsduur tussen doven van twee leds */

bool HKV[FCMAX];                /* Hulpvlag harde interne koppeling     */

bool VAG_OP[FCMAX];             /* VAG optie */
mulv ETRG[FCMAX];               /* Einde garantie rood */

mulv PROGKEUZE_CS;              /* Programmakeuze CRSV                  */

/* Hulpfunctie bepaal (fictief) conflicterend gewenst groen */
/* -------------------------------------------------------- */
bool somfbk_crsv(mulv fc);


/* Hulpfunctie bepaal conflicterend groen */
/* -------------------------------------- */
bool somgk_crsv(mulv fc);


/* Hulpfunctie bepaal conflicterend RA[] van richting met wachttijdvoorspeller */
/* --------------------------------------------------------------------------- */
bool somwtk_crsv(mulv fc);


/* Hulpfuncties bepaal aanhouden meeverlenggroen toegestaan */
/* -------------------------------------------------------- */
bool hf_mvg_crsv(void);	/* Algemeen					*/
bool hfkp_mvg(void);	/* Specifiek voor "voedende" signaalgroepen 	*/
			/* ... binnen een koppeling			*/

bool hf_mvg_krp(mulv fc1,mulv fcn);	/* Algemeen bij meerdere krp"s	*/
					/* ... per regelaar		*/

/* Hulpfuncties leg conflicten vast t.b.v. CRSV-module */
/* --------------------------------------------------- */
void proc_fict_conflict(mulv fc1,mulv fc2);	/* Fictieve conflicten      */
void proc_door_conflict(mulv fc1,mulv fc2);	/* Doorschrijven conflicten */


/* Hulpfunctie initialiseer CRSV */
/* ----------------------------- */
void proc_init_crsv(mulv progkeuze);


/* Hulpfuncties definitie signaalgroep-koppelingen t.b.v. CRSV-module */
/* ------------------------------------------------------------------ */
void proc_def_vtg_fts(      /* Vtg.fts.koppeling            */
	mulv fc1,           /* Signaalgroep 1 (voetganger)  */
	mulv fc2,           /* Signaalgroep 2 (fietser 1)   */
	mulv fc3,           /* Signaalgroep 3 (fietser 2)   */
	mulv mre23,         /* Meerea. fietser 2 met 3      */
	mulv mre32);        /* Meerea. fietser 3 met 2      */

void proc_def_vtg_bb(       /* BB.vtg.koppeling             */
	mulv fc1,           /* Signaalgroep 1               */
	mulv fc2,           /* Signaalgroep 2               */
	mulv kpt12,         /* KoppelTIJD eg1 -> eg2        */
	mulv gkg,           /* Geen KoppelGarantie bij alt  */
	mulv fc3,           /* Gekoppelde fietsrichting 3   */
	mulv fc4,           /* Gekoppelde fietsrichting 4   */
	mulv mre34,         /* Meerea. fietser 3 met 4      */
	mulv mre43,         /* Meerea. fietser 4 met 3      */
	mulv gkg_pri);	    /* Geen KoppelGarantie bij prio */

void proc_def_vtg_go(       /* GO voetgangerskoppeling      */
	mulv fc1,           /* Signaalgroep 1               */
	mulv fc2,           /* Signaalgroep 2	            */
	mulv kpa12,         /* Kop.aanvr. sg1 -> sg2        */
	mulv kpa21,         /* Kop.aanvr. sg2 -> sg1        */
	mulv kpt12,         /* KoppelTIJD sg1 -> eg2        */
	mulv kpt21,         /* KoppelTIJD sg2 -> eg1        */
	mulv gkg,           /* Geen KoppelGarantie bij alt  */
	mulv fc3,           /* Gekoppelde fietsrichting 3   */
	mulv fc3v,          /* Volgrichting van fietser 3   */
	mulv mr33v,         /* Naloop nooit zonder voeding  */
	mulv kpt3s,         /* Koppeltijd vanaf start fc3   */
	mulv kpt3e,         /* Koppeltijd vanaf einde fc3   */
	mulv hfk3e,         /* (HF)start naloop einde fc3   */
	mulv fc4,           /* Gekoppelde fietsrichting 4   */
	mulv fc4v,          /* Volgrichting van fietser 4   */
	mulv mr44v,         /* Naloop nooit zonder voeding  */
	mulv kpt4s,         /* Koppeltijd vanaf start fc4   */
	mulv kpt4e,         /* Koppeltijd vanaf einde fc4   */
	mulv hfk4e,         /* (HF)start naloop einde fc4   */
	mulv mre34,         /* Meerea. fietser 3 met 4      */
	mulv mre43,         /* Meerea. fietser 4 met 3      */
	mulv gkg_pri,       /* Geen KoppelGarantie bij prio */
                            /* 3e vtg is de middelste vtg!  */
	mulv vtg3,          /* Signaalgroep 3e voetganger   */
	mulv kpa31,         /* Kop.aanvr. sg3 -> sg1        */
	mulv kpa32,         /* Kop.aanvr. sg3 -> sg2        */
	mulv kpt13,         /* KoppelTIJD sg1 -> eg3        */
	mulv kpt23,         /* KoppelTIJD sg2 -> eg3        */
	mulv kpt31,         /* KoppelTIJD sg3 -> eg1        */
	mulv kpt32);        /* KoppelTIJD sg3 -> eg2        */

void proc_def_hki(		/* HKI.koppeling  [serieel] */
	mulv fc1,               /* Signaalgroep 1 [voedend] */
	mulv fc2,               /* Signaalgroep 2 [volg sg1]*/
	mulv kptvs12,           /* Max.voorst.sg1 -> sg2    */
	mulv kptsg12,           /* KoppelTIJD sg1 -> eg2    */
	mulv kpteg12,           /* KoppelTIJD eg1 -> eg2    */
	mulv kptdm12,           /* KoppelTIJD dm2 na hki    */
	mulv de11,              /* Det.1 sg1 tbv start.teg  */
	mulv de12,              /* Det.2 sg1 tbv start.teg  */
	mulv de13,              /* Det.3 sg1 tbv start.teg  */
	mulv fc3,               /* Signaalgroep 3 [volg sg2]*/
	mulv kptvs23,           /* Max.voorst.sg2 -> sg3    */
	mulv kptsg23,           /* KoppelTIJD sg2 -> eg3    */
	mulv kpteg23,           /* KoppelTIJD eg2 -> eg3    */
	mulv kptdm23,           /* KoppelTIJD dm3 na hki    */
	mulv de21,              /* Det.1 sg2 tbv start.teg  */
	mulv de22,              /* Det.2 sg2 tbv start.teg  */
	mulv de23,              /* Det.3 sg2 tbv start.teg  */
	mulv hf_hki);           /* Hulpfunctie hki aktief   */


/* Hulpfunctie definitie OV-prioriteit t.b.v. CRSV-module */
/* ------------------------------------------------------ */
void proc_def_ov(               /* Definitie OV-prioriteit           */
	mulv fc,                /* Signaalgroep                      */
	mulv deftts,            /* Default tijd      tot  stopstreep */
	mulv defvgr,            /* Default groentijd voor stopstreep */
	mulv uitbew,            /* Uitmeldbewaking                   */
	mulv maxveu);           /* Max.tijd voor einde uitstel       */


/* Hulpfuncties bepaal aktief stappenraster -> keuze en status */
/* ----------------------------------------------------------- */
void proc_crsv_progkeuze_slaaf( /* Programma-keuze SLAAF    */
	mulv tt1,              /* Index timer tijdvertr. omschakelen  */
	mulv mckode);          /* Index geheugenelement met kode complexcoordinator */

void proc_bepaal_MKODE_crsv(void);
void proc_crsv_progkeuze_master(/* Programma-keuze MASTER	*/
	mulv tt1);		/* Tijdvertr. omschakelen	*/

void proc_crsv_parm(		/* Aktuele instellingen		*/
	mulv ctyd,		/* Cyclustijd			*/
	mulv insc,		/* Inschakelpunt		*/
	mulv omsc,		/* Omschakelpunt		*/
	mulv sync);		/* Synchronisatiepunt		*/

void proc_crsv_stappenraster(	/* Stat.stap.raster SLAAF	*/
	bool ssync);		/* Start Koppelsignaal 5 "IN"	*/

void proc_crsv_master_raster(	/* Stat.stap.raster MASTER	*/
	mulv tt_s);		/* Tijdsduur sync.signaal	*/


/* Hulpfunctie t.b.v. resetten variabelen bij start CRSV-module */
/* ------------------------------------------------------------ */
void proc_reset_crsv_start(void);


/* Hulpfuncties t.b.v. in- en uitmelden en sorteren OV-meldingen */
/* ------------------------------------------------------------- */
void proc_ov_inm(mulv _sg);	/* Signaalgroep                */
void proc_ov_uit(mulv _sg);	/* Signaalgroep                */

void proc_buf_ov_crsv(          /* Fik170831                   */
	mulv _sg,               /* Signaalgroep                */
	mulv priv,              /* prioriteit bus te vroeg     */
	mulv prit,              /* prioriteit bus op tijd      */
	mulv pril,              /* prioriteit bus te laat      */
        mulv awt1,              /* aanwezigheidsduur 1e bus    */
        mulv awt2,              /* aanwezigheidsduur 2e bus    */
        mulv awt3,              /* aanwezigheidsduur 3e bus    */
        mulv stp1,              /* stiptheidscode 1e bus       */
        mulv stp2,              /* stiptheidscode 2e bus       */
        mulv stp3);             /* stiptheidscode 3e bus       */

void proc_sorteer_bus(
	mulv fc,		/* Signaalgroep                */
	mulv nr1,		/* Volgnummer 1                */
	mulv nr2);		/* Volgnummer 2                */


/* Hulpfuncties t.b.v. signaalgroepsturing CRSV-module         */
/* ---------------------------------------------------         */
void proc_cyc_crsv(		/* Bepaal cyclische realisatie */
	mulv fc,		/* Signaalgroep                */
	mulv cvas1,		/* PRM[cprim##]                */
	mulv cvas2);		/* NG -> enkel voorbereid      */

void proc_cyc_crsv_dvm(		/* Bepaal cyclische realisatie */
	mulv fc,		/* Signaalgroep                */
	mulv cdvm1,		/* PRM[cdvm##]                 */
	mulv cdvm2);		/* NG -> enkel voorbereid      */

void proc_akt_crsv(		/* Bepaal aktuele prim.gebieden*/
	mulv fc,		/* Signaalgroep                */
	mulv kapov,		/* PRM kapov                   */
	mulv tl1,		/* PRM Einde uitstel           */
	mulv tl2,		/* PRM Start primair           */
	mulv tl3,		/* PRM Start verleng           */
	mulv tl4,		/* PRM Einde primair           */
	mulv tl5,		/* PRM Einde groen             */
	mulv abm,		/* PRM Afbreekmethode          */
	mulv pri);		/* PRM Prio.buiten prim.gebied */

void proc_min_prim_crsv(void);	/* Min.gebied tbv OV-ingrepen  */

void proc_min_prim_vtg(		/* Corr.agv fts.vtg.oversteken */
	mulv fc1,               /* Signaalgroep 1              */
	mulv fc2,               /* Signaalgroep 2              */
	mulv fc3,               /* Signaalgroep 3              */
	mulv fc4,               /* Signaalgroep 4              */
	mulv fc5,               /* Signaalgroep 5              */
	mulv fc6,               /* Signaalgroep 6              */
	mulv fc7);              /* Signaalgroep 7              */

void proc_min_prim_hki(void);	/* Corr.agv seriele koppeling  */

void init_kapm_crsv(void);	/* Initialiseer ALTM[]-KAPM[]  */
void proc_corr_kapm(void);	/* Cor.ALTM-KAPM ivm oversteken*/

void proc_kapm_crsv_bb(		/* Bep.ALTM-KAPM voor bb.vtg.  */
	mulv fc1,		/* Signaalgroep 1	       */
	mulv fc2,		/* Signaalgroep 2	       */
	mulv kpt12,		/* KoppelTIJD eg1 -> eg2       */
	mulv gkpg_a,		/* Geen kop.garantie bij alt   */
	mulv gkpg_p);		/* Geen kop.garantie bij prio  */

void proc_kapm_crsv_go(		/* Bep.ALTM-KAPM voor go.vtg.  */
	mulv fc1,		/* Signaalgroep 1	       */
	mulv fc2,		/* Signaalgroep 2	       */
	mulv kpt12,		/* KoppelTIJD sg1 -> eg2       */
	mulv kpt21,		/* KoppelTIJD sg2 -> eg1       */
	mulv gkpg_a,		/* Geen kop.garantie bij alt   */
	mulv fc3,		/* Gekoppelde fietser 1        */
	mulv fc4,		/* Gekoppelde fietser 1 volg   */
	mulv kpt3s,		/* Koppeltijd vanaf start fc3  */
	mulv kpt3e,		/* Koppeltijd vanaf einde fc3  */
	mulv fc5,		/* Gekoppelde fietser 2        */
	mulv fc6,		/* Gekoppelde fietser 2 volg   */
	mulv kpt5s,		/* Koppeltijd vanaf start fc5  */
	mulv kpt5e,		/* Koppeltijd vanaf einde fc5  */
	mulv gkpg_p,		/* Geen kop.garantie bij prio  */
	mulv vtg3,              /* Signaalgroep 3e voetganger  */
	mulv kpt13,             /* KoppelTIJD sg1 -> eg3       */
	mulv kpt23,             /* KoppelTIJD sg2 -> eg3       */
	mulv kpt31,             /* KoppelTIJD sg3 -> eg1       */
	mulv kpt32);            /* KoppelTIJD sg3 -> eg2       */

void proc_open_prim_appl(	/* Bep.openhouden prim.gebied  */
	mulv fc,		/* Signaalgroep	               */
	mulv hf_op_pr);		/* Hulpfunctie op.prim.gebied  */

void proc_open_prim(		/* Corr.open.prim.fts.vtg.kop  */
	mulv fc1,		/* Signaalgroep 1              */
	mulv fc2,		/* Signaalgroep 2              */
	mulv fc3,		/* Signaalgroep 3              */
	mulv fc4,		/* Signaalgroep 4              */
	mulv fc5,		/* Signaalgroep 5              */
	mulv fc6,		/* Signaalgroep 6              */
	mulv fc7);		/* Signaalgroep 7              */

void proc_open_hki_prim(void);	/* Corr.open.prim.hki.kop.     */

void init_bteg_crsv(void);      /* Bepaal TEG[] bij start CRSV */
void proc_bteg_crsv(void);      /* Bepaal TEG[]                */

void init_bbtegcrsv(            /* Init TEG[] voor bb.vtg.kop. */
	mulv fc1,               /* Signaalgroep 1              */
	mulv fc2,               /* Signaalgroep 2              */
	mulv kpt12);            /* KoppelTIJD eg1 -> eg2       */

void corr_bbtegcrsv(            /* Corr.TEG[] voor bb.vtg.kop. */
	mulv fc1,               /* Signaalgroep 1              */
	mulv fc2,               /* Signaalgroep 2              */
	mulv kpt12);            /* KoppelTIJD eg1 -> eg2       */

void init_gotegcrsv(            /* Init TEG[] voor go.vtg.kop. */
	mulv fc1,               /* Signaalgroep 1              */
	mulv fc2,               /* Signaalgroep 2              */
	mulv kpa12,             /* Kop.aanvr. sg1 -> sg2       */
	mulv kpa21,             /* Kop.aanvr. sg2 -> sg1       */
	mulv kpt12,             /* KoppelTIJD sg1 -> eg2       */
	mulv kpt21,             /* KoppelTIJD sg2 -> eg1       */
	mulv fc3,		/* Gekoppelde fietser 1        */
	mulv fc4,		/* Gekoppelde fietser 1 volg   */
	mulv kpt3s,		/* Koppeltijd vanaf start fc3  */
	mulv kpt3e,		/* Koppeltijd vanaf einde fc3  */
	mulv fc5,		/* Gekoppelde fietser 2        */
	mulv fc6,		/* Gekoppelde fietser 2 volg   */
	mulv kpt5s,		/* Koppeltijd vanaf start fc5  */
	mulv kpt5e,		/* Koppeltijd vanaf einde fc5  */
	mulv vtg3,              /* Signaalgroep 3e voetganger  */
	mulv kpa31,             /* Kop.aanvr. sg3 -> sg1       */
	mulv kpa32,             /* Kop.aanvr. sg3 -> sg2       */
	mulv kpt13,             /* KoppelTIJD sg1 -> eg3       */
	mulv kpt23,             /* KoppelTIJD sg2 -> eg3       */
	mulv kpt31,             /* KoppelTIJD sg3 -> eg1       */
	mulv kpt32);            /* KoppelTIJD sg3 -> eg2       */

void corr_gotegcrsv(            /* Corr.TEG[] voor go.vtg.kop. */
	mulv fc1,               /* Signaalgroep 1              */
	mulv fc2,               /* Signaalgroep 2              */
	mulv kpt12,             /* KoppelTIJD sg1 -> eg2       */
	mulv kpt21,             /* KoppelTIJD sg2 -> eg1       */
	mulv fc3,		/* Gekoppelde fietser 1        */
	mulv fc4,		/* Gekoppelde fietser 1 volg   */
	mulv kpt3s,		/* Koppeltijd vanaf start fc3  */
	mulv kpt3e,		/* Koppeltijd vanaf einde fc3  */
	mulv fc5,		/* Gekoppelde fietser 2        */
	mulv fc6,		/* Gekoppelde fietser 2 volg   */
	mulv kpt5s,		/* Koppeltijd vanaf start fc5  */
	mulv kpt5e,		/* Koppeltijd vanaf einde fc5  */
	mulv vtg3,              /* Signaalgroep 3e voetganger  */
	mulv kpa31,             /* Kop.aanvr. sg3 -> sg1       */
	mulv kpa32,             /* Kop.aanvr. sg3 -> sg2       */
	mulv kpt13,             /* KoppelTIJD sg1 -> eg3       */
	mulv kpt23,             /* KoppelTIJD sg2 -> eg3       */
	mulv kpt31,             /* KoppelTIJD sg3 -> eg1       */
	mulv kpt32);            /* KoppelTIJD sg3 -> eg2       */

void corr_hkitegcrsv(void);     /* Corr.TEG[] voor hki.kop.    */

void proc_bttr_crsv(void);      /* Bepaal TTR[]                */

void proc_cttr_crsv_vtg_fts(	/* Corr.TTR[] -> vtg.fts.koppelingen */
	mulv fc1,		/* Signaalgroep 1 (voetganger)       */
	mulv fc2,		/* Signaalgroep 2 (voetganger)       */
	mulv fc3,		/* Gekoppelde fietsrichting 1	     */
	mulv fc4,		/* Gekoppelde fietsrichting 2	     */
	mulv fc5,		/* Gekoppelde fietsrichting 3	     */
	mulv fc6,		/* Gekoppelde fietsrichting 4	     */
	mulv fc7);		/* Signaalgroep 3e voetganger        */

void proc_cttr_crsv_hki(void);	/* Corr.TTR[] voor hki.koppeling     */

void proc_bttp_crsv(void);      /* Bepaal TTP[]                      */
void proc_prio_crsv(void);      /* Bepaal prioriteit openbaar vervoer*/
void proc_bcob_crsv(void);	/* Bepaal bit confl.prio.in aantocht */
void proc_btsp_crsv(void);	/* (her)Bereken TSP[]                */
void proc_bdsn_crsv(mulv fc);   /* Bepaal DSSP[] en DSEP[]           */

void proc_ctep_crsv_hki(void);  /* Corr.TEP[] voor hki.koppeling     */
void proc_ctsp_crsv_hki(void);	/* Corr.TSP[] voor hki.koppeling     */
void proc_cdssp_crsv_hki(void);	/* Corr.DSSP[] voor hki.koppeling    */
void proc_cpvas_crsv_hki(void);	/* Corr.PVAS[] voor hki.koppeling    */
void proc_ctegov_crsv_hki(void);/* Corr.TEG_OV[] voor hki.koppeling  */
void proc_knp_crsv_hki(void);	/* Bepaal klok na prioriteit         */

void proc_cttp_crsv_vtg_fts(	/* Corr.TTP[] en CTTP voor vtg.fts.  */
	mulv fc1,		/* Signaalgroep 1 (vtg)              */
	mulv fc2,		/* Signaalgroep 2 (vtg)              */
	mulv fc3,		/* Gekoppelde.fietsr. 1              */
	mulv fc4,		/* Gekoppelde.fietsr. 2              */
	mulv fc5,		/* Gekoppelde.fietsr. 3              */
	mulv fc6,		/* Gekoppelde.fietsr. 4              */
	mulv fc7);              /* Signaalgroep 3e voetganger        */

void proc_cttp_crsv_hki(void);	/* Corr.TTP[] voor seriele koppeling */
void proc_prim_crsv(void);      /* Bepaal primaire realisatie        */

void proc_covs_crsv_vtg_fts(    /* Corr.overslag bij vtg.fts.kop.    */
	mulv fc1,               /* Signaalgroep 1                    */
	mulv fc2);              /* Signaalgroep 2                    */

void proc_cprm_crsv_vtg_fts(	/* Corr.prim. -> vtg.fts.koppelingen */
	mulv fc1,               /* Signaalgroep 1                    */
	mulv fc2,               /* Signaalgroep 2                    */
	mulv fc3,               /* Signaalgroep 3                    */
	mulv fc4,               /* Signaalgroep 4                    */
	mulv fc5,               /* Signaalgroep 5                    */
	mulv fc6,               /* Signaalgroep 6                    */
	mulv fc7);              /* Signaalgroep 7                    */

void proc_altt_crsv(void);      /* Bepaal BAR[] door CRSV-module     */

void proc_altt_appl(		/* Bepaal BAR[] door applicate	     */
	mulv fc,		/* Signaalgroep			     */
	mulv hfbar);		/* Hulpfunctie BAR		     */

void proc_cbar_crsv_vtg_fts(	/* Corr.BAR[] -> vtg.fts.koppelingen */
	mulv fc1,		/* Signaalgroep 1 (voetganger)       */
	mulv fc2,		/* Signaalgroep 2 (voetganger)       */
	mulv fc3,		/* Gekoppelde fietsrichting 1	     */
	mulv fc4,		/* Gekoppelde fietsrichting 2	     */
	mulv fc5,		/* Gekoppelde fietsrichting 3	     */
	mulv fc6,		/* Gekoppelde fietsrichting 4	     */
	mulv fc7);              /* Signaalgroep 3e voetganger        */

void proc_TTK_hki_crsv(void);	/* Bepaal hulpwaarden voor inrijden  */
void proc_cbar_crsv_hki(void);  /* Corr.BAR[] voor hki.koppelingen   */
void proc_altr_crsv(void);      /* Bepaal alternatieve realisatie    */

void proc_vtg_fts_crsv(         /* Vtg.fts.koppeling        */
	mulv fc1,               /* Signaalgroep 1 (vtg)     */
	mulv fc2,               /* Signaalgroep 2 (fts 1)   */
	mulv fc3,               /* Signaalgroep 3 (fts 2)   */
	mulv mre23,             /* Meerea.fts 1 met fts 2   */
	mulv mre32) ;           /* Meerea.fts 2 met fts 1   */

void proc_bb_vtg_crsv(          /* BB.vtg.kop.inc.fiets     */
	mulv fc1,               /* Signaalgroep 1           */
	mulv fc2,               /* Signaalgroep 2           */
	mulv kpt12,             /* KoppelTIJD eg1 -> eg2    */
	mulv fc3,               /* Gekoppelde fietser 1     */
	mulv fc4,               /* Gekoppelde fietser 2     */
	mulv mre34,             /* Meerea. fietser 1 met 2  */
	mulv mre43);            /* Meerea. fietser 2 met 1  */

void proc_go_vtg_crsv(          /* GO.vtg.kop.inc.fiets        */
	mulv fc1,               /* Signaalgroep 1              */
	mulv fc2,               /* Signaalgroep 2              */
	mulv kpa12,             /* Koppelaanvraag sg1 -> sg2   */
	mulv kpa21,             /* Koppelaanvraag sg2 -> sg1   */
	mulv kpt12,             /* KoppelTIJD     sg1 -> eg2   */
	mulv kpt21,             /* KoppelTIJD     sg2 -> eg1   */
	mulv fc3,               /* Gekoppelde fietser 1        */
	mulv fc4,               /* Gekoppelde fietser 1 volg   */
	mulv mre34,             /* Naloop nooit zonder voeding */
	mulv kpt3s,             /* Koppeltijd vanaf start fc3  */
	mulv kpt3e,             /* Koppeltijd vanaf einde fc3  */
	mulv hfk3e,             /* (HF)start naloop einde fc3  */
	mulv fc5,               /* Gekoppelde fietser 2        */
	mulv fc6,               /* Gekoppelde fietser 2 volg   */
	mulv mre56,             /* Naloop nooit zonder voeding */
	mulv kpt5s,             /* Koppeltijd vanaf start fc5  */
	mulv kpt5e,             /* Koppeltijd vanaf einde fc5  */
	mulv hfk5e,             /* (HF)start naloop einde fc5  */
	mulv mre35,             /* Meerea. fietser 1 met 2     */
	mulv mre53,             /* Meerea. fietser 2 met 1     */
	mulv vtg3,              /* Signaalgroep 3e voetganger  */
	mulv kpa31,             /* Kop.aanvr. sg3 -> sg1       */
	mulv kpa32,             /* Kop.aanvr. sg3 -> sg2       */
	mulv kpt13,             /* KoppelTIJD sg1 -> eg3       */
	mulv kpt23,             /* KoppelTIJD sg2 -> eg3       */
	mulv kpt31,             /* KoppelTIJD sg3 -> eg1       */
	mulv kpt32);            /* KoppelTIJD sg3 -> eg2       */

void proc_hki_crsv(void);	/* HKI.koppeling  [serieel] */
void proc_wsgr_fca_crsv(void);	/* Bep.fic.wachtstand.gr.a. */

void proc_wsgr_crsv(            /* Afw.wachtgroen           */
	mulv fc,                /* Signaalgroep             */
	mulv wgr);              /* SCH(WGR)                 */

void proc_mvgr_crsv(            /* Afw.meeverlenggroen      */
	mulv fc,                /* Signaalgroep             */
	mulv mvg);              /* SCH(MVG)                 */

void proc_reset_crsv_einde(void);

void proc_vas_res_pel(          /* Fik100314     */
	count ap1,              /* awt peloton 1 */
	count ap2,              /* awt peloton 2 */
	count ap3);             /* awt peloton 3 */

void proc_ttpg_crsv(void);      /* Bepaal TTPG[] tbv WTvoorspeller */
void proc_wt_voorspeller_crsv(void); /* Wachttijdvoorspeller CRSV  */

/* Hulpfunctie koppeling CRSV-module -> CRS */
/* ---------------------------------------- */
void proc_crsv_to_ccol(void);   /* Bestuur SG-opties        */

/* Hulpfuncties t.b.v. combinering functie-aanroepen */
/* ------------------------------------------------- */
void CRSV_FC_main_1(void);
void CRSV_FC_main_2(void);
