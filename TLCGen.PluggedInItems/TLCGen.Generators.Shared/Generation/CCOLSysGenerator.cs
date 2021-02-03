using System.Collections.Generic;
using System.Linq;
using System.Text;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.Shared
{
    public static class CCOLSysGenerator
    {
        private static string _ts;
        private static string _fcpf;
        private static CCOLGenerationData _generationData;
        private static ICCOLGeneratorSettingsProvider _settingsProvider;

        public static string GenerateSysH(
            ControllerModel c, 
            CCOLGenerationData generationData,
            string ts, 
            Dictionary<CCOLCodeTypeEnum, SortedDictionary<int, ICCOLCodePieceGenerator>> orderedGenerators, 
            ICCOLGeneratorSettingsProvider settingsProvider,
            string pluginVersion,
            bool countOnwardsIo = false,
            bool hasAddFile = true)
        {
            _ts = ts;
            _generationData = generationData;
            _settingsProvider = settingsProvider;
            _fcpf = settingsProvider.GetPrefix("fc");

            var sb = new StringBuilder();
            sb.AppendLine("/* ALGEMENE APPLICATIEFILE */");
            sb.AppendLine("/* ----------------------- */");
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateFileHeader(c.Data, "sys.h", pluginVersion));
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateVersionHeader(c.Data));
            sb.AppendLine();
            sb.AppendLine($"#define SYSTEM \"{c.Data.Naam}\"");
            if (c.Data.AanmakenVerionSysh)
            {
                var ver = c.Data.Versies.LastOrDefault();
                if (ver!= null)
                {
                    sb.AppendLine($"#define VERSION \"{ver.Versie} {ver.Datum:yyyyMMdd}\"");
                }
            }
            sb.AppendLine();
            sb.Append(GenerateSysHFasen(c, hasAddFile));
            sb.AppendLine();
            sb.Append(GenerateSysHUitgangen(c, countOnwardsIo));
            sb.AppendLine();
            sb.Append(GenerateSysHDetectors(c, hasAddFile));
            sb.AppendLine();
            sb.Append(GenerateSysHIngangen(c, countOnwardsIo));
            sb.AppendLine();
            sb.Append(GenerateSysHHulpElementen(c));
            sb.AppendLine();
            sb.Append(GenerateSysHGeheugenElementen(c));
            sb.AppendLine();
            sb.Append(GenerateSysHTijdElementen(c));
            sb.AppendLine();
            sb.Append(GenerateSysHCounters(c));
            sb.AppendLine();
            sb.Append(GenerateSysHSchakelaars(c));
            sb.AppendLine();
            sb.Append(GenerateSysHParameters(c));
            sb.AppendLine();
            if (c.HasDSI())
            {
                sb.Append(GenerateSysHDS(c));
                sb.AppendLine();
            }
            var ov = 0;
            if (c.PrioData.PrioIngreepType == PrioIngreepTypeEnum.GeneriekePrioriteit)
            {
                foreach (var ovFC in c.PrioData.PrioIngrepen)
                {
                    sb.AppendLine($"{ts}#define prioFC{ovFC.FaseCyclus}{ovFC.Naam} {ov}");
                    ++ov;
                }
                foreach (var hdFC in c.PrioData.HDIngrepen)
                {
                    sb.AppendLine($"{ts}#define hdFC{hdFC.FaseCyclus} {ov}");
                    ++ov;
                }
                sb.AppendLine($"{ts}#define prioFCMAX {ov}");
                sb.AppendLine();
            }
            
            sb.AppendLine("/* modulen */");
            sb.AppendLine("/* ------- */");
            if (!c.Data.MultiModuleReeksen)
            {
                sb.AppendLine($"{ts}#define MLMAX1 {c.ModuleMolen.Modules.Count} /* aantal modulen */");
            }
            else
            {
                foreach (var r in c.MultiModuleMolens)
                {
                    sb.AppendLine($"{ts}#define {r.Reeks}MAX1 {r.Modules.Count} /* aantal modulen reeks {r.Reeks} */");
                }
            }
	        sb.AppendLine();
	        if (c.HalfstarData.IsHalfstar)
	        {
		        sb.AppendLine("/* signaalplannen */");
		        sb.AppendLine("/* -------------- */");
		        sb.AppendLine($"{ts}#define PLMAX1 {c.HalfstarData.SignaalPlannen.Count} /* aantal signaalplannen */");
		        sb.AppendLine();
	        }
            if (c.StarData.ToepassenStar)
            {
                sb.AppendLine("/* starre programma's */");
                sb.AppendLine("/* ------------------ */");
                var pr = 0;
                foreach (var programma in c.StarData.Programmas)
                {
                    sb.AppendLine($"{ts}#define STAR{pr + 1} {pr} /* programma {programma.Naam} */");
                    ++pr;
                }
                sb.AppendLine($"{ts}#define STARMAX {c.StarData.Programmas.Count} /* aantal starre programmas */");
                sb.AppendLine();
            }
	        sb.AppendLine("/* Aantal perioden voor max groen */");
            sb.AppendLine("/* ------- */");
			// Here: +1 to allow room for default period in arrays made with this value
            sb.AppendLine($"{ts}#define MPERIODMAX {c.PeriodenData.Perioden.Count(x => x.Type == PeriodeTypeEnum.Groentijden) + 1} /* aantal groenperioden */");
            sb.AppendLine();

            foreach (var gen in orderedGenerators[CCOLCodeTypeEnum.SysHBeforeUserDefines])
            {
                sb.Append(gen.Value.GetCode(c, CCOLCodeTypeEnum.SysHBeforeUserDefines, ts));
            }

            if (hasAddFile)
            {
                sb.AppendLine("/* Gebruikers toevoegingen file includen */");
                sb.AppendLine("/* ------------------------------------- */");
                sb.AppendLine($"{ts}#include \"{c.Data.Naam}sys.add\"");
                sb.AppendLine();
            }

            return sb.ToString();
        }

        private static string GenerateSysHFasen(ControllerModel controller, bool hasAddFile)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* fasecycli */");
            sb.AppendLine("/* --------- */");

            var pad1 = "FCMAX".Length;
            foreach (var fcm in controller.Fasen)
            {
                if ((_fcpf + fcm.Naam).Length > pad1) pad1 = (_fcpf + fcm.Naam).Length;
            }
            pad1 = pad1 + $"{_ts}#define  ".Length;

            var pad2 = controller.Fasen.Count.ToString().Length;

            var index = 0;
            foreach (var fcm in controller.Fasen)
            {
                sb.Append($"{_ts}#define {(_fcpf + fcm.Naam)} ".PadRight(pad1));
                sb.AppendLine($"{index.ToString()}".PadLeft(pad2));
                ++index;
            }

            sb.Append($"{_ts}#define FCMAX{(hasAddFile ? "1" : "")} ".PadRight(pad1));
            sb.Append($"{index.ToString()} ".PadLeft(pad2));
            sb.AppendLine("/* aantal fasecycli */");

            return sb.ToString();
        }

        private static string GenerateSysHUitgangen(ControllerModel controller, bool countOnwardsIo)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* overige uitgangen */");
            sb.AppendLine("/* ----------------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._uitgangen, _ts, "FCMAX"));

            return sb.ToString();
        }

        private static string GenerateSysHDetectors(ControllerModel controller, bool hasAddFile)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* detectie */");
            sb.AppendLine("/* -------- */");

            var pf = _settingsProvider.GetPrefix("d");

            var pad1 = "ISMAX".Length;
            if(controller.Fasen.Any() && controller.Fasen.SelectMany(x => x.Detectoren).Any())
            {
                pad1 = controller.Fasen.SelectMany(x => x.Detectoren).Max(x => (pf + x.Naam).Length);
            }
            if(controller.Detectoren.Any())
            {
                var _pad1 = controller.Detectoren.Max(x => (pf + x.Naam).Length);
                pad1 = _pad1 > pad1 ? _pad1 : pad1;
            }
            if (controller.SelectieveDetectoren.Any())
            {
                var _pad1 = controller.SelectieveDetectoren.Max(x => (pf + x.Naam).Length);
                pad1 = _pad1 > pad1 ? _pad1 : pad1;
            }
            var ovdummies = controller.PrioData.GetAllDummyDetectors();
            if (ovdummies.Any())
            {
                pad1 = ovdummies.Max(x => (pf + x.Naam).Length);
            }
            pad1 = pad1 + $"{_ts}#define  ".Length;

            var pad2 = controller.Fasen.Count.ToString().Length;

            var index = 0;
            foreach (var dm in controller.GetAllDetectors())
            {
                if (dm.Dummy) continue;
                sb.Append($"{_ts}#define {pf}{dm.Naam} ".PadRight(pad1));
                sb.AppendLine($"{index}".PadLeft(pad2));
                ++index;
            }

            var autom_index = index;

            /* Dummies */
            if (controller.Fasen.Any() && controller.Fasen.SelectMany(x => x.Detectoren).Any(x => x.Dummy) ||
                controller.Detectoren.Any() && controller.Detectoren.Any(x => x.Dummy) ||
                ovdummies.Any())
            {
                sb.AppendLine("#if (!defined AUTOMAAT && !defined AUTOMAAT_TEST) || defined VISSIM || defined PRACTICE_TEST");
                foreach (var dm in controller.GetAllDetectors(x => x.Dummy))
                {
                    sb.Append($"{_ts}#define {pf}{dm.Naam} ".PadRight(pad1));
                    sb.AppendLine($"{index}".PadLeft(pad2));
                    ++index;
                }
                foreach(var dm in ovdummies)
                {
                    sb.Append($"{_ts}#define {pf}{dm.Naam} ".PadRight(pad1));
                    sb.AppendLine($"{index}".PadLeft(pad2));
                    ++index;
                }
                sb.Append($"{_ts}#define DPMAX{(hasAddFile ? "1" : "")} ".PadRight(pad1));
                sb.Append($"{index} ".PadLeft(pad2));
                sb.AppendLine("/* aantal detectoren testomgeving */");
                sb.AppendLine("#else");
                sb.Append($"{_ts}#define DPMAX{(hasAddFile ? "1" : "")} ".PadRight(pad1));
                sb.Append($"{autom_index} ".PadLeft(pad2));
                sb.AppendLine("/* aantal detectoren automaat omgeving */");
                sb.AppendLine("#endif");
            }
            else
            {
                sb.Append($"{_ts}#define DPMAX{(hasAddFile ? "1" : "")} ".PadRight(pad1));
                sb.Append($"{index} ".PadLeft(pad2));
                sb.AppendLine("/* aantal detectoren */");
            }

            return sb.ToString();
        }

        private static string GenerateSysHIngangen(ControllerModel controller, bool countOnwardsIo)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* overige ingangen */");
            sb.AppendLine("/* ---------------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._ingangen, _ts, "DPMAX"));

            return sb.ToString();
        }

        private static string GenerateSysHHulpElementen(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* hulp elementen */");
            sb.AppendLine("/* -------------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._hulpElementen, _ts));

            return sb.ToString();
        }

        private static string GenerateSysHGeheugenElementen(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* geheugen elementen */");
            sb.AppendLine("/* ------------------ */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._geheugenElementen, _ts));

            return sb.ToString();
        }

        private static string GenerateSysHTijdElementen(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* tijd elementen */");
            sb.AppendLine("/* -------------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._timers, _ts));

            return sb.ToString();
        }
        
        private static string GenerateSysHCounters(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* teller elementen */");
            sb.AppendLine("/* ---------------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._counters, _ts));

            return sb.ToString();
        }

        private static string GenerateSysHSchakelaars(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* schakelaars */");
            sb.AppendLine("/* ----------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._schakelaars, _ts));

            return sb.ToString();
        }

        private static string GenerateSysHParameters(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* parameters */");
            sb.AppendLine("/* ---------- */");

            sb.Append(CCOLCodeGenerationHelper.GetAllElementsSysHLines(_generationData._parameters, _ts));

            return sb.ToString();
        }

        private static string GenerateSysHDS(ControllerModel controller)
        {
            var sb = new StringBuilder();
            
            var pf = _settingsProvider.GetPrefix("d");

            sb.AppendLine("/* Selectieve detectie */");
            sb.AppendLine("/* ------------------- */");

            var index = 0;
            var isvecom = controller.SelectieveDetectoren.Any();
            // Geen VECOM? Dan alleen een dummy lus tbv KAR
            if (!isvecom)
            {
                sb.AppendLine($"{_ts}#define dsdummy 0 /* Dummy SD lus 0: tbv KAR */");
                ++index;
            }
            // Anders ook een dummy lus, voor KAR en zodat VECOM begint op 1
            else
            {
                sb.AppendLine($"{_ts}#define dsdummy 0 /* Dummy SD lus 0: tbv KAR & VECOM start op 1 */");
                ++index;
                
                foreach (var d in controller.SelectieveDetectoren)
                {
                    sb.AppendLine($"{_ts}#define {(pf + d.Naam).ToUpper()} {index++}{(!string.IsNullOrWhiteSpace(d.Omschrijving) ? " /* " + d.Omschrijving + "*/" : "")}");
                }
            }
            sb.AppendLine($"{_ts}#define DSMAX    {index}");

            return sb.ToString();
        }
    }
}
