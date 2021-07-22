// Inspired by https://github.com/Radiateurs/Socket-implementation-UE4
// and https://forums.unrealengine.com/t/tcp-socket-listener-receiving-binary-data-into-ue4-from-a-python-script/7549

#pragma once
#include "UObject/Object.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "TCPSocketProperties.h"
#include "TCPSocket.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, FString, Message);


UCLASS(BlueprintType)
class AGENTSOCKET_API UTCPSocket : public UObject
{
	GENERATED_BODY()

public:
	UTCPSocket();

	static uint8 ID;

	// This has to be set before start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TCPSocket)
		FTCPSocketProperties Properties;

	UPROPERTY(BlueprintAssignable)
		FOnMessageReceived OnMessageReceived;

protected:
	void BeginDestroy();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWorld* World;

	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	FTimerHandle TCPSocketListenerTimerHandle;
	FTimerHandle TCPConnectionListenerTimerHandle;

	bool StartTCPReceiver(const FString& SocketName);

	FSocket* CreateTCPConnectionListener(
		const FString& SocketName,
		const int32 ReceiveBufferSize = 2 * 1024 * 1024
	);

	// Timer functions, could be threads
	void TCPConnectionListener();

	void TCPSocketListener();

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

	//Format String IP4 to number array
	bool FormatIP4ToNumber(const FString& IP, uint8(&Out)[4]);

	UFUNCTION(BlueprintCallable, Category = "TCP Socket")
		bool LaunchTCP();

	UFUNCTION(BlueprintCallable, Category = "TCP Socket")
		bool SendMessage(FString Message);

	// Send raw bytes to connected client
	UFUNCTION(BlueprintCallable, Category = "TCP Socket")
		bool SendBinary(TArray<uint8> &BinaryArray);

	UFUNCTION(/*BlueprintImplementableEvent, */Category = "TCP Socket")
		void MessageReceived(const FString& Message);

};
