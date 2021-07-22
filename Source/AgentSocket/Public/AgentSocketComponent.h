#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "JsonObjectConverter.h"
#include "TCPSocket.h"
#include "AgentAction.h"
#include "AgentResponse.h"
#include "StateResponse.h"
#include "RewardResponse.h"
#include "AgentSocketProperties.h"
#include "AgentEnvironment.h"
#include "ViewportCapturer.h"

#include "AgentSocketComponent.generated.h"


UCLASS(BlueprintType, ClassGroup = (AgentSocket), meta = (BlueprintSpawnableComponent))
class AGENTSOCKET_API UAgentSocketComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAgentSocketComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintReadOnly) // Setting UPROPERTY is needed to prevent UObject being garbage collected automatically
		UTCPSocket* TCPSocket;

	UPROPERTY(BlueprintReadOnly) // Setting UPROPERTY is needed to prevent UObject being garbage collected automatically
		UAgentEnvironment* Environment;

	UPROPERTY(BlueprintReadOnly) // Setting UPROPERTY is needed to prevent UObject being garbage collected automatically
		UAgentActionHandler* AgentActionHandler;

	UPROPERTY(BlueprintReadOnly) // Setting UPROPERTY is needed to prevent UObject being garbage collected automatically
		UViewportCapturer* ViewportCapturer;

	UFUNCTION()
	virtual void OnMessageReceived(FString Message);

	UFUNCTION(BlueprintCallable, Category = "Agent Socket")
		bool RespondSuccess();
	UFUNCTION(BlueprintCallable, Category = "Agent Socket")
		bool RespondError(FString ErrorMessage = "");

	UFUNCTION(BlueprintCallable, Category = "Agent Socket")
		void AddReward(int value);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agent Socket")
		FAgentSocketProperties Properties;

public:

	UFUNCTION(BlueprintCallable, Category = "Agent Socket")
		bool RunActions();

	UFUNCTION(BlueprintCallable, Category = "Agent Socket")
		bool SendStepResponse();


	UFUNCTION(BlueprintCallable, Category = "Agent Socket")
		bool RunSomething();

	UFUNCTION()
		void OnUpdateStream(const TArray<uint8>& CompressedBitmap);
	
};
