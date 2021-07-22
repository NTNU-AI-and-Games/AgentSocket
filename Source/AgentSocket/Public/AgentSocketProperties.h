// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViewportCapturerProperties.h"
#include "TCPSocketProperties.h"
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

	// Settings for the viewport capturer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FViewportCapturerProperties ViewportCapturer;

	// Settings for the tcp socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TCP Socket"))
		FTCPSocketProperties TCPSocket;
};
