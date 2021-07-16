// Inspired by https://github.com/Radiateurs/Socket-implementation-UE4
// and https://forums.unrealengine.com/t/tcp-socket-listener-receiving-binary-data-into-ue4-from-a-python-script/7549

#pragma once
#include "UObject/Object.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "TCPSocket.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, FString, Message);


UCLASS(BlueprintType)
class AGENTSOCKET_API UTCPSocket : public UObject
{
	GENERATED_BODY()

public:
	UTCPSocket();

	static uint8 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Socket")
		FString IP = "127.0.0.1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCP Socket")
		int Port = 11111;

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

	UFUNCTION(/*BlueprintImplementableEvent, */Category = "TCP Socket")
		void MessageReceived(const FString& Message);

};


//
//typedef struct	Fs_SocketContent
//{
//	FSocket* ConnectSocket;
//	FIPv4Endpoint	Address;
//	FString		Buffer;
//	bool		NewMessage;
//}				FSocketContent;
//
//
//UENUM()
//enum FCommand
//{
//	PAUSE = 0,
//	UNPAUSE = 1,
//	MOVE = 2,
//	SAY = 3,
//	QUIT = 4,
//	UNKNOWN
//};
//
//
//
//UCLASS()
//class MYPROJECT2_API ATest_Socket : public AActor
//{
//	GENERATED_BODY()
//	
//public:	
//	// Sets default values for this actor's properties
//	ATest_Socket();
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
//		TArray<FString> BufferList;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notifyer")
//		bool	NewMessage;
//	TArray<FSocketContent>	SocketList;
//	FSocket* ListenerSocket;
//	FSocket* ConnectSocket;
//	FIPv4Endpoint	RemoteAddress;
//
//	bool	FormatIP4ToNumber(const FString& TheIp, uint8(&Out)[4]);
//	FSocket* CreateTCPConnectionListener(const FString& SocketName, const FString& TheIp, uint32 ThePort, uint32 BufferSize = 2 * 1024 * 1024);
//	bool	StartTCPReceiver(const FString& SocketName, const FString& TheIp, uint32 ThePort);
//	void	TCPConnectionListener();
//	FString	StringFromBinaryArray(const TArray<uint8>& BinArray);
//	FString	GetCommandWithoutID(const FString& FullMessage);
//
//	// Sets default values for this pawn's properties
//	ATest_Socket();
//
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	// Called when the game ends
//	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
//
//	// Called every frame
//	virtual void Tick(float DeltaSeconds) override;
//
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
//
//	UFUNCTION(BlueprintCallable, Category = "Online")
//		bool	SendStringToClient(int32 ClientID, const FString MessageToSend);
//	UFUNCTION(BlueprintCallable, Category = "Online")
//		int32	GetClientIDFromLastMessage(const FString LastMessage);
//	UFUNCTION(BlueprintCallable, Category = "Parser")
//		bool	AskPause(const FString LastMessage);
//	UFUNCTION(BlueprintCallable, Category = "Init")
//		bool	CreateSocketConnection(const FString& SocketName, const FString& TheIP, int32 ThePort);
//	UFUNCTION(BlueprintCallable, Category = "Parser")
//		FCommand	ParseReceivedMessage(const FString& Message);
//	UFUNCTION(BlueprintCallable, Category = "Online")
//		void	TCPSocketListener();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//};
