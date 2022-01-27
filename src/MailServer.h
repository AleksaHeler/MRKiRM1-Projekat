#ifndef SERVER_H
#define SERVER_H


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
	//void	ResetData();

	// Server states
	enum	States {
		ServerState_Idle,
		ServerState_Connected
	};

	// Allowed users and their passwords
	int numberOfUsernames = 1;
	char usernames[1][MAX_STRING_SIZE] = {
		"aleksa"
	};
	char passwords[1][MAX_STRING_SIZE] = {
		"123456"
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

	SOCKET mSocket;
	SOCKADDR_IN socketAddress;
	HANDLE mhThread;
	DWORD mnThreadID;
	char buffer[BUFFER_SIZE];
	char username[MAX_STRING_SIZE];
	char password[MAX_STRING_SIZE];

	void InitSocket();
	void SendBufferToClient();
	void UdpToFsm();
};


#endif // SERVER_H