#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateResponse.h"
#include "RewardResponse.h"
#include "AgentResponse.h"
#include "AgentEnvironment.generated.h"


// A list of update items to update the agent environment
USTRUCT(BlueprintType)
struct AGENTSOCKET_API FStepResponse
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Agent Socket")
		EResponseType Type = EResponseType::STEP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent Socket")
		FStateResponse State;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent Socket")
		FRewardResponse Reward;
};


UCLASS(BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGENTSOCKET_API UAgentEnvironment : public UObject
{
	GENERATED_BODY()
	
public:
	void SetState();
	void SetScreenShot(const TArray<uint8>& CompressedBitmap);
	void ResetReward();

	// Should return the a threadsafe response
	FStepResponse Response;
	TArray<uint8> ImageResponse;

protected:
	void BeginDestroy();
};
