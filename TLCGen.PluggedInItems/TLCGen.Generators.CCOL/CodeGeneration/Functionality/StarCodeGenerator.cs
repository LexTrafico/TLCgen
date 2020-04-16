﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TLCGen.Generators.CCOL.Settings;
using TLCGen.Models;

namespace TLCGen.Generators.CCOL.CodeGeneration.Functionality
{
    [CCOLCodePieceGenerator]
    public class StarCodeGenerator : CCOLCodePieceGeneratorBase
    {
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _prmstarstart;
        private CCOLGeneratorCodeStringSettingModel _prmstareind;
        private CCOLGeneratorCodeStringSettingModel _prmstarprog;
        private CCOLGeneratorCodeStringSettingModel _prmstarprogdef;
        private CCOLGeneratorCodeStringSettingModel _schstar;
        private CCOLGeneratorCodeStringSettingModel _mstarprog;
        private CCOLGeneratorCodeStringSettingModel _prmstarcyclustijd;
#pragma warning restore 0649
        private string _mperiodstar;

        public override void CollectCCOLElements(ControllerModel c)
        {
            _myElements = new List<CCOLElement>();

            if (!c.StarData.ToepassenStar || !c.StarData.Programmas.Any()) return;

            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_schstar}", 0, CCOLElementTimeTypeEnum.SCH_type, _schstar));
            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_mstarprog}", _mstarprog));

            if (c.StarData.ProgrammaSturingViaParameter)
            {
                var dp = c.StarData.Programmas.FirstOrDefault(x => x.Naam == c.StarData.DefaultProgramma);
                var iDp = dp == null ? 0 : c.StarData.Programmas.IndexOf(dp);
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstarprogdef}", iDp, CCOLElementTimeTypeEnum.SCH_type, _prmstarprogdef));
            }

            if (c.StarData.ProgrammaTijdenInParameters)
            {
                foreach (var pr in c.StarData.Programmas)
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstarcyclustijd}", pr.Cyclustijd, CCOLElementTimeTypeEnum.TS_type, _prmstarcyclustijd, pr.Naam));
                    foreach (var f in pr.Fasen)
                    {
                        _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstarstart}", f.Start1, CCOLElementTimeTypeEnum.None, _prmstarstart, "1", f.FaseCyclus, pr.Naam));
                        _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstareind}", f.Eind1, CCOLElementTimeTypeEnum.None, _prmstareind, "1", f.FaseCyclus, pr.Naam));
                        if (f.Start2.HasValue && f.Eind2.HasValue)
                        {
                            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstarstart}", f.Start2.Value, CCOLElementTimeTypeEnum.None, _prmstarstart, "2", f.FaseCyclus, pr.Naam));
                            _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstareind}", f.Eind2.Value, CCOLElementTimeTypeEnum.None, _prmstareind, "2", f.FaseCyclus, pr.Naam));
                        }
                    }
                }
            }

            if (c.StarData.ProgrammaSturingViaKlok)
            {
                foreach (var p in c.StarData.PeriodenData)
                {
                    var dp = c.StarData.Programmas.FirstOrDefault(x => x.Naam == p.StarProgramma);
                    var iDp = dp == null ? 0 : c.StarData.Programmas.IndexOf(dp);
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_prmstarprog}", iDp, CCOLElementTimeTypeEnum.None, _prmstarprog, p.Periode));
                }
            }
        }

        public override bool HasCCOLElements() => true;

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            switch (type)
            {
                case CCOLCodeTypeEnum.RegCIncludes:
                case CCOLCodeTypeEnum.RegCTop:
                    return 100;
                case CCOLCodeTypeEnum.RegCPreApplication:
                    return 100;
                case CCOLCodeTypeEnum.RegCKlokPerioden:
                    return 20;
                case CCOLCodeTypeEnum.RegCVerlenggroen:
                case CCOLCodeTypeEnum.RegCMaxgroen:
                    return 60;
                case CCOLCodeTypeEnum.RegCPostApplication:
                    return 50;
            }
            return 0;
        }

        public override string GetCode(ControllerModel c, CCOLCodeTypeEnum type, string ts)
        {
            if (!c.StarData.ToepassenStar) return null;

            var sb = new StringBuilder();

            switch (type)
            {
                case CCOLCodeTypeEnum.RegCIncludes:
                    sb.AppendLine($"{ts}#include \"starfunc.c\" /* Functies t.b.v. star regelen */");
                    break;
                case CCOLCodeTypeEnum.RegCTop:
                    sb.AppendLine($"{ts}extern count star_cyclustimer;");
                    break;
                case CCOLCodeTypeEnum.RegCPreApplication:
                    if (c.Data.MultiModuleReeksen)
                    {
                        sb.AppendLine($"{ts}if (SMLA && MLA == ML1)");
                    }
                    else
                    {
                        sb.AppendLine($"{ts}if (SML && ML == ML1)");
                    }
                    sb.AppendLine($"{ts}{{");
                    sb.AppendLine($"{ts}{ts}star_cyclustimer = 0;");
                    sb.AppendLine($"{ts}}}");
                    break;

                case CCOLCodeTypeEnum.RegCKlokPerioden:

                    
                    sb.AppendLine($"{ts}/* Bepalen actief star programma */");
                    sb.AppendLine($"{ts}if (SCH[{_schpf}{_schstar}])");
                    sb.AppendLine($"{ts}{{");
                    if (c.StarData.ProgrammaSturingViaKlok)
                    {
                        sb.AppendLine($"{ts}{ts}/* Actief star programma o.b.v. kloksturing */");
                        sb.AppendLine($"{ts}{ts}if (MM[{_mpf}{_mperiodstar}] != 0) MM[{_mpf}{_mstarprog}] = MM[{_mpf}{_mperiodstar}];");
                    }
                    if (c.StarData.ProgrammaSturingViaParameter)
                    {
                        if (c.StarData.ProgrammaSturingViaKlok) sb.AppendLine();
                        sb.AppendLine($"{ts}{ts}/* Actief star programma o.b.v. parameter */");
                        sb.AppendLine($"{ts}{ts}if (PRM[{_prmpf}{_prmstarprogdef}] != 0) MM[{_mpf}{_mstarprog}] = PRM[{_prmpf}{_prmstarprogdef}];");
                    }
                    sb.AppendLine($"{ts}}}");

                    break;

                case CCOLCodeTypeEnum.RegCPostApplication:
                    sb.AppendLine($"{ts}/* stuur alles rood tbv programmawisseling */");
                    sb.AppendLine($"{ts}if (0) /* (MM[mprg_wissel]) TODO hoe wordt deze opgezet? */");
                    sb.AppendLine($"{ts}{{");
                    sb.AppendLine($"{ts}{ts}int fc;");
                    sb.AppendLine();
                    sb.AppendLine($"{ts}{ts}/*IH[hblok_volgrichting] = FALSE;*/");
                    sb.AppendLine();
                    sb.AppendLine($"{ts}{ts}/***************************************/");
                    sb.AppendLine($"{ts}{ts}/* stuur alle signaalgroepen naar rood */");
                    sb.AppendLine($"{ts}{ts}/***************************************/");
                    sb.AppendLine($"{ts}{ts}for (fc = 0; fc < FCMAX; ++fc)");
                    sb.AppendLine($"{ts}{ts}{{");
                    sb.AppendLine($"{ts}{ts}{ts}RR[fc] = (RW[fc] & BIT2 || YV[fc] & BIT2) ? FALSE : TRUE;");
                    sb.AppendLine($"{ts}{ts}{ts}Z[fc] = (RW[fc] & BIT2 || YV[fc] & BIT2) ? FALSE : TRUE;");
                    sb.AppendLine();
                    sb.AppendLine($"{ts}{ts}{ts}/*IH[hblok_volgrichting] |= (RW[fc] & BIT2 || YV[fc] & BIT2);*/");
                    sb.AppendLine($"{ts}{ts}}}");
                    sb.AppendLine($"{ts}}}");
                    sb.AppendLine();

                    break;
            }

            return sb.ToString();
        }

        public override bool SetSettings(CCOLGeneratorClassWithSettingsModel settings)
        {
            _mperiodstar = CCOLGeneratorSettingsProvider.Default.GetElementName("mperiodstar");

            return base.SetSettings(settings);
        }
    }
}