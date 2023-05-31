﻿namespace TLCGen.Generators.CCOL.CodeGeneration
{
    public enum CCOLCodeTypeEnum
    {
		// REG
        RegCBeforeIncludes,
        RegCIncludes,
        RegCTop,
        RegCKwcApplication,
        RegCPreApplication,
        RegCDetectieStoringAanvraag,
        RegCDetectieStoringMeetkriterium,
        RegCDetectieStoringVerlengGroen,
        RegCDetectieStoringMaxGroen,
        RegCKlokPerioden,
        RegCAanvragen,
        RegCBepaalRealisatieTijden,
        RegCBepaalInterStartGroenTijden,
        RegCVerlenggroen,
        RegCVerlenggroenNaAdd,
        RegCMaxgroen,
        RegCMaxgroenNaAdd,
        RegCWachtgroen,
        RegCMeetkriterium,
        RegCFileVerwerking,
        RegCMeeverlengen,
        RegCRealisatieAfhandelingVoorModules,
        RegCRealisatieAfhandelingModules,
        RegCRealisatieAfhandelingNaModules,
        RegCRealisatieAfhandeling,
        RegCAlternatieven,
        RegCSynchronisaties,
        RegCInitApplication,
        RegCApplication,
        RegCPostApplication,
        RegCPreSystemApplication,
        RegCSystemApplication,
        RegCSystemApplication2,
        RegCPostSystemApplication,
        RegCDumpApplication,
        RegCSpecialSignals,

        // PRIO
        PrioCIncludes,
        PrioCTop,
        PrioCInitPrio,
        PrioCInstellingen,
        PrioCRijTijdScenario,
        PrioCInUitMelden,
        PrioCOnderMaximum,
        PrioCAfkapGroen,
        PrioCStartGroenMomenten,
        PrioCAfkappen,
        PrioCTerugkomGroen,
        PrioCGroenVasthouden,
        PrioCMeetkriterium,
        PrioCPrioriteitsOpties,
        PrioCPrioriteitsNiveau,
        PrioCPrioriteitsToekenning,
        PrioCTegenhoudenConflicten,
        PrioCPostAfhandelingPrio,
        PrioCPARCorrecties,
        PrioCPARCcol,
        PrioCSpecialSignals,
        PrioCBottom,

        // TAB
        TabCBeforeIncludes,
	    TabCIncludes,
	    TabCTop,
        TabCControlDefaults,
	    TabCControlParameters,

        // SYS
        SysHDefines,
        SysHBeforeUserDefines,

        // HST
        HstCTop,
        HstCPostInitApplication,
		HstCPreApplication,
		HstCKlokPerioden,
		HstCAanvragen,
		HstCVerlenggroen,
		HstCMaxgroen,
		HstCWachtgroen,
		HstCMeetkriterium,
		HstCDetectieStoring,
		HstCMeeverlengen,
		HstCSynchronisaties,
		HstCAlternatief,
		HstCRealisatieAfhandeling,
		HstCPostApplication,
		HstCPreSystemApplication,
		HstCPostSystemApplication,
		HstCPostDumpApplication,
        HstCPrioHalfstarSettings
    };
}
