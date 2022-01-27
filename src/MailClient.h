#ifndef CLIENT_H
#define CLIENT_H


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
	void	Client_ReceiveMailError();		// -> ClientState_Connected


public:
	// Constructor/destructor
	Client();
	~Client();

	void Initialize();

	// Public functions for client
	void Login(const char* username, const char* password);							// -> ClientState_Connecting
	void Logout();																	// -> ClientState_Disconnecting
	void SendMail(const char* username, const char* subject, const char* text);		// -> ClientState_Connected
	void CheckMail(const char* username);											// -> ClientState_CheckMail
	void ReceiveMail(const char* username);											// -> ClientState_ReceiveMail

protected:
	static DWORD WINAPI UDPListener(LPVOID);

	// Network vars
	SOCKET mSocket;
	SOCKADDR_IN socketAddress;
	SOCKADDR_IN serverAddress;

	// Thread vars
	HANDLE mhThread;
	DWORD mnThreadID;

	// Buffers
	char buffer[BUFFER_SIZE];

	// Network helper functions
	void InitSocket();
	void SendBufferToServer();
	void UdpToFsm();
};


#endif // CLIENT_H