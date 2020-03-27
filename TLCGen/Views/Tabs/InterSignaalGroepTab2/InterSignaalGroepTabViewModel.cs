using GalaSoft.MvvmLight.Messaging;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using CodingConnected.FastWpfGrid;
using TLCGen.Extensions;
using TLCGen.Helpers;
using TLCGen.Messaging.Messages;
using TLCGen.Messaging.Requests;
using TLCGen.Models;
using TLCGen.Plugins;
using TLCGen.ViewModels.Enums;

namespace TLCGen.ViewModels
{

    [TLCGenTabItem(index: 16)]
    public class SynchronisatiesTabViewModel2 : TLCGenTabItemViewModel
    {

        #region Fields

        private List<string> _AllDetectoren;
        private ObservableCollection<string> _FasenNames;
        private ObservableCollection<string> _Detectoren;
        private bool _MatrixChanged;
        private DataTableGridModel _model;

        #endregion // Fields

        #region Properties

        public DataTableGridModel Model
        {
            get => _model;
            set
            {
                _model = value; 
                RaisePropertyChanged();
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

        public string TijdenLabel => _Controller?.Data?.Intergroen == true ? "Intergroen tijden" : "Ontruimingstijden";

        public string GarantieTijdenLabel => _Controller?.Data?.Intergroen == true ? "Gar. interg. tijden" : "Gar. ontr. tijden";

        public ObservableCollection<string> Detectoren
        {
            get
            {
                if (_Detectoren == null)
                    _Detectoren = new ObservableCollection<string>();
                return _Detectoren;
            }
        }

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
            get { return _Controller?.Data?.GarantieOntruimingsTijden ?? false; }
            set
            {
                _Controller.Data.GarantieOntruimingsTijden = value;
                MatrixChanged = true;
                RaisePropertyChanged("UseGarantieOntruimingsTijden", _Controller.Data.GarantieOntruimingsTijden, value, true);
            }
        }

        #endregion // Properties

        #region TabItem Overrides

        public override ImageSource Icon
        {
            get
            {
                var dict = new ResourceDictionary();
                var u = new Uri("pack://application:,,,/" +
                    System.Reflection.Assembly.GetExecutingAssembly().GetName().Name +
                    ";component/" + "Resources/TabIcons.xaml");
                dict.Source = u;
                return (DrawingImage)dict["InterSignaalGroepTabDrawingImage"];
            }
        }

        public override string DisplayName => "InterSignaalGroep2";

        public override bool IsEnabled
        {
            get => true;
            set { }
        }

        public override bool OnDeselectedPreview()
        {
            if (_Controller != null)
            {
                return IsMatrixOK();
            }
            else
            {
                return true;
            }
        }

        public override void OnSelected()
        {
            _AllDetectoren = new List<string>();
            if (Controller != null)
            {
                foreach (var fcm in Controller.Fasen)
                {
                    foreach (var dm in fcm.Detectoren)
                    {
                        _AllDetectoren.Add(dm.Naam);
                    }
                }
                foreach (var dm in Controller.Detectoren)
                {
                    _AllDetectoren.Add(dm.Naam);
                }
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
                BuildConflictMatrix();
            }
        }

        #endregion // TabItem Overrides

        #region Commands

        #endregion // Commands

        #region Command functionality

        #endregion // Command functionality

        #region Private methods

        private bool IsMatrixOK()
        {
            var s = Integrity.TLCGenIntegrityChecker.IsConflictMatrixOK(_Controller);
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
                Task.Factory.StartNew(() => MessageBox.Show(s, "Fout in conflictmatrix"));
                return false;
            }
        }

        #endregion // Private methods

        #region Public methods

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
                _model = null;
                RaisePropertyChanged("ConflictMatrix");
                return;
            }

            var fccount = Fasen.Count;

            _FasenNames = new ObservableCollection<string>();
            foreach (var fcvm in Fasen)
            {
                FasenNames.Add(fcvm.Naam);
            }
            RaisePropertyChanged("FasenNames");

            var model = new DataTableGridModel();
            model.DataSource = new DataTable("conflicts");
            for (var fcm_from = 0; fcm_from < fccount; ++fcm_from)
            {
                model.DataSource.Columns.Add(FasenNames[fcm_from]);
            }
            for (var fcm_from = 0; fcm_from < fccount; ++fcm_from)
            {
                var row = model.DataSource.NewRow();
                for (var fcm_to = 0; fcm_to < fccount; ++fcm_to)
                {
                    var c = _Controller.InterSignaalGroep.Conflicten.FirstOrDefault(x =>
                        x.FaseVan == _FasenNames[fcm_from] && x.FaseNaar == _FasenNames[fcm_to]);
                    if (c == null) row[fcm_to] = -1;
                    else row[fcm_to] = c.Waarde;
                }

                model.DataSource.Rows.Add(row);
            }

            Model = model;
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

        private void OnNameChanged(NameChangedMessage message)
        {
            if (Fasen.Any(x => x.Naam == message.NewName))
            {
                BuildConflictMatrix();
            }
        }

        private bool _IsProcessing = false;
        private DataTableGridModel _model1;

        private void OnProcesSynchornisationsRequested(ProcessSynchronisationsRequest request)
        {
            if (_IsProcessing)
                return;

            _IsProcessing = true;
            _IsProcessing = false;
        }

        private void OnIntergreenTimesTypeChanged(ControllerIntergreenTimesTypeChangedMessage msg)
        {
            RaisePropertyChanged(nameof(TijdenLabel));
            RaisePropertyChanged(nameof(GarantieTijdenLabel));
        }

        #endregion // TLCGen Event handling

        #region Constructor

        public SynchronisatiesTabViewModel2() : base()
        {
            MessengerInstance.Register(this, new Action<FasenChangedMessage>(OnFasenChanged));
            MessengerInstance.Register(this, new Action<FasenSortedMessage>(OnFasenSorted));
            MessengerInstance.Register(this, new Action<NameChangedMessage>(OnNameChanged));
            MessengerInstance.Register(this, new Action<ProcessSynchronisationsRequest>(OnProcesSynchornisationsRequested));
        }

        #endregion // Constructor
    }
}
