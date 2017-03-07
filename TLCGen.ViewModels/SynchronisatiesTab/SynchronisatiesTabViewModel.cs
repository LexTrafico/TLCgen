﻿using GalaSoft.MvvmLight.Messaging;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using TLCGen.Extensions;
using TLCGen.Helpers;
using TLCGen.Messaging;
using TLCGen.Messaging.Messages;
using TLCGen.Messaging.Requests;
using TLCGen.Models;
using TLCGen.Models.Enumerations;
using TLCGen.Plugins;
using TLCGen.ViewModels.Enums;

namespace TLCGen.ViewModels
{
    [TLCGenTabItem(index: 3)]
    public class SynchronisatiesTabViewModel : TLCGenTabItemViewModel
    {

        #region Fields
        
        private List<string> _AllDetectoren;
        private ObservableCollection<string> _FasenNames;
        private ObservableCollection<string> _Detectoren;
        private ObservableCollection<GarantieTijdConvertHelper> _GarantieTijdenConvertValues;
        private SynchronisatieViewModel _SelectedSynchronisatie;
        private GarantieTijdConvertHelper _SelectedGarantieTijdenConvertValue;
        private bool _MatrixChanged;
        private IntersignaalGroepTypeEnum _DisplayType;

        RelayCommand _DeleteValueCommand;
        RelayCommand _AddGarantieConvertValue;
        RelayCommand _RemoveGarantieConvertValue;
        RelayCommand _SetGarantieValuesCommand;

        #endregion // Fields

        #region Properties

        public IntersignaalGroepTypeEnum DisplayType
        {
            get { return _DisplayType; }
            set
            {
#warning TODO: check integrity before proceding
                _DisplayType = value;
                if (ConflictMatrix != null)
                {
                    foreach (SynchronisatieViewModel cvm in ConflictMatrix)
                    {
                        cvm.DisplayType = value;
                    }
                }
                RaisePropertyChanged(null);
            }
        }

        /// <summary>
        /// Collection of strings used to display matrix column and row headers
        /// </summary>
        public ObservableCollection<string> FasenNames
        {
            get
            {
                if (_FasenNames == null)
                    _FasenNames = new ObservableCollection<string>();
                return _FasenNames;
            }
        }

        /// <summary>
        /// Returns the collection of FaseCyclusViewModel from the main ControllerViewModel
        /// </summary>
        public IReadOnlyList<FaseCyclusModel> Fasen
        {
            get
            {
                return _Controller.Fasen;
            }
        }

        public ObservableCollection<string> Detectoren
        {
            get
            {
                if (_Detectoren == null)
                    _Detectoren = new ObservableCollection<string>();
                return _Detectoren;
            }
        }

        public ObservableCollection<GarantieTijdConvertHelper> GarantieTijdenConvertValues
        {
            get
            {
                if (_GarantieTijdenConvertValues == null)
                {
                    _GarantieTijdenConvertValues = new ObservableCollection<GarantieTijdConvertHelper>();
                }
                return _GarantieTijdenConvertValues;
            }
        }

        public GarantieTijdConvertHelper SelectedGarantieTijdenConvertValue
        {
            get { return _SelectedGarantieTijdenConvertValue; }
            set
            {
                _SelectedGarantieTijdenConvertValue = value;
                RaisePropertyChanged("SelectedGarantieTijdenConvertValue");
            }
        }

        public SynchronisatieViewModel SelectedSynchronisatie
        {
            get
            {
                return _SelectedSynchronisatie;
            }
            set
            {
                _SelectedSynchronisatie = value;
                if (_AllDetectoren != null && !string.IsNullOrEmpty(value.FaseVan))
                {
                    Detectoren.Clear();
                    foreach(var fc in _Controller.Fasen)
                    {
                        if (fc.Naam == _SelectedSynchronisatie.FaseVan)
                        {
                            foreach (var dm in fc.Detectoren)
                            {
                                Detectoren.Add(dm.Naam);
                            }
                            break;
                        }
                    }
                    value.NaloopVM.DetectieAfhankelijkPossible = Detectoren.Count > 0;
                    value.MeeaanvraagVM.DetectieAfhankelijkPossible = Detectoren.Count > 0;
                }
                RaisePropertyChanged("SelectedSynchronisatie");
                if (_SelectedSynchronisatie.DisplayType == IntersignaalGroepTypeEnum.Gelijkstart)
                {
                    RaisePropertyChanged("GelijkstartDeelConflict");
                    RaisePropertyChanged("GelijkstartOntruimingstijdFaseVan");
                    RaisePropertyChanged("GelijkstartOntruimingstijdFaseNaar");
                }
                RaisePropertyChanged("Comment1");
                RaisePropertyChanged("Comment2");
            }
        }

        /// <summary>
        /// Symmetrical, two dimensional matrix used to display phasecycle conflicts.
        /// </summary>
        public SynchronisatieViewModel[,] ConflictMatrix { get; set; }

        /// <summary>
        /// Boolean set by instances of ConflictViewModel when their DisplayWaarde property is 
        /// set by the user. We use this to monitor changes to the model, and to check if we need
        /// to check the matrix for symmetry if the user changes tabs, or tries to save the model.
        /// </summary>
        public bool MatrixChanged
        {
            get { return _MatrixChanged; }
            set
            {
                if (value)
                    RaisePropertyChanged("MatrixChanged", _MatrixChanged, value, true);
                _MatrixChanged = value;
            }
        }

        public bool UseGarantieOntruimingsTijden
        {
            get { return _Controller.Data.GarantieOntruimingsTijden; }
            set
            {
                RaisePropertyChanged("UseGarantieOntruimingsTijden", _Controller.Data.GarantieOntruimingsTijden, value, true);
                _Controller.Data.GarantieOntruimingsTijden = value;
                MatrixChanged = true;
            }
        }

        #endregion // Properties

        #region InterSignaalGroep Object Properties

        public int GelijkstartOntruimingstijdFaseVan
        {
            get
            {
                if (SelectedSynchronisatie == null)
                    return 0;

                return SelectedSynchronisatie.Gelijkstart.GelijkstartOntruimingstijdFaseVan;
            }
            set
            {
                SelectedSynchronisatie.Gelijkstart.GelijkstartOntruimingstijdFaseVan = value;
                foreach(SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if(svm.FaseVan == SelectedSynchronisatie.FaseNaar && svm.FaseNaar == SelectedSynchronisatie.FaseVan)
                    {
                        svm.Gelijkstart.GelijkstartOntruimingstijdFaseNaar = value;
                    }
                }
                RaisePropertyChanged("GelijkstartOntruimingstijdFaseVan");
            }
        }
        public int GelijkstartOntruimingstijdFaseNaar
        {
            get
            {
                if (SelectedSynchronisatie == null)
                    return 0;

                return SelectedSynchronisatie.Gelijkstart.GelijkstartOntruimingstijdFaseNaar;
            }
            set
            {
                SelectedSynchronisatie.Gelijkstart.GelijkstartOntruimingstijdFaseNaar = value;
                foreach (SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == SelectedSynchronisatie.FaseNaar && svm.FaseNaar == SelectedSynchronisatie.FaseVan)
                    {
                        svm.Gelijkstart.GelijkstartOntruimingstijdFaseVan = value;
                    }
                }
                RaisePropertyChanged("GelijkstartOntruimingstijdFaseNaar");
            }
        }
        public bool GelijkstartDeelConflict
        {
            get
            {
                if (SelectedSynchronisatie == null)
                    return false;

                return SelectedSynchronisatie.Gelijkstart.DeelConflict;
            }
            set
            {
                SelectedSynchronisatie.Gelijkstart.DeelConflict = value;
                foreach (SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == SelectedSynchronisatie.FaseNaar && svm.FaseNaar == SelectedSynchronisatie.FaseVan)
                    {
                        svm.Gelijkstart.DeelConflict = value;
                    }
                }
                RaisePropertyChanged("GelijkstartDeelConflict");
            }
        }

        public int VoorstartTijd
        {
            get
            {
                if (SelectedSynchronisatie == null)
                    return 0;

                return SelectedSynchronisatie.Voorstart.VoorstartTijd;
            }
            set
            {
                SelectedSynchronisatie.Voorstart.VoorstartTijd = value;
                RaisePropertyChanged("VoorstartTijd");
            }
        }

        public int VoorstartOntruimingstijd
        {
            get
            {
                if (SelectedSynchronisatie == null)
                    return 0;

                return SelectedSynchronisatie.Voorstart.VoorstartOntruimingstijd;
            }
            set
            {
                SelectedSynchronisatie.Voorstart.VoorstartOntruimingstijd = value;
                RaisePropertyChanged("VoorstartOntruimingstijd");
            }
        }

        public string Comment1
        {
            get
            {
                switch (DisplayType)
                {
                    case IntersignaalGroepTypeEnum.Gelijkstart:
                        return $"Fictive ontruimingstijd van {SelectedSynchronisatie.FaseVan} naar {SelectedSynchronisatie.FaseNaar}";
                    case IntersignaalGroepTypeEnum.Voorstart:
                        return $"Voorstarttijd van {SelectedSynchronisatie.FaseVan} naar {SelectedSynchronisatie.FaseNaar}";
                    case IntersignaalGroepTypeEnum.Meeaanvraag:
                        return $"Type meeaanvraag van {SelectedSynchronisatie.FaseVan} naar {SelectedSynchronisatie.FaseNaar}";
                    default:
                        return "";
                }
            }
        }
        public string Comment2
        {
            get
            {
                switch (DisplayType)
                {
                    case IntersignaalGroepTypeEnum.Gelijkstart:
                        return $"Fictieve ontruimingstijd van {SelectedSynchronisatie.FaseNaar} naar {SelectedSynchronisatie.FaseVan}";
                    case IntersignaalGroepTypeEnum.Voorstart:
                        return $"Voorstart ontruimingstijd van {SelectedSynchronisatie.FaseVan} naar {SelectedSynchronisatie.FaseNaar}";
                    default:
                        return "";
                }
            }
        }

        public NaloopViewModel NaloopVM
        {
            get
            {
                if (SelectedSynchronisatie == null || !SelectedSynchronisatie.HasNaloop)
                    return null;
                return new NaloopViewModel(SelectedSynchronisatie.Naloop);
            }
        }

        #endregion // SynchInterSignaalGroep BOjecObject Properties

        #region TabItem Overrides

        public override ImageSource Icon
        {
            get
            {
                ResourceDictionary dict = new ResourceDictionary();
                Uri u = new Uri("pack://application:,,,/" +
                    System.Reflection.Assembly.GetExecutingAssembly().GetName().Name +
                    ";component/" + "TabIcons.xaml");
                dict.Source = u;
                return (DrawingImage)dict["InterSignaalGroepTabDrawingImage"];
            }
        }

        public override string DisplayName
        {
            get
            {
                return "InterSignaalGroep";
            }
        }

        public override bool IsEnabled
        {
            get { return true; }
            set { }
        }

        public override bool OnDeselectedPreview()
        {
            return IsMatrixOK();   
        }

        public override void OnSelected()
        {
            _AllDetectoren = new List<string>();
            foreach (FaseCyclusModel fcm in Controller.Fasen)
            {
                foreach (DetectorModel dm in fcm.Detectoren)
                {
                    _AllDetectoren.Add(dm.Naam);
                }
            }
            foreach (DetectorModel dm in Controller.Detectoren)
            {
                _AllDetectoren.Add(dm.Naam);
            }
        }

        public override ControllerModel Controller
        {
            get
            {
                return base.Controller;
            }

            set
            {
                base.Controller = value;
                DisplayType = IntersignaalGroepTypeEnum.Conflict;
                BuildConflictMatrix();
            }
        }

        #endregion // TabItem Overrides

        #region Commands

        public ICommand DeleteValueCommand
        {
            get
            {
                if (_DeleteValueCommand == null)
                {
                    _DeleteValueCommand = new RelayCommand(DeleteValueCommand_Executed, DeleteValueCommand_CanExecute);
                }
                return _DeleteValueCommand;
            }
        }

        public ICommand SetGarantieValuesCommand
        {
            get
            {
                if (_SetGarantieValuesCommand == null)
                {
                    _SetGarantieValuesCommand = new RelayCommand(SetGarantieValuesCommand_Executed, SetGarantieValuesCommand_CanExecute);
                }
                return _SetGarantieValuesCommand;
            }
        }

        public ICommand AddGarantieConvertValue
        {
            get
            {
                if (_AddGarantieConvertValue == null)
                {
                    _AddGarantieConvertValue = new RelayCommand(AddGarantieConvertValue_Executed, AddGarantieConvertValue_CanExecute);
                }
                return _AddGarantieConvertValue;
            }
        }

        public ICommand RemoveGarantieConvertValue
        {
            get
            {
                if (_RemoveGarantieConvertValue == null)
                {
                    _RemoveGarantieConvertValue = new RelayCommand(RemoveGarantieConvertValue_Executed, RemoveGarantieConvertValue_CanExecute);
                }
                return _RemoveGarantieConvertValue;
            }
        }

        #endregion // Commands

        #region Command functionality

        void DeleteValueCommand_Executed(object prm)
        {
            if(SelectedSynchronisatie != null)
                SelectedSynchronisatie.ConflictValue = "";
        }

        bool DeleteValueCommand_CanExecute(object prm)
        {
            return SelectedSynchronisatie != null;
        }

        private bool SetGarantieValuesCommand_CanExecute(object obj)
        {
            return ConflictMatrix != null && 
                   Fasen != null && 
                   DisplayType == IntersignaalGroepTypeEnum.GarantieConflict &&
                   GarantieTijdenConvertValues != null &&
                   GarantieTijdenConvertValues.Count > 0;
        }

        private void SetGarantieValuesCommand_Executed(object obj)
        {
            int fccount = Fasen.Count;

            for (int fcvm_from = 0; fcvm_from < fccount; ++fcvm_from)
            {
                for (int fcvm_to = 0; fcvm_to < fccount; ++fcvm_to)
                {
                    int i;
                    if (!string.IsNullOrWhiteSpace(ConflictMatrix[fcvm_from, fcvm_to].GetConflictValue()) && Int32.TryParse(ConflictMatrix[fcvm_from, fcvm_to].GetConflictValue(), out i))
                    {
                        foreach(var conv in GarantieTijdenConvertValues)
                        {
                            if (i >= conv.Van && i < conv.Tot)
                            {
                                i -= conv.Verschil;
                                if (i < 0) i = 0;
                                break;
                            }
                        }
                        ConflictMatrix[fcvm_from, fcvm_to].ConflictValue = i.ToString();
                    }
                }
            }
        }

        private bool AddGarantieConvertValue_CanExecute(object obj)
        {
            return true;
        }

        private void AddGarantieConvertValue_Executed(object obj)
        {
            GarantieTijdConvertHelper h = new GarantieTijdConvertHelper(this);

            _SettingGarConvs = true;

            if (GarantieTijdenConvertValues.Count > 0)
            {
                h.Tot = GarantieTijdenConvertValues[GarantieTijdenConvertValues.Count - 1].Tot;
                h.MinVan = h.Van = h.Tot;
            }
            else
            {
                h.MinVan = h.Van = h.Tot = 0;
            }
            h.Verschil = 0;

            _SettingGarConvs = false;

            GarantieTijdenConvertValues.Add(h);
        }

        private bool RemoveGarantieConvertValue_CanExecute(object obj)
        {
            return SelectedGarantieTijdenConvertValue != null;
        }

        private void RemoveGarantieConvertValue_Executed(object obj)
        {
            GarantieTijdenConvertValues.Remove(SelectedGarantieTijdenConvertValue);
            SelectedGarantieTijdenConvertValue = null;

            SetGarantieConvertValuesVan();
            SetGarantieConvertValuesTot();
        }

        #endregion // Command functionality

        #region Private methods

        private bool IsMatrixOK()
        {
            string s = Integrity.TLCGenIntegrityChecker.IsConflictMatrixOK(_Controller);
            if (s == null)
            {
                if (_MatrixChanged == true)
                {
                    Integrity.TLCGenControllerModifier.Default.CorrectModel_AlteredConflicts();
                    Messenger.Default.Send(new ConflictsChangedMessage());
                }
                _MatrixChanged = false;
                return true;
            }
            else
            {
                MessageBox.Show(s, "Fout in conflictmatrix");
                return false;
            }
        }

        #endregion // Private methods

        #region Public methods

        private bool _SettingGarConvs = false;

        public void SetGarantieConvertValuesVan()
        {
            if (_SettingGarConvs || GarantieTijdenConvertValues.Count < 2)
                return;

            _SettingGarConvs = true;

            for (int i = 1; i < GarantieTijdenConvertValues.Count; ++i)
            {
                GarantieTijdenConvertValues[i].MinVan = GarantieTijdenConvertValues[i - 1].Tot;
                GarantieTijdenConvertValues[i].Van = GarantieTijdenConvertValues[i].MinVan;
            }

            _SettingGarConvs = false;
        }

        public void SetGarantieConvertValuesTot()
        {
            if (_SettingGarConvs || GarantieTijdenConvertValues.Count < 2)
                return;

            _SettingGarConvs = true;

            for (int i = 0; i < (GarantieTijdenConvertValues.Count - 1); ++i)
            {
                GarantieTijdenConvertValues[i].Tot = GarantieTijdenConvertValues[i + 1].Van;
            }

            _SettingGarConvs = false;
        }

        /// <summary>
        /// Builds a new string[,] to be exposed to the View. The 2D array is filled with data
        /// from the collection of FaseCyclusViewModel, and then from the collection of ConflictViewModel
        /// they contain.
        /// </summary>
        public void BuildConflictMatrix()
        {
            if (_Controller == null ||
                _Controller.Fasen == null ||
                _Controller.Fasen.Count <= 0)
            {
                ConflictMatrix = null;
                RaisePropertyChanged("ConflictMatrix");
                return;
            }

            int fccount = Fasen.Count;

            _FasenNames = new ObservableCollection<string>();
            foreach (FaseCyclusModel fcvm in Fasen)
            {
                FasenNames.Add(fcvm.Naam);
            }
            RaisePropertyChanged("FasenNames");

            ConflictMatrix = new SynchronisatieViewModel[fccount, fccount];
            for (int fcm_from = 0; fcm_from < fccount; ++fcm_from)
            {
                for (int fcm_to = 0; fcm_to < fccount; ++fcm_to)
                {
                    if (ConflictMatrix[fcm_from, fcm_to] == null)
                    {
                        if (fcm_from == fcm_to)
                        {
                            ConflictMatrix[fcm_from, fcm_to] = new SynchronisatieViewModel(true);
                        }

                        else
                        {
                            ConflictMatrix[fcm_from, fcm_to] = new SynchronisatieViewModel();
                            ConflictMatrix[fcm_from, fcm_to].FaseVan = Fasen[fcm_from].Naam;
                            ConflictMatrix[fcm_from, fcm_to].FaseNaar = Fasen[fcm_to].Naam;
                        }
                    }
                    ConflictMatrix[fcm_from, fcm_to].DisplayType = this.DisplayType;
                }
            }

            foreach (ConflictModel cm in _Controller.InterSignaalGroep.Conflicten)
            {
                foreach(SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == cm.FaseVan && svm.FaseNaar == cm.FaseNaar)
                    {
                        svm.Conflict = cm;
                        break;
                    }
                }
            }

            foreach (NaloopModel nm in _Controller.InterSignaalGroep.Nalopen)
            {
                foreach (SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == nm.FaseVan && svm.FaseNaar == nm.FaseNaar)
                    {
                        svm.Naloop = nm;
                        foreach (SynchronisatieViewModel svm_opp in ConflictMatrix)
                        {
                            if (svm_opp.FaseVan == nm.FaseNaar && svm_opp.FaseNaar == nm.FaseVan)
                            {
                                svm_opp.HasOppositeNaloop = true;
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            foreach (GelijkstartModel gm in _Controller.InterSignaalGroep.Gelijkstarten)
            {
                foreach (SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == gm.FaseVan && svm.FaseNaar == gm.FaseNaar)
                    {
                        svm.Gelijkstart = gm;
                        foreach (SynchronisatieViewModel svm_opp in ConflictMatrix)
                        {
                            if (svm_opp.FaseVan == gm.FaseNaar && svm_opp.FaseNaar == gm.FaseVan)
                            {
                                svm_opp.HasGelijkstart = true;
                                svm_opp.Gelijkstart.GelijkstartOntruimingstijdFaseNaar = svm.Gelijkstart.GelijkstartOntruimingstijdFaseVan;
                                svm_opp.Gelijkstart.GelijkstartOntruimingstijdFaseVan = svm.Gelijkstart.GelijkstartOntruimingstijdFaseNaar;
                                svm_opp.Gelijkstart.DeelConflict = svm.Gelijkstart.DeelConflict;
                                break;
                            }
                        }
                        break;
                    }
                }
            }


            foreach (VoorstartModel vm in _Controller.InterSignaalGroep.Voorstarten)
            {
                foreach (SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == vm.FaseVan && svm.FaseNaar == vm.FaseNaar)
                    {
                        svm.Voorstart = vm;
                        foreach (SynchronisatieViewModel svm_opp in ConflictMatrix)
                        {
                            if (svm_opp.FaseVan == vm.FaseNaar && svm_opp.FaseNaar == vm.FaseVan)
                            {
                                svm_opp.HasOppositeVoorstart = true;
                                break;
                            }
                        }
                    }
                }
            }

            foreach (MeeaanvraagModel mm in _Controller.InterSignaalGroep.Meeaanvragen)
            {
                foreach (SynchronisatieViewModel svm in ConflictMatrix)
                {
                    if (svm.FaseVan == mm.FaseVan && svm.FaseNaar == mm.FaseNaar)
                    {
                        svm.Meeaanvraag = mm;
                        foreach (SynchronisatieViewModel svm_opp in ConflictMatrix)
                        {
                            if (svm_opp.FaseVan == mm.FaseNaar && svm_opp.FaseNaar == mm.FaseVan)
                            {
                                svm_opp.HasOppositeMeeaanvraag = true;
                                break;
                            }
                        }
                    }
                }
            }
            
            RaisePropertyChanged("ConflictMatrix");

            _MatrixChanged = false;

            if (ConflictMatrix != null && ConflictMatrix.Length > 0)
                SelectedSynchronisatie = ConflictMatrix[0, 0];
        }

        /// <summary>
        /// Reads the property string[,] ConflictMatrix and saves all relevant entries as 
        /// instances of ConflictViewmodel in the collection of the relevant FaseCyclusViewModel.
        /// This also updates the Model data.
        /// </summary>
        public void SaveConflictMatrix()
        {
            if(ConflictMatrix == null || Fasen == null || !_MatrixChanged)
            {
                return;
            }

            int fccount = Fasen.Count;

            // Call extension method RemoveAll instead of built-in Clear(), see:
            // http://stackoverflow.com/questions/224155/when-clearing-an-observablecollection-there-are-no-items-in-e-olditems
            _Controller.InterSignaalGroep.Conflicten.RemoveAll();
            _Controller.InterSignaalGroep.Voorstarten.RemoveAll();
            _Controller.InterSignaalGroep.Gelijkstarten.RemoveAll();
            _Controller.InterSignaalGroep.Nalopen.RemoveAll();
            _Controller.InterSignaalGroep.Meeaanvragen.RemoveAll();
            bool[,] gelijkstartsaved = new bool[fccount, fccount];
            for (int fcvm_from = 0; fcvm_from < fccount; ++fcvm_from)
            {
                for (int fcvm_to = 0; fcvm_to < fccount; ++fcvm_to)
                {
                    if (ConflictMatrix[fcvm_from, fcvm_to].HasConflict || ConflictMatrix[fcvm_from, fcvm_to].HasGarantieConflict)
                    {
                        _Controller.InterSignaalGroep.Conflicten.Add(ConflictMatrix[fcvm_from, fcvm_to].Conflict);
                    }

                    if(ConflictMatrix[fcvm_from, fcvm_to].HasGelijkstart)
                    {
                        if(!gelijkstartsaved[fcvm_from, fcvm_to])
                            _Controller.InterSignaalGroep.Gelijkstarten.Add(ConflictMatrix[fcvm_from, fcvm_to].Gelijkstart);
                        gelijkstartsaved[fcvm_from, fcvm_to] = true;
                        gelijkstartsaved[fcvm_to, fcvm_from] = true;
                    }

                    if (ConflictMatrix[fcvm_from, fcvm_to].HasVoorstart)
                    {
                        _Controller.InterSignaalGroep.Voorstarten.Add(ConflictMatrix[fcvm_from, fcvm_to].Voorstart);
                    }

                    if (ConflictMatrix[fcvm_from, fcvm_to].HasNaloop)
                    {
                        _Controller.InterSignaalGroep.Nalopen.Add(ConflictMatrix[fcvm_from, fcvm_to].Naloop);
                    }

                    if (ConflictMatrix[fcvm_from, fcvm_to].HasMeeaanvraag)
                    {
                        _Controller.InterSignaalGroep.Meeaanvragen.Add(ConflictMatrix[fcvm_from, fcvm_to].Meeaanvraag);
                    }
                }
            }
        }

        #endregion // Public methods

        #region Collection Changed

        #endregion // Collection Changed

        #region TLCGen Event handling

        private void OnFasenChanged(FasenChangedMessage message)
        {
            BuildConflictMatrix();
        }

        private void OnFasenSorted(FasenSortedMessage message)
        {
            BuildConflictMatrix();
        }

        private void OnInterSignaalGroepChanged(InterSignaalGroepChangedMessage message)
        {
            _MatrixChanged = true;

            // Conflict
            if (message.Conflict != null)
            {
                int fccount = Fasen.Count;
                for (int fcm_from = 0; fcm_from < fccount; ++fcm_from)
                {
                    for (int fcm_to = 0; fcm_to < fccount; ++fcm_to)
                    {
                        if (Fasen[fcm_from].Naam == message.Conflict.FaseVan &&
                           Fasen[fcm_to].Naam == message.Conflict.FaseNaar)
                        {
                            var cvm2 = ConflictMatrix[fcm_to, fcm_from];
                            switch (ConflictMatrix[fcm_from, fcm_to].ConflictValue)
                            {
                                case "GKL":
                                    if (cvm2.ConflictValue != "GK")
                                        cvm2.ConflictValueNoMessaging = "GK";
                                    break;
                                case "GK":
                                    if (!(cvm2.ConflictValue == "GK" || cvm2.ConflictValue == "GKL"))
                                        cvm2.ConflictValueNoMessaging = "GK";
                                    break;
                                case "FK":
                                    if (cvm2.ConflictValue != "FK")
                                        cvm2.ConflictValueNoMessaging = "FK";
                                    break;
                                case "":
                                    if (cvm2.ConflictValue == "*" || cvm2.ConflictValue == "GK" || cvm2.ConflictValue == "GKL" || cvm2.ConflictValue == "FK")
                                        cvm2.ConflictValueNoMessaging = "";
                                    else
                                    {
                                        int ci;
                                        if (Int32.TryParse(cvm2.ConflictValue, out ci))
                                        {
                                            ConflictMatrix[fcm_from, fcm_to].ConflictValueNoMessaging = "*";
                                        }
                                    }
                                    break;
                                default:
                                    int i;
                                    if (Int32.TryParse(cvm2.ConflictValue, out i))
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        cvm2.ConflictValueNoMessaging = "*";
                                    }
                                    break;
                            }
                            return;
                        }
                    }
                }
                // conflict not found: faulty state!
                throw new NotImplementedException();
            }

            // Naloop
            if(message.Naloop != null)
            {
                int fccount = Fasen.Count;
                for (int fcm_from = 0; fcm_from < fccount; ++fcm_from)
                {
                    for (int fcm_to = 0; fcm_to < fccount; ++fcm_to)
                    {
                        if (Fasen[fcm_from].Naam == message.Naloop.FaseVan &&
                            Fasen[fcm_to].Naam == message.Naloop.FaseNaar)
                        {
                            var cvm2 = ConflictMatrix[fcm_to, fcm_from];
                            cvm2.HasOppositeNaloop = message.IsCoupled;
                        }
                    }
                }
            }

            // Voorstart
            if (message.Voorstart != null)
            {
                int fccount = Fasen.Count;
                for (int fcm_from = 0; fcm_from < fccount; ++fcm_from)
                {
                    for (int fcm_to = 0; fcm_to < fccount; ++fcm_to)
                    {
                        if (Fasen[fcm_from].Naam == message.Voorstart.FaseVan &&
                            Fasen[fcm_to].Naam == message.Voorstart.FaseNaar)
                        {
                            var cvm2 = ConflictMatrix[fcm_to, fcm_from];
                            cvm2.HasOppositeVoorstart = message.IsCoupled;
                        }
                    }
                }
            }

            // Gelijkstart
            if (message.Gelijkstart != null)
            {
                int fccount = Fasen.Count;
                for (int fcm_from = 0; fcm_from < fccount; ++fcm_from)
                {
                    for (int fcm_to = 0; fcm_to < fccount; ++fcm_to)
                    {
                        if (Fasen[fcm_from].Naam == message.Gelijkstart.FaseVan &&
                            Fasen[fcm_to].Naam == message.Gelijkstart.FaseNaar)
                        {
                            var cvm2 = ConflictMatrix[fcm_to, fcm_from];
                            cvm2.IsCoupledNoMessaging = message.IsCoupled;
                        }
                    }
                }
            }

            // Meeaanvraag
            if (message.Meeaanvraag != null)
            {
                int fccount = Fasen.Count;
                for (int fcm_from = 0; fcm_from < fccount; ++fcm_from)
                {
                    for (int fcm_to = 0; fcm_to < fccount; ++fcm_to)
                    {
                        if (Fasen[fcm_from].Naam == message.Meeaanvraag.FaseVan &&
                            Fasen[fcm_to].Naam == message.Meeaanvraag.FaseNaar)
                        {
                            var cvm2 = ConflictMatrix[fcm_to, fcm_from];
                            cvm2.HasOppositeMeeaanvraag = message.IsCoupled;
                        }
                    }
                }
            }
        }

        private bool _IsProcessing = false;
        private void OnProcesSynchornisationsRequested(ProcessSynchronisationsRequest request)
        {
            if (_IsProcessing)
                return;

            _IsProcessing = true;
            SaveConflictMatrix();
            request.Succes = IsMatrixOK();
            _IsProcessing = false;
        }

        #endregion // TLCGen Event handling

        #region Constructor

        public SynchronisatiesTabViewModel() : base()
        {
            Messenger.Default.Register(this, new Action<FasenChangedMessage>(OnFasenChanged));
            Messenger.Default.Register(this, new Action<FasenSortedMessage>(OnFasenSorted));
            Messenger.Default.Register(this, new Action<InterSignaalGroepChangedMessage>(OnInterSignaalGroepChanged));
            Messenger.Default.Register(this, new Action<ProcessSynchronisationsRequest>(OnProcesSynchornisationsRequested));
        }

        #endregion // Constructor
    }
}
