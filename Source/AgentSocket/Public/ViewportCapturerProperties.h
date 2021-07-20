#pragma once

#include "CoreMinimal.h"
#include "ViewportCapturerProperties.generated.h"


UENUM(BlueprintType)
enum class EColorFormat : uint8
{
	RGBA,
	GRAY,
};



USTRUCT(BlueprintType)
struct AGENTSOCKET_API FViewportCapturerProperties
{
public:
	GENERATED_USTRUCT_BODY()

	// Select which color format for compression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapturer)
		EColorFormat ColorFormatCompression = EColorFormat::GRAY;

	// Number of times per second. This is maximum capture framerate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapturer)
		int32 FrameRate = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapturer)
		bool bShouldCheckWindowResize = false;

	// True will use average filtering upon the image as it scales
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapturer)
		bool bUseAverageFilterForScale = false;

	// 1.0 will keep the original image size, without scaling. 0.5 scales the images to 50% of its original size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShouldScale", ClampMin = "0.001", ClampMax = "1.0"))
		float ScaleMultiplier = 1.0;

};
