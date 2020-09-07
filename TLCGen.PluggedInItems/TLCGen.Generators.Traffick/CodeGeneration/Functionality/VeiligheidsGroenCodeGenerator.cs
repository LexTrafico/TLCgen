using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using TLCGen.Generators.Shared;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.Traffick.CodeGeneration.Functionality
{
    [TraffickCodePieceGenerator]
    public class VeiligheidsGroenCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _tvag4;
        private CCOLGeneratorCodeStringSettingModel _tvt;
#pragma warning restore 0649
        private string _prmvag;

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();

            foreach (var fc in c.Fasen.Where(fc => fc.Detectoren.Any(x => x.VeiligheidsGroen != NooitAltijdAanUitEnum.Nooit)))
            {
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_tvag4}{fc.Naam}", fc.VeiligheidsGroenMaximaal, CCOLElementTimeTypeEnum.TE_type, _tvag4, fc.Naam));
                var volgt = fc.Detectoren.Max(x => x.VeiligheidsGroenVolgtijd);
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_tvt}{fc.Naam}", volgt, CCOLElementTimeTypeEnum.TE_type, _tvt, fc.Naam));
            }

            // other elements (_prmvag for detectors) are added in 'VerlengenCodeGenerator'
        }

        public override bool HasCCOLElements() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCVeiligheidsGroen => 100,
                _ => 0
            };
        }

        public override bool HasFunctionLocalVariables()
        {
            return true;
        }

        public override string GetCode(ControllerModel c, CCOLCodeTypeEnum type, string ts)
        {
            var sb = new StringBuilder();

            switch (type)
            {
                case CCOLCodeTypeEnum.RebCVeiligheidsGroen:
                    sb.AppendLine($"{ts}/* ----------------------- */");
                    sb.AppendLine($"{ts}/* Veiligheidsgroen (VAG4) */");
                    sb.AppendLine($"{ts}/* ----------------------- */");
                    foreach (var fc in c.Fasen.Where(fc => fc.Detectoren.Any(x => x.VeiligheidsGroen != NooitAltijdAanUitEnum.Nooit)))
                    {
                        sb.Append($"{ts}proc_vag4({_fcpf}{fc.Naam}, {_tpf}{_tvag4}{fc.Naam}, {_tpf}{_tvt}{fc.Naam},");
                        TraffickGenerator.GetDetectorArguments(fc, sb, x => x.VeiligheidsGroen != NooitAltijdAanUitEnum.Nooit, _prmvag, "         ", ts, _dpf, _prmpf);
                        sb.AppendLine();
                    }
                    break;
            }

            return sb.ToString();
        }

        public override bool SetSettings(CCOLGeneratorClassWithSettingsModel settings, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            _prmvag = settingsProvider.GetElementName("prmvag");

            return base.SetSettings(settings, settingsProvider);
        }
    }
}
