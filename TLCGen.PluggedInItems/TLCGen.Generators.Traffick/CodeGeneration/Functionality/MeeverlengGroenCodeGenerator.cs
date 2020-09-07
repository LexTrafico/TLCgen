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
    public class MeeverlengGroenCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _prmmv;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();

            foreach (var fc in c.Fasen.Where(x => x.Meeverlengen != NooitAltijdAanUitEnum.Nooit && x.Meeverlengen != NooitAltijdAanUitEnum.Altijd))
            {
                var sch = fc.Meeverlengen == NooitAltijdAanUitEnum.SchUit ? 0 : 1;
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmmv}{fc.Naam}", sch, CCOLElementTimeTypeEnum.TE_type, _prmmv, fc.Naam));
            }
        }

        public override bool HasCCOLElements() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCMeeverlengGroen => 100,
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
                case CCOLCodeTypeEnum.RebCMeeverlengGroen:
                    sb.AppendLine($"{ts}/* --------------- */");
                    sb.AppendLine($"{ts}/* Meeverlenggroen */");
                    sb.AppendLine($"{ts}/* --------------- */");
                    sb.AppendLine($"{ts}if (hf_mvg())");
                    sb.AppendLine($"{ts}{{");
                    foreach (var fc in c.Fasen.Where(x => x.Meeverlengen != NooitAltijdAanUitEnum.Nooit))
                    {
                        sb.AppendLine($"{ts}{ts}proc_mvg_sgd({_fcpf}{fc.Naam}, {_prmpf}{_prmmv}{fc.Naam});");
                    }
                    sb.AppendLine($"{ts}}}");
                    break;
            }

            return sb.ToString();
        }
    }
}
