// Copyright 2019 ayumax. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CaptureMachineProperties.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include <WinUser.h>
#else
using HBITMAP = void*;
using HDC = void*;
using HWND = void*;
#endif
#include "CaptureMachine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureMachineChangeTextureSize, UTexture2D*, NewTexture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureMachineUpdateStream, TArray<uint8>, CompressedBitmap);


UCLASS(BlueprintType, Blueprintable)
class AGENTSOCKET_API UCaptureMachine : public UObject
{
	GENERATED_BODY()
	
public:	
	UCaptureMachine();

	virtual void Start();
	virtual void Close();

	UFUNCTION(BlueprintPure, Category = WindowCapture2D)
	UTexture2D* CreateTexture();

	TArray<FColor> GetBitmapBufferCopy();

protected:
	bool FindTargetWindow(HWND hWnd);
	void UpdateTexture();
	void GetWindowSize(HWND hWnd);
	void ReCreateTexture();
	bool DoCapture();


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	FCaptureMachineProperties Properties;

	UPROPERTY(BlueprintReadOnly, Category = SceneCapture)
	class UTexture2D* TextureTarget;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
		FCaptureMachineChangeTextureSize ChangeTextureSize;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
		FCaptureMachineUpdateStream OnUpdateStream;

private:
	char* m_BitmapBuffer = nullptr;

	HBITMAP m_hBmp = nullptr;
	HDC m_MemDC = nullptr;
	HBITMAP m_hOriginalBmp = nullptr;
	HDC m_OriginalMemDC = nullptr;
	HWND m_TargetWindow = nullptr;

	FIntVector2D m_WindowSize;
	FIntVector2D m_OriginalWindowSize;
	FIntVector2D m_WindowOffset;

	class FWCWorkerThread* CaptureWorkerThread = nullptr;
	class FRunnableThread* CaptureThread = nullptr;
};
