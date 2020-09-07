/* RIS INTERFACE: PROCESBESTURING - APPLICATIEPROGRAMMA */
/* ==================================================== */

/* RIS  :  versie 1.0    */
/* FILE :  rif.inc       */
/* DATUM:  12-09-2018    */

#ifndef _RIFDEF_INC
#define _RIFDEF_INC

/* include file */
/* ------------ */
  #include "rif_defs.h" /* declaratie RIS Interface objecten */


/* Definities grootte databuffers */
/* ------------------------------ */
   #define RIF_MAX_ITSSTATION_PB   100   /* Grootte buffer interface t.b.v. CAM berichten procesbesturing    */
   #define RIF_MAX_PRIOREQUEST_PB   25   /*                          t.b.v. SRM berichten procesbesturing    */
   #define RIF_MAX_ACTIVEPRIO_AP    10   /*                          t.b.v. SSM berichten applicatie         */
   #define RIF_MAX_ITSEVENT_AP      10   /*                          t.b.v. DENM berichten applicatie        */
   #define RIF_MAX_ITSEVENT_PB      10   /*                          t.b.v. DENM berichten procesbesturing   */

#ifdef RIF_PUBLIC
   const rif_int RIF_ITSSTATION_PB_MAX  = RIF_MAX_ITSSTATION_PB;
   const rif_int RIF_PRIOREQUEST_PB_MAX = RIF_MAX_PRIOREQUEST_PB;
   const rif_int RIF_ACTIVEPRIO_AP_MAX  = RIF_MAX_ACTIVEPRIO_AP;
   const rif_int RIF_ITSEVENT_AP_MAX    = RIF_MAX_ITSEVENT_AP;
   const rif_int RIF_ITSEVENT_PB_MAX    = RIF_MAX_ITSEVENT_PB;
#else
   extern const rif_int RIF_ITSSTATION_PB_MAX;
   extern const rif_int RIF_PRIOREQUEST_PB_MAX;
   extern const rif_int RIF_ACTIVEPRIO_AP_MAX;
   extern const rif_int RIF_ITSEVENT_AP_MAX;
   extern const rif_int RIF_ITSEVENT_PB_MAX;
#endif


/* ITSSTATION_PB[]-buffer (CAM) */
/* ---------------------------- */
/* De buffer waar alle ITS stations in worden gezet. */
/* Deze wordt geschreven door de PB en gelezen door de AP. */

#ifdef RIF_PUBLIC
   struct Rif_ItsStation RIF_ITSSTATION_PB[RIF_MAX_ITSSTATION_PB];

   rif_int RIF_ITSSTATION_PB_WRITE;  /* Te schrijven door de PB */
   rif_int RIF_ITSSTATION_PB_READ;   /* Te schrijven door de AP */
#else
   extern struct Rif_ItsStation RIF_ITSSTATION_PB[];

   extern rif_int RIF_ITSSTATION_PB_WRITE;
   extern rif_int RIF_ITSSTATION_PB_READ;
#endif

/* PRIOREQUEST_PB[]-buffer (SRM) */
/* ----------------------------- */
/* De buffer waar alle prioritization requests in worden gezet. */
/* Deze wordt gelezen door de AP en geschreven door de PB. */

#ifdef RIF_PUBLIC
   struct Rif_PrioritizationRequest RIF_PRIOREQUEST_PB[RIF_MAX_PRIOREQUEST_PB];

   rif_int RIF_PRIOREQUEST_PB_WRITE; /* Te schrijven door de PB */
   rif_int RIF_PRIOREQUEST_PB_READ;  /* Te schrijven door de AP */
#else
   extern struct Rif_PrioritizationRequest RIF_PRIOREQUEST_PB[];

   extern rif_int RIF_PRIOREQUEST_PB_WRITE;
   extern rif_int RIF_PRIOREQUEST_PB_READ;
#endif

/* ACTIVEPRIO_AP[]-buffer (SSM) */
/* ---------------------------- */
/* De buffer waar alle active prioritizations in worden gezet. */
/* Deze wordt geschreven door de AP en gelezen door de PB. */

#ifdef RIF_PUBLIC
   struct Rif_ActivePrioritization RIF_ACTIVEPRIO_AP[RIF_MAX_ACTIVEPRIO_AP];

   rif_int RIF_ACTIVEPRIO_AP_READ;  /* Te schrijven door de PB */
   rif_int RIF_ACTIVEPRIO_AP_WRITE; /* Te schrijven door de AP */
#else
   extern struct Rif_ActivePrioritization RIF_ACTIVEPRIO_AP[];

   extern rif_int RIF_ACTIVEPRIO_AP_READ;
   extern rif_int RIF_ACTIVEPRIO_AP_WRITE;
#endif

/* ITSEVENT_AP[]-buffer (DENM) */
/* --------------------------- */
/* De buffer waar alle ITS events vanuit de AP in worden gezet. */
/* Deze wordt geschreven door de AP en gelezen door de PB. */

#ifdef RIF_PUBLIC
   struct Rif_ItsEvent RIF_ITSEVENT_AP[RIF_MAX_ITSEVENT_AP];

   rif_int RIF_ITSEVENT_AP_READ;  /* Te schrijven door de PB */
   rif_int RIF_ITSEVENT_AP_WRITE; /* Te schrijven door de AP */
#else
   extern struct Rif_ItsEvent RIF_ITSEVENT_AP[];

   extern rif_int RIF_ITSEVENT_AP_READ;
   extern rif_int RIF_ITSEVENT_AP_WRITE;
#endif

/* ITSEVENT_PB[]-buffer (DENM) */
/* --------------------------- */
/* De buffer waar alle ITS events vanuit de PB in worden gezet. */
/* Deze wordt geschreven door de PB en gelezen door de AP. */
/* De events die hier in komen zijn alleen de events van andere applicaties. */

#ifdef RIF_PUBLIC
   struct Rif_ItsEvent RIF_ITSEVENT_PB[RIF_MAX_ITSEVENT_PB];

   rif_int RIF_ITSEVENT_PB_WRITE; /* Te schrijven door de PB */
   rif_int RIF_ITSEVENT_PB_READ;  /* Te schrijven door de AP */
#else
   extern struct Rif_ItsEvent RIF_ITSEVENT_PB[];

   extern rif_int RIF_ITSEVENT_PB_WRITE;
   extern rif_int RIF_ITSEVENT_PB_READ;
#endif

#endif /* _RIFDEF_INC */
