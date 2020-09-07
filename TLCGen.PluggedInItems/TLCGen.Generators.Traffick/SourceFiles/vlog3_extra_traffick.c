/* DECLARATIE VLOG-3 FUNCTIES */
/* ========================== */
/*
 *  Huijskes VRT B.V.
 *  14 Oktoberplein 6
 *  Overloon
 *
 *
 *  Module: functies t.b.v. VLOG-3 omgevings factoren, en reden voor wachttijd
 *
 *  Filenaam        : hvrt_vlog3_extra.c
 *  Ontwerp         : Marc Scherjon
 *  Datum           : 19-01-2018
 *  CCOL            : versie 9.0 en hoger,  Generator PNH versie ?? en hoger
 *
  Revisie:
    09-10-2017 M. Scherjon:  opzet
    18-10-2017 M. Scherjon:  CIF_OMGEVING[1] gewijzigd in CIF_OMGEVING[0].
    19-01-2018 M. Scherjon:  ongebruikte redenen in commentaar gezet
    25-07-2018 P. Huijskes:  Reden voor wachttijd alleen bij A[] && R[]

*/

/* macrodefinities */ 
/* =============== */ 
	#define VLOG3_EXTRA                   /* vrijgave functies in deze file. Eventueel in systemapplication2() toepassen */

/*  #define RWT_NO_TOVERGROEN   */ /* Deze actief zetten indien de varibale TOV[] NIET aanwezig is !! */

/* include files */ 
/* ============= */ 
  #include <stdarg.h>                 /* gebruik variabele argumentenlijst                                */ 
  #include "sysdef.c"                 /* systeemdefinities                                                */ 

/*  #include "dpvar.h"                     detectie                                                         */
  #include "isvar.h"                    /* ingangssignalen                                                  */
  #include "fcvar.h"                    /* fasecycli                                                        */
/*  #include "usvar.h"                     uitgangen                                                        */
  #include "kfvar.h"                    /* konflikten                                                       */
/*  #include "schvar.h"                    schakelaars                                                      */
/*  #include "ctvar.h"                     counters                                                         */
/*  #include "mevar.h"                     memory elementen                                                 */
/*  #include "tmvar.h"                     tijd   elementen                                                 */
/*  #include "prmvar.h"                    vrije parameters                                                 */
/*  #include "hevar.h"                     hulp elementen                                                   */
/*  #include "mlvar.h"                     modulen                                                          */
#ifndef NO_VLOG
/*  #include "vlogvar.h"                   VLOG functies                                                    */
/*  #include "logvar.h"                    logging                                                          */
/*  #include "monvar.h"                    real time monitoring                                             */
#endif                                  /* *** ifdef -> AUTOMAAT                                            */
/*  #include "dsivar.h"                    selectieve detectie (variabelen)                                 */
/*  #include "prsvar.h"                    structuur                                                        */

/*  #include "rtappl.h"                    Standaard ccol routines                                          */
  #include "cif.inc"                    /* CVN interface                                                    */
/*  #include "bericht.h"                   cif_uber() functies  */

#ifndef AUTOMAAT
/*  #include "dplwvar.h"                   t.b.v. tekenfuncties  achteraan in system_application() */
#endif

/*  #include <ctype.h>                     isspace(), isdigit()		 	*/


#ifndef HVRT_PRMTEST
/*  #include "hvrt_prmtest.c"              Functies t.b.v. testen en corrigeren van parameterinstellingen */
#endif

/* declaratie globale variabelen */
/* ============================= */
/* zie .h file */

/* declaraties van functies */
/* ======================== */
#ifndef VLOG3_EXTRA_H
/*  #include "vlog3_extra.h"  */    /* Functie declaraties */
#endif

void proc_omgevingsfactor(bool regen, bool mist, bool gladheid);

void proc_reden_voor_wachtijd(void);
mulv reden_voor_wachttijd_fc(count fc);


/* definities van functies */
/* ======================= */

/* 
   Uit cif.inc :

 OMGEVINGSFACTOREN 
 ================= 

 macrodefinities omgevingsfactoren 
 --------------------------------- 
   #define CIF_OMGEVING_REGEN       1     BIT0 - Regen             
   #define CIF_OMGEVING_MIST        2     BIT1 - Mist              
   #define CIF_OMGEVING_GLADHEID    4     BIT2 - Kans op gladheid  

 omgevingsfactoren - databuffer 
 ------------------------------ 
   #define CIF_MAX_OMGEVING         1 	 grootte CIF_OMGEVING[]-buffer 

#ifdef CIF_PUBLIC
   s_int16 CIF_OMGEVING[CIF_MAX_OMGEVING];
#else
  extern s_int16 CIF_OMGEVING[];
#endif

 ********************************************************************************************	

 REDEN VOOR EXTRA WACHTTIJD 
 ========================== 

 macrodefinities reden voor extra wachttijd  
 ------------------------------------------- 
   #define CIF_FC_RWT_OV_INGREEP            	    1    BIT0 - openbaar vervoer ingreep 
   #define CIF_FC_RWT_HULPDIENST_INGREEP    	    2    BIT1 - hulpdienst ingreep       
   #define CIF_FC_RWT_TREIN_INGREEP         	    4    BIT2 - trein ingreep		 
   #define CIF_FC_RWT_BRUG_INGREEP          	    8    BIT3 - brug ingreep         	 
   #define CIF_FC_RWT_HOOGTEMELDING        		   16    BIT4 � hoogtemelding		 
   #define CIF_FC_RWT_WEERSINGREEP         		   32    BIT5 � weersingreep	  	 
   #define CIF_FC_RWT_FILE_INGREEP         		   64    BIT6 � tunnelingreep		 
   #define CIF_FC_RWT_TUNNEL_AFSLUITING   	    128    BIT7 � tunnelafsluiting	 
   #define CIF_FC_RWT_DOSEREN_ACTIEF      	    256    BIT8 - doseren actief		 
   #define CIF_FC_RWT_PRIORITEIT_VRACHTVERKEER  512    BIT9 � tunnelafsluiting	 
   #define CIF_FC_RWT_PRIORITEIT_FIETSVERKEER  1024    BIT10 - doseren actief		 
   #define CIF_FC_RWT_REDEN_ONBEKEND          16384    BIT14 - reden onbekend		 


 reden voor extra wachttijd - databuffer 
 --------------------------------------- 
#ifdef CIF_PUBLIC
   s_int16 CIF_FC_RWT[CIF_AANT_US_FC];
#else
  extern s_int16 CIF_FC_RWT[];
#endif

*/


void proc_omgevingsfactor(bool regen, bool mist, bool gladheid)
{

#ifndef NO_CVN_50
  CIF_OMGEVING[0] = FALSE;

  if ( regen > 0 )      CIF_OMGEVING[0] |= CIF_OMGEVING_REGEN;
  if ( mist > 0 )       CIF_OMGEVING[0] |= CIF_OMGEVING_MIST;
  if ( gladheid > 0 )   CIF_OMGEVING[0] |= CIF_OMGEVING_GLADHEID;

#endif

}




void proc_reden_voor_wachtijd(void)
{

#ifndef NO_CVN_50
	count i;

  for ( i = 0; i < FCMAX; i++ ) {
    CIF_FC_RWT[i] = reden_voor_wachttijd_fc((count) i);
  }
#endif

}




mulv reden_voor_wachttijd_fc(count fc)
{

#ifndef NO_CVN_50
	count i,k;
#endif

  mulv result;

  result = 0;

#ifndef NO_CVN_50
  for ( i = 0; i < FKFC_MAX[fc]; i++ ) {
	  if (A[fc] && R[fc]) {
    k = TO_pointer[fc][i];

    if ( HLPD[k][0] && BL[fc] )     result |= CIF_FC_RWT_HULPDIENST_INGREEP;
/*  if ( FALSE         )            result |= CIF_FC_RWT_TUNNEL_AFSLUITING; */
    if ( SGD[k] & BIT5 ) {
      if ( POV[k] & BIT5 )          result |= CIF_FC_RWT_OV_INGREEP;
      if ( _FIL[k] & BIT5 )         result |= CIF_FC_RWT_FILE_INGREEP;
      if ( FTS[k] & BIT5 )          result |= CIF_FC_RWT_PRIORITEIT_FIETSVERKEER;
      if ( PEL[k] & BIT5 )          result |= CIF_FC_RWT_REDEN_ONBEKEND;
      if ( G[k] && FIXATIE )        result |= CIF_FC_RWT_REDEN_ONBEKEND;
#ifndef RWT_NO_TOVERGROEN
      if ( TOV[k] & BIT5 )          result |= CIF_FC_RWT_PRIORITEIT_VRACHTVERKEER;
#endif
    }
  }
  }

  for ( k = 0; k < FC_MAX; k++ ) {
	  if (A[fc] && R[fc]) {
    if ( TMPc[k][fc] ) {

      if ( HLPD[k][0] && BL[fc] )   result |= CIF_FC_RWT_HULPDIENST_INGREEP;
/*    if ( FALSE         )          result |= CIF_FC_RWT_TUNNEL_AFSLUITING; */
      if ( SGD[k] & BIT5 ) {
        if ( POV[k] & BIT5 )        result |= CIF_FC_RWT_OV_INGREEP;
        if ( _FIL[k] & BIT5 )       result |= CIF_FC_RWT_FILE_INGREEP;
        if ( FTS[k] & BIT5 )        result |= CIF_FC_RWT_PRIORITEIT_FIETSVERKEER;
        if ( PEL[k] & BIT5 )        result |= CIF_FC_RWT_REDEN_ONBEKEND;
        if ( G[k] && FIXATIE )      result |= CIF_FC_RWT_REDEN_ONBEKEND;
#ifndef RWT_NO_TOVERGROEN
        if ( TOV[k] & BIT5 )        result |= CIF_FC_RWT_PRIORITEIT_VRACHTVERKEER;
#endif
      }
    }
  }
  }

  if ( A[fc] && R[fc] && _FIL[fc] & BIT1 )     result |= CIF_FC_RWT_FILE_INGREEP;
  if ( A[fc] && R[fc] && _FIL[fc] & BIT0 )     result |= CIF_FC_RWT_DOSEREN_ACTIEF;
/*  if ( FALSE             )          result |= CIF_FC_RWT_TREIN_INGREEP; */
/*  if ( FALSE             )          result |= CIF_FC_RWT_BRUG_INGREEP; */
  if ( A[fc] && R[fc] && K[fc] && FIXATIE )    result |= CIF_FC_RWT_REDEN_ONBEKEND;
#endif

  return(result);

}