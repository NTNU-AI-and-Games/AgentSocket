#pragma once

#include "CoreMinimal.h"
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
		EAgentActionState ActionType = EAgentActionState::ONCE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		FName Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket")
		FString Value = "";
};
