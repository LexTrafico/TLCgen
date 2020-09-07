/* ======================================================================================================== */
/* Fick CCOL generator                                                              Versie 1.7 / 4 sep 2020 */
/* -------------------------------------------------------------------------------------------------------- */
/* Kruispuntnummer:                      0001                                                               */
/* Locatie:                              -                                                                  */
/* Plaats:                               -                                                                  */
/* Ontwerp:                              -                                                                  */
/* User interface:                       Traffick Solutions - Ferry van den Heuvel                          */
/* -------------------------------------------------------------------------------------------------------- */
/* dumpfile.c                                                                                               */
/* Versie:                               1.0                                                                */
/* Door:                                 -                                                                  */
/* Datum generatie:                      07-09-2020                                                         */
/*  Tijd generatie:                      08:32:25 (+02:00)                                                  */
/* ======================================================================================================== */
  
/* -------------------------------------------------------------------------------------------------------- */
/* Definities                                                                                               */
/* -------------------------------------------------------------------------------------------------------- */
  /* Open dumpfile                                                                                          */
  
  fp = fopen("dumpfile.dmp", "wb");
  if (fp != NULL) {
    
    di = dumpstap + 1;
    
    /* Dump de huidige tijd                                                                                 */
    
    if (di >= MAXDUMPSTAP) di = 0;
    fprintf(fp, "Datum : %02d-%02d-%02d\r\n", CIF_KLOK[CIF_DAG],
      CIF_KLOK[CIF_MAAND],
      CIF_KLOK[CIF_JAAR]);
    
    /* Loop tot dumpstap                                                                                    */
    
    while (di != dumpstap) {
      if ((_UUR[di] > 0) || (_MIN[di] > 0) || (_SEC[di] > 0)) {
        fprintf(fp,"%02d:%02d:%02d",_UUR[di],_MIN[di],_SEC[di]);
        fprintf(fp,"  %4d",_BLK[di]);
        for(dj = 0; dj < FCMAX; dj++) {
          if ((FC_A[dj][di] != 'A') && (FC_A[dj][di] != 'E')) {
            fprintf(fp,"%3c",_FC[dj][di]);
          } else {
            fprintf(fp,"%3c",FC_A[dj][di]);
          }
        }
        fprintf(fp,"\r\n");
        if ((_SEC[di] == 0) || (_SEC[di] == 30)) {
          fprintf(fp,"              ");
          for(dj = 0; dj < FCMAX; dj++) {
            fprintf(fp,"%3s",FC_code[dj]);
          }
          fprintf(fp,"\r\n");
        }
      }
      
      /* Ga naar volgende                                                                                   */
      
      di++;
      if (di >= MAXDUMPSTAP) di = 0;
    }
    
    /* Sluit het bestand                                                                                    */
    
    fclose(fp);
  }
  
