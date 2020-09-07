using System.Collections.Generic;
using GalaSoft.MvvmLight;

namespace TLCGen.Generators.Shared.SettingsUI
{
    public class CCOLGeneratorSettingsViewModel : ViewModelBase
    {
        #region Fields

        private readonly CCOLGeneratorSettingsModel _settings;
        private string _generatorOrder;

        #endregion // Fields

        #region Properties

        public CCOLGeneratorVisualSettingsModel VisualSettings => _settings?.VisualSettings;

        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL9 => _settings?.VisualSettingsCCOL9;

        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL95 => _settings?.VisualSettingsCCOL95;

        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL100 => _settings?.VisualSettingsCCOL100;

        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL110 => _settings?.VisualSettingsCCOL110;

        public List<CodePieceSettingsTuple<string, CCOLGeneratorClassWithSettingsModel>> CodePieceGeneratorSettings => _settings?.CodePieceGeneratorSettings;

        public List<CCOLGeneratorCodeStringSettingModel> Prefixes => _settings?.Prefixes;

        public string TabSpace
        {
            get => _settings.TabSpace;
            set
            {
                _settings.TabSpace = value;
                RaisePropertyChanged();
            }
        }

        public string GeneratorOrder
        {
            get => _generatorOrder;
            set
            {
                _generatorOrder = value;
                RaisePropertyChanged();
            }
        }

        #endregion // Properties

        #region Commands

        /* 
		 * For potential future use
		 * 
        RelayCommand _saveSettingsCommand;
        public ICommand SaveSettingsCommand
        {
            get
            {
                if (_saveSettingsCommand == null)
                {
                    _saveSettingsCommand = new RelayCommand(SaveSettingsCommand_Executed, SaveSettingsCommand_CanExecute);
                }
                return _saveSettingsCommand;
            }
        }

        private void SaveSettingsCommand_Executed(object obj)
        {
            throw new NotImplementedException();
        }

        private bool SaveSettingsCommand_CanExecute(object obj)
        {
            throw new NotImplementedException();
        }

		*/

        #endregion // Commands

        #region Constructor

        public CCOLGeneratorSettingsViewModel(CCOLGeneratorSettingsModel settings, string order)
        {
            _settings = settings;
            _generatorOrder = order;
        }

        #endregion // Constructor
    }
}
