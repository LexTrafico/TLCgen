using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using TLCGen.Dependencies.Providers;
using TLCGen.Generators.Shared;
using TLCGen.Models;
using TLCGen.Generators.Shared;
using TLCGen.Models.Enumerations;
using TLCGen.Plugins;
using TLCGen.Generators.Traffick.Settings;

namespace TLCGen.Generators.Traffick.CodeGeneration
{
    public partial class TraffickGenerator
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

        private string ts => TraffickGeneratorSettingsProvider.Default.Settings.TabSpace ?? "";

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
                    TraffickGeneratorSettingsProvider.Default.Reset();
                    CCOLElementCollector.Reset();

                    _uspf = TraffickGeneratorSettingsProvider.Default.GetPrefix("us");
                    _ispf = TraffickGeneratorSettingsProvider.Default.GetPrefix("is");
                    _fcpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("fc");
                    _dpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("d");
                    _tpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("t");
                    _schpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("sch");
                    _hpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("h");
                    _mpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("m");
                    _cpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("c");
                    _prmpf = TraffickGeneratorSettingsProvider.Default.GetPrefix("prm");

                    foreach (var pgen in PieceGenerators)
                    {
                        pgen.CollectCCOLElements(c, TraffickGeneratorSettingsProvider.Default);
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

                    var CCOLElementLists = CCOLElementCollector.CollectAllCCOLElements(c, PieceGenerators.OrderBy(x => x.ElementGenerationOrder).ToList(), TraffickGeneratorSettingsProvider.Default);

                    CollectAllIO(_generationData);

                    if (CCOLElementLists == null || CCOLElementLists.Length != 8)
                        throw new IndexOutOfRangeException("Error collecting CCOL elements from controller.");

                    var prefixes = new Dictionary<string, string>
                    {
                        { "fc", TraffickGeneratorSettingsProvider.Default.GetPrefix("fc") },
                        { "d", TraffickGeneratorSettingsProvider.Default.GetPrefix("d") },
                        { "us", TraffickGeneratorSettingsProvider.Default.GetPrefix("us") },
                        { "is", TraffickGeneratorSettingsProvider.Default.GetPrefix("is") },
                        { "h", TraffickGeneratorSettingsProvider.Default.GetPrefix("h") },
                        { "t", TraffickGeneratorSettingsProvider.Default.GetPrefix("t") },
                        { "m", TraffickGeneratorSettingsProvider.Default.GetPrefix("m") },
                        { "c", TraffickGeneratorSettingsProvider.Default.GetPrefix("c") },
                        { "sch", TraffickGeneratorSettingsProvider.Default.GetPrefix("sch") },
                        { "prm", TraffickGeneratorSettingsProvider.Default.GetPrefix("prm") },
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

                    CCOLElementCollector.AddAllMaxElements(CCOLElementLists, false);

                    var pluginVersion = "0.8.0.0";

                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}reg.c"), GenerateRegC(c, TraffickGeneratorSettingsProvider.Default), Encoding.Default);
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}reb.c"), GenerateRebC(c, TraffickGeneratorSettingsProvider.Default), Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}reg.c");
                    if (!c.Data.NietGebruikenBitmap)
                    {
                        File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}dpl.c"), CCOLDplGenerator.GenerateDplC(c, _generationData, ts, OrderedPieceGenerators, TraffickGeneratorSettingsProvider.Default, pluginVersion, false), Encoding.Default);
                    }
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}tab.c"), CCOLTabGenerator.GenerateTabC(c, _generationData, ts, OrderedPieceGenerators, TraffickGeneratorSettingsProvider.Default, pluginVersion, false, false), Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}tab.c");
                    var externals = PieceGenerators.Where(x => x.HasSimulationElements(c)).SelectMany(x => x.GetSimulationElements(c)).ToList();
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}sim.c"), CCOLSimGenerator.GenerateSimC(c, ts, externals, TraffickGeneratorSettingsProvider.Default, pluginVersion, false), Encoding.Default);
                    File.WriteAllText(Path.Combine(sourcefilepath, $"{c.Data.Naam}sys.h"), CCOLSysGenerator.GenerateSysH(c, _generationData, ts, OrderedPieceGenerators, TraffickGeneratorSettingsProvider.Default, pluginVersion, true, false), Encoding.Default);
                    _allFiles.Add($"{c.Data.Naam}sys.h");
                    
                    CopySourceIfNeeded(c, "appl_krw.c", sourcefilepath);
                    CopySourceIfNeeded(c, "appl_pro.c", sourcefilepath);
                    CopySourceIfNeeded(c, "bt_t2r_t2g.c", sourcefilepath);
                    CopySourceIfNeeded(c, "bt_t2r_t2g.h", sourcefilepath);
                    CopySourceIfNeeded(c, "crsv2ccol.c", sourcefilepath);
                    CopySourceIfNeeded(c, "crsv2ccol.h", sourcefilepath);
                    CopySourceIfNeeded(c, "dumpfile.c", sourcefilepath);
                    CopySourceIfNeeded(c, "hvrt_fctiming.c", sourcefilepath);
                    CopySourceIfNeeded(c, "pro_fik.h", sourcefilepath);
                    CopySourceIfNeeded(c, "rif.inc", sourcefilepath);
                    CopySourceIfNeeded(c, "rif_defs.h", sourcefilepath);
                    CopySourceIfNeeded(c, "rif_func.c", sourcefilepath);
                    CopySourceIfNeeded(c, "rif_func.h", sourcefilepath);
                    CopySourceIfNeeded(c, "rif_func_traffick.c", sourcefilepath);
                    CopySourceIfNeeded(c, "rif_func_traffick.c", sourcefilepath);
                    CopySourceIfNeeded(c, "ris_sim.c", sourcefilepath);
                    CopySourceIfNeeded(c, "t2r_t2g.c", sourcefilepath);
                    CopySourceIfNeeded(c, "t2r_t2g.h", sourcefilepath);
                    CopySourceIfNeeded(c, "t2r_t2g_traffick.c", sourcefilepath);
                    CopySourceIfNeeded(c, "t2r_t2g_traffick.h", sourcefilepath);
                    CopySourceIfNeeded(c, "vlog3_extra_traffick.c", sourcefilepath);

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
            throw new NotImplementedException("TODO");
            //foreach (var lib in CCOLVisualProjectGenerator.GetNeededCCOLLibraries(c, true, 0))
            //{
            //    sb.AppendLine($"- {lib}");
            //}
            File.WriteAllText(filename, sb.ToString());
        }

        private void CopySourceIfNeeded(ControllerModel c, string filename, string sourcefilepath)
        {
            _allFiles.Add(filename);
            if ((!File.Exists(Path.Combine(sourcefilepath, filename)) || TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources)
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
            else if (TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating)
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
            if(TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating)
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
            if (TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating)
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

                    if (TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating &&
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
                    var set = TraffickGeneratorSettingsProvider.Default.Settings.CodePieceGeneratorSettings.Find(x => x.Item1 == v.GetType().Name);
                    if (set != null)
                    {
                        if (!v.SetSettings(set.Item2, TraffickGeneratorSettingsProvider.Default))
                        {
                            MessageBox.Show($"Error with {v.GetType().Name}.\nCould not load settings; code generation will be faulty.", "Error loading CCOL code generator settings.");
                            return;
                        }
                    }
                    else
                    {
                        if (!v.SetSettings(null, TraffickGeneratorSettingsProvider.Default))
                        {
                            MessageBox.Show($"Error with {v.GetType().Name}.\nCould not load settings; code generation will be faulty.", "Error loading CCOL code generator settings.");
                            return;
                        }
                        v.SetSettings(null, TraffickGeneratorSettingsProvider.Default);
                    }
                }
            }

            // Check if any plugins provide code
            foreach (var pl in TLCGenPluginManager.Default.ApplicationPlugins)
            {
                var type = pl.Item2.GetType();
                var attr = (TraffickCodePieceGeneratorAttribute)Attribute.GetCustomAttribute(type, typeof(TraffickCodePieceGeneratorAttribute));

                if (attr == null) continue;

                var genPlugin = pl.Item2 as ICCOLCodePieceGenerator;

                if (genPlugin == null) continue;

                PieceGenerators.Add(genPlugin);
                var set = TraffickGeneratorSettingsProvider.Default.Settings.CodePieceGeneratorSettings.Find(x => x.Item1 == type.Name);
                if (genPlugin.HasSettings()) genPlugin.SetSettings(set?.Item2, TraffickGeneratorSettingsProvider.Default);

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

        public TraffickGenerator()
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

            var ccolgen = typeof(TraffickGenerator).Assembly;
            foreach (var type in ccolgen.GetTypes())
            {
                // Find TraffickCodePieceGeneratorAttribute attribute, and if found, continue
                var attr = (TraffickCodePieceGeneratorAttribute)Attribute.GetCustomAttribute(type, typeof(TraffickCodePieceGeneratorAttribute));
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
