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

	// Server states
	enum	States {
		ServerState_Idle,
		ServerState_Connected
	};

	// Allowed users and their passwords
	static const int numberOfUsernames = 1;
	char usernames[1][MAX_STRING_SIZE] = {
		"aleksa"
	};
	char passwords[1][MAX_STRING_SIZE] = {
		"123456"
	};

	// User message boxes (for each user, an array of strings)
	int userMailCount[numberOfUsernames];
	char userMessageBoxSubjects[numberOfUsernames][MAX_MESSAGES_IN_INBOX][MAX_STRING_SIZE];
	char userMessageBoxTexts[numberOfUsernames][MAX_MESSAGES_IN_INBOX][MAX_MAIL_SIZE];

	// State: Server_Idle
	void	Server_Login();				// -> Server_Connected

	// State: Server_Connected
	void	Server_Logout();			// -> Server_Idle
	void	Server_SendMail();			// -> Server_Connected
	void	Server_CheckMail();			// -> Server_Connected
	void	Server_ReceiveMail();		// -> Server_Connected


public:
	// Constructor/destructor
	Server();
	~Server();

	void Initialize();

protected:
	static DWORD WINAPI UDPListener(LPVOID);

	// Network vars
	SOCKET mSocket;
	SOCKADDR_IN socketAddress;

	// Thread vars
	HANDLE mhThread;
	DWORD mnThreadID;

	// Buffers
	char buffer[BUFFER_SIZE];
	char username[MAX_STRING_SIZE];
	char password[MAX_STRING_SIZE];
	char messageSubject[MAX_STRING_SIZE];
	char messageText[MAX_MAIL_SIZE];

	// Network helper functions
	void InitSocket();
	void CloseSocket();
	void SendBufferToClient();
	void UdpToFsm();
};


#endif // SERVER_H