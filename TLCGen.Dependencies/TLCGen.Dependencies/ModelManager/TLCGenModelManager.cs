﻿using GalaSoft.MvvmLight.Messaging;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Windows;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Serialization;
using TLCGen.Extensions;
using TLCGen.Helpers;
using TLCGen.Integrity;
using TLCGen.Messaging.Messages;
using TLCGen.Messaging.Requests;
using TLCGen.Models;
using TLCGen.Models.Enumerations;

namespace TLCGen.ModelManagement
{
    public class TLCGenModelManager : ITLCGenModelManager
    {
        #region Fields

        private static readonly object _Locker = new object();
        private static ITLCGenModelManager _Default;

        private IMessenger _MessengerInstance;
        private Action<object, string> _setDefaultsAction;

        private List<Tuple<string, object>> _pluginDataToMove = new List<Tuple<string, object>>();

        #endregion // Fields

        #region Properties

        public static ITLCGenModelManager Default
        {
            get
            {
                if (_Default == null)
                {
                    lock (_Locker)
                    {
                        if (_Default == null)
                        {
                            _Default = new TLCGenModelManager();
                        }
                    }
                }
                return _Default;
            }
        }

        private IMessenger MessengerInstance
        {
            get { return _MessengerInstance; }
            set { _MessengerInstance = value; }
        }

        public ControllerModel Controller
        {
            get; set;
        }

        #endregion // Properties

        #region Public Methods

        public void ConvertToIntergroen(ControllerModel controller)
        {
            foreach(var igt in controller.InterSignaalGroep.Conflicten)
            {
                var fc = controller.Fasen.FirstOrDefault(x => x.Naam == igt.FaseVan);
                if (fc == null) continue;
                igt.Waarde += fc.TGL;
                if (igt.GarantieWaarde.HasValue) igt.GarantieWaarde += fc.TGL;
            }
        }

        public void ConvertToOntruimingstijden(ControllerModel controller)
        {
            foreach (var igt in controller.InterSignaalGroep.Conflicten)
            {
                var fc = controller.Fasen.FirstOrDefault(x => x.Naam == igt.FaseVan);
                if (fc == null) continue;
                igt.Waarde -= fc.TGL;
                if (igt.Waarde < 0) igt.Waarde = 0;
                if (igt.GarantieWaarde.HasValue)
                {
                    igt.GarantieWaarde -= fc.TGL;
                    if (igt.GarantieWaarde < 0) igt.GarantieWaarde = 0;
                }
            }
        }

        public static void OverrideDefault(ITLCGenModelManager provider)
        {
            _Default = provider;
        }

	    public void InjectDefaultAction(Action<object, string> setDefaultsAction)
	    {
		    _setDefaultsAction = setDefaultsAction;
	    }

        public bool CheckVersionOrder(ControllerModel controller)
        {
            var vc = Version.Parse(string.IsNullOrWhiteSpace(controller.Data.TLCGenVersie) ? "0.0.0.0" : controller.Data.TLCGenVersie);
            var vp = Assembly.GetEntryAssembly().GetName().Version;
            if(vc > vp)
            {
                MessageBox.Show($"Dit bestand is gemaakt met een nieuwere versie van TLCGen,\n" +
                                $"en kan met deze versie niet worden geopend.\n\n" +
                                $"Versie TLCGen: {vp.ToString()}\n" +
                                $"Versie bestand: {vc.ToString()}", "Versies komen niet overeen");
                return false;
            }
            return true;
        }

        /// <summary>
        /// THis method is supposed to set properties on the model that are not saved, and
        /// are only used internally to ensure correct displaying of settings to the end user
        /// </summary>
        public void PrepareModelForUI(ControllerModel controller)
        {
            // set signalen bitmap items availability
            controller.Signalen.ControllerHasPeriodRtAanvraag = controller.PeriodenData.Perioden.Any(x => x.Type == PeriodeTypeEnum.RateltikkersAanvraag);
            controller.Signalen.ControllerHasPeriodRtAltijd = controller.PeriodenData.Perioden.Any(x => x.Type == PeriodeTypeEnum.RateltikkersAltijd);
            controller.Signalen.ControllerHasPeriodRtDimmen = controller.PeriodenData.Perioden.Any(x => x.Type == PeriodeTypeEnum.RateltikkersDimmen);
            controller.Signalen.ControllerHasPeriodBellenDimmen= controller.PeriodenData.Perioden.Any(x => x.Type == PeriodeTypeEnum.BellenDimmen);

            // sort if needed
            if (!controller.Detectoren.IsSorted()) controller.Detectoren.BubbleSort();
            if (!controller.SelectieveDetectoren.IsSorted()) controller.SelectieveDetectoren.BubbleSort();
        }
        
        public void CorrectModelByVersion(ControllerModel controller, string filename)
        {
            // correct segment items
            foreach(var s in controller.Data.SegmentenDisplayBitmapData)
            {
                if (s.Naam.StartsWith("segm"))
                {
                    s.Naam = s.Naam.Replace("segm", "");
                }
            }

            if (string.IsNullOrWhiteSpace(controller.ModuleMolen.Reeks))
            {
                controller.ModuleMolen.Reeks = "ML";
            }

            foreach (var fcm in controller.Fasen)
            {
                if (!fcm.Detectoren.IsSorted())
                {
                    fcm.Detectoren.BubbleSort();
                    MessengerInstance.Send(new DetectorenChangedMessage(controller, null, null));
                }
            }

            // check PostAfhandelingOV_Add in ov.add
            if (filename != null)
            {
                var ovAddFile = Path.Combine(Path.GetDirectoryName(filename), controller.Data.Naam + "ov.add");
                if (File.Exists(ovAddFile))
                {
                    var ovaddtext = File.ReadAllLines(ovAddFile);
                    if (ovaddtext.All(x => !Regex.IsMatch(x, @"^\s*void\s+PostAfhandelingOV_Add.*")))
                    {
                        MessageBox.Show($"Let op! Deze versie van TLCGen maakt een functie\n" +
                                    $"'PostAfhandelingOV' aan in bestand {controller.Data.Naam}ov.c. Hierin wordt\n" +
                                    $"de functie 'PostAfhandelingOV_Add' aangeroepen, die echter\n" +
                                    $"ontbreekt in bestand {controller.Data.Naam}ov.add.", "Functie PostAfhandelingOV_Add ontbreekt.\n\n" +
                                    "Voeg deze dus toe, waarschijnlijk in plaats van 'void post_AfhandelingOV'," +
                                    "want die wordt niet aangeroepen.");
                    }
                }
            }

            var v = Version.Parse(string.IsNullOrWhiteSpace(controller.Data.TLCGenVersie) ? "0.0.0.0" : controller.Data.TLCGenVersie);
            
            // In version 0.2.2.0, the OVIngreepModel object was changed
            var checkVer = Version.Parse("0.2.2.0");
            if(v < checkVer)
            {
                bool vecom = false;
                foreach (var ov in controller.OVData.OVIngrepen)
                {
                    if (ov.KAR)
                    {
                        ov.MeldingenData.Inmeldingen.Add(new OVIngreepInUitMeldingModel
                        {
                            AlleenIndienGeenInmelding = false,
                            AntiJutterTijd = 15,
                            AntiJutterTijdToepassen = true,
                            InUit = OVIngreepInUitMeldingTypeEnum.Inmelding,
                            KijkNaarWisselStand = false,
                            OpvangStoring = false,
                            Type = OVIngreepInUitMeldingVoorwaardeTypeEnum.KARMelding
                        });
                        ov.MeldingenData.Uitmeldingen.Add(new OVIngreepInUitMeldingModel
                        {
                            AlleenIndienGeenInmelding = false,
                            AntiJutterTijd = 15,
                            AntiJutterTijdToepassen = true,
                            InUit = OVIngreepInUitMeldingTypeEnum.Uitmelding,
                            KijkNaarWisselStand = false,
                            OpvangStoring = false,
                            Type = OVIngreepInUitMeldingVoorwaardeTypeEnum.KARMelding
                        });
                    }
                    if(ov.Vecom && !vecom)
                    {
                        vecom = true;
                        MessageBox.Show("Dit is oud type TLCGen bestand. OV via VECOM moet opnieuw worden opgegeven.", "VECOM opnieuw invoeren.", MessageBoxButton.OK);
                    }
                }
            }

            // In version 0.2.3.0, handling of segments was altered.
            checkVer = Version.Parse("0.2.3.0");
            if(v < checkVer)
            {
                foreach (var s in controller.Data.SegmentenDisplayBitmapData)
                {
                    if (s.Naam.StartsWith("segm"))
                    {
                        s.Naam = s.Naam.Replace("segm", "");
                    }
                }
            }

            // In version 0.3.5.0, voetgangers got lanes.
            checkVer = Version.Parse("0.3.5.0");
            if (v < checkVer)
            {
                foreach (var s in controller.Fasen)
                {
                    if (!s.AantalRijstroken.HasValue || s.Type == FaseTypeEnum.Voetganger && s.AantalRijstroken.Value == 1)
                    {
                        switch (s.Type)
                        {
                            case FaseTypeEnum.Voetganger:
                                s.AantalRijstroken = 2;
                                break;
                            default:
                                s.AantalRijstroken = 1;
                                break;
                        }
                    }
                }
            }

            // Version 0.5.4.0
            // Sort controller.HalfstarData.FaseCyclusInstellingen after renaming
            // Set file ingrepen doseren to default 'Doseren toepassen'
            checkVer = Version.Parse("0.5.4.0");
            if (v < checkVer)
            {
                controller.HalfstarData.FaseCyclusInstellingen.BubbleSort();
                foreach (var fm in controller.FileIngrepen) fm.ToepassenDoseren = NooitAltijdAanUitEnum.Altijd;
            }

            if(_pluginDataToMove.Any())
            {
                var risData = _pluginDataToMove.FirstOrDefault(x => x.Item1 == "RISData")?.Item2;
                if (risData != null && (controller.RISData == null || controller.RISData.RISFasen.Count == 0)) controller.RISData = (RISDataModel)risData;
                var dhData = _pluginDataToMove.FirstOrDefault(x => x.Item1 == "SpecialsDenHaagData")?.Item2;
                if (dhData != null && (controller.AlternatievenPerBlokData == null || controller.AlternatievenPerBlokData.AlternatievenPerBlok?.Count == 0)) controller.AlternatievenPerBlokData = (AlternatievenPerBlokModel)risData;
                var rtdData = (Dictionary<string, bool>)_pluginDataToMove.FirstOrDefault(x => x.Item1 == "SpecialsRotterdamData")?.Item2;
                if (rtdData.ContainsKey("ToevoegenOVM")) controller.Data.ToevoegenOVM = rtdData["ToevoegenOVM"];
                if (rtdData.ContainsKey("PrmLoggingTfbMax")) controller.Data.PrmLoggingTfbMax = rtdData["PrmLoggingTfbMax"];
            }
        }

        private static void RenameXMLNode(XmlDocument doc, XmlNode oldRoot, string newname)
        {
            XmlNode newRoot = doc.CreateElement(newname);

            foreach (XmlNode childNode in oldRoot.ChildNodes)
            {
                newRoot.AppendChild(childNode.CloneNode(true));
            }
            XmlNode parent = oldRoot.ParentNode;
            parent.AppendChild(newRoot);
            parent.RemoveChild(oldRoot);
        }

        public void CorrectXmlDocumentByVersion(XmlDocument doc)
        {
            // get version
            var vi = doc.SelectSingleNode("//Data//TLCGenVersie");
            var v = Version.Parse(vi.InnerText);

            var checkVer = Version.Parse("0.5.4.0");
            if (v < checkVer)
            {
                // V0.5.4.0: RIS plugin moved inside TLCGen
                foreach (XmlNode node in doc.FirstChild.ChildNodes)
                {
                    if (node.LocalName == "RISData")
                    {
                        var risModel = XmlNodeConverter.ConvertNode<RISDataModel>(node);
                        _pluginDataToMove.Add(new Tuple<string, object>("RISData", risModel));
                        break;
                    }
                }

                // V0.5.4.0: property KruisingNaam on PelotonKoppelingModel changed to KoppelingNaam
                var item = doc.SelectSingleNode("//PelotonKoppelingenData//PelotonKoppelingen");
                if (item != null)
                {
                    var rowList = item.ChildNodes;
                    foreach (XmlNode n in rowList)
                    {
                        var c = n.SelectSingleNode("KruisingNaam");
                        if (c != null)
                        {
                            RenameXMLNode(doc, c, "KoppelingNaam");
                        }
                    }
                }

                // V0.5.4.0: property HalfstarFaseCyclusAlternatiefModel in HalfstarDataModel changed to HalfstarFaseCyclusInstellingenModel
                item = doc.SelectSingleNode("//HalfstarData//Alternatieven");
                if (item != null)
                {
                    var rowList = item.ChildNodes;
                    var newNodes = new List<XmlNode>();
                    for (var i = 0; i < rowList.Count; ++i)
                    {
                        XmlNode newRoot = doc.CreateElement("HalfstarFaseCyclusInstellingenModel");
                        foreach (XmlNode childNode in rowList[i].ChildNodes)
                        {
                            newRoot.AppendChild(childNode.CloneNode(true));
                        }
                        newNodes.Add(newRoot);
                    }
                    item.RemoveAll();
                    for (var i = 0; i < newNodes.Count; ++i)
                    {
                        item.AppendChild(newNodes[i]);
                    }
                    RenameXMLNode(doc, item, "FaseCyclusInstellingen");
                }
            }
            checkVer = Version.Parse("0.6.2.0");
            if (v < checkVer)
            {
                // V0.6.2.0: Den Haag specials plugin merged with TLCGen core
                foreach (XmlNode node in doc.FirstChild.ChildNodes)
                {
                    if (node.LocalName == "SpecialsDenHaag")
                    {
                        var data = @"<AlternatievenPerBlokModel>" + node.InnerXml + @"</AlternatievenPerBlokModel>";
                        using (TextReader reader = new StringReader(data))
                        {
                            XmlSerializer xs = new XmlSerializer(typeof(AlternatievenPerBlokModel));
                            var dhModel = xs.Deserialize(reader);
                            _pluginDataToMove.Add(new Tuple<string, object>("SpecialsDenHaagData", dhModel));
                        }
                        break;
                    }
                }
                // V0.6.2.0: Rotterdam specials plugin merged with TLCGen core
                foreach (XmlNode node in doc.FirstChild.ChildNodes)
                {
                    if (node.LocalName == "SpecialsRotterdam")
                    {
                        var dataRdam = new Dictionary<string, bool>();
                        foreach (XmlNode n in node.ChildNodes)
                        {
                            if (n.Name == "ToevoegenOVM") dataRdam.Add(n.Name, n.InnerText.ToLower() == "true");
                            if (n.Name == "PrmLoggingTfbMax") dataRdam.Add(n.Name, n.InnerText.ToLower() == "true");
                        }
                        _pluginDataToMove.Add(new Tuple<string, object>("SpecialsRotterdamData", dataRdam));
                        break;
                    }
                }
            }
        }

        public bool IsElementIdentifierUnique(TLCGenObjectTypeEnum objectType, string identifier, bool vissim = false)
        {
            if(!vissim) return TLCGenIntegrityChecker.IsElementNaamUnique(Controller, identifier, objectType);
            return TLCGenIntegrityChecker.IsElementVissimNaamUnique(Controller, identifier);
        }

        #endregion // Public Methods

        #region TLCGen Messaging

        public void OnFasenChanging(FasenChangingMessage message)
        {
            if (message.AddedFasen != null)
            {
                foreach (var fcm in message.AddedFasen)
                {
                    // PT Conflict prms
                    if (Controller.OVData.OVIngreepType != Models.Enumerations.OVIngreepTypeEnum.Geen)
                    {
                        var prms = new OVIngreepSignaalGroepParametersModel();
                        _setDefaultsAction?.Invoke(prms, null);
                        prms.FaseCyclus = fcm.Naam;
                        Controller.OVData.OVIngreepSignaalGroepParameters.Add(prms);
                    }

                    // Module settings
                    var fcmlm = new FaseCyclusModuleDataModel() { FaseCyclus = fcm.Naam };
	                _setDefaultsAction?.Invoke(fcmlm, null);
                    Controller.ModuleMolen.FasenModuleData.Add(fcmlm);

                    // Green times
                    foreach (var set in Controller.GroentijdenSets)
                    {
                        var mgm = new GroentijdModel { FaseCyclus = fcm.Naam };
                        _setDefaultsAction(mgm, fcm.Type.ToString());
                        set.Groentijden.Add(mgm);
                    }
                }
            }
            if (message.RemovedFasen != null)
            {
                foreach (var fcm in message.RemovedFasen)
                {
                    // PT Conflict prms
                    if (Controller.OVData.OVIngreepType != Models.Enumerations.OVIngreepTypeEnum.Geen)
                    {
                        OVIngreepSignaalGroepParametersModel _prms = null;
                        foreach (var prms in Controller.OVData.OVIngreepSignaalGroepParameters)
                        {
                            if (prms.FaseCyclus == fcm.Naam)
                            {
                                _prms = prms;
                            }
                        }
                        if (_prms != null)
                        {
                            Controller.OVData.OVIngreepSignaalGroepParameters.Remove(_prms);
                        }
                    }

                    // Module settings
                    FaseCyclusModuleDataModel fcvm = null;
                    foreach(var f in Controller.ModuleMolen.FasenModuleData)
                    {
                        if(fcm.Naam == f.FaseCyclus)
                        {
                            fcvm = f;
                        }
                    }
                    if (fcvm != null)
                    {
                        Controller.ModuleMolen.FasenModuleData.Remove(fcvm);
                    }

                    // Green times
                    foreach (var set in Controller.GroentijdenSets)
                    {
                        GroentijdModel mgm = null;
                        foreach (var mgvm in set.Groentijden)
                        {
                            if (mgvm.FaseCyclus == fcm.Naam)
                            {
                                mgm = mgvm;
                            }
                        }
                        if (mgm != null)
                        {
                            set.Groentijden.Remove(mgm);
                        }
                    }
                }
            }

            // Sorting
            Controller.OVData.OVIngreepSignaalGroepParameters.BubbleSort();
            foreach (var set in Controller.GroentijdenSets)
            {
                set.Groentijden.BubbleSort();
            }
            Controller.ModuleMolen.FasenModuleData.BubbleSort();

            // Messaging
            MessengerInstance.Send(new FasenChangedMessage(message.AddedFasen, message.RemovedFasen));
        }

        private void OnNameChanging(NameChangingMessage msg)
        {
            ChangeNameOnObject(Controller, msg.OldName, msg.NewName, msg.ObjectType);
            MessengerInstance.Send(new NameChangedMessage(msg.ObjectType, msg.OldName, msg.NewName));
        }

        public int ChangeNameOnObject(object obj, string oldName, string newName, TLCGenObjectTypeEnum objectType)
        {
            var i = 0;
            if (obj == null) return i;
            Type objType = obj.GetType();

            // class refers to?
            var refToAttr = objType.GetCustomAttribute<RefersToAttribute>();

            PropertyInfo[] properties = objType.GetProperties();
            foreach (PropertyInfo property in properties)
            {
                var ignore = (TLCGenIgnoreAttributeAttribute)property.GetCustomAttribute(typeof(TLCGenIgnoreAttributeAttribute));
                if (ignore != null) continue;

                object propValue = property.GetValue(obj);

                // for strings
                if (property.PropertyType == typeof(string))
                {
                    // if this is the referent string, set it if needed
                    if (refToAttr != null &&
                        (property.Name == refToAttr.ReferProperty1 && objectType == refToAttr.ObjectType1 || 
                         property.Name == refToAttr.ReferProperty2 && objectType == refToAttr.ObjectType2 ||
                         property.Name == refToAttr.ReferProperty3 && objectType == refToAttr.ObjectType3))
                    {
                        if ((string)propValue == oldName)
                        {
                            property.SetValue(obj, newName);
                            ++i;
                        }
                    }
                    // otherwise, check if the string has RefersTo itself, and set if needed
                    else
                    {
                        var strRefToAttr = property.GetCustomAttribute<RefersToAttribute>();
                        if (strRefToAttr != null &&
                            (objectType == strRefToAttr.ObjectType1 ||
                             objectType == strRefToAttr.ObjectType2 ||
                             objectType == strRefToAttr.ObjectType3))
                        {
                            if ((string)propValue == oldName)
                            {
                                property.SetValue(obj, newName);
                                ++i;
                            }
                        }
                    }
                }
                // for lists
                else if (propValue is IList elems)
                {
                    foreach (var item in elems)
                    {
                        ChangeNameOnObject(item, oldName, newName, objectType);
                    }
                }
                // for objects
                else if(!property.PropertyType.IsValueType)
                {
                    ChangeNameOnObject(propValue, oldName, newName, objectType);
                }
            }
            return i;
        }

        public void OnModelManagerMessage(ModelManagerMessageBase msg)
        {
            PrepareModelForUI(Controller);
            switch (msg)
            {
                case OVIngreepMeldingChangedMessage meldingMsg:
                    var ovi = Controller.OVData.OVIngrepen.FirstOrDefault(x => x.FaseCyclus == meldingMsg.FaseCyclus);
                    if (ovi != null && meldingMsg.MeldingType == OVIngreepInUitMeldingVoorwaardeTypeEnum.KARMelding)
                    {
                        var karMelding = ovi.MeldingenData.Inmeldingen.FirstOrDefault(x => x.Type == OVIngreepInUitMeldingVoorwaardeTypeEnum.KARMelding);

                        if (karMelding != null && ovi.DummyKARInmelding == null)
                        {
                            ovi.DummyKARInmelding = new DetectorModel()
                            {
                                Dummy = true,
                                Naam = "dummykarin" + ovi.FaseCyclus
                            };
                        }
                        else if (karMelding == null && ovi.DummyKARInmelding != null)
                        {
                            ovi.DummyKARInmelding = null;
                        }

                        karMelding = ovi.MeldingenData.Uitmeldingen.FirstOrDefault(x => x.Type == OVIngreepInUitMeldingVoorwaardeTypeEnum.KARMelding);

                        if (karMelding != null && ovi.DummyKARUitmelding == null)
                        {
                            ovi.DummyKARUitmelding = new DetectorModel()
                            {
                                Dummy = true,
                                Naam = "dummykaruit" + ovi.FaseCyclus
                            };
                        }
                        else if (karMelding == null && ovi.DummyKARUitmelding != null)
                        {
                            ovi.DummyKARUitmelding = null;
                        }
                    }
                    break;
                case ModulesChangedMessage modulesMessage:
                    if (Controller.Data.UitgangPerModule)
                    {
                        if (!Controller.Data.MultiModuleReeksen)
                        {
                            foreach (var m in Controller.ModuleMolen.Modules)
                            {
                                if (!Controller.Data.ModulenDisplayBitmapData.Any(x => x.Naam == m.Naam))
                                {
                                    Controller.Data.ModulenDisplayBitmapData.Add(new ModuleDisplayElementModel
                                    {
                                        Naam = m.Naam
                                    });
                                    Controller.Data.ModulenDisplayBitmapData.BubbleSort();
                                }
                            }
                        }
                        else
                        {
                            foreach (var m in Controller.MultiModuleMolens.SelectMany(x => x.Modules))
                            {
                                if (!Controller.Data.ModulenDisplayBitmapData.Any(x => x.Naam == m.Naam))
                                {
                                    Controller.Data.ModulenDisplayBitmapData.Add(new ModuleDisplayElementModel
                                    {
                                        Naam = m.Naam
                                    });
                                    Controller.Data.ModulenDisplayBitmapData.BubbleSort();
                                }
                            }
                        }
                        var rd = new List<ModuleDisplayElementModel>();
                        foreach (var md in Controller.Data.ModulenDisplayBitmapData)
                        {
                            if (!Controller.Data.MultiModuleReeksen)
                            {
                                if (!Controller.ModuleMolen.Modules.Any(x => x.Naam == md.Naam))
                                {
                                    rd.Add(md);
                                }
                            }
                            else
                            {
                                if (!Controller.MultiModuleMolens.SelectMany(x => x.Modules).Any(x => x.Naam == md.Naam))
                                {
                                    rd.Add(md);
                                }
                            }
                        }
                        foreach (var r in rd)
                        {
                            Controller.Data.ModulenDisplayBitmapData.Remove(r);
                        }
                    }
                    break;

            }
        }

        private void OnPrepareForGenerationRequest(PrepareForGenerationRequest msg)
        {
            foreach (var fcm in msg.Controller.Fasen)
            {
                fcm.Detectoren.BubbleSort();
            }
            msg.Controller.Detectoren.BubbleSort();
            msg.Controller.SelectieveDetectoren.BubbleSort();
        }

        private void OnDetectorenChangedMessage(DetectorenChangedMessage msg)
        {
            if (msg.RemovedDetectoren != null && msg.RemovedDetectoren.Any())
            {
                foreach (var d in msg.RemovedDetectoren)
                {
                    if (d.Type == DetectorTypeEnum.OpticomIngang)
                    {
                        foreach (var hd in msg.Controller.OVData.HDIngrepen)
                        {
                            if (hd.Opticom && hd.OpticomRelatedInput == d.Naam)
                            {
                                hd.Opticom = false;
                                hd.OpticomRelatedInput = null;
                            }
                        }
                    }
                }
            }
        }

        private void OnFaseDetectorTypeChangedMessage(FaseDetectorTypeChangedMessage msg)
        {
            if(msg.OldType == DetectorTypeEnum.OpticomIngang && msg.NewType != DetectorTypeEnum.OpticomIngang)
            {
                foreach (var hd in msg.Controller.OVData.HDIngrepen)
                {
                    if (hd.Opticom && hd.OpticomRelatedInput == msg.DetectorDefine)
                    {
                        hd.Opticom = false;
                        hd.OpticomRelatedInput = null;
                    }
                }
            }
        }

        #endregion // TLCGen Messaging

        #region Constructor

        public TLCGenModelManager(IMessenger messengerinstance = null)
        {
            if(messengerinstance == null)
            {
                MessengerInstance = Messenger.Default;
            }
            MessengerInstance.Register(this, new Action<FasenChangingMessage>(OnFasenChanging));
            MessengerInstance.Register(this, new Action<NameChangingMessage>(OnNameChanging));
            MessengerInstance.Register(this, true, new Action<ModelManagerMessageBase>(OnModelManagerMessage));
            MessengerInstance.Register(this, new Action<PrepareForGenerationRequest>(OnPrepareForGenerationRequest));
            MessengerInstance.Register(this, new Action<DetectorenChangedMessage>(OnDetectorenChangedMessage));
            MessengerInstance.Register(this, new Action<FaseDetectorTypeChangedMessage>(OnFaseDetectorTypeChangedMessage));
        }

        #endregion // Constructor
    }
}
