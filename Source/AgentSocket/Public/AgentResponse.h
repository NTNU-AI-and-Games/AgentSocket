#pragma once

#include "CoreMinimal.h"
#include "AgentResponse.generated.h"


UENUM(BlueprintType)
enum class EResponseType : uint8
{
	ACK,	// Acknowledgement for the sent command or query
	STATE,
	REWARD,
	STEP,
};
