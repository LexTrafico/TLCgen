/* HEADER FILE RIS FUNCTIES */
/* ======================== */
/*
 *	Huijskes VRT B.V.
 *	14 Oktoberplein 6
 *	Overloon
 *
 *	Module: functies t.b.v. afhandeling van RIS-interface berichten, en functies t.b.v. ris voertuigen in applicatie
		22-08-2018 P. Huijskes: Versie 2.00 aanpassing naar RIF interface
 *
 */

/* extern int RIF_ITSSTATION_PB_WRITE; */
/* extern int RIF_ITSSTATION_PB_READ;  */


/* Rijstrook definities */
	#define KRUISVLAK		BIT0
	#define RIJSTROOK_1		BIT1
	#define RIJSTROOK_2		BIT2
	#define RIJSTROOK_3		BIT3
	#define RIJSTROOK_4		BIT4
	#define RIJSTROOK_5		BIT5
	#define RIJSTROOK_6		BIT6
	#define RIJSTROOK_7		BIT7
	#define RIJSTROOK_8		BIT8

	#define RIJSTROOK_ALLE  (BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7|BIT8)

/* RIS declaraties */
/* --------------- */
	#define OPVV_MAX 10
	#define HLPD_MAX 10
	#define TOVERMAX 10

	#define ZWAAILICHT BIT0
	#define SIRENE     BIT1

	#define MIN_AFSTAND 6

	/* @@ MS 12-1-2018 : om te voorkomen dat voertuigen zonder update, te snel 'bewegen' */
	/* Om te beginnen, geen correctie = 100 % */
	#define CORRETION_SPEED								100  /* percentage snelheid */
	#define CORRETION_ACCELERATION_UP			100  /* percentage acceleratie indien versnellen */
	#define CORRETION_ACCELERATION_DOWN		100  /* percentage acceleratie indien vertragen */

/* Definities CCOL-applicatie - stond voorheen in ris.inc */
/* ------------------------------------------------------ */
  #define RIF_MAX_PRIO_REQUEST_AP  50   /*                          t.b.v. SRM berichten applicatie         */
 
#ifdef CIF_PUBLIC
  #define RIF_MAX_ITSSTATION_AP   FCMAX * 50
#else
  #define RIF_MAX_ITSSTATION_AP   400
#endif
 
  #define RIF_ITSMAXAANTAL         20      /* Maximum aantal buffers per machineslag ophalen */
	
  int BUFFER_pointer[RIF_MAX_ITSSTATION_AP];

  bool new_cam_data = FALSE;
  bool new_srm_data = FALSE;

  int new_cam_data_count = 0;
  int new_srm_data_count = 0;


  /* Datablok voor verwijzingen naar... */
struct _abon {
	int vag1_o;	/* Ondergrens */
	int vag1_b;	/* Bovengrens */
	int vag1_voertuig;	/* Voertuigtype */
	int vag1_p;	/* Aantal voertuigen */
	int vag2_o;	
	int vag2_b;
	int vag2_voertuig;
	int vag2_p;
	int vag3_o;	
	int vag3_b;
	int vag3_voertuig;
	int vag3_p;
	int vag4_o;	
	int vag4_b;
	int vag4_voertuig;
	int vag4_p;
	int tover_o;
	int tover_b;
	int tover_voertuig;
	int tover_p;
	int opvv_o;
	int opvv_b;
	int opvv_voertuig;
	int opvv_p;
	int hlpd_o;
	int hlpd_b;
	int hlpd_voertuig;
	int hlpd_p;
	int peloton_o;
	int peloton_b;
	int peloton_voertuig;
	int peloton_p;
	int vag1[RIF_MAX_ITSSTATION_AP];	/* Verwijzing naar index */
	int vag2[RIF_MAX_ITSSTATION_AP];
	int vag3[RIF_MAX_ITSSTATION_AP];
	int vag4[RIF_MAX_ITSSTATION_AP];
	int tover[RIF_MAX_ITSSTATION_AP];
	int opvv[RIF_MAX_ITSSTATION_AP];
	int hlpd[RIF_MAX_ITSSTATION_AP];
	int peloton[RIF_MAX_ITSSTATION_AP];
};

struct _tover {	/* Bij toevoeging van een element, dit in de functie bep_tovergroen_4_traffick(void) op 3 plekken toevoegen */
	rif_string ObjectID;
	double Speed;
	double acceleration; /* - 16.0 to 16.0 */
	double distance;
	bool vasthouden;
	int groenbewaking;
	bool vlag;			/* vlag staat een machineslag op bij een nieuw voertuig in het traject */
};

struct _opvv {	/* Bij toevoeging van een element, dit in de functie bep_opvv_4_traffick(void) op 3 plekken toevoegen */
	rif_string ObjectID;
	double Speed;
	double acceleration; /* - 16.0 to 16.0 */
	double distance;
	s_int16 stiptheid;
	int lijnnummer;
	bool vasthouden;
	int groenbewaking;
	bool s_vlag;			/* vlag staat een machineslag op bij een nieuw voertuig in het traject */
	bool e_vlag;
};

struct _hlpd {	/* Bij toevoeging van een element, dit in de functie bep_hlpd_4_traffick(void) op 3 plekken toevoegen */
	rif_string ObjectID;
	double Speed;
	double acceleration; /* - 16.0 to 16.0 */
	double distance;
	bool vasthouden;
	int groenbewaking;
	bool s_vlag;			/* vlag staat een machineslag op bij een nieuw voertuig in het traject */
	bool e_vlag;
};


struct _opvv  OPVV_buffer[FCMAX][HLPD_MAX];
struct _hlpd  HLPD_buffer[FCMAX][HLPD_MAX];
struct _tover TOVER_buffer[FCMAX][TOVERMAX];
struct _abon  _ABON[FCMAX];


struct Rif_ItsStation ITSSTATION_AP[RIF_MAX_ITSSTATION_AP];
struct Rif_ItsStation ITSSTATION_AP_corr[RIF_MAX_ITSSTATION_AP];

struct Rif_PrioritizationRequest PRIOREQUEST_AP[RIF_MAX_PRIO_REQUEST_AP];
struct Rif_PrioritizationRequest PRIOREQUEST_AP_corr[RIF_MAX_PRIO_REQUEST_AP];


int dagmaand[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


void RIS_ssm_bericht(rif_string ObjectID, int prioritizationState);



  extern  int64_t time_2_nu(void);
  extern void wissen_cam(struct Rif_ItsStation a[], int i, const int max);
  extern void wissen_srm(struct Rif_PrioritizationRequest a[], int i, const int max);

  extern void ris_update_ap(struct Rif_ItsStation a[], int i, const int max);
  extern void ris_update_ap_srm(struct Rif_PrioritizationRequest a[], int i, const int max);

  extern bool valid_objectid(rif_string id);
