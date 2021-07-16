#pragma once

#include "CoreMinimal.h"
#include "AgentResponse.h"
#include "StateResponse.generated.h"


UENUM(BlueprintType)
enum class EStateType : uint8
{
	PLAYER,				// Player property change (such as health change, weapon swap etc)
	PLAYER_SPAWN,		// Player spawns at location
	PLAYER_DEATH,		// Player dead
	//PLAYER_CHANGE_TEAM,	// If team change is possible in game
	ITEM_CREATED,		// Building/Tree/Platform... is created
	ITEM_DESTROYED,		// Building/Tree/Platform... is destroyed
	GRENADE_NEARBY,
};


// A list of update items to update the agent environment
USTRUCT(BlueprintType)
struct AGENTSOCKET_API FStateResponse
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AgentSocket|Reward")
		EResponseType Type = EResponseType::STATE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Update")
		EStateType StateType = EStateType::PLAYER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Update")
		FString value = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Update")
		TArray<uint8> imageValue;
};
