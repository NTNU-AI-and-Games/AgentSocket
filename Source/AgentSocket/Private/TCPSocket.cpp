// Inspired by https://forums.unrealengine.com/t/tcp-socket-listener-receiving-binary-data-into-ue4-from-a-python-script/7549/55

#include "TCPSocket.h"
#include <string>


UTCPSocket::UTCPSocket()
{
	if (GetOuter() == nullptr || GetOuter()->GetWorld() == nullptr) return;
	World = GetOuter()->GetWorld();
}


uint8 UTCPSocket::ID = 0;


void UTCPSocket::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogTemp, Warning, TEXT("TCPSocket>> Destroyed (port:%d)"), Properties.Port);
	if (GEngine != nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("TCP_Socket>> HUH")));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("TCP_Socket>> Destroyed (port:%d)"), Port));
	}
	if (World != nullptr) {
		FTimerManager& ftm = World->GetTimerManager();
		ftm.ClearTimer(TCPConnectionListenerTimerHandle);
		ftm.ClearTimer(TCPSocketListenerTimerHandle);
	}

	if (ConnectionSocket != NULL) ConnectionSocket->Close();
	if (ListenerSocket != NULL) ListenerSocket->Close();
}


// TCP Server Code
bool UTCPSocket::LaunchTCP()
{
	// Run only on clients; will NOT run in single player / standalone
	if (GEngine->GetNetMode(World) == NM_Client)
	{
		UE_LOG(LogSockets, Warning, TEXT("TCPSocket>> Creating TCP socket at port %d, time: %s"), Properties.Port, *(FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H:%M:%S.%s"))));
		if (!StartTCPReceiver("TCP Socket")) {
			UE_LOG(LogTemp, Warning, TEXT("TCPSocket>> Failed to create TCP socket"));
			return false;
		}
		return true;
	}
	return false;
}


// Start TCP Receiver
bool UTCPSocket::StartTCPReceiver(const FString& SocketName) {
	// CreateTCPConnectionListener
	ListenerSocket = CreateTCPConnectionListener(SocketName);

	if (!ListenerSocket)
	{
		// Possible cause: port number is already taken.
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("TCPSocket>> Listen socket could not be created! ~> %s %d"), *Properties.IP, Properties.Port));
		return false;
	}

	// Start the Listener
	World->GetTimerManager().SetTimer(TCPConnectionListenerTimerHandle, this, &UTCPSocket::TCPConnectionListener, 1.0f, true);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("TCPSocket>> TCP socket created at port %d"), Properties.Port));
	return true;
}


// Format IPv4 String as Number Parts
// Example "192.68.1.1" -> [192, 68, 1, 1]
bool UTCPSocket::FormatIP4ToNumber(const FString& _IP, uint8(&Out)[4])
{
	// IP Formatting
	_IP.Replace(TEXT(" "), TEXT(""));

	// String Parts
	TArray<FString> Parts;
	_IP.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() != 4)
		return false;

	// String to Number Parts
	for (int32 i = 0; i < 4; ++i)
	{
		Out[i] = FCString::Atoi(*Parts[i]);
	}

	return true;
}


// Create TCP Connection Listener
FSocket* UTCPSocket::CreateTCPConnectionListener(const FString& SocketName, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(Properties.IP, IP4Nums)) return false;

	// Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), Properties.Port);
	FSocket* ListenSocket = FTcpSocketBuilder(*SocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	// Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	return ListenSocket;
}


// TCP Connection Listener
void UTCPSocket::TCPConnectionListener()
{
	if (!ListenerSocket) return;

	// Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// Handle incoming connections
	ListenerSocket->HasPendingConnection(Pending);

	if (Pending)
	{
		// Already have a Connection? destroy previous
		if (ConnectionSocket)
		{
			ConnectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}

		// New Connection receive
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("TCPSocket>> Received socket connection"));


		if (ConnectionSocket != NULL)
		{
			// Global cache of current Remote Address
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);

			World->GetTimerManager().SetTimer(TCPSocketListenerTimerHandle, this, &UTCPSocket::TCPSocketListener, 0.1f, true);
		}
	}
}


// String From Binary Array
FString UTCPSocket::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	// Create a string from a byte array
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}


bool UTCPSocket::SendMessage(FString ToSend) {
	UE_LOG(LogSockets, Error, TEXT("josn2: %s"), *ToSend);
	ToSend = ToSend + LINE_TERMINATOR; // For Matlab we need a defined line break (fscanf function) " " ist not working, therefore use the LINE_TERMINATOR macro form UE

	TCHAR * SerializedChar = ToSend.GetCharArray().GetData();
	int32 Size = FCString::Strlen(SerializedChar);
	int32 Sent = 0;
	uint8* ResultChars = (uint8*)TCHAR_TO_UTF8(SerializedChar);
	if (ConnectionSocket) {
		return (!ConnectionSocket->Send(ResultChars, Size, Sent));
	}
	else {
		UE_LOG(LogSockets, Verbose, TEXT("TCPSocket>> No Client is connected. Send is not performed"));
		return false;
	}
}


bool UTCPSocket::SendBinary(TArray<uint8> &BinaryArray) {
	int32 Sent = 0;
	uint8* ResultChars = BinaryArray.GetData();
	if (ConnectionSocket) {
		return (!ConnectionSocket->Send(ResultChars, BinaryArray.Num(), Sent));
	}
	else {
		UE_LOG(LogSockets, Verbose, TEXT("TCPSocket>> No Client is connected. Send is not performed"));
		return false;
	}
}


void UTCPSocket::TCPSocketListener()
{
	if (!ConnectionSocket) return;

	// Binary Array
	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(FMath::Min(Size, 65507u), Size);

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}

	if (ReceivedData.Num() <= 0) return;

	// String From Binary Array
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	MessageReceived(ReceivedUE4String);
};



void UTCPSocket::MessageReceived(const FString& Message)
{
	UE_LOG(LogSockets, Warning, TEXT("TCPSocket>> (Port: %d) Message received at %s"), Properties.Port, *(FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H:%M:%S.%s"))));
	OnMessageReceived.Broadcast(Message);
}
