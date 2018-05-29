// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UEmqtt.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUEmqttModule"

void FUEmqttModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("UEmqtt")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString mosquittoPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/mosquitto/libraries"));

	if (!mosquittoPath.IsEmpty()) {
		mosquittoHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*mosquittoPath, TEXT("mosquitto.dll"))));
		mosquittoppHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*mosquittoPath, TEXT("mosquittopp.dll"))));
	}
}

void FUEmqttModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(mosquittoHandle);
	FPlatformProcess::FreeDllHandle(mosquittoppHandle);
	mosquittoHandle = nullptr;
	mosquittoppHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUEmqttModule, UEmqtt);

DEFINE_LOG_CATEGORY(LogMQTT);
