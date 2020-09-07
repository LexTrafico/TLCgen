using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TLCGen.Generators.VisualProject
{
    public class VisualProjectSettingsProvider
    {
        private static VisualProjectSettingsProvider _default;

        public static VisualProjectSettingsProvider Default => _default ??= new VisualProjectSettingsProvider();

        public VisualProjectSettingsModel Settings { get; set; }
    }
}
