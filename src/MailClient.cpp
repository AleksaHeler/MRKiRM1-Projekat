#include "MailClient.h"

#define StandardMessageCoding 0x00


//////////////////////////////////////////////////////////////////////////////////////
/// FINITE STATE MACHINE /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// Constructor/destructor
Client::Client() : FiniteStateMachine(CLIENT_TYPE_ID, CLIENT_MBX_ID, 0, 6, 2) { }
Client::~Client() { CloseSocket(); }

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
	InitEventProc(ClientState_ReceiveMail, ServerMSG_ReceiveMailError, (PROC_FUN_PTR)&Client::Client_ReceiveMailError);
}


//////////////////////////////////////////////////////////////////////////////////////
/// STATE TRANSITION FUNCTIONS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// ClientState_Connecting -> ClientState_Connected
void Client::Client_LoginOK() {
	printf(CLIENT_DEBUG_NAME"Successful login\n");
	SetState(ClientState_Connected);
}

// ClientState_Connecting -> ClientState_Idle
void Client::Client_LoginError() {
	printf(CLIENT_DEBUG_NAME"Login error!\n");
	SetState(ClientState_Idle);
}

// ClientState_Disconnecting -> ClientState_Idle
void Client::Client_Disconnect() {
	printf(CLIENT_DEBUG_NAME"Successful disconnect\n");
	SetState(ClientState_Idle);
}

// ClientState_CheckMail -> ClientState_Connected
void Client::Client_CheckMailResponse() {
	byte mailCount = 0;
	GetParamByte(PARAM_MAIL_COUNT, mailCount);
	printf(CLIENT_DEBUG_NAME"Inbox: %d unread messages\n", mailCount);
	SetState(ClientState_Connected);
}

// ClientState_ReceiveMail -> ClientState_Connected
void Client::Client_ReceiveMailResponse() {
	std::string str(buffer);																// Create string from array
	str = str.erase(0, 1);																	// Erase command text

	std::string subject = str.substr(0, str.find(MESSAGE_SPLIT_TOKEN));						// Get subject as substring
	str = str.erase(0, (int)subject.length() + strlen(MESSAGE_SPLIT_TOKEN));				// Erase subject from string

	std::string text = str;																	// Get text as substring

	printf(CLIENT_DEBUG_NAME"Received mail:\n");
	printf(CLIENT_DEBUG_NAME"\t subject: %s\n", subject.c_str());
	printf(CLIENT_DEBUG_NAME"\t    text: %s\n", text.c_str());

	SetState(ClientState_Connected);
}

void Client::Client_ReceiveMailError() {
	printf(CLIENT_DEBUG_NAME"Error: error while receiving mail! Maybe no mail available. Check with client.CheckMail('username');\n");
	SetState(ClientState_Connected);
}

//////////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// void Login();			// ClientState_Idle			->	ClientState_Connecting
// Send login message with username and password to server via net, change state to connecting to await response
void Client::Login(const char* username, const char* password) {
	if (GetState() != ClientState_Idle) {
		printf(CLIENT_DEBUG_NAME"Error: tried to login while not in idle state!\n");
		return;
	}
	strcpy(buffer, "\0");
	strcat(buffer, LOGIN_COMMAND);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, username);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, password);

	SendBufferToServer();
	SetState(ClientState_Connecting);
}

// void Logout();			// ClientState_Connected	->	ClientState_Disconnecting
void Client::Logout() {
	if(GetState() != ClientState_Connected){
		printf(CLIENT_DEBUG_NAME"Error: tried to logout while not in connected state!\n");
		return;
	}

	if (DEBUG) printf(CLIENT_DEBUG_NAME"Logging out...\n");
	strcpy(buffer, LOGOUT_COMMAND);
	SendBufferToServer();
	SetState(ClientState_Disconnecting);
}

// void SendMail();			// ClientState_Connected	->	ClientState_Connected
void Client::SendMail(const char* username, const char* subject, const char* text) {
	if (GetState() != ClientState_Connected) {
		printf(CLIENT_DEBUG_NAME"Error: tried to send mail while not in connected state!\n");
		return;
	}

	// Fill buffer with command and data
	strcpy(buffer, "\0");
	strcat(buffer, SENDMAIL_COMMAND);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, username);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, subject);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, text);

	// Send request
	SendBufferToServer();
	SetState(ClientState_Connected);
}

// void CheckMail();		// ClientState_Connected	->	ClientState_CheckMail
void Client::CheckMail(const char* username) {
	if (GetState() != ClientState_Connected) {
		printf(CLIENT_DEBUG_NAME"Error: tried to check mail while not in connected state!\n");
		return;
	}

	// Fill buffer with command and data
	strcpy(buffer, "\0");
	strcat(buffer, CHECKMAIL_COMMAND);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, username);

	// Send request
	SendBufferToServer();
	SetState(ClientState_CheckMail);
}

// void ReceiveMail();		// ClientState_Connected	->	ClientState_ReceiveMail
void Client::ReceiveMail(const char* username) {
	if (GetState() != ClientState_Connected) {
		printf(CLIENT_DEBUG_NAME"Error: tried to receive mail while not in connected state!\n");
		return;
	}

	// Fill buffer with command and data
	strcpy(buffer, "\0");
	strcat(buffer, RECEIVEMAIL_COMMAND);
	strcat(buffer, MESSAGE_SPLIT_TOKEN);
	strcat(buffer, username);

	// Send request
	SendBufferToServer();
	SetState(ClientState_ReceiveMail);
}


//////////////////////////////////////////////////////////////////////////////////////
/// NETWORK //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void Client::SendBufferToServer() {
	if (DEBUG) printf(CLIENT_DEBUG_NAME"Sending (%d) '%s' to server...\n", (uint8)buffer[0], buffer);
	sendto(mSocket, buffer, strlen(buffer), 0, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
}

DWORD Client::UDPListener(LPVOID param) {
	Client* pParent = (Client*)param;
	int nReceivedBytes;

	int slen = sizeof(pParent->serverAddress);

	printf(CLIENT_DEBUG_NAME"Client UDP listening on port %d\n", ntohs(pParent->socketAddress.sin_port));

	while (1) {
		memset(pParent->buffer, 0, BUFFER_SIZE);
		nReceivedBytes = recvfrom(pParent->mSocket, pParent->buffer, BUFFER_SIZE, 0, (SOCKADDR*)&pParent->serverAddress, &slen);
		pParent->buffer[nReceivedBytes] = '\0';
		if (nReceivedBytes == 0)
		{
			printf(CLIENT_DEBUG_NAME"Recv 0 bytes!\n");
			break;
		}
		if (nReceivedBytes < 0)
		{
			DWORD err = WSAGetLastError();
			printf(CLIENT_DEBUG_NAME"recieve error: %d\n", err);
			continue;
		}
		pParent->UdpToFsm();
		Sleep(500);
	};

	return 1;
}

void Client::InitSocket() {
	// Create the socket
	mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mSocket == INVALID_SOCKET)
	{
		printf(CLIENT_DEBUG_NAME"socket() failed! Error : %ld\n", WSAGetLastError());
		return;
	}

	// Set server socket address
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	// Bind to port (client port number will be server port num + 1 + clients ID)
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(SERVER_PORT + 1 + GetObject());
	socketAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	if (bind(mSocket, (SOCKADDR*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR)
	{
		printf(CLIENT_DEBUG_NAME"bind() failed! Error : %ld\n", WSAGetLastError());
		closesocket(mSocket);
		return;
	}

	// Then, start the thread that will listen on the the newly created socket
	mhThread = CreateThread(NULL, 0, UDPListener, (LPVOID)this, 0, &mnThreadID);
	if (mhThread == NULL)
	{
		// Cannot create thread
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
		return;
	}
}

void Client::CloseSocket(){
	TerminateThread(mhThread, 0);
	closesocket(mSocket);
	mSocket = INVALID_SOCKET;
}

void Client::UdpToFsm() {
	// Send different messages to client automate based on server responses
	switch (buffer[0]) {
	case ServerMSG_LoginOk:
		printf(CLIENT_DEBUG_NAME"Received 'ServerMSG_LoginOk' from server...\n");
		PrepareNewMessage(0x00, ServerMSG_LoginOk);
		SetMsgToAutomate(CLIENT_TYPE_ID);
		SetMsgObjectNumberTo(GetObject());
		SendMessage(CLIENT_MBX_ID);
		break;
	case ServerMSG_LoginError:
		printf(CLIENT_DEBUG_NAME"Received 'ServerMSG_LoginError' from server...\n");
		PrepareNewMessage(0x00, ServerMSG_LoginError);
		SetMsgToAutomate(CLIENT_TYPE_ID);
		SetMsgObjectNumberTo(GetObject());
		SendMessage(CLIENT_MBX_ID);
		break;
	case ServerMSG_LogoutOk:
		printf(CLIENT_DEBUG_NAME"Received 'ServerMSG_LogoutOk' from server...\n");
		PrepareNewMessage(0x00, ServerMSG_LogoutOk);
		SetMsgToAutomate(CLIENT_TYPE_ID);
		SetMsgObjectNumberTo(GetObject());
		SendMessage(CLIENT_MBX_ID);
		break;
	case ServerMSG_CheckMailResponse:
		printf(CLIENT_DEBUG_NAME"Received 'ServerMSG_CheckMailResponse' from server...\n");
		PrepareNewMessage(0x00, ServerMSG_CheckMailResponse);
		AddParamByte(PARAM_MAIL_COUNT, buffer[1]);
		SetMsgToAutomate(CLIENT_TYPE_ID);
		SetMsgObjectNumberTo(GetObject());
		SendMessage(CLIENT_MBX_ID);
		break;
	case ServerMSG_ReceiveMailError:
		printf(CLIENT_DEBUG_NAME"Received 'ServerMSG_ReceiveMailError' from server...\n");
		PrepareNewMessage(0x00, ServerMSG_ReceiveMailError);
		SetMsgToAutomate(CLIENT_TYPE_ID);
		SetMsgObjectNumberTo(GetObject());
		SendMessage(CLIENT_MBX_ID);
		break;
	case ServerMSG_ReceiveMailResponse:
		printf(CLIENT_DEBUG_NAME"Received 'ServerMSG_ReceiveMailResponse' from server...\n");
		PrepareNewMessage(0x00, ServerMSG_ReceiveMailResponse);
		SetMsgToAutomate(CLIENT_TYPE_ID);
		SetMsgObjectNumberTo(GetObject());
		SendMessage(CLIENT_MBX_ID);
		break;
	default:
		break;
	}
}