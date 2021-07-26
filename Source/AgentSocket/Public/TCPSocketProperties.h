#pragma once

#include "Core.h"
#include "TCPSocketProperties.generated.h"


USTRUCT(BlueprintType)
struct AGENTSOCKET_API FTCPSocketProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString IP = "0.0.0.0";

	// Default Port to use, It will take next (+1) available port if the port is not available
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Port = 11111;
};
