using System.Text;
using TLCGen.Extensions;
using TLCGen.Models;
using TLCGen.Generators.Shared;

namespace TLCGen.Generators.CCOL.CodeGeneration
{
    public partial class CCOLGenerator
    {
        /// <summary>
        /// Generates a file header
        /// </summary>
        /// <param name="data">The ControllerDataModel instance that holds the info for generation</param>
        /// <param name="fileappend">The string to append to the Controller name to get the file name.</param>
        /// <returns>A string holding the file header</returns>
        private string GenerateFileHeader(ControllerDataModel data, string fileappend)
        {
            return CCOLHeaderGenerator.GenerateFileHeader(data, fileappend, "0.8.0.0");
        }

        /// <summary>
        /// Generates a string with version info
        /// </summary>
        /// <param name="data">The ControllerDataModel instance that hold the version info</param>
        /// <returns>A string with version information</returns>
        private string GenerateVersionHeader(ControllerDataModel data)
        {
            return CCOLHeaderGenerator.GenerateVersionHeader(data);
        }   
    }
}
