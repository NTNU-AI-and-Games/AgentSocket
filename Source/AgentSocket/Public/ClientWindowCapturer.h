// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CaptureMachine.h"
#include "ClientWindowCapturer.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientWindowCapturerChangeTexture, UTexture2D*, NewTexture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientWindowCapturerUpdateStream, TArray<uint8>, CompressedBitmap);


UCLASS(BlueprintType, Blueprintable)
class AGENTSOCKET_API UClientWindowCapturer : public UObject
{
	GENERATED_BODY()
		int counter = 0;

protected:
	void BeginDestroy();

public:
	UClientWindowCapturer(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = ClientWindowCapturer)
		UTexture2D* Start();

	UFUNCTION()
		void OnChangeTexture(UTexture2D* NewTexture);

	UFUNCTION()
		void OnUpdateStream(TArray<uint8> CompressedBitmap);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
		FCaptureMachineProperties Properties;

	UPROPERTY(BlueprintReadOnly, Category = SceneCapture)
		class UTexture2D* TextureTarget;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
		FClientWindowCapturerChangeTexture ChangeTexture;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
		FClientWindowCapturerUpdateStream UpdateStream;


protected:
	UPROPERTY(Transient)
		UCaptureMachine* CaptureMachine = nullptr;
};