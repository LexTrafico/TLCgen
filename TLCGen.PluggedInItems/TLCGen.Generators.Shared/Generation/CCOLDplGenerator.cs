using System.Collections.Generic;
using System.Linq;
using System.Text;
using TLCGen.Models;

namespace TLCGen.Generators.Shared
{
    public static class CCOLDplGenerator
    {
        private static string _ts;
        private static string _fcpf;
        private static string _uspf;
        private static string _dpf;
        private static string _ispf;
        private static CCOLGenerationData _generationData;
        private static ICCOLGeneratorSettingsProvider _settingsProvider;

        public static string GenerateDplC(
            ControllerModel c,
            CCOLGenerationData generationData,
            string ts, 
            Dictionary<CCOLCodeTypeEnum, SortedDictionary<int, ICCOLCodePieceGenerator>> orderedGenerators, 
            ICCOLGeneratorSettingsProvider settingsProvider,
            string pluginVersion,
            bool hasAddFile = true)
        {
            _ts = ts;
            _generationData = generationData;
            _settingsProvider = settingsProvider;
            _fcpf = settingsProvider.GetPrefix("fc");
            _dpf = settingsProvider.GetPrefix("d");
            _uspf = settingsProvider.GetPrefix("us");
            _ispf = settingsProvider.GetPrefix("is");

            var sb = new StringBuilder();
            sb.AppendLine("/* DISPLAY APPLICATIE */");
            sb.AppendLine("/* ------------------ */");
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateFileHeader(c.Data, "dpl.c", pluginVersion));
            sb.AppendLine();
            sb.Append(CCOLHeaderGenerator.GenerateVersionHeader(c.Data));
            sb.AppendLine();
            sb.Append(GenerateDplCExtraDefines(c));
            sb.AppendLine();
            sb.Append(GenerateDplCIncludes(c));
            sb.AppendLine();
            sb.Append(GenerateDplCDisplayBackground(c));
            sb.AppendLine();
            sb.Append(GenerateDplCDisplayParameters(c, hasAddFile));

            return sb.ToString();
        }

        private static string GenerateDplCIncludes(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* include files */");
            sb.AppendLine("/* ------------- */");
            sb.AppendLine($"{_ts}#include \"sysdef.c\"");
            sb.AppendLine($"{_ts}#include \"{controller.Data.Naam}sys.h\"");
            sb.AppendLine($"{_ts}#include \"dplwvar.c\"");

            return sb.ToString();
        }

        private static string GenerateDplCExtraDefines(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("/* aantal ingangs-/uitgangs signalen */");
            sb.AppendLine("/* --------------------------------- */");
            var usmaxplus = 0;
            var ismaxplus = 0;

            foreach (var fcm in controller.Fasen)
            {
                if (fcm.BitmapCoordinaten?.Count > 1)
                {
                    for (var i = 1; i < fcm.BitmapCoordinaten.Count; ++i)
                    {
                        sb.AppendLine($"{_ts}#define {_fcpf}{fcm.Naam}_{i} (USMAX + {usmaxplus})");
                        ++usmaxplus;
                    }
                }
            }

            foreach (var item in _generationData.AllCCOLOutputElements)
            {
                if (item.Element?.BitmapCoordinaten?.Count > 1)
                {
                    for (var i = 1; i < item.Element.BitmapCoordinaten.Count; ++i)
                    {
                        sb.AppendLine($"{_ts}#define {item.Naam}_{i} (USMAX + {usmaxplus})");
                        ++usmaxplus;
                    }
                }
            }

            foreach (var item in _generationData.AllOutputModelElements)
            {
                if (item.BitmapCoordinaten?.Count > 1)
                {
                    for (var i = 1; i < item.BitmapCoordinaten.Count; ++i)
                    {
                        sb.AppendLine($"{_ts}#define {_uspf}{item.Naam}_{i} (USMAX + {usmaxplus})");
                        ++usmaxplus;
                    }
                }
            }

            var alldets = controller.GetAllDetectors().Concat(controller.PrioData.GetAllDummyDetectors());

            foreach (var dm in alldets)
            {
                if (dm.BitmapCoordinaten?.Count > 1)
                {
                    for (var i = 1; i < dm.BitmapCoordinaten.Count; ++i)
                    {
                        sb.AppendLine($"{_ts}#define {_dpf + dm.Naam}_{i} (ISMAX + {ismaxplus})");
                        ++ismaxplus;
                    }
                }
            }

            foreach (var item in _generationData.AllCCOLInputElements)
            {
                if (item.Element?.BitmapCoordinaten?.Count > 1)
                {
                    for (var i = 1; i < item.Element.BitmapCoordinaten.Count; ++i)
                    {
                        sb.AppendLine($"{_ts}#define {item.Naam}_{i} (ISMAX + {ismaxplus})");
                        ++ismaxplus;
                    }
                }
            }

            foreach (var item in _generationData.AllInputModelElements)
            {
                if (item.BitmapCoordinaten?.Count > 1)
                {
                    for (var i = 1; i < item.BitmapCoordinaten.Count; ++i)
                    {
                        sb.AppendLine($"{_ts}#define {_ispf}{item.Naam}_{i} (ISMAX + {ismaxplus})");
                        ++ismaxplus;
                    }
                }
            }

            sb.AppendLine();

            sb.AppendLine($"{_ts}#define USDPLMAX (USMAX + {usmaxplus})");
            sb.AppendLine($"{_ts}#define ISDPLMAX (ISMAX + {ismaxplus})");

            return sb.ToString();
        }

        private static string GenerateDplCDisplayBackground(ControllerModel controller)
        {
            var sb = new StringBuilder();

            sb.AppendLine("void display_background(void)");
            sb.AppendLine("{");
            var bmnaam = controller.Data.BitmapNaam;
            if(bmnaam != null)
            {
                if(!bmnaam.ToLower().EndsWith(".bmp"))
                {
                    bmnaam = bmnaam + ".bmp";
                }
            }
            sb.AppendLine($"{_ts}load_picture_bmp(\"{bmnaam}\");");
            sb.AppendLine("}");

            return sb.ToString();
        }

        private static string GetCoordinatesString(IOElementModel item, string itemdefine, string itemtype)
        {
            var sb = new StringBuilder();
            if (item.BitmapCoordinaten?.Count > 0)
            {
                sb.Append($"{_ts}X_{itemtype}[{itemdefine}] = {item.BitmapCoordinaten[0].X}; ");
                sb.AppendLine($"Y_{itemtype}[{itemdefine}] = {item.BitmapCoordinaten[0].Y};");
            }
            else
            {
                sb.Append($"{_ts}X_{itemtype}[{itemdefine}] = NG; ");
                sb.AppendLine($"Y_{itemtype}[{itemdefine}] = NG;");
            }

            if (item.BitmapCoordinaten?.Count > 1)
            {
                for (var i = 1; i < item.BitmapCoordinaten.Count; ++i)
                {
                    sb.Append($"{_ts}X_{itemtype}[{itemdefine}_{i}] = {item.BitmapCoordinaten[i].X}; ");
                    sb.Append($"Y_{itemtype}[{itemdefine}_{i}] = {item.BitmapCoordinaten[i].Y}; ");
                    sb.AppendLine($"NR_{itemtype}[{itemdefine}_{i}] = {itemdefine};");
                }
            }
            return sb.ToString();
        }

        private static string GenerateDplCDisplayParameters(ControllerModel controller, bool hasAddFile = true)
        {
            var sb = new StringBuilder();

            sb.AppendLine("void display_parameters(void)");
            sb.AppendLine("{");

            sb.AppendLine($"{_ts}/* fasecycli */");
            sb.AppendLine($"{_ts}/* --------- */");

            foreach(var fcm in controller.Fasen)
            {
                sb.Append(GetCoordinatesString(fcm, _fcpf + fcm.Naam, "us"));
            }

            sb.AppendLine();

            sb.AppendLine($"{_ts}/* detectie */");
            sb.AppendLine($"{_ts}/* -------- */");

            var ovdummydets = controller.PrioData.GetAllDummyDetectors();
            var alldets = controller.GetAllDetectors().Concat(ovdummydets);

            foreach (var dm in alldets.Where(x => !x.Dummy))
            {
                sb.Append(GetCoordinatesString(dm, _dpf + dm.Naam, "is"));
            }

            if (alldets.Any(x => x.Dummy))
            {
                sb.AppendLine("#if (!defined AUTOMAAT_TEST)");
                foreach (var dm in alldets.Where(x => x.Dummy))
                {
                    sb.Append(GetCoordinatesString(dm, _dpf + dm.Naam, "is"));
                }
                sb.AppendLine("#endif");
            }

            sb.AppendLine();

            sb.AppendLine($"{_ts}/* overige uitgangen */");
            sb.AppendLine($"{_ts}/* ----------------- */");

            foreach (var item in _generationData.AllCCOLOutputElements.Where(x => !x.Dummy))
            {
                if(item.Element != null) sb.Append(GetCoordinatesString(item.Element, item.Naam, "us"));
            }

            foreach (var item in _generationData.AllOutputModelElements.Where(x => !x.Dummy))
            {
                sb.Append(GetCoordinatesString(item, _uspf + item.Naam, "us"));
            }

            if (_generationData.AllCCOLOutputElements.Any(x => x.Dummy) || _generationData.AllOutputModelElements.Any(x => x.Dummy))
            {
                sb.AppendLine("#if (!defined AUTOMAAT_TEST)");
            }

            if (_generationData.AllCCOLOutputElements.Any(x => x.Dummy))
            {
                foreach (var item in _generationData.AllCCOLOutputElements.Where(x => x.Dummy))
                {
                    if (item.Element != null) sb.Append(GetCoordinatesString(item.Element, item.Naam, "us"));
                }
            }

            if (_generationData.AllOutputModelElements.Any(x => x.Dummy))
            {
                foreach (var item in _generationData.AllOutputModelElements.Where(x => x.Dummy))
                {
                    sb.Append(GetCoordinatesString(item, _uspf + item.Naam, "us"));
                }
            }

            if (_generationData.AllCCOLOutputElements.Any(x => x.Dummy) || _generationData.AllOutputModelElements.Any(x => x.Dummy))
            {
                sb.AppendLine("#endif");
            }

            sb.AppendLine();

            sb.AppendLine($"{_ts}/* overige ingangen */");
            sb.AppendLine($"{_ts}/* ---------------- */");

            foreach (var item in _generationData.AllCCOLInputElements.Where(x => !x.Dummy))
            {
                if (item.Element != null) sb.Append(GetCoordinatesString(item.Element, item.Naam, "is"));
            }

            foreach (var item in _generationData.AllInputModelElements.Where(x => !x.Dummy))
            {
                sb.Append(GetCoordinatesString(item, _ispf + item.Naam, "is"));
            }
            
            if (_generationData.AllCCOLInputElements.Any(x => x.Dummy) || _generationData.AllInputModelElements.Any(x => x.Dummy))
            {
                sb.AppendLine("#if (!defined AUTOMAAT_TEST)");
            }

            if (_generationData.AllCCOLInputElements.Any(x => x.Dummy))
            {
                foreach (var item in _generationData.AllCCOLInputElements.Where(x => x.Dummy))
                {
                    if (item.Element != null) sb.Append(GetCoordinatesString(item.Element, item.Naam, "is"));
                }
            }

            if (_generationData.AllInputModelElements.Any(x => x.Dummy))
            {
                foreach (var item in _generationData.AllInputModelElements.Where(x => x.Dummy))
                {
                    sb.Append(GetCoordinatesString(item, _ispf + item.Naam, "is"));
                }
            }

            if (_generationData.AllCCOLInputElements.Any(x => x.Dummy) || _generationData.AllInputModelElements.Any(x => x.Dummy))
            { 
                sb.AppendLine("#endif");
            }

            sb.AppendLine();

            if (hasAddFile)
            {
                sb.AppendLine($"{_ts}/* Gebruikers toevoegingen file includen */");
                sb.AppendLine($"{_ts}/* ------------------------------------- */");
                sb.AppendLine($"{_ts}#include \"{controller.Data.Naam}dpl.add\"");
                sb.AppendLine();
            }

            sb.AppendLine("}");

            return sb.ToString();
        }
    }
}
