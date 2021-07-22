#include "AgentEnvironment.h"
#include <Runtime/Engine/Public/ImageUtils.h>


void UAgentEnvironment::BeginDestroy()
{
	Super::BeginDestroy();
}


int UAgentEnvironment::GetReward()
{
	UE_LOG(LogTemp, Warning, TEXT("value is: %s"), *FString(Response.State.value));
	return 3214;
}


void UAgentEnvironment::SetScreenShot(const TArray<uint8>& CompressedBitmap)
{
	//UE_LOG(LogTemp, Error, TEXT("After ss, time: %s"), *(FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H:%M:%S.%s"))));
	ImageResponse = CompressedBitmap;
	return;
}
