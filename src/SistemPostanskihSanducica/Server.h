#pragma once

#include <fsm.h>
#include <fsmsystem.h>
#include <stdio.h>

#include "stdMsgpc16pl16.h"
#include "NetFSM.h"
#include "Const.h"


class Server : public FiniteStateMachine {

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
	void	ResetData();

	// Server states
	enum	States {
		ServerState_Idle,
		ServerState_Connected
	};

	// State: Server_Idle
	void	Server_Login();				// -> Server_Connected

	// State: Server_Connected
	void	Server_Logout();			// -> Server_Idle
	void	Server_SendMail();			// -> Server_Connected
	void	Server_CheckMail();			// -> Server_Connected
	void	Server_ReceiveMail();		// -> Server_Connected


public:
	Server();
	~Server();

	void Initialize();

protected:
	static DWORD WINAPI TCPListener(LPVOID);

	SOCKET m_Socket;
	SOCKADDR_IN socketAddress;
	HANDLE m_hThread;
	DWORD m_nThreadID;
	uint16 m_nMaxMsgSize;

	void InitSocket();
	void TcpToFsm(u_short port);
};
