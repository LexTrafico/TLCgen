using System;
using System.Linq;
using System.Text;
using TLCGen.Generators.Shared;
using TLCGen.Generators.Traffick.Settings;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.Traffick.CodeGeneration
{
    public partial class TraffickGenerator
    {
        public string GenerateRebC(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var sb = new StringBuilder();
            sb.AppendLine("/* REGELPROGRAMMA PROCEDURE */");
            sb.AppendLine("/* ------------------------ */");
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateFileHeader(c.Data, "reb.c", "1.0.0.0"));
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateVersionHeader(c.Data));
            sb.AppendLine();
            sb.Append(GenerateRebCBeforeIncludes(c));
            sb.Append(GenerateRebCIncludes(c));
            sb.Append(GenerateRebCTop(c));
            sb.Append(GenerateRebCInitApplication(c));
            sb.Append(GenerateRebCApplication(c));
            sb.Append(GenerateRebCSystemApplication(c));
            sb.Append(GenerateRebCDumpApplication(c));
            sb.Append(GenerateRegCSpecialSignals(c));
            
            return sb.ToString();
        }

        private string GenerateRebCBeforeIncludes(ControllerModel c)
        {
            var sb = new StringBuilder();

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCBeforeIncludes, false, true, false, true, ts);

            return sb.ToString();
        }

        private string GenerateRebCIncludes(ControllerModel c)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"#ifndef AUTOMAAT");
            sb.AppendLine($"{ts}#include \"appl_krw.c\"");
            sb.AppendLine($"#endif");
            
            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCIncludes, false, true, false, true, ts);
            
            return sb.ToString();
        }

        private string GenerateRebCTop(ControllerModel c)
        {
            var sb = new StringBuilder();

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCTop, false, true, false, true, ts);

            return sb.ToString();
        }
        
        private string GenerateRebCInitApplication(ControllerModel c)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"/* ------------------------ */");
            sb.AppendLine($"/* Initialisatie applicatie */");
            sb.AppendLine($"/* ------------------------ */");
            sb.AppendLine("void init_application(void)");
            sb.AppendLine("{");
            
            sb.AppendLine($"{ts}aantal_VAS = 0;");
            sb.AppendLine();

            sb.AppendLine($"{ts}/* ---------------------- */");
            sb.AppendLine($"{ts}/* Initialisatie regeling */");
            sb.AppendLine($"{ts}/* ---------------------- */");
            sb.AppendLine($"{ts}proc_init_reg();");
            sb.AppendLine();

            sb.AppendLine($"{ts}{ts}/* ----------------------------------------------- */");
            sb.AppendLine($"{ts}{ts}/* Indeling kruispuntarmen tbv hulpdienst ingrepen */");
            sb.AppendLine($"{ts}{ts}/* ----------------------------------------------- */");
            foreach (var fc in c.Fasen)
            {
                sb.AppendLine($"{ts}{ts}ARM[{_fcpf}{fc.Naam}] = 1;"); // TODO: use actual value
            }

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCInitApplication, false, true, false, true, ts);
            
            sb.AppendLine("}");
            sb.AppendLine();            
            
            return sb.ToString();
        }

        private string GenerateRebCApplication(ControllerModel c)
        {
            var prmitfb = TraffickGeneratorSettingsProvider.Default.GetElementName("prmitfb");
            var isfix = TraffickGeneratorSettingsProvider.Default.GetElementName("isfix");
            var prmagar = TraffickGeneratorSettingsProvider.Default.GetElementName("prmagar");
            var prmdfst = TraffickGeneratorSettingsProvider.Default.GetElementName("prmdfst");

            var sb = new StringBuilder();

            sb.AppendLine($"/* ----------------------- */");
            sb.AppendLine($"/* Specificatie applicatie */");
            sb.AppendLine($"/* ----------------------- */");
            sb.AppendLine("void application(void)");
            sb.AppendLine("{");
            sb.AppendLine($"{ts}/* ------------------- */");
            sb.AppendLine($"{ts}/* Automaat in bedrijf */");
            sb.AppendLine($"{ts}/* ------------------- */");
            sb.AppendLine($"{ts}BEDRYF = (CIF_WPS[CIF_PROG_STATUS] == CIF_STAT_REG);");
            sb.AppendLine();
            if (c.Data.FixatieMogelijk)
            {
                sb.AppendLine($"{ts}FIXATIE = (CIF_IS[{_ispf}{isfix}] != 0);");
                sb.AppendLine();
            }
            sb.AppendLine($"{ts}/* --------------- */");
            sb.AppendLine($"{ts}/* Garantie tijden */");
            sb.AppendLine($"{ts}/* --------------- */");
            sb.AppendLine($"{ts}proc_garantie_tijden();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------ */");
            sb.AppendLine($"{ts}/* Fasebewaking */");
            sb.AppendLine($"{ts}/* ------------ */");
            sb.AppendLine($"{ts}TFB_max = PRM[{_prmpf}{prmitfb}];");
            sb.AppendLine();
            sb.AppendLine($"{ts}if (TS) KNIP = !KNIP; /* tbv leds BP */");
            sb.AppendLine();
            
            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCKlokBehandeling, true, true, false, true, ts);
            
            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCGroentijden, true, true, false, true, ts);

            sb.AppendLine($"{ts}/* ------------------------------ */");
            sb.AppendLine($"{ts}/* Aanmaken klokgroen en klokrood */");
            sb.AppendLine($"{ts}/* ------------------------------ */");
            sb.AppendLine($"{ts}proc_klk_gr_r();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* --------------------- */");
            sb.AppendLine($"{ts}/* Reset stuurvariabelen */");
            sb.AppendLine($"{ts}/* --------------------- */");
            sb.AppendLine($"{ts}proc_reset_start();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------ */");
            sb.AppendLine($"{ts}/* Bepaal TTR[] */");
            sb.AppendLine($"{ts}/* ------------ */");
            sb.AppendLine($"{ts}proc_bepaal_ttr();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ---------------------------- */");
            sb.AppendLine($"{ts}/* Bepaal prioriteit toegestaan */");
            sb.AppendLine($"{ts}/* ---------------------------- */");
            sb.AppendLine($"{ts}proc_max_wt();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------------------------ */");
            sb.AppendLine($"{ts}/* Bepaal fasecyclus instellingen */");
            sb.AppendLine($"{ts}/* ------------------------------ */");
            foreach (var fc in c.Fasen)
            {
                sb.AppendLine($"{ts}proc_fc_instel({_fcpf}{fc.Naam}, {_prmpf}{prmagar}{fc.Naam}, {_prmpf}{prmdfst}{fc.Naam}, NG, NG, NG, NG, NG);");
            }
            sb.AppendLine();
            
            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCAanvragen, true, true, false, true, ts);
            
            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCVerlengen, true, true, false, true, ts);

            //sb.AppendLine($"{ts}/* -------------------------------- */");
            //sb.AppendLine($"{ts}/* Maatregelen bij detectie storing */");
            //sb.AppendLine($"{ts}/* -------------------------------- */");
            //foreach (var fc in c.Fasen.Where(x => x.Type == FaseTypeEnum.Auto))
            //{
            //    sb.AppendLine($"{ts}proc_det_mvt({_fcpf}{fc.Naam}, {_schpf}dst{fc.Naam}  , {_tpf}a{fc.Naam}      , d021      , d022      , NG        , NG        ,      ");
            //    sb.AppendLine($"{ts}{ts}{ts}{ts}{ts}{ts}{ts} NG        , tdhdst021 , NG        , NG        , NG        );");
            //}
            //sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------ */");
            sb.AppendLine($"{ts}/* BLOKindeling */");
            sb.AppendLine($"{ts}/* ------------ */");
            foreach (var fc in c.Fasen)
            {
                var ml = c.ModuleMolen.Modules.FirstOrDefault(x => x.Fasen.Any(x => x.FaseCyclus == fc.Naam));
                if (ml == null) continue;
                sb.AppendLine($"{ts}PBLK[{_fcpf}{fc.Naam}] = {c.ModuleMolen.Modules.IndexOf(ml) + 1};");
            }
            sb.AppendLine($"{ts}MAX_BLK = {c.ModuleMolen.Modules.Count};");
            sb.AppendLine();
            sb.AppendLine($"{ts}if (SSGD) init_sgd();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------------------------ */");
            sb.AppendLine($"{ts}/* Behandeling maximum tijdmeting */");
            sb.AppendLine($"{ts}/* ------------------------------ */");
            sb.AppendLine($"{ts}proc_maxgr_sgd();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* --------------------------- */");
            sb.AppendLine($"{ts}/* Bepaal primaire realisaties */");
            sb.AppendLine($"{ts}/* --------------------------- */");
            sb.AppendLine($"{ts}proc_rea_sgd();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------------------- */");
            sb.AppendLine($"{ts}/* Bepaal vooruitrealisaties */");
            sb.AppendLine($"{ts}/* ------------------------- */");
            sb.AppendLine($"{ts}proc_ppv_sgd();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* -------------------------------- */");
            sb.AppendLine($"{ts}/* Bepaal tijd tot einde groen auto */");
            sb.AppendLine($"{ts}/* -------------------------------- */");
            sb.AppendLine($"{ts}proc_TEG_sgd();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------------------------------ */");
            sb.AppendLine($"{ts}/* Bepaal tijd tot primaire realisaties */");
            sb.AppendLine($"{ts}/* ------------------------------------ */");
            sb.AppendLine($"{ts}proc_TTP_primair();");
            sb.AppendLine($"{ts}proc_TTP_versneld();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------------------------------------------- */");
            sb.AppendLine($"{ts}/* Bepaal toestemming alternatieve realisaties */");
            sb.AppendLine($"{ts}/* ------------------------------------------- */");
            sb.AppendLine($"{ts}proc_BAR_sgd();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ---------------------------------- */");
            sb.AppendLine($"{ts}/* Realiseer alternatieve realisaties */");
            sb.AppendLine($"{ts}/* ---------------------------------- */");
            sb.AppendLine($"{ts}proc_ALT_sgd();");
            sb.AppendLine();

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCMeeverlengGroen, true, true, false, true, ts);

            sb.AppendLine($"{ts}/* -------------------- */");
            sb.AppendLine($"{ts}/* Bepaal BLOK overgang */");
            sb.AppendLine($"{ts}/* -------------------- */");
            sb.AppendLine($"{ts}proc_nblok_sgd();");
            sb.AppendLine();

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCVeiligheidsGroen, true, true, false, true, ts);

            sb.AppendLine($"{ts}/* ---------------- */");
            sb.AppendLine($"{ts}/* Reset variabelen */");
            sb.AppendLine($"{ts}/* ---------------- */");
            sb.AppendLine($"{ts}proc_reset_einde();");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* ------- */");
            sb.AppendLine($"{ts}/* Fixatie */");
            sb.AppendLine($"{ts}/* ------- */");
            sb.AppendLine($"{ts}proc_fixatie();");
            sb.AppendLine();
            
            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCPostApplication, true, true, false, true, ts);

            //sb.AppendLine($"{ts}/* -------------------------------- */");
            //sb.AppendLine($"{ts}/* Display ingreep detectie storing */");
            //sb.AppendLine($"{ts}/* -------------------------------- */");
            //foreach (var fc in c.Fasen)
            //{
            //    sb.AppendLine($"{ts}CIF_GUS[usstoring{fc.Naam}] = BEDRYF && LEDST[{_fcpf}{fc.Naam}];");
            //}
            //sb.AppendLine();
            
            sb.AppendLine($"{ts}/* ----------------------- */");
            sb.AppendLine($"{ts}/* Display in testomgeving */");
            sb.AppendLine($"{ts}/* ----------------------- */");
            sb.AppendLine("#ifndef AUTOMAAT");
            sb.AppendLine($"{ts}UpdateDump();");
            sb.AppendLine("#endif");
            sb.AppendLine("}");
            sb.AppendLine();
            
            return sb.ToString();
        }

        private string GenerateRebCSystemApplication(ControllerModel c)
        {
            var usstap = TraffickGeneratorSettingsProvider.Default.GetElementName("usstap");
            var usblok = TraffickGeneratorSettingsProvider.Default.GetElementName("usblok");
            var usper = TraffickGeneratorSettingsProvider.Default.GetElementName("usper");

            var sb = new StringBuilder();

            sb.AppendLine($"/* ----------------- */");
            sb.AppendLine($"/* System applicatie */");
            sb.AppendLine($"/* ----------------- */");
            sb.AppendLine("void system_application(void)");
            sb.AppendLine("{");
            sb.AppendLine($"{ts}/* Automaat in bedrijf 1 */");
            sb.AppendLine($"{ts}BEDRYF  = (CIF_WPS[CIF_PROG_STATUS] == CIF_STAT_REG);");
            sb.AppendLine();
            sb.AppendLine($"{ts}/* Reset USsignalen indien automaat niet in bedrijf */");
            sb.AppendLine($"{ts}if (!BEDRYF)");
            sb.AppendLine($"{ts}{{");
            sb.AppendLine($"{ts}{ts}CIF_GUS[{_uspf}{usper}{c.PeriodenData.DefaultPeriodeNaam}] = FALSE;");
            foreach (var per in c.PeriodenData.Perioden.Where(x => x.Type == PeriodeTypeEnum.Groentijden))
            {
                sb.AppendLine($"{ts}{ts}CIF_GUS[{_uspf}{usper}{per.Naam}] = FALSE;");
            }
            sb.AppendLine();
            for (var ml = 1; ml <= c.ModuleMolen.Modules.Count; ml++)
            {
                sb.AppendLine($"{ts}{ts}CIF_GUS[{_uspf}{usblok}{ml}] = FALSE;");
            }

            sb.AppendLine($"{ts}{ts}CIF_GUS[{_uspf}{usstap}] = FALSE;");
            //sb.AppendLine();
            //sb.AppendLine($"{ts}{ts}CIF_GUS[{_uspf}storing02] = FALSE;");
            //sb.AppendLine($"{ts}{ts}CIF_GUS[{_uspf}storing05] = FALSE;");
            sb.AppendLine($"{ts}}}");

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCSystemApplication, false, true, false, true, ts);
            
            sb.AppendLine("}");
            sb.AppendLine();            
            
            return sb.ToString();
        }

        private string GenerateRebCDumpApplication(ControllerModel c)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"/* --------------- */");
            sb.AppendLine($"/* Dump applicatie */");
            sb.AppendLine($"/* --------------- */");
            sb.AppendLine("void dump_application(void)");
            sb.AppendLine("{");
            sb.AppendLine($"{ts}#ifdef TESTOMGEVING");
            sb.AppendLine($"{ts}{ts}#include \"dumpfile.c\"");
            sb.AppendLine($"{ts}#endif");

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCDumpApplication, false, true, false, true, ts);
            
            sb.AppendLine("}");
            sb.AppendLine();            
            
            return sb.ToString();
        }

        private string GenerateRegCSpecialSignals(ControllerModel c)
        {
            var sb = new StringBuilder();

            sb.AppendLine("#ifdef CCOL_IS_SPECIAL");
            sb.AppendLine("void is_special_signals(void)");
            sb.AppendLine("{");

            CCOLCodeGenerationHelper.AddCodeTypeToStringBuilder(c, OrderedPieceGenerators, sb, CCOLCodeTypeEnum.RebCSpecialSignals, true, true, false, true, ts);
            
            sb.AppendLine("}");
            sb.AppendLine("#endif");
            sb.AppendLine();

            return sb.ToString();
        }

        public static void GetDetectorArguments(FaseCyclusModel fc, StringBuilder sb, Predicate<DetectorModel> detPredicate, string prmName, string lineStart, string ts, string dpf, string prmpf)
        {
            var dc = 0;
            foreach (var d in fc.Detectoren.Where(x => detPredicate(x)))
            {
                if (dc % 3 == 0)
                {
                    sb.AppendLine();
                    sb.Append($"{ts}{lineStart}");
                }

                ++dc;
                sb.Append($"{dpf}{d.Naam}, {prmpf}{prmName}{d.Naam}");
                if (dc < 12) sb.Append(", ");
                if (dc >= 12) break;
            }

            for (; dc < 12; ++dc)
            {
                if (dc % 3 == 0)
                {
                    sb.AppendLine();
                    sb.Append($"{ts}{lineStart}");
                }

                sb.Append("NG, NG");
                if (dc < 11) sb.Append(", ");
            }
        }
    }
}
