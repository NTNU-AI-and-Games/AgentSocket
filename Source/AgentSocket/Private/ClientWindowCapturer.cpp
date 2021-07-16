// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientWindowCapturer.h"

#include "Engine/Texture2D.h"



UClientWindowCapturer::UClientWindowCapturer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UClientWindowCapturer::BeginDestroy()
{
	Super::BeginDestroy();
	//FlushRenderingCommands();

	if (CaptureMachine)
	{
		CaptureMachine->Close();
		CaptureMachine = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("ClientWindowCaputrer>> Destoryed"));
	UE_LOG(LogTemp, Warning, TEXT("ClientWindowCaputrer>> %d"), counter);
}

UTexture2D* UClientWindowCapturer::Start()
{
	if (CaptureMachine)
	{
		CaptureMachine->Close();
	}

	CaptureMachine = NewObject<UCaptureMachine>(this);

	CaptureMachine->Properties = Properties;

	CaptureMachine->ChangeTextureSize.AddDynamic(this, &UClientWindowCapturer::OnChangeTexture);
	CaptureMachine->OnUpdateStream.AddDynamic(this, &UClientWindowCapturer::OnUpdateStream);
	CaptureMachine->Start();

	return CaptureMachine->CreateTexture();
}

void UClientWindowCapturer::OnChangeTexture(UTexture2D* _NewTexture)
{
	counter++;
	UE_LOG(LogTemp, Warning, TEXT("NewTexture"));
	ChangeTexture.Broadcast(_NewTexture);
}


void UClientWindowCapturer::OnUpdateStream(TArray<uint8> CompressedBitmap)
{
	counter++;
	UE_LOG(LogTemp, Warning, TEXT("Update stream"));

	UpdateStream.Broadcast(CompressedBitmap);
}
