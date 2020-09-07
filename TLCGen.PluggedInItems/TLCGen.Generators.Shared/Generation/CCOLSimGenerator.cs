using System.Linq;
using System.Text;
using TLCGen.Models;
using System.Collections.Generic;
using System.Windows.Documents;

namespace TLCGen.Generators.Shared
{
    public static class CCOLSimGenerator
    {
        public static string GenerateSimC(
            ControllerModel controller, 
            string ts, 
            IEnumerable<DetectorSimulatieModel> externals, 
            ICCOLGeneratorSettingsProvider settingsProvider, 
            string pluginVersion,
            bool hasAddFile = true)
        {
            var sb = new StringBuilder();
            sb.AppendLine("/* SIMULATIE APPLICATIE */");
            sb.AppendLine("/* -------------------- */");
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateFileHeader(controller.Data, "sim.c", pluginVersion));
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateVersionHeader(controller.Data));
            sb.AppendLine();
            var prms = GenerateSimCSimulationParameters(controller, ts, externals, settingsProvider, out var lnkmax, hasAddFile);
            sb.Append(GenerateSimCExtraDefines(controller, lnkmax, hasAddFile));
            sb.AppendLine();
            sb.Append(GenerateSimCIncludes(controller, ts));
            sb.AppendLine();
            sb.Append(GenerateSimCSimulationDefaults(controller, ts));
            sb.AppendLine();
            sb.Append(prms);

            return sb.ToString();
        }

        private static string GenerateSimCIncludes(ControllerModel controller, string ts)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* include files */");
            sb.AppendLine("/* ------------- */");
            sb.AppendLine($"{ts}#include \"{controller.Data.Naam}sys.h\"");
            sb.AppendLine($"{ts}#include \"simvar.c\" /* simulatie variabelen */");


            return sb.ToString();
        }

        private static string GenerateSimCExtraDefines(ControllerModel controller, int lnkmax, bool hasAddFile)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"#define LNKMAX{(hasAddFile ? "1" : "")} {lnkmax} /* aantal links */");

            return sb.ToString();
        }

        private static string GenerateSimCSimulationDefaults(ControllerModel controller, string ts)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* Default waarden */");
            sb.AppendLine("/* --------------- */");
            sb.AppendLine("void simulation_defaults(void)");
            sb.AppendLine("{");
            sb.AppendLine($"{ts}S_defgenerator = NG;");
            sb.AppendLine($"{ts}S_defstopline  = 1800;");
            sb.AppendLine($"{ts}Q1_def         = 25;");
            sb.AppendLine($"{ts}Q2_def         = 50;");
            sb.AppendLine($"{ts}Q3_def         = 100;");
            sb.AppendLine($"{ts}Q4_def         = 200;");
            sb.AppendLine("}");

            return sb.ToString();
        }

        private static string GenerateSimCSimulationParameters(
            ControllerModel controller, 
            string ts, 
            IEnumerable<DetectorSimulatieModel> externals, 
            ICCOLGeneratorSettingsProvider settingsProvider,
            out int lnkmax,
            bool hasAddFile = true)
        {
            var sb = new StringBuilder();

            sb.AppendLine("void simulation_parameters(void)");
            sb.AppendLine("{");

            sb.AppendLine(" T_subrun_max  = 3600; /* subrun tijd                 */");
            sb.AppendLine(" RANDGEN_start = 123;  /* startwaarde randomgenerator */");

            sb.AppendLine();

            sb.AppendLine($"{ts}/* Link parameters */");
            sb.AppendLine($"{ts}/* --------------- */");

            var index = 0;
            var fasendets = controller.Fasen.SelectMany(x => x.Detectoren);
            var controllerdets = controller.Detectoren;
            var ovdummydets = controller.PrioData.GetAllDummyDetectors();
            var alldets = fasendets.Concat(controllerdets).Concat(ovdummydets).ToList();
            //var externals = PieceGenerators.Where(x => x.HasSimulationElements(controller)).SelectMany(x => x.GetSimulationElements(controller)).ToList();

            foreach (var dm in alldets.Where(x => !x.Dummy))
            {
                sb.AppendLine($"{ts}LNK_code[{index}] = \"{dm.Naam}\";");
                sb.AppendLine($"{ts}IS_nr[{index}] = { settingsProvider.GetPrefix("d") + dm.Naam};");
                sb.AppendLine($"{ts}FC_nr[{index}] = {(!string.IsNullOrWhiteSpace(dm.Simulatie.FCNr) && controller.Fasen.Any(x => x.Naam == dm.Simulatie.FCNr) ? settingsProvider.GetPrefix("fc") + dm.Simulatie.FCNr : "NG")};");
                sb.AppendLine($"{ts}S_generator[{index}] = NG;");
                sb.AppendLine($"{ts}S_stopline[{index}] = {dm.Simulatie.Stopline};");
                sb.AppendLine($"{ts}Q1[{index}] = {dm.Simulatie.Q1};");
                sb.AppendLine($"{ts}Q2[{index}] = {dm.Simulatie.Q2};");
                sb.AppendLine($"{ts}Q3[{index}] = {dm.Simulatie.Q3};");
                sb.AppendLine($"{ts}Q4[{index}] = {dm.Simulatie.Q4};");
                sb.AppendLine();
                ++index;
            }
            if(alldets.Any(x => x.Dummy) || externals.Any())
            {
                sb.AppendLine("#if (!defined AUTOMAAT_TEST)");
                foreach (var dm in alldets.Where(x => x.Dummy))
                {
                    sb.AppendLine($"{ts}LNK_code[{index}] = \"{dm.Naam}\";");
                    sb.AppendLine($"{ts}IS_nr[{index}] = { settingsProvider.GetPrefix("d") + dm.Naam};");
                    sb.AppendLine($"{ts}FC_nr[{index}] = {(!string.IsNullOrWhiteSpace(dm.Simulatie.FCNr) && controller.Fasen.Any(x => x.Naam == dm.Simulatie.FCNr) ? settingsProvider.GetPrefix("fc") + dm.Simulatie.FCNr : "NG")};");
                    sb.AppendLine($"{ts}S_generator[{index}] = NG;");
                    sb.AppendLine($"{ts}S_stopline[{index}] = {dm.Simulatie.Stopline};");
                    sb.AppendLine($"{ts}Q1[{index}] = {dm.Simulatie.Q1};");
                    sb.AppendLine($"{ts}Q2[{index}] = {dm.Simulatie.Q2};");
                    sb.AppendLine($"{ts}Q3[{index}] = {dm.Simulatie.Q3};");
                    sb.AppendLine($"{ts}Q4[{index}] = {dm.Simulatie.Q4};");
                    sb.AppendLine();
                    ++index;
                }
                foreach (var e in externals)
                {
                    sb.AppendLine($"{ts}LNK_code[{index}] = \"{e.RelatedName}\";");
                    sb.AppendLine($"{ts}IS_nr[{index}] = {settingsProvider.GetPrefix("ts") + e.RelatedName};");
                    sb.AppendLine($"{ts}FC_nr[{index}] = {(!string.IsNullOrWhiteSpace(e.FCNr) && controller.Fasen.Any(x => x.Naam == e.FCNr) ? settingsProvider.GetPrefix("fc") + e.FCNr : "NG")};");
                    sb.AppendLine($"{ts}S_generator[{index}] = NG;");
                    sb.AppendLine($"{ts}S_stopline[{index}] = {e.Stopline};");
                    sb.AppendLine($"{ts}Q1[{index}] = {e.Q1};");
                    sb.AppendLine($"{ts}Q2[{index}] = {e.Q2};");
                    sb.AppendLine($"{ts}Q3[{index}] = {e.Q3};");
                    sb.AppendLine($"{ts}Q4[{index}] = {e.Q4};");
                    sb.AppendLine();
                    ++index;
                }
                sb.AppendLine("#endif");
            }

            sb.AppendLine();

            if (hasAddFile)
            {
                sb.AppendLine($"{ts}/* Gebruikers toevoegingen file includen */");
                sb.AppendLine($"{ts}/* ------------------------------------- */");
                sb.AppendLine($"{ts}#include \"{controller.Data.Naam}sim.add\"");
                sb.AppendLine();
            }

            sb.AppendLine("}");

            lnkmax = index;

            return sb.ToString();
        }
    }
}
