using GalaSoft.MvvmLight.Messaging;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using TLCGen.Helpers;
using TLCGen.Models;
using TLCGen.Generators.Shared;
using TLCGen.Generators.Shared.SettingsUI;
using TLCGen.Models.Enumerations;
using TLCGen.Plugins;
using TLCGen.Generators.Traffick.CodeGeneration;
using TLCGen.Generators.Traffick.Settings;
using TLCGen.Messaging.Messages;

namespace TLCGen.Generators.Traffick
{
    [TLCGenPlugin(TLCGenPluginElems.Generator | TLCGenPluginElems.HasSettings | TLCGenPluginElems.MenuControl | TLCGenPluginElems.PlugMessaging)]
    public class TraffickCodeGeneratorPlugin : ITLCGenGenerator, ITLCGenHasSettings, ITLCGenMenuItem, ITLCGenPlugMessaging
    {
        private static bool _noView;

        #region ITLCGenGenerator

        private readonly TraffickGenerator _generator;

        [Browsable(false)]
        public UserControl GeneratorView { get; }

        public string GetGeneratorName()
        {
            return "Traffick";
        }

        public string GetGeneratorVersion()
        {
            return "0.8.0.0";
        }

        public string GetPluginName()
        {
            return GetGeneratorName();
        }

        [Browsable(false)]
        public ControllerModel Controller
        {
            get;
            set;
        }

        public void GenerateController()
        {
            if(_myVm.GenerateCodeCommand.CanExecute(null))
            {
                _myVm.GenerateCodeCommand.Execute(null);
            }
        }

        public bool CanGenerateController()
        {
            return _myVm.GenerateCodeCommand.CanExecute(null);
        }

        #endregion // ITLCGenGenerator

        #region ITLCGenPlugMessaging

        public void UpdateTLCGenMessaging()
        {
            Messenger.Default.Register(this, new Action<ControllerFileNameChangedMessage>(OnControllerFileNameChanged));
            Messenger.Default.Register(this, new Action<ControllerDataChangedMessage>(OnControllerDataChanged));
        }

        #endregion // ITLCGenPlugMessaging

        #region ITLCGenHasSettings

        public void LoadSettings()
        {
            TraffickGeneratorSettingsProvider.Default.Settings =
                TLCGenSerialization.DeSerializeData<CCOLGeneratorSettingsModel>(
                    ResourceReader.GetResourceTextFile("TLCGen.Generators.Traffick.Settings.traffickgendefaults.xml", this));

            var appdatpath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            var setpath = Path.Combine(appdatpath, @"TLCGen\TraffickGeneratorSettings\");
            if (!Directory.Exists(setpath))
                Directory.CreateDirectory(setpath);
            var setfile = Path.Combine(setpath, @"traffickgensettings.xml");
	        
            // read custom settings and overwrite defaults
            if (File.Exists(setfile))
            {
                var userSettings = TLCGenSerialization.DeSerialize<CCOLGeneratorSettingsModel>(setfile);

                // always overwrite visual settings
                var userVisualSettings = userSettings.AllVisualSettings; 
                var currVisualSettings = TraffickGeneratorSettingsProvider.Default.Settings.AllVisualSettings;

                var max = Enum.GetValues(typeof(CCOLVersieEnum)).Cast<int>().Max();
                for (int i = 0; i <= max; i++)
                {
                    currVisualSettings[i].CCOLIncludesPaden = userVisualSettings[i].CCOLIncludesPaden;
                    currVisualSettings[i].CCOLLibs = userVisualSettings[i].CCOLLibs;
                    currVisualSettings[i].CCOLLibsPath = userVisualSettings[i].CCOLLibsPath;
                    currVisualSettings[i].CCOLLibsPathNoTig = userVisualSettings[i].CCOLLibsPathNoTig;
                    currVisualSettings[i].CCOLPreprocessorDefinitions = userVisualSettings[i].CCOLPreprocessorDefinitions;
                    currVisualSettings[i].CCOLResPath = userVisualSettings[i].CCOLResPath;
                }
                
                // always overwrite visual tabspace and others
                TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating = userSettings.AlterAddHeadersWhileGenerating;
				TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating = userSettings.AlterAddFunctionsWhileGenerating;
                TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources = userSettings.AlwaysOverwriteSources;
                TraffickGeneratorSettingsProvider.Default.Settings.ReplaceRepeatingCommentsTextWithPeriods = userSettings.ReplaceRepeatingCommentsTextWithPeriods;
                TraffickGeneratorSettingsProvider.Default.Settings.TabSpace = userSettings.TabSpace;

                // prefixes: overwrite where needed
                if (userSettings.Prefixes.Any())
                {
                    foreach (var pf in userSettings.Prefixes)
                    {
                        var pf2 = TraffickGeneratorSettingsProvider.Default.Settings.Prefixes.FirstOrDefault(x => pf.Default == x.Default);
                        if (pf2 != null) pf2.Setting = pf.Setting;
                    }
                }

                // element name settings: overwrite where needed
                if (userSettings.CodePieceGeneratorSettings.Any())
                {
                    foreach (var cpg in userSettings.CodePieceGeneratorSettings)
                    {
                        foreach (var cpg2 in TraffickGeneratorSettingsProvider.Default.Settings.CodePieceGeneratorSettings)
                        {
                            if (cpg.Item1 == cpg2.Item1)
                            {
                                foreach(var set in cpg.Item2.Settings)
                                {
                                    foreach (var set2 in cpg2.Item2.Settings)
                                    {
                                        if (set.Type == set2.Type && set.Default == set2.Default)
                                        {
                                            set2.Setting = set.Setting;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            TraffickGeneratorSettingsProvider.Default.Reset();
            _generator.LoadSettings();

            if (!_noView)
            {

                if (_alwaysOverwriteSourcesMenuItem == null)
                {
                    _alwaysOverwriteSourcesMenuItem = new MenuItem
                    {
                        Header = "Altijd overschrijven bronbestanden"
                    };
                }

                _alwaysOverwriteSourcesMenuItem.IsChecked =
                    TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources;

                if (_alterAddHeadersWhileGeneratingMenuItem == null)
                {
                    _alterAddHeadersWhileGeneratingMenuItem = new MenuItem
                    {
                        Header = "Bijwerken add headers tijdens genereren"
                    };
                }

                _alterAddHeadersWhileGeneratingMenuItem.IsChecked =
                    TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating;

                if (_alterAddFunctionsWhileGeneratingMenuItem == null)
                {
                    _alterAddFunctionsWhileGeneratingMenuItem = new MenuItem
                    {
                        Header = "Bijwerken functies in add files tijdens genereren"
                    };
                }

                _alterAddFunctionsWhileGeneratingMenuItem.IsChecked =
                    TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating;

                if (_replaceRepeatingCommentsTextWithPeriodsMenuItem == null)
                {
                    _replaceRepeatingCommentsTextWithPeriodsMenuItem = new MenuItem
                    {
                        Header = "Herhalend commentaar vervangen door ..."
                    };
                }

                _replaceRepeatingCommentsTextWithPeriodsMenuItem.IsChecked =
                    TraffickGeneratorSettingsProvider.Default.Settings.ReplaceRepeatingCommentsTextWithPeriods;
            }
        }

        public void SaveSettings()
        {
            var appdatpath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            var setpath = Path.Combine(appdatpath, @"TLCGen\TraffickGeneratorSettings\");
            if (!Directory.Exists(setpath))
                Directory.CreateDirectory(setpath);
            var setfile = Path.Combine(setpath, @"traffickgensettings.xml");

            var settings = new CCOLGeneratorSettingsModel();

            // always save visual settings
            var saveVisualSettings = settings.AllVisualSettings; 
            var currVisualSettings = TraffickGeneratorSettingsProvider.Default.Settings.AllVisualSettings;

            var max = Enum.GetValues(typeof(CCOLVersieEnum)).Cast<int>().Max();
            for (int i = 0; i <= max; i++)
            {
                saveVisualSettings[i].CCOLIncludesPaden = currVisualSettings[i].CCOLIncludesPaden;
                saveVisualSettings[i].CCOLLibs = currVisualSettings[i].CCOLLibs;
                saveVisualSettings[i].CCOLLibsPath = currVisualSettings[i].CCOLLibsPath;
                saveVisualSettings[i].CCOLLibsPathNoTig = currVisualSettings[i].CCOLLibsPathNoTig;
                saveVisualSettings[i].CCOLPreprocessorDefinitions = currVisualSettings[i].CCOLPreprocessorDefinitions;
                saveVisualSettings[i].CCOLResPath = currVisualSettings[i].CCOLResPath;
            }
            
            // always save visual tabspace and others
            settings.AlterAddHeadersWhileGenerating = TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating;
            settings.AlterAddFunctionsWhileGenerating = TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating;
            settings.AlwaysOverwriteSources = TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources;
            settings.ReplaceRepeatingCommentsTextWithPeriods = TraffickGeneratorSettingsProvider.Default.Settings.ReplaceRepeatingCommentsTextWithPeriods;
            settings.TabSpace = TraffickGeneratorSettingsProvider.Default.Settings.TabSpace;

            // save prefixes where needed
            foreach(var pf in TraffickGeneratorSettingsProvider.Default.Settings.Prefixes)
            {
                if(pf.Setting != pf.Default)
                {
                    settings.Prefixes.Add(new CCOLGeneratorCodeStringSettingModel
                    {
                        Type = CCOLGeneratorSettingTypeEnum.Prefix,
                        Description = pf.Description,
                        Default = pf.Default,
                        Setting = pf.Setting
                    });
                }
            }

            // save element naming where needed
            foreach (var cpg in TraffickGeneratorSettingsProvider.Default.Settings.CodePieceGeneratorSettings)
            {
                var ncpg = new CodePieceSettingsTuple<string, CCOLGeneratorClassWithSettingsModel>
                {
                    Item1 = cpg.Item1,
                    Item2 = new CCOLGeneratorClassWithSettingsModel()
                };
                ncpg.Item2.ClassName = cpg.Item2.ClassName;
                ncpg.Item2.Description = cpg.Item2.Description;
                ncpg.Item2.Settings = new List<CCOLGeneratorCodeStringSettingModel>();

                foreach(var s in cpg.Item2.Settings)
                {
                    if(s.Setting != s.Default)
                    {
                        ncpg.Item2.Settings.Add(new CCOLGeneratorCodeStringSettingModel
                        {
                            Type = s.Type,
                            Default = s.Default,
                            Description = s.Description,
                            Setting = s.Setting
                        });
                    }
                }
                if (ncpg.Item2.Settings.Any())
                {
                    settings.CodePieceGeneratorSettings.Add(ncpg);
                }
            }

            TLCGenSerialization.Serialize(setfile, settings);
        }

		#endregion // ITLCGenHasSettings

        #region ITLCGenMenuItem

        private MenuItem _alwaysOverwriteSourcesMenuItem;
        private MenuItem _alterAddHeadersWhileGeneratingMenuItem;
        private MenuItem _alterAddFunctionsWhileGeneratingMenuItem;
        private MenuItem _resetSettingsMenuItem;
        private MenuItem _replaceRepeatingCommentsTextWithPeriodsMenuItem;
        private MenuItem _pluginMenuItem;

        public MenuItem Menu
        {
            get
            {
                if (_pluginMenuItem == null)
                {
                    _pluginMenuItem = new MenuItem
                    {
                        Header = "CCOL code generator"
                    };
                    var sitem1 = new MenuItem
                    {
                        Header = "CCOL code generator instellingen",
                        Command = ShowSettingsCommand
                    };
                    if (_alwaysOverwriteSourcesMenuItem == null)
                    {
                        _alwaysOverwriteSourcesMenuItem = new MenuItem
                        {
                            Header = "Altijd overschrijven bronbestanden"
                        };
                    }
                    if (_alterAddHeadersWhileGeneratingMenuItem == null)
                    {
                        _alterAddHeadersWhileGeneratingMenuItem = new MenuItem
                        {
                            Header = "Bijwerken add headers tijdens genereren"
                        };
                    }
                    if (_alterAddFunctionsWhileGeneratingMenuItem == null)
                    {
                        _alterAddFunctionsWhileGeneratingMenuItem = new MenuItem
                        {
                            Header = "Bijwerken code in add files tijdens genereren"
                        };
                    }
                    if (_resetSettingsMenuItem == null)
	                {
		                _resetSettingsMenuItem = new MenuItem
		                {
			                Header = "Reset CCOL generator instellingen"
		                };
	                }
                    if (_replaceRepeatingCommentsTextWithPeriodsMenuItem == null)
                    {
                        _replaceRepeatingCommentsTextWithPeriodsMenuItem = new MenuItem
                        {
                            Header = "Herhalend commentaar vervangen door ..."
                        };
                    }
					_alwaysOverwriteSourcesMenuItem.Click += (o, e) =>
                    {
                        TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources =
                            !TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources;
                        _alwaysOverwriteSourcesMenuItem.IsChecked =
                            TraffickGeneratorSettingsProvider.Default.Settings.AlwaysOverwriteSources;
                    };
                    _alterAddHeadersWhileGeneratingMenuItem.Click += (o, e) =>
                    {
                        TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating =
                            !TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating;
                        _alterAddHeadersWhileGeneratingMenuItem.IsChecked =
                            TraffickGeneratorSettingsProvider.Default.Settings.AlterAddHeadersWhileGenerating;
                    };
                    _alterAddFunctionsWhileGeneratingMenuItem.Click += (o, e) =>
                    {
                        TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating =
                            !TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating;
                        _alterAddFunctionsWhileGeneratingMenuItem.IsChecked =
                            TraffickGeneratorSettingsProvider.Default.Settings.AlterAddFunctionsWhileGenerating;
                    };
                    _resetSettingsMenuItem.Click += (o, e) =>
	                {
						TraffickGeneratorSettingsProvider.Default.Settings =
                            TLCGenSerialization.DeSerializeData<CCOLGeneratorSettingsModel>(
                                ResourceReader.GetResourceTextFile("TLCGen.Generators.CCOL.Settings.ccolgendefaults.xml", this));
                    };
                    _replaceRepeatingCommentsTextWithPeriodsMenuItem.Click += (o, e) =>
                    {
                        TraffickGeneratorSettingsProvider.Default.Settings.ReplaceRepeatingCommentsTextWithPeriods =
                            !TraffickGeneratorSettingsProvider.Default.Settings.ReplaceRepeatingCommentsTextWithPeriods;
                        _replaceRepeatingCommentsTextWithPeriodsMenuItem.IsChecked =
                            TraffickGeneratorSettingsProvider.Default.Settings.ReplaceRepeatingCommentsTextWithPeriods;
                    };
					_pluginMenuItem.Items.Add(sitem1);
                    _pluginMenuItem.Items.Add(_alwaysOverwriteSourcesMenuItem);
                    _pluginMenuItem.Items.Add(_alterAddHeadersWhileGeneratingMenuItem);
                    _pluginMenuItem.Items.Add(_alterAddFunctionsWhileGeneratingMenuItem);
                    _pluginMenuItem.Items.Add(_replaceRepeatingCommentsTextWithPeriodsMenuItem);
                }
                return _pluginMenuItem;
            }
        }

        #endregion ITLCGenMenuItem

        #region Properties

        [Browsable(false)]
        public string ControllerFileName { get; set; }

        #endregion // Properties

        #region Fields

        private readonly TraffickGeneratorViewModel _myVm;

        #endregion // Fields

        #region Commands

        RelayCommand _showSettingsCommand;
        public ICommand ShowSettingsCommand
        {
            get
            {
                if (_showSettingsCommand == null)
                {
                    _showSettingsCommand = new RelayCommand(ShowSettingsCommand_Executed, ShowSettingsCommand_CanExecute);
                }
                return _showSettingsCommand;
            }
        }

        #endregion // Commands

        #region Command Functionality

        private void ShowSettingsCommand_Executed(object obj)
        {
            var order = "";
            foreach (var cpg in TraffickGenerator.OrderedPieceGenerators)
            {
                foreach(var p in cpg.Value)
                {
                    order += $"{cpg.Key} [{p.Key}] - {p.Value.GetType().Name}" + Environment.NewLine;
                }
            }

            var w = new CCOLGeneratorSettingsView
            {
                DataContext =
                    new CCOLGeneratorSettingsViewModel(TraffickGeneratorSettingsProvider.Default.Settings, order)
            };
            var window = new Window
            {
                Title = "CCOL Code Generator instellingen",
                Content = w,
                Width = 560,
                Height = 450
            };

            window.ShowDialog();
        }

        private bool ShowSettingsCommand_CanExecute(object obj)
        {
            return true;
        }

        #endregion // Command Functionality

        #region Static Public Methods

        public static string GetVersion()
        {
            return _noView ? "UnitTest" : Assembly.GetEntryAssembly()?.GetName().Version.ToString();
        }

        #endregion // Static Public Methods

        #region TLCGen Events

        private void OnControllerFileNameChanged(ControllerFileNameChangedMessage msg)
        {
            if (msg.NewFileName == null) return;

            ControllerFileName = msg.NewFileName;
        }

        private void OnControllerDataChanged(ControllerDataChangedMessage msg)
        {
        }

        #endregion // TLCGen Events

        #region Constructor

        public TraffickCodeGeneratorPlugin() : this(false)
        {
        }

        public TraffickCodeGeneratorPlugin(bool noView = false)
        {
            _noView = noView;
            _generator = new TraffickGenerator();
            _myVm = new TraffickGeneratorViewModel(this, _generator);
            if (!noView)
            {
                GeneratorView = new TraffickGeneratorView {DataContext = _myVm};
            }

            var filesDef = Directory.GetFiles(
				Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Settings\\VisualTemplatesTraffick"),
				"*.xml", SearchOption.TopDirectoryOnly);
            
	        foreach (var t in filesDef)
	        {
		        if (!t.ToLower().EndsWith("_filters.xml"))
		        {
					_myVm.VisualProjects.Add(Path.GetFileNameWithoutExtension(t).Replace("_", " "));
				}
	        }

            _myVm.SelectedVisualProject = _myVm.VisualProjects.FirstOrDefault(x => x.StartsWith("Visual 2017"));
        }

        #endregion // Constructor
    }
}
