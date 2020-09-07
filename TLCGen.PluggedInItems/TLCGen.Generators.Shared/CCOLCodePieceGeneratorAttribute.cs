namespace TLCGen.Generators.Shared
{
    [System.AttributeUsage(System.AttributeTargets.Class)]
    public class CCOLCodePieceGeneratorAttribute : System.Attribute
    {
        /// <summary>
        /// Property Attribute to indicate a property is meant as setting for a
        /// class that implements ICCOLCodePieceGenerator.
        /// </summary>
        public CCOLCodePieceGeneratorAttribute()
        {
        }
    }

    [System.AttributeUsage(System.AttributeTargets.Class)]
    public class TraffickCodePieceGeneratorAttribute : System.Attribute
    {
        /// <summary>
        /// Property Attribute to indicate a property is meant as setting for a
        /// class that implements ICCOLCodePieceGenerator, but only used for Traffick gen
        /// </summary>
        public TraffickCodePieceGeneratorAttribute()
        {
        }
    }
}
