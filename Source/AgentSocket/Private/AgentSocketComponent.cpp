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


UAgentSocketComponent::UAgentSocketComponent() {}


void UAgentSocketComponent::OnMessageReceived(FString Message)
{
	UE_LOG(LogSockets, Warning, TEXT("AgentSocket>> Received %s"), *Message);

	// Split messages, and use only the first
	TArray<FString> Messages = SplitJSONMessages(Message);
	FString Msg = Messages[0];

	// If is ActionMapping
	FAgentAction Action;
	if (!ParseMessage(Msg, Action)) {
		RespondError(TEXT("Request is not properly formatted"));
	}
	else {
		if (RunAction(Action)) {
			SendStepResponse();
			//RespondSuccess();
		}
		else {
			RespondError(TEXT("Action properly formatted but failed to act"));
		}
	}
}


bool UAgentSocketComponent::ParseMessage(FString Message, struct FAgentAction& OutAction)
{
	FAgentAction Action;

	// Automatically convert json to struct with FJsonObjectConverter::JsonObjectStringToUStruct()
	// Note: It matches enums with string (not int)
	if (FJsonObjectConverter::JsonObjectStringToUStruct<FAgentAction>(*Message, &Action, 0, 0)) {
		OutAction = Action;
	}
	else {
		UE_LOG(LogSockets, Error, TEXT("AgentSocket>> JSON payload is not properly formatted"));
		return false;
	}
	return true;
}

// Called when the game starts or when spawned
void UAgentSocketComponent::BeginPlay()
{
	Super::BeginPlay();
	APawn* Pawn = Cast<APawn>(GetOwner());

	// Do not run if not controlled by the local player
	if (GEngine->GetNetMode(GetWorld()) != NM_Client) return;
	if (!Pawn->IsLocallyControlled()) return;


	// Create TCP Socket
	TCPSocket = NewObject<UTCPSocket>(this);
	if (TCPSocket->LaunchTCP()) {
		UE_LOG(LogSockets, Warning, TEXT("AgentSocket>> Created TCP socket for client"))
			TCPSocket->OnMessageReceived.AddDynamic(this, &UAgentSocketComponent::OnMessageReceived);
	}
	else {
		UE_LOG(LogSockets, Error, TEXT("AgentSocket>> Failed to launch TCP socket for AgentSocket"))
	}


	// Create ClientWindowCapturer
	ClientWindowCapturer = NewObject<UClientWindowCapturer>(this);
	ClientWindowCapturer->Properties = Properties.Capturer;
	ClientWindowCapturer->Start();
	Environment = NewObject<UAgentEnvironment>(this);

	// Bind texture change event
	ClientWindowCapturer->UpdateStream.AddDynamic(this, &UAgentSocketComponent::OnUpdateStream);
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


bool UAgentSocketComponent::RunAction(FAgentAction& Action)
{
	if (Action.Type == EAgentActionType::ACTION) {
		// Do the action
		FViewport* Viewport = GEngine->GameViewport->Viewport;
		/*FViewportClient* Client = Viewport->GetClient();
		Client->InputAxis(Viewport, 0, EKeys::MouseX, EInputEvent::IE_Axis, 0.5);*/

		FViewportClient* Client = Viewport->GetClient();


		/*UPlayerInput* playerInput = Cast<APlayerController>(Cast<APawn>(GetOwner())->Controller)->PlayerInput;
		FKey a = playerInput->ActionMappings[1].Key;
		FKey FirstFoundKey = playerInput->GetKeysForAction(Action.Name)[0].Key;*/

		// Get the first key mapping of the action, and input key
		UPlayerInput* playerInput = Cast<APlayerController>(Cast<APawn>(GetOwner())->Controller)->PlayerInput;
		//FInputActionKeyMapping mapping = playerInput->ActionMappings[0];
		if (playerInput->GetKeysForAction(Action.Name).Num() > 0) {
			FInputActionKeyMapping mapping = playerInput->GetKeysForAction(Action.Name)[0];
			FKey key = mapping.Key;

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
			playerInput->InputKey(key, EInputEvent::IE_Pressed, 1, 0);
		}
	}
	return true;
}


bool UAgentSocketComponent::SendState(FStateResponse StateResponse)
{
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString<FStateResponse>(StateResponse, Json);
	return TCPSocket->SendMessage(Json);
}


bool UAgentSocketComponent::SendReward(FRewardResponse RewardResponse)
{
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString<FRewardResponse>(RewardResponse, Json);
	return TCPSocket->SendMessage(Json);
}


void UAgentSocketComponent::AddReward(int value)
{
	Environment->Response.Reward.value += value;
}


bool UAgentSocketComponent::RunSomething()
{

	UE_LOG(LogSockets, Warning, TEXT("AgentSocket>> Running some command"));
	FViewport* Viewport = GEngine->GameViewport->Viewport;
	/*FViewportClient* Client = Viewport->GetClient();
	Client->InputAxis(Viewport, 0, EKeys::MouseX, EInputEvent::IE_Axis, 0.5);*/

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


bool UAgentSocketComponent::SendStepResponse()
{
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString<FStepResponse>(Environment->Response, Json);

	// Reset Responses
	Environment->Response.Reward.value = Properties.RewardDefaultValue;

	return TCPSocket->SendMessage(Json);
}


void UAgentSocketComponent::OnUpdateStream(TArray<uint8> CompressedBitmap)
{
	Environment->SetScreenShot(CompressedBitmap);
}
