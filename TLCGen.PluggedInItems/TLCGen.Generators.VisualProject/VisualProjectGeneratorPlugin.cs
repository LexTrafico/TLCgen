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
using TLCGen.Models.Enumerations;
using TLCGen.Plugins;
using System.Xml;

namespace TLCGen.Generators.VisualProject
{
    [TLCGenPlugin(TLCGenPluginElems.Generator | TLCGenPluginElems.HasSettings | TLCGenPluginElems.PlugMessaging)]
    public class CCOLCodeGeneratorPlugin : ITLCGenGenerator, ITLCGenHasSettings, ITLCGenPlugMessaging
    {
        private static bool _noView;

        #region ITLCGenGenerator

        private readonly CCOLVisualProjectGenerator _generator;

        [Browsable(false)]
        public UserControl GeneratorView { get; }

        public string GetGeneratorName()
        {
            return "Visual project";
        }

        public string GetGeneratorVersion()
        {
            return "1.0.0.0";
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
            Messenger.Default.Register(this, new Action<Messaging.Messages.ControllerFileNameChangedMessage>(OnControllerFileNameChanged));
            Messenger.Default.Register(this, new Action<Messaging.Messages.ControllerDataChangedMessage>(OnControllerDataChanged));
        }

        #endregion // ITLCGenPlugMessaging

        #region ITLCGenHasSettings

        public void LoadSettings()
        {
            VisualProjectSettingsProvider.Default.Settings =
                TLCGenSerialization.DeSerializeData<VisualProjectSettingsModel>(
                    ResourceReader.GetResourceTextFile("TLCGen.Generators.VisualProject.visualgendefaults.xml", this));

            var appdatpath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            var setpath = Path.Combine(appdatpath, @"TLCGen\");
            if (!Directory.Exists(setpath))
                Directory.CreateDirectory(setpath);
            var setfile = Path.Combine(setpath, @"visualgensettings.xml");
	        
            VisualProjectSettingsModel userSettings = null;

            // read custom settings and overwrite defaults
            if (File.Exists(setfile))
            {
                userSettings = TLCGenSerialization.DeSerialize<VisualProjectSettingsModel>(setfile);
            }
            else
            {
                var ccsetpath = Path.Combine(appdatpath, @"TLCGen\CCOLGeneratorSettings\");
                var ccsetfile = Path.Combine(ccsetpath, @"ccolgensettings.xml");
                if (File.Exists(ccsetfile))
                {
                    userSettings = new VisualProjectSettingsModel();
                    var doc = new XmlDocument();
                    doc.Load(ccsetfile);
                    foreach (XmlNode node in doc.FirstChild.ChildNodes)
                    {
                        switch (node.LocalName)
                        {
                            case "VisualSettings":
                                var model = XmlNodeConverter.ConvertNode<CCOLGeneratorVisualSettingsModel>(node);
                                userSettings.VisualSettings = model;
                                break;
                            case "VisualSettingsCCOL9":
                                var model9 = XmlNodeConverter.ConvertNode<CCOLGeneratorVisualSettingsModel>(node);
                                userSettings.VisualSettingsCCOL9 = model9;
                                break;
                            case "VisualSettingsCCOL95":
                                var model95 = XmlNodeConverter.ConvertNode<CCOLGeneratorVisualSettingsModel>(node);
                                userSettings.VisualSettingsCCOL95 = model95;
                                break;
                            case "VisualSettingsCCOL100":
                                var model10 = XmlNodeConverter.ConvertNode<CCOLGeneratorVisualSettingsModel>(node);
                                userSettings.VisualSettingsCCOL100 = model10;
                                break;
                            case "VisualSettingsCCOL110":
                                var model11 = XmlNodeConverter.ConvertNode<CCOLGeneratorVisualSettingsModel>(node);
                                userSettings.VisualSettingsCCOL110 = model11;
                                break;
                        }
                    }
                }
            }

            if (userSettings != null)
            {
                // always overwrite visual settings
                var userVisualSettings = userSettings.AllVisualSettings; 
                var currVisualSettings = VisualProjectSettingsProvider.Default.Settings.AllVisualSettings;

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
            }
        }

        public void SaveSettings()
        {
            var appdatpath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            var setpath = Path.Combine(appdatpath, @"TLCGen\");
            if (!Directory.Exists(setpath))
                Directory.CreateDirectory(setpath);
            var setfile = Path.Combine(setpath, @"visualgensettings.xml");

            var settings = new VisualProjectSettingsModel();

            // always save visual settings
            var saveVisualSettings = settings.AllVisualSettings; 
            var currVisualSettings = VisualProjectSettingsProvider.Default.Settings.AllVisualSettings;

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

            TLCGenSerialization.Serialize(setfile, settings);
        }

		#endregion // ITLCGenHasSettings

        #region Properties

        [Browsable(false)]
        public string ControllerFileName { get; set; }

        #endregion // Properties

        #region Fields

        private readonly CCOLGeneratorViewModel _myVm;

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
            var w = new CCOLGeneratorSettingsView
            {
                DataContext =
                    new CCOLGeneratorSettingsViewModel(CCOLGeneratorSettingsProvider.Default.Settings, _generator)
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

        private void OnControllerFileNameChanged(Messaging.Messages.ControllerFileNameChangedMessage msg)
        {
            if (msg.NewFileName == null) return;

            ControllerFileName = msg.NewFileName;
        }

        private void OnControllerDataChanged(Messaging.Messages.ControllerDataChangedMessage msg)
        {
        }

        #endregion // TLCGen Events

        #region Constructor

        public CCOLCodeGeneratorPlugin() : this(false)
        {
        }

        public CCOLCodeGeneratorPlugin(bool noView = false)
        {
            _noView = noView;
            _generator = new CCOLGenerator();
            _myVm = new CCOLGeneratorViewModel(this, _generator);
            if (!noView)
            {
                GeneratorView = new CCOLGeneratorView {DataContext = _myVm};
            }

	        var filesDef = Directory.GetFiles(
				Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Settings\\VisualTemplates"),
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
