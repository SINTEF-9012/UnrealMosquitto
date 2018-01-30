// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectName.h"
#include "MQTTMessage.h"

UMQTTMessage::UMQTTMessage() : UObject()
{
}

UMQTTMessage::~UMQTTMessage()
{
	if (Payload.Buffer != NULL) {
		free(Payload.Buffer);
	}
}

void UMQTTMessage::FromMosquitto(const struct mosquitto_message * message)
{
	// We copy the properties, it should be faster
	// than making getter/setters
	Topic = FString(message->topic);
	QOS = message->qos;
	Retain = message->retain;
	PayloadLength = message->payloadlen;

	// Allocate memory for the copy.
	// The additional byte is to save the \0 in case it is a string
	Payload.Buffer = malloc(PayloadLength + 1);
	((char*)Payload.Buffer)[PayloadLength] = 0;

	if (Payload.Buffer != NULL) {
		memcpy(Payload.Buffer, message->payload, PayloadLength);
	}
}

FString UMQTTMessage::GetPayloadAsString()
{
	return FString(UTF8_TO_TCHAR(Payload.Buffer));
}