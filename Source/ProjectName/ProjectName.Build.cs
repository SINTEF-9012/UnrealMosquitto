using UnrealBuildTool;
using System.IO;

public class ProjectName : ModuleRules
{
    public ProjectName(ReadOnlyTargetRules Target) : base(Target)
    {

        /** existing constructor code */

        // /!\ add this to the end /!\
        LoadThirdPartyDLL("mosquitto", Target);
        LoadThirdPartyDLL("mosquittopp", Target);
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }

    public bool LoadThirdPartyDLL(string libname, ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, libname, "Libraries");

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, libname + "." + PlatformString + ".lib"));

            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, libname, "Includes"));
        }

        Definitions.Add(string.Format("WITH_" + libname.ToUpper() + "_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
