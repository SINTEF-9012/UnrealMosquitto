// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HAL/Runnable.h"

#include <mosquittopp.h>
#include <queue>
#include <string>

#include "MQTTMessage.h"

namespace UnrealMosquitto {

	enum class InputEventType {
		Connect,
		Disconnect,
		Message,
		Subscribe,
		Unsubscribe,
		Publish,
		//Log,
		//Error
	};

	enum class OutputEventType {
		Publish,
		Subscribe,
		Unsubscribe,
	};

	struct InputSubscribeEvent {
		int mid;
		int qos_count;
		const int* granted_qos;
	};

	struct OutputSubscribeEvent {
		//int mid;
		int qos;
		char* sub;
	};

	struct OutputUnsubscribeEvent {
		//int mid;
		char* sub;
	};

	struct OutputPublishEvent {
		//int mid;
		char* topic;
		int payloadlen;
		void* payload;
		int qos;
		bool retain;
	};

	struct InputEvent {
		InputEventType type;
		union {
			int mid;
			InputSubscribeEvent subscription;
			struct mosquitto_message* message;
		};
	};


	struct OutputEvent {
		OutputEventType type;
		union {
			OutputSubscribeEvent subscription;
			OutputUnsubscribeEvent unsubscription;
			OutputPublishEvent message;
		};
	};

	// Will be declared later
	class FRunnableTask;

	/*
	MQTTClient manages the connection with the MQTT
	message broker.

	It requires a pointer to the task in which it will run.
	*/
	class MQTTClient : public mosqpp::mosquittopp
	{
	public:
		MQTTClient(const char* id);
		~MQTTClient();

		void on_connect(int rc);
		void on_disconnect(int rc);
		void on_publish(int mid);
		void on_message(const struct mosquitto_message * message);
		void on_subscribe(int mid, int qos_count, const int* granted_qos);
		void on_unsubscribe(int mid);

		UnrealMosquitto::FRunnableTask* Task;
	};

	/*
	Thread managing the connection with MQTT.

	Mosquitto supports multithread but this solution was simpler to
	compile.

	The thread needs parameters to configure the connection :
	Host, which is likely the IP address of the MQTT message broker,
	Port, which is the TCP port of the MQTT message broker,
	and ClientName which is an ID used when communicating with the
	MQTT message broker.

	Like MQTTClient, MonFRunnable must contain reference
	to a message queue and a lock.
	*/
	class FRunnableTask : public FRunnable {
	public:
		virtual bool Init() override;
		virtual uint32 Run() override;
		virtual void Stop() override;

		void PushInputEvent(InputEvent ev);
		void PushOutputEvent(OutputEvent ev);

	private:
		bool _continue;

	public:

		std::string Host;
		int32 Port;
		std::string ClientName;
		std::string Username;
		std::string Password;

		// The message queues are used to store incoming and outcoming MQTT messages.
		std::queue<InputEvent>* InputQueue;
		std::queue<OutputEvent>* OutputQueue;

		// They require a lock, to make it thread safe.
		FCriticalSection* InputQueueLock;
		FCriticalSection* OutputQueueLock;

		// The stop request asks the thread to stop. It is less aggressive than the Stop();
		void StopRequest();
	};
}

#include "UnrealMosquitto.generated.h"

UCLASS(Blueprintable)
class PROJECTNAME_API AUnrealMosquitto : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnrealMosquitto();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MQTT")
		FString Host = "127.0.0.1";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MQTT")
		int32 Port = 1883;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MQTT")
		FString Username = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MQTT")
		FString Password = "";


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MQTT")
		FString ClientName = "UnrealMosquitto";

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "MQTT")
		void OnConnect();

	UFUNCTION(BlueprintImplementableEvent, Category = "MQTT")
		void OnDisconnect();

	UFUNCTION(BlueprintImplementableEvent, Category = "MQTT")
		void OnPublish(int mid);

	UFUNCTION(BlueprintImplementableEvent, Category = "MQTT")
		void OnMessage(const UMQTTMessage* message);

	UFUNCTION(BlueprintImplementableEvent, Category = "MQTT")
		void OnSubscribe(int mid, const TArray<int>& qos);

	UFUNCTION(BlueprintImplementableEvent, Category = "MQTT")
		void OnUnsubscribe(int mid);

	// Methods
	UFUNCTION(BlueprintCallable, Category = "MQTT")
		void Subscribe(const FString& topic, int qos = 0);

	UFUNCTION(BlueprintCallable, Category = "MQTT")
		void Unsubscribe(const FString& topic);

	UFUNCTION(BlueprintCallable, Category = "MQTT")
		void PublishString(const FString& message, const FString& topic, int qos = 0, bool retain = false);

private:
	void MQTT_Worker();

	// References for the thread task
	UnrealMosquitto::FRunnableTask* _Task;

	// And the thread itself
	FRunnableThread* _Thread;	
	
};
