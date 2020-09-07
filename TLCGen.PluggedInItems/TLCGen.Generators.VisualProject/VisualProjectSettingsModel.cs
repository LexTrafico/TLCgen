using System;
using System.Xml.Serialization;

namespace TLCGen.Generators.VisualProject
{
    [Serializable]
    [XmlRoot(ElementName = "VisualProjectSettings")]
    public class VisualProjectSettingsModel
    {
        #region Properties
        
        public CCOLGeneratorVisualSettingsModel VisualSettings { get; set; }
        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL9 { get; set; }
        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL95 { get; set; }
        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL100 { get; set; }
        public CCOLGeneratorVisualSettingsModel VisualSettingsCCOL110 { get; set; }

        [XmlIgnore]
        public CCOLGeneratorVisualSettingsModel[] AllVisualSettings 
        {
            get
            {
                return new[]
                {
                    VisualSettings,
                    VisualSettingsCCOL9,
                    VisualSettingsCCOL95,
                    VisualSettingsCCOL100,
                    VisualSettingsCCOL110
                };
            }
        }

        #endregion // Properties

        #region Constructor

        public VisualProjectSettingsModel()
        {
            VisualSettings = new CCOLGeneratorVisualSettingsModel();
            VisualSettingsCCOL9 = new CCOLGeneratorVisualSettingsModel();
            VisualSettingsCCOL95 = new CCOLGeneratorVisualSettingsModel();
            VisualSettingsCCOL100 = new CCOLGeneratorVisualSettingsModel();
            VisualSettingsCCOL110 = new CCOLGeneratorVisualSettingsModel();
        }

        #endregion // Constructor
    }
}
