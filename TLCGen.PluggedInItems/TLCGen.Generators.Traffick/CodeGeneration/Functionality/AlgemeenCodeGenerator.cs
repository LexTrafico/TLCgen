using System;
using System.Collections.Generic;
using System.Text;
using TLCGen.Models;
using TLCGen.Generators.Shared;

namespace TLCGen.Generators.CCOL.CodeGeneration.Functionality
{
    [TraffickCodePieceGenerator]
    public class AlgemeenCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _prmitfb;
        private CCOLGeneratorCodeStringSettingModel _prmagar;
        private CCOLGeneratorCodeStringSettingModel _prmdfst;
        private CCOLGeneratorCodeStringSettingModel _isfix;
        private CCOLGeneratorCodeStringSettingModel _mttp;
        private CCOLGeneratorCodeStringSettingModel _mteg;
        private CCOLGeneratorCodeStringSettingModel _msgd;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();
            _myBitmapOutputs = new List<CCOLIOElement>();
            _myBitmapInputs = new List<CCOLIOElement>();

            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmitfb}", c.Data.Fasebewaking, CCOLElementTimeTypeEnum.TS_type, _prmitfb));
            if (c.Data.FixatieMogelijk)
            {
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_isfix}", _isfix));
                _myBitmapInputs.Add(new CCOLIOElement(c.Data.FixatieData.FixatieBitmapData, _ispf + _isfix));
            }

            foreach (var fc in c.Fasen)
            {
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_mttp}{fc.Naam}", CCOLElementTypeEnum.GeheugenElement, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_mteg}{fc.Naam}", CCOLElementTypeEnum.GeheugenElement, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_msgd}{fc.Naam}", CCOLElementTypeEnum.GeheugenElement, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmagar}{fc.Naam}", 0, CCOLElementTimeTypeEnum.None, _prmagar, fc.Naam));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmdfst}{fc.Naam}", 80, CCOLElementTimeTypeEnum.None, _prmdfst, fc.Naam));
            }
        }

        public override bool HasCCOLElements() => true;

        public override bool HasCCOLBitmapOutputs() => true;

        public override bool HasCCOLBitmapInputs() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCPostApplication => 100,
                _ => 0
            };
        }

        public override bool HasFunctionLocalVariables()
        {
            return true;
        }

        public override IEnumerable<Tuple<string, string, string>> GetFunctionLocalVariables(ControllerModel c, CCOLCodeTypeEnum type)
        {
            switch (type)
            {
                //case CCOLCodeTypeEnum.RegCInitApplication:
                //    if (c.Data.GeenDetectorGedragInAutomaatOmgeving)
                //        return new List<Tuple<string, string, string>> { new Tuple<string, string, string>("int", "i", "") };
                //    return base.GetFunctionLocalVariables(c, type);
                default:
                    return base.GetFunctionLocalVariables(c, type);
            }
        }

        public override string GetCode(ControllerModel c, CCOLCodeTypeEnum type, string ts)
        {
            var sb = new StringBuilder();

            switch (type)
            {
                case CCOLCodeTypeEnum.RebCPostApplication:
                    sb.AppendLine($"{ts}/* ---------------------------- */");
                    sb.AppendLine($"{ts}/* Buffer fasecyclus informatie */");
                    sb.AppendLine($"{ts}/* ---------------------------- */");
                    foreach (var fc in c.Fasen)
                    {
                        sb.AppendLine($"{ts}proc_fc_info({_fcpf}{fc.Naam}, {_mpf}{_msgd}{fc.Naam}, {_mpf}{_mttp}{fc.Naam}, {_mpf}{_mteg}{fc.Naam}, NG);");
                    }

                    sb.AppendLine($"{ts}");

                    break;
            }

            return sb.ToString();
        }

        public override bool SetSettings(CCOLGeneratorClassWithSettingsModel settings, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            //_hplact = settingsProvider.GetElementName("hplact");

            return base.SetSettings(settings, settingsProvider);
        }
    }
}
