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
    public class VerlengenCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _tvag1;
        private CCOLGeneratorCodeStringSettingModel _prmvag;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();

            foreach (var fc in c.Fasen)
            {
                if (fc.Detectoren.Any(x => x.Verlengen != DetectorVerlengenTypeEnum.Geen))
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_tvag1}{fc.Naam}", fc.Kopmax, CCOLElementTimeTypeEnum.TE_type, _tvag1, fc.Naam));
                    foreach (var d in fc.Detectoren.Where(x => x.Verlengen != DetectorVerlengenTypeEnum.Geen))
                    {
                        var type = d.Verlengen switch
                        {
                            DetectorVerlengenTypeEnum.Uit => 0,
                            DetectorVerlengenTypeEnum.Kopmax => 1,
                            DetectorVerlengenTypeEnum.MK1 => 2,
                            DetectorVerlengenTypeEnum.MK2 => 6,
                            _ => throw new ArgumentOutOfRangeException()
                        };
                        if (d.VeiligheidsGroen == NooitAltijdAanUitEnum.Altijd || d.VeiligheidsGroen == NooitAltijdAanUitEnum.SchAan)
                        {
                            type += 8;
                        }
                        _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmvag}{d.Naam}", type, CCOLElementTimeTypeEnum.None, _prmvag, fc.Naam));
                    }
                }
            }
        }

        public override bool HasCCOLElements() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCVerlengen => 100,
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
                case CCOLCodeTypeEnum.RebCVerlengen:
                    sb.AppendLine($"{ts}/* ---------------- */");
                    sb.AppendLine($"{ts}/* Verleng functies */");
                    sb.AppendLine($"{ts}/* ---------------- */");
                    foreach (var fc in c.Fasen)
                    {
                        sb.Append($"{ts}proc_verlengen({_fcpf}{fc.Naam}, {_tpf}{_tvag1}{fc.Naam},");
                        TraffickGenerator.GetDetectorArguments(fc, sb, x => x.Verlengen != DetectorVerlengenTypeEnum.Geen, _prmvag.ToString(), "              ", ts, _dpf, _prmpf);
                        sb.AppendLine(");");
                        sb.AppendLine();
                    }
                    break;
            }

            return sb.ToString();
        }
    }
}
