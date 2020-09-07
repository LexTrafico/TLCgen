using System.Text;
using TLCGen.Extensions;
using TLCGen.Models;

namespace TLCGen.Generators.Shared
{
    public static class CCOLHeaderGenerator
    {
        public static string GenerateFileHeader(ControllerDataModel data, string fileappend, string pluginVersion)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"/* KRUISPUNT: {data.Stad}");
            sb.Append(' ', 14);
            sb.Append(data.Naam);
            if (!string.IsNullOrWhiteSpace(data.Nummer) &&
                !string.Equals(data.Naam, data.Nummer))
            {
                sb.Append($" ({data.Nummer})");
            }
            sb.AppendLine();
            sb.Append(' ', 14);
            sb.AppendLine(data.Straat1);
            sb.Append(' ', 14);
            sb.AppendLine(data.Straat2);
            sb.AppendLine();

            sb.AppendLine($"   BESTAND:   {data.Naam}{fileappend}");
            sb.AppendLine($"      CCOL:   {data.CCOLVersie.GetDescription()}");
            sb.AppendLine($"    TLCGEN:   {data.TLCGenVersie}");
            sb.AppendLine($"   CCOLGEN:   {pluginVersion}");
            sb.AppendLine("*/");

            return sb.ToString();
        }

        public static string GenerateVersionHeader(ControllerDataModel data)
        {
            // Set up: variables
            var sb = new StringBuilder();
            var Distance = 3;
            const string verString = "Versie";
            const string datString = "Datum";
            const string ontString = "Ontwerper";
            const string comString = "Commentaar";
            var verLength = verString.Length + Distance;
            var datLength = datString.Length + Distance;
            var ontLength = ontString.Length + Distance;

            // Add top
            sb.AppendLine("/****************************** Versie commentaar ***********************************");
            sb.AppendLine(" *");

            // Determine 
            foreach (var vm in data.Versies)
            {
                if ((vm.Versie.Length + Distance) > verLength) verLength = vm.Versie.Length + Distance;
                if ((vm.Datum.ToString("dd-MM-yyyy").Length + Distance) > datLength) datLength = vm.Datum.ToString("dd-MM-yyyy").Length + Distance;
                if ((vm.Ontwerper.Length + Distance) > ontLength) ontLength = vm.Ontwerper.Length + Distance;
            }

            // Add title line
            sb.AppendFormat(" * {0}{1}{2}{3}",
                    verString.PadRight(verLength),
                    datString.PadRight(datLength),
                    ontString.PadRight(ontLength),
                    comString);
            sb.AppendLine();

            // Add version lines
            foreach (var vm in data.Versies)
            {
                sb.AppendFormat(" * {0}{1}{2}{3}",
                    vm.Versie.PadRight(verLength),
                    vm.Datum.ToString("dd-MM-yyyy").PadRight(datLength),
                    vm.Ontwerper.PadRight(ontLength),
                    vm.Commentaar);
                sb.AppendLine();
            }

            // Add bottom
            sb.AppendLine(" *");
            sb.AppendLine(" ************************************************************************************/");

            return sb.ToString();
        }
    }
}
