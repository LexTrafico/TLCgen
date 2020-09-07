/* ======================================================================================================== */
/* Fick CCOL generator                                                              Versie 1.7 / 4 sep 2020 */
/* -------------------------------------------------------------------------------------------------------- */
/* Kruispuntnummer:                      0001                                                               */
/* Locatie:                              -                                                                  */
/* Plaats:                               -                                                                  */
/* Ontwerp:                              -                                                                  */
/* User interface:                       Traffick Solutions - Ferry van den Heuvel                          */
/* -------------------------------------------------------------------------------------------------------- */
/* k0001krw.c                                                                                               */
/* Versie:                               1.0                                                                */
/* Door:                                 -                                                                  */
/* Datum generatie:                      07-09-2020                                                         */
/*  Tijd generatie:                      08:32:25 (+02:00)                                                  */
/* ======================================================================================================== */
  
void UpdateDump()
{
/* -------------------------------------------------------------------------------------------------------- */
/* Display TTP[] TEG[] TFB[]                                                                                */
/* -------------------------------------------------------------------------------------------------------- */
  xyprintf(1,0,"%s","Display TTP[] TEG[] TFB[]",1);

  for(hfc = 0; hfc < FCMAX; ++hfc)
  {
    xyprintf(1, 1 + hfc,"%s%s%7d%6d%6d", FC_code[hfc], "  ", TTP[hfc], TEG[hfc], TFB_timer[hfc],1);
  }
  
/* -------------------------------------------------------------------------------------------------------- */
/* overige informatie eenmaal per seconde updaten                                                           */
/* -------------------------------------------------------------------------------------------------------- */
  if (TS) {
    
    /* ---------------------------------------------------------------------------------------------------- */
    /* Initialisatie                                                                                        */
    /* ---------------------------------------------------------------------------------------------------- */
    for (hfc = 0; hfc < FCMAX; hfc++) {
      FC_A[hfc][dumpstap] = ' ';
    }
    for (hfc = 0; hfc < FCMAX; hfc++) {
      if (A[hfc] && !_ha[hfc]) {
        FC_A[hfc][dumpstap] = 'A';
      }
      if (!A[hfc] && _ha[hfc]) {
        FC_A[hfc][dumpstap] = 'E';
      }
      _ha[hfc] = (A[hfc] != 0);
    }
    
    /* ---------------------------------------------------------------------------------------------------- */
    /* Extra dumpinformatie                                                                                 */
    /* ---------------------------------------------------------------------------------------------------- */
    for (hfc = 0; hfc < FCMAX; hfc++) {
      if (CG[hfc] == CG_RV) { _FC[hfc][dumpstap] = ' '; }
      if (CG[hfc] == CG_RA) { _FC[hfc][dumpstap] = '.'; }
      if (CG[hfc] == CG_VS) { _FC[hfc][dumpstap] = 'S'; }
      if (CG[hfc] == CG_FG) { _FC[hfc][dumpstap] = '#'; }
      if (CG[hfc] == CG_WG) { _FC[hfc][dumpstap] = 'W'; }
      if (CG[hfc] == CG_VG) { _FC[hfc][dumpstap] = '['; }
      if (CG[hfc] == CG_MG) { _FC[hfc][dumpstap] = 'M'; }
      if (CG[hfc] == CG_GL) { _FC[hfc][dumpstap] = 'Z'; }
    }
    _UUR[dumpstap] = (char)CIF_KLOK[CIF_UUR];
    _MIN[dumpstap] = (char)CIF_KLOK[CIF_MINUUT];
    _SEC[dumpstap] = (char)CIF_KLOK[CIF_SECONDE];
    if (AKTPRG) _BLK[dumpstap] = STAP;
    else        _BLK[dumpstap] = BBLOK;
    dumpstap++;
    if (dumpstap >= MAXDUMPSTAP) dumpstap = 0;
    
  }
}  
