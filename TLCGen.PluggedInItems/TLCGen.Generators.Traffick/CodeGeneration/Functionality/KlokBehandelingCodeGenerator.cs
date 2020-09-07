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
    public class KlokBehandelingCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _usper;
#pragma warning restore 0649

        public override void CollectCCOLElements(ControllerModel c, ICCOLGeneratorSettingsProvider settingsProvider = null)
        {
            _myElements = new List<CCOLElement>();
            _myBitmapOutputs = new List<CCOLIOElement>();

            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_usper}{c.PeriodenData.DefaultPeriodeNaam}", _usper, c.PeriodenData.DefaultPeriodeNaam));
            _myBitmapOutputs.Add(new CCOLIOElement(c.Data.FixatieData.FixatieBitmapData, _uspf + _usper + c.PeriodenData.DefaultPeriodeNaam));

            foreach (var per in c.PeriodenData.Perioden.Where(x => x.Type == PeriodeTypeEnum.Groentijden))
            { 
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_usper}{per.Naam}", _usper, per.Naam));
                _myBitmapOutputs.Add(new CCOLIOElement(c.Data.FixatieData.FixatieBitmapData, _uspf + _usper + per.Naam));

                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{per.Naam}011", per.StartTijd.Days == 1 ? 24 : per.StartTijd.Hours, CCOLElementTimeTypeEnum.TE_type, CCOLElementTypeEnum.Parameter, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{per.Naam}012", per.StartTijd.Minutes, CCOLElementTimeTypeEnum.TE_type, CCOLElementTypeEnum.Parameter, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{per.Naam}013", per.EindTijd.Days == 1 ? 24 : per.StartTijd.Hours, CCOLElementTimeTypeEnum.TE_type, CCOLElementTypeEnum.Parameter, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{per.Naam}014", per.EindTijd.Minutes, CCOLElementTimeTypeEnum.TE_type, CCOLElementTypeEnum.Parameter, ""));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{per.Naam}015", (int)per.DagCode, CCOLElementTimeTypeEnum.TE_type, CCOLElementTypeEnum.Parameter, ""));
            }
        }

        public override bool HasCCOLElements() => true;

        public override bool HasCCOLBitmapOutputs() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            return type switch
            {
                CCOLCodeTypeEnum.RebCKlokBehandeling => 100,
                CCOLCodeTypeEnum.RebCPostApplication => 200,
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
                case CCOLCodeTypeEnum.RebCKlokBehandeling:
                    sb.AppendLine($"{ts}/* --------------- */");
                    sb.AppendLine($"{ts}/* Klokbehandeling */");
                    sb.AppendLine($"{ts}/* --------------- */");
                    foreach (var p in pers)
                    {
                        sb.AppendLine($"{ts}prog_{p.Naam} = proc_schakelklok({_prmpf}{p.Naam}011, {_prmpf}{p.Naam}012, {_prmpf}{p.Naam}013, {_prmpf}{p.Naam}014, {_prmpf}{p.Naam}015);");
                    }
                    var first = true;
                    foreach (var per in pers)
                    {
                        sb.Append(!first ? $"{ts}else " : $"{ts}");
                        first = false;
                        sb.Append($"if (prog_{per.Naam}) {{ ");
                        var firstF = true;
                        for (int p2 = 0; p2 < pers.Count; p2++)
                        {
                            if (pers[p2].Naam == per.Naam) continue;
                            if (!firstF) sb.Append(" = ");
                            firstF = false;
                            sb.Append($"prog_{pers[p2].Naam}");
                        }
                        sb.AppendLine(" = FALSE; }");
                    }
                    sb.AppendLine($"{ts}else prog_{c.PeriodenData.DefaultPeriodeNaam} = TRUE;");
                    sb.AppendLine();

                    // TODO remove this!
                    sb.AppendLine($"{ts}/* TODO: should be removed!!! */");
                    for (var iPer = 0; iPer < 7; iPer++)
                    {
                        if (iPer >= pers.Count) break;
                        switch (iPer)
                        {
                            case 0:
                                sb.Append($"{ts}prog_ochtend = prog_{pers[iPer].Naam};");  
                                break;
                            case 1:
                                sb.Append($"{ts}prog_avond = prog_{pers[iPer].Naam};");  
                                break;
                            case 3:
                                sb.Append($"{ts}prog_koopavond = prog_{pers[iPer].Naam};");  
                                break;
                            case 4:
                                sb.Append($"{ts}prog_zaterdag = prog_{pers[iPer].Naam};");  
                                break;
                            case 5:
                                sb.Append($"{ts}prog_nacht = prog_{pers[iPer].Naam};");  
                                break;
                            case 6:
                                sb.Append($"{ts}prog_extra = prog_{pers[iPer].Naam};");  
                                break;
                        }
                    }

                    sb.AppendLine();
                    break;

                case CCOLCodeTypeEnum.RebCPostApplication:
                    sb.AppendLine($"{ts}/* ------------------------------- */");
                    sb.AppendLine($"{ts}/* Display KLOK geschakelde maxima */");
                    sb.AppendLine($"{ts}/* ------------------------------- */");
                    foreach (var per in pers)
                    {
                        sb.AppendLine($"{ts}CIF_GUS[{_uspf}{_usper}{per.Naam}] = BEDRYF && (AKTPRG == 0) && (_dvm ==  0) && prog_{per.Naam}; /* {per.Naam} */");
                    }
                    break;
            }

            return sb.ToString();
        }
    }
}
