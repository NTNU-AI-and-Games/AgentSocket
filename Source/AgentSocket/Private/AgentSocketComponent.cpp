#pragma once

#include "AgentSocketComponent.h"
#include "TCPSocket.h"
#include "GameFramework/PlayerInput.h"


// Get index of next closing curly brackets
int GetCorrespondingClosingBracket(FString& text, int openParenPos) {
	int closePos = openParenPos;
	int depth = 1;
	while (depth > 0) {
		if (closePos >= text.Len() - 1) {
			return -1;
		}
		TCHAR c = text[++closePos];
		if (c == TCHAR('{')) {
			depth++;
		}
		else if (c == TCHAR('}')) {
			depth--;
		}
	}
	return closePos;
}

// Split Concatenated json messages
// Ex. "{ id: some1... }{ id: some2... }" --> ["{ id: some1... }", "{ id: some2... }"]
TArray<FString> SplitJSONMessages(FString& JSONMessages) {
	TArray<FString> MessageArray;
	int openBracketPos = 0;
	while (openBracketPos < JSONMessages.Len()) {
		int closeBracketPos = GetCorrespondingClosingBracket(JSONMessages, openBracketPos);
		if (closeBracketPos < 0) break;
		MessageArray.Add(JSONMessages.Mid(openBracketPos, closeBracketPos + 1 - openBracketPos));
		openBracketPos = closeBracketPos + 1;
	}
	return MessageArray;
}


UAgentSocketComponent::UAgentSocketComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UAgentSocketComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (ViewportCapturer) {
		ViewportCapturer->TickGrab();
	}
	if (AgentActionHandler) {
		AgentActionHandler->RunActions();
	}
}


void UAgentSocketComponent::OnMessageReceived(FString Message)
{
	UE_LOG(LogSockets, Verbose, TEXT("AgentSocket>> Received %s"), *Message);

	// Split messages, and use only the first
	TArray<FString> Messages = SplitJSONMessages(Message);
	FString Msg = Messages[0];
	//UE_LOG(LogSockets, Error, TEXT("AgentSocket>> Message: %s"), *Msg)

	// If is ActionMapping
	if (!AgentActionHandler->ParseMessage(Msg)) {
		RespondError(TEXT("Request is not properly formatted"));
	} else {
		if (AgentActionHandler->RunActions()) {
			SendStepResponse();
		} else {
			RespondError(TEXT("Action properly formatted but failed to act"));
		}
	}
}


void UAgentSocketComponent::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());

	// Do not run if not controlled by the local player
	if (GEngine->GetNetMode(GetWorld()) != NM_Client) return;
	if (!PlayerController->IsLocalPlayerController()) return;


	// Create TCP Socket
	TCPSocket = NewObject<UTCPSocket>(this);
	TCPSocket->Properties = Properties.TCPSocket;
	if (TCPSocket->LaunchTCP()) {
		UE_LOG(LogSockets, Warning, TEXT("AgentSocket>> Created TCP socket for client"))
			TCPSocket->OnMessageReceived.AddDynamic(this, &UAgentSocketComponent::OnMessageReceived);
	}
	else {
		UE_LOG(LogSockets, Error, TEXT("AgentSocket>> Failed to launch TCP socket for AgentSocket"))
	}

	UPlayerInput* PlayerInput = Cast<APlayerController>(GetOwner())->PlayerInput;
	
	Environment = NewObject<UAgentEnvironment>(this);
	AgentActionHandler = NewObject<UAgentActionHandler>(this);
	AgentActionHandler->Initialize(PlayerInput);

	ViewportCapturer = NewObject<UViewportCapturer>(this);
	ViewportCapturer->Properties = Properties.ViewportCapturer;
	ViewportCapturer->OnImageReady.AddDynamic(this, &UAgentSocketComponent::OnUpdateStream);
	ViewportCapturer->StartFrameGrab();
}


void UAgentSocketComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UTCPSocket::ID = 0;
}


bool UAgentSocketComponent::RespondError(FString ErrorMessage)
{
	FString f = FString::Format(TEXT(R"({"type":"ACK", "Status":"Error", "ErrorMsg": "{0}"})"), { *ErrorMessage });
	return TCPSocket->SendMessage(f);
}


bool UAgentSocketComponent::RunActions()
{
	return AgentActionHandler->RunActions();
}


void UAgentSocketComponent::AddReward(int value)
{
	Environment->Response.Reward.value += value;
}


void UAgentSocketComponent::SetGameReset(bool bIsGameReset)
{
	Environment->Response.State.GameOver = bIsGameReset;
}

void UAgentSocketComponent::SetGameOver(bool bIsGameOver)
{
	Environment->Response.State.GameReset = bIsGameOver;
}





bool UAgentSocketComponent::SendStepResponse()
{
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this]()
	{
		FString Json;

		// TODO: Improve performance of this. UStructToJsonObjectString is slow
		FJsonObjectConverter::UStructToJsonObjectString<FStepResponse>(Environment->Response, Json);

		// Send the JSON
		bool bSuccessJson = TCPSocket->SendMessage(Json);

		// Reset environment json responses
		Environment->Response.Reward.value = Properties.RewardDefaultValue;
		Environment->Response.State.GameOver = false;
		Environment->Response.State.GameReset = false;

		// Send the Image
		bool bSuccessImage = TCPSocket->SendBinary(Environment->ImageResponse);

	});
	return true;
}


void UAgentSocketComponent::OnUpdateStream(const TArray<uint8>& CompressedBitmap)
{
	Environment->SetScreenShot(CompressedBitmap);
}
