namespace TLCGen.Generators.Shared
{
    public interface ICCOLGeneratorSettingsProvider
    {
        void Reset();
        string GetPrefix(string type);
        string GetDefaultPrefix(string pfdefault);
        string GetPrefix(CCOLElementTypeEnum type);
        string GetPrefix(CCOLGeneratorSettingTypeEnum type);
        string GetDefaultPrefix(CCOLGeneratorSettingTypeEnum type);

        CCOLElement CreateElement(string name, int setting, CCOLElementTimeTypeEnum timeType, CCOLGeneratorCodeStringSettingModel element, params string [] elementnames);
        CCOLElement CreateElement(string name, CCOLGeneratorCodeStringSettingModel element, params string[] elementnames);
        CCOLElement CreateElement(string name, CCOLElementTypeEnum type, string description);
        CCOLElement CreateElement(string name, int setting, CCOLElementTimeTypeEnum timeType, CCOLElementTypeEnum type, string description);
        string GetElementName(string defaultwithprefix);
        string GetElementDescription(string description, CCOLElementTypeEnum type, params string [] elementnames);

        CCOLGeneratorSettingsModel Settings { get; set; }
    }
}