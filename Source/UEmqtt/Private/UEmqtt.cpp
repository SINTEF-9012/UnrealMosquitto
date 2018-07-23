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
	FString pluginDir = FPaths::Combine(IPluginManager::Get().FindPlugin("UEmqtt")->GetBaseDir(), TEXT("Source/ThirdParty/mosquitto"));
	FString binDirMosquitto = FPaths::Combine(*pluginDir, TEXT("libmosquitto/bin"));

	// Add on the relative location of the third party dll and load it
	FString openSslPath;
	FString mosquittoPath;
#if PLATFORM_WINDOWS
	openSslPath = FPaths::Combine(*pluginDir, TEXT("libssl/bin/Win64"));
	mosquittoPath = FPaths::Combine(*binDirMosquitto, TEXT("Win64"));
#elif PLATFORM_MAC
	mosquittoPath = FPaths::Combine(*binDirMosquitto, TEXT("Mac"));
#endif // PLATFORM_WINDOWS

	if (!mosquittoPath.IsEmpty())
	{
#if PLATFORM_WINDOWS
		cryptoHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*openSslPath, TEXT("libcrypto-1_1-x64.dll"))));
		openSslHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*openSslPath, TEXT("libssl-1_1-x64.dll"))));
		mosquittoHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*mosquittoPath, TEXT("mosquitto.dll"))));
		mosquittoppHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*mosquittoPath, TEXT("mosquittopp.dll"))));
#elif PLATFORM_MAC
		mosquittoHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*mosquittoPath, TEXT("mosquitto.dylib"))));
		mosquittoppHandle = FPlatformProcess::GetDllHandle(*(FPaths::Combine(*mosquittoPath, TEXT("mosquittopp.dylib"))));
#endif // PLATFORM_WINDOWS
	}
}

void FUEmqttModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(cryptoHandle);
	FPlatformProcess::FreeDllHandle(openSslHandle);
	FPlatformProcess::FreeDllHandle(mosquittoppHandle);
	FPlatformProcess::FreeDllHandle(mosquittoHandle);
	mosquittoHandle = nullptr;
	mosquittoppHandle = nullptr;
	openSslHandle = nullptr;
	cryptoHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUEmqttModule, UEmqtt);

DEFINE_LOG_CATEGORY(LogMQTT);
