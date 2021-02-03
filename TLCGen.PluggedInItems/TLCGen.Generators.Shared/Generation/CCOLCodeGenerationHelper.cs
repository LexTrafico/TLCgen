using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using TLCGen.Models;

namespace TLCGen.Generators.Shared
{
    public static class CCOLCodeGenerationHelper
    {
        public static void AddCodeTypeToStringBuilder(
            ControllerModel c, 
            Dictionary<CCOLCodeTypeEnum, SortedDictionary<int, ICCOLCodePieceGenerator>> orderedGenerators,
            StringBuilder sb, 
            CCOLCodeTypeEnum type, 
            bool includevars, 
            bool includecode, 
            bool addnewlinebefore, 
            bool addnewlineatend, 
            string ts,
            List<string> varsBefore = null)
	    {
			if (orderedGenerators[type].Any())
			{
                if ((includevars || includecode) && addnewlinebefore) sb.AppendLine();
                if (includevars)
                {
                    var vars = new List<string>();
                    var added = false;
                    if (varsBefore != null) vars = varsBefore;
                    foreach (var gen in orderedGenerators[type])
                    {
                        var lv = gen.Value.GetFunctionLocalVariables(c, type);
                        if (lv.Any())
                        {
                            foreach (var i in lv)
                            {
                                if (vars.All(x => x != i.Item2))
                                {
                                    vars.Add(i.Item2);
                                    added = true;
                                    sb.AppendLine(!string.IsNullOrWhiteSpace(i.Item3) ? $"{ts}{i.Item1} {i.Item2} = {i.Item3};" : $"{ts}{i.Item1} {i.Item2};");
                                }
                            }
                        }
                    }
                    if (added && addnewlineatend) sb.AppendLine();
                }
                if (includecode)
                {
                    foreach (var gen in orderedGenerators[type].Where(x => x.Value.HasCodeForController(c, type)))
                    {
                        var code = gen.Value.GetCode(c, type, ts);
                        if (!string.IsNullOrWhiteSpace(code))
                        {
                            sb.Append(code);
                            if (addnewlineatend) sb.AppendLine();
                        }
                    }
                }
			}
		}

        /// <summary>
        /// Generates all "sys.h" lines for a given instance of CCOLElemListData.
        /// The function loops all elements in the Elements member.
        /// </summary>
        /// <param name="data">The instance of CCOLElemListData to use for generation</param>
        /// <param name="numberdefine">Optional: this string will be used as follows:
        /// - if it is null, lines are generated like this: #define ElemName #
        /// - if it is not null, it goes like this: #define ElemName (numberdefine + #)</param>
        /// <param name="startIndex">Optional: if not 0, numberdefine will be ignored, and 
        /// counting will start from the position</param>
        /// <returns></returns>
        public static string GetAllElementsSysHLines(CCOLElemListData data, string ts, string numberdefine = null, List<CCOLElement> extraElements = null)
        {
            var sb = new StringBuilder();

            var pad1 = data.DefineMaxWidth + $"{ts}#define  ".Length;
            var pad2 = data.Elements.Count.ToString().Length;
            var pad3 = data.CommentsMaxWidth;
            var index = 0;

            foreach (var elem in data.Elements)
            {
                if (elem.Dummy || Regex.IsMatch(elem.Define, @"[A-Z]+MAX"))
                    continue;
                
                sb.Append($"{ts}#define {elem.Define} ".PadRight(pad1));
                if (string.IsNullOrWhiteSpace(numberdefine))
                {
                    sb.Append($"{index}".PadLeft(pad2));
                }
                else
                {
                    sb.Append($"({numberdefine} + ");
                    sb.Append($"{index}".PadLeft(pad2));
                    sb.Append(")");
                }
				if (!string.IsNullOrWhiteSpace(elem.Commentaar))
				{
                    sb.Append(" /* ");
                    sb.Append($"{elem.Commentaar}".PadRight(pad3));
                    sb.Append(" */");
                }
				sb.AppendLine();
                ++index;
            }
            var indexautom = index;

            if (data.Elements.Count > 0 && data.Elements.Any(x => x.Dummy))
            {
                sb.AppendLine("#if (!defined AUTOMAAT && !defined AUTOMAAT_TEST) || defined VISSIM");
                foreach (var elem in data.Elements)
                {
                    if (!elem.Dummy || Regex.IsMatch(elem.Define, @"[A-Z]+MAX"))
                        continue;

                    sb.Append($"{ts}#define {elem.Define} ".PadRight(pad1));
                    if (string.IsNullOrWhiteSpace(numberdefine))
                    {
                        sb.Append($"{indexautom}".PadLeft(pad2));
                    }
                    else
                    {
                        sb.Append($"({numberdefine} + ");
                        sb.Append($"{indexautom}".PadLeft(pad2));
                        sb.Append(")");
                    }
					if (!string.IsNullOrWhiteSpace(elem.Commentaar))
					{
                        sb.Append(" /* ");
                        sb.Append($"{elem.Commentaar}".PadRight(pad3));
                        sb.Append(" */");
                    }
					sb.AppendLine();
                    ++indexautom;
                }
                sb.Append($"{ts}#define {data.Elements.Last().Define} ".PadRight(pad1));
                if (string.IsNullOrWhiteSpace(numberdefine))
                {
                    sb.AppendLine($"{indexautom}".PadLeft(pad2));
                }
                else
                {
                    sb.Append($"({numberdefine} + ");
                    sb.Append($"{indexautom}".PadLeft(pad2));
                    sb.AppendLine(")");
                }
                sb.AppendLine("#else");
                sb.Append($"{ts}#define {data.Elements.Last().Define} ".PadRight(pad1));
                if (string.IsNullOrWhiteSpace(numberdefine))
                {
                    sb.AppendLine($"{index}".PadLeft(pad2));
                }
                else
                {
                    sb.Append($"({numberdefine} + ");
                    sb.Append($"{index}".PadLeft(pad2));
                    sb.AppendLine(")");
                }
                sb.AppendLine("#endif");
            }
            else if(data.Elements.Count > 0)
            {
                sb.Append($"{ts}#define {data.Elements.Last().Define} ".PadRight(pad1));
                if (string.IsNullOrWhiteSpace(numberdefine))
                {
                    sb.AppendLine($"{index}".PadLeft(pad2));
                }
                else
                {
                    sb.Append($"({numberdefine} + ");
                    sb.Append($"{index}".PadLeft(pad2));
                    sb.AppendLine(")");
                }
            }

            return sb.ToString();
        }

        public static string GetAllElementsTabCLines(CCOLElemListData data, string ts)
        {
            var sb = new StringBuilder();

            var pad1 = ts.Length + data.CCOLCodeWidth + 2 + data.DefineMaxWidth; // 3: [ ]
            var pad2 = data.NameMaxWidth + 6;  // 6: space = space " " ;
            var pad3 = data.CCOLSettingWidth + 3 + data.DefineMaxWidth; // 3: space [ ]
            var pad4 = data.SettingMaxWidth + 4;  // 4: space = space ;
            var pad5 = data.CCOLTTypeWidth + 3 + data.DefineMaxWidth; // 3: space [ ]
            var pad6 = data.CommentsMaxWidth;
            var pad7 = data.TTypeMaxWidth + 4; // 4: ' = ;'

            foreach (var elem in data.Elements)
            {
                if (elem.Dummy)
                    continue;

                if (!string.IsNullOrWhiteSpace(elem.Naam))
                {
                    sb.Append($"{ts}{data.CCOLCode}[{elem.Define}]".PadRight(pad1));
                    sb.Append($" = \"{elem.Naam}\";".PadRight(pad2));
                    if (!string.IsNullOrEmpty(data.CCOLSetting) && elem.Instelling.HasValue)
                    {
                        sb.Append($" {data.CCOLSetting}[{elem.Define}]".PadRight(pad3));
                        sb.Append($" = {elem.Instelling};".PadRight(pad4));
                    }
                    if (!string.IsNullOrEmpty(data.CCOLTType) && elem.TType != CCOLElementTimeTypeEnum.None)
                    {
                        sb.Append($" {data.CCOLTType}[{elem.Define}]".PadRight(pad5));
                        sb.Append($" = {elem.TType};");
                    }
                    else
                    {
                        sb.Append("".PadRight(pad5 + pad7));
                    }
					if (!string.IsNullOrWhiteSpace(elem.Commentaar))
					{
                        sb.Append(" /* ");
                        sb.Append($"{ elem.Commentaar}".PadRight(pad6));
                        sb.Append(" */");
					}
                    sb.AppendLine();
                }
            }

            if (data.Elements.Count > 0 && data.Elements.Any(x => x.Dummy))
            {
                sb.AppendLine("#if (!defined AUTOMAAT && !defined AUTOMAAT_TEST)");
                foreach (var delem in data.Elements)
                {
                    if (!delem.Dummy)
                        continue;
                    sb.Append($"{ts}{data.CCOLCode}[{delem.Define}]".PadRight(pad1));
                    sb.Append($" = \"{delem.Naam}\";".PadRight(pad2));
                    if (!string.IsNullOrEmpty(data.CCOLSetting) && delem.Instelling.HasValue)
                    {
                        sb.Append($" {data.CCOLSetting}[{delem.Define}]".PadRight(pad3));
                        sb.Append($" = {delem.Instelling};".PadRight(pad4));
                    }
                    if (!string.IsNullOrEmpty(data.CCOLTType) && delem.TType != CCOLElementTimeTypeEnum.None)
                    {
                        sb.Append($" {data.CCOLTType}[{delem.Define}]".PadRight(pad5));
                        sb.Append($" = {delem.TType};");
                    }
                    else
                    {
                        sb.Append("".PadRight(pad5));
                    }
                    if (!string.IsNullOrWhiteSpace(delem.Commentaar))
                    {
                        sb.Append(" /* ");
                        sb.Append($"{delem.Commentaar}".PadRight(pad6));
                        sb.Append(" */");
                    }
                    sb.AppendLine();
                }
                sb.AppendLine("#endif");
            }

            return sb.ToString();
        }
    }
}
