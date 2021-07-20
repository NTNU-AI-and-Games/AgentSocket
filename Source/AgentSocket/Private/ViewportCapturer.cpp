#pragma once

#include "ViewportCapturer.h"


void UViewportCapturer::BeginDestroy()
{
	Super::BeginDestroy();
	ReleaseFrameGrabber();
}


bool UViewportCapturer::ReleaseFrameGrabber()
{
	if (FrameGrabber.IsValid())
	{
		FrameGrabber->Shutdown();
		FrameGrabber = nullptr;
	}
	return true;
}


bool UViewportCapturer::StartFrameGrab() {
// (Inpired by RemoteHost plugin)
#if WITH_EDITOR
	if (GIsEditor) {
		for (const FWorldContext& Context : GEngine->GetWorldContexts()) {
			if (Context.WorldType == EWorldType::PIE) {
				FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
				if (SlatePlayInEditorSession) {
					if (SlatePlayInEditorSession->DestinationSlateViewport.IsValid()) {
						TSharedPtr<IAssetViewport> AssetViewport = SlatePlayInEditorSession->DestinationSlateViewport.Pin();
						SceneViewport = AssetViewport->GetSharedActiveViewport();
					}
					else if (SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.IsValid()) {
						SceneViewport = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport;
					}
				}
			}
		}
	}
	else
#endif
	{
		UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
		SceneViewport = GameEngine->SceneViewport;
	}
	if (!SceneViewport.IsValid()) {
		return false;
	}

	// Capture Start
	ReleaseFrameGrabber();
	FrameGrabber = MakeShared<FFrameGrabber>(SceneViewport.Pin().ToSharedRef(), SceneViewport.Pin()->GetSize());
	FrameGrabber->StartCapturingFrames();

	return true;
}


bool UViewportCapturer::CompressImage(
	const void* InUncompressedData,
	const int64 RawSize,
	const int32 InWidth,
	const int32 InHeight,
	TArray<uint8>& OutCompressedData,
	ERGBFormat ColFormat,
	EImageFormat ImgFormat)
{
	OutCompressedData.Reset();
	if (RawSize > 0)
	{
		IImageWrapperModule& ImgWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImgWrapperModule.CreateImageWrapper(ImgFormat);
		if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(InUncompressedData, RawSize, InWidth, InHeight, ColFormat, 8))
		{
			OutCompressedData = ImageWrapper->GetCompressed();
			return true;
		}
	}
	return false;
}


bool UViewportCapturer::CompressImageScaled(const void* InUncompressedData, const int64 RawSize, const FIntPoint BufferSize, TArray<uint8>& OutCompressedData, ERGBFormat ColFormat, EImageFormat ImgFormat)
{
	int32 Width = BufferSize.X;
	int32 Height = BufferSize.Y;
	
	return CompressImage(InUncompressedData, RawSize, Width, Height, OutCompressedData, ColFormat, ImgFormat);
}


bool UViewportCapturer::CompressImageGray(const TArray<FColor>& ColorData, const FIntPoint Size, TArray<uint8>& OutData)
{
	TArray<uint8> Grayscale;

	int32 ImgWidth = Size.X;
	int32 ImgHeight = Size.Y;
	int32 Area = ImgWidth * ImgHeight;

	Grayscale.AddUninitialized(Area);
	for (int32 i = 0; i < Area; i++)
	{
		const FColor& Color = ColorData[i];
		Grayscale[i] = (uint8)(Color.R * 0.299 + Color.G * 0.587 + Color.B * 0.114);
	}
	return CompressImageScaled(&Grayscale[0], Grayscale.Num(), Size, OutData, ERGBFormat::Gray, EImageFormat::PNG);
}


bool UViewportCapturer::CompressImageRGBA(const TArray<FColor>& InData, const FIntPoint BufferSize, TArray<uint8>& OutData)
{
	return CompressImageScaled(InData.GetData(), InData.GetAllocatedSize(), BufferSize, OutData, ERGBFormat::BGRA, EImageFormat::PNG);
}


bool UViewportCapturer::TickGrab()
{
	if (FrameGrabber.IsValid()) {
		FrameGrabber->CaptureThisFrame(FFramePayloadPtr());
		TArray<FCapturedFrameData> Frames = FrameGrabber->GetCapturedFrames();
		if (Frames.Num()) {
			const double ElapsedFrameTime = (FPlatformTime::Seconds() - PrevCaptureTime) * 1000;
			const int32 TargetFrameTime = 1000 / Properties.FrameRate;

			if (ElapsedFrameTime >= TargetFrameTime)
			{
				FCapturedFrameData& LastFrame = Frames.Last();

				TArray<FColor> ColorData = MoveTemp(LastFrame.ColorBuffer);
				FIntPoint Size = LastFrame.BufferSize;

				// Run compression on another thread to prevent lag
				AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this, Size, ColorData = MoveTemp(ColorData)]() mutable
				{
					TArray<uint8> CompressedColor;
					if (Properties.ColorFormatCompression == EColorFormat::GRAY) {
						if (CompressImageGray(ColorData, Size, CompressedColor)) {
							OnImageReady.Broadcast(CompressedColor);
						}
					}
					else if (Properties.ColorFormatCompression == EColorFormat::RGBA) {
						if (CompressImageRGBA(ColorData, Size, CompressedColor)) {
							OnImageReady.Broadcast(CompressedColor);
						}
					}
				});
				

				PrevCaptureTime = FPlatformTime::Seconds();
			}
		}
	}
	return true;
}

