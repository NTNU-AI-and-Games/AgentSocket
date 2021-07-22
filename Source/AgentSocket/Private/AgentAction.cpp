#include "AgentAction.h"


bool UAgentActionHandler::ParseMessage(FString Message)
{
	FAgentActions _Actions;

	// Automatically convert json to struct
	// Note: It matches enums with string (not int)
	if (FJsonObjectConverter::JsonObjectStringToUStruct<FAgentActions>(*Message, &_Actions, 0, 0)) {
		Actions = _Actions;
	}
	else {
		UE_LOG(LogSockets, Error, TEXT("AgentSocket>> JSON payload is not properly formatted"));
		return false;
	}
	return true;
}


bool UAgentActionHandler::Initialize(UPlayerInput* InPlayerInput)
{
	PlayerInput = InPlayerInput;
	return true;
}


bool UAgentActionHandler::RunActions()
{
	// Run all actions that is stored in the Actions property
	for (int i = Actions.Actions.Num(); i-- > 0;) {
		FAgentAction Action = Actions.Actions[i];
		RunAction(Action);
		
		// If the action should call only once, remove it after
		if (Action.State == EAgentActionState::ONCE) {
			Actions.Actions.RemoveAt(i);
		}
	}
	return true;
}


bool UAgentActionHandler::RunAction(const FAgentAction &Action)
{
	if (Action.Type == EAgentActionType::ACTION) {
		// Get the first key hook of the action with matching name, and input the key
		if (PlayerInput->GetKeysForAction(Action.Name).Num() > 0) {
			FInputActionKeyMapping mapping = PlayerInput->GetKeysForAction(Action.Name)[0];
			FKey key = mapping.Key;

			if (mapping.bShift) {
				PlayerInput->InputKey(EKeys::LeftShift, EInputEvent::IE_Pressed, 1, 0);
			}
			if (mapping.bCtrl) {
				PlayerInput->InputKey(EKeys::LeftControl, EInputEvent::IE_Pressed, 1, 0);
			}
			if (mapping.bAlt) {
				PlayerInput->InputKey(EKeys::LeftAlt, EInputEvent::IE_Pressed, 1, 0);
			}
			if (mapping.bCmd) {
				PlayerInput->InputKey(EKeys::LeftCommand, EInputEvent::IE_Pressed, 1, 0);
			}
			PlayerInput->InputKey(key, EInputEvent::IE_Pressed, 1, 0);
		}
	} else if (Action.Type == EAgentActionType::AXIS) {
		if (Action.Key != EKeys::Invalid) {
			// TODO: We may want to change the "deltaTime" to be equal to frame delta time
			PlayerInput->InputAxis(Action.Key, FCString::Atof(*Action.Value), 0.05, 1, false);
		}
	} else if (Action.Type == EAgentActionType::KEY) {
		if (Action.Key != EKeys::Invalid) {
			PlayerInput->InputKey(Action.Key, EInputEvent::IE_Pressed, 1, 0);
		}
	}
	return true;
}


void UAgentActionHandler::BeginDestroy()
{
	Super::BeginDestroy();
}
