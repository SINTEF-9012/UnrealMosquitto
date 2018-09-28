using UnrealBuildTool;
using System.IO;

public class UnrealMosquittoEditor : ModuleRules
{
	public UnrealMosquittoEditor(ReadOnlyTargetRules Target) : base(Target)
	{

		/** existing constructor code */
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"ProjectName",
			"BlueprintGraph"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
