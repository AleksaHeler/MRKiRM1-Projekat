#include "MailServer.h"

#define StandardMessageCoding 0x00


//////////////////////////////////////////////////////////////////////////////////////
/// FINITE STATE MACHINE /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// Constructor/destructor
Server::Server() : FiniteStateMachine(SERVER_TYPE_ID, SERVER_MBX_ID, 0, 2, 4) {}
Server::~Server() {}

// Getters
uint8 Server::GetAutomate() { return SERVER_TYPE_ID; }
uint8 Server::GetMbxId() { return SERVER_MBX_ID; }
uint32 Server::GetObject() { return GetObjectId(); }
MessageInterface* Server::GetMessageInterface(uint32 id) { return &StandardMsgCoding; }

// Setters
void Server::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}
void Server::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}
void Server::NoFreeInstances() {
	printf("[%d] Server::NoFreeInstances()\n", GetObjectId());
}
void Server::Reset() {
	printf("[%d] Server::Reset()\n", GetObjectId());
}


//////////////////////////////////////////////////////////////////////////////////////
/// INITIALIZATIONS //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void Server::Initialize() {
	SetState(ServerState_Idle);
	InitSocket();

	// Set state transition functions and triggers (messages)
	InitEventProc(ServerState_Idle, ClientMSG_Login, (PROC_FUN_PTR)&Server::Server_Login);
	InitEventProc(ServerState_Connected, ClientMSG_Logout, (PROC_FUN_PTR)&Server::Server_Logout);
	InitEventProc(ServerState_Connected, ClientMSG_SendMail, (PROC_FUN_PTR)&Server::Server_SendMail);
	InitEventProc(ServerState_Connected, ClientMSG_CheckMail, (PROC_FUN_PTR)&Server::Server_CheckMail);
	InitEventProc(ServerState_Connected, ClientMSG_ReceiveMail, (PROC_FUN_PTR)&Server::Server_ReceiveMail);
}


//////////////////////////////////////////////////////////////////////////////////////
/// STATE TRANSITION FUNCTIONS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


// ServerState_Idle -> ServerState_Connected
void Server::Server_Login() {
	// TODO: Check parameters for user info. If all ok send Ok
	//       response, else Error response and stay in idle
	printf(SERVER_DEBUG_NAME"Logging in { %s, %s }...\n", username, password);

	// TODO: login check credentials...
	bool loginOk = false;
	for (int i = 0; i < numberOfUsernames; i++) {
		if (strcmp(usernames[i], username) == 0) {
			printf(SERVER_DEBUG_NAME"Found user\n");
			if (strcmp(passwords[i], password) == 0) {
				printf(SERVER_DEBUG_NAME"Correct password\n");
				loginOk = true;
			}
			else {
				printf(SERVER_DEBUG_NAME"Error: incorrect password\n");
			}
		}
	}

	if (loginOk) buffer[0] = ServerMSG_LoginOk;
	else buffer[0] = ServerMSG_LoginError;
	buffer[1] = '\0';
	SendBufferToClient();

	if (loginOk) SetState(ServerState_Connected);
	else SetState(ServerState_Idle);
	
}

// ServerState_Connected -> ServerState_Idle
void Server::Server_Logout() {
	// TODO: Just transition to idle state and reset things
	printf(SERVER_DEBUG_NAME"Logging out...\n");
	buffer[0] = ServerMSG_LogoutOk;
	buffer[1] = '\0';
	SendBufferToClient();
	SetState(ServerState_Idle);
}

// ServerState_Connected -> ServerState_Connected
void Server::Server_SendMail() {
	// TODO: Check parameters - it contains all the info of message to be sent.
	//       Store message where its headed
	printf(SERVER_DEBUG_NAME"TODO: Server_SendMail\n");
	SetState(ServerState_Connected);
}

// ServerState_Connected -> ServerState_Connected
void Server::Server_CheckMail() {
	// TODO: Check parameters for user info. If there is mail for the user get number of mails
	//       waiting and send it back as parameter for message ServerMSG_CheckMailResponse
	printf(SERVER_DEBUG_NAME"TODO: Server_CheckMail\n");
	SetState(ServerState_Connected);
}

// ServerState_Connected -> ServerState_Connected
void Server::Server_ReceiveMail() {
	// TODO: Check parameters for user info. If there is mail for the user try and get a mail
	//       waiting and send it back as parameter for message ServerMSG_ReceiveMailResponse.
	//       If no mail is waiting, send error as parameter to message
	printf(SERVER_DEBUG_NAME"TODO: Server_ReceiveMail\n");
	SetState(ServerState_Connected);
}


//////////////////////////////////////////////////////////////////////////////////////
/// NETWORK //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void Server::SendBufferToClient() {
	SOCKADDR_IN destAddress;
	destAddress.sin_family = AF_INET;
	destAddress.sin_port = htons(SERVER_PORT + 1);
	destAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
	if (DEBUG) printf("[Server] Sending (%d) '%s' to client...\n", (uint8)buffer[0], buffer);
	sendto(mSocket, buffer, strlen(buffer), 0, (SOCKADDR*)&destAddress, sizeof(destAddress));
}

DWORD Server::TCPListener(LPVOID param) {
	Server* pParent = (Server*)param;
	int nReceivedBytes;

	printf(SERVER_DEBUG_NAME"Server UDP listening on port %d\n", ntohs(pParent->socketAddress.sin_port));

	SOCKADDR_IN recvAddress;
	int recvAddressLen = sizeof(recvAddress);

	while (1) {
		memset(pParent->buffer, 0, BUFFER_SIZE);
		nReceivedBytes = recvfrom(pParent->mSocket, pParent->buffer, BUFFER_SIZE, 0, (SOCKADDR*)&recvAddress, &recvAddressLen);
		pParent->buffer[nReceivedBytes] = '\0';
		if (nReceivedBytes == 0) {
			break;
		}
		if (nReceivedBytes < 0)
		{
			DWORD err = WSAGetLastError();
			printf(SERVER_DEBUG_NAME"recieve error: %d\n", err);
			continue;
		}
		pParent->UdpToFsm();
		Sleep(500);
	}

	return 1;
}

void Server::InitSocket() {
	// Create the socket
	mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mSocket == INVALID_SOCKET)
	{
		printf(SERVER_DEBUG_NAME"socket() failed! Error : %ld\n", WSAGetLastError());
		return;
	}

	// Bind to port
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(SERVER_PORT);
	socketAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

	if (bind(mSocket, (SOCKADDR*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR)
	{
		printf(SERVER_DEBUG_NAME"bind() failed! Error : %ld\n", WSAGetLastError());
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

void Server::UdpToFsm() {
	//if (DEBUG) printf(SERVER_DEBUG_NAME"Received (%d) '%s' from client...\n", (uint8)buffer[0], buffer);
	char tempBuff[BUFFER_SIZE];

	// TODO: logic on receiving (send correct message with parameters to server automate)

	// If login: parse data and send message to server automate
	strcpy(tempBuff, buffer);
	tempBuff[strlen(LOGIN_COMMAND)] = '\0';
	if (strcmp(tempBuff, LOGIN_COMMAND) == 0) {
		strcpy(tempBuff, buffer);								// Create local copy of buffer
		std::string str(tempBuff);								// Create string from array
		str = str.erase(0, strlen(LOGIN_COMMAND) + 1);			// Erase login command text (left with username and password)
		std::string user = str.substr(0, str.find(" "));		// Get username as substring
		str = str.erase(0, (int)user.length() + 1);				// Erase username from text
		std::string pass = str;									// Get password as remainder

		strcpy(username, user.c_str());
		strcpy(password, pass.c_str());

		if (DEBUG) printf(SERVER_DEBUG_NAME"Received '%s' from client\n", buffer);
		if (DEBUG) printf(SERVER_DEBUG_NAME"Extracted { user:%s, pass:%s }\n", username, password);

		PrepareNewMessage(0x00, ClientMSG_Login);
		SetMsgToAutomate(SERVER_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(SERVER_MBX_ID);
	}

	// If logout: send message to server automate to log out
	strcpy(tempBuff, buffer);
	tempBuff[strlen(LOGOUT_COMMAND)] = '\0';
	if (strcmp(tempBuff, LOGOUT_COMMAND) == 0) {
		PrepareNewMessage(0x00, ClientMSG_Logout);
		SetMsgToAutomate(SERVER_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(SERVER_MBX_ID);
	}

	
	//PrepareNewMessage(0x00, ClientMSG_Login);
	//SetMsgToAutomate(SERVER_TYPE_ID);
	//SetMsgObjectNumberTo(0);
	//AddParam(PARAM_USERNAME, buffer);
	//SendMessage(SERVER_MBX_ID);
}