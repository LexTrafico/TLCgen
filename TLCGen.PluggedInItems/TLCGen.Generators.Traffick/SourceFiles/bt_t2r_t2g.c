/* DECLARATIE SPAT FUNCTIES */
/* ======================== */
/*
 *  Huijskes VRT B.V.
 *  14 Oktoberplein 6
 *  Overloon
 *
 *
 *  Module: functies t.b.v. bijhouden en bepalen van betrouwbaarheid van uitgestuurde SPAT data 
 *
 *  Filenaam        : bt_t2r_t2g.c
 *  Ontwerp         : Marc Scherjon
 *  Datum           : 26-09-2017
 *  CCOL            : versie 9.0 en hoger,  Generator PNH versie ?? en hoger
 *
  Revisie:
    26-09-2017 M. Scherjon:  opzet
    29-09-2017 M. Scherjon:  test file ook apart compileren en als object aan de linker toe te voegen geslaagd.
    29-09-2017 M. Scherjon:  eerste test met EVENT registratie
    25-10-2017 M. Scherjon:  init variabelen bt_spat_score[]
    01-11-2017 M. Scherjon:  betrouwbaarheid op 'kans' methodiek en formules voor weging op basis van tijd en verschil 
    28-11-2017 M. Scherjon:  BITs aangepast t.b.v. optie Masker bij fc_timing
    19-01-2018 M. Scherjon:  Vervallen code verwijderd
    05-03-2018 M. Scherjon:  Ondergrens Betrouwbaarheid van 5% naar 20%
    15-03-2018 M. Scherjon:  Code formating aangepast
    17-03-2018 M. Scherjon:  Array van struct anders ingedeeld.
                             gebruik van pointer naar struct als argument in functies
                             3 hulpfuncties t.b.v. struct toegevoegd.
    26-03-2018 M. Scherjon:  Betrouwbaarheids registratie voor elke status gebruik van dezelfde functie.
    30-03-2018 M. Scherjon:  Betrouwbaarheids registratie test bits als argument.
    30-03-2018 M. Scherjon:  Afronding correcties toegevoegd.
    30-03-2018 M. Scherjon:  proc_spat_register_event() argumenten aangepast, SPAT macro's vervallen
    04-05-2018 M. Scherjon:  overbodige / vervallen includes verwijderd
*/

/* macrodefinities */ 
/* =============== */ 

#ifndef BT_SPAT
  #define BT_SPAT                       /* vrijgave functies in deze file. Eventueel in time2red_time2green toepassen */
#endif

/*  #define BT_SPAT_BT_GEM                        indien onvoldoende data beschikbaar gebruik dan gemiddelde en factor */
  #define BT_SPAT_BT_NO_GEM           -2      /*  @@ 17-03-2018  -2 : indien onvoldoende data beschikbaar en geen gemiddelde, dan 'deze' BT waarde */
  #define BT_SPAT_NO_BT               NG   

/* include files */ 
/* ============= */ 
  #include "sysdef.c"                 /*  systeemdefinities ( TRUE, FALSE, NG )                           */ 

/* declaraties van functies en variabelen */
/* ====================================== */
#ifndef BT_SPAT_H
  #define BT_SPAT_C
  #include "bt_t2r_t2g.h"     /* Functie en variabele declaraties */
#endif

/* alleen voor 'intern' gebruik */

/* macrodefinities */ 
/* =============== */ 

/* Algemene betrouwbaarheids (grens) waarden */
  #define BT_SPAT_BT_INIT             NG      /* %    : toegepaste betrouwbaarheid na init */
  #define BT_SPAT_BT_NO_REG           NG      /* %    : toegepaste betrouwbaarheid tijdens NIET REGELEN */
  #define BT_SPAT_BT_MIN              20      /* %    : minimum betrouwbaarheid -> onder deze grens dan BT_SPAT_NO_BT (NG) sturen */
  #define BT_SPAT_BT_MAX              95      /* %    : maximum betrouwbaarheid */

/* t.b.v. eenvoudige betrouwbaarheids berekening middels afgevlakte waarde op basis van gemiddelde score huidige cyclus en vorige cyclus */
  #define BT_SPAT_T2S_MIN              0      /* /te    : negeer score bij kleine waarde van T2SG. Dit is waarschijnlijk score 100%. Dat is nu even niet eerlijk' */
  #define BT_SPAT_T2S_SEC_CORR        50      /* /te    : corrigeer score bij kleinere T2S waarden i.v.m. seconde afronding in voorspelling */
  #define BT_SPAT_ALFA_DEF             3      /* factor : toegepaste afvlakking (indien niet opgegeven of ongeldig) */

/* t.b.v. betrouwbaarheids berekening middels weging van historische data */
  #define BT_SPAT_VENSTER_DEF         20      /* %    : Toegestane afwijking waarbij kans als GOED wordt beoordeeld */

  #define BT_SPAT_TIJDBASIS_DEF      900      /* /ts  : toegepaste tijdbasis voor 50% weging indien niet opgegeven of ongeldig */
  #define BT_SPAT_VERSCHIL_DEF        25      /* %    : toegepaste verschilpercentage voor 50% weging indien niet opgegeven of ongeldig */
  #define BT_SPAT_VENSTER_SEC_CORR    11      /* /te  : corrigeer toegestane afwijking bij kleinere T2S waarden i.v.m. seconde afronding in voorspelling */
  #define BT_SPAT_VENSTER_MIN         20      /* /te  : ondergrens voorspelling bij berekenen betrouwbaarheid */

  #define BT_SPAT_DATA_MIN            25      /* Minimum aantal data */
  #define BT_SPAT_DATA_OKE_MIN        10      /* Minimum aantal goede data */
  #define BT_SPAT_WEGING_MIN         100      /* Minimum weging */

#ifdef SPAT_TIJD_MAX
  #define BT_SPAT_TIMING_MAX   SPAT_TIJD_MAX  /* 32767  (intern) maximum lifetime van een EVENT in /te ( = 54 minuten en 36 sec ) */
#else
  #define BT_SPAT_TIMING_MAX         32768    /* 32767  (intern) maximum lifetime van een EVENT in /te ( = 54 minuten en 36 sec ) */
#endif

/* alleen voor 'intern' gebruik */
  bool set_bt_spat_register( struct _BT_SPAT_T2S *t2s, count j, mulv _volgnr, mulv _timing, mulv _t2s, mulv _bt, mulv _afwijking, mulv _score );
  bool verwerk_timing_bt_spat_register( struct _BT_SPAT_T2S *t2s, count j , mulv tiendesec );
  mulv lees_volgnummer_bt_spat_register( struct _BT_SPAT_T2S *t2s );

  void proc_spat_afgevlakte_betrouwbaarheid_t2s( struct _BT_SPAT_T2S *t2s , mulv num, mulv factor);

/* definities van functies */
/* ======================= */

/* 
- init = GEEN betrouwbaarheid

- indien een SPAT update plaats vindt middels een EVENT, dan deze gegevens opslaan in een buffer.

opgeslagen wordt:
 - Voor 'tijd tot start Rood', 'tijd tot start geel' en 'tijd tot start groen,
 - De voorspelling.
 - De verzonden betrouwbaarheid.(kans)
 - Een volgnummer (blijft gelijk tot start moment heeft plaats gevonden).
 - Alle events ( updates ) worden geregistreed.
 - Op start moment wordt bij alle voorspellingen de afwijking geregistreerd.

 - indien voldoende gegeven binnen zijn, kan hieruit nu de juiste betrouwbaarheid worden berekend.

 - tijdelijke berekening betrouwbaarheid door afvlakking.
 - Wordt weggeschreven in array 0.
 
 @@ 29-10-2017
 - berekening op basis van kans haalbaarheid, binnen een toegestane afwijking.
 - volgens VLOG3 doc is een afwijking van 20% nog altijd een score van 100%. 

 - Bij elke voorspelling kan dit anders zijn.
 - Op basis van de historische waarde wordt een weging toegepast.
 - Voor elke voorspelde waarde gelden de volgende wegingen:
  - Op basis van de verstreken tijd van event geld dat de weging elke 15 minuten (900 sec) halveert. ( i.v.m. formule => 56% ). ( 30 min = 25%, 45 min = 6%, 1 uur = 0% )
  - Op basis van het verschil van het event t.o.v. de voorspellingswaarde geldt dat bij een verschil van 25% de weging halveert (i.v.m. formule 56% ). ( 50% => 25% , 100% => 0% )
    vervolgens worden wegingen met elkaar vermenigvuldig.
    Het totaal aantal resultaten * de wegingen wordt berekend voor de resultaten waarbij de de voorspelling WEL gehaald is, en NIET.
    De verhouding van deze resultaten t.o.v. het totaal aantal wegingen is de betrouwbaarheid.

  - indien er onvoldoende data beschikbaar is, wordt -1 ( NG ) als betrouwbaarheid gegeven.
    onvoldoende is:
    - minder dan 50 geregistreerde EVENTS ( +- 15 minuten na opstraten )
    - minder dan 10 geregistreerde EVENTS met een score, en die jonger zijn als 54 minuten. ( te weinig verkeer )

  - formules: 
  weging tijd: = (0,625 * ( x ^ 2 ) - 500 * x + 100000)/1000 : waarbij x = 100% van de tijdbasis ( 15 minuten is ) Max is 400%, waarbij het resultaat dus tussen 0 en 100 ligt.
  weging verschil : = (( x ^ 2 ) - 200 * x + 10000)/100 : waarbij x een percentage tussen 0 en 100% is. waarbij het resultaat ook tussen 0 en 100 ligt.


  Voorbeeld:

  -> voorspelling = 30,0 sec
    event 15,0 sec met 10% afwijking en een verlopen tijd van 90 sec.
    - weging op basis van tijd = 93,3
    - weging op basis van verschil = 25
    - totale weging is 23,33 -> Ja

    event 25,0 sec met 25% afwijking en een verlopen tijd van 180 sec.
    - weging op basis van tijd = 87,1
    - weging op basis van verschil = 69,4
    - totale weging is 60,49 => Nee

    De betrouwbaarheid van de voorspelling van 30 sec bij deze 2 events = 27,8% ( weging totaal = 83,82 )


    event 35,0 sec met 10% afwijking en een verlopen tijd van 60 sec.
    - weging op basis van tijd = 95,5
    - weging op basis van verschil = 69,4
    - totale weging is 66,26 => Ja

    De betrouwbaarheid van de voorspelling van 30 sec bij deze 3 events = 59,7% ( weging totaal = 150,08 )

*/


bool set_bt_spat_register( struct _BT_SPAT_T2S *t2s, count j, mulv _volgnr, mulv _timing, mulv _t2s, mulv _bt, mulv _afwijking, mulv _score )
{

  bool result = FALSE;

  if ( (j >= 0) && (j < BT_SPAT_MAX) ) {
    t2s->volgnummer[j] = _volgnr;
    t2s->timing[j] = _timing;
    t2s->t2s[j] = _t2s;
    t2s->bt[j] = _bt;
    t2s->afwijking[j] = _afwijking;
    t2s->score[j] = _score;
    result = TRUE;
  }

  return(result);

}

bool verwerk_timing_bt_spat_register( struct _BT_SPAT_T2S *t2s, count j , mulv tiendesec )
{

  bool result = FALSE;
  mulv time;

  if ( (j > 0) && (j < BT_SPAT_MAX) ) {
    if ( t2s->timing[j] > NG ) {
      time = t2s->timing[j];
      if ( (time + tiendesec) < BT_SPAT_TIMING_MAX ) {
        time += tiendesec;
        t2s->timing[j] = time;
        result = TRUE;
      }
      else t2s->timing[j] = BT_SPAT_TIMING_MAX;
    }
  }

  return (result);
}

mulv test_volgnummer(mulv num )
{

  mulv result = num;

  if ( (result < 0) || (result >= BT_SPAT_MAX) ) {
    result = 1;
  }

  return (result);
}


mulv lees_volgnummer_bt_spat_register( struct _BT_SPAT_T2S *t2s )
{

  mulv result;

  result = test_volgnummer((mulv) t2s->volgnummer[0]);

  t2s->volgnummer[0] = result;

  return (result);
}



void init_spat_bt(count fcmax, bool init)
{

  count i, j;



  for ( i = 0; i < fcmax; i++ ) {
    for ( j = 0; j < BT_SPAT_MAX; j++ ) {
      set_bt_spat_register( &BT_SPAT_T2SG[i], j, NG, NG, NG, NG, NG, NG );
      set_bt_spat_register( &BT_SPAT_T2EG[i], j, NG, NG, NG, NG, NG, NG );
    }

    if ( init == FALSE ) { /* niet regelen */
      BT_SPAT_T2EG[i].bt[0] = BT_SPAT_BT_NO_REG;
      BT_SPAT_T2SG[i].bt[0] = BT_SPAT_BT_NO_REG;
      BT_SPAT_T2EG[i].score[0] = BT_SPAT_BT_NO_REG * 10; /* x 10 i.v.m. grotere nauwkeurigheid : betere afronding */
      BT_SPAT_T2SG[i].score[0] = BT_SPAT_BT_NO_REG * 10; /* x 10 i.v.m. grotere nauwkeurigheid : betere afronding */
    }
    if ( init == TRUE ) { /* init op start regelen */
      BT_SPAT_T2EG[i].bt[0] = BT_SPAT_BT_INIT;
      BT_SPAT_T2SG[i].bt[0] = BT_SPAT_BT_INIT;
      BT_SPAT_T2EG[i].score[0] = BT_SPAT_BT_INIT * 10; /* x 10 i.v.m. grotere nauwkeurigheid : betere afronding */
      BT_SPAT_T2SG[i].score[0] = BT_SPAT_BT_INIT * 10; /* x 10 i.v.m. grotere nauwkeurigheid : betere afronding */
      if ( BT_SPAT_T2EG[i].score[0] < NG ) BT_SPAT_T2EG[i].score[0] = NG;
      if ( BT_SPAT_T2SG[i].score[0] < NG ) BT_SPAT_T2SG[i].score[0] = NG;
    }

  }

}




void proc_spat_register_event( struct _BT_SPAT_T2S *_t2s, mulv t2s, bool status_start, bool status_end, bool bt_event, mulv tiendesec) 
                  /* @@ 26-03-2018 , @@ 30-03-2018 3 argumenten voor start, end en event */
{

  count i, j;
  mulv num;
  mulv verschil;
  long _score;

  bool tmp; /* hulp */

  tmp = FALSE;

  /* index '0' wordt gebruikt voor bijhouden actuele status en gegevens */
  num = lees_volgnummer_bt_spat_register( _t2s );

  if ( tiendesec ) {
    /* de timers van het actuele volgnummer bijwerken */
    for ( j = 1; j < BT_SPAT_MAX; j++ ) {
      verwerk_timing_bt_spat_register( _t2s , j , tiendesec );
    }
  }

  /* ============================================== */
  /* Start status                                   */
  /* ============================================== */
  if ( status_start ) {
    /* hier wordt niets mee gedaan */
  }

  /* ====================================================== */
  /* nieuw event aanmaken                                   */
  /* ====================================================== */
  if ( (bt_event) && (t2s > 0) ) {
    tmp = FALSE;
    j = 0;
    for ( i = 1; i < (2 * BT_SPAT_MAX - 1); i++ ) { /* bijna 2 rondjes */
      j++;
      j = test_volgnummer((mulv) j);
      if ( _t2s->volgnummer[j] == num ) { /* zoek huidig volgnummer */
        tmp = TRUE;
      }
      if ( tmp && (_t2s->volgnummer[j] != num) || (_t2s->volgnummer[j] == NG)
#ifdef TESTOMGEVING
        /* de lege plek (t.b.v. F11) weer opvullen  */
          || tmp && (_t2s->volgnummer[j] == num) && (_t2s->timing[j] == NG)
#endif
          ) {
        /* eerst volgende plek met 'oud' volgnummer, of lege plek */
        tmp = proc_geef_bt_t2s( _t2s, (mulv) t2s ); /* betrouwbaarheid */
        if ( tmp < NG ) tmp = NG;
        set_bt_spat_register( _t2s, j, 
            num,       /* volgnummer */
            0,            /* restart timing */
            t2s,         /* voorspelling */
            tmp,          /* betrouwbaarheid */ 
            NG, NG );     /* afwijking , score */
        tmp = FALSE;
        break;
      }
    }
  }

  /* ============================================== */
  /* Einde status                                   */
  /* ============================================== */
  if ( status_end ) {

    /* scores bepalen van alle events afgelopen cyclus */
    for ( j = 1; j < BT_SPAT_MAX; j++ ) {
      if ( (_t2s->volgnummer[j] == num) &&
          (_t2s->timing[j] > 0) &&
          (_t2s->t2s[j] > 0) &&
          (_t2s->score[j] == NG) ) {

        verschil = _t2s->t2s[j] - _t2s->timing[j];

        if ( _t2s->t2s[j] > 0 ) { /* beveiliging tegen delen door nul */
          if ( verschil >= 0 ) {
            _score = (((long) 100 * (long) verschil) + (long) (_t2s->t2s[j] / 2)) / (long) _t2s->t2s[j];
          }
          else {
            _score = (((long) 100 * (long) verschil) - (long) (_t2s->t2s[j] / 2)) / (long) _t2s->t2s[j];
          }
        }
        else {
          _score = 0;
        }

        if ( _score > 100 ) _score = 101;
        if ( _score < -100 ) _score = -101;

        _t2s->afwijking[j] = (mulv) _score;

        if ( _score > 100 ) _score = 100;
        if ( _score < -100 ) _score = -100;

        if ( _score < 0 ) {
          _t2s->score[j] = 100 + (mulv) _score;
        }
        else {
          _t2s->score[j] = 100 - (mulv) _score;
        }
      }
    }

    /* alle scores zijn bijgewerkt,  nu de afgevlakte betrouwbaarheid bijwerken */
    proc_spat_afgevlakte_betrouwbaarheid_t2s( _t2s, (mulv) num, (mulv) NG);

#ifdef TESTOMGEVING
    tmp = FALSE;
    j = 0;
    for ( i = 1; i < (2 * (BT_SPAT_MAX - 1)); i++ ) { /* bijna 2 rondjes */
      j++;
      j = test_volgnummer((mulv) j);
      if ( _t2s->volgnummer[j] == num ) { /* zoek huidig volgnummer */
        tmp = TRUE;
      }
      if ( tmp && (_t2s->volgnummer[j] != num) || (_t2s->volgnummer[j] == NG) ) {
        /* eerst volgende plek met 'ouder' volgnummer, of lege plek */
        tmp = FALSE;
        break;
      }
    }
#endif

    /* volgnummer bijwerken */
    num++;
    num = test_volgnummer((mulv) num);
    _t2s->volgnummer[0] = num;

#ifdef TESTOMGEVING
    set_bt_spat_register( _t2s, j, 
            num,       /* Schrijf het nieuwe volgnummer weg, i.v.m. F11 scherm */
            NG, NG, NG, NG, NG );
#endif

  }

}



void proc_spat_afgevlakte_betrouwbaarheid_t2s( struct _BT_SPAT_T2S *_t2s , mulv num, mulv factor)
{
  /* tijdelijke oplossing voor
    het bepalen van de betrouwbaarheid op basis van een afvlakking van
    alle historische gegevens

    De afvlakfactor wordt gecorrigeerd op basis van de lifetime van het event.

    De functie dient te worden aangeroepen nadat de score is bepaald ( op SG[] of SR[] )

    Bij elke aanroep wordt de betrouwbaarheid opnieuw berekend,
    en zou bij herhaalde aanroepen dus ook steeds weer opnieuw worden afgevlakt.

    num geeft het volgnummer van de laatste cyclus.

    */

  count i, j;
  mulv _factor;
  mulv old_score;
  mulv new_score;
  long _score;
  mulv correctie;
  mulv bt_spat_score[BT_SPAT_MAX] = { 0 };
  bool tmp; /* hulp vlag */

  tmp = FALSE;

  if ( (factor > 0) && (factor <= 10) ) {
    _factor = factor;
  }
  else _factor = BT_SPAT_ALFA_DEF;

  new_score = 0;
  old_score = _t2s->score[0];

  correctie = 0;

  i = 0;
  for ( j = 1; j < BT_SPAT_MAX; j++ ) {
    if ( (_t2s->volgnummer[j] == num) && (_t2s->score[j] > NG) ) {
#ifdef BT_SPAT_T2S_MIN
      if ( _t2s->t2s[j] > BT_SPAT_T2S_MIN ) {
#endif
        bt_spat_score[i] = _t2s->score[j];
#ifdef BT_SPAT_T2S_SEC_CORR
        if ( (_t2s->t2s[j] <= 100) && (_t2s->t2s[j] <= BT_SPAT_T2S_SEC_CORR) && (_t2s->score[j] < 100) ) {
          if ( ((100 - _t2s->t2s[j]) / 10) < (100 - _t2s->score[j]) ) {
            bt_spat_score[i] += (100 - _t2s->t2s[j]) / 10;
          }
          else {
            bt_spat_score[i] = 100;
          }
        }
#endif
        i++;
#ifdef BT_SPAT_T2S_MIN
      }
#endif
    }
  }

  if ( i > 0 ) { /* controle of er iets is gevonden */
    /* index '0' wordt gebruikt voor bijhouden actuele status en gegevens */

    /* bepaal eerst gemiddelde van score laatste cyclus */
    for ( j = 0; j < i; j++ ) {
      new_score += (bt_spat_score[j] + ((mulv) i / 2)) / (mulv) i;
    }
    _score = (long) ((10 - _factor) * old_score + _factor * new_score * 10 + 5) / 10;
    _t2s->score[0] = (mulv) _score;
    _t2s->bt[0] = (_t2s->score[0] + 5) / 10;
    if ( _t2s->bt[0] > BT_SPAT_BT_MAX ) _t2s->bt[0] = BT_SPAT_BT_MAX; /* maximum beperking */
    if ( _t2s->bt[0] < BT_SPAT_BT_MIN ) _t2s->bt[0] = BT_SPAT_NO_BT;  /* minimum beperking */

  }
  else {
    /* er is iets mis gegaan. Er zijn geen registraties geweest. Dus wordt er ook niks bijgewerkt. */
  }

}


mulv proc_geef_bt_t2s( struct _BT_SPAT_T2S *_t2s, mulv voorspel) /* @@ 17-03-2018: argument gewijzigd in pointer naar struct */
{

  bool oke = FALSE;
  count i, j;
  mulv tmp, tmp_t, data, data_oke;
  long_mulv l_tmp;

  long_mulv weging_timing = 0;
  long_mulv weging_t2s = 0;

  long_mulv totaal_weging_aantal = 0;
  long_mulv totaal_weging_score = 0;
  long_mulv betrouwbaarheid = 0;

  mulv temp_time = 0;
  mulv time_factor = 0;
  mulv verschil_factor = 0;

  mulv base_timing = BT_SPAT_TIJDBASIS_DEF;
  /*  mulv base_t2s_diff = BT_SPAT_VERSCHIL_DEF; */
  mulv base_venster = BT_SPAT_VENSTER_DEF;

  i = 0;
  data = 0;
  data_oke = 0;
  if ( voorspel > 0 ) {
    if ( voorspel < BT_SPAT_VENSTER_MIN ) voorspel = BT_SPAT_VENSTER_MIN;
    if ( (voorspel*base_venster < BT_SPAT_VENSTER_SEC_CORR * 100) ) {
      /* correctie bij kleine waarden van voorspelling, dan wordt een groter venster genomen. Dit i.v.m. o.a. seconde afronding */
      base_venster = ((BT_SPAT_VENSTER_SEC_CORR * 100 + voorspel / 2) / voorspel);
    }
    /* if ( voorspel < BT_SPAT_VENSTER_MIN ) voorspel = BT_SPAT_VENSTER_MIN; */ /* NIET @@ 30-03-2018 volgorde aangepast. Eerst correctie en daarna ondergrens -> ruimer venster */

    for ( j = 1; j < BT_SPAT_MAX; j++ ) {
      if ( (_t2s->volgnummer[j] > 0) &&
          (_t2s->timing[j] > 0) &&
          (_t2s->t2s[j] > 0) &&
          (_t2s->score[j] > NG) ) {
        if ( (base_timing > 0) && (_t2s->timing[j] < BT_SPAT_TIMING_MAX) ) { /* beveiliging delen door 0, en bereiken maximum 'life'time van timer (54 minuten) */
          l_tmp = (long_mulv) (((_t2s->timing[j] * 10) + (base_timing / 2)) / base_timing); /* *10 :  omrekening /te naar /ts !! + afronding */
        }
        else {
          l_tmp = 0;
        }
        if ( l_tmp > 400 ) l_tmp = 400;  /* maximum beperking 400% van basis tijd :  bij huidige instelling (900 sec) stopt het al bij 364% ( = 54 minuten = max timing )*/
        time_factor = (mulv) l_tmp;
        if ( _t2s->timing[j] > temp_time ) temp_time = _t2s->timing[j]; /* voor TEST:  bijhouden hoogste tijd */

        /* Formule voor weging 'tijd_factor' (0 - 400) :  (0,626 * ( x ^ 2 ) - 500 * x + 100000)/1000 */
        l_tmp = 100 * (time_factor*time_factor);
        l_tmp = (l_tmp + 80) / 160; /* deze samen met vorige regel = 0,626 * (x^2)   +80 i.v.m.afronding */
        l_tmp += 100000;
        l_tmp -= (500 * time_factor);
        l_tmp = ((l_tmp + 500) / 1000); /* +500 i.v.m.afronding */
        if ( l_tmp < 0 ) l_tmp = 0;     /* beveiliging */
        if ( l_tmp > 100 ) l_tmp = 100; /* beveiliging */
        weging_timing = (mulv) l_tmp;

        l_tmp = 100;
        verschil_factor = 0;
        tmp_t = _t2s->t2s[j];
        if ( voorspel > tmp_t ) {
          tmp = (voorspel - tmp_t);
        }
        else {
          tmp = (tmp_t - voorspel);
        }
        if ( tmp < voorspel ) {
          l_tmp = (long_mulv) (tmp * 100 + (voorspel / 2)) / voorspel; /* verschil in verhouding tot voorspelde waarde in Percentage ( 0 - 100 ) */
        }
        verschil_factor = (mulv) l_tmp;

        /* Formule voor weging 'verschil_factor' (0 - 100) : 	( x ^ 2 ) - 200 * x + 10000)/100 */
        l_tmp = (verschil_factor*verschil_factor);
        l_tmp += 10000;
        l_tmp -= (200 * verschil_factor);
        l_tmp = ((l_tmp + 50) / 100); /* +50 i.v.m.afronding */
        if ( l_tmp < 0 ) l_tmp = 0; /* beveiliging */
        if ( l_tmp > 100 ) l_tmp = 100; /* beveiliging */
        weging_t2s = (mulv) l_tmp;

        /* data controle */
        data++;
        if ( (weging_timing > 0) && (weging_t2s > 0) ) data_oke++;

        tmp = _t2s->afwijking[j];

        totaal_weging_aantal += ((weging_timing*weging_t2s) + 50) / 100; /* @@ 30-03-2018  +50 afronding correctie toegevoegd */
        if ( (tmp >= 0) && (tmp <= base_venster) || (tmp < 0) && ((tmp + base_venster) >= 0) ) {
          totaal_weging_score += ((weging_timing*weging_t2s) + 50) / 100; /* @@ 30-03-2018  +50 afronding correctie toegevoegd */
        }
      }
    }

    if ( (totaal_weging_aantal > 0) &&                  /* beveiliging op deling door 0 */
        (totaal_weging_aantal > BT_SPAT_WEGING_MIN) &&    /* minimmum aantal weging */
        (data > BT_SPAT_DATA_MIN) &&                      /* minimum hoeveelheid data */
        (data_oke > BT_SPAT_DATA_OKE_MIN) ) {             /* minimum aantal data die mee doet */
      betrouwbaarheid = ((totaal_weging_score * 100) + (totaal_weging_aantal/2)) / totaal_weging_aantal; /* @@ 30-03-2018  (../2) afronding correctie toegevoegd */
      if ( betrouwbaarheid > BT_SPAT_BT_MAX ) betrouwbaarheid = BT_SPAT_BT_MAX; /* @@ MS  14-11-2017   maximum beperking */
      if ( betrouwbaarheid < BT_SPAT_BT_MIN ) betrouwbaarheid = BT_SPAT_NO_BT;  /* @@ MS  15-11-2017   minimum beperking */
      oke = TRUE;
    }
    else {
#ifdef BT_SPAT_BT_GEM
      betrouwbaarheid = _t2s[0].bt;
#else
      betrouwbaarheid = BT_SPAT_BT_NO_GEM;
#endif
    }
  }
  else {
    betrouwbaarheid = BT_SPAT_NO_BT; /* bij voorspelling 0 of NG */
  }

  return((mulv) betrouwbaarheid);

}
