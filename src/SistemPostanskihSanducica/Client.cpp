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

// TODO: TCPListener(LPVOID)
// TODO: InitSocket()
// TODO: TcpToFsm(u_short port)