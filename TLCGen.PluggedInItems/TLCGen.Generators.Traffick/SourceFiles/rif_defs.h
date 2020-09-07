/* RIS INTERFACE: PROCESBESTURING - APPLICATIEPROGRAMMA */
/* ==================================================== */

/* RIS  :  versie 1.0    */
/* FILE :  rif_defs.h    */
/* DATUM:  12-09-2018    */

#ifndef _RIF_DEFS_H
#define _RIF_DEFS_H

/* include file */
/* ============ */
#include <stdint.h>

/* Typen variabelen */
/* ================ */
#define RIF_FALSE         0
#define RIF_TRUE          1
#define RIF_STRINGLENGTH 63

typedef uint64_t rif_timestamp;   /* uint64_t defined in: stdint.h */
typedef int      rif_int;
typedef double   rif_float;
typedef short    rif_bool;
typedef char     rif_string[RIF_STRINGLENGTH + 1];

/* Definitie array grootte */
/* ======================= */
#define RIF_MAXLANES  4    /* grootte array matches[RIF_MAXLANES] voor CAM berichten  */
#define RIF_MAXTRACES 5    /* grootte array traces[RIF_MAXTRACES] voor DENM berichten */
#define RIF_MAXPOINTS 10   /* grootte array points[RIF_MAXPOINTS] voor DENM berichten */

/* Type definities */
/* =============== */

/* StationType */
/* ----------- */
enum Rif_StationType {
    RIF_STATIONTYPE_UNKNOWN         =  0,
    RIF_STATIONTYPE_PEDESTRIAN      =  1,
    RIF_STATIONTYPE_CYCLIST         =  2,
    RIF_STATIONTYPE_MOPED           =  3,
    RIF_STATIONTYPE_MOTORCYCLE      =  4,
    RIF_STATIONTYPE_PASSENGERCAR    =  5,
    RIF_STATIONTYPE_BUS             =  6,
    RIF_STATIONTYPE_LIGHTTRUCK      =  7,
    RIF_STATIONTYPE_HEAVYTRUCK      =  8,
    RIF_STATIONTYPE_TRAILER         =  9,
    RIF_STATIONTYPE_SPECIALVEHICLES = 10,
    RIF_STATIONTYPE_TRAM            = 11,
    RIF_STATIONTYPE_ROADSIDEUNIT    = 15
};

/* VehicleRole */
/* ----------- */
enum Rif_VehicleRole {
    RIF_VEHICLEROLE_DEFAULT          =  0,   /* Default vehicle role as indicated by the vehicle type. */
    RIF_VEHICLEROLE_PUBLICTRANSPORT  =  1,   /* Vehicle is used to operate public transport service. */
    RIF_VEHICLEROLE_SPECIALTRANSPORT =  2,   /* Indication for special transport, e.g. oversized trucks. */
    RIF_VEHICLEROLE_DANGEROUSGOODS   =  3,   /* Vehicle used for dangerous goods transportation. */
    RIF_VEHICLEROLE_ROADWORK         =  4,   /* Vehicle used to realize roadwork or road maintenance mission. */
    RIF_VEHICLEROLE_RESCUE           =  5,   /* Vehicle used for rescue purposes, e.g. as a towing service. */
    RIF_VEHICLEROLE_EMERGENCY        =  6,   /* Vehicle used for emergency mission, e.g. ambulance, fire brigade. */
    RIF_VEHICLEROLE_SAFETYCAR        =  7,   /* Vehicle is used for public safety, e.g. patrol. */
    RIF_VEHICLEROLE_AGRICULTURE      =  8,   /* Vehicle is used for agriculture, e.g. farm tractor. */
    RIF_VEHICLEROLE_COMMERCIAL       =  9,   /* Vehicle is used for transportation of commercial goods. */
    RIF_VEHICLEROLE_MILITARY         = 10,   /* Vehicle is used for military purpose. */
    RIF_VEHICLEROLE_ROADOPERATOR     = 11,   /* Vehicle is used in road operator missions. */
    RIF_VEHICLEROLE_TAXI             = 12    /* Vehicle is used to provide an authorized taxi service. */
};

/* VehicleSubRole */
/* -------------- */
enum Rif_VehicleSubRole {
    RIF_VEHICLESUBROLE_UNKNOWN        =  0,  /* Default vehicle role as indicated by the vehicle type. */
    RIF_VEHICLESUBROLE_BUS            =  1,
    RIF_VEHICLESUBROLE_TRAM           =  2,
    RIF_VEHICLESUBROLE_METRO          =  3,
    RIF_VEHICLESUBROLE_TRAIN          =  4,
    RIF_VEHICLESUBROLE_EMERGENCY      =  5,  /* emergency vehicle with siren/lights. */
    RIF_VEHICLESUBROLE_SMOOTH         =  6,  /* ambulance smooth drive */
    RIF_VEHICLESUBROLE_TIMETABLE      =  7,  /* ambulance smooth drive */
    RIF_VEHICLESUBROLE_INTERVAL       =  8,  /* public transport time table service */
    RIF_VEHICLESUBROLE_EXPRESSTRANSIT =  9,
    RIF_VEHICLESUBROLE_NOSERVICE      = 10   /* vehicles that are not in active service */
};

/* CauseCode */
/* --------- */
enum Rif_CauseCode {
    RIF_CAUSECODE_UNKNOWN                                         =  0,
    RIF_CAUSECODE_TRAFFICCONDITION                                =  1,
    RIF_CAUSECODE_ACCIDENT                                        =  2,
    RIF_CAUSECODE_ROADWORKS                                       =  3,
    RIF_CAUSECODE_ADVERSEWEATHERCONDITION_ADHESION                =  6,
    RIF_CAUSECODE_HAZARDOUSLOCATION_SURFACECONDITION              =  9,
    RIF_CAUSECODE_HAZARDOUSLOCATION_OBSTACLEONTHEROAD             = 10,
    RIF_CAUSECODE_HAZARDOUSLOCATION_ANIMALONTHEROAD               = 11,
    RIF_CAUSECODE_HUMANPRESENCEONTHEROAD                          = 12,
    RIF_CAUSECODE_WRONGWAYDRIVING                                 = 14,
    RIF_CAUSECODE_RESCUEANDRECOVERYWORKINPROGRESS                 = 15,
    RIF_CAUSECODE_ADVERSEWEATHERCONDITION_EXTREMEWEATHERCONDITION = 17,
    RIF_CAUSECODE_ADVERSEWEATHERCONDITION_VISIBILITY              = 18,
    RIF_CAUSECODE_ADVERSEWEATHERCONDITION_PRECIPITATION           = 19,
    RIF_CAUSECODE_SLOWVEHICLE                                     = 26,
    RIF_CAUSECODE_DANGEROUSENDOFQUEUE                             = 27,
    RIF_CAUSECODE_VEHICLEBREAKDOWN                                = 91,
    RIF_CAUSECODE_POSTCRASH                                       = 92,
    RIF_CAUSECODE_HUMANPROBLEM                                    = 93,
    RIF_CAUSECODE_STATIONARYVEHICLE                               = 94,
    RIF_CAUSECODE_EMERGENCYVEHICLEAPPROACHING                     = 95,
    RIF_CAUSECODE_HAZARDOUSLOCATION_DANGEROUSCURVE                = 96,
    RIF_CAUSECODE_COLLISIONRISK                                   = 97,
    RIF_CAUSECODE_SIGNALVIOLATION                                 = 98,
    RIF_CAUSECODE_DANGEROUSSITUATION                              = 99
};

/* DangerousGoods */
/* -------------- */
enum Rif_DangerousGoods {
    RIF_DANGEROUSGOODS_UNKNOWN                                              = -1,
    RIF_DANGEROUSGOODS_EXPLOSIVES1                                          =  0,
    RIF_DANGEROUSGOODS_EXPLOSIVES2                                          =  1,
    RIF_DANGEROUSGOODS_EXPLOSIVES3                                          =  2,
    RIF_DANGEROUSGOODS_EXPLOSIVES4                                          =  3,
    RIF_DANGEROUSGOODS_EXPLOSIVES5                                          =  4,
    RIF_DANGEROUSGOODS_EXPLOSIVES6                                          =  5,
    RIF_DANGEROUSGOODS_FLAMMABLEGASES                                       =  6,
    RIF_DANGEROUSGOODS_NONFLAMMABLEGASES                                    =  7,
    RIF_DANGEROUSGOODS_TOXICGASES                                           =  8,
    RIF_DANGEROUSGOODS_FLAMMABLELIQUIDS                                     =  9,
    RIF_DANGEROUSGOODS_FLAMMABLESOLIDS                                      = 10,
    RIF_DANGEROUSGOODS_SUBSTANCESLIABLETOSPONTANEOUSCOMBUSTION              = 11,
    RIF_DANGEROUSGOODS_SUBSTANCESEMITTINGFLAMMABLEGASESUPONCONTACTWITHWATER = 12,
    RIF_DANGEROUSGOODS_OXIDIZINGSUBSTANCES                                  = 13,
    RIF_DANGEROUSGOODS_ORGANICPEROXIDES                                     = 14,
    RIF_DANGEROUSGOODS_TOXICSUBSTANCES                                      = 15,
    RIF_DANGEROUSGOODS_INFECTIOUSSUBSTANCES                                 = 16,
    RIF_DANGEROUSGOODS_RADIOACTIVEMATERIAL                                  = 17,
    RIF_DANGEROUSGOODS_CORROSIVESUBSTANCES                                  = 18,
    RIF_DANGEROUSGOODS_MISCELLANEOUSDANGEROUSSUBSTANCES                     = 19
};

/* PriorityRequestType */
/* ------------------- */
enum Rif_PriorityRequestType {
    RIF_PRIORITYREQUESTTYPE_NONE         = 0,
    RIF_PRIORITYREQUESTTYPE_REQUEST      = 1,
    RIF_PRIORITYREQUESTTYPE_UPDATE       = 2,
    RIF_PRIORITYREQUESTTYPE_CANCELLATION = 3
};

/* PrioritizationState */
/* ------------------- */
enum Rif_PrioritizationState {
    RIF_PRIORITIZATIONSTATE_UNKNOWN           = 0,   /* Unknown state. */
    RIF_PRIORITIZATIONSTATE_REQUESTED         = 1,   /* This prioritization request was detected by the traffic controller. */
    RIF_PRIORITIZATIONSTATE_PROCESSING        = 2,   /* Checking request (request is in queue, other requests are prior). */
    RIF_PRIORITIZATIONSTATE_WATCHOTHERTRAFFIC = 3,   /* Cannot give full permission, therefore watch for other traffic. Note that other requests may be present. */
    RIF_PRIORITIZATIONSTATE_GRANTED           = 4,   /* Intervention was successful and now prioritization is active. */
    RIF_PRIORITIZATIONSTATE_REJECTED          = 5,   /* The prioritization request was rejected by the traffic controller. */
    RIF_PRIORITIZATIONSTATE_MAXPRESENCE       = 6,   /* The request has exceeded maxPresence time.
                                                              Used when the controller has determined that the requester should then back off and request an alternative. */
    RIF_PRIORITIZATIONSTATE_RESERVICELOCKED   = 7    /* Prior conditions have resulted in a reservice locked event:
                                                              the controller requires the passage of time before another similar request will be accepted. */
};

/* TrafficDirection */
/* ---------------- */
enum Rif_TrafficDirection {
    RIF_TRAFFICDIRECTION_ALLTRAFFICDIRECTIONS = 0,
    RIF_TRAFFICDIRECTION_UPSTREAMTRAFFIC      = 1,
    RIF_TRAFFICDIRECTION_DOWNSTREAMTRAFFIC    = 2,
    RIF_TRAFFICDIRECTION_OPPOSITETRAFFIC      = 3
};

/* TurnIntention */
/* ------------- */
enum Rif_TurnIntention {
    RIF_TURNINTENTION_UNKNOWN  = 0,
    RIF_TURNINTENTION_LEFT     = 1,
    RIF_TURNINTENTION_STRAIGHT = 2,
    RIF_TURNINTENTION_RIGHT    = 3
};

/* TrustState */
/* ---------- */
enum Rif_TrustState {
    RIF_TRUSTSTATE_UNSECURED = 0,   /* no digital signature present     */
    RIF_TRUSTSTATE_UNTRUSTED = 1,   /* the digital signature is not trusted or cannot be verified */   
    RIF_TRUSTSTATE_TRUSTED   = 2    /* the digital signature is trusted */
};

/* --------------------------- */
/* ITS Station Interface (CAM) */
/* --------------------------- */
struct Rif_PublicTransport {
   rif_bool embarkation;
   rif_int  lineNr;
   rif_int  vehicleID;      /* Unique per company     */
   rif_int  serviceNr;      /* Same as block number   */     
   rif_int  journeyNr;      
   rif_int  supportNr;      /* Support journey number */
   rif_int  companyNr;
   rif_int  occupancy;      /* Number of passengers   */
};

struct Rif_SpecialTransport {
   rif_bool heavyLoad;
   rif_bool excessWidth;
   rif_bool excessLength;
   rif_bool excessHeight;
};

struct Rif_RoleAttributes {
   /* Deze velden zijn allemaal optioneel in de RIS-FI.
    * als ze niet bekend zijn, worden ze voor ints op -1 gezet en voor bools op false (0). */
   rif_bool                    lightBarActivated;
   rif_bool                    sirenActivated;
   enum Rif_CauseCode          incidentIndication;     /* CauseCode, zie definitie bovenaan. */
   rif_int                     incidentSubIndication;  /* SubCauseCode, groepeert suboorzaken van verkeerssituaties. */
   /* Als de de public transport en special transport niet bekend zijn,
    * dan worden de velden binnen deze attributen voor ints op -1 gezet en voor bools op false. */
   struct Rif_PublicTransport  publicTransport;
   struct Rif_SpecialTransport specialTransport;
   enum Rif_DangerousGoods     dangerousGoods;         /* DangerousGoods, zie definitie bovenaan, -1 indien onbekend. */
};

struct Rif_Location {      /* also used in ITS Events Interface (DENM) */
   rif_float latitude;     /* -90.0 t/m 90.0 degrees. */
   rif_float longitude;    /* -180.0 t/m 180.0 degrees. */
   rif_float elevation;    /* -100.0 t/m 8000.0 m, -101.0 indien onbekend. */
};

struct Rif_MapMatch {
   rif_string intersection;    /* Intersection ID, niet ingevuld ("") betekent geen map match. */
   rif_int    lane;            /* Lane hoort bij een SG, als hier 0 is ingevuld betekent dat deze zich op het conflict vlak (midden van het kruispunt) bevindt. */
   rif_string signalGroup;     /* SignalGroup ID, niet ingevuld ("") betekent geen map match. */
   /* T.o.v. de stopstreep van de SG, als deze waarde negatief is, dan is het een uitgaande lane.
    * (-) 0 t/m 429496729.5 m, -9,999,999.0 indien onbekend. */
   rif_float  distance;
   rif_float  offset;       /* 0 t/m 429496729.5 m, -1.0 indien onbekend. */
};

struct Rif_ItsStation {
   rif_string                id;            /* ObjectID van het voertuig. */
   enum Rif_StationType      stationType;   /* StationType, zie definitie bovenaan. */
   rif_timestamp             locationTime;  /* Timestamp: het aantal milliseconden sinds 1-1-1970 00:00:00:00 UTC.
                                             * Varieert van 0 tot 18446744073709509551615, eenheid 1 milliseconde. */
   enum Rif_VehicleRole      role;          /* VehicleRole, zie definitie bovenaan. */
   rif_float                 length;        /* 0 t/m 429496729.5 m, -1.0 indien onbekend. */
   rif_float                 width;         /* 0 t/m 429496729.5 m, -1.0 indien onbekend. */
   struct Rif_Location       location;
   /* Rijrichting/hoek/oriëntatie ten opzichte van WGS84 Noord, met de klok mee.
    * 0 t/m 360.0 graden, -1.0 indien onbekend. */
   rif_float                 heading;
   rif_float                 speed;         /* 0 t/m 99.0 m/s, -1.0 indien onbekend. */

#if defined TESTOMGEVING
   rif_float				 Speed_org;     /* 0 to 99.0 m/s, if unknown set to -1.0 */
#endif

   rif_float                 acceleration;  /* -16.0 to 16.0 m/s^2, -17.0 indien onbekend. */
   struct Rif_RoleAttributes roleAttributes;
   enum Rif_TurnIntention    turn;          /* TurnIndication, zie definitie bovenaan. */
   /* Meer dan 1 lane beschikbaar.
    * Als de MapMatch niet ingevuld/valid is dan zal de intersection ID binnen een MapMatch leeg zijn (""). */
   struct Rif_MapMatch       matches[RIF_MAXLANES];
   enum Rif_TrustState       trust;         /* TrustState, zie definitie bovenaan. */

   /* Geldigheid: 1 = valid, 0 = niet valid, verwijderen uit het buffer.
    * Behalve het id doen de andere velden er dan niet meer toe. */
   rif_bool valid;
};

/* ------------------------------------- */
/* PrioritizationRequest Interface (SRM) */
/* ------------------------------------- */
struct Rif_TransitStatus {
   /* Optioneel, alle velden zijn "false" indien onbekend. */
   rif_bool loading;       /* parking and unable to move at this time */
   rif_bool anADAuse;      /* an ADA§ access is in progress, wheelchairs, kneeling, etc. */
   rif_bool aBikeLoad;     /* loading of a bicycle is in progress */
   rif_bool doorOpen;      /* a vehicle door is open for passenger access */
   rif_bool charging;      /* a vehicle is connected to charging point */
   rif_bool atStopLine;    /* a vehicle is at the stop line for the lane it is in */
};

struct Rif_PrioritizationRequest {
   rif_string                   id;             /* ID van de request zelf, deze bestaat uit <itsStationID>_<requestID>. */
   rif_int                      sequenceNumber; /* Het vervolgnummer van deze request, deze kan gebruikt worden als de request veranderd. */
   enum Rif_PriorityRequestType requestType;    /* Als er geen request is dan is hier de waarde 0 ingevuld. */
   rif_string                   itsStation;     /* ObjectID van de bijbehorende itsStation waarvoor deze request geldt. */
   rif_string                   intersection;   /* ObjectID van het kruispunt waarvoor deze request geldt. */
   enum Rif_VehicleRole         role;           /* VehicleRole, zie definitie bovenaan. */
   enum Rif_VehicleSubRole      subrole;        /* VehicleSubRole, zie definitie bovenaan. */
   rif_timestamp                eta;            /* Timestamp: verwachte aankomsttijd van het voertuig bij de stopstreep. */
   /* De richtingen */
   rif_string                   signalGroup;    /* Indien beschikbaar, dan is dit een accurate richting, ("") als onbekend. */
   /* Nummer voor het groeperen van alle naderende rijstroken van een arm in één groep.
    * Fiets- en voetgangersovergangen die een kruispunt oversteken, hebben dezelfde approach als het kruispunt die zij oversteken.
    * 0 t/m 15, 0 indien onbekend. */
   rif_int                      approach;
   rif_string                   routeName;      /* Naam van de route, ("") indien onbekend. */
   struct Rif_TransitStatus     transitStatus;  /* Optioneel, alle velden zijn "false" indien onbekend. */
   rif_int                      punctuality;    /* -3600 to 3600 s, negative values indicate early arrival. -3601 als onbekend. */
   rif_int                      importance;     /* De belangrijkheid van het verzoek, -1 indien onbekend. */
   enum Rif_TrustState          trust;          /* TrustState, zie definitie bovenaan. */

   /* Geldigheid: 1 = valid, 0 = niet valid, verwijderen uit het buffer.
    * Behalve het id doen de andere velden er dan niet meer toe. */
   rif_bool valid;
};

/* ------------------------------------ */
/* ActivePrioritization Interface (SSM) */
/* ------------------------------------ */
struct Rif_ActivePrioritization {
   rif_string                   id;             /* ID van de priority request, om de active prioritization hieraan te kunnen koppelen. */
   rif_int                      sequenceNumber; /* Zelfde als de sequencyNumber van de bijbehorende priority request. */
   enum Rif_PrioritizationState prioState;      /* PrioritizationState, zie definitie bovenaan. */
};

/* --------------------------- */
/* ITS Events Interface (DENM) */
/* --------------------------- */
struct Rif_Path {
   /* Deze struct beschrijft een pad d.m.v een verzameling punten. */
   /* Punten worden in de volgorde gedefinieerd vanaf de dichtstbijzijnde afstand van de referentielocatie van het pad (bv. de stoplijn).*/
   /* De array wordt gelezen totdat een locatie met de waarde [0,0,0] wordt uitgelezen of totdat MAXPOINTS wordt bereikt. */
   struct Rif_Location points[RIF_MAXPOINTS];
};

/* Deze struct beschrijft een geografisch gebied, afgebakend door een geometrische vorm. */
struct Rif_Area {
   struct Rif_Location centre;     /* Middelpunt van het gebied. */
   /* De majorAxis is de afstand tussen het middelpunt en de korte zijde van de geometrische vorm (loodrecht op de korte zijde).
    * Voor een cirkel hebben de majorAxis en minorAxis dezelfde waarde (en circular heeft de waarde "true"). */
   rif_float           majorAxis;  /* Length: 0 t/m 429496729.5 m, -1.0 indien onbekend. */
   rif_float           minorAxis;  /* Length: 0 t/m 429496729.5 m, -1.0 indien onbekend. */
   rif_float           angle;      /* Heading: de azimut hoek van de lange kant van de geometrische vorm. 0 t/m 360,0, -1.0 indien onbekend. */
   rif_bool            circular;   /* Indien "false" dan is het gebied een rechthoekig gebied in plaats van een cirkelvormig gebied. */
};

struct Rif_ItsEvent {
   /* ID van het ITS event, deze wordt ook gebruikt wanneer er wordt verwezen naar eerder aangemaakte ITS events.
    * Bijvoorbeeld wanneer deze geupdate of verwijderd wordt. */
   rif_string                id;
   rif_timestamp             detectionTime;          /* Timestamp: het aantal milliseconden sinds 1-1-1970 00:00:00:00 UTC.
                                                      * Varieert van 0 tot 18446744073709509551615, eenheid 1 milliseconde. */
   enum Rif_CauseCode        eventType;              /* Causecode, zie definitie bovenaan. */
   rif_int                   eventSubType;           /* Subcausecode */
   struct Rif_Location       eventPosition;          /* Lokatie van de ITS event. */
   rif_int                   validityDuration;       /* Duur van het event, 0 t/m 86400 s */
   rif_float                 relevanceDistance;      /* 0 t/m 429496729.5 m, -1.0 indien onbekend. */
   enum Rif_TrafficDirection trafficDirection;       /* TrafficDirection, zie definitie bovenaan. */
   struct Rif_Path           traces[RIF_MAXTRACES];  /* Als er geen trace gebruikt wordt, dan moet de eerste Location binnen een Path op [0, 0, 0] worden gezet. */
   /* Optioneel veld in de RIS-FI.
    * Indien onbekend of niet gebruikt dan moet de centre Location binnen de Area op [0, 0, 0] worden gezet. */
   struct Rif_Area           destinationArea;
   rif_int                   repetitionInterval;     /* 0 t/m 10000 ms, -1 indien onbekend. */
   enum Rif_TrustState       trust;                  /* TrustState, zie definitie bovenaan. */

   /* Geldigheid: 1 = valid, 0 = niet valid, verwijderen uit het buffer.
    * Behalve het id doen de andere velden er dan niet meer toe. */
   rif_bool valid;
   /* Interne referentie waarmee de applicatie zelf aangemaakte ITS events kan updaten of verwijderen.
    * Door de applicatie zelf in te vullen. Voor ontvangen DENMs wordt -1 gebruikt. */
   rif_int  internalRef;
};

#endif /* _RIF_DEFS_H */
