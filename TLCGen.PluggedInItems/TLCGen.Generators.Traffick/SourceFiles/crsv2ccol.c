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
 * Wijziging:           verbeterslag in OV-verklikking op bedienpaneel
 * Datum:               8 maart 2018
 * Versie:		2.02
 * Door:                M.J.M. Fick
 * Copyright:		Movensis (c) 2015-2018
 *
 */

/* -------------------------------------------------------------------------------------------------------- */
/* Header file                                                                                              */
/* -------------------------------------------------------------------------------------------------------- */
  #include "crsv2ccol.h"                /* procedure header file                                            */
  
/* FUNCTIE BEPAAL (FICTIEF) CONFLICTEREND GEWENST GROEN */
/* ==================================================== */

bool somfbk_crsv(                                  /* Fik150101 */
	mulv fc)
{
 mulv  j;
 count k;

 for (j=0; j<GKFC_MAX[fc]; j++) {             /* primaire conflicten */
   k = TO_pointer[fc][j];
   if (B[k] || RA[k] || G[k] && (!MG[k] || RW[k])) return TRUE;
 }
 for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) {  /* fictieve conflicten */
   k = TO_pointer[fc][j];
   if (B[k] || RA[k]) return TRUE;
 }
 return FALSE;
}

/* FUNCTIE BEPAAL CONFLICTEREND GROEN */
/* ================================== */

bool somgk_crsv(                              /* Fik150101 */
	mulv fc)
{
 mulv  j;
 count k;

 for (j=0; j<GKFC_MAX[fc]; j++) {             /* primaire conflicten */
   k = TO_pointer[fc][j];
   if (G[k]) return TRUE;
 }
 return FALSE;
}

/* FUNCTIE BEPAAL CONFLICTEREND RA[] VAN RICHTING MET WACHTTIJDVOORSPELLER */
/* ======================================================================= */

bool somwtk_crsv(                             /* Fik170831 */
	mulv fc)
{
 mulv  j;
 count k;

 for (j=0; j<GKFC_MAX[fc]; j++) {             /* primaire conflicten */
   k = TO_pointer[fc][j];
   if (RA[k] && (XG[k]&BIT7)) return TRUE;
 }
 return FALSE;
}

/* FUNCTIES BEPAAL MEEVERLENGGROEN TOEGESTAAN */
/* ========================================== */

/* Hulpfunctie meeverlenggroen in geval van 1 kruispunt */
/* ---------------------------------------------------- */

bool hf_mvg_crsv(void)                        /* Fik150101 */
{
 mulv i;

 for (i=0; i<FCMAX; i++) {
   if (R[i] && VA[i] || XG[i] || G[i] && (!WG[i] || !(VA[i]&BIT5)) && (!MG[i] || RW[i])
		     || MG[i] && MK[i]) return TRUE;
 }
 return FALSE;
}

/* Hulpfunctie meeverlenggroen in geval van meerdere kruispunten */
/* ------------------------------------------------------------- */

bool hf_mvg_krp(                             /* Fik150101 */
	mulv fc1,       /* Signaalgroep 1 van kruispunt x */
	mulv fcn)       /* Signaalgroep n van kruispunt x */
{
 mulv i;

 for (i=fc1; i<=fcn; i++) {
  if (i<FCMAX) {	/* Beveiliging tegen array-overschrijding */
   if (R[i] && VA[i] || XG[i] || G[i] && (!WG[i] || !(VA[i]&BIT5)) && (!MG[i] || RW[i])
		     || MG[i] && MK[i]) return TRUE;
  }
 }
 return FALSE;
}

/* Hulpfunctie mvg voor voedende signaalgroepen binnen een koppeling */
/* ----------------------------------------------------------------- */

bool hfkp_mvg(void)                          /* Fik150101 */
{
 mulv i;

 for (i=0; i<FCMAX; i++) {
   if (R[i] && VA[i] || XG[i] || G[i] && (!WG[i] || !(VA[i]&BIT5)) && (!MG[i] || RW[i]) && !(YM[i]&BIT2)
		     || MG[i] && MK[i]) return TRUE;
 }
 return FALSE;
}

/* FUNCTIE VERSNELD DOORLOPEN STAPPENRASTER BIJ LOKAAL BEDRIJF */
/* =========================================================== */

bool snel_lokaal(void)                       /* Fik150101 */
{
 mulv  i,j;
 count k;
 mulv  prog_keuze = 20;
 bool  wens = FALSE;
 bool  doorschakel = TRUE;
 mulv  tijd_tot_groen = 0;

 if (PROGKEUZE_CS != NG) prog_keuze = PRM[PROGKEUZE_CS];

 HFIX = FIXATIE;

 if ((prog_keuze == 11) && !HFIX && !SPR && !SVAS && !OMSCH && AKTPRG) { 
   for (i=0; i<FCMAX; i++) {
     if (R[i] && VA[i]) wens = TRUE;
     if (G[i] && (!MG[i] || MK[i]) && (TTP[i] > 0)) wens = TRUE;
     if (G[i] && (TTP[i] ==0) && (VG1[i] || (MK[i]&BIT2) || VAG_OP[i] || !(VA[i]&BIT5))) doorschakel = FALSE;
     if (G[i] && (TTPR[i]==0)) doorschakel = FALSE;
     if (G[i] && SG[i])        doorschakel = FALSE;

     if (!G[i] && (GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i])) {
	if (TTR[i]>=TTP[i]) doorschakel = FALSE;
     }
     if (!G[i] && (GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i] || (TTPR[i]<255))) {
	if (TTR[i]>=TTPR[i]) doorschakel = FALSE;
     }
   }
   if (wens && doorschakel) {
     for (i=0; i<FCMAX; i++) {
       if (G[i]) {
         for (j=0; j<GKFC_MAX[i]; j++) {             /* primaire conflicten */
           k = TO_pointer[i][j];
           if (XG[k] || _XG[k] || G[k]) doorschakel = FALSE;

           if (TO_max[i][k] >= 0) tijd_tot_groen = TEG[i] + TGL_max[i]/10 + TO_max[i][k]/10;
           else                   tijd_tot_groen = TEG[i];

           if (( TTP[k]<=tijd_tot_groen) && (GL[k] || TRG[k] || VA[k] || (HVAS[k]&BIT5) || FCA[k])) doorschakel = FALSE;
           if ((TTPR[k]<=tijd_tot_groen) && (GL[k] || TRG[k] || VA[k] || (HVAS[k]&BIT5) || FCA[k] || (TTPR[k]<255))) doorschakel = FALSE;
         }
         for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {  /* fictieve conflicten */
           k = TO_pointer[i][j];
           if (XG[k] || _XG[k] || G[k]) doorschakel = FALSE;
         }
       }
     }
   }
 }
 if (wens && doorschakel) return TRUE;
 return FALSE;
}

/* PROCEDURE INITIALISEER REGELING */
/* =============================== */

void proc_init_crsv(                         /* Fik170831 */
	mulv progkeuze)           /* Programma keuze CRSV */
{
 mulv i,j;

 PROGKEUZE_CS   = progkeuze;
 HFIX = No_Sync = FALSE;

 aant_fts = aant_bbv = aant_gov = aant_hki = 0;
 _tts     = _vtg     = _vgr     = _pri     = NG;

 for (i=0; i<FCMAX; i++) {
   KNP[i]  = TTR_WV[i] = TTR_HK[i]  = TTR_HK_oud[i] = 0;
   TTPG[i] =  XG[i]    = TSV[i]     = KPGR[i]       = 0;
   HKV[i]  = _XG[i]    = _TTR_HK[i] = FALSE;
   TTPR[i] = VTTP[i]   = TPVU[i]    = UTTP[i] = SDSN[i] = TEP[i] = 255;
   EDSN[i] = DSSP[i]   = DSEP[i]    = VA[i]   = PEG[i]  = TTU[i] = TSP[i] = 0;

   /* Initialiseer signaalgroep-koppelingen */
   fts[i][0] =fts[i][1] =fts[i][2] =fts[i][3] =fts[i][4] =0;
   bbv[i][0] =bbv[i][1] =bbv[i][2] =bbv[i][3] =bbv[i][4] =bbv[i][5] =0;
   bbv[i][6] =bbv[i][7] =bbv[i][8] =0;
   gov[i][0] =gov[i][1] =gov[i][2] =gov[i][3] =gov[i][4] =gov[i][5] =0;
   gov[i][6] =gov[i][7] =gov[i][8] =gov[i][9] =gov[i][10]=gov[i][11]=0;
   gov[i][12]=gov[i][13]=gov[i][14]=gov[i][15]=gov[i][16]=gov[i][17]=0;
   gov[i][18]=gov[i][19]=gov[i][20]=gov[i][21]=gov[i][22]=gov[i][23]=0;
   gov[i][24]=gov[i][25]=gov[i][26]=gov[i][27]=gov[i][28]=0;

   for (j=0; j<21; j++) {
	hki[i][j] = 0;
   }
   for (j=0; j<FCMAX; j++) {
	RW_hki[j][i]  = 0;
	BW_hki[j][i]  = 0;
	XG_hki[j][i]  = 0;
	FCA_hki[j][i] = 0;
	KNP_hki[j][i] = 0;
	PVAS_hki[j][i]= 0;
   }

   /* Default OV-prioriteit NIET aanwezig */
   ovp[i][0] =ovp[i][1] =ovp[i][2] =ovp[i][3] =ovp[i][4] =ovp[i][5] =NG;
   ovp[i][6] =ovp[i][7] =ovp[i][8] =ovp[i][9] =ovp[i][10]=ovp[i][11]=NG;
   ovp[i][12]=ovp[i][13]=ovp[i][14]=ovp[i][15]=ovp[i][16]=ovp[i][17]=ovp[i][18]=NG;

   SVS2[i] = VS2M[i] = TEG_OV[i] = CTTP[i] = 0;
   PVAS[i] = PVS2[i] = 0;

   TTK_hki[i] = TEG_hki[i] = TSPR[i] = TEPR[i] = 0;
   BHK[i]     = BLW[i]     = FALSE;

   WTV_FC[i]  = FALSE;
   CON_BUS[i] = FALSE;
   HALT[i]    = 0;

   VAG_OP[i] = FALSE;
   if (TRG[i]) ETRG[i] = 2;
   else        ETRG[i] = 0;
 }
}

/* PROCEDURE BEPAAL VTG.FTS.KOPPELING */
/* ================================== */

void proc_def_vtg_fts(                       /* Fik150101 */
	mulv fc1,           /* Signaalgroep 1 (voetganger)*/
	mulv fc2,           /* Signaalgroep 2 (fietser 2) */
	mulv fc3,           /* Signaalgroep 3 (fietser 3) */
	mulv mre23,         /* Meerea. fietser 2 met 3    */
	mulv mre32)         /* Meerea. fietser 3 met 2    */
{
 bool _wtv = FALSE;

 if (aant_fts < FCMAX) {
	fts[aant_fts][0] = fc1;
	fts[aant_fts][1] = fc2;
	fts[aant_fts][2] = fc3;
	fts[aant_fts][3] = mre23;
	fts[aant_fts][4] = mre32;
	aant_fts++;

	if (fc1 != NG) { if (WTVFC[fc1]&BIT0) _wtv = TRUE; }
	if (fc2 != NG) { if (WTVFC[fc2]&BIT0) _wtv = TRUE; }
	if (fc3 != NG) { if (WTVFC[fc3]&BIT0) _wtv = TRUE; }

	if (_wtv) {
	  if (fc1 != NG) WTV_FC[fc1] |= BIT0;
	  if (fc2 != NG) WTV_FC[fc2] |= BIT0;
	  if (fc3 != NG) WTV_FC[fc3] |= BIT0;
        }
 }
}

/* PROCEDURE BEPAAL VTG.KOPPELING [ PROGRESSIEF ] */
/* ============================================== */

void proc_def_vtg_bb(                        /* Fik150101 */
	mulv fc1,         /* Signaalgroep 1               */
	mulv fc2,         /* Signaalgroep 2               */
	mulv kpt12,       /* KoppelTIJD eg1 -> eg2        */
	mulv gkg,         /* Geen KoppelGarantie bij alt  */
	mulv fc3,         /* Gekoppelde fietsrichting 3   */
	mulv fc4,         /* Gekoppelde fietsrichting 4   */
	mulv mre34,       /* Meerea. fietser 3 met 4      */
	mulv mre43,       /* Meerea. fietser 4 met 3      */
	mulv gkg_pri)     /* Geen KoppelGarantie bij prio */
{
 bool _wtv = FALSE;

 if (aant_bbv < FCMAX) {
	bbv[aant_bbv][0] = fc1;
	bbv[aant_bbv][1] = fc2;
	bbv[aant_bbv][2] = kpt12;
	bbv[aant_bbv][3] = gkg;
	bbv[aant_bbv][4] = fc3;
	bbv[aant_bbv][5] = fc4;
	bbv[aant_bbv][6] = mre34;
	bbv[aant_bbv][7] = mre43;
	bbv[aant_bbv][8] = gkg_pri;
	aant_bbv++;

	if (fc1 != NG) { if (WTVFC[fc1]&BIT0) _wtv = TRUE; }
	if (fc2 != NG) { if (WTVFC[fc2]&BIT0) _wtv = TRUE; }
	if (fc3 != NG) { if (WTVFC[fc3]&BIT0) _wtv = TRUE; }
	if (fc4 != NG) { if (WTVFC[fc4]&BIT0) _wtv = TRUE; }

	if (_wtv) {
	  if (fc1 != NG) WTV_FC[fc1] |= BIT0;
	  if (fc2 != NG) WTV_FC[fc2] |= BIT0;
	  if (fc3 != NG) WTV_FC[fc3] |= BIT0;
	  if (fc4 != NG) WTV_FC[fc4] |= BIT0;
        }
 }
}

/* PROCEDURE BEPAAL VTG.KOPPELING [ GESCHEIDEN ] */
/* ============================================= */

void proc_def_vtg_go(                        /* Fik150101 */
	mulv fc1,         /* Signaalgroep 1               */
	mulv fc2,         /* Signaalgroep 2	          */
	mulv kpa12,       /* Kop.aanvr. sg1 -> sg2        */
	mulv kpa21,       /* Kop.aanvr. sg2 -> sg1        */
	mulv kpt12,       /* KoppelTIJD sg1 -> eg2        */
	mulv kpt21,       /* KoppelTIJD sg2 -> eg1        */
	mulv gkg,         /* Geen KoppelGarantie bij alt  */
	mulv fc3,         /* Gekoppelde fietsrichting 3   */
	mulv fc3v,        /* Volgrichting van fietser 3   */
	mulv mr33v,       /* Naloop nooit zonder voeding  */
	mulv kpt3s,       /* Koppeltijd vanaf start fc3   */
	mulv kpt3e,       /* Koppeltijd vanaf einde fc3   */
	mulv hfk3e,       /* (HF)start naloop einde fc3   */
	mulv fc4,         /* Gekoppelde fietsrichting 4   */
	mulv fc4v,        /* Volgrichting van fietser 4   */
	mulv mr44v,       /* Naloop nooit zonder voeding  */
	mulv kpt4s,       /* Koppeltijd vanaf start fc4   */
	mulv kpt4e,       /* Koppeltijd vanaf einde fc4   */
	mulv hfk4e,       /* (HF)start naloop einde fc4   */
	mulv mre34,       /* Meerea. fietser 3 met 4      */
	mulv mre43,       /* Meerea. fietser 4 met 3      */
	mulv gkg_pri,     /* Geen KoppelGarantie bij prio */
                          /* 3e vtg is de middelste vtg!  */
	mulv vtg3,        /* Signaalgroep 3e voetganger   */
	mulv kpa31,       /* Kop.aanvr. sg3 -> sg1        */
	mulv kpa32,       /* Kop.aanvr. sg3 -> sg2        */
	mulv kpt13,       /* KoppelTIJD sg1 -> eg3        */
	mulv kpt23,       /* KoppelTIJD sg2 -> eg3        */
	mulv kpt31,       /* KoppelTIJD sg3 -> eg1        */
	mulv kpt32)       /* KoppelTIJD sg3 -> eg2        */
{
 bool _wtv = FALSE;

 if (aant_gov < FCMAX) {
	gov[aant_gov][0] = fc1;
	gov[aant_gov][1] = fc2;
	gov[aant_gov][2] = kpa12;
	gov[aant_gov][3] = kpa21;
	gov[aant_gov][4] = kpt12;
	gov[aant_gov][5] = kpt21;
	gov[aant_gov][6] = gkg;
	gov[aant_gov][7] = fc3;
	gov[aant_gov][8] = fc3v;
	gov[aant_gov][9] = mr33v;
	gov[aant_gov][10]= kpt3s;
	gov[aant_gov][11]= kpt3e;
	gov[aant_gov][12]= hfk3e;
	gov[aant_gov][13]= fc4;
	gov[aant_gov][14]= fc4v;
	gov[aant_gov][15]= mr44v;
	gov[aant_gov][16]= kpt4s;
	gov[aant_gov][17]= kpt4e;
	gov[aant_gov][18]= hfk4e;
	gov[aant_gov][19]= mre34;
	gov[aant_gov][20]= mre43;
	gov[aant_gov][21]= gkg_pri;
	gov[aant_gov][22]= vtg3;
	gov[aant_gov][23]= kpa31;
	gov[aant_gov][24]= kpa32;
	gov[aant_gov][25]= kpt13;
	gov[aant_gov][26]= kpt23;
	gov[aant_gov][27]= kpt31;
	gov[aant_gov][28]= kpt32;

	aant_gov++;

	if (fc1  != NG) { if (WTVFC[fc1]&BIT0)  _wtv = TRUE; }
	if (fc2  != NG) { if (WTVFC[fc2]&BIT0)  _wtv = TRUE; }
	if (fc3  != NG) { if (WTVFC[fc3]&BIT0)  _wtv = TRUE; }
	if (fc4  != NG) { if (WTVFC[fc4]&BIT0)  _wtv = TRUE; }
	if (fc3v != NG) { if (WTVFC[fc3v]&BIT0) _wtv = TRUE; }
	if (fc4v != NG) { if (WTVFC[fc4v]&BIT0) _wtv = TRUE; }
	if (vtg3 != NG) { if (WTVFC[vtg3]&BIT0) _wtv = TRUE; }

	if (_wtv) {
	  if (fc1  != NG) WTV_FC[fc1]  |= BIT0;
	  if (fc2  != NG) WTV_FC[fc2]  |= BIT0;
	  if (fc3  != NG) WTV_FC[fc3]  |= BIT0;
	  if (fc4  != NG) WTV_FC[fc4]  |= BIT0;
	  if (fc3v != NG) WTV_FC[fc3v] |= BIT0;
	  if (fc4v != NG) WTV_FC[fc4v] |= BIT0;
	  if (vtg3 != NG) WTV_FC[vtg3] |= BIT0;
        }
 }
}

/* PROCEDURE BEPAAL SERIELE KOPPELING */
/* ================================== */

void proc_def_hki(                           /* Fik150101 */
	mulv fc1,             /* Signaalgroep 1 [voedend] */
	mulv fc2,             /* Signaalgroep 2 [volg sg1]*/
	mulv kptvs12,         /* Max.voorst.sg1 -> sg2    */
	mulv kptsg12,         /* KoppelTIJD sg1 -> eg2    */
	mulv kpteg12,         /* KoppelTIJD eg1 -> eg2    */
	mulv kptdm12,         /* KoppelTIJD dm2 na hki    */
	mulv de11,            /* Det.1 sg1 tbv start.teg  */
	mulv de12,            /* Det.2 sg2 tbv start.teg  */
	mulv de13,            /* Det.3 sg3 tbv start.teg  */
	mulv fc3,             /* Signaalgroep 3 [volg sg2]*/
	mulv kptvs23,         /* Max.voorst.sg2 -> sg3    */
	mulv kptsg23,         /* KoppelTIJD sg2 -> eg3    */
	mulv kpteg23,         /* KoppelTIJD eg2 -> eg3    */
	mulv kptdm23,         /* KoppelTIJD dm3 na hki    */
	mulv de21,            /* Det.1 sg2 tbv start.teg  */
	mulv de22,            /* Det.2 sg2 tbv start.teg  */
	mulv de23,            /* Det.3 sg2 tbv start.teg  */
	mulv hf_hki)          /* Hulpfunctie hki aktief   */
{
 if (aant_hki < FCMAX) {
	hki[aant_hki][0] = fc1;
	hki[aant_hki][1] = fc2;
	hki[aant_hki][2] = kptvs12;
	hki[aant_hki][3] = kptsg12;
	hki[aant_hki][4] = kpteg12;
	hki[aant_hki][5] = kptdm12;
	hki[aant_hki][6] = de11;
	hki[aant_hki][7] = de12;
	hki[aant_hki][8] = de13;
	hki[aant_hki][9] = 0;		/* buffer tijd groen sg1 en niet groen sg2 */

	hki[aant_hki][10]= fc3;
	hki[aant_hki][11]= kptvs23;
	hki[aant_hki][12]= kptsg23;
	hki[aant_hki][13]= kpteg23;
	hki[aant_hki][14]= kptdm23;
	hki[aant_hki][15]= de21;
	hki[aant_hki][16]= de22;
	hki[aant_hki][17]= de23;
	hki[aant_hki][18]= 0;		/* buffer tijd groen sg2 en niet groen sg3 */

	hki[aant_hki][19]= hf_hki;

	/* status hki -> 0 = uit; 1= insch; 2= aan; 3= uitsch; */

	if (IH[hf_hki]) hki[aant_hki][20]= 2;
	else            hki[aant_hki][20]= 0;

	aant_hki++;
 }
}

/* PROCEDURE DEFINITIE OV-PRIORITEIT */
/* ================================= */

void proc_def_ov(                            /* Fik150101 */
	mulv fc,     /* Signaalgroep                      */
	mulv deftts, /* Default tijd      tot  stopstreep */
	mulv defvgr, /* Default groentijd voor stopstreep */
	mulv uitbew, /* Uitmeldbewaking                   */
	mulv maxveu) /* Max.tijd voor einde uitstel       */
{
 if ((fc >= 0) && (fc < FCMAX)) {
	ovp[fc][0] = deftts; /* Als geen definitie dan TTS verplicht */
	ovp[fc][1] = defvgr; /* Als geen definitie dan garantiegroen */
	ovp[fc][2] = uitbew; /* Verplicht anders definitie ongeldig! */
	ovp[fc][18]= maxveu; /* Als geen definitie dan gelijk aan 255*/
 }
}

/* PROCEDURE BEPAAL PROGRAMMA-KEUZE SLAAF */
/* ====================================== */

void proc_crsv_progkeuze_slaaf(              /* Fik170831 */
	mulv tt1,              /* Index timer tijdvertr. omschakelen  */
	mulv mckode)           /* Index geheugenelement met kode complexcoordinator */
{
 mulv def_prg = 0;

 #ifdef prmdefprg
 def_prg = PRM[prmdefprg];
 if (def_prg > aantal_VAS) def_prg = 0;
 #endif

 HFIX = FIXATIE;

 /* CKODE -> kode vanaf complex-coordinator                  */
 /* HKODE -> buffer CKODE t.b.v. start-puls                  */
 /* LKODE -> kode in lokale automaat (volgt vertraagt CKODE) */

 RT[tt1] = FALSE;
 CKODE = MM[mckode];
 /* NOOIT negeren DVM programma keuze */
 if ((negeer_cpc) && (CKODE < 21)) CKODE = 0;

 #ifndef prmctijd21
 if (CKODE > aantal_VAS) CKODE = 0;
 #else
 if (CKODE > aantal_VAS) {
   if ((CKODE < 21) || (CKODE > 26)) CKODE = 0;
 }
 #endif

 if (CKODE == 1) {
   #ifndef prmctyd_1
   CKODE = 0;
   #else
   if (PRM[prmctyd_1] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 2) {
   #ifndef prmctyd_2
   CKODE = 0;
   #else
   if (PRM[prmctyd_2] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 3) {
   #ifndef prmctyd_3
   CKODE = 0;
   #else
   if (PRM[prmctyd_3] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 4) {
   #ifndef prmctyd_4
   CKODE = 0;
   #else
   if (PRM[prmctyd_4] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 5) {
   #ifndef prmctyd_5
   CKODE = 0;
   #else
   if (PRM[prmctyd_5] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 6) {
   #ifndef prmctyd_6
   CKODE = 0;
   #else
   if (PRM[prmctyd_6] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 7) {
   #ifndef prmctyd_7
   CKODE = 0;
   #else
   if (PRM[prmctyd_7] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 8) {
   #ifndef prmctyd_8
   CKODE = 0;
   #else
   if (PRM[prmctyd_8] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 9) {
   #ifndef prmctyd_9
   CKODE = 0;
   #else
   if (PRM[prmctyd_9] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 21) {
   #ifndef prmctyd_21
   CKODE = 0;
   #else
   if (PRM[prmctyd_21] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 22) {
   #ifndef prmctyd_22
   CKODE = 0;
   #else
   if (PRM[prmctyd_22] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 23) {
   #ifndef prmctyd_23
   CKODE = 0;
   #else
   if (PRM[prmctyd_23] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 24) {
   #ifndef prmctyd_24
   CKODE = 0;
   #else
   if (PRM[prmctyd_24] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 25) {
   #ifndef prmctyd_25
   CKODE = 0;
   #else
   if (PRM[prmctyd_25] == 0) CKODE = 0;
   #endif
 }

 if (CKODE == 26) {
   #ifndef prmctyd_26
   CKODE = 0;
   #else
   if (PRM[prmctyd_26] == 0) CKODE = 0;
   #endif
 }

 if  (HKODE != CKODE) { HKODE   = CKODE;
		        RT[tt1] = TRUE;
		      }

 if ((LKODE != CKODE) && !RT[tt1] && !T[tt1] || SPR) LKODE = CKODE;

 GEWPRG = LKODE;
 if (GEWPRG == 0) { GEWPRG = def_prg; No_Sync = TRUE; }
 else             {                   No_Sync = FALSE; }
 OMSCH = (AKTPRG != GEWPRG) && !HFIX;

 if (((STAP == OPNT) || !OPNT) && OMSCH || SPR) {
	AKTPRG = GEWPRG;
        if (AKTPRG > 0) SVAS = TRUE;
        else            SSGD = TRUE;
 }

 CTYD = IPNT = OPNT = CPNT = FALSE;

 RT[tvfix] = FALSE;
 AT[tvfix] = T[tvfix] && (AKTPRG == 0);
}

/* PROCEDURE BEPAAL MKODE CRSV MODULE */
/* ================================== */
void proc_bepaal_MKODE_crsv(void)  /* Fik170831 */
{
 MKODE = 0;

 if (cpc_prog_vas9) MKODE = 9;
 if (cpc_prog_vas8) MKODE = 8;
 if (cpc_prog_vas7) MKODE = 7;
 if (cpc_prog_vas6) MKODE = 6;
 if (cpc_prog_vas5) MKODE = 5;
 if (cpc_prog_vas4) MKODE = 4;
 if (cpc_prog_vas3) MKODE = 3;
 if (cpc_prog_vas2) MKODE = 2;
 if (cpc_prog_vas1) MKODE = 1;

 if ((_dvm <= 10) && (_dvm >  0)) MKODE = _dvm;
 if ((_dvm <= 20) && (_dvm > 10)) MKODE = 0;
 if ( _dvm >  20) MKODE = _dvm;

 if (MKODE == 1) {
   #ifndef prmctyd_1
   MKODE = 0;
   #else
   if (PRM[prmctyd_1] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 2) {
   #ifndef prmctyd_2
   MKODE = 0;
   #else
   if (PRM[prmctyd_2] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 3) {
   #ifndef prmctyd_3
   MKODE = 0;
   #else
   if (PRM[prmctyd_3] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 4) {
   #ifndef prmctyd_4
   MKODE = 0;
   #else
   if (PRM[prmctyd_4] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 5) {
   #ifndef prmctyd_5
   MKODE = 0;
   #else
   if (PRM[prmctyd_5] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 6) {
   #ifndef prmctyd_6
   MKODE = 0;
   #else
   if (PRM[prmctyd_6] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 7) {
   #ifndef prmctyd_7
   MKODE = 0;
   #else
   if (PRM[prmctyd_7] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 8) {
   #ifndef prmctyd_8
   MKODE = 0;
   #else
   if (PRM[prmctyd_8] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 9) {
   #ifndef prmctyd_9
   MKODE = 0;
   #else
   if (PRM[prmctyd_9] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 21) {
   #ifndef prmctyd_21
   MKODE = 0;
   #else
   if (PRM[prmctyd_21] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 22) {
   #ifndef prmctyd_22
   MKODE = 0;
   #else
   if (PRM[prmctyd_22] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 23) {
   #ifndef prmctyd_23
   MKODE = 0;
   #else
   if (PRM[prmctyd_23] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 24) {
   #ifndef prmctyd_24
   MKODE = 0;
   #else
   if (PRM[prmctyd_24] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 25) {
   #ifndef prmctyd_25
   MKODE = 0;
   #else
   if (PRM[prmctyd_25] == 0) MKODE = 0;
   #endif
 }

 if (MKODE == 26) {
   #ifndef prmctyd_26
   MKODE = 0;
   #else
   if (PRM[prmctyd_26] == 0) MKODE = 0;
   #endif
 }
}

/* PROCEDURE BEPAAL PROGRAMMA-KEUZE MASTER */
/* ======================================= */

void proc_crsv_progkeuze_master(             /* Fik170831 */
	mulv tt1)              /* Tijdvertr. omschakelen  */
{
 mulv def_prg = 0;

 #ifdef prmdefprg
 def_prg = PRM[prmdefprg];
 if (def_prg > aantal_VAS) def_prg = 0;
 #endif

 HFIX = FIXATIE;

 /* CKODE -> kode vanaf complex-coordinator                  */
 /* HKODE -> buffer CKODE t.b.v. start-puls                  */
 /* LKODE -> kode in lokale automaat (volgt vertraagt CKODE) */

 RT[tt1] = FALSE;
 CKODE   = MKODE;
 if  (CKODE == 0)     { CKODE = def_prg; }
 if  (HKODE != CKODE) { HKODE   = CKODE;
		        RT[tt1] = TRUE;
		      }
 if ((LKODE != CKODE) && !RT[tt1] && !T[tt1] || SPR) LKODE = CKODE;

 GEWPRG = LKODE;
 OMSCH = (AKTPRG != GEWPRG) && !HFIX;

 if (((STAP == OPNT) || !OPNT) && OMSCH || SPR) {
	AKTPRG = GEWPRG;
        if (AKTPRG) SVAS = TRUE;
        else        SSGD = TRUE;
 }
 CTYD = IPNT = OPNT = CPNT = FALSE;

 RT[tvfix] = FALSE;
 AT[tvfix] = T[tvfix] && (AKTPRG == 0);
}

/* PROCEDURE BEPAAL AKTIEF STAPPENRASTER */
/* ===================================== */

void proc_crsv_parm(                         /* Fik150101 */
	mulv ctyd,                  /* Cyclustijd         */
	mulv insc,                  /* Inschakelpunt      */
	mulv omsc,                  /* Omschakelpunt      */
	mulv sync)                  /* Synchronisatiepunt */
{
 CTYD = PRM[ctyd];
 IPNT = PRM[insc];
 OPNT = PRM[omsc];
 CPNT = PRM[sync];

 if ((IPNT>CTYD) || !IPNT) IPNT = 1;
 if ( OPNT>CTYD)	   OPNT = FALSE;
 if ((CPNT>CTYD) || !CPNT) CPNT = 1;
}

/* PROCEDURE BEPAAL STATUS STAPPENRASTER SLAAF */
/* =========================================== */

void proc_crsv_stappenraster(                /* Fik170831 */
	bool ssync)                /* start Koppelsignaal 5 "IN" */

{
 mulv prog_keuze   = 20;
 if (PROGKEUZE_CS != NG) prog_keuze = PRM[PROGKEUZE_CS];

 HFIX = FIXATIE;
 RT[tvfix] |= HFIX;

 if (ssync && !SVAS && AKTPRG && !OMSCH && (prog_keuze != 11)
		  && !RT[tvfix] && !T[tvfix] && !No_Sync) {  /* Start synchronisatie */

   Straag = Svsnel = Sstart = Sverschil = Sversnel = Sblok = Sdubb = FALSE;

   /* Bepaal "vooruitlopen" t.o.v. het stappenraster */ 
   /* ---------------------------------------------- */

   if (STAP>=CPNT) { Sverschil = STAP - CPNT; }
   else		   { Sverschil = CTYD + STAP - CPNT; }

   /* Als "vooruitlopen" <= 50% v/d CTYD dan vertragen stappenraster */
   /* -------------------------------------------------------------- */

   if ((CTYD/2)>=Sverschil) { Straag = TRUE; Sblok = FALSE; }
   else {

     /* Bepaal "achterlopen" t.o.v. het stappenraster */
     /* --------------------------------------------- */

     Sverschil = CTYD - Sverschil;

     /* Als "achterlopen" <= 10% v/d CTYD dan versnellen stappenraster */
     /* -------------------------------------------------------------- */

     if ((CTYD/10)>=Sverschil) { Svsnel = TRUE; Sversnel = 10; }
     else {

       /* Herstart stappenraster indien vertragen EN versnellen ongewenst */
       /* --------------------------------------------------------------- */

       Sstart    = TRUE;
       Sverschil = FALSE;
     }
   }
 }

 if (TS && Straag) {	/* Vertraag stappenraster */

   if (Sverschil > 0) {
     Sblok = !Sblok;
     if (Sblok) Sverschil--;
   } else { Sverschil = Straag = Sblok = FALSE; }
 }

 if (TS && Svsnel) {	/* Versnel stappenraster */

   if (Sverschil > 0) {
     Sversnel++;
     if (Sversnel >= 10) {
       Sverschil--;
       Sdubb    = TRUE;
       Sversnel = 1;
     }
   } else { Sverschil = Svsnel = Sdubb = Sversnel = FALSE; }
 }

 if (SVAS || OMSCH) {	/* ALTIJD reset synchronisatie bij start VAS */
   if (SVAS) STAP  = IPNT;
   Straag = Svsnel = Sstart = Sverschil = Sversnel = Sblok = Sdubb = FALSE;
 }

 if (!HFIX) {
   if ( TS && !SVAS && (!Sblok || !Straag))  STAP++;
   if (!TS &&	         Sdubb &&  Svsnel) { STAP++;
				 	     Sdubb = FALSE;
				 	     N_ts  = TRUE;
   } else { N_ts = FALSE; }
 } else {		/* ALTIJD reset synchronisatie bij FIXATIE */
     Straag = Svsnel = Sstart = Sverschil = Sversnel = Sblok = Sdubb = FALSE;
     N_ts   = FALSE;
 }

 if (!TS && !N_ts && snel_lokaal()) {
   STAP++;
   N_ts = TRUE;
 }

 if (STAP>CTYD) STAP = 1;

 if (!AKTPRG) {		/* ALTIJD reset synchronisatie bij einde VAS */
   STAP   = FALSE;
   Straag = Svsnel = Sstart = Sverschil = Sversnel = Sblok = Sdubb = FALSE;
 }

 if (prog_keuze == 11 || No_Sync) {
   /* ALTIJD reset synch bij "snel"LOKAAL programma of indien default stappenraster aktief */
   Straag = Svsnel = Sstart = Sverschil = Sversnel = Sblok = Sdubb = FALSE;
 }

 if (Sstart) {
   STAP   = CPNT;
   Sstart = FALSE;
   SVAS   = TRUE;
 }
 /* proc_reset_crsv_start(); verplaatst naar reset_start() Traffick */
}

/* PROCEDURE BEPAAL STATUS STAPPENRASTER MASTER */
/* ============================================ */

void proc_crsv_master_raster(                /* Fik170831 */
	mulv t_s)          /* Duur synchronisatie-signaal */
{
 mulv prog_keuze = 20;
 if (PROGKEUZE_CS != NG) prog_keuze = PRM[PROGKEUZE_CS];

 HFIX = FIXATIE;

 RT[tvfix] |= HFIX;
 RT[t_s] = FALSE;

 if (SVAS)	  	   STAP = IPNT;
 if (TS && !SVAS && !HFIX) STAP++;

 N_ts = FALSE;
 if (!TS && snel_lokaal()) {
   STAP++;
   N_ts = TRUE;
 }
 if (STAP>CTYD) STAP = 1; /* @#@ Fik150201 */

 if (!AKTPRG) {
   STAP = FALSE;
 } else {
   if (TS && (STAP == CPNT) && !OMSCH && !HFIX && !RT[tvfix] && !T[tvfix]
				      && (prog_keuze != 11)) {
	RT[t_s] = TRUE;
   }
 }
 /* proc_reset_crsv_start(); verplaatst naar reset_start() Traffick */
}

/* PROCEDURE RESET VARIABELEN BIJ START SIGNAALGROEPSTURING */
/* ======================================================== */

void proc_reset_crsv_start(void)               /* Fik170831 */
{
 mulv i,j,x;
 mulv fc1,fc2,fc3,hulp_functie,stat;
 bool PVAS1,PVS22;
 bool kop_gewenst = FALSE;
 bool inschakel = FALSE;
 bool uitschakel = FALSE;
 bool _XG_ = FALSE;

 #ifdef schnooit
 if (SCH[schnooit]) {
   SCH[schnooit] = 0;
   CIF_PARM1WIJZAP = -2;
 }
 #endif 

 #ifdef schaltijd
 if (!SCH[schaltijd]) {
   SCH[schaltijd]  = 1;
   CIF_PARM1WIJZAP = -2;
 }
 #endif 

 #ifdef prmversie
 if (PRM[prmversie] != CRSV2CCOL_versie) {
   PRM[prmversie] = CRSV2CCOL_versie;
   CIF_PARM1WIJZAP = -2;
 }
 #endif

 HFIX = FIXATIE;

 for (i=0; i<FCMAX; i++) {
 	BHK[i] = HKV[i] = FALSE;
	if (RA[i] && WTV_FC[i]) {
		XG[i] |= BIT7;
	} else {
		XG[i] &= (~BIT7);
	}
 }

 for (i=0; i<aant_fts; i++) {
     _XG_ = FALSE;
	 if (fts[i][0] != NG) { if (XG[fts[i][0]]&BIT7) _XG_ = TRUE; }
	 if (fts[i][1] != NG) { if (XG[fts[i][1]]&BIT7) _XG_ = TRUE; }
	 if (fts[i][2] != NG) { if (XG[fts[i][2]]&BIT7) _XG_ = TRUE; }

     if (_XG_) {
       if (fts[i][0] != NG) { if (!G[fts[i][0]]) XG[fts[i][0]] |= BIT7; }
       if (fts[i][1] != NG) { if (!G[fts[i][1]]) XG[fts[i][1]] |= BIT7; }
       if (fts[i][2] != NG) { if (!G[fts[i][2]]) XG[fts[i][2]] |= BIT7; }
     }
 }

 for (i=0; i<aant_bbv; i++) {
     _XG_ = FALSE;
     if (bbv[i][0] != NG) { if (XG[bbv[i][0]]&BIT7) _XG_ = TRUE; }
     if (bbv[i][1] != NG) { if (XG[bbv[i][1]]&BIT7) _XG_ = TRUE; }
     if (bbv[i][4] != NG) { if (XG[bbv[i][4]]&BIT7) _XG_ = TRUE; }
     if (bbv[i][5] != NG) { if (XG[bbv[i][5]]&BIT7) _XG_ = TRUE; }

     if (_XG_) {
       if (bbv[i][0] != NG) { if (!G[bbv[i][0]]) XG[bbv[i][0]] |= BIT7; }
       if (bbv[i][1] != NG) { if (!G[bbv[i][1]]) XG[bbv[i][1]] |= BIT7; }
       if (bbv[i][4] != NG) { if (!G[bbv[i][4]]) XG[bbv[i][4]] |= BIT7; }
       if (bbv[i][5] != NG) { if (!G[bbv[i][5]]) XG[bbv[i][5]] |= BIT7; }
     }
 }

 for (i=0; i<aant_gov; i++) {
     _XG_ = FALSE;
     if (gov[i][0]  != NG) { if (XG[gov[i][0]]&BIT7)  _XG_ = TRUE; }
     if (gov[i][1]  != NG) { if (XG[gov[i][1]]&BIT7)  _XG_ = TRUE; }
     if (gov[i][7]  != NG) { if (XG[gov[i][7]]&BIT7)  _XG_ = TRUE; }
     if (gov[i][8]  != NG) { if (XG[gov[i][8]]&BIT7)  _XG_ = TRUE; }
     if (gov[i][13] != NG) { if (XG[gov[i][13]]&BIT7) _XG_ = TRUE; }
     if (gov[i][14] != NG) { if (XG[gov[i][14]]&BIT7) _XG_ = TRUE; }
     if (gov[i][22] != NG) { if (XG[gov[i][22]]&BIT7) _XG_ = TRUE; }

     if (_XG_) {
       if (gov[i][0]  != NG) { if (!G[gov[i][0]] ) XG[gov[i][0]]  |= BIT7; }
       if (gov[i][1]  != NG) { if (!G[gov[i][1]] ) XG[gov[i][1]]  |= BIT7; }
       if (gov[i][7]  != NG) { if (!G[gov[i][7]] ) XG[gov[i][7]]  |= BIT7; }
       if (gov[i][8]  != NG) { if (!G[gov[i][8]] ) XG[gov[i][8]]  |= BIT7; }
       if (gov[i][13] != NG) { if (!G[gov[i][13]]) XG[gov[i][13]] |= BIT7; }
       if (gov[i][14] != NG) { if (!G[gov[i][14]]) XG[gov[i][14]] |= BIT7; }
       if (gov[i][22] != NG) { if (!G[gov[i][22]]) XG[gov[i][22]] |= BIT7; }
     }
 }

 if (AKTPRG > 0) {
   BLOK = 0;
   SBL  = WACHT = FALSE;
   for (i=0; i<aant_hki; i++) {
       SGD[i] = HKI[i] = FCA[i] = 0;
       POV[i] = PEL[i] = PEL2[i]= FTS[i]= 0;
   }

   for (x=0; x<3; x++) {			/* 3x doorlopen ivm mogelijke geneste koppelingen */
     for (i=0; i<aant_hki; i++) {
	fc1          = hki[i][0];
	fc2          = hki[i][1];
	fc3          = hki[i][10];
	hulp_functie = hki[i][19];
	stat         = hki[i][20];
						/* fc2 wordt vastgehouden door koppeling vanaf fc1 */
	if (SG[fc2] && (RW_hki[i][fc2]&BIT3)) BW_hki[i][fc2] = TRUE;

	/* status hki -> 0 = uit; 1= insch; 2= aan; 3= uitsch; */
	kop_gewenst = FALSE;
	if (IH[hulp_functie]) kop_gewenst = TRUE;

	if ( kop_gewenst && (stat == 0)) stat = 1;
	if ( kop_gewenst && (stat == 3)) stat = 2;

	if (!kop_gewenst && (stat == 1)) stat = 0;
	if (!kop_gewenst && (stat == 2)) stat = 3;

	if (stat == 1) {			/* zoek gunstig inschakelmoment */
		inschakel = TRUE;
		if (G[fc1]) {
			inschakel = FALSE;
			BHK[fc1] = TRUE;
			if (somwtk_crsv(fc2)) inschakel = FALSE;
		}
		if (fc3 >= 0) {
			if (G[fc2]) {
				inschakel = FALSE;
				BHK[fc2] = TRUE;
				if (somwtk_crsv(fc3)) inschakel = FALSE;
			}
		}
		if (inschakel) stat = 2;
	}

	if (stat == 3) {			/* zoek gunstig uitschakelmoment */
		uitschakel = TRUE;
		if (G[fc2] && (RW_hki[i][fc2]&BIT3)) {
			uitschakel = FALSE;
			BHK[fc1] = TRUE;
		}
		if (fc3 >= 0) {
			if (G[fc3] && (RW_hki[i][fc3]&BIT3)) uitschakel = FALSE;
			BHK[fc2] = TRUE;
		}
		if (uitschakel) stat = 0;
	}
	hki[i][20] = stat;

        if (stat == 2) { /* bepaal HKV[] Fik150222 */
          if (G[fc1]) {
            HKV[fc2] = TRUE;
            if (fc3 >= 0) {
              if (G[fc1] || G[fc2]) HKV[fc3] = TRUE;
            }
          }
        }
     }
   }
 } else {					/* reset variabelen indien SGD aktief wordt */
   for (i=0; i<aant_hki; i++) {			/* schakel CRSV koppelingen uit */
     hki[i][20] = 0;
   }
   for (i=0; i<FCMAX; i++) {
     KNP[i]  = TTR_WV[i] = TTR_HK[i]  = TTR_HK_oud[i] = 0;
     TTPG[i] =  XG[i]    = TSV[i]     = KPGR[i]       = 0;
     HKV[i]  = _XG[i]    = _TTR_HK[i] = FALSE;
     TTPR[i] = VTTP[i]   = TPVU[i]    = UTTP[i] = SDSN[i] = TEP[i] = 255;
     EDSN[i] = DSSP[i]   = DSEP[i]    = VA[i]   = PEG[i]  = TTU[i] = TSP[i] = 0;

     for (j=0; j<FCMAX; j++) {
	RW_hki[j][i]  = 0;
	BW_hki[j][i]  = 0;
	XG_hki[j][i]  = 0;
	FCA_hki[j][i] = 0;
	KNP_hki[j][i] = 0;
	PVAS_hki[j][i]= 0;
     }

     SVS2[i] = VS2M[i] = TEG_OV[i] = CTTP[i] = 0;
     PVAS[i] = PVS2[i] = 0;

     TTK_hki[i] = TEG_hki[i] = TSPR[i] = TEPR[i] = 0;
     BHK[i]     = BLW[i]     = FALSE;
     CON_BUS[i] = VAG_OP[i]  = FALSE;

     HALT[i]  = halt_tim[i];                    /* neem over vanaf SGD programma */
     PVS2[i] |= BIT2;				/* voorkom doorsnijden bij overgang van SGD naar CRSV */
   }
 }

 for (i=0; i<FCMAX; i++) {
   PVAS1 = (PVAS[i]&BIT1);
   PVS22 = (PVS2[i]&BIT2);

   if ( SVAS) { PVAS[i]  = PVS2[i] = FALSE;
	 	  VA[i] &=(~BIT2);
		  VA[i] &=(~BIT4);
		 FCA[i] &=(~BIT3);

   		for (j=0; j<FCMAX; j++) {
			FCA_hki[j][i] &=(~BIT3);
		}

		if (PVAS1) PVAS[i] |= BIT1;
		if (PVS22) PVS2[i] |= BIT2;
   }

   if (R[i] && TRG[i]) ETRG[i] = 2;
   else { if (ETRG[i] > 0) ETRG[i]--; }

   if (AKTPRG > 0) MK[i] = YV[i] = FALSE;

   for (j=0; j<FCMAX; j++) {
	RW_hki[j][i] &=(~BIT3);
   }

   PVAS[i] &= (~BIT4);			/* Reset uitstellen  toegestaan */
   PVAS[i] &= (~BIT5);			/* Reset afbreken    toegestaan */
   PVAS[i] &= (~BIT6);			/* Reset doorsnijden toegestaan */

   PVS2[i] &= (~BIT0);			/* Reset prioriteit OV voor einde uitstel toegestaan */
   PVS2[i] &= (~BIT1);			/* Reset prioriteit OV na afkappunt       toegestaan */

   if (G[i]) {
		   VA[i] |= BIT0;	/* Altijd aanvraag indien groen */
		 PVS2[i] &=(~BIT4);	/* Reset terugkomen na afkappen */
   } else {
                   KG[i]  = 0;          /* Reset klokgroen op einde groen */
   }
		  BLW[i]  = FALSE;	/* Reset blokkeer wachtstand    */
		   VA[i] &=(~BIT7);	/* Reset start det/ma.aanvraag	*/
		  FCA[i] &=(~BIT2);	/* Reset openhouden aanv.gebied */

   if (EG[i]) {
		  VA[i]  = FALSE;	/* Reset aanvraag		 */
		  XG[i] &= (~BIT4);	/* Reset prioriteit volgrichting */

   		  for (j=0; j<FCMAX; j++) {
			XG_hki[j][i] &=(~BIT4);
		  }

		  if (PVS2[i]&BIT3) PVS2[i] |= BIT4;
   }

   if (SR[i] && (AKTPRG > 0)) {		/* Verwijder bussen waaraan prioriteit is verleend */
	for (j=1; j<=3; j++) {
		if (ovp[i][(j*5)+1] >= 10) {
			ovp[i][(j*5)-2] = NG;
			ovp[i][(j*5)-1] = NG;
			ovp[i][(j*5)  ] = NG;
			ovp[i][(j*5)+1] = NG;
			ovp[i][(j*5)+2] = NG;
		}
	}
	proc_sorteer_bus(i,1,2);
	proc_sorteer_bus(i,2,3);
	proc_sorteer_bus(i,1,2);
   }
   if (ovp[i][18] == -1) TPVU[i] = 255;
   else                  TPVU[i] = PRM[ovp[i][18]]/10;
 }

 if (SPR && (AKTPRG > 0) || ESGD) {
   for (i=0; i<FCMAX; i++) {
     TTP[i] = TTK[i] = 255;
   }
 }
 if (ESGD) init_bteg_crsv();

 _tts = _vtg = _vgr = _pri = NG;	/* Hulpvlaggen tbv OV-melding   */
 init_kapm_crsv();
}


/* PROCEDURE BEPAAL INMELDING OPENBAAR VERVOER */
/* =========================================== */

void proc_ov_inm(                            /* Fik150101 */
	mulv _sg)                         /* Signaalgroep */
{
 mulv _htts,_hvtg,_hvgr,_hpri;
 mulv _hbus;

 if ((_sg >= 0) && (_sg < FCMAX)) {	/* Signaalgroep is correct */
   if (ovp[_sg][2] >= 0) {		/* Uitmeldbewaking is gedefinieerd */
     if ((_tts >= 0) && (_tts < 255) || (ovp[_sg][0] >= 0)) {

	_htts = _tts;			/* Bepaal tts */
	if (((_htts < 0) || (_htts == 255)) && (ovp[_sg][0] >= 0))
	_htts = PRM[ovp[_sg][0]]/10;

	_hvtg = _vtg;			/* Bepaal vtg-nummer */
	if (_hvtg < 0) _hvtg = 0;

	_hvgr = _vgr;			/* Bepaal groen voor passage */
	if ((_hvgr < 0) && (ovp[_sg][1] >= 0))
		_hvgr = PRM[ovp[_sg][1]]/10;
	if (_hvgr < 0) _hvgr = TGG_max[_sg]/10;

	_hpri = _pri;			/*    Bepaal prioriteit toegestaan     */
	if (_hpri < 0) _hpri = 0;	/* 0: Geen prioriteit toegestaan       */
	if (_hpri > 2) _hpri = 0;	/* 1: Alleen aanhouden eigen groenfase */
					/* 2: Prioriteitsrealisatie toegestaan */
	/* Is voertuig al aanwezig? */
	_hbus = 0;
	if (_hvtg > 0) {
	  if      (ovp[_sg][4] == _hvtg) _hbus = 1; /* Ja, dan update */
	  else if (ovp[_sg][9] == _hvtg) _hbus = 2;
	  else if (ovp[_sg][14]== _hvtg) _hbus = 3;
	  else if (ovp[_sg][4] ==  NG  ) _hbus = 1; /* Nee, dan toevoegen */
	  else if (ovp[_sg][9] ==  NG  ) _hbus = 2;
	  else if (ovp[_sg][14]==  NG  ) _hbus = 3;
	} else {
	  if      (ovp[_sg][4] ==  NG  ) _hbus = 1; /* Geen voertuignummer? */
	  else if (ovp[_sg][9] ==  NG  ) _hbus = 2; /* dan alleen toevoegen */
	  else if (ovp[_sg][14]==  NG  ) _hbus = 3;
	}

	if (_hbus > 0) {
		ovp[_sg][(_hbus*5)-2] = _htts;
		ovp[_sg][(_hbus*5)-1] = _hvtg;
		ovp[_sg][(_hbus*5)  ] = _hvgr;
		ovp[_sg][(_hbus*5)+1] = _hpri;
		ovp[_sg][(_hbus*5)+2] = 0;	/* Start aanwezigheidstijd */
	}

	/* Sorteer bussen op basis van tijd tot stopstreep */
	proc_sorteer_bus(_sg,1,2);
	proc_sorteer_bus(_sg,2,3);
	proc_sorteer_bus(_sg,1,2);
     }
   }
 }
}

/* PROCEDURE BEPAAL UITMELDING OPENBAAR VERVOER */
/* =========================================== */

void proc_ov_uit(                            /* Fik150101 */
	mulv _sg)                         /* Signaalgroep */
{
 mulv verwijder;

 if ((_sg >= 0) && (_sg < FCMAX)) {	/* Signaalgroep is correct */
   if (ovp[_sg][2] >= 0) {		/* Uitmeldbewaking is gedefinieerd */
     if (_vtg > 0) {			/* Voertuignummer verwijderen */
	if (ovp[_sg][4] == _vtg) {
	  ovp[_sg][3] = ovp[_sg][4] = ovp[_sg][5] = NG;
	  ovp[_sg][6] = ovp[_sg][7] = NG;
	}
	if (ovp[_sg][9] == _vtg) {
	  ovp[_sg][8] = ovp[_sg][9] = ovp[_sg][10]= NG;
	  ovp[_sg][11]= ovp[_sg][12]= NG;
	}
	if (ovp[_sg][14]== _vtg) {
	  ovp[_sg][13]= ovp[_sg][14]= ovp[_sg][15]= NG;
	  ovp[_sg][16]= ovp[_sg][17]= NG;
	}
     }
     if (_vtg <= 0) {			/* Verwijder oudste "0" voertuig   */
	verwijder = 0;			/* indien niet aanwezig dan oudste */
	 				/* voertuig met nummer verwijderen */
	if      (ovp[_sg][4] == 0) verwijder = 1;
	else if (ovp[_sg][9] == 0) verwijder = 2;
	else if (ovp[_sg][14]== 0) verwijder = 3;
	else if (ovp[_sg][4] >  0) verwijder = 1;
	else if (ovp[_sg][9] >  0) verwijder = 2;
	else if (ovp[_sg][14]>  0) verwijder = 3;

	if (verwijder > 0) {
	  ovp[_sg][(verwijder*5)-2] = NG;
	  ovp[_sg][(verwijder*5)-1] = NG;
	  ovp[_sg][(verwijder*5)  ] = NG;
	  ovp[_sg][(verwijder*5)+1] = NG;
	  ovp[_sg][(verwijder*5)+2] = NG;
	}
     }

     /* Sorteer resterende bussen op basis van tijd tot stopstreep */
     proc_sorteer_bus(_sg,1,2);
     proc_sorteer_bus(_sg,2,3);
     proc_sorteer_bus(_sg,1,2);
   }
 }
}

/* PROCEDURE BEPAAL IN- EN UITMELDINGEN OPENBAAR VERVOER */
/* ===================================================== */

void proc_buf_ov_crsv(           /* Fik170831 */
    mulv _sg,                    /* Signaalgroep */
    mulv priv,                   /* prioriteit bus te vroeg */
    mulv prit,                   /* prioriteit bus op tijd */
    mulv pril,                   /* prioriteit bus te laat */
    mulv awt1,                   /* aanwezigheidsduur 1e bus */
    mulv awt2,                   /* aanwezigheidsduur 2e bus */
    mulv awt3,                   /* aanwezigheidsduur 3e bus */
    mulv stp1,                   /* stiptheidscode 1e bus */
    mulv stp2,                   /* stiptheidscode 2e bus */
    mulv stp3)                   /* stiptheidscode 3e bus */
{
 mulv _hbus;
 mulv _htts = 0;
 mulv _hpri = 0;
 mulv _hvtg = 0;                 /* niet op voertuig nummer bijhouden */
 mulv _hvgr = GTTP[_sg]/10;
 mulv code_stipt;
 mulv h_stiptheid;
 mulv pri_v,pri_t,pri_l;
 mulv verwijder;
 bool pri_uit = FALSE;

 if (priv != NG) {               /* controleer instellingen BUS te vroeg */
   if (PRM[priv] > 3) {
     PRM[priv] = 0;
     CIF_PARM1WIJZAP = -2;
   }
   pri_v = PRM[priv];
 } else {
   pri_v = 0;
 }
 if (prit != NG) {               /* controleer instellingen BUS op tijd */
   if (PRM[prit] > 3) {
     PRM[prit] = 0;
     CIF_PARM1WIJZAP = -2;
   }
   pri_t = PRM[prit];
 } else {
   pri_t = 0;
 }
 if (pril != NG) {               /* controleer instellingen BUS te laat */
   if (PRM[pril] > 3) {
     PRM[pril] = 0;
     CIF_PARM1WIJZAP = -2;
   }
   pri_l = PRM[pril];
 } else {
   pri_l = 0;
 }

 if ((pri_v == 0) && (pri_t == 0) && (pri_l == 0)) pri_uit = TRUE;

 /* KWCOV[_sg] = FALSE; -> verhuisd naar reset_start */
 if (MM[meovinm] == (_sg+1)) KWCOV[_sg] |= BIT0;
 if (MM[meovuit] == (_sg+1)) KWCOV[_sg] |= BIT1;
    
 if (MM[meovinm] == (_sg+1)) VLOG_OV[_sg] |= BIT1;
 if (MM[meovuit] == (_sg+1)) VLOG_OV[_sg] |= BIT2;

#ifdef KAR_AANWEZIG
 _htts = tijdtot_ss;
 if (_htts < 0) _htts = 0;

#ifdef prmttsov
 if (_htts > PRM[ttsov]/10) _htts = PRM[ttsov]/10;
#endif

 h_stiptheid = stiptheid;
 if (stiptheid == 3600) h_stiptheid = 0; /* 3600 = geen informatie                                       */
    
 if      (h_stiptheid>(T_max[tovt]/10))      { code_stipt = 3; } /* bus is te laat                       */
 else if (((T_max[tovv]/10)+h_stiptheid)>=0) { code_stipt = 2; } /* bus is op tijd                       */
 else                                        { code_stipt = 1; } /* bus is te vroeg                      */
#else                                   /* *** ifdef -> KAR_AANWEZIG */
 h_stiptheid = 0; code_stipt = 2;
 _htts = NG;
#endif                                  /* *** ifdef -> KAR_AANWEZIG */

#ifdef schdef_tts
 if (SCH[schdef_tts]) _htts = NG;
#endif

 _hpri = 0;
 if ((code_stipt == 1) && (pri_v > _hpri)) _hpri = pri_v;
 if ((code_stipt == 2) && (pri_t > _hpri)) _hpri = pri_t;
 if ((code_stipt == 3) && (pri_l > _hpri)) _hpri = pri_l;

 if ((_sg >= 0) && (MM[meovinm] == (_sg+1)) && !pri_uit) {
   if (ovp[_sg][2] >= 0) {		/* Uitmeldbewaking is gedefinieerd */
     if ((_tts >= 0) && (_tts < 255) || (ovp[_sg][0] >= 0)) {

	/* _htts = _tts;		   Bepaal tts */
	if (((_htts < 0) || (_htts == 255)) && (ovp[_sg][0] >= 0))
	_htts = PRM[ovp[_sg][0]]/10;

	/* _hvtg = _vtg;		   Bepaal vtg-nummer */
	if (_hvtg < 0) _hvtg = 0;

	/* _hvgr = _vgr;		   Bepaal groen voor passage */
	if ((_hvgr < 0) && (ovp[_sg][1] >= 0))
	_hvgr = PRM[ovp[_sg][1]]/10;
	if (_hvgr < 0) _hvgr = TGG_max[_sg]/10;

	/* _hpri = _pri;		      Bepaal prioriteit toegestaan     */
	if (_hpri < 0) _hpri = 0;	/* 0: Geen prioriteit toegestaan       */
	if (_hpri > 2) _hpri = 2;	/* 1: Alleen aanhouden eigen groenfase */
					/* 2: Prioriteitsrealisatie toegestaan */
	/* Is voertuig al aanwezig? */
	_hbus = 0;
	if (_hvtg > 0) {
	  if      (ovp[_sg][4] == _hvtg) _hbus = 1; /* Ja, dan update */
	  else if (ovp[_sg][9] == _hvtg) _hbus = 2;
	  else if (ovp[_sg][14]== _hvtg) _hbus = 3;
	  else if (ovp[_sg][4] ==  NG  ) _hbus = 1; /* Nee, dan toevoegen */
	  else if (ovp[_sg][9] ==  NG  ) _hbus = 2;
	  else if (ovp[_sg][14]==  NG  ) _hbus = 3;
	} else {
	  if      (ovp[_sg][4] ==  NG  ) _hbus = 1; /* Geen voertuignummer? */
	  else if (ovp[_sg][9] ==  NG  ) _hbus = 2; /* dan alleen toevoegen */
	  else if (ovp[_sg][14]==  NG  ) _hbus = 3;
	}

	if (_hbus > 0) {
		ovp[_sg][(_hbus*5)-2] = _htts;
		ovp[_sg][(_hbus*5)-1] = _hvtg;
		ovp[_sg][(_hbus*5)  ] = _hvgr;
		ovp[_sg][(_hbus*5)+1] = _hpri;
		ovp[_sg][(_hbus*5)+2] = 0;	/* Start aanwezigheidstijd */
	}

	/* Sorteer bussen op basis van tijd tot stopstreep */
	proc_sorteer_bus(_sg,1,2);
	proc_sorteer_bus(_sg,2,3);
	proc_sorteer_bus(_sg,1,2);
     }
   }
 }

 if ((_sg >= 0) && (MM[meovuit] == (_sg+1)) || pri_uit && (ovp[_sg][4] >= 0)) {
   if (ovp[_sg][2] >= 0) {		/* Uitmeldbewaking is gedefinieerd */
     if (_vtg > 0) {			/* Voertuignummer verwijderen */
	if (ovp[_sg][4] == _vtg) {
	  ovp[_sg][3] = ovp[_sg][4] = ovp[_sg][5] = NG;
	  ovp[_sg][6] = ovp[_sg][7] = NG;
	}
	if (ovp[_sg][9] == _vtg) {
	  ovp[_sg][8] = ovp[_sg][9] = ovp[_sg][10]= NG;
	  ovp[_sg][11]= ovp[_sg][12]= NG;
	}
	if (ovp[_sg][14]== _vtg) {
	  ovp[_sg][13]= ovp[_sg][14]= ovp[_sg][15]= NG;
	  ovp[_sg][16]= ovp[_sg][17]= NG;
	}
     }
     if (_vtg <= 0) {			/* Verwijder oudste "0" voertuig   */
	verwijder = 0;			/* indien niet aanwezig dan oudste */
	 				/* voertuig met nummer verwijderen */
	if      (ovp[_sg][4] == 0) verwijder = 1;
	else if (ovp[_sg][9] == 0) verwijder = 2;
	else if (ovp[_sg][14]== 0) verwijder = 3;
	else if (ovp[_sg][4] >  0) verwijder = 1;
	else if (ovp[_sg][9] >  0) verwijder = 2;
	else if (ovp[_sg][14]>  0) verwijder = 3;

	if (verwijder > 0) {
	  ovp[_sg][(verwijder*5)-2] = NG;
	  ovp[_sg][(verwijder*5)-1] = NG;
	  ovp[_sg][(verwijder*5)  ] = NG;
	  ovp[_sg][(verwijder*5)+1] = NG;
	  ovp[_sg][(verwijder*5)+2] = NG;
	}
     }

     /* Sorteer resterende bussen op basis van tijd tot stopstreep */
     proc_sorteer_bus(_sg,1,2);
     proc_sorteer_bus(_sg,2,3);
     proc_sorteer_bus(_sg,1,2);
   }
 }

 if ((awt1 != NG) && (MM[meovinm] == (_sg+1))) {
   if      (MM[awt1] == 0) { MM[awt1] = 1; MM[stp1] = code_stipt; }
   else if (MM[awt2] == 0) { MM[awt2] = 1; MM[stp2] = code_stipt; }
   else if (MM[awt3] == 0) { MM[awt3] = 1; MM[stp3] = code_stipt; }
 }

 if ((awt1 != NG) && TS) {               /* Bewerk buffer */
   if ((MM[awt1] > 0) && (MM[awt1]<1000)) MM[awt1]++;
   if ((MM[awt2] > 0) && (MM[awt2]<1000)) MM[awt2]++;
   if ((MM[awt3] > 0) && (MM[awt3]<1000)) MM[awt3]++;
 }

 if (awt1 != NG) {
   if ((ovp[_sg][14] == NG) && (MM[awt3] > 0)) {
     MM[awt1] = MM[awt2];
     MM[awt2] = MM[awt3];
     MM[awt3] = FALSE;
        
     MM[stp1] = MM[stp2];
     MM[stp2] = MM[stp3];
     MM[stp3] = FALSE;
   }

   if ((ovp[_sg][9] == NG) && (MM[awt2] > 0)) {
     MM[awt1] = MM[awt2];
     MM[awt2] = MM[awt3];
     MM[awt3] = FALSE;
        
     MM[stp1] = MM[stp2];
     MM[stp2] = MM[stp3];
     MM[stp3] = FALSE;
   }

   if ((ovp[_sg][4] == NG) && (MM[awt1] > 0)) {
     MM[awt1] = MM[awt2];
     MM[awt2] = MM[awt3];
     MM[awt3] = FALSE;
        
     MM[stp1] = MM[stp2];
     MM[stp2] = MM[stp3];
     MM[stp3] = FALSE;
   }

   if (MM[stp1] == 1) POV[_sg] |= BIT1;
   if (MM[stp1] == 2) POV[_sg] |= BIT2;
   if (MM[stp1] == 3) POV[_sg] |= BIT3;

   if (MM[stp2] == 1) POV[_sg] |= BIT1;
   if (MM[stp2] == 2) POV[_sg] |= BIT2;
   if (MM[stp2] == 3) POV[_sg] |= BIT3;

   if (MM[stp3] == 1) POV[_sg] |= BIT1;
   if (MM[stp3] == 2) POV[_sg] |= BIT2;
   if (MM[stp3] == 3) POV[_sg] |= BIT3;
 }
}

/* PROCEDURE SORTEER BUSSEN OP AFSTAND TOT STOPSTREEP */
/* ================================================== */

void proc_sorteer_bus(                       /* Fik150101 */
	mulv fc,                          /* Signaalgroep */
	mulv nr1,                         /* Volgnummer 1 */
	mulv nr2)                         /* Volgnummer 2 */
{
 bool wissel = FALSE;
 mulv hbuf1,hbuf2,hbuf3,hbuf4,hbuf5;

 if (ovp[fc][(nr2*5)-1] >= 0) { 		/* 2e bus is aanwezig */
   if (ovp[fc][(nr1*5)-1] < 0) wissel = TRUE;	/* 1e bus niet => wisselen */
   else {
     if ((ovp[fc][(nr2*5)-2] - ovp[fc][(nr2*5)+2]) <
         (ovp[fc][(nr1*5)-2] - ovp[fc][(nr1*5)+2])) wissel = TRUE;
   }
   if (wissel) {
	hbuf1 = ovp[fc][(nr1*5)-2];
	hbuf2 = ovp[fc][(nr1*5)-1];
	hbuf3 = ovp[fc][(nr1*5)  ];
	hbuf4 = ovp[fc][(nr1*5)+1];
	hbuf5 = ovp[fc][(nr1*5)+2];

	ovp[fc][(nr1*5)-2] = ovp[fc][(nr2*5)-2];
	ovp[fc][(nr1*5)-1] = ovp[fc][(nr2*5)-1];
	ovp[fc][(nr1*5)  ] = ovp[fc][(nr2*5)  ];
	ovp[fc][(nr1*5)+1] = ovp[fc][(nr2*5)+1];
	ovp[fc][(nr1*5)+2] = ovp[fc][(nr2*5)+2];

	ovp[fc][(nr2*5)-2] = hbuf1;
	ovp[fc][(nr2*5)-1] = hbuf2;
	ovp[fc][(nr2*5)  ] = hbuf3;
	ovp[fc][(nr2*5)+1] = hbuf4;
	ovp[fc][(nr2*5)+2] = hbuf5;
   }
 }
}

/* PROCEDURE BEPAAL CYCLISCHE REALISATIE */
/* ===================================== */

void proc_cyc_crsv(                   /* Fik170831  */
  mulv fc,                            /* fasecyclus */
  mulv cvas1,                         /* PRM[cprim] */
  mulv cvas2)                         /* NG -> voorbereid op 2 realisatie's */
{
  HVAS[fc] &=(~BIT5);
    
  if (cvas1 != NG) {
    if ((aantal_VAS == 1) && (PRM[cvas1] >   1) ||
        (aantal_VAS == 2) && (PRM[cvas1] >   3) ||
        (aantal_VAS == 3) && (PRM[cvas1] >   7) ||
        (aantal_VAS == 4) && (PRM[cvas1] >  15) ||
        (aantal_VAS == 5) && (PRM[cvas1] >  31) ||
        (aantal_VAS == 6) && (PRM[cvas1] >  63) ||
        (aantal_VAS == 7) && (PRM[cvas1] > 127) ||
        (aantal_VAS == 8) && (PRM[cvas1] > 255) ||
        (aantal_VAS == 9) && (PRM[cvas1] > 511)) {
      PRM[cvas1] = 0;
      CIF_PARM1WIJZAP = -2;
    }
  }
    
  if (cvas2 != NG) {
    PRM[cvas2] = 0;
    CIF_PARM1WIJZAP = -2;
  }
    
  if (!BL[fc])
  {
    if (cvas1 != NG)
    {
      if ((AKTPRG==1) && (PRM[cvas1]&BIT0)) HVAS[fc] |= BIT5;
      if ((AKTPRG==2) && (PRM[cvas1]&BIT1)) HVAS[fc] |= BIT5;
      if ((AKTPRG==3) && (PRM[cvas1]&BIT2)) HVAS[fc] |= BIT5;
      if ((AKTPRG==4) && (PRM[cvas1]&BIT3)) HVAS[fc] |= BIT5;
      if ((AKTPRG==5) && (PRM[cvas1]&BIT4)) HVAS[fc] |= BIT5;
      if ((AKTPRG==6) && (PRM[cvas1]&BIT5)) HVAS[fc] |= BIT5;
      if ((AKTPRG==7) && (PRM[cvas1]&BIT6)) HVAS[fc] |= BIT5;
      if ((AKTPRG==8) && (PRM[cvas1]&BIT7)) HVAS[fc] |= BIT5;
      if ((AKTPRG==9) && (PRM[cvas1]>=256)) HVAS[fc] |= BIT5;
    }
  }
}
  
/* PROCEDURE BEPAAL CYCLISCHE REALISATIE TIJDENS DVM */
/* ================================================= */

void proc_cyc_crsv_dvm(               /* Fik170831   */
  mulv fc,                            /* fasecyclus  */
  mulv cdvm1,                         /* PRM[cdvmxx] */
  mulv cdvm2)                         /* NG -> voorbereid op 2 realisatie's */
{
  if (cdvm1 != NG)
  {
    if (PRM[cdvm1] > 63)
    {
      PRM[cdvm1] = 0;
      CIF_PARM1WIJZAP = -2;
    }
  }
    
  if (cdvm2 != NG)
  {
    PRM[cdvm2] = 0;
    CIF_PARM1WIJZAP = -2;
  }
    
  if (!BL[fc])
  {
    if (cdvm1 != NG)
    {
      if ((AKTPRG==21) && (PRM[cdvm1]&BIT0)) HVAS[fc] |= BIT5;
      if ((AKTPRG==22) && (PRM[cdvm1]&BIT1)) HVAS[fc] |= BIT5;
      if ((AKTPRG==23) && (PRM[cdvm1]&BIT2)) HVAS[fc] |= BIT5;
      if ((AKTPRG==24) && (PRM[cdvm1]&BIT3)) HVAS[fc] |= BIT5;
      if ((AKTPRG==25) && (PRM[cdvm1]&BIT4)) HVAS[fc] |= BIT5;
      if ((AKTPRG==26) && (PRM[cdvm1]&BIT5)) HVAS[fc] |= BIT5;
    }
  }
}
  
/* PROCEDURE BEPAAL AKTUELE PRIMAIRE GEBIEDEN */
/* ========================================== */

void proc_akt_crsv(                          /* Fik170831 */
	mulv fc,                        /* Signaalgroep       */
	mulv kapov,                     /* PRM kapov          */
	mulv tl1,                       /* PRM Einde uitstel  */
	mulv tl2,                       /* PRM Start primair  */
	mulv tl3,                       /* PRM Start verleng  */
	mulv tl4,                       /* PRM Einde primair  */
	mulv tl5,                       /* PRM Einde groen    */
	mulv abm,                       /* PRM Afbreekmethode */
	mulv pri)                       /* PRM Prio.buiten primair gebied */
{
 mulv prog_keuze = 20;
 mulv SPRIM;
 mulv VASX;
 mulv _ABM;
 mulv _PRI;
 mulv HSTAP;
 bool PRIM_GEBIED;
 mulv waarde = 0;
 mulv percen = 0;

 if (PROGKEUZE_CS != NG) prog_keuze = PRM[PROGKEUZE_CS];

 if (abm != NG) {
   _ABM = PRM[abm];
 } else {
   _ABM = 7;
 }

 if (pri != NG) {
   _PRI = PRM[pri];
 } else {
   _PRI = 0;
 }

 if (_ABM&BIT0) PVAS[fc] |= BIT4;	/* Uitstel     groen door OV toegestaan */
 if (_ABM&BIT1) PVAS[fc] |= BIT5;	/* Afbreken    groen door OV toegestaan */
 if (_ABM&BIT2) PVAS[fc] |= BIT6;	/* Doorsnijden groen door OV toegestaan */

 if (_PRI&BIT0) PVS2[fc] |= BIT0;	/* Prioriteit OV voor einde uitstel toegestaan */
 if (_PRI&BIT1) PVS2[fc] |= BIT1;	/* Prioriteit OV na afkappunt       toegestaan */
						/* Bepaal MAXG volgens raster */
 if ((PRM[tl2] > 0) && (PRM[tl4] > 0)
  && (PRM[tl2] <= CTYD)
  && (PRM[tl4] <= CTYD)) {
   if (PRM[tl4] >= PRM[tl2]) {
     MAXG[fc] = PRM[tl4] - PRM[tl2];
   } else {
     MAXG[fc] = CTYD -
		PRM[tl2] + PRM[tl4];
   }
 }

 if (kapov !=  NG) percen = PRM[kapov];
 if (percen > 100) percen = 100;
 waarde = (percen * MAXG[fc])/100;
 if (waarde > KAPM[fc]) KAPM[fc] = waarde;

 if (ESGD) {
   PEG[fc] = TGG_max[fc]/10;
   if (TFG_max[fc]/10 > PEG[fc]) PEG[fc] = TFG_max[fc]/10;
   if (KAPM[fc]       > PEG[fc]) PEG[fc] = KAPM[fc];

   if (G[fc]) {
      if (KG[fc] >= KAPM[fc]) { PEG[fc] = 0; }
      else                    { PEG[fc] = KAPM[fc] - KG[fc]; }
   } else { PEG[fc] = 0; }

   if (MG[fc] && (KG[fc] >= TGG_max[fc]/10)) PEG[fc] = 0;
 }
						/* Bepaal cyclische realisatie */
 if ((HVAS[fc]&BIT5) && (prog_keuze != 10) 
	             && (prog_keuze != 11) && !BL[fc]) { HVAS[fc] |=   BIT5;  }
 else						       { HVAS[fc] &= (~BIT5); }
 if ((prog_keuze == 30) && !BL[fc]) HVAS[fc] |= BIT5;
 
 VAS1[fc] = VAS3[fc] = VAS5[fc] = 0;
 VAS4[fc] = PRM[tl4];

 SPRIM = PRM[tl2];
 if (STAP == SPRIM) HVAS[fc] &= (~BIT4);	/* Reset versnelde realisatie  */

 if (SVAS) {					/* Als op SVAS nog ALTM[] groen gemaakt kan worden dan voorkom overslag */
	SVS2[fc] = 0;
	PRIM_GEBIED = FALSE;

	if (!G[fc] && (SPRIM != 0) && (VAS4[fc] != 0) && (SPRIM != VAS4[fc])) {

		HSTAP = STAP + TTR[fc] + 2;	/* Voorkom afrondingsfouten */
		if (HSTAP > CTYD) HSTAP -= CTYD;

		if (VAS4[fc] > SPRIM) {
			if ((HSTAP >= SPRIM) && (HSTAP < VAS4[fc])) PRIM_GEBIED = TRUE;
		} else {
			if ((HSTAP >= SPRIM) || (HSTAP < VAS4[fc])) PRIM_GEBIED = TRUE;
		}
		if (PRIM_GEBIED) {
			if (HSTAP != SPRIM) SVS2[fc] = HSTAP;	/* SVS2 is altijd tenminste 1 sec. voorbij SPRIM */
			HSTAP += ALTM[fc];
			if (HSTAP > CTYD) HSTAP -= CTYD;

			PRIM_GEBIED = FALSE;
			if (VAS4[fc] > SPRIM) {
				if ((HSTAP >= SPRIM) && (HSTAP <= VAS4[fc])) PRIM_GEBIED = TRUE;
			} else {
				if ((HSTAP >= SPRIM) || (HSTAP <= VAS4[fc])) PRIM_GEBIED = TRUE;
			}
			if (!PRIM_GEBIED) {
				SVS2[fc] = 0;
			} else {				/* Als net niet past dan gebied 1 sec. naar voren schuiven */
								/* -> overslag op SVAS is in dat geval dus afh. van de afrondingen */
				if ((HSTAP == VAS4[fc]) && (SVS2[fc] != 0)) {
					SVS2[fc]--;
					if (SVS2[fc] == 0) SVS2[fc] = CTYD;
				}
			}
		}
	}
 }

 if (G[fc] || (HVAS[fc]&BIT3)) SVS2[fc] = 0;	/* Reset speciale start waarde bij SVAS */

 if (HVAS[fc]&BIT4) {
	VAS2[fc] = STAP;
 } else {
	if (SVS2[fc] == 0) VAS2[fc] = PRM[tl2];
	else		   VAS2[fc] = SVS2[fc];
 }

 if ((prog_keuze != 10) && (prog_keuze != 11)) {
	if (tl1 >= 0) VAS1[fc] = PRM[tl1];
	if (tl3 >= 0) VAS3[fc] = PRM[tl3];
	if (tl5 >= 0) VAS5[fc] = PRM[tl5];
 }
 if (prog_keuze == 30) VAS3[fc] = PRM[tl4];

 /* Bepaal primair gebied conform instellingen */
 /* ------------------------------------------ */

 HVAS[fc] &= (~BIT7);
 if ((SPRIM != 0)  && (VAS4[fc] != 0) && (SPRIM != VAS4[fc])) {
	if (VAS4[fc]>SPRIM) {
	  if ((STAP>=SPRIM) && (STAP<VAS4[fc])) HVAS[fc] |= BIT7;
	} else {
	  if ((STAP>=SPRIM) || (STAP<VAS4[fc])) HVAS[fc] |= BIT7;
	}
 }

 if ((VAS2[fc] != 0) && (VAS4[fc] != 0) && (/* VAS2[fc] */ SPRIM != VAS4[fc])) {

	/* Bepaal primair gebied incl.vooruitrealiseren */
	/* -------------------------------------------- */

	if (VAS4[fc]>VAS2[fc]) {  /* Start primair VOOR einde primair */

	  if ((STAP>=VAS2[fc]) && (STAP<VAS4[fc]) && !(HVAS[fc]&BIT3)) {
	  	HVAS[fc] |= BIT0;
	  	if ((STAP<VAS3[fc]) && (VAS3[fc]<=VAS4[fc]))
			 { HVAS[fc] |=   BIT2 ; }
		else	 { HVAS[fc] &= (~BIT2); }
	  } else {
			   HVAS[fc] &= (~BIT0);
			   HVAS[fc] &= (~BIT2);
			   HVAS[fc] &= (~BIT4);
			   PVS2[fc] &= (~BIT4);
	  }

	} else {		  /* Start primair NA einde primair */

	if (((STAP>=VAS2[fc]) || (STAP<VAS4[fc])) && !(HVAS[fc]&BIT3)) {
		HVAS[fc] |= BIT0;
		if ((VAS3[fc]!=0) &&
		   ((STAP>=VAS2[fc]) && ((STAP<VAS3[fc])
						|| (VAS3[fc]<=VAS4[fc]))
		||  (STAP<=VAS4[fc]) &&  (STAP<VAS3[fc])
						&& (VAS3[fc]<=VAS4[fc])))
			 { HVAS[fc] |=   BIT2 ; }
		else	 { HVAS[fc] &= (~BIT2); }
	} else {
			   HVAS[fc] &= (~BIT0);
			   HVAS[fc] &= (~BIT2);
			   HVAS[fc] &= (~BIT4);
			   PVS2[fc] &= (~BIT4);
               }
	}

	/* Bepaal blokkeer overgang naar groen */
	/* ----------------------------------- */

	if ((VAS1[fc] != 0) && (VAS1[fc] != VAS4[fc])) {

	  if (VAS1[fc]>VAS4[fc]) {   /* Start uitstel VOOR einde uitstel */

	    if ((STAP>=VAS4[fc]) && (STAP<VAS1[fc])
				 || (HVAS[fc]&BIT3)) { HVAS[fc] |=   BIT1 ;
	    } else {				       HVAS[fc] &= (~BIT1); }
	    
	  } else {		     /* Start uitstel NA einde uitstel   */

	    if ((STAP>=VAS4[fc]) || (STAP<VAS1[fc])
				 || (HVAS[fc]&BIT3)) { HVAS[fc] |=   BIT1 ;
	    } else {				       HVAS[fc] &= (~BIT1); }
	  }
	} else { HVAS[fc] &= (~BIT1); }

	/* Bepaal forceer overgang naar rood */
	/* --------------------------------- */

	if (VAS1[fc] != 0) VASX = VAS1[fc];	/* Einde NIET-groen-gebied */
	else		   VASX = VAS2[fc];	/* afh. van def.VAS1[fc]   */

	if ((VAS5[fc] != 0) && (VASX != VAS5[fc])) {

	  if (VASX>VAS5[fc]) {       /* Start forceer VOOR einde forceer */

	    if ((STAP>=VAS5[fc]) && (STAP<VASX)
		      || (HVAS[fc]&BIT3) && !G[fc]) { HVAS[fc] |=   BIT6 ;
	    } else {				      HVAS[fc] &= (~BIT6); }
	    
	  } else {		     /* Start forceer NA einde forceer   */

	    if ((STAP>=VAS5[fc]) || (STAP<VASX)
		      || (HVAS[fc]&BIT3) && !G[fc]) { HVAS[fc] |=   BIT6 ;
	    } else {				      HVAS[fc] &= (~BIT6); }
	  }
	} else { HVAS[fc] &= (~BIT6); }

   } else { HVAS[fc] &= (~BIT0); /* Indien Start en/of einde primair NIET */
	    HVAS[fc] &= (~BIT1); /* gedefinieerd dan richting ALTIJD als  */
	    HVAS[fc] &= (~BIT2); /* gerealiseerd beschouwen		  */
	    HVAS[fc] |=   BIT3 ;
	    HVAS[fc] &= (~BIT4);
	    HVAS[fc] &= (~BIT5);
	    HVAS[fc] &= (~BIT6);
	    PVS2[fc] &= (~BIT4);
 }
}

/* PROCEDURE BEPAAL MINIMAAL PRIMAIR GEBIED TBV OV-INGREPEN */
/* ======================================================== */

void proc_min_prim_crsv(void)                  /* Fik150101 */
{
   mulv i;
   mulv HEPR,REST;

   for (i=0; i<FCMAX; i++) {
	SDSN[i] = 255;	/* Initialiseer doorsnijden start */
	EDSN[i] = 0;	/*       ... en doorsnijden einde */

	if (AKTPRG && (HVAS[i]&BIT0)) {					/* Primaire realisatie aktief */
		HEPR = KAPM[i];
		if (KAPM[i] >= ALTM[i]) REST = ALTM[i];
		else                    REST = KAPM[i];
		if (G[i]) {
			if (TS && (KPGR[i]<255)) KPGR[i]++;		/* Bepaal duur primair groen  */
			if (KG[i] > KPGR[i]) KPGR[i] = KG[i];
			if ((PVAS[i]&BIT6) && !(PVS2[i]&BIT2)) {	/* Doorsnijden is toegestaan */
				if (KG[i] > REST) SDSN[i] = 0;
				else              SDSN[i] = REST - KG[i];
			}
		}
		if (KPGR[i] >= HEPR) HEPR  = 0;				/* Bereken benodigd groen */
		else                 HEPR -= KPGR[i];			/* na terugkomen          */
		if (HEPR < REST)     HEPR  = REST;	

		if (STAP <= VAS4[i]) {
			REST = VAS4[i] - STAP;
			if (REST <= HEPR) EDSN[i] = 0;
			else              EDSN[i] = REST - HEPR;
		} else {
			REST = CTYD - STAP + VAS4[i];
			if (REST <= HEPR) EDSN[i] = 0;
			else              EDSN[i] = REST - HEPR;
		}
		if (KPGR[i] >= KAPM[i]) { 
			EDSN[i] = 255;
			PVS2[i] &= (~BIT4);
		}
	} else {
		KPGR[i] = 0;						/* Reset duur primair groen */
	}
   }

   for (i=0; i<FCMAX; i++) {
	VS2M[i] = VAS2[i];						/* default gelijk aan start primair */
	if (PVAS[i]&BIT4) {						/* corrigeer als uitstel is toegestaan */
		if (VAS2[i] <= VAS4[i]) {
		  if ((VAS4[i] - KAPM[i]) > VAS2[i]) VS2M[i] = VAS4[i] - KAPM[i];
		} else {
		  HEPR = VAS4[i] + CTYD;
		  if (VAS2[i] <= HEPR) {
		    if ((HEPR - KAPM[i]) > VAS2[i]) {
			VS2M[i] = HEPR - KAPM[i];
			if (VS2M[i] > CTYD) VS2M[i] -= CTYD;
		    }
		  }
		}
	}
	if (PVS2[i]&BIT4) {						/* Corrigeer VS2M[] indien richting */
		VS2M[i] = STAP + EDSN[i];				/* terug gaat komen na afkappen     */
		if (VS2M[i] > CTYD) VS2M[i] -= CTYD;
	}
   }
   for (i=0; i<aant_fts; i++) {
     proc_min_prim_vtg(fts[i][0],NG,fts[i][1],NG,fts[i][2],NG,NG);
   }
   for (i=0; i<aant_bbv; i++) {
     proc_min_prim_vtg(bbv[i][0],bbv[i][1],bbv[i][4],NG,bbv[i][5],NG,NG);
   }
   for (i=0; i<aant_gov; i++) {
     proc_min_prim_vtg(gov[i][0],gov[i][1],gov[i][7]
			,gov[i][8],gov[i][13],gov[i][14],gov[i][22]);
   }
   proc_min_prim_hki();

   for (i=0; i<FCMAX; i++) {

	if ((VS2M[i] != 0) && (VAS4[i] != 0) && (VS2M[i] != VAS4[i])) {

		/* Bepaal garantie gebied primaire realisatie */
		/* ------------------------------------------ */

		if (VAS4[i]>VS2M[i]) {	/* Start primair VOOR einde primair */

		  if ((STAP>=VS2M[i]) && (STAP<VAS4[i]) && !(HVAS[i]&BIT3)) {
		  	PVAS[i] |= BIT0;
		  } else {
			PVAS[i] &= (~BIT0);
		  }

		} else {		/* Start primair NA einde primair */

		  if (((STAP>=VS2M[i]) || (STAP<VAS4[i])) && !(HVAS[i]&BIT3)) {
			PVAS[i] |= BIT0;
		  } else {
			PVAS[i] &= (~BIT0);
		  }
		}
	}
   }
}

/* CORRIGEER MIN.PRIMAIR GEBIED TBV OV-INGREPEN AGV GEKOPPELDE OVERSTEKEN */
/* ====================================================================== */

void proc_min_prim_vtg(                      /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv fc3,                            /* Signaalgroep 3 */
	mulv fc4,                            /* Signaalgroep 4 */
	mulv fc5,                            /* Signaalgroep 5 */
	mulv fc6,                            /* Signaalgroep 6 */
	mulv fc7)                            /* Signaalgroep 7 */
{
 mulv _minVS2 = (2*CTYD)+1;
 mulv HSPR;
 mulv i,_fc[7];

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;
 _fc[3]=fc4; _fc[4]=fc5; _fc[5]=fc6; _fc[6]=fc7;

 /* Bepaal VS2M */
 /* ----------- */
 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
	if (VS2M[_fc[i]] < VAS2[_fc[i]]) HSPR = VS2M[_fc[i]] + CTYD;
	else				 HSPR = VS2M[_fc[i]];
	if (HSPR < _minVS2) _minVS2 = HSPR;
   }
 }
 if (_minVS2 > (2*CTYD)) _minVS2 = 0;
 if (_minVS2 > CTYD) _minVS2 -= CTYD;

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) VS2M[_fc[i]] = _minVS2;
 }
}

/* CORRIGEER MIN.PRIMAIR GEBIED TBV OV-INGREPEN AGV SERIELE KOPPELING */
/* ================================================================== */

void proc_min_prim_hki(void)                 /* Fik150101 */
{
 mulv fc1,fc2,vs12,fc3,vs23,stat;
 mulv voorstart;
 mulv correctie;
 mulv _VS2M;
 mulv i,x;

 for (x=0; x<3; x++) {							/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1 = hki[i][0];
	fc2 = hki[i][1];
	vs12= hki[i][2];
	fc3 = hki[i][10];
	vs23= hki[i][11];
	stat= hki[i][20];

	if (stat >= 2) {						/* koppeling is aktief */
		if ((x == 1) && (TS || N_ts)) {
			if (XG_hki[i][fc2] && (hki[i][9]<255)) hki[i][9]++;
			if (fc3 >= 0) {					/* pas gaan tellen als fc2 groen geworden is! */
				if (XG_hki[i][fc3] && (hki[i][18]<255) && (G[fc2] || (hki[i][18]>0))) hki[i][18]++;
			}
		}

		if (G[fc2] || !XG_hki[i][fc2]) hki[i][9]=0;
		if (fc3 >= 0) {
			if (G[fc3] || !XG_hki[i][fc3]) hki[i][18]=0;
		}

		if (G[fc1] && (HVAS[fc1]&BIT0) && XG_hki[i][fc2]) {
			XG_hki[i][fc2] |= BIT3;
			XG[fc2] |= BIT3;
			if (fc3 >= 0) {
				if (XG_hki[i][fc3]) {
					XG_hki[i][fc3] |= BIT5;
					XG[fc3] |= BIT5;
				}
			}
		}
		if (fc3 >= 0) {
			if (G[fc2] && (HVAS[fc2]&BIT0) && XG_hki[i][fc3]) {
				XG_hki[i][fc3] |= BIT3;
				XG[fc3] |= BIT3;
			}
		}

		if (XG_hki[i][fc2]&BIT3) {				/* _VS2M is het startgroen moment van de voeding */
			if (hki[i][9] > CTYD) _VS2M = VAS2[fc1];	/* fout opgetreden */
			else {
				_VS2M = STAP - hki[i][9];
				if (_VS2M < 0) _VS2M += CTYD;
			}
		} else {
			_VS2M = VS2M[fc1];
		}							/* voorstart is de garandeerde resulterende voorstart */
		if (_VS2M <= VS2M[fc2]) voorstart = VS2M[fc2] - _VS2M;
		else voorstart = VS2M[fc2] + CTYD - _VS2M;

		if (voorstart > (T_max[vs12]/10)) {			/* te groot? dan pas VS2M[] volgrichting aan */
			correctie = voorstart - (T_max[vs12]/10);
			VS2M[fc2] -= correctie;
			if (VS2M[fc2] <= 0) VS2M[fc2] += CTYD;

			if (VAS2[fc2] <= VAS4[fc2]) {			/* VS2M[] kan nooit kleiner worden dan VAS2[] */
				if ((VS2M[fc2] < VAS2[fc2]) || (VS2M[fc2] > VAS4[fc2])) {
					VS2M[fc2] = VAS2[fc2];
				}
			} else {
				if ((VS2M[fc2] < VAS2[fc2]) && (VS2M[fc2] > VAS4[fc2])) {
					VS2M[fc2] = VAS2[fc2];
				}
			}
		}

		if (fc3 >= 0) {						/* corrigeer VS2M[fc3] tov startgroen fc2 */
			if (XG_hki[i][fc3]&BIT3) {
				if (hki[i][18] > CTYD) _VS2M = VAS2[fc2];
				else {
					_VS2M = STAP - hki[i][18];
					if (_VS2M < 0) _VS2M += CTYD;
				}
			} else {
				_VS2M = VS2M[fc2];
			}
			if (_VS2M <= VS2M[fc3]) voorstart = VS2M[fc3] - _VS2M;
			else voorstart = VS2M[fc3] + CTYD - _VS2M;

			if (voorstart > (T_max[vs23]/10)) {
				correctie = voorstart - (T_max[vs23]/10);
				VS2M[fc3] -= correctie;
				if (VS2M[fc3] <= 0) VS2M[fc3] += CTYD;

				if (VAS2[fc3] <= VAS4[fc3]) {
					if ((VS2M[fc3] < VAS2[fc3]) || (VS2M[fc3] > VAS4[fc3])) {
						VS2M[fc3] = VAS2[fc3];
					}
				} else {
					if ((VS2M[fc3] < VAS2[fc3]) && (VS2M[fc3] > VAS4[fc3])) {
						VS2M[fc3] = VAS2[fc3];
					}
				}
			}

			if (XG_hki[i][fc3]&BIT5) {			/* corrigeer VS2M[fc3] tov startgroen fc1 */
				if (hki[i][9] > CTYD) _VS2M = VAS2[fc1];
				else {
					_VS2M = STAP - hki[i][9];
					if (_VS2M < 0) _VS2M += CTYD;
				}
			} else {
				_VS2M = VS2M[fc1];
			}
			if (_VS2M <= VS2M[fc3]) voorstart = VS2M[fc3] - _VS2M;
			else voorstart = VS2M[fc3] + CTYD - _VS2M;

			if (voorstart > ((T_max[vs12]/10) + (T_max[vs23]/10))) {
				correctie = voorstart - (T_max[vs12]/10) - (T_max[vs23]/10);
				VS2M[fc3] -= correctie;
				if (VS2M[fc3] <= 0) VS2M[fc3] += CTYD;

				if (VAS2[fc3] <= VAS4[fc3]) {
					if ((VS2M[fc3] < VAS2[fc3]) || (VS2M[fc3] > VAS4[fc3])) {
						VS2M[fc3] = VAS2[fc3];
					}
				} else {
					if ((VS2M[fc3] < VAS2[fc3]) && (VS2M[fc3] > VAS4[fc3])) {
						VS2M[fc3] = VAS2[fc3];
					}
				}
			}
		}

	} else {							/* koppeling is niet aktief */
		hki[i][9] = 0;
		hki[i][18]= 0;
	}
    }
 }
}

/* PROCEDURE INITIALISEER ONDERGRENZEN VOOR AFKAPPEN, ALTM[] EN KAPM[] */
/* =================================================================== */

void init_kapm_crsv(void)                    /* Fik170831 */
{
 mulv i,garant;

 for (i=0; i<FCMAX; i++) {
	 garant  = TGG_max[i]/10;
	 /* ALTM[i] = KAPM[i] = */ MAXG[i] = TFG_max[i]/10;
	 if (garant > ALTM[i])
	 /* ALTM[i] = KAPM[i] = */ MAXG[i] = garant;
	 PVAS[i] &= (~BIT7);
 }
}

/* PROCEDURE CORRIGEER ALTM[] EN KAPM[] IVM GEKOPPELDE OVERSTEKEN */
/* ============================================================== */

void proc_corr_kapm(void)                    /* Fik170831 */
{
 mulv i;

 for (i=0; i<FCMAX; i++) {
   if (!G[i] && BL[i]) VA[i] = FALSE;        /* Reset aanvraag agv BL_optie */
 }

 for (i=0; i<aant_bbv; i++) {
	proc_kapm_crsv_bb(bbv[i][0],bbv[i][1],bbv[i][2],bbv[i][3],bbv[i][8]);
 }
 for (i=0; i<aant_gov; i++) {
	proc_kapm_crsv_go(gov[i][0],gov[i][1],gov[i][4],gov[i][5],
			  gov[i][6],gov[i][7],gov[i][8],gov[i][10],gov[i][11],
			  gov[i][13],gov[i][14],gov[i][16],gov[i][17],gov[i][21],
			  gov[i][22],gov[i][25],gov[i][26],gov[i][27],gov[i][28]);
 }
 if (AKTPRG) proc_min_prim_crsv();
}

/* PROCEDURE BEPAAL ALTM[] EN KAPM[] VOOR PROGRESSIEVE VOETGANGERSOVERSTEEK */
/* ======================================================================== */

void proc_kapm_crsv_bb(                      /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpt12,                          /* KoppelTIJD eg1 -> eg2 */
	mulv gkpg_a,                         /* Schakelaar geen kop.garantie bij alt  */
	mulv gkpg_p)                         /* Schakelaar geen kop.garantie bij prio */
{
 mulv kop12 = 0;

 if ((fc1 >= 0) && (fc2 >=0)) {
	kop12 = T_max[kpt12]/10;
	if ((ALTM[fc1] + kop12) > ALTM[fc2]) {
		if (!SCH[gkpg_a]) ALTM[fc2] = ALTM[fc1] + kop12;
	}
	if ((KAPM[fc1] + kop12) > KAPM[fc2]) {
		if (!SCH[gkpg_p]) KAPM[fc2] = KAPM[fc1] + kop12;
	}
	if (SCH[gkpg_p]) {                   /* vlag voetganger mag afgebroken worden door OV */
		PVAS[fc1] |= BIT7;
		PVAS[fc2] |= BIT7;
	}
 }
}

/* PROCEDURE BEPAAL ALTM[] EN KAPM[] VOOR GESCHEIDEN VOETGANGERSOVERSTEEK */
/* ====================================================================== */

void proc_kapm_crsv_go(                      /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpt12,                          /* KoppelTIJD sg1 -> eg2 */
	mulv kpt21,                          /* KoppelTIJD sg2 -> eg1 */
	mulv gkpg_a,                         /* Schakelaar geen kop.garantie bij alt  */
	mulv fc3,                            /* Gekoppelde fietser 1                  */
	mulv fc4,                            /* Gekoppelde fietser 1 volg             */
	mulv kpt3s,                          /* Koppeltijd vanaf start fc3            */
	mulv kpt3e,                          /* Koppeltijd vanaf einde fc3            */
	mulv fc5,                            /* Gekoppelde fietser 2                  */
	mulv fc6,                            /* Gekoppelde fietser 2 volg             */
	mulv kpt5s,                          /* Koppeltijd vanaf start fc5            */
	mulv kpt5e,                          /* Koppeltijd vanaf einde fc5            */
	mulv gkpg_p,                         /* Schakelaar geen kop.garantie bij prio */

	mulv vtg3,                           /* Signaalgroep 3e voetganger            */
	mulv kpt13,                          /* KoppelTIJD sg1 -> eg3                 */
	mulv kpt23,                          /* KoppelTIJD sg2 -> eg3                 */
	mulv kpt31,                          /* KoppelTIJD sg3 -> eg1                 */
	mulv kpt32)                          /* KoppelTIJD sg3 -> eg2                 */

{
 mulv kop12 = 0;
 mulv kop21 = 0;
 mulv kop3s = 0;
 mulv kop3e = 0;
 mulv kop5s = 0;
 mulv kop5e = 0;

 mulv akt12 = 0;
 mulv akt21 = 0;

 mulv kop13 = 0;   /* toevoegingen i.v.m. drie-traps-voetganger */
 mulv kop23 = 0;
 mulv kop31 = 0;
 mulv kop32 = 0;

 mulv akt13 = 0;
 mulv akt23 = 0;
 mulv akt31 = 0;
 mulv akt32 = 0;

 if ((fc1 >= 0) && (fc2 >= 0)) {
	kop12 = T_max[kpt12]/10;
	kop21 = T_max[kpt21]/10;
	if ((kop21 > ALTM[fc1]) && (!G[fc2] || SG[fc2]) && !SCH[gkpg_a]) ALTM[fc1] = kop21;
	if ((kop21 > KAPM[fc1]) && (!G[fc2] || SG[fc2]) && !SCH[gkpg_p]) KAPM[fc1] = kop21;
	if ((kop12 > ALTM[fc2]) && (!G[fc1] || SG[fc1]) && !SCH[gkpg_a]) ALTM[fc2] = kop12;
	if ((kop12 > KAPM[fc2]) && (!G[fc1] || SG[fc1]) && !SCH[gkpg_p]) KAPM[fc2] = kop12;

	if (G[fc1] && T[kpt12]) {
		akt12 = T_timer[kpt12]/10;
		if (((kop12 - akt12) > ALTM[fc2]) && !SCH[gkpg_a]) ALTM[fc2] = kop12 - akt12;
		if (((kop12 - akt12) > KAPM[fc2]) && !SCH[gkpg_p]) KAPM[fc2] = kop12 - akt12;
	}
	if (G[fc2] && T[kpt21]) {
		akt21 = T_timer[kpt21]/10;
		if (((kop21 - akt21) > ALTM[fc1]) && !SCH[gkpg_a]) ALTM[fc1] = kop21 - akt21;
		if (((kop21 - akt21) > KAPM[fc1]) && !SCH[gkpg_p]) KAPM[fc1] = kop21 - akt21;
	}
	if (SCH[gkpg_p]) {		 /* vlag voetganger mag afgebroken worden door OV */
		PVAS[fc1] |= BIT7;
		PVAS[fc2] |= BIT7;
	}
 }
 
 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0)) { /* correctie i.g.v. drie-traps-voetganger */
	kop13 = T_max[kpt13]/10;
	kop23 = T_max[kpt23]/10;
	kop31 = T_max[kpt31]/10;
	kop32 = T_max[kpt32]/10;

	if ((kop31 > ALTM[fc1]) && (!G[vtg3] || SG[vtg3]) && !SCH[gkpg_a]) ALTM[fc1] = kop31;
	if ((kop31 > KAPM[fc1]) && (!G[vtg3] || SG[vtg3]) && !SCH[gkpg_p]) KAPM[fc1] = kop31;
	if ((kop32 > ALTM[fc2]) && (!G[vtg3] || SG[vtg3]) && !SCH[gkpg_a]) ALTM[fc2] = kop32;
	if ((kop32 > KAPM[fc2]) && (!G[vtg3] || SG[vtg3]) && !SCH[gkpg_p]) KAPM[fc2] = kop32;

	if ((kop13 > ALTM[vtg3]) && (!G[fc1] || SG[fc1]) && !SCH[gkpg_a]) ALTM[vtg3] = kop13;
	if ((kop13 > KAPM[vtg3]) && (!G[fc1] || SG[fc1]) && !SCH[gkpg_p]) KAPM[vtg3] = kop13;
	if ((kop23 > ALTM[vtg3]) && (!G[fc2] || SG[fc2]) && !SCH[gkpg_a]) ALTM[vtg3] = kop23;
	if ((kop23 > KAPM[vtg3]) && (!G[fc2] || SG[fc2]) && !SCH[gkpg_p]) KAPM[vtg3] = kop23;

	if (G[fc1] && T[kpt13]) {
		akt13 = T_timer[kpt13]/10;
		if (((kop13 - akt13) > ALTM[vtg3]) && !SCH[gkpg_a]) ALTM[vtg3] = kop13 - akt13;
		if (((kop13 - akt13) > KAPM[vtg3]) && !SCH[gkpg_p]) KAPM[vtg3] = kop13 - akt13;
	}
	if (G[fc2] && T[kpt23]) {
		akt23 = T_timer[kpt23]/10;
		if (((kop23 - akt23) > ALTM[vtg3]) && !SCH[gkpg_a]) ALTM[vtg3] = kop23 - akt23;
		if (((kop23 - akt23) > KAPM[vtg3]) && !SCH[gkpg_p]) KAPM[vtg3] = kop23 - akt23;
	}
	if (G[vtg3] && T[kpt31]) {
		akt31 = T_timer[kpt31]/10;
		if (((kop31 - akt31) > ALTM[fc1]) && !SCH[gkpg_a]) ALTM[fc1] = kop31 - akt31;
		if (((kop31 - akt31) > KAPM[fc1]) && !SCH[gkpg_p]) KAPM[fc1] = kop31 - akt31;
	}
	if (G[vtg3] && T[kpt32]) {
		akt32 = T_timer[kpt32]/10;
		if (((kop32 - akt32) > ALTM[fc2]) && !SCH[gkpg_a]) ALTM[fc2] = kop32 - akt32;
		if (((kop32 - akt32) > KAPM[fc2]) && !SCH[gkpg_p]) KAPM[fc2] = kop32 - akt32;
	}
	if (SCH[gkpg_p]) {		 /* vlag voetganger mag afgebroken worden door OV */
		PVAS[vtg3] |= BIT7;
	}
 }

 if ((fc3 >= 0) && (fc4 >= 0)) {
	kop3s = T_max[kpt3s]/10;
	kop3e = T_max[kpt3e]/10;
	if (kop3s > ALTM[fc4]) ALTM[fc4] = kop3s;
	if (kop3s > KAPM[fc4]) KAPM[fc4] = kop3s;
	if ((ALTM[fc3] + kop3e) > ALTM[fc4])
		ALTM[fc4] = ALTM[fc3] + kop3e;
	if ((KAPM[fc3] + kop3e) > KAPM[fc4])
		KAPM[fc4] = KAPM[fc3] + kop3e;
 }
 if ((fc5 >= 0) && (fc6 >= 0)) {
	kop5s = T_max[kpt5s]/10;
	kop5e = T_max[kpt5e]/10;
	if (kop5s > ALTM[fc6]) ALTM[fc6] = kop5s;
	if (kop5s > KAPM[fc6]) KAPM[fc6] = kop5s;
	if ((ALTM[fc5] + kop5e) > ALTM[fc6])
		ALTM[fc6] = ALTM[fc5] + kop5e;
	if ((KAPM[fc5] + kop5e) > KAPM[fc6])
		KAPM[fc6] = KAPM[fc5] + kop5e;
 }
}


/* PROCEDURE BEPAAL OPENHOUDEN PRIMAIR AANVRAAGGEBIED DOOR APPLICATIE */
/* ================================================================== */

void proc_open_prim_appl(                    /* Fik150101 */
	mulv fc,                             /* Signaalgroep */
	mulv hf_op_pr)                       /* Hulpfunctie open_prim */
{
 mulv i;

 if (IH[hf_op_pr]) FCA[fc] |= BIT2;

 for (i=0; i<aant_fts; i++) {
   proc_open_prim(fts[i][0],NG,fts[i][1],NG,fts[i][2],NG,NG);
 }
 for (i=0; i<aant_bbv; i++) {
   proc_open_prim(bbv[i][0],bbv[i][1],bbv[i][4],NG,bbv[i][5],NG,NG);
 }
 for (i=0; i<aant_gov; i++) {
   proc_open_prim(gov[i][0],gov[i][1],gov[i][7]
		 ,gov[i][8],gov[i][13],gov[i][14],gov[i][22]);
 }
}

/* PROCEDURE CORRIGEER OPENHOUDEN PRIM.AANVRAAGGEBIED FTS.VTG.KOPPELINGEN */
/* ====================================================================== */

void proc_open_prim(                         /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv fc3,                            /* Signaalgroep 3 */
	mulv fc4,                            /* Signaalgroep 4 */
	mulv fc5,                            /* Signaalgroep 5 */
	mulv fc6,                            /* Signaalgroep 6 */
	mulv fc7)                            /* Signaalgroep 7 */
{
 bool _FCA = FALSE;
 mulv i,_fc[7];

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;
 _fc[3]=fc4; _fc[4]=fc5; _fc[5]=fc6; _fc[6]=fc7;

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
	if (FCA[_fc[i]]&BIT2) _FCA = TRUE;
   }
 }
 if (_FCA) {
   for (i=0; i<7; i++) {
     if (_fc[i] >= 0) FCA[_fc[i]] |= BIT2;
   }
 }
}

/* PROCEDURE CORRIGEER OPENHOUDEN PRIM.AANVRAAGGEBIED SERIELE KOPPELING */
/* ==================================================================== */

void proc_open_hki_prim(void)                /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,j,x;
 bool hki_gebied;

 for (x=0; x<3; x++) {							/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1 = hki[i][0];
	fc2 = hki[i][1];
	fc3 = hki[i][10];
	stat= hki[i][20];
	hki_gebied = FALSE;

	if (stat >= 2) {						/* koppeling is aktief */

		if ((VAS2[fc1] != 0) && (VAS4[fc1] != 0) && (VAS2[fc1] != VAS4[fc1]) &&
		    (VAS2[fc2] != 0) && (VAS4[fc2] != 0) && (VAS2[fc2] != VAS4[fc2])) {

			if (VAS4[fc2]>=VAS4[fc1]) {			/* einde primair fc1 VOOR einde primair fc2 */

				if ((STAP>=VAS4[fc1]) && (STAP<VAS4[fc2])) hki_gebied = TRUE;

			} else {					/* einde primair fc1 NA   einde primair fc2 */

				if ((STAP>=VAS4[fc1]) || (STAP<VAS4[fc2])) hki_gebied = TRUE;
			}

			if (G[fc1] || GL[fc1] || TRG[fc1] || VA[fc1]
					      || (HVAS[fc1]&BIT5) || FCA[fc1]) {
				if (!hki_gebied && !(HVAS[fc1]&BIT3)) {
					FCA[fc2] |= BIT2;
					FCA_hki[i][fc2] |= BIT3;
				}
			}

			if (G[fc1] || (HVAS[fc1]&BIT3) || hki_gebied ||	/* toegevoegd Fik110216 */
									!GL[fc1] && !TRG[fc1] && !VA[fc1] && !(HVAS[fc1]&BIT5) && !FCA[fc1]) {
				FCA_hki[i][fc2] &= (~BIT3);
			}

			if (G[fc1] && !G[fc2]) VA[fc2] |= BIT3;		/* fc2 volgt fc1, toegevoegd 110110 */

		} else {
			FCA_hki[i][fc2] &= (~BIT3);
		}

		if (fc3 >= 0) {
			if ((VAS2[fc2] != 0) && (VAS4[fc2] != 0) && (VAS2[fc2] != VAS4[fc2]) &&
			    (VAS2[fc3] != 0) && (VAS4[fc3] != 0) && (VAS2[fc3] != VAS4[fc3])) {

				if (VAS4[fc3]>=VAS4[fc2]) {		/* einde primair fc2 VOOR einde primair fc3 */

					if ((STAP>=VAS4[fc2]) && (STAP<VAS4[fc3])) hki_gebied = TRUE;

				} else {				/* einde primair fc2 NA   einde primair fc3 */

					if ((STAP>=VAS4[fc2]) || (STAP<VAS4[fc3])) hki_gebied = TRUE;
				}

				if (G[fc2] || GL[fc2] || TRG[fc2] || VA[fc2]
						      || (HVAS[fc2]&BIT5) || FCA[fc2]) {
					if (!hki_gebied && !(HVAS[fc2]&BIT3)) {
						FCA[fc3] |= BIT2;
						FCA_hki[i][fc3] |= BIT3;
					}
				}

				if (G[fc2] || (HVAS[fc2]&BIT3) || hki_gebied || !GL[fc2] && !TRG[fc2] && !VA[fc2] && !(HVAS[fc2]&BIT5) && !FCA[fc2]) {
					FCA_hki[i][fc3] &= (~BIT3);
				}
									/* fc3 volgt fc2, toegevoegd Fik110110, gewijzigd Fik110806 */
				/* if (!G[fc3] && (G[fc2] && BW_hki[i][fc2] || G[fc1] && !(FCA_hki[i][fc3]&BIT3))) VA[fc3] |= BIT3; */
				if (!G[fc3] && G[fc2] && BW_hki[i][fc2]) VA[fc3] |= BIT3;

			} else {
				FCA_hki[i][fc3] &= (~BIT3);
			}
		}

	} else {							/* koppeling is niet aktief */
		FCA_hki[i][fc2] &= (~BIT3);
		if (fc3 >= 0) FCA_hki[i][fc3] &= (~BIT3);
	}
    }

    for (j=0; j<FCMAX; j++) {					/* bepaal FCA[]&BIT3 per signaalgroep */
	FCA[j] &= (~BIT3);
    }

    for (i=0; i<aant_hki; i++) {
	for (j=0; j<FCMAX; j++) {
		if (FCA_hki[i][j]&BIT3) FCA[j] |= BIT3;
	}
    }
 }
}

/* PROCEDURE INITIALISATIE TIJD TOT EINDE GROEN, TEG[] */
/* =================================================== */

void init_bteg_crsv(void)                 /* Fik170831 */
{
 mulv i,garant;

 for (i=0; i<aant_bbv; i++) {
	proc_kapm_crsv_bb(bbv[i][0],bbv[i][1],bbv[i][2],bbv[i][3],bbv[i][8]);
 }

 for (i=0; i<aant_gov; i++) {
	proc_kapm_crsv_go(gov[i][0],gov[i][1],gov[i][4],gov[i][5],
			  gov[i][6],gov[i][7],gov[i][8],gov[i][10],gov[i][11],
			  gov[i][13],gov[i][14],gov[i][16],gov[i][17],gov[i][21],
			  gov[i][22],gov[i][25],gov[i][26],gov[i][27],gov[i][28]);
 }

 for (i=0; i<FCMAX; i++) {

	garant = TGG_max[i]/10;

	if (G[i]) {
	   if (KG[i] >= ALTM[i]) TEG[i] = 0;
	   else                  TEG[i] = ALTM[i] - KG[i];
	   if (MG[i] && (KG[i] >= garant)) {
		TEG[i] = 0;
	   }
	   if (SG[i]) {
		TEG[i] = ALTM[i];
	   }
	} else { 
          TEG[i] = 0;
          if (XG[i]) TEG[i] = TTR[i] + ALTM[i];
        }
 }

 for (i=0; i<aant_bbv; i++)
	init_bbtegcrsv(bbv[i][0],bbv[i][1],bbv[i][2]);

 for (i=0; i<aant_gov; i++)
	init_gotegcrsv(gov[i][0] ,gov[i][1] ,gov[i][2] ,gov[i][3] ,gov[i][4] ,gov[i][5],
		       gov[i][7] ,gov[i][8] ,gov[i][10],gov[i][11],
		       gov[i][13],gov[i][14],gov[i][16],gov[i][17],
                       gov[i][22],gov[i][23],gov[i][24],gov[i][25],gov[i][26],gov[i][27],gov[i][28]);

 /* init_hkitegcrsv(); voorbereid op overname hki vanuit SGD */
}

/* PROCEDURE BEPAAL TIJD TOT EINDE GROEN, TEG[] */
/* ============================================ */

void proc_bteg_crsv(void)                    /* Fik150101 */
{
 mulv i,garant;
 mulv _peg;

 proc_open_hki_prim();
 for (i=0; i<FCMAX; i++) {

	garant = TGG_max[i]/10;

	if (!ESGD) {
	  if (G[i]) {
	     if (TS) {
		if (TEG[i]>0) TEG[i]--;
		if (PEG[i]>0) PEG[i]--;
	     }
	     if (MG[i] && (KG[i] >= garant)) {
		TEG[i] = FALSE;
		PEG[i] = FALSE;
	     }
	     if (SG[i]) {
		TEG[i] = ALTM[i];
		PEG[i] = KAPM[i];
	     }
	  } else {
            TEG[i] = 0;
            PEG[i] = 0;
            if (XG[i]&BIT7) {
              TEG[i] = TTR[i] + ALTM[i];
              PEG[i] = TTR[i] + KAPM[i];
            }
          }
	}

	if (!(PVAS[i]&BIT5)) {	/* Corrigeer PEG[] als afbreken niet is toegestaan */
		_peg = 0;
		if (!(HVAS[i]&BIT3)) {
			if (SG[i] && (VA[i]&BIT4) || G[i] && ((HVAS[i]&BIT0) || (HVAS[i]&BIT4))) {
				if (STAP <= VAS4[i]) _peg = VAS4[i] - STAP;
				else                 _peg = CTYD - STAP + VAS4[i];
			}
		}
		if (_peg > PEG[i]) PEG[i] = _peg;
	}

 }
 for (i=0; i<aant_bbv; i++)
	corr_bbtegcrsv(bbv[i][0],bbv[i][1],bbv[i][2]);
 for (i=0; i<aant_gov; i++)
	corr_gotegcrsv(gov[i][0],gov[i][1],gov[i][4],gov[i][5],
			gov[i][7],gov[i][8],gov[i][10],gov[i][11],
			gov[i][13],gov[i][14],gov[i][16],gov[i][17],
                        gov[i][22],gov[i][23],gov[i][24],gov[i][25],gov[i][26],gov[i][27],gov[i][28]);
 corr_hkitegcrsv();
}

/* PROCEDURE INITIALISATIE TEG[] VOOR PROGRESSIEVE VOETGANGERSOVERSTEEK */
/* ==================================================================== */

void init_bbtegcrsv(                         /* Fik170831 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpt12)                          /* KoppelTIJD eg1 -> eg2 */
{
 mulv kop12 = 0;
 mulv akt12 = 0;

 if (kpt12 != NG) {
   kop12 = T_max[kpt12]/10;
   if (T[kpt12]) {
     akt12 = T_timer[kpt12]/10;
   } else {
     akt12 = 0;
   }
 }

 if ((fc1 >= 0) && (fc2 >= 0)) {
   if (XG[fc1] || G[fc1]) {
     if (((TEG[fc1] + kop12) > TEG[fc2]) && (XG[fc2] || G[fc2]))
			       TEG[fc2] = TEG[fc1] + kop12;
   } else {
     if (G[fc2] && (kpt12 != NG) && T[kpt12]) {
       if (kop12 - akt12 > TEG[fc2]) TEG[fc2] = kop12 - akt12;
     }
   }
 }
}

/* PROCEDURE CORRIGEER TEG[] VOOR PROGRESSIEVE VOETGANGERSOVERSTEEK */
/* ================================================================ */

void corr_bbtegcrsv(                         /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpt12)                          /* KoppelTIJD eg1 -> eg2 */
{
 mulv dgg_1 = 0;
 mulv dgg_2 = 0;
 mulv kop12 = 0;

 if ((fc1 >= 0) && (fc2 >= 0)) {
   if (!ESGD) {
     dgg_1 = TGG_max[fc1]/10;
     dgg_2 = TGG_max[fc2]/10;
     if (kpt12 != NG) kop12 = T_max[kpt12]/10;

     if (SG[fc1]) { TEG[fc1] = TFG_max[fc1]/10;
	if (dgg_1 > TEG[fc1]) TEG[fc1] = dgg_1;
     }
     if (SG[fc2]) { TEG[fc2] = TFG_max[fc2]/10;
	if (dgg_2 > TEG[fc2]) TEG[fc2] = dgg_2;
     }
     if ((XG[fc1]&BIT7) || G[fc1]) {
	if (((TEG[fc1] + kop12) > TEG[fc2]) && ((XG[fc2]&BIT7) || G[fc2]))
			          TEG[fc2] = TEG[fc1] + kop12;
     }
   }
   PEG[fc1] = TEG[fc1];
   PEG[fc2] = TEG[fc2];
 }
}

/* PROCEDURE INITIALISATIE TEG[] VOOR GESCHEIDEN VOETGANGERSOVERSTEEK */
/* ================================================================== */

void init_gotegcrsv(                         /* Fik170831 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpa12,                          /* Kop.aanvr. sg1 -> sg2      */
	mulv kpa21,                          /* Kop.aanvr. sg2 -> sg1      */
	mulv kpt12,                          /* KoppelTIJD sg1 -> eg2      */
	mulv kpt21,                          /* koppelTIJD sg2 -> eg1      */
	mulv fc3,                            /* Gekoppelde fietser 1       */
	mulv fc4,                            /* Gekoppelde fietser 1 volg  */
	mulv kpt3s,                          /* Koppeltijd vanaf start fc3 */
	mulv kpt3e,                          /* Koppeltijd vanaf einde fc3 */
	mulv fc5,                            /* Gekoppelde fietser 2       */
	mulv fc6,                            /* Gekoppelde fietser 2 volg  */
	mulv kpt5s,                          /* Koppeltijd vanaf start fc5 */
	mulv kpt5e,                          /* Koppeltijd vanaf einde fc5 */
	mulv vtg3,                           /* Signaalgroep 3e voetganger */
	mulv kpa31,                          /* Kop.aanvr. sg3 -> sg1      */
	mulv kpa32,                          /* Kop.aanvr. sg3 -> sg2      */
	mulv kpt13,                          /* KoppelTIJD sg1 -> eg3      */
	mulv kpt23,                          /* KoppelTIJD sg2 -> eg3      */
	mulv kpt31,                          /* KoppelTIJD sg3 -> eg1      */
	mulv kpt32)                          /* KoppelTIJD sg3 -> eg2      */
{
 mulv kop12 = 0;
 mulv kop21 = 0;
 mulv kop3s = 0;
 mulv kop3e = 0;
 mulv kop5s = 0;
 mulv kop5e = 0;

 mulv akt12 = 0;
 mulv akt21 = 0;
 mulv akt3s = 0;
 mulv akt3e = 0;
 mulv akt5s = 0;
 mulv akt5e = 0;

 mulv kop13 = 0;
 mulv kop23 = 0;
 mulv kop31 = 0;
 mulv kop32 = 0;

 mulv akt13 = 0;
 mulv akt23 = 0;
 mulv akt31 = 0;
 mulv akt32 = 0;

 if (kpt12 != NG) {
   kop12 = T_max[kpt12]/10;
   if (T[kpt12]) {
     akt12 = T_timer[kpt12]/10;
   } else {
     akt12 = 0;
   }
 }
 if (kpt13 != NG) {
   kop13 = T_max[kpt13]/10;
   if (T[kpt13]) {
     akt13 = T_timer[kpt13]/10;
   } else {
     akt13 = 0;
   }
 }

 if (kpt21 != NG) {
   kop21 = T_max[kpt21]/10;
   if (T[kpt21]) {
     akt21 = T_timer[kpt21]/10;
   } else {
     akt21 = 0;
   }
 }
 if (kpt23 != NG) {
   kop23 = T_max[kpt23]/10;
   if (T[kpt23]) {
     akt23 = T_timer[kpt23]/10;
   } else {
     akt23 = 0;
   }
 }

 if (kpt31 != NG) {
   kop31 = T_max[kpt31]/10;
   if (T[kpt31]) {
     akt31 = T_timer[kpt31]/10;
   } else {
     akt31 = 0;
   }
 }
 if (kpt32 != NG) {
   kop32 = T_max[kpt32]/10;
   if (T[kpt32]) {
     akt32 = T_timer[kpt32]/10;
   } else {
     akt32 = 0;
   }
 }

 if (kpt3s != NG) {
   kop3s = T_max[kpt3s]/10;
   if (T[kpt3s]) {
     akt3s = T_timer[kpt3s]/10;
   } else {
     akt3s = 0;
   }
 }
 if (kpt3e != NG) {
   kop3e = T_max[kpt3e]/10;
   if (T[kpt3e]) {
     akt3e = T_timer[kpt3e]/10;
   } else {
     akt3e = 0;
   }
 }

 if (kpt5s != NG) {
   kop5s = T_max[kpt5s]/10;
   if (T[kpt5s]) {
     akt5s = T_timer[kpt5s]/10;
   } else {
     akt5s = 0;
   }
 }
 if (kpt5e != NG) {
   kop5e = T_max[kpt5e]/10;
   if (T[kpt5e]) {
     akt5e = T_timer[kpt5e]/10;
   } else {
     akt5e = 0;
   }
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 < 0)) {

   if ((R[fc1] || SG[fc1]) && (kpa12 != NG) && MM[kpa12]) VA[fc1] |= BIT6;
   if (SG[fc1] && RA[fc2])                                XG[fc2] |= BIT2;

   if ((R[fc2] || SG[fc2]) && (kpa21 != NG) && MM[kpa21]) VA[fc2] |= BIT6;
   if (SG[fc2] && RA[fc1])                                XG[fc1] |= BIT2;

   if (XG[fc1]) TEG[fc1] = TTR[fc1] + ALTM[fc1]; 
   if (XG[fc2]) TEG[fc2] = TTR[fc2] + ALTM[fc2];

   if (XG[fc1] || SG[fc1] && (VA[fc1]&BIT6)) {
     if (XG[fc2] || G[fc2]) {
       if (XG[fc1] && (TTR[fc1] + kop12 > TEG[fc2])) TEG[fc2] = TTR[fc1] + kop12;
       if (SG[fc1] && (           kop12 > TEG[fc2])) TEG[fc2] = kop12;
     }
   }
   if (G[fc1] && (kpt12 != NG) && T[kpt12] && (XG[fc2] || G[fc2])) {
     if ((kop12 - akt12) > TEG[fc2]) TEG[fc2] = kop12 - akt12;
   }

   if (XG[fc2] || SG[fc2] && (VA[fc2]&BIT6)) {
     if (XG[fc1] || G[fc1]) {
       if (XG[fc2] && (TTR[fc2] + kop21 > TEG[fc1])) TEG[fc1] = TTR[fc2] + kop21;
       if (SG[fc2] && (           kop21 > TEG[fc1])) TEG[fc1] = kop21;
     }
   }
   if (G[fc2] && (kpt21 != NG) && T[kpt21] && (XG[fc1] || G[fc1])) {
     if ((kop21 - akt21) > TEG[fc1]) TEG[fc1] = kop21 - akt21;
   }
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0)) {

   if ((R[fc1] || SG[fc1] ) && (kpa12 != NG) && MM[kpa12]) VA[fc1]  |= BIT6;
   if (SG[fc1] && RA[fc2] )                                XG[fc2]  |= BIT2;
   if (SG[fc1] && RA[vtg3])                                XG[vtg3] |= BIT2;

   if ((R[fc2] || SG[fc2] ) && (kpa21 != NG) && MM[kpa21]) VA[fc2]  |= BIT6;
   if (SG[fc2] && RA[fc1] )                                XG[fc1]  |= BIT2;
   if (SG[fc2] && RA[vtg3])                                XG[vtg3] |= BIT2;

   if ((R[vtg3] || SG[vtg3]) && (kpa31 != NG) && MM[kpa31]) VA[fc3] |= BIT6;
   if ((R[vtg3] || SG[vtg3]) && (kpa32 != NG) && MM[kpa32]) VA[fc3] |= BIT6;
   if (SG[vtg3] && RA[fc1] )                                XG[fc1] |= BIT2;
   if (SG[vtg3] && RA[fc2] )                                XG[fc2] |= BIT2;

   if (XG[fc1] ) TEG[fc1]  = TTR[fc1] + ALTM[fc1];
   if (XG[fc2] ) TEG[fc2]  = TTR[fc2] + ALTM[fc2];
   if (XG[vtg3]) TEG[vtg3] = TTR[fc3] + ALTM[vtg3];

   if (XG[fc1] || SG[fc1] && (VA[fc1]&BIT6)) {
     if (XG[fc2] || G[fc2]) {
       if (XG[fc1] && (TTR[fc1] + kop12 > TEG[fc2])) TEG[fc2] = TTR[fc1] + kop12;
       if (SG[fc1] && (           kop12 > TEG[fc2])) TEG[fc2] = kop12;
     }
     if (XG[vtg3] || G[vtg3]) {
       if (XG[fc1] && (TTR[fc1] + kop13 > TEG[vtg3])) TEG[vtg3] = TTR[fc1] + kop13;
       if (SG[fc1] && (           kop13 > TEG[vtg3])) TEG[vtg3] = kop13;
     }
   }
   if (G[fc1] && (kpt12 != NG) && T[kpt12] && (XG[fc2] || G[fc2])) {
     if ((kop12 - akt12) > TEG[fc2]) TEG[fc2] = kop12 - akt12;
   }
   if (G[fc1] && (kpt13 != NG) && T[kpt13] && (XG[fc3] || G[fc3])) {
     if ((kop13 - akt13) > TEG[fc3]) TEG[fc3] = kop13 - akt13;
   }

   if (XG[fc2] || SG[fc2] && (VA[fc2]&BIT6)) {
     if (XG[fc1] || G[fc1]) {
       if (XG[fc2] && (TTR[fc2] + kop21 > TEG[fc1])) TEG[fc1] = TTR[fc2] + kop21;
       if (SG[fc2] && (           kop21 > TEG[fc1])) TEG[fc1] = kop21;
     }
     if (XG[vtg3] || G[vtg3]) {
       if (XG[fc2] && (TTR[fc2] + kop23 > TEG[vtg3])) TEG[vtg3] = TTR[fc2] + kop23;
       if (SG[fc2] && (           kop23 > TEG[vtg3])) TEG[vtg3] = kop23;
     }
   }
   if (G[fc2] && (kpt21 != NG) && T[kpt21] && (XG[fc1] || G[fc1])) {
     if ((kop21 - akt21) > TEG[fc1]) TEG[fc1] = kop21 - akt21;
   }
   if (G[fc2] && (kpt23 != NG) && T[kpt23] && (XG[fc3] || G[fc3])) {
     if ((kop23 - akt23) > TEG[fc3]) TEG[fc3] = kop23 - akt23;
   }

   if (XG[fc3] || SG[vtg3] && (VA[vtg3]&BIT6)) {
     if (XG[fc1] || G[fc1]) {
       if (XG[fc3] && (TTR[fc3] + kop31 > TEG[fc1])) TEG[fc1] = TTR[fc3] + kop31;
       if (SG[fc3] && (           kop31 > TEG[fc1]) && (kpa31 != NG) && MM[kpa31]) TEG[fc1] = kop31;
     }
     if (XG[fc2] || G[fc2]) {
       if (XG[fc3] && (TTR[fc3] + kop32 > TEG[fc2])) TEG[fc2] = TTR[fc2] + kop32;
       if (SG[fc3] && (           kop32 > TEG[fc2]) && (kpa32 != NG) && MM[kpa32]) TEG[fc2] = kop32;
     }
   }
   if (G[fc3] && (kpt31 != NG) && T[kpt31] && (XG[fc1] || G[fc1])) {
     if ((kop31 - akt31) > TEG[fc1]) TEG[fc1] = kop31 - akt31;
   }
   if (G[fc3] && (kpt32 != NG) && T[kpt32] && (XG[fc2] || G[fc2])) {
     if ((kop32 - akt32) > TEG[fc2]) TEG[fc2] = kop32 - akt32;
   }
 }

 if ((fc3 >= 0) && (fc4 >= 0)) {

   if (XG[fc3]) TEG[fc3] = TTR[fc3] + ALTM[fc3];
   if (XG[fc4]) TEG[fc4] = TTR[fc4] + ALTM[fc4];

   if (XG[fc3] && (TTR[fc3] + kop3s > TEG[fc4]) && (XG[fc4] || G[fc4])) TEG[fc4] = TTR[fc3] + kop3s;
   if (XG[fc3] && (TEG[fc3] + kop3e > TEG[fc4]) && (XG[fc4] || G[fc4])) TEG[fc4] = TEG[fc3] + kop3e;

   if (G[fc4] && (kpt3s != NG) && T[kpt3s]) {
     if (kop3s - akt3s > TEG[fc4]) TEG[fc4] = kop3s - akt3s;
   }
   if (G[fc3]) {
     if (((TEG[fc3] + kop3e) > TEG[fc4]) && G[fc4]) {
	   TEG[fc4] = TEG[fc3] + kop3e;
	 }
   } else {
     if (G[fc4] && (kpt3e != NG) && T[kpt3e]) {
       if (kop3e - akt3e > TEG[fc4]) TEG[fc4] = kop3e - akt3e;
     }
   }
 }

 if ((fc5 >= 0) && (fc6 >= 0)) {

   if (XG[fc5]) TEG[fc5] = TTR[fc5] + ALTM[fc5];
   if (XG[fc6]) TEG[fc6] = TTR[fc6] + ALTM[fc6];

   if (XG[fc5] && (TTR[fc5] + kop5s > TEG[fc6]) && (XG[fc6] || G[fc6])) TEG[fc6] = TTR[fc5] + kop5s;
   if (XG[fc5] && (TEG[fc5] + kop5e > TEG[fc6]) && (XG[fc6] || G[fc6])) TEG[fc6] = TEG[fc5] + kop5e;

   if (G[fc6] && (kpt5s != NG) && T[kpt5s]) {
     if (kop5s - akt5s > TEG[fc6]) TEG[fc6] = kop5s - akt5s;
   }
   if (G[fc5]) {
     if (((TEG[fc5] + kop5e) > TEG[fc6]) && G[fc6]) {
       TEG[fc6] = TEG[fc5] + kop5e;
	 }
   } else {
     if (G[fc6] && (kpt5e != NG) && T[kpt5e]) {
       if (kop5e - akt5e > TEG[fc6]) TEG[fc6] = kop5e - akt5e;
     }
   }
 }
}

/* PROCEDURE CORRIGEER TEG[] VOOR GESCHEIDEN VOETGANGERSOVERSTEEK */
/* ============================================================== */

void corr_gotegcrsv(                         /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpt12,                          /* KoppelTIJD sg1 -> eg2 */
	mulv kpt21,                          /* koppelTIJD sg2 -> eg1 */
	mulv fc3,                            /* Gekoppelde fietser 1       */
	mulv fc4,                            /* Gekoppelde fietser 1 volg  */
	mulv kpt3s,                          /* Koppeltijd vanaf start fc3 */
	mulv kpt3e,                          /* Koppeltijd vanaf einde fc3 */
	mulv fc5,                            /* Gekoppelde fietser 2       */
	mulv fc6,                            /* Gekoppelde fietser 2 volg  */
	mulv kpt5s,                          /* Koppeltijd vanaf start fc5 */
	mulv kpt5e,                          /* Koppeltijd vanaf einde fc5 */
	mulv vtg3,                           /* Signaalgroep 3e voetganger */
	mulv kpa31,                          /* Kop.aanvr. sg3 -> sg1      */
	mulv kpa32,                          /* Kop.aanvr. sg3 -> sg2      */
	mulv kpt13,                          /* KoppelTIJD sg1 -> eg3      */
	mulv kpt23,                          /* KoppelTIJD sg2 -> eg3      */
	mulv kpt31,                          /* KoppelTIJD sg3 -> eg1      */
	mulv kpt32)                          /* KoppelTIJD sg3 -> eg2      */
{
 mulv dvg_1 = 0;
 mulv dvg_2 = 0;
 mulv dgg_1 = 0;
 mulv dgg_2 = 0;
 mulv kop12 = 0;
 mulv kop21 = 0;
 mulv kop3s = 0;
 mulv kop3e = 0;
 mulv kop5s = 0;
 mulv kop5e = 0;

 mulv akt12 = 0;
 mulv akt21 = 0;

 mulv min_1 = 0;
 mulv min_2 = 0;

 mulv dvg_3 = 0;   /* toevoegingen i.v.m. drie-traps-voetganger */
 mulv dgg_3 = 0;

 mulv kop13 = 0;
 mulv kop23 = 0;
 mulv kop31 = 0;
 mulv kop32 = 0;

 mulv akt13 = 0;
 mulv akt23 = 0;
 mulv akt31 = 0;
 mulv akt32 = 0;

 mulv min_3 = 0;

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 < 0) && !ESGD) {
   dvg_1 = TFG_max[fc1]/10;
   dvg_2 = TFG_max[fc2]/10;
   dgg_1 = TGG_max[fc1]/10;
   dgg_2 = TGG_max[fc2]/10;
   if (kpt12 != NG) kop12 = T_max[kpt12]/10;
   if (kpt21 != NG) kop21 = T_max[kpt21]/10;

   min_1 = dvg_1; min_2 = dvg_2;
   if (dgg_1 > min_1) min_1 = dgg_1;
   if (dgg_2 > min_2) min_2 = dgg_2;

   if (kpt12 != NG) {
     if (T[kpt12]) {
  	akt12 = T_timer[kpt12]/10;
     } else {
	akt12 = 0;
     }
   }

   if (kpt21 != NG) {
     if (T[kpt21]) {
	akt21 = T_timer[kpt21]/10;
     } else {
	akt21 = 0;
     }
   }

   if ((fc3 < 0) && (fc4 < 0) && (fc5 < 0) && (fc6 < 0)) {
	if (SG[fc1] && (VA[fc1]&BIT6) && RA[fc2]) XG[fc2] |= BIT2;
	if (SG[fc2] && (VA[fc2]&BIT6) && RA[fc1]) XG[fc1] |= BIT2;
   }

   if (SG[fc1] || XG[fc1]) TEG[fc1] = min_1;
   if (SG[fc2] || XG[fc2]) TEG[fc2] = min_2;
   if (XG[fc1]&BIT7) TEG[fc1] = TTR[fc1] + ALTM[fc1];
   if (XG[fc2]&BIT7) TEG[fc2] = TTR[fc2] + ALTM[fc2];

   if (SG[fc1] && (VA[fc1]&BIT6)) {
     if ((kop12 > TEG[fc2]) && G[fc2]) TEG[fc2] = kop12;
   }
   if (SG[fc2] && (VA[fc2]&BIT6)) {
     if ((kop21 > TEG[fc1]) && G[fc1]) TEG[fc1] = kop21;
   }
   if (XG[fc1]) {
	if (!(XG[fc1]&BIT7))  TEG[fc1] += TTR[fc1];
	if ((kop21 - akt21) > TEG[fc1]) TEG[fc1] = kop21 - akt21;
	if (G[fc1]) XG[fc1] = FALSE;
	if (G[fc2] && ((TTR[fc1]+kop12) > TEG[fc2])) TEG[fc2] = TTR[fc1]+kop12;
	if (G[fc2] && SG[fc1] && !(VA[fc1]&BIT6)) {
	  if (SG[fc2] || SFG[fc2]) TEG[fc2] = min_2;
	  else {
	    if (dvg_2 >= dgg_2) {
	      if (TFG[fc2]) TEG[fc2] = dvg_2-(TFG_timer[fc2]/10);
	      else          TEG[fc2] = 0;
	    } else {
	      if (TGG[fc2]) TEG[fc2] = dgg_2-(TGG_timer[fc2]/10);
	      else          TEG[fc2] = 0;
	    }
	    if (TEG[fc2]<0) TEG[fc2] = 0;
	  }
	}
   }
   if (XG[fc2]) {
	if (!(XG[fc2]&BIT7))  TEG[fc2] += TTR[fc2];
	if ((kop12 - akt12) > TEG[fc2]) TEG[fc2] = kop12 - akt12;
	if (G[fc2]) XG[fc2] = FALSE;
	if (G[fc1] && ((TTR[fc2]+kop21) > TEG[fc1])) TEG[fc1] = TTR[fc2]+kop21;
	if (G[fc1] && SG[fc2] && !(VA[fc2]&BIT6)) {
	  if (SG[fc1] || SFG[fc1]) TEG[fc1] = min_1;
	  else {
	    if (dvg_1 >= dgg_1) {
	      if (TFG[fc1]) TEG[fc1] = dvg_1-(TFG_timer[fc1]/10);
	      else          TEG[fc1] = 0;
	    } else {
	      if (TGG[fc1]) TEG[fc1] = dgg_1-(TGG_timer[fc1]/10);
	      else          TEG[fc1] = 0;
	    }
	    if (TEG[fc1]<0) TEG[fc1] = 0;
	  }
	}
   }
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0) && !ESGD) {
   dvg_1 = TFG_max[fc1]/10;
   dvg_2 = TFG_max[fc2]/10;
   dgg_1 = TGG_max[fc1]/10;
   dgg_2 = TGG_max[fc2]/10;
   if (kpt12 != NG) kop12 = T_max[kpt12]/10;
   if (kpt21 != NG) kop21 = T_max[kpt21]/10;

   dvg_3 = TFG_max[vtg3]/10;
   dgg_3 = TGG_max[vtg3]/10;
   if (kpt13 != NG) kop13 = T_max[kpt13]/10;
   if (kpt23 != NG) kop23 = T_max[kpt23]/10;
   if (kpt31 != NG) kop31 = T_max[kpt31]/10;
   if (kpt32 != NG) kop32 = T_max[kpt32]/10;

   min_1 = dvg_1; min_2 = dvg_2; min_3 = dvg_3;
   if (dgg_1 > min_1) min_1 = dgg_1;
   if (dgg_2 > min_2) min_2 = dgg_2;
   if (dgg_3 > min_3) min_3 = dgg_3;

   if (T[kpt12]) {
	akt12 = T_timer[kpt12]/10;
   } else {
	akt12 = 0;
   }

   if (T[kpt13]) {
	akt13 = T_timer[kpt13]/10;
   } else {
	akt13 = 0;
   }

   if (T[kpt21]) {
	akt21 = T_timer[kpt21]/10;
   } else {
	akt21 = 0;
   }

   if (T[kpt23]) {
	akt23 = T_timer[kpt23]/10;
   } else {
	akt23 = 0;
   }

   if (T[kpt31]) {
	akt31 = T_timer[kpt31]/10;
   } else {
	akt31 = 0;
   }

   if (T[kpt32]) {
	akt32 = T_timer[kpt32]/10;
   } else {
	akt32 = 0;
   }

   if ((fc3 < 0) && (fc4 < 0) && (fc5 < 0) && (fc6 < 0)) {
	if (SG[fc1] && (VA[fc1]&BIT6) && RA[fc2] ) XG[fc2]  |= BIT2;
	if (SG[fc1] && (VA[fc1]&BIT6) && RA[vtg3]) XG[vtg3] |= BIT2;
	if (SG[fc2] && (VA[fc2]&BIT6) && RA[fc1] ) XG[fc1]  |= BIT2;
	if (SG[fc2] && (VA[fc2]&BIT6) && RA[vtg3]) XG[vtg3] |= BIT2;

	if (kpa31 >= 0) { if (SG[vtg3] && MM[kpa31] && RA[fc1]) XG[fc1] |= BIT2; }
	if (kpa32 >= 0) { if (SG[vtg3] && MM[kpa32] && RA[fc2]) XG[fc2] |= BIT2; }
   }
   if (SG[fc1]  || XG[fc1] ) TEG[fc1]  = min_1;
   if (SG[fc2]  || XG[fc2] ) TEG[fc2]  = min_2;
   if (SG[vtg3] || XG[vtg3]) TEG[vtg3] = min_3;

   if (XG[fc1]&BIT7) TEG[fc1] = TTR[fc1] + ALTM[fc1];
   if (XG[fc2]&BIT7) TEG[fc2] = TTR[fc2] + ALTM[fc2];
   if (XG[fc3]&BIT7) TEG[fc3] = TTR[fc3] + ALTM[fc3];

   if (SG[fc1] && (VA[fc1]&BIT6)) {
     if ((kop12 > TEG[fc2] ) && G[fc2] ) TEG[fc2]  = kop12;
     if ((kop13 > TEG[vtg3]) && G[vtg3]) TEG[vtg3] = kop13;
   }
   if (SG[fc2] && (VA[fc2]&BIT6)) {
     if ((kop21 > TEG[fc1] ) && G[fc1] ) TEG[fc1]  = kop21;
     if ((kop23 > TEG[vtg3]) && G[vtg3]) TEG[vtg3] = kop23;
   }
   if (kpa31 >= 0) {
     if (SG[vtg3] && MM[kpa31]) {
       if ((kop31 > TEG[fc1]) && G[fc1]) TEG[fc1] = kop31;
     }
   }
   if (kpa32 >= 0) {
     if (SG[vtg3] && MM[kpa32]) {
       if ((kop32 > TEG[fc2]) && G[fc2]) TEG[fc2] = kop32;
     }
   }

   if (XG[fc1]) {
	if (!(XG[fc1]&BIT7))  TEG[fc1] += TTR[fc1];
	if ((kop21 - akt21) > TEG[fc1]) TEG[fc1] = kop21 - akt21;
	if ((kop31 - akt31) > TEG[fc1]) TEG[fc1] = kop31 - akt31;
	if (G[fc1]) XG[fc1] = FALSE;
	if (G[fc2]  && ((TTR[fc1]+kop12) > TEG[fc2] )) TEG[fc2]  = TTR[fc1]+kop12;
	if (G[vtg3] && ((TTR[fc1]+kop13) > TEG[vtg3])) TEG[vtg3] = TTR[fc1]+kop13;
	if (G[fc2] && SG[fc1] && !(VA[fc1]&BIT6)) {
	  if (SG[fc2] || SFG[fc2]) TEG[fc2] = min_2;
	  else {
	    if (dvg_2 >= dgg_2) {
	      if (TFG[fc2]) TEG[fc2] = dvg_2-(TFG_timer[fc2]/10);
	      else          TEG[fc2] = 0;
	    } else {
	      if (TGG[fc2]) TEG[fc2] = dgg_2-(TGG_timer[fc2]/10);
	      else          TEG[fc2] = 0;
	    }
	    if (TEG[fc2]<0) TEG[fc2] = 0;
	  }
	}
	if (G[vtg3] && SG[fc1] && !(VA[fc1]&BIT6)) {
	  if (SG[vtg3] || SFG[vtg3]) TEG[vtg3] = min_3;
	  else {
	    if (dvg_3 >= dgg_3) {
	      if (TFG[vtg3]) TEG[vtg3] = dvg_3-(TFG_timer[vtg3]/10);
	      else           TEG[vtg3] = 0;
	    } else {
	      if (TGG[vtg3]) TEG[vtg3] = dgg_3-(TGG_timer[vtg3]/10);
	      else           TEG[vtg3] = 0;
	    }
	    if (TEG[vtg3]<0) TEG[vtg3] = 0;
	  }
	}
   }
   if (XG[fc2]) {
	if (!(XG[fc2]&BIT7))  TEG[fc2] += TTR[fc2];
	if ((kop12 - akt12) > TEG[fc2]) TEG[fc2] = kop12 - akt12;
	if ((kop32 - akt32) > TEG[fc2]) TEG[fc2] = kop32 - akt32;
	if (G[fc2]) XG[fc2] = FALSE;
	if (G[fc1]  && ((TTR[fc2]+kop21) > TEG[fc1] )) TEG[fc1]  = TTR[fc2]+kop21;
	if (G[vtg3] && ((TTR[fc2]+kop23) > TEG[vtg3])) TEG[vtg3] = TTR[fc2]+kop23;
	if (G[fc1] && SG[fc2] && !(VA[fc2]&BIT6)) {
	  if (SG[fc1] || SFG[fc1]) TEG[fc1] = min_1;
	  else {
	    if (dvg_1 >= dgg_1) {
	      if (TFG[fc1]) TEG[fc1] = dvg_1-(TFG_timer[fc1]/10);
	      else          TEG[fc1] = 0;
	    } else {
	      if (TGG[fc1]) TEG[fc1] = dgg_1-(TGG_timer[fc1]/10);
	      else          TEG[fc1] = 0;
	    }
	    if (TEG[fc1]<0) TEG[fc1] = 0;
	  }
	}
	if (G[vtg3] && SG[fc2] && !(VA[fc2]&BIT6)) {
	  if (SG[vtg3] || SFG[vtg3]) TEG[vtg3] = min_3;
	  else {
	    if (dvg_3 >= dgg_3) {
	      if (TFG[vtg3]) TEG[vtg3] = dvg_3-(TFG_timer[vtg3]/10);
	      else           TEG[vtg3] = 0;
	    } else {
	      if (TGG[vtg3]) TEG[vtg3] = dgg_3-(TGG_timer[vtg3]/10);
	      else           TEG[vtg3] = 0;
	    }
	    if (TEG[vtg3]<0) TEG[vtg3] = 0;
	  }
	}
   }
   if (XG[vtg3]) {
	if (!(XG[fc3]&BIT7))  TEG[vtg3] += TTR[vtg3];
	if ((kop13 - akt13) > TEG[vtg3]) TEG[vtg3] = kop13 - akt13;
	if ((kop23 - akt23) > TEG[vtg3]) TEG[vtg3] = kop23 - akt23;
	if (G[vtg3]) XG[vtg3] = FALSE;
	if (G[fc1] && ((TTR[vtg3]+kop31) > TEG[fc1])) TEG[fc1] = TTR[vtg3]+kop31;
	if (G[fc2] && ((TTR[vtg3]+kop32) > TEG[fc2])) TEG[fc2] = TTR[vtg3]+kop32;
	if (G[fc1] && SG[vtg3] && !MM[kpa31]) {
	  if (SG[fc1] || SFG[fc1]) TEG[fc1] = min_1;
	  else {
	    if (dvg_1 >= dgg_1) {
	      if (TFG[fc1]) TEG[fc1] = dvg_1-(TFG_timer[fc1]/10);
	      else          TEG[fc1] = 0;
	    } else {
	      if (TGG[fc1]) TEG[fc1] = dgg_1-(TGG_timer[fc1]/10);
	      else          TEG[fc1] = 0;
	    }
	    if (TEG[fc1]<0) TEG[fc1] = 0;
	  }
	}
	if (G[fc2] && SG[vtg3] && !MM[kpa32]) {
	  if (SG[fc2] || SFG[fc2]) TEG[fc2] = min_2;
	  else {
	    if (dvg_2 >= dgg_2) {
	      if (TFG[fc2]) TEG[fc2] = dvg_2-(TFG_timer[fc2]/10);
	      else          TEG[fc2] = 0;
	    } else {
	      if (TGG[fc2]) TEG[fc2] = dgg_2-(TGG_timer[fc2]/10);
	      else          TEG[fc2] = 0;
	    }
	    if (TEG[fc2]<0) TEG[fc2] = 0;
	  }
	}
   }
   /* 1x extra vanwege 3e voetganger */
   if (SG[fc1] && (VA[fc1]&BIT6)) {
     if ((kop12 > TEG[fc2] ) && G[fc2] ) TEG[fc2]  = kop12;
     if ((kop13 > TEG[vtg3]) && G[vtg3]) TEG[vtg3] = kop13;
   }
   if (SG[fc2] && (VA[fc2]&BIT6)) {
     if ((kop21 > TEG[fc1] ) && G[fc1] ) TEG[fc1]  = kop21;
     if ((kop23 > TEG[vtg3]) && G[vtg3]) TEG[vtg3] = kop23;
   }
   if (kpa31 >= 0) {
     if (SG[vtg3] && MM[kpa31]) {
       if ((kop31 > TEG[fc1]) && G[fc1]) TEG[fc1] = kop31;
     }
   }
   if (kpa32 >= 0) {
     if (SG[vtg3] && MM[kpa32]) {
       if ((kop32 > TEG[fc2]) && G[fc2]) TEG[fc2] = kop32;
     }
   }
 }

 if (fc1  >= 0) PEG[fc1]  = TEG[fc1];
 if (fc2  >= 0) PEG[fc2]  = TEG[fc2];
 if (vtg3 >= 0) PEG[vtg3] = TEG[vtg3];

 if ((fc3 >= 0) && (fc4 >= 0) && !ESGD) {
   kop3s = T_max[kpt3s]/10;
   kop3e = T_max[kpt3e]/10;
   if (SG[fc3]) {
     if (((TEG[fc3] + kop3s) > TEG[fc4]) && G[fc4])
           TEG[fc4] = TEG[fc3] + kop3s;
     if (((PEG[fc3] + kop3s) > PEG[fc4]) && G[fc4])
           PEG[fc4] = PEG[fc3] + kop3s;
   }
   if (G[fc3]) {
     if (((TEG[fc3] + kop3e) > TEG[fc4]) && G[fc4])
				TEG[fc4] = TEG[fc3] + kop3e;
     if (((PEG[fc3] + kop3e) > PEG[fc4]) && G[fc4])
				PEG[fc4] = PEG[fc3] + kop3e;
   }
 }
 if ((fc5 >= 0) && (fc6 >= 0) && !ESGD) {
   kop5s = T_max[kpt5s]/10;
   kop5e = T_max[kpt5e]/10;
   if (SG[fc5]) {
     if (((TEG[fc5] + kop5s) > TEG[fc6]) && G[fc6])
           TEG[fc6] = TEG[fc5] + kop5s;
     if (((PEG[fc5] + kop5s) > PEG[fc6]) && G[fc6])
           PEG[fc6] = PEG[fc5] + kop5s;
   }
   if (G[fc5]) {
     if (((TEG[fc5] + kop5e) > TEG[fc6]) && G[fc6])
				TEG[fc6] = TEG[fc5] + kop5e;
     if (((PEG[fc5] + kop5e) > PEG[fc6]) && G[fc6])
				PEG[fc6] = PEG[fc5] + kop5e;
   }
 }
}

/* PROCEDURE CORRIGEER TEG[] VOOR SERIELE KOPPELING */
/* ================================================ */

void corr_hkitegcrsv(void)                   /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,j,x;
 mulv kopsg12,kopsg23;
 mulv kopeg12,kopeg23;
 mulv kopdm12,kopdm23;
 mulv garant2,garant3;
 mulv akt;
 mulv bufteg2,bufteg3;
 mulv bufpeg2,bufpeg3;
 mulv _peg;

 for (i=0; i<aant_hki; i++) {
	fc2 = hki[i][1];
	fc3 = hki[i][10];
									/* bepaal juiste startwaarde TEG[] en PEG[] */
	if (KG[fc2] > ALTM[fc2]) TEG[fc2] = 0;
	if (KG[fc2] > KAPM[fc2]) PEG[fc2] = 0;

	if (!(PVAS[fc2]&BIT5)) {					/* Corrigeer PEG[] als afbreken niet is toegestaan */
		_peg = 0;
		if (!(HVAS[fc2]&BIT3)) {
			if (SG[fc2] && (VA[fc2]&BIT4) || G[fc2] && ((HVAS[fc2]&BIT0) || (HVAS[fc2]&BIT4))) {
						if (STAP <= VAS4[fc2]) _peg = VAS4[fc2] - STAP;
						else                   _peg = CTYD - STAP + VAS4[fc2];
			}
		}
		if (_peg > PEG[fc2]) PEG[fc2] = _peg;
	}

	if (fc3 >= 0) {
		if (KG[fc3] > ALTM[fc3]) TEG[fc3] = 0;
		if (KG[fc3] > KAPM[fc3]) PEG[fc3] = 0;

		if (!(PVAS[fc3]&BIT5)) {				/* Corrigeer PEG[] als afbreken niet is toegestaan */
			_peg = 0;
			if (!(HVAS[fc3]&BIT3)) {
				if (SG[fc3] && (VA[fc3]&BIT4) || G[fc3] && ((HVAS[fc3]&BIT0) || (HVAS[fc3]&BIT4))) {
							if (STAP <= VAS4[fc3]) _peg = VAS4[fc3] - STAP;
							else                   _peg = CTYD - STAP + VAS4[fc3];
				}
			}
			if (_peg > PEG[fc3]) PEG[fc3] = _peg;
		}
	}
 }

 for (x=0; x<3; x++) {							/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kopsg12 = T_max[hki[i][3]]/10;
	kopeg12 = T_max[hki[i][4]]/10;
	kopdm12 = T_max[hki[i][5]]/10;
	garant2 = TGG_max[fc2]/10;
	fc3     = hki[i][10];
	stat    = hki[i][20];

	if (fc3 >= 0) {
		kopsg23 = T_max[hki[i][12]]/10;
		kopeg23 = T_max[hki[i][13]]/10;
		kopdm23 = T_max[hki[i][14]]/10;
		garant3 = TGG_max[fc3]/10;
	} else {
		kopsg23 = 0;
		kopeg23 = 0;
		kopdm23 = 0;
		garant3 = 0;
	}

	if (stat >= 2) {						/* koppeling is aktief */

		bufteg2 = bufpeg2 = 0;

		if (G[fc1] || G[fc2] || XG_hki[i][fc2]) {
			bufteg2 = TEG[fc2];
			bufpeg2 = PEG[fc2];

			XG_hki[i][fc2] &= (~BIT1);
			XG_hki[i][fc2] &= (~BIT2);
			if (G[fc2]) XG_hki[i][fc2] &= (~BIT3);

			if (!G[fc2]) XG_hki[i][fc2] |= BIT1;
			if (!G[fc2] && (TTP[fc1] != 0)) XG_hki[i][fc2] |= BIT2;
			TEG[fc2] = PEG[fc2] = FALSE;

			if (!G[fc2]) {
				TEG[fc2] = TTR[fc2] + ALTM[fc2];
				PEG[fc2] = TTR[fc2] + KAPM[fc2];
			}
			if (SG[fc2]) { TEG[fc2] = ALTM[fc2]; PEG[fc2] = KAPM[fc2]; }
			else {
				if (G[fc2] && (ALTM[fc2] > KG[fc2])) TEG[fc2] = ALTM[fc2] - KG[fc2];
				if (G[fc2] && (KAPM[fc2] > KG[fc2])) PEG[fc2] = KAPM[fc2] - KG[fc2];
			}
			if (MG[fc2] && (KG[fc2] >= garant2)) { TEG[fc2] = PEG[fc2] = FALSE; }
			else {
				if (MG[fc2] && TGG[fc2]) {
					TEG[fc2] = garant2 - (TGG_timer[fc2]/10);
					PEG[fc2] = garant2 - (TGG_timer[fc2]/10);
					if (TEG[fc2] < 0) TEG[fc2] = 0;
					if (PEG[fc2] < 0) PEG[fc2] = 0;
				}
			}
			if (G[fc1]) {
				if (SG[fc1] && ((kopsg12 + kopdm12) > TEG[fc2]))
					TEG[fc2] = kopsg12 + kopdm12;
				if (TEG[fc1] + kopeg12 + kopdm12 > TEG[fc2])
					TEG[fc2] = TEG[fc1] + kopeg12 + kopdm12;
				if (SG[fc1] && ((kopsg12 + kopdm12) > PEG[fc2]))
					PEG[fc2] = kopsg12 + kopdm12;
				if (PEG[fc1] + kopeg12 + kopdm12 > PEG[fc2])
					PEG[fc2] = PEG[fc1] + kopeg12 + kopdm12;
			}
			if (RT[hki[i][3]] || T[hki[i][3]]) {	/* koppeltijd vanaf start groen */
				if (RT[hki[i][3]]) akt = 0;
				else               akt = T_timer[hki[i][3]]/10;
				if ((kopsg12 - akt + kopdm12) > TEG[fc2]) TEG[fc2] = kopsg12 - akt + kopdm12;
				if ((kopsg12 - akt + kopdm12) > PEG[fc2]) PEG[fc2] = kopsg12 - akt + kopdm12;
			}
			if (RT[hki[i][4]] || T[hki[i][4]]) {	/* koppeltijd vanaf einde groen */
				if (RT[hki[i][4]]) akt = 0;
				else               akt = T_timer[hki[i][4]]/10;
				if ((kopeg12 - akt + kopdm12) > TEG[fc2]) TEG[fc2] = kopeg12 - akt + kopdm12;
				if ((kopeg12 - akt + kopdm12) > PEG[fc2]) PEG[fc2] = kopeg12 - akt + kopdm12;
			}
			if (RT[hki[i][5]] || T[hki[i][5]]) {	/* duur max. na koppeling */
				if (RT[hki[i][5]]) akt = 0;
				else               akt = T_timer[hki[i][5]]/10;
				if ((kopdm12 - akt) > TEG[fc2])           TEG[fc2] = kopdm12 - akt;
				if ((kopdm12 - akt) > PEG[fc2])           PEG[fc2] = kopdm12 - akt;
			}
		}

		if (fc3 >= 0) {

			if (G[fc1] || G[fc2] || G[fc3] || XG_hki[i][fc3]) {
				bufteg3 = TEG[fc3];
				bufpeg3 = PEG[fc3];

				XG_hki[i][fc3] &= (~BIT1);
				XG_hki[i][fc3] &= (~BIT2);
				if (G[fc3]) XG_hki[i][fc3] &= (~BIT3);
				if (G[fc2] && BW_hki[i][fc2] || G[fc3]) XG_hki[i][fc3] &= (~BIT5);

				if (!G[fc3] && (!G[fc2] || BW_hki[i][fc2])) XG_hki[i][fc3] |= BIT1;
				if ((XG_hki[i][fc2]&BIT2) || G[fc2] && BW_hki[i][fc2] && !G[fc3] && (TTP[fc2] != 0)) XG_hki[i][fc3] |= BIT2;
				TEG[fc3] = PEG[fc3] = FALSE;

				if (!G[fc3] && (!G[fc2] || BW_hki[i][fc2])) {
					TEG[fc3] = TTR[fc3] + ALTM[fc3];
					PEG[fc3] = TTR[fc3] + KAPM[fc3];
				}
				if (SG[fc3]) { TEG[fc3] = ALTM[fc3]; PEG[fc3] = KAPM[fc3]; }
				else {
					if (G[fc3] && (ALTM[fc3] > KG[fc3])) TEG[fc3] = ALTM[fc3] - KG[fc3];
					if (G[fc3] && (KAPM[fc3] > KG[fc3])) PEG[fc3] = KAPM[fc3] - KG[fc3];
				}
				if (MG[fc3] && (KG[fc3] >= garant3)) { TEG[fc3] = PEG[fc3] = FALSE; }
				else {
					if (MG[fc3] && TGG[fc3]) {
						TEG[fc3] = garant3 - (TGG_timer[fc3]/10);
						PEG[fc3] = garant3 - (TGG_timer[fc3]/10);
						if (TEG[fc3] < 0) TEG[fc3] = 0;
						if (PEG[fc3] < 0) PEG[fc3] = 0;
					}
				}
				if (!G[fc2] && XG_hki[i][fc2]) {
					if ((TTR[fc2] + kopsg23 + kopdm23) > TEG[fc3])
						TEG[fc3] = TTR[fc2] + kopsg23 + kopdm23;
					if ((TEG[fc2] + kopeg23 + kopdm23) > TEG[fc3])
						TEG[fc3] = TEG[fc2] + kopeg23 + kopdm23;
					if ((TTR[fc2] + kopsg23 + kopdm23) > PEG[fc3])
						PEG[fc3] = TTR[fc2] + kopsg23 + kopdm23;
					if ((PEG[fc2] + kopeg23 + kopdm23) > PEG[fc3])
						PEG[fc3] = PEG[fc2] + kopeg23 + kopdm23;
				}
				if (G[fc2] && BW_hki[i][fc2]) {
					if (SG[fc2] && ((kopsg23 + kopdm23) > TEG[fc3]))
						TEG[fc3] = kopsg23 + kopdm23;
					if (TEG[fc2] + kopeg23 + kopdm23 > TEG[fc3])
						TEG[fc3] = TEG[fc2] + kopeg23 + kopdm23;
					if (SG[fc2] && ((kopsg23 + kopdm23) > PEG[fc3]))
						PEG[fc3] = kopsg23 + kopdm23;
					if (PEG[fc2] + kopeg23 + kopdm23 > PEG[fc3])
						PEG[fc3] = PEG[fc2] + kopeg23 + kopdm23;
				}
				if (RT[hki[i][12]] || T[hki[i][12]]) {	/* koppeltijd vanaf start groen */
					if (RT[hki[i][12]]) akt = 0;
					else                akt = T_timer[hki[i][12]]/10;
					if ((kopsg23 - akt + kopdm23) > TEG[fc3]) TEG[fc3] = kopsg23 - akt + kopdm23;
					if ((kopsg23 - akt + kopdm23) > PEG[fc3]) PEG[fc3] = kopsg23 - akt + kopdm23;
				}
				if (RT[hki[i][13]] || T[hki[i][13]]) {	/* koppeltijd vanaf einde groen */
					if (RT[hki[i][13]]) akt = 0;
					else                akt = T_timer[hki[i][13]]/10;
					if ((kopeg23 - akt + kopdm23) > TEG[fc3]) TEG[fc3] = kopeg23 - akt + kopdm23;
					if ((kopeg23 - akt + kopdm23) > PEG[fc3]) PEG[fc3] = kopeg23 - akt + kopdm23;
				}
				if (RT[hki[i][14]] || T[hki[i][14]]) {	/* duur max. na koppeling */
					if (RT[hki[i][14]]) akt = 0;
					else                akt = T_timer[hki[i][14]]/10;
					if ((kopdm23 - akt) > TEG[fc3])           TEG[fc3] = kopdm23 - akt;
					if ((kopdm23 - akt) > PEG[fc3])           PEG[fc3] = kopdm23 - akt;
				}
				if (bufteg3 > TEG[fc3]) TEG[fc3] = bufteg3;
				if (bufpeg3 > PEG[fc3]) PEG[fc3] = bufteg3;
			}
		}

		if (bufteg2 > TEG[fc2]) TEG[fc2] = bufteg2;
		if (bufpeg2 > PEG[fc2]) PEG[fc2] = bufteg2;

	} else {							/* koppeling is niet aktief */
		XG_hki[i][fc2] &= (~BIT1);
		XG_hki[i][fc2] &= (~BIT2);
		XG_hki[i][fc2] &= (~BIT3);
		if (fc3 >= 0) {
			XG_hki[i][fc3] &= (~BIT1);
			XG_hki[i][fc3] &= (~BIT2);
			XG_hki[i][fc3] &= (~BIT3);
			XG_hki[i][fc3] &= (~BIT5);
		}
	}
    }

    for (i=0; i<aant_hki; i++) {					/* bepaal XG[] per signaalgroep voor hki koppelingen */
	for (j=0; j<FCMAX; j++) {
		if ((j == hki[i][0]) || (j == hki[i][1]) || (j == hki[i][10])) {
			XG[j] &= (~BIT1);
			XG[j] &= (~BIT2);
			XG[j] &= (~BIT3);
			XG[j] &= (~BIT5);
		}
        }
    }

    for (i=0; i<aant_hki; i++) {
	for (j=0; j<FCMAX; j++) {
		if (XG_hki[i][j]&BIT1) XG[j] |= BIT1;
		if (XG_hki[i][j]&BIT2) XG[j] |= BIT2;
		if (XG_hki[i][j]&BIT3) XG[j] |= BIT3;
		if (XG_hki[i][j]&BIT5) XG[j] |= BIT5;
	}
    }
 }
}

/* PROCEDURE BEPAAL TIJD TOT ALTERNATIEVE REALISATIE, TTR[] */
/* ======================================================== */

void proc_bttr_crsv(void)                      /* Fik150101 */
{
 mulv i,j,k;
 mulv max,extra,ontruim;
 mulv rest;

 for (i=0; i<FCMAX; i++) {

   if (R[i] && !TRG[i] && (A[i] || (Aled[i] > 0)) && !(VA[i]&BIT1) && !BL[i]) {
     if (!VA[i]) VA[i] |= BIT7;			/* Start detectie- cq mee-aanvraag */
     VA[i] |= BIT1;
   }

   TTK[i] = TEP[i] = 255;	/* Initialisatie TTK[] en TEP[] */
   if (G[i]) TTR[i] = FALSE;	/* Reset  TTR[] */
   else {			/* Bepaal TTR[] */
	  max = 0;
          if (GL[i]) {
	   if (TGL[i]) {
             max=(TGL_max[i]-TGL_timer[i]+TRG_max[i])/10;
	   } else max=TRG_max[i]/10;  
	  }
	  if (TRG[i])
	  max=(TRG_max[i]-TRG_timer[i])/10;

	  extra = 0;
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
				/* Conflict is groen */
	    if (G[k] || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))) {

	      extra  = 2;	/* 2 sec. extra i.v.m. afrondingsfouten */
	      ontruim= 0;
	      if (TO_max[k][i] >= 0) {
		ontruim= (TGL_max[k]+TO_max[k][i])/10;
	      }
	      ontruim+=TEG[k];
	      if (ontruim > max) max = ontruim;
	    } else {
	      if (TO_max[k][i] >= 0) {
	        if (GL[k]) {
		  if (TGL[k]) {
		    ontruim=(TGL_max[k]-TGL_timer[k]+TO_max[k][i])/10;
		  } else ontruim=TO_max[k][i]/10;
		  if (ontruim > max) max = ontruim;
	        } else {
		  if (R[k] && TO[k][i]) {
		    ontruim=(TO_max[k][i]-TO_timer[k])/10;
		    if (ontruim > max) max = ontruim;
		  }
	        }
	      }
	    }
	  }
          if (RA[i] && (extra==2) || RV[i] && (extra==0)) extra = 1;
	  TTR[i] = max + extra;
      }

   if (G[i]) TTR_WV[i] = FALSE;	/* Reset  TTR_WV[] */
   else {			/* Bepaal TTR_WV[] */
	  max = 0;
          if (GL[i]) {
	   if (TGL[i]) {
             max=TGL_max[i]-TGL_timer[i]+TRG_max[i];
	   } else max=TRG_max[i];  
	  }
	  if (TRG[i])
	  max=TRG_max[i]-TRG_timer[i];

	  extra = 0;
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
				/* Conflict is groen */
	    if (G[k] || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))) {

	      ontruim= 0;
	      if (TO_max[k][i] >= 0) {
		ontruim=TGL_max[k]+TO_max[k][i];
	      }
	      ontruim+=(TEG[k]*10);
	      if (ontruim > max) max = ontruim;
	    } else {
	      if (TO_max[k][i] >= 0) {
	        if (GL[k]) {
		  if (TGL[k]) {
		    ontruim=TGL_max[k]-TGL_timer[k]+TO_max[k][i];
		  } else ontruim=TO_max[k][i];
		  if (ontruim > max) max = ontruim;
	        } else {
		  if (R[k] && TO[k][i]) {
		    ontruim=TO_max[k][i]-TO_timer[k];
		    if (ontruim > max) max = ontruim;
		  }
	        }
	      }
	    }
	  }
	  TTR_WV[i] = max;
      }

   /* if (SG[i]) VG1[i] = TRUE; -> verplaatst naar reset_start() in Traffick   */
   /* else	{                               en reset_einde() in Traffick   */
   /*	if (EG[i] || !FG[i] && !(MK[i]&BIT0) && !(MK[i]&BIT1)) VG1[i] = FALSE; */
   /*	}                                                                      */
   if (G[i] && (VG1[i] || (MK[i]&BIT2)) && (HVAS[i]&BIT0)) YV[i] |= BIT1;
   if (G[i] && (VG1[i] || (MK[i]&BIT2))                  ) YV[i] |= BIT2;
 }

 for (i=0; i<FCMAX; i++) {

   if (G[i]) TSV[i] = FALSE;	/* Reset  TSV[] */
   else {			/* Bepaal TSV[] */
	  max = 0;
          if (GL[i]) {
	   if (TGL[i]) {
             max=(TGL_max[i]-TGL_timer[i]+TRG_max[i])/10;
	   } else max=TRG_max[i]/10;  
	  }
	  if (TRG[i])
	  max=(TRG_max[i]-TRG_timer[i])/10;

	  extra = 0;
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
				/* Conflict is groen */
	    if (G[k] || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))) {

	      extra  = 2;	/* 2 sec. extra i.v.m. afrondingsfouten */
	      ontruim= 0;
	      if (TO_max[k][i] >= 0) {
		ontruim= (TGL_max[k]+TO_max[k][i])/10;
	      }

	      rest = 0;
              if ( SG[k] || SFG[k]) rest = TFG_max[k]/10;
	      if (TFG[k]) rest = TFG_max[k]/10 - TFG_timer[k]/10;
	      if ( !G[k]) rest = TFG_max[k]/10 + TTR[k];

	      ontruim+=rest;
	      if (ontruim > max) max = ontruim;
	    } else {
	      if (TO_max[k][i] >= 0) {
	        if (GL[k]) {
		  if (TGL[k]) {
		    ontruim=(TGL_max[k]-TGL_timer[k]+TO_max[k][i])/10;
		  } else ontruim=TO_max[k][i]/10;
		  if (ontruim > max) max = ontruim;
	        } else {
		  if (R[k] && TO[k][i]) {
		    ontruim=(TO_max[k][i]-TO_timer[k])/10;
		    if (ontruim > max) max = ontruim;
		  }
	        }
	      }
	    }
	  }
          if (RA[i] && (extra==2) || RV[i] && (extra==0)) extra = 1;
	  TSV[i] = max + extra;
      }
 }

 for (i=0; i<aant_fts; i++) {
   proc_cttr_crsv_vtg_fts(fts[i][0],NG,fts[i][1],NG,fts[i][2],NG,NG);
 }
 for (i=0; i<aant_bbv; i++) {
   proc_cttr_crsv_vtg_fts(bbv[i][0],bbv[i][1],bbv[i][4],NG,bbv[i][5],NG,NG);
 }
 for (i=0; i<aant_gov; i++) {
   proc_cttr_crsv_vtg_fts(gov[i][0],gov[i][1],gov[i][7]
			 ,gov[i][8],gov[i][13],gov[i][14],gov[i][22]);
 }
 proc_cttr_crsv_hki();
}

/* PROCEDURE CORRIGEER TTR[] VOOR FIETS.VOETGANGERSKOPPELINGEN */
/* =========================================================== */

void proc_cttr_crsv_vtg_fts(                 /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 (vtg) */
	mulv fc2,                            /* Signaalgroep 2 (vtg) */
	mulv fc3,                            /* Gekoppelde.fietsr. 1 */
	mulv fc4,                            /* Gekoppelde.fietsr. 2 */
	mulv fc5,                            /* Gekoppelde.fietsr. 3 */
	mulv fc6,                            /* Gekoppelde.fietsr. 4 */
	mulv fc7)                            /* Signaalgroep 3e voetganger */
{
 mulv _maxTTR = 0;
 mulv _maxTSV = 0;
 mulv _maxTTR_WV = 0;

 /* Bepaal TTR */
 /* ---------- */

 if (fc1 >= 0) {
	if (TTR[fc1]    > _maxTTR   ) _maxTTR    = TTR[fc1];
	if (TSV[fc1]    > _maxTSV   ) _maxTSV    = TSV[fc1];
	if (TTR_WV[fc1] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc1];
 }
 if (fc2 >= 0) {
	if (TTR[fc2]    > _maxTTR   ) _maxTTR    = TTR[fc2];
	if (TSV[fc2]    > _maxTSV   ) _maxTSV    = TSV[fc2];
	if (TTR_WV[fc2] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc2];
 }
 if (fc3 >= 0) {
	if (TTR[fc3]    > _maxTTR   ) _maxTTR    = TTR[fc3];
	if (TSV[fc3]    > _maxTSV   ) _maxTSV    = TSV[fc3];
	if (TTR_WV[fc3] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc3];
 }
 if (fc4 >= 0) {
	if (TTR[fc4]    > _maxTTR   ) _maxTTR    = TTR[fc4];
	if (TSV[fc4]    > _maxTSV   ) _maxTSV    = TSV[fc4];
	if (TTR_WV[fc4] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc4];
 }
 if (fc5 >= 0) {
	if (TTR[fc5]    > _maxTTR   ) _maxTTR    = TTR[fc5];
	if (TSV[fc5]    > _maxTSV   ) _maxTSV    = TSV[fc5];
	if (TTR_WV[fc5] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc5];
 }
 if (fc6 >= 0) {
	if (TTR[fc6]    > _maxTTR   ) _maxTTR    = TTR[fc6];
	if (TSV[fc6]    > _maxTSV   ) _maxTSV    = TSV[fc6];
	if (TTR_WV[fc6] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc6];
 }
 if (fc7 >= 0) {
	if (TTR[fc7]    > _maxTTR   ) _maxTTR    = TTR[fc7];
	if (TSV[fc7]    > _maxTSV   ) _maxTSV    = TSV[fc7];
	if (TTR_WV[fc7] > _maxTTR_WV) _maxTTR_WV = TTR_WV[fc7];
 }

 if (fc1 >= 0) { TTR[fc1] = _maxTTR; TSV[fc1] = _maxTSV; TTR_WV[fc1] = _maxTTR_WV; }
 if (fc2 >= 0) { TTR[fc2] = _maxTTR; TSV[fc2] = _maxTSV; TTR_WV[fc2] = _maxTTR_WV; }
 if (fc3 >= 0) { TTR[fc3] = _maxTTR; TSV[fc3] = _maxTSV; TTR_WV[fc3] = _maxTTR_WV; }
 if (fc4 >= 0) { TTR[fc4] = _maxTTR; TSV[fc4] = _maxTSV; TTR_WV[fc4] = _maxTTR_WV; }
 if (fc5 >= 0) { TTR[fc5] = _maxTTR; TSV[fc5] = _maxTSV; TTR_WV[fc5] = _maxTTR_WV; }
 if (fc6 >= 0) { TTR[fc6] = _maxTTR; TSV[fc6] = _maxTSV; TTR_WV[fc6] = _maxTTR_WV; }
 if (fc7 >= 0) { TTR[fc7] = _maxTTR; TSV[fc7] = _maxTSV; TTR_WV[fc7] = _maxTTR_WV; }

 /* Bepaal fictieve aanvragen voetgangers onderling */
 /* ----------------------------------------------- */

 if ((fc1 >= 0) && (fc2 >= 0) && (fc7 < 0)) {
	if (R[fc1] && VA[fc1] && !(VA[fc1]&BIT7) || (HVAS[fc1]&BIT5)) {
		FCA[fc2] |=   BIT0;
	} else {
		FCA[fc2] &= (~BIT0);
	}
	if (R[fc2] && VA[fc2] && !(VA[fc2]&BIT7) || (HVAS[fc2]&BIT5)) {
		FCA[fc1] |=   BIT0;
	} else {
		FCA[fc1] &= (~BIT0);
	}
 } else {
        if (fc7 < 0) {
		if (fc1 >= 0) FCA[fc1] &= (~BIT0);
		if (fc2 >= 0) FCA[fc2] &= (~BIT0);
	}
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (fc7 >= 0)) {
	if (R[fc1] && VA[fc1] && !(VA[fc1]&BIT7) || (HVAS[fc1]&BIT5) ||
           (R[fc2] && VA[fc2] && !(VA[fc2]&BIT7) || (HVAS[fc2]&BIT5))) {
		FCA[fc7] |=   BIT0;
	} else {
		FCA[fc7] &= (~BIT0);
	}
	if (R[fc1] && VA[fc1] && !(VA[fc1]&BIT7) || (HVAS[fc1]&BIT5) ||
           (R[fc7] && VA[fc7] && !(VA[fc7]&BIT7) || (HVAS[fc7]&BIT5))) {
		FCA[fc2] |=   BIT0;
	} else {
		FCA[fc2] &= (~BIT0);
	}
	if (R[fc2] && VA[fc2] && !(VA[fc2]&BIT7) || (HVAS[fc2]&BIT5) ||
           (R[fc7] && VA[fc7] && !(VA[fc7]&BIT7) || (HVAS[fc7]&BIT5))) {
		FCA[fc1] |=   BIT0;
	} else {
		FCA[fc1] &= (~BIT0);
	}
 } else {
        if (fc7 >= 0) {
		if (fc1 >= 0) FCA[fc1] &= (~BIT0);
		if (fc2 >= 0) FCA[fc2] &= (~BIT0);
	}
	if (fc7 >= 0) FCA[fc7] &= (~BIT0);
 }

 /* Bepaal fictieve aanvragen gekoppelde fietsers */
 /* --------------------------------------------- */

 if ((fc3 >= 0) && (fc4 >= 0)) {
	if (R[fc3] && VA[fc3] && !(VA[fc3]&BIT7) || (HVAS[fc3]&BIT5)) {
		FCA[fc4] |=   BIT0;
	} else {
		FCA[fc4] &= (~BIT0);
	}
 } else {
	if (fc4 >= 0) FCA[fc4] &= (~BIT0);
 }

 if ((fc5 >= 0) && (fc6 >= 0)) {
	if (R[fc5] && VA[fc5] && !(VA[fc5]&BIT7) || (HVAS[fc5]&BIT5)) {
		FCA[fc6] |=   BIT0;
	} else {
		FCA[fc6] &= (~BIT0);
	}
 } else {
	if (fc6 >= 0) FCA[fc6] &= (~BIT0);
 }

 /* Bepaal overige fictieve aanvragen */
 /* --------------------------------- */

 if (fc1 >= 0) FCA[fc1] &= (~BIT1);
 if (fc2 >= 0) FCA[fc2] &= (~BIT1);
 if (fc3 >= 0) FCA[fc3] &= (~BIT1);
 if (fc4 >= 0) FCA[fc4] &= (~BIT1);
 if (fc5 >= 0) FCA[fc5] &= (~BIT1);
 if (fc6 >= 0) FCA[fc6] &= (~BIT1);
 if (fc7 >= 0) FCA[fc7] &= (~BIT1);

 if (fc1 >= 0) {
	if (R[fc1] && VA[fc1] && !(VA[fc1]&BIT7) || (HVAS[fc1]&BIT5)) {
		if (fc3 >= 0) FCA[fc3] |= BIT1;
		if (fc4 >= 0) FCA[fc4] |= BIT1;
		if (fc5 >= 0) FCA[fc5] |= BIT1;
		if (fc6 >= 0) FCA[fc6] |= BIT1; /* fc2 en fc7 krijgen bit0 */
	}
 }
 if (fc2 >= 0) {
	if (R[fc2] && VA[fc2] && !(VA[fc2]&BIT7) || (HVAS[fc2]&BIT5)) {
		if (fc3 >= 0) FCA[fc3] |= BIT1;
		if (fc4 >= 0) FCA[fc4] |= BIT1;
		if (fc5 >= 0) FCA[fc5] |= BIT1;
		if (fc6 >= 0) FCA[fc6] |= BIT1; /* fc1 en fc7 krijgen bit0 */
	}
 }
 if (fc7 >= 0) {
	if (R[fc7] && VA[fc7] && !(VA[fc7]&BIT7) || (HVAS[fc7]&BIT5)) {
		if (fc3 >= 0) FCA[fc3] |= BIT1;
		if (fc4 >= 0) FCA[fc4] |= BIT1;
		if (fc5 >= 0) FCA[fc5] |= BIT1;
		if (fc6 >= 0) FCA[fc6] |= BIT1; /* fc1 en fc2 krijgen bit0 */
	}
 }

 if (fc3 >= 0) {
	if (R[fc3] && VA[fc3] && !(VA[fc3]&BIT7) || (HVAS[fc3]&BIT5)) {
		if (fc1 >= 0) FCA[fc1] |= BIT1;
		if (fc2 >= 0) FCA[fc2] |= BIT1;
		if (fc7 >= 0) FCA[fc7] |= BIT1;
		if (fc5 >= 0) FCA[fc5] |= BIT1;
		if (fc6 >= 0) FCA[fc6] |= BIT1; /* fc4 krijgt bit0 */
	}
 }
 if (fc4 >= 0) {
	if (R[fc4] && VA[fc4] && !(VA[fc4]&BIT7) || (HVAS[fc4]&BIT5)) {
		if (fc1 >= 0) FCA[fc1] |= BIT1;
		if (fc2 >= 0) FCA[fc2] |= BIT1;
		if (fc7 >= 0) FCA[fc7] |= BIT1;
		if (fc3 >= 0) FCA[fc3] |= BIT1;
		if (fc5 >= 0) FCA[fc5] |= BIT1;
		if (fc6 >= 0) FCA[fc6] |= BIT1;
	}
 }
 if (fc5 >= 0) {
	if (R[fc5] && VA[fc5] && !(VA[fc5]&BIT7) || (HVAS[fc5]&BIT5)) {
		if (fc1 >= 0) FCA[fc1] |= BIT1;
		if (fc2 >= 0) FCA[fc2] |= BIT1;
		if (fc7 >= 0) FCA[fc7] |= BIT1;
		if (fc3 >= 0) FCA[fc3] |= BIT1;
		if (fc4 >= 0) FCA[fc4] |= BIT1; /* fc6 krijgt bit0 */
	}
 }
 if (fc6 >= 0) {
	if (R[fc6] && VA[fc6] && !(VA[fc6]&BIT7) || (HVAS[fc6]&BIT5)) {
		if (fc1 >= 0) FCA[fc1] |= BIT1;
		if (fc2 >= 0) FCA[fc2] |= BIT1;
		if (fc7 >= 0) FCA[fc7] |= BIT1;
		if (fc3 >= 0) FCA[fc3] |= BIT1;
		if (fc4 >= 0) FCA[fc4] |= BIT1;
		if (fc5 >= 0) FCA[fc5] |= BIT1;
	}
 }
}

/* PROCEDURE CORRIGEER TTR[] VOOR SERIELE KOPPELING */
/* ================================================ */

void proc_cttr_crsv_hki(void)                /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,x;
 mulv kptvs12,kptvs23;
 mulv voorstart;

 for (x=0; x<3; x++) {							/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptvs12 = hki[i][2];
	fc3     = hki[i][10];
	kptvs23 = hki[i][11];
	stat    = hki[i][20];

	if (stat >= 2) {						/* koppeling is aktief */
		if (fc3 >= 0) {
			voorstart = T_max[kptvs23]/10;
			if (TTR[fc3] > voorstart) {
				if ((TTR[fc3] - voorstart) > TTR[fc2]) TTR[fc2] = TTR[fc3] - voorstart;
			}
		}
		voorstart = T_max[kptvs12]/10;
		if (TTR[fc2] > voorstart) {
			if ((TTR[fc2] - voorstart) > TTR[fc1]) TTR[fc1] = TTR[fc2] - voorstart;
		}
	}
    }
 }
}

/* PROCEDURE BEPAAL TIJD TOT PRIMAIRE REALISATIE, TTP[] */
/* ==================================================== */

void proc_bttp_crsv(void)                    /* Fik150101 */
{
 mulv i;

 for (i=0; i<FCMAX; i++) {

	if (AKTPRG) {				/* Bepaal TTP[] en UTTP[] */
	  if (HVAS[i]&BIT0) { VTTP[i] = TTP[i] = FALSE; }
	  else {
	    if ((TS || N_ts || SVAS) && !(HVAS[i]&BIT3)) {
		  if (STAP<VAS2[i]) { TTP[i] = VTTP[i] = VAS2[i] - STAP; }
		  else		    { TTP[i] = VTTP[i] = CTYD    - STAP + VAS2[i]; }
	    }
	  }
	  if (PVAS[i]&BIT0) { UTTP[i] = FALSE; }
	  else {
	    if ((TS || N_ts || SVAS)  && !(HVAS[i]&BIT3)) {
		  if (STAP<VS2M[i]) { UTTP[i] = VS2M[i] - STAP; }
		  else		    { UTTP[i] = CTYD    - STAP + VS2M[i]; }
	    }
	  }
	} else { TTP[i] = VTTP[i] = UTTP[i] = 255; }

	if (AKTPRG && (HVAS[i]&BIT1)) {		/* Bepaal TTU[] */
	    if (TS || N_ts || SVAS) {
		  if (STAP<VAS1[i]) { TTU[i] = VAS1[i] - STAP; }
		  else		    { TTU[i] = CTYD    - STAP + VAS1[i]; }
	  }
	} else { TTU[i] = 0; }
 }
}

/* PROCEDURE BEPAAL PRIORITEIT OPENBAAR VERVOER */
/* ============================================ */

void proc_prio_crsv(void)                    /* Fik150101 */
{
 mulv i,j,k,x;
 mulv _UTTP,_TEP,_REST;
 mulv ontruim,tts,_min_tts;
 mulv o_tijd,gltijd;
 mulv uitm_bew;
 mulv min_groen,start_pri;
 bool  prio_aktief,_uitm_bew;
 mulv prog_keuze = 20;
 if (PROGKEUZE_CS != NG) prog_keuze = PRM[PROGKEUZE_CS];

 if (AKTPRG) {

   /* Update aanwezigheidstijd openbaar vervoer */
   if (TS || N_ts) {
     for (i=0; i<FCMAX; i++) {
	if ((ovp[i][4] >= 0) && (ovp[i][7] < 255)) ovp[i][7]++;
	if ((ovp[i][9] >= 0) && (ovp[i][12]< 255)) ovp[i][12]++;
	if ((ovp[i][14]>= 0) && (ovp[i][17]< 255)) ovp[i][17]++;
     }
   }

   for (i=0; i<FCMAX; i++) {	/* Bepaal TEP[] */

     if ((VAS5[i] != 0) && !(PVS2[i]&BIT1)) {	/* Bepaal TEP[] agv afkapmoment */
	if (!(PVS2[i]&BIT0) && ((HVAS[i]&BIT1) || (HVAS[i]&BIT6))) TEP[i] = 0;
	else {
		if (STAP <= VAS5[i]) _REST = VAS5[i] - STAP;
		else _REST = CTYD - STAP + VAS5[i];
		if (_REST < TEP[i]) TEP[i] = _REST;
	}
     }

     if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {
	if ((HVAS[i]&BIT3) || (HVAS[i]&BIT0) && G[i] || SG[i] && (VA[i]&BIT4)) {
	  if (STAP<VS2M[i]) { _UTTP = VS2M[i] - STAP + CTYD;    } /* Gerealiseerd?  */
	  else		    { _UTTP = CTYD    - STAP + VS2M[i]; } /* ... dan + CTYD */
	} else {
	  _UTTP = UTTP[i]; /* Niet gerealiseerd? dan copie van UTTP[i] */
	}
        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TO_pointer[i][j];

	  gltijd = 0;
	  o_tijd = 0;
	  if (TO_max[k][i] >= 0) {
	    gltijd = TGL_max[k]/10;
	    o_tijd = TO_max[k][i]/10;
	  }
	  ontruim = gltijd + o_tijd;

	  if (XG[i] && RA[i]) { TEP[k] = FALSE; }
	  else {
	    if (_UTTP <= ontruim)              { TEP[k] = FALSE; }
	    else if ((_UTTP-ontruim) < TEP[k]) { TEP[k] = _UTTP - ontruim; }
	  }
	}
     }
   }
   proc_ctep_crsv_hki();

   for (i=0; i<FCMAX; i++) {	/* Bepaal TSP[] */
	TSP[i]  = 0;		/* Initialiseer TSP[]  */
	TTPR[i] = 255;		/* Initialiseer TTPR[] */
	PVAS[i] &= (~BIT3);	/* Reset conflict bit  */
	PVS2[i] &= (~BIT3);	/* Reset conflict bit doorsnijding */
   }
   proc_bcob_crsv();		/* Bepaal conflict bit */
   proc_btsp_crsv();		/* Bereken TSP[]       */
   proc_knp_crsv_hki();

   for (i=0; i<FCMAX; i++) {			/* Volgrichting volgt voeding */
	if (XG[i]&BIT4) {
		if (G[i]) TTPR[i] = 0;
		if (TSP[i] < TTPR[i]) TTPR[i] = TSP[i];
   	        if (R[i] && !TRG[i] && !somfbk_crsv(i) && ((TTPR[i] < TTR[i]) || (TTR[i] == 0)) || RA[i]) {
			 B[i] |= BIT4;
			VA[i] |= BIT0;
	        }
		proc_ctegov_crsv_hki();
		proc_btsp_crsv(); 		/* Herbereken TSP[] */
	}
   }

   for (i=0; i<FCMAX; i++) {			/* Aanhouden groen door OV */
     uitm_bew = 0;
     if (ovp[i][2] >= 0) uitm_bew = PRM[ovp[i][2]]/10;

     if (G[i] && (TEG_OV[i] > 0) && (KG[i] < ALTM[i]) && (YV[i]&BIT2)) TEG_OV[i] = ALTM[i] - KG[i];
     else TEG_OV[i] = 0;
     for (j=1; j<=3; j++) {
       prio_aktief = (ovp[i][(j*5)+1] >= 10);	/* Prioriteit al aktief */
       _TEP = TEP[i];
       if (prio_aktief) _TEP += 2;
       prio_aktief = FALSE;

       if (G[i] && (ovp[i][(j*5)-1] >= 0) && (ovp[i][(j*5)+1] >= 1)
		&& (prog_keuze != 30)) {				/* Er is een bus aanwezig   */
	 if ((KG[i] + _TEP) >= ovp[i][(j*5)]) {				/* Min.groen criterium oke  */
	   tts = ovp[i][(j*5)-2] - ovp[i][(j*5)+2];			/* TTS gecorrigeerd met awt */
	   _min_tts = ovp[i][(j*5)] - KG[i];
	   if (_min_tts < 0) _min_tts = 0;
	   if (tts < _min_tts) tts = _min_tts;				/* ... maar met een minimum */
	   if (tts <= uitm_bew) {					/* TTS kleiner dan uitm.bew.*/
	     if (tts <= _TEP) {						/* Bus past in het raster   */
	       if (tts > TEG_OV[i]) TEG_OV[i] = tts;			/* Bepaal TEG_OV            */
	       prio_aktief = TRUE;
	       TTPR[i] = 0;
	       PVAS[i] |= BIT2;						/* Vlag prioriteit aktief */
	       proc_ctegov_crsv_hki();
	       proc_btsp_crsv(); 					/* Herbereken TSP[] */
	     }
	   }
	 }
       }
       if (G[i]) {
         if (prio_aktief) {
	   if (ovp[i][(j*5)+1] >=100) ovp[i][(j*5)+1] -= 100;
	   if (ovp[i][(j*5)+1] <  10) ovp[i][(j*5)+1] += 10;
         } else {
	   if (ovp[i][(j*5)+1] >=100) ovp[i][(j*5)+1] -= 100;
	   if (ovp[i][(j*5)+1] >= 10) ovp[i][(j*5)+1] -= 10;
         }
       }
     }
     if (G[i] && (TEG_OV[i] > 0) && (TTPR[i] != 0)) {
	TTPR[i] = 0;
	PVAS[i] |= BIT2;				/* Vlag prioriteit aktief */
	proc_ctegov_crsv_hki();
	proc_btsp_crsv(); 				/* Herbereken TSP[] */
     }
     if (G[i] && (TTPR[i] != 0)) PVAS[i] &= (~BIT2);	/* Reset prioriteit aktief */
   }

   for (i=0; i<FCMAX; i++) {				/* prioriteitsrealisatie OV */
     proc_bdsn_crsv(i);					/* bepaal doorsnijden */
     uitm_bew = 0;
     if (ovp[i][2] >= 0) uitm_bew = PRM[ovp[i][2]]/10;
     /* TEG_OV[i] niet gelijk aan nul maken, is al gebeurd */

     if (!G[i] && !(XG[i]&BIT4)) {
	PVAS[i] &= (~BIT2);	/* Reset prioriteit aktief */
	VA[i]   &= (~BIT0);	/* Iedere machineslag opnieuw bepalen */
     }
     /* prioriteit dmv doorsnijden */
     for (j=1; j<=3; j++) {				/* ... Prioriteit in groen gaat voor  */
       prio_aktief = (ovp[i][(j*5)+1] >= 100);		/* Prioriteit al aktief */
       _TEP = DSEP[i];
       if (prio_aktief) _TEP += 2;
       prio_aktief = FALSE;

       if (R[i] && (ovp[i][(j*5)-1] >= 0) && (ovp[i][(j*5)+1] >= 2) && (PVS2[i]&BIT7)
		 && !(PVAS[i]&BIT3) && (prog_keuze != 30)) {	/* Er is een bus aanwezig   */
	if ((_TEP > (DSSP[i]+ALTM[i])) && (DSSP[i]<255)) {	/* Min.ALTM mogelijk        */
	 if ((_TEP-DSSP[i]) >= ovp[i][(j*5)]) {			/* Min.groen criterium oke  */
	   tts = ovp[i][(j*5)-2] - ovp[i][(j*5)+2];		/* TTS gecorrigeerd met awt */
	   if (tts < ovp[i][(j*5)]) tts = ovp[i][(j*5)];	/* ... maar minimaal vgr[]  */
	   if (tts <= uitm_bew) {				/* TTS kleiner dan uitm.bew.*/
	     if ((tts <= _TEP) && !BL[i]) {			/* Bus past in het raster   */
	       if (tts > TEG_OV[i]) TEG_OV[i] = tts;		/* Bepaal TEG_OV            */
	       prio_aktief = TRUE;
	       PVAS[i] |= BIT2;					/* Vlag prioriteit aktief   */

	       proc_ctegov_crsv_hki();
	       proc_bcob_crsv();	/* set blokkeer bit bij conflicten */

	       /* bepaal tijd tot prioriteitsrealisatie */
	       /* ------------------------------------- */
	       min_groen = TFG_max[i]/10;
	       if (ovp[i][(j*5)] > min_groen) min_groen = ovp[i][(j*5)];
	       start_pri = TEG_OV[i] - min_groen;
	       if (start_pri < DSSP[i]) start_pri = DSSP[i];
	       if (start_pri < TTPR[i]) TTPR[i] = start_pri;
   	       if (R[i] && !TRG[i] && !somfbk_crsv(i) && ((TTPR[i] < TTR[i]) || (TTR[i] == 0)) || RA[i]) {
			VA[i] |= BIT0;
			 B[i] |= BIT4;
	       }
	     }
	   }
	 }
	}
       }
       if (R[i]) {
         if (prio_aktief) {
	   if (ovp[i][(j*5)+1] < 100) ovp[i][(j*5)+1] += 100;
         } else {
	   if (ovp[i][(j*5)+1] >=100) {
		ovp[i][(j*5)+1] -= 100;
		if (ovp[i][(j*5)+1] < 10) ovp[i][(j*5)+1] += 10;
	   }
         }
       }
     }
     for (j=0; j<FCMAX; j++) PVS2[j] &= (~BIT7);	/* Ruim kladblok op */

     /* prioriteit dmv afbreken */
     for (j=1; j<=3; j++) {				/* ... Prioriteit in groen gaat voor  */
       prio_aktief = (ovp[i][(j*5)+1] >= 10)		/* Prioriteit al aktief */
		  && (ovp[i][(j*5)+1] < 100); 
       _TEP = TEP[i];
       if (prio_aktief) _TEP += 2;
       prio_aktief = FALSE;

       if (R[i] && (ovp[i][(j*5)-1] >= 0) && (ovp[i][(j*5)+1] >= 2) && (ovp[i][(j*5)+1] < 100)
		 && !(PVAS[i]&BIT3) && (prog_keuze != 30)) {	/* Er is een bus aanwezig   */
	if ((_TEP > (TSP[i]+ALTM[i])) && (TSP[i]<255))	 {	/* Min.ALTM mogelijk        */
	 if ((_TEP-TSP[i]) >= ovp[i][(j*5)]) {			/* Min.groen criterium oke  */
	   tts = ovp[i][(j*5)-2] - ovp[i][(j*5)+2];		/* TTS gecorrigeerd met awt */
	   if (tts < ovp[i][(j*5)]) tts = ovp[i][(j*5)];	/* ... maar minimaal vgr[]  */
	   if (tts <= uitm_bew) {				/* TTS kleiner dan uitm.bew.*/
	     if ((tts <= _TEP) && !BL[i]) {			/* Bus past in het raster   */
	       if (tts > TEG_OV[i]) TEG_OV[i] = tts;		/* Bepaal TEG_OV            */
	       prio_aktief = TRUE;
	       PVAS[i] |= BIT2;					/* Vlag prioriteit aktief   */

	       proc_ctegov_crsv_hki();
	       proc_bcob_crsv();	/* set blokkeer bit bij conflicten */

	       /* bepaal tijd tot prioriteitsrealisatie */
	       /* ------------------------------------- */
	       min_groen = TFG_max[i]/10;
	       if (ovp[i][(j*5)] > min_groen) min_groen = ovp[i][(j*5)];
	       start_pri = TEG_OV[i] - min_groen;
	       if (start_pri < TSP[i]) start_pri = TSP[i];
	       if (start_pri < TTPR[i]) TTPR[i] = start_pri;
   	       if (R[i] && !TRG[i] && !somfbk_crsv(i) && ((TTPR[i] < TTR[i]) || (TTR[i] == 0)) || RA[i]) {
			VA[i] |= BIT0;
			 B[i] |= BIT4;
	       }
	     }
	   }
	 }
	}
       }
       if (R[i]) {
         if (prio_aktief) {
	   if ((TTPR[i] + ALTM[i]) > TEG_OV[i]) TEG_OV[i] = TTPR[i] + ALTM[i];	/* Fik090816 */
	   if (ovp[i][(j*5)+1] < 10) ovp[i][(j*5)+1] += 10;
         } else {
	   if (ovp[i][(j*5)+1] >= 100) {
		if ((ovp[i][(j*5)+1] - 100) >= 10) ovp[i][(j*5)+1] -= 10;
	   }
	   if ((ovp[i][(j*5)+1] >=10) && (ovp[i][(j*5)+1] < 100)) ovp[i][(j*5)+1] -= 10;
         }
       }
     }
   }

   /* Corrigeer TTP[] na afloop aanhouden groen door busprioriteit */
   /* ------------------------------------------------------------ */
   for (i=0; i<FCMAX; i++) {
	if (G[i]) CTTP[i] = 0;
	if ((CTTP[i] > 0) && (TS || N_ts)) CTTP[i]--;
	if ( CTTP[i] > UTTP[i]) CTTP[i] = UTTP[i];

	if (TTP[i] < 255) {
	  if (CTTP[i] > TTP[i]) TTP[i] = CTTP[i];
	} else {
	  CTTP[i] = 0;
	}
   }

   /* Corrigeer TTP[] agv aktieve prioriteitsrealisatie */
   /* ------------------------------------------------- */
   for (i=0; i<FCMAX; i++) {

	if (R[i] && (PVAS[i]&BIT2) || G[i] && (TTPR[i] == 0)) {

	  TTP[i] = TTPR[i];
          uitm_bew = 0;
          if (ovp[i][2] >= 0) uitm_bew = PRM[ovp[i][2]]/10;
	  if (G[i]) {
		RW[i] |= BIT2;		/* Aanhouden groen busrichting       */
     		for (j=1; j<=3; j++) {	/* Bepaal aanspreken uitmeldbewaking */
			_uitm_bew = FALSE;

			/* Als bus langer aanwezig dan minimum groen en langer dan tts bij inmelding */
			if ((ovp[i][(j*5)-1] >= 0) && (KG[i] >= ovp[i][(j*5)]) && (ovp[i][(j*5)+2] >= ovp[i][(j*5)-2])) {
			  if ((ovp[i][(j*5)+2] >= uitm_bew) && (!VG1[i] && !(MK[i]&BIT2) || (KG[i] >= uitm_bew))) _uitm_bew = TRUE;
			}
			if (ovp[i][(j*5)+2] >= 255) _uitm_bew = TRUE;
			if (_uitm_bew) {
	  			ovp[i][(j*5)-2] = NG;
	  			ovp[i][(j*5)-1] = NG;
	  			ovp[i][(j*5)  ] = NG;
	  			ovp[i][(j*5)+1] = NG;
	 			ovp[i][(j*5)+2] = NG;

		     		/* Sorteer resterende bussen op basis van tijd tot stopstreep */
     				proc_sorteer_bus(i,1,2);
     				proc_sorteer_bus(i,2,3);
     				proc_sorteer_bus(i,1,2);
                                VLOG_OV[i] |= BIT3;
			}
 	        }
	  } else {
     		for (j=1; j<=3; j++) {	/* Bepaal aanspreken uitmeldbewaking */
			_uitm_bew = FALSE;
			if (ovp[i][(j*5)+2] >= 255) _uitm_bew = TRUE;
			if (_uitm_bew) {
	  			ovp[i][(j*5)-2] = NG;
	  			ovp[i][(j*5)-1] = NG;
	  			ovp[i][(j*5)  ] = NG;
	  			ovp[i][(j*5)+1] = NG;
	 			ovp[i][(j*5)+2] = NG;

		     		/* Sorteer resterende bussen op basis van tijd tot stopstreep */
     				proc_sorteer_bus(i,1,2);
     				proc_sorteer_bus(i,2,3);
     				proc_sorteer_bus(i,1,2);
			}
		}
	  }

          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
	    gltijd = 0;
	    o_tijd = 0;
	    if (TO_max[i][k] >= 0) {
	      gltijd = TGL_max[i]/10;
	      o_tijd = TO_max[i][k]/10;
	    }
	    ontruim = gltijd + o_tijd;
	    if (G[i] && (ontruim > CTTP[k])) CTTP[k] = ontruim;

	    gltijd = 0;
	    o_tijd = 0;
	    if (TO_max[k][i] >= 0) {
	      gltijd = TGL_max[k]/10;
	      o_tijd = TO_max[k][i]/10;
	    }
	    ontruim = gltijd + o_tijd;

	    if (TTPR[i]<=ontruim) {
		RR[k] |= BIT2;
		BAR[k] = TRUE;
		TTK[k] = FALSE;
		if (G[k]) Z[k] |= BIT2;
	    } else {
		if ((TTPR[i]-ontruim)<TTK[k]) { TTK[k] = TTPR[i]-ontruim; }
	    }

	    if (!G[k] || (FCA[k]&BIT0)) {	/* Fic.aanv. gekoppelde.vtg. aanwezig */
						/* danwel voedende fietser aanwezig   */

		ontruim = gltijd + o_tijd + TTR[k] + ALTM[k];
	        if (TTPR[i]<=ontruim) {
			RR[k] |= BIT2;
			BAR[k] = TRUE;
		}
	    }

	    gltijd = 0;
	    o_tijd = 0;
	    if (TO_max[i][k] >= 0) {
	      gltijd = TGL_max[i]/10;
	      o_tijd = TO_max[i][k]/10;
	    }
	    ontruim = TEG_OV[i] + gltijd + o_tijd;

	    if ((TTP[k] < 255) && !G[k] && !EG[k]) {
		if (TTP[k] < ontruim) TTP[k] = ontruim;
		if (UTTP[k] < TTP[k]) TTP[k] = UTTP[k];
	    }

	    if (!(HVAS[k]&BIT3)) {		/* Bepaal overslag conflicten */

		if ((UTTP[k]<=ontruim) && R[i]
				       && !TRG[k] && !(HVAS[k]&BIT5) 
				       && !FCA[k]
				       && (!VA[k] || (VA[k]&BIT7))) {
			HVAS[k] |= BIT3;
			 TTP[k]  = UTTP[k] = 255;
			   X[k] |= BIT1;	/* Registratie overslag */
		}
	    }
	  }

          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];

	    if ((TTP[k] < 255) && !G[k] && !EG[k]) {
		if (TTP[k] < TEG_OV[i]) TTP[k] = TEG_OV[i];
		if (UTTP[k] < TTP[k])   TTP[k] = UTTP[k];
	    }
	  }
	}
   }

   for (x=0; x<aant_fts; x++) {
     proc_cttp_crsv_vtg_fts(fts[x][0],NG,fts[x][1],NG,fts[x][2],NG,NG);
   }
   for (x=0; x<aant_bbv; x++) {
     proc_cttp_crsv_vtg_fts(bbv[x][0],bbv[x][1],bbv[x][4],NG,bbv[x][5],NG,NG);
   }
   for (x=0; x<aant_gov; x++) {
     proc_cttp_crsv_vtg_fts(gov[x][0],gov[x][1],gov[x][7]
			   ,gov[x][8],gov[x][13],gov[x][14],gov[x][22]);
   }
   proc_cttp_crsv_hki();

   for (i=0; i<FCMAX; i++) {
	if (XG[i] && RA[i] || (YM[i]&BIT2)) {
		RR[i] &= (~BIT2);
		BAR[i] = FALSE;
	}
   }

 } else { /* Geen AKTPGR dan TEG_OV[] resetten */
   for (i=0; i<FCMAX; i++) {
	CTTP[i] = TEG_OV[i] = 0;
	PVAS[i] &= (~BIT1);
	PVAS[i] &= (~BIT2);
	PVAS[i] &= (~BIT3);
   	for (j=0; j<FCMAX; j++) {
		PVAS_hki[j][i] &= (~BIT1);
	}
   }
 }
}

/* PROCEDURE BEPAAL CONFLICT BIT */
/* ============================= */

void proc_bcob_crsv(void)                    /* Fik150101 */
{
   mulv i,j,k;

   for (i=0; i<FCMAX; i++) {

	if (R[i] && (PVAS[i]&BIT2)) {
          for (j=0; j<GKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
	    PVAS[k] |= BIT3;
	    if ((PVS2[i]&BIT7) && (PVS2[k]&BIT7)) PVS2[k] |= BIT3;
	  }
          for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
            k = TO_pointer[i][j];
	    PVAS[k] |= BIT3;
	    if ((PVS2[i]&BIT7) && (PVS2[k]&BIT7)) PVS2[k] |= BIT3;
	  }
	}
   }
   proc_cpvas_crsv_hki();
}

/* PROCEDURE (HER)BEREKEN TSP[] */
/* ============================ */

void proc_btsp_crsv(void)                    /* Fik150101 */
{
   mulv i,j,k;
   mulv ontruim,max;
   mulv o_tijd,gltijd;
   mulv _PEG,_TEG,_TTU;
   bool  _primair;

   for (i=0; i<FCMAX; i++) {

	max = 0;
        if (GL[i]) {
	 if (TGL[i]) {
           max=(TGL_max[i]-TGL_timer[i]+TRG_max[i])/10;
	 } else max=TRG_max[i]/10;  
	}
	if (TRG[i])
	max=(TRG_max[i]-TRG_timer[i])/10;

	if (!G[i] && (VAS1[i] != 0) && !(PVS2[i]&BIT0)) {	/* Correctie indien geen prio voor einde uitstel */
		if (HVAS[i]&BIT1) {
			if ((TTR[i] <= TTU[i]) && !RA[i]) {
				if (TTU[i] > max) max = TTU[i];
			}
		}
		if ((HVAS[i]&BIT6) && !(HVAS[i]&BIT1) && !(VA[i]&BIT4)) {
			if (!RA[i]) {
				if (TTP[i] > max) max = TTP[i];
			}
		}
	}

	if (TTU[i] > TPVU[i]) _TTU = TTU[i] - TPVU[i];
	else                  _TTU = 0;

	if (!G[i] && (VAS1[i] != 0) && (PVS2[i]&BIT0)) {	/* Correctie indien wel prio voor einde uitstel */
		if (HVAS[i]&BIT1) {
			if ((TTR[i] <= _TTU) && !RA[i]) {
				if (_TTU > max) max = _TTU;
			}
		}
		if ((HVAS[i]&BIT6) && !(HVAS[i]&BIT1) && !(VA[i]&BIT4)) {
			if (!RA[i]) {
				if (TTP[i] > max) max = TTP[i];
			}
		}
	}

	if (max > TSP[i]) TSP[i] = max;

        for (j=0; j<GKFC_MAX[i]; j++) {
          k = TO_pointer[i][j];

	  if (G[k] || XG[k]) {

	    gltijd = 0;
	    o_tijd = 0;
	  
	    if (TO_max[k][i] >= 0) {
	      gltijd = TGL_max[k]/10;
	      o_tijd = TO_max[k][i]/10;
	    }

	    /* volgrichtingen hebben al de juiste PEG[] en TEG[] als voeding groen is */

            _primair = FALSE;
	    _PEG = PEG[k];
	    _TEG = TEG[k];

	    if (PVAS[k]&BIT7) {	/* voetganger waarvan koppeling mag worden afgebroken */
		if (G[k]) {
			if (KG[k] >= KAPM[k]) _PEG = 0;
			else                  _PEG = KAPM[k] - KG[k];
			if (KG[k] >= ALTM[k]) _TEG = 0;
			else                  _TEG = ALTM[k] - KG[k];
		} else {
			_PEG = TTR[k] + KAPM[k];
			_TEG = TTR[k] + ALTM[k];
		}
	    }

	    if (!(HVAS[k]&BIT3)) {
		if (SG[k] && (VA[k]&BIT4) || G[k] && ((HVAS[k]&BIT0) || (HVAS[k]&BIT4))) _primair = TRUE;
	    }
	    if (_primair) ontruim = _PEG + gltijd + o_tijd;
	    else {	/* Bij alternatief kies TEG[] tenzij PEG[] een lagere waarde heeft! */
		if (_TEG <= _PEG) {
			ontruim = _TEG + gltijd + o_tijd;
		} else {
			ontruim = _PEG + gltijd + o_tijd;
		}
	    }

	    if (ontruim > TSP[i]) TSP[i] = ontruim;

	    ontruim = TEG_OV[k] + gltijd + o_tijd;
	    if (ontruim > TSP[i]) TSP[i] = ontruim;

	  } else {

	    if (TO_max[k][i] >= 0) {
	      if (GL[k]) {
	        if (TGL[k]) {
		  ontruim=(TGL_max[k]-TGL_timer[k]+TO_max[k][i])/10;
		} else ontruim=TO_max[k][i]/10;
		if (ontruim > TSP[i]) TSP[i] = ontruim;
	      } else {
		if (R[k] && TO[k][i]) {
		  ontruim=(TO_max[k][i]-TO_timer[k])/10;
		  if (ontruim > TSP[i]) TSP[i] = ontruim;
		}
	      }
	    }
	  }
	}
   }
   proc_ctsp_crsv_hki();
}

/* PROCEDURE BEPAAL DSSP[] EN DSEP */
/* =============================== */

void proc_bdsn_crsv(                         /* Fik150101 */
	mulv fc)                             /* Signaalgroep */
{
   mulv i,j,k;
   mulv ontruim,max;
   mulv o_tijd,gltijd,o_tijd2,gltijd2;
   mulv _PEG, _TEG, _REST;
   mulv _UTTP = 255; /* @#@ Fik170831 */
   mulv _TTU = 0;
   bool  _primair,_doorsnij;

   for (i=0; i<FCMAX; i++) {	/* Kladblok, vlagt conflicten die doorsneden gaan worden */
	PVS2[i] &= (~BIT7);	/* ... en OV-richting die gaat doorsnijden               */
   }

   DSSP[fc] = 0;
   DSEP[fc] = 255;

   if ((VAS5[fc] != 0) && !(PVS2[fc]&BIT1)) {	/* Initialiseer DSEP[] agv afkapmoment */
	if (!(PVS2[fc]&BIT0) && ((HVAS[fc]&BIT1) || (HVAS[fc]&BIT6))) DSEP[fc] = 0;
	else {
		if (STAP <= VAS5[fc]) _REST = VAS5[fc] - STAP;
		else _REST = CTYD - STAP + VAS5[fc];
		if (_REST < DSEP[fc]) DSEP[fc] = _REST;
	}
   }

   max = 0;					/* Initialiseer DSSP[] agv GL[], TRG[] of uitstelmoment */
   if (GL[fc]) {
	if (TGL[fc]) {
           max=(TGL_max[fc]-TGL_timer[fc]+TRG_max[fc])/10;
	} else max=TRG_max[fc]/10;  
   }
   if (TRG[fc]) max=(TRG_max[fc]-TRG_timer[fc])/10;

   if (!G[fc] && (VAS1[fc] != 0) && !(PVS2[fc]&BIT0)) {
	if (HVAS[fc]&BIT1) {
		if ((TTR[fc] <= TTU[fc]) && !RA[fc]) {
			if (TTU[fc] > max) max = TTU[fc];
		}
	}
	if ((HVAS[fc]&BIT6) && !(HVAS[fc]&BIT1) && !(VA[fc]&BIT4)) {
		if (!RA[fc]) {
			if (TTP[fc] > max) max = TTP[fc];
		}
	}
   }

   if (TTU[fc] > TPVU[fc]) _TTU = TTU[fc] - TPVU[fc];
   else                    _TTU = 0;

   if (!G[fc] && (VAS1[fc] != 0) && (PVS2[fc]&BIT0)) {
	if (HVAS[fc]&BIT1) {
		if ((TTR[fc] <= _TTU) && !RA[fc]) {
			if (_TTU > max) max = _TTU;
		}
	}
	if ((HVAS[fc]&BIT6) && !(HVAS[fc]&BIT1) && !(VA[fc]&BIT4)) {
		if (!RA[fc]) {
			if (TTP[fc] > max) max = TTP[fc];
		}
	}
   }

   if (max > DSSP[fc]) DSSP[fc] = max;

   for (j=0; j<GKFC_MAX[fc]; j++) {		/* Bepaal DSSP[] en DSEP[] */
        k = TO_pointer[fc][j];

	if (G[k] || GL[k] || TRG[k] || VA[k] || (HVAS[k]&BIT5) || FCA[k]) {
		if ((HVAS[k]&BIT3) || (HVAS[k]&BIT0) && G[k] || SG[k] && (VA[k]&BIT4)) {
		  if (STAP<VS2M[k]) { _UTTP = VS2M[k] - STAP + CTYD;    } /* Gerealiseerd?  */
		  else		    { _UTTP = CTYD    - STAP + VS2M[k]; } /* ... dan + CTYD */
		} else {
		  _UTTP = UTTP[k]; /* Niet gerealiseerd? dan copie van UTTP[k] */
		}
	}

	if (XG[k] || G[k]) {
		gltijd = 0;
		o_tijd = 0;
		gltijd2= 0;
	 	o_tijd2= 0;
 
		if ((TO_max[k][fc] >= 0) && (TO_max[fc][k] >= 0)) {
			gltijd = TGL_max[k]/10;
			gltijd2= TGL_max[fc]/10;
			o_tijd = TO_max[k][fc]/10;
			o_tijd2= TO_max[fc][k]/10;
		}
		_primair = FALSE;
		if (!(HVAS[k]&BIT3)) {
			if (SG[k] && (VA[k]&BIT4) || G[k] && ((HVAS[k]&BIT0) || (HVAS[k]&BIT4))) _primair = TRUE;
		}
		_doorsnij = FALSE;
		if (_primair && ((SDSN[k] + gltijd + o_tijd + ALTM[fc] + gltijd2 + o_tijd2) < EDSN[k]) && (EDSN[k]<255)) _doorsnij = TRUE;
		
						/* conflict is primair groen en mag worden doorsneden */
		if (G[k] && (PVAS[k]&BIT6) && !(PVAS[k]&BIT2) && !(PVS2[k]&BIT2) && _primair && _doorsnij) {
			if ((SDSN[k] + gltijd  + o_tijd ) > DSSP[fc]) DSSP[fc] = SDSN[k] + gltijd  + o_tijd;
			if ((EDSN[k] - gltijd2 - o_tijd2) < DSEP[fc]) DSEP[fc] = EDSN[k] - gltijd2 - o_tijd2;
			PVS2[k]  |= BIT7;	/* schrijf weg in kladblok */
			PVS2[fc] |= BIT7;
		} else {			/* conflict mag niet worden doorsneden */
			
			_PEG = PEG[k];		/* volgrichtingen hebben al de juiste PEG[] en TEG[] als voeding groen is */
			_TEG = TEG[k];

			if (PVAS[k]&BIT7) {	/* voetganger waarvan koppeling mag worden afgebroken */
				if (G[k]) {
					if (KG[k] >= KAPM[k]) _PEG = 0;
					else                  _PEG = KAPM[k] - KG[k];
					if (KG[k] >= ALTM[k]) _TEG = 0;
					else                  _TEG = ALTM[k] - KG[k];
				} else {
					_PEG = TTR[k] + KAPM[k];
					_TEG = TTR[k] + ALTM[k];
				}
	    		}

			if (_primair) ontruim = _PEG + gltijd + o_tijd;
			else {	/* Bij alternatief kies TEG[] tenzij PEG[] een lagere waarde heeft! */
				if (_TEG <= _PEG) {
					ontruim = _TEG + gltijd + o_tijd;
				} else {
					ontruim = _PEG + gltijd + o_tijd;
				}
			}

			if (ontruim > DSSP[fc]) DSSP[fc] = ontruim;

			ontruim = TEG_OV[k] + gltijd + o_tijd;
			if (ontruim > DSSP[fc]) DSSP[fc] = ontruim;
			if (_UTTP < DSEP[fc])   DSEP[fc] = _UTTP;
		}

	} else {	/* richting is niet groen */

	    if (TO_max[k][fc] >= 0) {
	      if (GL[k]) {
	        if (TGL[k]) {
		  ontruim=(TGL_max[k]-TGL_timer[k]+TO_max[k][fc])/10;
		} else ontruim=TO_max[k][fc]/10;
		if (ontruim > DSSP[fc]) DSSP[fc] = ontruim;
	      } else {
		if (R[k] && TO[k][fc]) {
		  ontruim=(TO_max[k][fc]-TO_timer[k])/10;
		  if (ontruim > DSSP[fc]) DSSP[fc] = ontruim;
		}
	      }

	      /* if (UTTP[fc] < DSEP[fc]) DSEP[fc] = UTTP[fc]; @#@ Fik170831 */
	      if (_UTTP < DSEP[fc]) DSEP[fc] = _UTTP;
	    }
	}
   }
   proc_cdssp_crsv_hki();
}

/* PROCEDURE CORRIGEER TEP[] VOOR SERIELE KOPPELINGEN */
/* ================================================== */

void proc_ctep_crsv_hki(void)                   /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,j,x;
 mulv kpteg12,kptdm12;
 mulv kpteg23,kptdm23;
 mulv naloop;

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kpteg12 = hki[i][4];
	kptdm12 = hki[i][5];
	fc3     = hki[i][10];
	kpteg23 = hki[i][13];
	kptdm23 = hki[i][14];
	stat    = hki[i][20];

	if (stat >= 2) {			/* koppeling is aktief */

		if (fc3 >= 0) {
			if (VAS4[fc2] <= VAS4[fc3]) naloop = VAS4[fc3] - VAS4[fc2];
			else                        naloop = VAS4[fc3] - VAS4[fc2] + CTYD;

			if (((T_max[kpteg23]+T_max[kptdm23])/10) < naloop) {
				naloop = (T_max[kpteg23]+T_max[kptdm23])/10;
			}
			if ((TEP[fc3] - naloop) < TEP[fc2]) TEP[fc2] = TEP[fc3] - naloop;
			if (TEP[fc2] < 0) TEP[fc2] = 0;
		}
		if (VAS4[fc1] <= VAS4[fc2]) naloop = VAS4[fc2] - VAS4[fc1];
		else                        naloop = VAS4[fc2] - VAS4[fc1] + CTYD;

		if (((T_max[kpteg12]+T_max[kptdm12])/10) < naloop) {
			naloop = (T_max[kpteg12]+T_max[kptdm12])/10;
		}
		if ((TEP[fc2] - naloop) < TEP[fc1]) TEP[fc1] = TEP[fc2] - naloop;
		if (TEP[fc1] < 0) TEP[fc1] = 0;

		if (  G[fc1] && (TEG_OV[fc1] >  0)) XG_hki[i][fc2] |= BIT4;
		if ((!G[fc1] || (TEG_OV[fc1] == 0)) && (KNP[fc2] == 0)) XG_hki[i][fc2] &= (~BIT4);

		if (fc3 >= 0) {
			if (( G[fc2] && (TEG_OV[fc2] >  0)) || (XG_hki[i][fc2]&BIT4)) XG_hki[i][fc3] |= BIT4;
			if ((!G[fc2] || (TEG_OV[fc2] == 0)) && (KNP[fc3] == 0) && !(XG_hki[i][fc2]&BIT4)) XG_hki[i][fc3] &= (~BIT4);
		}

	} else {				/* koppeling is niet aktief */

		XG_hki[i][fc2] &= (~BIT4);
		if (fc3 >= 0) XG_hki[i][fc3] &= (~BIT4);

	}
    }

    for (j=0; j<FCMAX; j++) {			/* bepaal XG[]&BIT4 per signaalgroep */
	XG[j] &= (~BIT4);
    }

    for (i=0; i<aant_hki; i++) {
	for (j=0; j<FCMAX; j++) {
		if (XG_hki[i][j]&BIT4) {
			XG[j]   |= BIT4;
			PVAS[j] |= BIT2;	/* volgrichting volgt prioriteit voeding */
		}
	}
    }
 }
}

/* PROCEDURE CORRIGEER TSP[] VOOR SERIELE KOPPELINGEN */
/* ================================================== */

void proc_ctsp_crsv_hki(void)                   /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,x;
 mulv kptvs12,kptsg12;
 mulv kptvs23,kptsg23;
 mulv voorstart,start_kop;

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptvs12 = hki[i][2];
	kptsg12 = hki[i][3];
	fc3     = hki[i][10];
	kptvs23 = hki[i][11];
	kptsg23 = hki[i][12];
	stat    = hki[i][20];

	if (stat >= 2) {			/* koppeling is aktief */

		if (fc3 >= 0) {
			voorstart = T_max[kptvs23]/10;
			start_kop = T_max[kptsg23]/10;

			if ((TSP[fc2] + voorstart) < TSP[fc3]) TSP[fc2] = TSP[fc3] - voorstart;
			if ((TSP[fc2] + start_kop) > TEP[fc3]) TSP[fc2] = TEP[fc3] - start_kop;
			if (TSP[fc2] < 0) TSP[fc2] = 255;

			if (!G[fc3] && ((TEP[fc3] - TSP[fc3]) < ALTM[fc3])) { TSP[fc2] = 255; TSP[fc1] = 255; }
		}

		voorstart = T_max[kptvs12]/10;
		start_kop = T_max[kptsg12]/10;

		if ((TSP[fc1] + voorstart) < TSP[fc2]) TSP[fc1] = TSP[fc2] - voorstart;
		if ((TSP[fc1] + start_kop) > TEP[fc2]) TSP[fc1] = TEP[fc2] - start_kop;
		if (TSP[fc1] < 0) TSP[fc1] = 255;

		if (!G[fc2] && ((TEP[fc2] - TSP[fc2]) < ALTM[fc2])) TSP[fc1] = 255;
	}
    }
 }
}

/* PROCEDURE CORRIGEER DSSP[] VOOR SERIELE KOPPELINGEN */
/* =================================================== */

void proc_cdssp_crsv_hki(void)			/* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,x;
 mulv kptvs12,kptsg12;
 mulv kptvs23,kptsg23;
 mulv voorstart,start_kop;

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptvs12 = hki[i][2];
	kptsg12 = hki[i][3];
	fc3     = hki[i][10];
	kptvs23 = hki[i][11];
	kptsg23 = hki[i][12];
	stat    = hki[i][20];

	if (stat >= 2) {			/* koppeling is aktief */

		if (fc3 >= 0) {
			voorstart = T_max[kptvs23]/10;
			start_kop = T_max[kptsg23]/10;

			if ((DSSP[fc2] + voorstart) < TSP[fc3]) DSSP[fc2] = TSP[fc3] - voorstart;
			if ((DSSP[fc2] + start_kop) > TEP[fc3]) DSSP[fc2] = TEP[fc3] - start_kop;
			if (DSSP[fc2] < 0) DSSP[fc2] = 255;

			if (!G[fc3] && ((TEP[fc3] - TSP[fc3]) < ALTM[fc3])) { DSSP[fc2] = 255; DSSP[fc1] = 255; }
		}

		voorstart = T_max[kptvs12]/10;
		start_kop = T_max[kptsg12]/10;

		if ((DSSP[fc1] + voorstart) < TSP[fc2]) DSSP[fc1] = TSP[fc2] - voorstart;
		if ((DSSP[fc1] + start_kop) > TEP[fc2]) DSSP[fc1] = TEP[fc2] - start_kop;
		if (DSSP[fc1] < 0) DSSP[fc1] = 255;

		if (!G[fc2] && ((TEP[fc2] - TSP[fc2]) < ALTM[fc2])) DSSP[fc1] = 255;
	}
    }
 }
}

/* PROCEDURE CORRIGEER PVAS[] VOOR SERIELE KOPPELINGEN */
/* =================================================== */

void proc_cpvas_crsv_hki(void)				/* Fik150101 */
{
 mulv fc1,fc2,fc3,stat;
 mulv i,j,k,x;

 for (x=0; x<3; x++) {					/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	fc3     = hki[i][10];
	stat    = hki[i][20];

	if (stat >= 2) {							/* koppeling is aktief */

		if (R[fc1] && (PVAS[fc1]&BIT2)) {				/* voeding fc1 heeft prioriteit */
			for (j=0; j<GKFC_MAX[fc2]; j++) {			/* primaire conflicten fc2 */
   				k = TO_pointer[fc2][j];
				PVAS[k] |= BIT3;
			}
			for (j=GKFC_MAX[fc2]; j<FKFC_MAX[fc2]; j++) {		/* fictieve conflicten fc2 */
   				k = TO_pointer[fc2][j];
				PVAS[k] |= BIT3;
			}

			if (fc3 >= 0) {
				for (j=0; j<GKFC_MAX[fc3]; j++) {		/* primaire conflicten fc3 */
   					k = TO_pointer[fc3][j];
					PVAS[k] |= BIT3;
				}
				for (j=GKFC_MAX[fc3]; j<FKFC_MAX[fc3]; j++) {	/* fictieve conflicten fc3 */
   					k = TO_pointer[fc3][j];
					PVAS[k] |= BIT3;
				}
			}
   		}

		if (R[fc2] && (PVAS[fc2]&BIT2)) {				/* voeding fc2 heeft prioriteit */
			if (fc3 >= 0) {
				for (j=0; j<GKFC_MAX[fc3]; j++) {		/* primaire conflicten fc3 */
   					k = TO_pointer[fc3][j];
					PVAS[k] |= BIT3;
				}
				for (j=GKFC_MAX[fc3]; j<FKFC_MAX[fc3]; j++) {	/* fictieve conflicten fc3 */
   					k = TO_pointer[fc3][j];
					PVAS[k] |= BIT3;
				}
			}
		}

		if (fc3 >= 0) {
			if (PVAS[fc3]&BIT3) {
				PVAS[fc2] |= BIT3;				/* terugschrijven naar voeding */
				PVAS[fc1] |= BIT3;
			}
		}

		if (PVAS[fc2]&BIT3) PVAS[fc1] |= BIT3;				/* terugschrijven naar voeding */
	}
    }
 }
}

/* PROCEDURE CORRIGEER TEG_OV[] VOOR SERIELE KOPPELINGEN */
/* ===================================================== */

void proc_ctegov_crsv_hki(void)				/* Fik150101 */
{
 mulv fc1,fc2,fc3,i,j,x;
 mulv kptsg12,kpteg12,kptdm12;
 mulv kptsg23,kpteg23,kptdm23;
 mulv stat;
 mulv naloop;
 bool _start;
 mulv aktsg12,aktsg23;

 for (x=0; x<3; x++) {					/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptsg12 = hki[i][3];
	kpteg12 = hki[i][4];
	kptdm12 = hki[i][5];
	fc3     = hki[i][10];
	kptsg23 = hki[i][12];
	kpteg23 = hki[i][13];
	kptdm23 = hki[i][14];
	stat    = hki[i][20];

	if (stat >= 2) {				/* koppeling is aktief */

		if (VAS4[fc1] <= VAS4[fc2]) naloop = VAS4[fc2] - VAS4[fc1];
		else                        naloop = VAS4[fc2] - VAS4[fc1] + CTYD;

		_start = FALSE;
		if (RT[kptsg12] || T[kptsg12]) {
			if (RT[kptsg12]) aktsg12 = 0;
			else             aktsg12 = T_timer[kptsg12];
			if (((T_max[kptsg12]-aktsg12)/10) >
			    ((T_max[kpteg12]+T_max[kptdm12])/10)) {
				if (((T_max[kptsg12]-aktsg12)/10) < naloop) {
					naloop = (T_max[kptsg12]-aktsg12)/10;
					_start = TRUE;
				}
			}
		}

		if (!_start) {
			if (((T_max[kpteg12]+T_max[kptdm12])/10) < naloop) {
				naloop = (T_max[kpteg12]+T_max[kptdm12])/10;
			}
		}

		if (G[fc1] && (TEG_OV[fc1] > 0)) {
			KNP_hki[i][fc2] = naloop + 1;
			if (TEG_OV[fc2] < TEG_OV[fc1] + KNP_hki[i][fc2]) TEG_OV[fc2] = TEG_OV[fc1] + KNP_hki[i][fc2];
			PVAS_hki[i][fc2] |= BIT1;
		}

		if ((!G[fc1] || (TEG_OV[fc1] == 0)) && (XG[fc2] || G[fc2]) && (PVAS_hki[i][fc2]&BIT1)) {
			if (TEG_OV[fc2] < KNP_hki[i][fc2]) TEG_OV[fc2] = KNP_hki[i][fc2];
			if (KNP_hki[i][fc2] == 0) PVAS_hki[i][fc2] &= (~BIT1);
		}

		if ((SG[fc1] && !TEG_OV[fc1]) || (!G[fc2] && !XG[fc2]) || EG[fc2]) {
			PVAS_hki[i][fc2] &= (~BIT1);
		}

		if (fc3 != NG) {

			if (VAS4[fc2] <= VAS3[fc3]) naloop = VAS4[fc3] - VAS4[fc2];
			else                        naloop = VAS4[fc3] - VAS4[fc2] + CTYD;

			_start = FALSE;
			if (RT[kptsg23] || T[kptsg23]) {
				if (RT[kptsg23]) aktsg23 = 0;
				else             aktsg23 = T_timer[kptsg23];
				if (((T_max[kptsg23]-aktsg23)/10) >
				    ((T_max[kpteg23]+T_max[kptdm23])/10)) {
					if (((T_max[kptsg23]-aktsg23)/10) < naloop) {
						naloop = (T_max[kptsg23]-aktsg23)/10;
						_start = TRUE;
					}
				}
			}

			if (!_start) {
				if (((T_max[kpteg23]+T_max[kptdm23])/10) < naloop) {
					naloop = (T_max[kpteg23]+T_max[kptdm23])/10;
				}
			}

			if ((XG[fc2] || G[fc2]) && (TEG_OV[fc2] > 0)) {
				KNP_hki[i][fc3] = naloop + 1;
				if (TEG_OV[fc3] < TEG_OV[fc2] + KNP_hki[i][fc3]) TEG_OV[fc3] = TEG_OV[fc2] + KNP_hki[i][fc3];
				PVAS_hki[i][fc3] |= BIT1;
			}

			if (((!XG[fc2] && !G[fc2]) || (TEG_OV[fc2] == 0)) && (XG[fc3] || G[fc3]) && (PVAS_hki[i][fc3]&BIT1)) {
				if (TEG_OV[fc3] < KNP_hki[i][fc3]) TEG_OV[fc3] = KNP_hki[i][fc3];
				if (KNP_hki[i][fc3] == 0) PVAS_hki[i][fc3] &= (~BIT1);
			}

			if ((SG[fc1] && !TEG_OV[fc1]) || (SG[fc2] && !TEG_OV[fc2]) || (!G[fc3] && !XG[fc3]) || EG[fc3]) {
				PVAS_hki[i][fc3] &= (~BIT1);
			}
		}

	} else {					/* koppeling is niet aktief */
		 KNP_hki[i][fc2] = 0;
		PVAS_hki[i][fc2] &= (~BIT1);
		if (fc3 >= 0) {
			 KNP_hki[i][fc3] = 0;
			PVAS_hki[i][fc3] &= (~BIT1);
		}
	}
    }

    for (j=0; j<FCMAX; j++) {
	KNP[j] = 0;					/* bepaal KNP per signaalgroep */
	PVAS[j] &= (~BIT1);				/* bepaal PVAS[]&BIT1 per signaalgroep */
    }

    for (i=0; i<aant_hki; i++) {
	for (j=0; j<FCMAX; j++) {
		if (KNP_hki[i][j] > KNP[j]) {
			KNP[j] = KNP_hki[i][j];
		}
		if (PVAS_hki[i][j]&BIT1) {
			PVAS[j] |= BIT1;
		}
	}
    }
 }
}

/* PROCEDURE BEHANDEL KLOK NA AFLOOP PRIORITEIT */
/* ============================================ */

void proc_knp_crsv_hki(void)				/* Fik150101 */
{
 mulv fc1,fc2,fc3,i,j,x;
 mulv stat;

 for (x=0; x<3; x++) {					/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	fc3     = hki[i][10];
	stat    = hki[i][20];

	if (stat >= 2) {				/* koppeling is aktief */

		if ((!G[fc1] || (TEG_OV[fc1] == 0)) && (XG[fc2] || G[fc2]) && (PVAS_hki[i][fc2]&BIT1)) {
			if (TS && (x == 0) && (KNP_hki[i][fc2] > 0)) KNP_hki[i][fc2]--;
		}
		if (EG[fc2] || !(PVAS_hki[i][fc2]&BIT1)) KNP_hki[i][fc2] = 0;

		if (fc3 != NG) {
			if (((!XG[fc2] && !G[fc2]) || (TEG_OV[fc2] == 0)) && (XG[fc3] || G[fc3]) && (PVAS_hki[i][fc3]&BIT1)) {
				if (TS && (x == 0) && (KNP_hki[i][fc3] > 0)) KNP_hki[i][fc3]--;
			}
			if (EG[fc3] || !(PVAS_hki[i][fc3]&BIT1)) KNP_hki[i][fc3] = 0;
		}
	} else {					/* koppeling is niet aktief */
		 KNP_hki[i][fc2] = 0;
		PVAS_hki[i][fc2] &= (~BIT1);
		if (fc3 >= 0) {
			 KNP_hki[i][fc3] = 0;
			PVAS_hki[i][fc3] &= (~BIT1);
		}
	}
    }

    for (j=0; j<FCMAX; j++) {
	KNP[j] = 0;					/* bepaal KNP per signaalgroep */
	PVAS[j] &= (~BIT1);				/* bepaal PVAS[]&BIT1 per signaalgroep */
    }

    for (i=0; i<aant_hki; i++) {
	for (j=0; j<FCMAX; j++) {
		if (KNP_hki[i][j] > KNP[j]) {
			KNP[j] = KNP_hki[i][j];
		}
		if (PVAS_hki[i][j]&BIT1) {
			PVAS[j] |= BIT1;
		}
	}
    }
 }
}

/* PROCEDURE CORRIGEER TTP[] EN CTTP[] VOOR FIETS.VOETGANGERSKOPPELINGEN */
/* ===================================================================== */

void proc_cttp_crsv_vtg_fts(			/* Fik150101 */
	mulv fc1,				/* Signaalgroep 1 (vtg)	*/
	mulv fc2,				/* Signaalgroep 2 (vtg)	*/
	mulv fc3,				/* Gekoppelde.fietsr. 1	*/
	mulv fc4,				/* Gekoppelde.fietsr. 2	*/
	mulv fc5,				/* Gekoppelde.fietsr. 3	*/
	mulv fc6,				/* Gekoppelde.fietsr. 4	*/
	mulv fc7)                               /* Signaalgroep 3e voetganger */
{
 mulv _maxTTP = 0;
 mulv _maxCTTP= 0;
 mulv i,_fc[7];
 bool _RR2 = FALSE;

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;
 _fc[3]=fc4; _fc[4]=fc5; _fc[5]=fc6; _fc[6]=fc7;

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if ( TTP[_fc[i]] > _maxTTP)  _maxTTP =  TTP[_fc[i]];
     if (CTTP[_fc[i]] > _maxCTTP) _maxCTTP= CTTP[_fc[i]];
     if (RR[_fc[i]]&BIT2) _RR2 = TRUE;
   }
 }
 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
	TTP[_fc[i]] = _maxTTP;
	if (TTP[_fc[i]] > UTTP[_fc[i]]) TTP[_fc[i]] = UTTP[_fc[i]];
	CTTP[_fc[i]] = _maxCTTP;
	if (_RR2 && (!XG[_fc[i]] || !RA[_fc[i]]) && !(YM[_fc[i]]&BIT2)) {
		RR[_fc[i]] |= BIT2;
		BAR[_fc[i]] = TRUE;
	}
   }
 }
}

/* PROCEDURE CORRIGEER TTP[] VOOR SERIELE KOPPELING */
/* ================================================ */

void proc_cttp_crsv_hki(void)			/* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,x;
 mulv kptvs12;
 mulv kptvs23;
 mulv voorstart;

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptvs12 = hki[i][2];
	fc3     = hki[i][10];
	kptvs23 = hki[i][11];
	stat    = hki[i][20];

	if (stat >= 2) {			/* koppeling is aktief */

		if (fc3 >= 0) {
			voorstart = T_max[kptvs23]/10;
			if (!G[fc2] && !G[fc3] && (RR[fc3]&BIT2)) {
				BAR[fc2] |= !G[fc2];
				if ((TTP[fc3] > 0) && (TTP[fc2] < 255) && (TTP[fc3] < 255)) {
					if (TTP[fc2] + voorstart < TTP[fc3]) {
						TTP[fc2] = TTP[fc3] - voorstart;
						if (UTTP[fc2] < TTP[fc2]) TTP[fc2] = UTTP[fc2];
					}
				}
				if (TTP[fc2] > 0) RR[fc2] |= BIT2;
			}
		}

		if (!G[fc1] && !G[fc2] && (RR[fc2]&BIT2)) {
			voorstart = T_max[kptvs12]/10;
			BAR[fc1] |= !G[fc1];
			if ((TTP[fc2] > 0) && (TTP[fc1] < 255) && (TTP[fc2] < 255)) {
				if (TTP[fc1] + voorstart < TTP[fc2]) {
					TTP[fc1] = TTP[fc2] - voorstart;
					if (UTTP[fc1] < TTP[fc1]) TTP[fc1] = UTTP[fc1];
				}
			}
			if (TTP[fc1] > 0) RR[fc1] |= BIT2;
		}
	}
    }
 }
}

/* PROCEDURE BEPAAL PRIMAIRE REALISATIE */
/* ==================================== */

void proc_prim_crsv(void)                    /* Fik150101 */
{
 mulv i,j,k;
 mulv ontruim;
 mulv o_tijd,gltijd;
 bool  ppv;

 if (AKTPRG) {

   for (i=0; i<FCMAX; i++) {
	ppv = FALSE;

	/* Bepaal overslag bij start VAS-programma */
	/* --------------------------------------- */
	if (SVAS) {
	  if (((HVAS[i]&BIT0) || (TTP[i]<=TSV[i])) && !G[i]) {
	    		HVAS[i] |= BIT3;
	    		 TTP[i]  = UTTP[i] = 255;
	       		   X[i] |= BIT1;		/* Registratie overslag */
	  }
	  if (!(HVAS[i]&BIT3)) {
	    for (j=0; j<GKFC_MAX[i]; j++) {
	      k = TO_pointer[i][j];
							/* Conflict is groen */
	      if (G[k] || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))) {

		/* Bepaal overslag door alternatief conflict */
		/* ----------------------------------------- */
		gltijd = 0;
		o_tijd = 0;
		if (TO_max[k][i] >= 0) {
		  gltijd = TGL_max[k]/10;
		  o_tijd = TO_max[k][i]/10;
		}

	        ontruim = gltijd + o_tijd + TEG[k];
		if (TEG_OV[k] > TEG[k]) ontruim = gltijd + o_tijd + TEG_OV[k];
		if ((XG[k]&BIT7) && (TTR[k] + ALTM[k] > TEG[k]) && (TTR[k] + ALTM[k] > TEG_OV[k])) ontruim = gltijd + o_tijd + TTR[k] + ALTM[k];

	        if (TTP[i] <= ontruim + 1) {
		  HVAS[i] |= BIT3;
		   TTP[i]  = UTTP[i] = 255;
		     X[i] |= BIT1;	/* Registratie overslag */
	        }
	      }
	    }
	  }
	}

	/* Bepaal primair gerealiseerd */
	/* --------------------------- */

	if ((HVAS[i]&BIT0) && !(HVAS[i]&BIT3) && R[i] && !TRG[i]) {
		if (HVAS[i]&BIT5) { VA[i] |= BIT2; }
		if (VA[i]) 	  {  B[i] |= BIT1; }
		else { if (!(FCA[i]&BIT3)) HVAS[i] |= BIT3; }
		if (RR[i]&BIT2) B[i] &= (~BIT1);
	}
	if ((HVAS[i]&BIT0) && (MG[i] && !RW[i] && !(HVAS[i]&BIT2) || EG[i] && !(PVS2[i]&BIT4)))
	     HVAS[i] |= BIT3;
	if  (HVAS[i]&BIT3) {
		TTP[i] = UTTP[i] = 255;
		PVS2[i] &= (~BIT4);
	}

	/* Bepaal afkappen/blokkeren conflicterende groenrealisaties */
	/* --------------------------------------------------------- */

	if (!(HVAS[i]&BIT3)) {
	  ppv = TRUE;
	  for (j=0; j<GKFC_MAX[i]; j++) {
	    k = TO_pointer[i][j];

	    gltijd = 0;
	    o_tijd = 0;
	    if (TO_max[k][i] >= 0) {
	      gltijd = TGL_max[k]/10;
	      o_tijd = TO_max[k][i]/10;
	    }
	    ontruim = gltijd + o_tijd + TEG[k];

					/* Conflict is groen */

	    if (G[k] || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))) {

	      /* Bepaal overslag door alternatief conflict */
	      /* ----------------------------------------- */
                                                        /* @#@ Fik150201 */
	    /*if ((TTP[i]<=ontruim) &&  (TTP[k]>TTP[i])                                                                 && R[i]*/
	      if ((TTP[i]<=ontruim) && ((TTP[k]>TTP[i]) || HKV[k] && (G[k] || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7)))) && R[i]
				    && !TRG[i] && !(HVAS[i]&BIT5) 
				    && !FCA[i] && ((ETRG[i] != 1) || !(RR[i]&BIT2))
				    && (!VA[i] || (VA[i]&BIT7))) {
		HVAS[i] |= BIT3;
		 TTP[i]  = UTTP[i] = 255;
		   X[i] |= BIT1;	/* Registratie overslag */
	      }
	      
	      if (!(HVAS[i]&BIT3)) {

	        ontruim = gltijd + o_tijd;

		/* Bepaal tijd tot afkappen door primair conflict */
		/* ---------------------------------------------- */

		if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {
		  if (TTP[i]<=ontruim)		    { TTK[k] = FALSE; }
		  else if ((TTP[i]-ontruim)<TTK[k]) { TTK[k] = TTP[i]-ontruim;}
		}

	        if ((TTP[i] <= ontruim)
		   && (!(RR[i]&BIT2) || (UTTP[i] <= ontruim))
		   && ((TTP[k]>TTP[i]) ||  (TTP[k]==0))) {

		  if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {
		    if (!(YM[i]&BIT2)) BAR[k] |= TRUE;
		    if (G[k])		 Z[k] |= BIT1;
		  }
	        } else { ppv = FALSE; }

	        if (!((VTTP[i] <= ontruim)
		   && (!(RR[i]&BIT2) || (UTTP[i] <= ontruim))
		   && ((TTP[k]>VTTP[i]) ||  (TTP[k]==0)))) ppv = FALSE;

		if (FCA[k]&BIT0) {     /* Fic.aanv. gekoppelde.vtg. aanwezig */
				       /* danwel voedende fietser aanwezig   */

		  ontruim = gltijd + o_tijd + TTR[k] + ALTM[k];

	          if ((TTP[i] <= ontruim)
		     && (!(RR[i]&BIT2) || (UTTP[i] <= ontruim))
		     && ((TTP[k]>TTP[i]) ||  (TTP[k]==0))) {

		    if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {
		      if (!(YM[i]&BIT2)) BAR[k] |= TRUE;
		    }
	          } else { ppv = FALSE; }

	          if (!((VTTP[i] <= ontruim)
		     && (!(RR[i]&BIT2) || (UTTP[i] <= ontruim))
		     && ((TTP[k]>VTTP[i]) ||  (TTP[k]==0)))) ppv = FALSE;

		}
	      
	      }
	    } else {			/* Conflict is NIET groen */
	      
	      ontruim = gltijd + o_tijd + ALTM[k];

	      if ((TTP[i] <= ontruim + TTR[k]) 
		  && (!(RR[i]&BIT2) || (UTTP[i] <= ontruim + TTR[k]))
		  && (TTP[k]>TTP[i])) {

		if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {
		  if (!(YM[i]&BIT2)) BAR[k] |= TRUE;
		}
	      } else { ppv = FALSE; }

	      if (!((VTTP[i] <= ontruim + TTR[k]) 
		  && (!(RR[i]&BIT2) || (UTTP[i] <= ontruim + TTR[k]))
		  && (TTP[k]>VTTP[i]))) ppv = FALSE;

	      /* Bepaal tijd tot afkappen door primair conflict */
	      /* ---------------------------------------------- */

	      if (!(HVAS[i]&BIT3)) {

	        ontruim = gltijd + o_tijd;

		/* Bepaal tijd tot afkappen door primair conflict */
		/* ---------------------------------------------- */

		if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {
		  if (TTP[i]<=ontruim)		    { TTK[k] = FALSE; }
		  else if ((TTP[i]-ontruim)<TTK[k]) { TTK[k] = TTP[i]-ontruim;}
		}
	      }
	    }
	  }
	  for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
	    k = TO_pointer[i][j];
	    if (TTP[i] >TTP[k]) ppv = FALSE;
	    if (VTTP[i]>TTP[k]) ppv = FALSE;
	  }
	}

	/* Bepaal versnelde primaire realisatie */
	/* ------------------------------------ */

	if (RR[i]&BIT2) { ppv = FALSE; VA[i] &= (~BIT4); }

	if ((ppv || (VA[i]&BIT4)) && !(HVAS[i]&BIT0) && !(HVAS[i]&BIT3) && !(HVAS[i]&BIT1) && !(HVAS[i]&BIT6)) {
		if (G[i]) {
			   HVAS[i] |=  BIT4;
			     RW[i] |=  BIT1;
			     VA[i] &=(~BIT4);
			  }
		else { if (RA[i] && !BL[i]) VA[i] |= BIT4;
		     }
	} else { VA[i] &= (~BIT4);	/* Resetten indien alsnog RR[i] */
	       }

	/* Bepaal blokkeer overgang naar groen */
	/* ----------------------------------- */

	if ((HVAS[i]&BIT1) && !G[i]) {             /* Fik150315 toegevoegd || (HVAS[i]&BIT3) */
		if ((TTR[i] < TTU[i]) && !RA[i] || (HVAS[i]&BIT3)) {
				      BAR[i]  = TRUE;
		}
		if (!(B[i]&BIT4) || !RA[i]) X[i] |= BIT2;
	}

	/* Bepaal forceer overgang naar rood */
	/* --------------------------------- */

	if (HVAS[i]&BIT6) {
		if (!G[i] && !ppv)   {
			if ((TTR[i] < TTU[i]) && !RA[i]) {
				      BAR[i]  = TRUE;
			}
			if (!(B[i]&BIT4) || !RA[i]) X[i] |= BIT2;
		}
		if (G[i] && (TEG_OV[i] == 0) && ((TTPR[i] != 0) || !(PVS2[i]&BIT1))) {
			Z[i] |= BIT2;
		}
	}

	/* Bepaal vasthouden in VAG2e */
	/* -------------------------- */

	if ((HVAS[i]&BIT2) && G[i]) RW[i] |= BIT1;

	/* Reset realisatie-geheugen op einde primaire gebied */
	/* -------------------------------------------------- */

	if ((STAP==VAS4[i]) && (TS || N_ts)) {
				     HVAS[i] &= (~BIT3);
				     HVAS[i] &= (~BIT4); }
   }
 }
 for (i=0; i<aant_fts; i++) {
   proc_cprm_crsv_vtg_fts(fts[i][0],NG,fts[i][1],NG,fts[i][2],NG,NG);
 }
 for (i=0; i<aant_bbv; i++) {
   proc_cprm_crsv_vtg_fts(bbv[i][0],bbv[i][1],bbv[i][4],NG,bbv[i][5],NG,NG);
 }
 for (i=0; i<aant_gov; i++) {
   proc_cprm_crsv_vtg_fts(gov[i][0],gov[i][1],gov[i][7]
			 ,gov[i][8],gov[i][13],gov[i][14],gov[i][22]);
 }
}

/* PROCEDURE CORRIGEER OVERSLAG VOOR FTS.VTG.KOPPELINGEN */
/* ===================================================== */

void proc_covs_crsv_vtg_fts(                 /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2)                            /* Signaalgroep 2 */
{
 if (fc1 >= 0) {
   if ((X[fc1]&BIT1) || G[fc1] && (RW[fc1]&BIT1)) {
     if (fc2 >= 0) {
       if ((X[fc1]&BIT1) || !G[fc2]) {
	 if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
		HVAS[fc2] |= BIT3;
		 TTP[fc2]  = UTTP[fc2] = 255;
	 }
       }
     }
   }
 }
}

/* PROCEDURE CORRIGEER PRIMAIRE REALISATIE VOOR FTS.VTG.KOPPELINGEN */
/* ================================================================ */

void proc_cprm_crsv_vtg_fts(                 /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv fc3,                            /* Signaalgroep 3 */
	mulv fc4,                            /* Signaalgroep 4 */
	mulv fc5,                            /* Signaalgroep 5 */
	mulv fc6,                            /* Signaalgroep 6 */
	mulv fc7)                            /* Signaalgroep 7 */
{
 mulv i,j,_fc[7];

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;
 _fc[3]=fc4; _fc[4]=fc5; _fc[5]=fc6; _fc[6]=fc7;

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if (G[_fc[i]] && (HVAS[_fc[i]]&BIT4)) {
	for (j=0; j<7; j++) {
	  if (i != j) {
	    if (_fc[j] >= 0) {
	      if (G[_fc[j]] && !(HVAS[_fc[j]]&BIT0) && !(HVAS[_fc[j]]&BIT3)
						    && !(HVAS[_fc[j]]&BIT4)) {
		HVAS[_fc[j]] |=  BIT4;
		  RW[_fc[j]] |=  BIT1;
		  VA[_fc[j]] &=(~BIT4);
	      }
	    }
	  }
	}
     }
   }
 }
}

/* PROCEDURE BEPAAL TOESTEMMING ALTERNATIEVE REALISATIE DOOR CRSV-MODULE */
/* ===================================================================== */

void proc_altt_crsv(void)                /* Fik150101 */
{
 mulv i,j,k;

 for (i=0; i<FCMAX; i++) {

  if (!BAR[i] && R[i] && !TRG[i]) {

    for (j=0; j<GKFC_MAX[i]; j++) {      /* Primaire conflicten */
      k = TO_pointer[i][j];
      if (B[k] ||  RA[k]
	       ||  XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))
	       ||   G[k] && !(YM[k]&BIT2) && !MG[k]
	       ||  MG[k] && !(YM[k]&BIT2) && RW[k]) BAR[i] = TRUE;
    }

    for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) { /* Fictieve conflicten */
      k = TO_pointer[i][j];
      if (B[k] || RA[k]
	       || XG[k] && ((XG[k]&BIT2) || (XG[k]&BIT7) || (TTP[k] <= TTP[i]))) BAR[i] = TRUE;
    }

  } else { if (!G[i]) BAR[i] = TRUE; }
 }
 for (i=0; i<aant_fts; i++) {
   proc_cbar_crsv_vtg_fts(fts[i][0],NG,fts[i][1],NG,fts[i][2],NG,NG);
 }
 for (i=0; i<aant_bbv; i++) {
   proc_cbar_crsv_vtg_fts(bbv[i][0],bbv[i][1],bbv[i][4],NG,bbv[i][5],NG,NG);
 }
 for (i=0; i<aant_gov; i++) {
   proc_cbar_crsv_vtg_fts(gov[i][0],gov[i][1],gov[i][7]
			 ,gov[i][8],gov[i][13],gov[i][14],gov[i][22]);
 }
 proc_cbar_crsv_hki();
}

/* PROCEDURE BEPAAL TOESTEMMING ALTERNATIEVE REALISATIE DOOR APPLICATIE */
/* ==================================================================== */

void proc_altt_appl(                         /* Fik150101 */
	mulv fc,                             /* Signaalgroep    */
	mulv hfbar)                          /* Hulpfunctie BAR */
{
 mulv i;

 if (IH[hfbar]) BAR[fc] = TRUE;

 for (i=0; i<aant_fts; i++) {
   proc_cbar_crsv_vtg_fts(fts[i][0],NG,fts[i][1],NG,fts[i][2],NG,NG);
 }
 for (i=0; i<aant_bbv; i++) {
   proc_cbar_crsv_vtg_fts(bbv[i][0],bbv[i][1],bbv[i][4],NG,bbv[i][5],NG,NG);
 }
 for (i=0; i<aant_gov; i++) {
   proc_cbar_crsv_vtg_fts(gov[i][0],gov[i][1],gov[i][7]
			 ,gov[i][8],gov[i][13],gov[i][14],gov[i][22]);
 }
 proc_cbar_crsv_hki();
}

/* PROCEDURE CORRIGEER BAR[] VOOR FIETS.VOETGANGERSKOPPELINGEN */
/* =========================================================== */

void proc_cbar_crsv_vtg_fts(                 /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 (vtg) */
	mulv fc2,                            /* Signaalgroep 2 (vtg) */
	mulv fc3,                            /* Gekoppelde.fietsr. 1 */
	mulv fc4,                            /* Gekoppelde.fietsr. 2 */
	mulv fc5,                            /* Gekoppelde.fietsr. 3 */
	mulv fc6,                            /* Gekoppelde.fietsr. 4 */
	mulv fc7)                            /* Signaalgroep 3e voetganger */
{
 bool _BAR   = FALSE;
 bool _BAR_a = FALSE;

 mulv i,j,_fc[7];

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;
 _fc[3]=fc4; _fc[4]=fc5; _fc[5]=fc6; _fc[6]=fc7;

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if (BAR[_fc[i]]) _BAR = TRUE;
     if (GL[_fc[i]] || TRG[_fc[i]]
	 || BAR[_fc[i]] && R[_fc[i]] && (VA[_fc[i]] || WTV_FC[_fc[i]])) _BAR_a=TRUE;
   }
 }

 if (_BAR) {
	if (fc1 >= 0) {
		BAR[fc1] = TRUE;
		if (R[fc1] && (VA[fc1] || WTV_FC[fc1])) _BAR_a=TRUE;
	}
	if (fc2 >= 0) {
		BAR[fc2] = TRUE;
		if (R[fc2] && (VA[fc2] || WTV_FC[fc2])) _BAR_a=TRUE;
	}
	if (fc7 >= 0) {
		BAR[fc7] = TRUE;
		if (R[fc7] && (VA[fc7] || WTV_FC[fc7])) _BAR_a=TRUE;
	}
 }

 if ((fc3 >= 0) && (fc4 >= 0)) {
	if (BAR[fc3] || BAR[fc4]) {
		BAR[fc3] = BAR[fc4] = TRUE;
		if (R[fc3] && (VA[fc3] || WTV_FC[fc3])) _BAR_a=TRUE;
		if (R[fc4] && (VA[fc4] || WTV_FC[fc4])) _BAR_a=TRUE;
	}
 }

 if ((fc5 >= 0) && (fc6 >= 0)) {
	if (BAR[fc5] || BAR[fc6]) {
		BAR[fc5] = BAR[fc6] = TRUE;
		if (R[fc5] && (VA[fc5] || WTV_FC[fc5])) _BAR_a=TRUE;
		if (R[fc6] && (VA[fc6] || WTV_FC[fc6])) _BAR_a=TRUE;
	}
 }

 if (_BAR_a) {
    for (i=0; i<7; i++) {
	if (_fc[i] >= 0) BAR[_fc[i]] = TRUE;
    }
 }

 /* Correctie BAR i.v.m. aanhouden mvg gekoppelde richtingen */
 /* -------------------------------------------------------- */

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if (!G[_fc[i]] && !BAR[_fc[i]]) {
	for (j=0; j<7; j++) {
	  if (i != j) {
	    if (_fc[j] >= 0) {
	      if (G[_fc[j]] && (TTR[_fc[i]]>=TTK[_fc[j]])) BAR[_fc[i]] = TRUE;
	    }
	  }
	}
     }
   }
 }
}

/* PROCEDURE BEPAAL HULPWAARDEN VOOR INRIJDEN ONDER GROEN CONFLICTEN VOLGRICHTING(EN) */
/* ---------------------------------------------------------------------------------- */

void proc_TTK_hki_crsv(void)                 /* Fik150101 */
{
 mulv i,j,k,x;
 mulv ontruim_ik,ontruim_ki;
 mulv o_tijd_ki,gltijd_k;
 mulv o_tijd_ik,gltijd_i;
 mulv _hulp_teg;
 mulv fc1,fc2,fc3,stat;
 mulv kptvs12,kptvs23;
 mulv kopvs12,kopvs23;

   for (i=0; i<FCMAX; i++) {
	_XG[i] = FALSE;
	if (!XG[i] || G[i]) {
		TTR_HK[i] = 255;
		_TTR_HK[i] = FALSE;
	}
	TTR_HK_oud[i] = TTR_HK[i];
   }

   if (AKTPRG) {

	for (x=0; x<3; x++) {			/* 3x doorlopen ivm mogelijke geneste koppelingen */
		for (i=0; i<aant_hki; i++) {
			fc1     = hki[i][0];
			fc2     = hki[i][1];
			kptvs12 = hki[i][2];
			fc3     = hki[i][10];
			kptvs23 = hki[i][11];
			stat    = hki[i][20];
			if (stat >= 2) {

				kopvs12 = T_max[kptvs12]/10;
				if (fc3 >= 0) {
					kopvs23 = T_max[kptvs23]/10;
				} else {
					kopvs23 = 0;
				}

				if (XG[fc1]) _XG[fc2] = TRUE;
				if (fc3 >= 0) {
					if (XG[fc2]) _XG[fc3] = TRUE;
				}
						/* bepaal TTR_HK[] */
				if (XG[fc1]) {
					if ((TTR[fc1] + kopvs12) < TTR_HK[fc2]) TTR_HK[fc2] = TTR[fc1] + kopvs12;
					if (fc3 >= 0) {
						if ((TTR_HK[fc2] + kopvs23) < TTR_HK[fc3]) TTR_HK[fc3] = TTR_HK[fc2] + kopvs23;
					}
				}

				if (G[fc1]) {
					if (KG[fc1] >= kopvs12) TTR_HK[fc2] = 0;
					else {
						if ((kopvs12 - KG[fc1]) < TTR_HK[fc2]) TTR_HK[fc2] = kopvs12 - KG[fc1];
					}
					if (fc3 >= 0) {
						if (KG[fc1] >= kopvs12 + kopvs23) TTR_HK[fc3] = 0;
						else {
							if ((kopvs12 + kopvs23 - KG[fc1]) < TTR_HK[fc3]) TTR_HK[fc3] = kopvs12 + kopvs23 - KG[fc1];
						}
					}
				}

				if (EG[fc1]) {
					_TTR_HK[fc2] = TRUE;
					if (fc3 >= 0) _TTR_HK[fc3] = TRUE;
				}

			}
		}
	}

	for (i=0; i<FCMAX; i++) {
		if (TTR_HK[i] < TTR_HK_oud[i]) _TTR_HK[i] = 0;			/* idg is er kennelijk een andere voeding aktief (G of XG) en maatgevend */
		/* if ((TS || N_ts) && _TTR_HK[i] && (TTR_HK[i] > 0)) TTR_HK[i]--; aftellen _TTR_HK[] verplaatst naar proc_reset_crsv_einde() */
										/* ... want mag maar 1x per machineslag worden doorlopen !!!  */

		TTK_hki[i] = 255;	/* eerstvolgend conflict kapt fc i af na 255 seconden */
		TEG_hki[i] = 0;		/* lopend conflict zorgt dat fc i kan starten na 0 seconden */

		TSPR[i] = 255;
		TEPR[i] = 0;

		if (G[i] && !SG[i] && !FG[i] && (!WG[i] || !(VA[i]&BIT5)) && (KG[i] >= MAXG[i])) {

			for (j=0; j<GKFC_MAX[i]; j++) {
				k = TO_pointer[i][j];

				gltijd_k  = 0;
				o_tijd_ki = 0;

				gltijd_i  = 0;
				o_tijd_ik = 0;

				if ((TO_max[k][i] >= 0) && (TO_max[i][k] >= 0)) {
					gltijd_k  = TGL_max[k]/10;
					o_tijd_ki = TO_max[k][i]/10;

					gltijd_i  = TGL_max[i]/10;
					o_tijd_ik = TO_max[i][k]/10;
				}

				if (XG[k] || _XG[k]) {
					if (TTR_HK[k] <= (gltijd_i + o_tijd_ik)) {
						Z[i] |= BIT7;
						/* YV[i] &= (~BIT2); */
						/* MK[i] &= (~BIT0); */
						/* MK[i] &= (~BIT1); */
						/* MK[i] &= (~BIT2); */
					}
					if (_XG[k]) BHK[i] = TRUE;
				}
			}
		}

		if ((VAS2[i] != 0) && (VAS4[i] != 0) && (VAS2[i] != VAS4[i])) {

			if (TTP[i] == 255) {
				if (STAP <= VAS2[i]) TSPR[i] = VAS2[i] - STAP;
				else                 TSPR[i] = CTYD    - STAP + VAS2[i];

				/* Fik130216 -> als richting vooruit is gerealiseerd dan TSPR[i] corrigeren */
				if (TSPR[i] < CTYD - MAXG[i]) TSPR[i] = CTYD + TSPR[i];

			} else {
				TSPR[i] = TTP[i];
			}

			if (HVAS[i]&BIT0) {
				if (STAP <= VAS4[i]) TEPR[i] = VAS4[i] - STAP;
				else                 TEPR[i] = CTYD    - STAP + VAS4[i];
			}
		}
	}

	for (i=0; i<FCMAX; i++) {

		for (j=0; j<GKFC_MAX[i]; j++) {
			k = TO_pointer[i][j];

			gltijd_k  = 0;
			o_tijd_ki = 0;

			gltijd_i  = 0;
			o_tijd_ik = 0;

			if ((TO_max[k][i] >= 0) && (TO_max[i][k] >= 0)) {
				gltijd_k  = TGL_max[k]/10;
				o_tijd_ki = TO_max[k][i]/10;

				gltijd_i  = TGL_max[i]/10;
				o_tijd_ik = TO_max[i][k]/10;
			}

        		ontruim_ki = gltijd_k + o_tijd_ki;
        		ontruim_ik = gltijd_i + o_tijd_ik;

			if (G[i] || GL[i] || TRG[i] || VA[i] || (HVAS[i]&BIT5) || FCA[i]) {

				if (TTP[i] > 0) {

					if (TSPR[i] <= ontruim_ki)		  { TTK_hki[k] = 0; }
					else if ((TSPR[i]-ontruim_ki)<TTK_hki[k]) { TTK_hki[k] = TSPR[i]-ontruim_ki; }

					if ((KG[i] > MAXG[i]) || MG[i] && !RW[i]) _hulp_teg = 0; /* Fik130302 */
					else                                      _hulp_teg = MAXG[i] - KG[i];

					if (G[i] && (TEG_hki[k] < (   TEG[i]+ontruim_ik))) TEG_hki[k] =    TEG[i]+ontruim_ik;
					if (G[i] && (TEG_hki[k] < (_hulp_teg+ontruim_ik))) TEG_hki[k] = _hulp_teg+ontruim_ik;

				} else {

					if (!G[i]) TTK_hki[k] = 0;

					if (G[i] && (TEG_hki[k] < ( TEG[i]+ontruim_ik))) TEG_hki[k] =  TEG[i]+ontruim_ik;
					if (G[i] && (TEG_hki[k] < (TEPR[i]+ontruim_ik))) TEG_hki[k] = TEPR[i]+ontruim_ik;
				}

				if ((TTPR[i] > 0) && (TTPR[i] < 255)) {

					if (TTPR[i]<=ontruim_ki)		  { TTK_hki[k] = 0; }
					else if ((TTPR[i]-ontruim_ki)<TTK_hki[k]) { TTK_hki[k] = TTPR[i]-ontruim_ki; }

					if (G[i] && (TEG_hki[k] < (TEG_OV[i]+ontruim_ik))) TEG_hki[k] = TEG_OV[i]+ontruim_ik;

				} else {
					if (!G[i] && (TTPR[i] == 0)) TTK_hki[k] = FALSE;
					if ( G[i] && (TEG_hki[k] < (TEG_OV[i]+ontruim_ik))) TEG_hki[k] = TEG_OV[i]+ontruim_ik;
				}
			}
		}
	}
    }
}   

/* PROCEDURE CORRIGEER BAR[] VOOR SERIELE KOPPELINGEN */
/* ================================================== */

void proc_cbar_crsv_hki(void)                   /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,x;
 mulv kptvs12,kptsg12,kpteg12,kptdm12;
 mulv kptvs23,kptsg23,kpteg23,kptdm23;

 mulv kopvs12,kopsg12,kopeg12,kopdm12;
 mulv kopvs23,kopsg23,kopeg23,kopdm23;

 mulv tijd_tot_sg2,tijd_tot_eg2;

 proc_TTK_hki_crsv();				/* bepaal hulpwaarden TTK_hki[] en TEG_hki[] */

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptvs12 = hki[i][2];
	kptsg12 = hki[i][3];
	kpteg12 = hki[i][4];
	kptdm12 = hki[i][5];
	fc3     = hki[i][10];
	kptvs23 = hki[i][11];
	kptsg23 = hki[i][12];
	kpteg23 = hki[i][13];
	kptdm23 = hki[i][14];
	stat    = hki[i][20];

	kopvs12 = T_max[kptvs12]/10;
	kopsg12 = T_max[kptsg12]/10;
	kopeg12 = T_max[kpteg12]/10;
	kopdm12 = T_max[kptdm12]/10;

	if (fc3 >= 0) {
		kopvs23 = T_max[kptvs23]/10;
		kopsg23 = T_max[kptsg23]/10;
		kopeg23 = T_max[kpteg23]/10;
		kopdm23 = T_max[kptdm23]/10;
	} else {
		kopvs23 = 0;
		kopsg23 = 0;
		kopeg23 = 0;
		kopdm23 = 0;
	}

	if (stat >= 2) {			/* koppeling is aktief */

		if (!G[fc1]) {
			if (ALTM[fc1] + kopeg12 > kopsg12) {
				tijd_tot_eg2 = TTR[fc1]+ALTM[fc1]+kopeg12+kopdm12;
			} else {
				tijd_tot_eg2 = TTR[fc1]+kopsg12+kopdm12;
			}
			if ((TTR[fc2] + ALTM[fc2]) > tijd_tot_eg2) tijd_tot_eg2 = TTR[fc2] + ALTM[fc2];
			tijd_tot_sg2 = tijd_tot_eg2 - ALTM[fc1];

			if (G[fc2]) {
				if (ALTM[fc1] + kopeg12 > kopsg12) {
					if ((TTR[fc1]+ALTM[fc1]+kopeg12+kopdm12) >= TTK[fc2]) BAR[fc1] |= TRUE;
				} else {
					if ((TTR[fc1]          +kopsg12+kopdm12) >= TTK[fc2]) BAR[fc1] |= TRUE;
				}
			} else {
				if (TTP[fc2]>kopvs12) {
					/* if (BAR[fc2] || somfbk_crsv(fc2)) BAR[fc1] |= TRUE; */
                                        /* && somfbk_crsv(fc2) toegevoegd Fik130408 */
					if (somfbk_crsv(fc2) && ((TTR[fc2] > kopvs12) || (TEG_hki[fc2] > kopvs12))) BAR[fc1] |= TRUE;
					if (ALTM[fc1] + kopeg12 > kopsg12) {
						if ((TTR[fc1]+ALTM[fc1]+kopeg12+kopdm12) >= TTK_hki[fc2]) BAR[fc1] |= TRUE;
					} else {
						if ((TTR[fc1]          +kopsg12+kopdm12) >= TTK_hki[fc2]) BAR[fc1] |= TRUE;
					}
				 }
			}

			if (fc3 >= 0) {

				if (G[fc3]) {
					if (ALTM[fc2] + kopeg23 > kopsg23) {
						if ((tijd_tot_eg2+kopeg23+kopdm23) >= TTK[fc3]) BAR[fc1] |= TRUE;
					} else {
						if ((tijd_tot_sg2+kopsg23+kopdm23) >= TTK[fc3]) BAR[fc1] |= TRUE;
					}

				} else {
					if (TTP[fc3] > (tijd_tot_sg2 + kopvs23)) {
						if ((TTR[fc3] > (tijd_tot_sg2 + kopvs23)) || (TEG_hki[fc2] > (tijd_tot_sg2 + kopvs23))) BAR[fc1] |= TRUE;
						if (ALTM[fc2] + kopeg23 > kopsg23) {
							if ((tijd_tot_eg2+kopeg23+kopdm23) >= TTK_hki[fc3]) BAR[fc1] |= TRUE;
						} else {
							if ((tijd_tot_sg2+kopsg23+kopdm23) >= TTK_hki[fc3]) BAR[fc1] |= TRUE;
						}
				 	}
				}
			}

		} else {			/* Voedende richting is groen     */
			if (!G[fc2]) {		/* ... maar volgrichting nog niet */
				/* if ((TEG[fc2] >= TTK[fc2]) && (TTP[fc2] > TTK[fc2])) BAR[fc1] |= TRUE; Fik110806 */
				if ((TEG[fc2] >= TTK[fc2]) && (TTP[fc2] > TTK[fc2]) && (TTK[fc2] > 0)) BAR[fc1] |= TRUE;
			}
			if (fc3 >= 0) {
				if (!G[fc3]) {
					/* if ((TEG[fc3] >= TTK[fc3]) && (TTP[fc3] > TTK[fc3])) BAR[fc1] |= TRUE; Fik110806 */
					if ((TEG[fc3] >= TTK[fc3]) && (TTP[fc3] > TTK[fc3]) && (TTK[fc3] > 0)) BAR[fc1] |= TRUE;
				}
			}
		}

		if (fc3 >= 0) {
			if (!G[fc2] && !XG[fc2]) {
				if (G[fc3]) {
					if (ALTM[fc2] + kopeg23 > kopsg23) {
						if ((TTR[fc2]+ALTM[fc2]+kopeg23+kopdm23) >= TTK[fc3]) BAR[fc2] |= TRUE;
					} else {
						if ((TTR[fc2]          +kopsg23+kopdm23) >= TTK[fc3]) BAR[fc2] |= TRUE;
					}
				} else {
					if (TTP[fc3]>kopvs23) {
						/* if (BAR[fc3] || somfbk_crsv(fc3)) BAR[fc2] |= TRUE; */
                                                /* Fik130408 && somfbk_crsv(fc3) toegevoegd */
						if (somfbk_crsv(fc3) && ((TTR[fc3] > kopvs23) || (TEG_hki[fc3] > kopvs23))) BAR[fc2] |= TRUE;
						if (ALTM[fc2] + kopeg23 > kopsg23) {
							if ((TTR[fc2]+ALTM[fc2]+kopeg23+kopdm23) >= TTK_hki[fc3]) BAR[fc2] |= TRUE;
						} else {
							if ((TTR[fc2]          +kopsg23+kopdm23) >= TTK_hki[fc3]) BAR[fc2] |= TRUE;
						}
				 	}
				}
			} else {		/* Voedende richting is groen     */
				if (!G[fc3]) {	/* ... maar volgrichting nog niet */
					if ((TEG[fc3] >= TTK[fc3]) && (TTP[fc3] > TTK[fc3]) && (TTK[fc3] > 0) && BW_hki[i][fc2]) BAR[fc2] |= TRUE;
				}
			}
		}
	}
    }
 }
}

/* PROCEDURE BEPAAL ALTERNATIEVE REALISATIE */
/* ======================================== */

void proc_altr_crsv(void)			/* Fik150101 */
{
 mulv i,j,k,x,y;
 mulv fc1,fc2,fc3,stat;
 bool  altr,bewaak;

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	fc3     = hki[i][10];
	stat    = hki[i][20];

	if (stat >= 2) {			/* koppeling is aktief */

		if (B[fc1] || RA[fc1] || (PVS2[fc1]&BIT4)
			   ||  G[fc1] && !(YM[fc1]&BIT2) && !MG[fc1]
			   || MG[fc1] && !(YM[fc1]&BIT2) &&  RW[fc1]) {

			for (y=0; y<GKFC_MAX[fc2]; y++) {
				k = TO_pointer[fc2][y];
				if (!G[fc1] && !(PVS2[fc1]&BIT4) ||
				     G[fc2] || XG[fc2] && ((XG[fc2]&BIT2) || (XG[fc2]&BIT7) || (TTP[fc2] <= TTP[k]))) if (!G[k]) BAR[k] |= TRUE;
			}

			if (B[fc1] || RA[fc1] || (PVS2[fc1]&BIT4)) {

				for (y=GKFC_MAX[fc2]; y<FKFC_MAX[fc2]; y++) {
					k = TO_pointer[fc2][y];
					if (!G[k]) BAR[k] |= TRUE;
				}
			}

			if (fc3 >= 0) {

				for (y=0; y<GKFC_MAX[fc3]; y++) {
					k = TO_pointer[fc3][y];
					if (!G[fc1] && !(PVS2[fc1]&BIT4) ||
					     G[fc3] || XG[fc3] && ((XG[fc3]&BIT2) || (XG[fc3]&BIT7) || (TTP[fc3] <= TTP[k]))) if (!G[k]) BAR[k] |= TRUE;
				}

				if (B[fc1] || RA[fc1] || (PVS2[fc1]&BIT4)) {
					for (y=GKFC_MAX[fc3]; y<FKFC_MAX[fc3]; y++) {
						k = TO_pointer[fc3][y];
						if (!G[k]) BAR[k] |= TRUE;
					}
				}
			}
   		}

		if (fc3 >= 0) {

			if (B[fc2] || RA[fc2] || (PVS2[fc2]&BIT4)
				   ||  G[fc2] && !(YM[fc2]&BIT2) && !MG[fc2]
				   || MG[fc2] && !(YM[fc2]&BIT2) &&  RW[fc2]) {

				for (y=0; y<GKFC_MAX[fc3]; y++) {
					k = TO_pointer[fc3][y];
					if (!G[fc2] && !(PVS2[fc2]&BIT4) ||
					     G[fc3] || XG[fc3] && ((XG[fc3]&BIT2) || (XG[fc3]&BIT7) || (TTP[fc3] <= TTP[k]))) if (!G[k]) BAR[k] |= TRUE;
				}

				if (B[fc2] || RA[fc2] || (PVS2[fc2]&BIT4)) {
					for (y=GKFC_MAX[fc3]; y<FKFC_MAX[fc3]; y++) {
						k = TO_pointer[fc3][y];
						if (!G[k]) BAR[k] |= TRUE;
					}
				}
	   		}
		}

		if (!BAR[fc1] && R[fc1] && !TRG[fc1] && VA[fc1]) {

			for (y=0; y<GKFC_MAX[fc2]; y++) {
				k = TO_pointer[fc2][y];
				if (B[k]   || !BAR[k]   && (TFB_timer[k]  >=TFB_timer[fc1])) BAR[fc1] |= TRUE;
				if (B[fc1] || !BAR[fc1] && (TFB_timer[fc1]> TFB_timer[k]  )) BAR[k]   |= TRUE;
			}

			for (y=GKFC_MAX[fc2]; y<FKFC_MAX[fc2]; y++) {
				k = TO_pointer[fc2][y];
				if (B[k]   || !BAR[k]   && (TFB_timer[k]  >=TFB_timer[fc1])) BAR[fc1] |= TRUE;
				if (B[fc1] || !BAR[fc1] && (TFB_timer[fc1]> TFB_timer[k]  )) BAR[k]   |= TRUE;
			}

			if (fc3 >= 0) {

				for (y=0; y<GKFC_MAX[fc3]; y++) {
					k = TO_pointer[fc3][y];
					if (B[k]   || !BAR[k]   && (TFB_timer[k]  >=TFB_timer[fc1])) BAR[fc1] |= TRUE;
					if (B[fc1] || !BAR[fc1] && (TFB_timer[fc1]> TFB_timer[k]  )) BAR[k]   |= TRUE;
				}

				for (y=GKFC_MAX[fc3]; y<FKFC_MAX[fc3]; y++) {
					k = TO_pointer[fc3][y];
					if (B[k]   || !BAR[k]   && (TFB_timer[k]  >=TFB_timer[fc1])) BAR[fc1] |= TRUE;
					if (B[fc1] || !BAR[fc1] && (TFB_timer[fc1]> TFB_timer[k]  )) BAR[k]   |= TRUE;
				}

			}
		}

		if (fc3 >= 0) {

			if (!BAR[fc2] && R[fc2] && !TRG[fc2] && VA[fc2]) {

				for (y=0; y<GKFC_MAX[fc3]; y++) {
					k = TO_pointer[fc3][y];
					if (B[k]   || !BAR[k]   && (TFB_timer[k]  >=TFB_timer[fc2])) BAR[fc2] |= TRUE;
					if (B[fc2] || !BAR[fc2] && (TFB_timer[fc2]> TFB_timer[k]  )) BAR[k]   |= TRUE;
				}

				for (y=GKFC_MAX[fc3]; y<FKFC_MAX[fc3]; y++) {
					k = TO_pointer[fc3][y];
					if (B[k]   || !BAR[k]   && (TFB_timer[k]  >=TFB_timer[fc2])) BAR[fc2] |= TRUE;
					if (B[fc2] || !BAR[fc2] && (TFB_timer[fc2]> TFB_timer[k]  )) BAR[k]   |= TRUE;
				}
			}
		}
	}
    }
 }

 for (i=0; i<FCMAX; i++) {

	if (!BAR[i] && R[i] && !TRG[i] && VA[i] && !(VA[i]&BIT7)) {

		altr = TRUE;
		for (j=0; j<GKFC_MAX[i]; j++) {            /* Primaire conflicten */
			k = TO_pointer[i][j];
			if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i])) altr = FALSE;
		}

		for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {  /* Fictieve conflicten */
			k = TO_pointer[i][j];
			if (B[k] || !BAR[k] && (TFB_timer[k]>TFB_timer[i])) altr = FALSE;
		}

		if (altr && !somfbk_crsv(i) && AKTPRG) B[i] |= BIT2;
  	}
	if (RA[i] && (BAR[i] || somfbk_crsv(i)) && !(B[i]&BIT4) && !(HVAS[i]&BIT0)) RR[i] |= BIT1;
 }

 /* Fik130408 nooit meer dan 1x alternatief per machineslag indien dit een voedende hki richting betreft */
 bewaak = FALSE;
 for (i=0; i<FCMAX; i++) {
   if (R[i] && (B[i] == BIT2) && !RA[i]) {
     for (x=0; x<aant_hki; x++) {
       fc1     = hki[x][0];
       fc2     = hki[x][1];
       stat    = hki[x][20];
       if (stat >= 2) {
         if ((i == fc1) || (i == fc2)) bewaak = TRUE;
       }
     }
   }
 }

 if (bewaak) {
   for (i=0; i<FCMAX; i++) {
     if (R[i] && (B[i] == BIT2) && !RA[i]) {
       for (j=0; j<FCMAX; j++) {
         if (R[j] && (B[j] == BIT2) && !RA[j] && (i != j)) {
           if (TFB_timer[j] > TFB_timer[i]) B[i] &= (~BIT2); 
           else                 B[j] &= (~BIT2); 
         }
       }
     }
   }
 }

}

/* PROCEDURE VOETGANGERS.FIETSOVERSTEEK */
/* ==================================== */

void proc_vtg_fts_crsv(                      /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 (vtg)   */
	mulv fc2,                            /* Signaalgroep 2 (fts 1) */
	mulv fc3,                            /* Signaalgroep 3 (fts 2) */
	mulv mre23,                          /* Meerea.fts 1 met fts 2 */
	mulv mre32)                          /* Meerea.fts 2 met fts 1 */
{
 bool xu1,xu2,xu3;
 bool _BLW = FALSE;
 bool _REA = FALSE;

 mulv i,j,_fc[3];

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;

 /* Altijd gelijktijdig starten wachtstand */
 /* -------------------------------------- */

 for (i=0; i<3; i++) {
   if (_fc[i] >= 0) {
     if (GL[_fc[i]] || TRG[_fc[i]] || K[_fc[i]]
      || (R[_fc[i]] && (BAR[_fc[i]] || RR[_fc[i]]))) _BLW = TRUE;
   }
 }
 if (_BLW) {
   for (i=0; i<3; i++) {
     if (_fc[i] >= 0) BLW[_fc[i]] = TRUE;
   }
 }

 /* Opzetten cyc.prim.aanvraag indien gekoppelde richting versneld inkomt */
 /* --------------------------------------------------------------------- */

 for (i=0; i<3; i++) {
   if (_fc[i] >= 0) {
     if (R[_fc[i]] && !TRG[_fc[i]] && (HVAS[_fc[i]]&BIT5)) {
       for (j=0; j<3; j++) {
	 if (i != j) {
	   if (_fc[j] >= 0) {
	     if (VA[_fc[j]]&BIT4) { VA[_fc[i]] |= BIT2; VA[_fc[i]] |= BIT4; }
	   }
	 }
       }
     }
   }
 }

 /* Doorschrijven koppelaanvragen */
 /* ----------------------------- */

 if (fc2 >= 0) VA[fc2] &= (~BIT3);
 if (fc3 >= 0) VA[fc3] &= (~BIT3);

 if (fc1 >= 0) {
   if (R[fc1] && !TRG[fc1] && VA[fc1]) {
     if (fc2 >= 0) {
	if (!BL[fc2] || G[fc2]) VA[fc2] |= BIT3;
     }
     if (fc3 >= 0) {
	if (!BL[fc3] || G[fc3]) VA[fc3] |= BIT3;
     }
   }
 }
 if (fc2 >= 0) {
   if (R[fc2] && !TRG[fc2] && VA[fc2]) {
     if ((fc3 >= 0) && (mre32 >= 0)) {
	if (SCH[mre32] && (!BL[fc3] || G[fc3])) VA[fc3] |= BIT3;
     }
   }
 }
 if (fc3 >= 0) {
   if (R[fc3] && !TRG[fc3] && VA[fc3]) {
     if ((fc2 >= 0) && (mre23 >= 0)) {
	if (SCH[mre23] && (!BL[fc2] || G[fc2])) VA[fc2] |= BIT2;
     }
   }
 }

 /* Beurt t.b.v. gelijke start */
 /* -------------------------- */

 for (i=0; i<3; i++) {
   if (_fc[i] >= 0) {
     if ((B[_fc[i]] || RA[_fc[i]]) && VA[_fc[i]] && !RR[_fc[i]]) _REA = TRUE;
   }
 }
 if (_REA) {
   for (i=0; i<3; i++) {
     if (_fc[i] >= 0) {
	if (R[_fc[i]] && !TRG[_fc[i]] && VA[_fc[i]]) {
	  B[_fc[i]] |= BIT3;
	  RR[_fc[i]] = FALSE;
	}
     }
   }
 }

 /* Vasthouden meeverlenggroen */
 /* -------------------------- */

 if (fc1 >= 0) {
   if ((B[fc1] || RA[fc1]) && !RR[fc1]) {
     if (fc2 >= 0) { if (G[fc2]) YM[fc2] |= BIT3; }
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
   }
   if (G[fc1] && (!WG[fc1] || !(VA[fc1]&BIT5) || SWG[fc1]) && (!MG[fc1] || (RW[fc1]&BIT3))) {
     if (fc2 >= 0) { if (G[fc2] && !somfbk_crsv(fc2)) YM[fc2] |= BIT1; }
     if (fc3 >= 0) { if (G[fc3] && !somfbk_crsv(fc3)) YM[fc3] |= BIT1; }
   }
 }
 if (fc2 >= 0) {
   if ((B[fc2] || RA[fc2]) && !RR[fc2]) {
     if (fc1 >= 0) { if (G[fc1]) YM[fc1] |= BIT3; }
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
   }
 }
 if (fc3 >= 0) {
   if ((B[fc3] || RA[fc3]) && !RR[fc3]) {
     if (fc1 >= 0) { if (G[fc1]) YM[fc1] |= BIT3; }
     if (fc2 >= 0) { if (G[fc2]) YM[fc2] |= BIT3; }
   }
 }

 /* Uitstel t.b.v. gelijke start */
 /* ---------------------------- */

 xu1 = xu2 = xu3 = TRUE;

 if (fc1 >= 0) {
   if (RA[fc1] && !K[fc1] && !X[fc1] && !RR[fc1] || G[fc1]
   ||  RV[fc1] && !TRG[fc1] && !VA[fc1]) xu1 = FALSE;
 } else {
   xu1 = FALSE;
 }
 if (fc2 >= 0) {
   if (RA[fc2] && !K[fc2] && !X[fc2] && !RR[fc2] || G[fc2]
   ||  RV[fc2] && !TRG[fc2] && !VA[fc2]) xu2 = FALSE;
 } else {
   xu2 = FALSE;
 }
 if (fc3 >= 0) {
   if (RA[fc3] && !K[fc3] && !X[fc3] && !RR[fc3] || G[fc3]
   ||  RV[fc3] && !TRG[fc3] && !VA[fc3]) xu3 = FALSE;
 } else {
   xu3 = FALSE;
 }
 if (xu1 || xu2 || xu3) { if (fc1 >= 0) X[fc1] |= BIT3;
			  if (fc2 >= 0) X[fc2] |= BIT3;
			  if (fc3 >= 0) X[fc3] |= BIT3;
 }

 /* Altijd gelijktijdig overslag */
 /* ---------------------------- */

 if (fc1 >= 0) {
   proc_covs_crsv_vtg_fts(fc1,fc2);
   proc_covs_crsv_vtg_fts(fc1,fc3);
 }
 if (fc2 >= 0) {
   proc_covs_crsv_vtg_fts(fc2,fc1);
   proc_covs_crsv_vtg_fts(fc2,fc3);
 }
 if (fc3 >= 0) {
   proc_covs_crsv_vtg_fts(fc3,fc1);
   proc_covs_crsv_vtg_fts(fc3,fc2);
 }

 /* Voorkom doorsnijden koppeling langzaam verkeer */
 /* ---------------------------------------------- */

 for (i=0; i<3; i++) {
   if (_fc[i] >= 0) {
	PVS2[_fc[i]] |= BIT2;
   }
 }
}

/* PROCEDURE PROGRESSIEVE VOETGANGERSOVERSTEEK */
/* =========================================== */
void proc_bb_vtg_crsv(                       /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpt12,                          /* KoppelTIJD eg1 -> eg2 */
	mulv fc3,                            /* Gekoppelde fietser 1  */
	mulv fc4,                            /* Gekoppelde fietser 2  */
	mulv mre34,                          /* Meerea. fiets 1 met 2 */
	mulv mre43)                          /* Meerea. fiets 2 met 1 */
{
 bool xu1,xu2,xu3,xu4;
 bool _BLW = FALSE;
 bool _REA = FALSE;

 mulv i,j,_fc[4];
 mulv kop12 = 0;
 if ((fc1 >= 0) && (fc2 >= 0)) kop12 = T_max[kpt12]/10;

 _fc[0]=fc1; _fc[1]=fc2;
 _fc[2]=fc3; _fc[3]=fc4;

 /* Altijd gelijktijdig starten wachtstand */
 /* -------------------------------------- */

 for (i=0; i<4; i++) {
   if (_fc[i] >= 0) {
     if (GL[_fc[i]] || TRG[_fc[i]] || K[_fc[i]]
      || (R[_fc[i]] && (BAR[_fc[i]] || RR[_fc[i]]))) _BLW = TRUE;
   }
 }
 if (_BLW) {
   for (i=0; i<4; i++) {
     if (_fc[i] >= 0) BLW[_fc[i]] = TRUE;
   }
 }

 /* Opzetten cyc.prim.aanvraag indien gekoppelde richting versneld inkomt */
 /* --------------------------------------------------------------------- */

 for (i=0; i<4; i++) {
   if (_fc[i] >= 0) {
     if (R[_fc[i]] && !TRG[_fc[i]] && (HVAS[_fc[i]]&BIT5)) {
       for (j=0; j<4; j++) {
	 if (i != j) {
	   if (_fc[j] >= 0) {
	     if (VA[_fc[j]]&BIT4) { VA[_fc[i]] |= BIT2; VA[_fc[i]] |= BIT4; }
	   }
	 }
       }
     }
   }
 }

 /* Doorschrijven koppelaanvragen */
 /* ----------------------------- */

 if (fc2 >= 0) VA[fc2] &= (~BIT3);
 if (fc3 >= 0) VA[fc3] &= (~BIT3);
 if (fc4 >= 0) VA[fc4] &= (~BIT3);

 if ((fc1 >= 0) && (fc2 >= 0)) {	/* voetgangerskoppeling */
   if ((R[fc1] && !TRG[fc1] && VA[fc1] || G[fc1]) && (!BL[fc2] || G[fc2]))
      VA[fc2] |= BIT3;
 }
 if (fc1 >= 0) {			/* fietsers met voetgangers */
   if (R[fc1] && !TRG[fc1] && VA[fc1]) {
     if (fc3 >= 0) {
	if (!BL[fc3] || G[fc3]) VA[fc3] |= BIT3;
     }
     if (fc4 >= 0) {
	if (!BL[fc4] || G[fc4]) VA[fc4] |= BIT3;
     }
   }
 }
 if (fc2 >= 0) {
   if (R[fc2] && !TRG[fc2] && VA[fc2]) {
     if (fc3 >= 0) {
	if (!BL[fc3] || G[fc3]) VA[fc3] |= BIT3;
     }
     if (fc4 >= 0) {
	if (!BL[fc4] || G[fc4]) VA[fc4] |= BIT3;
     }
   }
 }
 if (fc3 >= 0) {			/* fietsers met fietsers */
   if (R[fc3] && !TRG[fc3] && VA[fc3]) {
     if ((fc4 >= 0) && (mre43 >= 0)) {
	if (SCH[mre43] && (!BL[fc4] || G[fc4])) VA[fc4] |= BIT3;
     }
   }
 }
 if (fc4 >= 0) {
   if (R[fc4] && !TRG[fc4] && VA[fc4]) {
     if ((fc3 >= 0) && (mre34 >= 0)) {
	if (SCH[mre34] && (!BL[fc3] || G[fc3])) VA[fc3] |= BIT3;
     }
   }
 }

 /* Herstart koppeltijd */
 /* ------------------- */

 RT[kpt12] = FALSE;
 if ((fc1 >= 0) && (fc2 >= 0)) {
   if (G[fc1]) {
	RT[kpt12] = TRUE;
	VA[fc1]  |= BIT6;
   }
 }

 /* Vasthouden groen naloop-richting */
 /* -------------------------------- */

 if ((fc1 >= 0) && (fc2 >= 0)) {
   if (G[fc1] || RT[kpt12] || T[kpt12] || B[fc1] || RA[fc1]) RW[fc2] |=  BIT3;
   if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3))    YM[fc2] |=  BIT2;
   else                                                      YM[fc2] &=(~BIT2);

   if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
	if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
		HVAS[fc2] |= BIT3;
		 TTP[fc2]  = UTTP[fc2] = 255;
	}
   }
 }

 /* Beurt t.b.v. gelijke start */
 /* -------------------------- */

 for (i=0; i<4; i++) {
   if (_fc[i] >= 0) {
     if ((B[_fc[i]] || RA[_fc[i]]) && VA[_fc[i]] && !RR[_fc[i]]) _REA = TRUE;
   }
 }
 if (_REA) {
   for (i=0; i<4; i++) {
     if (_fc[i] >= 0) {
	if (R[_fc[i]] && !TRG[_fc[i]] && VA[_fc[i]]) {
	  B[_fc[i]] |= BIT3;
	  RR[_fc[i]] = FALSE;
	}
     }
   }
 }

 /* Vasthouden meeverlenggroen */
 /* -------------------------- */

 if (fc1 >= 0) {
   if ((B[fc1] || RA[fc1]) && !RR[fc1]) {
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
     if (fc4 >= 0) { if (G[fc4]) YM[fc4] |= BIT3; }
   }
   if (G[fc1] && (!WG[fc1] || !(VA[fc1]&BIT5) || SWG[fc1]) && (!MG[fc1] || (RW[fc1]&BIT3))) {
     if (fc3 >= 0) { if (G[fc3] && !somfbk_crsv(fc3)) YM[fc3] |= BIT1; }
     if (fc4 >= 0) { if (G[fc4] && !somfbk_crsv(fc4)) YM[fc4] |= BIT1; }
   }
 }
 if (fc2 >= 0) {
   if ((B[fc2] || RA[fc2]) && !RR[fc2]) {
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
     if (fc4 >= 0) { if (G[fc4]) YM[fc4] |= BIT3; }
   }
   if (G[fc2] && (!WG[fc2] || !(VA[fc2]&BIT5) || SWG[fc2]) && (!MG[fc2] || (RW[fc2]&BIT3))) {
     if (fc3 >= 0) { if (G[fc3] && !somfbk_crsv(fc3)) YM[fc3] |= BIT1; }
     if (fc4 >= 0) { if (G[fc4] && !somfbk_crsv(fc4)) YM[fc4] |= BIT1; }
   }
 }
 if (fc3 >= 0) {
   if ((B[fc3] || RA[fc3]) && !RR[fc3]) {
     if (fc1 >= 0) { if (G[fc1]) YM[fc1] |= BIT3; }
     if (fc2 >= 0) { if (G[fc2]) YM[fc2] |= BIT3; }
     if (fc4 >= 0) { if (G[fc4]) YM[fc4] |= BIT3; }
   }
 }
 if (fc4 >= 0) {
   if ((B[fc4] || RA[fc4]) && !RR[fc4]) {
     if (fc1 >= 0) { if (G[fc1]) YM[fc1] |= BIT3; }
     if (fc2 >= 0) { if (G[fc2]) YM[fc2] |= BIT3; }
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
   }
 }

 /* Uitstel t.b.v. gelijke start */
 /* ---------------------------- */

 xu1 = xu2 = xu3 = xu4 = TRUE;

 if (fc1 >= 0) {
   if (RA[fc1] && !K[fc1] && !X[fc1] && !RR[fc1] || G[fc1]
   ||  RV[fc1] && !TRG[fc1] && !VA[fc1]) xu1 = FALSE;
 } else {
   xu1 = FALSE;
 }
 if (fc2 >= 0) {
   if (RA[fc2] && !K[fc2] && !X[fc2] && !RR[fc2] || G[fc2]
   ||  RV[fc2] && !TRG[fc2] && !VA[fc2]) xu2 = FALSE;
 } else {
   xu2 = FALSE;
 }
 if (fc3 >= 0) {
   if (RA[fc3] && !K[fc3] && !X[fc3] && !RR[fc3] || G[fc3]
   ||  RV[fc3] && !TRG[fc3] && !VA[fc3]) xu3 = FALSE;
 } else {
   xu3 = FALSE;
 }
 if (fc4 >= 0) {
   if (RA[fc4] && !K[fc4] && !X[fc4] && !RR[fc4] || G[fc4]
   ||  RV[fc4] && !TRG[fc4] && !VA[fc4]) xu4 = FALSE;
 } else {
   xu4 = FALSE;
 }
 if (xu1 || xu2 || xu3 || xu4) { if (fc1 >= 0) X[fc1] |= BIT3;
				 if (fc2 >= 0) X[fc2] |= BIT3;
				 if (fc3 >= 0) X[fc3] |= BIT3;
				 if (fc4 >= 0) X[fc4] |= BIT3;
 }

 /* Beeindig groen voedende richting */
 /* -------------------------------- */

 if ((fc1 >= 0) && (fc2 >= 0)) {
   if (G[fc1] && G[fc2] && (TTK[fc2] <= kop12) && !(HVAS[fc1]&BIT0)) {
	Z[fc1] |= BIT1;
   }
 }

 /* Altijd gelijktijdig overslag */
 /* ---------------------------- */

 if (fc1 >= 0) {
   proc_covs_crsv_vtg_fts(fc1,fc2);
   proc_covs_crsv_vtg_fts(fc1,fc3);
   proc_covs_crsv_vtg_fts(fc1,fc4);
 }
 if (fc2 >= 0) {
   proc_covs_crsv_vtg_fts(fc2,fc1);
   proc_covs_crsv_vtg_fts(fc2,fc3);
   proc_covs_crsv_vtg_fts(fc2,fc4);
 }
 if (fc3 >= 0) {
   proc_covs_crsv_vtg_fts(fc3,fc1);
   proc_covs_crsv_vtg_fts(fc3,fc2);
   proc_covs_crsv_vtg_fts(fc3,fc4);
 }
 if (fc4 >= 0) {
   proc_covs_crsv_vtg_fts(fc4,fc1);
   proc_covs_crsv_vtg_fts(fc4,fc2);
   proc_covs_crsv_vtg_fts(fc4,fc3);
 }

 /* Voorkom doorsnijden koppeling langzaam verkeer */
 /* ---------------------------------------------- */

 for (i=0; i<4; i++) {
   if (_fc[i] >= 0) {
	PVS2[_fc[i]] |= BIT2;
   }
 }
}

/* PROCEDURE GESCHEIDEN VOETGANGERSOVERSTEEK */
/* ========================================= */

void proc_go_vtg_crsv(                       /* Fik150101 */
	mulv fc1,                            /* Signaalgroep 1 */
	mulv fc2,                            /* Signaalgroep 2 */
	mulv kpa12,                          /* Koppelaanvraag sg1 -> sg2  */
	mulv kpa21,                          /* Koppelaanvraag sg2 -> sg1  */
	mulv kpt12,                          /* KoppelTIJD     sg1 -> eg2  */
	mulv kpt21,                          /* KoppelTIJD     sg2 -> eg1  */
	mulv fc3,                            /* Gekoppelde fietser 1       */
	mulv fc4,                            /* Gekoppelde fietser 1 volg  */
	mulv mre34,                          /* Naloop nooit zonder voeding*/
	mulv kpt3s,                          /* Koppeltijd vanaf start fc3 */
	mulv kpt3e,                          /* Koppeltijd vanaf einde fc3 */
	mulv hfk3e,                          /* (HF)start naloop einde fc3 */
	mulv fc5,                            /* Gekoppelde fietser 2       */
	mulv fc6,                            /* Gekoppelde fietser 2 volg  */
	mulv mre56,                          /* Naloop nooit zonder voeding*/
	mulv kpt5s,                          /* Koppeltijd vanaf start fc5 */
	mulv kpt5e,                          /* Koppeltijd vanaf einde fc5 */
	mulv hfk5e,                          /* (HF)start naloop einde fc5 */
	mulv mre35,                          /* Meerea. fietser 1 met 2    */
	mulv mre53,                          /* Meerea. fietser 2 met 1    */
	mulv vtg3,                           /* Signaalgroep 3e voetganger */
	mulv kpa31,                          /* Kop.aanvr. sg3 -> sg1      */
	mulv kpa32,                          /* Kop.aanvr. sg3 -> sg2      */
	mulv kpt13,                          /* KoppelTIJD sg1 -> eg3      */
	mulv kpt23,                          /* KoppelTIJD sg2 -> eg3      */
	mulv kpt31,                          /* KoppelTIJD sg3 -> eg1      */
	mulv kpt32)                          /* KoppelTIJD sg3 -> eg2      */
{
 bool xu1,xu2,xu3,xu4,xu5,xu6,xu7;
 bool _BLW = FALSE;
 bool _REA = FALSE;
 mulv i,j,_fc[7];

 mulv kop34 = 0;
 mulv kop56 = 0;

 if ((fc3 >= 0) && (fc4 >=0)) kop34 = T_max[kpt3e]/10;
 if ((fc5 >= 0) && (fc6 >=0)) kop56 = T_max[kpt5e]/10;

 _fc[0]=fc1; _fc[1]=fc2; _fc[2]=fc3;
 _fc[3]=fc4; _fc[4]=fc5; _fc[5]=fc6; _fc[6]=vtg3;

 /* Alleen naar RA[] indien gekoppelde richting ook kan inkomen */
 /* ----------------------------------------------------------- */

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 < 0)) {
   if ((B[fc1] || RA[fc1]) && somfbk_crsv(fc2)) { B[fc1] = FALSE; RR[fc1] |= BIT3; }
   if ((B[fc2] || RA[fc2]) && somfbk_crsv(fc1)) { B[fc2] = FALSE; RR[fc2] |= BIT3; }
 }
 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0)) {
   if ((B[fc1]  || RA[fc1] ) && (somfbk_crsv(fc2) || somfbk_crsv(vtg3))) { B[fc1]  = FALSE; RR[fc1]  |= BIT3; }
   if ((B[fc2]  || RA[fc2] ) && (somfbk_crsv(fc1) || somfbk_crsv(vtg3))) { B[fc2]  = FALSE; RR[fc2]  |= BIT3; }
   if ((B[vtg3] || RA[vtg3]) && (somfbk_crsv(fc1) || somfbk_crsv(fc2) )) { B[vtg3] = FALSE; RR[vtg3] |= BIT3; }
 }

 /* Altijd gelijktijdig starten wachtstand */
 /* -------------------------------------- */

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if (GL[_fc[i]] || TRG[_fc[i]] || K[_fc[i]]
      || (R[_fc[i]] && (BAR[_fc[i]] || RR[_fc[i]]))) _BLW = TRUE;
   }
 }
 if (_BLW) {
   for (i=0; i<7; i++) {
     if (_fc[i] >= 0) BLW[_fc[i]] = TRUE;
   }
 }

 /* Opzetten cyc.prim.aanvraag indien gekoppelde richting versneld inkomt */
 /* --------------------------------------------------------------------- */

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if (R[_fc[i]] && !TRG[_fc[i]] && (HVAS[_fc[i]]&BIT5)) {
       for (j=0; j<7; j++) {
	 if (i != j) {
	   if (_fc[j] >= 0) {
	     if (VA[_fc[j]]&BIT4) { VA[_fc[i]] |= BIT2; VA[_fc[i]] |= BIT4; }
	   }
	 }
       }
     }
   }
 }

 /* Doorschrijven koppelaanvragen */
 /* ----------------------------- */

 if (fc1  >= 0) VA[fc1]  &= (~BIT3);
 if (fc2  >= 0) VA[fc2]  &= (~BIT3);
 if (vtg3 >= 0) VA[vtg3] &= (~BIT3);
 if (fc3  >= 0) VA[fc3]  &= (~BIT3);
 if (fc4  >= 0) VA[fc4]  &= (~BIT3);
 if (fc5  >= 0) VA[fc5]  &= (~BIT3);
 if (fc6  >= 0) VA[fc6]  &= (~BIT3);

 if ((fc1  >= 0) && (kpa12 >= 0)) { if (R[fc1]  && MM[kpa12] && !BL[fc1] ) VA[fc1]  |= BIT6; }
 if ((fc2  >= 0) && (kpa21 >= 0)) { if (R[fc2]  && MM[kpa21] && !BL[fc2] ) VA[fc2]  |= BIT6; }
 if ((vtg3 >= 0) && (kpa31 >= 0)) { if (R[vtg3] && MM[kpa31] && !BL[vtg3]) VA[vtg3] |= BIT6; }
 if ((vtg3 >= 0) && (kpa32 >= 0)) { if (R[vtg3] && MM[kpa32] && !BL[vtg3]) VA[vtg3] |= BIT6; }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 < 0)) {	          /* voetgangerskoppeling 2 vtg */
   if (R[fc1] && !TRG[fc1] && (VA[fc1]&BIT6)
			   && (!BL[fc2] || G[fc2])) VA[fc2] |= BIT3;
   if (R[fc2] && !TRG[fc2] && (VA[fc2]&BIT6)
			   && (!BL[fc1] || G[fc1])) VA[fc1] |= BIT3;
   if (XG[fc1]) VA[fc1] |= BIT3;
   if (XG[fc2]) VA[fc2] |= BIT3;
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0)) {	          /* voetgangerskoppeling 3 vtg */
   if (R[fc1] && !TRG[fc1] && (VA[fc1]&BIT6)) {
     if (!BL[fc2]  || G[fc2] ) VA[fc2]  |= BIT3;
     if (!BL[vtg3] || G[vtg3]) VA[vtg3] |= BIT3;
   }
   if (R[fc2] && !TRG[fc2] && (VA[fc2]&BIT6)) {
     if (!BL[fc1]  || G[fc1] ) VA[fc1]  |= BIT3;
     if (!BL[vtg3] || G[vtg3]) VA[vtg3] |= BIT3;
   }
   if (R[vtg3] && !TRG[vtg3]) {
     if (kpa31 >= 0) { if (MM[kpa31] && (!BL[fc1] || G[fc1])) VA[fc1] |= BIT3; }
     if (kpa32 >= 0) { if (MM[kpa32] && (!BL[fc2] || G[fc2])) VA[fc2] |= BIT3; }
   }
   if (XG[fc1] ) VA[fc1]  |= BIT3;
   if (XG[fc2] ) VA[fc2]  |= BIT3;
   if (XG[vtg3]) VA[vtg3] |= BIT3;
 }

 if (fc1 >= 0) {			/* fietsers met voetgangers */
   if (R[fc1] && !TRG[fc1] && VA[fc1]) {
     if (fc3 >= 0) {
	if (!BL[fc3] || G[fc3]) VA[fc3] |= BIT3;
     }
     if (fc5 >= 0) {
	if (!BL[fc5] || G[fc5]) VA[fc5] |= BIT3;
     }
   }
 }

 if (fc2 >= 0) {
   if (R[fc2] && !TRG[fc2] && VA[fc2]) {
     if (fc3 >= 0) {
	if (!BL[fc3] || G[fc3]) VA[fc3] |= BIT3;
     }
     if (fc5 >= 0) {
	if (!BL[fc5] || G[fc5]) VA[fc5] |= BIT3;
     }
   }
 }

 if (vtg3 >= 0) {
   if (R[vtg3] && !TRG[vtg3] && VA[vtg3]) {
     if (fc3 >= 0) {
	if (!BL[fc3] || G[fc3]) VA[fc3] |= BIT3;
     }
     if (fc5 >= 0) {
	if (!BL[fc5] || G[fc5]) VA[fc5] |= BIT3;
     }
   }
 }

 if (fc4 >= 0) {			/* terugschrijven naar voeding */
   if (R[fc4] && !TRG[fc4] && VA[fc4]) {
     if ((fc3 >= 0) && (mre34 >= 0)) {
	if (SCH[mre34] && (!BL[fc3] || G[fc3])) VA[fc3] |= BIT3;
     }
   }
 }

 if (fc6 >= 0) {
   if (R[fc6] && !TRG[fc6] && VA[fc6]) {
     if ((fc5 >= 0) && (mre56 >= 0)) {
	if (SCH[mre56] && (!BL[fc5] || G[fc5])) VA[fc5] |= BIT3;
     }
   }
 }

 if (fc3 >= 0) {			/* fietsers met fietsers */
   if (R[fc3] && !TRG[fc3] && VA[fc3]) {
     if ((fc5 >= 0) && (mre53 >= 0)) {
	if (SCH[mre53] && (!BL[fc5] || G[fc5])) VA[fc5] |= BIT3;
     }
   }
 }

 if (fc5 >= 0) {
   if (R[fc5] && !TRG[fc5] && VA[fc5]) {
     if ((fc3 >= 0) && (mre35 >= 0)) {
	if (SCH[mre35] && (!BL[fc3] || G[fc3])) VA[fc3] |= BIT3;
     }
   }
 }

 if ((fc3 >= 0) && (fc4 >= 0)) {	/* volgrichting fietsers */
   if ((R[fc3] && !TRG[fc3] && VA[fc3] || G[fc3]) && (!BL[fc4] || G[fc4]))
      VA[fc4] |= BIT3;
 }

 if ((fc5 >= 0) && (fc6 >= 0)) {
   if ((R[fc5] && !TRG[fc5] && VA[fc5] || G[fc5]) && (!BL[fc6] || G[fc6]))
      VA[fc6] |= BIT3;
 }

 /* Herstart koppeltijden voetgangers */
 /* --------------------------------- */

 if (kpt12 >= 0)  RT[kpt12] = FALSE;
 if (kpt13 >= 0)  RT[kpt13] = FALSE;
 if (kpt21 >= 0)  RT[kpt21] = FALSE;
 if (kpt23 >= 0)  RT[kpt23] = FALSE;
 if (kpt31 >= 0)  RT[kpt31] = FALSE;
 if (kpt32 >= 0)  RT[kpt32] = FALSE;

 if ((fc1 >= 0) && (fc2 >= 0)) {
   if (SG[fc1] && (VA[fc1]&BIT6)) {
     RT[kpt12] = TRUE;
     if (vtg3 >= 0) RT[kpt13] = TRUE;
   }
   if (SG[fc2] && (VA[fc2]&BIT6)) {
     RT[kpt21] = TRUE;
     if (vtg3 >= 0) RT[kpt23] = TRUE;
   }
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0)) {
   if ((kpa31 >= 0) && (kpt31 >= 0)) { if (SG[vtg3] && MM[kpa31]) RT[kpt31] = TRUE; }
   if ((kpa32 >= 0) && (kpt32 >= 0)) { if (SG[vtg3] && MM[kpa32]) RT[kpt32] = TRUE; }
 }
					/* Reset koppelaanvraag */
 if (fc1  >= 0) { if (SG[fc1] ) VA[fc1]  &= (~BIT6); }
 if (fc2  >= 0) { if (SG[fc2] ) VA[fc2]  &= (~BIT6); }
 if (vtg3 >= 0) { if (SG[vtg3]) VA[vtg3] &= (~BIT6); }

 /* Vasthouden groen naloop-richting voetgangers */
 /* -------------------------------------------- */

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 < 0)) {
   if (SG[fc1] && RT[kpt12] || T[kpt12] || B[fc1] || RA[fc1]) RW[fc2] |=  BIT3;
   if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3))     YM[fc2] |=  BIT2;
   else                                                       YM[fc2] &=(~BIT2);

   if (SG[fc2] && RT[kpt21] || T[kpt21] || B[fc2] || RA[fc2]) RW[fc1] |=  BIT3;
   if ((WG[fc1] || VG[fc1] || MG[fc1])	&& (RW[fc1]&BIT3))    YM[fc1] |=  BIT2;
   else                                                       YM[fc1] &=(~BIT2);

   if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
	if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
		HVAS[fc2] |= BIT3;
		 TTP[fc2]  = UTTP[fc2] = 255;
	}
   }
   if ((HVAS[fc2]&BIT3) && G[fc1] && (YM[fc1]&BIT2)) {
	if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
		HVAS[fc1] |= BIT3;
		 TTP[fc1]  = UTTP[fc1] = 255;
	}
   }
 }

 if ((fc1 >= 0) && (fc2 >= 0) && (vtg3 >= 0)) {
   if (SG[fc1]  && RT[kpt12] || T[kpt12] || B[fc1]  || RA[fc1] ) RW[fc2]  |= BIT3;
   if (SG[fc1]  && RT[kpt13] || T[kpt13] || B[fc1]  || RA[fc1] ) RW[vtg3] |= BIT3;

   if (SG[fc2]  && RT[kpt21] || T[kpt21] || B[fc2]  || RA[fc2] ) RW[fc1]  |= BIT3;
   if (SG[fc2]  && RT[kpt23] || T[kpt23] || B[fc2]  || RA[fc2] ) RW[vtg3] |= BIT3;

   if (SG[vtg3] && RT[kpt31] || T[kpt31] || B[vtg3] || RA[vtg3]) RW[fc1]  |= BIT3;
   if (SG[vtg3] && RT[kpt32] || T[kpt32] || B[vtg3] || RA[vtg3]) RW[fc2]  |= BIT3;

   if ((WG[fc1]  || VG[fc1]  || MG[fc1] ) && (RW[fc1]&BIT3) ) YM[fc1]  |=  BIT2;
   else                                                       YM[fc1]  &=(~BIT2);

   if ((WG[fc2]  || VG[fc2]  || MG[fc2] ) && (RW[fc2]&BIT3) ) YM[fc2]  |=  BIT2;
   else                                                       YM[fc2]  &=(~BIT2);

   if ((WG[vtg3] || VG[vtg3] || MG[vtg3]) && (RW[vtg3]&BIT3)) YM[vtg3] |=  BIT2;
   else                                                       YM[vtg3] &=(~BIT2);

   if ((HVAS[fc1]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
	if ((HVAS[fc1]&BIT7) == (HVAS[fc2]&BIT7)) {
		HVAS[fc2] |= BIT3;
		 TTP[fc2]  = UTTP[fc2] = 255;
	}
   }
   if ((HVAS[fc1]&BIT3) && G[vtg3] && (YM[vtg3]&BIT2)) {
	if ((HVAS[fc1]&BIT7) == (HVAS[vtg3]&BIT7)) {
		HVAS[vtg3] |= BIT3;
		 TTP[vtg3]  = UTTP[vtg3] = 255;
	}
   }
   if ((HVAS[fc2]&BIT3) && G[fc1] && (YM[fc1]&BIT2)) {
	if ((HVAS[fc2]&BIT7) == (HVAS[fc1]&BIT7)) {
		HVAS[fc1] |= BIT3;
		 TTP[fc1]  = UTTP[fc1] = 255;
	}
   }
   if ((HVAS[fc2]&BIT3) && G[vtg3] && (YM[vtg3]&BIT2)) {
	if ((HVAS[fc2]&BIT7) == (HVAS[vtg3]&BIT7)) {
		HVAS[vtg3] |= BIT3;
		 TTP[vtg3]  = UTTP[vtg3] = 255;
	}
   }
   if ((HVAS[vtg3]&BIT3) && G[fc1] && (YM[fc1]&BIT2)) {
	if ((HVAS[vtg3]&BIT7) == (HVAS[fc1]&BIT7)) {
		HVAS[fc1] |= BIT3;
		 TTP[fc1]  = UTTP[fc1] = 255;
	}
   }
   if ((HVAS[vtg3]&BIT3) && G[fc2] && (YM[fc2]&BIT2)) {
	if ((HVAS[vtg3]&BIT7) == (HVAS[fc2]&BIT7)) {
		HVAS[fc2] |= BIT3;
		 TTP[fc2]  = UTTP[fc2] = 255;
	}
   }
 }

 /* Herstart koppeltijden fietsers */
 /* ------------------------------ */

 if (kpt3s >= 0) RT[kpt3s] = FALSE;
 if (kpt3e >= 0) RT[kpt3e] = FALSE;

 if ((fc3 >= 0) && (fc4 >= 0)) {
   if (SG[fc3]) {
	RT[kpt3s] = TRUE;
	VA[fc3]  |= BIT6;
   }
   if (G[fc3] && IH[hfk3e]) {
	RT[kpt3e] = TRUE;
   }
 }

 if (kpt5s >= 0) RT[kpt5s] = FALSE;
 if (kpt5e >= 0) RT[kpt5e] = FALSE;

 if ((fc5 >= 0) && (fc6 >= 0)) {
   if (SG[fc5]) {
	RT[kpt5s] = TRUE;
	VA[fc5]  |= BIT6;
   }
   if (G[fc5] && IH[hfk5e]) {
	RT[kpt5e] = TRUE;
   }
 }

 /* Vasthouden groen naloop-richting fietsers */
 /* ----------------------------------------- */

 if ((fc3 >= 0) && (fc4 >= 0)) {
   if (G[fc3] || T[kpt3s] || T[kpt3e] || B[fc3] || RA[fc3])
	RW[fc4] |= BIT3;

   if ((WG[fc4] || VG[fc4] || MG[fc4]) && (RW[fc4]&BIT3)) YM[fc4] |=  BIT2;
   else                                                   YM[fc4] &=(~BIT2);

   if ((HVAS[fc3]&BIT3) && G[fc4] && (YM[fc4]&BIT2)) {
	if ((HVAS[fc3]&BIT7) == (HVAS[fc4]&BIT7)) {
		HVAS[fc4] |= BIT3;
		 TTP[fc4]  = UTTP[fc4] = 255;
	}
   }
 }

 if ((fc5 >= 0) && (fc6 >= 0)) {
   if (G[fc5] || T[kpt5s] || T[kpt5e] || B[fc5] || RA[fc5])
	RW[fc6] |= BIT3;

   if ((WG[fc6] || VG[fc6] || MG[fc6]) && (RW[fc6]&BIT3)) YM[fc6] |=  BIT2;
   else                                                   YM[fc6] &=(~BIT2);

   if ((HVAS[fc5]&BIT3) && G[fc6] && (YM[fc6]&BIT2)) {
	if ((HVAS[fc5]&BIT7) == (HVAS[fc6]&BIT7)) {
		HVAS[fc6] |= BIT3;
		 TTP[fc6]  = UTTP[fc6] = 255;
	}
   }
 }

 /* Beurt t.b.v. gelijke start */
 /* -------------------------- */

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
     if ((B[_fc[i]] || RA[_fc[i]]) && VA[_fc[i]] && !RR[_fc[i]]) _REA = TRUE;
   }
 }
 if (_REA) {
   for (i=0; i<7; i++) {
     if (_fc[i] >= 0) {
	if (R[_fc[i]] && !TRG[_fc[i]] && VA[_fc[i]]) {
	  B[_fc[i]] |= BIT3;
	  RR[_fc[i]] = FALSE;
	}
     }
   }
 }

 /* Vasthouden meeverlenggroen */
 /* -------------------------- */

 if (fc1 >= 0) {
   if ((B[fc1] || RA[fc1]) && !RR[fc1]) {
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
     if (fc4 >= 0) { if (G[fc4]) YM[fc4] |= BIT3; }
     if (fc5 >= 0) { if (G[fc5]) YM[fc5] |= BIT3; }
     if (fc6 >= 0) { if (G[fc6]) YM[fc6] |= BIT3; }
   }
   if (G[fc1] && (!WG[fc1] || !(VA[fc1]&BIT5) || SWG[fc1]) && (!MG[fc1] || (RW[fc1]&BIT3))) {
     if (fc3 >= 0) { if (G[fc3] && !somfbk_crsv(fc3)) YM[fc3] |= BIT1; }
     if (fc5 >= 0) { if (G[fc5] && !somfbk_crsv(fc5)) YM[fc5] |= BIT1; }
   }
 }
 if (fc2 >= 0) {
   if ((B[fc2] || RA[fc2]) && !RR[fc2]) {
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
     if (fc4 >= 0) { if (G[fc4]) YM[fc4] |= BIT3; }
     if (fc5 >= 0) { if (G[fc5]) YM[fc5] |= BIT3; }
     if (fc6 >= 0) { if (G[fc6]) YM[fc6] |= BIT3; }
   }
   if (G[fc2] && (!WG[fc2] || !(VA[fc2]&BIT5) || SWG[fc2]) && (!MG[fc2] || (RW[fc2]&BIT3))) {
     if (fc3 >= 0) { if (G[fc3] && !somfbk_crsv(fc3)) YM[fc3] |= BIT1; }
     if (fc5 >= 0) { if (G[fc5] && !somfbk_crsv(fc5)) YM[fc5] |= BIT1; }
   }
 }
 if (vtg3 >= 0) {
   if ((B[vtg3] || RA[vtg3]) && !RR[vtg3]) {
     if (fc3 >= 0) { if (G[fc3]) YM[fc3] |= BIT3; }
     if (fc4 >= 0) { if (G[fc4]) YM[fc4] |= BIT3; }
     if (fc5 >= 0) { if (G[fc5]) YM[fc5] |= BIT3; }
     if (fc6 >= 0) { if (G[fc6]) YM[fc6] |= BIT3; }
   }
   if (G[vtg3] && (!WG[vtg3] || !(VA[vtg3]&BIT5) || SWG[vtg3]) && (!MG[vtg3] || (RW[vtg3]&BIT3))) {
     if (fc3 >= 0) { if (G[fc3] && !somfbk_crsv(fc3)) YM[fc3] |= BIT1; }
     if (fc5 >= 0) { if (G[fc5] && !somfbk_crsv(fc5)) YM[fc5] |= BIT1; }
   }
 }

 if (fc3 >= 0) {
   if ((B[fc3] || RA[fc3]) && !RR[fc3]) {
     if (fc1  >= 0) { if (G[fc1] ) YM[fc1]  |= BIT3; }
     if (fc2  >= 0) { if (G[fc2] ) YM[fc2]  |= BIT3; }
     if (vtg3 >= 0) { if (G[vtg3]) YM[vtg3] |= BIT3; }
     if (fc4  >= 0) { if (G[fc4] ) YM[fc4]  |= BIT3; }
     if (fc5  >= 0) { if (G[fc5] ) YM[fc5]  |= BIT3; }
     if (fc6  >= 0) { if (G[fc6] ) YM[fc6]  |= BIT3; }
   }
 }
 if (fc4 >= 0) {
   if ((B[fc4] || RA[fc4]) && !RR[fc4]) {
     if (fc1  >= 0) { if (G[fc1] ) YM[fc1]  |= BIT3; }
     if (fc2  >= 0) { if (G[fc2] ) YM[fc2]  |= BIT3; }
     if (vtg3 >= 0) { if (G[vtg3]) YM[vtg3] |= BIT3; }
     if (fc3  >= 0) { if (G[fc3] ) YM[fc3]  |= BIT3; }
     if (fc5  >= 0) { if (G[fc5] ) YM[fc5]  |= BIT3; }
     if (fc6  >= 0) { if (G[fc6] ) YM[fc6]  |= BIT3; }
   }
 }
 if (fc5 >= 0) {
   if ((B[fc5] || RA[fc5]) && !RR[fc5]) {
     if (fc1  >= 0) { if (G[fc1] ) YM[fc1]  |= BIT3; }
     if (fc2  >= 0) { if (G[fc2] ) YM[fc2]  |= BIT3; }
     if (vtg3 >= 0) { if (G[vtg3]) YM[vtg3] |= BIT3; }
     if (fc3  >= 0) { if (G[fc3] ) YM[fc3]  |= BIT3; }
     if (fc4  >= 0) { if (G[fc4] ) YM[fc4]  |= BIT3; }
     if (fc6  >= 0) { if (G[fc6] ) YM[fc6]  |= BIT3; }
   }
 }
 if (fc6 >= 0) {
   if ((B[fc6] || RA[fc6]) && !RR[fc6]) {
     if (fc1  >= 0) { if (G[fc1] ) YM[fc1]  |= BIT3; }
     if (fc2  >= 0) { if (G[fc2] ) YM[fc2]  |= BIT3; }
     if (vtg3 >= 0) { if (G[vtg3]) YM[vtg3] |= BIT3; }
     if (fc3  >= 0) { if (G[fc3] ) YM[fc3]  |= BIT3; }
     if (fc4  >= 0) { if (G[fc4] ) YM[fc4]  |= BIT3; }
     if (fc5  >= 0) { if (G[fc5] ) YM[fc5]  |= BIT3; }
   }
 }

 /* Uitstel t.b.v. gelijke start */
 /* ---------------------------- */

 if ((fc3 >= 0) || (fc4 >= 0) || (fc5 >= 0) || (fc6 >= 0)) {

   xu1 = xu2 = xu3 = xu4 = xu5 = xu6 = xu7 = TRUE;

   if (fc1 >= 0) {
     if (RA[fc1] && !K[fc1] && !X[fc1] && !RR[fc1] || G[fc1]
     ||  RV[fc1] && !TRG[fc1] && !VA[fc1]) xu1 = FALSE;
   } else {
     xu1 = FALSE;
   }
   if (fc2 >= 0) {
     if (RA[fc2] && !K[fc2] && !X[fc2] && !RR[fc2] || G[fc2]
     ||  RV[fc2] && !TRG[fc2] && !VA[fc2]) xu2 = FALSE;
   } else {
     xu2 = FALSE;
   }
   if (fc3 >= 0) {
     if (RA[fc3] && !K[fc3] && !X[fc3] && !RR[fc3] || G[fc3]
     ||  RV[fc3] && !TRG[fc3] && !VA[fc3]) xu3 = FALSE;
   } else {
     xu3 = FALSE;
   }
   if (fc4 >= 0) {
     if (RA[fc4] && !K[fc4] && !X[fc4] && !RR[fc4] || G[fc4]
     ||  RV[fc4] && !TRG[fc4] && !VA[fc4]) xu4 = FALSE;
   } else {
     xu4 = FALSE;
   }
   if (fc5 >= 0) {
     if (RA[fc5] && !K[fc5] && !X[fc5] && !RR[fc5] || G[fc5]
     ||  RV[fc5] && !TRG[fc5] && !VA[fc5]) xu5 = FALSE;
   } else {
     xu5 = FALSE;
   }
   if (fc6 >= 0) {
     if (RA[fc6] && !K[fc6] && !X[fc6] && !RR[fc6] || G[fc6]
     ||  RV[fc6] && !TRG[fc6] && !VA[fc6]) xu6 = FALSE;
   } else {
     xu6 = FALSE;
   }
   if (vtg3 >= 0) {
     if (RA[vtg3] && !K[vtg3] && !X[vtg3] && !RR[vtg3] || G[vtg3]
     ||  RV[vtg3] && !TRG[vtg3] && !VA[vtg3]) xu7 = FALSE;
   } else {
     xu7 = FALSE;
   }

 } else {	/* er is geen gekoppelde fietser aanwezig */

   xu1 = xu2 = xu7 = TRUE;
   xu3 = xu4 = xu5 = xu6 = FALSE;

   if (fc1 >= 0) {
     if (RA[fc1] && (!K[fc1] || !(VA[fc1]&BIT6) && ((VA[fc2]&BIT6) || G[fc2])) && !X[fc1] && !RR[fc1] || G[fc1]
     ||  RV[fc1] && !TRG[fc1] && !VA[fc1]) xu1 = FALSE;
   } else {
     xu1 = FALSE;
   }
   if (fc2 >= 0) {
     if (RA[fc2] && (!K[fc2] || !(VA[fc2]&BIT6) && ((VA[fc1]&BIT6) || G[fc1])) && !X[fc2] && !RR[fc2] || G[fc2]
     ||  RV[fc2] && !TRG[fc2] && !VA[fc2]) xu2 = FALSE;
   } else {
     xu2 = FALSE;
   }
   if (vtg3 >= 0) {
     if (RA[vtg3] && (!K[vtg3] || G[fc1] || G[fc2]) && !X[vtg3] && !RR[vtg3] || G[vtg3]
     ||  RV[vtg3] && !TRG[vtg3] && !VA[vtg3]) xu7 = FALSE;
   } else {
     xu7 = FALSE;
   }
 }

 if (xu1 || xu2 || xu3 || xu4 || xu5 || xu6 || xu7) { if (fc1  >= 0) X[fc1]  |= BIT3;
					              if (fc2  >= 0) X[fc2]  |= BIT3;
					              if (fc3  >= 0) X[fc3]  |= BIT3;
					              if (fc4  >= 0) X[fc4]  |= BIT3;
					              if (fc5  >= 0) X[fc5]  |= BIT3;
					              if (fc6  >= 0) X[fc6]  |= BIT3;
					              if (vtg3 >= 0) X[vtg3] |= BIT3;
 }

/* Garandeer nalooprichting voetgangerskoppeling */
/* --------------------------------------------- */
 if ((fc1 >= 0) && (fc2 >= 0)) {
   if ((XG[fc1]&BIT2) && RA[fc1] && VA[fc1]) {
	B[fc1]  |= BIT3; 
	RR[fc1]  = X[fc1] = FALSE;
	YM[fc2] |= BIT3;
        if (vtg3 >= 0) YM[vtg3] |= BIT3;
   }
   if ((XG[fc2]&BIT2) && RA[fc2] && VA[fc2]) {
	B[fc2]  |= BIT3; 
	RR[fc2]  = X[fc2] = FALSE;
	YM[fc1] |= BIT3;
        if (vtg3 >= 0) YM[vtg3] |= BIT3;
   }
   if (vtg3 >= 0) {
     if ((XG[vtg3]&BIT2) && RA[vtg3] && VA[vtg3]) {
	B[vtg3]  |= BIT3; 
	RR[vtg3]  = X[vtg3] = FALSE;
	YM[fc1]  |= BIT3;
	YM[fc2]  |= BIT3;
     }
   }
 }

 /* Beeindig groen voedende richting */
 /* -------------------------------- */

 if ((fc3 >= 0) && (fc4 >= 0)) {
   if (G[fc3] && G[fc4] && (TTK[fc4] <= kop34) && !(HVAS[fc3]&BIT0)) {
	Z[fc3] |= BIT1;
   }
 }
 if ((fc5 >= 0) && (fc6 >= 0)) {
   if (G[fc5] && G[fc6] && (TTK[fc6] <= kop56) && !(HVAS[fc5]&BIT0)) {
	Z[fc5] |= BIT1;
   }
 }

 /* Altijd gelijktijdig overslag */
 /* ---------------------------- */

 if (fc1 >= 0) {
   proc_covs_crsv_vtg_fts(fc1,fc2);
   proc_covs_crsv_vtg_fts(fc1,fc3);
   proc_covs_crsv_vtg_fts(fc1,fc4);
   proc_covs_crsv_vtg_fts(fc1,fc5);
   proc_covs_crsv_vtg_fts(fc1,fc6);
   proc_covs_crsv_vtg_fts(fc1,vtg3);
 }
 if (fc2 >= 0) {
   proc_covs_crsv_vtg_fts(fc2,fc1);
   proc_covs_crsv_vtg_fts(fc2,fc3);
   proc_covs_crsv_vtg_fts(fc2,fc4);
   proc_covs_crsv_vtg_fts(fc2,fc5);
   proc_covs_crsv_vtg_fts(fc2,fc6);
   proc_covs_crsv_vtg_fts(fc2,vtg3);
 }
 if (fc3 >= 0) {
   proc_covs_crsv_vtg_fts(fc3,fc1);
   proc_covs_crsv_vtg_fts(fc3,fc2);
   proc_covs_crsv_vtg_fts(fc3,fc4);
   proc_covs_crsv_vtg_fts(fc3,fc5);
   proc_covs_crsv_vtg_fts(fc3,fc6);
   proc_covs_crsv_vtg_fts(fc3,vtg3);
 }
 if (fc4 >= 0) {
   proc_covs_crsv_vtg_fts(fc4,fc1);
   proc_covs_crsv_vtg_fts(fc4,fc2);
   proc_covs_crsv_vtg_fts(fc4,fc3);
   proc_covs_crsv_vtg_fts(fc4,fc5);
   proc_covs_crsv_vtg_fts(fc4,fc6);
   proc_covs_crsv_vtg_fts(fc4,vtg3);
 }
 if (fc5 >= 0) {
   proc_covs_crsv_vtg_fts(fc5,fc1);
   proc_covs_crsv_vtg_fts(fc5,fc2);
   proc_covs_crsv_vtg_fts(fc5,fc3);
   proc_covs_crsv_vtg_fts(fc5,fc4);
   proc_covs_crsv_vtg_fts(fc5,fc6);
   proc_covs_crsv_vtg_fts(fc5,vtg3);
 }
 if (fc6 >= 0) {
   proc_covs_crsv_vtg_fts(fc6,fc1);
   proc_covs_crsv_vtg_fts(fc6,fc2);
   proc_covs_crsv_vtg_fts(fc6,fc3);
   proc_covs_crsv_vtg_fts(fc6,fc4);
   proc_covs_crsv_vtg_fts(fc6,fc5);
   proc_covs_crsv_vtg_fts(fc6,vtg3);
 }
 if (vtg3 >= 0) {
   proc_covs_crsv_vtg_fts(vtg3,fc1);
   proc_covs_crsv_vtg_fts(vtg3,fc2);
   proc_covs_crsv_vtg_fts(vtg3,fc3);
   proc_covs_crsv_vtg_fts(vtg3,fc4);
   proc_covs_crsv_vtg_fts(vtg3,fc5);
   proc_covs_crsv_vtg_fts(vtg3,fc6);
 }

 /* Voorkom doorsnijden koppeling langzaam verkeer */
 /* ---------------------------------------------- */

 for (i=0; i<7; i++) {
   if (_fc[i] >= 0) {
	PVS2[_fc[i]] |= BIT2;
   }
 }
}

/* PROCEDURE SERIELE KOPPELING */
/* =========================== */

void proc_hki_crsv(void)                        /* Fik150101 */
{
 mulv fc1,fc2,fc3,stat,i,j,x;
 mulv kptvs12,kptsg12,kpteg12,kptdm12;
 mulv kptvs23,kptsg23,kpteg23,kptdm23;
 mulv de11,de12,de13,de21,de22,de23;
 mulv kopvs12 = 0;
 mulv kopvs23 = 0;
 bool hki_gebied12 = FALSE; 
 bool hki_gebied23 = FALSE; 

 for (i=0; i<aant_hki; i++) {			/* reset bits voor richtingen die onderdeel zijn van tenminste 1 seriele koppeling */
	fc2 = hki[i][1];
	fc3 = hki[i][10];
	  YM[fc2] &= (~BIT2);
	PVS2[fc2] &= (~BIT2);
	if (fc3 >= 0) {
		  YM[fc3] &= (~BIT2);
		PVS2[fc3] &= (~BIT2);
	} else {
		BW_hki[i][fc2] = FALSE;		/* VLAG wordt waar als fc2 door groen (of naloop) vanaf fc1 wordt vastgehouden */
	}
 }

 for (x=0; x<3; x++) {				/* 3x doorlopen ivm mogelijke geneste koppelingen */
    for (i=0; i<aant_hki; i++) {
	fc1     = hki[i][0];
	fc2     = hki[i][1];
	kptvs12 = hki[i][2];
	kptsg12 = hki[i][3];
	kpteg12 = hki[i][4];
	kptdm12 = hki[i][5];
	de11    = hki[i][6];
	de12    = hki[i][7];
	de13    = hki[i][8];
	fc3     = hki[i][10];
	kptvs23 = hki[i][11];
	kptsg23 = hki[i][12];
	kpteg23 = hki[i][13];
	kptdm23 = hki[i][14];
	de21    = hki[i][15];
	de22    = hki[i][16];
	de23    = hki[i][17];
	stat    = hki[i][20];

	if (kptsg12 >= 0) RT[kptsg12] = AT[kptsg12] = FALSE;
	if (kpteg12 >= 0) RT[kpteg12] = AT[kpteg12] = FALSE;
	if (kptdm12 >= 0) RT[kptdm12] = AT[kptdm12] = FALSE;

	if (kptsg23 >= 0) RT[kptsg23] = AT[kptsg23] = FALSE;
	if (kpteg23 >= 0) RT[kpteg23] = AT[kpteg23] = FALSE;
	if (kptdm23 >= 0) RT[kptdm23] = AT[kptdm23] = FALSE;

	if (stat >= 2) {			/* koppeling is aktief */

		if (kptvs12 >= 0) kopvs12 = T_max[kptvs12]/10;
		if (kptvs23 >= 0) kopvs23 = T_max[kptvs23]/10;

		/* Set XG bit volgrichting bij alternatieve realisatie voeding */
		/* ----------------------------------------------------------- */
		if (SG[fc1] && !(HVAS[fc1]&BIT0) && !(HVAS[fc1]&BIT4) && (TTP[fc1] > 0) && (TTPR[fc1] > 0)) XG_hki[i][fc2] |= BIT6;
		if (G[fc2]) XG_hki[i][fc2] &= (~BIT6);

		if (fc3 >= 0) {
			if (SG[fc2] && !(HVAS[fc2]&BIT0) && !(HVAS[fc2]&BIT4) && (TTP[fc2] > 0) && (TTPR[fc2] > 0)) XG_hki[i][fc3] |= BIT6;
			if (G[fc3]) XG_hki[i][fc3] &= (~BIT6);
		}

		/* Voedende richting alleen wachtstand indien naloop-richting kan inkomen */
		/* ---------------------------------------------------------------------- */
		if (GL[fc2] || TRG[fc2] || K[fc2] || (R[fc2] && (BAR[fc2] || RR[fc2]))) {
			BLW[fc1] = TRUE;
		}

		if (fc3 >= 0) {
			if (GL[fc3] || TRG[fc3] || K[fc3] || (R[fc3] && (BAR[fc3] || RR[fc3]))) {
				BLW[fc1] = BLW[fc2] = TRUE;
			}
		}


		/* Doorschrijven koppelaanvraag */
		/* ---------------------------- */
		if (G[fc1] && (!BL[fc2] || G[fc2])) VA[fc2] |= BIT3;

		if (fc3 >= 0) {
			if (G[fc2] && BW_hki[i][fc2] && (!BL[fc3] || G[fc3])) VA[fc3] |= BIT3;
		}

		/* Herstart koppeltijden */
		/* --------------------- */
		if (G[fc1]) {
			if (SG[fc1]) RT[kptsg12] = TRUE;
			if (de11 >= 0) {
				if (D[de11] || (CIF_IS[de11]>=CIF_DET_STORING) || OG[de11] || BG[de11]) RT[kpteg12] = TRUE;
			}
			if (de12 >= 0) {
				if (D[de12] || (CIF_IS[de12]>=CIF_DET_STORING) || OG[de12] || BG[de12]) RT[kpteg12] = TRUE;
			}
			if (de13 >= 0) {
				if (D[de13] || (CIF_IS[de13]>=CIF_DET_STORING) || OG[de13] || BG[de13]) RT[kpteg12] = TRUE;
			}
			if (!(de11 >= 0) && !(de12 >= 0) && !(de13 >= 0)) RT[kpteg12] = TRUE;
			if (G[fc2]) VA[fc1] |= BIT6;
 		}
		if (RT[kptsg12] || T[kptsg12] || RT[kpteg12] || T[kpteg12]) RT[kptdm12] = TRUE;

		if (!G[fc1] && !T[kptsg12] && !T[kpteg12] && !VG1[fc2] && !(MK[fc2]&BIT2)) {
			AT[kptdm12] = TRUE;
			PVAS_hki[i][fc2] &= (~BIT1);
		}

		if (fc3 >= 0) {
			if (G[fc2]) {
				if (SG[fc2]) RT[kptsg23] = TRUE;
				if (BW_hki[i][fc2]) {
					if (de21 >= 0) {
						if (D[de21] || (CIF_IS[de21]>=CIF_DET_STORING) || OG[de21] || BG[de21]) RT[kpteg23] = TRUE;
					}
					if (de22 >= 0) {
						if (D[de22] || (CIF_IS[de22]>=CIF_DET_STORING) || OG[de22] || BG[de22]) RT[kpteg23] = TRUE;
					}
					if (de23 >= 0) {
						if (D[de23] || (CIF_IS[de23]>=CIF_DET_STORING) || OG[de23] || BG[de23]) RT[kpteg23] = TRUE;
					}
					if (!(de21 >= 0) && !(de22 >= 0) && !(de23 >= 0)) RT[kpteg23] = TRUE;
				}
				if (G[fc3]) VA[fc2] |= BIT6;
	 		}
			if (RT[kptsg23] || T[kptsg23] || RT[kpteg23] || T[kpteg23]) RT[kptdm23] = TRUE;

			if ((!G[fc2] || !(BW_hki[i][fc2])) && !T[kptsg23] && !T[kpteg23] && !VG1[fc3] && !(MK[fc3]&BIT2)) {
				AT[kptdm23] = TRUE;
				PVAS_hki[i][fc3] &= (~BIT1);
			}
		}

		/* Vasthouden groen naloop-richting */
		/* -------------------------------- */
		if (G[fc1] || (PVS2[fc1]&BIT4) || T[kptsg12] || T[kpteg12] || RT[kptdm12] || T[kptdm12]) {
			RW[fc2] |= BIT3;
			RW_hki[i][fc2] |= BIT3;
			BW_hki[i][fc2] = TRUE;
		}

		if ((WG[fc2] || VG[fc2] || MG[fc2]) && (RW[fc2]&BIT3)) YM[fc2] |= BIT2;
		if (  B[fc1] || RA[fc1] || (PVS2[fc1]&BIT4))           YM[fc2] |= BIT3;

		if (fc3 >= 0) {
			if (G[fc2] && BW_hki[i][fc2] || (PVS2[fc2]&BIT4) || T[kptsg23] || T[kpteg23] || RT[kptdm23] || T[kptdm23]) {
				RW[fc3] |= BIT3;
				RW_hki[i][fc3] |= BIT3;
			}

			if ((WG[fc3] || VG[fc3] || MG[fc3]) && (RW[fc3]&BIT3))  YM[fc3] |= BIT2;
			if (  B[fc2] || RA[fc2] || XG[fc2] || (PVS2[fc2]&BIT4)) YM[fc3] |= BIT3;
		}

		/* Uitstel voedende richting */
		/* ------------------------- */
		if ((TTP[fc1]>0) && (TTP[fc2]> kopvs12) && (TTR[fc2]>kopvs12)) X[fc1] |= BIT3;
		if (fc3 >= 0) {
			if ((TTP[fc1]>0) && (TTP[fc3]>(kopvs12+kopvs23)) && (TTR[fc3]>(kopvs12+kopvs23))) X[fc1] |= BIT3;
			if ((TTP[fc2]>0) && (TTP[fc3]> kopvs23)          && (TTR[fc3]> kopvs23))          X[fc2] |= BIT3;
		}

		/* Beurt naloop-richting */
		/* --------------------- */	/* RV[] && !somfbk_crsv() && (XG_hki[i][fc2]&BIT6) toegevoegd Fik110216 */
		if (XG[fc2] && (RV[fc2] && !somfbk_crsv(fc2) && (XG_hki[i][fc2]&BIT6) || RA[fc2]) && VA[fc2]) {
			B[fc2] |= BIT3; 
			RR[fc2] = FALSE;
		}

		if (fc3 >= 0) {
			if (XG[fc3] && (RV[fc3] && !somfbk_crsv(fc3) && (XG_hki[i][fc2]&BIT6) || RA[fc3]) && VA[fc3]) {
				B[fc3] |= BIT3; 
				RR[fc3] = FALSE;
			}
		}

		/* Beeindig groen voedende richting */
		/* -------------------------------- */						/* toegevoegd: (XG_hki[]&BIT6) Fik110216 */
/*	        if (G[fc1] && (G[fc2] || XG[fc2]) && (!(HVAS[fc1]&BIT0) || (KG[fc1] > ALTM[fc1])) */
/*		if (G[fc1] && (G[fc2] || XG[fc2]) && ((KG[fc1] > MAXG[fc1]) && (HVAS[fc1]&BIT0) || (KG[fc1] > ALTM[fc1]) && !(HVAS[fc1]&BIT0)) @#@ HVRT */
	        if (G[fc1] && (G[fc2] || XG[fc2]) && ((KG[fc1] > KAPM[fc1]) || (KG[fc1] > ALTM[fc1]))
                           && (TTK[fc2] <= TEG[fc2]) && ((TTK[fc2] > 0) || G[fc2]) && ((XG_hki[i][fc2]&BIT6) || RA[fc2] || G[fc2] || (TTK[fc2] > TTP[fc2]))) {
			Z[fc1] |= BIT1;
		}

		if (fc3 >= 0) {
			if (G[fc1] && (G[fc3] || XG[fc3]) && ((KG[fc1] > KAPM[fc1]) || (KG[fc1] > ALTM[fc1]))
                                   && (TTK[fc3] <= TEG[fc3]) && ((TTK[fc3] > 0) || G[fc3]) && ((XG_hki[i][fc3]&BIT6) || RA[fc3] || G[fc3] || (TTK[fc3] > TTP[fc3]))) {
				Z[fc1] |= BIT1;
			}
			if (G[fc2] && (G[fc3] || XG[fc3]) && ((KG[fc2] > KAPM[fc2]) || (KG[fc2] > ALTM[fc2]))
                                   && (TTK[fc3] <= TEG[fc3]) && ((TTK[fc3] > 0) || G[fc3]) && ((XG_hki[i][fc3]&BIT6) || RA[fc3] || G[fc3] || (TTK[fc3] > TTP[fc3]))) {
				if ((x > 0) && !(RW[fc2]&BIT3)) Z[fc2] |= BIT1;			/* in 1e rondje worden de RW-bitjes geset */
				if (G[fc1]) Z[fc1] |= BIT1;
			}
		}

		/* Overslag voedende richting bij overslag naloop-richting */
		/* ------------------------------------------------------- */
		hki_gebied12 = hki_gebied23 = FALSE;

		if (fc3 >= 0) {
			if ((VAS2[fc2] != 0) && (VAS4[fc2] != 0) && (VAS2[fc2] != VAS4[fc2]) &&
			    (VAS2[fc3] != 0) && (VAS4[fc3] != 0) && (VAS2[fc3] != VAS4[fc3])) {

				if (VAS4[fc3]>=VAS4[fc2]) {	/* einde primair fc2 VOOR einde primair fc3 */

					if ((STAP>=VAS4[fc2]) && (STAP<VAS4[fc3])) hki_gebied23 = TRUE;

				} else {			/* einde primair fc2 NA   einde primair fc3 */

					if ((STAP>=VAS4[fc2]) || (STAP<VAS4[fc3])) hki_gebied23 = TRUE;
				}

				if (!hki_gebied23 && (HVAS[fc3]&BIT3)) HVAS[fc2] |= BIT3;
				if (!hki_gebied23 && (HVAS[fc3]&BIT0) && !(HVAS[fc2]&BIT3)) {
					RW[fc3] |= BIT3;
					RW_hki[i][fc3] |= BIT3;
				}
			}
		}

		if ((VAS2[fc1] != 0) && (VAS4[fc1] != 0) && (VAS2[fc1] != VAS4[fc1]) &&
		    (VAS2[fc2] != 0) && (VAS4[fc2] != 0) && (VAS2[fc2] != VAS4[fc2])) {

			if (VAS4[fc2]>=VAS4[fc1]) {	/* einde primair fc1 VOOR einde primair fc2 */

				if ((STAP>=VAS4[fc1]) && (STAP<VAS4[fc2])) hki_gebied12 = TRUE;

			} else {			/* einde primair fc1 NA   einde primair fc2 */

				if ((STAP>=VAS4[fc1]) || (STAP<VAS4[fc2])) hki_gebied12 = TRUE;
			}

			if (!hki_gebied12 && (HVAS[fc2]&BIT3)) HVAS[fc1] |= BIT3;
			if (!hki_gebied12 && (HVAS[fc2]&BIT0) && !(HVAS[fc1]&BIT3)) {
				RW[fc2] |= BIT3;
				RW_hki[i][fc2] |= BIT3;
			}
		}

		/* Buffer RW[] agv koppeling tbv blokkeren doorsnijden */
		/* --------------------------------------------------- */
		if ((RW[fc2]&BIT3) || B[fc1] || RA[fc1]) PVS2[fc2] |= BIT2;

		if (fc3 >= 0) {
			if ((RW[fc3]&BIT3) || B[fc2] || RA[fc2] || XG[fc2] || PVS2[fc2]) PVS2[fc3] |= BIT2;
		}

		/* Reset BW_hki[][] */
		/* ---------------- */
		if (fc3 >= 0) {
			if (!(G[fc1] || (PVS2[fc1]&BIT4) || T[kptsg12] || T[kpteg12] || RT[kptdm12] || T[kptdm12])) {
				if (!(XG_hki[i][fc2]&BIT1)) {
					if (G[fc3] || !(XG_hki[i][fc3]&BIT1)) BW_hki[i][fc2] = FALSE;
				}
			}
		}

		/* Voorkom onterecht inkomen op SVAS */
		/* --------------------------------- */
		if (SVAS && !G[fc1]) RR[fc1] |= BIT3;

		if (fc3 >= 0) {
			if (SVAS && !G[fc2] && !G[fc1]) RR[fc2] |= BIT3;
		}
	} else {					/* koppeling is niet aktief */
		  XG_hki[i][fc2] &= (~BIT6);
		PVAS_hki[i][fc2] &= (~BIT1);
		if (fc3 >= 0) {
			  XG_hki[i][fc3] &= (~BIT6);
			PVAS_hki[i][fc3] &= (~BIT1);
		}
	}
    }

    for (j=0; j<FCMAX; j++) {
	PVAS[j] &= (~BIT1);				/* bepaal PVAS[]&BIT1 per signaalgroep */
    }

    for (i=0; i<aant_hki; i++) {
	for (j=0; j<FCMAX; j++) {
		if (PVAS_hki[i][j]&BIT1) {
			PVAS[j] |= BIT1;
		}
	}
    }
 }
}

/* PROCEDURE BEPAAL FICTIEVE WACHTSTAND GROEN AANVRAAG */
/* =================================================== */

void proc_wsgr_fca_crsv(void)                /* Fik150101 */
{
 mulv i,j,k;
 mulv fc1,fc2,fc3;
 bool  stat;

 for (i=0; i<FCMAX; i++) {
	if (VA[i] || B[i] || RA[i] || G[i] || GL[i] || TRG[i]) {
	  FCA[i] |= BIT6;                    /* Blok.fic.confl.wachtstand-aanvraag */
	}
	FCA[i] |= BIT7;                      /* Fictieve wachtstand-aanvraag */
 }

 for (i=0; i<aant_hki; i++) {
	fc1 = hki[i][0];
	fc2 = hki[i][1];
	fc3 = hki[i][10];
	stat= hki[i][20];

	if (stat >= 2) {
		if (FCA[fc1]&BIT6) FCA[fc2] |= BIT6;

		if (fc3 >= 0) {
			if (FCA[fc2]&BIT6) FCA[fc3] |= BIT6;
		}
	}

 } 

 for (i=0; i<FCMAX; i++) {
	for (j=0; j<GKFC_MAX[i]; j++) {             /* primaire conflicten */
   		k = TO_pointer[i][j];
   		if (FCA[k]&BIT6) FCA[i] &= (~BIT7);
	}
	for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {   /* fictieve conflicten */
   		k = TO_pointer[i][j];
   		if (FCA[k]&BIT6) FCA[i] &= (~BIT7);
   	}
 }

 for (i=0; i<aant_hki; i++) {
	fc1 = hki[i][0];
	fc2 = hki[i][1];
	fc3 = hki[i][10];
	stat= hki[i][20];

	if (stat >= 2) {
		if (fc3 >= 0) {
			if (!(FCA[fc3]&BIT7)) FCA[fc2] &= (~BIT7);
		}
		if (!(FCA[fc2]&BIT7)) FCA[fc1] &= (~BIT7);
	}
 }

}

/* PROCEDURE AFWIKKELING WACHTSTAND GROEN */
/* ====================================== */

void proc_wsgr_crsv(                         /* Fik150101 */
	mulv fc,                             /* Signaalgroep */
	mulv wgr)                            /* SCH[WGR]     */

{
 bool  wacht = TRUE;

 if (!SCH[wgr] || GL[fc] || TRG[fc] || K[fc] || !(FCA[fc]&BIT7)
	       || (R[fc] && (BAR[fc] || BLW[fc] || RR[fc] || BHK[fc]))) wacht = FALSE;
 if (wacht) {
	if (!BL[fc] || G[fc]) VA[fc] |= BIT5;
 } else { if (!B[fc] && !RA[fc]) VA[fc] &= (~BIT5); }
}

/* PROCEDURE AFWIKKELING MEEVERLENGGROEN */
/* ===================================== */

void proc_mvgr_crsv(                         /* Fik150101 */
	mulv fc,                             /* Signaalgroep */
	mulv mvg)                            /* SCH[MVG]    */

{
 mulv i,j,k,x;
 mulv fc1,fc2,fc3;
 bool  t_gcf;
 bool  stat;

 /* Fik130408 */
 /* Nooit meeverlengen als voedende richting van conflicterende hki reeds RVG() geworden is */

 if (MG[fc] && !BHK[fc]) {
   for (j=0; j<GKFC_MAX[fc]; j++) {
     k = TO_pointer[fc][j];
     for (i=0; i<aant_hki; i++) {
       fc1  = hki[i][0];
       fc2  = hki[i][1];
       fc3  = hki[i][10];
       stat = hki[i][20];
       if (stat >= 2) {
         if ((k == fc3) && RA[fc2] && !RR[fc2] || (k == fc2) && RA[fc1] && !RR[fc1]) BHK[fc] = TRUE;
       }
     }
   }
   for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) {
     k = TO_pointer[fc][j];
     for (i=0; i<aant_hki; i++) {
       fc1  = hki[i][0];
       fc2  = hki[i][1];
       fc3  = hki[i][10];
       stat = hki[i][20];
       if (stat >= 2) {
         if ((k == fc3) && RA[fc2] && !RR[fc2] || (k == fc2) && RA[fc1] && !RR[fc1]) BHK[fc] = TRUE;
       }
     }
   }
 }

 if (VA[fc]&BIT6) {	/* Voedende richting van interne koppeling */

	t_gcf = TRUE;
	for (j=0; j<GKFC_MAX[fc]; j++) {
   	  k = TO_pointer[fc][j];
   	  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
	}
	for (j=GKFC_MAX[fc]; j<FKFC_MAX[fc]; j++) {
   	  k = TO_pointer[fc][j];
   	  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
	}

	for (i=0; i<aant_hki; i++) {
	  stat = hki[i][20];
	  if (stat >= 2) {
	    if (fc == hki[i][0]) {
		x = hki[i][1];
		for (j=0; j<GKFC_MAX[x]; j++) {
   		  k = TO_pointer[x][j];
   		  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
		}
		for (j=GKFC_MAX[x]; j<FKFC_MAX[x]; j++) {
   		  k = TO_pointer[x][j];
   		  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
		}

		x = hki[i][10];
		if (x >= 0) {
			for (j=0; j<GKFC_MAX[x]; j++) {
   			  k = TO_pointer[x][j];
   			  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
			}
			for (j=GKFC_MAX[x]; j<FKFC_MAX[x]; j++) {
   			  k = TO_pointer[x][j];
   			  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
			}
		}
	    }
	    if (fc == hki[i][1]) {
		x = hki[i][10];
		if (x >= 0) {
			for (j=0; j<GKFC_MAX[x]; j++) {
   			  k = TO_pointer[x][j];
   			  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
			}
			for (j=GKFC_MAX[x]; j<FKFC_MAX[x]; j++) {
   			  k = TO_pointer[x][j];
   			  if (R[k] && !TRG[k] && VA[k] && !BAR[k]) t_gcf = FALSE;
			}
		}
	    }
	  }
	}

	if (SCH[mvg] && !somfbk_crsv(fc) && !BHK[fc] && t_gcf && hfkp_mvg()) YM[fc] |= BIT0;
 }
 else {			/* Overige signaalgroepen */

	if (SCH[mvg] && !somfbk_crsv(fc) && !BHK[fc]) YM[fc] |= BIT0;
 }
}

/* PROCEDURE RESET VARIABELEN BIJ EINDE SIGNAALGROEPSTURING */
/* ======================================================== */

void proc_reset_crsv_einde(void)               /* Fik150101 */
{
 mulv i;

 for (i=0; i<FCMAX; i++) {
   FCA[i] &= (~BIT6);
   FCA[i] &= (~BIT7);
   if ((B[i] || RA[i]) && !VA[i]) RR[i] = TRUE;
   if ( R[i] && !A[i]  && !VA[i]) TFB_timer[i] = 0;
   if ((TS || N_ts) && _TTR_HK[i] && (TTR_HK[i] > 0)) TTR_HK[i]--;
 }

 if (SVAS)           RTFB = TRUE;
 if (Straag || HFIX) HTFB = TRUE;
}

/* PROCEDURE RESET BUFFERS PELOTON KOPPELING */
/* ========================================= */

void proc_vas_res_pel(         /* Fik100314  */
 count ap1,                    /* awt peloton 1 */
 count ap2,                    /* awt peloton 2 */
 count ap3)                    /* awt peloton 3 */
{
 if (AKTPRG > 0) {
   MM[ap1] = MM[ap2] = MM[ap3] = 0;
 }
}

/* PROCEDURE BEPAAL TIJD TOT PRIMAIR GROEN, TTPG[] TBV WT-VOORSPELLER */
/* ================================================================== */

void proc_ttpg_crsv(void)                     /* Fik150101 */
{
 mulv i,j,k;
 bool ppv;

 for (i=0; i<FCMAX; i++) {

	CON_BUS[i] = FALSE;
	if (AKTPRG) {			 /* Bepaal TTPG[]   */
	  if (G[i]) { TTPG[i] = FALSE; } /* 0: indien groen */
	  else {
	    if (GL[i] || R[i] && !VA[i] && !(HVAS[i]&BIT5) || BL[i] ||
	       (VAS2[i] == 0) || (VAS4[i] == 0) || (VAS2[i] == VAS4[i])) {
		 TTPG[i] = 2550;	 /* 2550: indien geel       */
					 /* OF rood zonder aanvraag */
					 /* OF blokkeer_optie AAN   */
					 /* OF geen prim.realisatie */
	    } else {
	      /* Bepaal TTPG[] ogv stappenraster */

	      /* if (TS || SVAS) {  iedere machineslag aanroepen */

		if (STAP<VAS2[i]) { TTPG[i] = (VAS2[i] - STAP)*10; }
		else {
		  if (STAP == VAS2[i]) { TTPG[i] = 0; }
	       	  else { TTPG[i] = (CTYD - STAP + VAS2[i])*10; }
		}

		/* Corrigeer indien versneld afgesloten */

		if (HVAS[i]&BIT3) {
		 if ((VAS4[i]>VAS2[i]) && ((STAP<=VAS2[i]) || (STAP>=VAS4[i]))
		 ||  (VAS4[i]<VAS2[i]) &&  (STAP<=VAS2[i]) && (STAP>=VAS4[i])){
			TTPG[i] += (CTYD*10);
		 }
		}

		/* Corrigeer indien TTP[] verder ligt agv een busprioriteit */

		if (!(HVAS[i]&BIT3) && (TTP[i]<255) && ((TTP[i]*10)>TTPG[i])) {
			TTPG[i] = (TTP[i]*10);
#ifdef schhaltwtv
			if (SCH[schhaltwtv]) CON_BUS[i] = TRUE;
#endif
		}

		/* Corrigeer indien versnelde realisatie aktief */

		ppv = TRUE;
		for (j=0; j<GKFC_MAX[i]; j++) {
		  k = TO_pointer[i][j];
		  if (TTP[i]>TTP[k]) ppv = FALSE;
		}
		for (j=GKFC_MAX[i]; j<FKFC_MAX[i]; j++) {
		  k = TO_pointer[i][j];
		  if (TTP[i]>TTP[k]) ppv = FALSE;
		}
		if ((RV[i] && B[i] && VA[i] || RA[i]) && !RR[i]
			&& ((VA[i]&BIT4) || ppv) /* && TS */
			&& (TTPG[i] > TTR_WV[i])) TTPG[i] = TTR_WV[i];

		/* Corrigeer indien UITSTEL aktief */
		if (TTPG[i]<(TTU[i]*10)) TTPG[i] = (TTU[i]*10);

		/* Indien RA[] dan altijd aftellen op basis van TTR_WV[] */
		if (RA[i] && !SRA[i] && !RR[i] && !BL[i]) TTPG[i] = TTR_WV[i];

	   /* } */
	    }
	  }
	} else { TTPG[i] = 2550; }	/* 2550: indien niet CRSV */
 }
 /* SPR = SVAS = FALSE;	-> verplaatst naar reset_einde() in Traffick */
}

/* BEPAAL AANTAL LAMPJES WACHTTIJD VOORSPELLER */
/* =========================================== */

void proc_wt_voorspeller_crsv(void)          /* Fik150101 */
{
   mulv i,rest;

   for (i=0; i<FCMAX; i++) {
	if (WTV_FC[i]) {
		if (HALT[i] > 0) {
			if (HALT[i] > TE) HALT[i] -= TE;
			else              HALT[i] = 0;
		}

		if (BL[i] || CON_BUS[i] || HFIX) HALT[i] = 10;
		if (G[i] || GL[i]) Aled[i] = 0;
		if (R[i] && (VA[i]&BIT1) && !(VA[i]&BIT7) && (Aled[i] == 0)) {
			if (RR[i] || RV[i] && !B[i] && !(VA[i]&BIT5) || RA[i] && !SRA[i] || (TFB_timer[i] >= 2)) {
				Aled[i] = 31;
				Adwt[i] = 0;
			}
		} else {
			if (TE && (Adwt[i] < 600)) Adwt[i] += TE;
			if ((TTPG[i] < 2550) && !BL[i] && (Aled[i] > 0) && (!HALT[i] && !HFIX || (XG[i]&BIT7) && !somgk_crsv(i))) {
				Tdwt[i] = (TTPG[i]+Adwt[i])/Aled[i];
				rest    = (TTPG[i]+Adwt[i])%Aled[i];
				if ((2*rest) >= Aled[i]) Tdwt[i]++;
				if (Tdwt[i] < 1)   Tdwt[i] = 1;
				if (Tdwt[i] > 600) Tdwt[i] = 600;
				if (Adwt[i] >= Tdwt[i]) {
					if (Aled[i] > 1) Aled[i]--;
					Adwt[i] = 0;
				}
			}
		}
		halteer_wtv[i] = CON_BUS[i] && (Aled[i] > 0) && (!(XG[i]&BIT7) || somgk_crsv(i));

	} else {		/* Geen wachttijdvoorspeller! */
		Aled[i]        = 0;
		halteer_wtv[i] = FALSE;
		HALT[i]        = 0;
		Adwt[i]        = Tdwt[i] = 0;
	}
   }
}

/* PROCEDURE KOPPELING CRSV-MODULE -> CRS */
/* ====================================== */

void proc_crsv_to_ccol(void)                 /* Fik150101 */
{
 mulv i;

 proc_reset_crsv_einde();

 for (i=0; i<FCMAX; i++) {

   /* Bepaal aanvraag C-regelaar */
   /* -------------------------- */
   if (R[i] && VA[i]) A[i] |= BIT0;

   /* Bepaal aanhouden VAG2e(i) */
   /* ------------------------- */
   if (G[i] && (RW[i] || YV[i]) && !Z[i]) VAG_OP[i] = TRUE;
   else					  VAG_OP[i] = FALSE;

   if (!MG[i] && (RW[i] == BIT2)) {  /* Vasthouden groen OV ingreep in VG[] */
     RW[i] &= (~BIT2);
     YV[i] |= BIT2;
   }

   /* Bepaal aanhouden MG[i] */
   /* ---------------------- */
   if (G[i] && HFIX) {
     YM[i] |= BIT0;
     Z[i] = FALSE;
   }

   /* Wachtstandgroen */
   /* --------------- */
   if (G[i] && (VA[i]&BIT5) && (VG[i] || MG[i])) RW[i] |= BIT5;
   if (G[i] && (VA[i]&BIT5)                    ) YW[i] |= BIT5;
 }

 proc_ttpg_crsv();
 proc_wt_voorspeller_crsv();
}

/* PROCEDURES TBV COMBINERING FUNCTIE-AANROEPEN */
/* ============================================ */

/* Combinatie signaalgroepsturing 1e gedeelte */
/* ------------------------------------------ */

void CRSV_FC_main_1(void)
{
 proc_corr_kapm();
 proc_bteg_crsv();
 proc_bttr_crsv();
 proc_bttp_crsv();
 proc_prio_crsv();
 proc_prim_crsv();
 proc_altt_crsv();
}

/* Combinatie signaalgroepsturing 2e gedeelte */
/* ------------------------------------------ */

void CRSV_FC_main_2(void)
{
 mulv i;

 proc_altr_crsv();
 
 for (i=0; i<aant_fts; i++) {
   proc_vtg_fts_crsv(fts[i][0],fts[i][1],fts[i][2],fts[i][3],fts[i][4]);
 }
 for (i=0; i<aant_bbv; i++) {
   proc_bb_vtg_crsv(bbv[i][0],bbv[i][1],bbv[i][2],bbv[i][4],bbv[i][5]
					,bbv[i][6],bbv[i][7]);
 }
 for (i=0; i<aant_gov; i++) {
   proc_go_vtg_crsv(gov[i][0],gov[i][1],gov[i][2],gov[i][3],gov[i][4]
		      ,gov[i][5],gov[i][7],gov[i][8],gov[i][9],gov[i][10]
		      ,gov[i][11],gov[i][12],gov[i][13],gov[i][14],gov[i][15]
		      ,gov[i][16],gov[i][17],gov[i][18],gov[i][19],gov[i][20]
		      ,gov[i][22],gov[i][23],gov[i][24],gov[i][25],gov[i][26],gov[i][27],gov[i][28]);
 }
 proc_hki_crsv();
 proc_wsgr_fca_crsv();
}
