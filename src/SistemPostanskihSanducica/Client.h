#pragma once

#include <fsm.h>
#include <fsmsystem.h>

#include "../kernel/stdMsgpc16pl16.h"
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
	void	ResetData();

	// Client states
	enum	States {
		Client_Idle,
		Client_Connecting,
		Client_Connected,
		Client_Disconnecting,
		Client_CheckMail,
		Client_ReceiveMail
	};

	// State: Client_Idle
	void	Client_Login();					// -> Client_Connecting

	// State: Client_Connecting
	void	Client_LoginOK();				// -> Client_Connected
	void	Client_LoginError();			// -> Client_Idle

	// State: Client_Connected
	void	Client_Logout();				// -> Client_Disconnecting
	void	Client_SendMail();				// -> Client_Connected
	void	Client_CheckMail();				// -> Client_CheckMail
	void	Client_ReceiveMail();			// -> Client_ReceiveMail

	// State: Client_Disconnecting
	void	Client_Disconnect();			// -> Client_Idle

	// State: Client_CheckMail
	void	Client_CheckMailResponse();		// -> Client_Connected

	// State: Client_ReceiveMail
	void	Client_ReceiveMailResponse();	// -> Client_Connected


public:
	Client();
	~Client();

	void Initialize();
	void SendData(uint8 dst, uint8 len, const char* data);

protected:
	static DWORD WINAPI TCPListener(LPVOID);

	SOCKET m_Socket;
	SOCKADDR_IN socketAddress;
	SOCKADDR_IN serverAddress;
	HANDLE m_hThread;
	DWORD m_nThreadID;
	uint16 m_nMaxMsgSize;

	void InitSocket();
	void TcpToFsm();
};
