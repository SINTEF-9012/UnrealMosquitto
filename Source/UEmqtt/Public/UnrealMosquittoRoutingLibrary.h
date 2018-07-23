// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealMosquittoRoutingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UEMQTT_API UUnrealMosquittoRoutingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Routing not matched (string)"), Category = "Utilities|Routing")
	static bool RoutingNotMatched(const FString &A, const FString &B);
};
