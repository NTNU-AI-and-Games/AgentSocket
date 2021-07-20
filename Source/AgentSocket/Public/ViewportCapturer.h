#pragma once

#include "Core.h"
#include "UObject/NoExportTypes.h"
#include "ViewportCapturerProperties.h"

#include "Async/Async.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#include "IAssetViewport.h"
#endif

#include "FrameGrabber.h"

#include <Runtime/Engine/Public/ImageUtils.h>
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#include "ViewportCapturer.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImageReady, const TArray<uint8>&, Compressed);


UCLASS()
class AGENTSOCKET_API UViewportCapturer : public UObject
{
	GENERATED_BODY()

protected:
	void BeginDestroy();

public:
	bool StartFrameGrab();
	bool TickGrab();
	bool ReleaseFrameGrabber();
	
	bool CompressImage(
		const void* InUncompressedData,
		const int64 RawSize,
		const int32 InWidth,
		const int32 InHeight,
		TArray<uint8>& OutCompressedData,
		ERGBFormat ColFormat,
		EImageFormat ImgFormat);

	// This has to be set before start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapturer)
		FViewportCapturerProperties Properties;

	UPROPERTY(BlueprintAssignable)
		FOnImageReady OnImageReady;

private:
	TSharedPtr<FFrameGrabber> FrameGrabber;
	double PrevCaptureTime;
	//TSharedPtr<FSceneViewport> SceneViewport;
	TWeakPtr<FSceneViewport> SceneViewport;

	bool CompressImageScaled(const void* InUncompressedData, const int64 RawSize, const FIntPoint BufferSize, TArray<uint8>& OutCompressedData, ERGBFormat ColFormat, EImageFormat ImgFormat);
	bool CompressImageGray(const TArray<FColor> &InData, const FIntPoint BufferSize, TArray<uint8>& OutData);
	bool CompressImageRGBA(const TArray<FColor>& InData, const FIntPoint BufferSize, TArray<uint8>& OutData);
};
