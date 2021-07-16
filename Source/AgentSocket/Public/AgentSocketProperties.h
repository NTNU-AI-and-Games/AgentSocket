// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CaptureMachineProperties.h"
#include "AgentSocketProperties.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct AGENTSOCKET_API FAgentSocketProperties
{
	GENERATED_BODY()

	/** The title of the window you want to capture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RewardDefaultValue = -1;

	// Settings of the window capturer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCaptureMachineProperties Capturer;


};
