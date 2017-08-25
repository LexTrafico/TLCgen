﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using TLCGen.Models.Enumerations;
using TLCGen.Models.Settings;

namespace TLCGen.Models
{
    [Serializable]
    public class ControllerDataModel
    {
        #region Fields

        #endregion // Fields

        #region Properties

        [HasDefault(false)]
        public string Naam { get; set; }
        [HasDefault(false)]
        public string Stad { get; set; }
        [HasDefault(false)]
        public string Straat1 { get; set; }
        [HasDefault(false)]
        public string Straat2 { get; set; }
        [HasDefault(false)]
        public string BitmapNaam { get; set; }

        public int Fasebewaking { get; set; }
        public CCOLVersieEnum CCOLVersie { get; set; }
        public KWCTypeEnum KWCType { get; set; }
        public bool KWCUitgebreid { get; set; } // Note: this is not used yet, only meant for potential future functionality
        public VLOGTypeEnum VLOGType { get; set; }
        public bool VLOGInTestOmgeving { get; set; }
        public bool GarantieOntruimingsTijden { get; set; }
        public bool ExtraMeeverlengenInWG { get; set; }
        public GroentijdenTypeEnum TypeGroentijden { get; set; }
        public AansturingWaitsignalenEnum AansturingWaitsignalen { get; set; }
        public SynchronisatieTypeEnum NaloopSynchronisatieType { get; set; }

        public int HuidigeVersieMajor { get; set; }
        public int HuidigeVersieMinor { get; set; }
        public int HuidigeVersieRevision { get; set; }

        [Browsable(false)]
        public string TLCGenVersie { get; set; }

        [Browsable(false)]
        public List<SegmentDisplayElementModel> SegmentenDisplayBitmapData { get; set; }

        // Note: this is a feature for future use; it is not yet disclosed to the user
        private SegmentDisplayTypeEnum _segmentDisplayType;
        [Browsable(false)]
        public SegmentDisplayTypeEnum SegmentDisplayType
        {
            get => _segmentDisplayType;
            set => _segmentDisplayType = value;
        }

        public FixatieModel FixatieData { get; set; }

        [XmlIgnore]
        public bool FixatieMogelijk
        {
            get => FixatieData.FixatieMogelijk;
            set => FixatieData.FixatieMogelijk = value;
        }

        [XmlIgnore]
        public bool BijkomenTijdensFixatie
        {
            get => FixatieData.BijkomenTijdensFixatie;
            set => FixatieData.BijkomenTijdensFixatie = value;
        }


        [XmlElement(ElementName = "Versie")]
        public List<VersieModel> Versies { get; set; }

        #endregion // Properties

        #region Public Methods

        public void SetSegmentOutputs()
        {
            SegmentenDisplayBitmapData.Clear();
            switch (_segmentDisplayType)
            {
                case SegmentDisplayTypeEnum.EnkelDisplay:
                    if (SegmentenDisplayBitmapData.Count == 0)
                    {
                        for (int i = 1; i <= 7; ++i)
                        {
                            SegmentenDisplayBitmapData.Add(new SegmentDisplayElementModel() { Naam = "segm" + i });
                        }
                    }
                    break;
                case SegmentDisplayTypeEnum.DrieCijferDisplay:
                    if (SegmentenDisplayBitmapData.Count == 0)
                    {
                        for (int i = 1; i <= 7; ++i)
                        {
                            SegmentenDisplayBitmapData.Add(new SegmentDisplayElementModel() { Naam = "segma" + i });
                        }
                        for (int i = 1; i <= 7; ++i)
                        {
                            SegmentenDisplayBitmapData.Add(new SegmentDisplayElementModel() { Naam = "segmb" + i });
                        }
                        for (int i = 1; i <= 7; ++i)
                        {
                            SegmentenDisplayBitmapData.Add(new SegmentDisplayElementModel() { Naam = "segmc" + i });
                        }
                    }
                    break;
            }
        }

        #endregion // Public Methods

        #region Serialization

        [OnDeserialized]
        public void OnDeserialized(StreamingContext context)
        {
            if(SegmentenDisplayBitmapData?.Count == 0)
            {
                for (int i = 1; i <= 7; ++i)
                {
                    SegmentenDisplayBitmapData.Add(new SegmentDisplayElementModel() { Naam = "segm" + i });
                }
            }
        }

        #endregion // Serialization

        #region Constructor

        public ControllerDataModel()
        {
            FixatieData = new FixatieModel();
            Versies = new List<VersieModel>();
            SegmentenDisplayBitmapData = new List<SegmentDisplayElementModel>();
        }

        #endregion // Constructor
    }
}
