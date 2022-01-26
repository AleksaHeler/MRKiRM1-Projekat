#include "Server.h"

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
	SetState(ServerState_Connected);
}

// ServerState_Connected -> ServerState_Idle
void Server::Server_Logout() {
	// TODO: Just transition to idle state and reset things
	SetState(ServerState_Idle);
}

// ServerState_Connected -> ServerState_Connected
void Server::Server_SendMail() {
	// TODO: Check parameters - it contains all the info of message to be sent.
	//       Store message where its headed
	SetState(ServerState_Connected);
}

// ServerState_Connected -> ServerState_Connected
void Server::Server_CheckMail() {
	// TODO: Check parameters for user info. If there is mail for the user get number of mails
	//       waiting and send it back as parameter for message ServerMSG_CheckMailResponse
	SetState(ServerState_Connected);
}

// ServerState_Connected -> ServerState_Connected
void Server::Server_ReceiveMail() {
	// TODO: Check parameters for user info. If there is mail for the user try and get a mail
	//       waiting and send it back as parameter for message ServerMSG_ReceiveMailResponse.
	//       If no mail is waiting, send error as parameter to message
	SetState(ServerState_Connected);
}


//////////////////////////////////////////////////////////////////////////////////////
/// NETWORK //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
 
// TODO: TCPListener(LPVOID)
// TODO: InitSocket()
// TODO: TcpToFsm(u_short port)