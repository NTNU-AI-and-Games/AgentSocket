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
	if (AgentActionHandler) {
		AgentActionHandler->RunActions();
	}
	if (ViewportCapturer) {
		ViewportCapturer->TickGrab();
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


bool UAgentSocketComponent::RespondSuccess()
{
	return TCPSocket->SendMessage(R"({"type":"ACK", "Status":"OK"})");
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


bool UAgentSocketComponent::RunSomething()
{

	UE_LOG(LogSockets, Warning, TEXT("AgentSocket>> Running some command"));
	FViewport* Viewport = GEngine->GameViewport->Viewport;

	FViewportClient* Client = Viewport->GetClient();


	UPlayerInput* playerInput = Cast<APlayerController>(Cast<APawn>(GetOwner())->Controller)->PlayerInput;
	FKey a = playerInput->ActionMappings[0].Key;

	FInputActionKeyMapping mapping = playerInput->ActionMappings[0];
	FKey key = mapping.Key;

	playerInput->InputKey(key, EInputEvent::IE_Pressed, 1, 0);
	if (mapping.bShift) {
		playerInput->InputKey(EKeys::LeftShift, EInputEvent::IE_Pressed, 1, 0);
	}
	if (mapping.bCtrl) {
		playerInput->InputKey(EKeys::LeftControl, EInputEvent::IE_Pressed, 1, 0);
	}
	if (mapping.bAlt) {
		playerInput->InputKey(EKeys::LeftAlt, EInputEvent::IE_Pressed, 1, 0);
	}
	if (mapping.bCmd) {
		playerInput->InputKey(EKeys::LeftCommand, EInputEvent::IE_Pressed, 1, 0);
	}


	UE_LOG(LogTemp, Warning, TEXT("length: %s"), *playerInput->ActionMappings[25].Key.GetDisplayName().ToString());
	UE_LOG(LogTemp, Warning, TEXT("length: %d"), playerInput->ActionMappings.Num());
	UE_LOG(LogTemp, Warning, TEXT("key: %s"), *a.GetDisplayName().ToString());

	return true;
}


class AGENTSOCKET_API FSendTask : public FNonAbandonableTask
{
	friend class FAsyncTask<FSendTask>;
public:
	FSendTask(TFunction<bool()> InSendFunc) :SendFunc(InSendFunc) {};
	~FSendTask() {};

	void DoWork() {
		SendFunc();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTCPSocketSendTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	TFunction<bool()> SendFunc;
};


bool UAgentSocketComponent::SendStepResponse()
{
	return TCPSocket->SendBinary(Environment->ImageResponse);
}


void UAgentSocketComponent::OnUpdateStream(const TArray<uint8>& CompressedBitmap)
{
	Environment->SetScreenShot(CompressedBitmap);
}
