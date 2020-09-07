namespace TLCGen.Generators.Shared
{
    public class CCOLElement
    {
        public string Define { get; set; }
        public string Naam { get; set; }
        public CCOLElementTimeTypeEnum TType { get; set; }
        public int? Instelling { get; set; }
        public string Commentaar { get; set; }
        public CCOLElementTypeEnum Type { get; set; }
        public bool Dummy { get; set; }

        public CCOLElement()
        {
            Dummy = false;
        }

        public CCOLElement(string naam, CCOLElementTypeEnum type, ICCOLGeneratorSettingsProvider settingsProvider, string description = null)
        {
            Dummy = false;
            Naam = naam;
            Define = settingsProvider.GetPrefix(type) + naam;
            Type = type;
			Commentaar = description;
            TType = Type switch
            {
                CCOLElementTypeEnum.Schakelaar => CCOLElementTimeTypeEnum.SCH_type,
                _ => CCOLElementTimeTypeEnum.None
            };
        }

        public CCOLElement(string naam, int instelling, CCOLElementTimeTypeEnum ttype, CCOLElementTypeEnum type, ICCOLGeneratorSettingsProvider settingsProvider, string description = null)
        {
            Dummy = false;
            Naam = naam;
            Define = settingsProvider.GetPrefix(type) + naam;
            Instelling = instelling;
            TType = ttype;
            Type = type;
			Commentaar = description;
        }
    }
}
