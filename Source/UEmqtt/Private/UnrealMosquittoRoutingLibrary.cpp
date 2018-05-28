// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMosquittoRoutingLibrary.h"
#include "UEmqtt.h"

#include <mosquitto.h>

bool UUnrealMosquittoRoutingLibrary::RoutingNotMatched(const FString &A, const FString &B)
{
	const char *topic = TCHAR_TO_ANSI(*A);
	const char *sub = TCHAR_TO_ANSI(*B);

	bool result;
	mosquitto_topic_matches_sub(sub, topic, &result);

	return !result;
}