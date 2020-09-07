using System;
using System.Collections.Generic;
using TLCGen.Models;

namespace TLCGen.Generators.Shared
{
    public class CCOLGenerationData
    {
        public CCOLElemListData _uitgangen;
        public CCOLElemListData _ingangen;
        public CCOLElemListData _hulpElementen;
        public CCOLElemListData _geheugenElementen;
        public CCOLElemListData _timers;
        public CCOLElemListData _counters;
        public CCOLElemListData _schakelaars;
        public CCOLElemListData _parameters;

        public List<CCOLIOElement> AllCCOLOutputElements;
        public List<CCOLIOElement> AllCCOLInputElements;
        public List<IOElementModel> AllOutputModelElements;
        public List<IOElementModel> AllInputModelElements;

        public void InsertElementLists(CCOLElemListData[] CCOLElementLists)
        {
            if (CCOLElementLists.Length != 8) throw new ArgumentOutOfRangeException();

            _uitgangen = CCOLElementLists[0];
            _ingangen = CCOLElementLists[1];
            _hulpElementen = CCOLElementLists[2];
            _geheugenElementen = CCOLElementLists[3];
            _timers = CCOLElementLists[4];
            _counters = CCOLElementLists[5];
            _schakelaars = CCOLElementLists[6];
            _parameters = CCOLElementLists[7];
        }
    }

    public class CCOLElemListData
    {
        public List<CCOLElement> Elements { get; set; }

        public string CCOLCode { get; set; }
        public string CCOLSetting { get; set; }
        public string CCOLTType { get; set; }

        public int CCOLCodeWidth => CCOLCode == null ? 0 : CCOLCode.Length;
        public int CCOLSettingWidth => CCOLSetting == null ? 0 : CCOLSetting.Length;
        public int CCOLTTypeWidth => CCOLTType == null ? 0 : CCOLTType.Length;

        public int TTypeMaxWidth { get; set; }
        public int DefineMaxWidth { get; set; }
        public int NameMaxWidth { get; set; }
        public int SettingMaxWidth { get; set; }
        public int CommentsMaxWidth { get; set; }

        public void SetMax()
        {
            foreach (var elem in this.Elements)
            {
                if (elem.Define?.Length > this.DefineMaxWidth) this.DefineMaxWidth = elem.Define.Length;
                if (elem.Naam?.Length > this.NameMaxWidth) this.NameMaxWidth = elem.Naam.Length;
                if (elem.Instelling?.ToString().Length > this.SettingMaxWidth) this.SettingMaxWidth = elem.Instelling.ToString().Length;
                if (elem.Commentaar?.ToString().Length > this.CommentsMaxWidth) this.CommentsMaxWidth = elem.Commentaar.ToString().Length;
                if (elem.TType.ToString().Length > this.TTypeMaxWidth) this.TTypeMaxWidth = elem.TType.ToString().Length;
            }
        }

        public CCOLElemListData()
        {
            DefineMaxWidth = 0;
            NameMaxWidth = 0;
            SettingMaxWidth = 0;

            Elements = new List<CCOLElement>();
        }
    }
}
