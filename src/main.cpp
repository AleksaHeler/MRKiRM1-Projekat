#include "Const.h"
#include "MailServer.h"
#include "MailClient.h"

// FSM system instance
static FSMSystem sys(2 /* max number of automates types */, 2 /* max number of msg boxes */);
Server server;
Client client;


DWORD WINAPI SystemThread(void* data) {

	// Kernel buffer description block
	const uint8 myBuffClassNo = 4;										// number of buffer types
	uint32 myBuffsCount[myBuffClassNo] = { 50, 50, 5000, 10 };			// number of buffers of each buffer type
	uint32 myBuffsLength[myBuffClassNo] = { 256, 256, 512, 1024 };		// buffer size for each buffer type

	// Logging setting 
	LogFile lf("log.log" /*log file name*/, "./log.ini" /* message translator file */);
	LogAutomateNew::SetLogInterface(&lf);

	// Mandatory kernel initialization 
	printf("[*] Initializing system...\n");
	sys.InitKernel(myBuffClassNo, myBuffsCount, myBuffsLength, 2);

	// Add Server automate to the system 
	sys.Add(&server, SERVER_TYPE_ID, 1, false);

	// Add Client automate to the system
	sys.Add(&client, CLIENT_TYPE_ID, 1, false);

	// Starts the system - dispatches system messages
	printf("[*] Starting system...\n");
	sys.Start();

	return 0;
}

int main(int argc, char** argv) {
	int iResult;
	DWORD thread_id;
	HANDLE thread_handle;
	WSAData wsaData;

	// WSA initialization
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Start main thread
	thread_handle = CreateThread(NULL, 0, SystemThread, NULL, 0, &thread_id);
	Sleep(200);

	/////////////////////////////////////////////////////////////////////////////////////
	/// USER INPUT //////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	printf("Press any key to start testing...\n");
	_getch();
	printf("\n\t\t\t========== Logging in ==========\n");
	client.Login("aleksa", "123456");
	Sleep(2000);
	printf("\n\t\t\t========== Logging out ==========\n");
	client.Logout();
	Sleep(4000);

	printf("\n\t\t\t========== Logging in error ==========\n");
	client.Login("aleksa", "wrongpassword");
	Sleep(2000);

	printf("\n\t\t\t========== Logging in error ==========\n");
	client.Login("petar", "123456");
	Sleep(2000);

	printf("\n\t\t\t========== Try stuff without login (send, check, receive) ==========\n");
	client.SendMail("aleksa", "Hello!", "Hello world!");
	Sleep(2000);
	client.CheckMail("aleksa");
	Sleep(2000);
	client.ReceiveMail("aleksa");
	Sleep(2000);

	printf("\n\t\t\t========== Logging in right this time ==========\n");
	client.Login("aleksa", "123456");
	Sleep(2000);

	printf("\n\t\t\t========== Check mail ==========\n");
	client.CheckMail("aleksa");
	Sleep(2000);

	printf("\n\t\t\t========== Send mail ==========\n");
	client.SendMail("aleksa", "Hello!", "Hello world!");
	Sleep(4000);

	printf("\n\t\t\t========== Check mail ==========\n");
	client.CheckMail("aleksa");
	Sleep(2000);

	printf("\n\t\t\t========== Receive mail ==========\n");
	client.ReceiveMail("aleksa");
	Sleep(2000);

	printf("\n\t\t\t========== Check mail again ==========\n");
	client.CheckMail("aleksa");
	Sleep(2000);

	printf("\n\t\t\t========== Receive mail again ==========\n");
	client.ReceiveMail("aleksa");
	Sleep(2000);

	printf("\n\t\t\t========== Logging out ==========\n");
	client.Logout();
	Sleep(4000);

	// Wait for input to end
	printf("Press any key to stop system...\n");
	_getch();

	// Notify the system to stop - this causes the thread to finish
	printf("[*] Stopping system...\n");
	sys.StopSystem();
	Sleep(5000);

	// Free the thread handle
	CloseHandle(thread_handle);

	WSACleanup();

	return 0;
}