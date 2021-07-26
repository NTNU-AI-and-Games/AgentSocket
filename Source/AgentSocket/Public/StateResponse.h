#pragma once

#include "CoreMinimal.h"
#include "AgentResponse.h"
#include "StateResponse.generated.h"


// A list of update items to update the agent environment
USTRUCT(BlueprintType)
struct AGENTSOCKET_API FKeyValuePair
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AgentSocket|Reward")
		FString Key = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Update")
		FString Value = "";
};


// A list of update items to update the agent environment
USTRUCT(BlueprintType)
struct AGENTSOCKET_API FStateResponse
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AgentSocket|Reward")
		bool GameOver = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Update")
		bool GameReset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentSocket|Update")
		TArray<FKeyValuePair> Other;
};
