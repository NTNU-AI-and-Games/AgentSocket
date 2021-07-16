#pragma once

#include "CoreMinimal.h"
#include "AgentResponse.h"
#include "RewardResponse.generated.h"



// A list of update items to update the agent environment
USTRUCT(BlueprintType)
struct AGENTSOCKET_API FRewardResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AgentSocket|Reward")
		EResponseType Type = EResponseType::REWARD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Reward")
		int value = 0;
};
