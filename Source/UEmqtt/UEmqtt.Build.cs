// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UEmqtt : ModuleRules
{
	public UEmqtt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"UEmqtt/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"UEmqtt/Private",
				"ThirdParty/mosquitto/includes"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Projects",
				"InputCore",
				"BlueprintGraph"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

		PublicLibraryPaths.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "ThirdParty", "mosquitto", "libraries")));
		PublicAdditionalLibraries.Add("mosquitto.lib");
		PublicAdditionalLibraries.Add("mosquittopp.lib");
		PublicDelayLoadDLLs.Add("mosquitto.dll");
		PublicDelayLoadDLLs.Add("mosquittopp.dll");
	}
}
