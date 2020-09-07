using System.Collections.Generic;
using System.Linq;
using System.Text;
using TLCGen.Generators.Shared;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.Traffick.CodeGeneration
{
    public partial class TraffickGenerator
    {
        public string GenerateRegC(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var sb = new StringBuilder();
            sb.AppendLine("/* REGELPROGRAMMA */");
            sb.AppendLine("/* -------------- */");
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateFileHeader(c.Data, "reg.c", "1.0.0.0"));
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateVersionHeader(c.Data));
            sb.AppendLine();
            sb.Append(GenerateRegCBeforeIncludes(c));
            sb.Append(GenerateRegCIncludes(c));
            sb.Append(GenerateRegCTop(c));
            sb.Append(GenerateRegCVariables(c));
            sb.Append(GenerateRegCRegeling(c));

            return sb.ToString();
        }
        
        private string GenerateRegCBeforeIncludes(ControllerModel c)
        {
            var sb = new StringBuilder();

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RegCBeforeIncludes, false, true, false, true, ts);

            return sb.ToString();
        }

        private string GenerateRegCIncludes(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* include files */");
            sb.AppendLine("/* ------------- */");
            sb.AppendLine($"{ts}#include \"sysdef.c\"   /* definities                        */");
            sb.AppendLine($"{ts}#include \"{controller.Data.Naam}sys.h\"");
            sb.AppendLine($"{ts}#include \"dpvar.c\"    /* detectie elementen                */");
            sb.AppendLine($"{ts}#include \"isvar.c\"    /* ingangs elementen                 */");
            sb.AppendLine($"{ts}#include \"fcvar.c\"    /* fasecycli                         */");
            sb.AppendLine($"{ts}#include \"usvar.c\"    /* uitgangs elementen                */");
            sb.AppendLine($"{ts}#include \"kfvar.c\"    /* conflicten                        */");
            sb.AppendLine($"{ts}#include \"schvar.c\"   /* software schakelaars              */");
            sb.AppendLine($"{ts}#include \"ctvar.c\"    /* teller elementen                  */");
            sb.AppendLine($"{ts}#include \"hevar.c\"    /* hulp elementen                    */");
            sb.AppendLine($"{ts}#include \"mevar.c\"    /* geheugen elementen                */");
            sb.AppendLine($"{ts}#include \"tmvar.c\"    /* tijd elementen                    */");
            sb.AppendLine($"{ts}#include \"prmvar.c\"   /* parameters                        */");
            sb.AppendLine($"{ts}#include \"mlvar.c\"    /* modulen structuur                 */");
            if (controller.Data.GarantieOntruimingsTijden)
            {
                if(controller.Data.CCOLVersie >= CCOLVersieEnum.CCOL95 && controller.Data.Intergroen)
                {
                    sb.AppendLine($"{ts}#include \"tig_min.c\"   /* garantie-ontruimingstijden        */");
                }
                else
                {
                    sb.AppendLine($"{ts}#include \"to_min.c\"   /* garantie-ontruimingstijden        */");
                }
            }
            sb.AppendLine($"{ts}#include \"trg_min.c\"  /* garantie-roodtijden               */");
            sb.AppendLine($"{ts}#include \"tgg_min.c\"  /* garantie-groentijden              */");
            sb.AppendLine($"{ts}#include \"tgl_min.c\"  /* garantie-geeltijden               */");
            sb.AppendLine($"{ts}#include \"stdio.h\"    /* standaard in- output              */");
            sb.AppendLine($"{ts}#include \"bericht.h\"  /* berichtenbuffer                   */");
            sb.AppendLine($"{ts}#include \"cif.inc\"    /* cvn-c-routines                    */");
            if(controller.Data.VLOGType != VLOGTypeEnum.Geen)
            {
                sb.AppendLine($"{ts}#ifndef NO_VLOG");
                sb.AppendLine($"{ts}{ts}#include \"vlogvar.c\"  /* variabelen t.b.v. vlogfuncties                */");
                sb.AppendLine($"{ts}{ts}#include \"logvar.c\"   /* variabelen t.b.v. logging                     */");
                sb.AppendLine($"{ts}{ts}#include \"monvar.c\"   /* variabelen t.b.v. realtime monitoring         */");
                sb.AppendLine($"{ts}{ts}#include \"fbericht.h\"");
                sb.AppendLine($"{ts}#endif");
            }
            sb.AppendLine($"{ts}#include \"prsvar.c\"   /* parameters parser                 */");
            sb.AppendLine($"{ts}#include \"control.c\"  /* controller interface              */");
            sb.AppendLine($"{ts}#include \"rtappl.h\"   /* applicatie routines               */");
            sb.AppendLine($"{ts}#ifdef TESTOMGEVING");
            sb.AppendLine($"{ts}{ts}#ifndef AUTOMAAT");
            sb.AppendLine($"{ts}{ts}{ts}#include \"xyprintf.h\"");
            sb.AppendLine($"{ts}{ts}#endif /* *** ifndef -> AUTOMAAT */");
            sb.AppendLine($"{ts}#endif /* *** ifdef -> TESTOMGEVING */");
            sb.AppendLine($"{ts}");
            
            sb.AppendLine();
            var ccolVer = controller.Data.CCOLVersie switch
            {
                CCOLVersieEnum.CCOL8 => "80",
                CCOLVersieEnum.CCOL9 => "90",
                CCOLVersieEnum.CCOL95 => "95",
                CCOLVersieEnum.CCOL100 => "100",
                CCOLVersieEnum.CCOL110 => "110",
                _ => throw new System.NotImplementedException(),
            };
            sb.AppendLine($"{ts}#define CCOL_versie {ccolVer}");
            if (!controller.Data.Intergroen)
            {
                sb.AppendLine($"{ts}#define NO_TIGMAX");
            }

            return sb.ToString();
        }

        private string GenerateRegCTop(ControllerModel c)
        {
            var sb = new StringBuilder();

            foreach (var gen in OrderedPieceGenerators[CCOLCodeTypeEnum.RegCTop])
            {
                sb.Append(gen.Value.GetCode(c, CCOLCodeTypeEnum.RegCTop, ts));
            }
            sb.AppendLine();

            return sb.ToString();
        }

        private string GenerateRegCVariables(ControllerModel c)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"/* ------------------------------------ */");
            sb.AppendLine($"/* Declaratie variabelen regelstructuur */");
            sb.AppendLine($"/* ------------------------------------ */");
            sb.AppendLine($"{ts}mulv KG[FCMAX];                       /* klokgroen array                                                  */");
            sb.AppendLine($"{ts}mulv HG[FCMAX];                       /* klokgroen array (hulp-array)                                     */");
            sb.AppendLine($"{ts}mulv KR[FCMAX];                       /* klokrood  array                                                  */");
            sb.AppendLine($"{ts}mulv KGCA[FCMAX];                     /* klokgroen na conflict-aanvraag                                   */");
            sb.AppendLine($"{ts}mulv KGOM[FCMAX];                     /* klokgroen tbv ondermaximum busingreep                            */");
            sb.AppendLine($"{ts}mulv PRGR[FCMAX];                     /* klok primair groen                                               */");
            sb.AppendLine($"{ts}mulv KRVG[FCMAX];                     /* klok RVG                                                         */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool KNIP;                            /* knippersignaal tbv leds BP                                       */");
            sb.AppendLine($"{ts}bool BEDRYF;                          /* automaat in bedrijf                                              */");
            sb.AppendLine($"{ts}bool FIXATIE;                         /* automaat in fixatie                                              */");
            sb.AppendLine($"{ts}bool SOMEVG;                          /* sommatie EVG##                                                   */");
            sb.AppendLine($"{ts}bool SPR;                             /* start programma                                                  */");
            sb.AppendLine($"{ts}bool SBL;                             /* start BLOK                                                       */");
            sb.AppendLine($"{ts}bool Hreset;                          /* hulpvariabele tbv start SGD-VAS                                  */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv MAX_BLK;                         /* aantal blokken regelstructuur SGD                                */");
            sb.AppendLine($"{ts}mulv BLOK,BBLOK;                      /* BLOK-teller                                                      */");
            sb.AppendLine($"{ts}mulv STAP;                            /* stappenteller VAS                                                */");
            sb.AppendLine($"{ts}mulv AKTPRG;                          /* aktief  programma (0 = SGD)                                      */");
            sb.AppendLine($"{ts}mulv GEWPRG;                          /* gewenst programma (0 = SGD)                                      */");
            sb.AppendLine($"{ts}mulv WBLK;                            /* wachtblok                                                        */");
            sb.AppendLine($"{ts}bool WACHT;                           /* wachtblok aktief                                                 */");
            sb.AppendLine($"{ts}mulv _dvm=0;");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool AFC[FCMAX];                      /* aktieve fictieve conflicten                                      */");
            sb.AppendLine($"{ts}bool BAR[FCMAX];                      /* blokkeer alternatieve realisatie                                 */");
            sb.AppendLine($"{ts}mulv VFA[FCMAX];                      /* voorlopige fietsaanvraag aanwezig                                */");
            sb.AppendLine($"{ts}bool AIGL[FCMAX];                     /* onthoud aanvraag in geel                                         */");
            sb.AppendLine($"{ts}mulv TTPX[FCMAX];                     /* TTP[] tbv bepaling toest. ALT.realisatie                         */");
            sb.AppendLine($"{ts}mulv TEGX[FCMAX];                     /* TEG[] tbv bepaling toest. ALT.realisatie                         */");
            sb.AppendLine($"{ts}mulv ALTM[FCMAX];                     /* ondergrens V.A.-groen bij ALT.realisatie                         */");
            sb.AppendLine($"{ts}mulv KAPM[FCMAX];                     /* ondergrens V.A.-groen bij PRI.realisatie                         */");
            sb.AppendLine($"{ts}mulv TKMX[FCMAX];                     /* terugkomen na afkappen toegestaan                                */");
            sb.AppendLine($"{ts}mulv PRIM[FCMAX];                     /* maximumgroenduur primaire realisatie                             */");
            sb.AppendLine($"{ts}mulv PBLK[FCMAX];                     /* primaire BLOK indeling                                           */");
            sb.AppendLine($"{ts}mulv ARM[FCMAX];                      /* kruispuntarm tbv hulpdienstingreep                               */");
            sb.AppendLine($"{ts}mulv VARM[FCMAX];                     /* volgarm      tbv hulpdienstingreep                               */");
            sb.AppendLine($"{ts}mulv HVIN[FCMAX];                     /* hulpdienst ingreep                                               */");
            sb.AppendLine($"{ts}mulv HLPD[FCMAX][6];");
            sb.AppendLine($"{ts}bool _pri_akt[FCMAX];");
            sb.AppendLine($"{ts}bool FILE_VOOR_SS=FALSE;");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv SGD[FCMAX];                      /* SGD array -> hulpwaarden                                         */");
            sb.AppendLine($"{ts}mulv HKI[FCMAX];                      /* HKI array -> hulpwaarden                                         */");
            sb.AppendLine($"{ts}mulv HKII[FCMAX];                     /* HKI array -> hulpwaarden                                         */");
            sb.AppendLine($"{ts}mulv POV[FCMAX];                      /* BUS array -> hulpwaarden                                         */");
            sb.AppendLine($"{ts}mulv BUS1[FCMAX];                     /* BUS aktuele aanwezigheidstijd                                    */");
            sb.AppendLine($"{ts}mulv PEL[FCMAX];                      /* PEL array -> hulpwaarden                                         */");
            sb.AppendLine($"{ts}mulv PEL1[FCMAX];                     /* PEL aktuele aanwezigheidstijd                                    */");
            sb.AppendLine($"{ts}mulv PEL2[FCMAX];                     /* PEL uitverlengmaximum na ingreep                                 */");
            sb.AppendLine($"{ts}mulv RISPEL[FCMAX];                   /* RISPEL array -> hulpwaarden                                      */");
            sb.AppendLine($"{ts}mulv RISPEL1[FCMAX];                  /* RISPEL aktuele aanwezigheidstijd                                 */");
            sb.AppendLine($"{ts}mulv RISPEL2[FCMAX];                  /* RISPEL uitverlengmaximum na ingreep                              */");
            sb.AppendLine($"{ts}mulv TOV[FCMAX];                      /* TOV    array -> hulpwaarden                                      */");
            sb.AppendLine($"{ts}mulv VSM[FCMAX];                      /* VSM    array -> hulpwaarden                                      */");
            sb.AppendLine($"{ts}mulv VSM_ref[FCMAX];                  /* VSM    array -> referentie waarden                               */");
            sb.AppendLine($"{ts}mulv vracht1[FCMAX];                  /* hulpwaarde tbv lengte gevoelige det.                             */");
            sb.AppendLine($"{ts}mulv vracht2[FCMAX];                  /* hulpwaarde tbv lengte gevoelige det.                             */");
            sb.AppendLine($"{ts}mulv WSP[FCMAX];                      /* WSP    array -> hulpwaarden                                      */");
            sb.AppendLine($"{ts}mulv WSP_ref[FCMAX];                  /* WSP    array -> referentie waarden                               */");
            sb.AppendLine($"{ts}mulv FTS[FCMAX];                      /* FTS    array -> hulpwaarden                                      */");
            sb.AppendLine($"{ts}mulv RISFTS[FCMAX];                   /* RISFTS array -> hulpwaarden                                      */");
            sb.AppendLine($"{ts}mulv KAPC[FCMAX];                     /* Prioriteit mag conflicten afbreken                               */");
            sb.AppendLine($"{ts}mulv KVOV[FCMAX];                     /* Afkapvertraging OV prioriteit                                    */");
            sb.AppendLine($"{ts}mulv KVPK[FCMAX];                     /* Afkapvertraging pelotonkoppeling                                 */");
            sb.AppendLine($"{ts}mulv KVRPK[FCMAX];                    /* Afkapvertraging pelotonkoppeling RIS                             */");
            sb.AppendLine($"{ts}mulv KVFC[FCMAX];                     /* Afkapvertraging fasecyclus                                       */");
            sb.AppendLine($"{ts}mulv HPRI[FCMAX];                     /* Prioriteit toegestaan hulpvariabele                              */");
            sb.AppendLine($"{ts}mulv BPI[FCMAX];                      /* Blokkeer prioriteitsingreep                                      */");
            sb.AppendLine($"{ts}mulv TTK[FCMAX];                      /* tijd tot kappen                                                  */");
            sb.AppendLine($"{ts}mulv TTR[FCMAX];                      /* tijd tot realisatie                                              */");
            sb.AppendLine($"{ts}mulv TTC[FCMAX];                      /* tijd tot realisatie NIET gecorrigeerd                            */");
            sb.AppendLine($"{ts}mulv TTP[FCMAX];                      /* tijd tot primaire realisatie                                     */");
            sb.AppendLine($"{ts}mulv GTTP[FCMAX];                     /* gewenste startmoment bus voor stopstreep passage                 */");
            sb.AppendLine($"{ts}mulv TEG[FCMAX];                      /* tijd tot einde groen                                             */");
            sb.AppendLine($"{ts}mulv PRTEG[FCMAX];                    /* tijd tot einde groen igv terugkomen                              */");
            sb.AppendLine($"{ts}mulv _HOT[FCMAX];                     /* herstart ontruimingstijd (LHOVRA - R)                            */");
            sb.AppendLine($"{ts}bool DZ[FCMAX];                       /* dilemma-zone-meting                                              */");
            sb.AppendLine($"{ts}mulv STAR[FCMAX];                     /* tbv STAR uitverlengen bij det.storingen                          */");
            sb.AppendLine($"{ts}bool RAT[FCMAX];                      /* tbv aantsturen rateltikkers                                      */");
            sb.AppendLine($"{ts}bool RATGEL[FCMAX];                   /* tbv rateltikkers in testomgeving                                 */");
            sb.AppendLine($"{ts}mulv RATGN[FCMAX];                    /* tbv extra groen bij rateltikkers ZNST                            */");
            sb.AppendLine($"{ts}mulv KWCOV[FCMAX];                    /* tbv OV ingreep naar kwaliteitscentrale                           */");
            sb.AppendLine($"{ts}bool FCDVM[FCMAX];                    /* tbv hogere hiaattijden tijdens DVM prg.                          */");
            sb.AppendLine($"{ts}mulv OVUIT[FCMAX];                    /* buffer busuitmeldingen                                           */");
            sb.AppendLine($"{ts}mulv VLOG_OV[FCMAX];                  /* tbv VLOG OV- en HLPD bits                                        */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv _DOS[FCMAX];                     /* tbv doseren   bij FILE stroomafwaarts                            */");
            sb.AppendLine($"{ts}mulv _FBL[FCMAX];                     /* tbv blokkeren bij FILE stroomafwaarts                            */");
            sb.AppendLine($"{ts}mulv _FIL[FCMAX];                     /* tbv fc status bij FILE stroomafwaarts                            */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool LEDST[FCMAX];                    /* led detectie storing                                             */");
            sb.AppendLine($"{ts}bool DF[DPMAX];                       /* buffer detectie-storing                                          */");
            sb.AppendLine($"{ts}bool _VG4[DPMAX];                     /* aanhouden 4e VAG                                                 */");
            sb.AppendLine($"{ts}mulv _VT4[DPMAX];                     /* buffer volgtijden tbv veiligheidsgroen                           */");
            sb.AppendLine($"{ts}mulv _VT4_old[DPMAX];                 /* buffer volgtijden tbv veiligheidsgroen (vorig)                   */");
            sb.AppendLine($"{ts}mulv _TDH[DPMAX];                     /* hiaatmeting tbv DVM programma's                                  */");
            sb.AppendLine($"{ts}bool BLK_PRI_REA;                     /* blokkeer prioriteitsingrepen                                     */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv FC_type[FCMAX];                  /* fasecyclus type                                                  */");
            sb.AppendLine($"{ts}mulv DE_type[DPMAX];                  /* detector type                                                    */");
            sb.AppendLine($"{ts}mulv TGGL_max[FCMAX],hi,hj;           /* garantie geeltijden                                              */");
            sb.AppendLine($"{ts}mulv TGO_max[FCMAX][FCMAX];           /* garantie ontruimingstijden                                       */");
            sb.AppendLine($"{ts}bool MV_matrix[FCMAX][FCMAX];         /* meeverleng matrix tbv C-its                                      */");
            sb.AppendLine($"{ts}mulv DZ_tijd[FCMAX];                  /* VAG4 tijd tbv voorspellen maximum tijd tot einde groen           */");
            sb.AppendLine($"{ts}mulv AVR_GEBIED[FCMAX];               /* aantal voertuigen in C- its aanvraag gebied                      */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* -------------------------------- */");
            sb.AppendLine($"/* Declaratie tijdelijke conflicten */");
            sb.AppendLine($"/* -------------------------------- */");
            sb.AppendLine($"{ts}bool TMPc[FCMAX][FCMAX];              /* temporary conflictmatrix (primaire conflicten)                   */");
            sb.AppendLine($"{ts}bool TMPf[FCMAX][FCMAX];              /* temporary conflictmatrix (fictieve conflicten)                   */");
            sb.AppendLine($"{ts}mulv TMPi[FCMAX][FCMAX];              /* temporary intergroentijden matrix                                */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ---------------------------------- */");
            sb.AppendLine($"/* Declaratie tbv bijzondere ingrepen */");
            sb.AppendLine($"/* ---------------------------------- */");
            sb.AppendLine($"{ts}bool SPOOR     = FALSE;               /* SPOOR programma                                                  */");
            sb.AppendLine($"{ts}bool BRUG      = FALSE;               /* BRUG programma                                                   */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ------------------------------------ */");
            sb.AppendLine($"/* Declaratie tbv wachttijdvoorspellers */");
            sb.AppendLine($"/* ------------------------------------ */");
            sb.AppendLine($"{ts}mulv TWV[FCMAX];                      /* Wachttijd tbv wachttijdvoorspeller                               */");
            sb.AppendLine($"{ts}mulv EGW[FCMAX];                      /* Tijd tot einde groen voorspelling                                */");
            sb.AppendLine($"{ts}mulv TTW[FCMAX];                      /* Tijd tot realisatie in 0.1 sec.                                  */");
            sb.AppendLine($"{ts}mulv Aled[FCMAX];                     /* Aantal resterende ledjes wt voorsp.                              */");
            sb.AppendLine($"{ts}mulv Adwt[FCMAX];                     /* Tijd na  afvallen laatste ledje                                  */");
            sb.AppendLine($"{ts}mulv Tdwt[FCMAX];                     /* Tijd tot afvallen volgend ledje                                  */");
            sb.AppendLine($"{ts}bool BLWT[FCMAX];                     /* Blokkeer conflicterende busingreep                               */");
            sb.AppendLine($"{ts}bool _GRN[FCMAX];                     /* Hulparray RVG[] tbv wt voorspeller                               */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv WTVFC[FCMAX];");
            sb.AppendLine($"{ts}mulv halt_tim[FCMAX];");
            sb.AppendLine($"{ts}bool halteer_wtv[FCMAX];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ------------------------- */");
            sb.AppendLine($"/* Declaratie tbv definities */");
            sb.AppendLine($"/* ------------------------- */");
            sb.AppendLine($"{ts}mulv aantal_vtg_fts;");
            sb.AppendLine($"{ts}mulv kp_vtg_fts[FCMAX][17];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv aant_hki;");
            sb.AppendLine($"{ts}mulv hki[FCMAX][21];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool NEST = FALSE;");
            sb.AppendLine($"{ts}mulv _DCF[FCMAX];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv aantal_dcf_vs;");
            sb.AppendLine($"{ts}mulv dcf_vs[FCMAX][6];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv aantal_dcf_gs;");
            sb.AppendLine($"{ts}mulv dcf_gs[FCMAX][6];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}/* maximum voertuigbuffer tbv V-smile per richting                                                        */");
            sb.AppendLine($"{ts}#define MAX_VTG_VSM     10");
            sb.AppendLine($"{ts}mulv momenten_vsm[FCMAX][MAX_VTG_VSM][2];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}/* maximum voertuigbuffer tbv wachtstandplus per richting                                                 */");
            sb.AppendLine($"{ts}#define MAX_VTG_WSP     20");
            sb.AppendLine($"{ts}mulv momenten_wsp[FCMAX][MAX_VTG_WSP][4];");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ------------------ */");
            sb.AppendLine($"/* Declaratie tbv VAS */");
            sb.AppendLine($"/* ------------------ */");
            sb.AppendLine($"{ts}bool SVAS,SSGD,ESGD,OMSCH,N_ts;");
            sb.AppendLine($"{ts}mulv MKODE,CKODE,HKODE,LKODE;");
            sb.AppendLine($"{ts}mulv CTYD,IPNT,OPNT,CPNT;");
            sb.AppendLine($"{ts}mulv MKODE_oud,aantal_VAS;");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}mulv FCA[FCMAX];                      /* fictieve aanvraag aanwezig                                       */");
            sb.AppendLine($"{ts}mulv VAS1[FCMAX];                     /* VAS array -> einde uitstel                                       */");
            sb.AppendLine($"{ts}mulv VAS2[FCMAX];                     /* VAS array -> start primair                                       */");
            sb.AppendLine($"{ts}mulv VAS3[FCMAX];                     /* VAS array -> start verleng                                       */");
            sb.AppendLine($"{ts}mulv VAS4[FCMAX];                     /* VAS array -> einde primair                                       */");
            sb.AppendLine($"{ts}mulv VAS5[FCMAX];                     /* VAS array -> einde groen                                         */");
            sb.AppendLine($"{ts}mulv HVAS[FCMAX];                     /* VAS array -> hulpwaarden                                         */");
            sb.AppendLine($"{ts}mulv MAXG[FCMAX];                     /* Max.groen tbv KWC                                                */");
            sb.AppendLine($"{ts}mulv VASR[FCMAX];                     /* VAS aktieve realisatie                                           */");
            sb.AppendLine($"{ts}mulv TTUV[FCMAX];                     /* VAS tijd tot einde uitstel                                       */");
            sb.AppendLine($"{ts}bool VG1[FCMAX];                      /* tbv bepalen YV[] bij CRSV                                        */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool Straag;                          /* Vertraag stappenraster                                           */");
            sb.AppendLine($"{ts}bool Svsnel;                          /* Versnel  stappenraster                                           */");
            sb.AppendLine($"{ts}bool Sstart;                          /* Herstart stappenraster                                           */");
            sb.AppendLine($"{ts}bool Sblok;                           /* Blokkeer stappenraster                                           */");
            sb.AppendLine($"{ts}bool Sdubb;                           /* Verhoog  stappenraster                                           */");
            sb.AppendLine($"{ts}mulv Sverschil;                       /* Aktueel synch.verschil                                           */");
            sb.AppendLine($"{ts}mulv Sversnel;                        /* Hulpwaarde tbv versnellen                                        */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ------------------- */");
            sb.AppendLine($"/* Defines mbt FC_type */");
            sb.AppendLine($"/* ------------------- */");
            sb.AppendLine($"{ts}#define RESERVE        0");
            sb.AppendLine($"{ts}#define AUTO           1");
            sb.AppendLine($"{ts}#define FIETS          2");
            sb.AppendLine($"{ts}#define VOETGANGER     3");
            sb.AppendLine($"{ts}#define OV             4");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ------------------- */");
            sb.AppendLine($"/* Defines mbt DE_type */");
            sb.AppendLine($"/* ------------------- */");
            sb.AppendLine($"{ts}#define RESERVE        0");
            sb.AppendLine($"{ts}#define KOPLUS         1");
            sb.AppendLine($"{ts}#define LANGE_LUS      2");
            sb.AppendLine($"{ts}#define VERWEGLUS      3");
            sb.AppendLine($"{ts}#define DRUKKNOP       4");
            sb.AppendLine($"{ts}#define OVERIG         5");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ---------------------------- */");
            sb.AppendLine($"/* Klok afhankelijke variabelen */");
            sb.AppendLine($"/* ---------------------------- */");
            foreach (var p in c.PeriodenData.Perioden.Where(x => x.Type == PeriodeTypeEnum.Groentijden))
            { 
                sb.AppendLine($"{ts}bool prog_{p.Naam}   = FALSE;          /* KLOK {p.Naam}                                                     */");
            }
            sb.AppendLine($"{ts}bool prog_{c.PeriodenData.DefaultPeriodeNaam}   = FALSE;          /* KLOK {c.PeriodenData.DefaultPeriodeNaam}                                                     */");
            sb.AppendLine($"{ts}bool negeer_cpc     = FALSE;          /* negeer complexcoordinator                                        */");
            sb.AppendLine($"{ts}bool klok_knip      = FALSE;          /* KLOK knipperen VRI                                               */");
            sb.AppendLine($"{ts}bool klok_doven     = FALSE;          /* KLOK doven VRI                                                   */");
            
            // TODO this is provisional, should be removed!!!
            sb.AppendLine($"{ts}bool prog_ochtend   = FALSE;          /* TODO; should be removed!!!                                       */");
            sb.AppendLine($"{ts}bool prog_avond     = FALSE;          /* TODO; should be removed!!!                                       */");
            sb.AppendLine($"{ts}bool prog_koopavond = FALSE;          /* TODO; should be removed!!!                                       */");
            sb.AppendLine($"{ts}bool prog_zaterdag  = FALSE;          /* TODO; should be removed!!!                                       */");
            sb.AppendLine($"{ts}bool prog_nacht     = FALSE;          /* TODO; should be removed!!!                                       */");
            sb.AppendLine($"{ts}bool prog_extra     = FALSE;          /* TODO; should be removed!!!                                       */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool cpc_prog_vas1  = FALSE;          /* cpc progkeuze VAS 1                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas2  = FALSE;          /* cpc progkeuze VAS 2                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas3  = FALSE;          /* cpc progkeuze VAS 3                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas4  = FALSE;          /* cpc progkeuze VAS 4                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas5  = FALSE;          /* cpc progkeuze VAS 5                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas6  = FALSE;          /* cpc progkeuze VAS 6                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas7  = FALSE;          /* cpc progkeuze VAS 7                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas8  = FALSE;          /* cpc progkeuze VAS 8                                              */");
            sb.AppendLine($"{ts}bool cpc_prog_vas9  = FALSE;          /* cpc progkeuze VAS 9                                              */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool geen_pov       = FALSE;          /* GEEN bus prio                                                    */");
            sb.AppendLine($"{ts}bool geen_pel       = FALSE;          /* GEEN pel prio                                                    */");
            sb.AppendLine($"{ts}bool geen_fts       = FALSE;          /* GEEN fts prio                                                    */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool v_smile_stat   = FALSE;          /* V-smile statistiek                                               */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"{ts}bool rateltikkers   = FALSE;          /* rateltikkers                                                     */");
            sb.AppendLine($"{ts}bool dim_tikkers    = FALSE;          /* dimmen rateltikkers                                              */");
            sb.AppendLine($"{ts}");
            sb.AppendLine($"/* ------------------------------------------------ */");
            sb.AppendLine($"/* Declaratie ten behoeve van extra dump informatie */");
            sb.AppendLine($"/* ------------------------------------------------ */");
            sb.AppendLine($"{ts}#ifdef TESTOMGEVING");
            sb.AppendLine($"{ts}{ts}bool _ha[FCMAX];");
            sb.AppendLine($"{ts}{ts}count hfc;");
            sb.AppendLine($"{ts}{ts}");
            sb.AppendLine($"{ts}{ts}#define MAXDUMPSTAP 300");
            sb.AppendLine($"{ts}{ts}");
            sb.AppendLine($"{ts}{ts}FILE *fp;");
            sb.AppendLine($"{ts}{ts}char _UUR[MAXDUMPSTAP];");
            sb.AppendLine($"{ts}{ts}char _MIN[MAXDUMPSTAP];");
            sb.AppendLine($"{ts}{ts}char _SEC[MAXDUMPSTAP];");
            sb.AppendLine($"{ts}{ts}mulv _BLK[MAXDUMPSTAP];");
            sb.AppendLine($"{ts}{ts}char _FC[FCMAX][MAXDUMPSTAP];");
            sb.AppendLine($"{ts}{ts}char FC_A[FCMAX][MAXDUMPSTAP];");
            sb.AppendLine($"{ts}{ts}mulv dumpstap = 0;");
            sb.AppendLine($"{ts}{ts}mulv di,dj;");
            sb.AppendLine($"{ts}#endif /* *** ifdef -> TESTOMGEVING */");
            sb.AppendLine();

            return sb.ToString();
        }

        private string GenerateRegCRegeling(ControllerModel c)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"/* -------- */");
            sb.AppendLine($"/* Regeling */");
            sb.AppendLine($"/* -------- */");
            sb.AppendLine($"{ts}#include \"appl_pro.c\" /* standaard functies */");
            sb.AppendLine($"{ts}#include \"{c.Data.Naam}tab.c\" /* regelparameters */");
            sb.AppendLine($"{ts}#include \"{c.Data.Naam}reb.c\" /* regelprogramma */");
            
            return sb.ToString();
        }
    }
}
