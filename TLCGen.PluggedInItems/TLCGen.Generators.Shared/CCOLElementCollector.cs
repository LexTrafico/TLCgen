using System.Collections.Generic;
using System.IO.Packaging;
using System.Linq;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.Generators.Shared
{
    public static class CCOLElementCollector
    {
        #region Static Fields

        private static Dictionary<string, List<CCOLKoppelSignaal>> _koppelSignalen;

        #endregion // Static Fields

        #region Static Methods

        public static void AddKoppelSignaal(string koppeling, int count, string name, KoppelSignaalRichtingEnum richting)
        {
            var koppelingKey = koppeling + richting.ToString();
            if (!_koppelSignalen.ContainsKey(koppelingKey))
            {
                _koppelSignalen.Add(koppelingKey, new List<CCOLKoppelSignaal>());
            }
            else if (count != 0 && _koppelSignalen[koppelingKey].Any(x => x.Count == count))
            {
                Dependencies.Providers.TLCGenDialogProvider.Default.ShowMessageBox(
                    $"{(richting == KoppelSignaalRichtingEnum.In ? "Ingangssignaal" : "Uitgangssignaal")} " +
                       $"nummer {count} van koppeling {koppeling} wordt reeds elders gebruikt. Dit kan de juiste werking " +
                       $"van de regeling negatief beinvloeden.", 
                    "Koppelsignaal dubbel gebruikt", 
                    System.Windows.MessageBoxButton.OK);
            }
            if (count == 0)
            {
                // TODO msg!
            }
            else
            {
                _koppelSignalen[koppelingKey].Add(new CCOLKoppelSignaal { Count = count, Name = name, Richting = richting });
            }
        }

        public static int GetKoppelSignaalCount(string koppeling, string name, KoppelSignaalRichtingEnum richting)
        {
            var koppelingKey = koppeling + richting;
            CCOLKoppelSignaal ks = null;
            if (_koppelSignalen.ContainsKey(koppelingKey))
            {
                ks = _koppelSignalen[koppelingKey].FirstOrDefault(x => x.Name == name && x.Richting == richting);
            }
            return ks?.Count ?? 0;
        }

        #endregion // Static Methods

        #region Static Public Methods

        public static void Reset()
        {
            _koppelSignalen = new Dictionary<string, List<CCOLKoppelSignaal>>();
        }

        public static void AddAllMaxElements(CCOLElemListData[] lists, bool hasAddFiles = true)
        {
            var has1 = $"{(hasAddFiles ? "1" : "")}";
            lists[0].Elements.Add(new CCOLElement { Define = $"USMAX{has1}", Commentaar = "Totaal aantal uitgangen" });
            lists[1].Elements.Add(new CCOLElement { Define = $"ISMAX{has1}", Commentaar = "Totaal aantal ingangen" });
            lists[2].Elements.Add(new CCOLElement { Define = $"HEMAX{has1}", Commentaar = "Totaal aantal hulpelementen" });
            lists[3].Elements.Add(new CCOLElement { Define = $"MEMAX{has1}", Commentaar = "Totaal aantal geheugen elementen" });
            lists[4].Elements.Add(new CCOLElement { Define = $"TMMAX{has1}", Commentaar = "Totaal aantal timers" });
            lists[5].Elements.Add(new CCOLElement { Define = $"CTMAX{has1}", Commentaar = "Totaal aantal counters" });
            lists[6].Elements.Add(new CCOLElement { Define = $"SCHMAX{has1}", Commentaar = "Totaal aantal schakelaars" });
            lists[7].Elements.Add(new CCOLElement { Define = $"PRMMAX{has1}", Commentaar = "Totaal aantal parameters" });
        }

        public static CCOLElemListData[] CollectAllCCOLElements(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var lists = new CCOLElemListData[8];

            lists[0] = CollectAllUitgangen(controller, pgens, settingsProvider);
            lists[1] = CollectAllIngangen(controller, pgens, settingsProvider);
            lists[2] = CollectAllHulpElementen(controller, pgens, settingsProvider);
            lists[3] = CollectAllGeheugenElementen(controller, pgens, settingsProvider);
            lists[4] = CollectAllTimers(controller, pgens, settingsProvider);
            lists[5] = CollectAllCounters(controller, pgens, settingsProvider);
            lists[6] = CollectAllSchakelaars(controller, pgens, settingsProvider);
            lists[7] = CollectAllParameters(controller, pgens, settingsProvider);

            return lists;
        }

        #endregion // Static Public Methods

        #region Static Private Methods
        
        private static void AddDummyIfNeeded(CCOLElemListData data, string pf)
        {
            if (data.Elements.Count == 0)
            {
                data.Elements.Add(new CCOLElement {Define = $"{pf}dummy", Naam = "dummy"});
            }
        }

        private static CCOLElemListData CollectAllUitgangen(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData { CCOLCode = "US_code" };

            foreach (var pgen in pgens)
            {
                if (!pgen.HasCCOLElements()) continue;
                foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.Uitgang))
                {
                    data.Elements.Add(i);
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.Uitgang));

            return data;
        }

        private static CCOLElemListData CollectAllIngangen(ControllerModel controller, IEnumerable<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData { CCOLCode = "IS_code" };

            foreach (var pgen in pgens)
            {
                if (!pgen.HasCCOLElements()) continue;
                foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.Ingang))
                {
                    data.Elements.Add(i);
                }
            }
            
            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.Ingang));

            return data;
        }

        private static CCOLElemListData CollectAllHulpElementen(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData { CCOLCode = "H_code" };

            // Collect everything

            foreach (var pgen in pgens)
            {
                if (!pgen.HasCCOLElements()) continue;
                foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.HulpElement))
                {
                    data.Elements.Add(i);
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.HulpElement));

            return data;
        }

        private static CCOLElemListData CollectAllGeheugenElementen(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData { CCOLCode = "MM_code" };

            // Collect everything
            foreach (var pgen in pgens)
            {
                if (pgen.HasCCOLElements())
                {
                    foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.GeheugenElement))
                    {
                        data.Elements.Add(i);
                    }
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.GeheugenElement));

            return data;
        }

        private static CCOLElemListData CollectAllTimers(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData
            {
                CCOLCode = "T_code",
                CCOLSetting = "T_max",
                CCOLTType = "T_type"
            };

            foreach (var pgen in pgens)
            {
                if (pgen.HasCCOLElements())
                {
                    foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.Timer))
                    {
                        data.Elements.Add(i);
                    }
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.Timer));

            return data;
        }

        private static CCOLElemListData CollectAllSchakelaars(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData
            {
                CCOLCode = "SCH_code",
                CCOLSetting = "SCH"
            };

            foreach (var pgen in pgens)
            {
                if (pgen.HasCCOLElements())
                {
                    foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.Schakelaar))
                    {
                        data.Elements.Add(i);
                    }
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.Schakelaar));

            return data;
        }

        private static CCOLElemListData CollectAllCounters(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData
            {
                CCOLCode = "C_code",
                CCOLSetting = "C_max",
                CCOLTType = "C_type"
            };

            // Collect everything
            foreach (var pgen in pgens)
            {
                if (pgen.HasCCOLElements())
                {
                    foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.Counter))
                    {
                        data.Elements.Add(i);
                    }
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.Counter));

            return data;
        }

        private static CCOLElemListData CollectAllParameters(ControllerModel controller, List<ICCOLCodePieceGenerator> pgens, ICCOLGeneratorSettingsProvider settingsProvider)
        {
            var data = new CCOLElemListData
            {
                CCOLCode = "PRM_code",
                CCOLSetting = "PRM",
                CCOLTType = "PRM_type"
            };

            // Collect everything
            foreach(var pgen in pgens)
            {
                if (pgen.HasCCOLElements())
                {
                    foreach (var i in pgen.GetCCOLElements(CCOLElementTypeEnum.Parameter))
                    {
                        data.Elements.Add(i);
                    }
                }
            }

            AddDummyIfNeeded(data, settingsProvider.GetPrefix(CCOLElementTypeEnum.Parameter));

            return data;
        }

        #endregion // Static Private Methods

    }
}
