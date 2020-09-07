using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using TLCGen.Dependencies.Providers;
using TLCGen.Generators.Shared;
using TLCGen.Models;
using TLCGen.Generators.Shared.ProjectGeneration;
using TLCGen.Models.Enumerations;
using TLCGen.Plugins;

namespace TLCGen.Generators.CCOL.CodeGeneration
{
    public partial class CCOLGenerator
    {
        #region Fields

        private CCOLGenerationData _generationData;
        
        private List<string> _allFiles = new List<string>();

        private List<DetectorModel> _alleDetectoren;

        private string _uspf;
        private string _ispf;
        private string _fcpf;
        private string _dpf;
        private string _tpf;
        private string _cpf;
        private string _schpf;
        private string _prmpf;
        private string _hpf;
        private string _mpf;

        private string ts => CCOLGeneratorSettingsProvider.Default.Settings.TabSpace ?? "";

        private string _beginGeneratedHeader = "/* BEGIN GEGENEREERDE HEADER */";
        private string _endGeneratedHeader = "/* EINDE GEGENEREERDE HEADER */";

        #endregion // Fields

        #region Properties

        public static List<ICCOLCodePieceGenerator> PieceGenerators { get; private set; }
        public static Dictionary<CCOLCodeTypeEnum, SortedDictionary<int, ICCOLCodePieceGenerator>> OrderedPieceGenerators { get; private set; }

        #endregion // Properties

        #region Commands
        #endregion // Commands

        #region Command Functionality
        #endregion // Command Functionality

        #region Public Methods

        public string GenerateSourceFiles(ControllerModel c, string sourcefilepath)
        {
            _generationData = new CCOLGenerationData();

            if (Directory.Exists(sourcefilepath))
            {
                try
                {
                    CCOLGeneratorSettingsProvider.Default.Reset();
                    CCOLElementCollector.Reset();

                    _uspf = CCOLGeneratorSettingsProvider.Default.GetPrefix("us");
                    _ispf = CCOLGeneratorSettingsProvider.Default.GetPrefix("is");
                    _fcpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("fc");
                    _dpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("d");
                    _tpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("t");
                    _schpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("sch");
                    _hpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("h");
                    _mpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("m");
                    _cpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("c");
                    _prmpf = CCOLGeneratorSettingsProvider.Default.GetPrefix("prm");

                    foreach (var pgen in PieceGenerators)
                    {
                        pgen.CollectCCOLElements(c, CCOLGeneratorSettingsProvider.Default);
                    }

                    _alleDetectoren = new List<DetectorModel>();
                    foreach (var fcm in c.Fasen)
                    {
                        foreach (var dm in fcm.Detectoren)
                            _alleDetectoren.Add(dm);
                    }
                    foreach (var dm in c.Detectoren)
                        _alleDetectoren.Add(dm);
                    foreach (var dm in c.SelectieveDetectoren)
                        _alleDetectoren.Add(dm);

                    var CCOLElementLists = CCOLElementCollector.CollectAllCCOLElements(c, PieceGenerators.OrderBy(x => x.ElementGenerationOrder).ToList(), CCOLGeneratorSettingsProvider.Default);

                    CollectAllIO(_generationData);

                    if (CCOLElementLists == null || CCOLElementLists.Length != 8)
                        throw new IndexOutOfRangeException("Error collecting CCOL elements from controller.");

                    var prefixes = new Dictionary<string, string>
                    {
                        { "fc", CCOLGeneratorSettingsProvider.Default.GetPrefix("fc") },
                        { "d", CCOLGeneratorSettingsProvider.Default.GetPrefix("d") },
                        { "us", CCOLGeneratorSettingsProvider.Default.GetPrefix("us") },
                        { "is", CCOLGeneratorSettingsProvider.Default.GetPrefix("is") },
                        { "h", CCOLGeneratorSettingsProvider.Default.GetPrefix("h") },
                        { "t", CCOLGeneratorSettingsProvider.Default.GetPrefix("t") },
                        { "m", CCOLGeneratorSettingsProvider.Default.GetPrefix("m") },
                        { "c", CCOLGeneratorSettingsProvider.Default.GetPrefix("c") },
                        { "sch", CCOLGeneratorSettingsProvider.Default.GetPrefix("sch") },
                        { "prm", CCOLGeneratorSettingsProvider.Default.GetPrefix("prm") },
                    };

                    foreach (var pl in TLCGenPluginManager.Default.ApplicationPlugins)
                    {
                        if ((pl.Item1 & TLCGenPluginElems.IOElementProvider) != TLCGenPluginElems.IOElementProvider) continue;

                        var elemprov = pl.Item2 as ITLCGenElementProvider;
                        var elems = elemprov?.GetAllItems(prefixes);
                        if (elems == null) continue;
                        foreach (var elem in elems)
                        {
                            var ccolElement = elem as CCOLElement;
                            if (ccolElement == null) continue;
                            switch (ccolElement.Type)
                            {
                                case CCOLElementTypeEnum.Uitgang: CCOLElementLists[0].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.Ingang: CCOLElementLists[1].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.HulpElement: CCOLElementLists[2].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.GeheugenElement: CCOLElementLists[3].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.Timer: CCOLElementLists[4].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.Counter: CCOLElementLists[5].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.Schakelaar: CCOLElementLists[6].Elements.Add(ccolElement); break;
                                case CCOLElementTypeEnum.Parameter: CCOLElementLists[7].Elements.Add(ccolElement); break;
                            }
                        }
                    }

                    _generationData.InsertElementLists(CCOLElementLists);

                    foreach (var l in CCOLElementLists)
                    {
                        l.SetMax();
                    }

                    CCOLElementCollector.AddAllMaxElements(CCOLElementLists);

                    var pluginVersion = "0.8.0.0";

                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}reg.c"), GenerateRegC(c, CCOLGeneratorSettingsProvider.Default), Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}reg.c");
                    if (!c.Data.NietGebruikenBitmap)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}dpl.c"), CCOLDplGenerator.GenerateDplC(c, _generationData, ts, OrderedPieceGenerators, CCOLGeneratorSettingsProvider.Default, pluginVersion), Encoding.Default);
                    }
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}tab.c"), CCOLTabGenerator.GenerateTabC(c, _generationData, ts, OrderedPieceGenerators, CCOLGeneratorSettingsProvider.Default, pluginVersion), Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}tab.c");
                    var externals = PieceGenerators.Where(x => x.HasSimulationElements(c)).SelectMany(x => x.GetSimulationElements(c)).ToList();
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}sim.c"), CCOLSimGenerator.GenerateSimC(c, ts, externals, CCOLGeneratorSettingsProvider.Default, pluginVersion), Encoding.Default);
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}sys.h"), CCOLSysGenerator.GenerateSysH(c, _generationData, ts, OrderedPieceGenerators, CCOLGeneratorSettingsProvider.Default, pluginVersion), Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}sys.h");
                    if (c.RoBuGrover.ConflictGroepen?.Count > 0)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}rgv.c"), GenerateRgvC(c, CCOLGeneratorSettingsProvider.Default), Encoding.Default);
                        _allFiles.Add($"{c.Data.Naam}rgv.c");
                    }
                    if (c.PTPData.PTPKoppelingen?.Count > 0)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}ptp.c"), GeneratePtpC(c, CCOLGeneratorSettingsProvider.Default), Encoding.Default);
                        _allFiles.Add($"{c.Data.Naam}ptp.c");
                    }
                    if (c.PrioData.PrioIngreepType == Models.Enumerations.PrioIngreepTypeEnum.GeneriekePrioriteit &&
                        (c.PrioData.PrioIngrepen.Any() ||
                         c.PrioData.HDIngrepen.Any()))
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}prio.c"), GeneratePrioC(c, CCOLGeneratorSettingsProvider.Default), Encoding.Default);
                        _allFiles.Add($"{c.Data.Naam}prio.c");
                    }
                    if (c.HalfstarData.IsHalfstar)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}hst.c"), GenerateHstC(c), Encoding.Default);
                        _allFiles.Add($"{c.Data.Naam}hst.c");
                    }
                    if (c.Data.PracticeOmgeving)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, "ccolreg.txt"), GeneratePraticeCcolReg(c), Encoding.Default);
                        File.WriteAllText(Path.Combine(sourcefilepath, "ccolreg2.txt"), GeneratePraticeCcolReg2(c), Encoding.Default);
                    }
                    if (c.RISData.RISToepassen)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}rissim.c"), GenerateRisSimC(c, CCOLGeneratorSettingsProvider.Default, pluginVersion), Encoding.Default);
                    }

                    WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}reg.add"), c, GenerateRegAdd, GenerateRegAddHeader, Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}reg.add");
                    ReviseRegAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}reg.add"), c, Encoding.Default);
                    WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}tab.add"), c, GenerateTabAdd, GenerateTabAddHeader, Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}tab.add");
                    if (!c.Data.NietGebruikenBitmap)
                    {
                        WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}dpl.add"), c, GenerateDplAdd, GenerateDplAddHeader, Encoding.Default);
                    }
                    WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}sim.add"), c, GenerateSimAdd, GenerateSimAddHeader, Encoding.Default);
                    WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}sys.add"), c, GenerateSysAdd, GenerateSysAddHeader, Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}sys.add");
                    ReviseSysAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}sys.add"), c, Encoding.Default);
                    if (c.PrioData.PrioIngrepen.Count > 0 || c.PrioData.HDIngrepen.Count > 0)
                    {
                        if (c.PrioData.PrioIngreepType == Models.Enumerations.PrioIngreepTypeEnum.GeneriekePrioriteit)
                        {
                            WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}prio.add"), c, GeneratePrioAdd, GeneratePrioAddHeader, Encoding.Default);
                            _allFiles.Add($"{c.Data.Naam}prio.add");
                        }
                    }
                    if (c.HalfstarData.IsHalfstar)
                    {
                        WriteAndReviseAdd(Path.Combine(sourcefilepath, $"{c.Data.Naam}hst.add"), c, GenerateHstAdd, GenerateHstAddHeader, Encoding.Default);
                        _allFiles.Add($"{c.Data.Naam}hst.add");
                    }

                    CopySourceIfNeeded(c, "extra_func.c", sourcefilepath);
                    CopySourceIfNeeded(c, "extra_func.h", sourcefilepath);
                    CopySourceIfNeeded(c, "ccolfunc.c", sourcefilepath);
                    CopySourceIfNeeded(c, "ccolfunc.h", sourcefilepath);
                    CopySourceIfNeeded(c, "detectie.c", sourcefilepath);
                    CopySourceIfNeeded(c, "uitstuur.c", sourcefilepath);
                    CopySourceIfNeeded(c, "uitstuur.h", sourcefilepath);

                    if (c.Data.FixatieData.FixatieMogelijk)
                    {
                        CopySourceIfNeeded(c, "fixatie.c", sourcefilepath);
                        CopySourceIfNeeded(c, "fixatie.h", sourcefilepath);
                    }

                    if (c.PrioData.PrioIngrepen.Count > 0 || c.PrioData.HDIngrepen.Count > 0)
                    {
                        CopySourceIfNeeded(c, "extra_func_prio.c", sourcefilepath);
                        CopySourceIfNeeded(c, "extra_func_prio.h", sourcefilepath);
                    }

                    if (c.InterSignaalGroep.Nalopen.Any())
                    {
                        CopySourceIfNeeded(c, "gkvar.c", sourcefilepath);
                        CopySourceIfNeeded(c, "gkvar.h", sourcefilepath);
                        CopySourceIfNeeded(c, "nlvar.c", sourcefilepath);
                        CopySourceIfNeeded(c, "nlvar.h", sourcefilepath);
                        CopySourceIfNeeded(c, "nalopen.c", sourcefilepath);
                        CopySourceIfNeeded(c, "nalopen.h", sourcefilepath);
                    }

                    switch (c.Data.SynchronisatiesType)
                    {
                        case SynchronisatiesTypeEnum.SyncFunc when (c.InterSignaalGroep.Voorstarten.Any() || c.InterSignaalGroep.Gelijkstarten.Any()):
                            CopySourceIfNeeded(c, "syncfunc.c", sourcefilepath);
                            CopySourceIfNeeded(c, "syncvar.c", sourcefilepath);
                            CopySourceIfNeeded(c, "syncvar.h", sourcefilepath);
                            break;
                        case SynchronisatiesTypeEnum.RealFunc when (c.InterSignaalGroep.Voorstarten.Any() 
                                                                    || c.InterSignaalGroep.Gelijkstarten.Any()
                                                                    || c.InterSignaalGroep.Nalopen.Any(x => x.MaximaleVoorstart.HasValue)
                                                                    || c.InterSignaalGroep.LateReleases.Any()):
                            CopySourceIfNeeded(c, "realfunc.c", sourcefilepath);
                            break;
                    }

                    if (c.PrioData.PrioIngreepType == Models.Enumerations.PrioIngreepTypeEnum.GeneriekePrioriteit &&
                        (c.PrioData.PrioIngrepen.Any() || c.PrioData.HDIngrepen.Any()))
                    {
                        CopySourceIfNeeded(c, "prio.c", sourcefilepath);
                        CopySourceIfNeeded(c, "prio.h", sourcefilepath);
                    }

                    if (c.RoBuGrover.ConflictGroepen.Any())
                    {
                        CopySourceIfNeeded(c, "rgv_overslag.c", sourcefilepath);
                        CopySourceIfNeeded(c, "rgvfunc.c", sourcefilepath);
                        CopySourceIfNeeded(c, "rgvvar.c", sourcefilepath);
                        CopySourceIfNeeded(c, "winmg.c", sourcefilepath);
                        CopySourceIfNeeded(c, "winmg.h", sourcefilepath);
                    }

                    if (c.HalfstarData.IsHalfstar)
                    {
                        CopySourceIfNeeded(c, "halfstar.c", sourcefilepath);
                        CopySourceIfNeeded(c, "halfstar.h", sourcefilepath);
                        CopySourceIfNeeded(c, "halfstar_prio.c", sourcefilepath);
                        CopySourceIfNeeded(c, "halfstar_prio.h", sourcefilepath);
                    }

                    if (c.Fasen.Any(x => x.WachttijdVoorspeller))
                    {
                        CopySourceIfNeeded(c, "wtv_testwin.c", sourcefilepath);
                    }

                    if (c.RISData.RISToepassen)
                    {
                        CopySourceIfNeeded(c, "risappl.c", sourcefilepath);
                        CopySourceIfNeeded(c, "extra_func_ris.c", sourcefilepath);
                        CopySourceIfNeeded(c, "extra_func_ris.h", sourcefilepath);
                    }

                    if (c.StarData.ToepassenStar)
                    {
                        CopySourceIfNeeded(c, "starfunc.c", sourcefilepath);
                        CopySourceIfNeeded(c, "starfunc.h", sourcefilepath);
                        CopySourceIfNeeded(c, "starvar.c", sourcefilepath);
                        CopySourceIfNeeded(c, "starvar.h", sourcefilepath);
                    }

                    foreach (var pl in PieceGenerators)
                    {
                        var fs = pl.GetSourcesToCopy();
                        if (fs != null)
                        {
                            foreach (var f in fs)
                            {
                                CopySourceIfNeeded(c, f, sourcefilepath);
                            }
                        }
                    }

                    if (Directory.Exists(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "SourceFilesToCopy\\")))
                    {
                        try
                        {
                            foreach (var f in Directory.EnumerateFiles(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "SourceFilesToCopy\\")))
                            {
                                try
                                {
                                    var lines = File.ReadAllLines(f);
                                    if (lines != null && lines.Length > 0 && lines[0].StartsWith("CONDITION="))
                                    {
                                        var copy = false;
                                        var cond = lines[0].Replace("CONDITION=", "");
                                        copy = cond switch
                                        {
                                            "ALWAYS" => true,
                                            "OV" => (c.PrioData.PrioIngrepen.Count > 0 || c.PrioData.HDIngrepen.Count > 0),
                                            "SYNC" => (c.InterSignaalGroep.Gelijkstarten.Count > 0 || c.InterSignaalGroep.Voorstarten.Count > 0),
                                            "FIXATIE" => (c.Data.FixatieData.FixatieMogelijk),
                                            "NALOPEN" => (c.InterSignaalGroep.Nalopen.Count > 0),
                                            "RGV" => (c.RoBuGrover.SignaalGroepInstellingen.Count > 0),
                                            _ => copy
                                        };

                                        if (!copy || File.Exists(Path.Combine(sourcefilepath, Path.GetFileName(f)))) continue;

                                        var fileLines = new string[lines.Length - 1];
                                        Array.Copy(lines, 1, fileLines, 0, lines.Length - 1);
                                        File.WriteAllLines(Path.Combine(sourcefilepath, Path.GetFileName(f)), fileLines, Encoding.Default);
                                        _allFiles.Add(Path.GetFileName(f));
                                    }
                                }
                                catch
                                {
                                    // ignored
                                }
                            }
                        }
                        catch
                        {
                            // ignored
                        }
                    }

                    if (c.Data.GenererenIncludesLijst)
                    {
                        GenerateIncludesList(c, Path.Combine(sourcefilepath, $"{c.Data.Naam}_sources_list.txt"));
                    }
                }
                catch (Exception e)
                {
                    TLCGenDialogProvider.Default.ShowMessageBox(
                        "Er is een fout opgetreden tijdens genereren. " +
                        $"Controlleer of alle te genereren bestanden overschreven kunnen worden.\n\nOorspronkelijke foutmelding:\n{e.Message}", "Fout tijdens genereren", MessageBoxButton.OK);
                }

                return "CCOL source code gegenereerd";
            }
            return $"Map {sourcefilepath} niet gevonden. Niets gegenereerd.";
        }

        private void GenerateIncludesList(ControllerModel c, string filename)
        {
            var sb = new StringBuilder();
            sb.AppendLine($"Overzicht TLCGen bestanden regeling: {c.Data.Naam}");
            sb.AppendLine("  > excl. dpl, sim");
            sb.AppendLine("  > let op: diverse files zijn reeds opgenomen middels #include");
            foreach (var file in _allFiles)
            {
                sb.AppendLine($"- {file}");
            }
            sb.AppendLine();
            sb.AppendLine($"Overzicht benodigde CCOL libraries regeling: {c.Data.Naam}");
            foreach (var lib in CCOLVisualProjectGenerator.GetNeededCCOLLibraries(c, true, 0))
            {
                sb.AppendLine($"- {lib}");
            }
            File.WriteAllText(filename, sb.ToString());
        }

        private void CopySourceIfNeeded(ControllerModel c, string filename, string sourcefilepath)
        {
            _allFiles.Add(filename);
            if ((!File.Exists(Path.Combine(sourcefilepath, filename)) || CCOLGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources)
                && File.Exists(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "SourceFiles\\" + filename)))
            {
                if (File.Exists(Path.Combine(sourcefilepath, filename)))
                {
                    try
                    {
                        File.Delete(Path.Combine(sourcefilepath, filename));
                    }
                    catch
                    {
                        TLCGenDialogProvider.Default.ShowMessageBox($"Bestand {filename} kan niet worden overschreven. Staat het nog ergens open?", "Fout bij overschrijven bestand", MessageBoxButton.OK);
                        return;
                    }
                }
                if(c.Data.CCOLVersie < Models.Enumerations.CCOLVersieEnum.CCOL100)
                {
                    var text = File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "SourceFiles\\" + filename));
                    var rtext = Regex.Replace(text, @"(^|\s|\W)va_boolv", "$1va_bool");
                    rtext = Regex.Replace(rtext, @"(^|\W)boolv(\W)", "$1bool$2");
                    File.WriteAllText(Path.Combine(sourcefilepath, filename), rtext, Encoding.Default);
                }
                else // CCOL 10.0
                {
                    var text = File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "SourceFiles\\" + filename));
                    var rtext = Regex.Replace(text, @"(^|\s|\W)va_bool", "$1va_boolv");
                    rtext = Regex.Replace(rtext, @"(^|\W)bool(\W)", "$1boolv$2");
                    File.WriteAllText(Path.Combine(sourcefilepath, filename), rtext, Encoding.Default);
                }
            }
        }

        private void WriteAndReviseAdd(string filename, ControllerModel c, Func<ControllerModel, string> generateFunc,
            Func<ControllerModel, string> generateHeaderFunc, Encoding encoding)
        {
            if (!File.Exists(filename))
            {
                File.WriteAllText(filename, generateFunc(c), encoding);
            }
            else if (CCOLGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating)
            {
                try
                {
                    var addlines = File.ReadAllLines(filename);
                    var sb = new StringBuilder();

                    var header = false;
                    foreach (var l in addlines)
                    {
                        if (l == _endGeneratedHeader)
                        {
                            header = false;
                        }
                        else if (l == _beginGeneratedHeader)
                        {
                            header = true;
                            sb.Append(generateHeaderFunc(c));
                        }
                        else if (!header)
                        {
                            sb.AppendLine(l);
                        }
                    }
                    File.Delete(filename);
                    File.WriteAllText(filename, sb.ToString(), encoding);
                }
                catch
                {
                    // ignored
                }
            }
        }

        private void ReviseRegAdd(string filename, ControllerModel c, Encoding encoding)
        {
            if(CCOLGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating)
            {
                try
                {
                    var addlines = File.ReadAllLines(filename);

                    var wtvAdd = c.Fasen.Any(x => x.WachttijdVoorspeller) && addlines.All(x => !x.Contains("WachtijdvoorspellersWachttijd_Add"));
                    var realAdd = c.Data.SynchronisatiesType == SynchronisatiesTypeEnum.RealFunc && addlines.All(x => !x.Contains("RealisatieTijden_Add"));
                    var postSys2 = c.Data.CCOLVersie >= CCOLVersieEnum.CCOL9 && addlines.All(x => !x.Contains("post_system_application2"));

                    var sb = new StringBuilder();

                    foreach (var l in addlines)
                    {
                        if (wtvAdd && l.Contains("pre_system_application"))
                        {
                            sb.AppendLine("void WachtijdvoorspellersWachttijd_Add()");
                            sb.AppendLine("{");
                            sb.AppendLine($"{ts}");
                            sb.AppendLine("}");
                            sb.AppendLine();
                        }
                        if (realAdd && l.Contains("Maxgroen_Add"))
                        {
                            sb.AppendLine("void RealisatieTijden_Add()");
                            sb.AppendLine("{");
                            sb.AppendLine("");
                            sb.AppendLine("}");
                            sb.AppendLine();
                        }
                        if (postSys2 && l.Contains("post_dump_application"))
                        {
                            sb.AppendLine("void post_system_application2()");
                            sb.AppendLine("{");
                            sb.AppendLine("");
                            sb.AppendLine("}");
                            sb.AppendLine();
                        }
                        sb.AppendLine(l);
                    }
                    File.Delete(filename);
                    File.WriteAllText(filename, sb.ToString(), encoding);
                }
                catch
                {
                    // ignored
                }
            }
        }

        private void ReviseSysAdd(string filename, ControllerModel c, Encoding encoding)
        {
            if (CCOLGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating)
            {
                try
                {
                    var addlines = File.ReadAllLines(filename);

                    var addLnkmax = addlines.All(x => !x.Contains("LNKMAX"));
                    
                    var sb = new StringBuilder();

                    foreach (var l in addlines)
                    {
                        sb.AppendLine(l);
                    }

                    if (CCOLGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating &&
                        addLnkmax)
                    {
                        sb.AppendLine("#define LNKMAX (LNKMAX1+0) /* Totaal aantal gebruikte simulatie elementen */");
                    }

                    File.Delete(filename);
                    File.WriteAllText(filename, sb.ToString(), encoding);
                }
                catch
                {
                    // ignored
                }
            }
        }

        public void LoadSettings()
        {
            foreach(var v in PieceGenerators)
            {
                if (v.HasSettings())
                {
                    var set = CCOLGeneratorSettingsProvider.Default.Settings.CodePieceGeneratorSettings.Find(x => x.Item1 == v.GetType().Name);
                    if (set != null)
                    {
                        if (!v.SetSettings(set.Item2, CCOLGeneratorSettingsProvider.Default))
                        {
                            MessageBox.Show($"Error with {v.GetType().Name}.\nCould not load settings; code generation will be faulty.", "Error loading CCOL code generator settings.");
                            return;
                        }
                    }
                    else
                    {
                        if (!v.SetSettings(null, CCOLGeneratorSettingsProvider.Default))
                        {
                            MessageBox.Show($"Error with {v.GetType().Name}.\nCould not load settings; code generation will be faulty.", "Error loading CCOL code generator settings.");
                            return;
                        }
                        v.SetSettings(null, CCOLGeneratorSettingsProvider.Default);
                    }
                }
            }

            // Check if any plugins provide code
            foreach (var pl in TLCGenPluginManager.Default.ApplicationPlugins)
            {
                var type = pl.Item2.GetType();
                var attr = (CCOLCodePieceGeneratorAttribute)Attribute.GetCustomAttribute(type, typeof(CCOLCodePieceGeneratorAttribute));

                if (attr == null) continue;

                var genPlugin = pl.Item2 as ICCOLCodePieceGenerator;

                if (genPlugin == null) continue;

                PieceGenerators.Add(genPlugin);
                var set = CCOLGeneratorSettingsProvider.Default.Settings.CodePieceGeneratorSettings.Find(x => x.Item1 == type.Name);
                if (genPlugin.HasSettings()) genPlugin.SetSettings(set?.Item2, CCOLGeneratorSettingsProvider.Default);

                var codetypes = Enum.GetValues(typeof(CCOLCodeTypeEnum));
                foreach (var codetype in codetypes)
                {
                    var index = genPlugin.HasCode((CCOLCodeTypeEnum)codetype);
                    if (index > 0)
                    {
                        OrderedPieceGenerators[(CCOLCodeTypeEnum)codetype].Add(index, genPlugin);
                    }
                }
            }
        }

	    #endregion // Public Methods

        #region Private Methods

        private void CollectAllIO(CCOLGenerationData generationData)
        {
            generationData.AllCCOLOutputElements = new List<CCOLIOElement>();
            generationData.AllCCOLInputElements = new List<CCOLIOElement>();
            generationData.AllOutputModelElements = new List<IOElementModel>();
            generationData.AllInputModelElements = new List<IOElementModel>();

            foreach (var pgen in PieceGenerators)
            {
                if (pgen.HasCCOLBitmapOutputs())
                {
                    generationData.AllCCOLOutputElements.AddRange(pgen.GetCCOLBitmapOutputs());
                }
                if (pgen.HasCCOLBitmapInputs())
                {
                    generationData.AllCCOLInputElements.AddRange(pgen.GetCCOLBitmapInputs());
                }
            }

            foreach (var pl in Plugins.TLCGenPluginManager.Default.ApplicationParts.Concat(Plugins.TLCGenPluginManager.Default.ApplicationPlugins))
            {
                if ((pl.Item1 & Plugins.TLCGenPluginElems.IOElementProvider) == Plugins.TLCGenPluginElems.IOElementProvider)
                {
                    generationData.AllOutputModelElements.AddRange(((Plugins.ITLCGenElementProvider)pl.Item2).GetOutputItems());
                    generationData.AllInputModelElements.AddRange(((Plugins.ITLCGenElementProvider)pl.Item2).GetInputItems());
                }
            }
        }

        #endregion // Private Methods

        #region Constructor

        public CCOLGenerator()
        {
            if (PieceGenerators != null)
                throw new NullReferenceException();

            PieceGenerators = new List<ICCOLCodePieceGenerator>();
            OrderedPieceGenerators = new Dictionary<CCOLCodeTypeEnum, SortedDictionary<int, ICCOLCodePieceGenerator>>();
            var codetypes = Enum.GetValues(typeof(CCOLCodeTypeEnum));
            foreach (var type in codetypes)
            {
                OrderedPieceGenerators.Add((CCOLCodeTypeEnum)type, new SortedDictionary<int, ICCOLCodePieceGenerator>());
            }

            var ccolgen = typeof(CCOLGenerator).Assembly;
            foreach (var type in ccolgen.GetTypes())
            {
                // Find CCOLCodePieceGenerator attribute, and if found, continue
                var attr = (CCOLCodePieceGeneratorAttribute)Attribute.GetCustomAttribute(type, typeof(CCOLCodePieceGeneratorAttribute));
                if (attr != null)
                {
                    var v = Activator.CreateInstance(type) as ICCOLCodePieceGenerator;
                    PieceGenerators.Add(v);
                    foreach (var codetype in codetypes)
                    {
                        var index = v.HasCode((CCOLCodeTypeEnum) codetype);
                        if (index > 0)
                        {
                            try
                            {
                                OrderedPieceGenerators[(CCOLCodeTypeEnum) codetype].Add(index, v);
                            }
                            catch
                            {
                                MessageBox.Show($"Error while loading code generator {v.GetType().FullName}.\n" +
                                                $"Is the index number for {codetype} unique?",
                                    "Error loading code generator");
                            }
                        }
                    }
                }
            }
        }

        #endregion // Constructor
    }
}
