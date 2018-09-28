// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectName.h"
#include "UnrealMosquitto.h"

#include "RunnableThread.h"
#include "TimerManager.h"

#include <mosquitto.h>

#include <queue>
#include <iostream>
#include <string>

bool UnrealMosquitto::FRunnableTask::Init() {
	_continue = true;
	return true;
}

void UnrealMosquitto::FRunnableTask::StopRequest() {
	_continue = false;
}

void UnrealMosquitto::FRunnableTask::Stop() {
	StopRequest();
}

uint32 UnrealMosquitto::FRunnableTask::Run() {
	mosqpp::lib_init();

	// Creating the MQTT connection client.
	MQTTClient connection(ClientName.c_str());
	connection.max_inflight_messages_set(0);
	connection.Task = this;
	int returnCode = 0;
	if (!Username.empty()) {
		connection.username_pw_set(Username.c_str(), Password.c_str());
	}
	UE_LOG(LogTemp, Error, TEXT("Connecting to MQTT broker."));
	returnCode = connection.connect(Host.c_str(), Port, 10);

	if (returnCode != 0) {
		UE_LOG(LogTemp, Error, TEXT("Mosquitto connect error: %s"), ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
		_continue = false;
	}

	// While StopRequest() hasn't been called
	while (_continue) {

		// Lock critical region.
		OutputQueueLock->Lock();

		// Send the data from the output queue
		while (OutputQueue != NULL && !OutputQueue->empty()) {
			OutputEvent ev = OutputQueue->front();
			OutputQueue->pop();


			switch (ev.type) {
			case OutputEventType::Subscribe:
				returnCode = connection.subscribe(NULL, ev.subscription.sub, ev.subscription.qos);
				free(ev.subscription.sub);
				break;
			case OutputEventType::Unsubscribe:
				returnCode = connection.unsubscribe(NULL, ev.unsubscription.sub);
				free(ev.subscription.sub);
				break;
			case OutputEventType::Publish:
				returnCode = connection.publish(NULL, ev.message.topic, ev.message.payloadlen,
					ev.message.payload, ev.message.qos, ev.message.retain);
				free(ev.message.topic);
				free(ev.message.payload);
				break;
			}
		}

		// Unlock critical region.
		OutputQueueLock->Unlock();

		if (returnCode != 0) {
			UE_LOG(LogTemp, Error, TEXT("Mosquitto output error: %s"), ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
			connection.reconnect();
		}

		// Run the connection loop (receive, connect, etc...)
		returnCode = connection.loop();
		/*int i = 0;
		do {
		returnCode = connection.loop();
		} while (_continue && returnCode == MOSQ_ERR_SUCCESS && ++i < 10);*/

		if (returnCode != 0) {
			UE_LOG(LogTemp, Error, TEXT("Mosquitto loop error: %s"), ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
			connection.reconnect();
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Disconnecting."));
	returnCode = connection.disconnect();
	if (returnCode != 0) {
		UE_LOG(LogTemp, Error, TEXT("Mosquitto disconnect error: %s"), ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
	}

	// Clean delete
	delete InputQueue;
	delete OutputQueue;
	delete InputQueueLock;
	delete OutputQueueLock;
	InputQueue = NULL;
	OutputQueue = NULL;
	InputQueueLock = NULL;
	OutputQueueLock = NULL;

	return 0;
}

UnrealMosquitto::MQTTClient::MQTTClient(const char * id) : mosqpp::mosquittopp(id)
{
}

UnrealMosquitto::MQTTClient::~MQTTClient() {
}


void UnrealMosquitto::MQTTClient::on_connect(int rc)
{
	if (rc != 0) {
		UE_LOG(LogTemp, Error, TEXT("Not connected."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Connected."));

	InputEvent ev;
	ev.type = UnrealMosquitto::InputEventType::Connect;
	Task->PushInputEvent(ev);
}

void UnrealMosquitto::MQTTClient::on_disconnect(int rc)
{
	InputEvent ev;
	ev.type = UnrealMosquitto::InputEventType::Disconnect;

	Task->PushInputEvent(ev);
}

void UnrealMosquitto::MQTTClient::on_publish(int mid)
{
	InputEvent ev;
	ev.type = UnrealMosquitto::InputEventType::Publish;
	ev.mid = mid;

	Task->PushInputEvent(ev);
}

struct mosquitto_message* copy_mosquitto_message(const mosquitto_message* src) {
	struct mosquitto_message* dst = (struct mosquitto_message *) calloc(1, sizeof(struct mosquitto_message));

	dst->mid = src->mid;
	dst->topic = _strdup(src->topic);
	if (!dst->topic) return NULL;
	dst->qos = src->qos;
	dst->retain = src->retain;
	if (src->payloadlen) {
		dst->payload = malloc(src->payloadlen);
		if (!dst->payload) {
			free(dst->topic);
			return NULL;
		}
		memcpy(dst->payload, src->payload, src->payloadlen);
		dst->payloadlen = src->payloadlen;
	}
	else {
		dst->payloadlen = 0;
		dst->payload = NULL;
	}
	return dst;
}

void UnrealMosquitto::MQTTClient::on_message(const mosquitto_message * src)
{
	// Copy the MQTT message
	// We cannot copy it yet to a UObject because UObject MUST be created
	// in the main thread...
	auto dst = copy_mosquitto_message(src);

	InputEvent ev;
	ev.type = UnrealMosquitto::InputEventType::Message;
	ev.message = dst;

	UE_LOG(LogTemp, Warning, TEXT("Message received, Topic %s"), ANSI_TO_TCHAR(ev.message->topic));

	Task->PushInputEvent(ev);
}

void UnrealMosquitto::MQTTClient::on_subscribe(int mid, int qos_count, const int * granted_qos)
{
	InputEvent ev;
	ev.type = UnrealMosquitto::InputEventType::Subscribe;
	ev.subscription = {
		mid, qos_count, granted_qos
	};

	Task->PushInputEvent(ev);
}

void UnrealMosquitto::MQTTClient::on_unsubscribe(int mid)
{
	InputEvent ev;
	ev.type = UnrealMosquitto::InputEventType::Unsubscribe;
	ev.mid = mid;

	Task->PushInputEvent(ev);
}

void UnrealMosquitto::FRunnableTask::PushInputEvent(InputEvent ev)
{
	InputQueueLock->Lock();
	if (InputQueue != NULL) {
		InputQueue->push(ev);
	}
	InputQueueLock->Unlock();
}

void UnrealMosquitto::FRunnableTask::PushOutputEvent(OutputEvent ev)
{
	OutputQueueLock->Lock();
	if (OutputQueue != NULL) {
		OutputQueue->push(ev);
	}
	OutputQueueLock->Unlock();
}


AUnrealMosquitto::AUnrealMosquitto()
{
	// Disable event tick
	PrimaryActorTick.bCanEverTick = false;
}

void AUnrealMosquitto::BeginPlay()
{
	Super::BeginPlay();


	_Task = new UnrealMosquitto::FRunnableTask();

	// Filling the properties
	_Task->InputQueue = new std::queue<UnrealMosquitto::InputEvent>();
	_Task->OutputQueue = new std::queue<UnrealMosquitto::OutputEvent>();
	_Task->InputQueueLock = new FCriticalSection();
	_Task->OutputQueueLock = new FCriticalSection();

	_Task->Host = std::string(TCHAR_TO_ANSI(*Host));
	_Task->ClientName = std::string(TCHAR_TO_ANSI(*ClientName));
	_Task->Port = Port;
	_Task->Username = std::string(TCHAR_TO_ANSI(*Username));
	_Task->Password = std::string(TCHAR_TO_ANSI(*Password));

	_Thread = FRunnableThread::Create(_Task, TEXT("UnrealMosquitto"), 0, TPri_Normal, FGenericPlatformAffinity::GetNoAffinityMask());

	// Start the main thread timer that will be used to check the message queue
	FTimerHandle timer_handle;
	GetWorldTimerManager().SetTimer(timer_handle, this,
		&AUnrealMosquitto::MQTT_Worker, 0.05f, true);
}

// Safe cleaning
void AUnrealMosquitto::BeginDestroy() {
	Super::BeginDestroy();

	if (_Task != NULL) {
		_Task->StopRequest();
		return;
	}
}

void AUnrealMosquitto::MQTT_Worker()
{
	if (_Task->InputQueueLock == NULL) {
		return;
	}

	_Task->InputQueueLock->Lock();
	auto InputQueue = _Task->InputQueue;

	while (InputQueue != NULL && !InputQueue->empty()) {

		UnrealMosquitto::InputEvent ev = InputQueue->front();
		InputQueue->pop();

		TArray<int> qos;
		struct mosquitto_message* mosquitto_msg;
		UMQTTMessage* msg;

		switch (ev.type) {
		case UnrealMosquitto::InputEventType::Connect:
			OnConnect();
			break;

		case UnrealMosquitto::InputEventType::Disconnect:
			OnDisconnect();
			break;

		case UnrealMosquitto::InputEventType::Publish:
			OnPublish(ev.mid);
			break;

		case UnrealMosquitto::InputEventType::Message:
			mosquitto_msg = ev.message;

			// Convert the mosquitto struct to an Unreal object
			msg = NewObject<UMQTTMessage>();
			msg->FromMosquitto(mosquitto_msg);
			//UE_LOG(LogTemp, Warning, TEXT("InputEvent, Topic %s"), ANSI_TO_TCHAR(mosquitto_msg->topic));

			// Manual free because mosquitto_message_free(&mosquitto_msg)
			// is not compatible with Unreal's memory management
			free(mosquitto_msg->topic);
			free(mosquitto_msg->payload);
			free(mosquitto_msg);

			OnMessage(msg);
			break;

		case UnrealMosquitto::InputEventType::Subscribe:
			// Convert the C-like array to an Unreal array
			for (auto p = ev.subscription.granted_qos;
				p < ev.subscription.granted_qos + ev.subscription.qos_count; ++p) {
				qos.Add(*p);
			}
			OnSubscribe(ev.subscription.mid, qos);
			break;

		case UnrealMosquitto::InputEventType::Unsubscribe:
			OnUnsubscribe(ev.mid);
			break;
		}

	}
	_Task->InputQueueLock->Unlock();
}

void AUnrealMosquitto::Subscribe(const FString& topic, int qos /*= 0*/) {
	// Copy the topic
	char* sub = _strdup(TCHAR_TO_ANSI(*topic));

	UnrealMosquitto::OutputEvent ev;
	ev.type = UnrealMosquitto::OutputEventType::Subscribe;
	ev.subscription = {
		qos, sub
	};

	_Task->PushOutputEvent(ev);
}

void AUnrealMosquitto::Unsubscribe(const FString & topic) {
	// Copy the topic
	char* sub = _strdup(TCHAR_TO_ANSI(*topic));

	UnrealMosquitto::OutputEvent ev;
	ev.type = UnrealMosquitto::OutputEventType::Unsubscribe;
	ev.unsubscription = {
		sub
	};

	_Task->PushOutputEvent(ev);
}

void AUnrealMosquitto::PublishString(const FString& message, const FString& topic, int qos /*= 0*/, bool retain /*= false*/) {
	// Copy the topic and the message
	char* sub = _strdup(TCHAR_TO_ANSI(*topic));
	char* msg = _strdup(TCHAR_TO_ANSI(*message));

	UnrealMosquitto::OutputEvent ev;
	ev.type = UnrealMosquitto::OutputEventType::Publish;
	ev.message = {
		sub, (int)strlen(msg), msg, qos, retain
	};

	_Task->PushOutputEvent(ev);
}