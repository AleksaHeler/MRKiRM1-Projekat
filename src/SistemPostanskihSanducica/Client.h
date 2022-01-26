#pragma once

#include <fsm.h>
#include <fsmsystem.h>
#include <stdio.h>

#include "stdMsgpc16pl16.h"
#include "NetFSM.h"
#include "Const.h"


class Client : public FiniteStateMachine {

	// FSM
	StandardMessage StandardMsgCoding;

	// FSM
	MessageInterface* GetMessageInterface(uint32 id);
	void	SetDefaultHeader(uint8 infoCoding);
	void	SetDefaultFSMData();
	void	NoFreeInstances();
	void	Reset();
	uint8	GetMbxId();
	uint8	GetAutomate();
	uint32	GetObject();
	//void	ResetData();

	// Client states
	enum	States {
		ClientState_Idle,
		ClientState_Connecting,
		ClientState_Connected,
		ClientState_Disconnecting,
		ClientState_CheckMail,
		ClientState_ReceiveMail
	};

	// State: ClientState_Connecting
	void	Client_LoginOK();				// -> ClientState_Connected
	void	Client_LoginError();			// -> ClientState_Idle

	// State: ClientState_Disconnecting
	void	Client_Disconnect();			// -> ClientState_Idle

	// State: ClientState_CheckMail
	void	Client_CheckMailResponse();		// -> ClientState_Connected

	// State: ClientState_ReceiveMail
	void	Client_ReceiveMailResponse();	// -> ClientState_Connected


public:
	Client();
	~Client();

	void Initialize();
	void SendData(uint8 dst, uint8 len, const char* data);

	// Public functions for client
	void Login();			// -> ClientState_Connecting
	void Logout();			// -> ClientState_Disconnecting
	void SendMail();		// -> ClientState_Connected
	void CheckMail();		// -> ClientState_CheckMail
	void ReceiveMail();		// -> ClientState_ReceiveMail

protected:
	static DWORD WINAPI TCPListener(LPVOID);

	SOCKET mSocket;
	SOCKADDR_IN socketAddress;
	SOCKADDR_IN serverAddress;
	HANDLE mhThread;
	DWORD mnThreadID;
	uint16 mnMaxMsgSize;
	char buffer[BUFFER_SIZE];

	void InitSocket();
	void TcpToFsm();
};
