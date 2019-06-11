﻿using GalaSoft.MvvmLight.CommandWpf;
using GalaSoft.MvvmLight.Messaging;
using Microsoft.Win32;
using System;
using System.Collections.ObjectModel;
using System.Text.RegularExpressions;
using System.Windows.Input;
using TLCGen.Helpers;
using TLCGen.Messaging.Messages;
using TLCGen.Models;
using TLCGen.Plugins;
using RelayCommand = GalaSoft.MvvmLight.CommandWpf.RelayCommand;

namespace TLCGen.ViewModels
{
    [TLCGenTabItem(index: 2, type: TabItemTypeEnum.AlgemeenTab)]
    public class VersieBeheerTabViewModel : TLCGenTabItemViewModel
    {
        #region Fields

        private VersieViewModel _SelectedVersie;

        #endregion // Fields

        #region Properties

        public VersieViewModel SelectedVersie
        {
            get { return _SelectedVersie; }
            set
            {
                _SelectedVersie = value;
                RaisePropertyChanged("SelectedVersie");
            }
        }

        private ObservableCollection<VersieViewModel> _Versies;
        public ObservableCollection<VersieViewModel> Versies
        {
            get
            {
                if (_Versies == null)
                {
                    _Versies = new ObservableCollection<VersieViewModel>();
                }
                return _Versies;
            }
        }

        public int HuidigeVersieMajor
        {
            get => _Controller?.Data?.HuidigeVersieMajor ?? 0;
            set
            {
                _Controller.Data.HuidigeVersieMajor = value;
                RaisePropertyChanged<object>("HuidigeVersieMajor", broadcast: true);
            }
        }

        public int HuidigeVersieMinor
        {
            get => _Controller?.Data?.HuidigeVersieMinor ?? 0;
            set
            {
                _Controller.Data.HuidigeVersieMinor = value;
                RaisePropertyChanged<object>("HuidigeVersieMinor", broadcast: true);
            }
        }

        public int HuidigeVersieRevision
        {
            get => _Controller?.Data?.HuidigeVersieRevision ?? 0;
            set
            {
                _Controller.Data.HuidigeVersieRevision = value;
                RaisePropertyChanged<object>("HuidigeVersieRevision", broadcast: true);
            }
        }

        public bool AanmakenVerionSysh
        {
            get => _Controller?.Data?.AanmakenVerionSysh ?? false;
            set
            {
                _Controller.Data.AanmakenVerionSysh = value;
                RaisePropertyChanged<object>("AanmakenVerionSysh", broadcast: true);
            }
        }

        public bool StoreCurrentController
        {
            get => _Controller?.Data?.StoreCurrentController ?? false;
            set
            {
                _Controller.Data.StoreCurrentController = value;
                RaisePropertyChanged<object>("StoreCurrentController", broadcast: true);
            }
        }

        #endregion // Properties

        #region Commands

        RelayCommand _AddVersieCommand;
        public ICommand AddVersieCommand
        {
            get
            {
                if (_AddVersieCommand == null)
                {
                    _AddVersieCommand = new RelayCommand(AddVersieCommand_Executed, AddVersieCommand_CanExecute);
                }
                return _AddVersieCommand;
            }
        }

        RelayCommand _RemoveVersieCommand;
        public ICommand RemoveVersieCommand
        {
            get
            {
                if (_RemoveVersieCommand == null)
                {
                    _RemoveVersieCommand = new RelayCommand(RemoveVersieCommand_Executed, RemoveVersieCommand_CanExecute);
                }
                return _RemoveVersieCommand;
            }
        }

        RelayCommand _RestoreVersieCommand;
        public ICommand RestoreVersieCommand
        {
            get
            {
                if (_RestoreVersieCommand == null)
                {
                    _RestoreVersieCommand = new RelayCommand(RestoreVersieCommand_Executed, RestoreVersieCommand_CanExecute);
                }
                return _RestoreVersieCommand;
            }
        }

        #endregion // Commands

        #region Command Functionality

        void AddVersieCommand_Executed()
        {
            var vm = new VersieModel
            {
                Datum = DateTime.Now
            };
            string nextver = null;
            int nextmajor = 1, nextminor = 0;
            if (Versies != null && Versies.Count > 0)
            {
                var m = Regex.Match(Versies[Versies.Count - 1].Versie, @"([0-9]+)\.([0-9]+)\.([0-9]+)");
                if (m.Groups.Count == 4)
                {
                    var majver = m.Groups[1].Value;
                    var midver = m.Groups[2].Value;
                    if (int.TryParse(majver, out int nextmajver))
                    {
                        nextmajor = nextmajver;
                    }
                    if (int.TryParse(midver, out int nextmidver))
                    {
                        nextminor = nextmidver + 1;
                        nextver = m.Groups[1].Value + "." + (nextmidver + 1).ToString() + ".0";
                    }
                }
            }
            HuidigeVersieMajor = nextmajor;
            HuidigeVersieMinor = nextminor;
            HuidigeVersieRevision = 0;
            vm.Versie = nextver ?? "1.0.0";
            vm.Ontwerper = Environment.UserName;
            if (StoreCurrentController)
            {
                var controller = DeepCloner.DeepClone(Controller);
                controller.Data.Versies.Clear();
                vm.Controller = controller;
            }
            var vvm = new VersieViewModel(vm);
            Versies?.Add(vvm);
        }

        bool AddVersieCommand_CanExecute()
        {
            return Versies != null;
        }

        void RemoveVersieCommand_Executed()
        {
            Versies.Remove(SelectedVersie);
            SelectedVersie = null;
        }

        bool RemoveVersieCommand_CanExecute()
        {
            return Versies != null && Versies.Count > 0 && SelectedVersie != null;
        }

        void RestoreVersieCommand_Executed()
        {
            var c = DeepCloner.DeepClone(SelectedVersie.VersieEntry.Controller);

            var ve = DeepCloner.DeepClone(SelectedVersie.VersieEntry);
            ve.Controller = null;
            c.Data.Versies.Add(ve);

            var dlg = new SaveFileDialog();
            dlg.Filter = "TLCGen files|*.tlc|TLCGen gzipped files|*.tlcgz";
            dlg.CheckFileExists = false;
            dlg.CheckPathExists = true;
            dlg.FileName = c.Data.Naam + "_" + SelectedVersie.Versie + ".tlc";
            dlg.DefaultExt = ".tlc";
            if(!string.IsNullOrWhiteSpace(DataAccess.TLCGenControllerDataProvider.Default.ControllerFileName))
            {
                dlg.InitialDirectory = System.IO.Path.GetDirectoryName(DataAccess.TLCGenControllerDataProvider.Default.ControllerFileName);
            }
            dlg.Title = "Selecteer het bestand om de regeling in op te slaan";
            if (dlg.ShowDialog() == true)
            {
                try
                {
                    if (dlg.FileName.EndsWith(".tlcgz"))
                    {
                        TLCGenSerialization.SerializeGZip(dlg.FileName, c);
                    }
                    else if (dlg.FileName.EndsWith(".tlc"))
                    {
                        TLCGenSerialization.Serialize(dlg.FileName, c);
                    }
                }
                catch (Exception e)
                {
                    Dependencies.Providers.TLCGenDialogProvider.Default.ShowMessageBox("Fout bij terugzetten van regeling:\n\n" + e.ToString(), "Fout bij opslaan", System.Windows.MessageBoxButton.OK);
                }
                Dependencies.Providers.TLCGenDialogProvider.Default.ShowMessageBox($"Versie {SelectedVersie.Versie} is hier opgeslagen:{dlg.FileName}\n\nLET OP! Dit bestand bevat uitsluitend de data van die versie, zonder data van plugins.", $"Data van versie {SelectedVersie.Versie} opgeslagen", System.Windows.MessageBoxButton.OK);
            }
        }

        bool RestoreVersieCommand_CanExecute()
        {
            return Versies != null && Versies.Count > 0 && SelectedVersie != null && SelectedVersie.VersieEntry.Controller != null;
        }

        #endregion // Command Functionality

        #region TabItem Overrides

        public override string DisplayName
        {
            get
            {
                return "Versiebeheer";
            }
        }

        public override bool IsEnabled
        {
            get { return true; }
            set { }
        }

        public override void OnSelected()
        {
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
                if (base.Controller != null)
                {
                    Versies.CollectionChanged -= Versies_CollectionChanged;
                    Versies.Clear();
                    foreach (VersieModel vm in _Controller.Data.Versies)
                    {
                        VersieViewModel vvm = new VersieViewModel(vm);
                        Versies.Add(vvm);
                    }
                    Versies.CollectionChanged += Versies_CollectionChanged;
                    RaisePropertyChanged("");
                }
                else
                {
                    Versies.CollectionChanged -= Versies_CollectionChanged;
                    Versies.Clear();
                    RaisePropertyChanged("");
                }
            }
        }

        #endregion // TabItem Overrides

        #region Collection Changed

        private void Versies_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (e.NewItems != null && e.NewItems.Count > 0)
            {
                foreach (VersieViewModel vvm in e.NewItems)
                {
                    _Controller.Data.Versies.Add(vvm.VersieEntry);
                }
            }
            if (e.OldItems != null && e.OldItems.Count > 0)
            {
                foreach (VersieViewModel vvm in e.OldItems)
                {
                    _Controller.Data.Versies.Remove(vvm.VersieEntry);
                }
            }
            Messenger.Default.Send(new ControllerDataChangedMessage());
        }

        #endregion // Collection Changed

        #region Constructor

        public VersieBeheerTabViewModel() : base()
        {
        }

        #endregion // Constructor
    }
}
