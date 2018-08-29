﻿using System.Collections.Generic;
using System.Linq;
using System.Text;
using TLCGen.Generators.CCOL.Settings;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.CCOL.CodeGeneration.Functionality
{

    [CCOLCodePieceGenerator]
    public class RateltikkersCodeGenerator : CCOLCodePieceGeneratorBase
    {
        #region Fields
        
#pragma warning disable 0649
        private CCOLGeneratorCodeStringSettingModel _usrt;
        private CCOLGeneratorCodeStringSettingModel _usrtdim;
        private CCOLGeneratorCodeStringSettingModel _hrt;
        private CCOLGeneratorCodeStringSettingModel _tnlrt;
#pragma warning restore 0649
        private string _hperiod;
        private string _prmperrt;
        private string _prmperrta;
        private string _prmperrtdim;

        #endregion // Fields
        
        public override void CollectCCOLElements(ControllerModel c)
        {
            _myElements = new List<CCOLElement>();
            _myBitmapOutputs = new List<CCOLIOElement>();

            foreach (var rt in c.Signalen.Rateltikkers)
            {
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_usrt}{rt.FaseCyclus}", _usrt, rt.FaseCyclus));
                _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_hrt}{rt.FaseCyclus}", _hrt, rt.FaseCyclus));
                
                if (c.Signalen.DimUitgangPerTikker)
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_usrtdim}{rt.FaseCyclus}", _usrtdim, rt.FaseCyclus));
                }

                if (rt.Type == RateltikkerTypeEnum.Hoeflake)
                {
                    _myElements.Add(CCOLGeneratorSettingsProvider.Default.CreateElement($"{_tnlrt}{rt.FaseCyclus}", _tnlrt, rt.FaseCyclus));
                }

                _myBitmapOutputs.Add(new CCOLIOElement(rt.BitmapData as IOElementModel, $"{_uspf}{_usrt}{rt.FaseCyclus}"));
            }
        }

        public override bool HasCCOLElements()
        {
            return true;
        }

        public override IEnumerable<CCOLElement> GetCCOLElements(CCOLElementTypeEnum type)
        {
            return _myElements.Where(x => x.Type == type);
        }

        public override bool HasCCOLBitmapOutputs()
        {
            return true;
        }

        public override IEnumerable<CCOLIOElement> GetCCOLBitmapOutputs()
        {
            return _myBitmapOutputs;
        }

        public override int HasCode(CCOLCodeTypeEnum type)
        {
            switch (type)
            {
                case CCOLCodeTypeEnum.RegCSystemApplication:
                    return 60;
                default:
                    return 0;
            }
        }

        public override string GetCode(ControllerModel c, CCOLCodeTypeEnum type, string ts)
        {
            StringBuilder sb = new StringBuilder();

            switch (type)
            {
                case CCOLCodeTypeEnum.RegCSystemApplication:
                    if(c.Signalen.Rateltikkers.Count == 0)
                    {
                        return "";
                    }
                    sb.AppendLine($"{ts}/* uitsturing aanvraag rateltikkers */");
                    foreach (var rt in c.Signalen.Rateltikkers)
                    {
                        switch (rt.Type)
                        {
                            case RateltikkerTypeEnum.Accross:
                                sb.Append($"{ts}GUS[{_uspf}{_usrt}{rt.FaseCyclus}] = Rateltikkers_Accross({_fcpf}{rt.FaseCyclus}, {_hpf}{_hrt}{rt.FaseCyclus}, {_hpf}{_hperiod}{_prmperrta}, {_hpf}{_hperiod}{_prmperrt}, ");
                                foreach (var d in rt.Detectoren)
                                {
                                    sb.Append($"{_dpf}{d.Detector}, ");
                                }
                                sb.AppendLine("END);");
                                break;
                            case RateltikkerTypeEnum.Hoeflake:
                                sb.Append($"{ts}GUS[{_uspf}{_usrt}{rt.FaseCyclus}] = Rateltikkers({_fcpf}{rt.FaseCyclus}, {_hpf}{_hrt}{rt.FaseCyclus}, {_hpf}{_hperiod}{_prmperrta}, {_hpf}{_hperiod}{_prmperrt}, {_tpf}{_tnlrt}{rt.FaseCyclus}, ");
                                foreach (var d in rt.Detectoren)
                                {
                                    sb.Append($"{_dpf}{d.Detector}, ");
                                }
                                sb.AppendLine("END);");
                                break;
                        }
                    }

                    if (c.Signalen.DimUitgangPerTikker)
                    {
                        sb.AppendLine();
                        sb.AppendLine($"{ts}/* uitsturing dimming signaal per rateltikker */");
                        foreach (var rt in c.Signalen.Rateltikkers)
                        {
                            sb.AppendLine($"{ts}GUS[{_uspf}{_usrtdim}{rt.FaseCyclus}] = H[{_hpf}{_hperiod}{_prmperrtdim}];");
                        }
                    }

                    sb.AppendLine();
                    return sb.ToString();

                default:
                    return null;
            }
        }

        public override bool SetSettings(CCOLGeneratorClassWithSettingsModel settings)
        {
            _hperiod = CCOLGeneratorSettingsProvider.Default.GetElementName("hperiod");
            _prmperrt = CCOLGeneratorSettingsProvider.Default.GetElementName("prmperrt");
            _prmperrta = CCOLGeneratorSettingsProvider.Default.GetElementName("prmperrta");
            _prmperrtdim = CCOLGeneratorSettingsProvider.Default.GetElementName("prmperrtdim");

            return base.SetSettings(settings);
        }
    }
}
