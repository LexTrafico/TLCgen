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
    public class ModulenStructuurCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _usstap;
        private CCOLGeneratorCodeStringSettingModel _usblok;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();
            _myBitmapOutputs = new List<CCOLIOElement>();

            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_usstap}", CCOLElementTypeEnum.Uitgang, ""));
            _myBitmapOutputs.Add(new CCOLIOElement(new BitmapCoordinatenDataModel(), $"{_uspf}{_usstap}"));

            if (c.Data.UitgangPerModule)
            {
                for (var ml = 1; ml <= c.ModuleMolen.Modules.Count; ml++)
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_usblok}{ml}", CCOLElementTypeEnum.Uitgang, ""));
                    if (c.Data.UitgangPerModule)
                    {
                        _myBitmapOutputs.Add(new CCOLIOElement(c.Data.ModulenDisplayBitmapData[ml - 1].BitmapData, $"{_uspf}{_usblok}{ml}"));
                    }
                }
            }
        }

        public override bool HasCCOLElements() => true;

        public override bool HasCCOLBitmapOutputs() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCPostApplication => 300,
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
                case CCOLCodeTypeEnum.RebCPostApplication:
                    sb.AppendLine($"{ts}/* ------------------------ */");
                    sb.AppendLine($"{ts}/* BLOK- cq STAPverklikking */");
                    sb.AppendLine($"{ts}/* ------------------------ */");
                    sb.AppendLine($"{ts}if (!SBL) BBLOK = BLOK;");
                    sb.AppendLine($"{ts}if (BBLOK == 0) BBLOK = 1;");
                    sb.AppendLine($"{ts}");
                    if (c.Data.UitgangPerModule)
                    {
                        for (var ml = 1; ml <= c.ModuleMolen.Modules.Count; ml++)
                        {
                            sb.AppendLine($"{ts}CIF_GUS[{_uspf}{_usblok}{ml}] = BEDRYF && (AKTPRG == 0) && (BBLOK == {ml});");
                        }
                        sb.AppendLine($"{ts}");
                    }
                    sb.AppendLine($"{ts}if (AKTPRG == 0) CIF_GUS[{_uspf}{_usstap}] = BBLOK;");
                    sb.AppendLine($"{ts}else             CIF_GUS[{_uspf}{_usstap}] = STAP;");
                    break;
            }

            return sb.ToString();
        }
    }
}
