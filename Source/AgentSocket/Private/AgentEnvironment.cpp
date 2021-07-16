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


bool SaveBitmapAsPNG(int32 sizeX, int32 sizeY, const TArray<FColor>& bitmapData, const FString& filePath) {
	TArray<uint8> compressedBitmap;
	FImageUtils::CompressImageArray(sizeX, sizeY, bitmapData, compressedBitmap);
	return FFileHelper::SaveArrayToFile(compressedBitmap, *filePath);
}


void UAgentEnvironment::SetScreenShot(TArray<uint8> CompressedBitmap)
{
	Response.State.imageValue = CompressedBitmap;
	return;
}
