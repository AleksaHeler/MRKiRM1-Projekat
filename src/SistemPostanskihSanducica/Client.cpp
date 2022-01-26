#include "Client.h"

#define StandardMessageCoding 0x00


//////////////////////////////////////////////////////////////////////////////////////
/// FINITE STATE MACHINE /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// Constructor/destructor
Client::Client() : FiniteStateMachine(CLIENT_TYPE_ID, CLIENT_MBX_ID, 0, 6, 2) { }
Client::~Client() { }

// Getters
uint8 Client::GetAutomate() { return CLIENT_TYPE_ID; }
uint8 Client::GetMbxId() { return CLIENT_MBX_ID; }
uint32 Client::GetObject() { return GetObjectId(); }
MessageInterface* Client::GetMessageInterface(uint32 id) { return &StandardMsgCoding; }

// Setters
void Client::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}
void Client::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}
void Client::NoFreeInstances() {
	printf("[%d] Node::NoFreeInstances()\n", GetObjectId());
}
void Client::Reset() {
	printf("[%d] Node::Reset()\n", GetObjectId());
}


//////////////////////////////////////////////////////////////////////////////////////
/// INITIALIZATIONS //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void Client::Initialize() {
	SetState(ClientState_Idle);
	InitSocket();
	
	// Set state transition functions and triggers (messages)
	InitEventProc(ClientState_Connecting, ServerMSG_LoginOk, (PROC_FUN_PTR)&Client::Client_LoginOK);
	InitEventProc(ClientState_Connecting, ServerMSG_LoginError, (PROC_FUN_PTR)&Client::Client_LoginError);
	InitEventProc(ClientState_Disconnecting, ServerMSG_LogoutOk, (PROC_FUN_PTR)&Client::Client_Disconnect);
	InitEventProc(ClientState_CheckMail, ServerMSG_CheckMailResponse, (PROC_FUN_PTR)&Client::Client_CheckMailResponse);
	InitEventProc(ClientState_ReceiveMail, ServerMSG_ReceiveMailResponse, (PROC_FUN_PTR)&Client::Client_ReceiveMailResponse);
}


//////////////////////////////////////////////////////////////////////////////////////
/// STATE TRANSITION FUNCTIONS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// State: ClientState_Connecting
//void	Client_LoginOK();				// -> ClientState_Connected
//void	Client_LoginError();			// -> ClientState_Idle

// State: ClientState_Disconnecting
//void	Client_Disconnect();			// -> ClientState_Idle

// State: ClientState_CheckMail
//void	Client_CheckMailResponse();		// -> ClientState_Connected

// State: ClientState_ReceiveMail
//void	Client_ReceiveMailResponse();	// -> ClientState_Connected


//////////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//void SendData(uint8 dst, uint8 len, const char* data);
// void Login();			// ClientState_Idle			->	ClientState_Connecting
// void Logout();			// ClientState_Connected	->	ClientState_Disconnecting
// void SendMail();			// ClientState_Connected	->	ClientState_Connected
// void CheckMail();		// ClientState_Connected	->	ClientState_CheckMail
// void ReceiveMail();		// ClientState_Connected	->	ClientState_ReceiveMail


//////////////////////////////////////////////////////////////////////////////////////
/// NETWORK //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

DWORD Client::TCPListener(LPVOID param) {
	Client* pParent = (Client*)param;
	int nReceivedBytes;

	int slen = sizeof(pParent->serverAddress);

	printf("Node %d UDP listening on port %d\n", pParent->GetObject(), ntohs(pParent->socketAddress.sin_port));

	while(1) {
		nReceivedBytes = recvfrom(pParent->mSocket, pParent->buffer, BUFFER_SIZE, 0, (SOCKADDR*)&pParent->serverAddress, &slen);
		if (nReceivedBytes == 0)
		{
			printf("Recv 0 bytes!\n");
			break;
		}
		if (nReceivedBytes < 0)
		{
			DWORD err = WSAGetLastError();
			printf("Client recieve error: %d\n", err);
			continue;
		}
		pParent->TcpToFsm();
		Sleep(500);
	};

	return 1;
}

void Client::InitSocket() {
	// Create the socket
	mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		printf("Client: socket() failed! Error : %ld\n", WSAGetLastError());
		return;
	}

	// Set server socket address
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	// Bind to port (client port number will be server port num + 1 + clients ID) (IDs start from 0) 
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(SERVER_PORT + GetObject() + 1);
	socketAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	if (bind(mSocket, (SOCKADDR*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR)
	{
		printf("Client: bind() failed! Error : %ld\n", WSAGetLastError());
		closesocket(mSocket);
		return;
	}

	// Then, start the thread that will listen on the the newly created socket
	mhThread = CreateThread(NULL, 0, TCPListener, (LPVOID)this, 0, &mnThreadID);
	if (mhThread == NULL)
	{
		// Cannot create thread
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
		return;
	}
}

void Client::TcpToFsm() {
	printf("[TcpToFsm] Buffer: %s\n", buffer);
	//PrepareNewMessage(0x00, ClientMSG_Login);
	//SetMsgToAutomate(CLIENT_TYPE_ID);
	//SetMsgObjectNumberTo(GetObject());

	//SendMessage(CLIENT_MBX_ID);
}