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
    public class GroentijdenCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _prmmg;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();

            foreach (var set in c.GroentijdenSets)
            {
                foreach (var gt in set.Groentijden)
                {
                    if (gt.Waarde.HasValue)
                    {
                        _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmmg}{set.Naam}{gt.FaseCyclus}", gt.Waarde.Value, CCOLElementTimeTypeEnum.TE_type, _prmmg, gt.FaseCyclus, set.Naam));
                    }
                }
            }
        }

        public override bool HasCCOLElements() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCGroentijden => 100,
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
            var pers = c.PeriodenData.Perioden.Where(x => x.Type == PeriodeTypeEnum.Groentijden).ToList();

            switch (type)
            {
                case CCOLCodeTypeEnum.RebCGroentijden:
                    sb.AppendLine($"{ts}/* ------------------------------------------------ */");
                    sb.AppendLine($"{ts}/* Klokgeschakelde maximumgroentijden SGD programma */");
                    sb.AppendLine($"{ts}/* ------------------------------------------------ */");
                    foreach (var fc in c.Fasen)
                    {
                        sb.Append($"{ts}maxgroen({_fcpf}{fc.Naam}, ");
                        var i = 0;
                        foreach (var set in c.GroentijdenSets)
                        {
                            var fcGt = set.Groentijden.FirstOrDefault(x => x.FaseCyclus == fc.Naam);
                            if (fcGt != null)
                            {
                                sb.Append($"{_prmpf}{_prmmg}{set.Naam}{fc.Naam}, ");
                                ++i;
                                if (i == 7) break;
                            }
                        }

                        for (; i < 7; ++i) sb.Append("NG, ");
                        sb.AppendLine();
                        sb.AppendLine($"{ts}{ts}{ts}{ts}{ts} NG       , NG       , NG       , NG       , NG       , NG       , NG       , NG       ,      ");
                        sb.AppendLine($"{ts}{ts}{ts}{ts}{ts} NG       , NG       , NG       );");
                        sb.AppendLine();
                    }
                    break;
            }

            return sb.ToString();
        }
    }
}
