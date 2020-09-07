using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TLCGen.Generators.Shared;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.Traffick.CodeGeneration.Functionality
{
    [TraffickCodePieceGenerator]
    public class DetectieAanvragenCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _prmavr;
        private CCOLGeneratorCodeStringSettingModel _prmcyar;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();

            foreach (var fc in c.Fasen)
            {
                if (fc.UitgesteldeVasteAanvraag)
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmcyar}{fc.Naam}", fc.UitgesteldeVasteAanvraagTijdsduur, CCOLElementTimeTypeEnum.TE_type, _prmcyar, fc.Naam));
                }
                foreach (var d in fc.Detectoren.Where(x => x.Aanvraag != DetectorAanvraagTypeEnum.Geen))
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmavr}{d.Naam}", (int)d.Aanvraag - 1, CCOLElementTimeTypeEnum.None, _prmavr, d.Naam));
                }
            }
        }

        public override bool HasCCOLElements() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCAanvragen => 100,
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
                case CCOLCodeTypeEnum.RebCAanvragen:
                    sb.AppendLine($"{ts}/* ------------------ */");
                    sb.AppendLine($"{ts}/* Detector aanvragen */");
                    sb.AppendLine($"{ts}/* ------------------ */");
                    foreach (var fc in c.Fasen.Where(x => x.Detectoren.Any(x => x.Aanvraag != DetectorAanvraagTypeEnum.Geen)))
                    {
                        var cyar = "NG";
                        if (fc.UitgesteldeVasteAanvraag)
                        {
                            cyar = $"{_prmpf}cyar{fc.Naam}";
                        }

                        sb.Append($"{ts}proc_aanvragen({_fcpf}{fc.Naam}, {cyar}, ");
                        TraffickGenerator.GetDetectorArguments(fc, sb, x => x.Aanvraag != DetectorAanvraagTypeEnum.Geen, "avr", "              ", ts, _dpf, _prmpf);
                        sb.AppendLine(");");
                        sb.AppendLine();
                    }
                    break;
            }

            return sb.ToString();
        }
    }
}
