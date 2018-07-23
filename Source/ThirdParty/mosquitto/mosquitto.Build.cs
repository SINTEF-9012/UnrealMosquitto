// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class mosquitto : ModuleRules {
	public mosquitto(ReadOnlyTargetRules Target) : base(Target) {
		Type = ModuleType.External;
		string openSslPath = Path.Combine(ModuleDirectory, "libssl");
		string mosquittoPath = Path.Combine(ModuleDirectory, "libmosquitto");

		switch (Target.Platform) {
		case UnrealTargetPlatform.Win64:
			PublicAdditionalLibraries.AddRange(
				new string[] {
					"libcrypto.lib",
					"libssl.lib",
					"mosquitto.lib",
					"mosquittopp.lib"
				});

				PublicDelayLoadDLLs.AddRange(
				new string[] {
					"libcrypto-1_1-x64.dll",
					"libssl-1_1-x64.dll",
					"mosquitto.dll",
					"mosquittopp.dll"
				});
			break;

		default:
			return;
		}

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(openSslPath, "include"),
				Path.Combine(mosquittoPath, "include")
				// ... add public include paths required here ...
			});

		PublicLibraryPaths.AddRange(
			new string[] {
				Path.Combine(openSslPath, "lib", Target.Platform.ToString()),
				Path.Combine(mosquittoPath, "lib", Target.Platform.ToString())
				// ... add public library paths required here ...
			});
	}
}
