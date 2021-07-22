#pragma once

#include "CoreMinimal.h"
#include "AgentRequest.h"
#include "JsonObjectConverter.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Networking.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "AgentAction.generated.h"


UENUM(BlueprintType)
enum class EAgentActionType : uint8
{
	ACTION,		// Corresponds to Action mappings
	AXIS,		// Corresponds to Axis mappings
	KEY,		//
};


UENUM(BlueprintType)
enum class EAgentActionState : uint8
{
	ONCE,		// Run the action for one frame
	PRESSED,		// Run the action continuously, until release
	RELEASED,	// Stop running a held command
};


USTRUCT(BlueprintType)
struct AGENTSOCKET_API FAgentAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		EAgentActionType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		EAgentActionState State = EAgentActionState::ONCE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		FName Name = "";

	// The keys has to be passed in as shown in https://docs.unrealengine.com/4.26/en-US/API/Runtime/InputCore/EKeys/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		FKey Key = EKeys::Invalid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		FString Value = "";
};


USTRUCT(BlueprintType)
struct AGENTSOCKET_API FAgentActions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		EAgentRequestType Type = EAgentRequestType::ACTION;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		TArray<FAgentAction> Actions;
};


UCLASS(BlueprintType, ClassGroup = (AgentSocket))
class AGENTSOCKET_API UAgentActionHandler : public UObject
{
	GENERATED_BODY()

public:
	UAgentActionHandler() {};
	bool Initialize(UPlayerInput* InPlayerInput);

protected:
	void BeginDestroy();
	UPlayerInput* PlayerInput;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		FAgentActions Actions;

	UFUNCTION()
		bool RunActions();
	UFUNCTION()
		bool RunAction(const FAgentAction &Action);
	UFUNCTION()
		bool ParseMessage(FString Message);

};
